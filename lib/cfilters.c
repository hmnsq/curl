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

  जर(cf->connected) {
    *done = TRUE;
    परत CURLE_OK;
  }

  जर(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    जर(result || !*done)
      परत result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  परत CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data, bool *done)
{
  (शून्य)cf;
  (शून्य)data;
  *done = TRUE;
  परत CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना easy_pollset *ps)
{
  /* NOP */
  (शून्य)cf;
  (शून्य)data;
  (शून्य)ps;
  परत CURLE_OK;
}

bool Curl_cf_def_data_pending(संरचना Curl_cfilter *cf,
                              स्थिरांक संरचना Curl_easy *data)
{
  परत cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                          स्थिरांक uint8_t *buf, माप_प्रकार len, bool eos,
                          माप_प्रकार *pnwritten)
{
  जर(cf->next)
    परत cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  परत CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                          अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  जर(cf->next)
    परत cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  परत CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               bool *आदान_pending)
{
  परत cf->next ?
    cf->next->cft->is_alive(cf->next, data, आदान_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data)
{
  परत cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           पूर्णांक query, पूर्णांक *pres1, शून्य *pres2)
{
  परत cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
शून्य Curl_conn_trc_filters(संरचना Curl_easy *data,
                           int8_t sockindex, स्थिरांक अक्षर *info)
{
  जर(CURL_TRC_M_is_verbose(data) && data->conn) {
    संरचना Curl_cfilter *cf = data->conn->cfilter[sockindex];

    जर(cf) {
      अक्षर संदेश[256], *buf;
      पूर्णांक blen, n;

      buf = संदेश;
      blen = आकार(संदेश) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      साठी(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", संदेश, blen ? "" : "...");
    }
    अन्यथा
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

शून्य Curl_conn_cf_discard_chain(संरचना Curl_cfilter **pcf,
                                संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cfn, *cf = *pcf;

  जर(cf) {
    *pcf = रिक्त;
    पर्यंत(cf) {
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

CURLcode Curl_cf_recv(संरचना Curl_easy *data, int8_t sockindex, अक्षर *buf,
                      माप_प्रकार len, माप_प्रकार *pnread)
{
  संरचना Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  पर्यंत(cf && !cf->connected)
    cf = cf->next;
  जर(cf)
    परत cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  परत CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(संरचना Curl_easy *data, int8_t sockindex,
                      स्थिरांक uint8_t *buf, माप_प्रकार len, bool eos,
                      माप_प्रकार *pnwritten)
{
  संरचना Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  पर्यंत(cf && !cf->connected)
    cf = cf->next;
  जर(cf) {
    परत cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  परत CURLE_FAILED_INIT;
}

संरचना cf_io_ctx {
  संरचना Curl_easy *data;
  संरचना Curl_cfilter *cf;
};

स्थिर CURLcode cf_bufq_reader(शून्य *writer_ctx,
                               अचिन्हित अक्षर *buf, माप_प्रकार blen,
                               माप_प्रकार *pnread)
{
  संरचना cf_io_ctx *io = writer_ctx;
  परत Curl_conn_cf_recv(io->cf, io->data, (अक्षर *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           संरचना bufq *bufq,
                           माप_प्रकार maxlen,
                           माप_प्रकार *pnread)
{
  संरचना cf_io_ctx io;

  जर(!cf || !data) {
    *pnread = 0;
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  परत Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

स्थिर CURLcode cf_bufq_writer(शून्य *writer_ctx,
                               स्थिरांक uint8_t *buf, माप_प्रकार buflen,
                               माप_प्रकार *pnwritten)
{
  संरचना cf_io_ctx *io = writer_ctx;
  परत Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           संरचना bufq *bufq,
                           स्थिरांक अचिन्हित अक्षर *buf, माप_प्रकार blen,
                           माप_प्रकार *pnwritten)
{
  संरचना cf_io_ctx io;

  जर(!cf || !data) {
    *pnwritten = 0;
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  जर(buf && blen)
    परत Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  अन्यथा
    परत Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(संरचना Curl_cfilter **pcf,
                        स्थिरांक संरचना Curl_cftype *cft,
                        शून्य *ctx)
{
  संरचना Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, आकार(*cf));
  जर(!cf)
    जा out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  परत result;
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

शून्य Curl_conn_cf_घाला_after(संरचना Curl_cfilter *cf_at,
                               संरचना Curl_cfilter *cf_new)
{
  संरचना Curl_cfilter *tail, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  tail = cf_at->next;
  cf_at->next = cf_new;
  करा {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->next;
    cf_new = cf_new->next;
  } पर्यंत(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(संरचना Curl_cfilter **pcf,
                          संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cf = pcf ? *pcf : रिक्त;
  bool found = FALSE;
  जर(cf) {
    जर(cf->conn) {
      /* unlink if present in connection filter chain */
      संरचना Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      पर्यंत(*pprev) {
        जर(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = रिक्त;
          found = TRUE;
          थांबा;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  परत found;
}

CURLcode Curl_conn_cf_connect(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              bool *done)
{
  जर(cf)
    परत cf->cft->do_connect(cf, data, done);
  परत CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                           स्थिरांक uint8_t *buf, माप_प्रकार len, bool eos,
                           माप_प्रकार *pnwritten)
{
  जर(cf)
    परत cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  परत CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                           अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  जर(cf)
    परत cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  परत CURLE_RECV_ERROR;
}

स्थिर CURLcode cf_cntrl_all(संरचना connectdata *conn,
                             संरचना Curl_easy *data,
                             bool ignore_result,
                             पूर्णांक event, पूर्णांक arg1, शून्य *arg2)
{
  CURLcode result = CURLE_OK;
  पूर्णांक i;

  साठी(i = 0; i < (पूर्णांक)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    जर(!ignore_result && result)
      थांबा;
  }
  परत result;
}

bool Curl_conn_cf_is_ip_connected(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  जर(cf) {
    पूर्णांक मूल्य = 0;
    जर(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &मूल्य, रिक्त))
      परत !!मूल्य;
  }
  परत FALSE;
}

शून्य Curl_conn_cntrl_update_info(संरचना Curl_easy *data,
                                संरचना connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, रिक्त);
}

शून्य Curl_conn_cntrl_report_stats(संरचना Curl_easy *data,
                                  संरचना connectdata *conn,
                                  पूर्णांक sockindex)
{
  जर((अचिन्हित)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (शून्य)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, रिक्त);
}

शून्य Curl_conn_remove_setup_filters(संरचना Curl_easy *data,
                                    int8_t sockindex)
{
  संरचना Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  पर्यंत(*anchor) {
    संरचना Curl_cfilter *cf = *anchor;
    जर(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = रिक्त;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    अन्यथा
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(संरचना connectdata *conn, int8_t sockindex)
{
  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत FALSE;
  परत !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(संरचना connectdata *conn, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत FALSE;
  cf = conn->cfilter[sockindex];
  जर(cf)
    परत (bool)cf->connected;
  अन्यथा जर(conn->scheme->flags & PROTOPT_NONETWORK)
    परत TRUE;
  परत FALSE;
}

bool Curl_conn_is_ip_connected(संरचना Curl_easy *data, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत FALSE;
  cf = data->conn->cfilter[sockindex];
  पर्यंत(cf) {
    जर(cf->connected)
      परत TRUE;
    जर(cf->cft->flags & CF_TYPE_IP_CONNECT)
      परत FALSE;
    cf = cf->next;
  }
  परत FALSE;
}

#ifndef CURL_DISABLE_PROXY
स्थिर bool cf_is_tunneling(संरचना Curl_cfilter *cf)
{
  साठी(; cf; cf = cf->next) {
    जर((cf->cft->flags & CF_TYPE_PROXY))
      परत TRUE;
  }
  परत FALSE;
}

bool Curl_conn_is_tunneling(संरचना connectdata *conn, int8_t sockindex)
{
  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत FALSE;
  परत conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#अन्यथा
bool Curl_conn_is_tunneling(संरचना connectdata *conn, int8_t sockindex)
{
  (शून्य)conn;
  (शून्य)sockindex;
  परत FALSE;
}
#endif /* CURL_DISABLE_PROXY */

स्थिर bool cf_is_ssl(संरचना Curl_cfilter *cf)
{
  साठी(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    जर((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      परत TRUE;
    जर(cf->cft->flags & CF_TYPE_IP_CONNECT)
      परत FALSE;
  }
  परत FALSE;
}

bool Curl_conn_is_ssl(संरचना connectdata *conn, int8_t sockindex)
{
  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत FALSE;
  परत conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(संरचना Curl_easy *data,
                            संरचना connectdata *conn, int8_t sockindex,
                            पूर्णांक query,
                            संरचना curl_tlssessioninfo *info)
{
  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत FALSE;
  जर(Curl_conn_is_ssl(conn, sockindex)) {
    संरचना Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, रिक्त, (शून्य *)info) :
      CURLE_UNKNOWN_OPTION;
    परत !result;
  }
  परत FALSE;
}

CURLcode Curl_conn_get_ip_info(संरचना Curl_easy *data,
                               संरचना connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, संरचना ip_quadruple *ipquad)
{
  संरचना Curl_cfilter *cf;
  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : रिक्त;
  परत Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(संरचना connectdata *conn, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  जर(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    परत FALSE;
  साठी(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    जर(cf->cft->flags & CF_TYPE_MULTIPLEX)
      परत TRUE;
    जर(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      परत FALSE;
  }
  परत FALSE;
}

अचिन्हित अक्षर Curl_conn_get_transport(संरचना Curl_easy *data,
                                      संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  परत Curl_conn_cf_get_transport(cf, data);
}

पूर्णांक Curl_socktype_for_transport(uint8_t transport)
{
  निवड(transport) {
  स्थिती TRNSPRT_TCP:
    परत SOCK_STREAM;
  स्थिती TRNSPRT_UNIX:
    परत SOCK_STREAM;
  पूर्वनिर्धारित: /* UDP and QUIC */
    परत SOCK_DGRAM;
  }
}

पूर्णांक Curl_protocol_for_transport(uint8_t transport)
{
  निवड(transport) {
  स्थिती TRNSPRT_TCP:
    परत IPPROTO_TCP;
  स्थिती TRNSPRT_UNIX:
    परत IPPROTO_IP;
  पूर्वनिर्धारित: /* UDP and QUIC */
    परत IPPROTO_UDP;
  }
}

स्थिरांक अक्षर *Curl_conn_get_alpn_negotiated(संरचना Curl_easy *data,
                                          संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  परत Curl_conn_cf_get_alpn_negotiated(cf, data);
}

अचिन्हित अक्षर Curl_conn_http_version(संरचना Curl_easy *data,
                                     संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  अचिन्हित अक्षर v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  साठी(; cf; cf = cf->next) {
    जर(cf->cft->flags & CF_TYPE_HTTP) {
      पूर्णांक मूल्य = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &मूल्य, रिक्त);
      जर(!result && ((मूल्य < 0) || (मूल्य > 255)))
        result = CURLE_FAILED_INIT;
      अन्यथा
        v = (अचिन्हित अक्षर)मूल्य;
      थांबा;
    }
    जर(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      थांबा;
  }
  परत (अचिन्हित अक्षर)(result ? 0 : v);
}

bool Curl_conn_data_pending(संरचना Curl_easy *data, int8_t sockindex)
{
  संरचना Curl_cfilter *cf;

  (शून्य)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत FALSE;

  cf = data->conn->cfilter[sockindex];
  पर्यंत(cf && !cf->connected) {
    cf = cf->next;
  }
  जर(cf) {
    परत cf->cft->has_data_pending(cf, data);
  }
  परत FALSE;
}

bool Curl_conn_cf_needs_flush(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data)
{
  CURLcode result;
  पूर्णांक pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, रिक्त) : CURLE_UNKNOWN_OPTION;
  परत (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(संरचना Curl_easy *data, int8_t sockindex)
{
  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत FALSE;
  परत Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
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
  साठी(; cf && !result; cf = cf->next) {
    जर(cf->shutdown)
      सुरूठेवा;
    जर(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  परत result;
}

CURLcode Curl_conn_adjust_pollset(संरचना Curl_easy *data,
                                  संरचना connectdata *conn,
                                  संरचना easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  स्थिरांक bool want_io = !!ps->n;
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
  साठी(i = 0; (i < (पूर्णांक)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    जर(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  परत result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
पूर्णांक Curl_conn_cf_poll(संरचना Curl_cfilter *cf,
                      संरचना Curl_easy *data,
                      timediff_t timeout_ms)
{
  संरचना easy_pollset ps;
  पूर्णांक rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  जर(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  अन्यथा
    rc = -1;
  Curl_pollset_cleanup(&ps);
  परत rc;
}

शून्य Curl_conn_get_current_host(संरचना Curl_easy *data, int8_t sockindex,
                                स्थिरांक अक्षर **phost, पूर्णांक *pport)
{
  संरचना Curl_cfilter *cf, *cf_proxy = रिक्त;
  पूर्णांक portarg = -1;

  जर(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    जर(pport)
      *pport = -1;
    परत;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : रिक्त;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  पर्यंत(cf && !cf->connected) {
    जर((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  जर(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  जर(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           पूर्णांक event, पूर्णांक arg1, शून्य *arg2)
{
  (शून्य)cf;
  (शून्य)data;
  (शून्य)event;
  (शून्य)arg1;
  (शून्य)arg2;
  परत CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            bool ignore_result,
                            पूर्णांक event, पूर्णांक arg1, शून्य *arg2)
{
  CURLcode result = CURLE_OK;

  साठी(; cf; cf = cf->next) {
    जर(cf->cft->cntrl == Curl_cf_def_cntrl)
      सुरूठेवा;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    जर(!ignore_result && result)
      थांबा;
  }
  परत result;
}

curl_socket_t Curl_conn_cf_get_socket(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data)
{
  curl_socket_t sock;
  जर(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, रिक्त, &sock))
    परत sock;
  परत CURL_SOCKET_BAD;
}

अचिन्हित अक्षर Curl_conn_cf_get_transport(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data)
{
  पूर्णांक transport = 0;
  जर(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, रिक्त))
    परत (अचिन्हित अक्षर)transport;
  परत (अचिन्हित अक्षर)(data->conn ? data->conn->transport_wanted : 0);
}

स्थिरांक अक्षर *Curl_conn_cf_get_alpn_negotiated(संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *data)
{
  स्थिरांक अक्षर *alpn = रिक्त;
  CURL_TRC_CF(data, cf, "query ALPN");
  जर(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, रिक्त,
                           CURL_UNCONST(&alpn)))
    परत alpn;
  परत रिक्त;
}

स्थिर स्थिरांक संरचना Curl_sockaddr_ex *cf_get_remote_addr(
  संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  स्थिरांक संरचना Curl_sockaddr_ex *remote_addr = रिक्त;
  जर(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, रिक्त,
                     CURL_UNCONST(&remote_addr)))
    परत remote_addr;
  परत रिक्त;
}

CURLcode Curl_conn_cf_get_ip_info(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  bool *is_ipv6, संरचना ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  जर(cf) {
    पूर्णांक ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  परत result;
}

curl_socket_t Curl_conn_get_first_socket(संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cf;

  जर(!data->conn)
    परत CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  जर(cf && !cf->connected)
    परत Curl_conn_cf_get_socket(cf, data);
  परत data->conn->sock[FIRSTSOCKET];
}

स्थिरांक संरचना Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  संरचना Curl_easy *data, int8_t sockindex)
{
  संरचना Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : रिक्त;
  परत cf ? cf_get_remote_addr(cf, data) : रिक्त;
}

CURLcode Curl_conn_ev_data_setup(संरचना Curl_easy *data)
{
  परत cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, रिक्त);
}

CURLcode Curl_conn_flush(संरचना Curl_easy *data, int8_t sockindex)
{
  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  परत Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
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
  परत cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, रिक्त);
}

bool Curl_conn_is_alive(संरचना Curl_easy *data, संरचना connectdata *conn,
                        bool *आदान_pending)
{
  संरचना Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  परत cf && !cf->conn->bits.बंद &&
         cf->cft->is_alive(cf, data, आदान_pending);
}

CURLcode Curl_conn_keep_alive(संरचना Curl_easy *data,
                              संरचना connectdata *conn)
{
  CURLcode result = CURLE_OK;
  पूर्णांक i;

  साठी(i = 0; (i < (पूर्णांक)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    संरचना Curl_cfilter *cf = conn->cfilter[i];
    जर(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  परत result;
}

माप_प्रकार Curl_conn_get_max_concurrent(संरचना Curl_easy *data,
                                    संरचना connectdata *conn,
                                    int8_t sockindex)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;
  पूर्णांक n = -1;

  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, रिक्त) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  परत (result || n < 0) ? 1 : (माप_प्रकार)n;
}

पूर्णांक Curl_conn_get_stream_error(संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               int8_t sockindex)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;
  पूर्णांक n = 0;

  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, रिक्त) : CURLE_UNKNOWN_OPTION;
  परत (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(संरचना Curl_easy *data, curl_socket_t sockfd)
{
  जर(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    परत SECONDARYSOCKET;
  परत FIRSTSOCKET;
}

CURLcode Curl_conn_recv(संरचना Curl_easy *data, int8_t sockindex,
                        अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  जर(data && data->conn && data->conn->recv[sockindex])
    परत data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  परत CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(संरचना Curl_easy *data, int8_t sockindex,
                        स्थिरांक शून्य *buf, माप_प्रकार len, bool eos,
                        माप_प्रकार *pnwritten)
{
  माप_प्रकार write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  जर(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    स्थिरांक अक्षर *p = दो_पर्या("CURL_SMALLSENDS");
    जर(p) {
      curl_off_t altsize;
      जर(!curlx_str_संख्या(&p, &altsize, write_len)) {
        write_len = (माप_प्रकार)altsize;
        जर(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  जर(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    जर(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      परत CURLE_SEND_ERROR;
    }
    परत result;
  }
  *pnwritten = 0;
  परत CURLE_FAILED_INIT;
}
