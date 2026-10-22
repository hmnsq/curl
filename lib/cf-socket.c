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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_SYS_UN_H
#അന്തര്ഭാവ <sys/un.h> /* for sockaddr_un */
#endif
#ifdef HAVE_LINUX_TCP_H
#അന്തര്ഭാവ <linux/tcp.h>
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_NETINET_TCP_H)
#അന്തര്ഭാവ <netinet/tcp.h>
#endif
#ifdef HAVE_NETINET_UDP_H
#അന്തര്ഭാവ <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#അന്തര്ഭാവ <netinet/ip.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#അന്തര്ഭാവ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif

#ifdef HAVE_IFADDRS_H
#അന്തര്ഭാവ <ifaddrs.h>
#endif
#ifdef HAVE_NET_IF_H
#അന്തര്ഭാവ <net/if.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "if2ip.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-socket.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "curlx/inet_ntop.h"
#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "conncache.h"
#അന്തര്ഭാവ "multihandle.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "sockaddr.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curlx/nonblock.h"
#അന്തര്ഭാവ "curlx/strcopy.h"
#അന്തര്ഭാവ "curlx/version_win32.h"
#അന്തര്ഭാവ "curlx/strerr.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "curlx/wait.h"
#ifdef _WIN32
#അന്തര്ഭാവ <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#endif

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(ഘടന sockaddr *sa, curl_socklen_t salen,
                                  അക്ഷരം *addr, uint16_t *port);
UNITTEST CURLcode sockaddr2string(ഘടന sockaddr *sa, curl_socklen_t salen,
                                  അക്ഷരം *addr, uint16_t *port)
{
  CURLcode result;
  ഘടന sockaddr_in *si = ശൂന്യ;
#ifdef USE_IPV6
  ഘടന sockaddr_in6 *si6 = ശൂന്യ;
#endif
#ifdef USE_UNIX_SOCKETS
  ഘടന sockaddr_un *su = ശൂന്യ;
#അല്ലെങ്കിൽ
  (ശൂന്യം)salen;
#endif

  തിരഞ്ഞെടുപ്പ്(sa->sa_family) {
  അവസ്ഥ AF_INET:
    si = (ഘടന sockaddr_in *)(ശൂന്യം *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    എങ്കിൽ(!result) {
      *port = ntohs(si->sin_port);
      തിരികെ result;
    }
    നിർത്തുക;
#ifdef USE_IPV6
  അവസ്ഥ AF_INET6:
    si6 = (ഘടന sockaddr_in6 *)(ശൂന്യം *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    എങ്കിൽ(!result) {
      *port = ntohs(si6->sin6_port);
      തിരികെ result;
    }
    നിർത്തുക;
#endif
#ifdef USE_UNIX_SOCKETS
  അവസ്ഥ AF_UNIX:
    എങ്കിൽ(salen > (curl_socklen_t)വലുപ്പം(CURL_SA_FAMILY_T)) {
      su = (ഘടന sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_path);
    }
    അല്ലെങ്കിൽ
      addr[0] = 0; /* socket with no name */
    *port = 0;
    തിരികെ CURLE_OK;
#endif
  സ്ഥിരസ്ഥിതി:
    result = CURLE_UNSUPPORTED_PROTOCOL;
    നിർത്തുക;
  }

  addr[0] = '\0';
  *port = 0;
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം tcpnodelay(ഘടന Curl_cfilter *cf,
                       ഘടന Curl_easy *data,
                       curl_socket_t sockfd)
{
#എങ്കിൽ defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  പൂർണ്ണസംഖ്യ level = IPPROTO_TCP;
  VERBOSE(അക്ഷരം buffer[STRERROR_LEN]);

  എങ്കിൽ(setsockopt(sockfd, level, TCP_NODELAY, (ശൂന്യം *)&onoff, വലുപ്പം(onoff)) < 0)
    CURL_TRC_CF(data, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, buffer, വലുപ്പം(buffer)));
#അല്ലെങ്കിൽ
  (ശൂന്യം)cf;
  (ശൂന്യം)data;
  (ശൂന്യം)sockfd;
#endif
}

#എങ്കിൽ defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_THRESHOLD) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
#എങ്കിൽ defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
സ്ഥിതം ശൂന്യം alive_unit(പൂർണ്ണസംഖ്യ *x)
{
  /* make sure this doesn't wrap */
  എങ്കിൽ(*x < (INT_MAX / 1000))
    *x *= 1000;
  അല്ലെങ്കിൽ
    /* this is still almost 25 days */
    *x = INT_MAX;
}
#അല്ലെങ്കിൽ
#define alive_unit(x)
#endif
#endif

സ്ഥിതം ശൂന്യം tcpkeepalive(ഘടന Curl_cfilter *cf,
                         ഘടന Curl_easy *data,
                         curl_socket_t sockfd)
{
  പൂർണ്ണസംഖ്യ optval = data->set.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  എങ്കിൽ(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (ശൂന്യം *)&optval, വലുപ്പം(optval)) < 0) {
    CURL_TRC_CF(data, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  അല്ലെങ്കിൽ {
#ifdef USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    എങ്കിൽ(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
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
      എങ്കിൽ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (സ്ഥിരം അക്ഷരം *)&optval, വലുപ്പം(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      എങ്കിൽ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (സ്ഥിരം അക്ഷരം *)&optval, വലുപ്പം(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepcnt);
      എങ്കിൽ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (സ്ഥിരം അക്ഷരം *)&optval, വലുപ്പം(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    അല്ലെങ്കിൽ {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ifndef SIO_KEEPALIVE_VALS
#define SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      ഘടന tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#endif
      ഘടന tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(data->set.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      എങ്കിൽ(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, വലുപ്പം(vals),
                  ശൂന്യ, 0, &dummy, ശൂന്യ, ശൂന്യ) != 0) {
        CURL_TRC_CF(data, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#അല്ലെങ്കിൽ /* !USE_WINSOCK */
#ifdef TCP_KEEPIDLE
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    എങ്കിൽ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (ശൂന്യം *)&optval, വലുപ്പം(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#അല്ലെങ്കിലെങ്കിൽ defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    എങ്കിൽ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (ശൂന്യം *)&optval, വലുപ്പം(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#അല്ലെങ്കിലെങ്കിൽ defined(TCP_KEEPALIVE_THRESHOLD)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    എങ്കിൽ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_THRESHOLD,
                  (ശൂന്യം *)&optval, വലുപ്പം(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPINTVL
    optval = curlx_sltosi(data->set.tcp_keepintvl);
    alive_unit(&optval);
    എങ്കിൽ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (ശൂന്യം *)&optval, വലുപ്പം(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#അല്ലെങ്കിലെങ്കിൽ defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
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
      പൂർണ്ണസംഖ്യ keepcnt = curlx_sltosi(data->set.tcp_keepcnt);
      പൂർണ്ണസംഖ്യ keepintvl = curlx_sltosi(data->set.tcp_keepintvl);

      എങ്കിൽ(keepcnt > 0 && keepintvl > (INT_MAX / keepcnt))
        optval = INT_MAX;
      അല്ലെങ്കിൽ
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    എങ്കിൽ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_THRESHOLD,
                  (ശൂന്യം *)&optval, വലുപ്പം(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPCNT
    optval = curlx_sltosi(data->set.tcp_keepcnt);
    എങ്കിൽ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (ശൂന്യം *)&optval, വലുപ്പം(optval)) < 0) {
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
CURLcode Curl_socket_addr_from_ai(ഘടന Curl_sockaddr_ex *addr,
                                  സ്ഥിരം ഘടന Curl_addrinfo *ai,
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
  addr->addrlen = (unsigned പൂർണ്ണസംഖ്യ)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= വലുപ്പം(addr->curl_sa_addrbuf));
  എങ്കിൽ(addr->addrlen > വലുപ്പം(addr->curl_sa_addrbuf))
    തിരികെ CURLE_TOO_LARGE;

  memcpy(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  തിരികെ CURLE_OK;
}

#ifdef USE_SO_NOSIGPIPE
പൂർണ്ണസംഖ്യ Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  പൂർണ്ണസംഖ്യ onoff = 1;
  തിരികെ setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (ശൂന്യം *)&onoff, വലുപ്പം(onoff));
}
#endif /* USE_SO_NOSIGPIPE */

#എങ്കിൽ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
സ്ഥിതം uint32_t get_scope_id(ഘടന Curl_easy *data,
                             ഘടന sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  എങ്കിൽ(data->conn->scope_id)
    തിരികെ data->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  എങ്കിൽ(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#എങ്കിൽ defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    ഘടന ifaddrs *ifa, *ifa_list;
    എങ്കിൽ(getifaddrs(&ifa_list) == 0) {
      വേണ്ടി(ifa = ifa_list; ifa; ifa = ifa->ifa_next) {
        എങ്കിൽ(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          ഘടന sockaddr_in6 *s6 = (ശൂന്യം *)ifa->ifa_addr;
          എങ്കിൽ(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(data,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (unsigned long)scope_id);
            നിർത്തുക;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#endif /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  തിരികെ scope_id;
}
#endif

സ്ഥിതം CURLcode socket_open(ഘടന Curl_easy *data,
                            ഘടന Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  അക്ഷരം errbuf[STRERROR_LEN];

#ifdef SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#endif

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  എങ്കിൽ(data->set.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    ഘടന Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fopensocket);
    *sockfd = data->set.fopensocket(data->set.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (ഘടന curl_sockaddr *)addr);
    CURL_CBAPI_END(&guard);
  }
  അല്ലെങ്കിൽ {
    /* opensocket callback not set, so create the socket now */
#ifdef DEBUGBUILD
    എങ്കിൽ((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(data, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      തിരികെ CURLE_COULDNT_CONNECT;
    }
#endif
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    എങ്കിൽ((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  എങ്കിൽ(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(data, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, വലുപ്പം(errbuf)));
    തിരികെ CURLE_COULDNT_CONNECT;
  }

#ifdef USE_SO_NOSIGPIPE
  എങ്കിൽ(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(data, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, വലുപ്പം(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    തിരികെ CURLE_COULDNT_CONNECT;
  }
#endif /* USE_SO_NOSIGPIPE */

#എങ്കിൽ defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  എങ്കിൽ(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, വലുപ്പം(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    തിരികെ CURLE_COULDNT_CONNECT;
  }
#endif

#എങ്കിൽ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  എങ്കിൽ(addr->family == AF_INET6) {
    ഘടന sockaddr_in6 * സ്ഥിരം sa6 = (ശൂന്യം *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(data, sa6);
  }
#endif
  തിരികെ CURLE_OK;
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
CURLcode Curl_socket_open(ഘടന Curl_easy *data,
                          സ്ഥിരം ഘടന Curl_addrinfo *ai,
                          ഘടന Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  ഘടന Curl_sockaddr_ex dummy;
  CURLcode result;

  എങ്കിൽ(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  result = Curl_socket_addr_from_ai(addr, ai, transport);
  എങ്കിൽ(result)
    തിരികെ result;

  തിരികെ socket_open(data, addr, sockfd);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ socket_close(ഘടന Curl_easy *data, ഘടന connectdata *conn,
                        പൂർണ്ണസംഖ്യ use_callback, curl_socket_t sock)
{
  എങ്കിൽ(sock == CURL_SOCKET_BAD)
    തിരികെ 0;

  എങ്കിൽ(use_callback && conn && conn->fclosesocket) {
    ഘടന Curl_mapi_guard guard;
    പൂർണ്ണസംഖ്യ rc;
    Curl_multi_will_close(data, sock);
    CURL_CBAPI_START(&guard, data, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_END(&guard);
    തിരികെ rc;
  }

  എങ്കിൽ(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(data, sock);

  sclose(sock);

  തിരികെ 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
പൂർണ്ണസംഖ്യ Curl_socket_close(ഘടന Curl_easy *data, ഘടന connectdata *conn,
                      curl_socket_t sock)
{
  തിരികെ socket_close(data, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(സ്ഥിരം അക്ഷരം *നിവേശം,
                              അക്ഷരം **dev, അക്ഷരം **iface, അക്ഷരം **host)
{
  സ്ഥിതം സ്ഥിരം അക്ഷരം if_prefix[] = "if!";
  സ്ഥിതം സ്ഥിരം അക്ഷരം host_prefix[] = "host!";
  സ്ഥിതം സ്ഥിരം അക്ഷരം if_host_prefix[] = "ifhost!";
  size_t len;

  len = strlen(നിവേശം);
  എങ്കിൽ(len > 512)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(dev)
    *dev = ശൂന്യ;
  എങ്കിൽ(iface)
    *iface = ശൂന്യ;
  എങ്കിൽ(host)
    *host = ശൂന്യ;

  എങ്കിൽ(!strncmp(if_prefix, നിവേശം, strlen(if_prefix))) {
    നിവേശം += strlen(if_prefix);
    എങ്കിൽ(!*നിവേശം)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    എങ്കിൽ(iface) {
      *iface = curlx_memdup0(നിവേശം, len - strlen(if_prefix));
      തിരികെ *iface ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    തിരികെ CURLE_OK;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(host_prefix, നിവേശം, strlen(host_prefix))) {
    നിവേശം += strlen(host_prefix);
    എങ്കിൽ(!*നിവേശം)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    എങ്കിൽ(host) {
      *host = curlx_memdup0(നിവേശം, len - strlen(host_prefix));
      തിരികെ *host ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    തിരികെ CURLE_OK;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(if_host_prefix, നിവേശം, strlen(if_host_prefix))) {
    സ്ഥിരം അക്ഷരം *host_part;
    നിവേശം += strlen(if_host_prefix);
    len -= strlen(if_host_prefix);
    host_part = memchr(നിവേശം, '!', len);
    എങ്കിൽ(!host_part || !*(host_part + 1))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    എങ്കിൽ(iface) {
      *iface = curlx_memdup0(നിവേശം, host_part - നിവേശം);
      എങ്കിൽ(!*iface)
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
    ++host_part;
    എങ്കിൽ(host) {
      *host = curlx_memdup0(host_part, len - (host_part - നിവേശം));
      എങ്കിൽ(!*host) {
        എങ്കിൽ(iface)
          curlx_safefree(*iface);
        തിരികെ CURLE_OUT_OF_MEMORY;
      }
    }
    തിരികെ CURLE_OK;
  }

  എങ്കിൽ(!*നിവേശം)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  എങ്കിൽ(dev) {
    *dev = curlx_memdup0(നിവേശം, len);
    തിരികെ *dev ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  തിരികെ CURLE_OK;
}

#ifndef CURL_DISABLE_BINDLOCAL
സ്ഥിതം CURLcode bindlocal(ഘടന Curl_easy *data, ഘടന connectdata *conn,
                          curl_socket_t sockfd, പൂർണ്ണസംഖ്യ af, unsigned പൂർണ്ണസംഖ്യ scope,
                          uint8_t transport)
{
  ഘടന Curl_sockaddr_storage sa;
  ഘടന sockaddr *sock = (ഘടന sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  ഘടന sockaddr_in *si4 = (ഘടന sockaddr_in *)&sa;
#ifdef USE_IPV6
  ഘടന sockaddr_in6 *si6 = (ഘടന sockaddr_in6 *)&sa;
#endif
  ഘടന Curl_dns_entry *h = ശൂന്യ;
  unsigned short port = data->set.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  പൂർണ്ണസംഖ്യ portnum = data->set.localportrange;
  അക്ഷരം *dev_in = ശൂന്യ, *iface_in = ശൂന്യ, *host_in = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *iface = ശൂന്യ, *host = ശൂന്യ;
  പൂർണ്ണസംഖ്യ sockerr;
#ifdef IP_BIND_ADDRESS_NO_PORT
  പൂർണ്ണസംഖ്യ on = 1;
#endif
  CURLcode result = CURLE_OK;

#ifndef USE_IPV6
  (ശൂന്യം)scope;
#endif
  എങ്കിൽ(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    result = Curl_parse_interface(conn->interface_opts,
                                  &dev_in, &iface_in, &host_in);
    എങ്കിൽ(result)
      പോകുക out;
    /* This is how we use the values */
    iface = iface_in ? iface_in : dev_in;
    host = host_in ? host_in : dev_in;
  }

  എങ്കിൽ(!iface && !host && !port)
    പോകുക out;

  എങ്കിൽ(iface && (strlen(iface) >= 255)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക out;
  }
  memset(&sa, 0, വലുപ്പം(ഘടന Curl_sockaddr_storage));

  എങ്കിൽ(iface || host) {
    അക്ഷരം myhost[256] = "";
    പൂർണ്ണസംഖ്യ done = 0; /* -1 for error, 1 for address found */
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
    എങ്കിൽ(iface &&
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
      പോകുക out;
    }
#endif
    എങ്കിൽ(!host_in) {
      /* Discover IP from input device, then bind to it */
      if2ip_result = Curl_if2ip(af,
#ifdef USE_IPV6
                                scope, conn->scope_id,
#endif
                                iface, myhost, വലുപ്പം(myhost));
    }
    തിരഞ്ഞെടുപ്പ്(if2ip_result) {
    അവസ്ഥ IF2IP_NOT_FOUND:
      എങ്കിൽ(iface_in && !host_in) {
        /* Do not fall back to treating it as a hostname */
        അക്ഷരം buffer[STRERROR_LEN];
        data->state.os_errno = sockerr = SOCKERRNO;
        failf(data, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));
        result = CURLE_INTERFACE_FAILED;
        പോകുക out;
      }
      നിർത്തുക;
    അവസ്ഥ IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      result = CURLE_UNSUPPORTED_PROTOCOL;
      പോകുക out;
    അവസ്ഥ IF2IP_FOUND:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(data, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      done = 1;
      നിർത്തുക;
    }
    എങ്കിൽ(!iface_in || host_in) {
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
      എങ്കിൽ(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#endif

      (ശൂന്യം)Curl_resolv_blocking(data, dns_queries, host, 80, transport, &h);
      എങ്കിൽ(h) {
        പൂർണ്ണസംഖ്യ h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_address(h->addr, myhost, വലുപ്പം(myhost));
        infof(data, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_entry_unlink(data, &h); /* this will NULL, potential free h */
        എങ്കിൽ(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          result = CURLE_UNSUPPORTED_PROTOCOL;
          പോകുക out;
        }
        done = 1;
      }
      അല്ലെങ്കിൽ {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        done = -1;
      }
    }

    എങ്കിൽ(done > 0) {
#ifdef USE_IPV6
      /* IPv6 address */
      എങ്കിൽ(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        അക്ഷരം *scope_ptr = strchr(myhost, '%');
        എങ്കിൽ(scope_ptr)
          *(scope_ptr++) = '\0';
#endif
        എങ്കിൽ(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          എങ്കിൽ(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            എങ്കിൽ(curlx_str_സംഖ്യ((സ്ഥിരം അക്ഷരം **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              result = CURLE_UNSUPPORTED_PROTOCOL;
              പോകുക out;
            }
            si6->sin6_scope_id = (unsigned പൂർണ്ണസംഖ്യ)scope_id;
          }
#endif
        }
        sizeof_sa = വലുപ്പം(ഘടന sockaddr_in6);
      }
      അല്ലെങ്കിൽ
#endif
      /* IPv4 address */
      എങ്കിൽ((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(port);
        sizeof_sa = വലുപ്പം(ഘടന sockaddr_in);
      }
    }

    എങ്കിൽ(done < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      അക്ഷരം buffer[STRERROR_LEN];
      data->state.errorbuf = FALSE;
      data->state.os_errno = sockerr = SOCKERRNO;
      failf(data, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));
      result = CURLE_INTERFACE_FAILED;
      പോകുക out;
    }
  }
  അല്ലെങ്കിൽ {
    /* no device was given, prepare sa to match af's needs */
#ifdef USE_IPV6
    എങ്കിൽ(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(port);
      sizeof_sa = വലുപ്പം(ഘടന sockaddr_in6);
    }
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(port);
      sizeof_sa = വലുപ്പം(ഘടന sockaddr_in);
    }
  }
#ifdef IP_BIND_ADDRESS_NO_PORT
  (ശൂന്യം)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, വലുപ്പം(on));
#endif
  വേണ്ടി(;;) {
    എങ്കിൽ(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(data, "Local port: %hu", port);
      conn->bits.bound = TRUE;
      result = CURLE_OK;
      പോകുക out;
    }

    എങ്കിൽ(--portnum > 0) {
      port++; /* try next port */
      എങ്കിൽ(port == 0)
        നിർത്തുക;
      infof(data, "Bind to local port %d failed, trying next", port - 1);
      /* We reuse/clobber the port variable here below */
      എങ്കിൽ(sock->sa_family == AF_INET)
        si4->sin_port = htons(port);
#ifdef USE_IPV6
      അല്ലെങ്കിൽ
        si6->sin6_port = htons(port);
#endif
    }
    അല്ലെങ്കിൽ
      നിർത്തുക;
  }
  {
    അക്ഷരം buffer[STRERROR_LEN];
    data->state.os_errno = sockerr = SOCKERRNO;
    failf(data, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));
  }

  result = CURLE_INTERFACE_FAILED;

out:
  curlx_free(dev_in);
  curlx_free(iface_in);
  curlx_free(host_in);
  തിരികെ result;
}
#endif

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
സ്ഥിതം bool verifyconnect(curl_socket_t sockfd, പൂർണ്ണസംഖ്യ *psockerr)
{
  bool rc = TRUE;
#ifdef SO_ERROR
  പൂർണ്ണസംഖ്യ sockerr = 0;
  curl_socklen_t errSize = വലുപ്പം(sockerr);

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

  എങ്കിൽ(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (ശൂന്യം *)&sockerr, &errSize))
    sockerr = SOCKERRNO;
#എങ്കിൽ defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  എങ്കിൽ(EBADIOCTL == sockerr) {
    SET_SOCKERRNO(0);
    sockerr = 0;
  }
#endif
  എങ്കിൽ((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  അല്ലെങ്കിൽ
    /* This was not a successful connect */
    rc = FALSE;
  എങ്കിൽ(psockerr)
    *psockerr = sockerr;
#അല്ലെങ്കിൽ
  (ശൂന്യം)sockfd;
  എങ്കിൽ(psockerr)
    *psockerr = SOCKERRNO;
#endif
  തിരികെ rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
സ്ഥിതം CURLcode socket_connect_result(ഘടന Curl_easy *data,
                                      സ്ഥിരം അക്ഷരം *ipaddress, പൂർണ്ണസംഖ്യ sockerr)
{
  എങ്കിൽ(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    തിരികെ CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(അക്ഷരം buffer[STRERROR_LEN]);
    infof(data, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));
    NOVERBOSE((ശൂന്യം)ipaddress);
  }
  data->state.os_errno = sockerr;
  /* connect failed */
  തിരികെ CURLE_COULDNT_CONNECT;
}

ഘടന cf_socket_ctx {
  ഘടന Curl_peer *peer;
  ഘടന Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  ഘടന ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  ഘടന curltime started_at;        /* when socket was created */
  ഘടന curltime connected_at;      /* when socket connected/got first byte */
  ഘടന curltime first_byte_at;     /* when first byte was recvd */
#ifdef USE_WINSOCK
  ഘടന curltime last_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_size;                     /* the last set SO_SNDBUF size */
#endif
  പൂർണ്ണസംഖ്യ sockerr;                       /* socket error of last failure or 0 */
#ifdef DEBUGBUILD
  പൂർണ്ണസംഖ്യ connect_wait_ms;               /* ms to wait after 1st connect() */
  പൂർണ്ണസംഖ്യ wblock_percent;                /* percent of writes doing EAGAIN */
  പൂർണ്ണസംഖ്യ wpartial_percent;              /* percent of bytes written in send */
  പൂർണ്ണസംഖ്യ rblock_percent;                /* percent of reads doing EAGAIN */
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

സ്ഥിതം CURLcode cf_socket_ctx_init(ഘടന cf_socket_ctx *ctx,
                                   ഘടന Curl_peer *peer,
                                   ഘടന Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, വലുപ്പം(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ifdef DEBUGBUILD
  {
    സ്ഥിരം അക്ഷരം *p = getenv("CURL_DBG_SOCK_WBLOCK");
    എങ്കിൽ(p) {
      curl_off_t l;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, 100))
        ctx->wblock_percent = (പൂർണ്ണസംഖ്യ)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    എങ്കിൽ(p) {
      curl_off_t l;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, 100))
        ctx->wpartial_percent = (പൂർണ്ണസംഖ്യ)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    എങ്കിൽ(p) {
      curl_off_t l;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, 100))
        ctx->rblock_percent = (പൂർണ്ണസംഖ്യ)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    എങ്കിൽ(p) {
      curl_off_t l;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    എങ്കിൽ(p) {
      curl_off_t l;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, INT_MAX))
        ctx->connect_wait_ms = (പൂർണ്ണസംഖ്യ)l;
    }
  }
#endif

  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cf_socket_ctx_free(ഘടന cf_socket_ctx *ctx)
{
  എങ്കിൽ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

സ്ഥിതം CURLcode cf_socket_shutdown(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   bool *done)
{
  എങ്കിൽ(cf->connected) {
    ഘടന cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(data, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    എങ്കിൽ(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      unsigned അക്ഷരം buf[1024];
      (ശൂന്യം)sread(ctx->sock, buf, വലുപ്പം(buf));
    }
  }
  *done = TRUE;
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cf_socket_destroy(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  എങ്കിൽ(ctx) {
    എങ്കിൽ(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(data, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      എങ്കിൽ(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(data, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

സ്ഥിതം ശൂന്യം set_local_ip(ഘടന Curl_cfilter *cf,
                         ഘടന Curl_easy *data)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_port = 0;

#ifdef HAVE_GETSOCKNAME
  എങ്കിൽ((ctx->sock != CURL_SOCKET_BAD) &&
     !(data->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    ഘടന Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = വലുപ്പം(ഘടന Curl_sockaddr_storage);

    memset(&ssloc, 0, വലുപ്പം(ssloc));
    എങ്കിൽ(getsockname(ctx->sock, (ഘടന sockaddr *)&ssloc, &slen)) {
      VERBOSE(അക്ഷരം buffer[STRERROR_LEN]);
      VERBOSE(പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO);
      infof(data, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));
    }
    അല്ലെങ്കിൽ {
      CURLcode result = sockaddr2string((ഘടന sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_port);
      എങ്കിൽ(result)
        infof(data, "ssloc inet_ntop() failed with %d", (പൂർണ്ണസംഖ്യ)result);
    }
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
#endif
}

സ്ഥിതം CURLcode set_remote_ip(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  CURLcode result;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  result = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_port);
  എങ്കിൽ(result) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(data, "curl_sa_addr inet_ntop() failed with %d", (പൂർണ്ണസംഖ്യ)result);
    തിരികെ CURLE_FAILED_INIT;
  }
  തിരികെ CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
സ്ഥിതം പൂർണ്ണസംഖ്യ cf_socktype(പൂർണ്ണസംഖ്യ x)
{
#ifdef SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#endif
  തിരികെ x;
}

#ifdef _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ifndef SIO_TCP_INITIAL_RTO
#define SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#define TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
തരനിർവചനം ഘടന _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#endif /* SIO_TCP_INITIAL_RTO */

#ifndef TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#define TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#endif

സ്ഥിതം bool targets_localhost(ഘടന cf_socket_ctx *ctx)
{
  തിരികെ (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
സ്ഥിതം ശൂന്യം tcplocalhost(ഘടന Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  എങ്കിൽ(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD bytes = 0;
    memset(&rto, 0, വലുപ്പം(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    എങ്കിൽ(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    അല്ലെങ്കിൽ
      rto.MaxSynRetransmissions = 1;
    (ശൂന്യം)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, വലുപ്പം(rto),
                   ശൂന്യ, 0, &bytes, ശൂന്യ, ശൂന്യ);
  }
}
#അല്ലെങ്കിൽ
#define tcplocalhost(x, y)
#endif /* _WIN32 */

സ്ഥിതം CURLcode cf_socket_open(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  പൂർണ്ണസംഖ്യ error = 0;
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
  എങ്കിൽ(!data->set.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#endif
  result = socket_open(data, &ctx->addr, &ctx->sock);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  എങ്കിൽ(!data->set.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#endif
  എങ്കിൽ(result)
    പോകുക out;

  result = set_remote_ip(cf, data);
  എങ്കിൽ(result)
    പോകുക out;

#ifdef USE_IPV6
  എങ്കിൽ(ctx->addr.family == AF_INET6) {
#ifdef USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    പൂർണ്ണസംഖ്യ on = 0;
    (ശൂന്യം)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (ശൂന്യം *)&on, വലുപ്പം(on));
#endif
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      ഘടന sockaddr_in6 *sa6 = (ശൂന്യം *)&ctx->addr.curl_sa_addr;
      എങ്കിൽ(sa6->sin6_scope_id)
        infof(data, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_port,
              (unsigned long)sa6->sin6_scope_id);
      അല്ലെങ്കിൽ
#endif
        infof(data, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#endif
  }
  അല്ലെങ്കിൽ
#endif
    infof(data, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_port);

#ifdef USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#അല്ലെങ്കിൽ
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#endif
  എങ്കിൽ(is_tcp) {
    എങ്കിൽ(data->set.tcp_nodelay)
      tcpnodelay(cf, data, ctx->sock);

    എങ്കിൽ(data->set.tcp_keepalive)
      tcpkeepalive(cf, data, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  എങ്കിൽ(data->set.fsockopt) {
    /* activate callback for setting socket options */
    ഘടന Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_END(&guard);

    എങ്കിൽ(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ(error) {
      result = CURLE_ABORTED_BY_CALLBACK;
      പോകുക out;
    }
  }

#ifndef CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  എങ്കിൽ(ctx->addr.family == AF_INET
#ifdef USE_IPV6
     || ctx->addr.family == AF_INET6
#endif
    ) {
    result = bindlocal(data, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    എങ്കിൽ(result) {
      എങ്കിൽ(result == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        result = CURLE_COULDNT_CONNECT;
      }
      പോകുക out;
    }
  }
#endif

#ifndef CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  എങ്കിൽ(error < 0) {
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    പോകുക out;
  }
#അല്ലെങ്കിൽ
  എങ്കിൽ(data->set.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    എങ്കിൽ(error < 0) {
      result = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      പോകുക out;
    }
  }
#endif
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
out:
  എങ്കിൽ(result) {
    എങ്കിൽ(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(isconnected) {
    set_local_ip(cf, data);
    ctx->connected_at = *Curl_pgrs_now(data);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(data, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (പൂർണ്ണസംഖ്യ)result, ctx->sock);
  തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ do_connect(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                      bool is_tcp_fastopen)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
#ifdef TCP_FASTOPEN_CONNECT
  പൂർണ്ണസംഖ്യ optval = 1;
#endif
  പൂർണ്ണസംഖ്യ rc = -1;

  (ശൂന്യം)data;
  എങ്കിൽ(is_tcp_fastopen) {
#ifdef CONNECT_DATA_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_AVAILABLE
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    എങ്കിൽ(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = ശൂന്യ;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_DATA_IDEMPOTENT,
                    ശൂന്യ, 0, ശൂന്യ, ശൂന്യ);
    }
    അല്ലെങ്കിൽ {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  അല്ലെങ്കിൽ
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#അല്ലെങ്കിലെങ്കിൽ defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    എങ്കിൽ(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (ശൂന്യം *)&optval, വലുപ്പം(optval)) < 0)
      CURL_TRC_CF(data, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#അല്ലെങ്കിലെങ്കിൽ defined(MSG_FASTOPEN) /* old Linux */
    എങ്കിൽ(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    അല്ലെങ്കിൽ
      rc = 0; /* Do nothing */
#endif
  }
  അല്ലെങ്കിൽ {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  തിരികെ rc;
}

സ്ഥിതം CURLcode cf_tcp_connect(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               bool *done)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;
  പൂർണ്ണസംഖ്യ rc = 0;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  *done = FALSE; /* a negative world view is best */
  എങ്കിൽ(ctx->sock == CURL_SOCKET_BAD) {
    പൂർണ്ണസംഖ്യ sockerr;

    result = cf_socket_open(cf, data);
    എങ്കിൽ(result)
      പോകുക out;

    എങ്കിൽ(cf->connected) {
      *done = TRUE;
      തിരികെ CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, data, (bool)cf->conn->bits.tcp_fastopen);
    sockerr = SOCKERRNO;
    set_local_ip(cf, data);
    CURL_TRC_CF(data, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_port);
#ifdef DEBUGBUILD
    എങ്കിൽ(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(data, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#endif
    എങ്കിൽ(rc == -1) {
      ctx->sockerr = sockerr;
      result = socket_connect_result(data, ctx->ip.remote_ip, sockerr);
      പോകുക out;
    }
  }

#ifdef mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (ശൂന്യം)verifyconnect(ctx->sock, ശൂന്യ);
#endif
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  എങ്കിൽ(rc == 0) { /* no connection yet */
    CURL_TRC_CF(data, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    തിരികെ CURLE_OK;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(rc == CURL_CSELECT_OUT ||
#ifdef __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#endif
          cf->conn->bits.tcp_fastopen) {
    എങ്കിൽ(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(data);
      set_local_ip(cf, data);
      *done = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(data, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      തിരികെ CURLE_OK;
    }
    result = CURLE_COULDNT_CONNECT;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(data, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (ശൂന്യം)verifyconnect(ctx->sock, &ctx->sockerr);
    result = CURLE_COULDNT_CONNECT;
  }

out:
  എങ്കിൽ(result) {
    VERBOSE(അക്ഷരം buffer[STRERROR_LEN]);
    set_local_ip(cf, data);
    എങ്കിൽ(ctx->sockerr) {
      data->state.os_errno = ctx->sockerr;
      SET_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, buffer, വലുപ്പം(buffer)));
    }
    അല്ലെങ്കിൽ {
      VERBOSE(curlx_strcopy(buffer, വലുപ്പം(buffer), STRCONST("peer closed")));
    }
    എങ്കിൽ(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(data, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_port,
          ctx->ip.local_ip, ctx->ip.local_port,
          curlx_strerror(ctx->sockerr, buffer, വലുപ്പം(buffer)));
    *done = FALSE;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_socket_adjust_pollset(ഘടന Curl_cfilter *cf,
                                         ഘടന Curl_easy *data,
                                         ഘടന easy_pollset *ps)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    എങ്കിൽ(ctx->listening) {
      result = Curl_pollset_set_in_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!cf->connected) {
      result = Curl_pollset_set_out_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!ctx->active) {
      result = Curl_pollset_add_in(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  തിരികെ result;
}

#ifdef USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ifndef SIO_IDEAL_SEND_BACKLOG_QUERY
#define SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#endif

സ്ഥിതം ശൂന്യം win_update_sndbuf_size(ഘടന Curl_easy *data,
                                   ഘടന cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  എങ്കിൽ(curlx_ptimediff_ms(Curl_pgrs_now(data),
                        &ctx->last_sndbuf_query_at) > 1000) {
    എങ്കിൽ(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, വലുപ്പം(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_size &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (സ്ഥിരം അക്ഷരം *)&ideal, വലുപ്പം(ideal))) {
      ctx->sndbuf_size = ideal;
    }
    ctx->last_sndbuf_query_at = *Curl_pgrs_now(data);
  }
}

#endif /* USE_WINSOCK */

സ്ഥിതം CURLcode cf_socket_send(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                               സ്ഥിരം uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (ശൂന്യം)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  എങ്കിൽ(ctx->wblock_percent > 0) {
    unsigned അക്ഷരം c = 0;
    Curl_rand_bytes(data, FALSE, &c, 1);
    എങ്കിൽ(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      തിരികെ CURLE_AGAIN;
    }
  }
  എങ്കിൽ(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    എങ്കിൽ(!len)
      len = 1;
    CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#endif

#എങ്കിൽ defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  എങ്കിൽ(cf->conn->bits.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->bits.tcp_fastopen = FALSE;
  }
  അല്ലെങ്കിൽ
#endif
    rv = swrite(ctx->sock, buf, len);

  എങ്കിൽ(!curlx_sztouz(rv, pnwritten)) {
    പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;
    എങ്കിൽ(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    അല്ലെങ്കിൽ {
      അക്ഷരം buffer[STRERROR_LEN];
      failf(data, "Send failure: %s",
            curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_SEND_ERROR;
    }
  }

#ifdef USE_WINSOCK
  എങ്കിൽ(!result)
    win_update_sndbuf_size(data, ctx);
#endif

  CURL_TRC_CF(data, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (പൂർണ്ണസംഖ്യ)result, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_socket_recv(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                               അക്ഷരം *buf, size_t len, size_t *pnread)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial reads */
  എങ്കിൽ(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    unsigned അക്ഷരം c = 0;
    Curl_rand(data, &c, 1);
    എങ്കിൽ(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      തിരികെ CURLE_AGAIN;
    }
  }
  എങ്കിൽ(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#endif

  rv = sread(ctx->sock, buf, len);

  എങ്കിൽ(!curlx_sztouz(rv, pnread)) {
    പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;
    എങ്കിൽ(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    അല്ലെങ്കിൽ {
      അക്ഷരം buffer[STRERROR_LEN];
      failf(data, "Recv failure: %s",
            curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(data, cf, "recv(len=%zu) -> %d, %zu", len, (പൂർണ്ണസംഖ്യ)result, *pnread);
  എങ്കിൽ(!result && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(data);
    ctx->got_first_byte = TRUE;
  }
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_socket_update_data(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data)
{
  /* Update the IP info held in the transfer, if we have that. */
  എങ്കിൽ(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    ഘടന cf_socket_ctx *ctx = cf->ctx;
    data->info.primary = ctx->ip;
  }
}

സ്ഥിതം ശൂന്യം cf_socket_active(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  set_local_ip(cf, data);
#ifdef USE_IPV6
  എങ്കിൽ(cf->sockindex == FIRSTSOCKET)
    cf->conn->bits.ipv6 = (ctx->addr.family == AF_INET6);
#endif
  ctx->active = TRUE;
}

സ്ഥിതം CURLcode cf_socket_cntrl(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;

  (ശൂന്യം)arg1;
  (ശൂന്യം)arg2;
  തിരഞ്ഞെടുപ്പ്(event) {
  അവസ്ഥ CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_active(cf, data);
    cf_socket_update_data(cf, data);
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_DATA_SETUP:
    cf_socket_update_data(cf, data);
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_REPORT_STATS:
    എങ്കിൽ(cf->connected && !ctx->stats_reported) {
      ഘടന curltime *ts = ശൂന്യ;
      തിരഞ്ഞെടുപ്പ്(ctx->transport) {
      അവസ്ഥ TRNSPRT_UDP:
      അവസ്ഥ TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        എങ്കിൽ(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        ts = &ctx->connected_at;
        നിർത്തുക;
      }
      എങ്കിൽ(ts) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം bool cf_socket_conn_is_alive(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    bool *നിവേശം_pending)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  ഘടന pollfd pfd[1];
  പൂർണ്ണസംഖ്യ r;

  *നിവേശം_pending = FALSE;

  എങ്കിൽ(!ctx || ctx->sock == CURL_SOCKET_BAD)
    തിരികെ FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  എങ്കിൽ(r < 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll error, assume dead");
    തിരികെ FALSE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(r == 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll timeout, assume alive");
    തിരികെ TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(data, cf, "is_alive: err/hup/etc events, assume dead");
    തിരികെ FALSE;
  }

  CURL_TRC_CF(data, cf, "is_alive: valid events, looks alive");
  *നിവേശം_pending = TRUE;
  തിരികെ TRUE;
}

സ്ഥിതം CURLcode cf_socket_query(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;

  തിരഞ്ഞെടുപ്പ്(query) {
  അവസ്ഥ CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((സ്ഥിരം ഘടന Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : ശൂന്യ;
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_CONNECT_REPLY_MS:
    എങ്കിൽ(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (പൂർണ്ണസംഖ്യ)ms : INT_MAX;
    }
    അല്ലെങ്കിൽ
      *pres1 = -1;
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_IP_INFO:
#ifdef USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#അല്ലെങ്കിൽ
    *pres1 = FALSE;
#endif
    *(ഘടന ip_quadruple *)pres2 = ctx->ip;
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_REALLY_CONNECTED:
    എങ്കിൽ(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    അല്ലെങ്കിൽ
      *pres1 = ctx->got_first_byte;
    തിരികെ CURLE_OK;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ഘടന Curl_cftype Curl_cft_tcp = {
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

CURLcode Curl_cf_tcp_create(ഘടന Curl_cfilter **pcf,
                            ഘടന Curl_easy *data,
                            ഘടന Curl_peer *origin,
                            ഘടന Curl_peer *peer,
                            uint8_t transport_peer,
                            ഘടന connectdata *conn,
                            ഘടന Curl_sockaddr_ex *addr,
                            ഘടന Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ഘടന cf_socket_ctx *ctx = ശൂന്യ;
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  CURLcode result;

  (ശൂന്യം)data;
  (ശൂന്യം)origin;
  (ശൂന്യം)conn;
  (ശൂന്യം)tunnel_peer;
  (ശൂന്യം)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  എങ്കിൽ(!addr) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക out;
  }

  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

out:
  *pcf = (!result) ? cf : ശൂന്യ;
  എങ്കിൽ(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  തിരികെ result;
}

#ifdef __linux__
സ്ഥിതം ശൂന്യം linux_quic_mtu(ഘടന cf_socket_ctx *ctx)
{
  പൂർണ്ണസംഖ്യ val;
  തിരഞ്ഞെടുപ്പ്(ctx->addr.family) {
#ifdef IP_MTU_DISCOVER
  അവസ്ഥ AF_INET:
    val = IP_PMTUDISC_DO;
    (ശൂന്യം)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     വലുപ്പം(val));
    നിർത്തുക;
#endif
#ifdef IPV6_MTU_DISCOVER
  അവസ്ഥ AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (ശൂന്യം)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     വലുപ്പം(val));
    നിർത്തുക;
#endif
  }
}
#അല്ലെങ്കിൽ
#define linux_quic_mtu(x)
#endif

#എങ്കിൽ defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
സ്ഥിതം ശൂന്യം linux_quic_gro(ഘടന cf_socket_ctx *ctx)
{
  പൂർണ്ണസംഖ്യ one = 1;
  (ശൂന്യം)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)വലുപ്പം(one));
}
#അല്ലെങ്കിൽ
#define linux_quic_gro(x)
#endif

#എങ്കിൽ (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
സ്ഥിതം ശൂന്യം linux_quic_ecn(ഘടന cf_socket_ctx *ctx)
{
  unsigned പൂർണ്ണസംഖ്യ tos = 1;
  തിരഞ്ഞെടുപ്പ്(ctx->addr.family) {
  അവസ്ഥ AF_INET:
    (ശൂന്യം)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, വലുപ്പം(tos));
    നിർത്തുക;
#ifdef IPV6_RECVTCLASS
  അവസ്ഥ AF_INET6:
    (ശൂന്യം)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, വലുപ്പം(tos));
    നിർത്തുക;
#endif
  }
}
#അല്ലെങ്കിൽ
#define linux_quic_ecn(x)
#endif

#എങ്കിൽ (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
സ്ഥിതം ശൂന്യം linux_ip_dontfrag(ഘടന cf_socket_ctx *ctx)
{
  പൂർണ്ണസംഖ്യ val = 1;

  തിരഞ്ഞെടുപ്പ്(ctx->addr.family) {
  അവസ്ഥ AF_INET:
    (ശൂന്യം)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, വലുപ്പം(val));
    നിർത്തുക;
#ifdef IPV6_DONTFRAG
  അവസ്ഥ AF_INET6:
    (ശൂന്യം)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, വലുപ്പം(val));
    നിർത്തുക;
#endif
  }
}
#അല്ലെങ്കിൽ
#define linux_ip_dontfrag(x)
#endif

സ്ഥിതം CURLcode cf_udp_setup_quic(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  പൂർണ്ണസംഖ്യ rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  എങ്കിൽ(rc == -1) {
    തിരികെ socket_connect_result(data, ctx->ip.remote_ip, SOCKERRNO);
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

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_udp_connect(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               bool *done)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  *done = FALSE;
  എങ്കിൽ(ctx->sock == CURL_SOCKET_BAD) {
    result = cf_socket_open(cf, data);
    എങ്കിൽ(result) {
      CURL_TRC_CF(data, cf, "cf_udp_connect(), open failed -> %d",
                  (പൂർണ്ണസംഖ്യ)result);
      പോകുക out;
    }

    എങ്കിൽ(ctx->transport == TRNSPRT_QUIC) {
      result = cf_udp_setup_quic(cf, data);
      എങ്കിൽ(result)
        പോകുക out;
      CURL_TRC_CF(data, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
    }
    *done = TRUE;
    cf->connected = TRUE;
  }
out:
  തിരികെ result;
}

ഘടന Curl_cftype Curl_cft_udp = {
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

CURLcode Curl_cf_udp_create(ഘടന Curl_cfilter **pcf,
                            ഘടന Curl_easy *data,
                            ഘടന Curl_peer *origin,
                            ഘടന Curl_peer *peer,
                            uint8_t transport_peer,
                            ഘടന connectdata *conn,
                            ഘടന Curl_sockaddr_ex *addr,
                            ഘടന Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ഘടന cf_socket_ctx *ctx = ശൂന്യ;
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  CURLcode result;

  (ശൂന്യം)data;
  (ശൂന്യം)origin;
  (ശൂന്യം)conn;
  (ശൂന്യം)tunnel_peer;
  (ശൂന്യം)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

out:
  *pcf = (!result) ? cf : ശൂന്യ;
  എങ്കിൽ(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  തിരികെ result;
}

/* this is the TCP filter which can also handle this case */
ഘടന Curl_cftype Curl_cft_unix = {
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

CURLcode Curl_cf_unix_create(ഘടന Curl_cfilter **pcf,
                            ഘടന Curl_easy *data,
                            ഘടന Curl_peer *origin,
                            ഘടന Curl_peer *peer,
                            uint8_t transport_peer,
                            ഘടന connectdata *conn,
                            ഘടന Curl_sockaddr_ex *addr,
                            ഘടന Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ഘടന cf_socket_ctx *ctx = ശൂന്യ;
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  CURLcode result;

  (ശൂന്യം)data;
  (ശൂന്യം)origin;
  (ശൂന്യം)conn;
  (ശൂന്യം)tunnel_peer;
  (ശൂന്യം)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

out:
  *pcf = (!result) ? cf : ശൂന്യ;
  എങ്കിൽ(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  തിരികെ result;
}

സ്ഥിതം timediff_t cf_tcp_accept_timeleft(ഘടന Curl_cfilter *cf,
                                         ഘടന Curl_easy *data)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ifndef CURL_DISABLE_FTP
  എങ്കിൽ(data->set.accepttimeout > 0)
    timeout_ms = data->set.accepttimeout;
#endif

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(data);
  എങ്കിൽ(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  അല്ലെങ്കിൽ {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->started_at);
    എങ്കിൽ(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  തിരികെ timeout_ms;
}

സ്ഥിതം ശൂന്യം cf_tcp_set_accepted_remote_ip(ഘടന Curl_cfilter *cf,
                                          ഘടന Curl_easy *data)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
#ifdef HAVE_GETPEERNAME
  CURLcode result;
  ഘടന Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  plen = വലുപ്പം(ssrem);
  memset(&ssrem, 0, plen);
  എങ്കിൽ(getpeername(ctx->sock, (ഘടന sockaddr *)&ssrem, &plen)) {
    അക്ഷരം buffer[STRERROR_LEN];
    പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;
    failf(data, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, വലുപ്പം(buffer)));
    തിരികെ;
  }
  result = sockaddr2string((ഘടന sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_port);
  എങ്കിൽ(result) {
    failf(data, "ssrem inet_ntop() failed with %d", (പൂർണ്ണസംഖ്യ)result);
    തിരികെ;
  }
#അല്ലെങ്കിൽ
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  (ശൂന്യം)data;
#endif
}

സ്ഥിതം CURLcode cf_tcp_accept_connect(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      bool *done)
{
  ഘടന cf_socket_ctx *ctx = cf->ctx;
  അക്ഷരം errbuf[STRERROR_LEN];
#ifdef USE_IPV6
  ഘടന Curl_sockaddr_storage add;
#അല്ലെങ്കിൽ
  ഘടന sockaddr_in add;
#endif
  curl_socklen_t size = (curl_socklen_t)വലുപ്പം(add);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  പൂർണ്ണസംഖ്യ socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (ശൂന്യം)data;
  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  *done = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, data);
  എങ്കിൽ(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(data, "Accept timeout occurred while waiting server connect");
    തിരികെ CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(data, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(data, cf, "socket_check -> %x", (unsigned പൂർണ്ണസംഖ്യ)socketstate);
  തിരഞ്ഞെടുപ്പ്(socketstate) {
  അവസ്ഥ -1: /* error */
    /* let's die here */
    failf(data, "Error while waiting for server connect");
    തിരികെ CURLE_FTP_ACCEPT_FAILED;
  സ്ഥിരസ്ഥിതി:
    എങ്കിൽ(socketstate & CURL_CSELECT_IN) {
      infof(data, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    നിർത്തുക;
  }

  എങ്കിൽ(!incoming) {
    CURL_TRC_CF(data, cf, "nothing heard from the server yet");
    തിരികെ CURLE_OK;
  }

  size = വലുപ്പം(add);
#ifdef HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (ഘടന sockaddr *)&add, &size,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#അല്ലെങ്കിൽ
  s_accepted = CURL_ACCEPT(ctx->sock, (ഘടന sockaddr *)&add, &size);
#endif

  എങ്കിൽ(s_accepted == CURL_SOCKET_BAD) {
    failf(data, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, വലുപ്പം(errbuf)));
    തിരികെ CURLE_FTP_ACCEPT_FAILED;
  }
#ifndef HAVE_ACCEPT4
#ifdef HAVE_FCNTL
  എങ്കിൽ(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, വലുപ്പം(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    തിരികെ CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* HAVE_FCNTL */
  എങ്കിൽ(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(data, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, വലുപ്പം(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    തിരികെ CURLE_FTP_ACCEPT_FAILED;
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

  എങ്കിൽ(data->set.fsockopt) {
    ഘടന Curl_mapi_guard guard;
    പൂർണ്ണസംഖ്യ error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_END(&guard);

    എങ്കിൽ(error)
      തിരികെ CURLE_ABORTED_BY_CALLBACK;
  }
  *done = TRUE;
  തിരികെ CURLE_OK;
}

ഘടന Curl_cftype Curl_cft_tcp_accept = {
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

CURLcode Curl_conn_tcp_listen_set(ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode result;
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന cf_socket_ctx *ctx = ശൂന്യ;

  /* replace any existing */
  Curl_conn_cf_discard_all(data, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  result = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(data);
  conn->sock[sockindex] = ctx->sock;
  set_local_ip(cf, data);
  CURL_TRC_CF(data, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_port);

out:
  എങ്കിൽ(result) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  തിരികെ result;
}

bool Curl_conn_is_tcp_listen(ഘടന Curl_easy *data,
                             int8_t sockindex)
{
  ഘടന Curl_cfilter *cf = data->conn->cfilter[sockindex];
  ആകുമ്പോൾ(cf) {
    എങ്കിൽ(cf->cft == &Curl_cft_tcp_accept)
      തിരികെ TRUE;
    cf = cf->next;
  }
  തിരികെ FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
സ്ഥിതം bool cf_is_socket(ഘടന Curl_cfilter *cf)
{
  തിരികെ cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             curl_socket_t *psock,
                             സ്ഥിരം ഘടന Curl_sockaddr_ex **paddr,
                             ഘടന ip_quadruple *pip)
{
  (ശൂന്യം)data;
  എങ്കിൽ(cf_is_socket(cf) && cf->ctx) {
    ഘടന cf_socket_ctx *ctx = cf->ctx;

    എങ്കിൽ(psock)
      *psock = ctx->sock;
    എങ്കിൽ(paddr)
      *paddr = &ctx->addr;
    എങ്കിൽ(pip)
      *pip = ctx->ip;
    തിരികെ CURLE_OK;
  }
  തിരികെ CURLE_FAILED_INIT;
}
