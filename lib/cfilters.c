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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "strerror.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "url.h"
#समावेश "curl_trc.h"
#समावेश "progress.h"
#समावेश "select.h"
#समावेश "curlx/strparse.h"

CURLcode Curl_cf_def_connect(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  अगर(cf->अग्र) {
    परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, तथ्य, done);
    अगर(परिणाम || !*done)
      वापस परिणाम;
  }

  cf->connected = TRUE;
  *done = TRUE;
  वापस CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य, bool *done)
{
  (व्योम)cf;
  (व्योम)तथ्य;
  *done = TRUE;
  वापस CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा easy_pollset *ps)
{
  /* NOP */
  (व्योम)cf;
  (व्योम)तथ्य;
  (व्योम)ps;
  वापस CURLE_OK;
}

bool Curl_cf_def_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                              स्थिर काष्ठा Curl_easy *तथ्य)
{
  वापस cf->अग्र ?
    cf->अग्र->cft->has_तथ्य_pending(cf->अग्र, तथ्य) : FALSE;
}

CURLcode Curl_cf_def_send(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                          स्थिर uint8_t *buf, माप_प्रकार len, bool eos,
                          माप_प्रकार *pnwritten)
{
  अगर(cf->अग्र)
    वापस cf->अग्र->cft->do_send(cf->अग्र, तथ्य, buf, len, eos, pnwritten);
  *pnwritten = 0;
  वापस CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                          अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  अगर(cf->अग्र)
    वापस cf->अग्र->cft->do_recv(cf->अग्र, तथ्य, buf, len, pnread);
  *pnread = 0;
  वापस CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               bool *निवेश_pending)
{
  वापस cf->अग्र ?
    cf->अग्र->cft->is_alive(cf->अग्र, तथ्य, निवेश_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य)
{
  वापस cf->अग्र ?
    cf->अग्र->cft->keep_alive(cf->अग्र, तथ्य) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(काष्ठा Curl_cfilter *cf,
                           काष्ठा Curl_easy *तथ्य,
                           पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#अगर_घोषित CURLVERBOSE
व्योम Curl_conn_trc_filters(काष्ठा Curl_easy *तथ्य,
                           int8_t sockindex, स्थिर अक्षर *info)
{
  अगर(CURL_TRC_M_is_verbose(तथ्य) && तथ्य->conn) {
    काष्ठा Curl_cfilter *cf = तथ्य->conn->cfilter[sockindex];

    अगर(cf) {
      अक्षर संदेश[256], *buf;
      पूर्णांक blen, n;

      buf = संदेश;
      blen = माप(संदेश) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      क्रम(; cf && blen; cf = cf->अग्र) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(तथ्य, "%s%s", संदेश, blen ? "" : "...");
    }
    अन्यथा
      CURL_TRC_M(तथ्य, "%s [%d][-]", info, sockindex);
  }
}
#पूर्ण_अगर /* CURLVERBOSE */

व्योम Curl_conn_cf_discard_chain(काष्ठा Curl_cfilter **pcf,
                                काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cfilter *cfn, *cf = *pcf;

  अगर(cf) {
    *pcf = शून्य;
    जबतक(cf) {
      cfn = cf->अग्र;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->अग्र = शून्य;
      cf->cft->destroy(cf, तथ्य);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

व्योम Curl_conn_cf_discard_all(काष्ठा Curl_easy *तथ्य,
                              काष्ठा connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], तथ्य);
}

CURLcode Curl_cf_recv(काष्ठा Curl_easy *तथ्य, int8_t sockindex, अक्षर *buf,
                      माप_प्रकार len, माप_प्रकार *pnread)
{
  काष्ठा Curl_cfilter *cf;

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);
  cf = तथ्य->conn->cfilter[sockindex];
  जबतक(cf && !cf->connected)
    cf = cf->अग्र;
  अगर(cf)
    वापस cf->cft->do_recv(cf, तथ्य, buf, len, pnread);
  failf(तथ्य, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  वापस CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                      स्थिर uint8_t *buf, माप_प्रकार len, bool eos,
                      माप_प्रकार *pnwritten)
{
  काष्ठा Curl_cfilter *cf;

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);
  cf = तथ्य->conn->cfilter[sockindex];
  जबतक(cf && !cf->connected)
    cf = cf->अग्र;
  अगर(cf) {
    वापस cf->cft->do_send(cf, तथ्य, buf, len, eos, pnwritten);
  }
  failf(तथ्य, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  वापस CURLE_FAILED_INIT;
}

काष्ठा cf_io_ctx {
  काष्ठा Curl_easy *तथ्य;
  काष्ठा Curl_cfilter *cf;
};

अटल CURLcode cf_bufq_reader(व्योम *writer_ctx,
                               अचिन्हित अक्षर *buf, माप_प्रकार blen,
                               माप_प्रकार *pnread)
{
  काष्ठा cf_io_ctx *io = writer_ctx;
  वापस Curl_conn_cf_recv(io->cf, io->तथ्य, (अक्षर *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(काष्ठा Curl_cfilter *cf,
                           काष्ठा Curl_easy *तथ्य,
                           काष्ठा bufq *bufq,
                           माप_प्रकार maxlen,
                           माप_प्रकार *pnread)
{
  काष्ठा cf_io_ctx io;

  अगर(!cf || !तथ्य) {
    *pnread = 0;
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.तथ्य = तथ्य;
  io.cf = cf;
  वापस Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

अटल CURLcode cf_bufq_writer(व्योम *writer_ctx,
                               स्थिर uint8_t *buf, माप_प्रकार buflen,
                               माप_प्रकार *pnwritten)
{
  काष्ठा cf_io_ctx *io = writer_ctx;
  वापस Curl_conn_cf_send(io->cf, io->तथ्य, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(काष्ठा Curl_cfilter *cf,
                           काष्ठा Curl_easy *तथ्य,
                           काष्ठा bufq *bufq,
                           स्थिर अचिन्हित अक्षर *buf, माप_प्रकार blen,
                           माप_प्रकार *pnwritten)
{
  काष्ठा cf_io_ctx io;

  अगर(!cf || !तथ्य) {
    *pnwritten = 0;
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.तथ्य = तथ्य;
  io.cf = cf;
  अगर(buf && blen)
    वापस Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  अन्यथा
    वापस Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(काष्ठा Curl_cfilter **pcf,
                        स्थिर काष्ठा Curl_cftype *cft,
                        व्योम *ctx)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, माप(*cf));
  अगर(!cf)
    जाओ out;

  cf->cft = cft;
  cf->ctx = ctx;
  परिणाम = CURLE_OK;
out:
  *pcf = cf;
  वापस परिणाम;
}

व्योम Curl_conn_cf_add(काष्ठा Curl_easy *तथ्य,
                      काष्ठा connectdata *conn,
                      int8_t sockindex,
                      काष्ठा Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->अग्र);

  cf->अग्र = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(तथ्य, cf, "added");
}

व्योम Curl_conn_cf_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                               काष्ठा Curl_cfilter *cf_new)
{
  काष्ठा Curl_cfilter *पुच्छ, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  पुच्छ = cf_at->अग्र;
  cf_at->अग्र = cf_new;
  करो {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->अग्र;
    cf_new = cf_new->अग्र;
  } जबतक(cf_new);
  *pnext = पुच्छ;
}

bool Curl_conn_cf_discard(काष्ठा Curl_cfilter **pcf,
                          काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cfilter *cf = pcf ? *pcf : शून्य;
  bool found = FALSE;
  अगर(cf) {
    अगर(cf->conn) {
      /* unlink if present in connection filter chain */
      काष्ठा Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      जबतक(*pprev) {
        अगर(*pprev == *pcf) {
          *pprev = (*pcf)->अग्र;
          cf->अग्र = शून्य;
          found = TRUE;
          अवरोध;
        }
        pprev = &(*pprev)->अग्र;
      }
    }
    Curl_conn_cf_discard_chain(pcf, तथ्य);
  }
  वापस found;
}

CURLcode Curl_conn_cf_connect(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              bool *done)
{
  अगर(cf)
    वापस cf->cft->do_connect(cf, तथ्य, done);
  वापस CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                           स्थिर uint8_t *buf, माप_प्रकार len, bool eos,
                           माप_प्रकार *pnwritten)
{
  अगर(cf)
    वापस cf->cft->do_send(cf, तथ्य, buf, len, eos, pnwritten);
  *pnwritten = 0;
  वापस CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                           अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  अगर(cf)
    वापस cf->cft->do_recv(cf, तथ्य, buf, len, pnread);
  *pnread = 0;
  वापस CURLE_RECV_ERROR;
}

अटल CURLcode cf_cntrl_all(काष्ठा connectdata *conn,
                             काष्ठा Curl_easy *तथ्य,
                             bool ignore_परिणाम,
                             पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक i;

  क्रम(i = 0; i < (पूर्णांक)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    परिणाम = Curl_conn_cf_cntrl(conn->cfilter[i], तथ्य, ignore_परिणाम,
                                event, arg1, arg2);
    अगर(!ignore_परिणाम && परिणाम)
      अवरोध;
  }
  वापस परिणाम;
}

bool Curl_conn_cf_is_ip_connected(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य)
{
  अगर(cf) {
    पूर्णांक मूल्य = 0;
    अगर(!cf->cft->query(cf, तथ्य, CF_QUERY_REALLY_CONNECTED, &मूल्य, शून्य))
      वापस !!मूल्य;
  }
  वापस FALSE;
}

व्योम Curl_conn_cntrl_update_info(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *conn)
{
  cf_cntrl_all(conn, तथ्य, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, शून्य);
}

व्योम Curl_conn_cntrl_report_stats(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn,
                                  पूर्णांक sockindex)
{
  अगर((अचिन्हित)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (व्योम)Curl_conn_cf_cntrl(conn->cfilter[sockindex], तथ्य, TRUE,
                             CF_CTRL_REPORT_STATS, 0, शून्य);
}

व्योम Curl_conn_remove_setup_filters(काष्ठा Curl_easy *तथ्य,
                                    int8_t sockindex)
{
  काष्ठा Curl_cfilter **anchor = &तथ्य->conn->cfilter[sockindex];
  जबतक(*anchor) {
    काष्ठा Curl_cfilter *cf = *anchor;
    अगर(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->अग्र;
      cf->अग्र = शून्य;
      CURL_TRC_CF(तथ्य, cf, "removing connected setup filter");
      cf->cft->destroy(cf, तथ्य);
      curlx_free(cf);
    }
    अन्यथा
      anchor = &cf->अग्र;
  }
}

bool Curl_conn_is_setup(काष्ठा connectdata *conn, int8_t sockindex)
{
  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस FALSE;
  वापस !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(काष्ठा connectdata *conn, int8_t sockindex)
{
  काष्ठा Curl_cfilter *cf;

  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस FALSE;
  cf = conn->cfilter[sockindex];
  अगर(cf)
    वापस (bool)cf->connected;
  अन्यथा अगर(conn->scheme->flags & PROTOPT_NONETWORK)
    वापस TRUE;
  वापस FALSE;
}

bool Curl_conn_is_ip_connected(काष्ठा Curl_easy *तथ्य, int8_t sockindex)
{
  काष्ठा Curl_cfilter *cf;

  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस FALSE;
  cf = तथ्य->conn->cfilter[sockindex];
  जबतक(cf) {
    अगर(cf->connected)
      वापस TRUE;
    अगर(cf->cft->flags & CF_TYPE_IP_CONNECT)
      वापस FALSE;
    cf = cf->अग्र;
  }
  वापस FALSE;
}

#अगर_अघोषित CURL_DISABLE_PROXY
अटल bool cf_is_tunneling(काष्ठा Curl_cfilter *cf)
{
  क्रम(; cf; cf = cf->अग्र) {
    अगर((cf->cft->flags & CF_TYPE_PROXY))
      वापस TRUE;
  }
  वापस FALSE;
}

bool Curl_conn_is_tunneling(काष्ठा connectdata *conn, int8_t sockindex)
{
  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस FALSE;
  वापस conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#अन्यथा
bool Curl_conn_is_tunneling(काष्ठा connectdata *conn, int8_t sockindex)
{
  (व्योम)conn;
  (व्योम)sockindex;
  वापस FALSE;
}
#पूर्ण_अगर /* CURL_DISABLE_PROXY */

अटल bool cf_is_ssl(काष्ठा Curl_cfilter *cf)
{
  क्रम(; cf; cf = cf->अग्र) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    अगर((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      वापस TRUE;
    अगर(cf->cft->flags & CF_TYPE_IP_CONNECT)
      वापस FALSE;
  }
  वापस FALSE;
}

bool Curl_conn_is_ssl(काष्ठा connectdata *conn, int8_t sockindex)
{
  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस FALSE;
  वापस conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(काष्ठा Curl_easy *तथ्य,
                            काष्ठा connectdata *conn, int8_t sockindex,
                            पूर्णांक query,
                            काष्ठा curl_tlssessioninfo *info)
{
  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस FALSE;
  अगर(Curl_conn_is_ssl(conn, sockindex)) {
    काष्ठा Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode परिणाम = cf ?
      cf->cft->query(cf, तथ्य, query, शून्य, (व्योम *)info) :
      CURLE_UNKNOWN_OPTION;
    वापस !परिणाम;
  }
  वापस FALSE;
}

CURLcode Curl_conn_get_ip_info(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, काष्ठा ip_quadruple *ipquad)
{
  काष्ठा Curl_cfilter *cf;
  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : शून्य;
  वापस Curl_conn_cf_get_ip_info(cf, तथ्य, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(काष्ठा connectdata *conn, int8_t sockindex)
{
  काष्ठा Curl_cfilter *cf;

  अगर(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    वापस FALSE;
  क्रम(cf = conn->cfilter[sockindex]; cf; cf = cf->अग्र) {
    अगर(cf->cft->flags & CF_TYPE_MULTIPLEX)
      वापस TRUE;
    अगर(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      वापस FALSE;
  }
  वापस FALSE;
}

अचिन्हित अक्षर Curl_conn_get_transport(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn)
{
  काष्ठा Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  वापस Curl_conn_cf_get_transport(cf, तथ्य);
}

पूर्णांक Curl_socktype_for_transport(uint8_t transport)
{
  चयन(transport) {
  हाल TRNSPRT_TCP:
    वापस SOCK_STREAM;
  हाल TRNSPRT_UNIX:
    वापस SOCK_STREAM;
  शेष: /* UDP and QUIC */
    वापस SOCK_DGRAM;
  }
}

पूर्णांक Curl_protocol_for_transport(uint8_t transport)
{
  चयन(transport) {
  हाल TRNSPRT_TCP:
    वापस IPPROTO_TCP;
  हाल TRNSPRT_UNIX:
    वापस IPPROTO_IP;
  शेष: /* UDP and QUIC */
    वापस IPPROTO_UDP;
  }
}

स्थिर अक्षर *Curl_conn_get_alpn_negotiated(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा connectdata *conn)
{
  काष्ठा Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  वापस Curl_conn_cf_get_alpn_negotiated(cf, तथ्य);
}

अचिन्हित अक्षर Curl_conn_http_version(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा connectdata *conn)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_UNKNOWN_OPTION;
  अचिन्हित अक्षर v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  क्रम(; cf; cf = cf->अग्र) {
    अगर(cf->cft->flags & CF_TYPE_HTTP) {
      पूर्णांक मूल्य = 0;
      परिणाम = cf->cft->query(cf, तथ्य, CF_QUERY_HTTP_VERSION, &मूल्य, शून्य);
      अगर(!परिणाम && ((मूल्य < 0) || (मूल्य > 255)))
        परिणाम = CURLE_FAILED_INIT;
      अन्यथा
        v = (अचिन्हित अक्षर)मूल्य;
      अवरोध;
    }
    अगर(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      अवरोध;
  }
  वापस (अचिन्हित अक्षर)(परिणाम ? 0 : v);
}

bool Curl_conn_तथ्य_pending(काष्ठा Curl_easy *तथ्य, int8_t sockindex)
{
  काष्ठा Curl_cfilter *cf;

  (व्योम)तथ्य;
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);
  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस FALSE;

  cf = तथ्य->conn->cfilter[sockindex];
  जबतक(cf && !cf->connected) {
    cf = cf->अग्र;
  }
  अगर(cf) {
    वापस cf->cft->has_तथ्य_pending(cf, तथ्य);
  }
  वापस FALSE;
}

bool Curl_conn_cf_needs_flush(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम;
  पूर्णांक pending = 0;
  परिणाम = cf ? cf->cft->query(cf, तथ्य, CF_QUERY_NEED_FLUSH,
                               &pending, शून्य) : CURLE_UNKNOWN_OPTION;
  वापस (परिणाम || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(काष्ठा Curl_easy *तथ्य, int8_t sockindex)
{
  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस FALSE;
  वापस Curl_conn_cf_needs_flush(तथ्य->conn->cfilter[sockindex], तथ्य);
}

CURLcode Curl_conn_cf_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य,
                                     काष्ठा easy_pollset *ps)
{
  CURLcode परिणाम = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  क्रम(; cf && !परिणाम; cf = cf->अग्र) {
    अगर(cf->shutdown)
      जारी;
    अगर(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->अग्र || cf->अग्र->connected)
      परिणाम = cf->cft->adjust_pollset(cf, तथ्य, ps);
  }
  वापस परिणाम;
}

CURLcode Curl_conn_adjust_pollset(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn,
                                  काष्ठा easy_pollset *ps)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर bool want_io = !!ps->n;
  int8_t i;

  DEBUGASSERT(तथ्य);
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
  क्रम(i = 0; (i < (पूर्णांक)CURL_ARRAYSIZE(conn->cfilter)) && !परिणाम; ++i) {
    अगर(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      परिणाम = Curl_conn_cf_adjust_pollset(conn->cfilter[i], तथ्य, ps);
  }
  वापस परिणाम;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
पूर्णांक Curl_conn_cf_poll(काष्ठा Curl_cfilter *cf,
                      काष्ठा Curl_easy *तथ्य,
                      timediff_t timeout_ms)
{
  काष्ठा easy_pollset ps;
  पूर्णांक rc;
  CURLcode परिणाम;

  DEBUGASSERT(cf);
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);
  Curl_pollset_init(&ps);

  परिणाम = Curl_conn_cf_adjust_pollset(cf, तथ्य, &ps);
  अगर(!परिणाम)
    rc = Curl_pollset_poll(तथ्य, &ps, timeout_ms);
  अन्यथा
    rc = -1;
  Curl_pollset_cleanup(&ps);
  वापस rc;
}

व्योम Curl_conn_get_current_host(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                                स्थिर अक्षर **phost, पूर्णांक *pport)
{
  काष्ठा Curl_cfilter *cf, *cf_proxy = शून्य;
  पूर्णांक portarg = -1;

  अगर(!तथ्य->conn) {
    DEBUGASSERT(0);
    *phost = "";
    अगर(pport)
      *pport = -1;
    वापस;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? तथ्य->conn->cfilter[sockindex] : शून्य;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  जबतक(cf && !cf->connected) {
    अगर((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->अग्र;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  अगर(!cf_proxy || cf_proxy->cft->query(cf_proxy, तथ्य, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = तथ्य->conn->origin->hostname;
    portarg = तथ्य->conn->origin->port;
  }
  अगर(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(काष्ठा Curl_cfilter *cf,
                           काष्ठा Curl_easy *तथ्य,
                           पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  (व्योम)cf;
  (व्योम)तथ्य;
  (व्योम)event;
  (व्योम)arg1;
  (व्योम)arg2;
  वापस CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            bool ignore_परिणाम,
                            पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  CURLcode परिणाम = CURLE_OK;

  क्रम(; cf; cf = cf->अग्र) {
    अगर(cf->cft->cntrl == Curl_cf_def_cntrl)
      जारी;
    परिणाम = cf->cft->cntrl(cf, तथ्य, event, arg1, arg2);
    अगर(!ignore_परिणाम && परिणाम)
      अवरोध;
  }
  वापस परिणाम;
}

curl_socket_t Curl_conn_cf_get_socket(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य)
{
  curl_socket_t sock;
  अगर(cf && !cf->cft->query(cf, तथ्य, CF_QUERY_SOCKET, शून्य, &sock))
    वापस sock;
  वापस CURL_SOCKET_BAD;
}

अचिन्हित अक्षर Curl_conn_cf_get_transport(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य)
{
  पूर्णांक transport = 0;
  अगर(cf && !cf->cft->query(cf, तथ्य, CF_QUERY_TRANSPORT, &transport, शून्य))
    वापस (अचिन्हित अक्षर)transport;
  वापस (अचिन्हित अक्षर)(तथ्य->conn ? तथ्य->conn->transport_wanted : 0);
}

स्थिर अक्षर *Curl_conn_cf_get_alpn_negotiated(काष्ठा Curl_cfilter *cf,
                                             काष्ठा Curl_easy *तथ्य)
{
  स्थिर अक्षर *alpn = शून्य;
  CURL_TRC_CF(तथ्य, cf, "query ALPN");
  अगर(cf && !cf->cft->query(cf, तथ्य, CF_QUERY_ALPN_NEGOTIATED, शून्य,
                           CURL_UNCONST(&alpn)))
    वापस alpn;
  वापस शून्य;
}

अटल स्थिर काष्ठा Curl_sockaddr_ex *cf_get_remote_addr(
  काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  स्थिर काष्ठा Curl_sockaddr_ex *remote_addr = शून्य;
  अगर(cf &&
     !cf->cft->query(cf, तथ्य, CF_QUERY_REMOTE_ADDR, शून्य,
                     CURL_UNCONST(&remote_addr)))
    वापस remote_addr;
  वापस शून्य;
}

CURLcode Curl_conn_cf_get_ip_info(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  bool *is_ipv6, काष्ठा ip_quadruple *ipquad)
{
  CURLcode परिणाम = CURLE_UNKNOWN_OPTION;
  अगर(cf) {
    पूर्णांक ipv6 = 0;
    परिणाम = cf->cft->query(cf, तथ्य, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  वापस परिणाम;
}

curl_socket_t Curl_conn_get_first_socket(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cfilter *cf;

  अगर(!तथ्य->conn)
    वापस CURL_SOCKET_BAD;

  cf = तथ्य->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  अगर(cf && !cf->connected)
    वापस Curl_conn_cf_get_socket(cf, तथ्य);
  वापस तथ्य->conn->sock[FIRSTSOCKET];
}

स्थिर काष्ठा Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  काष्ठा Curl_easy *तथ्य, int8_t sockindex)
{
  काष्ठा Curl_cfilter *cf =
    (तथ्य->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    तथ्य->conn->cfilter[sockindex] : शून्य;
  वापस cf ? cf_get_remote_addr(cf, तथ्य) : शून्य;
}

CURLcode Curl_conn_ev_तथ्य_setup(काष्ठा Curl_easy *तथ्य)
{
  वापस cf_cntrl_all(तथ्य->conn, तथ्य, FALSE, CF_CTRL_तथ्य_SETUP, 0, शून्य);
}

CURLcode Curl_conn_flush(काष्ठा Curl_easy *तथ्य, int8_t sockindex)
{
  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  वापस Curl_conn_cf_cntrl(तथ्य->conn->cfilter[sockindex], तथ्य, FALSE,
                            CF_CTRL_FLUSH, 0, शून्य);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
व्योम Curl_conn_ev_तथ्य_done_send(काष्ठा Curl_easy *तथ्य)
{
  cf_cntrl_all(तथ्य->conn, तथ्य, TRUE, CF_CTRL_तथ्य_DONE_SEND, 0, शून्य);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
व्योम Curl_conn_ev_तथ्य_done(काष्ठा Curl_easy *तथ्य, bool premature)
{
  cf_cntrl_all(तथ्य->conn, तथ्य, TRUE, CF_CTRL_तथ्य_DONE, premature, शून्य);
}

CURLcode Curl_conn_ev_तथ्य_pause(काष्ठा Curl_easy *तथ्य, bool do_pause)
{
  वापस cf_cntrl_all(तथ्य->conn, तथ्य, FALSE,
                      CF_CTRL_तथ्य_PAUSE, do_pause, शून्य);
}

bool Curl_conn_is_alive(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                        bool *निवेश_pending)
{
  काष्ठा Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  वापस cf && !cf->conn->bits.बंद &&
         cf->cft->is_alive(cf, तथ्य, निवेश_pending);
}

CURLcode Curl_conn_keep_alive(काष्ठा Curl_easy *तथ्य,
                              काष्ठा connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक i;

  क्रम(i = 0; (i < (पूर्णांक)CURL_ARRAYSIZE(conn->cfilter)) && !परिणाम; ++i) {
    काष्ठा Curl_cfilter *cf = conn->cfilter[i];
    अगर(cf)
      परिणाम = cf->cft->keep_alive(cf, तथ्य);
  }
  वापस परिणाम;
}

माप_प्रकार Curl_conn_get_max_concurrent(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा connectdata *conn,
                                    int8_t sockindex)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम;
  पूर्णांक n = -1;

  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस 0;

  cf = conn->cfilter[sockindex];
  परिणाम = cf ? cf->cft->query(cf, तथ्य, CF_QUERY_MAX_CONCURRENT,
                               &n, शून्य) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  वापस (परिणाम || n < 0) ? 1 : (माप_प्रकार)n;
}

पूर्णांक Curl_conn_get_stream_error(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn,
                               int8_t sockindex)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम;
  पूर्णांक n = 0;

  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस 0;

  cf = conn->cfilter[sockindex];
  परिणाम = cf ? cf->cft->query(cf, तथ्य, CF_QUERY_STREAM_ERROR,
                               &n, शून्य) : CURLE_UNKNOWN_OPTION;
  वापस (परिणाम || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(काष्ठा Curl_easy *तथ्य, curl_socket_t sockfd)
{
  अगर(तथ्य && तथ्य->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == तथ्य->conn->sock[SECONDARYSOCKET])
    वापस SECONDARYSOCKET;
  वापस FIRSTSOCKET;
}

CURLcode Curl_conn_recv(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                        अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);
  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  अगर(तथ्य && तथ्य->conn && तथ्य->conn->recv[sockindex])
    वापस तथ्य->conn->recv[sockindex](तथ्य, sockindex, buf, len, pnread);
  *pnread = 0;
  वापस CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                        स्थिर व्योम *buf, माप_प्रकार len, bool eos,
                        माप_प्रकार *pnwritten)
{
  माप_प्रकार write_len = len;

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
#अगर_घोषित DEBUGBUILD
  अगर(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    स्थिर अक्षर *p = दो_पर्या("CURL_SMALLSENDS");
    अगर(p) {
      curl_off_t altsize;
      अगर(!curlx_str_संख्या(&p, &altsize, write_len)) {
        write_len = (माप_प्रकार)altsize;
        अगर(write_len != len)
          eos = FALSE;
      }
    }
  }
#पूर्ण_अगर
  अगर(तथ्य && तथ्य->conn && तथ्य->conn->send[sockindex]) {
    CURLcode परिणाम = तथ्य->conn->send[sockindex](तथ्य, sockindex, buf,
                                                  write_len, eos, pnwritten);
    अगर(!परिणाम && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      वापस CURLE_SEND_ERROR;
    }
    वापस परिणाम;
  }
  *pnwritten = 0;
  वापस CURLE_FAILED_INIT;
}
