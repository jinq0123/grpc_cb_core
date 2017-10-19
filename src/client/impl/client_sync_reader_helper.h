// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_IMPL_CLIENT_CLIENT_SYNC_READER_HELPER_H
#define GRPC_CB_CORE_IMPL_CLIENT_CLIENT_SYNC_READER_HELPER_H

#include <cassert>     // for assert()
#include <string>

#include <grpc_cb_core/common/call_sptr.h>          // for CallSptr
#include <grpc_cb_core/common/status.h>             // for Status
#include "common/impl/cqueue_for_pluck.h"           // for Pluck()
#include "cqtag/client_reader_read_cqtag.h"         // for ClientReaderReadCqTag
#include "cqtag/client_reader_recv_status_cqtag.h"  // for ClientReaderRecvStatusCqTag

namespace grpc_cb_core {
namespace ClientSyncReaderHelper {
// ClientSyncReaderHelper is used in ClientSyncReader and ClientSyncReaderWriter.

inline bool SyncReadOne(
    const CallSptr& call_sptr,
    const CQueueForPluckSptr& cq4p_sptr,
    std::string& response,
    Status& status);

inline Status SyncRecvStatus(
    const CallSptr& call_sptr,
    const CQueueForPluckSptr& cq4p_sptr);

// Todo: move to cpp file.

inline bool SyncReadOne(
    const CallSptr& call_sptr,
    const CQueueForPluckSptr& cq4p_sptr,
    std::string& response,
    Status& status) {
  if (!status.ok()) return false;

  ClientReaderReadCqTag tag(call_sptr);
  if (!tag.Start()) {
    status.SetInternalError("Failed to read server-side streaming.");
    return false;
  }

  // tag.Start() has queued the tag. Wait for completion.
  cq4p_sptr->Pluck(&tag);
  if (!tag.HasGotMsg())
      return false;  // Need to set EndOfStream?
  status = tag.GetResultMsg(response);
  return status.ok();
}

inline Status SyncRecvStatus(
    const CallSptr& call_sptr,
    const CQueueForPluckSptr& cq4p_sptr) {
  assert(call_sptr);
  assert(cq4p_sptr);
  ClientReaderRecvStatusCqTag tag(call_sptr);
  if (!tag.Start())
      return Status::InternalError("Failed to receive status.");
  cq4p_sptr->Pluck(&tag);
  return tag.GetStatus();
}

}  // namespace ClientSyncReaderHelper
}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_IMPL_CLIENT_CLIENT_SYNC_READER_HELPER_H
