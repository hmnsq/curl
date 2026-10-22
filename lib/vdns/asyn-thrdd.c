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
#అన్తర్భావ "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#నిర్వచితమైతే USE_RESOLV_THREADED

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_GETADDRINFO
#నిర్వచించు RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#లేకపోతే
#నిర్వచించు RESOLVER_ENOMEM  SOCKENOMEM
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "curl_threads.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "thrdqueue.h"
#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "vdns/httpsrr.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curlx/wait.h"

#నిర్వచితమైతే USE_ARES
#అన్తర్భావ <ares.h>
#నిర్వచితమైతే USE_HTTPSRR
#నిర్వచించు USE_HTTPSRR_ARES  /* the combo */
#అయితే_ముగింపు
#అయితే_ముగింపు


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
పూర్ణాంకం Curl_async_global_init(శూన్య)
{
#ఒకవేళ defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ఒకవేళ(ares_library_init(ARES_LIB_INIT_ALL)) {
    తిరిగి CURLE_FAILED_INIT;
  }
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
శూన్య Curl_async_global_cleanup(శూన్య)
{
#ఒకవేళ defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#అయితే_ముగింపు
}

#నిర్వచితమైతే CURLVERBOSE
#నిర్వచించు CURL_ASYN_అంశం_DESC_LEN   64
#నిర్వచించు async_అంశం_description(x)   (x)->description
#లేకపోతే
#నిర్వచించు async_అంశం_description(x)   ఖాళీ
#అయితే_ముగింపు

నిర్మాణం async_thrdd_అంశం {
  నిర్మాణం Curl_addrinfo *res;
#నిర్వచితమైతే CURLVERBOSE
  అక్షరం description[CURL_ASYN_అంశం_DESC_LEN];
#అయితే_ముగింపు
  పూర్ణాంకం sockerr;
  uint32_t mid;
  uint32_t resolv_id;
  uint16_t పోర్టు;
  uint8_t transport;
  uint8_t dns_queries;
  బిట్టు(negative); /* resolver answered that the name does not exist */
#నిర్వచితమైతే DEBUGBUILD
  uint32_t delay_ms;
  uint32_t delay_fail_ms;
  బిట్టు(dbg_negative);
#అయితే_ముగింపు
  అక్షరం hostname[1];
};

/* Give up reference to add_ctx */
స్థిర శూన్య async_thrdd_అంశం_destroy(నిర్మాణం async_thrdd_అంశం *అంశం)
{
  ఒకవేళ(అంశం) {
    ఒకవేళ(అంశం->res)
      Curl_freeaddrinfo(అంశం->res);
    curlx_free(అంశం);
  }
}

/* Initialize context for threaded resolver */
స్థిర నిర్మాణం async_thrdd_అంశం *async_thrdd_అంశం_create(
  నిర్మాణం Curl_easy *దత్తాంశం,
  uint32_t resolv_id, uint8_t dns_queries,
  స్థిరరాశి అక్షరం *hostname, uint16_t పోర్టు,
  uint8_t transport)
{
  size_t hostlen = అక్షరశ్రేణి_పొడవు(hostname);
  నిర్మాణం async_thrdd_అంశం *అంశం;

  అంశం = curlx_calloc(1, పరిమాణం(*అంశం) + hostlen);
  ఒకవేళ(!అంశం)
    తిరిగి ఖాళీ;

  ఒకవేళ(hostlen) /* null-terminator of name already in struct size */
    జ్ఞాపకం_నకలు(అంశం->hostname, hostname, hostlen);
  అంశం->mid = దత్తాంశం->mid;
  అంశం->resolv_id = resolv_id;
  అంశం->dns_queries = dns_queries;
  అంశం->పోర్టు = పోర్టు;
  అంశం->transport = transport;

#నిర్వచితమైతే CURLVERBOSE
  curl_msnprintf(అంశం->description, పరిమాణం(అంశం->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 దత్తాంశం->id, అంశం->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 అంశం->hostname, అంశం->పోర్టు);
#అయితే_ముగింపు

#నిర్వచితమైతే DEBUGBUILD
  {
    స్థిరరాశి అక్షరం *p = getenv("CURL_DBG_RESOLV_DELAY");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, UINT32_MAX)) {
        అంశం->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, UINT32_MAX)) {
        గుర్తులేని అక్షరం c = 0;
        Curl_rand_బైట్లు(దత్తాంశం, FALSE, &c, 1);
        అంశం->delay_fail_ms = (uint32_t)l + c;
      }
    }
    ఒకవేళ(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      అంశం->dbg_negative = TRUE;
  }
#అయితే_ముగింపు

  తిరిగి అంశం;
}

#నిర్వచితమైతే USE_HTTPSRR_ARES

స్థిర శూన్య async_thrdd_rr_పూర్తి(శూన్య *user_దత్తాంశం, ares_స్థితి_t స్థితి_2,
                                size_t timeouts,
                                స్థిరరాశి ares_dns_రికార్డు_t *dnsrec)
{
  నిర్మాణం Curl_resolv_async *async = user_దత్తాంశం;
  నిర్మాణం async_thrdd_ctx *thrdd = async ? &async->thrdd : ఖాళీ;

  (శూన్య)timeouts;
  ఒకవేళ(!thrdd)
    తిరిగి;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->పూర్తి = !async->queries_ongoing;
  ఒకవేళ((ARES_SUCCESS == స్థితి_2) && dnsrec)
    async->ఫలితం = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
స్థిర CURLcode async_rr_init(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_resolv_async *async)
{
  నిర్మాణం async_thrdd_ctx *thrdd = &async->thrdd;
  పూర్ణాంకం స్థితి_2;

  (శూన్య)దత్తాంశం;
  DEBUGASSERT(!thrdd->rr.channel);
  స్థితి_2 = ares_init_options(&thrdd->rr.channel, ఖాళీ, 0);
  ఒకవేళ(స్థితి_2 != ARES_SUCCESS) {
    thrdd->rr.channel = ఖాళీ;
    తిరిగి CURLE_FAILED_INIT;
  }
#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(getenv("CURL_DNS_SERVER")) {
    స్థిరరాశి అక్షరం *servers = getenv("CURL_DNS_SERVER");
    స్థితి_2 = ares_సమితి_servers_ports_csv(thrdd->rr.channel, servers);
    ఒకవేళ(స్థితి_2)
      తిరిగి CURLE_FAILED_INIT;
  }
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
స్థిర CURLcode async_rr_ప్రారంభం(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_resolv_async *async)
{
  నిర్మాణం async_thrdd_ctx *thrdd = &async->thrdd;
  అక్షరం *https_name = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(async->peer->పోర్టు != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->పోర్టు, async->peer->hostname);
    ఒకవేళ(!https_name) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_లోపల, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_పూర్తి, async, ఖాళీ);
  CURL_TRC_DNS(దత్తాంశం, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

బహిర్గతం:
  curlx_free(https_name);
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

శూన్య Curl_async_thrdd_shutdown(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(దత్తాంశం, async);
}

నిర్మాణం async_thrdd_సరిపోలిక_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

స్థిర bool async_thrdd_సరిపోలిక_అంశం(శూన్య *qitem, శూన్య *సరిపోలిక_దత్తాంశం)
{
  స్థిరరాశి నిర్మాణం async_thrdd_సరిపోలిక_ctx *ctx = సరిపోలిక_దత్తాంశం;
  నిర్మాణం async_thrdd_అంశం *అంశం = qitem;
  తిరిగి (అంశం->mid == ctx->mid) && (అంశం->resolv_id == ctx->resolv_id);
}

శూన్య Curl_async_thrdd_destroy(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_resolv_async *async)
{
  (శూన్య)దత్తాంశం;
  ఒకవేళ(async->queries_ongoing && !async->పూర్తి &&
     దత్తాంశం->multi && దత్తాంశం->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    నిర్మాణం async_thrdd_సరిపోలిక_ctx mctx;
    mctx.mid = దత్తాంశం->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(దత్తాంశం->multi->resolv_thrdq,
                     async_thrdd_సరిపోలిక_అంశం, &mctx);
  }
#నిర్వచితమైతే USE_HTTPSRR_ARES
  ఒకవేళ(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = ఖాళీ;
  }
#అయితే_ముగింపు
  async_thrdd_అంశం_destroy(async->thrdd.res_A);
  async->thrdd.res_A = ఖాళీ;
  async_thrdd_అంశం_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = ఖాళీ;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(నిర్మాణం Curl_easy *దత్తాంశం, uint32_t resolv_id,
                          నిర్మాణం Curl_dns_నమోదు **pdns)
{
  నిర్మాణం Curl_resolv_async *async = Curl_async_get(దత్తాంశం, resolv_id);
  నిర్మాణం async_thrdd_ctx *thrdd = async ? &async->thrdd : ఖాళీ;
  timediff_t milli, ms;
  CURLcode ఫలితం = CURLE_AGAIN;

  ఒకవేళ(!thrdd)
    తిరిగి CURLE_FAILED_INIT;

  అయితే(ఫలితం == CURLE_AGAIN) {
    అయితే(async->queries_ongoing && !async->పూర్తి) {
      Curl_async_thrdd_multi_process(దత్తాంశం->multi);
      ఒకవేళ(async->పూర్తి)
        ఆపు;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &async->ప్రారంభం);
      ఒకవేళ(ms < 3)
        milli = 0;
      లేకపోతే ఒకవేళ(ms <= 50)
        milli = ms / 3;
      లేకపోతే ఒకవేళ(ms <= 250)
        milli = 50;
      లేకపోతే
        milli = 200;
      CURL_TRC_DNS(దత్తాంశం, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    ఫలితం = Curl_async_take_ఫలితం(దత్తాంశం, async, pdns);
  }
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
స్థిర bool gai_negative(పూర్ణాంకం rc)
{
  ఎంపిక(rc) {
#నిర్వచితమైతే EAI_NONAME
  స్థితి EAI_NONAME:
#అయితే_ముగింపు
#ఒకవేళ defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  స్థితి EAI_NODATA:
#అయితే_ముగింపు
    తిరిగి TRUE;
  అప్రమేయ:
    తిరిగి FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
స్థిర శూన్య async_thrdd_అంశం_process(శూన్య *arg)
{
  నిర్మాణం async_thrdd_అంశం *అంశం = arg;
  నిర్మాణం addrinfo hints;
  అక్షరం service[12];
  పూర్ణాంకం pf = PF_INET;
  పూర్ణాంకం rc;

#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(అంశం->delay_ms) {
    curlx_wait_ms(అంశం->delay_ms);
  }
  ఒకవేళ(అంశం->delay_fail_ms) {
    curlx_wait_ms(అంశం->delay_fail_ms);
    అంశం->negative = అంశం->dbg_negative;
    తిరిగి;
  }
#అయితే_ముగింపు

  memset(&hints, 0, పరిమాణం(hints));
#నిర్వచితమైతే CURLRES_IPV6
  ఒకవేళ(అంశం->dns_queries & CURL_DNSQ_AAAA) {
    pf = (అంశం->dns_queries & CURL_DNSQ_A) ? PF_UNSPEC : PF_INET6;
  }
#అయితే_ముగింపు
  hints.ai_family = pf;
  hints.ai_socktype = Curl_socktype_for_transport(అంశం->transport);
  hints.ai_protocol = Curl_protocol_for_transport(అంశం->transport);
#నిర్వచితమైతే __APPLE__
  /* If we leave `ai_flags == 0` then macOS is looking for IPV4MAPPED
   * when doing AAAA queries. We do not want this "help". */
  hints.ai_flags = AI_ADDRCONFIG;
#అయితే_ముగింపు

  curl_msnprintf(service, పరిమాణం(service), "%u", అంశం->పోర్టు);
#నిర్వచితమైతే AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#అయితే_ముగింపు

  rc = Curl_getaddrinfo_ex(అంశం->hostname, service, &hints, &అంశం->res);
  ఒకవేళ(rc) {
    అంశం->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    ఒకవేళ(అంశం->sockerr == 0)
      అంశం->sockerr = RESOLVER_ENOMEM;
    అంశం->negative = gai_negative(rc);
  }
  లేకపోతే {
    Curl_addrinfo_సమితి_పోర్టు(అంశం->res, అంశం->పోర్టు);
  }
}

#లేకపోతే /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
స్థిర శూన్య async_thrdd_అంశం_process(శూన్య *arg)
{
  నిర్మాణం async_thrdd_అంశం *అంశం = arg;

#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(అంశం->delay_ms) {
    curlx_wait_ms(అంశం->delay_ms);
  }
  ఒకవేళ(అంశం->delay_fail_ms) {
    curlx_wait_ms(అంశం->delay_fail_ms);
    అంశం->negative = అంశం->dbg_negative;
    తిరిగి;
  }
#అయితే_ముగింపు
  అంశం->res = Curl_ipv4_resolve_r(అంశం->hostname, అంశం->పోర్టు);
  ఒకవేళ(!అంశం->res) {
    అంశం->sockerr = SOCKERRNO;
    ఒకవేళ(అంశం->sockerr == 0)
      అంశం->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    అంశం->negative = TRUE;
  }
}

#అయితే_ముగింపు /* HAVE_GETADDRINFO */

#నిర్వచితమైతే ENABLE_అంతర్గత_WAKEUP
స్థిర శూన్య async_thrdd_event(స్థిరరాశి నిర్మాణం curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              శూన్య *user_దత్తాంశం)
{
  నిర్మాణం Curl_multi *multi = user_దత్తాంశం;
  (శూన్య)tqueue;
  ఎంపిక(ev) {
  స్థితి CURL_THRDQ_EV_అంశం_పూర్తి:
    Curl_multi_wakeup_అంతర్గత(multi);
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
}
#లేకపోతే
#నిర్వచించు async_thrdd_event   ఖాళీ
#అయితే_ముగింపు

స్థిర శూన్య async_thrdd_అంశం_free(శూన్య *అంశం)
{
  async_thrdd_అంశం_destroy(అంశం);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(నిర్మాణం Curl_multi *multi,
                                     uint32_t min_దారాలు,
                                     uint32_t max_దారాలు,
                                     uint32_t idle_సమయం_ms)
{
  CURLcode ఫలితం;
  DEBUGASSERT(!multi->resolv_thrdq);
  ఫలితం = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_దారాలు, max_దారాలు, idle_సమయం_ms,
                             async_thrdd_అంశం_free,
                             async_thrdd_అంశం_process,
                             async_thrdd_event,
                             multi);
#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(!ఫలితం) {
    స్థిరరాశి అక్షరం *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, UINT32_MAX)) {
        ఫలితం = Curl_async_thrdd_multi_సమితి_props(
          multi, min_దారాలు, (uint32_t)l, idle_సమయం_ms);
      }
    }
  }
#అయితే_ముగింపు
  తిరిగి ఫలితం;
}

/* Tear down the thread queue, joining active threads or detaching them */
శూన్య Curl_async_thrdd_multi_destroy(నిర్మాణం Curl_multi *multi, bool join)
{
  ఒకవేళ(multi->resolv_thrdq) {
#నిర్వచితమైతే CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#అయితే_ముగింపు
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = ఖాళీ;
  }
}

#నిర్వచితమైతే CURLVERBOSE
స్థిర శూన్య async_thrdd_report_అంశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం async_thrdd_అంశం *అంశం)
{
  అక్షరం buf[MAX_IPADR_LEN];
  నిర్మాణం dynbuf tmp;
  స్థిరరాశి అక్షరం *sep = "";
  స్థిరరాశి నిర్మాణం Curl_addrinfo *ai = అంశం->res;
  CURLcode ఫలితం;
  పూర్ణాంకం ai_family;
#నిర్వచితమైతే USE_IPV6
  ai_family = (అంశం->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#లేకపోతే
  ai_family = AF_INET;
#అయితే_ముగింపు

  ఒకవేళ(!CURL_TRC_DNS_is_verbose(దత్తాంశం))
    తిరిగి;

  curlx_dyn_init(&tmp, 1024);
  కొరకు(; ai; ai = ai->ai_తదుపరి) {
    ఒకవేళ(ai->ai_family == ai_family) {
      Curl_printable_చిరునామా(ai, buf, పరిమాణం(buf));
      ఫలితం = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      ఒకవేళ(ఫలితం) {
        CURL_TRC_DNS(దత్తాంశం, "too many IP, cannot show");
        వెళ్ళు బహిర్గతం;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(దత్తాంశం, "Host %s:%u resolved IPv%c: %s",
               అంశం->hostname, అంశం->పోర్టు,
               (అంశం->dns_queries & CURL_DNSQ_AAAA) ? '6' : '4',
               (curlx_dyn_len(&tmp) ? curlx_dyn_ptr(&tmp) : "(none)"));
బహిర్గతం:
  curlx_dyn_free(&tmp);
}
#అయితే_ముగింపు /* CURLVERBOSE */

/* Process the receiving end of the thread queue, dispatching
 * processed items to their transfer when it can still be found
 * and has an `async` state present. Otherwise, destroy the item. */
శూన్య Curl_async_thrdd_multi_process(నిర్మాణం Curl_multi *multi)
{
  నిర్మాణం Curl_easy *దత్తాంశం;
  శూన్య *qitem;

  అయితే(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    నిర్మాణం async_thrdd_అంశం *అంశం = qitem;
    నిర్మాణం Curl_resolv_async *async = ఖాళీ;

    దత్తాంశం = Curl_multi_get_easy(multi, అంశం->mid);
    ఒకవేళ(దత్తాంశం)
      async = Curl_async_get(దత్తాంశం, అంశం->resolv_id);
    ఒకవేళ(async) {
      నిర్మాణం async_thrdd_అంశం **pdest = &async->thrdd.res_A;

#నిర్వచితమైతే CURLRES_IPV6
      ఒకవేళ(అంశం->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#అయితే_ముగింపు
      ఒకవేళ(!*pdest) {
        *pdest = అంశం;
        అంశం = ఖాళీ;
      }
      లేకపోతే
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(దత్తాంశం);
    }
    async_thrdd_అంశం_free(అంశం);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_సమితి_props(నిర్మాణం Curl_multi *multi,
                                          uint32_t min_దారాలు,
                                          uint32_t max_దారాలు,
                                          uint32_t idle_సమయం_ms)
{
  తిరిగి Curl_thrdq_సమితి_props(multi->resolv_thrdq,
                              min_దారాలు, max_దారాలు, idle_సమయం_ms);
}

స్థిర CURLcode async_thrdd_query(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  నిర్మాణం async_thrdd_అంశం *అంశం;
  CURLcode ఫలితం;

  అంశం = async_thrdd_అంశం_create(దత్తాంశం, async->id, dns_queries,
                                 async->peer->hostname, async->peer->పోర్టు,
                                 async->transport);
  ఒకవేళ(!అంశం) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  CURL_TRC_DNS(దత్తాంశం, "queueing query %s", అంశం->description);
  ఫలితం = Curl_thrdq_send(దత్తాంశం->multi->resolv_thrdq, అంశం,
                           async_అంశం_description(అంశం), async->timeout_ms);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  అంశం = ఖాళీ;
  async->queries_ongoing++;

బహిర్గతం:
  ఒకవేళ(అంశం)
    async_thrdd_అంశం_free(అంశం);
  తిరిగి ఫలితం;
}

CURLcode Curl_async_getaddrinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_resolv_async *async)
{
  CURLcode ఫలితం = CURLE_FAILED_INIT;
  శూన్య *resolver = ఖాళీ;

  ఒకవేళ(async->queries_ongoing || async->పూర్తి)
    తిరిగి CURLE_FAILED_INIT;

#నిర్వచితమైతే USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  ఒకవేళ((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    ఫలితం = async_rr_init(దత్తాంశం, async);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    resolver = async->thrdd.rr.channel;
  }
#అయితే_ముగింపు

  ఫలితం = Curl_resolv_announce_ప్రారంభం(దత్తాంశం, resolver);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

#నిర్వచితమైతే USE_HTTPSRR_ARES
  ఒకవేళ(resolver) {
    ఫలితం = async_rr_ప్రారంభం(దత్తాంశం, async);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు

#నిర్వచితమైతే CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  ఒకవేళ((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    ఫలితం = async_thrdd_query(దత్తాంశం, async, CURL_DNSQ_AAAA);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు
  ఒకవేళ(async->dns_queries & CURL_DNSQ_A) {
    ఫలితం = async_thrdd_query(దత్తాంశం, async, CURL_DNSQ_A);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  CURL_THRDQ_TRACE(దత్తాంశం->multi->resolv_thrdq, దత్తాంశం);

బహిర్గతం:
  ఒకవేళ(!async->queries_ongoing)
    async->పూర్తి = TRUE;

  ఒకవేళ(ఫలితం)
    CURL_TRC_DNS(దత్తాంశం, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->పోర్టు, (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

CURLcode Curl_async_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_resolv_async *async,
                            నిర్మాణం easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(దత్తాంశం, async);
#నిర్వచితమైతే USE_HTTPSRR_ARES
  ఒకవేళ(async->thrdd.rr.channel) {
    CURLcode ఫలితం = Curl_ares_pollset(దత్తాంశం, async->thrdd.rr.channel, ps);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    timeout_ms = Curl_ares_timeout_ms(దత్తాంశం, async, async->thrdd.rr.channel);
  }
#లేకపోతే
  (శూన్య)ps;
#అయితే_ముగింపు

  ఒకవేళ(!async->పూర్తి) {
    స్థిరరాశి నిర్మాణం curltime *pnow = Curl_pgrs_now(దత్తాంశం);
#నిర్వచితం_కాకపోతే ENABLE_అంతర్గత_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->ప్రారంభం);
    ఒకవేళ(elapsed_ms < 3)
      stutter_ms = 1;
    లేకపోతే ఒకవేళ(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    లేకపోతే ఒకవేళ(elapsed_ms <= 250)
      stutter_ms = 50;
    లేకపోతే
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#లేకపోతే
    ఒకవేళ(async->queries_ongoing &&
       !Curl_thrdq_check_started(దత్తాంశం->multi->resolv_thrdq)) {
      /* The queue has items but starting a worker thread to process
         them just failed again; expire soon to check once more,
         instead of sleeping on the full resolve timeout. */
      CURL_TRC_DNS(దత్తాంశం, "resolver thread start failed again, retrying");
      timeout_ms = CURLMIN(100, timeout_ms);
    }
#అయితే_ముగింపు
    Curl_expire_సమితి(దత్తాంశం, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  తిరిగి CURLE_OK;
}

#ఒకవేళ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
స్థిర bool async_thrdd_అంశం_missing_scope(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం async_thrdd_అంశం *అంశం)
{
  స్థిరరాశి నిర్మాణం Curl_addrinfo *ai;

  /* scope id already globally set */
  ఒకవేళ(దత్తాంశం->conn && దత్తాంశం->conn->scope_id)
    తిరిగి FALSE;

  కొరకు(ai = అంశం->res; ai; ai = ai->ai_తదుపరి) {
    ఒకవేళ(ai->ai_family == AF_INET6) {
      నిర్మాణం sockaddr_in6 *sa6 = (శూన్య *)ai->ai_addr;
      ఒకవేళ(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        తిరిగి TRUE;
    }
  }
  తిరిగి FALSE;
}

స్థిర శూన్య async_thrdd_అంశం_strip_results(నిర్మాణం async_thrdd_అంశం *అంశం,
                                           పూర్ణాంకం ai_family)
{
  నిర్మాణం Curl_addrinfo *ai = అంశం->res, **panchor = &అంశం->res;
  అయితే(ai) {
    ఒకవేళ(ai->ai_family == ai_family) {
      *panchor = ai->ai_తదుపరి;
      ai->ai_తదుపరి = ఖాళీ;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    లేకపోతే {
      panchor = &ai->ai_తదుపరి;
      ai = ai->ai_తదుపరి;
    }
  }
}

#అయితే_ముగింపు /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

స్థిర CURLcode async_thrdd_check_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం Curl_resolv_async *async)
{
  నిర్మాణం async_thrdd_ctx *thrdd = &async->thrdd;

  (శూన్య)దత్తాంశం;
  ఒకవేళ(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_అంశం(దత్తాంశం, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = ఖాళీ;
    thrdd->processed_A = TRUE;
  }

  ఒకవేళ(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#ఒకవేళ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    ఒకవేళ(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_అంశం_missing_scope(దత్తాంశం, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      నిర్మాణం async_thrdd_అంశం *అంశం = thrdd->res_AAAA;
      CURLcode ఫలితం;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = ఖాళీ;
      అంశం->dns_queries |= CURL_DNSQ_A;
      ఒకవేళ(అంశం->res) {
        Curl_freeaddrinfo(అంశం->res);
        అంశం->res = ఖాళీ;
      }

      CURL_TRC_DNS(దత్తాంశం, "re-queueing query %s for AF_UNSPEC resolve",
                   అంశం->description);
      ఫలితం = Curl_thrdq_send(దత్తాంశం->multi->resolv_thrdq, అంశం,
                               async_అంశం_description(అంశం),
                               async->timeout_ms);
      ఒకవేళ(ఫలితం) {
        async_thrdd_అంశం_free(అంశం);
        తిరిగి ఫలితం;
      }
      async->queries_ongoing++;
      తిరిగి CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_అంశం_strip_results(thrdd->res_AAAA, AF_INET);
#అయితే_ముగింపు /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_అంశం(దత్తాంశం, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = ఖాళీ;
    thrdd->processed_AAAA = TRUE;
  }

  ఒకవేళ(async->queries_ongoing)
    తిరిగి CURLE_AGAIN;
  async->పూర్తి = TRUE;
  తిరిగి CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_ఫలితం(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_resolv_async *async,
                                నిర్మాణం Curl_dns_నమోదు **pdns)
{
  నిర్మాణం async_thrdd_ctx *thrdd = &async->thrdd;
  నిర్మాణం Curl_dns_నమోదు *dns = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ఖాళీ;

#నిర్వచితమైతే USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  ఒకవేళ(thrdd->rr.channel)
    (శూన్య)Curl_ares_perform(thrdd->rr.channel, 0);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే ENABLE_అంతర్గత_WAKEUP
  Curl_async_thrdd_multi_process(దత్తాంశం->multi);
#అయితే_ముగింపు

  ఫలితం = async_thrdd_check_పూర్తి(దత్తాంశం, async);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  Curl_expire_clear(దత్తాంశం, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    స్థిరరాశి uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    ఒకవేళ(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    ఒకవేళ(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    ఒకవేళ(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  ఒకవేళ(async->ఫలితం) {
    ఫలితం = async->ఫలితం;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      దత్తాంశం, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    ఒకవేళ(!dns) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }

#నిర్వచితమైతే USE_HTTPSRR_ARES
  ఒకవేళ(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(దత్తాంశం, async->httpsrr);
    dns = Curl_dnsc_mk_https(దత్తాంశం, &async->httpsrr, async->peer);
    ఒకవేళ(!dns) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }
#అయితే_ముగింపు

  ఒకవేళ(dns) {
    *pdns = dns;
    dns = ఖాళీ;
  }
  CURL_THRDQ_TRACE(దత్తాంశం->multi->resolv_thrdq, దత్తాంశం);

బహిర్గతం:
  Curl_dns_నమోదు_unlink(దత్తాంశం, &dns);
  Curl_async_thrdd_shutdown(దత్తాంశం, async);
  ఒకవేళ(!ఫలితం && !*pdns)
    ఫలితం = Curl_async_failed(దత్తాంశం, async, ఖాళీ);
  ఒకవేళ(ఫలితం &&
     (ఫలితం != CURLE_COULDNT_RESOLVE_HOST) &&
     (ఫలితం != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(దత్తాంశం, "Error %d resolving %s:%d",
                 (పూర్ణాంకం)ఫలితం, async->peer->hostname, async->peer->పోర్టు);
  }
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* USE_RESOLV_THREADED */
