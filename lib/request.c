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
                             بنية Curl_easy *معلومات)
{
  CURLcode پایله;

  req->بشپړ = FALSE;
  req->upload_بشپړ = FALSE;
  req->upload_aborted = FALSE;
  req->download_بشپړ = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_لېږل_شوی = FALSE;
  req->ignorebody = FALSE;
  req->shutdown = FALSE;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->سرلیک = FALSE;
  req->headerline = 0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->httpversion_لېږل_شوی = 0;
  req->httpversion = 0;
  req->sendbuf_hds_len = 0;

  curlx_safefree(req->hd_auth);
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#د_که_پای

  پایله = Curl_client_پیل(معلومات);
  إذا(پایله)
    أرجع پایله;

  إذا(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, معلومات->سېټ.upload_بفر_اندازه, 1,
                    BUFQ_OPT_SOFT_حد);
    req->sendbuf_init = TRUE;
  }
  وإلا {
    Curl_bufq_reset(&req->sendbuf);
    إذا(معلومات->سېټ.upload_بفر_اندازه != req->sendbuf.chunk_اندازه) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, معلومات->سېټ.upload_بفر_اندازه, 1,
                      BUFQ_OPT_SOFT_حد);
    }
  }

  أرجع CURLE_OK;
}

CURLcode Curl_req_پیل(بنية SingleRequest *req,
                        بنية Curl_easy *معلومات)
{
  req->پیل = *Curl_pgrs_now(معلومات);
  أرجع Curl_req_soft_reset(req, معلومات);
}

ساكن CURLcode req_flush(بنية Curl_easy *معلومات);

CURLcode Curl_req_بشپړ(بنية SingleRequest *req,
                       بنية Curl_easy *معلومات, bool aborted)
{
  (فراغ)req;
  إذا(!aborted)
    (فراغ)req_flush(معلومات);
  Curl_client_reset(معلومات);
  أرجع CURLE_OK;
}

فراغ Curl_req_hard_reset(بنية SingleRequest *req, بنية Curl_easy *معلومات)
{
  بنية curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#د_که_پای
  Curl_client_reset(معلومات);
  إذا(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(معلومات);
  /* Can no longer memset() this struct as we need to keep some state */
  req->اندازه = -1;
  req->maxdownload = -1;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->پیل = t0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->headerline = 0;
  req->دننه_واټن = 0;
  req->httpcode = 0;
  req->io_flags = 0;
  req->upgr101 = UPGR101_NONE;
  req->sendbuf_hds_len = 0;
  req->timeofdoc = 0;
  req->location = عدم;
  req->newurl = عدم;
#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
  req->setcookies = 0;
#د_که_پای
  req->سرلیک = FALSE;
  req->content_range = FALSE;
  req->download_بشپړ = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_لېږل_شوی = FALSE;
  req->rewind_read = FALSE;
  req->upload_بشپړ = FALSE;
  req->upload_aborted = FALSE;
  req->ignorebody = FALSE;
  req->http_bodyless = FALSE;
  req->chunk = FALSE;
  req->resp_trailer = FALSE;
  req->ignore_cl = FALSE;
  req->upload_chunky = FALSE;
  req->no_body = معلومات->سېټ.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_بلاک(&معلومات->پرمختګ.dl.rlimit, FALSE, &t0);
  Curl_rlimit_بلاک(&معلومات->پرمختګ.ul.rlimit, FALSE, &t0);
}

فراغ Curl_req_free(بنية SingleRequest *req, بنية Curl_easy *معلومات)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#د_که_پای
  إذا(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(معلومات);
}

ساكن CURLcode xfer_send(بنية Curl_easy *معلومات,
                          ثابت حرف *buf, size_t blen,
                          size_t hds_len, size_t *pnwritten)
{
  CURLcode پایله = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#که_تعریف_شوی DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    size_t body_len = blen - hds_len;
    إذا(body_len) {
      ثابت حرف *p = getenv("CURL_SMALLREQSEND");
      إذا(p) {
        curl_off_t body_small;
        إذا(!curlx_str_شمېره(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#د_که_پای
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  إذا(معلومات->سېټ.max_send_چټکتیا) {
    size_t body_بایټونه = blen - hds_len;
    إذا((curl_off_t)body_بایټونه > معلومات->سېټ.max_send_چټکتیا)
      blen = hds_len + (size_t)معلومات->سېټ.max_send_چټکتیا;
  }

  إذا(معلومات->req.eos_read &&
    (Curl_bufq_is_تشه(&معلومات->req.sendbuf) ||
     Curl_bufq_len(&معلومات->req.sendbuf) == blen)) {
    DEBUGF(infof(معلومات, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  پایله = Curl_xfer_send(معلومات, buf, blen, eos, pnwritten);
  إذا(!پایله) {
    إذا(eos && (blen == *pnwritten))
      معلومات->req.eos_لېږل_شوی = TRUE;
    إذا(*pnwritten) {
      إذا(hds_len)
        Curl_debug(معلومات, CURLINFO_سرلیک_بهر, buf,
                   CURLMIN(hds_len, *pnwritten));
      إذا(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(معلومات, CURLINFO_معلومات_بهر, buf + hds_len, body_len);
        معلومات->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(معلومات, body_len);
      }
    }
  }
  أرجع پایله;
}

ساكن CURLcode req_send_بفر_flush(بنية Curl_easy *معلومات)
{
  CURLcode پایله = CURLE_OK;
  ثابت بینښه حرف *buf;
  size_t blen;

  طالما(Curl_bufq_peek(&معلومات->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(معلومات->req.sendbuf_hds_len, blen);
    پایله = xfer_send(معلومات, (ثابت حرف *)buf, blen, hds_len, &nwritten);
    إذا(پایله)
      اكسر;

    Curl_bufq_skip(&معلومات->req.sendbuf, nwritten);
    إذا(hds_len) {
      معلومات->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    إذا(nwritten < blen)
      اكسر;
  }
  أرجع پایله;
}

ساكن CURLcode req_سېټ_upload_بشپړ(بنية Curl_easy *معلومات)
{
  DEBUGASSERT(!معلومات->req.upload_بشپړ);
  معلومات->req.upload_بشپړ = TRUE;
  CURL_REQ_CLEAR_SEND(معلومات);

  إذا(معلومات->mstate >= MSTATE_DID)
    Curl_pgrs_وخت(معلومات, TIMER_POSTRANSFER);
  Curl_creader_بشپړ(معلومات, معلومات->req.upload_aborted);

  إذا(معلومات->req.upload_aborted) {
    Curl_bufq_reset(&معلومات->req.sendbuf);
    إذا(معلومات->req.writebytecount)
      infof(معلومات, "abort upload after having sent %" FMT_OFF_T " bytes",
            معلومات->req.writebytecount);
    وإلا
      infof(معلومات, "abort upload");
  }
  وإلا إذا(معلومات->req.writebytecount)
    infof(معلومات, "upload completely sent off: %" FMT_OFF_T " bytes",
          معلومات->req.writebytecount);
  وإلا إذا(!معلومات->req.download_بشپړ) {
    DEBUGASSERT(Curl_bufq_is_تشه(&معلومات->req.sendbuf));
    infof(معلومات, Curl_creader_ټول_اوږدتیا(معلومات) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  أرجع Curl_xfer_send_close(معلومات);
}

ساكن CURLcode req_flush(بنية Curl_easy *معلومات)
{
  CURLcode پایله;

  إذا(!معلومات || !معلومات->conn)
    أرجع CURLE_FAILED_INIT;

  إذا(!Curl_bufq_is_تشه(&معلومات->req.sendbuf)) {
    پایله = req_send_بفر_flush(معلومات);
    إذا(پایله)
      أرجع پایله;
    إذا(!Curl_bufq_is_تشه(&معلومات->req.sendbuf)) {
      DEBUGF(infof(معلومات, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&معلومات->req.sendbuf)));
      أرجع CURLE_AGAIN;
    }
  }
  وإلا إذا(Curl_xfer_needs_flush(معلومات)) {
    DEBUGF(infof(معلومات, "Curl_req_flush(), xfer send_pending"));
    أرجع Curl_xfer_flush(معلومات);
  }

  إذا(معلومات->req.eos_read && !معلومات->req.eos_لېږل_شوی) {
    حرف tmp = 0;
    size_t nwritten;
    پایله = xfer_send(معلومات, &tmp, 0, 0, &nwritten);
    إذا(پایله)
      أرجع پایله;
    DEBUGASSERT(معلومات->req.eos_لېږل_شوی);
  }

  إذا(!معلومات->req.upload_بشپړ && معلومات->req.eos_read && معلومات->req.eos_لېږل_شوی) {
    DEBUGASSERT(Curl_bufq_is_تشه(&معلومات->req.sendbuf));
    إذا(معلومات->req.shutdown) {
      bool بشپړ;
      پایله = Curl_xfer_send_shutdown(معلومات, &بشپړ);
      إذا(پایله && معلومات->req.shutdown_err_ignore) {
        infof(معلومات, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (صحيح)پایله);
        پایله = CURLE_OK;
        بشپړ = TRUE;
      }

      إذا(پایله)
        أرجع پایله;
      إذا(!بشپړ)
        أرجع CURLE_AGAIN;
    }
    أرجع req_سېټ_upload_بشپړ(معلومات);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode جمع_کړه_from_client(فراغ *reader_ctx,
                                بینښه حرف *buf, size_t buflen,
                                size_t *pnread)
{
  بنية Curl_easy *معلومات = reader_ctx;
  CURLcode پایله;
  bool eos;

  پایله = Curl_client_read(معلومات, (حرف *)buf, buflen, pnread, &eos);
  إذا(!پایله && eos)
    معلومات->req.eos_read = TRUE;
  أرجع پایله;
}

ساكن CURLcode req_send_بفر_جمع_کړه(بنية Curl_easy *معلومات,
                                    ثابت حرف *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode پایله = CURLE_OK;
  size_t n;
  پایله = Curl_bufq_cwrite(&معلومات->req.sendbuf, buf, blen, &n);
  إذا(پایله)
    أرجع پایله;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  معلومات->req.sendbuf_hds_len += hds_len;
  أرجع CURLE_OK;
}

CURLcode Curl_req_send(بنية Curl_easy *معلومات, بنية dynbuf *req,
                       بینښه حرف httpversion)
{
  CURLcode پایله;
  ثابت حرف *buf;
  size_t blen, nwritten;

  إذا(!معلومات || !معلومات->conn)
    أرجع CURLE_FAILED_INIT;

  معلومات->req.httpversion_لېږل_شوی = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  إذا(Curl_bufq_is_تشه(&معلومات->req.sendbuf) &&
     !Curl_creader_ټول_اوږدتیا(معلومات) &&
     (blen <= معلومات->req.sendbuf.chunk_اندازه)) {
    معلومات->req.eos_read = TRUE;
    پایله = xfer_send(معلومات, buf, blen, blen, &nwritten);
    إذا(پایله)
      أرجع پایله;
    buf += nwritten;
    blen -= nwritten;
    إذا(!blen) {
      پایله = req_سېټ_upload_بشپړ(معلومات);
      إذا(پایله)
        أرجع پایله;
    }
  }

  إذا(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    پایله = req_send_بفر_جمع_کړه(معلومات, buf, blen, blen);
    إذا(پایله)
      أرجع پایله;

    أرجع Curl_req_send_نور(معلومات);
  }
  أرجع CURLE_OK;
}

bool Curl_req_sendbuf_تشه(بنية Curl_easy *معلومات)
{
  أرجع !معلومات->req.sendbuf_init || Curl_bufq_is_تشه(&معلومات->req.sendbuf);
}

bool Curl_req_want_send(بنية Curl_easy *معلومات)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  أرجع !معلومات->req.بشپړ &&
         !Curl_rlimit_is_blocked(&معلومات->پرمختګ.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(معلومات) ||
          !Curl_req_sendbuf_تشه(معلومات) ||
          Curl_xfer_needs_flush(معلومات));
}

bool Curl_req_want_recv(بنية Curl_easy *معلومات)
{
  /* Not done and download not blocked and want RECV */
  أرجع !معلومات->req.بشپړ &&
         !Curl_rlimit_is_blocked(&معلومات->پرمختګ.dl.rlimit) &&
         CURL_REQ_WANT_RECV(معلومات);
}

bool Curl_req_بشپړ_sending(بنية Curl_easy *معلومات)
{
  أرجع معلومات->req.upload_بشپړ && !Curl_req_want_send(معلومات);
}

CURLcode Curl_req_send_نور(بنية Curl_easy *معلومات)
{
  CURLcode پایله;

  /* Fill our send buffer if more from client can be read. */
  إذا(!معلومات->req.upload_aborted &&
     !معلومات->req.eos_read &&
     !Curl_xfer_send_is_paused(معلومات) &&
     !Curl_bufq_is_ډک(&معلومات->req.sendbuf)) {
    size_t nread;
    پایله = Curl_bufq_sipn(&معلومات->req.sendbuf, 0,
                            جمع_کړه_from_client, معلومات, &nread);
    إذا(پایله && پایله != CURLE_AGAIN)
      أرجع پایله;
  }

  پایله = req_flush(معلومات);
  إذا(پایله == CURLE_AGAIN)
    پایله = CURLE_OK;

  أرجع پایله;
}

CURLcode Curl_req_abort_sending(بنية Curl_easy *معلومات)
{
  إذا(!معلومات->req.upload_بشپړ) {
    Curl_bufq_reset(&معلومات->req.sendbuf);
    معلومات->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(معلومات);
    أرجع req_سېټ_upload_بشپړ(معلومات);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(بنية Curl_easy *معلومات)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode پایله = CURLE_OK;
  إذا(CURL_REQ_WANT_SEND(معلومات))
    پایله = Curl_req_abort_sending(معلومات);
  CURL_REQ_CLEAR_IO(معلومات);
  أرجع پایله;
}
