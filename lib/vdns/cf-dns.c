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
#define CURL_HE_AAAA_AWAIT_MS    25

ಸಂರಚನಾ cf_dns_ctx {
  ಸಂರಚನಾ Curl_dns_entry *dns;
  ಸಂರಚನಾ Curl_peer *peer;
  CURLcode resolv_result;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

ಸ್ಥಿರ ಸಂರಚನಾ cf_dns_ctx *cf_dns_ctx_create(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  ಸಂರಚನಾ cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    ಧ್ರುವ ವರ್ಣ *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  ಪ್ರತ್ಯಾಗಮ ctx;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_dns_ctx_destroy(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ cf_dns_ctx *ctx)
{
  ಯದಿ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
ಸ್ಥಿರ ಶೂನ್ಯ cf_dns_report_addr(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ dynbuf *tmp,
                               ಧ್ರುವ ವರ್ಣ *label,
                               ಪೂರ್ಣಾಙ್ಕ ai_family,
                               ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai)
{
  ವರ್ಣ buf[MAX_IPADR_LEN];
  ಧ್ರುವ ವರ್ಣ *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  ಪ್ರತಿ(; ai; ai = ai->ai_next) {
    ಯದಿ(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, ಪರಿಮಾಣ(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      ಯದಿ(result) {
        infof(data, "too many IP, cannot show");
        ಪ್ರತ್ಯಾಗಮ;
      }
      sep = ", ";
    }
  }

  infof(data, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_dns_report(ಸಂರಚನಾ Curl_cfilter *cf,
                          ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ Curl_dns_entry *dns)
{
  ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ dynbuf tmp;

  ಯದಿ(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#ಅನ್ಯಥಾ
    DEBUGASSERT(0);
#endif
  }
  ಅನ್ಯಥಾ {
    curlx_dyn_init(&tmp, 1024);
    ಯದಿ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    ಅನ್ಯಥಾ ಯದಿ(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      ಯದಿ(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      ಅನ್ಯಥಾ ಯದಿ(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      ಅನ್ಯಥಾ {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        ಯದಿ(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        ಅನ್ಯಥಾ
          infof(data, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#ಅನ್ಯಥಾ
#define cf_dns_report(x, y, z) Curl_nop_stmt
#endif

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
ಸ್ಥಿರ CURLcode cf_dns_start(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(data);
  CURLcode result = CURLE_OK;

  *pdns = ರಿಕ್ತ;

  CURL_TRC_CF(data, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->port);
  ಯದಿ(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  ಯದಿ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ಯದಿ(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    ಅನ್ಯಥಾ ಯದಿ(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  ಅನ್ಯಥಾ ಯದಿ(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  ಅನ್ಯಥಾ {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  ಯದಿ(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(result == CURLE_AGAIN) { /* async resolv in progress */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, ರಿಕ್ತ, TIMER_STARTSINGLE));
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
  }
  ಅನ್ಯಥಾ {
    DEBUGASSERT(result);
    ಯದಿ(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    ಪ್ರತ್ಯಾಗಮ result;
  }
}

ಸ್ಥಿರ bool cf_dns_ready_to_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;

  ಯದಿ(ctx->resolv_result)
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಅನ್ಯಥಾ ಯದಿ(ctx->dns)
    ಪ್ರತ್ಯಾಗಮ TRUE;
#ifdef USE_CURL_ASYNC
  ಅನ್ಯಥಾ ಯದಿ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ಧ್ರುವ ಸಂರಚನಾ curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    ಯದಿ(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    ಯದಿ(remain_ms <= 0)
      ಪ್ರತ್ಯಾಗಮ TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಅನ್ಯಥಾ {
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  DEBUGASSERT(0); /* We should not come here */
  ಪ್ರತ್ಯಾಗಮ FALSE;
#endif /* USE_CURL_ASYNC */
}

ಸ್ಥಿರ CURLcode cf_dns_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               bool *done)
{
  ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *done = FALSE;
  ಯದಿ(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  ಯದಿ(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  ಯದಿ(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (ಪೂರ್ಣಾಙ್ಕ)ctx->resolv_result);
    ಪ್ರತ್ಯಾಗಮ ctx->resolv_result;
  }

  ಯದಿ(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    ಯದಿ((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  ಯದಿ(!cf_dns_ready_to_connect(cf, data)) {
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ಯದಿ(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    ಯದಿ(result || !sub_done)
      ಪ್ರತ್ಯಾಗಮ result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_dns_destroy(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_dns_ctx_destroy(data, ctx);
}

ಸ್ಥಿರ CURLcode cf_dns_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ easy_pollset *ps)
{
#ifdef USE_CURL_ASYNC
  ಯದಿ(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;
    ಯದಿ(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      ಯದಿ(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    ಪ್ರತ್ಯಾಗಮ Curl_resolv_pollset(data, ps);
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)ps;
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_dns_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ವಿಕಲ್ಪ(event) {
  ಪಕ್ಷ CF_CTRL_DATA_DONE:
    ಯದಿ(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_dns = {
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

ಸ್ಥಿರ CURLcode cf_dns_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                              ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_dns_ctx *ctx;
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)data;
  ctx = cf_dns_ctx_create(data, peer, dns_queries, transport, for_proxy);
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? ರಿಕ್ತ : cf;
  ಯದಿ(result)
    cf_dns_ctx_destroy(data, ctx);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
ಸ್ಥಿರ CURLcode cf_dns_add(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ connectdata *conn,
                           int8_t sockindex,
                           ಸಂರಚನಾ Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  bool for_proxy = FALSE;
  CURLcode result;

  ಯದಿ(!peer)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
ಸ್ಥಿರ CURLcode cf_dns_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result;

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, FALSE);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_dns_add_resolve(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ connectdata *conn,
                                   int8_t sockindex,
                                   ಸಂರಚನಾ Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  ಸಂರಚನಾ Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ಸಂರಚನಾ Curl_cfilter *cf_dns = ರಿಕ್ತ;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  ಯದಿ((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  ಪ್ರತಿ(; cf && dns_queries; cf = cf->next) {
    ಯದಿ(cf->cft == &Curl_cft_dns) {
      ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      ಯದಿ(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        ಯದಿ(!dns_queries) { /* already there */
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
        ಅನ್ಯಥಾ ಯದಿ(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
      }
    }
  }

  ಯದಿ(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    ಯದಿ(cf_dns)
      ಪ್ರತ್ಯಾಗಮ cf_dns_ಸೇರಿಸು_after(cf_dns, data, peer, dns_queries, transport);
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ connectdata *conn,
                                        int8_t sockindex,
                                        ಸಂರಚನಾ Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  ಪ್ರತ್ಯಾಗಮ cf_dns_add_resolve(data, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_result(ಸಂರಚನಾ connectdata *conn,
                                   int8_t sockindex,
                                   ಸಂರಚನಾ Curl_peer *peer)
{
  ಸಂರಚನಾ Curl_cfilter *cf = conn->cfilter[sockindex];
  ಪ್ರತಿ(; cf; cf = cf->next) {
    ಯದಿ(cf->cft == &Curl_cft_dns) {
      ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;
      ಯದಿ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        ಯದಿ(ctx->dns || ctx->resolv_result)
          ಪ್ರತ್ಯಾಗಮ ctx->resolv_result;
        ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *Curl_conn_dns_get_ai(ಸಂರಚನಾ Curl_easy *data,
                                                 ಸಂರಚನಾ Curl_peer *peer,
                                                 int8_t sockindex,
                                                 ಪೂರ್ಣಾಙ್ಕ ai_family,
                                                 unsigned ಪೂರ್ಣಾಙ್ಕ index)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ Curl_cfilter *cf = conn->cfilter[sockindex];

  ಪ್ರತಿ(; cf; cf = cf->next) {
    ಯದಿ(cf->cft == &Curl_cft_dns) {
      ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;
      ಯದಿ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        ಯದಿ(ctx->resolv_result)
          ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಅನ್ಯಥಾ ಯದಿ(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          ಯದಿ((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#ifdef USE_IPV6
          ಯದಿ((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
          ಪ್ರತ್ಯಾಗಮ Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        ಅನ್ಯಥಾ
          ಪ್ರತ್ಯಾಗಮ Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ connectdata *conn,
                                         int8_t sockindex,
                                         ಸಂರಚನಾ Curl_peer *peer)
{
  ಪ್ರತ್ಯಾಗಮ cf_dns_add_resolve(data, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_https_rrinfo *Curl_conn_dns_get_https(ಸಂರಚನಾ Curl_easy *data,
                                                        int8_t sockindex,
                                                        ಸಂರಚನಾ Curl_peer *peer)
{
  ಸಂರಚನಾ Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ಪ್ರತಿ(; cf; cf = cf->next) {
    ಯದಿ(cf->cft == &Curl_cft_dns) {
      ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;
      ಯದಿ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ಯದಿ(ctx->dns)
          ಪ್ರತ್ಯಾಗಮ ctx->dns->hinfo;
        ಅನ್ಯಥಾ
          ಪ್ರತ್ಯಾಗಮ Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

bool Curl_conn_dns_resolved_https(ಸಂರಚನಾ Curl_easy *data,
                                  int8_t sockindex,
                                  ಸಂರಚನಾ Curl_peer *peer)
{
  ಸಂರಚನಾ Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ಪ್ರತಿ(; cf; cf = cf->next) {
    ಯದಿ(cf->cft == &Curl_cft_dns) {
      ಸಂರಚನಾ cf_dns_ctx *ctx = cf->ctx;
      ಯದಿ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ಯದಿ(ctx->dns)
          ಪ್ರತ್ಯಾಗಮ TRUE;
        ಅನ್ಯಥಾ
          ಪ್ರತ್ಯಾಗಮ Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

#endif /* USE_HTTPSRR */
