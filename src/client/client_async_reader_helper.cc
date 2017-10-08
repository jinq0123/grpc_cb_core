// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_helper.h"

#include <cassert>  // for assert()

#include <grpc_cb_core/client/impl/client_async_read_handler.h>  // for HandleMsg()
#include <grpc_cb_core/client/impl/client_reader_read_cqtag.h>  // for ClientReaderReadCqTag

namespace grpc_cb_core {

ClientAsyncReaderHelper::ClientAsyncReaderHelper(CallSptr call_sptr,
    const ClientAsyncReadHandlerSptr& read_handler_sptr, const EndCb& end_cb)
    : call_sptr_(call_sptr),
      read_handler_sptr_(read_handler_sptr),
      end_cb_(end_cb) {
  assert(call_sptr);
  assert(read_handler_sptr);
  assert(end_cb);
}

ClientAsyncReaderHelper::~ClientAsyncReaderHelper() {}

// Setup to read each.
void ClientAsyncReaderHelper::Start() {
  if (started_) return;
  started_ = true;
  Next();
}

// Setup next async read.
void ClientAsyncReaderHelper::Next() {
  assert(started_);
  if (aborted_)  // Maybe writer failed.
    return;

  auto* tag = new ClientReaderReadCqTag(call_sptr_);
  auto sptr = shared_from_this();
  tag->SetCompleteCb([sptr, tag](bool success) {
      sptr->OnRead(success, *tag);
  });
  if (tag->Start()) return;

  delete tag;
  status_.SetInternalError("Failed to async read server stream.");
  end_cb_();
}

void ClientAsyncReaderHelper::OnRead(bool success, ClientReaderReadCqTag& tag) {
  if (aborted_)  // Maybe writer failed.
    return;
  assert(status_.ok());
  assert(end_cb_);
  if (!success) {
    status_.SetInternalError("ClientReaderReadCqTag failed.");
    end_cb_();
    return;
  }
  if (!tag.HasGotMsg()) {
    // End of read. Do not recv status in Reader. Do it after all reading and writing.
    end_cb_();
    return;
  }

  status_ = tag.GetResultMsg(read_handler_sptr_->GetMsg());
  if (!status_.ok()) {
    end_cb_();
    return;
  }

  read_handler_sptr_->HandleMsg();  // XXX check OK in msg parsing
  Next();
}

}  // namespace grpc_cb_core
