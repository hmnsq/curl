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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h> /* <netinet/tcp.h> may need it */
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_LINUX_TCP_H
#అన్తర్భావ <linux/tcp.h>
#లేదావేళ defined(HAVE_NETINET_TCP_H)
#అన్తర్భావ <netinet/tcp.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు

#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-ip-happy.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "sockaddr.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "vquic/vquic.h" /* for quic cfilters */


నిర్మాణం transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

స్థిర
#నిర్వచితం_కాకపోతే UNITTESTS
స్థిరరాశి
#అయితే_ముగింపు
నిర్మాణం transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
  { Curl_cf_h3_proxy_create, TRNSPRT_QUIC, TRUE },
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_TFTP
  { Curl_cf_udp_create, TRNSPRT_UDP, FALSE },
#అయితే_ముగింపు
#నిర్వచితమైతే USE_UNIX_SOCKETS
  { Curl_cf_unix_create, TRNSPRT_UNIX, FALSE },
  { Curl_cf_unix_create, TRNSPRT_UNIX, TRUE },
#అయితే_ముగింపు
};

స్థిర cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  కొరకు(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ఒకవేళ((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      తిరిగి transport_providers[i].cf_create;
  }
  తిరిగి ఖాళీ;
}

#నిర్వచితమైతే UNITTESTS
/* @unittest 2600 */
UNITTEST శూన్య debug_సమితి_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST శూన్య debug_సమితి_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  కొరకు(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ఒకవేళ(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#అయితే_ముగింపు /* UNITTESTS */

నిర్మాణం cf_ai_iter {
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం Curl_peer *peer;
  పూర్ణాంకం ai_family;
  గుర్తులేని పూర్ణాంకం n;
};

స్థిర శూన్య cf_ai_iter_init(నిర్మాణం cf_ai_iter *iter,
                            నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_peer *peer,
                            పూర్ణాంకం ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_addrinfo *cf_ai_iter_తదుపరి(నిర్మాణం cf_ai_iter *iter,
                                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి నిర్మాణం Curl_addrinfo *addr;

  ఒకవేళ(!iter->cf)
    తిరిగి ఖాళీ;

  addr = Curl_conn_dns_get_ai(దత్తాంశం, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  ఒకవేళ(addr)
    iter->n++;
  తిరిగి addr;
}

స్థిర bool cf_ai_iter_has_మరిన్ని(నిర్మాణం cf_ai_iter *iter,
                                నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి (iter->cf &&
          !!Curl_conn_dns_get_ai(దత్తాంశం, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

నిర్మాణం cf_ip_attempt {
  నిర్మాణం cf_ip_attempt *తదుపరి;
  నిర్మాణం Curl_peer *origin;
  నిర్మాణం Curl_peer *peer;
  నిర్మాణం Curl_peer *tunnel_peer;
  నిర్మాణం Curl_sockaddr_ex addr;
  నిర్మాణం Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode ఫలితం;
  పూర్ణాంకం ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  పూర్ణాంకం error;
  బిట్టు(connected);                    /* cf has connected */
  బిట్టు(shutdown);                     /* cf has shutdown */
  బిట్టు(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

స్థిర శూన్య cf_ip_attempt_free(నిర్మాణం cf_ip_attempt *a,
                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(a) {
    ఒకవేళ(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, దత్తాంశం);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

స్థిర CURLcode cf_ip_attempt_new(నిర్మాణం cf_ip_attempt **pa,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_peer *origin,
                                  నిర్మాణం Curl_peer *peer,
                                  uint8_t transport_peer,
                                  నిర్మాణం Curl_sockaddr_ex *addr,
                                  పూర్ణాంకం ai_family,
                                  నిర్మాణం Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  నిర్మాణం Curl_cfilter *wcf;
  నిర్మాణం cf_ip_attempt *a;
  CURLcode ఫలితం = CURLE_OK;

  *pa = ఖాళీ;
  a = curlx_calloc(1, పరిమాణం(*a));
  ఒకవేళ(!a)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  Curl_peer_link(&a->origin, origin);
  Curl_peer_link(&a->peer, peer);
  a->transport_peer = transport_peer;
  Curl_peer_link(&a->tunnel_peer, tunnel_peer);
  a->tunnel_transport = tunnel_transport;
  a->addr = *addr;
  a->ai_family = ai_family;
  a->ఫలితం = CURLE_OK;
  a->cf_create = cf_create;
  *pa = a;

  ఫలితం = a->cf_create(&a->cf, దత్తాంశం, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* the new filter might have sub-filters */
  కొరకు(wcf = a->cf; wcf; wcf = wcf->తదుపరి) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    cf_ip_attempt_free(a, దత్తాంశం);
    *pa = ఖాళీ;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_ip_attempt_connect(నిర్మాణం cf_ip_attempt *a,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  ఒకవేళ(!a->ఫలితం && !*connected) {
    /* evaluate again */
    a->ఫలితం = Curl_conn_cf_connect(a->cf, దత్తాంశం, connected);

    ఒకవేళ(!a->ఫలితం) {
      ఒకవేళ(*connected) {
        a->connected = TRUE;
      }
    }
    లేకపోతే {
      ఒకవేళ(a->ఫలితం == CURLE_WEIRD_SERVER_జవాబు)
        a->inconclusive = TRUE;
      ఒకవేళ(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, దత్తాంశం);
    }
  }
  తిరిగి a->ఫలితం;
}

నిర్మాణం cf_ip_ballers {
  నిర్మాణం cf_ip_attempt *running;
  నిర్మాణం cf_ip_attempt **running_తోక;  /* &running or &(last->next) */
  నిర్మాణం cf_ip_attempt *winner;
  నిర్మాణం cf_ai_iter addr_iter;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం cf_ai_iter ipv6_iter;
#అయితే_ముగింపు
  నిర్మాణం Curl_peer *origin;
  నిర్మాణం Curl_peer *peer;
  నిర్మాణం Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  నిర్మాణం curltime చివరి_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode చివరి_dead_ఫలితం;  /* result of last discarded hard failure */
  పూర్ణాంకం చివరి_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  బిట్టు(had_dead);              /* a hard failure was discarded */
};

స్థిర CURLcode cf_ip_attempt_restart(నిర్మాణం cf_ip_attempt *a,
                                      నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *wcf;
  CURLcode ఫలితం;

  ఒకవేళ(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, దత్తాంశం);

  a->ఫలితం = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = ఖాళీ;

  ఫలితం = a->cf_create(&a->cf, దత్తాంశం, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  ఒకవేళ(!ఫలితం) {
    bool dummy;
    /* the new filter might have sub-filters */
    కొరకు(wcf = a->cf; wcf; wcf = wcf->తదుపరి) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->ఫలితం = cf_ip_attempt_connect(a, దత్తాంశం, &dummy);
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_ip_ballers_clear(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం cf_ip_ballers *bs)
{
  అయితే(bs->running) {
    నిర్మాణం cf_ip_attempt *a = bs->running;
    bs->running = a->తదుపరి;
    cf_ip_attempt_free(a, దత్తాంశం);
  }
  bs->running_తోక = &bs->running;
  cf_ip_attempt_free(bs->winner, దత్తాంశం);
  bs->winner = ఖాళీ;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

స్థిర CURLcode cf_ip_ballers_init(నిర్మాణం cf_ip_ballers *bs,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం Curl_peer *origin,
                                   నిర్మాణం Curl_peer *peer,
                                   uint8_t transport_peer,
                                   నిర్మాణం Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, పరిమాణం(*bs));
  bs->running_తోక = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  ఒకవేళ(!bs->cf_create) {
    failf(దత్తాంశం, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->చివరి_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cf_ip_ballers_prune(నిర్మాణం cf_ip_ballers *bs,
                                నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                uint32_t max_concurrent)
{
  నిర్మాణం cf_ip_attempt *a = ఖాళీ, **panchor;
  uint32_t ongoing = 0;

  కొరకు(a = bs->running; a; a = a->తదుపరి) {
    ఒకవేళ(!a->ఫలితం && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  అయితే(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    ఒకవేళ(!a->ఫలితం && !a->connected) {
      *panchor = a->తదుపరి;
      ఒకవేళ(!a->తదుపరి)
        bs->running_తోక = panchor;
      a->తదుపరి = ఖాళీ;
      cf_ip_attempt_free(a, దత్తాంశం);
      --ongoing;
      CURL_TRC_CF(దత్తాంశం, cf, "discarding oldest attempt to keep limit");
    }
    లేకపోతే {
      panchor = &a->తదుపరి;
    }
  }
}

స్థిర CURLcode cf_ip_ballers_నిరంతరం(నిర్మాణం cf_ip_ballers *bs,
                                  నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం cf_ip_attempt *a = ఖాళీ, **panchor;
  bool do_మరిన్ని;
  timediff_t తదుపరి_expire_ms;
  uint32_t inconclusive, ongoing;
  స్థిరరాశి నిర్మాణం curltime *pnow = ఖాళీ;
  VERBOSE(పూర్ణాంకం i);

  ఒకవేళ(bs->winner)
    తిరిగి CURLE_OK;

evaluate:
  ఒకవేళ(Curl_timeleft_now_ms(దత్తాంశం, Curl_pgrs_now(దత్తాంశం)) < 0) {
    failf(దత్తాంశం, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(దత్తాంశం, ఖాళీ, TIMER_STARTSINGLE));
    తిరిగి CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  కొరకు(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->ఫలితం = cf_ip_attempt_connect(a, దత్తాంశం, connected);
    ఒకవేళ(!a->ఫలితం) {
      ఒకవేళ(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(దత్తాంశం, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->తదుపరి;
        a->తదుపరి = ఖాళీ;
        అయితే(bs->running) {
          a = bs->running;
          bs->running = a->తదుపరి;
          cf_ip_attempt_free(a, దత్తాంశం);
        }
        bs->running_తోక = &bs->running;
        తిరిగి CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->తదుపరి;
    }
    లేకపోతే ఒకవేళ(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->తదుపరి;
    }
    లేకపోతే {
      /* hard failure, discard right away */
      *panchor = a->తదుపరి;
      ఒకవేళ(!a->తదుపరి)
        bs->running_తోక = panchor;
      a->తదుపరి = ఖాళీ;
      bs->చివరి_dead_ఫలితం = a->ఫలితం;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, దత్తాంశం);
    }
  }
  ఒకవేళ(bs->running)
    CURL_TRC_CF(దత్తాంశం, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(దత్తాంశం);
  ఒకవేళ(!ongoing) {
    do_మరిన్ని = TRUE;
  }
  లేకపోతే {
    bool మరిన్ని_possible = cf_ai_iter_has_మరిన్ని(&bs->addr_iter, దత్తాంశం);
#నిర్వచితమైతే USE_IPV6
    ఒకవేళ(!మరిన్ని_possible)
      మరిన్ని_possible = cf_ai_iter_has_మరిన్ని(&bs->ipv6_iter, దత్తాంశం);
#అయితే_ముగింపు
    do_మరిన్ని = మరిన్ని_possible &&
      (curlx_ptimediff_ms(pnow, &bs->చివరి_attempt_started) >=
       bs->attempt_delay_ms);
    ఒకవేళ(do_మరిన్ని)
      CURL_TRC_CF(దత్తాంశం, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  ఒకవేళ(do_మరిన్ని) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    స్థిరరాశి నిర్మాణం Curl_addrinfo *ai = ఖాళీ;
    పూర్ణాంకం ai_family = 0;
    CURL_TRC_CF(దత్తాంశం, cf, "want to do more");
#నిర్వచితమైతే USE_IPV6
    ఒకవేళ((bs->చివరి_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_మరిన్ని(&bs->addr_iter, దత్తాంశం)) {
      ai = cf_ai_iter_తదుపరి(&bs->ipv6_iter, దత్తాంశం);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(దత్తాంశం, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#అయితే_ముగింపు
    ఒకవేళ(!ai) {
      ai = cf_ai_iter_తదుపరి(&bs->addr_iter, దత్తాంశం);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(దత్తాంశం, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(దత్తాంశం);

    ఒకవేళ(ai) {  /* try another address */
      నిర్మాణం Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      ఒకవేళ(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, దత్తాంశం, bs->max_concurrent - 1);

      ఫలితం = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;

      ఫలితం = cf_ip_attempt_new(&a, దత్తాంశం, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(దత్తాంశం, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (పూర్ణాంకం)ఫలితం);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_తోక = a;
      a->తదుపరి = ఖాళీ;
      bs->running_తోక = &a->తదుపరి;
      bs->చివరి_attempt_started = *pnow;
      bs->చివరి_attempt_ai_family = ai_family;
      /* and run everything again */
      వెళ్ళు evaluate;
    }
    లేకపోతే ఒకవేళ(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->చివరి_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      ఒకవేళ(delay_ms <= 0) {
        CURL_TRC_CF(దత్తాంశం, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        కొరకు(a = bs->running; a; a = a->తదుపరి) {
          VERBOSE(++i);
          ఒకవేళ(!a->inconclusive)
            కొనసాగు;
          ఫలితం = cf_ip_attempt_restart(a, cf, దత్తాంశం);
          CURL_TRC_CF(దత్తాంశం, cf, "restarted baller %d -> %d", i, (పూర్ణాంకం)ఫలితం);
          ఒకవేళ(ఫలితం) /* serious failure */
            వెళ్ళు బహిర్గతం;
          bs->చివరి_attempt_started = *pnow;
          వెళ్ళు evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      లేకపోతే {
        /* let's wait some more before restarting */
        infof(దత్తాంశం, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_సమితి(దత్తాంశం, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(దత్తాంశం, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      ఫలితం = bs->had_dead ? bs->చివరి_dead_ఫలితం : CURLE_COULDNT_CONNECT;
    }
  }

బహిర్గతం:
  ఒకవేళ(!ఫలితం) {
    bool మరిన్ని_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(దత్తాంశం);
    తదుపరి_expire_ms = Curl_timeleft_now_ms(దత్తాంశం, pnow);
    ఒకవేళ(తదుపరి_expire_ms < 0) {
      failf(దత్తాంశం, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(దత్తాంశం, ఖాళీ, TIMER_STARTSINGLE));
      తిరిగి CURLE_OPERATION_TIMEDOUT;
    }

    మరిన్ని_possible = cf_ai_iter_has_మరిన్ని(&bs->addr_iter, దత్తాంశం);
#నిర్వచితమైతే USE_IPV6
    ఒకవేళ(!మరిన్ని_possible)
      మరిన్ని_possible = cf_ai_iter_has_మరిన్ని(&bs->ipv6_iter, దత్తాంశం);
#అయితే_ముగింపు
    ఒకవేళ(మరిన్ని_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->చివరి_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      తదుపరి_expire_ms = CURLMIN(తదుపరి_expire_ms, expire_ms);
      ఒకవేళ(తదుపరి_expire_ms <= 0) {
        CURL_TRC_CF(దత్తాంశం, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        వెళ్ళు evaluate;
      }
      CURL_TRC_CF(దత్తాంశం, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", తదుపరి_expire_ms);
      Curl_expire_సమితి(దత్తాంశం, EXPIRE_HAPPY_EYEBALLS, తదుపరి_expire_ms, pnow);
    }
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_ip_ballers_shutdown(నిర్మాణం cf_ip_ballers *bs,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       bool *పూర్తి)
{
  నిర్మాణం cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *పూర్తి = TRUE;
  కొరకు(a = bs->running; a; a = a->తదుపరి) {
    bool bdone = FALSE;
    ఒకవేళ(a->shutdown || !a->cf)
      కొనసాగు;
    a->ఫలితం = a->cf->cft->do_shutdown(a->cf, దత్తాంశం, &bdone);
    ఒకవేళ(a->ఫలితం || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    లేకపోతే
      *పూర్తి = FALSE;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_ip_ballers_pollset(నిర్మాణం cf_ip_ballers *bs,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం cf_ip_attempt *a;
  CURLcode ఫలితం = CURLE_OK;
  కొరకు(a = bs->running; a && !ఫలితం; a = a->తదుపరి) {
    ఒకవేళ(a->ఫలితం)
      కొనసాగు;
    ఫలితం = Curl_conn_cf_adjust_pollset(a->cf, దత్తాంశం, ps);
  }
  తిరిగి ఫలితం;
}

స్థిర bool cf_ip_ballers_pending(నిర్మాణం cf_ip_ballers *bs,
                                  స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_ip_attempt *a;

  కొరకు(a = bs->running; a; a = a->తదుపరి) {
    ఒకవేళ(a->ఫలితం)
      కొనసాగు;
    ఒకవేళ(a->cf && a->cf->cft->has_దత్తాంశం_pending(a->cf, దత్తాంశం))
      తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

స్థిర పూర్ణాంకం cf_ip_ballers_min_జవాబు_ms(నిర్మాణం cf_ip_ballers *bs,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  పూర్ణాంకం జవాబు_ms = -1, breply_ms;
  నిర్మాణం cf_ip_attempt *a;

  కొరకు(a = bs->running; a; a = a->తదుపరి) {
    ఒకవేళ(a->cf && !a->cf->cft->query(a->cf, దత్తాంశం, CF_QUERY_CONNECT_జవాబు_MS,
                                   &breply_ms, ఖాళీ) &&
       breply_ms >= 0 && (జవాబు_ms < 0 || breply_ms < జవాబు_ms))
      జవాబు_ms = breply_ms;
  }
  తిరిగి జవాబు_ms;
}

రకనిర్వచనం గణన {
  SCFST_INIT,
  SCFST_నిరీక్షణ,
  SCFST_పూర్తి
} cf_connect_state;

నిర్మాణం cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  నిర్మాణం cf_ip_ballers ballers;
  బిట్టు(dns_resolved);
};

స్థిర CURLcode is_connected(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             bool *connected)
{
  నిర్మాణం cf_ip_happy_ctx *ctx = cf->ctx;
  నిర్మాణం connectdata *conn = cf->conn;
  CURLcode ఫలితం;

  ఫలితం = cf_ip_ballers_నిరంతరం(&ctx->ballers, cf, దత్తాంశం,
                             (bool)ctx->dns_resolved, connected);

  ఒకవేళ(!ఫలితం)
    తిరిగి CURLE_OK;
  లేకపోతే {
    నిర్మాణం Curl_peer *peer = ఖాళీ, *proxy_peer = ఖాళీ;
    అక్షరం viamsg[160];

    peer = Curl_conn_get_మొదటి_peer(conn, cf->sockindex);
    ఒకవేళ(!conn->origin || !peer)
      తిరిగి CURLE_FAILED_INIT;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    ఒకవేళ(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    లేకపోతే ఒకవేళ(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#అయితే_ముగింపు

    viamsg[0] = 0;
    ఒకవేళ(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#నిర్వచితమైతే USE_UNIX_SOCKETS
      ఒకవేళ(peer->unix_socket)
        curl_msnprintf(viamsg, పరిమాణం(viamsg), " over unix://%s",
                       peer->hostname);
      లేకపోతే
#అయితే_ముగింపు
      curl_msnprintf(viamsg, పరిమాణం(viamsg), " via %s:%u",
                     peer->hostname, peer->పోర్టు);
    }

    failf(దత్తాంశం, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->పోర్టు, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(దత్తాంశం, ఖాళీ, TIMER_STARTSINGLE),
          curl_easy_strerror(ఫలితం));

#నిర్వచితమైతే SOCKETIMEDOUT
    ఒకవేళ(SOCKETIMEDOUT == దత్తాంశం->state.os_errno)
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
#అయితే_ముగింపు

    తిరిగి ఫలితం;
  }
}

#నిర్వచించు IP_HE_MAX_CONCURRENT_ATTEMPTS     6

స్థిర CURLcode cf_ip_happy_init(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_ip_happy_ctx *ctx = cf->ctx;

  ఒకవేళ(Curl_timeleft_ms(దత్తాంశం) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(దత్తాంశం, "Connection time-out");
    తిరిగి CURLE_OPERATION_TIMEDOUT;
  }

  ఒకవేళ(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#నిర్వచితమైతే USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#లేకపోతే
    తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
#అయితే_ముగింపు
  }
  లేకపోతే { /* TCP/UDP/QUIC */
#నిర్వచితమైతే USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#అయితే_ముగింపు
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(దత్తాంశం, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cf_ip_happy_ctx_clear(నిర్మాణం cf_ip_happy_ctx *ctx,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  DEBUGASSERT(ctx);
  ఒకవేళ(ctx)
    cf_ip_ballers_clear(దత్తాంశం, &ctx->ballers);
}

స్థిర శూన్య cf_ip_happy_ctx_destroy(నిర్మాణం cf_ip_happy_ctx *ctx,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(ctx) {
    cf_ip_happy_ctx_clear(ctx, దత్తాంశం);
    curlx_free(ctx);
  }
}

స్థిర CURLcode cf_ip_happy_shutdown(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం,
                                     bool *పూర్తి)
{
  నిర్మాణం cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  ఫలితం = cf_ip_ballers_shutdown(&ctx->ballers, దత్తాంశం, పూర్తి);
  CURL_TRC_CF(దత్తాంశం, cf, "shutdown -> %d, done=%d", (పూర్ణాంకం)ఫలితం, *పూర్తి);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_ip_happy_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                           నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!cf->connected) {
    ఫలితం = cf_ip_ballers_pollset(&ctx->ballers, దత్తాంశం, ps);
    CURL_TRC_CF(దత్తాంశం, cf, "adjust_pollset -> %d, %u socks", (పూర్ణాంకం)ఫలితం,
                ps->n);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_ip_happy_connect(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    bool *పూర్తి)
{
  నిర్మాణం cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  ఒకవేళ(!దత్తాంశం)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *పూర్తి = FALSE;

  ఒకవేళ(!ctx->dns_resolved) {
    ఫలితం = Curl_conn_dns_addr_ఫలితం(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    ఒకవేళ(!ఫలితం)
      ctx->dns_resolved = TRUE;
    లేకపోతే ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      ఫలితం = CURLE_OK;
    }
    లేకపోతే /* real error */
      వెళ్ళు బహిర్గతం;
  }

  ఎంపిక(ctx->state) {
  స్థితి SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, దత్తాంశం));
    DEBUGASSERT(!cf->connected);
    ఫలితం = cf_ip_happy_init(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ctx->state = SCFST_నిరీక్షణ;
    FALLTHROUGH();
  స్థితి SCFST_నిరీక్షణ:
    ఫలితం = is_connected(cf, దత్తాంశం, పూర్తి);
    ఒకవేళ(!ఫలితం && *పూర్తి) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_పూర్తి;
      cf->connected = TRUE;
      cf->తదుపరి = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = ఖాళీ;

      ఒకవేళ(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrs_సమయం(దత్తాంశం, TIMER_APPCONNECT); /* we are connected already */
#నిర్వచితమైతే CURLVERBOSE
      ఒకవేళ(Curl_trc_cf_is_verbose(cf, దత్తాంశం)) {
        నిర్మాణం ip_quadruple ipquad;
        bool is_ipv6;
        ఒకవేళ(!Curl_conn_cf_get_ip_info(cf->తదుపరి, దత్తాంశం, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(దత్తాంశం, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_పోర్టు);
        }
      }
#అయితే_ముగింపు
      cf_ip_happy_ctx_clear(ctx, దత్తాంశం);
      Curl_expire_clear(దత్తాంశం, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(దత్తాంశం);
      దత్తాంశం->info.numconnects++; /* to track the # of connections made */
    }
    ఆపు;
  స్థితి SCFST_పూర్తి:
    *పూర్తి = TRUE;
    ఆపు;
  }
బహిర్గతం:
  తిరిగి ఫలితం;
}

స్థిర bool cf_ip_happy_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                                     స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_ip_happy_ctx *ctx = cf->ctx;

  ఒకవేళ(!cf->connected) {
    తిరిగి cf_ip_ballers_pending(&ctx->ballers, దత్తాంశం);
  }
  తిరిగి cf->తదుపరి->cft->has_దత్తాంశం_pending(cf->తదుపరి, దత్తాంశం);
}

స్థిర CURLcode cf_ip_happy_query(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం cf_ip_happy_ctx *ctx = cf->ctx;

  ఒకవేళ(!cf->connected) {
    ఎంపిక(query) {
    స్థితి CF_QUERY_CONNECT_జవాబు_MS: {
      *pres1 = cf_ip_ballers_min_జవాబు_ms(&ctx->ballers, దత్తాంశం);
      CURL_TRC_CF(దత్తాంశం, cf, "query connect reply: %dms", *pres1);
      తిరిగి CURLE_OK;
    }
    అప్రమేయ:
      ఆపు;
    }
  }

  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

స్థిర శూన్య cf_ip_happy_destroy(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(దత్తాంశం, cf, "destroy");
  ఒకవేళ(ctx) {
    cf_ip_happy_ctx_clear(ctx, దత్తాంశం);
    cf_ip_happy_ctx_destroy(ctx, దత్తాంశం);
  }
}

నిర్మాణం Curl_cftype Curl_cft_ip_happy = {
  "HAPPY-EYEBALLS",
  CF_TYPE_అమరిక,
  CURL_LOG_LVL_NONE,
  cf_ip_happy_destroy,
  cf_ip_happy_connect,
  cf_ip_happy_shutdown,
  cf_ip_happy_adjust_pollset,
  cf_ip_happy_దత్తాంశం_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

స్థిర CURLcode cf_ip_happy_create(నిర్మాణం Curl_cfilter **pcf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం Curl_peer *origin,
                                   నిర్మాణం Curl_peer *peer,
                                   uint8_t transport_peer,
                                   నిర్మాణం connectdata *conn,
                                   నిర్మాణం Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  నిర్మాణం cf_ip_happy_ctx *ctx = ఖాళీ;
  CURLcode ఫలితం;

  (శూన్య)దత్తాంశం;
  (శూన్య)conn;
  *pcf = ఖాళీ;
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ఫలితం = cf_ip_ballers_init(&ctx->ballers, దత్తాంశం,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              దత్తాంశం->సమితి.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, దత్తాంశం);
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_ip_happy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం Curl_peer *origin,
                                       నిర్మాణం Curl_peer *peer,
                                       uint8_t transport_peer,
                                       నిర్మాణం Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  ఫలితం = cf_ip_happy_create(&cf, దత్తాంశం, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  Curl_conn_cf_చొప్పించు_after(cf_at, cf);
  తిరిగి CURLE_OK;
}
