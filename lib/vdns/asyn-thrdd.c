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
#جيڪڏهن_وصف_ٿيل USE_RESOLV_THREADED

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_GETADDRINFO
#وصف_ڪريو RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#وإلا
#وصف_ڪريو RESOLVER_ENOMEM  SOCKENOMEM
#جيڪڏهن_پورو

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

#جيڪڏهن_وصف_ٿيل USE_ARES
#ضمن <ares.h>
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
#وصف_ڪريو USE_HTTPSRR_ARES  /* the combo */
#جيڪڏهن_پورو
#جيڪڏهن_پورو


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
#جيڪڏهن_پورو
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
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
#وصف_ڪريو CURL_ASYN_شي_DESC_LEN   64
#وصف_ڪريو async_شي_description(x)   (x)->description
#وإلا
#وصف_ڪريو async_شي_description(x)   عدم
#جيڪڏهن_پورو

بنية async_thrdd_شي {
  بنية Curl_addrinfo *res;
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  حرف description[CURL_ASYN_شي_DESC_LEN];
#جيڪڏهن_پورو
  صحيح sockerr;
  uint32_t mid;
  uint32_t resolv_id;
  uint16_t پورٽ;
  uint8_t transport;
  uint8_t dns_queries;
  ٻٽو_انگ(negative); /* resolver answered that the name does not exist */
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  uint32_t delay_ms;
  uint32_t delay_fail_ms;
  ٻٽو_انگ(dbg_negative);
#جيڪڏهن_پورو
  حرف hostname[1];
};

/* Give up reference to add_ctx */
ساكن فراغ async_thrdd_شي_destroy(بنية async_thrdd_شي *شي)
{
  إذا(شي) {
    إذا(شي->res)
      Curl_freeaddrinfo(شي->res);
    curlx_free(شي);
  }
}

/* Initialize context for threaded resolver */
ساكن بنية async_thrdd_شي *async_thrdd_شي_create(
  بنية Curl_easy *معلومات,
  uint32_t resolv_id, uint8_t dns_queries,
  ثابت حرف *hostname, uint16_t پورٽ,
  uint8_t transport)
{
  size_t hostlen = لڙي_ڊيگهه(hostname);
  بنية async_thrdd_شي *شي;

  شي = curlx_calloc(1, حجم(*شي) + hostlen);
  إذا(!شي)
    أرجع عدم;

  إذا(hostlen) /* null-terminator of name already in struct size */
    يادگيري_نقل(شي->hostname, hostname, hostlen);
  شي->mid = معلومات->mid;
  شي->resolv_id = resolv_id;
  شي->dns_queries = dns_queries;
  شي->پورٽ = پورٽ;
  شي->transport = transport;

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  curl_msnprintf(شي->description, حجم(شي->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 معلومات->id, شي->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 شي->hostname, شي->پورٽ);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_DELAY");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        شي->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        بنانشان حرف c = 0;
        Curl_rand_بائيٽون(معلومات, FALSE, &c, 1);
        شي->delay_fail_ms = (uint32_t)l + c;
      }
    }
    إذا(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      شي->dbg_negative = TRUE;
  }
#جيڪڏهن_پورو

  أرجع شي;
}

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR_ARES

ساكن فراغ async_thrdd_rr_پورو(فراغ *user_معلومات, ares_حالت_t حالت,
                                size_t timeouts,
                                ثابت ares_dns_رڪارڊ_t *dnsrec)
{
  بنية Curl_resolv_async *async = user_معلومات;
  بنية async_thrdd_ctx *thrdd = async ? &async->thrdd : عدم;

  (فراغ)timeouts;
  إذا(!thrdd)
    أرجع;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->پورو = !async->queries_ongoing;
  إذا((ARES_SUCCESS == حالت) && dnsrec)
    async->نتيجو = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
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
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  إذا(getenv("CURL_DNS_SERVER")) {
    ثابت حرف *servers = getenv("CURL_DNS_SERVER");
    حالت = ares_سيٽ_servers_ports_csv(thrdd->rr.channel, servers);
    إذا(حالت)
      أرجع CURLE_FAILED_INIT;
  }
#جيڪڏهن_پورو
  أرجع CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
ساكن CURLcode async_rr_شروعات(بنية Curl_easy *معلومات,
                               بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  حرف *https_name = عدم;
  CURLcode نتيجو = CURLE_OK;

  إذا(async->peer->پورٽ != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->پورٽ, async->peer->hostname);
    إذا(!https_name) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_اندريون, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_پورو, async, عدم);
  CURL_TRC_DNS(معلومات, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

ٻاهريون:
  curlx_free(https_name);
  أرجع نتيجو;
}
#جيڪڏهن_پورو

فراغ Curl_async_thrdd_shutdown(بنية Curl_easy *معلومات,
                               بنية Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(معلومات, async);
}

بنية async_thrdd_ميلاپ_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

ساكن bool async_thrdd_ميلاپ_شي(فراغ *qitem, فراغ *ميلاپ_معلومات)
{
  ثابت بنية async_thrdd_ميلاپ_ctx *ctx = ميلاپ_معلومات;
  بنية async_thrdd_شي *شي = qitem;
  أرجع (شي->mid == ctx->mid) && (شي->resolv_id == ctx->resolv_id);
}

فراغ Curl_async_thrdd_destroy(بنية Curl_easy *معلومات,
                              بنية Curl_resolv_async *async)
{
  (فراغ)معلومات;
  إذا(async->queries_ongoing && !async->پورو &&
     معلومات->multi && معلومات->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    بنية async_thrdd_ميلاپ_ctx mctx;
    mctx.mid = معلومات->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(معلومات->multi->resolv_thrdq,
                     async_thrdd_ميلاپ_شي, &mctx);
  }
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR_ARES
  إذا(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = عدم;
  }
#جيڪڏهن_پورو
  async_thrdd_شي_destroy(async->thrdd.res_A);
  async->thrdd.res_A = عدم;
  async_thrdd_شي_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = عدم;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(بنية Curl_easy *معلومات, uint32_t resolv_id,
                          بنية Curl_dns_داخلا **pdns)
{
  بنية Curl_resolv_async *async = Curl_async_get(معلومات, resolv_id);
  بنية async_thrdd_ctx *thrdd = async ? &async->thrdd : عدم;
  timediff_t milli, ms;
  CURLcode نتيجو = CURLE_AGAIN;

  إذا(!thrdd)
    أرجع CURLE_FAILED_INIT;

  طالما(نتيجو == CURLE_AGAIN) {
    طالما(async->queries_ongoing && !async->پورو) {
      Curl_async_thrdd_multi_process(معلومات->multi);
      إذا(async->پورو)
        اكسر;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &async->شروعات);
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
    نتيجو = Curl_async_take_نتيجو(معلومات, async, pdns);
  }
  أرجع نتيجو;
}

#جيڪڏهن_وصف_ٿيل HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
ساكن bool gai_negative(صحيح rc)
{
  بدل(rc) {
#جيڪڏهن_وصف_ٿيل EAI_NONAME
  حالة EAI_NONAME:
#جيڪڏهن_پورو
#إذا defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  حالة EAI_NODATA:
#جيڪڏهن_پورو
    أرجع TRUE;
  افتراضي:
    أرجع FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
ساكن فراغ async_thrdd_شي_process(فراغ *arg)
{
  بنية async_thrdd_شي *شي = arg;
  بنية addrinfo hints;
  حرف service[12];
  صحيح pf = PF_INET;
  صحيح rc;

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  إذا(شي->delay_ms) {
    curlx_wait_ms(شي->delay_ms);
  }
  إذا(شي->delay_fail_ms) {
    curlx_wait_ms(شي->delay_fail_ms);
    شي->negative = شي->dbg_negative;
    أرجع;
  }
#جيڪڏهن_پورو

  memset(&hints, 0, حجم(hints));
#جيڪڏهن_وصف_ٿيل CURLRES_IPV6
  إذا(شي->dns_queries & CURL_DNSQ_AAAA) {
    pf = (شي->dns_queries & CURL_DNSQ_A) ? PF_UNSPEC : PF_INET6;
  }
#جيڪڏهن_پورو
  hints.ai_family = pf;
  hints.ai_socktype = Curl_socktype_for_transport(شي->transport);
  hints.ai_protocol = Curl_protocol_for_transport(شي->transport);
#جيڪڏهن_وصف_ٿيل __APPLE__
  /* If we leave `ai_flags == 0` then macOS is looking for IPV4MAPPED
   * when doing AAAA queries. We do not want this "help". */
  hints.ai_flags = AI_ADDRCONFIG;
#جيڪڏهن_پورو

  curl_msnprintf(service, حجم(service), "%u", شي->پورٽ);
#جيڪڏهن_وصف_ٿيل AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#جيڪڏهن_پورو

  rc = Curl_getaddrinfo_ex(شي->hostname, service, &hints, &شي->res);
  إذا(rc) {
    شي->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    إذا(شي->sockerr == 0)
      شي->sockerr = RESOLVER_ENOMEM;
    شي->negative = gai_negative(rc);
  }
  وإلا {
    Curl_addrinfo_سيٽ_پورٽ(شي->res, شي->پورٽ);
  }
}

#وإلا /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
ساكن فراغ async_thrdd_شي_process(فراغ *arg)
{
  بنية async_thrdd_شي *شي = arg;

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  إذا(شي->delay_ms) {
    curlx_wait_ms(شي->delay_ms);
  }
  إذا(شي->delay_fail_ms) {
    curlx_wait_ms(شي->delay_fail_ms);
    شي->negative = شي->dbg_negative;
    أرجع;
  }
#جيڪڏهن_پورو
  شي->res = Curl_ipv4_resolve_r(شي->hostname, شي->پورٽ);
  إذا(!شي->res) {
    شي->sockerr = SOCKERRNO;
    إذا(شي->sockerr == 0)
      شي->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    شي->negative = TRUE;
  }
}

#جيڪڏهن_پورو /* HAVE_GETADDRINFO */

#جيڪڏهن_وصف_ٿيل ENABLE_اندروني_WAKEUP
ساكن فراغ async_thrdd_event(ثابت بنية curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              فراغ *user_معلومات)
{
  بنية Curl_multi *multi = user_معلومات;
  (فراغ)tqueue;
  بدل(ev) {
  حالة CURL_THRDQ_EV_شي_پورو:
    Curl_multi_wakeup_اندروني(multi);
    اكسر;
  افتراضي:
    اكسر;
  }
}
#وإلا
#وصف_ڪريو async_thrdd_event   عدم
#جيڪڏهن_پورو

ساكن فراغ async_thrdd_شي_free(فراغ *شي)
{
  async_thrdd_شي_destroy(شي);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(بنية Curl_multi *multi,
                                     uint32_t min_ڌاڳا,
                                     uint32_t max_ڌاڳا,
                                     uint32_t idle_وقت_ms)
{
  CURLcode نتيجو;
  DEBUGASSERT(!multi->resolv_thrdq);
  نتيجو = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_ڌاڳا, max_ڌاڳا, idle_وقت_ms,
                             async_thrdd_شي_free,
                             async_thrdd_شي_process,
                             async_thrdd_event,
                             multi);
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  إذا(!نتيجو) {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        نتيجو = Curl_async_thrdd_multi_سيٽ_props(
          multi, min_ڌاڳا, (uint32_t)l, idle_وقت_ms);
      }
    }
  }
#جيڪڏهن_پورو
  أرجع نتيجو;
}

/* Tear down the thread queue, joining active threads or detaching them */
فراغ Curl_async_thrdd_multi_destroy(بنية Curl_multi *multi, bool join)
{
  إذا(multi->resolv_thrdq) {
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#جيڪڏهن_پورو
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = عدم;
  }
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ساكن فراغ async_thrdd_report_شي(بنية Curl_easy *معلومات,
                                    بنية async_thrdd_شي *شي)
{
  حرف buf[MAX_IPADR_LEN];
  بنية dynbuf tmp;
  ثابت حرف *sep = "";
  ثابت بنية Curl_addrinfo *ai = شي->res;
  CURLcode نتيجو;
  صحيح ai_family;
#جيڪڏهن_وصف_ٿيل USE_IPV6
  ai_family = (شي->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#وإلا
  ai_family = AF_INET;
#جيڪڏهن_پورو

  إذا(!CURL_TRC_DNS_is_verbose(معلومات))
    أرجع;

  curlx_dyn_init(&tmp, 1024);
  لكل(; ai; ai = ai->ai_ايندڙ) {
    إذا(ai->ai_family == ai_family) {
      Curl_printable_پتو(ai, buf, حجم(buf));
      نتيجو = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      إذا(نتيجو) {
        CURL_TRC_DNS(معلومات, "too many IP, cannot show");
        اذهب ٻاهريون;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(معلومات, "Host %s:%u resolved IPv%c: %s",
               شي->hostname, شي->پورٽ,
               (شي->dns_queries & CURL_DNSQ_AAAA) ? '6' : '4',
               (curlx_dyn_len(&tmp) ? curlx_dyn_ptr(&tmp) : "(none)"));
ٻاهريون:
  curlx_dyn_free(&tmp);
}
#جيڪڏهن_پورو /* CURLVERBOSE */

/* Process the receiving end of the thread queue, dispatching
 * processed items to their transfer when it can still be found
 * and has an `async` state present. Otherwise, destroy the item. */
فراغ Curl_async_thrdd_multi_process(بنية Curl_multi *multi)
{
  بنية Curl_easy *معلومات;
  فراغ *qitem;

  طالما(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    بنية async_thrdd_شي *شي = qitem;
    بنية Curl_resolv_async *async = عدم;

    معلومات = Curl_multi_get_easy(multi, شي->mid);
    إذا(معلومات)
      async = Curl_async_get(معلومات, شي->resolv_id);
    إذا(async) {
      بنية async_thrdd_شي **pdest = &async->thrdd.res_A;

#جيڪڏهن_وصف_ٿيل CURLRES_IPV6
      إذا(شي->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#جيڪڏهن_پورو
      إذا(!*pdest) {
        *pdest = شي;
        شي = عدم;
      }
      وإلا
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(معلومات);
    }
    async_thrdd_شي_free(شي);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_سيٽ_props(بنية Curl_multi *multi,
                                          uint32_t min_ڌاڳا,
                                          uint32_t max_ڌاڳا,
                                          uint32_t idle_وقت_ms)
{
  أرجع Curl_thrdq_سيٽ_props(multi->resolv_thrdq,
                              min_ڌاڳا, max_ڌاڳا, idle_وقت_ms);
}

ساكن CURLcode async_thrdd_query(بنية Curl_easy *معلومات,
                                  بنية Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  بنية async_thrdd_شي *شي;
  CURLcode نتيجو;

  شي = async_thrdd_شي_create(معلومات, async->id, dns_queries,
                                 async->peer->hostname, async->peer->پورٽ,
                                 async->transport);
  إذا(!شي) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  CURL_TRC_DNS(معلومات, "queueing query %s", شي->description);
  نتيجو = Curl_thrdq_send(معلومات->multi->resolv_thrdq, شي,
                           async_شي_description(شي), async->timeout_ms);
  إذا(نتيجو)
    اذهب ٻاهريون;
  شي = عدم;
  async->queries_ongoing++;

ٻاهريون:
  إذا(شي)
    async_thrdd_شي_free(شي);
  أرجع نتيجو;
}

CURLcode Curl_async_getaddrinfo(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async)
{
  CURLcode نتيجو = CURLE_FAILED_INIT;
  فراغ *resolver = عدم;

  إذا(async->queries_ongoing || async->پورو)
    أرجع CURLE_FAILED_INIT;

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  إذا((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    نتيجو = async_rr_init(معلومات, async);
    إذا(نتيجو)
      اذهب ٻاهريون;
    resolver = async->thrdd.rr.channel;
  }
#جيڪڏهن_پورو

  نتيجو = Curl_resolv_announce_شروعات(معلومات, resolver);
  إذا(نتيجو)
    أرجع نتيجو;

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR_ARES
  إذا(resolver) {
    نتيجو = async_rr_شروعات(معلومات, async);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  إذا((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    نتيجو = async_thrdd_query(معلومات, async, CURL_DNSQ_AAAA);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
#جيڪڏهن_پورو
  إذا(async->dns_queries & CURL_DNSQ_A) {
    نتيجو = async_thrdd_query(معلومات, async, CURL_DNSQ_A);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  CURL_THRDQ_TRACE(معلومات->multi->resolv_thrdq, معلومات);

ٻاهريون:
  إذا(!async->queries_ongoing)
    async->پورو = TRUE;

  إذا(نتيجو)
    CURL_TRC_DNS(معلومات, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->پورٽ, (صحيح)نتيجو);
  أرجع نتيجو;
}

CURLcode Curl_async_pollset(بنية Curl_easy *معلومات,
                            بنية Curl_resolv_async *async,
                            بنية easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(معلومات, async);
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR_ARES
  إذا(async->thrdd.rr.channel) {
    CURLcode نتيجو = Curl_ares_pollset(معلومات, async->thrdd.rr.channel, ps);
    إذا(نتيجو)
      أرجع نتيجو;
    timeout_ms = Curl_ares_timeout_ms(معلومات, async, async->thrdd.rr.channel);
  }
#وإلا
  (فراغ)ps;
#جيڪڏهن_پورو

  إذا(!async->پورو) {
    ثابت بنية curltime *pnow = Curl_pgrs_now(معلومات);
#جيڪڏهن_وصف_نه_ٿيل ENABLE_اندروني_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->شروعات);
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
#جيڪڏهن_پورو
    Curl_expire_سيٽ(معلومات, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  أرجع CURLE_OK;
}

#إذا defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ساكن bool async_thrdd_شي_missing_scope(بنية Curl_easy *معلومات,
                                           بنية async_thrdd_شي *شي)
{
  ثابت بنية Curl_addrinfo *ai;

  /* scope id already globally set */
  إذا(معلومات->conn && معلومات->conn->scope_id)
    أرجع FALSE;

  لكل(ai = شي->res; ai; ai = ai->ai_ايندڙ) {
    إذا(ai->ai_family == AF_INET6) {
      بنية sockaddr_in6 *sa6 = (فراغ *)ai->ai_addr;
      إذا(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        أرجع TRUE;
    }
  }
  أرجع FALSE;
}

ساكن فراغ async_thrdd_شي_strip_results(بنية async_thrdd_شي *شي,
                                           صحيح ai_family)
{
  بنية Curl_addrinfo *ai = شي->res, **panchor = &شي->res;
  طالما(ai) {
    إذا(ai->ai_family == ai_family) {
      *panchor = ai->ai_ايندڙ;
      ai->ai_ايندڙ = عدم;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    وإلا {
      panchor = &ai->ai_ايندڙ;
      ai = ai->ai_ايندڙ;
    }
  }
}

#جيڪڏهن_پورو /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

ساكن CURLcode async_thrdd_check_پورو(بنية Curl_easy *معلومات,
                                       بنية Curl_resolv_async *async)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;

  (فراغ)معلومات;
  إذا(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_شي(معلومات, thrdd->res_A));
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
       async_thrdd_شي_missing_scope(معلومات, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      بنية async_thrdd_شي *شي = thrdd->res_AAAA;
      CURLcode نتيجو;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = عدم;
      شي->dns_queries |= CURL_DNSQ_A;
      إذا(شي->res) {
        Curl_freeaddrinfo(شي->res);
        شي->res = عدم;
      }

      CURL_TRC_DNS(معلومات, "re-queueing query %s for AF_UNSPEC resolve",
                   شي->description);
      نتيجو = Curl_thrdq_send(معلومات->multi->resolv_thrdq, شي,
                               async_شي_description(شي),
                               async->timeout_ms);
      إذا(نتيجو) {
        async_thrdd_شي_free(شي);
        أرجع نتيجو;
      }
      async->queries_ongoing++;
      أرجع CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_شي_strip_results(thrdd->res_AAAA, AF_INET);
#جيڪڏهن_پورو /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_شي(معلومات, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = عدم;
    thrdd->processed_AAAA = TRUE;
  }

  إذا(async->queries_ongoing)
    أرجع CURLE_AGAIN;
  async->پورو = TRUE;
  أرجع CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_نتيجو(بنية Curl_easy *معلومات,
                                بنية Curl_resolv_async *async,
                                بنية Curl_dns_داخلا **pdns)
{
  بنية async_thrdd_ctx *thrdd = &async->thrdd;
  بنية Curl_dns_داخلا *dns = عدم;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = عدم;

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  إذا(thrdd->rr.channel)
    (فراغ)Curl_ares_perform(thrdd->rr.channel, 0);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل ENABLE_اندروني_WAKEUP
  Curl_async_thrdd_multi_process(معلومات->multi);
#جيڪڏهن_پورو

  نتيجو = async_thrdd_check_پورو(معلومات, async);
  إذا(نتيجو)
    أرجع نتيجو;

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

  إذا(async->نتيجو) {
    نتيجو = async->نتيجو;
    اذهب ٻاهريون;
  }

  إذا(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      معلومات, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    إذا(!dns) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR_ARES
  إذا(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(معلومات, async->httpsrr);
    dns = Curl_dnsc_mk_https(معلومات, &async->httpsrr, async->peer);
    إذا(!dns) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }
#جيڪڏهن_پورو

  إذا(dns) {
    *pdns = dns;
    dns = عدم;
  }
  CURL_THRDQ_TRACE(معلومات->multi->resolv_thrdq, معلومات);

ٻاهريون:
  Curl_dns_داخلا_unlink(معلومات, &dns);
  Curl_async_thrdd_shutdown(معلومات, async);
  إذا(!نتيجو && !*pdns)
    نتيجو = Curl_async_failed(معلومات, async, عدم);
  إذا(نتيجو &&
     (نتيجو != CURLE_COULDNT_RESOLVE_HOST) &&
     (نتيجو != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(معلومات, "Error %d resolving %s:%d",
                 (صحيح)نتيجو, async->peer->hostname, async->peer->پورٽ);
  }
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* USE_RESOLV_THREADED */
