// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CHANNEL_SPTR_H
#define GRPC_CB_CORE_CHANNEL_SPTR_H

#include <memory>  // for shared_ptr

namespace grpc_cb_core {
class Channel;
using ChannelSptr = std::shared_ptr<Channel>;
}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_CHANNEL_SPTR_H
