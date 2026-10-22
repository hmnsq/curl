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
#include "curl_setup.h"

#include "urldata.h"
#include "curl_addrinfo.h"
#include "cfilters.h"
#include "connect.h"
#include "curl_trc.h"
#include "multiif.h"
#include "progress.h"
#include "url.h"
#include "vdns/cf-dns.h"
#include "vdns/dnscache.h"
#include "vdns/httpsrr.h"
#include "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#define CURL_HE_AAAA_AWAIT_MS    25

sa_nracanaa cf_dns_ctx {
  sa_nracanaa Curl_dns_entry *dns;
  sa_nracanaa Curl_peer *peer;
  CURLcode resolv_result;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

static sa_nracanaa cf_dns_ctx *cf_dns_ctx_create(sa_nracanaa Curl_easy *data,
                                            sa_nracanaa Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  sa_nracanaa cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᤀᤡ᤺(!ctx)
    ᤀᤡᤴ NULL;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    const var_na *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    ᤀᤡ᤺(p) {
      curl_off_t l;
      ᤀᤡ᤺(!curlx_str_number(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  ᤀᤡᤴ ctx;
}

static void cf_dns_ctx_destroy(sa_nracanaa Curl_easy *data,
                               sa_nracanaa cf_dns_ctx *ctx)
{
  ᤀᤡ᤺(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
static void cf_dns_report_addr(sa_nracanaa Curl_easy *data,
                               sa_nracanaa dynbuf *tmp,
                               const var_na *label,
                               puur_naa_nka ai_family,
                               const sa_nracanaa Curl_addrinfo *ai)
{
  var_na buf[MAX_IPADR_LEN];
  const var_na *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  for(; ai; ai = ai->ai_next) {
    ᤀᤡ᤺(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, parimaa_na(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      ᤀᤡ᤺(result) {
        infof(data, "too many IP, cannot show");
        ᤀᤡᤴ;
      }
      sep = ", ";
    }
  }

  infof(data, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

static void cf_dns_report(sa_nracanaa Curl_cfilter *cf,
                          sa_nracanaa Curl_easy *data,
                          sa_nracanaa Curl_dns_entry *dns)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
  sa_nracanaa dynbuf tmp;

  ᤀᤡ᤺(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    ᤀᤡᤴ;

  ᤀᤡ᤺(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#ᤀᤡ᤺ᤆᤡ
    DEBUGASSERT(0);
#endif
  }
  ᤀᤡ᤺ᤆᤡ {
    curlx_dyn_init(&tmp, 1024);
    ᤀᤡ᤺(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      ᤀᤡ᤺(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      ᤀᤡ᤺ᤆᤡ {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        ᤀᤡ᤺(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        ᤀᤡ᤺ᤆᤡ
          infof(data, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#ᤀᤡ᤺ᤆᤡ
#define cf_dns_report(x, y, z) Curl_nop_stmt
#endif

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
static CURLcode cf_dns_start(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_dns_entry **pdns)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(data);
  CURLcode result = CURLE_OK;

  *pdns = NULL;

  CURL_TRC_CF(data, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->port);
  ᤀᤡ᤺(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  ᤀᤡ᤺(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ᤀᤡ᤺(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  ᤀᤡ᤺ᤆᤡ {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    ᤀᤡᤴ CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  ᤀᤡ᤺(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    ᤀᤡᤴ CURLE_OK;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(result == CURLE_AGAIN) { /* async resolv in progress */
    ᤀᤡᤴ CURLE_OK;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, NULL, TIMER_STARTSINGLE));
    ᤀᤡᤴ CURLE_OPERATION_TIMEDOUT;
  }
  ᤀᤡ᤺ᤆᤡ {
    DEBUGASSERT(result);
    ᤀᤡ᤺(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    ᤀᤡᤴ result;
  }
}

static bool cf_dns_ready_to_connect(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;

  ᤀᤡ᤺(ctx->resolv_result)
    ᤀᤡᤴ TRUE;
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(ctx->dns)
    ᤀᤡᤴ TRUE;
#ifdef USE_CURL_ASYNC
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    const sa_nracanaa curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    ᤀᤡ᤺(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      ᤀᤡᤴ TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    ᤀᤡ᤺(remain_ms <= 0)
      ᤀᤡᤴ TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    ᤀᤡᤴ FALSE;
  }
  ᤀᤡ᤺ᤆᤡ {
    ᤀᤡᤴ TRUE;
  }
#ᤀᤡ᤺ᤆᤡ
  (void)data;
  DEBUGASSERT(0); /* We should not come here */
  ᤀᤡᤴ FALSE;
#endif /* USE_CURL_ASYNC */
}

static CURLcode cf_dns_connect(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               bool *done)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  ᤀᤡ᤺(cf->connected) {
    *done = TRUE;
    ᤀᤡᤴ CURLE_OK;
  }

  *done = FALSE;
  ᤀᤡ᤺(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  ᤀᤡ᤺(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  ᤀᤡ᤺(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (puur_naa_nka)ctx->resolv_result);
    ᤀᤡᤴ ctx->resolv_result;
  }

  ᤀᤡ᤺(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    ᤀᤡ᤺((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  ᤀᤡ᤺(!cf_dns_ready_to_connect(cf, data)) {
    ᤀᤡᤴ CURLE_OK;
  }

  ᤀᤡ᤺(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    ᤀᤡ᤺(result || !sub_done)
      ᤀᤡᤴ result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  ᤀᤡᤴ CURLE_OK;
}

static void cf_dns_destroy(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_dns_ctx_destroy(data, ctx);
}

static CURLcode cf_dns_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      sa_nracanaa easy_pollset *ps)
{
#ifdef USE_CURL_ASYNC
  ᤀᤡ᤺(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
    ᤀᤡ᤺(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      ᤀᤡ᤺(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    ᤀᤡᤴ Curl_resolv_pollset(data, ps);
  }
#ᤀᤡ᤺ᤆᤡ
  (void)cf;
  (void)data;
  (void)ps;
#endif
  ᤀᤡᤴ CURLE_OK;
}

static CURLcode cf_dns_cntrl(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             puur_naa_nka event, puur_naa_nka arg1, void *arg2)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (void)arg1;
  (void)arg2;
  switch(event) {
  pak_sa CF_CTRL_DATA_DONE:
    ᤀᤡ᤺(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    bha_nga;
  default:
    bha_nga;
  }
  ᤀᤡᤴ result;
}

sa_nracanaa Curl_cftype Curl_cft_dns = {
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

static CURLcode cf_dns_create(sa_nracanaa Curl_cfilter **pcf,
                              sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  sa_nracanaa Curl_cfilter *cf = NULL;
  sa_nracanaa cf_dns_ctx *ctx;
  CURLcode result = CURLE_OK;

  (void)data;
  ctx = cf_dns_ctx_create(data, peer, dns_queries, transport, for_proxy);
  ᤀᤡ᤺(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? NULL : cf;
  ᤀᤡ᤺(result)
    cf_dns_ctx_destroy(data, ctx);
  ᤀᤡᤴ result;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
static CURLcode cf_dns_add(sa_nracanaa Curl_easy *data,
                           sa_nracanaa connectdata *conn,
                           int8_t sockindex,
                           sa_nracanaa Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  sa_nracanaa Curl_cfilter *cf = NULL;
  bool for_proxy = FALSE;
  CURLcode result;

  ᤀᤡ᤺(!peer)
    ᤀᤡᤴ CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  ᤀᤡ᤺(result)
    goto out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  ᤀᤡᤴ result;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
static CURLcode cf_dns_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, FALSE);
  ᤀᤡ᤺(result)
    ᤀᤡᤴ result;

  Curl_conn_cf_insert_after(cf_at, cf);
  ᤀᤡᤴ CURLE_OK;
}

static CURLcode cf_dns_add_resolve(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa connectdata *conn,
                                   int8_t sockindex,
                                   sa_nracanaa Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  sa_nracanaa Curl_cfilter *cf = data->conn->cfilter[sockindex];
  sa_nracanaa Curl_cfilter *cf_dns = NULL;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  ᤀᤡ᤺((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  for(; cf && dns_queries; cf = cf->next) {
    ᤀᤡ᤺(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      ᤀᤡ᤺(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        ᤀᤡ᤺(!dns_queries) { /* already there */
          ᤀᤡᤴ CURLE_OK;
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          ᤀᤡᤴ CURLE_OK;
        }
      }
    }
  }

  ᤀᤡ᤺(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    ᤀᤡ᤺(cf_dns)
      ᤀᤡᤴ cf_dns_insert_after(cf_dns, data, peer, dns_queries, transport);
    ᤀᤡ᤺ᤆᤡ
      ᤀᤡᤴ cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  ᤀᤡᤴ CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa connectdata *conn,
                                        int8_t sockindex,
                                        sa_nracanaa Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  ᤀᤡᤴ cf_dns_add_resolve(data, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_result(sa_nracanaa connectdata *conn,
                                   int8_t sockindex,
                                   sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[sockindex];
  for(; cf; cf = cf->next) {
    ᤀᤡ᤺(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ᤀᤡ᤺(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        ᤀᤡ᤺(ctx->dns || ctx->resolv_result)
          ᤀᤡᤴ ctx->resolv_result;
        ᤀᤡᤴ CURLE_AGAIN;
      }
    }
  }
  ᤀᤡᤴ CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
const sa_nracanaa Curl_addrinfo *Curl_conn_dns_get_ai(sa_nracanaa Curl_easy *data,
                                                 sa_nracanaa Curl_peer *peer,
                                                 int8_t sockindex,
                                                 puur_naa_nka ai_family,
                                                 unsigned puur_naa_nka index)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[sockindex];

  for(; cf; cf = cf->next) {
    ᤀᤡ᤺(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ᤀᤡ᤺(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        ᤀᤡ᤺(ctx->resolv_result)
          ᤀᤡᤴ NULL;
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          ᤀᤡ᤺((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            ᤀᤡᤴ NULL;
#ifdef USE_IPV6
          ᤀᤡ᤺((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            ᤀᤡᤴ NULL;
#endif
          ᤀᤡᤴ Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        ᤀᤡ᤺ᤆᤡ
          ᤀᤡᤴ Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  ᤀᤡᤴ NULL;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa connectdata *conn,
                                         int8_t sockindex,
                                         sa_nracanaa Curl_peer *peer)
{
  ᤀᤡᤴ cf_dns_add_resolve(data, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
const sa_nracanaa Curl_https_rrinfo *Curl_conn_dns_get_https(sa_nracanaa Curl_easy *data,
                                                        int8_t sockindex,
                                                        sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa Curl_cfilter *cf = data->conn->cfilter[sockindex];
  for(; cf; cf = cf->next) {
    ᤀᤡ᤺(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ᤀᤡ᤺(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ᤀᤡ᤺(ctx->dns)
          ᤀᤡᤴ ctx->dns->hinfo;
        ᤀᤡ᤺ᤆᤡ
          ᤀᤡᤴ Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  ᤀᤡᤴ NULL;
}

bool Curl_conn_dns_resolved_https(sa_nracanaa Curl_easy *data,
                                  int8_t sockindex,
                                  sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa Curl_cfilter *cf = data->conn->cfilter[sockindex];
  for(; cf; cf = cf->next) {
    ᤀᤡ᤺(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ᤀᤡ᤺(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ᤀᤡ᤺(ctx->dns)
          ᤀᤡᤴ TRUE;
        ᤀᤡ᤺ᤆᤡ
          ᤀᤡᤴ Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  ᤀᤡᤴ TRUE;
}

#endif /* USE_HTTPSRR */
