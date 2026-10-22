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
#ಅನ್ತರ್ಭಾವ "strerror.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

CURLcode Curl_cf_def_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ಯದಿ(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ಯದಿ(result || !*done)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
  *done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ easy_pollset *ps)
{
  /* NOP */
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)ps;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

bool Curl_cf_def_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                          ಧ್ರುವ uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  ಯದಿ(cf->next)
    ಪ್ರತ್ಯಾಗಮ cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                          ವರ್ಣ *buf, size_t len, size_t *pnread)
{
  ಯದಿ(cf->next)
    ಪ್ರತ್ಯಾಗಮ cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               bool *ಒಳಹರಿವು_pending)
{
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->is_alive(cf->next, data, ಒಳಹರಿವು_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(ಸಂರಚನಾ Curl_cfilter *cf,
                           ಸಂರಚನಾ Curl_easy *data,
                           ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
ಶೂನ್ಯ Curl_conn_trc_filters(ಸಂರಚನಾ Curl_easy *data,
                           int8_t sockindex, ಧ್ರುವ ವರ್ಣ *info)
{
  ಯದಿ(CURL_TRC_M_is_verbose(data) && data->conn) {
    ಸಂರಚನಾ Curl_cfilter *cf = data->conn->cfilter[sockindex];

    ಯದಿ(cf) {
      ವರ್ಣ ಸಂದೇಶ[256], *buf;
      ಪೂರ್ಣಾಙ್ಕ blen, n;

      buf = ಸಂದೇಶ;
      blen = ಪರಿಮಾಣ(ಸಂದೇಶ) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      ಪ್ರತಿ(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", ಸಂದೇಶ, blen ? "" : "...");
    }
    ಅನ್ಯಥಾ
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

ಶೂನ್ಯ Curl_conn_cf_discard_chain(ಸಂರಚನಾ Curl_cfilter **pcf,
                                ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cfilter *cfn, *cf = *pcf;

  ಯದಿ(cf) {
    *pcf = ರಿಕ್ತ;
    ಯಾವತ್(cf) {
      cfn = cf->next;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->next = ರಿಕ್ತ;
      cf->cft->destroy(cf, data);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

ಶೂನ್ಯ Curl_conn_cf_discard_all(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], data);
}

CURLcode Curl_cf_recv(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex, ವರ್ಣ *buf,
                      size_t len, size_t *pnread)
{
  ಸಂರಚನಾ Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  ಯಾವತ್(cf && !cf->connected)
    cf = cf->next;
  ಯದಿ(cf)
    ಪ್ರತ್ಯಾಗಮ cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                      ಧ್ರುವ uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  ಸಂರಚನಾ Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  ಯಾವತ್(cf && !cf->connected)
    cf = cf->next;
  ಯದಿ(cf) {
    ಪ್ರತ್ಯಾಗಮ cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
}

ಸಂರಚನಾ cf_io_ctx {
  ಸಂರಚನಾ Curl_easy *data;
  ಸಂರಚನಾ Curl_cfilter *cf;
};

ಸ್ಥಿರ CURLcode cf_bufq_reader(ಶೂನ್ಯ *writer_ctx,
                               unsigned ವರ್ಣ *buf, size_t blen,
                               size_t *pnread)
{
  ಸಂರಚನಾ cf_io_ctx *io = writer_ctx;
  ಪ್ರತ್ಯಾಗಮ Curl_conn_cf_recv(io->cf, io->data, (ವರ್ಣ *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(ಸಂರಚನಾ Curl_cfilter *cf,
                           ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  ಸಂರಚನಾ cf_io_ctx io;

  ಯದಿ(!cf || !data) {
    *pnread = 0;
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  ಪ್ರತ್ಯಾಗಮ Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

ಸ್ಥಿರ CURLcode cf_bufq_writer(ಶೂನ್ಯ *writer_ctx,
                               ಧ್ರುವ uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  ಸಂರಚನಾ cf_io_ctx *io = writer_ctx;
  ಪ್ರತ್ಯಾಗಮ Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(ಸಂರಚನಾ Curl_cfilter *cf,
                           ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ bufq *bufq,
                           ಧ್ರುವ unsigned ವರ್ಣ *buf, size_t blen,
                           size_t *pnwritten)
{
  ಸಂರಚನಾ cf_io_ctx io;

  ಯದಿ(!cf || !data) {
    *pnwritten = 0;
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  ಯದಿ(buf && blen)
    ಪ್ರತ್ಯಾಗಮ Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                        ಧ್ರುವ ಸಂರಚನಾ Curl_cftype *cft,
                        ಶೂನ್ಯ *ctx)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, ಪರಿಮಾಣ(*cf));
  ಯದಿ(!cf)
    ಗಚ್ಛ out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_conn_cf_add(ಸಂರಚನಾ Curl_easy *data,
                      ಸಂರಚನಾ connectdata *conn,
                      int8_t sockindex,
                      ಸಂರಚನಾ Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->next);

  cf->next = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(data, cf, "added");
}

ಶೂನ್ಯ Curl_conn_cf_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                               ಸಂರಚನಾ Curl_cfilter *cf_new)
{
  ಸಂರಚನಾ Curl_cfilter *tail, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  tail = cf_at->next;
  cf_at->next = cf_new;
  ಕುರು {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->next;
    cf_new = cf_new->next;
  } ಯಾವತ್(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(ಸಂರಚನಾ Curl_cfilter **pcf,
                          ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = pcf ? *pcf : ರಿಕ್ತ;
  bool found = FALSE;
  ಯದಿ(cf) {
    ಯದಿ(cf->conn) {
      /* unlink if present in connection filter chain */
      ಸಂರಚನಾ Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      ಯಾವತ್(*pprev) {
        ಯದಿ(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = ರಿಕ್ತ;
          found = TRUE;
          ಭಙ್ಗ;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  ಪ್ರತ್ಯಾಗಮ found;
}

CURLcode Curl_conn_cf_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              bool *done)
{
  ಯದಿ(cf)
    ಪ್ರತ್ಯಾಗಮ cf->cft->do_connect(cf, data, done);
  ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                           ಧ್ರುವ uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ಯದಿ(cf)
    ಪ್ರತ್ಯಾಗಮ cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                           ವರ್ಣ *buf, size_t len, size_t *pnread)
{
  ಯದಿ(cf)
    ಪ್ರತ್ಯಾಗಮ cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
}

ಸ್ಥಿರ CURLcode cf_cntrl_all(ಸಂರಚನಾ connectdata *conn,
                             ಸಂರಚನಾ Curl_easy *data,
                             bool ignore_result,
                             ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ i;

  ಪ್ರತಿ(i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    ಯದಿ(!ignore_result && result)
      ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

bool Curl_conn_cf_is_ip_connected(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(cf) {
    ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ = 0;
    ಯದಿ(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &ಮೌಲ್ಯ, ರಿಕ್ತ))
      ಪ್ರತ್ಯಾಗಮ !!ಮೌಲ್ಯ;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಶೂನ್ಯ Curl_conn_cntrl_update_info(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, ರಿಕ್ತ);
}

ಶೂನ್ಯ Curl_conn_cntrl_report_stats(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn,
                                  ಪೂರ್ಣಾಙ್ಕ sockindex)
{
  ಯದಿ((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (ಶೂನ್ಯ)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, ರಿಕ್ತ);
}

ಶೂನ್ಯ Curl_conn_remove_setup_filters(ಸಂರಚನಾ Curl_easy *data,
                                    int8_t sockindex)
{
  ಸಂರಚನಾ Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  ಯಾವತ್(*anchor) {
    ಸಂರಚನಾ Curl_cfilter *cf = *anchor;
    ಯದಿ(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = ರಿಕ್ತ;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    ಅನ್ಯಥಾ
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(ಸಂರಚನಾ connectdata *conn, int8_t sockindex)
{
  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(ಸಂರಚನಾ connectdata *conn, int8_t sockindex)
{
  ಸಂರಚನಾ Curl_cfilter *cf;

  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  cf = conn->cfilter[sockindex];
  ಯದಿ(cf)
    ಪ್ರತ್ಯಾಗಮ (bool)cf->connected;
  ಅನ್ಯಥಾ ಯದಿ(conn->scheme->flags & PROTOPT_NONETWORK)
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_conn_is_ip_connected(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex)
{
  ಸಂರಚನಾ Curl_cfilter *cf;

  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  cf = data->conn->cfilter[sockindex];
  ಯಾವತ್(cf) {
    ಯದಿ(cf->connected)
      ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ಪ್ರತ್ಯಾಗಮ FALSE;
    cf = cf->next;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ifndef CURL_DISABLE_PROXY
ಸ್ಥಿರ bool cf_is_tunneling(ಸಂರಚನಾ Curl_cfilter *cf)
{
  ಪ್ರತಿ(; cf; cf = cf->next) {
    ಯದಿ((cf->cft->flags & CF_TYPE_PROXY))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_conn_is_tunneling(ಸಂರಚನಾ connectdata *conn, int8_t sockindex)
{
  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#ಅನ್ಯಥಾ
bool Curl_conn_is_tunneling(ಸಂರಚನಾ connectdata *conn, int8_t sockindex)
{
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)sockindex;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif /* CURL_DISABLE_PROXY */

ಸ್ಥಿರ bool cf_is_ssl(ಸಂರಚನಾ Curl_cfilter *cf)
{
  ಪ್ರತಿ(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    ಯದಿ((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_conn_is_ssl(ಸಂರಚನಾ connectdata *conn, int8_t sockindex)
{
  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ connectdata *conn, int8_t sockindex,
                            ಪೂರ್ಣಾಙ್ಕ query,
                            ಸಂರಚನಾ curl_tlssessioninfo *info)
{
  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(Curl_conn_is_ssl(conn, sockindex)) {
    ಸಂರಚನಾ Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, ರಿಕ್ತ, (ಶೂನ್ಯ *)info) :
      CURLE_UNKNOWN_OPTION;
    ಪ್ರತ್ಯಾಗಮ !result;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

CURLcode Curl_conn_get_ip_info(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, ಸಂರಚನಾ ip_quadruple *ipquad)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(ಸಂರಚನಾ connectdata *conn, int8_t sockindex)
{
  ಸಂರಚನಾ Curl_cfilter *cf;

  ಯದಿ(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತಿ(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    ಯದಿ(cf->cft->flags & CF_TYPE_MULTIPLEX)
      ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

unsigned ವರ್ಣ Curl_conn_get_transport(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ಪ್ರತ್ಯಾಗಮ Curl_conn_cf_get_transport(cf, data);
}

ಪೂರ್ಣಾಙ್ಕ Curl_socktype_for_transport(uint8_t transport)
{
  ವಿಕಲ್ಪ(transport) {
  ಪಕ್ಷ TRNSPRT_TCP:
    ಪ್ರತ್ಯಾಗಮ SOCK_STREAM;
  ಪಕ್ಷ TRNSPRT_UNIX:
    ಪ್ರತ್ಯಾಗಮ SOCK_STREAM;
  ಸ್ವಭಾವ: /* UDP and QUIC */
    ಪ್ರತ್ಯಾಗಮ SOCK_DGRAM;
  }
}

ಪೂರ್ಣಾಙ್ಕ Curl_protocol_for_transport(uint8_t transport)
{
  ವಿಕಲ್ಪ(transport) {
  ಪಕ್ಷ TRNSPRT_TCP:
    ಪ್ರತ್ಯಾಗಮ IPPROTO_TCP;
  ಪಕ್ಷ TRNSPRT_UNIX:
    ಪ್ರತ್ಯಾಗಮ IPPROTO_IP;
  ಸ್ವಭಾವ: /* UDP and QUIC */
    ಪ್ರತ್ಯಾಗಮ IPPROTO_UDP;
  }
}

ಧ್ರುವ ವರ್ಣ *Curl_conn_get_alpn_negotiated(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ಪ್ರತ್ಯಾಗಮ Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned ವರ್ಣ Curl_conn_http_version(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned ವರ್ಣ v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  ಪ್ರತಿ(; cf; cf = cf->next) {
    ಯದಿ(cf->cft->flags & CF_TYPE_HTTP) {
      ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &ಮೌಲ್ಯ, ರಿಕ್ತ);
      ಯದಿ(!result && ((ಮೌಲ್ಯ < 0) || (ಮೌಲ್ಯ > 255)))
        result = CURLE_FAILED_INIT;
      ಅನ್ಯಥಾ
        v = (unsigned ವರ್ಣ)ಮೌಲ್ಯ;
      ಭಙ್ಗ;
    }
    ಯದಿ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ (unsigned ವರ್ಣ)(result ? 0 : v);
}

bool Curl_conn_data_pending(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex)
{
  ಸಂರಚನಾ Curl_cfilter *cf;

  (ಶೂನ್ಯ)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  cf = data->conn->cfilter[sockindex];
  ಯಾವತ್(cf && !cf->connected) {
    cf = cf->next;
  }
  ಯದಿ(cf) {
    ಪ್ರತ್ಯಾಗಮ cf->cft->has_data_pending(cf, data);
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_conn_cf_needs_flush(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, ರಿಕ್ತ) : CURLE_UNKNOWN_OPTION;
  ಪ್ರತ್ಯಾಗಮ (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex)
{
  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
}

CURLcode Curl_conn_cf_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  ಪ್ರತಿ(; cf && !result; cf = cf->next) {
    ಯದಿ(cf->shutdown)
      ಅನುವರ್ತ;
    ಯದಿ(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_conn_adjust_pollset(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn,
                                  ಸಂರಚನಾ easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ bool want_io = !!ps->n;
  int8_t i;

  DEBUGASSERT(data);
  DEBUGASSERT(conn);
  /* During connect time, connection filters may add sockets to the pollset
   * even when the transfer neither wants to send nor receive. And those
   * sockets, when having events, are served. The same applies to a
   * filter chain whose shutdown has started.
   * Once a filter chain is connected however and before its shutdown
   * starts, a transfer that neither wants to send nor receive
   * will never call the connection filters. Any sockets added by the filters
   * will not change state and POLLIN/POLLOUT events will trigger forever,
   * making us busy loop. See #21671.
   * Gate each filter chain on its own state, so that one chain being in
   * connect or shutdown does not add poll events for the other. Check
   * against the transfer's own interest, before any chain added sockets
   * of its own. */
  ಪ್ರತಿ(i = 0; (i < (ಪೂರ್ಣಾಙ್ಕ)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    ಯದಿ(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
ಪೂರ್ಣಾಙ್ಕ Curl_conn_cf_poll(ಸಂರಚನಾ Curl_cfilter *cf,
                      ಸಂರಚನಾ Curl_easy *data,
                      timediff_t timeout_ms)
{
  ಸಂರಚನಾ easy_pollset ps;
  ಪೂರ್ಣಾಙ್ಕ rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  ಯದಿ(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  ಅನ್ಯಥಾ
    rc = -1;
  Curl_pollset_cleanup(&ps);
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಶೂನ್ಯ Curl_conn_get_current_host(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                                ಧ್ರುವ ವರ್ಣ **phost, ಪೂರ್ಣಾಙ್ಕ *pport)
{
  ಸಂರಚನಾ Curl_cfilter *cf, *cf_proxy = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ portarg = -1;

  ಯದಿ(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    ಯದಿ(pport)
      *pport = -1;
    ಪ್ರತ್ಯಾಗಮ;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : ರಿಕ್ತ;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  ಯಾವತ್(cf && !cf->connected) {
    ಯದಿ((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  ಯದಿ(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  ಯದಿ(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                           ಸಂರಚನಾ Curl_easy *data,
                           ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)event;
  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            bool ignore_result,
                            ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  CURLcode result = CURLE_OK;

  ಪ್ರತಿ(; cf; cf = cf->next) {
    ಯದಿ(cf->cft->cntrl == Curl_cf_def_cntrl)
      ಅನುವರ್ತ;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    ಯದಿ(!ignore_result && result)
      ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

curl_socket_t Curl_conn_cf_get_socket(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  curl_socket_t sock;
  ಯದಿ(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, ರಿಕ್ತ, &sock))
    ಪ್ರತ್ಯಾಗಮ sock;
  ಪ್ರತ್ಯಾಗಮ CURL_SOCKET_BAD;
}

unsigned ವರ್ಣ Curl_conn_cf_get_transport(ಸಂರಚನಾ Curl_cfilter *cf,
                                         ಸಂರಚನಾ Curl_easy *data)
{
  ಪೂರ್ಣಾಙ್ಕ transport = 0;
  ಯದಿ(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, ರಿಕ್ತ))
    ಪ್ರತ್ಯಾಗಮ (unsigned ವರ್ಣ)transport;
  ಪ್ರತ್ಯಾಗಮ (unsigned ವರ್ಣ)(data->conn ? data->conn->transport_wanted : 0);
}

ಧ್ರುವ ವರ್ಣ *Curl_conn_cf_get_alpn_negotiated(ಸಂರಚನಾ Curl_cfilter *cf,
                                             ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ ವರ್ಣ *alpn = ರಿಕ್ತ;
  CURL_TRC_CF(data, cf, "query ALPN");
  ಯದಿ(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, ರಿಕ್ತ,
                           CURL_UNCONST(&alpn)))
    ಪ್ರತ್ಯಾಗಮ alpn;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_sockaddr_ex *cf_get_remote_addr(
  ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ ಸಂರಚನಾ Curl_sockaddr_ex *remote_addr = ರಿಕ್ತ;
  ಯದಿ(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, ರಿಕ್ತ,
                     CURL_UNCONST(&remote_addr)))
    ಪ್ರತ್ಯಾಗಮ remote_addr;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

CURLcode Curl_conn_cf_get_ip_info(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  bool *is_ipv6, ಸಂರಚನಾ ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  ಯದಿ(cf) {
    ಪೂರ್ಣಾಙ್ಕ ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

curl_socket_t Curl_conn_get_first_socket(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cfilter *cf;

  ಯದಿ(!data->conn)
    ಪ್ರತ್ಯಾಗಮ CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  ಯದಿ(cf && !cf->connected)
    ಪ್ರತ್ಯಾಗಮ Curl_conn_cf_get_socket(cf, data);
  ಪ್ರತ್ಯಾಗಮ data->conn->sock[FIRSTSOCKET];
}

ಧ್ರುವ ಸಂರಚನಾ Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  ಸಂರಚನಾ Curl_easy *data, int8_t sockindex)
{
  ಸಂರಚನಾ Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ cf ? cf_get_remote_addr(cf, data) : ರಿಕ್ತ;
}

CURLcode Curl_conn_ev_data_setup(ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, ರಿಕ್ತ);
}

CURLcode Curl_conn_flush(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex)
{
  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಪ್ರತ್ಯಾಗಮ Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
                            CF_CTRL_FLUSH, 0, ರಿಕ್ತ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
ಶೂನ್ಯ Curl_conn_ev_data_done_send(ಸಂರಚನಾ Curl_easy *data)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE_SEND, 0, ರಿಕ್ತ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
ಶೂನ್ಯ Curl_conn_ev_data_done(ಸಂರಚನಾ Curl_easy *data, bool premature)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE, premature, ರಿಕ್ತ);
}

CURLcode Curl_conn_ev_data_pause(ಸಂರಚನಾ Curl_easy *data, bool do_pause)
{
  ಪ್ರತ್ಯಾಗಮ cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, ರಿಕ್ತ);
}

bool Curl_conn_is_alive(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ connectdata *conn,
                        bool *ಒಳಹರಿವು_pending)
{
  ಸಂರಚನಾ Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ಪ್ರತ್ಯಾಗಮ cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, ಒಳಹರಿವು_pending);
}

CURLcode Curl_conn_keep_alive(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ i;

  ಪ್ರತಿ(i = 0; (i < (ಪೂರ್ಣಾಙ್ಕ)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    ಸಂರಚನಾ Curl_cfilter *cf = conn->cfilter[i];
    ಯದಿ(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

size_t Curl_conn_get_max_concurrent(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ connectdata *conn,
                                    int8_t sockindex)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ n = -1;

  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, ರಿಕ್ತ) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  ಪ್ರತ್ಯಾಗಮ (result || n < 0) ? 1 : (size_t)n;
}

ಪೂರ್ಣಾಙ್ಕ Curl_conn_get_stream_error(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ connectdata *conn,
                               int8_t sockindex)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ n = 0;

  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, ರಿಕ್ತ) : CURLE_UNKNOWN_OPTION;
  ಪ್ರತ್ಯಾಗಮ (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(ಸಂರಚನಾ Curl_easy *data, curl_socket_t sockfd)
{
  ಯದಿ(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    ಪ್ರತ್ಯಾಗಮ SECONDARYSOCKET;
  ಪ್ರತ್ಯಾಗಮ FIRSTSOCKET;
}

CURLcode Curl_conn_recv(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                        ವರ್ಣ *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಯದಿ(data && data->conn && data->conn->recv[sockindex])
    ಪ್ರತ್ಯಾಗಮ data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                        ಧ್ರುವ ಶೂನ್ಯ *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  ಯದಿ(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ಧ್ರುವ ವರ್ಣ *p = getenv("CURL_SMALLSENDS");
    ಯದಿ(p) {
      curl_off_t altsize;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        ಯದಿ(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  ಯದಿ(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    ಯದಿ(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ result;
  }
  *pnwritten = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
}
