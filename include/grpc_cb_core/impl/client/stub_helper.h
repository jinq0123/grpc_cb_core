// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_IMPL_CLIENT_STUB_HELPER_H
#define GRPC_CB_CORE_IMPL_CLIENT_STUB_HELPER_H

#include <string>

#include <grpc_cb_core/service_stub.h>  // for ServiceStub
#include <grpc_cb_core/common/status.h>  // for Status
#include <grpc_cb_core/client/status_callback.h>  // for ErrorCallback
#include <grpc_cb_core/impl/client/wrap_response_callback.h>  // for WrapResponseCallback()

namespace grpc_cb_core {

// Helper to request stub.
class StubHelper {
public:
    StubHelper(ServiceStub& stub) : stub_(stub) {}

public:
    using string = std::string;

    inline Status SyncRequest(const string& method,
        const string& request, string* response);

    inline void AsyncRequest(const string& method,
        const string& request,
        const std::function<void (const string&)>& cb,
        const ErrorCallback& ecb);

private:
    ServiceStub& stub_;
};  // StubHelper

Status StubHelper::SyncRequest(const string& method,
    const string& request, string* response) {
  if (response)
    return stub_.SyncRequest(method, request, *response);

  string _;
  return stub_.SyncRequest(method, request, _);
}

void StubHelper::AsyncRequest(const string& method,
    const string& request,
    const std::function<void (const string&)>& cb,
    const ErrorCallback& ecb) {
  stub_.AsyncRequest(method, request, cb, ecb);
}

// XXX StubHelper is not necessary?

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_IMPL_CLIENT_STUB_HELPER_H
