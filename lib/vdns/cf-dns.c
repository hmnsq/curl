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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vdns/dnscache.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#define CURL_HE_AAAA_AWAIT_MS    25

संरचना cf_dns_ctx {
  संरचना Curl_dns_entry *dns;
  संरचना Curl_peer *peer;
  CURLcode resolv_result;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

स्थिर संरचना cf_dns_ctx *cf_dns_ctx_create(संरचना Curl_easy *data,
                                            संरचना Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  संरचना cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  जइ(!ctx)
    प्रत्यागम रिक्त;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    ध्रुव वर्ण *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    जइ(p) {
      curl_off_t l;
      जइ(!curlx_str_सङ्ख्या(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  प्रत्यागम ctx;
}

स्थिर शून्य cf_dns_ctx_destroy(संरचना Curl_easy *data,
                               संरचना cf_dns_ctx *ctx)
{
  जइ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
स्थिर शून्य cf_dns_report_addr(संरचना Curl_easy *data,
                               संरचना dynbuf *tmp,
                               ध्रुव वर्ण *label,
                               पूर्णाङ्क ai_family,
                               ध्रुव संरचना Curl_addrinfo *ai)
{
  वर्ण buf[MAX_IPADR_LEN];
  ध्रुव वर्ण *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  प्रति(; ai; ai = ai->ai_next) {
    जइ(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, परिमाण(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      जइ(result) {
        infof(data, "too many IP, cannot show");
        प्रत्यागम;
      }
      sep = ", ";
    }
  }

  infof(data, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

स्थिर शून्य cf_dns_report(संरचना Curl_cfilter *cf,
                          संरचना Curl_easy *data,
                          संरचना Curl_dns_entry *dns)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;
  संरचना dynbuf tmp;

  जइ(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    प्रत्यागम;

  जइ(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#अण्णहा
    DEBUGASSERT(0);
#endif
  }
  अण्णहा {
    curlx_dyn_init(&tmp, 1024);
    जइ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    अण्णहा जइ(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      जइ(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      अण्णहा जइ(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      अण्णहा {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        जइ(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        अण्णहा
          infof(data, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#अण्णहा
#define cf_dns_report(x, y, z) Curl_nop_stmt
#endif

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
स्थिर CURLcode cf_dns_start(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             संरचना Curl_dns_entry **pdns)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(data);
  CURLcode result = CURLE_OK;

  *pdns = रिक्त;

  CURL_TRC_CF(data, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->port);
  जइ(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  जइ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    जइ(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    अण्णहा जइ(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  अण्णहा जइ(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  अण्णहा {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  जइ(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    प्रत्यागम CURLE_OK;
  }
  अण्णहा जइ(result == CURLE_AGAIN) { /* async resolv in progress */
    प्रत्यागम CURLE_OK;
  }
  अण्णहा जइ(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, रिक्त, TIMER_STARTSINGLE));
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;
  }
  अण्णहा {
    DEBUGASSERT(result);
    जइ(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    प्रत्यागम result;
  }
}

स्थिर bool cf_dns_ready_to_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;

  जइ(ctx->resolv_result)
    प्रत्यागम TRUE;
  अण्णहा जइ(ctx->dns)
    प्रत्यागम TRUE;
#ifdef USE_CURL_ASYNC
  अण्णहा जइ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ध्रुव संरचना curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    जइ(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      प्रत्यागम TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    जइ(remain_ms <= 0)
      प्रत्यागम TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    प्रत्यागम FALSE;
  }
  अण्णहा {
    प्रत्यागम TRUE;
  }
#अण्णहा
  (शून्य)data;
  DEBUGASSERT(0); /* We should not come here */
  प्रत्यागम FALSE;
#endif /* USE_CURL_ASYNC */
}

स्थिर CURLcode cf_dns_connect(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               bool *done)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  जइ(cf->connected) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *done = FALSE;
  जइ(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  जइ(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  जइ(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (पूर्णाङ्क)ctx->resolv_result);
    प्रत्यागम ctx->resolv_result;
  }

  जइ(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    जइ((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  जइ(!cf_dns_ready_to_connect(cf, data)) {
    प्रत्यागम CURLE_OK;
  }

  जइ(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    जइ(result || !sub_done)
      प्रत्यागम result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_dns_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_dns_ctx_destroy(data, ctx);
}

स्थिर CURLcode cf_dns_adjust_pollset(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      संरचना easy_pollset *ps)
{
#ifdef USE_CURL_ASYNC
  जइ(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    संरचना cf_dns_ctx *ctx = cf->ctx;
    जइ(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      जइ(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    प्रत्यागम Curl_resolv_pollset(data, ps);
  }
#अण्णहा
  (शून्य)cf;
  (शून्य)data;
  (शून्य)ps;
#endif
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_dns_cntrl(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (शून्य)arg1;
  (शून्य)arg2;
  विकल्प(event) {
  पक्ष CF_CTRL_DATA_DONE:
    जइ(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम result;
}

संरचना Curl_cftype Curl_cft_dns = {
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

स्थिर CURLcode cf_dns_create(संरचना Curl_cfilter **pcf,
                              संरचना Curl_easy *data,
                              संरचना Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_dns_ctx *ctx;
  CURLcode result = CURLE_OK;

  (शून्य)data;
  ctx = cf_dns_ctx_create(data, peer, dns_queries, transport, for_proxy);
  जइ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? रिक्त : cf;
  जइ(result)
    cf_dns_ctx_destroy(data, ctx);
  प्रत्यागम result;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
स्थिर CURLcode cf_dns_add(संरचना Curl_easy *data,
                           संरचना connectdata *conn,
                           int8_t sockindex,
                           संरचना Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  संरचना Curl_cfilter *cf = रिक्त;
  bool for_proxy = FALSE;
  CURLcode result;

  जइ(!peer)
    प्रत्यागम CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  जइ(result)
    गच्छ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  प्रत्यागम result;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
स्थिर CURLcode cf_dns_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                    संरचना Curl_easy *data,
                                    संरचना Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, FALSE);
  जइ(result)
    प्रत्यागम result;

  Curl_conn_cf_निवेशय_after(cf_at, cf);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_dns_add_resolve(संरचना Curl_easy *data,
                                   संरचना connectdata *conn,
                                   int8_t sockindex,
                                   संरचना Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  संरचना Curl_cfilter *cf = data->conn->cfilter[sockindex];
  संरचना Curl_cfilter *cf_dns = रिक्त;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  जइ((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  प्रति(; cf && dns_queries; cf = cf->next) {
    जइ(cf->cft == &Curl_cft_dns) {
      संरचना cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      जइ(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        जइ(!dns_queries) { /* already there */
          प्रत्यागम CURLE_OK;
        }
        अण्णहा जइ(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          प्रत्यागम CURLE_OK;
        }
      }
    }
  }

  जइ(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    जइ(cf_dns)
      प्रत्यागम cf_dns_निवेशय_after(cf_dns, data, peer, dns_queries, transport);
    अण्णहा
      प्रत्यागम cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(संरचना Curl_easy *data,
                                        संरचना connectdata *conn,
                                        int8_t sockindex,
                                        संरचना Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  प्रत्यागम cf_dns_add_resolve(data, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_result(संरचना connectdata *conn,
                                   int8_t sockindex,
                                   संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf = conn->cfilter[sockindex];
  प्रति(; cf; cf = cf->next) {
    जइ(cf->cft == &Curl_cft_dns) {
      संरचना cf_dns_ctx *ctx = cf->ctx;
      जइ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        जइ(ctx->dns || ctx->resolv_result)
          प्रत्यागम ctx->resolv_result;
        प्रत्यागम CURLE_AGAIN;
      }
    }
  }
  प्रत्यागम CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
ध्रुव संरचना Curl_addrinfo *Curl_conn_dns_get_ai(संरचना Curl_easy *data,
                                                 संरचना Curl_peer *peer,
                                                 int8_t sockindex,
                                                 पूर्णाङ्क ai_family,
                                                 unsigned पूर्णाङ्क index)
{
  संरचना connectdata *conn = data->conn;
  संरचना Curl_cfilter *cf = conn->cfilter[sockindex];

  प्रति(; cf; cf = cf->next) {
    जइ(cf->cft == &Curl_cft_dns) {
      संरचना cf_dns_ctx *ctx = cf->ctx;
      जइ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        जइ(ctx->resolv_result)
          प्रत्यागम रिक्त;
        अण्णहा जइ(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          जइ((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            प्रत्यागम रिक्त;
#ifdef USE_IPV6
          जइ((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            प्रत्यागम रिक्त;
#endif
          प्रत्यागम Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        अण्णहा
          प्रत्यागम Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  प्रत्यागम रिक्त;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(संरचना Curl_easy *data,
                                         संरचना connectdata *conn,
                                         int8_t sockindex,
                                         संरचना Curl_peer *peer)
{
  प्रत्यागम cf_dns_add_resolve(data, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ध्रुव संरचना Curl_https_rrinfo *Curl_conn_dns_get_https(संरचना Curl_easy *data,
                                                        int8_t sockindex,
                                                        संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf = data->conn->cfilter[sockindex];
  प्रति(; cf; cf = cf->next) {
    जइ(cf->cft == &Curl_cft_dns) {
      संरचना cf_dns_ctx *ctx = cf->ctx;
      जइ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        जइ(ctx->dns)
          प्रत्यागम ctx->dns->hinfo;
        अण्णहा
          प्रत्यागम Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  प्रत्यागम रिक्त;
}

bool Curl_conn_dns_resolved_https(संरचना Curl_easy *data,
                                  int8_t sockindex,
                                  संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf = data->conn->cfilter[sockindex];
  प्रति(; cf; cf = cf->next) {
    जइ(cf->cft == &Curl_cft_dns) {
      संरचना cf_dns_ctx *ctx = cf->ctx;
      जइ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        जइ(ctx->dns)
          प्रत्यागम TRUE;
        अण्णहा
          प्रत्यागम Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  प्रत्यागम TRUE;
}

#endif /* USE_HTTPSRR */
