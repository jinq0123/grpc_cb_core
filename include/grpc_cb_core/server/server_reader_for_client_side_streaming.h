// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_CORE_sERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H
#define GRPC_CB_CORE_sERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H

#include <memory>  // for unique_ptr<>
#include <string>

#include <grpc_cb_core/common/call_sptr.h>       // for CallSptr
#include <grpc_cb_core/common/support/config.h>  // for GRPC_OVERRIDE
#include <grpc_cb_core/server/server_reader.h>   // for ServerReader

namespace grpc_cb_core {

class ServerReplier;
class Status;

// ServerReader for client-side streaming.
// Thread-safe.
class ServerReaderForClientSideStreaming : public ServerReader {
 public:
  // Default constructable.
  ServerReaderForClientSideStreaming() {}
  virtual ~ServerReaderForClientSideStreaming();

 public:
  using Replier = ServerReplier;
  // Start server reader. Called by generated codes.
  void Start(const CallSptr& call_sptr, const Replier& replier);

 public:
  void Reply(const std::string& response);
  void ReplyError(const Status& status);
  Replier& GetReplier();

 public:
  void OnMsg(const std::string& msg) GRPC_OVERRIDE {}
  void OnError(const Status& status) GRPC_OVERRIDE;
  void OnEnd() GRPC_OVERRIDE {}

 private:
  std::unique_ptr<Replier> replier_uptr_;
};  // class ServerReaderForClientSideStreaming

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_sERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H
