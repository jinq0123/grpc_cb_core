// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_IMPL_SERVER_SERVER_READER_CQTAG_H
#define GRPC_CB_CORE_IMPL_SERVER_SERVER_READER_CQTAG_H

#include <functional>
#include <memory>  // for shared_ptr<>
#include <string>

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include "common/impl/call.h"             // for StartBatch()
#include "common/impl/call_cqtag.h"       // for CallCqTag
#include "common/impl/call_op_data.h"     // for CodSendInitMd
#include "common/impl/call_operations.h"  // for CallOperations
#include <grpc_cb_core/common/support/config.h>        // for GRPC_FINAL
#include <grpc_cb_core/server_reader.h>                // for ServerReader

namespace grpc_cb_core {

class ServerReaderCqTag GRPC_FINAL : public CallCqTag {
 public:
  using Reader = ServerReader;
  using ReaderSptr = std::shared_ptr<Reader>;
  inline explicit ServerReaderCqTag(const CallSptr& call_sptr,
                                    const ReaderSptr& reader_sptr);

 public:
  inline bool Start() GRPC_MUST_USE_RESULT;
 public:
  inline void DoComplete(bool success) GRPC_OVERRIDE;
 private:
  CodRecvMsg cod_recv_msg_;
  ReaderSptr reader_sptr_;
};  // class ServerReaderCqTag

ServerReaderCqTag::ServerReaderCqTag(
    const CallSptr& call_sptr, const ReaderSptr& reader_sptr)
    : CallCqTag(call_sptr), reader_sptr_(reader_sptr) {
  assert(call_sptr);
  assert(reader_sptr);
}

bool ServerReaderCqTag::Start() {
  CallOperations ops;
  ops.RecvMsg(cod_recv_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

void ServerReaderCqTag::DoComplete(bool success) {
  if (!success) {
    reader_sptr_->OnError(Status::InternalError("ServerReaderCqTag failed."));
    return;
  }
  if (!cod_recv_msg_.HasGotMsg()) {
    reader_sptr_->OnEnd();
    return;
  }

  std::string msg;
  Status status = cod_recv_msg_.GetResultMsg(msg);
  if (!status.ok()) {
    reader_sptr_->OnError(status);
    return;
  }
  reader_sptr_->OnMsg(msg);

  const CallSptr& call_sptr = GetCallSptr();
  auto* tag = new ServerReaderCqTag(call_sptr, reader_sptr_);
  if (tag->Start()) return;
  delete tag;
  reader_sptr_->OnError(Status::InternalError(
    "Failed to read client stream."));
}

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_IMPL_SERVER_SERVER_READER_CQTAG_H
