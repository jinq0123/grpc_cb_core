// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/server/server_reader_for_bidi_streaming.h>

#include <cassert>

#include <grpc_cb_core/server/server_writer.h>  // for ServerWriter
#include "impl/server_reader_writer_cqtag.h"  // for ServerReaderWriterCqTag

namespace grpc_cb_core {

ServerReaderForBidiStreaming::~ServerReaderForBidiStreaming() {}

void ServerReaderForBidiStreaming::Start(
    const CallSptr& call_sptr, const WriterSptr& writer_sptr) {
  assert(call_sptr);
  assert(writer_sptr);
  writer_sptr_ = writer_sptr;

  using RwCqTag = ServerReaderWriterCqTag;
  RwCqTag* tag = new RwCqTag(call_sptr, shared_from_this());
  if (tag->Start()) return;

  delete tag;
  OnError(Status::InternalError("Failed to init bi-directional streaming."));
}

ServerWriter& ServerReaderForBidiStreaming::GetWriter() {
  assert(writer_sptr_);  // Must after Start()
  return *writer_sptr_;
}

void ServerReaderForBidiStreaming::OnError(const Status& status) {
  assert(writer_sptr_);
  writer_sptr_->AsyncClose(status);
}

}  // namespace grpc_cb_core
