// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/client/client_async_reader.h>

#include <cstdint>  // for int64_t
#include <string>

#include <grpc_cb_core/client/channel_sptr.h>  // for ChannelSptr
#include "impl/client_async_read_handler.h"  // for ClientAsyncReadHandler
#include "impl/client_async_reader_impl.h"  // for ClientAsyncReaderImpl
#include <grpc_cb_core/client/msg_str_cb.h>  // for MsgStrCb
#include <grpc_cb_core/client/status_cb.h>  // for StatusCb
#include <grpc_cb_core/common/completion_queue_sptr.h>  // for CompletionQueueSptr

namespace grpc_cb_core {

ClientAsyncReader::ClientAsyncReader(const ChannelSptr& channel,
                                     const std::string& method,
                                     const std::string& request,
                                     const CompletionQueueSptr& cq_sptr,
                                     int64_t timeout_ms)
    : impl_sptr_(new ClientAsyncReaderImpl(channel, method, request, cq_sptr,
                                           timeout_ms)) {}

void ClientAsyncReader::ReadEach(
    const MsgStrCb& msg_cb, const StatusCb& status_cb /*= StatusCb()*/) const {
  auto handler_sptr = std::make_shared<ClientAsyncReadHandler>(msg_cb);
  impl_sptr_->SetReadHandler(handler_sptr);
  impl_sptr_->SetStatusCb(status_cb);
  impl_sptr_->Start();
}

}  // namespace grpc_cb_core
