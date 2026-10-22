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
#include "curl_setup.h"

#include "urldata.h"
#include "cfilters.h"
#include "curlx/dynbuf.h"
#include "progress.h"
#include "request.h"
#include "sendf.h"
#include "curl_trc.h"
#include "transfer.h"
#include "url.h"
#include "curlx/strparse.h"

void Curl_req_init(struct SingleRequest *req)
{
  memset(req, 0, sizeof(*req));
}

CURLcode Curl_req_soft_reset(struct SingleRequest *req,
                             struct Curl_easy *data)
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
  если(result)
    вернуть result;

  если(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, data->set.upload_buffer_size, 1,
                    BUFQ_OPT_SOFT_LIMIT);
    req->sendbuf_init = TRUE;
  }
  иначе {
    Curl_bufq_reset(&req->sendbuf);
    если(data->set.upload_buffer_size != req->sendbuf.chunk_size) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, data->set.upload_buffer_size, 1,
                      BUFQ_OPT_SOFT_LIMIT);
    }
  }

  вернуть CURLE_OK;
}

CURLcode Curl_req_start(struct SingleRequest *req,
                        struct Curl_easy *data)
{
  req->start = *Curl_pgrs_now(data);
  вернуть Curl_req_soft_reset(req, data);
}

static CURLcode req_flush(struct Curl_easy *data);

CURLcode Curl_req_done(struct SingleRequest *req,
                       struct Curl_easy *data, bool aborted)
{
  (void)req;
  если(!aborted)
    (void)req_flush(data);
  Curl_client_reset(data);
  вернуть CURLE_OK;
}

void Curl_req_hard_reset(struct SingleRequest *req, struct Curl_easy *data)
{
  struct curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ifndef CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#endif
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#endif
  Curl_client_reset(data);
  если(req->sendbuf_init)
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
  req->location = NULL;
  req->newurl = NULL;
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

void Curl_req_free(struct SingleRequest *req, struct Curl_easy *data)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ifndef CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#endif
  если(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(data);
}

static CURLcode xfer_send(struct Curl_easy *data,
                          const char *buf, size_t blen,
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
    если(body_len) {
      const char *p = getenv("CURL_SMALLREQSEND");
      если(p) {
        curl_off_t body_small;
        если(!curlx_str_number(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#endif
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  если(data->set.max_send_speed) {
    size_t body_bytes = blen - hds_len;
    если((curl_off_t)body_bytes > data->set.max_send_speed)
      blen = hds_len + (size_t)data->set.max_send_speed;
  }

  если(data->req.eos_read &&
    (Curl_bufq_is_empty(&data->req.sendbuf) ||
     Curl_bufq_len(&data->req.sendbuf) == blen)) {
    DEBUGF(infof(data, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  result = Curl_xfer_send(data, buf, blen, eos, pnwritten);
  если(!result) {
    если(eos && (blen == *pnwritten))
      data->req.eos_sent = TRUE;
    если(*pnwritten) {
      если(hds_len)
        Curl_debug(data, CURLINFO_HEADER_OUT, buf,
                   CURLMIN(hds_len, *pnwritten));
      если(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(data, CURLINFO_DATA_OUT, buf + hds_len, body_len);
        data->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(data, body_len);
      }
    }
  }
  вернуть result;
}

static CURLcode req_send_buffer_flush(struct Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  const unsigned char *buf;
  size_t blen;

  пока(Curl_bufq_peek(&data->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(data->req.sendbuf_hds_len, blen);
    result = xfer_send(data, (const char *)buf, blen, hds_len, &nwritten);
    если(result)
      break;

    Curl_bufq_skip(&data->req.sendbuf, nwritten);
    если(hds_len) {
      data->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    если(nwritten < blen)
      break;
  }
  вернуть result;
}

static CURLcode req_set_upload_done(struct Curl_easy *data)
{
  DEBUGASSERT(!data->req.upload_done);
  data->req.upload_done = TRUE;
  CURL_REQ_CLEAR_SEND(data);

  если(data->mstate >= MSTATE_DID)
    Curl_pgrsTime(data, TIMER_POSTRANSFER);
  Curl_creader_done(data, data->req.upload_aborted);

  если(data->req.upload_aborted) {
    Curl_bufq_reset(&data->req.sendbuf);
    если(data->req.writebytecount)
      infof(data, "abort upload after having sent %" FMT_OFF_T " bytes",
            data->req.writebytecount);
    иначе
      infof(data, "abort upload");
  }
  иначе если(data->req.writebytecount)
    infof(data, "upload completely sent off: %" FMT_OFF_T " bytes",
          data->req.writebytecount);
  иначе если(!data->req.download_done) {
    DEBUGASSERT(Curl_bufq_is_empty(&data->req.sendbuf));
    infof(data, Curl_creader_total_length(data) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  вернуть Curl_xfer_send_close(data);
}

static CURLcode req_flush(struct Curl_easy *data)
{
  CURLcode result;

  если(!data || !data->conn)
    вернуть CURLE_FAILED_INIT;

  если(!Curl_bufq_is_empty(&data->req.sendbuf)) {
    result = req_send_buffer_flush(data);
    если(result)
      вернуть result;
    если(!Curl_bufq_is_empty(&data->req.sendbuf)) {
      DEBUGF(infof(data, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&data->req.sendbuf)));
      вернуть CURLE_AGAIN;
    }
  }
  иначе если(Curl_xfer_needs_flush(data)) {
    DEBUGF(infof(data, "Curl_req_flush(), xfer send_pending"));
    вернуть Curl_xfer_flush(data);
  }

  если(data->req.eos_read && !data->req.eos_sent) {
    char tmp = 0;
    size_t nwritten;
    result = xfer_send(data, &tmp, 0, 0, &nwritten);
    если(result)
      вернуть result;
    DEBUGASSERT(data->req.eos_sent);
  }

  если(!data->req.upload_done && data->req.eos_read && data->req.eos_sent) {
    DEBUGASSERT(Curl_bufq_is_empty(&data->req.sendbuf));
    если(data->req.shutdown) {
      bool done;
      result = Curl_xfer_send_shutdown(data, &done);
      если(result && data->req.shutdown_err_ignore) {
        infof(data, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (целое)result);
        result = CURLE_OK;
        done = TRUE;
      }

      если(result)
        вернуть result;
      если(!done)
        вернуть CURLE_AGAIN;
    }
    вернуть req_set_upload_done(data);
  }
  вернуть CURLE_OK;
}

static CURLcode add_from_client(void *reader_ctx,
                                unsigned char *buf, size_t buflen,
                                size_t *pnread)
{
  struct Curl_easy *data = reader_ctx;
  CURLcode result;
  bool eos;

  result = Curl_client_read(data, (char *)buf, buflen, pnread, &eos);
  если(!result && eos)
    data->req.eos_read = TRUE;
  вернуть result;
}

static CURLcode req_send_buffer_add(struct Curl_easy *data,
                                    const char *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode result = CURLE_OK;
  size_t n;
  result = Curl_bufq_cwrite(&data->req.sendbuf, buf, blen, &n);
  если(result)
    вернуть result;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  data->req.sendbuf_hds_len += hds_len;
  вернуть CURLE_OK;
}

CURLcode Curl_req_send(struct Curl_easy *data, struct dynbuf *req,
                       unsigned char httpversion)
{
  CURLcode result;
  const char *buf;
  size_t blen, nwritten;

  если(!data || !data->conn)
    вернуть CURLE_FAILED_INIT;

  data->req.httpversion_sent = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  если(Curl_bufq_is_empty(&data->req.sendbuf) &&
     !Curl_creader_total_length(data) &&
     (blen <= data->req.sendbuf.chunk_size)) {
    data->req.eos_read = TRUE;
    result = xfer_send(data, buf, blen, blen, &nwritten);
    если(result)
      вернуть result;
    buf += nwritten;
    blen -= nwritten;
    если(!blen) {
      result = req_set_upload_done(data);
      если(result)
        вернуть result;
    }
  }

  если(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    result = req_send_buffer_add(data, buf, blen, blen);
    если(result)
      вернуть result;

    вернуть Curl_req_send_more(data);
  }
  вернуть CURLE_OK;
}

bool Curl_req_sendbuf_empty(struct Curl_easy *data)
{
  вернуть !data->req.sendbuf_init || Curl_bufq_is_empty(&data->req.sendbuf);
}

bool Curl_req_want_send(struct Curl_easy *data)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  вернуть !data->req.done &&
         !Curl_rlimit_is_blocked(&data->progress.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(data) ||
          !Curl_req_sendbuf_empty(data) ||
          Curl_xfer_needs_flush(data));
}

bool Curl_req_want_recv(struct Curl_easy *data)
{
  /* Not done and download not blocked and want RECV */
  вернуть !data->req.done &&
         !Curl_rlimit_is_blocked(&data->progress.dl.rlimit) &&
         CURL_REQ_WANT_RECV(data);
}

bool Curl_req_done_sending(struct Curl_easy *data)
{
  вернуть data->req.upload_done && !Curl_req_want_send(data);
}

CURLcode Curl_req_send_more(struct Curl_easy *data)
{
  CURLcode result;

  /* Fill our send buffer if more from client can be read. */
  если(!data->req.upload_aborted &&
     !data->req.eos_read &&
     !Curl_xfer_send_is_paused(data) &&
     !Curl_bufq_is_full(&data->req.sendbuf)) {
    size_t nread;
    result = Curl_bufq_sipn(&data->req.sendbuf, 0,
                            add_from_client, data, &nread);
    если(result && result != CURLE_AGAIN)
      вернуть result;
  }

  result = req_flush(data);
  если(result == CURLE_AGAIN)
    result = CURLE_OK;

  вернуть result;
}

CURLcode Curl_req_abort_sending(struct Curl_easy *data)
{
  если(!data->req.upload_done) {
    Curl_bufq_reset(&data->req.sendbuf);
    data->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(data);
    вернуть req_set_upload_done(data);
  }
  вернуть CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(struct Curl_easy *data)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode result = CURLE_OK;
  если(CURL_REQ_WANT_SEND(data))
    result = Curl_req_abort_sending(data);
  CURL_REQ_CLEAR_IO(data);
  вернуть result;
}
