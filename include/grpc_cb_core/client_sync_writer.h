// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_SYNC_WRITER_H
#define GRPC_CB_CORE_CLIENT_SYNC_WRITER_H

#include <cassert>  // for assert()
#include <cstdint>  // for int64_t
#include <string>

#include <grpc_cb_core/channel.h>                           // for MakeSharedCall()
#include <grpc_cb_core/impl/call_sptr.h>                    // for CallSptr
#include <grpc_cb_core/impl/client/client_send_init_md_cqtag.h>  // for ClientSendInitMdCqTag
#include <grpc_cb_core/impl/client/client_sync_writer_helper.h>  // for ClientSyncWriterHelper
#include <grpc_cb_core/impl/client/client_writer_close_cqtag.h>  // for ClientWriterCloseCqTag
#include <grpc_cb_core/impl/cqueue_for_pluck_sptr.h>             // for CQueueForPluckSptr
#include <grpc_cb_core/status.h>                                 // for Status

namespace grpc_cb_core {

// Copyable.
class ClientSyncWriter GRPC_FINAL {
 public:
  inline ClientSyncWriter(const ChannelSptr& channel,
                          const std::string& method,
                          int64_t timeout_ms);

  // Todo: SyncGetInitMd();
  bool Write(const std::string& request) const {
    Data& d = *data_sptr_;
    return ClientSyncWriterHelper::SyncWrite(d.call_sptr, d.cq4p_sptr,
                                             request, d.status);
  }

  Status Close(std::string* response) const;

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CQueueForPluckSptr cq4p_sptr;
    CallSptr call_sptr;
    Status status;
  };
  using DataSptr = std::shared_ptr<Data>;

  DataSptr data_sptr_;  // Easy to copy.
};  // class ClientSyncWriter<>

ClientSyncWriter::ClientSyncWriter(const ChannelSptr& channel,
                                   const std::string& method,
                                   int64_t timeout_ms)
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

  data_sptr_->status.SetInternalError("Failed to start client sync writer.");
}

Status ClientSyncWriter::Close(std::string* response) const {
  assert(response);
  assert(data_sptr_);
  Data& data = *data_sptr_;
  assert(data.call_sptr);
  assert(data.cq4p_sptr);

  Status& status = data.status;
  if (!status.ok()) return status;
  ClientWriterCloseCqTag tag(data.call_sptr);
  if (!tag.Start()) {
    status.SetInternalError("Failed to finish client stream.");
    return status;
  }

  data.cq4p_sptr->Pluck(&tag);

  // Todo: Get trailing metadata.
  if (tag.IsStatusOk())
    status = tag.GetResponse(*response);
  else
    status = tag.GetStatus();

  return status;
}  // Close()

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_SYNC_WRITER_H
