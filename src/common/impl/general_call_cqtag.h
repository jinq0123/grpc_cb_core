// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_IMPL_GENERAL_CALL_CQTAG_H
#define GRPC_CB_CORE_IMPL_GENERAL_CALL_CQTAG_H

#include <cassert>  // for assert()
#include <functional>  // for function<>

#include "call_cqtag.h"  // for CallCqTag
#include <grpc_cb_core/common/support/config.h>  // for GRPC_OVERRIDE

namespace grpc_cb_core {

// Call completion queue tag with callback on completion.
class GeneralCallCqTag : public CallCqTag {
 public:
  explicit GeneralCallCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {
    assert(call_sptr);
  }

 public:
  // Callback on completion
  using CompleteCb = std::function<void (bool success)>;
  void SetCompleteCb(const CompleteCb& complete_cb) {
    complete_cb_ = complete_cb;
  }

  void DoComplete(bool success) GRPC_OVERRIDE {
    if (complete_cb_)
      complete_cb_(success);
  }

 private:
  CompleteCb complete_cb_;
};  // class GeneralCallCqTag

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_IMPL_GENERAL_CALL_CQTAG_H
