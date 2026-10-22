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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h> /* <netinet/tcp.h> may need it */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LINUX_TCP_H
#ಅನ್ತರ್ಭಾವ <linux/tcp.h>
#ಅಥವಾಆದರೆ defined(HAVE_NETINET_TCP_H)
#ಅನ್ತರ್ಭಾವ <netinet/tcp.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

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


ರಚನೆ transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

ಸ್ಥಿರ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNITTESTS
ಸ್ಥಿರಾಂಕ
#ಆದರೆ_ಅಂತ್ಯ
ರಚನೆ transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
  { Curl_cf_h3_proxy_create, TRNSPRT_QUIC, TRUE },
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_TFTP
  { Curl_cf_udp_create, TRNSPRT_UDP, FALSE },
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  { Curl_cf_unix_create, TRNSPRT_UNIX, FALSE },
  { Curl_cf_unix_create, TRNSPRT_UNIX, TRUE },
#ಆದರೆ_ಅಂತ್ಯ
};

ಸ್ಥಿರ cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ಆದರೆ((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      ಹಿಂತಿರುಗಿ transport_providers[i].cf_create;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNITTESTS
/* @unittest 2600 */
UNITTEST ಶೂನ್ಯ debug_ಗಣ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST ಶೂನ್ಯ debug_ಗಣ_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    ಆದರೆ(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* UNITTESTS */

ರಚನೆ cf_ai_iter {
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ Curl_peer *peer;
  ಪೂರ್ಣಾಂಕ ai_family;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n;
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_ai_iter_init(ರಚನೆ cf_ai_iter *iter,
                            ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_peer *peer,
                            ಪೂರ್ಣಾಂಕ ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *cf_ai_iter_ಮುಂದಿನ(ರಚನೆ cf_ai_iter *iter,
                                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *addr;

  ಆದರೆ(!iter->cf)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  addr = Curl_conn_dns_get_ai(ದತ್ತಾಂಶ, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  ಆದರೆ(addr)
    iter->n++;
  ಹಿಂತಿರುಗಿ addr;
}

ಸ್ಥಿರ bool cf_ai_iter_has_ಇನ್ನಷ್ಟು(ರಚನೆ cf_ai_iter *iter,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ (iter->cf &&
          !!Curl_conn_dns_get_ai(ದತ್ತಾಂಶ, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

ರಚನೆ cf_ip_attempt {
  ರಚನೆ cf_ip_attempt *ಮುಂದಿನ;
  ರಚನೆ Curl_peer *origin;
  ರಚನೆ Curl_peer *peer;
  ರಚನೆ Curl_peer *tunnel_peer;
  ರಚನೆ Curl_sockaddr_ex addr;
  ರಚನೆ Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  ಪೂರ್ಣಾಂಕ error;
  ಬಿಟ್(connected);                    /* cf has connected */
  ಬಿಟ್(shutdown);                     /* cf has shutdown */
  ಬಿಟ್(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_attempt_free(ರಚನೆ cf_ip_attempt *a,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(a) {
    ಆದರೆ(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, ದತ್ತಾಂಶ);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

ಸ್ಥಿರ CURLcode cf_ip_attempt_new(ರಚನೆ cf_ip_attempt **pa,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_peer *origin,
                                  ರಚನೆ Curl_peer *peer,
                                  uint8_t transport_peer,
                                  ರಚನೆ Curl_sockaddr_ex *addr,
                                  ಪೂರ್ಣಾಂಕ ai_family,
                                  ರಚನೆ Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  ರಚನೆ Curl_cfilter *wcf;
  ರಚನೆ cf_ip_attempt *a;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *pa = ಖಾಲಿ;
  a = curlx_calloc(1, ಗಾತ್ರ(*a));
  ಆದರೆ(!a)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  Curl_peer_link(&a->origin, origin);
  Curl_peer_link(&a->peer, peer);
  a->transport_peer = transport_peer;
  Curl_peer_link(&a->tunnel_peer, tunnel_peer);
  a->tunnel_transport = tunnel_transport;
  a->addr = *addr;
  a->ai_family = ai_family;
  a->ಫಲಿತಾಂಶ = CURLE_OK;
  a->cf_create = cf_create;
  *pa = a;

  ಫಲಿತಾಂಶ = a->cf_create(&a->cf, ದತ್ತಾಂಶ, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* the new filter might have sub-filters */
  ಗಾಗಿ(wcf = a->cf; wcf; wcf = wcf->ಮುಂದಿನ) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    cf_ip_attempt_free(a, ದತ್ತಾಂಶ);
    *pa = ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_ip_attempt_connect(ರಚನೆ cf_ip_attempt *a,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  ಆದರೆ(!a->ಫಲಿತಾಂಶ && !*connected) {
    /* evaluate again */
    a->ಫಲಿತಾಂಶ = Curl_conn_cf_connect(a->cf, ದತ್ತಾಂಶ, connected);

    ಆದರೆ(!a->ಫಲಿತಾಂಶ) {
      ಆದರೆ(*connected) {
        a->connected = TRUE;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(a->ಫಲಿತಾಂಶ == CURLE_WEIRD_SERVER_ಉತ್ತರ)
        a->inconclusive = TRUE;
      ಆದರೆ(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, ದತ್ತಾಂಶ);
    }
  }
  ಹಿಂತಿರುಗಿ a->ಫಲಿತಾಂಶ;
}

ರಚನೆ cf_ip_ballers {
  ರಚನೆ cf_ip_attempt *running;
  ರಚನೆ cf_ip_attempt **running_ಬಾಲ;  /* &running or &(last->next) */
  ರಚನೆ cf_ip_attempt *winner;
  ರಚನೆ cf_ai_iter addr_iter;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ cf_ai_iter ipv6_iter;
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ Curl_peer *origin;
  ರಚನೆ Curl_peer *peer;
  ರಚನೆ Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  ರಚನೆ curltime ಕೊನೆಯ_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode ಕೊನೆಯ_dead_ಫಲಿತಾಂಶ;  /* result of last discarded hard failure */
  ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  ಬಿಟ್(had_dead);              /* a hard failure was discarded */
};

ಸ್ಥಿರ CURLcode cf_ip_attempt_restart(ರಚನೆ cf_ip_attempt *a,
                                      ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *wcf;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, ದತ್ತಾಂಶ);

  a->ಫಲಿತಾಂಶ = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = ಖಾಲಿ;

  ಫಲಿತಾಂಶ = a->cf_create(&a->cf, ದತ್ತಾಂಶ, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    bool dummy;
    /* the new filter might have sub-filters */
    ಗಾಗಿ(wcf = a->cf; wcf; wcf = wcf->ಮುಂದಿನ) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->ಫಲಿತಾಂಶ = cf_ip_attempt_connect(a, ದತ್ತಾಂಶ, &dummy);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_ballers_clear(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ cf_ip_ballers *bs)
{
  ಆಗ(bs->running) {
    ರಚನೆ cf_ip_attempt *a = bs->running;
    bs->running = a->ಮುಂದಿನ;
    cf_ip_attempt_free(a, ದತ್ತಾಂಶ);
  }
  bs->running_ಬಾಲ = &bs->running;
  cf_ip_attempt_free(bs->winner, ದತ್ತಾಂಶ);
  bs->winner = ಖಾಲಿ;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

ಸ್ಥಿರ CURLcode cf_ip_ballers_init(ರಚನೆ cf_ip_ballers *bs,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ Curl_peer *origin,
                                   ರಚನೆ Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ರಚನೆ Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, ಗಾತ್ರ(*bs));
  bs->running_ಬಾಲ = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  ಆದರೆ(!bs->cf_create) {
    failf(ದತ್ತಾಂಶ, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->ಕೊನೆಯ_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_ballers_prune(ರಚನೆ cf_ip_ballers *bs,
                                ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                uint32_t max_concurrent)
{
  ರಚನೆ cf_ip_attempt *a = ಖಾಲಿ, **panchor;
  uint32_t ongoing = 0;

  ಗಾಗಿ(a = bs->running; a; a = a->ಮುಂದಿನ) {
    ಆದರೆ(!a->ಫಲಿತಾಂಶ && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  ಆಗ(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    ಆದರೆ(!a->ಫಲಿತಾಂಶ && !a->connected) {
      *panchor = a->ಮುಂದಿನ;
      ಆದರೆ(!a->ಮುಂದಿನ)
        bs->running_ಬಾಲ = panchor;
      a->ಮುಂದಿನ = ಖಾಲಿ;
      cf_ip_attempt_free(a, ದತ್ತಾಂಶ);
      --ongoing;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "discarding oldest attempt to keep limit");
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      panchor = &a->ಮುಂದಿನ;
    }
  }
}

ಸ್ಥಿರ CURLcode cf_ip_ballers_ಸತತ(ರಚನೆ cf_ip_ballers *bs,
                                  ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ cf_ip_attempt *a = ಖಾಲಿ, **panchor;
  bool do_ಇನ್ನಷ್ಟು;
  timediff_t ಮುಂದಿನ_expire_ms;
  uint32_t inconclusive, ongoing;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = ಖಾಲಿ;
  VERBOSE(ಪೂರ್ಣಾಂಕ i);

  ಆದರೆ(bs->winner)
    ಹಿಂತಿರುಗಿ CURLE_OK;

evaluate:
  ಆದರೆ(Curl_timeleft_now_ms(ದತ್ತಾಂಶ, Curl_pgrs_now(ದತ್ತಾಂಶ)) < 0) {
    failf(ದತ್ತಾಂಶ, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(ದತ್ತಾಂಶ, ಖಾಲಿ, TIMER_STARTSINGLE));
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  ಗಾಗಿ(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->ಫಲಿತಾಂಶ = cf_ip_attempt_connect(a, ದತ್ತಾಂಶ, connected);
    ಆದರೆ(!a->ಫಲಿತಾಂಶ) {
      ಆದರೆ(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->ಮುಂದಿನ;
        a->ಮುಂದಿನ = ಖಾಲಿ;
        ಆಗ(bs->running) {
          a = bs->running;
          bs->running = a->ಮುಂದಿನ;
          cf_ip_attempt_free(a, ದತ್ತಾಂಶ);
        }
        bs->running_ಬಾಲ = &bs->running;
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->ಮುಂದಿನ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->ಮುಂದಿನ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* hard failure, discard right away */
      *panchor = a->ಮುಂದಿನ;
      ಆದರೆ(!a->ಮುಂದಿನ)
        bs->running_ಬಾಲ = panchor;
      a->ಮುಂದಿನ = ಖಾಲಿ;
      bs->ಕೊನೆಯ_dead_ಫಲಿತಾಂಶ = a->ಫಲಿತಾಂಶ;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, ದತ್ತಾಂಶ);
    }
  }
  ಆದರೆ(bs->running)
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
  ಆದರೆ(!ongoing) {
    do_ಇನ್ನಷ್ಟು = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    bool ಇನ್ನಷ್ಟು_possible = cf_ai_iter_has_ಇನ್ನಷ್ಟು(&bs->addr_iter, ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಆದರೆ(!ಇನ್ನಷ್ಟು_possible)
      ಇನ್ನಷ್ಟು_possible = cf_ai_iter_has_ಇನ್ನಷ್ಟು(&bs->ipv6_iter, ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
    do_ಇನ್ನಷ್ಟು = ಇನ್ನಷ್ಟು_possible &&
      (curlx_ptimediff_ms(pnow, &bs->ಕೊನೆಯ_attempt_started) >=
       bs->attempt_delay_ms);
    ಆದರೆ(do_ಇನ್ನಷ್ಟು)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  ಆದರೆ(do_ಇನ್ನಷ್ಟು) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ ai_family = 0;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "want to do more");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಆದರೆ((bs->ಕೊನೆಯ_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_ಇನ್ನಷ್ಟು(&bs->addr_iter, ದತ್ತಾಂಶ)) {
      ai = cf_ai_iter_ಮುಂದಿನ(&bs->ipv6_iter, ದತ್ತಾಂಶ);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(!ai) {
      ai = cf_ai_iter_ಮುಂದಿನ(&bs->addr_iter, ದತ್ತಾಂಶ);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(ದತ್ತಾಂಶ);

    ಆದರೆ(ai) {  /* try another address */
      ರಚನೆ Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      ಆದರೆ(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, ದತ್ತಾಂಶ, bs->max_concurrent - 1);

      ಫಲಿತಾಂಶ = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;

      ಫಲಿತಾಂಶ = cf_ip_attempt_new(&a, ದತ್ತಾಂಶ, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_ಬಾಲ = a;
      a->ಮುಂದಿನ = ಖಾಲಿ;
      bs->running_ಬಾಲ = &a->ಮುಂದಿನ;
      bs->ಕೊನೆಯ_attempt_started = *pnow;
      bs->ಕೊನೆಯ_attempt_ai_family = ai_family;
      /* and run everything again */
      ಹೋಗು evaluate;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->ಕೊನೆಯ_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      ಆದರೆ(delay_ms <= 0) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        ಗಾಗಿ(a = bs->running; a; a = a->ಮುಂದಿನ) {
          VERBOSE(++i);
          ಆದರೆ(!a->inconclusive)
            ಮುಂದುವರಿ;
          ಫಲಿತಾಂಶ = cf_ip_attempt_restart(a, cf, ದತ್ತಾಂಶ);
          CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "restarted baller %d -> %d", i, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
          ಆದರೆ(ಫಲಿತಾಂಶ) /* serious failure */
            ಹೋಗು ಹೊರಹೋಗು;
          bs->ಕೊನೆಯ_attempt_started = *pnow;
          ಹೋಗು evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* let's wait some more before restarting */
        infof(ದತ್ತಾಂಶ, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      ಫಲಿತಾಂಶ = bs->had_dead ? bs->ಕೊನೆಯ_dead_ಫಲಿತಾಂಶ : CURLE_COULDNT_CONNECT;
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    bool ಇನ್ನಷ್ಟು_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
    ಮುಂದಿನ_expire_ms = Curl_timeleft_now_ms(ದತ್ತಾಂಶ, pnow);
    ಆದರೆ(ಮುಂದಿನ_expire_ms < 0) {
      failf(ದತ್ತಾಂಶ, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(ದತ್ತಾಂಶ, ಖಾಲಿ, TIMER_STARTSINGLE));
      ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
    }

    ಇನ್ನಷ್ಟು_possible = cf_ai_iter_has_ಇನ್ನಷ್ಟು(&bs->addr_iter, ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಆದರೆ(!ಇನ್ನಷ್ಟು_possible)
      ಇನ್ನಷ್ಟು_possible = cf_ai_iter_has_ಇನ್ನಷ್ಟು(&bs->ipv6_iter, ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(ಇನ್ನಷ್ಟು_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->ಕೊನೆಯ_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      ಮುಂದಿನ_expire_ms = CURLMIN(ಮುಂದಿನ_expire_ms, expire_ms);
      ಆದರೆ(ಮುಂದಿನ_expire_ms <= 0) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        ಹೋಗು evaluate;
      }
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", ಮುಂದಿನ_expire_ms);
      Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_HAPPY_EYEBALLS, ಮುಂದಿನ_expire_ms, pnow);
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_ip_ballers_shutdown(ರಚನೆ cf_ip_ballers *bs,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       bool *ಮುಗಿದ)
{
  ರಚನೆ cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *ಮುಗಿದ = TRUE;
  ಗಾಗಿ(a = bs->running; a; a = a->ಮುಂದಿನ) {
    bool bdone = FALSE;
    ಆದರೆ(a->shutdown || !a->cf)
      ಮುಂದುವರಿ;
    a->ಫಲಿತಾಂಶ = a->cf->cft->do_shutdown(a->cf, ದತ್ತಾಂಶ, &bdone);
    ಆದರೆ(a->ಫಲಿತಾಂಶ || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    ಇಲ್ಲದಿದ್ದರೆ
      *ಮುಗಿದ = FALSE;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_ip_ballers_pollset(ರಚನೆ cf_ip_ballers *bs,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ cf_ip_attempt *a;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಗಾಗಿ(a = bs->running; a && !ಫಲಿತಾಂಶ; a = a->ಮುಂದಿನ) {
    ಆದರೆ(a->ಫಲಿತಾಂಶ)
      ಮುಂದುವರಿ;
    ಫಲಿತಾಂಶ = Curl_conn_cf_adjust_pollset(a->cf, ದತ್ತಾಂಶ, ps);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool cf_ip_ballers_pending(ರಚನೆ cf_ip_ballers *bs,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ip_attempt *a;

  ಗಾಗಿ(a = bs->running; a; a = a->ಮುಂದಿನ) {
    ಆದರೆ(a->ಫಲಿತಾಂಶ)
      ಮುಂದುವರಿ;
    ಆದರೆ(a->cf && a->cf->cft->has_ದತ್ತಾಂಶ_pending(a->cf, ದತ್ತಾಂಶ))
      ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cf_ip_ballers_min_ಉತ್ತರ_ms(ರಚನೆ cf_ip_ballers *bs,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಪೂರ್ಣಾಂಕ ಉತ್ತರ_ms = -1, breply_ms;
  ರಚನೆ cf_ip_attempt *a;

  ಗಾಗಿ(a = bs->running; a; a = a->ಮುಂದಿನ) {
    ಆದರೆ(a->cf && !a->cf->cft->query(a->cf, ದತ್ತಾಂಶ, CF_QUERY_CONNECT_ಉತ್ತರ_MS,
                                   &breply_ms, ಖಾಲಿ) &&
       breply_ms >= 0 && (ಉತ್ತರ_ms < 0 || breply_ms < ಉತ್ತರ_ms))
      ಉತ್ತರ_ms = breply_ms;
  }
  ಹಿಂತಿರುಗಿ ಉತ್ತರ_ms;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  SCFST_INIT,
  SCFST_ಕಾಯುವಿಕೆ,
  SCFST_ಮುಗಿದ
} cf_connect_state;

ರಚನೆ cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  ರಚನೆ cf_ip_ballers ballers;
  ಬಿಟ್(dns_resolved);
};

ಸ್ಥಿರ CURLcode is_connected(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             bool *connected)
{
  ರಚನೆ cf_ip_happy_ctx *ctx = cf->ctx;
  ರಚನೆ connectdata *conn = cf->conn;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = cf_ip_ballers_ಸತತ(&ctx->ballers, cf, ದತ್ತಾಂಶ,
                             (bool)ctx->dns_resolved, connected);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಇಲ್ಲದಿದ್ದರೆ {
    ರಚನೆ Curl_peer *peer = ಖಾಲಿ, *proxy_peer = ಖಾಲಿ;
    ಅಕ್ಷರ viamsg[160];

    peer = Curl_conn_get_ಮೊದಲ_peer(conn, cf->sockindex);
    ಆದರೆ(!conn->origin || !peer)
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    ಆದರೆ(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#ಆದರೆ_ಅಂತ್ಯ

    viamsg[0] = 0;
    ಆದರೆ(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
      ಆದರೆ(peer->unix_socket)
        curl_msnprintf(viamsg, ಗಾತ್ರ(viamsg), " over unix://%s",
                       peer->hostname);
      ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
      curl_msnprintf(viamsg, ಗಾತ್ರ(viamsg), " via %s:%u",
                     peer->hostname, peer->ಪೋರ್ಟ್);
    }

    failf(ದತ್ತಾಂಶ, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->ಪೋರ್ಟ್, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(ದತ್ತಾಂಶ, ಖಾಲಿ, TIMER_STARTSINGLE),
          curl_easy_strerror(ಫಲಿತಾಂಶ));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SOCKETIMEDOUT
    ಆದರೆ(SOCKETIMEDOUT == ದತ್ತಾಂಶ->state.os_errno)
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
}

#ವ್ಯಾಖ್ಯಾನಿಸು IP_HE_MAX_CONCURRENT_ATTEMPTS     6

ಸ್ಥಿರ CURLcode cf_ip_happy_init(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ip_happy_ctx *ctx = cf->ctx;

  ಆದರೆ(Curl_timeleft_ms(ದತ್ತಾಂಶ) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(ದತ್ತಾಂಶ, "Connection time-out");
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
  }

  ಆದರೆ(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ { /* TCP/UDP/QUIC */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#ಆದರೆ_ಅಂತ್ಯ
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_happy_ctx_clear(ರಚನೆ cf_ip_happy_ctx *ctx,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  DEBUGASSERT(ctx);
  ಆದರೆ(ctx)
    cf_ip_ballers_clear(ದತ್ತಾಂಶ, &ctx->ballers);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_happy_ctx_destroy(ರಚನೆ cf_ip_happy_ctx *ctx,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ctx) {
    cf_ip_happy_ctx_clear(ctx, ದತ್ತಾಂಶ);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_ip_happy_shutdown(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     bool *ಮುಗಿದ)
{
  ರಚನೆ cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಫಲಿತಾಂಶ = cf_ip_ballers_shutdown(&ctx->ballers, ದತ್ತಾಂಶ, ಮುಗಿದ);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown -> %d, done=%d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_ip_happy_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!cf->connected) {
    ಫಲಿತಾಂಶ = cf_ip_ballers_pollset(&ctx->ballers, ದತ್ತಾಂಶ, ps);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust_pollset -> %d, %u socks", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
                ps->n);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_ip_happy_connect(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    bool *ಮುಗಿದ)
{
  ರಚನೆ cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *ಮುಗಿದ = FALSE;

  ಆದರೆ(!ctx->dns_resolved) {
    ಫಲಿತಾಂಶ = Curl_conn_dns_addr_ಫಲಿತಾಂಶ(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ctx->dns_resolved = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      ಫಲಿತಾಂಶ = CURLE_OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ /* real error */
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆಯ್ಕೆ(ctx->state) {
  ಸ್ಥಿತಿ SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ));
    DEBUGASSERT(!cf->connected);
    ಫಲಿತಾಂಶ = cf_ip_happy_init(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ctx->state = SCFST_ಕಾಯುವಿಕೆ;
    FALLTHROUGH();
  ಸ್ಥಿತಿ SCFST_ಕಾಯುವಿಕೆ:
    ಫಲಿತಾಂಶ = is_connected(cf, ದತ್ತಾಂಶ, ಮುಗಿದ);
    ಆದರೆ(!ಫಲಿತಾಂಶ && *ಮುಗಿದ) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_ಮುಗಿದ;
      cf->connected = TRUE;
      cf->ಮುಂದಿನ = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = ಖಾಲಿ;

      ಆದರೆ(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_APPCONNECT); /* we are connected already */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
      ಆದರೆ(Curl_trc_cf_is_verbose(cf, ದತ್ತಾಂಶ)) {
        ರಚನೆ ip_quadruple ipquad;
        bool is_ipv6;
        ಆದರೆ(!Curl_conn_cf_get_ip_info(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_ಪೋರ್ಟ್);
        }
      }
#ಆದರೆ_ಅಂತ್ಯ
      cf_ip_happy_ctx_clear(ctx, ದತ್ತಾಂಶ);
      Curl_expire_clear(ದತ್ತಾಂಶ, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(ದತ್ತಾಂಶ);
      ದತ್ತಾಂಶ->info.numconnects++; /* to track the # of connections made */
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ SCFST_ಮುಗಿದ:
    *ಮುಗಿದ = TRUE;
    ನಿಲ್ಲಿಸು;
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool cf_ip_happy_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                                     ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ip_happy_ctx *ctx = cf->ctx;

  ಆದರೆ(!cf->connected) {
    ಹಿಂತಿರುಗಿ cf_ip_ballers_pending(&ctx->ballers, ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ->cft->has_ದತ್ತಾಂಶ_pending(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ CURLcode cf_ip_happy_query(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ cf_ip_happy_ctx *ctx = cf->ctx;

  ಆದರೆ(!cf->connected) {
    ಆಯ್ಕೆ(query) {
    ಸ್ಥಿತಿ CF_QUERY_CONNECT_ಉತ್ತರ_MS: {
      *pres1 = cf_ip_ballers_min_ಉತ್ತರ_ms(&ctx->ballers, ದತ್ತಾಂಶ);
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "query connect reply: %dms", *pres1);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
  }

  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ip_happy_destroy(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "destroy");
  ಆದರೆ(ctx) {
    cf_ip_happy_ctx_clear(ctx, ದತ್ತಾಂಶ);
    cf_ip_happy_ctx_destroy(ctx, ದತ್ತಾಂಶ);
  }
}

ರಚನೆ Curl_cftype Curl_cft_ip_happy = {
  "HAPPY-EYEBALLS",
  CF_TYPE_ಸಿದ್ಧತೆ,
  CURL_LOG_LVL_NONE,
  cf_ip_happy_destroy,
  cf_ip_happy_connect,
  cf_ip_happy_shutdown,
  cf_ip_happy_adjust_pollset,
  cf_ip_happy_ದತ್ತಾಂಶ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

ಸ್ಥಿರ CURLcode cf_ip_happy_create(ರಚನೆ Curl_cfilter **pcf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ Curl_peer *origin,
                                   ರಚನೆ Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ರಚನೆ connectdata *conn,
                                   ರಚನೆ Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  ರಚನೆ cf_ip_happy_ctx *ctx = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)conn;
  *pcf = ಖಾಲಿ;
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = cf_ip_ballers_init(&ctx->ballers, ದತ್ತಾಂಶ,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              ದತ್ತಾಂಶ->ಗಣ.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_ip_happy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ Curl_peer *origin,
                                       ರಚನೆ Curl_peer *peer,
                                       uint8_t transport_peer,
                                       ರಚನೆ Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  ಫಲಿತಾಂಶ = cf_ip_happy_create(&cf, ದತ್ತಾಂಶ, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
