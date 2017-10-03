// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H
#define GRPC_CB_CORE_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H

#include <string>

namespace grpc_cb_core {

class Status;

// Base class for close handler template class.
class ClientAsyncWriterCloseHandler {
 public:
  virtual ~ClientAsyncWriterCloseHandler() {};

  virtual std::string& GetMsg() = 0;
  virtual void OnClose(const Status& status) = 0;
};  // class ClientAsyncWriterCloseHandler

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H
