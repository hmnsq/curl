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

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_LINUX_TCP_H
#अन्तर्भाव <linux/tcp.h>
#अथवायदि defined(HAVE_NETINET_TCP_H)
#अन्तर्भाव <netinet/tcp.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif

#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

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
#ifndef UNITTESTS
अचर
#endif
संरचना transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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

स्थिर cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  माप_प्रकार i;
  लागि(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    यदि((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      फर्काऊ transport_providers[i].cf_create;
  }
  फर्काऊ रिक्त;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST शून्य debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST शून्य debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  माप_प्रकार i;
  लागि(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    यदि(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

संरचना cf_ai_iter {
  संरचना Curl_cfilter *cf;
  संरचना Curl_peer *peer;
  पूर्णाङ्क ai_family;
  अचिन्हित पूर्णाङ्क n;
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

स्थिर अचर संरचना Curl_addrinfo *cf_ai_iter_next(संरचना cf_ai_iter *iter,
                                                   संरचना Curl_easy *data)
{
  अचर संरचना Curl_addrinfo *addr;

  यदि(!iter->cf)
    फर्काऊ रिक्त;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  यदि(addr)
    iter->n++;
  फर्काऊ addr;
}

स्थिर bool cf_ai_iter_has_more(संरचना cf_ai_iter *iter,
                                संरचना Curl_easy *data)
{
  फर्काऊ (iter->cf &&
          !!Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

संरचना cf_ip_attempt {
  संरचना cf_ip_attempt *next;
  संरचना Curl_peer *origin;
  संरचना Curl_peer *peer;
  संरचना Curl_peer *tunnel_peer;
  संरचना Curl_sockaddr_ex addr;
  संरचना Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode result;
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
                               संरचना Curl_easy *data)
{
  यदि(a) {
    यदि(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, data);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

स्थिर CURLcode cf_ip_attempt_new(संरचना cf_ip_attempt **pa,
                                  संरचना Curl_easy *data,
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
  CURLcode result = CURLE_OK;

  *pa = रिक्त;
  a = curlx_calloc(1, परिमाण(*a));
  यदि(!a)
    फर्काऊ CURLE_OUT_OF_MEMORY;

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
  यदि(result)
    गच्छ out;

  /* the new filter might have sub-filters */
  लागि(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  यदि(result) {
    cf_ip_attempt_free(a, data);
    *pa = रिक्त;
  }
  फर्काऊ result;
}

स्थिर CURLcode cf_ip_attempt_connect(संरचना cf_ip_attempt *a,
                                      संरचना Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  यदि(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    यदि(!a->result) {
      यदि(*connected) {
        a->connected = TRUE;
      }
    }
    अन्यथा {
      यदि(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      यदि(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  फर्काऊ a->result;
}

संरचना cf_ip_ballers {
  संरचना cf_ip_attempt *running;
  संरचना cf_ip_attempt **running_tail;  /* &running or &(last->next) */
  संरचना cf_ip_attempt *winner;
  संरचना cf_ai_iter addr_iter;
#ifdef USE_IPV6
  संरचना cf_ai_iter ipv6_iter;
#endif
  संरचना Curl_peer *origin;
  संरचना Curl_peer *peer;
  संरचना Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  संरचना curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_result;  /* result of last discarded hard failure */
  पूर्णाङ्क last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

स्थिर CURLcode cf_ip_attempt_restart(संरचना cf_ip_attempt *a,
                                      संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *wcf;
  CURLcode result;

  यदि(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = रिक्त;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  यदि(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    लागि(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  फर्काऊ result;
}

स्थिर शून्य cf_ip_ballers_clear(संरचना Curl_easy *data,
                                संरचना cf_ip_ballers *bs)
{
  जबसम्म(bs->running) {
    संरचना cf_ip_attempt *a = bs->running;
    bs->running = a->next;
    cf_ip_attempt_free(a, data);
  }
  bs->running_tail = &bs->running;
  cf_ip_attempt_free(bs->winner, data);
  bs->winner = रिक्त;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

स्थिर CURLcode cf_ip_ballers_init(संरचना cf_ip_ballers *bs,
                                   संरचना Curl_easy *data,
                                   संरचना Curl_peer *origin,
                                   संरचना Curl_peer *peer,
                                   uint8_t transport_peer,
                                   संरचना Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  स_रखो(bs, 0, परिमाण(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  यदि(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    फर्काऊ CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  फर्काऊ CURLE_OK;
}

स्थिर शून्य cf_ip_ballers_prune(संरचना cf_ip_ballers *bs,
                                संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                uint32_t max_concurrent)
{
  संरचना cf_ip_attempt *a = रिक्त, **panchor;
  uint32_t ongoing = 0;

  लागि(a = bs->running; a; a = a->next) {
    यदि(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  जबसम्म(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    यदि(!a->result && !a->connected) {
      *panchor = a->next;
      यदि(!a->next)
        bs->running_tail = panchor;
      a->next = रिक्त;
      cf_ip_attempt_free(a, data);
      --ongoing;
      CURL_TRC_CF(data, cf, "discarding oldest attempt to keep limit");
    }
    अन्यथा {
      panchor = &a->next;
    }
  }
}

स्थिर CURLcode cf_ip_ballers_run(संरचना cf_ip_ballers *bs,
                                  संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode result = CURLE_OK;
  संरचना cf_ip_attempt *a = रिक्त, **panchor;
  bool do_more;
  timediff_t next_expire_ms;
  uint32_t inconclusive, ongoing;
  अचर संरचना curltime *pnow = रिक्त;
  VERBOSE(पूर्णाङ्क i);

  यदि(bs->winner)
    फर्काऊ CURLE_OK;

evaluate:
  यदि(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, रिक्त, TIMER_STARTSINGLE));
    फर्काऊ CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  लागि(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    यदि(!a->result) {
      यदि(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = रिक्त;
        जबसम्म(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        फर्काऊ CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    अन्यथा यदि(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    अन्यथा {
      /* hard failure, discard right away */
      *panchor = a->next;
      यदि(!a->next)
        bs->running_tail = panchor;
      a->next = रिक्त;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  यदि(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  यदि(!ongoing) {
    do_more = TRUE;
  }
  अन्यथा {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    यदि(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    यदि(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  यदि(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    अचर संरचना Curl_addrinfo *ai = रिक्त;
    पूर्णाङ्क ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    यदि((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    यदि(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    यदि(ai) {  /* try another address */
      संरचना Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      यदि(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      यदि(result)
        गच्छ out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (पूर्णाङ्क)result);
      यदि(result)
        गच्छ out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_tail = a;
      a->next = रिक्त;
      bs->running_tail = &a->next;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      गच्छ evaluate;
    }
    अन्यथा यदि(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      यदि(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        लागि(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          यदि(!a->inconclusive)
            जारी;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (पूर्णाङ्क)result);
          यदि(result) /* serious failure */
            गच्छ out;
          bs->last_attempt_started = *pnow;
          गच्छ evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      अन्यथा {
        /* let's wait some more before restarting */
        infof(data, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      गच्छ out;
    }
    अन्यथा यदि(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  यदि(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    यदि(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, रिक्त, TIMER_STARTSINGLE));
      फर्काऊ CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    यदि(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    यदि(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      यदि(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        गच्छ evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  फर्काऊ result;
}

स्थिर CURLcode cf_ip_ballers_shutdown(संरचना cf_ip_ballers *bs,
                                       संरचना Curl_easy *data,
                                       bool *done)
{
  संरचना cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  लागि(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    यदि(a->shutdown || !a->cf)
      जारी;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    यदि(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    अन्यथा
      *done = FALSE;
  }
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode cf_ip_ballers_pollset(संरचना cf_ip_ballers *bs,
                                      संरचना Curl_easy *data,
                                      संरचना easy_pollset *ps)
{
  संरचना cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  लागि(a = bs->running; a && !result; a = a->next) {
    यदि(a->result)
      जारी;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  फर्काऊ result;
}

स्थिर bool cf_ip_ballers_pending(संरचना cf_ip_ballers *bs,
                                  अचर संरचना Curl_easy *data)
{
  संरचना cf_ip_attempt *a;

  लागि(a = bs->running; a; a = a->next) {
    यदि(a->result)
      जारी;
    यदि(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      फर्काऊ TRUE;
  }
  फर्काऊ FALSE;
}

स्थिर पूर्णाङ्क cf_ip_ballers_min_reply_ms(संरचना cf_ip_ballers *bs,
                                      संरचना Curl_easy *data)
{
  पूर्णाङ्क reply_ms = -1, breply_ms;
  संरचना cf_ip_attempt *a;

  लागि(a = bs->running; a; a = a->next) {
    यदि(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, रिक्त) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  फर्काऊ reply_ms;
}

प्रकारनिर्देश गणना {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

संरचना cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  संरचना cf_ip_ballers ballers;
  BIT(dns_resolved);
};

स्थिर CURLcode is_connected(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             bool *connected)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;
  संरचना connectdata *conn = cf->conn;
  CURLcode result;

  result = cf_ip_ballers_run(&ctx->ballers, cf, data,
                             (bool)ctx->dns_resolved, connected);

  यदि(!result)
    फर्काऊ CURLE_OK;
  अन्यथा {
    संरचना Curl_peer *peer = रिक्त, *proxy_peer = रिक्त;
    वर्ण viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    यदि(!conn->origin || !peer)
      फर्काऊ CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    यदि(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    अन्यथा यदि(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    यदि(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      यदि(peer->unix_socket)
        curl_msnprintf(viamsg, परिमाण(viamsg), " over unix://%s",
                       peer->hostname);
      अन्यथा
#endif
      curl_msnprintf(viamsg, परिमाण(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(data, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(data, रिक्त, TIMER_STARTSINGLE),
          curl_easy_strerror(result));

#ifdef SOCKETIMEDOUT
    यदि(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    फर्काऊ result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

स्थिर CURLcode cf_ip_happy_init(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  यदि(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    फर्काऊ CURLE_OPERATION_TIMEDOUT;
  }

  यदि(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#अन्यथा
    फर्काऊ CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  अन्यथा { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(data, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  फर्काऊ CURLE_OK;
}

स्थिर शून्य cf_ip_happy_ctx_clear(संरचना cf_ip_happy_ctx *ctx,
                                  संरचना Curl_easy *data)
{
  DEBUGASSERT(ctx);
  यदि(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

स्थिर शून्य cf_ip_happy_ctx_destroy(संरचना cf_ip_happy_ctx *ctx,
                                    संरचना Curl_easy *data)
{
  यदि(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    curlx_free(ctx);
  }
}

स्थिर CURLcode cf_ip_happy_shutdown(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data,
                                     bool *done)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  यदि(cf->connected) {
    *done = TRUE;
    फर्काऊ CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (पूर्णाङ्क)result, *done);
  फर्काऊ result;
}

स्थिर CURLcode cf_ip_happy_adjust_pollset(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *data,
                                           संरचना easy_pollset *ps)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  यदि(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (पूर्णाङ्क)result,
                ps->n);
  }
  फर्काऊ result;
}

स्थिर CURLcode cf_ip_happy_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    bool *done)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  यदि(!data)
    फर्काऊ CURLE_FAILED_INIT;

  यदि(cf->connected) {
    *done = TRUE;
    फर्काऊ CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  यदि(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    यदि(!result)
      ctx->dns_resolved = TRUE;
    अन्यथा यदि(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    अन्यथा /* real error */
      गच्छ out;
  }

  विकल्प(ctx->state) {
  पक्ष SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    यदि(result)
      गच्छ out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  पक्ष SCFST_WAITING:
    result = is_connected(cf, data, done);
    यदि(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = रिक्त;

      यदि(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      यदि(Curl_trc_cf_is_verbose(cf, data)) {
        संरचना ip_quadruple ipquad;
        bool is_ipv6;
        यदि(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
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
    रोक;
  पक्ष SCFST_DONE:
    *done = TRUE;
    रोक;
  }
out:
  फर्काऊ result;
}

स्थिर bool cf_ip_happy_data_pending(संरचना Curl_cfilter *cf,
                                     अचर संरचना Curl_easy *data)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  यदि(!cf->connected) {
    फर्काऊ cf_ip_ballers_pending(&ctx->ballers, data);
  }
  फर्काऊ cf->next->cft->has_data_pending(cf->next, data);
}

स्थिर CURLcode cf_ip_happy_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  यदि(!cf->connected) {
    विकल्प(query) {
    पक्ष CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      फर्काऊ CURLE_OK;
    }
    स्वभाव:
      रोक;
    }
  }

  फर्काऊ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर शून्य cf_ip_happy_destroy(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  यदि(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
}

संरचना Curl_cftype Curl_cft_ip_happy = {
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

स्थिर CURLcode cf_ip_happy_create(संरचना Curl_cfilter **pcf,
                                   संरचना Curl_easy *data,
                                   संरचना Curl_peer *origin,
                                   संरचना Curl_peer *peer,
                                   uint8_t transport_peer,
                                   संरचना connectdata *conn,
                                   संरचना Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  संरचना cf_ip_happy_ctx *ctx = रिक्त;
  CURLcode result;

  (शून्य)data;
  (शून्य)conn;
  *pcf = रिक्त;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  यदि(result)
    गच्छ out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  यदि(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  फर्काऊ result;
}

CURLcode Curl_cf_ip_happy_घुसाउ_after(संरचना Curl_cfilter *cf_at,
                                       संरचना Curl_easy *data,
                                       संरचना Curl_peer *origin,
                                       संरचना Curl_peer *peer,
                                       uint8_t transport_peer,
                                       संरचना Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  result = cf_ip_happy_create(&cf, data, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  यदि(result)
    फर्काऊ result;

  Curl_conn_cf_घुसाउ_after(cf_at, cf);
  फर्काऊ CURLE_OK;
}
