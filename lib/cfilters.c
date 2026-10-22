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
#अन्तर्भाव "strerror.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "curlx/strparse.h"

CURLcode Curl_cf_def_connect(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data, bool *done)
{
  CURLcode result;

  सचे(cf->connected) {
    *done = TRUE;
    पटिनिवत्त CURLE_OK;
  }

  सचे(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    सचे(result || !*done)
      पटिनिवत्त result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data, bool *done)
{
  (शून्य)cf;
  (शून्य)data;
  *done = TRUE;
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना easy_pollset *ps)
{
  /* NOP */
  (शून्य)cf;
  (शून्य)data;
  (शून्य)ps;
  पटिनिवत्त CURLE_OK;
}

bool Curl_cf_def_data_pending(संरचना Curl_cfilter *cf,
                              ध्रुव संरचना Curl_easy *data)
{
  पटिनिवत्त cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                          ध्रुव uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  सचे(cf->next)
    पटिनिवत्त cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  पटिनिवत्त CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                          वर्ण *buf, size_t len, size_t *pnread)
{
  सचे(cf->next)
    पटिनिवत्त cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  पटिनिवत्त CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               bool *निवेशः_pending)
{
  पटिनिवत्त cf->next ?
    cf->next->cft->is_alive(cf->next, data, निवेशः_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data)
{
  पटिनिवत्त cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  पटिनिवत्त cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
शून्य Curl_conn_trc_filters(संरचना Curl_easy *data,
                           int8_t sockindex, ध्रुव वर्ण *info)
{
  सचे(CURL_TRC_M_is_verbose(data) && data->conn) {
    संरचना Curl_cfilter *cf = data->conn->cfilter[sockindex];

    सचे(cf) {
      वर्ण सन्देशः[256], *buf;
      पूर्णाङ्क blen, n;

      buf = सन्देशः;
      blen = परिमाण(सन्देशः) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      अत्थाय(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", सन्देशः, blen ? "" : "...");
    }
    अञ्ञथा
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

शून्य Curl_conn_cf_discard_chain(संरचना Curl_cfilter **pcf,
                                संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cfn, *cf = *pcf;

  सचे(cf) {
    *pcf = रिक्त;
    याव(cf) {
      cfn = cf->next;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->next = रिक्त;
      cf->cft->destroy(cf, data);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

शून्य Curl_conn_cf_discard_all(संरचना Curl_easy *data,
                              संरचना connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], data);
}

CURLcode Curl_cf_recv(संरचना Curl_easy *data, int8_t sockindex, वर्ण *buf,
                      size_t len, size_t *pnread)
{
  संरचना Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  याव(cf && !cf->connected)
    cf = cf->next;
  सचे(cf)
    पटिनिवत्त cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  पटिनिवत्त CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(संरचना Curl_easy *data, int8_t sockindex,
                      ध्रुव uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  संरचना Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  याव(cf && !cf->connected)
    cf = cf->next;
  सचे(cf) {
    पटिनिवत्त cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  पटिनिवत्त CURLE_FAILED_INIT;
}

संरचना cf_io_ctx {
  संरचना Curl_easy *data;
  संरचना Curl_cfilter *cf;
};

स्थिर CURLcode cf_bufq_reader(शून्य *writer_ctx,
                               unsigned वर्ण *buf, size_t blen,
                               size_t *pnread)
{
  संरचना cf_io_ctx *io = writer_ctx;
  पटिनिवत्त Curl_conn_cf_recv(io->cf, io->data, (वर्ण *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           संरचना bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  संरचना cf_io_ctx io;

  सचे(!cf || !data) {
    *pnread = 0;
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  पटिनिवत्त Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

स्थिर CURLcode cf_bufq_writer(शून्य *writer_ctx,
                               ध्रुव uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  संरचना cf_io_ctx *io = writer_ctx;
  पटिनिवत्त Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           संरचना bufq *bufq,
                           ध्रुव unsigned वर्ण *buf, size_t blen,
                           size_t *pnwritten)
{
  संरचना cf_io_ctx io;

  सचे(!cf || !data) {
    *pnwritten = 0;
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  सचे(buf && blen)
    पटिनिवत्त Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  अञ्ञथा
    पटिनिवत्त Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(संरचना Curl_cfilter **pcf,
                        ध्रुव संरचना Curl_cftype *cft,
                        शून्य *ctx)
{
  संरचना Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, परिमाण(*cf));
  सचे(!cf)
    गच्छ out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  पटिनिवत्त result;
}

शून्य Curl_conn_cf_add(संरचना Curl_easy *data,
                      संरचना connectdata *conn,
                      int8_t sockindex,
                      संरचना Curl_cfilter *cf)
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

शून्य Curl_conn_cf_निवेशय_after(संरचना Curl_cfilter *cf_at,
                               संरचना Curl_cfilter *cf_new)
{
  संरचना Curl_cfilter *tail, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  tail = cf_at->next;
  cf_at->next = cf_new;
  करोहि {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->next;
    cf_new = cf_new->next;
  } याव(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(संरचना Curl_cfilter **pcf,
                          संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cf = pcf ? *pcf : रिक्त;
  bool found = FALSE;
  सचे(cf) {
    सचे(cf->conn) {
      /* unlink if present in connection filter chain */
      संरचना Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      याव(*pprev) {
        सचे(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = रिक्त;
          found = TRUE;
          भङ्ग;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  पटिनिवत्त found;
}

CURLcode Curl_conn_cf_connect(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              bool *done)
{
  सचे(cf)
    पटिनिवत्त cf->cft->do_connect(cf, data, done);
  पटिनिवत्त CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                           ध्रुव uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  सचे(cf)
    पटिनिवत्त cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  पटिनिवत्त CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                           वर्ण *buf, size_t len, size_t *pnread)
{
  सचे(cf)
    पटिनिवत्त cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  पटिनिवत्त CURLE_RECV_ERROR;
}

स्थिर CURLcode cf_cntrl_all(संरचना connectdata *conn,
                             संरचना Curl_easy *data,
                             bool ignore_result,
                             पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  CURLcode result = CURLE_OK;
  पूर्णाङ्क i;

  अत्थाय(i = 0; i < (पूर्णाङ्क)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    सचे(!ignore_result && result)
      भङ्ग;
  }
  पटिनिवत्त result;
}

bool Curl_conn_cf_is_ip_connected(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  सचे(cf) {
    पूर्णाङ्क मानम् = 0;
    सचे(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &मानम्, रिक्त))
      पटिनिवत्त !!मानम्;
  }
  पटिनिवत्त FALSE;
}

शून्य Curl_conn_cntrl_update_info(संरचना Curl_easy *data,
                                संरचना connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, रिक्त);
}

शून्य Curl_conn_cntrl_report_stats(संरचना Curl_easy *data,
                                  संरचना connectdata *conn,
                                  पूर्णाङ्क sockindex)
{
  सचे((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (शून्य)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, रिक्त);
}

शून्य Curl_conn_remove_setup_filters(संरचना Curl_easy *data,
                                    int8_t sockindex)
{
  संरचना Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  याव(*anchor) {
    संरचना Curl_cfilter *cf = *anchor;
    सचे(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = रिक्त;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    अञ्ञथा
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(संरचना connectdata *conn, int8_t sockindex)
{
  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त FALSE;
  पटिनिवत्त !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(संरचना connectdata *conn, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त FALSE;
  cf = conn->cfilter[sockindex];
  सचे(cf)
    पटिनिवत्त (bool)cf->connected;
  अञ्ञथा सचे(conn->scheme->flags & PROTOPT_NONETWORK)
    पटिनिवत्त TRUE;
  पटिनिवत्त FALSE;
}

bool Curl_conn_is_ip_connected(संरचना Curl_easy *data, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त FALSE;
  cf = data->conn->cfilter[sockindex];
  याव(cf) {
    सचे(cf->connected)
      पटिनिवत्त TRUE;
    सचे(cf->cft->flags & CF_TYPE_IP_CONNECT)
      पटिनिवत्त FALSE;
    cf = cf->next;
  }
  पटिनिवत्त FALSE;
}

#ifndef CURL_DISABLE_PROXY
स्थिर bool cf_is_tunneling(संरचना Curl_cfilter *cf)
{
  अत्थाय(; cf; cf = cf->next) {
    सचे((cf->cft->flags & CF_TYPE_PROXY))
      पटिनिवत्त TRUE;
  }
  पटिनिवत्त FALSE;
}

bool Curl_conn_is_tunneling(संरचना connectdata *conn, int8_t sockindex)
{
  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त FALSE;
  पटिनिवत्त conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#अञ्ञथा
bool Curl_conn_is_tunneling(संरचना connectdata *conn, int8_t sockindex)
{
  (शून्य)conn;
  (शून्य)sockindex;
  पटिनिवत्त FALSE;
}
#endif /* CURL_DISABLE_PROXY */

स्थिर bool cf_is_ssl(संरचना Curl_cfilter *cf)
{
  अत्थाय(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    सचे((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      पटिनिवत्त TRUE;
    सचे(cf->cft->flags & CF_TYPE_IP_CONNECT)
      पटिनिवत्त FALSE;
  }
  पटिनिवत्त FALSE;
}

bool Curl_conn_is_ssl(संरचना connectdata *conn, int8_t sockindex)
{
  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त FALSE;
  पटिनिवत्त conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(संरचना Curl_easy *data,
                            संरचना connectdata *conn, int8_t sockindex,
                            पूर्णाङ्क query,
                            संरचना curl_tlssessioninfo *info)
{
  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त FALSE;
  सचे(Curl_conn_is_ssl(conn, sockindex)) {
    संरचना Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, रिक्त, (शून्य *)info) :
      CURLE_UNKNOWN_OPTION;
    पटिनिवत्त !result;
  }
  पटिनिवत्त FALSE;
}

CURLcode Curl_conn_get_ip_info(संरचना Curl_easy *data,
                               संरचना connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, संरचना ip_quadruple *ipquad)
{
  संरचना Curl_cfilter *cf;
  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : रिक्त;
  पटिनिवत्त Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(संरचना connectdata *conn, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  सचे(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त FALSE;
  अत्थाय(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    सचे(cf->cft->flags & CF_TYPE_MULTIPLEX)
      पटिनिवत्त TRUE;
    सचे(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      पटिनिवत्त FALSE;
  }
  पटिनिवत्त FALSE;
}

unsigned वर्ण Curl_conn_get_transport(संरचना Curl_easy *data,
                                      संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  पटिनिवत्त Curl_conn_cf_get_transport(cf, data);
}

पूर्णाङ्क Curl_socktype_for_transport(uint8_t transport)
{
  विकल्प(transport) {
  पक्ष TRNSPRT_TCP:
    पटिनिवत्त SOCK_STREAM;
  पक्ष TRNSPRT_UNIX:
    पटिनिवत्त SOCK_STREAM;
  स्वभाव: /* UDP and QUIC */
    पटिनिवत्त SOCK_DGRAM;
  }
}

पूर्णाङ्क Curl_protocol_for_transport(uint8_t transport)
{
  विकल्प(transport) {
  पक्ष TRNSPRT_TCP:
    पटिनिवत्त IPPROTO_TCP;
  पक्ष TRNSPRT_UNIX:
    पटिनिवत्त IPPROTO_IP;
  स्वभाव: /* UDP and QUIC */
    पटिनिवत्त IPPROTO_UDP;
  }
}

ध्रुव वर्ण *Curl_conn_get_alpn_negotiated(संरचना Curl_easy *data,
                                          संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  पटिनिवत्त Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned वर्ण Curl_conn_http_version(संरचना Curl_easy *data,
                                     संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned वर्ण v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  अत्थाय(; cf; cf = cf->next) {
    सचे(cf->cft->flags & CF_TYPE_HTTP) {
      पूर्णाङ्क मानम् = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &मानम्, रिक्त);
      सचे(!result && ((मानम् < 0) || (मानम् > 255)))
        result = CURLE_FAILED_INIT;
      अञ्ञथा
        v = (unsigned वर्ण)मानम्;
      भङ्ग;
    }
    सचे(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      भङ्ग;
  }
  पटिनिवत्त (unsigned वर्ण)(result ? 0 : v);
}

bool Curl_conn_data_pending(संरचना Curl_easy *data, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  (शून्य)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त FALSE;

  cf = data->conn->cfilter[sockindex];
  याव(cf && !cf->connected) {
    cf = cf->next;
  }
  सचे(cf) {
    पटिनिवत्त cf->cft->has_data_pending(cf, data);
  }
  पटिनिवत्त FALSE;
}

bool Curl_conn_cf_needs_flush(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data)
{
  CURLcode result;
  पूर्णाङ्क pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, रिक्त) : CURLE_UNKNOWN_OPTION;
  पटिनिवत्त (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(संरचना Curl_easy *data, int8_t sockindex)
{
  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त FALSE;
  पटिनिवत्त Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
}

CURLcode Curl_conn_cf_adjust_pollset(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data,
                                     संरचना easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  अत्थाय(; cf && !result; cf = cf->next) {
    सचे(cf->shutdown)
      अनुवर्त;
    सचे(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  पटिनिवत्त result;
}

CURLcode Curl_conn_adjust_pollset(संरचना Curl_easy *data,
                                  संरचना connectdata *conn,
                                  संरचना easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  ध्रुव bool want_io = !!ps->n;
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
  अत्थाय(i = 0; (i < (पूर्णाङ्क)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    सचे(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  पटिनिवत्त result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
पूर्णाङ्क Curl_conn_cf_poll(संरचना Curl_cfilter *cf,
                      संरचना Curl_easy *data,
                      timediff_t timeout_ms)
{
  संरचना easy_pollset ps;
  पूर्णाङ्क rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  सचे(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  अञ्ञथा
    rc = -1;
  Curl_pollset_cleanup(&ps);
  पटिनिवत्त rc;
}

शून्य Curl_conn_get_current_host(संरचना Curl_easy *data, int8_t sockindex,
                                ध्रुव वर्ण **phost, पूर्णाङ्क *pport)
{
  संरचना Curl_cfilter *cf, *cf_proxy = रिक्त;
  पूर्णाङ्क portarg = -1;

  सचे(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    सचे(pport)
      *pport = -1;
    पटिनिवत्त;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : रिक्त;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  याव(cf && !cf->connected) {
    सचे((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  सचे(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  सचे(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  (शून्य)cf;
  (शून्य)data;
  (शून्य)event;
  (शून्य)arg1;
  (शून्य)arg2;
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            bool ignore_result,
                            पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  CURLcode result = CURLE_OK;

  अत्थाय(; cf; cf = cf->next) {
    सचे(cf->cft->cntrl == Curl_cf_def_cntrl)
      अनुवर्त;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    सचे(!ignore_result && result)
      भङ्ग;
  }
  पटिनिवत्त result;
}

curl_socket_t Curl_conn_cf_get_socket(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data)
{
  curl_socket_t sock;
  सचे(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, रिक्त, &sock))
    पटिनिवत्त sock;
  पटिनिवत्त CURL_SOCKET_BAD;
}

unsigned वर्ण Curl_conn_cf_get_transport(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data)
{
  पूर्णाङ्क transport = 0;
  सचे(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, रिक्त))
    पटिनिवत्त (unsigned वर्ण)transport;
  पटिनिवत्त (unsigned वर्ण)(data->conn ? data->conn->transport_wanted : 0);
}

ध्रुव वर्ण *Curl_conn_cf_get_alpn_negotiated(संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *data)
{
  ध्रुव वर्ण *alpn = रिक्त;
  CURL_TRC_CF(data, cf, "query ALPN");
  सचे(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, रिक्त,
                           CURL_UNCONST(&alpn)))
    पटिनिवत्त alpn;
  पटिनिवत्त रिक्त;
}

स्थिर ध्रुव संरचना Curl_sockaddr_ex *cf_get_remote_addr(
  संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  ध्रुव संरचना Curl_sockaddr_ex *remote_addr = रिक्त;
  सचे(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, रिक्त,
                     CURL_UNCONST(&remote_addr)))
    पटिनिवत्त remote_addr;
  पटिनिवत्त रिक्त;
}

CURLcode Curl_conn_cf_get_ip_info(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  bool *is_ipv6, संरचना ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  सचे(cf) {
    पूर्णाङ्क ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  पटिनिवत्त result;
}

curl_socket_t Curl_conn_get_first_socket(संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cf;

  सचे(!data->conn)
    पटिनिवत्त CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  सचे(cf && !cf->connected)
    पटिनिवत्त Curl_conn_cf_get_socket(cf, data);
  पटिनिवत्त data->conn->sock[FIRSTSOCKET];
}

ध्रुव संरचना Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  संरचना Curl_easy *data, int8_t sockindex)
{
  संरचना Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : रिक्त;
  पटिनिवत्त cf ? cf_get_remote_addr(cf, data) : रिक्त;
}

CURLcode Curl_conn_ev_data_setup(संरचना Curl_easy *data)
{
  पटिनिवत्त cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, रिक्त);
}

CURLcode Curl_conn_flush(संरचना Curl_easy *data, int8_t sockindex)
{
  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  पटिनिवत्त Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
                            CF_CTRL_FLUSH, 0, रिक्त);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
शून्य Curl_conn_ev_data_done_send(संरचना Curl_easy *data)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE_SEND, 0, रिक्त);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
शून्य Curl_conn_ev_data_done(संरचना Curl_easy *data, bool premature)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE, premature, रिक्त);
}

CURLcode Curl_conn_ev_data_pause(संरचना Curl_easy *data, bool do_pause)
{
  पटिनिवत्त cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, रिक्त);
}

bool Curl_conn_is_alive(संरचना Curl_easy *data, संरचना connectdata *conn,
                        bool *निवेशः_pending)
{
  संरचना Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  पटिनिवत्त cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, निवेशः_pending);
}

CURLcode Curl_conn_keep_alive(संरचना Curl_easy *data,
                              संरचना connectdata *conn)
{
  CURLcode result = CURLE_OK;
  पूर्णाङ्क i;

  अत्थाय(i = 0; (i < (पूर्णाङ्क)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    संरचना Curl_cfilter *cf = conn->cfilter[i];
    सचे(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  पटिनिवत्त result;
}

size_t Curl_conn_get_max_concurrent(संरचना Curl_easy *data,
                                    संरचना connectdata *conn,
                                    int8_t sockindex)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;
  पूर्णाङ्क n = -1;

  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, रिक्त) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  पटिनिवत्त (result || n < 0) ? 1 : (size_t)n;
}

पूर्णाङ्क Curl_conn_get_stream_error(संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               int8_t sockindex)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;
  पूर्णाङ्क n = 0;

  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, रिक्त) : CURLE_UNKNOWN_OPTION;
  पटिनिवत्त (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(संरचना Curl_easy *data, curl_socket_t sockfd)
{
  सचे(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    पटिनिवत्त SECONDARYSOCKET;
  पटिनिवत्त FIRSTSOCKET;
}

CURLcode Curl_conn_recv(संरचना Curl_easy *data, int8_t sockindex,
                        वर्ण *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  सचे(data && data->conn && data->conn->recv[sockindex])
    पटिनिवत्त data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  पटिनिवत्त CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(संरचना Curl_easy *data, int8_t sockindex,
                        ध्रुव शून्य *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  सचे(!CONN_SOCK_IDX_VALID(sockindex))
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  सचे(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ध्रुव वर्ण *p = getenv("CURL_SMALLSENDS");
    सचे(p) {
      curl_off_t altsize;
      सचे(!curlx_str_सङ्ख्या(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        सचे(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  सचे(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    सचे(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      पटिनिवत्त CURLE_SEND_ERROR;
    }
    पटिनिवत्त result;
  }
  *pnwritten = 0;
  पटिनिवत्त CURLE_FAILED_INIT;
}
