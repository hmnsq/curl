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
#ضمن "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_SYS_UN_H
#ضمن <sys/un.h> /* for sockaddr_un */
#endif
#ifdef HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#endif
#ifdef HAVE_NETINET_UDP_H
#ضمن <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#ضمن <netinet/ip.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif

#ifdef HAVE_IFADDRS_H
#ضمن <ifaddrs.h>
#endif
#ifdef HAVE_NET_IF_H
#ضمن <net/if.h>
#endif
#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "if2ip.h"
#ضمن "cfilters.h"
#ضمن "cf-socket.h"
#ضمن "connect.h"
#ضمن "curl_addrinfo.h"
#ضمن "select.h"
#ضمن "multiif.h"
#ضمن "curlx/inet_ntop.h"
#ضمن "curlx/inet_pton.h"
#ضمن "progress.h"
#ضمن "conncache.h"
#ضمن "multihandle.h"
#ضمن "rand.h"
#ضمن "sockaddr.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/nonblock.h"
#ضمن "curlx/strcopy.h"
#ضمن "curlx/version_win32.h"
#ضمن "curlx/strerr.h"
#ضمن "curlx/strparse.h"
#ضمن "curlx/wait.h"
#ifdef _WIN32
#ضمن <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#endif

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(بنية sockaddr *sa, curl_socklen_t salen,
                                  حرف *addr, uint16_t *port);
UNITTEST CURLcode sockaddr2string(بنية sockaddr *sa, curl_socklen_t salen,
                                  حرف *addr, uint16_t *port)
{
  CURLcode result;
  بنية sockaddr_in *si = عدم;
#ifdef USE_IPV6
  بنية sockaddr_in6 *si6 = عدم;
#endif
#ifdef USE_UNIX_SOCKETS
  بنية sockaddr_un *su = عدم;
#وإلا
  (فراغ)salen;
#endif

  بدل(sa->sa_family) {
  حالة AF_INET:
    si = (بنية sockaddr_in *)(فراغ *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    إذا(!result) {
      *port = ntohs(si->sin_port);
      أرجع result;
    }
    اكسر;
#ifdef USE_IPV6
  حالة AF_INET6:
    si6 = (بنية sockaddr_in6 *)(فراغ *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    إذا(!result) {
      *port = ntohs(si6->sin6_port);
      أرجع result;
    }
    اكسر;
#endif
#ifdef USE_UNIX_SOCKETS
  حالة AF_UNIX:
    إذا(salen > (curl_socklen_t)حجم(CURL_SA_FAMILY_T)) {
      su = (بنية sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_path);
    }
    وإلا
      addr[0] = 0; /* socket with no name */
    *port = 0;
    أرجع CURLE_OK;
#endif
  افتراضي:
    result = CURLE_UNSUPPORTED_PROTOCOL;
    اكسر;
  }

  addr[0] = '\0';
  *port = 0;
  أرجع result;
}

ساكن فراغ tcpnodelay(بنية Curl_cfilter *cf,
                       بنية Curl_easy *data,
                       curl_socket_t sockfd)
{
#إذا defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  صحيح level = IPPROTO_TCP;
  VERBOSE(حرف buffer[STRERROR_LEN]);

  إذا(setsockopt(sockfd, level, TCP_NODELAY, (فراغ *)&onoff, حجم(onoff)) < 0)
    CURL_TRC_CF(data, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, buffer, حجم(buffer)));
#وإلا
  (فراغ)cf;
  (فراغ)data;
  (فراغ)sockfd;
#endif
}

#إذا defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_THRESHOLD) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
#إذا defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
ساكن فراغ alive_unit(صحيح *x)
{
  /* make sure this doesn't wrap */
  إذا(*x < (INT_MAX / 1000))
    *x *= 1000;
  وإلا
    /* this is still almost 25 days */
    *x = INT_MAX;
}
#وإلا
#define alive_unit(x)
#endif
#endif

ساكن فراغ tcpkeepalive(بنية Curl_cfilter *cf,
                         بنية Curl_easy *data,
                         curl_socket_t sockfd)
{
  صحيح optval = data->set.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  إذا(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (فراغ *)&optval, حجم(optval)) < 0) {
    CURL_TRC_CF(data, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  وإلا {
#ifdef USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    إذا(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
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
      إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (ثابت حرف *)&optval, حجم(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (ثابت حرف *)&optval, حجم(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepcnt);
      إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (ثابت حرف *)&optval, حجم(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    وإلا {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ifndef SIO_KEEPALIVE_VALS
#define SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      بنية tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#endif
      بنية tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(data->set.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      إذا(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, حجم(vals),
                  عدم, 0, &dummy, عدم, عدم) != 0) {
        CURL_TRC_CF(data, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#وإلا /* !USE_WINSOCK */
#ifdef TCP_KEEPIDLE
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (فراغ *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وإلاإذا defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (فراغ *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وإلاإذا defined(TCP_KEEPALIVE_THRESHOLD)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_THRESHOLD,
                  (فراغ *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPINTVL
    optval = curlx_sltosi(data->set.tcp_keepintvl);
    alive_unit(&optval);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (فراغ *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وإلاإذا defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
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
      صحيح keepcnt = curlx_sltosi(data->set.tcp_keepcnt);
      صحيح keepintvl = curlx_sltosi(data->set.tcp_keepintvl);

      إذا(keepcnt > 0 && keepintvl > (INT_MAX / keepcnt))
        optval = INT_MAX;
      وإلا
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_THRESHOLD,
                  (فراغ *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPCNT
    optval = curlx_sltosi(data->set.tcp_keepcnt);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (فراغ *)&optval, حجم(optval)) < 0) {
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
CURLcode Curl_socket_addr_from_ai(بنية Curl_sockaddr_ex *addr,
                                  ثابت بنية Curl_addrinfo *ai,
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
  addr->addrlen = (unsigned صحيح)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= حجم(addr->curl_sa_addrbuf));
  إذا(addr->addrlen > حجم(addr->curl_sa_addrbuf))
    أرجع CURLE_TOO_LARGE;

  memcpy(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  أرجع CURLE_OK;
}

#ifdef USE_SO_NOSIGPIPE
صحيح Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  صحيح onoff = 1;
  أرجع setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (فراغ *)&onoff, حجم(onoff));
}
#endif /* USE_SO_NOSIGPIPE */

#إذا defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ساكن uint32_t get_scope_id(بنية Curl_easy *data,
                             بنية sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  إذا(data->conn->scope_id)
    أرجع data->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  إذا(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#إذا defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    بنية ifaddrs *ifa, *ifa_list;
    إذا(getifaddrs(&ifa_list) == 0) {
      لكل(ifa = ifa_list; ifa; ifa = ifa->ifa_next) {
        إذا(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          بنية sockaddr_in6 *s6 = (فراغ *)ifa->ifa_addr;
          إذا(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(data,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (unsigned long)scope_id);
            اكسر;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#endif /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  أرجع scope_id;
}
#endif

ساكن CURLcode socket_open(بنية Curl_easy *data,
                            بنية Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  حرف errbuf[STRERROR_LEN];

#ifdef SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#endif

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  إذا(data->set.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fopensocket);
    *sockfd = data->set.fopensocket(data->set.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (بنية curl_sockaddr *)addr);
    CURL_CBAPI_END(&guard);
  }
  وإلا {
    /* opensocket callback not set, so create the socket now */
#ifdef DEBUGBUILD
    إذا((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(data, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      أرجع CURLE_COULDNT_CONNECT;
    }
#endif
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    إذا((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      أرجع CURLE_OUT_OF_MEMORY;
  }

  إذا(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(data, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    أرجع CURLE_COULDNT_CONNECT;
  }

#ifdef USE_SO_NOSIGPIPE
  إذا(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(data, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    أرجع CURLE_COULDNT_CONNECT;
  }
#endif /* USE_SO_NOSIGPIPE */

#إذا defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  إذا(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    أرجع CURLE_COULDNT_CONNECT;
  }
#endif

#إذا defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  إذا(addr->family == AF_INET6) {
    بنية sockaddr_in6 * ثابت sa6 = (فراغ *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(data, sa6);
  }
#endif
  أرجع CURLE_OK;
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
CURLcode Curl_socket_open(بنية Curl_easy *data,
                          ثابت بنية Curl_addrinfo *ai,
                          بنية Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  بنية Curl_sockaddr_ex dummy;
  CURLcode result;

  إذا(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  result = Curl_socket_addr_from_ai(addr, ai, transport);
  إذا(result)
    أرجع result;

  أرجع socket_open(data, addr, sockfd);
}

ساكن صحيح socket_close(بنية Curl_easy *data, بنية connectdata *conn,
                        صحيح use_callback, curl_socket_t sock)
{
  إذا(sock == CURL_SOCKET_BAD)
    أرجع 0;

  إذا(use_callback && conn && conn->fclosesocket) {
    بنية Curl_mapi_guard guard;
    صحيح rc;
    Curl_multi_will_close(data, sock);
    CURL_CBAPI_START(&guard, data, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_END(&guard);
    أرجع rc;
  }

  إذا(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(data, sock);

  sclose(sock);

  أرجع 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
صحيح Curl_socket_close(بنية Curl_easy *data, بنية connectdata *conn,
                      curl_socket_t sock)
{
  أرجع socket_close(data, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(ثابت حرف *ماسوقن,
                              حرف **dev, حرف **iface, حرف **host)
{
  ساكن ثابت حرف if_prefix[] = "if!";
  ساكن ثابت حرف host_prefix[] = "host!";
  ساكن ثابت حرف if_host_prefix[] = "ifhost!";
  size_t len;

  len = strlen(ماسوقن);
  إذا(len > 512)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  إذا(dev)
    *dev = عدم;
  إذا(iface)
    *iface = عدم;
  إذا(host)
    *host = عدم;

  إذا(!strncmp(if_prefix, ماسوقن, strlen(if_prefix))) {
    ماسوقن += strlen(if_prefix);
    إذا(!*ماسوقن)
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    إذا(iface) {
      *iface = curlx_memdup0(ماسوقن, len - strlen(if_prefix));
      أرجع *iface ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    أرجع CURLE_OK;
  }
  وإلا إذا(!strncmp(host_prefix, ماسوقن, strlen(host_prefix))) {
    ماسوقن += strlen(host_prefix);
    إذا(!*ماسوقن)
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    إذا(host) {
      *host = curlx_memdup0(ماسوقن, len - strlen(host_prefix));
      أرجع *host ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    أرجع CURLE_OK;
  }
  وإلا إذا(!strncmp(if_host_prefix, ماسوقن, strlen(if_host_prefix))) {
    ثابت حرف *host_part;
    ماسوقن += strlen(if_host_prefix);
    len -= strlen(if_host_prefix);
    host_part = memchr(ماسوقن, '!', len);
    إذا(!host_part || !*(host_part + 1))
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    إذا(iface) {
      *iface = curlx_memdup0(ماسوقن, host_part - ماسوقن);
      إذا(!*iface)
        أرجع CURLE_OUT_OF_MEMORY;
    }
    ++host_part;
    إذا(host) {
      *host = curlx_memdup0(host_part, len - (host_part - ماسوقن));
      إذا(!*host) {
        إذا(iface)
          curlx_safefree(*iface);
        أرجع CURLE_OUT_OF_MEMORY;
      }
    }
    أرجع CURLE_OK;
  }

  إذا(!*ماسوقن)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  إذا(dev) {
    *dev = curlx_memdup0(ماسوقن, len);
    أرجع *dev ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  أرجع CURLE_OK;
}

#ifndef CURL_DISABLE_BINDLOCAL
ساكن CURLcode bindlocal(بنية Curl_easy *data, بنية connectdata *conn,
                          curl_socket_t sockfd, صحيح af, unsigned صحيح scope,
                          uint8_t transport)
{
  بنية Curl_sockaddr_storage sa;
  بنية sockaddr *sock = (بنية sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  بنية sockaddr_in *si4 = (بنية sockaddr_in *)&sa;
#ifdef USE_IPV6
  بنية sockaddr_in6 *si6 = (بنية sockaddr_in6 *)&sa;
#endif
  بنية Curl_dns_entry *h = عدم;
  unsigned short port = data->set.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  صحيح portnum = data->set.localportrange;
  حرف *dev_in = عدم, *iface_in = عدم, *host_in = عدم;
  ثابت حرف *iface = عدم, *host = عدم;
  صحيح sockerr;
#ifdef IP_BIND_ADDRESS_NO_PORT
  صحيح on = 1;
#endif
  CURLcode result = CURLE_OK;

#ifndef USE_IPV6
  (فراغ)scope;
#endif
  إذا(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    result = Curl_parse_interface(conn->interface_opts,
                                  &dev_in, &iface_in, &host_in);
    إذا(result)
      اذهب out;
    /* This is how we use the values */
    iface = iface_in ? iface_in : dev_in;
    host = host_in ? host_in : dev_in;
  }

  إذا(!iface && !host && !port)
    اذهب out;

  إذا(iface && (strlen(iface) >= 255)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }
  memset(&sa, 0, حجم(بنية Curl_sockaddr_storage));

  إذا(iface || host) {
    حرف myhost[256] = "";
    صحيح done = 0; /* -1 for error, 1 for address found */
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
    إذا(iface &&
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
      اذهب out;
    }
#endif
    إذا(!host_in) {
      /* Discover IP from input device, then bind to it */
      if2ip_result = Curl_if2ip(af,
#ifdef USE_IPV6
                                scope, conn->scope_id,
#endif
                                iface, myhost, حجم(myhost));
    }
    بدل(if2ip_result) {
    حالة IF2IP_NOT_FOUND:
      إذا(iface_in && !host_in) {
        /* Do not fall back to treating it as a hostname */
        حرف buffer[STRERROR_LEN];
        data->state.os_errno = sockerr = SOCKERRNO;
        failf(data, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, buffer, حجم(buffer)));
        result = CURLE_INTERFACE_FAILED;
        اذهب out;
      }
      اكسر;
    حالة IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      result = CURLE_UNSUPPORTED_PROTOCOL;
      اذهب out;
    حالة IF2IP_FOUND:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(data, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      done = 1;
      اكسر;
    }
    إذا(!iface_in || host_in) {
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
      إذا(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#endif

      (فراغ)Curl_resolv_blocking(data, dns_queries, host, 80, transport, &h);
      إذا(h) {
        صحيح h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_address(h->addr, myhost, حجم(myhost));
        infof(data, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_entry_unlink(data, &h); /* this will NULL, potential free h */
        إذا(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          result = CURLE_UNSUPPORTED_PROTOCOL;
          اذهب out;
        }
        done = 1;
      }
      وإلا {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        done = -1;
      }
    }

    إذا(done > 0) {
#ifdef USE_IPV6
      /* IPv6 address */
      إذا(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        حرف *scope_ptr = strchr(myhost, '%');
        إذا(scope_ptr)
          *(scope_ptr++) = '\0';
#endif
        إذا(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          إذا(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            إذا(curlx_str_نومبور((ثابت حرف **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              result = CURLE_UNSUPPORTED_PROTOCOL;
              اذهب out;
            }
            si6->sin6_scope_id = (unsigned صحيح)scope_id;
          }
#endif
        }
        sizeof_sa = حجم(بنية sockaddr_in6);
      }
      وإلا
#endif
      /* IPv4 address */
      إذا((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(port);
        sizeof_sa = حجم(بنية sockaddr_in);
      }
    }

    إذا(done < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      حرف buffer[STRERROR_LEN];
      data->state.errorbuf = FALSE;
      data->state.os_errno = sockerr = SOCKERRNO;
      failf(data, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, buffer, حجم(buffer)));
      result = CURLE_INTERFACE_FAILED;
      اذهب out;
    }
  }
  وإلا {
    /* no device was given, prepare sa to match af's needs */
#ifdef USE_IPV6
    إذا(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(port);
      sizeof_sa = حجم(بنية sockaddr_in6);
    }
    وإلا
#endif
    إذا(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(port);
      sizeof_sa = حجم(بنية sockaddr_in);
    }
  }
#ifdef IP_BIND_ADDRESS_NO_PORT
  (فراغ)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, حجم(on));
#endif
  لكل(;;) {
    إذا(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(data, "Local port: %hu", port);
      conn->bits.bound = TRUE;
      result = CURLE_OK;
      اذهب out;
    }

    إذا(--portnum > 0) {
      port++; /* try next port */
      إذا(port == 0)
        اكسر;
      infof(data, "Bind to local port %d failed, trying next", port - 1);
      /* We reuse/clobber the port variable here below */
      إذا(sock->sa_family == AF_INET)
        si4->sin_port = htons(port);
#ifdef USE_IPV6
      وإلا
        si6->sin6_port = htons(port);
#endif
    }
    وإلا
      اكسر;
  }
  {
    حرف buffer[STRERROR_LEN];
    data->state.os_errno = sockerr = SOCKERRNO;
    failf(data, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, حجم(buffer)));
  }

  result = CURLE_INTERFACE_FAILED;

out:
  curlx_free(dev_in);
  curlx_free(iface_in);
  curlx_free(host_in);
  أرجع result;
}
#endif

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
ساكن bool verifyconnect(curl_socket_t sockfd, صحيح *psockerr)
{
  bool rc = TRUE;
#ifdef SO_ERROR
  صحيح sockerr = 0;
  curl_socklen_t errSize = حجم(sockerr);

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

  إذا(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (فراغ *)&sockerr, &errSize))
    sockerr = SOCKERRNO;
#إذا defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  إذا(EBADIOCTL == sockerr) {
    SET_SOCKERRNO(0);
    sockerr = 0;
  }
#endif
  إذا((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  وإلا
    /* This was not a successful connect */
    rc = FALSE;
  إذا(psockerr)
    *psockerr = sockerr;
#وإلا
  (فراغ)sockfd;
  إذا(psockerr)
    *psockerr = SOCKERRNO;
#endif
  أرجع rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
ساكن CURLcode socket_connect_result(بنية Curl_easy *data,
                                      ثابت حرف *ipaddress, صحيح sockerr)
{
  إذا(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    أرجع CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(حرف buffer[STRERROR_LEN]);
    infof(data, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, buffer, حجم(buffer)));
    NOVERBOSE((فراغ)ipaddress);
  }
  data->state.os_errno = sockerr;
  /* connect failed */
  أرجع CURLE_COULDNT_CONNECT;
}

بنية cf_socket_ctx {
  بنية Curl_peer *peer;
  بنية Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  بنية ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  بنية curltime started_at;        /* when socket was created */
  بنية curltime connected_at;      /* when socket connected/got first byte */
  بنية curltime first_byte_at;     /* when first byte was recvd */
#ifdef USE_WINSOCK
  بنية curltime last_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_size;                     /* the last set SO_SNDBUF size */
#endif
  صحيح sockerr;                       /* socket error of last failure or 0 */
#ifdef DEBUGBUILD
  صحيح connect_wait_ms;               /* ms to wait after 1st connect() */
  صحيح wblock_percent;                /* percent of writes doing EAGAIN */
  صحيح wpartial_percent;              /* percent of bytes written in send */
  صحيح rblock_percent;                /* percent of reads doing EAGAIN */
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

ساكن CURLcode cf_socket_ctx_init(بنية cf_socket_ctx *ctx,
                                   بنية Curl_peer *peer,
                                   بنية Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, حجم(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ifdef DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_SOCK_WBLOCK");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نومبور(&p, &l, 100))
        ctx->wblock_percent = (صحيح)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نومبور(&p, &l, 100))
        ctx->wpartial_percent = (صحيح)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نومبور(&p, &l, 100))
        ctx->rblock_percent = (صحيح)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نومبور(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نومبور(&p, &l, INT_MAX))
        ctx->connect_wait_ms = (صحيح)l;
    }
  }
#endif

  أرجع CURLE_OK;
}

ساكن فراغ cf_socket_ctx_free(بنية cf_socket_ctx *ctx)
{
  إذا(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_socket_shutdown(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data,
                                   bool *done)
{
  إذا(cf->connected) {
    بنية cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(data, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    إذا(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      unsigned حرف buf[1024];
      (فراغ)sread(ctx->sock, buf, حجم(buf));
    }
  }
  *done = TRUE;
  أرجع CURLE_OK;
}

ساكن فراغ cf_socket_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  إذا(ctx) {
    إذا(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(data, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      إذا(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(data, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

ساكن فراغ set_local_ip(بنية Curl_cfilter *cf,
                         بنية Curl_easy *data)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_port = 0;

#ifdef HAVE_GETSOCKNAME
  إذا((ctx->sock != CURL_SOCKET_BAD) &&
     !(data->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    بنية Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = حجم(بنية Curl_sockaddr_storage);

    memset(&ssloc, 0, حجم(ssloc));
    إذا(getsockname(ctx->sock, (بنية sockaddr *)&ssloc, &slen)) {
      VERBOSE(حرف buffer[STRERROR_LEN]);
      VERBOSE(صحيح sockerr = SOCKERRNO);
      infof(data, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, buffer, حجم(buffer)));
    }
    وإلا {
      CURLcode result = sockaddr2string((بنية sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_port);
      إذا(result)
        infof(data, "ssloc inet_ntop() failed with %d", (صحيح)result);
    }
  }
#وإلا
  (فراغ)data;
#endif
}

ساكن CURLcode set_remote_ip(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  CURLcode result;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  result = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_port);
  إذا(result) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(data, "curl_sa_addr inet_ntop() failed with %d", (صحيح)result);
    أرجع CURLE_FAILED_INIT;
  }
  أرجع CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
ساكن صحيح cf_socktype(صحيح x)
{
#ifdef SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#endif
  أرجع x;
}

#ifdef _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ifndef SIO_TCP_INITIAL_RTO
#define SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#define TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#endif /* SIO_TCP_INITIAL_RTO */

#ifndef TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#define TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#endif

ساكن bool targets_localhost(بنية cf_socket_ctx *ctx)
{
  أرجع (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
ساكن فراغ tcplocalhost(بنية Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  إذا(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD bytes = 0;
    memset(&rto, 0, حجم(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    إذا(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    وإلا
      rto.MaxSynRetransmissions = 1;
    (فراغ)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, حجم(rto),
                   عدم, 0, &bytes, عدم, عدم);
  }
}
#وإلا
#define tcplocalhost(x, y)
#endif /* _WIN32 */

ساكن CURLcode cf_socket_open(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  صحيح error = 0;
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
  إذا(!data->set.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#endif
  result = socket_open(data, &ctx->addr, &ctx->sock);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  إذا(!data->set.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#endif
  إذا(result)
    اذهب out;

  result = set_remote_ip(cf, data);
  إذا(result)
    اذهب out;

#ifdef USE_IPV6
  إذا(ctx->addr.family == AF_INET6) {
#ifdef USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    صحيح on = 0;
    (فراغ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (فراغ *)&on, حجم(on));
#endif
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      بنية sockaddr_in6 *sa6 = (فراغ *)&ctx->addr.curl_sa_addr;
      إذا(sa6->sin6_scope_id)
        infof(data, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_port,
              (unsigned long)sa6->sin6_scope_id);
      وإلا
#endif
        infof(data, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#endif
  }
  وإلا
#endif
    infof(data, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_port);

#ifdef USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#وإلا
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#endif
  إذا(is_tcp) {
    إذا(data->set.tcp_nodelay)
      tcpnodelay(cf, data, ctx->sock);

    إذا(data->set.tcp_keepalive)
      tcpkeepalive(cf, data, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  إذا(data->set.fsockopt) {
    /* activate callback for setting socket options */
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_END(&guard);

    إذا(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    وإلا إذا(error) {
      result = CURLE_ABORTED_BY_CALLBACK;
      اذهب out;
    }
  }

#ifndef CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  إذا(ctx->addr.family == AF_INET
#ifdef USE_IPV6
     || ctx->addr.family == AF_INET6
#endif
    ) {
    result = bindlocal(data, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    إذا(result) {
      إذا(result == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        result = CURLE_COULDNT_CONNECT;
      }
      اذهب out;
    }
  }
#endif

#ifndef CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  إذا(error < 0) {
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    اذهب out;
  }
#وإلا
  إذا(data->set.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    إذا(error < 0) {
      result = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      اذهب out;
    }
  }
#endif
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
out:
  إذا(result) {
    إذا(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  وإلا إذا(isconnected) {
    set_local_ip(cf, data);
    ctx->connected_at = *Curl_pgrs_now(data);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(data, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (صحيح)result, ctx->sock);
  أرجع result;
}

ساكن صحيح do_connect(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                      bool is_tcp_fastopen)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
#ifdef TCP_FASTOPEN_CONNECT
  صحيح optval = 1;
#endif
  صحيح rc = -1;

  (فراغ)data;
  إذا(is_tcp_fastopen) {
#ifdef CONNECT_DATA_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_AVAILABLE
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    إذا(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = عدم;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_DATA_IDEMPOTENT,
                    عدم, 0, عدم, عدم);
    }
    وإلا {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  وإلا
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#وإلاإذا defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    إذا(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (فراغ *)&optval, حجم(optval)) < 0)
      CURL_TRC_CF(data, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#وإلاإذا defined(MSG_FASTOPEN) /* old Linux */
    إذا(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    وإلا
      rc = 0; /* Do nothing */
#endif
  }
  وإلا {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  أرجع rc;
}

ساكن CURLcode cf_tcp_connect(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               bool *done)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;
  صحيح rc = 0;

  إذا(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  *done = FALSE; /* a negative world view is best */
  إذا(ctx->sock == CURL_SOCKET_BAD) {
    صحيح sockerr;

    result = cf_socket_open(cf, data);
    إذا(result)
      اذهب out;

    إذا(cf->connected) {
      *done = TRUE;
      أرجع CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, data, (bool)cf->conn->bits.tcp_fastopen);
    sockerr = SOCKERRNO;
    set_local_ip(cf, data);
    CURL_TRC_CF(data, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_port);
#ifdef DEBUGBUILD
    إذا(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(data, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#endif
    إذا(rc == -1) {
      ctx->sockerr = sockerr;
      result = socket_connect_result(data, ctx->ip.remote_ip, sockerr);
      اذهب out;
    }
  }

#ifdef mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (فراغ)verifyconnect(ctx->sock, عدم);
#endif
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  إذا(rc == 0) { /* no connection yet */
    CURL_TRC_CF(data, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    أرجع CURLE_OK;
  }
  وإلا إذا(rc == CURL_CSELECT_OUT ||
#ifdef __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#endif
          cf->conn->bits.tcp_fastopen) {
    إذا(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(data);
      set_local_ip(cf, data);
      *done = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(data, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      أرجع CURLE_OK;
    }
    result = CURLE_COULDNT_CONNECT;
  }
  وإلا إذا(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(data, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (فراغ)verifyconnect(ctx->sock, &ctx->sockerr);
    result = CURLE_COULDNT_CONNECT;
  }

out:
  إذا(result) {
    VERBOSE(حرف buffer[STRERROR_LEN]);
    set_local_ip(cf, data);
    إذا(ctx->sockerr) {
      data->state.os_errno = ctx->sockerr;
      SET_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, buffer, حجم(buffer)));
    }
    وإلا {
      VERBOSE(curlx_strcopy(buffer, حجم(buffer), STRCONST("peer closed")));
    }
    إذا(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(data, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_port,
          ctx->ip.local_ip, ctx->ip.local_port,
          curlx_strerror(ctx->sockerr, buffer, حجم(buffer)));
    *done = FALSE;
  }
  أرجع result;
}

ساكن CURLcode cf_socket_adjust_pollset(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *data,
                                         بنية easy_pollset *ps)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  إذا(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    إذا(ctx->listening) {
      result = Curl_pollset_set_in_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    وإلا إذا(!cf->connected) {
      result = Curl_pollset_set_out_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    وإلا إذا(!ctx->active) {
      result = Curl_pollset_add_in(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  أرجع result;
}

#ifdef USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ifndef SIO_IDEAL_SEND_BACKLOG_QUERY
#define SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#endif

ساكن فراغ win_update_sndbuf_size(بنية Curl_easy *data,
                                   بنية cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  إذا(curlx_ptimediff_ms(Curl_pgrs_now(data),
                        &ctx->last_sndbuf_query_at) > 1000) {
    إذا(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, حجم(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_size &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (ثابت حرف *)&ideal, حجم(ideal))) {
      ctx->sndbuf_size = ideal;
    }
    ctx->last_sndbuf_query_at = *Curl_pgrs_now(data);
  }
}

#endif /* USE_WINSOCK */

ساكن CURLcode cf_socket_send(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (فراغ)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  إذا(ctx->wblock_percent > 0) {
    unsigned حرف c = 0;
    Curl_rand_bytes(data, FALSE, &c, 1);
    إذا(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      أرجع CURLE_AGAIN;
    }
  }
  إذا(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    إذا(!len)
      len = 1;
    CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#endif

#إذا defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  إذا(cf->conn->bits.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->bits.tcp_fastopen = FALSE;
  }
  وإلا
#endif
    rv = swrite(ctx->sock, buf, len);

  إذا(!curlx_sztouz(rv, pnwritten)) {
    صحيح sockerr = SOCKERRNO;
    إذا(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    وإلا {
      حرف buffer[STRERROR_LEN];
      failf(data, "Send failure: %s",
            curlx_strerror(sockerr, buffer, حجم(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_SEND_ERROR;
    }
  }

#ifdef USE_WINSOCK
  إذا(!result)
    win_update_sndbuf_size(data, ctx);
#endif

  CURL_TRC_CF(data, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (صحيح)result, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  أرجع result;
}

ساكن CURLcode cf_socket_recv(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                               حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial reads */
  إذا(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    unsigned حرف c = 0;
    Curl_rand(data, &c, 1);
    إذا(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      أرجع CURLE_AGAIN;
    }
  }
  إذا(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#endif

  rv = sread(ctx->sock, buf, len);

  إذا(!curlx_sztouz(rv, pnread)) {
    صحيح sockerr = SOCKERRNO;
    إذا(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    وإلا {
      حرف buffer[STRERROR_LEN];
      failf(data, "Recv failure: %s",
            curlx_strerror(sockerr, buffer, حجم(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(data, cf, "recv(len=%zu) -> %d, %zu", len, (صحيح)result, *pnread);
  إذا(!result && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(data);
    ctx->got_first_byte = TRUE;
  }
  أرجع result;
}

ساكن فراغ cf_socket_update_data(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data)
{
  /* Update the IP info held in the transfer, if we have that. */
  إذا(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    بنية cf_socket_ctx *ctx = cf->ctx;
    data->info.primary = ctx->ip;
  }
}

ساكن فراغ cf_socket_active(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  set_local_ip(cf, data);
#ifdef USE_IPV6
  إذا(cf->sockindex == FIRSTSOCKET)
    cf->conn->bits.ipv6 = (ctx->addr.family == AF_INET6);
#endif
  ctx->active = TRUE;
}

ساكن CURLcode cf_socket_cntrl(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_socket_ctx *ctx = cf->ctx;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_active(cf, data);
    cf_socket_update_data(cf, data);
    اكسر;
  حالة CF_CTRL_DATA_SETUP:
    cf_socket_update_data(cf, data);
    اكسر;
  حالة CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    اكسر;
  حالة CF_CTRL_REPORT_STATS:
    إذا(cf->connected && !ctx->stats_reported) {
      بنية curltime *ts = عدم;
      بدل(ctx->transport) {
      حالة TRNSPRT_UDP:
      حالة TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        إذا(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        اكسر;
      افتراضي:
        ts = &ctx->connected_at;
        اكسر;
      }
      إذا(ts) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  أرجع CURLE_OK;
}

ساكن bool cf_socket_conn_is_alive(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    bool *ماسوقن_pending)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  بنية pollfd pfd[1];
  صحيح r;

  *ماسوقن_pending = FALSE;

  إذا(!ctx || ctx->sock == CURL_SOCKET_BAD)
    أرجع FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  إذا(r < 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll error, assume dead");
    أرجع FALSE;
  }
  وإلا إذا(r == 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll timeout, assume alive");
    أرجع TRUE;
  }
  وإلا إذا(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(data, cf, "is_alive: err/hup/etc events, assume dead");
    أرجع FALSE;
  }

  CURL_TRC_CF(data, cf, "is_alive: valid events, looks alive");
  *ماسوقن_pending = TRUE;
  أرجع TRUE;
}

ساكن CURLcode cf_socket_query(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_socket_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    أرجع CURLE_OK;
  حالة CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    أرجع CURLE_OK;
  حالة CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((ثابت بنية Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : عدم;
    أرجع CURLE_OK;
  حالة CF_QUERY_CONNECT_REPLY_MS:
    إذا(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (صحيح)ms : INT_MAX;
    }
    وإلا
      *pres1 = -1;
    أرجع CURLE_OK;
  حالة CF_QUERY_IP_INFO:
#ifdef USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#وإلا
    *pres1 = FALSE;
#endif
    *(بنية ip_quadruple *)pres2 = ctx->ip;
    أرجع CURLE_OK;
  حالة CF_QUERY_REALLY_CONNECTED:
    إذا(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    وإلا
      *pres1 = ctx->got_first_byte;
    أرجع CURLE_OK;
  افتراضي:
    اكسر;
  }
  أرجع cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

بنية Curl_cftype Curl_cft_tcp = {
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

CURLcode Curl_cf_tcp_create(بنية Curl_cfilter **pcf,
                            بنية Curl_easy *data,
                            بنية Curl_peer *origin,
                            بنية Curl_peer *peer,
                            uint8_t transport_peer,
                            بنية connectdata *conn,
                            بنية Curl_sockaddr_ex *addr,
                            بنية Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  بنية cf_socket_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode result;

  (فراغ)data;
  (فراغ)origin;
  (فراغ)conn;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  إذا(!addr) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  إذا(result)
    اذهب out;

  result = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

out:
  *pcf = (!result) ? cf : عدم;
  إذا(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  أرجع result;
}

#ifdef __linux__
ساكن فراغ linux_quic_mtu(بنية cf_socket_ctx *ctx)
{
  صحيح val;
  بدل(ctx->addr.family) {
#ifdef IP_MTU_DISCOVER
  حالة AF_INET:
    val = IP_PMTUDISC_DO;
    (فراغ)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     حجم(val));
    اكسر;
#endif
#ifdef IPV6_MTU_DISCOVER
  حالة AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (فراغ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     حجم(val));
    اكسر;
#endif
  }
}
#وإلا
#define linux_quic_mtu(x)
#endif

#إذا defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
ساكن فراغ linux_quic_gro(بنية cf_socket_ctx *ctx)
{
  صحيح one = 1;
  (فراغ)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)حجم(one));
}
#وإلا
#define linux_quic_gro(x)
#endif

#إذا (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
ساكن فراغ linux_quic_ecn(بنية cf_socket_ctx *ctx)
{
  unsigned صحيح tos = 1;
  بدل(ctx->addr.family) {
  حالة AF_INET:
    (فراغ)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, حجم(tos));
    اكسر;
#ifdef IPV6_RECVTCLASS
  حالة AF_INET6:
    (فراغ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, حجم(tos));
    اكسر;
#endif
  }
}
#وإلا
#define linux_quic_ecn(x)
#endif

#إذا (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
ساكن فراغ linux_ip_dontfrag(بنية cf_socket_ctx *ctx)
{
  صحيح val = 1;

  بدل(ctx->addr.family) {
  حالة AF_INET:
    (فراغ)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, حجم(val));
    اكسر;
#ifdef IPV6_DONTFRAG
  حالة AF_INET6:
    (فراغ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, حجم(val));
    اكسر;
#endif
  }
}
#وإلا
#define linux_ip_dontfrag(x)
#endif

ساكن CURLcode cf_udp_setup_quic(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  صحيح rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  إذا(rc == -1) {
    أرجع socket_connect_result(data, ctx->ip.remote_ip, SOCKERRNO);
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

  أرجع CURLE_OK;
}

ساكن CURLcode cf_udp_connect(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               bool *done)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;

  إذا(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  *done = FALSE;
  إذا(ctx->sock == CURL_SOCKET_BAD) {
    result = cf_socket_open(cf, data);
    إذا(result) {
      CURL_TRC_CF(data, cf, "cf_udp_connect(), open failed -> %d",
                  (صحيح)result);
      اذهب out;
    }

    إذا(ctx->transport == TRNSPRT_QUIC) {
      result = cf_udp_setup_quic(cf, data);
      إذا(result)
        اذهب out;
      CURL_TRC_CF(data, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
    }
    *done = TRUE;
    cf->connected = TRUE;
  }
out:
  أرجع result;
}

بنية Curl_cftype Curl_cft_udp = {
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

CURLcode Curl_cf_udp_create(بنية Curl_cfilter **pcf,
                            بنية Curl_easy *data,
                            بنية Curl_peer *origin,
                            بنية Curl_peer *peer,
                            uint8_t transport_peer,
                            بنية connectdata *conn,
                            بنية Curl_sockaddr_ex *addr,
                            بنية Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  بنية cf_socket_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode result;

  (فراغ)data;
  (فراغ)origin;
  (فراغ)conn;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  إذا(result)
    اذهب out;

  result = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

out:
  *pcf = (!result) ? cf : عدم;
  إذا(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  أرجع result;
}

/* this is the TCP filter which can also handle this case */
بنية Curl_cftype Curl_cft_unix = {
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

CURLcode Curl_cf_unix_create(بنية Curl_cfilter **pcf,
                            بنية Curl_easy *data,
                            بنية Curl_peer *origin,
                            بنية Curl_peer *peer,
                            uint8_t transport_peer,
                            بنية connectdata *conn,
                            بنية Curl_sockaddr_ex *addr,
                            بنية Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  بنية cf_socket_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode result;

  (فراغ)data;
  (فراغ)origin;
  (فراغ)conn;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  إذا(result)
    اذهب out;

  result = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

out:
  *pcf = (!result) ? cf : عدم;
  إذا(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  أرجع result;
}

ساكن timediff_t cf_tcp_accept_timeleft(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *data)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ifndef CURL_DISABLE_FTP
  إذا(data->set.accepttimeout > 0)
    timeout_ms = data->set.accepttimeout;
#endif

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(data);
  إذا(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  وإلا {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->started_at);
    إذا(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  أرجع timeout_ms;
}

ساكن فراغ cf_tcp_set_accepted_remote_ip(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *data)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
#ifdef HAVE_GETPEERNAME
  CURLcode result;
  بنية Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  plen = حجم(ssrem);
  memset(&ssrem, 0, plen);
  إذا(getpeername(ctx->sock, (بنية sockaddr *)&ssrem, &plen)) {
    حرف buffer[STRERROR_LEN];
    صحيح sockerr = SOCKERRNO;
    failf(data, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, حجم(buffer)));
    أرجع;
  }
  result = sockaddr2string((بنية sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_port);
  إذا(result) {
    failf(data, "ssrem inet_ntop() failed with %d", (صحيح)result);
    أرجع;
  }
#وإلا
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  (فراغ)data;
#endif
}

ساكن CURLcode cf_tcp_accept_connect(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      bool *done)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  حرف errbuf[STRERROR_LEN];
#ifdef USE_IPV6
  بنية Curl_sockaddr_storage add;
#وإلا
  بنية sockaddr_in add;
#endif
  curl_socklen_t size = (curl_socklen_t)حجم(add);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  صحيح socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (فراغ)data;
  إذا(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  *done = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, data);
  إذا(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(data, "Accept timeout occurred while waiting server connect");
    أرجع CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(data, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(data, cf, "socket_check -> %x", (unsigned صحيح)socketstate);
  بدل(socketstate) {
  حالة -1: /* error */
    /* let's die here */
    failf(data, "Error while waiting for server connect");
    أرجع CURLE_FTP_ACCEPT_FAILED;
  افتراضي:
    إذا(socketstate & CURL_CSELECT_IN) {
      infof(data, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    اكسر;
  }

  إذا(!incoming) {
    CURL_TRC_CF(data, cf, "nothing heard from the server yet");
    أرجع CURLE_OK;
  }

  size = حجم(add);
#ifdef HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (بنية sockaddr *)&add, &size,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#وإلا
  s_accepted = CURL_ACCEPT(ctx->sock, (بنية sockaddr *)&add, &size);
#endif

  إذا(s_accepted == CURL_SOCKET_BAD) {
    failf(data, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    أرجع CURLE_FTP_ACCEPT_FAILED;
  }
#ifndef HAVE_ACCEPT4
#ifdef HAVE_FCNTL
  إذا(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    أرجع CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* HAVE_FCNTL */
  إذا(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(data, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    أرجع CURLE_FTP_ACCEPT_FAILED;
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

  إذا(data->set.fsockopt) {
    بنية Curl_mapi_guard guard;
    صحيح error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_END(&guard);

    إذا(error)
      أرجع CURLE_ABORTED_BY_CALLBACK;
  }
  *done = TRUE;
  أرجع CURLE_OK;
}

بنية Curl_cftype Curl_cft_tcp_accept = {
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

CURLcode Curl_conn_tcp_listen_set(بنية Curl_easy *data,
                                  بنية connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode result;
  بنية Curl_cfilter *cf = عدم;
  بنية cf_socket_ctx *ctx = عدم;

  /* replace any existing */
  Curl_conn_cf_discard_all(data, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  result = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  إذا(result)
    اذهب out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(data);
  conn->sock[sockindex] = ctx->sock;
  set_local_ip(cf, data);
  CURL_TRC_CF(data, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_port);

out:
  إذا(result) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  أرجع result;
}

bool Curl_conn_is_tcp_listen(بنية Curl_easy *data,
                             int8_t sockindex)
{
  بنية Curl_cfilter *cf = data->conn->cfilter[sockindex];
  طالما(cf) {
    إذا(cf->cft == &Curl_cft_tcp_accept)
      أرجع TRUE;
    cf = cf->next;
  }
  أرجع FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
ساكن bool cf_is_socket(بنية Curl_cfilter *cf)
{
  أرجع cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data,
                             curl_socket_t *psock,
                             ثابت بنية Curl_sockaddr_ex **paddr,
                             بنية ip_quadruple *pip)
{
  (فراغ)data;
  إذا(cf_is_socket(cf) && cf->ctx) {
    بنية cf_socket_ctx *ctx = cf->ctx;

    إذا(psock)
      *psock = ctx->sock;
    إذا(paddr)
      *paddr = &ctx->addr;
    إذا(pip)
      *pip = ctx->ip;
    أرجع CURLE_OK;
  }
  أرجع CURLE_FAILED_INIT;
}
