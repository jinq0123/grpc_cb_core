// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_ASYNC_WRITER_IMPL_H
#define GRPC_CB_CORE_CLIENT_ASYNC_WRITER_IMPL_H

#include <string>

#include <grpc_cb_core/client/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb_core/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb_core/common/support/config.h>  // for GRPC_FINAL

// for ClientAsyncWriterCloseHandlerSptr
#include "client_async_writer_close_handler_sptr.h"

namespace grpc_cb_core {

class ClientAsyncWriterImpl2;

// Only shared in ClientAsyncWriter, because we need dtr() to close writing.
// Thread-safe.
class ClientAsyncWriterImpl GRPC_FINAL {
 public:
  ClientAsyncWriterImpl(const ChannelSptr& channel, const std::string& method,
                        const CompletionQueueSptr& cq_sptr, int64_t timeout_ms);
  ~ClientAsyncWriterImpl();

  bool Write(const std::string& request);

  using CloseHandlerSptr = ClientAsyncWriterCloseHandlerSptr;
  void Close(const CloseHandlerSptr& handler_sptr);

  // Todo: Force to close, cancel all writing. CloseNow()
  // Todo: get queue size

 private:
  // Will live longer than ClientAsyncWriter.
  std::shared_ptr<ClientAsyncWriterImpl2> impl2_sptr_;
};  // class ClientAsyncWriterImpl

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_ASYNC_WRITER_IMPL_H
