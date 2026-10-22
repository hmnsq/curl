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
#که_تعریف_شوی USE_RESOLV_THREADED

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای
#که_تعریف_شوی __VMS
#ضمن <in.h>
#ضمن <inet.h>
#د_که_پای

#که_تعریف_شوی HAVE_GETADDRINFO
#تعریف_کړه RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#وإلا
#تعریف_کړه RESOLVER_ENOMEM  SOCKENOMEM
#د_که_پای

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

#که_تعریف_شوی USE_ARES
#ضمن <ares.h>
#که_تعریف_شوی USE_HTTPSRR
#تعریف_کړه USE_HTTPSRR_ARES  /* the combo */
#د_که_پای
#د_که_پای


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
#د_که_پای
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
#د_که_پای
}

#که_تعریف_شوی CURLVERBOSE
#تعریف_کړه CURL_ASYN_توکی_DESC_LEN   64
#تعریف_کړه async_توکی_description(x)   (x)->description
#وإلا
#تعریف_کړه async_توکی_description(x)   عدم
#د_که_پای

بنية async_thrdd_توکی {
  بنية Curl_addrinfo *res;
#که_تعریف_شوی CURLVERBOSE
  حرف description[CURL_ASYN_توکی_DESC_LEN];
#د_که_پای
  صحيح sockerr;
  uint32_t mid;
  uint32_t resolv_id;
  uint16_t پورټ;
  uint8_t transport;
  uint8_t dns_queries;
  بټ(negative); /* resolver answered that the name does not exist */
#که_تعریف_شوی DEBUGBUILD
  uint32_t delay_ms;
  uint32_t delay_fail_ms;
  بټ(dbg_negative);
#د_که_پای
  حرف hostname[1];
};

/* Give up reference to add_ctx */
ساكن فراغ async_thrdd_توکی_destroy(بنية async_thrdd_توکی *توکی)
{
  إذا(توکی) {
    إذا(توکی->res)
      Curl_freeaddrinfo(توکی->res);
    curlx_free(توکی);
  }
}

/* Initialize context for threaded resolver */
ساكن بنية async_thrdd_توکی *async_thrdd_توکی_create(
  بنية Curl_easy *معلومات,
  uint32_t resolv_id, uint8_t dns_queries,
  ثابت حرف *hostname, uint16_t پورټ,
  uint8_t transport)
{
  size_t hostlen = مزي_اوږدوالی(hostname);
  بنية async_thrdd_توکی *توکی;

  توکی = curlx_calloc(1, حجم(*توکی) + hostlen);
  إذا(!توکی)
    أرجع عدم;

  إذا(hostlen) /* null-terminator of name already in struct size */
    حافظه_نقل(توکی->hostname, hostname, hostlen);
  توکی->mid = معلومات->mid;
  توکی->resolv_id = resolv_id;
  توکی->dns_queries = dns_queries;
  توکی->پورټ = پورټ;
  توکی->transport = transport;

#که_تعریف_شوی CURLVERBOSE
  curl_msnprintf(توکی->description, حجم(توکی->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 معلومات->id, توکی->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 توکی->hostname, توکی->پورټ);
#د_که_پای

#که_تعریف_شوی DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_DELAY");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_شمېره(&p, &l, UINT32_MAX)) {
        توکی->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_شمېره(&p, &l, UINT32_MAX)) {
        بینښه حرف c = 0;
        Curl_rand_بایټونه(معلومات, FALSE, &c, 1);
        توکی->delay_fail_ms = (uint32_t)l + c;
      }
    }
    إذا(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      توکی->dbg_negative = TRUE;
  }
#د_که_پای

  أرجع توکی;
}

#که_تعریف_شوی USE_HTTPSRR_ARES

ساكن فراغ async_thrdd_rr_بشپړ(فراغ *user_معلومات, ares_حالت_t حالت,
                                size_t timeouts,
                                ثابت ares_dns_ریکارډ_t *dnsrec)
{
  بنية Curl_resolv_async *async = user_معلومات;
  بنية async_thrdd_ctx *thrdd = async ? &async->thrdd : عدم;

  (فراغ)timeouts;
  إذا(!thrdd)
    أرجع;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->بشپړ = !async->queries_ongoing;
  إذا((ARES_SUCCESS == حالت) && dnsrec)
    async->پایله = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
ساكن CURLcode async_rr_init(بنية Curl_easy *معلومات,
                              بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  صحيح حالت;

  (فراغ)معلومات;
  DEBUGASSERT(!thrdd->rr.channel);
  حالت = ares_init_options(&thrdd->rr.channel, عدم, 0);
  إذا(حالت != ARES_SUCCESS) {
    thrdd->rr.channel = عدم;
    أرجع CURLE_FAILED_INIT;
  }
#که_تعریف_شوی DEBUGBUILD
  إذا(getenv("CURL_DNS_SERVER")) {
    ثابت حرف *servers = getenv("CURL_DNS_SERVER");
    حالت = ares_سېټ_servers_ports_csv(thrdd->rr.channel, servers);
    إذا(حالت)
      أرجع CURLE_FAILED_INIT;
  }
#د_که_پای
  أرجع CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
ساكن CURLcode async_rr_پیل(بنية Curl_easy *معلومات,
                               بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  حرف *https_name = عدم;
  CURLcode پایله = CURLE_OK;

  إذا(async->peer->پورټ != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->پورټ, async->peer->hostname);
    إذا(!https_name) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب بهر;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_دننه, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_بشپړ, async, عدم);
  CURL_TRC_DNS(معلومات, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

بهر:
  curlx_free(https_name);
  أرجع پایله;
}
#د_که_پای

فراغ Curl_async_thrdd_shutdown(بنية Curl_easy *معلومات,
                               بنية Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(معلومات, async);
}

بنية async_thrdd_سمون_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

ساكن bool async_thrdd_سمون_توکی(فراغ *qitem, فراغ *سمون_معلومات)
{
  ثابت بنية async_thrdd_سمون_ctx *ctx = سمون_معلومات;
  بنية async_thrdd_توکی *توکی = qitem;
  أرجع (توکی->mid == ctx->mid) && (توکی->resolv_id == ctx->resolv_id);
}

فراغ Curl_async_thrdd_destroy(بنية Curl_easy *معلومات,
                              بنية Curl_resolv_async *async)
{
  (فراغ)معلومات;
  إذا(async->queries_ongoing && !async->بشپړ &&
     معلومات->multi && معلومات->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    بنية async_thrdd_سمون_ctx mctx;
    mctx.mid = معلومات->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(معلومات->multi->resolv_thrdq,
                     async_thrdd_سمون_توکی, &mctx);
  }
#که_تعریف_شوی USE_HTTPSRR_ARES
  إذا(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = عدم;
  }
#د_که_پای
  async_thrdd_توکی_destroy(async->thrdd.res_A);
  async->thrdd.res_A = عدم;
  async_thrdd_توکی_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = عدم;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(بنية Curl_easy *معلومات, uint32_t resolv_id,
                          بنية Curl_dns_ثبت **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(معلومات, resolv_id);
  بنية async_thrdd_ctx *thrdd = async ? &async->thrdd : عدم;
  timediff_t milli, ms;
  CURLcode پایله = CURLE_AGAIN;

  إذا(!thrdd)
    أرجع CURLE_FAILED_INIT;

  طالما(پایله == CURLE_AGAIN) {
    طالما(async->queries_ongoing && !async->بشپړ) {
      Curl_async_thrdd_multi_process(معلومات->multi);
      إذا(async->بشپړ)
        اكسر;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &async->پیل);
      إذا(ms < 3)
        milli = 0;
      وإلا إذا(ms <= 50)
        milli = ms / 3;
      وإلا إذا(ms <= 250)
        milli = 50;
      وإلا
        milli = 200;
      CURL_TRC_DNS(معلومات, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    پایله = Curl_async_take_پایله(معلومات, async, pdns);
  }
  أرجع پایله;
}

#که_تعریف_شوی HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
ساكن bool gai_negative(صحيح rc)
{
  بدل(rc) {
#که_تعریف_شوی EAI_NONAME
  حالة EAI_NONAME:
#د_که_پای
#إذا defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  حالة EAI_NODATA:
#د_که_پای
    أرجع TRUE;
  افتراضي:
    أرجع FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
ساكن فراغ async_thrdd_توکی_process(فراغ *arg)
{
  بنية async_thrdd_توکی *توکی = arg;
  بنية addrinfo hints;
  حرف service[12];
  صحيح pf = PF_INET;
  صحيح rc;

#که_تعریف_شوی DEBUGBUILD
  إذا(توکی->delay_ms) {
    curlx_wait_ms(توکی->delay_ms);
  }
  إذا(توکی->delay_fail_ms) {
    curlx_wait_ms(توکی->delay_fail_ms);
    توکی->negative = توکی->dbg_negative;
    أرجع;
  }
#د_که_پای

  memset(&hints, 0, حجم(hints));
#که_تعریف_شوی CURLRES_IPV6
  إذا(توکی->dns_queries & CURL_DNSQ_AAAA) {
    pf = (توکی->dns_queries & CURL_DNSQ_A) ? PF_UNSPEC : PF_INET6;
  }
#د_که_پای
  hints.ai_family = pf;
  hints.ai_socktype = Curl_socktype_for_transport(توکی->transport);
  hints.ai_protocol = Curl_protocol_for_transport(توکی->transport);
#که_تعریف_شوی __APPLE__
  /* If we leave `ai_flags == 0` then macOS is looking for IPV4MAPPED
   * when doing AAAA queries. We do not want this "help". */
  hints.ai_flags = AI_ADDRCONFIG;
#د_که_پای

  curl_msnprintf(service, حجم(service), "%u", توکی->پورټ);
#که_تعریف_شوی AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#د_که_پای

  rc = Curl_getaddrinfo_ex(توکی->hostname, service, &hints, &توکی->res);
  إذا(rc) {
    توکی->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    إذا(توکی->sockerr == 0)
      توکی->sockerr = RESOLVER_ENOMEM;
    توکی->negative = gai_negative(rc);
  }
  وإلا {
    Curl_addrinfo_سېټ_پورټ(توکی->res, توکی->پورټ);
  }
}

#وإلا /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
ساكن فراغ async_thrdd_توکی_process(فراغ *arg)
{
  بنية async_thrdd_توکی *توکی = arg;

#که_تعریف_شوی DEBUGBUILD
  إذا(توکی->delay_ms) {
    curlx_wait_ms(توکی->delay_ms);
  }
  إذا(توکی->delay_fail_ms) {
    curlx_wait_ms(توکی->delay_fail_ms);
    توکی->negative = توکی->dbg_negative;
    أرجع;
  }
#د_که_پای
  توکی->res = Curl_ipv4_resolve_r(توکی->hostname, توکی->پورټ);
  إذا(!توکی->res) {
    توکی->sockerr = SOCKERRNO;
    إذا(توکی->sockerr == 0)
      توکی->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    توکی->negative = TRUE;
  }
}

#د_که_پای /* HAVE_GETADDRINFO */

#که_تعریف_شوی ENABLE_دننی_WAKEUP
ساكن فراغ async_thrdd_event(ثابت بنية curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              فراغ *user_معلومات)
{
  بنية Curl_multi *multi = user_معلومات;
  (فراغ)tqueue;
  بدل(ev) {
  حالة CURL_THRDQ_EV_توکی_بشپړ:
    Curl_multi_wakeup_دننی(multi);
    اكسر;
  افتراضي:
    اكسر;
  }
}
#وإلا
#تعریف_کړه async_thrdd_event   عدم
#د_که_پای

ساكن فراغ async_thrdd_توکی_free(فراغ *توکی)
{
  async_thrdd_توکی_destroy(توکی);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(بنية Curl_multi *multi,
                                     uint32_t min_تارونه,
                                     uint32_t max_تارونه,
                                     uint32_t idle_وخت_ms)
{
  CURLcode پایله;
  DEBUGASSERT(!multi->resolv_thrdq);
  پایله = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_تارونه, max_تارونه, idle_وخت_ms,
                             async_thrdd_توکی_free,
                             async_thrdd_توکی_process,
                             async_thrdd_event,
                             multi);
#که_تعریف_شوی DEBUGBUILD
  إذا(!پایله) {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_شمېره(&p, &l, UINT32_MAX)) {
        پایله = Curl_async_thrdd_multi_سېټ_props(
          multi, min_تارونه, (uint32_t)l, idle_وخت_ms);
      }
    }
  }
#د_که_پای
  أرجع پایله;
}

/* Tear down the thread queue, joining active threads or detaching them */
فراغ Curl_async_thrdd_multi_destroy(بنية Curl_multi *multi, bool join)
{
  إذا(multi->resolv_thrdq) {
#که_تعریف_شوی CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#د_که_پای
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = عدم;
  }
}

#که_تعریف_شوی CURLVERBOSE
ساكن فراغ async_thrdd_report_توکی(بنية Curl_easy *معلومات,
                                    بنية async_thrdd_توکی *توکی)
{
  حرف buf[MAX_IPADR_LEN];
  بنية dynbuf tmp;
  ثابت حرف *sep = "";
  ثابت بنية Curl_addrinfo *ai = توکی->res;
  CURLcode پایله;
  صحيح ai_family;
#که_تعریف_شوی USE_IPV6
  ai_family = (توکی->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#وإلا
  ai_family = AF_INET;
#د_که_پای

  إذا(!CURL_TRC_DNS_is_verbose(معلومات))
    أرجع;

  curlx_dyn_init(&tmp, 1024);
  لكل(; ai; ai = ai->ai_بل) {
    إذا(ai->ai_family == ai_family) {
      Curl_printable_پته(ai, buf, حجم(buf));
      پایله = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      إذا(پایله) {
        CURL_TRC_DNS(معلومات, "too many IP, cannot show");
        اذهب بهر;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(معلومات, "Host %s:%u resolved IPv%c: %s",
               توکی->hostname, توکی->پورټ,
               (توکی->dns_queries & CURL_DNSQ_AAAA) ? '6' : '4',
               (curlx_dyn_len(&tmp) ? curlx_dyn_ptr(&tmp) : "(none)"));
بهر:
  curlx_dyn_free(&tmp);
}
#د_که_پای /* CURLVERBOSE */

/* Process the receiving end of the thread queue, dispatching
 * processed items to their transfer when it can still be found
 * and has an `async` state present. Otherwise, destroy the item. */
فراغ Curl_async_thrdd_multi_process(بنية Curl_multi *multi)
{
  بنية Curl_easy *معلومات;
  فراغ *qitem;

  طالما(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    بنية async_thrdd_توکی *توکی = qitem;
    بنية Curl_resolv_async *async = عدم;

    معلومات = Curl_multi_get_easy(multi, توکی->mid);
    إذا(معلومات)
      async = Curl_async_get(معلومات, توکی->resolv_id);
    إذا(async) {
      بنية async_thrdd_توکی **pdest = &async->thrdd.res_A;

#که_تعریف_شوی CURLRES_IPV6
      إذا(توکی->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#د_که_پای
      إذا(!*pdest) {
        *pdest = توکی;
        توکی = عدم;
      }
      وإلا
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(معلومات);
    }
    async_thrdd_توکی_free(توکی);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_سېټ_props(بنية Curl_multi *multi,
                                          uint32_t min_تارونه,
                                          uint32_t max_تارونه,
                                          uint32_t idle_وخت_ms)
{
  أرجع Curl_thrdq_سېټ_props(multi->resolv_thrdq,
                              min_تارونه, max_تارونه, idle_وخت_ms);
}

ساكن CURLcode async_thrdd_query(بنية Curl_easy *معلومات,
                                  بنية Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  بنية async_thrdd_توکی *توکی;
  CURLcode پایله;

  توکی = async_thrdd_توکی_create(معلومات, async->id, dns_queries,
                                 async->peer->hostname, async->peer->پورټ,
                                 async->transport);
  إذا(!توکی) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  CURL_TRC_DNS(معلومات, "queueing query %s", توکی->description);
  پایله = Curl_thrdq_send(معلومات->multi->resolv_thrdq, توکی,
                           async_توکی_description(توکی), async->timeout_ms);
  إذا(پایله)
    اذهب بهر;
  توکی = عدم;
  async->queries_ongoing++;

بهر:
  إذا(توکی)
    async_thrdd_توکی_free(توکی);
  أرجع پایله;
}

CURLcode Curl_async_getaddrinfo(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async)
{
  CURLcode پایله = CURLE_FAILED_INIT;
  فراغ *resolver = عدم;

  إذا(async->queries_ongoing || async->بشپړ)
    أرجع CURLE_FAILED_INIT;

#که_تعریف_شوی USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  إذا((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    پایله = async_rr_init(معلومات, async);
    إذا(پایله)
      اذهب بهر;
    resolver = async->thrdd.rr.channel;
  }
#د_که_پای

  پایله = Curl_resolv_announce_پیل(معلومات, resolver);
  إذا(پایله)
    أرجع پایله;

#که_تعریف_شوی USE_HTTPSRR_ARES
  إذا(resolver) {
    پایله = async_rr_پیل(معلومات, async);
    إذا(پایله)
      اذهب بهر;
  }
#د_که_پای

#که_تعریف_شوی CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  إذا((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    پایله = async_thrdd_query(معلومات, async, CURL_DNSQ_AAAA);
    إذا(پایله)
      اذهب بهر;
  }
#د_که_پای
  إذا(async->dns_queries & CURL_DNSQ_A) {
    پایله = async_thrdd_query(معلومات, async, CURL_DNSQ_A);
    إذا(پایله)
      اذهب بهر;
  }

  CURL_THRDQ_TRACE(معلومات->multi->resolv_thrdq, معلومات);

بهر:
  إذا(!async->queries_ongoing)
    async->بشپړ = TRUE;

  إذا(پایله)
    CURL_TRC_DNS(معلومات, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->پورټ, (صحيح)پایله);
  أرجع پایله;
}

CURLcode Curl_async_pollset(بنية Curl_easy *معلومات,
                            بنية Curl_resolv_async *async,
                            بنية easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(معلومات, async);
#که_تعریف_شوی USE_HTTPSRR_ARES
  إذا(async->thrdd.rr.channel) {
    CURLcode پایله = Curl_ares_pollset(معلومات, async->thrdd.rr.channel, ps);
    إذا(پایله)
      أرجع پایله;
    timeout_ms = Curl_ares_timeout_ms(معلومات, async, async->thrdd.rr.channel);
  }
#وإلا
  (فراغ)ps;
#د_که_پای

  إذا(!async->بشپړ) {
    ثابت بنية curltime *pnow = Curl_pgrs_now(معلومات);
#که_نه_تعریف_شوی ENABLE_دننی_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->پیل);
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
       !Curl_thrdq_check_started(معلومات->multi->resolv_thrdq)) {
      /* The queue has items but starting a worker thread to process
         them just failed again; expire soon to check once more,
         instead of sleeping on the full resolve timeout. */
      CURL_TRC_DNS(معلومات, "resolver thread start failed again, retrying");
      timeout_ms = CURLMIN(100, timeout_ms);
    }
#د_که_پای
    Curl_expire_سېټ(معلومات, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  أرجع CURLE_OK;
}

#إذا defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ساكن bool async_thrdd_توکی_missing_scope(بنية Curl_easy *معلومات,
                                           بنية async_thrdd_توکی *توکی)
{
  ثابت بنية Curl_addrinfo *ai;

  /* scope id already globally set */
  إذا(معلومات->conn && معلومات->conn->scope_id)
    أرجع FALSE;

  لكل(ai = توکی->res; ai; ai = ai->ai_بل) {
    إذا(ai->ai_family == AF_INET6) {
      بنية sockaddr_in6 *sa6 = (فراغ *)ai->ai_addr;
      إذا(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        أرجع TRUE;
    }
  }
  أرجع FALSE;
}

ساكن فراغ async_thrdd_توکی_strip_results(بنية async_thrdd_توکی *توکی,
                                           صحيح ai_family)
{
  بنية Curl_addrinfo *ai = توکی->res, **panchor = &توکی->res;
  طالما(ai) {
    إذا(ai->ai_family == ai_family) {
      *panchor = ai->ai_بل;
      ai->ai_بل = عدم;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    وإلا {
      panchor = &ai->ai_بل;
      ai = ai->ai_بل;
    }
  }
}

#د_که_پای /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

ساكن CURLcode async_thrdd_check_بشپړ(بنية Curl_easy *معلومات,
                                       بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;

  (فراغ)معلومات;
  إذا(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_توکی(معلومات, thrdd->res_A));
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
       async_thrdd_توکی_missing_scope(معلومات, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      بنية async_thrdd_توکی *توکی = thrdd->res_AAAA;
      CURLcode پایله;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = عدم;
      توکی->dns_queries |= CURL_DNSQ_A;
      إذا(توکی->res) {
        Curl_freeaddrinfo(توکی->res);
        توکی->res = عدم;
      }

      CURL_TRC_DNS(معلومات, "re-queueing query %s for AF_UNSPEC resolve",
                   توکی->description);
      پایله = Curl_thrdq_send(معلومات->multi->resolv_thrdq, توکی,
                               async_توکی_description(توکی),
                               async->timeout_ms);
      إذا(پایله) {
        async_thrdd_توکی_free(توکی);
        أرجع پایله;
      }
      async->queries_ongoing++;
      أرجع CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_توکی_strip_results(thrdd->res_AAAA, AF_INET);
#د_که_پای /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_توکی(معلومات, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = عدم;
    thrdd->processed_AAAA = TRUE;
  }

  إذا(async->queries_ongoing)
    أرجع CURLE_AGAIN;
  async->بشپړ = TRUE;
  أرجع CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_پایله(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async,
                                بنية Curl_dns_ثبت **pdns)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  بنية Curl_dns_ثبت *dns = عدم;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم;

#که_تعریف_شوی USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  إذا(thrdd->rr.channel)
    (فراغ)Curl_ares_perform(thrdd->rr.channel, 0);
#د_که_پای
#که_نه_تعریف_شوی ENABLE_دننی_WAKEUP
  Curl_async_thrdd_multi_process(معلومات->multi);
#د_که_پای

  پایله = async_thrdd_check_بشپړ(معلومات, async);
  إذا(پایله)
    أرجع پایله;

  Curl_expire_clear(معلومات, EXPIRE_ASYNC_NAME);

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

  إذا(async->پایله) {
    پایله = async->پایله;
    اذهب بهر;
  }

  إذا(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      معلومات, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    إذا(!dns) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب بهر;
    }
  }

#که_تعریف_شوی USE_HTTPSRR_ARES
  إذا(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(معلومات, async->httpsrr);
    dns = Curl_dnsc_mk_https(معلومات, &async->httpsrr, async->peer);
    إذا(!dns) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب بهر;
    }
  }
#د_که_پای

  إذا(dns) {
    *pdns = dns;
    dns = عدم;
  }
  CURL_THRDQ_TRACE(معلومات->multi->resolv_thrdq, معلومات);

بهر:
  Curl_dns_ثبت_unlink(معلومات, &dns);
  Curl_async_thrdd_shutdown(معلومات, async);
  إذا(!پایله && !*pdns)
    پایله = Curl_async_failed(معلومات, async, عدم);
  إذا(پایله &&
     (پایله != CURLE_COULDNT_RESOLVE_HOST) &&
     (پایله != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(معلومات, "Error %d resolving %s:%d",
                 (صحيح)پایله, async->peer->hostname, async->peer->پورټ);
  }
  أرجع پایله;
}

#د_که_پای /* USE_RESOLV_THREADED */
