#ifndef GRPC_CB_CORE_COMMON_BYTE_BUFFER_TO_STRING_H
#define GRPC_CB_CORE_COMMON_BYTE_BUFFER_TO_STRING_H

#include <string>

struct grpc_byte_buffer;

namespace grpc_cb_core {

bool ByteBufferToString(grpc_byte_buffer& from, std::string& to);

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_COMMON_BYTE_BUFFER_TO_STRING_H
