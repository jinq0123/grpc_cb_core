// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_write_worker.h"

#include <cassert>     // for assert()

#include "client_send_msg_cqtag.h"  // for ClientSendMsgCqTag
#include <grpc_cb_core/common/status.h>                      // for Status::ok()

namespace grpc_cb_core {

ClientAsyncWriteWorker::ClientAsyncWriteWorker(const CallSptr& call_sptr)
    : call_sptr_(call_sptr) {
  assert(call_sptr);
}

ClientAsyncWriteWorker::~ClientAsyncWriteWorker() {}

bool ClientAsyncWriteWorker::Queue(const std::string& msg) {
  Guard g(mtx_);
  if (aborted_)  // Maybe reader failed.
    return false;
  if (is_closing_)
    return true;  // ignore msg if is closing

  // cache messages
  msg_queue_.push(msg);
  return true;
  // DEL
  //if (is_writing_) return true;
  //return WriteNext();
}  // Queue()

void ClientAsyncWriteWorker::SetClosing() {
  Guard g(mtx_);
  if (is_closing_) return;
  if (aborted_) return;
  is_closing_ = true;
  if (is_writing_) return;  // will CallEndCb() in OnWritten()
  assert(msg_queue_.empty());
  CallEndCb();
}  // SetClosing()

// Abort writing. Stop sending.
void ClientAsyncWriteWorker::Abort() {
  Guard g(mtx_);
  aborted_ = true;
}

// return copy for thread-safety
const Status ClientAsyncWriteWorker::GetStatus() const {
  Guard g(mtx_);
  return status_;
}

bool ClientAsyncWriteWorker::WriteNext(const CompleteCb& complete_cb) {
  Guard g(mtx_);
  assert(!is_writing_);
  assert(!msg_queue_.empty());

  if (aborted_) return false;  // Maybe reader failed.
  is_writing_ = true;

  assert(call_sptr_);
  auto* tag = new ClientSendMsgCqTag(call_sptr_);
  tag->SetCompleteCb(complete_cb);
  bool ok = tag->Start(msg_queue_.front());
  msg_queue_.pop();  // may empty now but is_writing_
  if (ok) return true;

  delete tag;
  status_.SetInternalError("Failed to write client stream.");
  CallEndCb();  // error end
  return false;
}  // WriteNext()

void ClientAsyncWriteWorker::OnWritten(bool success) {
  Guard g(mtx_);
  assert(status_.ok());
  assert(is_writing_);
  is_writing_ = false;
  if (!success) {
    status_.SetInternalError("ClientSendMsgCqTag failed in ClientAsyncWriteWorker.");
    CallEndCb();  // error end
    return;
  }
  // XXX
  //if (!msg_queue_.empty()) {
  //  WriteNext();
  //  return;
  //}

  if (is_closing_)
    CallEndCb();  // normal end
}  // OnWritten()

void ClientAsyncWriteWorker::CallEndCb() {
  // private function need no Guard g(mtx_);
    // XXX
  //if (end_cb_)
  //  end_cb_();
}

}  // namespace grpc_cb_core
