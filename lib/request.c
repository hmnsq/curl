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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "request.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "curlx/strparse.h"

शून्य Curl_req_init(संरचना SingleRequest *req)
{
  स्मृतिं_पूरय(req, 0, परिमाण(*req));
}

CURLcode Curl_req_soft_reset(संरचना SingleRequest *req,
                             संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम;

  req->समाप्तम् = FALSE;
  req->upload_समाप्तम् = FALSE;
  req->upload_aborted = FALSE;
  req->download_समाप्तम् = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_प्रेषितम् = FALSE;
  req->ignorebody = FALSE;
  req->shutdown = FALSE;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->शीर्षकम् = FALSE;
  req->headerline = 0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->httpversion_प्रेषितम् = 0;
  req->httpversion = 0;
  req->sendbuf_hds_len = 0;

  curlx_safefree(req->hd_auth);
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#यदि_अन्तः

  परिणाम = Curl_client_start(दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, दत्त->समुच्चयः.upload_बफर_आकार, 1,
                    BUFQ_OPT_SOFT_सीमा);
    req->sendbuf_init = TRUE;
  }
  अन्यथा {
    Curl_bufq_reset(&req->sendbuf);
    यदि(दत्त->समुच्चयः.upload_बफर_आकार != req->sendbuf.chunk_आकार) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, दत्त->समुच्चयः.upload_बफर_आकार, 1,
                      BUFQ_OPT_SOFT_सीमा);
    }
  }

  प्रत्यागम CURLE_OK;
}

CURLcode Curl_req_start(संरचना SingleRequest *req,
                        संरचना Curl_easy *दत्त)
{
  req->start = *Curl_pgrs_now(दत्त);
  प्रत्यागम Curl_req_soft_reset(req, दत्त);
}

स्थिर CURLcode req_flush(संरचना Curl_easy *दत्त);

CURLcode Curl_req_समाप्तम्(संरचना SingleRequest *req,
                       संरचना Curl_easy *दत्त, bool aborted)
{
  (शून्य)req;
  यदि(!aborted)
    (शून्य)req_flush(दत्त);
  Curl_client_reset(दत्त);
  प्रत्यागम CURLE_OK;
}

शून्य Curl_req_hard_reset(संरचना SingleRequest *req, संरचना Curl_easy *दत्त)
{
  संरचना curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#यदि_अन्तः
  Curl_client_reset(दत्त);
  यदि(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(दत्त);
  /* Can no longer memset() this struct as we need to keep some state */
  req->आकार = -1;
  req->maxdownload = -1;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->start = t0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->headerline = 0;
  req->विस्थापनम् = 0;
  req->httpcode = 0;
  req->io_flags = 0;
  req->upgr101 = UPGR101_NONE;
  req->sendbuf_hds_len = 0;
  req->timeofdoc = 0;
  req->location = रिक्त;
  req->newurl = रिक्त;
#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
  req->setcookies = 0;
#यदि_अन्तः
  req->शीर्षकम् = FALSE;
  req->content_range = FALSE;
  req->download_समाप्तम् = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_प्रेषितम् = FALSE;
  req->rewind_read = FALSE;
  req->upload_समाप्तम् = FALSE;
  req->upload_aborted = FALSE;
  req->ignorebody = FALSE;
  req->http_bodyless = FALSE;
  req->chunk = FALSE;
  req->resp_trailer = FALSE;
  req->ignore_cl = FALSE;
  req->upload_chunky = FALSE;
  req->no_body = दत्त->समुच्चयः.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_खण्डम्(&दत्त->प्रगतिः.dl.rlimit, FALSE, &t0);
  Curl_rlimit_खण्डम्(&दत्त->प्रगतिः.ul.rlimit, FALSE, &t0);
}

शून्य Curl_req_free(संरचना SingleRequest *req, संरचना Curl_easy *दत्त)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#यदि_अन्तः
  यदि(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(दत्त);
}

स्थिर CURLcode xfer_send(संरचना Curl_easy *दत्त,
                          ध्रुव वर्ण *buf, size_t blen,
                          size_t hds_len, size_t *pnwritten)
{
  CURLcode परिणाम = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#यदि_परिभाषितम् DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    size_t body_len = blen - hds_len;
    यदि(body_len) {
      ध्रुव वर्ण *p = getenv("CURL_SMALLREQSEND");
      यदि(p) {
        curl_off_t body_small;
        यदि(!curlx_str_सङ्ख्या(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#यदि_अन्तः
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  यदि(दत्त->समुच्चयः.max_send_जवः) {
    size_t body_अष्टकानि = blen - hds_len;
    यदि((curl_off_t)body_अष्टकानि > दत्त->समुच्चयः.max_send_जवः)
      blen = hds_len + (size_t)दत्त->समुच्चयः.max_send_जवः;
  }

  यदि(दत्त->req.eos_read &&
    (Curl_bufq_is_रिक्तानि(&दत्त->req.sendbuf) ||
     Curl_bufq_len(&दत्त->req.sendbuf) == blen)) {
    DEBUGF(infof(दत्त, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  परिणाम = Curl_xfer_send(दत्त, buf, blen, eos, pnwritten);
  यदि(!परिणाम) {
    यदि(eos && (blen == *pnwritten))
      दत्त->req.eos_प्रेषितम् = TRUE;
    यदि(*pnwritten) {
      यदि(hds_len)
        Curl_debug(दत्त, CURLINFO_शीर्षकम्_बहिर्गमनम्, buf,
                   CURLMIN(hds_len, *pnwritten));
      यदि(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(दत्त, CURLINFO_दत्त_बहिर्गमनम्, buf + hds_len, body_len);
        दत्त->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(दत्त, body_len);
      }
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode req_send_बफर_flush(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव अचिह्नित वर्ण *buf;
  size_t blen;

  यावत्(Curl_bufq_peek(&दत्त->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(दत्त->req.sendbuf_hds_len, blen);
    परिणाम = xfer_send(दत्त, (ध्रुव वर्ण *)buf, blen, hds_len, &nwritten);
    यदि(परिणाम)
      भङ्ग;

    Curl_bufq_skip(&दत्त->req.sendbuf, nwritten);
    यदि(hds_len) {
      दत्त->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    यदि(nwritten < blen)
      भङ्ग;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode req_समुच्चयः_upload_समाप्तम्(संरचना Curl_easy *दत्त)
{
  DEBUGASSERT(!दत्त->req.upload_समाप्तम्);
  दत्त->req.upload_समाप्तम् = TRUE;
  CURL_REQ_CLEAR_SEND(दत्त);

  यदि(दत्त->mstate >= MSTATE_DID)
    Curl_pgrsTime(दत्त, TIMER_POSTRANSFER);
  Curl_creader_समाप्तम्(दत्त, दत्त->req.upload_aborted);

  यदि(दत्त->req.upload_aborted) {
    Curl_bufq_reset(&दत्त->req.sendbuf);
    यदि(दत्त->req.writebytecount)
      infof(दत्त, "abort upload after having sent %" FMT_OFF_T " bytes",
            दत्त->req.writebytecount);
    अन्यथा
      infof(दत्त, "abort upload");
  }
  अन्यथा यदि(दत्त->req.writebytecount)
    infof(दत्त, "upload completely sent off: %" FMT_OFF_T " bytes",
          दत्त->req.writebytecount);
  अन्यथा यदि(!दत्त->req.download_समाप्तम्) {
    DEBUGASSERT(Curl_bufq_is_रिक्तानि(&दत्त->req.sendbuf));
    infof(दत्त, Curl_creader_योग_दीर्घता(दत्त) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  प्रत्यागम Curl_xfer_send_close(दत्त);
}

स्थिर CURLcode req_flush(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम;

  यदि(!दत्त || !दत्त->conn)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(!Curl_bufq_is_रिक्तानि(&दत्त->req.sendbuf)) {
    परिणाम = req_send_बफर_flush(दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    यदि(!Curl_bufq_is_रिक्तानि(&दत्त->req.sendbuf)) {
      DEBUGF(infof(दत्त, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&दत्त->req.sendbuf)));
      प्रत्यागम CURLE_AGAIN;
    }
  }
  अन्यथा यदि(Curl_xfer_needs_flush(दत्त)) {
    DEBUGF(infof(दत्त, "Curl_req_flush(), xfer send_pending"));
    प्रत्यागम Curl_xfer_flush(दत्त);
  }

  यदि(दत्त->req.eos_read && !दत्त->req.eos_प्रेषितम्) {
    वर्ण tmp = 0;
    size_t nwritten;
    परिणाम = xfer_send(दत्त, &tmp, 0, 0, &nwritten);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    DEBUGASSERT(दत्त->req.eos_प्रेषितम्);
  }

  यदि(!दत्त->req.upload_समाप्तम् && दत्त->req.eos_read && दत्त->req.eos_प्रेषितम्) {
    DEBUGASSERT(Curl_bufq_is_रिक्तानि(&दत्त->req.sendbuf));
    यदि(दत्त->req.shutdown) {
      bool समाप्तम्;
      परिणाम = Curl_xfer_send_shutdown(दत्त, &समाप्तम्);
      यदि(परिणाम && दत्त->req.shutdown_err_ignore) {
        infof(दत्त, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (पूर्णाङ्क)परिणाम);
        परिणाम = CURLE_OK;
        समाप्तम् = TRUE;
      }

      यदि(परिणाम)
        प्रत्यागम परिणाम;
      यदि(!समाप्तम्)
        प्रत्यागम CURLE_AGAIN;
    }
    प्रत्यागम req_समुच्चयः_upload_समाप्तम्(दत्त);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode सङ्कलय_from_client(शून्य *reader_ctx,
                                अचिह्नित वर्ण *buf, size_t buflen,
                                size_t *pnread)
{
  संरचना Curl_easy *दत्त = reader_ctx;
  CURLcode परिणाम;
  bool eos;

  परिणाम = Curl_client_read(दत्त, (वर्ण *)buf, buflen, pnread, &eos);
  यदि(!परिणाम && eos)
    दत्त->req.eos_read = TRUE;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode req_send_बफर_सङ्कलय(संरचना Curl_easy *दत्त,
                                    ध्रुव वर्ण *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode परिणाम = CURLE_OK;
  size_t n;
  परिणाम = Curl_bufq_cwrite(&दत्त->req.sendbuf, buf, blen, &n);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  दत्त->req.sendbuf_hds_len += hds_len;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_req_send(संरचना Curl_easy *दत्त, संरचना dynbuf *req,
                       अचिह्नित वर्ण httpversion)
{
  CURLcode परिणाम;
  ध्रुव वर्ण *buf;
  size_t blen, nwritten;

  यदि(!दत्त || !दत्त->conn)
    प्रत्यागम CURLE_FAILED_INIT;

  दत्त->req.httpversion_प्रेषितम् = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  यदि(Curl_bufq_is_रिक्तानि(&दत्त->req.sendbuf) &&
     !Curl_creader_योग_दीर्घता(दत्त) &&
     (blen <= दत्त->req.sendbuf.chunk_आकार)) {
    दत्त->req.eos_read = TRUE;
    परिणाम = xfer_send(दत्त, buf, blen, blen, &nwritten);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    buf += nwritten;
    blen -= nwritten;
    यदि(!blen) {
      परिणाम = req_समुच्चयः_upload_समाप्तम्(दत्त);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
  }

  यदि(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    परिणाम = req_send_बफर_सङ्कलय(दत्त, buf, blen, blen);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    प्रत्यागम Curl_req_send_अधिकम्(दत्त);
  }
  प्रत्यागम CURLE_OK;
}

bool Curl_req_sendbuf_रिक्तानि(संरचना Curl_easy *दत्त)
{
  प्रत्यागम !दत्त->req.sendbuf_init || Curl_bufq_is_रिक्तानि(&दत्त->req.sendbuf);
}

bool Curl_req_want_send(संरचना Curl_easy *दत्त)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  प्रत्यागम !दत्त->req.समाप्तम् &&
         !Curl_rlimit_is_blocked(&दत्त->प्रगतिः.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(दत्त) ||
          !Curl_req_sendbuf_रिक्तानि(दत्त) ||
          Curl_xfer_needs_flush(दत्त));
}

bool Curl_req_want_recv(संरचना Curl_easy *दत्त)
{
  /* Not done and download not blocked and want RECV */
  प्रत्यागम !दत्त->req.समाप्तम् &&
         !Curl_rlimit_is_blocked(&दत्त->प्रगतिः.dl.rlimit) &&
         CURL_REQ_WANT_RECV(दत्त);
}

bool Curl_req_समाप्तम्_sending(संरचना Curl_easy *दत्त)
{
  प्रत्यागम दत्त->req.upload_समाप्तम् && !Curl_req_want_send(दत्त);
}

CURLcode Curl_req_send_अधिकम्(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम;

  /* Fill our send buffer if more from client can be read. */
  यदि(!दत्त->req.upload_aborted &&
     !दत्त->req.eos_read &&
     !Curl_xfer_send_is_paused(दत्त) &&
     !Curl_bufq_is_पूर्णम्(&दत्त->req.sendbuf)) {
    size_t nread;
    परिणाम = Curl_bufq_sipn(&दत्त->req.sendbuf, 0,
                            सङ्कलय_from_client, दत्त, &nread);
    यदि(परिणाम && परिणाम != CURLE_AGAIN)
      प्रत्यागम परिणाम;
  }

  परिणाम = req_flush(दत्त);
  यदि(परिणाम == CURLE_AGAIN)
    परिणाम = CURLE_OK;

  प्रत्यागम परिणाम;
}

CURLcode Curl_req_abort_sending(संरचना Curl_easy *दत्त)
{
  यदि(!दत्त->req.upload_समाप्तम्) {
    Curl_bufq_reset(&दत्त->req.sendbuf);
    दत्त->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(दत्त);
    प्रत्यागम req_समुच्चयः_upload_समाप्तम्(दत्त);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(संरचना Curl_easy *दत्त)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode परिणाम = CURLE_OK;
  यदि(CURL_REQ_WANT_SEND(दत्त))
    परिणाम = Curl_req_abort_sending(दत्त);
  CURL_REQ_CLEAR_IO(दत्त);
  प्रत्यागम परिणाम;
}
