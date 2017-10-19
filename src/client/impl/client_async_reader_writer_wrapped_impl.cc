// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_writer_impl.h"

#include <cassert>

#include "client_async_reader_writer_impl2.h"  // for ClientAsyncReaderWriterImpl2

namespace grpc_cb_core {

// Todo: SyncGetInitMd();

ClientAsyncReaderWriterWrappedImpl::ClientAsyncReaderWriterWrappedImpl(
    const ChannelSptr& channel, const std::string& method,
    const CompletionQueueSptr& cq_sptr, int64_t timeout_ms,
    const StatusCb& status_cb)
    : impl2_sptr_(new ClientAsyncReaderWriterImpl2(channel, method, cq_sptr,
                                                   timeout_ms, status_cb)) {
  assert(cq_sptr);
}

ClientAsyncReaderWriterWrappedImpl::~ClientAsyncReaderWriterWrappedImpl() {
  impl2_sptr_->CloseWriting();  // impl2_sptr_ will live on.
}

bool ClientAsyncReaderWriterWrappedImpl::Write(const std::string& msg) {
  return impl2_sptr_->Write(msg);
}

void ClientAsyncReaderWriterWrappedImpl::CloseWriting() {
  impl2_sptr_->CloseWriting();
}

void ClientAsyncReaderWriterWrappedImpl::ReadEach(const MsgStrCb& msg_cb) {
  impl2_sptr_->ReadEach(msg_cb);
}

void ClientAsyncReaderWriterWrappedImpl::SetErrorStatus(const Status& error_status) {
  assert(!error_status.ok());
  impl2_sptr_->SetErrorStatus(error_status);
}

}  // namespace grpc_cb_core
