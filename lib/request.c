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

void Curl_req_init(sa_nracanaa SingleRequest *req)
{
  memset(req, 0, parimaa_na(*req));
}

CURLcode Curl_req_soft_reset(sa_nracanaa SingleRequest *req,
                             sa_nracanaa Curl_easy *data)
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
  ᰀᰧ(result)
    ᰀᰧᰌ result;

  ᰀᰧ(!req->sendbuf_init) {
    Curl_bufq_init2(&req->sendbuf, data->set.upload_buffer_size, 1,
                    BUFQ_OPT_SOFT_LIMIT);
    req->sendbuf_init = TRUE;
  }
  ᰀᰧᰀ {
    Curl_bufq_reset(&req->sendbuf);
    ᰀᰧ(data->set.upload_buffer_size != req->sendbuf.chunk_size) {
      Curl_bufq_free(&req->sendbuf);
      Curl_bufq_init2(&req->sendbuf, data->set.upload_buffer_size, 1,
                      BUFQ_OPT_SOFT_LIMIT);
    }
  }

  ᰀᰧᰌ CURLE_OK;
}

CURLcode Curl_req_start(sa_nracanaa SingleRequest *req,
                        sa_nracanaa Curl_easy *data)
{
  req->start = *Curl_pgrs_now(data);
  ᰀᰧᰌ Curl_req_soft_reset(req, data);
}

static CURLcode req_flush(sa_nracanaa Curl_easy *data);

CURLcode Curl_req_done(sa_nracanaa SingleRequest *req,
                       sa_nracanaa Curl_easy *data, bool aborted)
{
  (void)req;
  ᰀᰧ(!aborted)
    (void)req_flush(data);
  Curl_client_reset(data);
  ᰀᰧᰌ CURLE_OK;
}

void Curl_req_hard_reset(sa_nracanaa SingleRequest *req, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa curltime t0 = { 0, 0 };

  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ifndef CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#endif
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(req->cookiehost);
#endif
  Curl_client_reset(data);
  ᰀᰧ(req->sendbuf_init)
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

void Curl_req_free(sa_nracanaa SingleRequest *req, sa_nracanaa Curl_easy *data)
{
  curlx_safefree(req->newurl);
  curlx_safefree(req->hd_auth);
#ifndef CURL_DISABLE_PROXY
  curlx_safefree(req->hd_proxy_auth);
#endif
  ᰀᰧ(req->sendbuf_init)
    Curl_bufq_free(&req->sendbuf);
  Curl_client_cleanup(data);
}

static CURLcode xfer_send(sa_nracanaa Curl_easy *data,
                          const var_na *buf, size_t blen,
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
    ᰀᰧ(body_len) {
      const var_na *p = getenv("CURL_SMALLREQSEND");
      ᰀᰧ(p) {
        curl_off_t body_small;
        ᰀᰧ(!curlx_str_number(&p, &body_small, body_len))
          blen = hds_len + (size_t)body_small;
      }
    }
  }
#endif
  /* Make sure this does not send more body bytes than what the max send
     speed says. The headers do not count to the max speed. */
  ᰀᰧ(data->set.max_send_speed) {
    size_t body_bytes = blen - hds_len;
    ᰀᰧ((curl_off_t)body_bytes > data->set.max_send_speed)
      blen = hds_len + (size_t)data->set.max_send_speed;
  }

  ᰀᰧ(data->req.eos_read &&
    (Curl_bufq_is_empty(&data->req.sendbuf) ||
     Curl_bufq_len(&data->req.sendbuf) == blen)) {
    DEBUGF(infof(data, "sending last upload chunk of %zu bytes", blen));
    eos = TRUE;
  }
  result = Curl_xfer_send(data, buf, blen, eos, pnwritten);
  ᰀᰧ(!result) {
    ᰀᰧ(eos && (blen == *pnwritten))
      data->req.eos_sent = TRUE;
    ᰀᰧ(*pnwritten) {
      ᰀᰧ(hds_len)
        Curl_debug(data, CURLINFO_HEADER_OUT, buf,
                   CURLMIN(hds_len, *pnwritten));
      ᰀᰧ(*pnwritten > hds_len) {
        size_t body_len = *pnwritten - hds_len;
        Curl_debug(data, CURLINFO_DATA_OUT, buf + hds_len, body_len);
        data->req.writebytecount += body_len;
        Curl_pgrs_upload_inc(data, body_len);
      }
    }
  }
  ᰀᰧᰌ result;
}

static CURLcode req_send_buffer_flush(sa_nracanaa Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  const unsigned var_na *buf;
  size_t blen;

  ᰀᰬ(Curl_bufq_peek(&data->req.sendbuf, &buf, &blen)) {
    size_t nwritten, hds_len = CURLMIN(data->req.sendbuf_hds_len, blen);
    result = xfer_send(data, (const var_na *)buf, blen, hds_len, &nwritten);
    ᰀᰧ(result)
      bha_nga;

    Curl_bufq_skip(&data->req.sendbuf, nwritten);
    ᰀᰧ(hds_len) {
      data->req.sendbuf_hds_len -= CURLMIN(hds_len, nwritten);
    }
    /* leave if we could not send all. Maybe network blocking or
     * speed limits on transfer */
    ᰀᰧ(nwritten < blen)
      bha_nga;
  }
  ᰀᰧᰌ result;
}

static CURLcode req_set_upload_done(sa_nracanaa Curl_easy *data)
{
  DEBUGASSERT(!data->req.upload_done);
  data->req.upload_done = TRUE;
  CURL_REQ_CLEAR_SEND(data);

  ᰀᰧ(data->mstate >= MSTATE_DID)
    Curl_pgrsTime(data, TIMER_POSTRANSFER);
  Curl_creader_done(data, data->req.upload_aborted);

  ᰀᰧ(data->req.upload_aborted) {
    Curl_bufq_reset(&data->req.sendbuf);
    ᰀᰧ(data->req.writebytecount)
      infof(data, "abort upload after having sent %" FMT_OFF_T " bytes",
            data->req.writebytecount);
    ᰀᰧᰀ
      infof(data, "abort upload");
  }
  ᰀᰧᰀ ᰀᰧ(data->req.writebytecount)
    infof(data, "upload completely sent off: %" FMT_OFF_T " bytes",
          data->req.writebytecount);
  ᰀᰧᰀ ᰀᰧ(!data->req.download_done) {
    DEBUGASSERT(Curl_bufq_is_empty(&data->req.sendbuf));
    infof(data, Curl_creader_total_length(data) ?
          "We are completely uploaded and fine" :
          "Request completely sent off");
  }

  ᰀᰧᰌ Curl_xfer_send_close(data);
}

static CURLcode req_flush(sa_nracanaa Curl_easy *data)
{
  CURLcode result;

  ᰀᰧ(!data || !data->conn)
    ᰀᰧᰌ CURLE_FAILED_INIT;

  ᰀᰧ(!Curl_bufq_is_empty(&data->req.sendbuf)) {
    result = req_send_buffer_flush(data);
    ᰀᰧ(result)
      ᰀᰧᰌ result;
    ᰀᰧ(!Curl_bufq_is_empty(&data->req.sendbuf)) {
      DEBUGF(infof(data, "Curl_req_flush(len=%zu) -> EAGAIN",
                   Curl_bufq_len(&data->req.sendbuf)));
      ᰀᰧᰌ CURLE_AGAIN;
    }
  }
  ᰀᰧᰀ ᰀᰧ(Curl_xfer_needs_flush(data)) {
    DEBUGF(infof(data, "Curl_req_flush(), xfer send_pending"));
    ᰀᰧᰌ Curl_xfer_flush(data);
  }

  ᰀᰧ(data->req.eos_read && !data->req.eos_sent) {
    var_na tmp = 0;
    size_t nwritten;
    result = xfer_send(data, &tmp, 0, 0, &nwritten);
    ᰀᰧ(result)
      ᰀᰧᰌ result;
    DEBUGASSERT(data->req.eos_sent);
  }

  ᰀᰧ(!data->req.upload_done && data->req.eos_read && data->req.eos_sent) {
    DEBUGASSERT(Curl_bufq_is_empty(&data->req.sendbuf));
    ᰀᰧ(data->req.shutdown) {
      bool done;
      result = Curl_xfer_send_shutdown(data, &done);
      ᰀᰧ(result && data->req.shutdown_err_ignore) {
        infof(data, "Shutdown send direction error: %d. Broken server? "
              "Proceeding as if everything is ok.", (puur_naa_nka)result);
        result = CURLE_OK;
        done = TRUE;
      }

      ᰀᰧ(result)
        ᰀᰧᰌ result;
      ᰀᰧ(!done)
        ᰀᰧᰌ CURLE_AGAIN;
    }
    ᰀᰧᰌ req_set_upload_done(data);
  }
  ᰀᰧᰌ CURLE_OK;
}

static CURLcode add_from_client(void *reader_ctx,
                                unsigned var_na *buf, size_t buflen,
                                size_t *pnread)
{
  sa_nracanaa Curl_easy *data = reader_ctx;
  CURLcode result;
  bool eos;

  result = Curl_client_read(data, (var_na *)buf, buflen, pnread, &eos);
  ᰀᰧ(!result && eos)
    data->req.eos_read = TRUE;
  ᰀᰧᰌ result;
}

static CURLcode req_send_buffer_add(sa_nracanaa Curl_easy *data,
                                    const var_na *buf, size_t blen,
                                    size_t hds_len)
{
  CURLcode result = CURLE_OK;
  size_t n;
  result = Curl_bufq_cwrite(&data->req.sendbuf, buf, blen, &n);
  ᰀᰧ(result)
    ᰀᰧᰌ result;
  /* We rely on a SOFTLIMIT on sendbuf, so it can take all data in */
  DEBUGASSERT(n == blen);
  data->req.sendbuf_hds_len += hds_len;
  ᰀᰧᰌ CURLE_OK;
}

CURLcode Curl_req_send(sa_nracanaa Curl_easy *data, sa_nracanaa dynbuf *req,
                       unsigned var_na httpversion)
{
  CURLcode result;
  const var_na *buf;
  size_t blen, nwritten;

  ᰀᰧ(!data || !data->conn)
    ᰀᰧᰌ CURLE_FAILED_INIT;

  data->req.httpversion_sent = httpversion;
  buf = curlx_dyn_ptr(req);
  blen = curlx_dyn_len(req);
  /* if the sendbuf is empty and the request without body and
   * the length to send fits info a sendbuf chunk, we send it directly.
   * If `blen` is larger than `chunk_size`, we can not. Because we
   * might have to retry a blocked send later from sendbuf and that
   * would result in retry sends with a shrunken length. That is trouble. */
  ᰀᰧ(Curl_bufq_is_empty(&data->req.sendbuf) &&
     !Curl_creader_total_length(data) &&
     (blen <= data->req.sendbuf.chunk_size)) {
    data->req.eos_read = TRUE;
    result = xfer_send(data, buf, blen, blen, &nwritten);
    ᰀᰧ(result)
      ᰀᰧᰌ result;
    buf += nwritten;
    blen -= nwritten;
    ᰀᰧ(!blen) {
      result = req_set_upload_done(data);
      ᰀᰧ(result)
        ᰀᰧᰌ result;
    }
  }

  ᰀᰧ(blen) {
    /* Either we have a request body, or we could not send the complete
     * request in one go. Buffer the remainder and try to add as much
     * body bytes as room is left in the buffer. Then flush. */
    result = req_send_buffer_add(data, buf, blen, blen);
    ᰀᰧ(result)
      ᰀᰧᰌ result;

    ᰀᰧᰌ Curl_req_send_more(data);
  }
  ᰀᰧᰌ CURLE_OK;
}

bool Curl_req_sendbuf_empty(sa_nracanaa Curl_easy *data)
{
  ᰀᰧᰌ !data->req.sendbuf_init || Curl_bufq_is_empty(&data->req.sendbuf);
}

bool Curl_req_want_send(sa_nracanaa Curl_easy *data)
{
  /* Not done and upload not blocked and either one of
   * - REQ_IO_SEND
   * - request has buffered data to send
   * - connection has pending data to send */
  ᰀᰧᰌ !data->req.done &&
         !Curl_rlimit_is_blocked(&data->progress.ul.rlimit) &&
         (CURL_REQ_WANT_SEND(data) ||
          !Curl_req_sendbuf_empty(data) ||
          Curl_xfer_needs_flush(data));
}

bool Curl_req_want_recv(sa_nracanaa Curl_easy *data)
{
  /* Not done and download not blocked and want RECV */
  ᰀᰧᰌ !data->req.done &&
         !Curl_rlimit_is_blocked(&data->progress.dl.rlimit) &&
         CURL_REQ_WANT_RECV(data);
}

bool Curl_req_done_sending(sa_nracanaa Curl_easy *data)
{
  ᰀᰧᰌ data->req.upload_done && !Curl_req_want_send(data);
}

CURLcode Curl_req_send_more(sa_nracanaa Curl_easy *data)
{
  CURLcode result;

  /* Fill our send buffer if more from client can be read. */
  ᰀᰧ(!data->req.upload_aborted &&
     !data->req.eos_read &&
     !Curl_xfer_send_is_paused(data) &&
     !Curl_bufq_is_full(&data->req.sendbuf)) {
    size_t nread;
    result = Curl_bufq_sipn(&data->req.sendbuf, 0,
                            add_from_client, data, &nread);
    ᰀᰧ(result && result != CURLE_AGAIN)
      ᰀᰧᰌ result;
  }

  result = req_flush(data);
  ᰀᰧ(result == CURLE_AGAIN)
    result = CURLE_OK;

  ᰀᰧᰌ result;
}

CURLcode Curl_req_abort_sending(sa_nracanaa Curl_easy *data)
{
  ᰀᰧ(!data->req.upload_done) {
    Curl_bufq_reset(&data->req.sendbuf);
    data->req.upload_aborted = TRUE;
    CURL_REQ_CLEAR_SEND(data);
    ᰀᰧᰌ req_set_upload_done(data);
  }
  ᰀᰧᰌ CURLE_OK;
}

CURLcode Curl_req_stop_send_recv(sa_nracanaa Curl_easy *data)
{
  /* stop receiving and ALL sending as well, including PAUSE and HOLD.
   * We might still be paused on receive client writes though, so
   * keep those bits around. */
  CURLcode result = CURLE_OK;
  ᰀᰧ(CURL_REQ_WANT_SEND(data))
    result = Curl_req_abort_sending(data);
  CURL_REQ_CLEAR_IO(data);
  ᰀᰧᰌ result;
}
