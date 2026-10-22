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

CURLcode Curl_cf_def_connect(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(cf->ಮುಂದಿನ) {
    ಫಲಿತಾಂಶ = cf->ಮುಂದಿನ->cft->do_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  cf->connected = TRUE;
  *ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  *ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ easy_pollset *ps)
{
  /* NOP */
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)ps;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

bool Curl_cf_def_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->has_ದತ್ತಾಂಶ_pending(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ) : FALSE;
}

CURLcode Curl_cf_def_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  ಆದರೆ(cf->ಮುಂದಿನ)
    ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ->cft->do_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಅಕ್ಷರ *buf, size_t len, size_t *pnread)
{
  ಆದರೆ(cf->ಮುಂದಿನ)
    ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ->cft->do_recv(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, buf, len, pnread);
  *pnread = 0;
  ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               bool *ಒಳಹರಿವು_pending)
{
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->is_alive(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಒಳಹರಿವು_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->keep_alive(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಶೂನ್ಯ Curl_conn_trc_filters(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           int8_t sockindex, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *info)
{
  ಆದರೆ(CURL_TRC_M_is_verbose(ದತ್ತಾಂಶ) && ದತ್ತಾಂಶ->conn) {
    ರಚನೆ Curl_cfilter *cf = ದತ್ತಾಂಶ->conn->cfilter[sockindex];

    ಆದರೆ(cf) {
      ಅಕ್ಷರ ಸಂದೇಶ[256], *buf;
      ಪೂರ್ಣಾಂಕ blen, n;

      buf = ಸಂದೇಶ;
      blen = ಗಾತ್ರ(ಸಂದೇಶ) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      ಗಾಗಿ(; cf && blen; cf = cf->ಮುಂದಿನ) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(ದತ್ತಾಂಶ, "%s%s", ಸಂದೇಶ, blen ? "" : "...");
    }
    ಇಲ್ಲದಿದ್ದರೆ
      CURL_TRC_M(ದತ್ತಾಂಶ, "%s [%d][-]", info, sockindex);
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

ಶೂನ್ಯ Curl_conn_cf_discard_chain(ರಚನೆ Curl_cfilter **pcf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cfn, *cf = *pcf;

  ಆದರೆ(cf) {
    *pcf = ಖಾಲಿ;
    ಆಗ(cf) {
      cfn = cf->ಮುಂದಿನ;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->ಮುಂದಿನ = ಖಾಲಿ;
      cf->cft->destroy(cf, ದತ್ತಾಂಶ);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

ಶೂನ್ಯ Curl_conn_cf_discard_all(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn, int8_t sockindex)
{
  conn->shutdown.ಆರಂಭ_ms[sockindex] = -1;
  conn->connect_started.ಗಣ[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], ದತ್ತಾಂಶ);
}

CURLcode Curl_cf_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex, ಅಕ್ಷರ *buf,
                      size_t len, size_t *pnread)
{
  ರಚನೆ Curl_cfilter *cf;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  cf = ದತ್ತಾಂಶ->conn->cfilter[sockindex];
  ಆಗ(cf && !cf->connected)
    cf = cf->ಮುಂದಿನ;
  ಆದರೆ(cf)
    ಹಿಂತಿರುಗಿ cf->cft->do_recv(cf, ದತ್ತಾಂಶ, buf, len, pnread);
  failf(ದತ್ತಾಂಶ, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                      ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  ರಚನೆ Curl_cfilter *cf;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  cf = ದತ್ತಾಂಶ->conn->cfilter[sockindex];
  ಆಗ(cf && !cf->connected)
    cf = cf->ಮುಂದಿನ;
  ಆದರೆ(cf) {
    ಹಿಂತಿರುಗಿ cf->cft->do_send(cf, ದತ್ತಾಂಶ, buf, len, eos, pnwritten);
  }
  failf(ದತ್ತಾಂಶ, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
}

ರಚನೆ cf_io_ctx {
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ರಚನೆ Curl_cfilter *cf;
};

ಸ್ಥಿರ CURLcode cf_bufq_reader(ಶೂನ್ಯ *writer_ctx,
                               ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t blen,
                               size_t *pnread)
{
  ರಚನೆ cf_io_ctx *io = writer_ctx;
  ಹಿಂತಿರುಗಿ Curl_conn_cf_recv(io->cf, io->ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  ರಚನೆ cf_io_ctx io;

  ಆದರೆ(!cf || !ದತ್ತಾಂಶ) {
    *pnread = 0;
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
  io.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
  io.cf = cf;
  ಹಿಂತಿರುಗಿ Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

ಸ್ಥಿರ CURLcode cf_bufq_writer(ಶೂನ್ಯ *writer_ctx,
                               ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  ರಚನೆ cf_io_ctx *io = writer_ctx;
  ಹಿಂತಿರುಗಿ Curl_conn_cf_send(io->cf, io->ದತ್ತಾಂಶ, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ bufq *bufq,
                           ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t blen,
                           size_t *pnwritten)
{
  ರಚನೆ cf_io_ctx io;

  ಆದರೆ(!cf || !ದತ್ತಾಂಶ) {
    *pnwritten = 0;
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
  io.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
  io.cf = cf;
  ಆದರೆ(buf && blen)
    ಹಿಂತಿರುಗಿ Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(ರಚನೆ Curl_cfilter **pcf,
                        ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cftype *cft,
                        ಶೂನ್ಯ *ctx)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, ಗಾತ್ರ(*cf));
  ಆದರೆ(!cf)
    ಹೋಗು ಹೊರಹೋಗು;

  cf->cft = cft;
  cf->ctx = ctx;
  ಫಲಿತಾಂಶ = CURLE_OK;
ಹೊರಹೋಗು:
  *pcf = cf;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_conn_cf_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ connectdata *conn,
                      int8_t sockindex,
                      ರಚನೆ Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->ಮುಂದಿನ);

  cf->ಮುಂದಿನ = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "added");
}

ಶೂನ್ಯ Curl_conn_cf_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                               ರಚನೆ Curl_cfilter *cf_new)
{
  ರಚನೆ Curl_cfilter *ಬಾಲ, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  ಬಾಲ = cf_at->ಮುಂದಿನ;
  cf_at->ಮುಂದಿನ = cf_new;
  ಮಾಡು {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->ಮುಂದಿನ;
    cf_new = cf_new->ಮುಂದಿನ;
  } ಆಗ(cf_new);
  *pnext = ಬಾಲ;
}

bool Curl_conn_cf_discard(ರಚನೆ Curl_cfilter **pcf,
                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = pcf ? *pcf : ಖಾಲಿ;
  bool ಸಿಕ್ಕಿತು = FALSE;
  ಆದರೆ(cf) {
    ಆದರೆ(cf->conn) {
      /* unlink if present in connection filter chain */
      ರಚನೆ Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      ಆಗ(*pprev) {
        ಆದರೆ(*pprev == *pcf) {
          *pprev = (*pcf)->ಮುಂದಿನ;
          cf->ಮುಂದಿನ = ಖಾಲಿ;
          ಸಿಕ್ಕಿತು = TRUE;
          ನಿಲ್ಲಿಸು;
        }
        pprev = &(*pprev)->ಮುಂದಿನ;
      }
    }
    Curl_conn_cf_discard_chain(pcf, ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

CURLcode Curl_conn_cf_connect(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              bool *ಮುಗಿದ)
{
  ಆದರೆ(cf)
    ಹಿಂತಿರುಗಿ cf->cft->do_connect(cf, ದತ್ತಾಂಶ, ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ಆದರೆ(cf)
    ಹಿಂತಿರುಗಿ cf->cft->do_send(cf, ದತ್ತಾಂಶ, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಅಕ್ಷರ *buf, size_t len, size_t *pnread)
{
  ಆದರೆ(cf)
    ಹಿಂತಿರುಗಿ cf->cft->do_recv(cf, ದತ್ತಾಂಶ, buf, len, pnread);
  *pnread = 0;
  ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
}

ಸ್ಥಿರ CURLcode cf_cntrl_all(ರಚನೆ connectdata *conn,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             bool ignore_ಫಲಿತಾಂಶ,
                             ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ i;

  ಗಾಗಿ(i = 0; i < (ಪೂರ್ಣಾಂಕ)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    ಫಲಿತಾಂಶ = Curl_conn_cf_cntrl(conn->cfilter[i], ದತ್ತಾಂಶ, ignore_ಫಲಿತಾಂಶ,
                                event, arg1, arg2);
    ಆದರೆ(!ignore_ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_conn_cf_is_ip_connected(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(cf) {
    ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ = 0;
    ಆದರೆ(!cf->cft->query(cf, ದತ್ತಾಂಶ, CF_QUERY_REALLY_CONNECTED, &ಮೌಲ್ಯ, ಖಾಲಿ))
      ಹಿಂತಿರುಗಿ !!ಮೌಲ್ಯ;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಶೂನ್ಯ Curl_conn_cntrl_update_info(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *conn)
{
  cf_cntrl_all(conn, ದತ್ತಾಂಶ, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, ಖಾಲಿ);
}

ಶೂನ್ಯ Curl_conn_cntrl_report_stats(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn,
                                  ಪೂರ್ಣಾಂಕ sockindex)
{
  ಆದರೆ((ಚಿಹ್ನೆರಹಿತ)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (ಶೂನ್ಯ)Curl_conn_cf_cntrl(conn->cfilter[sockindex], ದತ್ತಾಂಶ, TRUE,
                             CF_CTRL_REPORT_STATS, 0, ಖಾಲಿ);
}

ಶೂನ್ಯ Curl_conn_remove_ಸಿದ್ಧತೆ_filters(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    int8_t sockindex)
{
  ರಚನೆ Curl_cfilter **anchor = &ದತ್ತಾಂಶ->conn->cfilter[sockindex];
  ಆಗ(*anchor) {
    ರಚನೆ Curl_cfilter *cf = *anchor;
    ಆದರೆ(cf->connected && (cf->cft->flags & CF_TYPE_ಸಿದ್ಧತೆ)) {
      *anchor = cf->ಮುಂದಿನ;
      cf->ಮುಂದಿನ = ಖಾಲಿ;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "removing connected setup filter");
      cf->cft->destroy(cf, ದತ್ತಾಂಶ);
      curlx_free(cf);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      anchor = &cf->ಮುಂದಿನ;
  }
}

bool Curl_conn_is_ಸಿದ್ಧತೆ(ರಚನೆ connectdata *conn, int8_t sockindex)
{
  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(ರಚನೆ connectdata *conn, int8_t sockindex)
{
  ರಚನೆ Curl_cfilter *cf;

  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ FALSE;
  cf = conn->cfilter[sockindex];
  ಆದರೆ(cf)
    ಹಿಂತಿರುಗಿ (bool)cf->connected;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(conn->scheme->flags & PROTOPT_NONETWORK)
    ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_conn_is_ip_connected(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex)
{
  ರಚನೆ Curl_cfilter *cf;

  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ FALSE;
  cf = ದತ್ತಾಂಶ->conn->cfilter[sockindex];
  ಆಗ(cf) {
    ಆದರೆ(cf->connected)
      ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ಹಿಂತಿರುಗಿ FALSE;
    cf = cf->ಮುಂದಿನ;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
ಸ್ಥಿರ bool cf_is_tunneling(ರಚನೆ Curl_cfilter *cf)
{
  ಗಾಗಿ(; cf; cf = cf->ಮುಂದಿನ) {
    ಆದರೆ((cf->cft->flags & CF_TYPE_PROXY))
      ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_conn_is_tunneling(ರಚನೆ connectdata *conn, int8_t sockindex)
{
  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#ಇಲ್ಲದಿದ್ದರೆ
bool Curl_conn_is_tunneling(ರಚನೆ connectdata *conn, int8_t sockindex)
{
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)sockindex;
  ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_PROXY */

ಸ್ಥಿರ bool cf_is_ssl(ರಚನೆ Curl_cfilter *cf)
{
  ಗಾಗಿ(; cf; cf = cf->ಮುಂದಿನ) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    ಆದರೆ((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_conn_is_ssl(ರಚನೆ connectdata *conn, int8_t sockindex)
{
  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ connectdata *conn, int8_t sockindex,
                            ಪೂರ್ಣಾಂಕ query,
                            ರಚನೆ curl_tlssessioninfo *info)
{
  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(Curl_conn_is_ssl(conn, sockindex)) {
    ರಚನೆ Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode ಫಲಿತಾಂಶ = cf ?
      cf->cft->query(cf, ದತ್ತಾಂಶ, query, ಖಾಲಿ, (ಶೂನ್ಯ *)info) :
      CURLE_UNKNOWN_OPTION;
    ಹಿಂತಿರುಗಿ !ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

CURLcode Curl_conn_get_ip_info(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, ರಚನೆ ip_quadruple *ipquad)
{
  ರಚನೆ Curl_cfilter *cf;
  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  cf = conn ? conn->cfilter[sockindex] : ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ Curl_conn_cf_get_ip_info(cf, ದತ್ತಾಂಶ, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(ರಚನೆ connectdata *conn, int8_t sockindex)
{
  ರಚನೆ Curl_cfilter *cf;

  ಆದರೆ(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ FALSE;
  ಗಾಗಿ(cf = conn->cfilter[sockindex]; cf; cf = cf->ಮುಂದಿನ) {
    ಆದರೆ(cf->cft->flags & CF_TYPE_MULTIPLEX)
      ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ Curl_conn_get_transport(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn)
{
  ರಚನೆ Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ಹಿಂತಿರುಗಿ Curl_conn_cf_get_transport(cf, ದತ್ತಾಂಶ);
}

ಪೂರ್ಣಾಂಕ Curl_socktype_for_transport(uint8_t transport)
{
  ಆಯ್ಕೆ(transport) {
  ಸ್ಥಿತಿ TRNSPRT_TCP:
    ಹಿಂತಿರುಗಿ SOCK_STREAM;
  ಸ್ಥಿತಿ TRNSPRT_UNIX:
    ಹಿಂತಿರುಗಿ SOCK_STREAM;
  ಪೂರ್ವನಿಯೋಜಿತ: /* UDP and QUIC */
    ಹಿಂತಿರುಗಿ SOCK_DGRAM;
  }
}

ಪೂರ್ಣಾಂಕ Curl_protocol_for_transport(uint8_t transport)
{
  ಆಯ್ಕೆ(transport) {
  ಸ್ಥಿತಿ TRNSPRT_TCP:
    ಹಿಂತಿರುಗಿ IPPROTO_TCP;
  ಸ್ಥಿತಿ TRNSPRT_UNIX:
    ಹಿಂತಿರುಗಿ IPPROTO_IP;
  ಪೂರ್ವನಿಯೋಜಿತ: /* UDP and QUIC */
    ಹಿಂತಿರುಗಿ IPPROTO_UDP;
  }
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_conn_get_alpn_negotiated(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ connectdata *conn)
{
  ರಚನೆ Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ಹಿಂತಿರುಗಿ Curl_conn_cf_get_alpn_negotiated(cf, ದತ್ತಾಂಶ);
}

ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ Curl_conn_http_ಆವೃತ್ತಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ connectdata *conn)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ = CURLE_UNKNOWN_OPTION;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  ಗಾಗಿ(; cf; cf = cf->ಮುಂದಿನ) {
    ಆದರೆ(cf->cft->flags & CF_TYPE_HTTP) {
      ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ = 0;
      ಫಲಿತಾಂಶ = cf->cft->query(cf, ದತ್ತಾಂಶ, CF_QUERY_HTTP_ಆವೃತ್ತಿ, &ಮೌಲ್ಯ, ಖಾಲಿ);
      ಆದರೆ(!ಫಲಿತಾಂಶ && ((ಮೌಲ್ಯ < 0) || (ಮೌಲ್ಯ > 255)))
        ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
      ಇಲ್ಲದಿದ್ದರೆ
        v = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ಮೌಲ್ಯ;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(ಫಲಿತಾಂಶ ? 0 : v);
}

bool Curl_conn_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex)
{
  ರಚನೆ Curl_cfilter *cf;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ FALSE;

  cf = ದತ್ತಾಂಶ->conn->cfilter[sockindex];
  ಆಗ(cf && !cf->connected) {
    cf = cf->ಮುಂದಿನ;
  }
  ಆದರೆ(cf) {
    ಹಿಂತಿರುಗಿ cf->cft->has_ದತ್ತಾಂಶ_pending(cf, ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_conn_cf_needs_flush(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ pending = 0;
  ಫಲಿತಾಂಶ = cf ? cf->cft->query(cf, ದತ್ತಾಂಶ, CF_QUERY_NEED_FLUSH,
                               &pending, ಖಾಲಿ) : CURLE_UNKNOWN_OPTION;
  ಹಿಂತಿರುಗಿ (ಫಲಿತಾಂಶ || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex)
{
  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ Curl_conn_cf_needs_flush(ದತ್ತಾಂಶ->conn->cfilter[sockindex], ದತ್ತಾಂಶ);
}

CURLcode Curl_conn_cf_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ easy_pollset *ps)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  ಗಾಗಿ(; cf && !ಫಲಿತಾಂಶ; cf = cf->ಮುಂದಿನ) {
    ಆದರೆ(cf->shutdown)
      ಮುಂದುವರಿ;
    ಆದರೆ(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->ಮುಂದಿನ || cf->ಮುಂದಿನ->connected)
      ಫಲಿತಾಂಶ = cf->cft->adjust_pollset(cf, ದತ್ತಾಂಶ, ps);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_conn_adjust_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn,
                                  ರಚನೆ easy_pollset *ps)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ bool want_io = !!ps->n;
  int8_t i;

  DEBUGASSERT(ದತ್ತಾಂಶ);
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
  ಗಾಗಿ(i = 0; (i < (ಪೂರ್ಣಾಂಕ)CURL_ARRAYSIZE(conn->cfilter)) && !ಫಲಿತಾಂಶ; ++i) {
    ಆದರೆ(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_ಒಳ_SHUTDOWN(conn, i)))
      ಫಲಿತಾಂಶ = Curl_conn_cf_adjust_pollset(conn->cfilter[i], ದತ್ತಾಂಶ, ps);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
ಪೂರ್ಣಾಂಕ Curl_conn_cf_poll(ರಚನೆ Curl_cfilter *cf,
                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      timediff_t timeout_ms)
{
  ರಚನೆ easy_pollset ps;
  ಪೂರ್ಣಾಂಕ rc;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(cf);
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  Curl_pollset_init(&ps);

  ಫಲಿತಾಂಶ = Curl_conn_cf_adjust_pollset(cf, ದತ್ತಾಂಶ, &ps);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    rc = Curl_pollset_poll(ದತ್ತಾಂಶ, &ps, timeout_ms);
  ಇಲ್ಲದಿದ್ದರೆ
    rc = -1;
  Curl_pollset_cleanup(&ps);
  ಹಿಂತಿರುಗಿ rc;
}

ಶೂನ್ಯ Curl_conn_get_current_host(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **phost, ಪೂರ್ಣಾಂಕ *pport)
{
  ರಚನೆ Curl_cfilter *cf, *cf_proxy = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ portarg = -1;

  ಆದರೆ(!ದತ್ತಾಂಶ->conn) {
    DEBUGASSERT(0);
    *phost = "";
    ಆದರೆ(pport)
      *pport = -1;
    ಹಿಂತಿರುಗಿ;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? ದತ್ತಾಂಶ->conn->cfilter[sockindex] : ಖಾಲಿ;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  ಆಗ(cf && !cf->connected) {
    ಆದರೆ((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->ಮುಂದಿನ;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  ಆದರೆ(!cf_proxy || cf_proxy->cft->query(cf_proxy, ದತ್ತಾಂಶ, CF_QUERY_HOST_ಪೋರ್ಟ್,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = ದತ್ತಾಂಶ->conn->origin->hostname;
    portarg = ದತ್ತಾಂಶ->conn->origin->ಪೋರ್ಟ್;
  }
  ಆದರೆ(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)event;
  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            bool ignore_ಫಲಿತಾಂಶ,
                            ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಗಾಗಿ(; cf; cf = cf->ಮುಂದಿನ) {
    ಆದರೆ(cf->cft->cntrl == Curl_cf_def_cntrl)
      ಮುಂದುವರಿ;
    ಫಲಿತಾಂಶ = cf->cft->cntrl(cf, ದತ್ತಾಂಶ, event, arg1, arg2);
    ಆದರೆ(!ignore_ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

curl_socket_t Curl_conn_cf_get_socket(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  curl_socket_t sock;
  ಆದರೆ(cf && !cf->cft->query(cf, ದತ್ತಾಂಶ, CF_QUERY_SOCKET, ಖಾಲಿ, &sock))
    ಹಿಂತಿರುಗಿ sock;
  ಹಿಂತಿರುಗಿ CURL_SOCKET_BAD;
}

ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ Curl_conn_cf_get_transport(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಪೂರ್ಣಾಂಕ transport = 0;
  ಆದರೆ(cf && !cf->cft->query(cf, ದತ್ತಾಂಶ, CF_QUERY_TRANSPORT, &transport, ಖಾಲಿ))
    ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)transport;
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(ದತ್ತಾಂಶ->conn ? ದತ್ತಾಂಶ->conn->transport_wanted : 0);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_conn_cf_get_alpn_negotiated(ರಚನೆ Curl_cfilter *cf,
                                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alpn = ಖಾಲಿ;
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "query ALPN");
  ಆದರೆ(cf && !cf->cft->query(cf, ದತ್ತಾಂಶ, CF_QUERY_ALPN_NEGOTIATED, ಖಾಲಿ,
                           CURL_UNCONST(&alpn)))
    ಹಿಂತಿರುಗಿ alpn;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex *cf_get_remote_addr(
  ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex *remote_addr = ಖಾಲಿ;
  ಆದರೆ(cf &&
     !cf->cft->query(cf, ದತ್ತಾಂಶ, CF_QUERY_REMOTE_ADDR, ಖಾಲಿ,
                     CURL_UNCONST(&remote_addr)))
    ಹಿಂತಿರುಗಿ remote_addr;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

CURLcode Curl_conn_cf_get_ip_info(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  bool *is_ipv6, ರಚನೆ ip_quadruple *ipquad)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_UNKNOWN_OPTION;
  ಆದರೆ(cf) {
    ಪೂರ್ಣಾಂಕ ipv6 = 0;
    ಫಲಿತಾಂಶ = cf->cft->query(cf, ದತ್ತಾಂಶ, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

curl_socket_t Curl_conn_get_ಮೊದಲ_socket(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf;

  ಆದರೆ(!ದತ್ತಾಂಶ->conn)
    ಹಿಂತಿರುಗಿ CURL_SOCKET_BAD;

  cf = ದತ್ತಾಂಶ->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  ಆದರೆ(cf && !cf->connected)
    ಹಿಂತಿರುಗಿ Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET];
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex)
{
  ರಚನೆ Curl_cfilter *cf =
    (ದತ್ತಾಂಶ->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    ದತ್ತಾಂಶ->conn->cfilter[sockindex] : ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ cf ? cf_get_remote_addr(cf, ದತ್ತಾಂಶ) : ಖಾಲಿ;
}

CURLcode Curl_conn_ev_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ cf_cntrl_all(ದತ್ತಾಂಶ->conn, ದತ್ತಾಂಶ, FALSE, CF_CTRL_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ, 0, ಖಾಲಿ);
}

CURLcode Curl_conn_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex)
{
  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಹಿಂತಿರುಗಿ Curl_conn_cf_cntrl(ದತ್ತಾಂಶ->conn->cfilter[sockindex], ದತ್ತಾಂಶ, FALSE,
                            CF_CTRL_FLUSH, 0, ಖಾಲಿ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
ಶೂನ್ಯ Curl_conn_ev_ದತ್ತಾಂಶ_ಮುಗಿದ_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  cf_cntrl_all(ದತ್ತಾಂಶ->conn, ದತ್ತಾಂಶ, TRUE, CF_CTRL_ದತ್ತಾಂಶ_ಮುಗಿದ_SEND, 0, ಖಾಲಿ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
ಶೂನ್ಯ Curl_conn_ev_ದತ್ತಾಂಶ_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool premature)
{
  cf_cntrl_all(ದತ್ತಾಂಶ->conn, ದತ್ತಾಂಶ, TRUE, CF_CTRL_ದತ್ತಾಂಶ_ಮುಗಿದ, premature, ಖಾಲಿ);
}

CURLcode Curl_conn_ev_ದತ್ತಾಂಶ_pause(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool do_pause)
{
  ಹಿಂತಿರುಗಿ cf_cntrl_all(ದತ್ತಾಂಶ->conn, ದತ್ತಾಂಶ, FALSE,
                      CF_CTRL_ದತ್ತಾಂಶ_PAUSE, do_pause, ಖಾಲಿ);
}

bool Curl_conn_is_alive(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn,
                        bool *ಒಳಹರಿವು_pending)
{
  ರಚನೆ Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ಹಿಂತಿರುಗಿ cf && !cf->conn->ಬಿಟ್ಟುಗಳು.close &&
         cf->cft->is_alive(cf, ದತ್ತಾಂಶ, ಒಳಹರಿವು_pending);
}

CURLcode Curl_conn_keep_alive(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ i;

  ಗಾಗಿ(i = 0; (i < (ಪೂರ್ಣಾಂಕ)CURL_ARRAYSIZE(conn->cfilter)) && !ಫಲಿತಾಂಶ; ++i) {
    ರಚನೆ Curl_cfilter *cf = conn->cfilter[i];
    ಆದರೆ(cf)
      ಫಲಿತಾಂಶ = cf->cft->keep_alive(cf, ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

size_t Curl_conn_get_max_concurrent(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ connectdata *conn,
                                    int8_t sockindex)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ n = -1;

  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ 0;

  cf = conn->cfilter[sockindex];
  ಫಲಿತಾಂಶ = cf ? cf->cft->query(cf, ದತ್ತಾಂಶ, CF_QUERY_MAX_CONCURRENT,
                               &n, ಖಾಲಿ) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  ಹಿಂತಿರುಗಿ (ಫಲಿತಾಂಶ || n < 0) ? 1 : (size_t)n;
}

ಪೂರ್ಣಾಂಕ Curl_conn_get_ಹರಿವು_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn,
                               int8_t sockindex)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ n = 0;

  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ 0;

  cf = conn->cfilter[sockindex];
  ಫಲಿತಾಂಶ = cf ? cf->cft->query(cf, ದತ್ತಾಂಶ, CF_QUERY_ಹರಿವು_ERROR,
                               &n, ಖಾಲಿ) : CURLE_UNKNOWN_OPTION;
  ಹಿಂತಿರುಗಿ (ಫಲಿತಾಂಶ || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_socket_t sockfd)
{
  ಆದರೆ(ದತ್ತಾಂಶ && ದತ್ತಾಂಶ->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == ದತ್ತಾಂಶ->conn->sock[SECONDARYSOCKET])
    ಹಿಂತಿರುಗಿ SECONDARYSOCKET;
  ಹಿಂತಿರುಗಿ FIRSTSOCKET;
}

CURLcode Curl_conn_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                        ಅಕ್ಷರ *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಆದರೆ(ದತ್ತಾಂಶ && ದತ್ತಾಂಶ->conn && ದತ್ತಾಂಶ->conn->recv[sockindex])
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->conn->recv[sockindex](ದತ್ತಾಂಶ, sockindex, buf, len, pnread);
  *pnread = 0;
  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                        ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = getenv("CURL_SMALLSENDS");
    ಆದರೆ(p) {
      curl_off_t altsize;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        ಆದರೆ(write_len != len)
          eos = FALSE;
      }
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(ದತ್ತಾಂಶ && ದತ್ತಾಂಶ->conn && ದತ್ತಾಂಶ->conn->send[sockindex]) {
    CURLcode ಫಲಿತಾಂಶ = ದತ್ತಾಂಶ->conn->send[sockindex](ದತ್ತಾಂಶ, sockindex, buf,
                                                  write_len, eos, pnwritten);
    ಆದರೆ(!ಫಲಿತಾಂಶ && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  *pnwritten = 0;
  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
}
