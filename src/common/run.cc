// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/common/run.h>

#include "impl/do_next_completion.h"  // for DoNextCompletion()

namespace grpc_cb_core {

void Run(CompletionQueueForNext& cq4n) {
  while (DoNextCompletion(cq4n))
    ;
}

void Run(const CompletionQueueForNextSptr& cq4n_sptr) {
  if (cq4n_sptr)
    Run(*cq4n_sptr);
}

}  // namespace grpc_cb_core
