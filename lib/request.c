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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "request.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "curlx/strparse.h"

శూన్య Curl_req_init(నిర్మాణం SingleRequest *req)
{
  memset(req, 0, పరిమాణం(*req));
}

CURLcode Curl_req_soft_reset(నిర్మాణం SingleRequest *req,
                             నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం;

  req->పూర్తి = FALSE;
  req->upload_పూర్తి = FALSE;
  req->upload_aborted = FALSE;
  req->download_పూర్తి = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_పంపిన = FALSE;
  req->ignorebody = FALSE;
  req->shutdown = FALSE;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->శీర్షిక = FALSE;
  req->headerline = 0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->httpversion_పంపిన = 0;
  req->httpversion = 0;
  req->sendbuf_hds_len = 0;

  curlx_safefree(req->hd_auth);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#అయితే_ముగింపు

  ఫలితం = Curl_client_ప్రారంభం(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, దత్తాంశం->సమితి.upload_బఫరు_కొలత, 1,
                    BUFQ_OPT_SOFT_హద్దు);
    req->sendbuf_init = TRUE;
  }
  లేకపోతే {
    Curl_bufq_reset(&req->sendbuf);
    ఒకవేళ(దత్తాంశం->సమితి.upload_బఫరు_కొలత != req->sendbuf.chunk_కొలత) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, దత్తాంశం->సమితి.upload_బఫరు_కొలత, 1,
                      BUFQ_OPT_SOFT_హద్దు);
    }
  }

  తిరిగి CURLE_OK;
}

CURLcode Curl_req_ప్రారంభం(నిర్మాణం SingleRequest *req,
                        నిర్మాణం Curl_easy *దత్తాంశం)
{
  req->ప్రారంభం = *Curl_pgrs_now(దత్తాంశం);
  తిరిగి Curl_req_soft_reset(req, దత్తాంశం);
}

స్థిర CURLcode req_flush(నిర్మాణం Curl_easy *దత్తాంశం);

CURLcode Curl_req_పూర్తి(నిర్మాణం SingleRequest *req,
                       నిర్మాణం Curl_easy *దత్తాంశం, bool aborted)
{
  (శూన్య)req;
  ఒకవేళ(!aborted)
    (శూన్య)req_flush(దత్తాంశం);
  Curl_client_reset(దత్తాంశం);
  తిరిగి CURLE_OK;
}

శూన్య Curl_req_hard_reset(నిర్మాణం SingleRequest *req, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#అయితే_ముగింపు
  Curl_client_reset(దత్తాంశం);
  ఒకవేళ(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(దత్తాంశం);
  /* Can no longer memset() this struct as we need to keep some state */
  req->కొలత = -1;
  req->maxdownload = -1;
  req->bytecount = 0;
  req->writebytecount = 0;
  req->ప్రారంభం = t0;
  req->headerbytecount = 0;
  req->allheadercount = 0;
  req->deductheadercount = 0;
  req->headerline = 0;
  req->స్థానభ్రంశం = 0;
  req->httpcode = 0;
  req->io_flags = 0;
  req->upgr101 = UPGR101_NONE;
  req->sendbuf_hds_len = 0;
  req->timeofdoc = 0;
  req->location = ఖాళీ;
  req->newurl = ఖాళీ;
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  req->setcookies = 0;
#అయితే_ముగింపు
  req->శీర్షిక = FALSE;
  req->content_range = FALSE;
  req->download_పూర్తి = FALSE;
  req->eos_written = FALSE;
  req->eos_read = FALSE;
  req->eos_పంపిన = FALSE;
  req->rewind_read = FALSE;
  req->upload_పూర్తి = FALSE;
  req->upload_aborted = FALSE;
  req->ignorebody = FALSE;
  req->http_bodyless = FALSE;
  req->chunk = FALSE;
  req->resp_trailer = FALSE;
  req->ignore_cl = FALSE;
  req->upload_chunky = FALSE;
  req->no_body = దత్తాంశం->సమితి.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_ఖండం(&దత్తాంశం->పురోగతి.dl.rlimit, FALSE, &t0);
  Curl_rlimit_ఖండం(&దత్తాంశం->పురోగతి.ul.rlimit, FALSE, &t0);
}

శూన్య Curl_req_free(నిర్మాణం SingleRequest *req, నిర్మాణం Curl_easy *దత్తాంశం)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#అయితే_ముగింపు
  ఒకవేళ(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(దత్తాంశం);
}

స్థిర CURLcode xfer_send(నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి అక్షరం *buf, size_t blen,
                          size_t hds_len, size_t *pnwritten)
{
  CURLcode ఫలితం = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#నిర్వచితమైతే DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    size_t body_len = blen - hds_len;
    ఒకవేళ(body_len) {
      స్థిరరాశి అక్షరం *p = getenv("CURL_SMALLREQSEND");
      ఒకవేళ(p) {
        curl_off_t body_small;
        ఒకవేళ(!curlx_str_సంఖ్య(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#అయితే_ముగింపు
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  ఒకవేళ(దత్తాంశం->సమితి.max_send_వేగం) {
    size_t body_బైట్లు = blen - hds_len;
    ఒకవేళ((curl_off_t)body_బైట్లు > దత్తాంశం->సమితి.max_send_వేగం)
      blen = hds_len + (size_t)దత్తాంశం->సమితి.max_send_వేగం;
  }

  ఒకవేళ(దత్తాంశం->req.eos_read &&
    (Curl_bufq_is_ఖాళీలు(&దత్తాంశం->req.sendbuf) ||
     Curl_bufq_len(&దత్తాంశం->req.sendbuf) == blen)) {
    DEBUGF(infof(దత్తాంశం, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  ఫలితం = Curl_xfer_send(దత్తాంశం, buf, blen, eos, pnwritten);
  ఒకవేళ(!ఫలితం) {
    ఒకవేళ(eos && (blen == *pnwritten))
      దత్తాంశం->req.eos_పంపిన = TRUE;
    ఒకవేళ(*pnwritten) {
      ఒకవేళ(hds_len)
        Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_బహిర్గతం, buf,
                   CURLMIN(hds_len, *pnwritten));
      ఒకవేళ(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(దత్తాంశం, CURLINFO_దత్తాంశం_బహిర్గతం, buf + hds_len, body_len);
        దత్తాంశం->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(దత్తాంశం, body_len);
      }
    }
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode req_send_బఫరు_flush(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి గుర్తులేని అక్షరం *buf;
  size_t blen;

  అయితే(Curl_bufq_peek(&దత్తాంశం->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(దత్తాంశం->req.sendbuf_hds_len, blen);
    ఫలితం = xfer_send(దత్తాంశం, (స్థిరరాశి అక్షరం *)buf, blen, hds_len, &nwritten);
    ఒకవేళ(ఫలితం)
      ఆపు;

    Curl_bufq_skip(&దత్తాంశం->req.sendbuf, nwritten);
    ఒకవేళ(hds_len) {
      దత్తాంశం->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    ఒకవేళ(nwritten < blen)
      ఆపు;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode req_సమితి_upload_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం)
{
  DEBUGASSERT(!దత్తాంశం->req.upload_పూర్తి);
  దత్తాంశం->req.upload_పూర్తి = TRUE;
  CURL_REQ_CLEAR_SEND(దత్తాంశం);

  ఒకవేళ(దత్తాంశం->mstate >= MSTATE_DID)
    Curl_pgrs_సమయం(దత్తాంశం, TIMER_POSTRANSFER);
  Curl_creader_పూర్తి(దత్తాంశం, దత్తాంశం->req.upload_aborted);

  ఒకవేళ(దత్తాంశం->req.upload_aborted) {
    Curl_bufq_reset(&దత్తాంశం->req.sendbuf);
    ఒకవేళ(దత్తాంశం->req.writebytecount)
      infof(దత్తాంశం, "abort upload after having sent %" FMT_OFF_T " bytes",
            దత్తాంశం->req.writebytecount);
    లేకపోతే
      infof(దత్తాంశం, "abort upload");
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->req.writebytecount)
    infof(దత్తాంశం, "upload completely sent off: %" FMT_OFF_T " bytes",
          దత్తాంశం->req.writebytecount);
  లేకపోతే ఒకవేళ(!దత్తాంశం->req.download_పూర్తి) {
    DEBUGASSERT(Curl_bufq_is_ఖాళీలు(&దత్తాంశం->req.sendbuf));
    infof(దత్తాంశం, Curl_creader_మొత్తం_నిడివి(దత్తాంశం) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  తిరిగి Curl_xfer_send_close(దత్తాంశం);
}

స్థిర CURLcode req_flush(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం;

  ఒకవేళ(!దత్తాంశం || !దత్తాంశం->conn)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&దత్తాంశం->req.sendbuf)) {
    ఫలితం = req_send_బఫరు_flush(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&దత్తాంశం->req.sendbuf)) {
      DEBUGF(infof(దత్తాంశం, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&దత్తాంశం->req.sendbuf)));
      తిరిగి CURLE_AGAIN;
    }
  }
  లేకపోతే ఒకవేళ(Curl_xfer_needs_flush(దత్తాంశం)) {
    DEBUGF(infof(దత్తాంశం, "Curl_req_flush(), xfer send_pending"));
    తిరిగి Curl_xfer_flush(దత్తాంశం);
  }

  ఒకవేళ(దత్తాంశం->req.eos_read && !దత్తాంశం->req.eos_పంపిన) {
    అక్షరం tmp = 0;
    size_t nwritten;
    ఫలితం = xfer_send(దత్తాంశం, &tmp, 0, 0, &nwritten);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    DEBUGASSERT(దత్తాంశం->req.eos_పంపిన);
  }

  ఒకవేళ(!దత్తాంశం->req.upload_పూర్తి && దత్తాంశం->req.eos_read && దత్తాంశం->req.eos_పంపిన) {
    DEBUGASSERT(Curl_bufq_is_ఖాళీలు(&దత్తాంశం->req.sendbuf));
    ఒకవేళ(దత్తాంశం->req.shutdown) {
      bool పూర్తి;
      ఫలితం = Curl_xfer_send_shutdown(దత్తాంశం, &పూర్తి);
      ఒకవేళ(ఫలితం && దత్తాంశం->req.shutdown_err_ignore) {
        infof(దత్తాంశం, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (పూర్ణాంకం)ఫలితం);
        ఫలితం = CURLE_OK;
        పూర్తి = TRUE;
      }

      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      ఒకవేళ(!పూర్తి)
        తిరిగి CURLE_AGAIN;
    }
    తిరిగి req_సమితి_upload_పూర్తి(దత్తాంశం);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode కూడు_from_client(శూన్య *reader_ctx,
                                గుర్తులేని అక్షరం *buf, size_t buflen,
                                size_t *pnread)
{
  నిర్మాణం Curl_easy *దత్తాంశం = reader_ctx;
  CURLcode ఫలితం;
  bool eos;

  ఫలితం = Curl_client_read(దత్తాంశం, (అక్షరం *)buf, buflen, pnread, &eos);
  ఒకవేళ(!ఫలితం && eos)
    దత్తాంశం->req.eos_read = TRUE;
  తిరిగి ఫలితం;
}

స్థిర CURLcode req_send_బఫరు_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                                    స్థిరరాశి అక్షరం *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t n;
  ఫలితం = Curl_bufq_cwrite(&దత్తాంశం->req.sendbuf, buf, blen, &n);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  దత్తాంశం->req.sendbuf_hds_len += hds_len;
  తిరిగి CURLE_OK;
}

CURLcode Curl_req_send(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం dynbuf *req,
                       గుర్తులేని అక్షరం httpversion)
{
  CURLcode ఫలితం;
  స్థిరరాశి అక్షరం *buf;
  size_t blen, nwritten;

  ఒకవేళ(!దత్తాంశం || !దత్తాంశం->conn)
    తిరిగి CURLE_FAILED_INIT;

  దత్తాంశం->req.httpversion_పంపిన = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  ఒకవేళ(Curl_bufq_is_ఖాళీలు(&దత్తాంశం->req.sendbuf) &&
     !Curl_creader_మొత్తం_నిడివి(దత్తాంశం) &&
     (blen <= దత్తాంశం->req.sendbuf.chunk_కొలత)) {
    దత్తాంశం->req.eos_read = TRUE;
    ఫలితం = xfer_send(దత్తాంశం, buf, blen, blen, &nwritten);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    buf += nwritten;
    blen -= nwritten;
    ఒకవేళ(!blen) {
      ఫలితం = req_సమితి_upload_పూర్తి(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
  }

  ఒకవేళ(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    ఫలితం = req_send_బఫరు_కూడు(దత్తాంశం, buf, blen, blen);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    తిరిగి Curl_req_send_మరిన్ని(దత్తాంశం);
  }
  తిరిగి CURLE_OK;
}

bool Curl_req_sendbuf_ఖాళీలు(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి !దత్తాంశం->req.sendbuf_init || Curl_bufq_is_ఖాళీలు(&దత్తాంశం->req.sendbuf);
}

bool Curl_req_want_send(నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  తిరిగి !దత్తాంశం->req.పూర్తి &&
         !Curl_rlimit_is_blocked(&దత్తాంశం->పురోగతి.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(దత్తాంశం) ||
          !Curl_req_sendbuf_ఖాళీలు(దత్తాంశం) ||
          Curl_xfer_needs_flush(దత్తాంశం));
}

bool Curl_req_want_recv(నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* Not done and download not blocked and want RECV */
  తిరిగి !దత్తాంశం->req.పూర్తి &&
         !Curl_rlimit_is_blocked(&దత్తాంశం->పురోగతి.dl.rlimit) &&
         CURL_REQ_WANT_RECV(దత్తాంశం);
}

bool Curl_req_పూర్తి_sending(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి దత్తాంశం->req.upload_పూర్తి && !Curl_req_want_send(దత్తాంశం);
}

CURLcode Curl_req_send_మరిన్ని(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం;

  /* Fill our send buffer if more from client can be read. */
  ఒకవేళ(!దత్తాంశం->req.upload_aborted &&
     !దత్తాంశం->req.eos_read &&
     !Curl_xfer_send_is_paused(దత్తాంశం) &&
     !Curl_bufq_is_నిండిన(&దత్తాంశం->req.sendbuf)) {
    size_t nread;
    ఫలితం = Curl_bufq_sipn(&దత్తాంశం->req.sendbuf, 0,
                            కూడు_from_client, దత్తాంశం, &nread);
    ఒకవేళ(ఫలితం && ఫలితం != CURLE_AGAIN)
      తిరిగి ఫలితం;
  }

  ఫలితం = req_flush(దత్తాంశం);
  ఒకవేళ(ఫలితం == CURLE_AGAIN)
    ఫలితం = CURLE_OK;

  తిరిగి ఫలితం;
}

CURLcode Curl_req_abort_sending(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(!దత్తాంశం->req.upload_పూర్తి) {
    Curl_bufq_reset(&దత్తాంశం->req.sendbuf);
    దత్తాంశం->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(దత్తాంశం);
    తిరిగి req_సమితి_upload_పూర్తి(దత్తాంశం);
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode ఫలితం = CURLE_OK;
  ఒకవేళ(CURL_REQ_WANT_SEND(దత్తాంశం))
    ఫలితం = Curl_req_abort_sending(దత్తాంశం);
  CURL_REQ_CLEAR_IO(దత్తాంశం);
  తిరిగి ఫలితం;
}
