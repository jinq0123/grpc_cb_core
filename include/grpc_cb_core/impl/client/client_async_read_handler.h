// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H
#define GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H

#include <string>

namespace grpc_cb_core {

// As the base class of handler template class.
class ClientAsyncReadHandler {
 public:
  ClientAsyncReadHandler() {}
  virtual ~ClientAsyncReadHandler() {}

  virtual std::string& GetMsg() = 0;
  virtual void HandleMsg() = 0;
};  // class ClientAsyncReadHandler

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READ_HANDLER_H
