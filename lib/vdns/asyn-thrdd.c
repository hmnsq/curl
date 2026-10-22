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
#اگر_تعریف_شدہ USE_RESOLV_THREADED

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_GETADDRINFO
#تعریف_کرو RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#ورنہ
#تعریف_کرو RESOLVER_ENOMEM  SOCKENOMEM
#اگر_ختم

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

#اگر_تعریف_شدہ USE_ARES
#شامل <ares.h>
#اگر_تعریف_شدہ USE_HTTPSRR
#تعریف_کرو USE_HTTPSRR_ARES  /* the combo */
#اگر_ختم
#اگر_ختم


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
صحیح Curl_async_global_init(خالی)
{
#اگر defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  اگر(ares_library_init(ARES_LIB_INIT_ALL)) {
    واپس CURLE_FAILED_INIT;
  }
#اگر_ختم
  واپس CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
خالی Curl_async_global_cleanup(خالی)
{
#اگر defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#اگر_ختم
}

#اگر_تعریف_شدہ CURLVERBOSE
#تعریف_کرو CURL_ASYN_شے_DESC_LEN   64
#تعریف_کرو async_شے_description(x)   (x)->description
#ورنہ
#تعریف_کرو async_شے_description(x)   تہی
#اگر_ختم

ساخت async_thrdd_شے {
  ساخت Curl_addrinfo *res;
#اگر_تعریف_شدہ CURLVERBOSE
  حرف description[CURL_ASYN_شے_DESC_LEN];
#اگر_ختم
  صحیح sockerr;
  uint32_t mid;
  uint32_t resolv_id;
  uint16_t پورٹ;
  uint8_t transport;
  uint8_t dns_queries;
  ثنائی_ہندسہ(negative); /* resolver answered that the name does not exist */
#اگر_تعریف_شدہ DEBUGBUILD
  uint32_t delay_ms;
  uint32_t delay_fail_ms;
  ثنائی_ہندسہ(dbg_negative);
#اگر_ختم
  حرف hostname[1];
};

/* Give up reference to add_ctx */
ساکن خالی async_thrdd_شے_destroy(ساخت async_thrdd_شے *شے)
{
  اگر(شے) {
    اگر(شے->res)
      Curl_freeaddrinfo(شے->res);
    curlx_free(شے);
  }
}

/* Initialize context for threaded resolver */
ساکن ساخت async_thrdd_شے *async_thrdd_شے_create(
  ساخت Curl_easy *ڈیٹا,
  uint32_t resolv_id, uint8_t dns_queries,
  ثابت حرف *hostname, uint16_t پورٹ,
  uint8_t transport)
{
  size_t hostlen = لڑی_لمبائی(hostname);
  ساخت async_thrdd_شے *شے;

  شے = curlx_calloc(1, حجم(*شے) + hostlen);
  اگر(!شے)
    واپس تہی;

  اگر(hostlen) /* null-terminator of name already in struct size */
    حافظہ_نقل(شے->hostname, hostname, hostlen);
  شے->mid = ڈیٹا->mid;
  شے->resolv_id = resolv_id;
  شے->dns_queries = dns_queries;
  شے->پورٹ = پورٹ;
  شے->transport = transport;

#اگر_تعریف_شدہ CURLVERBOSE
  curl_msnprintf(شے->description, حجم(شے->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 ڈیٹا->id, شے->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 شے->hostname, شے->پورٹ);
#اگر_ختم

#اگر_تعریف_شدہ DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_DELAY");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, UINT32_MAX)) {
        شے->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, UINT32_MAX)) {
        غیرعلامتی حرف c = 0;
        Curl_rand_بائٹیں(ڈیٹا, FALSE, &c, 1);
        شے->delay_fail_ms = (uint32_t)l + c;
      }
    }
    اگر(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      شے->dbg_negative = TRUE;
  }
#اگر_ختم

  واپس شے;
}

#اگر_تعریف_شدہ USE_HTTPSRR_ARES

ساکن خالی async_thrdd_rr_مکمل(خالی *user_ڈیٹا, ares_حالت_t حالت_2,
                                size_t timeouts,
                                ثابت ares_dns_ریکارڈ_t *dnsrec)
{
  ساخت Curl_resolv_async *async = user_ڈیٹا;
  ساخت async_thrdd_ctx *thrdd = async ? &async->thrdd : تہی;

  (خالی)timeouts;
  اگر(!thrdd)
    واپس;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->مکمل = !async->queries_ongoing;
  اگر((ARES_SUCCESS == حالت_2) && dnsrec)
    async->نتیجہ = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
ساکن CURLcode async_rr_init(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_resolv_async *async)
{
  ساخت async_thrdd_ctx *thrdd = &async->thrdd;
  صحیح حالت_2;

  (خالی)ڈیٹا;
  DEBUGASSERT(!thrdd->rr.channel);
  حالت_2 = ares_init_options(&thrdd->rr.channel, تہی, 0);
  اگر(حالت_2 != ARES_SUCCESS) {
    thrdd->rr.channel = تہی;
    واپس CURLE_FAILED_INIT;
  }
#اگر_تعریف_شدہ DEBUGBUILD
  اگر(getenv("CURL_DNS_SERVER")) {
    ثابت حرف *servers = getenv("CURL_DNS_SERVER");
    حالت_2 = ares_سیٹ_servers_ports_csv(thrdd->rr.channel, servers);
    اگر(حالت_2)
      واپس CURLE_FAILED_INIT;
  }
#اگر_ختم
  واپس CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
ساکن CURLcode async_rr_آغاز(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_resolv_async *async)
{
  ساخت async_thrdd_ctx *thrdd = &async->thrdd;
  حرف *https_name = تہی;
  CURLcode نتیجہ = CURLE_OK;

  اگر(async->peer->پورٹ != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->پورٹ, async->peer->hostname);
    اگر(!https_name) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_اندر, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_مکمل, async, تہی);
  CURL_TRC_DNS(ڈیٹا, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

برآمد:
  curlx_free(https_name);
  واپس نتیجہ;
}
#اگر_ختم

خالی Curl_async_thrdd_shutdown(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(ڈیٹا, async);
}

ساخت async_thrdd_مطابقت_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

ساکن bool async_thrdd_مطابقت_شے(خالی *qitem, خالی *مطابقت_ڈیٹا)
{
  ثابت ساخت async_thrdd_مطابقت_ctx *ctx = مطابقت_ڈیٹا;
  ساخت async_thrdd_شے *شے = qitem;
  واپس (شے->mid == ctx->mid) && (شے->resolv_id == ctx->resolv_id);
}

خالی Curl_async_thrdd_destroy(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_resolv_async *async)
{
  (خالی)ڈیٹا;
  اگر(async->queries_ongoing && !async->مکمل &&
     ڈیٹا->multi && ڈیٹا->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    ساخت async_thrdd_مطابقت_ctx mctx;
    mctx.mid = ڈیٹا->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(ڈیٹا->multi->resolv_thrdq,
                     async_thrdd_مطابقت_شے, &mctx);
  }
#اگر_تعریف_شدہ USE_HTTPSRR_ARES
  اگر(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = تہی;
  }
#اگر_ختم
  async_thrdd_شے_destroy(async->thrdd.res_A);
  async->thrdd.res_A = تہی;
  async_thrdd_شے_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = تہی;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(ساخت Curl_easy *ڈیٹا, uint32_t resolv_id,
                          ساخت Curl_dns_اندراج **pdns)
{
  ساخت Curl_resolv_async *async = Curl_async_get(ڈیٹا, resolv_id);
  ساخت async_thrdd_ctx *thrdd = async ? &async->thrdd : تہی;
  timediff_t milli, ms;
  CURLcode نتیجہ = CURLE_AGAIN;

  اگر(!thrdd)
    واپس CURLE_FAILED_INIT;

  جب(نتیجہ == CURLE_AGAIN) {
    جب(async->queries_ongoing && !async->مکمل) {
      Curl_async_thrdd_multi_process(ڈیٹا->multi);
      اگر(async->مکمل)
        توڑو;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &async->آغاز);
      اگر(ms < 3)
        milli = 0;
      ورنہ اگر(ms <= 50)
        milli = ms / 3;
      ورنہ اگر(ms <= 250)
        milli = 50;
      ورنہ
        milli = 200;
      CURL_TRC_DNS(ڈیٹا, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    نتیجہ = Curl_async_take_نتیجہ(ڈیٹا, async, pdns);
  }
  واپس نتیجہ;
}

#اگر_تعریف_شدہ HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
ساکن bool gai_negative(صحیح rc)
{
  انتخاب(rc) {
#اگر_تعریف_شدہ EAI_NONAME
  حالت EAI_NONAME:
#اگر_ختم
#اگر defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  حالت EAI_NODATA:
#اگر_ختم
    واپس TRUE;
  طےشدہ:
    واپس FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
ساکن خالی async_thrdd_شے_process(خالی *arg)
{
  ساخت async_thrdd_شے *شے = arg;
  ساخت addrinfo hints;
  حرف service[12];
  صحیح pf = PF_INET;
  صحیح rc;

#اگر_تعریف_شدہ DEBUGBUILD
  اگر(شے->delay_ms) {
    curlx_wait_ms(شے->delay_ms);
  }
  اگر(شے->delay_fail_ms) {
    curlx_wait_ms(شے->delay_fail_ms);
    شے->negative = شے->dbg_negative;
    واپس;
  }
#اگر_ختم

  memset(&hints, 0, حجم(hints));
#اگر_تعریف_شدہ CURLRES_IPV6
  اگر(شے->dns_queries & CURL_DNSQ_AAAA) {
    pf = (شے->dns_queries & CURL_DNSQ_A) ? PF_UNSPEC : PF_INET6;
  }
#اگر_ختم
  hints.ai_family = pf;
  hints.ai_socktype = Curl_socktype_for_transport(شے->transport);
  hints.ai_protocol = Curl_protocol_for_transport(شے->transport);
#اگر_تعریف_شدہ __APPLE__
  /* If we leave `ai_flags == 0` then macOS is looking for IPV4MAPPED
   * when doing AAAA queries. We do not want this "help". */
  hints.ai_flags = AI_ADDRCONFIG;
#اگر_ختم

  curl_msnprintf(service, حجم(service), "%u", شے->پورٹ);
#اگر_تعریف_شدہ AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#اگر_ختم

  rc = Curl_getaddrinfo_ex(شے->hostname, service, &hints, &شے->res);
  اگر(rc) {
    شے->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    اگر(شے->sockerr == 0)
      شے->sockerr = RESOLVER_ENOMEM;
    شے->negative = gai_negative(rc);
  }
  ورنہ {
    Curl_addrinfo_سیٹ_پورٹ(شے->res, شے->پورٹ);
  }
}

#ورنہ /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
ساکن خالی async_thrdd_شے_process(خالی *arg)
{
  ساخت async_thrdd_شے *شے = arg;

#اگر_تعریف_شدہ DEBUGBUILD
  اگر(شے->delay_ms) {
    curlx_wait_ms(شے->delay_ms);
  }
  اگر(شے->delay_fail_ms) {
    curlx_wait_ms(شے->delay_fail_ms);
    شے->negative = شے->dbg_negative;
    واپس;
  }
#اگر_ختم
  شے->res = Curl_ipv4_resolve_r(شے->hostname, شے->پورٹ);
  اگر(!شے->res) {
    شے->sockerr = SOCKERRNO;
    اگر(شے->sockerr == 0)
      شے->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    شے->negative = TRUE;
  }
}

#اگر_ختم /* HAVE_GETADDRINFO */

#اگر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
ساکن خالی async_thrdd_event(ثابت ساخت curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              خالی *user_ڈیٹا)
{
  ساخت Curl_multi *multi = user_ڈیٹا;
  (خالی)tqueue;
  انتخاب(ev) {
  حالت CURL_THRDQ_EV_شے_مکمل:
    Curl_multi_wakeup_اندرونی(multi);
    توڑو;
  طےشدہ:
    توڑو;
  }
}
#ورنہ
#تعریف_کرو async_thrdd_event   تہی
#اگر_ختم

ساکن خالی async_thrdd_شے_free(خالی *شے)
{
  async_thrdd_شے_destroy(شے);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(ساخت Curl_multi *multi,
                                     uint32_t min_دھاگے,
                                     uint32_t max_دھاگے,
                                     uint32_t idle_وقت_ms)
{
  CURLcode نتیجہ;
  DEBUGASSERT(!multi->resolv_thrdq);
  نتیجہ = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_دھاگے, max_دھاگے, idle_وقت_ms,
                             async_thrdd_شے_free,
                             async_thrdd_شے_process,
                             async_thrdd_event,
                             multi);
#اگر_تعریف_شدہ DEBUGBUILD
  اگر(!نتیجہ) {
    ثابت حرف *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, UINT32_MAX)) {
        نتیجہ = Curl_async_thrdd_multi_سیٹ_props(
          multi, min_دھاگے, (uint32_t)l, idle_وقت_ms);
      }
    }
  }
#اگر_ختم
  واپس نتیجہ;
}

/* Tear down the thread queue, joining active threads or detaching them */
خالی Curl_async_thrdd_multi_destroy(ساخت Curl_multi *multi, bool join)
{
  اگر(multi->resolv_thrdq) {
#اگر_تعریف_شدہ CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#اگر_ختم
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = تہی;
  }
}

#اگر_تعریف_شدہ CURLVERBOSE
ساکن خالی async_thrdd_report_شے(ساخت Curl_easy *ڈیٹا,
                                    ساخت async_thrdd_شے *شے)
{
  حرف buf[MAX_IPADR_LEN];
  ساخت dynbuf tmp;
  ثابت حرف *sep = "";
  ثابت ساخت Curl_addrinfo *ai = شے->res;
  CURLcode نتیجہ;
  صحیح ai_family;
#اگر_تعریف_شدہ USE_IPV6
  ai_family = (شے->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#ورنہ
  ai_family = AF_INET;
#اگر_ختم

  اگر(!CURL_TRC_DNS_is_verbose(ڈیٹا))
    واپس;

  curlx_dyn_init(&tmp, 1024);
  برائے(; ai; ai = ai->ai_اگلا) {
    اگر(ai->ai_family == ai_family) {
      Curl_printable_پتہ(ai, buf, حجم(buf));
      نتیجہ = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      اگر(نتیجہ) {
        CURL_TRC_DNS(ڈیٹا, "too many IP, cannot show");
        جاؤ برآمد;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(ڈیٹا, "Host %s:%u resolved IPv%c: %s",
               شے->hostname, شے->پورٹ,
               (شے->dns_queries & CURL_DNSQ_AAAA) ? '6' : '4',
               (curlx_dyn_len(&tmp) ? curlx_dyn_ptr(&tmp) : "(none)"));
برآمد:
  curlx_dyn_free(&tmp);
}
#اگر_ختم /* CURLVERBOSE */

/* Process the receiving end of the thread queue, dispatching
 * processed items to their transfer when it can still be found
 * and has an `async` state present. Otherwise, destroy the item. */
خالی Curl_async_thrdd_multi_process(ساخت Curl_multi *multi)
{
  ساخت Curl_easy *ڈیٹا;
  خالی *qitem;

  جب(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    ساخت async_thrdd_شے *شے = qitem;
    ساخت Curl_resolv_async *async = تہی;

    ڈیٹا = Curl_multi_get_easy(multi, شے->mid);
    اگر(ڈیٹا)
      async = Curl_async_get(ڈیٹا, شے->resolv_id);
    اگر(async) {
      ساخت async_thrdd_شے **pdest = &async->thrdd.res_A;

#اگر_تعریف_شدہ CURLRES_IPV6
      اگر(شے->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#اگر_ختم
      اگر(!*pdest) {
        *pdest = شے;
        شے = تہی;
      }
      ورنہ
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(ڈیٹا);
    }
    async_thrdd_شے_free(شے);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_سیٹ_props(ساخت Curl_multi *multi,
                                          uint32_t min_دھاگے,
                                          uint32_t max_دھاگے,
                                          uint32_t idle_وقت_ms)
{
  واپس Curl_thrdq_سیٹ_props(multi->resolv_thrdq,
                              min_دھاگے, max_دھاگے, idle_وقت_ms);
}

ساکن CURLcode async_thrdd_query(ساخت Curl_easy *ڈیٹا,
                                  ساخت Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  ساخت async_thrdd_شے *شے;
  CURLcode نتیجہ;

  شے = async_thrdd_شے_create(ڈیٹا, async->id, dns_queries,
                                 async->peer->hostname, async->peer->پورٹ,
                                 async->transport);
  اگر(!شے) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  CURL_TRC_DNS(ڈیٹا, "queueing query %s", شے->description);
  نتیجہ = Curl_thrdq_send(ڈیٹا->multi->resolv_thrdq, شے,
                           async_شے_description(شے), async->timeout_ms);
  اگر(نتیجہ)
    جاؤ برآمد;
  شے = تہی;
  async->queries_ongoing++;

برآمد:
  اگر(شے)
    async_thrdd_شے_free(شے);
  واپس نتیجہ;
}

CURLcode Curl_async_getaddrinfo(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_resolv_async *async)
{
  CURLcode نتیجہ = CURLE_FAILED_INIT;
  خالی *resolver = تہی;

  اگر(async->queries_ongoing || async->مکمل)
    واپس CURLE_FAILED_INIT;

#اگر_تعریف_شدہ USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  اگر((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    نتیجہ = async_rr_init(ڈیٹا, async);
    اگر(نتیجہ)
      جاؤ برآمد;
    resolver = async->thrdd.rr.channel;
  }
#اگر_ختم

  نتیجہ = Curl_resolv_announce_آغاز(ڈیٹا, resolver);
  اگر(نتیجہ)
    واپس نتیجہ;

#اگر_تعریف_شدہ USE_HTTPSRR_ARES
  اگر(resolver) {
    نتیجہ = async_rr_آغاز(ڈیٹا, async);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
#اگر_ختم

#اگر_تعریف_شدہ CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  اگر((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    نتیجہ = async_thrdd_query(ڈیٹا, async, CURL_DNSQ_AAAA);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
#اگر_ختم
  اگر(async->dns_queries & CURL_DNSQ_A) {
    نتیجہ = async_thrdd_query(ڈیٹا, async, CURL_DNSQ_A);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  CURL_THRDQ_TRACE(ڈیٹا->multi->resolv_thrdq, ڈیٹا);

برآمد:
  اگر(!async->queries_ongoing)
    async->مکمل = TRUE;

  اگر(نتیجہ)
    CURL_TRC_DNS(ڈیٹا, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->پورٹ, (صحیح)نتیجہ);
  واپس نتیجہ;
}

CURLcode Curl_async_pollset(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_resolv_async *async,
                            ساخت easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(ڈیٹا, async);
#اگر_تعریف_شدہ USE_HTTPSRR_ARES
  اگر(async->thrdd.rr.channel) {
    CURLcode نتیجہ = Curl_ares_pollset(ڈیٹا, async->thrdd.rr.channel, ps);
    اگر(نتیجہ)
      واپس نتیجہ;
    timeout_ms = Curl_ares_timeout_ms(ڈیٹا, async, async->thrdd.rr.channel);
  }
#ورنہ
  (خالی)ps;
#اگر_ختم

  اگر(!async->مکمل) {
    ثابت ساخت curltime *pnow = Curl_pgrs_now(ڈیٹا);
#اگر_غیر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->آغاز);
    اگر(elapsed_ms < 3)
      stutter_ms = 1;
    ورنہ اگر(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    ورنہ اگر(elapsed_ms <= 250)
      stutter_ms = 50;
    ورنہ
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#ورنہ
    اگر(async->queries_ongoing &&
       !Curl_thrdq_check_started(ڈیٹا->multi->resolv_thrdq)) {
      /* The queue has items but starting a worker thread to process
         them just failed again; expire soon to check once more,
         instead of sleeping on the full resolve timeout. */
      CURL_TRC_DNS(ڈیٹا, "resolver thread start failed again, retrying");
      timeout_ms = CURLMIN(100, timeout_ms);
    }
#اگر_ختم
    Curl_expire_سیٹ(ڈیٹا, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  واپس CURLE_OK;
}

#اگر defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ساکن bool async_thrdd_شے_missing_scope(ساخت Curl_easy *ڈیٹا,
                                           ساخت async_thrdd_شے *شے)
{
  ثابت ساخت Curl_addrinfo *ai;

  /* scope id already globally set */
  اگر(ڈیٹا->conn && ڈیٹا->conn->scope_id)
    واپس FALSE;

  برائے(ai = شے->res; ai; ai = ai->ai_اگلا) {
    اگر(ai->ai_family == AF_INET6) {
      ساخت sockaddr_in6 *sa6 = (خالی *)ai->ai_addr;
      اگر(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        واپس TRUE;
    }
  }
  واپس FALSE;
}

ساکن خالی async_thrdd_شے_strip_results(ساخت async_thrdd_شے *شے,
                                           صحیح ai_family)
{
  ساخت Curl_addrinfo *ai = شے->res, **panchor = &شے->res;
  جب(ai) {
    اگر(ai->ai_family == ai_family) {
      *panchor = ai->ai_اگلا;
      ai->ai_اگلا = تہی;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    ورنہ {
      panchor = &ai->ai_اگلا;
      ai = ai->ai_اگلا;
    }
  }
}

#اگر_ختم /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

ساکن CURLcode async_thrdd_check_مکمل(ساخت Curl_easy *ڈیٹا,
                                       ساخت Curl_resolv_async *async)
{
  ساخت async_thrdd_ctx *thrdd = &async->thrdd;

  (خالی)ڈیٹا;
  اگر(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_شے(ڈیٹا, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = تہی;
    thrdd->processed_A = TRUE;
  }

  اگر(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#اگر defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    اگر(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_شے_missing_scope(ڈیٹا, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      ساخت async_thrdd_شے *شے = thrdd->res_AAAA;
      CURLcode نتیجہ;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = تہی;
      شے->dns_queries |= CURL_DNSQ_A;
      اگر(شے->res) {
        Curl_freeaddrinfo(شے->res);
        شے->res = تہی;
      }

      CURL_TRC_DNS(ڈیٹا, "re-queueing query %s for AF_UNSPEC resolve",
                   شے->description);
      نتیجہ = Curl_thrdq_send(ڈیٹا->multi->resolv_thrdq, شے,
                               async_شے_description(شے),
                               async->timeout_ms);
      اگر(نتیجہ) {
        async_thrdd_شے_free(شے);
        واپس نتیجہ;
      }
      async->queries_ongoing++;
      واپس CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_شے_strip_results(thrdd->res_AAAA, AF_INET);
#اگر_ختم /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_شے(ڈیٹا, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = تہی;
    thrdd->processed_AAAA = TRUE;
  }

  اگر(async->queries_ongoing)
    واپس CURLE_AGAIN;
  async->مکمل = TRUE;
  واپس CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_نتیجہ(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_resolv_async *async,
                                ساخت Curl_dns_اندراج **pdns)
{
  ساخت async_thrdd_ctx *thrdd = &async->thrdd;
  ساخت Curl_dns_اندراج *dns = تہی;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = تہی;

#اگر_تعریف_شدہ USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  اگر(thrdd->rr.channel)
    (خالی)Curl_ares_perform(thrdd->rr.channel, 0);
#اگر_ختم
#اگر_غیر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  Curl_async_thrdd_multi_process(ڈیٹا->multi);
#اگر_ختم

  نتیجہ = async_thrdd_check_مکمل(ڈیٹا, async);
  اگر(نتیجہ)
    واپس نتیجہ;

  Curl_expire_clear(ڈیٹا, EXPIRE_ASYNC_NAME);

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

  اگر(async->نتیجہ) {
    نتیجہ = async->نتیجہ;
    جاؤ برآمد;
  }

  اگر(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      ڈیٹا, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    اگر(!dns) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }

#اگر_تعریف_شدہ USE_HTTPSRR_ARES
  اگر(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(ڈیٹا, async->httpsrr);
    dns = Curl_dnsc_mk_https(ڈیٹا, &async->httpsrr, async->peer);
    اگر(!dns) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }
#اگر_ختم

  اگر(dns) {
    *pdns = dns;
    dns = تہی;
  }
  CURL_THRDQ_TRACE(ڈیٹا->multi->resolv_thrdq, ڈیٹا);

برآمد:
  Curl_dns_اندراج_unlink(ڈیٹا, &dns);
  Curl_async_thrdd_shutdown(ڈیٹا, async);
  اگر(!نتیجہ && !*pdns)
    نتیجہ = Curl_async_failed(ڈیٹا, async, تہی);
  اگر(نتیجہ &&
     (نتیجہ != CURLE_COULDNT_RESOLVE_HOST) &&
     (نتیجہ != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(ڈیٹا, "Error %d resolving %s:%d",
                 (صحیح)نتیجہ, async->peer->hostname, async->peer->پورٹ);
  }
  واپس نتیجہ;
}

#اگر_ختم /* USE_RESOLV_THREADED */
