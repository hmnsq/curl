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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "strerror.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "curlx/strparse.h"

CURLcode Curl_cf_def_connect(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  எனில்(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    எனில்(result || !*done)
      திரும்பு result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  திரும்பு CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data, bool *done)
{
  (வெறுமை)cf;
  (வெறுமை)data;
  *done = TRUE;
  திரும்பு CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு easy_pollset *ps)
{
  /* NOP */
  (வெறுமை)cf;
  (வெறுமை)data;
  (வெறுமை)ps;
  திரும்பு CURLE_OK;
}

bool Curl_cf_def_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                              மாறிலி கட்டமைப்பு Curl_easy *data)
{
  திரும்பு cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                          மாறிலி uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  எனில்(cf->next)
    திரும்பு cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  திரும்பு CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                          எழுத்து *buf, size_t len, size_t *pnread)
{
  எனில்(cf->next)
    திரும்பு cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  திரும்பு CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               bool *உள்ளீடு_pending)
{
  திரும்பு cf->next ?
    cf->next->cft->is_alive(cf->next, data, உள்ளீடு_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data)
{
  திரும்பு cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(கட்டமைப்பு Curl_cfilter *cf,
                           கட்டமைப்பு Curl_easy *data,
                           முழு query, முழு *pres1, வெறுமை *pres2)
{
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
வெறுமை Curl_conn_trc_filters(கட்டமைப்பு Curl_easy *data,
                           int8_t sockindex, மாறிலி எழுத்து *info)
{
  எனில்(CURL_TRC_M_is_verbose(data) && data->conn) {
    கட்டமைப்பு Curl_cfilter *cf = data->conn->cfilter[sockindex];

    எனில்(cf) {
      எழுத்து செய்தி[256], *buf;
      முழு blen, n;

      buf = செய்தி;
      blen = அளவு(செய்தி) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      க்கு(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", செய்தி, blen ? "" : "...");
    }
    இல்லையெனில்
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

வெறுமை Curl_conn_cf_discard_chain(கட்டமைப்பு Curl_cfilter **pcf,
                                கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cfilter *cfn, *cf = *pcf;

  எனில்(cf) {
    *pcf = வெற்று;
    போது(cf) {
      cfn = cf->next;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->next = வெற்று;
      cf->cft->destroy(cf, data);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

வெறுமை Curl_conn_cf_discard_all(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], data);
}

CURLcode Curl_cf_recv(கட்டமைப்பு Curl_easy *data, int8_t sockindex, எழுத்து *buf,
                      size_t len, size_t *pnread)
{
  கட்டமைப்பு Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  போது(cf && !cf->connected)
    cf = cf->next;
  எனில்(cf)
    திரும்பு cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  திரும்பு CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                      மாறிலி uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  கட்டமைப்பு Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  போது(cf && !cf->connected)
    cf = cf->next;
  எனில்(cf) {
    திரும்பு cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  திரும்பு CURLE_FAILED_INIT;
}

கட்டமைப்பு cf_io_ctx {
  கட்டமைப்பு Curl_easy *data;
  கட்டமைப்பு Curl_cfilter *cf;
};

நிலையான CURLcode cf_bufq_reader(வெறுமை *writer_ctx,
                               unsigned எழுத்து *buf, size_t blen,
                               size_t *pnread)
{
  கட்டமைப்பு cf_io_ctx *io = writer_ctx;
  திரும்பு Curl_conn_cf_recv(io->cf, io->data, (எழுத்து *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(கட்டமைப்பு Curl_cfilter *cf,
                           கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  கட்டமைப்பு cf_io_ctx io;

  எனில்(!cf || !data) {
    *pnread = 0;
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  திரும்பு Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

நிலையான CURLcode cf_bufq_writer(வெறுமை *writer_ctx,
                               மாறிலி uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  கட்டமைப்பு cf_io_ctx *io = writer_ctx;
  திரும்பு Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(கட்டமைப்பு Curl_cfilter *cf,
                           கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு bufq *bufq,
                           மாறிலி unsigned எழுத்து *buf, size_t blen,
                           size_t *pnwritten)
{
  கட்டமைப்பு cf_io_ctx io;

  எனில்(!cf || !data) {
    *pnwritten = 0;
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  எனில்(buf && blen)
    திரும்பு Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  இல்லையெனில்
    திரும்பு Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(கட்டமைப்பு Curl_cfilter **pcf,
                        மாறிலி கட்டமைப்பு Curl_cftype *cft,
                        வெறுமை *ctx)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, அளவு(*cf));
  எனில்(!cf)
    செல் out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  திரும்பு result;
}

வெறுமை Curl_conn_cf_add(கட்டமைப்பு Curl_easy *data,
                      கட்டமைப்பு connectdata *conn,
                      int8_t sockindex,
                      கட்டமைப்பு Curl_cfilter *cf)
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

வெறுமை Curl_conn_cf_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                               கட்டமைப்பு Curl_cfilter *cf_new)
{
  கட்டமைப்பு Curl_cfilter *tail, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  tail = cf_at->next;
  cf_at->next = cf_new;
  செய் {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->next;
    cf_new = cf_new->next;
  } போது(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(கட்டமைப்பு Curl_cfilter **pcf,
                          கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cfilter *cf = pcf ? *pcf : வெற்று;
  bool found = FALSE;
  எனில்(cf) {
    எனில்(cf->conn) {
      /* unlink if present in connection filter chain */
      கட்டமைப்பு Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      போது(*pprev) {
        எனில்(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = வெற்று;
          found = TRUE;
          நிறுத்து;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  திரும்பு found;
}

CURLcode Curl_conn_cf_connect(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              bool *done)
{
  எனில்(cf)
    திரும்பு cf->cft->do_connect(cf, data, done);
  திரும்பு CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                           மாறிலி uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  எனில்(cf)
    திரும்பு cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  திரும்பு CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                           எழுத்து *buf, size_t len, size_t *pnread)
{
  எனில்(cf)
    திரும்பு cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  திரும்பு CURLE_RECV_ERROR;
}

நிலையான CURLcode cf_cntrl_all(கட்டமைப்பு connectdata *conn,
                             கட்டமைப்பு Curl_easy *data,
                             bool ignore_result,
                             முழு event, முழு arg1, வெறுமை *arg2)
{
  CURLcode result = CURLE_OK;
  முழு i;

  க்கு(i = 0; i < (முழு)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    எனில்(!ignore_result && result)
      நிறுத்து;
  }
  திரும்பு result;
}

bool Curl_conn_cf_is_ip_connected(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data)
{
  எனில்(cf) {
    முழு மதிப்பு = 0;
    எனில்(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &மதிப்பு, வெற்று))
      திரும்பு !!மதிப்பு;
  }
  திரும்பு FALSE;
}

வெறுமை Curl_conn_cntrl_update_info(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, வெற்று);
}

வெறுமை Curl_conn_cntrl_report_stats(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn,
                                  முழு sockindex)
{
  எனில்((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (வெறுமை)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, வெற்று);
}

வெறுமை Curl_conn_remove_setup_filters(கட்டமைப்பு Curl_easy *data,
                                    int8_t sockindex)
{
  கட்டமைப்பு Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  போது(*anchor) {
    கட்டமைப்பு Curl_cfilter *cf = *anchor;
    எனில்(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = வெற்று;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    இல்லையெனில்
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(கட்டமைப்பு connectdata *conn, int8_t sockindex)
{
  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு FALSE;
  திரும்பு !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(கட்டமைப்பு connectdata *conn, int8_t sockindex)
{
  கட்டமைப்பு Curl_cfilter *cf;

  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு FALSE;
  cf = conn->cfilter[sockindex];
  எனில்(cf)
    திரும்பு (bool)cf->connected;
  இல்லையெனில் எனில்(conn->scheme->flags & PROTOPT_NONETWORK)
    திரும்பு TRUE;
  திரும்பு FALSE;
}

bool Curl_conn_is_ip_connected(கட்டமைப்பு Curl_easy *data, int8_t sockindex)
{
  கட்டமைப்பு Curl_cfilter *cf;

  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு FALSE;
  cf = data->conn->cfilter[sockindex];
  போது(cf) {
    எனில்(cf->connected)
      திரும்பு TRUE;
    எனில்(cf->cft->flags & CF_TYPE_IP_CONNECT)
      திரும்பு FALSE;
    cf = cf->next;
  }
  திரும்பு FALSE;
}

#ifndef CURL_DISABLE_PROXY
நிலையான bool cf_is_tunneling(கட்டமைப்பு Curl_cfilter *cf)
{
  க்கு(; cf; cf = cf->next) {
    எனில்((cf->cft->flags & CF_TYPE_PROXY))
      திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

bool Curl_conn_is_tunneling(கட்டமைப்பு connectdata *conn, int8_t sockindex)
{
  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு FALSE;
  திரும்பு conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#இல்லையெனில்
bool Curl_conn_is_tunneling(கட்டமைப்பு connectdata *conn, int8_t sockindex)
{
  (வெறுமை)conn;
  (வெறுமை)sockindex;
  திரும்பு FALSE;
}
#endif /* CURL_DISABLE_PROXY */

நிலையான bool cf_is_ssl(கட்டமைப்பு Curl_cfilter *cf)
{
  க்கு(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    எனில்((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      திரும்பு TRUE;
    எனில்(cf->cft->flags & CF_TYPE_IP_CONNECT)
      திரும்பு FALSE;
  }
  திரும்பு FALSE;
}

bool Curl_conn_is_ssl(கட்டமைப்பு connectdata *conn, int8_t sockindex)
{
  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு FALSE;
  திரும்பு conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு connectdata *conn, int8_t sockindex,
                            முழு query,
                            கட்டமைப்பு curl_tlssessioninfo *info)
{
  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு FALSE;
  எனில்(Curl_conn_is_ssl(conn, sockindex)) {
    கட்டமைப்பு Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, வெற்று, (வெறுமை *)info) :
      CURLE_UNKNOWN_OPTION;
    திரும்பு !result;
  }
  திரும்பு FALSE;
}

CURLcode Curl_conn_get_ip_info(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, கட்டமைப்பு ip_quadruple *ipquad)
{
  கட்டமைப்பு Curl_cfilter *cf;
  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : வெற்று;
  திரும்பு Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(கட்டமைப்பு connectdata *conn, int8_t sockindex)
{
  கட்டமைப்பு Curl_cfilter *cf;

  எனில்(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு FALSE;
  க்கு(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    எனில்(cf->cft->flags & CF_TYPE_MULTIPLEX)
      திரும்பு TRUE;
    எனில்(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      திரும்பு FALSE;
  }
  திரும்பு FALSE;
}

unsigned எழுத்து Curl_conn_get_transport(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  திரும்பு Curl_conn_cf_get_transport(cf, data);
}

முழு Curl_socktype_for_transport(uint8_t transport)
{
  தேர்வு(transport) {
  நிலை TRNSPRT_TCP:
    திரும்பு SOCK_STREAM;
  நிலை TRNSPRT_UNIX:
    திரும்பு SOCK_STREAM;
  இயல்பு: /* UDP and QUIC */
    திரும்பு SOCK_DGRAM;
  }
}

முழு Curl_protocol_for_transport(uint8_t transport)
{
  தேர்வு(transport) {
  நிலை TRNSPRT_TCP:
    திரும்பு IPPROTO_TCP;
  நிலை TRNSPRT_UNIX:
    திரும்பு IPPROTO_IP;
  இயல்பு: /* UDP and QUIC */
    திரும்பு IPPROTO_UDP;
  }
}

மாறிலி எழுத்து *Curl_conn_get_alpn_negotiated(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  திரும்பு Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned எழுத்து Curl_conn_http_version(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned எழுத்து v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  க்கு(; cf; cf = cf->next) {
    எனில்(cf->cft->flags & CF_TYPE_HTTP) {
      முழு மதிப்பு = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &மதிப்பு, வெற்று);
      எனில்(!result && ((மதிப்பு < 0) || (மதிப்பு > 255)))
        result = CURLE_FAILED_INIT;
      இல்லையெனில்
        v = (unsigned எழுத்து)மதிப்பு;
      நிறுத்து;
    }
    எனில்(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      நிறுத்து;
  }
  திரும்பு (unsigned எழுத்து)(result ? 0 : v);
}

bool Curl_conn_data_pending(கட்டமைப்பு Curl_easy *data, int8_t sockindex)
{
  கட்டமைப்பு Curl_cfilter *cf;

  (வெறுமை)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு FALSE;

  cf = data->conn->cfilter[sockindex];
  போது(cf && !cf->connected) {
    cf = cf->next;
  }
  எனில்(cf) {
    திரும்பு cf->cft->has_data_pending(cf, data);
  }
  திரும்பு FALSE;
}

bool Curl_conn_cf_needs_flush(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data)
{
  CURLcode result;
  முழு pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, வெற்று) : CURLE_UNKNOWN_OPTION;
  திரும்பு (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(கட்டமைப்பு Curl_easy *data, int8_t sockindex)
{
  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு FALSE;
  திரும்பு Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
}

CURLcode Curl_conn_cf_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  க்கு(; cf && !result; cf = cf->next) {
    எனில்(cf->shutdown)
      தொடர்;
    எனில்(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  திரும்பு result;
}

CURLcode Curl_conn_adjust_pollset(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn,
                                  கட்டமைப்பு easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  மாறிலி bool want_io = !!ps->n;
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
  க்கு(i = 0; (i < (முழு)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    எனில்(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  திரும்பு result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
முழு Curl_conn_cf_poll(கட்டமைப்பு Curl_cfilter *cf,
                      கட்டமைப்பு Curl_easy *data,
                      timediff_t timeout_ms)
{
  கட்டமைப்பு easy_pollset ps;
  முழு rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  எனில்(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  இல்லையெனில்
    rc = -1;
  Curl_pollset_cleanup(&ps);
  திரும்பு rc;
}

வெறுமை Curl_conn_get_current_host(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                                மாறிலி எழுத்து **phost, முழு *pport)
{
  கட்டமைப்பு Curl_cfilter *cf, *cf_proxy = வெற்று;
  முழு portarg = -1;

  எனில்(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    எனில்(pport)
      *pport = -1;
    திரும்பு;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : வெற்று;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  போது(cf && !cf->connected) {
    எனில்((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  எனில்(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  எனில்(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                           கட்டமைப்பு Curl_easy *data,
                           முழு event, முழு arg1, வெறுமை *arg2)
{
  (வெறுமை)cf;
  (வெறுமை)data;
  (வெறுமை)event;
  (வெறுமை)arg1;
  (வெறுமை)arg2;
  திரும்பு CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            bool ignore_result,
                            முழு event, முழு arg1, வெறுமை *arg2)
{
  CURLcode result = CURLE_OK;

  க்கு(; cf; cf = cf->next) {
    எனில்(cf->cft->cntrl == Curl_cf_def_cntrl)
      தொடர்;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    எனில்(!ignore_result && result)
      நிறுத்து;
  }
  திரும்பு result;
}

curl_socket_t Curl_conn_cf_get_socket(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data)
{
  curl_socket_t sock;
  எனில்(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, வெற்று, &sock))
    திரும்பு sock;
  திரும்பு CURL_SOCKET_BAD;
}

unsigned எழுத்து Curl_conn_cf_get_transport(கட்டமைப்பு Curl_cfilter *cf,
                                         கட்டமைப்பு Curl_easy *data)
{
  முழு transport = 0;
  எனில்(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, வெற்று))
    திரும்பு (unsigned எழுத்து)transport;
  திரும்பு (unsigned எழுத்து)(data->conn ? data->conn->transport_wanted : 0);
}

மாறிலி எழுத்து *Curl_conn_cf_get_alpn_negotiated(கட்டமைப்பு Curl_cfilter *cf,
                                             கட்டமைப்பு Curl_easy *data)
{
  மாறிலி எழுத்து *alpn = வெற்று;
  CURL_TRC_CF(data, cf, "query ALPN");
  எனில்(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, வெற்று,
                           CURL_UNCONST(&alpn)))
    திரும்பு alpn;
  திரும்பு வெற்று;
}

நிலையான மாறிலி கட்டமைப்பு Curl_sockaddr_ex *cf_get_remote_addr(
  கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  மாறிலி கட்டமைப்பு Curl_sockaddr_ex *remote_addr = வெற்று;
  எனில்(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, வெற்று,
                     CURL_UNCONST(&remote_addr)))
    திரும்பு remote_addr;
  திரும்பு வெற்று;
}

CURLcode Curl_conn_cf_get_ip_info(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  bool *is_ipv6, கட்டமைப்பு ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  எனில்(cf) {
    முழு ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  திரும்பு result;
}

curl_socket_t Curl_conn_get_first_socket(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cfilter *cf;

  எனில்(!data->conn)
    திரும்பு CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  எனில்(cf && !cf->connected)
    திரும்பு Curl_conn_cf_get_socket(cf, data);
  திரும்பு data->conn->sock[FIRSTSOCKET];
}

மாறிலி கட்டமைப்பு Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  கட்டமைப்பு Curl_easy *data, int8_t sockindex)
{
  கட்டமைப்பு Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : வெற்று;
  திரும்பு cf ? cf_get_remote_addr(cf, data) : வெற்று;
}

CURLcode Curl_conn_ev_data_setup(கட்டமைப்பு Curl_easy *data)
{
  திரும்பு cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, வெற்று);
}

CURLcode Curl_conn_flush(கட்டமைப்பு Curl_easy *data, int8_t sockindex)
{
  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  திரும்பு Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
                            CF_CTRL_FLUSH, 0, வெற்று);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
வெறுமை Curl_conn_ev_data_done_send(கட்டமைப்பு Curl_easy *data)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE_SEND, 0, வெற்று);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
வெறுமை Curl_conn_ev_data_done(கட்டமைப்பு Curl_easy *data, bool premature)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE, premature, வெற்று);
}

CURLcode Curl_conn_ev_data_pause(கட்டமைப்பு Curl_easy *data, bool do_pause)
{
  திரும்பு cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, வெற்று);
}

bool Curl_conn_is_alive(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn,
                        bool *உள்ளீடு_pending)
{
  கட்டமைப்பு Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  திரும்பு cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, உள்ளீடு_pending);
}

CURLcode Curl_conn_keep_alive(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு connectdata *conn)
{
  CURLcode result = CURLE_OK;
  முழு i;

  க்கு(i = 0; (i < (முழு)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    கட்டமைப்பு Curl_cfilter *cf = conn->cfilter[i];
    எனில்(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  திரும்பு result;
}

size_t Curl_conn_get_max_concurrent(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு connectdata *conn,
                                    int8_t sockindex)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result;
  முழு n = -1;

  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, வெற்று) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  திரும்பு (result || n < 0) ? 1 : (size_t)n;
}

முழு Curl_conn_get_stream_error(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு connectdata *conn,
                               int8_t sockindex)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result;
  முழு n = 0;

  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, வெற்று) : CURLE_UNKNOWN_OPTION;
  திரும்பு (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(கட்டமைப்பு Curl_easy *data, curl_socket_t sockfd)
{
  எனில்(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    திரும்பு SECONDARYSOCKET;
  திரும்பு FIRSTSOCKET;
}

CURLcode Curl_conn_recv(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                        எழுத்து *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  எனில்(data && data->conn && data->conn->recv[sockindex])
    திரும்பு data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  திரும்பு CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                        மாறிலி வெறுமை *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  எனில்(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    மாறிலி எழுத்து *p = getenv("CURL_SMALLSENDS");
    எனில்(p) {
      curl_off_t altsize;
      எனில்(!curlx_str_எண்(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        எனில்(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  எனில்(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    எனில்(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      திரும்பு CURLE_SEND_ERROR;
    }
    திரும்பு result;
  }
  *pnwritten = 0;
  திரும்பு CURLE_FAILED_INIT;
}
