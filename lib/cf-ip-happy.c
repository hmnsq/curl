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
#किंवाजर defined(HAVE_NETINET_TCP_H)
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
स्थिरांक
#endif
संरचना transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#जर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#जर !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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
  साठी(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    जर((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      परत transport_providers[i].cf_create;
  }
  परत रिक्त;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST शून्य debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST शून्य debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  माप_प्रकार i;
  साठी(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    जर(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

संरचना cf_ai_iter {
  संरचना Curl_cfilter *cf;
  संरचना Curl_peer *peer;
  पूर्णांक ai_family;
  अचिन्हित पूर्णांक n;
};

स्थिर शून्य cf_ai_iter_init(संरचना cf_ai_iter *iter,
                            संरचना Curl_cfilter *cf,
                            संरचना Curl_peer *peer,
                            पूर्णांक ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

स्थिर स्थिरांक संरचना Curl_addrinfo *cf_ai_iter_next(संरचना cf_ai_iter *iter,
                                                   संरचना Curl_easy *data)
{
  स्थिरांक संरचना Curl_addrinfo *addr;

  जर(!iter->cf)
    परत रिक्त;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  जर(addr)
    iter->n++;
  परत addr;
}

स्थिर bool cf_ai_iter_has_more(संरचना cf_ai_iter *iter,
                                संरचना Curl_easy *data)
{
  परत (iter->cf &&
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
  पूर्णांक ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  पूर्णांक error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

स्थिर शून्य cf_ip_attempt_free(संरचना cf_ip_attempt *a,
                               संरचना Curl_easy *data)
{
  जर(a) {
    जर(a->cf)
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
                                  पूर्णांक ai_family,
                                  संरचना Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  संरचना Curl_cfilter *wcf;
  संरचना cf_ip_attempt *a;
  CURLcode result = CURLE_OK;

  *pa = रिक्त;
  a = curlx_calloc(1, आकार(*a));
  जर(!a)
    परत CURLE_OUT_OF_MEMORY;

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
  जर(result)
    जा out;

  /* the new filter might have sub-filters */
  साठी(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  जर(result) {
    cf_ip_attempt_free(a, data);
    *pa = रिक्त;
  }
  परत result;
}

स्थिर CURLcode cf_ip_attempt_connect(संरचना cf_ip_attempt *a,
                                      संरचना Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  जर(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    जर(!a->result) {
      जर(*connected) {
        a->connected = TRUE;
      }
    }
    अन्यथा {
      जर(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      जर(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  परत a->result;
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
  पूर्णांक last_attempt_ai_family;
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

  जर(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = रिक्त;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  जर(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    साठी(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  परत result;
}

स्थिर शून्य cf_ip_ballers_clear(संरचना Curl_easy *data,
                                संरचना cf_ip_ballers *bs)
{
  पर्यंत(bs->running) {
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
  स_रखो(bs, 0, आकार(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  जर(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    परत CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  परत CURLE_OK;
}

स्थिर शून्य cf_ip_ballers_prune(संरचना cf_ip_ballers *bs,
                                संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                uint32_t max_concurrent)
{
  संरचना cf_ip_attempt *a = रिक्त, **panchor;
  uint32_t ongoing = 0;

  साठी(a = bs->running; a; a = a->next) {
    जर(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  पर्यंत(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    जर(!a->result && !a->connected) {
      *panchor = a->next;
      जर(!a->next)
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
  स्थिरांक संरचना curltime *pnow = रिक्त;
  VERBOSE(पूर्णांक i);

  जर(bs->winner)
    परत CURLE_OK;

evaluate:
  जर(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, रिक्त, TIMER_STARTSINGLE));
    परत CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  साठी(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    जर(!a->result) {
      जर(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = रिक्त;
        पर्यंत(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        परत CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    अन्यथा जर(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    अन्यथा {
      /* hard failure, discard right away */
      *panchor = a->next;
      जर(!a->next)
        bs->running_tail = panchor;
      a->next = रिक्त;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  जर(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  जर(!ongoing) {
    do_more = TRUE;
  }
  अन्यथा {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    जर(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    जर(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  जर(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    स्थिरांक संरचना Curl_addrinfo *ai = रिक्त;
    पूर्णांक ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    जर((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    जर(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    जर(ai) {  /* try another address */
      संरचना Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      जर(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      जर(result)
        जा out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (पूर्णांक)result);
      जर(result)
        जा out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_tail = a;
      a->next = रिक्त;
      bs->running_tail = &a->next;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      जा evaluate;
    }
    अन्यथा जर(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      जर(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        साठी(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          जर(!a->inconclusive)
            सुरूठेवा;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (पूर्णांक)result);
          जर(result) /* serious failure */
            जा out;
          bs->last_attempt_started = *pnow;
          जा evaluate;
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
      जा out;
    }
    अन्यथा जर(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  जर(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    जर(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, रिक्त, TIMER_STARTSINGLE));
      परत CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    जर(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    जर(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      जर(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        जा evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  परत result;
}

स्थिर CURLcode cf_ip_ballers_shutdown(संरचना cf_ip_ballers *bs,
                                       संरचना Curl_easy *data,
                                       bool *done)
{
  संरचना cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  साठी(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    जर(a->shutdown || !a->cf)
      सुरूठेवा;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    जर(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    अन्यथा
      *done = FALSE;
  }
  परत CURLE_OK;
}

स्थिर CURLcode cf_ip_ballers_pollset(संरचना cf_ip_ballers *bs,
                                      संरचना Curl_easy *data,
                                      संरचना easy_pollset *ps)
{
  संरचना cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  साठी(a = bs->running; a && !result; a = a->next) {
    जर(a->result)
      सुरूठेवा;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  परत result;
}

स्थिर bool cf_ip_ballers_pending(संरचना cf_ip_ballers *bs,
                                  स्थिरांक संरचना Curl_easy *data)
{
  संरचना cf_ip_attempt *a;

  साठी(a = bs->running; a; a = a->next) {
    जर(a->result)
      सुरूठेवा;
    जर(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      परत TRUE;
  }
  परत FALSE;
}

स्थिर पूर्णांक cf_ip_ballers_min_reply_ms(संरचना cf_ip_ballers *bs,
                                      संरचना Curl_easy *data)
{
  पूर्णांक reply_ms = -1, breply_ms;
  संरचना cf_ip_attempt *a;

  साठी(a = bs->running; a; a = a->next) {
    जर(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, रिक्त) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  परत reply_ms;
}

प्रकारव्याख्या गणना {
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

  जर(!result)
    परत CURLE_OK;
  अन्यथा {
    संरचना Curl_peer *peer = रिक्त, *proxy_peer = रिक्त;
    अक्षर viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    जर(!conn->origin || !peer)
      परत CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    जर(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    अन्यथा जर(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    जर(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      जर(peer->unix_socket)
        curl_msnprintf(viamsg, आकार(viamsg), " over unix://%s",
                       peer->hostname);
      अन्यथा
#endif
      curl_msnprintf(viamsg, आकार(viamsg), " via %s:%u",
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
    जर(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    परत result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

स्थिर CURLcode cf_ip_happy_init(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  जर(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    परत CURLE_OPERATION_TIMEDOUT;
  }

  जर(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#अन्यथा
    परत CURLE_UNSUPPORTED_PROTOCOL;
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
  परत CURLE_OK;
}

स्थिर शून्य cf_ip_happy_ctx_clear(संरचना cf_ip_happy_ctx *ctx,
                                  संरचना Curl_easy *data)
{
  DEBUGASSERT(ctx);
  जर(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

स्थिर शून्य cf_ip_happy_ctx_destroy(संरचना cf_ip_happy_ctx *ctx,
                                    संरचना Curl_easy *data)
{
  जर(ctx) {
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
  जर(cf->connected) {
    *done = TRUE;
    परत CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (पूर्णांक)result, *done);
  परत result;
}

स्थिर CURLcode cf_ip_happy_adjust_pollset(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *data,
                                           संरचना easy_pollset *ps)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  जर(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (पूर्णांक)result,
                ps->n);
  }
  परत result;
}

स्थिर CURLcode cf_ip_happy_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    bool *done)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  जर(!data)
    परत CURLE_FAILED_INIT;

  जर(cf->connected) {
    *done = TRUE;
    परत CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  जर(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    जर(!result)
      ctx->dns_resolved = TRUE;
    अन्यथा जर(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    अन्यथा /* real error */
      जा out;
  }

  निवड(ctx->state) {
  स्थिती SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    जर(result)
      जा out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  स्थिती SCFST_WAITING:
    result = is_connected(cf, data, done);
    जर(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = रिक्त;

      जर(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      जर(Curl_trc_cf_is_verbose(cf, data)) {
        संरचना ip_quadruple ipquad;
        bool is_ipv6;
        जर(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
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
    थांबा;
  स्थिती SCFST_DONE:
    *done = TRUE;
    थांबा;
  }
out:
  परत result;
}

स्थिर bool cf_ip_happy_data_pending(संरचना Curl_cfilter *cf,
                                     स्थिरांक संरचना Curl_easy *data)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  जर(!cf->connected) {
    परत cf_ip_ballers_pending(&ctx->ballers, data);
  }
  परत cf->next->cft->has_data_pending(cf->next, data);
}

स्थिर CURLcode cf_ip_happy_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णांक query, पूर्णांक *pres1, शून्य *pres2)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  जर(!cf->connected) {
    निवड(query) {
    स्थिती CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      परत CURLE_OK;
    }
    पूर्वनिर्धारित:
      थांबा;
    }
  }

  परत cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर शून्य cf_ip_happy_destroy(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data)
{
  संरचना cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  जर(ctx) {
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
  ctx = curlx_calloc(1, आकार(*ctx));
  जर(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  जर(result)
    जा out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  जर(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  परत result;
}

CURLcode Curl_cf_ip_happy_घाला_after(संरचना Curl_cfilter *cf_at,
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
  जर(result)
    परत result;

  Curl_conn_cf_घाला_after(cf_at, cf);
  परत CURLE_OK;
}
