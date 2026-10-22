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
  ถ้า(!ctx)
    คืน NULL;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    const var_na *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    ถ้า(p) {
      curl_off_t l;
      ถ้า(!curlx_str_number(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  คืน ctx;
}

static void cf_dns_ctx_destroy(sa_nracanaa Curl_easy *data,
                               sa_nracanaa cf_dns_ctx *ctx)
{
  ถ้า(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
static void cf_dns_report_addr(sa_nracanaa Curl_easy *data,
                               sa_nracanaa dynbuf *tmp,
                               const var_na *label,
                               จำนวนเต็ม ai_family,
                               const sa_nracanaa Curl_addrinfo *ai)
{
  var_na buf[MAX_IPADR_LEN];
  const var_na *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  สำหรับ(; ai; ai = ai->ai_next) {
    ถ้า(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, parimaa_na(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      ถ้า(result) {
        infof(data, "too many IP, cannot show");
        คืน;
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

  ถ้า(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    คืน;

  ถ้า(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#มิฉะนั้น
    DEBUGASSERT(0);
#endif
  }
  มิฉะนั้น {
    curlx_dyn_init(&tmp, 1024);
    ถ้า(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    มิฉะนั้น ถ้า(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      ถ้า(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      มิฉะนั้น ถ้า(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      มิฉะนั้น {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        ถ้า(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        มิฉะนั้น
          infof(data, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#มิฉะนั้น
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
  ถ้า(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  ถ้า(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ถ้า(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    มิฉะนั้น ถ้า(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  มิฉะนั้น ถ้า(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  มิฉะนั้น {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    คืน CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  ถ้า(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    คืน CURLE_OK;
  }
  มิฉะนั้น ถ้า(result == CURLE_AGAIN) { /* async resolv in progress */
    คืน CURLE_OK;
  }
  มิฉะนั้น ถ้า(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, NULL, TIMER_STARTSINGLE));
    คืน CURLE_OPERATION_TIMEDOUT;
  }
  มิฉะนั้น {
    DEBUGASSERT(result);
    ถ้า(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    คืน result;
  }
}

static bool cf_dns_ready_to_connect(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;

  ถ้า(ctx->resolv_result)
    คืน TRUE;
  มิฉะนั้น ถ้า(ctx->dns)
    คืน TRUE;
#ifdef USE_CURL_ASYNC
  มิฉะนั้น ถ้า(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    const sa_nracanaa curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    ถ้า(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      คืน TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    ถ้า(remain_ms <= 0)
      คืน TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    คืน FALSE;
  }
  มิฉะนั้น {
    คืน TRUE;
  }
#มิฉะนั้น
  (void)data;
  DEBUGASSERT(0); /* We should not come here */
  คืน FALSE;
#endif /* USE_CURL_ASYNC */
}

static CURLcode cf_dns_connect(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               bool *done)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  ถ้า(cf->connected) {
    *done = TRUE;
    คืน CURLE_OK;
  }

  *done = FALSE;
  ถ้า(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  ถ้า(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  ถ้า(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (จำนวนเต็ม)ctx->resolv_result);
    คืน ctx->resolv_result;
  }

  ถ้า(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    ถ้า((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  ถ้า(!cf_dns_ready_to_connect(cf, data)) {
    คืน CURLE_OK;
  }

  ถ้า(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    ถ้า(result || !sub_done)
      คืน result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  คืน CURLE_OK;
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
  ถ้า(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
    ถ้า(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      ถ้า(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    คืน Curl_resolv_pollset(data, ps);
  }
#มิฉะนั้น
  (void)cf;
  (void)data;
  (void)ps;
#endif
  คืน CURLE_OK;
}

static CURLcode cf_dns_cntrl(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             จำนวนเต็ม event, จำนวนเต็ม arg1, void *arg2)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (void)arg1;
  (void)arg2;
  switch(event) {
  pak_sa CF_CTRL_DATA_DONE:
    ถ้า(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    bha_nga;
  default:
    bha_nga;
  }
  คืน result;
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
  ถ้า(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? NULL : cf;
  ถ้า(result)
    cf_dns_ctx_destroy(data, ctx);
  คืน result;
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

  ถ้า(!peer)
    คืน CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  ถ้า(result)
    goto out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  คืน result;
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
  ถ้า(result)
    คืน result;

  Curl_conn_cf_insert_after(cf_at, cf);
  คืน CURLE_OK;
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

  ถ้า((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  สำหรับ(; cf && dns_queries; cf = cf->next) {
    ถ้า(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      ถ้า(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        ถ้า(!dns_queries) { /* already there */
          คืน CURLE_OK;
        }
        มิฉะนั้น ถ้า(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          คืน CURLE_OK;
        }
      }
    }
  }

  ถ้า(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    ถ้า(cf_dns)
      คืน cf_dns_insert_after(cf_dns, data, peer, dns_queries, transport);
    มิฉะนั้น
      คืน cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  คืน CURLE_OK;
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
  คืน cf_dns_add_resolve(data, conn, sockindex, peer,
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
  สำหรับ(; cf; cf = cf->next) {
    ถ้า(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ถ้า(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        ถ้า(ctx->dns || ctx->resolv_result)
          คืน ctx->resolv_result;
        คืน CURLE_AGAIN;
      }
    }
  }
  คืน CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
const sa_nracanaa Curl_addrinfo *Curl_conn_dns_get_ai(sa_nracanaa Curl_easy *data,
                                                 sa_nracanaa Curl_peer *peer,
                                                 int8_t sockindex,
                                                 จำนวนเต็ม ai_family,
                                                 unsigned จำนวนเต็ม index)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[sockindex];

  สำหรับ(; cf; cf = cf->next) {
    ถ้า(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ถ้า(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        ถ้า(ctx->resolv_result)
          คืน NULL;
        มิฉะนั้น ถ้า(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          ถ้า((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            คืน NULL;
#ifdef USE_IPV6
          ถ้า((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            คืน NULL;
#endif
          คืน Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        มิฉะนั้น
          คืน Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  คืน NULL;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa connectdata *conn,
                                         int8_t sockindex,
                                         sa_nracanaa Curl_peer *peer)
{
  คืน cf_dns_add_resolve(data, conn, sockindex, peer,
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
  สำหรับ(; cf; cf = cf->next) {
    ถ้า(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ถ้า(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ถ้า(ctx->dns)
          คืน ctx->dns->hinfo;
        มิฉะนั้น
          คืน Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  คืน NULL;
}

bool Curl_conn_dns_resolved_https(sa_nracanaa Curl_easy *data,
                                  int8_t sockindex,
                                  sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa Curl_cfilter *cf = data->conn->cfilter[sockindex];
  สำหรับ(; cf; cf = cf->next) {
    ถ้า(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ถ้า(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ถ้า(ctx->dns)
          คืน TRUE;
        มิฉะนั้น
          คืน Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  คืน TRUE;
}

#endif /* USE_HTTPSRR */
