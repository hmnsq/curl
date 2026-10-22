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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_SYS_UN_H
#அந்தர்பாவ <sys/un.h> /* for sockaddr_un */
#endif
#ifdef HAVE_LINUX_TCP_H
#அந்தர்பாவ <linux/tcp.h>
#அல்லதெனில் defined(HAVE_NETINET_TCP_H)
#அந்தர்பாவ <netinet/tcp.h>
#endif
#ifdef HAVE_NETINET_UDP_H
#அந்தர்பாவ <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#அந்தர்பாவ <netinet/ip.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif

#ifdef HAVE_IFADDRS_H
#அந்தர்பாவ <ifaddrs.h>
#endif
#ifdef HAVE_NET_IF_H
#அந்தர்பாவ <net/if.h>
#endif
#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "if2ip.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-socket.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "curlx/inet_ntop.h"
#அந்தர்பாவ "curlx/inet_pton.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "conncache.h"
#அந்தர்பாவ "multihandle.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "sockaddr.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "curlx/nonblock.h"
#அந்தர்பாவ "curlx/strcopy.h"
#அந்தர்பாவ "curlx/version_win32.h"
#அந்தர்பாவ "curlx/strerr.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "curlx/wait.h"
#ifdef _WIN32
#அந்தர்பாவ <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#endif

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(கட்டமைப்பு sockaddr *sa, curl_socklen_t salen,
                                  எழுத்து *addr, uint16_t *port);
UNITTEST CURLcode sockaddr2string(கட்டமைப்பு sockaddr *sa, curl_socklen_t salen,
                                  எழுத்து *addr, uint16_t *port)
{
  CURLcode result;
  கட்டமைப்பு sockaddr_in *si = வெற்று;
#ifdef USE_IPV6
  கட்டமைப்பு sockaddr_in6 *si6 = வெற்று;
#endif
#ifdef USE_UNIX_SOCKETS
  கட்டமைப்பு sockaddr_un *su = வெற்று;
#இல்லையெனில்
  (வெறுமை)salen;
#endif

  தேர்வு(sa->sa_family) {
  நிலை AF_INET:
    si = (கட்டமைப்பு sockaddr_in *)(வெறுமை *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    எனில்(!result) {
      *port = ntohs(si->sin_port);
      திரும்பு result;
    }
    நிறுத்து;
#ifdef USE_IPV6
  நிலை AF_INET6:
    si6 = (கட்டமைப்பு sockaddr_in6 *)(வெறுமை *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    எனில்(!result) {
      *port = ntohs(si6->sin6_port);
      திரும்பு result;
    }
    நிறுத்து;
#endif
#ifdef USE_UNIX_SOCKETS
  நிலை AF_UNIX:
    எனில்(salen > (curl_socklen_t)அளவு(CURL_SA_FAMILY_T)) {
      su = (கட்டமைப்பு sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_path);
    }
    இல்லையெனில்
      addr[0] = 0; /* socket with no name */
    *port = 0;
    திரும்பு CURLE_OK;
#endif
  இயல்பு:
    result = CURLE_UNSUPPORTED_PROTOCOL;
    நிறுத்து;
  }

  addr[0] = '\0';
  *port = 0;
  திரும்பு result;
}

நிலையான வெறுமை tcpnodelay(கட்டமைப்பு Curl_cfilter *cf,
                       கட்டமைப்பு Curl_easy *data,
                       curl_socket_t sockfd)
{
#எனில் defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  முழு level = IPPROTO_TCP;
  VERBOSE(எழுத்து buffer[STRERROR_LEN]);

  எனில்(setsockopt(sockfd, level, TCP_NODELAY, (வெறுமை *)&onoff, அளவு(onoff)) < 0)
    CURL_TRC_CF(data, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, buffer, அளவு(buffer)));
#இல்லையெனில்
  (வெறுமை)cf;
  (வெறுமை)data;
  (வெறுமை)sockfd;
#endif
}

#எனில் defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_THRESHOLD) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
#எனில் defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
நிலையான வெறுமை alive_unit(முழு *x)
{
  /* make sure this doesn't wrap */
  எனில்(*x < (INT_MAX / 1000))
    *x *= 1000;
  இல்லையெனில்
    /* this is still almost 25 days */
    *x = INT_MAX;
}
#இல்லையெனில்
#define alive_unit(x)
#endif
#endif

நிலையான வெறுமை tcpkeepalive(கட்டமைப்பு Curl_cfilter *cf,
                         கட்டமைப்பு Curl_easy *data,
                         curl_socket_t sockfd)
{
  முழு optval = data->set.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  எனில்(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (வெறுமை *)&optval, அளவு(optval)) < 0) {
    CURL_TRC_CF(data, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  இல்லையெனில் {
#ifdef USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    எனில்(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
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
      எனில்(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (மாறிலி எழுத்து *)&optval, அளவு(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      எனில்(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (மாறிலி எழுத்து *)&optval, அளவு(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepcnt);
      எனில்(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (மாறிலி எழுத்து *)&optval, அளவு(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    இல்லையெனில் {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ifndef SIO_KEEPALIVE_VALS
#define SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      கட்டமைப்பு tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#endif
      கட்டமைப்பு tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(data->set.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      எனில்(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, அளவு(vals),
                  வெற்று, 0, &dummy, வெற்று, வெற்று) != 0) {
        CURL_TRC_CF(data, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#இல்லையெனில் /* !USE_WINSOCK */
#ifdef TCP_KEEPIDLE
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    எனில்(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (வெறுமை *)&optval, அளவு(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#அல்லதெனில் defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    எனில்(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (வெறுமை *)&optval, அளவு(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#அல்லதெனில் defined(TCP_KEEPALIVE_THRESHOLD)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    எனில்(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_THRESHOLD,
                  (வெறுமை *)&optval, அளவு(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPINTVL
    optval = curlx_sltosi(data->set.tcp_keepintvl);
    alive_unit(&optval);
    எனில்(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (வெறுமை *)&optval, அளவு(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#அல்லதெனில் defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
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
      முழு keepcnt = curlx_sltosi(data->set.tcp_keepcnt);
      முழு keepintvl = curlx_sltosi(data->set.tcp_keepintvl);

      எனில்(keepcnt > 0 && keepintvl > (INT_MAX / keepcnt))
        optval = INT_MAX;
      இல்லையெனில்
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    எனில்(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_THRESHOLD,
                  (வெறுமை *)&optval, அளவு(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPCNT
    optval = curlx_sltosi(data->set.tcp_keepcnt);
    எனில்(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (வெறுமை *)&optval, அளவு(optval)) < 0) {
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
CURLcode Curl_socket_addr_from_ai(கட்டமைப்பு Curl_sockaddr_ex *addr,
                                  மாறிலி கட்டமைப்பு Curl_addrinfo *ai,
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
  addr->addrlen = (unsigned முழு)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= அளவு(addr->curl_sa_addrbuf));
  எனில்(addr->addrlen > அளவு(addr->curl_sa_addrbuf))
    திரும்பு CURLE_TOO_LARGE;

  memcpy(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  திரும்பு CURLE_OK;
}

#ifdef USE_SO_NOSIGPIPE
முழு Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  முழு onoff = 1;
  திரும்பு setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (வெறுமை *)&onoff, அளவு(onoff));
}
#endif /* USE_SO_NOSIGPIPE */

#எனில் defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
நிலையான uint32_t get_scope_id(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  எனில்(data->conn->scope_id)
    திரும்பு data->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  எனில்(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#எனில் defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    கட்டமைப்பு ifaddrs *ifa, *ifa_list;
    எனில்(getifaddrs(&ifa_list) == 0) {
      க்கு(ifa = ifa_list; ifa; ifa = ifa->ifa_next) {
        எனில்(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          கட்டமைப்பு sockaddr_in6 *s6 = (வெறுமை *)ifa->ifa_addr;
          எனில்(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(data,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (unsigned long)scope_id);
            நிறுத்து;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#endif /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  திரும்பு scope_id;
}
#endif

நிலையான CURLcode socket_open(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  எழுத்து errbuf[STRERROR_LEN];

#ifdef SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#endif

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  எனில்(data->set.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    கட்டமைப்பு Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fopensocket);
    *sockfd = data->set.fopensocket(data->set.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (கட்டமைப்பு curl_sockaddr *)addr);
    CURL_CBAPI_END(&guard);
  }
  இல்லையெனில் {
    /* opensocket callback not set, so create the socket now */
#ifdef DEBUGBUILD
    எனில்((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(data, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      திரும்பு CURLE_COULDNT_CONNECT;
    }
#endif
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    எனில்((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      திரும்பு CURLE_OUT_OF_MEMORY;
  }

  எனில்(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(data, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, அளவு(errbuf)));
    திரும்பு CURLE_COULDNT_CONNECT;
  }

#ifdef USE_SO_NOSIGPIPE
  எனில்(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(data, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, அளவு(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    திரும்பு CURLE_COULDNT_CONNECT;
  }
#endif /* USE_SO_NOSIGPIPE */

#எனில் defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  எனில்(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, அளவு(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    திரும்பு CURLE_COULDNT_CONNECT;
  }
#endif

#எனில் defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  எனில்(addr->family == AF_INET6) {
    கட்டமைப்பு sockaddr_in6 * மாறிலி sa6 = (வெறுமை *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(data, sa6);
  }
#endif
  திரும்பு CURLE_OK;
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
CURLcode Curl_socket_open(கட்டமைப்பு Curl_easy *data,
                          மாறிலி கட்டமைப்பு Curl_addrinfo *ai,
                          கட்டமைப்பு Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  கட்டமைப்பு Curl_sockaddr_ex dummy;
  CURLcode result;

  எனில்(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  result = Curl_socket_addr_from_ai(addr, ai, transport);
  எனில்(result)
    திரும்பு result;

  திரும்பு socket_open(data, addr, sockfd);
}

நிலையான முழு socket_close(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn,
                        முழு use_callback, curl_socket_t sock)
{
  எனில்(sock == CURL_SOCKET_BAD)
    திரும்பு 0;

  எனில்(use_callback && conn && conn->fclosesocket) {
    கட்டமைப்பு Curl_mapi_guard guard;
    முழு rc;
    Curl_multi_will_close(data, sock);
    CURL_CBAPI_START(&guard, data, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_END(&guard);
    திரும்பு rc;
  }

  எனில்(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(data, sock);

  sclose(sock);

  திரும்பு 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
முழு Curl_socket_close(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn,
                      curl_socket_t sock)
{
  திரும்பு socket_close(data, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(மாறிலி எழுத்து *உள்ளீடு,
                              எழுத்து **dev, எழுத்து **iface, எழுத்து **host)
{
  நிலையான மாறிலி எழுத்து if_prefix[] = "if!";
  நிலையான மாறிலி எழுத்து host_prefix[] = "host!";
  நிலையான மாறிலி எழுத்து if_host_prefix[] = "ifhost!";
  size_t len;

  len = strlen(உள்ளீடு);
  எனில்(len > 512)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(dev)
    *dev = வெற்று;
  எனில்(iface)
    *iface = வெற்று;
  எனில்(host)
    *host = வெற்று;

  எனில்(!strncmp(if_prefix, உள்ளீடு, strlen(if_prefix))) {
    உள்ளீடு += strlen(if_prefix);
    எனில்(!*உள்ளீடு)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    எனில்(iface) {
      *iface = curlx_memdup0(உள்ளீடு, len - strlen(if_prefix));
      திரும்பு *iface ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    திரும்பு CURLE_OK;
  }
  இல்லையெனில் எனில்(!strncmp(host_prefix, உள்ளீடு, strlen(host_prefix))) {
    உள்ளீடு += strlen(host_prefix);
    எனில்(!*உள்ளீடு)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    எனில்(host) {
      *host = curlx_memdup0(உள்ளீடு, len - strlen(host_prefix));
      திரும்பு *host ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    திரும்பு CURLE_OK;
  }
  இல்லையெனில் எனில்(!strncmp(if_host_prefix, உள்ளீடு, strlen(if_host_prefix))) {
    மாறிலி எழுத்து *host_part;
    உள்ளீடு += strlen(if_host_prefix);
    len -= strlen(if_host_prefix);
    host_part = memchr(உள்ளீடு, '!', len);
    எனில்(!host_part || !*(host_part + 1))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    எனில்(iface) {
      *iface = curlx_memdup0(உள்ளீடு, host_part - உள்ளீடு);
      எனில்(!*iface)
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
    ++host_part;
    எனில்(host) {
      *host = curlx_memdup0(host_part, len - (host_part - உள்ளீடு));
      எனில்(!*host) {
        எனில்(iface)
          curlx_safefree(*iface);
        திரும்பு CURLE_OUT_OF_MEMORY;
      }
    }
    திரும்பு CURLE_OK;
  }

  எனில்(!*உள்ளீடு)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  எனில்(dev) {
    *dev = curlx_memdup0(உள்ளீடு, len);
    திரும்பு *dev ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  திரும்பு CURLE_OK;
}

#ifndef CURL_DISABLE_BINDLOCAL
நிலையான CURLcode bindlocal(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn,
                          curl_socket_t sockfd, முழு af, unsigned முழு scope,
                          uint8_t transport)
{
  கட்டமைப்பு Curl_sockaddr_storage sa;
  கட்டமைப்பு sockaddr *sock = (கட்டமைப்பு sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  கட்டமைப்பு sockaddr_in *si4 = (கட்டமைப்பு sockaddr_in *)&sa;
#ifdef USE_IPV6
  கட்டமைப்பு sockaddr_in6 *si6 = (கட்டமைப்பு sockaddr_in6 *)&sa;
#endif
  கட்டமைப்பு Curl_dns_entry *h = வெற்று;
  unsigned short port = data->set.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  முழு portnum = data->set.localportrange;
  எழுத்து *dev_in = வெற்று, *iface_in = வெற்று, *host_in = வெற்று;
  மாறிலி எழுத்து *iface = வெற்று, *host = வெற்று;
  முழு sockerr;
#ifdef IP_BIND_ADDRESS_NO_PORT
  முழு on = 1;
#endif
  CURLcode result = CURLE_OK;

#ifndef USE_IPV6
  (வெறுமை)scope;
#endif
  எனில்(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    result = Curl_parse_interface(conn->interface_opts,
                                  &dev_in, &iface_in, &host_in);
    எனில்(result)
      செல் out;
    /* This is how we use the values */
    iface = iface_in ? iface_in : dev_in;
    host = host_in ? host_in : dev_in;
  }

  எனில்(!iface && !host && !port)
    செல் out;

  எனில்(iface && (strlen(iface) >= 255)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }
  memset(&sa, 0, அளவு(கட்டமைப்பு Curl_sockaddr_storage));

  எனில்(iface || host) {
    எழுத்து myhost[256] = "";
    முழு done = 0; /* -1 for error, 1 for address found */
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
    எனில்(iface &&
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
      செல் out;
    }
#endif
    எனில்(!host_in) {
      /* Discover IP from input device, then bind to it */
      if2ip_result = Curl_if2ip(af,
#ifdef USE_IPV6
                                scope, conn->scope_id,
#endif
                                iface, myhost, அளவு(myhost));
    }
    தேர்வு(if2ip_result) {
    நிலை IF2IP_NOT_FOUND:
      எனில்(iface_in && !host_in) {
        /* Do not fall back to treating it as a hostname */
        எழுத்து buffer[STRERROR_LEN];
        data->state.os_errno = sockerr = SOCKERRNO;
        failf(data, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, buffer, அளவு(buffer)));
        result = CURLE_INTERFACE_FAILED;
        செல் out;
      }
      நிறுத்து;
    நிலை IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      result = CURLE_UNSUPPORTED_PROTOCOL;
      செல் out;
    நிலை IF2IP_FOUND:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(data, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      done = 1;
      நிறுத்து;
    }
    எனில்(!iface_in || host_in) {
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
      எனில்(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#endif

      (வெறுமை)Curl_resolv_blocking(data, dns_queries, host, 80, transport, &h);
      எனில்(h) {
        முழு h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_address(h->addr, myhost, அளவு(myhost));
        infof(data, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_entry_unlink(data, &h); /* this will NULL, potential free h */
        எனில்(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          result = CURLE_UNSUPPORTED_PROTOCOL;
          செல் out;
        }
        done = 1;
      }
      இல்லையெனில் {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        done = -1;
      }
    }

    எனில்(done > 0) {
#ifdef USE_IPV6
      /* IPv6 address */
      எனில்(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        எழுத்து *scope_ptr = strchr(myhost, '%');
        எனில்(scope_ptr)
          *(scope_ptr++) = '\0';
#endif
        எனில்(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          எனில்(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            எனில்(curlx_str_எண்((மாறிலி எழுத்து **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              result = CURLE_UNSUPPORTED_PROTOCOL;
              செல் out;
            }
            si6->sin6_scope_id = (unsigned முழு)scope_id;
          }
#endif
        }
        sizeof_sa = அளவு(கட்டமைப்பு sockaddr_in6);
      }
      இல்லையெனில்
#endif
      /* IPv4 address */
      எனில்((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(port);
        sizeof_sa = அளவு(கட்டமைப்பு sockaddr_in);
      }
    }

    எனில்(done < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      எழுத்து buffer[STRERROR_LEN];
      data->state.errorbuf = FALSE;
      data->state.os_errno = sockerr = SOCKERRNO;
      failf(data, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, buffer, அளவு(buffer)));
      result = CURLE_INTERFACE_FAILED;
      செல் out;
    }
  }
  இல்லையெனில் {
    /* no device was given, prepare sa to match af's needs */
#ifdef USE_IPV6
    எனில்(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(port);
      sizeof_sa = அளவு(கட்டமைப்பு sockaddr_in6);
    }
    இல்லையெனில்
#endif
    எனில்(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(port);
      sizeof_sa = அளவு(கட்டமைப்பு sockaddr_in);
    }
  }
#ifdef IP_BIND_ADDRESS_NO_PORT
  (வெறுமை)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, அளவு(on));
#endif
  க்கு(;;) {
    எனில்(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(data, "Local port: %hu", port);
      conn->bits.bound = TRUE;
      result = CURLE_OK;
      செல் out;
    }

    எனில்(--portnum > 0) {
      port++; /* try next port */
      எனில்(port == 0)
        நிறுத்து;
      infof(data, "Bind to local port %d failed, trying next", port - 1);
      /* We reuse/clobber the port variable here below */
      எனில்(sock->sa_family == AF_INET)
        si4->sin_port = htons(port);
#ifdef USE_IPV6
      இல்லையெனில்
        si6->sin6_port = htons(port);
#endif
    }
    இல்லையெனில்
      நிறுத்து;
  }
  {
    எழுத்து buffer[STRERROR_LEN];
    data->state.os_errno = sockerr = SOCKERRNO;
    failf(data, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, அளவு(buffer)));
  }

  result = CURLE_INTERFACE_FAILED;

out:
  curlx_free(dev_in);
  curlx_free(iface_in);
  curlx_free(host_in);
  திரும்பு result;
}
#endif

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
நிலையான bool verifyconnect(curl_socket_t sockfd, முழு *psockerr)
{
  bool rc = TRUE;
#ifdef SO_ERROR
  முழு sockerr = 0;
  curl_socklen_t errSize = அளவு(sockerr);

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

  எனில்(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (வெறுமை *)&sockerr, &errSize))
    sockerr = SOCKERRNO;
#எனில் defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  எனில்(EBADIOCTL == sockerr) {
    SET_SOCKERRNO(0);
    sockerr = 0;
  }
#endif
  எனில்((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  இல்லையெனில்
    /* This was not a successful connect */
    rc = FALSE;
  எனில்(psockerr)
    *psockerr = sockerr;
#இல்லையெனில்
  (வெறுமை)sockfd;
  எனில்(psockerr)
    *psockerr = SOCKERRNO;
#endif
  திரும்பு rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
நிலையான CURLcode socket_connect_result(கட்டமைப்பு Curl_easy *data,
                                      மாறிலி எழுத்து *ipaddress, முழு sockerr)
{
  எனில்(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    திரும்பு CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(எழுத்து buffer[STRERROR_LEN]);
    infof(data, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, buffer, அளவு(buffer)));
    NOVERBOSE((வெறுமை)ipaddress);
  }
  data->state.os_errno = sockerr;
  /* connect failed */
  திரும்பு CURLE_COULDNT_CONNECT;
}

கட்டமைப்பு cf_socket_ctx {
  கட்டமைப்பு Curl_peer *peer;
  கட்டமைப்பு Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  கட்டமைப்பு ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  கட்டமைப்பு curltime started_at;        /* when socket was created */
  கட்டமைப்பு curltime connected_at;      /* when socket connected/got first byte */
  கட்டமைப்பு curltime first_byte_at;     /* when first byte was recvd */
#ifdef USE_WINSOCK
  கட்டமைப்பு curltime last_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_size;                     /* the last set SO_SNDBUF size */
#endif
  முழு sockerr;                       /* socket error of last failure or 0 */
#ifdef DEBUGBUILD
  முழு connect_wait_ms;               /* ms to wait after 1st connect() */
  முழு wblock_percent;                /* percent of writes doing EAGAIN */
  முழு wpartial_percent;              /* percent of bytes written in send */
  முழு rblock_percent;                /* percent of reads doing EAGAIN */
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

நிலையான CURLcode cf_socket_ctx_init(கட்டமைப்பு cf_socket_ctx *ctx,
                                   கட்டமைப்பு Curl_peer *peer,
                                   கட்டமைப்பு Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, அளவு(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ifdef DEBUGBUILD
  {
    மாறிலி எழுத்து *p = getenv("CURL_DBG_SOCK_WBLOCK");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, 100))
        ctx->wblock_percent = (முழு)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, 100))
        ctx->wpartial_percent = (முழு)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, 100))
        ctx->rblock_percent = (முழு)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, INT_MAX))
        ctx->connect_wait_ms = (முழு)l;
    }
  }
#endif

  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cf_socket_ctx_free(கட்டமைப்பு cf_socket_ctx *ctx)
{
  எனில்(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

நிலையான CURLcode cf_socket_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   bool *done)
{
  எனில்(cf->connected) {
    கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(data, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    எனில்(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      unsigned எழுத்து buf[1024];
      (வெறுமை)sread(ctx->sock, buf, அளவு(buf));
    }
  }
  *done = TRUE;
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cf_socket_destroy(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  எனில்(ctx) {
    எனில்(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(data, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      எனில்(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(data, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

நிலையான வெறுமை set_local_ip(கட்டமைப்பு Curl_cfilter *cf,
                         கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_port = 0;

#ifdef HAVE_GETSOCKNAME
  எனில்((ctx->sock != CURL_SOCKET_BAD) &&
     !(data->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    கட்டமைப்பு Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = அளவு(கட்டமைப்பு Curl_sockaddr_storage);

    memset(&ssloc, 0, அளவு(ssloc));
    எனில்(getsockname(ctx->sock, (கட்டமைப்பு sockaddr *)&ssloc, &slen)) {
      VERBOSE(எழுத்து buffer[STRERROR_LEN]);
      VERBOSE(முழு sockerr = SOCKERRNO);
      infof(data, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, buffer, அளவு(buffer)));
    }
    இல்லையெனில் {
      CURLcode result = sockaddr2string((கட்டமைப்பு sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_port);
      எனில்(result)
        infof(data, "ssloc inet_ntop() failed with %d", (முழு)result);
    }
  }
#இல்லையெனில்
  (வெறுமை)data;
#endif
}

நிலையான CURLcode set_remote_ip(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  CURLcode result;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  result = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_port);
  எனில்(result) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(data, "curl_sa_addr inet_ntop() failed with %d", (முழு)result);
    திரும்பு CURLE_FAILED_INIT;
  }
  திரும்பு CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
நிலையான முழு cf_socktype(முழு x)
{
#ifdef SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#endif
  திரும்பு x;
}

#ifdef _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ifndef SIO_TCP_INITIAL_RTO
#define SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#define TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
வகைவரை கட்டமைப்பு _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#endif /* SIO_TCP_INITIAL_RTO */

#ifndef TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#define TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#endif

நிலையான bool targets_localhost(கட்டமைப்பு cf_socket_ctx *ctx)
{
  திரும்பு (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
நிலையான வெறுமை tcplocalhost(கட்டமைப்பு Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  எனில்(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD bytes = 0;
    memset(&rto, 0, அளவு(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    எனில்(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    இல்லையெனில்
      rto.MaxSynRetransmissions = 1;
    (வெறுமை)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, அளவு(rto),
                   வெற்று, 0, &bytes, வெற்று, வெற்று);
  }
}
#இல்லையெனில்
#define tcplocalhost(x, y)
#endif /* _WIN32 */

நிலையான CURLcode cf_socket_open(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  முழு error = 0;
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
  எனில்(!data->set.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#endif
  result = socket_open(data, &ctx->addr, &ctx->sock);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  எனில்(!data->set.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#endif
  எனில்(result)
    செல் out;

  result = set_remote_ip(cf, data);
  எனில்(result)
    செல் out;

#ifdef USE_IPV6
  எனில்(ctx->addr.family == AF_INET6) {
#ifdef USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    முழு on = 0;
    (வெறுமை)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (வெறுமை *)&on, அளவு(on));
#endif
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      கட்டமைப்பு sockaddr_in6 *sa6 = (வெறுமை *)&ctx->addr.curl_sa_addr;
      எனில்(sa6->sin6_scope_id)
        infof(data, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_port,
              (unsigned long)sa6->sin6_scope_id);
      இல்லையெனில்
#endif
        infof(data, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#endif
  }
  இல்லையெனில்
#endif
    infof(data, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_port);

#ifdef USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#இல்லையெனில்
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#endif
  எனில்(is_tcp) {
    எனில்(data->set.tcp_nodelay)
      tcpnodelay(cf, data, ctx->sock);

    எனில்(data->set.tcp_keepalive)
      tcpkeepalive(cf, data, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  எனில்(data->set.fsockopt) {
    /* activate callback for setting socket options */
    கட்டமைப்பு Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_END(&guard);

    எனில்(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    இல்லையெனில் எனில்(error) {
      result = CURLE_ABORTED_BY_CALLBACK;
      செல் out;
    }
  }

#ifndef CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  எனில்(ctx->addr.family == AF_INET
#ifdef USE_IPV6
     || ctx->addr.family == AF_INET6
#endif
    ) {
    result = bindlocal(data, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    எனில்(result) {
      எனில்(result == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        result = CURLE_COULDNT_CONNECT;
      }
      செல் out;
    }
  }
#endif

#ifndef CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  எனில்(error < 0) {
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    செல் out;
  }
#இல்லையெனில்
  எனில்(data->set.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    எனில்(error < 0) {
      result = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      செல் out;
    }
  }
#endif
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
out:
  எனில்(result) {
    எனில்(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  இல்லையெனில் எனில்(isconnected) {
    set_local_ip(cf, data);
    ctx->connected_at = *Curl_pgrs_now(data);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(data, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (முழு)result, ctx->sock);
  திரும்பு result;
}

நிலையான முழு do_connect(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                      bool is_tcp_fastopen)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
#ifdef TCP_FASTOPEN_CONNECT
  முழு optval = 1;
#endif
  முழு rc = -1;

  (வெறுமை)data;
  எனில்(is_tcp_fastopen) {
#ifdef CONNECT_DATA_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_AVAILABLE
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    எனில்(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = வெற்று;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_DATA_IDEMPOTENT,
                    வெற்று, 0, வெற்று, வெற்று);
    }
    இல்லையெனில் {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  இல்லையெனில்
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#அல்லதெனில் defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    எனில்(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (வெறுமை *)&optval, அளவு(optval)) < 0)
      CURL_TRC_CF(data, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#அல்லதெனில் defined(MSG_FASTOPEN) /* old Linux */
    எனில்(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    இல்லையெனில்
      rc = 0; /* Do nothing */
#endif
  }
  இல்லையெனில் {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  திரும்பு rc;
}

நிலையான CURLcode cf_tcp_connect(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               bool *done)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;
  முழு rc = 0;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  *done = FALSE; /* a negative world view is best */
  எனில்(ctx->sock == CURL_SOCKET_BAD) {
    முழு sockerr;

    result = cf_socket_open(cf, data);
    எனில்(result)
      செல் out;

    எனில்(cf->connected) {
      *done = TRUE;
      திரும்பு CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, data, (bool)cf->conn->bits.tcp_fastopen);
    sockerr = SOCKERRNO;
    set_local_ip(cf, data);
    CURL_TRC_CF(data, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_port);
#ifdef DEBUGBUILD
    எனில்(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(data, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#endif
    எனில்(rc == -1) {
      ctx->sockerr = sockerr;
      result = socket_connect_result(data, ctx->ip.remote_ip, sockerr);
      செல் out;
    }
  }

#ifdef mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (வெறுமை)verifyconnect(ctx->sock, வெற்று);
#endif
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  எனில்(rc == 0) { /* no connection yet */
    CURL_TRC_CF(data, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    திரும்பு CURLE_OK;
  }
  இல்லையெனில் எனில்(rc == CURL_CSELECT_OUT ||
#ifdef __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#endif
          cf->conn->bits.tcp_fastopen) {
    எனில்(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(data);
      set_local_ip(cf, data);
      *done = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(data, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      திரும்பு CURLE_OK;
    }
    result = CURLE_COULDNT_CONNECT;
  }
  இல்லையெனில் எனில்(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(data, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (வெறுமை)verifyconnect(ctx->sock, &ctx->sockerr);
    result = CURLE_COULDNT_CONNECT;
  }

out:
  எனில்(result) {
    VERBOSE(எழுத்து buffer[STRERROR_LEN]);
    set_local_ip(cf, data);
    எனில்(ctx->sockerr) {
      data->state.os_errno = ctx->sockerr;
      SET_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, buffer, அளவு(buffer)));
    }
    இல்லையெனில் {
      VERBOSE(curlx_strcopy(buffer, அளவு(buffer), STRCONST("peer closed")));
    }
    எனில்(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(data, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_port,
          ctx->ip.local_ip, ctx->ip.local_port,
          curlx_strerror(ctx->sockerr, buffer, அளவு(buffer)));
    *done = FALSE;
  }
  திரும்பு result;
}

நிலையான CURLcode cf_socket_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                         கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  எனில்(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    எனில்(ctx->listening) {
      result = Curl_pollset_set_in_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    இல்லையெனில் எனில்(!cf->connected) {
      result = Curl_pollset_set_out_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    இல்லையெனில் எனில்(!ctx->active) {
      result = Curl_pollset_add_in(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  திரும்பு result;
}

#ifdef USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ifndef SIO_IDEAL_SEND_BACKLOG_QUERY
#define SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#endif

நிலையான வெறுமை win_update_sndbuf_size(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  எனில்(curlx_ptimediff_ms(Curl_pgrs_now(data),
                        &ctx->last_sndbuf_query_at) > 1000) {
    எனில்(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, அளவு(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_size &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (மாறிலி எழுத்து *)&ideal, அளவு(ideal))) {
      ctx->sndbuf_size = ideal;
    }
    ctx->last_sndbuf_query_at = *Curl_pgrs_now(data);
  }
}

#endif /* USE_WINSOCK */

நிலையான CURLcode cf_socket_send(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                               மாறிலி uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (வெறுமை)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  எனில்(ctx->wblock_percent > 0) {
    unsigned எழுத்து c = 0;
    Curl_rand_bytes(data, FALSE, &c, 1);
    எனில்(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      திரும்பு CURLE_AGAIN;
    }
  }
  எனில்(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    எனில்(!len)
      len = 1;
    CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#endif

#எனில் defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  எனில்(cf->conn->bits.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->bits.tcp_fastopen = FALSE;
  }
  இல்லையெனில்
#endif
    rv = swrite(ctx->sock, buf, len);

  எனில்(!curlx_sztouz(rv, pnwritten)) {
    முழு sockerr = SOCKERRNO;
    எனில்(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    இல்லையெனில் {
      எழுத்து buffer[STRERROR_LEN];
      failf(data, "Send failure: %s",
            curlx_strerror(sockerr, buffer, அளவு(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_SEND_ERROR;
    }
  }

#ifdef USE_WINSOCK
  எனில்(!result)
    win_update_sndbuf_size(data, ctx);
#endif

  CURL_TRC_CF(data, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (முழு)result, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  திரும்பு result;
}

நிலையான CURLcode cf_socket_recv(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                               எழுத்து *buf, size_t len, size_t *pnread)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial reads */
  எனில்(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    unsigned எழுத்து c = 0;
    Curl_rand(data, &c, 1);
    எனில்(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      திரும்பு CURLE_AGAIN;
    }
  }
  எனில்(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#endif

  rv = sread(ctx->sock, buf, len);

  எனில்(!curlx_sztouz(rv, pnread)) {
    முழு sockerr = SOCKERRNO;
    எனில்(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    இல்லையெனில் {
      எழுத்து buffer[STRERROR_LEN];
      failf(data, "Recv failure: %s",
            curlx_strerror(sockerr, buffer, அளவு(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(data, cf, "recv(len=%zu) -> %d, %zu", len, (முழு)result, *pnread);
  எனில்(!result && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(data);
    ctx->got_first_byte = TRUE;
  }
  திரும்பு result;
}

நிலையான வெறுமை cf_socket_update_data(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data)
{
  /* Update the IP info held in the transfer, if we have that. */
  எனில்(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
    data->info.primary = ctx->ip;
  }
}

நிலையான வெறுமை cf_socket_active(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  set_local_ip(cf, data);
#ifdef USE_IPV6
  எனில்(cf->sockindex == FIRSTSOCKET)
    cf->conn->bits.ipv6 = (ctx->addr.family == AF_INET6);
#endif
  ctx->active = TRUE;
}

நிலையான CURLcode cf_socket_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                முழு event, முழு arg1, வெறுமை *arg2)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;

  (வெறுமை)arg1;
  (வெறுமை)arg2;
  தேர்வு(event) {
  நிலை CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_active(cf, data);
    cf_socket_update_data(cf, data);
    நிறுத்து;
  நிலை CF_CTRL_DATA_SETUP:
    cf_socket_update_data(cf, data);
    நிறுத்து;
  நிலை CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    நிறுத்து;
  நிலை CF_CTRL_REPORT_STATS:
    எனில்(cf->connected && !ctx->stats_reported) {
      கட்டமைப்பு curltime *ts = வெற்று;
      தேர்வு(ctx->transport) {
      நிலை TRNSPRT_UDP:
      நிலை TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        எனில்(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        நிறுத்து;
      இயல்பு:
        ts = &ctx->connected_at;
        நிறுத்து;
      }
      எனில்(ts) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  திரும்பு CURLE_OK;
}

நிலையான bool cf_socket_conn_is_alive(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    bool *உள்ளீடு_pending)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  கட்டமைப்பு pollfd pfd[1];
  முழு r;

  *உள்ளீடு_pending = FALSE;

  எனில்(!ctx || ctx->sock == CURL_SOCKET_BAD)
    திரும்பு FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  எனில்(r < 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll error, assume dead");
    திரும்பு FALSE;
  }
  இல்லையெனில் எனில்(r == 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll timeout, assume alive");
    திரும்பு TRUE;
  }
  இல்லையெனில் எனில்(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(data, cf, "is_alive: err/hup/etc events, assume dead");
    திரும்பு FALSE;
  }

  CURL_TRC_CF(data, cf, "is_alive: valid events, looks alive");
  *உள்ளீடு_pending = TRUE;
  திரும்பு TRUE;
}

நிலையான CURLcode cf_socket_query(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;

  தேர்வு(query) {
  நிலை CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((மாறிலி கட்டமைப்பு Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : வெற்று;
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_CONNECT_REPLY_MS:
    எனில்(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (முழு)ms : INT_MAX;
    }
    இல்லையெனில்
      *pres1 = -1;
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_IP_INFO:
#ifdef USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#இல்லையெனில்
    *pres1 = FALSE;
#endif
    *(கட்டமைப்பு ip_quadruple *)pres2 = ctx->ip;
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_REALLY_CONNECTED:
    எனில்(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    இல்லையெனில்
      *pres1 = ctx->got_first_byte;
    திரும்பு CURLE_OK;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

கட்டமைப்பு Curl_cftype Curl_cft_tcp = {
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

CURLcode Curl_cf_tcp_create(கட்டமைப்பு Curl_cfilter **pcf,
                            கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_peer *origin,
                            கட்டமைப்பு Curl_peer *peer,
                            uint8_t transport_peer,
                            கட்டமைப்பு connectdata *conn,
                            கட்டமைப்பு Curl_sockaddr_ex *addr,
                            கட்டமைப்பு Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  கட்டமைப்பு cf_socket_ctx *ctx = வெற்று;
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  CURLcode result;

  (வெறுமை)data;
  (வெறுமை)origin;
  (வெறுமை)conn;
  (வெறுமை)tunnel_peer;
  (வெறுமை)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  எனில்(!addr) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }

  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  எனில்(result)
    செல் out;

  result = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

out:
  *pcf = (!result) ? cf : வெற்று;
  எனில்(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  திரும்பு result;
}

#ifdef __linux__
நிலையான வெறுமை linux_quic_mtu(கட்டமைப்பு cf_socket_ctx *ctx)
{
  முழு val;
  தேர்வு(ctx->addr.family) {
#ifdef IP_MTU_DISCOVER
  நிலை AF_INET:
    val = IP_PMTUDISC_DO;
    (வெறுமை)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     அளவு(val));
    நிறுத்து;
#endif
#ifdef IPV6_MTU_DISCOVER
  நிலை AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (வெறுமை)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     அளவு(val));
    நிறுத்து;
#endif
  }
}
#இல்லையெனில்
#define linux_quic_mtu(x)
#endif

#எனில் defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
நிலையான வெறுமை linux_quic_gro(கட்டமைப்பு cf_socket_ctx *ctx)
{
  முழு one = 1;
  (வெறுமை)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)அளவு(one));
}
#இல்லையெனில்
#define linux_quic_gro(x)
#endif

#எனில் (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
நிலையான வெறுமை linux_quic_ecn(கட்டமைப்பு cf_socket_ctx *ctx)
{
  unsigned முழு tos = 1;
  தேர்வு(ctx->addr.family) {
  நிலை AF_INET:
    (வெறுமை)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, அளவு(tos));
    நிறுத்து;
#ifdef IPV6_RECVTCLASS
  நிலை AF_INET6:
    (வெறுமை)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, அளவு(tos));
    நிறுத்து;
#endif
  }
}
#இல்லையெனில்
#define linux_quic_ecn(x)
#endif

#எனில் (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
நிலையான வெறுமை linux_ip_dontfrag(கட்டமைப்பு cf_socket_ctx *ctx)
{
  முழு val = 1;

  தேர்வு(ctx->addr.family) {
  நிலை AF_INET:
    (வெறுமை)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, அளவு(val));
    நிறுத்து;
#ifdef IPV6_DONTFRAG
  நிலை AF_INET6:
    (வெறுமை)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, அளவு(val));
    நிறுத்து;
#endif
  }
}
#இல்லையெனில்
#define linux_ip_dontfrag(x)
#endif

நிலையான CURLcode cf_udp_setup_quic(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  முழு rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  எனில்(rc == -1) {
    திரும்பு socket_connect_result(data, ctx->ip.remote_ip, SOCKERRNO);
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

  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_udp_connect(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               bool *done)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  *done = FALSE;
  எனில்(ctx->sock == CURL_SOCKET_BAD) {
    result = cf_socket_open(cf, data);
    எனில்(result) {
      CURL_TRC_CF(data, cf, "cf_udp_connect(), open failed -> %d",
                  (முழு)result);
      செல் out;
    }

    எனில்(ctx->transport == TRNSPRT_QUIC) {
      result = cf_udp_setup_quic(cf, data);
      எனில்(result)
        செல் out;
      CURL_TRC_CF(data, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
    }
    *done = TRUE;
    cf->connected = TRUE;
  }
out:
  திரும்பு result;
}

கட்டமைப்பு Curl_cftype Curl_cft_udp = {
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

CURLcode Curl_cf_udp_create(கட்டமைப்பு Curl_cfilter **pcf,
                            கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_peer *origin,
                            கட்டமைப்பு Curl_peer *peer,
                            uint8_t transport_peer,
                            கட்டமைப்பு connectdata *conn,
                            கட்டமைப்பு Curl_sockaddr_ex *addr,
                            கட்டமைப்பு Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  கட்டமைப்பு cf_socket_ctx *ctx = வெற்று;
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  CURLcode result;

  (வெறுமை)data;
  (வெறுமை)origin;
  (வெறுமை)conn;
  (வெறுமை)tunnel_peer;
  (வெறுமை)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  எனில்(result)
    செல் out;

  result = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

out:
  *pcf = (!result) ? cf : வெற்று;
  எனில்(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  திரும்பு result;
}

/* this is the TCP filter which can also handle this case */
கட்டமைப்பு Curl_cftype Curl_cft_unix = {
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

CURLcode Curl_cf_unix_create(கட்டமைப்பு Curl_cfilter **pcf,
                            கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு Curl_peer *origin,
                            கட்டமைப்பு Curl_peer *peer,
                            uint8_t transport_peer,
                            கட்டமைப்பு connectdata *conn,
                            கட்டமைப்பு Curl_sockaddr_ex *addr,
                            கட்டமைப்பு Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  கட்டமைப்பு cf_socket_ctx *ctx = வெற்று;
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  CURLcode result;

  (வெறுமை)data;
  (வெறுமை)origin;
  (வெறுமை)conn;
  (வெறுமை)tunnel_peer;
  (வெறுமை)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  எனில்(result)
    செல் out;

  result = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

out:
  *pcf = (!result) ? cf : வெற்று;
  எனில்(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  திரும்பு result;
}

நிலையான timediff_t cf_tcp_accept_timeleft(கட்டமைப்பு Curl_cfilter *cf,
                                         கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ifndef CURL_DISABLE_FTP
  எனில்(data->set.accepttimeout > 0)
    timeout_ms = data->set.accepttimeout;
#endif

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(data);
  எனில்(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  இல்லையெனில் {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->started_at);
    எனில்(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  திரும்பு timeout_ms;
}

நிலையான வெறுமை cf_tcp_set_accepted_remote_ip(கட்டமைப்பு Curl_cfilter *cf,
                                          கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
#ifdef HAVE_GETPEERNAME
  CURLcode result;
  கட்டமைப்பு Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  plen = அளவு(ssrem);
  memset(&ssrem, 0, plen);
  எனில்(getpeername(ctx->sock, (கட்டமைப்பு sockaddr *)&ssrem, &plen)) {
    எழுத்து buffer[STRERROR_LEN];
    முழு sockerr = SOCKERRNO;
    failf(data, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, அளவு(buffer)));
    திரும்பு;
  }
  result = sockaddr2string((கட்டமைப்பு sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_port);
  எனில்(result) {
    failf(data, "ssrem inet_ntop() failed with %d", (முழு)result);
    திரும்பு;
  }
#இல்லையெனில்
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  (வெறுமை)data;
#endif
}

நிலையான CURLcode cf_tcp_accept_connect(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      bool *done)
{
  கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;
  எழுத்து errbuf[STRERROR_LEN];
#ifdef USE_IPV6
  கட்டமைப்பு Curl_sockaddr_storage add;
#இல்லையெனில்
  கட்டமைப்பு sockaddr_in add;
#endif
  curl_socklen_t size = (curl_socklen_t)அளவு(add);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  முழு socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (வெறுமை)data;
  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  *done = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, data);
  எனில்(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(data, "Accept timeout occurred while waiting server connect");
    திரும்பு CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(data, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(data, cf, "socket_check -> %x", (unsigned முழு)socketstate);
  தேர்வு(socketstate) {
  நிலை -1: /* error */
    /* let's die here */
    failf(data, "Error while waiting for server connect");
    திரும்பு CURLE_FTP_ACCEPT_FAILED;
  இயல்பு:
    எனில்(socketstate & CURL_CSELECT_IN) {
      infof(data, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    நிறுத்து;
  }

  எனில்(!incoming) {
    CURL_TRC_CF(data, cf, "nothing heard from the server yet");
    திரும்பு CURLE_OK;
  }

  size = அளவு(add);
#ifdef HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (கட்டமைப்பு sockaddr *)&add, &size,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#இல்லையெனில்
  s_accepted = CURL_ACCEPT(ctx->sock, (கட்டமைப்பு sockaddr *)&add, &size);
#endif

  எனில்(s_accepted == CURL_SOCKET_BAD) {
    failf(data, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, அளவு(errbuf)));
    திரும்பு CURLE_FTP_ACCEPT_FAILED;
  }
#ifndef HAVE_ACCEPT4
#ifdef HAVE_FCNTL
  எனில்(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, அளவு(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    திரும்பு CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* HAVE_FCNTL */
  எனில்(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(data, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, அளவு(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    திரும்பு CURLE_FTP_ACCEPT_FAILED;
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

  எனில்(data->set.fsockopt) {
    கட்டமைப்பு Curl_mapi_guard guard;
    முழு error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_END(&guard);

    எனில்(error)
      திரும்பு CURLE_ABORTED_BY_CALLBACK;
  }
  *done = TRUE;
  திரும்பு CURLE_OK;
}

கட்டமைப்பு Curl_cftype Curl_cft_tcp_accept = {
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

CURLcode Curl_conn_tcp_listen_set(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode result;
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு cf_socket_ctx *ctx = வெற்று;

  /* replace any existing */
  Curl_conn_cf_discard_all(data, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  result = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  எனில்(result)
    செல் out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(data);
  conn->sock[sockindex] = ctx->sock;
  set_local_ip(cf, data);
  CURL_TRC_CF(data, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_port);

out:
  எனில்(result) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  திரும்பு result;
}

bool Curl_conn_is_tcp_listen(கட்டமைப்பு Curl_easy *data,
                             int8_t sockindex)
{
  கட்டமைப்பு Curl_cfilter *cf = data->conn->cfilter[sockindex];
  போது(cf) {
    எனில்(cf->cft == &Curl_cft_tcp_accept)
      திரும்பு TRUE;
    cf = cf->next;
  }
  திரும்பு FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
நிலையான bool cf_is_socket(கட்டமைப்பு Curl_cfilter *cf)
{
  திரும்பு cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             curl_socket_t *psock,
                             மாறிலி கட்டமைப்பு Curl_sockaddr_ex **paddr,
                             கட்டமைப்பு ip_quadruple *pip)
{
  (வெறுமை)data;
  எனில்(cf_is_socket(cf) && cf->ctx) {
    கட்டமைப்பு cf_socket_ctx *ctx = cf->ctx;

    எனில்(psock)
      *psock = ctx->sock;
    எனில்(paddr)
      *paddr = &ctx->addr;
    எனில்(pip)
      *pip = ctx->ip;
    திரும்பு CURLE_OK;
  }
  திரும்பு CURLE_FAILED_INIT;
}
