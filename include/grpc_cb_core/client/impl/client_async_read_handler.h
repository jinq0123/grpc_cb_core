// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H
#define GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H

#include <functional>
#include <string>
#include <grpc_cb_core/common/support/config.h>  // for GRPC_FINAL
#include <grpc_cb_core/client/msg_cb.h>  // for MsgCb

namespace grpc_cb_core {

// Handler to async read.
class ClientAsyncReadHandler GRPC_FINAL {
 public:
  explicit ClientAsyncReadHandler(const MsgCb& msg_cb) : on_msg_(msg_cb) {}

  std::string& GetMsg() { return msg_; }
  void HandleMsg() { if (on_msg_) on_msg_(msg_); }

 private:
  MsgCb on_msg_;
  std::string msg_;
};  // class ClientAsyncReadHandler

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H
