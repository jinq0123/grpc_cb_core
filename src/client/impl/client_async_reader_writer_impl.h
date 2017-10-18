// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H
#define GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H

#include <string>

#include <grpc_cb_core/client/channel_sptr.h>           // for ChannelSptr
#include <grpc_cb_core/client/msg_str_cb.h>             // for MsgStrCb
#include <grpc_cb_core/client/status_cb.h>              // for StatusCb
#include <grpc_cb_core/common/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb_core/common/support/config.h>         // for GRPC_FINAL

namespace grpc_cb_core {

class ClientAsyncReaderWriterImpl2;
class Status;

// Thread-safe.
// Only shared in ClientAsyncReaderWriter, because we need dtr() to close writing.
class ClientAsyncReaderWriterImpl GRPC_FINAL {
 public:
  ClientAsyncReaderWriterImpl(const ChannelSptr& channel,
                              const std::string& method,
                              const CompletionQueueSptr& cq_sptr,
                              int64_t timeout_ms,
                              const StatusCb& status_cb);
  ~ClientAsyncReaderWriterImpl();

 public:
  bool Write(const std::string& msg);
  // CloseWriting() is optional. Auto closed on dtr().
  void CloseWriting();

  // Todo: Force to close reading/writing. Cancel all reading/writing.

  void ReadEach(const MsgStrCb& msg_cb);

  // Set error status to break reading. Such as when parsing message failed.
  void SetErrorStatus(const Status& error_status);

 private:
  // Live longer than ClientAsyncReaderWriter.
  std::shared_ptr<ClientAsyncReaderWriterImpl2> impl2_sptr_;
  // XXX Do we really need Impl and Impl2?
};  // class ClientAsyncReaderWriterImpl

// Todo: SyncGetInitMd();

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H
