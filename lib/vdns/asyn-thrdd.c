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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GETADDRINFO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESOLVER_ENOMEM  SOCKENOMEM
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_threads.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "thrdqueue.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/wait.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
#ਅਨ੍ਤਰ੍ਭਾਵ <ares.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_HTTPSRR_ARES  /* the combo */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
ਪੂਰਨ Curl_async_global_init(ਖਾਲੀ)
{
#ਜੇ defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ਜੇ(ares_library_init(ARES_LIB_INIT_ALL)) {
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
ਖਾਲੀ Curl_async_global_cleanup(ਖਾਲੀ)
{
#ਜੇ defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASYN_ਵਸਤੂ_DESC_LEN   64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ async_ਵਸਤੂ_description(x)   (x)->description
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ async_ਵਸਤੂ_description(x)   ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਬਣਤਰ async_thrdd_ਵਸਤੂ {
  ਬਣਤਰ Curl_addrinfo *res;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਅੱਖਰ description[CURL_ASYN_ਵਸਤੂ_DESC_LEN];
#ਜੇ_ਅੰਤ
  ਪੂਰਨ sockerr;
  uint32_t mid;
  uint32_t resolv_id;
  uint16_t ਪੋਰਟ;
  uint8_t transport;
  uint8_t dns_queries;
  ਦੋਆਧਾਰੀ_ਅੰਕ(negative); /* resolver answered that the name does not exist */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  uint32_t delay_ms;
  uint32_t delay_fail_ms;
  ਦੋਆਧਾਰੀ_ਅੰਕ(dbg_negative);
#ਜੇ_ਅੰਤ
  ਅੱਖਰ hostname[1];
};

/* Give up reference to add_ctx */
ਸਥਿਰ ਖਾਲੀ async_thrdd_ਵਸਤੂ_destroy(ਬਣਤਰ async_thrdd_ਵਸਤੂ *ਵਸਤੂ)
{
  ਜੇ(ਵਸਤੂ) {
    ਜੇ(ਵਸਤੂ->res)
      Curl_freeaddrinfo(ਵਸਤੂ->res);
    curlx_free(ਵਸਤੂ);
  }
}

/* Initialize context for threaded resolver */
ਸਥਿਰ ਬਣਤਰ async_thrdd_ਵਸਤੂ *async_thrdd_ਵਸਤੂ_create(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  uint32_t resolv_id, uint8_t dns_queries,
  ਅਚਲ ਅੱਖਰ *hostname, uint16_t ਪੋਰਟ,
  uint8_t transport)
{
  size_t hostlen = ਲੜੀ_ਲੰਬਾਈ(hostname);
  ਬਣਤਰ async_thrdd_ਵਸਤੂ *ਵਸਤੂ;

  ਵਸਤੂ = curlx_calloc(1, ਆਕਾਰ(*ਵਸਤੂ) + hostlen);
  ਜੇ(!ਵਸਤੂ)
    ਵਾਪਸ ਨਿਰਾ;

  ਜੇ(hostlen) /* null-terminator of name already in struct size */
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਵਸਤੂ->hostname, hostname, hostlen);
  ਵਸਤੂ->mid = ਅੰਕੜੇ->mid;
  ਵਸਤੂ->resolv_id = resolv_id;
  ਵਸਤੂ->dns_queries = dns_queries;
  ਵਸਤੂ->ਪੋਰਟ = ਪੋਰਟ;
  ਵਸਤੂ->transport = transport;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  curl_msnprintf(ਵਸਤੂ->description, ਆਕਾਰ(ਵਸਤੂ->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 ਅੰਕੜੇ->id, ਵਸਤੂ->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 ਵਸਤੂ->hostname, ਵਸਤੂ->ਪੋਰਟ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  {
    ਅਚਲ ਅੱਖਰ *p = getenv("CURL_DBG_RESOLV_DELAY");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, UINT32_MAX)) {
        ਵਸਤੂ->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, UINT32_MAX)) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c = 0;
        Curl_rand_ਬਾਈਟਾਂ(ਅੰਕੜੇ, FALSE, &c, 1);
        ਵਸਤੂ->delay_fail_ms = (uint32_t)l + c;
      }
    }
    ਜੇ(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      ਵਸਤੂ->dbg_negative = TRUE;
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ ਵਸਤੂ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR_ARES

ਸਥਿਰ ਖਾਲੀ async_thrdd_rr_ਪੂਰਾ(ਖਾਲੀ *user_ਅੰਕੜੇ, ares_ਸਥਿਤੀ_t ਸਥਿਤੀ,
                                size_t timeouts,
                                ਅਚਲ ares_dns_ਰਿਕਾਰਡ_t *dnsrec)
{
  ਬਣਤਰ Curl_resolv_async *async = user_ਅੰਕੜੇ;
  ਬਣਤਰ async_thrdd_ctx *thrdd = async ? &async->thrdd : ਨਿਰਾ;

  (ਖਾਲੀ)timeouts;
  ਜੇ(!thrdd)
    ਵਾਪਸ;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->ਪੂਰਾ = !async->queries_ongoing;
  ਜੇ((ARES_SUCCESS == ਸਥਿਤੀ) && dnsrec)
    async->ਨਤੀਜਾ = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
ਸਥਿਰ CURLcode async_rr_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_resolv_async *async)
{
  ਬਣਤਰ async_thrdd_ctx *thrdd = &async->thrdd;
  ਪੂਰਨ ਸਥਿਤੀ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(!thrdd->rr.channel);
  ਸਥਿਤੀ = ares_init_options(&thrdd->rr.channel, ਨਿਰਾ, 0);
  ਜੇ(ਸਥਿਤੀ != ARES_SUCCESS) {
    thrdd->rr.channel = ਨਿਰਾ;
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(getenv("CURL_DNS_SERVER")) {
    ਅਚਲ ਅੱਖਰ *servers = getenv("CURL_DNS_SERVER");
    ਸਥਿਤੀ = ares_ਸੈੱਟ_servers_ports_csv(thrdd->rr.channel, servers);
    ਜੇ(ਸਥਿਤੀ)
      ਵਾਪਸ CURLE_FAILED_INIT;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
ਸਥਿਰ CURLcode async_rr_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_resolv_async *async)
{
  ਬਣਤਰ async_thrdd_ctx *thrdd = &async->thrdd;
  ਅੱਖਰ *https_name = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(async->peer->ਪੋਰਟ != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->ਪੋਰਟ, async->peer->hostname);
    ਜੇ(!https_name) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_ਵਿੱਚ, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_ਪੂਰਾ, async, ਨਿਰਾ);
  CURL_TRC_DNS(ਅੰਕੜੇ, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

ਬਾਹਰਲਾ:
  curlx_free(https_name);
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ Curl_async_thrdd_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(ਅੰਕੜੇ, async);
}

ਬਣਤਰ async_thrdd_ਮੇਲ_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

ਸਥਿਰ bool async_thrdd_ਮੇਲ_ਵਸਤੂ(ਖਾਲੀ *qitem, ਖਾਲੀ *ਮੇਲ_ਅੰਕੜੇ)
{
  ਅਚਲ ਬਣਤਰ async_thrdd_ਮੇਲ_ctx *ctx = ਮੇਲ_ਅੰਕੜੇ;
  ਬਣਤਰ async_thrdd_ਵਸਤੂ *ਵਸਤੂ = qitem;
  ਵਾਪਸ (ਵਸਤੂ->mid == ctx->mid) && (ਵਸਤੂ->resolv_id == ctx->resolv_id);
}

ਖਾਲੀ Curl_async_thrdd_destroy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_resolv_async *async)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(async->queries_ongoing && !async->ਪੂਰਾ &&
     ਅੰਕੜੇ->multi && ਅੰਕੜੇ->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    ਬਣਤਰ async_thrdd_ਮੇਲ_ctx mctx;
    mctx.mid = ਅੰਕੜੇ->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(ਅੰਕੜੇ->multi->resolv_thrdq,
                     async_thrdd_ਮੇਲ_ਵਸਤੂ, &mctx);
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR_ARES
  ਜੇ(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = ਨਿਰਾ;
  }
#ਜੇ_ਅੰਤ
  async_thrdd_ਵਸਤੂ_destroy(async->thrdd.res_A);
  async->thrdd.res_A = ਨਿਰਾ;
  async_thrdd_ਵਸਤੂ_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = ਨਿਰਾ;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, uint32_t resolv_id,
                          ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਬਣਤਰ Curl_resolv_async *async = Curl_async_get(ਅੰਕੜੇ, resolv_id);
  ਬਣਤਰ async_thrdd_ctx *thrdd = async ? &async->thrdd : ਨਿਰਾ;
  timediff_t milli, ms;
  CURLcode ਨਤੀਜਾ = CURLE_AGAIN;

  ਜੇ(!thrdd)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜਦੋਂ(ਨਤੀਜਾ == CURLE_AGAIN) {
    ਜਦੋਂ(async->queries_ongoing && !async->ਪੂਰਾ) {
      Curl_async_thrdd_multi_process(ਅੰਕੜੇ->multi);
      ਜੇ(async->ਪੂਰਾ)
        ਰੋਕੋ;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &async->ਸ਼ੁਰੂ);
      ਜੇ(ms < 3)
        milli = 0;
      ਨਹੀਂਤਾਂ ਜੇ(ms <= 50)
        milli = ms / 3;
      ਨਹੀਂਤਾਂ ਜੇ(ms <= 250)
        milli = 50;
      ਨਹੀਂਤਾਂ
        milli = 200;
      CURL_TRC_DNS(ਅੰਕੜੇ, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    ਨਤੀਜਾ = Curl_async_take_ਨਤੀਜਾ(ਅੰਕੜੇ, async, pdns);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
ਸਥਿਰ bool gai_negative(ਪੂਰਨ rc)
{
  ਚੋਣ(rc) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ EAI_NONAME
  ਹਾਲਤ EAI_NONAME:
#ਜੇ_ਅੰਤ
#ਜੇ defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  ਹਾਲਤ EAI_NODATA:
#ਜੇ_ਅੰਤ
    ਵਾਪਸ TRUE;
  ਮੂਲ:
    ਵਾਪਸ FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
ਸਥਿਰ ਖਾਲੀ async_thrdd_ਵਸਤੂ_process(ਖਾਲੀ *arg)
{
  ਬਣਤਰ async_thrdd_ਵਸਤੂ *ਵਸਤੂ = arg;
  ਬਣਤਰ addrinfo hints;
  ਅੱਖਰ service[12];
  ਪੂਰਨ pf = PF_INET;
  ਪੂਰਨ rc;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(ਵਸਤੂ->delay_ms) {
    curlx_wait_ms(ਵਸਤੂ->delay_ms);
  }
  ਜੇ(ਵਸਤੂ->delay_fail_ms) {
    curlx_wait_ms(ਵਸਤੂ->delay_fail_ms);
    ਵਸਤੂ->negative = ਵਸਤੂ->dbg_negative;
    ਵਾਪਸ;
  }
#ਜੇ_ਅੰਤ

  memset(&hints, 0, ਆਕਾਰ(hints));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_IPV6
  ਜੇ(ਵਸਤੂ->dns_queries & CURL_DNSQ_AAAA) {
    pf = (ਵਸਤੂ->dns_queries & CURL_DNSQ_A) ? PF_UNSPEC : PF_INET6;
  }
#ਜੇ_ਅੰਤ
  hints.ai_family = pf;
  hints.ai_socktype = Curl_socktype_for_transport(ਵਸਤੂ->transport);
  hints.ai_protocol = Curl_protocol_for_transport(ਵਸਤੂ->transport);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __APPLE__
  /* If we leave `ai_flags == 0` then macOS is looking for IPV4MAPPED
   * when doing AAAA queries. We do not want this "help". */
  hints.ai_flags = AI_ADDRCONFIG;
#ਜੇ_ਅੰਤ

  curl_msnprintf(service, ਆਕਾਰ(service), "%u", ਵਸਤੂ->ਪੋਰਟ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#ਜੇ_ਅੰਤ

  rc = Curl_getaddrinfo_ex(ਵਸਤੂ->hostname, service, &hints, &ਵਸਤੂ->res);
  ਜੇ(rc) {
    ਵਸਤੂ->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    ਜੇ(ਵਸਤੂ->sockerr == 0)
      ਵਸਤੂ->sockerr = RESOLVER_ENOMEM;
    ਵਸਤੂ->negative = gai_negative(rc);
  }
  ਨਹੀਂਤਾਂ {
    Curl_addrinfo_ਸੈੱਟ_ਪੋਰਟ(ਵਸਤੂ->res, ਵਸਤੂ->ਪੋਰਟ);
  }
}

#ਨਹੀਂਤਾਂ /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
ਸਥਿਰ ਖਾਲੀ async_thrdd_ਵਸਤੂ_process(ਖਾਲੀ *arg)
{
  ਬਣਤਰ async_thrdd_ਵਸਤੂ *ਵਸਤੂ = arg;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(ਵਸਤੂ->delay_ms) {
    curlx_wait_ms(ਵਸਤੂ->delay_ms);
  }
  ਜੇ(ਵਸਤੂ->delay_fail_ms) {
    curlx_wait_ms(ਵਸਤੂ->delay_fail_ms);
    ਵਸਤੂ->negative = ਵਸਤੂ->dbg_negative;
    ਵਾਪਸ;
  }
#ਜੇ_ਅੰਤ
  ਵਸਤੂ->res = Curl_ipv4_resolve_r(ਵਸਤੂ->hostname, ਵਸਤੂ->ਪੋਰਟ);
  ਜੇ(!ਵਸਤੂ->res) {
    ਵਸਤੂ->sockerr = SOCKERRNO;
    ਜੇ(ਵਸਤੂ->sockerr == 0)
      ਵਸਤੂ->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    ਵਸਤੂ->negative = TRUE;
  }
}

#ਜੇ_ਅੰਤ /* HAVE_GETADDRINFO */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
ਸਥਿਰ ਖਾਲੀ async_thrdd_event(ਅਚਲ ਬਣਤਰ curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_multi *multi = user_ਅੰਕੜੇ;
  (ਖਾਲੀ)tqueue;
  ਚੋਣ(ev) {
  ਹਾਲਤ CURL_THRDQ_EV_ਵਸਤੂ_ਪੂਰਾ:
    Curl_multi_wakeup_ਅੰਦਰੂਨੀ(multi);
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ async_thrdd_event   ਨਿਰਾ
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ async_thrdd_ਵਸਤੂ_free(ਖਾਲੀ *ਵਸਤੂ)
{
  async_thrdd_ਵਸਤੂ_destroy(ਵਸਤੂ);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(ਬਣਤਰ Curl_multi *multi,
                                     uint32_t min_ਧਾਗੇ,
                                     uint32_t max_ਧਾਗੇ,
                                     uint32_t idle_ਸਮਾਂ_ms)
{
  CURLcode ਨਤੀਜਾ;
  DEBUGASSERT(!multi->resolv_thrdq);
  ਨਤੀਜਾ = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_ਧਾਗੇ, max_ਧਾਗੇ, idle_ਸਮਾਂ_ms,
                             async_thrdd_ਵਸਤੂ_free,
                             async_thrdd_ਵਸਤੂ_process,
                             async_thrdd_event,
                             multi);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(!ਨਤੀਜਾ) {
    ਅਚਲ ਅੱਖਰ *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, UINT32_MAX)) {
        ਨਤੀਜਾ = Curl_async_thrdd_multi_ਸੈੱਟ_props(
          multi, min_ਧਾਗੇ, (uint32_t)l, idle_ਸਮਾਂ_ms);
      }
    }
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Tear down the thread queue, joining active threads or detaching them */
ਖਾਲੀ Curl_async_thrdd_multi_destroy(ਬਣਤਰ Curl_multi *multi, bool join)
{
  ਜੇ(multi->resolv_thrdq) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#ਜੇ_ਅੰਤ
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = ਨਿਰਾ;
  }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ ਖਾਲੀ async_thrdd_report_ਵਸਤੂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ async_thrdd_ਵਸਤੂ *ਵਸਤੂ)
{
  ਅੱਖਰ buf[MAX_IPADR_LEN];
  ਬਣਤਰ dynbuf tmp;
  ਅਚਲ ਅੱਖਰ *sep = "";
  ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai = ਵਸਤੂ->res;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ ai_family;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ai_family = (ਵਸਤੂ->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#ਨਹੀਂਤਾਂ
  ai_family = AF_INET;
#ਜੇ_ਅੰਤ

  ਜੇ(!CURL_TRC_DNS_is_verbose(ਅੰਕੜੇ))
    ਵਾਪਸ;

  curlx_dyn_init(&tmp, 1024);
  ਲਈ(; ai; ai = ai->ai_ਅਗਲਾ) {
    ਜੇ(ai->ai_family == ai_family) {
      Curl_printable_ਪਤਾ(ai, buf, ਆਕਾਰ(buf));
      ਨਤੀਜਾ = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      ਜੇ(ਨਤੀਜਾ) {
        CURL_TRC_DNS(ਅੰਕੜੇ, "too many IP, cannot show");
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(ਅੰਕੜੇ, "Host %s:%u resolved IPv%c: %s",
               ਵਸਤੂ->hostname, ਵਸਤੂ->ਪੋਰਟ,
               (ਵਸਤੂ->dns_queries & CURL_DNSQ_AAAA) ? '6' : '4',
               (curlx_dyn_len(&tmp) ? curlx_dyn_ptr(&tmp) : "(none)"));
ਬਾਹਰਲਾ:
  curlx_dyn_free(&tmp);
}
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

/* Process the receiving end of the thread queue, dispatching
 * processed items to their transfer when it can still be found
 * and has an `async` state present. Otherwise, destroy the item. */
ਖਾਲੀ Curl_async_thrdd_multi_process(ਬਣਤਰ Curl_multi *multi)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ਖਾਲੀ *qitem;

  ਜਦੋਂ(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    ਬਣਤਰ async_thrdd_ਵਸਤੂ *ਵਸਤੂ = qitem;
    ਬਣਤਰ Curl_resolv_async *async = ਨਿਰਾ;

    ਅੰਕੜੇ = Curl_multi_get_easy(multi, ਵਸਤੂ->mid);
    ਜੇ(ਅੰਕੜੇ)
      async = Curl_async_get(ਅੰਕੜੇ, ਵਸਤੂ->resolv_id);
    ਜੇ(async) {
      ਬਣਤਰ async_thrdd_ਵਸਤੂ **pdest = &async->thrdd.res_A;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_IPV6
      ਜੇ(ਵਸਤੂ->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#ਜੇ_ਅੰਤ
      ਜੇ(!*pdest) {
        *pdest = ਵਸਤੂ;
        ਵਸਤੂ = ਨਿਰਾ;
      }
      ਨਹੀਂਤਾਂ
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(ਅੰਕੜੇ);
    }
    async_thrdd_ਵਸਤੂ_free(ਵਸਤੂ);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_ਸੈੱਟ_props(ਬਣਤਰ Curl_multi *multi,
                                          uint32_t min_ਧਾਗੇ,
                                          uint32_t max_ਧਾਗੇ,
                                          uint32_t idle_ਸਮਾਂ_ms)
{
  ਵਾਪਸ Curl_thrdq_ਸੈੱਟ_props(multi->resolv_thrdq,
                              min_ਧਾਗੇ, max_ਧਾਗੇ, idle_ਸਮਾਂ_ms);
}

ਸਥਿਰ CURLcode async_thrdd_query(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  ਬਣਤਰ async_thrdd_ਵਸਤੂ *ਵਸਤੂ;
  CURLcode ਨਤੀਜਾ;

  ਵਸਤੂ = async_thrdd_ਵਸਤੂ_create(ਅੰਕੜੇ, async->id, dns_queries,
                                 async->peer->hostname, async->peer->ਪੋਰਟ,
                                 async->transport);
  ਜੇ(!ਵਸਤੂ) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  CURL_TRC_DNS(ਅੰਕੜੇ, "queueing query %s", ਵਸਤੂ->description);
  ਨਤੀਜਾ = Curl_thrdq_send(ਅੰਕੜੇ->multi->resolv_thrdq, ਵਸਤੂ,
                           async_ਵਸਤੂ_description(ਵਸਤੂ), async->timeout_ms);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਵਸਤੂ = ਨਿਰਾ;
  async->queries_ongoing++;

ਬਾਹਰਲਾ:
  ਜੇ(ਵਸਤੂ)
    async_thrdd_ਵਸਤੂ_free(ਵਸਤੂ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_async_getaddrinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_resolv_async *async)
{
  CURLcode ਨਤੀਜਾ = CURLE_FAILED_INIT;
  ਖਾਲੀ *resolver = ਨਿਰਾ;

  ਜੇ(async->queries_ongoing || async->ਪੂਰਾ)
    ਵਾਪਸ CURLE_FAILED_INIT;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  ਜੇ((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    ਨਤੀਜਾ = async_rr_init(ਅੰਕੜੇ, async);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    resolver = async->thrdd.rr.channel;
  }
#ਜੇ_ਅੰਤ

  ਨਤੀਜਾ = Curl_resolv_announce_ਸ਼ੁਰੂ(ਅੰਕੜੇ, resolver);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR_ARES
  ਜੇ(resolver) {
    ਨਤੀਜਾ = async_rr_ਸ਼ੁਰੂ(ਅੰਕੜੇ, async);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  ਜੇ((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    ਨਤੀਜਾ = async_thrdd_query(ਅੰਕੜੇ, async, CURL_DNSQ_AAAA);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ
  ਜੇ(async->dns_queries & CURL_DNSQ_A) {
    ਨਤੀਜਾ = async_thrdd_query(ਅੰਕੜੇ, async, CURL_DNSQ_A);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  CURL_THRDQ_TRACE(ਅੰਕੜੇ->multi->resolv_thrdq, ਅੰਕੜੇ);

ਬਾਹਰਲਾ:
  ਜੇ(!async->queries_ongoing)
    async->ਪੂਰਾ = TRUE;

  ਜੇ(ਨਤੀਜਾ)
    CURL_TRC_DNS(ਅੰਕੜੇ, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->ਪੋਰਟ, (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_async_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_resolv_async *async,
                            ਬਣਤਰ easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(ਅੰਕੜੇ, async);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR_ARES
  ਜੇ(async->thrdd.rr.channel) {
    CURLcode ਨਤੀਜਾ = Curl_ares_pollset(ਅੰਕੜੇ, async->thrdd.rr.channel, ps);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    timeout_ms = Curl_ares_timeout_ms(ਅੰਕੜੇ, async, async->thrdd.rr.channel);
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ps;
#ਜੇ_ਅੰਤ

  ਜੇ(!async->ਪੂਰਾ) {
    ਅਚਲ ਬਣਤਰ curltime *pnow = Curl_pgrs_now(ਅੰਕੜੇ);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->ਸ਼ੁਰੂ);
    ਜੇ(elapsed_ms < 3)
      stutter_ms = 1;
    ਨਹੀਂਤਾਂ ਜੇ(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    ਨਹੀਂਤਾਂ ਜੇ(elapsed_ms <= 250)
      stutter_ms = 50;
    ਨਹੀਂਤਾਂ
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#ਨਹੀਂਤਾਂ
    ਜੇ(async->queries_ongoing &&
       !Curl_thrdq_check_started(ਅੰਕੜੇ->multi->resolv_thrdq)) {
      /* The queue has items but starting a worker thread to process
         them just failed again; expire soon to check once more,
         instead of sleeping on the full resolve timeout. */
      CURL_TRC_DNS(ਅੰਕੜੇ, "resolver thread start failed again, retrying");
      timeout_ms = CURLMIN(100, timeout_ms);
    }
#ਜੇ_ਅੰਤ
    Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ਸਥਿਰ bool async_thrdd_ਵਸਤੂ_missing_scope(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ async_thrdd_ਵਸਤੂ *ਵਸਤੂ)
{
  ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai;

  /* scope id already globally set */
  ਜੇ(ਅੰਕੜੇ->conn && ਅੰਕੜੇ->conn->scope_id)
    ਵਾਪਸ FALSE;

  ਲਈ(ai = ਵਸਤੂ->res; ai; ai = ai->ai_ਅਗਲਾ) {
    ਜੇ(ai->ai_family == AF_INET6) {
      ਬਣਤਰ sockaddr_in6 *sa6 = (ਖਾਲੀ *)ai->ai_addr;
      ਜੇ(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        ਵਾਪਸ TRUE;
    }
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਖਾਲੀ async_thrdd_ਵਸਤੂ_strip_results(ਬਣਤਰ async_thrdd_ਵਸਤੂ *ਵਸਤੂ,
                                           ਪੂਰਨ ai_family)
{
  ਬਣਤਰ Curl_addrinfo *ai = ਵਸਤੂ->res, **panchor = &ਵਸਤੂ->res;
  ਜਦੋਂ(ai) {
    ਜੇ(ai->ai_family == ai_family) {
      *panchor = ai->ai_ਅਗਲਾ;
      ai->ai_ਅਗਲਾ = ਨਿਰਾ;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    ਨਹੀਂਤਾਂ {
      panchor = &ai->ai_ਅਗਲਾ;
      ai = ai->ai_ਅਗਲਾ;
    }
  }
}

#ਜੇ_ਅੰਤ /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

ਸਥਿਰ CURLcode async_thrdd_check_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ Curl_resolv_async *async)
{
  ਬਣਤਰ async_thrdd_ctx *thrdd = &async->thrdd;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_ਵਸਤੂ(ਅੰਕੜੇ, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = ਨਿਰਾ;
    thrdd->processed_A = TRUE;
  }

  ਜੇ(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#ਜੇ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    ਜੇ(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_ਵਸਤੂ_missing_scope(ਅੰਕੜੇ, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      ਬਣਤਰ async_thrdd_ਵਸਤੂ *ਵਸਤੂ = thrdd->res_AAAA;
      CURLcode ਨਤੀਜਾ;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = ਨਿਰਾ;
      ਵਸਤੂ->dns_queries |= CURL_DNSQ_A;
      ਜੇ(ਵਸਤੂ->res) {
        Curl_freeaddrinfo(ਵਸਤੂ->res);
        ਵਸਤੂ->res = ਨਿਰਾ;
      }

      CURL_TRC_DNS(ਅੰਕੜੇ, "re-queueing query %s for AF_UNSPEC resolve",
                   ਵਸਤੂ->description);
      ਨਤੀਜਾ = Curl_thrdq_send(ਅੰਕੜੇ->multi->resolv_thrdq, ਵਸਤੂ,
                               async_ਵਸਤੂ_description(ਵਸਤੂ),
                               async->timeout_ms);
      ਜੇ(ਨਤੀਜਾ) {
        async_thrdd_ਵਸਤੂ_free(ਵਸਤੂ);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      async->queries_ongoing++;
      ਵਾਪਸ CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_ਵਸਤੂ_strip_results(thrdd->res_AAAA, AF_INET);
#ਜੇ_ਅੰਤ /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_ਵਸਤੂ(ਅੰਕੜੇ, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = ਨਿਰਾ;
    thrdd->processed_AAAA = TRUE;
  }

  ਜੇ(async->queries_ongoing)
    ਵਾਪਸ CURLE_AGAIN;
  async->ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_ਨਤੀਜਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_resolv_async *async,
                                ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਬਣਤਰ async_thrdd_ctx *thrdd = &async->thrdd;
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  ਜੇ(thrdd->rr.channel)
    (ਖਾਲੀ)Curl_ares_perform(thrdd->rr.channel, 0);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
  Curl_async_thrdd_multi_process(ਅੰਕੜੇ->multi);
#ਜੇ_ਅੰਤ

  ਨਤੀਜਾ = async_thrdd_check_ਪੂਰਾ(ਅੰਕੜੇ, async);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  Curl_expire_clear(ਅੰਕੜੇ, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    ਅਚਲ uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    ਜੇ(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    ਜੇ(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    ਜੇ(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  ਜੇ(async->ਨਤੀਜਾ) {
    ਨਤੀਜਾ = async->ਨਤੀਜਾ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      ਅੰਕੜੇ, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    ਜੇ(!dns) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR_ARES
  ਜੇ(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(ਅੰਕੜੇ, async->httpsrr);
    dns = Curl_dnsc_mk_https(ਅੰਕੜੇ, &async->httpsrr, async->peer);
    ਜੇ(!dns) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
#ਜੇ_ਅੰਤ

  ਜੇ(dns) {
    *pdns = dns;
    dns = ਨਿਰਾ;
  }
  CURL_THRDQ_TRACE(ਅੰਕੜੇ->multi->resolv_thrdq, ਅੰਕੜੇ);

ਬਾਹਰਲਾ:
  Curl_dns_ਇੰਦਰਾਜ_unlink(ਅੰਕੜੇ, &dns);
  Curl_async_thrdd_shutdown(ਅੰਕੜੇ, async);
  ਜੇ(!ਨਤੀਜਾ && !*pdns)
    ਨਤੀਜਾ = Curl_async_failed(ਅੰਕੜੇ, async, ਨਿਰਾ);
  ਜੇ(ਨਤੀਜਾ &&
     (ਨਤੀਜਾ != CURLE_COULDNT_RESOLVE_HOST) &&
     (ਨਤੀਜਾ != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(ਅੰਕੜੇ, "Error %d resolving %s:%d",
                 (ਪੂਰਨ)ਨਤੀਜਾ, async->peer->hostname, async->peer->ਪੋਰਟ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* USE_RESOLV_THREADED */
