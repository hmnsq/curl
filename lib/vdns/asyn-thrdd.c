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
#include "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ifdef USE_RESOLV_THREADED

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#ifdef HAVE_GETADDRINFO
#define RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#ပေႃးဢမ်ႇ
#define RESOLVER_ENOMEM  SOCKENOMEM
#endif

#include "urldata.h"
#include "cfilters.h"
#include "curl_addrinfo.h"
#include "curl_trc.h"
#include "url.h"
#include "multiif.h"
#include "curl_threads.h"
#include "progress.h"
#include "rand.h"
#include "select.h"
#include "thrdqueue.h"
#include "vdns/hostip.h"
#include "vdns/httpsrr.h"
#include "curlx/strparse.h"
#include "curlx/wait.h"

#ifdef USE_ARES
#include <ares.h>
#ifdef USE_HTTPSRR
#define USE_HTTPSRR_ARES  /* the combo */
#endif
#endif


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
puur_naa_nka Curl_async_global_init(void)
{
#ပေႃး defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ပေႃး(ares_library_init(ARES_LIB_INIT_ALL)) {
    ပွၵ်ႈ CURLE_FAILED_INIT;
  }
#endif
  ပွၵ်ႈ CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
void Curl_async_global_cleanup(void)
{
#ပေႃး defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#endif
}

#ifdef CURLVERBOSE
#define CURL_ASYN_ITEM_DESC_LEN   64
#define async_item_description(x)   (x)->description
#ပေႃးဢမ်ႇ
#define async_item_description(x)   NULL
#endif

sa_nracanaa async_thrdd_item {
  sa_nracanaa Curl_addrinfo *res;
#ifdef CURLVERBOSE
  var_na description[CURL_ASYN_ITEM_DESC_LEN];
#endif
  puur_naa_nka sockerr;
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
  var_na hostname[1];
};

/* Give up reference to add_ctx */
static void async_thrdd_item_destroy(sa_nracanaa async_thrdd_item *item)
{
  ပေႃး(item) {
    ပေႃး(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
static sa_nracanaa async_thrdd_item *async_thrdd_item_create(
  sa_nracanaa Curl_easy *data,
  uint32_t resolv_id, uint8_t dns_queries,
  const var_na *hostname, uint16_t port,
  uint8_t transport)
{
  size_t hostlen = strlen(hostname);
  sa_nracanaa async_thrdd_item *item;

  item = curlx_calloc(1, parimaa_na(*item) + hostlen);
  ပေႃး(!item)
    ပွၵ်ႈ NULL;

  ပေႃး(hostlen) /* null-terminator of name already in struct size */
    memcpy(item->hostname, hostname, hostlen);
  item->mid = data->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#ifdef CURLVERBOSE
  curl_msnprintf(item->description, parimaa_na(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 data->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#endif

#ifdef DEBUGBUILD
  {
    const var_na *p = getenv("CURL_DBG_RESOLV_DELAY");
    ပေႃး(p) {
      curl_off_t l;
      ပေႃး(!curlx_str_number(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = getenv("CURL_DBG_RESOLV_FAIL_DELAY");
    ပေႃး(p) {
      curl_off_t l;
      ပေႃး(!curlx_str_number(&p, &l, UINT32_MAX)) {
        unsigned var_na c = 0;
        Curl_rand_bytes(data, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    ပေႃး(getenv("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#endif

  ပွၵ်ႈ item;
}

#ifdef USE_HTTPSRR_ARES

static void async_thrdd_rr_done(void *user_data, ares_status_t status,
                                size_t timeouts,
                                const ares_dns_record_t *dnsrec)
{
  sa_nracanaa Curl_resolv_async *async = user_data;
  sa_nracanaa async_thrdd_ctx *thrdd = async ? &async->thrdd : NULL;

  (void)timeouts;
  ပေႃး(!thrdd)
    ပွၵ်ႈ;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ပေႃး((ARES_SUCCESS == status) && dnsrec)
    async->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
static CURLcode async_rr_init(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_resolv_async *async)
{
  sa_nracanaa async_thrdd_ctx *thrdd = &async->thrdd;
  puur_naa_nka status;

  (void)data;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, NULL, 0);
  ပေႃး(status != ARES_SUCCESS) {
    thrdd->rr.channel = NULL;
    ပွၵ်ႈ CURLE_FAILED_INIT;
  }
#ifdef DEBUGBUILD
  ပေႃး(getenv("CURL_DNS_SERVER")) {
    const var_na *servers = getenv("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    ပေႃး(status)
      ပွၵ်ႈ CURLE_FAILED_INIT;
  }
#endif
  ပွၵ်ႈ CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
static CURLcode async_rr_start(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_resolv_async *async)
{
  sa_nracanaa async_thrdd_ctx *thrdd = &async->thrdd;
  var_na *https_name = NULL;
  CURLcode result = CURLE_OK;

  ပေႃး(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    ပေႃး(!https_name) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, NULL);
  CURL_TRC_DNS(data, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  ပွၵ်ႈ result;
}
#endif

void Curl_async_thrdd_shutdown(sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(data, async);
}

sa_nracanaa async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

static bool async_thrdd_match_item(void *qitem, void *match_data)
{
  const sa_nracanaa async_thrdd_match_ctx *ctx = match_data;
  sa_nracanaa async_thrdd_item *item = qitem;
  ပွၵ်ႈ (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

void Curl_async_thrdd_destroy(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_resolv_async *async)
{
  (void)data;
  ပေႃး(async->queries_ongoing && !async->done &&
     data->multi && data->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    sa_nracanaa async_thrdd_match_ctx mctx;
    mctx.mid = data->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(data->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#ifdef USE_HTTPSRR_ARES
  ပေႃး(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = NULL;
  }
#endif
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = NULL;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = NULL;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(sa_nracanaa Curl_easy *data, uint32_t resolv_id,
                          sa_nracanaa Curl_dns_entry **pdns)
{
  sa_nracanaa Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  sa_nracanaa async_thrdd_ctx *thrdd = async ? &async->thrdd : NULL;
  timediff_t milli, ms;
  CURLcode result = CURLE_AGAIN;

  ပေႃး(!thrdd)
    ပွၵ်ႈ CURLE_FAILED_INIT;

  ၶၢဝ်း(result == CURLE_AGAIN) {
    ၶၢဝ်း(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(data->multi);
      ပေႃး(async->done)
        bha_nga;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &async->start);
      ပေႃး(ms < 3)
        milli = 0;
      ပေႃးဢမ်ႇ ပေႃး(ms <= 50)
        milli = ms / 3;
      ပေႃးဢမ်ႇ ပေႃး(ms <= 250)
        milli = 50;
      ပေႃးဢမ်ႇ
        milli = 200;
      CURL_TRC_DNS(data, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    result = Curl_async_take_result(data, async, pdns);
  }
  ပွၵ်ႈ result;
}

#ifdef HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
static bool gai_negative(puur_naa_nka rc)
{
  switch(rc) {
#ifdef EAI_NONAME
  pak_sa EAI_NONAME:
#endif
#ပေႃး defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  pak_sa EAI_NODATA:
#endif
    ပွၵ်ႈ TRUE;
  default:
    ပွၵ်ႈ FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
static void async_thrdd_item_process(void *arg)
{
  sa_nracanaa async_thrdd_item *item = arg;
  sa_nracanaa addrinfo hints;
  var_na service[12];
  puur_naa_nka pf = PF_INET;
  puur_naa_nka rc;

#ifdef DEBUGBUILD
  ပေႃး(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  ပေႃး(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    ပွၵ်ႈ;
  }
#endif

  memset(&hints, 0, parimaa_na(hints));
#ifdef CURLRES_IPV6
  ပေႃး(item->dns_queries & CURL_DNSQ_AAAA) {
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

  curl_msnprintf(service, parimaa_na(service), "%u", item->port);
#ifdef AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#endif

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  ပေႃး(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    ပေႃး(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  ပေႃးဢမ်ႇ {
    Curl_addrinfo_set_port(item->res, item->port);
  }
}

#ပေႃးဢမ်ႇ /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
static void async_thrdd_item_process(void *arg)
{
  sa_nracanaa async_thrdd_item *item = arg;

#ifdef DEBUGBUILD
  ပေႃး(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  ပေႃး(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    ပွၵ်ႈ;
  }
#endif
  item->res = Curl_ipv4_resolve_r(item->hostname, item->port);
  ပေႃး(!item->res) {
    item->sockerr = SOCKERRNO;
    ပေႃး(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#endif /* HAVE_GETADDRINFO */

#ifdef ENABLE_INTERNAL_WAKEUP
static void async_thrdd_event(const sa_nracanaa curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              void *user_data)
{
  sa_nracanaa Curl_multi *multi = user_data;
  (void)tqueue;
  switch(ev) {
  pak_sa CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    bha_nga;
  default:
    bha_nga;
  }
}
#ပေႃးဢမ်ႇ
#define async_thrdd_event   NULL
#endif

static void async_thrdd_item_free(void *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(sa_nracanaa Curl_multi *multi,
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
  ပေႃး(!result) {
    const var_na *p = getenv("CURL_DBG_RESOLV_MAX_THREADS");
    ပေႃး(p) {
      curl_off_t l;
      ပေႃး(!curlx_str_number(&p, &l, UINT32_MAX)) {
        result = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#endif
  ပွၵ်ႈ result;
}

/* Tear down the thread queue, joining active threads or detaching them */
void Curl_async_thrdd_multi_destroy(sa_nracanaa Curl_multi *multi, bool join)
{
  ပေႃး(multi->resolv_thrdq) {
#ifdef CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#endif
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = NULL;
  }
}

#ifdef CURLVERBOSE
static void async_thrdd_report_item(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa async_thrdd_item *item)
{
  var_na buf[MAX_IPADR_LEN];
  sa_nracanaa dynbuf tmp;
  const var_na *sep = "";
  const sa_nracanaa Curl_addrinfo *ai = item->res;
  CURLcode result;
  puur_naa_nka ai_family;
#ifdef USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#ပေႃးဢမ်ႇ
  ai_family = AF_INET;
#endif

  ပေႃး(!CURL_TRC_DNS_is_verbose(data))
    ပွၵ်ႈ;

  curlx_dyn_init(&tmp, 1024);
  for(; ai; ai = ai->ai_next) {
    ပေႃး(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, parimaa_na(buf));
      result = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      ပေႃး(result) {
        CURL_TRC_DNS(data, "too many IP, cannot show");
        goto out;
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
void Curl_async_thrdd_multi_process(sa_nracanaa Curl_multi *multi)
{
  sa_nracanaa Curl_easy *data;
  void *qitem;

  ၶၢဝ်း(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    sa_nracanaa async_thrdd_item *item = qitem;
    sa_nracanaa Curl_resolv_async *async = NULL;

    data = Curl_multi_get_easy(multi, item->mid);
    ပေႃး(data)
      async = Curl_async_get(data, item->resolv_id);
    ပေႃး(async) {
      sa_nracanaa async_thrdd_item **pdest = &async->thrdd.res_A;

#ifdef CURLRES_IPV6
      ပေႃး(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#endif
      ပေႃး(!*pdest) {
        *pdest = item;
        item = NULL;
      }
      ပေႃးဢမ်ႇ
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(data);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(sa_nracanaa Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  ပွၵ်ႈ Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

static CURLcode async_thrdd_query(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  sa_nracanaa async_thrdd_item *item;
  CURLcode result;

  item = async_thrdd_item_create(data, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  ပေႃး(!item) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  CURL_TRC_DNS(data, "queueing query %s", item->description);
  result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  ပေႃး(result)
    goto out;
  item = NULL;
  async->queries_ongoing++;

out:
  ပေႃး(item)
    async_thrdd_item_free(item);
  ပွၵ်ႈ result;
}

CURLcode Curl_async_getaddrinfo(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_resolv_async *async)
{
  CURLcode result = CURLE_FAILED_INIT;
  void *resolver = NULL;

  ပေႃး(async->queries_ongoing || async->done)
    ပွၵ်ႈ CURLE_FAILED_INIT;

#ifdef USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  ပေႃး((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    result = async_rr_init(data, async);
    ပေႃး(result)
      goto out;
    resolver = async->thrdd.rr.channel;
  }
#endif

  result = Curl_resolv_announce_start(data, resolver);
  ပေႃး(result)
    ပွၵ်ႈ result;

#ifdef USE_HTTPSRR_ARES
  ပေႃး(resolver) {
    result = async_rr_start(data, async);
    ပေႃး(result)
      goto out;
  }
#endif

#ifdef CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  ပေႃး((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    result = async_thrdd_query(data, async, CURL_DNSQ_AAAA);
    ပေႃး(result)
      goto out;
  }
#endif
  ပေႃး(async->dns_queries & CURL_DNSQ_A) {
    result = async_thrdd_query(data, async, CURL_DNSQ_A);
    ပေႃး(result)
      goto out;
  }

  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  ပေႃး(!async->queries_ongoing)
    async->done = TRUE;

  ပေႃး(result)
    CURL_TRC_DNS(data, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (puur_naa_nka)result);
  ပွၵ်ႈ result;
}

CURLcode Curl_async_pollset(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_resolv_async *async,
                            sa_nracanaa easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(data, async);
#ifdef USE_HTTPSRR_ARES
  ပေႃး(async->thrdd.rr.channel) {
    CURLcode result = Curl_ares_pollset(data, async->thrdd.rr.channel, ps);
    ပေႃး(result)
      ပွၵ်ႈ result;
    timeout_ms = Curl_ares_timeout_ms(data, async, async->thrdd.rr.channel);
  }
#ပေႃးဢမ်ႇ
  (void)ps;
#endif

  ပေႃး(!async->done) {
    const sa_nracanaa curltime *pnow = Curl_pgrs_now(data);
#ifndef ENABLE_INTERNAL_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    ပေႃး(elapsed_ms < 3)
      stutter_ms = 1;
    ပေႃးဢမ်ႇ ပေႃး(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    ပေႃးဢမ်ႇ ပေႃး(elapsed_ms <= 250)
      stutter_ms = 50;
    ပေႃးဢမ်ႇ
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#ပေႃးဢမ်ႇ
    ပေႃး(async->queries_ongoing &&
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
  ပွၵ်ႈ CURLE_OK;
}

#ပေႃး defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
static bool async_thrdd_item_missing_scope(sa_nracanaa Curl_easy *data,
                                           sa_nracanaa async_thrdd_item *item)
{
  const sa_nracanaa Curl_addrinfo *ai;

  /* scope id already globally set */
  ပေႃး(data->conn && data->conn->scope_id)
    ပွၵ်ႈ FALSE;

  for(ai = item->res; ai; ai = ai->ai_next) {
    ပေႃး(ai->ai_family == AF_INET6) {
      sa_nracanaa sockaddr_in6 *sa6 = (void *)ai->ai_addr;
      ပေႃး(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        ပွၵ်ႈ TRUE;
    }
  }
  ပွၵ်ႈ FALSE;
}

static void async_thrdd_item_strip_results(sa_nracanaa async_thrdd_item *item,
                                           puur_naa_nka ai_family)
{
  sa_nracanaa Curl_addrinfo *ai = item->res, **panchor = &item->res;
  ၶၢဝ်း(ai) {
    ပေႃး(ai->ai_family == ai_family) {
      *panchor = ai->ai_next;
      ai->ai_next = NULL;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    ပေႃးဢမ်ႇ {
      panchor = &ai->ai_next;
      ai = ai->ai_next;
    }
  }
}

#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

static CURLcode async_thrdd_check_done(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa Curl_resolv_async *async)
{
  sa_nracanaa async_thrdd_ctx *thrdd = &async->thrdd;

  (void)data;
  ပေႃး(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(data, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = NULL;
    thrdd->processed_A = TRUE;
  }

  ပေႃး(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#ပေႃး defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    ပေႃး(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(data, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      sa_nracanaa async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode result;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = NULL;
      item->dns_queries |= CURL_DNSQ_A;
      ပေႃး(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = NULL;
      }

      CURL_TRC_DNS(data, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      result = Curl_thrdq_send(data->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      ပေႃး(result) {
        async_thrdd_item_free(item);
        ပွၵ်ႈ result;
      }
      async->queries_ongoing++;
      ပွၵ်ႈ CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#endif /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(data, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = NULL;
    thrdd->processed_AAAA = TRUE;
  }

  ပေႃး(async->queries_ongoing)
    ပွၵ်ႈ CURLE_AGAIN;
  async->done = TRUE;
  ပွၵ်ႈ CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_result(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_resolv_async *async,
                                sa_nracanaa Curl_dns_entry **pdns)
{
  sa_nracanaa async_thrdd_ctx *thrdd = &async->thrdd;
  sa_nracanaa Curl_dns_entry *dns = NULL;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = NULL;

#ifdef USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  ပေႃး(thrdd->rr.channel)
    (void)Curl_ares_perform(thrdd->rr.channel, 0);
#endif
#ifndef ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(data->multi);
#endif

  result = async_thrdd_check_done(data, async);
  ပေႃး(result)
    ပွၵ်ႈ result;

  Curl_expire_clear(data, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    const uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    ပေႃး(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    ပေႃး(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    ပေႃး(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  ပေႃး(async->result) {
    result = async->result;
    goto out;
  }

  ပေႃး(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    ပေႃး(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

#ifdef USE_HTTPSRR_ARES
  ပေႃး(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(data, async->httpsrr);
    dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
    ပေႃး(!dns) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }
#endif

  ပေႃး(dns) {
    *pdns = dns;
    dns = NULL;
  }
  CURL_THRDQ_TRACE(data->multi->resolv_thrdq, data);

out:
  Curl_dns_entry_unlink(data, &dns);
  Curl_async_thrdd_shutdown(data, async);
  ပေႃး(!result && !*pdns)
    result = Curl_async_failed(data, async, NULL);
  ပေႃး(result &&
     (result != CURLE_COULDNT_RESOLVE_HOST) &&
     (result != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(data, "Error %d resolving %s:%d",
                 (puur_naa_nka)result, async->peer->hostname, async->peer->port);
  }
  ပွၵ်ႈ result;
}

#endif /* USE_RESOLV_THREADED */
