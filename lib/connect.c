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
#include "curl_setup.h"

#include "urldata.h"
#include "curl_trc.h"
#include "strerror.h"
#include "cfilters.h"
#include "connect.h"
#include "cf-https-connect.h"
#include "cf-setup.h"
#include "multiif.h"
#include "progress.h"
#include "conncache.h"
#include "multihandle.h"
#include "select.h"
#include "vdns/cf-dns.h"
#include "curlx/strparse.h"

#ꦪꦼꦤ꧀ !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

ga_nanaa alpnid Curl_alpn2alpnid(const unsigned var_na *name, size_t len)
{
  ꦪꦼꦤ꧀(len == 2) {
    ꦪꦼꦤ꧀(!memcmp(name, "h1", 2))
      ꦮꦭꦶ ALPN_h1;
    ꦪꦼꦤ꧀(!memcmp(name, "h2", 2))
      ꦮꦭꦶ ALPN_h2;
    ꦪꦼꦤ꧀(!memcmp(name, "h3", 2))
      ꦮꦭꦶ ALPN_h3;
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(len == 8 && !memcmp(name, "http/1.1", 8))
    ꦮꦭꦶ ALPN_h1;
  ꦮꦭꦶ ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
static timediff_t connect_left_ms(sa_nracanaa Curl_easy *data,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t left_ms = timeout_ms - used_ms;
  ꦮꦭꦶ left_ms ? left_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(sa_nracanaa Curl_easy *data,
                                    const sa_nracanaa curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(sa_nracanaa Curl_easy *data,
                                    const sa_nracanaa curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  ꦪꦼꦤ꧀(data->conn && CURL_CONN_IN_SHUTDOWN(data->conn, FIRSTSOCKET))
    ꦮꦭꦶ Curl_cshutdn_timeleft_ms(data, data->conn, FIRSTSOCKET);
  ꦭꦶꦪ ꦪꦼꦤ꧀(Curl_is_connecting(data)) {
    ctimeleft_ms = connect_left_ms(data,
      Curl_pgrs_since_ms(data, pnow, TIMER_STARTSINGLE));
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(!data->set.timeout || data->set.connect_only) {
    ꦮꦭꦶ 0; /* no timeout in place or checked, return "no limit" */
  }

  ꦪꦼꦤ꧀(data->set.timeout) {
    timeleft_ms = data->set.timeout -
                  Curl_pgrs_since_ms(data, pnow, TIMER_STARTOP);
    ꦪꦼꦤ꧀(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  ꦪꦼꦤ꧀(!ctimeleft_ms)
    ꦮꦭꦶ timeleft_ms;
  ꦭꦶꦪ ꦪꦼꦤ꧀(!timeleft_ms)
    ꦮꦭꦶ ctimeleft_ms;
  ꦮꦭꦶ CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(sa_nracanaa Curl_easy *data)
{
  ꦮꦭꦶ timeleft_now_ms(data, Curl_pgrs_now(data));
}

timediff_t Curl_timeleft_now_ms(sa_nracanaa Curl_easy *data,
                                const sa_nracanaa curltime *pnow)
{
  ꦮꦭꦶ timeleft_now_ms(data, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  sa_nracanaa connectdata *conn = Curl_cpool_get_last_conn(data);
  ꦪꦼꦤ꧀(connp)
    *connp = conn;
  ꦮꦭꦶ conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

void Curl_conncontrol(sa_nracanaa connectdata *conn, puur_naa_nka ctrl)
{
  ꦪꦼꦤ꧀(!conn) {
    DEBUGASSERT(0);
    ꦮꦭꦶ;
  }
  switch(ctrl) {
    pak_sa CONNCTRL_CONN_KEEP:
      conn->bits.close = FALSE;
      bha_nga;
    pak_sa CONNCTRL_CONN_CLOSE:
      conn->bits.close = TRUE;
      bha_nga;
    pak_sa CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      ꦪꦼꦤ꧀(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.close = TRUE;
      bha_nga;
    default:
      DEBUGASSERT(0);
      bha_nga;
  }
}

CURLcode Curl_conn_setup(sa_nracanaa Curl_easy *data,
                         sa_nracanaa connectdata *conn,
                         int8_t sockindex,
                         puur_naa_nka ssl_mode)
{
  sa_nracanaa Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode result = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(data);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  ꦪꦼꦤ꧀(!first_peer)
    ꦮꦭꦶ CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  ꦪꦼꦤ꧀(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    result = Curl_cf_https_setup(
      data, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    ꦪꦼꦤ꧀(result)
      goto out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  ꦪꦼꦤ꧀(!conn->cfilter[sockindex]) {
    result = Curl_cf_setup_add(data, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    ꦪꦼꦤ꧀(result)
      goto out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(data, conn->ip_version);
  result = Curl_conn_dns_add_addr_resolve(data, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  ꦪꦼꦤ꧀(result)
    goto out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  ꦮꦭꦶ result;
}

#ifdef CURLVERBOSE
static CURLcode conn_connect_trace(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa Curl_cfilter *cf)
{
  ꦪꦼꦤ꧀(Curl_trc_is_verbose(data)) {
    sa_nracanaa ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode result;

    result = Curl_conn_cf_get_ip_info(cf, data, &is_ipv6, &ipquad);
    ꦪꦼꦤ꧀(result)
      ꦮꦭꦶ result;

    infof(data, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(data->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  ꦮꦭꦶ CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
static void conn_report_stats(sa_nracanaa Curl_easy *data, puur_naa_nka sockindex)
{
  /* We do gather stats for the second socket...yet */
  ꦪꦼꦤ꧀(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(data, data->conn, sockindex);
  }
}

void Curl_conn_report_failed(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  ꦪꦼꦤ꧀(!data->conn || (sockindex != FIRSTSOCKET))
    ꦮꦭꦶ;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (void)Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, NULL);
  conn_report_stats(data, sockindex);
}

CURLcode Curl_conn_connect(sa_nracanaa Curl_easy *data,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#define CF_CONN_NUM_POLLS_ON_STACK 5
  sa_nracanaa pollfd a_few_on_stack[CF_CONN_NUM_POLLS_ON_STACK];
  sa_nracanaa easy_pollset ps;
  sa_nracanaa curl_pollfds cpfds;
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ CURLE_BAD_FUNCTION_ARGUMENT;

  ꦪꦼꦤ꧀(data->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    ꦮꦭꦶ CURLE_OK;
  }

  cf = data->conn->cfilter[sockindex];
  ꦪꦼꦤ꧀(!cf) {
    *done = FALSE;
    ꦮꦭꦶ CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  ꦪꦼꦤ꧀(*done)
    ꦮꦭꦶ CURLE_OK;

  ꦪꦼꦤ꧀(!data->conn->connect_started.set[sockindex]) {
    data->conn->connect_started.at[sockindex] = *Curl_pgrs_now(data);
    data->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_stack, CF_CONN_NUM_POLLS_ON_STACK);
  ꦱꦼꦢꦼꦭ(!*done) {
    ꦪꦼꦤ꧀(Curl_conn_needs_flush(data, sockindex)) {
      DEBUGF(infof(data, "Curl_conn_connect(index=%d), flush", sockindex));
      result = Curl_conn_flush(data, sockindex);
      ꦪꦼꦤ꧀(result && (result != CURLE_AGAIN))
        goto out;
    }

    result = cf->cft->do_connect(cf, data, done);
    CURL_TRC_CF(data, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (puur_naa_nka)result, *done);
    ꦪꦼꦤ꧀(!result && *done) {
      /* A final sanity check on connection security */
      ꦪꦼꦤ꧀((data->state.origin->scheme->flags & PROTOPT_SSL) &&
         (sockindex == FIRSTSOCKET) &&
         !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        DEBUGASSERT(0);
        failf(data, "transfer requires SSL, but not connected via SSL");
        result = CURLE_FAILED_INIT;
        goto out;
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
      goto out;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀(result) {
      CURL_TRC_CF(data, cf, "Curl_conn_connect(), filter returned %d",
                  (puur_naa_nka)result);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "failed to connect"));
      Curl_conn_report_failed(data, sockindex);
      goto out;
    }

    ꦪꦼꦤ꧀(!blocking)
      goto out;
    ꦭꦶꦪ {
      /* check allowed time left */
      const sa_nracanaa curltime *pnow = Curl_pgrs_now(data);
      timediff_t timeout_ms = Curl_timeleft_now_ms(data, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, data);
      puur_naa_nka rc;

      ꦪꦼꦤ꧀(!Curl_is_connecting(data)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        const sa_nracanaa curltime *started =
          &data->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_left_ms(data,
          curlx_ptimediff_ms(pnow, started));
        ꦪꦼꦤ꧀(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      ꦪꦼꦤ꧀(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(data, "connect timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        goto out;
      }

      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      ꦪꦼꦤ꧀(sockfd != CURL_SOCKET_BAD)
        result = Curl_pollset_set_out_only(data, &ps, sockfd);
      ꦪꦼꦤ꧀(!result)
        result = Curl_conn_adjust_pollset(data, data->conn, &ps);
      ꦪꦼꦤ꧀(result)
        goto out;
      result = Curl_pollfds_add_ps(&cpfds, &ps);
      ꦪꦼꦤ꧀(result)
        goto out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      ꦪꦼꦤ꧀(rc < 0) {
        result = CURLE_COULDNT_CONNECT;
        goto out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  ꦮꦭꦶ result;
}

void Curl_conn_set_multiplex(sa_nracanaa connectdata *conn)
{
  ꦪꦼꦤ꧀(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    ꦪꦼꦤ꧀(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

sa_nracanaa Curl_peer *Curl_conn_get_origin(sa_nracanaa connectdata *conn,
                                       int8_t sockindex)
{
  ꦮꦭꦶ (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

sa_nracanaa Curl_peer *Curl_conn_get_destination(sa_nracanaa connectdata *conn,
                                            int8_t sockindex)
{
  ꦮꦭꦶ (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

sa_nracanaa Curl_peer *Curl_conn_get_first_peer(sa_nracanaa connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  ꦪꦼꦤ꧀(conn->socks_proxy.peer)
    ꦮꦭꦶ conn->socks_proxy.peer;
  ꦪꦼꦤ꧀(conn->http_proxy.peer)
    ꦮꦭꦶ conn->http_proxy.peer;
#endif
  ꦮꦭꦶ (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
