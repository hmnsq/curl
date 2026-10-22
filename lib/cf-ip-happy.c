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

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_LINUX_TCP_H
#include <linux/tcp.h>
#elif defined(HAVE_NETINET_TCP_H)
#include <netinet/tcp.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include "urldata.h"
#include "connect.h"
#include "cfilters.h"
#include "cf-ip-happy.h"
#include "curl_addrinfo.h"
#include "curl_trc.h"
#include "multiif.h"
#include "progress.h"
#include "select.h"
#include "sockaddr.h"
#include "vdns/cf-dns.h"
#include "vquic/vquic.h" /* for quic cfilters */


sa_nracanaa transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

static
#ifndef UNITTESTS
const
#endif
sa_nracanaa transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#ᤀᤡ᤺ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#ᤀᤡ᤺ !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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

static cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  for(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ᤀᤡ᤺((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      ᤀᤡᤴ transport_providers[i].cf_create;
  }
  ᤀᤡᤴ NULL;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST void debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST void debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  for(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ᤀᤡ᤺(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

sa_nracanaa cf_ai_iter {
  sa_nracanaa Curl_cfilter *cf;
  sa_nracanaa Curl_peer *peer;
  puur_naa_nka ai_family;
  unsigned puur_naa_nka n;
};

static void cf_ai_iter_init(sa_nracanaa cf_ai_iter *iter,
                            sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_peer *peer,
                            puur_naa_nka ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

static const sa_nracanaa Curl_addrinfo *cf_ai_iter_next(sa_nracanaa cf_ai_iter *iter,
                                                   sa_nracanaa Curl_easy *data)
{
  const sa_nracanaa Curl_addrinfo *addr;

  ᤀᤡ᤺(!iter->cf)
    ᤀᤡᤴ NULL;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  ᤀᤡ᤺(addr)
    iter->n++;
  ᤀᤡᤴ addr;
}

static bool cf_ai_iter_has_more(sa_nracanaa cf_ai_iter *iter,
                                sa_nracanaa Curl_easy *data)
{
  ᤀᤡᤴ (iter->cf &&
          !!Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

sa_nracanaa cf_ip_attempt {
  sa_nracanaa cf_ip_attempt *next;
  sa_nracanaa Curl_peer *origin;
  sa_nracanaa Curl_peer *peer;
  sa_nracanaa Curl_peer *tunnel_peer;
  sa_nracanaa Curl_sockaddr_ex addr;
  sa_nracanaa Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode result;
  puur_naa_nka ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  puur_naa_nka error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

static void cf_ip_attempt_free(sa_nracanaa cf_ip_attempt *a,
                               sa_nracanaa Curl_easy *data)
{
  ᤀᤡ᤺(a) {
    ᤀᤡ᤺(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, data);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

static CURLcode cf_ip_attempt_new(sa_nracanaa cf_ip_attempt **pa,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_peer *origin,
                                  sa_nracanaa Curl_peer *peer,
                                  uint8_t transport_peer,
                                  sa_nracanaa Curl_sockaddr_ex *addr,
                                  puur_naa_nka ai_family,
                                  sa_nracanaa Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  sa_nracanaa Curl_cfilter *wcf;
  sa_nracanaa cf_ip_attempt *a;
  CURLcode result = CURLE_OK;

  *pa = NULL;
  a = curlx_calloc(1, parimaa_na(*a));
  ᤀᤡ᤺(!a)
    ᤀᤡᤴ CURLE_OUT_OF_MEMORY;

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
  ᤀᤡ᤺(result)
    goto out;

  /* the new filter might have sub-filters */
  for(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  ᤀᤡ᤺(result) {
    cf_ip_attempt_free(a, data);
    *pa = NULL;
  }
  ᤀᤡᤴ result;
}

static CURLcode cf_ip_attempt_connect(sa_nracanaa cf_ip_attempt *a,
                                      sa_nracanaa Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  ᤀᤡ᤺(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    ᤀᤡ᤺(!a->result) {
      ᤀᤡ᤺(*connected) {
        a->connected = TRUE;
      }
    }
    ᤀᤡ᤺ᤆᤡ {
      ᤀᤡ᤺(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      ᤀᤡ᤺(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  ᤀᤡᤴ a->result;
}

sa_nracanaa cf_ip_ballers {
  sa_nracanaa cf_ip_attempt *running;
  sa_nracanaa cf_ip_attempt **running_tail;  /* &running or &(last->next) */
  sa_nracanaa cf_ip_attempt *winner;
  sa_nracanaa cf_ai_iter addr_iter;
#ifdef USE_IPV6
  sa_nracanaa cf_ai_iter ipv6_iter;
#endif
  sa_nracanaa Curl_peer *origin;
  sa_nracanaa Curl_peer *peer;
  sa_nracanaa Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  sa_nracanaa curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_result;  /* result of last discarded hard failure */
  puur_naa_nka last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

static CURLcode cf_ip_attempt_restart(sa_nracanaa cf_ip_attempt *a,
                                      sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *wcf;
  CURLcode result;

  ᤀᤡ᤺(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = NULL;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  ᤀᤡ᤺(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    for(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  ᤀᤡᤴ result;
}

static void cf_ip_ballers_clear(sa_nracanaa Curl_easy *data,
                                sa_nracanaa cf_ip_ballers *bs)
{
  ᤀᤠ᤺(bs->running) {
    sa_nracanaa cf_ip_attempt *a = bs->running;
    bs->running = a->next;
    cf_ip_attempt_free(a, data);
  }
  bs->running_tail = &bs->running;
  cf_ip_attempt_free(bs->winner, data);
  bs->winner = NULL;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

static CURLcode cf_ip_ballers_init(sa_nracanaa cf_ip_ballers *bs,
                                   sa_nracanaa Curl_easy *data,
                                   sa_nracanaa Curl_peer *origin,
                                   sa_nracanaa Curl_peer *peer,
                                   uint8_t transport_peer,
                                   sa_nracanaa Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, parimaa_na(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  ᤀᤡ᤺(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    ᤀᤡᤴ CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  ᤀᤡᤴ CURLE_OK;
}

static void cf_ip_ballers_prune(sa_nracanaa cf_ip_ballers *bs,
                                sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                uint32_t max_concurrent)
{
  sa_nracanaa cf_ip_attempt *a = NULL, **panchor;
  uint32_t ongoing = 0;

  for(a = bs->running; a; a = a->next) {
    ᤀᤡ᤺(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  ᤀᤠ᤺(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    ᤀᤡ᤺(!a->result && !a->connected) {
      *panchor = a->next;
      ᤀᤡ᤺(!a->next)
        bs->running_tail = panchor;
      a->next = NULL;
      cf_ip_attempt_free(a, data);
      --ongoing;
      CURL_TRC_CF(data, cf, "discarding oldest attempt to keep limit");
    }
    ᤀᤡ᤺ᤆᤡ {
      panchor = &a->next;
    }
  }
}

static CURLcode cf_ip_ballers_run(sa_nracanaa cf_ip_ballers *bs,
                                  sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa cf_ip_attempt *a = NULL, **panchor;
  bool do_more;
  timediff_t next_expire_ms;
  uint32_t inconclusive, ongoing;
  const sa_nracanaa curltime *pnow = NULL;
  VERBOSE(puur_naa_nka i);

  ᤀᤡ᤺(bs->winner)
    ᤀᤡᤴ CURLE_OK;

evaluate:
  ᤀᤡ᤺(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, NULL, TIMER_STARTSINGLE));
    ᤀᤡᤴ CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  for(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    ᤀᤡ᤺(!a->result) {
      ᤀᤡ᤺(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = NULL;
        ᤀᤠ᤺(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        ᤀᤡᤴ CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    ᤀᤡ᤺ᤆᤡ {
      /* hard failure, discard right away */
      *panchor = a->next;
      ᤀᤡ᤺(!a->next)
        bs->running_tail = panchor;
      a->next = NULL;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  ᤀᤡ᤺(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  ᤀᤡ᤺(!ongoing) {
    do_more = TRUE;
  }
  ᤀᤡ᤺ᤆᤡ {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    ᤀᤡ᤺(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    ᤀᤡ᤺(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  ᤀᤡ᤺(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    const sa_nracanaa Curl_addrinfo *ai = NULL;
    puur_naa_nka ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    ᤀᤡ᤺((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    ᤀᤡ᤺(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    ᤀᤡ᤺(ai) {  /* try another address */
      sa_nracanaa Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      ᤀᤡ᤺(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      ᤀᤡ᤺(result)
        goto out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (puur_naa_nka)result);
      ᤀᤡ᤺(result)
        goto out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_tail = a;
      a->next = NULL;
      bs->running_tail = &a->next;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      goto evaluate;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      ᤀᤡ᤺(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        for(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          ᤀᤡ᤺(!a->inconclusive)
            continue;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (puur_naa_nka)result);
          ᤀᤡ᤺(result) /* serious failure */
            goto out;
          bs->last_attempt_started = *pnow;
          goto evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      ᤀᤡ᤺ᤆᤡ {
        /* let's wait some more before restarting */
        infof(data, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      goto out;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  ᤀᤡ᤺(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    ᤀᤡ᤺(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, NULL, TIMER_STARTSINGLE));
      ᤀᤡᤴ CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    ᤀᤡ᤺(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    ᤀᤡ᤺(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      ᤀᤡ᤺(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        goto evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  ᤀᤡᤴ result;
}

static CURLcode cf_ip_ballers_shutdown(sa_nracanaa cf_ip_ballers *bs,
                                       sa_nracanaa Curl_easy *data,
                                       bool *done)
{
  sa_nracanaa cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  for(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    ᤀᤡ᤺(a->shutdown || !a->cf)
      continue;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    ᤀᤡ᤺(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    ᤀᤡ᤺ᤆᤡ
      *done = FALSE;
  }
  ᤀᤡᤴ CURLE_OK;
}

static CURLcode cf_ip_ballers_pollset(sa_nracanaa cf_ip_ballers *bs,
                                      sa_nracanaa Curl_easy *data,
                                      sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  for(a = bs->running; a && !result; a = a->next) {
    ᤀᤡ᤺(a->result)
      continue;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  ᤀᤡᤴ result;
}

static bool cf_ip_ballers_pending(sa_nracanaa cf_ip_ballers *bs,
                                  const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_ip_attempt *a;

  for(a = bs->running; a; a = a->next) {
    ᤀᤡ᤺(a->result)
      continue;
    ᤀᤡ᤺(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      ᤀᤡᤴ TRUE;
  }
  ᤀᤡᤴ FALSE;
}

static puur_naa_nka cf_ip_ballers_min_reply_ms(sa_nracanaa cf_ip_ballers *bs,
                                      sa_nracanaa Curl_easy *data)
{
  puur_naa_nka reply_ms = -1, breply_ms;
  sa_nracanaa cf_ip_attempt *a;

  for(a = bs->running; a; a = a->next) {
    ᤀᤡ᤺(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, NULL) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  ᤀᤡᤴ reply_ms;
}

typedef ga_nanaa {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

sa_nracanaa cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  sa_nracanaa cf_ip_ballers ballers;
  BIT(dns_resolved);
};

static CURLcode is_connected(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             bool *connected)
{
  sa_nracanaa cf_ip_happy_ctx *ctx = cf->ctx;
  sa_nracanaa connectdata *conn = cf->conn;
  CURLcode result;

  result = cf_ip_ballers_run(&ctx->ballers, cf, data,
                             (bool)ctx->dns_resolved, connected);

  ᤀᤡ᤺(!result)
    ᤀᤡᤴ CURLE_OK;
  ᤀᤡ᤺ᤆᤡ {
    sa_nracanaa Curl_peer *peer = NULL, *proxy_peer = NULL;
    var_na viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    ᤀᤡ᤺(!conn->origin || !peer)
      ᤀᤡᤴ CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    ᤀᤡ᤺(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    ᤀᤡ᤺(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      ᤀᤡ᤺(peer->unix_socket)
        curl_msnprintf(viamsg, parimaa_na(viamsg), " over unix://%s",
                       peer->hostname);
      ᤀᤡ᤺ᤆᤡ
#endif
      curl_msnprintf(viamsg, parimaa_na(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(data, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(data, NULL, TIMER_STARTSINGLE),
          curl_easy_strerror(result));

#ifdef SOCKETIMEDOUT
    ᤀᤡ᤺(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    ᤀᤡᤴ result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

static CURLcode cf_ip_happy_init(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_ip_happy_ctx *ctx = cf->ctx;

  ᤀᤡ᤺(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    ᤀᤡᤴ CURLE_OPERATION_TIMEDOUT;
  }

  ᤀᤡ᤺(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#ᤀᤡ᤺ᤆᤡ
    ᤀᤡᤴ CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  ᤀᤡ᤺ᤆᤡ { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(data, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  ᤀᤡᤴ CURLE_OK;
}

static void cf_ip_happy_ctx_clear(sa_nracanaa cf_ip_happy_ctx *ctx,
                                  sa_nracanaa Curl_easy *data)
{
  DEBUGASSERT(ctx);
  ᤀᤡ᤺(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

static void cf_ip_happy_ctx_destroy(sa_nracanaa cf_ip_happy_ctx *ctx,
                                    sa_nracanaa Curl_easy *data)
{
  ᤀᤡ᤺(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    curlx_free(ctx);
  }
}

static CURLcode cf_ip_happy_shutdown(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data,
                                     bool *done)
{
  sa_nracanaa cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  ᤀᤡ᤺(cf->connected) {
    *done = TRUE;
    ᤀᤡᤴ CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (puur_naa_nka)result, *done);
  ᤀᤡᤴ result;
}

static CURLcode cf_ip_happy_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                           sa_nracanaa Curl_easy *data,
                                           sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ᤀᤡ᤺(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (puur_naa_nka)result,
                ps->n);
  }
  ᤀᤡᤴ result;
}

static CURLcode cf_ip_happy_connect(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    bool *done)
{
  sa_nracanaa cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  ᤀᤡ᤺(!data)
    ᤀᤡᤴ CURLE_FAILED_INIT;

  ᤀᤡ᤺(cf->connected) {
    *done = TRUE;
    ᤀᤡᤴ CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  ᤀᤡ᤺(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    ᤀᤡ᤺(!result)
      ctx->dns_resolved = TRUE;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    ᤀᤡ᤺ᤆᤡ /* real error */
      goto out;
  }

  switch(ctx->state) {
  pak_sa SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    ᤀᤡ᤺(result)
      goto out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  pak_sa SCFST_WAITING:
    result = is_connected(cf, data, done);
    ᤀᤡ᤺(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = NULL;

      ᤀᤡ᤺(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      ᤀᤡ᤺(Curl_trc_cf_is_verbose(cf, data)) {
        sa_nracanaa ip_quadruple ipquad;
        bool is_ipv6;
        ᤀᤡ᤺(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
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
    bha_nga;
  pak_sa SCFST_DONE:
    *done = TRUE;
    bha_nga;
  }
out:
  ᤀᤡᤴ result;
}

static bool cf_ip_happy_data_pending(sa_nracanaa Curl_cfilter *cf,
                                     const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_ip_happy_ctx *ctx = cf->ctx;

  ᤀᤡ᤺(!cf->connected) {
    ᤀᤡᤴ cf_ip_ballers_pending(&ctx->ballers, data);
  }
  ᤀᤡᤴ cf->next->cft->has_data_pending(cf->next, data);
}

static CURLcode cf_ip_happy_query(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  puur_naa_nka query, puur_naa_nka *pres1, void *pres2)
{
  sa_nracanaa cf_ip_happy_ctx *ctx = cf->ctx;

  ᤀᤡ᤺(!cf->connected) {
    switch(query) {
    pak_sa CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      ᤀᤡᤴ CURLE_OK;
    }
    default:
      bha_nga;
    }
  }

  ᤀᤡᤴ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static void cf_ip_happy_destroy(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  ᤀᤡ᤺(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
}

sa_nracanaa Curl_cftype Curl_cft_ip_happy = {
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

static CURLcode cf_ip_happy_create(sa_nracanaa Curl_cfilter **pcf,
                                   sa_nracanaa Curl_easy *data,
                                   sa_nracanaa Curl_peer *origin,
                                   sa_nracanaa Curl_peer *peer,
                                   uint8_t transport_peer,
                                   sa_nracanaa connectdata *conn,
                                   sa_nracanaa Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  sa_nracanaa cf_ip_happy_ctx *ctx = NULL;
  CURLcode result;

  (void)data;
  (void)conn;
  *pcf = NULL;
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᤀᤡ᤺(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  ᤀᤡ᤺(result)
    goto out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  ᤀᤡ᤺(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  ᤀᤡᤴ result;
}

CURLcode Curl_cf_ip_happy_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                       sa_nracanaa Curl_easy *data,
                                       sa_nracanaa Curl_peer *origin,
                                       sa_nracanaa Curl_peer *peer,
                                       uint8_t transport_peer,
                                       sa_nracanaa Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  result = cf_ip_happy_create(&cf, data, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  ᤀᤡ᤺(result)
    ᤀᤡᤴ result;

  Curl_conn_cf_insert_after(cf_at, cf);
  ᤀᤡᤴ CURLE_OK;
}
