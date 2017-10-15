// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_WRITER_HELPER_H
#define GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_WRITER_HELPER_H

#include <functional>  // for function<>
#include <memory>  // for enable_shared_from_this<>
#include <mutex>  // for recursive_mutex
#include <queue>
#include <string>

#include <grpc_cb_core/common/call_sptr.h>  // for CallSptr
#include <grpc_cb_core/common/status.h>          // for Status
#include <grpc_cb_core/common/support/config.h>  // for GRPC_FINAL

namespace grpc_cb_core {

// Cache messages and write one by one.
// Thread-safe.
// Used by ClientAsyncWriter and ClientAsyncReaderWriter.
// Differ from ClientAsyncReaderHelper:
//  ReaderHelper is ended by the peer, while WriterHelper is ended by Writer.
//  When Writer is destructed, WriterHelper must be informed that
//    there are no more writing.
//  And WriterHelper must live longer than Writer.
class ClientAsyncWriterHelper GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncWriterHelper> {
 public:
  using EndCb = std::function<void()>;
  ClientAsyncWriterHelper(const CallSptr& call_sptr,
                          const EndCb& end_cb);
  ~ClientAsyncWriterHelper();

 public:
  bool Queue(const std::string& msg);

  // Set the end of messages. Differ with the close.
  // Do not queue further. May trigger end_cb().
  void End();

  void Abort() { aborted_ = true; }  // Abort writing. Stop sending.
  const Status& GetStatus() const { return status_; }

 public:
  void OnWritten(bool success);

 private:
  bool WriteNext();

 private:
  // WriteNext() may lock the mutex recursively.
  using Mutex = std::recursive_mutex;
  mutable Mutex mtx_;
  using Guard = std::lock_guard<Mutex>;

  const CallSptr call_sptr_;
  bool aborted_ = false;  // to abort writer
  const EndCb end_cb_;  // callback on the end
  Status status_;

  std::queue<std::string> msg_queue_;  // Cache messages to write.

  // Grpc only allows to write one by one.
  // When the last msg is writing, the queue is empty, so we need it.
  bool is_writing_ = false;

  // no more msg to queue after End()
  bool is_queue_ended_ = false;
};  // class ClientAsyncWriterHelper

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_WRITER_HELPER_H
