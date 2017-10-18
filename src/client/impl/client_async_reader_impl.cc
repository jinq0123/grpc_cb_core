// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_impl.h"

#include <cassert>  // for assert()

#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()
#include "client_async_read_worker.h"
#include "client_reader_async_recv_status_cqtag.h"  // for ClientReaderAsyncRecvStatusCqTag
#include "client_reader_init_cqtag.h"               // for ClientReaderInitCqTag

namespace grpc_cb_core {

ClientAsyncReaderImpl::ClientAsyncReaderImpl(
    const ChannelSptr& channel, const std::string& method,
    const std::string& request, const CompletionQueueSptr& cq_sptr,
    int64_t timeout_ms)
    : call_sptr_(channel->MakeSharedCall(method, *cq_sptr, timeout_ms)) {
  assert(cq_sptr);
  assert(channel);
  assert(call_sptr_);

  // Todo: Move to Start()
  ClientReaderInitCqTag* tag = new ClientReaderInitCqTag(call_sptr_);
  if (tag->Start(request))
    return;

  delete tag;
  status_.SetInternalError("Failed to start async client reader.");
  CallStatusCb();
}

ClientAsyncReaderImpl::~ClientAsyncReaderImpl() {}

void ClientAsyncReaderImpl::Start(const MsgStrCb& msg_cb/* = nullptr*/,
    const StatusCb& status_cb/* = nullptr*/) {
  Guard g(mtx_);
  if (reader_sptr_)
    return;  // already started

  status_cb_ = status_cb;

  // Impl and Helper will share each other until the end of reading.
  auto sptr = shared_from_this();
  reader_sptr_.reset(new ClientAsyncReaderHelper(call_sptr_, msg_cb,
      [sptr]() {
        auto p2 = sptr;
        p2->OnEndOfReading();  // will clear this function
        // sptr is invalid now
      }));
  reader_sptr_->Start();
}

static void RecvStatus(const CallSptr& call_sptr, const StatusCb& status_cb) {
  assert(call_sptr);

  auto* tag = new ClientReaderAsyncRecvStatusCqTag(call_sptr);
  tag->SetStatusCb(status_cb);
  if (tag->Start()) return;

  delete tag;
  if (status_cb)
    status_cb(Status::InternalError("Failed to receive status."));
}

void ClientAsyncReaderImpl::OnEndOfReading() {
  Guard g(mtx_);
  assert(reader_sptr_);
  if (reading_ended_) return;
  reading_ended_ = true;
  reader_sptr_->Abort();  // Stop circular sharing.

  if (!status_.ok()) return;
  status_ = reader_sptr_->GetStatus();
  if (status_.ok()) {
    RecvStatus(call_sptr_, status_cb_);  // run cb on recv
    return;
  }

  CallStatusCb();
}

void ClientAsyncReaderImpl::CallStatusCb() {
  if (!status_cb_) return;
  status_cb_(status_);
  status_cb_ = nullptr;
}

}  // namespace grpc_cb_core
