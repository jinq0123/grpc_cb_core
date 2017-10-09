// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/client/client_sync_reader_writer.h>

#include "impl/client_sync_reader_writer_impl.h"  // for ClientSyncReaderWriterImpl

namespace grpc_cb_core {

ClientSyncReaderWriter::ClientSyncReaderWriter(const ChannelSptr& channel,
                                               const std::string& method,
                                               int64_t timeout_ms)
    : impl_sptr_(new Impl(channel, method, timeout_ms)) {
  assert(channel);
}

bool ClientSyncReaderWriter::Write(const std::string& request) const {
  return impl_sptr_->Write(request);
}

// Optional. Writing is auto closed in dtr().
// Redundant calls are ignored.
void ClientSyncReaderWriter::CloseWriting() { impl_sptr_->CloseWriting(); }

bool ClientSyncReaderWriter::ReadOne(std::string* response) const {
  return impl_sptr_->ReadOne(response);
}

Status ClientSyncReaderWriter::RecvStatus() const {
  return impl_sptr_->RecvStatus();
}

// Set error status to break reading. Such as when parsing message failed.
void ClientSyncReaderWriter::SetErrorStatus(const Status& error_status) {
  assert(!error_status.ok());
  impl_sptr_->SetErrorStatus(error_status);
}

// Todo: SyncGetInitMd();
// Todo: same as ClientReader?

}  // namespace grpc_cb_core
