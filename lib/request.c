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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "request.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

ಶೂನ್ಯ Curl_req_init(ರಚನೆ SingleRequest *req)
{
  memset(req, 0, ಗಾತ್ರ(*req));
}

CURLcode Curl_req_soft_reset(ರಚನೆ SingleRequest *req,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;

  req->ಮುಗಿದ = FALSE;
  req->upload_ಮುಗಿದ = FALSE;
  req->upload_aborted = FALSE;
  req->download_ಮುಗಿದ = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_ಕಳುಹಿಸಿದ = FALSE;
  req->ignorebody = FALSE;
  req->shutdown = FALSE;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->ಶೀರ್ಷಿಕೆ = FALSE;
  req->headerline = 0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->httpversion_ಕಳುಹಿಸಿದ = 0;
  req->httpversion = 0;
  req->sendbuf_hds_len = 0;

  curlx_safefree(req->hd_auth);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#ಆದರೆ_ಅಂತ್ಯ

  ಫಲಿತಾಂಶ = Curl_client_ಆರಂಭ(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, ದತ್ತಾಂಶ->ಗಣ.upload_ಬಫರ್_ಗಾತ್ರ, 1,
                    BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
    req->sendbuf_init = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    Curl_bufq_reset(&req->sendbuf);
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.upload_ಬಫರ್_ಗಾತ್ರ != req->sendbuf.chunk_ಗಾತ್ರ) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, ದತ್ತಾಂಶ->ಗಣ.upload_ಬಫರ್_ಗಾತ್ರ, 1,
                      BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_req_ಆರಂಭ(ರಚನೆ SingleRequest *req,
                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  req->ಆರಂಭ = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ Curl_req_soft_reset(req, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ CURLcode req_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

CURLcode Curl_req_ಮುಗಿದ(ರಚನೆ SingleRequest *req,
                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool aborted)
{
  (ಶೂನ್ಯ)req;
  ಆದರೆ(!aborted)
    (ಶೂನ್ಯ)req_flush(ದತ್ತಾಂಶ);
  Curl_client_reset(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_req_hard_reset(ರಚನೆ SingleRequest *req, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#ಆದರೆ_ಅಂತ್ಯ
  Curl_client_reset(ದತ್ತಾಂಶ);
  ಆದರೆ(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(ದತ್ತಾಂಶ);
  /* Can no longer memset() this struct as we need to keep some state */
  req->ಗಾತ್ರ_2 = -1;
  req->maxdownload = -1;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->ಆರಂಭ = t0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->headerline = 0;
  req->ಸ್ಥಾನಾಂತರ = 0;
  req->httpcode = 0;
  req->io_flags = 0;
  req->upgr101 = UPGR101_NONE;
  req->sendbuf_hds_len = 0;
  req->timeofdoc = 0;
  req->location = ಖಾಲಿ;
  req->newurl = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  req->setcookies = 0;
#ಆದರೆ_ಅಂತ್ಯ
  req->ಶೀರ್ಷಿಕೆ = FALSE;
  req->content_range = FALSE;
  req->download_ಮುಗಿದ = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_ಕಳುಹಿಸಿದ = FALSE;
  req->rewind_read = FALSE;
  req->upload_ಮುಗಿದ = FALSE;
  req->upload_aborted = FALSE;
  req->ignorebody = FALSE;
  req->http_bodyless = FALSE;
  req->chunk = FALSE;
  req->resp_trailer = FALSE;
  req->ignore_cl = FALSE;
  req->upload_chunky = FALSE;
  req->no_body = ದತ್ತಾಂಶ->ಗಣ.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_ಖಂಡ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit, FALSE, &t0);
  Curl_rlimit_ಖಂಡ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit, FALSE, &t0);
}

ಶೂನ್ಯ Curl_req_free(ರಚನೆ SingleRequest *req, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ CURLcode xfer_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                          size_t hds_len, size_t *pnwritten)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    size_t body_len = blen - hds_len;
    ಆದರೆ(body_len) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = getenv("CURL_SMALLREQSEND");
      ಆದರೆ(p) {
        curl_off_t body_small;
        ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.max_send_ವೇಗ) {
    size_t body_ಬೈಟುಗಳು = blen - hds_len;
    ಆದರೆ((curl_off_t)body_ಬೈಟುಗಳು > ದತ್ತಾಂಶ->ಗಣ.max_send_ವೇಗ)
      blen = hds_len + (size_t)ದತ್ತಾಂಶ->ಗಣ.max_send_ವೇಗ;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->req.eos_read &&
    (Curl_bufq_is_ಖಾಲಿಗಳು(&ದತ್ತಾಂಶ->req.sendbuf) ||
     Curl_bufq_len(&ದತ್ತಾಂಶ->req.sendbuf) == blen)) {
    DEBUGF(infof(ದತ್ತಾಂಶ, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, buf, blen, eos, pnwritten);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಆದರೆ(eos && (blen == *pnwritten))
      ದತ್ತಾಂಶ->req.eos_ಕಳುಹಿಸಿದ = TRUE;
    ಆದರೆ(*pnwritten) {
      ಆದರೆ(hds_len)
        Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಹೊರಹೋಗು, buf,
                   CURLMIN(hds_len, *pnwritten));
      ಆದರೆ(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, buf + hds_len, body_len);
        ದತ್ತಾಂಶ->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(ದತ್ತಾಂಶ, body_len);
      }
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode req_send_ಬಫರ್_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf;
  size_t blen;

  ಆಗ(Curl_bufq_peek(&ದತ್ತಾಂಶ->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(ದತ್ತಾಂಶ->req.sendbuf_hds_len, blen);
    ಫಲಿತಾಂಶ = xfer_send(ದತ್ತಾಂಶ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf, blen, hds_len, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    Curl_bufq_skip(&ದತ್ತಾಂಶ->req.sendbuf, nwritten);
    ಆದರೆ(hds_len) {
      ದತ್ತಾಂಶ->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    ಆದರೆ(nwritten < blen)
      ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode req_ಗಣ_upload_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  DEBUGASSERT(!ದತ್ತಾಂಶ->req.upload_ಮುಗಿದ);
  ದತ್ತಾಂಶ->req.upload_ಮುಗಿದ = TRUE;
  CURL_REQ_CLEAR_SEND(ದತ್ತಾಂಶ);

  ಆದರೆ(ದತ್ತಾಂಶ->mstate >= MSTATE_DID)
    Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_POSTRANSFER);
  Curl_creader_ಮುಗಿದ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->req.upload_aborted);

  ಆದರೆ(ದತ್ತಾಂಶ->req.upload_aborted) {
    Curl_bufq_reset(&ದತ್ತಾಂಶ->req.sendbuf);
    ಆದರೆ(ದತ್ತಾಂಶ->req.writebytecount)
      infof(ದತ್ತಾಂಶ, "abort upload after having sent %" FMT_OFF_T " bytes",
            ದತ್ತಾಂಶ->req.writebytecount);
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "abort upload");
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->req.writebytecount)
    infof(ದತ್ತಾಂಶ, "upload completely sent off: %" FMT_OFF_T " bytes",
          ದತ್ತಾಂಶ->req.writebytecount);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ದತ್ತಾಂಶ->req.download_ಮುಗಿದ) {
    DEBUGASSERT(Curl_bufq_is_ಖಾಲಿಗಳು(&ದತ್ತಾಂಶ->req.sendbuf));
    infof(ದತ್ತಾಂಶ, Curl_creader_ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  ಹಿಂತಿರುಗಿ Curl_xfer_send_close(ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ CURLcode req_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!ದತ್ತಾಂಶ || !ದತ್ತಾಂಶ->conn)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ದತ್ತಾಂಶ->req.sendbuf)) {
    ಫಲಿತಾಂಶ = req_send_ಬಫರ್_flush(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ದತ್ತಾಂಶ->req.sendbuf)) {
      DEBUGF(infof(ದತ್ತಾಂಶ, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&ದತ್ತಾಂಶ->req.sendbuf)));
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Curl_xfer_needs_flush(ದತ್ತಾಂಶ)) {
    DEBUGF(infof(ದತ್ತಾಂಶ, "Curl_req_flush(), xfer send_pending"));
    ಹಿಂತಿರುಗಿ Curl_xfer_flush(ದತ್ತಾಂಶ);
  }

  ಆದರೆ(ದತ್ತಾಂಶ->req.eos_read && !ದತ್ತಾಂಶ->req.eos_ಕಳುಹಿಸಿದ) {
    ಅಕ್ಷರ tmp = 0;
    size_t nwritten;
    ಫಲಿತಾಂಶ = xfer_send(ದತ್ತಾಂಶ, &tmp, 0, 0, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    DEBUGASSERT(ದತ್ತಾಂಶ->req.eos_ಕಳುಹಿಸಿದ);
  }

  ಆದರೆ(!ದತ್ತಾಂಶ->req.upload_ಮುಗಿದ && ದತ್ತಾಂಶ->req.eos_read && ದತ್ತಾಂಶ->req.eos_ಕಳುಹಿಸಿದ) {
    DEBUGASSERT(Curl_bufq_is_ಖಾಲಿಗಳು(&ದತ್ತಾಂಶ->req.sendbuf));
    ಆದರೆ(ದತ್ತಾಂಶ->req.shutdown) {
      bool ಮುಗಿದ;
      ಫಲಿತಾಂಶ = Curl_xfer_send_shutdown(ದತ್ತಾಂಶ, &ಮುಗಿದ);
      ಆದರೆ(ಫಲಿತಾಂಶ && ದತ್ತಾಂಶ->req.shutdown_err_ignore) {
        infof(ದತ್ತಾಂಶ, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಫಲಿತಾಂಶ = CURLE_OK;
        ಮುಗಿದ = TRUE;
      }

      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ಆದರೆ(!ಮುಗಿದ)
        ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
    ಹಿಂತಿರುಗಿ req_ಗಣ_upload_ಮುಗಿದ(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ಕೂಡಿಸು_from_client(ಶೂನ್ಯ *reader_ctx,
                                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t buflen,
                                size_t *pnread)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = reader_ctx;
  CURLcode ಫಲಿತಾಂಶ;
  bool eos;

  ಫಲಿತಾಂಶ = Curl_client_read(ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)buf, buflen, pnread, &eos);
  ಆದರೆ(!ಫಲಿತಾಂಶ && eos)
    ದತ್ತಾಂಶ->req.eos_read = TRUE;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode req_send_ಬಫರ್_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t n;
  ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ದತ್ತಾಂಶ->req.sendbuf, buf, blen, &n);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  ದತ್ತಾಂಶ->req.sendbuf_hds_len += hds_len;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_req_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ dynbuf *req,
                       ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ httpversion)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf;
  size_t blen, nwritten;

  ಆದರೆ(!ದತ್ತಾಂಶ || !ದತ್ತಾಂಶ->conn)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ದತ್ತಾಂಶ->req.httpversion_ಕಳುಹಿಸಿದ = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ದತ್ತಾಂಶ->req.sendbuf) &&
     !Curl_creader_ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ) &&
     (blen <= ದತ್ತಾಂಶ->req.sendbuf.chunk_ಗಾತ್ರ)) {
    ದತ್ತಾಂಶ->req.eos_read = TRUE;
    ಫಲಿತಾಂಶ = xfer_send(ದತ್ತಾಂಶ, buf, blen, blen, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    buf += nwritten;
    blen -= nwritten;
    ಆದರೆ(!blen) {
      ಫಲಿತಾಂಶ = req_ಗಣ_upload_ಮುಗಿದ(ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  ಆದರೆ(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    ಫಲಿತಾಂಶ = req_send_ಬಫರ್_ಕೂಡಿಸು(ದತ್ತಾಂಶ, buf, blen, blen);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಹಿಂತಿರುಗಿ Curl_req_send_ಇನ್ನಷ್ಟು(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

bool Curl_req_sendbuf_ಖಾಲಿಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ !ದತ್ತಾಂಶ->req.sendbuf_init || Curl_bufq_is_ಖಾಲಿಗಳು(&ದತ್ತಾಂಶ->req.sendbuf);
}

bool Curl_req_want_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  ಹಿಂತಿರುಗಿ !ದತ್ತಾಂಶ->req.ಮುಗಿದ &&
         !Curl_rlimit_is_blocked(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(ದತ್ತಾಂಶ) ||
          !Curl_req_sendbuf_ಖಾಲಿಗಳು(ದತ್ತಾಂಶ) ||
          Curl_xfer_needs_flush(ದತ್ತಾಂಶ));
}

bool Curl_req_want_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* Not done and download not blocked and want RECV */
  ಹಿಂತಿರುಗಿ !ದತ್ತಾಂಶ->req.ಮುಗಿದ &&
         !Curl_rlimit_is_blocked(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit) &&
         CURL_REQ_WANT_RECV(ದತ್ತಾಂಶ);
}

bool Curl_req_ಮುಗಿದ_sending(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->req.upload_ಮುಗಿದ && !Curl_req_want_send(ದತ್ತಾಂಶ);
}

CURLcode Curl_req_send_ಇನ್ನಷ್ಟು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;

  /* Fill our send buffer if more from client can be read. */
  ಆದರೆ(!ದತ್ತಾಂಶ->req.upload_aborted &&
     !ದತ್ತಾಂಶ->req.eos_read &&
     !Curl_xfer_send_is_paused(ದತ್ತಾಂಶ) &&
     !Curl_bufq_is_ತುಂಬಿದ(&ದತ್ತಾಂಶ->req.sendbuf)) {
    size_t nread;
    ಫಲಿತಾಂಶ = Curl_bufq_sipn(&ದತ್ತಾಂಶ->req.sendbuf, 0,
                            ಕೂಡಿಸು_from_client, ದತ್ತಾಂಶ, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ != CURLE_AGAIN)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಫಲಿತಾಂಶ = req_flush(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
    ಫಲಿತಾಂಶ = CURLE_OK;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_req_abort_sending(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(!ದತ್ತಾಂಶ->req.upload_ಮುಗಿದ) {
    Curl_bufq_reset(&ದತ್ತಾಂಶ->req.sendbuf);
    ದತ್ತಾಂಶ->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(ದತ್ತಾಂಶ);
    ಹಿಂತಿರುಗಿ req_ಗಣ_upload_ಮುಗಿದ(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(CURL_REQ_WANT_SEND(ದತ್ತಾಂಶ))
    ಫಲಿತಾಂಶ = Curl_req_abort_sending(ದತ್ತಾಂಶ);
  CURL_REQ_CLEAR_IO(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
