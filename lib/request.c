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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "request.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "curlx/strparse.h"

শূন্য Curl_req_init(কাঠামো SingleRequest *req)
{
  memset(req, 0, আকার(*req));
}

CURLcode Curl_req_soft_reset(কাঠামো SingleRequest *req,
                             কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল;

  req->done = FALSE;
  req->upload_done = FALSE;
  req->upload_aborted = FALSE;
  req->download_done = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_sent = FALSE;
  req->ignorebody = FALSE;
  req->shutdown = FALSE;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->header = FALSE;
  req->headerline = 0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->httpversion_sent = 0;
  req->httpversion = 0;
  req->sendbuf_hds_len = 0;

  curlx_safefree(req->hd_auth);
#ifndef CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#endif

  ফলাফল = Curl_client_start(তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, তথ্য->set.upload_বাফার_আকার, 1,
                    BUFQ_OPT_SOFT_LIMIT);
    req->sendbuf_init = TRUE;
  }
  অন্যথা {
    Curl_bufq_reset(&req->sendbuf);
    যদি(তথ্য->set.upload_বাফার_আকার != req->sendbuf.chunk_আকার) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, তথ্য->set.upload_বাফার_আকার, 1,
                      BUFQ_OPT_SOFT_LIMIT);
    }
  }

  ফেরত CURLE_OK;
}

CURLcode Curl_req_start(কাঠামো SingleRequest *req,
                        কাঠামো Curl_easy *তথ্য)
{
  req->start = *Curl_pgrs_now(তথ্য);
  ফেরত Curl_req_soft_reset(req, তথ্য);
}

স্থির CURLcode req_flush(কাঠামো Curl_easy *তথ্য);

CURLcode Curl_req_done(কাঠামো SingleRequest *req,
                       কাঠামো Curl_easy *তথ্য, bool aborted)
{
  (শূন্য)req;
  যদি(!aborted)
    (শূন্য)req_flush(তথ্য);
  Curl_client_reset(তথ্য);
  ফেরত CURLE_OK;
}

শূন্য Curl_req_hard_reset(কাঠামো SingleRequest *req, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ifndef CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#endif
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#endif
  Curl_client_reset(তথ্য);
  যদি(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(তথ্য);
  /* Can no longer memset() this struct as we need to keep some state */
  req->আকার_2 = -1;
  req->maxdownload = -1;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->start = t0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->headerline = 0;
  req->offset = 0;
  req->httpcode = 0;
  req->io_flags = 0;
  req->upgr101 = UPGR101_NONE;
  req->sendbuf_hds_len = 0;
  req->timeofdoc = 0;
  req->location = ফাঁকা;
  req->newurl = ফাঁকা;
#ifndef CURL_DISABLE_COOKIES
  req->setcookies = 0;
#endif
  req->header = FALSE;
  req->content_range = FALSE;
  req->download_done = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_sent = FALSE;
  req->rewind_read = FALSE;
  req->upload_done = FALSE;
  req->upload_aborted = FALSE;
  req->ignorebody = FALSE;
  req->http_bodyless = FALSE;
  req->chunk = FALSE;
  req->resp_trailer = FALSE;
  req->ignore_cl = FALSE;
  req->upload_chunky = FALSE;
  req->no_body = তথ্য->set.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_block(&তথ্য->progress.dl.rlimit, FALSE, &t0);
  Curl_rlimit_block(&তথ্য->progress.ul.rlimit, FALSE, &t0);
}

শূন্য Curl_req_free(কাঠামো SingleRequest *req, কাঠামো Curl_easy *তথ্য)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ifndef CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#endif
  যদি(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(তথ্য);
}

স্থির CURLcode xfer_send(কাঠামো Curl_easy *তথ্য,
                          ধ্রুবক অক্ষর *buf, size_t blen,
                          size_t hds_len, size_t *pnwritten)
{
  CURLcode ফলাফল = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#ifdef DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    size_t body_len = blen - hds_len;
    যদি(body_len) {
      ধ্রুবক অক্ষর *p = getenv("CURL_SMALLREQSEND");
      যদি(p) {
        curl_off_t body_small;
        যদি(!curlx_str_সংখ্যা(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#endif
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  যদি(তথ্য->set.max_send_speed) {
    size_t body_bytes = blen - hds_len;
    যদি((curl_off_t)body_bytes > তথ্য->set.max_send_speed)
      blen = hds_len + (size_t)তথ্য->set.max_send_speed;
  }

  যদি(তথ্য->req.eos_read &&
    (Curl_bufq_is_empty(&তথ্য->req.sendbuf) ||
     Curl_bufq_len(&তথ্য->req.sendbuf) == blen)) {
    DEBUGF(infof(তথ্য, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  ফলাফল = Curl_xfer_send(তথ্য, buf, blen, eos, pnwritten);
  যদি(!ফলাফল) {
    যদি(eos && (blen == *pnwritten))
      তথ্য->req.eos_sent = TRUE;
    যদি(*pnwritten) {
      যদি(hds_len)
        Curl_debug(তথ্য, CURLINFO_HEADER_OUT, buf,
                   CURLMIN(hds_len, *pnwritten));
      যদি(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(তথ্য, CURLINFO_তথ্য_OUT, buf + hds_len, body_len);
        তথ্য->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(তথ্য, body_len);
      }
    }
  }
  ফেরত ফলাফল;
}

স্থির CURLcode req_send_বাফার_flush(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক unsigned অক্ষর *buf;
  size_t blen;

  যখন(Curl_bufq_peek(&তথ্য->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(তথ্য->req.sendbuf_hds_len, blen);
    ফলাফল = xfer_send(তথ্য, (ধ্রুবক অক্ষর *)buf, blen, hds_len, &nwritten);
    যদি(ফলাফল)
      বিরতি;

    Curl_bufq_skip(&তথ্য->req.sendbuf, nwritten);
    যদি(hds_len) {
      তথ্য->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    যদি(nwritten < blen)
      বিরতি;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode req_set_upload_done(কাঠামো Curl_easy *তথ্য)
{
  DEBUGASSERT(!তথ্য->req.upload_done);
  তথ্য->req.upload_done = TRUE;
  CURL_REQ_CLEAR_SEND(তথ্য);

  যদি(তথ্য->mstate >= MSTATE_DID)
    Curl_pgrsTime(তথ্য, TIMER_POSTRANSFER);
  Curl_creader_done(তথ্য, তথ্য->req.upload_aborted);

  যদি(তথ্য->req.upload_aborted) {
    Curl_bufq_reset(&তথ্য->req.sendbuf);
    যদি(তথ্য->req.writebytecount)
      infof(তথ্য, "abort upload after having sent %" FMT_OFF_T " bytes",
            তথ্য->req.writebytecount);
    অন্যথা
      infof(তথ্য, "abort upload");
  }
  অন্যথা যদি(তথ্য->req.writebytecount)
    infof(তথ্য, "upload completely sent off: %" FMT_OFF_T " bytes",
          তথ্য->req.writebytecount);
  অন্যথা যদি(!তথ্য->req.download_done) {
    DEBUGASSERT(Curl_bufq_is_empty(&তথ্য->req.sendbuf));
    infof(তথ্য, Curl_creader_যোগ_দৈর্ঘ্য(তথ্য) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  ফেরত Curl_xfer_send_close(তথ্য);
}

স্থির CURLcode req_flush(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল;

  যদি(!তথ্য || !তথ্য->conn)
    ফেরত CURLE_FAILED_INIT;

  যদি(!Curl_bufq_is_empty(&তথ্য->req.sendbuf)) {
    ফলাফল = req_send_বাফার_flush(তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    যদি(!Curl_bufq_is_empty(&তথ্য->req.sendbuf)) {
      DEBUGF(infof(তথ্য, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&তথ্য->req.sendbuf)));
      ফেরত CURLE_AGAIN;
    }
  }
  অন্যথা যদি(Curl_xfer_needs_flush(তথ্য)) {
    DEBUGF(infof(তথ্য, "Curl_req_flush(), xfer send_pending"));
    ফেরত Curl_xfer_flush(তথ্য);
  }

  যদি(তথ্য->req.eos_read && !তথ্য->req.eos_sent) {
    অক্ষর tmp = 0;
    size_t nwritten;
    ফলাফল = xfer_send(তথ্য, &tmp, 0, 0, &nwritten);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    DEBUGASSERT(তথ্য->req.eos_sent);
  }

  যদি(!তথ্য->req.upload_done && তথ্য->req.eos_read && তথ্য->req.eos_sent) {
    DEBUGASSERT(Curl_bufq_is_empty(&তথ্য->req.sendbuf));
    যদি(তথ্য->req.shutdown) {
      bool done;
      ফলাফল = Curl_xfer_send_shutdown(তথ্য, &done);
      যদি(ফলাফল && তথ্য->req.shutdown_err_ignore) {
        infof(তথ্য, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (পূর্ণসংখ্যা)ফলাফল);
        ফলাফল = CURLE_OK;
        done = TRUE;
      }

      যদি(ফলাফল)
        ফেরত ফলাফল;
      যদি(!done)
        ফেরত CURLE_AGAIN;
    }
    ফেরত req_set_upload_done(তথ্য);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode add_from_client(শূন্য *reader_ctx,
                                unsigned অক্ষর *buf, size_t buflen,
                                size_t *pnread)
{
  কাঠামো Curl_easy *তথ্য = reader_ctx;
  CURLcode ফলাফল;
  bool eos;

  ফলাফল = Curl_client_read(তথ্য, (অক্ষর *)buf, buflen, pnread, &eos);
  যদি(!ফলাফল && eos)
    তথ্য->req.eos_read = TRUE;
  ফেরত ফলাফল;
}

স্থির CURLcode req_send_বাফার_add(কাঠামো Curl_easy *তথ্য,
                                    ধ্রুবক অক্ষর *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t n;
  ফলাফল = Curl_bufq_cwrite(&তথ্য->req.sendbuf, buf, blen, &n);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  তথ্য->req.sendbuf_hds_len += hds_len;
  ফেরত CURLE_OK;
}

CURLcode Curl_req_send(কাঠামো Curl_easy *তথ্য, কাঠামো dynbuf *req,
                       unsigned অক্ষর httpversion)
{
  CURLcode ফলাফল;
  ধ্রুবক অক্ষর *buf;
  size_t blen, nwritten;

  যদি(!তথ্য || !তথ্য->conn)
    ফেরত CURLE_FAILED_INIT;

  তথ্য->req.httpversion_sent = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  যদি(Curl_bufq_is_empty(&তথ্য->req.sendbuf) &&
     !Curl_creader_যোগ_দৈর্ঘ্য(তথ্য) &&
     (blen <= তথ্য->req.sendbuf.chunk_আকার)) {
    তথ্য->req.eos_read = TRUE;
    ফলাফল = xfer_send(তথ্য, buf, blen, blen, &nwritten);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    buf += nwritten;
    blen -= nwritten;
    যদি(!blen) {
      ফলাফল = req_set_upload_done(তথ্য);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
  }

  যদি(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    ফলাফল = req_send_বাফার_add(তথ্য, buf, blen, blen);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    ফেরত Curl_req_send_more(তথ্য);
  }
  ফেরত CURLE_OK;
}

bool Curl_req_sendbuf_empty(কাঠামো Curl_easy *তথ্য)
{
  ফেরত !তথ্য->req.sendbuf_init || Curl_bufq_is_empty(&তথ্য->req.sendbuf);
}

bool Curl_req_want_send(কাঠামো Curl_easy *তথ্য)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  ফেরত !তথ্য->req.done &&
         !Curl_rlimit_is_blocked(&তথ্য->progress.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(তথ্য) ||
          !Curl_req_sendbuf_empty(তথ্য) ||
          Curl_xfer_needs_flush(তথ্য));
}

bool Curl_req_want_recv(কাঠামো Curl_easy *তথ্য)
{
  /* Not done and download not blocked and want RECV */
  ফেরত !তথ্য->req.done &&
         !Curl_rlimit_is_blocked(&তথ্য->progress.dl.rlimit) &&
         CURL_REQ_WANT_RECV(তথ্য);
}

bool Curl_req_done_sending(কাঠামো Curl_easy *তথ্য)
{
  ফেরত তথ্য->req.upload_done && !Curl_req_want_send(তথ্য);
}

CURLcode Curl_req_send_more(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল;

  /* Fill our send buffer if more from client can be read. */
  যদি(!তথ্য->req.upload_aborted &&
     !তথ্য->req.eos_read &&
     !Curl_xfer_send_is_paused(তথ্য) &&
     !Curl_bufq_is_full(&তথ্য->req.sendbuf)) {
    size_t nread;
    ফলাফল = Curl_bufq_sipn(&তথ্য->req.sendbuf, 0,
                            add_from_client, তথ্য, &nread);
    যদি(ফলাফল && ফলাফল != CURLE_AGAIN)
      ফেরত ফলাফল;
  }

  ফলাফল = req_flush(তথ্য);
  যদি(ফলাফল == CURLE_AGAIN)
    ফলাফল = CURLE_OK;

  ফেরত ফলাফল;
}

CURLcode Curl_req_abort_sending(কাঠামো Curl_easy *তথ্য)
{
  যদি(!তথ্য->req.upload_done) {
    Curl_bufq_reset(&তথ্য->req.sendbuf);
    তথ্য->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(তথ্য);
    ফেরত req_set_upload_done(তথ্য);
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(কাঠামো Curl_easy *তথ্য)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode ফলাফল = CURLE_OK;
  যদি(CURL_REQ_WANT_SEND(তথ্য))
    ফলাফল = Curl_req_abort_sending(তথ্য);
  CURL_REQ_CLEAR_IO(তথ্য);
  ফেরত ফলাফল;
}
