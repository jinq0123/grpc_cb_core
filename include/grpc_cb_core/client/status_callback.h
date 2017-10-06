// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_STATUS_CALLBACK_H
#define GRPC_CB_CORE_STATUS_CALLBACK_H

#include <functional>  // for std::function()

namespace grpc_cb_core {

class Status;
using StatusCb = std::function<void (const Status& status)>;
using ErrorCb = StatusCb;

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_STATUS_CALLBACK_H
