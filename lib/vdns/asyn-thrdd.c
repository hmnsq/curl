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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GETADDRINFO
#ವ್ಯಾಖ್ಯಾನಿಸು RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು RESOLVER_ENOMEM  SOCKENOMEM
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "thrdqueue.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curlx/wait.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
#ಅನ್ತರ್ಭಾವ <ares.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
#ವ್ಯಾಖ್ಯಾನಿಸು USE_HTTPSRR_ARES  /* the combo */
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
ಪೂರ್ಣಾಂಕ Curl_async_global_init(ಶೂನ್ಯ)
{
#ಆದರೆ defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ಆದರೆ(ares_library_init(ARES_LIB_INIT_ALL)) {
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
ಶೂನ್ಯ Curl_async_global_cleanup(ಶೂನ್ಯ)
{
#ಆದರೆ defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASYN_ಅಂಶ_DESC_LEN   64
#ವ್ಯಾಖ್ಯಾನಿಸು async_ಅಂಶ_description(x)   (x)->description
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು async_ಅಂಶ_description(x)   ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ async_thrdd_ಅಂಶ {
  ರಚನೆ Curl_addrinfo *res;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಅಕ್ಷರ description[CURL_ASYN_ಅಂಶ_DESC_LEN];
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ sockerr;
  uint32_t mid;
  uint32_t resolv_id;
  uint16_t ಪೋರ್ಟ್;
  uint8_t transport;
  uint8_t dns_queries;
  ಬಿಟ್(negative); /* resolver answered that the name does not exist */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  uint32_t delay_ms;
  uint32_t delay_fail_ms;
  ಬಿಟ್(dbg_negative);
#ಆದರೆ_ಅಂತ್ಯ
  ಅಕ್ಷರ hostname[1];
};

/* Give up reference to add_ctx */
ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_ಅಂಶ_destroy(ರಚನೆ async_thrdd_ಅಂಶ *ಅಂಶ)
{
  ಆದರೆ(ಅಂಶ) {
    ಆದರೆ(ಅಂಶ->res)
      Curl_freeaddrinfo(ಅಂಶ->res);
    curlx_free(ಅಂಶ);
  }
}

/* Initialize context for threaded resolver */
ಸ್ಥಿರ ರಚನೆ async_thrdd_ಅಂಶ *async_thrdd_ಅಂಶ_create(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  uint32_t resolv_id, uint8_t dns_queries,
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname, uint16_t ಪೋರ್ಟ್,
  uint8_t transport)
{
  size_t hostlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hostname);
  ರಚನೆ async_thrdd_ಅಂಶ *ಅಂಶ;

  ಅಂಶ = curlx_calloc(1, ಗಾತ್ರ(*ಅಂಶ) + hostlen);
  ಆದರೆ(!ಅಂಶ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಆದರೆ(hostlen) /* null-terminator of name already in struct size */
    ಸ್ಮರಣೆ_ನಕಲು(ಅಂಶ->hostname, hostname, hostlen);
  ಅಂಶ->mid = ದತ್ತಾಂಶ->mid;
  ಅಂಶ->resolv_id = resolv_id;
  ಅಂಶ->dns_queries = dns_queries;
  ಅಂಶ->ಪೋರ್ಟ್ = ಪೋರ್ಟ್;
  ಅಂಶ->transport = transport;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  curl_msnprintf(ಅಂಶ->description, ಗಾತ್ರ(ಅಂಶ->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 ದತ್ತಾಂಶ->id, ಅಂಶ->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 ಅಂಶ->hostname, ಅಂಶ->ಪೋರ್ಟ್);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = getenv("CURL_DBG_RESOLV_DELAY");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, UINT32_MAX)) {
        ಅಂಶ->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, UINT32_MAX)) {
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c = 0;
        Curl_rand_ಬೈಟುಗಳು(ದತ್ತಾಂಶ, FALSE, &c, 1);
        ಅಂಶ->delay_fail_ms = (uint32_t)l + c;
      }
    }
    ಆದರೆ(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      ಅಂಶ->dbg_negative = TRUE;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ ಅಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR_ARES

ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_rr_ಮುಗಿದ(ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ares_ಸ್ಥಿತಿ_t ಸ್ಥಿತಿ_2,
                                size_t timeouts,
                                ಸ್ಥಿರಾಂಕ ares_dns_ದಾಖಲೆ_t *dnsrec)
{
  ರಚನೆ Curl_resolv_async *async = user_ದತ್ತಾಂಶ;
  ರಚನೆ async_thrdd_ctx *thrdd = async ? &async->thrdd : ಖಾಲಿ;

  (ಶೂನ್ಯ)timeouts;
  ಆದರೆ(!thrdd)
    ಹಿಂತಿರುಗಿ;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->ಮುಗಿದ = !async->queries_ongoing;
  ಆದರೆ((ARES_SUCCESS == ಸ್ಥಿತಿ_2) && dnsrec)
    async->ಫಲಿತಾಂಶ = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
ಸ್ಥಿರ CURLcode async_rr_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_resolv_async *async)
{
  ರಚನೆ async_thrdd_ctx *thrdd = &async->thrdd;
  ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(!thrdd->rr.channel);
  ಸ್ಥಿತಿ_2 = ares_init_options(&thrdd->rr.channel, ಖಾಲಿ, 0);
  ಆದರೆ(ಸ್ಥಿತಿ_2 != ARES_SUCCESS) {
    thrdd->rr.channel = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(getenv("CURL_DNS_SERVER")) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *servers = getenv("CURL_DNS_SERVER");
    ಸ್ಥಿತಿ_2 = ares_ಗಣ_servers_ports_csv(thrdd->rr.channel, servers);
    ಆದರೆ(ಸ್ಥಿತಿ_2)
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
ಸ್ಥಿರ CURLcode async_rr_ಆರಂಭ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_resolv_async *async)
{
  ರಚನೆ async_thrdd_ctx *thrdd = &async->thrdd;
  ಅಕ್ಷರ *https_name = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(async->peer->ಪೋರ್ಟ್ != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->ಪೋರ್ಟ್, async->peer->hostname);
    ಆದರೆ(!https_name) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_ಒಳ, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_ಮುಗಿದ, async, ಖಾಲಿ);
  CURL_TRC_DNS(ದತ್ತಾಂಶ, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

ಹೊರಹೋಗು:
  curlx_free(https_name);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ Curl_async_thrdd_shutdown(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(ದತ್ತಾಂಶ, async);
}

ರಚನೆ async_thrdd_ಹೊಂದಿಕೆ_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

ಸ್ಥಿರ bool async_thrdd_ಹೊಂದಿಕೆ_ಅಂಶ(ಶೂನ್ಯ *qitem, ಶೂನ್ಯ *ಹೊಂದಿಕೆ_ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ async_thrdd_ಹೊಂದಿಕೆ_ctx *ctx = ಹೊಂದಿಕೆ_ದತ್ತಾಂಶ;
  ರಚನೆ async_thrdd_ಅಂಶ *ಅಂಶ = qitem;
  ಹಿಂತಿರುಗಿ (ಅಂಶ->mid == ctx->mid) && (ಅಂಶ->resolv_id == ctx->resolv_id);
}

ಶೂನ್ಯ Curl_async_thrdd_destroy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_resolv_async *async)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(async->queries_ongoing && !async->ಮುಗಿದ &&
     ದತ್ತಾಂಶ->multi && ದತ್ತಾಂಶ->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    ರಚನೆ async_thrdd_ಹೊಂದಿಕೆ_ctx mctx;
    mctx.mid = ದತ್ತಾಂಶ->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(ದತ್ತಾಂಶ->multi->resolv_thrdq,
                     async_thrdd_ಹೊಂದಿಕೆ_ಅಂಶ, &mctx);
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR_ARES
  ಆದರೆ(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = ಖಾಲಿ;
  }
#ಆದರೆ_ಅಂತ್ಯ
  async_thrdd_ಅಂಶ_destroy(async->thrdd.res_A);
  async->thrdd.res_A = ಖಾಲಿ;
  async_thrdd_ಅಂಶ_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = ಖಾಲಿ;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, uint32_t resolv_id,
                          ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ರಚನೆ Curl_resolv_async *async = Curl_async_get(ದತ್ತಾಂಶ, resolv_id);
  ರಚನೆ async_thrdd_ctx *thrdd = async ? &async->thrdd : ಖಾಲಿ;
  timediff_t milli, ms;
  CURLcode ಫಲಿತಾಂಶ = CURLE_AGAIN;

  ಆದರೆ(!thrdd)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆಗ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    ಆಗ(async->queries_ongoing && !async->ಮುಗಿದ) {
      Curl_async_thrdd_multi_process(ದತ್ತಾಂಶ->multi);
      ಆದರೆ(async->ಮುಗಿದ)
        ನಿಲ್ಲಿಸು;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &async->ಆರಂಭ);
      ಆದರೆ(ms < 3)
        milli = 0;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ms <= 50)
        milli = ms / 3;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ms <= 250)
        milli = 50;
      ಇಲ್ಲದಿದ್ದರೆ
        milli = 200;
      CURL_TRC_DNS(ದತ್ತಾಂಶ, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    ಫಲಿತಾಂಶ = Curl_async_take_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ, async, pdns);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
ಸ್ಥಿರ bool gai_negative(ಪೂರ್ಣಾಂಕ rc)
{
  ಆಯ್ಕೆ(rc) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ EAI_NONAME
  ಸ್ಥಿತಿ EAI_NONAME:
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  ಸ್ಥಿತಿ EAI_NODATA:
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ TRUE;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_ಅಂಶ_process(ಶೂನ್ಯ *arg)
{
  ರಚನೆ async_thrdd_ಅಂಶ *ಅಂಶ = arg;
  ರಚನೆ addrinfo hints;
  ಅಕ್ಷರ service[12];
  ಪೂರ್ಣಾಂಕ pf = PF_INET;
  ಪೂರ್ಣಾಂಕ rc;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(ಅಂಶ->delay_ms) {
    curlx_wait_ms(ಅಂಶ->delay_ms);
  }
  ಆದರೆ(ಅಂಶ->delay_fail_ms) {
    curlx_wait_ms(ಅಂಶ->delay_fail_ms);
    ಅಂಶ->negative = ಅಂಶ->dbg_negative;
    ಹಿಂತಿರುಗಿ;
  }
#ಆದರೆ_ಅಂತ್ಯ

  memset(&hints, 0, ಗಾತ್ರ(hints));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_IPV6
  ಆದರೆ(ಅಂಶ->dns_queries & CURL_DNSQ_AAAA) {
    pf = (ಅಂಶ->dns_queries & CURL_DNSQ_A) ? PF_UNSPEC : PF_INET6;
  }
#ಆದರೆ_ಅಂತ್ಯ
  hints.ai_family = pf;
  hints.ai_socktype = Curl_socktype_for_transport(ಅಂಶ->transport);
  hints.ai_protocol = Curl_protocol_for_transport(ಅಂಶ->transport);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __APPLE__
  /* If we leave `ai_flags == 0` then macOS is looking for IPV4MAPPED
   * when doing AAAA queries. We do not want this "help". */
  hints.ai_flags = AI_ADDRCONFIG;
#ಆದರೆ_ಅಂತ್ಯ

  curl_msnprintf(service, ಗಾತ್ರ(service), "%u", ಅಂಶ->ಪೋರ್ಟ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#ಆದರೆ_ಅಂತ್ಯ

  rc = Curl_getaddrinfo_ex(ಅಂಶ->hostname, service, &hints, &ಅಂಶ->res);
  ಆದರೆ(rc) {
    ಅಂಶ->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    ಆದರೆ(ಅಂಶ->sockerr == 0)
      ಅಂಶ->sockerr = RESOLVER_ENOMEM;
    ಅಂಶ->negative = gai_negative(rc);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    Curl_addrinfo_ಗಣ_ಪೋರ್ಟ್(ಅಂಶ->res, ಅಂಶ->ಪೋರ್ಟ್);
  }
}

#ಇಲ್ಲದಿದ್ದರೆ /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_ಅಂಶ_process(ಶೂನ್ಯ *arg)
{
  ರಚನೆ async_thrdd_ಅಂಶ *ಅಂಶ = arg;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(ಅಂಶ->delay_ms) {
    curlx_wait_ms(ಅಂಶ->delay_ms);
  }
  ಆದರೆ(ಅಂಶ->delay_fail_ms) {
    curlx_wait_ms(ಅಂಶ->delay_fail_ms);
    ಅಂಶ->negative = ಅಂಶ->dbg_negative;
    ಹಿಂತಿರುಗಿ;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಅಂಶ->res = Curl_ipv4_resolve_r(ಅಂಶ->hostname, ಅಂಶ->ಪೋರ್ಟ್);
  ಆದರೆ(!ಅಂಶ->res) {
    ಅಂಶ->sockerr = SOCKERRNO;
    ಆದರೆ(ಅಂಶ->sockerr == 0)
      ಅಂಶ->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    ಅಂಶ->negative = TRUE;
  }
}

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GETADDRINFO */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_event(ಸ್ಥಿರಾಂಕ ರಚನೆ curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_multi *multi = user_ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)tqueue;
  ಆಯ್ಕೆ(ev) {
  ಸ್ಥಿತಿ CURL_THRDQ_EV_ಅಂಶ_ಮುಗಿದ:
    Curl_multi_wakeup_ಆಂತರಿಕ(multi);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು async_thrdd_event   ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_ಅಂಶ_free(ಶೂನ್ಯ *ಅಂಶ)
{
  async_thrdd_ಅಂಶ_destroy(ಅಂಶ);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(ರಚನೆ Curl_multi *multi,
                                     uint32_t min_ಎಳೆಗಳು,
                                     uint32_t max_ಎಳೆಗಳು,
                                     uint32_t idle_ಸಮಯ_ms)
{
  CURLcode ಫಲಿತಾಂಶ;
  DEBUGASSERT(!multi->resolv_thrdq);
  ಫಲಿತಾಂಶ = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_ಎಳೆಗಳು, max_ಎಳೆಗಳು, idle_ಸಮಯ_ms,
                             async_thrdd_ಅಂಶ_free,
                             async_thrdd_ಅಂಶ_process,
                             async_thrdd_event,
                             multi);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, UINT32_MAX)) {
        ಫಲಿತಾಂಶ = Curl_async_thrdd_multi_ಗಣ_props(
          multi, min_ಎಳೆಗಳು, (uint32_t)l, idle_ಸಮಯ_ms);
      }
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Tear down the thread queue, joining active threads or detaching them */
ಶೂನ್ಯ Curl_async_thrdd_multi_destroy(ರಚನೆ Curl_multi *multi, bool join)
{
  ಆದರೆ(multi->resolv_thrdq) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#ಆದರೆ_ಅಂತ್ಯ
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = ಖಾಲಿ;
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_report_ಅಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ async_thrdd_ಅಂಶ *ಅಂಶ)
{
  ಅಕ್ಷರ buf[MAX_IPADR_LEN];
  ರಚನೆ dynbuf tmp;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = "";
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai = ಅಂಶ->res;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ ai_family;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ai_family = (ಅಂಶ->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#ಇಲ್ಲದಿದ್ದರೆ
  ai_family = AF_INET;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!CURL_TRC_DNS_is_verbose(ದತ್ತಾಂಶ))
    ಹಿಂತಿರುಗಿ;

  curlx_dyn_init(&tmp, 1024);
  ಗಾಗಿ(; ai; ai = ai->ai_ಮುಂದಿನ) {
    ಆದರೆ(ai->ai_family == ai_family) {
      Curl_printable_ವಿಳಾಸ(ai, buf, ಗಾತ್ರ(buf));
      ಫಲಿತಾಂಶ = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        CURL_TRC_DNS(ದತ್ತಾಂಶ, "too many IP, cannot show");
        ಹೋಗು ಹೊರಹೋಗು;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(ದತ್ತಾಂಶ, "Host %s:%u resolved IPv%c: %s",
               ಅಂಶ->hostname, ಅಂಶ->ಪೋರ್ಟ್,
               (ಅಂಶ->dns_queries & CURL_DNSQ_AAAA) ? '6' : '4',
               (curlx_dyn_len(&tmp) ? curlx_dyn_ptr(&tmp) : "(none)"));
ಹೊರಹೋಗು:
  curlx_dyn_free(&tmp);
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

/* Process the receiving end of the thread queue, dispatching
 * processed items to their transfer when it can still be found
 * and has an `async` state present. Otherwise, destroy the item. */
ಶೂನ್ಯ Curl_async_thrdd_multi_process(ರಚನೆ Curl_multi *multi)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ಶೂನ್ಯ *qitem;

  ಆಗ(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    ರಚನೆ async_thrdd_ಅಂಶ *ಅಂಶ = qitem;
    ರಚನೆ Curl_resolv_async *async = ಖಾಲಿ;

    ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, ಅಂಶ->mid);
    ಆದರೆ(ದತ್ತಾಂಶ)
      async = Curl_async_get(ದತ್ತಾಂಶ, ಅಂಶ->resolv_id);
    ಆದರೆ(async) {
      ರಚನೆ async_thrdd_ಅಂಶ **pdest = &async->thrdd.res_A;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_IPV6
      ಆದರೆ(ಅಂಶ->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ(!*pdest) {
        *pdest = ಅಂಶ;
        ಅಂಶ = ಖಾಲಿ;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    }
    async_thrdd_ಅಂಶ_free(ಅಂಶ);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_ಗಣ_props(ರಚನೆ Curl_multi *multi,
                                          uint32_t min_ಎಳೆಗಳು,
                                          uint32_t max_ಎಳೆಗಳು,
                                          uint32_t idle_ಸಮಯ_ms)
{
  ಹಿಂತಿರುಗಿ Curl_thrdq_ಗಣ_props(multi->resolv_thrdq,
                              min_ಎಳೆಗಳು, max_ಎಳೆಗಳು, idle_ಸಮಯ_ms);
}

ಸ್ಥಿರ CURLcode async_thrdd_query(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  ರಚನೆ async_thrdd_ಅಂಶ *ಅಂಶ;
  CURLcode ಫಲಿತಾಂಶ;

  ಅಂಶ = async_thrdd_ಅಂಶ_create(ದತ್ತಾಂಶ, async->id, dns_queries,
                                 async->peer->hostname, async->peer->ಪೋರ್ಟ್,
                                 async->transport);
  ಆದರೆ(!ಅಂಶ) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  CURL_TRC_DNS(ದತ್ತಾಂಶ, "queueing query %s", ಅಂಶ->description);
  ಫಲಿತಾಂಶ = Curl_thrdq_send(ದತ್ತಾಂಶ->multi->resolv_thrdq, ಅಂಶ,
                           async_ಅಂಶ_description(ಅಂಶ), async->timeout_ms);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಅಂಶ = ಖಾಲಿ;
  async->queries_ongoing++;

ಹೊರಹೋಗು:
  ಆದರೆ(ಅಂಶ)
    async_thrdd_ಅಂಶ_free(ಅಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_async_getaddrinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_resolv_async *async)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
  ಶೂನ್ಯ *resolver = ಖಾಲಿ;

  ಆದರೆ(async->queries_ongoing || async->ಮುಗಿದ)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  ಆದರೆ((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    ಫಲಿತಾಂಶ = async_rr_init(ದತ್ತಾಂಶ, async);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    resolver = async->thrdd.rr.channel;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಫಲಿತಾಂಶ = Curl_resolv_announce_ಆರಂಭ(ದತ್ತಾಂಶ, resolver);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR_ARES
  ಆದರೆ(resolver) {
    ಫಲಿತಾಂಶ = async_rr_ಆರಂಭ(ದತ್ತಾಂಶ, async);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  ಆದರೆ((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    ಫಲಿತಾಂಶ = async_thrdd_query(ದತ್ತಾಂಶ, async, CURL_DNSQ_AAAA);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(async->dns_queries & CURL_DNSQ_A) {
    ಫಲಿತಾಂಶ = async_thrdd_query(ದತ್ತಾಂಶ, async, CURL_DNSQ_A);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  CURL_THRDQ_TRACE(ದತ್ತಾಂಶ->multi->resolv_thrdq, ದತ್ತಾಂಶ);

ಹೊರಹೋಗು:
  ಆದರೆ(!async->queries_ongoing)
    async->ಮುಗಿದ = TRUE;

  ಆದರೆ(ಫಲಿತಾಂಶ)
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->ಪೋರ್ಟ್, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_async_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_resolv_async *async,
                            ರಚನೆ easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(ದತ್ತಾಂಶ, async);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR_ARES
  ಆದರೆ(async->thrdd.rr.channel) {
    CURLcode ಫಲಿತಾಂಶ = Curl_ares_pollset(ದತ್ತಾಂಶ, async->thrdd.rr.channel, ps);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    timeout_ms = Curl_ares_timeout_ms(ದತ್ತಾಂಶ, async, async->thrdd.rr.channel);
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ps;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!async->ಮುಗಿದ) {
    ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->ಆರಂಭ);
    ಆದರೆ(elapsed_ms < 3)
      stutter_ms = 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(elapsed_ms <= 250)
      stutter_ms = 50;
    ಇಲ್ಲದಿದ್ದರೆ
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ(async->queries_ongoing &&
       !Curl_thrdq_check_started(ದತ್ತಾಂಶ->multi->resolv_thrdq)) {
      /* The queue has items but starting a worker thread to process
         them just failed again; expire soon to check once more,
         instead of sleeping on the full resolve timeout. */
      CURL_TRC_DNS(ದತ್ತಾಂಶ, "resolver thread start failed again, retrying");
      timeout_ms = CURLMIN(100, timeout_ms);
    }
#ಆದರೆ_ಅಂತ್ಯ
    Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ಸ್ಥಿರ bool async_thrdd_ಅಂಶ_missing_scope(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ async_thrdd_ಅಂಶ *ಅಂಶ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai;

  /* scope id already globally set */
  ಆದರೆ(ದತ್ತಾಂಶ->conn && ದತ್ತಾಂಶ->conn->scope_id)
    ಹಿಂತಿರುಗಿ FALSE;

  ಗಾಗಿ(ai = ಅಂಶ->res; ai; ai = ai->ai_ಮುಂದಿನ) {
    ಆದರೆ(ai->ai_family == AF_INET6) {
      ರಚನೆ sockaddr_in6 *sa6 = (ಶೂನ್ಯ *)ai->ai_addr;
      ಆದರೆ(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_ಅಂಶ_strip_results(ರಚನೆ async_thrdd_ಅಂಶ *ಅಂಶ,
                                           ಪೂರ್ಣಾಂಕ ai_family)
{
  ರಚನೆ Curl_addrinfo *ai = ಅಂಶ->res, **panchor = &ಅಂಶ->res;
  ಆಗ(ai) {
    ಆದರೆ(ai->ai_family == ai_family) {
      *panchor = ai->ai_ಮುಂದಿನ;
      ai->ai_ಮುಂದಿನ = ಖಾಲಿ;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      panchor = &ai->ai_ಮುಂದಿನ;
      ai = ai->ai_ಮುಂದಿನ;
    }
  }
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

ಸ್ಥಿರ CURLcode async_thrdd_check_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ Curl_resolv_async *async)
{
  ರಚನೆ async_thrdd_ctx *thrdd = &async->thrdd;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_ಅಂಶ(ದತ್ತಾಂಶ, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = ಖಾಲಿ;
    thrdd->processed_A = TRUE;
  }

  ಆದರೆ(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#ಆದರೆ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    ಆದರೆ(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_ಅಂಶ_missing_scope(ದತ್ತಾಂಶ, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      ರಚನೆ async_thrdd_ಅಂಶ *ಅಂಶ = thrdd->res_AAAA;
      CURLcode ಫಲಿತಾಂಶ;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = ಖಾಲಿ;
      ಅಂಶ->dns_queries |= CURL_DNSQ_A;
      ಆದರೆ(ಅಂಶ->res) {
        Curl_freeaddrinfo(ಅಂಶ->res);
        ಅಂಶ->res = ಖಾಲಿ;
      }

      CURL_TRC_DNS(ದತ್ತಾಂಶ, "re-queueing query %s for AF_UNSPEC resolve",
                   ಅಂಶ->description);
      ಫಲಿತಾಂಶ = Curl_thrdq_send(ದತ್ತಾಂಶ->multi->resolv_thrdq, ಅಂಶ,
                               async_ಅಂಶ_description(ಅಂಶ),
                               async->timeout_ms);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        async_thrdd_ಅಂಶ_free(ಅಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      async->queries_ongoing++;
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_ಅಂಶ_strip_results(thrdd->res_AAAA, AF_INET);
#ಆದರೆ_ಅಂತ್ಯ /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_ಅಂಶ(ದತ್ತಾಂಶ, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = ಖಾಲಿ;
    thrdd->processed_AAAA = TRUE;
  }

  ಆದರೆ(async->queries_ongoing)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  async->ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_ಫಲಿತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_resolv_async *async,
                                ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ರಚನೆ async_thrdd_ctx *thrdd = &async->thrdd;
  ರಚನೆ Curl_dns_ನಮೂದು *dns = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  ಆದರೆ(thrdd->rr.channel)
    (ಶೂನ್ಯ)Curl_ares_perform(thrdd->rr.channel, 0);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
  Curl_async_thrdd_multi_process(ದತ್ತಾಂಶ->multi);
#ಆದರೆ_ಅಂತ್ಯ

  ಫಲಿತಾಂಶ = async_thrdd_check_ಮುಗಿದ(ದತ್ತಾಂಶ, async);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  Curl_expire_clear(ದತ್ತಾಂಶ, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    ಸ್ಥಿರಾಂಕ uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    ಆದರೆ(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    ಆದರೆ(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    ಆದರೆ(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  ಆದರೆ(async->ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = async->ಫಲಿತಾಂಶ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      ದತ್ತಾಂಶ, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    ಆದರೆ(!dns) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR_ARES
  ಆದರೆ(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(ದತ್ತಾಂಶ, async->httpsrr);
    dns = Curl_dnsc_mk_https(ದತ್ತಾಂಶ, &async->httpsrr, async->peer);
    ಆದರೆ(!dns) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(dns) {
    *pdns = dns;
    dns = ಖಾಲಿ;
  }
  CURL_THRDQ_TRACE(ದತ್ತಾಂಶ->multi->resolv_thrdq, ದತ್ತಾಂಶ);

ಹೊರಹೋಗು:
  Curl_dns_ನಮೂದು_unlink(ದತ್ತಾಂಶ, &dns);
  Curl_async_thrdd_shutdown(ದತ್ತಾಂಶ, async);
  ಆದರೆ(!ಫಲಿತಾಂಶ && !*pdns)
    ಫಲಿತಾಂಶ = Curl_async_failed(ದತ್ತಾಂಶ, async, ಖಾಲಿ);
  ಆದರೆ(ಫಲಿತಾಂಶ &&
     (ಫಲಿತಾಂಶ != CURLE_COULDNT_RESOLVE_HOST) &&
     (ಫಲಿತಾಂಶ != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "Error %d resolving %s:%d",
                 (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, async->peer->hostname, async->peer->ಪೋರ್ಟ್);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_RESOLV_THREADED */
