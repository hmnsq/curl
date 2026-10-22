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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_LINUX_TCP_H
#ಅನ್ತರ್ಭಾವ <linux/tcp.h>
#ನೋಚೇದ್ಯದಿ defined(HAVE_NETINET_TCP_H)
#ಅನ್ತರ್ಭಾವ <netinet/tcp.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif

#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-ip-happy.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "sockaddr.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h" /* for quic cfilters */


ಸಂರಚನಾ transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

ಸ್ಥಿರ
#ifndef UNITTESTS
ಧ್ರುವ
#endif
ಸಂರಚನಾ transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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

ಸ್ಥಿರ cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ಯದಿ((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      ಪ್ರತ್ಯಾಗಮ transport_providers[i].cf_create;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST ಶೂನ್ಯ debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST ಶೂನ್ಯ debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ಯದಿ(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

ಸಂರಚನಾ cf_ai_iter {
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ Curl_peer *peer;
  ಪೂರ್ಣಾಙ್ಕ ai_family;
  unsigned ಪೂರ್ಣಾಙ್ಕ n;
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_ai_iter_init(ಸಂರಚನಾ cf_ai_iter *iter,
                            ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_peer *peer,
                            ಪೂರ್ಣಾಙ್ಕ ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *cf_ai_iter_next(ಸಂರಚನಾ cf_ai_iter *iter,
                                                   ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *addr;

  ಯದಿ(!iter->cf)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  ಯದಿ(addr)
    iter->n++;
  ಪ್ರತ್ಯಾಗಮ addr;
}

ಸ್ಥಿರ bool cf_ai_iter_has_more(ಸಂರಚನಾ cf_ai_iter *iter,
                                ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ (iter->cf &&
          !!Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

ಸಂರಚನಾ cf_ip_attempt {
  ಸಂರಚನಾ cf_ip_attempt *next;
  ಸಂರಚನಾ Curl_peer *origin;
  ಸಂರಚನಾ Curl_peer *peer;
  ಸಂರಚನಾ Curl_peer *tunnel_peer;
  ಸಂರಚನಾ Curl_sockaddr_ex addr;
  ಸಂರಚನಾ Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  ಪೂರ್ಣಾಙ್ಕ error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_attempt_free(ಸಂರಚನಾ cf_ip_attempt *a,
                               ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(a) {
    ಯದಿ(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, data);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

ಸ್ಥಿರ CURLcode cf_ip_attempt_new(ಸಂರಚನಾ cf_ip_attempt **pa,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_peer *origin,
                                  ಸಂರಚನಾ Curl_peer *peer,
                                  uint8_t transport_peer,
                                  ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                                  ಪೂರ್ಣಾಙ್ಕ ai_family,
                                  ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  ಸಂರಚನಾ Curl_cfilter *wcf;
  ಸಂರಚನಾ cf_ip_attempt *a;
  CURLcode result = CURLE_OK;

  *pa = ರಿಕ್ತ;
  a = curlx_calloc(1, ಪರಿಮಾಣ(*a));
  ಯದಿ(!a)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

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
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* the new filter might have sub-filters */
  ಪ್ರತಿ(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  ಯದಿ(result) {
    cf_ip_attempt_free(a, data);
    *pa = ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_ip_attempt_connect(ಸಂರಚನಾ cf_ip_attempt *a,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  ಯದಿ(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    ಯದಿ(!a->result) {
      ಯದಿ(*connected) {
        a->connected = TRUE;
      }
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      ಯದಿ(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  ಪ್ರತ್ಯಾಗಮ a->result;
}

ಸಂರಚನಾ cf_ip_ballers {
  ಸಂರಚನಾ cf_ip_attempt *running;
  ಸಂರಚನಾ cf_ip_attempt **running_tail;  /* &running or &(last->next) */
  ಸಂರಚನಾ cf_ip_attempt *winner;
  ಸಂರಚನಾ cf_ai_iter addr_iter;
#ifdef USE_IPV6
  ಸಂರಚನಾ cf_ai_iter ipv6_iter;
#endif
  ಸಂರಚನಾ Curl_peer *origin;
  ಸಂರಚನಾ Curl_peer *peer;
  ಸಂರಚನಾ Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  ಸಂರಚನಾ curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_result;  /* result of last discarded hard failure */
  ಪೂರ್ಣಾಙ್ಕ last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

ಸ್ಥಿರ CURLcode cf_ip_attempt_restart(ಸಂರಚನಾ cf_ip_attempt *a,
                                      ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cfilter *wcf;
  CURLcode result;

  ಯದಿ(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = ರಿಕ್ತ;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  ಯದಿ(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    ಪ್ರತಿ(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_ballers_clear(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ cf_ip_ballers *bs)
{
  ಯಾವತ್(bs->running) {
    ಸಂರಚನಾ cf_ip_attempt *a = bs->running;
    bs->running = a->next;
    cf_ip_attempt_free(a, data);
  }
  bs->running_tail = &bs->running;
  cf_ip_attempt_free(bs->winner, data);
  bs->winner = ರಿಕ್ತ;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

ಸ್ಥಿರ CURLcode cf_ip_ballers_init(ಸಂರಚನಾ cf_ip_ballers *bs,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ Curl_peer *origin,
                                   ಸಂರಚನಾ Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, ಪರಿಮಾಣ(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  ಯದಿ(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_ballers_prune(ಸಂರಚನಾ cf_ip_ballers *bs,
                                ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                uint32_t max_concurrent)
{
  ಸಂರಚನಾ cf_ip_attempt *a = ರಿಕ್ತ, **panchor;
  uint32_t ongoing = 0;

  ಪ್ರತಿ(a = bs->running; a; a = a->next) {
    ಯದಿ(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  ಯಾವತ್(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    ಯದಿ(!a->result && !a->connected) {
      *panchor = a->next;
      ಯದಿ(!a->next)
        bs->running_tail = panchor;
      a->next = ರಿಕ್ತ;
      cf_ip_attempt_free(a, data);
      --ongoing;
      CURL_TRC_CF(data, cf, "discarding oldest attempt to keep limit");
    }
    ಅನ್ಯಥಾ {
      panchor = &a->next;
    }
  }
}

ಸ್ಥಿರ CURLcode cf_ip_ballers_run(ಸಂರಚನಾ cf_ip_ballers *bs,
                                  ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ cf_ip_attempt *a = ರಿಕ್ತ, **panchor;
  bool do_more;
  timediff_t next_expire_ms;
  uint32_t inconclusive, ongoing;
  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow = ರಿಕ್ತ;
  VERBOSE(ಪೂರ್ಣಾಙ್ಕ i);

  ಯದಿ(bs->winner)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

evaluate:
  ಯದಿ(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, ರಿಕ್ತ, TIMER_STARTSINGLE));
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  ಪ್ರತಿ(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    ಯದಿ(!a->result) {
      ಯದಿ(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = ರಿಕ್ತ;
        ಯಾವತ್(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    ಅನ್ಯಥಾ ಯದಿ(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    ಅನ್ಯಥಾ {
      /* hard failure, discard right away */
      *panchor = a->next;
      ಯದಿ(!a->next)
        bs->running_tail = panchor;
      a->next = ರಿಕ್ತ;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  ಯದಿ(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  ಯದಿ(!ongoing) {
    do_more = TRUE;
  }
  ಅನ್ಯಥಾ {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    ಯದಿ(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    ಯದಿ(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  ಯದಿ(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    ಯದಿ((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    ಯದಿ(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    ಯದಿ(ai) {  /* try another address */
      ಸಂರಚನಾ Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      ಯದಿ(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      ಯದಿ(result)
        ಗಚ್ಛ out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (ಪೂರ್ಣಾಙ್ಕ)result);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_tail = a;
      a->next = ರಿಕ್ತ;
      bs->running_tail = &a->next;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      ಗಚ್ಛ evaluate;
    }
    ಅನ್ಯಥಾ ಯದಿ(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      ಯದಿ(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        ಪ್ರತಿ(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          ಯದಿ(!a->inconclusive)
            ಅನುವರ್ತ;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (ಪೂರ್ಣಾಙ್ಕ)result);
          ಯದಿ(result) /* serious failure */
            ಗಚ್ಛ out;
          bs->last_attempt_started = *pnow;
          ಗಚ್ಛ evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      ಅನ್ಯಥಾ {
        /* let's wait some more before restarting */
        infof(data, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  ಯದಿ(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    ಯದಿ(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, ರಿಕ್ತ, TIMER_STARTSINGLE));
      ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    ಯದಿ(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    ಯದಿ(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      ಯದಿ(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        ಗಚ್ಛ evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_ip_ballers_shutdown(ಸಂರಚನಾ cf_ip_ballers *bs,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       bool *done)
{
  ಸಂರಚನಾ cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  ಪ್ರತಿ(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    ಯದಿ(a->shutdown || !a->cf)
      ಅನುವರ್ತ;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    ಯದಿ(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    ಅನ್ಯಥಾ
      *done = FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_ip_ballers_pollset(ಸಂರಚನಾ cf_ip_ballers *bs,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  ಪ್ರತಿ(a = bs->running; a && !result; a = a->next) {
    ಯದಿ(a->result)
      ಅನುವರ್ತ;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool cf_ip_ballers_pending(ಸಂರಚನಾ cf_ip_ballers *bs,
                                  ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_ip_attempt *a;

  ಪ್ರತಿ(a = bs->running; a; a = a->next) {
    ಯದಿ(a->result)
      ಅನುವರ್ತ;
    ಯದಿ(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cf_ip_ballers_min_reply_ms(ಸಂರಚನಾ cf_ip_ballers *bs,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  ಪೂರ್ಣಾಙ್ಕ reply_ms = -1, breply_ms;
  ಸಂರಚನಾ cf_ip_attempt *a;

  ಪ್ರತಿ(a = bs->running; a; a = a->next) {
    ಯದಿ(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, ರಿಕ್ತ) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  ಪ್ರತ್ಯಾಗಮ reply_ms;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

ಸಂರಚನಾ cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  ಸಂರಚನಾ cf_ip_ballers ballers;
  BIT(dns_resolved);
};

ಸ್ಥಿರ CURLcode is_connected(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             bool *connected)
{
  ಸಂರಚನಾ cf_ip_happy_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ connectdata *conn = cf->conn;
  CURLcode result;

  result = cf_ip_ballers_run(&ctx->ballers, cf, data,
                             (bool)ctx->dns_resolved, connected);

  ಯದಿ(!result)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಅನ್ಯಥಾ {
    ಸಂರಚನಾ Curl_peer *peer = ರಿಕ್ತ, *proxy_peer = ರಿಕ್ತ;
    ವರ್ಣ viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    ಯದಿ(!conn->origin || !peer)
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    ಯದಿ(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    ಅನ್ಯಥಾ ಯದಿ(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    ಯದಿ(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      ಯದಿ(peer->unix_socket)
        curl_msnprintf(viamsg, ಪರಿಮಾಣ(viamsg), " over unix://%s",
                       peer->hostname);
      ಅನ್ಯಥಾ
#endif
      curl_msnprintf(viamsg, ಪರಿಮಾಣ(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(data, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(data, ರಿಕ್ತ, TIMER_STARTSINGLE),
          curl_easy_strerror(result));

#ifdef SOCKETIMEDOUT
    ಯದಿ(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    ಪ್ರತ್ಯಾಗಮ result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

ಸ್ಥಿರ CURLcode cf_ip_happy_init(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_ip_happy_ctx *ctx = cf->ctx;

  ಯದಿ(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
  }

  ಯದಿ(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  ಅನ್ಯಥಾ { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(data, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_happy_ctx_clear(ಸಂರಚನಾ cf_ip_happy_ctx *ctx,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  DEBUGASSERT(ctx);
  ಯದಿ(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_happy_ctx_destroy(ಸಂರಚನಾ cf_ip_happy_ctx *ctx,
                                    ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_ip_happy_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data,
                                     bool *done)
{
  ಸಂರಚನಾ cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (ಪೂರ್ಣಾಙ್ಕ)result, *done);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_ip_happy_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                           ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (ಪೂರ್ಣಾಙ್ಕ)result,
                ps->n);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_ip_happy_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    bool *done)
{
  ಸಂರಚನಾ cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  ಯದಿ(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    ಯದಿ(!result)
      ctx->dns_resolved = TRUE;
    ಅನ್ಯಥಾ ಯದಿ(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    ಅನ್ಯಥಾ /* real error */
      ಗಚ್ಛ out;
  }

  ವಿಕಲ್ಪ(ctx->state) {
  ಪಕ್ಷ SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  ಪಕ್ಷ SCFST_WAITING:
    result = is_connected(cf, data, done);
    ಯದಿ(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = ರಿಕ್ತ;

      ಯದಿ(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      ಯದಿ(Curl_trc_cf_is_verbose(cf, data)) {
        ಸಂರಚನಾ ip_quadruple ipquad;
        bool is_ipv6;
        ಯದಿ(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
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
    ಭಙ್ಗ;
  ಪಕ್ಷ SCFST_DONE:
    *done = TRUE;
    ಭಙ್ಗ;
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool cf_ip_happy_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_ip_happy_ctx *ctx = cf->ctx;

  ಯದಿ(!cf->connected) {
    ಪ್ರತ್ಯಾಗಮ cf_ip_ballers_pending(&ctx->ballers, data);
  }
  ಪ್ರತ್ಯಾಗಮ cf->next->cft->has_data_pending(cf->next, data);
}

ಸ್ಥಿರ CURLcode cf_ip_happy_query(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ cf_ip_happy_ctx *ctx = cf->ctx;

  ಯದಿ(!cf->connected) {
    ವಿಕಲ್ಪ(query) {
    ಪಕ್ಷ CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
  }

  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_happy_destroy(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  ಯದಿ(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
}

ಸಂರಚನಾ Curl_cftype Curl_cft_ip_happy = {
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

ಸ್ಥಿರ CURLcode cf_ip_happy_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ Curl_peer *origin,
                                   ಸಂರಚನಾ Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ಸಂರಚನಾ connectdata *conn,
                                   ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  ಸಂರಚನಾ cf_ip_happy_ctx *ctx = ರಿಕ್ತ;
  CURLcode result;

  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)conn;
  *pcf = ರಿಕ್ತ;
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  ಯದಿ(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_ip_happy_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ Curl_peer *origin,
                                       ಸಂರಚನಾ Curl_peer *peer,
                                       uint8_t transport_peer,
                                       ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  result = cf_ip_happy_create(&cf, data, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
