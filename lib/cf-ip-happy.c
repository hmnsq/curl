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


struct transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

static
#ifndef UNITTESTS
const
#endif
struct transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#nếu !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#nếu !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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
  cho(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    nếu((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      trả_về transport_providers[i].cf_create;
  }
  trả_về NULL;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST void debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST void debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  cho(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    nếu(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

struct cf_ai_iter {
  struct Curl_cfilter *cf;
  struct Curl_peer *peer;
  số_nguyên ai_family;
  unsigned số_nguyên n;
};

static void cf_ai_iter_init(struct cf_ai_iter *iter,
                            struct Curl_cfilter *cf,
                            struct Curl_peer *peer,
                            số_nguyên ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

static const struct Curl_addrinfo *cf_ai_iter_next(struct cf_ai_iter *iter,
                                                   struct Curl_easy *data)
{
  const struct Curl_addrinfo *addr;

  nếu(!iter->cf)
    trả_về NULL;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  nếu(addr)
    iter->n++;
  trả_về addr;
}

static bool cf_ai_iter_has_more(struct cf_ai_iter *iter,
                                struct Curl_easy *data)
{
  trả_về (iter->cf &&
          !!Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

struct cf_ip_attempt {
  struct cf_ip_attempt *next;
  struct Curl_peer *origin;
  struct Curl_peer *peer;
  struct Curl_peer *tunnel_peer;
  struct Curl_sockaddr_ex addr;
  struct Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode result;
  số_nguyên ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  số_nguyên error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

static void cf_ip_attempt_free(struct cf_ip_attempt *a,
                               struct Curl_easy *data)
{
  nếu(a) {
    nếu(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, data);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

static CURLcode cf_ip_attempt_new(struct cf_ip_attempt **pa,
                                  struct Curl_easy *data,
                                  struct Curl_cfilter *cf,
                                  struct Curl_peer *origin,
                                  struct Curl_peer *peer,
                                  uint8_t transport_peer,
                                  struct Curl_sockaddr_ex *addr,
                                  số_nguyên ai_family,
                                  struct Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  struct Curl_cfilter *wcf;
  struct cf_ip_attempt *a;
  CURLcode result = CURLE_OK;

  *pa = NULL;
  a = curlx_calloc(1, sizeof(*a));
  nếu(!a)
    trả_về CURLE_OUT_OF_MEMORY;

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
  nếu(result)
    goto out;

  /* the new filter might have sub-filters */
  cho(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  nếu(result) {
    cf_ip_attempt_free(a, data);
    *pa = NULL;
  }
  trả_về result;
}

static CURLcode cf_ip_attempt_connect(struct cf_ip_attempt *a,
                                      struct Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  nếu(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    nếu(!a->result) {
      nếu(*connected) {
        a->connected = TRUE;
      }
    }
    không_thì {
      nếu(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      nếu(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  trả_về a->result;
}

struct cf_ip_ballers {
  struct cf_ip_attempt *running;
  struct cf_ip_attempt **running_tail;  /* &running or &(last->next) */
  struct cf_ip_attempt *winner;
  struct cf_ai_iter addr_iter;
#ifdef USE_IPV6
  struct cf_ai_iter ipv6_iter;
#endif
  struct Curl_peer *origin;
  struct Curl_peer *peer;
  struct Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  struct curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_result;  /* result of last discarded hard failure */
  số_nguyên last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

static CURLcode cf_ip_attempt_restart(struct cf_ip_attempt *a,
                                      struct Curl_cfilter *cf,
                                      struct Curl_easy *data)
{
  struct Curl_cfilter *wcf;
  CURLcode result;

  nếu(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = NULL;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  nếu(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    cho(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  trả_về result;
}

static void cf_ip_ballers_clear(struct Curl_easy *data,
                                struct cf_ip_ballers *bs)
{
  trong_khi(bs->running) {
    struct cf_ip_attempt *a = bs->running;
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

static CURLcode cf_ip_ballers_init(struct cf_ip_ballers *bs,
                                   struct Curl_easy *data,
                                   struct Curl_peer *origin,
                                   struct Curl_peer *peer,
                                   uint8_t transport_peer,
                                   struct Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, sizeof(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  nếu(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    trả_về CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  trả_về CURLE_OK;
}

static void cf_ip_ballers_prune(struct cf_ip_ballers *bs,
                                struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                uint32_t max_concurrent)
{
  struct cf_ip_attempt *a = NULL, **panchor;
  uint32_t ongoing = 0;

  cho(a = bs->running; a; a = a->next) {
    nếu(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  trong_khi(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    nếu(!a->result && !a->connected) {
      *panchor = a->next;
      nếu(!a->next)
        bs->running_tail = panchor;
      a->next = NULL;
      cf_ip_attempt_free(a, data);
      --ongoing;
      CURL_TRC_CF(data, cf, "discarding oldest attempt to keep limit");
    }
    không_thì {
      panchor = &a->next;
    }
  }
}

static CURLcode cf_ip_ballers_run(struct cf_ip_ballers *bs,
                                  struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode result = CURLE_OK;
  struct cf_ip_attempt *a = NULL, **panchor;
  bool do_more;
  timediff_t next_expire_ms;
  uint32_t inconclusive, ongoing;
  const struct curltime *pnow = NULL;
  VERBOSE(số_nguyên i);

  nếu(bs->winner)
    trả_về CURLE_OK;

evaluate:
  nếu(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, NULL, TIMER_STARTSINGLE));
    trả_về CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  cho(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    nếu(!a->result) {
      nếu(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = NULL;
        trong_khi(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        trả_về CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    không_thì nếu(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    không_thì {
      /* hard failure, discard right away */
      *panchor = a->next;
      nếu(!a->next)
        bs->running_tail = panchor;
      a->next = NULL;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  nếu(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  nếu(!ongoing) {
    do_more = TRUE;
  }
  không_thì {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    nếu(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    nếu(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  nếu(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    const struct Curl_addrinfo *ai = NULL;
    số_nguyên ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    nếu((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    nếu(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    nếu(ai) {  /* try another address */
      struct Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      nếu(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      nếu(result)
        goto out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (số_nguyên)result);
      nếu(result)
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
    không_thì nếu(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      nếu(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        cho(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          nếu(!a->inconclusive)
            continue;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (số_nguyên)result);
          nếu(result) /* serious failure */
            goto out;
          bs->last_attempt_started = *pnow;
          goto evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      không_thì {
        /* let's wait some more before restarting */
        infof(data, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      goto out;
    }
    không_thì nếu(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  nếu(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    nếu(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, NULL, TIMER_STARTSINGLE));
      trả_về CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    nếu(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    nếu(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      nếu(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        goto evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  trả_về result;
}

static CURLcode cf_ip_ballers_shutdown(struct cf_ip_ballers *bs,
                                       struct Curl_easy *data,
                                       bool *done)
{
  struct cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  cho(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    nếu(a->shutdown || !a->cf)
      continue;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    nếu(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    không_thì
      *done = FALSE;
  }
  trả_về CURLE_OK;
}

static CURLcode cf_ip_ballers_pollset(struct cf_ip_ballers *bs,
                                      struct Curl_easy *data,
                                      struct easy_pollset *ps)
{
  struct cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  cho(a = bs->running; a && !result; a = a->next) {
    nếu(a->result)
      continue;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  trả_về result;
}

static bool cf_ip_ballers_pending(struct cf_ip_ballers *bs,
                                  const struct Curl_easy *data)
{
  struct cf_ip_attempt *a;

  cho(a = bs->running; a; a = a->next) {
    nếu(a->result)
      continue;
    nếu(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      trả_về TRUE;
  }
  trả_về FALSE;
}

static số_nguyên cf_ip_ballers_min_reply_ms(struct cf_ip_ballers *bs,
                                      struct Curl_easy *data)
{
  số_nguyên reply_ms = -1, breply_ms;
  struct cf_ip_attempt *a;

  cho(a = bs->running; a; a = a->next) {
    nếu(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, NULL) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  trả_về reply_ms;
}

typedef enum {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

struct cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  struct cf_ip_ballers ballers;
  BIT(dns_resolved);
};

static CURLcode is_connected(struct Curl_cfilter *cf,
                             struct Curl_easy *data,
                             bool *connected)
{
  struct cf_ip_happy_ctx *ctx = cf->ctx;
  struct connectdata *conn = cf->conn;
  CURLcode result;

  result = cf_ip_ballers_run(&ctx->ballers, cf, data,
                             (bool)ctx->dns_resolved, connected);

  nếu(!result)
    trả_về CURLE_OK;
  không_thì {
    struct Curl_peer *peer = NULL, *proxy_peer = NULL;
    char viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    nếu(!conn->origin || !peer)
      trả_về CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    nếu(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    không_thì nếu(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    nếu(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      nếu(peer->unix_socket)
        curl_msnprintf(viamsg, sizeof(viamsg), " over unix://%s",
                       peer->hostname);
      không_thì
#endif
      curl_msnprintf(viamsg, sizeof(viamsg), " via %s:%u",
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
    nếu(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    trả_về result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

static CURLcode cf_ip_happy_init(struct Curl_cfilter *cf,
                                 struct Curl_easy *data)
{
  struct cf_ip_happy_ctx *ctx = cf->ctx;

  nếu(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    trả_về CURLE_OPERATION_TIMEDOUT;
  }

  nếu(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#không_thì
    trả_về CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  không_thì { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(data, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  trả_về CURLE_OK;
}

static void cf_ip_happy_ctx_clear(struct cf_ip_happy_ctx *ctx,
                                  struct Curl_easy *data)
{
  DEBUGASSERT(ctx);
  nếu(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

static void cf_ip_happy_ctx_destroy(struct cf_ip_happy_ctx *ctx,
                                    struct Curl_easy *data)
{
  nếu(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    curlx_free(ctx);
  }
}

static CURLcode cf_ip_happy_shutdown(struct Curl_cfilter *cf,
                                     struct Curl_easy *data,
                                     bool *done)
{
  struct cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  nếu(cf->connected) {
    *done = TRUE;
    trả_về CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (số_nguyên)result, *done);
  trả_về result;
}

static CURLcode cf_ip_happy_adjust_pollset(struct Curl_cfilter *cf,
                                           struct Curl_easy *data,
                                           struct easy_pollset *ps)
{
  struct cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  nếu(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (số_nguyên)result,
                ps->n);
  }
  trả_về result;
}

static CURLcode cf_ip_happy_connect(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    bool *done)
{
  struct cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  nếu(!data)
    trả_về CURLE_FAILED_INIT;

  nếu(cf->connected) {
    *done = TRUE;
    trả_về CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  nếu(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    nếu(!result)
      ctx->dns_resolved = TRUE;
    không_thì nếu(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    không_thì /* real error */
      goto out;
  }

  switch(ctx->state) {
  case SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    nếu(result)
      goto out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  case SCFST_WAITING:
    result = is_connected(cf, data, done);
    nếu(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = NULL;

      nếu(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      nếu(Curl_trc_cf_is_verbose(cf, data)) {
        struct ip_quadruple ipquad;
        bool is_ipv6;
        nếu(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
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
    break;
  case SCFST_DONE:
    *done = TRUE;
    break;
  }
out:
  trả_về result;
}

static bool cf_ip_happy_data_pending(struct Curl_cfilter *cf,
                                     const struct Curl_easy *data)
{
  struct cf_ip_happy_ctx *ctx = cf->ctx;

  nếu(!cf->connected) {
    trả_về cf_ip_ballers_pending(&ctx->ballers, data);
  }
  trả_về cf->next->cft->has_data_pending(cf->next, data);
}

static CURLcode cf_ip_happy_query(struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  số_nguyên query, số_nguyên *pres1, void *pres2)
{
  struct cf_ip_happy_ctx *ctx = cf->ctx;

  nếu(!cf->connected) {
    switch(query) {
    case CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      trả_về CURLE_OK;
    }
    default:
      break;
    }
  }

  trả_về cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static void cf_ip_happy_destroy(struct Curl_cfilter *cf,
                                struct Curl_easy *data)
{
  struct cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  nếu(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
}

struct Curl_cftype Curl_cft_ip_happy = {
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

static CURLcode cf_ip_happy_create(struct Curl_cfilter **pcf,
                                   struct Curl_easy *data,
                                   struct Curl_peer *origin,
                                   struct Curl_peer *peer,
                                   uint8_t transport_peer,
                                   struct connectdata *conn,
                                   struct Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  struct cf_ip_happy_ctx *ctx = NULL;
  CURLcode result;

  (void)data;
  (void)conn;
  *pcf = NULL;
  ctx = curlx_calloc(1, sizeof(*ctx));
  nếu(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  nếu(result)
    goto out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  nếu(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  trả_về result;
}

CURLcode Curl_cf_ip_happy_insert_after(struct Curl_cfilter *cf_at,
                                       struct Curl_easy *data,
                                       struct Curl_peer *origin,
                                       struct Curl_peer *peer,
                                       uint8_t transport_peer,
                                       struct Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  struct Curl_cfilter *cf;
  CURLcode result;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  result = cf_ip_happy_create(&cf, data, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  nếu(result)
    trả_về result;

  Curl_conn_cf_insert_after(cf_at, cf);
  trả_về CURLE_OK;
}
