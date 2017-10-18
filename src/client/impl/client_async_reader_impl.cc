// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_impl.h"

#include <cassert>  // for assert()

#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()
#include "client_async_read_worker.h"     // for ClientAsyncReadWorker
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
  if (reading_started_)
    return;  // already started

  status_cb_ = status_cb;

  // Impl and Worker will share each other until the end of reading.
  auto sptr = shared_from_this();
  auto reader_sptr = std::make_shared<ClientAsyncReadWorker>(
      call_sptr_, msg_cb,
      [sptr]() {
        sptr->OnEndOfReading();  // will clear this function, XXX
        // sptr is invalid now  XXX
      });
  reader_sptr->Start();
  reader_wptr_ = reader_sptr;
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
  assert(reading_started_);
  if (reading_ended_) return;
  reading_ended_ = true;

  if (!status_.ok()) return;
  auto reader_sptr = reader_wptr_.lock();
  assert(reader_sptr);
  status_ = reader_sptr->GetStatus();
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
