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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "strerror.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

CURLcode Curl_cf_def_connect(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଯଦି(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ଯଦି(result || !*done)
      ଫେରନ୍ତୁ result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data, bool *done)
{
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
  *done = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା easy_pollset *ps)
{
  /* NOP */
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)ps;
  ଫେରନ୍ତୁ CURLE_OK;
}

bool Curl_cf_def_data_pending(ସଂରଚନା Curl_cfilter *cf,
                              ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                          ଅଚଳ uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  ଯଦି(cf->next)
    ଫେରନ୍ତୁ cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ଫେରନ୍ତୁ CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                          ଅକ୍ଷର *buf, size_t len, size_t *pnread)
{
  ଯଦି(cf->next)
    ଫେରନ୍ତୁ cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  ଫେରନ୍ତୁ CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               bool *ନିବେଶ_pending)
{
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->is_alive(cf->next, data, ନିବେଶ_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(ସଂରଚନା Curl_cfilter *cf,
                           ସଂରଚନା Curl_easy *data,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
ଶୂନ୍ୟ Curl_conn_trc_filters(ସଂରଚନା Curl_easy *data,
                           int8_t sockindex, ଅଚଳ ଅକ୍ଷର *info)
{
  ଯଦି(CURL_TRC_M_is_verbose(data) && data->conn) {
    ସଂରଚନା Curl_cfilter *cf = data->conn->cfilter[sockindex];

    ଯଦି(cf) {
      ଅକ୍ଷର ସନ୍ଦେଶ[256], *buf;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା blen, n;

      buf = ସନ୍ଦେଶ;
      blen = ଆକାର(ସନ୍ଦେଶ) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      ପାଇଁ(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", ସନ୍ଦେଶ, blen ? "" : "...");
    }
    ଅନ୍ୟଥା
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

ଶୂନ୍ୟ Curl_conn_cf_discard_chain(ସଂରଚନା Curl_cfilter **pcf,
                                ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cfilter *cfn, *cf = *pcf;

  ଯଦି(cf) {
    *pcf = ଖାଲି;
    ଯେତେବେଳେ(cf) {
      cfn = cf->next;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->next = ଖାଲି;
      cf->cft->destroy(cf, data);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

ଶୂନ୍ୟ Curl_conn_cf_discard_all(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], data);
}

CURLcode Curl_cf_recv(ସଂରଚନା Curl_easy *data, int8_t sockindex, ଅକ୍ଷର *buf,
                      size_t len, size_t *pnread)
{
  ସଂରଚନା Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  ଯେତେବେଳେ(cf && !cf->connected)
    cf = cf->next;
  ଯଦି(cf)
    ଫେରନ୍ତୁ cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  ଫେରନ୍ତୁ CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                      ଅଚଳ uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  ସଂରଚନା Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  ଯେତେବେଳେ(cf && !cf->connected)
    cf = cf->next;
  ଯଦି(cf) {
    ଫେରନ୍ତୁ cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  ଫେରନ୍ତୁ CURLE_FAILED_INIT;
}

ସଂରଚନା cf_io_ctx {
  ସଂରଚନା Curl_easy *data;
  ସଂରଚନା Curl_cfilter *cf;
};

ସ୍ଥିର CURLcode cf_bufq_reader(ଶୂନ୍ୟ *writer_ctx,
                               unsigned ଅକ୍ଷର *buf, size_t blen,
                               size_t *pnread)
{
  ସଂରଚନା cf_io_ctx *io = writer_ctx;
  ଫେରନ୍ତୁ Curl_conn_cf_recv(io->cf, io->data, (ଅକ୍ଷର *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(ସଂରଚନା Curl_cfilter *cf,
                           ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  ସଂରଚନା cf_io_ctx io;

  ଯଦି(!cf || !data) {
    *pnread = 0;
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  ଫେରନ୍ତୁ Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

ସ୍ଥିର CURLcode cf_bufq_writer(ଶୂନ୍ୟ *writer_ctx,
                               ଅଚଳ uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  ସଂରଚନା cf_io_ctx *io = writer_ctx;
  ଫେରନ୍ତୁ Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(ସଂରଚନା Curl_cfilter *cf,
                           ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା bufq *bufq,
                           ଅଚଳ unsigned ଅକ୍ଷର *buf, size_t blen,
                           size_t *pnwritten)
{
  ସଂରଚନା cf_io_ctx io;

  ଯଦି(!cf || !data) {
    *pnwritten = 0;
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  ଯଦି(buf && blen)
    ଫେରନ୍ତୁ Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(ସଂରଚନା Curl_cfilter **pcf,
                        ଅଚଳ ସଂରଚନା Curl_cftype *cft,
                        ଶୂନ୍ୟ *ctx)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, ଆକାର(*cf));
  ଯଦି(!cf)
    ଯାଅ out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_conn_cf_add(ସଂରଚନା Curl_easy *data,
                      ସଂରଚନା connectdata *conn,
                      int8_t sockindex,
                      ସଂରଚନା Curl_cfilter *cf)
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

ଶୂନ୍ୟ Curl_conn_cf_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                               ସଂରଚନା Curl_cfilter *cf_new)
{
  ସଂରଚନା Curl_cfilter *tail, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  tail = cf_at->next;
  cf_at->next = cf_new;
  କର {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->next;
    cf_new = cf_new->next;
  } ଯେତେବେଳେ(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(ସଂରଚନା Curl_cfilter **pcf,
                          ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cfilter *cf = pcf ? *pcf : ଖାଲି;
  bool found = FALSE;
  ଯଦି(cf) {
    ଯଦି(cf->conn) {
      /* unlink if present in connection filter chain */
      ସଂରଚନା Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      ଯେତେବେଳେ(*pprev) {
        ଯଦି(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = ଖାଲି;
          found = TRUE;
          ଭାଙ୍ଗ;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  ଫେରନ୍ତୁ found;
}

CURLcode Curl_conn_cf_connect(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              bool *done)
{
  ଯଦି(cf)
    ଫେରନ୍ତୁ cf->cft->do_connect(cf, data, done);
  ଫେରନ୍ତୁ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                           ଅଚଳ uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ଯଦି(cf)
    ଫେରନ୍ତୁ cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ଫେରନ୍ତୁ CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                           ଅକ୍ଷର *buf, size_t len, size_t *pnread)
{
  ଯଦି(cf)
    ଫେରନ୍ତୁ cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  ଫେରନ୍ତୁ CURLE_RECV_ERROR;
}

ସ୍ଥିର CURLcode cf_cntrl_all(ସଂରଚନା connectdata *conn,
                             ସଂରଚନା Curl_easy *data,
                             bool ignore_result,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ପାଇଁ(i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    ଯଦି(!ignore_result && result)
      ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

bool Curl_conn_cf_is_ip_connected(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data)
{
  ଯଦି(cf) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ = 0;
    ଯଦି(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &ମୂଲ୍ୟ, ଖାଲି))
      ଫେରନ୍ତୁ !!ମୂଲ୍ୟ;
  }
  ଫେରନ୍ତୁ FALSE;
}

ଶୂନ୍ୟ Curl_conn_cntrl_update_info(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, ଖାଲି);
}

ଶୂନ୍ୟ Curl_conn_cntrl_report_stats(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata *conn,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockindex)
{
  ଯଦି((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (ଶୂନ୍ୟ)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, ଖାଲି);
}

ଶୂନ୍ୟ Curl_conn_remove_setup_filters(ସଂରଚନା Curl_easy *data,
                                    int8_t sockindex)
{
  ସଂରଚନା Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  ଯେତେବେଳେ(*anchor) {
    ସଂରଚନା Curl_cfilter *cf = *anchor;
    ଯଦି(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = ଖାଲି;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    ଅନ୍ୟଥା
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(ସଂରଚନା connectdata *conn, int8_t sockindex)
{
  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(ସଂରଚନା connectdata *conn, int8_t sockindex)
{
  ସଂରଚନା Curl_cfilter *cf;

  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ FALSE;
  cf = conn->cfilter[sockindex];
  ଯଦି(cf)
    ଫେରନ୍ତୁ (bool)cf->connected;
  ଅନ୍ୟଥା ଯଦି(conn->scheme->flags & PROTOPT_NONETWORK)
    ଫେରନ୍ତୁ TRUE;
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_conn_is_ip_connected(ସଂରଚନା Curl_easy *data, int8_t sockindex)
{
  ସଂରଚନା Curl_cfilter *cf;

  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ FALSE;
  cf = data->conn->cfilter[sockindex];
  ଯେତେବେଳେ(cf) {
    ଯଦି(cf->connected)
      ଫେରନ୍ତୁ TRUE;
    ଯଦି(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ଫେରନ୍ତୁ FALSE;
    cf = cf->next;
  }
  ଫେରନ୍ତୁ FALSE;
}

#ifndef CURL_DISABLE_PROXY
ସ୍ଥିର bool cf_is_tunneling(ସଂରଚନା Curl_cfilter *cf)
{
  ପାଇଁ(; cf; cf = cf->next) {
    ଯଦି((cf->cft->flags & CF_TYPE_PROXY))
      ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_conn_is_tunneling(ସଂରଚନା connectdata *conn, int8_t sockindex)
{
  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#ଅନ୍ୟଥା
bool Curl_conn_is_tunneling(ସଂରଚନା connectdata *conn, int8_t sockindex)
{
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)sockindex;
  ଫେରନ୍ତୁ FALSE;
}
#endif /* CURL_DISABLE_PROXY */

ସ୍ଥିର bool cf_is_ssl(ସଂରଚନା Curl_cfilter *cf)
{
  ପାଇଁ(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    ଯଦି((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      ଫେରନ୍ତୁ TRUE;
    ଯଦି(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ଫେରନ୍ତୁ FALSE;
  }
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_conn_is_ssl(ସଂରଚନା connectdata *conn, int8_t sockindex)
{
  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା connectdata *conn, int8_t sockindex,
                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା query,
                            ସଂରଚନା curl_tlssessioninfo *info)
{
  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(Curl_conn_is_ssl(conn, sockindex)) {
    ସଂରଚନା Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, ଖାଲି, (ଶୂନ୍ୟ *)info) :
      CURLE_UNKNOWN_OPTION;
    ଫେରନ୍ତୁ !result;
  }
  ଫେରନ୍ତୁ FALSE;
}

CURLcode Curl_conn_get_ip_info(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, ସଂରଚନା ip_quadruple *ipquad)
{
  ସଂରଚନା Curl_cfilter *cf;
  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : ଖାଲି;
  ଫେରନ୍ତୁ Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(ସଂରଚନା connectdata *conn, int8_t sockindex)
{
  ସଂରଚନା Curl_cfilter *cf;

  ଯଦି(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ FALSE;
  ପାଇଁ(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    ଯଦି(cf->cft->flags & CF_TYPE_MULTIPLEX)
      ଫେରନ୍ତୁ TRUE;
    ଯଦି(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ଫେରନ୍ତୁ FALSE;
  }
  ଫେରନ୍ତୁ FALSE;
}

unsigned ଅକ୍ଷର Curl_conn_get_transport(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ଫେରନ୍ତୁ Curl_conn_cf_get_transport(cf, data);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_socktype_for_transport(uint8_t transport)
{
  ବାଛ(transport) {
  ସ୍ଥିତି TRNSPRT_TCP:
    ଫେରନ୍ତୁ SOCK_STREAM;
  ସ୍ଥିତି TRNSPRT_UNIX:
    ଫେରନ୍ତୁ SOCK_STREAM;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* UDP and QUIC */
    ଫେରନ୍ତୁ SOCK_DGRAM;
  }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_protocol_for_transport(uint8_t transport)
{
  ବାଛ(transport) {
  ସ୍ଥିତି TRNSPRT_TCP:
    ଫେରନ୍ତୁ IPPROTO_TCP;
  ସ୍ଥିତି TRNSPRT_UNIX:
    ଫେରନ୍ତୁ IPPROTO_IP;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* UDP and QUIC */
    ଫେରନ୍ତୁ IPPROTO_UDP;
  }
}

ଅଚଳ ଅକ୍ଷର *Curl_conn_get_alpn_negotiated(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ଫେରନ୍ତୁ Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned ଅକ୍ଷର Curl_conn_http_version(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned ଅକ୍ଷର v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  ପାଇଁ(; cf; cf = cf->next) {
    ଯଦି(cf->cft->flags & CF_TYPE_HTTP) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &ମୂଲ୍ୟ, ଖାଲି);
      ଯଦି(!result && ((ମୂଲ୍ୟ < 0) || (ମୂଲ୍ୟ > 255)))
        result = CURLE_FAILED_INIT;
      ଅନ୍ୟଥା
        v = (unsigned ଅକ୍ଷର)ମୂଲ୍ୟ;
      ଭାଙ୍ଗ;
    }
    ଯଦି(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ (unsigned ଅକ୍ଷର)(result ? 0 : v);
}

bool Curl_conn_data_pending(ସଂରଚନା Curl_easy *data, int8_t sockindex)
{
  ସଂରଚନା Curl_cfilter *cf;

  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ FALSE;

  cf = data->conn->cfilter[sockindex];
  ଯେତେବେଳେ(cf && !cf->connected) {
    cf = cf->next;
  }
  ଯଦି(cf) {
    ଫେରନ୍ତୁ cf->cft->has_data_pending(cf, data);
  }
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_conn_cf_needs_flush(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data)
{
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, ଖାଲି) : CURLE_UNKNOWN_OPTION;
  ଫେରନ୍ତୁ (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(ସଂରଚନା Curl_easy *data, int8_t sockindex)
{
  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
}

CURLcode Curl_conn_cf_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  ପାଇଁ(; cf && !result; cf = cf->next) {
    ଯଦି(cf->shutdown)
      ଜାରି;
    ଯଦି(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_conn_adjust_pollset(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata *conn,
                                  ସଂରଚନା easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ bool want_io = !!ps->n;
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
  ପାଇଁ(i = 0; (i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    ଯଦି(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  ଫେରନ୍ତୁ result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_conn_cf_poll(ସଂରଚନା Curl_cfilter *cf,
                      ସଂରଚନା Curl_easy *data,
                      timediff_t timeout_ms)
{
  ସଂରଚନା easy_pollset ps;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  ଯଦି(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  ଅନ୍ୟଥା
    rc = -1;
  Curl_pollset_cleanup(&ps);
  ଫେରନ୍ତୁ rc;
}

ଶୂନ୍ୟ Curl_conn_get_current_host(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                                ଅଚଳ ଅକ୍ଷର **phost, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pport)
{
  ସଂରଚନା Curl_cfilter *cf, *cf_proxy = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା portarg = -1;

  ଯଦି(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    ଯଦି(pport)
      *pport = -1;
    ଫେରନ୍ତୁ;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : ଖାଲି;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  ଯେତେବେଳେ(cf && !cf->connected) {
    ଯଦି((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  ଯଦି(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  ଯଦି(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(ସଂରଚନା Curl_cfilter *cf,
                           ସଂରଚନା Curl_easy *data,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)event;
  (ଶୂନ୍ୟ)arg1;
  (ଶୂନ୍ୟ)arg2;
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            bool ignore_result,
                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  CURLcode result = CURLE_OK;

  ପାଇଁ(; cf; cf = cf->next) {
    ଯଦି(cf->cft->cntrl == Curl_cf_def_cntrl)
      ଜାରି;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    ଯଦି(!ignore_result && result)
      ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

curl_socket_t Curl_conn_cf_get_socket(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data)
{
  curl_socket_t sock;
  ଯଦି(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, ଖାଲି, &sock))
    ଫେରନ୍ତୁ sock;
  ଫେରନ୍ତୁ CURL_SOCKET_BAD;
}

unsigned ଅକ୍ଷର Curl_conn_cf_get_transport(ସଂରଚନା Curl_cfilter *cf,
                                         ସଂରଚନା Curl_easy *data)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା transport = 0;
  ଯଦି(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, ଖାଲି))
    ଫେରନ୍ତୁ (unsigned ଅକ୍ଷର)transport;
  ଫେରନ୍ତୁ (unsigned ଅକ୍ଷର)(data->conn ? data->conn->transport_wanted : 0);
}

ଅଚଳ ଅକ୍ଷର *Curl_conn_cf_get_alpn_negotiated(ସଂରଚନା Curl_cfilter *cf,
                                             ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ ଅକ୍ଷର *alpn = ଖାଲି;
  CURL_TRC_CF(data, cf, "query ALPN");
  ଯଦି(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, ଖାଲି,
                           CURL_UNCONST(&alpn)))
    ଫେରନ୍ତୁ alpn;
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_sockaddr_ex *cf_get_remote_addr(
  ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ ସଂରଚନା Curl_sockaddr_ex *remote_addr = ଖାଲି;
  ଯଦି(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, ଖାଲି,
                     CURL_UNCONST(&remote_addr)))
    ଫେରନ୍ତୁ remote_addr;
  ଫେରନ୍ତୁ ଖାଲି;
}

CURLcode Curl_conn_cf_get_ip_info(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  bool *is_ipv6, ସଂରଚନା ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  ଯଦି(cf) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  ଫେରନ୍ତୁ result;
}

curl_socket_t Curl_conn_get_first_socket(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cfilter *cf;

  ଯଦି(!data->conn)
    ଫେରନ୍ତୁ CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  ଯଦି(cf && !cf->connected)
    ଫେରନ୍ତୁ Curl_conn_cf_get_socket(cf, data);
  ଫେରନ୍ତୁ data->conn->sock[FIRSTSOCKET];
}

ଅଚଳ ସଂରଚନା Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  ସଂରଚନା Curl_easy *data, int8_t sockindex)
{
  ସଂରଚନା Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : ଖାଲି;
  ଫେରନ୍ତୁ cf ? cf_get_remote_addr(cf, data) : ଖାଲି;
}

CURLcode Curl_conn_ev_data_setup(ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, ଖାଲି);
}

CURLcode Curl_conn_flush(ସଂରଚନା Curl_easy *data, int8_t sockindex)
{
  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଫେରନ୍ତୁ Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
                            CF_CTRL_FLUSH, 0, ଖାଲି);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
ଶୂନ୍ୟ Curl_conn_ev_data_done_send(ସଂରଚନା Curl_easy *data)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE_SEND, 0, ଖାଲି);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
ଶୂନ୍ୟ Curl_conn_ev_data_done(ସଂରଚନା Curl_easy *data, bool premature)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE, premature, ଖାଲି);
}

CURLcode Curl_conn_ev_data_pause(ସଂରଚନା Curl_easy *data, bool do_pause)
{
  ଫେରନ୍ତୁ cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, ଖାଲି);
}

bool Curl_conn_is_alive(ସଂରଚନା Curl_easy *data, ସଂରଚନା connectdata *conn,
                        bool *ନିବେଶ_pending)
{
  ସଂରଚନା Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ଫେରନ୍ତୁ cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, ନିବେଶ_pending);
}

CURLcode Curl_conn_keep_alive(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ପାଇଁ(i = 0; (i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    ସଂରଚନା Curl_cfilter *cf = conn->cfilter[i];
    ଯଦି(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  ଫେରନ୍ତୁ result;
}

size_t Curl_conn_get_max_concurrent(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା connectdata *conn,
                                    int8_t sockindex)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = -1;

  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, ଖାଲି) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  ଫେରନ୍ତୁ (result || n < 0) ? 1 : (size_t)n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_conn_get_stream_error(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା connectdata *conn,
                               int8_t sockindex)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;

  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, ଖାଲି) : CURLE_UNKNOWN_OPTION;
  ଫେରନ୍ତୁ (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(ସଂରଚନା Curl_easy *data, curl_socket_t sockfd)
{
  ଯଦି(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    ଫେରନ୍ତୁ SECONDARYSOCKET;
  ଫେରନ୍ତୁ FIRSTSOCKET;
}

CURLcode Curl_conn_recv(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                        ଅକ୍ଷର *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଯଦି(data && data->conn && data->conn->recv[sockindex])
    ଫେରନ୍ତୁ data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  ଫେରନ୍ତୁ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                        ଅଚଳ ଶୂନ୍ୟ *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  ଯଦି(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ଅଚଳ ଅକ୍ଷର *p = getenv("CURL_SMALLSENDS");
    ଯଦି(p) {
      curl_off_t altsize;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        ଯଦି(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  ଯଦି(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    ଯଦି(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }
    ଫେରନ୍ତୁ result;
  }
  *pnwritten = 0;
  ଫେରନ୍ତୁ CURLE_FAILED_INIT;
}
