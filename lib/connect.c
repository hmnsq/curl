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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "strerror.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "cf-https-connect.h"
#అన్తర్భావ "cf-setup.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "conncache.h"
#అన్తర్భావ "multihandle.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "curlx/strparse.h"

#ఒకవేళ !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

గణన alpnid Curl_alpn2alpnid(స్థిరరాశి గుర్తులేని అక్షరం *name, size_t len)
{
  ఒకవేళ(len == 2) {
    ఒకవేళ(!memcmp(name, "h1", 2))
      తిరిగి ALPN_h1;
    ఒకవేళ(!memcmp(name, "h2", 2))
      తిరిగి ALPN_h2;
    ఒకవేళ(!memcmp(name, "h3", 2))
      తిరిగి ALPN_h3;
  }
  లేకపోతే ఒకవేళ(len == 8 && !memcmp(name, "http/1.1", 8))
    తిరిగి ALPN_h1;
  తిరిగి ALPN_none; /* unknown, probably rubbish input */
}

#అయితే_ముగింపు

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
స్థిర timediff_t connect_ఎడమ_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                  timediff_t వాడిన_ms)
{
  timediff_t timeout_ms = (దత్తాంశం->సమితి.connecttimeout > 0) ?
    దత్తాంశం->సమితి.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t ఎడమ_ms = timeout_ms - వాడిన_ms;
  తిరిగి ఎడమ_ms ? ఎడమ_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                    స్థిరరాశి నిర్మాణం curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                    స్థిరరాశి నిర్మాణం curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  ఒకవేళ(దత్తాంశం->conn && CURL_CONN_లోపల_SHUTDOWN(దత్తాంశం->conn, FIRSTSOCKET))
    తిరిగి Curl_cshutdn_timeleft_ms(దత్తాంశం, దత్తాంశం->conn, FIRSTSOCKET);
  లేకపోతే ఒకవేళ(Curl_is_connecting(దత్తాంశం)) {
    ctimeleft_ms = connect_ఎడమ_ms(దత్తాంశం,
      Curl_pgrs_since_ms(దత్తాంశం, pnow, TIMER_STARTSINGLE));
  }
  లేకపోతే ఒకవేళ(!దత్తాంశం->సమితి.timeout || దత్తాంశం->సమితి.connect_only) {
    తిరిగి 0; /* no timeout in place or checked, return "no limit" */
  }

  ఒకవేళ(దత్తాంశం->సమితి.timeout) {
    timeleft_ms = దత్తాంశం->సమితి.timeout -
                  Curl_pgrs_since_ms(దత్తాంశం, pnow, TIMER_STARTOP);
    ఒకవేళ(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  ఒకవేళ(!ctimeleft_ms)
    తిరిగి timeleft_ms;
  లేకపోతే ఒకవేళ(!timeleft_ms)
    తిరిగి ctimeleft_ms;
  తిరిగి CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి timeleft_now_ms(దత్తాంశం, Curl_pgrs_now(దత్తాంశం));
}

timediff_t Curl_timeleft_now_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                స్థిరరాశి నిర్మాణం curltime *pnow)
{
  తిరిగి timeleft_now_ms(దత్తాంశం, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  నిర్మాణం connectdata *conn = Curl_cpool_get_చివరి_conn(దత్తాంశం);
  ఒకవేళ(connp)
    *connp = conn;
  తిరిగి conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

శూన్య Curl_conncontrol(నిర్మాణం connectdata *conn, పూర్ణాంకం ctrl)
{
  ఒకవేళ(!conn) {
    DEBUGASSERT(0);
    తిరిగి;
  }
  ఎంపిక(ctrl) {
    స్థితి CONNCTRL_CONN_KEEP:
      conn->బిట్లు.close = FALSE;
      ఆపు;
    స్థితి CONNCTRL_CONN_CLOSE:
      conn->బిట్లు.close = TRUE;
      ఆపు;
    స్థితి CONNCTRL_ప్రవాహం_CLOSE:
      /* stream close when multiplexing does not affect connection */
      ఒకవేళ(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->బిట్లు.close = TRUE;
      ఆపు;
    అప్రమేయ:
      DEBUGASSERT(0);
      ఆపు;
  }
}

CURLcode Curl_conn_అమరిక(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం connectdata *conn,
                         int8_t sockindex,
                         పూర్ణాంకం ssl_బహుళకం)
{
  నిర్మాణం Curl_peer *మొదటి_peer = Curl_conn_get_మొదటి_peer(conn, sockindex);
  CURLcode ఫలితం = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  ఒకవేళ(!మొదటి_peer)
    తిరిగి CURLE_FAILED_INIT;

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  ఒకవేళ(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_బహుళకం != CURL_CF_SSL_DISABLE);
    ఫలితం = Curl_cf_https_అమరిక(
      దత్తాంశం, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  ఒకవేళ(!conn->cfilter[sockindex]) {
    ఫలితం = Curl_cf_అమరిక_కూడు(దత్తాంశం, conn, sockindex,
                               conn->transport_wanted, ssl_బహుళకం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(దత్తాంశం, conn->ip_సంస్కరణ);
  ఫలితం = Curl_conn_dns_కూడు_addr_resolve(దత్తాంశం, conn, sockindex,
                                          మొదటి_peer, dns_queries,
                                          conn->transport_wanted);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  DEBUGASSERT(conn->cfilter[sockindex]);
బహిర్గతం:
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే CURLVERBOSE
స్థిర CURLcode conn_connect_trace(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం Curl_cfilter *cf)
{
  ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం)) {
    నిర్మాణం ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode ఫలితం;

    ఫలితం = Curl_conn_cf_get_ip_info(cf, దత్తాంశం, &is_ipv6, &ipquad);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    infof(దత్తాంశం, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(దత్తాంశం->conn),
          ipquad.remote_ip, ipquad.remote_పోర్టు,
          ipquad.local_ip, ipquad.local_పోర్టు);
  }
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు

/**
 * Update connection statistics
 */
స్థిర శూన్య conn_report_stats(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం sockindex)
{
  /* We do gather stats for the second socket...yet */
  ఒకవేళ(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(దత్తాంశం, దత్తాంశం->conn, sockindex);
  }
}

శూన్య Curl_conn_report_failed(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex)
{
  ఒకవేళ(!దత్తాంశం->conn || (sockindex != FIRSTSOCKET))
    తిరిగి;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (శూన్య)Curl_conn_cf_cntrl(దత్తాంశం->conn->cfilter[sockindex], దత్తాంశం, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, ఖాళీ);
  conn_report_stats(దత్తాంశం, sockindex);
}

CURLcode Curl_conn_connect(నిర్మాణం Curl_easy *దత్తాంశం,
                           int8_t sockindex,
                           bool blocking,
                           bool *పూర్తి)
{
#నిర్వచించు CF_CONN_అంకె_POLLS_ON_దొంతర 5
  నిర్మాణం pollfd a_few_on_దొంతర[CF_CONN_అంకె_POLLS_ON_దొంతర];
  నిర్మాణం easy_pollset ps;
  నిర్మాణం curl_pollfds cpfds;
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);
  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(దత్తాంశం->conn->scheme->flags & PROTOPT_NONETWORK) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  cf = దత్తాంశం->conn->cfilter[sockindex];
  ఒకవేళ(!cf) {
    *పూర్తి = FALSE;
    తిరిగి CURLE_FAILED_INIT;
  }

  *పూర్తి = (bool)cf->connected;
  ఒకవేళ(*పూర్తి)
    తిరిగి CURLE_OK;

  ఒకవేళ(!దత్తాంశం->conn->connect_started.సమితి[sockindex]) {
    దత్తాంశం->conn->connect_started.at[sockindex] = *Curl_pgrs_now(దత్తాంశం);
    దత్తాంశం->conn->connect_started.సమితి[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_దొంతర, CF_CONN_అంకె_POLLS_ON_దొంతర);
  అయితే(!*పూర్తి) {
    ఒకవేళ(Curl_conn_needs_flush(దత్తాంశం, sockindex)) {
      DEBUGF(infof(దత్తాంశం, "Curl_conn_connect(index=%d), flush", sockindex));
      ఫలితం = Curl_conn_flush(దత్తాంశం, sockindex);
      ఒకవేళ(ఫలితం && (ఫలితం != CURLE_AGAIN))
        వెళ్ళు బహిర్గతం;
    }

    ఫలితం = cf->cft->do_connect(cf, దత్తాంశం, పూర్తి);
    CURL_TRC_CF(దత్తాంశం, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (పూర్ణాంకం)ఫలితం, *పూర్తి);
    ఒకవేళ(!ఫలితం && *పూర్తి) {
      /* A final sanity check on connection security */
      ఒకవేళ((దత్తాంశం->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(దత్తాంశం->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(దత్తాంశం, "transfer requires SSL, but not connected via SSL");
        ఫలితం = CURLE_FAILED_INIT;
        వెళ్ళు బహిర్గతం;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(దత్తాంశం, దత్తాంశం->conn);
      conn_report_stats(దత్తాంశం, sockindex);
      Curl_cpool_conn_was_వాడిన(దత్తాంశం, దత్తాంశం->conn, Curl_pgrs_now(దత్తాంశం));
      VERBOSE(ఫలితం = conn_connect_trace(దత్తాంశం, cf));
      VERBOSE(Curl_conn_trc_filters(దత్తాంశం, sockindex, "connected"));
      Curl_conn_remove_అమరిక_filters(దత్తాంశం, sockindex);
      VERBOSE(Curl_conn_trc_filters(దత్తాంశం, sockindex, "reduced to"));
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "Curl_conn_connect(), filter returned %d",
                  (పూర్ణాంకం)ఫలితం);
      VERBOSE(Curl_conn_trc_filters(దత్తాంశం, sockindex, "failed to connect"));
      Curl_conn_report_failed(దత్తాంశం, sockindex);
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(!blocking)
      వెళ్ళు బహిర్గతం;
    లేకపోతే {
      /* check allowed time left */
      స్థిరరాశి నిర్మాణం curltime *pnow = Curl_pgrs_now(దత్తాంశం);
      timediff_t timeout_ms = Curl_timeleft_now_ms(దత్తాంశం, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, దత్తాంశం);
      పూర్ణాంకం rc;

      ఒకవేళ(!Curl_is_connecting(దత్తాంశం)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        స్థిరరాశి నిర్మాణం curltime *started =
          &దత్తాంశం->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_ఎడమ_ms(దత్తాంశం,
          curlx_ptimediff_ms(pnow, started));
        ఒకవేళ(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      ఒకవేళ(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(దత్తాంశం, "connect timeout");
        ఫలితం = CURLE_OPERATION_TIMEDOUT;
        వెళ్ళు బహిర్గతం;
      }

      CURL_TRC_CF(దత్తాంశం, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      ఒకవేళ(sockfd != CURL_SOCKET_BAD)
        ఫలితం = Curl_pollset_సమితి_బహిర్గతం_only(దత్తాంశం, &ps, sockfd);
      ఒకవేళ(!ఫలితం)
        ఫలితం = Curl_conn_adjust_pollset(దత్తాంశం, దత్తాంశం->conn, &ps);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      ఫలితం = Curl_pollfds_కూడు_ps(&cpfds, &ps);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(దత్తాంశం, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      ఒకవేళ(rc < 0) {
        ఫలితం = CURLE_COULDNT_CONNECT;
        వెళ్ళు బహిర్గతం;
      }
      /* continue iterating */
    }
  }

బహిర్గతం:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  తిరిగి ఫలితం;
}

శూన్య Curl_conn_సమితి_multiplex(నిర్మాణం connectdata *conn)
{
  ఒకవేళ(!conn->బిట్లు.multiplex) {
    conn->బిట్లు.multiplex = TRUE;
    ఒకవేళ(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

నిర్మాణం Curl_peer *Curl_conn_get_origin(నిర్మాణం connectdata *conn,
                                       int8_t sockindex)
{
  తిరిగి (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

నిర్మాణం Curl_peer *Curl_conn_get_destination(నిర్మాణం connectdata *conn,
                                            int8_t sockindex)
{
  తిరిగి (sockindex == SECONDARYSOCKET) ?
    (conn->ద్వారా_peer2 ? conn->ద్వారా_peer2 : conn->origin2) :
    (conn->ద్వారా_peer ? conn->ద్వారా_peer : conn->origin);
}

నిర్మాణం Curl_peer *Curl_conn_get_మొదటి_peer(నిర్మాణం connectdata *conn,
                                           int8_t sockindex)
{
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ(conn->socks_proxy.peer)
    తిరిగి conn->socks_proxy.peer;
  ఒకవేళ(conn->http_proxy.peer)
    తిరిగి conn->http_proxy.peer;
#అయితే_ముగింపు
  తిరిగి (sockindex == SECONDARYSOCKET) ?
    (conn->ద్వారా_peer2 ? conn->ద్వారా_peer2 : conn->origin2) :
    (conn->ద్వారా_peer ? conn->ద్వారా_peer : conn->origin);
}
