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

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h> /* <netinet/tcp.h> may need it */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جيڪڏهن_پورو

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
#جيڪڏهن_وصف_نه_ٿيل UNITTESTS
ثابت
#جيڪڏهن_پورو
بنية transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
  { Curl_cf_h3_proxy_create, TRNSPRT_QUIC, TRUE },
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_TFTP
  { Curl_cf_udp_create, TRNSPRT_UDP, FALSE },
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
  { Curl_cf_unix_create, TRNSPRT_UNIX, FALSE },
  { Curl_cf_unix_create, TRNSPRT_UNIX, TRUE },
#جيڪڏهن_پورو
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

#جيڪڏهن_وصف_ٿيل UNITTESTS
/* @unittest 2600 */
UNITTEST فراغ debug_سيٽ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST فراغ debug_سيٽ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    إذا(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#جيڪڏهن_پورو /* UNITTESTS */

بنية cf_ai_iter {
  بنية Curl_cfilter *cf;
  بنية Curl_peer *peer;
  صحيح ai_family;
  بنانشان صحيح n;
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

ساكن ثابت بنية Curl_addrinfo *cf_ai_iter_ايندڙ(بنية cf_ai_iter *iter,
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

ساكن bool cf_ai_iter_has_وڌيڪ(بنية cf_ai_iter *iter,
                                بنية Curl_easy *معلومات)
{
  أرجع (iter->cf &&
          !!Curl_conn_dns_get_ai(معلومات, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

بنية cf_ip_attempt {
  بنية cf_ip_attempt *ايندڙ;
  بنية Curl_peer *origin;
  بنية Curl_peer *peer;
  بنية Curl_peer *tunnel_peer;
  بنية Curl_sockaddr_ex addr;
  بنية Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode نتيجو;
  صحيح ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  صحيح error;
  ٻٽو_انگ(connected);                    /* cf has connected */
  ٻٽو_انگ(shutdown);                     /* cf has shutdown */
  ٻٽو_انگ(inconclusive);                 /* connect was not a hard failure, we
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
  CURLcode نتيجو = CURLE_OK;

  *pa = عدم;
  a = curlx_calloc(1, حجم(*a));
  إذا(!a)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  Curl_peer_link(&a->origin, origin);
  Curl_peer_link(&a->peer, peer);
  a->transport_peer = transport_peer;
  Curl_peer_link(&a->tunnel_peer, tunnel_peer);
  a->tunnel_transport = tunnel_transport;
  a->addr = *addr;
  a->ai_family = ai_family;
  a->نتيجو = CURLE_OK;
  a->cf_create = cf_create;
  *pa = a;

  نتيجو = a->cf_create(&a->cf, معلومات, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  إذا(نتيجو)
    اذهب ٻاهريون;

  /* the new filter might have sub-filters */
  لكل(wcf = a->cf; wcf; wcf = wcf->ايندڙ) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

ٻاهريون:
  إذا(نتيجو) {
    cf_ip_attempt_free(a, معلومات);
    *pa = عدم;
  }
  أرجع نتيجو;
}

ساكن CURLcode cf_ip_attempt_connect(بنية cf_ip_attempt *a,
                                      بنية Curl_easy *معلومات,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  إذا(!a->نتيجو && !*connected) {
    /* evaluate again */
    a->نتيجو = Curl_conn_cf_connect(a->cf, معلومات, connected);

    إذا(!a->نتيجو) {
      إذا(*connected) {
        a->connected = TRUE;
      }
    }
    وإلا {
      إذا(a->نتيجو == CURLE_WEIRD_SERVER_جواب)
        a->inconclusive = TRUE;
      إذا(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, معلومات);
    }
  }
  أرجع a->نتيجو;
}

بنية cf_ip_ballers {
  بنية cf_ip_attempt *running;
  بنية cf_ip_attempt **running_پڇ;  /* &running or &(last->next) */
  بنية cf_ip_attempt *winner;
  بنية cf_ai_iter addr_iter;
#جيڪڏهن_وصف_ٿيل USE_IPV6
  بنية cf_ai_iter ipv6_iter;
#جيڪڏهن_پورو
  بنية Curl_peer *origin;
  بنية Curl_peer *peer;
  بنية Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  بنية curltime آخري_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode آخري_dead_نتيجو;  /* result of last discarded hard failure */
  صحيح آخري_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  ٻٽو_انگ(had_dead);              /* a hard failure was discarded */
};

ساكن CURLcode cf_ip_attempt_restart(بنية cf_ip_attempt *a,
                                      بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *wcf;
  CURLcode نتيجو;

  إذا(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, معلومات);

  a->نتيجو = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = عدم;

  نتيجو = a->cf_create(&a->cf, معلومات, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  إذا(!نتيجو) {
    bool dummy;
    /* the new filter might have sub-filters */
    لكل(wcf = a->cf; wcf; wcf = wcf->ايندڙ) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->نتيجو = cf_ip_attempt_connect(a, معلومات, &dummy);
  }
  أرجع نتيجو;
}

ساكن فراغ cf_ip_ballers_clear(بنية Curl_easy *معلومات,
                                بنية cf_ip_ballers *bs)
{
  طالما(bs->running) {
    بنية cf_ip_attempt *a = bs->running;
    bs->running = a->ايندڙ;
    cf_ip_attempt_free(a, معلومات);
  }
  bs->running_پڇ = &bs->running;
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
  bs->running_پڇ = &bs->running;
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
  bs->آخري_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  أرجع CURLE_OK;
}

ساكن فراغ cf_ip_ballers_prune(بنية cf_ip_ballers *bs,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                uint32_t max_concurrent)
{
  بنية cf_ip_attempt *a = عدم, **panchor;
  uint32_t ongoing = 0;

  لكل(a = bs->running; a; a = a->ايندڙ) {
    إذا(!a->نتيجو && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  طالما(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    إذا(!a->نتيجو && !a->connected) {
      *panchor = a->ايندڙ;
      إذا(!a->ايندڙ)
        bs->running_پڇ = panchor;
      a->ايندڙ = عدم;
      cf_ip_attempt_free(a, معلومات);
      --ongoing;
      CURL_TRC_CF(معلومات, cf, "discarding oldest attempt to keep limit");
    }
    وإلا {
      panchor = &a->ايندڙ;
    }
  }
}

ساكن CURLcode cf_ip_ballers_لاڳيتو(بنية cf_ip_ballers *bs,
                                  بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode نتيجو = CURLE_OK;
  بنية cf_ip_attempt *a = عدم, **panchor;
  bool do_وڌيڪ;
  timediff_t ايندڙ_expire_ms;
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
    a->نتيجو = cf_ip_attempt_connect(a, معلومات, connected);
    إذا(!a->نتيجو) {
      إذا(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(معلومات, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->ايندڙ;
        a->ايندڙ = عدم;
        طالما(bs->running) {
          a = bs->running;
          bs->running = a->ايندڙ;
          cf_ip_attempt_free(a, معلومات);
        }
        bs->running_پڇ = &bs->running;
        أرجع CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->ايندڙ;
    }
    وإلا إذا(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->ايندڙ;
    }
    وإلا {
      /* hard failure, discard right away */
      *panchor = a->ايندڙ;
      إذا(!a->ايندڙ)
        bs->running_پڇ = panchor;
      a->ايندڙ = عدم;
      bs->آخري_dead_نتيجو = a->نتيجو;
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
    do_وڌيڪ = TRUE;
  }
  وإلا {
    bool وڌيڪ_possible = cf_ai_iter_has_وڌيڪ(&bs->addr_iter, معلومات);
#جيڪڏهن_وصف_ٿيل USE_IPV6
    إذا(!وڌيڪ_possible)
      وڌيڪ_possible = cf_ai_iter_has_وڌيڪ(&bs->ipv6_iter, معلومات);
#جيڪڏهن_پورو
    do_وڌيڪ = وڌيڪ_possible &&
      (curlx_ptimediff_ms(pnow, &bs->آخري_attempt_started) >=
       bs->attempt_delay_ms);
    إذا(do_وڌيڪ)
      CURL_TRC_CF(معلومات, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  إذا(do_وڌيڪ) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ثابت بنية Curl_addrinfo *ai = عدم;
    صحيح ai_family = 0;
    CURL_TRC_CF(معلومات, cf, "want to do more");
#جيڪڏهن_وصف_ٿيل USE_IPV6
    إذا((bs->آخري_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_وڌيڪ(&bs->addr_iter, معلومات)) {
      ai = cf_ai_iter_ايندڙ(&bs->ipv6_iter, معلومات);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(معلومات, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#جيڪڏهن_پورو
    إذا(!ai) {
      ai = cf_ai_iter_ايندڙ(&bs->addr_iter, معلومات);
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

      نتيجو = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      إذا(نتيجو)
        اذهب ٻاهريون;

      نتيجو = cf_ip_attempt_new(&a, معلومات, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(معلومات, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (صحيح)نتيجو);
      إذا(نتيجو)
        اذهب ٻاهريون;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_پڇ = a;
      a->ايندڙ = عدم;
      bs->running_پڇ = &a->ايندڙ;
      bs->آخري_attempt_started = *pnow;
      bs->آخري_attempt_ai_family = ai_family;
      /* and run everything again */
      اذهب evaluate;
    }
    وإلا إذا(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->آخري_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      إذا(delay_ms <= 0) {
        CURL_TRC_CF(معلومات, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        لكل(a = bs->running; a; a = a->ايندڙ) {
          VERBOSE(++i);
          إذا(!a->inconclusive)
            استمر;
          نتيجو = cf_ip_attempt_restart(a, cf, معلومات);
          CURL_TRC_CF(معلومات, cf, "restarted baller %d -> %d", i, (صحيح)نتيجو);
          إذا(نتيجو) /* serious failure */
            اذهب ٻاهريون;
          bs->آخري_attempt_started = *pnow;
          اذهب evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      وإلا {
        /* let's wait some more before restarting */
        infof(معلومات, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_سيٽ(معلومات, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      اذهب ٻاهريون;
    }
    وإلا إذا(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(معلومات, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      نتيجو = bs->had_dead ? bs->آخري_dead_نتيجو : CURLE_COULDNT_CONNECT;
    }
  }

ٻاهريون:
  إذا(!نتيجو) {
    bool وڌيڪ_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(معلومات);
    ايندڙ_expire_ms = Curl_timeleft_now_ms(معلومات, pnow);
    إذا(ايندڙ_expire_ms < 0) {
      failf(معلومات, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(معلومات, عدم, TIMER_STARTSINGLE));
      أرجع CURLE_OPERATION_TIMEDOUT;
    }

    وڌيڪ_possible = cf_ai_iter_has_وڌيڪ(&bs->addr_iter, معلومات);
#جيڪڏهن_وصف_ٿيل USE_IPV6
    إذا(!وڌيڪ_possible)
      وڌيڪ_possible = cf_ai_iter_has_وڌيڪ(&bs->ipv6_iter, معلومات);
#جيڪڏهن_پورو
    إذا(وڌيڪ_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->آخري_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      ايندڙ_expire_ms = CURLMIN(ايندڙ_expire_ms, expire_ms);
      إذا(ايندڙ_expire_ms <= 0) {
        CURL_TRC_CF(معلومات, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        اذهب evaluate;
      }
      CURL_TRC_CF(معلومات, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", ايندڙ_expire_ms);
      Curl_expire_سيٽ(معلومات, EXPIRE_HAPPY_EYEBALLS, ايندڙ_expire_ms, pnow);
    }
  }
  أرجع نتيجو;
}

ساكن CURLcode cf_ip_ballers_shutdown(بنية cf_ip_ballers *bs,
                                       بنية Curl_easy *معلومات,
                                       bool *پورو)
{
  بنية cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *پورو = TRUE;
  لكل(a = bs->running; a; a = a->ايندڙ) {
    bool bdone = FALSE;
    إذا(a->shutdown || !a->cf)
      استمر;
    a->نتيجو = a->cf->cft->do_shutdown(a->cf, معلومات, &bdone);
    إذا(a->نتيجو || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    وإلا
      *پورو = FALSE;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ip_ballers_pollset(بنية cf_ip_ballers *bs,
                                      بنية Curl_easy *معلومات,
                                      بنية easy_pollset *ps)
{
  بنية cf_ip_attempt *a;
  CURLcode نتيجو = CURLE_OK;
  لكل(a = bs->running; a && !نتيجو; a = a->ايندڙ) {
    إذا(a->نتيجو)
      استمر;
    نتيجو = Curl_conn_cf_adjust_pollset(a->cf, معلومات, ps);
  }
  أرجع نتيجو;
}

ساكن bool cf_ip_ballers_pending(بنية cf_ip_ballers *bs,
                                  ثابت بنية Curl_easy *معلومات)
{
  بنية cf_ip_attempt *a;

  لكل(a = bs->running; a; a = a->ايندڙ) {
    إذا(a->نتيجو)
      استمر;
    إذا(a->cf && a->cf->cft->has_معلومات_pending(a->cf, معلومات))
      أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن صحيح cf_ip_ballers_min_جواب_ms(بنية cf_ip_ballers *bs,
                                      بنية Curl_easy *معلومات)
{
  صحيح جواب_ms = -1, breply_ms;
  بنية cf_ip_attempt *a;

  لكل(a = bs->running; a; a = a->ايندڙ) {
    إذا(a->cf && !a->cf->cft->query(a->cf, معلومات, CF_QUERY_CONNECT_جواب_MS,
                                   &breply_ms, عدم) &&
       breply_ms >= 0 && (جواب_ms < 0 || breply_ms < جواب_ms))
      جواب_ms = breply_ms;
  }
  أرجع جواب_ms;
}

تعريفنوع تعداد {
  SCFST_INIT,
  SCFST_انتظار,
  SCFST_پورو
} cf_connect_state;

بنية cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  بنية cf_ip_ballers ballers;
  ٻٽو_انگ(dns_resolved);
};

ساكن CURLcode is_connected(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات,
                             bool *connected)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  بنية connectdata *conn = cf->conn;
  CURLcode نتيجو;

  نتيجو = cf_ip_ballers_لاڳيتو(&ctx->ballers, cf, معلومات,
                             (bool)ctx->dns_resolved, connected);

  إذا(!نتيجو)
    أرجع CURLE_OK;
  وإلا {
    بنية Curl_peer *peer = عدم, *proxy_peer = عدم;
    حرف viamsg[160];

    peer = Curl_conn_get_پهريون_peer(conn, cf->sockindex);
    إذا(!conn->origin || !peer)
      أرجع CURLE_FAILED_INIT;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    إذا(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    وإلا إذا(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#جيڪڏهن_پورو

    viamsg[0] = 0;
    إذا(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
      إذا(peer->unix_socket)
        curl_msnprintf(viamsg, حجم(viamsg), " over unix://%s",
                       peer->hostname);
      وإلا
#جيڪڏهن_پورو
      curl_msnprintf(viamsg, حجم(viamsg), " via %s:%u",
                     peer->hostname, peer->پورٽ);
    }

    failf(معلومات, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->پورٽ, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(معلومات, عدم, TIMER_STARTSINGLE),
          curl_easy_strerror(نتيجو));

#جيڪڏهن_وصف_ٿيل SOCKETIMEDOUT
    إذا(SOCKETIMEDOUT == معلومات->state.os_errno)
      نتيجو = CURLE_OPERATION_TIMEDOUT;
#جيڪڏهن_پورو

    أرجع نتيجو;
  }
}

#وصف_ڪريو IP_HE_MAX_CONCURRENT_ATTEMPTS     6

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
#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#وإلا
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
#جيڪڏهن_پورو
  }
  وإلا { /* TCP/UDP/QUIC */
#جيڪڏهن_وصف_ٿيل USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#جيڪڏهن_پورو
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
                                     bool *پورو)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(معلومات);
  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  نتيجو = cf_ip_ballers_shutdown(&ctx->ballers, معلومات, پورو);
  CURL_TRC_CF(معلومات, cf, "shutdown -> %d, done=%d", (صحيح)نتيجو, *پورو);
  أرجع نتيجو;
}

ساكن CURLcode cf_ip_happy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات,
                                           بنية easy_pollset *ps)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  إذا(!cf->connected) {
    نتيجو = cf_ip_ballers_pollset(&ctx->ballers, معلومات, ps);
    CURL_TRC_CF(معلومات, cf, "adjust_pollset -> %d, %u socks", (صحيح)نتيجو,
                ps->n);
  }
  أرجع نتيجو;
}

ساكن CURLcode cf_ip_happy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    bool *پورو)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  إذا(!معلومات)
    أرجع CURLE_FAILED_INIT;

  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *پورو = FALSE;

  إذا(!ctx->dns_resolved) {
    نتيجو = Curl_conn_dns_addr_نتيجو(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    إذا(!نتيجو)
      ctx->dns_resolved = TRUE;
    وإلا إذا(نتيجو == CURLE_AGAIN) {
      نتيجو = CURLE_OK;
    }
    وإلا /* real error */
      اذهب ٻاهريون;
  }

  بدل(ctx->state) {
  حالة SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, معلومات));
    DEBUGASSERT(!cf->connected);
    نتيجو = cf_ip_happy_init(cf, معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
    ctx->state = SCFST_انتظار;
    FALLTHROUGH();
  حالة SCFST_انتظار:
    نتيجو = is_connected(cf, معلومات, پورو);
    إذا(!نتيجو && *پورو) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_پورو;
      cf->connected = TRUE;
      cf->ايندڙ = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = عدم;

      إذا(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrs_وقت(معلومات, TIMER_APPCONNECT); /* we are connected already */
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
      إذا(Curl_trc_cf_is_verbose(cf, معلومات)) {
        بنية ip_quadruple ipquad;
        bool is_ipv6;
        إذا(!Curl_conn_cf_get_ip_info(cf->ايندڙ, معلومات, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(معلومات, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_پورٽ);
        }
      }
#جيڪڏهن_پورو
      cf_ip_happy_ctx_clear(ctx, معلومات);
      Curl_expire_clear(معلومات, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(معلومات);
      معلومات->info.numconnects++; /* to track the # of connections made */
    }
    اكسر;
  حالة SCFST_پورو:
    *پورو = TRUE;
    اكسر;
  }
ٻاهريون:
  أرجع نتيجو;
}

ساكن bool cf_ip_happy_معلومات_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *معلومات)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  إذا(!cf->connected) {
    أرجع cf_ip_ballers_pending(&ctx->ballers, معلومات);
  }
  أرجع cf->ايندڙ->cft->has_معلومات_pending(cf->ايندڙ, معلومات);
}

ساكن CURLcode cf_ip_happy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ip_happy_ctx *ctx = cf->ctx;

  إذا(!cf->connected) {
    بدل(query) {
    حالة CF_QUERY_CONNECT_جواب_MS: {
      *pres1 = cf_ip_ballers_min_جواب_ms(&ctx->ballers, معلومات);
      CURL_TRC_CF(معلومات, cf, "query connect reply: %dms", *pres1);
      أرجع CURLE_OK;
    }
    افتراضي:
      اكسر;
    }
  }

  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, query, pres1, pres2) :
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
  CF_TYPE_تياري,
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
  CURLcode نتيجو;

  (فراغ)معلومات;
  (فراغ)conn;
  *pcf = عدم;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  نتيجو = cf_ip_ballers_init(&ctx->ballers, معلومات,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              معلومات->سيٽ.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  إذا(نتيجو)
    اذهب ٻاهريون;

  نتيجو = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

ٻاهريون:
  إذا(نتيجو) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, معلومات);
  }
  أرجع نتيجو;
}

CURLcode Curl_cf_ip_happy_وجهو_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *معلومات,
                                       بنية Curl_peer *origin,
                                       بنية Curl_peer *peer,
                                       uint8_t transport_peer,
                                       بنية Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  نتيجو = cf_ip_happy_create(&cf, معلومات, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  إذا(نتيجو)
    أرجع نتيجو;

  Curl_conn_cf_وجهو_after(cf_at, cf);
  أرجع CURLE_OK;
}
