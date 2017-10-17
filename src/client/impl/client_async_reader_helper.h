// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_READER_HELPER_H
#define GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_READER_HELPER_H

#include <functional>
#include <memory>  // for enable_shared_from_this<>
#include <mutex>  // for recursive_mutex

#include <grpc_cb_core/common/call_sptr.h>       // for CallSptr
#include <grpc_cb_core/common/status.h>          // for Status
#include <grpc_cb_core/common/support/config.h>  // for GRPC_FINAL

#include "client_async_read_handler_sptr.h"  // for ClientAsyncReadHandlerSptr
#include "client_async_reader_helper_sptr.h"

namespace grpc_cb_core {

class ClientReaderReadCqTag;

// Thread-safe.
// Used in ClientAsyncReader and ClientAsyncReaderWriter.
class ClientAsyncReaderHelper GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncReaderHelper> {
 public:
  using EndCb = std::function<void()>;
  ClientAsyncReaderHelper(CallSptr call_sptr,
                          const ClientAsyncReadHandlerSptr& read_handler_sptr,
                          const EndCb& end_cb);
  ~ClientAsyncReaderHelper();

 public:
  void Start();
  void Abort();
  const Status GetStatus() const;  // return copy for thread-safety

 public:
  // for ClientReaderReadCqTag::OnComplete()
  void OnRead(bool success, ClientReaderReadCqTag& tag);

 private:
  void Next();

 private:
  // Next() may lock the mutex recursively.
  using Mutex = std::recursive_mutex;
  mutable Mutex mtx_;
  using Guard = std::lock_guard<Mutex>;

  const CallSptr call_sptr_;
  bool aborted_{ false };  // abort reader
  Status status_;
  bool started_{ false };

  ClientAsyncReadHandlerSptr read_handler_sptr_;
  EndCb end_cb_;
};  // ClientAsyncReaderHelper

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_IMPL_CLIENT_ASYNC_READER_HELPER_H
