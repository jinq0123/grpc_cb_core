// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/server/server_reader_for_bidi_streaming.h>

#include <cassert>

#include <grpc_cb_core/server/server_writer.h>  // for ServerWriter

namespace grpc_cb_core {

ServerReaderForBidiStreaming::~ServerReaderForBidiStreaming() {}

void ServerReaderForBidiStreaming::SetWriter(const Writer& writer) {
  writer_uptr_.reset(new Writer(writer));
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
