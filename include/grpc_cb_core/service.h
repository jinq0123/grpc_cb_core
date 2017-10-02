// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_SERVICE_H
#define GRPC_CB_CORE_SERVICE_H

#include <string>

#include <grpc_cb_core/impl/call_sptr.h>  // for CallSptr
#include <grpc_cb_core/support/grpc_cb_api.h>  // for GRPC_CB_CORE_API
#include <grpc_cb_core/support/protobuf_fwd.h>  // for ServiceDescriptor

struct grpc_byte_buffer;

namespace grpc_cb_core {

// Service base class.
class GRPC_CB_CORE_API Service {
 public:
  const std::string& GetFullName() const;
  size_t GetMethodCount() const;
  bool IsMethodClientStreaming(size_t method_index) const;

 public:
  virtual const std::string& GetMethodName(size_t method_index) const = 0;

  // TODO: need request_context. Need client address in Ctr?
  virtual void CallMethod(size_t method_index, grpc_byte_buffer* request,
                          const CallSptr& call_sptr) = 0;

 private:
  virtual const ::google::protobuf::ServiceDescriptor& GetDescriptor()
      const = 0;
};

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_SERVICE_H
