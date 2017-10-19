// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_impl_wrapper.h"

#include <cassert>  // for assert()

#include "client_async_writer_impl2.h"  // for ClientAsyncWriterImpl2

namespace grpc_cb_core {

ClientAsyncWriterImplWrapper::ClientAsyncWriterImplWrapper(const ChannelSptr& channel,
                                             const std::string& method,
                                             const CompletionQueueSptr& cq_sptr,
                                             int64_t timeout_ms)
    : impl2_sptr_(
          new ClientAsyncWriterImpl2(channel, method, cq_sptr, timeout_ms)) {
  assert(cq_sptr);
  assert(channel);
}

ClientAsyncWriterImplWrapper::~ClientAsyncWriterImplWrapper() {
  impl2_sptr_->Close();  // without handler
}

bool ClientAsyncWriterImplWrapper::Write(const std::string& request) {
  return impl2_sptr_->Write(request);
}

void ClientAsyncWriterImplWrapper::Close(const CloseCb& close_cb) {
  impl2_sptr_->Close(close_cb);
}

}  // namespace grpc_cb_core
