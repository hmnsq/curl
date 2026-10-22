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
#अन्तर्भाव "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#यदि_परिभाषितम् USE_RESOLV_THREADED

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_GETADDRINFO
#परिभाषय RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#अन्यथा
#परिभाषय RESOLVER_ENOMEM  SOCKENOMEM
#यदि_अन्तः

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "curl_threads.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "thrdqueue.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curlx/wait.h"

#यदि_परिभाषितम् USE_ARES
#अन्तर्भाव <ares.h>
#यदि_परिभाषितम् USE_HTTPSRR
#परिभाषय USE_HTTPSRR_ARES  /* the combo */
#यदि_अन्तः
#यदि_अन्तः


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
पूर्णाङ्क Curl_async_global_init(शून्य)
{
#यदि defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  यदि(ares_library_init(ARES_LIB_INIT_ALL)) {
    प्रत्यागम CURLE_FAILED_INIT;
  }
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
शून्य Curl_async_global_cleanup(शून्य)
{
#यदि defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#यदि_अन्तः
}

#यदि_परिभाषितम् CURLVERBOSE
#परिभाषय CURL_ASYN_ITEM_DESC_LEN   64
#परिभाषय async_item_description(x)   (x)->description
#अन्यथा
#परिभाषय async_item_description(x)   रिक्त
#यदि_अन्तः

संरचना async_thrdd_item {
  संरचना Curl_addrinfo *res;
#यदि_परिभाषितम् CURLVERBOSE
  वर्ण description[CURL_ASYN_ITEM_DESC_LEN];
#यदि_अन्तः
  पूर्णाङ्क sockerr;
  uint32_t mid;
  uint32_t resolv_id;
  uint16_t पत्तनम्;
  uint8_t transport;
  uint8_t dns_queries;
  BIT(negative); /* resolver answered that the name does not exist */
#यदि_परिभाषितम् DEBUGBUILD
  uint32_t delay_ms;
  uint32_t delay_fail_ms;
  BIT(dbg_negative);
#यदि_अन्तः
  वर्ण hostname[1];
};

/* Give up reference to add_ctx */
स्थिर शून्य async_thrdd_item_destroy(संरचना async_thrdd_item *item)
{
  यदि(item) {
    यदि(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
स्थिर संरचना async_thrdd_item *async_thrdd_item_create(
  संरचना Curl_easy *दत्त,
  uint32_t resolv_id, uint8_t dns_queries,
  ध्रुव वर्ण *hostname, uint16_t पत्तनम्,
  uint8_t transport)
{
  size_t hostlen = वाक्यदैर्घ्यम्(hostname);
  संरचना async_thrdd_item *item;

  item = curlx_calloc(1, परिमाण(*item) + hostlen);
  यदि(!item)
    प्रत्यागम रिक्त;

  यदि(hostlen) /* null-terminator of name already in struct size */
    स्मृतिं_प्रतिलिख(item->hostname, hostname, hostlen);
  item->mid = दत्त->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->पत्तनम् = पत्तनम्;
  item->transport = transport;

#यदि_परिभाषितम् CURLVERBOSE
  curl_msnprintf(item->description, परिमाण(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 दत्त->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->पत्तनम्);
#यदि_अन्तः

#यदि_परिभाषितम् DEBUGBUILD
  {
    ध्रुव वर्ण *p = getenv("CURL_DBG_RESOLV_DELAY");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, UINT32_MAX)) {
        अचिह्नित वर्ण c = 0;
        Curl_rand_अष्टकानि(दत्त, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    यदि(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#यदि_अन्तः

  प्रत्यागम item;
}

#यदि_परिभाषितम् USE_HTTPSRR_ARES

स्थिर शून्य async_thrdd_rr_समाप्तम्(शून्य *user_दत्त, ares_स्थितिः_t स्थितिः,
                                size_t timeouts,
                                ध्रुव ares_dns_लेखः_t *dnsrec)
{
  संरचना Curl_resolv_async *async = user_दत्त;
  संरचना async_thrdd_ctx *thrdd = async ? &async->thrdd : रिक्त;

  (शून्य)timeouts;
  यदि(!thrdd)
    प्रत्यागम;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->समाप्तम् = !async->queries_ongoing;
  यदि((ARES_SUCCESS == स्थितिः) && dnsrec)
    async->परिणाम = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
स्थिर CURLcode async_rr_init(संरचना Curl_easy *दत्त,
                              संरचना Curl_resolv_async *async)
{
  संरचना async_thrdd_ctx *thrdd = &async->thrdd;
  पूर्णाङ्क स्थितिः;

  (शून्य)दत्त;
  DEBUGASSERT(!thrdd->rr.channel);
  स्थितिः = ares_init_options(&thrdd->rr.channel, रिक्त, 0);
  यदि(स्थितिः != ARES_SUCCESS) {
    thrdd->rr.channel = रिक्त;
    प्रत्यागम CURLE_FAILED_INIT;
  }
#यदि_परिभाषितम् DEBUGBUILD
  यदि(getenv("CURL_DNS_SERVER")) {
    ध्रुव वर्ण *servers = getenv("CURL_DNS_SERVER");
    स्थितिः = ares_समुच्चयः_servers_ports_csv(thrdd->rr.channel, servers);
    यदि(स्थितिः)
      प्रत्यागम CURLE_FAILED_INIT;
  }
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
स्थिर CURLcode async_rr_start(संरचना Curl_easy *दत्त,
                               संरचना Curl_resolv_async *async)
{
  संरचना async_thrdd_ctx *thrdd = &async->thrdd;
  वर्ण *https_name = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  यदि(async->peer->पत्तनम् != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->पत्तनम्, async->peer->hostname);
    यदि(!https_name) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_अन्तर्, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_समाप्तम्, async, रिक्त);
  CURL_TRC_DNS(दत्त, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

बहिर्गमनम्:
  curlx_free(https_name);
  प्रत्यागम परिणाम;
}
#यदि_अन्तः

शून्य Curl_async_thrdd_shutdown(संरचना Curl_easy *दत्त,
                               संरचना Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(दत्त, async);
}

संरचना async_thrdd_मेलनम्_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

स्थिर bool async_thrdd_मेलनम्_item(शून्य *qitem, शून्य *मेलनम्_दत्त)
{
  ध्रुव संरचना async_thrdd_मेलनम्_ctx *ctx = मेलनम्_दत्त;
  संरचना async_thrdd_item *item = qitem;
  प्रत्यागम (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

शून्य Curl_async_thrdd_destroy(संरचना Curl_easy *दत्त,
                              संरचना Curl_resolv_async *async)
{
  (शून्य)दत्त;
  यदि(async->queries_ongoing && !async->समाप्तम् &&
     दत्त->multi && दत्त->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    संरचना async_thrdd_मेलनम्_ctx mctx;
    mctx.mid = दत्त->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(दत्त->multi->resolv_thrdq,
                     async_thrdd_मेलनम्_item, &mctx);
  }
#यदि_परिभाषितम् USE_HTTPSRR_ARES
  यदि(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = रिक्त;
  }
#यदि_अन्तः
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = रिक्त;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = रिक्त;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(संरचना Curl_easy *दत्त, uint32_t resolv_id,
                          संरचना Curl_dns_प्रविष्टिः **pdns)
{
  संरचना Curl_resolv_async *async = Curl_async_get(दत्त, resolv_id);
  संरचना async_thrdd_ctx *thrdd = async ? &async->thrdd : रिक्त;
  timediff_t milli, ms;
  CURLcode परिणाम = CURLE_AGAIN;

  यदि(!thrdd)
    प्रत्यागम CURLE_FAILED_INIT;

  यावत्(परिणाम == CURLE_AGAIN) {
    यावत्(async->queries_ongoing && !async->समाप्तम्) {
      Curl_async_thrdd_multi_process(दत्त->multi);
      यदि(async->समाप्तम्)
        भङ्ग;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &async->start);
      यदि(ms < 3)
        milli = 0;
      अन्यथा यदि(ms <= 50)
        milli = ms / 3;
      अन्यथा यदि(ms <= 250)
        milli = 50;
      अन्यथा
        milli = 200;
      CURL_TRC_DNS(दत्त, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    परिणाम = Curl_async_take_परिणाम(दत्त, async, pdns);
  }
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
स्थिर bool gai_negative(पूर्णाङ्क rc)
{
  विकल्प(rc) {
#यदि_परिभाषितम् EAI_NONAME
  पक्ष EAI_NONAME:
#यदि_अन्तः
#यदि defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  पक्ष EAI_NODATA:
#यदि_अन्तः
    प्रत्यागम TRUE;
  स्वभाव:
    प्रत्यागम FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
स्थिर शून्य async_thrdd_item_process(शून्य *arg)
{
  संरचना async_thrdd_item *item = arg;
  संरचना addrinfo hints;
  वर्ण service[12];
  पूर्णाङ्क pf = PF_INET;
  पूर्णाङ्क rc;

#यदि_परिभाषितम् DEBUGBUILD
  यदि(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  यदि(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    प्रत्यागम;
  }
#यदि_अन्तः

  स्मृतिं_पूरय(&hints, 0, परिमाण(hints));
#यदि_परिभाषितम् CURLRES_IPV6
  यदि(item->dns_queries & CURL_DNSQ_AAAA) {
    pf = (item->dns_queries & CURL_DNSQ_A) ? PF_UNSPEC : PF_INET6;
  }
#यदि_अन्तः
  hints.ai_family = pf;
  hints.ai_socktype = Curl_socktype_for_transport(item->transport);
  hints.ai_protocol = Curl_protocol_for_transport(item->transport);
#यदि_परिभाषितम् __APPLE__
  /* If we leave `ai_flags == 0` then macOS is looking for IPV4MAPPED
   * when doing AAAA queries. We do not want this "help". */
  hints.ai_flags = AI_ADDRCONFIG;
#यदि_अन्तः

  curl_msnprintf(service, परिमाण(service), "%u", item->पत्तनम्);
#यदि_परिभाषितम् AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#यदि_अन्तः

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  यदि(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    यदि(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  अन्यथा {
    Curl_addrinfo_समुच्चयः_पत्तनम्(item->res, item->पत्तनम्);
  }
}

#अन्यथा /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
स्थिर शून्य async_thrdd_item_process(शून्य *arg)
{
  संरचना async_thrdd_item *item = arg;

#यदि_परिभाषितम् DEBUGBUILD
  यदि(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  यदि(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    प्रत्यागम;
  }
#यदि_अन्तः
  item->res = Curl_ipv4_resolve_r(item->hostname, item->पत्तनम्);
  यदि(!item->res) {
    item->sockerr = SOCKERRNO;
    यदि(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#यदि_अन्तः /* HAVE_GETADDRINFO */

#यदि_परिभाषितम् ENABLE_आन्तरिकम्_WAKEUP
स्थिर शून्य async_thrdd_event(ध्रुव संरचना curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              शून्य *user_दत्त)
{
  संरचना Curl_multi *multi = user_दत्त;
  (शून्य)tqueue;
  विकल्प(ev) {
  पक्ष CURL_THRDQ_EV_ITEM_समाप्तम्:
    Curl_multi_wakeup_आन्तरिकम्(multi);
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
}
#अन्यथा
#परिभाषय async_thrdd_event   रिक्त
#यदि_अन्तः

स्थिर शून्य async_thrdd_item_free(शून्य *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(संरचना Curl_multi *multi,
                                     uint32_t min_तन्तवः,
                                     uint32_t max_तन्तवः,
                                     uint32_t idle_time_ms)
{
  CURLcode परिणाम;
  DEBUGASSERT(!multi->resolv_thrdq);
  परिणाम = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_तन्तवः, max_तन्तवः, idle_time_ms,
                             async_thrdd_item_free,
                             async_thrdd_item_process,
                             async_thrdd_event,
                             multi);
#यदि_परिभाषितम् DEBUGBUILD
  यदि(!परिणाम) {
    ध्रुव वर्ण *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, UINT32_MAX)) {
        परिणाम = Curl_async_thrdd_multi_समुच्चयः_props(
          multi, min_तन्तवः, (uint32_t)l, idle_time_ms);
      }
    }
  }
#यदि_अन्तः
  प्रत्यागम परिणाम;
}

/* Tear down the thread queue, joining active threads or detaching them */
शून्य Curl_async_thrdd_multi_destroy(संरचना Curl_multi *multi, bool join)
{
  यदि(multi->resolv_thrdq) {
#यदि_परिभाषितम् CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#यदि_अन्तः
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = रिक्त;
  }
}

#यदि_परिभाषितम् CURLVERBOSE
स्थिर शून्य async_thrdd_report_item(संरचना Curl_easy *दत्त,
                                    संरचना async_thrdd_item *item)
{
  वर्ण buf[MAX_IPADR_LEN];
  संरचना dynbuf tmp;
  ध्रुव वर्ण *sep = "";
  ध्रुव संरचना Curl_addrinfo *ai = item->res;
  CURLcode परिणाम;
  पूर्णाङ्क ai_family;
#यदि_परिभाषितम् USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#अन्यथा
  ai_family = AF_INET;
#यदि_अन्तः

  यदि(!CURL_TRC_DNS_is_verbose(दत्त))
    प्रत्यागम;

  curlx_dyn_init(&tmp, 1024);
  प्रति(; ai; ai = ai->ai_अग्र) {
    यदि(ai->ai_family == ai_family) {
      Curl_printable_स्थानाङ्कः(ai, buf, परिमाण(buf));
      परिणाम = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      यदि(परिणाम) {
        CURL_TRC_DNS(दत्त, "too many IP, cannot show");
        गच्छ बहिर्गमनम्;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(दत्त, "Host %s:%u resolved IPv%c: %s",
               item->hostname, item->पत्तनम्,
               (item->dns_queries & CURL_DNSQ_AAAA) ? '6' : '4',
               (curlx_dyn_len(&tmp) ? curlx_dyn_ptr(&tmp) : "(none)"));
बहिर्गमनम्:
  curlx_dyn_free(&tmp);
}
#यदि_अन्तः /* CURLVERBOSE */

/* Process the receiving end of the thread queue, dispatching
 * processed items to their transfer when it can still be found
 * and has an `async` state present. Otherwise, destroy the item. */
शून्य Curl_async_thrdd_multi_process(संरचना Curl_multi *multi)
{
  संरचना Curl_easy *दत्त;
  शून्य *qitem;

  यावत्(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    संरचना async_thrdd_item *item = qitem;
    संरचना Curl_resolv_async *async = रिक्त;

    दत्त = Curl_multi_get_easy(multi, item->mid);
    यदि(दत्त)
      async = Curl_async_get(दत्त, item->resolv_id);
    यदि(async) {
      संरचना async_thrdd_item **pdest = &async->thrdd.res_A;

#यदि_परिभाषितम् CURLRES_IPV6
      यदि(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#यदि_अन्तः
      यदि(!*pdest) {
        *pdest = item;
        item = रिक्त;
      }
      अन्यथा
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(दत्त);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_समुच्चयः_props(संरचना Curl_multi *multi,
                                          uint32_t min_तन्तवः,
                                          uint32_t max_तन्तवः,
                                          uint32_t idle_time_ms)
{
  प्रत्यागम Curl_thrdq_समुच्चयः_props(multi->resolv_thrdq,
                              min_तन्तवः, max_तन्तवः, idle_time_ms);
}

स्थिर CURLcode async_thrdd_query(संरचना Curl_easy *दत्त,
                                  संरचना Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  संरचना async_thrdd_item *item;
  CURLcode परिणाम;

  item = async_thrdd_item_create(दत्त, async->id, dns_queries,
                                 async->peer->hostname, async->peer->पत्तनम्,
                                 async->transport);
  यदि(!item) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  CURL_TRC_DNS(दत्त, "queueing query %s", item->description);
  परिणाम = Curl_thrdq_send(दत्त->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  item = रिक्त;
  async->queries_ongoing++;

बहिर्गमनम्:
  यदि(item)
    async_thrdd_item_free(item);
  प्रत्यागम परिणाम;
}

CURLcode Curl_async_getaddrinfo(संरचना Curl_easy *दत्त,
                                संरचना Curl_resolv_async *async)
{
  CURLcode परिणाम = CURLE_FAILED_INIT;
  शून्य *resolver = रिक्त;

  यदि(async->queries_ongoing || async->समाप्तम्)
    प्रत्यागम CURLE_FAILED_INIT;

#यदि_परिभाषितम् USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  यदि((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    परिणाम = async_rr_init(दत्त, async);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    resolver = async->thrdd.rr.channel;
  }
#यदि_अन्तः

  परिणाम = Curl_resolv_announce_start(दत्त, resolver);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

#यदि_परिभाषितम् USE_HTTPSRR_ARES
  यदि(resolver) {
    परिणाम = async_rr_start(दत्त, async);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः

#यदि_परिभाषितम् CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  यदि((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    परिणाम = async_thrdd_query(दत्त, async, CURL_DNSQ_AAAA);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः
  यदि(async->dns_queries & CURL_DNSQ_A) {
    परिणाम = async_thrdd_query(दत्त, async, CURL_DNSQ_A);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  CURL_THRDQ_TRACE(दत्त->multi->resolv_thrdq, दत्त);

बहिर्गमनम्:
  यदि(!async->queries_ongoing)
    async->समाप्तम् = TRUE;

  यदि(परिणाम)
    CURL_TRC_DNS(दत्त, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->पत्तनम्, (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

CURLcode Curl_async_pollset(संरचना Curl_easy *दत्त,
                            संरचना Curl_resolv_async *async,
                            संरचना easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(दत्त, async);
#यदि_परिभाषितम् USE_HTTPSRR_ARES
  यदि(async->thrdd.rr.channel) {
    CURLcode परिणाम = Curl_ares_pollset(दत्त, async->thrdd.rr.channel, ps);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    timeout_ms = Curl_ares_timeout_ms(दत्त, async, async->thrdd.rr.channel);
  }
#अन्यथा
  (शून्य)ps;
#यदि_अन्तः

  यदि(!async->समाप्तम्) {
    ध्रुव संरचना curltime *pnow = Curl_pgrs_now(दत्त);
#यदि_अपरिभाषितम् ENABLE_आन्तरिकम्_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    यदि(elapsed_ms < 3)
      stutter_ms = 1;
    अन्यथा यदि(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    अन्यथा यदि(elapsed_ms <= 250)
      stutter_ms = 50;
    अन्यथा
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#अन्यथा
    यदि(async->queries_ongoing &&
       !Curl_thrdq_check_started(दत्त->multi->resolv_thrdq)) {
      /* The queue has items but starting a worker thread to process
         them just failed again; expire soon to check once more,
         instead of sleeping on the full resolve timeout. */
      CURL_TRC_DNS(दत्त, "resolver thread start failed again, retrying");
      timeout_ms = CURLMIN(100, timeout_ms);
    }
#यदि_अन्तः
    Curl_expire_समुच्चयः(दत्त, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  प्रत्यागम CURLE_OK;
}

#यदि defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
स्थिर bool async_thrdd_item_missing_scope(संरचना Curl_easy *दत्त,
                                           संरचना async_thrdd_item *item)
{
  ध्रुव संरचना Curl_addrinfo *ai;

  /* scope id already globally set */
  यदि(दत्त->conn && दत्त->conn->scope_id)
    प्रत्यागम FALSE;

  प्रति(ai = item->res; ai; ai = ai->ai_अग्र) {
    यदि(ai->ai_family == AF_INET6) {
      संरचना sockaddr_in6 *sa6 = (शून्य *)ai->ai_addr;
      यदि(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        प्रत्यागम TRUE;
    }
  }
  प्रत्यागम FALSE;
}

स्थिर शून्य async_thrdd_item_strip_results(संरचना async_thrdd_item *item,
                                           पूर्णाङ्क ai_family)
{
  संरचना Curl_addrinfo *ai = item->res, **panchor = &item->res;
  यावत्(ai) {
    यदि(ai->ai_family == ai_family) {
      *panchor = ai->ai_अग्र;
      ai->ai_अग्र = रिक्त;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    अन्यथा {
      panchor = &ai->ai_अग्र;
      ai = ai->ai_अग्र;
    }
  }
}

#यदि_अन्तः /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

स्थिर CURLcode async_thrdd_check_समाप्तम्(संरचना Curl_easy *दत्त,
                                       संरचना Curl_resolv_async *async)
{
  संरचना async_thrdd_ctx *thrdd = &async->thrdd;

  (शून्य)दत्त;
  यदि(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(दत्त, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = रिक्त;
    thrdd->processed_A = TRUE;
  }

  यदि(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#यदि defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    यदि(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(दत्त, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      संरचना async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode परिणाम;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = रिक्त;
      item->dns_queries |= CURL_DNSQ_A;
      यदि(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = रिक्त;
      }

      CURL_TRC_DNS(दत्त, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      परिणाम = Curl_thrdq_send(दत्त->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      यदि(परिणाम) {
        async_thrdd_item_free(item);
        प्रत्यागम परिणाम;
      }
      async->queries_ongoing++;
      प्रत्यागम CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#यदि_अन्तः /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(दत्त, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = रिक्त;
    thrdd->processed_AAAA = TRUE;
  }

  यदि(async->queries_ongoing)
    प्रत्यागम CURLE_AGAIN;
  async->समाप्तम् = TRUE;
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_परिणाम(संरचना Curl_easy *दत्त,
                                संरचना Curl_resolv_async *async,
                                संरचना Curl_dns_प्रविष्टिः **pdns)
{
  संरचना async_thrdd_ctx *thrdd = &async->thrdd;
  संरचना Curl_dns_प्रविष्टिः *dns = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = रिक्त;

#यदि_परिभाषितम् USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  यदि(thrdd->rr.channel)
    (शून्य)Curl_ares_perform(thrdd->rr.channel, 0);
#यदि_अन्तः
#यदि_अपरिभाषितम् ENABLE_आन्तरिकम्_WAKEUP
  Curl_async_thrdd_multi_process(दत्त->multi);
#यदि_अन्तः

  परिणाम = async_thrdd_check_समाप्तम्(दत्त, async);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  Curl_expire_clear(दत्त, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    ध्रुव uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    यदि(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    यदि(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    यदि(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  यदि(async->परिणाम) {
    परिणाम = async->परिणाम;
    गच्छ बहिर्गमनम्;
  }

  यदि(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      दत्त, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    यदि(!dns) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }

#यदि_परिभाषितम् USE_HTTPSRR_ARES
  यदि(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(दत्त, async->httpsrr);
    dns = Curl_dnsc_mk_https(दत्त, &async->httpsrr, async->peer);
    यदि(!dns) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }
#यदि_अन्तः

  यदि(dns) {
    *pdns = dns;
    dns = रिक्त;
  }
  CURL_THRDQ_TRACE(दत्त->multi->resolv_thrdq, दत्त);

बहिर्गमनम्:
  Curl_dns_प्रविष्टिः_unlink(दत्त, &dns);
  Curl_async_thrdd_shutdown(दत्त, async);
  यदि(!परिणाम && !*pdns)
    परिणाम = Curl_async_failed(दत्त, async, रिक्त);
  यदि(परिणाम &&
     (परिणाम != CURLE_COULDNT_RESOLVE_HOST) &&
     (परिणाम != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(दत्त, "Error %d resolving %s:%d",
                 (पूर्णाङ्क)परिणाम, async->peer->hostname, async->peer->पत्तनम्);
  }
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* USE_RESOLV_THREADED */
