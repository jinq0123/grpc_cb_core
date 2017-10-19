// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_impl.h"

#include <cassert>  // for assert()

#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()
#include "client_reader_async_recv_status_cqtag.h"  // for ClientReaderAsyncRecvStatusCqTag
#include "client_reader_init_cqtag.h"               // for ClientReaderInitCqTag
#include "client_reader_read_cqtag.h"               // for ClientReaderReadCqTag

namespace grpc_cb_core {

// XXX move request to Start()?

ClientAsyncReaderImpl::ClientAsyncReaderImpl(
    const ChannelSptr& channel, const std::string& method,
    const std::string& request, const CompletionQueueSptr& cq_sptr,
    int64_t timeout_ms)
    : call_sptr_(channel->MakeSharedCall(method, *cq_sptr, timeout_ms)),
      request_(request) {
  assert(cq_sptr);
  assert(channel);
  assert(call_sptr_);
}

ClientAsyncReaderImpl::~ClientAsyncReaderImpl() {}

void ClientAsyncReaderImpl::Start(const MsgStrCb& msg_cb/* = nullptr*/,
    const StatusCb& status_cb/* = nullptr*/) {
  Guard g(mtx_);
  if (is_started_)
    return;  // already started
  is_started_ = true;
  msg_cb_ = msg_cb;
  status_cb_ = status_cb;

  assert(status_.ok());
  if (Init()) ReadNext();
}  // Start()

static void RecvStatus(const CallSptr& call_sptr, const StatusCb& status_cb) {
  assert(call_sptr);

  auto* tag = new ClientReaderAsyncRecvStatusCqTag(call_sptr);
  tag->SetStatusCb(status_cb);
  if (tag->Start()) return;

  delete tag;
  if (status_cb)
    status_cb(Status::InternalError("Failed to receive status."));
}  // RecvStatus()

void ClientAsyncReaderImpl::OnRead(bool success, ClientReaderReadCqTag& tag) {
  Guard g(mtx_);  // Callback need guard.
  assert(is_started_);
  assert(status_.ok());
  if (!success) {
    status_.SetInternalError("ClientReaderReadCqTag failed.");
    CallStatusCb();
    return;
  }
  if (!tag.HasGotMsg()) {
    RecvStatus(call_sptr_, status_cb_);  // run cb on recv
    return;
  }

  std::string sMsg;
  status_ = tag.GetResultMsg(sMsg);
  if (!status_.ok()) {
    CallStatusCb();
    return;
  }

  if (msg_cb_) {
    status_ = msg_cb_(sMsg);
    if (!status_.ok()) {
      CallStatusCb();
      return;
    }
  }

  ReadNext();
}  // OnRead()

bool ClientAsyncReaderImpl::Init() {
  // private function has no Guard
  assert(status_.ok());
  auto* tag = new ClientReaderInitCqTag(call_sptr_);
  if (tag->Start(request_))
    return true;

  delete tag;
  status_.SetInternalError("Failed to start async client reader.");
  CallStatusCb();  // must after Start()
  return false;
}  // Init()

void ClientAsyncReaderImpl::ReadNext() {
  // private function has no Guard
  assert(status_.ok());
  auto* tag = new ClientReaderReadCqTag(call_sptr_);
  auto sptr = shared_from_this();  // shared in CqTag
  tag->SetCompleteCb([sptr, tag](bool success) {
      sptr->OnRead(success, *tag);
  });
  if (tag->Start()) return;

  delete tag;
  status_.SetInternalError("Failed to async read server stream.");
  CallStatusCb();
}  // ReadNext()

void ClientAsyncReaderImpl::CallStatusCb() {
  // private function has no Guard
  if (!status_cb_) return;
  status_cb_(status_);
  status_cb_ = nullptr;
}

}  // namespace grpc_cb_core
