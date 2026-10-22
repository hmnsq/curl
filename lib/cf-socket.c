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

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_SYS_UN_H
#অন্তর্ভাব <sys/un.h> /* for sockaddr_un */
#endif
#ifdef HAVE_LINUX_TCP_H
#অন্তর্ভাব <linux/tcp.h>
#নতুবা_যদি defined(HAVE_NETINET_TCP_H)
#অন্তর্ভাব <netinet/tcp.h>
#endif
#ifdef HAVE_NETINET_UDP_H
#অন্তর্ভাব <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#অন্তর্ভাব <netinet/ip.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif

#ifdef HAVE_IFADDRS_H
#অন্তর্ভাব <ifaddrs.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাব <net/if.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "if2ip.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-socket.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "curlx/inet_ntop.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "conncache.h"
#অন্তর্ভাব "multihandle.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "sockaddr.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/nonblock.h"
#অন্তর্ভাব "curlx/strcopy.h"
#অন্তর্ভাব "curlx/version_win32.h"
#অন্তর্ভাব "curlx/strerr.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curlx/wait.h"
#ifdef _WIN32
#অন্তর্ভাব <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#endif

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(কাঠামো sockaddr *sa, curl_socklen_t salen,
                                  অক্ষর *addr, uint16_t *port);
UNITTEST CURLcode sockaddr2string(কাঠামো sockaddr *sa, curl_socklen_t salen,
                                  অক্ষর *addr, uint16_t *port)
{
  CURLcode ফলাফল;
  কাঠামো sockaddr_in *si = ফাঁকা;
#ifdef USE_IPV6
  কাঠামো sockaddr_in6 *si6 = ফাঁকা;
#endif
#ifdef USE_UNIX_SOCKETS
  কাঠামো sockaddr_un *su = ফাঁকা;
#অন্যথা
  (শূন্য)salen;
#endif

  নির্বাচন(sa->sa_family) {
  ক্ষেত্র AF_INET:
    si = (কাঠামো sockaddr_in *)(শূন্য *)sa;
    ফলাফল = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    যদি(!ফলাফল) {
      *port = ntohs(si->sin_port);
      ফেরত ফলাফল;
    }
    বিরতি;
#ifdef USE_IPV6
  ক্ষেত্র AF_INET6:
    si6 = (কাঠামো sockaddr_in6 *)(শূন্য *)sa;
    ফলাফল = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    যদি(!ফলাফল) {
      *port = ntohs(si6->sin6_port);
      ফেরত ফলাফল;
    }
    বিরতি;
#endif
#ifdef USE_UNIX_SOCKETS
  ক্ষেত্র AF_UNIX:
    যদি(salen > (curl_socklen_t)আকার(CURL_SA_FAMILY_T)) {
      su = (কাঠামো sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_path);
    }
    অন্যথা
      addr[0] = 0; /* socket with no name */
    *port = 0;
    ফেরত CURLE_OK;
#endif
  পূর্বনির্ধারিত:
    ফলাফল = CURLE_UNSUPPORTED_PROTOCOL;
    বিরতি;
  }

  addr[0] = '\0';
  *port = 0;
  ফেরত ফলাফল;
}

স্থির শূন্য tcpnodelay(কাঠামো Curl_cfilter *cf,
                       কাঠামো Curl_easy *তথ্য,
                       curl_socket_t sockfd)
{
#যদি defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  পূর্ণসংখ্যা level = IPPROTO_TCP;
  VERBOSE(অক্ষর বাফার[STRERROR_LEN]);

  যদি(setsockopt(sockfd, level, TCP_NODELAY, (শূন্য *)&onoff, আকার(onoff)) < 0)
    CURL_TRC_CF(তথ্য, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, বাফার, আকার(বাফার)));
#অন্যথা
  (শূন্য)cf;
  (শূন্য)তথ্য;
  (শূন্য)sockfd;
#endif
}

#যদি defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_THRESHOLD) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
#যদি defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
স্থির শূন্য alive_unit(পূর্ণসংখ্যা *x)
{
  /* make sure this doesn't wrap */
  যদি(*x < (INT_MAX / 1000))
    *x *= 1000;
  অন্যথা
    /* this is still almost 25 days */
    *x = INT_MAX;
}
#অন্যথা
#define alive_unit(x)
#endif
#endif

স্থির শূন্য tcpkeepalive(কাঠামো Curl_cfilter *cf,
                         কাঠামো Curl_easy *তথ্য,
                         curl_socket_t sockfd)
{
  পূর্ণসংখ্যা optval = তথ্য->set.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  যদি(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (শূন্য *)&optval, আকার(optval)) < 0) {
    CURL_TRC_CF(তথ্য, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  অন্যথা {
#ifdef USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    যদি(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      CURL_TRC_CF(তথ্য, cf, "Set TCP_KEEP* on fd=%" FMT_SOCKET_T, sockfd);
      optval = curlx_sltosi(তথ্য->set.tcp_keepidle);
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
      যদি(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (ধ্রুবক অক্ষর *)&optval, আকার(optval)) < 0) {
        CURL_TRC_CF(তথ্য, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(তথ্য->set.tcp_keepintvl);
      যদি(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (ধ্রুবক অক্ষর *)&optval, আকার(optval)) < 0) {
        CURL_TRC_CF(তথ্য, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(তথ্য->set.tcp_keepcnt);
      যদি(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (ধ্রুবক অক্ষর *)&optval, আকার(optval)) < 0) {
        CURL_TRC_CF(তথ্য, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    অন্যথা {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ifndef SIO_KEEPALIVE_VALS
#define SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      কাঠামো tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#endif
      কাঠামো tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(তথ্য->set.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(তথ্য->set.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      যদি(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, আকার(vals),
                  ফাঁকা, 0, &dummy, ফাঁকা, ফাঁকা) != 0) {
        CURL_TRC_CF(তথ্য, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#অন্যথা /* !USE_WINSOCK */
#ifdef TCP_KEEPIDLE
    optval = curlx_sltosi(তথ্য->set.tcp_keepidle);
    alive_unit(&optval);
    যদি(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (শূন্য *)&optval, আকার(optval)) < 0) {
      CURL_TRC_CF(তথ্য, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#নতুবা_যদি defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(তথ্য->set.tcp_keepidle);
    alive_unit(&optval);
    যদি(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (শূন্য *)&optval, আকার(optval)) < 0) {
      CURL_TRC_CF(তথ্য, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#নতুবা_যদি defined(TCP_KEEPALIVE_THRESHOLD)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(তথ্য->set.tcp_keepidle);
    alive_unit(&optval);
    যদি(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_THRESHOLD,
                  (শূন্য *)&optval, আকার(optval)) < 0) {
      CURL_TRC_CF(তথ্য, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPINTVL
    optval = curlx_sltosi(তথ্য->set.tcp_keepintvl);
    alive_unit(&optval);
    যদি(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (শূন্য *)&optval, আকার(optval)) < 0) {
      CURL_TRC_CF(তথ্য, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#নতুবা_যদি defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
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
      পূর্ণসংখ্যা keepcnt = curlx_sltosi(তথ্য->set.tcp_keepcnt);
      পূর্ণসংখ্যা keepintvl = curlx_sltosi(তথ্য->set.tcp_keepintvl);

      যদি(keepcnt > 0 && keepintvl > (INT_MAX / keepcnt))
        optval = INT_MAX;
      অন্যথা
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    যদি(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_THRESHOLD,
                  (শূন্য *)&optval, আকার(optval)) < 0) {
      CURL_TRC_CF(তথ্য, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPCNT
    optval = curlx_sltosi(তথ্য->set.tcp_keepcnt);
    যদি(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (শূন্য *)&optval, আকার(optval)) < 0) {
      CURL_TRC_CF(তথ্য, cf, "Failed to set TCP_KEEPCNT on fd "
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
CURLcode Curl_socket_addr_from_ai(কাঠামো Curl_sockaddr_ex *addr,
                                  ধ্রুবক কাঠামো Curl_addrinfo *ai,
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
  addr->addrlen = (unsigned পূর্ণসংখ্যা)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= আকার(addr->curl_sa_addrbuf));
  যদি(addr->addrlen > আকার(addr->curl_sa_addrbuf))
    ফেরত CURLE_TOO_LARGE;

  memcpy(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  ফেরত CURLE_OK;
}

#ifdef USE_SO_NOSIGPIPE
পূর্ণসংখ্যা Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  পূর্ণসংখ্যা onoff = 1;
  ফেরত setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (শূন্য *)&onoff, আকার(onoff));
}
#endif /* USE_SO_NOSIGPIPE */

#যদি defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
স্থির uint32_t get_scope_id(কাঠামো Curl_easy *তথ্য,
                             কাঠামো sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  যদি(তথ্য->conn->scope_id)
    ফেরত তথ্য->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  যদি(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#যদি defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    কাঠামো ifaddrs *ifa, *ifa_list;
    যদি(getifaddrs(&ifa_list) == 0) {
      জন্য(ifa = ifa_list; ifa; ifa = ifa->ifa_পরবর্তী) {
        যদি(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          কাঠামো sockaddr_in6 *s6 = (শূন্য *)ifa->ifa_addr;
          যদি(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(তথ্য,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (unsigned long)scope_id);
            বিরতি;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#endif /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  ফেরত scope_id;
}
#endif

স্থির CURLcode socket_open(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  অক্ষর errbuf[STRERROR_LEN];

#ifdef SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#endif

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);
  যদি(তথ্য->set.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    কাঠামো Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, তথ্য, easy_fopensocket);
    *sockfd = তথ্য->set.fopensocket(তথ্য->set.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (কাঠামো curl_sockaddr *)addr);
    CURL_CBAPI_END(&guard);
  }
  অন্যথা {
    /* opensocket callback not set, so create the socket now */
#ifdef DEBUGBUILD
    যদি((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(তথ্য, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      ফেরত CURLE_COULDNT_CONNECT;
    }
#endif
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    যদি((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(তথ্য, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, আকার(errbuf)));
    ফেরত CURLE_COULDNT_CONNECT;
  }

#ifdef USE_SO_NOSIGPIPE
  যদি(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(তথ্য, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, আকার(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ফেরত CURLE_COULDNT_CONNECT;
  }
#endif /* USE_SO_NOSIGPIPE */

#যদি defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  যদি(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(তথ্য, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, আকার(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ফেরত CURLE_COULDNT_CONNECT;
  }
#endif

#যদি defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  যদি(addr->family == AF_INET6) {
    কাঠামো sockaddr_in6 * ধ্রুবক sa6 = (শূন্য *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(তথ্য, sa6);
  }
#endif
  ফেরত CURLE_OK;
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
CURLcode Curl_socket_open(কাঠামো Curl_easy *তথ্য,
                          ধ্রুবক কাঠামো Curl_addrinfo *ai,
                          কাঠামো Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  কাঠামো Curl_sockaddr_ex dummy;
  CURLcode ফলাফল;

  যদি(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  ফলাফল = Curl_socket_addr_from_ai(addr, ai, transport);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফেরত socket_open(তথ্য, addr, sockfd);
}

স্থির পূর্ণসংখ্যা socket_close(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn,
                        পূর্ণসংখ্যা use_callback, curl_socket_t sock)
{
  যদি(sock == CURL_SOCKET_BAD)
    ফেরত 0;

  যদি(use_callback && conn && conn->fclosesocket) {
    কাঠামো Curl_mapi_guard guard;
    পূর্ণসংখ্যা rc;
    Curl_multi_will_close(তথ্য, sock);
    CURL_CBAPI_START(&guard, তথ্য, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_END(&guard);
    ফেরত rc;
  }

  যদি(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(তথ্য, sock);

  sclose(sock);

  ফেরত 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
পূর্ণসংখ্যা Curl_socket_close(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn,
                      curl_socket_t sock)
{
  ফেরত socket_close(তথ্য, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(ধ্রুবক অক্ষর *প্রবেশ,
                              অক্ষর **dev, অক্ষর **iface, অক্ষর **host)
{
  স্থির ধ্রুবক অক্ষর if_prefix[] = "if!";
  স্থির ধ্রুবক অক্ষর host_prefix[] = "host!";
  স্থির ধ্রুবক অক্ষর if_host_prefix[] = "ifhost!";
  size_t len;

  len = strlen(প্রবেশ);
  যদি(len > 512)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(dev)
    *dev = ফাঁকা;
  যদি(iface)
    *iface = ফাঁকা;
  যদি(host)
    *host = ফাঁকা;

  যদি(!strncmp(if_prefix, প্রবেশ, strlen(if_prefix))) {
    প্রবেশ += strlen(if_prefix);
    যদি(!*প্রবেশ)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(iface) {
      *iface = curlx_memdup0(প্রবেশ, len - strlen(if_prefix));
      ফেরত *iface ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    ফেরত CURLE_OK;
  }
  অন্যথা যদি(!strncmp(host_prefix, প্রবেশ, strlen(host_prefix))) {
    প্রবেশ += strlen(host_prefix);
    যদি(!*প্রবেশ)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(host) {
      *host = curlx_memdup0(প্রবেশ, len - strlen(host_prefix));
      ফেরত *host ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    ফেরত CURLE_OK;
  }
  অন্যথা যদি(!strncmp(if_host_prefix, প্রবেশ, strlen(if_host_prefix))) {
    ধ্রুবক অক্ষর *host_part;
    প্রবেশ += strlen(if_host_prefix);
    len -= strlen(if_host_prefix);
    host_part = memchr(প্রবেশ, '!', len);
    যদি(!host_part || !*(host_part + 1))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(iface) {
      *iface = curlx_memdup0(প্রবেশ, host_part - প্রবেশ);
      যদি(!*iface)
        ফেরত CURLE_OUT_OF_MEMORY;
    }
    ++host_part;
    যদি(host) {
      *host = curlx_memdup0(host_part, len - (host_part - প্রবেশ));
      যদি(!*host) {
        যদি(iface)
          curlx_safefree(*iface);
        ফেরত CURLE_OUT_OF_MEMORY;
      }
    }
    ফেরত CURLE_OK;
  }

  যদি(!*প্রবেশ)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  যদি(dev) {
    *dev = curlx_memdup0(প্রবেশ, len);
    ফেরত *dev ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  ফেরত CURLE_OK;
}

#ifndef CURL_DISABLE_BINDLOCAL
স্থির CURLcode bindlocal(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn,
                          curl_socket_t sockfd, পূর্ণসংখ্যা af, unsigned পূর্ণসংখ্যা scope,
                          uint8_t transport)
{
  কাঠামো Curl_sockaddr_storage sa;
  কাঠামো sockaddr *sock = (কাঠামো sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  কাঠামো sockaddr_in *si4 = (কাঠামো sockaddr_in *)&sa;
#ifdef USE_IPV6
  কাঠামো sockaddr_in6 *si6 = (কাঠামো sockaddr_in6 *)&sa;
#endif
  কাঠামো Curl_dns_entry *h = ফাঁকা;
  unsigned short port = তথ্য->set.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  পূর্ণসংখ্যা portnum = তথ্য->set.localportrange;
  অক্ষর *dev_in = ফাঁকা, *iface_in = ফাঁকা, *host_in = ফাঁকা;
  ধ্রুবক অক্ষর *iface = ফাঁকা, *host = ফাঁকা;
  পূর্ণসংখ্যা sockerr;
#ifdef IP_BIND_ADDRESS_NO_PORT
  পূর্ণসংখ্যা on = 1;
#endif
  CURLcode ফলাফল = CURLE_OK;

#ifndef USE_IPV6
  (শূন্য)scope;
#endif
  যদি(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    ফলাফল = Curl_parse_interface(conn->interface_opts,
                                  &dev_in, &iface_in, &host_in);
    যদি(ফলাফল)
      যাও out;
    /* This is how we use the values */
    iface = iface_in ? iface_in : dev_in;
    host = host_in ? host_in : dev_in;
  }

  যদি(!iface && !host && !port)
    যাও out;

  যদি(iface && (strlen(iface) >= 255)) {
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }
  memset(&sa, 0, আকার(কাঠামো Curl_sockaddr_storage));

  যদি(iface || host) {
    অক্ষর myhost[256] = "";
    পূর্ণসংখ্যা done = 0; /* -1 for error, 1 for address found */
    if2ip_ফলাফল_t if2ip_ফলাফল = IF2IP_NOT_FOUND;

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
    যদি(iface &&
       setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                  iface, (curl_socklen_t)strlen(iface) + 1) == 0 &&
       /* This is often "errno 1, error: Operation not permitted" if you are
        * not running as root or another suitable privileged user. If it
        * succeeds it means the parameter was a valid interface and not an IP
        * address. Return immediately.
        */
       !host_in) {
      infof(তথ্য, "socket successfully bound to interface '%s'", iface);
      ফলাফল = CURLE_OK;
      যাও out;
    }
#endif
    যদি(!host_in) {
      /* Discover IP from input device, then bind to it */
      if2ip_ফলাফল = Curl_if2ip(af,
#ifdef USE_IPV6
                                scope, conn->scope_id,
#endif
                                iface, myhost, আকার(myhost));
    }
    নির্বাচন(if2ip_ফলাফল) {
    ক্ষেত্র IF2IP_NOT_FOUND:
      যদি(iface_in && !host_in) {
        /* Do not fall back to treating it as a hostname */
        অক্ষর বাফার[STRERROR_LEN];
        তথ্য->state.os_errno = sockerr = SOCKERRNO;
        failf(তথ্য, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, বাফার, আকার(বাফার)));
        ফলাফল = CURLE_INTERFACE_FAILED;
        যাও out;
      }
      বিরতি;
    ক্ষেত্র IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      ফলাফল = CURLE_UNSUPPORTED_PROTOCOL;
      যাও out;
    ক্ষেত্র IF2IP_FOUND:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(তথ্য, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      done = 1;
      বিরতি;
    }
    যদি(!iface_in || host_in) {
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
      যদি(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#endif

      (শূন্য)Curl_resolv_blocking(তথ্য, dns_queries, host, 80, transport, &h);
      যদি(h) {
        পূর্ণসংখ্যা h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_address(h->addr, myhost, আকার(myhost));
        infof(তথ্য, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_entry_unlink(তথ্য, &h); /* this will NULL, potential free h */
        যদি(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          ফলাফল = CURLE_UNSUPPORTED_PROTOCOL;
          যাও out;
        }
        done = 1;
      }
      অন্যথা {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        done = -1;
      }
    }

    যদি(done > 0) {
#ifdef USE_IPV6
      /* IPv6 address */
      যদি(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        অক্ষর *scope_ptr = strchr(myhost, '%');
        যদি(scope_ptr)
          *(scope_ptr++) = '\0';
#endif
        যদি(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          যদি(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            যদি(curlx_str_সংখ্যা((ধ্রুবক অক্ষর **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              ফলাফল = CURLE_UNSUPPORTED_PROTOCOL;
              যাও out;
            }
            si6->sin6_scope_id = (unsigned পূর্ণসংখ্যা)scope_id;
          }
#endif
        }
        sizeof_sa = আকার(কাঠামো sockaddr_in6);
      }
      অন্যথা
#endif
      /* IPv4 address */
      যদি((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(port);
        sizeof_sa = আকার(কাঠামো sockaddr_in);
      }
    }

    যদি(done < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      অক্ষর বাফার[STRERROR_LEN];
      তথ্য->state.errorbuf = FALSE;
      তথ্য->state.os_errno = sockerr = SOCKERRNO;
      failf(তথ্য, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, বাফার, আকার(বাফার)));
      ফলাফল = CURLE_INTERFACE_FAILED;
      যাও out;
    }
  }
  অন্যথা {
    /* no device was given, prepare sa to match af's needs */
#ifdef USE_IPV6
    যদি(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(port);
      sizeof_sa = আকার(কাঠামো sockaddr_in6);
    }
    অন্যথা
#endif
    যদি(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(port);
      sizeof_sa = আকার(কাঠামো sockaddr_in);
    }
  }
#ifdef IP_BIND_ADDRESS_NO_PORT
  (শূন্য)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, আকার(on));
#endif
  জন্য(;;) {
    যদি(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(তথ্য, "Local port: %hu", port);
      conn->bits.bound = TRUE;
      ফলাফল = CURLE_OK;
      যাও out;
    }

    যদি(--portnum > 0) {
      port++; /* try next port */
      যদি(port == 0)
        বিরতি;
      infof(তথ্য, "Bind to local port %d failed, trying next", port - 1);
      /* We reuse/clobber the port variable here below */
      যদি(sock->sa_family == AF_INET)
        si4->sin_port = htons(port);
#ifdef USE_IPV6
      অন্যথা
        si6->sin6_port = htons(port);
#endif
    }
    অন্যথা
      বিরতি;
  }
  {
    অক্ষর বাফার[STRERROR_LEN];
    তথ্য->state.os_errno = sockerr = SOCKERRNO;
    failf(তথ্য, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, বাফার, আকার(বাফার)));
  }

  ফলাফল = CURLE_INTERFACE_FAILED;

out:
  curlx_free(dev_in);
  curlx_free(iface_in);
  curlx_free(host_in);
  ফেরত ফলাফল;
}
#endif

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
স্থির bool verifyconnect(curl_socket_t sockfd, পূর্ণসংখ্যা *psockerr)
{
  bool rc = TRUE;
#ifdef SO_ERROR
  পূর্ণসংখ্যা sockerr = 0;
  curl_socklen_t err_আকার = আকার(sockerr);

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

  যদি(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (শূন্য *)&sockerr, &err_আকার))
    sockerr = SOCKERRNO;
#যদি defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  যদি(EBADIOCTL == sockerr) {
    SET_SOCKERRNO(0);
    sockerr = 0;
  }
#endif
  যদি((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  অন্যথা
    /* This was not a successful connect */
    rc = FALSE;
  যদি(psockerr)
    *psockerr = sockerr;
#অন্যথা
  (শূন্য)sockfd;
  যদি(psockerr)
    *psockerr = SOCKERRNO;
#endif
  ফেরত rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
স্থির CURLcode socket_connect_ফলাফল(কাঠামো Curl_easy *তথ্য,
                                      ধ্রুবক অক্ষর *ipaddress, পূর্ণসংখ্যা sockerr)
{
  যদি(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    ফেরত CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(অক্ষর বাফার[STRERROR_LEN]);
    infof(তথ্য, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, বাফার, আকার(বাফার)));
    NOVERBOSE((শূন্য)ipaddress);
  }
  তথ্য->state.os_errno = sockerr;
  /* connect failed */
  ফেরত CURLE_COULDNT_CONNECT;
}

কাঠামো cf_socket_ctx {
  কাঠামো Curl_peer *peer;
  কাঠামো Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  কাঠামো ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  কাঠামো curltime started_at;        /* when socket was created */
  কাঠামো curltime connected_at;      /* when socket connected/got first byte */
  কাঠামো curltime first_byte_at;     /* when first byte was recvd */
#ifdef USE_WINSOCK
  কাঠামো curltime last_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_আকার;                     /* the last set SO_SNDBUF size */
#endif
  পূর্ণসংখ্যা sockerr;                       /* socket error of last failure or 0 */
#ifdef DEBUGBUILD
  পূর্ণসংখ্যা connect_wait_ms;               /* ms to wait after 1st connect() */
  পূর্ণসংখ্যা wblock_percent;                /* percent of writes doing EAGAIN */
  পূর্ণসংখ্যা wpartial_percent;              /* percent of bytes written in send */
  পূর্ণসংখ্যা rblock_percent;                /* percent of reads doing EAGAIN */
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

স্থির CURLcode cf_socket_ctx_init(কাঠামো cf_socket_ctx *ctx,
                                   কাঠামো Curl_peer *peer,
                                   কাঠামো Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, আকার(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ifdef DEBUGBUILD
  {
    ধ্রুবক অক্ষর *p = getenv("CURL_DBG_SOCK_WBLOCK");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, 100))
        ctx->wblock_percent = (পূর্ণসংখ্যা)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, 100))
        ctx->wpartial_percent = (পূর্ণসংখ্যা)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, 100))
        ctx->rblock_percent = (পূর্ণসংখ্যা)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, INT_MAX))
        ctx->connect_wait_ms = (পূর্ণসংখ্যা)l;
    }
  }
#endif

  ফেরত CURLE_OK;
}

স্থির শূন্য cf_socket_ctx_free(কাঠামো cf_socket_ctx *ctx)
{
  যদি(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

স্থির CURLcode cf_socket_shutdown(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   bool *done)
{
  যদি(cf->connected) {
    কাঠামো cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(তথ্য, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    যদি(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      unsigned অক্ষর buf[1024];
      (শূন্য)sread(ctx->sock, buf, আকার(buf));
    }
  }
  *done = TRUE;
  ফেরত CURLE_OK;
}

স্থির শূন্য cf_socket_destroy(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(তথ্য, cf, "destroy");
  যদি(ctx) {
    যদি(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(তথ্য, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      যদি(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(তথ্য, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

স্থির শূন্য set_local_ip(কাঠামো Curl_cfilter *cf,
                         কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_port = 0;

#ifdef HAVE_GETSOCKNAME
  যদি((ctx->sock != CURL_SOCKET_BAD) &&
     !(তথ্য->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    কাঠামো Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = আকার(কাঠামো Curl_sockaddr_storage);

    memset(&ssloc, 0, আকার(ssloc));
    যদি(getsockname(ctx->sock, (কাঠামো sockaddr *)&ssloc, &slen)) {
      VERBOSE(অক্ষর বাফার[STRERROR_LEN]);
      VERBOSE(পূর্ণসংখ্যা sockerr = SOCKERRNO);
      infof(তথ্য, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, বাফার, আকার(বাফার)));
    }
    অন্যথা {
      CURLcode ফলাফল = sockaddr2string((কাঠামো sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_port);
      যদি(ফলাফল)
        infof(তথ্য, "ssloc inet_ntop() failed with %d", (পূর্ণসংখ্যা)ফলাফল);
    }
  }
#অন্যথা
  (শূন্য)তথ্য;
#endif
}

স্থির CURLcode set_remote_ip(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  CURLcode ফলাফল;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  ফলাফল = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_port);
  যদি(ফলাফল) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(তথ্য, "curl_sa_addr inet_ntop() failed with %d", (পূর্ণসংখ্যা)ফলাফল);
    ফেরত CURLE_FAILED_INIT;
  }
  ফেরত CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
স্থির পূর্ণসংখ্যা cf_socktype(পূর্ণসংখ্যা x)
{
#ifdef SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#endif
  ফেরত x;
}

#ifdef _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ifndef SIO_TCP_INITIAL_RTO
#define SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#define TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
প্রকারলেখ কাঠামো _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#endif /* SIO_TCP_INITIAL_RTO */

#ifndef TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#define TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#endif

স্থির bool targets_localhost(কাঠামো cf_socket_ctx *ctx)
{
  ফেরত (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
স্থির শূন্য tcplocalhost(কাঠামো Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  যদি(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD bytes = 0;
    memset(&rto, 0, আকার(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    যদি(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    অন্যথা
      rto.MaxSynRetransmissions = 1;
    (শূন্য)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, আকার(rto),
                   ফাঁকা, 0, &bytes, ফাঁকা, ফাঁকা);
  }
}
#অন্যথা
#define tcplocalhost(x, y)
#endif /* _WIN32 */

স্থির CURLcode cf_socket_open(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  পূর্ণসংখ্যা error = 0;
  bool isconnected = FALSE;
  CURLcode ফলাফল = CURLE_COULDNT_CONNECT;
  bool is_tcp;

  DEBUGASSERT(ctx->sock == CURL_SOCKET_BAD);
  ctx->started_at = *Curl_pgrs_now(তথ্য);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Do not tuck SOCK_NONBLOCK into socktype when opensocket callback is set
   * because we would not know how socktype is about to be used in the
   * callback, SOCK_NONBLOCK might get factored out before calling socket().
   */
  যদি(!তথ্য->set.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#endif
  ফলাফল = socket_open(তথ্য, &ctx->addr, &ctx->sock);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  যদি(!তথ্য->set.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#endif
  যদি(ফলাফল)
    যাও out;

  ফলাফল = set_remote_ip(cf, তথ্য);
  যদি(ফলাফল)
    যাও out;

#ifdef USE_IPV6
  যদি(ctx->addr.family == AF_INET6) {
#ifdef USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    পূর্ণসংখ্যা on = 0;
    (শূন্য)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (শূন্য *)&on, আকার(on));
#endif
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      কাঠামো sockaddr_in6 *sa6 = (শূন্য *)&ctx->addr.curl_sa_addr;
      যদি(sa6->sin6_scope_id)
        infof(তথ্য, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_port,
              (unsigned long)sa6->sin6_scope_id);
      অন্যথা
#endif
        infof(তথ্য, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#endif
  }
  অন্যথা
#endif
    infof(তথ্য, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_port);

#ifdef USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#অন্যথা
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#endif
  যদি(is_tcp) {
    যদি(তথ্য->set.tcp_nodelay)
      tcpnodelay(cf, তথ্য, ctx->sock);

    যদি(তথ্য->set.tcp_keepalive)
      tcpkeepalive(cf, তথ্য, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  যদি(তথ্য->set.fsockopt) {
    /* activate callback for setting socket options */
    কাঠামো Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, তথ্য, easy_fsockopt);
    error = তথ্য->set.fsockopt(তথ্য->set.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_END(&guard);

    যদি(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    অন্যথা যদি(error) {
      ফলাফল = CURLE_ABORTED_BY_CALLBACK;
      যাও out;
    }
  }

#ifndef CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  যদি(ctx->addr.family == AF_INET
#ifdef USE_IPV6
     || ctx->addr.family == AF_INET6
#endif
    ) {
    ফলাফল = bindlocal(তথ্য, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        ফলাফল = CURLE_COULDNT_CONNECT;
      }
      যাও out;
    }
  }
#endif

#ifndef CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  যদি(error < 0) {
    ফলাফল = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    যাও out;
  }
#অন্যথা
  যদি(তথ্য->set.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    যদি(error < 0) {
      ফলাফল = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      যাও out;
    }
  }
#endif
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
out:
  যদি(ফলাফল) {
    যদি(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(তথ্য, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  অন্যথা যদি(isconnected) {
    set_local_ip(cf, তথ্য);
    ctx->connected_at = *Curl_pgrs_now(তথ্য);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(তথ্য, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (পূর্ণসংখ্যা)ফলাফল, ctx->sock);
  ফেরত ফলাফল;
}

স্থির পূর্ণসংখ্যা do_connect(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                      bool is_tcp_fastopen)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
#ifdef TCP_FASTOPEN_CONNECT
  পূর্ণসংখ্যা optval = 1;
#endif
  পূর্ণসংখ্যা rc = -1;

  (শূন্য)তথ্য;
  যদি(is_tcp_fastopen) {
#ifdef CONNECT_তথ্য_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_AVAILABLE
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    যদি(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = ফাঁকা;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_তথ্য_IDEMPOTENT,
                    ফাঁকা, 0, ফাঁকা, ফাঁকা);
    }
    অন্যথা {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  অন্যথা
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#নতুবা_যদি defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    যদি(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (শূন্য *)&optval, আকার(optval)) < 0)
      CURL_TRC_CF(তথ্য, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#নতুবা_যদি defined(MSG_FASTOPEN) /* old Linux */
    যদি(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    অন্যথা
      rc = 0; /* Do nothing */
#endif
  }
  অন্যথা {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  ফেরত rc;
}

স্থির CURLcode cf_tcp_connect(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               bool *done)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_COULDNT_CONNECT;
  পূর্ণসংখ্যা rc = 0;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  *done = FALSE; /* a negative world view is best */
  যদি(ctx->sock == CURL_SOCKET_BAD) {
    পূর্ণসংখ্যা sockerr;

    ফলাফল = cf_socket_open(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;

    যদি(cf->connected) {
      *done = TRUE;
      ফেরত CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, তথ্য, (bool)cf->conn->bits.tcp_fastopen);
    sockerr = SOCKERRNO;
    set_local_ip(cf, তথ্য);
    CURL_TRC_CF(তথ্য, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_port);
#ifdef DEBUGBUILD
    যদি(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(তথ্য, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#endif
    যদি(rc == -1) {
      ctx->sockerr = sockerr;
      ফলাফল = socket_connect_ফলাফল(তথ্য, ctx->ip.remote_ip, sockerr);
      যাও out;
    }
  }

#ifdef mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (শূন্য)verifyconnect(ctx->sock, ফাঁকা);
#endif
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  যদি(rc == 0) { /* no connection yet */
    CURL_TRC_CF(তথ্য, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    ফেরত CURLE_OK;
  }
  অন্যথা যদি(rc == CURL_CSELECT_OUT ||
#ifdef __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#endif
          cf->conn->bits.tcp_fastopen) {
    যদি(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(তথ্য);
      set_local_ip(cf, তথ্য);
      *done = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(তথ্য, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      ফেরত CURLE_OK;
    }
    ফলাফল = CURLE_COULDNT_CONNECT;
  }
  অন্যথা যদি(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(তথ্য, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (শূন্য)verifyconnect(ctx->sock, &ctx->sockerr);
    ফলাফল = CURLE_COULDNT_CONNECT;
  }

out:
  যদি(ফলাফল) {
    VERBOSE(অক্ষর বাফার[STRERROR_LEN]);
    set_local_ip(cf, তথ্য);
    যদি(ctx->sockerr) {
      তথ্য->state.os_errno = ctx->sockerr;
      SET_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, বাফার, আকার(বাফার)));
    }
    অন্যথা {
      VERBOSE(curlx_strcopy(বাফার, আকার(বাফার), STRCONST("peer closed")));
    }
    যদি(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(তথ্য, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(তথ্য, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_port,
          ctx->ip.local_ip, ctx->ip.local_port,
          curlx_strerror(ctx->sockerr, বাফার, আকার(বাফার)));
    *done = FALSE;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_socket_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য,
                                         কাঠামো easy_pollset *ps)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    যদি(ctx->listening) {
      ফলাফল = Curl_pollset_set_in_only(তথ্য, ps, ctx->sock);
      CURL_TRC_CF(তথ্য, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    অন্যথা যদি(!cf->connected) {
      ফলাফল = Curl_pollset_set_out_only(তথ্য, ps, ctx->sock);
      CURL_TRC_CF(তথ্য, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    অন্যথা যদি(!ctx->active) {
      ফলাফল = Curl_pollset_add_in(তথ্য, ps, ctx->sock);
      CURL_TRC_CF(তথ্য, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  ফেরত ফলাফল;
}

#ifdef USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ifndef SIO_IDEAL_SEND_BACKLOG_QUERY
#define SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#endif

স্থির শূন্য win_update_sndbuf_আকার(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  যদি(curlx_ptimediff_ms(Curl_pgrs_now(তথ্য),
                        &ctx->last_sndbuf_query_at) > 1000) {
    যদি(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, আকার(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_আকার &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (ধ্রুবক অক্ষর *)&ideal, আকার(ideal))) {
      ctx->sndbuf_আকার = ideal;
    }
    ctx->last_sndbuf_query_at = *Curl_pgrs_now(তথ্য);
  }
}

#endif /* USE_WINSOCK */

স্থির CURLcode cf_socket_send(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                               ধ্রুবক uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode ফলাফল = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (শূন্য)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  যদি(ctx->wblock_percent > 0) {
    unsigned অক্ষর c = 0;
    Curl_rand_bytes(তথ্য, FALSE, &c, 1);
    যদি(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(তথ্য, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      ফেরত CURLE_AGAIN;
    }
  }
  যদি(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    যদি(!len)
      len = 1;
    CURL_TRC_CF(তথ্য, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#endif

#যদি defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  যদি(cf->conn->bits.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->bits.tcp_fastopen = FALSE;
  }
  অন্যথা
#endif
    rv = swrite(ctx->sock, buf, len);

  যদি(!curlx_sztouz(rv, pnwritten)) {
    পূর্ণসংখ্যা sockerr = SOCKERRNO;
    যদি(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
      ) {
      ফলাফল = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    অন্যথা {
      অক্ষর বাফার[STRERROR_LEN];
      failf(তথ্য, "Send failure: %s",
            curlx_strerror(sockerr, বাফার, আকার(বাফার)));
      তথ্য->state.os_errno = sockerr;
      ফলাফল = CURLE_SEND_ERROR;
    }
  }

#ifdef USE_WINSOCK
  যদি(!ফলাফল)
    win_update_sndbuf_আকার(তথ্য, ctx);
#endif

  CURL_TRC_CF(তথ্য, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  ফেরত ফলাফল;
}

স্থির CURLcode cf_socket_recv(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                               অক্ষর *buf, size_t len, size_t *pnread)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial reads */
  যদি(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    unsigned অক্ষর c = 0;
    Curl_rand(তথ্য, &c, 1);
    যদি(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(তথ্য, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      ফেরত CURLE_AGAIN;
    }
  }
  যদি(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(তথ্য, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#endif

  rv = sread(ctx->sock, buf, len);

  যদি(!curlx_sztouz(rv, pnread)) {
    পূর্ণসংখ্যা sockerr = SOCKERRNO;
    যদি(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR)
#endif
      ) {
      ফলাফল = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    অন্যথা {
      অক্ষর বাফার[STRERROR_LEN];
      failf(তথ্য, "Recv failure: %s",
            curlx_strerror(sockerr, বাফার, আকার(বাফার)));
      তথ্য->state.os_errno = sockerr;
      ফলাফল = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(তথ্য, cf, "recv(len=%zu) -> %d, %zu", len, (পূর্ণসংখ্যা)ফলাফল, *pnread);
  যদি(!ফলাফল && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(তথ্য);
    ctx->got_first_byte = TRUE;
  }
  ফেরত ফলাফল;
}

স্থির শূন্য cf_socket_update_তথ্য(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য)
{
  /* Update the IP info held in the transfer, if we have that. */
  যদি(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    কাঠামো cf_socket_ctx *ctx = cf->ctx;
    তথ্য->info.primary = ctx->ip;
  }
}

স্থির শূন্য cf_socket_active(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  set_local_ip(cf, তথ্য);
#ifdef USE_IPV6
  যদি(cf->sockindex == FIRSTSOCKET)
    cf->conn->bits.ipv6 = (ctx->addr.family == AF_INET6);
#endif
  ctx->active = TRUE;
}

স্থির CURLcode cf_socket_cntrl(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;

  (শূন্য)arg1;
  (শূন্য)arg2;
  নির্বাচন(event) {
  ক্ষেত্র CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_active(cf, তথ্য);
    cf_socket_update_তথ্য(cf, তথ্য);
    বিরতি;
  ক্ষেত্র CF_CTRL_তথ্য_SETUP:
    cf_socket_update_তথ্য(cf, তথ্য);
    বিরতি;
  ক্ষেত্র CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    বিরতি;
  ক্ষেত্র CF_CTRL_REPORT_STATS:
    যদি(cf->connected && !ctx->stats_reported) {
      কাঠামো curltime *ts = ফাঁকা;
      নির্বাচন(ctx->transport) {
      ক্ষেত্র TRNSPRT_UDP:
      ক্ষেত্র TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        যদি(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        বিরতি;
      পূর্বনির্ধারিত:
        ts = &ctx->connected_at;
        বিরতি;
      }
      যদি(ts) {
        Curl_pgrsTimeWas(তথ্য, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  ফেরত CURLE_OK;
}

স্থির bool cf_socket_conn_is_alive(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    bool *প্রবেশ_pending)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  কাঠামো pollfd pfd[1];
  পূর্ণসংখ্যা r;

  *প্রবেশ_pending = FALSE;

  যদি(!ctx || ctx->sock == CURL_SOCKET_BAD)
    ফেরত FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  যদি(r < 0) {
    CURL_TRC_CF(তথ্য, cf, "is_alive: poll error, assume dead");
    ফেরত FALSE;
  }
  অন্যথা যদি(r == 0) {
    CURL_TRC_CF(তথ্য, cf, "is_alive: poll timeout, assume alive");
    ফেরত TRUE;
  }
  অন্যথা যদি(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(তথ্য, cf, "is_alive: err/hup/etc events, assume dead");
    ফেরত FALSE;
  }

  CURL_TRC_CF(তথ্য, cf, "is_alive: valid events, looks alive");
  *প্রবেশ_pending = TRUE;
  ফেরত TRUE;
}

স্থির CURLcode cf_socket_query(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;

  নির্বাচন(query) {
  ক্ষেত্র CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((ধ্রুবক কাঠামো Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : ফাঁকা;
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_CONNECT_REPLY_MS:
    যদি(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (পূর্ণসংখ্যা)ms : INT_MAX;
    }
    অন্যথা
      *pres1 = -1;
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_IP_INFO:
#ifdef USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#অন্যথা
    *pres1 = FALSE;
#endif
    *(কাঠামো ip_quadruple *)pres2 = ctx->ip;
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_REALLY_CONNECTED:
    যদি(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    অন্যথা
      *pres1 = ctx->got_first_byte;
    ফেরত CURLE_OK;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

কাঠামো Curl_cftype Curl_cft_tcp = {
  "TCP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_তথ্য_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_tcp_create(কাঠামো Curl_cfilter **pcf,
                            কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_peer *origin,
                            কাঠামো Curl_peer *peer,
                            uint8_t transport_peer,
                            কাঠামো connectdata *conn,
                            কাঠামো Curl_sockaddr_ex *addr,
                            কাঠামো Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  কাঠামো cf_socket_ctx *ctx = ফাঁকা;
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  CURLcode ফলাফল;

  (শূন্য)তথ্য;
  (শূন্য)origin;
  (শূন্য)conn;
  (শূন্য)tunnel_peer;
  (শূন্য)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  যদি(!addr) {
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }

  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  ফলাফল = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

out:
  *pcf = (!ফলাফল) ? cf : ফাঁকা;
  যদি(ফলাফল) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ফেরত ফলাফল;
}

#ifdef __linux__
স্থির শূন্য linux_quic_mtu(কাঠামো cf_socket_ctx *ctx)
{
  পূর্ণসংখ্যা val;
  নির্বাচন(ctx->addr.family) {
#ifdef IP_MTU_DISCOVER
  ক্ষেত্র AF_INET:
    val = IP_PMTUDISC_DO;
    (শূন্য)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     আকার(val));
    বিরতি;
#endif
#ifdef IPV6_MTU_DISCOVER
  ক্ষেত্র AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (শূন্য)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     আকার(val));
    বিরতি;
#endif
  }
}
#অন্যথা
#define linux_quic_mtu(x)
#endif

#যদি defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
স্থির শূন্য linux_quic_gro(কাঠামো cf_socket_ctx *ctx)
{
  পূর্ণসংখ্যা one = 1;
  (শূন্য)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)আকার(one));
}
#অন্যথা
#define linux_quic_gro(x)
#endif

#যদি (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
স্থির শূন্য linux_quic_ecn(কাঠামো cf_socket_ctx *ctx)
{
  unsigned পূর্ণসংখ্যা tos = 1;
  নির্বাচন(ctx->addr.family) {
  ক্ষেত্র AF_INET:
    (শূন্য)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, আকার(tos));
    বিরতি;
#ifdef IPV6_RECVTCLASS
  ক্ষেত্র AF_INET6:
    (শূন্য)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, আকার(tos));
    বিরতি;
#endif
  }
}
#অন্যথা
#define linux_quic_ecn(x)
#endif

#যদি (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
স্থির শূন্য linux_ip_dontfrag(কাঠামো cf_socket_ctx *ctx)
{
  পূর্ণসংখ্যা val = 1;

  নির্বাচন(ctx->addr.family) {
  ক্ষেত্র AF_INET:
    (শূন্য)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, আকার(val));
    বিরতি;
#ifdef IPV6_DONTFRAG
  ক্ষেত্র AF_INET6:
    (শূন্য)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, আকার(val));
    বিরতি;
#endif
  }
}
#অন্যথা
#define linux_ip_dontfrag(x)
#endif

স্থির CURLcode cf_udp_setup_quic(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  পূর্ণসংখ্যা rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  যদি(rc == -1) {
    ফেরত socket_connect_ফলাফল(তথ্য, ctx->ip.remote_ip, SOCKERRNO);
  }
  ctx->sock_connected = TRUE;
  set_local_ip(cf, তথ্য);
  CURL_TRC_CF(তথ্য, cf, "%s socket %" FMT_SOCKET_T
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

  ফেরত CURLE_OK;
}

স্থির CURLcode cf_udp_connect(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               bool *done)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_COULDNT_CONNECT;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  *done = FALSE;
  যদি(ctx->sock == CURL_SOCKET_BAD) {
    ফলাফল = cf_socket_open(cf, তথ্য);
    যদি(ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "cf_udp_connect(), open failed -> %d",
                  (পূর্ণসংখ্যা)ফলাফল);
      যাও out;
    }

    যদি(ctx->transport == TRNSPRT_QUIC) {
      ফলাফল = cf_udp_setup_quic(cf, তথ্য);
      যদি(ফলাফল)
        যাও out;
      CURL_TRC_CF(তথ্য, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
    }
    *done = TRUE;
    cf->connected = TRUE;
  }
out:
  ফেরত ফলাফল;
}

কাঠামো Curl_cftype Curl_cft_udp = {
  "UDP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_udp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_তথ্য_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_udp_create(কাঠামো Curl_cfilter **pcf,
                            কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_peer *origin,
                            কাঠামো Curl_peer *peer,
                            uint8_t transport_peer,
                            কাঠামো connectdata *conn,
                            কাঠামো Curl_sockaddr_ex *addr,
                            কাঠামো Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  কাঠামো cf_socket_ctx *ctx = ফাঁকা;
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  CURLcode ফলাফল;

  (শূন্য)তথ্য;
  (শূন্য)origin;
  (শূন্য)conn;
  (শূন্য)tunnel_peer;
  (শূন্য)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  ফলাফল = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

out:
  *pcf = (!ফলাফল) ? cf : ফাঁকা;
  যদি(ফলাফল) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ফেরত ফলাফল;
}

/* this is the TCP filter which can also handle this case */
কাঠামো Curl_cftype Curl_cft_unix = {
  "UNIX",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_তথ্য_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_unix_create(কাঠামো Curl_cfilter **pcf,
                            কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_peer *origin,
                            কাঠামো Curl_peer *peer,
                            uint8_t transport_peer,
                            কাঠামো connectdata *conn,
                            কাঠামো Curl_sockaddr_ex *addr,
                            কাঠামো Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  কাঠামো cf_socket_ctx *ctx = ফাঁকা;
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  CURLcode ফলাফল;

  (শূন্য)তথ্য;
  (শূন্য)origin;
  (শূন্য)conn;
  (শূন্য)tunnel_peer;
  (শূন্য)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  ফলাফল = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

out:
  *pcf = (!ফলাফল) ? cf : ফাঁকা;
  যদি(ফলাফল) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ফেরত ফলাফল;
}

স্থির timediff_t cf_tcp_accept_timeleft(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ifndef CURL_DISABLE_FTP
  যদি(তথ্য->set.accepttimeout > 0)
    timeout_ms = তথ্য->set.accepttimeout;
#endif

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(তথ্য);
  যদি(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  অন্যথা {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &ctx->started_at);
    যদি(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  ফেরত timeout_ms;
}

স্থির শূন্য cf_tcp_set_accepted_remote_ip(কাঠামো Curl_cfilter *cf,
                                          কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
#ifdef HAVE_GETPEERNAME
  CURLcode ফলাফল;
  কাঠামো Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  plen = আকার(ssrem);
  memset(&ssrem, 0, plen);
  যদি(getpeername(ctx->sock, (কাঠামো sockaddr *)&ssrem, &plen)) {
    অক্ষর বাফার[STRERROR_LEN];
    পূর্ণসংখ্যা sockerr = SOCKERRNO;
    failf(তথ্য, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, বাফার, আকার(বাফার)));
    ফেরত;
  }
  ফলাফল = sockaddr2string((কাঠামো sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_port);
  যদি(ফলাফল) {
    failf(তথ্য, "ssrem inet_ntop() failed with %d", (পূর্ণসংখ্যা)ফলাফল);
    ফেরত;
  }
#অন্যথা
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  (শূন্য)তথ্য;
#endif
}

স্থির CURLcode cf_tcp_accept_connect(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      bool *done)
{
  কাঠামো cf_socket_ctx *ctx = cf->ctx;
  অক্ষর errbuf[STRERROR_LEN];
#ifdef USE_IPV6
  কাঠামো Curl_sockaddr_storage add;
#অন্যথা
  কাঠামো sockaddr_in add;
#endif
  curl_socklen_t আকার_2 = (curl_socklen_t)আকার(add);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  পূর্ণসংখ্যা socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (শূন্য)তথ্য;
  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  *done = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, তথ্য);
  যদি(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(তথ্য, "Accept timeout occurred while waiting server connect");
    ফেরত CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(তথ্য, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(তথ্য, cf, "socket_check -> %x", (unsigned পূর্ণসংখ্যা)socketstate);
  নির্বাচন(socketstate) {
  ক্ষেত্র -1: /* error */
    /* let's die here */
    failf(তথ্য, "Error while waiting for server connect");
    ফেরত CURLE_FTP_ACCEPT_FAILED;
  পূর্বনির্ধারিত:
    যদি(socketstate & CURL_CSELECT_IN) {
      infof(তথ্য, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    বিরতি;
  }

  যদি(!incoming) {
    CURL_TRC_CF(তথ্য, cf, "nothing heard from the server yet");
    ফেরত CURLE_OK;
  }

  আকার_2 = আকার(add);
#ifdef HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (কাঠামো sockaddr *)&add, &আকার_2,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#অন্যথা
  s_accepted = CURL_ACCEPT(ctx->sock, (কাঠামো sockaddr *)&add, &আকার_2);
#endif

  যদি(s_accepted == CURL_SOCKET_BAD) {
    failf(তথ্য, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, আকার(errbuf)));
    ফেরত CURLE_FTP_ACCEPT_FAILED;
  }
#ifndef HAVE_ACCEPT4
#ifdef HAVE_FCNTL
  যদি(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(তথ্য, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, আকার(errbuf)));
    Curl_socket_close(তথ্য, cf->conn, s_accepted);
    ফেরত CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* HAVE_FCNTL */
  যদি(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(তথ্য, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, আকার(errbuf)));
    Curl_socket_close(তথ্য, cf->conn, s_accepted);
    ফেরত CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* !HAVE_ACCEPT4 */
  infof(তথ্য, "Connection accepted from server");

  /* Replace any filter on SECONDARY with one listening on this socket */
  ctx->listening = FALSE;
  ctx->accepted = TRUE;
  socket_close(তথ্য, cf->conn, TRUE, ctx->sock);
  ctx->sock = s_accepted;

  cf->conn->sock[cf->sockindex] = ctx->sock;
  cf_tcp_set_accepted_remote_ip(cf, তথ্য);
  set_local_ip(cf, তথ্য);
  ctx->active = TRUE;
  ctx->connected_at = *Curl_pgrs_now(তথ্য);
  cf->connected = TRUE;
  CURL_TRC_CF(তথ্য, cf, "accepted_set(sock=%" FMT_SOCKET_T
              ", remote=%s port=%d)",
              ctx->sock, ctx->ip.remote_ip, ctx->ip.remote_port);

  যদি(তথ্য->set.fsockopt) {
    কাঠামো Curl_mapi_guard guard;
    পূর্ণসংখ্যা error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, তথ্য, easy_fsockopt);
    error = তথ্য->set.fsockopt(তথ্য->set.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_END(&guard);

    যদি(error)
      ফেরত CURLE_ABORTED_BY_CALLBACK;
  }
  *done = TRUE;
  ফেরত CURLE_OK;
}

কাঠামো Curl_cftype Curl_cft_tcp_accept = {
  "TCP-ACCEPT",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_accept_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_তথ্য_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_conn_tcp_listen_set(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode ফলাফল;
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো cf_socket_ctx *ctx = ফাঁকা;

  /* replace any existing */
  Curl_conn_cf_discard_all(তথ্য, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  ফলাফল = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  যদি(ফলাফল)
    যাও out;
  Curl_conn_cf_add(তথ্য, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(তথ্য);
  conn->sock[sockindex] = ctx->sock;
  set_local_ip(cf, তথ্য);
  CURL_TRC_CF(তথ্য, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_port);

out:
  যদি(ফলাফল) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  ফেরত ফলাফল;
}

bool Curl_conn_is_tcp_listen(কাঠামো Curl_easy *তথ্য,
                             int8_t sockindex)
{
  কাঠামো Curl_cfilter *cf = তথ্য->conn->cfilter[sockindex];
  যখন(cf) {
    যদি(cf->cft == &Curl_cft_tcp_accept)
      ফেরত TRUE;
    cf = cf->পরবর্তী;
  }
  ফেরত FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
স্থির bool cf_is_socket(কাঠামো Curl_cfilter *cf)
{
  ফেরত cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             curl_socket_t *psock,
                             ধ্রুবক কাঠামো Curl_sockaddr_ex **paddr,
                             কাঠামো ip_quadruple *pip)
{
  (শূন্য)তথ্য;
  যদি(cf_is_socket(cf) && cf->ctx) {
    কাঠামো cf_socket_ctx *ctx = cf->ctx;

    যদি(psock)
      *psock = ctx->sock;
    যদি(paddr)
      *paddr = &ctx->addr;
    যদি(pip)
      *pip = ctx->ip;
    ফেরত CURLE_OK;
  }
  ফেরত CURLE_FAILED_INIT;
}
