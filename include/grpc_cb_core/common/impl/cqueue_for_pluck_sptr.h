// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_IMPL_CQUEUE_FOR_PLUCK_SPTR_H
#define GRPC_CB_CORE_IMPL_CQUEUE_FOR_PLUCK_SPTR_H

#include <memory>

namespace grpc_cb_core {
class CQueueForPluck;
using CQueueForPluckSptr = std::shared_ptr<CQueueForPluck>;
}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_IMPL_CQUEUE_FOR_PLUCK_SPTR_H
