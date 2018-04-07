# grpc_cb_core

C++ [gRPC](http://www.grpc.io/) core library with callback interface.
It depends on grpc but not grpc++ nor protobuf.
Used by [grpc_cb](https://github.com/jinq0123/grpc_cb) library
 and [grpc-lua](https://github.com/jinq0123/grpc-lua).
Using this lib,
 it is possible to make grpc library using an IDL other than protobuf,
 or to bind a script language other than lua. 

## Build

### Build with conan
1. Install [conan](http://docs.conan.io/en/latest/installation.html).
1. Add conan repositories:
    + `conan remote add remote_bintray_conan-community https://api.bintray.com/conan/conan-community/conan`
    + `conan remote add remote_bintray_bincrafters https://api.bintray.com/conan/bincrafters/public-conan`
    + `conan remote add remote_bintray_inexorgame https://api.bintray.com/conan/inexorgame/inexor-conan`
    + `conan remote add remote_bintray_conan https://api.bintray.com/conan/conan/conan-transit`
    + `conan remote add remote_bintray_jinq0123 https://api.bintray.com/conan/jinq0123/test`
1. `conan create user/channel --build missing`
    * The result `grpc_cb_core.lib` is in `~/.conan/data/grpc_cb_core/0.1/user/channel/package/`...

### VS solution
See [premake/README.md](premake/README.md) to use premake5 to generate VS solution.

## Not supported yet
* Compression
* Security
* Metadata
