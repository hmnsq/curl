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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_LINUX_TCP_H
#அந்தர்பாவ <linux/tcp.h>
#அல்லதெனில் defined(HAVE_NETINET_TCP_H)
#அந்தர்பாவ <netinet/tcp.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif

#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-ip-happy.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "sockaddr.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "vquic/vquic.h" /* for quic cfilters */


கட்டமைப்பு transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

நிலையான
#ifndef UNITTESTS
மாறிலி
#endif
கட்டமைப்பு transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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

நிலையான cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  க்கு(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    எனில்((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      திரும்பு transport_providers[i].cf_create;
  }
  திரும்பு வெற்று;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST வெறுமை debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST வெறுமை debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  க்கு(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    எனில்(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

கட்டமைப்பு cf_ai_iter {
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு Curl_peer *peer;
  முழு ai_family;
  unsigned முழு n;
};

நிலையான வெறுமை cf_ai_iter_init(கட்டமைப்பு cf_ai_iter *iter,
                            கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_peer *peer,
                            முழு ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

நிலையான மாறிலி கட்டமைப்பு Curl_addrinfo *cf_ai_iter_next(கட்டமைப்பு cf_ai_iter *iter,
                                                   கட்டமைப்பு Curl_easy *data)
{
  மாறிலி கட்டமைப்பு Curl_addrinfo *addr;

  எனில்(!iter->cf)
    திரும்பு வெற்று;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  எனில்(addr)
    iter->n++;
  திரும்பு addr;
}

நிலையான bool cf_ai_iter_has_more(கட்டமைப்பு cf_ai_iter *iter,
                                கட்டமைப்பு Curl_easy *data)
{
  திரும்பு (iter->cf &&
          !!Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

கட்டமைப்பு cf_ip_attempt {
  கட்டமைப்பு cf_ip_attempt *next;
  கட்டமைப்பு Curl_peer *origin;
  கட்டமைப்பு Curl_peer *peer;
  கட்டமைப்பு Curl_peer *tunnel_peer;
  கட்டமைப்பு Curl_sockaddr_ex addr;
  கட்டமைப்பு Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode result;
  முழு ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  முழு error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

நிலையான வெறுமை cf_ip_attempt_free(கட்டமைப்பு cf_ip_attempt *a,
                               கட்டமைப்பு Curl_easy *data)
{
  எனில்(a) {
    எனில்(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, data);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

நிலையான CURLcode cf_ip_attempt_new(கட்டமைப்பு cf_ip_attempt **pa,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_peer *origin,
                                  கட்டமைப்பு Curl_peer *peer,
                                  uint8_t transport_peer,
                                  கட்டமைப்பு Curl_sockaddr_ex *addr,
                                  முழு ai_family,
                                  கட்டமைப்பு Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  கட்டமைப்பு Curl_cfilter *wcf;
  கட்டமைப்பு cf_ip_attempt *a;
  CURLcode result = CURLE_OK;

  *pa = வெற்று;
  a = curlx_calloc(1, அளவு(*a));
  எனில்(!a)
    திரும்பு CURLE_OUT_OF_MEMORY;

  Curl_peer_link(&a->origin, origin);
  Curl_peer_link(&a->peer, peer);
  a->transport_peer = transport_peer;
  Curl_peer_link(&a->tunnel_peer, tunnel_peer);
  a->tunnel_transport = tunnel_transport;
  a->addr = *addr;
  a->ai_family = ai_family;
  a->result = CURLE_OK;
  a->cf_create = cf_create;
  *pa = a;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr, a->tunnel_peer,
                        a->tunnel_transport);
  எனில்(result)
    செல் out;

  /* the new filter might have sub-filters */
  க்கு(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  எனில்(result) {
    cf_ip_attempt_free(a, data);
    *pa = வெற்று;
  }
  திரும்பு result;
}

நிலையான CURLcode cf_ip_attempt_connect(கட்டமைப்பு cf_ip_attempt *a,
                                      கட்டமைப்பு Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  எனில்(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    எனில்(!a->result) {
      எனில்(*connected) {
        a->connected = TRUE;
      }
    }
    இல்லையெனில் {
      எனில்(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      எனில்(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  திரும்பு a->result;
}

கட்டமைப்பு cf_ip_ballers {
  கட்டமைப்பு cf_ip_attempt *running;
  கட்டமைப்பு cf_ip_attempt **running_tail;  /* &running or &(last->next) */
  கட்டமைப்பு cf_ip_attempt *winner;
  கட்டமைப்பு cf_ai_iter addr_iter;
#ifdef USE_IPV6
  கட்டமைப்பு cf_ai_iter ipv6_iter;
#endif
  கட்டமைப்பு Curl_peer *origin;
  கட்டமைப்பு Curl_peer *peer;
  கட்டமைப்பு Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  கட்டமைப்பு curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_result;  /* result of last discarded hard failure */
  முழு last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

நிலையான CURLcode cf_ip_attempt_restart(கட்டமைப்பு cf_ip_attempt *a,
                                      கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cfilter *wcf;
  CURLcode result;

  எனில்(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = வெற்று;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  எனில்(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    க்கு(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  திரும்பு result;
}

நிலையான வெறுமை cf_ip_ballers_clear(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு cf_ip_ballers *bs)
{
  போது(bs->running) {
    கட்டமைப்பு cf_ip_attempt *a = bs->running;
    bs->running = a->next;
    cf_ip_attempt_free(a, data);
  }
  bs->running_tail = &bs->running;
  cf_ip_attempt_free(bs->winner, data);
  bs->winner = வெற்று;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

நிலையான CURLcode cf_ip_ballers_init(கட்டமைப்பு cf_ip_ballers *bs,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு Curl_peer *origin,
                                   கட்டமைப்பு Curl_peer *peer,
                                   uint8_t transport_peer,
                                   கட்டமைப்பு Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, அளவு(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  எனில்(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cf_ip_ballers_prune(கட்டமைப்பு cf_ip_ballers *bs,
                                கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                uint32_t max_concurrent)
{
  கட்டமைப்பு cf_ip_attempt *a = வெற்று, **panchor;
  uint32_t ongoing = 0;

  க்கு(a = bs->running; a; a = a->next) {
    எனில்(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  போது(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    எனில்(!a->result && !a->connected) {
      *panchor = a->next;
      எனில்(!a->next)
        bs->running_tail = panchor;
      a->next = வெற்று;
      cf_ip_attempt_free(a, data);
      --ongoing;
      CURL_TRC_CF(data, cf, "discarding oldest attempt to keep limit");
    }
    இல்லையெனில் {
      panchor = &a->next;
    }
  }
}

நிலையான CURLcode cf_ip_ballers_run(கட்டமைப்பு cf_ip_ballers *bs,
                                  கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு cf_ip_attempt *a = வெற்று, **panchor;
  bool do_more;
  timediff_t next_expire_ms;
  uint32_t inconclusive, ongoing;
  மாறிலி கட்டமைப்பு curltime *pnow = வெற்று;
  VERBOSE(முழு i);

  எனில்(bs->winner)
    திரும்பு CURLE_OK;

evaluate:
  எனில்(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, வெற்று, TIMER_STARTSINGLE));
    திரும்பு CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  க்கு(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    எனில்(!a->result) {
      எனில்(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = வெற்று;
        போது(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        திரும்பு CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    இல்லையெனில் எனில்(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    இல்லையெனில் {
      /* hard failure, discard right away */
      *panchor = a->next;
      எனில்(!a->next)
        bs->running_tail = panchor;
      a->next = வெற்று;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  எனில்(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  எனில்(!ongoing) {
    do_more = TRUE;
  }
  இல்லையெனில் {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    எனில்(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    எனில்(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  எனில்(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    மாறிலி கட்டமைப்பு Curl_addrinfo *ai = வெற்று;
    முழு ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    எனில்((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    எனில்(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    எனில்(ai) {  /* try another address */
      கட்டமைப்பு Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      எனில்(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      எனில்(result)
        செல் out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (முழு)result);
      எனில்(result)
        செல் out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_tail = a;
      a->next = வெற்று;
      bs->running_tail = &a->next;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      செல் evaluate;
    }
    இல்லையெனில் எனில்(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      எனில்(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        க்கு(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          எனில்(!a->inconclusive)
            தொடர்;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (முழு)result);
          எனில்(result) /* serious failure */
            செல் out;
          bs->last_attempt_started = *pnow;
          செல் evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      இல்லையெனில் {
        /* let's wait some more before restarting */
        infof(data, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      செல் out;
    }
    இல்லையெனில் எனில்(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  எனில்(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    எனில்(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, வெற்று, TIMER_STARTSINGLE));
      திரும்பு CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    எனில்(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    எனில்(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      எனில்(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        செல் evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  திரும்பு result;
}

நிலையான CURLcode cf_ip_ballers_shutdown(கட்டமைப்பு cf_ip_ballers *bs,
                                       கட்டமைப்பு Curl_easy *data,
                                       bool *done)
{
  கட்டமைப்பு cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  க்கு(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    எனில்(a->shutdown || !a->cf)
      தொடர்;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    எனில்(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    இல்லையெனில்
      *done = FALSE;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_ip_ballers_pollset(கட்டமைப்பு cf_ip_ballers *bs,
                                      கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  க்கு(a = bs->running; a && !result; a = a->next) {
    எனில்(a->result)
      தொடர்;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  திரும்பு result;
}

நிலையான bool cf_ip_ballers_pending(கட்டமைப்பு cf_ip_ballers *bs,
                                  மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_ip_attempt *a;

  க்கு(a = bs->running; a; a = a->next) {
    எனில்(a->result)
      தொடர்;
    எனில்(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

நிலையான முழு cf_ip_ballers_min_reply_ms(கட்டமைப்பு cf_ip_ballers *bs,
                                      கட்டமைப்பு Curl_easy *data)
{
  முழு reply_ms = -1, breply_ms;
  கட்டமைப்பு cf_ip_attempt *a;

  க்கு(a = bs->running; a; a = a->next) {
    எனில்(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, வெற்று) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  திரும்பு reply_ms;
}

வகைவரை எண்ணிக்கை {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

கட்டமைப்பு cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  கட்டமைப்பு cf_ip_ballers ballers;
  BIT(dns_resolved);
};

நிலையான CURLcode is_connected(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             bool *connected)
{
  கட்டமைப்பு cf_ip_happy_ctx *ctx = cf->ctx;
  கட்டமைப்பு connectdata *conn = cf->conn;
  CURLcode result;

  result = cf_ip_ballers_run(&ctx->ballers, cf, data,
                             (bool)ctx->dns_resolved, connected);

  எனில்(!result)
    திரும்பு CURLE_OK;
  இல்லையெனில் {
    கட்டமைப்பு Curl_peer *peer = வெற்று, *proxy_peer = வெற்று;
    எழுத்து viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    எனில்(!conn->origin || !peer)
      திரும்பு CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    எனில்(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    இல்லையெனில் எனில்(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    எனில்(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      எனில்(peer->unix_socket)
        curl_msnprintf(viamsg, அளவு(viamsg), " over unix://%s",
                       peer->hostname);
      இல்லையெனில்
#endif
      curl_msnprintf(viamsg, அளவு(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(data, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(data, வெற்று, TIMER_STARTSINGLE),
          curl_easy_strerror(result));

#ifdef SOCKETIMEDOUT
    எனில்(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    திரும்பு result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

நிலையான CURLcode cf_ip_happy_init(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_ip_happy_ctx *ctx = cf->ctx;

  எனில்(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    திரும்பு CURLE_OPERATION_TIMEDOUT;
  }

  எனில்(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#இல்லையெனில்
    திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  இல்லையெனில் { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(data, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cf_ip_happy_ctx_clear(கட்டமைப்பு cf_ip_happy_ctx *ctx,
                                  கட்டமைப்பு Curl_easy *data)
{
  DEBUGASSERT(ctx);
  எனில்(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

நிலையான வெறுமை cf_ip_happy_ctx_destroy(கட்டமைப்பு cf_ip_happy_ctx *ctx,
                                    கட்டமைப்பு Curl_easy *data)
{
  எனில்(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    curlx_free(ctx);
  }
}

நிலையான CURLcode cf_ip_happy_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data,
                                     bool *done)
{
  கட்டமைப்பு cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (முழு)result, *done);
  திரும்பு result;
}

நிலையான CURLcode cf_ip_happy_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                           கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  எனில்(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (முழு)result,
                ps->n);
  }
  திரும்பு result;
}

நிலையான CURLcode cf_ip_happy_connect(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    bool *done)
{
  கட்டமைப்பு cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  எனில்(!data)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  எனில்(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    எனில்(!result)
      ctx->dns_resolved = TRUE;
    இல்லையெனில் எனில்(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    இல்லையெனில் /* real error */
      செல் out;
  }

  தேர்வு(ctx->state) {
  நிலை SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    எனில்(result)
      செல் out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  நிலை SCFST_WAITING:
    result = is_connected(cf, data, done);
    எனில்(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = வெற்று;

      எனில்(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      எனில்(Curl_trc_cf_is_verbose(cf, data)) {
        கட்டமைப்பு ip_quadruple ipquad;
        bool is_ipv6;
        எனில்(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
          CURL_TRC_CF(data, cf, "Connected to %s (%s) port %u",
                      ctx->ballers.peer->hostname,
                      ipquad.remote_ip, ipquad.remote_port);
        }
      }
#endif
      cf_ip_happy_ctx_clear(ctx, data);
      Curl_expire_clear(data, EXPIRE_HAPPY_EYEBALLS);
      /* whatever errors were reported by ballers, clear our errorbuf */
      Curl_reset_fail(data);
      data->info.numconnects++; /* to track the # of connections made */
    }
    நிறுத்து;
  நிலை SCFST_DONE:
    *done = TRUE;
    நிறுத்து;
  }
out:
  திரும்பு result;
}

நிலையான bool cf_ip_happy_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                                     மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_ip_happy_ctx *ctx = cf->ctx;

  எனில்(!cf->connected) {
    திரும்பு cf_ip_ballers_pending(&ctx->ballers, data);
  }
  திரும்பு cf->next->cft->has_data_pending(cf->next, data);
}

நிலையான CURLcode cf_ip_happy_query(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு cf_ip_happy_ctx *ctx = cf->ctx;

  எனில்(!cf->connected) {
    தேர்வு(query) {
    நிலை CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      திரும்பு CURLE_OK;
    }
    இயல்பு:
      நிறுத்து;
    }
  }

  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

நிலையான வெறுமை cf_ip_happy_destroy(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  எனில்(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
}

கட்டமைப்பு Curl_cftype Curl_cft_ip_happy = {
  "HAPPY-EYEBALLS",
  CF_TYPE_SETUP,
  CURL_LOG_LVL_NONE,
  cf_ip_happy_destroy,
  cf_ip_happy_connect,
  cf_ip_happy_shutdown,
  cf_ip_happy_adjust_pollset,
  cf_ip_happy_data_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ip_happy_query,
};

நிலையான CURLcode cf_ip_happy_create(கட்டமைப்பு Curl_cfilter **pcf,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு Curl_peer *origin,
                                   கட்டமைப்பு Curl_peer *peer,
                                   uint8_t transport_peer,
                                   கட்டமைப்பு connectdata *conn,
                                   கட்டமைப்பு Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  கட்டமைப்பு cf_ip_happy_ctx *ctx = வெற்று;
  CURLcode result;

  (வெறுமை)data;
  (வெறுமை)conn;
  *pcf = வெற்று;
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  எனில்(result)
    செல் out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  எனில்(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  திரும்பு result;
}

CURLcode Curl_cf_ip_happy_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                       கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு Curl_peer *origin,
                                       கட்டமைப்பு Curl_peer *peer,
                                       uint8_t transport_peer,
                                       கட்டமைப்பு Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  result = cf_ip_happy_create(&cf, data, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  எனில்(result)
    திரும்பு result;

  Curl_conn_cf_செருகு_after(cf_at, cf);
  திரும்பு CURLE_OK;
}
