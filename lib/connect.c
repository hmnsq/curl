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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "strerror.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "cf-https-connect.h"
#ضمن "cf-setup.h"
#ضمن "multiif.h"
#ضمن "progress.h"
#ضمن "conncache.h"
#ضمن "multihandle.h"
#ضمن "select.h"
#ضمن "vdns/cf-dns.h"
#ضمن "curlx/strparse.h"

#𐡄𐡍 !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

تعداد alpnid Curl_alpn2alpnid(ثابت unsigned حرف *name, size_t len)
{
  𐡄𐡍(len == 2) {
    𐡄𐡍(!memcmp(name, "h1", 2))
      أرجع ALPN_h1;
    𐡄𐡍(!memcmp(name, "h2", 2))
      أرجع ALPN_h2;
    𐡄𐡍(!memcmp(name, "h3", 2))
      أرجع ALPN_h3;
  }
  وإلا 𐡄𐡍(len == 8 && !memcmp(name, "http/1.1", 8))
    أرجع ALPN_h1;
  أرجع ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
ساكن timediff_t connect_left_ms(بنية Curl_easy *data,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t left_ms = timeout_ms - used_ms;
  أرجع left_ms ? left_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(بنية Curl_easy *data,
                                    ثابت بنية curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(بنية Curl_easy *data,
                                    ثابت بنية curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  𐡄𐡍(data->conn && CURL_CONN_IN_SHUTDOWN(data->conn, FIRSTSOCKET))
    أرجع Curl_cshutdn_timeleft_ms(data, data->conn, FIRSTSOCKET);
  وإلا 𐡄𐡍(Curl_is_connecting(data)) {
    ctimeleft_ms = connect_left_ms(data,
      Curl_pgrs_since_ms(data, pnow, TIMER_STARTSINGLE));
  }
  وإلا 𐡄𐡍(!data->set.timeout || data->set.connect_only) {
    أرجع 0; /* no timeout in place or checked, return "no limit" */
  }

  𐡄𐡍(data->set.timeout) {
    timeleft_ms = data->set.timeout -
                  Curl_pgrs_since_ms(data, pnow, TIMER_STARTOP);
    𐡄𐡍(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  𐡄𐡍(!ctimeleft_ms)
    أرجع timeleft_ms;
  وإلا 𐡄𐡍(!timeleft_ms)
    أرجع ctimeleft_ms;
  أرجع CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(بنية Curl_easy *data)
{
  أرجع timeleft_now_ms(data, Curl_pgrs_now(data));
}

timediff_t Curl_timeleft_now_ms(بنية Curl_easy *data,
                                ثابت بنية curltime *pnow)
{
  أرجع timeleft_now_ms(data, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(بنية Curl_easy *data,
                                  بنية connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  بنية connectdata *conn = Curl_cpool_get_last_conn(data);
  𐡄𐡍(connp)
    *connp = conn;
  أرجع conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

فراغ Curl_conncontrol(بنية connectdata *conn, صحيح ctrl)
{
  𐡄𐡍(!conn) {
    DEBUGASSERT(0);
    أرجع;
  }
  بدل(ctrl) {
    حالة CONNCTRL_CONN_KEEP:
      conn->bits.close = FALSE;
      اكسر;
    حالة CONNCTRL_CONN_CLOSE:
      conn->bits.close = TRUE;
      اكسر;
    حالة CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      𐡄𐡍(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.close = TRUE;
      اكسر;
    افتراضي:
      DEBUGASSERT(0);
      اكسر;
  }
}

CURLcode Curl_conn_setup(بنية Curl_easy *data,
                         بنية connectdata *conn,
                         int8_t sockindex,
                         صحيح ssl_mode)
{
  بنية Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode result = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(data);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  𐡄𐡍(!first_peer)
    أرجع CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  𐡄𐡍(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    result = Curl_cf_https_setup(
      data, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    𐡄𐡍(result)
      اذهب out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  𐡄𐡍(!conn->cfilter[sockindex]) {
    result = Curl_cf_setup_add(data, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    𐡄𐡍(result)
      اذهب out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(data, conn->ip_version);
  result = Curl_conn_dns_add_addr_resolve(data, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  𐡄𐡍(result)
    اذهب out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  أرجع result;
}

#ifdef CURLVERBOSE
ساكن CURLcode conn_connect_trace(بنية Curl_easy *data,
                                   بنية Curl_cfilter *cf)
{
  𐡄𐡍(Curl_trc_is_verbose(data)) {
    بنية ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode result;

    result = Curl_conn_cf_get_ip_info(cf, data, &is_ipv6, &ipquad);
    𐡄𐡍(result)
      أرجع result;

    infof(data, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(data->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  أرجع CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
ساكن فراغ conn_report_stats(بنية Curl_easy *data, صحيح sockindex)
{
  /* We do gather stats for the second socket...yet */
  𐡄𐡍(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(data, data->conn, sockindex);
  }
}

فراغ Curl_conn_report_failed(بنية Curl_easy *data, int8_t sockindex)
{
  𐡄𐡍(!data->conn || (sockindex != FIRSTSOCKET))
    أرجع;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (فراغ)Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
  conn_report_stats(data, sockindex);
}

CURLcode Curl_conn_connect(بنية Curl_easy *data,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#define CF_CONN_NUM_POLLS_ON_STACK 5
  بنية pollfd a_few_on_stack[CF_CONN_NUM_POLLS_ON_STACK];
  بنية easy_pollset ps;
  بنية curl_pollfds cpfds;
  بنية Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  𐡄𐡍(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  𐡄𐡍(data->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  cf = data->conn->cfilter[sockindex];
  𐡄𐡍(!cf) {
    *done = FALSE;
    أرجع CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  𐡄𐡍(*done)
    أرجع CURLE_OK;

  𐡄𐡍(!data->conn->connect_started.set[sockindex]) {
    data->conn->connect_started.at[sockindex] = *Curl_pgrs_now(data);
    data->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_stack, CF_CONN_NUM_POLLS_ON_STACK);
  طالما(!*done) {
    𐡄𐡍(Curl_conn_needs_flush(data, sockindex)) {
      DEBUGF(infof(data, "Curl_conn_connect(index=%d), flush", sockindex));
      result = Curl_conn_flush(data, sockindex);
      𐡄𐡍(result && (result != CURLE_AGAIN))
        اذهب out;
    }

    result = cf->cft->do_connect(cf, data, done);
    CURL_TRC_CF(data, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (صحيح)result, *done);
    𐡄𐡍(!result && *done) {
      /* A final sanity check on connection security */
      𐡄𐡍((data->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(data, "transfer requires SSL, but not connected via SSL");
        result = CURLE_FAILED_INIT;
        اذهب out;
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
      اذهب out;
    }
    وإلا 𐡄𐡍(result) {
      CURL_TRC_CF(data, cf, "Curl_conn_connect(), filter returned %d",
                  (صحيح)result);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "failed to connect"));
      Curl_conn_report_failed(data, sockindex);
      اذهب out;
    }

    𐡄𐡍(!blocking)
      اذهب out;
    وإلا {
      /* check allowed time left */
      ثابت بنية curltime *pnow = Curl_pgrs_now(data);
      timediff_t timeout_ms = Curl_timeleft_now_ms(data, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, data);
      صحيح rc;

      𐡄𐡍(!Curl_is_connecting(data)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        ثابت بنية curltime *started =
          &data->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_left_ms(data,
          curlx_ptimediff_ms(pnow, started));
        𐡄𐡍(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      𐡄𐡍(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(data, "connect timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        اذهب out;
      }

      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      𐡄𐡍(sockfd != CURL_SOCKET_BAD)
        result = Curl_pollset_set_out_only(data, &ps, sockfd);
      𐡄𐡍(!result)
        result = Curl_conn_adjust_pollset(data, data->conn, &ps);
      𐡄𐡍(result)
        اذهب out;
      result = Curl_pollfds_add_ps(&cpfds, &ps);
      𐡄𐡍(result)
        اذهب out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      𐡄𐡍(rc < 0) {
        result = CURLE_COULDNT_CONNECT;
        اذهب out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  أرجع result;
}

فراغ Curl_conn_set_multiplex(بنية connectdata *conn)
{
  𐡄𐡍(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    𐡄𐡍(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

بنية Curl_peer *Curl_conn_get_origin(بنية connectdata *conn,
                                       int8_t sockindex)
{
  أرجع (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

بنية Curl_peer *Curl_conn_get_destination(بنية connectdata *conn,
                                            int8_t sockindex)
{
  أرجع (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

بنية Curl_peer *Curl_conn_get_first_peer(بنية connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  𐡄𐡍(conn->socks_proxy.peer)
    أرجع conn->socks_proxy.peer;
  𐡄𐡍(conn->http_proxy.peer)
    أرجع conn->http_proxy.peer;
#endif
  أرجع (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
