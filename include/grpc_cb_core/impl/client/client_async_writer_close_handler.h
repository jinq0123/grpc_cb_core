// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H
#define GRPC_CB_CORE_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H

#include <functional>
#include <string>

#include <grpc_cb_core/support/config.h>  // for GRPC_FINAL

namespace grpc_cb_core {

class Status;

// Handler for client async writer.
class ClientAsyncWriterCloseHandler GRPC_FINAL {
 public:
  using ClosedCallback = std::function<void (const Status&, const std::string&)>;
  explicit ClientAsyncWriterCloseHandler(
      const ClosedCallback& on_closed = ClosedCallback())
      : on_closed_(on_closed) {};

  std::string& GetMsg() { return msg_; }
  void OnClose(const Status& status) {
    if (on_closed_) on_closed_(status, msg_);
  }

 private:
  std::string msg_;
  ClosedCallback on_closed_;
};  // class ClientAsyncWriterCloseHandler

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H
