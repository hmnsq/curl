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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "curlx/timediff.h"
#অন্তর্ভাব "vdns/hostip.h"
#অন্তর্ভাব "vdns/httpsrr.h"
#অন্তর্ভাব <ares.h>

#যদি ARES_VERSION < 0x011000
#error "c-ares 1.16.0 or greater required"
#endif

#ifdef USE_HTTPSRR
#যদি ARES_VERSION < 0x011c00
#error "c-ares 1.28.0 or greater required for HTTPSRR"
#endif
#define HTTPSRR_WORKS
#endif

#define CARES_TIMEOUT_PER_ATTEMPT 2000

স্থির পূর্ণাঙ্ক ares_ver = 0;

স্থির CURLcode async_ares_set_dns_servers(সংরচনা Curl_easy *data,
                                           সংরচনা Curl_resolv_async *async);
স্থির CURLcode async_ares_set_dns_interface(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_resolv_async *async);
স্থির CURLcode async_ares_set_dns_local_ip4(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_resolv_async *async);
স্থির CURLcode async_ares_set_dns_local_ip6(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
পূর্ণাঙ্ক Curl_async_global_init(শূন্য)
{
#ifdef CARES_HAVE_ARES_LIBRARY_INIT
  যদি(ares_library_init(ARES_LIB_INIT_ALL)) {
    প্রত্যাগম CURLE_FAILED_INIT;
  }
#endif
  ares_version(&ares_ver);
  প্রত্যাগম CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
শূন্য Curl_async_global_cleanup(শূন্য)
{
#ifdef CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#endif
}

স্থির শূন্য sock_state_cb(শূন্য *data, ares_socket_t socket_fd,
                          পূর্ণাঙ্ক readable, পূর্ণাঙ্ক writable)
{
  সংরচনা Curl_easy *easy = data;
  যদি(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

স্থির CURLcode async_ares_init(সংরচনা Curl_easy *data,
                                সংরচনা Curl_resolv_async *async)
{
  সংরচনা async_ares_ctx *ares = &async->ares;
  পূর্ণাঙ্ক status;
  সংরচনা ares_options options;
  পূর্ণাঙ্ক optmask = ARES_OPT_SOCK_STATE_CB;
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
  যদি(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_PER_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  status = ares_init_options(&ares->channel, &options, optmask);
  যদি(status != ARES_SUCCESS) {
    ares->channel = রিক্ত;
    result = (status == ARES_ENOMEM) ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
    গচ্ছ out;
  }

  result = async_ares_set_dns_servers(data, async);
  যদি(result && result != CURLE_NOT_BUILT_IN)
    গচ্ছ out;

  result = async_ares_set_dns_interface(data, async);
  যদি(result && result != CURLE_NOT_BUILT_IN)
    গচ্ছ out;

  result = async_ares_set_dns_local_ip4(data, async);
  যদি(result && result != CURLE_NOT_BUILT_IN)
    গচ্ছ out;

  result = async_ares_set_dns_local_ip6(data, async);
  যদি(result && result != CURLE_NOT_BUILT_IN)
    গচ্ছ out;

  result = CURLE_OK;

out:
  যদি(result && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = রিক্ত;
  }
  প্রত্যাগম result;
}

শূন্য Curl_async_ares_shutdown(সংরচনা Curl_easy *data,
                              সংরচনা Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(data, async);
}

শূন্য Curl_async_ares_destroy(সংরচনা Curl_easy *data,
                             সংরচনা Curl_resolv_async *async)
{
  সংরচনা async_ares_ctx *ares = &async->ares;
  (শূন্য)data;
  যদি(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = রিক্ত;
  }
}

CURLcode Curl_async_pollset(সংরচনা Curl_easy *data,
                            সংরচনা Curl_resolv_async *async,
                            সংরচনা easy_pollset *ps)
{
  সংরচনা async_ares_ctx *ares = &async->ares;
  CURLcode result = CURLE_OK;

  যদি(ares->channel) {
    result = Curl_ares_pollset(data, ares->channel, ps);
    যদি(!result) {
      timediff_t ms = Curl_ares_timeout_ms(data, async, ares->channel);
      Curl_expire(data, ms, EXPIRE_ASYNC_NAME);
    }
  }
  প্রত্যাগম result;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_result(সংরচনা Curl_easy *data,
                                সংরচনা Curl_resolv_async *async,
                                সংরচনা Curl_dns_entry **pdns)
{
  সংরচনা async_ares_ctx *ares = &async->ares;
  সংরচনা Curl_dns_entry *dns = রিক্ত;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = রিক্ত;
  যদি(!ares)
    প্রত্যাগম CURLE_FAILED_INIT;

  যদি(Curl_ares_perform(ares->channel, 0) < 0) {
    result = CURLE_UNRECOVERABLE_POLL;
    গচ্ছ out;
  }

  যদি(async->queries_ongoing) {
    result = CURLE_AGAIN;
    গচ্ছ out;
  }

  /* all c-ares operations done, what is the result to report? */
  result = ares->result;
  যদি(ares->ares_status == ARES_SUCCESS && !result) {

    যদি(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(data, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      যদি(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
    }

#ifdef HTTPSRR_WORKS
    যদি(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      যদি(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
    }
#endif
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  যদি(!result && !dns) {
    ধ্রুব বর্ণ *বার্তা = রিক্ত;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_status == ARES_ENOTFOUND) ||
       (ares->ares_status == ARES_ENODATA));
    যদি(ares->ares_status != ARES_SUCCESS)
      বার্তা = ares_strerror(ares->ares_status);
    result = Curl_async_failed(data, async, বার্তা);
  }

  CURL_TRC_DNS(data, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (পূর্ণাঙ্ক)result, (পূর্ণাঙ্ক)ares->result, ares->ares_status,
               dns ? "" : "not ");

out:
  যদি(result != CURLE_AGAIN)
    ares->result = result;
  *pdns = result ? রিক্ত : dns;
  প্রত্যাগম result;
}

স্থির timediff_t async_ares_poll_timeout(সংরচনা async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  সংরচনা timeval *ares_calced, time_buf, max_timeout;
  পূর্ণাঙ্ক itimeout_ms;

#যদি TIMEDIFF_T_MAX > INT_MAX
  itimeout_ms = (timeout_ms > INT_MAX) ? INT_MAX :
                 ((timeout_ms < 0) ? -1 : (পূর্ণাঙ্ক)timeout_ms);
#অন্যথা
  itimeout_ms = (পূর্ণাঙ্ক)timeout_ms;
#endif
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  যদি(!ares_calced->tv_sec)
    প্রত্যাগম (timediff_t)(ares_calced->tv_usec / 1000);
  অন্যথা
    প্রত্যাগম 1000;
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
CURLcode Curl_async_await(সংরচনা Curl_easy *data, uint32_t resolv_id,
                          সংরচনা Curl_dns_entry **pdns)
{
  সংরচনা Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  সংরচনা async_ares_ctx *ares = async ? &async->ares : রিক্ত;
  সংরচনা curltime start = *Curl_pgrs_now(data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = রিক্ত; /* clear on entry */

  যদি(!ares)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  যাবত্(!result) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(data);
    যদি(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &start);
      যদি(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      অন্যথা
        timeout_ms = -1;
    }

    যদি(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      ভঙ্গ;
    }

    যদি(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      result = CURLE_UNRECOVERABLE_POLL;
      ভঙ্গ;
    }

    result = Curl_async_take_result(data, async, pdns);
    যদি(result == CURLE_AGAIN)
      result = CURLE_OK;
    অন্যথা যদি(result || *pdns)
      ভঙ্গ;

    যদি(Curl_pgrsUpdate(data)) {
      result = CURLE_ABORTED_BY_CALLBACK;
      ভঙ্গ;
    }
  }

  যদি(result)
    ares_cancel(ares->channel);
  প্রত্যাগম result;
}

#define CARES_MAX_ADDRS 256

স্থির bool async_ares_addr_seen(ধ্রুব সংরচনা Curl_addrinfo *cafirst,
                                 পূর্ণাঙ্ক family, size_t ss_size,
                                 ধ্রুব শূন্য *addr)
{
  ধ্রুব সংরচনা Curl_addrinfo *ca;
  প্রতি(ca = cafirst; ca; ca = ca->ai_next) {
    যদি((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_size) &&
       !memcmp(ca->ai_addr, addr, ss_size))
      প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
স্থির সংরচনা Curl_addrinfo *async_ares_node2addr(
  সংরচনা ares_addrinfo_গ্রন্থি *গ্রন্থি)
{
  /* traverse the ares_addrinfo_node list */
  সংরচনা ares_addrinfo_গ্রন্থি *ai;
  সংরচনা Curl_addrinfo *cafirst = রিক্ত;
  সংরচনা Curl_addrinfo *calast = রিক্ত;
  পূর্ণাঙ্ক error = 0;
  unsigned পূর্ণাঙ্ক naddrs = 0;

  প্রতি(ai = গ্রন্থি; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_next) {
    size_t ss_size;
    সংরচনা Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    যদি(ai->ai_family == AF_INET)
      ss_size = পরিমাণ(সংরচনা sockaddr_in);
#ifdef USE_IPV6
    অন্যথা যদি(ai->ai_family == AF_INET6)
      ss_size = পরিমাণ(সংরচনা sockaddr_in6);
#endif
    অন্যথা
      অনুবর্ত;

    /* ignore elements without required address info */
    যদি(!ai->ai_addr || !(ai->ai_addrlen > 0))
      অনুবর্ত;

    /* ignore elements with bogus address size */
    যদি((size_t)ai->ai_addrlen < ss_size)
      অনুবর্ত;

    /* ignore duplicate addresses already collected */
    যদি(async_ares_addr_seen(cafirst, ai->ai_family, ss_size, ai->ai_addr))
      অনুবর্ত;

    ca = curlx_malloc(পরিমাণ(সংরচনা Curl_addrinfo) + ss_size);
    যদি(!ca) {
      error = EAI_MEMORY;
      ভঙ্গ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = রিক্ত;
    ca->ai_canonname = রিক্ত;
    ca->ai_next      = রিক্ত;

    ca->ai_addr = (শূন্য *)((বর্ণ *)ca + পরিমাণ(সংরচনা Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    /* if the return list is empty, this becomes the first element */
    যদি(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    যদি(calast)
      calast->ai_next = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  যদি(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = রিক্ত;
  }

  প্রত্যাগম cafirst;
}

স্থির শূন্য async_ares_A_cb(শূন্য *user_data, পূর্ণাঙ্ক status, পূর্ণাঙ্ক timeouts,
                            সংরচনা ares_addrinfo *ares_ai)
{
  সংরচনা Curl_resolv_async *async = user_data;
  সংরচনা async_ares_ctx *ares = async ? &async->ares : রিক্ত;

  (শূন্য)timeouts;
  যদি(!async)
    প্রত্যাগম;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  যদি(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  অন্যথা {
    যদি((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    যদি(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}

#ifdef CURLRES_IPV6
স্থির শূন্য async_ares_AAAA_cb(শূন্য *user_data, পূর্ণাঙ্ক status, পূর্ণাঙ্ক timeouts,
                               সংরচনা ares_addrinfo *ares_ai)
{
  সংরচনা Curl_resolv_async *async = user_data;
  সংরচনা async_ares_ctx *ares = async ? &async->ares : রিক্ত;

  (শূন্য)timeouts;
  যদি(!async)
    প্রত্যাগম;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  যদি(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  অন্যথা {
    যদি((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    যদি(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}
#endif /* CURLRES_IPV6 */

#ifdef USE_HTTPSRR
স্থির শূন্য async_ares_rr_done(শূন্য *user_data, ares_status_t status,
                               size_t timeouts,
                               ধ্রুব ares_dns_record_t *dnsrec)
{
  সংরচনা Curl_resolv_async *async = user_data;
  সংরচনা async_ares_ctx *ares = async ? &async->ares : রিক্ত;

  যদি(!async)
    প্রত্যাগম;

  (শূন্য)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ares->ares_status = status;
  যদি((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
    ares->transient_err = TRUE;
  যদি((ARES_SUCCESS != status) || !dnsrec)
    প্রত্যাগম;
  ares->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#endif /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(সংরচনা Curl_easy *data,
                                সংরচনা Curl_resolv_async *async)
{
  সংরচনা async_ares_ctx *ares = &async->ares;
  বর্ণ service[12];
  পূর্ণাঙ্ক socktype;
  CURLcode result = CURLE_OK;

  যদি(ares->channel) {
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    গচ্ছ out;
  }

  result = async_ares_init(data, async);
  যদি(result)
    গচ্ছ out;

  result = Curl_resolv_announce_start(data, ares->channel);
  যদি(result)
    গচ্ছ out;

#যদি defined(CURLVERBOSE) && ARES_VERSION >= 0x011800 /* >= v1.24.0 */
  যদি(CURL_TRC_DNS_is_verbose(data)) {
    বর্ণ *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(data, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#endif

  curl_msnprintf(service, পরিমাণ(service), "%d", async->peer->port);
  socktype =
    (Curl_conn_get_transport(data, data->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifdef CURLRES_IPV6
  যদি(async->dns_queries & CURL_DNSQ_AAAA) {
    সংরচনা ares_addrinfo_hints hints;

    memset(&hints, 0, পরিমাণ(hints));
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

  যদি(async->dns_queries & CURL_DNSQ_A) {
    সংরচনা ares_addrinfo_hints hints;

    memset(&hints, 0, পরিমাণ(hints));
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
  যদি(async->dns_queries & CURL_DNSQ_HTTPS) {
    বর্ণ *https_name = রিক্ত;
    যদি(async->peer->port != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->port, async->peer->hostname);
      যদি(!https_name)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
    CURL_TRC_DNS(data, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_done, async, রিক্ত);
    curlx_free(https_name);
  }
#endif /* USE_HTTPSRR */

out:
  ares->result = result;
  প্রত্যাগম result ? result : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
স্থির CURLcode async_ares_set_dns_servers(সংরচনা Curl_easy *data,
                                           সংরচনা Curl_resolv_async *async)
{
  সংরচনা async_ares_ctx *ares = async ? &async->ares : রিক্ত;
  CURLcode result = CURLE_NOT_BUILT_IN;
  ধ্রুব বর্ণ *servers = CURL_EASY_STR(data, STRING_DNS_SERVERS);
  পূর্ণাঙ্ক ares_result = ARES_SUCCESS;

#ifdef DEBUGBUILD
  যদি(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#endif

  যদি(!servers)
    প্রত্যাগম CURLE_OK;

  /* if channel is not there, this is a parameter check */
  যদি(ares && ares->channel)
    ares_result = ares_set_servers_ports_csv(ares->channel, servers);
  বিকল্প(ares_result) {
  পক্ষ ARES_SUCCESS:
    result = CURLE_OK;
    ভঙ্গ;
  পক্ষ ARES_ENOMEM:
    result = CURLE_OUT_OF_MEMORY;
    ভঙ্গ;
  পক্ষ ARES_ENOTINITIALIZED:
  পক্ষ ARES_ENODATA:
  পক্ষ ARES_EBADSTR:
  স্বভাব:
    DEBUGF(infof(data, "bad servers set"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ভঙ্গ;
  }
  প্রত্যাগম result;
}

স্থির CURLcode async_ares_set_dns_interface(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_resolv_async *async)
{
  সংরচনা async_ares_ctx *ares = async ? &async->ares : রিক্ত;
  ধ্রুব বর্ণ *interf = CURL_EASY_STR(data, STRING_DNS_INTERFACE);

  যদি(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  যদি(ares && ares->channel)
    ares_set_local_dev(ares->channel, interf);

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode async_ares_set_dns_local_ip4(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_resolv_async *async)
{
  সংরচনা async_ares_ctx *ares = async ? &async->ares : রিক্ত;
  সংরচনা in_addr a4;
  ধ্রুব বর্ণ *local_ip4 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4);

  যদি(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  অন্যথা {
    যদি(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(data, "bad DNS IPv4 address"));
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  যদি(ares && ares->channel)
    ares_set_local_ip4(ares->channel, ntohl(a4.s_addr));

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode async_ares_set_dns_local_ip6(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_resolv_async *async)
{
#ifdef USE_IPV6
  সংরচনা async_ares_ctx *ares = async ? &async->ares : রিক্ত;
  unsigned বর্ণ a6[INET6_ADDRSTRLEN];
  ধ্রুব বর্ণ *local_ip6 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6);

  যদি(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, পরিমাণ(a6));
  }
  অন্যথা {
    যদি(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(data, "bad DNS IPv6 address"));
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there, this is a parameter check */
  যদি(ares && ares->channel)
    ares_set_local_ip6(ares->channel, a6);

  প্রত্যাগম CURLE_OK;
#অন্যথা /* no IPv6 support */
  (শূন্য)data;
  (শূন্য)async;
  প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
}

#endif /* USE_RESOLV_ARES */
