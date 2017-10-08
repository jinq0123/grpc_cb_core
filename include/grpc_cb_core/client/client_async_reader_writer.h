// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_ASYNC_READER_WRITER_H
#define GRPC_CB_CORE_CLIENT_ASYNC_READER_WRITER_H

#include <cassert>
#include <cstdint>  // for int64_t
#include <string>

#include <grpc_cb_core/client/impl/client_async_read_handler.h>  // for ClientAsyncReadHandler
#include <grpc_cb_core/client/impl/client_async_reader_writer_impl.h>  // for ClientAsyncReaderWriterImpl
#include <grpc_cb_core/client/msg_str_cb.h>  // for MsgStrCb
#include <grpc_cb_core/client/status_cb.h>  // for StatusCb
#include <grpc_cb_core/common/support/config.h>   // for GRPC_FINAL

namespace grpc_cb_core {

// Copyable. Thread-safe.
class ClientAsyncReaderWriter GRPC_FINAL {
 public:
  // Todo: Move status_cb to Set()
  ClientAsyncReaderWriter(const ChannelSptr& channel, const std::string& method,
                          const CompletionQueueSptr& cq_sptr,
                          int64_t timeout_ms,
                          const StatusCb& status_cb = StatusCb())
      : impl_sptr_(new Impl(channel, method, cq_sptr, timeout_ms, status_cb)) {
    assert(cq_sptr);
    assert(channel);
  }

 public:
  bool Write(const std::string& request) const {
    return impl_sptr_->Write(request);
  }

  // Optional. Writing is auto closed in dtr().
  // Redundant calls are ignored.
  void CloseWriting() {
    impl_sptr_->CloseWriting();
  }

  void ReadEach(const MsgStrCb& msg_cb) {
    auto handler_sptr = std::make_shared<ClientAsyncReadHandler>(msg_cb);
    impl_sptr_->ReadEach(handler_sptr);
  }

 private:
  using Impl = ClientAsyncReaderWriterImpl;
  const std::shared_ptr<Impl> impl_sptr_;
};  // class ClientAsyncReaderWriter

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_CLIENT_ASYNC_READER_WRITER_H
