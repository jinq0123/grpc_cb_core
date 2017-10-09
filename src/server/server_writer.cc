// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/server/server_writer.h>

#include <cassert>

#include "impl/server_writer_impl.h"  // for ServerWriterImpl

// XXX format

namespace grpc_cb_core {

  ServerWriter::ServerWriter(const CallSptr& call_sptr)
      : impl_sptr_(new ServerWriterImpl(call_sptr)) {
    assert(call_sptr);
  }

  ServerWriter::~ServerWriter() {}

  bool ServerWriter::Write(const std::string& response) const {
    return impl_sptr_->Write(response);
  }
  bool ServerWriter::SyncWrite(const std::string& response) const {
    return impl_sptr_->SyncWrite(response);
  }
  void ServerWriter::AsyncWrite(const std::string& response) const {
    impl_sptr_->AsyncWrite(response);
  }

  size_t ServerWriter::GetQueueSize() const {
    return impl_sptr_->GetQueueSize();
  }
  size_t ServerWriter::GetHighQueueSize() const {
    return impl_sptr_->GetHighQueueSize();
  }
  void ServerWriter::SetHighQueueSize(size_t high_queue_size) {
    impl_sptr_->SetHighQueueSize(high_queue_size);
  }

  // Close() is optional. Dtr() will auto SyncClose().
  // Redundant close will be ignored.
  void ServerWriter::SyncClose(const Status& status) const {
    impl_sptr_->SyncClose(status);
  }
  void ServerWriter::AsyncClose(const Status& status) const {
    impl_sptr_->AsyncClose(status);
  }
  bool ServerWriter::IsClosed() const { return impl_sptr_->IsClosed(); }

}  // namespace grpc_cb_core
