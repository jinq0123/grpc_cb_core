// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_impl2.h"

#include <cassert>  // for assert()

#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()

#include "client_async_writer_close_handler.h"  // for OnClose()
#include "client_async_writer_helper.h"         // for ClientAsyncWriterHelper
#include "client_send_init_md_cqtag.h"          // for ClientSendInitMdCqTag
#include "client_writer_close_cqtag.h"          // for ClientWriterCloseCqTag

namespace grpc_cb_core {

ClientAsyncWriterImpl2::ClientAsyncWriterImpl2(
    const ChannelSptr& channel, const std::string& method,
    const CompletionQueueSptr& cq_sptr, int64_t timeout_ms)
    : cq_sptr_(cq_sptr),
      call_sptr_(channel->MakeSharedCall(method, *cq_sptr, timeout_ms)) {
  assert(cq_sptr);
  assert(channel);
  ClientSendInitMdCqTag* tag = new ClientSendInitMdCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  SetInternalError("Failed to init client stream.");
  // Call close handler when Close(CloseHandler)
}

ClientAsyncWriterImpl2::~ClientAsyncWriterImpl2() {
  // Have done CallCloseHandler().
}

bool ClientAsyncWriterImpl2::Write(const std::string& request) {
  Guard g(mtx_);
  if (!status_.ok())
    return false;
  if (writing_ended_)
    return false;
  if (writer_sptr_)
    return writer_sptr_->Queue(request);

  // Impl2 and WriterHelper shared each other untill OnEndOfWriting().
  auto sptr = shared_from_this();
  writer_sptr_.reset(new ClientAsyncWriterHelper(call_sptr_,
      [sptr]() {
        auto p2 = sptr;
        p2->OnEndOfWriting();  // will delete this function<>
        // sptr is invalid now
      }));
  return writer_sptr_->Queue(request);
}  // Write()

void ClientAsyncWriterImpl2::Close(const CloseHandlerSptr& handler_sptr) {
  Guard g(mtx_);

  if (close_handler_set_) return;  // already done
  close_handler_set_ = true;
  close_handler_sptr_ = handler_sptr;  // reset after CallCloseHandler()

  if (!status_.ok()) {
    CallCloseHandler();
    return;
  }

  if (writer_sptr_) {
    writer_sptr_->SetClosing();  // May trigger OnEndOfWriting().
  } else {
    writing_ended_ = true;  // Ended without start.
    SendCloseIfNot();
  }
}  // Close()

// Finally close...
void ClientAsyncWriterImpl2::SendCloseIfNot() {
  assert(writing_ended_);  // Must be ended.
  if (!status_.ok())
    return;

  if (has_sent_close_) return;
  has_sent_close_ = true;
  auto sptr = shared_from_this();
  auto* tag = new ClientWriterCloseCqTag(call_sptr_);
  tag->SetCompleteCb([sptr, tag](bool success) {
    sptr->OnClosed(success, *tag);
  });
  if (tag->Start())
    return;

  delete tag;
  SetInternalError("Failed to close client stream.");  // Calls CallCloseHandler();
}  // SendCloseIfNot()

void ClientAsyncWriterImpl2::CallCloseHandler() {
  if (!close_handler_sptr_) return;
  close_handler_sptr_->OnClose(status_);
  close_handler_sptr_.reset();
}

// Callback of ClientWriterCloseCqTag::OnComplete()
void ClientAsyncWriterImpl2::OnClosed(bool success, ClientWriterCloseCqTag& tag) {
  Guard g(mtx_);

  // Todo: Get trailing metadata.
  if (tag.IsStatusOk()) {
    if (close_handler_sptr_) {
      status_ = tag.GetResponse(close_handler_sptr_->GetMsg());
    } else {
      status_.SetInternalError("Response is ignored.");
    }
  } else {
    status_ = tag.GetStatus();
  }

  CallCloseHandler();
}  // OnClosed()

void ClientAsyncWriterImpl2::OnEndOfWriting() {
  Guard g(mtx_);
  assert(writer_sptr_);
  if (writing_ended_) return;
  writing_ended_ = true;
  writer_sptr_->Abort();  // Stop circular sharing.

  if (!status_.ok()) return;
  status_ = writer_sptr_->GetStatus();
  if (status_.ok())
    SendCloseIfNot();
  else
    CallCloseHandler();
}  // OnEndOfWriting()

void ClientAsyncWriterImpl2::SetInternalError(const std::string& sError) {
  status_.SetInternalError(sError);
  CallCloseHandler();
  writing_ended_ = true;
  if (writer_sptr_)
    writer_sptr_->Abort();
}

}  // namespace grpc_cb_core
