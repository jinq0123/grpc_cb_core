#ifndef GRPC_CB_CORE_CLINET_IMPL_CLIENT_SYNC_WRITER_DATA_H
#define GRPC_CB_CORE_CLINET_IMPL_CLIENT_SYNC_WRITER_DATA_H

#include <memory>  // for shared_ptr<>
#include <string>

#include <grpc_cb_core/common/call_sptr.h>  // for CallSptr
#include "common/impl/cqueue_for_pluck_sptr.h"  // for CQueueForPluckSptr
#include <grpc_cb_core/common/status.h>  // for Status

namespace grpc_cb_core {

// Todo: Delete it.

// Wrap all data in shared struct pointer to make copy quick.
struct ClientSyncWriterData {
  CQueueForPluckSptr cq4p_sptr;
  CallSptr call_sptr;
  Status status;
};

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLINET_IMPL_CLIENT_SYNC_WRITER_DATA_H
