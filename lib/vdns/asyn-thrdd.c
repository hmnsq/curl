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
#شامل "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ifdef USE_RESOLV_THREADED

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

#ifdef HAVE_GETADDRINFO
#define RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#وگرنه
#define RESOLVER_ENOMEM  SOCKENOMEM
#endif

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "url.h"
#شامل "multiif.h"
#شامل "curl_threads.h"
#شامل "progress.h"
#شامل "rand.h"
#شامل "select.h"
#شامل "thrdqueue.h"
#شامل "vdns/hostip.h"
#شامل "vdns/httpsrr.h"
#شامل "curlx/strparse.h"
#شامل "curlx/wait.h"

#ifdef USE_ARES
#شامل <ares.h>
#ifdef USE_HTTPSRR
#define USE_HTTPSRR_ARES  /* the combo */
#endif
#endif


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
صحیح Curl_async_global_init(تهی)
{
#اگر defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  اگر(ares_library_init(ARES_LIB_INIT_ALL)) {
    بازگشت CURLE_FAILED_INIT;
  }
#endif
  بازگشت CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
تهی Curl_async_global_cleanup(تهی)
{
#اگر defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#endif
}

#ifdef CURLVERBOSE
#define CURL_ASYN_ITEM_DESC_LEN   64
#define async_item_description(x)   (x)->description
#وگرنه
#define async_item_description(x)   پوچ
#endif

ساختار async_thrdd_item {
  ساختار Curl_addrinfo *res;
#ifdef CURLVERBOSE
  نویسه description[CURL_ASYN_ITEM_DESC_LEN];
#endif
  صحیح sockerr;
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
  نویسه hostname[1];
};

/* Give up reference to add_ctx */
ایستا تهی async_thrdd_item_destroy(ساختار async_thrdd_item *item)
{
  اگر(item) {
    اگر(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
ایستا ساختار async_thrdd_item *async_thrdd_item_create(
  ساختار Curl_easy *data,
  uint32_t resolv_id, uint8_t dns_queries,
  ثابت نویسه *hostname, uint16_t port,
  uint8_t transport)
{
  size_t hostlen = strlen(hostname);
  ساختار async_thrdd_item *item;

  item = curlx_calloc(1, اندازه(*item) + hostlen);
  اگر(!item)
    بازگشت پوچ;

  اگر(hostlen) /* null-terminator of name already in struct size */
    memcpy(item->hostname, hostname, hostlen);
  item->mid = data->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#ifdef CURLVERBOSE
  curl_msnprintf(item->description, اندازه(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 data->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#endif

#ifdef DEBUGBUILD
  {
    ثابت نویسه *p = getenv("CURL_DBG_RESOLV_DELAY");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        unsigned نویسه c = 0;
        Curl_rand_bytes(data, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    اگر(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#endif

  بازگشت item;
}

#ifdef USE_HTTPSRR_ARES

ایستا تهی async_thrdd_rr_done(تهی *user_data, ares_status_t status,
                                size_t timeouts,
                                ثابت ares_dns_record_t *dnsrec)
{
  ساختار Curl_resolv_async *async = user_data;
  ساختار async_thrdd_ctx *thrdd = async ? &async->thrdd : پوچ;

  (تهی)timeouts;
  اگر(!thrdd)
    بازگشت;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  اگر((ARES_SUCCESS == status) && dnsrec)
    async->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
ایستا CURLcode async_rr_init(ساختار Curl_easy *data,
                              ساختار Curl_resolv_async *async)
{
  ساختار async_thrdd_ctx *thrdd = &async->thrdd;
  صحیح status;

  (تهی)data;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, پوچ, 0);
  اگر(status != ARES_SUCCESS) {
    thrdd->rr.channel = پوچ;
    بازگشت CURLE_FAILED_INIT;
  }
#ifdef DEBUGBUILD
  اگر(getenv("CURL_DNS_SERVER")) {
    ثابت نویسه *servers = getenv("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    اگر(status)
      بازگشت CURLE_FAILED_INIT;
  }
#endif
  بازگشت CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
ایستا CURLcode async_rr_start(ساختار Curl_easy *data,
                               ساختار Curl_resolv_async *async)
{
  ساختار async_thrdd_ctx *thrdd = &async->thrdd;
  نویسه *https_name = پوچ;
  CURLcode result = CURLE_OK;

  اگر(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    اگر(!https_name) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, پوچ);
  CURL_TRC_DNS(data, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  بازگشت result;
}
#endif

تهی Curl_async_thrdd_shutdown(ساختار Curl_easy *data,
                               ساختار Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(data, async);
}

ساختار async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

ایستا bool async_thrdd_match_item(تهی *qitem, تهی *match_data)
{
  ثابت ساختار async_thrdd_match_ctx *ctx = match_data;
  ساختار async_thrdd_item *item = qitem;
  بازگشت (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

تهی Curl_async_thrdd_destroy(ساختار Curl_easy *data,
                              ساختار Curl_resolv_async *async)
{
  (تهی)data;
  اگر(async->queries_ongoing && !async->done &&
     data->multi && data->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    ساختار async_thrdd_match_ctx mctx;
    mctx.mid = data->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(data->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#ifdef USE_HTTPSRR_ARES
  اگر(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = پوچ;
  }
#endif
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = پوچ;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = پوچ;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(ساختار Curl_easy *data, uint32_t resolv_id,
                          ساختار Curl_dns_entry **pdns)
{
  ساختار Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ساختار async_thrdd_ctx *thrdd = async ? &async->thrdd : پوچ;
  timediff_t milli, ms;
  CURLcode result = CURLE_AGAIN;

  اگر(!thrdd)
    بازگشت CURLE_FAILED_INIT;

  وقتی(result == CURLE_AGAIN) {
    وقتی(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(data->multi);
      اگر(async->done)
        بشکن;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
      اگر(ms < 3)
        milli = 0;
      وگرنه اگر(ms <= 50)
        milli = ms / 3;
      وگرنه اگر(ms <= 250)
        milli = 50;
      وگرنه
        milli = 200;
      CURL_TRC_DNS(data, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    result = Curl_async_take_result(data, async, pdns);
  }
  بازگشت result;
}

#ifdef HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
ایستا bool gai_negative(صحیح rc)
{
  انتخاب(rc) {
#ifdef EAI_NONAME
  حالت EAI_NONAME:
#endif
#اگر defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  حالت EAI_NODATA:
#endif
    بازگشت TRUE;
  پیشفرض:
    بازگشت FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
ایستا تهی async_thrdd_item_process(تهی *arg)
{
  ساختار async_thrdd_item *item = arg;
  ساختار addrinfo hints;
  نویسه service[12];
  صحیح pf = PF_INET;
  صحیح rc;

#ifdef DEBUGBUILD
  اگر(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  اگر(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    بازگشت;
  }
#endif

  memset(&hints, 0, اندازه(hints));
#ifdef CURLRES_IPV6
  اگر(item->dns_queries & CURL_DNSQ_AAAA) {
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

  curl_msnprintf(service, اندازه(service), "%u", item->port);
#ifdef AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#endif

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  اگر(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    اگر(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  وگرنه {
    Curl_addrinfo_set_port(item->res, item->port);
  }
}

#وگرنه /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
ایستا تهی async_thrdd_item_process(تهی *arg)
{
  ساختار async_thrdd_item *item = arg;

#ifdef DEBUGBUILD
  اگر(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  اگر(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    بازگشت;
  }
#endif
  item->res = Curl_ipv4_resolve_r(item->hostname, item->port);
  اگر(!item->res) {
    item->sockerr = SOCKERRNO;
    اگر(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#endif /* HAVE_GETADDRINFO */

#ifdef ENABLE_INTERNAL_WAKEUP
ایستا تهی async_thrdd_event(ثابت ساختار curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              تهی *user_data)
{
  ساختار Curl_multi *multi = user_data;
  (تهی)tqueue;
  انتخاب(ev) {
  حالت CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    بشکن;
  پیشفرض:
    بشکن;
  }
}
#وگرنه
#define async_thrdd_event   پوچ
#endif

ایستا تهی async_thrdd_item_free(تهی *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(ساختار Curl_multi *multi,
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
  اگر(!result) {
    ثابت نویسه *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        result = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#endif
  بازگشت result;
}

/* Tear down the thread queue, joining active threads or detaching them */
تهی Curl_async_thrdd_multi_destroy(ساختار Curl_multi *multi, bool join)
{
  اگر(multi->resolv_thrdq) {
#ifdef CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#endif
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = پوچ;
  }
}

#ifdef CURLVERBOSE
ایستا تهی async_thrdd_report_item(ساختار Curl_easy *data,
                                    ساختار async_thrdd_item *item)
{
  نویسه buf[MAX_IPADR_LEN];
  ساختار dynbuf tmp;
  ثابت نویسه *sep = "";
  ثابت ساختار Curl_addrinfo *ai = item->res;
  CURLcode result;
  صحیح ai_family;
#ifdef USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#وگرنه
  ai_family = AF_INET;
#endif

  اگر(!CURL_TRC_DNS_is_verbose(data))
    بازگشت;

  curlx_dyn_init(&tmp, 1024);
  برای(; ai; ai = ai->ai_next) {
    اگر(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, اندازه(buf));
      result = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      اگر(result) {
        CURL_TRC_DNS(data, "too many IP, cannot show");
        برو out;
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
تهی Curl_async_thrdd_multi_process(ساختار Curl_multi *multi)
{
  ساختار Curl_easy *data;
  تهی *qitem;

  وقتی(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    ساختار async_thrdd_item *item = qitem;
    ساختار Curl_resolv_async *async = پوچ;

    data = Curl_multi_get_easy(multi, item->mid);
    اگر(data)
      async = Curl_async_get(data, item->resolv_id);
    اگر(async) {
      ساختار async_thrdd_item **pdest = &async->thrdd.res_A;

#ifdef CURLRES_IPV6
      اگر(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#endif
      اگر(!*pdest) {
        *pdest = item;
        item = پوچ;
      }
      وگرنه
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(data);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(ساختار Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  بازگشت Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

ایستا CURLcode async_thrdd_query(ساختار Curl_easy *data,
                                  ساختار Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  ساختار async_thrdd_item *item;
  CURLcode result;

  item = async_thrdd_item_create(data, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  اگر(!item) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  CURL_TRC_DNS(data, "queueing query %s", item->description);
  result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  اگر(result)
    برو out;
  item = پوچ;
  async->queries_ongoing++;

out:
  اگر(item)
    async_thrdd_item_free(item);
  بازگشت result;
}

CURLcode Curl_async_getaddrinfo(ساختار Curl_easy *data,
                                ساختار Curl_resolv_async *async)
{
  CURLcode result = CURLE_FAILED_INIT;
  تهی *resolver = پوچ;

  اگر(async->queries_ongoing || async->done)
    بازگشت CURLE_FAILED_INIT;

#ifdef USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  اگر((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    result = async_rr_init(data, async);
    اگر(result)
      برو out;
    resolver = async->thrdd.rr.channel;
  }
#endif

  result = Curl_resolv_announce_start(data, resolver);
  اگر(result)
    بازگشت result;

#ifdef USE_HTTPSRR_ARES
  اگر(resolver) {
    result = async_rr_start(data, async);
    اگر(result)
      برو out;
  }
#endif

#ifdef CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  اگر((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    result = async_thrdd_query(data, async, CURL_DNSQ_AAAA);
    اگر(result)
      برو out;
  }
#endif
  اگر(async->dns_queries & CURL_DNSQ_A) {
    result = async_thrdd_query(data, async, CURL_DNSQ_A);
    اگر(result)
      برو out;
  }

  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  اگر(!async->queries_ongoing)
    async->done = TRUE;

  اگر(result)
    CURL_TRC_DNS(data, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (صحیح)result);
  بازگشت result;
}

CURLcode Curl_async_pollset(ساختار Curl_easy *data,
                            ساختار Curl_resolv_async *async,
                            ساختار easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(data, async);
#ifdef USE_HTTPSRR_ARES
  اگر(async->thrdd.rr.channel) {
    CURLcode result = Curl_ares_pollset(data, async->thrdd.rr.channel, ps);
    اگر(result)
      بازگشت result;
    timeout_ms = Curl_ares_timeout_ms(data, async, async->thrdd.rr.channel);
  }
#وگرنه
  (تهی)ps;
#endif

  اگر(!async->done) {
    ثابت ساختار curltime *pnow = Curl_pgrs_now(data);
#ifndef ENABLE_INTERNAL_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    اگر(elapsed_ms < 3)
      stutter_ms = 1;
    وگرنه اگر(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    وگرنه اگر(elapsed_ms <= 250)
      stutter_ms = 50;
    وگرنه
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#وگرنه
    اگر(async->queries_ongoing &&
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
  بازگشت CURLE_OK;
}

#اگر defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ایستا bool async_thrdd_item_missing_scope(ساختار Curl_easy *data,
                                           ساختار async_thrdd_item *item)
{
  ثابت ساختار Curl_addrinfo *ai;

  /* scope id already globally set */
  اگر(data->conn && data->conn->scope_id)
    بازگشت FALSE;

  برای(ai = item->res; ai; ai = ai->ai_next) {
    اگر(ai->ai_family == AF_INET6) {
      ساختار sockaddr_in6 *sa6 = (تهی *)ai->ai_addr;
      اگر(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        بازگشت TRUE;
    }
  }
  بازگشت FALSE;
}

ایستا تهی async_thrdd_item_strip_results(ساختار async_thrdd_item *item,
                                           صحیح ai_family)
{
  ساختار Curl_addrinfo *ai = item->res, **panchor = &item->res;
  وقتی(ai) {
    اگر(ai->ai_family == ai_family) {
      *panchor = ai->ai_next;
      ai->ai_next = پوچ;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    وگرنه {
      panchor = &ai->ai_next;
      ai = ai->ai_next;
    }
  }
}

#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

ایستا CURLcode async_thrdd_check_done(ساختار Curl_easy *data,
                                       ساختار Curl_resolv_async *async)
{
  ساختار async_thrdd_ctx *thrdd = &async->thrdd;

  (تهی)data;
  اگر(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(data, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = پوچ;
    thrdd->processed_A = TRUE;
  }

  اگر(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#اگر defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    اگر(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(data, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      ساختار async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode result;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = پوچ;
      item->dns_queries |= CURL_DNSQ_A;
      اگر(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = پوچ;
      }

      CURL_TRC_DNS(data, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      اگر(result) {
        async_thrdd_item_free(item);
        بازگشت result;
      }
      async->queries_ongoing++;
      بازگشت CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(data, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = پوچ;
    thrdd->processed_AAAA = TRUE;
  }

  اگر(async->queries_ongoing)
    بازگشت CURLE_AGAIN;
  async->done = TRUE;
  بازگشت CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_result(ساختار Curl_easy *data,
                                ساختار Curl_resolv_async *async,
                                ساختار Curl_dns_entry **pdns)
{
  ساختار async_thrdd_ctx *thrdd = &async->thrdd;
  ساختار Curl_dns_entry *dns = پوچ;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = پوچ;

#ifdef USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  اگر(thrdd->rr.channel)
    (تهی)Curl_ares_perform(thrdd->rr.channel, 0);
#endif
#ifndef ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(data->multi);
#endif

  result = async_thrdd_check_done(data, async);
  اگر(result)
    بازگشت result;

  Curl_expire_clear(data, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    ثابت uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    اگر(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    اگر(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    اگر(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  اگر(async->result) {
    result = async->result;
    برو out;
  }

  اگر(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    اگر(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
  }

#ifdef USE_HTTPSRR_ARES
  اگر(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(data, async->httpsrr);
    dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
    اگر(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
  }
#endif

  اگر(dns) {
    *pdns = dns;
    dns = پوچ;
  }
  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  Curl_dns_entry_unlink(data, &dns);
  Curl_async_thrdd_shutdown(data, async);
  اگر(!result && !*pdns)
    result = Curl_async_failed(data, async, پوچ);
  اگر(result &&
     (result != CURLE_COULDNT_RESOLVE_HOST) &&
     (result != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(data, "Error %d resolving %s:%d",
                 (صحیح)result, async->peer->hostname, async->peer->port);
  }
  بازگشت result;
}

#endif /* USE_RESOLV_THREADED */
