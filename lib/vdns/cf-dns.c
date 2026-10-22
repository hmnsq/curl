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
#تعریف_کړه CURL_HE_AAAA_AWAIT_MS    25

بنية cf_dns_ctx {
  بنية Curl_dns_ثبت *dns;
  بنية Curl_peer *peer;
  CURLcode resolv_پایله;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  بټ(started);
  بټ(announced);
  بټ(for_proxy);
};

ساكن بنية cf_dns_ctx *cf_dns_ctx_create(بنية Curl_easy *معلومات,
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
#که_تعریف_شوی DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_شمېره(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#د_که_پای

  CURL_TRC_DNS(معلومات, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->پورټ, ctx->transport);
  أرجع ctx;
}

ساكن فراغ cf_dns_ctx_destroy(بنية Curl_easy *معلومات,
                               بنية cf_dns_ctx *ctx)
{
  إذا(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_ثبت_unlink(معلومات, &ctx->dns);
    curlx_free(ctx);
  }
}

#که_تعریف_شوی CURLVERBOSE
ساكن فراغ cf_dns_report_addr(بنية Curl_easy *معلومات,
                               بنية dynbuf *tmp,
                               ثابت حرف *label,
                               صحيح ai_family,
                               ثابت بنية Curl_addrinfo *ai)
{
  حرف buf[MAX_IPADR_LEN];
  ثابت حرف *sep = "";
  CURLcode پایله;

  curlx_dyn_reset(tmp);
  لكل(; ai; ai = ai->ai_بل) {
    إذا(ai->ai_family == ai_family) {
      Curl_printable_پته(ai, buf, حجم(buf));
      پایله = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      إذا(پایله) {
        infof(معلومات, "too many IP, cannot show");
        أرجع;
      }
      sep = ", ";
    }
  }

  infof(معلومات, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

ساكن فراغ cf_dns_report(بنية Curl_cfilter *cf,
                          بنية Curl_easy *معلومات,
                          بنية Curl_dns_ثبت *dns)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  بنية dynbuf tmp;

  إذا(!Curl_trc_is_verbose(معلومات) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    أرجع;

  إذا(ctx->peer->unix_socket) {
#که_تعریف_شوی USE_UNIX_SOCKETS
    CURL_TRC_CF(معلومات, cf, "resolved unix://%s", ctx->peer->hostname);
#وإلا
    DEBUGASSERT(0);
#د_که_پای
  }
  وإلا {
    curlx_dyn_init(&tmp, 1024);
    إذا(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(معلومات, "Host %s:%u was resolved.", dns->hostname, dns->پورټ);
#که_تعریف_شوی CURLRES_IPV6
      cf_dns_report_addr(معلومات, &tmp, "IPv6: ", AF_INET6, dns->addr);
#د_که_پای
      cf_dns_report_addr(معلومات, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#که_تعریف_شوی USE_HTTPSRR
    وإلا إذا(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      إذا(!dns->hinfo)
        infof(معلومات, "HTTPS-RR %s:%u: -", dns->hostname, dns->پورټ);
      وإلا إذا(!Curl_httpsrr_applicable(معلومات, dns->hinfo))
        infof(معلومات, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->پورټ);
      وإلا {
        CURLcode پایله = Curl_httpsrr_print(&tmp, dns->hinfo);
        إذا(!پایله)
          infof(معلومات, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->پورټ, curlx_dyn_ptr(&tmp));
        وإلا
          infof(معلومات, "Error printing HTTPS-RR information");
      }
    }
#د_که_پای
    curlx_dyn_free(&tmp);
  }
}
#وإلا
#تعریف_کړه cf_dns_report(x, y, z) Curl_nop_stmt
#د_که_پای

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
ساكن CURLcode cf_dns_پیل(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات,
                             بنية Curl_dns_ثبت **pdns)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(معلومات);
  CURLcode پایله = CURLE_OK;

  *pdns = عدم;

  CURL_TRC_CF(معلومات, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->پورټ);
  إذا(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  إذا(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    إذا(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#که_تعریف_شوی USE_IPV6
    وإلا إذا(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#د_که_پای

    پایله = Curl_resolv(معلومات, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#که_تعریف_شوی USE_HTTPSRR
  وإلا إذا(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    پایله = Curl_resolv_https(معلومات, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#د_که_پای
  وإلا {
    failf(معلومات, "unsupported DNS queries %x", ctx->dns_queries);
    أرجع CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!پایله || !*pdns);
  إذا(!پایله) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    أرجع CURLE_OK;
  }
  وإلا إذا(پایله == CURLE_AGAIN) { /* async resolv in progress */
    أرجع CURLE_OK;
  }
  وإلا إذا(پایله == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(معلومات, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(معلومات, عدم, TIMER_STARTSINGLE));
    أرجع CURLE_OPERATION_TIMEDOUT;
  }
  وإلا {
    DEBUGASSERT(پایله);
    إذا(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(معلومات, "Could not resolve: %s", ctx->peer->hostname);
    أرجع پایله;
  }
}

ساكن bool cf_dns_ready_منزل_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات)
{
  بنية cf_dns_ctx *ctx = cf->ctx;

  إذا(ctx->resolv_پایله)
    أرجع TRUE;
  وإلا إذا(ctx->dns)
    أرجع TRUE;
#که_تعریف_شوی USE_CURL_ASYNC
  وإلا إذا(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ثابت بنية curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    إذا(Curl_resolv_has_answers(معلومات, ctx->resolv_id, CURL_DNSQ_AAAA))
      أرجع TRUE;
    pnow = Curl_pgrs_now(معلومات);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(معلومات, ctx->resolv_id, pnow);
    إذا(remain_ms <= 0)
      أرجع TRUE;
    CURL_TRC_CF(معلومات, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_سېټ(معلومات, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    أرجع FALSE;
  }
  وإلا {
    أرجع TRUE;
  }
#وإلا
  (فراغ)معلومات;
  DEBUGASSERT(0); /* We should not come here */
  أرجع FALSE;
#د_که_پای /* USE_CURL_ASYNC */
}

ساكن CURLcode cf_dns_connect(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               bool *بشپړ)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  *بشپړ = FALSE;
  إذا(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_پایله = cf_dns_پیل(cf, معلومات, &ctx->dns);
  }

  إذا(!ctx->dns && !ctx->resolv_پایله) {
    ctx->resolv_پایله =
      Curl_resolv_take_پایله(معلومات, ctx->resolv_id, &ctx->dns);
  }

  إذا(ctx->resolv_پایله && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(معلومات, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (صحيح)ctx->resolv_پایله);
    أرجع ctx->resolv_پایله;
  }

  إذا(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    إذا((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->بټونه.dns_resolved = TRUE;
      Curl_pgrs_وخت(معلومات, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, معلومات, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  إذا(!cf_dns_ready_منزل_connect(cf, معلومات)) {
    أرجع CURLE_OK;
  }

  إذا(cf->بل && !cf->بل->connected) {
    bool sub_بشپړ;
    CURLcode پایله = Curl_conn_cf_connect(cf->بل, معلومات, &sub_بشپړ);
    إذا(پایله || !sub_بشپړ)
      أرجع پایله;
    DEBUGASSERT(sub_بشپړ);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(معلومات, cf, "connected filter chain below");
  *بشپړ = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(معلومات, ctx->resolv_id);
  أرجع CURLE_OK;
}

ساكن فراغ cf_dns_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(معلومات, cf, "destroy");
  cf_dns_ctx_destroy(معلومات, ctx);
}

ساكن CURLcode cf_dns_adjust_pollset(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      بنية easy_pollset *ps)
{
#که_تعریف_شوی USE_CURL_ASYNC
  إذا(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    بنية cf_dns_ctx *ctx = cf->ctx;
    إذا(!ctx->started) {
      CURL_TRC_CF(معلومات, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->پورټ,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_پایله = cf_dns_پیل(cf, معلومات, &ctx->dns);
      إذا(ctx->resolv_پایله || ctx->dns) {
        Curl_multi_mark_dirty(معلومات);
      }
    }
    أرجع Curl_resolv_pollset(معلومات, ps);
  }
#وإلا
  (فراغ)cf;
  (فراغ)معلومات;
  (فراغ)ps;
#د_که_پای
  أرجع CURLE_OK;
}

ساكن CURLcode cf_dns_cntrl(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات,
                             صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_معلومات_بشپړ:
    إذا(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_ثبت_unlink(معلومات, &ctx->dns);
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع پایله;
}

بنية Curl_cftype Curl_cft_dns = {
  "DNS",
  CF_TYPE_تیاری | CF_TYPE_DNS,
  CURL_LOG_LVL_NONE,
  cf_dns_destroy,
  cf_dns_connect,
  Curl_cf_def_shutdown,
  cf_dns_adjust_pollset,
  Curl_cf_def_معلومات_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_dns_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساكن CURLcode cf_dns_create(بنية Curl_cfilter **pcf,
                              بنية Curl_easy *معلومات,
                              بنية Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_dns_ctx *ctx;
  CURLcode پایله = CURLE_OK;

  (فراغ)معلومات;
  ctx = cf_dns_ctx_create(معلومات, peer, dns_queries, transport, for_proxy);
  إذا(!ctx) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  پایله = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

بهر:
  *pcf = پایله ? عدم : cf;
  إذا(پایله)
    cf_dns_ctx_destroy(معلومات, ctx);
  أرجع پایله;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
ساكن CURLcode cf_dns_جمع_کړه(بنية Curl_easy *معلومات,
                           بنية connectdata *conn,
                           int8_t sockindex,
                           بنية Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  بنية Curl_cfilter *cf = عدم;
  bool for_proxy = FALSE;
  CURLcode پایله;

  إذا(!peer)
    أرجع CURLE_FAILED_INIT;
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#د_که_پای

  پایله = cf_dns_create(&cf, معلومات, peer, dns_queries, transport, for_proxy);
  إذا(پایله)
    اذهب بهر;
  Curl_conn_cf_جمع_کړه(معلومات, conn, sockindex, cf);
بهر:
  أرجع پایله;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
ساكن CURLcode cf_dns_ورننباسه_after(بنية Curl_cfilter *cf_at,
                                    بنية Curl_easy *معلومات,
                                    بنية Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله;

  پایله = cf_dns_create(&cf, معلومات, peer, dns_queries, transport, FALSE);
  إذا(پایله)
    أرجع پایله;

  Curl_conn_cf_ورننباسه_after(cf_at, cf);
  أرجع CURLE_OK;
}

ساكن CURLcode cf_dns_جمع_کړه_resolve(بنية Curl_easy *معلومات,
                                   بنية connectdata *conn,
                                   int8_t sockindex,
                                   بنية Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  بنية Curl_cfilter *cf = معلومات->conn->cfilter[sockindex];
  بنية Curl_cfilter *cf_dns = عدم;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  إذا((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  لكل(; cf && dns_queries; cf = cf->بل) {
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
          CURL_TRC_DNS(معلومات, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->پورټ);
          أرجع CURLE_OK;
        }
      }
    }
  }

  إذا(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    إذا(cf_dns)
      أرجع cf_dns_ورننباسه_after(cf_dns, معلومات, peer, dns_queries, transport);
    وإلا
      أرجع cf_dns_جمع_کړه(معلومات, conn, sockindex, peer, dns_queries, transport);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_conn_dns_جمع_کړه_addr_resolve(بنية Curl_easy *معلومات,
                                        بنية connectdata *conn,
                                        int8_t sockindex,
                                        بنية Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  أرجع cf_dns_جمع_کړه_resolve(معلومات, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_پایله(بنية connectdata *conn,
                                   int8_t sockindex,
                                   بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = conn->cfilter[sockindex];
  لكل(; cf; cf = cf->بل) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        إذا(ctx->dns || ctx->resolv_پایله)
          أرجع ctx->resolv_پایله;
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
ثابت بنية Curl_addrinfo *Curl_conn_dns_get_ai(بنية Curl_easy *معلومات,
                                                 بنية Curl_peer *peer,
                                                 int8_t sockindex,
                                                 صحيح ai_family,
                                                 بینښه صحيح شاخص)
{
  بنية connectdata *conn = معلومات->conn;
  بنية Curl_cfilter *cf = conn->cfilter[sockindex];

  لكل(; cf; cf = cf->بل) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(معلومات, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    شاخص, peer->hostname, peer->پورټ, ai_family, !!ctx->dns);
        إذا(ctx->resolv_پایله)
          أرجع عدم;
        وإلا إذا(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          إذا((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            أرجع عدم;
#که_تعریف_شوی USE_IPV6
          إذا((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            أرجع عدم;
#د_که_پای
          أرجع Curl_addrinfo_get(ctx->dns->addr, ai_family, شاخص);
        }
        وإلا
          أرجع Curl_resolv_get_ai(معلومات, ctx->resolv_id, ai_family, شاخص);
      }
    }
  }
  أرجع عدم;
}

#که_تعریف_شوی USE_HTTPSRR
CURLcode Curl_conn_dns_جمع_کړه_https_resolve(بنية Curl_easy *معلومات,
                                         بنية connectdata *conn,
                                         int8_t sockindex,
                                         بنية Curl_peer *peer)
{
  أرجع cf_dns_جمع_کړه_resolve(معلومات, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ثابت بنية Curl_https_rrinfo *Curl_conn_dns_get_https(بنية Curl_easy *معلومات,
                                                        int8_t sockindex,
                                                        بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = معلومات->conn->cfilter[sockindex];
  لكل(; cf; cf = cf->بل) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        إذا(ctx->dns)
          أرجع ctx->dns->hinfo;
        وإلا
          أرجع Curl_resolv_get_https(معلومات, ctx->resolv_id);
      }
    }
  }
  أرجع عدم;
}

bool Curl_conn_dns_resolved_https(بنية Curl_easy *معلومات,
                                  int8_t sockindex,
                                  بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = معلومات->conn->cfilter[sockindex];
  لكل(; cf; cf = cf->بل) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        إذا(ctx->dns)
          أرجع TRUE;
        وإلا
          أرجع Curl_resolv_knows_https(معلومات, ctx->resolv_id);
      }
    }
  }
  أرجع TRUE;
}

#د_که_پای /* USE_HTTPSRR */
