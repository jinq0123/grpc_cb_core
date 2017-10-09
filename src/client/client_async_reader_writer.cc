// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/client/client_async_reader_writer.h>

#include <cassert>
#include <cstdint>  // for int64_t
#include <string>

#include <grpc_cb_core/client/impl/client_async_read_handler.h>  // for ClientAsyncReadHandler
#include <grpc_cb_core/client/impl/client_async_reader_writer_impl.h>  // for ClientAsyncReaderWriterImpl
#include <grpc_cb_core/client/msg_str_cb.h>      // for MsgStrCb
#include <grpc_cb_core/client/status_cb.h>       // for StatusCb

namespace grpc_cb_core {

// Todo: Move status_cb to Set()
ClientAsyncReaderWriter::ClientAsyncReaderWriter(
    const ChannelSptr& channel, const std::string& method,
    const CompletionQueueSptr& cq_sptr, int64_t timeout_ms,
    const StatusCb& status_cb/* = StatusCb()*/)
    : impl_sptr_(new Impl(channel, method, cq_sptr, timeout_ms, status_cb)) {
  assert(cq_sptr);
  assert(channel);
}

bool ClientAsyncReaderWriter::Write(const std::string& request) const {
  return impl_sptr_->Write(request);
}

void ClientAsyncReaderWriter::CloseWriting() { impl_sptr_->CloseWriting(); }

void ClientAsyncReaderWriter::ReadEach(const MsgStrCb& msg_cb) {
  auto handler_sptr = std::make_shared<ClientAsyncReadHandler>(msg_cb);
  impl_sptr_->ReadEach(handler_sptr);
}

// Set error status to break reading. Such as when parsing message failed.
void ClientAsyncReaderWriter::SetErrorStatus(const Status& error_status) {
  assert(!error_status.ok());
  impl_sptr_->SetErrorStatus(error_status);
}

}  // namespace grpc_cb_core
