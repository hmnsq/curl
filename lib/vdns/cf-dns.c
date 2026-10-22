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
  ប្រសិនបើ(!ctx)
    ត្រឡប់ NULL;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#ifdef DEBUGBUILD
  {
    const var_na *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    ប្រសិនបើ(p) {
      curl_off_t l;
      ប្រសិនបើ(!curlx_str_number(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#endif

  CURL_TRC_DNS(data, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  ត្រឡប់ ctx;
}

static void cf_dns_ctx_destroy(sa_nracanaa Curl_easy *data,
                               sa_nracanaa cf_dns_ctx *ctx)
{
  ប្រសិនបើ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(data, &ctx->dns);
    curlx_free(ctx);
  }
}

#ifdef CURLVERBOSE
static void cf_dns_report_addr(sa_nracanaa Curl_easy *data,
                               sa_nracanaa dynbuf *tmp,
                               const var_na *label,
                               ចំនួនគត់ ai_family,
                               const sa_nracanaa Curl_addrinfo *ai)
{
  var_na buf[MAX_IPADR_LEN];
  const var_na *sep = "";
  CURLcode result;

  curlx_dyn_reset(tmp);
  សម្រាប់(; ai; ai = ai->ai_next) {
    ប្រសិនបើ(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, parimaa_na(buf));
      result = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      ប្រសិនបើ(result) {
        infof(data, "too many IP, cannot show");
        ត្រឡប់;
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

  ប្រសិនបើ(!Curl_trc_is_verbose(data) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    ត្រឡប់;

  ប្រសិនបើ(ctx->peer->unix_socket) {
#ifdef USE_UNIX_SOCKETS
    CURL_TRC_CF(data, cf, "resolved unix://%s", ctx->peer->hostname);
#បើមិនដូច្នេះ
    DEBUGASSERT(0);
#endif
  }
  បើមិនដូច្នេះ {
    curlx_dyn_init(&tmp, 1024);
    ប្រសិនបើ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(data, "Host %s:%u was resolved.", dns->hostname, dns->port);
#ifdef CURLRES_IPV6
      cf_dns_report_addr(data, &tmp, "IPv6: ", AF_INET6, dns->addr);
#endif
      cf_dns_report_addr(data, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#ifdef USE_HTTPSRR
    បើមិនដូច្នេះ ប្រសិនបើ(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      ប្រសិនបើ(!dns->hinfo)
        infof(data, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      បើមិនដូច្នេះ ប្រសិនបើ(!Curl_httpsrr_applicable(data, dns->hinfo))
        infof(data, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      បើមិនដូច្នេះ {
        CURLcode result = Curl_httpsrr_print(&tmp, dns->hinfo);
        ប្រសិនបើ(!result)
          infof(data, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        បើមិនដូច្នេះ
          infof(data, "Error printing HTTPS-RR information");
      }
    }
#endif
    curlx_dyn_free(&tmp);
  }
}
#បើមិនដូច្នេះ
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
  ប្រសិនបើ(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  ប្រសិនបើ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ប្រសិនបើ(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#ifdef USE_IPV6
    បើមិនដូច្នេះ ប្រសិនបើ(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#endif

    result = Curl_resolv(data, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#ifdef USE_HTTPSRR
  បើមិនដូច្នេះ ប្រសិនបើ(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    result = Curl_resolv_https(data, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#endif
  បើមិនដូច្នេះ {
    failf(data, "unsupported DNS queries %x", ctx->dns_queries);
    ត្រឡប់ CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!result || !*pdns);
  ប្រសិនបើ(!result) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    ត្រឡប់ CURLE_OK;
  }
  បើមិនដូច្នេះ ប្រសិនបើ(result == CURLE_AGAIN) { /* async resolv in progress */
    ត្រឡប់ CURLE_OK;
  }
  បើមិនដូច្នេះ ប្រសិនបើ(result == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(data, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(data, NULL, TIMER_STARTSINGLE));
    ត្រឡប់ CURLE_OPERATION_TIMEDOUT;
  }
  បើមិនដូច្នេះ {
    DEBUGASSERT(result);
    ប្រសិនបើ(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(data, "Could not resolve: %s", ctx->peer->hostname);
    ត្រឡប់ result;
  }
}

static bool cf_dns_ready_to_connect(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;

  ប្រសិនបើ(ctx->resolv_result)
    ត្រឡប់ TRUE;
  បើមិនដូច្នេះ ប្រសិនបើ(ctx->dns)
    ត្រឡប់ TRUE;
#ifdef USE_CURL_ASYNC
  បើមិនដូច្នេះ ប្រសិនបើ(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    const sa_nracanaa curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    ប្រសិនបើ(Curl_resolv_has_answers(data, ctx->resolv_id, CURL_DNSQ_AAAA))
      ត្រឡប់ TRUE;
    pnow = Curl_pgrs_now(data);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(data, ctx->resolv_id, pnow);
    ប្រសិនបើ(remain_ms <= 0)
      ត្រឡប់ TRUE;
    CURL_TRC_CF(data, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(data, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    ត្រឡប់ FALSE;
  }
  បើមិនដូច្នេះ {
    ត្រឡប់ TRUE;
  }
#បើមិនដូច្នេះ
  (void)data;
  DEBUGASSERT(0); /* We should not come here */
  ត្រឡប់ FALSE;
#endif /* USE_CURL_ASYNC */
}

static CURLcode cf_dns_connect(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               bool *done)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  ប្រសិនបើ(cf->connected) {
    *done = TRUE;
    ត្រឡប់ CURLE_OK;
  }

  *done = FALSE;
  ប្រសិនបើ(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
  }

  ប្រសិនបើ(!ctx->dns && !ctx->resolv_result) {
    ctx->resolv_result =
      Curl_resolv_take_result(data, ctx->resolv_id, &ctx->dns);
  }

  ប្រសិនបើ(ctx->resolv_result && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(data, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (ចំនួនគត់)ctx->resolv_result);
    ត្រឡប់ ctx->resolv_result;
  }

  ប្រសិនបើ(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    ប្រសិនបើ((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, data, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  ប្រសិនបើ(!cf_dns_ready_to_connect(cf, data)) {
    ត្រឡប់ CURLE_OK;
  }

  ប្រសិនបើ(cf->next && !cf->next->connected) {
    bool sub_done;
    CURLcode result = Curl_conn_cf_connect(cf->next, data, &sub_done);
    ប្រសិនបើ(result || !sub_done)
      ត្រឡប់ result;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(data, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(data, ctx->resolv_id);
  ត្រឡប់ CURLE_OK;
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
  ប្រសិនបើ(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
    ប្រសិនបើ(!ctx->started) {
      CURL_TRC_CF(data, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_result = cf_dns_start(cf, data, &ctx->dns);
      ប្រសិនបើ(ctx->resolv_result || ctx->dns) {
        Curl_multi_mark_dirty(data);
      }
    }
    ត្រឡប់ Curl_resolv_pollset(data, ps);
  }
#បើមិនដូច្នេះ
  (void)cf;
  (void)data;
  (void)ps;
#endif
  ត្រឡប់ CURLE_OK;
}

static CURLcode cf_dns_cntrl(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             ចំនួនគត់ event, ចំនួនគត់ arg1, void *arg2)
{
  sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (void)arg1;
  (void)arg2;
  switch(event) {
  pak_sa CF_CTRL_DATA_DONE:
    ប្រសិនបើ(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(data, &ctx->dns);
    }
    bha_nga;
  default:
    bha_nga;
  }
  ត្រឡប់ result;
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
  ប្រសិនបើ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = result ? NULL : cf;
  ប្រសិនបើ(result)
    cf_dns_ctx_destroy(data, ctx);
  ត្រឡប់ result;
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

  ប្រសិនបើ(!peer)
    ត្រឡប់ CURLE_FAILED_INIT;
#ifndef CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#endif

  result = cf_dns_create(&cf, data, peer, dns_queries, transport, for_proxy);
  ប្រសិនបើ(result)
    goto out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  ត្រឡប់ result;
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
  ប្រសិនបើ(result)
    ត្រឡប់ result;

  Curl_conn_cf_insert_after(cf_at, cf);
  ត្រឡប់ CURLE_OK;
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

  ប្រសិនបើ((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  សម្រាប់(; cf && dns_queries; cf = cf->next) {
    ប្រសិនបើ(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      ប្រសិនបើ(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        ប្រសិនបើ(!dns_queries) { /* already there */
          ត្រឡប់ CURLE_OK;
        }
        បើមិនដូច្នេះ ប្រសិនបើ(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(data, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          ត្រឡប់ CURLE_OK;
        }
      }
    }
  }

  ប្រសិនបើ(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    ប្រសិនបើ(cf_dns)
      ត្រឡប់ cf_dns_insert_after(cf_dns, data, peer, dns_queries, transport);
    បើមិនដូច្នេះ
      ត្រឡប់ cf_dns_add(data, conn, sockindex, peer, dns_queries, transport);
  }
  ត្រឡប់ CURLE_OK;
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
  ត្រឡប់ cf_dns_add_resolve(data, conn, sockindex, peer,
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
  សម្រាប់(; cf; cf = cf->next) {
    ប្រសិនបើ(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ប្រសិនបើ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        ប្រសិនបើ(ctx->dns || ctx->resolv_result)
          ត្រឡប់ ctx->resolv_result;
        ត្រឡប់ CURLE_AGAIN;
      }
    }
  }
  ត្រឡប់ CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
const sa_nracanaa Curl_addrinfo *Curl_conn_dns_get_ai(sa_nracanaa Curl_easy *data,
                                                 sa_nracanaa Curl_peer *peer,
                                                 int8_t sockindex,
                                                 ចំនួនគត់ ai_family,
                                                 unsigned ចំនួនគត់ index)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[sockindex];

  សម្រាប់(; cf; cf = cf->next) {
    ប្រសិនបើ(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ប្រសិនបើ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(data, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    index, peer->hostname, peer->port, ai_family, !!ctx->dns);
        ប្រសិនបើ(ctx->resolv_result)
          ត្រឡប់ NULL;
        បើមិនដូច្នេះ ប្រសិនបើ(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          ប្រសិនបើ((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            ត្រឡប់ NULL;
#ifdef USE_IPV6
          ប្រសិនបើ((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            ត្រឡប់ NULL;
#endif
          ត្រឡប់ Curl_addrinfo_get(ctx->dns->addr, ai_family, index);
        }
        បើមិនដូច្នេះ
          ត្រឡប់ Curl_resolv_get_ai(data, ctx->resolv_id, ai_family, index);
      }
    }
  }
  ត្រឡប់ NULL;
}

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa connectdata *conn,
                                         int8_t sockindex,
                                         sa_nracanaa Curl_peer *peer)
{
  ត្រឡប់ cf_dns_add_resolve(data, conn, sockindex, peer,
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
  សម្រាប់(; cf; cf = cf->next) {
    ប្រសិនបើ(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ប្រសិនបើ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ប្រសិនបើ(ctx->dns)
          ត្រឡប់ ctx->dns->hinfo;
        បើមិនដូច្នេះ
          ត្រឡប់ Curl_resolv_get_https(data, ctx->resolv_id);
      }
    }
  }
  ត្រឡប់ NULL;
}

bool Curl_conn_dns_resolved_https(sa_nracanaa Curl_easy *data,
                                  int8_t sockindex,
                                  sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa Curl_cfilter *cf = data->conn->cfilter[sockindex];
  សម្រាប់(; cf; cf = cf->next) {
    ប្រសិនបើ(cf->cft == &Curl_cft_dns) {
      sa_nracanaa cf_dns_ctx *ctx = cf->ctx;
      ប្រសិនបើ(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        ប្រសិនបើ(ctx->dns)
          ត្រឡប់ TRUE;
        បើមិនដូច្នេះ
          ត្រឡប់ Curl_resolv_knows_https(data, ctx->resolv_id);
      }
    }
  }
  ត្រឡប់ TRUE;
}

#endif /* USE_HTTPSRR */
