// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_WRITE_WWORKER_H
#define GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_WRITE_WWORKER_H

#include <functional>  // for function<>
#include <mutex>  // for recursive_mutex
#include <queue>
#include <string>

#include <grpc_cb_core/common/call_sptr.h>  // for CallSptr
#include <grpc_cb_core/common/status.h>          // for Status
#include <grpc_cb_core/common/support/config.h>  // for GRPC_FINAL

namespace grpc_cb_core {

// Cache messages and write one by one.
// Thread-safe. XXX
// Used by ClientAsyncWriter and ClientAsyncReaderWriter.
//
// Differ from ClientAsyncReadWorker: XXX
//  ReadWorker is ended by the peer, while WriteWorker is ended by Writer.
//  When Writer is destructed, WriteWorker must be informed that
//    there are no more writing.
//  WriteWorker may live longer than Writer.
class ClientAsyncWriteWorker GRPC_FINAL {
 public:
  using WriteCb = std::function<void()>;
  ClientAsyncWriteWorker(const CallSptr& call_sptr,
                          const WriteCb& write_cb);
  ~ClientAsyncWriteWorker();

 public:
  bool Queue(const std::string& msg);

  // Set the end of messages. Differ with the close.
  // Do not queue further. May trigger end_cb().
  void SetClosing();

  void Abort();  // Abort writing. Stop sending.
  const Status GetStatus() const;  // return copy for thread-safety

 public:
  void OnWritten(bool success);

 private:
  bool WriteNext();
  void CallEndCb();

 private:
  // OnWritten() may lock the mutex recursively.
  using Mutex = std::recursive_mutex;
  mutable Mutex mtx_;
  using Guard = std::lock_guard<Mutex>;

  const CallSptr call_sptr_;
  bool aborted_ = false;  // to abort writer
  const WriteCb write_cb_;
  // XXX EndCb end_cb_;  // callback on the end
  Status status_;

  std::queue<std::string> msg_queue_;  // Cache messages to write.

  // Grpc only allows to write one by one.
  // When the last msg is writing, the queue is empty, so we need it.
  bool is_writing_ = false;

  // no more msg to queue after SetClosing()
  bool is_closing_ = false;
};  // class ClientAsyncWriteWorker

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_WRITE_WWORKER_H
