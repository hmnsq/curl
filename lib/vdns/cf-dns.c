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
#define CURL_HE_AAAA_AWAIT_MS    25

ساختار cf_dns_ctx {
  ساختار Curl_dns_entry *dns;
  ساختار Curl_peer *peer;
  CURLcode resolv_result;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

ایستا ساختار cf_dns_ctx *cf_dns_ctx_create(ساختار Curl_easy *data,
                                            ساختار Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  ساختار cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx)
    بازگشت پوچ;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    ثابت نویسه *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  بازگشت ctx;
}

ایستا تهی cf_dns_ctx_destroy(ساختار Curl_easy *data,
                               ساختار cf_dns_ctx *ctx)
{
  اگر(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
ایستا تهی cf_dns_report_addr(ساختار Curl_easy *data,
                               ساختار dynbuf *tmp,
                               ثابت نویسه *label,
                               صحیح ai_family,
                               ثابت ساختار Curl_addrinfo *ai)
{
  نویسه buf[MAX_IPADR_LEN];
  ثابت نویسه *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  برای(; ai; ai = ai->ai_next) {
    اگر(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, اندازه(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      اگر(result) {
        infof(data, "too many IP, cannot show");
        بازگشت;
      }
      sep = ", ";
    }
  }

  infof(data, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

ایستا تهی cf_dns_report(ساختار Curl_cfilter *cf,
                          ساختار Curl_easy *data,
                          ساختار Curl_dns_entry *dns)
{
  ساختار cf_dns_ctx *ctx = cf->ctx;
  ساختار dynbuf tmp;

  اگر(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    بازگشت;

  اگر(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#وگرنه
    DEBUGASSERT(0);
#endif
  }
  وگرنه {
    curlx_dyn_init(&tmp, 1024);
    اگر(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    وگرنه اگر(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      اگر(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      وگرنه اگر(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      وگرنه {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        اگر(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        وگرنه
          infof(data, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#وگرنه
#define cf_dns_report(x, y, z) Curl_nop_stmt
#endif

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
ایستا CURLcode cf_dns_start(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             ساختار Curl_dns_entry **pdns)
{
  ساختار cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(data);
  CURLcode result = CURLE_OK;

  *pdns = پوچ;

  CURL_TRC_CF(data, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->port);
  اگر(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  اگر(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    اگر(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    وگرنه اگر(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  وگرنه اگر(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  وگرنه {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    بازگشت CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  اگر(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    بازگشت CURLE_OK;
  }
  وگرنه اگر(result == CURLE_AGAIN) { /* async resolv in progress */
    بازگشت CURLE_OK;
  }
  وگرنه اگر(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, پوچ, TIMER_STARTSINGLE));
    بازگشت CURLE_OPERATION_TIMEDOUT;
  }
  وگرنه {
    DEBUGASSERT(result);
    اگر(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    بازگشت result;
  }
}

ایستا bool cf_dns_ready_to_connect(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data)
{
  ساختار cf_dns_ctx *ctx = cf->ctx;

  اگر(ctx->resolv_result)
    بازگشت TRUE;
  وگرنه اگر(ctx->dns)
    بازگشت TRUE;
#ifdef USE_CURL_ASYNC
  وگرنه اگر(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ثابت ساختار curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    اگر(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      بازگشت TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    اگر(remain_ms <= 0)
      بازگشت TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    بازگشت FALSE;
  }
  وگرنه {
    بازگشت TRUE;
  }
#وگرنه
  (تهی)data;
  DEBUGASSERT(0); /* We should not come here */
  بازگشت FALSE;
#endif /* USE_CURL_ASYNC */
}

ایستا CURLcode cf_dns_connect(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               bool *done)
{
  ساختار cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  *done = FALSE;
  اگر(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  اگر(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  اگر(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (صحیح)ctx->resolv_result);
    بازگشت ctx->resolv_result;
  }

  اگر(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    اگر((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  اگر(!cf_dns_ready_to_connect(cf, data)) {
    بازگشت CURLE_OK;
  }

  اگر(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    اگر(result || !sub_done)
      بازگشت result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  بازگشت CURLE_OK;
}

ایستا تهی cf_dns_destroy(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_dns_ctx_destroy(data, ctx);
}

ایستا CURLcode cf_dns_adjust_pollset(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      ساختار easy_pollset *ps)
{
#ifdef USE_CURL_ASYNC
  اگر(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    ساختار cf_dns_ctx *ctx = cf->ctx;
    اگر(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      اگر(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    بازگشت Curl_resolv_pollset(data, ps);
  }
#وگرنه
  (تهی)cf;
  (تهی)data;
  (تهی)ps;
#endif
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_dns_cntrl(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             صحیح event, صحیح arg1, تهی *arg2)
{
  ساختار cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (تهی)arg1;
  (تهی)arg2;
  انتخاب(event) {
  حالت CF_CTRL_DATA_DONE:
    اگر(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت result;
}

ساختار Curl_cftype Curl_cft_dns = {
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

ایستا CURLcode cf_dns_create(ساختار Curl_cfilter **pcf,
                              ساختار Curl_easy *data,
                              ساختار Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  ساختار Curl_cfilter *cf = پوچ;
  ساختار cf_dns_ctx *ctx;
  CURLcode result = CURLE_OK;

  (تهی)data;
  ctx = cf_dns_ctx_create(data, peer, dns_queries, transport, for_proxy);
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? پوچ : cf;
  اگر(result)
    cf_dns_ctx_destroy(data, ctx);
  بازگشت result;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
ایستا CURLcode cf_dns_add(ساختار Curl_easy *data,
                           ساختار connectdata *conn,
                           int8_t sockindex,
                           ساختار Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  ساختار Curl_cfilter *cf = پوچ;
  bool for_proxy = FALSE;
  CURLcode result;

  اگر(!peer)
    بازگشت CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  اگر(result)
    برو out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  بازگشت result;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
ایستا CURLcode cf_dns_درج_after(ساختار Curl_cfilter *cf_at,
                                    ساختار Curl_easy *data,
                                    ساختار Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  ساختار Curl_cfilter *cf;
  CURLcode result;

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, FALSE);
  اگر(result)
    بازگشت result;

  Curl_conn_cf_درج_after(cf_at, cf);
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_dns_add_resolve(ساختار Curl_easy *data,
                                   ساختار connectdata *conn,
                                   int8_t sockindex,
                                   ساختار Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  ساختار Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ساختار Curl_cfilter *cf_dns = پوچ;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  اگر((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  برای(; cf && dns_queries; cf = cf->next) {
    اگر(cf->cft == &Curl_cft_dns) {
      ساختار cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      اگر(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        اگر(!dns_queries) { /* already there */
          بازگشت CURLE_OK;
        }
        وگرنه اگر(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          بازگشت CURLE_OK;
        }
      }
    }
  }

  اگر(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    اگر(cf_dns)
      بازگشت cf_dns_درج_after(cf_dns, data, peer, dns_queries, transport);
    وگرنه
      بازگشت cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(ساختار Curl_easy *data,
                                        ساختار connectdata *conn,
                                        int8_t sockindex,
                                        ساختار Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  بازگشت cf_dns_add_resolve(data, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_result(ساختار connectdata *conn,
                                   int8_t sockindex,
                                   ساختار Curl_peer *peer)
{
  ساختار Curl_cfilter *cf = conn->cfilter[sockindex];
  برای(; cf; cf = cf->next) {
    اگر(cf->cft == &Curl_cft_dns) {
      ساختار cf_dns_ctx *ctx = cf->ctx;
      اگر(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        اگر(ctx->dns || ctx->resolv_result)
          بازگشت ctx->resolv_result;
        بازگشت CURLE_AGAIN;
      }
    }
  }
  بازگشت CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
ثابت ساختار Curl_addrinfo *Curl_conn_dns_get_ai(ساختار Curl_easy *data,
                                                 ساختار Curl_peer *peer,
                                                 int8_t sockindex,
                                                 صحیح ai_family,
                                                 unsigned صحیح index)
{
  ساختار connectdata *conn = data->conn;
  ساختار Curl_cfilter *cf = conn->cfilter[sockindex];

  برای(; cf; cf = cf->next) {
    اگر(cf->cft == &Curl_cft_dns) {
      ساختار cf_dns_ctx *ctx = cf->ctx;
      اگر(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        اگر(ctx->resolv_result)
          بازگشت پوچ;
        وگرنه اگر(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          اگر((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            بازگشت پوچ;
#ifdef USE_IPV6
          اگر((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            بازگشت پوچ;
#endif
          بازگشت Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        وگرنه
          بازگشت Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  بازگشت پوچ;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(ساختار Curl_easy *data,
                                         ساختار connectdata *conn,
                                         int8_t sockindex,
                                         ساختار Curl_peer *peer)
{
  بازگشت cf_dns_add_resolve(data, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ثابت ساختار Curl_https_rrinfo *Curl_conn_dns_get_https(ساختار Curl_easy *data,
                                                        int8_t sockindex,
                                                        ساختار Curl_peer *peer)
{
  ساختار Curl_cfilter *cf = data->conn->cfilter[sockindex];
  برای(; cf; cf = cf->next) {
    اگر(cf->cft == &Curl_cft_dns) {
      ساختار cf_dns_ctx *ctx = cf->ctx;
      اگر(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        اگر(ctx->dns)
          بازگشت ctx->dns->hinfo;
        وگرنه
          بازگشت Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  بازگشت پوچ;
}

bool Curl_conn_dns_resolved_https(ساختار Curl_easy *data,
                                  int8_t sockindex,
                                  ساختار Curl_peer *peer)
{
  ساختار Curl_cfilter *cf = data->conn->cfilter[sockindex];
  برای(; cf; cf = cf->next) {
    اگر(cf->cft == &Curl_cft_dns) {
      ساختار cf_dns_ctx *ctx = cf->ctx;
      اگر(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        اگر(ctx->dns)
          بازگشت TRUE;
        وگرنه
          بازگشت Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  بازگشت TRUE;
}

#endif /* USE_HTTPSRR */
