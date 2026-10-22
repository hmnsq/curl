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
#تعریف_کن CURL_HE_AAAA_AWAIT_MS    25

بنية cf_dns_ctx {
  بنية Curl_dns_مدخل *dns;
  بنية Curl_peer *peer;
  CURLcode resolv_نتیجه;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  بیت(started);
  بیت(announced);
  بیت(for_proxy);
};

ساكن بنية cf_dns_ctx *cf_dns_ctx_create(بنية Curl_easy *داده,
                                            بنية Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  بنية cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    أرجع عدم;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#اگر_تعریف_شده DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#پایان_اگر

  CURL_TRC_DNS(داده, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->پورت, ctx->transport);
  أرجع ctx;
}

ساكن فراغ cf_dns_ctx_destroy(بنية Curl_easy *داده,
                               بنية cf_dns_ctx *ctx)
{
  إذا(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_مدخل_unlink(داده, &ctx->dns);
    curlx_free(ctx);
  }
}

#اگر_تعریف_شده CURLVERBOSE
ساكن فراغ cf_dns_report_addr(بنية Curl_easy *داده,
                               بنية dynbuf *tmp,
                               ثابت حرف *label,
                               صحيح ai_family,
                               ثابت بنية Curl_addrinfo *ai)
{
  حرف buf[MAX_IPADR_LEN];
  ثابت حرف *sep = "";
  CURLcode نتیجه;

  curlx_dyn_reset(tmp);
  لكل(; ai; ai = ai->ai_بعدی) {
    إذا(ai->ai_family == ai_family) {
      Curl_printable_آدرس(ai, buf, حجم(buf));
      نتیجه = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      إذا(نتیجه) {
        infof(داده, "too many IP, cannot show");
        أرجع;
      }
      sep = ", ";
    }
  }

  infof(داده, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

ساكن فراغ cf_dns_report(بنية Curl_cfilter *cf,
                          بنية Curl_easy *داده,
                          بنية Curl_dns_مدخل *dns)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  بنية dynbuf tmp;

  إذا(!Curl_trc_is_verbose(داده) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    أرجع;

  إذا(ctx->peer->unix_socket) {
#اگر_تعریف_شده USE_UNIX_SOCKETS
    CURL_TRC_CF(داده, cf, "resolved unix://%s", ctx->peer->hostname);
#وإلا
    DEBUGASSERT(0);
#پایان_اگر
  }
  وإلا {
    curlx_dyn_init(&tmp, 1024);
    إذا(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(داده, "Host %s:%u was resolved.", dns->hostname, dns->پورت);
#اگر_تعریف_شده CURLRES_IPV6
      cf_dns_report_addr(داده, &tmp, "IPv6: ", AF_INET6, dns->addr);
#پایان_اگر
      cf_dns_report_addr(داده, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#اگر_تعریف_شده USE_HTTPSRR
    وإلا إذا(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      إذا(!dns->hinfo)
        infof(داده, "HTTPS-RR %s:%u: -", dns->hostname, dns->پورت);
      وإلا إذا(!Curl_httpsrr_applicable(داده, dns->hinfo))
        infof(داده, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->پورت);
      وإلا {
        CURLcode نتیجه = Curl_httpsrr_print(&tmp, dns->hinfo);
        إذا(!نتیجه)
          infof(داده, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->پورت, curlx_dyn_ptr(&tmp));
        وإلا
          infof(داده, "Error printing HTTPS-RR information");
      }
    }
#پایان_اگر
    curlx_dyn_free(&tmp);
  }
}
#وإلا
#تعریف_کن cf_dns_report(x, y, z) Curl_nop_stmt
#پایان_اگر

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
ساكن CURLcode cf_dns_شروع(بنية Curl_cfilter *cf,
                             بنية Curl_easy *داده,
                             بنية Curl_dns_مدخل **pdns)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(داده);
  CURLcode نتیجه = CURLE_OK;

  *pdns = عدم;

  CURL_TRC_CF(داده, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->پورت);
  إذا(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  إذا(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    إذا(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#اگر_تعریف_شده USE_IPV6
    وإلا إذا(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#پایان_اگر

    نتیجه = Curl_resolv(داده, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#اگر_تعریف_شده USE_HTTPSRR
  وإلا إذا(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    نتیجه = Curl_resolv_https(داده, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#پایان_اگر
  وإلا {
    failf(داده, "unsupported DNS queries %x", ctx->dns_queries);
    أرجع CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!نتیجه || !*pdns);
  إذا(!نتیجه) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    أرجع CURLE_OK;
  }
  وإلا إذا(نتیجه == CURLE_AGAIN) { /* async resolv in progress */
    أرجع CURLE_OK;
  }
  وإلا إذا(نتیجه == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(داده, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(داده, عدم, TIMER_STARTSINGLE));
    أرجع CURLE_OPERATION_TIMEDOUT;
  }
  وإلا {
    DEBUGASSERT(نتیجه);
    إذا(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(داده, "Could not resolve: %s", ctx->peer->hostname);
    أرجع نتیجه;
  }
}

ساكن bool cf_dns_ready_مقصد_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده)
{
  بنية cf_dns_ctx *ctx = cf->ctx;

  إذا(ctx->resolv_نتیجه)
    أرجع TRUE;
  وإلا إذا(ctx->dns)
    أرجع TRUE;
#اگر_تعریف_شده USE_CURL_ASYNC
  وإلا إذا(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ثابت بنية curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    إذا(Curl_resolv_has_answers(داده, ctx->resolv_id, CURL_DNSQ_AAAA))
      أرجع TRUE;
    pnow = Curl_pgrs_now(داده);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(داده, ctx->resolv_id, pnow);
    إذا(remain_ms <= 0)
      أرجع TRUE;
    CURL_TRC_CF(داده, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_مجموعه(داده, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    أرجع FALSE;
  }
  وإلا {
    أرجع TRUE;
  }
#وإلا
  (فراغ)داده;
  DEBUGASSERT(0); /* We should not come here */
  أرجع FALSE;
#پایان_اگر /* USE_CURL_ASYNC */
}

ساكن CURLcode cf_dns_connect(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               bool *انجامشده)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  *انجامشده = FALSE;
  إذا(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_نتیجه = cf_dns_شروع(cf, داده, &ctx->dns);
  }

  إذا(!ctx->dns && !ctx->resolv_نتیجه) {
    ctx->resolv_نتیجه =
      Curl_resolv_take_نتیجه(داده, ctx->resolv_id, &ctx->dns);
  }

  إذا(ctx->resolv_نتیجه && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(داده, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (صحيح)ctx->resolv_نتیجه);
    أرجع ctx->resolv_نتیجه;
  }

  إذا(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    إذا((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->بیتها.dns_resolved = TRUE;
      Curl_pgrs_زمان(داده, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, داده, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  إذا(!cf_dns_ready_مقصد_connect(cf, داده)) {
    أرجع CURLE_OK;
  }

  إذا(cf->بعدی && !cf->بعدی->connected) {
    bool sub_انجامشده;
    CURLcode نتیجه = Curl_conn_cf_connect(cf->بعدی, داده, &sub_انجامشده);
    إذا(نتیجه || !sub_انجامشده)
      أرجع نتیجه;
    DEBUGASSERT(sub_انجامشده);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(داده, cf, "connected filter chain below");
  *انجامشده = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(داده, ctx->resolv_id);
  أرجع CURLE_OK;
}

ساكن فراغ cf_dns_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(داده, cf, "destroy");
  cf_dns_ctx_destroy(داده, ctx);
}

ساكن CURLcode cf_dns_adjust_pollset(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده,
                                      بنية easy_pollset *ps)
{
#اگر_تعریف_شده USE_CURL_ASYNC
  إذا(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    بنية cf_dns_ctx *ctx = cf->ctx;
    إذا(!ctx->started) {
      CURL_TRC_CF(داده, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->پورت,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_نتیجه = cf_dns_شروع(cf, داده, &ctx->dns);
      إذا(ctx->resolv_نتیجه || ctx->dns) {
        Curl_multi_mark_dirty(داده);
      }
    }
    أرجع Curl_resolv_pollset(داده, ps);
  }
#وإلا
  (فراغ)cf;
  (فراغ)داده;
  (فراغ)ps;
#پایان_اگر
  أرجع CURLE_OK;
}

ساكن CURLcode cf_dns_cntrl(بنية Curl_cfilter *cf,
                             بنية Curl_easy *داده,
                             صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_داده_انجامشده:
    إذا(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_مدخل_unlink(داده, &ctx->dns);
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتیجه;
}

بنية Curl_cftype Curl_cft_dns = {
  "DNS",
  CF_TYPE_راهاندازی | CF_TYPE_DNS,
  CURL_LOG_LVL_NONE,
  cf_dns_destroy,
  cf_dns_connect,
  Curl_cf_def_shutdown,
  cf_dns_adjust_pollset,
  Curl_cf_def_داده_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_dns_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساكن CURLcode cf_dns_create(بنية Curl_cfilter **pcf,
                              بنية Curl_easy *داده,
                              بنية Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_dns_ctx *ctx;
  CURLcode نتیجه = CURLE_OK;

  (فراغ)داده;
  ctx = cf_dns_ctx_create(داده, peer, dns_queries, transport, for_proxy);
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  نتیجه = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

بیرون:
  *pcf = نتیجه ? عدم : cf;
  إذا(نتیجه)
    cf_dns_ctx_destroy(داده, ctx);
  أرجع نتیجه;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
ساكن CURLcode cf_dns_جمع_کن(بنية Curl_easy *داده,
                           بنية connectdata *conn,
                           int8_t sockindex,
                           بنية Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  بنية Curl_cfilter *cf = عدم;
  bool for_proxy = FALSE;
  CURLcode نتیجه;

  إذا(!peer)
    أرجع CURLE_FAILED_INIT;
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#پایان_اگر

  نتیجه = cf_dns_create(&cf, داده, peer, dns_queries, transport, for_proxy);
  إذا(نتیجه)
    اذهب بیرون;
  Curl_conn_cf_جمع_کن(داده, conn, sockindex, cf);
بیرون:
  أرجع نتیجه;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
ساكن CURLcode cf_dns_درج_after(بنية Curl_cfilter *cf_at,
                                    بنية Curl_easy *داده,
                                    بنية Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه;

  نتیجه = cf_dns_create(&cf, داده, peer, dns_queries, transport, FALSE);
  إذا(نتیجه)
    أرجع نتیجه;

  Curl_conn_cf_درج_after(cf_at, cf);
  أرجع CURLE_OK;
}

ساكن CURLcode cf_dns_جمع_کن_resolve(بنية Curl_easy *داده,
                                   بنية connectdata *conn,
                                   int8_t sockindex,
                                   بنية Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  بنية Curl_cfilter *cf = داده->conn->cfilter[sockindex];
  بنية Curl_cfilter *cf_dns = عدم;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  إذا((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  لكل(; cf && dns_queries; cf = cf->بعدی) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      إذا(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        إذا(!dns_queries) { /* already there */
          أرجع CURLE_OK;
        }
        وإلا إذا(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(داده, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->پورت);
          أرجع CURLE_OK;
        }
      }
    }
  }

  إذا(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    إذا(cf_dns)
      أرجع cf_dns_درج_after(cf_dns, داده, peer, dns_queries, transport);
    وإلا
      أرجع cf_dns_جمع_کن(داده, conn, sockindex, peer, dns_queries, transport);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_conn_dns_جمع_کن_addr_resolve(بنية Curl_easy *داده,
                                        بنية connectdata *conn,
                                        int8_t sockindex,
                                        بنية Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  أرجع cf_dns_جمع_کن_resolve(داده, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_نتیجه(بنية connectdata *conn,
                                   int8_t sockindex,
                                   بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = conn->cfilter[sockindex];
  لكل(; cf; cf = cf->بعدی) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        إذا(ctx->dns || ctx->resolv_نتیجه)
          أرجع ctx->resolv_نتیجه;
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
ثابت بنية Curl_addrinfo *Curl_conn_dns_get_ai(بنية Curl_easy *داده,
                                                 بنية Curl_peer *peer,
                                                 int8_t sockindex,
                                                 صحيح ai_family,
                                                 بیعلامت صحيح شاخص)
{
  بنية connectdata *conn = داده->conn;
  بنية Curl_cfilter *cf = conn->cfilter[sockindex];

  لكل(; cf; cf = cf->بعدی) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(داده, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    شاخص, peer->hostname, peer->پورت, ai_family, !!ctx->dns);
        إذا(ctx->resolv_نتیجه)
          أرجع عدم;
        وإلا إذا(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          إذا((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            أرجع عدم;
#اگر_تعریف_شده USE_IPV6
          إذا((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            أرجع عدم;
#پایان_اگر
          أرجع Curl_addrinfo_get(ctx->dns->addr, ai_family, شاخص);
        }
        وإلا
          أرجع Curl_resolv_get_ai(داده, ctx->resolv_id, ai_family, شاخص);
      }
    }
  }
  أرجع عدم;
}

#اگر_تعریف_شده USE_HTTPSRR
CURLcode Curl_conn_dns_جمع_کن_https_resolve(بنية Curl_easy *داده,
                                         بنية connectdata *conn,
                                         int8_t sockindex,
                                         بنية Curl_peer *peer)
{
  أرجع cf_dns_جمع_کن_resolve(داده, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ثابت بنية Curl_https_rrinfo *Curl_conn_dns_get_https(بنية Curl_easy *داده,
                                                        int8_t sockindex,
                                                        بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = داده->conn->cfilter[sockindex];
  لكل(; cf; cf = cf->بعدی) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        إذا(ctx->dns)
          أرجع ctx->dns->hinfo;
        وإلا
          أرجع Curl_resolv_get_https(داده, ctx->resolv_id);
      }
    }
  }
  أرجع عدم;
}

bool Curl_conn_dns_resolved_https(بنية Curl_easy *داده,
                                  int8_t sockindex,
                                  بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = داده->conn->cfilter[sockindex];
  لكل(; cf; cf = cf->بعدی) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        إذا(ctx->dns)
          أرجع TRUE;
        وإلا
          أرجع Curl_resolv_knows_https(داده, ctx->resolv_id);
      }
    }
  }
  أرجع TRUE;
}

#پایان_اگر /* USE_HTTPSRR */
