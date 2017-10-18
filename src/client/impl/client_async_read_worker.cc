// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_read_worker.h"

#include <cassert>  // for assert()

#include "client_reader_read_cqtag.h"  // for ClientReaderReadCqTag

namespace grpc_cb_core {

ClientAsyncReadWorker::ClientAsyncReadWorker(CallSptr call_sptr,
    const MsgStrCb& msg_cb, const EndCb& end_cb)
    : call_sptr_(call_sptr),
      msg_cb_(msg_cb),
      end_cb_(end_cb) {
  assert(call_sptr);
}

ClientAsyncReadWorker::~ClientAsyncReadWorker() {}

// Setup to read each.
void ClientAsyncReadWorker::Start() {
  Guard g(mtx_);
  if (aborted_) return;
  if (started_) return;
  started_ = true;
  Next();
}

void ClientAsyncReadWorker::Abort() {
  Guard g(mtx_);
  aborted_ = true;
}  // Abort()

// Return copy for thread-safety.
const Status ClientAsyncReadWorker::GetStatus() const {
  Guard g(mtx_);
  return status_;
}

// Setup next async read.
void ClientAsyncReadWorker::Next() {
  // private function need no Guard.
  assert(started_);
  assert(!aborted_);

  auto* tag = new ClientReaderReadCqTag(call_sptr_);
  auto sptr = shared_from_this();
  tag->SetCompleteCb([sptr, tag](bool success) {
      sptr->OnRead(success, *tag);
  });
  if (tag->Start()) return;

  delete tag;
  status_.SetInternalError("Failed to async read server stream.");
  CallEndCb();
}  // Next()

void ClientAsyncReadWorker::OnRead(bool success, ClientReaderReadCqTag& tag) {
  Guard g(mtx_);
  if (aborted_)  // Maybe writer failed.
    return;
  assert(status_.ok());
  if (!success) {
    status_.SetInternalError("ClientReaderReadCqTag failed.");
    CallEndCb();
    return;
  }
  if (!tag.HasGotMsg()) {
    // CallEndCb of read.
    // Receiving status will be after all reading and writing.
    CallEndCb();
    return;
  }

  std::string sMsg;
  status_ = tag.GetResultMsg(sMsg);
  if (!status_.ok()) {
    CallEndCb();
    return;
  }

  if (msg_cb_) {
    status_ = msg_cb_(sMsg);
    if (!status_.ok()) {
      CallEndCb();
      return;
    }
  }

  Next();
}  // OnRead()

void ClientAsyncReadWorker::CallEndCb() {
  // private function need no Guard.
  if (end_cb_)
    end_cb_();
}

}  // namespace grpc_cb_core
