// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_WRITE_WORKER_SPTR_H
#define GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_WRITE_WORKER_SPTR_H

#include <memory>

namespace grpc_cb_core {
class ClientAsyncWriteWorker;
using ClientAsyncWriteWorkerSptr = std::shared_ptr<ClientAsyncWriteWorker>;
}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_WRITE_WORKER_SPTR_H
