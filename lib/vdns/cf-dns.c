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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/dnscache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_HE_AAAA_AWAIT_MS    25

ਬਣਤਰ cf_dns_ctx {
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns;
  ਬਣਤਰ Curl_peer *peer;
  CURLcode resolv_ਨਤੀਜਾ;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  ਦੋਆਧਾਰੀ_ਅੰਕ(started);
  ਦੋਆਧਾਰੀ_ਅੰਕ(announced);
  ਦੋਆਧਾਰੀ_ਅੰਕ(for_proxy);
};

ਸਥਿਰ ਬਣਤਰ cf_dns_ctx *cf_dns_ctx_create(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  ਬਣਤਰ cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx)
    ਵਾਪਸ ਨਿਰਾ;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  {
    ਅਚਲ ਅੱਖਰ *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#ਜੇ_ਅੰਤ

  CURL_TRC_DNS(ਅੰਕੜੇ, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->ਪੋਰਟ, ctx->transport);
  ਵਾਪਸ ctx;
}

ਸਥਿਰ ਖਾਲੀ cf_dns_ctx_destroy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ cf_dns_ctx *ctx)
{
  ਜੇ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_ਇੰਦਰਾਜ_unlink(ਅੰਕੜੇ, &ctx->dns);
    curlx_free(ctx);
  }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ ਖਾਲੀ cf_dns_report_addr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ dynbuf *tmp,
                               ਅਚਲ ਅੱਖਰ *label,
                               ਪੂਰਨ ai_family,
                               ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai)
{
  ਅੱਖਰ buf[MAX_IPADR_LEN];
  ਅਚਲ ਅੱਖਰ *sep = "";
  CURLcode ਨਤੀਜਾ;

  curlx_dyn_reset(tmp);
  ਲਈ(; ai; ai = ai->ai_ਅਗਲਾ) {
    ਜੇ(ai->ai_family == ai_family) {
      Curl_printable_ਪਤਾ(ai, buf, ਆਕਾਰ(buf));
      ਨਤੀਜਾ = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      ਜੇ(ਨਤੀਜਾ) {
        infof(ਅੰਕੜੇ, "too many IP, cannot show");
        ਵਾਪਸ;
      }
      sep = ", ";
    }
  }

  infof(ਅੰਕੜੇ, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

ਸਥਿਰ ਖਾਲੀ cf_dns_report(ਬਣਤਰ Curl_cfilter *cf,
                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns)
{
  ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;
  ਬਣਤਰ dynbuf tmp;

  ਜੇ(!Curl_trc_is_verbose(ਅੰਕੜੇ) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    ਵਾਪਸ;

  ਜੇ(ctx->peer->unix_socket) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "resolved unix://%s", ctx->peer->hostname);
#ਨਹੀਂਤਾਂ
    DEBUGASSERT(0);
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ {
    curlx_dyn_init(&tmp, 1024);
    ਜੇ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(ਅੰਕੜੇ, "Host %s:%u was resolved.", dns->hostname, dns->ਪੋਰਟ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_IPV6
      cf_dns_report_addr(ਅੰਕੜੇ, &tmp, "IPv6: ", AF_INET6, dns->addr);
#ਜੇ_ਅੰਤ
      cf_dns_report_addr(ਅੰਕੜੇ, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
    ਨਹੀਂਤਾਂ ਜੇ(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      ਜੇ(!dns->hinfo)
        infof(ਅੰਕੜੇ, "HTTPS-RR %s:%u: -", dns->hostname, dns->ਪੋਰਟ);
      ਨਹੀਂਤਾਂ ਜੇ(!Curl_httpsrr_applicable(ਅੰਕੜੇ, dns->hinfo))
        infof(ਅੰਕੜੇ, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->ਪੋਰਟ);
      ਨਹੀਂਤਾਂ {
        CURLcode ਨਤੀਜਾ = Curl_httpsrr_print(&tmp, dns->hinfo);
        ਜੇ(!ਨਤੀਜਾ)
          infof(ਅੰਕੜੇ, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->ਪੋਰਟ, curlx_dyn_ptr(&tmp));
        ਨਹੀਂਤਾਂ
          infof(ਅੰਕੜੇ, "Error printing HTTPS-RR information");
      }
    }
#ਜੇ_ਅੰਤ
    curlx_dyn_free(&tmp);
  }
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cf_dns_report(x, y, z) Curl_nop_stmt
#ਜੇ_ਅੰਤ

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
ਸਥਿਰ CURLcode cf_dns_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *pdns = ਨਿਰਾ;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->ਪੋਰਟ);
  ਜੇ(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  ਜੇ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ਜੇ(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਨਹੀਂਤਾਂ ਜੇ(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#ਜੇ_ਅੰਤ

    ਨਤੀਜਾ = Curl_resolv(ਅੰਕੜੇ, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਨਹੀਂਤਾਂ ਜੇ(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    ਨਤੀਜਾ = Curl_resolv_https(ਅੰਕੜੇ, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#ਜੇ_ਅੰਤ
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "unsupported DNS queries %x", ctx->dns_queries);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!ਨਤੀਜਾ || !*pdns);
  ਜੇ(!ਨਤੀਜਾ) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    ਵਾਪਸ CURLE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) { /* async resolv in progress */
    ਵਾਪਸ CURLE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(ਅੰਕੜੇ, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(ਅੰਕੜੇ, ਨਿਰਾ, TIMER_STARTSINGLE));
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
  }
  ਨਹੀਂਤਾਂ {
    DEBUGASSERT(ਨਤੀਜਾ);
    ਜੇ(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(ਅੰਕੜੇ, "Could not resolve: %s", ctx->peer->hostname);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
}

ਸਥਿਰ bool cf_dns_ready_ਮੰਜ਼ਿਲ_connect(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;

  ਜੇ(ctx->resolv_ਨਤੀਜਾ)
    ਵਾਪਸ TRUE;
  ਨਹੀਂਤਾਂ ਜੇ(ctx->dns)
    ਵਾਪਸ TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_ASYNC
  ਨਹੀਂਤਾਂ ਜੇ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ਅਚਲ ਬਣਤਰ curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    ਜੇ(Curl_resolv_has_answers(ਅੰਕੜੇ, ctx->resolv_id, CURL_DNSQ_AAAA))
      ਵਾਪਸ TRUE;
    pnow = Curl_pgrs_now(ਅੰਕੜੇ);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(ਅੰਕੜੇ, ctx->resolv_id, pnow);
    ਜੇ(remain_ms <= 0)
      ਵਾਪਸ TRUE;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    ਵਾਪਸ FALSE;
  }
  ਨਹੀਂਤਾਂ {
    ਵਾਪਸ TRUE;
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(0); /* We should not come here */
  ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ /* USE_CURL_ASYNC */
}

ਸਥਿਰ CURLcode cf_dns_connect(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  *ਪੂਰਾ = FALSE;
  ਜੇ(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_ਨਤੀਜਾ = cf_dns_ਸ਼ੁਰੂ(cf, ਅੰਕੜੇ, &ctx->dns);
  }

  ਜੇ(!ctx->dns && !ctx->resolv_ਨਤੀਜਾ) {
    ctx->resolv_ਨਤੀਜਾ =
      Curl_resolv_take_ਨਤੀਜਾ(ਅੰਕੜੇ, ctx->resolv_id, &ctx->dns);
  }

  ਜੇ(ctx->resolv_ਨਤੀਜਾ && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (ਪੂਰਨ)ctx->resolv_ਨਤੀਜਾ);
    ਵਾਪਸ ctx->resolv_ਨਤੀਜਾ;
  }

  ਜੇ(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    ਜੇ((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->ਬਿੱਟ.dns_resolved = TRUE;
      Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, ਅੰਕੜੇ, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  ਜੇ(!cf_dns_ready_ਮੰਜ਼ਿਲ_connect(cf, ਅੰਕੜੇ)) {
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(cf->ਅਗਲਾ && !cf->ਅਗਲਾ->connected) {
    bool sub_ਪੂਰਾ;
    CURLcode ਨਤੀਜਾ = Curl_conn_cf_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, &sub_ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ || !sub_ਪੂਰਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    DEBUGASSERT(sub_ਪੂਰਾ);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "connected filter chain below");
  *ਪੂਰਾ = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(ਅੰਕੜੇ, ctx->resolv_id);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cf_dns_destroy(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "destroy");
  cf_dns_ctx_destroy(ਅੰਕੜੇ, ctx);
}

ਸਥਿਰ CURLcode cf_dns_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ easy_pollset *ps)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_ASYNC
  ਜੇ(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;
    ਜੇ(!ctx->started) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->ਪੋਰਟ,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_ਨਤੀਜਾ = cf_dns_ਸ਼ੁਰੂ(cf, ਅੰਕੜੇ, &ctx->dns);
      ਜੇ(ctx->resolv_ਨਤੀਜਾ || ctx->dns) {
        Curl_multi_mark_dirty(ਅੰਕੜੇ);
      }
    }
    ਵਾਪਸ Curl_resolv_pollset(ਅੰਕੜੇ, ps);
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)ps;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_dns_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)arg1;
  (ਖਾਲੀ)arg2;
  ਚੋਣ(event) {
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_ਪੂਰਾ:
    ਜੇ(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_ਇੰਦਰਾਜ_unlink(ਅੰਕੜੇ, &ctx->dns);
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਬਣਤਰ Curl_cftype Curl_cft_dns = {
  "DNS",
  CF_TYPE_ਤਿਆਰੀ | CF_TYPE_DNS,
  CURL_LOG_LVL_NONE,
  cf_dns_destroy,
  cf_dns_connect,
  Curl_cf_def_shutdown,
  cf_dns_adjust_pollset,
  Curl_cf_def_ਅੰਕੜੇ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_dns_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ਸਥਿਰ CURLcode cf_dns_create(ਬਣਤਰ Curl_cfilter **pcf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ cf_dns_ctx *ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ctx = cf_dns_ctx_create(ਅੰਕੜੇ, peer, dns_queries, transport, for_proxy);
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

ਬਾਹਰਲਾ:
  *pcf = ਨਤੀਜਾ ? ਨਿਰਾ : cf;
  ਜੇ(ਨਤੀਜਾ)
    cf_dns_ctx_destroy(ਅੰਕੜੇ, ctx);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
ਸਥਿਰ CURLcode cf_dns_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ connectdata *conn,
                           int8_t sockindex,
                           ਬਣਤਰ Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  bool for_proxy = FALSE;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!peer)
    ਵਾਪਸ CURLE_FAILED_INIT;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#ਜੇ_ਅੰਤ

  ਨਤੀਜਾ = cf_dns_create(&cf, ਅੰਕੜੇ, peer, dns_queries, transport, for_proxy);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  Curl_conn_cf_ਜੋੜੋ(ਅੰਕੜੇ, conn, sockindex, cf);
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
ਸਥਿਰ CURLcode cf_dns_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = cf_dns_create(&cf, ਅੰਕੜੇ, peer, dns_queries, transport, FALSE);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  Curl_conn_cf_ਪਾਓ_after(cf_at, cf);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_dns_ਜੋੜੋ_resolve(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ connectdata *conn,
                                   int8_t sockindex,
                                   ਬਣਤਰ Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  ਬਣਤਰ Curl_cfilter *cf = ਅੰਕੜੇ->conn->cfilter[sockindex];
  ਬਣਤਰ Curl_cfilter *cf_dns = ਨਿਰਾ;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  ਜੇ((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  ਲਈ(; cf && dns_queries; cf = cf->ਅਗਲਾ) {
    ਜੇ(cf->cft == &Curl_cft_dns) {
      ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      ਜੇ(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        ਜੇ(!dns_queries) { /* already there */
          ਵਾਪਸ CURLE_OK;
        }
        ਨਹੀਂਤਾਂ ਜੇ(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(ਅੰਕੜੇ, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->ਪੋਰਟ);
          ਵਾਪਸ CURLE_OK;
        }
      }
    }
  }

  ਜੇ(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    ਜੇ(cf_dns)
      ਵਾਪਸ cf_dns_ਪਾਓ_after(cf_dns, ਅੰਕੜੇ, peer, dns_queries, transport);
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ cf_dns_ਜੋੜੋ(ਅੰਕੜੇ, conn, sockindex, peer, dns_queries, transport);
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_conn_dns_ਜੋੜੋ_addr_resolve(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ connectdata *conn,
                                        int8_t sockindex,
                                        ਬਣਤਰ Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  ਵਾਪਸ cf_dns_ਜੋੜੋ_resolve(ਅੰਕੜੇ, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_ਨਤੀਜਾ(ਬਣਤਰ connectdata *conn,
                                   int8_t sockindex,
                                   ਬਣਤਰ Curl_peer *peer)
{
  ਬਣਤਰ Curl_cfilter *cf = conn->cfilter[sockindex];
  ਲਈ(; cf; cf = cf->ਅਗਲਾ) {
    ਜੇ(cf->cft == &Curl_cft_dns) {
      ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;
      ਜੇ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        ਜੇ(ctx->dns || ctx->resolv_ਨਤੀਜਾ)
          ਵਾਪਸ ctx->resolv_ਨਤੀਜਾ;
        ਵਾਪਸ CURLE_AGAIN;
      }
    }
  }
  ਵਾਪਸ CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
ਅਚਲ ਬਣਤਰ Curl_addrinfo *Curl_conn_dns_get_ai(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                 ਬਣਤਰ Curl_peer *peer,
                                                 int8_t sockindex,
                                                 ਪੂਰਨ ai_family,
                                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ Curl_cfilter *cf = conn->cfilter[sockindex];

  ਲਈ(; cf; cf = cf->ਅਗਲਾ) {
    ਜੇ(cf->cft == &Curl_cft_dns) {
      ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;
      ਜੇ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    ਸੂਚਕਾਂਕ, peer->hostname, peer->ਪੋਰਟ, ai_family, !!ctx->dns);
        ਜੇ(ctx->resolv_ਨਤੀਜਾ)
          ਵਾਪਸ ਨਿਰਾ;
        ਨਹੀਂਤਾਂ ਜੇ(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          ਜੇ((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
          ਜੇ((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
          ਵਾਪਸ Curl_addrinfo_get(ctx->dns->addr, ai_family, ਸੂਚਕਾਂਕ);
        }
        ਨਹੀਂਤਾਂ
          ਵਾਪਸ Curl_resolv_get_ai(ਅੰਕੜੇ, ctx->resolv_id, ai_family, ਸੂਚਕਾਂਕ);
      }
    }
  }
  ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
CURLcode Curl_conn_dns_ਜੋੜੋ_https_resolve(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ connectdata *conn,
                                         int8_t sockindex,
                                         ਬਣਤਰ Curl_peer *peer)
{
  ਵਾਪਸ cf_dns_ਜੋੜੋ_resolve(ਅੰਕੜੇ, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ਅਚਲ ਬਣਤਰ Curl_https_rrinfo *Curl_conn_dns_get_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                        int8_t sockindex,
                                                        ਬਣਤਰ Curl_peer *peer)
{
  ਬਣਤਰ Curl_cfilter *cf = ਅੰਕੜੇ->conn->cfilter[sockindex];
  ਲਈ(; cf; cf = cf->ਅਗਲਾ) {
    ਜੇ(cf->cft == &Curl_cft_dns) {
      ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;
      ਜੇ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ਜੇ(ctx->dns)
          ਵਾਪਸ ctx->dns->hinfo;
        ਨਹੀਂਤਾਂ
          ਵਾਪਸ Curl_resolv_get_https(ਅੰਕੜੇ, ctx->resolv_id);
      }
    }
  }
  ਵਾਪਸ ਨਿਰਾ;
}

bool Curl_conn_dns_resolved_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  int8_t sockindex,
                                  ਬਣਤਰ Curl_peer *peer)
{
  ਬਣਤਰ Curl_cfilter *cf = ਅੰਕੜੇ->conn->cfilter[sockindex];
  ਲਈ(; cf; cf = cf->ਅਗਲਾ) {
    ਜੇ(cf->cft == &Curl_cft_dns) {
      ਬਣਤਰ cf_dns_ctx *ctx = cf->ctx;
      ਜੇ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ਜੇ(ctx->dns)
          ਵਾਪਸ TRUE;
        ਨਹੀਂਤਾਂ
          ਵਾਪਸ Curl_resolv_knows_https(ਅੰਕੜੇ, ctx->resolv_id);
      }
    }
  }
  ਵਾਪਸ TRUE;
}

#ਜੇ_ਅੰਤ /* USE_HTTPSRR */
