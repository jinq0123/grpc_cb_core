// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CALL_CQTAG_H
#define GRPC_CB_CORE_CLIENT_CALL_CQTAG_H

#include <grpc/support/port_platform.h>  // for GRPC_MUST_USE_RESULT

#include "common/impl/call.h"             // for GetMaxMsgSize()
#include "common/impl/call_cqtag.h"       // for CallCqTag
#include "common/impl/call_op_data.h"     // for CodSendInitMd
#include "common/impl/call_operations.h"  // for CallOperations
#include "common/impl/metadata_vector.h"  // for MetadataVector

namespace grpc_cb_core {

// Completion queue tag (CqTag) for client call for both sync and async calls.
// ClientAsyncCallCqTag derives from it.
class ClientCallCqTag : public CallCqTag {
 public:
  explicit ClientCallCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {
    assert(call_sptr);
  }
  virtual ~ClientCallCqTag() {}

 public:
  inline bool Start(const std::string& request) GRPC_MUST_USE_RESULT;

 public:
  Status GetResponse(std::string& response) const;

 private:
  inline bool StartOps(CallOperations& ops);

 private:
  CodSendInitMd cod_send_init_md_;  // Todo: set init metadata
  CodSendMsg cod_send_msg_;
  CodRecvInitMd cod_recv_init_md_;
  CodRecvMsg cod_recv_msg_;
  CodClientRecvStatus cod_client_recv_status_;
};  // class ClientCallCqTag

bool ClientCallCqTag::Start(const std::string& request) {
  CallOperations ops;
  ops.SendMsg(request, cod_send_msg_);
  return StartOps(ops);
}

bool ClientCallCqTag::StartOps(CallOperations& ops) {
  // Todo: Fill send_init_md_array_ -> FillMetadataVector()
  ops.SendInitMd(cod_send_init_md_);
  ops.RecvInitMd(cod_recv_init_md_);
  ops.RecvMsg(cod_recv_msg_);
  ops.ClientSendClose();
  ops.ClientRecvStatus(cod_client_recv_status_);
  return GetCallSptr()->StartBatch(ops, this);
}

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_CALL_CQTAG_H
