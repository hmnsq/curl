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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strerror.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "cf-https-connect.h"
#অন্তর্ভাব "cf-setup.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "conncache.h"
#অন্তর্ভাব "multihandle.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "curlx/strparse.h"

#যদি !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

গণনা alpnid Curl_alpn2alpnid(ধ্রুবক unsigned অক্ষর *name, size_t len)
{
  যদি(len == 2) {
    যদি(!memcmp(name, "h1", 2))
      ফেরত ALPN_h1;
    যদি(!memcmp(name, "h2", 2))
      ফেরত ALPN_h2;
    যদি(!memcmp(name, "h3", 2))
      ফেরত ALPN_h3;
  }
  অন্যথা যদি(len == 8 && !memcmp(name, "http/1.1", 8))
    ফেরত ALPN_h1;
  ফেরত ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
স্থির timediff_t connect_বাম_ms(কাঠামো Curl_easy *তথ্য,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (তথ্য->set.connecttimeout > 0) ?
    তথ্য->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t বাম_ms = timeout_ms - used_ms;
  ফেরত বাম_ms ? বাম_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(কাঠামো Curl_easy *তথ্য,
                                    ধ্রুবক কাঠামো curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(কাঠামো Curl_easy *তথ্য,
                                    ধ্রুবক কাঠামো curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  যদি(তথ্য->conn && CURL_CONN_IN_SHUTDOWN(তথ্য->conn, FIRSTSOCKET))
    ফেরত Curl_cshutdn_timeleft_ms(তথ্য, তথ্য->conn, FIRSTSOCKET);
  অন্যথা যদি(Curl_is_connecting(তথ্য)) {
    ctimeleft_ms = connect_বাম_ms(তথ্য,
      Curl_pgrs_since_ms(তথ্য, pnow, TIMER_STARTSINGLE));
  }
  অন্যথা যদি(!তথ্য->set.timeout || তথ্য->set.connect_only) {
    ফেরত 0; /* no timeout in place or checked, return "no limit" */
  }

  যদি(তথ্য->set.timeout) {
    timeleft_ms = তথ্য->set.timeout -
                  Curl_pgrs_since_ms(তথ্য, pnow, TIMER_STARTOP);
    যদি(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  যদি(!ctimeleft_ms)
    ফেরত timeleft_ms;
  অন্যথা যদি(!timeleft_ms)
    ফেরত ctimeleft_ms;
  ফেরত CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(কাঠামো Curl_easy *তথ্য)
{
  ফেরত timeleft_now_ms(তথ্য, Curl_pgrs_now(তথ্য));
}

timediff_t Curl_timeleft_now_ms(কাঠামো Curl_easy *তথ্য,
                                ধ্রুবক কাঠামো curltime *pnow)
{
  ফেরত timeleft_now_ms(তথ্য, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  কাঠামো connectdata *conn = Curl_cpool_get_last_conn(তথ্য);
  যদি(connp)
    *connp = conn;
  ফেরত conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

শূন্য Curl_conncontrol(কাঠামো connectdata *conn, পূর্ণসংখ্যা ctrl)
{
  যদি(!conn) {
    DEBUGASSERT(0);
    ফেরত;
  }
  নির্বাচন(ctrl) {
    ক্ষেত্র CONNCTRL_CONN_KEEP:
      conn->bits.close = FALSE;
      বিরতি;
    ক্ষেত্র CONNCTRL_CONN_CLOSE:
      conn->bits.close = TRUE;
      বিরতি;
    ক্ষেত্র CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      যদি(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.close = TRUE;
      বিরতি;
    পূর্বনির্ধারিত:
      DEBUGASSERT(0);
      বিরতি;
  }
}

CURLcode Curl_conn_setup(কাঠামো Curl_easy *তথ্য,
                         কাঠামো connectdata *conn,
                         int8_t sockindex,
                         পূর্ণসংখ্যা ssl_mode)
{
  কাঠামো Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode ফলাফল = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  যদি(!first_peer)
    ফেরত CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  যদি(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    ফলাফল = Curl_cf_https_setup(
      তথ্য, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    যদি(ফলাফল)
      যাও out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  যদি(!conn->cfilter[sockindex]) {
    ফলাফল = Curl_cf_setup_add(তথ্য, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    যদি(ফলাফল)
      যাও out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(তথ্য, conn->ip_version);
  ফলাফল = Curl_conn_dns_add_addr_resolve(তথ্য, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  যদি(ফলাফল)
    যাও out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  ফেরত ফলাফল;
}

#ifdef CURLVERBOSE
স্থির CURLcode conn_connect_trace(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো Curl_cfilter *cf)
{
  যদি(Curl_trc_is_verbose(তথ্য)) {
    কাঠামো ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode ফলাফল;

    ফলাফল = Curl_conn_cf_get_ip_info(cf, তথ্য, &is_ipv6, &ipquad);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    infof(তথ্য, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(তথ্য->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  ফেরত CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
স্থির শূন্য conn_report_stats(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা sockindex)
{
  /* We do gather stats for the second socket...yet */
  যদি(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(তথ্য, তথ্য->conn, sockindex);
  }
}

শূন্য Curl_conn_report_failed(কাঠামো Curl_easy *তথ্য, int8_t sockindex)
{
  যদি(!তথ্য->conn || (sockindex != FIRSTSOCKET))
    ফেরত;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (শূন্য)Curl_conn_cf_cntrl(তথ্য->conn->cfilter[sockindex], তথ্য, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, ফাঁকা);
  conn_report_stats(তথ্য, sockindex);
}

CURLcode Curl_conn_connect(কাঠামো Curl_easy *তথ্য,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#define CF_CONN_অঙ্ক_POLLS_ON_স্তূপ 5
  কাঠামো pollfd a_few_on_স্তূপ[CF_CONN_অঙ্ক_POLLS_ON_স্তূপ];
  কাঠামো easy_pollset ps;
  কাঠামো curl_pollfds cpfds;
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(তথ্য->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  cf = তথ্য->conn->cfilter[sockindex];
  যদি(!cf) {
    *done = FALSE;
    ফেরত CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  যদি(*done)
    ফেরত CURLE_OK;

  যদি(!তথ্য->conn->connect_started.set[sockindex]) {
    তথ্য->conn->connect_started.at[sockindex] = *Curl_pgrs_now(তথ্য);
    তথ্য->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_স্তূপ, CF_CONN_অঙ্ক_POLLS_ON_স্তূপ);
  যখন(!*done) {
    যদি(Curl_conn_needs_flush(তথ্য, sockindex)) {
      DEBUGF(infof(তথ্য, "Curl_conn_connect(index=%d), flush", sockindex));
      ফলাফল = Curl_conn_flush(তথ্য, sockindex);
      যদি(ফলাফল && (ফলাফল != CURLE_AGAIN))
        যাও out;
    }

    ফলাফল = cf->cft->do_connect(cf, তথ্য, done);
    CURL_TRC_CF(তথ্য, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (পূর্ণসংখ্যা)ফলাফল, *done);
    যদি(!ফলাফল && *done) {
      /* A final sanity check on connection security */
      যদি((তথ্য->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(তথ্য->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(তথ্য, "transfer requires SSL, but not connected via SSL");
        ফলাফল = CURLE_FAILED_INIT;
        যাও out;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(তথ্য, তথ্য->conn);
      conn_report_stats(তথ্য, sockindex);
      Curl_cpool_conn_was_used(তথ্য, তথ্য->conn, Curl_pgrs_now(তথ্য));
      VERBOSE(ফলাফল = conn_connect_trace(তথ্য, cf));
      VERBOSE(Curl_conn_trc_filters(তথ্য, sockindex, "connected"));
      Curl_conn_remove_setup_filters(তথ্য, sockindex);
      VERBOSE(Curl_conn_trc_filters(তথ্য, sockindex, "reduced to"));
      যাও out;
    }
    অন্যথা যদি(ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "Curl_conn_connect(), filter returned %d",
                  (পূর্ণসংখ্যা)ফলাফল);
      VERBOSE(Curl_conn_trc_filters(তথ্য, sockindex, "failed to connect"));
      Curl_conn_report_failed(তথ্য, sockindex);
      যাও out;
    }

    যদি(!blocking)
      যাও out;
    অন্যথা {
      /* check allowed time left */
      ধ্রুবক কাঠামো curltime *pnow = Curl_pgrs_now(তথ্য);
      timediff_t timeout_ms = Curl_timeleft_now_ms(তথ্য, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, তথ্য);
      পূর্ণসংখ্যা rc;

      যদি(!Curl_is_connecting(তথ্য)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ধ্রুবক কাঠামো curltime *started =
          &তথ্য->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_বাম_ms(তথ্য,
          curlx_ptimediff_ms(pnow, started));
        যদি(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      যদি(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(তথ্য, "connect timeout");
        ফলাফল = CURLE_OPERATION_TIMEDOUT;
        যাও out;
      }

      CURL_TRC_CF(তথ্য, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      যদি(sockfd != CURL_SOCKET_BAD)
        ফলাফল = Curl_pollset_set_out_only(তথ্য, &ps, sockfd);
      যদি(!ফলাফল)
        ফলাফল = Curl_conn_adjust_pollset(তথ্য, তথ্য->conn, &ps);
      যদি(ফলাফল)
        যাও out;
      ফলাফল = Curl_pollfds_add_ps(&cpfds, &ps);
      যদি(ফলাফল)
        যাও out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(তথ্য, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      যদি(rc < 0) {
        ফলাফল = CURLE_COULDNT_CONNECT;
        যাও out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  ফেরত ফলাফল;
}

শূন্য Curl_conn_set_multiplex(কাঠামো connectdata *conn)
{
  যদি(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    যদি(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

কাঠামো Curl_peer *Curl_conn_get_origin(কাঠামো connectdata *conn,
                                       int8_t sockindex)
{
  ফেরত (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

কাঠামো Curl_peer *Curl_conn_get_destination(কাঠামো connectdata *conn,
                                            int8_t sockindex)
{
  ফেরত (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

কাঠামো Curl_peer *Curl_conn_get_first_peer(কাঠামো connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  যদি(conn->socks_proxy.peer)
    ফেরত conn->socks_proxy.peer;
  যদি(conn->http_proxy.peer)
    ফেরত conn->http_proxy.peer;
#endif
  ফেরত (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
