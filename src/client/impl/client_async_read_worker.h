// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_READ_WORKER_H
#define GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_READ_WORKER_H

#include <functional>
#include <memory>  // for enable_shared_from_this<>
#include <mutex>  // for recursive_mutex

#include <grpc_cb_core/client/msg_str_cb.h>      // for MsgStrCb
#include <grpc_cb_core/common/call_sptr.h>       // for CallSptr
#include <grpc_cb_core/common/status.h>          // for Status
#include <grpc_cb_core/common/support/config.h>  // for GRPC_FINAL

namespace grpc_cb_core {

class ClientReaderReadCqTag;

// Thread-safe.
// Used in ClientAsyncReader and ClientAsyncReaderWriter.
class ClientAsyncReadWorker GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncReadWorker> {
 public:
  using EndCb = std::function<void()>;
  ClientAsyncReadWorker(CallSptr call_sptr,
                          const MsgStrCb& msg_cb,
                          const EndCb& end_cb);
  ~ClientAsyncReadWorker();

 public:
  void Start();
  void Abort();
  const Status GetStatus() const;  // return copy for thread-safety

 public:
  // for ClientReaderReadCqTag::OnComplete()
  void OnRead(bool success, ClientReaderReadCqTag& tag);

 private:
  void Next();
  void CallEndCb();

 private:
  // OnRead() may call other member functions.
  using Mutex = std::recursive_mutex;
  mutable Mutex mtx_;
  using Guard = std::lock_guard<Mutex>;

  const CallSptr call_sptr_;
  bool aborted_{ false };  // abort reader
  Status status_;
  bool started_{ false };

  MsgStrCb msg_cb_;
  EndCb end_cb_;
};  // ClientAsyncReadWorker

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_READ_WORKER_H
