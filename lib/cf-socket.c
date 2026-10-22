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

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_SYS_UN_H
#ଅନ୍ତର୍ଭାଵ <sys/un.h> /* for sockaddr_un */
#endif
#ifdef HAVE_LINUX_TCP_H
#ଅନ୍ତର୍ଭାଵ <linux/tcp.h>
#ନଚେତଯଦି defined(HAVE_NETINET_TCP_H)
#ଅନ୍ତର୍ଭାଵ <netinet/tcp.h>
#endif
#ifdef HAVE_NETINET_UDP_H
#ଅନ୍ତର୍ଭାଵ <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#ଅନ୍ତର୍ଭାଵ <netinet/ip.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif

#ifdef HAVE_IFADDRS_H
#ଅନ୍ତର୍ଭାଵ <ifaddrs.h>
#endif
#ifdef HAVE_NET_IF_H
#ଅନ୍ତର୍ଭାଵ <net/if.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "if2ip.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-socket.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_ntop.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "conncache.h"
#ଅନ୍ତର୍ଭାଵ "multihandle.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "sockaddr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curlx/nonblock.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"
#ଅନ୍ତର୍ଭାଵ "curlx/version_win32.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strerr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curlx/wait.h"
#ifdef _WIN32
#ଅନ୍ତର୍ଭାଵ <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#endif

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(ସଂରଚନା sockaddr *sa, curl_socklen_t salen,
                                  ଅକ୍ଷର *addr, uint16_t *port);
UNITTEST CURLcode sockaddr2string(ସଂରଚନା sockaddr *sa, curl_socklen_t salen,
                                  ଅକ୍ଷର *addr, uint16_t *port)
{
  CURLcode result;
  ସଂରଚନା sockaddr_in *si = ଖାଲି;
#ifdef USE_IPV6
  ସଂରଚନା sockaddr_in6 *si6 = ଖାଲି;
#endif
#ifdef USE_UNIX_SOCKETS
  ସଂରଚନା sockaddr_un *su = ଖାଲି;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)salen;
#endif

  ବାଛ(sa->sa_family) {
  ସ୍ଥିତି AF_INET:
    si = (ସଂରଚନା sockaddr_in *)(ଶୂନ୍ୟ *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    ଯଦି(!result) {
      *port = ntohs(si->sin_port);
      ଫେରନ୍ତୁ result;
    }
    ଭାଙ୍ଗ;
#ifdef USE_IPV6
  ସ୍ଥିତି AF_INET6:
    si6 = (ସଂରଚନା sockaddr_in6 *)(ଶୂନ୍ୟ *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    ଯଦି(!result) {
      *port = ntohs(si6->sin6_port);
      ଫେରନ୍ତୁ result;
    }
    ଭାଙ୍ଗ;
#endif
#ifdef USE_UNIX_SOCKETS
  ସ୍ଥିତି AF_UNIX:
    ଯଦି(salen > (curl_socklen_t)ଆକାର(CURL_SA_FAMILY_T)) {
      su = (ସଂରଚନା sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_path);
    }
    ଅନ୍ୟଥା
      addr[0] = 0; /* socket with no name */
    *port = 0;
    ଫେରନ୍ତୁ CURLE_OK;
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ଭାଙ୍ଗ;
  }

  addr[0] = '\0';
  *port = 0;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ tcpnodelay(ସଂରଚନା Curl_cfilter *cf,
                       ସଂରଚନା Curl_easy *data,
                       curl_socket_t sockfd)
{
#ଯଦି defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା level = IPPROTO_TCP;
  VERBOSE(ଅକ୍ଷର buffer[STRERROR_LEN]);

  ଯଦି(setsockopt(sockfd, level, TCP_NODELAY, (ଶୂନ୍ୟ *)&onoff, ଆକାର(onoff)) < 0)
    CURL_TRC_CF(data, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, buffer, ଆକାର(buffer)));
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)sockfd;
#endif
}

#ଯଦି defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_THRESHOLD) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
#ଯଦି defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
ସ୍ଥିର ଶୂନ୍ୟ alive_unit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x)
{
  /* make sure this doesn't wrap */
  ଯଦି(*x < (INT_MAX / 1000))
    *x *= 1000;
  ଅନ୍ୟଥା
    /* this is still almost 25 days */
    *x = INT_MAX;
}
#ଅନ୍ୟଥା
#define alive_unit(x)
#endif
#endif

ସ୍ଥିର ଶୂନ୍ୟ tcpkeepalive(ସଂରଚନା Curl_cfilter *cf,
                         ସଂରଚନା Curl_easy *data,
                         curl_socket_t sockfd)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା optval = data->set.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  ଯଦି(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (ଶୂନ୍ୟ *)&optval, ଆକାର(optval)) < 0) {
    CURL_TRC_CF(data, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  ଅନ୍ୟଥା {
#ifdef USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    ଯଦି(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
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
      ଯଦି(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (ଅଚଳ ଅକ୍ଷର *)&optval, ଆକାର(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      ଯଦି(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (ଅଚଳ ଅକ୍ଷର *)&optval, ଆକାର(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepcnt);
      ଯଦି(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (ଅଚଳ ଅକ୍ଷର *)&optval, ଆକାର(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    ଅନ୍ୟଥା {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ifndef SIO_KEEPALIVE_VALS
#define SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      ସଂରଚନା tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#endif
      ସଂରଚନା tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(data->set.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      ଯଦି(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, ଆକାର(vals),
                  ଖାଲି, 0, &dummy, ଖାଲି, ଖାଲି) != 0) {
        CURL_TRC_CF(data, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#ଅନ୍ୟଥା /* !USE_WINSOCK */
#ifdef TCP_KEEPIDLE
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    ଯଦି(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (ଶୂନ୍ୟ *)&optval, ଆକାର(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ନଚେତଯଦି defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    ଯଦି(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (ଶୂନ୍ୟ *)&optval, ଆକାର(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ନଚେତଯଦି defined(TCP_KEEPALIVE_THRESHOLD)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    ଯଦି(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_THRESHOLD,
                  (ଶୂନ୍ୟ *)&optval, ଆକାର(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPINTVL
    optval = curlx_sltosi(data->set.tcp_keepintvl);
    alive_unit(&optval);
    ଯଦି(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (ଶୂନ୍ୟ *)&optval, ଆକାର(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ନଚେତଯଦି defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
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
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା keepcnt = curlx_sltosi(data->set.tcp_keepcnt);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା keepintvl = curlx_sltosi(data->set.tcp_keepintvl);

      ଯଦି(keepcnt > 0 && keepintvl > (INT_MAX / keepcnt))
        optval = INT_MAX;
      ଅନ୍ୟଥା
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    ଯଦି(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_THRESHOLD,
                  (ଶୂନ୍ୟ *)&optval, ଆକାର(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPCNT
    optval = curlx_sltosi(data->set.tcp_keepcnt);
    ଯଦି(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (ଶୂନ୍ୟ *)&optval, ଆକାର(optval)) < 0) {
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
CURLcode Curl_socket_addr_from_ai(ସଂରଚନା Curl_sockaddr_ex *addr,
                                  ଅଚଳ ସଂରଚନା Curl_addrinfo *ai,
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
  addr->addrlen = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= ଆକାର(addr->curl_sa_addrbuf));
  ଯଦି(addr->addrlen > ଆକାର(addr->curl_sa_addrbuf))
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;

  memcpy(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  ଫେରନ୍ତୁ CURLE_OK;
}

#ifdef USE_SO_NOSIGPIPE
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା onoff = 1;
  ଫେରନ୍ତୁ setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (ଶୂନ୍ୟ *)&onoff, ଆକାର(onoff));
}
#endif /* USE_SO_NOSIGPIPE */

#ଯଦି defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ସ୍ଥିର uint32_t get_scope_id(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  ଯଦି(data->conn->scope_id)
    ଫେରନ୍ତୁ data->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  ଯଦି(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#ଯଦି defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    ସଂରଚନା ifaddrs *ifa, *ifa_list;
    ଯଦି(getifaddrs(&ifa_list) == 0) {
      ପାଇଁ(ifa = ifa_list; ifa; ifa = ifa->ifa_next) {
        ଯଦି(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          ସଂରଚନା sockaddr_in6 *s6 = (ଶୂନ୍ୟ *)ifa->ifa_addr;
          ଯଦି(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(data,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (unsigned long)scope_id);
            ଭାଙ୍ଗ;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#endif /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  ଫେରନ୍ତୁ scope_id;
}
#endif

ସ୍ଥିର CURLcode socket_open(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  ଅକ୍ଷର errbuf[STRERROR_LEN];

#ifdef SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#endif

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ଯଦି(data->set.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    ସଂରଚନା Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fopensocket);
    *sockfd = data->set.fopensocket(data->set.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (ସଂରଚନା curl_sockaddr *)addr);
    CURL_CBAPI_END(&guard);
  }
  ଅନ୍ୟଥା {
    /* opensocket callback not set, so create the socket now */
#ifdef DEBUGBUILD
    ଯଦି((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(data, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
    }
#endif
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    ଯଦି((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଯଦି(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(data, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, ଆକାର(errbuf)));
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }

#ifdef USE_SO_NOSIGPIPE
  ଯଦି(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(data, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, ଆକାର(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }
#endif /* USE_SO_NOSIGPIPE */

#ଯଦି defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  ଯଦି(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, ଆକାର(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }
#endif

#ଯଦି defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  ଯଦି(addr->family == AF_INET6) {
    ସଂରଚନା sockaddr_in6 * ଅଚଳ sa6 = (ଶୂନ୍ୟ *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(data, sa6);
  }
#endif
  ଫେରନ୍ତୁ CURLE_OK;
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
CURLcode Curl_socket_open(ସଂରଚନା Curl_easy *data,
                          ଅଚଳ ସଂରଚନା Curl_addrinfo *ai,
                          ସଂରଚନା Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  ସଂରଚନା Curl_sockaddr_ex dummy;
  CURLcode result;

  ଯଦି(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  result = Curl_socket_addr_from_ai(addr, ai, transport);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଫେରନ୍ତୁ socket_open(data, addr, sockfd);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_close(ସଂରଚନା Curl_easy *data, ସଂରଚନା connectdata *conn,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_callback, curl_socket_t sock)
{
  ଯଦି(sock == CURL_SOCKET_BAD)
    ଫେରନ୍ତୁ 0;

  ଯଦି(use_callback && conn && conn->fclosesocket) {
    ସଂରଚନା Curl_mapi_guard guard;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
    Curl_multi_will_close(data, sock);
    CURL_CBAPI_START(&guard, data, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_END(&guard);
    ଫେରନ୍ତୁ rc;
  }

  ଯଦି(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(data, sock);

  sclose(sock);

  ଫେରନ୍ତୁ 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_socket_close(ସଂରଚନା Curl_easy *data, ସଂରଚନା connectdata *conn,
                      curl_socket_t sock)
{
  ଫେରନ୍ତୁ socket_close(data, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(ଅଚଳ ଅକ୍ଷର *ନିବେଶ,
                              ଅକ୍ଷର **dev, ଅକ୍ଷର **iface, ଅକ୍ଷର **host)
{
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର if_prefix[] = "if!";
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର host_prefix[] = "host!";
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର if_host_prefix[] = "ifhost!";
  size_t len;

  len = strlen(ନିବେଶ);
  ଯଦି(len > 512)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(dev)
    *dev = ଖାଲି;
  ଯଦି(iface)
    *iface = ଖାଲି;
  ଯଦି(host)
    *host = ଖାଲି;

  ଯଦି(!strncmp(if_prefix, ନିବେଶ, strlen(if_prefix))) {
    ନିବେଶ += strlen(if_prefix);
    ଯଦି(!*ନିବେଶ)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    ଯଦି(iface) {
      *iface = curlx_memdup0(ନିବେଶ, len - strlen(if_prefix));
      ଫେରନ୍ତୁ *iface ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(host_prefix, ନିବେଶ, strlen(host_prefix))) {
    ନିବେଶ += strlen(host_prefix);
    ଯଦି(!*ନିବେଶ)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    ଯଦି(host) {
      *host = curlx_memdup0(ନିବେଶ, len - strlen(host_prefix));
      ଫେରନ୍ତୁ *host ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(if_host_prefix, ନିବେଶ, strlen(if_host_prefix))) {
    ଅଚଳ ଅକ୍ଷର *host_part;
    ନିବେଶ += strlen(if_host_prefix);
    len -= strlen(if_host_prefix);
    host_part = memchr(ନିବେଶ, '!', len);
    ଯଦି(!host_part || !*(host_part + 1))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    ଯଦି(iface) {
      *iface = curlx_memdup0(ନିବେଶ, host_part - ନିବେଶ);
      ଯଦି(!*iface)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    ++host_part;
    ଯଦି(host) {
      *host = curlx_memdup0(host_part, len - (host_part - ନିବେଶ));
      ଯଦି(!*host) {
        ଯଦି(iface)
          curlx_safefree(*iface);
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
    }
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଯଦି(!*ନିବେଶ)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଯଦି(dev) {
    *dev = curlx_memdup0(ନିବେଶ, len);
    ଫେରନ୍ତୁ *dev ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

#ifndef CURL_DISABLE_BINDLOCAL
ସ୍ଥିର CURLcode bindlocal(ସଂରଚନା Curl_easy *data, ସଂରଚନା connectdata *conn,
                          curl_socket_t sockfd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା scope,
                          uint8_t transport)
{
  ସଂରଚନା Curl_sockaddr_storage sa;
  ସଂରଚନା sockaddr *sock = (ସଂରଚନା sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  ସଂରଚନା sockaddr_in *si4 = (ସଂରଚନା sockaddr_in *)&sa;
#ifdef USE_IPV6
  ସଂରଚନା sockaddr_in6 *si6 = (ସଂରଚନା sockaddr_in6 *)&sa;
#endif
  ସଂରଚନା Curl_dns_entry *h = ଖାଲି;
  unsigned short port = data->set.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା portnum = data->set.localportrange;
  ଅକ୍ଷର *dev_in = ଖାଲି, *iface_in = ଖାଲି, *host_in = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *iface = ଖାଲି, *host = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr;
#ifdef IP_BIND_ADDRESS_NO_PORT
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା on = 1;
#endif
  CURLcode result = CURLE_OK;

#ifndef USE_IPV6
  (ଶୂନ୍ୟ)scope;
#endif
  ଯଦି(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    result = Curl_parse_interface(conn->interface_opts,
                                  &dev_in, &iface_in, &host_in);
    ଯଦି(result)
      ଯାଅ out;
    /* This is how we use the values */
    iface = iface_in ? iface_in : dev_in;
    host = host_in ? host_in : dev_in;
  }

  ଯଦି(!iface && !host && !port)
    ଯାଅ out;

  ଯଦି(iface && (strlen(iface) >= 255)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }
  memset(&sa, 0, ଆକାର(ସଂରଚନା Curl_sockaddr_storage));

  ଯଦି(iface || host) {
    ଅକ୍ଷର myhost[256] = "";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା done = 0; /* -1 for error, 1 for address found */
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
    ଯଦି(iface &&
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
      ଯାଅ out;
    }
#endif
    ଯଦି(!host_in) {
      /* Discover IP from input device, then bind to it */
      if2ip_result = Curl_if2ip(af,
#ifdef USE_IPV6
                                scope, conn->scope_id,
#endif
                                iface, myhost, ଆକାର(myhost));
    }
    ବାଛ(if2ip_result) {
    ସ୍ଥିତି IF2IP_NOT_FOUND:
      ଯଦି(iface_in && !host_in) {
        /* Do not fall back to treating it as a hostname */
        ଅକ୍ଷର buffer[STRERROR_LEN];
        data->state.os_errno = sockerr = SOCKERRNO;
        failf(data, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, buffer, ଆକାର(buffer)));
        result = CURLE_INTERFACE_FAILED;
        ଯାଅ out;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      result = CURLE_UNSUPPORTED_PROTOCOL;
      ଯାଅ out;
    ସ୍ଥିତି IF2IP_FOUND:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(data, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      done = 1;
      ଭାଙ୍ଗ;
    }
    ଯଦି(!iface_in || host_in) {
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
      ଯଦି(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#endif

      (ଶୂନ୍ୟ)Curl_resolv_blocking(data, dns_queries, host, 80, transport, &h);
      ଯଦି(h) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_address(h->addr, myhost, ଆକାର(myhost));
        infof(data, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_entry_unlink(data, &h); /* this will NULL, potential free h */
        ଯଦି(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          result = CURLE_UNSUPPORTED_PROTOCOL;
          ଯାଅ out;
        }
        done = 1;
      }
      ଅନ୍ୟଥା {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        done = -1;
      }
    }

    ଯଦି(done > 0) {
#ifdef USE_IPV6
      /* IPv6 address */
      ଯଦି(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        ଅକ୍ଷର *scope_ptr = strchr(myhost, '%');
        ଯଦି(scope_ptr)
          *(scope_ptr++) = '\0';
#endif
        ଯଦି(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          ଯଦି(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            ଯଦି(curlx_str_ସଂଖ୍ୟା((ଅଚଳ ଅକ୍ଷର **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              result = CURLE_UNSUPPORTED_PROTOCOL;
              ଯାଅ out;
            }
            si6->sin6_scope_id = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)scope_id;
          }
#endif
        }
        sizeof_sa = ଆକାର(ସଂରଚନା sockaddr_in6);
      }
      ଅନ୍ୟଥା
#endif
      /* IPv4 address */
      ଯଦି((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(port);
        sizeof_sa = ଆକାର(ସଂରଚନା sockaddr_in);
      }
    }

    ଯଦି(done < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      ଅକ୍ଷର buffer[STRERROR_LEN];
      data->state.errorbuf = FALSE;
      data->state.os_errno = sockerr = SOCKERRNO;
      failf(data, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, buffer, ଆକାର(buffer)));
      result = CURLE_INTERFACE_FAILED;
      ଯାଅ out;
    }
  }
  ଅନ୍ୟଥା {
    /* no device was given, prepare sa to match af's needs */
#ifdef USE_IPV6
    ଯଦି(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(port);
      sizeof_sa = ଆକାର(ସଂରଚନା sockaddr_in6);
    }
    ଅନ୍ୟଥା
#endif
    ଯଦି(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(port);
      sizeof_sa = ଆକାର(ସଂରଚନା sockaddr_in);
    }
  }
#ifdef IP_BIND_ADDRESS_NO_PORT
  (ଶୂନ୍ୟ)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, ଆକାର(on));
#endif
  ପାଇଁ(;;) {
    ଯଦି(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(data, "Local port: %hu", port);
      conn->bits.bound = TRUE;
      result = CURLE_OK;
      ଯାଅ out;
    }

    ଯଦି(--portnum > 0) {
      port++; /* try next port */
      ଯଦି(port == 0)
        ଭାଙ୍ଗ;
      infof(data, "Bind to local port %d failed, trying next", port - 1);
      /* We reuse/clobber the port variable here below */
      ଯଦି(sock->sa_family == AF_INET)
        si4->sin_port = htons(port);
#ifdef USE_IPV6
      ଅନ୍ୟଥା
        si6->sin6_port = htons(port);
#endif
    }
    ଅନ୍ୟଥା
      ଭାଙ୍ଗ;
  }
  {
    ଅକ୍ଷର buffer[STRERROR_LEN];
    data->state.os_errno = sockerr = SOCKERRNO;
    failf(data, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, ଆକାର(buffer)));
  }

  result = CURLE_INTERFACE_FAILED;

out:
  curlx_free(dev_in);
  curlx_free(iface_in);
  curlx_free(host_in);
  ଫେରନ୍ତୁ result;
}
#endif

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
ସ୍ଥିର bool verifyconnect(curl_socket_t sockfd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *psockerr)
{
  bool rc = TRUE;
#ifdef SO_ERROR
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = 0;
  curl_socklen_t errSize = ଆକାର(sockerr);

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

  ଯଦି(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (ଶୂନ୍ୟ *)&sockerr, &errSize))
    sockerr = SOCKERRNO;
#ଯଦି defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  ଯଦି(EBADIOCTL == sockerr) {
    SET_SOCKERRNO(0);
    sockerr = 0;
  }
#endif
  ଯଦି((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  ଅନ୍ୟଥା
    /* This was not a successful connect */
    rc = FALSE;
  ଯଦି(psockerr)
    *psockerr = sockerr;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)sockfd;
  ଯଦି(psockerr)
    *psockerr = SOCKERRNO;
#endif
  ଫେରନ୍ତୁ rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
ସ୍ଥିର CURLcode socket_connect_result(ସଂରଚନା Curl_easy *data,
                                      ଅଚଳ ଅକ୍ଷର *ipaddress, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr)
{
  ଯଦି(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    ଫେରନ୍ତୁ CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(ଅକ୍ଷର buffer[STRERROR_LEN]);
    infof(data, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, buffer, ଆକାର(buffer)));
    NOVERBOSE((ଶୂନ୍ୟ)ipaddress);
  }
  data->state.os_errno = sockerr;
  /* connect failed */
  ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
}

ସଂରଚନା cf_socket_ctx {
  ସଂରଚନା Curl_peer *peer;
  ସଂରଚନା Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  ସଂରଚନା ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  ସଂରଚନା curltime started_at;        /* when socket was created */
  ସଂରଚନା curltime connected_at;      /* when socket connected/got first byte */
  ସଂରଚନା curltime first_byte_at;     /* when first byte was recvd */
#ifdef USE_WINSOCK
  ସଂରଚନା curltime last_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_size;                     /* the last set SO_SNDBUF size */
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr;                       /* socket error of last failure or 0 */
#ifdef DEBUGBUILD
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା connect_wait_ms;               /* ms to wait after 1st connect() */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା wblock_percent;                /* percent of writes doing EAGAIN */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା wpartial_percent;              /* percent of bytes written in send */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rblock_percent;                /* percent of reads doing EAGAIN */
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

ସ୍ଥିର CURLcode cf_socket_ctx_init(ସଂରଚନା cf_socket_ctx *ctx,
                                   ସଂରଚନା Curl_peer *peer,
                                   ସଂରଚନା Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, ଆକାର(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ifdef DEBUGBUILD
  {
    ଅଚଳ ଅକ୍ଷର *p = getenv("CURL_DBG_SOCK_WBLOCK");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, 100))
        ctx->wblock_percent = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, 100))
        ctx->wpartial_percent = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, 100))
        ctx->rblock_percent = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, INT_MAX))
        ctx->connect_wait_ms = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)l;
    }
  }
#endif

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_socket_ctx_free(ସଂରଚନା cf_socket_ctx *ctx)
{
  ଯଦି(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

ସ୍ଥିର CURLcode cf_socket_shutdown(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   bool *done)
{
  ଯଦି(cf->connected) {
    ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(data, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    ଯଦି(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      unsigned ଅକ୍ଷର buf[1024];
      (ଶୂନ୍ୟ)sread(ctx->sock, buf, ଆକାର(buf));
    }
  }
  *done = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_socket_destroy(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  ଯଦି(ctx) {
    ଯଦି(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(data, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      ଯଦି(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(data, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ set_local_ip(ସଂରଚନା Curl_cfilter *cf,
                         ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_port = 0;

#ifdef HAVE_GETSOCKNAME
  ଯଦି((ctx->sock != CURL_SOCKET_BAD) &&
     !(data->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    ସଂରଚନା Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = ଆକାର(ସଂରଚନା Curl_sockaddr_storage);

    memset(&ssloc, 0, ଆକାର(ssloc));
    ଯଦି(getsockname(ctx->sock, (ସଂରଚନା sockaddr *)&ssloc, &slen)) {
      VERBOSE(ଅକ୍ଷର buffer[STRERROR_LEN]);
      VERBOSE(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO);
      infof(data, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, buffer, ଆକାର(buffer)));
    }
    ଅନ୍ୟଥା {
      CURLcode result = sockaddr2string((ସଂରଚନା sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_port);
      ଯଦି(result)
        infof(data, "ssloc inet_ntop() failed with %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    }
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
#endif
}

ସ୍ଥିର CURLcode set_remote_ip(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  CURLcode result;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  result = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_port);
  ଯଦି(result) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(data, "curl_sa_addr inet_ntop() failed with %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cf_socktype(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x)
{
#ifdef SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#endif
  ଫେରନ୍ତୁ x;
}

#ifdef _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ifndef SIO_TCP_INITIAL_RTO
#define SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#define TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#endif /* SIO_TCP_INITIAL_RTO */

#ifndef TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#define TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#endif

ସ୍ଥିର bool targets_localhost(ସଂରଚନା cf_socket_ctx *ctx)
{
  ଫେରନ୍ତୁ (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
ସ୍ଥିର ଶୂନ୍ୟ tcplocalhost(ସଂରଚନା Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  ଯଦି(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD bytes = 0;
    memset(&rto, 0, ଆକାର(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    ଯଦି(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    ଅନ୍ୟଥା
      rto.MaxSynRetransmissions = 1;
    (ଶୂନ୍ୟ)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, ଆକାର(rto),
                   ଖାଲି, 0, &bytes, ଖାଲି, ଖାଲି);
  }
}
#ଅନ୍ୟଥା
#define tcplocalhost(x, y)
#endif /* _WIN32 */

ସ୍ଥିର CURLcode cf_socket_open(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = 0;
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
  ଯଦି(!data->set.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#endif
  result = socket_open(data, &ctx->addr, &ctx->sock);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  ଯଦି(!data->set.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#endif
  ଯଦି(result)
    ଯାଅ out;

  result = set_remote_ip(cf, data);
  ଯଦି(result)
    ଯାଅ out;

#ifdef USE_IPV6
  ଯଦି(ctx->addr.family == AF_INET6) {
#ifdef USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା on = 0;
    (ଶୂନ୍ୟ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (ଶୂନ୍ୟ *)&on, ଆକାର(on));
#endif
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      ସଂରଚନା sockaddr_in6 *sa6 = (ଶୂନ୍ୟ *)&ctx->addr.curl_sa_addr;
      ଯଦି(sa6->sin6_scope_id)
        infof(data, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_port,
              (unsigned long)sa6->sin6_scope_id);
      ଅନ୍ୟଥା
#endif
        infof(data, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#endif
  }
  ଅନ୍ୟଥା
#endif
    infof(data, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_port);

#ifdef USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#ଅନ୍ୟଥା
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#endif
  ଯଦି(is_tcp) {
    ଯଦି(data->set.tcp_nodelay)
      tcpnodelay(cf, data, ctx->sock);

    ଯଦି(data->set.tcp_keepalive)
      tcpkeepalive(cf, data, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  ଯଦି(data->set.fsockopt) {
    /* activate callback for setting socket options */
    ସଂରଚନା Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_END(&guard);

    ଯଦି(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    ଅନ୍ୟଥା ଯଦି(error) {
      result = CURLE_ABORTED_BY_CALLBACK;
      ଯାଅ out;
    }
  }

#ifndef CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  ଯଦି(ctx->addr.family == AF_INET
#ifdef USE_IPV6
     || ctx->addr.family == AF_INET6
#endif
    ) {
    result = bindlocal(data, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    ଯଦି(result) {
      ଯଦି(result == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        result = CURLE_COULDNT_CONNECT;
      }
      ଯାଅ out;
    }
  }
#endif

#ifndef CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  ଯଦି(error < 0) {
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    ଯାଅ out;
  }
#ଅନ୍ୟଥା
  ଯଦି(data->set.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    ଯଦି(error < 0) {
      result = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      ଯାଅ out;
    }
  }
#endif
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
out:
  ଯଦି(result) {
    ଯଦି(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  ଅନ୍ୟଥା ଯଦି(isconnected) {
    set_local_ip(cf, data);
    ctx->connected_at = *Curl_pgrs_now(data);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(data, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, ctx->sock);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_connect(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                      bool is_tcp_fastopen)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
#ifdef TCP_FASTOPEN_CONNECT
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା optval = 1;
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = -1;

  (ଶୂନ୍ୟ)data;
  ଯଦି(is_tcp_fastopen) {
#ifdef CONNECT_DATA_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_AVAILABLE
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    ଯଦି(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = ଖାଲି;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_DATA_IDEMPOTENT,
                    ଖାଲି, 0, ଖାଲି, ଖାଲି);
    }
    ଅନ୍ୟଥା {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  ଅନ୍ୟଥା
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#ନଚେତଯଦି defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    ଯଦି(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (ଶୂନ୍ୟ *)&optval, ଆକାର(optval)) < 0)
      CURL_TRC_CF(data, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#ନଚେତଯଦି defined(MSG_FASTOPEN) /* old Linux */
    ଯଦି(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    ଅନ୍ୟଥା
      rc = 0; /* Do nothing */
#endif
  }
  ଅନ୍ୟଥା {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର CURLcode cf_tcp_connect(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               bool *done)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *done = FALSE; /* a negative world view is best */
  ଯଦି(ctx->sock == CURL_SOCKET_BAD) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr;

    result = cf_socket_open(cf, data);
    ଯଦି(result)
      ଯାଅ out;

    ଯଦି(cf->connected) {
      *done = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, data, (bool)cf->conn->bits.tcp_fastopen);
    sockerr = SOCKERRNO;
    set_local_ip(cf, data);
    CURL_TRC_CF(data, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_port);
#ifdef DEBUGBUILD
    ଯଦି(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(data, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#endif
    ଯଦି(rc == -1) {
      ctx->sockerr = sockerr;
      result = socket_connect_result(data, ctx->ip.remote_ip, sockerr);
      ଯାଅ out;
    }
  }

#ifdef mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (ଶୂନ୍ୟ)verifyconnect(ctx->sock, ଖାଲି);
#endif
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  ଯଦି(rc == 0) { /* no connection yet */
    CURL_TRC_CF(data, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଅନ୍ୟଥା ଯଦି(rc == CURL_CSELECT_OUT ||
#ifdef __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#endif
          cf->conn->bits.tcp_fastopen) {
    ଯଦି(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(data);
      set_local_ip(cf, data);
      *done = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(data, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      ଫେରନ୍ତୁ CURLE_OK;
    }
    result = CURLE_COULDNT_CONNECT;
  }
  ଅନ୍ୟଥା ଯଦି(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(data, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (ଶୂନ୍ୟ)verifyconnect(ctx->sock, &ctx->sockerr);
    result = CURLE_COULDNT_CONNECT;
  }

out:
  ଯଦି(result) {
    VERBOSE(ଅକ୍ଷର buffer[STRERROR_LEN]);
    set_local_ip(cf, data);
    ଯଦି(ctx->sockerr) {
      data->state.os_errno = ctx->sockerr;
      SET_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, buffer, ଆକାର(buffer)));
    }
    ଅନ୍ୟଥା {
      VERBOSE(curlx_strcopy(buffer, ଆକାର(buffer), STRCONST("peer closed")));
    }
    ଯଦି(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(data, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_port,
          ctx->ip.local_ip, ctx->ip.local_port,
          curlx_strerror(ctx->sockerr, buffer, ଆକାର(buffer)));
    *done = FALSE;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_socket_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                         ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ଯଦି(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    ଯଦି(ctx->listening) {
      result = Curl_pollset_set_in_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ଅନ୍ୟଥା ଯଦି(!cf->connected) {
      result = Curl_pollset_set_out_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ଅନ୍ୟଥା ଯଦି(!ctx->active) {
      result = Curl_pollset_add_in(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  ଫେରନ୍ତୁ result;
}

#ifdef USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ifndef SIO_IDEAL_SEND_BACKLOG_QUERY
#define SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#endif

ସ୍ଥିର ଶୂନ୍ୟ win_update_sndbuf_size(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  ଯଦି(curlx_ptimediff_ms(Curl_pgrs_now(data),
                        &ctx->last_sndbuf_query_at) > 1000) {
    ଯଦି(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, ଆକାର(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_size &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (ଅଚଳ ଅକ୍ଷର *)&ideal, ଆକାର(ideal))) {
      ctx->sndbuf_size = ideal;
    }
    ctx->last_sndbuf_query_at = *Curl_pgrs_now(data);
  }
}

#endif /* USE_WINSOCK */

ସ୍ଥିର CURLcode cf_socket_send(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                               ଅଚଳ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (ଶୂନ୍ୟ)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  ଯଦି(ctx->wblock_percent > 0) {
    unsigned ଅକ୍ଷର c = 0;
    Curl_rand_bytes(data, FALSE, &c, 1);
    ଯଦି(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
  }
  ଯଦି(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    ଯଦି(!len)
      len = 1;
    CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#endif

#ଯଦି defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  ଯଦି(cf->conn->bits.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->bits.tcp_fastopen = FALSE;
  }
  ଅନ୍ୟଥା
#endif
    rv = swrite(ctx->sock, buf, len);

  ଯଦି(!curlx_sztouz(rv, pnwritten)) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;
    ଯଦି(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ଅନ୍ୟଥା {
      ଅକ୍ଷର buffer[STRERROR_LEN];
      failf(data, "Send failure: %s",
            curlx_strerror(sockerr, buffer, ଆକାର(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_SEND_ERROR;
    }
  }

#ifdef USE_WINSOCK
  ଯଦି(!result)
    win_update_sndbuf_size(data, ctx);
#endif

  CURL_TRC_CF(data, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_socket_recv(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                               ଅକ୍ଷର *buf, size_t len, size_t *pnread)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial reads */
  ଯଦି(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    unsigned ଅକ୍ଷର c = 0;
    Curl_rand(data, &c, 1);
    ଯଦି(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
  }
  ଯଦି(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#endif

  rv = sread(ctx->sock, buf, len);

  ଯଦି(!curlx_sztouz(rv, pnread)) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;
    ଯଦି(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ଅନ୍ୟଥା {
      ଅକ୍ଷର buffer[STRERROR_LEN];
      failf(data, "Recv failure: %s",
            curlx_strerror(sockerr, buffer, ଆକାର(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(data, cf, "recv(len=%zu) -> %d, %zu", len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread);
  ଯଦି(!result && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(data);
    ctx->got_first_byte = TRUE;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_socket_update_data(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data)
{
  /* Update the IP info held in the transfer, if we have that. */
  ଯଦି(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
    data->info.primary = ctx->ip;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ cf_socket_active(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  set_local_ip(cf, data);
#ifdef USE_IPV6
  ଯଦି(cf->sockindex == FIRSTSOCKET)
    cf->conn->bits.ipv6 = (ctx->addr.family == AF_INET6);
#endif
  ctx->active = TRUE;
}

ସ୍ଥିର CURLcode cf_socket_cntrl(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;

  (ଶୂନ୍ୟ)arg1;
  (ଶୂନ୍ୟ)arg2;
  ବାଛ(event) {
  ସ୍ଥିତି CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_active(cf, data);
    cf_socket_update_data(cf, data);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_DATA_SETUP:
    cf_socket_update_data(cf, data);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_REPORT_STATS:
    ଯଦି(cf->connected && !ctx->stats_reported) {
      ସଂରଚନା curltime *ts = ଖାଲି;
      ବାଛ(ctx->transport) {
      ସ୍ଥିତି TRNSPRT_UDP:
      ସ୍ଥିତି TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        ଯଦି(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ts = &ctx->connected_at;
        ଭାଙ୍ଗ;
      }
      ଯଦି(ts) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର bool cf_socket_conn_is_alive(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    bool *ନିବେଶ_pending)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  ସଂରଚନା pollfd pfd[1];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

  *ନିବେଶ_pending = FALSE;

  ଯଦି(!ctx || ctx->sock == CURL_SOCKET_BAD)
    ଫେରନ୍ତୁ FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  ଯଦି(r < 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll error, assume dead");
    ଫେରନ୍ତୁ FALSE;
  }
  ଅନ୍ୟଥା ଯଦି(r == 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll timeout, assume alive");
    ଫେରନ୍ତୁ TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(data, cf, "is_alive: err/hup/etc events, assume dead");
    ଫେରନ୍ତୁ FALSE;
  }

  CURL_TRC_CF(data, cf, "is_alive: valid events, looks alive");
  *ନିବେଶ_pending = TRUE;
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର CURLcode cf_socket_query(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;

  ବାଛ(query) {
  ସ୍ଥିତି CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((ଅଚଳ ସଂରଚନା Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : ଖାଲି;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CF_QUERY_CONNECT_REPLY_MS:
    ଯଦି(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ms : INT_MAX;
    }
    ଅନ୍ୟଥା
      *pres1 = -1;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CF_QUERY_IP_INFO:
#ifdef USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#ଅନ୍ୟଥା
    *pres1 = FALSE;
#endif
    *(ସଂରଚନା ip_quadruple *)pres2 = ctx->ip;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CF_QUERY_REALLY_CONNECTED:
    ଯଦି(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    ଅନ୍ୟଥା
      *pres1 = ctx->got_first_byte;
    ଫେରନ୍ତୁ CURLE_OK;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ସଂରଚନା Curl_cftype Curl_cft_tcp = {
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

CURLcode Curl_cf_tcp_create(ସଂରଚନା Curl_cfilter **pcf,
                            ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_peer *origin,
                            ସଂରଚନା Curl_peer *peer,
                            uint8_t transport_peer,
                            ସଂରଚନା connectdata *conn,
                            ସଂରଚନା Curl_sockaddr_ex *addr,
                            ସଂରଚନା Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ସଂରଚନା cf_socket_ctx *ctx = ଖାଲି;
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  CURLcode result;

  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)origin;
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)tunnel_peer;
  (ଶୂନ୍ୟ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  ଯଦି(!addr) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }

  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ଯଦି(result)
    ଯାଅ out;

  result = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

out:
  *pcf = (!result) ? cf : ଖାଲି;
  ଯଦି(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ଫେରନ୍ତୁ result;
}

#ifdef __linux__
ସ୍ଥିର ଶୂନ୍ୟ linux_quic_mtu(ସଂରଚନା cf_socket_ctx *ctx)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା val;
  ବାଛ(ctx->addr.family) {
#ifdef IP_MTU_DISCOVER
  ସ୍ଥିତି AF_INET:
    val = IP_PMTUDISC_DO;
    (ଶୂନ୍ୟ)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     ଆକାର(val));
    ଭାଙ୍ଗ;
#endif
#ifdef IPV6_MTU_DISCOVER
  ସ୍ଥିତି AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (ଶୂନ୍ୟ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     ଆକାର(val));
    ଭାଙ୍ଗ;
#endif
  }
}
#ଅନ୍ୟଥା
#define linux_quic_mtu(x)
#endif

#ଯଦି defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
ସ୍ଥିର ଶୂନ୍ୟ linux_quic_gro(ସଂରଚନା cf_socket_ctx *ctx)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା one = 1;
  (ଶୂନ୍ୟ)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)ଆକାର(one));
}
#ଅନ୍ୟଥା
#define linux_quic_gro(x)
#endif

#ଯଦି (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
ସ୍ଥିର ଶୂନ୍ୟ linux_quic_ecn(ସଂରଚନା cf_socket_ctx *ctx)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା tos = 1;
  ବାଛ(ctx->addr.family) {
  ସ୍ଥିତି AF_INET:
    (ଶୂନ୍ୟ)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, ଆକାର(tos));
    ଭାଙ୍ଗ;
#ifdef IPV6_RECVTCLASS
  ସ୍ଥିତି AF_INET6:
    (ଶୂନ୍ୟ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, ଆକାର(tos));
    ଭାଙ୍ଗ;
#endif
  }
}
#ଅନ୍ୟଥା
#define linux_quic_ecn(x)
#endif

#ଯଦି (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
ସ୍ଥିର ଶୂନ୍ୟ linux_ip_dontfrag(ସଂରଚନା cf_socket_ctx *ctx)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା val = 1;

  ବାଛ(ctx->addr.family) {
  ସ୍ଥିତି AF_INET:
    (ଶୂନ୍ୟ)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, ଆକାର(val));
    ଭାଙ୍ଗ;
#ifdef IPV6_DONTFRAG
  ସ୍ଥିତି AF_INET6:
    (ଶୂନ୍ୟ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, ଆକାର(val));
    ଭାଙ୍ଗ;
#endif
  }
}
#ଅନ୍ୟଥା
#define linux_ip_dontfrag(x)
#endif

ସ୍ଥିର CURLcode cf_udp_setup_quic(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  ଯଦି(rc == -1) {
    ଫେରନ୍ତୁ socket_connect_result(data, ctx->ip.remote_ip, SOCKERRNO);
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

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_udp_connect(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               bool *done)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *done = FALSE;
  ଯଦି(ctx->sock == CURL_SOCKET_BAD) {
    result = cf_socket_open(cf, data);
    ଯଦି(result) {
      CURL_TRC_CF(data, cf, "cf_udp_connect(), open failed -> %d",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଯାଅ out;
    }

    ଯଦି(ctx->transport == TRNSPRT_QUIC) {
      result = cf_udp_setup_quic(cf, data);
      ଯଦି(result)
        ଯାଅ out;
      CURL_TRC_CF(data, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
    }
    *done = TRUE;
    cf->connected = TRUE;
  }
out:
  ଫେରନ୍ତୁ result;
}

ସଂରଚନା Curl_cftype Curl_cft_udp = {
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

CURLcode Curl_cf_udp_create(ସଂରଚନା Curl_cfilter **pcf,
                            ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_peer *origin,
                            ସଂରଚନା Curl_peer *peer,
                            uint8_t transport_peer,
                            ସଂରଚନା connectdata *conn,
                            ସଂରଚନା Curl_sockaddr_ex *addr,
                            ସଂରଚନା Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ସଂରଚନା cf_socket_ctx *ctx = ଖାଲି;
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  CURLcode result;

  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)origin;
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)tunnel_peer;
  (ଶୂନ୍ୟ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ଯଦି(result)
    ଯାଅ out;

  result = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

out:
  *pcf = (!result) ? cf : ଖାଲି;
  ଯଦି(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ଫେରନ୍ତୁ result;
}

/* this is the TCP filter which can also handle this case */
ସଂରଚନା Curl_cftype Curl_cft_unix = {
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

CURLcode Curl_cf_unix_create(ସଂରଚନା Curl_cfilter **pcf,
                            ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_peer *origin,
                            ସଂରଚନା Curl_peer *peer,
                            uint8_t transport_peer,
                            ସଂରଚନା connectdata *conn,
                            ସଂରଚନା Curl_sockaddr_ex *addr,
                            ସଂରଚନା Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ସଂରଚନା cf_socket_ctx *ctx = ଖାଲି;
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  CURLcode result;

  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)origin;
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)tunnel_peer;
  (ଶୂନ୍ୟ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ଯଦି(result)
    ଯାଅ out;

  result = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

out:
  *pcf = (!result) ? cf : ଖାଲି;
  ଯଦି(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର timediff_t cf_tcp_accept_timeleft(ସଂରଚନା Curl_cfilter *cf,
                                         ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ifndef CURL_DISABLE_FTP
  ଯଦି(data->set.accepttimeout > 0)
    timeout_ms = data->set.accepttimeout;
#endif

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(data);
  ଯଦି(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  ଅନ୍ୟଥା {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->started_at);
    ଯଦି(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  ଫେରନ୍ତୁ timeout_ms;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_tcp_set_accepted_remote_ip(ସଂରଚନା Curl_cfilter *cf,
                                          ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
#ifdef HAVE_GETPEERNAME
  CURLcode result;
  ସଂରଚନା Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  plen = ଆକାର(ssrem);
  memset(&ssrem, 0, plen);
  ଯଦି(getpeername(ctx->sock, (ସଂରଚନା sockaddr *)&ssrem, &plen)) {
    ଅକ୍ଷର buffer[STRERROR_LEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;
    failf(data, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, ଆକାର(buffer)));
    ଫେରନ୍ତୁ;
  }
  result = sockaddr2string((ସଂରଚନା sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_port);
  ଯଦି(result) {
    failf(data, "ssrem inet_ntop() failed with %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଫେରନ୍ତୁ;
  }
#ଅନ୍ୟଥା
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  (ଶୂନ୍ୟ)data;
#endif
}

ସ୍ଥିର CURLcode cf_tcp_accept_connect(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      bool *done)
{
  ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;
  ଅକ୍ଷର errbuf[STRERROR_LEN];
#ifdef USE_IPV6
  ସଂରଚନା Curl_sockaddr_storage add;
#ଅନ୍ୟଥା
  ସଂରଚନା sockaddr_in add;
#endif
  curl_socklen_t size = (curl_socklen_t)ଆକାର(add);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (ଶୂନ୍ୟ)data;
  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *done = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, data);
  ଯଦି(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(data, "Accept timeout occurred while waiting server connect");
    ଫେରନ୍ତୁ CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(data, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(data, cf, "socket_check -> %x", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)socketstate);
  ବାଛ(socketstate) {
  ସ୍ଥିତି -1: /* error */
    /* let's die here */
    failf(data, "Error while waiting for server connect");
    ଫେରନ୍ତୁ CURLE_FTP_ACCEPT_FAILED;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯଦି(socketstate & CURL_CSELECT_IN) {
      infof(data, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    ଭାଙ୍ଗ;
  }

  ଯଦି(!incoming) {
    CURL_TRC_CF(data, cf, "nothing heard from the server yet");
    ଫେରନ୍ତୁ CURLE_OK;
  }

  size = ଆକାର(add);
#ifdef HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (ସଂରଚନା sockaddr *)&add, &size,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#ଅନ୍ୟଥା
  s_accepted = CURL_ACCEPT(ctx->sock, (ସଂରଚନା sockaddr *)&add, &size);
#endif

  ଯଦି(s_accepted == CURL_SOCKET_BAD) {
    failf(data, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, ଆକାର(errbuf)));
    ଫେରନ୍ତୁ CURLE_FTP_ACCEPT_FAILED;
  }
#ifndef HAVE_ACCEPT4
#ifdef HAVE_FCNTL
  ଯଦି(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, ଆକାର(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    ଫେରନ୍ତୁ CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* HAVE_FCNTL */
  ଯଦି(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(data, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, ଆକାର(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    ଫେରନ୍ତୁ CURLE_FTP_ACCEPT_FAILED;
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

  ଯଦି(data->set.fsockopt) {
    ସଂରଚନା Curl_mapi_guard guard;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_END(&guard);

    ଯଦି(error)
      ଫେରନ୍ତୁ CURLE_ABORTED_BY_CALLBACK;
  }
  *done = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସଂରଚନା Curl_cftype Curl_cft_tcp_accept = {
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

CURLcode Curl_conn_tcp_listen_set(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode result;
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା cf_socket_ctx *ctx = ଖାଲି;

  /* replace any existing */
  Curl_conn_cf_discard_all(data, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  result = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  ଯଦି(result)
    ଯାଅ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(data);
  conn->sock[sockindex] = ctx->sock;
  set_local_ip(cf, data);
  CURL_TRC_CF(data, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_port);

out:
  ଯଦି(result) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  ଫେରନ୍ତୁ result;
}

bool Curl_conn_is_tcp_listen(ସଂରଚନା Curl_easy *data,
                             int8_t sockindex)
{
  ସଂରଚନା Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ଯେତେବେଳେ(cf) {
    ଯଦି(cf->cft == &Curl_cft_tcp_accept)
      ଫେରନ୍ତୁ TRUE;
    cf = cf->next;
  }
  ଫେରନ୍ତୁ FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
ସ୍ଥିର bool cf_is_socket(ସଂରଚନା Curl_cfilter *cf)
{
  ଫେରନ୍ତୁ cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             curl_socket_t *psock,
                             ଅଚଳ ସଂରଚନା Curl_sockaddr_ex **paddr,
                             ସଂରଚନା ip_quadruple *pip)
{
  (ଶୂନ୍ୟ)data;
  ଯଦି(cf_is_socket(cf) && cf->ctx) {
    ସଂରଚନା cf_socket_ctx *ctx = cf->ctx;

    ଯଦି(psock)
      *psock = ctx->sock;
    ଯଦି(paddr)
      *paddr = &ctx->addr;
    ଯଦି(pip)
      *pip = ctx->ip;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଫେରନ୍ତୁ CURLE_FAILED_INIT;
}
