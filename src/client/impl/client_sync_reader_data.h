#ifndef GRPC_CB_CORE_CLINET_IMPL_CLIENT_SYNC_READER_DATA_H
#define GRPC_CB_CORE_CLINET_IMPL_CLIENT_SYNC_READER_DATA_H

#include <string>

#include <grpc_cb_core/client/msg_str_cb.h>  // for MsgStrCb
#include <grpc_cb_core/client/status_cb.h>  // for StatusCb
#include <grpc_cb_core/common/call_sptr.h>   // for CallSptr
#include <grpc_cb_core/common/impl/cqueue_for_pluck_sptr.h>  // for CQueueForPluckSptr
#include <grpc_cb_core/common/status.h>                      // for Status

namespace grpc_cb_core {

// Todo: Delete it.

// Wrap all data in shared struct pointer to make copy quick.
struct ClientSyncReaderData {
    CQueueForPluckSptr cq4p_sptr;
    CallSptr call_sptr;
    Status status;

    MsgStrCb msg_cb;
    StatusCb status_cb;
};

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLINET_IMPL_CLIENT_SYNC_READER_DATA_H
