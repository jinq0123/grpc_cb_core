// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_SERVER_READER_FOR_BIDI_STREAMING_H
#define GRPC_CB_CORE_SERVER_READER_FOR_BIDI_STREAMING_H

#include <memory>  // for shared_ptr<>
#include <string>

#include <grpc_cb_core/common/call_sptr.h>       // for CallSptr
#include <grpc_cb_core/common/support/config.h>  // for GRPC_OVERRIDE
#include <grpc_cb_core/server/server_reader.h>   // for ServerReader

namespace grpc_cb_core {

class ServerWriter;
class Status;

// ServerReader for bidirectional streaming.
// Thread-safe.
class ServerReaderForBidiStreaming : public ServerReader {
 public:
  // Default constructable.
  ServerReaderForBidiStreaming() {}
  virtual ~ServerReaderForBidiStreaming();

 public:
  using Writer = ServerWriter;
  using WriterSptr = std::shared_ptr<Writer>;
  // Start server reader. Called by generated codes.
  void Start(const CallSptr& call_sptr, const WriterSptr& writer_sptr);

 public:
  Writer& GetWriter();

 public:
  void OnError(const Status& status) GRPC_OVERRIDE;

 private:
  WriterSptr writer_sptr_;
};  // class ServerReaderForBidiStreaming

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_SERVER_READER_FOR_BIDI_STREAMING_H
