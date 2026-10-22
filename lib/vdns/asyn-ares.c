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

#ifdef USE_RESOLV_ARES

/***********************************************************************
 * Only for ares-enabled builds and only for functions that fulfill
 * the asynch resolver backend API as defined in asyn.h,
 * nothing else belongs in this file!
 **********************************************************************/

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

#include "urldata.h"
#include "cfilters.h"
#include "curl_addrinfo.h"
#include "curl_trc.h"
#include "url.h"
#include "multiif.h"
#include "curlx/inet_pton.h"
#include "connect.h"
#include "select.h"
#include "progress.h"
#include "curlx/timediff.h"
#include "vdns/hostip.h"
#include "vdns/httpsrr.h"
#include <ares.h>

#如果 ARES_VERSION < 0x011000
#error "c-ares 1.16.0 or greater required"
#endif

#ifdef USE_HTTPSRR
#如果 ARES_VERSION < 0x011c00
#error "c-ares 1.28.0 or greater required for HTTPSRR"
#endif
#define HTTPSRR_WORKS
#endif

#define CARES_TIMEOUT_PER_ATTEMPT 2000

static 整数 ares_ver = 0;

static CURLcode async_ares_set_dns_servers(struct Curl_easy *data,
                                           struct Curl_resolv_async *async);
static CURLcode async_ares_set_dns_interface(struct Curl_easy *data,
                                             struct Curl_resolv_async *async);
static CURLcode async_ares_set_dns_local_ip4(struct Curl_easy *data,
                                             struct Curl_resolv_async *async);
static CURLcode async_ares_set_dns_local_ip6(struct Curl_easy *data,
                                             struct Curl_resolv_async *async);

/*
 * Curl_async_global_init() - the generic low-level asynchronous name
 * resolve API. Called from curl_global_init() to initialize global resolver
 * environment. Initializes ares library.
 */
整数 Curl_async_global_init(void)
{
#ifdef CARES_HAVE_ARES_LIBRARY_INIT
  如果(ares_library_init(ARES_LIB_INIT_ALL)) {
    返回 CURLE_FAILED_INIT;
  }
#endif
  ares_version(&ares_ver);
  返回 CURLE_OK;
}

/*
 * Curl_async_global_cleanup()
 *
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Deinitializes ares library.
 */
void Curl_async_global_cleanup(void)
{
#ifdef CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#endif
}

static void sock_state_cb(void *data, ares_socket_t socket_fd,
                          整数 readable, 整数 writable)
{
  struct Curl_easy *easy = data;
  如果(!readable && !writable) {
    DEBUGASSERT(easy);
    Curl_multi_will_close(easy, socket_fd);
  }
}

static CURLcode async_ares_init(struct Curl_easy *data,
                                struct Curl_resolv_async *async)
{
  struct async_ares_ctx *ares = &async->ares;
  整数 status;
  struct ares_options options;
  整数 optmask = ARES_OPT_SOCK_STATE_CB;
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
  如果(ares_ver < 0x011400) {
    options.timeout = CARES_TIMEOUT_PER_ATTEMPT;
    optmask |= ARES_OPT_TIMEOUTMS;
  }

  status = ares_init_options(&ares->channel, &options, optmask);
  如果(status != ARES_SUCCESS) {
    ares->channel = NULL;
    result = (status == ARES_ENOMEM) ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
    goto out;
  }

  result = async_ares_set_dns_servers(data, async);
  如果(result && result != CURLE_NOT_BUILT_IN)
    goto out;

  result = async_ares_set_dns_interface(data, async);
  如果(result && result != CURLE_NOT_BUILT_IN)
    goto out;

  result = async_ares_set_dns_local_ip4(data, async);
  如果(result && result != CURLE_NOT_BUILT_IN)
    goto out;

  result = async_ares_set_dns_local_ip6(data, async);
  如果(result && result != CURLE_NOT_BUILT_IN)
    goto out;

  result = CURLE_OK;

out:
  如果(result && ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = NULL;
  }
  返回 result;
}

void Curl_async_ares_shutdown(struct Curl_easy *data,
                              struct Curl_resolv_async *async)
{
  /* c-ares has a method to "cancel" operations on a channel, but
   * as reported in #18216, this does not totally reset the channel
   * and ares may get stuck.
   * We need to destroy the channel and on demand create a new
   * one to avoid that. */
  Curl_async_ares_destroy(data, async);
}

void Curl_async_ares_destroy(struct Curl_easy *data,
                             struct Curl_resolv_async *async)
{
  struct async_ares_ctx *ares = &async->ares;
  (void)data;
  如果(ares->channel) {
    ares_destroy(ares->channel);
    ares->channel = NULL;
  }
}

CURLcode Curl_async_pollset(struct Curl_easy *data,
                            struct Curl_resolv_async *async,
                            struct easy_pollset *ps)
{
  struct async_ares_ctx *ares = &async->ares;
  CURLcode result = CURLE_OK;

  如果(ares->channel) {
    result = Curl_ares_pollset(data, ares->channel, ps);
    如果(!result) {
      timediff_t ms = Curl_ares_timeout_ms(data, async, ares->channel);
      Curl_expire(data, ms, EXPIRE_ASYNC_NAME);
    }
  }
  返回 result;
}

/*
 * Curl_async_take_result() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 *
 * Returns normal CURLcode errors.
 */
CURLcode Curl_async_take_result(struct Curl_easy *data,
                                struct Curl_resolv_async *async,
                                struct Curl_dns_entry **pdns)
{
  struct async_ares_ctx *ares = &async->ares;
  struct Curl_dns_entry *dns = NULL;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = NULL;
  如果(!ares)
    返回 CURLE_FAILED_INIT;

  如果(Curl_ares_perform(ares->channel, 0) < 0) {
    result = CURLE_UNRECOVERABLE_POLL;
    goto out;
  }

  如果(async->queries_ongoing) {
    result = CURLE_AGAIN;
    goto out;
  }

  /* all c-ares operations done, what is the result to report? */
  result = ares->result;
  如果(ares->ares_status == ARES_SUCCESS && !result) {

    如果(CURL_DNSQ_IS_ADDR(async->dns_queries)) {
      dns = Curl_dnsc_mk_addr2(data, async->dns_queries,
                               &async->ai_AAAA, &async->ai_A,
                               async->peer);
      如果(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }

#ifdef HTTPSRR_WORKS
    如果(!dns && (async->dns_queries & CURL_DNSQ_HTTPS)) {
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      如果(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }
#endif
  }
  /* if we have not found anything, report the proper
   * CURLE_COULDNT_RESOLVE_* code */
  如果(!result && !dns) {
    const char *msg = NULL;
    /* only an authoritative "does not exist" answer from every query
       may be cached as a negative entry, not transient failures like
       timeouts or server troubles */
    async->negative_answer = !ares->transient_err &&
      ((ares->ares_status == ARES_ENOTFOUND) ||
       (ares->ares_status == ARES_ENODATA));
    如果(ares->ares_status != ARES_SUCCESS)
      msg = ares_strerror(ares->ares_status);
    result = Curl_async_failed(data, async, msg);
  }

  CURL_TRC_DNS(data, "[%s] ares_take_result, result=%d, ares_result=%d, "
               "ares_status=%d, dns=%sfound",
               Curl_resolv_query_str(async->dns_queries),
               (整数)result, (整数)ares->result, ares->ares_status,
               dns ? "" : "not ");

out:
  如果(result != CURLE_AGAIN)
    ares->result = result;
  *pdns = result ? NULL : dns;
  返回 result;
}

static timediff_t async_ares_poll_timeout(struct async_ares_ctx *ares,
                                          timediff_t timeout_ms)
{
  struct timeval *ares_calced, time_buf, max_timeout;
  整数 itimeout_ms;

#如果 TIMEDIFF_T_MAX > INT_MAX
  itimeout_ms = (timeout_ms > INT_MAX) ? INT_MAX :
                 ((timeout_ms < 0) ? -1 : (整数)timeout_ms);
#否则
  itimeout_ms = (整数)timeout_ms;
#endif
  max_timeout.tv_sec = itimeout_ms / 1000;
  max_timeout.tv_usec = (itimeout_ms % 1000) * 1000;

  /* c-ares tells us the shortest timeout of any operation on channel */
  ares_calced = ares_timeout(ares->channel, &max_timeout, &time_buf);
  /* use the timeout period ares returned to us above if less than one
     second is left, otherwise use 1000ms to make sure the progress callback
     gets called frequent enough */
  如果(!ares_calced->tv_sec)
    返回 (timediff_t)(ares_calced->tv_usec / 1000);
  否则
    返回 1000;
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
CURLcode Curl_async_await(struct Curl_easy *data, uint32_t resolv_id,
                          struct Curl_dns_entry **pdns)
{
  struct Curl_resolv_async *async = Curl_async_get(data, resolv_id);
  struct async_ares_ctx *ares = async ? &async->ares : NULL;
  struct curltime start = *Curl_pgrs_now(data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(pdns);
  *pdns = NULL; /* clear on entry */

  如果(!ares)
    返回 CURLE_FAILED_INIT;

  /* Wait for the name resolve query to complete or time out. */
  当(!result) {
    timediff_t timeout_ms;

    timeout_ms = Curl_timeleft_ms(data);
    如果(!timeout_ms) { /* no applicable timeout from `data`*/
      timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &start);
      如果(elapsed_ms < CURL_TIMEOUT_RESOLVE_MS)
        timeout_ms = CURL_TIMEOUT_RESOLVE_MS - elapsed_ms;
      否则
        timeout_ms = -1;
    }

    如果(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      break;
    }

    如果(Curl_ares_perform(ares->channel,
                         async_ares_poll_timeout(ares, timeout_ms)) < 0) {
      result = CURLE_UNRECOVERABLE_POLL;
      break;
    }

    result = Curl_async_take_result(data, async, pdns);
    如果(result == CURLE_AGAIN)
      result = CURLE_OK;
    否则 如果(result || *pdns)
      break;

    如果(Curl_pgrsUpdate(data)) {
      result = CURLE_ABORTED_BY_CALLBACK;
      break;
    }
  }

  如果(result)
    ares_cancel(ares->channel);
  返回 result;
}

#define CARES_MAX_ADDRS 256

static bool async_ares_addr_seen(const struct Curl_addrinfo *cafirst,
                                 整数 family, size_t ss_size,
                                 const void *addr)
{
  const struct Curl_addrinfo *ca;
  为(ca = cafirst; ca; ca = ca->ai_next) {
    如果((ca->ai_family == family) &&
       ((size_t)ca->ai_addrlen == ss_size) &&
       !memcmp(ca->ai_addr, addr, ss_size))
      返回 TRUE;
  }
  返回 FALSE;
}

/*
 * async_ares_node2addr() converts an address list provided by c-ares
 * to an internal libcurl compatible list.
 */
static struct Curl_addrinfo *async_ares_node2addr(
  struct ares_addrinfo_node *node)
{
  /* traverse the ares_addrinfo_node list */
  struct ares_addrinfo_node *ai;
  struct Curl_addrinfo *cafirst = NULL;
  struct Curl_addrinfo *calast = NULL;
  整数 error = 0;
  unsigned 整数 naddrs = 0;

  为(ai = node; ai && (naddrs < CARES_MAX_ADDRS); ai = ai->ai_next) {
    size_t ss_size;
    struct Curl_addrinfo *ca;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    如果(ai->ai_family == AF_INET)
      ss_size = sizeof(struct sockaddr_in);
#ifdef USE_IPV6
    否则 如果(ai->ai_family == AF_INET6)
      ss_size = sizeof(struct sockaddr_in6);
#endif
    否则
      continue;

    /* ignore elements without required address info */
    如果(!ai->ai_addr || !(ai->ai_addrlen > 0))
      continue;

    /* ignore elements with bogus address size */
    如果((size_t)ai->ai_addrlen < ss_size)
      continue;

    /* ignore duplicate addresses already collected */
    如果(async_ares_addr_seen(cafirst, ai->ai_family, ss_size, ai->ai_addr))
      continue;

    ca = curlx_malloc(sizeof(struct Curl_addrinfo) + ss_size);
    如果(!ca) {
      error = EAI_MEMORY;
      break;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = NULL;
    ca->ai_canonname = NULL;
    ca->ai_next      = NULL;

    ca->ai_addr = (void *)((char *)ca + sizeof(struct Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    /* if the return list is empty, this becomes the first element */
    如果(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    如果(calast)
      calast->ai_next = ca;
    calast = ca;
    ++naddrs;
  }

  /* if we failed, destroy the Curl_addrinfo list */
  如果(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = NULL;
  }

  返回 cafirst;
}

static void async_ares_A_cb(void *user_data, 整数 status, 整数 timeouts,
                            struct ares_addrinfo *ares_ai)
{
  struct Curl_resolv_async *async = user_data;
  struct async_ares_ctx *ares = async ? &async->ares : NULL;

  (void)timeouts;
  如果(!async)
    返回;

  async->dns_responses |= CURL_DNSQ_A;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  如果(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_A = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  否则 {
    如果((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    如果(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}

#ifdef CURLRES_IPV6
static void async_ares_AAAA_cb(void *user_data, 整数 status, 整数 timeouts,
                               struct ares_addrinfo *ares_ai)
{
  struct Curl_resolv_async *async = user_data;
  struct async_ares_ctx *ares = async ? &async->ares : NULL;

  (void)timeouts;
  如果(!async)
    返回;

  async->dns_responses |= CURL_DNSQ_AAAA;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  如果(status == ARES_SUCCESS) {
    ares->ares_status = ARES_SUCCESS;
    async->ai_AAAA = async_ares_node2addr(ares_ai->nodes);
    ares_freeaddrinfo(ares_ai);
  }
  否则 {
    如果((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
      ares->transient_err = TRUE;
    如果(ares->ares_status != ARES_SUCCESS) /* do not overwrite success */
      ares->ares_status = status;
  }
}
#endif /* CURLRES_IPV6 */

#ifdef USE_HTTPSRR
static void async_ares_rr_done(void *user_data, ares_status_t status,
                               size_t timeouts,
                               const ares_dns_record_t *dnsrec)
{
  struct Curl_resolv_async *async = user_data;
  struct async_ares_ctx *ares = async ? &async->ares : NULL;

  如果(!async)
    返回;

  (void)timeouts;
  async->dns_responses |= CURL_DNSQ_HTTPS;
  async->queries_ongoing--;
  async->done = !async->queries_ongoing;
  ares->ares_status = status;
  如果((status != ARES_ENOTFOUND) && (status != ARES_ENODATA))
    ares->transient_err = TRUE;
  如果((ARES_SUCCESS != status) || !dnsrec)
    返回;
  ares->result = Curl_httpsrr_from_ares(dnsrec, &async->httpsrr);
}
#endif /* USE_HTTPSRR */

/*
 * Curl_async_getaddrinfo() - when using ares
 *
 * Starts a name resolve for the given hostname and port number.
 */
CURLcode Curl_async_getaddrinfo(struct Curl_easy *data,
                                struct Curl_resolv_async *async)
{
  struct async_ares_ctx *ares = &async->ares;
  char service[12];
  整数 socktype;
  CURLcode result = CURLE_OK;

  如果(ares->channel) {
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    goto out;
  }

  result = async_ares_init(data, async);
  如果(result)
    goto out;

  result = Curl_resolv_announce_start(data, ares->channel);
  如果(result)
    goto out;

#如果 defined(CURLVERBOSE) && ARES_VERSION >= 0x011800 /* >= v1.24.0 */
  如果(CURL_TRC_DNS_is_verbose(data)) {
    char *csv = ares_get_servers_csv(ares->channel);
    CURL_TRC_DNS(data, "ares: servers=%s", csv);
    ares_free_string(csv);
  }
#endif

  curl_msnprintf(service, sizeof(service), "%d", async->peer->port);
  socktype =
    (Curl_conn_get_transport(data, data->conn) == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifdef CURLRES_IPV6
  如果(async->dns_queries & CURL_DNSQ_AAAA) {
    struct ares_addrinfo_hints hints;

    memset(&hints, 0, sizeof(hints));
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

  如果(async->dns_queries & CURL_DNSQ_A) {
    struct ares_addrinfo_hints hints;

    memset(&hints, 0, sizeof(hints));
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
  如果(async->dns_queries & CURL_DNSQ_HTTPS) {
    char *https_name = NULL;
    如果(async->peer->port != 443) {
      https_name = curl_maprintf("_%u._https.%s",
                                 async->peer->port, async->peer->hostname);
      如果(!https_name)
        返回 CURLE_OUT_OF_MEMORY;
    }
    CURL_TRC_DNS(data, "[HTTPS] ares: query records for %s",
                 https_name ? https_name : async->peer->hostname);
    async->queries_ongoing++;
    ares_query_dnsrec(ares->channel,
                      https_name ? https_name : async->peer->hostname,
                      ARES_CLASS_IN, ARES_REC_TYPE_HTTPS,
                      async_ares_rr_done, async, NULL);
    curlx_free(https_name);
  }
#endif /* USE_HTTPSRR */

out:
  ares->result = result;
  返回 result ? result : (async->queries_ongoing ? CURLE_AGAIN : CURLE_OK);
}

/* Set what DNS server are is to use. This is called in 2 situations:
 * 1. when the application does 'CURLOPT_DNS_SERVERS' and passing NULL
 *    means any previous set value should be unset. Which means
 *    we need to destroy and create the are channel anew, if there is one.
 * 2. When we lazy init the ares channel and NULL means that there
 *    are no preferences and we do not reset any existing channel. */
static CURLcode async_ares_set_dns_servers(struct Curl_easy *data,
                                           struct Curl_resolv_async *async)
{
  struct async_ares_ctx *ares = async ? &async->ares : NULL;
  CURLcode result = CURLE_NOT_BUILT_IN;
  const char *servers = CURL_EASY_STR(data, STRING_DNS_SERVERS);
  整数 ares_result = ARES_SUCCESS;

#ifdef DEBUGBUILD
  如果(getenv("CURL_DNS_SERVER"))
    servers = getenv("CURL_DNS_SERVER");
#endif

  如果(!servers)
    返回 CURLE_OK;

  /* if channel is not there, this is a parameter check */
  如果(ares && ares->channel)
    ares_result = ares_set_servers_ports_csv(ares->channel, servers);
  switch(ares_result) {
  case ARES_SUCCESS:
    result = CURLE_OK;
    break;
  case ARES_ENOMEM:
    result = CURLE_OUT_OF_MEMORY;
    break;
  case ARES_ENOTINITIALIZED:
  case ARES_ENODATA:
  case ARES_EBADSTR:
  default:
    DEBUGF(infof(data, "bad servers set"));
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    break;
  }
  返回 result;
}

static CURLcode async_ares_set_dns_interface(struct Curl_easy *data,
                                             struct Curl_resolv_async *async)
{
  struct async_ares_ctx *ares = async ? &async->ares : NULL;
  const char *interf = CURL_EASY_STR(data, STRING_DNS_INTERFACE);

  如果(!interf)
    interf = "";

  /* if channel is not there, this is a parameter check */
  如果(ares && ares->channel)
    ares_set_local_dev(ares->channel, interf);

  返回 CURLE_OK;
}

static CURLcode async_ares_set_dns_local_ip4(struct Curl_easy *data,
                                             struct Curl_resolv_async *async)
{
  struct async_ares_ctx *ares = async ? &async->ares : NULL;
  struct in_addr a4;
  const char *local_ip4 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4);

  如果(!local_ip4 || (local_ip4[0] == 0)) {
    a4.s_addr = 0; /* disabled: do not bind to a specific address */
  }
  否则 {
    如果(curlx_inet_pton(AF_INET, local_ip4, &a4) != 1) {
      DEBUGF(infof(data, "bad DNS IPv4 address"));
      返回 CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there yet, this is a parameter check */
  如果(ares && ares->channel)
    ares_set_local_ip4(ares->channel, ntohl(a4.s_addr));

  返回 CURLE_OK;
}

static CURLcode async_ares_set_dns_local_ip6(struct Curl_easy *data,
                                             struct Curl_resolv_async *async)
{
#ifdef USE_IPV6
  struct async_ares_ctx *ares = async ? &async->ares : NULL;
  unsigned char a6[INET6_ADDRSTRLEN];
  const char *local_ip6 = CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6);

  如果(!local_ip6 || (local_ip6[0] == 0)) {
    /* disabled: do not bind to a specific address */
    memset(a6, 0, sizeof(a6));
  }
  否则 {
    如果(curlx_inet_pton(AF_INET6, local_ip6, a6) != 1) {
      DEBUGF(infof(data, "bad DNS IPv6 address"));
      返回 CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }

  /* if channel is not there, this is a parameter check */
  如果(ares && ares->channel)
    ares_set_local_ip6(ares->channel, a6);

  返回 CURLE_OK;
#否则 /* no IPv6 support */
  (void)data;
  (void)async;
  返回 CURLE_NOT_BUILT_IN;
#endif
}

#endif /* USE_RESOLV_ARES */
