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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_LINUX_TCP_H
#অন্তর্ভাব <linux/tcp.h>
#নতুবা_যদি defined(HAVE_NETINET_TCP_H)
#অন্তর্ভাব <netinet/tcp.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif

#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-ip-happy.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "sockaddr.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "vquic/vquic.h" /* for quic cfilters */


কাঠামো transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

স্থির
#ifndef UNITTESTS
ধ্রুবক
#endif
কাঠামো transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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

স্থির cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  জন্য(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    যদি((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      ফেরত transport_providers[i].cf_create;
  }
  ফেরত ফাঁকা;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST শূন্য debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST শূন্য debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  জন্য(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    যদি(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

কাঠামো cf_ai_iter {
  কাঠামো Curl_cfilter *cf;
  কাঠামো Curl_peer *peer;
  পূর্ণসংখ্যা ai_family;
  unsigned পূর্ণসংখ্যা n;
};

স্থির শূন্য cf_ai_iter_init(কাঠামো cf_ai_iter *iter,
                            কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_peer *peer,
                            পূর্ণসংখ্যা ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

স্থির ধ্রুবক কাঠামো Curl_addrinfo *cf_ai_iter_পরবর্তী(কাঠামো cf_ai_iter *iter,
                                                   কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক কাঠামো Curl_addrinfo *addr;

  যদি(!iter->cf)
    ফেরত ফাঁকা;

  addr = Curl_conn_dns_get_ai(তথ্য, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  যদি(addr)
    iter->n++;
  ফেরত addr;
}

স্থির bool cf_ai_iter_has_more(কাঠামো cf_ai_iter *iter,
                                কাঠামো Curl_easy *তথ্য)
{
  ফেরত (iter->cf &&
          !!Curl_conn_dns_get_ai(তথ্য, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

কাঠামো cf_ip_attempt {
  কাঠামো cf_ip_attempt *পরবর্তী;
  কাঠামো Curl_peer *origin;
  কাঠামো Curl_peer *peer;
  কাঠামো Curl_peer *tunnel_peer;
  কাঠামো Curl_sockaddr_ex addr;
  কাঠামো Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  পূর্ণসংখ্যা error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

স্থির শূন্য cf_ip_attempt_free(কাঠামো cf_ip_attempt *a,
                               কাঠামো Curl_easy *তথ্য)
{
  যদি(a) {
    যদি(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, তথ্য);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

স্থির CURLcode cf_ip_attempt_new(কাঠামো cf_ip_attempt **pa,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_peer *origin,
                                  কাঠামো Curl_peer *peer,
                                  uint8_t transport_peer,
                                  কাঠামো Curl_sockaddr_ex *addr,
                                  পূর্ণসংখ্যা ai_family,
                                  কাঠামো Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  কাঠামো Curl_cfilter *wcf;
  কাঠামো cf_ip_attempt *a;
  CURLcode ফলাফল = CURLE_OK;

  *pa = ফাঁকা;
  a = curlx_calloc(1, আকার(*a));
  যদি(!a)
    ফেরত CURLE_OUT_OF_MEMORY;

  Curl_peer_link(&a->origin, origin);
  Curl_peer_link(&a->peer, peer);
  a->transport_peer = transport_peer;
  Curl_peer_link(&a->tunnel_peer, tunnel_peer);
  a->tunnel_transport = tunnel_transport;
  a->addr = *addr;
  a->ai_family = ai_family;
  a->ফলাফল = CURLE_OK;
  a->cf_create = cf_create;
  *pa = a;

  ফলাফল = a->cf_create(&a->cf, তথ্য, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  যদি(ফলাফল)
    যাও out;

  /* the new filter might have sub-filters */
  জন্য(wcf = a->cf; wcf; wcf = wcf->পরবর্তী) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  যদি(ফলাফল) {
    cf_ip_attempt_free(a, তথ্য);
    *pa = ফাঁকা;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_ip_attempt_connect(কাঠামো cf_ip_attempt *a,
                                      কাঠামো Curl_easy *তথ্য,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  যদি(!a->ফলাফল && !*connected) {
    /* evaluate again */
    a->ফলাফল = Curl_conn_cf_connect(a->cf, তথ্য, connected);

    যদি(!a->ফলাফল) {
      যদি(*connected) {
        a->connected = TRUE;
      }
    }
    অন্যথা {
      যদি(a->ফলাফল == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      যদি(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, তথ্য);
    }
  }
  ফেরত a->ফলাফল;
}

কাঠামো cf_ip_ballers {
  কাঠামো cf_ip_attempt *running;
  কাঠামো cf_ip_attempt **running_লেজ;  /* &running or &(last->next) */
  কাঠামো cf_ip_attempt *winner;
  কাঠামো cf_ai_iter addr_iter;
#ifdef USE_IPV6
  কাঠামো cf_ai_iter ipv6_iter;
#endif
  কাঠামো Curl_peer *origin;
  কাঠামো Curl_peer *peer;
  কাঠামো Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  কাঠামো curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_ফলাফল;  /* result of last discarded hard failure */
  পূর্ণসংখ্যা last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

স্থির CURLcode cf_ip_attempt_restart(কাঠামো cf_ip_attempt *a,
                                      কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cfilter *wcf;
  CURLcode ফলাফল;

  যদি(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, তথ্য);

  a->ফলাফল = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = ফাঁকা;

  ফলাফল = a->cf_create(&a->cf, তথ্য, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  যদি(!ফলাফল) {
    bool dummy;
    /* the new filter might have sub-filters */
    জন্য(wcf = a->cf; wcf; wcf = wcf->পরবর্তী) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->ফলাফল = cf_ip_attempt_connect(a, তথ্য, &dummy);
  }
  ফেরত ফলাফল;
}

স্থির শূন্য cf_ip_ballers_clear(কাঠামো Curl_easy *তথ্য,
                                কাঠামো cf_ip_ballers *bs)
{
  যখন(bs->running) {
    কাঠামো cf_ip_attempt *a = bs->running;
    bs->running = a->পরবর্তী;
    cf_ip_attempt_free(a, তথ্য);
  }
  bs->running_লেজ = &bs->running;
  cf_ip_attempt_free(bs->winner, তথ্য);
  bs->winner = ফাঁকা;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

স্থির CURLcode cf_ip_ballers_init(কাঠামো cf_ip_ballers *bs,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো Curl_peer *origin,
                                   কাঠামো Curl_peer *peer,
                                   uint8_t transport_peer,
                                   কাঠামো Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, আকার(*bs));
  bs->running_লেজ = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  যদি(!bs->cf_create) {
    failf(তথ্য, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    ফেরত CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  ফেরত CURLE_OK;
}

স্থির শূন্য cf_ip_ballers_prune(কাঠামো cf_ip_ballers *bs,
                                কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                uint32_t max_concurrent)
{
  কাঠামো cf_ip_attempt *a = ফাঁকা, **panchor;
  uint32_t ongoing = 0;

  জন্য(a = bs->running; a; a = a->পরবর্তী) {
    যদি(!a->ফলাফল && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  যখন(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    যদি(!a->ফলাফল && !a->connected) {
      *panchor = a->পরবর্তী;
      যদি(!a->পরবর্তী)
        bs->running_লেজ = panchor;
      a->পরবর্তী = ফাঁকা;
      cf_ip_attempt_free(a, তথ্য);
      --ongoing;
      CURL_TRC_CF(তথ্য, cf, "discarding oldest attempt to keep limit");
    }
    অন্যথা {
      panchor = &a->পরবর্তী;
    }
  }
}

স্থির CURLcode cf_ip_ballers_run(কাঠামো cf_ip_ballers *bs,
                                  কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো cf_ip_attempt *a = ফাঁকা, **panchor;
  bool do_more;
  timediff_t পরবর্তী_expire_ms;
  uint32_t inconclusive, ongoing;
  ধ্রুবক কাঠামো curltime *pnow = ফাঁকা;
  VERBOSE(পূর্ণসংখ্যা i);

  যদি(bs->winner)
    ফেরত CURLE_OK;

evaluate:
  যদি(Curl_timeleft_now_ms(তথ্য, Curl_pgrs_now(তথ্য)) < 0) {
    failf(তথ্য, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(তথ্য, ফাঁকা, TIMER_STARTSINGLE));
    ফেরত CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  জন্য(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->ফলাফল = cf_ip_attempt_connect(a, তথ্য, connected);
    যদি(!a->ফলাফল) {
      যদি(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(তথ্য, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->পরবর্তী;
        a->পরবর্তী = ফাঁকা;
        যখন(bs->running) {
          a = bs->running;
          bs->running = a->পরবর্তী;
          cf_ip_attempt_free(a, তথ্য);
        }
        bs->running_লেজ = &bs->running;
        ফেরত CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->পরবর্তী;
    }
    অন্যথা যদি(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->পরবর্তী;
    }
    অন্যথা {
      /* hard failure, discard right away */
      *panchor = a->পরবর্তী;
      যদি(!a->পরবর্তী)
        bs->running_লেজ = panchor;
      a->পরবর্তী = ফাঁকা;
      bs->last_dead_ফলাফল = a->ফলাফল;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, তথ্য);
    }
  }
  যদি(bs->running)
    CURL_TRC_CF(তথ্য, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(তথ্য);
  যদি(!ongoing) {
    do_more = TRUE;
  }
  অন্যথা {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, তথ্য);
#ifdef USE_IPV6
    যদি(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, তথ্য);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    যদি(do_more)
      CURL_TRC_CF(তথ্য, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  যদি(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ধ্রুবক কাঠামো Curl_addrinfo *ai = ফাঁকা;
    পূর্ণসংখ্যা ai_family = 0;
    CURL_TRC_CF(তথ্য, cf, "want to do more");
#ifdef USE_IPV6
    যদি((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, তথ্য)) {
      ai = cf_ai_iter_পরবর্তী(&bs->ipv6_iter, তথ্য);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(তথ্য, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    যদি(!ai) {
      ai = cf_ai_iter_পরবর্তী(&bs->addr_iter, তথ্য);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(তথ্য, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(তথ্য);

    যদি(ai) {  /* try another address */
      কাঠামো Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      যদি(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, তথ্য, bs->max_concurrent - 1);

      ফলাফল = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      যদি(ফলাফল)
        যাও out;

      ফলাফল = cf_ip_attempt_new(&a, তথ্য, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(তথ্য, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (পূর্ণসংখ্যা)ফলাফল);
      যদি(ফলাফল)
        যাও out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_লেজ = a;
      a->পরবর্তী = ফাঁকা;
      bs->running_লেজ = &a->পরবর্তী;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      যাও evaluate;
    }
    অন্যথা যদি(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      যদি(delay_ms <= 0) {
        CURL_TRC_CF(তথ্য, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        জন্য(a = bs->running; a; a = a->পরবর্তী) {
          VERBOSE(++i);
          যদি(!a->inconclusive)
            চালু;
          ফলাফল = cf_ip_attempt_restart(a, cf, তথ্য);
          CURL_TRC_CF(তথ্য, cf, "restarted baller %d -> %d", i, (পূর্ণসংখ্যা)ফলাফল);
          যদি(ফলাফল) /* serious failure */
            যাও out;
          bs->last_attempt_started = *pnow;
          যাও evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      অন্যথা {
        /* let's wait some more before restarting */
        infof(তথ্য, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(তথ্য, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      যাও out;
    }
    অন্যথা যদি(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(তথ্য, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      ফলাফল = bs->had_dead ? bs->last_dead_ফলাফল : CURLE_COULDNT_CONNECT;
    }
  }

out:
  যদি(!ফলাফল) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(তথ্য);
    পরবর্তী_expire_ms = Curl_timeleft_now_ms(তথ্য, pnow);
    যদি(পরবর্তী_expire_ms < 0) {
      failf(তথ্য, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(তথ্য, ফাঁকা, TIMER_STARTSINGLE));
      ফেরত CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, তথ্য);
#ifdef USE_IPV6
    যদি(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, তথ্য);
#endif
    যদি(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      পরবর্তী_expire_ms = CURLMIN(পরবর্তী_expire_ms, expire_ms);
      যদি(পরবর্তী_expire_ms <= 0) {
        CURL_TRC_CF(তথ্য, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        যাও evaluate;
      }
      CURL_TRC_CF(তথ্য, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", পরবর্তী_expire_ms);
      Curl_expire_set(তথ্য, EXPIRE_HAPPY_EYEBALLS, পরবর্তী_expire_ms, pnow);
    }
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_ip_ballers_shutdown(কাঠামো cf_ip_ballers *bs,
                                       কাঠামো Curl_easy *তথ্য,
                                       bool *done)
{
  কাঠামো cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  জন্য(a = bs->running; a; a = a->পরবর্তী) {
    bool bdone = FALSE;
    যদি(a->shutdown || !a->cf)
      চালু;
    a->ফলাফল = a->cf->cft->do_shutdown(a->cf, তথ্য, &bdone);
    যদি(a->ফলাফল || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    অন্যথা
      *done = FALSE;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_ip_ballers_pollset(কাঠামো cf_ip_ballers *bs,
                                      কাঠামো Curl_easy *তথ্য,
                                      কাঠামো easy_pollset *ps)
{
  কাঠামো cf_ip_attempt *a;
  CURLcode ফলাফল = CURLE_OK;
  জন্য(a = bs->running; a && !ফলাফল; a = a->পরবর্তী) {
    যদি(a->ফলাফল)
      চালু;
    ফলাফল = Curl_conn_cf_adjust_pollset(a->cf, তথ্য, ps);
  }
  ফেরত ফলাফল;
}

স্থির bool cf_ip_ballers_pending(কাঠামো cf_ip_ballers *bs,
                                  ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_ip_attempt *a;

  জন্য(a = bs->running; a; a = a->পরবর্তী) {
    যদি(a->ফলাফল)
      চালু;
    যদি(a->cf && a->cf->cft->has_তথ্য_pending(a->cf, তথ্য))
      ফেরত TRUE;
  }
  ফেরত FALSE;
}

স্থির পূর্ণসংখ্যা cf_ip_ballers_min_reply_ms(কাঠামো cf_ip_ballers *bs,
                                      কাঠামো Curl_easy *তথ্য)
{
  পূর্ণসংখ্যা reply_ms = -1, breply_ms;
  কাঠামো cf_ip_attempt *a;

  জন্য(a = bs->running; a; a = a->পরবর্তী) {
    যদি(a->cf && !a->cf->cft->query(a->cf, তথ্য, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, ফাঁকা) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  ফেরত reply_ms;
}

প্রকারলেখ গণনা {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

কাঠামো cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  কাঠামো cf_ip_ballers ballers;
  BIT(dns_resolved);
};

স্থির CURLcode is_connected(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             bool *connected)
{
  কাঠামো cf_ip_happy_ctx *ctx = cf->ctx;
  কাঠামো connectdata *conn = cf->conn;
  CURLcode ফলাফল;

  ফলাফল = cf_ip_ballers_run(&ctx->ballers, cf, তথ্য,
                             (bool)ctx->dns_resolved, connected);

  যদি(!ফলাফল)
    ফেরত CURLE_OK;
  অন্যথা {
    কাঠামো Curl_peer *peer = ফাঁকা, *proxy_peer = ফাঁকা;
    অক্ষর viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    যদি(!conn->origin || !peer)
      ফেরত CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    যদি(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    অন্যথা যদি(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    যদি(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      যদি(peer->unix_socket)
        curl_msnprintf(viamsg, আকার(viamsg), " over unix://%s",
                       peer->hostname);
      অন্যথা
#endif
      curl_msnprintf(viamsg, আকার(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(তথ্য, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(তথ্য, ফাঁকা, TIMER_STARTSINGLE),
          curl_easy_strerror(ফলাফল));

#ifdef SOCKETIMEDOUT
    যদি(SOCKETIMEDOUT == তথ্য->state.os_errno)
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
#endif

    ফেরত ফলাফল;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

স্থির CURLcode cf_ip_happy_init(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_ip_happy_ctx *ctx = cf->ctx;

  যদি(Curl_timeleft_ms(তথ্য) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(তথ্য, "Connection time-out");
    ফেরত CURLE_OPERATION_TIMEDOUT;
  }

  যদি(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#অন্যথা
    ফেরত CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  অন্যথা { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(তথ্য, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  ফেরত CURLE_OK;
}

স্থির শূন্য cf_ip_happy_ctx_clear(কাঠামো cf_ip_happy_ctx *ctx,
                                  কাঠামো Curl_easy *তথ্য)
{
  DEBUGASSERT(ctx);
  যদি(ctx)
    cf_ip_ballers_clear(তথ্য, &ctx->ballers);
}

স্থির শূন্য cf_ip_happy_ctx_destroy(কাঠামো cf_ip_happy_ctx *ctx,
                                    কাঠামো Curl_easy *তথ্য)
{
  যদি(ctx) {
    cf_ip_happy_ctx_clear(ctx, তথ্য);
    curlx_free(ctx);
  }
}

স্থির CURLcode cf_ip_happy_shutdown(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য,
                                     bool *done)
{
  কাঠামো cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(তথ্য);
  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  ফলাফল = cf_ip_ballers_shutdown(&ctx->ballers, তথ্য, done);
  CURL_TRC_CF(তথ্য, cf, "shutdown -> %d, done=%d", (পূর্ণসংখ্যা)ফলাফল, *done);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_ip_happy_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                           কাঠামো Curl_easy *তথ্য,
                                           কাঠামো easy_pollset *ps)
{
  কাঠামো cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!cf->connected) {
    ফলাফল = cf_ip_ballers_pollset(&ctx->ballers, তথ্য, ps);
    CURL_TRC_CF(তথ্য, cf, "adjust_pollset -> %d, %u socks", (পূর্ণসংখ্যা)ফলাফল,
                ps->n);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_ip_happy_connect(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    bool *done)
{
  কাঠামো cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  যদি(!তথ্য)
    ফেরত CURLE_FAILED_INIT;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  যদি(!ctx->dns_resolved) {
    ফলাফল = Curl_conn_dns_addr_ফলাফল(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    যদি(!ফলাফল)
      ctx->dns_resolved = TRUE;
    অন্যথা যদি(ফলাফল == CURLE_AGAIN) {
      ফলাফল = CURLE_OK;
    }
    অন্যথা /* real error */
      যাও out;
  }

  নির্বাচন(ctx->state) {
  ক্ষেত্র SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, তথ্য));
    DEBUGASSERT(!cf->connected);
    ফলাফল = cf_ip_happy_init(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  ক্ষেত্র SCFST_WAITING:
    ফলাফল = is_connected(cf, তথ্য, done);
    যদি(!ফলাফল && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->পরবর্তী = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = ফাঁকা;

      যদি(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(তথ্য, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      যদি(Curl_trc_cf_is_verbose(cf, তথ্য)) {
        কাঠামো ip_quadruple ipquad;
        bool is_ipv6;
        যদি(!Curl_conn_cf_get_ip_info(cf->পরবর্তী, তথ্য, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(তথ্য, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_port);
        }
      }
#endif
      cf_ip_happy_ctx_clear(ctx, তথ্য);
      Curl_expire_clear(তথ্য, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(তথ্য);
      তথ্য->info.numconnects++; /* to track the # of connections made */
    }
    বিরতি;
  ক্ষেত্র SCFST_DONE:
    *done = TRUE;
    বিরতি;
  }
out:
  ফেরত ফলাফল;
}

স্থির bool cf_ip_happy_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                                     ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_ip_happy_ctx *ctx = cf->ctx;

  যদি(!cf->connected) {
    ফেরত cf_ip_ballers_pending(&ctx->ballers, তথ্য);
  }
  ফেরত cf->পরবর্তী->cft->has_তথ্য_pending(cf->পরবর্তী, তথ্য);
}

স্থির CURLcode cf_ip_happy_query(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো cf_ip_happy_ctx *ctx = cf->ctx;

  যদি(!cf->connected) {
    নির্বাচন(query) {
    ক্ষেত্র CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, তথ্য);
      CURL_TRC_CF(তথ্য, cf, "query connect reply: %dms", *pres1);
      ফেরত CURLE_OK;
    }
    পূর্বনির্ধারিত:
      বিরতি;
    }
  }

  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির শূন্য cf_ip_happy_destroy(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(তথ্য, cf, "destroy");
  যদি(ctx) {
    cf_ip_happy_ctx_clear(ctx, তথ্য);
    cf_ip_happy_ctx_destroy(ctx, তথ্য);
  }
}

কাঠামো Curl_cftype Curl_cft_ip_happy = {
  "HAPPY-EYEBALLS",
  CF_TYPE_SETUP,
  CURL_LOG_LVL_NONE,
  cf_ip_happy_destroy,
  cf_ip_happy_connect,
  cf_ip_happy_shutdown,
  cf_ip_happy_adjust_pollset,
  cf_ip_happy_তথ্য_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

স্থির CURLcode cf_ip_happy_create(কাঠামো Curl_cfilter **pcf,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো Curl_peer *origin,
                                   কাঠামো Curl_peer *peer,
                                   uint8_t transport_peer,
                                   কাঠামো connectdata *conn,
                                   কাঠামো Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  কাঠামো cf_ip_happy_ctx *ctx = ফাঁকা;
  CURLcode ফলাফল;

  (শূন্য)তথ্য;
  (শূন্য)conn;
  *pcf = ফাঁকা;
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  ফলাফল = cf_ip_ballers_init(&ctx->ballers, তথ্য,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              তথ্য->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  যদি(ফলাফল) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, তথ্য);
  }
  ফেরত ফলাফল;
}

CURLcode Curl_cf_ip_happy_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                       কাঠামো Curl_easy *তথ্য,
                                       কাঠামো Curl_peer *origin,
                                       কাঠামো Curl_peer *peer,
                                       uint8_t transport_peer,
                                       কাঠামো Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  ফলাফল = cf_ip_happy_create(&cf, তথ্য, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  Curl_conn_cf_ঢোকাও_after(cf_at, cf);
  ফেরত CURLE_OK;
}
