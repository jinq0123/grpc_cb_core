// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_CORE_CLIENT_SYNC_READER_H
#define GRPC_CB_CORE_CLIENT_SYNC_READER_H

#include <cstdint>  // for int64_t
#include <string>

#include <grpc_cb_core/client/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb_core/client/impl/client_sync_reader_data.h>  // for ClientSyncReaderDataSptr
#include <grpc_cb_core/common/support/config.h>                // for GRPC_FINAL

namespace grpc_cb_core {

class Status;
struct ClientSyncReaderData;

// Copyable. Client sync reader.
class ClientSyncReader GRPC_FINAL {
 public:
  ClientSyncReader(const ChannelSptr& channel, const std::string& method,
                   const std::string& request, int64_t timeout_ms);

 public:
  // Return false if error or end of stream.
  bool ReadOne(std::string* response) const;
  Status RecvStatus() const;

  // Set error status to break reading. Such as when parsing message failed.
  void SetErrorStatus(const Status& error_status);

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  using Data = ClientSyncReaderData;
  using DataSptr = ClientSyncReaderDataSptr;
  DataSptr data_sptr_;
};  // class ClientSyncReader

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_SYNC_READER_H
