// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_writer_impl2.h"

#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()
#include "client_reader_read_cqtag.h"     // for ClientReaderReadCqTag
#include "client_recv_init_md_cqtag.h"    // for ClientRecvInitMdCqTag
#include "client_send_close_cqtag.h"      // for ClientSendCloseCqTag
#include "client_send_init_md_cqtag.h"    // ClientSendInitMdCqTag
#include "client_send_msg_cqtag.h"        // for ClientSendMsgCqTag
#include "common/impl/complete_cb.h"      // for CompleteCb

namespace grpc_cb_core {

using Sptr = std::shared_ptr<ClientAsyncReaderWriterImpl2>;

// Todo: SyncGetInitMd();

ClientAsyncReaderWriterImpl2::ClientAsyncReaderWriterImpl2(
    const ChannelSptr& channel, const std::string& method,
    const CompletionQueueSptr& cq_sptr, int64_t timeout_ms,
    const StatusCb& status_cb)
    : call_sptr_(channel->MakeSharedCall(method, *cq_sptr, timeout_ms)),
      status_cb_(status_cb) {
  assert(cq_sptr);
  assert(call_sptr_);
}  // ClientAsyncReaderWriterImpl2()

void ClientAsyncReaderWriterImpl2::InitIfNot() {
  // private function has no Guard
  if (inited_) return;
  inited_ = true;
  if (!status_.ok()) return;

  ClientSendInitMdCqTag* send_tag = new ClientSendInitMdCqTag(call_sptr_);
  if (!send_tag->Start()) {
    delete send_tag;
    SetInternalError("Failed to send init metadata to init bidirectional streaming.");
    return;
  }  // if

  ClientRecvInitMdCqTag* recv_tag = new ClientRecvInitMdCqTag(call_sptr_);
  if (!recv_tag->Start()) {
    delete recv_tag;
    SetInternalError("Failed to receive init metadata to init bidirectional streaming.");
    return;
  }  // if
}  // InitIfNot()

ClientAsyncReaderWriterImpl2::~ClientAsyncReaderWriterImpl2() {
  // No CqTag shares this.
  assert(reading_ended_);
  assert(writing_ended_);
}

bool ClientAsyncReaderWriterImpl2::Write(const std::string& msg) {
  Guard g(mtx_);
  InitIfNot();
  if (!status_.ok()) {
    assert(reading_ended_ && writing_ended_);
    return false;
  }
  if (writing_closing_ || writing_ended_)
    return false;

  bool is_sending = !msg_queue_.empty();
  msg_queue_.push(msg);
  if (is_sending) return true;
  return SendNext();
}  // Write()

void ClientAsyncReaderWriterImpl2::CloseWriting() {
  Guard g(mtx_);
  if (writing_closing_) return;
  writing_closing_ = true;

  if (!status_.ok()) return;
  if (!msg_queue_.empty()) return;  // sending

  // End when all messages are written.
  assert(!has_sent_close_);
  SendClose();
  assert(writing_ended_);
}  // CloseWriting()

// Send close to half-close when writing are ended.
void ClientAsyncReaderWriterImpl2::SendClose() {
  // private function has no Guard
  assert(!has_sent_close_);
  assert((has_sent_close_ = true));
  assert(status_.ok());
  writing_ended_ = true;  // Normal end.
  ClientSendCloseCqTag* tag = new ClientSendCloseCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  SetInternalError("Failed to close writing.");  // calls status_cb_
}  // SendClose()

void ClientAsyncReaderWriterImpl2::ReadEach(const MsgStrCb& msg_cb) {
  Guard g(mtx_);
  if (reading_started_) return;  // already started.
  reading_started_ = true;
  InitIfNot();
  if (!status_.ok()) return;
  msg_cb_ = msg_cb;
  ReadNext();
}  // ReadEach()

void ClientAsyncReaderWriterImpl2::SetErrorStatus(const Status& error_status) {
  assert(!error_status.ok());
  Guard g(mtx_);
  if (!status_.ok()) return;
  InternalSetErrorStatus(error_status);
}  // SetErrorStatus()

void ClientAsyncReaderWriterImpl2::OnSent(bool success) {
  assert(!msg_queue_.empty());
  msg_queue_.pop();  // front msg is sent

  if (!status_.ok()) {
    assert(!status_cb_);  // already called and rest
    return;
  }

  if (!msg_queue_.empty()) {
    SendNext();
    return;
  }

  if (!writing_closing_)
    return;
  SendClose();
  assert(writing_ended_);
  if (reading_ended_)
    CallStatusCb();  // Both ended.
}  // OnSent()

void ClientAsyncReaderWriterImpl2::OnRead(bool success,
    ClientReaderReadCqTag& tag) {
  Guard g(mtx_);  // Callback needs Guard.
  if (!status_.ok()) {
    assert(!status_cb_);  // already called and reset
    return;
  }
  if (!success) {
    SetInternalError("ClientReaderReadCqTag failed.");
    return;
  }
  if (!tag.HasGotMsg()) {  // End of reading.
    reading_ended_ = true;  // Normal end.
    if (writing_ended_)
      CallStatusCb();
    return;
  }

  std::string sMsg;
  Status status = tag.GetResultMsg(sMsg);
  if (!status.ok()) {
    InternalSetErrorStatus(status);  // trigger status cb
    return;
  }

  if (msg_cb_) {
    Status status = msg_cb_(sMsg);
    if (!status.ok()) {
      InternalSetErrorStatus(status);
      return;
    }
  }

  ReadNext();
}  // OnRead()

bool ClientAsyncReaderWriterImpl2::SendNext() {
  // private function has no Guard
  assert(!msg_queue_.empty());
  assert(call_sptr_);
  auto* tag = new ClientSendMsgCqTag(call_sptr_);
  auto sptr = shared_from_this();
  CompleteCb complete_cb = [sptr](bool success) {
    sptr->OnSent(success);
  };
  tag->SetCompleteCb(complete_cb);
  bool ok = tag->Start(msg_queue_.front());
  if (ok) return true;

  delete tag;
  SetInternalError("Failed to write bidirectional streaming.");
  return false;
}  // SendNext()

void ClientAsyncReaderWriterImpl2::ReadNext() {
  // private function has no Guard
  auto* tag = new ClientReaderReadCqTag(call_sptr_);
  auto sptr = shared_from_this();
  tag->SetCompleteCb([sptr, tag](bool success) {
      sptr->OnRead(success, *tag);
  });
  if (tag->Start()) return;

  delete tag;
  SetInternalError("Failed to async read bidi streaming.");
}  // ReadNext()

// Set status, call status callback and reset helpers.
void ClientAsyncReaderWriterImpl2::SetInternalError(const std::string& sError) {
  // private function has no Guard
  assert(status_.ok());
  InternalSetErrorStatus(Status::InternalError(sError));
}

void ClientAsyncReaderWriterImpl2::InternalSetErrorStatus(
    const Status& error_status) {
  assert(!error_status.ok());
  // private function has no Guard
  assert(status_.ok());
  status_ = error_status;
  CallStatusCb();
  // Set ended on error.
  reading_ended_ = true;
  writing_ended_ = true;
}  // InternalSetErrorStatus()

void ClientAsyncReaderWriterImpl2::CallStatusCb() {
  // private function has no Guard
  if (!status_cb_) return;
  status_cb_(status_);
  status_cb_ = nullptr;
}

}  // namespace grpc_cb_core
