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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "vdns/dnscache.h"
#అన్తర్భావ "vdns/httpsrr.h"
#అన్తర్భావ "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#నిర్వచించు CURL_HE_AAAA_AWAIT_MS    25

నిర్మాణం cf_dns_ctx {
  నిర్మాణం Curl_dns_నమోదు *dns;
  నిర్మాణం Curl_peer *peer;
  CURLcode resolv_ఫలితం;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  బిట్టు(started);
  బిట్టు(announced);
  బిట్టు(for_proxy);
};

స్థిర నిర్మాణం cf_dns_ctx *cf_dns_ctx_create(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  నిర్మాణం cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx)
    తిరిగి ఖాళీ;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#నిర్వచితమైతే DEBUGBUILD
  {
    స్థిరరాశి అక్షరం *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#అయితే_ముగింపు

  CURL_TRC_DNS(దత్తాంశం, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->పోర్టు, ctx->transport);
  తిరిగి ctx;
}

స్థిర శూన్య cf_dns_ctx_destroy(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం cf_dns_ctx *ctx)
{
  ఒకవేళ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_నమోదు_unlink(దత్తాంశం, &ctx->dns);
    curlx_free(ctx);
  }
}

#నిర్వచితమైతే CURLVERBOSE
స్థిర శూన్య cf_dns_report_addr(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం dynbuf *tmp,
                               స్థిరరాశి అక్షరం *label,
                               పూర్ణాంకం ai_family,
                               స్థిరరాశి నిర్మాణం Curl_addrinfo *ai)
{
  అక్షరం buf[MAX_IPADR_LEN];
  స్థిరరాశి అక్షరం *sep = "";
  CURLcode ఫలితం;

  curlx_dyn_reset(tmp);
  కొరకు(; ai; ai = ai->ai_తదుపరి) {
    ఒకవేళ(ai->ai_family == ai_family) {
      Curl_printable_చిరునామా(ai, buf, పరిమాణం(buf));
      ఫలితం = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      ఒకవేళ(ఫలితం) {
        infof(దత్తాంశం, "too many IP, cannot show");
        తిరిగి;
      }
      sep = ", ";
    }
  }

  infof(దత్తాంశం, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

స్థిర శూన్య cf_dns_report(నిర్మాణం Curl_cfilter *cf,
                          నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం Curl_dns_నమోదు *dns)
{
  నిర్మాణం cf_dns_ctx *ctx = cf->ctx;
  నిర్మాణం dynbuf tmp;

  ఒకవేళ(!Curl_trc_is_verbose(దత్తాంశం) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    తిరిగి;

  ఒకవేళ(ctx->peer->unix_socket) {
#నిర్వచితమైతే USE_UNIX_SOCKETS
    CURL_TRC_CF(దత్తాంశం, cf, "resolved unix://%s", ctx->peer->hostname);
#లేకపోతే
    DEBUGASSERT(0);
#అయితే_ముగింపు
  }
  లేకపోతే {
    curlx_dyn_init(&tmp, 1024);
    ఒకవేళ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(దత్తాంశం, "Host %s:%u was resolved.", dns->hostname, dns->పోర్టు);
#నిర్వచితమైతే CURLRES_IPV6
      cf_dns_report_addr(దత్తాంశం, &tmp, "IPv6: ", AF_INET6, dns->addr);
#అయితే_ముగింపు
      cf_dns_report_addr(దత్తాంశం, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#నిర్వచితమైతే USE_HTTPSRR
    లేకపోతే ఒకవేళ(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      ఒకవేళ(!dns->hinfo)
        infof(దత్తాంశం, "HTTPS-RR %s:%u: -", dns->hostname, dns->పోర్టు);
      లేకపోతే ఒకవేళ(!Curl_httpsrr_applicable(దత్తాంశం, dns->hinfo))
        infof(దత్తాంశం, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->పోర్టు);
      లేకపోతే {
        CURLcode ఫలితం = Curl_httpsrr_print(&tmp, dns->hinfo);
        ఒకవేళ(!ఫలితం)
          infof(దత్తాంశం, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->పోర్టు, curlx_dyn_ptr(&tmp));
        లేకపోతే
          infof(దత్తాంశం, "Error printing HTTPS-RR information");
      }
    }
#అయితే_ముగింపు
    curlx_dyn_free(&tmp);
  }
}
#లేకపోతే
#నిర్వచించు cf_dns_report(x, y, z) Curl_nop_stmt
#అయితే_ముగింపు

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
స్థిర CURLcode cf_dns_ప్రారంభం(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_dns_నమోదు **pdns)
{
  నిర్మాణం cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(దత్తాంశం);
  CURLcode ఫలితం = CURLE_OK;

  *pdns = ఖాళీ;

  CURL_TRC_CF(దత్తాంశం, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->పోర్టు);
  ఒకవేళ(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  ఒకవేళ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ఒకవేళ(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#నిర్వచితమైతే USE_IPV6
    లేకపోతే ఒకవేళ(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#అయితే_ముగింపు

    ఫలితం = Curl_resolv(దత్తాంశం, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#నిర్వచితమైతే USE_HTTPSRR
  లేకపోతే ఒకవేళ(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    ఫలితం = Curl_resolv_https(దత్తాంశం, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#అయితే_ముగింపు
  లేకపోతే {
    failf(దత్తాంశం, "unsupported DNS queries %x", ctx->dns_queries);
    తిరిగి CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!ఫలితం || !*pdns);
  ఒకవేళ(!ఫలితం) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    తిరిగి CURLE_OK;
  }
  లేకపోతే ఒకవేళ(ఫలితం == CURLE_AGAIN) { /* async resolv in progress */
    తిరిగి CURLE_OK;
  }
  లేకపోతే ఒకవేళ(ఫలితం == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(దత్తాంశం, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(దత్తాంశం, ఖాళీ, TIMER_STARTSINGLE));
    తిరిగి CURLE_OPERATION_TIMEDOUT;
  }
  లేకపోతే {
    DEBUGASSERT(ఫలితం);
    ఒకవేళ(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(దత్తాంశం, "Could not resolve: %s", ctx->peer->hostname);
    తిరిగి ఫలితం;
  }
}

స్థిర bool cf_dns_ready_గమ్యం_connect(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_dns_ctx *ctx = cf->ctx;

  ఒకవేళ(ctx->resolv_ఫలితం)
    తిరిగి TRUE;
  లేకపోతే ఒకవేళ(ctx->dns)
    తిరిగి TRUE;
#నిర్వచితమైతే USE_CURL_ASYNC
  లేకపోతే ఒకవేళ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    స్థిరరాశి నిర్మాణం curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    ఒకవేళ(Curl_resolv_has_answers(దత్తాంశం, ctx->resolv_id, CURL_DNSQ_AAAA))
      తిరిగి TRUE;
    pnow = Curl_pgrs_now(దత్తాంశం);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(దత్తాంశం, ctx->resolv_id, pnow);
    ఒకవేళ(remain_ms <= 0)
      తిరిగి TRUE;
    CURL_TRC_CF(దత్తాంశం, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_సమితి(దత్తాంశం, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    తిరిగి FALSE;
  }
  లేకపోతే {
    తిరిగి TRUE;
  }
#లేకపోతే
  (శూన్య)దత్తాంశం;
  DEBUGASSERT(0); /* We should not come here */
  తిరిగి FALSE;
#అయితే_ముగింపు /* USE_CURL_ASYNC */
}

స్థిర CURLcode cf_dns_connect(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               bool *పూర్తి)
{
  నిర్మాణం cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  *పూర్తి = FALSE;
  ఒకవేళ(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_ఫలితం = cf_dns_ప్రారంభం(cf, దత్తాంశం, &ctx->dns);
  }

  ఒకవేళ(!ctx->dns && !ctx->resolv_ఫలితం) {
    ctx->resolv_ఫలితం =
      Curl_resolv_take_ఫలితం(దత్తాంశం, ctx->resolv_id, &ctx->dns);
  }

  ఒకవేళ(ctx->resolv_ఫలితం && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(దత్తాంశం, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (పూర్ణాంకం)ctx->resolv_ఫలితం);
    తిరిగి ctx->resolv_ఫలితం;
  }

  ఒకవేళ(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    ఒకవేళ((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->బిట్లు.dns_resolved = TRUE;
      Curl_pgrs_సమయం(దత్తాంశం, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, దత్తాంశం, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  ఒకవేళ(!cf_dns_ready_గమ్యం_connect(cf, దత్తాంశం)) {
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(cf->తదుపరి && !cf->తదుపరి->connected) {
    bool sub_పూర్తి;
    CURLcode ఫలితం = Curl_conn_cf_connect(cf->తదుపరి, దత్తాంశం, &sub_పూర్తి);
    ఒకవేళ(ఫలితం || !sub_పూర్తి)
      తిరిగి ఫలితం;
    DEBUGASSERT(sub_పూర్తి);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(దత్తాంశం, cf, "connected filter chain below");
  *పూర్తి = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(దత్తాంశం, ctx->resolv_id);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cf_dns_destroy(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(దత్తాంశం, cf, "destroy");
  cf_dns_ctx_destroy(దత్తాంశం, ctx);
}

స్థిర CURLcode cf_dns_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం easy_pollset *ps)
{
#నిర్వచితమైతే USE_CURL_ASYNC
  ఒకవేళ(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    నిర్మాణం cf_dns_ctx *ctx = cf->ctx;
    ఒకవేళ(!ctx->started) {
      CURL_TRC_CF(దత్తాంశం, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->పోర్టు,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_ఫలితం = cf_dns_ప్రారంభం(cf, దత్తాంశం, &ctx->dns);
      ఒకవేళ(ctx->resolv_ఫలితం || ctx->dns) {
        Curl_multi_mark_dirty(దత్తాంశం);
      }
    }
    తిరిగి Curl_resolv_pollset(దత్తాంశం, ps);
  }
#లేకపోతే
  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
  (శూన్య)ps;
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_dns_cntrl(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  నిర్మాణం cf_dns_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)arg1;
  (శూన్య)arg2;
  ఎంపిక(event) {
  స్థితి CF_CTRL_దత్తాంశం_పూర్తి:
    ఒకవేళ(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_నమోదు_unlink(దత్తాంశం, &ctx->dns);
    }
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి ఫలితం;
}

నిర్మాణం Curl_cftype Curl_cft_dns = {
  "DNS",
  CF_TYPE_అమరిక | CF_TYPE_DNS,
  CURL_LOG_LVL_NONE,
  cf_dns_destroy,
  cf_dns_connect,
  Curl_cf_def_shutdown,
  cf_dns_adjust_pollset,
  Curl_cf_def_దత్తాంశం_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_dns_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

స్థిర CURLcode cf_dns_create(నిర్మాణం Curl_cfilter **pcf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం cf_dns_ctx *ctx;
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)దత్తాంశం;
  ctx = cf_dns_ctx_create(దత్తాంశం, peer, dns_queries, transport, for_proxy);
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

బహిర్గతం:
  *pcf = ఫలితం ? ఖాళీ : cf;
  ఒకవేళ(ఫలితం)
    cf_dns_ctx_destroy(దత్తాంశం, ctx);
  తిరిగి ఫలితం;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
స్థిర CURLcode cf_dns_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం connectdata *conn,
                           int8_t sockindex,
                           నిర్మాణం Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  bool for_proxy = FALSE;
  CURLcode ఫలితం;

  ఒకవేళ(!peer)
    తిరిగి CURLE_FAILED_INIT;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#అయితే_ముగింపు

  ఫలితం = cf_dns_create(&cf, దత్తాంశం, peer, dns_queries, transport, for_proxy);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  Curl_conn_cf_కూడు(దత్తాంశం, conn, sockindex, cf);
బహిర్గతం:
  తిరిగి ఫలితం;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
స్థిర CURLcode cf_dns_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం;

  ఫలితం = cf_dns_create(&cf, దత్తాంశం, peer, dns_queries, transport, FALSE);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  Curl_conn_cf_చొప్పించు_after(cf_at, cf);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_dns_కూడు_resolve(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం connectdata *conn,
                                   int8_t sockindex,
                                   నిర్మాణం Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  నిర్మాణం Curl_cfilter *cf = దత్తాంశం->conn->cfilter[sockindex];
  నిర్మాణం Curl_cfilter *cf_dns = ఖాళీ;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  ఒకవేళ((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  కొరకు(; cf && dns_queries; cf = cf->తదుపరి) {
    ఒకవేళ(cf->cft == &Curl_cft_dns) {
      నిర్మాణం cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      ఒకవేళ(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        ఒకవేళ(!dns_queries) { /* already there */
          తిరిగి CURLE_OK;
        }
        లేకపోతే ఒకవేళ(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(దత్తాంశం, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->పోర్టు);
          తిరిగి CURLE_OK;
        }
      }
    }
  }

  ఒకవేళ(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    ఒకవేళ(cf_dns)
      తిరిగి cf_dns_చొప్పించు_after(cf_dns, దత్తాంశం, peer, dns_queries, transport);
    లేకపోతే
      తిరిగి cf_dns_కూడు(దత్తాంశం, conn, sockindex, peer, dns_queries, transport);
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_conn_dns_కూడు_addr_resolve(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం connectdata *conn,
                                        int8_t sockindex,
                                        నిర్మాణం Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  తిరిగి cf_dns_కూడు_resolve(దత్తాంశం, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_ఫలితం(నిర్మాణం connectdata *conn,
                                   int8_t sockindex,
                                   నిర్మాణం Curl_peer *peer)
{
  నిర్మాణం Curl_cfilter *cf = conn->cfilter[sockindex];
  కొరకు(; cf; cf = cf->తదుపరి) {
    ఒకవేళ(cf->cft == &Curl_cft_dns) {
      నిర్మాణం cf_dns_ctx *ctx = cf->ctx;
      ఒకవేళ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        ఒకవేళ(ctx->dns || ctx->resolv_ఫలితం)
          తిరిగి ctx->resolv_ఫలితం;
        తిరిగి CURLE_AGAIN;
      }
    }
  }
  తిరిగి CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
స్థిరరాశి నిర్మాణం Curl_addrinfo *Curl_conn_dns_get_ai(నిర్మాణం Curl_easy *దత్తాంశం,
                                                 నిర్మాణం Curl_peer *peer,
                                                 int8_t sockindex,
                                                 పూర్ణాంకం ai_family,
                                                 గుర్తులేని పూర్ణాంకం సూచీ)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం Curl_cfilter *cf = conn->cfilter[sockindex];

  కొరకు(; cf; cf = cf->తదుపరి) {
    ఒకవేళ(cf->cft == &Curl_cft_dns) {
      నిర్మాణం cf_dns_ctx *ctx = cf->ctx;
      ఒకవేళ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(దత్తాంశం, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    సూచీ, peer->hostname, peer->పోర్టు, ai_family, !!ctx->dns);
        ఒకవేళ(ctx->resolv_ఫలితం)
          తిరిగి ఖాళీ;
        లేకపోతే ఒకవేళ(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          ఒకవేళ((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            తిరిగి ఖాళీ;
#నిర్వచితమైతే USE_IPV6
          ఒకవేళ((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            తిరిగి ఖాళీ;
#అయితే_ముగింపు
          తిరిగి Curl_addrinfo_get(ctx->dns->addr, ai_family, సూచీ);
        }
        లేకపోతే
          తిరిగి Curl_resolv_get_ai(దత్తాంశం, ctx->resolv_id, ai_family, సూచీ);
      }
    }
  }
  తిరిగి ఖాళీ;
}

#నిర్వచితమైతే USE_HTTPSRR
CURLcode Curl_conn_dns_కూడు_https_resolve(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం connectdata *conn,
                                         int8_t sockindex,
                                         నిర్మాణం Curl_peer *peer)
{
  తిరిగి cf_dns_కూడు_resolve(దత్తాంశం, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
స్థిరరాశి నిర్మాణం Curl_https_rrinfo *Curl_conn_dns_get_https(నిర్మాణం Curl_easy *దత్తాంశం,
                                                        int8_t sockindex,
                                                        నిర్మాణం Curl_peer *peer)
{
  నిర్మాణం Curl_cfilter *cf = దత్తాంశం->conn->cfilter[sockindex];
  కొరకు(; cf; cf = cf->తదుపరి) {
    ఒకవేళ(cf->cft == &Curl_cft_dns) {
      నిర్మాణం cf_dns_ctx *ctx = cf->ctx;
      ఒకవేళ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ఒకవేళ(ctx->dns)
          తిరిగి ctx->dns->hinfo;
        లేకపోతే
          తిరిగి Curl_resolv_get_https(దత్తాంశం, ctx->resolv_id);
      }
    }
  }
  తిరిగి ఖాళీ;
}

bool Curl_conn_dns_resolved_https(నిర్మాణం Curl_easy *దత్తాంశం,
                                  int8_t sockindex,
                                  నిర్మాణం Curl_peer *peer)
{
  నిర్మాణం Curl_cfilter *cf = దత్తాంశం->conn->cfilter[sockindex];
  కొరకు(; cf; cf = cf->తదుపరి) {
    ఒకవేళ(cf->cft == &Curl_cft_dns) {
      నిర్మాణం cf_dns_ctx *ctx = cf->ctx;
      ఒకవేళ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ఒకవేళ(ctx->dns)
          తిరిగి TRUE;
        లేకపోతే
          తిరిగి Curl_resolv_knows_https(దత్తాంశం, ctx->resolv_id);
      }
    }
  }
  తిరిగి TRUE;
}

#అయితే_ముగింపు /* USE_HTTPSRR */
