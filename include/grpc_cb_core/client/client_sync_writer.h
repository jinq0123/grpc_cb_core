// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_SYNC_WRITER_H
#define GRPC_CB_CORE_CLIENT_SYNC_WRITER_H

#include <cstdint>  // for int64_t
#include <string>

#include <grpc_cb_core/client/channel_sptr.h>                // for ChannelSptr
#include <grpc_cb_core/common/call_sptr.h>                   // for CallSptr
#include <grpc_cb_core/common/impl/cqueue_for_pluck_sptr.h>  // for CQueueForPluckSptr
#include <grpc_cb_core/common/status.h>                      // for Status
#include <grpc_cb_core/common/support/config.h>              // for GRPC_FINAL

namespace grpc_cb_core {

// Copyable.
class ClientSyncWriter GRPC_FINAL {
 public:
  ClientSyncWriter(const ChannelSptr& channel,
                   const std::string& method,
                   int64_t timeout_ms);

  // Todo: SyncGetInitMd();
  bool Write(const std::string& request) const;
  Status Close(std::string* response) const;

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CQueueForPluckSptr cq4p_sptr;
    CallSptr call_sptr;
    Status status;
  };
  using DataSptr = std::shared_ptr<Data>;

  DataSptr data_sptr_;  // Easy to copy.
};  // class ClientSyncWriter

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_SYNC_WRITER_H
