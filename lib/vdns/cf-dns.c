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

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "vdns/dnscache.h"
#அந்தர்பாவ "vdns/httpsrr.h"
#அந்தர்பாவ "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#define CURL_HE_AAAA_AWAIT_MS    25

கட்டமைப்பு cf_dns_ctx {
  கட்டமைப்பு Curl_dns_entry *dns;
  கட்டமைப்பு Curl_peer *peer;
  CURLcode resolv_result;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

நிலையான கட்டமைப்பு cf_dns_ctx *cf_dns_ctx_create(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  கட்டமைப்பு cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx)
    திரும்பு வெற்று;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    மாறிலி எழுத்து *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  திரும்பு ctx;
}

நிலையான வெறுமை cf_dns_ctx_destroy(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு cf_dns_ctx *ctx)
{
  எனில்(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
நிலையான வெறுமை cf_dns_report_addr(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு dynbuf *tmp,
                               மாறிலி எழுத்து *label,
                               முழு ai_family,
                               மாறிலி கட்டமைப்பு Curl_addrinfo *ai)
{
  எழுத்து buf[MAX_IPADR_LEN];
  மாறிலி எழுத்து *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  க்கு(; ai; ai = ai->ai_next) {
    எனில்(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, அளவு(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      எனில்(result) {
        infof(data, "too many IP, cannot show");
        திரும்பு;
      }
      sep = ", ";
    }
  }

  infof(data, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

நிலையான வெறுமை cf_dns_report(கட்டமைப்பு Curl_cfilter *cf,
                          கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு Curl_dns_entry *dns)
{
  கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;
  கட்டமைப்பு dynbuf tmp;

  எனில்(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    திரும்பு;

  எனில்(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#இல்லையெனில்
    DEBUGASSERT(0);
#endif
  }
  இல்லையெனில் {
    curlx_dyn_init(&tmp, 1024);
    எனில்(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    இல்லையெனில் எனில்(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      எனில்(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      இல்லையெனில் எனில்(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      இல்லையெனில் {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        எனில்(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        இல்லையெனில்
          infof(data, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#இல்லையெனில்
#define cf_dns_report(x, y, z) Curl_nop_stmt
#endif

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
நிலையான CURLcode cf_dns_start(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_dns_entry **pdns)
{
  கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(data);
  CURLcode result = CURLE_OK;

  *pdns = வெற்று;

  CURL_TRC_CF(data, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->port);
  எனில்(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  எனில்(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    எனில்(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    இல்லையெனில் எனில்(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  இல்லையெனில் எனில்(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  இல்லையெனில் {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    திரும்பு CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  எனில்(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    திரும்பு CURLE_OK;
  }
  இல்லையெனில் எனில்(result == CURLE_AGAIN) { /* async resolv in progress */
    திரும்பு CURLE_OK;
  }
  இல்லையெனில் எனில்(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, வெற்று, TIMER_STARTSINGLE));
    திரும்பு CURLE_OPERATION_TIMEDOUT;
  }
  இல்லையெனில் {
    DEBUGASSERT(result);
    எனில்(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    திரும்பு result;
  }
}

நிலையான bool cf_dns_ready_to_connect(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;

  எனில்(ctx->resolv_result)
    திரும்பு TRUE;
  இல்லையெனில் எனில்(ctx->dns)
    திரும்பு TRUE;
#ifdef USE_CURL_ASYNC
  இல்லையெனில் எனில்(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    மாறிலி கட்டமைப்பு curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    எனில்(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      திரும்பு TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    எனில்(remain_ms <= 0)
      திரும்பு TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    திரும்பு FALSE;
  }
  இல்லையெனில் {
    திரும்பு TRUE;
  }
#இல்லையெனில்
  (வெறுமை)data;
  DEBUGASSERT(0); /* We should not come here */
  திரும்பு FALSE;
#endif /* USE_CURL_ASYNC */
}

நிலையான CURLcode cf_dns_connect(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               bool *done)
{
  கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  *done = FALSE;
  எனில்(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  எனில்(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  எனில்(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (முழு)ctx->resolv_result);
    திரும்பு ctx->resolv_result;
  }

  எனில்(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    எனில்((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  எனில்(!cf_dns_ready_to_connect(cf, data)) {
    திரும்பு CURLE_OK;
  }

  எனில்(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    எனில்(result || !sub_done)
      திரும்பு result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cf_dns_destroy(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_dns_ctx_destroy(data, ctx);
}

நிலையான CURLcode cf_dns_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு easy_pollset *ps)
{
#ifdef USE_CURL_ASYNC
  எனில்(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;
    எனில்(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      எனில்(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    திரும்பு Curl_resolv_pollset(data, ps);
  }
#இல்லையெனில்
  (வெறுமை)cf;
  (வெறுமை)data;
  (வெறுமை)ps;
#endif
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_dns_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             முழு event, முழு arg1, வெறுமை *arg2)
{
  கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (வெறுமை)arg1;
  (வெறுமை)arg2;
  தேர்வு(event) {
  நிலை CF_CTRL_DATA_DONE:
    எனில்(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு result;
}

கட்டமைப்பு Curl_cftype Curl_cft_dns = {
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

நிலையான CURLcode cf_dns_create(கட்டமைப்பு Curl_cfilter **pcf,
                              கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு cf_dns_ctx *ctx;
  CURLcode result = CURLE_OK;

  (வெறுமை)data;
  ctx = cf_dns_ctx_create(data, peer, dns_queries, transport, for_proxy);
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? வெற்று : cf;
  எனில்(result)
    cf_dns_ctx_destroy(data, ctx);
  திரும்பு result;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
நிலையான CURLcode cf_dns_add(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு connectdata *conn,
                           int8_t sockindex,
                           கட்டமைப்பு Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  bool for_proxy = FALSE;
  CURLcode result;

  எனில்(!peer)
    திரும்பு CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  எனில்(result)
    செல் out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  திரும்பு result;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
நிலையான CURLcode cf_dns_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result;

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, FALSE);
  எனில்(result)
    திரும்பு result;

  Curl_conn_cf_செருகு_after(cf_at, cf);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_dns_add_resolve(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு connectdata *conn,
                                   int8_t sockindex,
                                   கட்டமைப்பு Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  கட்டமைப்பு Curl_cfilter *cf = data->conn->cfilter[sockindex];
  கட்டமைப்பு Curl_cfilter *cf_dns = வெற்று;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  எனில்((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  க்கு(; cf && dns_queries; cf = cf->next) {
    எனில்(cf->cft == &Curl_cft_dns) {
      கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      எனில்(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        எனில்(!dns_queries) { /* already there */
          திரும்பு CURLE_OK;
        }
        இல்லையெனில் எனில்(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          திரும்பு CURLE_OK;
        }
      }
    }
  }

  எனில்(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    எனில்(cf_dns)
      திரும்பு cf_dns_செருகு_after(cf_dns, data, peer, dns_queries, transport);
    இல்லையெனில்
      திரும்பு cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு connectdata *conn,
                                        int8_t sockindex,
                                        கட்டமைப்பு Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  திரும்பு cf_dns_add_resolve(data, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_result(கட்டமைப்பு connectdata *conn,
                                   int8_t sockindex,
                                   கட்டமைப்பு Curl_peer *peer)
{
  கட்டமைப்பு Curl_cfilter *cf = conn->cfilter[sockindex];
  க்கு(; cf; cf = cf->next) {
    எனில்(cf->cft == &Curl_cft_dns) {
      கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;
      எனில்(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        எனில்(ctx->dns || ctx->resolv_result)
          திரும்பு ctx->resolv_result;
        திரும்பு CURLE_AGAIN;
      }
    }
  }
  திரும்பு CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
மாறிலி கட்டமைப்பு Curl_addrinfo *Curl_conn_dns_get_ai(கட்டமைப்பு Curl_easy *data,
                                                 கட்டமைப்பு Curl_peer *peer,
                                                 int8_t sockindex,
                                                 முழு ai_family,
                                                 unsigned முழு index)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு Curl_cfilter *cf = conn->cfilter[sockindex];

  க்கு(; cf; cf = cf->next) {
    எனில்(cf->cft == &Curl_cft_dns) {
      கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;
      எனில்(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        எனில்(ctx->resolv_result)
          திரும்பு வெற்று;
        இல்லையெனில் எனில்(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          எனில்((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            திரும்பு வெற்று;
#ifdef USE_IPV6
          எனில்((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            திரும்பு வெற்று;
#endif
          திரும்பு Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        இல்லையெனில்
          திரும்பு Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  திரும்பு வெற்று;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு connectdata *conn,
                                         int8_t sockindex,
                                         கட்டமைப்பு Curl_peer *peer)
{
  திரும்பு cf_dns_add_resolve(data, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
மாறிலி கட்டமைப்பு Curl_https_rrinfo *Curl_conn_dns_get_https(கட்டமைப்பு Curl_easy *data,
                                                        int8_t sockindex,
                                                        கட்டமைப்பு Curl_peer *peer)
{
  கட்டமைப்பு Curl_cfilter *cf = data->conn->cfilter[sockindex];
  க்கு(; cf; cf = cf->next) {
    எனில்(cf->cft == &Curl_cft_dns) {
      கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;
      எனில்(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        எனில்(ctx->dns)
          திரும்பு ctx->dns->hinfo;
        இல்லையெனில்
          திரும்பு Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  திரும்பு வெற்று;
}

bool Curl_conn_dns_resolved_https(கட்டமைப்பு Curl_easy *data,
                                  int8_t sockindex,
                                  கட்டமைப்பு Curl_peer *peer)
{
  கட்டமைப்பு Curl_cfilter *cf = data->conn->cfilter[sockindex];
  க்கு(; cf; cf = cf->next) {
    எனில்(cf->cft == &Curl_cft_dns) {
      கட்டமைப்பு cf_dns_ctx *ctx = cf->ctx;
      எனில்(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        எனில்(ctx->dns)
          திரும்பு TRUE;
        இல்லையெனில்
          திரும்பு Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  திரும்பு TRUE;
}

#endif /* USE_HTTPSRR */
