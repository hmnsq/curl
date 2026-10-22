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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_LINUX_TCP_H
#ଅନ୍ତର୍ଭାଵ <linux/tcp.h>
#ନଚେତଯଦି defined(HAVE_NETINET_TCP_H)
#ଅନ୍ତର୍ଭାଵ <netinet/tcp.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif

#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-ip-happy.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "sockaddr.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h" /* for quic cfilters */


ସଂରଚନା transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

ସ୍ଥିର
#ifndef UNITTESTS
ଅଚଳ
#endif
ସଂରଚନା transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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

ସ୍ଥିର cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ଯଦି((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      ଫେରନ୍ତୁ transport_providers[i].cf_create;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST ଶୂନ୍ୟ debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST ଶୂନ୍ୟ debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ଯଦି(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

ସଂରଚନା cf_ai_iter {
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା Curl_peer *peer;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
};

ସ୍ଥିର ଶୂନ୍ୟ cf_ai_iter_init(ସଂରଚନା cf_ai_iter *iter,
                            ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_peer *peer,
                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_addrinfo *cf_ai_iter_next(ସଂରଚନା cf_ai_iter *iter,
                                                   ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ ସଂରଚନା Curl_addrinfo *addr;

  ଯଦି(!iter->cf)
    ଫେରନ୍ତୁ ଖାଲି;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  ଯଦି(addr)
    iter->n++;
  ଫେରନ୍ତୁ addr;
}

ସ୍ଥିର bool cf_ai_iter_has_more(ସଂରଚନା cf_ai_iter *iter,
                                ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ (iter->cf &&
          !!Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

ସଂରଚନା cf_ip_attempt {
  ସଂରଚନା cf_ip_attempt *next;
  ସଂରଚନା Curl_peer *origin;
  ସଂରଚନା Curl_peer *peer;
  ସଂରଚନା Curl_peer *tunnel_peer;
  ସଂରଚନା Curl_sockaddr_ex addr;
  ସଂରଚନା Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

ସ୍ଥିର ଶୂନ୍ୟ cf_ip_attempt_free(ସଂରଚନା cf_ip_attempt *a,
                               ସଂରଚନା Curl_easy *data)
{
  ଯଦି(a) {
    ଯଦି(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, data);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

ସ୍ଥିର CURLcode cf_ip_attempt_new(ସଂରଚନା cf_ip_attempt **pa,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_peer *origin,
                                  ସଂରଚନା Curl_peer *peer,
                                  uint8_t transport_peer,
                                  ସଂରଚନା Curl_sockaddr_ex *addr,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family,
                                  ସଂରଚନା Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  ସଂରଚନା Curl_cfilter *wcf;
  ସଂରଚନା cf_ip_attempt *a;
  CURLcode result = CURLE_OK;

  *pa = ଖାଲି;
  a = curlx_calloc(1, ଆକାର(*a));
  ଯଦି(!a)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

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
  ଯଦି(result)
    ଯାଅ out;

  /* the new filter might have sub-filters */
  ପାଇଁ(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  ଯଦି(result) {
    cf_ip_attempt_free(a, data);
    *pa = ଖାଲି;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_ip_attempt_connect(ସଂରଚନା cf_ip_attempt *a,
                                      ସଂରଚନା Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  ଯଦି(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    ଯଦି(!a->result) {
      ଯଦି(*connected) {
        a->connected = TRUE;
      }
    }
    ଅନ୍ୟଥା {
      ଯଦି(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      ଯଦି(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  ଫେରନ୍ତୁ a->result;
}

ସଂରଚନା cf_ip_ballers {
  ସଂରଚନା cf_ip_attempt *running;
  ସଂରଚନା cf_ip_attempt **running_tail;  /* &running or &(last->next) */
  ସଂରଚନା cf_ip_attempt *winner;
  ସଂରଚନା cf_ai_iter addr_iter;
#ifdef USE_IPV6
  ସଂରଚନା cf_ai_iter ipv6_iter;
#endif
  ସଂରଚନା Curl_peer *origin;
  ସଂରଚନା Curl_peer *peer;
  ସଂରଚନା Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  ସଂରଚନା curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_result;  /* result of last discarded hard failure */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

ସ୍ଥିର CURLcode cf_ip_attempt_restart(ସଂରଚନା cf_ip_attempt *a,
                                      ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cfilter *wcf;
  CURLcode result;

  ଯଦି(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = ଖାଲି;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  ଯଦି(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    ପାଇଁ(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_ip_ballers_clear(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା cf_ip_ballers *bs)
{
  ଯେତେବେଳେ(bs->running) {
    ସଂରଚନା cf_ip_attempt *a = bs->running;
    bs->running = a->next;
    cf_ip_attempt_free(a, data);
  }
  bs->running_tail = &bs->running;
  cf_ip_attempt_free(bs->winner, data);
  bs->winner = ଖାଲି;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

ସ୍ଥିର CURLcode cf_ip_ballers_init(ସଂରଚନା cf_ip_ballers *bs,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା Curl_peer *origin,
                                   ସଂରଚନା Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ସଂରଚନା Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, ଆକାର(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  ଯଦି(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_ip_ballers_prune(ସଂରଚନା cf_ip_ballers *bs,
                                ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                uint32_t max_concurrent)
{
  ସଂରଚନା cf_ip_attempt *a = ଖାଲି, **panchor;
  uint32_t ongoing = 0;

  ପାଇଁ(a = bs->running; a; a = a->next) {
    ଯଦି(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  ଯେତେବେଳେ(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    ଯଦି(!a->result && !a->connected) {
      *panchor = a->next;
      ଯଦି(!a->next)
        bs->running_tail = panchor;
      a->next = ଖାଲି;
      cf_ip_attempt_free(a, data);
      --ongoing;
      CURL_TRC_CF(data, cf, "discarding oldest attempt to keep limit");
    }
    ଅନ୍ୟଥା {
      panchor = &a->next;
    }
  }
}

ସ୍ଥିର CURLcode cf_ip_ballers_run(ସଂରଚନା cf_ip_ballers *bs,
                                  ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା cf_ip_attempt *a = ଖାଲି, **panchor;
  bool do_more;
  timediff_t next_expire_ms;
  uint32_t inconclusive, ongoing;
  ଅଚଳ ସଂରଚନା curltime *pnow = ଖାଲି;
  VERBOSE(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i);

  ଯଦି(bs->winner)
    ଫେରନ୍ତୁ CURLE_OK;

evaluate:
  ଯଦି(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, ଖାଲି, TIMER_STARTSINGLE));
    ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  ପାଇଁ(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    ଯଦି(!a->result) {
      ଯଦି(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = ଖାଲି;
        ଯେତେବେଳେ(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        ଫେରନ୍ତୁ CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    ଅନ୍ୟଥା ଯଦି(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    ଅନ୍ୟଥା {
      /* hard failure, discard right away */
      *panchor = a->next;
      ଯଦି(!a->next)
        bs->running_tail = panchor;
      a->next = ଖାଲି;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  ଯଦି(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  ଯଦି(!ongoing) {
    do_more = TRUE;
  }
  ଅନ୍ୟଥା {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    ଯଦି(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    ଯଦି(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  ଯଦି(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ଅଚଳ ସଂରଚନା Curl_addrinfo *ai = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    ଯଦି((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    ଯଦି(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    ଯଦି(ai) {  /* try another address */
      ସଂରଚନା Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      ଯଦି(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      ଯଦି(result)
        ଯାଅ out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଯଦି(result)
        ଯାଅ out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_tail = a;
      a->next = ଖାଲି;
      bs->running_tail = &a->next;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      ଯାଅ evaluate;
    }
    ଅନ୍ୟଥା ଯଦି(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      ଯଦି(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        ପାଇଁ(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          ଯଦି(!a->inconclusive)
            ଜାରି;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
          ଯଦି(result) /* serious failure */
            ଯାଅ out;
          bs->last_attempt_started = *pnow;
          ଯାଅ evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      ଅନ୍ୟଥା {
        /* let's wait some more before restarting */
        infof(data, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  ଯଦି(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    ଯଦି(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, ଖାଲି, TIMER_STARTSINGLE));
      ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    ଯଦି(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    ଯଦି(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      ଯଦି(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        ଯାଅ evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_ip_ballers_shutdown(ସଂରଚନା cf_ip_ballers *bs,
                                       ସଂରଚନା Curl_easy *data,
                                       bool *done)
{
  ସଂରଚନା cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  ପାଇଁ(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    ଯଦି(a->shutdown || !a->cf)
      ଜାରି;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    ଯଦି(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    ଅନ୍ୟଥା
      *done = FALSE;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_ip_ballers_pollset(ସଂରଚନା cf_ip_ballers *bs,
                                      ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  ପାଇଁ(a = bs->running; a && !result; a = a->next) {
    ଯଦି(a->result)
      ଜାରି;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool cf_ip_ballers_pending(ସଂରଚନା cf_ip_ballers *bs,
                                  ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_ip_attempt *a;

  ପାଇଁ(a = bs->running; a; a = a->next) {
    ଯଦି(a->result)
      ଜାରି;
    ଯଦି(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cf_ip_ballers_min_reply_ms(ସଂରଚନା cf_ip_ballers *bs,
                                      ସଂରଚନା Curl_easy *data)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା reply_ms = -1, breply_ms;
  ସଂରଚନା cf_ip_attempt *a;

  ପାଇଁ(a = bs->running; a; a = a->next) {
    ଯଦି(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, ଖାଲି) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  ଫେରନ୍ତୁ reply_ms;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

ସଂରଚନା cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  ସଂରଚନା cf_ip_ballers ballers;
  BIT(dns_resolved);
};

ସ୍ଥିର CURLcode is_connected(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             bool *connected)
{
  ସଂରଚନା cf_ip_happy_ctx *ctx = cf->ctx;
  ସଂରଚନା connectdata *conn = cf->conn;
  CURLcode result;

  result = cf_ip_ballers_run(&ctx->ballers, cf, data,
                             (bool)ctx->dns_resolved, connected);

  ଯଦି(!result)
    ଫେରନ୍ତୁ CURLE_OK;
  ଅନ୍ୟଥା {
    ସଂରଚନା Curl_peer *peer = ଖାଲି, *proxy_peer = ଖାଲି;
    ଅକ୍ଷର viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    ଯଦି(!conn->origin || !peer)
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    ଯଦି(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    ଅନ୍ୟଥା ଯଦି(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    ଯଦି(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      ଯଦି(peer->unix_socket)
        curl_msnprintf(viamsg, ଆକାର(viamsg), " over unix://%s",
                       peer->hostname);
      ଅନ୍ୟଥା
#endif
      curl_msnprintf(viamsg, ଆକାର(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(data, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(data, ଖାଲି, TIMER_STARTSINGLE),
          curl_easy_strerror(result));

#ifdef SOCKETIMEDOUT
    ଯଦି(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    ଫେରନ୍ତୁ result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

ସ୍ଥିର CURLcode cf_ip_happy_init(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_ip_happy_ctx *ctx = cf->ctx;

  ଯଦି(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
  }

  ଯଦି(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  ଅନ୍ୟଥା { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(data, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_ip_happy_ctx_clear(ସଂରଚନା cf_ip_happy_ctx *ctx,
                                  ସଂରଚନା Curl_easy *data)
{
  DEBUGASSERT(ctx);
  ଯଦି(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

ସ୍ଥିର ଶୂନ୍ୟ cf_ip_happy_ctx_destroy(ସଂରଚନା cf_ip_happy_ctx *ctx,
                                    ସଂରଚନା Curl_easy *data)
{
  ଯଦି(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    curlx_free(ctx);
  }
}

ସ୍ଥିର CURLcode cf_ip_happy_shutdown(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data,
                                     bool *done)
{
  ସଂରଚନା cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *done);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_ip_happy_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                           ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ଯଦି(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
                ps->n);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_ip_happy_connect(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    bool *done)
{
  ସଂରଚନା cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  ଯଦି(!data)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  ଯଦି(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    ଯଦି(!result)
      ctx->dns_resolved = TRUE;
    ଅନ୍ୟଥା ଯଦି(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    ଅନ୍ୟଥା /* real error */
      ଯାଅ out;
  }

  ବାଛ(ctx->state) {
  ସ୍ଥିତି SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    ଯଦି(result)
      ଯାଅ out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  ସ୍ଥିତି SCFST_WAITING:
    result = is_connected(cf, data, done);
    ଯଦି(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = ଖାଲି;

      ଯଦି(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      ଯଦି(Curl_trc_cf_is_verbose(cf, data)) {
        ସଂରଚନା ip_quadruple ipquad;
        bool is_ipv6;
        ଯଦି(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
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
    ଭାଙ୍ଗ;
  ସ୍ଥିତି SCFST_DONE:
    *done = TRUE;
    ଭାଙ୍ଗ;
  }
out:
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool cf_ip_happy_data_pending(ସଂରଚନା Curl_cfilter *cf,
                                     ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_ip_happy_ctx *ctx = cf->ctx;

  ଯଦି(!cf->connected) {
    ଫେରନ୍ତୁ cf_ip_ballers_pending(&ctx->ballers, data);
  }
  ଫେରନ୍ତୁ cf->next->cft->has_data_pending(cf->next, data);
}

ସ୍ଥିର CURLcode cf_ip_happy_query(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା cf_ip_happy_ctx *ctx = cf->ctx;

  ଯଦି(!cf->connected) {
    ବାଛ(query) {
    ସ୍ଥିତି CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
  }

  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_ip_happy_destroy(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  ଯଦି(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
}

ସଂରଚନା Curl_cftype Curl_cft_ip_happy = {
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

ସ୍ଥିର CURLcode cf_ip_happy_create(ସଂରଚନା Curl_cfilter **pcf,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା Curl_peer *origin,
                                   ସଂରଚନା Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ସଂରଚନା connectdata *conn,
                                   ସଂରଚନା Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  ସଂରଚନା cf_ip_happy_ctx *ctx = ଖାଲି;
  CURLcode result;

  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)conn;
  *pcf = ଖାଲି;
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  ଯଦି(result)
    ଯାଅ out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  ଯଦି(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_ip_happy_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                       ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା Curl_peer *origin,
                                       ସଂରଚନା Curl_peer *peer,
                                       uint8_t transport_peer,
                                       ସଂରଚନା Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  result = cf_ip_happy_create(&cf, data, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);
  ଫେରନ୍ତୁ CURLE_OK;
}
