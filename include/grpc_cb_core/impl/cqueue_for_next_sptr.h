// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_IMPL_CQUEUE_FOR_NEXT_SPTR_H
#define GRPC_CB_CORE_IMPL_CQUEUE_FOR_NEXT_SPTR_H

#include <memory>

namespace grpc_cb_core {
class CQueueForNext;
using CQueueForNextSptr = std::shared_ptr<CQueueForNext>;
}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_IMPL_CQUEUE_FOR_NEXT_SPTR_H
