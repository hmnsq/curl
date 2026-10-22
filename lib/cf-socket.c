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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h> /* <netinet/tcp.h> may need it */
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_UN_H
#అన్తర్భావ <sys/un.h> /* for sockaddr_un */
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_LINUX_TCP_H
#అన్తర్భావ <linux/tcp.h>
#లేదావేళ defined(HAVE_NETINET_TCP_H)
#అన్తర్భావ <netinet/tcp.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETINET_UDP_H
#అన్తర్భావ <netinet/udp.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETINET_IP_H
#అన్తర్భావ <netinet/ip.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_IFADDRS_H
#అన్తర్భావ <ifaddrs.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NET_IF_H
#అన్తర్భావ <net/if.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "if2ip.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-socket.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "curlx/inet_ntop.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "conncache.h"
#అన్తర్భావ "multihandle.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "sockaddr.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/nonblock.h"
#అన్తర్భావ "curlx/strcopy.h"
#అన్తర్భావ "curlx/version_win32.h"
#అన్తర్భావ "curlx/strerr.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curlx/wait.h"
#నిర్వచితమైతే _WIN32
#అన్తర్భావ <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#అయితే_ముగింపు

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(నిర్మాణం sockaddr *sa, curl_socklen_t salen,
                                  అక్షరం *addr, uint16_t *పోర్టు);
UNITTEST CURLcode sockaddr2string(నిర్మాణం sockaddr *sa, curl_socklen_t salen,
                                  అక్షరం *addr, uint16_t *పోర్టు)
{
  CURLcode ఫలితం;
  నిర్మాణం sockaddr_in *si = ఖాళీ;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం sockaddr_in6 *si6 = ఖాళీ;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_UNIX_SOCKETS
  నిర్మాణం sockaddr_un *su = ఖాళీ;
#లేకపోతే
  (శూన్య)salen;
#అయితే_ముగింపు

  ఎంపిక(sa->sa_family) {
  స్థితి AF_INET:
    si = (నిర్మాణం sockaddr_in *)(శూన్య *)sa;
    ఫలితం = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    ఒకవేళ(!ఫలితం) {
      *పోర్టు = ntohs(si->sin_port);
      తిరిగి ఫలితం;
    }
    ఆపు;
#నిర్వచితమైతే USE_IPV6
  స్థితి AF_INET6:
    si6 = (నిర్మాణం sockaddr_in6 *)(శూన్య *)sa;
    ఫలితం = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    ఒకవేళ(!ఫలితం) {
      *పోర్టు = ntohs(si6->sin6_port);
      తిరిగి ఫలితం;
    }
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_UNIX_SOCKETS
  స్థితి AF_UNIX:
    ఒకవేళ(salen > (curl_socklen_t)పరిమాణం(CURL_SA_FAMILY_T)) {
      su = (నిర్మాణం sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_మార్గం);
    }
    లేకపోతే
      addr[0] = 0; /* socket with no name */
    *పోర్టు = 0;
    తిరిగి CURLE_OK;
#అయితే_ముగింపు
  అప్రమేయ:
    ఫలితం = CURLE_UNSUPPORTED_PROTOCOL;
    ఆపు;
  }

  addr[0] = '\0';
  *పోర్టు = 0;
  తిరిగి ఫలితం;
}

స్థిర శూన్య tcpnodelay(నిర్మాణం Curl_cfilter *cf,
                       నిర్మాణం Curl_easy *దత్తాంశం,
                       curl_socket_t sockfd)
{
#ఒకవేళ defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  పూర్ణాంకం level = IPPROTO_TCP;
  VERBOSE(అక్షరం బఫరు[STRERROR_LEN]);

  ఒకవేళ(setsockopt(sockfd, level, TCP_NODELAY, (శూన్య *)&onoff, పరిమాణం(onoff)) < 0)
    CURL_TRC_CF(దత్తాంశం, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, బఫరు, పరిమాణం(బఫరు)));
#లేకపోతే
  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
  (శూన్య)sockfd;
#అయితే_ముగింపు
}

#ఒకవేళ defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_పరిమితి) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_పరిమితి)
#ఒకవేళ defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
స్థిర శూన్య alive_unit(పూర్ణాంకం *x)
{
  /* make sure this doesn't wrap */
  ఒకవేళ(*x < (గరిష్ఠ_పూర్ణాంకం / 1000))
    *x *= 1000;
  లేకపోతే
    /* this is still almost 25 days */
    *x = గరిష్ఠ_పూర్ణాంకం;
}
#లేకపోతే
#నిర్వచించు alive_unit(x)
#అయితే_ముగింపు
#అయితే_ముగింపు

స్థిర శూన్య tcpkeepalive(నిర్మాణం Curl_cfilter *cf,
                         నిర్మాణం Curl_easy *దత్తాంశం,
                         curl_socket_t sockfd)
{
  పూర్ణాంకం optval = దత్తాంశం->సమితి.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  ఒకవేళ(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (శూన్య *)&optval, పరిమాణం(optval)) < 0) {
    CURL_TRC_CF(దత్తాంశం, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  లేకపోతే {
#నిర్వచితమైతే USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    ఒకవేళ(curlx_verify_windows_సంస్కరణ(10, 0, 16299, PLATFORM_WINNT,
                                    సంస్కరణ_GREATER_THAN_EQUAL)) {
      CURL_TRC_CF(దత్తాంశం, cf, "Set TCP_KEEP* on fd=%" FMT_SOCKET_T, sockfd);
      optval = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepidle);
/* Offered by mingw-w64 v12+, MS SDK 6.0A/VS2008+ */
#నిర్వచితం_కాకపోతే TCP_KEEPALIVE
#నిర్వచించు TCP_KEEPALIVE 3
#అయితే_ముగింపు
/* Offered by mingw-w64 v12+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#నిర్వచితం_కాకపోతే TCP_KEEPCNT
#నిర్వచించు TCP_KEEPCNT 16
#అయితే_ముగింపు
/* Offered by mingw-w64 v12+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#నిర్వచితం_కాకపోతే TCP_KEEPIDLE
#నిర్వచించు TCP_KEEPIDLE TCP_KEEPALIVE
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే TCP_KEEPINTVL
#నిర్వచించు TCP_KEEPINTVL 17
#అయితే_ముగింపు
      ఒకవేళ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (స్థిరరాశి అక్షరం *)&optval, పరిమాణం(optval)) < 0) {
        CURL_TRC_CF(దత్తాంశం, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepintvl);
      ఒకవేళ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (స్థిరరాశి అక్షరం *)&optval, పరిమాణం(optval)) < 0) {
        CURL_TRC_CF(దత్తాంశం, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepcnt);
      ఒకవేళ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (స్థిరరాశి అక్షరం *)&optval, పరిమాణం(optval)) < 0) {
        CURL_TRC_CF(దత్తాంశం, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    లేకపోతే {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#నిర్వచితం_కాకపోతే SIO_KEEPALIVE_VALS
#నిర్వచించు SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      నిర్మాణం tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#అయితే_ముగింపు
      నిర్మాణం tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      ఒకవేళ(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, పరిమాణం(vals),
                  ఖాళీ, 0, &dummy, ఖాళీ, ఖాళీ) != 0) {
        CURL_TRC_CF(దత్తాంశం, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#లేకపోతే /* !USE_WINSOCK */
#నిర్వచితమైతే TCP_KEEPIDLE
    optval = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepidle);
    alive_unit(&optval);
    ఒకవేళ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (శూన్య *)&optval, పరిమాణం(optval)) < 0) {
      CURL_TRC_CF(దత్తాంశం, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#లేదావేళ defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepidle);
    alive_unit(&optval);
    ఒకవేళ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (శూన్య *)&optval, పరిమాణం(optval)) < 0) {
      CURL_TRC_CF(దత్తాంశం, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#లేదావేళ defined(TCP_KEEPALIVE_పరిమితి)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepidle);
    alive_unit(&optval);
    ఒకవేళ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_పరిమితి,
                  (శూన్య *)&optval, పరిమాణం(optval)) < 0) {
      CURL_TRC_CF(దత్తాంశం, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#అయితే_ముగింపు
#నిర్వచితమైతే TCP_KEEPINTVL
    optval = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepintvl);
    alive_unit(&optval);
    ఒకవేళ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (శూన్య *)&optval, పరిమాణం(optval)) < 0) {
      CURL_TRC_CF(దత్తాంశం, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#లేదావేళ defined(TCP_KEEPALIVE_ABORT_పరిమితి)
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
      పూర్ణాంకం keepcnt = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepcnt);
      పూర్ణాంకం keepintvl = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepintvl);

      ఒకవేళ(keepcnt > 0 && keepintvl > (గరిష్ఠ_పూర్ణాంకం / keepcnt))
        optval = గరిష్ఠ_పూర్ణాంకం;
      లేకపోతే
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    ఒకవేళ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_పరిమితి,
                  (శూన్య *)&optval, పరిమాణం(optval)) < 0) {
      CURL_TRC_CF(దత్తాంశం, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#అయితే_ముగింపు
#నిర్వచితమైతే TCP_KEEPCNT
    optval = curlx_sltosi(దత్తాంశం->సమితి.tcp_keepcnt);
    ఒకవేళ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (శూన్య *)&optval, పరిమాణం(optval)) < 0) {
      CURL_TRC_CF(దత్తాంశం, cf, "Failed to set TCP_KEEPCNT on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#అయితే_ముగింపు
#అయితే_ముగింపు /* USE_WINSOCK */
  }
}

/**
 * Assign the addrinfo `ai` to the Curl_sockaddr_ex `addr` with
 * transport determining socktype and protocol.
 */
CURLcode Curl_socket_addr_from_ai(నిర్మాణం Curl_sockaddr_ex *addr,
                                  స్థిరరాశి నిర్మాణం Curl_addrinfo *ai,
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
  addr->addrlen = (గుర్తులేని పూర్ణాంకం)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= పరిమాణం(addr->curl_sa_addrbuf));
  ఒకవేళ(addr->addrlen > పరిమాణం(addr->curl_sa_addrbuf))
    తిరిగి CURLE_TOO_LARGE;

  జ్ఞాపకం_నకలు(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే USE_SO_NOSIGPIPE
పూర్ణాంకం Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  పూర్ణాంకం onoff = 1;
  తిరిగి setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (శూన్య *)&onoff, పరిమాణం(onoff));
}
#అయితే_ముగింపు /* USE_SO_NOSIGPIPE */

#ఒకవేళ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
స్థిర uint32_t get_scope_id(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  ఒకవేళ(దత్తాంశం->conn->scope_id)
    తిరిగి దత్తాంశం->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  ఒకవేళ(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#ఒకవేళ defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    నిర్మాణం ifaddrs *ifa, *ifa_list;
    ఒకవేళ(getifaddrs(&ifa_list) == 0) {
      కొరకు(ifa = ifa_list; ifa; ifa = ifa->ifa_తదుపరి) {
        ఒకవేళ(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          నిర్మాణం sockaddr_in6 *s6 = (శూన్య *)ifa->ifa_addr;
          ఒకవేళ(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(దత్తాంశం,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (గుర్తులేని దీర్ఘ)scope_id);
            ఆపు;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#అయితే_ముగింపు /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  తిరిగి scope_id;
}
#అయితే_ముగింపు

స్థిర CURLcode socket_open(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  అక్షరం errbuf[STRERROR_LEN];

#నిర్వచితమైతే SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#అయితే_ముగింపు

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);
  ఒకవేళ(దత్తాంశం->సమితి.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    నిర్మాణం Curl_mapi_guard guard;
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fopensocket);
    *sockfd = దత్తాంశం->సమితి.fopensocket(దత్తాంశం->సమితి.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (నిర్మాణం curl_sockaddr *)addr);
    CURL_CBAPI_చివర(&guard);
  }
  లేకపోతే {
    /* opensocket callback not set, so create the socket now */
#నిర్వచితమైతే DEBUGBUILD
    ఒకవేళ((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(దత్తాంశం, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      తిరిగి CURLE_COULDNT_CONNECT;
    }
#అయితే_ముగింపు
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    ఒకవేళ((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(దత్తాంశం, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, పరిమాణం(errbuf)));
    తిరిగి CURLE_COULDNT_CONNECT;
  }

#నిర్వచితమైతే USE_SO_NOSIGPIPE
  ఒకవేళ(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(దత్తాంశం, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, పరిమాణం(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    తిరిగి CURLE_COULDNT_CONNECT;
  }
#అయితే_ముగింపు /* USE_SO_NOSIGPIPE */

#ఒకవేళ defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  ఒకవేళ(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(దత్తాంశం, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, పరిమాణం(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    తిరిగి CURLE_COULDNT_CONNECT;
  }
#అయితే_ముగింపు

#ఒకవేళ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  ఒకవేళ(addr->family == AF_INET6) {
    నిర్మాణం sockaddr_in6 * స్థిరరాశి sa6 = (శూన్య *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(దత్తాంశం, sa6);
  }
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
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
CURLcode Curl_socket_open(నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి నిర్మాణం Curl_addrinfo *ai,
                          నిర్మాణం Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  నిర్మాణం Curl_sockaddr_ex dummy;
  CURLcode ఫలితం;

  ఒకవేళ(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  ఫలితం = Curl_socket_addr_from_ai(addr, ai, transport);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  తిరిగి socket_open(దత్తాంశం, addr, sockfd);
}

స్థిర పూర్ణాంకం socket_close(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn,
                        పూర్ణాంకం use_callback, curl_socket_t sock)
{
  ఒకవేళ(sock == CURL_SOCKET_BAD)
    తిరిగి 0;

  ఒకవేళ(use_callback && conn && conn->fclosesocket) {
    నిర్మాణం Curl_mapi_guard guard;
    పూర్ణాంకం rc;
    Curl_multi_will_close(దత్తాంశం, sock);
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_చివర(&guard);
    తిరిగి rc;
  }

  ఒకవేళ(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(దత్తాంశం, sock);

  sclose(sock);

  తిరిగి 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
పూర్ణాంకం Curl_socket_close(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn,
                      curl_socket_t sock)
{
  తిరిగి socket_close(దత్తాంశం, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(స్థిరరాశి అక్షరం *ఆదానం,
                              అక్షరం **dev, అక్షరం **iface, అక్షరం **host)
{
  స్థిర స్థిరరాశి అక్షరం if_ఉపసర్గ[] = "if!";
  స్థిర స్థిరరాశి అక్షరం host_ఉపసర్గ[] = "host!";
  స్థిర స్థిరరాశి అక్షరం if_host_ఉపసర్గ[] = "ifhost!";
  size_t len;

  len = అక్షరశ్రేణి_పొడవు(ఆదానం);
  ఒకవేళ(len > 512)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(dev)
    *dev = ఖాళీ;
  ఒకవేళ(iface)
    *iface = ఖాళీ;
  ఒకవేళ(host)
    *host = ఖాళీ;

  ఒకవేళ(!అక్షరశ్రేణి_పోలిక(if_ఉపసర్గ, ఆదానం, అక్షరశ్రేణి_పొడవు(if_ఉపసర్గ))) {
    ఆదానం += అక్షరశ్రేణి_పొడవు(if_ఉపసర్గ);
    ఒకవేళ(!*ఆదానం)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    ఒకవేళ(iface) {
      *iface = curlx_memdup0(ఆదానం, len - అక్షరశ్రేణి_పొడవు(if_ఉపసర్గ));
      తిరిగి *iface ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    తిరిగి CURLE_OK;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(host_ఉపసర్గ, ఆదానం, అక్షరశ్రేణి_పొడవు(host_ఉపసర్గ))) {
    ఆదానం += అక్షరశ్రేణి_పొడవు(host_ఉపసర్గ);
    ఒకవేళ(!*ఆదానం)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    ఒకవేళ(host) {
      *host = curlx_memdup0(ఆదానం, len - అక్షరశ్రేణి_పొడవు(host_ఉపసర్గ));
      తిరిగి *host ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    తిరిగి CURLE_OK;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(if_host_ఉపసర్గ, ఆదానం, అక్షరశ్రేణి_పొడవు(if_host_ఉపసర్గ))) {
    స్థిరరాశి అక్షరం *host_part;
    ఆదానం += అక్షరశ్రేణి_పొడవు(if_host_ఉపసర్గ);
    len -= అక్షరశ్రేణి_పొడవు(if_host_ఉపసర్గ);
    host_part = memchr(ఆదానం, '!', len);
    ఒకవేళ(!host_part || !*(host_part + 1))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    ఒకవేళ(iface) {
      *iface = curlx_memdup0(ఆదానం, host_part - ఆదానం);
      ఒకవేళ(!*iface)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    ++host_part;
    ఒకవేళ(host) {
      *host = curlx_memdup0(host_part, len - (host_part - ఆదానం));
      ఒకవేళ(!*host) {
        ఒకవేళ(iface)
          curlx_safefree(*iface);
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
    }
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(!*ఆదానం)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  ఒకవేళ(dev) {
    *dev = curlx_memdup0(ఆదానం, len);
    తిరిగి *dev ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  తిరిగి CURLE_OK;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_BINDLOCAL
స్థిర CURLcode bindlocal(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn,
                          curl_socket_t sockfd, పూర్ణాంకం af, గుర్తులేని పూర్ణాంకం scope,
                          uint8_t transport)
{
  నిర్మాణం Curl_sockaddr_storage sa;
  నిర్మాణం sockaddr *sock = (నిర్మాణం sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  నిర్మాణం sockaddr_in *si4 = (నిర్మాణం sockaddr_in *)&sa;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం sockaddr_in6 *si6 = (నిర్మాణం sockaddr_in6 *)&sa;
#అయితే_ముగింపు
  నిర్మాణం Curl_dns_నమోదు *h = ఖాళీ;
  గుర్తులేని పొట్టి పోర్టు = దత్తాంశం->సమితి.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  పూర్ణాంకం portnum = దత్తాంశం->సమితి.localportrange;
  అక్షరం *dev_లోపల = ఖాళీ, *iface_లోపల = ఖాళీ, *host_లోపల = ఖాళీ;
  స్థిరరాశి అక్షరం *iface = ఖాళీ, *host = ఖాళీ;
  పూర్ణాంకం sockerr;
#నిర్వచితమైతే IP_BIND_ADDRESS_NO_PORT
  పూర్ణాంకం on = 1;
#అయితే_ముగింపు
  CURLcode ఫలితం = CURLE_OK;

#నిర్వచితం_కాకపోతే USE_IPV6
  (శూన్య)scope;
#అయితే_ముగింపు
  ఒకవేళ(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    ఫలితం = Curl_parse_interface(conn->interface_opts,
                                  &dev_లోపల, &iface_లోపల, &host_లోపల);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    /* This is how we use the values */
    iface = iface_లోపల ? iface_లోపల : dev_లోపల;
    host = host_లోపల ? host_లోపల : dev_లోపల;
  }

  ఒకవేళ(!iface && !host && !పోర్టు)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(iface && (అక్షరశ్రేణి_పొడవు(iface) >= 255)) {
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }
  memset(&sa, 0, పరిమాణం(నిర్మాణం Curl_sockaddr_storage));

  ఒకవేళ(iface || host) {
    అక్షరం myhost[256] = "";
    పూర్ణాంకం పూర్తి = 0; /* -1 for error, 1 for address found */
    if2ip_ఫలితం_t if2ip_ఫలితం = IF2_IP_NOT_దొరికింది;

#నిర్వచితమైతే SO_BINDTODEVICE
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
    ఒకవేళ(iface &&
       setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                  iface, (curl_socklen_t)అక్షరశ్రేణి_పొడవు(iface) + 1) == 0 &&
       /* This is often "errno 1, error: Operation not permitted" if you are
        * not running as root or another suitable privileged user. If it
        * succeeds it means the parameter was a valid interface and not an IP
        * address. Return immediately.
        */
       !host_లోపల) {
      infof(దత్తాంశం, "socket successfully bound to interface '%s'", iface);
      ఫలితం = CURLE_OK;
      వెళ్ళు బహిర్గతం;
    }
#అయితే_ముగింపు
    ఒకవేళ(!host_లోపల) {
      /* Discover IP from input device, then bind to it */
      if2ip_ఫలితం = Curl_if2ip(af,
#నిర్వచితమైతే USE_IPV6
                                scope, conn->scope_id,
#అయితే_ముగింపు
                                iface, myhost, పరిమాణం(myhost));
    }
    ఎంపిక(if2ip_ఫలితం) {
    స్థితి IF2_IP_NOT_దొరికింది:
      ఒకవేళ(iface_లోపల && !host_లోపల) {
        /* Do not fall back to treating it as a hostname */
        అక్షరం బఫరు[STRERROR_LEN];
        దత్తాంశం->state.os_errno = sockerr = SOCKERRNO;
        failf(దత్తాంశం, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));
        ఫలితం = CURLE_INTERFACE_FAILED;
        వెళ్ళు బహిర్గతం;
      }
      ఆపు;
    స్థితి IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      ఫలితం = CURLE_UNSUPPORTED_PROTOCOL;
      వెళ్ళు బహిర్గతం;
    స్థితి IF2_IP_దొరికింది:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(దత్తాంశం, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      పూర్తి = 1;
      ఆపు;
    }
    ఒకవేళ(!iface_లోపల || host_లోపల) {
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
#నిర్వచితమైతే USE_IPV6
      ఒకవేళ(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#అయితే_ముగింపు

      (శూన్య)Curl_resolv_blocking(దత్తాంశం, dns_queries, host, 80, transport, &h);
      ఒకవేళ(h) {
        పూర్ణాంకం h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_చిరునామా(h->addr, myhost, పరిమాణం(myhost));
        infof(దత్తాంశం, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_నమోదు_unlink(దత్తాంశం, &h); /* this will NULL, potential free h */
        ఒకవేళ(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          ఫలితం = CURLE_UNSUPPORTED_PROTOCOL;
          వెళ్ళు బహిర్గతం;
        }
        పూర్తి = 1;
      }
      లేకపోతే {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        పూర్తి = -1;
      }
    }

    ఒకవేళ(పూర్తి > 0) {
#నిర్వచితమైతే USE_IPV6
      /* IPv6 address */
      ఒకవేళ(af == AF_INET6) {
#నిర్వచితమైతే HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        అక్షరం *scope_ptr = strchr(myhost, '%');
        ఒకవేళ(scope_ptr)
          *(scope_ptr++) = '\0';
#అయితే_ముగింపు
        ఒకవేళ(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(పోర్టు);
#నిర్వచితమైతే HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          ఒకవేళ(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            ఒకవేళ(curlx_str_సంఖ్య((స్థిరరాశి అక్షరం **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              ఫలితం = CURLE_UNSUPPORTED_PROTOCOL;
              వెళ్ళు బహిర్గతం;
            }
            si6->sin6_scope_id = (గుర్తులేని పూర్ణాంకం)scope_id;
          }
#అయితే_ముగింపు
        }
        sizeof_sa = పరిమాణం(నిర్మాణం sockaddr_in6);
      }
      లేకపోతే
#అయితే_ముగింపు
      /* IPv4 address */
      ఒకవేళ((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(పోర్టు);
        sizeof_sa = పరిమాణం(నిర్మాణం sockaddr_in);
      }
    }

    ఒకవేళ(పూర్తి < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      అక్షరం బఫరు[STRERROR_LEN];
      దత్తాంశం->state.errorbuf = FALSE;
      దత్తాంశం->state.os_errno = sockerr = SOCKERRNO;
      failf(దత్తాంశం, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));
      ఫలితం = CURLE_INTERFACE_FAILED;
      వెళ్ళు బహిర్గతం;
    }
  }
  లేకపోతే {
    /* no device was given, prepare sa to match af's needs */
#నిర్వచితమైతే USE_IPV6
    ఒకవేళ(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(పోర్టు);
      sizeof_sa = పరిమాణం(నిర్మాణం sockaddr_in6);
    }
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(పోర్టు);
      sizeof_sa = పరిమాణం(నిర్మాణం sockaddr_in);
    }
  }
#నిర్వచితమైతే IP_BIND_ADDRESS_NO_PORT
  (శూన్య)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, పరిమాణం(on));
#అయితే_ముగింపు
  కొరకు(;;) {
    ఒకవేళ(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(దత్తాంశం, "Local port: %hu", పోర్టు);
      conn->బిట్లు.bound = TRUE;
      ఫలితం = CURLE_OK;
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(--portnum > 0) {
      పోర్టు++; /* try next port */
      ఒకవేళ(పోర్టు == 0)
        ఆపు;
      infof(దత్తాంశం, "Bind to local port %d failed, trying next", పోర్టు - 1);
      /* We reuse/clobber the port variable here below */
      ఒకవేళ(sock->sa_family == AF_INET)
        si4->sin_port = htons(పోర్టు);
#నిర్వచితమైతే USE_IPV6
      లేకపోతే
        si6->sin6_port = htons(పోర్టు);
#అయితే_ముగింపు
    }
    లేకపోతే
      ఆపు;
  }
  {
    అక్షరం బఫరు[STRERROR_LEN];
    దత్తాంశం->state.os_errno = sockerr = SOCKERRNO;
    failf(దత్తాంశం, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));
  }

  ఫలితం = CURLE_INTERFACE_FAILED;

బహిర్గతం:
  curlx_free(dev_లోపల);
  curlx_free(iface_లోపల);
  curlx_free(host_లోపల);
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
స్థిర bool verifyconnect(curl_socket_t sockfd, పూర్ణాంకం *psockerr)
{
  bool rc = TRUE;
#నిర్వచితమైతే SO_ERROR
  పూర్ణాంకం sockerr = 0;
  curl_socklen_t err_కొలత = పరిమాణం(sockerr);

#నిర్వచితమైతే _WIN32
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
#అయితే_ముగింపు

  ఒకవేళ(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (శూన్య *)&sockerr, &err_కొలత))
    sockerr = SOCKERRNO;
#ఒకవేళ defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  ఒకవేళ(EBADIOCTL == sockerr) {
    సమితి_SOCKERRNO(0);
    sockerr = 0;
  }
#అయితే_ముగింపు
  ఒకవేళ((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  లేకపోతే
    /* This was not a successful connect */
    rc = FALSE;
  ఒకవేళ(psockerr)
    *psockerr = sockerr;
#లేకపోతే
  (శూన్య)sockfd;
  ఒకవేళ(psockerr)
    *psockerr = SOCKERRNO;
#అయితే_ముగింపు
  తిరిగి rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
స్థిర CURLcode socket_connect_ఫలితం(నిర్మాణం Curl_easy *దత్తాంశం,
                                      స్థిరరాశి అక్షరం *ipaddress, పూర్ణాంకం sockerr)
{
  ఒకవేళ(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    తిరిగి CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(అక్షరం బఫరు[STRERROR_LEN]);
    infof(దత్తాంశం, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));
    NOVERBOSE((శూన్య)ipaddress);
  }
  దత్తాంశం->state.os_errno = sockerr;
  /* connect failed */
  తిరిగి CURLE_COULDNT_CONNECT;
}

నిర్మాణం cf_socket_ctx {
  నిర్మాణం Curl_peer *peer;
  నిర్మాణం Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  నిర్మాణం ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  నిర్మాణం curltime started_at;        /* when socket was created */
  నిర్మాణం curltime connected_at;      /* when socket connected/got first byte */
  నిర్మాణం curltime మొదటి_byte_at;     /* when first byte was recvd */
#నిర్వచితమైతే USE_WINSOCK
  నిర్మాణం curltime చివరి_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_కొలత;                     /* the last set SO_SNDBUF size */
#అయితే_ముగింపు
  పూర్ణాంకం sockerr;                       /* socket error of last failure or 0 */
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం connect_wait_ms;               /* ms to wait after 1st connect() */
  పూర్ణాంకం wblock_percent;                /* percent of writes doing EAGAIN */
  పూర్ణాంకం wpartial_percent;              /* percent of bytes written in send */
  పూర్ణాంకం rblock_percent;                /* percent of reads doing EAGAIN */
  size_t recv_max;                   /* max enforced read size */
#అయితే_ముగింపు
  uint8_t transport;
  బిట్టు(got_మొదటి_byte);               /* if first byte was received */
  బిట్టు(listening);                    /* socket is listening */
  బిట్టు(accepted);                     /* socket was accepted, not connected */
  బిట్టు(sock_connected);               /* socket is "connected", e.g. in UDP */
  బిట్టు(చురుకైన);
  బిట్టు(stats_reported);
};

స్థిర CURLcode cf_socket_ctx_init(నిర్మాణం cf_socket_ctx *ctx,
                                   నిర్మాణం Curl_peer *peer,
                                   నిర్మాణం Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, పరిమాణం(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#నిర్వచితమైతే DEBUGBUILD
  {
    స్థిరరాశి అక్షరం *p = getenv("CURL_DBG_SOCK_WBLOCK");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, 100))
        ctx->wblock_percent = (పూర్ణాంకం)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, 100))
        ctx->wpartial_percent = (పూర్ణాంకం)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, 100))
        ctx->rblock_percent = (పూర్ణాంకం)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, గరిష్ఠ_పూర్ణాంకం))
        ctx->connect_wait_ms = (పూర్ణాంకం)l;
    }
  }
#అయితే_ముగింపు

  తిరిగి CURLE_OK;
}

స్థిర శూన్య cf_socket_ctx_free(నిర్మాణం cf_socket_ctx *ctx)
{
  ఒకవేళ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

స్థిర CURLcode cf_socket_shutdown(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   bool *పూర్తి)
{
  ఒకవేళ(cf->connected) {
    నిర్మాణం cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(దత్తాంశం, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    ఒకవేళ(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      గుర్తులేని అక్షరం buf[1024];
      (శూన్య)sread(ctx->sock, buf, పరిమాణం(buf));
    }
  }
  *పూర్తి = TRUE;
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cf_socket_destroy(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(దత్తాంశం, cf, "destroy");
  ఒకవేళ(ctx) {
    ఒకవేళ(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(దత్తాంశం, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      ఒకవేళ(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(దత్తాంశం, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

స్థిర శూన్య సమితి_local_ip(నిర్మాణం Curl_cfilter *cf,
                         నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_పోర్టు = 0;

#నిర్వచితమైతే HAVE_GETSOCKNAME
  ఒకవేళ((ctx->sock != CURL_SOCKET_BAD) &&
     !(దత్తాంశం->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    నిర్మాణం Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = పరిమాణం(నిర్మాణం Curl_sockaddr_storage);

    memset(&ssloc, 0, పరిమాణం(ssloc));
    ఒకవేళ(getsockname(ctx->sock, (నిర్మాణం sockaddr *)&ssloc, &slen)) {
      VERBOSE(అక్షరం బఫరు[STRERROR_LEN]);
      VERBOSE(పూర్ణాంకం sockerr = SOCKERRNO);
      infof(దత్తాంశం, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));
    }
    లేకపోతే {
      CURLcode ఫలితం = sockaddr2string((నిర్మాణం sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_పోర్టు);
      ఒకవేళ(ఫలితం)
        infof(దత్తాంశం, "ssloc inet_ntop() failed with %d", (పూర్ణాంకం)ఫలితం);
    }
  }
#లేకపోతే
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు
}

స్థిర CURLcode సమితి_remote_ip(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  CURLcode ఫలితం;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  ఫలితం = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_పోర్టు);
  ఒకవేళ(ఫలితం) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(దత్తాంశం, "curl_sa_addr inet_ntop() failed with %d", (పూర్ణాంకం)ఫలితం);
    తిరిగి CURLE_FAILED_INIT;
  }
  తిరిగి CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
స్థిర పూర్ణాంకం cf_socktype(పూర్ణాంకం x)
{
#నిర్వచితమైతే SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#అయితే_ముగింపు
  తిరిగి x;
}

#నిర్వచితమైతే _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#నిర్వచితం_కాకపోతే SIO_TCP_INITIAL_RTO
#నిర్వచించు SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#నిర్వచించు TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
రకనిర్వచనం నిర్మాణం _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#అయితే_ముగింపు /* SIO_TCP_INITIAL_RTO */

#నిర్వచితం_కాకపోతే TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#నిర్వచించు TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#అయితే_ముగింపు

స్థిర bool targets_localhost(నిర్మాణం cf_socket_ctx *ctx)
{
  తిరిగి (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
స్థిర శూన్య tcplocalhost(నిర్మాణం Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  ఒకవేళ(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD బైట్లు = 0;
    memset(&rto, 0, పరిమాణం(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    ఒకవేళ(curlx_verify_windows_సంస్కరణ(10, 0, 16299, PLATFORM_WINNT,
                                    సంస్కరణ_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    లేకపోతే
      rto.MaxSynRetransmissions = 1;
    (శూన్య)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, పరిమాణం(rto),
                   ఖాళీ, 0, &బైట్లు, ఖాళీ, ఖాళీ);
  }
}
#లేకపోతే
#నిర్వచించు tcplocalhost(x, y)
#అయితే_ముగింపు /* _WIN32 */

స్థిర CURLcode cf_socket_open(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  పూర్ణాంకం error = 0;
  bool isconnected = FALSE;
  CURLcode ఫలితం = CURLE_COULDNT_CONNECT;
  bool is_tcp;

  DEBUGASSERT(ctx->sock == CURL_SOCKET_BAD);
  ctx->started_at = *Curl_pgrs_now(దత్తాంశం);
#నిర్వచితమైతే CURL_USE_SOCK_NONBLOCK
  /* Do not tuck SOCK_NONBLOCK into socktype when opensocket callback is set
   * because we would not know how socktype is about to be used in the
   * callback, SOCK_NONBLOCK might get factored out before calling socket().
   */
  ఒకవేళ(!దత్తాంశం->సమితి.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#అయితే_ముగింపు
  ఫలితం = socket_open(దత్తాంశం, &ctx->addr, &ctx->sock);
#నిర్వచితమైతే CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  ఒకవేళ(!దత్తాంశం->సమితి.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#అయితే_ముగింపు
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = సమితి_remote_ip(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

#నిర్వచితమైతే USE_IPV6
  ఒకవేళ(ctx->addr.family == AF_INET6) {
#నిర్వచితమైతే USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    పూర్ణాంకం on = 0;
    (శూన్య)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (శూన్య *)&on, పరిమాణం(on));
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      నిర్మాణం sockaddr_in6 *sa6 = (శూన్య *)&ctx->addr.curl_sa_addr;
      ఒకవేళ(sa6->sin6_scope_id)
        infof(దత్తాంశం, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_పోర్టు,
              (గుర్తులేని దీర్ఘ)sa6->sin6_scope_id);
      లేకపోతే
#అయితే_ముగింపు
        infof(దత్తాంశం, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_పోర్టు);
#నిర్వచితమైతే HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#అయితే_ముగింపు
  }
  లేకపోతే
#అయితే_ముగింపు
    infof(దత్తాంశం, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_పోర్టు);

#నిర్వచితమైతే USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#లేకపోతే
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#అయితే_ముగింపు
  ఒకవేళ(is_tcp) {
    ఒకవేళ(దత్తాంశం->సమితి.tcp_nodelay)
      tcpnodelay(cf, దత్తాంశం, ctx->sock);

    ఒకవేళ(దత్తాంశం->సమితి.tcp_keepalive)
      tcpkeepalive(cf, దత్తాంశం, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  ఒకవేళ(దత్తాంశం->సమితి.fsockopt) {
    /* activate callback for setting socket options */
    నిర్మాణం Curl_mapi_guard guard;
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fsockopt);
    error = దత్తాంశం->సమితి.fsockopt(దత్తాంశం->సమితి.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_చివర(&guard);

    ఒకవేళ(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    లేకపోతే ఒకవేళ(error) {
      ఫలితం = CURLE_ABORTED_BY_CALLBACK;
      వెళ్ళు బహిర్గతం;
    }
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  ఒకవేళ(ctx->addr.family == AF_INET
#నిర్వచితమైతే USE_IPV6
     || ctx->addr.family == AF_INET6
#అయితే_ముగింపు
    ) {
    ఫలితం = bindlocal(దత్తాంశం, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        ఫలితం = CURLE_COULDNT_CONNECT;
      }
      వెళ్ళు బహిర్గతం;
    }
  }
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  ఒకవేళ(error < 0) {
    ఫలితం = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    వెళ్ళు బహిర్గతం;
  }
#లేకపోతే
  ఒకవేళ(దత్తాంశం->సమితి.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    ఒకవేళ(error < 0) {
      ఫలితం = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      వెళ్ళు బహిర్గతం;
    }
  }
#అయితే_ముగింపు
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(దత్తాంశం, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  లేకపోతే ఒకవేళ(isconnected) {
    సమితి_local_ip(cf, దత్తాంశం);
    ctx->connected_at = *Curl_pgrs_now(దత్తాంశం);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(దత్తాంశం, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (పూర్ణాంకం)ఫలితం, ctx->sock);
  తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం do_connect(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                      bool is_tcp_fastopen)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
#నిర్వచితమైతే TCP_FASTOPEN_CONNECT
  పూర్ణాంకం optval = 1;
#అయితే_ముగింపు
  పూర్ణాంకం rc = -1;

  (శూన్య)దత్తాంశం;
  ఒకవేళ(is_tcp_fastopen) {
#నిర్వచితమైతే CONNECT_దత్తాంశం_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_అందుబాటు
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    ఒకవేళ(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = ఖాళీ;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_దత్తాంశం_IDEMPOTENT,
                    ఖాళీ, 0, ఖాళీ, ఖాళీ);
    }
    లేకపోతే {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  లేకపోతే
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#లేదావేళ defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    ఒకవేళ(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (శూన్య *)&optval, పరిమాణం(optval)) < 0)
      CURL_TRC_CF(దత్తాంశం, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#లేదావేళ defined(MSG_FASTOPEN) /* old Linux */
    ఒకవేళ(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    లేకపోతే
      rc = 0; /* Do nothing */
#అయితే_ముగింపు
  }
  లేకపోతే {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  తిరిగి rc;
}

స్థిర CURLcode cf_tcp_connect(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               bool *పూర్తి)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_COULDNT_CONNECT;
  పూర్ణాంకం rc = 0;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  *పూర్తి = FALSE; /* a negative world view is best */
  ఒకవేళ(ctx->sock == CURL_SOCKET_BAD) {
    పూర్ణాంకం sockerr;

    ఫలితం = cf_socket_open(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    ఒకవేళ(cf->connected) {
      *పూర్తి = TRUE;
      తిరిగి CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, దత్తాంశం, (bool)cf->conn->బిట్లు.tcp_fastopen);
    sockerr = SOCKERRNO;
    సమితి_local_ip(cf, దత్తాంశం);
    CURL_TRC_CF(దత్తాంశం, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_పోర్టు);
#నిర్వచితమైతే DEBUGBUILD
    ఒకవేళ(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(దత్తాంశం, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#అయితే_ముగింపు
    ఒకవేళ(rc == -1) {
      ctx->sockerr = sockerr;
      ఫలితం = socket_connect_ఫలితం(దత్తాంశం, ctx->ip.remote_ip, sockerr);
      వెళ్ళు బహిర్గతం;
    }
  }

#నిర్వచితమైతే mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (శూన్య)verifyconnect(ctx->sock, ఖాళీ);
#అయితే_ముగింపు
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  ఒకవేళ(rc == 0) { /* no connection yet */
    CURL_TRC_CF(దత్తాంశం, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    తిరిగి CURLE_OK;
  }
  లేకపోతే ఒకవేళ(rc == CURL_CSELECT_బహిర్గతం ||
#నిర్వచితమైతే __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#అయితే_ముగింపు
          cf->conn->బిట్లు.tcp_fastopen) {
    ఒకవేళ(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(దత్తాంశం);
      సమితి_local_ip(cf, దత్తాంశం);
      *పూర్తి = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(దత్తాంశం, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      తిరిగి CURLE_OK;
    }
    ఫలితం = CURLE_COULDNT_CONNECT;
  }
  లేకపోతే ఒకవేళ(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(దత్తాంశం, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (శూన్య)verifyconnect(ctx->sock, &ctx->sockerr);
    ఫలితం = CURLE_COULDNT_CONNECT;
  }

బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    VERBOSE(అక్షరం బఫరు[STRERROR_LEN]);
    సమితి_local_ip(cf, దత్తాంశం);
    ఒకవేళ(ctx->sockerr) {
      దత్తాంశం->state.os_errno = ctx->sockerr;
      సమితి_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, బఫరు, పరిమాణం(బఫరు)));
    }
    లేకపోతే {
      VERBOSE(curlx_strcopy(బఫరు, పరిమాణం(బఫరు), STRCONST("peer closed")));
    }
    ఒకవేళ(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(దత్తాంశం, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(దత్తాంశం, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_పోర్టు,
          ctx->ip.local_ip, ctx->ip.local_పోర్టు,
          curlx_strerror(ctx->sockerr, బఫరు, పరిమాణం(బఫరు)));
    *పూర్తి = FALSE;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_socket_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    ఒకవేళ(ctx->listening) {
      ఫలితం = Curl_pollset_సమితి_లోపల_only(దత్తాంశం, ps, ctx->sock);
      CURL_TRC_CF(దత్తాంశం, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    లేకపోతే ఒకవేళ(!cf->connected) {
      ఫలితం = Curl_pollset_సమితి_బహిర్గతం_only(దత్తాంశం, ps, ctx->sock);
      CURL_TRC_CF(దత్తాంశం, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    లేకపోతే ఒకవేళ(!ctx->చురుకైన) {
      ఫలితం = Curl_pollset_కూడు_లోపల(దత్తాంశం, ps, ctx->sock);
      CURL_TRC_CF(దత్తాంశం, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#నిర్వచితం_కాకపోతే SIO_IDEAL_SEND_BACKLOG_QUERY
#నిర్వచించు SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#అయితే_ముగింపు

స్థిర శూన్య win_update_sndbuf_కొలత(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  ఒకవేళ(curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం),
                        &ctx->చివరి_sndbuf_query_at) > 1000) {
    ఒకవేళ(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, పరిమాణం(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_కొలత &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (స్థిరరాశి అక్షరం *)&ideal, పరిమాణం(ideal))) {
      ctx->sndbuf_కొలత = ideal;
    }
    ctx->చివరి_sndbuf_query_at = *Curl_pgrs_now(దత్తాంశం);
  }
}

#అయితే_ముగింపు /* USE_WINSOCK */

స్థిర CURLcode cf_socket_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                               స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode ఫలితం = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (శూన్య)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#నిర్వచితమైతే DEBUGBUILD
  /* simulate network blocking/partial writes */
  ఒకవేళ(ctx->wblock_percent > 0) {
    గుర్తులేని అక్షరం c = 0;
    Curl_rand_బైట్లు(దత్తాంశం, FALSE, &c, 1);
    ఒకవేళ(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(దత్తాంశం, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      తిరిగి CURLE_AGAIN;
    }
  }
  ఒకవేళ(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    ఒకవేళ(!len)
      len = 1;
    CURL_TRC_CF(దత్తాంశం, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#అయితే_ముగింపు

#ఒకవేళ defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  ఒకవేళ(cf->conn->బిట్లు.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->బిట్లు.tcp_fastopen = FALSE;
  }
  లేకపోతే
#అయితే_ముగింపు
    rv = swrite(ctx->sock, buf, len);

  ఒకవేళ(!curlx_sztouz(rv, pnwritten)) {
    పూర్ణాంకం sockerr = SOCKERRNO;
    ఒకవేళ(SOCK_EAGAIN(sockerr)
#నిర్వచితం_కాకపోతే USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#అయితే_ముగింపు
      ) {
      ఫలితం = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    లేకపోతే {
      అక్షరం బఫరు[STRERROR_LEN];
      failf(దత్తాంశం, "Send failure: %s",
            curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));
      దత్తాంశం->state.os_errno = sockerr;
      ఫలితం = CURLE_SEND_ERROR;
    }
  }

#నిర్వచితమైతే USE_WINSOCK
  ఒకవేళ(!ఫలితం)
    win_update_sndbuf_కొలత(దత్తాంశం, ctx);
#అయితే_ముగింపు

  CURL_TRC_CF(దత్తాంశం, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (పూర్ణాంకం)ఫలితం, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_socket_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                               అక్షరం *buf, size_t len, size_t *pnread)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#నిర్వచితమైతే DEBUGBUILD
  /* simulate network blocking/partial reads */
  ఒకవేళ(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    గుర్తులేని అక్షరం c = 0;
    Curl_rand(దత్తాంశం, &c, 1);
    ఒకవేళ(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(దత్తాంశం, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      తిరిగి CURLE_AGAIN;
    }
  }
  ఒకవేళ(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(దత్తాంశం, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#అయితే_ముగింపు

  rv = sread(ctx->sock, buf, len);

  ఒకవేళ(!curlx_sztouz(rv, pnread)) {
    పూర్ణాంకం sockerr = SOCKERRNO;
    ఒకవేళ(SOCK_EAGAIN(sockerr)
#నిర్వచితం_కాకపోతే USE_WINSOCK
       || (sockerr == SOCKEINTR)
#అయితే_ముగింపు
      ) {
      ఫలితం = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    లేకపోతే {
      అక్షరం బఫరు[STRERROR_LEN];
      failf(దత్తాంశం, "Recv failure: %s",
            curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));
      దత్తాంశం->state.os_errno = sockerr;
      ఫలితం = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(దత్తాంశం, cf, "recv(len=%zu) -> %d, %zu", len, (పూర్ణాంకం)ఫలితం, *pnread);
  ఒకవేళ(!ఫలితం && !ctx->got_మొదటి_byte) {
    ctx->మొదటి_byte_at = *Curl_pgrs_now(దత్తాంశం);
    ctx->got_మొదటి_byte = TRUE;
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_socket_update_దత్తాంశం(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* Update the IP info held in the transfer, if we have that. */
  ఒకవేళ(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
    దత్తాంశం->info.primary = ctx->ip;
  }
}

స్థిర శూన్య cf_socket_చురుకైన(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  సమితి_local_ip(cf, దత్తాంశం);
#నిర్వచితమైతే USE_IPV6
  ఒకవేళ(cf->sockindex == FIRSTSOCKET)
    cf->conn->బిట్లు.ipv6 = (ctx->addr.family == AF_INET6);
#అయితే_ముగింపు
  ctx->చురుకైన = TRUE;
}

స్థిర CURLcode cf_socket_cntrl(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;

  (శూన్య)arg1;
  (శూన్య)arg2;
  ఎంపిక(event) {
  స్థితి CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_చురుకైన(cf, దత్తాంశం);
    cf_socket_update_దత్తాంశం(cf, దత్తాంశం);
    ఆపు;
  స్థితి CF_CTRL_దత్తాంశం_అమరిక:
    cf_socket_update_దత్తాంశం(cf, దత్తాంశం);
    ఆపు;
  స్థితి CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    ఆపు;
  స్థితి CF_CTRL_REPORT_STATS:
    ఒకవేళ(cf->connected && !ctx->stats_reported) {
      నిర్మాణం curltime *ts = ఖాళీ;
      ఎంపిక(ctx->transport) {
      స్థితి TRNSPRT_UDP:
      స్థితి TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        ఒకవేళ(ctx->got_మొదటి_byte)
          ts = &ctx->మొదటి_byte_at;
        ఆపు;
      అప్రమేయ:
        ts = &ctx->connected_at;
        ఆపు;
      }
      ఒకవేళ(ts) {
        Curl_pgrs_సమయం_Was(దత్తాంశం, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  తిరిగి CURLE_OK;
}

స్థిర bool cf_socket_conn_is_alive(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    bool *ఆదానం_pending)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  నిర్మాణం pollfd pfd[1];
  పూర్ణాంకం r;

  *ఆదానం_pending = FALSE;

  ఒకవేళ(!ctx || ctx->sock == CURL_SOCKET_BAD)
    తిరిగి FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  ఒకవేళ(r < 0) {
    CURL_TRC_CF(దత్తాంశం, cf, "is_alive: poll error, assume dead");
    తిరిగి FALSE;
  }
  లేకపోతే ఒకవేళ(r == 0) {
    CURL_TRC_CF(దత్తాంశం, cf, "is_alive: poll timeout, assume alive");
    తిరిగి TRUE;
  }
  లేకపోతే ఒకవేళ(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(దత్తాంశం, cf, "is_alive: err/hup/etc events, assume dead");
    తిరిగి FALSE;
  }

  CURL_TRC_CF(దత్తాంశం, cf, "is_alive: valid events, looks alive");
  *ఆదానం_pending = TRUE;
  తిరిగి TRUE;
}

స్థిర CURLcode cf_socket_query(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;

  ఎంపిక(query) {
  స్థితి CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    తిరిగి CURLE_OK;
  స్థితి CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    తిరిగి CURLE_OK;
  స్థితి CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((స్థిరరాశి నిర్మాణం Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : ఖాళీ;
    తిరిగి CURLE_OK;
  స్థితి CF_QUERY_CONNECT_జవాబు_MS:
    ఒకవేళ(ctx->got_మొదటి_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->మొదటి_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < గరిష్ఠ_పూర్ణాంకం) ? (పూర్ణాంకం)ms : గరిష్ఠ_పూర్ణాంకం;
    }
    లేకపోతే
      *pres1 = -1;
    తిరిగి CURLE_OK;
  స్థితి CF_QUERY_IP_INFO:
#నిర్వచితమైతే USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#లేకపోతే
    *pres1 = FALSE;
#అయితే_ముగింపు
    *(నిర్మాణం ip_quadruple *)pres2 = ctx->ip;
    తిరిగి CURLE_OK;
  స్థితి CF_QUERY_REALLY_CONNECTED:
    ఒకవేళ(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    లేకపోతే
      *pres1 = ctx->got_మొదటి_byte;
    తిరిగి CURLE_OK;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

నిర్మాణం Curl_cftype Curl_cft_tcp = {
  "TCP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_దత్తాంశం_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_tcp_create(నిర్మాణం Curl_cfilter **pcf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_peer *origin,
                            నిర్మాణం Curl_peer *peer,
                            uint8_t transport_peer,
                            నిర్మాణం connectdata *conn,
                            నిర్మాణం Curl_sockaddr_ex *addr,
                            నిర్మాణం Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  నిర్మాణం cf_socket_ctx *ctx = ఖాళీ;
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  CURLcode ఫలితం;

  (శూన్య)దత్తాంశం;
  (శూన్య)origin;
  (శూన్య)conn;
  (శూన్య)tunnel_peer;
  (శూన్య)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  ఒకవేళ(!addr) {
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }

  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

బహిర్గతం:
  *pcf = (!ఫలితం) ? cf : ఖాళీ;
  ఒకవేళ(ఫలితం) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  తిరిగి ఫలితం;
}

#నిర్వచితమైతే __linux__
స్థిర శూన్య linux_quic_mtu(నిర్మాణం cf_socket_ctx *ctx)
{
  పూర్ణాంకం val;
  ఎంపిక(ctx->addr.family) {
#నిర్వచితమైతే IP_MTU_DISCOVER
  స్థితి AF_INET:
    val = IP_PMTUDISC_DO;
    (శూన్య)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     పరిమాణం(val));
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే IPV6_MTU_DISCOVER
  స్థితి AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (శూన్య)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     పరిమాణం(val));
    ఆపు;
#అయితే_ముగింపు
  }
}
#లేకపోతే
#నిర్వచించు linux_quic_mtu(x)
#అయితే_ముగింపు

#ఒకవేళ defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
స్థిర శూన్య linux_quic_gro(నిర్మాణం cf_socket_ctx *ctx)
{
  పూర్ణాంకం one = 1;
  (శూన్య)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)పరిమాణం(one));
}
#లేకపోతే
#నిర్వచించు linux_quic_gro(x)
#అయితే_ముగింపు

#ఒకవేళ (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
స్థిర శూన్య linux_quic_ecn(నిర్మాణం cf_socket_ctx *ctx)
{
  గుర్తులేని పూర్ణాంకం tos = 1;
  ఎంపిక(ctx->addr.family) {
  స్థితి AF_INET:
    (శూన్య)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, పరిమాణం(tos));
    ఆపు;
#నిర్వచితమైతే IPV6_RECVTCLASS
  స్థితి AF_INET6:
    (శూన్య)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, పరిమాణం(tos));
    ఆపు;
#అయితే_ముగింపు
  }
}
#లేకపోతే
#నిర్వచించు linux_quic_ecn(x)
#అయితే_ముగింపు

#ఒకవేళ (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
స్థిర శూన్య linux_ip_dontfrag(నిర్మాణం cf_socket_ctx *ctx)
{
  పూర్ణాంకం val = 1;

  ఎంపిక(ctx->addr.family) {
  స్థితి AF_INET:
    (శూన్య)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, పరిమాణం(val));
    ఆపు;
#నిర్వచితమైతే IPV6_DONTFRAG
  స్థితి AF_INET6:
    (శూన్య)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, పరిమాణం(val));
    ఆపు;
#అయితే_ముగింపు
  }
}
#లేకపోతే
#నిర్వచించు linux_ip_dontfrag(x)
#అయితే_ముగింపు

స్థిర CURLcode cf_udp_అమరిక_quic(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  పూర్ణాంకం rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  ఒకవేళ(rc == -1) {
    తిరిగి socket_connect_ఫలితం(దత్తాంశం, ctx->ip.remote_ip, SOCKERRNO);
  }
  ctx->sock_connected = TRUE;
  సమితి_local_ip(cf, దత్తాంశం);
  CURL_TRC_CF(దత్తాంశం, cf, "%s socket %" FMT_SOCKET_T
              " connected: [%s:%d] -> [%s:%d]",
              (ctx->transport == TRNSPRT_QUIC) ? "QUIC" : "UDP",
              ctx->sock, ctx->ip.local_ip, ctx->ip.local_పోర్టు,
              ctx->ip.remote_ip, ctx->ip.remote_పోర్టు);

  /* Currently, cf->ctx->sock is always non-blocking because the only
   * caller to cf_udp_setup_quic() is cf_udp_connect() that passes the
   * non-blocking socket created by cf_socket_open() to it. Thus, we
   * do not need to call curlx_nonblock() in cf_udp_setup_quic() anymore.
   */
  linux_quic_ecn(ctx);
  linux_quic_mtu(ctx);
  linux_ip_dontfrag(ctx);
  linux_quic_gro(ctx);

  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_udp_connect(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               bool *పూర్తి)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_COULDNT_CONNECT;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  *పూర్తి = FALSE;
  ఒకవేళ(ctx->sock == CURL_SOCKET_BAD) {
    ఫలితం = cf_socket_open(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "cf_udp_connect(), open failed -> %d",
                  (పూర్ణాంకం)ఫలితం);
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(ctx->transport == TRNSPRT_QUIC) {
      ఫలితం = cf_udp_అమరిక_quic(cf, దత్తాంశం);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      CURL_TRC_CF(దత్తాంశం, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_పోర్టు);
    }
    *పూర్తి = TRUE;
    cf->connected = TRUE;
  }
బహిర్గతం:
  తిరిగి ఫలితం;
}

నిర్మాణం Curl_cftype Curl_cft_udp = {
  "UDP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_udp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_దత్తాంశం_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_udp_create(నిర్మాణం Curl_cfilter **pcf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_peer *origin,
                            నిర్మాణం Curl_peer *peer,
                            uint8_t transport_peer,
                            నిర్మాణం connectdata *conn,
                            నిర్మాణం Curl_sockaddr_ex *addr,
                            నిర్మాణం Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  నిర్మాణం cf_socket_ctx *ctx = ఖాళీ;
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  CURLcode ఫలితం;

  (శూన్య)దత్తాంశం;
  (శూన్య)origin;
  (శూన్య)conn;
  (శూన్య)tunnel_peer;
  (శూన్య)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

బహిర్గతం:
  *pcf = (!ఫలితం) ? cf : ఖాళీ;
  ఒకవేళ(ఫలితం) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  తిరిగి ఫలితం;
}

/* this is the TCP filter which can also handle this case */
నిర్మాణం Curl_cftype Curl_cft_unix = {
  "UNIX",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_దత్తాంశం_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_unix_create(నిర్మాణం Curl_cfilter **pcf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_peer *origin,
                            నిర్మాణం Curl_peer *peer,
                            uint8_t transport_peer,
                            నిర్మాణం connectdata *conn,
                            నిర్మాణం Curl_sockaddr_ex *addr,
                            నిర్మాణం Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  నిర్మాణం cf_socket_ctx *ctx = ఖాళీ;
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  CURLcode ఫలితం;

  (శూన్య)దత్తాంశం;
  (శూన్య)origin;
  (శూన్య)conn;
  (శూన్య)tunnel_peer;
  (శూన్య)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

బహిర్గతం:
  *pcf = (!ఫలితం) ? cf : ఖాళీ;
  ఒకవేళ(ఫలితం) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  తిరిగి ఫలితం;
}

స్థిర timediff_t cf_tcp_accept_timeleft(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  ఒకవేళ(దత్తాంశం->సమితి.accepttimeout > 0)
    timeout_ms = దత్తాంశం->సమితి.accepttimeout;
#అయితే_ముగింపు

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(దత్తాంశం);
  ఒకవేళ(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  లేకపోతే {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &ctx->started_at);
    ఒకవేళ(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  తిరిగి timeout_ms;
}

స్థిర శూన్య cf_tcp_సమితి_accepted_remote_ip(నిర్మాణం Curl_cfilter *cf,
                                          నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
#నిర్వచితమైతే HAVE_GETPEERNAME
  CURLcode ఫలితం;
  నిర్మాణం Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_పోర్టు = 0;
  plen = పరిమాణం(ssrem);
  memset(&ssrem, 0, plen);
  ఒకవేళ(getpeername(ctx->sock, (నిర్మాణం sockaddr *)&ssrem, &plen)) {
    అక్షరం బఫరు[STRERROR_LEN];
    పూర్ణాంకం sockerr = SOCKERRNO;
    failf(దత్తాంశం, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, బఫరు, పరిమాణం(బఫరు)));
    తిరిగి;
  }
  ఫలితం = sockaddr2string((నిర్మాణం sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_పోర్టు);
  ఒకవేళ(ఫలితం) {
    failf(దత్తాంశం, "ssrem inet_ntop() failed with %d", (పూర్ణాంకం)ఫలితం);
    తిరిగి;
  }
#లేకపోతే
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_పోర్టు = 0;
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు
}

స్థిర CURLcode cf_tcp_accept_connect(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      bool *పూర్తి)
{
  నిర్మాణం cf_socket_ctx *ctx = cf->ctx;
  అక్షరం errbuf[STRERROR_LEN];
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం Curl_sockaddr_storage కూడు;
#లేకపోతే
  నిర్మాణం sockaddr_in కూడు;
#అయితే_ముగింపు
  curl_socklen_t కొలత = (curl_socklen_t)పరిమాణం(కూడు);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  పూర్ణాంకం socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (శూన్య)దత్తాంశం;
  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  *పూర్తి = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, దత్తాంశం);
  ఒకవేళ(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(దత్తాంశం, "Accept timeout occurred while waiting server connect");
    తిరిగి CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(దత్తాంశం, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_పోర్టు);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(దత్తాంశం, cf, "socket_check -> %x", (గుర్తులేని పూర్ణాంకం)socketstate);
  ఎంపిక(socketstate) {
  స్థితి -1: /* error */
    /* let's die here */
    failf(దత్తాంశం, "Error while waiting for server connect");
    తిరిగి CURLE_FTP_ACCEPT_FAILED;
  అప్రమేయ:
    ఒకవేళ(socketstate & CURL_CSELECT_లోపల) {
      infof(దత్తాంశం, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    ఆపు;
  }

  ఒకవేళ(!incoming) {
    CURL_TRC_CF(దత్తాంశం, cf, "nothing heard from the server yet");
    తిరిగి CURLE_OK;
  }

  కొలత = పరిమాణం(కూడు);
#నిర్వచితమైతే HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (నిర్మాణం sockaddr *)&కూడు, &కొలత,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#లేకపోతే
  s_accepted = CURL_ACCEPT(ctx->sock, (నిర్మాణం sockaddr *)&కూడు, &కొలత);
#అయితే_ముగింపు

  ఒకవేళ(s_accepted == CURL_SOCKET_BAD) {
    failf(దత్తాంశం, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, పరిమాణం(errbuf)));
    తిరిగి CURLE_FTP_ACCEPT_FAILED;
  }
#నిర్వచితం_కాకపోతే HAVE_ACCEPT4
#నిర్వచితమైతే HAVE_FCNTL
  ఒకవేళ(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(దత్తాంశం, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, పరిమాణం(errbuf)));
    Curl_socket_close(దత్తాంశం, cf->conn, s_accepted);
    తిరిగి CURLE_FTP_ACCEPT_FAILED;
  }
#అయితే_ముగింపు /* HAVE_FCNTL */
  ఒకవేళ(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(దత్తాంశం, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, పరిమాణం(errbuf)));
    Curl_socket_close(దత్తాంశం, cf->conn, s_accepted);
    తిరిగి CURLE_FTP_ACCEPT_FAILED;
  }
#అయితే_ముగింపు /* !HAVE_ACCEPT4 */
  infof(దత్తాంశం, "Connection accepted from server");

  /* Replace any filter on SECONDARY with one listening on this socket */
  ctx->listening = FALSE;
  ctx->accepted = TRUE;
  socket_close(దత్తాంశం, cf->conn, TRUE, ctx->sock);
  ctx->sock = s_accepted;

  cf->conn->sock[cf->sockindex] = ctx->sock;
  cf_tcp_సమితి_accepted_remote_ip(cf, దత్తాంశం);
  సమితి_local_ip(cf, దత్తాంశం);
  ctx->చురుకైన = TRUE;
  ctx->connected_at = *Curl_pgrs_now(దత్తాంశం);
  cf->connected = TRUE;
  CURL_TRC_CF(దత్తాంశం, cf, "accepted_set(sock=%" FMT_SOCKET_T
              ", remote=%s port=%d)",
              ctx->sock, ctx->ip.remote_ip, ctx->ip.remote_పోర్టు);

  ఒకవేళ(దత్తాంశం->సమితి.fsockopt) {
    నిర్మాణం Curl_mapi_guard guard;
    పూర్ణాంకం error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fsockopt);
    error = దత్తాంశం->సమితి.fsockopt(దత్తాంశం->సమితి.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_చివర(&guard);

    ఒకవేళ(error)
      తిరిగి CURLE_ABORTED_BY_CALLBACK;
  }
  *పూర్తి = TRUE;
  తిరిగి CURLE_OK;
}

నిర్మాణం Curl_cftype Curl_cft_tcp_accept = {
  "TCP-ACCEPT",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_accept_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_దత్తాంశం_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_conn_tcp_listen_సమితి(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode ఫలితం;
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం cf_socket_ctx *ctx = ఖాళీ;

  /* replace any existing */
  Curl_conn_cf_discard_all(దత్తాంశం, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  ఫలితం = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  Curl_conn_cf_కూడు(దత్తాంశం, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(దత్తాంశం);
  conn->sock[sockindex] = ctx->sock;
  సమితి_local_ip(cf, దత్తాంశం);
  CURL_TRC_CF(దత్తాంశం, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_పోర్టు);

బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  తిరిగి ఫలితం;
}

bool Curl_conn_is_tcp_listen(నిర్మాణం Curl_easy *దత్తాంశం,
                             int8_t sockindex)
{
  నిర్మాణం Curl_cfilter *cf = దత్తాంశం->conn->cfilter[sockindex];
  అయితే(cf) {
    ఒకవేళ(cf->cft == &Curl_cft_tcp_accept)
      తిరిగి TRUE;
    cf = cf->తదుపరి;
  }
  తిరిగి FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
స్థిర bool cf_is_socket(నిర్మాణం Curl_cfilter *cf)
{
  తిరిగి cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             curl_socket_t *psock,
                             స్థిరరాశి నిర్మాణం Curl_sockaddr_ex **paddr,
                             నిర్మాణం ip_quadruple *pip)
{
  (శూన్య)దత్తాంశం;
  ఒకవేళ(cf_is_socket(cf) && cf->ctx) {
    నిర్మాణం cf_socket_ctx *ctx = cf->ctx;

    ఒకవేళ(psock)
      *psock = ctx->sock;
    ఒకవేళ(paddr)
      *paddr = &ctx->addr;
    ఒకవేళ(pip)
      *pip = ctx->ip;
    తిరిగి CURLE_OK;
  }
  తిరిగి CURLE_FAILED_INIT;
}
