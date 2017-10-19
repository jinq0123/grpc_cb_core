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
#include "common/impl/complete_cb.h"  // for CompleteCb

namespace grpc_cb_core {

// Cache messages and write one by one.
// Used by ClientAsyncWriter and ClientAsyncReaderWriter.
//
// Differ from ClientAsyncReadWorker: XXX
//  ReadWorker is ended by the peer, while WriteWorker is ended by Writer.
//  When Writer is destructed, WriteWorker must be informed that
//    there are no more writing.
//  WriteWorker may live longer than Writer.
class ClientAsyncWriteWorker GRPC_FINAL {
 public:
  explicit ClientAsyncWriteWorker(const CallSptr& call_sptr);
  ~ClientAsyncWriteWorker();

 public:
  bool Queue(const std::string& msg);

  // Set the end of messages. Differ with the close.
  // Do not queue further. May trigger end_cb().
  void SetClosing();

  void Abort();  // Abort writing. Stop sending.
  const Status GetStatus() const;  // return copy for thread-safety

  bool IsWriting() const { return false; }  // XXX

 public:
  void OnWritten(bool success);

 public:
  bool WriteNext(const CompleteCb& complete_cb);

 private:
  void CallEndCb();

 private:
  const CallSptr call_sptr_;
  bool aborted_ = false;  // to abort writer
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
