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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_LINUX_TCP_H
#অন্তর্ভাৱ <linux/tcp.h>
#নোচেদ্যদি defined(HAVE_NETINET_TCP_H)
#অন্তর্ভাৱ <netinet/tcp.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাৱ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif

#ifdef __VMS
#অন্তর্ভাৱ <in.h>
#অন্তর্ভাৱ <inet.h>
#endif

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "cf-ip-happy.h"
#অন্তর্ভাৱ "curl_addrinfo.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "sockaddr.h"
#অন্তর্ভাৱ "vdns/cf-dns.h"
#অন্তর্ভাৱ "vquic/vquic.h" /* for quic cfilters */


সংরচনা transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

স্থির
#ifndef UNITTESTS
ধ্রুৱ
#endif
সংরচনা transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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

স্থির cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  বাবে(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    যদি((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      ঘূৰাই transport_providers[i].cf_create;
  }
  ঘূৰাই রিক্ত;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST শূন্য debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST শূন্য debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  বাবে(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    যদি(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

সংরচনা cf_ai_iter {
  সংরচনা Curl_cfilter *cf;
  সংরচনা Curl_peer *peer;
  পূর্ণাঙ্ক ai_family;
  unsigned পূর্ণাঙ্ক n;
};

স্থির শূন্য cf_ai_iter_init(সংরচনা cf_ai_iter *iter,
                            সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_peer *peer,
                            পূর্ণাঙ্ক ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

স্থির ধ্রুৱ সংরচনা Curl_addrinfo *cf_ai_iter_next(সংরচনা cf_ai_iter *iter,
                                                   সংরচনা Curl_easy *data)
{
  ধ্রুৱ সংরচনা Curl_addrinfo *addr;

  যদি(!iter->cf)
    ঘূৰাই রিক্ত;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  যদি(addr)
    iter->n++;
  ঘূৰাই addr;
}

স্থির bool cf_ai_iter_has_more(সংরচনা cf_ai_iter *iter,
                                সংরচনা Curl_easy *data)
{
  ঘূৰাই (iter->cf &&
          !!Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

সংরচনা cf_ip_attempt {
  সংরচনা cf_ip_attempt *next;
  সংরচনা Curl_peer *origin;
  সংরচনা Curl_peer *peer;
  সংরচনা Curl_peer *tunnel_peer;
  সংরচনা Curl_sockaddr_ex addr;
  সংরচনা Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode result;
  পূর্ণাঙ্ক ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  পূর্ণাঙ্ক error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

স্থির শূন্য cf_ip_attempt_free(সংরচনা cf_ip_attempt *a,
                               সংরচনা Curl_easy *data)
{
  যদি(a) {
    যদি(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, data);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

স্থির CURLcode cf_ip_attempt_new(সংরচনা cf_ip_attempt **pa,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_peer *origin,
                                  সংরচনা Curl_peer *peer,
                                  uint8_t transport_peer,
                                  সংরচনা Curl_sockaddr_ex *addr,
                                  পূর্ণাঙ্ক ai_family,
                                  সংরচনা Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  সংরচনা Curl_cfilter *wcf;
  সংরচনা cf_ip_attempt *a;
  CURLcode result = CURLE_OK;

  *pa = রিক্ত;
  a = curlx_calloc(1, পরিমাণ(*a));
  যদি(!a)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

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
  যদি(result)
    গচ্ছ out;

  /* the new filter might have sub-filters */
  বাবে(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  যদি(result) {
    cf_ip_attempt_free(a, data);
    *pa = রিক্ত;
  }
  ঘূৰাই result;
}

স্থির CURLcode cf_ip_attempt_connect(সংরচনা cf_ip_attempt *a,
                                      সংরচনা Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  যদি(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    যদি(!a->result) {
      যদি(*connected) {
        a->connected = TRUE;
      }
    }
    অন্যথা {
      যদি(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      যদি(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  ঘূৰাই a->result;
}

সংরচনা cf_ip_ballers {
  সংরচনা cf_ip_attempt *running;
  সংরচনা cf_ip_attempt **running_tail;  /* &running or &(last->next) */
  সংরচনা cf_ip_attempt *winner;
  সংরচনা cf_ai_iter addr_iter;
#ifdef USE_IPV6
  সংরচনা cf_ai_iter ipv6_iter;
#endif
  সংরচনা Curl_peer *origin;
  সংরচনা Curl_peer *peer;
  সংরচনা Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  সংরচনা curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_result;  /* result of last discarded hard failure */
  পূর্ণাঙ্ক last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

স্থির CURLcode cf_ip_attempt_restart(সংরচনা cf_ip_attempt *a,
                                      সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cfilter *wcf;
  CURLcode result;

  যদি(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = রিক্ত;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  যদি(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    বাবে(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  ঘূৰাই result;
}

স্থির শূন্য cf_ip_ballers_clear(সংরচনা Curl_easy *data,
                                সংরচনা cf_ip_ballers *bs)
{
  যেতিয়ালৈ(bs->running) {
    সংরচনা cf_ip_attempt *a = bs->running;
    bs->running = a->next;
    cf_ip_attempt_free(a, data);
  }
  bs->running_tail = &bs->running;
  cf_ip_attempt_free(bs->winner, data);
  bs->winner = রিক্ত;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

স্থির CURLcode cf_ip_ballers_init(সংরচনা cf_ip_ballers *bs,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা Curl_peer *origin,
                                   সংরচনা Curl_peer *peer,
                                   uint8_t transport_peer,
                                   সংরচনা Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, পরিমাণ(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  যদি(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    ঘূৰাই CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  ঘূৰাই CURLE_OK;
}

স্থির শূন্য cf_ip_ballers_prune(সংরচনা cf_ip_ballers *bs,
                                সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                uint32_t max_concurrent)
{
  সংরচনা cf_ip_attempt *a = রিক্ত, **panchor;
  uint32_t ongoing = 0;

  বাবে(a = bs->running; a; a = a->next) {
    যদি(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  যেতিয়ালৈ(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    যদি(!a->result && !a->connected) {
      *panchor = a->next;
      যদি(!a->next)
        bs->running_tail = panchor;
      a->next = রিক্ত;
      cf_ip_attempt_free(a, data);
      --ongoing;
      CURL_TRC_CF(data, cf, "discarding oldest attempt to keep limit");
    }
    অন্যথা {
      panchor = &a->next;
    }
  }
}

স্থির CURLcode cf_ip_ballers_run(সংরচনা cf_ip_ballers *bs,
                                  সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode result = CURLE_OK;
  সংরচনা cf_ip_attempt *a = রিক্ত, **panchor;
  bool do_more;
  timediff_t next_expire_ms;
  uint32_t inconclusive, ongoing;
  ধ্রুৱ সংরচনা curltime *pnow = রিক্ত;
  VERBOSE(পূর্ণাঙ্ক i);

  যদি(bs->winner)
    ঘূৰাই CURLE_OK;

evaluate:
  যদি(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, রিক্ত, TIMER_STARTSINGLE));
    ঘূৰাই CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  বাবে(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    যদি(!a->result) {
      যদি(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = রিক্ত;
        যেতিয়ালৈ(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        ঘূৰাই CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    অন্যথা যদি(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    অন্যথা {
      /* hard failure, discard right away */
      *panchor = a->next;
      যদি(!a->next)
        bs->running_tail = panchor;
      a->next = রিক্ত;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  যদি(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  যদি(!ongoing) {
    do_more = TRUE;
  }
  অন্যথা {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    যদি(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    যদি(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  যদি(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    ধ্রুৱ সংরচনা Curl_addrinfo *ai = রিক্ত;
    পূর্ণাঙ্ক ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    যদি((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    যদি(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    যদি(ai) {  /* try another address */
      সংরচনা Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      যদি(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      যদি(result)
        গচ্ছ out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (পূর্ণাঙ্ক)result);
      যদি(result)
        গচ্ছ out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_tail = a;
      a->next = রিক্ত;
      bs->running_tail = &a->next;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      গচ্ছ evaluate;
    }
    অন্যথা যদি(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      যদি(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        বাবে(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          যদি(!a->inconclusive)
            অনুৱর্ত;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (পূর্ণাঙ্ক)result);
          যদি(result) /* serious failure */
            গচ্ছ out;
          bs->last_attempt_started = *pnow;
          গচ্ছ evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      অন্যথা {
        /* let's wait some more before restarting */
        infof(data, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      গচ্ছ out;
    }
    অন্যথা যদি(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  যদি(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    যদি(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, রিক্ত, TIMER_STARTSINGLE));
      ঘূৰাই CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    যদি(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    যদি(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      যদি(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        গচ্ছ evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  ঘূৰাই result;
}

স্থির CURLcode cf_ip_ballers_shutdown(সংরচনা cf_ip_ballers *bs,
                                       সংরচনা Curl_easy *data,
                                       bool *done)
{
  সংরচনা cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  বাবে(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    যদি(a->shutdown || !a->cf)
      অনুৱর্ত;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    যদি(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    অন্যথা
      *done = FALSE;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cf_ip_ballers_pollset(সংরচনা cf_ip_ballers *bs,
                                      সংরচনা Curl_easy *data,
                                      সংরচনা easy_pollset *ps)
{
  সংরচনা cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  বাবে(a = bs->running; a && !result; a = a->next) {
    যদি(a->result)
      অনুৱর্ত;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  ঘূৰাই result;
}

স্থির bool cf_ip_ballers_pending(সংরচনা cf_ip_ballers *bs,
                                  ধ্রুৱ সংরচনা Curl_easy *data)
{
  সংরচনা cf_ip_attempt *a;

  বাবে(a = bs->running; a; a = a->next) {
    যদি(a->result)
      অনুৱর্ত;
    যদি(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      ঘূৰাই TRUE;
  }
  ঘূৰাই FALSE;
}

স্থির পূর্ণাঙ্ক cf_ip_ballers_min_reply_ms(সংরচনা cf_ip_ballers *bs,
                                      সংরচনা Curl_easy *data)
{
  পূর্ণাঙ্ক reply_ms = -1, breply_ms;
  সংরচনা cf_ip_attempt *a;

  বাবে(a = bs->running; a; a = a->next) {
    যদি(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, রিক্ত) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  ঘূৰাই reply_ms;
}

প্রকারনির্দেশ গণনা {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

সংরচনা cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  সংরচনা cf_ip_ballers ballers;
  BIT(dns_resolved);
};

স্থির CURLcode is_connected(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             bool *connected)
{
  সংরচনা cf_ip_happy_ctx *ctx = cf->ctx;
  সংরচনা connectdata *conn = cf->conn;
  CURLcode result;

  result = cf_ip_ballers_run(&ctx->ballers, cf, data,
                             (bool)ctx->dns_resolved, connected);

  যদি(!result)
    ঘূৰাই CURLE_OK;
  অন্যথা {
    সংরচনা Curl_peer *peer = রিক্ত, *proxy_peer = রিক্ত;
    ৱর্ণ viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    যদি(!conn->origin || !peer)
      ঘূৰাই CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    যদি(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    অন্যথা যদি(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    যদি(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      যদি(peer->unix_socket)
        curl_msnprintf(viamsg, পরিমাণ(viamsg), " over unix://%s",
                       peer->hostname);
      অন্যথা
#endif
      curl_msnprintf(viamsg, পরিমাণ(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(data, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(data, রিক্ত, TIMER_STARTSINGLE),
          curl_easy_strerror(result));

#ifdef SOCKETIMEDOUT
    যদি(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    ঘূৰাই result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

স্থির CURLcode cf_ip_happy_init(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data)
{
  সংরচনা cf_ip_happy_ctx *ctx = cf->ctx;

  যদি(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    ঘূৰাই CURLE_OPERATION_TIMEDOUT;
  }

  যদি(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#অন্যথা
    ঘূৰাই CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  অন্যথা { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(data, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  ঘূৰাই CURLE_OK;
}

স্থির শূন্য cf_ip_happy_ctx_clear(সংরচনা cf_ip_happy_ctx *ctx,
                                  সংরচনা Curl_easy *data)
{
  DEBUGASSERT(ctx);
  যদি(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

স্থির শূন্য cf_ip_happy_ctx_destroy(সংরচনা cf_ip_happy_ctx *ctx,
                                    সংরচনা Curl_easy *data)
{
  যদি(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    curlx_free(ctx);
  }
}

স্থির CURLcode cf_ip_happy_shutdown(সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data,
                                     bool *done)
{
  সংরচনা cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  যদি(cf->connected) {
    *done = TRUE;
    ঘূৰাই CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (পূর্ণাঙ্ক)result, *done);
  ঘূৰাই result;
}

স্থির CURLcode cf_ip_happy_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                           সংরচনা Curl_easy *data,
                                           সংরচনা easy_pollset *ps)
{
  সংরচনা cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  যদি(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (পূর্ণাঙ্ক)result,
                ps->n);
  }
  ঘূৰাই result;
}

স্থির CURLcode cf_ip_happy_connect(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    bool *done)
{
  সংরচনা cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  যদি(!data)
    ঘূৰাই CURLE_FAILED_INIT;

  যদি(cf->connected) {
    *done = TRUE;
    ঘূৰাই CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  যদি(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    যদি(!result)
      ctx->dns_resolved = TRUE;
    অন্যথা যদি(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    অন্যথা /* real error */
      গচ্ছ out;
  }

  ৱিকল্প(ctx->state) {
  পক্ষ SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    যদি(result)
      গচ্ছ out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  পক্ষ SCFST_WAITING:
    result = is_connected(cf, data, done);
    যদি(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = রিক্ত;

      যদি(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      যদি(Curl_trc_cf_is_verbose(cf, data)) {
        সংরচনা ip_quadruple ipquad;
        bool is_ipv6;
        যদি(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
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
    ভঙ্গ;
  পক্ষ SCFST_DONE:
    *done = TRUE;
    ভঙ্গ;
  }
out:
  ঘূৰাই result;
}

স্থির bool cf_ip_happy_data_pending(সংরচনা Curl_cfilter *cf,
                                     ধ্রুৱ সংরচনা Curl_easy *data)
{
  সংরচনা cf_ip_happy_ctx *ctx = cf->ctx;

  যদি(!cf->connected) {
    ঘূৰাই cf_ip_ballers_pending(&ctx->ballers, data);
  }
  ঘূৰাই cf->next->cft->has_data_pending(cf->next, data);
}

স্থির CURLcode cf_ip_happy_query(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  সংরচনা cf_ip_happy_ctx *ctx = cf->ctx;

  যদি(!cf->connected) {
    ৱিকল্প(query) {
    পক্ষ CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      ঘূৰাই CURLE_OK;
    }
    স্ৱভাৱ:
      ভঙ্গ;
    }
  }

  ঘূৰাই cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির শূন্য cf_ip_happy_destroy(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data)
{
  সংরচনা cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  যদি(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
}

সংরচনা Curl_cftype Curl_cft_ip_happy = {
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

স্থির CURLcode cf_ip_happy_create(সংরচনা Curl_cfilter **pcf,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা Curl_peer *origin,
                                   সংরচনা Curl_peer *peer,
                                   uint8_t transport_peer,
                                   সংরচনা connectdata *conn,
                                   সংরচনা Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  সংরচনা cf_ip_happy_ctx *ctx = রিক্ত;
  CURLcode result;

  (শূন্য)data;
  (শূন্য)conn;
  *pcf = রিক্ত;
  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  যদি(result)
    গচ্ছ out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  যদি(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  ঘূৰাই result;
}

CURLcode Curl_cf_ip_happy_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                       সংরচনা Curl_easy *data,
                                       সংরচনা Curl_peer *origin,
                                       সংরচনা Curl_peer *peer,
                                       uint8_t transport_peer,
                                       সংরচনা Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  result = cf_ip_happy_create(&cf, data, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  যদি(result)
    ঘূৰাই result;

  Curl_conn_cf_ঢোকাও_after(cf_at, cf);
  ঘূৰাই CURLE_OK;
}
