// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_IMPL_CLIENT_CLIENT_ASYNC_READER_IMPL_H
#define GRPC_CB_CORE_IMPL_CLIENT_CLIENT_ASYNC_READER_IMPL_H

#include <memory>  // for enable_shared_from_this<>
#include <mutex>
#include <string>

#include <grpc_cb_core/client/channel_sptr.h>           // for ChannelSptr
#include <grpc_cb_core/client/status_cb.h>              // for StatusCb
#include <grpc_cb_core/common/call_sptr.h>              // for CallSptr
#include <grpc_cb_core/common/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb_core/common/status.h>                 // for Status
#include <grpc_cb_core/common/support/config.h>         // for GRPC_FINAL
#include "client_async_read_handler_sptr.h"  // for ClientAsyncReadHandlerSptr

namespace grpc_cb_core {

class ClientAsyncReaderHelper;

// Thread-safe.
class ClientAsyncReaderImpl GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncReaderImpl> {
 public:
  ClientAsyncReaderImpl(const ChannelSptr& channel, const std::string& method,
      const std::string& request, const CompletionQueueSptr& cq_sptr,
      int64_t timeout_ms);
  ~ClientAsyncReaderImpl();

 public:
  // ReadHandler must be set before Start().
  void SetReadHandler(const ClientAsyncReadHandlerSptr& handler);
  void SetStatusCb(const StatusCb& status_cb);
  void Start();

  // Todo: Stop reading any more...

 private:
  // Reader callback on end with status.
  void OnEndOfReading();

  void CallStatusCb();

 private:
  // ReaderHelper callback will lock again.
  using Mutex = std::recursive_mutex;
  Mutex mtx_;
  using Guard = std::lock_guard<Mutex>;

  const CallSptr call_sptr_;
  Status status_;
  ClientAsyncReadHandlerSptr read_handler_sptr_;
  StatusCb status_cb_;
  bool set_status_cb_once_ = false;  // set status_cb_ only once

  // ReaderHelper will be shared by CqTag.
  std::shared_ptr<ClientAsyncReaderHelper> reader_sptr_;
  bool reading_ended_ = false;
};  // class ClientAsyncReaderImpl

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_IMPL_CLIENT_CLIENT_ASYNC_READER_IMPL_H
