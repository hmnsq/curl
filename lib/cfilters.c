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
                              संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  यदि(cf->अग्र) {
    परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, दत्त, समाप्तम्);
    यदि(परिणाम || !*समाप्तम्)
      प्रत्यागम परिणाम;
  }

  cf->connected = TRUE;
  *समाप्तम् = TRUE;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  (शून्य)cf;
  (शून्य)दत्त;
  *समाप्तम् = TRUE;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    संरचना easy_pollset *ps)
{
  /* NOP */
  (शून्य)cf;
  (शून्य)दत्त;
  (शून्य)ps;
  प्रत्यागम CURLE_OK;
}

bool Curl_cf_def_दत्त_pending(संरचना Curl_cfilter *cf,
                              ध्रुव संरचना Curl_easy *दत्त)
{
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->has_दत्त_pending(cf->अग्र, दत्त) : FALSE;
}

CURLcode Curl_cf_def_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                          ध्रुव uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  यदि(cf->अग्र)
    प्रत्यागम cf->अग्र->cft->do_send(cf->अग्र, दत्त, buf, len, eos, pnwritten);
  *pnwritten = 0;
  प्रत्यागम CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                          वर्ण *buf, size_t len, size_t *pnread)
{
  यदि(cf->अग्र)
    प्रत्यागम cf->अग्र->cft->do_recv(cf->अग्र, दत्त, buf, len, pnread);
  *pnread = 0;
  प्रत्यागम CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               bool *निवेशः_pending)
{
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->is_alive(cf->अग्र, दत्त, निवेशः_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त)
{
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->keep_alive(cf->अग्र, दत्त) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *दत्त,
                           पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#यदि_परिभाषितम् CURLVERBOSE
शून्य Curl_conn_trc_filters(संरचना Curl_easy *दत्त,
                           int8_t sockindex, ध्रुव वर्ण *info)
{
  यदि(CURL_TRC_M_is_verbose(दत्त) && दत्त->conn) {
    संरचना Curl_cfilter *cf = दत्त->conn->cfilter[sockindex];

    यदि(cf) {
      वर्ण सन्देशः[256], *buf;
      पूर्णाङ्क blen, n;

      buf = सन्देशः;
      blen = परिमाण(सन्देशः) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      प्रति(; cf && blen; cf = cf->अग्र) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(दत्त, "%s%s", सन्देशः, blen ? "" : "...");
    }
    अन्यथा
      CURL_TRC_M(दत्त, "%s [%d][-]", info, sockindex);
  }
}
#यदि_अन्तः /* CURLVERBOSE */

शून्य Curl_conn_cf_discard_chain(संरचना Curl_cfilter **pcf,
                                संरचना Curl_easy *दत्त)
{
  संरचना Curl_cfilter *cfn, *cf = *pcf;

  यदि(cf) {
    *pcf = रिक्त;
    यावत्(cf) {
      cfn = cf->अग्र;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->अग्र = रिक्त;
      cf->cft->destroy(cf, दत्त);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

शून्य Curl_conn_cf_discard_all(संरचना Curl_easy *दत्त,
                              संरचना connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.समुच्चयः[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], दत्त);
}

CURLcode Curl_cf_recv(संरचना Curl_easy *दत्त, int8_t sockindex, वर्ण *buf,
                      size_t len, size_t *pnread)
{
  संरचना Curl_cfilter *cf;

  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);
  cf = दत्त->conn->cfilter[sockindex];
  यावत्(cf && !cf->connected)
    cf = cf->अग्र;
  यदि(cf)
    प्रत्यागम cf->cft->do_recv(cf, दत्त, buf, len, pnread);
  failf(दत्त, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  प्रत्यागम CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(संरचना Curl_easy *दत्त, int8_t sockindex,
                      ध्रुव uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  संरचना Curl_cfilter *cf;

  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);
  cf = दत्त->conn->cfilter[sockindex];
  यावत्(cf && !cf->connected)
    cf = cf->अग्र;
  यदि(cf) {
    प्रत्यागम cf->cft->do_send(cf, दत्त, buf, len, eos, pnwritten);
  }
  failf(दत्त, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  प्रत्यागम CURLE_FAILED_INIT;
}

संरचना cf_io_ctx {
  संरचना Curl_easy *दत्त;
  संरचना Curl_cfilter *cf;
};

स्थिर CURLcode cf_bufq_reader(शून्य *writer_ctx,
                               अचिह्नित वर्ण *buf, size_t blen,
                               size_t *pnread)
{
  संरचना cf_io_ctx *io = writer_ctx;
  प्रत्यागम Curl_conn_cf_recv(io->cf, io->दत्त, (वर्ण *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *दत्त,
                           संरचना bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  संरचना cf_io_ctx io;

  यदि(!cf || !दत्त) {
    *pnread = 0;
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
  io.दत्त = दत्त;
  io.cf = cf;
  प्रत्यागम Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

स्थिर CURLcode cf_bufq_writer(शून्य *writer_ctx,
                               ध्रुव uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  संरचना cf_io_ctx *io = writer_ctx;
  प्रत्यागम Curl_conn_cf_send(io->cf, io->दत्त, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *दत्त,
                           संरचना bufq *bufq,
                           ध्रुव अचिह्नित वर्ण *buf, size_t blen,
                           size_t *pnwritten)
{
  संरचना cf_io_ctx io;

  यदि(!cf || !दत्त) {
    *pnwritten = 0;
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
  io.दत्त = दत्त;
  io.cf = cf;
  यदि(buf && blen)
    प्रत्यागम Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  अन्यथा
    प्रत्यागम Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(संरचना Curl_cfilter **pcf,
                        ध्रुव संरचना Curl_cftype *cft,
                        शून्य *ctx)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, परिमाण(*cf));
  यदि(!cf)
    गच्छ बहिर्गमनम्;

  cf->cft = cft;
  cf->ctx = ctx;
  परिणाम = CURLE_OK;
बहिर्गमनम्:
  *pcf = cf;
  प्रत्यागम परिणाम;
}

शून्य Curl_conn_cf_सङ्कलय(संरचना Curl_easy *दत्त,
                      संरचना connectdata *conn,
                      int8_t sockindex,
                      संरचना Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->अग्र);

  cf->अग्र = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(दत्त, cf, "added");
}

शून्य Curl_conn_cf_निवेशय_after(संरचना Curl_cfilter *cf_at,
                               संरचना Curl_cfilter *cf_new)
{
  संरचना Curl_cfilter *पुच्छ, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  पुच्छ = cf_at->अग्र;
  cf_at->अग्र = cf_new;
  कुरु {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->अग्र;
    cf_new = cf_new->अग्र;
  } यावत्(cf_new);
  *pnext = पुच्छ;
}

bool Curl_conn_cf_discard(संरचना Curl_cfilter **pcf,
                          संरचना Curl_easy *दत्त)
{
  संरचना Curl_cfilter *cf = pcf ? *pcf : रिक्त;
  bool प्राप्तम् = FALSE;
  यदि(cf) {
    यदि(cf->conn) {
      /* unlink if present in connection filter chain */
      संरचना Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      यावत्(*pprev) {
        यदि(*pprev == *pcf) {
          *pprev = (*pcf)->अग्र;
          cf->अग्र = रिक्त;
          प्राप्तम् = TRUE;
          भङ्ग;
        }
        pprev = &(*pprev)->अग्र;
      }
    }
    Curl_conn_cf_discard_chain(pcf, दत्त);
  }
  प्रत्यागम प्राप्तम्;
}

CURLcode Curl_conn_cf_connect(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              bool *समाप्तम्)
{
  यदि(cf)
    प्रत्यागम cf->cft->do_connect(cf, दत्त, समाप्तम्);
  प्रत्यागम CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                           ध्रुव uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  यदि(cf)
    प्रत्यागम cf->cft->do_send(cf, दत्त, buf, len, eos, pnwritten);
  *pnwritten = 0;
  प्रत्यागम CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                           वर्ण *buf, size_t len, size_t *pnread)
{
  यदि(cf)
    प्रत्यागम cf->cft->do_recv(cf, दत्त, buf, len, pnread);
  *pnread = 0;
  प्रत्यागम CURLE_RECV_ERROR;
}

स्थिर CURLcode cf_cntrl_all(संरचना connectdata *conn,
                             संरचना Curl_easy *दत्त,
                             bool ignore_परिणाम,
                             पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क i;

  प्रति(i = 0; i < (पूर्णाङ्क)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    परिणाम = Curl_conn_cf_cntrl(conn->cfilter[i], दत्त, ignore_परिणाम,
                                event, arg1, arg2);
    यदि(!ignore_परिणाम && परिणाम)
      भङ्ग;
  }
  प्रत्यागम परिणाम;
}

bool Curl_conn_cf_is_ip_connected(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त)
{
  यदि(cf) {
    पूर्णाङ्क मानम् = 0;
    यदि(!cf->cft->query(cf, दत्त, CF_QUERY_REALLY_CONNECTED, &मानम्, रिक्त))
      प्रत्यागम !!मानम्;
  }
  प्रत्यागम FALSE;
}

शून्य Curl_conn_cntrl_update_info(संरचना Curl_easy *दत्त,
                                संरचना connectdata *conn)
{
  cf_cntrl_all(conn, दत्त, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, रिक्त);
}

शून्य Curl_conn_cntrl_report_stats(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn,
                                  पूर्णाङ्क sockindex)
{
  यदि((अचिह्नित)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (शून्य)Curl_conn_cf_cntrl(conn->cfilter[sockindex], दत्त, TRUE,
                             CF_CTRL_REPORT_STATS, 0, रिक्त);
}

शून्य Curl_conn_remove_सज्जता_filters(संरचना Curl_easy *दत्त,
                                    int8_t sockindex)
{
  संरचना Curl_cfilter **anchor = &दत्त->conn->cfilter[sockindex];
  यावत्(*anchor) {
    संरचना Curl_cfilter *cf = *anchor;
    यदि(cf->connected && (cf->cft->flags & CF_TYPE_सज्जता)) {
      *anchor = cf->अग्र;
      cf->अग्र = रिक्त;
      CURL_TRC_CF(दत्त, cf, "removing connected setup filter");
      cf->cft->destroy(cf, दत्त);
      curlx_free(cf);
    }
    अन्यथा
      anchor = &cf->अग्र;
  }
}

bool Curl_conn_is_सज्जता(संरचना connectdata *conn, int8_t sockindex)
{
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम FALSE;
  प्रत्यागम !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(संरचना connectdata *conn, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम FALSE;
  cf = conn->cfilter[sockindex];
  यदि(cf)
    प्रत्यागम (bool)cf->connected;
  अन्यथा यदि(conn->scheme->flags & PROTOPT_NONETWORK)
    प्रत्यागम TRUE;
  प्रत्यागम FALSE;
}

bool Curl_conn_is_ip_connected(संरचना Curl_easy *दत्त, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम FALSE;
  cf = दत्त->conn->cfilter[sockindex];
  यावत्(cf) {
    यदि(cf->connected)
      प्रत्यागम TRUE;
    यदि(cf->cft->flags & CF_TYPE_IP_CONNECT)
      प्रत्यागम FALSE;
    cf = cf->अग्र;
  }
  प्रत्यागम FALSE;
}

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
स्थिर bool cf_is_tunneling(संरचना Curl_cfilter *cf)
{
  प्रति(; cf; cf = cf->अग्र) {
    यदि((cf->cft->flags & CF_TYPE_PROXY))
      प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

bool Curl_conn_is_tunneling(संरचना connectdata *conn, int8_t sockindex)
{
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम FALSE;
  प्रत्यागम conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#अन्यथा
bool Curl_conn_is_tunneling(संरचना connectdata *conn, int8_t sockindex)
{
  (शून्य)conn;
  (शून्य)sockindex;
  प्रत्यागम FALSE;
}
#यदि_अन्तः /* CURL_DISABLE_PROXY */

स्थिर bool cf_is_ssl(संरचना Curl_cfilter *cf)
{
  प्रति(; cf; cf = cf->अग्र) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    यदि((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      प्रत्यागम TRUE;
    यदि(cf->cft->flags & CF_TYPE_IP_CONNECT)
      प्रत्यागम FALSE;
  }
  प्रत्यागम FALSE;
}

bool Curl_conn_is_ssl(संरचना connectdata *conn, int8_t sockindex)
{
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम FALSE;
  प्रत्यागम conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(संरचना Curl_easy *दत्त,
                            संरचना connectdata *conn, int8_t sockindex,
                            पूर्णाङ्क query,
                            संरचना curl_tlssessioninfo *info)
{
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम FALSE;
  यदि(Curl_conn_is_ssl(conn, sockindex)) {
    संरचना Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode परिणाम = cf ?
      cf->cft->query(cf, दत्त, query, रिक्त, (शून्य *)info) :
      CURLE_UNKNOWN_OPTION;
    प्रत्यागम !परिणाम;
  }
  प्रत्यागम FALSE;
}

CURLcode Curl_conn_get_ip_info(संरचना Curl_easy *दत्त,
                               संरचना connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, संरचना ip_quadruple *ipquad)
{
  संरचना Curl_cfilter *cf;
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  cf = conn ? conn->cfilter[sockindex] : रिक्त;
  प्रत्यागम Curl_conn_cf_get_ip_info(cf, दत्त, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(संरचना connectdata *conn, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  यदि(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम FALSE;
  प्रति(cf = conn->cfilter[sockindex]; cf; cf = cf->अग्र) {
    यदि(cf->cft->flags & CF_TYPE_MULTIPLEX)
      प्रत्यागम TRUE;
    यदि(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      प्रत्यागम FALSE;
  }
  प्रत्यागम FALSE;
}

अचिह्नित वर्ण Curl_conn_get_transport(संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  प्रत्यागम Curl_conn_cf_get_transport(cf, दत्त);
}

पूर्णाङ्क Curl_socktype_for_transport(uint8_t transport)
{
  विकल्प(transport) {
  पक्ष TRNSPRT_TCP:
    प्रत्यागम SOCK_STREAM;
  पक्ष TRNSPRT_UNIX:
    प्रत्यागम SOCK_STREAM;
  स्वभाव: /* UDP and QUIC */
    प्रत्यागम SOCK_DGRAM;
  }
}

पूर्णाङ्क Curl_protocol_for_transport(uint8_t transport)
{
  विकल्प(transport) {
  पक्ष TRNSPRT_TCP:
    प्रत्यागम IPPROTO_TCP;
  पक्ष TRNSPRT_UNIX:
    प्रत्यागम IPPROTO_IP;
  स्वभाव: /* UDP and QUIC */
    प्रत्यागम IPPROTO_UDP;
  }
}

ध्रुव वर्ण *Curl_conn_get_alpn_negotiated(संरचना Curl_easy *दत्त,
                                          संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  प्रत्यागम Curl_conn_cf_get_alpn_negotiated(cf, दत्त);
}

अचिह्नित वर्ण Curl_conn_http_संस्करणम्(संरचना Curl_easy *दत्त,
                                     संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_UNKNOWN_OPTION;
  अचिह्नित वर्ण v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  प्रति(; cf; cf = cf->अग्र) {
    यदि(cf->cft->flags & CF_TYPE_HTTP) {
      पूर्णाङ्क मानम् = 0;
      परिणाम = cf->cft->query(cf, दत्त, CF_QUERY_HTTP_संस्करणम्, &मानम्, रिक्त);
      यदि(!परिणाम && ((मानम् < 0) || (मानम् > 255)))
        परिणाम = CURLE_FAILED_INIT;
      अन्यथा
        v = (अचिह्नित वर्ण)मानम्;
      भङ्ग;
    }
    यदि(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      भङ्ग;
  }
  प्रत्यागम (अचिह्नित वर्ण)(परिणाम ? 0 : v);
}

bool Curl_conn_दत्त_pending(संरचना Curl_easy *दत्त, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  (शून्य)दत्त;
  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम FALSE;

  cf = दत्त->conn->cfilter[sockindex];
  यावत्(cf && !cf->connected) {
    cf = cf->अग्र;
  }
  यदि(cf) {
    प्रत्यागम cf->cft->has_दत्त_pending(cf, दत्त);
  }
  प्रत्यागम FALSE;
}

bool Curl_conn_cf_needs_flush(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम;
  पूर्णाङ्क pending = 0;
  परिणाम = cf ? cf->cft->query(cf, दत्त, CF_QUERY_NEED_FLUSH,
                               &pending, रिक्त) : CURLE_UNKNOWN_OPTION;
  प्रत्यागम (परिणाम || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(संरचना Curl_easy *दत्त, int8_t sockindex)
{
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम FALSE;
  प्रत्यागम Curl_conn_cf_needs_flush(दत्त->conn->cfilter[sockindex], दत्त);
}

CURLcode Curl_conn_cf_adjust_pollset(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त,
                                     संरचना easy_pollset *ps)
{
  CURLcode परिणाम = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  प्रति(; cf && !परिणाम; cf = cf->अग्र) {
    यदि(cf->shutdown)
      अनुवर्त;
    यदि(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->अग्र || cf->अग्र->connected)
      परिणाम = cf->cft->adjust_pollset(cf, दत्त, ps);
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_conn_adjust_pollset(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn,
                                  संरचना easy_pollset *ps)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव bool want_io = !!ps->n;
  int8_t i;

  DEBUGASSERT(दत्त);
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
  प्रति(i = 0; (i < (पूर्णाङ्क)CURL_ARRAYSIZE(conn->cfilter)) && !परिणाम; ++i) {
    यदि(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_अन्तर्_SHUTDOWN(conn, i)))
      परिणाम = Curl_conn_cf_adjust_pollset(conn->cfilter[i], दत्त, ps);
  }
  प्रत्यागम परिणाम;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
पूर्णाङ्क Curl_conn_cf_poll(संरचना Curl_cfilter *cf,
                      संरचना Curl_easy *दत्त,
                      timediff_t timeout_ms)
{
  संरचना easy_pollset ps;
  पूर्णाङ्क rc;
  CURLcode परिणाम;

  DEBUGASSERT(cf);
  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);
  Curl_pollset_init(&ps);

  परिणाम = Curl_conn_cf_adjust_pollset(cf, दत्त, &ps);
  यदि(!परिणाम)
    rc = Curl_pollset_poll(दत्त, &ps, timeout_ms);
  अन्यथा
    rc = -1;
  Curl_pollset_cleanup(&ps);
  प्रत्यागम rc;
}

शून्य Curl_conn_get_current_host(संरचना Curl_easy *दत्त, int8_t sockindex,
                                ध्रुव वर्ण **phost, पूर्णाङ्क *pport)
{
  संरचना Curl_cfilter *cf, *cf_proxy = रिक्त;
  पूर्णाङ्क portarg = -1;

  यदि(!दत्त->conn) {
    DEBUGASSERT(0);
    *phost = "";
    यदि(pport)
      *pport = -1;
    प्रत्यागम;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? दत्त->conn->cfilter[sockindex] : रिक्त;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  यावत्(cf && !cf->connected) {
    यदि((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->अग्र;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  यदि(!cf_proxy || cf_proxy->cft->query(cf_proxy, दत्त, CF_QUERY_HOST_पत्तनम्,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = दत्त->conn->origin->hostname;
    portarg = दत्त->conn->origin->पत्तनम्;
  }
  यदि(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *दत्त,
                           पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  (शून्य)cf;
  (शून्य)दत्त;
  (शून्य)event;
  (शून्य)arg1;
  (शून्य)arg2;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            bool ignore_परिणाम,
                            पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  CURLcode परिणाम = CURLE_OK;

  प्रति(; cf; cf = cf->अग्र) {
    यदि(cf->cft->cntrl == Curl_cf_def_cntrl)
      अनुवर्त;
    परिणाम = cf->cft->cntrl(cf, दत्त, event, arg1, arg2);
    यदि(!ignore_परिणाम && परिणाम)
      भङ्ग;
  }
  प्रत्यागम परिणाम;
}

curl_socket_t Curl_conn_cf_get_socket(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त)
{
  curl_socket_t sock;
  यदि(cf && !cf->cft->query(cf, दत्त, CF_QUERY_SOCKET, रिक्त, &sock))
    प्रत्यागम sock;
  प्रत्यागम CURL_SOCKET_BAD;
}

अचिह्नित वर्ण Curl_conn_cf_get_transport(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त)
{
  पूर्णाङ्क transport = 0;
  यदि(cf && !cf->cft->query(cf, दत्त, CF_QUERY_TRANSPORT, &transport, रिक्त))
    प्रत्यागम (अचिह्नित वर्ण)transport;
  प्रत्यागम (अचिह्नित वर्ण)(दत्त->conn ? दत्त->conn->transport_wanted : 0);
}

ध्रुव वर्ण *Curl_conn_cf_get_alpn_negotiated(संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *दत्त)
{
  ध्रुव वर्ण *alpn = रिक्त;
  CURL_TRC_CF(दत्त, cf, "query ALPN");
  यदि(cf && !cf->cft->query(cf, दत्त, CF_QUERY_ALPN_NEGOTIATED, रिक्त,
                           CURL_UNCONST(&alpn)))
    प्रत्यागम alpn;
  प्रत्यागम रिक्त;
}

स्थिर ध्रुव संरचना Curl_sockaddr_ex *cf_get_remote_addr(
  संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  ध्रुव संरचना Curl_sockaddr_ex *remote_addr = रिक्त;
  यदि(cf &&
     !cf->cft->query(cf, दत्त, CF_QUERY_REMOTE_ADDR, रिक्त,
                     CURL_UNCONST(&remote_addr)))
    प्रत्यागम remote_addr;
  प्रत्यागम रिक्त;
}

CURLcode Curl_conn_cf_get_ip_info(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  bool *is_ipv6, संरचना ip_quadruple *ipquad)
{
  CURLcode परिणाम = CURLE_UNKNOWN_OPTION;
  यदि(cf) {
    पूर्णाङ्क ipv6 = 0;
    परिणाम = cf->cft->query(cf, दत्त, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  प्रत्यागम परिणाम;
}

curl_socket_t Curl_conn_get_first_socket(संरचना Curl_easy *दत्त)
{
  संरचना Curl_cfilter *cf;

  यदि(!दत्त->conn)
    प्रत्यागम CURL_SOCKET_BAD;

  cf = दत्त->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  यदि(cf && !cf->connected)
    प्रत्यागम Curl_conn_cf_get_socket(cf, दत्त);
  प्रत्यागम दत्त->conn->sock[FIRSTSOCKET];
}

ध्रुव संरचना Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  संरचना Curl_easy *दत्त, int8_t sockindex)
{
  संरचना Curl_cfilter *cf =
    (दत्त->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    दत्त->conn->cfilter[sockindex] : रिक्त;
  प्रत्यागम cf ? cf_get_remote_addr(cf, दत्त) : रिक्त;
}

CURLcode Curl_conn_ev_दत्त_सज्जता(संरचना Curl_easy *दत्त)
{
  प्रत्यागम cf_cntrl_all(दत्त->conn, दत्त, FALSE, CF_CTRL_दत्त_सज्जता, 0, रिक्त);
}

CURLcode Curl_conn_flush(संरचना Curl_easy *दत्त, int8_t sockindex)
{
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  प्रत्यागम Curl_conn_cf_cntrl(दत्त->conn->cfilter[sockindex], दत्त, FALSE,
                            CF_CTRL_FLUSH, 0, रिक्त);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
शून्य Curl_conn_ev_दत्त_समाप्तम्_send(संरचना Curl_easy *दत्त)
{
  cf_cntrl_all(दत्त->conn, दत्त, TRUE, CF_CTRL_दत्त_समाप्तम्_SEND, 0, रिक्त);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
शून्य Curl_conn_ev_दत्त_समाप्तम्(संरचना Curl_easy *दत्त, bool premature)
{
  cf_cntrl_all(दत्त->conn, दत्त, TRUE, CF_CTRL_दत्त_समाप्तम्, premature, रिक्त);
}

CURLcode Curl_conn_ev_दत्त_pause(संरचना Curl_easy *दत्त, bool do_pause)
{
  प्रत्यागम cf_cntrl_all(दत्त->conn, दत्त, FALSE,
                      CF_CTRL_दत्त_PAUSE, do_pause, रिक्त);
}

bool Curl_conn_is_alive(संरचना Curl_easy *दत्त, संरचना connectdata *conn,
                        bool *निवेशः_pending)
{
  संरचना Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  प्रत्यागम cf && !cf->conn->द्वयङ्काः.close &&
         cf->cft->is_alive(cf, दत्त, निवेशः_pending);
}

CURLcode Curl_conn_keep_alive(संरचना Curl_easy *दत्त,
                              संरचना connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क i;

  प्रति(i = 0; (i < (पूर्णाङ्क)CURL_ARRAYSIZE(conn->cfilter)) && !परिणाम; ++i) {
    संरचना Curl_cfilter *cf = conn->cfilter[i];
    यदि(cf)
      परिणाम = cf->cft->keep_alive(cf, दत्त);
  }
  प्रत्यागम परिणाम;
}

size_t Curl_conn_get_max_concurrent(संरचना Curl_easy *दत्त,
                                    संरचना connectdata *conn,
                                    int8_t sockindex)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम;
  पूर्णाङ्क n = -1;

  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम 0;

  cf = conn->cfilter[sockindex];
  परिणाम = cf ? cf->cft->query(cf, दत्त, CF_QUERY_MAX_CONCURRENT,
                               &n, रिक्त) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  प्रत्यागम (परिणाम || n < 0) ? 1 : (size_t)n;
}

पूर्णाङ्क Curl_conn_get_प्रवाहः_error(संरचना Curl_easy *दत्त,
                               संरचना connectdata *conn,
                               int8_t sockindex)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम;
  पूर्णाङ्क n = 0;

  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम 0;

  cf = conn->cfilter[sockindex];
  परिणाम = cf ? cf->cft->query(cf, दत्त, CF_QUERY_प्रवाहः_ERROR,
                               &n, रिक्त) : CURLE_UNKNOWN_OPTION;
  प्रत्यागम (परिणाम || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(संरचना Curl_easy *दत्त, curl_socket_t sockfd)
{
  यदि(दत्त && दत्त->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == दत्त->conn->sock[SECONDARYSOCKET])
    प्रत्यागम SECONDARYSOCKET;
  प्रत्यागम FIRSTSOCKET;
}

CURLcode Curl_conn_recv(संरचना Curl_easy *दत्त, int8_t sockindex,
                        वर्ण *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  यदि(दत्त && दत्त->conn && दत्त->conn->recv[sockindex])
    प्रत्यागम दत्त->conn->recv[sockindex](दत्त, sockindex, buf, len, pnread);
  *pnread = 0;
  प्रत्यागम CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(संरचना Curl_easy *दत्त, int8_t sockindex,
                        ध्रुव शून्य *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
#यदि_परिभाषितम् DEBUGBUILD
  यदि(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ध्रुव वर्ण *p = getenv("CURL_SMALLSENDS");
    यदि(p) {
      curl_off_t altsize;
      यदि(!curlx_str_सङ्ख्या(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        यदि(write_len != len)
          eos = FALSE;
      }
    }
  }
#यदि_अन्तः
  यदि(दत्त && दत्त->conn && दत्त->conn->send[sockindex]) {
    CURLcode परिणाम = दत्त->conn->send[sockindex](दत्त, sockindex, buf,
                                                  write_len, eos, pnwritten);
    यदि(!परिणाम && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      प्रत्यागम CURLE_SEND_ERROR;
    }
    प्रत्यागम परिणाम;
  }
  *pnwritten = 0;
  प्रत्यागम CURLE_FAILED_INIT;
}
