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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "strerror.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "cf-https-connect.h"
#അന്തര്ഭാവ "cf-setup.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "conncache.h"
#അന്തര്ഭാവ "multihandle.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "curlx/strparse.h"

#എങ്കിൽ !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

എണ്ണൽ alpnid Curl_alpn2alpnid(സ്ഥിരം unsigned അക്ഷരം *name, size_t len)
{
  എങ്കിൽ(len == 2) {
    എങ്കിൽ(!memcmp(name, "h1", 2))
      തിരികെ ALPN_h1;
    എങ്കിൽ(!memcmp(name, "h2", 2))
      തിരികെ ALPN_h2;
    എങ്കിൽ(!memcmp(name, "h3", 2))
      തിരികെ ALPN_h3;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(len == 8 && !memcmp(name, "http/1.1", 8))
    തിരികെ ALPN_h1;
  തിരികെ ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
സ്ഥിതം timediff_t connect_ഇടത്_ms(ഘടന Curl_easy *data,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t ഇടത്_ms = timeout_ms - used_ms;
  തിരികെ ഇടത്_ms ? ഇടത്_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(ഘടന Curl_easy *data,
                                    സ്ഥിരം ഘടന curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(ഘടന Curl_easy *data,
                                    സ്ഥിരം ഘടന curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  എങ്കിൽ(data->conn && CURL_CONN_IN_SHUTDOWN(data->conn, FIRSTSOCKET))
    തിരികെ Curl_cshutdn_timeleft_ms(data, data->conn, FIRSTSOCKET);
  അല്ലെങ്കിൽ എങ്കിൽ(Curl_is_connecting(data)) {
    ctimeleft_ms = connect_ഇടത്_ms(data,
      Curl_pgrs_since_ms(data, pnow, TIMER_STARTSINGLE));
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!data->set.timeout || data->set.connect_only) {
    തിരികെ 0; /* no timeout in place or checked, return "no limit" */
  }

  എങ്കിൽ(data->set.timeout) {
    timeleft_ms = data->set.timeout -
                  Curl_pgrs_since_ms(data, pnow, TIMER_STARTOP);
    എങ്കിൽ(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  എങ്കിൽ(!ctimeleft_ms)
    തിരികെ timeleft_ms;
  അല്ലെങ്കിൽ എങ്കിൽ(!timeleft_ms)
    തിരികെ ctimeleft_ms;
  തിരികെ CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(ഘടന Curl_easy *data)
{
  തിരികെ timeleft_now_ms(data, Curl_pgrs_now(data));
}

timediff_t Curl_timeleft_now_ms(ഘടന Curl_easy *data,
                                സ്ഥിരം ഘടന curltime *pnow)
{
  തിരികെ timeleft_now_ms(data, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(ഘടന Curl_easy *data,
                                  ഘടന connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  ഘടന connectdata *conn = Curl_cpool_get_last_conn(data);
  എങ്കിൽ(connp)
    *connp = conn;
  തിരികെ conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

ശൂന്യം Curl_conncontrol(ഘടന connectdata *conn, പൂർണ്ണസംഖ്യ ctrl)
{
  എങ്കിൽ(!conn) {
    DEBUGASSERT(0);
    തിരികെ;
  }
  തിരഞ്ഞെടുപ്പ്(ctrl) {
    അവസ്ഥ CONNCTRL_CONN_KEEP:
      conn->bits.close = FALSE;
      നിർത്തുക;
    അവസ്ഥ CONNCTRL_CONN_CLOSE:
      conn->bits.close = TRUE;
      നിർത്തുക;
    അവസ്ഥ CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      എങ്കിൽ(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.close = TRUE;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      DEBUGASSERT(0);
      നിർത്തുക;
  }
}

CURLcode Curl_conn_setup(ഘടന Curl_easy *data,
                         ഘടന connectdata *conn,
                         int8_t sockindex,
                         പൂർണ്ണസംഖ്യ ssl_mode)
{
  ഘടന Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode result = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(data);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  എങ്കിൽ(!first_peer)
    തിരികെ CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  എങ്കിൽ(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    result = Curl_cf_https_setup(
      data, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    എങ്കിൽ(result)
      പോകുക out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  എങ്കിൽ(!conn->cfilter[sockindex]) {
    result = Curl_cf_setup_add(data, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    എങ്കിൽ(result)
      പോകുക out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(data, conn->ip_version);
  result = Curl_conn_dns_add_addr_resolve(data, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  എങ്കിൽ(result)
    പോകുക out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  തിരികെ result;
}

#ifdef CURLVERBOSE
സ്ഥിതം CURLcode conn_connect_trace(ഘടന Curl_easy *data,
                                   ഘടന Curl_cfilter *cf)
{
  എങ്കിൽ(Curl_trc_is_verbose(data)) {
    ഘടന ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode result;

    result = Curl_conn_cf_get_ip_info(cf, data, &is_ipv6, &ipquad);
    എങ്കിൽ(result)
      തിരികെ result;

    infof(data, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(data->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  തിരികെ CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
സ്ഥിതം ശൂന്യം conn_report_stats(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ sockindex)
{
  /* We do gather stats for the second socket...yet */
  എങ്കിൽ(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(data, data->conn, sockindex);
  }
}

ശൂന്യം Curl_conn_report_failed(ഘടന Curl_easy *data, int8_t sockindex)
{
  എങ്കിൽ(!data->conn || (sockindex != FIRSTSOCKET))
    തിരികെ;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (ശൂന്യം)Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, ശൂന്യ);
  conn_report_stats(data, sockindex);
}

CURLcode Curl_conn_connect(ഘടന Curl_easy *data,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#define CF_CONN_അക്കം_POLLS_ON_അട്ടി 5
  ഘടന pollfd a_few_on_അട്ടി[CF_CONN_അക്കം_POLLS_ON_അട്ടി];
  ഘടന easy_pollset ps;
  ഘടന curl_pollfds cpfds;
  ഘടന Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(data->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  cf = data->conn->cfilter[sockindex];
  എങ്കിൽ(!cf) {
    *done = FALSE;
    തിരികെ CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  എങ്കിൽ(*done)
    തിരികെ CURLE_OK;

  എങ്കിൽ(!data->conn->connect_started.set[sockindex]) {
    data->conn->connect_started.at[sockindex] = *Curl_pgrs_now(data);
    data->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_അട്ടി, CF_CONN_അക്കം_POLLS_ON_അട്ടി);
  ആകുമ്പോൾ(!*done) {
    എങ്കിൽ(Curl_conn_needs_flush(data, sockindex)) {
      DEBUGF(infof(data, "Curl_conn_connect(index=%d), flush", sockindex));
      result = Curl_conn_flush(data, sockindex);
      എങ്കിൽ(result && (result != CURLE_AGAIN))
        പോകുക out;
    }

    result = cf->cft->do_connect(cf, data, done);
    CURL_TRC_CF(data, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (പൂർണ്ണസംഖ്യ)result, *done);
    എങ്കിൽ(!result && *done) {
      /* A final sanity check on connection security */
      എങ്കിൽ((data->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(data, "transfer requires SSL, but not connected via SSL");
        result = CURLE_FAILED_INIT;
        പോകുക out;
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
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(result) {
      CURL_TRC_CF(data, cf, "Curl_conn_connect(), filter returned %d",
                  (പൂർണ്ണസംഖ്യ)result);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "failed to connect"));
      Curl_conn_report_failed(data, sockindex);
      പോകുക out;
    }

    എങ്കിൽ(!blocking)
      പോകുക out;
    അല്ലെങ്കിൽ {
      /* check allowed time left */
      സ്ഥിരം ഘടന curltime *pnow = Curl_pgrs_now(data);
      timediff_t timeout_ms = Curl_timeleft_now_ms(data, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, data);
      പൂർണ്ണസംഖ്യ rc;

      എങ്കിൽ(!Curl_is_connecting(data)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        സ്ഥിരം ഘടന curltime *started =
          &data->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_ഇടത്_ms(data,
          curlx_ptimediff_ms(pnow, started));
        എങ്കിൽ(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      എങ്കിൽ(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(data, "connect timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        പോകുക out;
      }

      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      എങ്കിൽ(sockfd != CURL_SOCKET_BAD)
        result = Curl_pollset_set_out_only(data, &ps, sockfd);
      എങ്കിൽ(!result)
        result = Curl_conn_adjust_pollset(data, data->conn, &ps);
      എങ്കിൽ(result)
        പോകുക out;
      result = Curl_pollfds_add_ps(&cpfds, &ps);
      എങ്കിൽ(result)
        പോകുക out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      എങ്കിൽ(rc < 0) {
        result = CURLE_COULDNT_CONNECT;
        പോകുക out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  തിരികെ result;
}

ശൂന്യം Curl_conn_set_multiplex(ഘടന connectdata *conn)
{
  എങ്കിൽ(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    എങ്കിൽ(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

ഘടന Curl_peer *Curl_conn_get_origin(ഘടന connectdata *conn,
                                       int8_t sockindex)
{
  തിരികെ (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

ഘടന Curl_peer *Curl_conn_get_destination(ഘടന connectdata *conn,
                                            int8_t sockindex)
{
  തിരികെ (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

ഘടന Curl_peer *Curl_conn_get_first_peer(ഘടന connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  എങ്കിൽ(conn->socks_proxy.peer)
    തിരികെ conn->socks_proxy.peer;
  എങ്കിൽ(conn->http_proxy.peer)
    തിരികെ conn->http_proxy.peer;
#endif
  തിരികെ (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
