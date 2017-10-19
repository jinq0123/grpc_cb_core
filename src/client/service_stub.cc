// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb_core/client/service_stub.h>

#include <grpc_cb_core/client/channel.h>  // for GetCallTimeoutMs()
#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()
#include <grpc_cb_core/common/completion_queue_for_next.h>  // for CompletionQueueForNext
#include <grpc_cb_core/common/completion_queue_for_next.h>  // to convert *cq4n_sptr_ to CompletionQueue in MakeSharedCall()
#include <grpc_cb_core/common/run.h>  // for Run()
#include "common/impl/call.h"
#include "common/impl/cqueue_for_pluck.h"        // for CQueueForPluck
#include "impl/cqtag/client_async_call_cqtag.h"  // for ClientAsyncCallCqTag
#include "impl/cqtag/client_call_cqtag.h"        // for ClientCallCqTag

namespace grpc_cb_core {

// default empty error callback
ErrorCb ServiceStub::default_error_cb_;

ServiceStub::ServiceStub(const ChannelSptr& channel_sptr,
    const CompletionQueueForNextSptr& cq4n_sptr/* = nullptr */)
    : channel_sptr_(channel_sptr),  // copy shared_ptr
    cq4n_sptr_(cq4n_sptr),
    error_cb_(default_error_cb_),
    call_timeout_ms_(channel_sptr->GetCallTimeoutMs()) {
  assert(channel_sptr);
  // Use an internal cq if no external cq.
  if (!cq4n_sptr_)
    cq4n_sptr_.reset(new CompletionQueueForNext);
}

ServiceStub::~ServiceStub() {
  assert(cq4n_sptr_);
}

Status ServiceStub::SyncRequest(const string& method, const string& request,
                                    string& response) {
  CQueueForPluck cq4p;
  CallSptr call_sptr(MakeSharedCall(method, cq4p));
  ClientCallCqTag tag(call_sptr);
  if (!tag.Start(request))
    return Status::InternalError("Failed to request.");
  cq4p.Pluck(&tag);
  return tag.GetResponse(response);
}

void ServiceStub::AsyncRequest(const string& method, const string& request,
                               const RespStrCb& response_cb,
                               const ErrorCb& error_cb) {
  CallSptr call_sptr(MakeSharedCall(method));
  using CqTag = ClientAsyncCallCqTag;
  CqTag* tag = new CqTag(call_sptr);
  tag->SetResponseCb(response_cb);
  tag->SetErrorCb(error_cb);
  if (tag->Start(request))
    return;

  delete tag;
  if (error_cb)
    error_cb(Status::InternalError("Failed to async request."));
}

// Blocking run stub.
void ServiceStub::Run() {
  assert(cq4n_sptr_);
  grpc_cb_core::Run(cq4n_sptr_);
}

void ServiceStub::Shutdown() {
  assert(cq4n_sptr_);
  cq4n_sptr_->Shutdown();
}

CallSptr ServiceStub::MakeSharedCall(const string& method) const {
  assert(cq4n_sptr_);
  return MakeSharedCall(method, *cq4n_sptr_);
}

CallSptr ServiceStub::MakeSharedCall(const string& method, CompletionQueue& cq) const {
  return GetChannel().MakeSharedCall(method, cq, GetCallTimeoutMs());
}

}  // namespace grpc_cb_core

