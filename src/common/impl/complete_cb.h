// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_COMMON_IMPL_COMPLETE_CB_H
#define GRPC_CB_CORE_COMMON_IMPL_COMPLETE_CB_H

#include <functional>  // for function<>

namespace grpc_cb_core {

// Callback on completion
using CompleteCb = std::function<void (bool success)>;

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_COMMON_IMPL_COMPLETE_CB_H
