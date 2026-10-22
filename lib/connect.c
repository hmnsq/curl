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
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "strerror.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "cf-https-connect.h"
#ଅନ୍ତର୍ଭାଵ "cf-setup.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "conncache.h"
#ଅନ୍ତର୍ଭାଵ "multihandle.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

#ଯଦି !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

ଗଣନା alpnid Curl_alpn2alpnid(ଅଚଳ unsigned ଅକ୍ଷର *name, size_t len)
{
  ଯଦି(len == 2) {
    ଯଦି(!memcmp(name, "h1", 2))
      ଫେରନ୍ତୁ ALPN_h1;
    ଯଦି(!memcmp(name, "h2", 2))
      ଫେରନ୍ତୁ ALPN_h2;
    ଯଦି(!memcmp(name, "h3", 2))
      ଫେରନ୍ତୁ ALPN_h3;
  }
  ଅନ୍ୟଥା ଯଦି(len == 8 && !memcmp(name, "http/1.1", 8))
    ଫେରନ୍ତୁ ALPN_h1;
  ଫେରନ୍ତୁ ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
ସ୍ଥିର timediff_t connect_ବାମ_ms(ସଂରଚନା Curl_easy *data,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t ବାମ_ms = timeout_ms - used_ms;
  ଫେରନ୍ତୁ ବାମ_ms ? ବାମ_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(ସଂରଚନା Curl_easy *data,
                                    ଅଚଳ ସଂରଚନା curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(ସଂରଚନା Curl_easy *data,
                                    ଅଚଳ ସଂରଚନା curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  ଯଦି(data->conn && CURL_CONN_IN_SHUTDOWN(data->conn, FIRSTSOCKET))
    ଫେରନ୍ତୁ Curl_cshutdn_timeleft_ms(data, data->conn, FIRSTSOCKET);
  ଅନ୍ୟଥା ଯଦି(Curl_is_connecting(data)) {
    ctimeleft_ms = connect_ବାମ_ms(data,
      Curl_pgrs_since_ms(data, pnow, TIMER_STARTSINGLE));
  }
  ଅନ୍ୟଥା ଯଦି(!data->set.timeout || data->set.connect_only) {
    ଫେରନ୍ତୁ 0; /* no timeout in place or checked, return "no limit" */
  }

  ଯଦି(data->set.timeout) {
    timeleft_ms = data->set.timeout -
                  Curl_pgrs_since_ms(data, pnow, TIMER_STARTOP);
    ଯଦି(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  ଯଦି(!ctimeleft_ms)
    ଫେରନ୍ତୁ timeleft_ms;
  ଅନ୍ୟଥା ଯଦି(!timeleft_ms)
    ଫେରନ୍ତୁ ctimeleft_ms;
  ଫେରନ୍ତୁ CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ timeleft_now_ms(data, Curl_pgrs_now(data));
}

timediff_t Curl_timeleft_now_ms(ସଂରଚନା Curl_easy *data,
                                ଅଚଳ ସଂରଚନା curltime *pnow)
{
  ଫେରନ୍ତୁ timeleft_now_ms(data, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  ସଂରଚନା connectdata *conn = Curl_cpool_get_last_conn(data);
  ଯଦି(connp)
    *connp = conn;
  ଫେରନ୍ତୁ conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

ଶୂନ୍ୟ Curl_conncontrol(ସଂରଚନା connectdata *conn, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctrl)
{
  ଯଦି(!conn) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ;
  }
  ବାଛ(ctrl) {
    ସ୍ଥିତି CONNCTRL_CONN_KEEP:
      conn->bits.close = FALSE;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CONNCTRL_CONN_CLOSE:
      conn->bits.close = TRUE;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      ଯଦି(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.close = TRUE;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      DEBUGASSERT(0);
      ଭାଙ୍ଗ;
  }
}

CURLcode Curl_conn_setup(ସଂରଚନା Curl_easy *data,
                         ସଂରଚନା connectdata *conn,
                         int8_t sockindex,
                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_mode)
{
  ସଂରଚନା Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode result = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(data);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  ଯଦି(!first_peer)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  ଯଦି(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    result = Curl_cf_https_setup(
      data, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    ଯଦି(result)
      ଯାଅ out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  ଯଦି(!conn->cfilter[sockindex]) {
    result = Curl_cf_setup_add(data, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    ଯଦି(result)
      ଯାଅ out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(data, conn->ip_version);
  result = Curl_conn_dns_add_addr_resolve(data, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  ଯଦି(result)
    ଯାଅ out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  ଫେରନ୍ତୁ result;
}

#ifdef CURLVERBOSE
ସ୍ଥିର CURLcode conn_connect_trace(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା Curl_cfilter *cf)
{
  ଯଦି(Curl_trc_is_verbose(data)) {
    ସଂରଚନା ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode result;

    result = Curl_conn_cf_get_ip_info(cf, data, &is_ipv6, &ipquad);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    infof(data, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(data->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
ସ୍ଥିର ଶୂନ୍ୟ conn_report_stats(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockindex)
{
  /* We do gather stats for the second socket...yet */
  ଯଦି(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(data, data->conn, sockindex);
  }
}

ଶୂନ୍ୟ Curl_conn_report_failed(ସଂରଚନା Curl_easy *data, int8_t sockindex)
{
  ଯଦି(!data->conn || (sockindex != FIRSTSOCKET))
    ଫେରନ୍ତୁ;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (ଶୂନ୍ୟ)Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, ଖାଲି);
  conn_report_stats(data, sockindex);
}

CURLcode Curl_conn_connect(ସଂରଚନା Curl_easy *data,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#define CF_CONN_ଅଙ୍କ_POLLS_ON_ଗଦା 5
  ସଂରଚନା pollfd a_few_on_ଗଦା[CF_CONN_ଅଙ୍କ_POLLS_ON_ଗଦା];
  ସଂରଚନା easy_pollset ps;
  ସଂରଚନା curl_pollfds cpfds;
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(data->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  cf = data->conn->cfilter[sockindex];
  ଯଦି(!cf) {
    *done = FALSE;
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  ଯଦି(*done)
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(!data->conn->connect_started.set[sockindex]) {
    data->conn->connect_started.at[sockindex] = *Curl_pgrs_now(data);
    data->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ଗଦା, CF_CONN_ଅଙ୍କ_POLLS_ON_ଗଦା);
  ଯେତେବେଳେ(!*done) {
    ଯଦି(Curl_conn_needs_flush(data, sockindex)) {
      DEBUGF(infof(data, "Curl_conn_connect(index=%d), flush", sockindex));
      result = Curl_conn_flush(data, sockindex);
      ଯଦି(result && (result != CURLE_AGAIN))
        ଯାଅ out;
    }

    result = cf->cft->do_connect(cf, data, done);
    CURL_TRC_CF(data, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *done);
    ଯଦି(!result && *done) {
      /* A final sanity check on connection security */
      ଯଦି((data->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(data, "transfer requires SSL, but not connected via SSL");
        result = CURLE_FAILED_INIT;
        ଯାଅ out;
      }
      /* Now that the complete filter chain is connected, let all filters
       * persist information at the connection. E.g. cf-socket sets the
       * socket and ip related information. */
      Curl_conn_cntrl_update_info(data, data->conn);
      conn_report_stats(data, sockindex);
      Curl_cpool_conn_was_used(data, data->conn, Curl_pgrs_now(data));
      VERBOSE(result = conn_connect_trace(data, cf));
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "connected"));
      Curl_conn_remove_setup_filters(data, sockindex);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "reduced to"));
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(result) {
      CURL_TRC_CF(data, cf, "Curl_conn_connect(), filter returned %d",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "failed to connect"));
      Curl_conn_report_failed(data, sockindex);
      ଯାଅ out;
    }

    ଯଦି(!blocking)
      ଯାଅ out;
    ଅନ୍ୟଥା {
      /* check allowed time left */
      ଅଚଳ ସଂରଚନା curltime *pnow = Curl_pgrs_now(data);
      timediff_t timeout_ms = Curl_timeleft_now_ms(data, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, data);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

      ଯଦି(!Curl_is_connecting(data)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ଅଚଳ ସଂରଚନା curltime *started =
          &data->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_ବାମ_ms(data,
          curlx_ptimediff_ms(pnow, started));
        ଯଦି(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      ଯଦି(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(data, "connect timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        ଯାଅ out;
      }

      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      ଯଦି(sockfd != CURL_SOCKET_BAD)
        result = Curl_pollset_set_out_only(data, &ps, sockfd);
      ଯଦି(!result)
        result = Curl_conn_adjust_pollset(data, data->conn, &ps);
      ଯଦି(result)
        ଯାଅ out;
      result = Curl_pollfds_add_ps(&cpfds, &ps);
      ଯଦି(result)
        ଯାଅ out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      ଯଦି(rc < 0) {
        result = CURLE_COULDNT_CONNECT;
        ଯାଅ out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_conn_set_multiplex(ସଂରଚନା connectdata *conn)
{
  ଯଦି(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    ଯଦି(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

ସଂରଚନା Curl_peer *Curl_conn_get_origin(ସଂରଚନା connectdata *conn,
                                       int8_t sockindex)
{
  ଫେରନ୍ତୁ (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

ସଂରଚନା Curl_peer *Curl_conn_get_destination(ସଂରଚନା connectdata *conn,
                                            int8_t sockindex)
{
  ଫେରନ୍ତୁ (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

ସଂରଚନା Curl_peer *Curl_conn_get_first_peer(ସଂରଚନା connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  ଯଦି(conn->socks_proxy.peer)
    ଫେରନ୍ତୁ conn->socks_proxy.peer;
  ଯଦି(conn->http_proxy.peer)
    ଫେରନ୍ତୁ conn->http_proxy.peer;
#endif
  ଫେରନ୍ତୁ (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
