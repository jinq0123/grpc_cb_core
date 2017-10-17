// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_ASYNC_READER_H
#define GRPC_CB_CORE_CLIENT_ASYNC_READER_H

#include <cstdint>  // for int64_t
#include <string>

#include <grpc_cb_core/client/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb_core/client/msg_str_cb.h>  // for MsgStrCb
#include <grpc_cb_core/client/status_cb.h>  // for StatusCb
#include <grpc_cb_core/common/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb_core/common/support/config.h>  // for GRPC_FINAL

namespace grpc_cb_core {

class ClientAsyncReaderImpl;

// Copyable. Thread-safe.
class ClientAsyncReader GRPC_FINAL {
 public:
  ClientAsyncReader(const ChannelSptr& channel, const std::string& method,
                    const std::string& request,
                    const CompletionQueueSptr& cq_sptr, int64_t timeout_ms);

 public:
  void ReadEach(const MsgStrCb& msg_cb,
      const StatusCb& status_cb = nullptr) const;

 private:
  const std::shared_ptr<ClientAsyncReaderImpl> impl_sptr_;
};  // class ClientAsyncReader

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_ASYNC_READER_H
