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
                             بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ;

  req->پورا = FALSE;
  req->upload_پورا = FALSE;
  req->upload_aborted = FALSE;
  req->download_پورا = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_گھلیا = FALSE;
  req->ignorebody = FALSE;
  req->shutdown = FALSE;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->سرخی = FALSE;
  req->headerline = 0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->httpversion_گھلیا = 0;
  req->httpversion = 0;
  req->sendbuf_hds_len = 0;

  curlx_safefree(req->hd_auth);
#جے_بنا_تعریف CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#جے_انت

  نتیجہ = Curl_client_شروع(انکڑے);
  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, انکڑے->سیٹ.upload_بفر_ناپ, 1,
                    BUFQ_OPT_SOFT_حد_بندی);
    req->sendbuf_init = TRUE;
  }
  وإلا {
    Curl_bufq_reset(&req->sendbuf);
    إذا(انکڑے->سیٹ.upload_بفر_ناپ != req->sendbuf.chunk_ناپ) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, انکڑے->سیٹ.upload_بفر_ناپ, 1,
                      BUFQ_OPT_SOFT_حد_بندی);
    }
  }

  أرجع CURLE_OK;
}

CURLcode Curl_req_شروع(بنية SingleRequest *req,
                        بنية Curl_easy *انکڑے)
{
  req->شروع = *Curl_pgrs_now(انکڑے);
  أرجع Curl_req_soft_reset(req, انکڑے);
}

ساكن CURLcode req_flush(بنية Curl_easy *انکڑے);

CURLcode Curl_req_پورا(بنية SingleRequest *req,
                       بنية Curl_easy *انکڑے, bool aborted)
{
  (فراغ)req;
  إذا(!aborted)
    (فراغ)req_flush(انکڑے);
  Curl_client_reset(انکڑے);
  أرجع CURLE_OK;
}

فراغ Curl_req_hard_reset(بنية SingleRequest *req, بنية Curl_easy *انکڑے)
{
  بنية curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#جے_بنا_تعریف CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#جے_انت
  Curl_client_reset(انکڑے);
  إذا(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(انکڑے);
  /* Can no longer memset() this struct as we need to keep some state */
  req->ناپ = -1;
  req->maxdownload = -1;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->شروع = t0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->headerline = 0;
  req->وتھ = 0;
  req->httpcode = 0;
  req->io_flags = 0;
  req->upgr101 = UPGR101_NONE;
  req->sendbuf_hds_len = 0;
  req->timeofdoc = 0;
  req->location = عدم;
  req->newurl = عدم;
#جے_بنا_تعریف CURL_DISABLE_COOKIES
  req->setcookies = 0;
#جے_انت
  req->سرخی = FALSE;
  req->content_range = FALSE;
  req->download_پورا = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_گھلیا = FALSE;
  req->rewind_read = FALSE;
  req->upload_پورا = FALSE;
  req->upload_aborted = FALSE;
  req->ignorebody = FALSE;
  req->http_bodyless = FALSE;
  req->chunk = FALSE;
  req->resp_trailer = FALSE;
  req->ignore_cl = FALSE;
  req->upload_chunky = FALSE;
  req->no_body = انکڑے->سیٹ.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_بلاک(&انکڑے->ترقی.dl.rlimit, FALSE, &t0);
  Curl_rlimit_بلاک(&انکڑے->ترقی.ul.rlimit, FALSE, &t0);
}

فراغ Curl_req_free(بنية SingleRequest *req, بنية Curl_easy *انکڑے)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#جے_بنا_تعریف CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#جے_انت
  إذا(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(انکڑے);
}

ساكن CURLcode xfer_send(بنية Curl_easy *انکڑے,
                          ثابت حرف *buf, size_t blen,
                          size_t hds_len, size_t *pnwritten)
{
  CURLcode نتیجہ = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#جے_تعریف_شدہ DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    size_t body_len = blen - hds_len;
    إذا(body_len) {
      ثابت حرف *p = getenv("CURL_SMALLREQSEND");
      إذا(p) {
        curl_off_t body_small;
        إذا(!curlx_str_نمبر(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#جے_انت
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  إذا(انکڑے->سیٹ.max_send_رفتار) {
    size_t body_بائٹاں = blen - hds_len;
    إذا((curl_off_t)body_بائٹاں > انکڑے->سیٹ.max_send_رفتار)
      blen = hds_len + (size_t)انکڑے->سیٹ.max_send_رفتار;
  }

  إذا(انکڑے->req.eos_read &&
    (Curl_bufq_is_خالی_تھاواں(&انکڑے->req.sendbuf) ||
     Curl_bufq_len(&انکڑے->req.sendbuf) == blen)) {
    DEBUGF(infof(انکڑے, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  نتیجہ = Curl_xfer_send(انکڑے, buf, blen, eos, pnwritten);
  إذا(!نتیجہ) {
    إذا(eos && (blen == *pnwritten))
      انکڑے->req.eos_گھلیا = TRUE;
    إذا(*pnwritten) {
      إذا(hds_len)
        Curl_debug(انکڑے, CURLINFO_سرخی_باہرلا, buf,
                   CURLMIN(hds_len, *pnwritten));
      إذا(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(انکڑے, CURLINFO_انکڑے_باہرلا, buf + hds_len, body_len);
        انکڑے->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(انکڑے, body_len);
      }
    }
  }
  أرجع نتیجہ;
}

ساكن CURLcode req_send_بفر_flush(بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت بنانشانی حرف *buf;
  size_t blen;

  طالما(Curl_bufq_peek(&انکڑے->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(انکڑے->req.sendbuf_hds_len, blen);
    نتیجہ = xfer_send(انکڑے, (ثابت حرف *)buf, blen, hds_len, &nwritten);
    إذا(نتیجہ)
      اكسر;

    Curl_bufq_skip(&انکڑے->req.sendbuf, nwritten);
    إذا(hds_len) {
      انکڑے->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    إذا(nwritten < blen)
      اكسر;
  }
  أرجع نتیجہ;
}

ساكن CURLcode req_سیٹ_upload_پورا(بنية Curl_easy *انکڑے)
{
  DEBUGASSERT(!انکڑے->req.upload_پورا);
  انکڑے->req.upload_پورا = TRUE;
  CURL_REQ_CLEAR_SEND(انکڑے);

  إذا(انکڑے->mstate >= MSTATE_DID)
    Curl_pgrs_ویلا(انکڑے, TIMER_POSTRANSFER);
  Curl_creader_پورا(انکڑے, انکڑے->req.upload_aborted);

  إذا(انکڑے->req.upload_aborted) {
    Curl_bufq_reset(&انکڑے->req.sendbuf);
    إذا(انکڑے->req.writebytecount)
      infof(انکڑے, "abort upload after having sent %" FMT_OFF_T " bytes",
            انکڑے->req.writebytecount);
    وإلا
      infof(انکڑے, "abort upload");
  }
  وإلا إذا(انکڑے->req.writebytecount)
    infof(انکڑے, "upload completely sent off: %" FMT_OFF_T " bytes",
          انکڑے->req.writebytecount);
  وإلا إذا(!انکڑے->req.download_پورا) {
    DEBUGASSERT(Curl_bufq_is_خالی_تھاواں(&انکڑے->req.sendbuf));
    infof(انکڑے, Curl_creader_کل_لمائی(انکڑے) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  أرجع Curl_xfer_send_close(انکڑے);
}

ساكن CURLcode req_flush(بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ;

  إذا(!انکڑے || !انکڑے->conn)
    أرجع CURLE_FAILED_INIT;

  إذا(!Curl_bufq_is_خالی_تھاواں(&انکڑے->req.sendbuf)) {
    نتیجہ = req_send_بفر_flush(انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
    إذا(!Curl_bufq_is_خالی_تھاواں(&انکڑے->req.sendbuf)) {
      DEBUGF(infof(انکڑے, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&انکڑے->req.sendbuf)));
      أرجع CURLE_AGAIN;
    }
  }
  وإلا إذا(Curl_xfer_needs_flush(انکڑے)) {
    DEBUGF(infof(انکڑے, "Curl_req_flush(), xfer send_pending"));
    أرجع Curl_xfer_flush(انکڑے);
  }

  إذا(انکڑے->req.eos_read && !انکڑے->req.eos_گھلیا) {
    حرف tmp = 0;
    size_t nwritten;
    نتیجہ = xfer_send(انکڑے, &tmp, 0, 0, &nwritten);
    إذا(نتیجہ)
      أرجع نتیجہ;
    DEBUGASSERT(انکڑے->req.eos_گھلیا);
  }

  إذا(!انکڑے->req.upload_پورا && انکڑے->req.eos_read && انکڑے->req.eos_گھلیا) {
    DEBUGASSERT(Curl_bufq_is_خالی_تھاواں(&انکڑے->req.sendbuf));
    إذا(انکڑے->req.shutdown) {
      bool پورا;
      نتیجہ = Curl_xfer_send_shutdown(انکڑے, &پورا);
      إذا(نتیجہ && انکڑے->req.shutdown_err_ignore) {
        infof(انکڑے, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (صحيح)نتیجہ);
        نتیجہ = CURLE_OK;
        پورا = TRUE;
      }

      إذا(نتیجہ)
        أرجع نتیجہ;
      إذا(!پورا)
        أرجع CURLE_AGAIN;
    }
    أرجع req_سیٹ_upload_پورا(انکڑے);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode جوڑو_from_client(فراغ *reader_ctx,
                                بنانشانی حرف *buf, size_t buflen,
                                size_t *pnread)
{
  بنية Curl_easy *انکڑے = reader_ctx;
  CURLcode نتیجہ;
  bool eos;

  نتیجہ = Curl_client_read(انکڑے, (حرف *)buf, buflen, pnread, &eos);
  إذا(!نتیجہ && eos)
    انکڑے->req.eos_read = TRUE;
  أرجع نتیجہ;
}

ساكن CURLcode req_send_بفر_جوڑو(بنية Curl_easy *انکڑے,
                                    ثابت حرف *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t n;
  نتیجہ = Curl_bufq_cwrite(&انکڑے->req.sendbuf, buf, blen, &n);
  إذا(نتیجہ)
    أرجع نتیجہ;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  انکڑے->req.sendbuf_hds_len += hds_len;
  أرجع CURLE_OK;
}

CURLcode Curl_req_send(بنية Curl_easy *انکڑے, بنية dynbuf *req,
                       بنانشانی حرف httpversion)
{
  CURLcode نتیجہ;
  ثابت حرف *buf;
  size_t blen, nwritten;

  إذا(!انکڑے || !انکڑے->conn)
    أرجع CURLE_FAILED_INIT;

  انکڑے->req.httpversion_گھلیا = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  إذا(Curl_bufq_is_خالی_تھاواں(&انکڑے->req.sendbuf) &&
     !Curl_creader_کل_لمائی(انکڑے) &&
     (blen <= انکڑے->req.sendbuf.chunk_ناپ)) {
    انکڑے->req.eos_read = TRUE;
    نتیجہ = xfer_send(انکڑے, buf, blen, blen, &nwritten);
    إذا(نتیجہ)
      أرجع نتیجہ;
    buf += nwritten;
    blen -= nwritten;
    إذا(!blen) {
      نتیجہ = req_سیٹ_upload_پورا(انکڑے);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
  }

  إذا(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    نتیجہ = req_send_بفر_جوڑو(انکڑے, buf, blen, blen);
    إذا(نتیجہ)
      أرجع نتیجہ;

    أرجع Curl_req_send_ہور(انکڑے);
  }
  أرجع CURLE_OK;
}

bool Curl_req_sendbuf_خالی_تھاواں(بنية Curl_easy *انکڑے)
{
  أرجع !انکڑے->req.sendbuf_init || Curl_bufq_is_خالی_تھاواں(&انکڑے->req.sendbuf);
}

bool Curl_req_want_send(بنية Curl_easy *انکڑے)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  أرجع !انکڑے->req.پورا &&
         !Curl_rlimit_is_blocked(&انکڑے->ترقی.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(انکڑے) ||
          !Curl_req_sendbuf_خالی_تھاواں(انکڑے) ||
          Curl_xfer_needs_flush(انکڑے));
}

bool Curl_req_want_recv(بنية Curl_easy *انکڑے)
{
  /* Not done and download not blocked and want RECV */
  أرجع !انکڑے->req.پورا &&
         !Curl_rlimit_is_blocked(&انکڑے->ترقی.dl.rlimit) &&
         CURL_REQ_WANT_RECV(انکڑے);
}

bool Curl_req_پورا_sending(بنية Curl_easy *انکڑے)
{
  أرجع انکڑے->req.upload_پورا && !Curl_req_want_send(انکڑے);
}

CURLcode Curl_req_send_ہور(بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ;

  /* Fill our send buffer if more from client can be read. */
  إذا(!انکڑے->req.upload_aborted &&
     !انکڑے->req.eos_read &&
     !Curl_xfer_send_is_paused(انکڑے) &&
     !Curl_bufq_is_بھریا(&انکڑے->req.sendbuf)) {
    size_t nread;
    نتیجہ = Curl_bufq_sipn(&انکڑے->req.sendbuf, 0,
                            جوڑو_from_client, انکڑے, &nread);
    إذا(نتیجہ && نتیجہ != CURLE_AGAIN)
      أرجع نتیجہ;
  }

  نتیجہ = req_flush(انکڑے);
  إذا(نتیجہ == CURLE_AGAIN)
    نتیجہ = CURLE_OK;

  أرجع نتیجہ;
}

CURLcode Curl_req_abort_sending(بنية Curl_easy *انکڑے)
{
  إذا(!انکڑے->req.upload_پورا) {
    Curl_bufq_reset(&انکڑے->req.sendbuf);
    انکڑے->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(انکڑے);
    أرجع req_سیٹ_upload_پورا(انکڑے);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(بنية Curl_easy *انکڑے)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode نتیجہ = CURLE_OK;
  إذا(CURL_REQ_WANT_SEND(انکڑے))
    نتیجہ = Curl_req_abort_sending(انکڑے);
  CURL_REQ_CLEAR_IO(انکڑے);
  أرجع نتیجہ;
}
