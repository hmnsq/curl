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
#समावेश "curl_setup.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#अगर_घोषित USE_RESOLV_THREADED

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_GETADDRINFO
#घोषणा RESOLVER_ENOMEM  EAI_MEMORY  /* = WSA_NOT_ENOUGH_MEMORY on Windows */
#अन्यथा
#घोषणा RESOLVER_ENOMEM  SOCKENOMEM
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "url.h"
#समावेश "multiif.h"
#समावेश "curl_threads.h"
#समावेश "progress.h"
#समावेश "rand.h"
#समावेश "select.h"
#समावेश "thrdqueue.h"
#समावेश "vdns/hostip.h"
#समावेश "vdns/httpsrr.h"
#समावेश "curlx/strparse.h"
#समावेश "curlx/wait.h"

#अगर_घोषित USE_ARES
#समावेश <ares.h>
#अगर_घोषित USE_HTTPSRR
#घोषणा USE_HTTPSRR_ARES  /* the combo */
#पूर्ण_अगर
#पूर्ण_अगर


/*
 * Curl_async_global_init()
 * Called from curl_global_init() to initialize global resolver environment.
 * Does nothing here.
 */
पूर्णांक Curl_async_global_init(व्योम)
{
#अगर defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  अगर(ares_library_init(ARES_LIB_INIT_ALL)) {
    वापस CURLE_FAILED_INIT;
  }
#पूर्ण_अगर
  वापस CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
व्योम Curl_async_global_cleanup(व्योम)
{
#अगर defined(USE_ARES) && defined(CARES_HAVE_ARES_LIBRARY_INIT)
  ares_library_cleanup();
#पूर्ण_अगर
}

#अगर_घोषित CURLVERBOSE
#घोषणा CURL_ASYN_ITEM_DESC_LEN   64
#घोषणा async_item_description(x)   (x)->description
#अन्यथा
#घोषणा async_item_description(x)   शून्य
#पूर्ण_अगर

काष्ठा async_thrdd_item {
  काष्ठा Curl_addrinfo *res;
#अगर_घोषित CURLVERBOSE
  अक्षर description[CURL_ASYN_ITEM_DESC_LEN];
#पूर्ण_अगर
  पूर्णांक sockerr;
  uint32_t mid;
  uint32_t resolv_id;
  uint16_t port;
  uint8_t transport;
  uint8_t dns_queries;
  BIT(negative); /* resolver answered that the name does not exist */
#अगर_घोषित DEBUGBUILD
  uint32_t delay_ms;
  uint32_t delay_fail_ms;
  BIT(dbg_negative);
#पूर्ण_अगर
  अक्षर hostname[1];
};

/* Give up reference to add_ctx */
अटल व्योम async_thrdd_item_destroy(काष्ठा async_thrdd_item *item)
{
  अगर(item) {
    अगर(item->res)
      Curl_freeaddrinfo(item->res);
    curlx_free(item);
  }
}

/* Initialize context for threaded resolver */
अटल काष्ठा async_thrdd_item *async_thrdd_item_create(
  काष्ठा Curl_easy *तथ्य,
  uint32_t resolv_id, uint8_t dns_queries,
  स्थिर अक्षर *hostname, uint16_t port,
  uint8_t transport)
{
  माप_प्रकार hostlen = म_माप(hostname);
  काष्ठा async_thrdd_item *item;

  item = curlx_calloc(1, माप(*item) + hostlen);
  अगर(!item)
    वापस शून्य;

  अगर(hostlen) /* null-terminator of name already in struct size */
    स_नकल(item->hostname, hostname, hostlen);
  item->mid = तथ्य->mid;
  item->resolv_id = resolv_id;
  item->dns_queries = dns_queries;
  item->port = port;
  item->transport = transport;

#अगर_घोषित CURLVERBOSE
  curl_msnprintf(item->description, माप(item->description),
                 "[%" FMT_OFF_T "/%u] %s %s:%u",
                 तथ्य->id, item->resolv_id,
                 Curl_resolv_query_str(dns_queries),
                 item->hostname, item->port);
#पूर्ण_अगर

#अगर_घोषित DEBUGBUILD
  {
    स्थिर अक्षर *p = दो_पर्या("CURL_DBG_RESOLV_DELAY");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, UINT32_MAX)) {
        item->delay_ms = (uint32_t)l;
      }
    }
    p = दो_पर्या("CURL_DBG_RESOLV_FAIL_DELAY");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, UINT32_MAX)) {
        अचिन्हित अक्षर c = 0;
        Curl_rand_bytes(तथ्य, FALSE, &c, 1);
        item->delay_fail_ms = (uint32_t)l + c;
      }
    }
    अगर(दो_पर्या("CURL_DBG_RESOLV_FAIL_NEGATIVE"))
      item->dbg_negative = TRUE;
  }
#पूर्ण_अगर

  वापस item;
}

#अगर_घोषित USE_HTTPSRR_ARES

अटल व्योम async_thrdd_rr_done(व्योम *user_तथ्य, ares_status_t status,
                                माप_प्रकार timeouts,
                                स्थिर ares_dns_record_t *dnsrec)
{
  काष्ठा Curl_resolv_async *async = user_तथ्य;
  काष्ठा async_thrdd_ctx *thrdd = async ? &async->thrdd : शून्य;

  (व्योम)timeouts;
  अगर(!thrdd)
    वापस;

  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  अगर((ARES_SUCCESS == status) && dnsrec)
    async->परिणाम = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}

/* Initialize the c-ares channel used for the HTTPS RR side-query, without
 * submitting any query on it yet. */
अटल CURLcode async_rr_init(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_resolv_async *async)
{
  काष्ठा async_thrdd_ctx *thrdd = &async->thrdd;
  पूर्णांक status;

  (व्योम)तथ्य;
  DEBUGASSERT(!thrdd->rr.channel);
  status = ares_init_options(&thrdd->rr.channel, शून्य, 0);
  अगर(status != ARES_SUCCESS) {
    thrdd->rr.channel = शून्य;
    वापस CURLE_FAILED_INIT;
  }
#अगर_घोषित DEBUGBUILD
  अगर(दो_पर्या("CURL_DNS_SERVER")) {
    स्थिर अक्षर *servers = दो_पर्या("CURL_DNS_SERVER");
    status = ares_set_servers_ports_csv(thrdd->rr.channel, servers);
    अगर(status)
      वापस CURLE_FAILED_INIT;
  }
#पूर्ण_अगर
  वापस CURLE_OK;
}

/* Submit the HTTPS RR query on the already initialized channel. */
अटल CURLcode async_rr_start(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_resolv_async *async)
{
  काष्ठा async_thrdd_ctx *thrdd = &async->thrdd;
  अक्षर *https_name = शून्य;
  CURLcode परिणाम = CURLE_OK;

  अगर(async->peer->port != 443) {
    https_name = curl_maprintf("_%u._https.%s",
                               async->peer->port, async->peer->hostname);
    अगर(!https_name) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }

  async->queries_ongoing++;
  ares_query_dnsrec(thrdd->rr.channel,
                    https_name ? https_name : async->peer->hostname,
                    ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                    async_thrdd_rr_done, async, शून्य);
  CURL_TRC_DNS(तथ्य, "[HTTPS] query records for %s",
               https_name ? https_name : async->peer->hostname);

out:
  curlx_free(https_name);
  वापस परिणाम;
}
#पूर्ण_अगर

व्योम Curl_async_thrdd_shutdown(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_resolv_async *async)
{
  Curl_async_thrdd_destroy(तथ्य, async);
}

काष्ठा async_thrdd_match_ctx {
  uint32_t mid;
  uint32_t resolv_id;
};

अटल bool async_thrdd_match_item(व्योम *qitem, व्योम *match_तथ्य)
{
  स्थिर काष्ठा async_thrdd_match_ctx *ctx = match_तथ्य;
  काष्ठा async_thrdd_item *item = qitem;
  वापस (item->mid == ctx->mid) && (item->resolv_id == ctx->resolv_id);
}

व्योम Curl_async_thrdd_destroy(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_resolv_async *async)
{
  (व्योम)तथ्य;
  अगर(async->queries_ongoing && !async->done &&
     तथ्य->multi && तथ्य->multi->resolv_thrdq) {
    /* Remove any resolve items still queued */
    काष्ठा async_thrdd_match_ctx mctx;
    mctx.mid = तथ्य->mid;
    mctx.resolv_id = async->id;
    Curl_thrdq_clear(तथ्य->multi->resolv_thrdq,
                     async_thrdd_match_item, &mctx);
  }
#अगर_घोषित USE_HTTPSRR_ARES
  अगर(async->thrdd.rr.channel) {
    ares_destroy(async->thrdd.rr.channel);
    async->thrdd.rr.channel = शून्य;
  }
#पूर्ण_अगर
  async_thrdd_item_destroy(async->thrdd.res_A);
  async->thrdd.res_A = शून्य;
  async_thrdd_item_destroy(async->thrdd.res_AAAA);
  async->thrdd.res_AAAA = शून्य;
}

/*
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 */
CURLcode Curl_async_await(काष्ठा Curl_easy *तथ्य, uint32_t resolv_id,
                          काष्ठा Curl_dns_entry **pdns)
{
  काष्ठा Curl_resolv_async *async = Curl_async_get(तथ्य, resolv_id);
  काष्ठा async_thrdd_ctx *thrdd = async ? &async->thrdd : शून्य;
  timediff_t milli, ms;
  CURLcode परिणाम = CURLE_AGAIN;

  अगर(!thrdd)
    वापस CURLE_FAILED_INIT;

  जबतक(परिणाम == CURLE_AGAIN) {
    जबतक(async->queries_ongoing && !async->done) {
      Curl_async_thrdd_multi_process(तथ्य->multi);
      अगर(async->done)
        अवरोध;

      ms = curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &async->start);
      अगर(ms < 3)
        milli = 0;
      अन्यथा अगर(ms <= 50)
        milli = ms / 3;
      अन्यथा अगर(ms <= 250)
        milli = 50;
      अन्यथा
        milli = 200;
      CURL_TRC_DNS(तथ्य, "await, waiting %" FMT_TIMEDIFF_T "ms", milli);
      curlx_wait_ms(milli);
    }
    परिणाम = Curl_async_take_परिणाम(तथ्य, async, pdns);
  }
  वापस परिणाम;
}

#अगर_घोषित HAVE_GETADDRINFO

/* Was the getaddrinfo() failure an authoritative negative answer,
   i.e. the resolver responded that the name (or its data) does not
   exist? Transient failures like EAI_AGAIN and local troubles must
   not count as negative answers. */
अटल bool gai_negative(पूर्णांक rc)
{
  चयन(rc) {
#अगर_घोषित EAI_NONAME
  हाल EAI_NONAME:
#पूर्ण_अगर
#अगर defined(EAI_NODATA) && \
  (!defined(EAI_NONAME) || (EAI_NODATA != EAI_NONAME))
  हाल EAI_NODATA:
#पूर्ण_अगर
    वापस TRUE;
  शेष:
    वापस FALSE;
  }
}

/* Process the item, using Curl_getaddrinfo_ex() */
अटल व्योम async_thrdd_item_process(व्योम *arg)
{
  काष्ठा async_thrdd_item *item = arg;
  काष्ठा addrinfo hints;
  अक्षर service[12];
  पूर्णांक pf = PF_INET;
  पूर्णांक rc;

#अगर_घोषित DEBUGBUILD
  अगर(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  अगर(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    वापस;
  }
#पूर्ण_अगर

  स_रखो(&hints, 0, माप(hints));
#अगर_घोषित CURLRES_IPV6
  अगर(item->dns_queries & CURL_DNSQ_AAAA) {
    pf = (item->dns_queries & CURL_DNSQ_A) ? PF_UNSPEC : PF_INET6;
  }
#पूर्ण_अगर
  hints.ai_family = pf;
  hints.ai_socktype = Curl_socktype_for_transport(item->transport);
  hints.ai_protocol = Curl_protocol_for_transport(item->transport);
#अगर_घोषित __APPLE__
  /* If we leave `ai_flags == 0` then macOS is looking for IPV4MAPPED
   * when doing AAAA queries. We do not want this "help". */
  hints.ai_flags = AI_ADDRCONFIG;
#पूर्ण_अगर

  curl_msnprintf(service, माप(service), "%u", item->port);
#अगर_घोषित AI_NUMERICSERV
  hints.ai_flags |= AI_NUMERICSERV;
#पूर्ण_अगर

  rc = Curl_getaddrinfo_ex(item->hostname, service, &hints, &item->res);
  अगर(rc) {
    item->sockerr = SOCKERRNO ? SOCKERRNO : rc;
    अगर(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    item->negative = gai_negative(rc);
  }
  अन्यथा {
    Curl_addrinfo_set_port(item->res, item->port);
  }
}

#अन्यथा /* HAVE_GETADDRINFO */

/* Process the item, using Curl_ipv4_resolve_r() */
अटल व्योम async_thrdd_item_process(व्योम *arg)
{
  काष्ठा async_thrdd_item *item = arg;

#अगर_घोषित DEBUGBUILD
  अगर(item->delay_ms) {
    curlx_wait_ms(item->delay_ms);
  }
  अगर(item->delay_fail_ms) {
    curlx_wait_ms(item->delay_fail_ms);
    item->negative = item->dbg_negative;
    वापस;
  }
#पूर्ण_अगर
  item->res = Curl_ipv4_resolve_r(item->hostname, item->port);
  अगर(!item->res) {
    item->sockerr = SOCKERRNO;
    अगर(item->sockerr == 0)
      item->sockerr = RESOLVER_ENOMEM;
    /* this resolver cannot tell a transient failure from an
       authoritative negative answer, treat it as before */
    item->negative = TRUE;
  }
}

#पूर्ण_अगर /* HAVE_GETADDRINFO */

#अगर_घोषित ENABLE_INTERNAL_WAKEUP
अटल व्योम async_thrdd_event(स्थिर काष्ठा curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              व्योम *user_तथ्य)
{
  काष्ठा Curl_multi *multi = user_तथ्य;
  (व्योम)tqueue;
  चयन(ev) {
  हाल CURL_THRDQ_EV_ITEM_DONE:
    Curl_multi_wakeup_internal(multi);
    अवरोध;
  शेष:
    अवरोध;
  }
}
#अन्यथा
#घोषणा async_thrdd_event   शून्य
#पूर्ण_अगर

अटल व्योम async_thrdd_item_free(व्योम *item)
{
  async_thrdd_item_destroy(item);
}

/* Create a thread queue for processing resolv items */
CURLcode Curl_async_thrdd_multi_init(काष्ठा Curl_multi *multi,
                                     uint32_t min_threads,
                                     uint32_t max_threads,
                                     uint32_t idle_time_ms)
{
  CURLcode परिणाम;
  DEBUGASSERT(!multi->resolv_thrdq);
  परिणाम = Curl_thrdq_create(&multi->resolv_thrdq, "DNS",
                             min_threads, max_threads, idle_time_ms,
                             async_thrdd_item_free,
                             async_thrdd_item_process,
                             async_thrdd_event,
                             multi);
#अगर_घोषित DEBUGBUILD
  अगर(!परिणाम) {
    स्थिर अक्षर *p = दो_पर्या("CURL_DBG_RESOLV_MAX_THREADS");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, UINT32_MAX)) {
        परिणाम = Curl_async_thrdd_multi_set_props(
          multi, min_threads, (uint32_t)l, idle_time_ms);
      }
    }
  }
#पूर्ण_अगर
  वापस परिणाम;
}

/* Tear down the thread queue, joining active threads or detaching them */
व्योम Curl_async_thrdd_multi_destroy(काष्ठा Curl_multi *multi, bool join)
{
  अगर(multi->resolv_thrdq) {
#अगर_घोषित CURLVERBOSE
    CURL_TRC_DNS(multi->admin, "destroy thread queue+pool, join=%d", join);
    CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
#पूर्ण_अगर
    Curl_thrdq_destroy(multi->resolv_thrdq, join);
    multi->resolv_thrdq = शून्य;
  }
}

#अगर_घोषित CURLVERBOSE
अटल व्योम async_thrdd_report_item(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा async_thrdd_item *item)
{
  अक्षर buf[MAX_IPADR_LEN];
  काष्ठा dynbuf tmp;
  स्थिर अक्षर *sep = "";
  स्थिर काष्ठा Curl_addrinfo *ai = item->res;
  CURLcode परिणाम;
  पूर्णांक ai_family;
#अगर_घोषित USE_IPV6
  ai_family = (item->dns_queries & CURL_DNSQ_AAAA) ? AF_INET6 : AF_INET;
#अन्यथा
  ai_family = AF_INET;
#पूर्ण_अगर

  अगर(!CURL_TRC_DNS_is_verbose(तथ्य))
    वापस;

  curlx_dyn_init(&tmp, 1024);
  क्रम(; ai; ai = ai->ai_अग्र) {
    अगर(ai->ai_family == ai_family) {
      Curl_printable_address(ai, buf, माप(buf));
      परिणाम = curlx_dyn_addf(&tmp, "%s%s", sep, buf);
      अगर(परिणाम) {
        CURL_TRC_DNS(तथ्य, "too many IP, cannot show");
        जाओ out;
      }
      sep = ", ";
    }
  }

  CURL_TRC_DNS(तथ्य, "Host %s:%u resolved IPv%c: %s",
               item->hostname, item->port,
               (item->dns_queries & CURL_DNSQ_AAAA) ? '6' : '4',
               (curlx_dyn_len(&tmp) ? curlx_dyn_ptr(&tmp) : "(none)"));
out:
  curlx_dyn_free(&tmp);
}
#पूर्ण_अगर /* CURLVERBOSE */

/* Process the receiving end of the thread queue, dispatching
 * processed items to their transfer when it can still be found
 * and has an `async` state present. Otherwise, destroy the item. */
व्योम Curl_async_thrdd_multi_process(काष्ठा Curl_multi *multi)
{
  काष्ठा Curl_easy *तथ्य;
  व्योम *qitem;

  जबतक(!Curl_thrdq_recv(multi->resolv_thrdq, &qitem)) {
    /* dispatch resolve result */
    काष्ठा async_thrdd_item *item = qitem;
    काष्ठा Curl_resolv_async *async = शून्य;

    तथ्य = Curl_multi_get_easy(multi, item->mid);
    अगर(तथ्य)
      async = Curl_async_get(तथ्य, item->resolv_id);
    अगर(async) {
      काष्ठा async_thrdd_item **pdest = &async->thrdd.res_A;

#अगर_घोषित CURLRES_IPV6
      अगर(item->dns_queries & CURL_DNSQ_AAAA)
        pdest = &async->thrdd.res_AAAA;
#पूर्ण_अगर
      अगर(!*pdest) {
        *pdest = item;
        item = शून्य;
      }
      अन्यथा
        DEBUGASSERT(0); /* should not receive duplicates here */

      --async->queries_ongoing;
      Curl_multi_mark_dirty(तथ्य);
    }
    async_thrdd_item_free(item);
  }
  CURL_THRDQ_TRACE(multi->resolv_thrdq, multi->admin);
}

CURLcode Curl_async_thrdd_multi_set_props(काष्ठा Curl_multi *multi,
                                          uint32_t min_threads,
                                          uint32_t max_threads,
                                          uint32_t idle_time_ms)
{
  वापस Curl_thrdq_set_props(multi->resolv_thrdq,
                              min_threads, max_threads, idle_time_ms);
}

अटल CURLcode async_thrdd_query(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा Curl_resolv_async *async,
                                  uint8_t dns_queries)
{
  काष्ठा async_thrdd_item *item;
  CURLcode परिणाम;

  item = async_thrdd_item_create(तथ्य, async->id, dns_queries,
                                 async->peer->hostname, async->peer->port,
                                 async->transport);
  अगर(!item) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  CURL_TRC_DNS(तथ्य, "queueing query %s", item->description);
  परिणाम = Curl_thrdq_send(तथ्य->multi->resolv_thrdq, item,
                           async_item_description(item), async->timeout_ms);
  अगर(परिणाम)
    जाओ out;
  item = शून्य;
  async->queries_ongoing++;

out:
  अगर(item)
    async_thrdd_item_free(item);
  वापस परिणाम;
}

CURLcode Curl_async_getaddrinfo(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_resolv_async *async)
{
  CURLcode परिणाम = CURLE_FAILED_INIT;
  व्योम *resolver = शून्य;

  अगर(async->queries_ongoing || async->done)
    वापस CURLE_FAILED_INIT;

#अगर_घोषित USE_HTTPSRR_ARES
  DEBUGASSERT(!async->thrdd.rr.channel);
  अगर((async->dns_queries & CURL_DNSQ_HTTPS) && !async->is_ipaddr) {
    परिणाम = async_rr_init(तथ्य, async);
    अगर(परिणाम)
      जाओ out;
    resolver = async->thrdd.rr.channel;
  }
#पूर्ण_अगर

  परिणाम = Curl_resolv_announce_start(तथ्य, resolver);
  अगर(परिणाम)
    वापस परिणाम;

#अगर_घोषित USE_HTTPSRR_ARES
  अगर(resolver) {
    परिणाम = async_rr_start(तथ्य, async);
    अगर(परिणाम)
      जाओ out;
  }
#पूर्ण_अगर

#अगर_घोषित CURLRES_IPV6
  /* Do not start an AAAA query for an IPv4 address when
   * we will start an A query for it. */
  अगर((async->dns_queries & CURL_DNSQ_AAAA) &&
     !(async->is_ipv4addr && (async->dns_queries & CURL_DNSQ_A))) {
    परिणाम = async_thrdd_query(तथ्य, async, CURL_DNSQ_AAAA);
    अगर(परिणाम)
      जाओ out;
  }
#पूर्ण_अगर
  अगर(async->dns_queries & CURL_DNSQ_A) {
    परिणाम = async_thrdd_query(तथ्य, async, CURL_DNSQ_A);
    अगर(परिणाम)
      जाओ out;
  }

  CURL_THRDQ_TRACE(तथ्य->multi->resolv_thrdq, तथ्य);

out:
  अगर(!async->queries_ongoing)
    async->done = TRUE;

  अगर(परिणाम)
    CURL_TRC_DNS(तथ्य, "error queueing query %s:%d -> %d",
                 async->peer->hostname, async->peer->port, (पूर्णांक)परिणाम);
  वापस परिणाम;
}

CURLcode Curl_async_pollset(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_resolv_async *async,
                            काष्ठा easy_pollset *ps)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_async_timeleft_ms(तथ्य, async);
#अगर_घोषित USE_HTTPSRR_ARES
  अगर(async->thrdd.rr.channel) {
    CURLcode परिणाम = Curl_ares_pollset(तथ्य, async->thrdd.rr.channel, ps);
    अगर(परिणाम)
      वापस परिणाम;
    timeout_ms = Curl_ares_timeout_ms(तथ्य, async, async->thrdd.rr.channel);
  }
#अन्यथा
  (व्योम)ps;
#पूर्ण_अगर

  अगर(!async->done) {
    स्थिर काष्ठा curltime *pnow = Curl_pgrs_now(तथ्य);
#अगर_अघोषित ENABLE_INTERNAL_WAKEUP
    timediff_t stutter_ms, elapsed_ms;
    elapsed_ms = curlx_ptimediff_ms(pnow, &async->start);
    अगर(elapsed_ms < 3)
      stutter_ms = 1;
    अन्यथा अगर(elapsed_ms <= 50)
      stutter_ms = elapsed_ms / 3;
    अन्यथा अगर(elapsed_ms <= 250)
      stutter_ms = 50;
    अन्यथा
      stutter_ms = 200;
    timeout_ms = CURLMIN(stutter_ms, timeout_ms);
#अन्यथा
    अगर(async->queries_ongoing &&
       !Curl_thrdq_check_started(तथ्य->multi->resolv_thrdq)) {
      /* The queue has items but starting a worker thread to process
         them just failed again; expire soon to check once more,
         instead of sleeping on the full resolve timeout. */
      CURL_TRC_DNS(तथ्य, "resolver thread start failed again, retrying");
      timeout_ms = CURLMIN(100, timeout_ms);
    }
#पूर्ण_अगर
    Curl_expire_set(तथ्य, EXPIRE_ASYNC_NAME, timeout_ms, pnow);
  }
  वापस CURLE_OK;
}

#अगर defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
अटल bool async_thrdd_item_missing_scope(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा async_thrdd_item *item)
{
  स्थिर काष्ठा Curl_addrinfo *ai;

  /* scope id already globally set */
  अगर(तथ्य->conn && तथ्य->conn->scope_id)
    वापस FALSE;

  क्रम(ai = item->res; ai; ai = ai->ai_अग्र) {
    अगर(ai->ai_family == AF_INET6) {
      काष्ठा sockaddr_in6 *sa6 = (व्योम *)ai->ai_addr;
      अगर(IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) && !sa6->sin6_scope_id)
        वापस TRUE;
    }
  }
  वापस FALSE;
}

अटल व्योम async_thrdd_item_strip_results(काष्ठा async_thrdd_item *item,
                                           पूर्णांक ai_family)
{
  काष्ठा Curl_addrinfo *ai = item->res, **panchor = &item->res;
  जबतक(ai) {
    अगर(ai->ai_family == ai_family) {
      *panchor = ai->ai_अग्र;
      ai->ai_अग्र = शून्य;
      Curl_freeaddrinfo(ai);
      ai = *panchor;
    }
    अन्यथा {
      panchor = &ai->ai_अग्र;
      ai = ai->ai_अग्र;
    }
  }
}

#पूर्ण_अगर /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */

अटल CURLcode async_thrdd_check_done(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा Curl_resolv_async *async)
{
  काष्ठा async_thrdd_ctx *thrdd = &async->thrdd;

  (व्योम)तथ्य;
  अगर(thrdd->res_A && !thrdd->processed_A) {
    VERBOSE(async_thrdd_report_item(तथ्य, thrdd->res_A));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_A->dns_queries;
    async->ai_A = thrdd->res_A->res;
    thrdd->res_A->res = शून्य;
    thrdd->processed_A = TRUE;
  }

  अगर(thrdd->res_AAAA && !thrdd->processed_AAAA) {
#अगर defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
    /* do we accept the incoming AAAA response? */
    अगर(!(thrdd->res_AAAA->dns_queries & CURL_DNSQ_A) &&
       async_thrdd_item_missing_scope(तथ्य, thrdd->res_AAAA)) {
      /* We queried "only" AF_INET6. This may be problematic when the
       * result has ipv6 link-local addresses and did not give
       * any scope id for it. glibc has a long outstanding bug
       * <https://sourceware.org/bugzilla/show_bug.cgi?id=14413>
       * that gives scope ids only on AF_UNSPEC queries. */
      काष्ठा async_thrdd_item *item = thrdd->res_AAAA;
      CURLcode परिणाम;

      /* Reuse the item and queue it again, this time with added
       * CURL_DNSQ_A which resolves using AF_UNSPEC. */
      thrdd->res_AAAA = शून्य;
      item->dns_queries |= CURL_DNSQ_A;
      अगर(item->res) {
        Curl_freeaddrinfo(item->res);
        item->res = शून्य;
      }

      CURL_TRC_DNS(तथ्य, "re-queueing query %s for AF_UNSPEC resolve",
                   item->description);
      परिणाम = Curl_thrdq_send(तथ्य->multi->resolv_thrdq, item,
                               async_item_description(item),
                               async->timeout_ms);
      अगर(परिणाम) {
        async_thrdd_item_free(item);
        वापस परिणाम;
      }
      async->queries_ongoing++;
      वापस CURLE_AGAIN;
    }
    /* accepting the AAAA result, strip it of any AF_INET entries,
     * as we might have resolved it with AF_UNSPEC. */
    async_thrdd_item_strip_results(thrdd->res_AAAA, AF_INET);
#पूर्ण_अगर /* USE_IPV6 && HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID */
    VERBOSE(async_thrdd_report_item(तथ्य, thrdd->res_AAAA));
    /* move addrinfos to the async result member */
    async->dns_responses |= thrdd->res_AAAA->dns_queries;
    async->ai_AAAA = thrdd->res_AAAA->res;
    thrdd->res_AAAA->res = शून्य;
    thrdd->processed_AAAA = TRUE;
  }

  अगर(async->queries_ongoing)
    वापस CURLE_AGAIN;
  async->done = TRUE;
  वापस CURLE_OK;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_async_take_परिणाम(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_resolv_async *async,
                                काष्ठा Curl_dns_entry **pdns)
{
  काष्ठा async_thrdd_ctx *thrdd = &async->thrdd;
  काष्ठा Curl_dns_entry *dns = शून्य;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = शून्य;

#अगर_घोषित USE_HTTPSRR_ARES
  /* best effort, ignore errors */
  अगर(thrdd->rr.channel)
    (व्योम)Curl_ares_perform(thrdd->rr.channel, 0);
#पूर्ण_अगर
#अगर_अघोषित ENABLE_INTERNAL_WAKEUP
  Curl_async_thrdd_multi_process(तथ्य->multi);
#पूर्ण_अगर

  परिणाम = async_thrdd_check_done(तथ्य, async);
  अगर(परिणाम)
    वापस परिणाम;

  Curl_expire_clear(तथ्य, EXPIRE_ASYNC_NAME);

  /* A failure is an authoritative negative answer, eligible for
     negative caching, only when every A/AAAA query performed came
     back answering that the name does not exist. A query that
     failed transiently or never returned is not an answer. */
  {
    स्थिर uint8_t ip_queries =
      async->dns_queries & (CURL_DNSQ_A | CURL_DNSQ_AAAA);
    bool negative = (async->dns_responses & ip_queries) == ip_queries;
    अगर(thrdd->res_A && (async->ai_A || !thrdd->res_A->negative))
      negative = FALSE;
    अगर(thrdd->res_AAAA && (async->ai_AAAA || !thrdd->res_AAAA->negative))
      negative = FALSE;
    अगर(!thrdd->res_A && !thrdd->res_AAAA)
      negative = FALSE;
    async->negative_answer = negative;
  }

  अगर(async->परिणाम) {
    परिणाम = async->परिणाम;
    जाओ out;
  }

  अगर(async->ai_A || async->ai_AAAA) {
    dns = Curl_dnsc_mk_addr2(
      तथ्य, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
    अगर(!dns) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }

#अगर_घोषित USE_HTTPSRR_ARES
  अगर(!dns && thrdd->rr.channel) {
    Curl_httpsrr_trace(तथ्य, async->httpsrr);
    dns = Curl_dnsc_mk_https(तथ्य, &async->httpsrr, async->peer);
    अगर(!dns) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }
#पूर्ण_अगर

  अगर(dns) {
    *pdns = dns;
    dns = शून्य;
  }
  CURL_THRDQ_TRACE(तथ्य->multi->resolv_thrdq, तथ्य);

out:
  Curl_dns_entry_unlink(तथ्य, &dns);
  Curl_async_thrdd_shutdown(तथ्य, async);
  अगर(!परिणाम && !*pdns)
    परिणाम = Curl_async_failed(तथ्य, async, शून्य);
  अगर(परिणाम &&
     (परिणाम != CURLE_COULDNT_RESOLVE_HOST) &&
     (परिणाम != CURLE_COULDNT_RESOLVE_PROXY)) {
    CURL_TRC_DNS(तथ्य, "Error %d resolving %s:%d",
                 (पूर्णांक)परिणाम, async->peer->hostname, async->peer->port);
  }
  वापस परिणाम;
}

#पूर्ण_अगर /* USE_RESOLV_THREADED */
