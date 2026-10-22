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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "curlx/dynbuf.h"
#شامل "progress.h"
#شامل "request.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "transfer.h"
#شامل "url.h"
#شامل "curlx/strparse.h"

خالی Curl_req_init(ساخت SingleRequest *req)
{
  memset(req, 0, حجم(*req));
}

CURLcode Curl_req_soft_reset(ساخت SingleRequest *req,
                             ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ;

  req->مکمل = FALSE;
  req->upload_مکمل = FALSE;
  req->upload_aborted = FALSE;
  req->download_مکمل = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_بھیجا = FALSE;
  req->ignorebody = FALSE;
  req->shutdown = FALSE;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->سرخی = FALSE;
  req->headerline = 0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->httpversion_بھیجا = 0;
  req->httpversion = 0;
  req->sendbuf_hds_len = 0;

  curlx_safefree(req->hd_auth);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#اگر_ختم

  نتیجہ = Curl_client_آغاز(ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, ڈیٹا->سیٹ.upload_بفر_سائز, 1,
                    BUFQ_OPT_SOFT_حد_بندی);
    req->sendbuf_init = TRUE;
  }
  ورنہ {
    Curl_bufq_reset(&req->sendbuf);
    اگر(ڈیٹا->سیٹ.upload_بفر_سائز != req->sendbuf.chunk_سائز) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, ڈیٹا->سیٹ.upload_بفر_سائز, 1,
                      BUFQ_OPT_SOFT_حد_بندی);
    }
  }

  واپس CURLE_OK;
}

CURLcode Curl_req_آغاز(ساخت SingleRequest *req,
                        ساخت Curl_easy *ڈیٹا)
{
  req->آغاز = *Curl_pgrs_now(ڈیٹا);
  واپس Curl_req_soft_reset(req, ڈیٹا);
}

ساکن CURLcode req_flush(ساخت Curl_easy *ڈیٹا);

CURLcode Curl_req_مکمل(ساخت SingleRequest *req,
                       ساخت Curl_easy *ڈیٹا, bool aborted)
{
  (خالی)req;
  اگر(!aborted)
    (خالی)req_flush(ڈیٹا);
  Curl_client_reset(ڈیٹا);
  واپس CURLE_OK;
}

خالی Curl_req_hard_reset(ساخت SingleRequest *req, ساخت Curl_easy *ڈیٹا)
{
  ساخت curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#اگر_ختم
  Curl_client_reset(ڈیٹا);
  اگر(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(ڈیٹا);
  /* Can no longer memset() this struct as we need to keep some state */
  req->سائز = -1;
  req->maxdownload = -1;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->آغاز = t0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->headerline = 0;
  req->انحراف = 0;
  req->httpcode = 0;
  req->io_flags = 0;
  req->upgr101 = UPGR101_NONE;
  req->sendbuf_hds_len = 0;
  req->timeofdoc = 0;
  req->location = تہی;
  req->newurl = تہی;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
  req->setcookies = 0;
#اگر_ختم
  req->سرخی = FALSE;
  req->content_range = FALSE;
  req->download_مکمل = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_بھیجا = FALSE;
  req->rewind_read = FALSE;
  req->upload_مکمل = FALSE;
  req->upload_aborted = FALSE;
  req->ignorebody = FALSE;
  req->http_bodyless = FALSE;
  req->chunk = FALSE;
  req->resp_trailer = FALSE;
  req->ignore_cl = FALSE;
  req->upload_chunky = FALSE;
  req->no_body = ڈیٹا->سیٹ.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_بلاک(&ڈیٹا->پیش_رفت.dl.rlimit, FALSE, &t0);
  Curl_rlimit_بلاک(&ڈیٹا->پیش_رفت.ul.rlimit, FALSE, &t0);
}

خالی Curl_req_free(ساخت SingleRequest *req, ساخت Curl_easy *ڈیٹا)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#اگر_ختم
  اگر(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(ڈیٹا);
}

ساکن CURLcode xfer_send(ساخت Curl_easy *ڈیٹا,
                          ثابت حرف *buf, size_t blen,
                          size_t hds_len, size_t *pnwritten)
{
  CURLcode نتیجہ = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#اگر_تعریف_شدہ DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    size_t body_len = blen - hds_len;
    اگر(body_len) {
      ثابت حرف *p = getenv("CURL_SMALLREQSEND");
      اگر(p) {
        curl_off_t body_small;
        اگر(!curlx_str_نمبر(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#اگر_ختم
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  اگر(ڈیٹا->سیٹ.max_send_رفتار) {
    size_t body_بائٹیں = blen - hds_len;
    اگر((curl_off_t)body_بائٹیں > ڈیٹا->سیٹ.max_send_رفتار)
      blen = hds_len + (size_t)ڈیٹا->سیٹ.max_send_رفتار;
  }

  اگر(ڈیٹا->req.eos_read &&
    (Curl_bufq_is_خالی_خانے(&ڈیٹا->req.sendbuf) ||
     Curl_bufq_len(&ڈیٹا->req.sendbuf) == blen)) {
    DEBUGF(infof(ڈیٹا, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  نتیجہ = Curl_xfer_send(ڈیٹا, buf, blen, eos, pnwritten);
  اگر(!نتیجہ) {
    اگر(eos && (blen == *pnwritten))
      ڈیٹا->req.eos_بھیجا = TRUE;
    اگر(*pnwritten) {
      اگر(hds_len)
        Curl_debug(ڈیٹا, CURLINFO_سرخی_برآمد, buf,
                   CURLMIN(hds_len, *pnwritten));
      اگر(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(ڈیٹا, CURLINFO_ڈیٹا_برآمد, buf + hds_len, body_len);
        ڈیٹا->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(ڈیٹا, body_len);
      }
    }
  }
  واپس نتیجہ;
}

ساکن CURLcode req_send_بفر_flush(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت غیرعلامتی حرف *buf;
  size_t blen;

  جب(Curl_bufq_peek(&ڈیٹا->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(ڈیٹا->req.sendbuf_hds_len, blen);
    نتیجہ = xfer_send(ڈیٹا, (ثابت حرف *)buf, blen, hds_len, &nwritten);
    اگر(نتیجہ)
      توڑو;

    Curl_bufq_skip(&ڈیٹا->req.sendbuf, nwritten);
    اگر(hds_len) {
      ڈیٹا->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    اگر(nwritten < blen)
      توڑو;
  }
  واپس نتیجہ;
}

ساکن CURLcode req_سیٹ_upload_مکمل(ساخت Curl_easy *ڈیٹا)
{
  DEBUGASSERT(!ڈیٹا->req.upload_مکمل);
  ڈیٹا->req.upload_مکمل = TRUE;
  CURL_REQ_CLEAR_SEND(ڈیٹا);

  اگر(ڈیٹا->mstate >= MSTATE_DID)
    Curl_pgrs_وقت(ڈیٹا, TIMER_POSTRANSFER);
  Curl_creader_مکمل(ڈیٹا, ڈیٹا->req.upload_aborted);

  اگر(ڈیٹا->req.upload_aborted) {
    Curl_bufq_reset(&ڈیٹا->req.sendbuf);
    اگر(ڈیٹا->req.writebytecount)
      infof(ڈیٹا, "abort upload after having sent %" FMT_OFF_T " bytes",
            ڈیٹا->req.writebytecount);
    ورنہ
      infof(ڈیٹا, "abort upload");
  }
  ورنہ اگر(ڈیٹا->req.writebytecount)
    infof(ڈیٹا, "upload completely sent off: %" FMT_OFF_T " bytes",
          ڈیٹا->req.writebytecount);
  ورنہ اگر(!ڈیٹا->req.download_مکمل) {
    DEBUGASSERT(Curl_bufq_is_خالی_خانے(&ڈیٹا->req.sendbuf));
    infof(ڈیٹا, Curl_creader_کل_لمبائی(ڈیٹا) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  واپس Curl_xfer_send_close(ڈیٹا);
}

ساکن CURLcode req_flush(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ;

  اگر(!ڈیٹا || !ڈیٹا->conn)
    واپس CURLE_FAILED_INIT;

  اگر(!Curl_bufq_is_خالی_خانے(&ڈیٹا->req.sendbuf)) {
    نتیجہ = req_send_بفر_flush(ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
    اگر(!Curl_bufq_is_خالی_خانے(&ڈیٹا->req.sendbuf)) {
      DEBUGF(infof(ڈیٹا, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&ڈیٹا->req.sendbuf)));
      واپس CURLE_AGAIN;
    }
  }
  ورنہ اگر(Curl_xfer_needs_flush(ڈیٹا)) {
    DEBUGF(infof(ڈیٹا, "Curl_req_flush(), xfer send_pending"));
    واپس Curl_xfer_flush(ڈیٹا);
  }

  اگر(ڈیٹا->req.eos_read && !ڈیٹا->req.eos_بھیجا) {
    حرف tmp = 0;
    size_t nwritten;
    نتیجہ = xfer_send(ڈیٹا, &tmp, 0, 0, &nwritten);
    اگر(نتیجہ)
      واپس نتیجہ;
    DEBUGASSERT(ڈیٹا->req.eos_بھیجا);
  }

  اگر(!ڈیٹا->req.upload_مکمل && ڈیٹا->req.eos_read && ڈیٹا->req.eos_بھیجا) {
    DEBUGASSERT(Curl_bufq_is_خالی_خانے(&ڈیٹا->req.sendbuf));
    اگر(ڈیٹا->req.shutdown) {
      bool مکمل;
      نتیجہ = Curl_xfer_send_shutdown(ڈیٹا, &مکمل);
      اگر(نتیجہ && ڈیٹا->req.shutdown_err_ignore) {
        infof(ڈیٹا, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (صحیح)نتیجہ);
        نتیجہ = CURLE_OK;
        مکمل = TRUE;
      }

      اگر(نتیجہ)
        واپس نتیجہ;
      اگر(!مکمل)
        واپس CURLE_AGAIN;
    }
    واپس req_سیٹ_upload_مکمل(ڈیٹا);
  }
  واپس CURLE_OK;
}

ساکن CURLcode جوڑو_from_client(خالی *reader_ctx,
                                غیرعلامتی حرف *buf, size_t buflen,
                                size_t *pnread)
{
  ساخت Curl_easy *ڈیٹا = reader_ctx;
  CURLcode نتیجہ;
  bool eos;

  نتیجہ = Curl_client_read(ڈیٹا, (حرف *)buf, buflen, pnread, &eos);
  اگر(!نتیجہ && eos)
    ڈیٹا->req.eos_read = TRUE;
  واپس نتیجہ;
}

ساکن CURLcode req_send_بفر_جوڑو(ساخت Curl_easy *ڈیٹا,
                                    ثابت حرف *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t n;
  نتیجہ = Curl_bufq_cwrite(&ڈیٹا->req.sendbuf, buf, blen, &n);
  اگر(نتیجہ)
    واپس نتیجہ;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  ڈیٹا->req.sendbuf_hds_len += hds_len;
  واپس CURLE_OK;
}

CURLcode Curl_req_send(ساخت Curl_easy *ڈیٹا, ساخت dynbuf *req,
                       غیرعلامتی حرف httpversion)
{
  CURLcode نتیجہ;
  ثابت حرف *buf;
  size_t blen, nwritten;

  اگر(!ڈیٹا || !ڈیٹا->conn)
    واپس CURLE_FAILED_INIT;

  ڈیٹا->req.httpversion_بھیجا = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  اگر(Curl_bufq_is_خالی_خانے(&ڈیٹا->req.sendbuf) &&
     !Curl_creader_کل_لمبائی(ڈیٹا) &&
     (blen <= ڈیٹا->req.sendbuf.chunk_سائز)) {
    ڈیٹا->req.eos_read = TRUE;
    نتیجہ = xfer_send(ڈیٹا, buf, blen, blen, &nwritten);
    اگر(نتیجہ)
      واپس نتیجہ;
    buf += nwritten;
    blen -= nwritten;
    اگر(!blen) {
      نتیجہ = req_سیٹ_upload_مکمل(ڈیٹا);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
  }

  اگر(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    نتیجہ = req_send_بفر_جوڑو(ڈیٹا, buf, blen, blen);
    اگر(نتیجہ)
      واپس نتیجہ;

    واپس Curl_req_send_مزید(ڈیٹا);
  }
  واپس CURLE_OK;
}

bool Curl_req_sendbuf_خالی_خانے(ساخت Curl_easy *ڈیٹا)
{
  واپس !ڈیٹا->req.sendbuf_init || Curl_bufq_is_خالی_خانے(&ڈیٹا->req.sendbuf);
}

bool Curl_req_want_send(ساخت Curl_easy *ڈیٹا)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  واپس !ڈیٹا->req.مکمل &&
         !Curl_rlimit_is_blocked(&ڈیٹا->پیش_رفت.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(ڈیٹا) ||
          !Curl_req_sendbuf_خالی_خانے(ڈیٹا) ||
          Curl_xfer_needs_flush(ڈیٹا));
}

bool Curl_req_want_recv(ساخت Curl_easy *ڈیٹا)
{
  /* Not done and download not blocked and want RECV */
  واپس !ڈیٹا->req.مکمل &&
         !Curl_rlimit_is_blocked(&ڈیٹا->پیش_رفت.dl.rlimit) &&
         CURL_REQ_WANT_RECV(ڈیٹا);
}

bool Curl_req_مکمل_sending(ساخت Curl_easy *ڈیٹا)
{
  واپس ڈیٹا->req.upload_مکمل && !Curl_req_want_send(ڈیٹا);
}

CURLcode Curl_req_send_مزید(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ;

  /* Fill our send buffer if more from client can be read. */
  اگر(!ڈیٹا->req.upload_aborted &&
     !ڈیٹا->req.eos_read &&
     !Curl_xfer_send_is_paused(ڈیٹا) &&
     !Curl_bufq_is_بھرا(&ڈیٹا->req.sendbuf)) {
    size_t nread;
    نتیجہ = Curl_bufq_sipn(&ڈیٹا->req.sendbuf, 0,
                            جوڑو_from_client, ڈیٹا, &nread);
    اگر(نتیجہ && نتیجہ != CURLE_AGAIN)
      واپس نتیجہ;
  }

  نتیجہ = req_flush(ڈیٹا);
  اگر(نتیجہ == CURLE_AGAIN)
    نتیجہ = CURLE_OK;

  واپس نتیجہ;
}

CURLcode Curl_req_abort_sending(ساخت Curl_easy *ڈیٹا)
{
  اگر(!ڈیٹا->req.upload_مکمل) {
    Curl_bufq_reset(&ڈیٹا->req.sendbuf);
    ڈیٹا->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(ڈیٹا);
    واپس req_سیٹ_upload_مکمل(ڈیٹا);
  }
  واپس CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(ساخت Curl_easy *ڈیٹا)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode نتیجہ = CURLE_OK;
  اگر(CURL_REQ_WANT_SEND(ڈیٹا))
    نتیجہ = Curl_req_abort_sending(ڈیٹا);
  CURL_REQ_CLEAR_IO(ڈیٹا);
  واپس نتیجہ;
}
