// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_SERVER_REPLIER_H
#define GRPC_CB_CORE_SERVER_REPLIER_H

#include <memory>
#include <string>

#include <grpc_cb_core/common/call_sptr.h>            // for CallSptr

namespace grpc_cb_core {

class ServerReplierImpl;
class Status;

// ServerReplier is for unary rpc call and client-side streaming rpc.
// ServerWriter is for server streaming rpc.
// ServerReader is provided by user for client streaming rpc.

// Copyable. Thread-safe.
// Safe to delete before completion.
// Only accept the 1st reply and ignore other replies.
class ServerReplier {
 public:
  explicit ServerReplier(const CallSptr& call_sptr);
  virtual ~ServerReplier();

 public:
  void Reply(const std::string& response) const;
  void ReplyError(const Status& status) const;

private:
  const std::shared_ptr<ServerReplierImpl> impl_sptr_;  // copyable
};  // class ServerReplier

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_SERVER_REPLIER_H
