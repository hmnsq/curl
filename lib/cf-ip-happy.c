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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h> /* <netinet/tcp.h> may need it */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LINUX_TCP_H
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/tcp.h>
#ਜਾਂਜੇ defined(HAVE_NETINET_TCP_H)
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/tcp.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-ip-happy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sockaddr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h" /* for quic cfilters */


ਬਣਤਰ transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

ਸਥਿਰ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNITTESTS
ਅਚਲ
#ਜੇ_ਅੰਤ
ਬਣਤਰ transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
  { Curl_cf_h3_proxy_create, TRNSPRT_QUIC, TRUE },
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TFTP
  { Curl_cf_udp_create, TRNSPRT_UDP, FALSE },
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  { Curl_cf_unix_create, TRNSPRT_UNIX, FALSE },
  { Curl_cf_unix_create, TRNSPRT_UNIX, TRUE },
#ਜੇ_ਅੰਤ
};

ਸਥਿਰ cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  ਲਈ(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ਜੇ((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      ਵਾਪਸ transport_providers[i].cf_create;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNITTESTS
/* @unittest 2600 */
UNITTEST ਖਾਲੀ debug_ਸੈੱਟ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST ਖਾਲੀ debug_ਸੈੱਟ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  ਲਈ(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ਜੇ(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#ਜੇ_ਅੰਤ /* UNITTESTS */

ਬਣਤਰ cf_ai_iter {
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ Curl_peer *peer;
  ਪੂਰਨ ai_family;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n;
};

ਸਥਿਰ ਖਾਲੀ cf_ai_iter_init(ਬਣਤਰ cf_ai_iter *iter,
                            ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_peer *peer,
                            ਪੂਰਨ ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_addrinfo *cf_ai_iter_ਅਗਲਾ(ਬਣਤਰ cf_ai_iter *iter,
                                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਬਣਤਰ Curl_addrinfo *addr;

  ਜੇ(!iter->cf)
    ਵਾਪਸ ਨਿਰਾ;

  addr = Curl_conn_dns_get_ai(ਅੰਕੜੇ, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  ਜੇ(addr)
    iter->n++;
  ਵਾਪਸ addr;
}

ਸਥਿਰ bool cf_ai_iter_has_ਹੋਰ(ਬਣਤਰ cf_ai_iter *iter,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ (iter->cf &&
          !!Curl_conn_dns_get_ai(ਅੰਕੜੇ, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

ਬਣਤਰ cf_ip_attempt {
  ਬਣਤਰ cf_ip_attempt *ਅਗਲਾ;
  ਬਣਤਰ Curl_peer *origin;
  ਬਣਤਰ Curl_peer *peer;
  ਬਣਤਰ Curl_peer *tunnel_peer;
  ਬਣਤਰ Curl_sockaddr_ex addr;
  ਬਣਤਰ Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  ਪੂਰਨ error;
  ਦੋਆਧਾਰੀ_ਅੰਕ(connected);                    /* cf has connected */
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutdown);                     /* cf has shutdown */
  ਦੋਆਧਾਰੀ_ਅੰਕ(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

ਸਥਿਰ ਖਾਲੀ cf_ip_attempt_free(ਬਣਤਰ cf_ip_attempt *a,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(a) {
    ਜੇ(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, ਅੰਕੜੇ);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

ਸਥਿਰ CURLcode cf_ip_attempt_new(ਬਣਤਰ cf_ip_attempt **pa,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_peer *origin,
                                  ਬਣਤਰ Curl_peer *peer,
                                  uint8_t transport_peer,
                                  ਬਣਤਰ Curl_sockaddr_ex *addr,
                                  ਪੂਰਨ ai_family,
                                  ਬਣਤਰ Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  ਬਣਤਰ Curl_cfilter *wcf;
  ਬਣਤਰ cf_ip_attempt *a;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *pa = ਨਿਰਾ;
  a = curlx_calloc(1, ਆਕਾਰ(*a));
  ਜੇ(!a)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  Curl_peer_link(&a->origin, origin);
  Curl_peer_link(&a->peer, peer);
  a->transport_peer = transport_peer;
  Curl_peer_link(&a->tunnel_peer, tunnel_peer);
  a->tunnel_transport = tunnel_transport;
  a->addr = *addr;
  a->ai_family = ai_family;
  a->ਨਤੀਜਾ = CURLE_OK;
  a->cf_create = cf_create;
  *pa = a;

  ਨਤੀਜਾ = a->cf_create(&a->cf, ਅੰਕੜੇ, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* the new filter might have sub-filters */
  ਲਈ(wcf = a->cf; wcf; wcf = wcf->ਅਗਲਾ) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    cf_ip_attempt_free(a, ਅੰਕੜੇ);
    *pa = ਨਿਰਾ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_ip_attempt_connect(ਬਣਤਰ cf_ip_attempt *a,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  ਜੇ(!a->ਨਤੀਜਾ && !*connected) {
    /* evaluate again */
    a->ਨਤੀਜਾ = Curl_conn_cf_connect(a->cf, ਅੰਕੜੇ, connected);

    ਜੇ(!a->ਨਤੀਜਾ) {
      ਜੇ(*connected) {
        a->connected = TRUE;
      }
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(a->ਨਤੀਜਾ == CURLE_WEIRD_SERVER_ਜਵਾਬ)
        a->inconclusive = TRUE;
      ਜੇ(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, ਅੰਕੜੇ);
    }
  }
  ਵਾਪਸ a->ਨਤੀਜਾ;
}

ਬਣਤਰ cf_ip_ballers {
  ਬਣਤਰ cf_ip_attempt *running;
  ਬਣਤਰ cf_ip_attempt **running_ਪੂਛ;  /* &running or &(last->next) */
  ਬਣਤਰ cf_ip_attempt *winner;
  ਬਣਤਰ cf_ai_iter addr_iter;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ cf_ai_iter ipv6_iter;
#ਜੇ_ਅੰਤ
  ਬਣਤਰ Curl_peer *origin;
  ਬਣਤਰ Curl_peer *peer;
  ਬਣਤਰ Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  ਬਣਤਰ curltime ਆਖਰੀ_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode ਆਖਰੀ_dead_ਨਤੀਜਾ;  /* result of last discarded hard failure */
  ਪੂਰਨ ਆਖਰੀ_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  ਦੋਆਧਾਰੀ_ਅੰਕ(had_dead);              /* a hard failure was discarded */
};

ਸਥਿਰ CURLcode cf_ip_attempt_restart(ਬਣਤਰ cf_ip_attempt *a,
                                      ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *wcf;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, ਅੰਕੜੇ);

  a->ਨਤੀਜਾ = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = ਨਿਰਾ;

  ਨਤੀਜਾ = a->cf_create(&a->cf, ਅੰਕੜੇ, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  ਜੇ(!ਨਤੀਜਾ) {
    bool dummy;
    /* the new filter might have sub-filters */
    ਲਈ(wcf = a->cf; wcf; wcf = wcf->ਅਗਲਾ) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->ਨਤੀਜਾ = cf_ip_attempt_connect(a, ਅੰਕੜੇ, &dummy);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_ip_ballers_clear(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ cf_ip_ballers *bs)
{
  ਜਦੋਂ(bs->running) {
    ਬਣਤਰ cf_ip_attempt *a = bs->running;
    bs->running = a->ਅਗਲਾ;
    cf_ip_attempt_free(a, ਅੰਕੜੇ);
  }
  bs->running_ਪੂਛ = &bs->running;
  cf_ip_attempt_free(bs->winner, ਅੰਕੜੇ);
  bs->winner = ਨਿਰਾ;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

ਸਥਿਰ CURLcode cf_ip_ballers_init(ਬਣਤਰ cf_ip_ballers *bs,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ Curl_peer *origin,
                                   ਬਣਤਰ Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ਬਣਤਰ Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, ਆਕਾਰ(*bs));
  bs->running_ਪੂਛ = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  ਜੇ(!bs->cf_create) {
    failf(ਅੰਕੜੇ, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->ਆਖਰੀ_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cf_ip_ballers_prune(ਬਣਤਰ cf_ip_ballers *bs,
                                ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                uint32_t max_concurrent)
{
  ਬਣਤਰ cf_ip_attempt *a = ਨਿਰਾ, **panchor;
  uint32_t ongoing = 0;

  ਲਈ(a = bs->running; a; a = a->ਅਗਲਾ) {
    ਜੇ(!a->ਨਤੀਜਾ && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  ਜਦੋਂ(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    ਜੇ(!a->ਨਤੀਜਾ && !a->connected) {
      *panchor = a->ਅਗਲਾ;
      ਜੇ(!a->ਅਗਲਾ)
        bs->running_ਪੂਛ = panchor;
      a->ਅਗਲਾ = ਨਿਰਾ;
      cf_ip_attempt_free(a, ਅੰਕੜੇ);
      --ongoing;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "discarding oldest attempt to keep limit");
    }
    ਨਹੀਂਤਾਂ {
      panchor = &a->ਅਗਲਾ;
    }
  }
}

ਸਥਿਰ CURLcode cf_ip_ballers_ਲੜੀਵਾਰ(ਬਣਤਰ cf_ip_ballers *bs,
                                  ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ cf_ip_attempt *a = ਨਿਰਾ, **panchor;
  bool do_ਹੋਰ;
  timediff_t ਅਗਲਾ_expire_ms;
  uint32_t inconclusive, ongoing;
  ਅਚਲ ਬਣਤਰ curltime *pnow = ਨਿਰਾ;
  VERBOSE(ਪੂਰਨ i);

  ਜੇ(bs->winner)
    ਵਾਪਸ CURLE_OK;

evaluate:
  ਜੇ(Curl_timeleft_now_ms(ਅੰਕੜੇ, Curl_pgrs_now(ਅੰਕੜੇ)) < 0) {
    failf(ਅੰਕੜੇ, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(ਅੰਕੜੇ, ਨਿਰਾ, TIMER_STARTSINGLE));
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  ਲਈ(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->ਨਤੀਜਾ = cf_ip_attempt_connect(a, ਅੰਕੜੇ, connected);
    ਜੇ(!a->ਨਤੀਜਾ) {
      ਜੇ(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->ਅਗਲਾ;
        a->ਅਗਲਾ = ਨਿਰਾ;
        ਜਦੋਂ(bs->running) {
          a = bs->running;
          bs->running = a->ਅਗਲਾ;
          cf_ip_attempt_free(a, ਅੰਕੜੇ);
        }
        bs->running_ਪੂਛ = &bs->running;
        ਵਾਪਸ CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->ਅਗਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->ਅਗਲਾ;
    }
    ਨਹੀਂਤਾਂ {
      /* hard failure, discard right away */
      *panchor = a->ਅਗਲਾ;
      ਜੇ(!a->ਅਗਲਾ)
        bs->running_ਪੂਛ = panchor;
      a->ਅਗਲਾ = ਨਿਰਾ;
      bs->ਆਖਰੀ_dead_ਨਤੀਜਾ = a->ਨਤੀਜਾ;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, ਅੰਕੜੇ);
    }
  }
  ਜੇ(bs->running)
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(ਅੰਕੜੇ);
  ਜੇ(!ongoing) {
    do_ਹੋਰ = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    bool ਹੋਰ_possible = cf_ai_iter_has_ਹੋਰ(&bs->addr_iter, ਅੰਕੜੇ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਜੇ(!ਹੋਰ_possible)
      ਹੋਰ_possible = cf_ai_iter_has_ਹੋਰ(&bs->ipv6_iter, ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ
    do_ਹੋਰ = ਹੋਰ_possible &&
      (curlx_ptimediff_ms(pnow, &bs->ਆਖਰੀ_attempt_started) >=
       bs->attempt_delay_ms);
    ਜੇ(do_ਹੋਰ)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  ਜੇ(do_ਹੋਰ) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai = ਨਿਰਾ;
    ਪੂਰਨ ai_family = 0;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "want to do more");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਜੇ((bs->ਆਖਰੀ_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_ਹੋਰ(&bs->addr_iter, ਅੰਕੜੇ)) {
      ai = cf_ai_iter_ਅਗਲਾ(&bs->ipv6_iter, ਅੰਕੜੇ);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#ਜੇ_ਅੰਤ
    ਜੇ(!ai) {
      ai = cf_ai_iter_ਅਗਲਾ(&bs->addr_iter, ਅੰਕੜੇ);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(ਅੰਕੜੇ);

    ਜੇ(ai) {  /* try another address */
      ਬਣਤਰ Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      ਜੇ(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, ਅੰਕੜੇ, bs->max_concurrent - 1);

      ਨਤੀਜਾ = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;

      ਨਤੀਜਾ = cf_ip_attempt_new(&a, ਅੰਕੜੇ, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (ਪੂਰਨ)ਨਤੀਜਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_ਪੂਛ = a;
      a->ਅਗਲਾ = ਨਿਰਾ;
      bs->running_ਪੂਛ = &a->ਅਗਲਾ;
      bs->ਆਖਰੀ_attempt_started = *pnow;
      bs->ਆਖਰੀ_attempt_ai_family = ai_family;
      /* and run everything again */
      ਜਾਓ evaluate;
    }
    ਨਹੀਂਤਾਂ ਜੇ(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->ਆਖਰੀ_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      ਜੇ(delay_ms <= 0) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        ਲਈ(a = bs->running; a; a = a->ਅਗਲਾ) {
          VERBOSE(++i);
          ਜੇ(!a->inconclusive)
            ਜਾਰੀ;
          ਨਤੀਜਾ = cf_ip_attempt_restart(a, cf, ਅੰਕੜੇ);
          CURL_TRC_CF(ਅੰਕੜੇ, cf, "restarted baller %d -> %d", i, (ਪੂਰਨ)ਨਤੀਜਾ);
          ਜੇ(ਨਤੀਜਾ) /* serious failure */
            ਜਾਓ ਬਾਹਰਲਾ;
          bs->ਆਖਰੀ_attempt_started = *pnow;
          ਜਾਓ evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      ਨਹੀਂਤਾਂ {
        /* let's wait some more before restarting */
        infof(ਅੰਕੜੇ, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      ਨਤੀਜਾ = bs->had_dead ? bs->ਆਖਰੀ_dead_ਨਤੀਜਾ : CURLE_COULDNT_CONNECT;
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ(!ਨਤੀਜਾ) {
    bool ਹੋਰ_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(ਅੰਕੜੇ);
    ਅਗਲਾ_expire_ms = Curl_timeleft_now_ms(ਅੰਕੜੇ, pnow);
    ਜੇ(ਅਗਲਾ_expire_ms < 0) {
      failf(ਅੰਕੜੇ, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(ਅੰਕੜੇ, ਨਿਰਾ, TIMER_STARTSINGLE));
      ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
    }

    ਹੋਰ_possible = cf_ai_iter_has_ਹੋਰ(&bs->addr_iter, ਅੰਕੜੇ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਜੇ(!ਹੋਰ_possible)
      ਹੋਰ_possible = cf_ai_iter_has_ਹੋਰ(&bs->ipv6_iter, ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ
    ਜੇ(ਹੋਰ_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->ਆਖਰੀ_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      ਅਗਲਾ_expire_ms = CURLMIN(ਅਗਲਾ_expire_ms, expire_ms);
      ਜੇ(ਅਗਲਾ_expire_ms <= 0) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        ਜਾਓ evaluate;
      }
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", ਅਗਲਾ_expire_ms);
      Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_HAPPY_EYEBALLS, ਅਗਲਾ_expire_ms, pnow);
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_ip_ballers_shutdown(ਬਣਤਰ cf_ip_ballers *bs,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *ਪੂਰਾ = TRUE;
  ਲਈ(a = bs->running; a; a = a->ਅਗਲਾ) {
    bool bdone = FALSE;
    ਜੇ(a->shutdown || !a->cf)
      ਜਾਰੀ;
    a->ਨਤੀਜਾ = a->cf->cft->do_shutdown(a->cf, ਅੰਕੜੇ, &bdone);
    ਜੇ(a->ਨਤੀਜਾ || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    ਨਹੀਂਤਾਂ
      *ਪੂਰਾ = FALSE;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_ip_ballers_pollset(ਬਣਤਰ cf_ip_ballers *bs,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ cf_ip_attempt *a;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਲਈ(a = bs->running; a && !ਨਤੀਜਾ; a = a->ਅਗਲਾ) {
    ਜੇ(a->ਨਤੀਜਾ)
      ਜਾਰੀ;
    ਨਤੀਜਾ = Curl_conn_cf_adjust_pollset(a->cf, ਅੰਕੜੇ, ps);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool cf_ip_ballers_pending(ਬਣਤਰ cf_ip_ballers *bs,
                                  ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ip_attempt *a;

  ਲਈ(a = bs->running; a; a = a->ਅਗਲਾ) {
    ਜੇ(a->ਨਤੀਜਾ)
      ਜਾਰੀ;
    ਜੇ(a->cf && a->cf->cft->has_ਅੰਕੜੇ_pending(a->cf, ਅੰਕੜੇ))
      ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਪੂਰਨ cf_ip_ballers_min_ਜਵਾਬ_ms(ਬਣਤਰ cf_ip_ballers *bs,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਪੂਰਨ ਜਵਾਬ_ms = -1, breply_ms;
  ਬਣਤਰ cf_ip_attempt *a;

  ਲਈ(a = bs->running; a; a = a->ਅਗਲਾ) {
    ਜੇ(a->cf && !a->cf->cft->query(a->cf, ਅੰਕੜੇ, CF_QUERY_CONNECT_ਜਵਾਬ_MS,
                                   &breply_ms, ਨਿਰਾ) &&
       breply_ms >= 0 && (ਜਵਾਬ_ms < 0 || breply_ms < ਜਵਾਬ_ms))
      ਜਵਾਬ_ms = breply_ms;
  }
  ਵਾਪਸ ਜਵਾਬ_ms;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  SCFST_INIT,
  SCFST_ਉਡੀਕ,
  SCFST_ਪੂਰਾ
} cf_connect_state;

ਬਣਤਰ cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  ਬਣਤਰ cf_ip_ballers ballers;
  ਦੋਆਧਾਰੀ_ਅੰਕ(dns_resolved);
};

ਸਥਿਰ CURLcode is_connected(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             bool *connected)
{
  ਬਣਤਰ cf_ip_happy_ctx *ctx = cf->ctx;
  ਬਣਤਰ connectdata *conn = cf->conn;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = cf_ip_ballers_ਲੜੀਵਾਰ(&ctx->ballers, cf, ਅੰਕੜੇ,
                             (bool)ctx->dns_resolved, connected);

  ਜੇ(!ਨਤੀਜਾ)
    ਵਾਪਸ CURLE_OK;
  ਨਹੀਂਤਾਂ {
    ਬਣਤਰ Curl_peer *peer = ਨਿਰਾ, *proxy_peer = ਨਿਰਾ;
    ਅੱਖਰ viamsg[160];

    peer = Curl_conn_get_ਪਹਿਲਾ_peer(conn, cf->sockindex);
    ਜੇ(!conn->origin || !peer)
      ਵਾਪਸ CURLE_FAILED_INIT;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    ਜੇ(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    ਨਹੀਂਤਾਂ ਜੇ(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#ਜੇ_ਅੰਤ

    viamsg[0] = 0;
    ਜੇ(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
      ਜੇ(peer->unix_socket)
        curl_msnprintf(viamsg, ਆਕਾਰ(viamsg), " over unix://%s",
                       peer->hostname);
      ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
      curl_msnprintf(viamsg, ਆਕਾਰ(viamsg), " via %s:%u",
                     peer->hostname, peer->ਪੋਰਟ);
    }

    failf(ਅੰਕੜੇ, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->ਪੋਰਟ, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(ਅੰਕੜੇ, ਨਿਰਾ, TIMER_STARTSINGLE),
          curl_easy_strerror(ਨਤੀਜਾ));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SOCKETIMEDOUT
    ਜੇ(SOCKETIMEDOUT == ਅੰਕੜੇ->state.os_errno)
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਨਤੀਜਾ;
  }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IP_HE_MAX_CONCURRENT_ATTEMPTS     6

ਸਥਿਰ CURLcode cf_ip_happy_init(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ip_happy_ctx *ctx = cf->ctx;

  ਜੇ(Curl_timeleft_ms(ਅੰਕੜੇ) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(ਅੰਕੜੇ, "Connection time-out");
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
  }

  ਜੇ(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ { /* TCP/UDP/QUIC */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#ਜੇ_ਅੰਤ
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cf_ip_happy_ctx_clear(ਬਣਤਰ cf_ip_happy_ctx *ctx,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  DEBUGASSERT(ctx);
  ਜੇ(ctx)
    cf_ip_ballers_clear(ਅੰਕੜੇ, &ctx->ballers);
}

ਸਥਿਰ ਖਾਲੀ cf_ip_happy_ctx_destroy(ਬਣਤਰ cf_ip_happy_ctx *ctx,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ctx) {
    cf_ip_happy_ctx_clear(ctx, ਅੰਕੜੇ);
    curlx_free(ctx);
  }
}

ਸਥਿਰ CURLcode cf_ip_happy_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  ਨਤੀਜਾ = cf_ip_ballers_shutdown(&ctx->ballers, ਅੰਕੜੇ, ਪੂਰਾ);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown -> %d, done=%d", (ਪੂਰਨ)ਨਤੀਜਾ, *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_ip_happy_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!cf->connected) {
    ਨਤੀਜਾ = cf_ip_ballers_pollset(&ctx->ballers, ਅੰਕੜੇ, ps);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust_pollset -> %d, %u socks", (ਪੂਰਨ)ਨਤੀਜਾ,
                ps->n);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_ip_happy_connect(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *ਪੂਰਾ = FALSE;

  ਜੇ(!ctx->dns_resolved) {
    ਨਤੀਜਾ = Curl_conn_dns_addr_ਨਤੀਜਾ(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    ਜੇ(!ਨਤੀਜਾ)
      ctx->dns_resolved = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      ਨਤੀਜਾ = CURLE_OK;
    }
    ਨਹੀਂਤਾਂ /* real error */
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਚੋਣ(ctx->state) {
  ਹਾਲਤ SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ));
    DEBUGASSERT(!cf->connected);
    ਨਤੀਜਾ = cf_ip_happy_init(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ctx->state = SCFST_ਉਡੀਕ;
    FALLTHROUGH();
  ਹਾਲਤ SCFST_ਉਡੀਕ:
    ਨਤੀਜਾ = is_connected(cf, ਅੰਕੜੇ, ਪੂਰਾ);
    ਜੇ(!ਨਤੀਜਾ && *ਪੂਰਾ) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_ਪੂਰਾ;
      cf->connected = TRUE;
      cf->ਅਗਲਾ = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = ਨਿਰਾ;

      ਜੇ(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_APPCONNECT); /* we are connected already */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
      ਜੇ(Curl_trc_cf_is_verbose(cf, ਅੰਕੜੇ)) {
        ਬਣਤਰ ip_quadruple ipquad;
        bool is_ipv6;
        ਜੇ(!Curl_conn_cf_get_ip_info(cf->ਅਗਲਾ, ਅੰਕੜੇ, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(ਅੰਕੜੇ, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_ਪੋਰਟ);
        }
      }
#ਜੇ_ਅੰਤ
      cf_ip_happy_ctx_clear(ctx, ਅੰਕੜੇ);
      Curl_expire_clear(ਅੰਕੜੇ, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(ਅੰਕੜੇ);
      ਅੰਕੜੇ->info.numconnects++; /* to track the # of connections made */
    }
    ਰੋਕੋ;
  ਹਾਲਤ SCFST_ਪੂਰਾ:
    *ਪੂਰਾ = TRUE;
    ਰੋਕੋ;
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool cf_ip_happy_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                                     ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ip_happy_ctx *ctx = cf->ctx;

  ਜੇ(!cf->connected) {
    ਵਾਪਸ cf_ip_ballers_pending(&ctx->ballers, ਅੰਕੜੇ);
  }
  ਵਾਪਸ cf->ਅਗਲਾ->cft->has_ਅੰਕੜੇ_pending(cf->ਅਗਲਾ, ਅੰਕੜੇ);
}

ਸਥਿਰ CURLcode cf_ip_happy_query(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ cf_ip_happy_ctx *ctx = cf->ctx;

  ਜੇ(!cf->connected) {
    ਚੋਣ(query) {
    ਹਾਲਤ CF_QUERY_CONNECT_ਜਵਾਬ_MS: {
      *pres1 = cf_ip_ballers_min_ਜਵਾਬ_ms(&ctx->ballers, ਅੰਕੜੇ);
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "query connect reply: %dms", *pres1);
      ਵਾਪਸ CURLE_OK;
    }
    ਮੂਲ:
      ਰੋਕੋ;
    }
  }

  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ਸਥਿਰ ਖਾਲੀ cf_ip_happy_destroy(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "destroy");
  ਜੇ(ctx) {
    cf_ip_happy_ctx_clear(ctx, ਅੰਕੜੇ);
    cf_ip_happy_ctx_destroy(ctx, ਅੰਕੜੇ);
  }
}

ਬਣਤਰ Curl_cftype Curl_cft_ip_happy = {
  "HAPPY-EYEBALLS",
  CF_TYPE_ਤਿਆਰੀ,
  CURL_LOG_LVL_NONE,
  cf_ip_happy_destroy,
  cf_ip_happy_connect,
  cf_ip_happy_shutdown,
  cf_ip_happy_adjust_pollset,
  cf_ip_happy_ਅੰਕੜੇ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

ਸਥਿਰ CURLcode cf_ip_happy_create(ਬਣਤਰ Curl_cfilter **pcf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ Curl_peer *origin,
                                   ਬਣਤਰ Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ਬਣਤਰ connectdata *conn,
                                   ਬਣਤਰ Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  ਬਣਤਰ cf_ip_happy_ctx *ctx = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)conn;
  *pcf = ਨਿਰਾ;
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = cf_ip_ballers_init(&ctx->ballers, ਅੰਕੜੇ,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              ਅੰਕੜੇ->ਸੈੱਟ.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, ਅੰਕੜੇ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_ip_happy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ Curl_peer *origin,
                                       ਬਣਤਰ Curl_peer *peer,
                                       uint8_t transport_peer,
                                       ਬਣਤਰ Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  ਨਤੀਜਾ = cf_ip_happy_create(&cf, ਅੰਕੜੇ, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  Curl_conn_cf_ਪਾਓ_after(cf_at, cf);
  ਵਾਪਸ CURLE_OK;
}
