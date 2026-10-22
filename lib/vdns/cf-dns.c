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

সংরচনা cf_dns_ctx {
  সংরচনা Curl_dns_entry *dns;
  সংরচনা Curl_peer *peer;
  CURLcode resolv_result;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

স্থির সংরচনা cf_dns_ctx *cf_dns_ctx_create(সংরচনা Curl_easy *data,
                                            সংরচনা Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  সংরচনা cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx)
    প্রত্যাগম রিক্ত;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    ধ্রুব বর্ণ *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  প্রত্যাগম ctx;
}

স্থির শূন্য cf_dns_ctx_destroy(সংরচনা Curl_easy *data,
                               সংরচনা cf_dns_ctx *ctx)
{
  যদি(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
স্থির শূন্য cf_dns_report_addr(সংরচনা Curl_easy *data,
                               সংরচনা dynbuf *tmp,
                               ধ্রুব বর্ণ *label,
                               পূর্ণাঙ্ক ai_family,
                               ধ্রুব সংরচনা Curl_addrinfo *ai)
{
  বর্ণ buf[MAX_IPADR_LEN];
  ধ্রুব বর্ণ *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  প্রতি(; ai; ai = ai->ai_next) {
    যদি(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, পরিমাণ(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      যদি(result) {
        infof(data, "too many IP, cannot show");
        প্রত্যাগম;
      }
      sep = ", ";
    }
  }

  infof(data, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

স্থির শূন্য cf_dns_report(সংরচনা Curl_cfilter *cf,
                          সংরচনা Curl_easy *data,
                          সংরচনা Curl_dns_entry *dns)
{
  সংরচনা cf_dns_ctx *ctx = cf->ctx;
  সংরচনা dynbuf tmp;

  যদি(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    প্রত্যাগম;

  যদি(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#অন্যথা
    DEBUGASSERT(0);
#endif
  }
  অন্যথা {
    curlx_dyn_init(&tmp, 1024);
    যদি(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    অন্যথা যদি(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      যদি(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      অন্যথা যদি(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      অন্যথা {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        যদি(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        অন্যথা
          infof(data, "Error printing HTTPS-RR information");
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
স্থির CURLcode cf_dns_start(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             সংরচনা Curl_dns_entry **pdns)
{
  সংরচনা cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(data);
  CURLcode result = CURLE_OK;

  *pdns = রিক্ত;

  CURL_TRC_CF(data, cf, "[%s] cf_dns_start %s %s:%u",
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

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  অন্যথা যদি(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  অন্যথা {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    প্রত্যাগম CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  যদি(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    প্রত্যাগম CURLE_OK;
  }
  অন্যথা যদি(result == CURLE_AGAIN) { /* async resolv in progress */
    প্রত্যাগম CURLE_OK;
  }
  অন্যথা যদি(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, রিক্ত, TIMER_STARTSINGLE));
    প্রত্যাগম CURLE_OPERATION_TIMEDOUT;
  }
  অন্যথা {
    DEBUGASSERT(result);
    যদি(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    প্রত্যাগম result;
  }
}

স্থির bool cf_dns_ready_to_connect(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data)
{
  সংরচনা cf_dns_ctx *ctx = cf->ctx;

  যদি(ctx->resolv_result)
    প্রত্যাগম TRUE;
  অন্যথা যদি(ctx->dns)
    প্রত্যাগম TRUE;
#ifdef USE_CURL_ASYNC
  অন্যথা যদি(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ধ্রুব সংরচনা curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    যদি(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      প্রত্যাগম TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    যদি(remain_ms <= 0)
      প্রত্যাগম TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    প্রত্যাগম FALSE;
  }
  অন্যথা {
    প্রত্যাগম TRUE;
  }
#অন্যথা
  (শূন্য)data;
  DEBUGASSERT(0); /* We should not come here */
  প্রত্যাগম FALSE;
#endif /* USE_CURL_ASYNC */
}

স্থির CURLcode cf_dns_connect(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               bool *done)
{
  সংরচনা cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  যদি(cf->connected) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  *done = FALSE;
  যদি(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  যদি(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  যদি(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (পূর্ণাঙ্ক)ctx->resolv_result);
    প্রত্যাগম ctx->resolv_result;
  }

  যদি(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    যদি((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  যদি(!cf_dns_ready_to_connect(cf, data)) {
    প্রত্যাগম CURLE_OK;
  }

  যদি(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    যদি(result || !sub_done)
      প্রত্যাগম result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য cf_dns_destroy(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_dns_ctx_destroy(data, ctx);
}

স্থির CURLcode cf_dns_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      সংরচনা easy_pollset *ps)
{
#ifdef USE_CURL_ASYNC
  যদি(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    সংরচনা cf_dns_ctx *ctx = cf->ctx;
    যদি(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      যদি(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    প্রত্যাগম Curl_resolv_pollset(data, ps);
  }
#অন্যথা
  (শূন্য)cf;
  (শূন্য)data;
  (শূন্য)ps;
#endif
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode cf_dns_cntrl(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  সংরচনা cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (শূন্য)arg1;
  (শূন্য)arg2;
  বিকল্প(event) {
  পক্ষ CF_CTRL_DATA_DONE:
    যদি(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    ভঙ্গ;
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম result;
}

সংরচনা Curl_cftype Curl_cft_dns = {
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

স্থির CURLcode cf_dns_create(সংরচনা Curl_cfilter **pcf,
                              সংরচনা Curl_easy *data,
                              সংরচনা Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  সংরচনা cf_dns_ctx *ctx;
  CURLcode result = CURLE_OK;

  (শূন্য)data;
  ctx = cf_dns_ctx_create(data, peer, dns_queries, transport, for_proxy);
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? রিক্ত : cf;
  যদি(result)
    cf_dns_ctx_destroy(data, ctx);
  প্রত্যাগম result;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
স্থির CURLcode cf_dns_add(সংরচনা Curl_easy *data,
                           সংরচনা connectdata *conn,
                           int8_t sockindex,
                           সংরচনা Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  bool for_proxy = FALSE;
  CURLcode result;

  যদি(!peer)
    প্রত্যাগম CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  যদি(result)
    গচ্ছ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  প্রত্যাগম result;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
স্থির CURLcode cf_dns_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                    সংরচনা Curl_easy *data,
                                    সংরচনা Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result;

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, FALSE);
  যদি(result)
    প্রত্যাগম result;

  Curl_conn_cf_ঢোকাও_after(cf_at, cf);
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode cf_dns_add_resolve(সংরচনা Curl_easy *data,
                                   সংরচনা connectdata *conn,
                                   int8_t sockindex,
                                   সংরচনা Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  সংরচনা Curl_cfilter *cf = data->conn->cfilter[sockindex];
  সংরচনা Curl_cfilter *cf_dns = রিক্ত;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  যদি((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  প্রতি(; cf && dns_queries; cf = cf->next) {
    যদি(cf->cft == &Curl_cft_dns) {
      সংরচনা cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      যদি(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        যদি(!dns_queries) { /* already there */
          প্রত্যাগম CURLE_OK;
        }
        অন্যথা যদি(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          প্রত্যাগম CURLE_OK;
        }
      }
    }
  }

  যদি(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    যদি(cf_dns)
      প্রত্যাগম cf_dns_ঢোকাও_after(cf_dns, data, peer, dns_queries, transport);
    অন্যথা
      প্রত্যাগম cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(সংরচনা Curl_easy *data,
                                        সংরচনা connectdata *conn,
                                        int8_t sockindex,
                                        সংরচনা Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  প্রত্যাগম cf_dns_add_resolve(data, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_result(সংরচনা connectdata *conn,
                                   int8_t sockindex,
                                   সংরচনা Curl_peer *peer)
{
  সংরচনা Curl_cfilter *cf = conn->cfilter[sockindex];
  প্রতি(; cf; cf = cf->next) {
    যদি(cf->cft == &Curl_cft_dns) {
      সংরচনা cf_dns_ctx *ctx = cf->ctx;
      যদি(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        যদি(ctx->dns || ctx->resolv_result)
          প্রত্যাগম ctx->resolv_result;
        প্রত্যাগম CURLE_AGAIN;
      }
    }
  }
  প্রত্যাগম CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
ধ্রুব সংরচনা Curl_addrinfo *Curl_conn_dns_get_ai(সংরচনা Curl_easy *data,
                                                 সংরচনা Curl_peer *peer,
                                                 int8_t sockindex,
                                                 পূর্ণাঙ্ক ai_family,
                                                 unsigned পূর্ণাঙ্ক index)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা Curl_cfilter *cf = conn->cfilter[sockindex];

  প্রতি(; cf; cf = cf->next) {
    যদি(cf->cft == &Curl_cft_dns) {
      সংরচনা cf_dns_ctx *ctx = cf->ctx;
      যদি(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        যদি(ctx->resolv_result)
          প্রত্যাগম রিক্ত;
        অন্যথা যদি(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          যদি((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            প্রত্যাগম রিক্ত;
#ifdef USE_IPV6
          যদি((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            প্রত্যাগম রিক্ত;
#endif
          প্রত্যাগম Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        অন্যথা
          প্রত্যাগম Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  প্রত্যাগম রিক্ত;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(সংরচনা Curl_easy *data,
                                         সংরচনা connectdata *conn,
                                         int8_t sockindex,
                                         সংরচনা Curl_peer *peer)
{
  প্রত্যাগম cf_dns_add_resolve(data, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ধ্রুব সংরচনা Curl_https_rrinfo *Curl_conn_dns_get_https(সংরচনা Curl_easy *data,
                                                        int8_t sockindex,
                                                        সংরচনা Curl_peer *peer)
{
  সংরচনা Curl_cfilter *cf = data->conn->cfilter[sockindex];
  প্রতি(; cf; cf = cf->next) {
    যদি(cf->cft == &Curl_cft_dns) {
      সংরচনা cf_dns_ctx *ctx = cf->ctx;
      যদি(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        যদি(ctx->dns)
          প্রত্যাগম ctx->dns->hinfo;
        অন্যথা
          প্রত্যাগম Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  প্রত্যাগম রিক্ত;
}

bool Curl_conn_dns_resolved_https(সংরচনা Curl_easy *data,
                                  int8_t sockindex,
                                  সংরচনা Curl_peer *peer)
{
  সংরচনা Curl_cfilter *cf = data->conn->cfilter[sockindex];
  প্রতি(; cf; cf = cf->next) {
    যদি(cf->cft == &Curl_cft_dns) {
      সংরচনা cf_dns_ctx *ctx = cf->ctx;
      যদি(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        যদি(ctx->dns)
          প্রত্যাগম TRUE;
        অন্যথা
          প্রত্যাগম Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  প্রত্যাগম TRUE;
}

#endif /* USE_HTTPSRR */
