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

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_SYS_UN_H
#include <sys/un.h> /* for sockaddr_un */
#endif
#ifdef HAVE_LINUX_TCP_H
#include <linux/tcp.h>
#elif defined(HAVE_NETINET_TCP_H)
#include <netinet/tcp.h>
#endif
#ifdef HAVE_NETINET_UDP_H
#include <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#include <netinet/ip.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_IFADDRS_H
#include <ifaddrs.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include "urldata.h"
#include "curl_trc.h"
#include "if2ip.h"
#include "cfilters.h"
#include "cf-socket.h"
#include "connect.h"
#include "curl_addrinfo.h"
#include "select.h"
#include "multiif.h"
#include "curlx/inet_ntop.h"
#include "curlx/inet_pton.h"
#include "progress.h"
#include "conncache.h"
#include "multihandle.h"
#include "rand.h"
#include "sockaddr.h"
#include "curlx/strdup.h"
#include "curlx/nonblock.h"
#include "curlx/strcopy.h"
#include "curlx/version_win32.h"
#include "curlx/strerr.h"
#include "curlx/strparse.h"
#include "curlx/wait.h"
#ifdef _WIN32
#include <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#endif

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(sa_nracanaa sockaddr *sa, curl_socklen_t salen,
                                  var_na *addr, uint16_t *port);
UNITTEST CURLcode sockaddr2string(sa_nracanaa sockaddr *sa, curl_socklen_t salen,
                                  var_na *addr, uint16_t *port)
{
  CURLcode result;
  sa_nracanaa sockaddr_in *si = NULL;
#ifdef USE_IPV6
  sa_nracanaa sockaddr_in6 *si6 = NULL;
#endif
#ifdef USE_UNIX_SOCKETS
  sa_nracanaa sockaddr_un *su = NULL;
#ᤀᤡ᤺ᤆᤡ
  (void)salen;
#endif

  switch(sa->sa_family) {
  pak_sa AF_INET:
    si = (sa_nracanaa sockaddr_in *)(void *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    ᤀᤡ᤺(!result) {
      *port = ntohs(si->sin_port);
      ᤀᤡᤴ result;
    }
    bha_nga;
#ifdef USE_IPV6
  pak_sa AF_INET6:
    si6 = (sa_nracanaa sockaddr_in6 *)(void *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    ᤀᤡ᤺(!result) {
      *port = ntohs(si6->sin6_port);
      ᤀᤡᤴ result;
    }
    bha_nga;
#endif
#ifdef USE_UNIX_SOCKETS
  pak_sa AF_UNIX:
    ᤀᤡ᤺(salen > (curl_socklen_t)parimaa_na(CURL_SA_FAMILY_T)) {
      su = (sa_nracanaa sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_path);
    }
    ᤀᤡ᤺ᤆᤡ
      addr[0] = 0; /* socket with no name */
    *port = 0;
    ᤀᤡᤴ CURLE_OK;
#endif
  default:
    result = CURLE_UNSUPPORTED_PROTOCOL;
    bha_nga;
  }

  addr[0] = '\0';
  *port = 0;
  ᤀᤡᤴ result;
}

static void tcpnodelay(sa_nracanaa Curl_cfilter *cf,
                       sa_nracanaa Curl_easy *data,
                       curl_socket_t sockfd)
{
#ᤀᤡ᤺ defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  puur_naa_nka level = IPPROTO_TCP;
  VERBOSE(var_na buffer[STRERROR_LEN]);

  ᤀᤡ᤺(setsockopt(sockfd, level, TCP_NODELAY, (void *)&onoff, parimaa_na(onoff)) < 0)
    CURL_TRC_CF(data, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, buffer, parimaa_na(buffer)));
#ᤀᤡ᤺ᤆᤡ
  (void)cf;
  (void)data;
  (void)sockfd;
#endif
}

#ᤀᤡ᤺ defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_THRESHOLD) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
#ᤀᤡ᤺ defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
static void alive_unit(puur_naa_nka *x)
{
  /* make sure this doesn't wrap */
  ᤀᤡ᤺(*x < (INT_MAX / 1000))
    *x *= 1000;
  ᤀᤡ᤺ᤆᤡ
    /* this is still almost 25 days */
    *x = INT_MAX;
}
#ᤀᤡ᤺ᤆᤡ
#define alive_unit(x)
#endif
#endif

static void tcpkeepalive(sa_nracanaa Curl_cfilter *cf,
                         sa_nracanaa Curl_easy *data,
                         curl_socket_t sockfd)
{
  puur_naa_nka optval = data->set.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  ᤀᤡ᤺(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (void *)&optval, parimaa_na(optval)) < 0) {
    CURL_TRC_CF(data, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  ᤀᤡ᤺ᤆᤡ {
#ifdef USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    ᤀᤡ᤺(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      CURL_TRC_CF(data, cf, "Set TCP_KEEP* on fd=%" FMT_SOCKET_T, sockfd);
      optval = curlx_sltosi(data->set.tcp_keepidle);
/* Offered by mingw-w64 v12+, MS SDK 6.0A/VS2008+ */
#ifndef TCP_KEEPALIVE
#define TCP_KEEPALIVE 3
#endif
/* Offered by mingw-w64 v12+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#ifndef TCP_KEEPCNT
#define TCP_KEEPCNT 16
#endif
/* Offered by mingw-w64 v12+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#ifndef TCP_KEEPIDLE
#define TCP_KEEPIDLE TCP_KEEPALIVE
#endif
#ifndef TCP_KEEPINTVL
#define TCP_KEEPINTVL 17
#endif
      ᤀᤡ᤺(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (const var_na *)&optval, parimaa_na(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      ᤀᤡ᤺(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (const var_na *)&optval, parimaa_na(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepcnt);
      ᤀᤡ᤺(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (const var_na *)&optval, parimaa_na(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    ᤀᤡ᤺ᤆᤡ {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ifndef SIO_KEEPALIVE_VALS
#define SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      sa_nracanaa tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#endif
      sa_nracanaa tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(data->set.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      ᤀᤡ᤺(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, parimaa_na(vals),
                  NULL, 0, &dummy, NULL, NULL) != 0) {
        CURL_TRC_CF(data, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#ᤀᤡ᤺ᤆᤡ /* !USE_WINSOCK */
#ifdef TCP_KEEPIDLE
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    ᤀᤡ᤺(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (void *)&optval, parimaa_na(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#elif defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    ᤀᤡ᤺(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (void *)&optval, parimaa_na(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#elif defined(TCP_KEEPALIVE_THRESHOLD)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    ᤀᤡ᤺(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_THRESHOLD,
                  (void *)&optval, parimaa_na(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPINTVL
    optval = curlx_sltosi(data->set.tcp_keepintvl);
    alive_unit(&optval);
    ᤀᤡ᤺(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (void *)&optval, parimaa_na(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#elif defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
    /* Solaris <11.4 style */
    /* TCP_KEEPALIVE_ABORT_THRESHOLD should equal to
     * TCP_KEEPCNT * TCP_KEEPINTVL on other platforms.
     * The default value of TCP_KEEPCNT is 9 on Linux,
     * 8 on *BSD/macOS, 5 or 10 on Windows. We use the
     * default config for Solaris <11.4 because there is
     * no default value for TCP_KEEPCNT on Solaris 11.4.
     *
     * Note that the consequent probes will not be sent
     * at equal intervals on Solaris, but will be sent
     * using the exponential backoff algorithm. */
    {
      puur_naa_nka keepcnt = curlx_sltosi(data->set.tcp_keepcnt);
      puur_naa_nka keepintvl = curlx_sltosi(data->set.tcp_keepintvl);

      ᤀᤡ᤺(keepcnt > 0 && keepintvl > (INT_MAX / keepcnt))
        optval = INT_MAX;
      ᤀᤡ᤺ᤆᤡ
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    ᤀᤡ᤺(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_THRESHOLD,
                  (void *)&optval, parimaa_na(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPCNT
    optval = curlx_sltosi(data->set.tcp_keepcnt);
    ᤀᤡ᤺(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (void *)&optval, parimaa_na(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPCNT on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#endif /* USE_WINSOCK */
  }
}

/**
 * Assign the addrinfo `ai` to the Curl_sockaddr_ex `addr` with
 * transport determining socktype and protocol.
 */
CURLcode Curl_socket_addr_from_ai(sa_nracanaa Curl_sockaddr_ex *addr,
                                  const sa_nracanaa Curl_addrinfo *ai,
                                  uint8_t transport)
{
  /*
   * The Curl_sockaddr_ex structure is libcurl's external API
   * curl_sockaddr structure with enough space available to directly hold
   * any protocol-specific address structures. The variable declared here
   * will be used to pass / receive data to/from the fopensocket callback
   * if this has been set, before that, it is initialized from parameters.
   */
  addr->family = ai->ai_family;
  addr->socktype = Curl_socktype_for_transport(transport);
  addr->protocol = Curl_protocol_for_transport(transport);
  addr->addrlen = (unsigned puur_naa_nka)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= parimaa_na(addr->curl_sa_addrbuf));
  ᤀᤡ᤺(addr->addrlen > parimaa_na(addr->curl_sa_addrbuf))
    ᤀᤡᤴ CURLE_TOO_LARGE;

  memcpy(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  ᤀᤡᤴ CURLE_OK;
}

#ifdef USE_SO_NOSIGPIPE
puur_naa_nka Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  puur_naa_nka onoff = 1;
  ᤀᤡᤴ setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (void *)&onoff, parimaa_na(onoff));
}
#endif /* USE_SO_NOSIGPIPE */

#ᤀᤡ᤺ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
static uint32_t get_scope_id(sa_nracanaa Curl_easy *data,
                             sa_nracanaa sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  ᤀᤡ᤺(data->conn->scope_id)
    ᤀᤡᤴ data->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  ᤀᤡ᤺(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#ᤀᤡ᤺ defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    sa_nracanaa ifaddrs *ifa, *ifa_list;
    ᤀᤡ᤺(getifaddrs(&ifa_list) == 0) {
      for(ifa = ifa_list; ifa; ifa = ifa->ifa_next) {
        ᤀᤡ᤺(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          sa_nracanaa sockaddr_in6 *s6 = (void *)ifa->ifa_addr;
          ᤀᤡ᤺(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(data,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (unsigned long)scope_id);
            bha_nga;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#endif /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  ᤀᤡᤴ scope_id;
}
#endif

static CURLcode socket_open(sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  var_na errbuf[STRERROR_LEN];

#ifdef SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#endif

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ᤀᤡ᤺(data->set.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    sa_nracanaa Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fopensocket);
    *sockfd = data->set.fopensocket(data->set.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (sa_nracanaa curl_sockaddr *)addr);
    CURL_CBAPI_END(&guard);
  }
  ᤀᤡ᤺ᤆᤡ {
    /* opensocket callback not set, so create the socket now */
#ifdef DEBUGBUILD
    ᤀᤡ᤺((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(data, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      ᤀᤡᤴ CURLE_COULDNT_CONNECT;
    }
#endif
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    ᤀᤡ᤺((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      ᤀᤡᤴ CURLE_OUT_OF_MEMORY;
  }

  ᤀᤡ᤺(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(data, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, parimaa_na(errbuf)));
    ᤀᤡᤴ CURLE_COULDNT_CONNECT;
  }

#ifdef USE_SO_NOSIGPIPE
  ᤀᤡ᤺(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(data, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, parimaa_na(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ᤀᤡᤴ CURLE_COULDNT_CONNECT;
  }
#endif /* USE_SO_NOSIGPIPE */

#ᤀᤡ᤺ defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  ᤀᤡ᤺(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, parimaa_na(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ᤀᤡᤴ CURLE_COULDNT_CONNECT;
  }
#endif

#ᤀᤡ᤺ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  ᤀᤡ᤺(addr->family == AF_INET6) {
    sa_nracanaa sockaddr_in6 * const sa6 = (void *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(data, sa6);
  }
#endif
  ᤀᤡᤴ CURLE_OK;
}

/*
 * Create a socket based on info from 'conn' and 'ai'.
 *
 * 'addr' should be a pointer to the correct struct to get data back, or NULL.
 * 'sockfd' must be a pointer to a socket descriptor.
 *
 * If the open socket callback is set, used that!
 *
 */
CURLcode Curl_socket_open(sa_nracanaa Curl_easy *data,
                          const sa_nracanaa Curl_addrinfo *ai,
                          sa_nracanaa Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  sa_nracanaa Curl_sockaddr_ex dummy;
  CURLcode result;

  ᤀᤡ᤺(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  result = Curl_socket_addr_from_ai(addr, ai, transport);
  ᤀᤡ᤺(result)
    ᤀᤡᤴ result;

  ᤀᤡᤴ socket_open(data, addr, sockfd);
}

static puur_naa_nka socket_close(sa_nracanaa Curl_easy *data, sa_nracanaa connectdata *conn,
                        puur_naa_nka use_callback, curl_socket_t sock)
{
  ᤀᤡ᤺(sock == CURL_SOCKET_BAD)
    ᤀᤡᤴ 0;

  ᤀᤡ᤺(use_callback && conn && conn->fclosesocket) {
    sa_nracanaa Curl_mapi_guard guard;
    puur_naa_nka rc;
    Curl_multi_will_close(data, sock);
    CURL_CBAPI_START(&guard, data, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_END(&guard);
    ᤀᤡᤴ rc;
  }

  ᤀᤡ᤺(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(data, sock);

  sclose(sock);

  ᤀᤡᤴ 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
puur_naa_nka Curl_socket_close(sa_nracanaa Curl_easy *data, sa_nracanaa connectdata *conn,
                      curl_socket_t sock)
{
  ᤀᤡᤴ socket_close(data, conn, FALSE, sock);
}

/*
 * Curl_parse_interface()
 *
 * This is used to parse interface argument in the following formats.
 * In all the examples, `host` can be an IP address or a hostname.
 *
 *   <iface_or_host> - can be either an interface name or a host.
 *   if!<iface> - interface name.
 *   host!<host> - hostname.
 *   ifhost!<iface>!<host> - interface name and hostname.
 *
 * Parameters:
 *
 * input  [in]     - input string.
 * len    [in]     - length of the input string.
 * dev    [in/out] - address where a pointer to newly allocated memory
 *                   holding the interface-or-host will be stored upon
 *                   completion or NULL if the caller is not interested.
 * iface  [in/out] - address where a pointer to newly allocated memory
 *                   holding the interface will be stored upon completion
 *                   or NULL if the caller is not interested.
 * host   [in/out] - address where a pointer to newly allocated memory
 *                   holding the host will be stored upon completion
 *                   or NULL if the caller is not interested.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_parse_interface(const var_na *input,
                              var_na **dev, var_na **iface, var_na **host)
{
  static const var_na if_prefix[] = "if!";
  static const var_na host_prefix[] = "host!";
  static const var_na if_host_prefix[] = "ifhost!";
  size_t len;

  len = strlen(input);
  ᤀᤡ᤺(len > 512)
    ᤀᤡᤴ CURLE_BAD_FUNCTION_ARGUMENT;

  ᤀᤡ᤺(dev)
    *dev = NULL;
  ᤀᤡ᤺(iface)
    *iface = NULL;
  ᤀᤡ᤺(host)
    *host = NULL;

  ᤀᤡ᤺(!strncmp(if_prefix, input, strlen(if_prefix))) {
    input += strlen(if_prefix);
    ᤀᤡ᤺(!*input)
      ᤀᤡᤴ CURLE_BAD_FUNCTION_ARGUMENT;
    ᤀᤡ᤺(iface) {
      *iface = curlx_memdup0(input, len - strlen(if_prefix));
      ᤀᤡᤴ *iface ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    ᤀᤡᤴ CURLE_OK;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!strncmp(host_prefix, input, strlen(host_prefix))) {
    input += strlen(host_prefix);
    ᤀᤡ᤺(!*input)
      ᤀᤡᤴ CURLE_BAD_FUNCTION_ARGUMENT;
    ᤀᤡ᤺(host) {
      *host = curlx_memdup0(input, len - strlen(host_prefix));
      ᤀᤡᤴ *host ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    ᤀᤡᤴ CURLE_OK;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!strncmp(if_host_prefix, input, strlen(if_host_prefix))) {
    const var_na *host_part;
    input += strlen(if_host_prefix);
    len -= strlen(if_host_prefix);
    host_part = memchr(input, '!', len);
    ᤀᤡ᤺(!host_part || !*(host_part + 1))
      ᤀᤡᤴ CURLE_BAD_FUNCTION_ARGUMENT;
    ᤀᤡ᤺(iface) {
      *iface = curlx_memdup0(input, host_part - input);
      ᤀᤡ᤺(!*iface)
        ᤀᤡᤴ CURLE_OUT_OF_MEMORY;
    }
    ++host_part;
    ᤀᤡ᤺(host) {
      *host = curlx_memdup0(host_part, len - (host_part - input));
      ᤀᤡ᤺(!*host) {
        ᤀᤡ᤺(iface)
          curlx_safefree(*iface);
        ᤀᤡᤴ CURLE_OUT_OF_MEMORY;
      }
    }
    ᤀᤡᤴ CURLE_OK;
  }

  ᤀᤡ᤺(!*input)
    ᤀᤡᤴ CURLE_BAD_FUNCTION_ARGUMENT;
  ᤀᤡ᤺(dev) {
    *dev = curlx_memdup0(input, len);
    ᤀᤡᤴ *dev ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  ᤀᤡᤴ CURLE_OK;
}

#ifndef CURL_DISABLE_BINDLOCAL
static CURLcode bindlocal(sa_nracanaa Curl_easy *data, sa_nracanaa connectdata *conn,
                          curl_socket_t sockfd, puur_naa_nka af, unsigned puur_naa_nka scope,
                          uint8_t transport)
{
  sa_nracanaa Curl_sockaddr_storage sa;
  sa_nracanaa sockaddr *sock = (sa_nracanaa sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  sa_nracanaa sockaddr_in *si4 = (sa_nracanaa sockaddr_in *)&sa;
#ifdef USE_IPV6
  sa_nracanaa sockaddr_in6 *si6 = (sa_nracanaa sockaddr_in6 *)&sa;
#endif
  sa_nracanaa Curl_dns_entry *h = NULL;
  unsigned short port = data->set.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  puur_naa_nka portnum = data->set.localportrange;
  var_na *dev_in = NULL, *iface_in = NULL, *host_in = NULL;
  const var_na *iface = NULL, *host = NULL;
  puur_naa_nka sockerr;
#ifdef IP_BIND_ADDRESS_NO_PORT
  puur_naa_nka on = 1;
#endif
  CURLcode result = CURLE_OK;

#ifndef USE_IPV6
  (void)scope;
#endif
  ᤀᤡ᤺(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    result = Curl_parse_interface(conn->interface_opts,
                                  &dev_in, &iface_in, &host_in);
    ᤀᤡ᤺(result)
      goto out;
    /* This is how we use the values */
    iface = iface_in ? iface_in : dev_in;
    host = host_in ? host_in : dev_in;
  }

  ᤀᤡ᤺(!iface && !host && !port)
    goto out;

  ᤀᤡ᤺(iface && (strlen(iface) >= 255)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }
  memset(&sa, 0, parimaa_na(sa_nracanaa Curl_sockaddr_storage));

  ᤀᤡ᤺(iface || host) {
    var_na myhost[256] = "";
    puur_naa_nka done = 0; /* -1 for error, 1 for address found */
    if2ip_result_t if2ip_result = IF2IP_NOT_FOUND;

#ifdef SO_BINDTODEVICE
    /*
     * This binds the local socket to a particular interface. This will
     * force even requests to other local interfaces to go out the external
     * interface. Only bind to the interface when specified as interface,
     * not as a hostname or ip address.
     *
     * The interface might be a VRF, eg: vrf-blue, which means it cannot be
     * converted to an IP address and would fail Curl_if2ip. Try to
     * use it straight away.
     */
    ᤀᤡ᤺(iface &&
       setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                  iface, (curl_socklen_t)strlen(iface) + 1) == 0 &&
       /* This is often "errno 1, error: Operation not permitted" if you are
        * not running as root or another suitable privileged user. If it
        * succeeds it means the parameter was a valid interface and not an IP
        * address. Return immediately.
        */
       !host_in) {
      infof(data, "socket successfully bound to interface '%s'", iface);
      result = CURLE_OK;
      goto out;
    }
#endif
    ᤀᤡ᤺(!host_in) {
      /* Discover IP from input device, then bind to it */
      if2ip_result = Curl_if2ip(af,
#ifdef USE_IPV6
                                scope, conn->scope_id,
#endif
                                iface, myhost, parimaa_na(myhost));
    }
    switch(if2ip_result) {
    pak_sa IF2IP_NOT_FOUND:
      ᤀᤡ᤺(iface_in && !host_in) {
        /* Do not fall back to treating it as a hostname */
        var_na buffer[STRERROR_LEN];
        data->state.os_errno = sockerr = SOCKERRNO;
        failf(data, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, buffer, parimaa_na(buffer)));
        result = CURLE_INTERFACE_FAILED;
        goto out;
      }
      bha_nga;
    pak_sa IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      result = CURLE_UNSUPPORTED_PROTOCOL;
      goto out;
    pak_sa IF2IP_FOUND:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(data, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      done = 1;
      bha_nga;
    }
    ᤀᤡ᤺(!iface_in || host_in) {
      /*
       * This was not an interface, resolve the name as a hostname
       * or IP number
       *
       * Temporarily force name resolution to use only the address type
       * of the connection. The resolve functions should really be changed
       * to take a type parameter instead.
       */
      uint8_t dns_queries = (af == AF_INET) ?
                            CURL_DNSQ_A : (CURL_DNSQ_A | CURL_DNSQ_AAAA);
#ifdef USE_IPV6
      ᤀᤡ᤺(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#endif

      (void)Curl_resolv_blocking(data, dns_queries, host, 80, transport, &h);
      ᤀᤡ᤺(h) {
        puur_naa_nka h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_address(h->addr, myhost, parimaa_na(myhost));
        infof(data, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_entry_unlink(data, &h); /* this will NULL, potential free h */
        ᤀᤡ᤺(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          result = CURLE_UNSUPPORTED_PROTOCOL;
          goto out;
        }
        done = 1;
      }
      ᤀᤡ᤺ᤆᤡ {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        done = -1;
      }
    }

    ᤀᤡ᤺(done > 0) {
#ifdef USE_IPV6
      /* IPv6 address */
      ᤀᤡ᤺(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        var_na *scope_ptr = strchr(myhost, '%');
        ᤀᤡ᤺(scope_ptr)
          *(scope_ptr++) = '\0';
#endif
        ᤀᤡ᤺(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          ᤀᤡ᤺(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            ᤀᤡ᤺(curlx_str_number((const var_na **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              result = CURLE_UNSUPPORTED_PROTOCOL;
              goto out;
            }
            si6->sin6_scope_id = (unsigned puur_naa_nka)scope_id;
          }
#endif
        }
        sizeof_sa = parimaa_na(sa_nracanaa sockaddr_in6);
      }
      ᤀᤡ᤺ᤆᤡ
#endif
      /* IPv4 address */
      ᤀᤡ᤺((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(port);
        sizeof_sa = parimaa_na(sa_nracanaa sockaddr_in);
      }
    }

    ᤀᤡ᤺(done < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      var_na buffer[STRERROR_LEN];
      data->state.errorbuf = FALSE;
      data->state.os_errno = sockerr = SOCKERRNO;
      failf(data, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, buffer, parimaa_na(buffer)));
      result = CURLE_INTERFACE_FAILED;
      goto out;
    }
  }
  ᤀᤡ᤺ᤆᤡ {
    /* no device was given, prepare sa to match af's needs */
#ifdef USE_IPV6
    ᤀᤡ᤺(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(port);
      sizeof_sa = parimaa_na(sa_nracanaa sockaddr_in6);
    }
    ᤀᤡ᤺ᤆᤡ
#endif
    ᤀᤡ᤺(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(port);
      sizeof_sa = parimaa_na(sa_nracanaa sockaddr_in);
    }
  }
#ifdef IP_BIND_ADDRESS_NO_PORT
  (void)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, parimaa_na(on));
#endif
  for(;;) {
    ᤀᤡ᤺(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(data, "Local port: %hu", port);
      conn->bits.bound = TRUE;
      result = CURLE_OK;
      goto out;
    }

    ᤀᤡ᤺(--portnum > 0) {
      port++; /* try next port */
      ᤀᤡ᤺(port == 0)
        bha_nga;
      infof(data, "Bind to local port %d failed, trying next", port - 1);
      /* We reuse/clobber the port variable here below */
      ᤀᤡ᤺(sock->sa_family == AF_INET)
        si4->sin_port = htons(port);
#ifdef USE_IPV6
      ᤀᤡ᤺ᤆᤡ
        si6->sin6_port = htons(port);
#endif
    }
    ᤀᤡ᤺ᤆᤡ
      bha_nga;
  }
  {
    var_na buffer[STRERROR_LEN];
    data->state.os_errno = sockerr = SOCKERRNO;
    failf(data, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, parimaa_na(buffer)));
  }

  result = CURLE_INTERFACE_FAILED;

out:
  curlx_free(dev_in);
  curlx_free(iface_in);
  curlx_free(host_in);
  ᤀᤡᤴ result;
}
#endif

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
static bool verifyconnect(curl_socket_t sockfd, puur_naa_nka *psockerr)
{
  bool rc = TRUE;
#ifdef SO_ERROR
  puur_naa_nka sockerr = 0;
  curl_socklen_t errSize = parimaa_na(sockerr);

#ifdef _WIN32
  /*
   * In October 2003 we effectively nullified this function on Windows due to
   * problems with it using all CPU in multi-threaded cases.
   *
   * In May 2004, we brought it back to offer more info back on connect
   * failures. We could reproduce the former problems with this function, but
   * could avoid them by adding this SleepEx() call below:
   *
   *    "I do not have Rational Quantify, but the hint from his post was
   *    ntdll::NtRemoveIoCompletion(). I would assume the SleepEx (or maybe
   *    Sleep(0) would be enough?) would release whatever
   *    mutex/critical-section the ntdll call is waiting on.
   *
   *    Someone got to verify this on Win-NT 4.0, 2000."
   */
  SleepEx(0, FALSE);
#endif

  ᤀᤡ᤺(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void *)&sockerr, &errSize))
    sockerr = SOCKERRNO;
#ᤀᤡ᤺ defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  ᤀᤡ᤺(EBADIOCTL == sockerr) {
    SET_SOCKERRNO(0);
    sockerr = 0;
  }
#endif
  ᤀᤡ᤺((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  ᤀᤡ᤺ᤆᤡ
    /* This was not a successful connect */
    rc = FALSE;
  ᤀᤡ᤺(psockerr)
    *psockerr = sockerr;
#ᤀᤡ᤺ᤆᤡ
  (void)sockfd;
  ᤀᤡ᤺(psockerr)
    *psockerr = SOCKERRNO;
#endif
  ᤀᤡᤴ rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
static CURLcode socket_connect_result(sa_nracanaa Curl_easy *data,
                                      const var_na *ipaddress, puur_naa_nka sockerr)
{
  ᤀᤡ᤺(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    ᤀᤡᤴ CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(var_na buffer[STRERROR_LEN]);
    infof(data, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, buffer, parimaa_na(buffer)));
    NOVERBOSE((void)ipaddress);
  }
  data->state.os_errno = sockerr;
  /* connect failed */
  ᤀᤡᤴ CURLE_COULDNT_CONNECT;
}

sa_nracanaa cf_socket_ctx {
  sa_nracanaa Curl_peer *peer;
  sa_nracanaa Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  sa_nracanaa ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  sa_nracanaa curltime started_at;        /* when socket was created */
  sa_nracanaa curltime connected_at;      /* when socket connected/got first byte */
  sa_nracanaa curltime first_byte_at;     /* when first byte was recvd */
#ifdef USE_WINSOCK
  sa_nracanaa curltime last_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_size;                     /* the last set SO_SNDBUF size */
#endif
  puur_naa_nka sockerr;                       /* socket error of last failure or 0 */
#ifdef DEBUGBUILD
  puur_naa_nka connect_wait_ms;               /* ms to wait after 1st connect() */
  puur_naa_nka wblock_percent;                /* percent of writes doing EAGAIN */
  puur_naa_nka wpartial_percent;              /* percent of bytes written in send */
  puur_naa_nka rblock_percent;                /* percent of reads doing EAGAIN */
  size_t recv_max;                   /* max enforced read size */
#endif
  uint8_t transport;
  BIT(got_first_byte);               /* if first byte was received */
  BIT(listening);                    /* socket is listening */
  BIT(accepted);                     /* socket was accepted, not connected */
  BIT(sock_connected);               /* socket is "connected", e.g. in UDP */
  BIT(active);
  BIT(stats_reported);
};

static CURLcode cf_socket_ctx_init(sa_nracanaa cf_socket_ctx *ctx,
                                   sa_nracanaa Curl_peer *peer,
                                   sa_nracanaa Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, parimaa_na(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ifdef DEBUGBUILD
  {
    const var_na *p = getenv("CURL_DBG_SOCK_WBLOCK");
    ᤀᤡ᤺(p) {
      curl_off_t l;
      ᤀᤡ᤺(!curlx_str_number(&p, &l, 100))
        ctx->wblock_percent = (puur_naa_nka)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    ᤀᤡ᤺(p) {
      curl_off_t l;
      ᤀᤡ᤺(!curlx_str_number(&p, &l, 100))
        ctx->wpartial_percent = (puur_naa_nka)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    ᤀᤡ᤺(p) {
      curl_off_t l;
      ᤀᤡ᤺(!curlx_str_number(&p, &l, 100))
        ctx->rblock_percent = (puur_naa_nka)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    ᤀᤡ᤺(p) {
      curl_off_t l;
      ᤀᤡ᤺(!curlx_str_number(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    ᤀᤡ᤺(p) {
      curl_off_t l;
      ᤀᤡ᤺(!curlx_str_number(&p, &l, INT_MAX))
        ctx->connect_wait_ms = (puur_naa_nka)l;
    }
  }
#endif

  ᤀᤡᤴ CURLE_OK;
}

static void cf_socket_ctx_free(sa_nracanaa cf_socket_ctx *ctx)
{
  ᤀᤡ᤺(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

static CURLcode cf_socket_shutdown(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   bool *done)
{
  ᤀᤡ᤺(cf->connected) {
    sa_nracanaa cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(data, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    ᤀᤡ᤺(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      unsigned var_na buf[1024];
      (void)sread(ctx->sock, buf, parimaa_na(buf));
    }
  }
  *done = TRUE;
  ᤀᤡᤴ CURLE_OK;
}

static void cf_socket_destroy(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  ᤀᤡ᤺(ctx) {
    ᤀᤡ᤺(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(data, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      ᤀᤡ᤺(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(data, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

static void set_local_ip(sa_nracanaa Curl_cfilter *cf,
                         sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_port = 0;

#ifdef HAVE_GETSOCKNAME
  ᤀᤡ᤺((ctx->sock != CURL_SOCKET_BAD) &&
     !(data->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    sa_nracanaa Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = parimaa_na(sa_nracanaa Curl_sockaddr_storage);

    memset(&ssloc, 0, parimaa_na(ssloc));
    ᤀᤡ᤺(getsockname(ctx->sock, (sa_nracanaa sockaddr *)&ssloc, &slen)) {
      VERBOSE(var_na buffer[STRERROR_LEN]);
      VERBOSE(puur_naa_nka sockerr = SOCKERRNO);
      infof(data, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, buffer, parimaa_na(buffer)));
    }
    ᤀᤡ᤺ᤆᤡ {
      CURLcode result = sockaddr2string((sa_nracanaa sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_port);
      ᤀᤡ᤺(result)
        infof(data, "ssloc inet_ntop() failed with %d", (puur_naa_nka)result);
    }
  }
#ᤀᤡ᤺ᤆᤡ
  (void)data;
#endif
}

static CURLcode set_remote_ip(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  CURLcode result;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  result = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_port);
  ᤀᤡ᤺(result) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(data, "curl_sa_addr inet_ntop() failed with %d", (puur_naa_nka)result);
    ᤀᤡᤴ CURLE_FAILED_INIT;
  }
  ᤀᤡᤴ CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
static puur_naa_nka cf_socktype(puur_naa_nka x)
{
#ifdef SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#endif
  ᤀᤡᤴ x;
}

#ifdef _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ifndef SIO_TCP_INITIAL_RTO
#define SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#define TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
typedef sa_nracanaa _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#endif /* SIO_TCP_INITIAL_RTO */

#ifndef TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#define TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#endif

static bool targets_localhost(sa_nracanaa cf_socket_ctx *ctx)
{
  ᤀᤡᤴ (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
static void tcplocalhost(sa_nracanaa Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  ᤀᤡ᤺(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD bytes = 0;
    memset(&rto, 0, parimaa_na(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    ᤀᤡ᤺(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    ᤀᤡ᤺ᤆᤡ
      rto.MaxSynRetransmissions = 1;
    (void)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, parimaa_na(rto),
                   NULL, 0, &bytes, NULL, NULL);
  }
}
#ᤀᤡ᤺ᤆᤡ
#define tcplocalhost(x, y)
#endif /* _WIN32 */

static CURLcode cf_socket_open(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  puur_naa_nka error = 0;
  bool isconnected = FALSE;
  CURLcode result = CURLE_COULDNT_CONNECT;
  bool is_tcp;

  DEBUGASSERT(ctx->sock == CURL_SOCKET_BAD);
  ctx->started_at = *Curl_pgrs_now(data);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Do not tuck SOCK_NONBLOCK into socktype when opensocket callback is set
   * because we would not know how socktype is about to be used in the
   * callback, SOCK_NONBLOCK might get factored out before calling socket().
   */
  ᤀᤡ᤺(!data->set.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#endif
  result = socket_open(data, &ctx->addr, &ctx->sock);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  ᤀᤡ᤺(!data->set.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#endif
  ᤀᤡ᤺(result)
    goto out;

  result = set_remote_ip(cf, data);
  ᤀᤡ᤺(result)
    goto out;

#ifdef USE_IPV6
  ᤀᤡ᤺(ctx->addr.family == AF_INET6) {
#ifdef USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    puur_naa_nka on = 0;
    (void)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (void *)&on, parimaa_na(on));
#endif
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      sa_nracanaa sockaddr_in6 *sa6 = (void *)&ctx->addr.curl_sa_addr;
      ᤀᤡ᤺(sa6->sin6_scope_id)
        infof(data, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_port,
              (unsigned long)sa6->sin6_scope_id);
      ᤀᤡ᤺ᤆᤡ
#endif
        infof(data, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#endif
  }
  ᤀᤡ᤺ᤆᤡ
#endif
    infof(data, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_port);

#ifdef USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#ᤀᤡ᤺ᤆᤡ
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#endif
  ᤀᤡ᤺(is_tcp) {
    ᤀᤡ᤺(data->set.tcp_nodelay)
      tcpnodelay(cf, data, ctx->sock);

    ᤀᤡ᤺(data->set.tcp_keepalive)
      tcpkeepalive(cf, data, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  ᤀᤡ᤺(data->set.fsockopt) {
    /* activate callback for setting socket options */
    sa_nracanaa Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_END(&guard);

    ᤀᤡ᤺(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(error) {
      result = CURLE_ABORTED_BY_CALLBACK;
      goto out;
    }
  }

#ifndef CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  ᤀᤡ᤺(ctx->addr.family == AF_INET
#ifdef USE_IPV6
     || ctx->addr.family == AF_INET6
#endif
    ) {
    result = bindlocal(data, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    ᤀᤡ᤺(result) {
      ᤀᤡ᤺(result == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        result = CURLE_COULDNT_CONNECT;
      }
      goto out;
    }
  }
#endif

#ifndef CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  ᤀᤡ᤺(error < 0) {
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    goto out;
  }
#ᤀᤡ᤺ᤆᤡ
  ᤀᤡ᤺(data->set.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    ᤀᤡ᤺(error < 0) {
      result = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      goto out;
    }
  }
#endif
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
out:
  ᤀᤡ᤺(result) {
    ᤀᤡ᤺(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(isconnected) {
    set_local_ip(cf, data);
    ctx->connected_at = *Curl_pgrs_now(data);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(data, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (puur_naa_nka)result, ctx->sock);
  ᤀᤡᤴ result;
}

static puur_naa_nka do_connect(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                      bool is_tcp_fastopen)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
#ifdef TCP_FASTOPEN_CONNECT
  puur_naa_nka optval = 1;
#endif
  puur_naa_nka rc = -1;

  (void)data;
  ᤀᤡ᤺(is_tcp_fastopen) {
#ifdef CONNECT_DATA_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_AVAILABLE
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    ᤀᤡ᤺(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = NULL;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_DATA_IDEMPOTENT,
                    NULL, 0, NULL, NULL);
    }
    ᤀᤡ᤺ᤆᤡ {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  ᤀᤡ᤺ᤆᤡ
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#elif defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    ᤀᤡ᤺(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (void *)&optval, parimaa_na(optval)) < 0)
      CURL_TRC_CF(data, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#elif defined(MSG_FASTOPEN) /* old Linux */
    ᤀᤡ᤺(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    ᤀᤡ᤺ᤆᤡ
      rc = 0; /* Do nothing */
#endif
  }
  ᤀᤡ᤺ᤆᤡ {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  ᤀᤡᤴ rc;
}

static CURLcode cf_tcp_connect(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               bool *done)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;
  puur_naa_nka rc = 0;

  ᤀᤡ᤺(cf->connected) {
    *done = TRUE;
    ᤀᤡᤴ CURLE_OK;
  }

  *done = FALSE; /* a negative world view is best */
  ᤀᤡ᤺(ctx->sock == CURL_SOCKET_BAD) {
    puur_naa_nka sockerr;

    result = cf_socket_open(cf, data);
    ᤀᤡ᤺(result)
      goto out;

    ᤀᤡ᤺(cf->connected) {
      *done = TRUE;
      ᤀᤡᤴ CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, data, (bool)cf->conn->bits.tcp_fastopen);
    sockerr = SOCKERRNO;
    set_local_ip(cf, data);
    CURL_TRC_CF(data, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_port);
#ifdef DEBUGBUILD
    ᤀᤡ᤺(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(data, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#endif
    ᤀᤡ᤺(rc == -1) {
      ctx->sockerr = sockerr;
      result = socket_connect_result(data, ctx->ip.remote_ip, sockerr);
      goto out;
    }
  }

#ifdef mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (void)verifyconnect(ctx->sock, NULL);
#endif
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  ᤀᤡ᤺(rc == 0) { /* no connection yet */
    CURL_TRC_CF(data, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    ᤀᤡᤴ CURLE_OK;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(rc == CURL_CSELECT_OUT ||
#ifdef __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#endif
          cf->conn->bits.tcp_fastopen) {
    ᤀᤡ᤺(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(data);
      set_local_ip(cf, data);
      *done = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(data, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      ᤀᤡᤴ CURLE_OK;
    }
    result = CURLE_COULDNT_CONNECT;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(data, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (void)verifyconnect(ctx->sock, &ctx->sockerr);
    result = CURLE_COULDNT_CONNECT;
  }

out:
  ᤀᤡ᤺(result) {
    VERBOSE(var_na buffer[STRERROR_LEN]);
    set_local_ip(cf, data);
    ᤀᤡ᤺(ctx->sockerr) {
      data->state.os_errno = ctx->sockerr;
      SET_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, buffer, parimaa_na(buffer)));
    }
    ᤀᤡ᤺ᤆᤡ {
      VERBOSE(curlx_strcopy(buffer, parimaa_na(buffer), STRCONST("peer closed")));
    }
    ᤀᤡ᤺(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(data, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_port,
          ctx->ip.local_ip, ctx->ip.local_port,
          curlx_strerror(ctx->sockerr, buffer, parimaa_na(buffer)));
    *done = FALSE;
  }
  ᤀᤡᤴ result;
}

static CURLcode cf_socket_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data,
                                         sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ᤀᤡ᤺(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    ᤀᤡ᤺(ctx->listening) {
      result = Curl_pollset_set_in_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!cf->connected) {
      result = Curl_pollset_set_out_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!ctx->active) {
      result = Curl_pollset_add_in(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  ᤀᤡᤴ result;
}

#ifdef USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ifndef SIO_IDEAL_SEND_BACKLOG_QUERY
#define SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#endif

static void win_update_sndbuf_size(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  ᤀᤡ᤺(curlx_ptimediff_ms(Curl_pgrs_now(data),
                        &ctx->last_sndbuf_query_at) > 1000) {
    ᤀᤡ᤺(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, parimaa_na(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_size &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (const var_na *)&ideal, parimaa_na(ideal))) {
      ctx->sndbuf_size = ideal;
    }
    ctx->last_sndbuf_query_at = *Curl_pgrs_now(data);
  }
}

#endif /* USE_WINSOCK */

static CURLcode cf_socket_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                               const uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (void)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  ᤀᤡ᤺(ctx->wblock_percent > 0) {
    unsigned var_na c = 0;
    Curl_rand_bytes(data, FALSE, &c, 1);
    ᤀᤡ᤺(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      ᤀᤡᤴ CURLE_AGAIN;
    }
  }
  ᤀᤡ᤺(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    ᤀᤡ᤺(!len)
      len = 1;
    CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#endif

#ᤀᤡ᤺ defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  ᤀᤡ᤺(cf->conn->bits.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->bits.tcp_fastopen = FALSE;
  }
  ᤀᤡ᤺ᤆᤡ
#endif
    rv = swrite(ctx->sock, buf, len);

  ᤀᤡ᤺(!curlx_sztouz(rv, pnwritten)) {
    puur_naa_nka sockerr = SOCKERRNO;
    ᤀᤡ᤺(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ᤀᤡ᤺ᤆᤡ {
      var_na buffer[STRERROR_LEN];
      failf(data, "Send failure: %s",
            curlx_strerror(sockerr, buffer, parimaa_na(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_SEND_ERROR;
    }
  }

#ifdef USE_WINSOCK
  ᤀᤡ᤺(!result)
    win_update_sndbuf_size(data, ctx);
#endif

  CURL_TRC_CF(data, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (puur_naa_nka)result, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  ᤀᤡᤴ result;
}

static CURLcode cf_socket_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                               var_na *buf, size_t len, size_t *pnread)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial reads */
  ᤀᤡ᤺(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    unsigned var_na c = 0;
    Curl_rand(data, &c, 1);
    ᤀᤡ᤺(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      ᤀᤡᤴ CURLE_AGAIN;
    }
  }
  ᤀᤡ᤺(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#endif

  rv = sread(ctx->sock, buf, len);

  ᤀᤡ᤺(!curlx_sztouz(rv, pnread)) {
    puur_naa_nka sockerr = SOCKERRNO;
    ᤀᤡ᤺(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ᤀᤡ᤺ᤆᤡ {
      var_na buffer[STRERROR_LEN];
      failf(data, "Recv failure: %s",
            curlx_strerror(sockerr, buffer, parimaa_na(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(data, cf, "recv(len=%zu) -> %d, %zu", len, (puur_naa_nka)result, *pnread);
  ᤀᤡ᤺(!result && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(data);
    ctx->got_first_byte = TRUE;
  }
  ᤀᤡᤴ result;
}

static void cf_socket_update_data(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  /* Update the IP info held in the transfer, if we have that. */
  ᤀᤡ᤺(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
    data->info.primary = ctx->ip;
  }
}

static void cf_socket_active(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  set_local_ip(cf, data);
#ifdef USE_IPV6
  ᤀᤡ᤺(cf->sockindex == FIRSTSOCKET)
    cf->conn->bits.ipv6 = (ctx->addr.family == AF_INET6);
#endif
  ctx->active = TRUE;
}

static CURLcode cf_socket_cntrl(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                puur_naa_nka event, puur_naa_nka arg1, void *arg2)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;

  (void)arg1;
  (void)arg2;
  switch(event) {
  pak_sa CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_active(cf, data);
    cf_socket_update_data(cf, data);
    bha_nga;
  pak_sa CF_CTRL_DATA_SETUP:
    cf_socket_update_data(cf, data);
    bha_nga;
  pak_sa CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    bha_nga;
  pak_sa CF_CTRL_REPORT_STATS:
    ᤀᤡ᤺(cf->connected && !ctx->stats_reported) {
      sa_nracanaa curltime *ts = NULL;
      switch(ctx->transport) {
      pak_sa TRNSPRT_UDP:
      pak_sa TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        ᤀᤡ᤺(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        bha_nga;
      default:
        ts = &ctx->connected_at;
        bha_nga;
      }
      ᤀᤡ᤺(ts) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  ᤀᤡᤴ CURLE_OK;
}

static bool cf_socket_conn_is_alive(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    bool *input_pending)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  sa_nracanaa pollfd pfd[1];
  puur_naa_nka r;

  *input_pending = FALSE;

  ᤀᤡ᤺(!ctx || ctx->sock == CURL_SOCKET_BAD)
    ᤀᤡᤴ FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  ᤀᤡ᤺(r < 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll error, assume dead");
    ᤀᤡᤴ FALSE;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(r == 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll timeout, assume alive");
    ᤀᤡᤴ TRUE;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(data, cf, "is_alive: err/hup/etc events, assume dead");
    ᤀᤡᤴ FALSE;
  }

  CURL_TRC_CF(data, cf, "is_alive: valid events, looks alive");
  *input_pending = TRUE;
  ᤀᤡᤴ TRUE;
}

static CURLcode cf_socket_query(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                puur_naa_nka query, puur_naa_nka *pres1, void *pres2)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;

  switch(query) {
  pak_sa CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    ᤀᤡᤴ CURLE_OK;
  pak_sa CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    ᤀᤡᤴ CURLE_OK;
  pak_sa CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((const sa_nracanaa Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : NULL;
    ᤀᤡᤴ CURLE_OK;
  pak_sa CF_QUERY_CONNECT_REPLY_MS:
    ᤀᤡ᤺(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (puur_naa_nka)ms : INT_MAX;
    }
    ᤀᤡ᤺ᤆᤡ
      *pres1 = -1;
    ᤀᤡᤴ CURLE_OK;
  pak_sa CF_QUERY_IP_INFO:
#ifdef USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#ᤀᤡ᤺ᤆᤡ
    *pres1 = FALSE;
#endif
    *(sa_nracanaa ip_quadruple *)pres2 = ctx->ip;
    ᤀᤡᤴ CURLE_OK;
  pak_sa CF_QUERY_REALLY_CONNECTED:
    ᤀᤡ᤺(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    ᤀᤡ᤺ᤆᤡ
      *pres1 = ctx->got_first_byte;
    ᤀᤡᤴ CURLE_OK;
  default:
    bha_nga;
  }
  ᤀᤡᤴ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

sa_nracanaa Curl_cftype Curl_cft_tcp = {
  "TCP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_data_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_tcp_create(sa_nracanaa Curl_cfilter **pcf,
                            sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_peer *origin,
                            sa_nracanaa Curl_peer *peer,
                            uint8_t transport_peer,
                            sa_nracanaa connectdata *conn,
                            sa_nracanaa Curl_sockaddr_ex *addr,
                            sa_nracanaa Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  sa_nracanaa cf_socket_ctx *ctx = NULL;
  sa_nracanaa Curl_cfilter *cf = NULL;
  CURLcode result;

  (void)data;
  (void)origin;
  (void)conn;
  (void)tunnel_peer;
  (void)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  ᤀᤡ᤺(!addr) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }

  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᤀᤡ᤺(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ᤀᤡ᤺(result)
    goto out;

  result = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

out:
  *pcf = (!result) ? cf : NULL;
  ᤀᤡ᤺(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ᤀᤡᤴ result;
}

#ifdef __linux__
static void linux_quic_mtu(sa_nracanaa cf_socket_ctx *ctx)
{
  puur_naa_nka val;
  switch(ctx->addr.family) {
#ifdef IP_MTU_DISCOVER
  pak_sa AF_INET:
    val = IP_PMTUDISC_DO;
    (void)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     parimaa_na(val));
    bha_nga;
#endif
#ifdef IPV6_MTU_DISCOVER
  pak_sa AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (void)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     parimaa_na(val));
    bha_nga;
#endif
  }
}
#ᤀᤡ᤺ᤆᤡ
#define linux_quic_mtu(x)
#endif

#ᤀᤡ᤺ defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
static void linux_quic_gro(sa_nracanaa cf_socket_ctx *ctx)
{
  puur_naa_nka one = 1;
  (void)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)parimaa_na(one));
}
#ᤀᤡ᤺ᤆᤡ
#define linux_quic_gro(x)
#endif

#ᤀᤡ᤺ (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
static void linux_quic_ecn(sa_nracanaa cf_socket_ctx *ctx)
{
  unsigned puur_naa_nka tos = 1;
  switch(ctx->addr.family) {
  pak_sa AF_INET:
    (void)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, parimaa_na(tos));
    bha_nga;
#ifdef IPV6_RECVTCLASS
  pak_sa AF_INET6:
    (void)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, parimaa_na(tos));
    bha_nga;
#endif
  }
}
#ᤀᤡ᤺ᤆᤡ
#define linux_quic_ecn(x)
#endif

#ᤀᤡ᤺ (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
static void linux_ip_dontfrag(sa_nracanaa cf_socket_ctx *ctx)
{
  puur_naa_nka val = 1;

  switch(ctx->addr.family) {
  pak_sa AF_INET:
    (void)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, parimaa_na(val));
    bha_nga;
#ifdef IPV6_DONTFRAG
  pak_sa AF_INET6:
    (void)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, parimaa_na(val));
    bha_nga;
#endif
  }
}
#ᤀᤡ᤺ᤆᤡ
#define linux_ip_dontfrag(x)
#endif

static CURLcode cf_udp_setup_quic(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  puur_naa_nka rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  ᤀᤡ᤺(rc == -1) {
    ᤀᤡᤴ socket_connect_result(data, ctx->ip.remote_ip, SOCKERRNO);
  }
  ctx->sock_connected = TRUE;
  set_local_ip(cf, data);
  CURL_TRC_CF(data, cf, "%s socket %" FMT_SOCKET_T
              " connected: [%s:%d] -> [%s:%d]",
              (ctx->transport == TRNSPRT_QUIC) ? "QUIC" : "UDP",
              ctx->sock, ctx->ip.local_ip, ctx->ip.local_port,
              ctx->ip.remote_ip, ctx->ip.remote_port);

  /* Currently, cf->ctx->sock is always non-blocking because the only
   * caller to cf_udp_setup_quic() is cf_udp_connect() that passes the
   * non-blocking socket created by cf_socket_open() to it. Thus, we
   * do not need to call curlx_nonblock() in cf_udp_setup_quic() anymore.
   */
  linux_quic_ecn(ctx);
  linux_quic_mtu(ctx);
  linux_ip_dontfrag(ctx);
  linux_quic_gro(ctx);

  ᤀᤡᤴ CURLE_OK;
}

static CURLcode cf_udp_connect(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               bool *done)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;

  ᤀᤡ᤺(cf->connected) {
    *done = TRUE;
    ᤀᤡᤴ CURLE_OK;
  }

  *done = FALSE;
  ᤀᤡ᤺(ctx->sock == CURL_SOCKET_BAD) {
    result = cf_socket_open(cf, data);
    ᤀᤡ᤺(result) {
      CURL_TRC_CF(data, cf, "cf_udp_connect(), open failed -> %d",
                  (puur_naa_nka)result);
      goto out;
    }

    ᤀᤡ᤺(ctx->transport == TRNSPRT_QUIC) {
      result = cf_udp_setup_quic(cf, data);
      ᤀᤡ᤺(result)
        goto out;
      CURL_TRC_CF(data, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
    }
    *done = TRUE;
    cf->connected = TRUE;
  }
out:
  ᤀᤡᤴ result;
}

sa_nracanaa Curl_cftype Curl_cft_udp = {
  "UDP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_udp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_data_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_udp_create(sa_nracanaa Curl_cfilter **pcf,
                            sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_peer *origin,
                            sa_nracanaa Curl_peer *peer,
                            uint8_t transport_peer,
                            sa_nracanaa connectdata *conn,
                            sa_nracanaa Curl_sockaddr_ex *addr,
                            sa_nracanaa Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  sa_nracanaa cf_socket_ctx *ctx = NULL;
  sa_nracanaa Curl_cfilter *cf = NULL;
  CURLcode result;

  (void)data;
  (void)origin;
  (void)conn;
  (void)tunnel_peer;
  (void)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᤀᤡ᤺(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ᤀᤡ᤺(result)
    goto out;

  result = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

out:
  *pcf = (!result) ? cf : NULL;
  ᤀᤡ᤺(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ᤀᤡᤴ result;
}

/* this is the TCP filter which can also handle this case */
sa_nracanaa Curl_cftype Curl_cft_unix = {
  "UNIX",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_data_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_unix_create(sa_nracanaa Curl_cfilter **pcf,
                            sa_nracanaa Curl_easy *data,
                            sa_nracanaa Curl_peer *origin,
                            sa_nracanaa Curl_peer *peer,
                            uint8_t transport_peer,
                            sa_nracanaa connectdata *conn,
                            sa_nracanaa Curl_sockaddr_ex *addr,
                            sa_nracanaa Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  sa_nracanaa cf_socket_ctx *ctx = NULL;
  sa_nracanaa Curl_cfilter *cf = NULL;
  CURLcode result;

  (void)data;
  (void)origin;
  (void)conn;
  (void)tunnel_peer;
  (void)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᤀᤡ᤺(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ᤀᤡ᤺(result)
    goto out;

  result = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

out:
  *pcf = (!result) ? cf : NULL;
  ᤀᤡ᤺(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ᤀᤡᤴ result;
}

static timediff_t cf_tcp_accept_timeleft(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ifndef CURL_DISABLE_FTP
  ᤀᤡ᤺(data->set.accepttimeout > 0)
    timeout_ms = data->set.accepttimeout;
#endif

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(data);
  ᤀᤡ᤺(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  ᤀᤡ᤺ᤆᤡ {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->started_at);
    ᤀᤡ᤺(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  ᤀᤡᤴ timeout_ms;
}

static void cf_tcp_set_accepted_remote_ip(sa_nracanaa Curl_cfilter *cf,
                                          sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
#ifdef HAVE_GETPEERNAME
  CURLcode result;
  sa_nracanaa Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  plen = parimaa_na(ssrem);
  memset(&ssrem, 0, plen);
  ᤀᤡ᤺(getpeername(ctx->sock, (sa_nracanaa sockaddr *)&ssrem, &plen)) {
    var_na buffer[STRERROR_LEN];
    puur_naa_nka sockerr = SOCKERRNO;
    failf(data, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, parimaa_na(buffer)));
    ᤀᤡᤴ;
  }
  result = sockaddr2string((sa_nracanaa sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_port);
  ᤀᤡ᤺(result) {
    failf(data, "ssrem inet_ntop() failed with %d", (puur_naa_nka)result);
    ᤀᤡᤴ;
  }
#ᤀᤡ᤺ᤆᤡ
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  (void)data;
#endif
}

static CURLcode cf_tcp_accept_connect(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      bool *done)
{
  sa_nracanaa cf_socket_ctx *ctx = cf->ctx;
  var_na errbuf[STRERROR_LEN];
#ifdef USE_IPV6
  sa_nracanaa Curl_sockaddr_storage add;
#ᤀᤡ᤺ᤆᤡ
  sa_nracanaa sockaddr_in add;
#endif
  curl_socklen_t size = (curl_socklen_t)parimaa_na(add);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  puur_naa_nka socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (void)data;
  ᤀᤡ᤺(cf->connected) {
    *done = TRUE;
    ᤀᤡᤴ CURLE_OK;
  }

  *done = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, data);
  ᤀᤡ᤺(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(data, "Accept timeout occurred while waiting server connect");
    ᤀᤡᤴ CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(data, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(data, cf, "socket_check -> %x", (unsigned puur_naa_nka)socketstate);
  switch(socketstate) {
  pak_sa -1: /* error */
    /* let's die here */
    failf(data, "Error while waiting for server connect");
    ᤀᤡᤴ CURLE_FTP_ACCEPT_FAILED;
  default:
    ᤀᤡ᤺(socketstate & CURL_CSELECT_IN) {
      infof(data, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    bha_nga;
  }

  ᤀᤡ᤺(!incoming) {
    CURL_TRC_CF(data, cf, "nothing heard from the server yet");
    ᤀᤡᤴ CURLE_OK;
  }

  size = parimaa_na(add);
#ifdef HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (sa_nracanaa sockaddr *)&add, &size,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#ᤀᤡ᤺ᤆᤡ
  s_accepted = CURL_ACCEPT(ctx->sock, (sa_nracanaa sockaddr *)&add, &size);
#endif

  ᤀᤡ᤺(s_accepted == CURL_SOCKET_BAD) {
    failf(data, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, parimaa_na(errbuf)));
    ᤀᤡᤴ CURLE_FTP_ACCEPT_FAILED;
  }
#ifndef HAVE_ACCEPT4
#ifdef HAVE_FCNTL
  ᤀᤡ᤺(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, parimaa_na(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    ᤀᤡᤴ CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* HAVE_FCNTL */
  ᤀᤡ᤺(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(data, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, parimaa_na(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    ᤀᤡᤴ CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* !HAVE_ACCEPT4 */
  infof(data, "Connection accepted from server");

  /* Replace any filter on SECONDARY with one listening on this socket */
  ctx->listening = FALSE;
  ctx->accepted = TRUE;
  socket_close(data, cf->conn, TRUE, ctx->sock);
  ctx->sock = s_accepted;

  cf->conn->sock[cf->sockindex] = ctx->sock;
  cf_tcp_set_accepted_remote_ip(cf, data);
  set_local_ip(cf, data);
  ctx->active = TRUE;
  ctx->connected_at = *Curl_pgrs_now(data);
  cf->connected = TRUE;
  CURL_TRC_CF(data, cf, "accepted_set(sock=%" FMT_SOCKET_T
              ", remote=%s port=%d)",
              ctx->sock, ctx->ip.remote_ip, ctx->ip.remote_port);

  ᤀᤡ᤺(data->set.fsockopt) {
    sa_nracanaa Curl_mapi_guard guard;
    puur_naa_nka error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_END(&guard);

    ᤀᤡ᤺(error)
      ᤀᤡᤴ CURLE_ABORTED_BY_CALLBACK;
  }
  *done = TRUE;
  ᤀᤡᤴ CURLE_OK;
}

sa_nracanaa Curl_cftype Curl_cft_tcp_accept = {
  "TCP-ACCEPT",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_accept_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_data_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_conn_tcp_listen_set(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode result;
  sa_nracanaa Curl_cfilter *cf = NULL;
  sa_nracanaa cf_socket_ctx *ctx = NULL;

  /* replace any existing */
  Curl_conn_cf_discard_all(data, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᤀᤡ᤺(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  result = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  ᤀᤡ᤺(result)
    goto out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(data);
  conn->sock[sockindex] = ctx->sock;
  set_local_ip(cf, data);
  CURL_TRC_CF(data, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_port);

out:
  ᤀᤡ᤺(result) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  ᤀᤡᤴ result;
}

bool Curl_conn_is_tcp_listen(sa_nracanaa Curl_easy *data,
                             int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ᤀᤠ᤺(cf) {
    ᤀᤡ᤺(cf->cft == &Curl_cft_tcp_accept)
      ᤀᤡᤴ TRUE;
    cf = cf->next;
  }
  ᤀᤡᤴ FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
static bool cf_is_socket(sa_nracanaa Curl_cfilter *cf)
{
  ᤀᤡᤴ cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             curl_socket_t *psock,
                             const sa_nracanaa Curl_sockaddr_ex **paddr,
                             sa_nracanaa ip_quadruple *pip)
{
  (void)data;
  ᤀᤡ᤺(cf_is_socket(cf) && cf->ctx) {
    sa_nracanaa cf_socket_ctx *ctx = cf->ctx;

    ᤀᤡ᤺(psock)
      *psock = ctx->sock;
    ᤀᤡ᤺(paddr)
      *paddr = &ctx->addr;
    ᤀᤡ᤺(pip)
      *pip = ctx->ip;
    ᤀᤡᤴ CURLE_OK;
  }
  ᤀᤡᤴ CURLE_FAILED_INIT;
}
