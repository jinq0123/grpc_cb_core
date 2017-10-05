// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_READER_READ_CQTAG_H
#define GRPC_CB_CORE_CLIENT_CLIENT_READER_READ_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb_core/common/impl/call.h>                // for GetMaxMsgSize()
#include <grpc_cb_core/common/impl/call_op_data.h>        // for CodRecvMsg
#include <grpc_cb_core/common/impl/call_operations.h>     // for CallOperations
#include <grpc_cb_core/common/impl/general_call_cqtag.h>  // for GeneralCallCqTag
#include <grpc_cb_core/common/support/config.h>    // for GRPC_FINAL

namespace grpc_cb_core {

// Completion queue tag to read msg.
// For ClientReader and ClientReaderWriter.
class ClientReaderReadCqTag GRPC_FINAL : public GeneralCallCqTag {
 public:
  explicit ClientReaderReadCqTag(const CallSptr& call_sptr)
      : GeneralCallCqTag(call_sptr) {}

  inline bool Start() GRPC_MUST_USE_RESULT;

  // To detect end of stream.
  inline bool HasGotMsg() const { return cod_recv_msg_.HasGotMsg(); }
  inline Status GetResultMsg(std::string& message) GRPC_MUST_USE_RESULT {
    return cod_recv_msg_.GetResultMsg(message);
    // XXX no use? GetCallSptr()->GetMaxMsgSize());
  }

 private:
  CodRecvMsg cod_recv_msg_;
};  // class ClientReaderReadCqTag

bool ClientReaderReadCqTag::Start() {
  CallOperations ops;
  ops.RecvMsg(cod_recv_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_CLIENT_CLIENT_READER_READ_CQTAG_H
