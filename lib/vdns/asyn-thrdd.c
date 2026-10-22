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
#അന്തര്ഭാവ "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ifdef USE_RESOLV_THREADED

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#ifdef HAVE_GETADDRINFO
#define RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#അല്ലെങ്കിൽ
#define RESOLVER_ENOMEM  SOCKENOMEM
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "curl_threads.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "thrdqueue.h"
#അന്തര്ഭാവ "vdns/hostip.h"
#അന്തര്ഭാവ "vdns/httpsrr.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "curlx/wait.h"

#ifdef USE_ARES
#അന്തര്ഭാവ <ares.h>
#ifdef USE_HTTPSRR
#define USE_HTTPSRR_ARES  /* the combo */
#endif
#endif


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
പൂർണ്ണസംഖ്യ Curl_async_global_init(ശൂന്യം)
{
#എങ്കിൽ defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  എങ്കിൽ(ares_library_init(ARES_LIB_INIT_ALL)) {
    തിരികെ CURLE_FAILED_INIT;
  }
#endif
  തിരികെ CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
ശൂന്യം Curl_async_global_cleanup(ശൂന്യം)
{
#എങ്കിൽ defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#endif
}

#ifdef CURLVERBOSE
#define CURL_ASYN_ITEM_DESC_LEN   64
#define async_item_description(x)   (x)->description
#അല്ലെങ്കിൽ
#define async_item_description(x)   ശൂന്യ
#endif

ഘടന async_thrdd_item {
  ഘടന Curl_addrinfo *res;
#ifdef CURLVERBOSE
  അക്ഷരം description[CURL_ASYN_ITEM_DESC_LEN];
#endif
  പൂർണ്ണസംഖ്യ sockerr;
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
  അക്ഷരം hostname[1];
};

/* Give up reference to add_ctx */
സ്ഥിതം ശൂന്യം async_thrdd_item_destroy(ഘടന async_thrdd_item *item)
{
  എങ്കിൽ(item) {
    എങ്കിൽ(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
സ്ഥിതം ഘടന async_thrdd_item *async_thrdd_item_create(
  ഘടന Curl_easy *data,
  uint32_t resolv_id, uint8_t dns_queries,
  സ്ഥിരം അക്ഷരം *hostname, uint16_t port,
  uint8_t transport)
{
  size_t hostlen = strlen(hostname);
  ഘടന async_thrdd_item *item;

  item = curlx_calloc(1, വലുപ്പം(*item) + hostlen);
  എങ്കിൽ(!item)
    തിരികെ ശൂന്യ;

  എങ്കിൽ(hostlen) /* null-terminator of name already in struct size */
    memcpy(item->hostname, hostname, hostlen);
  item->mid = data->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#ifdef CURLVERBOSE
  curl_msnprintf(item->description, വലുപ്പം(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 data->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#endif

#ifdef DEBUGBUILD
  {
    സ്ഥിരം അക്ഷരം *p = getenv("CURL_DBG_RESOLV_DELAY");
    എങ്കിൽ(p) {
      curl_off_t l;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    എങ്കിൽ(p) {
      curl_off_t l;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, UINT32_MAX)) {
        unsigned അക്ഷരം c = 0;
        Curl_rand_bytes(data, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    എങ്കിൽ(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#endif

  തിരികെ item;
}

#ifdef USE_HTTPSRR_ARES

സ്ഥിതം ശൂന്യം async_thrdd_rr_done(ശൂന്യം *user_data, ares_status_t status,
                                size_t timeouts,
                                സ്ഥിരം ares_dns_record_t *dnsrec)
{
  ഘടന Curl_resolv_async *async = user_data;
  ഘടന async_thrdd_ctx *thrdd = async ? &async->thrdd : ശൂന്യ;

  (ശൂന്യം)timeouts;
  എങ്കിൽ(!thrdd)
    തിരികെ;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  എങ്കിൽ((ARES_SUCCESS == status) && dnsrec)
    async->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
സ്ഥിതം CURLcode async_rr_init(ഘടന Curl_easy *data,
                              ഘടന Curl_resolv_async *async)
{
  ഘടന async_thrdd_ctx *thrdd = &async->thrdd;
  പൂർണ്ണസംഖ്യ status;

  (ശൂന്യം)data;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, ശൂന്യ, 0);
  എങ്കിൽ(status != ARES_SUCCESS) {
    thrdd->rr.channel = ശൂന്യ;
    തിരികെ CURLE_FAILED_INIT;
  }
#ifdef DEBUGBUILD
  എങ്കിൽ(getenv("CURL_DNS_SERVER")) {
    സ്ഥിരം അക്ഷരം *servers = getenv("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    എങ്കിൽ(status)
      തിരികെ CURLE_FAILED_INIT;
  }
#endif
  തിരികെ CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
സ്ഥിതം CURLcode async_rr_start(ഘടന Curl_easy *data,
                               ഘടന Curl_resolv_async *async)
{
  ഘടന async_thrdd_ctx *thrdd = &async->thrdd;
  അക്ഷരം *https_name = ശൂന്യ;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    എങ്കിൽ(!https_name) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, ശൂന്യ);
  CURL_TRC_DNS(data, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  തിരികെ result;
}
#endif

ശൂന്യം Curl_async_thrdd_shutdown(ഘടന Curl_easy *data,
                               ഘടന Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(data, async);
}

ഘടന async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

സ്ഥിതം bool async_thrdd_match_item(ശൂന്യം *qitem, ശൂന്യം *match_data)
{
  സ്ഥിരം ഘടന async_thrdd_match_ctx *ctx = match_data;
  ഘടന async_thrdd_item *item = qitem;
  തിരികെ (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

ശൂന്യം Curl_async_thrdd_destroy(ഘടന Curl_easy *data,
                              ഘടന Curl_resolv_async *async)
{
  (ശൂന്യം)data;
  എങ്കിൽ(async->queries_ongoing && !async->done &&
     data->multi && data->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    ഘടന async_thrdd_match_ctx mctx;
    mctx.mid = data->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(data->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#ifdef USE_HTTPSRR_ARES
  എങ്കിൽ(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = ശൂന്യ;
  }
#endif
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = ശൂന്യ;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = ശൂന്യ;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(ഘടന Curl_easy *data, uint32_t resolv_id,
                          ഘടന Curl_dns_entry **pdns)
{
  ഘടന Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ഘടന async_thrdd_ctx *thrdd = async ? &async->thrdd : ശൂന്യ;
  timediff_t milli, ms;
  CURLcode result = CURLE_AGAIN;

  എങ്കിൽ(!thrdd)
    തിരികെ CURLE_FAILED_INIT;

  ആകുമ്പോൾ(result == CURLE_AGAIN) {
    ആകുമ്പോൾ(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(data->multi);
      എങ്കിൽ(async->done)
        നിർത്തുക;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
      എങ്കിൽ(ms < 3)
        milli = 0;
      അല്ലെങ്കിൽ എങ്കിൽ(ms <= 50)
        milli = ms / 3;
      അല്ലെങ്കിൽ എങ്കിൽ(ms <= 250)
        milli = 50;
      അല്ലെങ്കിൽ
        milli = 200;
      CURL_TRC_DNS(data, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    result = Curl_async_take_result(data, async, pdns);
  }
  തിരികെ result;
}

#ifdef HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
സ്ഥിതം bool gai_negative(പൂർണ്ണസംഖ്യ rc)
{
  തിരഞ്ഞെടുപ്പ്(rc) {
#ifdef EAI_NONAME
  അവസ്ഥ EAI_NONAME:
#endif
#എങ്കിൽ defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  അവസ്ഥ EAI_NODATA:
#endif
    തിരികെ TRUE;
  സ്ഥിരസ്ഥിതി:
    തിരികെ FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
സ്ഥിതം ശൂന്യം async_thrdd_item_process(ശൂന്യം *arg)
{
  ഘടന async_thrdd_item *item = arg;
  ഘടന addrinfo hints;
  അക്ഷരം service[12];
  പൂർണ്ണസംഖ്യ pf = PF_INET;
  പൂർണ്ണസംഖ്യ rc;

#ifdef DEBUGBUILD
  എങ്കിൽ(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  എങ്കിൽ(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    തിരികെ;
  }
#endif

  memset(&hints, 0, വലുപ്പം(hints));
#ifdef CURLRES_IPV6
  എങ്കിൽ(item->dns_queries & CURL_DNSQ_AAAA) {
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

  curl_msnprintf(service, വലുപ്പം(service), "%u", item->port);
#ifdef AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#endif

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  എങ്കിൽ(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    എങ്കിൽ(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  അല്ലെങ്കിൽ {
    Curl_addrinfo_set_port(item->res, item->port);
  }
}

#അല്ലെങ്കിൽ /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
സ്ഥിതം ശൂന്യം async_thrdd_item_process(ശൂന്യം *arg)
{
  ഘടന async_thrdd_item *item = arg;

#ifdef DEBUGBUILD
  എങ്കിൽ(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  എങ്കിൽ(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    തിരികെ;
  }
#endif
  item->res = Curl_ipv4_resolve_r(item->hostname, item->port);
  എങ്കിൽ(!item->res) {
    item->sockerr = SOCKERRNO;
    എങ്കിൽ(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#endif /* HAVE_GETADDRINFO */

#ifdef ENABLE_INTERNAL_WAKEUP
സ്ഥിതം ശൂന്യം async_thrdd_event(സ്ഥിരം ഘടന curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              ശൂന്യം *user_data)
{
  ഘടന Curl_multi *multi = user_data;
  (ശൂന്യം)tqueue;
  തിരഞ്ഞെടുപ്പ്(ev) {
  അവസ്ഥ CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
}
#അല്ലെങ്കിൽ
#define async_thrdd_event   ശൂന്യ
#endif

സ്ഥിതം ശൂന്യം async_thrdd_item_free(ശൂന്യം *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(ഘടന Curl_multi *multi,
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
  എങ്കിൽ(!result) {
    സ്ഥിരം അക്ഷരം *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    എങ്കിൽ(p) {
      curl_off_t l;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, UINT32_MAX)) {
        result = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#endif
  തിരികെ result;
}

/* Tear down the thread queue, joining active threads or detaching them */
ശൂന്യം Curl_async_thrdd_multi_destroy(ഘടന Curl_multi *multi, bool join)
{
  എങ്കിൽ(multi->resolv_thrdq) {
#ifdef CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#endif
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = ശൂന്യ;
  }
}

#ifdef CURLVERBOSE
സ്ഥിതം ശൂന്യം async_thrdd_report_item(ഘടന Curl_easy *data,
                                    ഘടന async_thrdd_item *item)
{
  അക്ഷരം buf[MAX_IPADR_LEN];
  ഘടന dynbuf tmp;
  സ്ഥിരം അക്ഷരം *sep = "";
  സ്ഥിരം ഘടന Curl_addrinfo *ai = item->res;
  CURLcode result;
  പൂർണ്ണസംഖ്യ ai_family;
#ifdef USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#അല്ലെങ്കിൽ
  ai_family = AF_INET;
#endif

  എങ്കിൽ(!CURL_TRC_DNS_is_verbose(data))
    തിരികെ;

  curlx_dyn_init(&tmp, 1024);
  വേണ്ടി(; ai; ai = ai->ai_next) {
    എങ്കിൽ(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, വലുപ്പം(buf));
      result = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      എങ്കിൽ(result) {
        CURL_TRC_DNS(data, "too many IP, cannot show");
        പോകുക out;
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
ശൂന്യം Curl_async_thrdd_multi_process(ഘടന Curl_multi *multi)
{
  ഘടന Curl_easy *data;
  ശൂന്യം *qitem;

  ആകുമ്പോൾ(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    ഘടന async_thrdd_item *item = qitem;
    ഘടന Curl_resolv_async *async = ശൂന്യ;

    data = Curl_multi_get_easy(multi, item->mid);
    എങ്കിൽ(data)
      async = Curl_async_get(data, item->resolv_id);
    എങ്കിൽ(async) {
      ഘടന async_thrdd_item **pdest = &async->thrdd.res_A;

#ifdef CURLRES_IPV6
      എങ്കിൽ(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#endif
      എങ്കിൽ(!*pdest) {
        *pdest = item;
        item = ശൂന്യ;
      }
      അല്ലെങ്കിൽ
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(data);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(ഘടന Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  തിരികെ Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

സ്ഥിതം CURLcode async_thrdd_query(ഘടന Curl_easy *data,
                                  ഘടന Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  ഘടന async_thrdd_item *item;
  CURLcode result;

  item = async_thrdd_item_create(data, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  എങ്കിൽ(!item) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  CURL_TRC_DNS(data, "queueing query %s", item->description);
  result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  എങ്കിൽ(result)
    പോകുക out;
  item = ശൂന്യ;
  async->queries_ongoing++;

out:
  എങ്കിൽ(item)
    async_thrdd_item_free(item);
  തിരികെ result;
}

CURLcode Curl_async_getaddrinfo(ഘടന Curl_easy *data,
                                ഘടന Curl_resolv_async *async)
{
  CURLcode result = CURLE_FAILED_INIT;
  ശൂന്യം *resolver = ശൂന്യ;

  എങ്കിൽ(async->queries_ongoing || async->done)
    തിരികെ CURLE_FAILED_INIT;

#ifdef USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  എങ്കിൽ((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    result = async_rr_init(data, async);
    എങ്കിൽ(result)
      പോകുക out;
    resolver = async->thrdd.rr.channel;
  }
#endif

  result = Curl_resolv_announce_start(data, resolver);
  എങ്കിൽ(result)
    തിരികെ result;

#ifdef USE_HTTPSRR_ARES
  എങ്കിൽ(resolver) {
    result = async_rr_start(data, async);
    എങ്കിൽ(result)
      പോകുക out;
  }
#endif

#ifdef CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  എങ്കിൽ((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    result = async_thrdd_query(data, async, CURL_DNSQ_AAAA);
    എങ്കിൽ(result)
      പോകുക out;
  }
#endif
  എങ്കിൽ(async->dns_queries & CURL_DNSQ_A) {
    result = async_thrdd_query(data, async, CURL_DNSQ_A);
    എങ്കിൽ(result)
      പോകുക out;
  }

  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  എങ്കിൽ(!async->queries_ongoing)
    async->done = TRUE;

  എങ്കിൽ(result)
    CURL_TRC_DNS(data, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

CURLcode Curl_async_pollset(ഘടന Curl_easy *data,
                            ഘടന Curl_resolv_async *async,
                            ഘടന easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(data, async);
#ifdef USE_HTTPSRR_ARES
  എങ്കിൽ(async->thrdd.rr.channel) {
    CURLcode result = Curl_ares_pollset(data, async->thrdd.rr.channel, ps);
    എങ്കിൽ(result)
      തിരികെ result;
    timeout_ms = Curl_ares_timeout_ms(data, async, async->thrdd.rr.channel);
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)ps;
#endif

  എങ്കിൽ(!async->done) {
    സ്ഥിരം ഘടന curltime *pnow = Curl_pgrs_now(data);
#ifndef ENABLE_INTERNAL_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    എങ്കിൽ(elapsed_ms < 3)
      stutter_ms = 1;
    അല്ലെങ്കിൽ എങ്കിൽ(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    അല്ലെങ്കിൽ എങ്കിൽ(elapsed_ms <= 250)
      stutter_ms = 50;
    അല്ലെങ്കിൽ
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#അല്ലെങ്കിൽ
    എങ്കിൽ(async->queries_ongoing &&
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
  തിരികെ CURLE_OK;
}

#എങ്കിൽ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
സ്ഥിതം bool async_thrdd_item_missing_scope(ഘടന Curl_easy *data,
                                           ഘടന async_thrdd_item *item)
{
  സ്ഥിരം ഘടന Curl_addrinfo *ai;

  /* scope id already globally set */
  എങ്കിൽ(data->conn && data->conn->scope_id)
    തിരികെ FALSE;

  വേണ്ടി(ai = item->res; ai; ai = ai->ai_next) {
    എങ്കിൽ(ai->ai_family == AF_INET6) {
      ഘടന sockaddr_in6 *sa6 = (ശൂന്യം *)ai->ai_addr;
      എങ്കിൽ(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        തിരികെ TRUE;
    }
  }
  തിരികെ FALSE;
}

സ്ഥിതം ശൂന്യം async_thrdd_item_strip_results(ഘടന async_thrdd_item *item,
                                           പൂർണ്ണസംഖ്യ ai_family)
{
  ഘടന Curl_addrinfo *ai = item->res, **panchor = &item->res;
  ആകുമ്പോൾ(ai) {
    എങ്കിൽ(ai->ai_family == ai_family) {
      *panchor = ai->ai_next;
      ai->ai_next = ശൂന്യ;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    അല്ലെങ്കിൽ {
      panchor = &ai->ai_next;
      ai = ai->ai_next;
    }
  }
}

#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

സ്ഥിതം CURLcode async_thrdd_check_done(ഘടന Curl_easy *data,
                                       ഘടന Curl_resolv_async *async)
{
  ഘടന async_thrdd_ctx *thrdd = &async->thrdd;

  (ശൂന്യം)data;
  എങ്കിൽ(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(data, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = ശൂന്യ;
    thrdd->processed_A = TRUE;
  }

  എങ്കിൽ(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#എങ്കിൽ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    എങ്കിൽ(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(data, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      ഘടന async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode result;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = ശൂന്യ;
      item->dns_queries |= CURL_DNSQ_A;
      എങ്കിൽ(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = ശൂന്യ;
      }

      CURL_TRC_DNS(data, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      എങ്കിൽ(result) {
        async_thrdd_item_free(item);
        തിരികെ result;
      }
      async->queries_ongoing++;
      തിരികെ CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(data, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = ശൂന്യ;
    thrdd->processed_AAAA = TRUE;
  }

  എങ്കിൽ(async->queries_ongoing)
    തിരികെ CURLE_AGAIN;
  async->done = TRUE;
  തിരികെ CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_result(ഘടന Curl_easy *data,
                                ഘടന Curl_resolv_async *async,
                                ഘടന Curl_dns_entry **pdns)
{
  ഘടന async_thrdd_ctx *thrdd = &async->thrdd;
  ഘടന Curl_dns_entry *dns = ശൂന്യ;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ശൂന്യ;

#ifdef USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  എങ്കിൽ(thrdd->rr.channel)
    (ശൂന്യം)Curl_ares_perform(thrdd->rr.channel, 0);
#endif
#ifndef ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(data->multi);
#endif

  result = async_thrdd_check_done(data, async);
  എങ്കിൽ(result)
    തിരികെ result;

  Curl_expire_clear(data, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    സ്ഥിരം uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    എങ്കിൽ(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    എങ്കിൽ(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    എങ്കിൽ(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  എങ്കിൽ(async->result) {
    result = async->result;
    പോകുക out;
  }

  എങ്കിൽ(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    എങ്കിൽ(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
  }

#ifdef USE_HTTPSRR_ARES
  എങ്കിൽ(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(data, async->httpsrr);
    dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
    എങ്കിൽ(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
  }
#endif

  എങ്കിൽ(dns) {
    *pdns = dns;
    dns = ശൂന്യ;
  }
  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  Curl_dns_entry_unlink(data, &dns);
  Curl_async_thrdd_shutdown(data, async);
  എങ്കിൽ(!result && !*pdns)
    result = Curl_async_failed(data, async, ശൂന്യ);
  എങ്കിൽ(result &&
     (result != CURLE_COULDNT_RESOLVE_HOST) &&
     (result != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(data, "Error %d resolving %s:%d",
                 (പൂർണ്ണസംഖ്യ)result, async->peer->hostname, async->peer->port);
  }
  തിരികെ result;
}

#endif /* USE_RESOLV_THREADED */
