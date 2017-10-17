// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_helper.h"

#include <cassert>     // for assert()

#include "client_send_msg_cqtag.h"  // for ClientSendMsgCqTag
#include <grpc_cb_core/common/status.h>                      // for Status::ok()

namespace grpc_cb_core {

ClientAsyncWriterHelper::ClientAsyncWriterHelper(
    const CallSptr& call_sptr, const EndCb& end_cb)
    : call_sptr_(call_sptr), end_cb_(end_cb) {
  assert(call_sptr);
  assert(end_cb);
}

ClientAsyncWriterHelper::~ClientAsyncWriterHelper() {}

bool ClientAsyncWriterHelper::Queue(const std::string& msg) {
  Guard g(mtx_);
  if (aborted_)  // Maybe reader failed.
    return false;
  if (is_closing_)
    return true;  // ignore msg if is closing

  // cache messages
  msg_queue_.push(msg);
  if (is_writing_) return true;
  return WriteNext();
}  // Queue()

void ClientAsyncWriterHelper::SetClosing() {
  Guard g(mtx_);
  if (is_closing_) return;
  if (aborted_) return;
  is_closing_ = true;
  if (is_writing_) return;  // call end_cb() in OnWritten()
  assert(msg_queue_.empty());
  end_cb_();
}  // SetClosing()

// Abort writing. Stop sending.
void ClientAsyncWriterHelper::Abort() {
  Guard g(mtx_);
  aborted_ = true;
  end_cb_ = EndCb();  // to stop circular sharing
}

// return copy for thread-safety
const Status ClientAsyncWriterHelper::GetStatus() const {
  Guard g(mtx_);
  return status_;
}

bool ClientAsyncWriterHelper::WriteNext() {
  Guard g(mtx_);
  assert(!is_writing_);
  assert(!msg_queue_.empty());

  if (aborted_) return false;  // Maybe reader failed.
  is_writing_ = true;

  assert(call_sptr_);
  auto* tag = new ClientSendMsgCqTag(call_sptr_);
  auto sptr = shared_from_this();
  tag->SetCompleteCb([sptr](bool success) {
      sptr->OnWritten(success);
  });

  bool ok = tag->Start(msg_queue_.front());
  msg_queue_.pop();  // may empty now but is_writing_
  if (ok) return true;

  delete tag;
  status_.SetInternalError("Failed to write client stream.");
  end_cb_();  // error end
  return false;
}  // WriteNext()

void ClientAsyncWriterHelper::OnWritten(bool success) {
  Guard g(mtx_);
  assert(status_.ok());
  assert(is_writing_);
  is_writing_ = false;
  if (!success) {
    status_.SetInternalError("ClientSendMsgCqTag failed in ClientAsyncWriterHelper.");
    end_cb_();  // error end
    return;
  }
  if (!msg_queue_.empty()) {
    WriteNext();
    return;
  }

  if (is_closing_)
    end_cb_();  // normal end
}  // OnWritten()

}  // namespace grpc_cb_core
