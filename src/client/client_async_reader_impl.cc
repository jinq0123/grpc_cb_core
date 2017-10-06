// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/client/impl/client_async_reader_impl.h>

#include <cassert>     // for assert()

#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()
#include <grpc_cb_core/client/impl/client_reader_init_cqtag.h>  // for ClientReaderInitCqTag

#include "client_async_reader_helper.h"
#include "client_async_reader_recv_status.h"

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
  // Todo: move ClientReaderInitCqTag to src/
  ClientReaderInitCqTag* tag = new ClientReaderInitCqTag(call_sptr_);
  if (tag->Start(request))
    return;

  delete tag;
  status_.SetInternalError("Failed to start async client reader.");
  CallStatusCb();
}

ClientAsyncReaderImpl::~ClientAsyncReaderImpl() {}

void ClientAsyncReaderImpl::SetReadHandler(
    const ClientAsyncReadHandlerSptr& handler) {
  Guard g(mtx_);
  read_handler_sptr_ = handler;
}

void ClientAsyncReaderImpl::SetStatusCb(const StatusCb& status_cb) {
  Guard g(mtx_);
  if (set_status_cb_once_) return;
  set_status_cb_once_ = true;
  status_cb_ = status_cb;
  if (!status_.ok())
    CallStatusCb();
}

void ClientAsyncReaderImpl::Start() {
  Guard g(mtx_);
  if (reading_started_) return;
  reading_started_ = true;
  assert(!reader_sptr_);

  // Impl and Helper will share each other until the end of reading.
  auto sptr = shared_from_this();
  reader_sptr_.reset(new ClientAsyncReaderHelper(
      call_sptr_, read_handler_sptr_, [sptr]() { sptr->OnEndOfReading(); }));
  reader_sptr_->Start();
}

void ClientAsyncReaderImpl::OnEndOfReading() {
  Guard g(mtx_);
  assert(reading_started_);

  if (!reader_sptr_) return;
  Status r_status(reader_sptr_->GetStatus());  // Copy before reset()
  reader_sptr_.reset();  // Stop circular sharing.

  if (!status_.ok()) return;
  status_ = r_status;
  if (status_.ok()) {
    ClientAsyncReader::RecvStatus(call_sptr_, status_cb_);
    return;
  }

  CallStatusCb();
}

void ClientAsyncReaderImpl::CallStatusCb() {
  if (!status_cb_) return;
  status_cb_(status_);
  status_cb_ = StatusCb();
}

}  // namespace grpc_cb_core
