// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_impl.h"

#include <cassert>  // for assert()

#include "client_async_writer_impl2.h"  // for ClientAsyncWriterImpl2

namespace grpc_cb_core {

ClientAsyncWriterImpl::ClientAsyncWriterImpl(const ChannelSptr& channel,
                                             const std::string& method,
                                             const CompletionQueueSptr& cq_sptr,
                                             int64_t timeout_ms)
    : impl2_sptr_(
          new ClientAsyncWriterImpl2(channel, method, cq_sptr, timeout_ms)) {
  assert(cq_sptr);
  assert(channel);
}

ClientAsyncWriterImpl::~ClientAsyncWriterImpl() {
  impl2_sptr_->Close();  // without handler
}

bool ClientAsyncWriterImpl::Write(const std::string& request) {
  return impl2_sptr_->Write(request);
}

void ClientAsyncWriterImpl::Close(const CloseHandlerSptr& handler_sptr) {
  impl2_sptr_->Close(handler_sptr);
}

}  // namespace grpc_cb_core
