// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/server/server_reader_for_client_side_streaming.h>

#include <cassert>

#include <grpc_cb_core/server/server_replier.h>  // for ReplyError()
#include "impl/server_reader_cqtag.h"  // for ServerReaderCqTag

namespace grpc_cb_core {

ServerReaderForClientSideStreaming::~ServerReaderForClientSideStreaming() {}

void ServerReaderForClientSideStreaming::Start(
    const CallSptr& call_sptr, const ReplierSptr& replier_sptr) {
  assert(call_sptr);
  assert(replier_sptr);
  replier_sptr_ = replier_sptr;

  using CqTag = ServerReaderCqTag;
  CqTag* tag = new CqTag(call_sptr, shared_from_this());
  if (tag->Start()) return;

  delete tag;
  OnError(Status::InternalError("Failed to init client-side streaming."));
}

void ServerReaderForClientSideStreaming::ReplyStr(const std::string& response) {
  assert(replier_sptr_);  // Must after Start().
  replier_sptr_->ReplyStr(response);
}

void ServerReaderForClientSideStreaming::ReplyError(const Status& status) {
  assert(replier_sptr_);  // Must after Start().
  replier_sptr_->ReplyError(status);
}

ServerReplier& ServerReaderForClientSideStreaming::GetReplier() {
  assert(replier_sptr_);  // Must after Start().
  return *replier_sptr_;
}

void ServerReaderForClientSideStreaming::OnError(const Status& status) {
  ReplyError(status);
}

}  // namespace grpc_cb_core
