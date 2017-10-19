// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_SEND_CLOSE_CQTAG_H
#define GRPC_CB_CORE_CLIENT_CLIENT_SEND_CLOSE_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb_core/common/support/config.h>  // for GRPC_FINAL
#include "common/impl/call.h"                    // for StartBatch()
#include "common/impl/call_operations.h"         // for CallOperations
#include "common/impl/cqtag/call_cqtag.h"        // for CallCqTag

namespace grpc_cb_core {

class ClientSendCloseCqTag GRPC_FINAL : public CallCqTag {
 public:
  inline explicit ClientSendCloseCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {}
  inline bool Start() GRPC_MUST_USE_RESULT;
};  // class ClientSendCloseCqTag

bool ClientSendCloseCqTag::Start() {
  CallOperations ops;
  ops.ClientSendClose();
  return GetCallSptr()->StartBatch(ops, this);
}

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_CLIENT_CLIENT_SEND_CLOSE_CQTAG_H
