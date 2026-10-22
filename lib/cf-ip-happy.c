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

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h> /* <netinet/tcp.h> may need it */
#د_که_پای
#که_تعریف_شوی HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#د_که_پای
#که_تعریف_شوی HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای

#که_تعریف_شوی __VMS
#ضمن <in.h>
#ضمن <inet.h>
#د_که_پای

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
#که_نه_تعریف_شوی UNITTESTS
ثابت
#د_که_پای
بنية transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#د_که_پای
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
  { Curl_cf_h3_proxy_create, TRNSPRT_QUIC, TRUE },
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_TFTP
  { Curl_cf_udp_create, TRNSPRT_UDP, FALSE },
#د_که_پای
#که_تعریف_شوی USE_UNIX_SOCKETS
  { Curl_cf_unix_create, TRNSPRT_UNIX, FALSE },
  { Curl_cf_unix_create, TRNSPRT_UNIX, TRUE },
#د_که_پای
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

#که_تعریف_شوی UNITTESTS
/* @unittest 2600 */
UNITTEST فراغ debug_سېټ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST فراغ debug_سېټ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    إذا(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#د_که_پای /* UNITTESTS */

بنية cf_ai_iter {
  بنية Curl_cfilter *cf;
  بنية Curl_peer *peer;
  صحيح ai_family;
  بینښه صحيح n;
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

ساكن ثابت بنية Curl_addrinfo *cf_ai_iter_بل(بنية cf_ai_iter *iter,
                                                   بنية Curl_easy *معلومات)
{
  ثابت بنية Curl_addrinfo *addr;

  إذا(!iter->cf)
    أرجع عدم;

  addr = Curl_conn_dns_get_ai(معلومات, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  إذا(addr)
    iter->n++;
  أرجع addr;
}

ساكن bool cf_ai_iter_has_نور(بنية cf_ai_iter *iter,
                                بنية Curl_easy *معلومات)
{
  أرجع (iter->cf &&
          !!Curl_conn_dns_get_ai(معلومات, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

بنية cf_ip_attempt {
  بنية cf_ip_attempt *بل;
  بنية Curl_peer *origin;
  بنية Curl_peer *peer;
  بنية Curl_peer *tunnel_peer;
  بنية Curl_sockaddr_ex addr;
  بنية Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode پایله;
  صحيح ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  صحيح error;
  بټ(connected);                    /* cf has connected */
  بټ(shutdown);                     /* cf has shutdown */
  بټ(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

ساكن فراغ cf_ip_attempt_free(بنية cf_ip_attempt *a,
                               بنية Curl_easy *معلومات)
{
  إذا(a) {
    إذا(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, معلومات);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

ساكن CURLcode cf_ip_attempt_new(بنية cf_ip_attempt **pa,
                                  بنية Curl_easy *معلومات,
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
  CURLcode پایله = CURLE_OK;

  *pa = عدم;
  a = curlx_calloc(1, حجم(*a));
  إذا(!a)
    أرجع CURLE_بهر_د_حافظه;

  Curl_peer_link(&a->origin, origin);
  Curl_peer_link(&a->peer, peer);
  a->transport_peer = transport_peer;
  Curl_peer_link(&a->tunnel_peer, tunnel_peer);
  a->tunnel_transport = tunnel_transport;
  a->addr = *addr;
  a->ai_family = ai_family;
  a->پایله = CURLE_OK;
  a->cf_create = cf_create;
  *pa = a;

  پایله = a->cf_create(&a->cf, معلومات, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  إذا(پایله)
    اذهب بهر;

  /* the new filter might have sub-filters */
  لكل(wcf = a->cf; wcf; wcf = wcf->بل) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

بهر:
  إذا(پایله) {
    cf_ip_attempt_free(a, معلومات);
    *pa = عدم;
  }
  أرجع پایله;
}

ساكن CURLcode cf_ip_attempt_connect(بنية cf_ip_attempt *a,
                                      بنية Curl_easy *معلومات,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  إذا(!a->پایله && !*connected) {
    /* evaluate again */
    a->پایله = Curl_conn_cf_connect(a->cf, معلومات, connected);

    إذا(!a->پایله) {
      إذا(*connected) {
        a->connected = TRUE;
      }
    }
    وإلا {
      إذا(a->پایله == CURLE_WEIRD_SERVER_ځواب)
        a->inconclusive = TRUE;
      إذا(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, معلومات);
    }
  }
  أرجع a->پایله;
}

بنية cf_ip_ballers {
  بنية cf_ip_attempt *running;
  بنية cf_ip_attempt **running_لکۍ;  /* &running or &(last->next) */
  بنية cf_ip_attempt *winner;
  بنية cf_ai_iter addr_iter;
#که_تعریف_شوی USE_IPV6
  بنية cf_ai_iter ipv6_iter;
#د_که_پای
  بنية Curl_peer *origin;
  بنية Curl_peer *peer;
  بنية Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  بنية curltime وروستی_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode وروستی_dead_پایله;  /* result of last discarded hard failure */
  صحيح وروستی_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  بټ(had_dead);              /* a hard failure was discarded */
};

ساكن CURLcode cf_ip_attempt_restart(بنية cf_ip_attempt *a,
                                      بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *wcf;
  CURLcode پایله;

  إذا(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, معلومات);

  a->پایله = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = عدم;

  پایله = a->cf_create(&a->cf, معلومات, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  إذا(!پایله) {
    bool dummy;
    /* the new filter might have sub-filters */
    لكل(wcf = a->cf; wcf; wcf = wcf->بل) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->پایله = cf_ip_attempt_connect(a, معلومات, &dummy);
  }
  أرجع پایله;
}

ساكن فراغ cf_ip_ballers_clear(بنية Curl_easy *معلومات,
                                بنية cf_ip_ballers *bs)
{
  طالما(bs->running) {
    بنية cf_ip_attempt *a = bs->running;
    bs->running = a->بل;
    cf_ip_attempt_free(a, معلومات);
  }
  bs->running_لکۍ = &bs->running;
  cf_ip_attempt_free(bs->winner, معلومات);
  bs->winner = عدم;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

ساكن CURLcode cf_ip_ballers_init(بنية cf_ip_ballers *bs,
                                   بنية Curl_easy *معلومات,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   uint8_t transport_peer,
                                   بنية Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, حجم(*bs));
  bs->running_لکۍ = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  إذا(!bs->cf_create) {
    failf(معلومات, "unsupported transport type %u%s",
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
  bs->وروستی_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  أرجع CURLE_OK;
}

ساكن فراغ cf_ip_ballers_prune(بنية cf_ip_ballers *bs,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                uint32_t max_concurrent)
{
  بنية cf_ip_attempt *a = عدم, **panchor;
  uint32_t ongoing = 0;

  لكل(a = bs->running; a; a = a->بل) {
    إذا(!a->پایله && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  طالما(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    إذا(!a->پایله && !a->connected) {
      *panchor = a->بل;
      إذا(!a->بل)
        bs->running_لکۍ = panchor;
      a->بل = عدم;
      cf_ip_attempt_free(a, معلومات);
      --ongoing;
      CURL_TRC_CF(معلومات, cf, "discarding oldest attempt to keep limit");
    }
    وإلا {
      panchor = &a->بل;
    }
  }
}

ساكن CURLcode cf_ip_ballers_پرلپسې(بنية cf_ip_ballers *bs,
                                  بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode پایله = CURLE_OK;
  بنية cf_ip_attempt *a = عدم, **panchor;
  bool do_نور;
  timediff_t بل_expire_ms;
  uint32_t inconclusive, ongoing;
  ثابت بنية curltime *pnow = عدم;
  VERBOSE(صحيح i);

  إذا(bs->winner)
    أرجع CURLE_OK;

evaluate:
  إذا(Curl_timeleft_now_ms(معلومات, Curl_pgrs_now(معلومات)) < 0) {
    failf(معلومات, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(معلومات, عدم, TIMER_STARTSINGLE));
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  لكل(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->پایله = cf_ip_attempt_connect(a, معلومات, connected);
    إذا(!a->پایله) {
      إذا(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(معلومات, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->بل;
        a->بل = عدم;
        طالما(bs->running) {
          a = bs->running;
          bs->running = a->بل;
          cf_ip_attempt_free(a, معلومات);
        }
        bs->running_لکۍ = &bs->running;
        أرجع CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->بل;
    }
    وإلا إذا(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->بل;
    }
    وإلا {
      /* hard failure, discard right away */
      *panchor = a->بل;
      إذا(!a->بل)
        bs->running_لکۍ = panchor;
      a->بل = عدم;
      bs->وروستی_dead_پایله = a->پایله;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, معلومات);
    }
  }
  إذا(bs->running)
    CURL_TRC_CF(معلومات, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(معلومات);
  إذا(!ongoing) {
    do_نور = TRUE;
  }
  وإلا {
    bool نور_possible = cf_ai_iter_has_نور(&bs->addr_iter, معلومات);
#که_تعریف_شوی USE_IPV6
    إذا(!نور_possible)
      نور_possible = cf_ai_iter_has_نور(&bs->ipv6_iter, معلومات);
#د_که_پای
    do_نور = نور_possible &&
      (curlx_ptimediff_ms(pnow, &bs->وروستی_attempt_started) >=
       bs->attempt_delay_ms);
    إذا(do_نور)
      CURL_TRC_CF(معلومات, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  إذا(do_نور) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ثابت بنية Curl_addrinfo *ai = عدم;
    صحيح ai_family = 0;
    CURL_TRC_CF(معلومات, cf, "want to do more");
#که_تعریف_شوی USE_IPV6
    إذا((bs->وروستی_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_نور(&bs->addr_iter, معلومات)) {
      ai = cf_ai_iter_بل(&bs->ipv6_iter, معلومات);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(معلومات, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#د_که_پای
    إذا(!ai) {
      ai = cf_ai_iter_بل(&bs->addr_iter, معلومات);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(معلومات, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(معلومات);

    إذا(ai) {  /* try another address */
      بنية Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      إذا(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, معلومات, bs->max_concurrent - 1);

      پایله = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      إذا(پایله)
        اذهب بهر;

      پایله = cf_ip_attempt_new(&a, معلومات, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(معلومات, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (صحيح)پایله);
      إذا(پایله)
        اذهب بهر;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_لکۍ = a;
      a->بل = عدم;
      bs->running_لکۍ = &a->بل;
      bs->وروستی_attempt_started = *pnow;
      bs->وروستی_attempt_ai_family = ai_family;
      /* and run everything again */
      اذهب evaluate;
    }
    وإلا إذا(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->وروستی_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      إذا(delay_ms <= 0) {
        CURL_TRC_CF(معلومات, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        لكل(a = bs->running; a; a = a->بل) {
          VERBOSE(++i);
          إذا(!a->inconclusive)
            استمر;
          پایله = cf_ip_attempt_restart(a, cf, معلومات);
          CURL_TRC_CF(معلومات, cf, "restarted baller %d -> %d", i, (صحيح)پایله);
          إذا(پایله) /* serious failure */
            اذهب بهر;
          bs->وروستی_attempt_started = *pnow;
          اذهب evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      وإلا {
        /* let's wait some more before restarting */
        infof(معلومات, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_سېټ(معلومات, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      اذهب بهر;
    }
    وإلا إذا(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(معلومات, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      پایله = bs->had_dead ? bs->وروستی_dead_پایله : CURLE_COULDNT_CONNECT;
    }
  }

بهر:
  إذا(!پایله) {
    bool نور_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(معلومات);
    بل_expire_ms = Curl_timeleft_now_ms(معلومات, pnow);
    إذا(بل_expire_ms < 0) {
      failf(معلومات, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(معلومات, عدم, TIMER_STARTSINGLE));
      أرجع CURLE_OPERATION_TIMEDOUT;
    }

    نور_possible = cf_ai_iter_has_نور(&bs->addr_iter, معلومات);
#که_تعریف_شوی USE_IPV6
    إذا(!نور_possible)
      نور_possible = cf_ai_iter_has_نور(&bs->ipv6_iter, معلومات);
#د_که_پای
    إذا(نور_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->وروستی_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      بل_expire_ms = CURLMIN(بل_expire_ms, expire_ms);
      إذا(بل_expire_ms <= 0) {
        CURL_TRC_CF(معلومات, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        اذهب evaluate;
      }
      CURL_TRC_CF(معلومات, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", بل_expire_ms);
      Curl_expire_سېټ(معلومات, EXPIRE_HAPPY_EYEBALLS, بل_expire_ms, pnow);
    }
  }
  أرجع پایله;
}

ساكن CURLcode cf_ip_ballers_shutdown(بنية cf_ip_ballers *bs,
                                       بنية Curl_easy *معلومات,
                                       bool *بشپړ)
{
  بنية cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *بشپړ = TRUE;
  لكل(a = bs->running; a; a = a->بل) {
    bool bdone = FALSE;
    إذا(a->shutdown || !a->cf)
      استمر;
    a->پایله = a->cf->cft->do_shutdown(a->cf, معلومات, &bdone);
    إذا(a->پایله || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    وإلا
      *بشپړ = FALSE;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ip_ballers_pollset(بنية cf_ip_ballers *bs,
                                      بنية Curl_easy *معلومات,
                                      بنية easy_pollset *ps)
{
  بنية cf_ip_attempt *a;
  CURLcode پایله = CURLE_OK;
  لكل(a = bs->running; a && !پایله; a = a->بل) {
    إذا(a->پایله)
      استمر;
    پایله = Curl_conn_cf_adjust_pollset(a->cf, معلومات, ps);
  }
  أرجع پایله;
}

ساكن bool cf_ip_ballers_pending(بنية cf_ip_ballers *bs,
                                  ثابت بنية Curl_easy *معلومات)
{
  بنية cf_ip_attempt *a;

  لكل(a = bs->running; a; a = a->بل) {
    إذا(a->پایله)
      استمر;
    إذا(a->cf && a->cf->cft->has_معلومات_pending(a->cf, معلومات))
      أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن صحيح cf_ip_ballers_min_ځواب_ms(بنية cf_ip_ballers *bs,
                                      بنية Curl_easy *معلومات)
{
  صحيح ځواب_ms = -1, breply_ms;
  بنية cf_ip_attempt *a;

  لكل(a = bs->running; a; a = a->بل) {
    إذا(a->cf && !a->cf->cft->query(a->cf, معلومات, CF_QUERY_CONNECT_ځواب_MS,
                                   &breply_ms, عدم) &&
       breply_ms >= 0 && (ځواب_ms < 0 || breply_ms < ځواب_ms))
      ځواب_ms = breply_ms;
  }
  أرجع ځواب_ms;
}

تعريفنوع تعداد {
  SCFST_INIT,
  SCFST_انتظار,
  SCFST_بشپړ
} cf_connect_state;

بنية cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  بنية cf_ip_ballers ballers;
  بټ(dns_resolved);
};

ساكن CURLcode is_connected(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات,
                             bool *connected)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  بنية connectdata *conn = cf->conn;
  CURLcode پایله;

  پایله = cf_ip_ballers_پرلپسې(&ctx->ballers, cf, معلومات,
                             (bool)ctx->dns_resolved, connected);

  إذا(!پایله)
    أرجع CURLE_OK;
  وإلا {
    بنية Curl_peer *peer = عدم, *proxy_peer = عدم;
    حرف viamsg[160];

    peer = Curl_conn_get_لومړی_peer(conn, cf->sockindex);
    إذا(!conn->origin || !peer)
      أرجع CURLE_FAILED_INIT;

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
    إذا(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    وإلا إذا(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#د_که_پای

    viamsg[0] = 0;
    إذا(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#که_تعریف_شوی USE_UNIX_SOCKETS
      إذا(peer->unix_socket)
        curl_msnprintf(viamsg, حجم(viamsg), " over unix://%s",
                       peer->hostname);
      وإلا
#د_که_پای
      curl_msnprintf(viamsg, حجم(viamsg), " via %s:%u",
                     peer->hostname, peer->پورټ);
    }

    failf(معلومات, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->پورټ, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(معلومات, عدم, TIMER_STARTSINGLE),
          curl_easy_strerror(پایله));

#که_تعریف_شوی SOCKETIMEDOUT
    إذا(SOCKETIMEDOUT == معلومات->state.os_errno)
      پایله = CURLE_OPERATION_TIMEDOUT;
#د_که_پای

    أرجع پایله;
  }
}

#تعریف_کړه IP_HE_MAX_CONCURRENT_ATTEMPTS     6

ساكن CURLcode cf_ip_happy_init(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  إذا(Curl_timeleft_ms(معلومات) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(معلومات, "Connection time-out");
    أرجع CURLE_OPERATION_TIMEDOUT;
  }

  إذا(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#که_تعریف_شوی USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#وإلا
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
#د_که_پای
  }
  وإلا { /* TCP/UDP/QUIC */
#که_تعریف_شوی USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#د_که_پای
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(معلومات, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  أرجع CURLE_OK;
}

ساكن فراغ cf_ip_happy_ctx_clear(بنية cf_ip_happy_ctx *ctx,
                                  بنية Curl_easy *معلومات)
{
  DEBUGASSERT(ctx);
  إذا(ctx)
    cf_ip_ballers_clear(معلومات, &ctx->ballers);
}

ساكن فراغ cf_ip_happy_ctx_destroy(بنية cf_ip_happy_ctx *ctx,
                                    بنية Curl_easy *معلومات)
{
  إذا(ctx) {
    cf_ip_happy_ctx_clear(ctx, معلومات);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_ip_happy_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات,
                                     bool *بشپړ)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(معلومات);
  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  پایله = cf_ip_ballers_shutdown(&ctx->ballers, معلومات, بشپړ);
  CURL_TRC_CF(معلومات, cf, "shutdown -> %d, done=%d", (صحيح)پایله, *بشپړ);
  أرجع پایله;
}

ساكن CURLcode cf_ip_happy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات,
                                           بنية easy_pollset *ps)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  إذا(!cf->connected) {
    پایله = cf_ip_ballers_pollset(&ctx->ballers, معلومات, ps);
    CURL_TRC_CF(معلومات, cf, "adjust_pollset -> %d, %u socks", (صحيح)پایله,
                ps->n);
  }
  أرجع پایله;
}

ساكن CURLcode cf_ip_happy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    bool *بشپړ)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  إذا(!معلومات)
    أرجع CURLE_FAILED_INIT;

  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *بشپړ = FALSE;

  إذا(!ctx->dns_resolved) {
    پایله = Curl_conn_dns_addr_پایله(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    إذا(!پایله)
      ctx->dns_resolved = TRUE;
    وإلا إذا(پایله == CURLE_AGAIN) {
      پایله = CURLE_OK;
    }
    وإلا /* real error */
      اذهب بهر;
  }

  بدل(ctx->state) {
  حالة SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, معلومات));
    DEBUGASSERT(!cf->connected);
    پایله = cf_ip_happy_init(cf, معلومات);
    إذا(پایله)
      اذهب بهر;
    ctx->state = SCFST_انتظار;
    FALLTHROUGH();
  حالة SCFST_انتظار:
    پایله = is_connected(cf, معلومات, بشپړ);
    إذا(!پایله && *بشپړ) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_بشپړ;
      cf->connected = TRUE;
      cf->بل = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = عدم;

      إذا(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrs_وخت(معلومات, TIMER_APPCONNECT); /* we are connected already */
#که_تعریف_شوی CURLVERBOSE
      إذا(Curl_trc_cf_is_verbose(cf, معلومات)) {
        بنية ip_quadruple ipquad;
        bool is_ipv6;
        إذا(!Curl_conn_cf_get_ip_info(cf->بل, معلومات, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(معلومات, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_پورټ);
        }
      }
#د_که_پای
      cf_ip_happy_ctx_clear(ctx, معلومات);
      Curl_expire_clear(معلومات, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(معلومات);
      معلومات->info.numconnects++; /* to track the # of connections made */
    }
    اكسر;
  حالة SCFST_بشپړ:
    *بشپړ = TRUE;
    اكسر;
  }
بهر:
  أرجع پایله;
}

ساكن bool cf_ip_happy_معلومات_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *معلومات)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  إذا(!cf->connected) {
    أرجع cf_ip_ballers_pending(&ctx->ballers, معلومات);
  }
  أرجع cf->بل->cft->has_معلومات_pending(cf->بل, معلومات);
}

ساكن CURLcode cf_ip_happy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  إذا(!cf->connected) {
    بدل(query) {
    حالة CF_QUERY_CONNECT_ځواب_MS: {
      *pres1 = cf_ip_ballers_min_ځواب_ms(&ctx->ballers, معلومات);
      CURL_TRC_CF(معلومات, cf, "query connect reply: %dms", *pres1);
      أرجع CURLE_OK;
    }
    افتراضي:
      اكسر;
    }
  }

  أرجع cf->بل ?
    cf->بل->cft->query(cf->بل, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن فراغ cf_ip_happy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(معلومات, cf, "destroy");
  إذا(ctx) {
    cf_ip_happy_ctx_clear(ctx, معلومات);
    cf_ip_happy_ctx_destroy(ctx, معلومات);
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
  cf_ip_happy_معلومات_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

ساكن CURLcode cf_ip_happy_create(بنية Curl_cfilter **pcf,
                                   بنية Curl_easy *معلومات,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   uint8_t transport_peer,
                                   بنية connectdata *conn,
                                   بنية Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  بنية cf_ip_happy_ctx *ctx = عدم;
  CURLcode پایله;

  (فراغ)معلومات;
  (فراغ)conn;
  *pcf = عدم;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  پایله = cf_ip_ballers_init(&ctx->ballers, معلومات,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              معلومات->سېټ.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  إذا(پایله)
    اذهب بهر;

  پایله = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

بهر:
  إذا(پایله) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, معلومات);
  }
  أرجع پایله;
}

CURLcode Curl_cf_ip_happy_ورننباسه_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *معلومات,
                                       بنية Curl_peer *origin,
                                       بنية Curl_peer *peer,
                                       uint8_t transport_peer,
                                       بنية Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  پایله = cf_ip_happy_create(&cf, معلومات, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  إذا(پایله)
    أرجع پایله;

  Curl_conn_cf_ورننباسه_after(cf_at, cf);
  أرجع CURLE_OK;
}
