# How to implement client streaming rpc service?

Client streaming is client-side streaming or bi-directional streaming.

To implement a client streaming rpc service,
 user should implement a subclass of `grpc_cb_core::ServerReader`.

Make a shared `ServerReader` subclass and start the service by
`StartForClientSideStreaming()` or `StartForBidiStreaming()`.
