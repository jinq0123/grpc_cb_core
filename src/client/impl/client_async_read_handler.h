// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H
#define GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H

#include <functional>
#include <string>

#include <grpc_cb_core/common/status.h>  // for Status
#include <grpc_cb_core/common/support/config.h>  // for GRPC_FINAL
#include <grpc_cb_core/client/msg_str_cb.h>  // for MsgStrCb

namespace grpc_cb_core {

// Handler to async read.
class ClientAsyncReadHandler GRPC_FINAL {
 public:
  explicit ClientAsyncReadHandler(const MsgStrCb& msg_cb) : msg_cb_(msg_cb) {}

  std::string& GetMsg() { return msg_; }
  Status HandleMsg() {
    if (msg_cb_) return msg_cb_(msg_);  // maybe fail in parsing.
    return Status::OK;
  }

 private:
  MsgStrCb msg_cb_;
  std::string msg_;
};  // class ClientAsyncReadHandler

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H
