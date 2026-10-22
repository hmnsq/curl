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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h> /* <netinet/tcp.h> may need it */
#पूर्ण_अगर
#अगर_घोषित HAVE_LINUX_TCP_H
#समावेश <linux/tcp.h>
#या_अगर defined(HAVE_NETINET_TCP_H)
#समावेश <netinet/tcp.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर

#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "connect.h"
#समावेश "cfilters.h"
#समावेश "cf-ip-happy.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "multiif.h"
#समावेश "progress.h"
#समावेश "select.h"
#समावेश "sockaddr.h"
#समावेश "vdns/cf-dns.h"
#समावेश "vquic/vquic.h" /* for quic cfilters */


काष्ठा transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

अटल
#अगर_अघोषित UNITTESTS
स्थिर
#पूर्ण_अगर
काष्ठा transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
  { Curl_cf_h3_proxy_create, TRNSPRT_QUIC, TRUE },
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_TFTP
  { Curl_cf_udp_create, TRNSPRT_UDP, FALSE },
#पूर्ण_अगर
#अगर_घोषित USE_UNIX_SOCKETS
  { Curl_cf_unix_create, TRNSPRT_UNIX, FALSE },
  { Curl_cf_unix_create, TRNSPRT_UNIX, TRUE },
#पूर्ण_अगर
};

अटल cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  माप_प्रकार i;
  क्रम(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    अगर((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      वापस transport_providers[i].cf_create;
  }
  वापस शून्य;
}

#अगर_घोषित UNITTESTS
/* @unittest 2600 */
UNITTEST व्योम debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST व्योम debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  माप_प्रकार i;
  क्रम(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    अगर(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#पूर्ण_अगर /* UNITTESTS */

काष्ठा cf_ai_iter {
  काष्ठा Curl_cfilter *cf;
  काष्ठा Curl_peer *peer;
  पूर्णांक ai_family;
  अचिन्हित पूर्णांक n;
};

अटल व्योम cf_ai_iter_init(काष्ठा cf_ai_iter *iter,
                            काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_peer *peer,
                            पूर्णांक ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

अटल स्थिर काष्ठा Curl_addrinfo *cf_ai_iter_अग्र(काष्ठा cf_ai_iter *iter,
                                                   काष्ठा Curl_easy *तथ्य)
{
  स्थिर काष्ठा Curl_addrinfo *addr;

  अगर(!iter->cf)
    वापस शून्य;

  addr = Curl_conn_dns_get_ai(तथ्य, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  अगर(addr)
    iter->n++;
  वापस addr;
}

अटल bool cf_ai_iter_has_more(काष्ठा cf_ai_iter *iter,
                                काष्ठा Curl_easy *तथ्य)
{
  वापस (iter->cf &&
          !!Curl_conn_dns_get_ai(तथ्य, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

काष्ठा cf_ip_attempt {
  काष्ठा cf_ip_attempt *अग्र;
  काष्ठा Curl_peer *origin;
  काष्ठा Curl_peer *peer;
  काष्ठा Curl_peer *tunnel_peer;
  काष्ठा Curl_sockaddr_ex addr;
  काष्ठा Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode परिणाम;
  पूर्णांक ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  पूर्णांक error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

अटल व्योम cf_ip_attempt_free(काष्ठा cf_ip_attempt *a,
                               काष्ठा Curl_easy *तथ्य)
{
  अगर(a) {
    अगर(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, तथ्य);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

अटल CURLcode cf_ip_attempt_new(काष्ठा cf_ip_attempt **pa,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_peer *origin,
                                  काष्ठा Curl_peer *peer,
                                  uint8_t transport_peer,
                                  काष्ठा Curl_sockaddr_ex *addr,
                                  पूर्णांक ai_family,
                                  काष्ठा Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  काष्ठा Curl_cfilter *wcf;
  काष्ठा cf_ip_attempt *a;
  CURLcode परिणाम = CURLE_OK;

  *pa = शून्य;
  a = curlx_calloc(1, माप(*a));
  अगर(!a)
    वापस CURLE_OUT_OF_MEMORY;

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

  परिणाम = a->cf_create(&a->cf, तथ्य, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  अगर(परिणाम)
    जाओ out;

  /* the new filter might have sub-filters */
  क्रम(wcf = a->cf; wcf; wcf = wcf->अग्र) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  अगर(परिणाम) {
    cf_ip_attempt_free(a, तथ्य);
    *pa = शून्य;
  }
  वापस परिणाम;
}

अटल CURLcode cf_ip_attempt_connect(काष्ठा cf_ip_attempt *a,
                                      काष्ठा Curl_easy *तथ्य,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  अगर(!a->परिणाम && !*connected) {
    /* evaluate again */
    a->परिणाम = Curl_conn_cf_connect(a->cf, तथ्य, connected);

    अगर(!a->परिणाम) {
      अगर(*connected) {
        a->connected = TRUE;
      }
    }
    अन्यथा {
      अगर(a->परिणाम == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      अगर(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, तथ्य);
    }
  }
  वापस a->परिणाम;
}

काष्ठा cf_ip_ballers {
  काष्ठा cf_ip_attempt *running;
  काष्ठा cf_ip_attempt **running_पुच्छ;  /* &running or &(last->next) */
  काष्ठा cf_ip_attempt *winner;
  काष्ठा cf_ai_iter addr_iter;
#अगर_घोषित USE_IPV6
  काष्ठा cf_ai_iter ipv6_iter;
#पूर्ण_अगर
  काष्ठा Curl_peer *origin;
  काष्ठा Curl_peer *peer;
  काष्ठा Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  काष्ठा curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_परिणाम;  /* result of last discarded hard failure */
  पूर्णांक last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

अटल CURLcode cf_ip_attempt_restart(काष्ठा cf_ip_attempt *a,
                                      काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cfilter *wcf;
  CURLcode परिणाम;

  अगर(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, तथ्य);

  a->परिणाम = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = शून्य;

  परिणाम = a->cf_create(&a->cf, तथ्य, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  अगर(!परिणाम) {
    bool dummy;
    /* the new filter might have sub-filters */
    क्रम(wcf = a->cf; wcf; wcf = wcf->अग्र) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->परिणाम = cf_ip_attempt_connect(a, तथ्य, &dummy);
  }
  वापस परिणाम;
}

अटल व्योम cf_ip_ballers_clear(काष्ठा Curl_easy *तथ्य,
                                काष्ठा cf_ip_ballers *bs)
{
  जबतक(bs->running) {
    काष्ठा cf_ip_attempt *a = bs->running;
    bs->running = a->अग्र;
    cf_ip_attempt_free(a, तथ्य);
  }
  bs->running_पुच्छ = &bs->running;
  cf_ip_attempt_free(bs->winner, तथ्य);
  bs->winner = शून्य;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

अटल CURLcode cf_ip_ballers_init(काष्ठा cf_ip_ballers *bs,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा Curl_peer *origin,
                                   काष्ठा Curl_peer *peer,
                                   uint8_t transport_peer,
                                   काष्ठा Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  स_रखो(bs, 0, माप(*bs));
  bs->running_पुच्छ = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  अगर(!bs->cf_create) {
    failf(तथ्य, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    वापस CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  वापस CURLE_OK;
}

अटल व्योम cf_ip_ballers_prune(काष्ठा cf_ip_ballers *bs,
                                काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                uint32_t max_concurrent)
{
  काष्ठा cf_ip_attempt *a = शून्य, **panchor;
  uint32_t ongoing = 0;

  क्रम(a = bs->running; a; a = a->अग्र) {
    अगर(!a->परिणाम && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  जबतक(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    अगर(!a->परिणाम && !a->connected) {
      *panchor = a->अग्र;
      अगर(!a->अग्र)
        bs->running_पुच्छ = panchor;
      a->अग्र = शून्य;
      cf_ip_attempt_free(a, तथ्य);
      --ongoing;
      CURL_TRC_CF(तथ्य, cf, "discarding oldest attempt to keep limit");
    }
    अन्यथा {
      panchor = &a->अग्र;
    }
  }
}

अटल CURLcode cf_ip_ballers_run(काष्ठा cf_ip_ballers *bs,
                                  काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा cf_ip_attempt *a = शून्य, **panchor;
  bool do_more;
  timediff_t अग्र_expire_ms;
  uint32_t inconclusive, ongoing;
  स्थिर काष्ठा curltime *pnow = शून्य;
  VERBOSE(पूर्णांक i);

  अगर(bs->winner)
    वापस CURLE_OK;

evaluate:
  अगर(Curl_timeleft_now_ms(तथ्य, Curl_pgrs_now(तथ्य)) < 0) {
    failf(तथ्य, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(तथ्य, शून्य, TIMER_STARTSINGLE));
    वापस CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  क्रम(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->परिणाम = cf_ip_attempt_connect(a, तथ्य, connected);
    अगर(!a->परिणाम) {
      अगर(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(तथ्य, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->अग्र;
        a->अग्र = शून्य;
        जबतक(bs->running) {
          a = bs->running;
          bs->running = a->अग्र;
          cf_ip_attempt_free(a, तथ्य);
        }
        bs->running_पुच्छ = &bs->running;
        वापस CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->अग्र;
    }
    अन्यथा अगर(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->अग्र;
    }
    अन्यथा {
      /* hard failure, discard right away */
      *panchor = a->अग्र;
      अगर(!a->अग्र)
        bs->running_पुच्छ = panchor;
      a->अग्र = शून्य;
      bs->last_dead_परिणाम = a->परिणाम;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, तथ्य);
    }
  }
  अगर(bs->running)
    CURL_TRC_CF(तथ्य, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(तथ्य);
  अगर(!ongoing) {
    do_more = TRUE;
  }
  अन्यथा {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, तथ्य);
#अगर_घोषित USE_IPV6
    अगर(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, तथ्य);
#पूर्ण_अगर
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    अगर(do_more)
      CURL_TRC_CF(तथ्य, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  अगर(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    स्थिर काष्ठा Curl_addrinfo *ai = शून्य;
    पूर्णांक ai_family = 0;
    CURL_TRC_CF(तथ्य, cf, "want to do more");
#अगर_घोषित USE_IPV6
    अगर((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, तथ्य)) {
      ai = cf_ai_iter_अग्र(&bs->ipv6_iter, तथ्य);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(तथ्य, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#पूर्ण_अगर
    अगर(!ai) {
      ai = cf_ai_iter_अग्र(&bs->addr_iter, तथ्य);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(तथ्य, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(तथ्य);

    अगर(ai) {  /* try another address */
      काष्ठा Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      अगर(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, तथ्य, bs->max_concurrent - 1);

      परिणाम = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      अगर(परिणाम)
        जाओ out;

      परिणाम = cf_ip_attempt_new(&a, तथ्य, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(तथ्य, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (पूर्णांक)परिणाम);
      अगर(परिणाम)
        जाओ out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_पुच्छ = a;
      a->अग्र = शून्य;
      bs->running_पुच्छ = &a->अग्र;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      जाओ evaluate;
    }
    अन्यथा अगर(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      अगर(delay_ms <= 0) {
        CURL_TRC_CF(तथ्य, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        क्रम(a = bs->running; a; a = a->अग्र) {
          VERBOSE(++i);
          अगर(!a->inconclusive)
            जारी;
          परिणाम = cf_ip_attempt_restart(a, cf, तथ्य);
          CURL_TRC_CF(तथ्य, cf, "restarted baller %d -> %d", i, (पूर्णांक)परिणाम);
          अगर(परिणाम) /* serious failure */
            जाओ out;
          bs->last_attempt_started = *pnow;
          जाओ evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      अन्यथा {
        /* let's wait some more before restarting */
        infof(तथ्य, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(तथ्य, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      जाओ out;
    }
    अन्यथा अगर(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(तथ्य, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      परिणाम = bs->had_dead ? bs->last_dead_परिणाम : CURLE_COULDNT_CONNECT;
    }
  }

out:
  अगर(!परिणाम) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(तथ्य);
    अग्र_expire_ms = Curl_timeleft_now_ms(तथ्य, pnow);
    अगर(अग्र_expire_ms < 0) {
      failf(तथ्य, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(तथ्य, शून्य, TIMER_STARTSINGLE));
      वापस CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, तथ्य);
#अगर_घोषित USE_IPV6
    अगर(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, तथ्य);
#पूर्ण_अगर
    अगर(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      अग्र_expire_ms = CURLMIN(अग्र_expire_ms, expire_ms);
      अगर(अग्र_expire_ms <= 0) {
        CURL_TRC_CF(तथ्य, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        जाओ evaluate;
      }
      CURL_TRC_CF(तथ्य, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", अग्र_expire_ms);
      Curl_expire_set(तथ्य, EXPIRE_HAPPY_EYEBALLS, अग्र_expire_ms, pnow);
    }
  }
  वापस परिणाम;
}

अटल CURLcode cf_ip_ballers_shutdown(काष्ठा cf_ip_ballers *bs,
                                       काष्ठा Curl_easy *तथ्य,
                                       bool *done)
{
  काष्ठा cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  क्रम(a = bs->running; a; a = a->अग्र) {
    bool bdone = FALSE;
    अगर(a->shutdown || !a->cf)
      जारी;
    a->परिणाम = a->cf->cft->do_shutdown(a->cf, तथ्य, &bdone);
    अगर(a->परिणाम || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    अन्यथा
      *done = FALSE;
  }
  वापस CURLE_OK;
}

अटल CURLcode cf_ip_ballers_pollset(काष्ठा cf_ip_ballers *bs,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा easy_pollset *ps)
{
  काष्ठा cf_ip_attempt *a;
  CURLcode परिणाम = CURLE_OK;
  क्रम(a = bs->running; a && !परिणाम; a = a->अग्र) {
    अगर(a->परिणाम)
      जारी;
    परिणाम = Curl_conn_cf_adjust_pollset(a->cf, तथ्य, ps);
  }
  वापस परिणाम;
}

अटल bool cf_ip_ballers_pending(काष्ठा cf_ip_ballers *bs,
                                  स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_ip_attempt *a;

  क्रम(a = bs->running; a; a = a->अग्र) {
    अगर(a->परिणाम)
      जारी;
    अगर(a->cf && a->cf->cft->has_तथ्य_pending(a->cf, तथ्य))
      वापस TRUE;
  }
  वापस FALSE;
}

अटल पूर्णांक cf_ip_ballers_min_reply_ms(काष्ठा cf_ip_ballers *bs,
                                      काष्ठा Curl_easy *तथ्य)
{
  पूर्णांक reply_ms = -1, breply_ms;
  काष्ठा cf_ip_attempt *a;

  क्रम(a = bs->running; a; a = a->अग्र) {
    अगर(a->cf && !a->cf->cft->query(a->cf, तथ्य, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, शून्य) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  वापस reply_ms;
}

प्रकार क्रमागत {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

काष्ठा cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  काष्ठा cf_ip_ballers ballers;
  BIT(dns_resolved);
};

अटल CURLcode is_connected(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             bool *connected)
{
  काष्ठा cf_ip_happy_ctx *ctx = cf->ctx;
  काष्ठा connectdata *conn = cf->conn;
  CURLcode परिणाम;

  परिणाम = cf_ip_ballers_run(&ctx->ballers, cf, तथ्य,
                             (bool)ctx->dns_resolved, connected);

  अगर(!परिणाम)
    वापस CURLE_OK;
  अन्यथा {
    काष्ठा Curl_peer *peer = शून्य, *proxy_peer = शून्य;
    अक्षर viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    अगर(!conn->origin || !peer)
      वापस CURLE_FAILED_INIT;

#अगर_अघोषित CURL_DISABLE_PROXY
    अगर(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    अन्यथा अगर(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#पूर्ण_अगर

    viamsg[0] = 0;
    अगर(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#अगर_घोषित USE_UNIX_SOCKETS
      अगर(peer->unix_socket)
        curl_msnprintf(viamsg, माप(viamsg), " over unix://%s",
                       peer->hostname);
      अन्यथा
#पूर्ण_अगर
      curl_msnprintf(viamsg, माप(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(तथ्य, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(तथ्य, शून्य, TIMER_STARTSINGLE),
          curl_easy_strerror(परिणाम));

#अगर_घोषित SOCKETIMEDOUT
    अगर(SOCKETIMEDOUT == तथ्य->state.os_errno)
      परिणाम = CURLE_OPERATION_TIMEDOUT;
#पूर्ण_अगर

    वापस परिणाम;
  }
}

#घोषणा IP_HE_MAX_CONCURRENT_ATTEMPTS     6

अटल CURLcode cf_ip_happy_init(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_ip_happy_ctx *ctx = cf->ctx;

  अगर(Curl_timeleft_ms(तथ्य) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(तथ्य, "Connection time-out");
    वापस CURLE_OPERATION_TIMEDOUT;
  }

  अगर(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#अगर_घोषित USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#अन्यथा
    वापस CURLE_UNSUPPORTED_PROTOCOL;
#पूर्ण_अगर
  }
  अन्यथा { /* TCP/UDP/QUIC */
#अगर_घोषित USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#पूर्ण_अगर
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(तथ्य, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  वापस CURLE_OK;
}

अटल व्योम cf_ip_happy_ctx_clear(काष्ठा cf_ip_happy_ctx *ctx,
                                  काष्ठा Curl_easy *तथ्य)
{
  DEBUGASSERT(ctx);
  अगर(ctx)
    cf_ip_ballers_clear(तथ्य, &ctx->ballers);
}

अटल व्योम cf_ip_happy_ctx_destroy(काष्ठा cf_ip_happy_ctx *ctx,
                                    काष्ठा Curl_easy *तथ्य)
{
  अगर(ctx) {
    cf_ip_happy_ctx_clear(ctx, तथ्य);
    curlx_free(ctx);
  }
}

अटल CURLcode cf_ip_happy_shutdown(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य,
                                     bool *done)
{
  काष्ठा cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(तथ्य);
  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  परिणाम = cf_ip_ballers_shutdown(&ctx->ballers, तथ्य, done);
  CURL_TRC_CF(तथ्य, cf, "shutdown -> %d, done=%d", (पूर्णांक)परिणाम, *done);
  वापस परिणाम;
}

अटल CURLcode cf_ip_happy_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                           काष्ठा Curl_easy *तथ्य,
                                           काष्ठा easy_pollset *ps)
{
  काष्ठा cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(!cf->connected) {
    परिणाम = cf_ip_ballers_pollset(&ctx->ballers, तथ्य, ps);
    CURL_TRC_CF(तथ्य, cf, "adjust_pollset -> %d, %u socks", (पूर्णांक)परिणाम,
                ps->n);
  }
  वापस परिणाम;
}

अटल CURLcode cf_ip_happy_connect(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    bool *done)
{
  काष्ठा cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  अगर(!तथ्य)
    वापस CURLE_FAILED_INIT;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  अगर(!ctx->dns_resolved) {
    परिणाम = Curl_conn_dns_addr_परिणाम(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    अगर(!परिणाम)
      ctx->dns_resolved = TRUE;
    अन्यथा अगर(परिणाम == CURLE_AGAIN) {
      परिणाम = CURLE_OK;
    }
    अन्यथा /* real error */
      जाओ out;
  }

  चयन(ctx->state) {
  हाल SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, तथ्य));
    DEBUGASSERT(!cf->connected);
    परिणाम = cf_ip_happy_init(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  हाल SCFST_WAITING:
    परिणाम = is_connected(cf, तथ्य, done);
    अगर(!परिणाम && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->अग्र = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = शून्य;

      अगर(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(तथ्य, TIMER_APPCONNECT); /* we are connected already */
#अगर_घोषित CURLVERBOSE
      अगर(Curl_trc_cf_is_verbose(cf, तथ्य)) {
        काष्ठा ip_quadruple ipquad;
        bool is_ipv6;
        अगर(!Curl_conn_cf_get_ip_info(cf->अग्र, तथ्य, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(तथ्य, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_port);
        }
      }
#पूर्ण_अगर
      cf_ip_happy_ctx_clear(ctx, तथ्य);
      Curl_expire_clear(तथ्य, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(तथ्य);
      तथ्य->info.numconnects++; /* to track the # of connections made */
    }
    अवरोध;
  हाल SCFST_DONE:
    *done = TRUE;
    अवरोध;
  }
out:
  वापस परिणाम;
}

अटल bool cf_ip_happy_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                                     स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_ip_happy_ctx *ctx = cf->ctx;

  अगर(!cf->connected) {
    वापस cf_ip_ballers_pending(&ctx->ballers, तथ्य);
  }
  वापस cf->अग्र->cft->has_तथ्य_pending(cf->अग्र, तथ्य);
}

अटल CURLcode cf_ip_happy_query(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा cf_ip_happy_ctx *ctx = cf->ctx;

  अगर(!cf->connected) {
    चयन(query) {
    हाल CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, तथ्य);
      CURL_TRC_CF(तथ्य, cf, "query connect reply: %dms", *pres1);
      वापस CURLE_OK;
    }
    शेष:
      अवरोध;
    }
  }

  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

अटल व्योम cf_ip_happy_destroy(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(तथ्य, cf, "destroy");
  अगर(ctx) {
    cf_ip_happy_ctx_clear(ctx, तथ्य);
    cf_ip_happy_ctx_destroy(ctx, तथ्य);
  }
}

काष्ठा Curl_cftype Curl_cft_ip_happy = {
  "HAPPY-EYEBALLS",
  CF_TYPE_SETUP,
  CURL_LOG_LVL_NONE,
  cf_ip_happy_destroy,
  cf_ip_happy_connect,
  cf_ip_happy_shutdown,
  cf_ip_happy_adjust_pollset,
  cf_ip_happy_तथ्य_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

अटल CURLcode cf_ip_happy_create(काष्ठा Curl_cfilter **pcf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा Curl_peer *origin,
                                   काष्ठा Curl_peer *peer,
                                   uint8_t transport_peer,
                                   काष्ठा connectdata *conn,
                                   काष्ठा Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  काष्ठा cf_ip_happy_ctx *ctx = शून्य;
  CURLcode परिणाम;

  (व्योम)तथ्य;
  (व्योम)conn;
  *pcf = शून्य;
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  परिणाम = cf_ip_ballers_init(&ctx->ballers, तथ्य,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              तथ्य->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  अगर(परिणाम) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, तथ्य);
  }
  वापस परिणाम;
}

CURLcode Curl_cf_ip_happy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                       काष्ठा Curl_easy *तथ्य,
                                       काष्ठा Curl_peer *origin,
                                       काष्ठा Curl_peer *peer,
                                       uint8_t transport_peer,
                                       काष्ठा Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  परिणाम = cf_ip_happy_create(&cf, तथ्य, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  अगर(परिणाम)
    वापस परिणाम;

  Curl_conn_cf_जोड़ो_after(cf_at, cf);
  वापस CURLE_OK;
}
