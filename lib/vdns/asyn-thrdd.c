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
#অন্তর্ভাৱ "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ifdef USE_RESOLV_THREADED

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef __VMS
#অন্তর্ভাৱ <in.h>
#অন্তর্ভাৱ <inet.h>
#endif

#ifdef HAVE_GETADDRINFO
#define RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#অন্যথা
#define RESOLVER_ENOMEM  SOCKENOMEM
#endif

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "curl_addrinfo.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "curl_threads.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "rand.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "thrdqueue.h"
#অন্তর্ভাৱ "vdns/hostip.h"
#অন্তর্ভাৱ "vdns/httpsrr.h"
#অন্তর্ভাৱ "curlx/strparse.h"
#অন্তর্ভাৱ "curlx/wait.h"

#ifdef USE_ARES
#অন্তর্ভাৱ <ares.h>
#ifdef USE_HTTPSRR
#define USE_HTTPSRR_ARES  /* the combo */
#endif
#endif


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
পূর্ণাঙ্ক Curl_async_global_init(শূন্য)
{
#যদি defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  যদি(ares_library_init(ARES_LIB_INIT_ALL)) {
    ঘূৰাই CURLE_FAILED_INIT;
  }
#endif
  ঘূৰাই CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
শূন্য Curl_async_global_cleanup(শূন্য)
{
#যদি defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#endif
}

#ifdef CURLVERBOSE
#define CURL_ASYN_ITEM_DESC_LEN   64
#define async_item_description(x)   (x)->description
#অন্যথা
#define async_item_description(x)   রিক্ত
#endif

সংরচনা async_thrdd_item {
  সংরচনা Curl_addrinfo *res;
#ifdef CURLVERBOSE
  ৱর্ণ description[CURL_ASYN_ITEM_DESC_LEN];
#endif
  পূর্ণাঙ্ক sockerr;
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
  ৱর্ণ hostname[1];
};

/* Give up reference to add_ctx */
স্থির শূন্য async_thrdd_item_destroy(সংরচনা async_thrdd_item *item)
{
  যদি(item) {
    যদি(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
স্থির সংরচনা async_thrdd_item *async_thrdd_item_create(
  সংরচনা Curl_easy *data,
  uint32_t resolv_id, uint8_t dns_queries,
  ধ্রুৱ ৱর্ণ *hostname, uint16_t port,
  uint8_t transport)
{
  size_t hostlen = strlen(hostname);
  সংরচনা async_thrdd_item *item;

  item = curlx_calloc(1, পরিমাণ(*item) + hostlen);
  যদি(!item)
    ঘূৰাই রিক্ত;

  যদি(hostlen) /* null-terminator of name already in struct size */
    memcpy(item->hostname, hostname, hostlen);
  item->mid = data->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#ifdef CURLVERBOSE
  curl_msnprintf(item->description, পরিমাণ(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 data->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#endif

#ifdef DEBUGBUILD
  {
    ধ্রুৱ ৱর্ণ *p = getenv("CURL_DBG_RESOLV_DELAY");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, UINT32_MAX)) {
        unsigned ৱর্ণ c = 0;
        Curl_rand_bytes(data, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    যদি(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#endif

  ঘূৰাই item;
}

#ifdef USE_HTTPSRR_ARES

স্থির শূন্য async_thrdd_rr_done(শূন্য *user_data, ares_status_t status,
                                size_t timeouts,
                                ধ্রুৱ ares_dns_record_t *dnsrec)
{
  সংরচনা Curl_resolv_async *async = user_data;
  সংরচনা async_thrdd_ctx *thrdd = async ? &async->thrdd : রিক্ত;

  (শূন্য)timeouts;
  যদি(!thrdd)
    ঘূৰাই;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  যদি((ARES_SUCCESS == status) && dnsrec)
    async->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
স্থির CURLcode async_rr_init(সংরচনা Curl_easy *data,
                              সংরচনা Curl_resolv_async *async)
{
  সংরচনা async_thrdd_ctx *thrdd = &async->thrdd;
  পূর্ণাঙ্ক status;

  (শূন্য)data;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, রিক্ত, 0);
  যদি(status != ARES_SUCCESS) {
    thrdd->rr.channel = রিক্ত;
    ঘূৰাই CURLE_FAILED_INIT;
  }
#ifdef DEBUGBUILD
  যদি(getenv("CURL_DNS_SERVER")) {
    ধ্রুৱ ৱর্ণ *servers = getenv("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    যদি(status)
      ঘূৰাই CURLE_FAILED_INIT;
  }
#endif
  ঘূৰাই CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
স্থির CURLcode async_rr_start(সংরচনা Curl_easy *data,
                               সংরচনা Curl_resolv_async *async)
{
  সংরচনা async_thrdd_ctx *thrdd = &async->thrdd;
  ৱর্ণ *https_name = রিক্ত;
  CURLcode result = CURLE_OK;

  যদি(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    যদি(!https_name) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, রিক্ত);
  CURL_TRC_DNS(data, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  ঘূৰাই result;
}
#endif

শূন্য Curl_async_thrdd_shutdown(সংরচনা Curl_easy *data,
                               সংরচনা Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(data, async);
}

সংরচনা async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

স্থির bool async_thrdd_match_item(শূন্য *qitem, শূন্য *match_data)
{
  ধ্রুৱ সংরচনা async_thrdd_match_ctx *ctx = match_data;
  সংরচনা async_thrdd_item *item = qitem;
  ঘূৰাই (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

শূন্য Curl_async_thrdd_destroy(সংরচনা Curl_easy *data,
                              সংরচনা Curl_resolv_async *async)
{
  (শূন্য)data;
  যদি(async->queries_ongoing && !async->done &&
     data->multi && data->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    সংরচনা async_thrdd_match_ctx mctx;
    mctx.mid = data->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(data->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#ifdef USE_HTTPSRR_ARES
  যদি(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = রিক্ত;
  }
#endif
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = রিক্ত;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = রিক্ত;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(সংরচনা Curl_easy *data, uint32_t resolv_id,
                          সংরচনা Curl_dns_entry **pdns)
{
  সংরচনা Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  সংরচনা async_thrdd_ctx *thrdd = async ? &async->thrdd : রিক্ত;
  timediff_t milli, ms;
  CURLcode result = CURLE_AGAIN;

  যদি(!thrdd)
    ঘূৰাই CURLE_FAILED_INIT;

  যেতিয়ালৈ(result == CURLE_AGAIN) {
    যেতিয়ালৈ(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(data->multi);
      যদি(async->done)
        ভঙ্গ;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
      যদি(ms < 3)
        milli = 0;
      অন্যথা যদি(ms <= 50)
        milli = ms / 3;
      অন্যথা যদি(ms <= 250)
        milli = 50;
      অন্যথা
        milli = 200;
      CURL_TRC_DNS(data, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    result = Curl_async_take_result(data, async, pdns);
  }
  ঘূৰাই result;
}

#ifdef HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
স্থির bool gai_negative(পূর্ণাঙ্ক rc)
{
  ৱিকল্প(rc) {
#ifdef EAI_NONAME
  পক্ষ EAI_NONAME:
#endif
#যদি defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  পক্ষ EAI_NODATA:
#endif
    ঘূৰাই TRUE;
  স্ৱভাৱ:
    ঘূৰাই FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
স্থির শূন্য async_thrdd_item_process(শূন্য *arg)
{
  সংরচনা async_thrdd_item *item = arg;
  সংরচনা addrinfo hints;
  ৱর্ণ service[12];
  পূর্ণাঙ্ক pf = PF_INET;
  পূর্ণাঙ্ক rc;

#ifdef DEBUGBUILD
  যদি(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  যদি(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    ঘূৰাই;
  }
#endif

  memset(&hints, 0, পরিমাণ(hints));
#ifdef CURLRES_IPV6
  যদি(item->dns_queries & CURL_DNSQ_AAAA) {
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

  curl_msnprintf(service, পরিমাণ(service), "%u", item->port);
#ifdef AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#endif

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  যদি(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    যদি(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  অন্যথা {
    Curl_addrinfo_set_port(item->res, item->port);
  }
}

#অন্যথা /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
স্থির শূন্য async_thrdd_item_process(শূন্য *arg)
{
  সংরচনা async_thrdd_item *item = arg;

#ifdef DEBUGBUILD
  যদি(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  যদি(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    ঘূৰাই;
  }
#endif
  item->res = Curl_ipv4_resolve_r(item->hostname, item->port);
  যদি(!item->res) {
    item->sockerr = SOCKERRNO;
    যদি(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#endif /* HAVE_GETADDRINFO */

#ifdef ENABLE_INTERNAL_WAKEUP
স্থির শূন্য async_thrdd_event(ধ্রুৱ সংরচনা curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              শূন্য *user_data)
{
  সংরচনা Curl_multi *multi = user_data;
  (শূন্য)tqueue;
  ৱিকল্প(ev) {
  পক্ষ CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    ভঙ্গ;
  স্ৱভাৱ:
    ভঙ্গ;
  }
}
#অন্যথা
#define async_thrdd_event   রিক্ত
#endif

স্থির শূন্য async_thrdd_item_free(শূন্য *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(সংরচনা Curl_multi *multi,
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
  যদি(!result) {
    ধ্রুৱ ৱর্ণ *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, UINT32_MAX)) {
        result = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#endif
  ঘূৰাই result;
}

/* Tear down the thread queue, joining active threads or detaching them */
শূন্য Curl_async_thrdd_multi_destroy(সংরচনা Curl_multi *multi, bool join)
{
  যদি(multi->resolv_thrdq) {
#ifdef CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#endif
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = রিক্ত;
  }
}

#ifdef CURLVERBOSE
স্থির শূন্য async_thrdd_report_item(সংরচনা Curl_easy *data,
                                    সংরচনা async_thrdd_item *item)
{
  ৱর্ণ buf[MAX_IPADR_LEN];
  সংরচনা dynbuf tmp;
  ধ্রুৱ ৱর্ণ *sep = "";
  ধ্রুৱ সংরচনা Curl_addrinfo *ai = item->res;
  CURLcode result;
  পূর্ণাঙ্ক ai_family;
#ifdef USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#অন্যথা
  ai_family = AF_INET;
#endif

  যদি(!CURL_TRC_DNS_is_verbose(data))
    ঘূৰাই;

  curlx_dyn_init(&tmp, 1024);
  বাবে(; ai; ai = ai->ai_next) {
    যদি(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, পরিমাণ(buf));
      result = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      যদি(result) {
        CURL_TRC_DNS(data, "too many IP, cannot show");
        গচ্ছ out;
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
শূন্য Curl_async_thrdd_multi_process(সংরচনা Curl_multi *multi)
{
  সংরচনা Curl_easy *data;
  শূন্য *qitem;

  যেতিয়ালৈ(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    সংরচনা async_thrdd_item *item = qitem;
    সংরচনা Curl_resolv_async *async = রিক্ত;

    data = Curl_multi_get_easy(multi, item->mid);
    যদি(data)
      async = Curl_async_get(data, item->resolv_id);
    যদি(async) {
      সংরচনা async_thrdd_item **pdest = &async->thrdd.res_A;

#ifdef CURLRES_IPV6
      যদি(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#endif
      যদি(!*pdest) {
        *pdest = item;
        item = রিক্ত;
      }
      অন্যথা
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(data);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(সংরচনা Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  ঘূৰাই Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

স্থির CURLcode async_thrdd_query(সংরচনা Curl_easy *data,
                                  সংরচনা Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  সংরচনা async_thrdd_item *item;
  CURLcode result;

  item = async_thrdd_item_create(data, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  যদি(!item) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  CURL_TRC_DNS(data, "queueing query %s", item->description);
  result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  যদি(result)
    গচ্ছ out;
  item = রিক্ত;
  async->queries_ongoing++;

out:
  যদি(item)
    async_thrdd_item_free(item);
  ঘূৰাই result;
}

CURLcode Curl_async_getaddrinfo(সংরচনা Curl_easy *data,
                                সংরচনা Curl_resolv_async *async)
{
  CURLcode result = CURLE_FAILED_INIT;
  শূন্য *resolver = রিক্ত;

  যদি(async->queries_ongoing || async->done)
    ঘূৰাই CURLE_FAILED_INIT;

#ifdef USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  যদি((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    result = async_rr_init(data, async);
    যদি(result)
      গচ্ছ out;
    resolver = async->thrdd.rr.channel;
  }
#endif

  result = Curl_resolv_announce_start(data, resolver);
  যদি(result)
    ঘূৰাই result;

#ifdef USE_HTTPSRR_ARES
  যদি(resolver) {
    result = async_rr_start(data, async);
    যদি(result)
      গচ্ছ out;
  }
#endif

#ifdef CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  যদি((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    result = async_thrdd_query(data, async, CURL_DNSQ_AAAA);
    যদি(result)
      গচ্ছ out;
  }
#endif
  যদি(async->dns_queries & CURL_DNSQ_A) {
    result = async_thrdd_query(data, async, CURL_DNSQ_A);
    যদি(result)
      গচ্ছ out;
  }

  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  যদি(!async->queries_ongoing)
    async->done = TRUE;

  যদি(result)
    CURL_TRC_DNS(data, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

CURLcode Curl_async_pollset(সংরচনা Curl_easy *data,
                            সংরচনা Curl_resolv_async *async,
                            সংরচনা easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(data, async);
#ifdef USE_HTTPSRR_ARES
  যদি(async->thrdd.rr.channel) {
    CURLcode result = Curl_ares_pollset(data, async->thrdd.rr.channel, ps);
    যদি(result)
      ঘূৰাই result;
    timeout_ms = Curl_ares_timeout_ms(data, async, async->thrdd.rr.channel);
  }
#অন্যথা
  (শূন্য)ps;
#endif

  যদি(!async->done) {
    ধ্রুৱ সংরচনা curltime *pnow = Curl_pgrs_now(data);
#ifndef ENABLE_INTERNAL_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    যদি(elapsed_ms < 3)
      stutter_ms = 1;
    অন্যথা যদি(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    অন্যথা যদি(elapsed_ms <= 250)
      stutter_ms = 50;
    অন্যথা
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#অন্যথা
    যদি(async->queries_ongoing &&
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
  ঘূৰাই CURLE_OK;
}

#যদি defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
স্থির bool async_thrdd_item_missing_scope(সংরচনা Curl_easy *data,
                                           সংরচনা async_thrdd_item *item)
{
  ধ্রুৱ সংরচনা Curl_addrinfo *ai;

  /* scope id already globally set */
  যদি(data->conn && data->conn->scope_id)
    ঘূৰাই FALSE;

  বাবে(ai = item->res; ai; ai = ai->ai_next) {
    যদি(ai->ai_family == AF_INET6) {
      সংরচনা sockaddr_in6 *sa6 = (শূন্য *)ai->ai_addr;
      যদি(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        ঘূৰাই TRUE;
    }
  }
  ঘূৰাই FALSE;
}

স্থির শূন্য async_thrdd_item_strip_results(সংরচনা async_thrdd_item *item,
                                           পূর্ণাঙ্ক ai_family)
{
  সংরচনা Curl_addrinfo *ai = item->res, **panchor = &item->res;
  যেতিয়ালৈ(ai) {
    যদি(ai->ai_family == ai_family) {
      *panchor = ai->ai_next;
      ai->ai_next = রিক্ত;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    অন্যথা {
      panchor = &ai->ai_next;
      ai = ai->ai_next;
    }
  }
}

#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

স্থির CURLcode async_thrdd_check_done(সংরচনা Curl_easy *data,
                                       সংরচনা Curl_resolv_async *async)
{
  সংরচনা async_thrdd_ctx *thrdd = &async->thrdd;

  (শূন্য)data;
  যদি(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(data, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = রিক্ত;
    thrdd->processed_A = TRUE;
  }

  যদি(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#যদি defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    যদি(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(data, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      সংরচনা async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode result;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = রিক্ত;
      item->dns_queries |= CURL_DNSQ_A;
      যদি(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = রিক্ত;
      }

      CURL_TRC_DNS(data, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      যদি(result) {
        async_thrdd_item_free(item);
        ঘূৰাই result;
      }
      async->queries_ongoing++;
      ঘূৰাই CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(data, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = রিক্ত;
    thrdd->processed_AAAA = TRUE;
  }

  যদি(async->queries_ongoing)
    ঘূৰাই CURLE_AGAIN;
  async->done = TRUE;
  ঘূৰাই CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_result(সংরচনা Curl_easy *data,
                                সংরচনা Curl_resolv_async *async,
                                সংরচনা Curl_dns_entry **pdns)
{
  সংরচনা async_thrdd_ctx *thrdd = &async->thrdd;
  সংরচনা Curl_dns_entry *dns = রিক্ত;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = রিক্ত;

#ifdef USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  যদি(thrdd->rr.channel)
    (শূন্য)Curl_ares_perform(thrdd->rr.channel, 0);
#endif
#ifndef ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(data->multi);
#endif

  result = async_thrdd_check_done(data, async);
  যদি(result)
    ঘূৰাই result;

  Curl_expire_clear(data, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    ধ্রুৱ uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    যদি(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    যদি(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    যদি(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  যদি(async->result) {
    result = async->result;
    গচ্ছ out;
  }

  যদি(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    যদি(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
  }

#ifdef USE_HTTPSRR_ARES
  যদি(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(data, async->httpsrr);
    dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
    যদি(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
  }
#endif

  যদি(dns) {
    *pdns = dns;
    dns = রিক্ত;
  }
  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  Curl_dns_entry_unlink(data, &dns);
  Curl_async_thrdd_shutdown(data, async);
  যদি(!result && !*pdns)
    result = Curl_async_failed(data, async, রিক্ত);
  যদি(result &&
     (result != CURLE_COULDNT_RESOLVE_HOST) &&
     (result != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(data, "Error %d resolving %s:%d",
                 (পূর্ণাঙ্ক)result, async->peer->hostname, async->peer->port);
  }
  ঘূৰাই result;
}

#endif /* USE_RESOLV_THREADED */
