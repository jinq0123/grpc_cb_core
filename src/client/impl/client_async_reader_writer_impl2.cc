// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_writer_impl2.h"

#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()
#include "client_async_read_worker.h"     // for ClientAsyncReadWorker
#include "client_async_write_worker.h"    // for ClientAsyncWriteWorker
#include "client_recv_init_md_cqtag.h"    // for ClientRecvInitMdCqTag
#include "client_send_close_cqtag.h"      // for ClientSendCloseCqTag
#include "client_send_init_md_cqtag.h"    // ClientSendInitMdCqTag

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

  ClientSendInitMdCqTag* send_tag = new ClientSendInitMdCqTag(call_sptr_);
  if (!send_tag->Start()) {
    delete send_tag;
    SetInternalError("Failed to send init metadata to init stream.");
    return;
  }

  ClientRecvInitMdCqTag* recv_tag = new ClientRecvInitMdCqTag(call_sptr_);
  if (!recv_tag->Start()) {
    delete recv_tag;
    SetInternalError("Failed to receive init metadata to init stream.");
    return;
  }
}

ClientAsyncReaderWriterImpl2::~ClientAsyncReaderWriterImpl2() {
  // Reader and Writer helpers share this.
  assert(reading_ended_);
  assert(writing_ended_);
  SendCloseIfNot();
}

bool ClientAsyncReaderWriterImpl2::Write(const std::string& msg) {
  Guard g(mtx_);

  if (!status_.ok()) {
    assert(reading_ended_ && writing_ended_);
    return false;
  }
  if (writing_closing_ || writing_ended_)
    return false;

  if (writing_started_) {
    assert(writer_sptr_);  // because not CloseWriting() yet
    return writer_sptr_->Queue(msg);
  }
  writing_started_ = true;
  assert(!writer_sptr_);

  // Impl2 and WriteWorker share each other untill OnEndOfWriting().
  auto sptr = shared_from_this();  // can not in ctr().
  writer_sptr_.reset(new ClientAsyncWriteWorker(call_sptr_,
      [sptr]() {
        sptr->OnEndOfWriting();
      }));
  writer_wptr_ = writer_sptr_;
  return writer_sptr_->Queue(msg);
}

void ClientAsyncReaderWriterImpl2::CloseWriting() {
  Guard g(mtx_);
  if (writing_closing_) return;
  writing_closing_ = true;

  auto writer_sptr = writer_sptr_;
  writer_sptr_.reset();  // always stop circular sharing

  // End when all messages are written.
  if (writer_sptr)
    writer_sptr->SetClosing();
}

// Called in dtr().
// Send close to half-close when writing are ended.
void ClientAsyncReaderWriterImpl2::SendCloseIfNot() {
  // private function need no Guard
  assert(writing_ended_);  // Must be ended.
  if (!status_.ok()) return;
  if (has_sent_close_) return;
  has_sent_close_ = true;

  ClientSendCloseCqTag* tag = new ClientSendCloseCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  SetInternalError("Failed to close writing.");  // calls status_cb_
}

// Todo: same as ClientReader?

void ClientAsyncReaderWriterImpl2::ReadEach(const MsgStrCb& msg_cb) {
  Guard g(mtx_);
  if (reading_started_) return;  // already started.
  reading_started_ = true;

  // Impl2 and ReadWorker will share each other until OnEndOfReading().
  auto sptr = shared_from_this();
  auto reader_sptr = std::make_shared<ClientAsyncReadWorker>(
      call_sptr_, msg_cb,
      [sptr]() {
        sptr->OnEndOfReading();
      });
  reader_sptr->Start();
  reader_wptr_ = reader_sptr;
}

void ClientAsyncReaderWriterImpl2::OnEndOfReading() {
  Guard g(mtx_);  // Callback need Guard.
  assert(reading_started_);
  if (reading_ended_) return;
  reading_ended_ = true;

  if (!status_.ok()) return;
  auto reader_sptr = reader_wptr_.lock();
  assert(reader_sptr);
  status_ = reader_sptr->GetStatus();
  if (!status_.ok() || writing_ended_)
    CallStatusCb();
}

void ClientAsyncReaderWriterImpl2::OnEndOfWriting() {
  Guard g(mtx_);  // Callback need Guard.
  assert(writing_started_);
  assert(!writing_ended_);  // only call once
  writing_ended_ = true;

  if (!status_.ok()) return;
  auto writer_sptr = writer_wptr_.lock();
  assert(writer_sptr);
  status_ = writer_sptr->GetStatus();
  if (!status_.ok()) {
    CallStatusCb();
    return;
  }

  SendCloseIfNot();
}

// Set status, call status callback and reset helpers.
void ClientAsyncReaderWriterImpl2::SetInternalError(const std::string& sError) {
  // private function need no Guard
  status_.SetInternalError(sError);
  CallStatusCb();

  reading_ended_ = true;
  writing_ended_ = true;
  auto reader_sptr = reader_wptr_.lock();
  auto writer_sptr = writer_wptr_.lock();
  if (reader_sptr) reader_sptr->Abort();
  if (writer_sptr) writer_sptr->Abort();
}

void ClientAsyncReaderWriterImpl2::CallStatusCb() {
  // private function need no Guard
  if (!status_cb_) return;
  status_cb_(status_);
  status_cb_ = nullptr;
}

}  // namespace grpc_cb_core
