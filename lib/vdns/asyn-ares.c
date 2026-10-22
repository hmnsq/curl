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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curlx/timediff.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ <ares.h>

#ಯದಿ ARES_VERSION < 0x011000
#error "c-ares 1.16.0 or greater required"
#endif

#ifdef USE_HTTPSRR
#ಯದಿ ARES_VERSION < 0x011c00
#error "c-ares 1.28.0 or greater required for HTTPSRR"
#endif
#define HTTPSRR_WORKS
#endif

#define CARES_TIMEOUT_PER_ATTEMPT 2000

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ares_ver = 0;

ಸ್ಥಿರ CURLcode async_ares_set_dns_servers(ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ Curl_resolv_async *async);
ಸ್ಥಿರ CURLcode async_ares_set_dns_interface(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ Curl_resolv_async *async);
ಸ್ಥಿರ CURLcode async_ares_set_dns_local_ip4(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ Curl_resolv_async *async);
ಸ್ಥಿರ CURLcode async_ares_set_dns_local_ip6(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
ಪೂರ್ಣಾಙ್ಕ Curl_async_global_init(ಶೂನ್ಯ)
{
#ifdef CARES_HAVE_ARES_LIBRARY_INIT
  ಯದಿ(ares_library_init(ARES_LIB_INIT_ALL)) {
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
#endif
  ares_version(&ares_ver);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
ಶೂನ್ಯ Curl_async_global_cleanup(ಶೂನ್ಯ)
{
#ifdef CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ sock_state_cb(ಶೂನ್ಯ *data, ares_socket_t socket_fd,
                          ಪೂರ್ಣಾಙ್ಕ readable, ಪೂರ್ಣಾಙ್ಕ writable)
{
  ಸಂರಚನಾ Curl_easy *easy = data;
  ಯದಿ(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

ಸ್ಥಿರ CURLcode async_ares_init(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಸಂರಚನಾ async_ares_ctx *ares = &async->ares;
  ಪೂರ್ಣಾಙ್ಕ status;
  ಸಂರಚನಾ ares_options options;
  ಪೂರ್ಣಾಙ್ಕ optmask = ARES_OPT_SOCK_STATE_CB;
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
  ಯದಿ(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_PER_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  status = ares_init_options(&ares->channel, &options, optmask);
  ಯದಿ(status != ARES_SUCCESS) {
    ares->channel = ರಿಕ್ತ;
    result = (status == ARES_ENOMEM) ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
    ಗಚ್ಛ out;
  }

  result = async_ares_set_dns_servers(data, async);
  ಯದಿ(result && result != CURLE_NOT_BUILT_IN)
    ಗಚ್ಛ out;

  result = async_ares_set_dns_interface(data, async);
  ಯದಿ(result && result != CURLE_NOT_BUILT_IN)
    ಗಚ್ಛ out;

  result = async_ares_set_dns_local_ip4(data, async);
  ಯದಿ(result && result != CURLE_NOT_BUILT_IN)
    ಗಚ್ಛ out;

  result = async_ares_set_dns_local_ip6(data, async);
  ಯದಿ(result && result != CURLE_NOT_BUILT_IN)
    ಗಚ್ಛ out;

  result = CURLE_OK;

out:
  ಯದಿ(result && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_async_ares_shutdown(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(data, async);
}

ಶೂನ್ಯ Curl_async_ares_destroy(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಸಂರಚನಾ async_ares_ctx *ares = &async->ares;
  (ಶೂನ್ಯ)data;
  ಯದಿ(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = ರಿಕ್ತ;
  }
}

CURLcode Curl_async_pollset(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_resolv_async *async,
                            ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ async_ares_ctx *ares = &async->ares;
  CURLcode result = CURLE_OK;

  ಯದಿ(ares->channel) {
    result = Curl_ares_pollset(data, ares->channel, ps);
    ಯದಿ(!result) {
      timediff_t ms = Curl_ares_timeout_ms(data, async, ares->channel);
      Curl_expire(data, ms, EXPIRE_ASYNC_NAME);
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_result(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_resolv_async *async,
                                ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಸಂರಚನಾ async_ares_ctx *ares = &async->ares;
  ಸಂರಚನಾ Curl_dns_entry *dns = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ರಿಕ್ತ;
  ಯದಿ(!ares)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(Curl_ares_perform(ares->channel, 0) < 0) {
    result = CURLE_UNRECOVERABLE_POLL;
    ಗಚ್ಛ out;
  }

  ಯದಿ(async->queries_ongoing) {
    result = CURLE_AGAIN;
    ಗಚ್ಛ out;
  }

  /* all c-ares operations done, what is the result to report? */
  result = ares->result;
  ಯದಿ(ares->ares_status == ARES_SUCCESS && !result) {

    ಯದಿ(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(data, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      ಯದಿ(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
    }

#ifdef HTTPSRR_WORKS
    ಯದಿ(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      ಯದಿ(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
    }
#endif
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  ಯದಿ(!result && !dns) {
    ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ = ರಿಕ್ತ;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_status == ARES_ENOTFOUND) ||
       (ares->ares_status == ARES_ENODATA));
    ಯದಿ(ares->ares_status != ARES_SUCCESS)
      ಸಂದೇಶ = ares_strerror(ares->ares_status);
    result = Curl_async_failed(data, async, ಸಂದೇಶ);
  }

  CURL_TRC_DNS(data, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (ಪೂರ್ಣಾಙ್ಕ)result, (ಪೂರ್ಣಾಙ್ಕ)ares->result, ares->ares_status,
               dns ? "" : "not ");

out:
  ಯದಿ(result != CURLE_AGAIN)
    ares->result = result;
  *pdns = result ? ರಿಕ್ತ : dns;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ timediff_t async_ares_poll_timeout(ಸಂರಚನಾ async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  ಸಂರಚನಾ timeval *ares_calced, time_buf, max_timeout;
  ಪೂರ್ಣಾಙ್ಕ itimeout_ms;

#ಯದಿ TIMEDIFF_T_MAX > INT_MAX
  itimeout_ms = (timeout_ms > INT_MAX) ? INT_MAX :
                 ((timeout_ms < 0) ? -1 : (ಪೂರ್ಣಾಙ್ಕ)timeout_ms);
#ಅನ್ಯಥಾ
  itimeout_ms = (ಪೂರ್ಣಾಙ್ಕ)timeout_ms;
#endif
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  ಯದಿ(!ares_calced->tv_sec)
    ಪ್ರತ್ಯಾಗಮ (timediff_t)(ares_calced->tv_usec / 1000);
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ 1000;
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
CURLcode Curl_async_await(ಸಂರಚನಾ Curl_easy *data, uint32_t resolv_id,
                          ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಸಂರಚನಾ Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  ಸಂರಚನಾ async_ares_ctx *ares = async ? &async->ares : ರಿಕ್ತ;
  ಸಂರಚನಾ curltime start = *Curl_pgrs_now(data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = ರಿಕ್ತ; /* clear on entry */

  ಯದಿ(!ares)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  ಯಾವತ್(!result) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(data);
    ಯದಿ(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &start);
      ಯದಿ(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      ಅನ್ಯಥಾ
        timeout_ms = -1;
    }

    ಯದಿ(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      ಭಙ್ಗ;
    }

    ಯದಿ(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      result = CURLE_UNRECOVERABLE_POLL;
      ಭಙ್ಗ;
    }

    result = Curl_async_take_result(data, async, pdns);
    ಯದಿ(result == CURLE_AGAIN)
      result = CURLE_OK;
    ಅನ್ಯಥಾ ಯದಿ(result || *pdns)
      ಭಙ್ಗ;

    ಯದಿ(Curl_pgrsUpdate(data)) {
      result = CURLE_ABORTED_BY_CALLBACK;
      ಭಙ್ಗ;
    }
  }

  ಯದಿ(result)
    ares_cancel(ares->channel);
  ಪ್ರತ್ಯಾಗಮ result;
}

#define CARES_MAX_ADDRS 256

ಸ್ಥಿರ bool async_ares_addr_seen(ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *cafirst,
                                 ಪೂರ್ಣಾಙ್ಕ family, size_t ss_size,
                                 ಧ್ರುವ ಶೂನ್ಯ *addr)
{
  ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ca;
  ಪ್ರತಿ(ca = cafirst; ca; ca = ca->ai_next) {
    ಯದಿ((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_size) &&
       !memcmp(ca->ai_addr, addr, ss_size))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
ಸ್ಥಿರ ಸಂರಚನಾ Curl_addrinfo *async_ares_node2addr(
  ಸಂರಚನಾ ares_addrinfo_ಗಂಟು *ಗಂಟು)
{
  /* traverse the ares_addrinfo_node list */
  ಸಂರಚನಾ ares_addrinfo_ಗಂಟು *ai;
  ಸಂರಚನಾ Curl_addrinfo *cafirst = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_addrinfo *calast = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ error = 0;
  unsigned ಪೂರ್ಣಾಙ್ಕ naddrs = 0;

  ಪ್ರತಿ(ai = ಗಂಟು; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_next) {
    size_t ss_size;
    ಸಂರಚನಾ Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ಯದಿ(ai->ai_family == AF_INET)
      ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in);
#ifdef USE_IPV6
    ಅನ್ಯಥಾ ಯದಿ(ai->ai_family == AF_INET6)
      ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6);
#endif
    ಅನ್ಯಥಾ
      ಅನುವರ್ತ;

    /* ignore elements without required address info */
    ಯದಿ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ಅನುವರ್ತ;

    /* ignore elements with bogus address size */
    ಯದಿ((size_t)ai->ai_addrlen < ss_size)
      ಅನುವರ್ತ;

    /* ignore duplicate addresses already collected */
    ಯದಿ(async_ares_addr_seen(cafirst, ai->ai_family, ss_size, ai->ai_addr))
      ಅನುವರ್ತ;

    ca = curlx_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo) + ss_size);
    ಯದಿ(!ca) {
      error = EAI_MEMORY;
      ಭಙ್ಗ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ರಿಕ್ತ;
    ca->ai_canonname = ರಿಕ್ತ;
    ca->ai_next      = ರಿಕ್ತ;

    ca->ai_addr = (ಶೂನ್ಯ *)((ವರ್ಣ *)ca + ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    /* if the return list is empty, this becomes the first element */
    ಯದಿ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ಯದಿ(calast)
      calast->ai_next = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  ಯದಿ(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ರಿಕ್ತ;
  }

  ಪ್ರತ್ಯಾಗಮ cafirst;
}

ಸ್ಥಿರ ಶೂನ್ಯ async_ares_A_cb(ಶೂನ್ಯ *user_data, ಪೂರ್ಣಾಙ್ಕ status, ಪೂರ್ಣಾಙ್ಕ timeouts,
                            ಸಂರಚನಾ ares_addrinfo *ares_ai)
{
  ಸಂರಚನಾ Curl_resolv_async *async = user_data;
  ಸಂರಚನಾ async_ares_ctx *ares = async ? &async->ares : ರಿಕ್ತ;

  (ಶೂನ್ಯ)timeouts;
  ಯದಿ(!async)
    ಪ್ರತ್ಯಾಗಮ;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ಯದಿ(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  ಅನ್ಯಥಾ {
    ಯದಿ((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    ಯದಿ(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}

#ifdef CURLRES_IPV6
ಸ್ಥಿರ ಶೂನ್ಯ async_ares_AAAA_cb(ಶೂನ್ಯ *user_data, ಪೂರ್ಣಾಙ್ಕ status, ಪೂರ್ಣಾಙ್ಕ timeouts,
                               ಸಂರಚನಾ ares_addrinfo *ares_ai)
{
  ಸಂರಚನಾ Curl_resolv_async *async = user_data;
  ಸಂರಚನಾ async_ares_ctx *ares = async ? &async->ares : ರಿಕ್ತ;

  (ಶೂನ್ಯ)timeouts;
  ಯದಿ(!async)
    ಪ್ರತ್ಯಾಗಮ;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ಯದಿ(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  ಅನ್ಯಥಾ {
    ಯದಿ((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    ಯದಿ(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}
#endif /* CURLRES_IPV6 */

#ifdef USE_HTTPSRR
ಸ್ಥಿರ ಶೂನ್ಯ async_ares_rr_done(ಶೂನ್ಯ *user_data, ares_status_t status,
                               size_t timeouts,
                               ಧ್ರುವ ares_dns_record_t *dnsrec)
{
  ಸಂರಚನಾ Curl_resolv_async *async = user_data;
  ಸಂರಚನಾ async_ares_ctx *ares = async ? &async->ares : ರಿಕ್ತ;

  ಯದಿ(!async)
    ಪ್ರತ್ಯಾಗಮ;

  (ಶೂನ್ಯ)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ares->ares_status = status;
  ಯದಿ((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
    ares->transient_err = TRUE;
  ಯದಿ((ARES_SUCCESS != status) || !dnsrec)
    ಪ್ರತ್ಯಾಗಮ;
  ares->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#endif /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಸಂರಚನಾ async_ares_ctx *ares = &async->ares;
  ವರ್ಣ service[12];
  ಪೂರ್ಣಾಙ್ಕ socktype;
  CURLcode result = CURLE_OK;

  ಯದಿ(ares->channel) {
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    ಗಚ್ಛ out;
  }

  result = async_ares_init(data, async);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_resolv_announce_start(data, ares->channel);
  ಯದಿ(result)
    ಗಚ್ಛ out;

#ಯದಿ defined(CURLVERBOSE) && ARES_VERSION >= 0x011800 /* >= v1.24.0 */
  ಯದಿ(CURL_TRC_DNS_is_verbose(data)) {
    ವರ್ಣ *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(data, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#endif

  curl_msnprintf(service, ಪರಿಮಾಣ(service), "%d", async->peer->port);
  socktype =
    (Curl_conn_get_transport(data, data->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifdef CURLRES_IPV6
  ಯದಿ(async->dns_queries & CURL_DNSQ_AAAA) {
    ಸಂರಚನಾ ares_addrinfo_hints hints;

    memset(&hints, 0, ಪರಿಮಾಣ(hints));
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

  ಯದಿ(async->dns_queries & CURL_DNSQ_A) {
    ಸಂರಚನಾ ares_addrinfo_hints hints;

    memset(&hints, 0, ಪರಿಮಾಣ(hints));
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
  ಯದಿ(async->dns_queries & CURL_DNSQ_HTTPS) {
    ವರ್ಣ *https_name = ರಿಕ್ತ;
    ಯದಿ(async->peer->port != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->port, async->peer->hostname);
      ಯದಿ(!https_name)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    CURL_TRC_DNS(data, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_done, async, ರಿಕ್ತ);
    curlx_free(https_name);
  }
#endif /* USE_HTTPSRR */

out:
  ares->result = result;
  ಪ್ರತ್ಯಾಗಮ result ? result : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
ಸ್ಥಿರ CURLcode async_ares_set_dns_servers(ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಸಂರಚನಾ async_ares_ctx *ares = async ? &async->ares : ರಿಕ್ತ;
  CURLcode result = CURLE_NOT_BUILT_IN;
  ಧ್ರುವ ವರ್ಣ *servers = CURL_EASY_STR(data, STRING_DNS_SERVERS);
  ಪೂರ್ಣಾಙ್ಕ ares_result = ARES_SUCCESS;

#ifdef DEBUGBUILD
  ಯದಿ(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#endif

  ಯದಿ(!servers)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* if channel is not there, this is a parameter check */
  ಯದಿ(ares && ares->channel)
    ares_result = ares_set_servers_ports_csv(ares->channel, servers);
  ವಿಕಲ್ಪ(ares_result) {
  ಪಕ್ಷ ARES_SUCCESS:
    result = CURLE_OK;
    ಭಙ್ಗ;
  ಪಕ್ಷ ARES_ENOMEM:
    result = CURLE_OUT_OF_MEMORY;
    ಭಙ್ಗ;
  ಪಕ್ಷ ARES_ENOTINITIALIZED:
  ಪಕ್ಷ ARES_ENODATA:
  ಪಕ್ಷ ARES_EBADSTR:
  ಸ್ವಭಾವ:
    DEBUGF(infof(data, "bad servers set"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode async_ares_set_dns_interface(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಸಂರಚನಾ async_ares_ctx *ares = async ? &async->ares : ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *interf = CURL_EASY_STR(data, STRING_DNS_INTERFACE);

  ಯದಿ(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  ಯದಿ(ares && ares->channel)
    ares_set_local_dev(ares->channel, interf);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode async_ares_set_dns_local_ip4(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಸಂರಚನಾ async_ares_ctx *ares = async ? &async->ares : ರಿಕ್ತ;
  ಸಂರಚನಾ in_addr a4;
  ಧ್ರುವ ವರ್ಣ *local_ip4 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4);

  ಯದಿ(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(data, "bad DNS IPv4 address"));
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  ಯದಿ(ares && ares->channel)
    ares_set_local_ip4(ares->channel, ntohl(a4.s_addr));

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode async_ares_set_dns_local_ip6(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ Curl_resolv_async *async)
{
#ifdef USE_IPV6
  ಸಂರಚನಾ async_ares_ctx *ares = async ? &async->ares : ರಿಕ್ತ;
  unsigned ವರ್ಣ a6[INET6_ADDRSTRLEN];
  ಧ್ರುವ ವರ್ಣ *local_ip6 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6);

  ಯದಿ(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, ಪರಿಮಾಣ(a6));
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(data, "bad DNS IPv6 address"));
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there, this is a parameter check */
  ಯದಿ(ares && ares->channel)
    ares_set_local_ip6(ares->channel, a6);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
#ಅನ್ಯಥಾ /* no IPv6 support */
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)async;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
}

#endif /* USE_RESOLV_ARES */
