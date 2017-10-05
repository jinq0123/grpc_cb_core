// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_SERVER_CREDENTIALS_H
#define GRPC_CB_CORE_SERVER_CREDENTIALS_H

struct grpc_server_credentials;

namespace grpc_cb_core {

// Wrapper around \a grpc_server_credentials, a way to authenticate a server.
class ServerCredentials {
 public:
  virtual grpc_server_credentials* c_creds() const { return nullptr; }
};

class InsecureServerCredentials : public ServerCredentials {
};

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_SERVER_CREDENTIALS_H
