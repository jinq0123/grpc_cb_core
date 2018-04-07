// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CHANNEL_H
#define GRPC_CB_CORE_CHANNEL_H

#include <atomic>  // for atomic<>
#include <memory>  // for unique_ptr<>
#include <string>

#include <grpc_cb_core/client/channel_sptr.h>              // for ChannelSptr
#include <grpc_cb_core/common/call_sptr.h>            // for CallSptr
#include <grpc_cb_core/common/support/grpc_library.h>         // for GrpcLibrary
#include <grpc_cb_core/common/support/config.h>            // for GRPC_OVERRIDE
#include <grpc_cb_core/common/support/grpc_cb_core_api.h>  // for GRPC_CB_CORE_API

struct grpc_channel;
struct grpc_completion_queue;

namespace grpc_cb_core {

class CompletionQueue;

/// Channel represents a connection to an endpoint.
/// Thread-safe.
class GRPC_CB_CORE_API Channel : public GrpcLibrary,
                            public std::enable_shared_from_this<Channel> {
 public:
  explicit Channel(const std::string& target);
  virtual ~Channel() GRPC_OVERRIDE;

 public:
  void SetCallTimeoutMs(int64_t timeout_ms) { call_timeout_ms_ = timeout_ms; }
  // Stub will GetCallTimeoutMs().
  int64_t GetCallTimeoutMs() const { return call_timeout_ms_; }

 public:
  CallSptr MakeSharedCall(const std::string& method, CompletionQueue& cq,
                          int64_t timeout_ms) const;

 private:
  CallSptr MakeSharedCall(const std::string& method, CompletionQueue& cq,
                          const gpr_timespec& deadline) const;

 private:
  const std::unique_ptr<grpc_channel, void (*)(grpc_channel*)>
      c_channel_uptr_;  // owned
  std::atomic<int64_t> call_timeout_ms_{ INT64_MAX };
};

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_CHANNEL_H
