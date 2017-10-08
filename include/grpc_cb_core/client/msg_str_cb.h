// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_MSG_STR_CB_H
#define GRPC_CB_CORE_CLIENT_MSG_STR_CB_H

#include <functional>
#include <string>

namespace grpc_cb_core {

class Status;

// Message string callback function.
using MsgStrCb = std::function<Status (const std::string& message)>;
// Response string callback function.
using RespStrCb = MsgStrCb;

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_MSG_STR_CB_H
