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

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h> /* <netinet/tcp.h> may need it */
#پایان_اگر
#اگر_تعریف_شده HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر

#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

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
#اگر_تعریف_نشده UNITTESTS
ثابت
#پایان_اگر
بنية transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#پایان_اگر
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
  { Curl_cf_h3_proxy_create, TRNSPRT_QUIC, TRUE },
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_TFTP
  { Curl_cf_udp_create, TRNSPRT_UDP, FALSE },
#پایان_اگر
#اگر_تعریف_شده USE_UNIX_SOCKETS
  { Curl_cf_unix_create, TRNSPRT_UNIX, FALSE },
  { Curl_cf_unix_create, TRNSPRT_UNIX, TRUE },
#پایان_اگر
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

#اگر_تعریف_شده UNITTESTS
/* @unittest 2600 */
UNITTEST فراغ debug_مجموعه_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST فراغ debug_مجموعه_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    إذا(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#پایان_اگر /* UNITTESTS */

بنية cf_ai_iter {
  بنية Curl_cfilter *cf;
  بنية Curl_peer *peer;
  صحيح ai_family;
  بیعلامت صحيح n;
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

ساكن ثابت بنية Curl_addrinfo *cf_ai_iter_بعدی(بنية cf_ai_iter *iter,
                                                   بنية Curl_easy *داده)
{
  ثابت بنية Curl_addrinfo *addr;

  إذا(!iter->cf)
    أرجع عدم;

  addr = Curl_conn_dns_get_ai(داده, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  إذا(addr)
    iter->n++;
  أرجع addr;
}

ساكن bool cf_ai_iter_has_بیشتر(بنية cf_ai_iter *iter,
                                بنية Curl_easy *داده)
{
  أرجع (iter->cf &&
          !!Curl_conn_dns_get_ai(داده, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

بنية cf_ip_attempt {
  بنية cf_ip_attempt *بعدی;
  بنية Curl_peer *origin;
  بنية Curl_peer *peer;
  بنية Curl_peer *tunnel_peer;
  بنية Curl_sockaddr_ex addr;
  بنية Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode نتیجه;
  صحيح ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  صحيح error;
  بیت(connected);                    /* cf has connected */
  بیت(shutdown);                     /* cf has shutdown */
  بیت(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

ساكن فراغ cf_ip_attempt_free(بنية cf_ip_attempt *a,
                               بنية Curl_easy *داده)
{
  إذا(a) {
    إذا(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, داده);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

ساكن CURLcode cf_ip_attempt_new(بنية cf_ip_attempt **pa,
                                  بنية Curl_easy *داده,
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
  CURLcode نتیجه = CURLE_OK;

  *pa = عدم;
  a = curlx_calloc(1, حجم(*a));
  إذا(!a)
    أرجع CURLE_بیرون_از_حافظه;

  Curl_peer_link(&a->origin, origin);
  Curl_peer_link(&a->peer, peer);
  a->transport_peer = transport_peer;
  Curl_peer_link(&a->tunnel_peer, tunnel_peer);
  a->tunnel_transport = tunnel_transport;
  a->addr = *addr;
  a->ai_family = ai_family;
  a->نتیجه = CURLE_OK;
  a->cf_create = cf_create;
  *pa = a;

  نتیجه = a->cf_create(&a->cf, داده, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  إذا(نتیجه)
    اذهب بیرون;

  /* the new filter might have sub-filters */
  لكل(wcf = a->cf; wcf; wcf = wcf->بعدی) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

بیرون:
  إذا(نتیجه) {
    cf_ip_attempt_free(a, داده);
    *pa = عدم;
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_ip_attempt_connect(بنية cf_ip_attempt *a,
                                      بنية Curl_easy *داده,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  إذا(!a->نتیجه && !*connected) {
    /* evaluate again */
    a->نتیجه = Curl_conn_cf_connect(a->cf, داده, connected);

    إذا(!a->نتیجه) {
      إذا(*connected) {
        a->connected = TRUE;
      }
    }
    وإلا {
      إذا(a->نتیجه == CURLE_WEIRD_SERVER_پاسخ)
        a->inconclusive = TRUE;
      إذا(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, داده);
    }
  }
  أرجع a->نتیجه;
}

بنية cf_ip_ballers {
  بنية cf_ip_attempt *running;
  بنية cf_ip_attempt **running_دم;  /* &running or &(last->next) */
  بنية cf_ip_attempt *winner;
  بنية cf_ai_iter addr_iter;
#اگر_تعریف_شده USE_IPV6
  بنية cf_ai_iter ipv6_iter;
#پایان_اگر
  بنية Curl_peer *origin;
  بنية Curl_peer *peer;
  بنية Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  بنية curltime آخرین_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode آخرین_dead_نتیجه;  /* result of last discarded hard failure */
  صحيح آخرین_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  بیت(had_dead);              /* a hard failure was discarded */
};

ساكن CURLcode cf_ip_attempt_restart(بنية cf_ip_attempt *a,
                                      بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده)
{
  بنية Curl_cfilter *wcf;
  CURLcode نتیجه;

  إذا(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, داده);

  a->نتیجه = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = عدم;

  نتیجه = a->cf_create(&a->cf, داده, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  إذا(!نتیجه) {
    bool dummy;
    /* the new filter might have sub-filters */
    لكل(wcf = a->cf; wcf; wcf = wcf->بعدی) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->نتیجه = cf_ip_attempt_connect(a, داده, &dummy);
  }
  أرجع نتیجه;
}

ساكن فراغ cf_ip_ballers_clear(بنية Curl_easy *داده,
                                بنية cf_ip_ballers *bs)
{
  طالما(bs->running) {
    بنية cf_ip_attempt *a = bs->running;
    bs->running = a->بعدی;
    cf_ip_attempt_free(a, داده);
  }
  bs->running_دم = &bs->running;
  cf_ip_attempt_free(bs->winner, داده);
  bs->winner = عدم;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

ساكن CURLcode cf_ip_ballers_init(بنية cf_ip_ballers *bs,
                                   بنية Curl_easy *داده,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   uint8_t transport_peer,
                                   بنية Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, حجم(*bs));
  bs->running_دم = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  إذا(!bs->cf_create) {
    failf(داده, "unsupported transport type %u%s",
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
  bs->آخرین_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  أرجع CURLE_OK;
}

ساكن فراغ cf_ip_ballers_prune(بنية cf_ip_ballers *bs,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                uint32_t max_concurrent)
{
  بنية cf_ip_attempt *a = عدم, **panchor;
  uint32_t ongoing = 0;

  لكل(a = bs->running; a; a = a->بعدی) {
    إذا(!a->نتیجه && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  طالما(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    إذا(!a->نتیجه && !a->connected) {
      *panchor = a->بعدی;
      إذا(!a->بعدی)
        bs->running_دم = panchor;
      a->بعدی = عدم;
      cf_ip_attempt_free(a, داده);
      --ongoing;
      CURL_TRC_CF(داده, cf, "discarding oldest attempt to keep limit");
    }
    وإلا {
      panchor = &a->بعدی;
    }
  }
}

ساكن CURLcode cf_ip_ballers_پیاپی(بنية cf_ip_ballers *bs,
                                  بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode نتیجه = CURLE_OK;
  بنية cf_ip_attempt *a = عدم, **panchor;
  bool do_بیشتر;
  timediff_t بعدی_expire_ms;
  uint32_t inconclusive, ongoing;
  ثابت بنية curltime *pnow = عدم;
  VERBOSE(صحيح i);

  إذا(bs->winner)
    أرجع CURLE_OK;

evaluate:
  إذا(Curl_timeleft_now_ms(داده, Curl_pgrs_now(داده)) < 0) {
    failf(داده, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(داده, عدم, TIMER_STARTSINGLE));
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  لكل(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->نتیجه = cf_ip_attempt_connect(a, داده, connected);
    إذا(!a->نتیجه) {
      إذا(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(داده, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->بعدی;
        a->بعدی = عدم;
        طالما(bs->running) {
          a = bs->running;
          bs->running = a->بعدی;
          cf_ip_attempt_free(a, داده);
        }
        bs->running_دم = &bs->running;
        أرجع CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->بعدی;
    }
    وإلا إذا(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->بعدی;
    }
    وإلا {
      /* hard failure, discard right away */
      *panchor = a->بعدی;
      إذا(!a->بعدی)
        bs->running_دم = panchor;
      a->بعدی = عدم;
      bs->آخرین_dead_نتیجه = a->نتیجه;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, داده);
    }
  }
  إذا(bs->running)
    CURL_TRC_CF(داده, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(داده);
  إذا(!ongoing) {
    do_بیشتر = TRUE;
  }
  وإلا {
    bool بیشتر_possible = cf_ai_iter_has_بیشتر(&bs->addr_iter, داده);
#اگر_تعریف_شده USE_IPV6
    إذا(!بیشتر_possible)
      بیشتر_possible = cf_ai_iter_has_بیشتر(&bs->ipv6_iter, داده);
#پایان_اگر
    do_بیشتر = بیشتر_possible &&
      (curlx_ptimediff_ms(pnow, &bs->آخرین_attempt_started) >=
       bs->attempt_delay_ms);
    إذا(do_بیشتر)
      CURL_TRC_CF(داده, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  إذا(do_بیشتر) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ثابت بنية Curl_addrinfo *ai = عدم;
    صحيح ai_family = 0;
    CURL_TRC_CF(داده, cf, "want to do more");
#اگر_تعریف_شده USE_IPV6
    إذا((bs->آخرین_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_بیشتر(&bs->addr_iter, داده)) {
      ai = cf_ai_iter_بعدی(&bs->ipv6_iter, داده);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(داده, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#پایان_اگر
    إذا(!ai) {
      ai = cf_ai_iter_بعدی(&bs->addr_iter, داده);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(داده, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(داده);

    إذا(ai) {  /* try another address */
      بنية Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      إذا(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, داده, bs->max_concurrent - 1);

      نتیجه = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      إذا(نتیجه)
        اذهب بیرون;

      نتیجه = cf_ip_attempt_new(&a, داده, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(داده, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (صحيح)نتیجه);
      إذا(نتیجه)
        اذهب بیرون;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_دم = a;
      a->بعدی = عدم;
      bs->running_دم = &a->بعدی;
      bs->آخرین_attempt_started = *pnow;
      bs->آخرین_attempt_ai_family = ai_family;
      /* and run everything again */
      اذهب evaluate;
    }
    وإلا إذا(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->آخرین_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      إذا(delay_ms <= 0) {
        CURL_TRC_CF(داده, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        لكل(a = bs->running; a; a = a->بعدی) {
          VERBOSE(++i);
          إذا(!a->inconclusive)
            استمر;
          نتیجه = cf_ip_attempt_restart(a, cf, داده);
          CURL_TRC_CF(داده, cf, "restarted baller %d -> %d", i, (صحيح)نتیجه);
          إذا(نتیجه) /* serious failure */
            اذهب بیرون;
          bs->آخرین_attempt_started = *pnow;
          اذهب evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      وإلا {
        /* let's wait some more before restarting */
        infof(داده, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_مجموعه(داده, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      اذهب بیرون;
    }
    وإلا إذا(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(داده, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      نتیجه = bs->had_dead ? bs->آخرین_dead_نتیجه : CURLE_COULDNT_CONNECT;
    }
  }

بیرون:
  إذا(!نتیجه) {
    bool بیشتر_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(داده);
    بعدی_expire_ms = Curl_timeleft_now_ms(داده, pnow);
    إذا(بعدی_expire_ms < 0) {
      failf(داده, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(داده, عدم, TIMER_STARTSINGLE));
      أرجع CURLE_OPERATION_TIMEDOUT;
    }

    بیشتر_possible = cf_ai_iter_has_بیشتر(&bs->addr_iter, داده);
#اگر_تعریف_شده USE_IPV6
    إذا(!بیشتر_possible)
      بیشتر_possible = cf_ai_iter_has_بیشتر(&bs->ipv6_iter, داده);
#پایان_اگر
    إذا(بیشتر_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->آخرین_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      بعدی_expire_ms = CURLMIN(بعدی_expire_ms, expire_ms);
      إذا(بعدی_expire_ms <= 0) {
        CURL_TRC_CF(داده, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        اذهب evaluate;
      }
      CURL_TRC_CF(داده, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", بعدی_expire_ms);
      Curl_expire_مجموعه(داده, EXPIRE_HAPPY_EYEBALLS, بعدی_expire_ms, pnow);
    }
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_ip_ballers_shutdown(بنية cf_ip_ballers *bs,
                                       بنية Curl_easy *داده,
                                       bool *انجامشده)
{
  بنية cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *انجامشده = TRUE;
  لكل(a = bs->running; a; a = a->بعدی) {
    bool bdone = FALSE;
    إذا(a->shutdown || !a->cf)
      استمر;
    a->نتیجه = a->cf->cft->do_shutdown(a->cf, داده, &bdone);
    إذا(a->نتیجه || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    وإلا
      *انجامشده = FALSE;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ip_ballers_pollset(بنية cf_ip_ballers *bs,
                                      بنية Curl_easy *داده,
                                      بنية easy_pollset *ps)
{
  بنية cf_ip_attempt *a;
  CURLcode نتیجه = CURLE_OK;
  لكل(a = bs->running; a && !نتیجه; a = a->بعدی) {
    إذا(a->نتیجه)
      استمر;
    نتیجه = Curl_conn_cf_adjust_pollset(a->cf, داده, ps);
  }
  أرجع نتیجه;
}

ساكن bool cf_ip_ballers_pending(بنية cf_ip_ballers *bs,
                                  ثابت بنية Curl_easy *داده)
{
  بنية cf_ip_attempt *a;

  لكل(a = bs->running; a; a = a->بعدی) {
    إذا(a->نتیجه)
      استمر;
    إذا(a->cf && a->cf->cft->has_داده_pending(a->cf, داده))
      أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن صحيح cf_ip_ballers_min_پاسخ_ms(بنية cf_ip_ballers *bs,
                                      بنية Curl_easy *داده)
{
  صحيح پاسخ_ms = -1, breply_ms;
  بنية cf_ip_attempt *a;

  لكل(a = bs->running; a; a = a->بعدی) {
    إذا(a->cf && !a->cf->cft->query(a->cf, داده, CF_QUERY_CONNECT_پاسخ_MS,
                                   &breply_ms, عدم) &&
       breply_ms >= 0 && (پاسخ_ms < 0 || breply_ms < پاسخ_ms))
      پاسخ_ms = breply_ms;
  }
  أرجع پاسخ_ms;
}

تعريفنوع تعداد {
  SCFST_INIT,
  SCFST_انتظار,
  SCFST_انجامشده
} cf_connect_state;

بنية cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  بنية cf_ip_ballers ballers;
  بیت(dns_resolved);
};

ساكن CURLcode is_connected(بنية Curl_cfilter *cf,
                             بنية Curl_easy *داده,
                             bool *connected)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  بنية connectdata *conn = cf->conn;
  CURLcode نتیجه;

  نتیجه = cf_ip_ballers_پیاپی(&ctx->ballers, cf, داده,
                             (bool)ctx->dns_resolved, connected);

  إذا(!نتیجه)
    أرجع CURLE_OK;
  وإلا {
    بنية Curl_peer *peer = عدم, *proxy_peer = عدم;
    حرف viamsg[160];

    peer = Curl_conn_get_اولی_peer(conn, cf->sockindex);
    إذا(!conn->origin || !peer)
      أرجع CURLE_FAILED_INIT;

#اگر_تعریف_نشده CURL_DISABLE_PROXY
    إذا(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    وإلا إذا(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#پایان_اگر

    viamsg[0] = 0;
    إذا(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#اگر_تعریف_شده USE_UNIX_SOCKETS
      إذا(peer->unix_socket)
        curl_msnprintf(viamsg, حجم(viamsg), " over unix://%s",
                       peer->hostname);
      وإلا
#پایان_اگر
      curl_msnprintf(viamsg, حجم(viamsg), " via %s:%u",
                     peer->hostname, peer->پورت);
    }

    failf(داده, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->پورت, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(داده, عدم, TIMER_STARTSINGLE),
          curl_easy_strerror(نتیجه));

#اگر_تعریف_شده SOCKETIMEDOUT
    إذا(SOCKETIMEDOUT == داده->state.os_errno)
      نتیجه = CURLE_OPERATION_TIMEDOUT;
#پایان_اگر

    أرجع نتیجه;
  }
}

#تعریف_کن IP_HE_MAX_CONCURRENT_ATTEMPTS     6

ساكن CURLcode cf_ip_happy_init(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  إذا(Curl_timeleft_ms(داده) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(داده, "Connection time-out");
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  إذا(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#اگر_تعریف_شده USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#وإلا
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
#پایان_اگر
  }
  وإلا { /* TCP/UDP/QUIC */
#اگر_تعریف_شده USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#پایان_اگر
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(داده, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  أرجع CURLE_OK;
}

ساكن فراغ cf_ip_happy_ctx_clear(بنية cf_ip_happy_ctx *ctx,
                                  بنية Curl_easy *داده)
{
  DEBUGASSERT(ctx);
  إذا(ctx)
    cf_ip_ballers_clear(داده, &ctx->ballers);
}

ساكن فراغ cf_ip_happy_ctx_destroy(بنية cf_ip_happy_ctx *ctx,
                                    بنية Curl_easy *داده)
{
  إذا(ctx) {
    cf_ip_happy_ctx_clear(ctx, داده);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_ip_happy_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *داده,
                                     bool *انجامشده)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(داده);
  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  نتیجه = cf_ip_ballers_shutdown(&ctx->ballers, داده, انجامشده);
  CURL_TRC_CF(داده, cf, "shutdown -> %d, done=%d", (صحيح)نتیجه, *انجامشده);
  أرجع نتیجه;
}

ساكن CURLcode cf_ip_happy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *داده,
                                           بنية easy_pollset *ps)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  إذا(!cf->connected) {
    نتیجه = cf_ip_ballers_pollset(&ctx->ballers, داده, ps);
    CURL_TRC_CF(داده, cf, "adjust_pollset -> %d, %u socks", (صحيح)نتیجه,
                ps->n);
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_ip_happy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    bool *انجامشده)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  إذا(!داده)
    أرجع CURLE_FAILED_INIT;

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *انجامشده = FALSE;

  إذا(!ctx->dns_resolved) {
    نتیجه = Curl_conn_dns_addr_نتیجه(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    إذا(!نتیجه)
      ctx->dns_resolved = TRUE;
    وإلا إذا(نتیجه == CURLE_AGAIN) {
      نتیجه = CURLE_OK;
    }
    وإلا /* real error */
      اذهب بیرون;
  }

  بدل(ctx->state) {
  حالة SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, داده));
    DEBUGASSERT(!cf->connected);
    نتیجه = cf_ip_happy_init(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
    ctx->state = SCFST_انتظار;
    FALLTHROUGH();
  حالة SCFST_انتظار:
    نتیجه = is_connected(cf, داده, انجامشده);
    إذا(!نتیجه && *انجامشده) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_انجامشده;
      cf->connected = TRUE;
      cf->بعدی = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = عدم;

      إذا(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrs_زمان(داده, TIMER_APPCONNECT); /* we are connected already */
#اگر_تعریف_شده CURLVERBOSE
      إذا(Curl_trc_cf_is_verbose(cf, داده)) {
        بنية ip_quadruple ipquad;
        bool is_ipv6;
        إذا(!Curl_conn_cf_get_ip_info(cf->بعدی, داده, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(داده, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_پورت);
        }
      }
#پایان_اگر
      cf_ip_happy_ctx_clear(ctx, داده);
      Curl_expire_clear(داده, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(داده);
      داده->info.numconnects++; /* to track the # of connections made */
    }
    اكسر;
  حالة SCFST_انجامشده:
    *انجامشده = TRUE;
    اكسر;
  }
بیرون:
  أرجع نتیجه;
}

ساكن bool cf_ip_happy_داده_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *داده)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  إذا(!cf->connected) {
    أرجع cf_ip_ballers_pending(&ctx->ballers, داده);
  }
  أرجع cf->بعدی->cft->has_داده_pending(cf->بعدی, داده);
}

ساكن CURLcode cf_ip_happy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  إذا(!cf->connected) {
    بدل(query) {
    حالة CF_QUERY_CONNECT_پاسخ_MS: {
      *pres1 = cf_ip_ballers_min_پاسخ_ms(&ctx->ballers, داده);
      CURL_TRC_CF(داده, cf, "query connect reply: %dms", *pres1);
      أرجع CURLE_OK;
    }
    افتراضي:
      اكسر;
    }
  }

  أرجع cf->بعدی ?
    cf->بعدی->cft->query(cf->بعدی, داده, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن فراغ cf_ip_happy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(داده, cf, "destroy");
  إذا(ctx) {
    cf_ip_happy_ctx_clear(ctx, داده);
    cf_ip_happy_ctx_destroy(ctx, داده);
  }
}

بنية Curl_cftype Curl_cft_ip_happy = {
  "HAPPY-EYEBALLS",
  CF_TYPE_راهاندازی,
  CURL_LOG_LVL_NONE,
  cf_ip_happy_destroy,
  cf_ip_happy_connect,
  cf_ip_happy_shutdown,
  cf_ip_happy_adjust_pollset,
  cf_ip_happy_داده_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

ساكن CURLcode cf_ip_happy_create(بنية Curl_cfilter **pcf,
                                   بنية Curl_easy *داده,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   uint8_t transport_peer,
                                   بنية connectdata *conn,
                                   بنية Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  بنية cf_ip_happy_ctx *ctx = عدم;
  CURLcode نتیجه;

  (فراغ)داده;
  (فراغ)conn;
  *pcf = عدم;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  نتیجه = cf_ip_ballers_init(&ctx->ballers, داده,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              داده->مجموعه.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

بیرون:
  إذا(نتیجه) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, داده);
  }
  أرجع نتیجه;
}

CURLcode Curl_cf_ip_happy_درج_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *داده,
                                       بنية Curl_peer *origin,
                                       بنية Curl_peer *peer,
                                       uint8_t transport_peer,
                                       بنية Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  نتیجه = cf_ip_happy_create(&cf, داده, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  إذا(نتیجه)
    أرجع نتیجه;

  Curl_conn_cf_درج_after(cf_at, cf);
  أرجع CURLE_OK;
}
