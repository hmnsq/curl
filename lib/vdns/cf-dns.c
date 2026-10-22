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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "curl_addrinfo.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "curl_trc.h"
#ضمن "multiif.h"
#ضمن "progress.h"
#ضمن "url.h"
#ضمن "vdns/cf-dns.h"
#ضمن "vdns/dnscache.h"
#ضمن "vdns/httpsrr.h"
#ضمن "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#define CURL_HE_AAAA_AWAIT_MS    25

بنية cf_dns_ctx {
  بنية Curl_dns_entry *dns;
  بنية Curl_peer *peer;
  CURLcode resolv_result;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

ساكن بنية cf_dns_ctx *cf_dns_ctx_create(بنية Curl_easy *data,
                                            بنية Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  بنية cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, حجم(*ctx));
  𐤀𐤌(!ctx)
    أرجع عدم;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    𐤀𐤌(p) {
      curl_off_t l;
      𐤀𐤌(!curlx_str_number(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  أرجع ctx;
}

ساكن فراغ cf_dns_ctx_destroy(بنية Curl_easy *data,
                               بنية cf_dns_ctx *ctx)
{
  𐤀𐤌(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
ساكن فراغ cf_dns_report_addr(بنية Curl_easy *data,
                               بنية dynbuf *tmp,
                               ثابت حرف *label,
                               صحيح ai_family,
                               ثابت بنية Curl_addrinfo *ai)
{
  حرف buf[MAX_IPADR_LEN];
  ثابت حرف *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  لكل(; ai; ai = ai->ai_next) {
    𐤀𐤌(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, حجم(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      𐤀𐤌(result) {
        infof(data, "too many IP, cannot show");
        أرجع;
      }
      sep = ", ";
    }
  }

  infof(data, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

ساكن فراغ cf_dns_report(بنية Curl_cfilter *cf,
                          بنية Curl_easy *data,
                          بنية Curl_dns_entry *dns)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  بنية dynbuf tmp;

  𐤀𐤌(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    أرجع;

  𐤀𐤌(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#وإلا
    DEBUGASSERT(0);
#endif
  }
  وإلا {
    curlx_dyn_init(&tmp, 1024);
    𐤀𐤌(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    وإلا 𐤀𐤌(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      𐤀𐤌(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      وإلا 𐤀𐤌(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      وإلا {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        𐤀𐤌(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        وإلا
          infof(data, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#وإلا
#define cf_dns_report(x, y, z) Curl_nop_stmt
#endif

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
ساكن CURLcode cf_dns_start(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data,
                             بنية Curl_dns_entry **pdns)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(data);
  CURLcode result = CURLE_OK;

  *pdns = عدم;

  CURL_TRC_CF(data, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->port);
  𐤀𐤌(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  𐤀𐤌(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    𐤀𐤌(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    وإلا 𐤀𐤌(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  وإلا 𐤀𐤌(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  وإلا {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    أرجع CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  𐤀𐤌(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    أرجع CURLE_OK;
  }
  وإلا 𐤀𐤌(result == CURLE_AGAIN) { /* async resolv in progress */
    أرجع CURLE_OK;
  }
  وإلا 𐤀𐤌(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, عدم, TIMER_STARTSINGLE));
    أرجع CURLE_OPERATION_TIMEDOUT;
  }
  وإلا {
    DEBUGASSERT(result);
    𐤀𐤌(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    أرجع result;
  }
}

ساكن bool cf_dns_ready_to_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data)
{
  بنية cf_dns_ctx *ctx = cf->ctx;

  𐤀𐤌(ctx->resolv_result)
    أرجع TRUE;
  وإلا 𐤀𐤌(ctx->dns)
    أرجع TRUE;
#ifdef USE_CURL_ASYNC
  وإلا 𐤀𐤌(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ثابت بنية curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    𐤀𐤌(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      أرجع TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    𐤀𐤌(remain_ms <= 0)
      أرجع TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    أرجع FALSE;
  }
  وإلا {
    أرجع TRUE;
  }
#وإلا
  (فراغ)data;
  DEBUGASSERT(0); /* We should not come here */
  أرجع FALSE;
#endif /* USE_CURL_ASYNC */
}

ساكن CURLcode cf_dns_connect(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               bool *done)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  𐤀𐤌(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  *done = FALSE;
  𐤀𐤌(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  𐤀𐤌(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  𐤀𐤌(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (صحيح)ctx->resolv_result);
    أرجع ctx->resolv_result;
  }

  𐤀𐤌(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    𐤀𐤌((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  𐤀𐤌(!cf_dns_ready_to_connect(cf, data)) {
    أرجع CURLE_OK;
  }

  𐤀𐤌(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    𐤀𐤌(result || !sub_done)
      أرجع result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  أرجع CURLE_OK;
}

ساكن فراغ cf_dns_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_dns_ctx_destroy(data, ctx);
}

ساكن CURLcode cf_dns_adjust_pollset(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      بنية easy_pollset *ps)
{
#ifdef USE_CURL_ASYNC
  𐤀𐤌(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    بنية cf_dns_ctx *ctx = cf->ctx;
    𐤀𐤌(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      𐤀𐤌(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    أرجع Curl_resolv_pollset(data, ps);
  }
#وإلا
  (فراغ)cf;
  (فراغ)data;
  (فراغ)ps;
#endif
  أرجع CURLE_OK;
}

ساكن CURLcode cf_dns_cntrl(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data,
                             صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_DATA_DONE:
    𐤀𐤌(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع result;
}

بنية Curl_cftype Curl_cft_dns = {
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

ساكن CURLcode cf_dns_create(بنية Curl_cfilter **pcf,
                              بنية Curl_easy *data,
                              بنية Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_dns_ctx *ctx;
  CURLcode result = CURLE_OK;

  (فراغ)data;
  ctx = cf_dns_ctx_create(data, peer, dns_queries, transport, for_proxy);
  𐤀𐤌(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? عدم : cf;
  𐤀𐤌(result)
    cf_dns_ctx_destroy(data, ctx);
  أرجع result;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
ساكن CURLcode cf_dns_add(بنية Curl_easy *data,
                           بنية connectdata *conn,
                           int8_t sockindex,
                           بنية Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  بنية Curl_cfilter *cf = عدم;
  bool for_proxy = FALSE;
  CURLcode result;

  𐤀𐤌(!peer)
    أرجع CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  𐤀𐤌(result)
    اذهب out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  أرجع result;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
ساكن CURLcode cf_dns_insert_after(بنية Curl_cfilter *cf_at,
                                    بنية Curl_easy *data,
                                    بنية Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  بنية Curl_cfilter *cf;
  CURLcode result;

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, FALSE);
  𐤀𐤌(result)
    أرجع result;

  Curl_conn_cf_insert_after(cf_at, cf);
  أرجع CURLE_OK;
}

ساكن CURLcode cf_dns_add_resolve(بنية Curl_easy *data,
                                   بنية connectdata *conn,
                                   int8_t sockindex,
                                   بنية Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  بنية Curl_cfilter *cf = data->conn->cfilter[sockindex];
  بنية Curl_cfilter *cf_dns = عدم;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  𐤀𐤌((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  لكل(; cf && dns_queries; cf = cf->next) {
    𐤀𐤌(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      𐤀𐤌(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        𐤀𐤌(!dns_queries) { /* already there */
          أرجع CURLE_OK;
        }
        وإلا 𐤀𐤌(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          أرجع CURLE_OK;
        }
      }
    }
  }

  𐤀𐤌(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    𐤀𐤌(cf_dns)
      أرجع cf_dns_insert_after(cf_dns, data, peer, dns_queries, transport);
    وإلا
      أرجع cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(بنية Curl_easy *data,
                                        بنية connectdata *conn,
                                        int8_t sockindex,
                                        بنية Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  أرجع cf_dns_add_resolve(data, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_result(بنية connectdata *conn,
                                   int8_t sockindex,
                                   بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = conn->cfilter[sockindex];
  لكل(; cf; cf = cf->next) {
    𐤀𐤌(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      𐤀𐤌(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        𐤀𐤌(ctx->dns || ctx->resolv_result)
          أرجع ctx->resolv_result;
        أرجع CURLE_AGAIN;
      }
    }
  }
  أرجع CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
ثابت بنية Curl_addrinfo *Curl_conn_dns_get_ai(بنية Curl_easy *data,
                                                 بنية Curl_peer *peer,
                                                 int8_t sockindex,
                                                 صحيح ai_family,
                                                 unsigned صحيح index)
{
  بنية connectdata *conn = data->conn;
  بنية Curl_cfilter *cf = conn->cfilter[sockindex];

  لكل(; cf; cf = cf->next) {
    𐤀𐤌(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      𐤀𐤌(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        𐤀𐤌(ctx->resolv_result)
          أرجع عدم;
        وإلا 𐤀𐤌(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          𐤀𐤌((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            أرجع عدم;
#ifdef USE_IPV6
          𐤀𐤌((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            أرجع عدم;
#endif
          أرجع Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        وإلا
          أرجع Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  أرجع عدم;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(بنية Curl_easy *data,
                                         بنية connectdata *conn,
                                         int8_t sockindex,
                                         بنية Curl_peer *peer)
{
  أرجع cf_dns_add_resolve(data, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ثابت بنية Curl_https_rrinfo *Curl_conn_dns_get_https(بنية Curl_easy *data,
                                                        int8_t sockindex,
                                                        بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = data->conn->cfilter[sockindex];
  لكل(; cf; cf = cf->next) {
    𐤀𐤌(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      𐤀𐤌(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        𐤀𐤌(ctx->dns)
          أرجع ctx->dns->hinfo;
        وإلا
          أرجع Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  أرجع عدم;
}

bool Curl_conn_dns_resolved_https(بنية Curl_easy *data,
                                  int8_t sockindex,
                                  بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = data->conn->cfilter[sockindex];
  لكل(; cf; cf = cf->next) {
    𐤀𐤌(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      𐤀𐤌(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        𐤀𐤌(ctx->dns)
          أرجع TRUE;
        وإلا
          أرجع Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  أرجع TRUE;
}

#endif /* USE_HTTPSRR */
