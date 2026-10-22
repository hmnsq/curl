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
  CURLcode نتيجو;

  req->پورو = FALSE;
  req->upload_پورو = FALSE;
  req->upload_aborted = FALSE;
  req->download_پورو = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_موڪليل = FALSE;
  req->ignorebody = FALSE;
  req->shutdown = FALSE;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->سرو = FALSE;
  req->headerline = 0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->httpversion_موڪليل = 0;
  req->httpversion = 0;
  req->sendbuf_hds_len = 0;

  curlx_safefree(req->hd_auth);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#جيڪڏهن_پورو

  نتيجو = Curl_client_شروعات(معلومات);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, معلومات->سيٽ.upload_بفر_سائيز, 1,
                    BUFQ_OPT_SOFT_حد_بندي);
    req->sendbuf_init = TRUE;
  }
  وإلا {
    Curl_bufq_reset(&req->sendbuf);
    إذا(معلومات->سيٽ.upload_بفر_سائيز != req->sendbuf.chunk_سائيز) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, معلومات->سيٽ.upload_بفر_سائيز, 1,
                      BUFQ_OPT_SOFT_حد_بندي);
    }
  }

  أرجع CURLE_OK;
}

CURLcode Curl_req_شروعات(بنية SingleRequest *req,
                        بنية Curl_easy *معلومات)
{
  req->شروعات = *Curl_pgrs_now(معلومات);
  أرجع Curl_req_soft_reset(req, معلومات);
}

ساكن CURLcode req_flush(بنية Curl_easy *معلومات);

CURLcode Curl_req_پورو(بنية SingleRequest *req,
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
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#جيڪڏهن_پورو
  Curl_client_reset(معلومات);
  إذا(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(معلومات);
  /* Can no longer memset() this struct as we need to keep some state */
  req->سائيز = -1;
  req->maxdownload = -1;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->شروعات = t0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->headerline = 0;
  req->وٿي = 0;
  req->httpcode = 0;
  req->io_flags = 0;
  req->upgr101 = UPGR101_NONE;
  req->sendbuf_hds_len = 0;
  req->timeofdoc = 0;
  req->location = عدم;
  req->newurl = عدم;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
  req->setcookies = 0;
#جيڪڏهن_پورو
  req->سرو = FALSE;
  req->content_range = FALSE;
  req->download_پورو = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_موڪليل = FALSE;
  req->rewind_read = FALSE;
  req->upload_پورو = FALSE;
  req->upload_aborted = FALSE;
  req->ignorebody = FALSE;
  req->http_bodyless = FALSE;
  req->chunk = FALSE;
  req->resp_trailer = FALSE;
  req->ignore_cl = FALSE;
  req->upload_chunky = FALSE;
  req->no_body = معلومات->سيٽ.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_بلاڪ(&معلومات->ترقي.dl.rlimit, FALSE, &t0);
  Curl_rlimit_بلاڪ(&معلومات->ترقي.ul.rlimit, FALSE, &t0);
}

فراغ Curl_req_free(بنية SingleRequest *req, بنية Curl_easy *معلومات)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#جيڪڏهن_پورو
  إذا(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(معلومات);
}

ساكن CURLcode xfer_send(بنية Curl_easy *معلومات,
                          ثابت حرف *buf, size_t blen,
                          size_t hds_len, size_t *pnwritten)
{
  CURLcode نتيجو = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
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
#جيڪڏهن_پورو
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  إذا(معلومات->سيٽ.max_send_رفتار) {
    size_t body_بائيٽون = blen - hds_len;
    إذا((curl_off_t)body_بائيٽون > معلومات->سيٽ.max_send_رفتار)
      blen = hds_len + (size_t)معلومات->سيٽ.max_send_رفتار;
  }

  إذا(معلومات->req.eos_read &&
    (Curl_bufq_is_خالي_جايون(&معلومات->req.sendbuf) ||
     Curl_bufq_len(&معلومات->req.sendbuf) == blen)) {
    DEBUGF(infof(معلومات, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  نتيجو = Curl_xfer_send(معلومات, buf, blen, eos, pnwritten);
  إذا(!نتيجو) {
    إذا(eos && (blen == *pnwritten))
      معلومات->req.eos_موڪليل = TRUE;
    إذا(*pnwritten) {
      إذا(hds_len)
        Curl_debug(معلومات, CURLINFO_سرو_ٻاهريون, buf,
                   CURLMIN(hds_len, *pnwritten));
      إذا(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(معلومات, CURLINFO_معلومات_ٻاهريون, buf + hds_len, body_len);
        معلومات->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(معلومات, body_len);
      }
    }
  }
  أرجع نتيجو;
}

ساكن CURLcode req_send_بفر_flush(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت بنانشان حرف *buf;
  size_t blen;

  طالما(Curl_bufq_peek(&معلومات->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(معلومات->req.sendbuf_hds_len, blen);
    نتيجو = xfer_send(معلومات, (ثابت حرف *)buf, blen, hds_len, &nwritten);
    إذا(نتيجو)
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
  أرجع نتيجو;
}

ساكن CURLcode req_سيٽ_upload_پورو(بنية Curl_easy *معلومات)
{
  DEBUGASSERT(!معلومات->req.upload_پورو);
  معلومات->req.upload_پورو = TRUE;
  CURL_REQ_CLEAR_SEND(معلومات);

  إذا(معلومات->mstate >= MSTATE_DID)
    Curl_pgrs_وقت(معلومات, TIMER_POSTRANSFER);
  Curl_creader_پورو(معلومات, معلومات->req.upload_aborted);

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
  وإلا إذا(!معلومات->req.download_پورو) {
    DEBUGASSERT(Curl_bufq_is_خالي_جايون(&معلومات->req.sendbuf));
    infof(معلومات, Curl_creader_ڪل_ڊيگهائي(معلومات) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  أرجع Curl_xfer_send_close(معلومات);
}

ساكن CURLcode req_flush(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو;

  إذا(!معلومات || !معلومات->conn)
    أرجع CURLE_FAILED_INIT;

  إذا(!Curl_bufq_is_خالي_جايون(&معلومات->req.sendbuf)) {
    نتيجو = req_send_بفر_flush(معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
    إذا(!Curl_bufq_is_خالي_جايون(&معلومات->req.sendbuf)) {
      DEBUGF(infof(معلومات, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&معلومات->req.sendbuf)));
      أرجع CURLE_AGAIN;
    }
  }
  وإلا إذا(Curl_xfer_needs_flush(معلومات)) {
    DEBUGF(infof(معلومات, "Curl_req_flush(), xfer send_pending"));
    أرجع Curl_xfer_flush(معلومات);
  }

  إذا(معلومات->req.eos_read && !معلومات->req.eos_موڪليل) {
    حرف tmp = 0;
    size_t nwritten;
    نتيجو = xfer_send(معلومات, &tmp, 0, 0, &nwritten);
    إذا(نتيجو)
      أرجع نتيجو;
    DEBUGASSERT(معلومات->req.eos_موڪليل);
  }

  إذا(!معلومات->req.upload_پورو && معلومات->req.eos_read && معلومات->req.eos_موڪليل) {
    DEBUGASSERT(Curl_bufq_is_خالي_جايون(&معلومات->req.sendbuf));
    إذا(معلومات->req.shutdown) {
      bool پورو;
      نتيجو = Curl_xfer_send_shutdown(معلومات, &پورو);
      إذا(نتيجو && معلومات->req.shutdown_err_ignore) {
        infof(معلومات, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (صحيح)نتيجو);
        نتيجو = CURLE_OK;
        پورو = TRUE;
      }

      إذا(نتيجو)
        أرجع نتيجو;
      إذا(!پورو)
        أرجع CURLE_AGAIN;
    }
    أرجع req_سيٽ_upload_پورو(معلومات);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode جوڙيو_from_client(فراغ *reader_ctx,
                                بنانشان حرف *buf, size_t buflen,
                                size_t *pnread)
{
  بنية Curl_easy *معلومات = reader_ctx;
  CURLcode نتيجو;
  bool eos;

  نتيجو = Curl_client_read(معلومات, (حرف *)buf, buflen, pnread, &eos);
  إذا(!نتيجو && eos)
    معلومات->req.eos_read = TRUE;
  أرجع نتيجو;
}

ساكن CURLcode req_send_بفر_جوڙيو(بنية Curl_easy *معلومات,
                                    ثابت حرف *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode نتيجو = CURLE_OK;
  size_t n;
  نتيجو = Curl_bufq_cwrite(&معلومات->req.sendbuf, buf, blen, &n);
  إذا(نتيجو)
    أرجع نتيجو;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  معلومات->req.sendbuf_hds_len += hds_len;
  أرجع CURLE_OK;
}

CURLcode Curl_req_send(بنية Curl_easy *معلومات, بنية dynbuf *req,
                       بنانشان حرف httpversion)
{
  CURLcode نتيجو;
  ثابت حرف *buf;
  size_t blen, nwritten;

  إذا(!معلومات || !معلومات->conn)
    أرجع CURLE_FAILED_INIT;

  معلومات->req.httpversion_موڪليل = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  إذا(Curl_bufq_is_خالي_جايون(&معلومات->req.sendbuf) &&
     !Curl_creader_ڪل_ڊيگهائي(معلومات) &&
     (blen <= معلومات->req.sendbuf.chunk_سائيز)) {
    معلومات->req.eos_read = TRUE;
    نتيجو = xfer_send(معلومات, buf, blen, blen, &nwritten);
    إذا(نتيجو)
      أرجع نتيجو;
    buf += nwritten;
    blen -= nwritten;
    إذا(!blen) {
      نتيجو = req_سيٽ_upload_پورو(معلومات);
      إذا(نتيجو)
        أرجع نتيجو;
    }
  }

  إذا(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    نتيجو = req_send_بفر_جوڙيو(معلومات, buf, blen, blen);
    إذا(نتيجو)
      أرجع نتيجو;

    أرجع Curl_req_send_وڌيڪ(معلومات);
  }
  أرجع CURLE_OK;
}

bool Curl_req_sendbuf_خالي_جايون(بنية Curl_easy *معلومات)
{
  أرجع !معلومات->req.sendbuf_init || Curl_bufq_is_خالي_جايون(&معلومات->req.sendbuf);
}

bool Curl_req_want_send(بنية Curl_easy *معلومات)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  أرجع !معلومات->req.پورو &&
         !Curl_rlimit_is_blocked(&معلومات->ترقي.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(معلومات) ||
          !Curl_req_sendbuf_خالي_جايون(معلومات) ||
          Curl_xfer_needs_flush(معلومات));
}

bool Curl_req_want_recv(بنية Curl_easy *معلومات)
{
  /* Not done and download not blocked and want RECV */
  أرجع !معلومات->req.پورو &&
         !Curl_rlimit_is_blocked(&معلومات->ترقي.dl.rlimit) &&
         CURL_REQ_WANT_RECV(معلومات);
}

bool Curl_req_پورو_sending(بنية Curl_easy *معلومات)
{
  أرجع معلومات->req.upload_پورو && !Curl_req_want_send(معلومات);
}

CURLcode Curl_req_send_وڌيڪ(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو;

  /* Fill our send buffer if more from client can be read. */
  إذا(!معلومات->req.upload_aborted &&
     !معلومات->req.eos_read &&
     !Curl_xfer_send_is_paused(معلومات) &&
     !Curl_bufq_is_ڀريل(&معلومات->req.sendbuf)) {
    size_t nread;
    نتيجو = Curl_bufq_sipn(&معلومات->req.sendbuf, 0,
                            جوڙيو_from_client, معلومات, &nread);
    إذا(نتيجو && نتيجو != CURLE_AGAIN)
      أرجع نتيجو;
  }

  نتيجو = req_flush(معلومات);
  إذا(نتيجو == CURLE_AGAIN)
    نتيجو = CURLE_OK;

  أرجع نتيجو;
}

CURLcode Curl_req_abort_sending(بنية Curl_easy *معلومات)
{
  إذا(!معلومات->req.upload_پورو) {
    Curl_bufq_reset(&معلومات->req.sendbuf);
    معلومات->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(معلومات);
    أرجع req_سيٽ_upload_پورو(معلومات);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(بنية Curl_easy *معلومات)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode نتيجو = CURLE_OK;
  إذا(CURL_REQ_WANT_SEND(معلومات))
    نتيجو = Curl_req_abort_sending(معلومات);
  CURL_REQ_CLEAR_IO(معلومات);
  أرجع نتيجو;
}
