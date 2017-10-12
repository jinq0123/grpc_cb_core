// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_CORE_SERVER_SERVER_READER_H
#define GRPC_CB_CORE_SERVER_SERVER_READER_H

#include <string>

#include <grpc_cb_core/common/call_sptr.h>  // for CallSptr

namespace grpc_cb_core {

class Status;

// ServerReader is the interface of client streaming handler,
//  for both client-side streaming and bi-directional streaming.
// Thread-safe.
class ServerReader : public std::enable_shared_from_this<ServerReader> {
 public:
  ServerReader() {}
  virtual ~ServerReader() {}

 public:
  virtual void OnMsgStr(const std::string& msg_str) {}  // XXX need to return Status
  virtual void OnError(const Status& status) {}
  virtual void OnEnd() {}

 public:
  // XXX need document...
  void StartForBidiStreaming(const CallSptr& call_sptr);
  void StartForClientSideStreaming(const CallSptr& call_sptr);
};  // class ServerReader

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_SERVER_SERVER_READER_H
