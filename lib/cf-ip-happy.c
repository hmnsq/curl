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

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h> /* <netinet/tcp.h> may need it */
#جے_انت
#جے_تعریف_شدہ HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#جے_انت
#جے_تعریف_شدہ HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت

#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

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
#جے_بنا_تعریف UNITTESTS
ثابت
#جے_انت
بنية transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#جے_انت
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
  { Curl_cf_h3_proxy_create, TRNSPRT_QUIC, TRUE },
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_TFTP
  { Curl_cf_udp_create, TRNSPRT_UDP, FALSE },
#جے_انت
#جے_تعریف_شدہ USE_UNIX_SOCKETS
  { Curl_cf_unix_create, TRNSPRT_UNIX, FALSE },
  { Curl_cf_unix_create, TRNSPRT_UNIX, TRUE },
#جے_انت
};

ساكن cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    إذا((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      أرجع transport_providers[i].cf_create;
  }
  أرجع عدم;
}

#جے_تعریف_شدہ UNITTESTS
/* @unittest 2600 */
UNITTEST فراغ debug_سیٹ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST فراغ debug_سیٹ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    إذا(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#جے_انت /* UNITTESTS */

بنية cf_ai_iter {
  بنية Curl_cfilter *cf;
  بنية Curl_peer *peer;
  صحيح ai_family;
  بنانشانی صحيح n;
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

ساكن ثابت بنية Curl_addrinfo *cf_ai_iter_اگلا(بنية cf_ai_iter *iter,
                                                   بنية Curl_easy *انکڑے)
{
  ثابت بنية Curl_addrinfo *addr;

  إذا(!iter->cf)
    أرجع عدم;

  addr = Curl_conn_dns_get_ai(انکڑے, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  إذا(addr)
    iter->n++;
  أرجع addr;
}

ساكن bool cf_ai_iter_has_ہور(بنية cf_ai_iter *iter,
                                بنية Curl_easy *انکڑے)
{
  أرجع (iter->cf &&
          !!Curl_conn_dns_get_ai(انکڑے, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

بنية cf_ip_attempt {
  بنية cf_ip_attempt *اگلا;
  بنية Curl_peer *origin;
  بنية Curl_peer *peer;
  بنية Curl_peer *tunnel_peer;
  بنية Curl_sockaddr_ex addr;
  بنية Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode نتیجہ;
  صحيح ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  صحيح error;
  ثنائی_ہندسہ(connected);                    /* cf has connected */
  ثنائی_ہندسہ(shutdown);                     /* cf has shutdown */
  ثنائی_ہندسہ(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

ساكن فراغ cf_ip_attempt_free(بنية cf_ip_attempt *a,
                               بنية Curl_easy *انکڑے)
{
  إذا(a) {
    إذا(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, انکڑے);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

ساكن CURLcode cf_ip_attempt_new(بنية cf_ip_attempt **pa,
                                  بنية Curl_easy *انکڑے,
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
  CURLcode نتیجہ = CURLE_OK;

  *pa = عدم;
  a = curlx_calloc(1, حجم(*a));
  إذا(!a)
    أرجع CURLE_باہرلا_دا_حافظہ;

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

  نتیجہ = a->cf_create(&a->cf, انکڑے, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* the new filter might have sub-filters */
  لكل(wcf = a->cf; wcf; wcf = wcf->اگلا) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

باہرلا:
  إذا(نتیجہ) {
    cf_ip_attempt_free(a, انکڑے);
    *pa = عدم;
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_ip_attempt_connect(بنية cf_ip_attempt *a,
                                      بنية Curl_easy *انکڑے,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  إذا(!a->نتیجہ && !*connected) {
    /* evaluate again */
    a->نتیجہ = Curl_conn_cf_connect(a->cf, انکڑے, connected);

    إذا(!a->نتیجہ) {
      إذا(*connected) {
        a->connected = TRUE;
      }
    }
    وإلا {
      إذا(a->نتیجہ == CURLE_WEIRD_SERVER_جواب)
        a->inconclusive = TRUE;
      إذا(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, انکڑے);
    }
  }
  أرجع a->نتیجہ;
}

بنية cf_ip_ballers {
  بنية cf_ip_attempt *running;
  بنية cf_ip_attempt **running_پوچھ;  /* &running or &(last->next) */
  بنية cf_ip_attempt *winner;
  بنية cf_ai_iter addr_iter;
#جے_تعریف_شدہ USE_IPV6
  بنية cf_ai_iter ipv6_iter;
#جے_انت
  بنية Curl_peer *origin;
  بنية Curl_peer *peer;
  بنية Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  بنية curltime آخری_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode آخری_dead_نتیجہ;  /* result of last discarded hard failure */
  صحيح آخری_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  ثنائی_ہندسہ(had_dead);              /* a hard failure was discarded */
};

ساكن CURLcode cf_ip_attempt_restart(بنية cf_ip_attempt *a,
                                      بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے)
{
  بنية Curl_cfilter *wcf;
  CURLcode نتیجہ;

  إذا(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, انکڑے);

  a->نتیجہ = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = عدم;

  نتیجہ = a->cf_create(&a->cf, انکڑے, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  إذا(!نتیجہ) {
    bool dummy;
    /* the new filter might have sub-filters */
    لكل(wcf = a->cf; wcf; wcf = wcf->اگلا) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->نتیجہ = cf_ip_attempt_connect(a, انکڑے, &dummy);
  }
  أرجع نتیجہ;
}

ساكن فراغ cf_ip_ballers_clear(بنية Curl_easy *انکڑے,
                                بنية cf_ip_ballers *bs)
{
  طالما(bs->running) {
    بنية cf_ip_attempt *a = bs->running;
    bs->running = a->اگلا;
    cf_ip_attempt_free(a, انکڑے);
  }
  bs->running_پوچھ = &bs->running;
  cf_ip_attempt_free(bs->winner, انکڑے);
  bs->winner = عدم;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

ساكن CURLcode cf_ip_ballers_init(بنية cf_ip_ballers *bs,
                                   بنية Curl_easy *انکڑے,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   uint8_t transport_peer,
                                   بنية Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, حجم(*bs));
  bs->running_پوچھ = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  إذا(!bs->cf_create) {
    failf(انکڑے, "unsupported transport type %u%s",
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
  bs->آخری_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  أرجع CURLE_OK;
}

ساكن فراغ cf_ip_ballers_prune(بنية cf_ip_ballers *bs,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                uint32_t max_concurrent)
{
  بنية cf_ip_attempt *a = عدم, **panchor;
  uint32_t ongoing = 0;

  لكل(a = bs->running; a; a = a->اگلا) {
    إذا(!a->نتیجہ && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  طالما(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    إذا(!a->نتیجہ && !a->connected) {
      *panchor = a->اگلا;
      إذا(!a->اگلا)
        bs->running_پوچھ = panchor;
      a->اگلا = عدم;
      cf_ip_attempt_free(a, انکڑے);
      --ongoing;
      CURL_TRC_CF(انکڑے, cf, "discarding oldest attempt to keep limit");
    }
    وإلا {
      panchor = &a->اگلا;
    }
  }
}

ساكن CURLcode cf_ip_ballers_لگاتار(بنية cf_ip_ballers *bs,
                                  بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية cf_ip_attempt *a = عدم, **panchor;
  bool do_ہور;
  timediff_t اگلا_expire_ms;
  uint32_t inconclusive, ongoing;
  ثابت بنية curltime *pnow = عدم;
  VERBOSE(صحيح i);

  إذا(bs->winner)
    أرجع CURLE_OK;

evaluate:
  إذا(Curl_timeleft_now_ms(انکڑے, Curl_pgrs_now(انکڑے)) < 0) {
    failf(انکڑے, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(انکڑے, عدم, TIMER_STARTSINGLE));
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  لكل(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->نتیجہ = cf_ip_attempt_connect(a, انکڑے, connected);
    إذا(!a->نتیجہ) {
      إذا(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(انکڑے, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->اگلا;
        a->اگلا = عدم;
        طالما(bs->running) {
          a = bs->running;
          bs->running = a->اگلا;
          cf_ip_attempt_free(a, انکڑے);
        }
        bs->running_پوچھ = &bs->running;
        أرجع CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->اگلا;
    }
    وإلا إذا(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->اگلا;
    }
    وإلا {
      /* hard failure, discard right away */
      *panchor = a->اگلا;
      إذا(!a->اگلا)
        bs->running_پوچھ = panchor;
      a->اگلا = عدم;
      bs->آخری_dead_نتیجہ = a->نتیجہ;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, انکڑے);
    }
  }
  إذا(bs->running)
    CURL_TRC_CF(انکڑے, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(انکڑے);
  إذا(!ongoing) {
    do_ہور = TRUE;
  }
  وإلا {
    bool ہور_possible = cf_ai_iter_has_ہور(&bs->addr_iter, انکڑے);
#جے_تعریف_شدہ USE_IPV6
    إذا(!ہور_possible)
      ہور_possible = cf_ai_iter_has_ہور(&bs->ipv6_iter, انکڑے);
#جے_انت
    do_ہور = ہور_possible &&
      (curlx_ptimediff_ms(pnow, &bs->آخری_attempt_started) >=
       bs->attempt_delay_ms);
    إذا(do_ہور)
      CURL_TRC_CF(انکڑے, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  إذا(do_ہور) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ثابت بنية Curl_addrinfo *ai = عدم;
    صحيح ai_family = 0;
    CURL_TRC_CF(انکڑے, cf, "want to do more");
#جے_تعریف_شدہ USE_IPV6
    إذا((bs->آخری_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_ہور(&bs->addr_iter, انکڑے)) {
      ai = cf_ai_iter_اگلا(&bs->ipv6_iter, انکڑے);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(انکڑے, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#جے_انت
    إذا(!ai) {
      ai = cf_ai_iter_اگلا(&bs->addr_iter, انکڑے);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(انکڑے, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(انکڑے);

    إذا(ai) {  /* try another address */
      بنية Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      إذا(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, انکڑے, bs->max_concurrent - 1);

      نتیجہ = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      إذا(نتیجہ)
        اذهب باہرلا;

      نتیجہ = cf_ip_attempt_new(&a, انکڑے, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(انکڑے, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (صحيح)نتیجہ);
      إذا(نتیجہ)
        اذهب باہرلا;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_پوچھ = a;
      a->اگلا = عدم;
      bs->running_پوچھ = &a->اگلا;
      bs->آخری_attempt_started = *pnow;
      bs->آخری_attempt_ai_family = ai_family;
      /* and run everything again */
      اذهب evaluate;
    }
    وإلا إذا(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->آخری_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      إذا(delay_ms <= 0) {
        CURL_TRC_CF(انکڑے, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        لكل(a = bs->running; a; a = a->اگلا) {
          VERBOSE(++i);
          إذا(!a->inconclusive)
            استمر;
          نتیجہ = cf_ip_attempt_restart(a, cf, انکڑے);
          CURL_TRC_CF(انکڑے, cf, "restarted baller %d -> %d", i, (صحيح)نتیجہ);
          إذا(نتیجہ) /* serious failure */
            اذهب باہرلا;
          bs->آخری_attempt_started = *pnow;
          اذهب evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      وإلا {
        /* let's wait some more before restarting */
        infof(انکڑے, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_سیٹ(انکڑے, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      اذهب باہرلا;
    }
    وإلا إذا(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(انکڑے, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      نتیجہ = bs->had_dead ? bs->آخری_dead_نتیجہ : CURLE_COULDNT_CONNECT;
    }
  }

باہرلا:
  إذا(!نتیجہ) {
    bool ہور_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(انکڑے);
    اگلا_expire_ms = Curl_timeleft_now_ms(انکڑے, pnow);
    إذا(اگلا_expire_ms < 0) {
      failf(انکڑے, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(انکڑے, عدم, TIMER_STARTSINGLE));
      أرجع CURLE_OPERATION_TIMEDOUT;
    }

    ہور_possible = cf_ai_iter_has_ہور(&bs->addr_iter, انکڑے);
#جے_تعریف_شدہ USE_IPV6
    إذا(!ہور_possible)
      ہور_possible = cf_ai_iter_has_ہور(&bs->ipv6_iter, انکڑے);
#جے_انت
    إذا(ہور_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->آخری_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      اگلا_expire_ms = CURLMIN(اگلا_expire_ms, expire_ms);
      إذا(اگلا_expire_ms <= 0) {
        CURL_TRC_CF(انکڑے, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        اذهب evaluate;
      }
      CURL_TRC_CF(انکڑے, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", اگلا_expire_ms);
      Curl_expire_سیٹ(انکڑے, EXPIRE_HAPPY_EYEBALLS, اگلا_expire_ms, pnow);
    }
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_ip_ballers_shutdown(بنية cf_ip_ballers *bs,
                                       بنية Curl_easy *انکڑے,
                                       bool *پورا)
{
  بنية cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *پورا = TRUE;
  لكل(a = bs->running; a; a = a->اگلا) {
    bool bdone = FALSE;
    إذا(a->shutdown || !a->cf)
      استمر;
    a->نتیجہ = a->cf->cft->do_shutdown(a->cf, انکڑے, &bdone);
    إذا(a->نتیجہ || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    وإلا
      *پورا = FALSE;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ip_ballers_pollset(بنية cf_ip_ballers *bs,
                                      بنية Curl_easy *انکڑے,
                                      بنية easy_pollset *ps)
{
  بنية cf_ip_attempt *a;
  CURLcode نتیجہ = CURLE_OK;
  لكل(a = bs->running; a && !نتیجہ; a = a->اگلا) {
    إذا(a->نتیجہ)
      استمر;
    نتیجہ = Curl_conn_cf_adjust_pollset(a->cf, انکڑے, ps);
  }
  أرجع نتیجہ;
}

ساكن bool cf_ip_ballers_pending(بنية cf_ip_ballers *bs,
                                  ثابت بنية Curl_easy *انکڑے)
{
  بنية cf_ip_attempt *a;

  لكل(a = bs->running; a; a = a->اگلا) {
    إذا(a->نتیجہ)
      استمر;
    إذا(a->cf && a->cf->cft->has_انکڑے_pending(a->cf, انکڑے))
      أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن صحيح cf_ip_ballers_min_جواب_ms(بنية cf_ip_ballers *bs,
                                      بنية Curl_easy *انکڑے)
{
  صحيح جواب_ms = -1, breply_ms;
  بنية cf_ip_attempt *a;

  لكل(a = bs->running; a; a = a->اگلا) {
    إذا(a->cf && !a->cf->cft->query(a->cf, انکڑے, CF_QUERY_CONNECT_جواب_MS,
                                   &breply_ms, عدم) &&
       breply_ms >= 0 && (جواب_ms < 0 || breply_ms < جواب_ms))
      جواب_ms = breply_ms;
  }
  أرجع جواب_ms;
}

تعريفنوع تعداد {
  SCFST_INIT,
  SCFST_اڈیک,
  SCFST_پورا
} cf_connect_state;

بنية cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  بنية cf_ip_ballers ballers;
  ثنائی_ہندسہ(dns_resolved);
};

ساكن CURLcode is_connected(بنية Curl_cfilter *cf,
                             بنية Curl_easy *انکڑے,
                             bool *connected)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  بنية connectdata *conn = cf->conn;
  CURLcode نتیجہ;

  نتیجہ = cf_ip_ballers_لگاتار(&ctx->ballers, cf, انکڑے,
                             (bool)ctx->dns_resolved, connected);

  إذا(!نتیجہ)
    أرجع CURLE_OK;
  وإلا {
    بنية Curl_peer *peer = عدم, *proxy_peer = عدم;
    حرف viamsg[160];

    peer = Curl_conn_get_پہلا_peer(conn, cf->sockindex);
    إذا(!conn->origin || !peer)
      أرجع CURLE_FAILED_INIT;

#جے_بنا_تعریف CURL_DISABLE_PROXY
    إذا(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    وإلا إذا(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#جے_انت

    viamsg[0] = 0;
    إذا(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#جے_تعریف_شدہ USE_UNIX_SOCKETS
      إذا(peer->unix_socket)
        curl_msnprintf(viamsg, حجم(viamsg), " over unix://%s",
                       peer->hostname);
      وإلا
#جے_انت
      curl_msnprintf(viamsg, حجم(viamsg), " via %s:%u",
                     peer->hostname, peer->پورٹ);
    }

    failf(انکڑے, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->پورٹ, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(انکڑے, عدم, TIMER_STARTSINGLE),
          curl_easy_strerror(نتیجہ));

#جے_تعریف_شدہ SOCKETIMEDOUT
    إذا(SOCKETIMEDOUT == انکڑے->state.os_errno)
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
#جے_انت

    أرجع نتیجہ;
  }
}

#تعریف_کرو IP_HE_MAX_CONCURRENT_ATTEMPTS     6

ساكن CURLcode cf_ip_happy_init(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  إذا(Curl_timeleft_ms(انکڑے) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(انکڑے, "Connection time-out");
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  إذا(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#جے_تعریف_شدہ USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#وإلا
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
#جے_انت
  }
  وإلا { /* TCP/UDP/QUIC */
#جے_تعریف_شدہ USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#جے_انت
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(انکڑے, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  أرجع CURLE_OK;
}

ساكن فراغ cf_ip_happy_ctx_clear(بنية cf_ip_happy_ctx *ctx,
                                  بنية Curl_easy *انکڑے)
{
  DEBUGASSERT(ctx);
  إذا(ctx)
    cf_ip_ballers_clear(انکڑے, &ctx->ballers);
}

ساكن فراغ cf_ip_happy_ctx_destroy(بنية cf_ip_happy_ctx *ctx,
                                    بنية Curl_easy *انکڑے)
{
  إذا(ctx) {
    cf_ip_happy_ctx_clear(ctx, انکڑے);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_ip_happy_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *انکڑے,
                                     bool *پورا)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(انکڑے);
  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  نتیجہ = cf_ip_ballers_shutdown(&ctx->ballers, انکڑے, پورا);
  CURL_TRC_CF(انکڑے, cf, "shutdown -> %d, done=%d", (صحيح)نتیجہ, *پورا);
  أرجع نتیجہ;
}

ساكن CURLcode cf_ip_happy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *انکڑے,
                                           بنية easy_pollset *ps)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!cf->connected) {
    نتیجہ = cf_ip_ballers_pollset(&ctx->ballers, انکڑے, ps);
    CURL_TRC_CF(انکڑے, cf, "adjust_pollset -> %d, %u socks", (صحيح)نتیجہ,
                ps->n);
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_ip_happy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    bool *پورا)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  إذا(!انکڑے)
    أرجع CURLE_FAILED_INIT;

  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *پورا = FALSE;

  إذا(!ctx->dns_resolved) {
    نتیجہ = Curl_conn_dns_addr_نتیجہ(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    إذا(!نتیجہ)
      ctx->dns_resolved = TRUE;
    وإلا إذا(نتیجہ == CURLE_AGAIN) {
      نتیجہ = CURLE_OK;
    }
    وإلا /* real error */
      اذهب باہرلا;
  }

  بدل(ctx->state) {
  حالة SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, انکڑے));
    DEBUGASSERT(!cf->connected);
    نتیجہ = cf_ip_happy_init(cf, انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
    ctx->state = SCFST_اڈیک;
    FALLTHROUGH();
  حالة SCFST_اڈیک:
    نتیجہ = is_connected(cf, انکڑے, پورا);
    إذا(!نتیجہ && *پورا) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_پورا;
      cf->connected = TRUE;
      cf->اگلا = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = عدم;

      إذا(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrs_ویلا(انکڑے, TIMER_APPCONNECT); /* we are connected already */
#جے_تعریف_شدہ CURLVERBOSE
      إذا(Curl_trc_cf_is_verbose(cf, انکڑے)) {
        بنية ip_quadruple ipquad;
        bool is_ipv6;
        إذا(!Curl_conn_cf_get_ip_info(cf->اگلا, انکڑے, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(انکڑے, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_پورٹ);
        }
      }
#جے_انت
      cf_ip_happy_ctx_clear(ctx, انکڑے);
      Curl_expire_clear(انکڑے, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(انکڑے);
      انکڑے->info.numconnects++; /* to track the # of connections made */
    }
    اكسر;
  حالة SCFST_پورا:
    *پورا = TRUE;
    اكسر;
  }
باہرلا:
  أرجع نتیجہ;
}

ساكن bool cf_ip_happy_انکڑے_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *انکڑے)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  إذا(!cf->connected) {
    أرجع cf_ip_ballers_pending(&ctx->ballers, انکڑے);
  }
  أرجع cf->اگلا->cft->has_انکڑے_pending(cf->اگلا, انکڑے);
}

ساكن CURLcode cf_ip_happy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  إذا(!cf->connected) {
    بدل(query) {
    حالة CF_QUERY_CONNECT_جواب_MS: {
      *pres1 = cf_ip_ballers_min_جواب_ms(&ctx->ballers, انکڑے);
      CURL_TRC_CF(انکڑے, cf, "query connect reply: %dms", *pres1);
      أرجع CURLE_OK;
    }
    افتراضي:
      اكسر;
    }
  }

  أرجع cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, انکڑے, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن فراغ cf_ip_happy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(انکڑے, cf, "destroy");
  إذا(ctx) {
    cf_ip_happy_ctx_clear(ctx, انکڑے);
    cf_ip_happy_ctx_destroy(ctx, انکڑے);
  }
}

بنية Curl_cftype Curl_cft_ip_happy = {
  "HAPPY-EYEBALLS",
  CF_TYPE_تیاری,
  CURL_LOG_LVL_NONE,
  cf_ip_happy_destroy,
  cf_ip_happy_connect,
  cf_ip_happy_shutdown,
  cf_ip_happy_adjust_pollset,
  cf_ip_happy_انکڑے_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

ساكن CURLcode cf_ip_happy_create(بنية Curl_cfilter **pcf,
                                   بنية Curl_easy *انکڑے,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   uint8_t transport_peer,
                                   بنية connectdata *conn,
                                   بنية Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  بنية cf_ip_happy_ctx *ctx = عدم;
  CURLcode نتیجہ;

  (فراغ)انکڑے;
  (فراغ)conn;
  *pcf = عدم;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  نتیجہ = cf_ip_ballers_init(&ctx->ballers, انکڑے,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              انکڑے->سیٹ.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

باہرلا:
  إذا(نتیجہ) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, انکڑے);
  }
  أرجع نتیجہ;
}

CURLcode Curl_cf_ip_happy_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *انکڑے,
                                       بنية Curl_peer *origin,
                                       بنية Curl_peer *peer,
                                       uint8_t transport_peer,
                                       بنية Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجہ;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  نتیجہ = cf_ip_happy_create(&cf, انکڑے, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  إذا(نتیجہ)
    أرجع نتیجہ;

  Curl_conn_cf_داخل_کرو_after(cf_at, cf);
  أرجع CURLE_OK;
}
