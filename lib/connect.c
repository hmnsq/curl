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

#якщо !defined(CURL_DISABLE_ALTSVC) || defined(USE_HTTPSRR)

enum alpnid Curl_alpn2alpnid(const unsigned char *name, size_t len)
{
  якщо(len == 2) {
    якщо(!memcmp(name, "h1", 2))
      повернути ALPN_h1;
    якщо(!memcmp(name, "h2", 2))
      повернути ALPN_h2;
    якщо(!memcmp(name, "h3", 2))
      повернути ALPN_h3;
  }
  інакше якщо(len == 8 && !memcmp(name, "http/1.1", 8))
    повернути ALPN_h1;
  повернути ALPN_none; /* unknown, probably rubbish input */
}

#endif

/*
 * Milliseconds left of the connect timeout after `used_ms` have been spent.
 * One home for the default timeout and the zero-to-expired sentinel.
 */
static timediff_t connect_left_ms(struct Curl_easy *data,
                                  timediff_t used_ms)
{
  timediff_t timeout_ms = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout : DEFAULT_CONNECT_TIMEOUT;
  timediff_t left_ms = timeout_ms - used_ms;
  повернути left_ms ? left_ms : -1; /* 0 is "no limit", fake 1 ms expiry */
}

/*
 * timeleft_now_ms() returns the amount of milliseconds left allowed for the
 * transfer/connection. If the value is 0, there is no timeout (ie there is
 * infinite time left). If the value is negative, the timeout time has already
 * elapsed.
 *
 * @unittest 1303
 */
UNITTEST timediff_t timeleft_now_ms(struct Curl_easy *data,
                                    const struct curltime *pnow);
UNITTEST timediff_t timeleft_now_ms(struct Curl_easy *data,
                                    const struct curltime *pnow)
{
  timediff_t timeleft_ms = 0;
  timediff_t ctimeleft_ms = 0;

  якщо(data->conn && CURL_CONN_IN_SHUTDOWN(data->conn, FIRSTSOCKET))
    повернути Curl_cshutdn_timeleft_ms(data, data->conn, FIRSTSOCKET);
  інакше якщо(Curl_is_connecting(data)) {
    ctimeleft_ms = connect_left_ms(data,
      Curl_pgrs_since_ms(data, pnow, TIMER_STARTSINGLE));
  }
  інакше якщо(!data->set.timeout || data->set.connect_only) {
    повернути 0; /* no timeout in place or checked, return "no limit" */
  }

  якщо(data->set.timeout) {
    timeleft_ms = data->set.timeout -
                  Curl_pgrs_since_ms(data, pnow, TIMER_STARTOP);
    якщо(!timeleft_ms)
      timeleft_ms = -1; /* 0 is "no limit", fake 1 ms expiry */
  }

  якщо(!ctimeleft_ms)
    повернути timeleft_ms;
  інакше якщо(!timeleft_ms)
    повернути ctimeleft_ms;
  повернути CURLMIN(ctimeleft_ms, timeleft_ms);
}

timediff_t Curl_timeleft_ms(struct Curl_easy *data)
{
  повернути timeleft_now_ms(data, Curl_pgrs_now(data));
}

timediff_t Curl_timeleft_now_ms(struct Curl_easy *data,
                                const struct curltime *pnow)
{
  повернути timeleft_now_ms(data, pnow);
}

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(struct Curl_easy *data,
                                  struct connectdata **connp)
{
  /* this works for an easy handle:
   * - that has been used for curl_easy_perform()
   * - that is associated with a multi handle, and whose connection
   *   was detached with CURLOPT_CONNECT_ONLY
   */
  struct connectdata *conn = Curl_cpool_get_last_conn(data);
  якщо(connp)
    *connp = conn;
  повернути conn ? conn->sock[FIRSTSOCKET] : CURL_SOCKET_BAD;
}

void Curl_conncontrol(struct connectdata *conn, ціле ctrl)
{
  якщо(!conn) {
    DEBUGASSERT(0);
    повернути;
  }
  switch(ctrl) {
    case CONNCTRL_CONN_KEEP:
      conn->bits.close = FALSE;
      break;
    case CONNCTRL_CONN_CLOSE:
      conn->bits.close = TRUE;
      break;
    case CONNCTRL_STREAM_CLOSE:
      /* stream close when multiplexing does not affect connection */
      якщо(!Curl_conn_is_multiplex(conn, FIRSTSOCKET))
        conn->bits.close = TRUE;
      break;
    default:
      DEBUGASSERT(0);
      break;
  }
}

CURLcode Curl_conn_setup(struct Curl_easy *data,
                         struct connectdata *conn,
                         int8_t sockindex,
                         ціле ssl_mode)
{
  struct Curl_peer *first_peer = Curl_conn_get_first_peer(conn, sockindex);
  CURLcode result = CURLE_OK;
  uint8_t dns_queries;

  DEBUGASSERT(data);
  DEBUGASSERT(conn->scheme);
  DEBUGASSERT(!conn->cfilter[sockindex]);

  якщо(!first_peer)
    повернути CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_HTTP
  якщо(!conn->cfilter[sockindex] &&
     conn->scheme->protocol == CURLPROTO_HTTPS) {
    DEBUGASSERT(ssl_mode != CURL_CF_SSL_DISABLE);
    result = Curl_cf_https_setup(
      data, Curl_conn_get_destination(conn, sockindex), conn, sockindex);
    якщо(result)
      goto out;
  }
#endif /* !CURL_DISABLE_HTTP */

  /* Still no cfilter set, apply default. */
  якщо(!conn->cfilter[sockindex]) {
    result = Curl_cf_setup_add(data, conn, sockindex,
                               conn->transport_wanted, ssl_mode);
    якщо(result)
      goto out;
  }

  /* Whatever the filter chain will be in the end, it will need the
   * resolving of `first_peer`. Add that now so the resolve is started
   * right away. */
  dns_queries = Curl_resolv_dns_queries(data, conn->ip_version);
  result = Curl_conn_dns_add_addr_resolve(data, conn, sockindex,
                                          first_peer, dns_queries,
                                          conn->transport_wanted);
  якщо(result)
    goto out;

  DEBUGASSERT(conn->cfilter[sockindex]);
out:
  повернути result;
}

#ifdef CURLVERBOSE
static CURLcode conn_connect_trace(struct Curl_easy *data,
                                   struct Curl_cfilter *cf)
{
  якщо(Curl_trc_is_verbose(data)) {
    struct ip_quadruple ipquad;
    bool is_ipv6;
    CURLcode result;

    result = Curl_conn_cf_get_ip_info(cf, data, &is_ipv6, &ipquad);
    якщо(result)
      повернути result;

    infof(data, "Established %sconnection to %s (%s port %u) from %s port %u ",
          (cf->sockindex == SECONDARYSOCKET) ? "2nd " : "",
          CURL_CONN_HOST_DISPNAME(data->conn),
          ipquad.remote_ip, ipquad.remote_port,
          ipquad.local_ip, ipquad.local_port);
  }
  повернути CURLE_OK;
}
#endif

/**
 * Update connection statistics
 */
static void conn_report_stats(struct Curl_easy *data, ціле sockindex)
{
  /* We do gather stats for the second socket...yet */
  якщо(sockindex == FIRSTSOCKET) {
    Curl_conn_cntrl_report_stats(data, data->conn, sockindex);
  }
}

void Curl_conn_report_failed(struct Curl_easy *data, int8_t sockindex)
{
  якщо(!data->conn || (sockindex != FIRSTSOCKET))
    повернути;
  /* Only filters connected at IP level have info to update. Filters
   * still racing for a connection, like happy eyeballing, pass it on
   * to a connected one only. */
  (void)Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, TRUE,
                           CF_CTRL_CONN_INFO_UPDATE, 0, NULL);
  conn_report_stats(data, sockindex);
}

CURLcode Curl_conn_connect(struct Curl_easy *data,
                           int8_t sockindex,
                           bool blocking,
                           bool *done)
{
#define CF_CONN_NUM_POLLS_ON_STACK 5
  struct pollfd a_few_on_stack[CF_CONN_NUM_POLLS_ON_STACK];
  struct easy_pollset ps;
  struct curl_pollfds cpfds;
  struct Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  якщо(!CONN_SOCK_IDX_VALID(sockindex))
    повернути CURLE_BAD_FUNCTION_ARGUMENT;

  якщо(data->conn->scheme->flags & PROTOPT_NONETWORK) {
    *done = TRUE;
    повернути CURLE_OK;
  }

  cf = data->conn->cfilter[sockindex];
  якщо(!cf) {
    *done = FALSE;
    повернути CURLE_FAILED_INIT;
  }

  *done = (bool)cf->connected;
  якщо(*done)
    повернути CURLE_OK;

  якщо(!data->conn->connect_started.set[sockindex]) {
    data->conn->connect_started.at[sockindex] = *Curl_pgrs_now(data);
    data->conn->connect_started.set[sockindex] = TRUE;
  }

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_stack, CF_CONN_NUM_POLLS_ON_STACK);
  поки(!*done) {
    якщо(Curl_conn_needs_flush(data, sockindex)) {
      DEBUGF(infof(data, "Curl_conn_connect(index=%d), flush", sockindex));
      result = Curl_conn_flush(data, sockindex);
      якщо(result && (result != CURLE_AGAIN))
        goto out;
    }

    result = cf->cft->do_connect(cf, data, done);
    CURL_TRC_CF(data, cf, "Curl_conn_connect(block=%d) -> %d, done=%d",
                blocking, (ціле)result, *done);
    якщо(!result && *done) {
      /* A final sanity check on connection security */
      якщо((data->state.origin->scheme->flags & PROTOPT_SSL) &&
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
    інакше якщо(result) {
      CURL_TRC_CF(data, cf, "Curl_conn_connect(), filter returned %d",
                  (ціле)result);
      VERBOSE(Curl_conn_trc_filters(data, sockindex, "failed to connect"));
      Curl_conn_report_failed(data, sockindex);
      goto out;
    }

    якщо(!blocking)
      goto out;
    інакше {
      /* check allowed time left */
      const struct curltime *pnow = Curl_pgrs_now(data);
      timediff_t timeout_ms = Curl_timeleft_now_ms(data, pnow);
      curl_socket_t sockfd = Curl_conn_cf_get_socket(cf, data);
      ціле rc;

      якщо(!Curl_is_connecting(data)) {
        /* Past MSTATE_DO, e.g. an FTP data connection handshake in DO_MORE,
         * timeleft_now_ms() leaves the connect timeout out. Bound the wait
         * by this socket's whole connect phase, or a peer that never
         * finishes the handshake blocks forever. */
        const struct curltime *started =
          &data->conn->connect_started.at[sockindex];
        timediff_t ctimeleft_ms = connect_left_ms(data,
          curlx_ptimediff_ms(pnow, started));
        якщо(!timeout_ms || (ctimeleft_ms < timeout_ms))
          timeout_ms = ctimeleft_ms;
      }

      якщо(timeout_ms < 0) {
        /* no need to continue if time already is up */
        failf(data, "connect timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        goto out;
      }

      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), do poll");
      Curl_pollset_reset(&ps);
      Curl_pollfds_reset(&cpfds);
      /* In general, we want to send after connect, wait on that. */
      якщо(sockfd != CURL_SOCKET_BAD)
        result = Curl_pollset_set_out_only(data, &ps, sockfd);
      якщо(!result)
        result = Curl_conn_adjust_pollset(data, data->conn, &ps);
      якщо(result)
        goto out;
      result = Curl_pollfds_add_ps(&cpfds, &ps);
      якщо(result)
        goto out;

      rc = Curl_poll(cpfds.pfds, cpfds.n,
                     CURLMIN(timeout_ms ? timeout_ms : 1000,
                             (cpfds.n ? 1000 : 10)));
      CURL_TRC_CF(data, cf, "Curl_conn_connect(block=1), Curl_poll() -> %d",
                  rc);
      якщо(rc < 0) {
        result = CURLE_COULDNT_CONNECT;
        goto out;
      }
      /* continue iterating */
    }
  }

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  повернути result;
}

void Curl_conn_set_multiplex(struct connectdata *conn)
{
  якщо(!conn->bits.multiplex) {
    conn->bits.multiplex = TRUE;
    якщо(conn->attached_multi) {
      Curl_multi_connchanged(conn->attached_multi);
    }
  }
}

struct Curl_peer *Curl_conn_get_origin(struct connectdata *conn,
                                       int8_t sockindex)
{
  повернути (sockindex == SECONDARYSOCKET) ?
    conn->origin2 : conn->origin;
}

struct Curl_peer *Curl_conn_get_destination(struct connectdata *conn,
                                            int8_t sockindex)
{
  повернути (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}

struct Curl_peer *Curl_conn_get_first_peer(struct connectdata *conn,
                                           int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  якщо(conn->socks_proxy.peer)
    повернути conn->socks_proxy.peer;
  якщо(conn->http_proxy.peer)
    повернути conn->http_proxy.peer;
#endif
  повернути (sockindex == SECONDARYSOCKET) ?
    (conn->via_peer2 ? conn->via_peer2 : conn->origin2) :
    (conn->via_peer ? conn->via_peer : conn->origin);
}
