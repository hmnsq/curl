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
#جے_تعریف_شدہ USE_RESOLV_THREADED

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

#جے_تعریف_شدہ HAVE_GETADDRINFO
#تعریف_کرو RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#وإلا
#تعریف_کرو RESOLVER_ENOMEM  SOCKENOMEM
#جے_انت

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

#جے_تعریف_شدہ USE_ARES
#ضمن <ares.h>
#جے_تعریف_شدہ USE_HTTPSRR
#تعریف_کرو USE_HTTPSRR_ARES  /* the combo */
#جے_انت
#جے_انت


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
#جے_انت
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
#جے_انت
}

#جے_تعریف_شدہ CURLVERBOSE
#تعریف_کرو CURL_ASYN_شے_DESC_LEN   64
#تعریف_کرو async_شے_description(x)   (x)->description
#وإلا
#تعریف_کرو async_شے_description(x)   عدم
#جے_انت

بنية async_thrdd_شے {
  بنية Curl_addrinfo *res;
#جے_تعریف_شدہ CURLVERBOSE
  حرف description[CURL_ASYN_شے_DESC_LEN];
#جے_انت
  صحيح sockerr;
  uint32_t mid;
  uint32_t resolv_id;
  uint16_t پورٹ;
  uint8_t transport;
  uint8_t dns_queries;
  ثنائی_ہندسہ(negative); /* resolver answered that the name does not exist */
#جے_تعریف_شدہ DEBUGBUILD
  uint32_t delay_ms;
  uint32_t delay_fail_ms;
  ثنائی_ہندسہ(dbg_negative);
#جے_انت
  حرف hostname[1];
};

/* Give up reference to add_ctx */
ساكن فراغ async_thrdd_شے_destroy(بنية async_thrdd_شے *شے)
{
  إذا(شے) {
    إذا(شے->res)
      Curl_freeaddrinfo(شے->res);
    curlx_free(شے);
  }
}

/* Initialize context for threaded resolver */
ساكن بنية async_thrdd_شے *async_thrdd_شے_create(
  بنية Curl_easy *انکڑے,
  uint32_t resolv_id, uint8_t dns_queries,
  ثابت حرف *hostname, uint16_t پورٹ,
  uint8_t transport)
{
  size_t hostlen = لڑی_لمبائی(hostname);
  بنية async_thrdd_شے *شے;

  شے = curlx_calloc(1, حجم(*شے) + hostlen);
  إذا(!شے)
    أرجع عدم;

  إذا(hostlen) /* null-terminator of name already in struct size */
    حافظہ_نقل(شے->hostname, hostname, hostlen);
  شے->mid = انکڑے->mid;
  شے->resolv_id = resolv_id;
  شے->dns_queries = dns_queries;
  شے->پورٹ = پورٹ;
  شے->transport = transport;

#جے_تعریف_شدہ CURLVERBOSE
  curl_msnprintf(شے->description, حجم(شے->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 انکڑے->id, شے->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 شے->hostname, شے->پورٹ);
#جے_انت

#جے_تعریف_شدہ DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_DELAY");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نمبر(&p, &l, UINT32_MAX)) {
        شے->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نمبر(&p, &l, UINT32_MAX)) {
        بنانشانی حرف c = 0;
        Curl_rand_بائٹاں(انکڑے, FALSE, &c, 1);
        شے->delay_fail_ms = (uint32_t)l + c;
      }
    }
    إذا(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      شے->dbg_negative = TRUE;
  }
#جے_انت

  أرجع شے;
}

#جے_تعریف_شدہ USE_HTTPSRR_ARES

ساكن فراغ async_thrdd_rr_پورا(فراغ *user_انکڑے, ares_حالت_t حالت,
                                size_t timeouts,
                                ثابت ares_dns_ریکارڈ_t *dnsrec)
{
  بنية Curl_resolv_async *async = user_انکڑے;
  بنية async_thrdd_ctx *thrdd = async ? &async->thrdd : عدم;

  (فراغ)timeouts;
  إذا(!thrdd)
    أرجع;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->پورا = !async->queries_ongoing;
  إذا((ARES_SUCCESS == حالت) && dnsrec)
    async->نتیجہ = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
ساكن CURLcode async_rr_init(بنية Curl_easy *انکڑے,
                              بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  صحيح حالت;

  (فراغ)انکڑے;
  DEBUGASSERT(!thrdd->rr.channel);
  حالت = ares_init_options(&thrdd->rr.channel, عدم, 0);
  إذا(حالت != ARES_SUCCESS) {
    thrdd->rr.channel = عدم;
    أرجع CURLE_FAILED_INIT;
  }
#جے_تعریف_شدہ DEBUGBUILD
  إذا(getenv("CURL_DNS_SERVER")) {
    ثابت حرف *servers = getenv("CURL_DNS_SERVER");
    حالت = ares_سیٹ_servers_ports_csv(thrdd->rr.channel, servers);
    إذا(حالت)
      أرجع CURLE_FAILED_INIT;
  }
#جے_انت
  أرجع CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
ساكن CURLcode async_rr_شروع(بنية Curl_easy *انکڑے,
                               بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  حرف *https_name = عدم;
  CURLcode نتیجہ = CURLE_OK;

  إذا(async->peer->پورٹ != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->پورٹ, async->peer->hostname);
    إذا(!https_name) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_وچ, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_پورا, async, عدم);
  CURL_TRC_DNS(انکڑے, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

باہرلا:
  curlx_free(https_name);
  أرجع نتیجہ;
}
#جے_انت

فراغ Curl_async_thrdd_shutdown(بنية Curl_easy *انکڑے,
                               بنية Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(انکڑے, async);
}

بنية async_thrdd_میل_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

ساكن bool async_thrdd_میل_شے(فراغ *qitem, فراغ *میل_انکڑے)
{
  ثابت بنية async_thrdd_میل_ctx *ctx = میل_انکڑے;
  بنية async_thrdd_شے *شے = qitem;
  أرجع (شے->mid == ctx->mid) && (شے->resolv_id == ctx->resolv_id);
}

فراغ Curl_async_thrdd_destroy(بنية Curl_easy *انکڑے,
                              بنية Curl_resolv_async *async)
{
  (فراغ)انکڑے;
  إذا(async->queries_ongoing && !async->پورا &&
     انکڑے->multi && انکڑے->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    بنية async_thrdd_میل_ctx mctx;
    mctx.mid = انکڑے->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(انکڑے->multi->resolv_thrdq,
                     async_thrdd_میل_شے, &mctx);
  }
#جے_تعریف_شدہ USE_HTTPSRR_ARES
  إذا(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = عدم;
  }
#جے_انت
  async_thrdd_شے_destroy(async->thrdd.res_A);
  async->thrdd.res_A = عدم;
  async_thrdd_شے_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = عدم;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(بنية Curl_easy *انکڑے, uint32_t resolv_id,
                          بنية Curl_dns_اندراج **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(انکڑے, resolv_id);
  بنية async_thrdd_ctx *thrdd = async ? &async->thrdd : عدم;
  timediff_t milli, ms;
  CURLcode نتیجہ = CURLE_AGAIN;

  إذا(!thrdd)
    أرجع CURLE_FAILED_INIT;

  طالما(نتیجہ == CURLE_AGAIN) {
    طالما(async->queries_ongoing && !async->پورا) {
      Curl_async_thrdd_multi_process(انکڑے->multi);
      إذا(async->پورا)
        اكسر;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(انکڑے), &async->شروع);
      إذا(ms < 3)
        milli = 0;
      وإلا إذا(ms <= 50)
        milli = ms / 3;
      وإلا إذا(ms <= 250)
        milli = 50;
      وإلا
        milli = 200;
      CURL_TRC_DNS(انکڑے, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    نتیجہ = Curl_async_take_نتیجہ(انکڑے, async, pdns);
  }
  أرجع نتیجہ;
}

#جے_تعریف_شدہ HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
ساكن bool gai_negative(صحيح rc)
{
  بدل(rc) {
#جے_تعریف_شدہ EAI_NONAME
  حالة EAI_NONAME:
#جے_انت
#إذا defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  حالة EAI_NODATA:
#جے_انت
    أرجع TRUE;
  افتراضي:
    أرجع FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
ساكن فراغ async_thrdd_شے_process(فراغ *arg)
{
  بنية async_thrdd_شے *شے = arg;
  بنية addrinfo hints;
  حرف service[12];
  صحيح pf = PF_INET;
  صحيح rc;

#جے_تعریف_شدہ DEBUGBUILD
  إذا(شے->delay_ms) {
    curlx_wait_ms(شے->delay_ms);
  }
  إذا(شے->delay_fail_ms) {
    curlx_wait_ms(شے->delay_fail_ms);
    شے->negative = شے->dbg_negative;
    أرجع;
  }
#جے_انت

  memset(&hints, 0, حجم(hints));
#جے_تعریف_شدہ CURLRES_IPV6
  إذا(شے->dns_queries & CURL_DNSQ_AAAA) {
    pf = (شے->dns_queries & CURL_DNSQ_A) ? PF_UNSPEC : PF_INET6;
  }
#جے_انت
  hints.ai_family = pf;
  hints.ai_socktype = Curl_socktype_for_transport(شے->transport);
  hints.ai_protocol = Curl_protocol_for_transport(شے->transport);
#جے_تعریف_شدہ __APPLE__
  /* If we leave `ai_flags == 0` then macOS is looking for IPV4MAPPED
   * when doing AAAA queries. We do not want this "help". */
  hints.ai_flags = AI_ADDRCONFIG;
#جے_انت

  curl_msnprintf(service, حجم(service), "%u", شے->پورٹ);
#جے_تعریف_شدہ AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#جے_انت

  rc = Curl_getaddrinfo_ex(شے->hostname, service, &hints, &شے->res);
  إذا(rc) {
    شے->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    إذا(شے->sockerr == 0)
      شے->sockerr = RESOLVER_ENOMEM;
    شے->negative = gai_negative(rc);
  }
  وإلا {
    Curl_addrinfo_سیٹ_پورٹ(شے->res, شے->پورٹ);
  }
}

#وإلا /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
ساكن فراغ async_thrdd_شے_process(فراغ *arg)
{
  بنية async_thrdd_شے *شے = arg;

#جے_تعریف_شدہ DEBUGBUILD
  إذا(شے->delay_ms) {
    curlx_wait_ms(شے->delay_ms);
  }
  إذا(شے->delay_fail_ms) {
    curlx_wait_ms(شے->delay_fail_ms);
    شے->negative = شے->dbg_negative;
    أرجع;
  }
#جے_انت
  شے->res = Curl_ipv4_resolve_r(شے->hostname, شے->پورٹ);
  إذا(!شے->res) {
    شے->sockerr = SOCKERRNO;
    إذا(شے->sockerr == 0)
      شے->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    شے->negative = TRUE;
  }
}

#جے_انت /* HAVE_GETADDRINFO */

#جے_تعریف_شدہ ENABLE_اندرونی_WAKEUP
ساكن فراغ async_thrdd_event(ثابت بنية curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              فراغ *user_انکڑے)
{
  بنية Curl_multi *multi = user_انکڑے;
  (فراغ)tqueue;
  بدل(ev) {
  حالة CURL_THRDQ_EV_شے_پورا:
    Curl_multi_wakeup_اندرونی(multi);
    اكسر;
  افتراضي:
    اكسر;
  }
}
#وإلا
#تعریف_کرو async_thrdd_event   عدم
#جے_انت

ساكن فراغ async_thrdd_شے_free(فراغ *شے)
{
  async_thrdd_شے_destroy(شے);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(بنية Curl_multi *multi,
                                     uint32_t min_دھاگے,
                                     uint32_t max_دھاگے,
                                     uint32_t idle_ویلا_ms)
{
  CURLcode نتیجہ;
  DEBUGASSERT(!multi->resolv_thrdq);
  نتیجہ = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_دھاگے, max_دھاگے, idle_ویلا_ms,
                             async_thrdd_شے_free,
                             async_thrdd_شے_process,
                             async_thrdd_event,
                             multi);
#جے_تعریف_شدہ DEBUGBUILD
  إذا(!نتیجہ) {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نمبر(&p, &l, UINT32_MAX)) {
        نتیجہ = Curl_async_thrdd_multi_سیٹ_props(
          multi, min_دھاگے, (uint32_t)l, idle_ویلا_ms);
      }
    }
  }
#جے_انت
  أرجع نتیجہ;
}

/* Tear down the thread queue, joining active threads or detaching them */
فراغ Curl_async_thrdd_multi_destroy(بنية Curl_multi *multi, bool join)
{
  إذا(multi->resolv_thrdq) {
#جے_تعریف_شدہ CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#جے_انت
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = عدم;
  }
}

#جے_تعریف_شدہ CURLVERBOSE
ساكن فراغ async_thrdd_report_شے(بنية Curl_easy *انکڑے,
                                    بنية async_thrdd_شے *شے)
{
  حرف buf[MAX_IPADR_LEN];
  بنية dynbuf tmp;
  ثابت حرف *sep = "";
  ثابت بنية Curl_addrinfo *ai = شے->res;
  CURLcode نتیجہ;
  صحيح ai_family;
#جے_تعریف_شدہ USE_IPV6
  ai_family = (شے->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#وإلا
  ai_family = AF_INET;
#جے_انت

  إذا(!CURL_TRC_DNS_is_verbose(انکڑے))
    أرجع;

  curlx_dyn_init(&tmp, 1024);
  لكل(; ai; ai = ai->ai_اگلا) {
    إذا(ai->ai_family == ai_family) {
      Curl_printable_پتہ(ai, buf, حجم(buf));
      نتیجہ = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      إذا(نتیجہ) {
        CURL_TRC_DNS(انکڑے, "too many IP, cannot show");
        اذهب باہرلا;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(انکڑے, "Host %s:%u resolved IPv%c: %s",
               شے->hostname, شے->پورٹ,
               (شے->dns_queries & CURL_DNSQ_AAAA) ? '6' : '4',
               (curlx_dyn_len(&tmp) ? curlx_dyn_ptr(&tmp) : "(none)"));
باہرلا:
  curlx_dyn_free(&tmp);
}
#جے_انت /* CURLVERBOSE */

/* Process the receiving end of the thread queue, dispatching
 * processed items to their transfer when it can still be found
 * and has an `async` state present. Otherwise, destroy the item. */
فراغ Curl_async_thrdd_multi_process(بنية Curl_multi *multi)
{
  بنية Curl_easy *انکڑے;
  فراغ *qitem;

  طالما(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    بنية async_thrdd_شے *شے = qitem;
    بنية Curl_resolv_async *async = عدم;

    انکڑے = Curl_multi_get_easy(multi, شے->mid);
    إذا(انکڑے)
      async = Curl_async_get(انکڑے, شے->resolv_id);
    إذا(async) {
      بنية async_thrdd_شے **pdest = &async->thrdd.res_A;

#جے_تعریف_شدہ CURLRES_IPV6
      إذا(شے->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#جے_انت
      إذا(!*pdest) {
        *pdest = شے;
        شے = عدم;
      }
      وإلا
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(انکڑے);
    }
    async_thrdd_شے_free(شے);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_سیٹ_props(بنية Curl_multi *multi,
                                          uint32_t min_دھاگے,
                                          uint32_t max_دھاگے,
                                          uint32_t idle_ویلا_ms)
{
  أرجع Curl_thrdq_سیٹ_props(multi->resolv_thrdq,
                              min_دھاگے, max_دھاگے, idle_ویلا_ms);
}

ساكن CURLcode async_thrdd_query(بنية Curl_easy *انکڑے,
                                  بنية Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  بنية async_thrdd_شے *شے;
  CURLcode نتیجہ;

  شے = async_thrdd_شے_create(انکڑے, async->id, dns_queries,
                                 async->peer->hostname, async->peer->پورٹ,
                                 async->transport);
  إذا(!شے) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  CURL_TRC_DNS(انکڑے, "queueing query %s", شے->description);
  نتیجہ = Curl_thrdq_send(انکڑے->multi->resolv_thrdq, شے,
                           async_شے_description(شے), async->timeout_ms);
  إذا(نتیجہ)
    اذهب باہرلا;
  شے = عدم;
  async->queries_ongoing++;

باہرلا:
  إذا(شے)
    async_thrdd_شے_free(شے);
  أرجع نتیجہ;
}

CURLcode Curl_async_getaddrinfo(بنية Curl_easy *انکڑے,
                                بنية Curl_resolv_async *async)
{
  CURLcode نتیجہ = CURLE_FAILED_INIT;
  فراغ *resolver = عدم;

  إذا(async->queries_ongoing || async->پورا)
    أرجع CURLE_FAILED_INIT;

#جے_تعریف_شدہ USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  إذا((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    نتیجہ = async_rr_init(انکڑے, async);
    إذا(نتیجہ)
      اذهب باہرلا;
    resolver = async->thrdd.rr.channel;
  }
#جے_انت

  نتیجہ = Curl_resolv_announce_شروع(انکڑے, resolver);
  إذا(نتیجہ)
    أرجع نتیجہ;

#جے_تعریف_شدہ USE_HTTPSRR_ARES
  إذا(resolver) {
    نتیجہ = async_rr_شروع(انکڑے, async);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
#جے_انت

#جے_تعریف_شدہ CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  إذا((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    نتیجہ = async_thrdd_query(انکڑے, async, CURL_DNSQ_AAAA);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
#جے_انت
  إذا(async->dns_queries & CURL_DNSQ_A) {
    نتیجہ = async_thrdd_query(انکڑے, async, CURL_DNSQ_A);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  CURL_THRDQ_TRACE(انکڑے->multi->resolv_thrdq, انکڑے);

باہرلا:
  إذا(!async->queries_ongoing)
    async->پورا = TRUE;

  إذا(نتیجہ)
    CURL_TRC_DNS(انکڑے, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->پورٹ, (صحيح)نتیجہ);
  أرجع نتیجہ;
}

CURLcode Curl_async_pollset(بنية Curl_easy *انکڑے,
                            بنية Curl_resolv_async *async,
                            بنية easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(انکڑے, async);
#جے_تعریف_شدہ USE_HTTPSRR_ARES
  إذا(async->thrdd.rr.channel) {
    CURLcode نتیجہ = Curl_ares_pollset(انکڑے, async->thrdd.rr.channel, ps);
    إذا(نتیجہ)
      أرجع نتیجہ;
    timeout_ms = Curl_ares_timeout_ms(انکڑے, async, async->thrdd.rr.channel);
  }
#وإلا
  (فراغ)ps;
#جے_انت

  إذا(!async->پورا) {
    ثابت بنية curltime *pnow = Curl_pgrs_now(انکڑے);
#جے_بنا_تعریف ENABLE_اندرونی_WAKEUP
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
       !Curl_thrdq_check_started(انکڑے->multi->resolv_thrdq)) {
      /* The queue has items but starting a worker thread to process
         them just failed again; expire soon to check once more,
         instead of sleeping on the full resolve timeout. */
      CURL_TRC_DNS(انکڑے, "resolver thread start failed again, retrying");
      timeout_ms = CURLMIN(100, timeout_ms);
    }
#جے_انت
    Curl_expire_سیٹ(انکڑے, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  أرجع CURLE_OK;
}

#إذا defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ساكن bool async_thrdd_شے_missing_scope(بنية Curl_easy *انکڑے,
                                           بنية async_thrdd_شے *شے)
{
  ثابت بنية Curl_addrinfo *ai;

  /* scope id already globally set */
  إذا(انکڑے->conn && انکڑے->conn->scope_id)
    أرجع FALSE;

  لكل(ai = شے->res; ai; ai = ai->ai_اگلا) {
    إذا(ai->ai_family == AF_INET6) {
      بنية sockaddr_in6 *sa6 = (فراغ *)ai->ai_addr;
      إذا(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        أرجع TRUE;
    }
  }
  أرجع FALSE;
}

ساكن فراغ async_thrdd_شے_strip_results(بنية async_thrdd_شے *شے,
                                           صحيح ai_family)
{
  بنية Curl_addrinfo *ai = شے->res, **panchor = &شے->res;
  طالما(ai) {
    إذا(ai->ai_family == ai_family) {
      *panchor = ai->ai_اگلا;
      ai->ai_اگلا = عدم;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    وإلا {
      panchor = &ai->ai_اگلا;
      ai = ai->ai_اگلا;
    }
  }
}

#جے_انت /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

ساكن CURLcode async_thrdd_check_پورا(بنية Curl_easy *انکڑے,
                                       بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;

  (فراغ)انکڑے;
  إذا(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_شے(انکڑے, thrdd->res_A));
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
       async_thrdd_شے_missing_scope(انکڑے, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      بنية async_thrdd_شے *شے = thrdd->res_AAAA;
      CURLcode نتیجہ;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = عدم;
      شے->dns_queries |= CURL_DNSQ_A;
      إذا(شے->res) {
        Curl_freeaddrinfo(شے->res);
        شے->res = عدم;
      }

      CURL_TRC_DNS(انکڑے, "re-queueing query %s for AF_UNSPEC resolve",
                   شے->description);
      نتیجہ = Curl_thrdq_send(انکڑے->multi->resolv_thrdq, شے,
                               async_شے_description(شے),
                               async->timeout_ms);
      إذا(نتیجہ) {
        async_thrdd_شے_free(شے);
        أرجع نتیجہ;
      }
      async->queries_ongoing++;
      أرجع CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_شے_strip_results(thrdd->res_AAAA, AF_INET);
#جے_انت /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_شے(انکڑے, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = عدم;
    thrdd->processed_AAAA = TRUE;
  }

  إذا(async->queries_ongoing)
    أرجع CURLE_AGAIN;
  async->پورا = TRUE;
  أرجع CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_نتیجہ(بنية Curl_easy *انکڑے,
                                بنية Curl_resolv_async *async,
                                بنية Curl_dns_اندراج **pdns)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  بنية Curl_dns_اندراج *dns = عدم;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم;

#جے_تعریف_شدہ USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  إذا(thrdd->rr.channel)
    (فراغ)Curl_ares_perform(thrdd->rr.channel, 0);
#جے_انت
#جے_بنا_تعریف ENABLE_اندرونی_WAKEUP
  Curl_async_thrdd_multi_process(انکڑے->multi);
#جے_انت

  نتیجہ = async_thrdd_check_پورا(انکڑے, async);
  إذا(نتیجہ)
    أرجع نتیجہ;

  Curl_expire_clear(انکڑے, EXPIRE_ASYNC_NAME);

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

  إذا(async->نتیجہ) {
    نتیجہ = async->نتیجہ;
    اذهب باہرلا;
  }

  إذا(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      انکڑے, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    إذا(!dns) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }

#جے_تعریف_شدہ USE_HTTPSRR_ARES
  إذا(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(انکڑے, async->httpsrr);
    dns = Curl_dnsc_mk_https(انکڑے, &async->httpsrr, async->peer);
    إذا(!dns) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }
#جے_انت

  إذا(dns) {
    *pdns = dns;
    dns = عدم;
  }
  CURL_THRDQ_TRACE(انکڑے->multi->resolv_thrdq, انکڑے);

باہرلا:
  Curl_dns_اندراج_unlink(انکڑے, &dns);
  Curl_async_thrdd_shutdown(انکڑے, async);
  إذا(!نتیجہ && !*pdns)
    نتیجہ = Curl_async_failed(انکڑے, async, عدم);
  إذا(نتیجہ &&
     (نتیجہ != CURLE_COULDNT_RESOLVE_HOST) &&
     (نتیجہ != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(انکڑے, "Error %d resolving %s:%d",
                 (صحيح)نتیجہ, async->peer->hostname, async->peer->پورٹ);
  }
  أرجع نتیجہ;
}

#جے_انت /* USE_RESOLV_THREADED */
