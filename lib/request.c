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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "request.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

ਖਾਲੀ Curl_req_init(ਬਣਤਰ SingleRequest *req)
{
  memset(req, 0, ਆਕਾਰ(*req));
}

CURLcode Curl_req_soft_reset(ਬਣਤਰ SingleRequest *req,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;

  req->ਪੂਰਾ = FALSE;
  req->upload_ਪੂਰਾ = FALSE;
  req->upload_aborted = FALSE;
  req->download_ਪੂਰਾ = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_ਭੇਜਿਆ = FALSE;
  req->ignorebody = FALSE;
  req->shutdown = FALSE;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->ਸਿਰਲੇਖ = FALSE;
  req->headerline = 0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->httpversion_ਭੇਜਿਆ = 0;
  req->httpversion = 0;
  req->sendbuf_hds_len = 0;

  curlx_safefree(req->hd_auth);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#ਜੇ_ਅੰਤ

  ਨਤੀਜਾ = Curl_client_ਸ਼ੁਰੂ(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, ਅੰਕੜੇ->ਸੈੱਟ.upload_ਬਫਰ_ਮਾਪ, 1,
                    BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
    req->sendbuf_init = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    Curl_bufq_reset(&req->sendbuf);
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.upload_ਬਫਰ_ਮਾਪ != req->sendbuf.chunk_ਮਾਪ) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, ਅੰਕੜੇ->ਸੈੱਟ.upload_ਬਫਰ_ਮਾਪ, 1,
                      BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
    }
  }

  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_req_ਸ਼ੁਰੂ(ਬਣਤਰ SingleRequest *req,
                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  req->ਸ਼ੁਰੂ = *Curl_pgrs_now(ਅੰਕੜੇ);
  ਵਾਪਸ Curl_req_soft_reset(req, ਅੰਕੜੇ);
}

ਸਥਿਰ CURLcode req_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

CURLcode Curl_req_ਪੂਰਾ(ਬਣਤਰ SingleRequest *req,
                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool aborted)
{
  (ਖਾਲੀ)req;
  ਜੇ(!aborted)
    (ਖਾਲੀ)req_flush(ਅੰਕੜੇ);
  Curl_client_reset(ਅੰਕੜੇ);
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_req_hard_reset(ਬਣਤਰ SingleRequest *req, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#ਜੇ_ਅੰਤ
  Curl_client_reset(ਅੰਕੜੇ);
  ਜੇ(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(ਅੰਕੜੇ);
  /* Can no longer memset() this struct as we need to keep some state */
  req->ਮਾਪ = -1;
  req->maxdownload = -1;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->ਸ਼ੁਰੂ = t0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->headerline = 0;
  req->ਵਿੱਥ = 0;
  req->httpcode = 0;
  req->io_flags = 0;
  req->upgr101 = UPGR101_NONE;
  req->sendbuf_hds_len = 0;
  req->timeofdoc = 0;
  req->location = ਨਿਰਾ;
  req->newurl = ਨਿਰਾ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  req->setcookies = 0;
#ਜੇ_ਅੰਤ
  req->ਸਿਰਲੇਖ = FALSE;
  req->content_range = FALSE;
  req->download_ਪੂਰਾ = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_ਭੇਜਿਆ = FALSE;
  req->rewind_read = FALSE;
  req->upload_ਪੂਰਾ = FALSE;
  req->upload_aborted = FALSE;
  req->ignorebody = FALSE;
  req->http_bodyless = FALSE;
  req->chunk = FALSE;
  req->resp_trailer = FALSE;
  req->ignore_cl = FALSE;
  req->upload_chunky = FALSE;
  req->no_body = ਅੰਕੜੇ->ਸੈੱਟ.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_ਖੰਡ(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit, FALSE, &t0);
  Curl_rlimit_ਖੰਡ(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit, FALSE, &t0);
}

ਖਾਲੀ Curl_req_free(ਬਣਤਰ SingleRequest *req, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#ਜੇ_ਅੰਤ
  ਜੇ(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(ਅੰਕੜੇ);
}

ਸਥਿਰ CURLcode xfer_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                          size_t hds_len, size_t *pnwritten)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    size_t body_len = blen - hds_len;
    ਜੇ(body_len) {
      ਅਚਲ ਅੱਖਰ *p = getenv("CURL_SMALLREQSEND");
      ਜੇ(p) {
        curl_off_t body_small;
        ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#ਜੇ_ਅੰਤ
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.max_send_ਰਫਤਾਰ) {
    size_t body_ਬਾਈਟਾਂ = blen - hds_len;
    ਜੇ((curl_off_t)body_ਬਾਈਟਾਂ > ਅੰਕੜੇ->ਸੈੱਟ.max_send_ਰਫਤਾਰ)
      blen = hds_len + (size_t)ਅੰਕੜੇ->ਸੈੱਟ.max_send_ਰਫਤਾਰ;
  }

  ਜੇ(ਅੰਕੜੇ->req.eos_read &&
    (Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਅੰਕੜੇ->req.sendbuf) ||
     Curl_bufq_len(&ਅੰਕੜੇ->req.sendbuf) == blen)) {
    DEBUGF(infof(ਅੰਕੜੇ, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, buf, blen, eos, pnwritten);
  ਜੇ(!ਨਤੀਜਾ) {
    ਜੇ(eos && (blen == *pnwritten))
      ਅੰਕੜੇ->req.eos_ਭੇਜਿਆ = TRUE;
    ਜੇ(*pnwritten) {
      ਜੇ(hds_len)
        Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਬਾਹਰਲਾ, buf,
                   CURLMIN(hds_len, *pnwritten));
      ਜੇ(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(ਅੰਕੜੇ, CURLINFO_ਅੰਕੜੇ_ਬਾਹਰਲਾ, buf + hds_len, body_len);
        ਅੰਕੜੇ->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(ਅੰਕੜੇ, body_len);
      }
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode req_send_ਬਫਰ_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf;
  size_t blen;

  ਜਦੋਂ(Curl_bufq_peek(&ਅੰਕੜੇ->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(ਅੰਕੜੇ->req.sendbuf_hds_len, blen);
    ਨਤੀਜਾ = xfer_send(ਅੰਕੜੇ, (ਅਚਲ ਅੱਖਰ *)buf, blen, hds_len, &nwritten);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    Curl_bufq_skip(&ਅੰਕੜੇ->req.sendbuf, nwritten);
    ਜੇ(hds_len) {
      ਅੰਕੜੇ->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    ਜੇ(nwritten < blen)
      ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode req_ਸੈੱਟ_upload_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  DEBUGASSERT(!ਅੰਕੜੇ->req.upload_ਪੂਰਾ);
  ਅੰਕੜੇ->req.upload_ਪੂਰਾ = TRUE;
  CURL_REQ_CLEAR_SEND(ਅੰਕੜੇ);

  ਜੇ(ਅੰਕੜੇ->mstate >= MSTATE_DID)
    Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_POSTRANSFER);
  Curl_creader_ਪੂਰਾ(ਅੰਕੜੇ, ਅੰਕੜੇ->req.upload_aborted);

  ਜੇ(ਅੰਕੜੇ->req.upload_aborted) {
    Curl_bufq_reset(&ਅੰਕੜੇ->req.sendbuf);
    ਜੇ(ਅੰਕੜੇ->req.writebytecount)
      infof(ਅੰਕੜੇ, "abort upload after having sent %" FMT_OFF_T " bytes",
            ਅੰਕੜੇ->req.writebytecount);
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "abort upload");
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->req.writebytecount)
    infof(ਅੰਕੜੇ, "upload completely sent off: %" FMT_OFF_T " bytes",
          ਅੰਕੜੇ->req.writebytecount);
  ਨਹੀਂਤਾਂ ਜੇ(!ਅੰਕੜੇ->req.download_ਪੂਰਾ) {
    DEBUGASSERT(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਅੰਕੜੇ->req.sendbuf));
    infof(ਅੰਕੜੇ, Curl_creader_ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  ਵਾਪਸ Curl_xfer_send_close(ਅੰਕੜੇ);
}

ਸਥਿਰ CURLcode req_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!ਅੰਕੜੇ || !ਅੰਕੜੇ->conn)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਅੰਕੜੇ->req.sendbuf)) {
    ਨਤੀਜਾ = req_send_ਬਫਰ_flush(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਅੰਕੜੇ->req.sendbuf)) {
      DEBUGF(infof(ਅੰਕੜੇ, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&ਅੰਕੜੇ->req.sendbuf)));
      ਵਾਪਸ CURLE_AGAIN;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(Curl_xfer_needs_flush(ਅੰਕੜੇ)) {
    DEBUGF(infof(ਅੰਕੜੇ, "Curl_req_flush(), xfer send_pending"));
    ਵਾਪਸ Curl_xfer_flush(ਅੰਕੜੇ);
  }

  ਜੇ(ਅੰਕੜੇ->req.eos_read && !ਅੰਕੜੇ->req.eos_ਭੇਜਿਆ) {
    ਅੱਖਰ tmp = 0;
    size_t nwritten;
    ਨਤੀਜਾ = xfer_send(ਅੰਕੜੇ, &tmp, 0, 0, &nwritten);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    DEBUGASSERT(ਅੰਕੜੇ->req.eos_ਭੇਜਿਆ);
  }

  ਜੇ(!ਅੰਕੜੇ->req.upload_ਪੂਰਾ && ਅੰਕੜੇ->req.eos_read && ਅੰਕੜੇ->req.eos_ਭੇਜਿਆ) {
    DEBUGASSERT(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਅੰਕੜੇ->req.sendbuf));
    ਜੇ(ਅੰਕੜੇ->req.shutdown) {
      bool ਪੂਰਾ;
      ਨਤੀਜਾ = Curl_xfer_send_shutdown(ਅੰਕੜੇ, &ਪੂਰਾ);
      ਜੇ(ਨਤੀਜਾ && ਅੰਕੜੇ->req.shutdown_err_ignore) {
        infof(ਅੰਕੜੇ, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (ਪੂਰਨ)ਨਤੀਜਾ);
        ਨਤੀਜਾ = CURLE_OK;
        ਪੂਰਾ = TRUE;
      }

      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      ਜੇ(!ਪੂਰਾ)
        ਵਾਪਸ CURLE_AGAIN;
    }
    ਵਾਪਸ req_ਸੈੱਟ_upload_ਪੂਰਾ(ਅੰਕੜੇ);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ਜੋੜੋ_from_client(ਖਾਲੀ *reader_ctx,
                                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t buflen,
                                size_t *pnread)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = reader_ctx;
  CURLcode ਨਤੀਜਾ;
  bool eos;

  ਨਤੀਜਾ = Curl_client_read(ਅੰਕੜੇ, (ਅੱਖਰ *)buf, buflen, pnread, &eos);
  ਜੇ(!ਨਤੀਜਾ && eos)
    ਅੰਕੜੇ->req.eos_read = TRUE;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode req_send_ਬਫਰ_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t n;
  ਨਤੀਜਾ = Curl_bufq_cwrite(&ਅੰਕੜੇ->req.sendbuf, buf, blen, &n);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  ਅੰਕੜੇ->req.sendbuf_hds_len += hds_len;
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_req_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ dynbuf *req,
                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ httpversion)
{
  CURLcode ਨਤੀਜਾ;
  ਅਚਲ ਅੱਖਰ *buf;
  size_t blen, nwritten;

  ਜੇ(!ਅੰਕੜੇ || !ਅੰਕੜੇ->conn)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਅੰਕੜੇ->req.httpversion_ਭੇਜਿਆ = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਅੰਕੜੇ->req.sendbuf) &&
     !Curl_creader_ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ) &&
     (blen <= ਅੰਕੜੇ->req.sendbuf.chunk_ਮਾਪ)) {
    ਅੰਕੜੇ->req.eos_read = TRUE;
    ਨਤੀਜਾ = xfer_send(ਅੰਕੜੇ, buf, blen, blen, &nwritten);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    buf += nwritten;
    blen -= nwritten;
    ਜੇ(!blen) {
      ਨਤੀਜਾ = req_ਸੈੱਟ_upload_ਪੂਰਾ(ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  ਜੇ(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    ਨਤੀਜਾ = req_send_ਬਫਰ_ਜੋੜੋ(ਅੰਕੜੇ, buf, blen, blen);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਵਾਪਸ Curl_req_send_ਹੋਰ(ਅੰਕੜੇ);
  }
  ਵਾਪਸ CURLE_OK;
}

bool Curl_req_sendbuf_ਖਾਲੀ_ਥਾਵਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ !ਅੰਕੜੇ->req.sendbuf_init || Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਅੰਕੜੇ->req.sendbuf);
}

bool Curl_req_want_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  ਵਾਪਸ !ਅੰਕੜੇ->req.ਪੂਰਾ &&
         !Curl_rlimit_is_blocked(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(ਅੰਕੜੇ) ||
          !Curl_req_sendbuf_ਖਾਲੀ_ਥਾਵਾਂ(ਅੰਕੜੇ) ||
          Curl_xfer_needs_flush(ਅੰਕੜੇ));
}

bool Curl_req_want_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* Not done and download not blocked and want RECV */
  ਵਾਪਸ !ਅੰਕੜੇ->req.ਪੂਰਾ &&
         !Curl_rlimit_is_blocked(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit) &&
         CURL_REQ_WANT_RECV(ਅੰਕੜੇ);
}

bool Curl_req_ਪੂਰਾ_sending(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ ਅੰਕੜੇ->req.upload_ਪੂਰਾ && !Curl_req_want_send(ਅੰਕੜੇ);
}

CURLcode Curl_req_send_ਹੋਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;

  /* Fill our send buffer if more from client can be read. */
  ਜੇ(!ਅੰਕੜੇ->req.upload_aborted &&
     !ਅੰਕੜੇ->req.eos_read &&
     !Curl_xfer_send_is_paused(ਅੰਕੜੇ) &&
     !Curl_bufq_is_ਭਰਿਆ(&ਅੰਕੜੇ->req.sendbuf)) {
    size_t nread;
    ਨਤੀਜਾ = Curl_bufq_sipn(&ਅੰਕੜੇ->req.sendbuf, 0,
                            ਜੋੜੋ_from_client, ਅੰਕੜੇ, &nread);
    ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ != CURLE_AGAIN)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਨਤੀਜਾ = req_flush(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
    ਨਤੀਜਾ = CURLE_OK;

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_req_abort_sending(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(!ਅੰਕੜੇ->req.upload_ਪੂਰਾ) {
    Curl_bufq_reset(&ਅੰਕੜੇ->req.sendbuf);
    ਅੰਕੜੇ->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(ਅੰਕੜੇ);
    ਵਾਪਸ req_ਸੈੱਟ_upload_ਪੂਰਾ(ਅੰਕੜੇ);
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(CURL_REQ_WANT_SEND(ਅੰਕੜੇ))
    ਨਤੀਜਾ = Curl_req_abort_sending(ਅੰਕੜੇ);
  CURL_REQ_CLEAR_IO(ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}
