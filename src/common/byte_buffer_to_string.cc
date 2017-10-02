#include "byte_buffer_to_string.h"

#include <grpc_cb_core/status.h>  // for Status

#include <grpc/byte_buffer_reader.h>  // for grpc_byte_buffer_reader
#include <grpc/byte_buffer.h>  // for grpc_byte_buffer_reader_init()
#include <grpc_cb_core/support/slice.h>  // for StringFromCopiedSlice()

namespace grpc_cb_core {

Status ByteBufferToString(grpc_byte_buffer& from, std::string& to) {
  grpc_byte_buffer_reader reader;
  if (!grpc_byte_buffer_reader_init(&reader, &from)) {
    return Status::InternalError("Couldn't initialize byte buffer reader");
  }
  grpc_slice slice = grpc_byte_buffer_reader_readall(&reader);
  to = StringFromCopiedSlice(slice);
  grpc_slice_unref(slice);
  grpc_byte_buffer_reader_destroy(&reader);
  return Status::OK;
}

}  // namespace grpc_cb_core

