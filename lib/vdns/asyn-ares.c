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

#यदि_परिभाषितम् USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

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

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curlx/timediff.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव <ares.h>

#यदि ARES_संस्करणम् < 0x011000
#दोषः "c-ares 1.16.0 or greater required"
#यदि_अन्तः

#यदि_परिभाषितम् USE_HTTPSRR
#यदि ARES_संस्करणम् < 0x011c00
#दोषः "c-ares 1.28.0 or greater required for HTTPSRR"
#यदि_अन्तः
#परिभाषय HTTPSRR_कार्यं_करोति
#यदि_अन्तः

#परिभाषय CARES_TIMEOUT_प्रत्येकम्_ATTEMPT 2000

स्थिर पूर्णाङ्क ares_ver = 0;

स्थिर CURLcode async_ares_समुच्चयः_dns_servers(संरचना Curl_easy *दत्त,
                                           संरचना Curl_resolv_async *async);
स्थिर CURLcode async_ares_समुच्चयः_dns_interface(संरचना Curl_easy *दत्त,
                                             संरचना Curl_resolv_async *async);
स्थिर CURLcode async_ares_समुच्चयः_dns_local_ip4(संरचना Curl_easy *दत्त,
                                             संरचना Curl_resolv_async *async);
स्थिर CURLcode async_ares_समुच्चयः_dns_local_ip6(संरचना Curl_easy *दत्त,
                                             संरचना Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
पूर्णाङ्क Curl_async_global_init(शून्य)
{
#यदि_परिभाषितम् CARES_HAVE_ARES_LIBRARY_INIT
  यदि(ares_library_init(ARES_LIB_INIT_ALL)) {
    प्रत्यागम CURLE_FAILED_INIT;
  }
#यदि_अन्तः
  ares_संस्करणम्(&ares_ver);
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
शून्य Curl_async_global_cleanup(शून्य)
{
#यदि_परिभाषितम् CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#यदि_अन्तः
}

स्थिर शून्य sock_state_cb(शून्य *दत्त, ares_socket_t socket_fd,
                          पूर्णाङ्क readable, पूर्णाङ्क writable)
{
  संरचना Curl_easy *easy = दत्त;
  यदि(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

स्थिर CURLcode async_ares_init(संरचना Curl_easy *दत्त,
                                संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = &async->ares;
  पूर्णाङ्क स्थितिः;
  संरचना ares_options options;
  पूर्णाङ्क optmask = ARES_OPT_SOCK_STATE_CB;
  CURLcode परिणाम = CURLE_OK;

  /* initial status - failed */
  ares->ares_स्थितिः = ARES_ENOTFOUND;
  async->queries_ongoing = 0;

  options.sock_state_cb = sock_state_cb;
  options.sock_state_cb_दत्त = दत्त;

  DEBUGASSERT(!ares->channel);
  /*
     if c ares < 1.20.0: curl set timeout to CARES_TIMEOUT_PER_ATTEMPT (2s)

     if c-ares >= 1.20.0 it already has the timeout to 2s, curl does not need
     to set the timeout value;

     if c-ares >= 1.24.0, user can set the timeout via /etc/resolv.conf to
     overwrite c-ares' timeout.
   */
  DEBUGASSERT(ares_ver);
  यदि(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_प्रत्येकम्_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  स्थितिः = ares_init_options(&ares->channel, &options, optmask);
  यदि(स्थितिः != ARES_SUCCESS) {
    ares->channel = रिक्त;
    परिणाम = (स्थितिः == ARES_ENOMEM) ? CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः : CURLE_FAILED_INIT;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = async_ares_समुच्चयः_dns_servers(दत्त, async);
  यदि(परिणाम && परिणाम != CURLE_NOT_BUILT_अन्तर्)
    गच्छ बहिर्गमनम्;

  परिणाम = async_ares_समुच्चयः_dns_interface(दत्त, async);
  यदि(परिणाम && परिणाम != CURLE_NOT_BUILT_अन्तर्)
    गच्छ बहिर्गमनम्;

  परिणाम = async_ares_समुच्चयः_dns_local_ip4(दत्त, async);
  यदि(परिणाम && परिणाम != CURLE_NOT_BUILT_अन्तर्)
    गच्छ बहिर्गमनम्;

  परिणाम = async_ares_समुच्चयः_dns_local_ip6(दत्त, async);
  यदि(परिणाम && परिणाम != CURLE_NOT_BUILT_अन्तर्)
    गच्छ बहिर्गमनम्;

  परिणाम = CURLE_OK;

बहिर्गमनम्:
  यदि(परिणाम && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = रिक्त;
  }
  प्रत्यागम परिणाम;
}

शून्य Curl_async_ares_shutdown(संरचना Curl_easy *दत्त,
                              संरचना Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(दत्त, async);
}

शून्य Curl_async_ares_destroy(संरचना Curl_easy *दत्त,
                             संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = &async->ares;
  (शून्य)दत्त;
  यदि(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = रिक्त;
  }
}

CURLcode Curl_async_pollset(संरचना Curl_easy *दत्त,
                            संरचना Curl_resolv_async *async,
                            संरचना easy_pollset *ps)
{
  संरचना async_ares_ctx *ares = &async->ares;
  CURLcode परिणाम = CURLE_OK;

  यदि(ares->channel) {
    परिणाम = Curl_ares_pollset(दत्त, ares->channel, ps);
    यदि(!परिणाम) {
      timediff_t ms = Curl_ares_timeout_ms(दत्त, async, ares->channel);
      Curl_expire(दत्त, ms, EXPIRE_ASYNC_NAME);
    }
  }
  प्रत्यागम परिणाम;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_परिणाम(संरचना Curl_easy *दत्त,
                                संरचना Curl_resolv_async *async,
                                संरचना Curl_dns_प्रविष्टिः **pdns)
{
  संरचना async_ares_ctx *ares = &async->ares;
  संरचना Curl_dns_प्रविष्टिः *dns = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = रिक्त;
  यदि(!ares)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(Curl_ares_perform(ares->channel, 0) < 0) {
    परिणाम = CURLE_UNRECOVERABLE_POLL;
    गच्छ बहिर्गमनम्;
  }

  यदि(async->queries_ongoing) {
    परिणाम = CURLE_AGAIN;
    गच्छ बहिर्गमनम्;
  }

  /* all c-ares operations done, what is the result to report? */
  परिणाम = ares->परिणाम;
  यदि(ares->ares_स्थितिः == ARES_SUCCESS && !परिणाम) {

    यदि(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(दत्त, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      यदि(!dns) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
    }

#यदि_परिभाषितम् HTTPSRR_कार्यं_करोति
    यदि(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(दत्त, &async->httpsrr, async->peer);
      यदि(!dns) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
    }
#यदि_अन्तः
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  यदि(!परिणाम && !dns) {
    ध्रुव वर्ण *सन्देशः = रिक्त;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_स्थितिः == ARES_ENOTFOUND) ||
       (ares->ares_स्थितिः == ARES_ENODATA));
    यदि(ares->ares_स्थितिः != ARES_SUCCESS)
      सन्देशः = ares_strerror(ares->ares_स्थितिः);
    परिणाम = Curl_async_failed(दत्त, async, सन्देशः);
  }

  CURL_TRC_DNS(दत्त, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (पूर्णाङ्क)परिणाम, (पूर्णाङ्क)ares->परिणाम, ares->ares_स्थितिः,
               dns ? "" : "not ");

बहिर्गमनम्:
  यदि(परिणाम != CURLE_AGAIN)
    ares->परिणाम = परिणाम;
  *pdns = परिणाम ? रिक्त : dns;
  प्रत्यागम परिणाम;
}

स्थिर timediff_t async_ares_poll_timeout(संरचना async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  संरचना timeval *ares_calced, time_buf, max_timeout;
  पूर्णाङ्क itimeout_ms;

#यदि TIMEDIFF_T_MAX > अधिकतम_पूर्णाङ्कः
  itimeout_ms = (timeout_ms > अधिकतम_पूर्णाङ्कः) ? अधिकतम_पूर्णाङ्कः :
                 ((timeout_ms < 0) ? -1 : (पूर्णाङ्क)timeout_ms);
#अन्यथा
  itimeout_ms = (पूर्णाङ्क)timeout_ms;
#यदि_अन्तः
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  यदि(!ares_calced->tv_sec)
    प्रत्यागम (timediff_t)(ares_calced->tv_usec / 1000);
  अन्यथा
    प्रत्यागम 1000;
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
CURLcode Curl_async_await(संरचना Curl_easy *दत्त, uint32_t resolv_id,
                          संरचना Curl_dns_प्रविष्टिः **pdns)
{
  संरचना Curl_resolv_async *async = Curl_async_get(दत्त, resolv_id);
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;
  संरचना curltime start = *Curl_pgrs_now(दत्त);
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = रिक्त; /* clear on entry */

  यदि(!ares)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  यावत्(!परिणाम) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(दत्त);
    यदि(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &start);
      यदि(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      अन्यथा
        timeout_ms = -1;
    }

    यदि(timeout_ms < 0) {
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      भङ्ग;
    }

    यदि(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      परिणाम = CURLE_UNRECOVERABLE_POLL;
      भङ्ग;
    }

    परिणाम = Curl_async_take_परिणाम(दत्त, async, pdns);
    यदि(परिणाम == CURLE_AGAIN)
      परिणाम = CURLE_OK;
    अन्यथा यदि(परिणाम || *pdns)
      भङ्ग;

    यदि(Curl_pgrsUpdate(दत्त)) {
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
      भङ्ग;
    }
  }

  यदि(परिणाम)
    ares_cancel(ares->channel);
  प्रत्यागम परिणाम;
}

#परिभाषय CARES_MAX_ADDRS 256

स्थिर bool async_ares_addr_आलोकितम्(ध्रुव संरचना Curl_addrinfo *cafirst,
                                 पूर्णाङ्क family, size_t ss_आकार,
                                 ध्रुव शून्य *addr)
{
  ध्रुव संरचना Curl_addrinfo *ca;
  प्रति(ca = cafirst; ca; ca = ca->ai_अग्र) {
    यदि((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_आकार) &&
       !memcmp(ca->ai_addr, addr, ss_आकार))
      प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
स्थिर संरचना Curl_addrinfo *async_ares_node2addr(
  संरचना ares_addrinfo_ग्रन्थिः *ग्रन्थिः)
{
  /* traverse the ares_addrinfo_node list */
  संरचना ares_addrinfo_ग्रन्थिः *ai;
  संरचना Curl_addrinfo *cafirst = रिक्त;
  संरचना Curl_addrinfo *calast = रिक्त;
  पूर्णाङ्क error = 0;
  अचिह्नित पूर्णाङ्क naddrs = 0;

  प्रति(ai = ग्रन्थिः; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_अग्र) {
    size_t ss_आकार;
    संरचना Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    यदि(ai->ai_family == AF_INET)
      ss_आकार = परिमाण(संरचना sockaddr_in);
#यदि_परिभाषितम् USE_IPV6
    अन्यथा यदि(ai->ai_family == AF_INET6)
      ss_आकार = परिमाण(संरचना sockaddr_in6);
#यदि_अन्तः
    अन्यथा
      अनुवर्त;

    /* ignore elements without required address info */
    यदि(!ai->ai_addr || !(ai->ai_addrlen > 0))
      अनुवर्त;

    /* ignore elements with bogus address size */
    यदि((size_t)ai->ai_addrlen < ss_आकार)
      अनुवर्त;

    /* ignore duplicate addresses already collected */
    यदि(async_ares_addr_आलोकितम्(cafirst, ai->ai_family, ss_आकार, ai->ai_addr))
      अनुवर्त;

    ca = curlx_malloc(परिमाण(संरचना Curl_addrinfo) + ss_आकार);
    यदि(!ca) {
      error = EAI_MEMORY;
      भङ्ग;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_आकार;
    ca->ai_addr      = रिक्त;
    ca->ai_canonname = रिक्त;
    ca->ai_अग्र      = रिक्त;

    ca->ai_addr = (शून्य *)((वर्ण *)ca + परिमाण(संरचना Curl_addrinfo));
    स्मृतिं_प्रतिलिख(ca->ai_addr, ai->ai_addr, ss_आकार);

    /* if the return list is empty, this becomes the first element */
    यदि(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    यदि(calast)
      calast->ai_अग्र = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  यदि(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = रिक्त;
  }

  प्रत्यागम cafirst;
}

स्थिर शून्य async_ares_A_cb(शून्य *user_दत्त, पूर्णाङ्क स्थितिः, पूर्णाङ्क timeouts,
                            संरचना ares_addrinfo *ares_ai)
{
  संरचना Curl_resolv_async *async = user_दत्त;
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;

  (शून्य)timeouts;
  यदि(!async)
    प्रत्यागम;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->समाप्तम् = !async->queries_ongoing;
  यदि(स्थितिः == ARES_SUCCESS) {
    ares->ares_स्थितिः = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  अन्यथा {
    यदि((स्थितिः != ARES_ENOTFOUND) && (स्थितिः != ARES_ENODATA))
      ares->transient_err = TRUE;
    यदि(ares->ares_स्थितिः != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_स्थितिः = स्थितिः;
  }
}

#यदि_परिभाषितम् CURLRES_IPV6
स्थिर शून्य async_ares_AAAA_cb(शून्य *user_दत्त, पूर्णाङ्क स्थितिः, पूर्णाङ्क timeouts,
                               संरचना ares_addrinfo *ares_ai)
{
  संरचना Curl_resolv_async *async = user_दत्त;
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;

  (शून्य)timeouts;
  यदि(!async)
    प्रत्यागम;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->समाप्तम् = !async->queries_ongoing;
  यदि(स्थितिः == ARES_SUCCESS) {
    ares->ares_स्थितिः = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  अन्यथा {
    यदि((स्थितिः != ARES_ENOTFOUND) && (स्थितिः != ARES_ENODATA))
      ares->transient_err = TRUE;
    यदि(ares->ares_स्थितिः != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_स्थितिः = स्थितिः;
  }
}
#यदि_अन्तः /* CURLRES_IPV6 */

#यदि_परिभाषितम् USE_HTTPSRR
स्थिर शून्य async_ares_rr_समाप्तम्(शून्य *user_दत्त, ares_स्थितिः_t स्थितिः,
                               size_t timeouts,
                               ध्रुव ares_dns_लेखः_t *dnsrec)
{
  संरचना Curl_resolv_async *async = user_दत्त;
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;

  यदि(!async)
    प्रत्यागम;

  (शून्य)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->समाप्तम् = !async->queries_ongoing;
  ares->ares_स्थितिः = स्थितिः;
  यदि((स्थितिः != ARES_ENOTFOUND) && (स्थितिः != ARES_ENODATA))
    ares->transient_err = TRUE;
  यदि((ARES_SUCCESS != स्थितिः) || !dnsrec)
    प्रत्यागम;
  ares->परिणाम = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#यदि_अन्तः /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(संरचना Curl_easy *दत्त,
                                संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = &async->ares;
  वर्ण service[12];
  पूर्णाङ्क socktype;
  CURLcode परिणाम = CURLE_OK;

  यदि(ares->channel) {
    DEBUGASSERT(0);
    परिणाम = CURLE_FAILED_INIT;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = async_ares_init(दत्त, async);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_resolv_announce_start(दत्त, ares->channel);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

#यदि defined(CURLVERBOSE) && ARES_संस्करणम् >= 0x011800 /* >= v1.24.0 */
  यदि(CURL_TRC_DNS_is_verbose(दत्त)) {
    वर्ण *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(दत्त, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#यदि_अन्तः

  curl_msnprintf(service, परिमाण(service), "%d", async->peer->पत्तनम्);
  socktype =
    (Curl_conn_get_transport(दत्त, दत्त->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#यदि_परिभाषितम् CURLRES_IPV6
  यदि(async->dns_queries & CURL_DNSQ_AAAA) {
    संरचना ares_addrinfo_hints hints;

    स्मृतिं_पूरय(&hints, 0, परिमाण(hints));
    CURL_TRC_DNS(दत्त, "[AAAA] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET6;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_AAAA_cb, async);
  }
#यदि_अन्तः /* CURLRES_IPV6 */

  यदि(async->dns_queries & CURL_DNSQ_A) {
    संरचना ares_addrinfo_hints hints;

    स्मृतिं_पूरय(&hints, 0, परिमाण(hints));
    CURL_TRC_DNS(दत्त, "[A] ares: query records for %s",
                 async->peer->hostname);
    hints.ai_family = PF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = ARES_AI_NUMERICSERV;
    async->queries_ongoing++;
    ares_getaddrinfo(ares->channel, async->peer->hostname,
                     service, &hints, async_ares_A_cb, async);
  }

#यदि_परिभाषितम् USE_HTTPSRR
  यदि(async->dns_queries & CURL_DNSQ_HTTPS) {
    वर्ण *https_name = रिक्त;
    यदि(async->peer->पत्तनम् != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->पत्तनम्, async->peer->hostname);
      यदि(!https_name)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    CURL_TRC_DNS(दत्त, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_अन्तर्, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_समाप्तम्, async, रिक्त);
    curlx_free(https_name);
  }
#यदि_अन्तः /* USE_HTTPSRR */

बहिर्गमनम्:
  ares->परिणाम = परिणाम;
  प्रत्यागम परिणाम ? परिणाम : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
स्थिर CURLcode async_ares_समुच्चयः_dns_servers(संरचना Curl_easy *दत्त,
                                           संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;
  CURLcode परिणाम = CURLE_NOT_BUILT_अन्तर्;
  ध्रुव वर्ण *servers = CURL_EASY_STR(दत्त, STRING_DNS_SERVERS);
  पूर्णाङ्क ares_परिणाम = ARES_SUCCESS;

#यदि_परिभाषितम् DEBUGBUILD
  यदि(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#यदि_अन्तः

  यदि(!servers)
    प्रत्यागम CURLE_OK;

  /* if channel is not there, this is a parameter check */
  यदि(ares && ares->channel)
    ares_परिणाम = ares_समुच्चयः_servers_ports_csv(ares->channel, servers);
  विकल्प(ares_परिणाम) {
  पक्ष ARES_SUCCESS:
    परिणाम = CURLE_OK;
    भङ्ग;
  पक्ष ARES_ENOMEM:
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    भङ्ग;
  पक्ष ARES_ENOTINITIALIZED:
  पक्ष ARES_ENODATA:
  पक्ष ARES_EBADSTR:
  स्वभाव:
    DEBUGF(infof(दत्त, "bad servers set"));
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode async_ares_समुच्चयः_dns_interface(संरचना Curl_easy *दत्त,
                                             संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;
  ध्रुव वर्ण *interf = CURL_EASY_STR(दत्त, STRING_DNS_INTERFACE);

  यदि(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  यदि(ares && ares->channel)
    ares_समुच्चयः_local_dev(ares->channel, interf);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode async_ares_समुच्चयः_dns_local_ip4(संरचना Curl_easy *दत्त,
                                             संरचना Curl_resolv_async *async)
{
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;
  संरचना in_addr a4;
  ध्रुव वर्ण *local_ip4 = CURL_EASY_STR(दत्त, STRING_DNS_LOCAL_IP4);

  यदि(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  अन्यथा {
    यदि(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(दत्त, "bad DNS IPv4 address"));
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  यदि(ares && ares->channel)
    ares_समुच्चयः_local_ip4(ares->channel, ntohl(a4.s_addr));

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode async_ares_समुच्चयः_dns_local_ip6(संरचना Curl_easy *दत्त,
                                             संरचना Curl_resolv_async *async)
{
#यदि_परिभाषितम् USE_IPV6
  संरचना async_ares_ctx *ares = async ? &async->ares : रिक्त;
  अचिह्नित वर्ण a6[INET6_ADDRSTRLEN];
  ध्रुव वर्ण *local_ip6 = CURL_EASY_STR(दत्त, STRING_DNS_LOCAL_IP6);

  यदि(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    स्मृतिं_पूरय(a6, 0, परिमाण(a6));
  }
  अन्यथा {
    यदि(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(दत्त, "bad DNS IPv6 address"));
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }
  }

  /* if channel is not there, this is a parameter check */
  यदि(ares && ares->channel)
    ares_समुच्चयः_local_ip6(ares->channel, a6);

  प्रत्यागम CURLE_OK;
#अन्यथा /* no IPv6 support */
  (शून्य)दत्त;
  (शून्य)async;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
}

#यदि_अन्तः /* USE_RESOLV_ARES */
