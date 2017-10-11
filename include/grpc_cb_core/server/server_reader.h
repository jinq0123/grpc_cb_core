// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_SERVER_READER_H
#define GRPC_CB_CORE_SERVER_READER_H

#include <string>
#include <memory>

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
  virtual void OnMsgStr(const std::string& msg_str) {}
  virtual void OnError(const Status& status) {}
  virtual void OnEnd() {}
};  // class ServerReader

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_SERVER_READER_H
