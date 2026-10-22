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

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif

#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

#ضمن "urldata.h"
#ضمن "connect.h"
#ضمن "cfilters.h"
#ضمن "cf-ip-happy.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "multiif.h"
#ضمن "progress.h"
#ضمن "select.h"
#ضمن "sockaddr.h"
#ضمن "vdns/cf-dns.h"
#ضمن "vquic/vquic.h" /* for quic cfilters */


بنية transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

ساكن
#ifndef UNITTESTS
ثابت
#endif
بنية transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#𐡄𐡍 !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#𐡄𐡍 !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
  { Curl_cf_h3_proxy_create, TRNSPRT_QUIC, TRUE },
#endif
#ifndef CURL_DISABLE_TFTP
  { Curl_cf_udp_create, TRNSPRT_UDP, FALSE },
#endif
#ifdef USE_UNIX_SOCKETS
  { Curl_cf_unix_create, TRNSPRT_UNIX, FALSE },
  { Curl_cf_unix_create, TRNSPRT_UNIX, TRUE },
#endif
};

ساكن cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    𐡄𐡍((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      أرجع transport_providers[i].cf_create;
  }
  أرجع عدم;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST فراغ debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST فراغ debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    𐡄𐡍(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

بنية cf_ai_iter {
  بنية Curl_cfilter *cf;
  بنية Curl_peer *peer;
  صحيح ai_family;
  unsigned صحيح n;
};

ساكن فراغ cf_ai_iter_init(بنية cf_ai_iter *iter,
                            بنية Curl_cfilter *cf,
                            بنية Curl_peer *peer,
                            صحيح ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

ساكن ثابت بنية Curl_addrinfo *cf_ai_iter_next(بنية cf_ai_iter *iter,
                                                   بنية Curl_easy *data)
{
  ثابت بنية Curl_addrinfo *addr;

  𐡄𐡍(!iter->cf)
    أرجع عدم;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  𐡄𐡍(addr)
    iter->n++;
  أرجع addr;
}

ساكن bool cf_ai_iter_has_more(بنية cf_ai_iter *iter,
                                بنية Curl_easy *data)
{
  أرجع (iter->cf &&
          !!Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

بنية cf_ip_attempt {
  بنية cf_ip_attempt *next;
  بنية Curl_peer *origin;
  بنية Curl_peer *peer;
  بنية Curl_peer *tunnel_peer;
  بنية Curl_sockaddr_ex addr;
  بنية Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode result;
  صحيح ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  صحيح error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

ساكن فراغ cf_ip_attempt_free(بنية cf_ip_attempt *a,
                               بنية Curl_easy *data)
{
  𐡄𐡍(a) {
    𐡄𐡍(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, data);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

ساكن CURLcode cf_ip_attempt_new(بنية cf_ip_attempt **pa,
                                  بنية Curl_easy *data,
                                  بنية Curl_cfilter *cf,
                                  بنية Curl_peer *origin,
                                  بنية Curl_peer *peer,
                                  uint8_t transport_peer,
                                  بنية Curl_sockaddr_ex *addr,
                                  صحيح ai_family,
                                  بنية Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  بنية Curl_cfilter *wcf;
  بنية cf_ip_attempt *a;
  CURLcode result = CURLE_OK;

  *pa = عدم;
  a = curlx_calloc(1, حجم(*a));
  𐡄𐡍(!a)
    أرجع CURLE_OUT_OF_MEMORY;

  Curl_peer_link(&a->origin, origin);
  Curl_peer_link(&a->peer, peer);
  a->transport_peer = transport_peer;
  Curl_peer_link(&a->tunnel_peer, tunnel_peer);
  a->tunnel_transport = tunnel_transport;
  a->addr = *addr;
  a->ai_family = ai_family;
  a->result = CURLE_OK;
  a->cf_create = cf_create;
  *pa = a;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  𐡄𐡍(result)
    اذهب out;

  /* the new filter might have sub-filters */
  لكل(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  𐡄𐡍(result) {
    cf_ip_attempt_free(a, data);
    *pa = عدم;
  }
  أرجع result;
}

ساكن CURLcode cf_ip_attempt_connect(بنية cf_ip_attempt *a,
                                      بنية Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  𐡄𐡍(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    𐡄𐡍(!a->result) {
      𐡄𐡍(*connected) {
        a->connected = TRUE;
      }
    }
    وإلا {
      𐡄𐡍(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      𐡄𐡍(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  أرجع a->result;
}

بنية cf_ip_ballers {
  بنية cf_ip_attempt *running;
  بنية cf_ip_attempt **running_tail;  /* &running or &(last->next) */
  بنية cf_ip_attempt *winner;
  بنية cf_ai_iter addr_iter;
#ifdef USE_IPV6
  بنية cf_ai_iter ipv6_iter;
#endif
  بنية Curl_peer *origin;
  بنية Curl_peer *peer;
  بنية Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  بنية curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_result;  /* result of last discarded hard failure */
  صحيح last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

ساكن CURLcode cf_ip_attempt_restart(بنية cf_ip_attempt *a,
                                      بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data)
{
  بنية Curl_cfilter *wcf;
  CURLcode result;

  𐡄𐡍(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = عدم;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  𐡄𐡍(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    لكل(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  أرجع result;
}

ساكن فراغ cf_ip_ballers_clear(بنية Curl_easy *data,
                                بنية cf_ip_ballers *bs)
{
  طالما(bs->running) {
    بنية cf_ip_attempt *a = bs->running;
    bs->running = a->next;
    cf_ip_attempt_free(a, data);
  }
  bs->running_tail = &bs->running;
  cf_ip_attempt_free(bs->winner, data);
  bs->winner = عدم;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

ساكن CURLcode cf_ip_ballers_init(بنية cf_ip_ballers *bs,
                                   بنية Curl_easy *data,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   uint8_t transport_peer,
                                   بنية Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, حجم(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  𐡄𐡍(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  أرجع CURLE_OK;
}

ساكن فراغ cf_ip_ballers_prune(بنية cf_ip_ballers *bs,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                uint32_t max_concurrent)
{
  بنية cf_ip_attempt *a = عدم, **panchor;
  uint32_t ongoing = 0;

  لكل(a = bs->running; a; a = a->next) {
    𐡄𐡍(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  طالما(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    𐡄𐡍(!a->result && !a->connected) {
      *panchor = a->next;
      𐡄𐡍(!a->next)
        bs->running_tail = panchor;
      a->next = عدم;
      cf_ip_attempt_free(a, data);
      --ongoing;
      CURL_TRC_CF(data, cf, "discarding oldest attempt to keep limit");
    }
    وإلا {
      panchor = &a->next;
    }
  }
}

ساكن CURLcode cf_ip_ballers_run(بنية cf_ip_ballers *bs,
                                  بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode result = CURLE_OK;
  بنية cf_ip_attempt *a = عدم, **panchor;
  bool do_more;
  timediff_t next_expire_ms;
  uint32_t inconclusive, ongoing;
  ثابت بنية curltime *pnow = عدم;
  VERBOSE(صحيح i);

  𐡄𐡍(bs->winner)
    أرجع CURLE_OK;

evaluate:
  𐡄𐡍(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, عدم, TIMER_STARTSINGLE));
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  لكل(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    𐡄𐡍(!a->result) {
      𐡄𐡍(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = عدم;
        طالما(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        أرجع CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    وإلا 𐡄𐡍(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    وإلا {
      /* hard failure, discard right away */
      *panchor = a->next;
      𐡄𐡍(!a->next)
        bs->running_tail = panchor;
      a->next = عدم;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  𐡄𐡍(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  𐡄𐡍(!ongoing) {
    do_more = TRUE;
  }
  وإلا {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    𐡄𐡍(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    𐡄𐡍(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  𐡄𐡍(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ثابت بنية Curl_addrinfo *ai = عدم;
    صحيح ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    𐡄𐡍((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    𐡄𐡍(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    𐡄𐡍(ai) {  /* try another address */
      بنية Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      𐡄𐡍(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      𐡄𐡍(result)
        اذهب out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (صحيح)result);
      𐡄𐡍(result)
        اذهب out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_tail = a;
      a->next = عدم;
      bs->running_tail = &a->next;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      اذهب evaluate;
    }
    وإلا 𐡄𐡍(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      𐡄𐡍(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        لكل(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          𐡄𐡍(!a->inconclusive)
            استمر;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (صحيح)result);
          𐡄𐡍(result) /* serious failure */
            اذهب out;
          bs->last_attempt_started = *pnow;
          اذهب evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      وإلا {
        /* let's wait some more before restarting */
        infof(data, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      اذهب out;
    }
    وإلا 𐡄𐡍(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  𐡄𐡍(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    𐡄𐡍(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, عدم, TIMER_STARTSINGLE));
      أرجع CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    𐡄𐡍(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    𐡄𐡍(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      𐡄𐡍(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        اذهب evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  أرجع result;
}

ساكن CURLcode cf_ip_ballers_shutdown(بنية cf_ip_ballers *bs,
                                       بنية Curl_easy *data,
                                       bool *done)
{
  بنية cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  لكل(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    𐡄𐡍(a->shutdown || !a->cf)
      استمر;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    𐡄𐡍(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    وإلا
      *done = FALSE;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ip_ballers_pollset(بنية cf_ip_ballers *bs,
                                      بنية Curl_easy *data,
                                      بنية easy_pollset *ps)
{
  بنية cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  لكل(a = bs->running; a && !result; a = a->next) {
    𐡄𐡍(a->result)
      استمر;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  أرجع result;
}

ساكن bool cf_ip_ballers_pending(بنية cf_ip_ballers *bs,
                                  ثابت بنية Curl_easy *data)
{
  بنية cf_ip_attempt *a;

  لكل(a = bs->running; a; a = a->next) {
    𐡄𐡍(a->result)
      استمر;
    𐡄𐡍(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن صحيح cf_ip_ballers_min_reply_ms(بنية cf_ip_ballers *bs,
                                      بنية Curl_easy *data)
{
  صحيح reply_ms = -1, breply_ms;
  بنية cf_ip_attempt *a;

  لكل(a = bs->running; a; a = a->next) {
    𐡄𐡍(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, عدم) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  أرجع reply_ms;
}

تعريفنوع تعداد {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

بنية cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  بنية cf_ip_ballers ballers;
  BIT(dns_resolved);
};

ساكن CURLcode is_connected(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data,
                             bool *connected)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  بنية connectdata *conn = cf->conn;
  CURLcode result;

  result = cf_ip_ballers_run(&ctx->ballers, cf, data,
                             (bool)ctx->dns_resolved, connected);

  𐡄𐡍(!result)
    أرجع CURLE_OK;
  وإلا {
    بنية Curl_peer *peer = عدم, *proxy_peer = عدم;
    حرف viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    𐡄𐡍(!conn->origin || !peer)
      أرجع CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    𐡄𐡍(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    وإلا 𐡄𐡍(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    𐡄𐡍(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      𐡄𐡍(peer->unix_socket)
        curl_msnprintf(viamsg, حجم(viamsg), " over unix://%s",
                       peer->hostname);
      وإلا
#endif
      curl_msnprintf(viamsg, حجم(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(data, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(data, عدم, TIMER_STARTSINGLE),
          curl_easy_strerror(result));

#ifdef SOCKETIMEDOUT
    𐡄𐡍(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    أرجع result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

ساكن CURLcode cf_ip_happy_init(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  𐡄𐡍(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  𐡄𐡍(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#وإلا
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  وإلا { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(data, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  أرجع CURLE_OK;
}

ساكن فراغ cf_ip_happy_ctx_clear(بنية cf_ip_happy_ctx *ctx,
                                  بنية Curl_easy *data)
{
  DEBUGASSERT(ctx);
  𐡄𐡍(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

ساكن فراغ cf_ip_happy_ctx_destroy(بنية cf_ip_happy_ctx *ctx,
                                    بنية Curl_easy *data)
{
  𐡄𐡍(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_ip_happy_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *data,
                                     bool *done)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  𐡄𐡍(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (صحيح)result, *done);
  أرجع result;
}

ساكن CURLcode cf_ip_happy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *data,
                                           بنية easy_pollset *ps)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  𐡄𐡍(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (صحيح)result,
                ps->n);
  }
  أرجع result;
}

ساكن CURLcode cf_ip_happy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    bool *done)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  𐡄𐡍(!data)
    أرجع CURLE_FAILED_INIT;

  𐡄𐡍(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  𐡄𐡍(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    𐡄𐡍(!result)
      ctx->dns_resolved = TRUE;
    وإلا 𐡄𐡍(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    وإلا /* real error */
      اذهب out;
  }

  بدل(ctx->state) {
  حالة SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    𐡄𐡍(result)
      اذهب out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  حالة SCFST_WAITING:
    result = is_connected(cf, data, done);
    𐡄𐡍(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = عدم;

      𐡄𐡍(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      𐡄𐡍(Curl_trc_cf_is_verbose(cf, data)) {
        بنية ip_quadruple ipquad;
        bool is_ipv6;
        𐡄𐡍(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(data, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_port);
        }
      }
#endif
      cf_ip_happy_ctx_clear(ctx, data);
      Curl_expire_clear(data, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(data);
      data->info.numconnects++; /* to track the # of connections made */
    }
    اكسر;
  حالة SCFST_DONE:
    *done = TRUE;
    اكسر;
  }
out:
  أرجع result;
}

ساكن bool cf_ip_happy_data_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *data)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  𐡄𐡍(!cf->connected) {
    أرجع cf_ip_ballers_pending(&ctx->ballers, data);
  }
  أرجع cf->next->cft->has_data_pending(cf->next, data);
}

ساكن CURLcode cf_ip_happy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  𐡄𐡍(!cf->connected) {
    بدل(query) {
    حالة CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      أرجع CURLE_OK;
    }
    افتراضي:
      اكسر;
    }
  }

  أرجع cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن فراغ cf_ip_happy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  𐡄𐡍(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
}

بنية Curl_cftype Curl_cft_ip_happy = {
  "HAPPY-EYEBALLS",
  CF_TYPE_SETUP,
  CURL_LOG_LVL_NONE,
  cf_ip_happy_destroy,
  cf_ip_happy_connect,
  cf_ip_happy_shutdown,
  cf_ip_happy_adjust_pollset,
  cf_ip_happy_data_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

ساكن CURLcode cf_ip_happy_create(بنية Curl_cfilter **pcf,
                                   بنية Curl_easy *data,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   uint8_t transport_peer,
                                   بنية connectdata *conn,
                                   بنية Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  بنية cf_ip_happy_ctx *ctx = عدم;
  CURLcode result;

  (فراغ)data;
  (فراغ)conn;
  *pcf = عدم;
  ctx = curlx_calloc(1, حجم(*ctx));
  𐡄𐡍(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  𐡄𐡍(result)
    اذهب out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  𐡄𐡍(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  أرجع result;
}

CURLcode Curl_cf_ip_happy_insert_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *data,
                                       بنية Curl_peer *origin,
                                       بنية Curl_peer *peer,
                                       uint8_t transport_peer,
                                       بنية Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  بنية Curl_cfilter *cf;
  CURLcode result;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  result = cf_ip_happy_create(&cf, data, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  𐡄𐡍(result)
    أرجع result;

  Curl_conn_cf_insert_after(cf_at, cf);
  أرجع CURLE_OK;
}
