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
#অন্তর্ভাব "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ifdef USE_RESOLV_THREADED

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#ifdef HAVE_GETADDRINFO
#define RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#অন্যথা
#define RESOLVER_ENOMEM  SOCKENOMEM
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "curl_threads.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "thrdqueue.h"
#অন্তর্ভাব "vdns/hostip.h"
#অন্তর্ভাব "vdns/httpsrr.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curlx/wait.h"

#ifdef USE_ARES
#অন্তর্ভাব <ares.h>
#ifdef USE_HTTPSRR
#define USE_HTTPSRR_ARES  /* the combo */
#endif
#endif


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
পূর্ণসংখ্যা Curl_async_global_init(শূন্য)
{
#যদি defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  যদি(ares_library_init(ARES_LIB_INIT_ALL)) {
    ফেরত CURLE_FAILED_INIT;
  }
#endif
  ফেরত CURLE_OK;
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
#define async_item_description(x)   ফাঁকা
#endif

কাঠামো async_thrdd_item {
  কাঠামো Curl_addrinfo *res;
#ifdef CURLVERBOSE
  অক্ষর description[CURL_ASYN_ITEM_DESC_LEN];
#endif
  পূর্ণসংখ্যা sockerr;
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
  অক্ষর hostname[1];
};

/* Give up reference to add_ctx */
স্থির শূন্য async_thrdd_item_destroy(কাঠামো async_thrdd_item *item)
{
  যদি(item) {
    যদি(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
স্থির কাঠামো async_thrdd_item *async_thrdd_item_create(
  কাঠামো Curl_easy *তথ্য,
  uint32_t resolv_id, uint8_t dns_queries,
  ধ্রুবক অক্ষর *hostname, uint16_t port,
  uint8_t transport)
{
  size_t hostlen = strlen(hostname);
  কাঠামো async_thrdd_item *item;

  item = curlx_calloc(1, আকার(*item) + hostlen);
  যদি(!item)
    ফেরত ফাঁকা;

  যদি(hostlen) /* null-terminator of name already in struct size */
    memcpy(item->hostname, hostname, hostlen);
  item->mid = তথ্য->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#ifdef CURLVERBOSE
  curl_msnprintf(item->description, আকার(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 তথ্য->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#endif

#ifdef DEBUGBUILD
  {
    ধ্রুবক অক্ষর *p = getenv("CURL_DBG_RESOLV_DELAY");
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
        unsigned অক্ষর c = 0;
        Curl_rand_bytes(তথ্য, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    যদি(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#endif

  ফেরত item;
}

#ifdef USE_HTTPSRR_ARES

স্থির শূন্য async_thrdd_rr_done(শূন্য *user_তথ্য, ares_status_t status,
                                size_t timeouts,
                                ধ্রুবক ares_dns_record_t *dnsrec)
{
  কাঠামো Curl_resolv_async *async = user_তথ্য;
  কাঠামো async_thrdd_ctx *thrdd = async ? &async->thrdd : ফাঁকা;

  (শূন্য)timeouts;
  যদি(!thrdd)
    ফেরত;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  যদি((ARES_SUCCESS == status) && dnsrec)
    async->ফলাফল = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
স্থির CURLcode async_rr_init(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_resolv_async *async)
{
  কাঠামো async_thrdd_ctx *thrdd = &async->thrdd;
  পূর্ণসংখ্যা status;

  (শূন্য)তথ্য;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, ফাঁকা, 0);
  যদি(status != ARES_SUCCESS) {
    thrdd->rr.channel = ফাঁকা;
    ফেরত CURLE_FAILED_INIT;
  }
#ifdef DEBUGBUILD
  যদি(getenv("CURL_DNS_SERVER")) {
    ধ্রুবক অক্ষর *servers = getenv("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    যদি(status)
      ফেরত CURLE_FAILED_INIT;
  }
#endif
  ফেরত CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
স্থির CURLcode async_rr_start(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_resolv_async *async)
{
  কাঠামো async_thrdd_ctx *thrdd = &async->thrdd;
  অক্ষর *https_name = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  যদি(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    যদি(!https_name) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, ফাঁকা);
  CURL_TRC_DNS(তথ্য, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  ফেরত ফলাফল;
}
#endif

শূন্য Curl_async_thrdd_shutdown(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(তথ্য, async);
}

কাঠামো async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

স্থির bool async_thrdd_match_item(শূন্য *qitem, শূন্য *match_তথ্য)
{
  ধ্রুবক কাঠামো async_thrdd_match_ctx *ctx = match_তথ্য;
  কাঠামো async_thrdd_item *item = qitem;
  ফেরত (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

শূন্য Curl_async_thrdd_destroy(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_resolv_async *async)
{
  (শূন্য)তথ্য;
  যদি(async->queries_ongoing && !async->done &&
     তথ্য->multi && তথ্য->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    কাঠামো async_thrdd_match_ctx mctx;
    mctx.mid = তথ্য->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(তথ্য->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#ifdef USE_HTTPSRR_ARES
  যদি(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = ফাঁকা;
  }
#endif
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = ফাঁকা;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = ফাঁকা;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(কাঠামো Curl_easy *তথ্য, uint32_t resolv_id,
                          কাঠামো Curl_dns_entry **pdns)
{
  কাঠামো Curl_resolv_async *async = Curl_async_get(তথ্য, resolv_id);
  কাঠামো async_thrdd_ctx *thrdd = async ? &async->thrdd : ফাঁকা;
  timediff_t milli, ms;
  CURLcode ফলাফল = CURLE_AGAIN;

  যদি(!thrdd)
    ফেরত CURLE_FAILED_INIT;

  যখন(ফলাফল == CURLE_AGAIN) {
    যখন(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(তথ্য->multi);
      যদি(async->done)
        বিরতি;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &async->start);
      যদি(ms < 3)
        milli = 0;
      অন্যথা যদি(ms <= 50)
        milli = ms / 3;
      অন্যথা যদি(ms <= 250)
        milli = 50;
      অন্যথা
        milli = 200;
      CURL_TRC_DNS(তথ্য, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    ফলাফল = Curl_async_take_ফলাফল(তথ্য, async, pdns);
  }
  ফেরত ফলাফল;
}

#ifdef HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
স্থির bool gai_negative(পূর্ণসংখ্যা rc)
{
  নির্বাচন(rc) {
#ifdef EAI_NONAME
  ক্ষেত্র EAI_NONAME:
#endif
#যদি defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  ক্ষেত্র EAI_NODATA:
#endif
    ফেরত TRUE;
  পূর্বনির্ধারিত:
    ফেরত FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
স্থির শূন্য async_thrdd_item_process(শূন্য *arg)
{
  কাঠামো async_thrdd_item *item = arg;
  কাঠামো addrinfo hints;
  অক্ষর service[12];
  পূর্ণসংখ্যা pf = PF_INET;
  পূর্ণসংখ্যা rc;

#ifdef DEBUGBUILD
  যদি(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  যদি(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    ফেরত;
  }
#endif

  memset(&hints, 0, আকার(hints));
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

  curl_msnprintf(service, আকার(service), "%u", item->port);
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
  কাঠামো async_thrdd_item *item = arg;

#ifdef DEBUGBUILD
  যদি(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  যদি(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    ফেরত;
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
স্থির শূন্য async_thrdd_event(ধ্রুবক কাঠামো curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              শূন্য *user_তথ্য)
{
  কাঠামো Curl_multi *multi = user_তথ্য;
  (শূন্য)tqueue;
  নির্বাচন(ev) {
  ক্ষেত্র CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
}
#অন্যথা
#define async_thrdd_event   ফাঁকা
#endif

স্থির শূন্য async_thrdd_item_free(শূন্য *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(কাঠামো Curl_multi *multi,
                                     uint32_t min_threads,
                                     uint32_t max_threads,
                                     uint32_t idle_time_ms)
{
  CURLcode ফলাফল;
  DEBUGASSERT(!multi->resolv_thrdq);
  ফলাফল = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_threads, max_threads, idle_time_ms,
                             async_thrdd_item_free,
                             async_thrdd_item_process,
                             async_thrdd_event,
                             multi);
#ifdef DEBUGBUILD
  যদি(!ফলাফল) {
    ধ্রুবক অক্ষর *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, UINT32_MAX)) {
        ফলাফল = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#endif
  ফেরত ফলাফল;
}

/* Tear down the thread queue, joining active threads or detaching them */
শূন্য Curl_async_thrdd_multi_destroy(কাঠামো Curl_multi *multi, bool join)
{
  যদি(multi->resolv_thrdq) {
#ifdef CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#endif
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = ফাঁকা;
  }
}

#ifdef CURLVERBOSE
স্থির শূন্য async_thrdd_report_item(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো async_thrdd_item *item)
{
  অক্ষর buf[MAX_IPADR_LEN];
  কাঠামো dynbuf tmp;
  ধ্রুবক অক্ষর *sep = "";
  ধ্রুবক কাঠামো Curl_addrinfo *ai = item->res;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা ai_family;
#ifdef USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#অন্যথা
  ai_family = AF_INET;
#endif

  যদি(!CURL_TRC_DNS_is_verbose(তথ্য))
    ফেরত;

  curlx_dyn_init(&tmp, 1024);
  জন্য(; ai; ai = ai->ai_পরবর্তী) {
    যদি(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, আকার(buf));
      ফলাফল = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      যদি(ফলাফল) {
        CURL_TRC_DNS(তথ্য, "too many IP, cannot show");
        যাও out;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(তথ্য, "Host %s:%u resolved IPv%c: %s",
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
শূন্য Curl_async_thrdd_multi_process(কাঠামো Curl_multi *multi)
{
  কাঠামো Curl_easy *তথ্য;
  শূন্য *qitem;

  যখন(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    কাঠামো async_thrdd_item *item = qitem;
    কাঠামো Curl_resolv_async *async = ফাঁকা;

    তথ্য = Curl_multi_get_easy(multi, item->mid);
    যদি(তথ্য)
      async = Curl_async_get(তথ্য, item->resolv_id);
    যদি(async) {
      কাঠামো async_thrdd_item **pdest = &async->thrdd.res_A;

#ifdef CURLRES_IPV6
      যদি(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#endif
      যদি(!*pdest) {
        *pdest = item;
        item = ফাঁকা;
      }
      অন্যথা
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(তথ্য);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(কাঠামো Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  ফেরত Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

স্থির CURLcode async_thrdd_query(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  কাঠামো async_thrdd_item *item;
  CURLcode ফলাফল;

  item = async_thrdd_item_create(তথ্য, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  যদি(!item) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  CURL_TRC_DNS(তথ্য, "queueing query %s", item->description);
  ফলাফল = Curl_thrdq_send(তথ্য->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  যদি(ফলাফল)
    যাও out;
  item = ফাঁকা;
  async->queries_ongoing++;

out:
  যদি(item)
    async_thrdd_item_free(item);
  ফেরত ফলাফল;
}

CURLcode Curl_async_getaddrinfo(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_resolv_async *async)
{
  CURLcode ফলাফল = CURLE_FAILED_INIT;
  শূন্য *resolver = ফাঁকা;

  যদি(async->queries_ongoing || async->done)
    ফেরত CURLE_FAILED_INIT;

#ifdef USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  যদি((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    ফলাফল = async_rr_init(তথ্য, async);
    যদি(ফলাফল)
      যাও out;
    resolver = async->thrdd.rr.channel;
  }
#endif

  ফলাফল = Curl_resolv_announce_start(তথ্য, resolver);
  যদি(ফলাফল)
    ফেরত ফলাফল;

#ifdef USE_HTTPSRR_ARES
  যদি(resolver) {
    ফলাফল = async_rr_start(তথ্য, async);
    যদি(ফলাফল)
      যাও out;
  }
#endif

#ifdef CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  যদি((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    ফলাফল = async_thrdd_query(তথ্য, async, CURL_DNSQ_AAAA);
    যদি(ফলাফল)
      যাও out;
  }
#endif
  যদি(async->dns_queries & CURL_DNSQ_A) {
    ফলাফল = async_thrdd_query(তথ্য, async, CURL_DNSQ_A);
    যদি(ফলাফল)
      যাও out;
  }

  CURL_THRDQ_TRACE(তথ্য->multi->resolv_thrdq, তথ্য);

out:
  যদি(!async->queries_ongoing)
    async->done = TRUE;

  যদি(ফলাফল)
    CURL_TRC_DNS(তথ্য, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

CURLcode Curl_async_pollset(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_resolv_async *async,
                            কাঠামো easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(তথ্য, async);
#ifdef USE_HTTPSRR_ARES
  যদি(async->thrdd.rr.channel) {
    CURLcode ফলাফল = Curl_ares_pollset(তথ্য, async->thrdd.rr.channel, ps);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    timeout_ms = Curl_ares_timeout_ms(তথ্য, async, async->thrdd.rr.channel);
  }
#অন্যথা
  (শূন্য)ps;
#endif

  যদি(!async->done) {
    ধ্রুবক কাঠামো curltime *pnow = Curl_pgrs_now(তথ্য);
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
       !Curl_thrdq_check_started(তথ্য->multi->resolv_thrdq)) {
      /* The queue has items but starting a worker thread to process
         them just failed again; expire soon to check once more,
         instead of sleeping on the full resolve timeout. */
      CURL_TRC_DNS(তথ্য, "resolver thread start failed again, retrying");
      timeout_ms = CURLMIN(100, timeout_ms);
    }
#endif
    Curl_expire_set(তথ্য, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  ফেরত CURLE_OK;
}

#যদি defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
স্থির bool async_thrdd_item_missing_scope(কাঠামো Curl_easy *তথ্য,
                                           কাঠামো async_thrdd_item *item)
{
  ধ্রুবক কাঠামো Curl_addrinfo *ai;

  /* scope id already globally set */
  যদি(তথ্য->conn && তথ্য->conn->scope_id)
    ফেরত FALSE;

  জন্য(ai = item->res; ai; ai = ai->ai_পরবর্তী) {
    যদি(ai->ai_family == AF_INET6) {
      কাঠামো sockaddr_in6 *sa6 = (শূন্য *)ai->ai_addr;
      যদি(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        ফেরত TRUE;
    }
  }
  ফেরত FALSE;
}

স্থির শূন্য async_thrdd_item_strip_results(কাঠামো async_thrdd_item *item,
                                           পূর্ণসংখ্যা ai_family)
{
  কাঠামো Curl_addrinfo *ai = item->res, **panchor = &item->res;
  যখন(ai) {
    যদি(ai->ai_family == ai_family) {
      *panchor = ai->ai_পরবর্তী;
      ai->ai_পরবর্তী = ফাঁকা;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    অন্যথা {
      panchor = &ai->ai_পরবর্তী;
      ai = ai->ai_পরবর্তী;
    }
  }
}

#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

স্থির CURLcode async_thrdd_check_done(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো Curl_resolv_async *async)
{
  কাঠামো async_thrdd_ctx *thrdd = &async->thrdd;

  (শূন্য)তথ্য;
  যদি(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(তথ্য, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = ফাঁকা;
    thrdd->processed_A = TRUE;
  }

  যদি(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#যদি defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    যদি(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(তথ্য, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      কাঠামো async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode ফলাফল;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = ফাঁকা;
      item->dns_queries |= CURL_DNSQ_A;
      যদি(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = ফাঁকা;
      }

      CURL_TRC_DNS(তথ্য, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      ফলাফল = Curl_thrdq_send(তথ্য->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      যদি(ফলাফল) {
        async_thrdd_item_free(item);
        ফেরত ফলাফল;
      }
      async->queries_ongoing++;
      ফেরত CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(তথ্য, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = ফাঁকা;
    thrdd->processed_AAAA = TRUE;
  }

  যদি(async->queries_ongoing)
    ফেরত CURLE_AGAIN;
  async->done = TRUE;
  ফেরত CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_ফলাফল(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_resolv_async *async,
                                কাঠামো Curl_dns_entry **pdns)
{
  কাঠামো async_thrdd_ctx *thrdd = &async->thrdd;
  কাঠামো Curl_dns_entry *dns = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ফাঁকা;

#ifdef USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  যদি(thrdd->rr.channel)
    (শূন্য)Curl_ares_perform(thrdd->rr.channel, 0);
#endif
#ifndef ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(তথ্য->multi);
#endif

  ফলাফল = async_thrdd_check_done(তথ্য, async);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  Curl_expire_clear(তথ্য, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    ধ্রুবক uint8_t ip_queries =
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

  যদি(async->ফলাফল) {
    ফলাফল = async->ফলাফল;
    যাও out;
  }

  যদি(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      তথ্য, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    যদি(!dns) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }

#ifdef USE_HTTPSRR_ARES
  যদি(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(তথ্য, async->httpsrr);
    dns = Curl_dnsc_mk_https(তথ্য, &async->httpsrr, async->peer);
    যদি(!dns) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }
#endif

  যদি(dns) {
    *pdns = dns;
    dns = ফাঁকা;
  }
  CURL_THRDQ_TRACE(তথ্য->multi->resolv_thrdq, তথ্য);

out:
  Curl_dns_entry_unlink(তথ্য, &dns);
  Curl_async_thrdd_shutdown(তথ্য, async);
  যদি(!ফলাফল && !*pdns)
    ফলাফল = Curl_async_failed(তথ্য, async, ফাঁকা);
  যদি(ফলাফল &&
     (ফলাফল != CURLE_COULDNT_RESOLVE_HOST) &&
     (ফলাফল != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(তথ্য, "Error %d resolving %s:%d",
                 (পূর্ণসংখ্যা)ফলাফল, async->peer->hostname, async->peer->port);
  }
  ফেরত ফলাফল;
}

#endif /* USE_RESOLV_THREADED */
