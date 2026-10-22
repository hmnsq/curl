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
  स_रखो(req, 0, परिमाण(*req));
}

CURLcode Curl_req_soft_reset(संरचना SingleRequest *req,
                             संरचना Curl_easy *data)
{
  CURLcode result;

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

  result = Curl_client_start(data);
  यदि(result)
    फर्काऊ result;

  यदि(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, data->set.upload_buffer_size, 1,
                    BUFQ_OPT_SOFT_LIMIT);
    req->sendbuf_init = TRUE;
  }
  अन्यथा {
    Curl_bufq_reset(&req->sendbuf);
    यदि(data->set.upload_buffer_size != req->sendbuf.chunk_size) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, data->set.upload_buffer_size, 1,
                      BUFQ_OPT_SOFT_LIMIT);
    }
  }

  फर्काऊ CURLE_OK;
}

CURLcode Curl_req_start(संरचना SingleRequest *req,
                        संरचना Curl_easy *data)
{
  req->start = *Curl_pgrs_now(data);
  फर्काऊ Curl_req_soft_reset(req, data);
}

स्थिर CURLcode req_flush(संरचना Curl_easy *data);

CURLcode Curl_req_done(संरचना SingleRequest *req,
                       संरचना Curl_easy *data, bool aborted)
{
  (शून्य)req;
  यदि(!aborted)
    (शून्य)req_flush(data);
  Curl_client_reset(data);
  फर्काऊ CURLE_OK;
}

शून्य Curl_req_hard_reset(संरचना SingleRequest *req, संरचना Curl_easy *data)
{
  संरचना curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ifndef CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#endif
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#endif
  Curl_client_reset(data);
  यदि(req->sendbuf_init)
    Curl_bufq_reset(&req->sendbuf);

  /* clear any resolve data */
  Curl_resolv_destroy_all(data);
  /* Can no longer memset() this struct as we need to keep some state */
  req->size = -1;
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
  req->location = रिक्त;
  req->newurl = रिक्त;
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
  req->no_body = data->set.opt_no_body;
  req->authneg = FALSE;
  req->shutdown = FALSE;
  /* Unpause all directions */
  Curl_rlimit_block(&data->progress.dl.rlimit, FALSE, &t0);
  Curl_rlimit_block(&data->progress.ul.rlimit, FALSE, &t0);
}

शून्य Curl_req_free(संरचना SingleRequest *req, संरचना Curl_easy *data)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ifndef CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#endif
  यदि(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(data);
}

स्थिर CURLcode xfer_send(संरचना Curl_easy *data,
                          अचर वर्ण *buf, माप_प्रकार blen,
                          माप_प्रकार hds_len, माप_प्रकार *pnwritten)
{
  CURLcode result = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#ifdef DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    माप_प्रकार body_len = blen - hds_len;
    यदि(body_len) {
      अचर वर्ण *p = दो_पर्या("CURL_SMALLREQSEND");
      यदि(p) {
        curl_off_t body_small;
        यदि(!curlx_str_सङ्ख्या(&p, &body_small, body_len))
          blen = hds_len + (माप_प्रकार)body_small;
      }
    }
  }
#endif
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  यदि(data->set.max_send_speed) {
    माप_प्रकार body_bytes = blen - hds_len;
    यदि((curl_off_t)body_bytes > data->set.max_send_speed)
      blen = hds_len + (माप_प्रकार)data->set.max_send_speed;
  }

  यदि(data->req.eos_read &&
    (Curl_bufq_is_empty(&data->req.sendbuf) ||
     Curl_bufq_len(&data->req.sendbuf) == blen)) {
    DEBUGF(infof(data, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  result = Curl_xfer_send(data, buf, blen, eos, pnwritten);
  यदि(!result) {
    यदि(eos && (blen == *pnwritten))
      data->req.eos_sent = TRUE;
    यदि(*pnwritten) {
      यदि(hds_len)
        Curl_debug(data, CURLINFO_HEADER_OUT, buf,
                   CURLMIN(hds_len, *pnwritten));
      यदि(*pnwritten > hds_len) {
        माप_प्रकार body_len = *pnwritten - hds_len;
        Curl_debug(data, CURLINFO_DATA_OUT, buf + hds_len, body_len);
        data->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(data, body_len);
      }
    }
  }
  फर्काऊ result;
}

स्थिर CURLcode req_send_buffer_flush(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  अचर अचिन्हित वर्ण *buf;
  माप_प्रकार blen;

  जबसम्म(Curl_bufq_peek(&data->req.sendbuf, &buf, &blen)) {
    माप_प्रकार nwritten, hds_len = CURLMIN(data->req.sendbuf_hds_len, blen);
    result = xfer_send(data, (अचर वर्ण *)buf, blen, hds_len, &nwritten);
    यदि(result)
      रोक;

    Curl_bufq_skip(&data->req.sendbuf, nwritten);
    यदि(hds_len) {
      data->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    यदि(nwritten < blen)
      रोक;
  }
  फर्काऊ result;
}

स्थिर CURLcode req_set_upload_done(संरचना Curl_easy *data)
{
  DEBUGASSERT(!data->req.upload_done);
  data->req.upload_done = TRUE;
  CURL_REQ_CLEAR_SEND(data);

  यदि(data->mstate >= MSTATE_DID)
    Curl_pgrsTime(data, TIMER_POSTRANSFER);
  Curl_creader_done(data, data->req.upload_aborted);

  यदि(data->req.upload_aborted) {
    Curl_bufq_reset(&data->req.sendbuf);
    यदि(data->req.writebytecount)
      infof(data, "abort upload after having sent %" FMT_OFF_T " bytes",
            data->req.writebytecount);
    अन्यथा
      infof(data, "abort upload");
  }
  अन्यथा यदि(data->req.writebytecount)
    infof(data, "upload completely sent off: %" FMT_OFF_T " bytes",
          data->req.writebytecount);
  अन्यथा यदि(!data->req.download_done) {
    DEBUGASSERT(Curl_bufq_is_empty(&data->req.sendbuf));
    infof(data, Curl_creader_total_length(data) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  फर्काऊ Curl_xfer_send_close(data);
}

स्थिर CURLcode req_flush(संरचना Curl_easy *data)
{
  CURLcode result;

  यदि(!data || !data->conn)
    फर्काऊ CURLE_FAILED_INIT;

  यदि(!Curl_bufq_is_empty(&data->req.sendbuf)) {
    result = req_send_buffer_flush(data);
    यदि(result)
      फर्काऊ result;
    यदि(!Curl_bufq_is_empty(&data->req.sendbuf)) {
      DEBUGF(infof(data, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&data->req.sendbuf)));
      फर्काऊ CURLE_AGAIN;
    }
  }
  अन्यथा यदि(Curl_xfer_needs_flush(data)) {
    DEBUGF(infof(data, "Curl_req_flush(), xfer send_pending"));
    फर्काऊ Curl_xfer_flush(data);
  }

  यदि(data->req.eos_read && !data->req.eos_sent) {
    वर्ण tmp = 0;
    माप_प्रकार nwritten;
    result = xfer_send(data, &tmp, 0, 0, &nwritten);
    यदि(result)
      फर्काऊ result;
    DEBUGASSERT(data->req.eos_sent);
  }

  यदि(!data->req.upload_done && data->req.eos_read && data->req.eos_sent) {
    DEBUGASSERT(Curl_bufq_is_empty(&data->req.sendbuf));
    यदि(data->req.shutdown) {
      bool done;
      result = Curl_xfer_send_shutdown(data, &done);
      यदि(result && data->req.shutdown_err_ignore) {
        infof(data, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (पूर्णाङ्क)result);
        result = CURLE_OK;
        done = TRUE;
      }

      यदि(result)
        फर्काऊ result;
      यदि(!done)
        फर्काऊ CURLE_AGAIN;
    }
    फर्काऊ req_set_upload_done(data);
  }
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode add_from_client(शून्य *reader_ctx,
                                अचिन्हित वर्ण *buf, माप_प्रकार buflen,
                                माप_प्रकार *pnread)
{
  संरचना Curl_easy *data = reader_ctx;
  CURLcode result;
  bool eos;

  result = Curl_client_read(data, (वर्ण *)buf, buflen, pnread, &eos);
  यदि(!result && eos)
    data->req.eos_read = TRUE;
  फर्काऊ result;
}

स्थिर CURLcode req_send_buffer_add(संरचना Curl_easy *data,
                                    अचर वर्ण *buf, माप_प्रकार blen,
                                    माप_प्रकार hds_len)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार n;
  result = Curl_bufq_cwrite(&data->req.sendbuf, buf, blen, &n);
  यदि(result)
    फर्काऊ result;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  data->req.sendbuf_hds_len += hds_len;
  फर्काऊ CURLE_OK;
}

CURLcode Curl_req_send(संरचना Curl_easy *data, संरचना dynbuf *req,
                       अचिन्हित वर्ण httpversion)
{
  CURLcode result;
  अचर वर्ण *buf;
  माप_प्रकार blen, nwritten;

  यदि(!data || !data->conn)
    फर्काऊ CURLE_FAILED_INIT;

  data->req.httpversion_sent = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  यदि(Curl_bufq_is_empty(&data->req.sendbuf) &&
     !Curl_creader_total_length(data) &&
     (blen <= data->req.sendbuf.chunk_size)) {
    data->req.eos_read = TRUE;
    result = xfer_send(data, buf, blen, blen, &nwritten);
    यदि(result)
      फर्काऊ result;
    buf += nwritten;
    blen -= nwritten;
    यदि(!blen) {
      result = req_set_upload_done(data);
      यदि(result)
        फर्काऊ result;
    }
  }

  यदि(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    result = req_send_buffer_add(data, buf, blen, blen);
    यदि(result)
      फर्काऊ result;

    फर्काऊ Curl_req_send_more(data);
  }
  फर्काऊ CURLE_OK;
}

bool Curl_req_sendbuf_empty(संरचना Curl_easy *data)
{
  फर्काऊ !data->req.sendbuf_init || Curl_bufq_is_empty(&data->req.sendbuf);
}

bool Curl_req_want_send(संरचना Curl_easy *data)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  फर्काऊ !data->req.done &&
         !Curl_rlimit_is_blocked(&data->progress.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(data) ||
          !Curl_req_sendbuf_empty(data) ||
          Curl_xfer_needs_flush(data));
}

bool Curl_req_want_recv(संरचना Curl_easy *data)
{
  /* Not done and download not blocked and want RECV */
  फर्काऊ !data->req.done &&
         !Curl_rlimit_is_blocked(&data->progress.dl.rlimit) &&
         CURL_REQ_WANT_RECV(data);
}

bool Curl_req_done_sending(संरचना Curl_easy *data)
{
  फर्काऊ data->req.upload_done && !Curl_req_want_send(data);
}

CURLcode Curl_req_send_more(संरचना Curl_easy *data)
{
  CURLcode result;

  /* Fill our send buffer if more from client can be read. */
  यदि(!data->req.upload_aborted &&
     !data->req.eos_read &&
     !Curl_xfer_send_is_paused(data) &&
     !Curl_bufq_is_full(&data->req.sendbuf)) {
    माप_प्रकार nread;
    result = Curl_bufq_sipn(&data->req.sendbuf, 0,
                            add_from_client, data, &nread);
    यदि(result && result != CURLE_AGAIN)
      फर्काऊ result;
  }

  result = req_flush(data);
  यदि(result == CURLE_AGAIN)
    result = CURLE_OK;

  फर्काऊ result;
}

CURLcode Curl_req_abort_sending(संरचना Curl_easy *data)
{
  यदि(!data->req.upload_done) {
    Curl_bufq_reset(&data->req.sendbuf);
    data->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(data);
    फर्काऊ req_set_upload_done(data);
  }
  फर्काऊ CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(संरचना Curl_easy *data)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode result = CURLE_OK;
  यदि(CURL_REQ_WANT_SEND(data))
    result = Curl_req_abort_sending(data);
  CURL_REQ_CLEAR_IO(data);
  फर्काऊ result;
}
