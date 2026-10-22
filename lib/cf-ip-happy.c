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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h> /* <netinet/tcp.h> may need it */
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_LINUX_TCP_H
#अन्तर्भाव <linux/tcp.h>
#नोचेद्यदि defined(HAVE_NETINET_TCP_H)
#अन्तर्भाव <netinet/tcp.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः

#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-ip-happy.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "sockaddr.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vquic/vquic.h" /* for quic cfilters */


संरचना transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

स्थिर
#यदि_अपरिभाषितम् UNITTESTS
ध्रुव
#यदि_अन्तः
संरचना transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
  { Curl_cf_h3_proxy_create, TRNSPRT_QUIC, TRUE },
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_TFTP
  { Curl_cf_udp_create, TRNSPRT_UDP, FALSE },
#यदि_अन्तः
#यदि_परिभाषितम् USE_UNIX_SOCKETS
  { Curl_cf_unix_create, TRNSPRT_UNIX, FALSE },
  { Curl_cf_unix_create, TRNSPRT_UNIX, TRUE },
#यदि_अन्तः
};

स्थिर cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  प्रति(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    यदि((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      प्रत्यागम transport_providers[i].cf_create;
  }
  प्रत्यागम रिक्त;
}

#यदि_परिभाषितम् UNITTESTS
/* @unittest 2600 */
UNITTEST शून्य debug_समुच्चयः_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST शून्य debug_समुच्चयः_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  प्रति(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    यदि(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#यदि_अन्तः /* UNITTESTS */

संरचना cf_ai_iter {
  संरचना Curl_cfilter *cf;
  संरचना Curl_peer *peer;
  पूर्णाङ्क ai_family;
  अचिह्नित पूर्णाङ्क n;
};

स्थिर शून्य cf_ai_iter_init(संरचना cf_ai_iter *iter,
                            संरचना Curl_cfilter *cf,
                            संरचना Curl_peer *peer,
                            पूर्णाङ्क ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

स्थिर ध्रुव संरचना Curl_addrinfo *cf_ai_iter_अग्र(संरचना cf_ai_iter *iter,
                                                   संरचना Curl_easy *दत्त)
{
  ध्रुव संरचना Curl_addrinfo *addr;

  यदि(!iter->cf)
    प्रत्यागम रिक्त;

  addr = Curl_conn_dns_get_ai(दत्त, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  यदि(addr)
    iter->n++;
  प्रत्यागम addr;
}

स्थिर bool cf_ai_iter_has_अधिकम्(संरचना cf_ai_iter *iter,
                                संरचना Curl_easy *दत्त)
{
  प्रत्यागम (iter->cf &&
          !!Curl_conn_dns_get_ai(दत्त, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

संरचना cf_ip_attempt {
  संरचना cf_ip_attempt *अग्र;
  संरचना Curl_peer *origin;
  संरचना Curl_peer *peer;
  संरचना Curl_peer *tunnel_peer;
  संरचना Curl_sockaddr_ex addr;
  संरचना Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode परिणाम;
  पूर्णाङ्क ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  पूर्णाङ्क error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

स्थिर शून्य cf_ip_attempt_free(संरचना cf_ip_attempt *a,
                               संरचना Curl_easy *दत्त)
{
  यदि(a) {
    यदि(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, दत्त);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

स्थिर CURLcode cf_ip_attempt_new(संरचना cf_ip_attempt **pa,
                                  संरचना Curl_easy *दत्त,
                                  संरचना Curl_cfilter *cf,
                                  संरचना Curl_peer *origin,
                                  संरचना Curl_peer *peer,
                                  uint8_t transport_peer,
                                  संरचना Curl_sockaddr_ex *addr,
                                  पूर्णाङ्क ai_family,
                                  संरचना Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  संरचना Curl_cfilter *wcf;
  संरचना cf_ip_attempt *a;
  CURLcode परिणाम = CURLE_OK;

  *pa = रिक्त;
  a = curlx_calloc(1, परिमाण(*a));
  यदि(!a)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  Curl_peer_link(&a->origin, origin);
  Curl_peer_link(&a->peer, peer);
  a->transport_peer = transport_peer;
  Curl_peer_link(&a->tunnel_peer, tunnel_peer);
  a->tunnel_transport = tunnel_transport;
  a->addr = *addr;
  a->ai_family = ai_family;
  a->परिणाम = CURLE_OK;
  a->cf_create = cf_create;
  *pa = a;

  परिणाम = a->cf_create(&a->cf, दत्त, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* the new filter might have sub-filters */
  प्रति(wcf = a->cf; wcf; wcf = wcf->अग्र) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

बहिर्गमनम्:
  यदि(परिणाम) {
    cf_ip_attempt_free(a, दत्त);
    *pa = रिक्त;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_ip_attempt_connect(संरचना cf_ip_attempt *a,
                                      संरचना Curl_easy *दत्त,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  यदि(!a->परिणाम && !*connected) {
    /* evaluate again */
    a->परिणाम = Curl_conn_cf_connect(a->cf, दत्त, connected);

    यदि(!a->परिणाम) {
      यदि(*connected) {
        a->connected = TRUE;
      }
    }
    अन्यथा {
      यदि(a->परिणाम == CURLE_WEIRD_SERVER_प्रत्युत्तरम्)
        a->inconclusive = TRUE;
      यदि(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, दत्त);
    }
  }
  प्रत्यागम a->परिणाम;
}

संरचना cf_ip_ballers {
  संरचना cf_ip_attempt *running;
  संरचना cf_ip_attempt **running_पुच्छ;  /* &running or &(last->next) */
  संरचना cf_ip_attempt *winner;
  संरचना cf_ai_iter addr_iter;
#यदि_परिभाषितम् USE_IPV6
  संरचना cf_ai_iter ipv6_iter;
#यदि_अन्तः
  संरचना Curl_peer *origin;
  संरचना Curl_peer *peer;
  संरचना Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  संरचना curltime चरमः_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode चरमः_dead_परिणाम;  /* result of last discarded hard failure */
  पूर्णाङ्क चरमः_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

स्थिर CURLcode cf_ip_attempt_restart(संरचना cf_ip_attempt *a,
                                      संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त)
{
  संरचना Curl_cfilter *wcf;
  CURLcode परिणाम;

  यदि(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, दत्त);

  a->परिणाम = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = रिक्त;

  परिणाम = a->cf_create(&a->cf, दत्त, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  यदि(!परिणाम) {
    bool dummy;
    /* the new filter might have sub-filters */
    प्रति(wcf = a->cf; wcf; wcf = wcf->अग्र) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->परिणाम = cf_ip_attempt_connect(a, दत्त, &dummy);
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_ip_ballers_clear(संरचना Curl_easy *दत्त,
                                संरचना cf_ip_ballers *bs)
{
  यावत्(bs->running) {
    संरचना cf_ip_attempt *a = bs->running;
    bs->running = a->अग्र;
    cf_ip_attempt_free(a, दत्त);
  }
  bs->running_पुच्छ = &bs->running;
  cf_ip_attempt_free(bs->winner, दत्त);
  bs->winner = रिक्त;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

स्थिर CURLcode cf_ip_ballers_init(संरचना cf_ip_ballers *bs,
                                   संरचना Curl_easy *दत्त,
                                   संरचना Curl_peer *origin,
                                   संरचना Curl_peer *peer,
                                   uint8_t transport_peer,
                                   संरचना Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  स्मृतिं_पूरय(bs, 0, परिमाण(*bs));
  bs->running_पुच्छ = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  यदि(!bs->cf_create) {
    failf(दत्त, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->चरमः_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_ip_ballers_prune(संरचना cf_ip_ballers *bs,
                                संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                uint32_t max_concurrent)
{
  संरचना cf_ip_attempt *a = रिक्त, **panchor;
  uint32_t ongoing = 0;

  प्रति(a = bs->running; a; a = a->अग्र) {
    यदि(!a->परिणाम && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  यावत्(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    यदि(!a->परिणाम && !a->connected) {
      *panchor = a->अग्र;
      यदि(!a->अग्र)
        bs->running_पुच्छ = panchor;
      a->अग्र = रिक्त;
      cf_ip_attempt_free(a, दत्त);
      --ongoing;
      CURL_TRC_CF(दत्त, cf, "discarding oldest attempt to keep limit");
    }
    अन्यथा {
      panchor = &a->अग्र;
    }
  }
}

स्थिर CURLcode cf_ip_ballers_सततम्(संरचना cf_ip_ballers *bs,
                                  संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना cf_ip_attempt *a = रिक्त, **panchor;
  bool do_अधिकम्;
  timediff_t अग्र_expire_ms;
  uint32_t inconclusive, ongoing;
  ध्रुव संरचना curltime *pnow = रिक्त;
  VERBOSE(पूर्णाङ्क i);

  यदि(bs->winner)
    प्रत्यागम CURLE_OK;

evaluate:
  यदि(Curl_timeleft_now_ms(दत्त, Curl_pgrs_now(दत्त)) < 0) {
    failf(दत्त, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(दत्त, रिक्त, TIMER_STARTSINGLE));
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  प्रति(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->परिणाम = cf_ip_attempt_connect(a, दत्त, connected);
    यदि(!a->परिणाम) {
      यदि(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(दत्त, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->अग्र;
        a->अग्र = रिक्त;
        यावत्(bs->running) {
          a = bs->running;
          bs->running = a->अग्र;
          cf_ip_attempt_free(a, दत्त);
        }
        bs->running_पुच्छ = &bs->running;
        प्रत्यागम CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->अग्र;
    }
    अन्यथा यदि(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->अग्र;
    }
    अन्यथा {
      /* hard failure, discard right away */
      *panchor = a->अग्र;
      यदि(!a->अग्र)
        bs->running_पुच्छ = panchor;
      a->अग्र = रिक्त;
      bs->चरमः_dead_परिणाम = a->परिणाम;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, दत्त);
    }
  }
  यदि(bs->running)
    CURL_TRC_CF(दत्त, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(दत्त);
  यदि(!ongoing) {
    do_अधिकम् = TRUE;
  }
  अन्यथा {
    bool अधिकम्_possible = cf_ai_iter_has_अधिकम्(&bs->addr_iter, दत्त);
#यदि_परिभाषितम् USE_IPV6
    यदि(!अधिकम्_possible)
      अधिकम्_possible = cf_ai_iter_has_अधिकम्(&bs->ipv6_iter, दत्त);
#यदि_अन्तः
    do_अधिकम् = अधिकम्_possible &&
      (curlx_ptimediff_ms(pnow, &bs->चरमः_attempt_started) >=
       bs->attempt_delay_ms);
    यदि(do_अधिकम्)
      CURL_TRC_CF(दत्त, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  यदि(do_अधिकम्) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ध्रुव संरचना Curl_addrinfo *ai = रिक्त;
    पूर्णाङ्क ai_family = 0;
    CURL_TRC_CF(दत्त, cf, "want to do more");
#यदि_परिभाषितम् USE_IPV6
    यदि((bs->चरमः_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_अधिकम्(&bs->addr_iter, दत्त)) {
      ai = cf_ai_iter_अग्र(&bs->ipv6_iter, दत्त);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(दत्त, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#यदि_अन्तः
    यदि(!ai) {
      ai = cf_ai_iter_अग्र(&bs->addr_iter, दत्त);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(दत्त, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(दत्त);

    यदि(ai) {  /* try another address */
      संरचना Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      यदि(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, दत्त, bs->max_concurrent - 1);

      परिणाम = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;

      परिणाम = cf_ip_attempt_new(&a, दत्त, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(दत्त, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (पूर्णाङ्क)परिणाम);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_पुच्छ = a;
      a->अग्र = रिक्त;
      bs->running_पुच्छ = &a->अग्र;
      bs->चरमः_attempt_started = *pnow;
      bs->चरमः_attempt_ai_family = ai_family;
      /* and run everything again */
      गच्छ evaluate;
    }
    अन्यथा यदि(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->चरमः_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      यदि(delay_ms <= 0) {
        CURL_TRC_CF(दत्त, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        प्रति(a = bs->running; a; a = a->अग्र) {
          VERBOSE(++i);
          यदि(!a->inconclusive)
            अनुवर्त;
          परिणाम = cf_ip_attempt_restart(a, cf, दत्त);
          CURL_TRC_CF(दत्त, cf, "restarted baller %d -> %d", i, (पूर्णाङ्क)परिणाम);
          यदि(परिणाम) /* serious failure */
            गच्छ बहिर्गमनम्;
          bs->चरमः_attempt_started = *pnow;
          गच्छ evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      अन्यथा {
        /* let's wait some more before restarting */
        infof(दत्त, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_समुच्चयः(दत्त, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(दत्त, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      परिणाम = bs->had_dead ? bs->चरमः_dead_परिणाम : CURLE_COULDNT_CONNECT;
    }
  }

बहिर्गमनम्:
  यदि(!परिणाम) {
    bool अधिकम्_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(दत्त);
    अग्र_expire_ms = Curl_timeleft_now_ms(दत्त, pnow);
    यदि(अग्र_expire_ms < 0) {
      failf(दत्त, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(दत्त, रिक्त, TIMER_STARTSINGLE));
      प्रत्यागम CURLE_OPERATION_TIMEDOUT;
    }

    अधिकम्_possible = cf_ai_iter_has_अधिकम्(&bs->addr_iter, दत्त);
#यदि_परिभाषितम् USE_IPV6
    यदि(!अधिकम्_possible)
      अधिकम्_possible = cf_ai_iter_has_अधिकम्(&bs->ipv6_iter, दत्त);
#यदि_अन्तः
    यदि(अधिकम्_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->चरमः_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      अग्र_expire_ms = CURLMIN(अग्र_expire_ms, expire_ms);
      यदि(अग्र_expire_ms <= 0) {
        CURL_TRC_CF(दत्त, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        गच्छ evaluate;
      }
      CURL_TRC_CF(दत्त, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", अग्र_expire_ms);
      Curl_expire_समुच्चयः(दत्त, EXPIRE_HAPPY_EYEBALLS, अग्र_expire_ms, pnow);
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_ip_ballers_shutdown(संरचना cf_ip_ballers *bs,
                                       संरचना Curl_easy *दत्त,
                                       bool *समाप्तम्)
{
  संरचना cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *समाप्तम् = TRUE;
  प्रति(a = bs->running; a; a = a->अग्र) {
    bool bdone = FALSE;
    यदि(a->shutdown || !a->cf)
      अनुवर्त;
    a->परिणाम = a->cf->cft->do_shutdown(a->cf, दत्त, &bdone);
    यदि(a->परिणाम || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    अन्यथा
      *समाप्तम् = FALSE;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_ip_ballers_pollset(संरचना cf_ip_ballers *bs,
                                      संरचना Curl_easy *दत्त,
                                      संरचना easy_pollset *ps)
{
  संरचना cf_ip_attempt *a;
  CURLcode परिणाम = CURLE_OK;
  प्रति(a = bs->running; a && !परिणाम; a = a->अग्र) {
    यदि(a->परिणाम)
      अनुवर्त;
    परिणाम = Curl_conn_cf_adjust_pollset(a->cf, दत्त, ps);
  }
  प्रत्यागम परिणाम;
}

स्थिर bool cf_ip_ballers_pending(संरचना cf_ip_ballers *bs,
                                  ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना cf_ip_attempt *a;

  प्रति(a = bs->running; a; a = a->अग्र) {
    यदि(a->परिणाम)
      अनुवर्त;
    यदि(a->cf && a->cf->cft->has_दत्त_pending(a->cf, दत्त))
      प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

स्थिर पूर्णाङ्क cf_ip_ballers_min_प्रत्युत्तरम्_ms(संरचना cf_ip_ballers *bs,
                                      संरचना Curl_easy *दत्त)
{
  पूर्णाङ्क प्रत्युत्तरम्_ms = -1, breply_ms;
  संरचना cf_ip_attempt *a;

  प्रति(a = bs->running; a; a = a->अग्र) {
    यदि(a->cf && !a->cf->cft->query(a->cf, दत्त, CF_QUERY_CONNECT_प्रत्युत्तरम्_MS,
                                   &breply_ms, रिक्त) &&
       breply_ms >= 0 && (प्रत्युत्तरम्_ms < 0 || breply_ms < प्रत्युत्तरम्_ms))
      प्रत्युत्तरम्_ms = breply_ms;
  }
  प्रत्यागम प्रत्युत्तरम्_ms;
}

प्रकारनिर्देश गणना {
  SCFST_INIT,
  SCFST_प्रतीक्षा,
  SCFST_समाप्तम्
} cf_connect_state;

संरचना cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  संरचना cf_ip_ballers ballers;
  BIT(dns_resolved);
};

स्थिर CURLcode is_connected(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             bool *connected)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;
  संरचना connectdata *conn = cf->conn;
  CURLcode परिणाम;

  परिणाम = cf_ip_ballers_सततम्(&ctx->ballers, cf, दत्त,
                             (bool)ctx->dns_resolved, connected);

  यदि(!परिणाम)
    प्रत्यागम CURLE_OK;
  अन्यथा {
    संरचना Curl_peer *peer = रिक्त, *proxy_peer = रिक्त;
    वर्ण viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    यदि(!conn->origin || !peer)
      प्रत्यागम CURLE_FAILED_INIT;

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    यदि(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    अन्यथा यदि(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#यदि_अन्तः

    viamsg[0] = 0;
    यदि(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#यदि_परिभाषितम् USE_UNIX_SOCKETS
      यदि(peer->unix_socket)
        curl_msnprintf(viamsg, परिमाण(viamsg), " over unix://%s",
                       peer->hostname);
      अन्यथा
#यदि_अन्तः
      curl_msnprintf(viamsg, परिमाण(viamsg), " via %s:%u",
                     peer->hostname, peer->पत्तनम्);
    }

    failf(दत्त, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->पत्तनम्, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(दत्त, रिक्त, TIMER_STARTSINGLE),
          curl_easy_strerror(परिणाम));

#यदि_परिभाषितम् SOCKETIMEDOUT
    यदि(SOCKETIMEDOUT == दत्त->state.os_errno)
      परिणाम = CURLE_OPERATION_TIMEDOUT;
#यदि_अन्तः

    प्रत्यागम परिणाम;
  }
}

#परिभाषय IP_HE_MAX_CONCURRENT_ATTEMPTS     6

स्थिर CURLcode cf_ip_happy_init(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  यदि(Curl_timeleft_ms(दत्त) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(दत्त, "Connection time-out");
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;
  }

  यदि(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#यदि_परिभाषितम् USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#अन्यथा
    प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
#यदि_अन्तः
  }
  अन्यथा { /* TCP/UDP/QUIC */
#यदि_परिभाषितम् USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#यदि_अन्तः
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(दत्त, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_ip_happy_ctx_clear(संरचना cf_ip_happy_ctx *ctx,
                                  संरचना Curl_easy *दत्त)
{
  DEBUGASSERT(ctx);
  यदि(ctx)
    cf_ip_ballers_clear(दत्त, &ctx->ballers);
}

स्थिर शून्य cf_ip_happy_ctx_destroy(संरचना cf_ip_happy_ctx *ctx,
                                    संरचना Curl_easy *दत्त)
{
  यदि(ctx) {
    cf_ip_happy_ctx_clear(ctx, दत्त);
    curlx_free(ctx);
  }
}

स्थिर CURLcode cf_ip_happy_shutdown(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त,
                                     bool *समाप्तम्)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(दत्त);
  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  परिणाम = cf_ip_ballers_shutdown(&ctx->ballers, दत्त, समाप्तम्);
  CURL_TRC_CF(दत्त, cf, "shutdown -> %d, done=%d", (पूर्णाङ्क)परिणाम, *समाप्तम्);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_ip_happy_adjust_pollset(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *दत्त,
                                           संरचना easy_pollset *ps)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(!cf->connected) {
    परिणाम = cf_ip_ballers_pollset(&ctx->ballers, दत्त, ps);
    CURL_TRC_CF(दत्त, cf, "adjust_pollset -> %d, %u socks", (पूर्णाङ्क)परिणाम,
                ps->n);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_ip_happy_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    bool *समाप्तम्)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  यदि(!दत्त)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *समाप्तम् = FALSE;

  यदि(!ctx->dns_resolved) {
    परिणाम = Curl_conn_dns_addr_परिणाम(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    यदि(!परिणाम)
      ctx->dns_resolved = TRUE;
    अन्यथा यदि(परिणाम == CURLE_AGAIN) {
      परिणाम = CURLE_OK;
    }
    अन्यथा /* real error */
      गच्छ बहिर्गमनम्;
  }

  विकल्प(ctx->state) {
  पक्ष SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, दत्त));
    DEBUGASSERT(!cf->connected);
    परिणाम = cf_ip_happy_init(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    ctx->state = SCFST_प्रतीक्षा;
    FALLTHROUGH();
  पक्ष SCFST_प्रतीक्षा:
    परिणाम = is_connected(cf, दत्त, समाप्तम्);
    यदि(!परिणाम && *समाप्तम्) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_समाप्तम्;
      cf->connected = TRUE;
      cf->अग्र = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = रिक्त;

      यदि(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(दत्त, TIMER_APPCONNECT); /* we are connected already */
#यदि_परिभाषितम् CURLVERBOSE
      यदि(Curl_trc_cf_is_verbose(cf, दत्त)) {
        संरचना ip_quadruple ipquad;
        bool is_ipv6;
        यदि(!Curl_conn_cf_get_ip_info(cf->अग्र, दत्त, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(दत्त, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_पत्तनम्);
        }
      }
#यदि_अन्तः
      cf_ip_happy_ctx_clear(ctx, दत्त);
      Curl_expire_clear(दत्त, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(दत्त);
      दत्त->info.numconnects++; /* to track the # of connections made */
    }
    भङ्ग;
  पक्ष SCFST_समाप्तम्:
    *समाप्तम् = TRUE;
    भङ्ग;
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

स्थिर bool cf_ip_happy_दत्त_pending(संरचना Curl_cfilter *cf,
                                     ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  यदि(!cf->connected) {
    प्रत्यागम cf_ip_ballers_pending(&ctx->ballers, दत्त);
  }
  प्रत्यागम cf->अग्र->cft->has_दत्त_pending(cf->अग्र, दत्त);
}

स्थिर CURLcode cf_ip_happy_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  यदि(!cf->connected) {
    विकल्प(query) {
    पक्ष CF_QUERY_CONNECT_प्रत्युत्तरम्_MS: {
      *pres1 = cf_ip_ballers_min_प्रत्युत्तरम्_ms(&ctx->ballers, दत्त);
      CURL_TRC_CF(दत्त, cf, "query connect reply: %dms", *pres1);
      प्रत्यागम CURLE_OK;
    }
    स्वभाव:
      भङ्ग;
    }
  }

  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर शून्य cf_ip_happy_destroy(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(दत्त, cf, "destroy");
  यदि(ctx) {
    cf_ip_happy_ctx_clear(ctx, दत्त);
    cf_ip_happy_ctx_destroy(ctx, दत्त);
  }
}

संरचना Curl_cftype Curl_cft_ip_happy = {
  "HAPPY-EYEBALLS",
  CF_TYPE_सज्जता,
  CURL_LOG_LVL_NONE,
  cf_ip_happy_destroy,
  cf_ip_happy_connect,
  cf_ip_happy_shutdown,
  cf_ip_happy_adjust_pollset,
  cf_ip_happy_दत्त_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

स्थिर CURLcode cf_ip_happy_create(संरचना Curl_cfilter **pcf,
                                   संरचना Curl_easy *दत्त,
                                   संरचना Curl_peer *origin,
                                   संरचना Curl_peer *peer,
                                   uint8_t transport_peer,
                                   संरचना connectdata *conn,
                                   संरचना Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  संरचना cf_ip_happy_ctx *ctx = रिक्त;
  CURLcode परिणाम;

  (शून्य)दत्त;
  (शून्य)conn;
  *pcf = रिक्त;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  परिणाम = cf_ip_ballers_init(&ctx->ballers, दत्त,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              दत्त->समुच्चयः.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

बहिर्गमनम्:
  यदि(परिणाम) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, दत्त);
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_ip_happy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                       संरचना Curl_easy *दत्त,
                                       संरचना Curl_peer *origin,
                                       संरचना Curl_peer *peer,
                                       uint8_t transport_peer,
                                       संरचना Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  परिणाम = cf_ip_happy_create(&cf, दत्त, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  Curl_conn_cf_निवेशय_after(cf_at, cf);
  प्रत्यागम CURLE_OK;
}
