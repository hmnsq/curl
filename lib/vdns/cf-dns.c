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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "vdns/dnscache.h"
#അന്തര്ഭാവ "vdns/httpsrr.h"
#അന്തര്ഭാവ "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#define CURL_HE_AAAA_AWAIT_MS    25

ഘടന cf_dns_ctx {
  ഘടന Curl_dns_entry *dns;
  ഘടന Curl_peer *peer;
  CURLcode resolv_result;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

സ്ഥിതം ഘടന cf_dns_ctx *cf_dns_ctx_create(ഘടന Curl_easy *data,
                                            ഘടന Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  ഘടന cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx)
    തിരികെ ശൂന്യ;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    സ്ഥിരം അക്ഷരം *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    എങ്കിൽ(p) {
      curl_off_t l;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  തിരികെ ctx;
}

സ്ഥിതം ശൂന്യം cf_dns_ctx_destroy(ഘടന Curl_easy *data,
                               ഘടന cf_dns_ctx *ctx)
{
  എങ്കിൽ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
സ്ഥിതം ശൂന്യം cf_dns_report_addr(ഘടന Curl_easy *data,
                               ഘടന dynbuf *tmp,
                               സ്ഥിരം അക്ഷരം *label,
                               പൂർണ്ണസംഖ്യ ai_family,
                               സ്ഥിരം ഘടന Curl_addrinfo *ai)
{
  അക്ഷരം buf[MAX_IPADR_LEN];
  സ്ഥിരം അക്ഷരം *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  വേണ്ടി(; ai; ai = ai->ai_next) {
    എങ്കിൽ(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, വലുപ്പം(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      എങ്കിൽ(result) {
        infof(data, "too many IP, cannot show");
        തിരികെ;
      }
      sep = ", ";
    }
  }

  infof(data, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

സ്ഥിതം ശൂന്യം cf_dns_report(ഘടന Curl_cfilter *cf,
                          ഘടന Curl_easy *data,
                          ഘടന Curl_dns_entry *dns)
{
  ഘടന cf_dns_ctx *ctx = cf->ctx;
  ഘടന dynbuf tmp;

  എങ്കിൽ(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    തിരികെ;

  എങ്കിൽ(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#അല്ലെങ്കിൽ
    DEBUGASSERT(0);
#endif
  }
  അല്ലെങ്കിൽ {
    curlx_dyn_init(&tmp, 1024);
    എങ്കിൽ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    അല്ലെങ്കിൽ എങ്കിൽ(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      എങ്കിൽ(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      അല്ലെങ്കിൽ എങ്കിൽ(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      അല്ലെങ്കിൽ {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        എങ്കിൽ(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        അല്ലെങ്കിൽ
          infof(data, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#അല്ലെങ്കിൽ
#define cf_dns_report(x, y, z) Curl_nop_stmt
#endif

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
സ്ഥിതം CURLcode cf_dns_start(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             ഘടന Curl_dns_entry **pdns)
{
  ഘടന cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(data);
  CURLcode result = CURLE_OK;

  *pdns = ശൂന്യ;

  CURL_TRC_CF(data, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->port);
  എങ്കിൽ(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  എങ്കിൽ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    എങ്കിൽ(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    അല്ലെങ്കിൽ എങ്കിൽ(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  അല്ലെങ്കിൽ എങ്കിൽ(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  അല്ലെങ്കിൽ {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    തിരികെ CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  എങ്കിൽ(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    തിരികെ CURLE_OK;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(result == CURLE_AGAIN) { /* async resolv in progress */
    തിരികെ CURLE_OK;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, ശൂന്യ, TIMER_STARTSINGLE));
    തിരികെ CURLE_OPERATION_TIMEDOUT;
  }
  അല്ലെങ്കിൽ {
    DEBUGASSERT(result);
    എങ്കിൽ(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    തിരികെ result;
  }
}

സ്ഥിതം bool cf_dns_ready_to_connect(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data)
{
  ഘടന cf_dns_ctx *ctx = cf->ctx;

  എങ്കിൽ(ctx->resolv_result)
    തിരികെ TRUE;
  അല്ലെങ്കിൽ എങ്കിൽ(ctx->dns)
    തിരികെ TRUE;
#ifdef USE_CURL_ASYNC
  അല്ലെങ്കിൽ എങ്കിൽ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    സ്ഥിരം ഘടന curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    എങ്കിൽ(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      തിരികെ TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    എങ്കിൽ(remain_ms <= 0)
      തിരികെ TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    തിരികെ FALSE;
  }
  അല്ലെങ്കിൽ {
    തിരികെ TRUE;
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  DEBUGASSERT(0); /* We should not come here */
  തിരികെ FALSE;
#endif /* USE_CURL_ASYNC */
}

സ്ഥിതം CURLcode cf_dns_connect(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               bool *done)
{
  ഘടന cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  *done = FALSE;
  എങ്കിൽ(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  എങ്കിൽ(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  എങ്കിൽ(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (പൂർണ്ണസംഖ്യ)ctx->resolv_result);
    തിരികെ ctx->resolv_result;
  }

  എങ്കിൽ(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    എങ്കിൽ((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  എങ്കിൽ(!cf_dns_ready_to_connect(cf, data)) {
    തിരികെ CURLE_OK;
  }

  എങ്കിൽ(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    എങ്കിൽ(result || !sub_done)
      തിരികെ result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cf_dns_destroy(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_dns_ctx_destroy(data, ctx);
}

സ്ഥിതം CURLcode cf_dns_adjust_pollset(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      ഘടന easy_pollset *ps)
{
#ifdef USE_CURL_ASYNC
  എങ്കിൽ(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    ഘടന cf_dns_ctx *ctx = cf->ctx;
    എങ്കിൽ(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      എങ്കിൽ(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    തിരികെ Curl_resolv_pollset(data, ps);
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)cf;
  (ശൂന്യം)data;
  (ശൂന്യം)ps;
#endif
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_dns_cntrl(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  ഘടന cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ശൂന്യം)arg1;
  (ശൂന്യം)arg2;
  തിരഞ്ഞെടുപ്പ്(event) {
  അവസ്ഥ CF_CTRL_DATA_DONE:
    എങ്കിൽ(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ result;
}

ഘടന Curl_cftype Curl_cft_dns = {
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

സ്ഥിതം CURLcode cf_dns_create(ഘടന Curl_cfilter **pcf,
                              ഘടന Curl_easy *data,
                              ഘടന Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന cf_dns_ctx *ctx;
  CURLcode result = CURLE_OK;

  (ശൂന്യം)data;
  ctx = cf_dns_ctx_create(data, peer, dns_queries, transport, for_proxy);
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? ശൂന്യ : cf;
  എങ്കിൽ(result)
    cf_dns_ctx_destroy(data, ctx);
  തിരികെ result;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
സ്ഥിതം CURLcode cf_dns_add(ഘടന Curl_easy *data,
                           ഘടന connectdata *conn,
                           int8_t sockindex,
                           ഘടന Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  bool for_proxy = FALSE;
  CURLcode result;

  എങ്കിൽ(!peer)
    തിരികെ CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  എങ്കിൽ(result)
    പോകുക out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  തിരികെ result;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
സ്ഥിതം CURLcode cf_dns_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                    ഘടന Curl_easy *data,
                                    ഘടന Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result;

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, FALSE);
  എങ്കിൽ(result)
    തിരികെ result;

  Curl_conn_cf_തിരുകുക_after(cf_at, cf);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_dns_add_resolve(ഘടന Curl_easy *data,
                                   ഘടന connectdata *conn,
                                   int8_t sockindex,
                                   ഘടന Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  ഘടന Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ഘടന Curl_cfilter *cf_dns = ശൂന്യ;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  എങ്കിൽ((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  വേണ്ടി(; cf && dns_queries; cf = cf->next) {
    എങ്കിൽ(cf->cft == &Curl_cft_dns) {
      ഘടന cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      എങ്കിൽ(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        എങ്കിൽ(!dns_queries) { /* already there */
          തിരികെ CURLE_OK;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          തിരികെ CURLE_OK;
        }
      }
    }
  }

  എങ്കിൽ(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    എങ്കിൽ(cf_dns)
      തിരികെ cf_dns_തിരുകുക_after(cf_dns, data, peer, dns_queries, transport);
    അല്ലെങ്കിൽ
      തിരികെ cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(ഘടന Curl_easy *data,
                                        ഘടന connectdata *conn,
                                        int8_t sockindex,
                                        ഘടന Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  തിരികെ cf_dns_add_resolve(data, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_result(ഘടന connectdata *conn,
                                   int8_t sockindex,
                                   ഘടന Curl_peer *peer)
{
  ഘടന Curl_cfilter *cf = conn->cfilter[sockindex];
  വേണ്ടി(; cf; cf = cf->next) {
    എങ്കിൽ(cf->cft == &Curl_cft_dns) {
      ഘടന cf_dns_ctx *ctx = cf->ctx;
      എങ്കിൽ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        എങ്കിൽ(ctx->dns || ctx->resolv_result)
          തിരികെ ctx->resolv_result;
        തിരികെ CURLE_AGAIN;
      }
    }
  }
  തിരികെ CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
സ്ഥിരം ഘടന Curl_addrinfo *Curl_conn_dns_get_ai(ഘടന Curl_easy *data,
                                                 ഘടന Curl_peer *peer,
                                                 int8_t sockindex,
                                                 പൂർണ്ണസംഖ്യ ai_family,
                                                 unsigned പൂർണ്ണസംഖ്യ index)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന Curl_cfilter *cf = conn->cfilter[sockindex];

  വേണ്ടി(; cf; cf = cf->next) {
    എങ്കിൽ(cf->cft == &Curl_cft_dns) {
      ഘടന cf_dns_ctx *ctx = cf->ctx;
      എങ്കിൽ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        എങ്കിൽ(ctx->resolv_result)
          തിരികെ ശൂന്യ;
        അല്ലെങ്കിൽ എങ്കിൽ(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          എങ്കിൽ((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            തിരികെ ശൂന്യ;
#ifdef USE_IPV6
          എങ്കിൽ((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            തിരികെ ശൂന്യ;
#endif
          തിരികെ Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        അല്ലെങ്കിൽ
          തിരികെ Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  തിരികെ ശൂന്യ;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(ഘടന Curl_easy *data,
                                         ഘടന connectdata *conn,
                                         int8_t sockindex,
                                         ഘടന Curl_peer *peer)
{
  തിരികെ cf_dns_add_resolve(data, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
സ്ഥിരം ഘടന Curl_https_rrinfo *Curl_conn_dns_get_https(ഘടന Curl_easy *data,
                                                        int8_t sockindex,
                                                        ഘടന Curl_peer *peer)
{
  ഘടന Curl_cfilter *cf = data->conn->cfilter[sockindex];
  വേണ്ടി(; cf; cf = cf->next) {
    എങ്കിൽ(cf->cft == &Curl_cft_dns) {
      ഘടന cf_dns_ctx *ctx = cf->ctx;
      എങ്കിൽ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        എങ്കിൽ(ctx->dns)
          തിരികെ ctx->dns->hinfo;
        അല്ലെങ്കിൽ
          തിരികെ Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  തിരികെ ശൂന്യ;
}

bool Curl_conn_dns_resolved_https(ഘടന Curl_easy *data,
                                  int8_t sockindex,
                                  ഘടന Curl_peer *peer)
{
  ഘടന Curl_cfilter *cf = data->conn->cfilter[sockindex];
  വേണ്ടി(; cf; cf = cf->next) {
    എങ്കിൽ(cf->cft == &Curl_cft_dns) {
      ഘടന cf_dns_ctx *ctx = cf->ctx;
      എങ്കിൽ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        എങ്കിൽ(ctx->dns)
          തിരികെ TRUE;
        അല്ലെങ്കിൽ
          തിരികെ Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  തിരികെ TRUE;
}

#endif /* USE_HTTPSRR */
