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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "curl_addrinfo.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "curl_trc.h"
#شامل "multiif.h"
#شامل "progress.h"
#شامل "url.h"
#شامل "vdns/cf-dns.h"
#شامل "vdns/dnscache.h"
#شامل "vdns/httpsrr.h"
#شامل "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#تعریف_کرو CURL_HE_AAAA_AWAIT_MS    25

ساخت cf_dns_ctx {
  ساخت Curl_dns_اندراج *dns;
  ساخت Curl_peer *peer;
  CURLcode resolv_نتیجہ;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  ثنائی_ہندسہ(started);
  ثنائی_ہندسہ(announced);
  ثنائی_ہندسہ(for_proxy);
};

ساکن ساخت cf_dns_ctx *cf_dns_ctx_create(ساخت Curl_easy *ڈیٹا,
                                            ساخت Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  ساخت cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx)
    واپس تہی;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#اگر_تعریف_شدہ DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#اگر_ختم

  CURL_TRC_DNS(ڈیٹا, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->پورٹ, ctx->transport);
  واپس ctx;
}

ساکن خالی cf_dns_ctx_destroy(ساخت Curl_easy *ڈیٹا,
                               ساخت cf_dns_ctx *ctx)
{
  اگر(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_اندراج_unlink(ڈیٹا, &ctx->dns);
    curlx_free(ctx);
  }
}

#اگر_تعریف_شدہ CURLVERBOSE
ساکن خالی cf_dns_report_addr(ساخت Curl_easy *ڈیٹا,
                               ساخت dynbuf *tmp,
                               ثابت حرف *label,
                               صحیح ai_family,
                               ثابت ساخت Curl_addrinfo *ai)
{
  حرف buf[MAX_IPADR_LEN];
  ثابت حرف *sep = "";
  CURLcode نتیجہ;

  curlx_dyn_reset(tmp);
  برائے(; ai; ai = ai->ai_اگلا) {
    اگر(ai->ai_family == ai_family) {
      Curl_printable_پتہ(ai, buf, حجم(buf));
      نتیجہ = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      اگر(نتیجہ) {
        infof(ڈیٹا, "too many IP, cannot show");
        واپس;
      }
      sep = ", ";
    }
  }

  infof(ڈیٹا, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

ساکن خالی cf_dns_report(ساخت Curl_cfilter *cf,
                          ساخت Curl_easy *ڈیٹا,
                          ساخت Curl_dns_اندراج *dns)
{
  ساخت cf_dns_ctx *ctx = cf->ctx;
  ساخت dynbuf tmp;

  اگر(!Curl_trc_is_verbose(ڈیٹا) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    واپس;

  اگر(ctx->peer->unix_socket) {
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
    CURL_TRC_CF(ڈیٹا, cf, "resolved unix://%s", ctx->peer->hostname);
#ورنہ
    DEBUGASSERT(0);
#اگر_ختم
  }
  ورنہ {
    curlx_dyn_init(&tmp, 1024);
    اگر(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(ڈیٹا, "Host %s:%u was resolved.", dns->hostname, dns->پورٹ);
#اگر_تعریف_شدہ CURLRES_IPV6
      cf_dns_report_addr(ڈیٹا, &tmp, "IPv6: ", AF_INET6, dns->addr);
#اگر_ختم
      cf_dns_report_addr(ڈیٹا, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#اگر_تعریف_شدہ USE_HTTPSRR
    ورنہ اگر(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      اگر(!dns->hinfo)
        infof(ڈیٹا, "HTTPS-RR %s:%u: -", dns->hostname, dns->پورٹ);
      ورنہ اگر(!Curl_httpsrr_applicable(ڈیٹا, dns->hinfo))
        infof(ڈیٹا, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->پورٹ);
      ورنہ {
        CURLcode نتیجہ = Curl_httpsrr_print(&tmp, dns->hinfo);
        اگر(!نتیجہ)
          infof(ڈیٹا, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->پورٹ, curlx_dyn_ptr(&tmp));
        ورنہ
          infof(ڈیٹا, "Error printing HTTPS-RR information");
      }
    }
#اگر_ختم
    curlx_dyn_free(&tmp);
  }
}
#ورنہ
#تعریف_کرو cf_dns_report(x, y, z) Curl_nop_stmt
#اگر_ختم

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
ساکن CURLcode cf_dns_آغاز(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_dns_اندراج **pdns)
{
  ساخت cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(ڈیٹا);
  CURLcode نتیجہ = CURLE_OK;

  *pdns = تہی;

  CURL_TRC_CF(ڈیٹا, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->پورٹ);
  اگر(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  اگر(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    اگر(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#اگر_تعریف_شدہ USE_IPV6
    ورنہ اگر(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#اگر_ختم

    نتیجہ = Curl_resolv(ڈیٹا, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#اگر_تعریف_شدہ USE_HTTPSRR
  ورنہ اگر(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    نتیجہ = Curl_resolv_https(ڈیٹا, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#اگر_ختم
  ورنہ {
    failf(ڈیٹا, "unsupported DNS queries %x", ctx->dns_queries);
    واپس CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!نتیجہ || !*pdns);
  اگر(!نتیجہ) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    واپس CURLE_OK;
  }
  ورنہ اگر(نتیجہ == CURLE_AGAIN) { /* async resolv in progress */
    واپس CURLE_OK;
  }
  ورنہ اگر(نتیجہ == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(ڈیٹا, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(ڈیٹا, تہی, TIMER_STARTSINGLE));
    واپس CURLE_OPERATION_TIMEDOUT;
  }
  ورنہ {
    DEBUGASSERT(نتیجہ);
    اگر(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(ڈیٹا, "Could not resolve: %s", ctx->peer->hostname);
    واپس نتیجہ;
  }
}

ساکن bool cf_dns_ready_مقصد_connect(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_dns_ctx *ctx = cf->ctx;

  اگر(ctx->resolv_نتیجہ)
    واپس TRUE;
  ورنہ اگر(ctx->dns)
    واپس TRUE;
#اگر_تعریف_شدہ USE_CURL_ASYNC
  ورنہ اگر(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ثابت ساخت curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    اگر(Curl_resolv_has_answers(ڈیٹا, ctx->resolv_id, CURL_DNSQ_AAAA))
      واپس TRUE;
    pnow = Curl_pgrs_now(ڈیٹا);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(ڈیٹا, ctx->resolv_id, pnow);
    اگر(remain_ms <= 0)
      واپس TRUE;
    CURL_TRC_CF(ڈیٹا, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_سیٹ(ڈیٹا, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    واپس FALSE;
  }
  ورنہ {
    واپس TRUE;
  }
#ورنہ
  (خالی)ڈیٹا;
  DEBUGASSERT(0); /* We should not come here */
  واپس FALSE;
#اگر_ختم /* USE_CURL_ASYNC */
}

ساکن CURLcode cf_dns_connect(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               bool *مکمل)
{
  ساخت cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  *مکمل = FALSE;
  اگر(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_نتیجہ = cf_dns_آغاز(cf, ڈیٹا, &ctx->dns);
  }

  اگر(!ctx->dns && !ctx->resolv_نتیجہ) {
    ctx->resolv_نتیجہ =
      Curl_resolv_take_نتیجہ(ڈیٹا, ctx->resolv_id, &ctx->dns);
  }

  اگر(ctx->resolv_نتیجہ && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(ڈیٹا, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (صحیح)ctx->resolv_نتیجہ);
    واپس ctx->resolv_نتیجہ;
  }

  اگر(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    اگر((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->بٹ.dns_resolved = TRUE;
      Curl_pgrs_وقت(ڈیٹا, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, ڈیٹا, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  اگر(!cf_dns_ready_مقصد_connect(cf, ڈیٹا)) {
    واپس CURLE_OK;
  }

  اگر(cf->اگلا && !cf->اگلا->connected) {
    bool sub_مکمل;
    CURLcode نتیجہ = Curl_conn_cf_connect(cf->اگلا, ڈیٹا, &sub_مکمل);
    اگر(نتیجہ || !sub_مکمل)
      واپس نتیجہ;
    DEBUGASSERT(sub_مکمل);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(ڈیٹا, cf, "connected filter chain below");
  *مکمل = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(ڈیٹا, ctx->resolv_id);
  واپس CURLE_OK;
}

ساکن خالی cf_dns_destroy(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ڈیٹا, cf, "destroy");
  cf_dns_ctx_destroy(ڈیٹا, ctx);
}

ساکن CURLcode cf_dns_adjust_pollset(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      ساخت easy_pollset *ps)
{
#اگر_تعریف_شدہ USE_CURL_ASYNC
  اگر(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    ساخت cf_dns_ctx *ctx = cf->ctx;
    اگر(!ctx->started) {
      CURL_TRC_CF(ڈیٹا, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->پورٹ,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_نتیجہ = cf_dns_آغاز(cf, ڈیٹا, &ctx->dns);
      اگر(ctx->resolv_نتیجہ || ctx->dns) {
        Curl_multi_mark_dirty(ڈیٹا);
      }
    }
    واپس Curl_resolv_pollset(ڈیٹا, ps);
  }
#ورنہ
  (خالی)cf;
  (خالی)ڈیٹا;
  (خالی)ps;
#اگر_ختم
  واپس CURLE_OK;
}

ساکن CURLcode cf_dns_cntrl(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             صحیح event, صحیح arg1, خالی *arg2)
{
  ساخت cf_dns_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  (خالی)arg1;
  (خالی)arg2;
  انتخاب(event) {
  حالت CF_CTRL_ڈیٹا_مکمل:
    اگر(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_اندراج_unlink(ڈیٹا, &ctx->dns);
    }
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس نتیجہ;
}

ساخت Curl_cftype Curl_cft_dns = {
  "DNS",
  CF_TYPE_تیاری | CF_TYPE_DNS,
  CURL_LOG_LVL_NONE,
  cf_dns_destroy,
  cf_dns_connect,
  Curl_cf_def_shutdown,
  cf_dns_adjust_pollset,
  Curl_cf_def_ڈیٹا_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_dns_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساکن CURLcode cf_dns_create(ساخت Curl_cfilter **pcf,
                              ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  ساخت Curl_cfilter *cf = تہی;
  ساخت cf_dns_ctx *ctx;
  CURLcode نتیجہ = CURLE_OK;

  (خالی)ڈیٹا;
  ctx = cf_dns_ctx_create(ڈیٹا, peer, dns_queries, transport, for_proxy);
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

برآمد:
  *pcf = نتیجہ ? تہی : cf;
  اگر(نتیجہ)
    cf_dns_ctx_destroy(ڈیٹا, ctx);
  واپس نتیجہ;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
ساکن CURLcode cf_dns_جوڑو(ساخت Curl_easy *ڈیٹا,
                           ساخت connectdata *conn,
                           int8_t sockindex,
                           ساخت Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  ساخت Curl_cfilter *cf = تہی;
  bool for_proxy = FALSE;
  CURLcode نتیجہ;

  اگر(!peer)
    واپس CURLE_FAILED_INIT;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#اگر_ختم

  نتیجہ = cf_dns_create(&cf, ڈیٹا, peer, dns_queries, transport, for_proxy);
  اگر(نتیجہ)
    جاؤ برآمد;
  Curl_conn_cf_جوڑو(ڈیٹا, conn, sockindex, cf);
برآمد:
  واپس نتیجہ;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
ساکن CURLcode cf_dns_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ;

  نتیجہ = cf_dns_create(&cf, ڈیٹا, peer, dns_queries, transport, FALSE);
  اگر(نتیجہ)
    واپس نتیجہ;

  Curl_conn_cf_داخل_کرو_after(cf_at, cf);
  واپس CURLE_OK;
}

ساکن CURLcode cf_dns_جوڑو_resolve(ساخت Curl_easy *ڈیٹا,
                                   ساخت connectdata *conn,
                                   int8_t sockindex,
                                   ساخت Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  ساخت Curl_cfilter *cf = ڈیٹا->conn->cfilter[sockindex];
  ساخت Curl_cfilter *cf_dns = تہی;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  اگر((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  برائے(; cf && dns_queries; cf = cf->اگلا) {
    اگر(cf->cft == &Curl_cft_dns) {
      ساخت cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      اگر(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        اگر(!dns_queries) { /* already there */
          واپس CURLE_OK;
        }
        ورنہ اگر(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(ڈیٹا, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->پورٹ);
          واپس CURLE_OK;
        }
      }
    }
  }

  اگر(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    اگر(cf_dns)
      واپس cf_dns_داخل_کرو_after(cf_dns, ڈیٹا, peer, dns_queries, transport);
    ورنہ
      واپس cf_dns_جوڑو(ڈیٹا, conn, sockindex, peer, dns_queries, transport);
  }
  واپس CURLE_OK;
}

CURLcode Curl_conn_dns_جوڑو_addr_resolve(ساخت Curl_easy *ڈیٹا,
                                        ساخت connectdata *conn,
                                        int8_t sockindex,
                                        ساخت Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  واپس cf_dns_جوڑو_resolve(ڈیٹا, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_نتیجہ(ساخت connectdata *conn,
                                   int8_t sockindex,
                                   ساخت Curl_peer *peer)
{
  ساخت Curl_cfilter *cf = conn->cfilter[sockindex];
  برائے(; cf; cf = cf->اگلا) {
    اگر(cf->cft == &Curl_cft_dns) {
      ساخت cf_dns_ctx *ctx = cf->ctx;
      اگر(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        اگر(ctx->dns || ctx->resolv_نتیجہ)
          واپس ctx->resolv_نتیجہ;
        واپس CURLE_AGAIN;
      }
    }
  }
  واپس CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
ثابت ساخت Curl_addrinfo *Curl_conn_dns_get_ai(ساخت Curl_easy *ڈیٹا,
                                                 ساخت Curl_peer *peer,
                                                 int8_t sockindex,
                                                 صحیح ai_family,
                                                 غیرعلامتی صحیح اشاریہ)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت Curl_cfilter *cf = conn->cfilter[sockindex];

  برائے(; cf; cf = cf->اگلا) {
    اگر(cf->cft == &Curl_cft_dns) {
      ساخت cf_dns_ctx *ctx = cf->ctx;
      اگر(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(ڈیٹا, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    اشاریہ, peer->hostname, peer->پورٹ, ai_family, !!ctx->dns);
        اگر(ctx->resolv_نتیجہ)
          واپس تہی;
        ورنہ اگر(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          اگر((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            واپس تہی;
#اگر_تعریف_شدہ USE_IPV6
          اگر((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            واپس تہی;
#اگر_ختم
          واپس Curl_addrinfo_get(ctx->dns->addr, ai_family, اشاریہ);
        }
        ورنہ
          واپس Curl_resolv_get_ai(ڈیٹا, ctx->resolv_id, ai_family, اشاریہ);
      }
    }
  }
  واپس تہی;
}

#اگر_تعریف_شدہ USE_HTTPSRR
CURLcode Curl_conn_dns_جوڑو_https_resolve(ساخت Curl_easy *ڈیٹا,
                                         ساخت connectdata *conn,
                                         int8_t sockindex,
                                         ساخت Curl_peer *peer)
{
  واپس cf_dns_جوڑو_resolve(ڈیٹا, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ثابت ساخت Curl_https_rrinfo *Curl_conn_dns_get_https(ساخت Curl_easy *ڈیٹا,
                                                        int8_t sockindex,
                                                        ساخت Curl_peer *peer)
{
  ساخت Curl_cfilter *cf = ڈیٹا->conn->cfilter[sockindex];
  برائے(; cf; cf = cf->اگلا) {
    اگر(cf->cft == &Curl_cft_dns) {
      ساخت cf_dns_ctx *ctx = cf->ctx;
      اگر(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        اگر(ctx->dns)
          واپس ctx->dns->hinfo;
        ورنہ
          واپس Curl_resolv_get_https(ڈیٹا, ctx->resolv_id);
      }
    }
  }
  واپس تہی;
}

bool Curl_conn_dns_resolved_https(ساخت Curl_easy *ڈیٹا,
                                  int8_t sockindex,
                                  ساخت Curl_peer *peer)
{
  ساخت Curl_cfilter *cf = ڈیٹا->conn->cfilter[sockindex];
  برائے(; cf; cf = cf->اگلا) {
    اگر(cf->cft == &Curl_cft_dns) {
      ساخت cf_dns_ctx *ctx = cf->ctx;
      اگر(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        اگر(ctx->dns)
          واپس TRUE;
        ورنہ
          واپس Curl_resolv_knows_https(ڈیٹا, ctx->resolv_id);
      }
    }
  }
  واپس TRUE;
}

#اگر_ختم /* USE_HTTPSRR */
