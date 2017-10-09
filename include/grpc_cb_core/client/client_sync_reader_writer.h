// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_SYNC_READER_WRITER_H
#define GRPC_CB_CORE_CLIENT_SYNC_READER_WRITER_H

#include <cstdint>  // for int64_t
#include <memory>  // for shared_ptr
#include <string>

#include <grpc_cb_core/client/impl/client_sync_reader_writer_impl.h>  // for ClientSyncReaderWriterImpl

namespace grpc_cb_core {

// Copyable.
class ClientSyncReaderWriter GRPC_FINAL {
 public:
  ClientSyncReaderWriter(const ChannelSptr& channel, const std::string& method, int64_t timeout_ms)
      : impl_sptr_(new Impl(channel, method, timeout_ms)) {
    assert(channel);
  }

 public:
  bool Write(const std::string& request) const {
    return impl_sptr_->Write(request);
  }

  // Optional. Writing is auto closed in dtr().
  // Redundant calls are ignored.
  void CloseWriting() {
    impl_sptr_->CloseWriting();
  }

  bool ReadOne(std::string* response) const {
    return impl_sptr_->ReadOne(response);
  }

  Status RecvStatus() const {
    return impl_sptr_->RecvStatus();
  }

  // Set error status to break reading. Such as when parsing message failed.
  void SetErrorStatus(const Status& error_status) {
    assert(!error_status.ok());
    impl_sptr_->SetErrorStatus(error_status);
  }

 private:
  using Impl = ClientSyncReaderWriterImpl;
  const std::shared_ptr<Impl> impl_sptr_;
};  // class ClientSyncReaderWriter

// Todo: SyncGetInitMd();

// Todo: same as ClientReader?

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_CLIENT_CLIENT_SYNC_READER_WRITER_H
