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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "curl_addrinfo.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "curl_trc.h"
#समावेश "multiif.h"
#समावेश "progress.h"
#समावेश "url.h"
#समावेश "vdns/cf-dns.h"
#समावेश "vdns/dnscache.h"
#समावेश "vdns/httpsrr.h"
#समावेश "curlx/strparse.h"

/* Max time to wait for AAAA before connecting sub-filters, e.g.
 * letting cf-ip-happy.c do its work. */
#घोषणा CURL_HE_AAAA_AWAIT_MS    25

काष्ठा cf_dns_ctx {
  काष्ठा Curl_dns_entry *dns;
  काष्ठा Curl_peer *peer;
  CURLcode resolv_परिणाम;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

अटल काष्ठा cf_dns_ctx *cf_dns_ctx_create(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  काष्ठा cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx)
    वापस शून्य;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#अगर_घोषित DEBUGBUILD
  {
    स्थिर अक्षर *p = दो_पर्या("CURL_DBG_HE_AAAA_AWAIT_MS");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#पूर्ण_अगर

  CURL_TRC_DNS(तथ्य, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->port, ctx->transport);
  वापस ctx;
}

अटल व्योम cf_dns_ctx_destroy(काष्ठा Curl_easy *तथ्य,
                               काष्ठा cf_dns_ctx *ctx)
{
  अगर(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_entry_unlink(तथ्य, &ctx->dns);
    curlx_free(ctx);
  }
}

#अगर_घोषित CURLVERBOSE
अटल व्योम cf_dns_report_addr(काष्ठा Curl_easy *तथ्य,
                               काष्ठा dynbuf *tmp,
                               स्थिर अक्षर *label,
                               पूर्णांक ai_family,
                               स्थिर काष्ठा Curl_addrinfo *ai)
{
  अक्षर buf[MAX_IPADR_LEN];
  स्थिर अक्षर *sep = "";
  CURLcode परिणाम;

  curlx_dyn_reset(tmp);
  क्रम(; ai; ai = ai->ai_अग्र) {
    अगर(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, माप(buf));
      परिणाम = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      अगर(परिणाम) {
        infof(तथ्य, "too many IP, cannot show");
        वापस;
      }
      sep = ", ";
    }
  }

  infof(तथ्य, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

अटल व्योम cf_dns_report(काष्ठा Curl_cfilter *cf,
                          काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_dns_entry *dns)
{
  काष्ठा cf_dns_ctx *ctx = cf->ctx;
  काष्ठा dynbuf tmp;

  अगर(!Curl_trc_is_verbose(तथ्य) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    वापस;

  अगर(ctx->peer->unix_socket) {
#अगर_घोषित USE_UNIX_SOCKETS
    CURL_TRC_CF(तथ्य, cf, "resolved unix://%s", ctx->peer->hostname);
#अन्यथा
    DEBUGASSERT(0);
#पूर्ण_अगर
  }
  अन्यथा {
    curlx_dyn_init(&tmp, 1024);
    अगर(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(तथ्य, "Host %s:%u was resolved.", dns->hostname, dns->port);
#अगर_घोषित CURLRES_IPV6
      cf_dns_report_addr(तथ्य, &tmp, "IPv6: ", AF_INET6, dns->addr);
#पूर्ण_अगर
      cf_dns_report_addr(तथ्य, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#अगर_घोषित USE_HTTPSRR
    अन्यथा अगर(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      अगर(!dns->hinfo)
        infof(तथ्य, "HTTPS-RR %s:%u: -", dns->hostname, dns->port);
      अन्यथा अगर(!Curl_httpsrr_applicable(तथ्य, dns->hinfo))
        infof(तथ्य, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->port);
      अन्यथा {
        CURLcode परिणाम = Curl_httpsrr_print(&tmp, dns->hinfo);
        अगर(!परिणाम)
          infof(तथ्य, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->port, curlx_dyn_ptr(&tmp));
        अन्यथा
          infof(तथ्य, "Error printing HTTPS-RR information");
      }
    }
#पूर्ण_अगर
    curlx_dyn_free(&tmp);
  }
}
#अन्यथा
#घोषणा cf_dns_report(x, y, z) Curl_nop_stmt
#पूर्ण_अगर

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
अटल CURLcode cf_dns_start(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_dns_entry **pdns)
{
  काष्ठा cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(तथ्य);
  CURLcode परिणाम = CURLE_OK;

  *pdns = शून्य;

  CURL_TRC_CF(तथ्य, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->port);
  अगर(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  अगर(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    अगर(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#अगर_घोषित USE_IPV6
    अन्यथा अगर(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#पूर्ण_अगर

    परिणाम = Curl_resolv(तथ्य, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#अगर_घोषित USE_HTTPSRR
  अन्यथा अगर(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    परिणाम = Curl_resolv_https(तथ्य, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#पूर्ण_अगर
  अन्यथा {
    failf(तथ्य, "unsupported DNS queries %x", ctx->dns_queries);
    वापस CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!परिणाम || !*pdns);
  अगर(!परिणाम) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    वापस CURLE_OK;
  }
  अन्यथा अगर(परिणाम == CURLE_AGAIN) { /* async resolv in progress */
    वापस CURLE_OK;
  }
  अन्यथा अगर(परिणाम == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(तथ्य, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(तथ्य, शून्य, TIMER_STARTSINGLE));
    वापस CURLE_OPERATION_TIMEDOUT;
  }
  अन्यथा {
    DEBUGASSERT(परिणाम);
    अगर(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(तथ्य, "Could not resolve: %s", ctx->peer->hostname);
    वापस परिणाम;
  }
}

अटल bool cf_dns_ready_to_connect(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_dns_ctx *ctx = cf->ctx;

  अगर(ctx->resolv_परिणाम)
    वापस TRUE;
  अन्यथा अगर(ctx->dns)
    वापस TRUE;
#अगर_घोषित USE_CURL_ASYNC
  अन्यथा अगर(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    स्थिर काष्ठा curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    अगर(Curl_resolv_has_answers(तथ्य, ctx->resolv_id, CURL_DNSQ_AAAA))
      वापस TRUE;
    pnow = Curl_pgrs_now(तथ्य);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(तथ्य, ctx->resolv_id, pnow);
    अगर(remain_ms <= 0)
      वापस TRUE;
    CURL_TRC_CF(तथ्य, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_set(तथ्य, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    वापस FALSE;
  }
  अन्यथा {
    वापस TRUE;
  }
#अन्यथा
  (व्योम)तथ्य;
  DEBUGASSERT(0); /* We should not come here */
  वापस FALSE;
#पूर्ण_अगर /* USE_CURL_ASYNC */
}

अटल CURLcode cf_dns_connect(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               bool *done)
{
  काष्ठा cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  *done = FALSE;
  अगर(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_परिणाम = cf_dns_start(cf, तथ्य, &ctx->dns);
  }

  अगर(!ctx->dns && !ctx->resolv_परिणाम) {
    ctx->resolv_परिणाम =
      Curl_resolv_take_परिणाम(तथ्य, ctx->resolv_id, &ctx->dns);
  }

  अगर(ctx->resolv_परिणाम && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(तथ्य, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (पूर्णांक)ctx->resolv_परिणाम);
    वापस ctx->resolv_परिणाम;
  }

  अगर(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    अगर((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->bits.dns_resolved = TRUE;
      Curl_pgrsTime(तथ्य, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, तथ्य, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  अगर(!cf_dns_ready_to_connect(cf, तथ्य)) {
    वापस CURLE_OK;
  }

  अगर(cf->अग्र && !cf->अग्र->connected) {
    bool sub_done;
    CURLcode परिणाम = Curl_conn_cf_connect(cf->अग्र, तथ्य, &sub_done);
    अगर(परिणाम || !sub_done)
      वापस परिणाम;
    DEBUGASSERT(sub_done);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(तथ्य, cf, "connected filter chain below");
  *done = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(तथ्य, ctx->resolv_id);
  वापस CURLE_OK;
}

अटल व्योम cf_dns_destroy(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(तथ्य, cf, "destroy");
  cf_dns_ctx_destroy(तथ्य, ctx);
}

अटल CURLcode cf_dns_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा easy_pollset *ps)
{
#अगर_घोषित USE_CURL_ASYNC
  अगर(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    काष्ठा cf_dns_ctx *ctx = cf->ctx;
    अगर(!ctx->started) {
      CURL_TRC_CF(तथ्य, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->port,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_परिणाम = cf_dns_start(cf, तथ्य, &ctx->dns);
      अगर(ctx->resolv_परिणाम || ctx->dns) {
        Curl_multi_mark_dirty(तथ्य);
      }
    }
    वापस Curl_resolv_pollset(तथ्य, ps);
  }
#अन्यथा
  (व्योम)cf;
  (व्योम)तथ्य;
  (व्योम)ps;
#पूर्ण_अगर
  वापस CURLE_OK;
}

अटल CURLcode cf_dns_cntrl(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  काष्ठा cf_dns_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  (व्योम)arg1;
  (व्योम)arg2;
  चयन(event) {
  हाल CF_CTRL_तथ्य_DONE:
    अगर(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_entry_unlink(तथ्य, &ctx->dns);
    }
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस परिणाम;
}

काष्ठा Curl_cftype Curl_cft_dns = {
  "DNS",
  CF_TYPE_SETUP | CF_TYPE_DNS,
  CURL_LOG_LVL_NONE,
  cf_dns_destroy,
  cf_dns_connect,
  Curl_cf_def_shutdown,
  cf_dns_adjust_pollset,
  Curl_cf_def_तथ्य_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_dns_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

अटल CURLcode cf_dns_create(काष्ठा Curl_cfilter **pcf,
                              काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा cf_dns_ctx *ctx;
  CURLcode परिणाम = CURLE_OK;

  (व्योम)तथ्य;
  ctx = cf_dns_ctx_create(तथ्य, peer, dns_queries, transport, for_proxy);
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  परिणाम = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

out:
  *pcf = परिणाम ? शून्य : cf;
  अगर(परिणाम)
    cf_dns_ctx_destroy(तथ्य, ctx);
  वापस परिणाम;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
अटल CURLcode cf_dns_add(काष्ठा Curl_easy *तथ्य,
                           काष्ठा connectdata *conn,
                           int8_t sockindex,
                           काष्ठा Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  bool for_proxy = FALSE;
  CURLcode परिणाम;

  अगर(!peer)
    वापस CURLE_FAILED_INIT;
#अगर_अघोषित CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#पूर्ण_अगर

  परिणाम = cf_dns_create(&cf, तथ्य, peer, dns_queries, transport, for_proxy);
  अगर(परिणाम)
    जाओ out;
  Curl_conn_cf_add(तथ्य, conn, sockindex, cf);
out:
  वापस परिणाम;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
अटल CURLcode cf_dns_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम;

  परिणाम = cf_dns_create(&cf, तथ्य, peer, dns_queries, transport, FALSE);
  अगर(परिणाम)
    वापस परिणाम;

  Curl_conn_cf_जोड़ो_after(cf_at, cf);
  वापस CURLE_OK;
}

अटल CURLcode cf_dns_add_resolve(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा connectdata *conn,
                                   int8_t sockindex,
                                   काष्ठा Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  काष्ठा Curl_cfilter *cf = तथ्य->conn->cfilter[sockindex];
  काष्ठा Curl_cfilter *cf_dns = शून्य;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  अगर((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  क्रम(; cf && dns_queries; cf = cf->अग्र) {
    अगर(cf->cft == &Curl_cft_dns) {
      काष्ठा cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      अगर(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        अगर(!dns_queries) { /* already there */
          वापस CURLE_OK;
        }
        अन्यथा अगर(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(तथ्य, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->port);
          वापस CURLE_OK;
        }
      }
    }
  }

  अगर(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    अगर(cf_dns)
      वापस cf_dns_जोड़ो_after(cf_dns, तथ्य, peer, dns_queries, transport);
    अन्यथा
      वापस cf_dns_add(तथ्य, conn, sockindex, peer, dns_queries, transport);
  }
  वापस CURLE_OK;
}

CURLcode Curl_conn_dns_add_addr_resolve(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा connectdata *conn,
                                        int8_t sockindex,
                                        काष्ठा Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  वापस cf_dns_add_resolve(तथ्य, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_परिणाम(काष्ठा connectdata *conn,
                                   int8_t sockindex,
                                   काष्ठा Curl_peer *peer)
{
  काष्ठा Curl_cfilter *cf = conn->cfilter[sockindex];
  क्रम(; cf; cf = cf->अग्र) {
    अगर(cf->cft == &Curl_cft_dns) {
      काष्ठा cf_dns_ctx *ctx = cf->ctx;
      अगर(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        अगर(ctx->dns || ctx->resolv_परिणाम)
          वापस ctx->resolv_परिणाम;
        वापस CURLE_AGAIN;
      }
    }
  }
  वापस CURLE_FAILED_INIT; /* no one is resolving */
}

/* Return the addrinfo at `index` for the given `family` from the
 * first "resolve" filter at the connection. If the DNS resolving is
 * not done yet or if no address for the family exists, returns NULL.
 */
स्थिर काष्ठा Curl_addrinfo *Curl_conn_dns_get_ai(काष्ठा Curl_easy *तथ्य,
                                                 काष्ठा Curl_peer *peer,
                                                 int8_t sockindex,
                                                 पूर्णांक ai_family,
                                                 अचिन्हित पूर्णांक सूचकांक)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा Curl_cfilter *cf = conn->cfilter[sockindex];

  क्रम(; cf; cf = cf->अग्र) {
    अगर(cf->cft == &Curl_cft_dns) {
      काष्ठा cf_dns_ctx *ctx = cf->ctx;
      अगर(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(तथ्य, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    सूचकांक, peer->hostname, peer->port, ai_family, !!ctx->dns);
        अगर(ctx->resolv_परिणाम)
          वापस शून्य;
        अन्यथा अगर(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          अगर((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            वापस शून्य;
#अगर_घोषित USE_IPV6
          अगर((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            वापस शून्य;
#पूर्ण_अगर
          वापस Curl_addrinfo_get(ctx->dns->addr, ai_family, सूचकांक);
        }
        अन्यथा
          वापस Curl_resolv_get_ai(तथ्य, ctx->resolv_id, ai_family, सूचकांक);
      }
    }
  }
  वापस शून्य;
}

#अगर_घोषित USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा connectdata *conn,
                                         int8_t sockindex,
                                         काष्ठा Curl_peer *peer)
{
  वापस cf_dns_add_resolve(तथ्य, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
स्थिर काष्ठा Curl_https_rrinfo *Curl_conn_dns_get_https(काष्ठा Curl_easy *तथ्य,
                                                        int8_t sockindex,
                                                        काष्ठा Curl_peer *peer)
{
  काष्ठा Curl_cfilter *cf = तथ्य->conn->cfilter[sockindex];
  क्रम(; cf; cf = cf->अग्र) {
    अगर(cf->cft == &Curl_cft_dns) {
      काष्ठा cf_dns_ctx *ctx = cf->ctx;
      अगर(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        अगर(ctx->dns)
          वापस ctx->dns->hinfo;
        अन्यथा
          वापस Curl_resolv_get_https(तथ्य, ctx->resolv_id);
      }
    }
  }
  वापस शून्य;
}

bool Curl_conn_dns_resolved_https(काष्ठा Curl_easy *तथ्य,
                                  int8_t sockindex,
                                  काष्ठा Curl_peer *peer)
{
  काष्ठा Curl_cfilter *cf = तथ्य->conn->cfilter[sockindex];
  क्रम(; cf; cf = cf->अग्र) {
    अगर(cf->cft == &Curl_cft_dns) {
      काष्ठा cf_dns_ctx *ctx = cf->ctx;
      अगर(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        अगर(ctx->dns)
          वापस TRUE;
        अन्यथा
          वापस Curl_resolv_knows_https(तथ्य, ctx->resolv_id);
      }
    }
  }
  वापस TRUE;
}

#पूर्ण_अगर /* USE_HTTPSRR */
