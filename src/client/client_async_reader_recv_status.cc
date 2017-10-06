// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_recv_status.h"

#include <cassert>

#include <grpc_cb_core/client/impl/client_reader_async_recv_status_cqtag.h>  // for ClientReaderAsyncRecvStatusCqTag
#include <grpc_cb_core/common/status.h>  // for Status

namespace grpc_cb_core {
namespace ClientAsyncReader {

void RecvStatus(const CallSptr& call_sptr, const StatusCb& status_cb) {
  assert(call_sptr);

  auto* tag = new ClientReaderAsyncRecvStatusCqTag(call_sptr);
  tag->SetStatusCb(status_cb);
  if (tag->Start()) return;

  delete tag;
  if (status_cb)
    status_cb(Status::InternalError("Failed to receive status."));
}

}  // namespace ClientAsyncReader
}  // namespace grpc_cb_core
