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

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "vdns/dnscache.h"
#અન્તર્ભાવ "vdns/httpsrr.h"
#અન્તર્ભાવ "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#define CURL_HE_AAAA_AWAIT_MS    25

સંરચના cf_dns_ctx {
  સંરચના Curl_dns_entry *dns;
  સંરચના Curl_peer *peer;
  CURLcode resolv_result;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

સ્થિર સંરચના cf_dns_ctx *cf_dns_ctx_create(સંરચના Curl_easy *data,
                                            સંરચના Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  સંરચના cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx)
    પાછા ખાલી;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    અચલ અક્ષર *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    જો(p) {
      curl_off_t l;
      જો(!curlx_str_સંખ્યા(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  પાછા ctx;
}

સ્થિર શૂન્ય cf_dns_ctx_destroy(સંરચના Curl_easy *data,
                               સંરચના cf_dns_ctx *ctx)
{
  જો(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
સ્થિર શૂન્ય cf_dns_report_addr(સંરચના Curl_easy *data,
                               સંરચના dynbuf *tmp,
                               અચલ અક્ષર *label,
                               પૂર્ણાંક ai_family,
                               અચલ સંરચના Curl_addrinfo *ai)
{
  અક્ષર buf[MAX_IPADR_LEN];
  અચલ અક્ષર *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  માટે(; ai; ai = ai->ai_next) {
    જો(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, કદ(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      જો(result) {
        infof(data, "too many IP, cannot show");
        પાછા;
      }
      sep = ", ";
    }
  }

  infof(data, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

સ્થિર શૂન્ય cf_dns_report(સંરચના Curl_cfilter *cf,
                          સંરચના Curl_easy *data,
                          સંરચના Curl_dns_entry *dns)
{
  સંરચના cf_dns_ctx *ctx = cf->ctx;
  સંરચના dynbuf tmp;

  જો(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    પાછા;

  જો(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#નહીંતર
    DEBUGASSERT(0);
#endif
  }
  નહીંતર {
    curlx_dyn_init(&tmp, 1024);
    જો(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    નહીંતર જો(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      જો(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      નહીંતર જો(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      નહીંતર {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        જો(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        નહીંતર
          infof(data, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#નહીંતર
#define cf_dns_report(x, y, z) Curl_nop_stmt
#endif

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
સ્થિર CURLcode cf_dns_start(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             સંરચના Curl_dns_entry **pdns)
{
  સંરચના cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(data);
  CURLcode result = CURLE_OK;

  *pdns = ખાલી;

  CURL_TRC_CF(data, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->port);
  જો(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  જો(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    જો(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    નહીંતર જો(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  નહીંતર જો(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  નહીંતર {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    પાછા CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  જો(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    પાછા CURLE_OK;
  }
  નહીંતર જો(result == CURLE_AGAIN) { /* async resolv in progress */
    પાછા CURLE_OK;
  }
  નહીંતર જો(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, ખાલી, TIMER_STARTSINGLE));
    પાછા CURLE_OPERATION_TIMEDOUT;
  }
  નહીંતર {
    DEBUGASSERT(result);
    જો(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    પાછા result;
  }
}

સ્થિર bool cf_dns_ready_to_connect(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data)
{
  સંરચના cf_dns_ctx *ctx = cf->ctx;

  જો(ctx->resolv_result)
    પાછા TRUE;
  નહીંતર જો(ctx->dns)
    પાછા TRUE;
#ifdef USE_CURL_ASYNC
  નહીંતર જો(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    અચલ સંરચના curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    જો(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      પાછા TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    જો(remain_ms <= 0)
      પાછા TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    પાછા FALSE;
  }
  નહીંતર {
    પાછા TRUE;
  }
#નહીંતર
  (શૂન્ય)data;
  DEBUGASSERT(0); /* We should not come here */
  પાછા FALSE;
#endif /* USE_CURL_ASYNC */
}

સ્થિર CURLcode cf_dns_connect(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               bool *done)
{
  સંરચના cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  *done = FALSE;
  જો(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  જો(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  જો(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (પૂર્ણાંક)ctx->resolv_result);
    પાછા ctx->resolv_result;
  }

  જો(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    જો((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  જો(!cf_dns_ready_to_connect(cf, data)) {
    પાછા CURLE_OK;
  }

  જો(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    જો(result || !sub_done)
      પાછા result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cf_dns_destroy(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_dns_ctx_destroy(data, ctx);
}

સ્થિર CURLcode cf_dns_adjust_pollset(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      સંરચના easy_pollset *ps)
{
#ifdef USE_CURL_ASYNC
  જો(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    સંરચના cf_dns_ctx *ctx = cf->ctx;
    જો(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      જો(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    પાછા Curl_resolv_pollset(data, ps);
  }
#નહીંતર
  (શૂન્ય)cf;
  (શૂન્ય)data;
  (શૂન્ય)ps;
#endif
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_dns_cntrl(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  સંરચના cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (શૂન્ય)arg1;
  (શૂન્ય)arg2;
  પસંદગી(event) {
  સ્થિતિ CF_CTRL_DATA_DONE:
    જો(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા result;
}

સંરચના Curl_cftype Curl_cft_dns = {
  "DNS",
  CF_TYPE_SETUP | CF_TYPE_DNS,
  CURL_LOG_LVL_NONE,
  cf_dns_destroy,
  cf_dns_connect,
  Curl_cf_def_shutdown,
  cf_dns_adjust_pollset,
  Curl_cf_def_data_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_dns_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

સ્થિર CURLcode cf_dns_create(સંરચના Curl_cfilter **pcf,
                              સંરચના Curl_easy *data,
                              સંરચના Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના cf_dns_ctx *ctx;
  CURLcode result = CURLE_OK;

  (શૂન્ય)data;
  ctx = cf_dns_ctx_create(data, peer, dns_queries, transport, for_proxy);
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? ખાલી : cf;
  જો(result)
    cf_dns_ctx_destroy(data, ctx);
  પાછા result;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
સ્થિર CURLcode cf_dns_add(સંરચના Curl_easy *data,
                           સંરચના connectdata *conn,
                           int8_t sockindex,
                           સંરચના Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  bool for_proxy = FALSE;
  CURLcode result;

  જો(!peer)
    પાછા CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  જો(result)
    જાઓ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  પાછા result;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
સ્થિર CURLcode cf_dns_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                    સંરચના Curl_easy *data,
                                    સંરચના Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result;

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, FALSE);
  જો(result)
    પાછા result;

  Curl_conn_cf_ઉમેરો_after(cf_at, cf);
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_dns_add_resolve(સંરચના Curl_easy *data,
                                   સંરચના connectdata *conn,
                                   int8_t sockindex,
                                   સંરચના Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  સંરચના Curl_cfilter *cf = data->conn->cfilter[sockindex];
  સંરચના Curl_cfilter *cf_dns = ખાલી;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  જો((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  માટે(; cf && dns_queries; cf = cf->next) {
    જો(cf->cft == &Curl_cft_dns) {
      સંરચના cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      જો(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        જો(!dns_queries) { /* already there */
          પાછા CURLE_OK;
        }
        નહીંતર જો(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          પાછા CURLE_OK;
        }
      }
    }
  }

  જો(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    જો(cf_dns)
      પાછા cf_dns_ઉમેરો_after(cf_dns, data, peer, dns_queries, transport);
    નહીંતર
      પાછા cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  પાછા CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(સંરચના Curl_easy *data,
                                        સંરચના connectdata *conn,
                                        int8_t sockindex,
                                        સંરચના Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  પાછા cf_dns_add_resolve(data, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_result(સંરચના connectdata *conn,
                                   int8_t sockindex,
                                   સંરચના Curl_peer *peer)
{
  સંરચના Curl_cfilter *cf = conn->cfilter[sockindex];
  માટે(; cf; cf = cf->next) {
    જો(cf->cft == &Curl_cft_dns) {
      સંરચના cf_dns_ctx *ctx = cf->ctx;
      જો(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        જો(ctx->dns || ctx->resolv_result)
          પાછા ctx->resolv_result;
        પાછા CURLE_AGAIN;
      }
    }
  }
  પાછા CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
અચલ સંરચના Curl_addrinfo *Curl_conn_dns_get_ai(સંરચના Curl_easy *data,
                                                 સંરચના Curl_peer *peer,
                                                 int8_t sockindex,
                                                 પૂર્ણાંક ai_family,
                                                 unsigned પૂર્ણાંક index)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના Curl_cfilter *cf = conn->cfilter[sockindex];

  માટે(; cf; cf = cf->next) {
    જો(cf->cft == &Curl_cft_dns) {
      સંરચના cf_dns_ctx *ctx = cf->ctx;
      જો(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        જો(ctx->resolv_result)
          પાછા ખાલી;
        નહીંતર જો(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          જો((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            પાછા ખાલી;
#ifdef USE_IPV6
          જો((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            પાછા ખાલી;
#endif
          પાછા Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        નહીંતર
          પાછા Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  પાછા ખાલી;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(સંરચના Curl_easy *data,
                                         સંરચના connectdata *conn,
                                         int8_t sockindex,
                                         સંરચના Curl_peer *peer)
{
  પાછા cf_dns_add_resolve(data, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
અચલ સંરચના Curl_https_rrinfo *Curl_conn_dns_get_https(સંરચના Curl_easy *data,
                                                        int8_t sockindex,
                                                        સંરચના Curl_peer *peer)
{
  સંરચના Curl_cfilter *cf = data->conn->cfilter[sockindex];
  માટે(; cf; cf = cf->next) {
    જો(cf->cft == &Curl_cft_dns) {
      સંરચના cf_dns_ctx *ctx = cf->ctx;
      જો(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        જો(ctx->dns)
          પાછા ctx->dns->hinfo;
        નહીંતર
          પાછા Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  પાછા ખાલી;
}

bool Curl_conn_dns_resolved_https(સંરચના Curl_easy *data,
                                  int8_t sockindex,
                                  સંરચના Curl_peer *peer)
{
  સંરચના Curl_cfilter *cf = data->conn->cfilter[sockindex];
  માટે(; cf; cf = cf->next) {
    જો(cf->cft == &Curl_cft_dns) {
      સંરચના cf_dns_ctx *ctx = cf->ctx;
      જો(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        જો(ctx->dns)
          પાછા TRUE;
        નહીંતર
          પાછા Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  પાછા TRUE;
}

#endif /* USE_HTTPSRR */
