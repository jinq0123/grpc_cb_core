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
}

void ClientAsyncReaderWriterImpl2::InitIfNot() {
  // private function has no Guard
  if (inited_) return;
  inited_ = true;

  ClientSendInitMdCqTag* send_tag = new ClientSendInitMdCqTag(call_sptr_);
  if (!send_tag->Start()) {
    delete send_tag;
    SetInternalError("Failed to send init metadata to init bidirectional streaming.");
    return;
  }

  ClientRecvInitMdCqTag* recv_tag = new ClientRecvInitMdCqTag(call_sptr_);
  if (!recv_tag->Start()) {
    delete recv_tag;
    SetInternalError("Failed to receive init metadata to init bidirectional streaming.");
    return;
  }
}

ClientAsyncReaderWriterImpl2::~ClientAsyncReaderWriterImpl2() {
  // Reader and Writer helpers share this.
  assert(reading_ended_);
  assert(writing_ended_);
  if (inited_) SendCloseIfNot();
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
  return TryToSendNext();
}

void ClientAsyncReaderWriterImpl2::CloseWriting() {
  Guard g(mtx_);
  if (writing_closing_) return;
  writing_closing_ = true;

  if (!msg_queue_.empty()) return;  // sending

  // XXX 
  // End when all messages are written.
}

// Called in dtr().
// Send close to half-close when writing are ended.
void ClientAsyncReaderWriterImpl2::SendCloseIfNot() {
  // private function has no Guard
  assert(writing_ended_);  // Must be ended.
  if (!status_.ok()) return;
  if (has_sent_close_) return;
  has_sent_close_ = true;

  ClientSendCloseCqTag* tag = new ClientSendCloseCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  SetInternalError("Failed to close writing.");  // calls status_cb_
}

void ClientAsyncReaderWriterImpl2::ReadEach(const MsgStrCb& msg_cb) {
  Guard g(mtx_);
  InitIfNot();
  if (reading_started_) return;  // already started.
  reading_started_ = true;
  msg_cb_ = msg_cb;
  ReadNext();
}

void ClientAsyncReaderWriterImpl2::OnEndOfReading() {
  Guard g(mtx_);  // Callback need Guard.
  assert(reading_started_);
  if (reading_ended_) return;
  reading_ended_ = true;

  if (!status_.ok()) return;
  // XXX
  //auto reader_sptr = reader_wptr_.lock();
  //assert(reader_sptr);
  //status_ = reader_sptr->GetStatus();
  if (!status_.ok() || writing_ended_)
    CallStatusCb();
}

void ClientAsyncReaderWriterImpl2::OnEndOfWriting() {
  Guard g(mtx_);  // Callback need Guard.
  // XXX assert(writing_started_);
  assert(!writing_ended_);  // only call once
  writing_ended_ = true;

  if (!status_.ok()) return;
  // XXX
  //auto writer_sptr = writer_wptr_.lock();
  //assert(writer_sptr);
  //status_ = writer_sptr->GetStatus();
  if (!status_.ok()) {
    CallStatusCb();
    return;
  }

  SendCloseIfNot();
}

void ClientAsyncReaderWriterImpl2::OnSent(bool success) {
  assert(!msg_queue_.empty());
  msg_queue_.pop();  // front msg is sent

  if (!status_.ok()) {
    assert(!status_cb_);  // already called and rest  XXX
    return;
  }

  if (!msg_queue_.empty()) {
    TryToSendNext();
    return;
  }

  // XXX is_closing?
}  // OnSent()

void ClientAsyncReaderWriterImpl2::OnRead(bool success,
    ClientReaderReadCqTag& tag) {
  Guard g(mtx_);  // Callback needs Guard.
  if (!status_.ok()) {
    assert(!status_cb_);  // already called and reset  XXX
    return;
  }
  if (!success) {
    SetInternalError("ClientReaderReadCqTag failed.");
    return;
  }
  if (!tag.HasGotMsg()) {
    // CallStatusCb of read.
    // Receiving status will be after all reading and writing.
    // XXX CallStatusCb();
    return;
  }

  std::string sMsg;
  status_ = tag.GetResultMsg(sMsg);
  if (!status_.ok()) {
    CallStatusCb();
    return;
  }

  if (msg_cb_) {
    status_ = msg_cb_(sMsg);
    if (!status_.ok()) {
      CallStatusCb();
      return;
    }
  }

  ReadNext();
}  // OnRead()

bool ClientAsyncReaderWriterImpl2::TryToSendNext() {
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
}

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
  status_.SetInternalError(sError);
  CallStatusCb();

  reading_ended_ = true;
  writing_ended_ = true;
}

void ClientAsyncReaderWriterImpl2::CallStatusCb() {
  // private function has no Guard
  if (!status_cb_) return;
  status_cb_(status_);
  status_cb_ = nullptr;
}

}  // namespace grpc_cb_core
