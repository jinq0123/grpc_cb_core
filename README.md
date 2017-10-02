# gRPC_cb_core
C++ [gRPC](http://www.grpc.io/) core library with callback interface. Depend on grpc but not on grpc++ nor no protobuf.

## Build

### Build with conan
1. Install [conan](http://docs.conan.io/en/latest/installation.html).
1. `conan remote add remote_bintray_jinq0123 https://api.bintray.com/conan/jinq0123/test`
1. `conan create user/channel --build missing`
    * The result `grpc_cb.lib` is in `~/.conan/data/grpc_cb/0.1/user/channel/package/`...

### VS solution
See [premake/README.md](premake/README.md) to use premake5 to generate VS solution.

## Todo
1. Lua binding. Convert grpc_byte_buffer to string, which is needed by lua.
1. Connect and disconnect event.
1. Export for unity.
1. Compression
1. Security
1. Metadata
1. Support message types other than protobuffer.

