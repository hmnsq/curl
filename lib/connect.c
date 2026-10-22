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
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "strerror.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "cf-https-connect.h"
#அந்தர்பாவ "cf-setup.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "conncache.h"
#அந்தர்பாவ "multihandle.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "curlx/strparse.h"

#எனில் !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

எண்ணிக்கை alpnid Curl_alpn2alpnid(மாறிலி unsigned எழுத்து *name, size_t len)
{
  எனில்(len == 2) {
    எனில்(!memcmp(name, "h1", 2))
      திரும்பு ALPN_h1;
    எனில்(!memcmp(name, "h2", 2))
      திரும்பு ALPN_h2;
    எனில்(!memcmp(name, "h3", 2))
      திரும்பு ALPN_h3;
  }
  இல்லையெனில் எனில்(len == 8 && !memcmp(name, "http/1.1", 8))
    திரும்பு ALPN_h1;
  திரும்பு ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
நிலையான timediff_t connect_இடது_ms(கட்டமைப்பு Curl_easy *data,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t இடது_ms = timeout_ms - used_ms;
  திரும்பு இடது_ms ? இடது_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(கட்டமைப்பு Curl_easy *data,
                                    மாறிலி கட்டமைப்பு curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(கட்டமைப்பு Curl_easy *data,
                                    மாறிலி கட்டமைப்பு curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  எனில்(data->conn && CURL_CONN_IN_SHUTDOWN(data->conn, FIRSTSOCKET))
    திரும்பு Curl_cshutdn_timeleft_ms(data, data->conn, FIRSTSOCKET);
  இல்லையெனில் எனில்(Curl_is_connecting(data)) {
    ctimeleft_ms = connect_இடது_ms(data,
      Curl_pgrs_since_ms(data, pnow, TIMER_STARTSINGLE));
  }
  இல்லையெனில் எனில்(!data->set.timeout || data->set.connect_only) {
    திரும்பு 0; /* no timeout in place or checked, return "no limit" */
  }

  எனில்(data->set.timeout) {
    timeleft_ms = data->set.timeout -
                  Curl_pgrs_since_ms(data, pnow, TIMER_STARTOP);
    எனில்(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  எனில்(!ctimeleft_ms)
    திரும்பு timeleft_ms;
  இல்லையெனில் எனில்(!timeleft_ms)
    திரும்பு ctimeleft_ms;
  திரும்பு CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(கட்டமைப்பு Curl_easy *data)
{
  திரும்பு timeleft_now_ms(data, Curl_pgrs_now(data));
}

timediff_t Curl_timeleft_now_ms(கட்டமைப்பு Curl_easy *data,
                                மாறிலி கட்டமைப்பு curltime *pnow)
{
  திரும்பு timeleft_now_ms(data, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  கட்டமைப்பு connectdata *conn = Curl_cpool_get_last_conn(data);
  எனில்(connp)
    *connp = conn;
  திரும்பு conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

வெறுமை Curl_conncontrol(கட்டமைப்பு connectdata *conn, முழு ctrl)
{
  எனில்(!conn) {
    DEBUGASSERT(0);
    திரும்பு;
  }
  தேர்வு(ctrl) {
    நிலை CONNCTRL_CONN_KEEP:
      conn->bits.close = FALSE;
      நிறுத்து;
    நிலை CONNCTRL_CONN_CLOSE:
      conn->bits.close = TRUE;
      நிறுத்து;
    நிலை CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      எனில்(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.close = TRUE;
      நிறுத்து;
    இயல்பு:
      DEBUGASSERT(0);
      நிறுத்து;
  }
}

CURLcode Curl_conn_setup(கட்டமைப்பு Curl_easy *data,
                         கட்டமைப்பு connectdata *conn,
                         int8_t sockindex,
                         முழு ssl_mode)
{
  கட்டமைப்பு Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode result = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(data);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  எனில்(!first_peer)
    திரும்பு CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  எனில்(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    result = Curl_cf_https_setup(
      data, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    எனில்(result)
      செல் out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  எனில்(!conn->cfilter[sockindex]) {
    result = Curl_cf_setup_add(data, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    எனில்(result)
      செல் out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(data, conn->ip_version);
  result = Curl_conn_dns_add_addr_resolve(data, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  எனில்(result)
    செல் out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  திரும்பு result;
}

#ifdef CURLVERBOSE
நிலையான CURLcode conn_connect_trace(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு Curl_cfilter *cf)
{
  எனில்(Curl_trc_is_verbose(data)) {
    கட்டமைப்பு ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode result;

    result = Curl_conn_cf_get_ip_info(cf, data, &is_ipv6, &ipquad);
    எனில்(result)
      திரும்பு result;

    infof(data, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(data->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  திரும்பு CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
நிலையான வெறுமை conn_report_stats(கட்டமைப்பு Curl_easy *data, முழு sockindex)
{
  /* We do gather stats for the second socket...yet */
  எனில்(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(data, data->conn, sockindex);
  }
}

வெறுமை Curl_conn_report_failed(கட்டமைப்பு Curl_easy *data, int8_t sockindex)
{
  எனில்(!data->conn || (sockindex != FIRSTSOCKET))
    திரும்பு;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (வெறுமை)Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, வெற்று);
  conn_report_stats(data, sockindex);
}

CURLcode Curl_conn_connect(கட்டமைப்பு Curl_easy *data,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#define CF_CONN_இலக்கம்_POLLS_ON_அடுக்கு 5
  கட்டமைப்பு pollfd a_few_on_அடுக்கு[CF_CONN_இலக்கம்_POLLS_ON_அடுக்கு];
  கட்டமைப்பு easy_pollset ps;
  கட்டமைப்பு curl_pollfds cpfds;
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(data->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  cf = data->conn->cfilter[sockindex];
  எனில்(!cf) {
    *done = FALSE;
    திரும்பு CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  எனில்(*done)
    திரும்பு CURLE_OK;

  எனில்(!data->conn->connect_started.set[sockindex]) {
    data->conn->connect_started.at[sockindex] = *Curl_pgrs_now(data);
    data->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_அடுக்கு, CF_CONN_இலக்கம்_POLLS_ON_அடுக்கு);
  போது(!*done) {
    எனில்(Curl_conn_needs_flush(data, sockindex)) {
      DEBUGF(infof(data, "Curl_conn_connect(index=%d), flush", sockindex));
      result = Curl_conn_flush(data, sockindex);
      எனில்(result && (result != CURLE_AGAIN))
        செல் out;
    }

    result = cf->cft->do_connect(cf, data, done);
    CURL_TRC_CF(data, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (முழு)result, *done);
    எனில்(!result && *done) {
      /* A final sanity check on connection security */
      எனில்((data->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(data, "transfer requires SSL, but not connected via SSL");
        result = CURLE_FAILED_INIT;
        செல் out;
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
      செல் out;
    }
    இல்லையெனில் எனில்(result) {
      CURL_TRC_CF(data, cf, "Curl_conn_connect(), filter returned %d",
                  (முழு)result);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "failed to connect"));
      Curl_conn_report_failed(data, sockindex);
      செல் out;
    }

    எனில்(!blocking)
      செல் out;
    இல்லையெனில் {
      /* check allowed time left */
      மாறிலி கட்டமைப்பு curltime *pnow = Curl_pgrs_now(data);
      timediff_t timeout_ms = Curl_timeleft_now_ms(data, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, data);
      முழு rc;

      எனில்(!Curl_is_connecting(data)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        மாறிலி கட்டமைப்பு curltime *started =
          &data->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_இடது_ms(data,
          curlx_ptimediff_ms(pnow, started));
        எனில்(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      எனில்(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(data, "connect timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        செல் out;
      }

      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      எனில்(sockfd != CURL_SOCKET_BAD)
        result = Curl_pollset_set_out_only(data, &ps, sockfd);
      எனில்(!result)
        result = Curl_conn_adjust_pollset(data, data->conn, &ps);
      எனில்(result)
        செல் out;
      result = Curl_pollfds_add_ps(&cpfds, &ps);
      எனில்(result)
        செல் out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      எனில்(rc < 0) {
        result = CURLE_COULDNT_CONNECT;
        செல் out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  திரும்பு result;
}

வெறுமை Curl_conn_set_multiplex(கட்டமைப்பு connectdata *conn)
{
  எனில்(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    எனில்(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

கட்டமைப்பு Curl_peer *Curl_conn_get_origin(கட்டமைப்பு connectdata *conn,
                                       int8_t sockindex)
{
  திரும்பு (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

கட்டமைப்பு Curl_peer *Curl_conn_get_destination(கட்டமைப்பு connectdata *conn,
                                            int8_t sockindex)
{
  திரும்பு (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

கட்டமைப்பு Curl_peer *Curl_conn_get_first_peer(கட்டமைப்பு connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  எனில்(conn->socks_proxy.peer)
    திரும்பு conn->socks_proxy.peer;
  எனில்(conn->http_proxy.peer)
    திரும்பு conn->http_proxy.peer;
#endif
  திரும்பு (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
