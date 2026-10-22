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
#شامل "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_SYS_UN_H
#شامل <sys/un.h> /* for sockaddr_un */
#endif
#ifdef HAVE_LINUX_TCP_H
#شامل <linux/tcp.h>
#وگرنهاگر defined(HAVE_NETINET_TCP_H)
#شامل <netinet/tcp.h>
#endif
#ifdef HAVE_NETINET_UDP_H
#شامل <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#شامل <netinet/ip.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif

#ifdef HAVE_IFADDRS_H
#شامل <ifaddrs.h>
#endif
#ifdef HAVE_NET_IF_H
#شامل <net/if.h>
#endif
#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "if2ip.h"
#شامل "cfilters.h"
#شامل "cf-socket.h"
#شامل "connect.h"
#شامل "curl_addrinfo.h"
#شامل "select.h"
#شامل "multiif.h"
#شامل "curlx/inet_ntop.h"
#شامل "curlx/inet_pton.h"
#شامل "progress.h"
#شامل "conncache.h"
#شامل "multihandle.h"
#شامل "rand.h"
#شامل "sockaddr.h"
#شامل "curlx/strdup.h"
#شامل "curlx/nonblock.h"
#شامل "curlx/strcopy.h"
#شامل "curlx/version_win32.h"
#شامل "curlx/strerr.h"
#شامل "curlx/strparse.h"
#شامل "curlx/wait.h"
#ifdef _WIN32
#شامل <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#endif

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(ساختار sockaddr *sa, curl_socklen_t salen,
                                  نویسه *addr, uint16_t *port);
UNITTEST CURLcode sockaddr2string(ساختار sockaddr *sa, curl_socklen_t salen,
                                  نویسه *addr, uint16_t *port)
{
  CURLcode result;
  ساختار sockaddr_in *si = پوچ;
#ifdef USE_IPV6
  ساختار sockaddr_in6 *si6 = پوچ;
#endif
#ifdef USE_UNIX_SOCKETS
  ساختار sockaddr_un *su = پوچ;
#وگرنه
  (تهی)salen;
#endif

  انتخاب(sa->sa_family) {
  حالت AF_INET:
    si = (ساختار sockaddr_in *)(تهی *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    اگر(!result) {
      *port = ntohs(si->sin_port);
      بازگشت result;
    }
    بشکن;
#ifdef USE_IPV6
  حالت AF_INET6:
    si6 = (ساختار sockaddr_in6 *)(تهی *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    اگر(!result) {
      *port = ntohs(si6->sin6_port);
      بازگشت result;
    }
    بشکن;
#endif
#ifdef USE_UNIX_SOCKETS
  حالت AF_UNIX:
    اگر(salen > (curl_socklen_t)اندازه(CURL_SA_FAMILY_T)) {
      su = (ساختار sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_path);
    }
    وگرنه
      addr[0] = 0; /* socket with no name */
    *port = 0;
    بازگشت CURLE_OK;
#endif
  پیشفرض:
    result = CURLE_UNSUPPORTED_PROTOCOL;
    بشکن;
  }

  addr[0] = '\0';
  *port = 0;
  بازگشت result;
}

ایستا تهی tcpnodelay(ساختار Curl_cfilter *cf,
                       ساختار Curl_easy *data,
                       curl_socket_t sockfd)
{
#اگر defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  صحیح level = IPPROTO_TCP;
  VERBOSE(نویسه buffer[STRERROR_LEN]);

  اگر(setsockopt(sockfd, level, TCP_NODELAY, (تهی *)&onoff, اندازه(onoff)) < 0)
    CURL_TRC_CF(data, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, buffer, اندازه(buffer)));
#وگرنه
  (تهی)cf;
  (تهی)data;
  (تهی)sockfd;
#endif
}

#اگر defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_THRESHOLD) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
#اگر defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
ایستا تهی alive_unit(صحیح *x)
{
  /* make sure this doesn't wrap */
  اگر(*x < (INT_MAX / 1000))
    *x *= 1000;
  وگرنه
    /* this is still almost 25 days */
    *x = INT_MAX;
}
#وگرنه
#define alive_unit(x)
#endif
#endif

ایستا تهی tcpkeepalive(ساختار Curl_cfilter *cf,
                         ساختار Curl_easy *data,
                         curl_socket_t sockfd)
{
  صحیح optval = data->set.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  اگر(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (تهی *)&optval, اندازه(optval)) < 0) {
    CURL_TRC_CF(data, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  وگرنه {
#ifdef USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    اگر(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
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
      اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (ثابت نویسه *)&optval, اندازه(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (ثابت نویسه *)&optval, اندازه(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepcnt);
      اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (ثابت نویسه *)&optval, اندازه(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    وگرنه {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ifndef SIO_KEEPALIVE_VALS
#define SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      ساختار tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#endif
      ساختار tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(data->set.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      اگر(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, اندازه(vals),
                  پوچ, 0, &dummy, پوچ, پوچ) != 0) {
        CURL_TRC_CF(data, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#وگرنه /* !USE_WINSOCK */
#ifdef TCP_KEEPIDLE
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (تهی *)&optval, اندازه(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وگرنهاگر defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (تهی *)&optval, اندازه(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وگرنهاگر defined(TCP_KEEPALIVE_THRESHOLD)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_THRESHOLD,
                  (تهی *)&optval, اندازه(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPINTVL
    optval = curlx_sltosi(data->set.tcp_keepintvl);
    alive_unit(&optval);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (تهی *)&optval, اندازه(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وگرنهاگر defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
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
      صحیح keepcnt = curlx_sltosi(data->set.tcp_keepcnt);
      صحیح keepintvl = curlx_sltosi(data->set.tcp_keepintvl);

      اگر(keepcnt > 0 && keepintvl > (INT_MAX / keepcnt))
        optval = INT_MAX;
      وگرنه
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_THRESHOLD,
                  (تهی *)&optval, اندازه(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPCNT
    optval = curlx_sltosi(data->set.tcp_keepcnt);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (تهی *)&optval, اندازه(optval)) < 0) {
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
CURLcode Curl_socket_addr_from_ai(ساختار Curl_sockaddr_ex *addr,
                                  ثابت ساختار Curl_addrinfo *ai,
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
  addr->addrlen = (unsigned صحیح)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= اندازه(addr->curl_sa_addrbuf));
  اگر(addr->addrlen > اندازه(addr->curl_sa_addrbuf))
    بازگشت CURLE_TOO_LARGE;

  memcpy(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  بازگشت CURLE_OK;
}

#ifdef USE_SO_NOSIGPIPE
صحیح Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  صحیح onoff = 1;
  بازگشت setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (تهی *)&onoff, اندازه(onoff));
}
#endif /* USE_SO_NOSIGPIPE */

#اگر defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ایستا uint32_t get_scope_id(ساختار Curl_easy *data,
                             ساختار sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  اگر(data->conn->scope_id)
    بازگشت data->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  اگر(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#اگر defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    ساختار ifaddrs *ifa, *ifa_list;
    اگر(getifaddrs(&ifa_list) == 0) {
      برای(ifa = ifa_list; ifa; ifa = ifa->ifa_next) {
        اگر(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          ساختار sockaddr_in6 *s6 = (تهی *)ifa->ifa_addr;
          اگر(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(data,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (unsigned long)scope_id);
            بشکن;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#endif /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  بازگشت scope_id;
}
#endif

ایستا CURLcode socket_open(ساختار Curl_easy *data,
                            ساختار Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  نویسه errbuf[STRERROR_LEN];

#ifdef SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#endif

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  اگر(data->set.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    ساختار Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fopensocket);
    *sockfd = data->set.fopensocket(data->set.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (ساختار curl_sockaddr *)addr);
    CURL_CBAPI_END(&guard);
  }
  وگرنه {
    /* opensocket callback not set, so create the socket now */
#ifdef DEBUGBUILD
    اگر((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(data, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      بازگشت CURLE_COULDNT_CONNECT;
    }
#endif
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    اگر((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      بازگشت CURLE_OUT_OF_MEMORY;
  }

  اگر(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(data, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, اندازه(errbuf)));
    بازگشت CURLE_COULDNT_CONNECT;
  }

#ifdef USE_SO_NOSIGPIPE
  اگر(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(data, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, اندازه(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    بازگشت CURLE_COULDNT_CONNECT;
  }
#endif /* USE_SO_NOSIGPIPE */

#اگر defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  اگر(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, اندازه(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    بازگشت CURLE_COULDNT_CONNECT;
  }
#endif

#اگر defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  اگر(addr->family == AF_INET6) {
    ساختار sockaddr_in6 * ثابت sa6 = (تهی *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(data, sa6);
  }
#endif
  بازگشت CURLE_OK;
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
CURLcode Curl_socket_open(ساختار Curl_easy *data,
                          ثابت ساختار Curl_addrinfo *ai,
                          ساختار Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  ساختار Curl_sockaddr_ex dummy;
  CURLcode result;

  اگر(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  result = Curl_socket_addr_from_ai(addr, ai, transport);
  اگر(result)
    بازگشت result;

  بازگشت socket_open(data, addr, sockfd);
}

ایستا صحیح socket_close(ساختار Curl_easy *data, ساختار connectdata *conn,
                        صحیح use_callback, curl_socket_t sock)
{
  اگر(sock == CURL_SOCKET_BAD)
    بازگشت 0;

  اگر(use_callback && conn && conn->fclosesocket) {
    ساختار Curl_mapi_guard guard;
    صحیح rc;
    Curl_multi_will_close(data, sock);
    CURL_CBAPI_START(&guard, data, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_END(&guard);
    بازگشت rc;
  }

  اگر(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(data, sock);

  sclose(sock);

  بازگشت 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
صحیح Curl_socket_close(ساختار Curl_easy *data, ساختار connectdata *conn,
                      curl_socket_t sock)
{
  بازگشت socket_close(data, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(ثابت نویسه *درون_داد,
                              نویسه **dev, نویسه **iface, نویسه **host)
{
  ایستا ثابت نویسه if_prefix[] = "if!";
  ایستا ثابت نویسه host_prefix[] = "host!";
  ایستا ثابت نویسه if_host_prefix[] = "ifhost!";
  size_t len;

  len = strlen(درون_داد);
  اگر(len > 512)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  اگر(dev)
    *dev = پوچ;
  اگر(iface)
    *iface = پوچ;
  اگر(host)
    *host = پوچ;

  اگر(!strncmp(if_prefix, درون_داد, strlen(if_prefix))) {
    درون_داد += strlen(if_prefix);
    اگر(!*درون_داد)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    اگر(iface) {
      *iface = curlx_memdup0(درون_داد, len - strlen(if_prefix));
      بازگشت *iface ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    بازگشت CURLE_OK;
  }
  وگرنه اگر(!strncmp(host_prefix, درون_داد, strlen(host_prefix))) {
    درون_داد += strlen(host_prefix);
    اگر(!*درون_داد)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    اگر(host) {
      *host = curlx_memdup0(درون_داد, len - strlen(host_prefix));
      بازگشت *host ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    بازگشت CURLE_OK;
  }
  وگرنه اگر(!strncmp(if_host_prefix, درون_داد, strlen(if_host_prefix))) {
    ثابت نویسه *host_part;
    درون_داد += strlen(if_host_prefix);
    len -= strlen(if_host_prefix);
    host_part = memchr(درون_داد, '!', len);
    اگر(!host_part || !*(host_part + 1))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    اگر(iface) {
      *iface = curlx_memdup0(درون_داد, host_part - درون_داد);
      اگر(!*iface)
        بازگشت CURLE_OUT_OF_MEMORY;
    }
    ++host_part;
    اگر(host) {
      *host = curlx_memdup0(host_part, len - (host_part - درون_داد));
      اگر(!*host) {
        اگر(iface)
          curlx_safefree(*iface);
        بازگشت CURLE_OUT_OF_MEMORY;
      }
    }
    بازگشت CURLE_OK;
  }

  اگر(!*درون_داد)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  اگر(dev) {
    *dev = curlx_memdup0(درون_داد, len);
    بازگشت *dev ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  بازگشت CURLE_OK;
}

#ifndef CURL_DISABLE_BINDLOCAL
ایستا CURLcode bindlocal(ساختار Curl_easy *data, ساختار connectdata *conn,
                          curl_socket_t sockfd, صحیح af, unsigned صحیح scope,
                          uint8_t transport)
{
  ساختار Curl_sockaddr_storage sa;
  ساختار sockaddr *sock = (ساختار sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  ساختار sockaddr_in *si4 = (ساختار sockaddr_in *)&sa;
#ifdef USE_IPV6
  ساختار sockaddr_in6 *si6 = (ساختار sockaddr_in6 *)&sa;
#endif
  ساختار Curl_dns_entry *h = پوچ;
  unsigned short port = data->set.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  صحیح portnum = data->set.localportrange;
  نویسه *dev_in = پوچ, *iface_in = پوچ, *host_in = پوچ;
  ثابت نویسه *iface = پوچ, *host = پوچ;
  صحیح sockerr;
#ifdef IP_BIND_ADDRESS_NO_PORT
  صحیح on = 1;
#endif
  CURLcode result = CURLE_OK;

#ifndef USE_IPV6
  (تهی)scope;
#endif
  اگر(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    result = Curl_parse_interface(conn->interface_opts,
                                  &dev_in, &iface_in, &host_in);
    اگر(result)
      برو out;
    /* This is how we use the values */
    iface = iface_in ? iface_in : dev_in;
    host = host_in ? host_in : dev_in;
  }

  اگر(!iface && !host && !port)
    برو out;

  اگر(iface && (strlen(iface) >= 255)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو out;
  }
  memset(&sa, 0, اندازه(ساختار Curl_sockaddr_storage));

  اگر(iface || host) {
    نویسه myhost[256] = "";
    صحیح done = 0; /* -1 for error, 1 for address found */
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
    اگر(iface &&
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
      برو out;
    }
#endif
    اگر(!host_in) {
      /* Discover IP from input device, then bind to it */
      if2ip_result = Curl_if2ip(af,
#ifdef USE_IPV6
                                scope, conn->scope_id,
#endif
                                iface, myhost, اندازه(myhost));
    }
    انتخاب(if2ip_result) {
    حالت IF2IP_NOT_FOUND:
      اگر(iface_in && !host_in) {
        /* Do not fall back to treating it as a hostname */
        نویسه buffer[STRERROR_LEN];
        data->state.os_errno = sockerr = SOCKERRNO;
        failf(data, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, buffer, اندازه(buffer)));
        result = CURLE_INTERFACE_FAILED;
        برو out;
      }
      بشکن;
    حالت IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      result = CURLE_UNSUPPORTED_PROTOCOL;
      برو out;
    حالت IF2IP_FOUND:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(data, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      done = 1;
      بشکن;
    }
    اگر(!iface_in || host_in) {
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
      اگر(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#endif

      (تهی)Curl_resolv_blocking(data, dns_queries, host, 80, transport, &h);
      اگر(h) {
        صحیح h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_address(h->addr, myhost, اندازه(myhost));
        infof(data, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_entry_unlink(data, &h); /* this will NULL, potential free h */
        اگر(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          result = CURLE_UNSUPPORTED_PROTOCOL;
          برو out;
        }
        done = 1;
      }
      وگرنه {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        done = -1;
      }
    }

    اگر(done > 0) {
#ifdef USE_IPV6
      /* IPv6 address */
      اگر(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        نویسه *scope_ptr = strchr(myhost, '%');
        اگر(scope_ptr)
          *(scope_ptr++) = '\0';
#endif
        اگر(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          اگر(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            اگر(curlx_str_عدد((ثابت نویسه **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              result = CURLE_UNSUPPORTED_PROTOCOL;
              برو out;
            }
            si6->sin6_scope_id = (unsigned صحیح)scope_id;
          }
#endif
        }
        sizeof_sa = اندازه(ساختار sockaddr_in6);
      }
      وگرنه
#endif
      /* IPv4 address */
      اگر((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(port);
        sizeof_sa = اندازه(ساختار sockaddr_in);
      }
    }

    اگر(done < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      نویسه buffer[STRERROR_LEN];
      data->state.errorbuf = FALSE;
      data->state.os_errno = sockerr = SOCKERRNO;
      failf(data, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, buffer, اندازه(buffer)));
      result = CURLE_INTERFACE_FAILED;
      برو out;
    }
  }
  وگرنه {
    /* no device was given, prepare sa to match af's needs */
#ifdef USE_IPV6
    اگر(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(port);
      sizeof_sa = اندازه(ساختار sockaddr_in6);
    }
    وگرنه
#endif
    اگر(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(port);
      sizeof_sa = اندازه(ساختار sockaddr_in);
    }
  }
#ifdef IP_BIND_ADDRESS_NO_PORT
  (تهی)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, اندازه(on));
#endif
  برای(;;) {
    اگر(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(data, "Local port: %hu", port);
      conn->bits.bound = TRUE;
      result = CURLE_OK;
      برو out;
    }

    اگر(--portnum > 0) {
      port++; /* try next port */
      اگر(port == 0)
        بشکن;
      infof(data, "Bind to local port %d failed, trying next", port - 1);
      /* We reuse/clobber the port variable here below */
      اگر(sock->sa_family == AF_INET)
        si4->sin_port = htons(port);
#ifdef USE_IPV6
      وگرنه
        si6->sin6_port = htons(port);
#endif
    }
    وگرنه
      بشکن;
  }
  {
    نویسه buffer[STRERROR_LEN];
    data->state.os_errno = sockerr = SOCKERRNO;
    failf(data, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, اندازه(buffer)));
  }

  result = CURLE_INTERFACE_FAILED;

out:
  curlx_free(dev_in);
  curlx_free(iface_in);
  curlx_free(host_in);
  بازگشت result;
}
#endif

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
ایستا bool verifyconnect(curl_socket_t sockfd, صحیح *psockerr)
{
  bool rc = TRUE;
#ifdef SO_ERROR
  صحیح sockerr = 0;
  curl_socklen_t errSize = اندازه(sockerr);

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

  اگر(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (تهی *)&sockerr, &errSize))
    sockerr = SOCKERRNO;
#اگر defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  اگر(EBADIOCTL == sockerr) {
    SET_SOCKERRNO(0);
    sockerr = 0;
  }
#endif
  اگر((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  وگرنه
    /* This was not a successful connect */
    rc = FALSE;
  اگر(psockerr)
    *psockerr = sockerr;
#وگرنه
  (تهی)sockfd;
  اگر(psockerr)
    *psockerr = SOCKERRNO;
#endif
  بازگشت rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
ایستا CURLcode socket_connect_result(ساختار Curl_easy *data,
                                      ثابت نویسه *ipaddress, صحیح sockerr)
{
  اگر(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    بازگشت CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(نویسه buffer[STRERROR_LEN]);
    infof(data, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, buffer, اندازه(buffer)));
    NOVERBOSE((تهی)ipaddress);
  }
  data->state.os_errno = sockerr;
  /* connect failed */
  بازگشت CURLE_COULDNT_CONNECT;
}

ساختار cf_socket_ctx {
  ساختار Curl_peer *peer;
  ساختار Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  ساختار ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  ساختار curltime started_at;        /* when socket was created */
  ساختار curltime connected_at;      /* when socket connected/got first byte */
  ساختار curltime first_byte_at;     /* when first byte was recvd */
#ifdef USE_WINSOCK
  ساختار curltime last_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_size;                     /* the last set SO_SNDBUF size */
#endif
  صحیح sockerr;                       /* socket error of last failure or 0 */
#ifdef DEBUGBUILD
  صحیح connect_wait_ms;               /* ms to wait after 1st connect() */
  صحیح wblock_percent;                /* percent of writes doing EAGAIN */
  صحیح wpartial_percent;              /* percent of bytes written in send */
  صحیح rblock_percent;                /* percent of reads doing EAGAIN */
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

ایستا CURLcode cf_socket_ctx_init(ساختار cf_socket_ctx *ctx,
                                   ساختار Curl_peer *peer,
                                   ساختار Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, اندازه(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ifdef DEBUGBUILD
  {
    ثابت نویسه *p = getenv("CURL_DBG_SOCK_WBLOCK");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, 100))
        ctx->wblock_percent = (صحیح)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, 100))
        ctx->wpartial_percent = (صحیح)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, 100))
        ctx->rblock_percent = (صحیح)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, INT_MAX))
        ctx->connect_wait_ms = (صحیح)l;
    }
  }
#endif

  بازگشت CURLE_OK;
}

ایستا تهی cf_socket_ctx_free(ساختار cf_socket_ctx *ctx)
{
  اگر(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

ایستا CURLcode cf_socket_shutdown(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   bool *done)
{
  اگر(cf->connected) {
    ساختار cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(data, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    اگر(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      unsigned نویسه buf[1024];
      (تهی)sread(ctx->sock, buf, اندازه(buf));
    }
  }
  *done = TRUE;
  بازگشت CURLE_OK;
}

ایستا تهی cf_socket_destroy(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  اگر(ctx) {
    اگر(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(data, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      اگر(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(data, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

ایستا تهی set_local_ip(ساختار Curl_cfilter *cf,
                         ساختار Curl_easy *data)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_port = 0;

#ifdef HAVE_GETSOCKNAME
  اگر((ctx->sock != CURL_SOCKET_BAD) &&
     !(data->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    ساختار Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = اندازه(ساختار Curl_sockaddr_storage);

    memset(&ssloc, 0, اندازه(ssloc));
    اگر(getsockname(ctx->sock, (ساختار sockaddr *)&ssloc, &slen)) {
      VERBOSE(نویسه buffer[STRERROR_LEN]);
      VERBOSE(صحیح sockerr = SOCKERRNO);
      infof(data, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, buffer, اندازه(buffer)));
    }
    وگرنه {
      CURLcode result = sockaddr2string((ساختار sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_port);
      اگر(result)
        infof(data, "ssloc inet_ntop() failed with %d", (صحیح)result);
    }
  }
#وگرنه
  (تهی)data;
#endif
}

ایستا CURLcode set_remote_ip(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  CURLcode result;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  result = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_port);
  اگر(result) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(data, "curl_sa_addr inet_ntop() failed with %d", (صحیح)result);
    بازگشت CURLE_FAILED_INIT;
  }
  بازگشت CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
ایستا صحیح cf_socktype(صحیح x)
{
#ifdef SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#endif
  بازگشت x;
}

#ifdef _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ifndef SIO_TCP_INITIAL_RTO
#define SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#define TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعریفنوع ساختار _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#endif /* SIO_TCP_INITIAL_RTO */

#ifndef TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#define TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#endif

ایستا bool targets_localhost(ساختار cf_socket_ctx *ctx)
{
  بازگشت (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
ایستا تهی tcplocalhost(ساختار Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  اگر(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD bytes = 0;
    memset(&rto, 0, اندازه(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    اگر(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    وگرنه
      rto.MaxSynRetransmissions = 1;
    (تهی)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, اندازه(rto),
                   پوچ, 0, &bytes, پوچ, پوچ);
  }
}
#وگرنه
#define tcplocalhost(x, y)
#endif /* _WIN32 */

ایستا CURLcode cf_socket_open(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  صحیح error = 0;
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
  اگر(!data->set.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#endif
  result = socket_open(data, &ctx->addr, &ctx->sock);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  اگر(!data->set.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#endif
  اگر(result)
    برو out;

  result = set_remote_ip(cf, data);
  اگر(result)
    برو out;

#ifdef USE_IPV6
  اگر(ctx->addr.family == AF_INET6) {
#ifdef USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    صحیح on = 0;
    (تهی)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (تهی *)&on, اندازه(on));
#endif
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      ساختار sockaddr_in6 *sa6 = (تهی *)&ctx->addr.curl_sa_addr;
      اگر(sa6->sin6_scope_id)
        infof(data, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_port,
              (unsigned long)sa6->sin6_scope_id);
      وگرنه
#endif
        infof(data, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#endif
  }
  وگرنه
#endif
    infof(data, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_port);

#ifdef USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#وگرنه
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#endif
  اگر(is_tcp) {
    اگر(data->set.tcp_nodelay)
      tcpnodelay(cf, data, ctx->sock);

    اگر(data->set.tcp_keepalive)
      tcpkeepalive(cf, data, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  اگر(data->set.fsockopt) {
    /* activate callback for setting socket options */
    ساختار Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_END(&guard);

    اگر(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    وگرنه اگر(error) {
      result = CURLE_ABORTED_BY_CALLBACK;
      برو out;
    }
  }

#ifndef CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  اگر(ctx->addr.family == AF_INET
#ifdef USE_IPV6
     || ctx->addr.family == AF_INET6
#endif
    ) {
    result = bindlocal(data, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    اگر(result) {
      اگر(result == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        result = CURLE_COULDNT_CONNECT;
      }
      برو out;
    }
  }
#endif

#ifndef CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  اگر(error < 0) {
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    برو out;
  }
#وگرنه
  اگر(data->set.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    اگر(error < 0) {
      result = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      برو out;
    }
  }
#endif
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
out:
  اگر(result) {
    اگر(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  وگرنه اگر(isconnected) {
    set_local_ip(cf, data);
    ctx->connected_at = *Curl_pgrs_now(data);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(data, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (صحیح)result, ctx->sock);
  بازگشت result;
}

ایستا صحیح do_connect(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                      bool is_tcp_fastopen)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
#ifdef TCP_FASTOPEN_CONNECT
  صحیح optval = 1;
#endif
  صحیح rc = -1;

  (تهی)data;
  اگر(is_tcp_fastopen) {
#ifdef CONNECT_DATA_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_AVAILABLE
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    اگر(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = پوچ;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_DATA_IDEMPOTENT,
                    پوچ, 0, پوچ, پوچ);
    }
    وگرنه {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  وگرنه
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#وگرنهاگر defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    اگر(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (تهی *)&optval, اندازه(optval)) < 0)
      CURL_TRC_CF(data, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#وگرنهاگر defined(MSG_FASTOPEN) /* old Linux */
    اگر(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    وگرنه
      rc = 0; /* Do nothing */
#endif
  }
  وگرنه {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  بازگشت rc;
}

ایستا CURLcode cf_tcp_connect(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               bool *done)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;
  صحیح rc = 0;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  *done = FALSE; /* a negative world view is best */
  اگر(ctx->sock == CURL_SOCKET_BAD) {
    صحیح sockerr;

    result = cf_socket_open(cf, data);
    اگر(result)
      برو out;

    اگر(cf->connected) {
      *done = TRUE;
      بازگشت CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, data, (bool)cf->conn->bits.tcp_fastopen);
    sockerr = SOCKERRNO;
    set_local_ip(cf, data);
    CURL_TRC_CF(data, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_port);
#ifdef DEBUGBUILD
    اگر(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(data, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#endif
    اگر(rc == -1) {
      ctx->sockerr = sockerr;
      result = socket_connect_result(data, ctx->ip.remote_ip, sockerr);
      برو out;
    }
  }

#ifdef mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (تهی)verifyconnect(ctx->sock, پوچ);
#endif
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  اگر(rc == 0) { /* no connection yet */
    CURL_TRC_CF(data, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    بازگشت CURLE_OK;
  }
  وگرنه اگر(rc == CURL_CSELECT_OUT ||
#ifdef __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#endif
          cf->conn->bits.tcp_fastopen) {
    اگر(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(data);
      set_local_ip(cf, data);
      *done = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(data, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      بازگشت CURLE_OK;
    }
    result = CURLE_COULDNT_CONNECT;
  }
  وگرنه اگر(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(data, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (تهی)verifyconnect(ctx->sock, &ctx->sockerr);
    result = CURLE_COULDNT_CONNECT;
  }

out:
  اگر(result) {
    VERBOSE(نویسه buffer[STRERROR_LEN]);
    set_local_ip(cf, data);
    اگر(ctx->sockerr) {
      data->state.os_errno = ctx->sockerr;
      SET_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, buffer, اندازه(buffer)));
    }
    وگرنه {
      VERBOSE(curlx_strcopy(buffer, اندازه(buffer), STRCONST("peer closed")));
    }
    اگر(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(data, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_port,
          ctx->ip.local_ip, ctx->ip.local_port,
          curlx_strerror(ctx->sockerr, buffer, اندازه(buffer)));
    *done = FALSE;
  }
  بازگشت result;
}

ایستا CURLcode cf_socket_adjust_pollset(ساختار Curl_cfilter *cf,
                                         ساختار Curl_easy *data,
                                         ساختار easy_pollset *ps)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  اگر(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    اگر(ctx->listening) {
      result = Curl_pollset_set_in_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    وگرنه اگر(!cf->connected) {
      result = Curl_pollset_set_out_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    وگرنه اگر(!ctx->active) {
      result = Curl_pollset_add_in(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  بازگشت result;
}

#ifdef USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ifndef SIO_IDEAL_SEND_BACKLOG_QUERY
#define SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#endif

ایستا تهی win_update_sndbuf_size(ساختار Curl_easy *data,
                                   ساختار cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  اگر(curlx_ptimediff_ms(Curl_pgrs_now(data),
                        &ctx->last_sndbuf_query_at) > 1000) {
    اگر(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, اندازه(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_size &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (ثابت نویسه *)&ideal, اندازه(ideal))) {
      ctx->sndbuf_size = ideal;
    }
    ctx->last_sndbuf_query_at = *Curl_pgrs_now(data);
  }
}

#endif /* USE_WINSOCK */

ایستا CURLcode cf_socket_send(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (تهی)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  اگر(ctx->wblock_percent > 0) {
    unsigned نویسه c = 0;
    Curl_rand_bytes(data, FALSE, &c, 1);
    اگر(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      بازگشت CURLE_AGAIN;
    }
  }
  اگر(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    اگر(!len)
      len = 1;
    CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#endif

#اگر defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  اگر(cf->conn->bits.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->bits.tcp_fastopen = FALSE;
  }
  وگرنه
#endif
    rv = swrite(ctx->sock, buf, len);

  اگر(!curlx_sztouz(rv, pnwritten)) {
    صحیح sockerr = SOCKERRNO;
    اگر(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    وگرنه {
      نویسه buffer[STRERROR_LEN];
      failf(data, "Send failure: %s",
            curlx_strerror(sockerr, buffer, اندازه(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_SEND_ERROR;
    }
  }

#ifdef USE_WINSOCK
  اگر(!result)
    win_update_sndbuf_size(data, ctx);
#endif

  CURL_TRC_CF(data, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (صحیح)result, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  بازگشت result;
}

ایستا CURLcode cf_socket_recv(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                               نویسه *buf, size_t len, size_t *pnread)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial reads */
  اگر(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    unsigned نویسه c = 0;
    Curl_rand(data, &c, 1);
    اگر(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      بازگشت CURLE_AGAIN;
    }
  }
  اگر(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#endif

  rv = sread(ctx->sock, buf, len);

  اگر(!curlx_sztouz(rv, pnread)) {
    صحیح sockerr = SOCKERRNO;
    اگر(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    وگرنه {
      نویسه buffer[STRERROR_LEN];
      failf(data, "Recv failure: %s",
            curlx_strerror(sockerr, buffer, اندازه(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(data, cf, "recv(len=%zu) -> %d, %zu", len, (صحیح)result, *pnread);
  اگر(!result && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(data);
    ctx->got_first_byte = TRUE;
  }
  بازگشت result;
}

ایستا تهی cf_socket_update_data(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data)
{
  /* Update the IP info held in the transfer, if we have that. */
  اگر(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    ساختار cf_socket_ctx *ctx = cf->ctx;
    data->info.primary = ctx->ip;
  }
}

ایستا تهی cf_socket_active(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  set_local_ip(cf, data);
#ifdef USE_IPV6
  اگر(cf->sockindex == FIRSTSOCKET)
    cf->conn->bits.ipv6 = (ctx->addr.family == AF_INET6);
#endif
  ctx->active = TRUE;
}

ایستا CURLcode cf_socket_cntrl(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                صحیح event, صحیح arg1, تهی *arg2)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;

  (تهی)arg1;
  (تهی)arg2;
  انتخاب(event) {
  حالت CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_active(cf, data);
    cf_socket_update_data(cf, data);
    بشکن;
  حالت CF_CTRL_DATA_SETUP:
    cf_socket_update_data(cf, data);
    بشکن;
  حالت CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    بشکن;
  حالت CF_CTRL_REPORT_STATS:
    اگر(cf->connected && !ctx->stats_reported) {
      ساختار curltime *ts = پوچ;
      انتخاب(ctx->transport) {
      حالت TRNSPRT_UDP:
      حالت TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        اگر(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        بشکن;
      پیشفرض:
        ts = &ctx->connected_at;
        بشکن;
      }
      اگر(ts) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  بازگشت CURLE_OK;
}

ایستا bool cf_socket_conn_is_alive(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    bool *درون_داد_pending)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  ساختار pollfd pfd[1];
  صحیح r;

  *درون_داد_pending = FALSE;

  اگر(!ctx || ctx->sock == CURL_SOCKET_BAD)
    بازگشت FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  اگر(r < 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll error, assume dead");
    بازگشت FALSE;
  }
  وگرنه اگر(r == 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll timeout, assume alive");
    بازگشت TRUE;
  }
  وگرنه اگر(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(data, cf, "is_alive: err/hup/etc events, assume dead");
    بازگشت FALSE;
  }

  CURL_TRC_CF(data, cf, "is_alive: valid events, looks alive");
  *درون_داد_pending = TRUE;
  بازگشت TRUE;
}

ایستا CURLcode cf_socket_query(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    بازگشت CURLE_OK;
  حالت CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    بازگشت CURLE_OK;
  حالت CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((ثابت ساختار Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : پوچ;
    بازگشت CURLE_OK;
  حالت CF_QUERY_CONNECT_REPLY_MS:
    اگر(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (صحیح)ms : INT_MAX;
    }
    وگرنه
      *pres1 = -1;
    بازگشت CURLE_OK;
  حالت CF_QUERY_IP_INFO:
#ifdef USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#وگرنه
    *pres1 = FALSE;
#endif
    *(ساختار ip_quadruple *)pres2 = ctx->ip;
    بازگشت CURLE_OK;
  حالت CF_QUERY_REALLY_CONNECTED:
    اگر(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    وگرنه
      *pres1 = ctx->got_first_byte;
    بازگشت CURLE_OK;
  پیشفرض:
    بشکن;
  }
  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساختار Curl_cftype Curl_cft_tcp = {
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

CURLcode Curl_cf_tcp_create(ساختار Curl_cfilter **pcf,
                            ساختار Curl_easy *data,
                            ساختار Curl_peer *origin,
                            ساختار Curl_peer *peer,
                            uint8_t transport_peer,
                            ساختار connectdata *conn,
                            ساختار Curl_sockaddr_ex *addr,
                            ساختار Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ساختار cf_socket_ctx *ctx = پوچ;
  ساختار Curl_cfilter *cf = پوچ;
  CURLcode result;

  (تهی)data;
  (تهی)origin;
  (تهی)conn;
  (تهی)tunnel_peer;
  (تهی)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  اگر(!addr) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو out;
  }

  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  اگر(result)
    برو out;

  result = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

out:
  *pcf = (!result) ? cf : پوچ;
  اگر(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  بازگشت result;
}

#ifdef __linux__
ایستا تهی linux_quic_mtu(ساختار cf_socket_ctx *ctx)
{
  صحیح val;
  انتخاب(ctx->addr.family) {
#ifdef IP_MTU_DISCOVER
  حالت AF_INET:
    val = IP_PMTUDISC_DO;
    (تهی)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     اندازه(val));
    بشکن;
#endif
#ifdef IPV6_MTU_DISCOVER
  حالت AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (تهی)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     اندازه(val));
    بشکن;
#endif
  }
}
#وگرنه
#define linux_quic_mtu(x)
#endif

#اگر defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
ایستا تهی linux_quic_gro(ساختار cf_socket_ctx *ctx)
{
  صحیح one = 1;
  (تهی)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)اندازه(one));
}
#وگرنه
#define linux_quic_gro(x)
#endif

#اگر (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
ایستا تهی linux_quic_ecn(ساختار cf_socket_ctx *ctx)
{
  unsigned صحیح tos = 1;
  انتخاب(ctx->addr.family) {
  حالت AF_INET:
    (تهی)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, اندازه(tos));
    بشکن;
#ifdef IPV6_RECVTCLASS
  حالت AF_INET6:
    (تهی)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, اندازه(tos));
    بشکن;
#endif
  }
}
#وگرنه
#define linux_quic_ecn(x)
#endif

#اگر (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
ایستا تهی linux_ip_dontfrag(ساختار cf_socket_ctx *ctx)
{
  صحیح val = 1;

  انتخاب(ctx->addr.family) {
  حالت AF_INET:
    (تهی)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, اندازه(val));
    بشکن;
#ifdef IPV6_DONTFRAG
  حالت AF_INET6:
    (تهی)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, اندازه(val));
    بشکن;
#endif
  }
}
#وگرنه
#define linux_ip_dontfrag(x)
#endif

ایستا CURLcode cf_udp_setup_quic(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  صحیح rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  اگر(rc == -1) {
    بازگشت socket_connect_result(data, ctx->ip.remote_ip, SOCKERRNO);
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

  بازگشت CURLE_OK;
}

ایستا CURLcode cf_udp_connect(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               bool *done)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  *done = FALSE;
  اگر(ctx->sock == CURL_SOCKET_BAD) {
    result = cf_socket_open(cf, data);
    اگر(result) {
      CURL_TRC_CF(data, cf, "cf_udp_connect(), open failed -> %d",
                  (صحیح)result);
      برو out;
    }

    اگر(ctx->transport == TRNSPRT_QUIC) {
      result = cf_udp_setup_quic(cf, data);
      اگر(result)
        برو out;
      CURL_TRC_CF(data, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
    }
    *done = TRUE;
    cf->connected = TRUE;
  }
out:
  بازگشت result;
}

ساختار Curl_cftype Curl_cft_udp = {
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

CURLcode Curl_cf_udp_create(ساختار Curl_cfilter **pcf,
                            ساختار Curl_easy *data,
                            ساختار Curl_peer *origin,
                            ساختار Curl_peer *peer,
                            uint8_t transport_peer,
                            ساختار connectdata *conn,
                            ساختار Curl_sockaddr_ex *addr,
                            ساختار Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ساختار cf_socket_ctx *ctx = پوچ;
  ساختار Curl_cfilter *cf = پوچ;
  CURLcode result;

  (تهی)data;
  (تهی)origin;
  (تهی)conn;
  (تهی)tunnel_peer;
  (تهی)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  اگر(result)
    برو out;

  result = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

out:
  *pcf = (!result) ? cf : پوچ;
  اگر(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  بازگشت result;
}

/* this is the TCP filter which can also handle this case */
ساختار Curl_cftype Curl_cft_unix = {
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

CURLcode Curl_cf_unix_create(ساختار Curl_cfilter **pcf,
                            ساختار Curl_easy *data,
                            ساختار Curl_peer *origin,
                            ساختار Curl_peer *peer,
                            uint8_t transport_peer,
                            ساختار connectdata *conn,
                            ساختار Curl_sockaddr_ex *addr,
                            ساختار Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ساختار cf_socket_ctx *ctx = پوچ;
  ساختار Curl_cfilter *cf = پوچ;
  CURLcode result;

  (تهی)data;
  (تهی)origin;
  (تهی)conn;
  (تهی)tunnel_peer;
  (تهی)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  اگر(result)
    برو out;

  result = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

out:
  *pcf = (!result) ? cf : پوچ;
  اگر(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  بازگشت result;
}

ایستا timediff_t cf_tcp_accept_timeleft(ساختار Curl_cfilter *cf,
                                         ساختار Curl_easy *data)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ifndef CURL_DISABLE_FTP
  اگر(data->set.accepttimeout > 0)
    timeout_ms = data->set.accepttimeout;
#endif

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(data);
  اگر(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  وگرنه {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->started_at);
    اگر(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  بازگشت timeout_ms;
}

ایستا تهی cf_tcp_set_accepted_remote_ip(ساختار Curl_cfilter *cf,
                                          ساختار Curl_easy *data)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
#ifdef HAVE_GETPEERNAME
  CURLcode result;
  ساختار Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  plen = اندازه(ssrem);
  memset(&ssrem, 0, plen);
  اگر(getpeername(ctx->sock, (ساختار sockaddr *)&ssrem, &plen)) {
    نویسه buffer[STRERROR_LEN];
    صحیح sockerr = SOCKERRNO;
    failf(data, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, اندازه(buffer)));
    بازگشت;
  }
  result = sockaddr2string((ساختار sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_port);
  اگر(result) {
    failf(data, "ssrem inet_ntop() failed with %d", (صحیح)result);
    بازگشت;
  }
#وگرنه
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  (تهی)data;
#endif
}

ایستا CURLcode cf_tcp_accept_connect(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      bool *done)
{
  ساختار cf_socket_ctx *ctx = cf->ctx;
  نویسه errbuf[STRERROR_LEN];
#ifdef USE_IPV6
  ساختار Curl_sockaddr_storage add;
#وگرنه
  ساختار sockaddr_in add;
#endif
  curl_socklen_t size = (curl_socklen_t)اندازه(add);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  صحیح socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (تهی)data;
  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  *done = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, data);
  اگر(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(data, "Accept timeout occurred while waiting server connect");
    بازگشت CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(data, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(data, cf, "socket_check -> %x", (unsigned صحیح)socketstate);
  انتخاب(socketstate) {
  حالت -1: /* error */
    /* let's die here */
    failf(data, "Error while waiting for server connect");
    بازگشت CURLE_FTP_ACCEPT_FAILED;
  پیشفرض:
    اگر(socketstate & CURL_CSELECT_IN) {
      infof(data, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    بشکن;
  }

  اگر(!incoming) {
    CURL_TRC_CF(data, cf, "nothing heard from the server yet");
    بازگشت CURLE_OK;
  }

  size = اندازه(add);
#ifdef HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (ساختار sockaddr *)&add, &size,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#وگرنه
  s_accepted = CURL_ACCEPT(ctx->sock, (ساختار sockaddr *)&add, &size);
#endif

  اگر(s_accepted == CURL_SOCKET_BAD) {
    failf(data, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, اندازه(errbuf)));
    بازگشت CURLE_FTP_ACCEPT_FAILED;
  }
#ifndef HAVE_ACCEPT4
#ifdef HAVE_FCNTL
  اگر(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, اندازه(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    بازگشت CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* HAVE_FCNTL */
  اگر(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(data, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, اندازه(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    بازگشت CURLE_FTP_ACCEPT_FAILED;
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

  اگر(data->set.fsockopt) {
    ساختار Curl_mapi_guard guard;
    صحیح error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_END(&guard);

    اگر(error)
      بازگشت CURLE_ABORTED_BY_CALLBACK;
  }
  *done = TRUE;
  بازگشت CURLE_OK;
}

ساختار Curl_cftype Curl_cft_tcp_accept = {
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

CURLcode Curl_conn_tcp_listen_set(ساختار Curl_easy *data,
                                  ساختار connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode result;
  ساختار Curl_cfilter *cf = پوچ;
  ساختار cf_socket_ctx *ctx = پوچ;

  /* replace any existing */
  Curl_conn_cf_discard_all(data, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  result = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  اگر(result)
    برو out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(data);
  conn->sock[sockindex] = ctx->sock;
  set_local_ip(cf, data);
  CURL_TRC_CF(data, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_port);

out:
  اگر(result) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  بازگشت result;
}

bool Curl_conn_is_tcp_listen(ساختار Curl_easy *data,
                             int8_t sockindex)
{
  ساختار Curl_cfilter *cf = data->conn->cfilter[sockindex];
  وقتی(cf) {
    اگر(cf->cft == &Curl_cft_tcp_accept)
      بازگشت TRUE;
    cf = cf->next;
  }
  بازگشت FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
ایستا bool cf_is_socket(ساختار Curl_cfilter *cf)
{
  بازگشت cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             curl_socket_t *psock,
                             ثابت ساختار Curl_sockaddr_ex **paddr,
                             ساختار ip_quadruple *pip)
{
  (تهی)data;
  اگر(cf_is_socket(cf) && cf->ctx) {
    ساختار cf_socket_ctx *ctx = cf->ctx;

    اگر(psock)
      *psock = ctx->sock;
    اگر(paddr)
      *paddr = &ctx->addr;
    اگر(pip)
      *pip = ctx->ip;
    بازگشت CURLE_OK;
  }
  بازگشت CURLE_FAILED_INIT;
}
