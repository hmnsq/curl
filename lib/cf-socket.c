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

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_SYS_UN_H
#ಅನ್ತರ್ಭಾವ <sys/un.h> /* for sockaddr_un */
#endif
#ifdef HAVE_LINUX_TCP_H
#ಅನ್ತರ್ಭಾವ <linux/tcp.h>
#ನೋಚೇದ್ಯದಿ defined(HAVE_NETINET_TCP_H)
#ಅನ್ತರ್ಭಾವ <netinet/tcp.h>
#endif
#ifdef HAVE_NETINET_UDP_H
#ಅನ್ತರ್ಭಾವ <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#ಅನ್ತರ್ಭಾವ <netinet/ip.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif

#ifdef HAVE_IFADDRS_H
#ಅನ್ತರ್ಭಾವ <ifaddrs.h>
#endif
#ifdef HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#endif
#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "if2ip.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-socket.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "conncache.h"
#ಅನ್ತರ್ಭಾವ "multihandle.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "sockaddr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/nonblock.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"
#ಅನ್ತರ್ಭಾವ "curlx/version_win32.h"
#ಅನ್ತರ್ಭಾವ "curlx/strerr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curlx/wait.h"
#ifdef _WIN32
#ಅನ್ತರ್ಭಾವ <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#endif

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(ಸಂರಚನಾ sockaddr *sa, curl_socklen_t salen,
                                  ವರ್ಣ *addr, uint16_t *port);
UNITTEST CURLcode sockaddr2string(ಸಂರಚನಾ sockaddr *sa, curl_socklen_t salen,
                                  ವರ್ಣ *addr, uint16_t *port)
{
  CURLcode result;
  ಸಂರಚನಾ sockaddr_in *si = ರಿಕ್ತ;
#ifdef USE_IPV6
  ಸಂರಚನಾ sockaddr_in6 *si6 = ರಿಕ್ತ;
#endif
#ifdef USE_UNIX_SOCKETS
  ಸಂರಚನಾ sockaddr_un *su = ರಿಕ್ತ;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)salen;
#endif

  ವಿಕಲ್ಪ(sa->sa_family) {
  ಪಕ್ಷ AF_INET:
    si = (ಸಂರಚನಾ sockaddr_in *)(ಶೂನ್ಯ *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    ಯದಿ(!result) {
      *port = ntohs(si->sin_port);
      ಪ್ರತ್ಯಾಗಮ result;
    }
    ಭಙ್ಗ;
#ifdef USE_IPV6
  ಪಕ್ಷ AF_INET6:
    si6 = (ಸಂರಚನಾ sockaddr_in6 *)(ಶೂನ್ಯ *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    ಯದಿ(!result) {
      *port = ntohs(si6->sin6_port);
      ಪ್ರತ್ಯಾಗಮ result;
    }
    ಭಙ್ಗ;
#endif
#ifdef USE_UNIX_SOCKETS
  ಪಕ್ಷ AF_UNIX:
    ಯದಿ(salen > (curl_socklen_t)ಪರಿಮಾಣ(CURL_SA_FAMILY_T)) {
      su = (ಸಂರಚನಾ sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_path);
    }
    ಅನ್ಯಥಾ
      addr[0] = 0; /* socket with no name */
    *port = 0;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
#endif
  ಸ್ವಭಾವ:
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ಭಙ್ಗ;
  }

  addr[0] = '\0';
  *port = 0;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ tcpnodelay(ಸಂರಚನಾ Curl_cfilter *cf,
                       ಸಂರಚನಾ Curl_easy *data,
                       curl_socket_t sockfd)
{
#ಯದಿ defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  ಪೂರ್ಣಾಙ್ಕ level = IPPROTO_TCP;
  VERBOSE(ವರ್ಣ buffer[STRERROR_LEN]);

  ಯದಿ(setsockopt(sockfd, level, TCP_NODELAY, (ಶೂನ್ಯ *)&onoff, ಪರಿಮಾಣ(onoff)) < 0)
    CURL_TRC_CF(data, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, buffer, ಪರಿಮಾಣ(buffer)));
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)sockfd;
#endif
}

#ಯದಿ defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_THRESHOLD) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
#ಯದಿ defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
ಸ್ಥಿರ ಶೂನ್ಯ alive_unit(ಪೂರ್ಣಾಙ್ಕ *x)
{
  /* make sure this doesn't wrap */
  ಯದಿ(*x < (INT_MAX / 1000))
    *x *= 1000;
  ಅನ್ಯಥಾ
    /* this is still almost 25 days */
    *x = INT_MAX;
}
#ಅನ್ಯಥಾ
#define alive_unit(x)
#endif
#endif

ಸ್ಥಿರ ಶೂನ್ಯ tcpkeepalive(ಸಂರಚನಾ Curl_cfilter *cf,
                         ಸಂರಚನಾ Curl_easy *data,
                         curl_socket_t sockfd)
{
  ಪೂರ್ಣಾಙ್ಕ optval = data->set.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  ಯದಿ(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (ಶೂನ್ಯ *)&optval, ಪರಿಮಾಣ(optval)) < 0) {
    CURL_TRC_CF(data, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  ಅನ್ಯಥಾ {
#ifdef USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    ಯದಿ(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
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
      ಯದಿ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (ಧ್ರುವ ವರ್ಣ *)&optval, ಪರಿಮಾಣ(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      ಯದಿ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (ಧ್ರುವ ವರ್ಣ *)&optval, ಪರಿಮಾಣ(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepcnt);
      ಯದಿ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (ಧ್ರುವ ವರ್ಣ *)&optval, ಪರಿಮಾಣ(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    ಅನ್ಯಥಾ {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ifndef SIO_KEEPALIVE_VALS
#define SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      ಸಂರಚನಾ tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#endif
      ಸಂರಚನಾ tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(data->set.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      ಯದಿ(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, ಪರಿಮಾಣ(vals),
                  ರಿಕ್ತ, 0, &dummy, ರಿಕ್ತ, ರಿಕ್ತ) != 0) {
        CURL_TRC_CF(data, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#ಅನ್ಯಥಾ /* !USE_WINSOCK */
#ifdef TCP_KEEPIDLE
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    ಯದಿ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (ಶೂನ್ಯ *)&optval, ಪರಿಮಾಣ(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ನೋಚೇದ್ಯದಿ defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    ಯದಿ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (ಶೂನ್ಯ *)&optval, ಪರಿಮಾಣ(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ನೋಚೇದ್ಯದಿ defined(TCP_KEEPALIVE_THRESHOLD)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    ಯದಿ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_THRESHOLD,
                  (ಶೂನ್ಯ *)&optval, ಪರಿಮಾಣ(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPINTVL
    optval = curlx_sltosi(data->set.tcp_keepintvl);
    alive_unit(&optval);
    ಯದಿ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (ಶೂನ್ಯ *)&optval, ಪರಿಮಾಣ(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ನೋಚೇದ್ಯದಿ defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
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
      ಪೂರ್ಣಾಙ್ಕ keepcnt = curlx_sltosi(data->set.tcp_keepcnt);
      ಪೂರ್ಣಾಙ್ಕ keepintvl = curlx_sltosi(data->set.tcp_keepintvl);

      ಯದಿ(keepcnt > 0 && keepintvl > (INT_MAX / keepcnt))
        optval = INT_MAX;
      ಅನ್ಯಥಾ
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    ಯದಿ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_THRESHOLD,
                  (ಶೂನ್ಯ *)&optval, ಪರಿಮಾಣ(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPCNT
    optval = curlx_sltosi(data->set.tcp_keepcnt);
    ಯದಿ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (ಶೂನ್ಯ *)&optval, ಪರಿಮಾಣ(optval)) < 0) {
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
CURLcode Curl_socket_addr_from_ai(ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                                  ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai,
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
  addr->addrlen = (unsigned ಪೂರ್ಣಾಙ್ಕ)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= ಪರಿಮಾಣ(addr->curl_sa_addrbuf));
  ಯದಿ(addr->addrlen > ಪರಿಮಾಣ(addr->curl_sa_addrbuf))
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;

  memcpy(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef USE_SO_NOSIGPIPE
ಪೂರ್ಣಾಙ್ಕ Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  ಪೂರ್ಣಾಙ್ಕ onoff = 1;
  ಪ್ರತ್ಯಾಗಮ setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (ಶೂನ್ಯ *)&onoff, ಪರಿಮಾಣ(onoff));
}
#endif /* USE_SO_NOSIGPIPE */

#ಯದಿ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ಸ್ಥಿರ uint32_t get_scope_id(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  ಯದಿ(data->conn->scope_id)
    ಪ್ರತ್ಯಾಗಮ data->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  ಯದಿ(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#ಯದಿ defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    ಸಂರಚನಾ ifaddrs *ifa, *ifa_list;
    ಯದಿ(getifaddrs(&ifa_list) == 0) {
      ಪ್ರತಿ(ifa = ifa_list; ifa; ifa = ifa->ifa_next) {
        ಯದಿ(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          ಸಂರಚನಾ sockaddr_in6 *s6 = (ಶೂನ್ಯ *)ifa->ifa_addr;
          ಯದಿ(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(data,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (unsigned long)scope_id);
            ಭಙ್ಗ;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#endif /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  ಪ್ರತ್ಯಾಗಮ scope_id;
}
#endif

ಸ್ಥಿರ CURLcode socket_open(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  ವರ್ಣ errbuf[STRERROR_LEN];

#ifdef SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#endif

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ಯದಿ(data->set.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    ಸಂರಚನಾ Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fopensocket);
    *sockfd = data->set.fopensocket(data->set.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (ಸಂರಚನಾ curl_sockaddr *)addr);
    CURL_CBAPI_END(&guard);
  }
  ಅನ್ಯಥಾ {
    /* opensocket callback not set, so create the socket now */
#ifdef DEBUGBUILD
    ಯದಿ((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(data, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
    }
#endif
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    ಯದಿ((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಯದಿ(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(data, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಪರಿಮಾಣ(errbuf)));
    ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
  }

#ifdef USE_SO_NOSIGPIPE
  ಯದಿ(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(data, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಪರಿಮಾಣ(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
  }
#endif /* USE_SO_NOSIGPIPE */

#ಯದಿ defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  ಯದಿ(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಪರಿಮಾಣ(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
  }
#endif

#ಯದಿ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  ಯದಿ(addr->family == AF_INET6) {
    ಸಂರಚನಾ sockaddr_in6 * ಧ್ರುವ sa6 = (ಶೂನ್ಯ *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(data, sa6);
  }
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
CURLcode Curl_socket_open(ಸಂರಚನಾ Curl_easy *data,
                          ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *ai,
                          ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  ಸಂರಚನಾ Curl_sockaddr_ex dummy;
  CURLcode result;

  ಯದಿ(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  result = Curl_socket_addr_from_ai(addr, ai, transport);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಪ್ರತ್ಯಾಗಮ socket_open(data, addr, sockfd);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ socket_close(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ connectdata *conn,
                        ಪೂರ್ಣಾಙ್ಕ use_callback, curl_socket_t sock)
{
  ಯದಿ(sock == CURL_SOCKET_BAD)
    ಪ್ರತ್ಯಾಗಮ 0;

  ಯದಿ(use_callback && conn && conn->fclosesocket) {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    ಪೂರ್ಣಾಙ್ಕ rc;
    Curl_multi_will_close(data, sock);
    CURL_CBAPI_START(&guard, data, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_END(&guard);
    ಪ್ರತ್ಯಾಗಮ rc;
  }

  ಯದಿ(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(data, sock);

  sclose(sock);

  ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
ಪೂರ್ಣಾಙ್ಕ Curl_socket_close(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ connectdata *conn,
                      curl_socket_t sock)
{
  ಪ್ರತ್ಯಾಗಮ socket_close(data, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು,
                              ವರ್ಣ **dev, ವರ್ಣ **iface, ವರ್ಣ **host)
{
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ if_prefix[] = "if!";
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ host_prefix[] = "host!";
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ if_host_prefix[] = "ifhost!";
  size_t len;

  len = strlen(ಒಳಹರಿವು);
  ಯದಿ(len > 512)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಯದಿ(dev)
    *dev = ರಿಕ್ತ;
  ಯದಿ(iface)
    *iface = ರಿಕ್ತ;
  ಯದಿ(host)
    *host = ರಿಕ್ತ;

  ಯದಿ(!strncmp(if_prefix, ಒಳಹರಿವು, strlen(if_prefix))) {
    ಒಳಹರಿವು += strlen(if_prefix);
    ಯದಿ(!*ಒಳಹರಿವು)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    ಯದಿ(iface) {
      *iface = curlx_memdup0(ಒಳಹರಿವು, len - strlen(if_prefix));
      ಪ್ರತ್ಯಾಗಮ *iface ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(host_prefix, ಒಳಹರಿವು, strlen(host_prefix))) {
    ಒಳಹರಿವು += strlen(host_prefix);
    ಯದಿ(!*ಒಳಹರಿವು)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    ಯದಿ(host) {
      *host = curlx_memdup0(ಒಳಹರಿವು, len - strlen(host_prefix));
      ಪ್ರತ್ಯಾಗಮ *host ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(if_host_prefix, ಒಳಹರಿವು, strlen(if_host_prefix))) {
    ಧ್ರುವ ವರ್ಣ *host_part;
    ಒಳಹರಿವು += strlen(if_host_prefix);
    len -= strlen(if_host_prefix);
    host_part = memchr(ಒಳಹರಿವು, '!', len);
    ಯದಿ(!host_part || !*(host_part + 1))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    ಯದಿ(iface) {
      *iface = curlx_memdup0(ಒಳಹರಿವು, host_part - ಒಳಹರಿವು);
      ಯದಿ(!*iface)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    ++host_part;
    ಯದಿ(host) {
      *host = curlx_memdup0(host_part, len - (host_part - ಒಳಹರಿವು));
      ಯದಿ(!*host) {
        ಯದಿ(iface)
          curlx_safefree(*iface);
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ಯದಿ(!*ಒಳಹರಿವು)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಯದಿ(dev) {
    *dev = curlx_memdup0(ಒಳಹರಿವು, len);
    ಪ್ರತ್ಯಾಗಮ *dev ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifndef CURL_DISABLE_BINDLOCAL
ಸ್ಥಿರ CURLcode bindlocal(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ connectdata *conn,
                          curl_socket_t sockfd, ಪೂರ್ಣಾಙ್ಕ af, unsigned ಪೂರ್ಣಾಙ್ಕ scope,
                          uint8_t transport)
{
  ಸಂರಚನಾ Curl_sockaddr_storage sa;
  ಸಂರಚನಾ sockaddr *sock = (ಸಂರಚನಾ sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  ಸಂರಚನಾ sockaddr_in *si4 = (ಸಂರಚನಾ sockaddr_in *)&sa;
#ifdef USE_IPV6
  ಸಂರಚನಾ sockaddr_in6 *si6 = (ಸಂರಚನಾ sockaddr_in6 *)&sa;
#endif
  ಸಂರಚನಾ Curl_dns_entry *h = ರಿಕ್ತ;
  unsigned short port = data->set.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  ಪೂರ್ಣಾಙ್ಕ portnum = data->set.localportrange;
  ವರ್ಣ *dev_in = ರಿಕ್ತ, *iface_in = ರಿಕ್ತ, *host_in = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *iface = ರಿಕ್ತ, *host = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ sockerr;
#ifdef IP_BIND_ADDRESS_NO_PORT
  ಪೂರ್ಣಾಙ್ಕ on = 1;
#endif
  CURLcode result = CURLE_OK;

#ifndef USE_IPV6
  (ಶೂನ್ಯ)scope;
#endif
  ಯದಿ(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    result = Curl_parse_interface(conn->interface_opts,
                                  &dev_in, &iface_in, &host_in);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    /* This is how we use the values */
    iface = iface_in ? iface_in : dev_in;
    host = host_in ? host_in : dev_in;
  }

  ಯದಿ(!iface && !host && !port)
    ಗಚ್ಛ out;

  ಯದಿ(iface && (strlen(iface) >= 255)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }
  memset(&sa, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_sockaddr_storage));

  ಯದಿ(iface || host) {
    ವರ್ಣ myhost[256] = "";
    ಪೂರ್ಣಾಙ್ಕ done = 0; /* -1 for error, 1 for address found */
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
    ಯದಿ(iface &&
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
      ಗಚ್ಛ out;
    }
#endif
    ಯದಿ(!host_in) {
      /* Discover IP from input device, then bind to it */
      if2ip_result = Curl_if2ip(af,
#ifdef USE_IPV6
                                scope, conn->scope_id,
#endif
                                iface, myhost, ಪರಿಮಾಣ(myhost));
    }
    ವಿಕಲ್ಪ(if2ip_result) {
    ಪಕ್ಷ IF2IP_NOT_FOUND:
      ಯದಿ(iface_in && !host_in) {
        /* Do not fall back to treating it as a hostname */
        ವರ್ಣ buffer[STRERROR_LEN];
        data->state.os_errno = sockerr = SOCKERRNO;
        failf(data, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));
        result = CURLE_INTERFACE_FAILED;
        ಗಚ್ಛ out;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      result = CURLE_UNSUPPORTED_PROTOCOL;
      ಗಚ್ಛ out;
    ಪಕ್ಷ IF2IP_FOUND:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(data, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      done = 1;
      ಭಙ್ಗ;
    }
    ಯದಿ(!iface_in || host_in) {
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
      ಯದಿ(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#endif

      (ಶೂನ್ಯ)Curl_resolv_blocking(data, dns_queries, host, 80, transport, &h);
      ಯದಿ(h) {
        ಪೂರ್ಣಾಙ್ಕ h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_address(h->addr, myhost, ಪರಿಮಾಣ(myhost));
        infof(data, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_entry_unlink(data, &h); /* this will NULL, potential free h */
        ಯದಿ(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          result = CURLE_UNSUPPORTED_PROTOCOL;
          ಗಚ್ಛ out;
        }
        done = 1;
      }
      ಅನ್ಯಥಾ {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        done = -1;
      }
    }

    ಯದಿ(done > 0) {
#ifdef USE_IPV6
      /* IPv6 address */
      ಯದಿ(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        ವರ್ಣ *scope_ptr = strchr(myhost, '%');
        ಯದಿ(scope_ptr)
          *(scope_ptr++) = '\0';
#endif
        ಯದಿ(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          ಯದಿ(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            ಯದಿ(curlx_str_ಸಂಖ್ಯೆ((ಧ್ರುವ ವರ್ಣ **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              result = CURLE_UNSUPPORTED_PROTOCOL;
              ಗಚ್ಛ out;
            }
            si6->sin6_scope_id = (unsigned ಪೂರ್ಣಾಙ್ಕ)scope_id;
          }
#endif
        }
        sizeof_sa = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6);
      }
      ಅನ್ಯಥಾ
#endif
      /* IPv4 address */
      ಯದಿ((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(port);
        sizeof_sa = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in);
      }
    }

    ಯದಿ(done < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      ವರ್ಣ buffer[STRERROR_LEN];
      data->state.errorbuf = FALSE;
      data->state.os_errno = sockerr = SOCKERRNO;
      failf(data, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));
      result = CURLE_INTERFACE_FAILED;
      ಗಚ್ಛ out;
    }
  }
  ಅನ್ಯಥಾ {
    /* no device was given, prepare sa to match af's needs */
#ifdef USE_IPV6
    ಯದಿ(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(port);
      sizeof_sa = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6);
    }
    ಅನ್ಯಥಾ
#endif
    ಯದಿ(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(port);
      sizeof_sa = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in);
    }
  }
#ifdef IP_BIND_ADDRESS_NO_PORT
  (ಶೂನ್ಯ)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, ಪರಿಮಾಣ(on));
#endif
  ಪ್ರತಿ(;;) {
    ಯದಿ(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(data, "Local port: %hu", port);
      conn->bits.bound = TRUE;
      result = CURLE_OK;
      ಗಚ್ಛ out;
    }

    ಯದಿ(--portnum > 0) {
      port++; /* try next port */
      ಯದಿ(port == 0)
        ಭಙ್ಗ;
      infof(data, "Bind to local port %d failed, trying next", port - 1);
      /* We reuse/clobber the port variable here below */
      ಯದಿ(sock->sa_family == AF_INET)
        si4->sin_port = htons(port);
#ifdef USE_IPV6
      ಅನ್ಯಥಾ
        si6->sin6_port = htons(port);
#endif
    }
    ಅನ್ಯಥಾ
      ಭಙ್ಗ;
  }
  {
    ವರ್ಣ buffer[STRERROR_LEN];
    data->state.os_errno = sockerr = SOCKERRNO;
    failf(data, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));
  }

  result = CURLE_INTERFACE_FAILED;

out:
  curlx_free(dev_in);
  curlx_free(iface_in);
  curlx_free(host_in);
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
ಸ್ಥಿರ bool verifyconnect(curl_socket_t sockfd, ಪೂರ್ಣಾಙ್ಕ *psockerr)
{
  bool rc = TRUE;
#ifdef SO_ERROR
  ಪೂರ್ಣಾಙ್ಕ sockerr = 0;
  curl_socklen_t errSize = ಪರಿಮಾಣ(sockerr);

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

  ಯದಿ(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (ಶೂನ್ಯ *)&sockerr, &errSize))
    sockerr = SOCKERRNO;
#ಯದಿ defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  ಯದಿ(EBADIOCTL == sockerr) {
    SET_SOCKERRNO(0);
    sockerr = 0;
  }
#endif
  ಯದಿ((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  ಅನ್ಯಥಾ
    /* This was not a successful connect */
    rc = FALSE;
  ಯದಿ(psockerr)
    *psockerr = sockerr;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)sockfd;
  ಯದಿ(psockerr)
    *psockerr = SOCKERRNO;
#endif
  ಪ್ರತ್ಯಾಗಮ rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
ಸ್ಥಿರ CURLcode socket_connect_result(ಸಂರಚನಾ Curl_easy *data,
                                      ಧ್ರುವ ವರ್ಣ *ipaddress, ಪೂರ್ಣಾಙ್ಕ sockerr)
{
  ಯದಿ(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(ವರ್ಣ buffer[STRERROR_LEN]);
    infof(data, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));
    NOVERBOSE((ಶೂನ್ಯ)ipaddress);
  }
  data->state.os_errno = sockerr;
  /* connect failed */
  ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
}

ಸಂರಚನಾ cf_socket_ctx {
  ಸಂರಚನಾ Curl_peer *peer;
  ಸಂರಚನಾ Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  ಸಂರಚನಾ ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  ಸಂರಚನಾ curltime started_at;        /* when socket was created */
  ಸಂರಚನಾ curltime connected_at;      /* when socket connected/got first byte */
  ಸಂರಚನಾ curltime first_byte_at;     /* when first byte was recvd */
#ifdef USE_WINSOCK
  ಸಂರಚನಾ curltime last_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_size;                     /* the last set SO_SNDBUF size */
#endif
  ಪೂರ್ಣಾಙ್ಕ sockerr;                       /* socket error of last failure or 0 */
#ifdef DEBUGBUILD
  ಪೂರ್ಣಾಙ್ಕ connect_wait_ms;               /* ms to wait after 1st connect() */
  ಪೂರ್ಣಾಙ್ಕ wblock_percent;                /* percent of writes doing EAGAIN */
  ಪೂರ್ಣಾಙ್ಕ wpartial_percent;              /* percent of bytes written in send */
  ಪೂರ್ಣಾಙ್ಕ rblock_percent;                /* percent of reads doing EAGAIN */
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

ಸ್ಥಿರ CURLcode cf_socket_ctx_init(ಸಂರಚನಾ cf_socket_ctx *ctx,
                                   ಸಂರಚನಾ Curl_peer *peer,
                                   ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, ಪರಿಮಾಣ(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ifdef DEBUGBUILD
  {
    ಧ್ರುವ ವರ್ಣ *p = getenv("CURL_DBG_SOCK_WBLOCK");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 100))
        ctx->wblock_percent = (ಪೂರ್ಣಾಙ್ಕ)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 100))
        ctx->wpartial_percent = (ಪೂರ್ಣಾಙ್ಕ)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 100))
        ctx->rblock_percent = (ಪೂರ್ಣಾಙ್ಕ)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, INT_MAX))
        ctx->connect_wait_ms = (ಪೂರ್ಣಾಙ್ಕ)l;
    }
  }
#endif

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_socket_ctx_free(ಸಂರಚನಾ cf_socket_ctx *ctx)
{
  ಯದಿ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_socket_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   bool *done)
{
  ಯದಿ(cf->connected) {
    ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(data, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    ಯದಿ(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      unsigned ವರ್ಣ buf[1024];
      (ಶೂನ್ಯ)sread(ctx->sock, buf, ಪರಿಮಾಣ(buf));
    }
  }
  *done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_socket_destroy(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  ಯದಿ(ctx) {
    ಯದಿ(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(data, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      ಯದಿ(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(data, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ set_local_ip(ಸಂರಚನಾ Curl_cfilter *cf,
                         ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_port = 0;

#ifdef HAVE_GETSOCKNAME
  ಯದಿ((ctx->sock != CURL_SOCKET_BAD) &&
     !(data->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    ಸಂರಚನಾ Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_sockaddr_storage);

    memset(&ssloc, 0, ಪರಿಮಾಣ(ssloc));
    ಯದಿ(getsockname(ctx->sock, (ಸಂರಚನಾ sockaddr *)&ssloc, &slen)) {
      VERBOSE(ವರ್ಣ buffer[STRERROR_LEN]);
      VERBOSE(ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO);
      infof(data, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));
    }
    ಅನ್ಯಥಾ {
      CURLcode result = sockaddr2string((ಸಂರಚನಾ sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_port);
      ಯದಿ(result)
        infof(data, "ssloc inet_ntop() failed with %d", (ಪೂರ್ಣಾಙ್ಕ)result);
    }
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
#endif
}

ಸ್ಥಿರ CURLcode set_remote_ip(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  CURLcode result;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  result = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_port);
  ಯದಿ(result) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(data, "curl_sa_addr inet_ntop() failed with %d", (ಪೂರ್ಣಾಙ್ಕ)result);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cf_socktype(ಪೂರ್ಣಾಙ್ಕ x)
{
#ifdef SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#endif
  ಪ್ರತ್ಯಾಗಮ x;
}

#ifdef _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ifndef SIO_TCP_INITIAL_RTO
#define SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#define TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#endif /* SIO_TCP_INITIAL_RTO */

#ifndef TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#define TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#endif

ಸ್ಥಿರ bool targets_localhost(ಸಂರಚನಾ cf_socket_ctx *ctx)
{
  ಪ್ರತ್ಯಾಗಮ (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
ಸ್ಥಿರ ಶೂನ್ಯ tcplocalhost(ಸಂರಚನಾ Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  ಯದಿ(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD bytes = 0;
    memset(&rto, 0, ಪರಿಮಾಣ(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    ಯದಿ(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    ಅನ್ಯಥಾ
      rto.MaxSynRetransmissions = 1;
    (ಶೂನ್ಯ)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, ಪರಿಮಾಣ(rto),
                   ರಿಕ್ತ, 0, &bytes, ರಿಕ್ತ, ರಿಕ್ತ);
  }
}
#ಅನ್ಯಥಾ
#define tcplocalhost(x, y)
#endif /* _WIN32 */

ಸ್ಥಿರ CURLcode cf_socket_open(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಙ್ಕ error = 0;
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
  ಯದಿ(!data->set.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#endif
  result = socket_open(data, &ctx->addr, &ctx->sock);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  ಯದಿ(!data->set.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#endif
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = set_remote_ip(cf, data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

#ifdef USE_IPV6
  ಯದಿ(ctx->addr.family == AF_INET6) {
#ifdef USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    ಪೂರ್ಣಾಙ್ಕ on = 0;
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (ಶೂನ್ಯ *)&on, ಪರಿಮಾಣ(on));
#endif
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      ಸಂರಚನಾ sockaddr_in6 *sa6 = (ಶೂನ್ಯ *)&ctx->addr.curl_sa_addr;
      ಯದಿ(sa6->sin6_scope_id)
        infof(data, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_port,
              (unsigned long)sa6->sin6_scope_id);
      ಅನ್ಯಥಾ
#endif
        infof(data, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#endif
  }
  ಅನ್ಯಥಾ
#endif
    infof(data, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_port);

#ifdef USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#ಅನ್ಯಥಾ
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#endif
  ಯದಿ(is_tcp) {
    ಯದಿ(data->set.tcp_nodelay)
      tcpnodelay(cf, data, ctx->sock);

    ಯದಿ(data->set.tcp_keepalive)
      tcpkeepalive(cf, data, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  ಯದಿ(data->set.fsockopt) {
    /* activate callback for setting socket options */
    ಸಂರಚನಾ Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_END(&guard);

    ಯದಿ(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    ಅನ್ಯಥಾ ಯದಿ(error) {
      result = CURLE_ABORTED_BY_CALLBACK;
      ಗಚ್ಛ out;
    }
  }

#ifndef CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  ಯದಿ(ctx->addr.family == AF_INET
#ifdef USE_IPV6
     || ctx->addr.family == AF_INET6
#endif
    ) {
    result = bindlocal(data, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    ಯದಿ(result) {
      ಯದಿ(result == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        result = CURLE_COULDNT_CONNECT;
      }
      ಗಚ್ಛ out;
    }
  }
#endif

#ifndef CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  ಯದಿ(error < 0) {
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    ಗಚ್ಛ out;
  }
#ಅನ್ಯಥಾ
  ಯದಿ(data->set.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    ಯದಿ(error < 0) {
      result = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      ಗಚ್ಛ out;
    }
  }
#endif
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
out:
  ಯದಿ(result) {
    ಯದಿ(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(isconnected) {
    set_local_ip(cf, data);
    ctx->connected_at = *Curl_pgrs_now(data);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(data, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (ಪೂರ್ಣಾಙ್ಕ)result, ctx->sock);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ do_connect(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                      bool is_tcp_fastopen)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
#ifdef TCP_FASTOPEN_CONNECT
  ಪೂರ್ಣಾಙ್ಕ optval = 1;
#endif
  ಪೂರ್ಣಾಙ್ಕ rc = -1;

  (ಶೂನ್ಯ)data;
  ಯದಿ(is_tcp_fastopen) {
#ifdef CONNECT_DATA_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_AVAILABLE
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    ಯದಿ(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = ರಿಕ್ತ;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_DATA_IDEMPOTENT,
                    ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  ಅನ್ಯಥಾ
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#ನೋಚೇದ್ಯದಿ defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    ಯದಿ(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (ಶೂನ್ಯ *)&optval, ಪರಿಮಾಣ(optval)) < 0)
      CURL_TRC_CF(data, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#ನೋಚೇದ್ಯದಿ defined(MSG_FASTOPEN) /* old Linux */
    ಯದಿ(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    ಅನ್ಯಥಾ
      rc = 0; /* Do nothing */
#endif
  }
  ಅನ್ಯಥಾ {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ CURLcode cf_tcp_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               bool *done)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;
  ಪೂರ್ಣಾಙ್ಕ rc = 0;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *done = FALSE; /* a negative world view is best */
  ಯದಿ(ctx->sock == CURL_SOCKET_BAD) {
    ಪೂರ್ಣಾಙ್ಕ sockerr;

    result = cf_socket_open(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;

    ಯದಿ(cf->connected) {
      *done = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, data, (bool)cf->conn->bits.tcp_fastopen);
    sockerr = SOCKERRNO;
    set_local_ip(cf, data);
    CURL_TRC_CF(data, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_port);
#ifdef DEBUGBUILD
    ಯದಿ(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(data, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#endif
    ಯದಿ(rc == -1) {
      ctx->sockerr = sockerr;
      result = socket_connect_result(data, ctx->ip.remote_ip, sockerr);
      ಗಚ್ಛ out;
    }
  }

#ifdef mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (ಶೂನ್ಯ)verifyconnect(ctx->sock, ರಿಕ್ತ);
#endif
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  ಯದಿ(rc == 0) { /* no connection yet */
    CURL_TRC_CF(data, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(rc == CURL_CSELECT_OUT ||
#ifdef __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#endif
          cf->conn->bits.tcp_fastopen) {
    ಯದಿ(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(data);
      set_local_ip(cf, data);
      *done = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(data, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    result = CURLE_COULDNT_CONNECT;
  }
  ಅನ್ಯಥಾ ಯದಿ(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(data, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (ಶೂನ್ಯ)verifyconnect(ctx->sock, &ctx->sockerr);
    result = CURLE_COULDNT_CONNECT;
  }

out:
  ಯದಿ(result) {
    VERBOSE(ವರ್ಣ buffer[STRERROR_LEN]);
    set_local_ip(cf, data);
    ಯದಿ(ctx->sockerr) {
      data->state.os_errno = ctx->sockerr;
      SET_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, buffer, ಪರಿಮಾಣ(buffer)));
    }
    ಅನ್ಯಥಾ {
      VERBOSE(curlx_strcopy(buffer, ಪರಿಮಾಣ(buffer), STRCONST("peer closed")));
    }
    ಯದಿ(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(data, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_port,
          ctx->ip.local_ip, ctx->ip.local_port,
          curlx_strerror(ctx->sockerr, buffer, ಪರಿಮಾಣ(buffer)));
    *done = FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_socket_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                         ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    ಯದಿ(ctx->listening) {
      result = Curl_pollset_set_in_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ಅನ್ಯಥಾ ಯದಿ(!cf->connected) {
      result = Curl_pollset_set_out_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ಅನ್ಯಥಾ ಯದಿ(!ctx->active) {
      result = Curl_pollset_add_in(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ifndef SIO_IDEAL_SEND_BACKLOG_QUERY
#define SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#endif

ಸ್ಥಿರ ಶೂನ್ಯ win_update_sndbuf_size(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  ಯದಿ(curlx_ptimediff_ms(Curl_pgrs_now(data),
                        &ctx->last_sndbuf_query_at) > 1000) {
    ಯದಿ(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, ಪರಿಮಾಣ(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_size &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (ಧ್ರುವ ವರ್ಣ *)&ideal, ಪರಿಮಾಣ(ideal))) {
      ctx->sndbuf_size = ideal;
    }
    ctx->last_sndbuf_query_at = *Curl_pgrs_now(data);
  }
}

#endif /* USE_WINSOCK */

ಸ್ಥಿರ CURLcode cf_socket_send(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                               ಧ್ರುವ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  ಯದಿ(ctx->wblock_percent > 0) {
    unsigned ವರ್ಣ c = 0;
    Curl_rand_bytes(data, FALSE, &c, 1);
    ಯದಿ(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
  }
  ಯದಿ(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    ಯದಿ(!len)
      len = 1;
    CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#endif

#ಯದಿ defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  ಯದಿ(cf->conn->bits.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->bits.tcp_fastopen = FALSE;
  }
  ಅನ್ಯಥಾ
#endif
    rv = swrite(ctx->sock, buf, len);

  ಯದಿ(!curlx_sztouz(rv, pnwritten)) {
    ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;
    ಯದಿ(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ಅನ್ಯಥಾ {
      ವರ್ಣ buffer[STRERROR_LEN];
      failf(data, "Send failure: %s",
            curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_SEND_ERROR;
    }
  }

#ifdef USE_WINSOCK
  ಯದಿ(!result)
    win_update_sndbuf_size(data, ctx);
#endif

  CURL_TRC_CF(data, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_socket_recv(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                               ವರ್ಣ *buf, size_t len, size_t *pnread)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial reads */
  ಯದಿ(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    unsigned ವರ್ಣ c = 0;
    Curl_rand(data, &c, 1);
    ಯದಿ(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
  }
  ಯದಿ(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#endif

  rv = sread(ctx->sock, buf, len);

  ಯದಿ(!curlx_sztouz(rv, pnread)) {
    ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;
    ಯದಿ(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ಅನ್ಯಥಾ {
      ವರ್ಣ buffer[STRERROR_LEN];
      failf(data, "Recv failure: %s",
            curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(data, cf, "recv(len=%zu) -> %d, %zu", len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread);
  ಯದಿ(!result && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(data);
    ctx->got_first_byte = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_socket_update_data(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  /* Update the IP info held in the transfer, if we have that. */
  ಯದಿ(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
    data->info.primary = ctx->ip;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_socket_active(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  set_local_ip(cf, data);
#ifdef USE_IPV6
  ಯದಿ(cf->sockindex == FIRSTSOCKET)
    cf->conn->bits.ipv6 = (ctx->addr.family == AF_INET6);
#endif
  ctx->active = TRUE;
}

ಸ್ಥಿರ CURLcode cf_socket_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ವಿಕಲ್ಪ(event) {
  ಪಕ್ಷ CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_active(cf, data);
    cf_socket_update_data(cf, data);
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_DATA_SETUP:
    cf_socket_update_data(cf, data);
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_REPORT_STATS:
    ಯದಿ(cf->connected && !ctx->stats_reported) {
      ಸಂರಚನಾ curltime *ts = ರಿಕ್ತ;
      ವಿಕಲ್ಪ(ctx->transport) {
      ಪಕ್ಷ TRNSPRT_UDP:
      ಪಕ್ಷ TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        ಯದಿ(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        ts = &ctx->connected_at;
        ಭಙ್ಗ;
      }
      ಯದಿ(ts) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ bool cf_socket_conn_is_alive(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    bool *ಒಳಹರಿವು_pending)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ pollfd pfd[1];
  ಪೂರ್ಣಾಙ್ಕ r;

  *ಒಳಹರಿವು_pending = FALSE;

  ಯದಿ(!ctx || ctx->sock == CURL_SOCKET_BAD)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  ಯದಿ(r < 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll error, assume dead");
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಅನ್ಯಥಾ ಯದಿ(r == 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll timeout, assume alive");
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(data, cf, "is_alive: err/hup/etc events, assume dead");
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  CURL_TRC_CF(data, cf, "is_alive: valid events, looks alive");
  *ಒಳಹರಿವು_pending = TRUE;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ CURLcode cf_socket_query(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;

  ವಿಕಲ್ಪ(query) {
  ಪಕ್ಷ CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((ಧ್ರುವ ಸಂರಚನಾ Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_CONNECT_REPLY_MS:
    ಯದಿ(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (ಪೂರ್ಣಾಙ್ಕ)ms : INT_MAX;
    }
    ಅನ್ಯಥಾ
      *pres1 = -1;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_IP_INFO:
#ifdef USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#ಅನ್ಯಥಾ
    *pres1 = FALSE;
#endif
    *(ಸಂರಚನಾ ip_quadruple *)pres2 = ctx->ip;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_REALLY_CONNECTED:
    ಯದಿ(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    ಅನ್ಯಥಾ
      *pres1 = ctx->got_first_byte;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_tcp = {
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

CURLcode Curl_cf_tcp_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_peer *origin,
                            ಸಂರಚನಾ Curl_peer *peer,
                            uint8_t transport_peer,
                            ಸಂರಚನಾ connectdata *conn,
                            ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                            ಸಂರಚನಾ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  CURLcode result;

  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  ಯದಿ(!addr) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }

  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

out:
  *pcf = (!result) ? cf : ರಿಕ್ತ;
  ಯದಿ(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef __linux__
ಸ್ಥಿರ ಶೂನ್ಯ linux_quic_mtu(ಸಂರಚನಾ cf_socket_ctx *ctx)
{
  ಪೂರ್ಣಾಙ್ಕ val;
  ವಿಕಲ್ಪ(ctx->addr.family) {
#ifdef IP_MTU_DISCOVER
  ಪಕ್ಷ AF_INET:
    val = IP_PMTUDISC_DO;
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     ಪರಿಮಾಣ(val));
    ಭಙ್ಗ;
#endif
#ifdef IPV6_MTU_DISCOVER
  ಪಕ್ಷ AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     ಪರಿಮಾಣ(val));
    ಭಙ್ಗ;
#endif
  }
}
#ಅನ್ಯಥಾ
#define linux_quic_mtu(x)
#endif

#ಯದಿ defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
ಸ್ಥಿರ ಶೂನ್ಯ linux_quic_gro(ಸಂರಚನಾ cf_socket_ctx *ctx)
{
  ಪೂರ್ಣಾಙ್ಕ one = 1;
  (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)ಪರಿಮಾಣ(one));
}
#ಅನ್ಯಥಾ
#define linux_quic_gro(x)
#endif

#ಯದಿ (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
ಸ್ಥಿರ ಶೂನ್ಯ linux_quic_ecn(ಸಂರಚನಾ cf_socket_ctx *ctx)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ tos = 1;
  ವಿಕಲ್ಪ(ctx->addr.family) {
  ಪಕ್ಷ AF_INET:
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, ಪರಿಮಾಣ(tos));
    ಭಙ್ಗ;
#ifdef IPV6_RECVTCLASS
  ಪಕ್ಷ AF_INET6:
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, ಪರಿಮಾಣ(tos));
    ಭಙ್ಗ;
#endif
  }
}
#ಅನ್ಯಥಾ
#define linux_quic_ecn(x)
#endif

#ಯದಿ (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
ಸ್ಥಿರ ಶೂನ್ಯ linux_ip_dontfrag(ಸಂರಚನಾ cf_socket_ctx *ctx)
{
  ಪೂರ್ಣಾಙ್ಕ val = 1;

  ವಿಕಲ್ಪ(ctx->addr.family) {
  ಪಕ್ಷ AF_INET:
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, ಪರಿಮಾಣ(val));
    ಭಙ್ಗ;
#ifdef IPV6_DONTFRAG
  ಪಕ್ಷ AF_INET6:
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, ಪರಿಮಾಣ(val));
    ಭಙ್ಗ;
#endif
  }
}
#ಅನ್ಯಥಾ
#define linux_ip_dontfrag(x)
#endif

ಸ್ಥಿರ CURLcode cf_udp_setup_quic(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಙ್ಕ rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  ಯದಿ(rc == -1) {
    ಪ್ರತ್ಯಾಗಮ socket_connect_result(data, ctx->ip.remote_ip, SOCKERRNO);
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

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_udp_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               bool *done)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *done = FALSE;
  ಯದಿ(ctx->sock == CURL_SOCKET_BAD) {
    result = cf_socket_open(cf, data);
    ಯದಿ(result) {
      CURL_TRC_CF(data, cf, "cf_udp_connect(), open failed -> %d",
                  (ಪೂರ್ಣಾಙ್ಕ)result);
      ಗಚ್ಛ out;
    }

    ಯದಿ(ctx->transport == TRNSPRT_QUIC) {
      result = cf_udp_setup_quic(cf, data);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      CURL_TRC_CF(data, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
    }
    *done = TRUE;
    cf->connected = TRUE;
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_udp = {
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

CURLcode Curl_cf_udp_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_peer *origin,
                            ಸಂರಚನಾ Curl_peer *peer,
                            uint8_t transport_peer,
                            ಸಂರಚನಾ connectdata *conn,
                            ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                            ಸಂರಚನಾ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  CURLcode result;

  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

out:
  *pcf = (!result) ? cf : ರಿಕ್ತ;
  ಯದಿ(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* this is the TCP filter which can also handle this case */
ಸಂರಚನಾ Curl_cftype Curl_cft_unix = {
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

CURLcode Curl_cf_unix_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_peer *origin,
                            ಸಂರಚನಾ Curl_peer *peer,
                            uint8_t transport_peer,
                            ಸಂರಚನಾ connectdata *conn,
                            ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                            ಸಂರಚನಾ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  CURLcode result;

  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

out:
  *pcf = (!result) ? cf : ರಿಕ್ತ;
  ಯದಿ(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ timediff_t cf_tcp_accept_timeleft(ಸಂರಚನಾ Curl_cfilter *cf,
                                         ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ifndef CURL_DISABLE_FTP
  ಯದಿ(data->set.accepttimeout > 0)
    timeout_ms = data->set.accepttimeout;
#endif

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(data);
  ಯದಿ(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  ಅನ್ಯಥಾ {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->started_at);
    ಯದಿ(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  ಪ್ರತ್ಯಾಗಮ timeout_ms;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_tcp_set_accepted_remote_ip(ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
#ifdef HAVE_GETPEERNAME
  CURLcode result;
  ಸಂರಚನಾ Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  plen = ಪರಿಮಾಣ(ssrem);
  memset(&ssrem, 0, plen);
  ಯದಿ(getpeername(ctx->sock, (ಸಂರಚನಾ sockaddr *)&ssrem, &plen)) {
    ವರ್ಣ buffer[STRERROR_LEN];
    ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;
    failf(data, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, ಪರಿಮಾಣ(buffer)));
    ಪ್ರತ್ಯಾಗಮ;
  }
  result = sockaddr2string((ಸಂರಚನಾ sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_port);
  ಯದಿ(result) {
    failf(data, "ssrem inet_ntop() failed with %d", (ಪೂರ್ಣಾಙ್ಕ)result);
    ಪ್ರತ್ಯಾಗಮ;
  }
#ಅನ್ಯಥಾ
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  (ಶೂನ್ಯ)data;
#endif
}

ಸ್ಥಿರ CURLcode cf_tcp_accept_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      bool *done)
{
  ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;
  ವರ್ಣ errbuf[STRERROR_LEN];
#ifdef USE_IPV6
  ಸಂರಚನಾ Curl_sockaddr_storage add;
#ಅನ್ಯಥಾ
  ಸಂರಚನಾ sockaddr_in add;
#endif
  curl_socklen_t size = (curl_socklen_t)ಪರಿಮಾಣ(add);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  ಪೂರ್ಣಾಙ್ಕ socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (ಶೂನ್ಯ)data;
  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *done = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, data);
  ಯದಿ(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(data, "Accept timeout occurred while waiting server connect");
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(data, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(data, cf, "socket_check -> %x", (unsigned ಪೂರ್ಣಾಙ್ಕ)socketstate);
  ವಿಕಲ್ಪ(socketstate) {
  ಪಕ್ಷ -1: /* error */
    /* let's die here */
    failf(data, "Error while waiting for server connect");
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_ACCEPT_FAILED;
  ಸ್ವಭಾವ:
    ಯದಿ(socketstate & CURL_CSELECT_IN) {
      infof(data, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    ಭಙ್ಗ;
  }

  ಯದಿ(!incoming) {
    CURL_TRC_CF(data, cf, "nothing heard from the server yet");
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  size = ಪರಿಮಾಣ(add);
#ifdef HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (ಸಂರಚನಾ sockaddr *)&add, &size,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#ಅನ್ಯಥಾ
  s_accepted = CURL_ACCEPT(ctx->sock, (ಸಂರಚನಾ sockaddr *)&add, &size);
#endif

  ಯದಿ(s_accepted == CURL_SOCKET_BAD) {
    failf(data, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಪರಿಮಾಣ(errbuf)));
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_ACCEPT_FAILED;
  }
#ifndef HAVE_ACCEPT4
#ifdef HAVE_FCNTL
  ಯದಿ(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಪರಿಮಾಣ(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* HAVE_FCNTL */
  ಯದಿ(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(data, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಪರಿಮಾಣ(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_ACCEPT_FAILED;
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

  ಯದಿ(data->set.fsockopt) {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    ಪೂರ್ಣಾಙ್ಕ error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_END(&guard);

    ಯದಿ(error)
      ಪ್ರತ್ಯಾಗಮ CURLE_ABORTED_BY_CALLBACK;
  }
  *done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_tcp_accept = {
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

CURLcode Curl_conn_tcp_listen_set(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode result;
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_socket_ctx *ctx = ರಿಕ್ತ;

  /* replace any existing */
  Curl_conn_cf_discard_all(data, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  result = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(data);
  conn->sock[sockindex] = ctx->sock;
  set_local_ip(cf, data);
  CURL_TRC_CF(data, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_port);

out:
  ಯದಿ(result) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

bool Curl_conn_is_tcp_listen(ಸಂರಚನಾ Curl_easy *data,
                             int8_t sockindex)
{
  ಸಂರಚನಾ Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ಯಾವತ್(cf) {
    ಯದಿ(cf->cft == &Curl_cft_tcp_accept)
      ಪ್ರತ್ಯಾಗಮ TRUE;
    cf = cf->next;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
ಸ್ಥಿರ bool cf_is_socket(ಸಂರಚನಾ Curl_cfilter *cf)
{
  ಪ್ರತ್ಯಾಗಮ cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             curl_socket_t *psock,
                             ಧ್ರುವ ಸಂರಚನಾ Curl_sockaddr_ex **paddr,
                             ಸಂರಚನಾ ip_quadruple *pip)
{
  (ಶೂನ್ಯ)data;
  ಯದಿ(cf_is_socket(cf) && cf->ctx) {
    ಸಂರಚನಾ cf_socket_ctx *ctx = cf->ctx;

    ಯದಿ(psock)
      *psock = ctx->sock;
    ಯದಿ(paddr)
      *paddr = &ctx->addr;
    ಯದಿ(pip)
      *pip = ctx->ip;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
}
