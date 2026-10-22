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
#समावेश "curl_trc.h"
#समावेश "strerror.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "cf-https-connect.h"
#समावेश "cf-setup.h"
#समावेश "multiif.h"
#समावेश "progress.h"
#समावेश "conncache.h"
#समावेश "multihandle.h"
#समावेश "select.h"
#समावेश "vdns/cf-dns.h"
#समावेश "curlx/strparse.h"

#अगर !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

क्रमागत alpnid Curl_alpn2alpnid(स्थिर अचिन्हित अक्षर *name, माप_प्रकार len)
{
  अगर(len == 2) {
    अगर(!स_भेद(name, "h1", 2))
      वापस ALPN_h1;
    अगर(!स_भेद(name, "h2", 2))
      वापस ALPN_h2;
    अगर(!स_भेद(name, "h3", 2))
      वापस ALPN_h3;
  }
  अन्यथा अगर(len == 8 && !स_भेद(name, "http/1.1", 8))
    वापस ALPN_h1;
  वापस ALPN_none; /* unknown, probably rubbish input */
}

#पूर्ण_अगर

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
अटल timediff_t connect_बायाँ_ms(काष्ठा Curl_easy *तथ्य,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (तथ्य->set.connecttimeout > 0) ?
    तथ्य->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t बायाँ_ms = timeout_ms - used_ms;
  वापस बायाँ_ms ? बायाँ_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(काष्ठा Curl_easy *तथ्य,
                                    स्थिर काष्ठा curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(काष्ठा Curl_easy *तथ्य,
                                    स्थिर काष्ठा curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  अगर(तथ्य->conn && CURL_CONN_IN_SHUTDOWN(तथ्य->conn, FIRSTSOCKET))
    वापस Curl_cshutdn_timeleft_ms(तथ्य, तथ्य->conn, FIRSTSOCKET);
  अन्यथा अगर(Curl_is_connecting(तथ्य)) {
    ctimeleft_ms = connect_बायाँ_ms(तथ्य,
      Curl_pgrs_since_ms(तथ्य, pnow, TIMER_STARTSINGLE));
  }
  अन्यथा अगर(!तथ्य->set.timeout || तथ्य->set.connect_only) {
    वापस 0; /* no timeout in place or checked, return "no limit" */
  }

  अगर(तथ्य->set.timeout) {
    timeleft_ms = तथ्य->set.timeout -
                  Curl_pgrs_since_ms(तथ्य, pnow, TIMER_STARTOP);
    अगर(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  अगर(!ctimeleft_ms)
    वापस timeleft_ms;
  अन्यथा अगर(!timeleft_ms)
    वापस ctimeleft_ms;
  वापस CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(काष्ठा Curl_easy *तथ्य)
{
  वापस timeleft_now_ms(तथ्य, Curl_pgrs_now(तथ्य));
}

timediff_t Curl_timeleft_now_ms(काष्ठा Curl_easy *तथ्य,
                                स्थिर काष्ठा curltime *pnow)
{
  वापस timeleft_now_ms(तथ्य, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  काष्ठा connectdata *conn = Curl_cpool_get_last_conn(तथ्य);
  अगर(connp)
    *connp = conn;
  वापस conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

व्योम Curl_conncontrol(काष्ठा connectdata *conn, पूर्णांक ctrl)
{
  अगर(!conn) {
    DEBUGASSERT(0);
    वापस;
  }
  चयन(ctrl) {
    हाल CONNCTRL_CONN_KEEP:
      conn->bits.बंद = FALSE;
      अवरोध;
    हाल CONNCTRL_CONN_CLOSE:
      conn->bits.बंद = TRUE;
      अवरोध;
    हाल CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      अगर(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.बंद = TRUE;
      अवरोध;
    शेष:
      DEBUGASSERT(0);
      अवरोध;
  }
}

CURLcode Curl_conn_setup(काष्ठा Curl_easy *तथ्य,
                         काष्ठा connectdata *conn,
                         int8_t sockindex,
                         पूर्णांक ssl_mode)
{
  काष्ठा Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode परिणाम = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  अगर(!first_peer)
    वापस CURLE_FAILED_INIT;

#अगर_अघोषित CURL_DISABLE_HTTP
  अगर(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    परिणाम = Curl_cf_https_setup(
      तथ्य, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    अगर(परिणाम)
      जाओ out;
  }
#पूर्ण_अगर /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  अगर(!conn->cfilter[sockindex]) {
    परिणाम = Curl_cf_setup_add(तथ्य, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    अगर(परिणाम)
      जाओ out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(तथ्य, conn->ip_version);
  परिणाम = Curl_conn_dns_add_addr_resolve(तथ्य, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  अगर(परिणाम)
    जाओ out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  वापस परिणाम;
}

#अगर_घोषित CURLVERBOSE
अटल CURLcode conn_connect_trace(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा Curl_cfilter *cf)
{
  अगर(Curl_trc_is_verbose(तथ्य)) {
    काष्ठा ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode परिणाम;

    परिणाम = Curl_conn_cf_get_ip_info(cf, तथ्य, &is_ipv6, &ipquad);
    अगर(परिणाम)
      वापस परिणाम;

    infof(तथ्य, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(तथ्य->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  वापस CURLE_OK;
}
#पूर्ण_अगर

/**
 * Update connection statistics
 */
अटल व्योम conn_report_stats(काष्ठा Curl_easy *तथ्य, पूर्णांक sockindex)
{
  /* We do gather stats for the second socket...yet */
  अगर(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(तथ्य, तथ्य->conn, sockindex);
  }
}

व्योम Curl_conn_report_failed(काष्ठा Curl_easy *तथ्य, int8_t sockindex)
{
  अगर(!तथ्य->conn || (sockindex != FIRSTSOCKET))
    वापस;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (व्योम)Curl_conn_cf_cntrl(तथ्य->conn->cfilter[sockindex], तथ्य, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, शून्य);
  conn_report_stats(तथ्य, sockindex);
}

CURLcode Curl_conn_connect(काष्ठा Curl_easy *तथ्य,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#घोषणा CF_CONN_अंक_POLLS_ON_ढेर 5
  काष्ठा pollfd a_few_on_ढेर[CF_CONN_अंक_POLLS_ON_ढेर];
  काष्ठा easy_pollset ps;
  काष्ठा curl_pollfds cpfds;
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);
  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  अगर(तथ्य->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  cf = तथ्य->conn->cfilter[sockindex];
  अगर(!cf) {
    *done = FALSE;
    वापस CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  अगर(*done)
    वापस CURLE_OK;

  अगर(!तथ्य->conn->connect_started.set[sockindex]) {
    तथ्य->conn->connect_started.at[sockindex] = *Curl_pgrs_now(तथ्य);
    तथ्य->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ढेर, CF_CONN_अंक_POLLS_ON_ढेर);
  जबतक(!*done) {
    अगर(Curl_conn_needs_flush(तथ्य, sockindex)) {
      DEBUGF(infof(तथ्य, "Curl_conn_connect(index=%d), flush", sockindex));
      परिणाम = Curl_conn_flush(तथ्य, sockindex);
      अगर(परिणाम && (परिणाम != CURLE_AGAIN))
        जाओ out;
    }

    परिणाम = cf->cft->do_connect(cf, तथ्य, done);
    CURL_TRC_CF(तथ्य, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (पूर्णांक)परिणाम, *done);
    अगर(!परिणाम && *done) {
      /* A final sanity check on connection security */
      अगर((तथ्य->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(तथ्य->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(तथ्य, "transfer requires SSL, but not connected via SSL");
        परिणाम = CURLE_FAILED_INIT;
        जाओ out;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(तथ्य, तथ्य->conn);
      conn_report_stats(तथ्य, sockindex);
      Curl_cpool_conn_was_used(तथ्य, तथ्य->conn, Curl_pgrs_now(तथ्य));
      VERBOSE(परिणाम = conn_connect_trace(तथ्य, cf));
      VERBOSE(Curl_conn_trc_filters(तथ्य, sockindex, "connected"));
      Curl_conn_remove_setup_filters(तथ्य, sockindex);
      VERBOSE(Curl_conn_trc_filters(तथ्य, sockindex, "reduced to"));
      जाओ out;
    }
    अन्यथा अगर(परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "Curl_conn_connect(), filter returned %d",
                  (पूर्णांक)परिणाम);
      VERBOSE(Curl_conn_trc_filters(तथ्य, sockindex, "failed to connect"));
      Curl_conn_report_failed(तथ्य, sockindex);
      जाओ out;
    }

    अगर(!blocking)
      जाओ out;
    अन्यथा {
      /* check allowed time left */
      स्थिर काष्ठा curltime *pnow = Curl_pgrs_now(तथ्य);
      timediff_t timeout_ms = Curl_timeleft_now_ms(तथ्य, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, तथ्य);
      पूर्णांक rc;

      अगर(!Curl_is_connecting(तथ्य)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        स्थिर काष्ठा curltime *started =
          &तथ्य->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_बायाँ_ms(तथ्य,
          curlx_ptimediff_ms(pnow, started));
        अगर(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      अगर(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(तथ्य, "connect timeout");
        परिणाम = CURLE_OPERATION_TIMEDOUT;
        जाओ out;
      }

      CURL_TRC_CF(तथ्य, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      अगर(sockfd != CURL_SOCKET_BAD)
        परिणाम = Curl_pollset_set_out_only(तथ्य, &ps, sockfd);
      अगर(!परिणाम)
        परिणाम = Curl_conn_adjust_pollset(तथ्य, तथ्य->conn, &ps);
      अगर(परिणाम)
        जाओ out;
      परिणाम = Curl_pollfds_add_ps(&cpfds, &ps);
      अगर(परिणाम)
        जाओ out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(तथ्य, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      अगर(rc < 0) {
        परिणाम = CURLE_COULDNT_CONNECT;
        जाओ out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  वापस परिणाम;
}

व्योम Curl_conn_set_multiplex(काष्ठा connectdata *conn)
{
  अगर(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    अगर(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

काष्ठा Curl_peer *Curl_conn_get_origin(काष्ठा connectdata *conn,
                                       int8_t sockindex)
{
  वापस (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

काष्ठा Curl_peer *Curl_conn_get_destination(काष्ठा connectdata *conn,
                                            int8_t sockindex)
{
  वापस (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

काष्ठा Curl_peer *Curl_conn_get_first_peer(काष्ठा connectdata *conn,
                                           int8_t sockindex)
{
#अगर_अघोषित CURL_DISABLE_PROXY
  अगर(conn->socks_proxy.peer)
    वापस conn->socks_proxy.peer;
  अगर(conn->http_proxy.peer)
    वापस conn->http_proxy.peer;
#पूर्ण_अगर
  वापस (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
