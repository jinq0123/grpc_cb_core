// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_RECV_STATUS_H
#define GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_RECV_STATUS_H

#include <grpc_cb_core/client/status_callback.h>  // for StatusCallback
#include <grpc_cb_core/common/impl/call_sptr.h>   // for CallSptr

namespace grpc_cb_core {
namespace ClientAsyncReader {
void RecvStatus(const CallSptr& call_sptr, const StatusCallback& on_status);
}  // namespace ClientAsyncReader
}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_RECV_STATUS_H
