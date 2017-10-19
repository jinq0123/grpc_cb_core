// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_SYNC_READER_WRITER_IMPL_H
#define GRPC_CB_CORE_CLIENT_CLIENT_SYNC_READER_WRITER_IMPL_H

#include <cstdint>  // for int64_t
#include <string>

#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()
#include "client_recv_init_md_cqtag.h"  // for ClientRecvInitMdCqTag
#include "client_send_close_cqtag.h"  // for ClientSendCloseCqTag
#include "client_send_init_md_cqtag.h"  // for ClientSendInitMdCqTag
#include "client_sync_reader_data.h"  // for ClientSyncReaderDataSptr
#include "client_sync_reader_helper.h"  // for ClientSyncReaderHelper
#include "client_sync_writer_helper.h"  // for ClientSyncWriterHelper
#include "common/impl/cqueue_for_pluck.h"  // for CQueueForPluck
#include <grpc_cb_core/common/status.h>                 // for Status
#include <grpc_cb_core/common/support/config.h>         // for GRPC_FINAL

namespace grpc_cb_core {

// XXX thread-safe?

class ClientSyncReaderWriterImpl GRPC_FINAL {
 public:
  using string = std::string;
  inline ClientSyncReaderWriterImpl(const ChannelSptr& channel,
                                    const string& method,
                                    int64_t timeout_ms);
  inline ~ClientSyncReaderWriterImpl();

 public:
  inline bool Write(const string& request) const;
  // Optional. Auto called in dtr().
  inline void CloseWriting();

  inline bool ReadOne(string* response);
  inline Status RecvStatus() const {
    const Data& d = *data_sptr_;
    if (!d.status.ok()) return d.status;
    return ClientSyncReaderHelper::SyncRecvStatus(d.call_sptr, d.cq4p_sptr);
  }

  // Set error status to break reading. Such as when parsing message failed.
  void SetErrorStatus(const Status& error_status) {
    assert(!error_status.ok());
    data_sptr_->status = error_status;
  }

 private:
  inline void RecvInitMdIfNot();

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  using Data = ClientSyncReaderData;
  using DataSptr = std::shared_ptr<Data>;
  DataSptr data_sptr_;  // Same as reader. Easy to copy.
  bool writing_closed_ = false;  // Is SyncCloseWriting() called?
  bool init_md_received_ = false;  // to receive init metadata once
};  // class ClientSyncReaderWriterImpl

// Todo: SyncGetInitMd();

ClientSyncReaderWriterImpl::ClientSyncReaderWriterImpl(
    const ChannelSptr& channel, const string& method, int64_t timeout_ms)
    // Todo: same as ClientReader?
    : data_sptr_(new Data) {
  assert(channel);
  CQueueForPluckSptr cq4p_sptr(new CQueueForPluck);
  CallSptr call_sptr = channel->MakeSharedCall(method, *cq4p_sptr, timeout_ms);
  data_sptr_->cq4p_sptr = cq4p_sptr;
  data_sptr_->call_sptr = call_sptr;
  ClientSendInitMdCqTag tag(call_sptr);
  if (tag.Start()) {
    cq4p_sptr->Pluck(&tag);
    return;
  }
  data_sptr_->status.SetInternalError("Failed to start client sync reader writer.");
}

ClientSyncReaderWriterImpl::~ClientSyncReaderWriterImpl() {
  CloseWriting();
}

bool ClientSyncReaderWriterImpl::Write(const string& request) const {
  assert(data_sptr_);
  Data& d = *data_sptr_;
  assert(d.call_sptr);
  return ClientSyncWriterHelper::SyncWrite(d.call_sptr, d.cq4p_sptr, request,
                                               d.status);
}

void ClientSyncReaderWriterImpl::CloseWriting() {
  if (writing_closed_) return;
  writing_closed_ = true;
  Status& status = data_sptr_->status;
  if (!status.ok()) return;

  ClientSendCloseCqTag tag(data_sptr_->call_sptr);
  if (tag.Start()) {
    data_sptr_->cq4p_sptr->Pluck(&tag);
    return;
  }

  status.SetInternalError("Failed to close writing.");
}

// Todo: same as ClientReader?
bool ClientSyncReaderWriterImpl::ReadOne(string* response) {
  assert(response);
  Data& d = *data_sptr_;
  Status& status = d.status;
  if (!status.ok()) return false;
  RecvInitMdIfNot();
  if (!status.ok()) return false;
  return ClientSyncReaderHelper::SyncReadOne(
      d.call_sptr, d.cq4p_sptr, *response, status);
}

void ClientSyncReaderWriterImpl::RecvInitMdIfNot() {
  if (init_md_received_) return;
  init_md_received_ = true;
  Status& status = data_sptr_->status;
  assert(status.ok());

  ClientRecvInitMdCqTag tag(data_sptr_->call_sptr);
  if (tag.Start()) {
    data_sptr_->cq4p_sptr->Pluck(&tag);
    return;
  }
  status.SetInternalError("Failed to receive init metadata.");
}

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_CLIENT_SYNC_READER_WRITER_IMPL_H
