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
#અન્તર્ભાવ "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ifdef USE_RESOLV_THREADED

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#ifdef HAVE_GETADDRINFO
#define RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#નહીંતર
#define RESOLVER_ENOMEM  SOCKENOMEM
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "curl_threads.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "thrdqueue.h"
#અન્તર્ભાવ "vdns/hostip.h"
#અન્તર્ભાવ "vdns/httpsrr.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "curlx/wait.h"

#ifdef USE_ARES
#અન્તર્ભાવ <ares.h>
#ifdef USE_HTTPSRR
#define USE_HTTPSRR_ARES  /* the combo */
#endif
#endif


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
પૂર્ણાંક Curl_async_global_init(શૂન્ય)
{
#જો defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  જો(ares_library_init(ARES_LIB_INIT_ALL)) {
    પાછા CURLE_FAILED_INIT;
  }
#endif
  પાછા CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
શૂન્ય Curl_async_global_cleanup(શૂન્ય)
{
#જો defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#endif
}

#ifdef CURLVERBOSE
#define CURL_ASYN_ITEM_DESC_LEN   64
#define async_item_description(x)   (x)->description
#નહીંતર
#define async_item_description(x)   ખાલી
#endif

સંરચના async_thrdd_item {
  સંરચના Curl_addrinfo *res;
#ifdef CURLVERBOSE
  અક્ષર description[CURL_ASYN_ITEM_DESC_LEN];
#endif
  પૂર્ણાંક sockerr;
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
  અક્ષર hostname[1];
};

/* Give up reference to add_ctx */
સ્થિર શૂન્ય async_thrdd_item_destroy(સંરચના async_thrdd_item *item)
{
  જો(item) {
    જો(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
સ્થિર સંરચના async_thrdd_item *async_thrdd_item_create(
  સંરચના Curl_easy *data,
  uint32_t resolv_id, uint8_t dns_queries,
  અચલ અક્ષર *hostname, uint16_t port,
  uint8_t transport)
{
  size_t hostlen = strlen(hostname);
  સંરચના async_thrdd_item *item;

  item = curlx_calloc(1, કદ(*item) + hostlen);
  જો(!item)
    પાછા ખાલી;

  જો(hostlen) /* null-terminator of name already in struct size */
    memcpy(item->hostname, hostname, hostlen);
  item->mid = data->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#ifdef CURLVERBOSE
  curl_msnprintf(item->description, કદ(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 data->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#endif

#ifdef DEBUGBUILD
  {
    અચલ અક્ષર *p = getenv("CURL_DBG_RESOLV_DELAY");
    જો(p) {
      curl_off_t l;
      જો(!curlx_str_સંખ્યા(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    જો(p) {
      curl_off_t l;
      જો(!curlx_str_સંખ્યા(&p, &l, UINT32_MAX)) {
        unsigned અક્ષર c = 0;
        Curl_rand_bytes(data, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    જો(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#endif

  પાછા item;
}

#ifdef USE_HTTPSRR_ARES

સ્થિર શૂન્ય async_thrdd_rr_done(શૂન્ય *user_data, ares_status_t status,
                                size_t timeouts,
                                અચલ ares_dns_record_t *dnsrec)
{
  સંરચના Curl_resolv_async *async = user_data;
  સંરચના async_thrdd_ctx *thrdd = async ? &async->thrdd : ખાલી;

  (શૂન્ય)timeouts;
  જો(!thrdd)
    પાછા;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  જો((ARES_SUCCESS == status) && dnsrec)
    async->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
સ્થિર CURLcode async_rr_init(સંરચના Curl_easy *data,
                              સંરચના Curl_resolv_async *async)
{
  સંરચના async_thrdd_ctx *thrdd = &async->thrdd;
  પૂર્ણાંક status;

  (શૂન્ય)data;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, ખાલી, 0);
  જો(status != ARES_SUCCESS) {
    thrdd->rr.channel = ખાલી;
    પાછા CURLE_FAILED_INIT;
  }
#ifdef DEBUGBUILD
  જો(getenv("CURL_DNS_SERVER")) {
    અચલ અક્ષર *servers = getenv("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    જો(status)
      પાછા CURLE_FAILED_INIT;
  }
#endif
  પાછા CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
સ્થિર CURLcode async_rr_start(સંરચના Curl_easy *data,
                               સંરચના Curl_resolv_async *async)
{
  સંરચના async_thrdd_ctx *thrdd = &async->thrdd;
  અક્ષર *https_name = ખાલી;
  CURLcode result = CURLE_OK;

  જો(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    જો(!https_name) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, ખાલી);
  CURL_TRC_DNS(data, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  પાછા result;
}
#endif

શૂન્ય Curl_async_thrdd_shutdown(સંરચના Curl_easy *data,
                               સંરચના Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(data, async);
}

સંરચના async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

સ્થિર bool async_thrdd_match_item(શૂન્ય *qitem, શૂન્ય *match_data)
{
  અચલ સંરચના async_thrdd_match_ctx *ctx = match_data;
  સંરચના async_thrdd_item *item = qitem;
  પાછા (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

શૂન્ય Curl_async_thrdd_destroy(સંરચના Curl_easy *data,
                              સંરચના Curl_resolv_async *async)
{
  (શૂન્ય)data;
  જો(async->queries_ongoing && !async->done &&
     data->multi && data->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    સંરચના async_thrdd_match_ctx mctx;
    mctx.mid = data->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(data->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#ifdef USE_HTTPSRR_ARES
  જો(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = ખાલી;
  }
#endif
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = ખાલી;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = ખાલી;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(સંરચના Curl_easy *data, uint32_t resolv_id,
                          સંરચના Curl_dns_entry **pdns)
{
  સંરચના Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  સંરચના async_thrdd_ctx *thrdd = async ? &async->thrdd : ખાલી;
  timediff_t milli, ms;
  CURLcode result = CURLE_AGAIN;

  જો(!thrdd)
    પાછા CURLE_FAILED_INIT;

  જ્યારે(result == CURLE_AGAIN) {
    જ્યારે(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(data->multi);
      જો(async->done)
        તોડો;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
      જો(ms < 3)
        milli = 0;
      નહીંતર જો(ms <= 50)
        milli = ms / 3;
      નહીંતર જો(ms <= 250)
        milli = 50;
      નહીંતર
        milli = 200;
      CURL_TRC_DNS(data, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    result = Curl_async_take_result(data, async, pdns);
  }
  પાછા result;
}

#ifdef HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
સ્થિર bool gai_negative(પૂર્ણાંક rc)
{
  પસંદગી(rc) {
#ifdef EAI_NONAME
  સ્થિતિ EAI_NONAME:
#endif
#જો defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  સ્થિતિ EAI_NODATA:
#endif
    પાછા TRUE;
  મૂળભૂત:
    પાછા FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
સ્થિર શૂન્ય async_thrdd_item_process(શૂન્ય *arg)
{
  સંરચના async_thrdd_item *item = arg;
  સંરચના addrinfo hints;
  અક્ષર service[12];
  પૂર્ણાંક pf = PF_INET;
  પૂર્ણાંક rc;

#ifdef DEBUGBUILD
  જો(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  જો(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    પાછા;
  }
#endif

  memset(&hints, 0, કદ(hints));
#ifdef CURLRES_IPV6
  જો(item->dns_queries & CURL_DNSQ_AAAA) {
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

  curl_msnprintf(service, કદ(service), "%u", item->port);
#ifdef AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#endif

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  જો(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    જો(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  નહીંતર {
    Curl_addrinfo_set_port(item->res, item->port);
  }
}

#નહીંતર /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
સ્થિર શૂન્ય async_thrdd_item_process(શૂન્ય *arg)
{
  સંરચના async_thrdd_item *item = arg;

#ifdef DEBUGBUILD
  જો(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  જો(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    પાછા;
  }
#endif
  item->res = Curl_ipv4_resolve_r(item->hostname, item->port);
  જો(!item->res) {
    item->sockerr = SOCKERRNO;
    જો(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#endif /* HAVE_GETADDRINFO */

#ifdef ENABLE_INTERNAL_WAKEUP
સ્થિર શૂન્ય async_thrdd_event(અચલ સંરચના curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              શૂન્ય *user_data)
{
  સંરચના Curl_multi *multi = user_data;
  (શૂન્ય)tqueue;
  પસંદગી(ev) {
  સ્થિતિ CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    તોડો;
  મૂળભૂત:
    તોડો;
  }
}
#નહીંતર
#define async_thrdd_event   ખાલી
#endif

સ્થિર શૂન્ય async_thrdd_item_free(શૂન્ય *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(સંરચના Curl_multi *multi,
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
  જો(!result) {
    અચલ અક્ષર *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    જો(p) {
      curl_off_t l;
      જો(!curlx_str_સંખ્યા(&p, &l, UINT32_MAX)) {
        result = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#endif
  પાછા result;
}

/* Tear down the thread queue, joining active threads or detaching them */
શૂન્ય Curl_async_thrdd_multi_destroy(સંરચના Curl_multi *multi, bool join)
{
  જો(multi->resolv_thrdq) {
#ifdef CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#endif
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = ખાલી;
  }
}

#ifdef CURLVERBOSE
સ્થિર શૂન્ય async_thrdd_report_item(સંરચના Curl_easy *data,
                                    સંરચના async_thrdd_item *item)
{
  અક્ષર buf[MAX_IPADR_LEN];
  સંરચના dynbuf tmp;
  અચલ અક્ષર *sep = "";
  અચલ સંરચના Curl_addrinfo *ai = item->res;
  CURLcode result;
  પૂર્ણાંક ai_family;
#ifdef USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#નહીંતર
  ai_family = AF_INET;
#endif

  જો(!CURL_TRC_DNS_is_verbose(data))
    પાછા;

  curlx_dyn_init(&tmp, 1024);
  માટે(; ai; ai = ai->ai_next) {
    જો(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, કદ(buf));
      result = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      જો(result) {
        CURL_TRC_DNS(data, "too many IP, cannot show");
        જાઓ out;
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
શૂન્ય Curl_async_thrdd_multi_process(સંરચના Curl_multi *multi)
{
  સંરચના Curl_easy *data;
  શૂન્ય *qitem;

  જ્યારે(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    સંરચના async_thrdd_item *item = qitem;
    સંરચના Curl_resolv_async *async = ખાલી;

    data = Curl_multi_get_easy(multi, item->mid);
    જો(data)
      async = Curl_async_get(data, item->resolv_id);
    જો(async) {
      સંરચના async_thrdd_item **pdest = &async->thrdd.res_A;

#ifdef CURLRES_IPV6
      જો(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#endif
      જો(!*pdest) {
        *pdest = item;
        item = ખાલી;
      }
      નહીંતર
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(data);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(સંરચના Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  પાછા Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

સ્થિર CURLcode async_thrdd_query(સંરચના Curl_easy *data,
                                  સંરચના Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  સંરચના async_thrdd_item *item;
  CURLcode result;

  item = async_thrdd_item_create(data, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  જો(!item) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  CURL_TRC_DNS(data, "queueing query %s", item->description);
  result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  જો(result)
    જાઓ out;
  item = ખાલી;
  async->queries_ongoing++;

out:
  જો(item)
    async_thrdd_item_free(item);
  પાછા result;
}

CURLcode Curl_async_getaddrinfo(સંરચના Curl_easy *data,
                                સંરચના Curl_resolv_async *async)
{
  CURLcode result = CURLE_FAILED_INIT;
  શૂન્ય *resolver = ખાલી;

  જો(async->queries_ongoing || async->done)
    પાછા CURLE_FAILED_INIT;

#ifdef USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  જો((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    result = async_rr_init(data, async);
    જો(result)
      જાઓ out;
    resolver = async->thrdd.rr.channel;
  }
#endif

  result = Curl_resolv_announce_start(data, resolver);
  જો(result)
    પાછા result;

#ifdef USE_HTTPSRR_ARES
  જો(resolver) {
    result = async_rr_start(data, async);
    જો(result)
      જાઓ out;
  }
#endif

#ifdef CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  જો((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    result = async_thrdd_query(data, async, CURL_DNSQ_AAAA);
    જો(result)
      જાઓ out;
  }
#endif
  જો(async->dns_queries & CURL_DNSQ_A) {
    result = async_thrdd_query(data, async, CURL_DNSQ_A);
    જો(result)
      જાઓ out;
  }

  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  જો(!async->queries_ongoing)
    async->done = TRUE;

  જો(result)
    CURL_TRC_DNS(data, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (પૂર્ણાંક)result);
  પાછા result;
}

CURLcode Curl_async_pollset(સંરચના Curl_easy *data,
                            સંરચના Curl_resolv_async *async,
                            સંરચના easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(data, async);
#ifdef USE_HTTPSRR_ARES
  જો(async->thrdd.rr.channel) {
    CURLcode result = Curl_ares_pollset(data, async->thrdd.rr.channel, ps);
    જો(result)
      પાછા result;
    timeout_ms = Curl_ares_timeout_ms(data, async, async->thrdd.rr.channel);
  }
#નહીંતર
  (શૂન્ય)ps;
#endif

  જો(!async->done) {
    અચલ સંરચના curltime *pnow = Curl_pgrs_now(data);
#ifndef ENABLE_INTERNAL_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    જો(elapsed_ms < 3)
      stutter_ms = 1;
    નહીંતર જો(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    નહીંતર જો(elapsed_ms <= 250)
      stutter_ms = 50;
    નહીંતર
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#નહીંતર
    જો(async->queries_ongoing &&
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
  પાછા CURLE_OK;
}

#જો defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
સ્થિર bool async_thrdd_item_missing_scope(સંરચના Curl_easy *data,
                                           સંરચના async_thrdd_item *item)
{
  અચલ સંરચના Curl_addrinfo *ai;

  /* scope id already globally set */
  જો(data->conn && data->conn->scope_id)
    પાછા FALSE;

  માટે(ai = item->res; ai; ai = ai->ai_next) {
    જો(ai->ai_family == AF_INET6) {
      સંરચના sockaddr_in6 *sa6 = (શૂન્ય *)ai->ai_addr;
      જો(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        પાછા TRUE;
    }
  }
  પાછા FALSE;
}

સ્થિર શૂન્ય async_thrdd_item_strip_results(સંરચના async_thrdd_item *item,
                                           પૂર્ણાંક ai_family)
{
  સંરચના Curl_addrinfo *ai = item->res, **panchor = &item->res;
  જ્યારે(ai) {
    જો(ai->ai_family == ai_family) {
      *panchor = ai->ai_next;
      ai->ai_next = ખાલી;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    નહીંતર {
      panchor = &ai->ai_next;
      ai = ai->ai_next;
    }
  }
}

#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

સ્થિર CURLcode async_thrdd_check_done(સંરચના Curl_easy *data,
                                       સંરચના Curl_resolv_async *async)
{
  સંરચના async_thrdd_ctx *thrdd = &async->thrdd;

  (શૂન્ય)data;
  જો(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(data, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = ખાલી;
    thrdd->processed_A = TRUE;
  }

  જો(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#જો defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    જો(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(data, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      સંરચના async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode result;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = ખાલી;
      item->dns_queries |= CURL_DNSQ_A;
      જો(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = ખાલી;
      }

      CURL_TRC_DNS(data, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      જો(result) {
        async_thrdd_item_free(item);
        પાછા result;
      }
      async->queries_ongoing++;
      પાછા CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(data, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = ખાલી;
    thrdd->processed_AAAA = TRUE;
  }

  જો(async->queries_ongoing)
    પાછા CURLE_AGAIN;
  async->done = TRUE;
  પાછા CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_result(સંરચના Curl_easy *data,
                                સંરચના Curl_resolv_async *async,
                                સંરચના Curl_dns_entry **pdns)
{
  સંરચના async_thrdd_ctx *thrdd = &async->thrdd;
  સંરચના Curl_dns_entry *dns = ખાલી;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ખાલી;

#ifdef USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  જો(thrdd->rr.channel)
    (શૂન્ય)Curl_ares_perform(thrdd->rr.channel, 0);
#endif
#ifndef ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(data->multi);
#endif

  result = async_thrdd_check_done(data, async);
  જો(result)
    પાછા result;

  Curl_expire_clear(data, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    અચલ uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    જો(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    જો(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    જો(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  જો(async->result) {
    result = async->result;
    જાઓ out;
  }

  જો(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    જો(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }

#ifdef USE_HTTPSRR_ARES
  જો(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(data, async->httpsrr);
    dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
    જો(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }
#endif

  જો(dns) {
    *pdns = dns;
    dns = ખાલી;
  }
  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  Curl_dns_entry_unlink(data, &dns);
  Curl_async_thrdd_shutdown(data, async);
  જો(!result && !*pdns)
    result = Curl_async_failed(data, async, ખાલી);
  જો(result &&
     (result != CURLE_COULDNT_RESOLVE_HOST) &&
     (result != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(data, "Error %d resolving %s:%d",
                 (પૂર્ણાંક)result, async->peer->hostname, async->peer->port);
  }
  પાછા result;
}

#endif /* USE_RESOLV_THREADED */
