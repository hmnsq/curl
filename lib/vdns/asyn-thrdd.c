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
#ضمن "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ifdef USE_RESOLV_THREADED

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

#ifdef HAVE_GETADDRINFO
#define RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#אחרת
#define RESOLVER_ENOMEM  SOCKENOMEM
#endif

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "url.h"
#ضمن "multiif.h"
#ضمن "curl_threads.h"
#ضمن "progress.h"
#ضمن "rand.h"
#ضمن "select.h"
#ضمن "thrdqueue.h"
#ضمن "vdns/hostip.h"
#ضمن "vdns/httpsrr.h"
#ضمن "curlx/strparse.h"
#ضمن "curlx/wait.h"

#ifdef USE_ARES
#ضمن <ares.h>
#ifdef USE_HTTPSRR
#define USE_HTTPSRR_ARES  /* the combo */
#endif
#endif


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
صحيح Curl_async_global_init(فراغ)
{
#אם defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  אם(ares_library_init(ARES_LIB_INIT_ALL)) {
    החזר CURLE_FAILED_INIT;
  }
#endif
  החזר CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
فراغ Curl_async_global_cleanup(فراغ)
{
#אם defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#endif
}

#ifdef CURLVERBOSE
#define CURL_ASYN_ITEM_DESC_LEN   64
#define async_item_description(x)   (x)->description
#אחרת
#define async_item_description(x)   عدم
#endif

بنية async_thrdd_item {
  بنية Curl_addrinfo *res;
#ifdef CURLVERBOSE
  حرف description[CURL_ASYN_ITEM_DESC_LEN];
#endif
  صحيح sockerr;
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
  حرف hostname[1];
};

/* Give up reference to add_ctx */
ساكن فراغ async_thrdd_item_destroy(بنية async_thrdd_item *item)
{
  אם(item) {
    אם(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
ساكن بنية async_thrdd_item *async_thrdd_item_create(
  بنية Curl_easy *data,
  uint32_t resolv_id, uint8_t dns_queries,
  ثابت حرف *hostname, uint16_t port,
  uint8_t transport)
{
  size_t hostlen = strlen(hostname);
  بنية async_thrdd_item *item;

  item = curlx_calloc(1, حجم(*item) + hostlen);
  אם(!item)
    החזר عدم;

  אם(hostlen) /* null-terminator of name already in struct size */
    memcpy(item->hostname, hostname, hostlen);
  item->mid = data->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#ifdef CURLVERBOSE
  curl_msnprintf(item->description, حجم(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 data->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#endif

#ifdef DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_DELAY");
    אם(p) {
      curl_off_t l;
      אם(!curlx_str_number(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    אם(p) {
      curl_off_t l;
      אם(!curlx_str_number(&p, &l, UINT32_MAX)) {
        unsigned حرف c = 0;
        Curl_rand_bytes(data, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    אם(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#endif

  החזר item;
}

#ifdef USE_HTTPSRR_ARES

ساكن فراغ async_thrdd_rr_done(فراغ *user_data, ares_status_t status,
                                size_t timeouts,
                                ثابت ares_dns_record_t *dnsrec)
{
  بنية Curl_resolv_async *async = user_data;
  بنية async_thrdd_ctx *thrdd = async ? &async->thrdd : عدم;

  (فراغ)timeouts;
  אם(!thrdd)
    החזר;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  אם((ARES_SUCCESS == status) && dnsrec)
    async->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
ساكن CURLcode async_rr_init(بنية Curl_easy *data,
                              بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  صحيح status;

  (فراغ)data;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, عدم, 0);
  אם(status != ARES_SUCCESS) {
    thrdd->rr.channel = عدم;
    החזר CURLE_FAILED_INIT;
  }
#ifdef DEBUGBUILD
  אם(getenv("CURL_DNS_SERVER")) {
    ثابت حرف *servers = getenv("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    אם(status)
      החזר CURLE_FAILED_INIT;
  }
#endif
  החזר CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
ساكن CURLcode async_rr_start(بنية Curl_easy *data,
                               بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  حرف *https_name = عدم;
  CURLcode result = CURLE_OK;

  אם(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    אם(!https_name) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, عدم);
  CURL_TRC_DNS(data, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  החזר result;
}
#endif

فراغ Curl_async_thrdd_shutdown(بنية Curl_easy *data,
                               بنية Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(data, async);
}

بنية async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

ساكن bool async_thrdd_match_item(فراغ *qitem, فراغ *match_data)
{
  ثابت بنية async_thrdd_match_ctx *ctx = match_data;
  بنية async_thrdd_item *item = qitem;
  החזר (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

فراغ Curl_async_thrdd_destroy(بنية Curl_easy *data,
                              بنية Curl_resolv_async *async)
{
  (فراغ)data;
  אם(async->queries_ongoing && !async->done &&
     data->multi && data->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    بنية async_thrdd_match_ctx mctx;
    mctx.mid = data->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(data->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#ifdef USE_HTTPSRR_ARES
  אם(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = عدم;
  }
#endif
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = عدم;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = عدم;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(بنية Curl_easy *data, uint32_t resolv_id,
                          بنية Curl_dns_entry **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  بنية async_thrdd_ctx *thrdd = async ? &async->thrdd : عدم;
  timediff_t milli, ms;
  CURLcode result = CURLE_AGAIN;

  אם(!thrdd)
    החזר CURLE_FAILED_INIT;

  כלעוד(result == CURLE_AGAIN) {
    כלעוד(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(data->multi);
      אם(async->done)
        اكسر;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
      אם(ms < 3)
        milli = 0;
      אחרת אם(ms <= 50)
        milli = ms / 3;
      אחרת אם(ms <= 250)
        milli = 50;
      אחרת
        milli = 200;
      CURL_TRC_DNS(data, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    result = Curl_async_take_result(data, async, pdns);
  }
  החזר result;
}

#ifdef HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
ساكن bool gai_negative(صحيح rc)
{
  بدل(rc) {
#ifdef EAI_NONAME
  حالة EAI_NONAME:
#endif
#אם defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  حالة EAI_NODATA:
#endif
    החזר TRUE;
  افتراضي:
    החזר FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
ساكن فراغ async_thrdd_item_process(فراغ *arg)
{
  بنية async_thrdd_item *item = arg;
  بنية addrinfo hints;
  حرف service[12];
  صحيح pf = PF_INET;
  صحيح rc;

#ifdef DEBUGBUILD
  אם(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  אם(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    החזר;
  }
#endif

  memset(&hints, 0, حجم(hints));
#ifdef CURLRES_IPV6
  אם(item->dns_queries & CURL_DNSQ_AAAA) {
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

  curl_msnprintf(service, حجم(service), "%u", item->port);
#ifdef AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#endif

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  אם(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    אם(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  אחרת {
    Curl_addrinfo_set_port(item->res, item->port);
  }
}

#אחרת /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
ساكن فراغ async_thrdd_item_process(فراغ *arg)
{
  بنية async_thrdd_item *item = arg;

#ifdef DEBUGBUILD
  אם(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  אם(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    החזר;
  }
#endif
  item->res = Curl_ipv4_resolve_r(item->hostname, item->port);
  אם(!item->res) {
    item->sockerr = SOCKERRNO;
    אם(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#endif /* HAVE_GETADDRINFO */

#ifdef ENABLE_INTERNAL_WAKEUP
ساكن فراغ async_thrdd_event(ثابت بنية curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              فراغ *user_data)
{
  بنية Curl_multi *multi = user_data;
  (فراغ)tqueue;
  بدل(ev) {
  حالة CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    اكسر;
  افتراضي:
    اكسر;
  }
}
#אחרת
#define async_thrdd_event   عدم
#endif

ساكن فراغ async_thrdd_item_free(فراغ *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(بنية Curl_multi *multi,
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
  אם(!result) {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    אם(p) {
      curl_off_t l;
      אם(!curlx_str_number(&p, &l, UINT32_MAX)) {
        result = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#endif
  החזר result;
}

/* Tear down the thread queue, joining active threads or detaching them */
فراغ Curl_async_thrdd_multi_destroy(بنية Curl_multi *multi, bool join)
{
  אם(multi->resolv_thrdq) {
#ifdef CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#endif
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = عدم;
  }
}

#ifdef CURLVERBOSE
ساكن فراغ async_thrdd_report_item(بنية Curl_easy *data,
                                    بنية async_thrdd_item *item)
{
  حرف buf[MAX_IPADR_LEN];
  بنية dynbuf tmp;
  ثابت حرف *sep = "";
  ثابت بنية Curl_addrinfo *ai = item->res;
  CURLcode result;
  صحيح ai_family;
#ifdef USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#אחרת
  ai_family = AF_INET;
#endif

  אם(!CURL_TRC_DNS_is_verbose(data))
    החזר;

  curlx_dyn_init(&tmp, 1024);
  עבור(; ai; ai = ai->ai_next) {
    אם(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, حجم(buf));
      result = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      אם(result) {
        CURL_TRC_DNS(data, "too many IP, cannot show");
        اذهب out;
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
فراغ Curl_async_thrdd_multi_process(بنية Curl_multi *multi)
{
  بنية Curl_easy *data;
  فراغ *qitem;

  כלעוד(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    بنية async_thrdd_item *item = qitem;
    بنية Curl_resolv_async *async = عدم;

    data = Curl_multi_get_easy(multi, item->mid);
    אם(data)
      async = Curl_async_get(data, item->resolv_id);
    אם(async) {
      بنية async_thrdd_item **pdest = &async->thrdd.res_A;

#ifdef CURLRES_IPV6
      אם(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#endif
      אם(!*pdest) {
        *pdest = item;
        item = عدم;
      }
      אחרת
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(data);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(بنية Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  החזר Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

ساكن CURLcode async_thrdd_query(بنية Curl_easy *data,
                                  بنية Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  بنية async_thrdd_item *item;
  CURLcode result;

  item = async_thrdd_item_create(data, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  אם(!item) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  CURL_TRC_DNS(data, "queueing query %s", item->description);
  result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  אם(result)
    اذهب out;
  item = عدم;
  async->queries_ongoing++;

out:
  אם(item)
    async_thrdd_item_free(item);
  החזר result;
}

CURLcode Curl_async_getaddrinfo(بنية Curl_easy *data,
                                بنية Curl_resolv_async *async)
{
  CURLcode result = CURLE_FAILED_INIT;
  فراغ *resolver = عدم;

  אם(async->queries_ongoing || async->done)
    החזר CURLE_FAILED_INIT;

#ifdef USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  אם((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    result = async_rr_init(data, async);
    אם(result)
      اذهب out;
    resolver = async->thrdd.rr.channel;
  }
#endif

  result = Curl_resolv_announce_start(data, resolver);
  אם(result)
    החזר result;

#ifdef USE_HTTPSRR_ARES
  אם(resolver) {
    result = async_rr_start(data, async);
    אם(result)
      اذهب out;
  }
#endif

#ifdef CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  אם((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    result = async_thrdd_query(data, async, CURL_DNSQ_AAAA);
    אם(result)
      اذهب out;
  }
#endif
  אם(async->dns_queries & CURL_DNSQ_A) {
    result = async_thrdd_query(data, async, CURL_DNSQ_A);
    אם(result)
      اذهب out;
  }

  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  אם(!async->queries_ongoing)
    async->done = TRUE;

  אם(result)
    CURL_TRC_DNS(data, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (صحيح)result);
  החזר result;
}

CURLcode Curl_async_pollset(بنية Curl_easy *data,
                            بنية Curl_resolv_async *async,
                            بنية easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(data, async);
#ifdef USE_HTTPSRR_ARES
  אם(async->thrdd.rr.channel) {
    CURLcode result = Curl_ares_pollset(data, async->thrdd.rr.channel, ps);
    אם(result)
      החזר result;
    timeout_ms = Curl_ares_timeout_ms(data, async, async->thrdd.rr.channel);
  }
#אחרת
  (فراغ)ps;
#endif

  אם(!async->done) {
    ثابت بنية curltime *pnow = Curl_pgrs_now(data);
#ifndef ENABLE_INTERNAL_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    אם(elapsed_ms < 3)
      stutter_ms = 1;
    אחרת אם(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    אחרת אם(elapsed_ms <= 250)
      stutter_ms = 50;
    אחרת
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#אחרת
    אם(async->queries_ongoing &&
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
  החזר CURLE_OK;
}

#אם defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ساكن bool async_thrdd_item_missing_scope(بنية Curl_easy *data,
                                           بنية async_thrdd_item *item)
{
  ثابت بنية Curl_addrinfo *ai;

  /* scope id already globally set */
  אם(data->conn && data->conn->scope_id)
    החזר FALSE;

  עבור(ai = item->res; ai; ai = ai->ai_next) {
    אם(ai->ai_family == AF_INET6) {
      بنية sockaddr_in6 *sa6 = (فراغ *)ai->ai_addr;
      אם(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        החזר TRUE;
    }
  }
  החזר FALSE;
}

ساكن فراغ async_thrdd_item_strip_results(بنية async_thrdd_item *item,
                                           صحيح ai_family)
{
  بنية Curl_addrinfo *ai = item->res, **panchor = &item->res;
  כלעוד(ai) {
    אם(ai->ai_family == ai_family) {
      *panchor = ai->ai_next;
      ai->ai_next = عدم;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    אחרת {
      panchor = &ai->ai_next;
      ai = ai->ai_next;
    }
  }
}

#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

ساكن CURLcode async_thrdd_check_done(بنية Curl_easy *data,
                                       بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;

  (فراغ)data;
  אם(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(data, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = عدم;
    thrdd->processed_A = TRUE;
  }

  אם(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#אם defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    אם(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(data, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      بنية async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode result;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = عدم;
      item->dns_queries |= CURL_DNSQ_A;
      אם(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = عدم;
      }

      CURL_TRC_DNS(data, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      אם(result) {
        async_thrdd_item_free(item);
        החזר result;
      }
      async->queries_ongoing++;
      החזר CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(data, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = عدم;
    thrdd->processed_AAAA = TRUE;
  }

  אם(async->queries_ongoing)
    החזר CURLE_AGAIN;
  async->done = TRUE;
  החזר CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_result(بنية Curl_easy *data,
                                بنية Curl_resolv_async *async,
                                بنية Curl_dns_entry **pdns)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  بنية Curl_dns_entry *dns = عدم;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم;

#ifdef USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  אם(thrdd->rr.channel)
    (فراغ)Curl_ares_perform(thrdd->rr.channel, 0);
#endif
#ifndef ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(data->multi);
#endif

  result = async_thrdd_check_done(data, async);
  אם(result)
    החזר result;

  Curl_expire_clear(data, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    ثابت uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    אם(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    אם(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    אם(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  אם(async->result) {
    result = async->result;
    اذهب out;
  }

  אם(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    אם(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
  }

#ifdef USE_HTTPSRR_ARES
  אם(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(data, async->httpsrr);
    dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
    אם(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
  }
#endif

  אם(dns) {
    *pdns = dns;
    dns = عدم;
  }
  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  Curl_dns_entry_unlink(data, &dns);
  Curl_async_thrdd_shutdown(data, async);
  אם(!result && !*pdns)
    result = Curl_async_failed(data, async, عدم);
  אם(result &&
     (result != CURLE_COULDNT_RESOLVE_HOST) &&
     (result != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(data, "Error %d resolving %s:%d",
                 (صحيح)result, async->peer->hostname, async->peer->port);
  }
  החזר result;
}

#endif /* USE_RESOLV_THREADED */
