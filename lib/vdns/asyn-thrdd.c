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
#அந்தர்பாவ "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ifdef USE_RESOLV_THREADED

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#ifdef HAVE_GETADDRINFO
#define RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#இல்லையெனில்
#define RESOLVER_ENOMEM  SOCKENOMEM
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "curl_threads.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "thrdqueue.h"
#அந்தர்பாவ "vdns/hostip.h"
#அந்தர்பாவ "vdns/httpsrr.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "curlx/wait.h"

#ifdef USE_ARES
#அந்தர்பாவ <ares.h>
#ifdef USE_HTTPSRR
#define USE_HTTPSRR_ARES  /* the combo */
#endif
#endif


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
முழு Curl_async_global_init(வெறுமை)
{
#எனில் defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  எனில்(ares_library_init(ARES_LIB_INIT_ALL)) {
    திரும்பு CURLE_FAILED_INIT;
  }
#endif
  திரும்பு CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
வெறுமை Curl_async_global_cleanup(வெறுமை)
{
#எனில் defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#endif
}

#ifdef CURLVERBOSE
#define CURL_ASYN_ITEM_DESC_LEN   64
#define async_item_description(x)   (x)->description
#இல்லையெனில்
#define async_item_description(x)   வெற்று
#endif

கட்டமைப்பு async_thrdd_item {
  கட்டமைப்பு Curl_addrinfo *res;
#ifdef CURLVERBOSE
  எழுத்து description[CURL_ASYN_ITEM_DESC_LEN];
#endif
  முழு sockerr;
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
  எழுத்து hostname[1];
};

/* Give up reference to add_ctx */
நிலையான வெறுமை async_thrdd_item_destroy(கட்டமைப்பு async_thrdd_item *item)
{
  எனில்(item) {
    எனில்(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
நிலையான கட்டமைப்பு async_thrdd_item *async_thrdd_item_create(
  கட்டமைப்பு Curl_easy *data,
  uint32_t resolv_id, uint8_t dns_queries,
  மாறிலி எழுத்து *hostname, uint16_t port,
  uint8_t transport)
{
  size_t hostlen = strlen(hostname);
  கட்டமைப்பு async_thrdd_item *item;

  item = curlx_calloc(1, அளவு(*item) + hostlen);
  எனில்(!item)
    திரும்பு வெற்று;

  எனில்(hostlen) /* null-terminator of name already in struct size */
    memcpy(item->hostname, hostname, hostlen);
  item->mid = data->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#ifdef CURLVERBOSE
  curl_msnprintf(item->description, அளவு(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 data->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#endif

#ifdef DEBUGBUILD
  {
    மாறிலி எழுத்து *p = getenv("CURL_DBG_RESOLV_DELAY");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, UINT32_MAX)) {
        unsigned எழுத்து c = 0;
        Curl_rand_bytes(data, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    எனில்(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#endif

  திரும்பு item;
}

#ifdef USE_HTTPSRR_ARES

நிலையான வெறுமை async_thrdd_rr_done(வெறுமை *user_data, ares_status_t status,
                                size_t timeouts,
                                மாறிலி ares_dns_record_t *dnsrec)
{
  கட்டமைப்பு Curl_resolv_async *async = user_data;
  கட்டமைப்பு async_thrdd_ctx *thrdd = async ? &async->thrdd : வெற்று;

  (வெறுமை)timeouts;
  எனில்(!thrdd)
    திரும்பு;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  எனில்((ARES_SUCCESS == status) && dnsrec)
    async->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
நிலையான CURLcode async_rr_init(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_resolv_async *async)
{
  கட்டமைப்பு async_thrdd_ctx *thrdd = &async->thrdd;
  முழு status;

  (வெறுமை)data;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, வெற்று, 0);
  எனில்(status != ARES_SUCCESS) {
    thrdd->rr.channel = வெற்று;
    திரும்பு CURLE_FAILED_INIT;
  }
#ifdef DEBUGBUILD
  எனில்(getenv("CURL_DNS_SERVER")) {
    மாறிலி எழுத்து *servers = getenv("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    எனில்(status)
      திரும்பு CURLE_FAILED_INIT;
  }
#endif
  திரும்பு CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
நிலையான CURLcode async_rr_start(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_resolv_async *async)
{
  கட்டமைப்பு async_thrdd_ctx *thrdd = &async->thrdd;
  எழுத்து *https_name = வெற்று;
  CURLcode result = CURLE_OK;

  எனில்(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    எனில்(!https_name) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, வெற்று);
  CURL_TRC_DNS(data, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  திரும்பு result;
}
#endif

வெறுமை Curl_async_thrdd_shutdown(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(data, async);
}

கட்டமைப்பு async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

நிலையான bool async_thrdd_match_item(வெறுமை *qitem, வெறுமை *match_data)
{
  மாறிலி கட்டமைப்பு async_thrdd_match_ctx *ctx = match_data;
  கட்டமைப்பு async_thrdd_item *item = qitem;
  திரும்பு (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

வெறுமை Curl_async_thrdd_destroy(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_resolv_async *async)
{
  (வெறுமை)data;
  எனில்(async->queries_ongoing && !async->done &&
     data->multi && data->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    கட்டமைப்பு async_thrdd_match_ctx mctx;
    mctx.mid = data->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(data->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#ifdef USE_HTTPSRR_ARES
  எனில்(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = வெற்று;
  }
#endif
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = வெற்று;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = வெற்று;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(கட்டமைப்பு Curl_easy *data, uint32_t resolv_id,
                          கட்டமைப்பு Curl_dns_entry **pdns)
{
  கட்டமைப்பு Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  கட்டமைப்பு async_thrdd_ctx *thrdd = async ? &async->thrdd : வெற்று;
  timediff_t milli, ms;
  CURLcode result = CURLE_AGAIN;

  எனில்(!thrdd)
    திரும்பு CURLE_FAILED_INIT;

  போது(result == CURLE_AGAIN) {
    போது(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(data->multi);
      எனில்(async->done)
        நிறுத்து;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
      எனில்(ms < 3)
        milli = 0;
      இல்லையெனில் எனில்(ms <= 50)
        milli = ms / 3;
      இல்லையெனில் எனில்(ms <= 250)
        milli = 50;
      இல்லையெனில்
        milli = 200;
      CURL_TRC_DNS(data, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    result = Curl_async_take_result(data, async, pdns);
  }
  திரும்பு result;
}

#ifdef HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
நிலையான bool gai_negative(முழு rc)
{
  தேர்வு(rc) {
#ifdef EAI_NONAME
  நிலை EAI_NONAME:
#endif
#எனில் defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  நிலை EAI_NODATA:
#endif
    திரும்பு TRUE;
  இயல்பு:
    திரும்பு FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
நிலையான வெறுமை async_thrdd_item_process(வெறுமை *arg)
{
  கட்டமைப்பு async_thrdd_item *item = arg;
  கட்டமைப்பு addrinfo hints;
  எழுத்து service[12];
  முழு pf = PF_INET;
  முழு rc;

#ifdef DEBUGBUILD
  எனில்(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  எனில்(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    திரும்பு;
  }
#endif

  memset(&hints, 0, அளவு(hints));
#ifdef CURLRES_IPV6
  எனில்(item->dns_queries & CURL_DNSQ_AAAA) {
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

  curl_msnprintf(service, அளவு(service), "%u", item->port);
#ifdef AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#endif

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  எனில்(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    எனில்(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  இல்லையெனில் {
    Curl_addrinfo_set_port(item->res, item->port);
  }
}

#இல்லையெனில் /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
நிலையான வெறுமை async_thrdd_item_process(வெறுமை *arg)
{
  கட்டமைப்பு async_thrdd_item *item = arg;

#ifdef DEBUGBUILD
  எனில்(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  எனில்(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    திரும்பு;
  }
#endif
  item->res = Curl_ipv4_resolve_r(item->hostname, item->port);
  எனில்(!item->res) {
    item->sockerr = SOCKERRNO;
    எனில்(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#endif /* HAVE_GETADDRINFO */

#ifdef ENABLE_INTERNAL_WAKEUP
நிலையான வெறுமை async_thrdd_event(மாறிலி கட்டமைப்பு curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              வெறுமை *user_data)
{
  கட்டமைப்பு Curl_multi *multi = user_data;
  (வெறுமை)tqueue;
  தேர்வு(ev) {
  நிலை CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
}
#இல்லையெனில்
#define async_thrdd_event   வெற்று
#endif

நிலையான வெறுமை async_thrdd_item_free(வெறுமை *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(கட்டமைப்பு Curl_multi *multi,
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
  எனில்(!result) {
    மாறிலி எழுத்து *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, UINT32_MAX)) {
        result = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#endif
  திரும்பு result;
}

/* Tear down the thread queue, joining active threads or detaching them */
வெறுமை Curl_async_thrdd_multi_destroy(கட்டமைப்பு Curl_multi *multi, bool join)
{
  எனில்(multi->resolv_thrdq) {
#ifdef CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#endif
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = வெற்று;
  }
}

#ifdef CURLVERBOSE
நிலையான வெறுமை async_thrdd_report_item(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு async_thrdd_item *item)
{
  எழுத்து buf[MAX_IPADR_LEN];
  கட்டமைப்பு dynbuf tmp;
  மாறிலி எழுத்து *sep = "";
  மாறிலி கட்டமைப்பு Curl_addrinfo *ai = item->res;
  CURLcode result;
  முழு ai_family;
#ifdef USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#இல்லையெனில்
  ai_family = AF_INET;
#endif

  எனில்(!CURL_TRC_DNS_is_verbose(data))
    திரும்பு;

  curlx_dyn_init(&tmp, 1024);
  க்கு(; ai; ai = ai->ai_next) {
    எனில்(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, அளவு(buf));
      result = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      எனில்(result) {
        CURL_TRC_DNS(data, "too many IP, cannot show");
        செல் out;
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
வெறுமை Curl_async_thrdd_multi_process(கட்டமைப்பு Curl_multi *multi)
{
  கட்டமைப்பு Curl_easy *data;
  வெறுமை *qitem;

  போது(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    கட்டமைப்பு async_thrdd_item *item = qitem;
    கட்டமைப்பு Curl_resolv_async *async = வெற்று;

    data = Curl_multi_get_easy(multi, item->mid);
    எனில்(data)
      async = Curl_async_get(data, item->resolv_id);
    எனில்(async) {
      கட்டமைப்பு async_thrdd_item **pdest = &async->thrdd.res_A;

#ifdef CURLRES_IPV6
      எனில்(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#endif
      எனில்(!*pdest) {
        *pdest = item;
        item = வெற்று;
      }
      இல்லையெனில்
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(data);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(கட்டமைப்பு Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  திரும்பு Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

நிலையான CURLcode async_thrdd_query(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  கட்டமைப்பு async_thrdd_item *item;
  CURLcode result;

  item = async_thrdd_item_create(data, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  எனில்(!item) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  CURL_TRC_DNS(data, "queueing query %s", item->description);
  result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  எனில்(result)
    செல் out;
  item = வெற்று;
  async->queries_ongoing++;

out:
  எனில்(item)
    async_thrdd_item_free(item);
  திரும்பு result;
}

CURLcode Curl_async_getaddrinfo(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_resolv_async *async)
{
  CURLcode result = CURLE_FAILED_INIT;
  வெறுமை *resolver = வெற்று;

  எனில்(async->queries_ongoing || async->done)
    திரும்பு CURLE_FAILED_INIT;

#ifdef USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  எனில்((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    result = async_rr_init(data, async);
    எனில்(result)
      செல் out;
    resolver = async->thrdd.rr.channel;
  }
#endif

  result = Curl_resolv_announce_start(data, resolver);
  எனில்(result)
    திரும்பு result;

#ifdef USE_HTTPSRR_ARES
  எனில்(resolver) {
    result = async_rr_start(data, async);
    எனில்(result)
      செல் out;
  }
#endif

#ifdef CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  எனில்((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    result = async_thrdd_query(data, async, CURL_DNSQ_AAAA);
    எனில்(result)
      செல் out;
  }
#endif
  எனில்(async->dns_queries & CURL_DNSQ_A) {
    result = async_thrdd_query(data, async, CURL_DNSQ_A);
    எனில்(result)
      செல் out;
  }

  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  எனில்(!async->queries_ongoing)
    async->done = TRUE;

  எனில்(result)
    CURL_TRC_DNS(data, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (முழு)result);
  திரும்பு result;
}

CURLcode Curl_async_pollset(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_resolv_async *async,
                            கட்டமைப்பு easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(data, async);
#ifdef USE_HTTPSRR_ARES
  எனில்(async->thrdd.rr.channel) {
    CURLcode result = Curl_ares_pollset(data, async->thrdd.rr.channel, ps);
    எனில்(result)
      திரும்பு result;
    timeout_ms = Curl_ares_timeout_ms(data, async, async->thrdd.rr.channel);
  }
#இல்லையெனில்
  (வெறுமை)ps;
#endif

  எனில்(!async->done) {
    மாறிலி கட்டமைப்பு curltime *pnow = Curl_pgrs_now(data);
#ifndef ENABLE_INTERNAL_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    எனில்(elapsed_ms < 3)
      stutter_ms = 1;
    இல்லையெனில் எனில்(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    இல்லையெனில் எனில்(elapsed_ms <= 250)
      stutter_ms = 50;
    இல்லையெனில்
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#இல்லையெனில்
    எனில்(async->queries_ongoing &&
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
  திரும்பு CURLE_OK;
}

#எனில் defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
நிலையான bool async_thrdd_item_missing_scope(கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு async_thrdd_item *item)
{
  மாறிலி கட்டமைப்பு Curl_addrinfo *ai;

  /* scope id already globally set */
  எனில்(data->conn && data->conn->scope_id)
    திரும்பு FALSE;

  க்கு(ai = item->res; ai; ai = ai->ai_next) {
    எனில்(ai->ai_family == AF_INET6) {
      கட்டமைப்பு sockaddr_in6 *sa6 = (வெறுமை *)ai->ai_addr;
      எனில்(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        திரும்பு TRUE;
    }
  }
  திரும்பு FALSE;
}

நிலையான வெறுமை async_thrdd_item_strip_results(கட்டமைப்பு async_thrdd_item *item,
                                           முழு ai_family)
{
  கட்டமைப்பு Curl_addrinfo *ai = item->res, **panchor = &item->res;
  போது(ai) {
    எனில்(ai->ai_family == ai_family) {
      *panchor = ai->ai_next;
      ai->ai_next = வெற்று;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    இல்லையெனில் {
      panchor = &ai->ai_next;
      ai = ai->ai_next;
    }
  }
}

#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

நிலையான CURLcode async_thrdd_check_done(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு Curl_resolv_async *async)
{
  கட்டமைப்பு async_thrdd_ctx *thrdd = &async->thrdd;

  (வெறுமை)data;
  எனில்(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(data, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = வெற்று;
    thrdd->processed_A = TRUE;
  }

  எனில்(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#எனில் defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    எனில்(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(data, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      கட்டமைப்பு async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode result;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = வெற்று;
      item->dns_queries |= CURL_DNSQ_A;
      எனில்(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = வெற்று;
      }

      CURL_TRC_DNS(data, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      எனில்(result) {
        async_thrdd_item_free(item);
        திரும்பு result;
      }
      async->queries_ongoing++;
      திரும்பு CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(data, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = வெற்று;
    thrdd->processed_AAAA = TRUE;
  }

  எனில்(async->queries_ongoing)
    திரும்பு CURLE_AGAIN;
  async->done = TRUE;
  திரும்பு CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_result(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_resolv_async *async,
                                கட்டமைப்பு Curl_dns_entry **pdns)
{
  கட்டமைப்பு async_thrdd_ctx *thrdd = &async->thrdd;
  கட்டமைப்பு Curl_dns_entry *dns = வெற்று;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = வெற்று;

#ifdef USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  எனில்(thrdd->rr.channel)
    (வெறுமை)Curl_ares_perform(thrdd->rr.channel, 0);
#endif
#ifndef ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(data->multi);
#endif

  result = async_thrdd_check_done(data, async);
  எனில்(result)
    திரும்பு result;

  Curl_expire_clear(data, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    மாறிலி uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    எனில்(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    எனில்(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    எனில்(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  எனில்(async->result) {
    result = async->result;
    செல் out;
  }

  எனில்(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    எனில்(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }

#ifdef USE_HTTPSRR_ARES
  எனில்(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(data, async->httpsrr);
    dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
    எனில்(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }
#endif

  எனில்(dns) {
    *pdns = dns;
    dns = வெற்று;
  }
  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  Curl_dns_entry_unlink(data, &dns);
  Curl_async_thrdd_shutdown(data, async);
  எனில்(!result && !*pdns)
    result = Curl_async_failed(data, async, வெற்று);
  எனில்(result &&
     (result != CURLE_COULDNT_RESOLVE_HOST) &&
     (result != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(data, "Error %d resolving %s:%d",
                 (முழு)result, async->peer->hostname, async->peer->port);
  }
  திரும்பு result;
}

#endif /* USE_RESOLV_THREADED */
