/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "curlx/dynbuf.h"
#ضمن "progress.h"
#ضمن "request.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "transfer.h"
#ضمن "url.h"
#ضمن "curlx/strparse.h"

فراغ Curl_req_init(بنية SingleRequest *req)
{
  memset(req, 0, حجم(*req));
}

CURLcode Curl_req_soft_reset(بنية SingleRequest *req,
                             بنية Curl_easy *داده)
{
  CURLcode نتیجه;

  req->انجامشده = FALSE;
  req->upload_انجامشده = FALSE;
  req->upload_aborted = FALSE;
  req->download_انجامشده = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_فرستاده = FALSE;
  req->ignorebody = FALSE;
  req->shutdown = FALSE;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->سرآیند = FALSE;
  req->headerline = 0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->httpversion_فرستاده = 0;
  req->httpversion = 0;
  req->sendbuf_hds_len = 0;

  curlx_safefree(req->hd_auth);
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#پایان_اگر

  نتیجه = Curl_client_شروع(داده);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, داده->مجموعه.upload_میانگیر_اندازه, 1,
                    BUFQ_OPT_SOFT_حد);
    req->sendbuf_init = TRUE;
  }
  وإلا {
    Curl_bufq_reset(&req->sendbuf);
    إذا(داده->مجموعه.upload_میانگیر_اندازه != req->sendbuf.chunk_اندازه) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, داده->مجموعه.upload_میانگیر_اندازه, 1,
                      BUFQ_OPT_SOFT_حد);
    }
  }

  أرجع CURLE_OK;
}

CURLcode Curl_req_شروع(بنية SingleRequest *req,
                        بنية Curl_easy *داده)
{
  req->شروع = *Curl_pgrs_now(داده);
  أرجع Curl_req_soft_reset(req, داده);
}

ساكن CURLcode req_flush(بنية Curl_easy *داده);

CURLcode Curl_req_انجامشده(بنية SingleRequest *req,
                       بنية Curl_easy *داده, bool aborted)
{
  (فراغ)req;
  إذا(!aborted)
    (فراغ)req_flush(داده);
  Curl_client_reset(داده);
  أرجع CURLE_OK;
}

فراغ Curl_req_hard_reset(بنية SingleRequest *req, بنية Curl_easy *داده)
{
  بنية curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#پایان_اگر
  Curl_client_reset(داده);
  إذا(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(داده);
  /* Can no longer memset() this struct as we need to keep some state */
  req->اندازه = -1;
  req->maxdownload = -1;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->شروع = t0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->headerline = 0;
  req->جابه_جایی = 0;
  req->httpcode = 0;
  req->io_flags = 0;
  req->upgr101 = UPGR101_NONE;
  req->sendbuf_hds_len = 0;
  req->timeofdoc = 0;
  req->location = عدم;
  req->newurl = عدم;
#اگر_تعریف_نشده CURL_DISABLE_COOKIES
  req->setcookies = 0;
#پایان_اگر
  req->سرآیند = FALSE;
  req->content_range = FALSE;
  req->download_انجامشده = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_فرستاده = FALSE;
  req->rewind_read = FALSE;
  req->upload_انجامشده = FALSE;
  req->upload_aborted = FALSE;
  req->ignorebody = FALSE;
  req->http_bodyless = FALSE;
  req->chunk = FALSE;
  req->resp_trailer = FALSE;
  req->ignore_cl = FALSE;
  req->upload_chunky = FALSE;
  req->no_body = داده->مجموعه.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_بلوک(&داده->پیشرفت.dl.rlimit, FALSE, &t0);
  Curl_rlimit_بلوک(&داده->پیشرفت.ul.rlimit, FALSE, &t0);
}

فراغ Curl_req_free(بنية SingleRequest *req, بنية Curl_easy *داده)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#پایان_اگر
  إذا(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(داده);
}

ساكن CURLcode xfer_send(بنية Curl_easy *داده,
                          ثابت حرف *buf, size_t blen,
                          size_t hds_len, size_t *pnwritten)
{
  CURLcode نتیجه = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#اگر_تعریف_شده DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    size_t body_len = blen - hds_len;
    إذا(body_len) {
      ثابت حرف *p = getenv("CURL_SMALLREQSEND");
      إذا(p) {
        curl_off_t body_small;
        إذا(!curlx_str_عدد(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#پایان_اگر
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  إذا(داده->مجموعه.max_send_سرعت) {
    size_t body_بایتها = blen - hds_len;
    إذا((curl_off_t)body_بایتها > داده->مجموعه.max_send_سرعت)
      blen = hds_len + (size_t)داده->مجموعه.max_send_سرعت;
  }

  إذا(داده->req.eos_read &&
    (Curl_bufq_is_خالی(&داده->req.sendbuf) ||
     Curl_bufq_len(&داده->req.sendbuf) == blen)) {
    DEBUGF(infof(داده, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  نتیجه = Curl_xfer_send(داده, buf, blen, eos, pnwritten);
  إذا(!نتیجه) {
    إذا(eos && (blen == *pnwritten))
      داده->req.eos_فرستاده = TRUE;
    إذا(*pnwritten) {
      إذا(hds_len)
        Curl_debug(داده, CURLINFO_سرآیند_بیرون, buf,
                   CURLMIN(hds_len, *pnwritten));
      إذا(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(داده, CURLINFO_داده_بیرون, buf + hds_len, body_len);
        داده->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(داده, body_len);
      }
    }
  }
  أرجع نتیجه;
}

ساكن CURLcode req_send_میانگیر_flush(بنية Curl_easy *داده)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت بیعلامت حرف *buf;
  size_t blen;

  طالما(Curl_bufq_peek(&داده->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(داده->req.sendbuf_hds_len, blen);
    نتیجه = xfer_send(داده, (ثابت حرف *)buf, blen, hds_len, &nwritten);
    إذا(نتیجه)
      اكسر;

    Curl_bufq_skip(&داده->req.sendbuf, nwritten);
    إذا(hds_len) {
      داده->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    إذا(nwritten < blen)
      اكسر;
  }
  أرجع نتیجه;
}

ساكن CURLcode req_مجموعه_upload_انجامشده(بنية Curl_easy *داده)
{
  DEBUGASSERT(!داده->req.upload_انجامشده);
  داده->req.upload_انجامشده = TRUE;
  CURL_REQ_CLEAR_SEND(داده);

  إذا(داده->mstate >= MSTATE_DID)
    Curl_pgrs_زمان(داده, TIMER_POSTRANSFER);
  Curl_creader_انجامشده(داده, داده->req.upload_aborted);

  إذا(داده->req.upload_aborted) {
    Curl_bufq_reset(&داده->req.sendbuf);
    إذا(داده->req.writebytecount)
      infof(داده, "abort upload after having sent %" FMT_OFF_T " bytes",
            داده->req.writebytecount);
    وإلا
      infof(داده, "abort upload");
  }
  وإلا إذا(داده->req.writebytecount)
    infof(داده, "upload completely sent off: %" FMT_OFF_T " bytes",
          داده->req.writebytecount);
  وإلا إذا(!داده->req.download_انجامشده) {
    DEBUGASSERT(Curl_bufq_is_خالی(&داده->req.sendbuf));
    infof(داده, Curl_creader_مجموع_درازی(داده) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  أرجع Curl_xfer_send_close(داده);
}

ساكن CURLcode req_flush(بنية Curl_easy *داده)
{
  CURLcode نتیجه;

  إذا(!داده || !داده->conn)
    أرجع CURLE_FAILED_INIT;

  إذا(!Curl_bufq_is_خالی(&داده->req.sendbuf)) {
    نتیجه = req_send_میانگیر_flush(داده);
    إذا(نتیجه)
      أرجع نتیجه;
    إذا(!Curl_bufq_is_خالی(&داده->req.sendbuf)) {
      DEBUGF(infof(داده, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&داده->req.sendbuf)));
      أرجع CURLE_AGAIN;
    }
  }
  وإلا إذا(Curl_xfer_needs_flush(داده)) {
    DEBUGF(infof(داده, "Curl_req_flush(), xfer send_pending"));
    أرجع Curl_xfer_flush(داده);
  }

  إذا(داده->req.eos_read && !داده->req.eos_فرستاده) {
    حرف tmp = 0;
    size_t nwritten;
    نتیجه = xfer_send(داده, &tmp, 0, 0, &nwritten);
    إذا(نتیجه)
      أرجع نتیجه;
    DEBUGASSERT(داده->req.eos_فرستاده);
  }

  إذا(!داده->req.upload_انجامشده && داده->req.eos_read && داده->req.eos_فرستاده) {
    DEBUGASSERT(Curl_bufq_is_خالی(&داده->req.sendbuf));
    إذا(داده->req.shutdown) {
      bool انجامشده;
      نتیجه = Curl_xfer_send_shutdown(داده, &انجامشده);
      إذا(نتیجه && داده->req.shutdown_err_ignore) {
        infof(داده, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (صحيح)نتیجه);
        نتیجه = CURLE_OK;
        انجامشده = TRUE;
      }

      إذا(نتیجه)
        أرجع نتیجه;
      إذا(!انجامشده)
        أرجع CURLE_AGAIN;
    }
    أرجع req_مجموعه_upload_انجامشده(داده);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode جمع_کن_from_client(فراغ *reader_ctx,
                                بیعلامت حرف *buf, size_t buflen,
                                size_t *pnread)
{
  بنية Curl_easy *داده = reader_ctx;
  CURLcode نتیجه;
  bool eos;

  نتیجه = Curl_client_read(داده, (حرف *)buf, buflen, pnread, &eos);
  إذا(!نتیجه && eos)
    داده->req.eos_read = TRUE;
  أرجع نتیجه;
}

ساكن CURLcode req_send_میانگیر_جمع_کن(بنية Curl_easy *داده,
                                    ثابت حرف *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode نتیجه = CURLE_OK;
  size_t n;
  نتیجه = Curl_bufq_cwrite(&داده->req.sendbuf, buf, blen, &n);
  إذا(نتیجه)
    أرجع نتیجه;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  داده->req.sendbuf_hds_len += hds_len;
  أرجع CURLE_OK;
}

CURLcode Curl_req_send(بنية Curl_easy *داده, بنية dynbuf *req,
                       بیعلامت حرف httpversion)
{
  CURLcode نتیجه;
  ثابت حرف *buf;
  size_t blen, nwritten;

  إذا(!داده || !داده->conn)
    أرجع CURLE_FAILED_INIT;

  داده->req.httpversion_فرستاده = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  إذا(Curl_bufq_is_خالی(&داده->req.sendbuf) &&
     !Curl_creader_مجموع_درازی(داده) &&
     (blen <= داده->req.sendbuf.chunk_اندازه)) {
    داده->req.eos_read = TRUE;
    نتیجه = xfer_send(داده, buf, blen, blen, &nwritten);
    إذا(نتیجه)
      أرجع نتیجه;
    buf += nwritten;
    blen -= nwritten;
    إذا(!blen) {
      نتیجه = req_مجموعه_upload_انجامشده(داده);
      إذا(نتیجه)
        أرجع نتیجه;
    }
  }

  إذا(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    نتیجه = req_send_میانگیر_جمع_کن(داده, buf, blen, blen);
    إذا(نتیجه)
      أرجع نتیجه;

    أرجع Curl_req_send_بیشتر(داده);
  }
  أرجع CURLE_OK;
}

bool Curl_req_sendbuf_خالی(بنية Curl_easy *داده)
{
  أرجع !داده->req.sendbuf_init || Curl_bufq_is_خالی(&داده->req.sendbuf);
}

bool Curl_req_want_send(بنية Curl_easy *داده)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  أرجع !داده->req.انجامشده &&
         !Curl_rlimit_is_blocked(&داده->پیشرفت.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(داده) ||
          !Curl_req_sendbuf_خالی(داده) ||
          Curl_xfer_needs_flush(داده));
}

bool Curl_req_want_recv(بنية Curl_easy *داده)
{
  /* Not done and download not blocked and want RECV */
  أرجع !داده->req.انجامشده &&
         !Curl_rlimit_is_blocked(&داده->پیشرفت.dl.rlimit) &&
         CURL_REQ_WANT_RECV(داده);
}

bool Curl_req_انجامشده_sending(بنية Curl_easy *داده)
{
  أرجع داده->req.upload_انجامشده && !Curl_req_want_send(داده);
}

CURLcode Curl_req_send_بیشتر(بنية Curl_easy *داده)
{
  CURLcode نتیجه;

  /* Fill our send buffer if more from client can be read. */
  إذا(!داده->req.upload_aborted &&
     !داده->req.eos_read &&
     !Curl_xfer_send_is_paused(داده) &&
     !Curl_bufq_is_پر(&داده->req.sendbuf)) {
    size_t nread;
    نتیجه = Curl_bufq_sipn(&داده->req.sendbuf, 0,
                            جمع_کن_from_client, داده, &nread);
    إذا(نتیجه && نتیجه != CURLE_AGAIN)
      أرجع نتیجه;
  }

  نتیجه = req_flush(داده);
  إذا(نتیجه == CURLE_AGAIN)
    نتیجه = CURLE_OK;

  أرجع نتیجه;
}

CURLcode Curl_req_abort_sending(بنية Curl_easy *داده)
{
  إذا(!داده->req.upload_انجامشده) {
    Curl_bufq_reset(&داده->req.sendbuf);
    داده->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(داده);
    أرجع req_مجموعه_upload_انجامشده(داده);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(بنية Curl_easy *داده)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode نتیجه = CURLE_OK;
  إذا(CURL_REQ_WANT_SEND(داده))
    نتیجه = Curl_req_abort_sending(داده);
  CURL_REQ_CLEAR_IO(داده);
  أرجع نتیجه;
}
