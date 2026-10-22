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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "vdns/dnscache.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_HE_AAAA_AWAIT_MS    25

ರಚನೆ cf_dns_ctx {
  ರಚನೆ Curl_dns_ನಮೂದು *dns;
  ರಚನೆ Curl_peer *peer;
  CURLcode resolv_ಫಲಿತಾಂಶ;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  ಬಿಟ್(started);
  ಬಿಟ್(announced);
  ಬಿಟ್(for_proxy);
};

ಸ್ಥಿರ ರಚನೆ cf_dns_ctx *cf_dns_ctx_create(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  ರಚನೆ cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  CURL_TRC_DNS(ದತ್ತಾಂಶ, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->ಪೋರ್ಟ್, ctx->transport);
  ಹಿಂತಿರುಗಿ ctx;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_dns_ctx_destroy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ cf_dns_ctx *ctx)
{
  ಆದರೆ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_ನಮೂದು_unlink(ದತ್ತಾಂಶ, &ctx->dns);
    curlx_free(ctx);
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ ಶೂನ್ಯ cf_dns_report_addr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ dynbuf *tmp,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label,
                               ಪೂರ್ಣಾಂಕ ai_family,
                               ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai)
{
  ಅಕ್ಷರ buf[MAX_IPADR_LEN];
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = "";
  CURLcode ಫಲಿತಾಂಶ;

  curlx_dyn_reset(tmp);
  ಗಾಗಿ(; ai; ai = ai->ai_ಮುಂದಿನ) {
    ಆದರೆ(ai->ai_family == ai_family) {
      Curl_printable_ವಿಳಾಸ(ai, buf, ಗಾತ್ರ(buf));
      ಫಲಿತಾಂಶ = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        infof(ದತ್ತಾಂಶ, "too many IP, cannot show");
        ಹಿಂತಿರುಗಿ;
      }
      sep = ", ";
    }
  }

  infof(ದತ್ತಾಂಶ, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_dns_report(ರಚನೆ Curl_cfilter *cf,
                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ Curl_dns_ನಮೂದು *dns)
{
  ರಚನೆ cf_dns_ctx *ctx = cf->ctx;
  ರಚನೆ dynbuf tmp;

  ಆದರೆ(!Curl_trc_is_verbose(ದತ್ತಾಂಶ) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(ctx->peer->unix_socket) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "resolved unix://%s", ctx->peer->hostname);
#ಇಲ್ಲದಿದ್ದರೆ
    DEBUGASSERT(0);
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    curlx_dyn_init(&tmp, 1024);
    ಆದರೆ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(ದತ್ತಾಂಶ, "Host %s:%u was resolved.", dns->hostname, dns->ಪೋರ್ಟ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_IPV6
      cf_dns_report_addr(ದತ್ತಾಂಶ, &tmp, "IPv6: ", AF_INET6, dns->addr);
#ಆದರೆ_ಅಂತ್ಯ
      cf_dns_report_addr(ದತ್ತಾಂಶ, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      ಆದರೆ(!dns->hinfo)
        infof(ದತ್ತಾಂಶ, "HTTPS-RR %s:%u: -", dns->hostname, dns->ಪೋರ್ಟ್);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!Curl_httpsrr_applicable(ದತ್ತಾಂಶ, dns->hinfo))
        infof(ದತ್ತಾಂಶ, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->ಪೋರ್ಟ್);
      ಇಲ್ಲದಿದ್ದರೆ {
        CURLcode ಫಲಿತಾಂಶ = Curl_httpsrr_print(&tmp, dns->hinfo);
        ಆದರೆ(!ಫಲಿತಾಂಶ)
          infof(ದತ್ತಾಂಶ, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->ಪೋರ್ಟ್, curlx_dyn_ptr(&tmp));
        ಇಲ್ಲದಿದ್ದರೆ
          infof(ದತ್ತಾಂಶ, "Error printing HTTPS-RR information");
      }
    }
#ಆದರೆ_ಅಂತ್ಯ
    curlx_dyn_free(&tmp);
  }
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು cf_dns_report(x, y, z) Curl_nop_stmt
#ಆದರೆ_ಅಂತ್ಯ

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
ಸ್ಥಿರ CURLcode cf_dns_ಆರಂಭ(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ರಚನೆ cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *pdns = ಖಾಲಿ;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->ಪೋರ್ಟ್);
  ಆದರೆ(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  ಆದರೆ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ಆದರೆ(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#ಆದರೆ_ಅಂತ್ಯ

    ಫಲಿತಾಂಶ = Curl_resolv(ದತ್ತಾಂಶ, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    ಫಲಿತಾಂಶ = Curl_resolv_https(ದತ್ತಾಂಶ, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "unsupported DNS queries %x", ctx->dns_queries);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!ಫಲಿತಾಂಶ || !*pdns);
  ಆದರೆ(!ಫಲಿತಾಂಶ) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) { /* async resolv in progress */
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(ದತ್ತಾಂಶ, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(ದತ್ತಾಂಶ, ಖಾಲಿ, TIMER_STARTSINGLE));
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    DEBUGASSERT(ಫಲಿತಾಂಶ);
    ಆದರೆ(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(ದತ್ತಾಂಶ, "Could not resolve: %s", ctx->peer->hostname);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
}

ಸ್ಥಿರ bool cf_dns_ready_ಗಮ್ಯ_connect(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_dns_ctx *ctx = cf->ctx;

  ಆದರೆ(ctx->resolv_ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ TRUE;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->dns)
    ಹಿಂತಿರುಗಿ TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_ASYNC
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    ಆದರೆ(Curl_resolv_has_answers(ದತ್ತಾಂಶ, ctx->resolv_id, CURL_DNSQ_AAAA))
      ಹಿಂತಿರುಗಿ TRUE;
    pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(ದತ್ತಾಂಶ, ctx->resolv_id, pnow);
    ಆದರೆ(remain_ms <= 0)
      ಹಿಂತಿರುಗಿ TRUE;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    ಹಿಂತಿರುಗಿ FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ TRUE;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(0); /* We should not come here */
  ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ /* USE_CURL_ASYNC */
}

ಸ್ಥಿರ CURLcode cf_dns_connect(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               bool *ಮುಗಿದ)
{
  ರಚನೆ cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *ಮುಗಿದ = FALSE;
  ಆದರೆ(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_ಫಲಿತಾಂಶ = cf_dns_ಆರಂಭ(cf, ದತ್ತಾಂಶ, &ctx->dns);
  }

  ಆದರೆ(!ctx->dns && !ctx->resolv_ಫಲಿತಾಂಶ) {
    ctx->resolv_ಫಲಿತಾಂಶ =
      Curl_resolv_take_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ, ctx->resolv_id, &ctx->dns);
  }

  ಆದರೆ(ctx->resolv_ಫಲಿತಾಂಶ && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (ಪೂರ್ಣಾಂಕ)ctx->resolv_ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ ctx->resolv_ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    ಆದರೆ((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->ಬಿಟ್ಟುಗಳು.dns_resolved = TRUE;
      Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, ದತ್ತಾಂಶ, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  ಆದರೆ(!cf_dns_ready_ಗಮ್ಯ_connect(cf, ದತ್ತಾಂಶ)) {
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(cf->ಮುಂದಿನ && !cf->ಮುಂದಿನ->connected) {
    bool sub_ಮುಗಿದ;
    CURLcode ಫಲಿತಾಂಶ = Curl_conn_cf_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &sub_ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ || !sub_ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    DEBUGASSERT(sub_ಮುಗಿದ);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connected filter chain below");
  *ಮುಗಿದ = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(ದತ್ತಾಂಶ, ctx->resolv_id);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_dns_destroy(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "destroy");
  cf_dns_ctx_destroy(ದತ್ತಾಂಶ, ctx);
}

ಸ್ಥಿರ CURLcode cf_dns_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ easy_pollset *ps)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_ASYNC
  ಆದರೆ(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    ರಚನೆ cf_dns_ctx *ctx = cf->ctx;
    ಆದರೆ(!ctx->started) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->ಪೋರ್ಟ್,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_ಫಲಿತಾಂಶ = cf_dns_ಆರಂಭ(cf, ದತ್ತಾಂಶ, &ctx->dns);
      ಆದರೆ(ctx->resolv_ಫಲಿತಾಂಶ || ctx->dns) {
        Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
      }
    }
    ಹಿಂತಿರುಗಿ Curl_resolv_pollset(ದತ್ತಾಂಶ, ps);
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)ps;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_dns_cntrl(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  ರಚನೆ cf_dns_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ಆಯ್ಕೆ(event) {
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_ಮುಗಿದ:
    ಆದರೆ(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_ನಮೂದು_unlink(ದತ್ತಾಂಶ, &ctx->dns);
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ರಚನೆ Curl_cftype Curl_cft_dns = {
  "DNS",
  CF_TYPE_ಸಿದ್ಧತೆ | CF_TYPE_DNS,
  CURL_LOG_LVL_NONE,
  cf_dns_destroy,
  cf_dns_connect,
  Curl_cf_def_shutdown,
  cf_dns_adjust_pollset,
  Curl_cf_def_ದತ್ತಾಂಶ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_dns_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ಸ್ಥಿರ CURLcode cf_dns_create(ರಚನೆ Curl_cfilter **pcf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ cf_dns_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ctx = cf_dns_ctx_create(ದತ್ತಾಂಶ, peer, dns_queries, transport, for_proxy);
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

ಹೊರಹೋಗು:
  *pcf = ಫಲಿತಾಂಶ ? ಖಾಲಿ : cf;
  ಆದರೆ(ಫಲಿತಾಂಶ)
    cf_dns_ctx_destroy(ದತ್ತಾಂಶ, ctx);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
ಸ್ಥಿರ CURLcode cf_dns_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ connectdata *conn,
                           int8_t sockindex,
                           ರಚನೆ Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  bool for_proxy = FALSE;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!peer)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#ಆದರೆ_ಅಂತ್ಯ

  ಫಲಿತಾಂಶ = cf_dns_create(&cf, ದತ್ತಾಂಶ, peer, dns_queries, transport, for_proxy);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  Curl_conn_cf_ಕೂಡಿಸು(ದತ್ತಾಂಶ, conn, sockindex, cf);
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
ಸ್ಥಿರ CURLcode cf_dns_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = cf_dns_create(&cf, ದತ್ತಾಂಶ, peer, dns_queries, transport, FALSE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_dns_ಕೂಡಿಸು_resolve(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ connectdata *conn,
                                   int8_t sockindex,
                                   ರಚನೆ Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  ರಚನೆ Curl_cfilter *cf = ದತ್ತಾಂಶ->conn->cfilter[sockindex];
  ರಚನೆ Curl_cfilter *cf_dns = ಖಾಲಿ;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  ಆದರೆ((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  ಗಾಗಿ(; cf && dns_queries; cf = cf->ಮುಂದಿನ) {
    ಆದರೆ(cf->cft == &Curl_cft_dns) {
      ರಚನೆ cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      ಆದರೆ(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        ಆದರೆ(!dns_queries) { /* already there */
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(ದತ್ತಾಂಶ, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->ಪೋರ್ಟ್);
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
      }
    }
  }

  ಆದರೆ(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    ಆದರೆ(cf_dns)
      ಹಿಂತಿರುಗಿ cf_dns_ಸೇರಿಸು_after(cf_dns, ದತ್ತಾಂಶ, peer, dns_queries, transport);
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ cf_dns_ಕೂಡಿಸು(ದತ್ತಾಂಶ, conn, sockindex, peer, dns_queries, transport);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_conn_dns_ಕೂಡಿಸು_addr_resolve(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ connectdata *conn,
                                        int8_t sockindex,
                                        ರಚನೆ Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  ಹಿಂತಿರುಗಿ cf_dns_ಕೂಡಿಸು_resolve(ದತ್ತಾಂಶ, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_ಫಲಿತಾಂಶ(ರಚನೆ connectdata *conn,
                                   int8_t sockindex,
                                   ರಚನೆ Curl_peer *peer)
{
  ರಚನೆ Curl_cfilter *cf = conn->cfilter[sockindex];
  ಗಾಗಿ(; cf; cf = cf->ಮುಂದಿನ) {
    ಆದರೆ(cf->cft == &Curl_cft_dns) {
      ರಚನೆ cf_dns_ctx *ctx = cf->ctx;
      ಆದರೆ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        ಆದರೆ(ctx->dns || ctx->resolv_ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ctx->resolv_ಫಲಿತಾಂಶ;
        ಹಿಂತಿರುಗಿ CURLE_AGAIN;
      }
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *Curl_conn_dns_get_ai(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                 ರಚನೆ Curl_peer *peer,
                                                 int8_t sockindex,
                                                 ಪೂರ್ಣಾಂಕ ai_family,
                                                 ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ Curl_cfilter *cf = conn->cfilter[sockindex];

  ಗಾಗಿ(; cf; cf = cf->ಮುಂದಿನ) {
    ಆದರೆ(cf->cft == &Curl_cft_dns) {
      ರಚನೆ cf_dns_ctx *ctx = cf->ctx;
      ಆದರೆ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    ಸೂಚ್ಯಂಕ, peer->hostname, peer->ಪೋರ್ಟ್, ai_family, !!ctx->dns);
        ಆದರೆ(ctx->resolv_ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          ಆದರೆ((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
          ಆದರೆ((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
          ಹಿಂತಿರುಗಿ Curl_addrinfo_get(ctx->dns->addr, ai_family, ಸೂಚ್ಯಂಕ);
        }
        ಇಲ್ಲದಿದ್ದರೆ
          ಹಿಂತಿರುಗಿ Curl_resolv_get_ai(ದತ್ತಾಂಶ, ctx->resolv_id, ai_family, ಸೂಚ್ಯಂಕ);
      }
    }
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
CURLcode Curl_conn_dns_ಕೂಡಿಸು_https_resolve(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ connectdata *conn,
                                         int8_t sockindex,
                                         ರಚನೆ Curl_peer *peer)
{
  ಹಿಂತಿರುಗಿ cf_dns_ಕೂಡಿಸು_resolve(ದತ್ತಾಂಶ, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_https_rrinfo *Curl_conn_dns_get_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                        int8_t sockindex,
                                                        ರಚನೆ Curl_peer *peer)
{
  ರಚನೆ Curl_cfilter *cf = ದತ್ತಾಂಶ->conn->cfilter[sockindex];
  ಗಾಗಿ(; cf; cf = cf->ಮುಂದಿನ) {
    ಆದರೆ(cf->cft == &Curl_cft_dns) {
      ರಚನೆ cf_dns_ctx *ctx = cf->ctx;
      ಆದರೆ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ಆದರೆ(ctx->dns)
          ಹಿಂತಿರುಗಿ ctx->dns->hinfo;
        ಇಲ್ಲದಿದ್ದರೆ
          ಹಿಂತಿರುಗಿ Curl_resolv_get_https(ದತ್ತಾಂಶ, ctx->resolv_id);
      }
    }
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

bool Curl_conn_dns_resolved_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  int8_t sockindex,
                                  ರಚನೆ Curl_peer *peer)
{
  ರಚನೆ Curl_cfilter *cf = ದತ್ತಾಂಶ->conn->cfilter[sockindex];
  ಗಾಗಿ(; cf; cf = cf->ಮುಂದಿನ) {
    ಆದರೆ(cf->cft == &Curl_cft_dns) {
      ರಚನೆ cf_dns_ctx *ctx = cf->ctx;
      ಆದರೆ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ಆದರೆ(ctx->dns)
          ಹಿಂತಿರುಗಿ TRUE;
        ಇಲ್ಲದಿದ್ದರೆ
          ಹಿಂತಿರುಗಿ Curl_resolv_knows_https(ದತ್ತಾಂಶ, ctx->resolv_id);
      }
    }
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */
