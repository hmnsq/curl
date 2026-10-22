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
                             بنية Curl_easy *data)
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
  אם(result)
    החזר result;

  אם(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, data->set.upload_buffer_size, 1,
                    BUFQ_OPT_SOFT_LIMIT);
    req->sendbuf_init = TRUE;
  }
  אחרת {
    Curl_bufq_reset(&req->sendbuf);
    אם(data->set.upload_buffer_size != req->sendbuf.chunk_size) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, data->set.upload_buffer_size, 1,
                      BUFQ_OPT_SOFT_LIMIT);
    }
  }

  החזר CURLE_OK;
}

CURLcode Curl_req_start(بنية SingleRequest *req,
                        بنية Curl_easy *data)
{
  req->start = *Curl_pgrs_now(data);
  החזר Curl_req_soft_reset(req, data);
}

ساكن CURLcode req_flush(بنية Curl_easy *data);

CURLcode Curl_req_done(بنية SingleRequest *req,
                       بنية Curl_easy *data, bool aborted)
{
  (فراغ)req;
  אם(!aborted)
    (فراغ)req_flush(data);
  Curl_client_reset(data);
  החזר CURLE_OK;
}

فراغ Curl_req_hard_reset(بنية SingleRequest *req, بنية Curl_easy *data)
{
  بنية curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ifndef CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#endif
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#endif
  Curl_client_reset(data);
  אם(req->sendbuf_init)
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
  req->location = عدم;
  req->newurl = عدم;
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

فراغ Curl_req_free(بنية SingleRequest *req, بنية Curl_easy *data)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ifndef CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#endif
  אם(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(data);
}

ساكن CURLcode xfer_send(بنية Curl_easy *data,
                          ثابت حرف *buf, size_t blen,
                          size_t hds_len, size_t *pnwritten)
{
  CURLcode result = CURLE_OK;
  bool eos = FALSE;

  *pnwritten = 0;
  DEBUGASSERT(hds_len <= blen);
#ifdef DEBUGBUILD
  {
    /* Allow debug builds to override this logic to force short initial
       sends */
    size_t body_len = blen - hds_len;
    אם(body_len) {
      ثابت حرف *p = getenv("CURL_SMALLREQSEND");
      אם(p) {
        curl_off_t body_small;
        אם(!curlx_str_number(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#endif
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  אם(data->set.max_send_speed) {
    size_t body_bytes = blen - hds_len;
    אם((curl_off_t)body_bytes > data->set.max_send_speed)
      blen = hds_len + (size_t)data->set.max_send_speed;
  }

  אם(data->req.eos_read &&
    (Curl_bufq_is_empty(&data->req.sendbuf) ||
     Curl_bufq_len(&data->req.sendbuf) == blen)) {
    DEBUGF(infof(data, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  result = Curl_xfer_send(data, buf, blen, eos, pnwritten);
  אם(!result) {
    אם(eos && (blen == *pnwritten))
      data->req.eos_sent = TRUE;
    אם(*pnwritten) {
      אם(hds_len)
        Curl_debug(data, CURLINFO_HEADER_OUT, buf,
                   CURLMIN(hds_len, *pnwritten));
      אם(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(data, CURLINFO_DATA_OUT, buf + hds_len, body_len);
        data->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(data, body_len);
      }
    }
  }
  החזר result;
}

ساكن CURLcode req_send_buffer_flush(بنية Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ثابت unsigned حرف *buf;
  size_t blen;

  כלעוד(Curl_bufq_peek(&data->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(data->req.sendbuf_hds_len, blen);
    result = xfer_send(data, (ثابت حرف *)buf, blen, hds_len, &nwritten);
    אם(result)
      اكسر;

    Curl_bufq_skip(&data->req.sendbuf, nwritten);
    אם(hds_len) {
      data->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    אם(nwritten < blen)
      اكسر;
  }
  החזר result;
}

ساكن CURLcode req_set_upload_done(بنية Curl_easy *data)
{
  DEBUGASSERT(!data->req.upload_done);
  data->req.upload_done = TRUE;
  CURL_REQ_CLEAR_SEND(data);

  אם(data->mstate >= MSTATE_DID)
    Curl_pgrsTime(data, TIMER_POSTRANSFER);
  Curl_creader_done(data, data->req.upload_aborted);

  אם(data->req.upload_aborted) {
    Curl_bufq_reset(&data->req.sendbuf);
    אם(data->req.writebytecount)
      infof(data, "abort upload after having sent %" FMT_OFF_T " bytes",
            data->req.writebytecount);
    אחרת
      infof(data, "abort upload");
  }
  אחרת אם(data->req.writebytecount)
    infof(data, "upload completely sent off: %" FMT_OFF_T " bytes",
          data->req.writebytecount);
  אחרת אם(!data->req.download_done) {
    DEBUGASSERT(Curl_bufq_is_empty(&data->req.sendbuf));
    infof(data, Curl_creader_total_length(data) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  החזר Curl_xfer_send_close(data);
}

ساكن CURLcode req_flush(بنية Curl_easy *data)
{
  CURLcode result;

  אם(!data || !data->conn)
    החזר CURLE_FAILED_INIT;

  אם(!Curl_bufq_is_empty(&data->req.sendbuf)) {
    result = req_send_buffer_flush(data);
    אם(result)
      החזר result;
    אם(!Curl_bufq_is_empty(&data->req.sendbuf)) {
      DEBUGF(infof(data, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&data->req.sendbuf)));
      החזר CURLE_AGAIN;
    }
  }
  אחרת אם(Curl_xfer_needs_flush(data)) {
    DEBUGF(infof(data, "Curl_req_flush(), xfer send_pending"));
    החזר Curl_xfer_flush(data);
  }

  אם(data->req.eos_read && !data->req.eos_sent) {
    حرف tmp = 0;
    size_t nwritten;
    result = xfer_send(data, &tmp, 0, 0, &nwritten);
    אם(result)
      החזר result;
    DEBUGASSERT(data->req.eos_sent);
  }

  אם(!data->req.upload_done && data->req.eos_read && data->req.eos_sent) {
    DEBUGASSERT(Curl_bufq_is_empty(&data->req.sendbuf));
    אם(data->req.shutdown) {
      bool done;
      result = Curl_xfer_send_shutdown(data, &done);
      אם(result && data->req.shutdown_err_ignore) {
        infof(data, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (صحيح)result);
        result = CURLE_OK;
        done = TRUE;
      }

      אם(result)
        החזר result;
      אם(!done)
        החזר CURLE_AGAIN;
    }
    החזר req_set_upload_done(data);
  }
  החזר CURLE_OK;
}

ساكن CURLcode add_from_client(فراغ *reader_ctx,
                                unsigned حرف *buf, size_t buflen,
                                size_t *pnread)
{
  بنية Curl_easy *data = reader_ctx;
  CURLcode result;
  bool eos;

  result = Curl_client_read(data, (حرف *)buf, buflen, pnread, &eos);
  אם(!result && eos)
    data->req.eos_read = TRUE;
  החזר result;
}

ساكن CURLcode req_send_buffer_add(بنية Curl_easy *data,
                                    ثابت حرف *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode result = CURLE_OK;
  size_t n;
  result = Curl_bufq_cwrite(&data->req.sendbuf, buf, blen, &n);
  אם(result)
    החזר result;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  data->req.sendbuf_hds_len += hds_len;
  החזר CURLE_OK;
}

CURLcode Curl_req_send(بنية Curl_easy *data, بنية dynbuf *req,
                       unsigned حرف httpversion)
{
  CURLcode result;
  ثابت حرف *buf;
  size_t blen, nwritten;

  אם(!data || !data->conn)
    החזר CURLE_FAILED_INIT;

  data->req.httpversion_sent = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  אם(Curl_bufq_is_empty(&data->req.sendbuf) &&
     !Curl_creader_total_length(data) &&
     (blen <= data->req.sendbuf.chunk_size)) {
    data->req.eos_read = TRUE;
    result = xfer_send(data, buf, blen, blen, &nwritten);
    אם(result)
      החזר result;
    buf += nwritten;
    blen -= nwritten;
    אם(!blen) {
      result = req_set_upload_done(data);
      אם(result)
        החזר result;
    }
  }

  אם(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    result = req_send_buffer_add(data, buf, blen, blen);
    אם(result)
      החזר result;

    החזר Curl_req_send_more(data);
  }
  החזר CURLE_OK;
}

bool Curl_req_sendbuf_empty(بنية Curl_easy *data)
{
  החזר !data->req.sendbuf_init || Curl_bufq_is_empty(&data->req.sendbuf);
}

bool Curl_req_want_send(بنية Curl_easy *data)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  החזר !data->req.done &&
         !Curl_rlimit_is_blocked(&data->progress.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(data) ||
          !Curl_req_sendbuf_empty(data) ||
          Curl_xfer_needs_flush(data));
}

bool Curl_req_want_recv(بنية Curl_easy *data)
{
  /* Not done and download not blocked and want RECV */
  החזר !data->req.done &&
         !Curl_rlimit_is_blocked(&data->progress.dl.rlimit) &&
         CURL_REQ_WANT_RECV(data);
}

bool Curl_req_done_sending(بنية Curl_easy *data)
{
  החזר data->req.upload_done && !Curl_req_want_send(data);
}

CURLcode Curl_req_send_more(بنية Curl_easy *data)
{
  CURLcode result;

  /* Fill our send buffer if more from client can be read. */
  אם(!data->req.upload_aborted &&
     !data->req.eos_read &&
     !Curl_xfer_send_is_paused(data) &&
     !Curl_bufq_is_full(&data->req.sendbuf)) {
    size_t nread;
    result = Curl_bufq_sipn(&data->req.sendbuf, 0,
                            add_from_client, data, &nread);
    אם(result && result != CURLE_AGAIN)
      החזר result;
  }

  result = req_flush(data);
  אם(result == CURLE_AGAIN)
    result = CURLE_OK;

  החזר result;
}

CURLcode Curl_req_abort_sending(بنية Curl_easy *data)
{
  אם(!data->req.upload_done) {
    Curl_bufq_reset(&data->req.sendbuf);
    data->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(data);
    החזר req_set_upload_done(data);
  }
  החזר CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(بنية Curl_easy *data)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode result = CURLE_OK;
  אם(CURL_REQ_WANT_SEND(data))
    result = Curl_req_abort_sending(data);
  CURL_REQ_CLEAR_IO(data);
  החזר result;
}
