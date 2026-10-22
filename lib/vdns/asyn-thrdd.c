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
#اگر_تعریف_شده USE_RESOLV_THREADED

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_GETADDRINFO
#تعریف_کن RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#وإلا
#تعریف_کن RESOLVER_ENOMEM  SOCKENOMEM
#پایان_اگر

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

#اگر_تعریف_شده USE_ARES
#ضمن <ares.h>
#اگر_تعریف_شده USE_HTTPSRR
#تعریف_کن USE_HTTPSRR_ARES  /* the combo */
#پایان_اگر
#پایان_اگر


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
صحيح Curl_async_global_init(فراغ)
{
#إذا defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  إذا(ares_library_init(ARES_LIB_INIT_ALL)) {
    أرجع CURLE_FAILED_INIT;
  }
#پایان_اگر
  أرجع CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
فراغ Curl_async_global_cleanup(فراغ)
{
#إذا defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#پایان_اگر
}

#اگر_تعریف_شده CURLVERBOSE
#تعریف_کن CURL_ASYN_قلم_DESC_LEN   64
#تعریف_کن async_قلم_description(x)   (x)->description
#وإلا
#تعریف_کن async_قلم_description(x)   عدم
#پایان_اگر

بنية async_thrdd_قلم {
  بنية Curl_addrinfo *res;
#اگر_تعریف_شده CURLVERBOSE
  حرف description[CURL_ASYN_قلم_DESC_LEN];
#پایان_اگر
  صحيح sockerr;
  uint32_t mid;
  uint32_t resolv_id;
  uint16_t پورت;
  uint8_t transport;
  uint8_t dns_queries;
  بیت(negative); /* resolver answered that the name does not exist */
#اگر_تعریف_شده DEBUGBUILD
  uint32_t delay_ms;
  uint32_t delay_fail_ms;
  بیت(dbg_negative);
#پایان_اگر
  حرف hostname[1];
};

/* Give up reference to add_ctx */
ساكن فراغ async_thrdd_قلم_destroy(بنية async_thrdd_قلم *قلم)
{
  إذا(قلم) {
    إذا(قلم->res)
      Curl_freeaddrinfo(قلم->res);
    curlx_free(قلم);
  }
}

/* Initialize context for threaded resolver */
ساكن بنية async_thrdd_قلم *async_thrdd_قلم_create(
  بنية Curl_easy *داده,
  uint32_t resolv_id, uint8_t dns_queries,
  ثابت حرف *hostname, uint16_t پورت,
  uint8_t transport)
{
  size_t hostlen = طول_رشته(hostname);
  بنية async_thrdd_قلم *قلم;

  قلم = curlx_calloc(1, حجم(*قلم) + hostlen);
  إذا(!قلم)
    أرجع عدم;

  إذا(hostlen) /* null-terminator of name already in struct size */
    کپی_حافظه(قلم->hostname, hostname, hostlen);
  قلم->mid = داده->mid;
  قلم->resolv_id = resolv_id;
  قلم->dns_queries = dns_queries;
  قلم->پورت = پورت;
  قلم->transport = transport;

#اگر_تعریف_شده CURLVERBOSE
  curl_msnprintf(قلم->description, حجم(قلم->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 داده->id, قلم->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 قلم->hostname, قلم->پورت);
#پایان_اگر

#اگر_تعریف_شده DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_DELAY");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        قلم->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        بیعلامت حرف c = 0;
        Curl_rand_بایتها(داده, FALSE, &c, 1);
        قلم->delay_fail_ms = (uint32_t)l + c;
      }
    }
    إذا(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      قلم->dbg_negative = TRUE;
  }
#پایان_اگر

  أرجع قلم;
}

#اگر_تعریف_شده USE_HTTPSRR_ARES

ساكن فراغ async_thrdd_rr_انجامشده(فراغ *user_داده, ares_وضعیت_t وضعیت,
                                size_t timeouts,
                                ثابت ares_dns_رکورد_t *dnsrec)
{
  بنية Curl_resolv_async *async = user_داده;
  بنية async_thrdd_ctx *thrdd = async ? &async->thrdd : عدم;

  (فراغ)timeouts;
  إذا(!thrdd)
    أرجع;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->انجامشده = !async->queries_ongoing;
  إذا((ARES_SUCCESS == وضعیت) && dnsrec)
    async->نتیجه = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
ساكن CURLcode async_rr_init(بنية Curl_easy *داده,
                              بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  صحيح وضعیت;

  (فراغ)داده;
  DEBUGASSERT(!thrdd->rr.channel);
  وضعیت = ares_init_options(&thrdd->rr.channel, عدم, 0);
  إذا(وضعیت != ARES_SUCCESS) {
    thrdd->rr.channel = عدم;
    أرجع CURLE_FAILED_INIT;
  }
#اگر_تعریف_شده DEBUGBUILD
  إذا(getenv("CURL_DNS_SERVER")) {
    ثابت حرف *servers = getenv("CURL_DNS_SERVER");
    وضعیت = ares_مجموعه_servers_ports_csv(thrdd->rr.channel, servers);
    إذا(وضعیت)
      أرجع CURLE_FAILED_INIT;
  }
#پایان_اگر
  أرجع CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
ساكن CURLcode async_rr_شروع(بنية Curl_easy *داده,
                               بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  حرف *https_name = عدم;
  CURLcode نتیجه = CURLE_OK;

  إذا(async->peer->پورت != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->پورت, async->peer->hostname);
    إذا(!https_name) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_درون, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_انجامشده, async, عدم);
  CURL_TRC_DNS(داده, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

بیرون:
  curlx_free(https_name);
  أرجع نتیجه;
}
#پایان_اگر

فراغ Curl_async_thrdd_shutdown(بنية Curl_easy *داده,
                               بنية Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(داده, async);
}

بنية async_thrdd_تطابق_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

ساكن bool async_thrdd_تطابق_قلم(فراغ *qitem, فراغ *تطابق_داده)
{
  ثابت بنية async_thrdd_تطابق_ctx *ctx = تطابق_داده;
  بنية async_thrdd_قلم *قلم = qitem;
  أرجع (قلم->mid == ctx->mid) && (قلم->resolv_id == ctx->resolv_id);
}

فراغ Curl_async_thrdd_destroy(بنية Curl_easy *داده,
                              بنية Curl_resolv_async *async)
{
  (فراغ)داده;
  إذا(async->queries_ongoing && !async->انجامشده &&
     داده->multi && داده->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    بنية async_thrdd_تطابق_ctx mctx;
    mctx.mid = داده->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(داده->multi->resolv_thrdq,
                     async_thrdd_تطابق_قلم, &mctx);
  }
#اگر_تعریف_شده USE_HTTPSRR_ARES
  إذا(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = عدم;
  }
#پایان_اگر
  async_thrdd_قلم_destroy(async->thrdd.res_A);
  async->thrdd.res_A = عدم;
  async_thrdd_قلم_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = عدم;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(بنية Curl_easy *داده, uint32_t resolv_id,
                          بنية Curl_dns_مدخل **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(داده, resolv_id);
  بنية async_thrdd_ctx *thrdd = async ? &async->thrdd : عدم;
  timediff_t milli, ms;
  CURLcode نتیجه = CURLE_AGAIN;

  إذا(!thrdd)
    أرجع CURLE_FAILED_INIT;

  طالما(نتیجه == CURLE_AGAIN) {
    طالما(async->queries_ongoing && !async->انجامشده) {
      Curl_async_thrdd_multi_process(داده->multi);
      إذا(async->انجامشده)
        اكسر;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(داده), &async->شروع);
      إذا(ms < 3)
        milli = 0;
      وإلا إذا(ms <= 50)
        milli = ms / 3;
      وإلا إذا(ms <= 250)
        milli = 50;
      وإلا
        milli = 200;
      CURL_TRC_DNS(داده, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    نتیجه = Curl_async_take_نتیجه(داده, async, pdns);
  }
  أرجع نتیجه;
}

#اگر_تعریف_شده HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
ساكن bool gai_negative(صحيح rc)
{
  بدل(rc) {
#اگر_تعریف_شده EAI_NONAME
  حالة EAI_NONAME:
#پایان_اگر
#إذا defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  حالة EAI_NODATA:
#پایان_اگر
    أرجع TRUE;
  افتراضي:
    أرجع FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
ساكن فراغ async_thrdd_قلم_process(فراغ *arg)
{
  بنية async_thrdd_قلم *قلم = arg;
  بنية addrinfo hints;
  حرف service[12];
  صحيح pf = PF_INET;
  صحيح rc;

#اگر_تعریف_شده DEBUGBUILD
  إذا(قلم->delay_ms) {
    curlx_wait_ms(قلم->delay_ms);
  }
  إذا(قلم->delay_fail_ms) {
    curlx_wait_ms(قلم->delay_fail_ms);
    قلم->negative = قلم->dbg_negative;
    أرجع;
  }
#پایان_اگر

  memset(&hints, 0, حجم(hints));
#اگر_تعریف_شده CURLRES_IPV6
  إذا(قلم->dns_queries & CURL_DNSQ_AAAA) {
    pf = (قلم->dns_queries & CURL_DNSQ_A) ? PF_UNSPEC : PF_INET6;
  }
#پایان_اگر
  hints.ai_family = pf;
  hints.ai_socktype = Curl_socktype_for_transport(قلم->transport);
  hints.ai_protocol = Curl_protocol_for_transport(قلم->transport);
#اگر_تعریف_شده __APPLE__
  /* If we leave `ai_flags == 0` then macOS is looking for IPV4MAPPED
   * when doing AAAA queries. We do not want this "help". */
  hints.ai_flags = AI_ADDRCONFIG;
#پایان_اگر

  curl_msnprintf(service, حجم(service), "%u", قلم->پورت);
#اگر_تعریف_شده AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#پایان_اگر

  rc = Curl_getaddrinfo_ex(قلم->hostname, service, &hints, &قلم->res);
  إذا(rc) {
    قلم->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    إذا(قلم->sockerr == 0)
      قلم->sockerr = RESOLVER_ENOMEM;
    قلم->negative = gai_negative(rc);
  }
  وإلا {
    Curl_addrinfo_مجموعه_پورت(قلم->res, قلم->پورت);
  }
}

#وإلا /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
ساكن فراغ async_thrdd_قلم_process(فراغ *arg)
{
  بنية async_thrdd_قلم *قلم = arg;

#اگر_تعریف_شده DEBUGBUILD
  إذا(قلم->delay_ms) {
    curlx_wait_ms(قلم->delay_ms);
  }
  إذا(قلم->delay_fail_ms) {
    curlx_wait_ms(قلم->delay_fail_ms);
    قلم->negative = قلم->dbg_negative;
    أرجع;
  }
#پایان_اگر
  قلم->res = Curl_ipv4_resolve_r(قلم->hostname, قلم->پورت);
  إذا(!قلم->res) {
    قلم->sockerr = SOCKERRNO;
    إذا(قلم->sockerr == 0)
      قلم->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    قلم->negative = TRUE;
  }
}

#پایان_اگر /* HAVE_GETADDRINFO */

#اگر_تعریف_شده ENABLE_داخلی_WAKEUP
ساكن فراغ async_thrdd_event(ثابت بنية curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              فراغ *user_داده)
{
  بنية Curl_multi *multi = user_داده;
  (فراغ)tqueue;
  بدل(ev) {
  حالة CURL_THRDQ_EV_قلم_انجامشده:
    Curl_multi_wakeup_داخلی(multi);
    اكسر;
  افتراضي:
    اكسر;
  }
}
#وإلا
#تعریف_کن async_thrdd_event   عدم
#پایان_اگر

ساكن فراغ async_thrdd_قلم_free(فراغ *قلم)
{
  async_thrdd_قلم_destroy(قلم);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(بنية Curl_multi *multi,
                                     uint32_t min_نخها,
                                     uint32_t max_نخها,
                                     uint32_t idle_زمان_ms)
{
  CURLcode نتیجه;
  DEBUGASSERT(!multi->resolv_thrdq);
  نتیجه = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_نخها, max_نخها, idle_زمان_ms,
                             async_thrdd_قلم_free,
                             async_thrdd_قلم_process,
                             async_thrdd_event,
                             multi);
#اگر_تعریف_شده DEBUGBUILD
  إذا(!نتیجه) {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        نتیجه = Curl_async_thrdd_multi_مجموعه_props(
          multi, min_نخها, (uint32_t)l, idle_زمان_ms);
      }
    }
  }
#پایان_اگر
  أرجع نتیجه;
}

/* Tear down the thread queue, joining active threads or detaching them */
فراغ Curl_async_thrdd_multi_destroy(بنية Curl_multi *multi, bool join)
{
  إذا(multi->resolv_thrdq) {
#اگر_تعریف_شده CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#پایان_اگر
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = عدم;
  }
}

#اگر_تعریف_شده CURLVERBOSE
ساكن فراغ async_thrdd_report_قلم(بنية Curl_easy *داده,
                                    بنية async_thrdd_قلم *قلم)
{
  حرف buf[MAX_IPADR_LEN];
  بنية dynbuf tmp;
  ثابت حرف *sep = "";
  ثابت بنية Curl_addrinfo *ai = قلم->res;
  CURLcode نتیجه;
  صحيح ai_family;
#اگر_تعریف_شده USE_IPV6
  ai_family = (قلم->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#وإلا
  ai_family = AF_INET;
#پایان_اگر

  إذا(!CURL_TRC_DNS_is_verbose(داده))
    أرجع;

  curlx_dyn_init(&tmp, 1024);
  لكل(; ai; ai = ai->ai_بعدی) {
    إذا(ai->ai_family == ai_family) {
      Curl_printable_آدرس(ai, buf, حجم(buf));
      نتیجه = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      إذا(نتیجه) {
        CURL_TRC_DNS(داده, "too many IP, cannot show");
        اذهب بیرون;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(داده, "Host %s:%u resolved IPv%c: %s",
               قلم->hostname, قلم->پورت,
               (قلم->dns_queries & CURL_DNSQ_AAAA) ? '6' : '4',
               (curlx_dyn_len(&tmp) ? curlx_dyn_ptr(&tmp) : "(none)"));
بیرون:
  curlx_dyn_free(&tmp);
}
#پایان_اگر /* CURLVERBOSE */

/* Process the receiving end of the thread queue, dispatching
 * processed items to their transfer when it can still be found
 * and has an `async` state present. Otherwise, destroy the item. */
فراغ Curl_async_thrdd_multi_process(بنية Curl_multi *multi)
{
  بنية Curl_easy *داده;
  فراغ *qitem;

  طالما(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    بنية async_thrdd_قلم *قلم = qitem;
    بنية Curl_resolv_async *async = عدم;

    داده = Curl_multi_get_easy(multi, قلم->mid);
    إذا(داده)
      async = Curl_async_get(داده, قلم->resolv_id);
    إذا(async) {
      بنية async_thrdd_قلم **pdest = &async->thrdd.res_A;

#اگر_تعریف_شده CURLRES_IPV6
      إذا(قلم->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#پایان_اگر
      إذا(!*pdest) {
        *pdest = قلم;
        قلم = عدم;
      }
      وإلا
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(داده);
    }
    async_thrdd_قلم_free(قلم);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_مجموعه_props(بنية Curl_multi *multi,
                                          uint32_t min_نخها,
                                          uint32_t max_نخها,
                                          uint32_t idle_زمان_ms)
{
  أرجع Curl_thrdq_مجموعه_props(multi->resolv_thrdq,
                              min_نخها, max_نخها, idle_زمان_ms);
}

ساكن CURLcode async_thrdd_query(بنية Curl_easy *داده,
                                  بنية Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  بنية async_thrdd_قلم *قلم;
  CURLcode نتیجه;

  قلم = async_thrdd_قلم_create(داده, async->id, dns_queries,
                                 async->peer->hostname, async->peer->پورت,
                                 async->transport);
  إذا(!قلم) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  CURL_TRC_DNS(داده, "queueing query %s", قلم->description);
  نتیجه = Curl_thrdq_send(داده->multi->resolv_thrdq, قلم,
                           async_قلم_description(قلم), async->timeout_ms);
  إذا(نتیجه)
    اذهب بیرون;
  قلم = عدم;
  async->queries_ongoing++;

بیرون:
  إذا(قلم)
    async_thrdd_قلم_free(قلم);
  أرجع نتیجه;
}

CURLcode Curl_async_getaddrinfo(بنية Curl_easy *داده,
                                بنية Curl_resolv_async *async)
{
  CURLcode نتیجه = CURLE_FAILED_INIT;
  فراغ *resolver = عدم;

  إذا(async->queries_ongoing || async->انجامشده)
    أرجع CURLE_FAILED_INIT;

#اگر_تعریف_شده USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  إذا((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    نتیجه = async_rr_init(داده, async);
    إذا(نتیجه)
      اذهب بیرون;
    resolver = async->thrdd.rr.channel;
  }
#پایان_اگر

  نتیجه = Curl_resolv_announce_شروع(داده, resolver);
  إذا(نتیجه)
    أرجع نتیجه;

#اگر_تعریف_شده USE_HTTPSRR_ARES
  إذا(resolver) {
    نتیجه = async_rr_شروع(داده, async);
    إذا(نتیجه)
      اذهب بیرون;
  }
#پایان_اگر

#اگر_تعریف_شده CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  إذا((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    نتیجه = async_thrdd_query(داده, async, CURL_DNSQ_AAAA);
    إذا(نتیجه)
      اذهب بیرون;
  }
#پایان_اگر
  إذا(async->dns_queries & CURL_DNSQ_A) {
    نتیجه = async_thrdd_query(داده, async, CURL_DNSQ_A);
    إذا(نتیجه)
      اذهب بیرون;
  }

  CURL_THRDQ_TRACE(داده->multi->resolv_thrdq, داده);

بیرون:
  إذا(!async->queries_ongoing)
    async->انجامشده = TRUE;

  إذا(نتیجه)
    CURL_TRC_DNS(داده, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->پورت, (صحيح)نتیجه);
  أرجع نتیجه;
}

CURLcode Curl_async_pollset(بنية Curl_easy *داده,
                            بنية Curl_resolv_async *async,
                            بنية easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(داده, async);
#اگر_تعریف_شده USE_HTTPSRR_ARES
  إذا(async->thrdd.rr.channel) {
    CURLcode نتیجه = Curl_ares_pollset(داده, async->thrdd.rr.channel, ps);
    إذا(نتیجه)
      أرجع نتیجه;
    timeout_ms = Curl_ares_timeout_ms(داده, async, async->thrdd.rr.channel);
  }
#وإلا
  (فراغ)ps;
#پایان_اگر

  إذا(!async->انجامشده) {
    ثابت بنية curltime *pnow = Curl_pgrs_now(داده);
#اگر_تعریف_نشده ENABLE_داخلی_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->شروع);
    إذا(elapsed_ms < 3)
      stutter_ms = 1;
    وإلا إذا(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    وإلا إذا(elapsed_ms <= 250)
      stutter_ms = 50;
    وإلا
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#وإلا
    إذا(async->queries_ongoing &&
       !Curl_thrdq_check_started(داده->multi->resolv_thrdq)) {
      /* The queue has items but starting a worker thread to process
         them just failed again; expire soon to check once more,
         instead of sleeping on the full resolve timeout. */
      CURL_TRC_DNS(داده, "resolver thread start failed again, retrying");
      timeout_ms = CURLMIN(100, timeout_ms);
    }
#پایان_اگر
    Curl_expire_مجموعه(داده, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  أرجع CURLE_OK;
}

#إذا defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ساكن bool async_thrdd_قلم_missing_scope(بنية Curl_easy *داده,
                                           بنية async_thrdd_قلم *قلم)
{
  ثابت بنية Curl_addrinfo *ai;

  /* scope id already globally set */
  إذا(داده->conn && داده->conn->scope_id)
    أرجع FALSE;

  لكل(ai = قلم->res; ai; ai = ai->ai_بعدی) {
    إذا(ai->ai_family == AF_INET6) {
      بنية sockaddr_in6 *sa6 = (فراغ *)ai->ai_addr;
      إذا(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        أرجع TRUE;
    }
  }
  أرجع FALSE;
}

ساكن فراغ async_thrdd_قلم_strip_results(بنية async_thrdd_قلم *قلم,
                                           صحيح ai_family)
{
  بنية Curl_addrinfo *ai = قلم->res, **panchor = &قلم->res;
  طالما(ai) {
    إذا(ai->ai_family == ai_family) {
      *panchor = ai->ai_بعدی;
      ai->ai_بعدی = عدم;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    وإلا {
      panchor = &ai->ai_بعدی;
      ai = ai->ai_بعدی;
    }
  }
}

#پایان_اگر /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

ساكن CURLcode async_thrdd_check_انجامشده(بنية Curl_easy *داده,
                                       بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;

  (فراغ)داده;
  إذا(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_قلم(داده, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = عدم;
    thrdd->processed_A = TRUE;
  }

  إذا(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#إذا defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    إذا(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_قلم_missing_scope(داده, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      بنية async_thrdd_قلم *قلم = thrdd->res_AAAA;
      CURLcode نتیجه;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = عدم;
      قلم->dns_queries |= CURL_DNSQ_A;
      إذا(قلم->res) {
        Curl_freeaddrinfo(قلم->res);
        قلم->res = عدم;
      }

      CURL_TRC_DNS(داده, "re-queueing query %s for AF_UNSPEC resolve",
                   قلم->description);
      نتیجه = Curl_thrdq_send(داده->multi->resolv_thrdq, قلم,
                               async_قلم_description(قلم),
                               async->timeout_ms);
      إذا(نتیجه) {
        async_thrdd_قلم_free(قلم);
        أرجع نتیجه;
      }
      async->queries_ongoing++;
      أرجع CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_قلم_strip_results(thrdd->res_AAAA, AF_INET);
#پایان_اگر /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_قلم(داده, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = عدم;
    thrdd->processed_AAAA = TRUE;
  }

  إذا(async->queries_ongoing)
    أرجع CURLE_AGAIN;
  async->انجامشده = TRUE;
  أرجع CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_نتیجه(بنية Curl_easy *داده,
                                بنية Curl_resolv_async *async,
                                بنية Curl_dns_مدخل **pdns)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  بنية Curl_dns_مدخل *dns = عدم;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم;

#اگر_تعریف_شده USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  إذا(thrdd->rr.channel)
    (فراغ)Curl_ares_perform(thrdd->rr.channel, 0);
#پایان_اگر
#اگر_تعریف_نشده ENABLE_داخلی_WAKEUP
  Curl_async_thrdd_multi_process(داده->multi);
#پایان_اگر

  نتیجه = async_thrdd_check_انجامشده(داده, async);
  إذا(نتیجه)
    أرجع نتیجه;

  Curl_expire_clear(داده, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    ثابت uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    إذا(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    إذا(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    إذا(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  إذا(async->نتیجه) {
    نتیجه = async->نتیجه;
    اذهب بیرون;
  }

  إذا(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      داده, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    إذا(!dns) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }

#اگر_تعریف_شده USE_HTTPSRR_ARES
  إذا(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(داده, async->httpsrr);
    dns = Curl_dnsc_mk_https(داده, &async->httpsrr, async->peer);
    إذا(!dns) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }
#پایان_اگر

  إذا(dns) {
    *pdns = dns;
    dns = عدم;
  }
  CURL_THRDQ_TRACE(داده->multi->resolv_thrdq, داده);

بیرون:
  Curl_dns_مدخل_unlink(داده, &dns);
  Curl_async_thrdd_shutdown(داده, async);
  إذا(!نتیجه && !*pdns)
    نتیجه = Curl_async_failed(داده, async, عدم);
  إذا(نتیجه &&
     (نتیجه != CURLE_COULDNT_RESOLVE_HOST) &&
     (نتیجه != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(داده, "Error %d resolving %s:%d",
                 (صحيح)نتیجه, async->peer->hostname, async->peer->پورت);
  }
  أرجع نتیجه;
}

#پایان_اگر /* USE_RESOLV_THREADED */
