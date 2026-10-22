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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "vdns/dnscache.h"
#অন্তর্ভাব "vdns/httpsrr.h"
#অন্তর্ভাব "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#define CURL_HE_AAAA_AWAIT_MS    25

কাঠামো cf_dns_ctx {
  কাঠামো Curl_dns_entry *dns;
  কাঠামো Curl_peer *peer;
  CURLcode resolv_ফলাফল;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

স্থির কাঠামো cf_dns_ctx *cf_dns_ctx_create(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  কাঠামো cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx)
    ফেরত ফাঁকা;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    ধ্রুবক অক্ষর *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(তথ্য, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  ফেরত ctx;
}

স্থির শূন্য cf_dns_ctx_destroy(কাঠামো Curl_easy *তথ্য,
                               কাঠামো cf_dns_ctx *ctx)
{
  যদি(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(তথ্য, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
স্থির শূন্য cf_dns_report_addr(কাঠামো Curl_easy *তথ্য,
                               কাঠামো dynbuf *tmp,
                               ধ্রুবক অক্ষর *label,
                               পূর্ণসংখ্যা ai_family,
                               ধ্রুবক কাঠামো Curl_addrinfo *ai)
{
  অক্ষর buf[MAX_IPADR_LEN];
  ধ্রুবক অক্ষর *sep = "";
  CURLcode ফলাফল;

  curlx_dyn_reset(tmp);
  জন্য(; ai; ai = ai->ai_পরবর্তী) {
    যদি(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, আকার(buf));
      ফলাফল = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      যদি(ফলাফল) {
        infof(তথ্য, "too many IP, cannot show");
        ফেরত;
      }
      sep = ", ";
    }
  }

  infof(তথ্য, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

স্থির শূন্য cf_dns_report(কাঠামো Curl_cfilter *cf,
                          কাঠামো Curl_easy *তথ্য,
                          কাঠামো Curl_dns_entry *dns)
{
  কাঠামো cf_dns_ctx *ctx = cf->ctx;
  কাঠামো dynbuf tmp;

  যদি(!Curl_trc_is_verbose(তথ্য) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    ফেরত;

  যদি(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(তথ্য, cf, "resolved unix://%s", ctx->peer->hostname);
#অন্যথা
    DEBUGASSERT(0);
#endif
  }
  অন্যথা {
    curlx_dyn_init(&tmp, 1024);
    যদি(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(তথ্য, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(তথ্য, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(তথ্য, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    অন্যথা যদি(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      যদি(!dns->hinfo)
        infof(তথ্য, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      অন্যথা যদি(!Curl_httpsrr_applicable(তথ্য, dns->hinfo))
        infof(তথ্য, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      অন্যথা {
        CURLcode ফলাফল = Curl_httpsrr_print(&tmp, dns->hinfo);
        যদি(!ফলাফল)
          infof(তথ্য, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        অন্যথা
          infof(তথ্য, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#অন্যথা
#define cf_dns_report(x, y, z) Curl_nop_stmt
#endif

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
স্থির CURLcode cf_dns_start(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_dns_entry **pdns)
{
  কাঠামো cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(তথ্য);
  CURLcode ফলাফল = CURLE_OK;

  *pdns = ফাঁকা;

  CURL_TRC_CF(তথ্য, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->port);
  যদি(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  যদি(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    যদি(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    অন্যথা যদি(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    ফলাফল = Curl_resolv(তথ্য, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  অন্যথা যদি(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    ফলাফল = Curl_resolv_https(তথ্য, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  অন্যথা {
    failf(তথ্য, "unsupported DNS queries %x", ctx->dns_queries);
    ফেরত CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!ফলাফল || !*pdns);
  যদি(!ফলাফল) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    ফেরত CURLE_OK;
  }
  অন্যথা যদি(ফলাফল == CURLE_AGAIN) { /* async resolv in progress */
    ফেরত CURLE_OK;
  }
  অন্যথা যদি(ফলাফল == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(তথ্য, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(তথ্য, ফাঁকা, TIMER_STARTSINGLE));
    ফেরত CURLE_OPERATION_TIMEDOUT;
  }
  অন্যথা {
    DEBUGASSERT(ফলাফল);
    যদি(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(তথ্য, "Could not resolve: %s", ctx->peer->hostname);
    ফেরত ফলাফল;
  }
}

স্থির bool cf_dns_ready_to_connect(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_dns_ctx *ctx = cf->ctx;

  যদি(ctx->resolv_ফলাফল)
    ফেরত TRUE;
  অন্যথা যদি(ctx->dns)
    ফেরত TRUE;
#ifdef USE_CURL_ASYNC
  অন্যথা যদি(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ধ্রুবক কাঠামো curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    যদি(Curl_resolv_has_answers(তথ্য, ctx->resolv_id, CURL_DNSQ_AAAA))
      ফেরত TRUE;
    pnow = Curl_pgrs_now(তথ্য);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(তথ্য, ctx->resolv_id, pnow);
    যদি(remain_ms <= 0)
      ফেরত TRUE;
    CURL_TRC_CF(তথ্য, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(তথ্য, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    ফেরত FALSE;
  }
  অন্যথা {
    ফেরত TRUE;
  }
#অন্যথা
  (শূন্য)তথ্য;
  DEBUGASSERT(0); /* We should not come here */
  ফেরত FALSE;
#endif /* USE_CURL_ASYNC */
}

স্থির CURLcode cf_dns_connect(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               bool *done)
{
  কাঠামো cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  *done = FALSE;
  যদি(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_ফলাফল = cf_dns_start(cf, তথ্য, &ctx->dns);
  }

  যদি(!ctx->dns && !ctx->resolv_ফলাফল) {
    ctx->resolv_ফলাফল =
      Curl_resolv_take_ফলাফল(তথ্য, ctx->resolv_id, &ctx->dns);
  }

  যদি(ctx->resolv_ফলাফল && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(তথ্য, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (পূর্ণসংখ্যা)ctx->resolv_ফলাফল);
    ফেরত ctx->resolv_ফলাফল;
  }

  যদি(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    যদি((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(তথ্য, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, তথ্য, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  যদি(!cf_dns_ready_to_connect(cf, তথ্য)) {
    ফেরত CURLE_OK;
  }

  যদি(cf->পরবর্তী && !cf->পরবর্তী->connected) {
    bool sub_done;
    CURLcode ফলাফল = Curl_conn_cf_connect(cf->পরবর্তী, তথ্য, &sub_done);
    যদি(ফলাফল || !sub_done)
      ফেরত ফলাফল;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(তথ্য, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(তথ্য, ctx->resolv_id);
  ফেরত CURLE_OK;
}

স্থির শূন্য cf_dns_destroy(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(তথ্য, cf, "destroy");
  cf_dns_ctx_destroy(তথ্য, ctx);
}

স্থির CURLcode cf_dns_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      কাঠামো easy_pollset *ps)
{
#ifdef USE_CURL_ASYNC
  যদি(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    কাঠামো cf_dns_ctx *ctx = cf->ctx;
    যদি(!ctx->started) {
      CURL_TRC_CF(তথ্য, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_ফলাফল = cf_dns_start(cf, তথ্য, &ctx->dns);
      যদি(ctx->resolv_ফলাফল || ctx->dns) {
        Curl_multi_mark_dirty(তথ্য);
      }
    }
    ফেরত Curl_resolv_pollset(তথ্য, ps);
  }
#অন্যথা
  (শূন্য)cf;
  (শূন্য)তথ্য;
  (শূন্য)ps;
#endif
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_dns_cntrl(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  কাঠামো cf_dns_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)arg1;
  (শূন্য)arg2;
  নির্বাচন(event) {
  ক্ষেত্র CF_CTRL_তথ্য_DONE:
    যদি(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(তথ্য, &ctx->dns);
    }
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত ফলাফল;
}

কাঠামো Curl_cftype Curl_cft_dns = {
  "DNS",
  CF_TYPE_SETUP | CF_TYPE_DNS,
  CURL_LOG_LVL_NONE,
  cf_dns_destroy,
  cf_dns_connect,
  Curl_cf_def_shutdown,
  cf_dns_adjust_pollset,
  Curl_cf_def_তথ্য_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_dns_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

স্থির CURLcode cf_dns_create(কাঠামো Curl_cfilter **pcf,
                              কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো cf_dns_ctx *ctx;
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)তথ্য;
  ctx = cf_dns_ctx_create(তথ্য, peer, dns_queries, transport, for_proxy);
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = ফলাফল ? ফাঁকা : cf;
  যদি(ফলাফল)
    cf_dns_ctx_destroy(তথ্য, ctx);
  ফেরত ফলাফল;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
স্থির CURLcode cf_dns_add(কাঠামো Curl_easy *তথ্য,
                           কাঠামো connectdata *conn,
                           int8_t sockindex,
                           কাঠামো Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  bool for_proxy = FALSE;
  CURLcode ফলাফল;

  যদি(!peer)
    ফেরত CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  ফলাফল = cf_dns_create(&cf, তথ্য, peer, dns_queries, transport, for_proxy);
  যদি(ফলাফল)
    যাও out;
  Curl_conn_cf_add(তথ্য, conn, sockindex, cf);
out:
  ফেরত ফলাফল;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
স্থির CURLcode cf_dns_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল;

  ফলাফল = cf_dns_create(&cf, তথ্য, peer, dns_queries, transport, FALSE);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  Curl_conn_cf_ঢোকাও_after(cf_at, cf);
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_dns_add_resolve(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো connectdata *conn,
                                   int8_t sockindex,
                                   কাঠামো Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  কাঠামো Curl_cfilter *cf = তথ্য->conn->cfilter[sockindex];
  কাঠামো Curl_cfilter *cf_dns = ফাঁকা;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  যদি((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  জন্য(; cf && dns_queries; cf = cf->পরবর্তী) {
    যদি(cf->cft == &Curl_cft_dns) {
      কাঠামো cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      যদি(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        যদি(!dns_queries) { /* already there */
          ফেরত CURLE_OK;
        }
        অন্যথা যদি(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(তথ্য, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          ফেরত CURLE_OK;
        }
      }
    }
  }

  যদি(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    যদি(cf_dns)
      ফেরত cf_dns_ঢোকাও_after(cf_dns, তথ্য, peer, dns_queries, transport);
    অন্যথা
      ফেরত cf_dns_add(তথ্য, conn, sockindex, peer, dns_queries, transport);
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো connectdata *conn,
                                        int8_t sockindex,
                                        কাঠামো Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  ফেরত cf_dns_add_resolve(তথ্য, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_ফলাফল(কাঠামো connectdata *conn,
                                   int8_t sockindex,
                                   কাঠামো Curl_peer *peer)
{
  কাঠামো Curl_cfilter *cf = conn->cfilter[sockindex];
  জন্য(; cf; cf = cf->পরবর্তী) {
    যদি(cf->cft == &Curl_cft_dns) {
      কাঠামো cf_dns_ctx *ctx = cf->ctx;
      যদি(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        যদি(ctx->dns || ctx->resolv_ফলাফল)
          ফেরত ctx->resolv_ফলাফল;
        ফেরত CURLE_AGAIN;
      }
    }
  }
  ফেরত CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
ধ্রুবক কাঠামো Curl_addrinfo *Curl_conn_dns_get_ai(কাঠামো Curl_easy *তথ্য,
                                                 কাঠামো Curl_peer *peer,
                                                 int8_t sockindex,
                                                 পূর্ণসংখ্যা ai_family,
                                                 unsigned পূর্ণসংখ্যা সূচক)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো Curl_cfilter *cf = conn->cfilter[sockindex];

  জন্য(; cf; cf = cf->পরবর্তী) {
    যদি(cf->cft == &Curl_cft_dns) {
      কাঠামো cf_dns_ctx *ctx = cf->ctx;
      যদি(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(তথ্য, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    সূচক, peer->hostname, peer->port, ai_family, !!ctx->dns);
        যদি(ctx->resolv_ফলাফল)
          ফেরত ফাঁকা;
        অন্যথা যদি(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          যদি((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            ফেরত ফাঁকা;
#ifdef USE_IPV6
          যদি((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            ফেরত ফাঁকা;
#endif
          ফেরত Curl_addrinfo_get(ctx->dns->addr, ai_family, সূচক);
        }
        অন্যথা
          ফেরত Curl_resolv_get_ai(তথ্য, ctx->resolv_id, ai_family, সূচক);
      }
    }
  }
  ফেরত ফাঁকা;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো connectdata *conn,
                                         int8_t sockindex,
                                         কাঠামো Curl_peer *peer)
{
  ফেরত cf_dns_add_resolve(তথ্য, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ধ্রুবক কাঠামো Curl_https_rrinfo *Curl_conn_dns_get_https(কাঠামো Curl_easy *তথ্য,
                                                        int8_t sockindex,
                                                        কাঠামো Curl_peer *peer)
{
  কাঠামো Curl_cfilter *cf = তথ্য->conn->cfilter[sockindex];
  জন্য(; cf; cf = cf->পরবর্তী) {
    যদি(cf->cft == &Curl_cft_dns) {
      কাঠামো cf_dns_ctx *ctx = cf->ctx;
      যদি(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        যদি(ctx->dns)
          ফেরত ctx->dns->hinfo;
        অন্যথা
          ফেরত Curl_resolv_get_https(তথ্য, ctx->resolv_id);
      }
    }
  }
  ফেরত ফাঁকা;
}

bool Curl_conn_dns_resolved_https(কাঠামো Curl_easy *তথ্য,
                                  int8_t sockindex,
                                  কাঠামো Curl_peer *peer)
{
  কাঠামো Curl_cfilter *cf = তথ্য->conn->cfilter[sockindex];
  জন্য(; cf; cf = cf->পরবর্তী) {
    যদি(cf->cft == &Curl_cft_dns) {
      কাঠামো cf_dns_ctx *ctx = cf->ctx;
      যদি(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        যদি(ctx->dns)
          ফেরত TRUE;
        অন্যথা
          ফেরত Curl_resolv_knows_https(তথ্য, ctx->resolv_id);
      }
    }
  }
  ফেরত TRUE;
}

#endif /* USE_HTTPSRR */
