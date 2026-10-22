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
#تعریف_کرو CURL_HE_AAAA_AWAIT_MS    25

بنية cf_dns_ctx {
  بنية Curl_dns_اندراج *dns;
  بنية Curl_peer *peer;
  CURLcode resolv_نتیجہ;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  ثنائی_ہندسہ(started);
  ثنائی_ہندسہ(announced);
  ثنائی_ہندسہ(for_proxy);
};

ساكن بنية cf_dns_ctx *cf_dns_ctx_create(بنية Curl_easy *انکڑے,
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
#جے_تعریف_شدہ DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نمبر(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#جے_انت

  CURL_TRC_DNS(انکڑے, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->پورٹ, ctx->transport);
  أرجع ctx;
}

ساكن فراغ cf_dns_ctx_destroy(بنية Curl_easy *انکڑے,
                               بنية cf_dns_ctx *ctx)
{
  إذا(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_اندراج_unlink(انکڑے, &ctx->dns);
    curlx_free(ctx);
  }
}

#جے_تعریف_شدہ CURLVERBOSE
ساكن فراغ cf_dns_report_addr(بنية Curl_easy *انکڑے,
                               بنية dynbuf *tmp,
                               ثابت حرف *label,
                               صحيح ai_family,
                               ثابت بنية Curl_addrinfo *ai)
{
  حرف buf[MAX_IPADR_LEN];
  ثابت حرف *sep = "";
  CURLcode نتیجہ;

  curlx_dyn_reset(tmp);
  لكل(; ai; ai = ai->ai_اگلا) {
    إذا(ai->ai_family == ai_family) {
      Curl_printable_پتہ(ai, buf, حجم(buf));
      نتیجہ = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      إذا(نتیجہ) {
        infof(انکڑے, "too many IP, cannot show");
        أرجع;
      }
      sep = ", ";
    }
  }

  infof(انکڑے, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

ساكن فراغ cf_dns_report(بنية Curl_cfilter *cf,
                          بنية Curl_easy *انکڑے,
                          بنية Curl_dns_اندراج *dns)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  بنية dynbuf tmp;

  إذا(!Curl_trc_is_verbose(انکڑے) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    أرجع;

  إذا(ctx->peer->unix_socket) {
#جے_تعریف_شدہ USE_UNIX_SOCKETS
    CURL_TRC_CF(انکڑے, cf, "resolved unix://%s", ctx->peer->hostname);
#وإلا
    DEBUGASSERT(0);
#جے_انت
  }
  وإلا {
    curlx_dyn_init(&tmp, 1024);
    إذا(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(انکڑے, "Host %s:%u was resolved.", dns->hostname, dns->پورٹ);
#جے_تعریف_شدہ CURLRES_IPV6
      cf_dns_report_addr(انکڑے, &tmp, "IPv6: ", AF_INET6, dns->addr);
#جے_انت
      cf_dns_report_addr(انکڑے, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#جے_تعریف_شدہ USE_HTTPSRR
    وإلا إذا(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      إذا(!dns->hinfo)
        infof(انکڑے, "HTTPS-RR %s:%u: -", dns->hostname, dns->پورٹ);
      وإلا إذا(!Curl_httpsrr_applicable(انکڑے, dns->hinfo))
        infof(انکڑے, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->پورٹ);
      وإلا {
        CURLcode نتیجہ = Curl_httpsrr_print(&tmp, dns->hinfo);
        إذا(!نتیجہ)
          infof(انکڑے, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->پورٹ, curlx_dyn_ptr(&tmp));
        وإلا
          infof(انکڑے, "Error printing HTTPS-RR information");
      }
    }
#جے_انت
    curlx_dyn_free(&tmp);
  }
}
#وإلا
#تعریف_کرو cf_dns_report(x, y, z) Curl_nop_stmt
#جے_انت

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
ساكن CURLcode cf_dns_شروع(بنية Curl_cfilter *cf,
                             بنية Curl_easy *انکڑے,
                             بنية Curl_dns_اندراج **pdns)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(انکڑے);
  CURLcode نتیجہ = CURLE_OK;

  *pdns = عدم;

  CURL_TRC_CF(انکڑے, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->پورٹ);
  إذا(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  إذا(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    إذا(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#جے_تعریف_شدہ USE_IPV6
    وإلا إذا(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#جے_انت

    نتیجہ = Curl_resolv(انکڑے, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#جے_تعریف_شدہ USE_HTTPSRR
  وإلا إذا(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    نتیجہ = Curl_resolv_https(انکڑے, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#جے_انت
  وإلا {
    failf(انکڑے, "unsupported DNS queries %x", ctx->dns_queries);
    أرجع CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!نتیجہ || !*pdns);
  إذا(!نتیجہ) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    أرجع CURLE_OK;
  }
  وإلا إذا(نتیجہ == CURLE_AGAIN) { /* async resolv in progress */
    أرجع CURLE_OK;
  }
  وإلا إذا(نتیجہ == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(انکڑے, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(انکڑے, عدم, TIMER_STARTSINGLE));
    أرجع CURLE_OPERATION_TIMEDOUT;
  }
  وإلا {
    DEBUGASSERT(نتیجہ);
    إذا(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(انکڑے, "Could not resolve: %s", ctx->peer->hostname);
    أرجع نتیجہ;
  }
}

ساكن bool cf_dns_ready_منزل_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے)
{
  بنية cf_dns_ctx *ctx = cf->ctx;

  إذا(ctx->resolv_نتیجہ)
    أرجع TRUE;
  وإلا إذا(ctx->dns)
    أرجع TRUE;
#جے_تعریف_شدہ USE_CURL_ASYNC
  وإلا إذا(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ثابت بنية curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    إذا(Curl_resolv_has_answers(انکڑے, ctx->resolv_id, CURL_DNSQ_AAAA))
      أرجع TRUE;
    pnow = Curl_pgrs_now(انکڑے);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(انکڑے, ctx->resolv_id, pnow);
    إذا(remain_ms <= 0)
      أرجع TRUE;
    CURL_TRC_CF(انکڑے, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_سیٹ(انکڑے, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    أرجع FALSE;
  }
  وإلا {
    أرجع TRUE;
  }
#وإلا
  (فراغ)انکڑے;
  DEBUGASSERT(0); /* We should not come here */
  أرجع FALSE;
#جے_انت /* USE_CURL_ASYNC */
}

ساكن CURLcode cf_dns_connect(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               bool *پورا)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  *پورا = FALSE;
  إذا(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_نتیجہ = cf_dns_شروع(cf, انکڑے, &ctx->dns);
  }

  إذا(!ctx->dns && !ctx->resolv_نتیجہ) {
    ctx->resolv_نتیجہ =
      Curl_resolv_take_نتیجہ(انکڑے, ctx->resolv_id, &ctx->dns);
  }

  إذا(ctx->resolv_نتیجہ && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(انکڑے, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (صحيح)ctx->resolv_نتیجہ);
    أرجع ctx->resolv_نتیجہ;
  }

  إذا(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    إذا((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->بٹ.dns_resolved = TRUE;
      Curl_pgrs_ویلا(انکڑے, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, انکڑے, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  إذا(!cf_dns_ready_منزل_connect(cf, انکڑے)) {
    أرجع CURLE_OK;
  }

  إذا(cf->اگلا && !cf->اگلا->connected) {
    bool sub_پورا;
    CURLcode نتیجہ = Curl_conn_cf_connect(cf->اگلا, انکڑے, &sub_پورا);
    إذا(نتیجہ || !sub_پورا)
      أرجع نتیجہ;
    DEBUGASSERT(sub_پورا);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(انکڑے, cf, "connected filter chain below");
  *پورا = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(انکڑے, ctx->resolv_id);
  أرجع CURLE_OK;
}

ساكن فراغ cf_dns_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے)
{
  بنية cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(انکڑے, cf, "destroy");
  cf_dns_ctx_destroy(انکڑے, ctx);
}

ساكن CURLcode cf_dns_adjust_pollset(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے,
                                      بنية easy_pollset *ps)
{
#جے_تعریف_شدہ USE_CURL_ASYNC
  إذا(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    بنية cf_dns_ctx *ctx = cf->ctx;
    إذا(!ctx->started) {
      CURL_TRC_CF(انکڑے, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->پورٹ,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_نتیجہ = cf_dns_شروع(cf, انکڑے, &ctx->dns);
      إذا(ctx->resolv_نتیجہ || ctx->dns) {
        Curl_multi_mark_dirty(انکڑے);
      }
    }
    أرجع Curl_resolv_pollset(انکڑے, ps);
  }
#وإلا
  (فراغ)cf;
  (فراغ)انکڑے;
  (فراغ)ps;
#جے_انت
  أرجع CURLE_OK;
}

ساكن CURLcode cf_dns_cntrl(بنية Curl_cfilter *cf,
                             بنية Curl_easy *انکڑے,
                             صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_dns_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_انکڑے_پورا:
    إذا(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_اندراج_unlink(انکڑے, &ctx->dns);
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتیجہ;
}

بنية Curl_cftype Curl_cft_dns = {
  "DNS",
  CF_TYPE_تیاری | CF_TYPE_DNS,
  CURL_LOG_LVL_NONE,
  cf_dns_destroy,
  cf_dns_connect,
  Curl_cf_def_shutdown,
  cf_dns_adjust_pollset,
  Curl_cf_def_انکڑے_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_dns_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساكن CURLcode cf_dns_create(بنية Curl_cfilter **pcf,
                              بنية Curl_easy *انکڑے,
                              بنية Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_dns_ctx *ctx;
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)انکڑے;
  ctx = cf_dns_ctx_create(انکڑے, peer, dns_queries, transport, for_proxy);
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

باہرلا:
  *pcf = نتیجہ ? عدم : cf;
  إذا(نتیجہ)
    cf_dns_ctx_destroy(انکڑے, ctx);
  أرجع نتیجہ;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
ساكن CURLcode cf_dns_جوڑو(بنية Curl_easy *انکڑے,
                           بنية connectdata *conn,
                           int8_t sockindex,
                           بنية Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  بنية Curl_cfilter *cf = عدم;
  bool for_proxy = FALSE;
  CURLcode نتیجہ;

  إذا(!peer)
    أرجع CURLE_FAILED_INIT;
#جے_بنا_تعریف CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#جے_انت

  نتیجہ = cf_dns_create(&cf, انکڑے, peer, dns_queries, transport, for_proxy);
  إذا(نتیجہ)
    اذهب باہرلا;
  Curl_conn_cf_جوڑو(انکڑے, conn, sockindex, cf);
باہرلا:
  أرجع نتیجہ;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
ساكن CURLcode cf_dns_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                    بنية Curl_easy *انکڑے,
                                    بنية Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجہ;

  نتیجہ = cf_dns_create(&cf, انکڑے, peer, dns_queries, transport, FALSE);
  إذا(نتیجہ)
    أرجع نتیجہ;

  Curl_conn_cf_داخل_کرو_after(cf_at, cf);
  أرجع CURLE_OK;
}

ساكن CURLcode cf_dns_جوڑو_resolve(بنية Curl_easy *انکڑے,
                                   بنية connectdata *conn,
                                   int8_t sockindex,
                                   بنية Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  بنية Curl_cfilter *cf = انکڑے->conn->cfilter[sockindex];
  بنية Curl_cfilter *cf_dns = عدم;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  إذا((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  لكل(; cf && dns_queries; cf = cf->اگلا) {
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
          CURL_TRC_DNS(انکڑے, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->پورٹ);
          أرجع CURLE_OK;
        }
      }
    }
  }

  إذا(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    إذا(cf_dns)
      أرجع cf_dns_داخل_کرو_after(cf_dns, انکڑے, peer, dns_queries, transport);
    وإلا
      أرجع cf_dns_جوڑو(انکڑے, conn, sockindex, peer, dns_queries, transport);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_conn_dns_جوڑو_addr_resolve(بنية Curl_easy *انکڑے,
                                        بنية connectdata *conn,
                                        int8_t sockindex,
                                        بنية Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  أرجع cf_dns_جوڑو_resolve(انکڑے, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_نتیجہ(بنية connectdata *conn,
                                   int8_t sockindex,
                                   بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = conn->cfilter[sockindex];
  لكل(; cf; cf = cf->اگلا) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        إذا(ctx->dns || ctx->resolv_نتیجہ)
          أرجع ctx->resolv_نتیجہ;
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
ثابت بنية Curl_addrinfo *Curl_conn_dns_get_ai(بنية Curl_easy *انکڑے,
                                                 بنية Curl_peer *peer,
                                                 int8_t sockindex,
                                                 صحيح ai_family,
                                                 بنانشانی صحيح اشاریہ)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية Curl_cfilter *cf = conn->cfilter[sockindex];

  لكل(; cf; cf = cf->اگلا) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(انکڑے, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    اشاریہ, peer->hostname, peer->پورٹ, ai_family, !!ctx->dns);
        إذا(ctx->resolv_نتیجہ)
          أرجع عدم;
        وإلا إذا(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          إذا((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            أرجع عدم;
#جے_تعریف_شدہ USE_IPV6
          إذا((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            أرجع عدم;
#جے_انت
          أرجع Curl_addrinfo_get(ctx->dns->addr, ai_family, اشاریہ);
        }
        وإلا
          أرجع Curl_resolv_get_ai(انکڑے, ctx->resolv_id, ai_family, اشاریہ);
      }
    }
  }
  أرجع عدم;
}

#جے_تعریف_شدہ USE_HTTPSRR
CURLcode Curl_conn_dns_جوڑو_https_resolve(بنية Curl_easy *انکڑے,
                                         بنية connectdata *conn,
                                         int8_t sockindex,
                                         بنية Curl_peer *peer)
{
  أرجع cf_dns_جوڑو_resolve(انکڑے, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ثابت بنية Curl_https_rrinfo *Curl_conn_dns_get_https(بنية Curl_easy *انکڑے,
                                                        int8_t sockindex,
                                                        بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = انکڑے->conn->cfilter[sockindex];
  لكل(; cf; cf = cf->اگلا) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        إذا(ctx->dns)
          أرجع ctx->dns->hinfo;
        وإلا
          أرجع Curl_resolv_get_https(انکڑے, ctx->resolv_id);
      }
    }
  }
  أرجع عدم;
}

bool Curl_conn_dns_resolved_https(بنية Curl_easy *انکڑے,
                                  int8_t sockindex,
                                  بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = انکڑے->conn->cfilter[sockindex];
  لكل(; cf; cf = cf->اگلا) {
    إذا(cf->cft == &Curl_cft_dns) {
      بنية cf_dns_ctx *ctx = cf->ctx;
      إذا(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        إذا(ctx->dns)
          أرجع TRUE;
        وإلا
          أرجع Curl_resolv_knows_https(انکڑے, ctx->resolv_id);
      }
    }
  }
  أرجع TRUE;
}

#جے_انت /* USE_HTTPSRR */
