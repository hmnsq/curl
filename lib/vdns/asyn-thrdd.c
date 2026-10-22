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
#ifdef USE_RESOLV_THREADED

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ifdef HAVE_GETADDRINFO
#define RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#ಅನ್ಯಥಾ
#define RESOLVER_ENOMEM  SOCKENOMEM
#endif

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

#ifdef USE_ARES
#ಅನ್ತರ್ಭಾವ <ares.h>
#ifdef USE_HTTPSRR
#define USE_HTTPSRR_ARES  /* the combo */
#endif
#endif


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
ಪೂರ್ಣಾಙ್ಕ Curl_async_global_init(ಶೂನ್ಯ)
{
#ಯದಿ defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ಯದಿ(ares_library_init(ARES_LIB_INIT_ALL)) {
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
ಶೂನ್ಯ Curl_async_global_cleanup(ಶೂನ್ಯ)
{
#ಯದಿ defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#endif
}

#ifdef CURLVERBOSE
#define CURL_ASYN_ITEM_DESC_LEN   64
#define async_item_description(x)   (x)->description
#ಅನ್ಯಥಾ
#define async_item_description(x)   ರಿಕ್ತ
#endif

ಸಂರಚನಾ async_thrdd_item {
  ಸಂರಚನಾ Curl_addrinfo *res;
#ifdef CURLVERBOSE
  ವರ್ಣ description[CURL_ASYN_ITEM_DESC_LEN];
#endif
  ಪೂರ್ಣಾಙ್ಕ sockerr;
  uint32_t mid;
  uint32_t resolv_id;
  uint16_t port;
  uint8_t transport;
  uint8_t dns_queries;
  BIT(negative); /* resolver answered that the name does not exist */
#ifdef DEBUGBUILD
  uint32_t delay_ms;
  uint32_t delay_fail_ms;
  BIT(dbg_negative);
#endif
  ವರ್ಣ hostname[1];
};

/* Give up reference to add_ctx */
ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_item_destroy(ಸಂರಚನಾ async_thrdd_item *item)
{
  ಯದಿ(item) {
    ಯದಿ(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
ಸ್ಥಿರ ಸಂರಚನಾ async_thrdd_item *async_thrdd_item_create(
  ಸಂರಚನಾ Curl_easy *data,
  uint32_t resolv_id, uint8_t dns_queries,
  ಧ್ರುವ ವರ್ಣ *hostname, uint16_t port,
  uint8_t transport)
{
  size_t hostlen = strlen(hostname);
  ಸಂರಚನಾ async_thrdd_item *item;

  item = curlx_calloc(1, ಪರಿಮಾಣ(*item) + hostlen);
  ಯದಿ(!item)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  ಯದಿ(hostlen) /* null-terminator of name already in struct size */
    memcpy(item->hostname, hostname, hostlen);
  item->mid = data->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#ifdef CURLVERBOSE
  curl_msnprintf(item->description, ಪರಿಮಾಣ(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 data->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#endif

#ifdef DEBUGBUILD
  {
    ಧ್ರುವ ವರ್ಣ *p = getenv("CURL_DBG_RESOLV_DELAY");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, UINT32_MAX)) {
        unsigned ವರ್ಣ c = 0;
        Curl_rand_bytes(data, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    ಯದಿ(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#endif

  ಪ್ರತ್ಯಾಗಮ item;
}

#ifdef USE_HTTPSRR_ARES

ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_rr_done(ಶೂನ್ಯ *user_data, ares_status_t status,
                                size_t timeouts,
                                ಧ್ರುವ ares_dns_record_t *dnsrec)
{
  ಸಂರಚನಾ Curl_resolv_async *async = user_data;
  ಸಂರಚನಾ async_thrdd_ctx *thrdd = async ? &async->thrdd : ರಿಕ್ತ;

  (ಶೂನ್ಯ)timeouts;
  ಯದಿ(!thrdd)
    ಪ್ರತ್ಯಾಗಮ;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ಯದಿ((ARES_SUCCESS == status) && dnsrec)
    async->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
ಸ್ಥಿರ CURLcode async_rr_init(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಸಂರಚನಾ async_thrdd_ctx *thrdd = &async->thrdd;
  ಪೂರ್ಣಾಙ್ಕ status;

  (ಶೂನ್ಯ)data;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, ರಿಕ್ತ, 0);
  ಯದಿ(status != ARES_SUCCESS) {
    thrdd->rr.channel = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
#ifdef DEBUGBUILD
  ಯದಿ(getenv("CURL_DNS_SERVER")) {
    ಧ್ರುವ ವರ್ಣ *servers = getenv("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    ಯದಿ(status)
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
ಸ್ಥಿರ CURLcode async_rr_start(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಸಂರಚನಾ async_thrdd_ctx *thrdd = &async->thrdd;
  ವರ್ಣ *https_name = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;

  ಯದಿ(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    ಯದಿ(!https_name) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, ರಿಕ್ತ);
  CURL_TRC_DNS(data, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

ಶೂನ್ಯ Curl_async_thrdd_shutdown(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(data, async);
}

ಸಂರಚನಾ async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

ಸ್ಥಿರ bool async_thrdd_match_item(ಶೂನ್ಯ *qitem, ಶೂನ್ಯ *match_data)
{
  ಧ್ರುವ ಸಂರಚನಾ async_thrdd_match_ctx *ctx = match_data;
  ಸಂರಚನಾ async_thrdd_item *item = qitem;
  ಪ್ರತ್ಯಾಗಮ (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

ಶೂನ್ಯ Curl_async_thrdd_destroy(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_resolv_async *async)
{
  (ಶೂನ್ಯ)data;
  ಯದಿ(async->queries_ongoing && !async->done &&
     data->multi && data->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    ಸಂರಚನಾ async_thrdd_match_ctx mctx;
    mctx.mid = data->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(data->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#ifdef USE_HTTPSRR_ARES
  ಯದಿ(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = ರಿಕ್ತ;
  }
#endif
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = ರಿಕ್ತ;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = ರಿಕ್ತ;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(ಸಂರಚನಾ Curl_easy *data, uint32_t resolv_id,
                          ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಸಂರಚನಾ Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ಸಂರಚನಾ async_thrdd_ctx *thrdd = async ? &async->thrdd : ರಿಕ್ತ;
  timediff_t milli, ms;
  CURLcode result = CURLE_AGAIN;

  ಯದಿ(!thrdd)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯಾವತ್(result == CURLE_AGAIN) {
    ಯಾವತ್(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(data->multi);
      ಯದಿ(async->done)
        ಭಙ್ಗ;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
      ಯದಿ(ms < 3)
        milli = 0;
      ಅನ್ಯಥಾ ಯದಿ(ms <= 50)
        milli = ms / 3;
      ಅನ್ಯಥಾ ಯದಿ(ms <= 250)
        milli = 50;
      ಅನ್ಯಥಾ
        milli = 200;
      CURL_TRC_DNS(data, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    result = Curl_async_take_result(data, async, pdns);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
ಸ್ಥಿರ bool gai_negative(ಪೂರ್ಣಾಙ್ಕ rc)
{
  ವಿಕಲ್ಪ(rc) {
#ifdef EAI_NONAME
  ಪಕ್ಷ EAI_NONAME:
#endif
#ಯದಿ defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  ಪಕ್ಷ EAI_NODATA:
#endif
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_item_process(ಶೂನ್ಯ *arg)
{
  ಸಂರಚನಾ async_thrdd_item *item = arg;
  ಸಂರಚನಾ addrinfo hints;
  ವರ್ಣ service[12];
  ಪೂರ್ಣಾಙ್ಕ pf = PF_INET;
  ಪೂರ್ಣಾಙ್ಕ rc;

#ifdef DEBUGBUILD
  ಯದಿ(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  ಯದಿ(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    ಪ್ರತ್ಯಾಗಮ;
  }
#endif

  memset(&hints, 0, ಪರಿಮಾಣ(hints));
#ifdef CURLRES_IPV6
  ಯದಿ(item->dns_queries & CURL_DNSQ_AAAA) {
    pf = (item->dns_queries & CURL_DNSQ_A) ? PF_UNSPEC : PF_INET6;
  }
#endif
  hints.ai_family = pf;
  hints.ai_socktype = Curl_socktype_for_transport(item->transport);
  hints.ai_protocol = Curl_protocol_for_transport(item->transport);
#ifdef __APPLE__
  /* If we leave `ai_flags == 0` then macOS is looking for IPV4MAPPED
   * when doing AAAA queries. We do not want this "help". */
  hints.ai_flags = AI_ADDRCONFIG;
#endif

  curl_msnprintf(service, ಪರಿಮಾಣ(service), "%u", item->port);
#ifdef AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#endif

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  ಯದಿ(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    ಯದಿ(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  ಅನ್ಯಥಾ {
    Curl_addrinfo_set_port(item->res, item->port);
  }
}

#ಅನ್ಯಥಾ /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_item_process(ಶೂನ್ಯ *arg)
{
  ಸಂರಚನಾ async_thrdd_item *item = arg;

#ifdef DEBUGBUILD
  ಯದಿ(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  ಯದಿ(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    ಪ್ರತ್ಯಾಗಮ;
  }
#endif
  item->res = Curl_ipv4_resolve_r(item->hostname, item->port);
  ಯದಿ(!item->res) {
    item->sockerr = SOCKERRNO;
    ಯದಿ(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#endif /* HAVE_GETADDRINFO */

#ifdef ENABLE_INTERNAL_WAKEUP
ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_event(ಧ್ರುವ ಸಂರಚನಾ curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ Curl_multi *multi = user_data;
  (ಶೂನ್ಯ)tqueue;
  ವಿಕಲ್ಪ(ev) {
  ಪಕ್ಷ CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
}
#ಅನ್ಯಥಾ
#define async_thrdd_event   ರಿಕ್ತ
#endif

ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_item_free(ಶೂನ್ಯ *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(ಸಂರಚನಾ Curl_multi *multi,
                                     uint32_t min_threads,
                                     uint32_t max_threads,
                                     uint32_t idle_time_ms)
{
  CURLcode result;
  DEBUGASSERT(!multi->resolv_thrdq);
  result = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_threads, max_threads, idle_time_ms,
                             async_thrdd_item_free,
                             async_thrdd_item_process,
                             async_thrdd_event,
                             multi);
#ifdef DEBUGBUILD
  ಯದಿ(!result) {
    ಧ್ರುವ ವರ್ಣ *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, UINT32_MAX)) {
        result = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#endif
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Tear down the thread queue, joining active threads or detaching them */
ಶೂನ್ಯ Curl_async_thrdd_multi_destroy(ಸಂರಚನಾ Curl_multi *multi, bool join)
{
  ಯದಿ(multi->resolv_thrdq) {
#ifdef CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#endif
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = ರಿಕ್ತ;
  }
}

#ifdef CURLVERBOSE
ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_report_item(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ async_thrdd_item *item)
{
  ವರ್ಣ buf[MAX_IPADR_LEN];
  ಸಂರಚನಾ dynbuf tmp;
  ಧ್ರುವ ವರ್ಣ *sep = "";
  ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai = item->res;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ ai_family;
#ifdef USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#ಅನ್ಯಥಾ
  ai_family = AF_INET;
#endif

  ಯದಿ(!CURL_TRC_DNS_is_verbose(data))
    ಪ್ರತ್ಯಾಗಮ;

  curlx_dyn_init(&tmp, 1024);
  ಪ್ರತಿ(; ai; ai = ai->ai_next) {
    ಯದಿ(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, ಪರಿಮಾಣ(buf));
      result = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      ಯದಿ(result) {
        CURL_TRC_DNS(data, "too many IP, cannot show");
        ಗಚ್ಛ out;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(data, "Host %s:%u resolved IPv%c: %s",
               item->hostname, item->port,
               (item->dns_queries & CURL_DNSQ_AAAA) ? '6' : '4',
               (curlx_dyn_len(&tmp) ? curlx_dyn_ptr(&tmp) : "(none)"));
out:
  curlx_dyn_free(&tmp);
}
#endif /* CURLVERBOSE */

/* Process the receiving end of the thread queue, dispatching
 * processed items to their transfer when it can still be found
 * and has an `async` state present. Otherwise, destroy the item. */
ಶೂನ್ಯ Curl_async_thrdd_multi_process(ಸಂರಚನಾ Curl_multi *multi)
{
  ಸಂರಚನಾ Curl_easy *data;
  ಶೂನ್ಯ *qitem;

  ಯಾವತ್(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    ಸಂರಚನಾ async_thrdd_item *item = qitem;
    ಸಂರಚನಾ Curl_resolv_async *async = ರಿಕ್ತ;

    data = Curl_multi_get_easy(multi, item->mid);
    ಯದಿ(data)
      async = Curl_async_get(data, item->resolv_id);
    ಯದಿ(async) {
      ಸಂರಚನಾ async_thrdd_item **pdest = &async->thrdd.res_A;

#ifdef CURLRES_IPV6
      ಯದಿ(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#endif
      ಯದಿ(!*pdest) {
        *pdest = item;
        item = ರಿಕ್ತ;
      }
      ಅನ್ಯಥಾ
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(data);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(ಸಂರಚನಾ Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  ಪ್ರತ್ಯಾಗಮ Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

ಸ್ಥಿರ CURLcode async_thrdd_query(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  ಸಂರಚನಾ async_thrdd_item *item;
  CURLcode result;

  item = async_thrdd_item_create(data, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  ಯದಿ(!item) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  CURL_TRC_DNS(data, "queueing query %s", item->description);
  result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  item = ರಿಕ್ತ;
  async->queries_ongoing++;

out:
  ಯದಿ(item)
    async_thrdd_item_free(item);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_async_getaddrinfo(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_resolv_async *async)
{
  CURLcode result = CURLE_FAILED_INIT;
  ಶೂನ್ಯ *resolver = ರಿಕ್ತ;

  ಯದಿ(async->queries_ongoing || async->done)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

#ifdef USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  ಯದಿ((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    result = async_rr_init(data, async);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    resolver = async->thrdd.rr.channel;
  }
#endif

  result = Curl_resolv_announce_start(data, resolver);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

#ifdef USE_HTTPSRR_ARES
  ಯದಿ(resolver) {
    result = async_rr_start(data, async);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
#endif

#ifdef CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  ಯದಿ((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    result = async_thrdd_query(data, async, CURL_DNSQ_AAAA);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
#endif
  ಯದಿ(async->dns_queries & CURL_DNSQ_A) {
    result = async_thrdd_query(data, async, CURL_DNSQ_A);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  ಯದಿ(!async->queries_ongoing)
    async->done = TRUE;

  ಯದಿ(result)
    CURL_TRC_DNS(data, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_async_pollset(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_resolv_async *async,
                            ಸಂರಚನಾ easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(data, async);
#ifdef USE_HTTPSRR_ARES
  ಯದಿ(async->thrdd.rr.channel) {
    CURLcode result = Curl_ares_pollset(data, async->thrdd.rr.channel, ps);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    timeout_ms = Curl_ares_timeout_ms(data, async, async->thrdd.rr.channel);
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)ps;
#endif

  ಯದಿ(!async->done) {
    ಧ್ರುವ ಸಂರಚನಾ curltime *pnow = Curl_pgrs_now(data);
#ifndef ENABLE_INTERNAL_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    ಯದಿ(elapsed_ms < 3)
      stutter_ms = 1;
    ಅನ್ಯಥಾ ಯದಿ(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    ಅನ್ಯಥಾ ಯದಿ(elapsed_ms <= 250)
      stutter_ms = 50;
    ಅನ್ಯಥಾ
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#ಅನ್ಯಥಾ
    ಯದಿ(async->queries_ongoing &&
       !Curl_thrdq_check_started(data->multi->resolv_thrdq)) {
      /* The queue has items but starting a worker thread to process
         them just failed again; expire soon to check once more,
         instead of sleeping on the full resolve timeout. */
      CURL_TRC_DNS(data, "resolver thread start failed again, retrying");
      timeout_ms = CURLMIN(100, timeout_ms);
    }
#endif
    Curl_expire_set(data, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ಯದಿ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ಸ್ಥಿರ bool async_thrdd_item_missing_scope(ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ async_thrdd_item *item)
{
  ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai;

  /* scope id already globally set */
  ಯದಿ(data->conn && data->conn->scope_id)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಪ್ರತಿ(ai = item->res; ai; ai = ai->ai_next) {
    ಯದಿ(ai->ai_family == AF_INET6) {
      ಸಂರಚನಾ sockaddr_in6 *sa6 = (ಶೂನ್ಯ *)ai->ai_addr;
      ಯದಿ(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ async_thrdd_item_strip_results(ಸಂರಚನಾ async_thrdd_item *item,
                                           ಪೂರ್ಣಾಙ್ಕ ai_family)
{
  ಸಂರಚನಾ Curl_addrinfo *ai = item->res, **panchor = &item->res;
  ಯಾವತ್(ai) {
    ಯದಿ(ai->ai_family == ai_family) {
      *panchor = ai->ai_next;
      ai->ai_next = ರಿಕ್ತ;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    ಅನ್ಯಥಾ {
      panchor = &ai->ai_next;
      ai = ai->ai_next;
    }
  }
}

#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

ಸ್ಥಿರ CURLcode async_thrdd_check_done(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಸಂರಚನಾ async_thrdd_ctx *thrdd = &async->thrdd;

  (ಶೂನ್ಯ)data;
  ಯದಿ(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(data, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = ರಿಕ್ತ;
    thrdd->processed_A = TRUE;
  }

  ಯದಿ(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#ಯದಿ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    ಯದಿ(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(data, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      ಸಂರಚನಾ async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode result;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = ರಿಕ್ತ;
      item->dns_queries |= CURL_DNSQ_A;
      ಯದಿ(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = ರಿಕ್ತ;
      }

      CURL_TRC_DNS(data, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      ಯದಿ(result) {
        async_thrdd_item_free(item);
        ಪ್ರತ್ಯಾಗಮ result;
      }
      async->queries_ongoing++;
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(data, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = ರಿಕ್ತ;
    thrdd->processed_AAAA = TRUE;
  }

  ಯದಿ(async->queries_ongoing)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  async->done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_result(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_resolv_async *async,
                                ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಸಂರಚನಾ async_thrdd_ctx *thrdd = &async->thrdd;
  ಸಂರಚನಾ Curl_dns_entry *dns = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ರಿಕ್ತ;

#ifdef USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  ಯದಿ(thrdd->rr.channel)
    (ಶೂನ್ಯ)Curl_ares_perform(thrdd->rr.channel, 0);
#endif
#ifndef ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(data->multi);
#endif

  result = async_thrdd_check_done(data, async);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  Curl_expire_clear(data, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    ಧ್ರುವ uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    ಯದಿ(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    ಯದಿ(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    ಯದಿ(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  ಯದಿ(async->result) {
    result = async->result;
    ಗಚ್ಛ out;
  }

  ಯದಿ(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    ಯದಿ(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }

#ifdef USE_HTTPSRR_ARES
  ಯದಿ(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(data, async->httpsrr);
    dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
    ಯದಿ(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }
#endif

  ಯದಿ(dns) {
    *pdns = dns;
    dns = ರಿಕ್ತ;
  }
  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  Curl_dns_entry_unlink(data, &dns);
  Curl_async_thrdd_shutdown(data, async);
  ಯದಿ(!result && !*pdns)
    result = Curl_async_failed(data, async, ರಿಕ್ತ);
  ಯದಿ(result &&
     (result != CURLE_COULDNT_RESOLVE_HOST) &&
     (result != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(data, "Error %d resolving %s:%d",
                 (ಪೂರ್ಣಾಙ್ಕ)result, async->peer->hostname, async->peer->port);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* USE_RESOLV_THREADED */
