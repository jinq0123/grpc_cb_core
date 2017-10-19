// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/client/client_async_writer.h>

#include <cassert>  // for assert()
#include <cstdint>  // for int64_t
#include <string>

#include <grpc_cb_core/client/channel_sptr.h>           // for ChannelSptr
#include <grpc_cb_core/client/close_cb.h>               // for CloseCb
#include <grpc_cb_core/common/completion_queue_sptr.h>  // for CompletionQueueSptr
#include "impl/client_async_writer_wrapped_impl.h"  // for ClientAsyncWriterWrappedImpl

namespace grpc_cb_core {

ClientAsyncWriter::ClientAsyncWriter(const ChannelSptr& channel,
                                     const std::string& method,
                                     const CompletionQueueSptr& cq_sptr,
                                     int64_t timeout_ms)
    // Todo: same as ClientReader?
    : impl_sptr_(
          new ClientAsyncWriterWrappedImpl(channel, method, cq_sptr, timeout_ms)) {
  assert(channel);
  assert(cq_sptr);
}

// Todo: Get queue size()
// Todo: SyncGetInitMd();

bool ClientAsyncWriter::Write(const std::string& request) const {
  return impl_sptr_->Write(request);
}

void ClientAsyncWriter::Close(const CloseCb& close_cb/* = nullptr*/) {
  impl_sptr_->Close(close_cb);
}  // Close()

}  // namespace grpc_cb_core
