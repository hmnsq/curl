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
#परिभाषय CURL_HE_AAAA_AWAIT_MS    25

संरचना cf_dns_ctx {
  संरचना Curl_dns_प्रविष्टिः *dns;
  संरचना Curl_peer *peer;
  CURLcode resolv_परिणाम;
  timediff_t he_aaaa_await_ms;
  uint32_t resolv_id;
  uint8_t dns_queries;
  uint8_t transport;
  BIT(started);
  BIT(announced);
  BIT(for_proxy);
};

स्थिर संरचना cf_dns_ctx *cf_dns_ctx_create(संरचना Curl_easy *दत्त,
                                            संरचना Curl_peer *peer,
                                            uint8_t dns_queries,
                                            uint8_t transport,
                                            bool for_proxy)
{
  संरचना cf_dns_ctx *ctx;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx)
    प्रत्यागम रिक्त;

  Curl_peer_link(&ctx->peer, peer);
  ctx->dns_queries = dns_queries;
  ctx->transport = transport;
  ctx->for_proxy = for_proxy;
  ctx->he_aaaa_await_ms = CURL_HE_AAAA_AWAIT_MS;
#यदि_परिभाषितम् DEBUGBUILD
  {
    ध्रुव वर्ण *p = getenv("CURL_DBG_HE_AAAA_AWAIT_MS");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, UINT32_MAX)) {
        ctx->he_aaaa_await_ms = (uint32_t)l;
      }
    }
  }
#यदि_अन्तः

  CURL_TRC_DNS(दत्त, "[%s] created DNS filter for %s:%u, transport=%x",
               Curl_resolv_query_str(ctx->dns_queries),
               peer->hostname, peer->पत्तनम्, ctx->transport);
  प्रत्यागम ctx;
}

स्थिर शून्य cf_dns_ctx_destroy(संरचना Curl_easy *दत्त,
                               संरचना cf_dns_ctx *ctx)
{
  यदि(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_dns_प्रविष्टिः_unlink(दत्त, &ctx->dns);
    curlx_free(ctx);
  }
}

#यदि_परिभाषितम् CURLVERBOSE
स्थिर शून्य cf_dns_report_addr(संरचना Curl_easy *दत्त,
                               संरचना dynbuf *tmp,
                               ध्रुव वर्ण *label,
                               पूर्णाङ्क ai_family,
                               ध्रुव संरचना Curl_addrinfo *ai)
{
  वर्ण buf[MAX_IPADR_LEN];
  ध्रुव वर्ण *sep = "";
  CURLcode परिणाम;

  curlx_dyn_reset(tmp);
  प्रति(; ai; ai = ai->ai_अग्र) {
    यदि(ai->ai_family == ai_family) {
      Curl_printable_स्थानाङ्कः(ai, buf, परिमाण(buf));
      परिणाम = curlx_dyn_addf(tmp, "%s%s", sep, buf);
      यदि(परिणाम) {
        infof(दत्त, "too many IP, cannot show");
        प्रत्यागम;
      }
      sep = ", ";
    }
  }

  infof(दत्त, "%s%s", label,
        (curlx_dyn_len(tmp) ? curlx_dyn_ptr(tmp) : "(none)"));
}

स्थिर शून्य cf_dns_report(संरचना Curl_cfilter *cf,
                          संरचना Curl_easy *दत्त,
                          संरचना Curl_dns_प्रविष्टिः *dns)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;
  संरचना dynbuf tmp;

  यदि(!Curl_trc_is_verbose(दत्त) ||
     /* ignore no name or numerical IP addresses */
     !dns->hostname[0] || Curl_host_is_ipnum(dns->hostname))
    प्रत्यागम;

  यदि(ctx->peer->unix_socket) {
#यदि_परिभाषितम् USE_UNIX_SOCKETS
    CURL_TRC_CF(दत्त, cf, "resolved unix://%s", ctx->peer->hostname);
#अन्यथा
    DEBUGASSERT(0);
#यदि_अन्तः
  }
  अन्यथा {
    curlx_dyn_init(&tmp, 1024);
    यदि(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
      infof(दत्त, "Host %s:%u was resolved.", dns->hostname, dns->पत्तनम्);
#यदि_परिभाषितम् CURLRES_IPV6
      cf_dns_report_addr(दत्त, &tmp, "IPv6: ", AF_INET6, dns->addr);
#यदि_अन्तः
      cf_dns_report_addr(दत्त, &tmp, "IPv4: ", AF_INET, dns->addr);
    }
#यदि_परिभाषितम् USE_HTTPSRR
    अन्यथा यदि(ctx->dns_queries & CURL_DNSQ_HTTPS) {
      यदि(!dns->hinfo)
        infof(दत्त, "HTTPS-RR %s:%u: -", dns->hostname, dns->पत्तनम्);
      अन्यथा यदि(!Curl_httpsrr_applicable(दत्त, dns->hinfo))
        infof(दत्त, "HTTPS-RR %s:%u: not applicable",
              dns->hostname, dns->पत्तनम्);
      अन्यथा {
        CURLcode परिणाम = Curl_httpsrr_print(&tmp, dns->hinfo);
        यदि(!परिणाम)
          infof(दत्त, "HTTPS-RR %s:%u: %s",
                dns->hostname, dns->पत्तनम्, curlx_dyn_ptr(&tmp));
        अन्यथा
          infof(दत्त, "Error printing HTTPS-RR information");
      }
    }
#यदि_अन्तः
    curlx_dyn_free(&tmp);
  }
}
#अन्यथा
#परिभाषय cf_dns_report(x, y, z) Curl_nop_stmt
#यदि_अन्तः

/*************************************************************
 * Resolve the address of the server or proxy
 *************************************************************/
स्थिर CURLcode cf_dns_start(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             संरचना Curl_dns_प्रविष्टिः **pdns)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = Curl_timeleft_ms(दत्त);
  CURLcode परिणाम = CURLE_OK;

  *pdns = रिक्त;

  CURL_TRC_CF(दत्त, cf, "[%s] cf_dns_start %s %s:%u",
              Curl_resolv_query_str(ctx->dns_queries),
              ctx->peer->unix_socket ? "unix-domain-socket" : "host",
              ctx->peer->hostname, ctx->peer->पत्तनम्);
  यदि(ctx->peer->unix_socket)
    ctx->dns_queries = CURL_DNSQ_A; /* treat it like an A resolve */

  यदि(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    यदि(Curl_is_ipv4addr(ctx->peer->hostname))
      ctx->dns_queries |= CURL_DNSQ_A;
#यदि_परिभाषितम् USE_IPV6
    अन्यथा यदि(ctx->peer->ipv6)
      ctx->dns_queries |= CURL_DNSQ_AAAA;
#यदि_अन्तः

    परिणाम = Curl_resolv(दत्त, ctx->peer, ctx->dns_queries, ctx->transport,
                         (bool)ctx->for_proxy, timeout_ms,
                         &ctx->resolv_id, pdns);
  }
#यदि_परिभाषितम् USE_HTTPSRR
  अन्यथा यदि(ctx->dns_queries == CURL_DNSQ_HTTPS) {
    परिणाम = Curl_resolv_https(दत्त, ctx->peer, (bool)ctx->for_proxy,
                               timeout_ms, &ctx->resolv_id, pdns);
  }
#यदि_अन्तः
  अन्यथा {
    failf(दत्त, "unsupported DNS queries %x", ctx->dns_queries);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  DEBUGASSERT(!परिणाम || !*pdns);
  यदि(!परिणाम) { /* resolved right away, either sync or from dnscache */
    DEBUGASSERT(*pdns);
    प्रत्यागम CURLE_OK;
  }
  अन्यथा यदि(परिणाम == CURLE_AGAIN) { /* async resolv in progress */
    प्रत्यागम CURLE_OK;
  }
  अन्यथा यदि(परिणाम == CURLE_OPERATION_TIMEDOUT) { /* took too long */
    failf(दत्त, "Failed to resolve '%s' with timeout after %"
          FMT_TIMEDIFF_T " ms", ctx->peer->hostname,
          Curl_pgrs_since_ms(दत्त, रिक्त, TIMER_STARTSINGLE));
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;
  }
  अन्यथा {
    DEBUGASSERT(परिणाम);
    यदि(ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))
      failf(दत्त, "Could not resolve: %s", ctx->peer->hostname);
    प्रत्यागम परिणाम;
  }
}

स्थिर bool cf_dns_ready_गन्तव्यम्_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;

  यदि(ctx->resolv_परिणाम)
    प्रत्यागम TRUE;
  अन्यथा यदि(ctx->dns)
    प्रत्यागम TRUE;
#यदि_परिभाषितम् USE_CURL_ASYNC
  अन्यथा यदि(CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
    ध्रुव संरचना curltime *pnow;
    timediff_t remain_ms;
    /* For Happy Eyeballing, we can start on either A or AAAA resolves,
     * but AAAA is preferred. We enforce a small delay for missing
     * AAAA to arrive, then we let the connect continue.
     * Note: if AAAA was never started (-4), it is considered to have
     * an answer (e.g. a negative one). */
    यदि(Curl_resolv_has_answers(दत्त, ctx->resolv_id, CURL_DNSQ_AAAA))
      प्रत्यागम TRUE;
    pnow = Curl_pgrs_now(दत्त);
    remain_ms = ctx->he_aaaa_await_ms -
                Curl_resolv_elapsed_ms(दत्त, ctx->resolv_id, pnow);
    यदि(remain_ms <= 0)
      प्रत्यागम TRUE;
    CURL_TRC_CF(दत्त, cf, "[%s] still waiting %" FMT_TIMEDIFF_T
                "ms for AAAA result",
                Curl_resolv_query_str(ctx->dns_queries), remain_ms);
    Curl_expire_समुच्चयः(दत्त, EXPIRE_HAPPY_EYEBALLS, remain_ms, pnow);
    प्रत्यागम FALSE;
  }
  अन्यथा {
    प्रत्यागम TRUE;
  }
#अन्यथा
  (शून्य)दत्त;
  DEBUGASSERT(0); /* We should not come here */
  प्रत्यागम FALSE;
#यदि_अन्तः /* USE_CURL_ASYNC */
}

स्थिर CURLcode cf_dns_connect(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               bool *समाप्तम्)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;
  bool ip_query = (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA));

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *समाप्तम् = FALSE;
  यदि(!ctx->started) {
    ctx->started = TRUE;
    ctx->resolv_परिणाम = cf_dns_start(cf, दत्त, &ctx->dns);
  }

  यदि(!ctx->dns && !ctx->resolv_परिणाम) {
    ctx->resolv_परिणाम =
      Curl_resolv_take_परिणाम(दत्त, ctx->resolv_id, &ctx->dns);
  }

  यदि(ctx->resolv_परिणाम && ip_query) {
    /* failing A|AAAA resolves is a hard failure. */
    CURL_TRC_CF(दत्त, cf, "[%s] error resolving: %d",
                Curl_resolv_query_str(ctx->dns_queries),
                (पूर्णाङ्क)ctx->resolv_परिणाम);
    प्रत्यागम ctx->resolv_परिणाम;
  }

  यदि(ctx->dns && !ctx->announced) {
    ctx->announced = TRUE;
    यदि((cf->sockindex == FIRSTSOCKET) && ip_query) {
      cf->conn->द्वयङ्काः.dns_resolved = TRUE;
      Curl_pgrsTime(दत्त, TIMER_NAMELOOKUP);
    }
    cf_dns_report(cf, दत्त, ctx->dns);
  }

  /* Delay connection sub-filters when we are still waiting for AAAA */
  यदि(!cf_dns_ready_गन्तव्यम्_connect(cf, दत्त)) {
    प्रत्यागम CURLE_OK;
  }

  यदि(cf->अग्र && !cf->अग्र->connected) {
    bool sub_समाप्तम्;
    CURLcode परिणाम = Curl_conn_cf_connect(cf->अग्र, दत्त, &sub_समाप्तम्);
    यदि(परिणाम || !sub_समाप्तम्)
      प्रत्यागम परिणाम;
    DEBUGASSERT(sub_समाप्तम्);
  }

  /* sub filter chain is connected, this means the filter has done
   * its work and is connected as well, if it has results or not. */
  CURL_TRC_CF(दत्त, cf, "connected filter chain below");
  *समाप्तम् = TRUE;
  cf->connected = TRUE;
  Curl_resolv_destroy(दत्त, ctx->resolv_id);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_dns_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;

  CURL_TRC_CF(दत्त, cf, "destroy");
  cf_dns_ctx_destroy(दत्त, ctx);
}

स्थिर CURLcode cf_dns_adjust_pollset(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      संरचना easy_pollset *ps)
{
#यदि_परिभाषितम् USE_CURL_ASYNC
  यदि(!cf->connected) {
    /* The pollset only works when we have started the resolving.
     * Otherwise we might wait on the WAKEUP socketpair forever.
     * Since DNS filters can be added in the middle of a connect
     * attempt, they are not always started that way. */
    संरचना cf_dns_ctx *ctx = cf->ctx;
    यदि(!ctx->started) {
      CURL_TRC_CF(दत्त, cf, "adjust_pollset, starting %s:%u queries=%s",
                  ctx->peer->hostname, ctx->peer->पत्तनम्,
                  Curl_resolv_query_str(ctx->dns_queries));
      ctx->started = TRUE;
      ctx->resolv_परिणाम = cf_dns_start(cf, दत्त, &ctx->dns);
      यदि(ctx->resolv_परिणाम || ctx->dns) {
        Curl_multi_mark_dirty(दत्त);
      }
    }
    प्रत्यागम Curl_resolv_pollset(दत्त, ps);
  }
#अन्यथा
  (शून्य)cf;
  (शून्य)दत्त;
  (शून्य)ps;
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_dns_cntrl(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना cf_dns_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  (शून्य)arg1;
  (शून्य)arg2;
  विकल्प(event) {
  पक्ष CF_CTRL_दत्त_समाप्तम्:
    यदि(ctx->dns) {
      /* Should only come here when the connect attempt failed and
       * `data` is giving up on it. On a successful connect, we already
       * unlinked the DNS entry. */
      Curl_dns_प्रविष्टिः_unlink(दत्त, &ctx->dns);
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

संरचना Curl_cftype Curl_cft_dns = {
  "DNS",
  CF_TYPE_सज्जता | CF_TYPE_DNS,
  CURL_LOG_LVL_NONE,
  cf_dns_destroy,
  cf_dns_connect,
  Curl_cf_def_shutdown,
  cf_dns_adjust_pollset,
  Curl_cf_def_दत्त_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_dns_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

स्थिर CURLcode cf_dns_create(संरचना Curl_cfilter **pcf,
                              संरचना Curl_easy *दत्त,
                              संरचना Curl_peer *peer,
                              uint8_t dns_queries,
                              uint8_t transport,
                              bool for_proxy)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_dns_ctx *ctx;
  CURLcode परिणाम = CURLE_OK;

  (शून्य)दत्त;
  ctx = cf_dns_ctx_create(दत्त, peer, dns_queries, transport, for_proxy);
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = Curl_cf_create(&cf, &Curl_cft_dns, ctx);

बहिर्गमनम्:
  *pcf = परिणाम ? रिक्त : cf;
  यदि(परिणाम)
    cf_dns_ctx_destroy(दत्त, ctx);
  प्रत्यागम परिणाम;
}

/* Adds a "resolv" filter at the top of the connection's filter chain.
 * The filter will resolve the peer on the first connect attempt. */
स्थिर CURLcode cf_dns_सङ्कलय(संरचना Curl_easy *दत्त,
                           संरचना connectdata *conn,
                           int8_t sockindex,
                           संरचना Curl_peer *peer,
                           uint8_t dns_queries,
                           uint8_t transport)
{
  संरचना Curl_cfilter *cf = रिक्त;
  bool for_proxy = FALSE;
  CURLcode परिणाम;

  यदि(!peer)
    प्रत्यागम CURLE_FAILED_INIT;
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  for_proxy = (peer == conn->socks_proxy.peer) ||
              (peer == conn->http_proxy.peer);
#यदि_अन्तः

  परिणाम = cf_dns_create(&cf, दत्त, peer, dns_queries, transport, for_proxy);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  Curl_conn_cf_सङ्कलय(दत्त, conn, sockindex, cf);
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

/* Insert a new "resolv" filter directly after `cf`. It will
 * start a DNS resolve for the given peer on the
 * first connect attempt.
 * See socks.c on how this is used to make a non-blocking DNS
 * resolve during connect.
 */
स्थिर CURLcode cf_dns_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                    संरचना Curl_easy *दत्त,
                                    संरचना Curl_peer *peer,
                                    uint8_t dns_queries,
                                    uint8_t transport)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम;

  परिणाम = cf_dns_create(&cf, दत्त, peer, dns_queries, transport, FALSE);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  Curl_conn_cf_निवेशय_after(cf_at, cf);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_dns_सङ्कलय_resolve(संरचना Curl_easy *दत्त,
                                   संरचना connectdata *conn,
                                   int8_t sockindex,
                                   संरचना Curl_peer *peer,
                                   uint8_t dns_queries,
                                   uint8_t transport)
{
  संरचना Curl_cfilter *cf = दत्त->conn->cfilter[sockindex];
  संरचना Curl_cfilter *cf_dns = रिक्त;
  bool is_addr = CURL_DNSQ_IS_ADDR(dns_queries);

  यदि((dns_queries & CURL_DNSQ_HTTPS) &&
     Curl_is_ipaddr(peer->hostname)) {
    dns_queries = (uint8_t)(dns_queries & ~CURL_DNSQ_HTTPS);
  }

  प्रति(; cf && dns_queries; cf = cf->अग्र) {
    यदि(cf->cft == &Curl_cft_dns) {
      संरचना cf_dns_ctx *ctx = cf->ctx;
      cf_dns = cf;
      यदि(Curl_peer_same_destination(ctx->peer, peer)) {
        /* subtract queries already being scheduled/ongoing */
        dns_queries = (uint8_t)(~ctx->dns_queries & dns_queries);
        यदि(!dns_queries) { /* already there */
          प्रत्यागम CURLE_OK;
        }
        अन्यथा यदि(is_addr && !ctx->started &&
                CURL_DNSQ_IS_ADDR(ctx->dns_queries)) {
          ctx->dns_queries |= dns_queries;
          CURL_TRC_DNS(दत्त, "[%s] added queries=%s for %s:%u",
                       Curl_resolv_query_str(ctx->dns_queries),
                       Curl_resolv_query_str(dns_queries),
                       peer->hostname, peer->पत्तनम्);
          प्रत्यागम CURLE_OK;
        }
      }
    }
  }

  यदि(dns_queries) {
    /* No existing filter is handling these, add a new DNS filter
     * (after the last one if there was one already. FCFS. */
    यदि(cf_dns)
      प्रत्यागम cf_dns_निवेशय_after(cf_dns, दत्त, peer, dns_queries, transport);
    अन्यथा
      प्रत्यागम cf_dns_सङ्कलय(दत्त, conn, sockindex, peer, dns_queries, transport);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_conn_dns_सङ्कलय_addr_resolve(संरचना Curl_easy *दत्त,
                                        संरचना connectdata *conn,
                                        int8_t sockindex,
                                        संरचना Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport)
{
  /* should only have address query bits */
  DEBUGASSERT(!(dns_queries & ~CURL_DNSQ_ADDR));
  प्रत्यागम cf_dns_सङ्कलय_resolve(दत्त, conn, sockindex, peer,
                            (dns_queries & CURL_DNSQ_ADDR), transport);
}

/* Return the result of the DNS address resolution for peer.
 * Searches for a DNS filter from the top of the filter chain down. Returns
 * - CURLE_AGAIN when not done yet
 * - CURLE_OK when DNS was successfully resolved
 * - CURLR_FAILED_INIT when no resolv filter was found
 * - error returned by the DNS resolv
 */
CURLcode Curl_conn_dns_addr_परिणाम(संरचना connectdata *conn,
                                   int8_t sockindex,
                                   संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf = conn->cfilter[sockindex];
  प्रति(; cf; cf = cf->अग्र) {
    यदि(cf->cft == &Curl_cft_dns) {
      संरचना cf_dns_ctx *ctx = cf->ctx;
      यदि(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        यदि(ctx->dns || ctx->resolv_परिणाम)
          प्रत्यागम ctx->resolv_परिणाम;
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
ध्रुव संरचना Curl_addrinfo *Curl_conn_dns_get_ai(संरचना Curl_easy *दत्त,
                                                 संरचना Curl_peer *peer,
                                                 int8_t sockindex,
                                                 पूर्णाङ्क ai_family,
                                                 अचिह्नित पूर्णाङ्क सूचकांक)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना Curl_cfilter *cf = conn->cfilter[sockindex];

  प्रति(; cf; cf = cf->अग्र) {
    यदि(cf->cft == &Curl_cft_dns) {
      संरचना cf_dns_ctx *ctx = cf->ctx;
      यदि(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA))) {
        CURL_TRC_CF(दत्त, cf, "get %uth result for %s:%u, family=%d, dns=%d",
                    सूचकांक, peer->hostname, peer->पत्तनम्, ai_family, !!ctx->dns);
        यदि(ctx->resolv_परिणाम)
          प्रत्यागम रिक्त;
        अन्यथा यदि(ctx->dns) {
          /* A cached DNS entry may contain address families that we
           * here never queried for. We want to give no results for those. */
          यदि((ai_family == AF_INET) && !(ctx->dns_queries & CURL_DNSQ_A))
            प्रत्यागम रिक्त;
#यदि_परिभाषितम् USE_IPV6
          यदि((ai_family == AF_INET6) && !(ctx->dns_queries & CURL_DNSQ_AAAA))
            प्रत्यागम रिक्त;
#यदि_अन्तः
          प्रत्यागम Curl_addrinfo_get(ctx->dns->addr, ai_family, सूचकांक);
        }
        अन्यथा
          प्रत्यागम Curl_resolv_get_ai(दत्त, ctx->resolv_id, ai_family, सूचकांक);
      }
    }
  }
  प्रत्यागम रिक्त;
}

#यदि_परिभाषितम् USE_HTTPSRR
CURLcode Curl_conn_dns_सङ्कलय_https_resolve(संरचना Curl_easy *दत्त,
                                         संरचना connectdata *conn,
                                         int8_t sockindex,
                                         संरचना Curl_peer *peer)
{
  प्रत्यागम cf_dns_सङ्कलय_resolve(दत्त, conn, sockindex, peer,
                            CURL_DNSQ_HTTPS, conn->transport_wanted);
}

/* Return the HTTPS-RR info from the first "resolve" filter at the
 * connection. If the DNS resolving is not done yet or if there
 * is no HTTPS-RR info, returns NULL.
 */
ध्रुव संरचना Curl_https_rrinfo *Curl_conn_dns_get_https(संरचना Curl_easy *दत्त,
                                                        int8_t sockindex,
                                                        संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf = दत्त->conn->cfilter[sockindex];
  प्रति(; cf; cf = cf->अग्र) {
    यदि(cf->cft == &Curl_cft_dns) {
      संरचना cf_dns_ctx *ctx = cf->ctx;
      यदि(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        यदि(ctx->dns)
          प्रत्यागम ctx->dns->hinfo;
        अन्यथा
          प्रत्यागम Curl_resolv_get_https(दत्त, ctx->resolv_id);
      }
    }
  }
  प्रत्यागम रिक्त;
}

bool Curl_conn_dns_resolved_https(संरचना Curl_easy *दत्त,
                                  int8_t sockindex,
                                  संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf = दत्त->conn->cfilter[sockindex];
  प्रति(; cf; cf = cf->अग्र) {
    यदि(cf->cft == &Curl_cft_dns) {
      संरचना cf_dns_ctx *ctx = cf->ctx;
      यदि(Curl_peer_same_destination(ctx->peer, peer) &&
         (ctx->dns_queries & CURL_DNSQ_HTTPS)) {
        यदि(ctx->dns)
          प्रत्यागम TRUE;
        अन्यथा
          प्रत्यागम Curl_resolv_knows_https(दत्त, ctx->resolv_id);
      }
    }
  }
  प्रत्यागम TRUE;
}

#यदि_अन्तः /* USE_HTTPSRR */
