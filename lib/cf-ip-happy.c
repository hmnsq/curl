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

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_LINUX_TCP_H
#شامل <linux/tcp.h>
#وگرنهاگر defined(HAVE_NETINET_TCP_H)
#شامل <netinet/tcp.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif

#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

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


ساختار transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

ایستا
#ifndef UNITTESTS
ثابت
#endif
ساختار transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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

ایستا cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  برای(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    اگر((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      بازگشت transport_providers[i].cf_create;
  }
  بازگشت پوچ;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST تهی debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST تهی debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  برای(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    اگر(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

ساختار cf_ai_iter {
  ساختار Curl_cfilter *cf;
  ساختار Curl_peer *peer;
  صحیح ai_family;
  unsigned صحیح n;
};

ایستا تهی cf_ai_iter_init(ساختار cf_ai_iter *iter,
                            ساختار Curl_cfilter *cf,
                            ساختار Curl_peer *peer,
                            صحیح ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

ایستا ثابت ساختار Curl_addrinfo *cf_ai_iter_next(ساختار cf_ai_iter *iter,
                                                   ساختار Curl_easy *data)
{
  ثابت ساختار Curl_addrinfo *addr;

  اگر(!iter->cf)
    بازگشت پوچ;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  اگر(addr)
    iter->n++;
  بازگشت addr;
}

ایستا bool cf_ai_iter_has_more(ساختار cf_ai_iter *iter,
                                ساختار Curl_easy *data)
{
  بازگشت (iter->cf &&
          !!Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

ساختار cf_ip_attempt {
  ساختار cf_ip_attempt *next;
  ساختار Curl_peer *origin;
  ساختار Curl_peer *peer;
  ساختار Curl_peer *tunnel_peer;
  ساختار Curl_sockaddr_ex addr;
  ساختار Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode result;
  صحیح ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  صحیح error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

ایستا تهی cf_ip_attempt_free(ساختار cf_ip_attempt *a,
                               ساختار Curl_easy *data)
{
  اگر(a) {
    اگر(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, data);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

ایستا CURLcode cf_ip_attempt_new(ساختار cf_ip_attempt **pa,
                                  ساختار Curl_easy *data,
                                  ساختار Curl_cfilter *cf,
                                  ساختار Curl_peer *origin,
                                  ساختار Curl_peer *peer,
                                  uint8_t transport_peer,
                                  ساختار Curl_sockaddr_ex *addr,
                                  صحیح ai_family,
                                  ساختار Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  ساختار Curl_cfilter *wcf;
  ساختار cf_ip_attempt *a;
  CURLcode result = CURLE_OK;

  *pa = پوچ;
  a = curlx_calloc(1, اندازه(*a));
  اگر(!a)
    بازگشت CURLE_OUT_OF_MEMORY;

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
  اگر(result)
    برو out;

  /* the new filter might have sub-filters */
  برای(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  اگر(result) {
    cf_ip_attempt_free(a, data);
    *pa = پوچ;
  }
  بازگشت result;
}

ایستا CURLcode cf_ip_attempt_connect(ساختار cf_ip_attempt *a,
                                      ساختار Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  اگر(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    اگر(!a->result) {
      اگر(*connected) {
        a->connected = TRUE;
      }
    }
    وگرنه {
      اگر(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      اگر(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  بازگشت a->result;
}

ساختار cf_ip_ballers {
  ساختار cf_ip_attempt *running;
  ساختار cf_ip_attempt **running_tail;  /* &running or &(last->next) */
  ساختار cf_ip_attempt *winner;
  ساختار cf_ai_iter addr_iter;
#ifdef USE_IPV6
  ساختار cf_ai_iter ipv6_iter;
#endif
  ساختار Curl_peer *origin;
  ساختار Curl_peer *peer;
  ساختار Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  ساختار curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_result;  /* result of last discarded hard failure */
  صحیح last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

ایستا CURLcode cf_ip_attempt_restart(ساختار cf_ip_attempt *a,
                                      ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data)
{
  ساختار Curl_cfilter *wcf;
  CURLcode result;

  اگر(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = پوچ;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  اگر(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    برای(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  بازگشت result;
}

ایستا تهی cf_ip_ballers_clear(ساختار Curl_easy *data,
                                ساختار cf_ip_ballers *bs)
{
  وقتی(bs->running) {
    ساختار cf_ip_attempt *a = bs->running;
    bs->running = a->next;
    cf_ip_attempt_free(a, data);
  }
  bs->running_tail = &bs->running;
  cf_ip_attempt_free(bs->winner, data);
  bs->winner = پوچ;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

ایستا CURLcode cf_ip_ballers_init(ساختار cf_ip_ballers *bs,
                                   ساختار Curl_easy *data,
                                   ساختار Curl_peer *origin,
                                   ساختار Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ساختار Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, اندازه(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  اگر(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    بازگشت CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  بازگشت CURLE_OK;
}

ایستا تهی cf_ip_ballers_prune(ساختار cf_ip_ballers *bs,
                                ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                uint32_t max_concurrent)
{
  ساختار cf_ip_attempt *a = پوچ, **panchor;
  uint32_t ongoing = 0;

  برای(a = bs->running; a; a = a->next) {
    اگر(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  وقتی(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    اگر(!a->result && !a->connected) {
      *panchor = a->next;
      اگر(!a->next)
        bs->running_tail = panchor;
      a->next = پوچ;
      cf_ip_attempt_free(a, data);
      --ongoing;
      CURL_TRC_CF(data, cf, "discarding oldest attempt to keep limit");
    }
    وگرنه {
      panchor = &a->next;
    }
  }
}

ایستا CURLcode cf_ip_ballers_run(ساختار cf_ip_ballers *bs,
                                  ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode result = CURLE_OK;
  ساختار cf_ip_attempt *a = پوچ, **panchor;
  bool do_more;
  timediff_t next_expire_ms;
  uint32_t inconclusive, ongoing;
  ثابت ساختار curltime *pnow = پوچ;
  VERBOSE(صحیح i);

  اگر(bs->winner)
    بازگشت CURLE_OK;

evaluate:
  اگر(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, پوچ, TIMER_STARTSINGLE));
    بازگشت CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  برای(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    اگر(!a->result) {
      اگر(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = پوچ;
        وقتی(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        بازگشت CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    وگرنه اگر(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    وگرنه {
      /* hard failure, discard right away */
      *panchor = a->next;
      اگر(!a->next)
        bs->running_tail = panchor;
      a->next = پوچ;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  اگر(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  اگر(!ongoing) {
    do_more = TRUE;
  }
  وگرنه {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    اگر(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    اگر(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  اگر(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ثابت ساختار Curl_addrinfo *ai = پوچ;
    صحیح ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    اگر((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    اگر(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    اگر(ai) {  /* try another address */
      ساختار Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      اگر(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      اگر(result)
        برو out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (صحیح)result);
      اگر(result)
        برو out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_tail = a;
      a->next = پوچ;
      bs->running_tail = &a->next;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      برو evaluate;
    }
    وگرنه اگر(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      اگر(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        برای(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          اگر(!a->inconclusive)
            ادامه;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (صحیح)result);
          اگر(result) /* serious failure */
            برو out;
          bs->last_attempt_started = *pnow;
          برو evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      وگرنه {
        /* let's wait some more before restarting */
        infof(data, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      برو out;
    }
    وگرنه اگر(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  اگر(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    اگر(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, پوچ, TIMER_STARTSINGLE));
      بازگشت CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    اگر(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    اگر(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      اگر(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        برو evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  بازگشت result;
}

ایستا CURLcode cf_ip_ballers_shutdown(ساختار cf_ip_ballers *bs,
                                       ساختار Curl_easy *data,
                                       bool *done)
{
  ساختار cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  برای(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    اگر(a->shutdown || !a->cf)
      ادامه;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    اگر(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    وگرنه
      *done = FALSE;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_ip_ballers_pollset(ساختار cf_ip_ballers *bs,
                                      ساختار Curl_easy *data,
                                      ساختار easy_pollset *ps)
{
  ساختار cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  برای(a = bs->running; a && !result; a = a->next) {
    اگر(a->result)
      ادامه;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  بازگشت result;
}

ایستا bool cf_ip_ballers_pending(ساختار cf_ip_ballers *bs,
                                  ثابت ساختار Curl_easy *data)
{
  ساختار cf_ip_attempt *a;

  برای(a = bs->running; a; a = a->next) {
    اگر(a->result)
      ادامه;
    اگر(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      بازگشت TRUE;
  }
  بازگشت FALSE;
}

ایستا صحیح cf_ip_ballers_min_reply_ms(ساختار cf_ip_ballers *bs,
                                      ساختار Curl_easy *data)
{
  صحیح reply_ms = -1, breply_ms;
  ساختار cf_ip_attempt *a;

  برای(a = bs->running; a; a = a->next) {
    اگر(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, پوچ) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  بازگشت reply_ms;
}

تعریفنوع شمارش {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

ساختار cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  ساختار cf_ip_ballers ballers;
  BIT(dns_resolved);
};

ایستا CURLcode is_connected(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             bool *connected)
{
  ساختار cf_ip_happy_ctx *ctx = cf->ctx;
  ساختار connectdata *conn = cf->conn;
  CURLcode result;

  result = cf_ip_ballers_run(&ctx->ballers, cf, data,
                             (bool)ctx->dns_resolved, connected);

  اگر(!result)
    بازگشت CURLE_OK;
  وگرنه {
    ساختار Curl_peer *peer = پوچ, *proxy_peer = پوچ;
    نویسه viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    اگر(!conn->origin || !peer)
      بازگشت CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    اگر(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    وگرنه اگر(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    اگر(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      اگر(peer->unix_socket)
        curl_msnprintf(viamsg, اندازه(viamsg), " over unix://%s",
                       peer->hostname);
      وگرنه
#endif
      curl_msnprintf(viamsg, اندازه(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(data, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(data, پوچ, TIMER_STARTSINGLE),
          curl_easy_strerror(result));

#ifdef SOCKETIMEDOUT
    اگر(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    بازگشت result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

ایستا CURLcode cf_ip_happy_init(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data)
{
  ساختار cf_ip_happy_ctx *ctx = cf->ctx;

  اگر(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    بازگشت CURLE_OPERATION_TIMEDOUT;
  }

  اگر(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#وگرنه
    بازگشت CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  وگرنه { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(data, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  بازگشت CURLE_OK;
}

ایستا تهی cf_ip_happy_ctx_clear(ساختار cf_ip_happy_ctx *ctx,
                                  ساختار Curl_easy *data)
{
  DEBUGASSERT(ctx);
  اگر(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

ایستا تهی cf_ip_happy_ctx_destroy(ساختار cf_ip_happy_ctx *ctx,
                                    ساختار Curl_easy *data)
{
  اگر(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    curlx_free(ctx);
  }
}

ایستا CURLcode cf_ip_happy_shutdown(ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data,
                                     bool *done)
{
  ساختار cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (صحیح)result, *done);
  بازگشت result;
}

ایستا CURLcode cf_ip_happy_adjust_pollset(ساختار Curl_cfilter *cf,
                                           ساختار Curl_easy *data,
                                           ساختار easy_pollset *ps)
{
  ساختار cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  اگر(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (صحیح)result,
                ps->n);
  }
  بازگشت result;
}

ایستا CURLcode cf_ip_happy_connect(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    bool *done)
{
  ساختار cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  اگر(!data)
    بازگشت CURLE_FAILED_INIT;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  اگر(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    اگر(!result)
      ctx->dns_resolved = TRUE;
    وگرنه اگر(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    وگرنه /* real error */
      برو out;
  }

  انتخاب(ctx->state) {
  حالت SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    اگر(result)
      برو out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  حالت SCFST_WAITING:
    result = is_connected(cf, data, done);
    اگر(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = پوچ;

      اگر(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      اگر(Curl_trc_cf_is_verbose(cf, data)) {
        ساختار ip_quadruple ipquad;
        bool is_ipv6;
        اگر(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
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
    بشکن;
  حالت SCFST_DONE:
    *done = TRUE;
    بشکن;
  }
out:
  بازگشت result;
}

ایستا bool cf_ip_happy_data_pending(ساختار Curl_cfilter *cf,
                                     ثابت ساختار Curl_easy *data)
{
  ساختار cf_ip_happy_ctx *ctx = cf->ctx;

  اگر(!cf->connected) {
    بازگشت cf_ip_ballers_pending(&ctx->ballers, data);
  }
  بازگشت cf->next->cft->has_data_pending(cf->next, data);
}

ایستا CURLcode cf_ip_happy_query(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار cf_ip_happy_ctx *ctx = cf->ctx;

  اگر(!cf->connected) {
    انتخاب(query) {
    حالت CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      بازگشت CURLE_OK;
    }
    پیشفرض:
      بشکن;
    }
  }

  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ایستا تهی cf_ip_happy_destroy(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data)
{
  ساختار cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  اگر(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
}

ساختار Curl_cftype Curl_cft_ip_happy = {
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

ایستا CURLcode cf_ip_happy_create(ساختار Curl_cfilter **pcf,
                                   ساختار Curl_easy *data,
                                   ساختار Curl_peer *origin,
                                   ساختار Curl_peer *peer,
                                   uint8_t transport_peer,
                                   ساختار connectdata *conn,
                                   ساختار Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  ساختار cf_ip_happy_ctx *ctx = پوچ;
  CURLcode result;

  (تهی)data;
  (تهی)conn;
  *pcf = پوچ;
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  اگر(result)
    برو out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  اگر(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  بازگشت result;
}

CURLcode Curl_cf_ip_happy_درج_after(ساختار Curl_cfilter *cf_at,
                                       ساختار Curl_easy *data,
                                       ساختار Curl_peer *origin,
                                       ساختار Curl_peer *peer,
                                       uint8_t transport_peer,
                                       ساختار Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  ساختار Curl_cfilter *cf;
  CURLcode result;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  result = cf_ip_happy_create(&cf, data, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  اگر(result)
    بازگشت result;

  Curl_conn_cf_درج_after(cf_at, cf);
  بازگشت CURLE_OK;
}
