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
#ਅਨ੍ਤਰ੍ਭਾਵ "strerror.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

CURLcode Curl_cf_def_connect(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(cf->ਅਗਲਾ) {
    ਨਤੀਜਾ = cf->ਅਗਲਾ->cft->do_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  cf->connected = TRUE;
  *ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  *ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ easy_pollset *ps)
{
  /* NOP */
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)ps;
  ਵਾਪਸ CURLE_OK;
}

bool Curl_cf_def_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                              ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->has_ਅੰਕੜੇ_pending(cf->ਅਗਲਾ, ਅੰਕੜੇ) : FALSE;
}

CURLcode Curl_cf_def_send(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  ਜੇ(cf->ਅਗਲਾ)
    ਵਾਪਸ cf->ਅਗਲਾ->cft->do_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ਵਾਪਸ CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅੱਖਰ *buf, size_t len, size_t *pnread)
{
  ਜੇ(cf->ਅਗਲਾ)
    ਵਾਪਸ cf->ਅਗਲਾ->cft->do_recv(cf->ਅਗਲਾ, ਅੰਕੜੇ, buf, len, pnread);
  *pnread = 0;
  ਵਾਪਸ CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               bool *ਨਿਵੇਸ਼_pending)
{
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->is_alive(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਵੇਸ਼_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->keep_alive(cf->ਅਗਲਾ, ਅੰਕੜੇ) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(ਬਣਤਰ Curl_cfilter *cf,
                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਖਾਲੀ Curl_conn_trc_filters(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           int8_t sockindex, ਅਚਲ ਅੱਖਰ *info)
{
  ਜੇ(CURL_TRC_M_is_verbose(ਅੰਕੜੇ) && ਅੰਕੜੇ->conn) {
    ਬਣਤਰ Curl_cfilter *cf = ਅੰਕੜੇ->conn->cfilter[sockindex];

    ਜੇ(cf) {
      ਅੱਖਰ ਸੁਨੇਹਾ[256], *buf;
      ਪੂਰਨ blen, n;

      buf = ਸੁਨੇਹਾ;
      blen = ਆਕਾਰ(ਸੁਨੇਹਾ) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      ਲਈ(; cf && blen; cf = cf->ਅਗਲਾ) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(ਅੰਕੜੇ, "%s%s", ਸੁਨੇਹਾ, blen ? "" : "...");
    }
    ਨਹੀਂਤਾਂ
      CURL_TRC_M(ਅੰਕੜੇ, "%s [%d][-]", info, sockindex);
  }
}
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

ਖਾਲੀ Curl_conn_cf_discard_chain(ਬਣਤਰ Curl_cfilter **pcf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cfn, *cf = *pcf;

  ਜੇ(cf) {
    *pcf = ਨਿਰਾ;
    ਜਦੋਂ(cf) {
      cfn = cf->ਅਗਲਾ;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->ਅਗਲਾ = ਨਿਰਾ;
      cf->cft->destroy(cf, ਅੰਕੜੇ);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

ਖਾਲੀ Curl_conn_cf_discard_all(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn, int8_t sockindex)
{
  conn->shutdown.ਸ਼ੁਰੂ_ms[sockindex] = -1;
  conn->connect_started.ਸੈੱਟ[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], ਅੰਕੜੇ);
}

CURLcode Curl_cf_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex, ਅੱਖਰ *buf,
                      size_t len, size_t *pnread)
{
  ਬਣਤਰ Curl_cfilter *cf;

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  cf = ਅੰਕੜੇ->conn->cfilter[sockindex];
  ਜਦੋਂ(cf && !cf->connected)
    cf = cf->ਅਗਲਾ;
  ਜੇ(cf)
    ਵਾਪਸ cf->cft->do_recv(cf, ਅੰਕੜੇ, buf, len, pnread);
  failf(ਅੰਕੜੇ, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  ਵਾਪਸ CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                      ਅਚਲ uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  ਬਣਤਰ Curl_cfilter *cf;

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  cf = ਅੰਕੜੇ->conn->cfilter[sockindex];
  ਜਦੋਂ(cf && !cf->connected)
    cf = cf->ਅਗਲਾ;
  ਜੇ(cf) {
    ਵਾਪਸ cf->cft->do_send(cf, ਅੰਕੜੇ, buf, len, eos, pnwritten);
  }
  failf(ਅੰਕੜੇ, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  ਵਾਪਸ CURLE_FAILED_INIT;
}

ਬਣਤਰ cf_io_ctx {
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ਬਣਤਰ Curl_cfilter *cf;
};

ਸਥਿਰ CURLcode cf_bufq_reader(ਖਾਲੀ *writer_ctx,
                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t blen,
                               size_t *pnread)
{
  ਬਣਤਰ cf_io_ctx *io = writer_ctx;
  ਵਾਪਸ Curl_conn_cf_recv(io->cf, io->ਅੰਕੜੇ, (ਅੱਖਰ *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(ਬਣਤਰ Curl_cfilter *cf,
                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  ਬਣਤਰ cf_io_ctx io;

  ਜੇ(!cf || !ਅੰਕੜੇ) {
    *pnread = 0;
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
  io.ਅੰਕੜੇ = ਅੰਕੜੇ;
  io.cf = cf;
  ਵਾਪਸ Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

ਸਥਿਰ CURLcode cf_bufq_writer(ਖਾਲੀ *writer_ctx,
                               ਅਚਲ uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  ਬਣਤਰ cf_io_ctx *io = writer_ctx;
  ਵਾਪਸ Curl_conn_cf_send(io->cf, io->ਅੰਕੜੇ, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(ਬਣਤਰ Curl_cfilter *cf,
                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ bufq *bufq,
                           ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t blen,
                           size_t *pnwritten)
{
  ਬਣਤਰ cf_io_ctx io;

  ਜੇ(!cf || !ਅੰਕੜੇ) {
    *pnwritten = 0;
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
  io.ਅੰਕੜੇ = ਅੰਕੜੇ;
  io.cf = cf;
  ਜੇ(buf && blen)
    ਵਾਪਸ Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(ਬਣਤਰ Curl_cfilter **pcf,
                        ਅਚਲ ਬਣਤਰ Curl_cftype *cft,
                        ਖਾਲੀ *ctx)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, ਆਕਾਰ(*cf));
  ਜੇ(!cf)
    ਜਾਓ ਬਾਹਰਲਾ;

  cf->cft = cft;
  cf->ctx = ctx;
  ਨਤੀਜਾ = CURLE_OK;
ਬਾਹਰਲਾ:
  *pcf = cf;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_conn_cf_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ connectdata *conn,
                      int8_t sockindex,
                      ਬਣਤਰ Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->ਅਗਲਾ);

  cf->ਅਗਲਾ = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "added");
}

ਖਾਲੀ Curl_conn_cf_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                               ਬਣਤਰ Curl_cfilter *cf_new)
{
  ਬਣਤਰ Curl_cfilter *ਪੂਛ, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  ਪੂਛ = cf_at->ਅਗਲਾ;
  cf_at->ਅਗਲਾ = cf_new;
  ਕਰੋ {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->ਅਗਲਾ;
    cf_new = cf_new->ਅਗਲਾ;
  } ਜਦੋਂ(cf_new);
  *pnext = ਪੂਛ;
}

bool Curl_conn_cf_discard(ਬਣਤਰ Curl_cfilter **pcf,
                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = pcf ? *pcf : ਨਿਰਾ;
  bool ਮਿਲਿਆ = FALSE;
  ਜੇ(cf) {
    ਜੇ(cf->conn) {
      /* unlink if present in connection filter chain */
      ਬਣਤਰ Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      ਜਦੋਂ(*pprev) {
        ਜੇ(*pprev == *pcf) {
          *pprev = (*pcf)->ਅਗਲਾ;
          cf->ਅਗਲਾ = ਨਿਰਾ;
          ਮਿਲਿਆ = TRUE;
          ਰੋਕੋ;
        }
        pprev = &(*pprev)->ਅਗਲਾ;
      }
    }
    Curl_conn_cf_discard_chain(pcf, ਅੰਕੜੇ);
  }
  ਵਾਪਸ ਮਿਲਿਆ;
}

CURLcode Curl_conn_cf_connect(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              bool *ਪੂਰਾ)
{
  ਜੇ(cf)
    ਵਾਪਸ cf->cft->do_connect(cf, ਅੰਕੜੇ, ਪੂਰਾ);
  ਵਾਪਸ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਅਚਲ uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ਜੇ(cf)
    ਵਾਪਸ cf->cft->do_send(cf, ਅੰਕੜੇ, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ਵਾਪਸ CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਅੱਖਰ *buf, size_t len, size_t *pnread)
{
  ਜੇ(cf)
    ਵਾਪਸ cf->cft->do_recv(cf, ਅੰਕੜੇ, buf, len, pnread);
  *pnread = 0;
  ਵਾਪਸ CURLE_RECV_ERROR;
}

ਸਥਿਰ CURLcode cf_cntrl_all(ਬਣਤਰ connectdata *conn,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             bool ignore_ਨਤੀਜਾ,
                             ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ i;

  ਲਈ(i = 0; i < (ਪੂਰਨ)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    ਨਤੀਜਾ = Curl_conn_cf_cntrl(conn->cfilter[i], ਅੰਕੜੇ, ignore_ਨਤੀਜਾ,
                                event, arg1, arg2);
    ਜੇ(!ignore_ਨਤੀਜਾ && ਨਤੀਜਾ)
      ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_conn_cf_is_ip_connected(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(cf) {
    ਪੂਰਨ ਮੁੱਲ = 0;
    ਜੇ(!cf->cft->query(cf, ਅੰਕੜੇ, CF_QUERY_REALLY_CONNECTED, &ਮੁੱਲ, ਨਿਰਾ))
      ਵਾਪਸ !!ਮੁੱਲ;
  }
  ਵਾਪਸ FALSE;
}

ਖਾਲੀ Curl_conn_cntrl_update_info(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *conn)
{
  cf_cntrl_all(conn, ਅੰਕੜੇ, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, ਨਿਰਾ);
}

ਖਾਲੀ Curl_conn_cntrl_report_stats(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn,
                                  ਪੂਰਨ sockindex)
{
  ਜੇ((ਬਿਨਾਨਿਸ਼ਾਨੀ)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (ਖਾਲੀ)Curl_conn_cf_cntrl(conn->cfilter[sockindex], ਅੰਕੜੇ, TRUE,
                             CF_CTRL_REPORT_STATS, 0, ਨਿਰਾ);
}

ਖਾਲੀ Curl_conn_remove_ਤਿਆਰੀ_filters(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    int8_t sockindex)
{
  ਬਣਤਰ Curl_cfilter **anchor = &ਅੰਕੜੇ->conn->cfilter[sockindex];
  ਜਦੋਂ(*anchor) {
    ਬਣਤਰ Curl_cfilter *cf = *anchor;
    ਜੇ(cf->connected && (cf->cft->flags & CF_TYPE_ਤਿਆਰੀ)) {
      *anchor = cf->ਅਗਲਾ;
      cf->ਅਗਲਾ = ਨਿਰਾ;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "removing connected setup filter");
      cf->cft->destroy(cf, ਅੰਕੜੇ);
      curlx_free(cf);
    }
    ਨਹੀਂਤਾਂ
      anchor = &cf->ਅਗਲਾ;
  }
}

bool Curl_conn_is_ਤਿਆਰੀ(ਬਣਤਰ connectdata *conn, int8_t sockindex)
{
  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ FALSE;
  ਵਾਪਸ !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(ਬਣਤਰ connectdata *conn, int8_t sockindex)
{
  ਬਣਤਰ Curl_cfilter *cf;

  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ FALSE;
  cf = conn->cfilter[sockindex];
  ਜੇ(cf)
    ਵਾਪਸ (bool)cf->connected;
  ਨਹੀਂਤਾਂ ਜੇ(conn->scheme->flags & PROTOPT_NONETWORK)
    ਵਾਪਸ TRUE;
  ਵਾਪਸ FALSE;
}

bool Curl_conn_is_ip_connected(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex)
{
  ਬਣਤਰ Curl_cfilter *cf;

  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ FALSE;
  cf = ਅੰਕੜੇ->conn->cfilter[sockindex];
  ਜਦੋਂ(cf) {
    ਜੇ(cf->connected)
      ਵਾਪਸ TRUE;
    ਜੇ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ਵਾਪਸ FALSE;
    cf = cf->ਅਗਲਾ;
  }
  ਵਾਪਸ FALSE;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
ਸਥਿਰ bool cf_is_tunneling(ਬਣਤਰ Curl_cfilter *cf)
{
  ਲਈ(; cf; cf = cf->ਅਗਲਾ) {
    ਜੇ((cf->cft->flags & CF_TYPE_PROXY))
      ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

bool Curl_conn_is_tunneling(ਬਣਤਰ connectdata *conn, int8_t sockindex)
{
  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ FALSE;
  ਵਾਪਸ conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#ਨਹੀਂਤਾਂ
bool Curl_conn_is_tunneling(ਬਣਤਰ connectdata *conn, int8_t sockindex)
{
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)sockindex;
  ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ /* CURL_DISABLE_PROXY */

ਸਥਿਰ bool cf_is_ssl(ਬਣਤਰ Curl_cfilter *cf)
{
  ਲਈ(; cf; cf = cf->ਅਗਲਾ) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    ਜੇ((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      ਵਾਪਸ TRUE;
    ਜੇ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ FALSE;
}

bool Curl_conn_is_ssl(ਬਣਤਰ connectdata *conn, int8_t sockindex)
{
  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ FALSE;
  ਵਾਪਸ conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ connectdata *conn, int8_t sockindex,
                            ਪੂਰਨ query,
                            ਬਣਤਰ curl_tlssessioninfo *info)
{
  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ FALSE;
  ਜੇ(Curl_conn_is_ssl(conn, sockindex)) {
    ਬਣਤਰ Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode ਨਤੀਜਾ = cf ?
      cf->cft->query(cf, ਅੰਕੜੇ, query, ਨਿਰਾ, (ਖਾਲੀ *)info) :
      CURLE_UNKNOWN_OPTION;
    ਵਾਪਸ !ਨਤੀਜਾ;
  }
  ਵਾਪਸ FALSE;
}

CURLcode Curl_conn_get_ip_info(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, ਬਣਤਰ ip_quadruple *ipquad)
{
  ਬਣਤਰ Curl_cfilter *cf;
  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  cf = conn ? conn->cfilter[sockindex] : ਨਿਰਾ;
  ਵਾਪਸ Curl_conn_cf_get_ip_info(cf, ਅੰਕੜੇ, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(ਬਣਤਰ connectdata *conn, int8_t sockindex)
{
  ਬਣਤਰ Curl_cfilter *cf;

  ਜੇ(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ FALSE;
  ਲਈ(cf = conn->cfilter[sockindex]; cf; cf = cf->ਅਗਲਾ) {
    ਜੇ(cf->cft->flags & CF_TYPE_MULTIPLEX)
      ਵਾਪਸ TRUE;
    ਜੇ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ FALSE;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ Curl_conn_get_transport(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ਵਾਪਸ Curl_conn_cf_get_transport(cf, ਅੰਕੜੇ);
}

ਪੂਰਨ Curl_socktype_for_transport(uint8_t transport)
{
  ਚੋਣ(transport) {
  ਹਾਲਤ TRNSPRT_TCP:
    ਵਾਪਸ SOCK_STREAM;
  ਹਾਲਤ TRNSPRT_UNIX:
    ਵਾਪਸ SOCK_STREAM;
  ਮੂਲ: /* UDP and QUIC */
    ਵਾਪਸ SOCK_DGRAM;
  }
}

ਪੂਰਨ Curl_protocol_for_transport(uint8_t transport)
{
  ਚੋਣ(transport) {
  ਹਾਲਤ TRNSPRT_TCP:
    ਵਾਪਸ IPPROTO_TCP;
  ਹਾਲਤ TRNSPRT_UNIX:
    ਵਾਪਸ IPPROTO_IP;
  ਮੂਲ: /* UDP and QUIC */
    ਵਾਪਸ IPPROTO_UDP;
  }
}

ਅਚਲ ਅੱਖਰ *Curl_conn_get_alpn_negotiated(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ਵਾਪਸ Curl_conn_cf_get_alpn_negotiated(cf, ਅੰਕੜੇ);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ Curl_conn_http_ਸੰਸਕਰਣ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ = CURLE_UNKNOWN_OPTION;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  ਲਈ(; cf; cf = cf->ਅਗਲਾ) {
    ਜੇ(cf->cft->flags & CF_TYPE_HTTP) {
      ਪੂਰਨ ਮੁੱਲ = 0;
      ਨਤੀਜਾ = cf->cft->query(cf, ਅੰਕੜੇ, CF_QUERY_HTTP_ਸੰਸਕਰਣ, &ਮੁੱਲ, ਨਿਰਾ);
      ਜੇ(!ਨਤੀਜਾ && ((ਮੁੱਲ < 0) || (ਮੁੱਲ > 255)))
        ਨਤੀਜਾ = CURLE_FAILED_INIT;
      ਨਹੀਂਤਾਂ
        v = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ਮੁੱਲ;
      ਰੋਕੋ;
    }
    ਜੇ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ਰੋਕੋ;
  }
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(ਨਤੀਜਾ ? 0 : v);
}

bool Curl_conn_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex)
{
  ਬਣਤਰ Curl_cfilter *cf;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ FALSE;

  cf = ਅੰਕੜੇ->conn->cfilter[sockindex];
  ਜਦੋਂ(cf && !cf->connected) {
    cf = cf->ਅਗਲਾ;
  }
  ਜੇ(cf) {
    ਵਾਪਸ cf->cft->has_ਅੰਕੜੇ_pending(cf, ਅੰਕੜੇ);
  }
  ਵਾਪਸ FALSE;
}

bool Curl_conn_cf_needs_flush(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ pending = 0;
  ਨਤੀਜਾ = cf ? cf->cft->query(cf, ਅੰਕੜੇ, CF_QUERY_NEED_FLUSH,
                               &pending, ਨਿਰਾ) : CURLE_UNKNOWN_OPTION;
  ਵਾਪਸ (ਨਤੀਜਾ || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex)
{
  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ FALSE;
  ਵਾਪਸ Curl_conn_cf_needs_flush(ਅੰਕੜੇ->conn->cfilter[sockindex], ਅੰਕੜੇ);
}

CURLcode Curl_conn_cf_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ easy_pollset *ps)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  ਲਈ(; cf && !ਨਤੀਜਾ; cf = cf->ਅਗਲਾ) {
    ਜੇ(cf->shutdown)
      ਜਾਰੀ;
    ਜੇ(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->ਅਗਲਾ || cf->ਅਗਲਾ->connected)
      ਨਤੀਜਾ = cf->cft->adjust_pollset(cf, ਅੰਕੜੇ, ps);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_conn_adjust_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn,
                                  ਬਣਤਰ easy_pollset *ps)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ bool want_io = !!ps->n;
  int8_t i;

  DEBUGASSERT(ਅੰਕੜੇ);
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
  ਲਈ(i = 0; (i < (ਪੂਰਨ)CURL_ARRAYSIZE(conn->cfilter)) && !ਨਤੀਜਾ; ++i) {
    ਜੇ(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_ਵਿੱਚ_SHUTDOWN(conn, i)))
      ਨਤੀਜਾ = Curl_conn_cf_adjust_pollset(conn->cfilter[i], ਅੰਕੜੇ, ps);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
ਪੂਰਨ Curl_conn_cf_poll(ਬਣਤਰ Curl_cfilter *cf,
                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      timediff_t timeout_ms)
{
  ਬਣਤਰ easy_pollset ps;
  ਪੂਰਨ rc;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(cf);
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  Curl_pollset_init(&ps);

  ਨਤੀਜਾ = Curl_conn_cf_adjust_pollset(cf, ਅੰਕੜੇ, &ps);
  ਜੇ(!ਨਤੀਜਾ)
    rc = Curl_pollset_poll(ਅੰਕੜੇ, &ps, timeout_ms);
  ਨਹੀਂਤਾਂ
    rc = -1;
  Curl_pollset_cleanup(&ps);
  ਵਾਪਸ rc;
}

ਖਾਲੀ Curl_conn_get_current_host(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                                ਅਚਲ ਅੱਖਰ **phost, ਪੂਰਨ *pport)
{
  ਬਣਤਰ Curl_cfilter *cf, *cf_proxy = ਨਿਰਾ;
  ਪੂਰਨ portarg = -1;

  ਜੇ(!ਅੰਕੜੇ->conn) {
    DEBUGASSERT(0);
    *phost = "";
    ਜੇ(pport)
      *pport = -1;
    ਵਾਪਸ;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? ਅੰਕੜੇ->conn->cfilter[sockindex] : ਨਿਰਾ;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  ਜਦੋਂ(cf && !cf->connected) {
    ਜੇ((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->ਅਗਲਾ;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  ਜੇ(!cf_proxy || cf_proxy->cft->query(cf_proxy, ਅੰਕੜੇ, CF_QUERY_HOST_ਪੋਰਟ,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = ਅੰਕੜੇ->conn->origin->hostname;
    portarg = ਅੰਕੜੇ->conn->origin->ਪੋਰਟ;
  }
  ਜੇ(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)event;
  (ਖਾਲੀ)arg1;
  (ਖਾਲੀ)arg2;
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            bool ignore_ਨਤੀਜਾ,
                            ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਲਈ(; cf; cf = cf->ਅਗਲਾ) {
    ਜੇ(cf->cft->cntrl == Curl_cf_def_cntrl)
      ਜਾਰੀ;
    ਨਤੀਜਾ = cf->cft->cntrl(cf, ਅੰਕੜੇ, event, arg1, arg2);
    ਜੇ(!ignore_ਨਤੀਜਾ && ਨਤੀਜਾ)
      ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

curl_socket_t Curl_conn_cf_get_socket(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  curl_socket_t sock;
  ਜੇ(cf && !cf->cft->query(cf, ਅੰਕੜੇ, CF_QUERY_SOCKET, ਨਿਰਾ, &sock))
    ਵਾਪਸ sock;
  ਵਾਪਸ CURL_SOCKET_BAD;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ Curl_conn_cf_get_transport(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਪੂਰਨ transport = 0;
  ਜੇ(cf && !cf->cft->query(cf, ਅੰਕੜੇ, CF_QUERY_TRANSPORT, &transport, ਨਿਰਾ))
    ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)transport;
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(ਅੰਕੜੇ->conn ? ਅੰਕੜੇ->conn->transport_wanted : 0);
}

ਅਚਲ ਅੱਖਰ *Curl_conn_cf_get_alpn_negotiated(ਬਣਤਰ Curl_cfilter *cf,
                                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਅੱਖਰ *alpn = ਨਿਰਾ;
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "query ALPN");
  ਜੇ(cf && !cf->cft->query(cf, ਅੰਕੜੇ, CF_QUERY_ALPN_NEGOTIATED, ਨਿਰਾ,
                           CURL_UNCONST(&alpn)))
    ਵਾਪਸ alpn;
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex *cf_get_remote_addr(
  ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex *remote_addr = ਨਿਰਾ;
  ਜੇ(cf &&
     !cf->cft->query(cf, ਅੰਕੜੇ, CF_QUERY_REMOTE_ADDR, ਨਿਰਾ,
                     CURL_UNCONST(&remote_addr)))
    ਵਾਪਸ remote_addr;
  ਵਾਪਸ ਨਿਰਾ;
}

CURLcode Curl_conn_cf_get_ip_info(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  bool *is_ipv6, ਬਣਤਰ ip_quadruple *ipquad)
{
  CURLcode ਨਤੀਜਾ = CURLE_UNKNOWN_OPTION;
  ਜੇ(cf) {
    ਪੂਰਨ ipv6 = 0;
    ਨਤੀਜਾ = cf->cft->query(cf, ਅੰਕੜੇ, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

curl_socket_t Curl_conn_get_ਪਹਿਲਾ_socket(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf;

  ਜੇ(!ਅੰਕੜੇ->conn)
    ਵਾਪਸ CURL_SOCKET_BAD;

  cf = ਅੰਕੜੇ->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  ਜੇ(cf && !cf->connected)
    ਵਾਪਸ Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ);
  ਵਾਪਸ ਅੰਕੜੇ->conn->sock[FIRSTSOCKET];
}

ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex)
{
  ਬਣਤਰ Curl_cfilter *cf =
    (ਅੰਕੜੇ->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    ਅੰਕੜੇ->conn->cfilter[sockindex] : ਨਿਰਾ;
  ਵਾਪਸ cf ? cf_get_remote_addr(cf, ਅੰਕੜੇ) : ਨਿਰਾ;
}

CURLcode Curl_conn_ev_ਅੰਕੜੇ_ਤਿਆਰੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ cf_cntrl_all(ਅੰਕੜੇ->conn, ਅੰਕੜੇ, FALSE, CF_CTRL_ਅੰਕੜੇ_ਤਿਆਰੀ, 0, ਨਿਰਾ);
}

CURLcode Curl_conn_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex)
{
  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਵਾਪਸ Curl_conn_cf_cntrl(ਅੰਕੜੇ->conn->cfilter[sockindex], ਅੰਕੜੇ, FALSE,
                            CF_CTRL_FLUSH, 0, ਨਿਰਾ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
ਖਾਲੀ Curl_conn_ev_ਅੰਕੜੇ_ਪੂਰਾ_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  cf_cntrl_all(ਅੰਕੜੇ->conn, ਅੰਕੜੇ, TRUE, CF_CTRL_ਅੰਕੜੇ_ਪੂਰਾ_SEND, 0, ਨਿਰਾ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
ਖਾਲੀ Curl_conn_ev_ਅੰਕੜੇ_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool premature)
{
  cf_cntrl_all(ਅੰਕੜੇ->conn, ਅੰਕੜੇ, TRUE, CF_CTRL_ਅੰਕੜੇ_ਪੂਰਾ, premature, ਨਿਰਾ);
}

CURLcode Curl_conn_ev_ਅੰਕੜੇ_pause(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool do_pause)
{
  ਵਾਪਸ cf_cntrl_all(ਅੰਕੜੇ->conn, ਅੰਕੜੇ, FALSE,
                      CF_CTRL_ਅੰਕੜੇ_PAUSE, do_pause, ਨਿਰਾ);
}

bool Curl_conn_is_alive(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                        bool *ਨਿਵੇਸ਼_pending)
{
  ਬਣਤਰ Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ਵਾਪਸ cf && !cf->conn->ਬਿੱਟ.close &&
         cf->cft->is_alive(cf, ਅੰਕੜੇ, ਨਿਵੇਸ਼_pending);
}

CURLcode Curl_conn_keep_alive(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ i;

  ਲਈ(i = 0; (i < (ਪੂਰਨ)CURL_ARRAYSIZE(conn->cfilter)) && !ਨਤੀਜਾ; ++i) {
    ਬਣਤਰ Curl_cfilter *cf = conn->cfilter[i];
    ਜੇ(cf)
      ਨਤੀਜਾ = cf->cft->keep_alive(cf, ਅੰਕੜੇ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

size_t Curl_conn_get_max_concurrent(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ connectdata *conn,
                                    int8_t sockindex)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ n = -1;

  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ 0;

  cf = conn->cfilter[sockindex];
  ਨਤੀਜਾ = cf ? cf->cft->query(cf, ਅੰਕੜੇ, CF_QUERY_MAX_CONCURRENT,
                               &n, ਨਿਰਾ) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  ਵਾਪਸ (ਨਤੀਜਾ || n < 0) ? 1 : (size_t)n;
}

ਪੂਰਨ Curl_conn_get_ਧਾਰਾ_error(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn,
                               int8_t sockindex)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ n = 0;

  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ 0;

  cf = conn->cfilter[sockindex];
  ਨਤੀਜਾ = cf ? cf->cft->query(cf, ਅੰਕੜੇ, CF_QUERY_ਧਾਰਾ_ERROR,
                               &n, ਨਿਰਾ) : CURLE_UNKNOWN_OPTION;
  ਵਾਪਸ (ਨਤੀਜਾ || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_socket_t sockfd)
{
  ਜੇ(ਅੰਕੜੇ && ਅੰਕੜੇ->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == ਅੰਕੜੇ->conn->sock[SECONDARYSOCKET])
    ਵਾਪਸ SECONDARYSOCKET;
  ਵਾਪਸ FIRSTSOCKET;
}

CURLcode Curl_conn_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                        ਅੱਖਰ *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਜੇ(ਅੰਕੜੇ && ਅੰਕੜੇ->conn && ਅੰਕੜੇ->conn->recv[sockindex])
    ਵਾਪਸ ਅੰਕੜੇ->conn->recv[sockindex](ਅੰਕੜੇ, sockindex, buf, len, pnread);
  *pnread = 0;
  ਵਾਪਸ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                        ਅਚਲ ਖਾਲੀ *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ਅਚਲ ਅੱਖਰ *p = getenv("CURL_SMALLSENDS");
    ਜੇ(p) {
      curl_off_t altsize;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        ਜੇ(write_len != len)
          eos = FALSE;
      }
    }
  }
#ਜੇ_ਅੰਤ
  ਜੇ(ਅੰਕੜੇ && ਅੰਕੜੇ->conn && ਅੰਕੜੇ->conn->send[sockindex]) {
    CURLcode ਨਤੀਜਾ = ਅੰਕੜੇ->conn->send[sockindex](ਅੰਕੜੇ, sockindex, buf,
                                                  write_len, eos, pnwritten);
    ਜੇ(!ਨਤੀਜਾ && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      ਵਾਪਸ CURLE_SEND_ERROR;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  *pnwritten = 0;
  ਵਾਪਸ CURLE_FAILED_INIT;
}
