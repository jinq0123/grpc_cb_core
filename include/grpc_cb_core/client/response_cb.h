// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_RESPONSE_CB_H
#define GRPC_CB_CORE_CLIENT_RESPONSE_CB_H

#include <functional>
#include <string>

namespace grpc_cb_core {

// Response callback function.
using ResponseCb = std::function<void (const std::string& response)>;

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_RESPONSE_CB_H
