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
#ifdef USE_RESOLV_THREADED

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#ifdef HAVE_GETADDRINFO
#define RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#अञ्ञथा
#define RESOLVER_ENOMEM  SOCKENOMEM
#endif

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

#ifdef USE_ARES
#अन्तर्भाव <ares.h>
#ifdef USE_HTTPSRR
#define USE_HTTPSRR_ARES  /* the combo */
#endif
#endif


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
पूर्णाङ्क Curl_async_global_init(शून्य)
{
#सचे defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  सचे(ares_library_init(ARES_LIB_INIT_ALL)) {
    पटिनिवत्त CURLE_FAILED_INIT;
  }
#endif
  पटिनिवत्त CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
शून्य Curl_async_global_cleanup(शून्य)
{
#सचे defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#endif
}

#ifdef CURLVERBOSE
#define CURL_ASYN_ITEM_DESC_LEN   64
#define async_item_description(x)   (x)->description
#अञ्ञथा
#define async_item_description(x)   रिक्त
#endif

संरचना async_thrdd_item {
  संरचना Curl_addrinfo *res;
#ifdef CURLVERBOSE
  वर्ण description[CURL_ASYN_ITEM_DESC_LEN];
#endif
  पूर्णाङ्क sockerr;
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
  वर्ण hostname[1];
};

/* Give up reference to add_ctx */
स्थिर शून्य async_thrdd_item_destroy(संरचना async_thrdd_item *item)
{
  सचे(item) {
    सचे(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
स्थिर संरचना async_thrdd_item *async_thrdd_item_create(
  संरचना Curl_easy *data,
  uint32_t resolv_id, uint8_t dns_queries,
  ध्रुव वर्ण *hostname, uint16_t port,
  uint8_t transport)
{
  size_t hostlen = strlen(hostname);
  संरचना async_thrdd_item *item;

  item = curlx_calloc(1, परिमाण(*item) + hostlen);
  सचे(!item)
    पटिनिवत्त रिक्त;

  सचे(hostlen) /* null-terminator of name already in struct size */
    memcpy(item->hostname, hostname, hostlen);
  item->mid = data->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#ifdef CURLVERBOSE
  curl_msnprintf(item->description, परिमाण(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 data->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#endif

#ifdef DEBUGBUILD
  {
    ध्रुव वर्ण *p = getenv("CURL_DBG_RESOLV_DELAY");
    सचे(p) {
      curl_off_t l;
      सचे(!curlx_str_सङ्ख्या(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    सचे(p) {
      curl_off_t l;
      सचे(!curlx_str_सङ्ख्या(&p, &l, UINT32_MAX)) {
        unsigned वर्ण c = 0;
        Curl_rand_bytes(data, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    सचे(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#endif

  पटिनिवत्त item;
}

#ifdef USE_HTTPSRR_ARES

स्थिर शून्य async_thrdd_rr_done(शून्य *user_data, ares_status_t status,
                                size_t timeouts,
                                ध्रुव ares_dns_record_t *dnsrec)
{
  संरचना Curl_resolv_async *async = user_data;
  संरचना async_thrdd_ctx *thrdd = async ? &async->thrdd : रिक्त;

  (शून्य)timeouts;
  सचे(!thrdd)
    पटिनिवत्त;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  सचे((ARES_SUCCESS == status) && dnsrec)
    async->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
स्थिर CURLcode async_rr_init(संरचना Curl_easy *data,
                              संरचना Curl_resolv_async *async)
{
  संरचना async_thrdd_ctx *thrdd = &async->thrdd;
  पूर्णाङ्क status;

  (शून्य)data;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, रिक्त, 0);
  सचे(status != ARES_SUCCESS) {
    thrdd->rr.channel = रिक्त;
    पटिनिवत्त CURLE_FAILED_INIT;
  }
#ifdef DEBUGBUILD
  सचे(getenv("CURL_DNS_SERVER")) {
    ध्रुव वर्ण *servers = getenv("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    सचे(status)
      पटिनिवत्त CURLE_FAILED_INIT;
  }
#endif
  पटिनिवत्त CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
स्थिर CURLcode async_rr_start(संरचना Curl_easy *data,
                               संरचना Curl_resolv_async *async)
{
  संरचना async_thrdd_ctx *thrdd = &async->thrdd;
  वर्ण *https_name = रिक्त;
  CURLcode result = CURLE_OK;

  सचे(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    सचे(!https_name) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, रिक्त);
  CURL_TRC_DNS(data, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  पटिनिवत्त result;
}
#endif

शून्य Curl_async_thrdd_shutdown(संरचना Curl_easy *data,
                               संरचना Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(data, async);
}

संरचना async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

स्थिर bool async_thrdd_match_item(शून्य *qitem, शून्य *match_data)
{
  ध्रुव संरचना async_thrdd_match_ctx *ctx = match_data;
  संरचना async_thrdd_item *item = qitem;
  पटिनिवत्त (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

शून्य Curl_async_thrdd_destroy(संरचना Curl_easy *data,
                              संरचना Curl_resolv_async *async)
{
  (शून्य)data;
  सचे(async->queries_ongoing && !async->done &&
     data->multi && data->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    संरचना async_thrdd_match_ctx mctx;
    mctx.mid = data->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(data->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#ifdef USE_HTTPSRR_ARES
  सचे(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = रिक्त;
  }
#endif
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = रिक्त;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = रिक्त;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(संरचना Curl_easy *data, uint32_t resolv_id,
                          संरचना Curl_dns_entry **pdns)
{
  संरचना Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  संरचना async_thrdd_ctx *thrdd = async ? &async->thrdd : रिक्त;
  timediff_t milli, ms;
  CURLcode result = CURLE_AGAIN;

  सचे(!thrdd)
    पटिनिवत्त CURLE_FAILED_INIT;

  याव(result == CURLE_AGAIN) {
    याव(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(data->multi);
      सचे(async->done)
        भङ्ग;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
      सचे(ms < 3)
        milli = 0;
      अञ्ञथा सचे(ms <= 50)
        milli = ms / 3;
      अञ्ञथा सचे(ms <= 250)
        milli = 50;
      अञ्ञथा
        milli = 200;
      CURL_TRC_DNS(data, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    result = Curl_async_take_result(data, async, pdns);
  }
  पटिनिवत्त result;
}

#ifdef HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
स्थिर bool gai_negative(पूर्णाङ्क rc)
{
  विकल्प(rc) {
#ifdef EAI_NONAME
  पक्ष EAI_NONAME:
#endif
#सचे defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  पक्ष EAI_NODATA:
#endif
    पटिनिवत्त TRUE;
  स्वभाव:
    पटिनिवत्त FALSE;
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

#ifdef DEBUGBUILD
  सचे(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  सचे(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    पटिनिवत्त;
  }
#endif

  memset(&hints, 0, परिमाण(hints));
#ifdef CURLRES_IPV6
  सचे(item->dns_queries & CURL_DNSQ_AAAA) {
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

  curl_msnprintf(service, परिमाण(service), "%u", item->port);
#ifdef AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#endif

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  सचे(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    सचे(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  अञ्ञथा {
    Curl_addrinfo_set_port(item->res, item->port);
  }
}

#अञ्ञथा /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
स्थिर शून्य async_thrdd_item_process(शून्य *arg)
{
  संरचना async_thrdd_item *item = arg;

#ifdef DEBUGBUILD
  सचे(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  सचे(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    पटिनिवत्त;
  }
#endif
  item->res = Curl_ipv4_resolve_r(item->hostname, item->port);
  सचे(!item->res) {
    item->sockerr = SOCKERRNO;
    सचे(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#endif /* HAVE_GETADDRINFO */

#ifdef ENABLE_INTERNAL_WAKEUP
स्थिर शून्य async_thrdd_event(ध्रुव संरचना curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              शून्य *user_data)
{
  संरचना Curl_multi *multi = user_data;
  (शून्य)tqueue;
  विकल्प(ev) {
  पक्ष CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
}
#अञ्ञथा
#define async_thrdd_event   रिक्त
#endif

स्थिर शून्य async_thrdd_item_free(शून्य *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(संरचना Curl_multi *multi,
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
  सचे(!result) {
    ध्रुव वर्ण *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    सचे(p) {
      curl_off_t l;
      सचे(!curlx_str_सङ्ख्या(&p, &l, UINT32_MAX)) {
        result = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#endif
  पटिनिवत्त result;
}

/* Tear down the thread queue, joining active threads or detaching them */
शून्य Curl_async_thrdd_multi_destroy(संरचना Curl_multi *multi, bool join)
{
  सचे(multi->resolv_thrdq) {
#ifdef CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#endif
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = रिक्त;
  }
}

#ifdef CURLVERBOSE
स्थिर शून्य async_thrdd_report_item(संरचना Curl_easy *data,
                                    संरचना async_thrdd_item *item)
{
  वर्ण buf[MAX_IPADR_LEN];
  संरचना dynbuf tmp;
  ध्रुव वर्ण *sep = "";
  ध्रुव संरचना Curl_addrinfo *ai = item->res;
  CURLcode result;
  पूर्णाङ्क ai_family;
#ifdef USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#अञ्ञथा
  ai_family = AF_INET;
#endif

  सचे(!CURL_TRC_DNS_is_verbose(data))
    पटिनिवत्त;

  curlx_dyn_init(&tmp, 1024);
  अत्थाय(; ai; ai = ai->ai_next) {
    सचे(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, परिमाण(buf));
      result = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      सचे(result) {
        CURL_TRC_DNS(data, "too many IP, cannot show");
        गच्छ out;
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
शून्य Curl_async_thrdd_multi_process(संरचना Curl_multi *multi)
{
  संरचना Curl_easy *data;
  शून्य *qitem;

  याव(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    संरचना async_thrdd_item *item = qitem;
    संरचना Curl_resolv_async *async = रिक्त;

    data = Curl_multi_get_easy(multi, item->mid);
    सचे(data)
      async = Curl_async_get(data, item->resolv_id);
    सचे(async) {
      संरचना async_thrdd_item **pdest = &async->thrdd.res_A;

#ifdef CURLRES_IPV6
      सचे(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#endif
      सचे(!*pdest) {
        *pdest = item;
        item = रिक्त;
      }
      अञ्ञथा
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(data);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(संरचना Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  पटिनिवत्त Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

स्थिर CURLcode async_thrdd_query(संरचना Curl_easy *data,
                                  संरचना Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  संरचना async_thrdd_item *item;
  CURLcode result;

  item = async_thrdd_item_create(data, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  सचे(!item) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  CURL_TRC_DNS(data, "queueing query %s", item->description);
  result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  सचे(result)
    गच्छ out;
  item = रिक्त;
  async->queries_ongoing++;

out:
  सचे(item)
    async_thrdd_item_free(item);
  पटिनिवत्त result;
}

CURLcode Curl_async_getaddrinfo(संरचना Curl_easy *data,
                                संरचना Curl_resolv_async *async)
{
  CURLcode result = CURLE_FAILED_INIT;
  शून्य *resolver = रिक्त;

  सचे(async->queries_ongoing || async->done)
    पटिनिवत्त CURLE_FAILED_INIT;

#ifdef USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  सचे((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    result = async_rr_init(data, async);
    सचे(result)
      गच्छ out;
    resolver = async->thrdd.rr.channel;
  }
#endif

  result = Curl_resolv_announce_start(data, resolver);
  सचे(result)
    पटिनिवत्त result;

#ifdef USE_HTTPSRR_ARES
  सचे(resolver) {
    result = async_rr_start(data, async);
    सचे(result)
      गच्छ out;
  }
#endif

#ifdef CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  सचे((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    result = async_thrdd_query(data, async, CURL_DNSQ_AAAA);
    सचे(result)
      गच्छ out;
  }
#endif
  सचे(async->dns_queries & CURL_DNSQ_A) {
    result = async_thrdd_query(data, async, CURL_DNSQ_A);
    सचे(result)
      गच्छ out;
  }

  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  सचे(!async->queries_ongoing)
    async->done = TRUE;

  सचे(result)
    CURL_TRC_DNS(data, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (पूर्णाङ्क)result);
  पटिनिवत्त result;
}

CURLcode Curl_async_pollset(संरचना Curl_easy *data,
                            संरचना Curl_resolv_async *async,
                            संरचना easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(data, async);
#ifdef USE_HTTPSRR_ARES
  सचे(async->thrdd.rr.channel) {
    CURLcode result = Curl_ares_pollset(data, async->thrdd.rr.channel, ps);
    सचे(result)
      पटिनिवत्त result;
    timeout_ms = Curl_ares_timeout_ms(data, async, async->thrdd.rr.channel);
  }
#अञ्ञथा
  (शून्य)ps;
#endif

  सचे(!async->done) {
    ध्रुव संरचना curltime *pnow = Curl_pgrs_now(data);
#ifndef ENABLE_INTERNAL_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    सचे(elapsed_ms < 3)
      stutter_ms = 1;
    अञ्ञथा सचे(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    अञ्ञथा सचे(elapsed_ms <= 250)
      stutter_ms = 50;
    अञ्ञथा
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#अञ्ञथा
    सचे(async->queries_ongoing &&
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
  पटिनिवत्त CURLE_OK;
}

#सचे defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
स्थिर bool async_thrdd_item_missing_scope(संरचना Curl_easy *data,
                                           संरचना async_thrdd_item *item)
{
  ध्रुव संरचना Curl_addrinfo *ai;

  /* scope id already globally set */
  सचे(data->conn && data->conn->scope_id)
    पटिनिवत्त FALSE;

  अत्थाय(ai = item->res; ai; ai = ai->ai_next) {
    सचे(ai->ai_family == AF_INET6) {
      संरचना sockaddr_in6 *sa6 = (शून्य *)ai->ai_addr;
      सचे(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        पटिनिवत्त TRUE;
    }
  }
  पटिनिवत्त FALSE;
}

स्थिर शून्य async_thrdd_item_strip_results(संरचना async_thrdd_item *item,
                                           पूर्णाङ्क ai_family)
{
  संरचना Curl_addrinfo *ai = item->res, **panchor = &item->res;
  याव(ai) {
    सचे(ai->ai_family == ai_family) {
      *panchor = ai->ai_next;
      ai->ai_next = रिक्त;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    अञ्ञथा {
      panchor = &ai->ai_next;
      ai = ai->ai_next;
    }
  }
}

#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

स्थिर CURLcode async_thrdd_check_done(संरचना Curl_easy *data,
                                       संरचना Curl_resolv_async *async)
{
  संरचना async_thrdd_ctx *thrdd = &async->thrdd;

  (शून्य)data;
  सचे(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(data, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = रिक्त;
    thrdd->processed_A = TRUE;
  }

  सचे(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#सचे defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    सचे(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(data, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      संरचना async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode result;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = रिक्त;
      item->dns_queries |= CURL_DNSQ_A;
      सचे(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = रिक्त;
      }

      CURL_TRC_DNS(data, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      सचे(result) {
        async_thrdd_item_free(item);
        पटिनिवत्त result;
      }
      async->queries_ongoing++;
      पटिनिवत्त CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(data, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = रिक्त;
    thrdd->processed_AAAA = TRUE;
  }

  सचे(async->queries_ongoing)
    पटिनिवत्त CURLE_AGAIN;
  async->done = TRUE;
  पटिनिवत्त CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_result(संरचना Curl_easy *data,
                                संरचना Curl_resolv_async *async,
                                संरचना Curl_dns_entry **pdns)
{
  संरचना async_thrdd_ctx *thrdd = &async->thrdd;
  संरचना Curl_dns_entry *dns = रिक्त;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = रिक्त;

#ifdef USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  सचे(thrdd->rr.channel)
    (शून्य)Curl_ares_perform(thrdd->rr.channel, 0);
#endif
#ifndef ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(data->multi);
#endif

  result = async_thrdd_check_done(data, async);
  सचे(result)
    पटिनिवत्त result;

  Curl_expire_clear(data, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    ध्रुव uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    सचे(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    सचे(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    सचे(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  सचे(async->result) {
    result = async->result;
    गच्छ out;
  }

  सचे(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    सचे(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
  }

#ifdef USE_HTTPSRR_ARES
  सचे(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(data, async->httpsrr);
    dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
    सचे(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
  }
#endif

  सचे(dns) {
    *pdns = dns;
    dns = रिक्त;
  }
  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  Curl_dns_entry_unlink(data, &dns);
  Curl_async_thrdd_shutdown(data, async);
  सचे(!result && !*pdns)
    result = Curl_async_failed(data, async, रिक्त);
  सचे(result &&
     (result != CURLE_COULDNT_RESOLVE_HOST) &&
     (result != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(data, "Error %d resolving %s:%d",
                 (पूर्णाङ्क)result, async->peer->hostname, async->peer->port);
  }
  पटिनिवत्त result;
}

#endif /* USE_RESOLV_THREADED */
