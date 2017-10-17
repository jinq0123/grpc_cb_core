// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H
#define GRPC_CB_CORE_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H

#include <functional>
#include <string>

#include <grpc_cb_core/common/support/config.h>  // for GRPC_FINAL
#include <grpc_cb_core/client/close_cb.h>  // for CloseCb

namespace grpc_cb_core {

class Status;

// Handler for client async writer.
class ClientAsyncWriterCloseHandler GRPC_FINAL {
 public:
  explicit ClientAsyncWriterCloseHandler(
      const CloseCb& close_cb = nullptr)
      : close_cb_(close_cb) {};

  std::string& GetMsg() { return msg_; }
  void OnClose(const Status& status) {
    if (close_cb_) close_cb_(status, msg_);
  }

 private:
  std::string msg_;
  CloseCb close_cb_;
};  // class ClientAsyncWriterCloseHandler

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H
