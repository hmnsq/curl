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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "vdns/dnscache.h"
#ଅନ୍ତର୍ଭାଵ "vdns/httpsrr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#define CURL_HE_AAAA_AWAIT_MS    25

ସଂରଚନା cf_dns_ctx {
  ସଂରଚନା Curl_dns_entry *dns;
  ସଂରଚନା Curl_peer *peer;
  CURLcode resolv_result;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

ସ୍ଥିର ସଂରଚନା cf_dns_ctx *cf_dns_ctx_create(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  ସଂରଚନା cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx)
    ଫେରନ୍ତୁ ଖାଲି;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    ଅଚଳ ଅକ୍ଷର *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  ଫେରନ୍ତୁ ctx;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_dns_ctx_destroy(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା cf_dns_ctx *ctx)
{
  ଯଦି(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
ସ୍ଥିର ଶୂନ୍ୟ cf_dns_report_addr(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା dynbuf *tmp,
                               ଅଚଳ ଅକ୍ଷର *label,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family,
                               ଅଚଳ ସଂରଚନା Curl_addrinfo *ai)
{
  ଅକ୍ଷର buf[MAX_IPADR_LEN];
  ଅଚଳ ଅକ୍ଷର *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  ପାଇଁ(; ai; ai = ai->ai_next) {
    ଯଦି(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, ଆକାର(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      ଯଦି(result) {
        infof(data, "too many IP, cannot show");
        ଫେରନ୍ତୁ;
      }
      sep = ", ";
    }
  }

  infof(data, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

ସ୍ଥିର ଶୂନ୍ୟ cf_dns_report(ସଂରଚନା Curl_cfilter *cf,
                          ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା Curl_dns_entry *dns)
{
  ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;
  ସଂରଚନା dynbuf tmp;

  ଯଦି(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    ଫେରନ୍ତୁ;

  ଯଦି(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#ଅନ୍ୟଥା
    DEBUGASSERT(0);
#endif
  }
  ଅନ୍ୟଥା {
    curlx_dyn_init(&tmp, 1024);
    ଯଦି(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    ଅନ୍ୟଥା ଯଦି(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      ଯଦି(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      ଅନ୍ୟଥା ଯଦି(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      ଅନ୍ୟଥା {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        ଯଦି(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        ଅନ୍ୟଥା
          infof(data, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#ଅନ୍ୟଥା
#define cf_dns_report(x, y, z) Curl_nop_stmt
#endif

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
ସ୍ଥିର CURLcode cf_dns_start(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_dns_entry **pdns)
{
  ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(data);
  CURLcode result = CURLE_OK;

  *pdns = ଖାଲି;

  CURL_TRC_CF(data, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->port);
  ଯଦି(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  ଯଦି(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ଯଦି(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    ଅନ୍ୟଥା ଯଦି(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  ଅନ୍ୟଥା ଯଦି(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  ଅନ୍ୟଥା {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  ଯଦି(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଅନ୍ୟଥା ଯଦି(result == CURLE_AGAIN) { /* async resolv in progress */
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଅନ୍ୟଥା ଯଦି(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, ଖାଲି, TIMER_STARTSINGLE));
    ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
  }
  ଅନ୍ୟଥା {
    DEBUGASSERT(result);
    ଯଦି(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    ଫେରନ୍ତୁ result;
  }
}

ସ୍ଥିର bool cf_dns_ready_to_connect(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;

  ଯଦି(ctx->resolv_result)
    ଫେରନ୍ତୁ TRUE;
  ଅନ୍ୟଥା ଯଦି(ctx->dns)
    ଫେରନ୍ତୁ TRUE;
#ifdef USE_CURL_ASYNC
  ଅନ୍ୟଥା ଯଦି(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ଅଚଳ ସଂରଚନା curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    ଯଦି(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      ଫେରନ୍ତୁ TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    ଯଦି(remain_ms <= 0)
      ଫେରନ୍ତୁ TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    ଫେରନ୍ତୁ FALSE;
  }
  ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ TRUE;
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(0); /* We should not come here */
  ଫେରନ୍ତୁ FALSE;
#endif /* USE_CURL_ASYNC */
}

ସ୍ଥିର CURLcode cf_dns_connect(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               bool *done)
{
  ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *done = FALSE;
  ଯଦି(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  ଯଦି(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  ଯଦି(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ctx->resolv_result);
    ଫେରନ୍ତୁ ctx->resolv_result;
  }

  ଯଦି(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    ଯଦି((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  ଯଦି(!cf_dns_ready_to_connect(cf, data)) {
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଯଦି(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    ଯଦି(result || !sub_done)
      ଫେରନ୍ତୁ result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_dns_destroy(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_dns_ctx_destroy(data, ctx);
}

ସ୍ଥିର CURLcode cf_dns_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା easy_pollset *ps)
{
#ifdef USE_CURL_ASYNC
  ଯଦି(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;
    ଯଦି(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      ଯଦି(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    ଫେରନ୍ତୁ Curl_resolv_pollset(data, ps);
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)ps;
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_dns_cntrl(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)arg1;
  (ଶୂନ୍ୟ)arg2;
  ବାଛ(event) {
  ସ୍ଥିତି CF_CTRL_DATA_DONE:
    ଯଦି(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସଂରଚନା Curl_cftype Curl_cft_dns = {
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

ସ୍ଥିର CURLcode cf_dns_create(ସଂରଚନା Curl_cfilter **pcf,
                              ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା cf_dns_ctx *ctx;
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)data;
  ctx = cf_dns_ctx_create(data, peer, dns_queries, transport, for_proxy);
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? ଖାଲି : cf;
  ଯଦି(result)
    cf_dns_ctx_destroy(data, ctx);
  ଫେରନ୍ତୁ result;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
ସ୍ଥିର CURLcode cf_dns_add(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା connectdata *conn,
                           int8_t sockindex,
                           ସଂରଚନା Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  bool for_proxy = FALSE;
  CURLcode result;

  ଯଦି(!peer)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  ଯଦି(result)
    ଯାଅ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  ଫେରନ୍ତୁ result;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
ସ୍ଥିର CURLcode cf_dns_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result;

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, FALSE);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_dns_add_resolve(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା connectdata *conn,
                                   int8_t sockindex,
                                   ସଂରଚନା Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  ସଂରଚନା Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ସଂରଚନା Curl_cfilter *cf_dns = ଖାଲି;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  ଯଦି((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  ପାଇଁ(; cf && dns_queries; cf = cf->next) {
    ଯଦି(cf->cft == &Curl_cft_dns) {
      ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      ଯଦି(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        ଯଦି(!dns_queries) { /* already there */
          ଫେରନ୍ତୁ CURLE_OK;
        }
        ଅନ୍ୟଥା ଯଦି(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          ଫେରନ୍ତୁ CURLE_OK;
        }
      }
    }
  }

  ଯଦି(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    ଯଦି(cf_dns)
      ଫେରନ୍ତୁ cf_dns_ଭର୍ତ୍ତି_after(cf_dns, data, peer, dns_queries, transport);
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା connectdata *conn,
                                        int8_t sockindex,
                                        ସଂରଚନା Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  ଫେରନ୍ତୁ cf_dns_add_resolve(data, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_result(ସଂରଚନା connectdata *conn,
                                   int8_t sockindex,
                                   ସଂରଚନା Curl_peer *peer)
{
  ସଂରଚନା Curl_cfilter *cf = conn->cfilter[sockindex];
  ପାଇଁ(; cf; cf = cf->next) {
    ଯଦି(cf->cft == &Curl_cft_dns) {
      ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;
      ଯଦି(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        ଯଦି(ctx->dns || ctx->resolv_result)
          ଫେରନ୍ତୁ ctx->resolv_result;
        ଫେରନ୍ତୁ CURLE_AGAIN;
      }
    }
  }
  ଫେରନ୍ତୁ CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
ଅଚଳ ସଂରଚନା Curl_addrinfo *Curl_conn_dns_get_ai(ସଂରଚନା Curl_easy *data,
                                                 ସଂରଚନା Curl_peer *peer,
                                                 int8_t sockindex,
                                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family,
                                                 unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା index)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା Curl_cfilter *cf = conn->cfilter[sockindex];

  ପାଇଁ(; cf; cf = cf->next) {
    ଯଦି(cf->cft == &Curl_cft_dns) {
      ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;
      ଯଦି(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        ଯଦି(ctx->resolv_result)
          ଫେରନ୍ତୁ ଖାଲି;
        ଅନ୍ୟଥା ଯଦି(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          ଯଦି((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            ଫେରନ୍ତୁ ଖାଲି;
#ifdef USE_IPV6
          ଯଦି((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            ଫେରନ୍ତୁ ଖାଲି;
#endif
          ଫେରନ୍ତୁ Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        ଅନ୍ୟଥା
          ଫେରନ୍ତୁ Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା connectdata *conn,
                                         int8_t sockindex,
                                         ସଂରଚନା Curl_peer *peer)
{
  ଫେରନ୍ତୁ cf_dns_add_resolve(data, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ଅଚଳ ସଂରଚନା Curl_https_rrinfo *Curl_conn_dns_get_https(ସଂରଚନା Curl_easy *data,
                                                        int8_t sockindex,
                                                        ସଂରଚନା Curl_peer *peer)
{
  ସଂରଚନା Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ପାଇଁ(; cf; cf = cf->next) {
    ଯଦି(cf->cft == &Curl_cft_dns) {
      ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;
      ଯଦି(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ଯଦି(ctx->dns)
          ଫେରନ୍ତୁ ctx->dns->hinfo;
        ଅନ୍ୟଥା
          ଫେରନ୍ତୁ Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

bool Curl_conn_dns_resolved_https(ସଂରଚନା Curl_easy *data,
                                  int8_t sockindex,
                                  ସଂରଚନା Curl_peer *peer)
{
  ସଂରଚନା Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ପାଇଁ(; cf; cf = cf->next) {
    ଯଦି(cf->cft == &Curl_cft_dns) {
      ସଂରଚନା cf_dns_ctx *ctx = cf->ctx;
      ଯଦି(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ଯଦି(ctx->dns)
          ଫେରନ୍ତୁ TRUE;
        ଅନ୍ୟଥା
          ଫେରନ୍ତୁ Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  ଫେରନ୍ତୁ TRUE;
}

#endif /* USE_HTTPSRR */
