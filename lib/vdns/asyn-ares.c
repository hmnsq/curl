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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "curlx/timediff.h"
#ଅନ୍ତର୍ଭାଵ "vdns/hostip.h"
#ଅନ୍ତର୍ଭାଵ "vdns/httpsrr.h"
#ଅନ୍ତର୍ଭାଵ <ares.h>

#ଯଦି ARES_VERSION < 0x011000
#error "c-ares 1.16.0 or greater required"
#endif

#ifdef USE_HTTPSRR
#ଯଦି ARES_VERSION < 0x011c00
#error "c-ares 1.28.0 or greater required for HTTPSRR"
#endif
#define HTTPSRR_WORKS
#endif

#define CARES_TIMEOUT_PER_ATTEMPT 2000

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ares_ver = 0;

ସ୍ଥିର CURLcode async_ares_set_dns_servers(ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା Curl_resolv_async *async);
ସ୍ଥିର CURLcode async_ares_set_dns_interface(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା Curl_resolv_async *async);
ସ୍ଥିର CURLcode async_ares_set_dns_local_ip4(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା Curl_resolv_async *async);
ସ୍ଥିର CURLcode async_ares_set_dns_local_ip6(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_async_global_init(ଶୂନ୍ୟ)
{
#ifdef CARES_HAVE_ARES_LIBRARY_INIT
  ଯଦି(ares_library_init(ARES_LIB_INIT_ALL)) {
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
#endif
  ares_version(&ares_ver);
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
ଶୂନ୍ୟ Curl_async_global_cleanup(ଶୂନ୍ୟ)
{
#ifdef CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ sock_state_cb(ଶୂନ୍ୟ *data, ares_socket_t socket_fd,
                          ପୂର୍ଣ୍ଣସଂଖ୍ୟା readable, ପୂର୍ଣ୍ଣସଂଖ୍ୟା writable)
{
  ସଂରଚନା Curl_easy *easy = data;
  ଯଦି(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

ସ୍ଥିର CURLcode async_ares_init(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_resolv_async *async)
{
  ସଂରଚନା async_ares_ctx *ares = &async->ares;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା status;
  ସଂରଚନା ares_options options;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା optmask = ARES_OPT_SOCK_STATE_CB;
  CURLcode result = CURLE_OK;

  /* initial status - failed */
  ares->ares_status = ARES_ENOTFOUND;
  async->queries_ongoing = 0;

  options.sock_state_cb = sock_state_cb;
  options.sock_state_cb_data = data;

  DEBUGASSERT(!ares->channel);
  /*
     if c ares < 1.20.0: curl set timeout to CARES_TIMEOUT_PER_ATTEMPT (2s)

     if c-ares >= 1.20.0 it already has the timeout to 2s, curl does not need
     to set the timeout value;

     if c-ares >= 1.24.0, user can set the timeout via /etc/resolv.conf to
     overwrite c-ares' timeout.
   */
  DEBUGASSERT(ares_ver);
  ଯଦି(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_PER_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  status = ares_init_options(&ares->channel, &options, optmask);
  ଯଦି(status != ARES_SUCCESS) {
    ares->channel = ଖାଲି;
    result = (status == ARES_ENOMEM) ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
    ଯାଅ out;
  }

  result = async_ares_set_dns_servers(data, async);
  ଯଦି(result && result != CURLE_NOT_BUILT_IN)
    ଯାଅ out;

  result = async_ares_set_dns_interface(data, async);
  ଯଦି(result && result != CURLE_NOT_BUILT_IN)
    ଯାଅ out;

  result = async_ares_set_dns_local_ip4(data, async);
  ଯଦି(result && result != CURLE_NOT_BUILT_IN)
    ଯାଅ out;

  result = async_ares_set_dns_local_ip6(data, async);
  ଯଦି(result && result != CURLE_NOT_BUILT_IN)
    ଯାଅ out;

  result = CURLE_OK;

out:
  ଯଦି(result && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ଖାଲି;
  }
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_async_ares_shutdown(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(data, async);
}

ଶୂନ୍ୟ Curl_async_ares_destroy(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_resolv_async *async)
{
  ସଂରଚନା async_ares_ctx *ares = &async->ares;
  (ଶୂନ୍ୟ)data;
  ଯଦି(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ଖାଲି;
  }
}

CURLcode Curl_async_pollset(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_resolv_async *async,
                            ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା async_ares_ctx *ares = &async->ares;
  CURLcode result = CURLE_OK;

  ଯଦି(ares->channel) {
    result = Curl_ares_pollset(data, ares->channel, ps);
    ଯଦି(!result) {
      timediff_t ms = Curl_ares_timeout_ms(data, async, ares->channel);
      Curl_expire(data, ms, EXPIRE_ASYNC_NAME);
    }
  }
  ଫେରନ୍ତୁ result;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_result(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_resolv_async *async,
                                ସଂରଚନା Curl_dns_entry **pdns)
{
  ସଂରଚନା async_ares_ctx *ares = &async->ares;
  ସଂରଚନା Curl_dns_entry *dns = ଖାଲି;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ଖାଲି;
  ଯଦି(!ares)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(Curl_ares_perform(ares->channel, 0) < 0) {
    result = CURLE_UNRECOVERABLE_POLL;
    ଯାଅ out;
  }

  ଯଦି(async->queries_ongoing) {
    result = CURLE_AGAIN;
    ଯାଅ out;
  }

  /* all c-ares operations done, what is the result to report? */
  result = ares->result;
  ଯଦି(ares->ares_status == ARES_SUCCESS && !result) {

    ଯଦି(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(data, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      ଯଦି(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
    }

#ifdef HTTPSRR_WORKS
    ଯଦି(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      ଯଦି(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
    }
#endif
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  ଯଦି(!result && !dns) {
    ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ = ଖାଲି;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_status == ARES_ENOTFOUND) ||
       (ares->ares_status == ARES_ENODATA));
    ଯଦି(ares->ares_status != ARES_SUCCESS)
      ସନ୍ଦେଶ = ares_strerror(ares->ares_status);
    result = Curl_async_failed(data, async, ସନ୍ଦେଶ);
  }

  CURL_TRC_DNS(data, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ares->result, ares->ares_status,
               dns ? "" : "not ");

out:
  ଯଦି(result != CURLE_AGAIN)
    ares->result = result;
  *pdns = result ? ଖାଲି : dns;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର timediff_t async_ares_poll_timeout(ସଂରଚନା async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  ସଂରଚନା timeval *ares_calced, time_buf, max_timeout;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା itimeout_ms;

#ଯଦି TIMEDIFF_T_MAX > INT_MAX
  itimeout_ms = (timeout_ms > INT_MAX) ? INT_MAX :
                 ((timeout_ms < 0) ? -1 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)timeout_ms);
#ଅନ୍ୟଥା
  itimeout_ms = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)timeout_ms;
#endif
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  ଯଦି(!ares_calced->tv_sec)
    ଫେରନ୍ତୁ (timediff_t)(ares_calced->tv_usec / 1000);
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ 1000;
}

/*
 * Curl_async_await()
 *
 * Waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 *
 * 'pdns' MUST be non-NULL.
 *
 * Returns CURLE_COULDNT_RESOLVE_HOST if the host was not resolved,
 * CURLE_OPERATION_TIMEDOUT if a time-out occurred, or other errors.
 */
CURLcode Curl_async_await(ସଂରଚନା Curl_easy *data, uint32_t resolv_id,
                          ସଂରଚନା Curl_dns_entry **pdns)
{
  ସଂରଚନା Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ସଂରଚନା async_ares_ctx *ares = async ? &async->ares : ଖାଲି;
  ସଂରଚନା curltime start = *Curl_pgrs_now(data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ଖାଲି; /* clear on entry */

  ଯଦି(!ares)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  ଯେତେବେଳେ(!result) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(data);
    ଯଦି(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &start);
      ଯଦି(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      ଅନ୍ୟଥା
        timeout_ms = -1;
    }

    ଯଦି(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      ଭାଙ୍ଗ;
    }

    ଯଦି(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      result = CURLE_UNRECOVERABLE_POLL;
      ଭାଙ୍ଗ;
    }

    result = Curl_async_take_result(data, async, pdns);
    ଯଦି(result == CURLE_AGAIN)
      result = CURLE_OK;
    ଅନ୍ୟଥା ଯଦି(result || *pdns)
      ଭାଙ୍ଗ;

    ଯଦି(Curl_pgrsUpdate(data)) {
      result = CURLE_ABORTED_BY_CALLBACK;
      ଭାଙ୍ଗ;
    }
  }

  ଯଦି(result)
    ares_cancel(ares->channel);
  ଫେରନ୍ତୁ result;
}

#define CARES_MAX_ADDRS 256

ସ୍ଥିର bool async_ares_addr_seen(ଅଚଳ ସଂରଚନା Curl_addrinfo *cafirst,
                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, size_t ss_size,
                                 ଅଚଳ ଶୂନ୍ୟ *addr)
{
  ଅଚଳ ସଂରଚନା Curl_addrinfo *ca;
  ପାଇଁ(ca = cafirst; ca; ca = ca->ai_next) {
    ଯଦି((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_size) &&
       !memcmp(ca->ai_addr, addr, ss_size))
      ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
ସ୍ଥିର ସଂରଚନା Curl_addrinfo *async_ares_node2addr(
  ସଂରଚନା ares_addrinfo_ଗଣ୍ଠି *ଗଣ୍ଠି)
{
  /* traverse the ares_addrinfo_node list */
  ସଂରଚନା ares_addrinfo_ଗଣ୍ଠି *ai;
  ସଂରଚନା Curl_addrinfo *cafirst = ଖାଲି;
  ସଂରଚନା Curl_addrinfo *calast = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = 0;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା naddrs = 0;

  ପାଇଁ(ai = ଗଣ୍ଠି; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_next) {
    size_t ss_size;
    ସଂରଚନା Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ଯଦି(ai->ai_family == AF_INET)
      ss_size = ଆକାର(ସଂରଚନା sockaddr_in);
#ifdef USE_IPV6
    ଅନ୍ୟଥା ଯଦି(ai->ai_family == AF_INET6)
      ss_size = ଆକାର(ସଂରଚନା sockaddr_in6);
#endif
    ଅନ୍ୟଥା
      ଜାରି;

    /* ignore elements without required address info */
    ଯଦି(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ଜାରି;

    /* ignore elements with bogus address size */
    ଯଦି((size_t)ai->ai_addrlen < ss_size)
      ଜାରି;

    /* ignore duplicate addresses already collected */
    ଯଦି(async_ares_addr_seen(cafirst, ai->ai_family, ss_size, ai->ai_addr))
      ଜାରି;

    ca = curlx_malloc(ଆକାର(ସଂରଚନା Curl_addrinfo) + ss_size);
    ଯଦି(!ca) {
      error = EAI_MEMORY;
      ଭାଙ୍ଗ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ଖାଲି;
    ca->ai_canonname = ଖାଲି;
    ca->ai_next      = ଖାଲି;

    ca->ai_addr = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ca + ଆକାର(ସଂରଚନା Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    /* if the return list is empty, this becomes the first element */
    ଯଦି(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ଯଦି(calast)
      calast->ai_next = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  ଯଦି(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ଖାଲି;
  }

  ଫେରନ୍ତୁ cafirst;
}

ସ୍ଥିର ଶୂନ୍ୟ async_ares_A_cb(ଶୂନ୍ୟ *user_data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା status, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeouts,
                            ସଂରଚନା ares_addrinfo *ares_ai)
{
  ସଂରଚନା Curl_resolv_async *async = user_data;
  ସଂରଚନା async_ares_ctx *ares = async ? &async->ares : ଖାଲି;

  (ଶୂନ୍ୟ)timeouts;
  ଯଦି(!async)
    ଫେରନ୍ତୁ;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ଯଦି(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  ଅନ୍ୟଥା {
    ଯଦି((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    ଯଦି(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}

#ifdef CURLRES_IPV6
ସ୍ଥିର ଶୂନ୍ୟ async_ares_AAAA_cb(ଶୂନ୍ୟ *user_data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା status, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeouts,
                               ସଂରଚନା ares_addrinfo *ares_ai)
{
  ସଂରଚନା Curl_resolv_async *async = user_data;
  ସଂରଚନା async_ares_ctx *ares = async ? &async->ares : ଖାଲି;

  (ଶୂନ୍ୟ)timeouts;
  ଯଦି(!async)
    ଫେରନ୍ତୁ;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ଯଦି(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  ଅନ୍ୟଥା {
    ଯଦି((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    ଯଦି(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}
#endif /* CURLRES_IPV6 */

#ifdef USE_HTTPSRR
ସ୍ଥିର ଶୂନ୍ୟ async_ares_rr_done(ଶୂନ୍ୟ *user_data, ares_status_t status,
                               size_t timeouts,
                               ଅଚଳ ares_dns_record_t *dnsrec)
{
  ସଂରଚନା Curl_resolv_async *async = user_data;
  ସଂରଚନା async_ares_ctx *ares = async ? &async->ares : ଖାଲି;

  ଯଦି(!async)
    ଫେରନ୍ତୁ;

  (ଶୂନ୍ୟ)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ares->ares_status = status;
  ଯଦି((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
    ares->transient_err = TRUE;
  ଯଦି((ARES_SUCCESS != status) || !dnsrec)
    ଫେରନ୍ତୁ;
  ares->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#endif /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_resolv_async *async)
{
  ସଂରଚନା async_ares_ctx *ares = &async->ares;
  ଅକ୍ଷର service[12];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା socktype;
  CURLcode result = CURLE_OK;

  ଯଦି(ares->channel) {
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    ଯାଅ out;
  }

  result = async_ares_init(data, async);
  ଯଦି(result)
    ଯାଅ out;

  result = Curl_resolv_announce_start(data, ares->channel);
  ଯଦି(result)
    ଯାଅ out;

#ଯଦି defined(CURLVERBOSE) && ARES_VERSION >= 0x011800 /* >= v1.24.0 */
  ଯଦି(CURL_TRC_DNS_is_verbose(data)) {
    ଅକ୍ଷର *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(data, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#endif

  curl_msnprintf(service, ଆକାର(service), "%d", async->peer->port);
  socktype =
    (Curl_conn_get_transport(data, data->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifdef CURLRES_IPV6
  ଯଦି(async->dns_queries & CURL_DNSQ_AAAA) {
    ସଂରଚନା ares_addrinfo_hints hints;

    memset(&hints, 0, ଆକାର(hints));
    CURL_TRC_DNS(data, "[AAAA] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET6;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_AAAA_cb, async);
  }
#endif /* CURLRES_IPV6 */

  ଯଦି(async->dns_queries & CURL_DNSQ_A) {
    ସଂରଚନା ares_addrinfo_hints hints;

    memset(&hints, 0, ଆକାର(hints));
    CURL_TRC_DNS(data, "[A] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_A_cb, async);
  }

#ifdef USE_HTTPSRR
  ଯଦି(async->dns_queries & CURL_DNSQ_HTTPS) {
    ଅକ୍ଷର *https_name = ଖାଲି;
    ଯଦି(async->peer->port != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->port, async->peer->hostname);
      ଯଦି(!https_name)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    CURL_TRC_DNS(data, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_done, async, ଖାଲି);
    curlx_free(https_name);
  }
#endif /* USE_HTTPSRR */

out:
  ares->result = result;
  ଫେରନ୍ତୁ result ? result : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
ସ୍ଥିର CURLcode async_ares_set_dns_servers(ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା Curl_resolv_async *async)
{
  ସଂରଚନା async_ares_ctx *ares = async ? &async->ares : ଖାଲି;
  CURLcode result = CURLE_NOT_BUILT_IN;
  ଅଚଳ ଅକ୍ଷର *servers = CURL_EASY_STR(data, STRING_DNS_SERVERS);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ares_result = ARES_SUCCESS;

#ifdef DEBUGBUILD
  ଯଦି(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#endif

  ଯଦି(!servers)
    ଫେରନ୍ତୁ CURLE_OK;

  /* if channel is not there, this is a parameter check */
  ଯଦି(ares && ares->channel)
    ares_result = ares_set_servers_ports_csv(ares->channel, servers);
  ବାଛ(ares_result) {
  ସ୍ଥିତି ARES_SUCCESS:
    result = CURLE_OK;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି ARES_ENOMEM:
    result = CURLE_OUT_OF_MEMORY;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି ARES_ENOTINITIALIZED:
  ସ୍ଥିତି ARES_ENODATA:
  ସ୍ଥିତି ARES_EBADSTR:
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUGF(infof(data, "bad servers set"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode async_ares_set_dns_interface(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା Curl_resolv_async *async)
{
  ସଂରଚନା async_ares_ctx *ares = async ? &async->ares : ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *interf = CURL_EASY_STR(data, STRING_DNS_INTERFACE);

  ଯଦି(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  ଯଦି(ares && ares->channel)
    ares_set_local_dev(ares->channel, interf);

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode async_ares_set_dns_local_ip4(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା Curl_resolv_async *async)
{
  ସଂରଚନା async_ares_ctx *ares = async ? &async->ares : ଖାଲି;
  ସଂରଚନା in_addr a4;
  ଅଚଳ ଅକ୍ଷର *local_ip4 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4);

  ଯଦି(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  ଅନ୍ୟଥା {
    ଯଦି(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(data, "bad DNS IPv4 address"));
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  ଯଦି(ares && ares->channel)
    ares_set_local_ip4(ares->channel, ntohl(a4.s_addr));

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode async_ares_set_dns_local_ip6(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା Curl_resolv_async *async)
{
#ifdef USE_IPV6
  ସଂରଚନା async_ares_ctx *ares = async ? &async->ares : ଖାଲି;
  unsigned ଅକ୍ଷର a6[INET6_ADDRSTRLEN];
  ଅଚଳ ଅକ୍ଷର *local_ip6 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6);

  ଯଦି(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, ଆକାର(a6));
  }
  ଅନ୍ୟଥା {
    ଯଦି(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(data, "bad DNS IPv6 address"));
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there, this is a parameter check */
  ଯଦି(ares && ares->channel)
    ares_set_local_ip6(ares->channel, a6);

  ଫେରନ୍ତୁ CURLE_OK;
#ଅନ୍ୟଥା /* no IPv6 support */
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)async;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
}

#endif /* USE_RESOLV_ARES */
