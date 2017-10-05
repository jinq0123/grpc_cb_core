// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H
#define GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H

#include <functional>
#include <string>
#include <grpc_cb_core/common/support/config.h>  // for GRPC_FINAL

namespace grpc_cb_core {

// Handler to async read.
class ClientAsyncReadHandler GRPC_FINAL {
 public:
  using OnMsg = std::function<void(const std::string&)>;
  explicit ClientAsyncReadHandler(const OnMsg& on_msg) : on_msg_(on_msg) {}

  std::string& GetMsg() { return msg_; }
  void HandleMsg() { if (on_msg_) on_msg_(msg_); }

 private:
  OnMsg on_msg_;
  std::string msg_;
};  // class ClientAsyncReadHandler

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H
