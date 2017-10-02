#ifndef GRPC_CB_CORE_RUN_H
#define GRPC_CB_CORE_RUN_H

#include <grpc_cb_core/completion_queue_for_next.h>  // for CompletionQueueForNext
#include <grpc_cb_core/completion_queue_for_next_sptr.h>  // for CompletionQueueForNextSptr
#include <grpc_cb_core/support/grpc_cb_core_api.h>  // for GRPC_CB_CORE_API

namespace grpc_cb_core {

GRPC_CB_CORE_API void Run(CompletionQueueForNext& cq4n);
GRPC_CB_CORE_API void Run(const CompletionQueueForNextSptr& cq4n_sptr);

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_RUN_H
