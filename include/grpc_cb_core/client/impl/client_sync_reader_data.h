#ifndef GRPC_CB_CORE_IMPL_CLINET_CLIENT_SYNC_READER_DATA_H
#define GRPC_CB_CORE_IMPL_CLINET_CLIENT_SYNC_READER_DATA_H

#include <memory>  // for shared_ptr<>
#include <string>

#include <grpc_cb_core/client/status_callback.h>  // for StatusCb
#include <grpc_cb_core/common/impl/call_sptr.h>   // for CallSptr
#include <grpc_cb_core/common/impl/cqueue_for_pluck_sptr.h>  // for CQueueForPluckSptr
#include <grpc_cb_core/common/status.h>                      // for Status

namespace grpc_cb_core {

// Todo: Delete it.

// Wrap all data in shared struct pointer to make copy quick.
struct ClientSyncReaderData {
    CQueueForPluckSptr cq4p_sptr;
    CallSptr call_sptr;
    Status status;

    using MsgCallback = std::function<void(const std::string&)>;
    MsgCallback on_msg;
    StatusCb on_status;
};

using ClientSyncReaderDataSptr = std::shared_ptr<ClientSyncReaderData>;

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_IMPL_CLINET_CLIENT_SYNC_READER_DATA_H
