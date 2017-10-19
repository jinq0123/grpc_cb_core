// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef CPP_CB_SERVER_SERVER_REPLIER_CQTAT_H
#define CPP_CB_SERVER_SERVER_REPLIER_CQTAT_H

#include <string>

#include "common/impl/call.h"             // for Call
#include "common/impl/call_cqtag.h"       // for CallCqTag
#include "common/impl/call_op_data.h"     // for CodSendInitMd
#include "common/impl/call_operations.h"  // for CallOperations
#include <grpc_cb_core/common/call_sptr.h>        // for CallSptr
#include <grpc_cb_core/common/support/config.h>        // for GRPC_FINAL

namespace grpc_cb_core {

class ServerReplierCqTag GRPC_FINAL : public CallCqTag {
 public:
  ServerReplierCqTag(const CallSptr& call_sptr, bool send_init_md)
    : CallCqTag(call_sptr), send_init_md_(send_init_md) {}

  inline bool StartReply(const std::string& msg) GRPC_MUST_USE_RESULT;
  inline bool StartReplyError(const Status& status) GRPC_MUST_USE_RESULT;

 private:
  inline void SendMsgAndStatus(const std::string& msg,
                               CallOperations& ops);

 private:
  CodSendInitMd cod_send_init_md_;
  CodSendMsg cod_send_msg_;
  CodServerSendStatus cod_server_send_status_;
  bool send_init_md_ = false;  // need to send initial metadata
};

bool ServerReplierCqTag::StartReply(const std::string& msg) {
  CallOperations ops;
  if (send_init_md_) {  // Todo: use CodSendInitMd uptr?
    ops.SendInitMd(cod_send_init_md_);  // Todo: init metadata
  }
  SendMsgAndStatus(msg, ops);
  return GetCallSptr()->StartBatch(ops, this);
}

bool ServerReplierCqTag::StartReplyError(const Status& status) {
  CallOperations ops;
  if (send_init_md_) {
    ops.SendInitMd(cod_send_init_md_);
  }
  ops.ServerSendStatus(status, cod_server_send_status_);
  return GetCallSptr()->StartBatch(ops, this);
}

void ServerReplierCqTag::SendMsgAndStatus(const std::string& msg,
                                          CallOperations& ops) {
  ops.SendMsg(msg, cod_send_msg_);
  ops.ServerSendStatus(Status::OK, cod_server_send_status_);
}

}  // namespace grpc_cb_core
#endif  // CPP_CB_SERVER_SERVER_REPLIER_CQTAT_H
