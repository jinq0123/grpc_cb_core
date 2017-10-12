// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/server/server_reader.h>

#include <cassert>

#include "impl/server_reader_cqtag.h"         // for ServerReaderCqTag
#include "impl/server_reader_writer_cqtag.h"  // for ServerReaderWriterCqTag

namespace grpc_cb_core {

Status ServerReader::OnMsgStr(const std::string& msg_str) {
  return Status::OK;
}

void ServerReader::StartForBidiStreaming(const CallSptr& call_sptr) {
  assert(call_sptr);

  using RwCqTag = ServerReaderWriterCqTag;
  RwCqTag* tag = new RwCqTag(call_sptr, shared_from_this());
  if (tag->Start()) return;

  delete tag;
  OnError(Status::InternalError("Failed to init bi-directional streaming."));
}

void ServerReader::StartForClientSideStreaming(const CallSptr& call_sptr)
{
  assert(call_sptr);

  using CqTag = ServerReaderCqTag;
  CqTag* tag = new CqTag(call_sptr, shared_from_this());
  if (tag->Start()) return;

  delete tag;
  OnError(Status::InternalError("Failed to init client-side streaming."));
}

}  // namespace grpc_cb_core
