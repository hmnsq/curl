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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "curlx/dynbuf.h"
#समावेश "progress.h"
#समावेश "request.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "transfer.h"
#समावेश "url.h"
#समावेश "curlx/strparse.h"

व्योम Curl_req_init(काष्ठा SingleRequest *req)
{
  स_रखो(req, 0, माप(*req));
}

CURLcode Curl_req_soft_reset(काष्ठा SingleRequest *req,
                             काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम;

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
#अगर_अघोषित CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#पूर्ण_अगर

  परिणाम = Curl_client_start(तथ्य);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, तथ्य->set.upload_बफर_आकार, 1,
                    BUFQ_OPT_SOFT_LIMIT);
    req->sendbuf_init = TRUE;
  }
  अन्यथा {
    Curl_bufq_reset(&req->sendbuf);
    अगर(तथ्य->set.upload_बफर_आकार != req->sendbuf.chunk_आकार) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, तथ्य->set.upload_बफर_आकार, 1,
                      BUFQ_OPT_SOFT_LIMIT);
    }
  }

  वापस CURLE_OK;
}

CURLcode Curl_req_start(काष्ठा SingleRequest *req,
                        काष्ठा Curl_easy *तथ्य)
{
  req->start = *Curl_pgrs_now(तथ्य);
  वापस Curl_req_soft_reset(req, तथ्य);
}

अटल CURLcode req_flush(काष्ठा Curl_easy *तथ्य);

CURLcode Curl_req_done(काष्ठा SingleRequest *req,
                       काष्ठा Curl_easy *तथ्य, bool aborted)
{
  (व्योम)req;
  अगर(!aborted)
    (व्योम)req_flush(तथ्य);
  Curl_client_reset(तथ्य);
  वापस CURLE_OK;
}

व्योम Curl_req_hard_reset(काष्ठा SingleRequest *req, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#अगर_अघोषित CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#पूर्ण_अगर
  Curl_client_reset(तथ्य);
  अगर(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(तथ्य);
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
  req->offset = 0;
  req->httpcode = 0;
  req->io_flags = 0;
  req->upgr101 = UPGR101_NONE;
  req->sendbuf_hds_len = 0;
  req->timeofdoc = 0;
  req->location = शून्य;
  req->newurl = शून्य;
#अगर_अघोषित CURL_DISABLE_COOKIES
  req->setcookies = 0;
#पूर्ण_अगर
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
  req->no_body = तथ्य->set.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_block(&तथ्य->progress.dl.rlimit, FALSE, &t0);
  Curl_rlimit_block(&तथ्य->progress.ul.rlimit, FALSE, &t0);
}

व्योम Curl_req_free(काष्ठा SingleRequest *req, काष्ठा Curl_easy *तथ्य)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#अगर_अघोषित CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#पूर्ण_अगर
  अगर(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(तथ्य);
}

अटल CURLcode xfer_send(काष्ठा Curl_easy *तथ्य,
                          स्थिर अक्षर *buf, माप_प्रकार blen,
                          माप_प्रकार hds_len, माप_प्रकार *pnwritten)
{
  CURLcode परिणाम = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#अगर_घोषित DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    माप_प्रकार body_len = blen - hds_len;
    अगर(body_len) {
      स्थिर अक्षर *p = दो_पर्या("CURL_SMALLREQSEND");
      अगर(p) {
        curl_off_t body_small;
        अगर(!curlx_str_संख्या(&p, &body_small, body_len))
          blen = hds_len + (माप_प्रकार)body_small;
      }
    }
  }
#पूर्ण_अगर
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  अगर(तथ्य->set.max_send_speed) {
    माप_प्रकार body_bytes = blen - hds_len;
    अगर((curl_off_t)body_bytes > तथ्य->set.max_send_speed)
      blen = hds_len + (माप_प्रकार)तथ्य->set.max_send_speed;
  }

  अगर(तथ्य->req.eos_read &&
    (Curl_bufq_is_empty(&तथ्य->req.sendbuf) ||
     Curl_bufq_len(&तथ्य->req.sendbuf) == blen)) {
    DEBUGF(infof(तथ्य, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  परिणाम = Curl_xfer_send(तथ्य, buf, blen, eos, pnwritten);
  अगर(!परिणाम) {
    अगर(eos && (blen == *pnwritten))
      तथ्य->req.eos_sent = TRUE;
    अगर(*pnwritten) {
      अगर(hds_len)
        Curl_debug(तथ्य, CURLINFO_HEADER_OUT, buf,
                   CURLMIN(hds_len, *pnwritten));
      अगर(*pnwritten > hds_len) {
        माप_प्रकार body_len = *pnwritten - hds_len;
        Curl_debug(तथ्य, CURLINFO_तथ्य_OUT, buf + hds_len, body_len);
        तथ्य->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(तथ्य, body_len);
      }
    }
  }
  वापस परिणाम;
}

अटल CURLcode req_send_बफर_flush(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अचिन्हित अक्षर *buf;
  माप_प्रकार blen;

  जबतक(Curl_bufq_peek(&तथ्य->req.sendbuf, &buf, &blen)) {
    माप_प्रकार nwritten, hds_len = CURLMIN(तथ्य->req.sendbuf_hds_len, blen);
    परिणाम = xfer_send(तथ्य, (स्थिर अक्षर *)buf, blen, hds_len, &nwritten);
    अगर(परिणाम)
      अवरोध;

    Curl_bufq_skip(&तथ्य->req.sendbuf, nwritten);
    अगर(hds_len) {
      तथ्य->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    अगर(nwritten < blen)
      अवरोध;
  }
  वापस परिणाम;
}

अटल CURLcode req_set_upload_done(काष्ठा Curl_easy *तथ्य)
{
  DEBUGASSERT(!तथ्य->req.upload_done);
  तथ्य->req.upload_done = TRUE;
  CURL_REQ_CLEAR_SEND(तथ्य);

  अगर(तथ्य->mstate >= MSTATE_DID)
    Curl_pgrsTime(तथ्य, TIMER_POSTRANSFER);
  Curl_creader_done(तथ्य, तथ्य->req.upload_aborted);

  अगर(तथ्य->req.upload_aborted) {
    Curl_bufq_reset(&तथ्य->req.sendbuf);
    अगर(तथ्य->req.writebytecount)
      infof(तथ्य, "abort upload after having sent %" FMT_OFF_T " bytes",
            तथ्य->req.writebytecount);
    अन्यथा
      infof(तथ्य, "abort upload");
  }
  अन्यथा अगर(तथ्य->req.writebytecount)
    infof(तथ्य, "upload completely sent off: %" FMT_OFF_T " bytes",
          तथ्य->req.writebytecount);
  अन्यथा अगर(!तथ्य->req.download_done) {
    DEBUGASSERT(Curl_bufq_is_empty(&तथ्य->req.sendbuf));
    infof(तथ्य, Curl_creader_योग_लंबाई(तथ्य) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  वापस Curl_xfer_send_close(तथ्य);
}

अटल CURLcode req_flush(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम;

  अगर(!तथ्य || !तथ्य->conn)
    वापस CURLE_FAILED_INIT;

  अगर(!Curl_bufq_is_empty(&तथ्य->req.sendbuf)) {
    परिणाम = req_send_बफर_flush(तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
    अगर(!Curl_bufq_is_empty(&तथ्य->req.sendbuf)) {
      DEBUGF(infof(तथ्य, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&तथ्य->req.sendbuf)));
      वापस CURLE_AGAIN;
    }
  }
  अन्यथा अगर(Curl_xfer_needs_flush(तथ्य)) {
    DEBUGF(infof(तथ्य, "Curl_req_flush(), xfer send_pending"));
    वापस Curl_xfer_flush(तथ्य);
  }

  अगर(तथ्य->req.eos_read && !तथ्य->req.eos_sent) {
    अक्षर tmp = 0;
    माप_प्रकार nwritten;
    परिणाम = xfer_send(तथ्य, &tmp, 0, 0, &nwritten);
    अगर(परिणाम)
      वापस परिणाम;
    DEBUGASSERT(तथ्य->req.eos_sent);
  }

  अगर(!तथ्य->req.upload_done && तथ्य->req.eos_read && तथ्य->req.eos_sent) {
    DEBUGASSERT(Curl_bufq_is_empty(&तथ्य->req.sendbuf));
    अगर(तथ्य->req.shutdown) {
      bool done;
      परिणाम = Curl_xfer_send_shutdown(तथ्य, &done);
      अगर(परिणाम && तथ्य->req.shutdown_err_ignore) {
        infof(तथ्य, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (पूर्णांक)परिणाम);
        परिणाम = CURLE_OK;
        done = TRUE;
      }

      अगर(परिणाम)
        वापस परिणाम;
      अगर(!done)
        वापस CURLE_AGAIN;
    }
    वापस req_set_upload_done(तथ्य);
  }
  वापस CURLE_OK;
}

अटल CURLcode add_from_client(व्योम *reader_ctx,
                                अचिन्हित अक्षर *buf, माप_प्रकार buflen,
                                माप_प्रकार *pnread)
{
  काष्ठा Curl_easy *तथ्य = reader_ctx;
  CURLcode परिणाम;
  bool eos;

  परिणाम = Curl_client_read(तथ्य, (अक्षर *)buf, buflen, pnread, &eos);
  अगर(!परिणाम && eos)
    तथ्य->req.eos_read = TRUE;
  वापस परिणाम;
}

अटल CURLcode req_send_बफर_add(काष्ठा Curl_easy *तथ्य,
                                    स्थिर अक्षर *buf, माप_प्रकार blen,
                                    माप_प्रकार hds_len)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार n;
  परिणाम = Curl_bufq_cwrite(&तथ्य->req.sendbuf, buf, blen, &n);
  अगर(परिणाम)
    वापस परिणाम;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  तथ्य->req.sendbuf_hds_len += hds_len;
  वापस CURLE_OK;
}

CURLcode Curl_req_send(काष्ठा Curl_easy *तथ्य, काष्ठा dynbuf *req,
                       अचिन्हित अक्षर httpversion)
{
  CURLcode परिणाम;
  स्थिर अक्षर *buf;
  माप_प्रकार blen, nwritten;

  अगर(!तथ्य || !तथ्य->conn)
    वापस CURLE_FAILED_INIT;

  तथ्य->req.httpversion_sent = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  अगर(Curl_bufq_is_empty(&तथ्य->req.sendbuf) &&
     !Curl_creader_योग_लंबाई(तथ्य) &&
     (blen <= तथ्य->req.sendbuf.chunk_आकार)) {
    तथ्य->req.eos_read = TRUE;
    परिणाम = xfer_send(तथ्य, buf, blen, blen, &nwritten);
    अगर(परिणाम)
      वापस परिणाम;
    buf += nwritten;
    blen -= nwritten;
    अगर(!blen) {
      परिणाम = req_set_upload_done(तथ्य);
      अगर(परिणाम)
        वापस परिणाम;
    }
  }

  अगर(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    परिणाम = req_send_बफर_add(तथ्य, buf, blen, blen);
    अगर(परिणाम)
      वापस परिणाम;

    वापस Curl_req_send_more(तथ्य);
  }
  वापस CURLE_OK;
}

bool Curl_req_sendbuf_empty(काष्ठा Curl_easy *तथ्य)
{
  वापस !तथ्य->req.sendbuf_init || Curl_bufq_is_empty(&तथ्य->req.sendbuf);
}

bool Curl_req_want_send(काष्ठा Curl_easy *तथ्य)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  वापस !तथ्य->req.done &&
         !Curl_rlimit_is_blocked(&तथ्य->progress.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(तथ्य) ||
          !Curl_req_sendbuf_empty(तथ्य) ||
          Curl_xfer_needs_flush(तथ्य));
}

bool Curl_req_want_recv(काष्ठा Curl_easy *तथ्य)
{
  /* Not done and download not blocked and want RECV */
  वापस !तथ्य->req.done &&
         !Curl_rlimit_is_blocked(&तथ्य->progress.dl.rlimit) &&
         CURL_REQ_WANT_RECV(तथ्य);
}

bool Curl_req_done_sending(काष्ठा Curl_easy *तथ्य)
{
  वापस तथ्य->req.upload_done && !Curl_req_want_send(तथ्य);
}

CURLcode Curl_req_send_more(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम;

  /* Fill our send buffer if more from client can be read. */
  अगर(!तथ्य->req.upload_aborted &&
     !तथ्य->req.eos_read &&
     !Curl_xfer_send_is_paused(तथ्य) &&
     !Curl_bufq_is_full(&तथ्य->req.sendbuf)) {
    माप_प्रकार nread;
    परिणाम = Curl_bufq_sipn(&तथ्य->req.sendbuf, 0,
                            add_from_client, तथ्य, &nread);
    अगर(परिणाम && परिणाम != CURLE_AGAIN)
      वापस परिणाम;
  }

  परिणाम = req_flush(तथ्य);
  अगर(परिणाम == CURLE_AGAIN)
    परिणाम = CURLE_OK;

  वापस परिणाम;
}

CURLcode Curl_req_abort_sending(काष्ठा Curl_easy *तथ्य)
{
  अगर(!तथ्य->req.upload_done) {
    Curl_bufq_reset(&तथ्य->req.sendbuf);
    तथ्य->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(तथ्य);
    वापस req_set_upload_done(तथ्य);
  }
  वापस CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(काष्ठा Curl_easy *तथ्य)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode परिणाम = CURLE_OK;
  अगर(CURL_REQ_WANT_SEND(तथ्य))
    परिणाम = Curl_req_abort_sending(तथ्य);
  CURL_REQ_CLEAR_IO(तथ्य);
  वापस परिणाम;
}
