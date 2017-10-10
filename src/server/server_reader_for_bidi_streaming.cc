// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/server/server_reader_for_bidi_streaming.h>

#include <cassert>

#include <grpc_cb_core/server/server_writer.h>  // for ServerWriter
#include "impl/server_reader_writer_cqtag.h"  // for ServerReaderWriterCqTag

namespace grpc_cb_core {

ServerReaderForBidiStreaming::~ServerReaderForBidiStreaming() {}

void ServerReaderForBidiStreaming::Start(
    const CallSptr& call_sptr, const Writer& writer) {
  assert(call_sptr);
  writer_uptr_.reset(new Writer(writer));

  using RwCqTag = ServerReaderWriterCqTag;
  RwCqTag* tag = new RwCqTag(call_sptr, shared_from_this());
  if (tag->Start()) return;

  delete tag;
  OnError(Status::InternalError("Failed to init bi-directional streaming."));
}

ServerWriter& ServerReaderForBidiStreaming::GetWriter() {
  assert(writer_uptr_);
  return *writer_uptr_;
}

void ServerReaderForBidiStreaming::OnError(const Status& status) {
  assert(writer_uptr_);
  writer_uptr_->AsyncClose(status);
}

}  // namespace grpc_cb_core
