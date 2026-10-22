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

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_LINUX_TCP_H
#അന്തര്ഭാവ <linux/tcp.h>
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_NETINET_TCP_H)
#അന്തര്ഭാവ <netinet/tcp.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#അന്തര്ഭാവ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif

#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-ip-happy.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "sockaddr.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "vquic/vquic.h" /* for quic cfilters */


ഘടന transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

സ്ഥിതം
#ifndef UNITTESTS
സ്ഥിരം
#endif
ഘടന transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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

സ്ഥിതം cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    എങ്കിൽ((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      തിരികെ transport_providers[i].cf_create;
  }
  തിരികെ ശൂന്യ;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST ശൂന്യം debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST ശൂന്യം debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    എങ്കിൽ(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

ഘടന cf_ai_iter {
  ഘടന Curl_cfilter *cf;
  ഘടന Curl_peer *peer;
  പൂർണ്ണസംഖ്യ ai_family;
  unsigned പൂർണ്ണസംഖ്യ n;
};

സ്ഥിതം ശൂന്യം cf_ai_iter_init(ഘടന cf_ai_iter *iter,
                            ഘടന Curl_cfilter *cf,
                            ഘടന Curl_peer *peer,
                            പൂർണ്ണസംഖ്യ ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_addrinfo *cf_ai_iter_next(ഘടന cf_ai_iter *iter,
                                                   ഘടന Curl_easy *data)
{
  സ്ഥിരം ഘടന Curl_addrinfo *addr;

  എങ്കിൽ(!iter->cf)
    തിരികെ ശൂന്യ;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  എങ്കിൽ(addr)
    iter->n++;
  തിരികെ addr;
}

സ്ഥിതം bool cf_ai_iter_has_more(ഘടന cf_ai_iter *iter,
                                ഘടന Curl_easy *data)
{
  തിരികെ (iter->cf &&
          !!Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

ഘടന cf_ip_attempt {
  ഘടന cf_ip_attempt *next;
  ഘടന Curl_peer *origin;
  ഘടന Curl_peer *peer;
  ഘടന Curl_peer *tunnel_peer;
  ഘടന Curl_sockaddr_ex addr;
  ഘടന Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode result;
  പൂർണ്ണസംഖ്യ ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  പൂർണ്ണസംഖ്യ error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

സ്ഥിതം ശൂന്യം cf_ip_attempt_free(ഘടന cf_ip_attempt *a,
                               ഘടന Curl_easy *data)
{
  എങ്കിൽ(a) {
    എങ്കിൽ(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, data);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

സ്ഥിതം CURLcode cf_ip_attempt_new(ഘടന cf_ip_attempt **pa,
                                  ഘടന Curl_easy *data,
                                  ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_peer *origin,
                                  ഘടന Curl_peer *peer,
                                  uint8_t transport_peer,
                                  ഘടന Curl_sockaddr_ex *addr,
                                  പൂർണ്ണസംഖ്യ ai_family,
                                  ഘടന Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  ഘടന Curl_cfilter *wcf;
  ഘടന cf_ip_attempt *a;
  CURLcode result = CURLE_OK;

  *pa = ശൂന്യ;
  a = curlx_calloc(1, വലുപ്പം(*a));
  എങ്കിൽ(!a)
    തിരികെ CURLE_OUT_OF_MEMORY;

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
  എങ്കിൽ(result)
    പോകുക out;

  /* the new filter might have sub-filters */
  വേണ്ടി(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  എങ്കിൽ(result) {
    cf_ip_attempt_free(a, data);
    *pa = ശൂന്യ;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_ip_attempt_connect(ഘടന cf_ip_attempt *a,
                                      ഘടന Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  എങ്കിൽ(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    എങ്കിൽ(!a->result) {
      എങ്കിൽ(*connected) {
        a->connected = TRUE;
      }
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      എങ്കിൽ(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  തിരികെ a->result;
}

ഘടന cf_ip_ballers {
  ഘടന cf_ip_attempt *running;
  ഘടന cf_ip_attempt **running_tail;  /* &running or &(last->next) */
  ഘടന cf_ip_attempt *winner;
  ഘടന cf_ai_iter addr_iter;
#ifdef USE_IPV6
  ഘടന cf_ai_iter ipv6_iter;
#endif
  ഘടന Curl_peer *origin;
  ഘടന Curl_peer *peer;
  ഘടന Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  ഘടന curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_result;  /* result of last discarded hard failure */
  പൂർണ്ണസംഖ്യ last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

സ്ഥിതം CURLcode cf_ip_attempt_restart(ഘടന cf_ip_attempt *a,
                                      ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data)
{
  ഘടന Curl_cfilter *wcf;
  CURLcode result;

  എങ്കിൽ(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = ശൂന്യ;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  എങ്കിൽ(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    വേണ്ടി(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_ip_ballers_clear(ഘടന Curl_easy *data,
                                ഘടന cf_ip_ballers *bs)
{
  ആകുമ്പോൾ(bs->running) {
    ഘടന cf_ip_attempt *a = bs->running;
    bs->running = a->next;
    cf_ip_attempt_free(a, data);
  }
  bs->running_tail = &bs->running;
  cf_ip_attempt_free(bs->winner, data);
  bs->winner = ശൂന്യ;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

സ്ഥിതം CURLcode cf_ip_ballers_init(ഘടന cf_ip_ballers *bs,
                                   ഘടന Curl_easy *data,
                                   ഘടന Curl_peer *origin,
                                   ഘടന Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ഘടന Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, വലുപ്പം(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  എങ്കിൽ(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cf_ip_ballers_prune(ഘടന cf_ip_ballers *bs,
                                ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                uint32_t max_concurrent)
{
  ഘടന cf_ip_attempt *a = ശൂന്യ, **panchor;
  uint32_t ongoing = 0;

  വേണ്ടി(a = bs->running; a; a = a->next) {
    എങ്കിൽ(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  ആകുമ്പോൾ(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    എങ്കിൽ(!a->result && !a->connected) {
      *panchor = a->next;
      എങ്കിൽ(!a->next)
        bs->running_tail = panchor;
      a->next = ശൂന്യ;
      cf_ip_attempt_free(a, data);
      --ongoing;
      CURL_TRC_CF(data, cf, "discarding oldest attempt to keep limit");
    }
    അല്ലെങ്കിൽ {
      panchor = &a->next;
    }
  }
}

സ്ഥിതം CURLcode cf_ip_ballers_run(ഘടന cf_ip_ballers *bs,
                                  ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode result = CURLE_OK;
  ഘടന cf_ip_attempt *a = ശൂന്യ, **panchor;
  bool do_more;
  timediff_t next_expire_ms;
  uint32_t inconclusive, ongoing;
  സ്ഥിരം ഘടന curltime *pnow = ശൂന്യ;
  VERBOSE(പൂർണ്ണസംഖ്യ i);

  എങ്കിൽ(bs->winner)
    തിരികെ CURLE_OK;

evaluate:
  എങ്കിൽ(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, ശൂന്യ, TIMER_STARTSINGLE));
    തിരികെ CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  വേണ്ടി(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    എങ്കിൽ(!a->result) {
      എങ്കിൽ(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = ശൂന്യ;
        ആകുമ്പോൾ(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        തിരികെ CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    അല്ലെങ്കിൽ {
      /* hard failure, discard right away */
      *panchor = a->next;
      എങ്കിൽ(!a->next)
        bs->running_tail = panchor;
      a->next = ശൂന്യ;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  എങ്കിൽ(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  എങ്കിൽ(!ongoing) {
    do_more = TRUE;
  }
  അല്ലെങ്കിൽ {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    എങ്കിൽ(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    എങ്കിൽ(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  എങ്കിൽ(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    സ്ഥിരം ഘടന Curl_addrinfo *ai = ശൂന്യ;
    പൂർണ്ണസംഖ്യ ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    എങ്കിൽ((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    എങ്കിൽ(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    എങ്കിൽ(ai) {  /* try another address */
      ഘടന Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      എങ്കിൽ(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      എങ്കിൽ(result)
        പോകുക out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (പൂർണ്ണസംഖ്യ)result);
      എങ്കിൽ(result)
        പോകുക out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_tail = a;
      a->next = ശൂന്യ;
      bs->running_tail = &a->next;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      പോകുക evaluate;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      എങ്കിൽ(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        വേണ്ടി(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          എങ്കിൽ(!a->inconclusive)
            തുടരുക;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (പൂർണ്ണസംഖ്യ)result);
          എങ്കിൽ(result) /* serious failure */
            പോകുക out;
          bs->last_attempt_started = *pnow;
          പോകുക evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      അല്ലെങ്കിൽ {
        /* let's wait some more before restarting */
        infof(data, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  എങ്കിൽ(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    എങ്കിൽ(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, ശൂന്യ, TIMER_STARTSINGLE));
      തിരികെ CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    എങ്കിൽ(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    എങ്കിൽ(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      എങ്കിൽ(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        പോകുക evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_ip_ballers_shutdown(ഘടന cf_ip_ballers *bs,
                                       ഘടന Curl_easy *data,
                                       bool *done)
{
  ഘടന cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  വേണ്ടി(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    എങ്കിൽ(a->shutdown || !a->cf)
      തുടരുക;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    എങ്കിൽ(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    അല്ലെങ്കിൽ
      *done = FALSE;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_ip_ballers_pollset(ഘടന cf_ip_ballers *bs,
                                      ഘടന Curl_easy *data,
                                      ഘടന easy_pollset *ps)
{
  ഘടന cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  വേണ്ടി(a = bs->running; a && !result; a = a->next) {
    എങ്കിൽ(a->result)
      തുടരുക;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  തിരികെ result;
}

സ്ഥിതം bool cf_ip_ballers_pending(ഘടന cf_ip_ballers *bs,
                                  സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന cf_ip_attempt *a;

  വേണ്ടി(a = bs->running; a; a = a->next) {
    എങ്കിൽ(a->result)
      തുടരുക;
    എങ്കിൽ(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cf_ip_ballers_min_reply_ms(ഘടന cf_ip_ballers *bs,
                                      ഘടന Curl_easy *data)
{
  പൂർണ്ണസംഖ്യ reply_ms = -1, breply_ms;
  ഘടന cf_ip_attempt *a;

  വേണ്ടി(a = bs->running; a; a = a->next) {
    എങ്കിൽ(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, ശൂന്യ) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  തിരികെ reply_ms;
}

തരനിർവചനം എണ്ണൽ {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

ഘടന cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  ഘടന cf_ip_ballers ballers;
  BIT(dns_resolved);
};

സ്ഥിതം CURLcode is_connected(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             bool *connected)
{
  ഘടന cf_ip_happy_ctx *ctx = cf->ctx;
  ഘടന connectdata *conn = cf->conn;
  CURLcode result;

  result = cf_ip_ballers_run(&ctx->ballers, cf, data,
                             (bool)ctx->dns_resolved, connected);

  എങ്കിൽ(!result)
    തിരികെ CURLE_OK;
  അല്ലെങ്കിൽ {
    ഘടന Curl_peer *peer = ശൂന്യ, *proxy_peer = ശൂന്യ;
    അക്ഷരം viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    എങ്കിൽ(!conn->origin || !peer)
      തിരികെ CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    എങ്കിൽ(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    അല്ലെങ്കിൽ എങ്കിൽ(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    എങ്കിൽ(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      എങ്കിൽ(peer->unix_socket)
        curl_msnprintf(viamsg, വലുപ്പം(viamsg), " over unix://%s",
                       peer->hostname);
      അല്ലെങ്കിൽ
#endif
      curl_msnprintf(viamsg, വലുപ്പം(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(data, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(data, ശൂന്യ, TIMER_STARTSINGLE),
          curl_easy_strerror(result));

#ifdef SOCKETIMEDOUT
    എങ്കിൽ(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    തിരികെ result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

സ്ഥിതം CURLcode cf_ip_happy_init(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data)
{
  ഘടന cf_ip_happy_ctx *ctx = cf->ctx;

  എങ്കിൽ(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    തിരികെ CURLE_OPERATION_TIMEDOUT;
  }

  എങ്കിൽ(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#അല്ലെങ്കിൽ
    തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  അല്ലെങ്കിൽ { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(data, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cf_ip_happy_ctx_clear(ഘടന cf_ip_happy_ctx *ctx,
                                  ഘടന Curl_easy *data)
{
  DEBUGASSERT(ctx);
  എങ്കിൽ(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

സ്ഥിതം ശൂന്യം cf_ip_happy_ctx_destroy(ഘടന cf_ip_happy_ctx *ctx,
                                    ഘടന Curl_easy *data)
{
  എങ്കിൽ(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    curlx_free(ctx);
  }
}

സ്ഥിതം CURLcode cf_ip_happy_shutdown(ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data,
                                     bool *done)
{
  ഘടന cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (പൂർണ്ണസംഖ്യ)result, *done);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_ip_happy_adjust_pollset(ഘടന Curl_cfilter *cf,
                                           ഘടന Curl_easy *data,
                                           ഘടന easy_pollset *ps)
{
  ഘടന cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (പൂർണ്ണസംഖ്യ)result,
                ps->n);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_ip_happy_connect(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    bool *done)
{
  ഘടന cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  എങ്കിൽ(!data)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  എങ്കിൽ(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    എങ്കിൽ(!result)
      ctx->dns_resolved = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    അല്ലെങ്കിൽ /* real error */
      പോകുക out;
  }

  തിരഞ്ഞെടുപ്പ്(ctx->state) {
  അവസ്ഥ SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  അവസ്ഥ SCFST_WAITING:
    result = is_connected(cf, data, done);
    എങ്കിൽ(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = ശൂന്യ;

      എങ്കിൽ(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      എങ്കിൽ(Curl_trc_cf_is_verbose(cf, data)) {
        ഘടന ip_quadruple ipquad;
        bool is_ipv6;
        എങ്കിൽ(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
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
    നിർത്തുക;
  അവസ്ഥ SCFST_DONE:
    *done = TRUE;
    നിർത്തുക;
  }
out:
  തിരികെ result;
}

സ്ഥിതം bool cf_ip_happy_data_pending(ഘടന Curl_cfilter *cf,
                                     സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന cf_ip_happy_ctx *ctx = cf->ctx;

  എങ്കിൽ(!cf->connected) {
    തിരികെ cf_ip_ballers_pending(&ctx->ballers, data);
  }
  തിരികെ cf->next->cft->has_data_pending(cf->next, data);
}

സ്ഥിതം CURLcode cf_ip_happy_query(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന cf_ip_happy_ctx *ctx = cf->ctx;

  എങ്കിൽ(!cf->connected) {
    തിരഞ്ഞെടുപ്പ്(query) {
    അവസ്ഥ CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      തിരികെ CURLE_OK;
    }
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
  }

  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

സ്ഥിതം ശൂന്യം cf_ip_happy_destroy(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data)
{
  ഘടന cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  എങ്കിൽ(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
}

ഘടന Curl_cftype Curl_cft_ip_happy = {
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

സ്ഥിതം CURLcode cf_ip_happy_create(ഘടന Curl_cfilter **pcf,
                                   ഘടന Curl_easy *data,
                                   ഘടന Curl_peer *origin,
                                   ഘടന Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ഘടന connectdata *conn,
                                   ഘടന Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  ഘടന cf_ip_happy_ctx *ctx = ശൂന്യ;
  CURLcode result;

  (ശൂന്യം)data;
  (ശൂന്യം)conn;
  *pcf = ശൂന്യ;
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  എങ്കിൽ(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  തിരികെ result;
}

CURLcode Curl_cf_ip_happy_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                       ഘടന Curl_easy *data,
                                       ഘടന Curl_peer *origin,
                                       ഘടന Curl_peer *peer,
                                       uint8_t transport_peer,
                                       ഘടന Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  result = cf_ip_happy_create(&cf, data, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  എങ്കിൽ(result)
    തിരികെ result;

  Curl_conn_cf_തിരുകുക_after(cf_at, cf);
  തിരികെ CURLE_OK;
}
