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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h> /* <netinet/tcp.h> may need it */
#اگر_ختم
#اگر_تعریف_شدہ HAVE_LINUX_TCP_H
#شامل <linux/tcp.h>
#وگرنہ_اگر defined(HAVE_NETINET_TCP_H)
#شامل <netinet/tcp.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم

#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#شامل "urldata.h"
#شامل "connect.h"
#شامل "cfilters.h"
#شامل "cf-ip-happy.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "multiif.h"
#شامل "progress.h"
#شامل "select.h"
#شامل "sockaddr.h"
#شامل "vdns/cf-dns.h"
#شامل "vquic/vquic.h" /* for quic cfilters */


ساخت transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

ساکن
#اگر_غیر_تعریف_شدہ UNITTESTS
ثابت
#اگر_ختم
ساخت transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#اگر_ختم
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
  { Curl_cf_h3_proxy_create, TRNSPRT_QUIC, TRUE },
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_TFTP
  { Curl_cf_udp_create, TRNSPRT_UDP, FALSE },
#اگر_ختم
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
  { Curl_cf_unix_create, TRNSPRT_UNIX, FALSE },
  { Curl_cf_unix_create, TRNSPRT_UNIX, TRUE },
#اگر_ختم
};

ساکن cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  برائے(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    اگر((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      واپس transport_providers[i].cf_create;
  }
  واپس تہی;
}

#اگر_تعریف_شدہ UNITTESTS
/* @unittest 2600 */
UNITTEST خالی debug_سیٹ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST خالی debug_سیٹ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  برائے(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    اگر(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#اگر_ختم /* UNITTESTS */

ساخت cf_ai_iter {
  ساخت Curl_cfilter *cf;
  ساخت Curl_peer *peer;
  صحیح ai_family;
  غیرعلامتی صحیح n;
};

ساکن خالی cf_ai_iter_init(ساخت cf_ai_iter *iter,
                            ساخت Curl_cfilter *cf,
                            ساخت Curl_peer *peer,
                            صحیح ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

ساکن ثابت ساخت Curl_addrinfo *cf_ai_iter_اگلا(ساخت cf_ai_iter *iter,
                                                   ساخت Curl_easy *ڈیٹا)
{
  ثابت ساخت Curl_addrinfo *addr;

  اگر(!iter->cf)
    واپس تہی;

  addr = Curl_conn_dns_get_ai(ڈیٹا, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  اگر(addr)
    iter->n++;
  واپس addr;
}

ساکن bool cf_ai_iter_has_مزید(ساخت cf_ai_iter *iter,
                                ساخت Curl_easy *ڈیٹا)
{
  واپس (iter->cf &&
          !!Curl_conn_dns_get_ai(ڈیٹا, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

ساخت cf_ip_attempt {
  ساخت cf_ip_attempt *اگلا;
  ساخت Curl_peer *origin;
  ساخت Curl_peer *peer;
  ساخت Curl_peer *tunnel_peer;
  ساخت Curl_sockaddr_ex addr;
  ساخت Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode نتیجہ;
  صحیح ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  صحیح error;
  ثنائی_ہندسہ(connected);                    /* cf has connected */
  ثنائی_ہندسہ(shutdown);                     /* cf has shutdown */
  ثنائی_ہندسہ(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

ساکن خالی cf_ip_attempt_free(ساخت cf_ip_attempt *a,
                               ساخت Curl_easy *ڈیٹا)
{
  اگر(a) {
    اگر(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, ڈیٹا);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

ساکن CURLcode cf_ip_attempt_new(ساخت cf_ip_attempt **pa,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت Curl_cfilter *cf,
                                  ساخت Curl_peer *origin,
                                  ساخت Curl_peer *peer,
                                  uint8_t transport_peer,
                                  ساخت Curl_sockaddr_ex *addr,
                                  صحیح ai_family,
                                  ساخت Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  ساخت Curl_cfilter *wcf;
  ساخت cf_ip_attempt *a;
  CURLcode نتیجہ = CURLE_OK;

  *pa = تہی;
  a = curlx_calloc(1, حجم(*a));
  اگر(!a)
    واپس CURLE_برآمد_کا_حافظہ;

  Curl_peer_link(&a->origin, origin);
  Curl_peer_link(&a->peer, peer);
  a->transport_peer = transport_peer;
  Curl_peer_link(&a->tunnel_peer, tunnel_peer);
  a->tunnel_transport = tunnel_transport;
  a->addr = *addr;
  a->ai_family = ai_family;
  a->نتیجہ = CURLE_OK;
  a->cf_create = cf_create;
  *pa = a;

  نتیجہ = a->cf_create(&a->cf, ڈیٹا, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* the new filter might have sub-filters */
  برائے(wcf = a->cf; wcf; wcf = wcf->اگلا) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

برآمد:
  اگر(نتیجہ) {
    cf_ip_attempt_free(a, ڈیٹا);
    *pa = تہی;
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_ip_attempt_connect(ساخت cf_ip_attempt *a,
                                      ساخت Curl_easy *ڈیٹا,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  اگر(!a->نتیجہ && !*connected) {
    /* evaluate again */
    a->نتیجہ = Curl_conn_cf_connect(a->cf, ڈیٹا, connected);

    اگر(!a->نتیجہ) {
      اگر(*connected) {
        a->connected = TRUE;
      }
    }
    ورنہ {
      اگر(a->نتیجہ == CURLE_WEIRD_SERVER_جواب)
        a->inconclusive = TRUE;
      اگر(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, ڈیٹا);
    }
  }
  واپس a->نتیجہ;
}

ساخت cf_ip_ballers {
  ساخت cf_ip_attempt *running;
  ساخت cf_ip_attempt **running_دم;  /* &running or &(last->next) */
  ساخت cf_ip_attempt *winner;
  ساخت cf_ai_iter addr_iter;
#اگر_تعریف_شدہ USE_IPV6
  ساخت cf_ai_iter ipv6_iter;
#اگر_ختم
  ساخت Curl_peer *origin;
  ساخت Curl_peer *peer;
  ساخت Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  ساخت curltime آخری_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode آخری_dead_نتیجہ;  /* result of last discarded hard failure */
  صحیح آخری_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  ثنائی_ہندسہ(had_dead);              /* a hard failure was discarded */
};

ساکن CURLcode cf_ip_attempt_restart(ساخت cf_ip_attempt *a,
                                      ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cfilter *wcf;
  CURLcode نتیجہ;

  اگر(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, ڈیٹا);

  a->نتیجہ = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = تہی;

  نتیجہ = a->cf_create(&a->cf, ڈیٹا, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  اگر(!نتیجہ) {
    bool dummy;
    /* the new filter might have sub-filters */
    برائے(wcf = a->cf; wcf; wcf = wcf->اگلا) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->نتیجہ = cf_ip_attempt_connect(a, ڈیٹا, &dummy);
  }
  واپس نتیجہ;
}

ساکن خالی cf_ip_ballers_clear(ساخت Curl_easy *ڈیٹا,
                                ساخت cf_ip_ballers *bs)
{
  جب(bs->running) {
    ساخت cf_ip_attempt *a = bs->running;
    bs->running = a->اگلا;
    cf_ip_attempt_free(a, ڈیٹا);
  }
  bs->running_دم = &bs->running;
  cf_ip_attempt_free(bs->winner, ڈیٹا);
  bs->winner = تہی;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

ساکن CURLcode cf_ip_ballers_init(ساخت cf_ip_ballers *bs,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت Curl_peer *origin,
                                   ساخت Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ساخت Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, حجم(*bs));
  bs->running_دم = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  اگر(!bs->cf_create) {
    failf(ڈیٹا, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    واپس CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->آخری_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  واپس CURLE_OK;
}

ساکن خالی cf_ip_ballers_prune(ساخت cf_ip_ballers *bs,
                                ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                uint32_t max_concurrent)
{
  ساخت cf_ip_attempt *a = تہی, **panchor;
  uint32_t ongoing = 0;

  برائے(a = bs->running; a; a = a->اگلا) {
    اگر(!a->نتیجہ && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  جب(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    اگر(!a->نتیجہ && !a->connected) {
      *panchor = a->اگلا;
      اگر(!a->اگلا)
        bs->running_دم = panchor;
      a->اگلا = تہی;
      cf_ip_attempt_free(a, ڈیٹا);
      --ongoing;
      CURL_TRC_CF(ڈیٹا, cf, "discarding oldest attempt to keep limit");
    }
    ورنہ {
      panchor = &a->اگلا;
    }
  }
}

ساکن CURLcode cf_ip_ballers_تسلسل(ساخت cf_ip_ballers *bs,
                                  ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت cf_ip_attempt *a = تہی, **panchor;
  bool do_مزید;
  timediff_t اگلا_expire_ms;
  uint32_t inconclusive, ongoing;
  ثابت ساخت curltime *pnow = تہی;
  VERBOSE(صحیح i);

  اگر(bs->winner)
    واپس CURLE_OK;

evaluate:
  اگر(Curl_timeleft_now_ms(ڈیٹا, Curl_pgrs_now(ڈیٹا)) < 0) {
    failf(ڈیٹا, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(ڈیٹا, تہی, TIMER_STARTSINGLE));
    واپس CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  برائے(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->نتیجہ = cf_ip_attempt_connect(a, ڈیٹا, connected);
    اگر(!a->نتیجہ) {
      اگر(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(ڈیٹا, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->اگلا;
        a->اگلا = تہی;
        جب(bs->running) {
          a = bs->running;
          bs->running = a->اگلا;
          cf_ip_attempt_free(a, ڈیٹا);
        }
        bs->running_دم = &bs->running;
        واپس CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->اگلا;
    }
    ورنہ اگر(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->اگلا;
    }
    ورنہ {
      /* hard failure, discard right away */
      *panchor = a->اگلا;
      اگر(!a->اگلا)
        bs->running_دم = panchor;
      a->اگلا = تہی;
      bs->آخری_dead_نتیجہ = a->نتیجہ;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, ڈیٹا);
    }
  }
  اگر(bs->running)
    CURL_TRC_CF(ڈیٹا, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(ڈیٹا);
  اگر(!ongoing) {
    do_مزید = TRUE;
  }
  ورنہ {
    bool مزید_possible = cf_ai_iter_has_مزید(&bs->addr_iter, ڈیٹا);
#اگر_تعریف_شدہ USE_IPV6
    اگر(!مزید_possible)
      مزید_possible = cf_ai_iter_has_مزید(&bs->ipv6_iter, ڈیٹا);
#اگر_ختم
    do_مزید = مزید_possible &&
      (curlx_ptimediff_ms(pnow, &bs->آخری_attempt_started) >=
       bs->attempt_delay_ms);
    اگر(do_مزید)
      CURL_TRC_CF(ڈیٹا, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  اگر(do_مزید) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ثابت ساخت Curl_addrinfo *ai = تہی;
    صحیح ai_family = 0;
    CURL_TRC_CF(ڈیٹا, cf, "want to do more");
#اگر_تعریف_شدہ USE_IPV6
    اگر((bs->آخری_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_مزید(&bs->addr_iter, ڈیٹا)) {
      ai = cf_ai_iter_اگلا(&bs->ipv6_iter, ڈیٹا);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(ڈیٹا, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#اگر_ختم
    اگر(!ai) {
      ai = cf_ai_iter_اگلا(&bs->addr_iter, ڈیٹا);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(ڈیٹا, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(ڈیٹا);

    اگر(ai) {  /* try another address */
      ساخت Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      اگر(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, ڈیٹا, bs->max_concurrent - 1);

      نتیجہ = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      اگر(نتیجہ)
        جاؤ برآمد;

      نتیجہ = cf_ip_attempt_new(&a, ڈیٹا, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(ڈیٹا, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (صحیح)نتیجہ);
      اگر(نتیجہ)
        جاؤ برآمد;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_دم = a;
      a->اگلا = تہی;
      bs->running_دم = &a->اگلا;
      bs->آخری_attempt_started = *pnow;
      bs->آخری_attempt_ai_family = ai_family;
      /* and run everything again */
      جاؤ evaluate;
    }
    ورنہ اگر(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->آخری_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      اگر(delay_ms <= 0) {
        CURL_TRC_CF(ڈیٹا, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        برائے(a = bs->running; a; a = a->اگلا) {
          VERBOSE(++i);
          اگر(!a->inconclusive)
            جاری;
          نتیجہ = cf_ip_attempt_restart(a, cf, ڈیٹا);
          CURL_TRC_CF(ڈیٹا, cf, "restarted baller %d -> %d", i, (صحیح)نتیجہ);
          اگر(نتیجہ) /* serious failure */
            جاؤ برآمد;
          bs->آخری_attempt_started = *pnow;
          جاؤ evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      ورنہ {
        /* let's wait some more before restarting */
        infof(ڈیٹا, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_سیٹ(ڈیٹا, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      جاؤ برآمد;
    }
    ورنہ اگر(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(ڈیٹا, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      نتیجہ = bs->had_dead ? bs->آخری_dead_نتیجہ : CURLE_COULDNT_CONNECT;
    }
  }

برآمد:
  اگر(!نتیجہ) {
    bool مزید_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(ڈیٹا);
    اگلا_expire_ms = Curl_timeleft_now_ms(ڈیٹا, pnow);
    اگر(اگلا_expire_ms < 0) {
      failf(ڈیٹا, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(ڈیٹا, تہی, TIMER_STARTSINGLE));
      واپس CURLE_OPERATION_TIMEDOUT;
    }

    مزید_possible = cf_ai_iter_has_مزید(&bs->addr_iter, ڈیٹا);
#اگر_تعریف_شدہ USE_IPV6
    اگر(!مزید_possible)
      مزید_possible = cf_ai_iter_has_مزید(&bs->ipv6_iter, ڈیٹا);
#اگر_ختم
    اگر(مزید_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->آخری_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      اگلا_expire_ms = CURLMIN(اگلا_expire_ms, expire_ms);
      اگر(اگلا_expire_ms <= 0) {
        CURL_TRC_CF(ڈیٹا, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        جاؤ evaluate;
      }
      CURL_TRC_CF(ڈیٹا, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", اگلا_expire_ms);
      Curl_expire_سیٹ(ڈیٹا, EXPIRE_HAPPY_EYEBALLS, اگلا_expire_ms, pnow);
    }
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_ip_ballers_shutdown(ساخت cf_ip_ballers *bs,
                                       ساخت Curl_easy *ڈیٹا,
                                       bool *مکمل)
{
  ساخت cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *مکمل = TRUE;
  برائے(a = bs->running; a; a = a->اگلا) {
    bool bdone = FALSE;
    اگر(a->shutdown || !a->cf)
      جاری;
    a->نتیجہ = a->cf->cft->do_shutdown(a->cf, ڈیٹا, &bdone);
    اگر(a->نتیجہ || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    ورنہ
      *مکمل = FALSE;
  }
  واپس CURLE_OK;
}

ساکن CURLcode cf_ip_ballers_pollset(ساخت cf_ip_ballers *bs,
                                      ساخت Curl_easy *ڈیٹا,
                                      ساخت easy_pollset *ps)
{
  ساخت cf_ip_attempt *a;
  CURLcode نتیجہ = CURLE_OK;
  برائے(a = bs->running; a && !نتیجہ; a = a->اگلا) {
    اگر(a->نتیجہ)
      جاری;
    نتیجہ = Curl_conn_cf_adjust_pollset(a->cf, ڈیٹا, ps);
  }
  واپس نتیجہ;
}

ساکن bool cf_ip_ballers_pending(ساخت cf_ip_ballers *bs,
                                  ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_ip_attempt *a;

  برائے(a = bs->running; a; a = a->اگلا) {
    اگر(a->نتیجہ)
      جاری;
    اگر(a->cf && a->cf->cft->has_ڈیٹا_pending(a->cf, ڈیٹا))
      واپس TRUE;
  }
  واپس FALSE;
}

ساکن صحیح cf_ip_ballers_min_جواب_ms(ساخت cf_ip_ballers *bs,
                                      ساخت Curl_easy *ڈیٹا)
{
  صحیح جواب_ms = -1, breply_ms;
  ساخت cf_ip_attempt *a;

  برائے(a = bs->running; a; a = a->اگلا) {
    اگر(a->cf && !a->cf->cft->query(a->cf, ڈیٹا, CF_QUERY_CONNECT_جواب_MS,
                                   &breply_ms, تہی) &&
       breply_ms >= 0 && (جواب_ms < 0 || breply_ms < جواب_ms))
      جواب_ms = breply_ms;
  }
  واپس جواب_ms;
}

قسمتعریف شمار {
  SCFST_INIT,
  SCFST_انتظار,
  SCFST_مکمل
} cf_connect_state;

ساخت cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  ساخت cf_ip_ballers ballers;
  ثنائی_ہندسہ(dns_resolved);
};

ساکن CURLcode is_connected(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             bool *connected)
{
  ساخت cf_ip_happy_ctx *ctx = cf->ctx;
  ساخت connectdata *conn = cf->conn;
  CURLcode نتیجہ;

  نتیجہ = cf_ip_ballers_تسلسل(&ctx->ballers, cf, ڈیٹا,
                             (bool)ctx->dns_resolved, connected);

  اگر(!نتیجہ)
    واپس CURLE_OK;
  ورنہ {
    ساخت Curl_peer *peer = تہی, *proxy_peer = تہی;
    حرف viamsg[160];

    peer = Curl_conn_get_پہلا_peer(conn, cf->sockindex);
    اگر(!conn->origin || !peer)
      واپس CURLE_FAILED_INIT;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    اگر(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    ورنہ اگر(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#اگر_ختم

    viamsg[0] = 0;
    اگر(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
      اگر(peer->unix_socket)
        curl_msnprintf(viamsg, حجم(viamsg), " over unix://%s",
                       peer->hostname);
      ورنہ
#اگر_ختم
      curl_msnprintf(viamsg, حجم(viamsg), " via %s:%u",
                     peer->hostname, peer->پورٹ);
    }

    failf(ڈیٹا, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->پورٹ, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(ڈیٹا, تہی, TIMER_STARTSINGLE),
          curl_easy_strerror(نتیجہ));

#اگر_تعریف_شدہ SOCKETIMEDOUT
    اگر(SOCKETIMEDOUT == ڈیٹا->state.os_errno)
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
#اگر_ختم

    واپس نتیجہ;
  }
}

#تعریف_کرو IP_HE_MAX_CONCURRENT_ATTEMPTS     6

ساکن CURLcode cf_ip_happy_init(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_ip_happy_ctx *ctx = cf->ctx;

  اگر(Curl_timeleft_ms(ڈیٹا) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(ڈیٹا, "Connection time-out");
    واپس CURLE_OPERATION_TIMEDOUT;
  }

  اگر(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#ورنہ
    واپس CURLE_UNSUPPORTED_PROTOCOL;
#اگر_ختم
  }
  ورنہ { /* TCP/UDP/QUIC */
#اگر_تعریف_شدہ USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#اگر_ختم
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(ڈیٹا, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  واپس CURLE_OK;
}

ساکن خالی cf_ip_happy_ctx_clear(ساخت cf_ip_happy_ctx *ctx,
                                  ساخت Curl_easy *ڈیٹا)
{
  DEBUGASSERT(ctx);
  اگر(ctx)
    cf_ip_ballers_clear(ڈیٹا, &ctx->ballers);
}

ساکن خالی cf_ip_happy_ctx_destroy(ساخت cf_ip_happy_ctx *ctx,
                                    ساخت Curl_easy *ڈیٹا)
{
  اگر(ctx) {
    cf_ip_happy_ctx_clear(ctx, ڈیٹا);
    curlx_free(ctx);
  }
}

ساکن CURLcode cf_ip_happy_shutdown(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا,
                                     bool *مکمل)
{
  ساخت cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(ڈیٹا);
  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  نتیجہ = cf_ip_ballers_shutdown(&ctx->ballers, ڈیٹا, مکمل);
  CURL_TRC_CF(ڈیٹا, cf, "shutdown -> %d, done=%d", (صحیح)نتیجہ, *مکمل);
  واپس نتیجہ;
}

ساکن CURLcode cf_ip_happy_adjust_pollset(ساخت Curl_cfilter *cf,
                                           ساخت Curl_easy *ڈیٹا,
                                           ساخت easy_pollset *ps)
{
  ساخت cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!cf->connected) {
    نتیجہ = cf_ip_ballers_pollset(&ctx->ballers, ڈیٹا, ps);
    CURL_TRC_CF(ڈیٹا, cf, "adjust_pollset -> %d, %u socks", (صحیح)نتیجہ,
                ps->n);
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_ip_happy_connect(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    bool *مکمل)
{
  ساخت cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  اگر(!ڈیٹا)
    واپس CURLE_FAILED_INIT;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *مکمل = FALSE;

  اگر(!ctx->dns_resolved) {
    نتیجہ = Curl_conn_dns_addr_نتیجہ(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    اگر(!نتیجہ)
      ctx->dns_resolved = TRUE;
    ورنہ اگر(نتیجہ == CURLE_AGAIN) {
      نتیجہ = CURLE_OK;
    }
    ورنہ /* real error */
      جاؤ برآمد;
  }

  انتخاب(ctx->state) {
  حالت SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, ڈیٹا));
    DEBUGASSERT(!cf->connected);
    نتیجہ = cf_ip_happy_init(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
    ctx->state = SCFST_انتظار;
    FALLTHROUGH();
  حالت SCFST_انتظار:
    نتیجہ = is_connected(cf, ڈیٹا, مکمل);
    اگر(!نتیجہ && *مکمل) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_مکمل;
      cf->connected = TRUE;
      cf->اگلا = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = تہی;

      اگر(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrs_وقت(ڈیٹا, TIMER_APPCONNECT); /* we are connected already */
#اگر_تعریف_شدہ CURLVERBOSE
      اگر(Curl_trc_cf_is_verbose(cf, ڈیٹا)) {
        ساخت ip_quadruple ipquad;
        bool is_ipv6;
        اگر(!Curl_conn_cf_get_ip_info(cf->اگلا, ڈیٹا, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(ڈیٹا, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_پورٹ);
        }
      }
#اگر_ختم
      cf_ip_happy_ctx_clear(ctx, ڈیٹا);
      Curl_expire_clear(ڈیٹا, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(ڈیٹا);
      ڈیٹا->info.numconnects++; /* to track the # of connections made */
    }
    توڑو;
  حالت SCFST_مکمل:
    *مکمل = TRUE;
    توڑو;
  }
برآمد:
  واپس نتیجہ;
}

ساکن bool cf_ip_happy_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                                     ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_ip_happy_ctx *ctx = cf->ctx;

  اگر(!cf->connected) {
    واپس cf_ip_ballers_pending(&ctx->ballers, ڈیٹا);
  }
  واپس cf->اگلا->cft->has_ڈیٹا_pending(cf->اگلا, ڈیٹا);
}

ساکن CURLcode cf_ip_happy_query(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت cf_ip_happy_ctx *ctx = cf->ctx;

  اگر(!cf->connected) {
    انتخاب(query) {
    حالت CF_QUERY_CONNECT_جواب_MS: {
      *pres1 = cf_ip_ballers_min_جواب_ms(&ctx->ballers, ڈیٹا);
      CURL_TRC_CF(ڈیٹا, cf, "query connect reply: %dms", *pres1);
      واپس CURLE_OK;
    }
    طےشدہ:
      توڑو;
    }
  }

  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساکن خالی cf_ip_happy_destroy(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ڈیٹا, cf, "destroy");
  اگر(ctx) {
    cf_ip_happy_ctx_clear(ctx, ڈیٹا);
    cf_ip_happy_ctx_destroy(ctx, ڈیٹا);
  }
}

ساخت Curl_cftype Curl_cft_ip_happy = {
  "HAPPY-EYEBALLS",
  CF_TYPE_تیاری,
  CURL_LOG_LVL_NONE,
  cf_ip_happy_destroy,
  cf_ip_happy_connect,
  cf_ip_happy_shutdown,
  cf_ip_happy_adjust_pollset,
  cf_ip_happy_ڈیٹا_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

ساکن CURLcode cf_ip_happy_create(ساخت Curl_cfilter **pcf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت Curl_peer *origin,
                                   ساخت Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ساخت connectdata *conn,
                                   ساخت Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  ساخت cf_ip_happy_ctx *ctx = تہی;
  CURLcode نتیجہ;

  (خالی)ڈیٹا;
  (خالی)conn;
  *pcf = تہی;
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  نتیجہ = cf_ip_ballers_init(&ctx->ballers, ڈیٹا,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              ڈیٹا->سیٹ.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

برآمد:
  اگر(نتیجہ) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, ڈیٹا);
  }
  واپس نتیجہ;
}

CURLcode Curl_cf_ip_happy_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                       ساخت Curl_easy *ڈیٹا,
                                       ساخت Curl_peer *origin,
                                       ساخت Curl_peer *peer,
                                       uint8_t transport_peer,
                                       ساخت Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  نتیجہ = cf_ip_happy_create(&cf, ڈیٹا, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  اگر(نتیجہ)
    واپس نتیجہ;

  Curl_conn_cf_داخل_کرو_after(cf_at, cf);
  واپس CURLE_OK;
}
