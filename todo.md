# Todo

* WriteOptions
* Handle ServerReplier, ServerReader ServerWriter likely.

* Extract super class from ServerReplier and ServerWriter to Close() on error.
  Then ServerReaderForBidiSteaming and ServerReaderForClientSideStream can merge into one.
  
* Move CqTag files into subdir.  
* Rename Impl2 to Impl, Impl to WrappedImpl

* Rename worker to handler
