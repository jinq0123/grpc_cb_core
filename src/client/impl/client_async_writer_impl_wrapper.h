// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_WRITER_IMPL_WRAPPER_H
#define GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_WRITER_IMPL_WRAPPER_H

#include <string>

#include <grpc_cb_core/client/channel_sptr.h>           // for ChannelSptr
#include <grpc_cb_core/client/close_cb.h>               // for CloseCb
#include <grpc_cb_core/common/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb_core/common/support/config.h>         // for GRPC_FINAL

namespace grpc_cb_core {

class ClientAsyncWriterImpl;

// Only shared in ClientAsyncWriter, because we need dtr() to close writing.
// Thread-safe.
class ClientAsyncWriterImplWrapper GRPC_FINAL {
 public:
  ClientAsyncWriterImplWrapper(const ChannelSptr& channel,
                               const std::string& method,
                               const CompletionQueueSptr& cq_sptr,
                               int64_t timeout_ms);
  ~ClientAsyncWriterImplWrapper();

  bool Write(const std::string& request);
  void Close(const CloseCb& close_cb);

  // Todo: Force to close, cancel all writing. CloseNow()
  // Todo: get queue size

 private:
  // Will live longer than ClientAsyncWriter.
  std::shared_ptr<ClientAsyncWriterImpl> impl_sptr_;
};  // class ClientAsyncWriterImplWrapper

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_WRITER_IMPL_WRAPPER_H
