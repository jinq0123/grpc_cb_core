// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_HELPER_SPTR_H
#define GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_HELPER_SPTR_H

#include <memory>

namespace grpc_cb_core {
class ClientAsyncReaderHelper;
using ClientAsyncReaderHelperSptr = std::shared_ptr<ClientAsyncReaderHelper>;
}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_HELPER_SPTR_H
