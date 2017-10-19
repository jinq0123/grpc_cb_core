// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_impl2.h"

#include <cassert>  // for assert()

#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()

#include "common/impl/complete_cb.h"          // for CompleteCb
#include "cqtag/client_send_init_md_cqtag.h"  // for ClientSendInitMdCqTag
#include "cqtag/client_send_msg_cqtag.h"      // for ClientSendMsgCqTag
#include "cqtag/client_writer_close_cqtag.h"  // for ClientWriterCloseCqTag

namespace grpc_cb_core {

ClientAsyncWriterImpl::ClientAsyncWriterImpl(
    const ChannelSptr& channel, const std::string& method,
    const CompletionQueueSptr& cq_sptr, int64_t timeout_ms)
    : call_sptr_(channel->MakeSharedCall(method, *cq_sptr, timeout_ms)) {
  assert(cq_sptr);
  assert(channel);
  assert(call_sptr_);

  ClientSendInitMdCqTag* tag = new ClientSendInitMdCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  status_.SetInternalError("Failed to init client-side streaming.");
  assert(!is_closing_);  // Will CallCloseCb() after Close(close_cb).
}

ClientAsyncWriterImpl::~ClientAsyncWriterImpl() {
  // Have done CallCloseCb().
}

// Queue messages -> Send messages one by one
// Close writing -> Send messages -> Send close

bool ClientAsyncWriterImpl::Write(const std::string& request) {
  Guard g(mtx_);
  if (!status_.ok())
    return false;
  if (is_closing_)
    return false;  // Ignore any messages after Close().

  bool is_sending = !msg_queue_.empty();  // Is sending the front msg?
  msg_queue_.push(request);
  if (is_sending) return true;
  return TryToSendNext();  // Resume sending.
}  // Write()

void ClientAsyncWriterImpl::Close(const CloseCb& close_cb/* = nullptr*/) {
  Guard g(mtx_);
  if (is_closing_) return;  // already done
  is_closing_ = true;
  close_cb_ = close_cb;  // reset in CallCloseCb()

  if (!status_.ok()) {
    CallCloseCb();
    return;
  }

  if (msg_queue_.empty())  // All messages are sent.
    SendClose();
}  // Close()

void ClientAsyncWriterImpl::OnSent(bool success) {
  Guard g(mtx_);  // Callback needs Guard.
  assert(!msg_queue_.empty());
  msg_queue_.pop();  // The front is sent.

  if (!status_.ok()) {
    assert(!close_cb_);  // Already reset in CallCloseCb() or not set yet.
    return;
  }
  if (!success) {
    status_.SetInternalError("Failed to send message.");
    TryToCallCloseCb();  // error end
    return;
  }

  if (!msg_queue_.empty()) {
    TryToSendNext();
    return;
  }

  // All messages are sent. Wait for Close()
  if (is_closing_)
    SendClose();  // normal end
}  // OnSent()

// Finally close...
void ClientAsyncWriterImpl::SendClose() {
  // private function need no Guard.
  assert(is_closing_);  // Must after Close().
  assert(status_.ok());
  assert(!has_sent_close_);
  assert((has_sent_close_ = true));

  auto sptr = shared_from_this();
  auto* tag = new ClientWriterCloseCqTag(call_sptr_);
  tag->SetCompleteCb([sptr, tag](bool success) {
    sptr->OnClosed(success, *tag);
  });
  if (tag->Start())
    return;

  delete tag;
  status_.SetInternalError("Failed to close client-side streaming.");
  CallCloseCb();
}  // SendClose()

void ClientAsyncWriterImpl::TryToCallCloseCb() {
  // private function need no Guard.
  assert(!status_.ok());  // always on error
  if (is_closing_)
    CallCloseCb();
}

void ClientAsyncWriterImpl::CallCloseCb(const std::string& sMsg/* = ""*/) {
  // private function need no Guard.
  if (!close_cb_) return;
  close_cb_(status_, sMsg);
  close_cb_ = nullptr;  // called once
}

// Callback of ClientWriterCloseCqTag::OnComplete()
void ClientAsyncWriterImpl::OnClosed(bool success, ClientWriterCloseCqTag& tag) {
  Guard g(mtx_);  // Callback need Guard.

  if (!status_.ok()) {
    assert(!close_cb_);  // Already called and reset.
    return;
  }

  if (!tag.IsStatusOk()) {
    status_ = tag.GetStatus();
    CallCloseCb();
    return;
  }

  // Todo: Get trailing metadata.
  std::string sMsg;
  status_ = tag.GetResponse(sMsg);
  CallCloseCb(sMsg);  // normal end
}  // OnClosed()

bool ClientAsyncWriterImpl::TryToSendNext() {
  // private function has no Guard.
  assert(!msg_queue_.empty());
  assert(status_.ok());

  auto* tag = new ClientSendMsgCqTag(call_sptr_);
  auto sptr = shared_from_this();  // CqTag will keep sptr
  CompleteCb complete_cb = [sptr](bool success) {
    sptr->OnSent(success);
  };
  tag->SetCompleteCb(complete_cb);

  bool ok = tag->Start(msg_queue_.front());  // Send the front.
  if (ok) return true;

  delete tag;
  status_.SetInternalError("Failed to write client-side streaming.");
  TryToCallCloseCb();
  return false;
}  // TryToSendNext()

}  // namespace grpc_cb_core
