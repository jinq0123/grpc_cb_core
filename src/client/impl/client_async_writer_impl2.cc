// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_impl2.h"

#include <cassert>  // for assert()

#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()

#include "client_async_write_worker.h"         // for ClientAsyncWriteWorker
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
  // Have done CallCloseCb().
}

bool ClientAsyncWriterImpl2::Write(const std::string& request) {
  Guard g(mtx_);
  if (!status_.ok())
    return false;
  if (writing_closing_ || writing_ended_)
    return false;

  if (writing_started_) {
    assert(writer_sptr_);  // because not Close() yet
    return writer_sptr_->Queue(request);
  }
  writing_started_ = true;
  assert(!writer_sptr_);

  // CqTag keeps WriteWorker, which keeps this, which weakly keeps WriterWorker
  auto sptr = shared_from_this();
  writer_sptr_.reset(new ClientAsyncWriteWorker(call_sptr_,
      [sptr]() {
        sptr->OnEndOfWriting();
      }));
  writer_wptr_ = writer_sptr_;
  return writer_sptr_->Queue(request);
}  // Write()

void ClientAsyncWriterImpl2::Close(const CloseCb& close_cb/* = nullptr*/) {
  Guard g(mtx_);
  if (writing_closing_) return;  // already done
  writing_closing_ = true;

  close_cb_ = close_cb;  // reset in CallCloseCb()
  auto writer_sptr(writer_sptr_);
  writer_sptr_.reset();  // always stop circular sharing

  if (!status_.ok()) {
    CallCloseCb();
    return;
  }

  if (writing_started_) {
    assert(writer_sptr);
    writer_sptr->SetClosing();  // May trigger OnEndOfWriting().
  } else {
    writing_ended_ = true;  // Ended without start.
    SendCloseIfNot();
  }
}  // Close()

// Finally close...
void ClientAsyncWriterImpl2::SendCloseIfNot() {
  // private function need no Guard.
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
  SetInternalError("Failed to close client stream.");  // Calls CallCloseCb();
}  // SendCloseIfNot()

void ClientAsyncWriterImpl2::CallCloseCb(const std::string& sMsg/* = ""*/) {
  // private function need no Guard.
  if (!close_cb_) return;
  close_cb_(status_, sMsg);
  close_cb_ = nullptr;
}

// Callback of ClientWriterCloseCqTag::OnComplete()
void ClientAsyncWriterImpl2::OnClosed(bool success, ClientWriterCloseCqTag& tag) {
  Guard g(mtx_);  // Callback need Guard.

  if (!tag.IsStatusOk()) {
    status_ = tag.GetStatus();
    CallCloseCb();
    return;
  }

  // Todo: Get trailing metadata.
  std::string sMsg;
  status_ = tag.GetResponse(sMsg);
  CallCloseCb(sMsg);
}  // OnClosed()

void ClientAsyncWriterImpl2::OnEndOfWriting() {
  Guard g(mtx_);  // Callback need Guard.
  assert(writing_started_);
  assert(!writing_ended_);  // call OnEndOfWriting() only once
  writing_ended_ = true;

  if (!status_.ok()) return;
  auto writer_sptr = writer_wptr_.lock();
  assert(writer_sptr);
  status_ = writer_sptr->GetStatus();
  if (status_.ok())
    SendCloseIfNot();
  else
    CallCloseCb();
}  // OnEndOfWriting()

void ClientAsyncWriterImpl2::SetInternalError(const std::string& sError) {
  // private function need no Guard.
  status_.SetInternalError(sError);
  CallCloseCb();
  writing_ended_ = true;
  auto writer_sptr = writer_wptr_.lock();
  if (writer_sptr)
    writer_sptr->Abort();
}

}  // namespace grpc_cb_core
