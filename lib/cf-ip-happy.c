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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_LINUX_TCP_H
#અન્તર્ભાવ <linux/tcp.h>
#અથવાજો defined(HAVE_NETINET_TCP_H)
#અન્તર્ભાવ <netinet/tcp.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#અન્તર્ભાવ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif

#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-ip-happy.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "sockaddr.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "vquic/vquic.h" /* for quic cfilters */


સંરચના transport_provider {
  cf_ip_connect_create *cf_create;
  uint8_t transport;
  bool tunnel;
};

સ્થિર
#ifndef UNITTESTS
અચલ
#endif
સંરચના transport_provider transport_providers[] = {
  { Curl_cf_tcp_create, TRNSPRT_TCP, FALSE },
  { Curl_cf_tcp_create, TRNSPRT_TCP, TRUE },
#જો !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { Curl_cf_quic_create, TRNSPRT_QUIC, FALSE },
#endif
#જો !defined(CURL_DISABLE_HTTP) && defined(USE_PROXY_HTTP3)
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

સ્થિર cf_ip_connect_create *get_cf_create(uint8_t transport,
                                           bool tunnel)
{
  size_t i;
  માટે(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    જો((transport == transport_providers[i].transport) &&
       (tunnel == transport_providers[i].tunnel))
      પાછા transport_providers[i].cf_create;
  }
  પાછા ખાલી;
}

#ifdef UNITTESTS
/* @unittest 2600 */
UNITTEST શૂન્ય debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create);
UNITTEST શૂન્ય debug_set_transport_provider(
  uint8_t transport, cf_ip_connect_create *cf_create)
{
  size_t i;
  માટે(i = 0; i < CURL_ARRAYSIZE(transport_providers); ++i) {
    જો(transport == transport_providers[i].transport) {
      transport_providers[i].cf_create = cf_create;
    }
  }
}
#endif /* UNITTESTS */

સંરચના cf_ai_iter {
  સંરચના Curl_cfilter *cf;
  સંરચના Curl_peer *peer;
  પૂર્ણાંક ai_family;
  unsigned પૂર્ણાંક n;
};

સ્થિર શૂન્ય cf_ai_iter_init(સંરચના cf_ai_iter *iter,
                            સંરચના Curl_cfilter *cf,
                            સંરચના Curl_peer *peer,
                            પૂર્ણાંક ai_family)
{
  iter->cf = cf;
  iter->peer = peer; /* not linked, ctx->ballers owns and has same lifetime */
  iter->ai_family = ai_family;
  iter->n = 0;
}

સ્થિર અચલ સંરચના Curl_addrinfo *cf_ai_iter_next(સંરચના cf_ai_iter *iter,
                                                   સંરચના Curl_easy *data)
{
  અચલ સંરચના Curl_addrinfo *addr;

  જો(!iter->cf)
    પાછા ખાલી;

  addr = Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                              iter->ai_family, iter->n);
  જો(addr)
    iter->n++;
  પાછા addr;
}

સ્થિર bool cf_ai_iter_has_more(સંરચના cf_ai_iter *iter,
                                સંરચના Curl_easy *data)
{
  પાછા (iter->cf &&
          !!Curl_conn_dns_get_ai(data, iter->peer, iter->cf->sockindex,
                                 iter->ai_family, iter->n));
}

સંરચના cf_ip_attempt {
  સંરચના cf_ip_attempt *next;
  સંરચના Curl_peer *origin;
  સંરચના Curl_peer *peer;
  સંરચના Curl_peer *tunnel_peer;
  સંરચના Curl_sockaddr_ex addr;
  સંરચના Curl_cfilter *cf;           /* current sub-cfilter connecting */
  cf_ip_connect_create *cf_create;
  CURLcode result;
  પૂર્ણાંક ai_family;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  પૂર્ણાંક error;
  BIT(connected);                    /* cf has connected */
  BIT(shutdown);                     /* cf has shutdown */
  BIT(inconclusive);                 /* connect was not a hard failure, we
                                      * might talk to a restarting server */
};

સ્થિર શૂન્ય cf_ip_attempt_free(સંરચના cf_ip_attempt *a,
                               સંરચના Curl_easy *data)
{
  જો(a) {
    જો(a->cf)
      Curl_conn_cf_discard_chain(&a->cf, data);
    Curl_peer_unlink(&a->origin);
    Curl_peer_unlink(&a->peer);
    Curl_peer_unlink(&a->tunnel_peer);
    curlx_free(a);
  }
}

સ્થિર CURLcode cf_ip_attempt_new(સંરચના cf_ip_attempt **pa,
                                  સંરચના Curl_easy *data,
                                  સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_peer *origin,
                                  સંરચના Curl_peer *peer,
                                  uint8_t transport_peer,
                                  સંરચના Curl_sockaddr_ex *addr,
                                  પૂર્ણાંક ai_family,
                                  સંરચના Curl_peer *tunnel_peer,
                                  uint8_t tunnel_transport,
                                  cf_ip_connect_create *cf_create)
{
  સંરચના Curl_cfilter *wcf;
  સંરચના cf_ip_attempt *a;
  CURLcode result = CURLE_OK;

  *pa = ખાલી;
  a = curlx_calloc(1, કદ(*a));
  જો(!a)
    પાછા CURLE_OUT_OF_MEMORY;

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
  જો(result)
    જાઓ out;

  /* the new filter might have sub-filters */
  માટે(wcf = a->cf; wcf; wcf = wcf->next) {
    wcf->conn = cf->conn;
    wcf->sockindex = cf->sockindex;
  }

out:
  જો(result) {
    cf_ip_attempt_free(a, data);
    *pa = ખાલી;
  }
  પાછા result;
}

સ્થિર CURLcode cf_ip_attempt_connect(સંરચના cf_ip_attempt *a,
                                      સંરચના Curl_easy *data,
                                      bool *connected)
{
  *connected = (bool)a->connected;
  જો(!a->result && !*connected) {
    /* evaluate again */
    a->result = Curl_conn_cf_connect(a->cf, data, connected);

    જો(!a->result) {
      જો(*connected) {
        a->connected = TRUE;
      }
    }
    નહીંતર {
      જો(a->result == CURLE_WEIRD_SERVER_REPLY)
        a->inconclusive = TRUE;
      જો(a->cf)
        Curl_conn_cf_discard_chain(&a->cf, data);
    }
  }
  પાછા a->result;
}

સંરચના cf_ip_ballers {
  સંરચના cf_ip_attempt *running;
  સંરચના cf_ip_attempt **running_tail;  /* &running or &(last->next) */
  સંરચના cf_ip_attempt *winner;
  સંરચના cf_ai_iter addr_iter;
#ifdef USE_IPV6
  સંરચના cf_ai_iter ipv6_iter;
#endif
  સંરચના Curl_peer *origin;
  સંરચના Curl_peer *peer;
  સંરચના Curl_peer *tunnel_peer;
  cf_ip_connect_create *cf_create;   /* for creating cf */
  સંરચના curltime last_attempt_started;
  timediff_t attempt_delay_ms;
  CURLcode last_dead_result;  /* result of last discarded hard failure */
  પૂર્ણાંક last_attempt_ai_family;
  uint32_t max_concurrent;
  uint8_t transport_peer;
  uint8_t tunnel_transport;
  BIT(had_dead);              /* a hard failure was discarded */
};

સ્થિર CURLcode cf_ip_attempt_restart(સંરચના cf_ip_attempt *a,
                                      સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data)
{
  સંરચના Curl_cfilter *wcf;
  CURLcode result;

  જો(a->cf)
    Curl_conn_cf_discard_chain(&a->cf, data);

  a->result = CURLE_OK;
  a->connected = FALSE;
  a->inconclusive = FALSE;
  a->cf = ખાલી;

  result = a->cf_create(&a->cf, data, a->origin, a->peer, a->transport_peer,
                        cf->conn, &a->addr,
                        a->tunnel_peer, a->tunnel_transport);
  જો(!result) {
    bool dummy;
    /* the new filter might have sub-filters */
    માટે(wcf = a->cf; wcf; wcf = wcf->next) {
      wcf->conn = cf->conn;
      wcf->sockindex = cf->sockindex;
    }
    a->result = cf_ip_attempt_connect(a, data, &dummy);
  }
  પાછા result;
}

સ્થિર શૂન્ય cf_ip_ballers_clear(સંરચના Curl_easy *data,
                                સંરચના cf_ip_ballers *bs)
{
  જ્યારે(bs->running) {
    સંરચના cf_ip_attempt *a = bs->running;
    bs->running = a->next;
    cf_ip_attempt_free(a, data);
  }
  bs->running_tail = &bs->running;
  cf_ip_attempt_free(bs->winner, data);
  bs->winner = ખાલી;
  Curl_peer_unlink(&bs->origin);
  Curl_peer_unlink(&bs->peer);
  Curl_peer_unlink(&bs->tunnel_peer);
}

સ્થિર CURLcode cf_ip_ballers_init(સંરચના cf_ip_ballers *bs,
                                   સંરચના Curl_easy *data,
                                   સંરચના Curl_peer *origin,
                                   સંરચના Curl_peer *peer,
                                   uint8_t transport_peer,
                                   સંરચના Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport,
                                   timediff_t attempt_delay_ms,
                                   uint32_t max_concurrent)
{
  memset(bs, 0, કદ(*bs));
  bs->running_tail = &bs->running;
  bs->cf_create = get_cf_create(transport_peer, !!tunnel_peer);
  જો(!bs->cf_create) {
    failf(data, "unsupported transport type %u%s",
          transport_peer, tunnel_peer ? " to proxy" : "");
    પાછા CURLE_UNSUPPORTED_PROTOCOL;
  }
  Curl_peer_link(&bs->origin, origin);
  Curl_peer_link(&bs->peer, peer);
  bs->transport_peer = transport_peer;
  Curl_peer_link(&bs->tunnel_peer, tunnel_peer);
  bs->tunnel_transport = tunnel_transport;
  bs->attempt_delay_ms = attempt_delay_ms;
  bs->max_concurrent = max_concurrent;
  bs->last_attempt_ai_family = AF_INET; /* so AF_INET6 is next */
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cf_ip_ballers_prune(સંરચના cf_ip_ballers *bs,
                                સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                uint32_t max_concurrent)
{
  સંરચના cf_ip_attempt *a = ખાલી, **panchor;
  uint32_t ongoing = 0;

  માટે(a = bs->running; a; a = a->next) {
    જો(!a->result && !a->connected)
      ++ongoing;
  }

  panchor = &bs->running;
  જ્યારે(*panchor && (ongoing > max_concurrent)) {
    a = *panchor;
    જો(!a->result && !a->connected) {
      *panchor = a->next;
      જો(!a->next)
        bs->running_tail = panchor;
      a->next = ખાલી;
      cf_ip_attempt_free(a, data);
      --ongoing;
      CURL_TRC_CF(data, cf, "discarding oldest attempt to keep limit");
    }
    નહીંતર {
      panchor = &a->next;
    }
  }
}

સ્થિર CURLcode cf_ip_ballers_run(સંરચના cf_ip_ballers *bs,
                                  સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  bool dns_resolved,
                                  bool *connected)
{
  CURLcode result = CURLE_OK;
  સંરચના cf_ip_attempt *a = ખાલી, **panchor;
  bool do_more;
  timediff_t next_expire_ms;
  uint32_t inconclusive, ongoing;
  અચલ સંરચના curltime *pnow = ખાલી;
  VERBOSE(પૂર્ણાંક i);

  જો(bs->winner)
    પાછા CURLE_OK;

evaluate:
  જો(Curl_timeleft_now_ms(data, Curl_pgrs_now(data)) < 0) {
    failf(data, "Connection timeout after %" FMT_OFF_T " ms",
          Curl_pgrs_since_ms(data, ખાલી, TIMER_STARTSINGLE));
    પાછા CURLE_OPERATION_TIMEDOUT;
  }

  ongoing = inconclusive = 0;

  /* check if a running baller connects now */
  VERBOSE(i = -1);
  માટે(panchor = &bs->running; *panchor;) {
    VERBOSE(++i);
    a = *panchor;
    a->result = cf_ip_attempt_connect(a, data, connected);
    જો(!a->result) {
      જો(*connected) {
        /* connected, declare the winner, remove from running,
         * clear remaining running list. */
        CURL_TRC_CF(data, cf, "connect attempt #%d successful", i);
        bs->winner = a;
        *panchor = a->next;
        a->next = ખાલી;
        જ્યારે(bs->running) {
          a = bs->running;
          bs->running = a->next;
          cf_ip_attempt_free(a, data);
        }
        bs->running_tail = &bs->running;
        પાછા CURLE_OK;
      }
      /* still running */
      ++ongoing;
      panchor = &(*panchor)->next;
    }
    નહીંતર જો(a->inconclusive) { /* failed, but inconclusive */
      ++inconclusive;
      panchor = &(*panchor)->next;
    }
    નહીંતર {
      /* hard failure, discard right away */
      *panchor = a->next;
      જો(!a->next)
        bs->running_tail = panchor;
      a->next = ખાલી;
      bs->last_dead_result = a->result;
      bs->had_dead = TRUE;
      cf_ip_attempt_free(a, data);
    }
  }
  જો(bs->running)
    CURL_TRC_CF(data, cf, "checked connect attempts: "
                "%u ongoing, %u inconclusive", ongoing, inconclusive);

  /* no attempt connected yet, start another one? */
  pnow = Curl_pgrs_now(data);
  જો(!ongoing) {
    do_more = TRUE;
  }
  નહીંતર {
    bool more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    જો(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    do_more = more_possible &&
      (curlx_ptimediff_ms(pnow, &bs->last_attempt_started) >=
       bs->attempt_delay_ms);
    જો(do_more)
      CURL_TRC_CF(data, cf, "happy eyeballs timeout expired, "
                  "start next attempt");
  }

  જો(do_more) {
    /* start the next attempt if there is another ip address to try.
     * Alternate between address families when possible. */
    અચલ સંરચના Curl_addrinfo *ai = ખાલી;
    પૂર્ણાંક ai_family = 0;
    CURL_TRC_CF(data, cf, "want to do more");
#ifdef USE_IPV6
    જો((bs->last_attempt_ai_family == AF_INET) ||
       !cf_ai_iter_has_more(&bs->addr_iter, data)) {
      ai = cf_ai_iter_next(&bs->ipv6_iter, data);
      ai_family = bs->ipv6_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next AAAA address: %s",
                  ai ? "found" : "none");
    }
#endif
    જો(!ai) {
      ai = cf_ai_iter_next(&bs->addr_iter, data);
      ai_family = bs->addr_iter.ai_family;
      CURL_TRC_CF(data, cf, "check for next A address: %s",
                  ai ? "found" : "none");
    }
    /* We are (re-)starting attempts. We are not interested in
     * keeping old failure information. The new attempt will either
     * succeed or persist new failure. */
    Curl_reset_fail(data);

    જો(ai) {  /* try another address */
      સંરચના Curl_sockaddr_ex addr;

      /* Discard oldest to make room for new attempt */
      જો(bs->max_concurrent)
        cf_ip_ballers_prune(bs, cf, data, bs->max_concurrent - 1);

      result = Curl_socket_addr_from_ai(&addr, ai, bs->transport_peer);
      જો(result)
        જાઓ out;

      result = cf_ip_attempt_new(&a, data, cf, bs->origin, bs->peer,
                                 bs->transport_peer, &addr, ai_family,
                                 bs->tunnel_peer, bs->tunnel_transport,
                                 bs->cf_create);
      CURL_TRC_CF(data, cf, "starting %s attempt for ipv%s -> %d",
                  bs->running ? "next" : "first",
                  (ai_family == AF_INET) ? "4" : "6", (પૂર્ણાંક)result);
      જો(result)
        જાઓ out;
      DEBUGASSERT(a);

      /* append to running list */
      *bs->running_tail = a;
      a->next = ખાલી;
      bs->running_tail = &a->next;
      bs->last_attempt_started = *pnow;
      bs->last_attempt_ai_family = ai_family;
      /* and run everything again */
      જાઓ evaluate;
    }
    નહીંતર જો(inconclusive) {
      /* tried all addresses, no success but some where inconclusive.
       * Let's restart the inconclusive ones. */
      timediff_t since_ms =
        curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      timediff_t delay_ms = bs->attempt_delay_ms - since_ms;
      જો(delay_ms <= 0) {
        CURL_TRC_CF(data, cf, "all attempts inconclusive, restarting one");
        VERBOSE(i = -1);
        માટે(a = bs->running; a; a = a->next) {
          VERBOSE(++i);
          જો(!a->inconclusive)
            ચાલુ;
          result = cf_ip_attempt_restart(a, cf, data);
          CURL_TRC_CF(data, cf, "restarted baller %d -> %d", i, (પૂર્ણાંક)result);
          જો(result) /* serious failure */
            જાઓ out;
          bs->last_attempt_started = *pnow;
          જાઓ evaluate;
        }
        DEBUGASSERT(0); /* should not come here */
      }
      નહીંતર {
        /* let's wait some more before restarting */
        infof(data, "connect attempts inconclusive, retrying "
                    "in %" FMT_TIMEDIFF_T "ms", delay_ms);
        Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, delay_ms, pnow);
      }
      /* attempt timeout for restart has not expired yet */
      જાઓ out;
    }
    નહીંતર જો(!ongoing && dns_resolved) {
      /* no more addresses, no inconclusive attempts */
      CURL_TRC_CF(data, cf, "no more attempts to try");
      DEBUGASSERT(!bs->running);
      result = bs->had_dead ? bs->last_dead_result : CURLE_COULDNT_CONNECT;
    }
  }

out:
  જો(!result) {
    bool more_possible;

    /* when do we need to be called again? */
    pnow = Curl_pgrs_now(data);
    next_expire_ms = Curl_timeleft_now_ms(data, pnow);
    જો(next_expire_ms < 0) {
      failf(data, "Connection timeout after %" FMT_OFF_T " ms",
            Curl_pgrs_since_ms(data, ખાલી, TIMER_STARTSINGLE));
      પાછા CURLE_OPERATION_TIMEDOUT;
    }

    more_possible = cf_ai_iter_has_more(&bs->addr_iter, data);
#ifdef USE_IPV6
    જો(!more_possible)
      more_possible = cf_ai_iter_has_more(&bs->ipv6_iter, data);
#endif
    જો(more_possible) {
      timediff_t expire_ms, elapsed_ms;

      elapsed_ms = curlx_ptimediff_ms(pnow, &bs->last_attempt_started);
      expire_ms = CURLMAX(bs->attempt_delay_ms - elapsed_ms, 0);
      next_expire_ms = CURLMIN(next_expire_ms, expire_ms);
      જો(next_expire_ms <= 0) {
        CURL_TRC_CF(data, cf, "HAPPY_EYEBALLS timeout due, re-evaluate");
        જાઓ evaluate;
      }
      CURL_TRC_CF(data, cf, "next HAPPY_EYEBALLS timeout in %" FMT_TIMEDIFF_T
                  "ms", next_expire_ms);
      Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, next_expire_ms, pnow);
    }
  }
  પાછા result;
}

સ્થિર CURLcode cf_ip_ballers_shutdown(સંરચના cf_ip_ballers *bs,
                                       સંરચના Curl_easy *data,
                                       bool *done)
{
  સંરચના cf_ip_attempt *a;

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  *done = TRUE;
  માટે(a = bs->running; a; a = a->next) {
    bool bdone = FALSE;
    જો(a->shutdown || !a->cf)
      ચાલુ;
    a->result = a->cf->cft->do_shutdown(a->cf, data, &bdone);
    જો(a->result || bdone)
      a->shutdown = TRUE; /* treat a failed shutdown as done */
    નહીંતર
      *done = FALSE;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_ip_ballers_pollset(સંરચના cf_ip_ballers *bs,
                                      સંરચના Curl_easy *data,
                                      સંરચના easy_pollset *ps)
{
  સંરચના cf_ip_attempt *a;
  CURLcode result = CURLE_OK;
  માટે(a = bs->running; a && !result; a = a->next) {
    જો(a->result)
      ચાલુ;
    result = Curl_conn_cf_adjust_pollset(a->cf, data, ps);
  }
  પાછા result;
}

સ્થિર bool cf_ip_ballers_pending(સંરચના cf_ip_ballers *bs,
                                  અચલ સંરચના Curl_easy *data)
{
  સંરચના cf_ip_attempt *a;

  માટે(a = bs->running; a; a = a->next) {
    જો(a->result)
      ચાલુ;
    જો(a->cf && a->cf->cft->has_data_pending(a->cf, data))
      પાછા TRUE;
  }
  પાછા FALSE;
}

સ્થિર પૂર્ણાંક cf_ip_ballers_min_reply_ms(સંરચના cf_ip_ballers *bs,
                                      સંરચના Curl_easy *data)
{
  પૂર્ણાંક reply_ms = -1, breply_ms;
  સંરચના cf_ip_attempt *a;

  માટે(a = bs->running; a; a = a->next) {
    જો(a->cf && !a->cf->cft->query(a->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                                   &breply_ms, ખાલી) &&
       breply_ms >= 0 && (reply_ms < 0 || breply_ms < reply_ms))
      reply_ms = breply_ms;
  }
  પાછા reply_ms;
}

પ્રકારવ્યાખ્યા ગણના {
  SCFST_INIT,
  SCFST_WAITING,
  SCFST_DONE
} cf_connect_state;

સંરચના cf_ip_happy_ctx {
  cf_ip_connect_create *cf_create;
  cf_connect_state state;
  સંરચના cf_ip_ballers ballers;
  BIT(dns_resolved);
};

સ્થિર CURLcode is_connected(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             bool *connected)
{
  સંરચના cf_ip_happy_ctx *ctx = cf->ctx;
  સંરચના connectdata *conn = cf->conn;
  CURLcode result;

  result = cf_ip_ballers_run(&ctx->ballers, cf, data,
                             (bool)ctx->dns_resolved, connected);

  જો(!result)
    પાછા CURLE_OK;
  નહીંતર {
    સંરચના Curl_peer *peer = ખાલી, *proxy_peer = ખાલી;
    અક્ષર viamsg[160];

    peer = Curl_conn_get_first_peer(conn, cf->sockindex);
    જો(!conn->origin || !peer)
      પાછા CURLE_FAILED_INIT;

#ifndef CURL_DISABLE_PROXY
    જો(conn->socks_proxy.peer)
      proxy_peer = conn->socks_proxy.peer;
    નહીંતર જો(conn->http_proxy.peer)
      proxy_peer = conn->http_proxy.peer;
#endif

    viamsg[0] = 0;
    જો(!Curl_peer_equal(peer, conn->origin) &&
       !Curl_peer_equal(peer, proxy_peer)) {
#ifdef USE_UNIX_SOCKETS
      જો(peer->unix_socket)
        curl_msnprintf(viamsg, કદ(viamsg), " over unix://%s",
                       peer->hostname);
      નહીંતર
#endif
      curl_msnprintf(viamsg, કદ(viamsg), " via %s:%u",
                     peer->hostname, peer->port);
    }

    failf(data, "Failed to connect to %s:%u%s %s%s%safter "
          "%" FMT_TIMEDIFF_T " ms: %s",
          conn->origin->hostname, conn->origin->port, viamsg,
          proxy_peer ? "over proxy " : "",
          proxy_peer ? proxy_peer->hostname : "",
          proxy_peer ? " " : "",
          Curl_pgrs_since_ms(data, ખાલી, TIMER_STARTSINGLE),
          curl_easy_strerror(result));

#ifdef SOCKETIMEDOUT
    જો(SOCKETIMEDOUT == data->state.os_errno)
      result = CURLE_OPERATION_TIMEDOUT;
#endif

    પાછા result;
  }
}

#define IP_HE_MAX_CONCURRENT_ATTEMPTS     6

સ્થિર CURLcode cf_ip_happy_init(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data)
{
  સંરચના cf_ip_happy_ctx *ctx = cf->ctx;

  જો(Curl_timeleft_ms(data) < 0) {
    /* a precaution, no need to continue if time already is up */
    failf(data, "Connection time-out");
    પાછા CURLE_OPERATION_TIMEDOUT;
  }

  જો(ctx->ballers.transport_peer == TRNSPRT_UNIX) {
#ifdef USE_UNIX_SOCKETS
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_UNIX);
#નહીંતર
    પાછા CURLE_UNSUPPORTED_PROTOCOL;
#endif
  }
  નહીંતર { /* TCP/UDP/QUIC */
#ifdef USE_IPV6
    cf_ai_iter_init(&ctx->ballers.ipv6_iter, cf, ctx->ballers.peer, AF_INET6);
#endif
    cf_ai_iter_init(&ctx->ballers.addr_iter, cf, ctx->ballers.peer, AF_INET);
  }

  CURL_TRC_CF(data, cf, "init ip ballers for transport %u",
              ctx->ballers.transport_peer);
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cf_ip_happy_ctx_clear(સંરચના cf_ip_happy_ctx *ctx,
                                  સંરચના Curl_easy *data)
{
  DEBUGASSERT(ctx);
  જો(ctx)
    cf_ip_ballers_clear(data, &ctx->ballers);
}

સ્થિર શૂન્ય cf_ip_happy_ctx_destroy(સંરચના cf_ip_happy_ctx *ctx,
                                    સંરચના Curl_easy *data)
{
  જો(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    curlx_free(ctx);
  }
}

સ્થિર CURLcode cf_ip_happy_shutdown(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data,
                                     bool *done)
{
  સંરચના cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  result = cf_ip_ballers_shutdown(&ctx->ballers, data, done);
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (પૂર્ણાંક)result, *done);
  પાછા result;
}

સ્થિર CURLcode cf_ip_happy_adjust_pollset(સંરચના Curl_cfilter *cf,
                                           સંરચના Curl_easy *data,
                                           સંરચના easy_pollset *ps)
{
  સંરચના cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  જો(!cf->connected) {
    result = cf_ip_ballers_pollset(&ctx->ballers, data, ps);
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (પૂર્ણાંક)result,
                ps->n);
  }
  પાછા result;
}

સ્થિર CURLcode cf_ip_happy_connect(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    bool *done)
{
  સંરચના cf_ip_happy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  /* -Werror=null-dereference finds false positives suddenly. */
  જો(!data)
    પાછા CURLE_FAILED_INIT;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  DEBUGASSERT(ctx);
  *done = FALSE;

  જો(!ctx->dns_resolved) {
    result = Curl_conn_dns_addr_result(cf->conn, cf->sockindex,
                                       ctx->ballers.peer);
    જો(!result)
      ctx->dns_resolved = TRUE;
    નહીંતર જો(result == CURLE_AGAIN) {
      result = CURLE_OK;
    }
    નહીંતર /* real error */
      જાઓ out;
  }

  પસંદગી(ctx->state) {
  સ્થિતિ SCFST_INIT:
    DEBUGASSERT(CURL_SOCKET_BAD == Curl_conn_cf_get_socket(cf, data));
    DEBUGASSERT(!cf->connected);
    result = cf_ip_happy_init(cf, data);
    જો(result)
      જાઓ out;
    ctx->state = SCFST_WAITING;
    FALLTHROUGH();
  સ્થિતિ SCFST_WAITING:
    result = is_connected(cf, data, done);
    જો(!result && *done) {
      DEBUGASSERT(ctx->ballers.winner);
      DEBUGASSERT(ctx->ballers.winner->cf);
      DEBUGASSERT(ctx->ballers.winner->cf->connected);
      /* we have a winner. Install and activate it.
       * close/free all others. */
      ctx->state = SCFST_DONE;
      cf->connected = TRUE;
      cf->next = ctx->ballers.winner->cf;
      ctx->ballers.winner->cf = ખાલી;

      જો(cf->conn->scheme->protocol & PROTO_FAMILY_SSH)
        Curl_pgrsTime(data, TIMER_APPCONNECT); /* we are connected already */
#ifdef CURLVERBOSE
      જો(Curl_trc_cf_is_verbose(cf, data)) {
        સંરચના ip_quadruple ipquad;
        bool is_ipv6;
        જો(!Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad)) {
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
    તોડો;
  સ્થિતિ SCFST_DONE:
    *done = TRUE;
    તોડો;
  }
out:
  પાછા result;
}

સ્થિર bool cf_ip_happy_data_pending(સંરચના Curl_cfilter *cf,
                                     અચલ સંરચના Curl_easy *data)
{
  સંરચના cf_ip_happy_ctx *ctx = cf->ctx;

  જો(!cf->connected) {
    પાછા cf_ip_ballers_pending(&ctx->ballers, data);
  }
  પાછા cf->next->cft->has_data_pending(cf->next, data);
}

સ્થિર CURLcode cf_ip_happy_query(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના cf_ip_happy_ctx *ctx = cf->ctx;

  જો(!cf->connected) {
    પસંદગી(query) {
    સ્થિતિ CF_QUERY_CONNECT_REPLY_MS: {
      *pres1 = cf_ip_ballers_min_reply_ms(&ctx->ballers, data);
      CURL_TRC_CF(data, cf, "query connect reply: %dms", *pres1);
      પાછા CURLE_OK;
    }
    મૂળભૂત:
      તોડો;
    }
  }

  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

સ્થિર શૂન્ય cf_ip_happy_destroy(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data)
{
  સંરચના cf_ip_happy_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  જો(ctx) {
    cf_ip_happy_ctx_clear(ctx, data);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
}

સંરચના Curl_cftype Curl_cft_ip_happy = {
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

સ્થિર CURLcode cf_ip_happy_create(સંરચના Curl_cfilter **pcf,
                                   સંરચના Curl_easy *data,
                                   સંરચના Curl_peer *origin,
                                   સંરચના Curl_peer *peer,
                                   uint8_t transport_peer,
                                   સંરચના connectdata *conn,
                                   સંરચના Curl_peer *tunnel_peer,
                                   uint8_t tunnel_transport)
{
  સંરચના cf_ip_happy_ctx *ctx = ખાલી;
  CURLcode result;

  (શૂન્ય)data;
  (શૂન્ય)conn;
  *pcf = ખાલી;
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  result = cf_ip_ballers_init(&ctx->ballers, data,
                              origin, peer, transport_peer,
                              tunnel_peer, tunnel_transport,
                              data->set.happy_eyeballs_timeout,
                              IP_HE_MAX_CONCURRENT_ATTEMPTS);
  જો(result)
    જાઓ out;

  result = Curl_cf_create(pcf, &Curl_cft_ip_happy, ctx);

out:
  જો(result) {
    curlx_safefree(*pcf);
    cf_ip_happy_ctx_destroy(ctx, data);
  }
  પાછા result;
}

CURLcode Curl_cf_ip_happy_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                       સંરચના Curl_easy *data,
                                       સંરચના Curl_peer *origin,
                                       સંરચના Curl_peer *peer,
                                       uint8_t transport_peer,
                                       સંરચના Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result;

  /* Need to be first */
  DEBUGASSERT(cf_at);
  result = cf_ip_happy_create(&cf, data, origin, peer, transport_peer,
                              cf_at->conn, tunnel_peer, tunnel_transport);
  જો(result)
    પાછા result;

  Curl_conn_cf_ઉમેરો_after(cf_at, cf);
  પાછા CURLE_OK;
}
