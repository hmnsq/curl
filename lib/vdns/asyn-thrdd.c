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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ifdef USE_RESOLV_THREADED

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ifdef HAVE_GETADDRINFO
#define RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#ଅନ୍ୟଥା
#define RESOLVER_ENOMEM  SOCKENOMEM
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "curl_threads.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "thrdqueue.h"
#ଅନ୍ତର୍ଭାଵ "vdns/hostip.h"
#ଅନ୍ତର୍ଭାଵ "vdns/httpsrr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curlx/wait.h"

#ifdef USE_ARES
#ଅନ୍ତର୍ଭାଵ <ares.h>
#ifdef USE_HTTPSRR
#define USE_HTTPSRR_ARES  /* the combo */
#endif
#endif


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_async_global_init(ଶୂନ୍ୟ)
{
#ଯଦି defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ଯଦି(ares_library_init(ARES_LIB_INIT_ALL)) {
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
ଶୂନ୍ୟ Curl_async_global_cleanup(ଶୂନ୍ୟ)
{
#ଯଦି defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#endif
}

#ifdef CURLVERBOSE
#define CURL_ASYN_ITEM_DESC_LEN   64
#define async_item_description(x)   (x)->description
#ଅନ୍ୟଥା
#define async_item_description(x)   ଖାଲି
#endif

ସଂରଚନା async_thrdd_item {
  ସଂରଚନା Curl_addrinfo *res;
#ifdef CURLVERBOSE
  ଅକ୍ଷର description[CURL_ASYN_ITEM_DESC_LEN];
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr;
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
  ଅକ୍ଷର hostname[1];
};

/* Give up reference to add_ctx */
ସ୍ଥିର ଶୂନ୍ୟ async_thrdd_item_destroy(ସଂରଚନା async_thrdd_item *item)
{
  ଯଦି(item) {
    ଯଦି(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
ସ୍ଥିର ସଂରଚନା async_thrdd_item *async_thrdd_item_create(
  ସଂରଚନା Curl_easy *data,
  uint32_t resolv_id, uint8_t dns_queries,
  ଅଚଳ ଅକ୍ଷର *hostname, uint16_t port,
  uint8_t transport)
{
  size_t hostlen = strlen(hostname);
  ସଂରଚନା async_thrdd_item *item;

  item = curlx_calloc(1, ଆକାର(*item) + hostlen);
  ଯଦି(!item)
    ଫେରନ୍ତୁ ଖାଲି;

  ଯଦି(hostlen) /* null-terminator of name already in struct size */
    memcpy(item->hostname, hostname, hostlen);
  item->mid = data->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#ifdef CURLVERBOSE
  curl_msnprintf(item->description, ଆକାର(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 data->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#endif

#ifdef DEBUGBUILD
  {
    ଅଚଳ ଅକ୍ଷର *p = getenv("CURL_DBG_RESOLV_DELAY");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, UINT32_MAX)) {
        unsigned ଅକ୍ଷର c = 0;
        Curl_rand_bytes(data, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    ଯଦି(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#endif

  ଫେରନ୍ତୁ item;
}

#ifdef USE_HTTPSRR_ARES

ସ୍ଥିର ଶୂନ୍ୟ async_thrdd_rr_done(ଶୂନ୍ୟ *user_data, ares_status_t status,
                                size_t timeouts,
                                ଅଚଳ ares_dns_record_t *dnsrec)
{
  ସଂରଚନା Curl_resolv_async *async = user_data;
  ସଂରଚନା async_thrdd_ctx *thrdd = async ? &async->thrdd : ଖାଲି;

  (ଶୂନ୍ୟ)timeouts;
  ଯଦି(!thrdd)
    ଫେରନ୍ତୁ;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ଯଦି((ARES_SUCCESS == status) && dnsrec)
    async->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
ସ୍ଥିର CURLcode async_rr_init(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_resolv_async *async)
{
  ସଂରଚନା async_thrdd_ctx *thrdd = &async->thrdd;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା status;

  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, ଖାଲି, 0);
  ଯଦି(status != ARES_SUCCESS) {
    thrdd->rr.channel = ଖାଲି;
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
#ifdef DEBUGBUILD
  ଯଦି(getenv("CURL_DNS_SERVER")) {
    ଅଚଳ ଅକ୍ଷର *servers = getenv("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    ଯଦି(status)
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
ସ୍ଥିର CURLcode async_rr_start(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_resolv_async *async)
{
  ସଂରଚନା async_thrdd_ctx *thrdd = &async->thrdd;
  ଅକ୍ଷର *https_name = ଖାଲି;
  CURLcode result = CURLE_OK;

  ଯଦି(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    ଯଦି(!https_name) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, ଖାଲି);
  CURL_TRC_DNS(data, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  ଫେରନ୍ତୁ result;
}
#endif

ଶୂନ୍ୟ Curl_async_thrdd_shutdown(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(data, async);
}

ସଂରଚନା async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

ସ୍ଥିର bool async_thrdd_match_item(ଶୂନ୍ୟ *qitem, ଶୂନ୍ୟ *match_data)
{
  ଅଚଳ ସଂରଚନା async_thrdd_match_ctx *ctx = match_data;
  ସଂରଚନା async_thrdd_item *item = qitem;
  ଫେରନ୍ତୁ (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

ଶୂନ୍ୟ Curl_async_thrdd_destroy(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_resolv_async *async)
{
  (ଶୂନ୍ୟ)data;
  ଯଦି(async->queries_ongoing && !async->done &&
     data->multi && data->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    ସଂରଚନା async_thrdd_match_ctx mctx;
    mctx.mid = data->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(data->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#ifdef USE_HTTPSRR_ARES
  ଯଦି(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = ଖାଲି;
  }
#endif
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = ଖାଲି;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = ଖାଲି;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(ସଂରଚନା Curl_easy *data, uint32_t resolv_id,
                          ସଂରଚନା Curl_dns_entry **pdns)
{
  ସଂରଚନା Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ସଂରଚନା async_thrdd_ctx *thrdd = async ? &async->thrdd : ଖାଲି;
  timediff_t milli, ms;
  CURLcode result = CURLE_AGAIN;

  ଯଦି(!thrdd)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯେତେବେଳେ(result == CURLE_AGAIN) {
    ଯେତେବେଳେ(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(data->multi);
      ଯଦି(async->done)
        ଭାଙ୍ଗ;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
      ଯଦି(ms < 3)
        milli = 0;
      ଅନ୍ୟଥା ଯଦି(ms <= 50)
        milli = ms / 3;
      ଅନ୍ୟଥା ଯଦି(ms <= 250)
        milli = 50;
      ଅନ୍ୟଥା
        milli = 200;
      CURL_TRC_DNS(data, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    result = Curl_async_take_result(data, async, pdns);
  }
  ଫେରନ୍ତୁ result;
}

#ifdef HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
ସ୍ଥିର bool gai_negative(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc)
{
  ବାଛ(rc) {
#ifdef EAI_NONAME
  ସ୍ଥିତି EAI_NONAME:
#endif
#ଯଦି defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  ସ୍ଥିତି EAI_NODATA:
#endif
    ଫେରନ୍ତୁ TRUE;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
ସ୍ଥିର ଶୂନ୍ୟ async_thrdd_item_process(ଶୂନ୍ୟ *arg)
{
  ସଂରଚନା async_thrdd_item *item = arg;
  ସଂରଚନା addrinfo hints;
  ଅକ୍ଷର service[12];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pf = PF_INET;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

#ifdef DEBUGBUILD
  ଯଦି(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  ଯଦି(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    ଫେରନ୍ତୁ;
  }
#endif

  memset(&hints, 0, ଆକାର(hints));
#ifdef CURLRES_IPV6
  ଯଦି(item->dns_queries & CURL_DNSQ_AAAA) {
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

  curl_msnprintf(service, ଆକାର(service), "%u", item->port);
#ifdef AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#endif

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  ଯଦି(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    ଯଦି(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  ଅନ୍ୟଥା {
    Curl_addrinfo_set_port(item->res, item->port);
  }
}

#ଅନ୍ୟଥା /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
ସ୍ଥିର ଶୂନ୍ୟ async_thrdd_item_process(ଶୂନ୍ୟ *arg)
{
  ସଂରଚନା async_thrdd_item *item = arg;

#ifdef DEBUGBUILD
  ଯଦି(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  ଯଦି(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    ଫେରନ୍ତୁ;
  }
#endif
  item->res = Curl_ipv4_resolve_r(item->hostname, item->port);
  ଯଦି(!item->res) {
    item->sockerr = SOCKERRNO;
    ଯଦି(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#endif /* HAVE_GETADDRINFO */

#ifdef ENABLE_INTERNAL_WAKEUP
ସ୍ଥିର ଶୂନ୍ୟ async_thrdd_event(ଅଚଳ ସଂରଚନା curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା Curl_multi *multi = user_data;
  (ଶୂନ୍ୟ)tqueue;
  ବାଛ(ev) {
  ସ୍ଥିତି CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
}
#ଅନ୍ୟଥା
#define async_thrdd_event   ଖାଲି
#endif

ସ୍ଥିର ଶୂନ୍ୟ async_thrdd_item_free(ଶୂନ୍ୟ *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(ସଂରଚନା Curl_multi *multi,
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
  ଯଦି(!result) {
    ଅଚଳ ଅକ୍ଷର *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, UINT32_MAX)) {
        result = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#endif
  ଫେରନ୍ତୁ result;
}

/* Tear down the thread queue, joining active threads or detaching them */
ଶୂନ୍ୟ Curl_async_thrdd_multi_destroy(ସଂରଚନା Curl_multi *multi, bool join)
{
  ଯଦି(multi->resolv_thrdq) {
#ifdef CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#endif
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = ଖାଲି;
  }
}

#ifdef CURLVERBOSE
ସ୍ଥିର ଶୂନ୍ୟ async_thrdd_report_item(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା async_thrdd_item *item)
{
  ଅକ୍ଷର buf[MAX_IPADR_LEN];
  ସଂରଚନା dynbuf tmp;
  ଅଚଳ ଅକ୍ଷର *sep = "";
  ଅଚଳ ସଂରଚନା Curl_addrinfo *ai = item->res;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family;
#ifdef USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#ଅନ୍ୟଥା
  ai_family = AF_INET;
#endif

  ଯଦି(!CURL_TRC_DNS_is_verbose(data))
    ଫେରନ୍ତୁ;

  curlx_dyn_init(&tmp, 1024);
  ପାଇଁ(; ai; ai = ai->ai_next) {
    ଯଦି(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, ଆକାର(buf));
      result = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      ଯଦି(result) {
        CURL_TRC_DNS(data, "too many IP, cannot show");
        ଯାଅ out;
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
ଶୂନ୍ୟ Curl_async_thrdd_multi_process(ସଂରଚନା Curl_multi *multi)
{
  ସଂରଚନା Curl_easy *data;
  ଶୂନ୍ୟ *qitem;

  ଯେତେବେଳେ(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    ସଂରଚନା async_thrdd_item *item = qitem;
    ସଂରଚନା Curl_resolv_async *async = ଖାଲି;

    data = Curl_multi_get_easy(multi, item->mid);
    ଯଦି(data)
      async = Curl_async_get(data, item->resolv_id);
    ଯଦି(async) {
      ସଂରଚନା async_thrdd_item **pdest = &async->thrdd.res_A;

#ifdef CURLRES_IPV6
      ଯଦି(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#endif
      ଯଦି(!*pdest) {
        *pdest = item;
        item = ଖାଲି;
      }
      ଅନ୍ୟଥା
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(data);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(ସଂରଚନା Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  ଫେରନ୍ତୁ Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

ସ୍ଥିର CURLcode async_thrdd_query(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  ସଂରଚନା async_thrdd_item *item;
  CURLcode result;

  item = async_thrdd_item_create(data, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  ଯଦି(!item) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  CURL_TRC_DNS(data, "queueing query %s", item->description);
  result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  ଯଦି(result)
    ଯାଅ out;
  item = ଖାଲି;
  async->queries_ongoing++;

out:
  ଯଦି(item)
    async_thrdd_item_free(item);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_async_getaddrinfo(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_resolv_async *async)
{
  CURLcode result = CURLE_FAILED_INIT;
  ଶୂନ୍ୟ *resolver = ଖାଲି;

  ଯଦି(async->queries_ongoing || async->done)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

#ifdef USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  ଯଦି((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    result = async_rr_init(data, async);
    ଯଦି(result)
      ଯାଅ out;
    resolver = async->thrdd.rr.channel;
  }
#endif

  result = Curl_resolv_announce_start(data, resolver);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

#ifdef USE_HTTPSRR_ARES
  ଯଦି(resolver) {
    result = async_rr_start(data, async);
    ଯଦି(result)
      ଯାଅ out;
  }
#endif

#ifdef CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  ଯଦି((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    result = async_thrdd_query(data, async, CURL_DNSQ_AAAA);
    ଯଦି(result)
      ଯାଅ out;
  }
#endif
  ଯଦି(async->dns_queries & CURL_DNSQ_A) {
    result = async_thrdd_query(data, async, CURL_DNSQ_A);
    ଯଦି(result)
      ଯାଅ out;
  }

  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  ଯଦି(!async->queries_ongoing)
    async->done = TRUE;

  ଯଦି(result)
    CURL_TRC_DNS(data, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_async_pollset(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_resolv_async *async,
                            ସଂରଚନା easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(data, async);
#ifdef USE_HTTPSRR_ARES
  ଯଦି(async->thrdd.rr.channel) {
    CURLcode result = Curl_ares_pollset(data, async->thrdd.rr.channel, ps);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    timeout_ms = Curl_ares_timeout_ms(data, async, async->thrdd.rr.channel);
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)ps;
#endif

  ଯଦି(!async->done) {
    ଅଚଳ ସଂରଚନା curltime *pnow = Curl_pgrs_now(data);
#ifndef ENABLE_INTERNAL_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    ଯଦି(elapsed_ms < 3)
      stutter_ms = 1;
    ଅନ୍ୟଥା ଯଦି(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    ଅନ୍ୟଥା ଯଦି(elapsed_ms <= 250)
      stutter_ms = 50;
    ଅନ୍ୟଥା
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#ଅନ୍ୟଥା
    ଯଦି(async->queries_ongoing &&
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
  ଫେରନ୍ତୁ CURLE_OK;
}

#ଯଦି defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ସ୍ଥିର bool async_thrdd_item_missing_scope(ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା async_thrdd_item *item)
{
  ଅଚଳ ସଂରଚନା Curl_addrinfo *ai;

  /* scope id already globally set */
  ଯଦି(data->conn && data->conn->scope_id)
    ଫେରନ୍ତୁ FALSE;

  ପାଇଁ(ai = item->res; ai; ai = ai->ai_next) {
    ଯଦି(ai->ai_family == AF_INET6) {
      ସଂରଚନା sockaddr_in6 *sa6 = (ଶୂନ୍ୟ *)ai->ai_addr;
      ଯଦି(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        ଫେରନ୍ତୁ TRUE;
    }
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ଶୂନ୍ୟ async_thrdd_item_strip_results(ସଂରଚନା async_thrdd_item *item,
                                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family)
{
  ସଂରଚନା Curl_addrinfo *ai = item->res, **panchor = &item->res;
  ଯେତେବେଳେ(ai) {
    ଯଦି(ai->ai_family == ai_family) {
      *panchor = ai->ai_next;
      ai->ai_next = ଖାଲି;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    ଅନ୍ୟଥା {
      panchor = &ai->ai_next;
      ai = ai->ai_next;
    }
  }
}

#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

ସ୍ଥିର CURLcode async_thrdd_check_done(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା Curl_resolv_async *async)
{
  ସଂରଚନା async_thrdd_ctx *thrdd = &async->thrdd;

  (ଶୂନ୍ୟ)data;
  ଯଦି(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(data, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = ଖାଲି;
    thrdd->processed_A = TRUE;
  }

  ଯଦି(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#ଯଦି defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    ଯଦି(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(data, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      ସଂରଚନା async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode result;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = ଖାଲି;
      item->dns_queries |= CURL_DNSQ_A;
      ଯଦି(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = ଖାଲି;
      }

      CURL_TRC_DNS(data, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      ଯଦି(result) {
        async_thrdd_item_free(item);
        ଫେରନ୍ତୁ result;
      }
      async->queries_ongoing++;
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(data, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = ଖାଲି;
    thrdd->processed_AAAA = TRUE;
  }

  ଯଦି(async->queries_ongoing)
    ଫେରନ୍ତୁ CURLE_AGAIN;
  async->done = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_result(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_resolv_async *async,
                                ସଂରଚନା Curl_dns_entry **pdns)
{
  ସଂରଚନା async_thrdd_ctx *thrdd = &async->thrdd;
  ସଂରଚନା Curl_dns_entry *dns = ଖାଲି;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ଖାଲି;

#ifdef USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  ଯଦି(thrdd->rr.channel)
    (ଶୂନ୍ୟ)Curl_ares_perform(thrdd->rr.channel, 0);
#endif
#ifndef ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(data->multi);
#endif

  result = async_thrdd_check_done(data, async);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  Curl_expire_clear(data, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    ଅଚଳ uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    ଯଦି(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    ଯଦି(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    ଯଦି(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  ଯଦି(async->result) {
    result = async->result;
    ଯାଅ out;
  }

  ଯଦି(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    ଯଦି(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
  }

#ifdef USE_HTTPSRR_ARES
  ଯଦି(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(data, async->httpsrr);
    dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
    ଯଦି(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
  }
#endif

  ଯଦି(dns) {
    *pdns = dns;
    dns = ଖାଲି;
  }
  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  Curl_dns_entry_unlink(data, &dns);
  Curl_async_thrdd_shutdown(data, async);
  ଯଦି(!result && !*pdns)
    result = Curl_async_failed(data, async, ଖାଲି);
  ଯଦି(result &&
     (result != CURLE_COULDNT_RESOLVE_HOST) &&
     (result != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(data, "Error %d resolving %s:%d",
                 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, async->peer->hostname, async->peer->port);
  }
  ଫେରନ୍ତୁ result;
}

#endif /* USE_RESOLV_THREADED */
