// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_writer_impl2.h"

#include <grpc_cb_core/client/channel.h>  // for MakeSharedCall()
#include "client_recv_init_md_cqtag.h"  // for ClientRecvInitMdCqTag
#include "client_send_close_cqtag.h"    // for ClientSendCloseCqTag
#include "client_send_init_md_cqtag.h"  // ClientSendInitMdCqTag

#include "client_async_reader_helper.h"  // for ClientAsyncReaderHelper
#include "client_async_writer_helper.h"  // for ClientAsyncWriterHelper

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

  if (writing_ended_)
    return false;
  if (writer_sptr_)
    return writer_sptr_->Queue(msg);

  // Impl2 and WriterHelper share each other untill OnEndOfWriting().
  auto sptr = shared_from_this();  // can not in ctr().
  writer_sptr_.reset(new ClientAsyncWriterHelper(call_sptr_,
      [sptr]() {
        auto p2 = sptr;
        p2->OnEndOfWriting();  // will clear this function<>
        // sptr is invalid now
      }));
  return writer_sptr_->Queue(msg);
}

void ClientAsyncReaderWriterImpl2::CloseWriting() {
  Guard g(mtx_);
  // End when all messages are written.
  if (writer_sptr_)
    writer_sptr_->SetClosing();
}

// Called in dtr().
// Send close to half-close when writing are ended.
void ClientAsyncReaderWriterImpl2::SendCloseIfNot() {
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
  if (reader_sptr_) return;  // already started.

  // Impl2 and ReaderHelper will share each other until OnEndOfReading().
  auto sptr = shared_from_this();
  reader_sptr_.reset(new ClientAsyncReaderHelper(call_sptr_, msg_cb,
      [sptr]() {
        auto p2 = sptr;
        p2->OnEndOfReading();  // will clear this function<>
        // sptr is invalid now
      }));
  reader_sptr_->Start();
}

void ClientAsyncReaderWriterImpl2::OnEndOfReading() {
  Guard g(mtx_);
  assert(reader_sptr_);
  if (reading_ended_) return;
  reading_ended_ = true;
  reader_sptr_->Abort();  // Stop circular sharing.

  if (!status_.ok()) return;
  status_ = reader_sptr_->GetStatus();
  if (!status_.ok() || writing_ended_)
    CallStatusCb();
}

void ClientAsyncReaderWriterImpl2::OnEndOfWriting() {
  Guard g(mtx_);
  assert(writer_sptr_);
  if (writing_ended_) return;
  writing_ended_ = true;
  writer_sptr_->Abort();  // Stop circular sharing.

  if (!status_.ok()) return;
  status_ = writer_sptr_->GetStatus();
  if (!status_.ok()) {
    CallStatusCb();
    return;
  }

  SendCloseIfNot();
}

// Set status, call status callback and reset helpers.
void ClientAsyncReaderWriterImpl2::SetInternalError(const std::string& sError) {
  status_.SetInternalError(sError);
  CallStatusCb();

  reading_ended_ = true;
  writing_ended_ = true;
  if (reader_sptr_)
    reader_sptr_->Abort();
  if (writer_sptr_)
    writer_sptr_->Abort();
}

void ClientAsyncReaderWriterImpl2::CallStatusCb() {
  if (!status_cb_) return;
  status_cb_(status_);
  status_cb_ = nullptr;
}

}  // namespace grpc_cb_core
