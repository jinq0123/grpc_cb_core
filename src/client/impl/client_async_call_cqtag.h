// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_ASYNC_CALL_CQTAG_H
#define GRPC_CB_CORE_CLIENT_ASYNC_CALL_CQTAG_H

#include <string>

#include "client_call_cqtag.h"  // for ClientCallCqTag
#include <grpc_cb_core/client/msg_str_cb.h>                  // for RespStrCb
#include <grpc_cb_core/client/status_cb.h>               // for ErrorCb
#include <grpc_cb_core/common/support/config.h>          // for GRPC_FINAL

namespace grpc_cb_core {

// Completion queue tag (CqTag) for client async call.
// Derived from ClientCallCqTag, adding response_cb, error_cb.
class ClientAsyncCallCqTag GRPC_FINAL : public ClientCallCqTag {
 public:
  explicit ClientAsyncCallCqTag(const CallSptr call_sptr)
     : ClientCallCqTag(call_sptr) {}

 public:
  void SetResponseCb(const RespStrCb& response_cb) {
    response_cb_ = response_cb;
  }
  void SetErrorCb(const ErrorCb& error_cb) {
    error_cb_ = error_cb;
  }

 public:
  void DoComplete(bool success) GRPC_OVERRIDE {
    if (!success) {
      CallErrorCb(Status::InternalError("ClientAsyncCallCqTag failed."));
      return;
    }

    std::string resp;
    Status status = GetResponse(resp);
    if (!status.ok()) {
      CallErrorCb(status);
      return;
    }

    if (!response_cb_) return;
    status = response_cb_(resp);
    if (!status.ok()) CallErrorCb(status);
  };

 private:
  void CallErrorCb(const Status& status) const {
    if (error_cb_)
      error_cb_(status);
  }

 private:
  RespStrCb response_cb_;
  ErrorCb error_cb_;
};  // class ClientAsyncCallCqTag

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_ASYNC_CALL_CQTAG_H
