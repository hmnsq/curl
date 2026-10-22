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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_SYS_UN_H
#अन्तर्भाव <sys/un.h> /* for sockaddr_un */
#endif
#ifdef HAVE_LINUX_TCP_H
#अन्तर्भाव <linux/tcp.h>
#किंवाजर defined(HAVE_NETINET_TCP_H)
#अन्तर्भाव <netinet/tcp.h>
#endif
#ifdef HAVE_NETINET_UDP_H
#अन्तर्भाव <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#अन्तर्भाव <netinet/ip.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif

#ifdef HAVE_IFADDRS_H
#अन्तर्भाव <ifaddrs.h>
#endif
#ifdef HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "if2ip.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-socket.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "curlx/inet_ntop.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "conncache.h"
#अन्तर्भाव "multihandle.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "sockaddr.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/nonblock.h"
#अन्तर्भाव "curlx/strcopy.h"
#अन्तर्भाव "curlx/version_win32.h"
#अन्तर्भाव "curlx/strerr.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curlx/wait.h"
#ifdef _WIN32
#अन्तर्भाव <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#endif

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(संरचना sockaddr *sa, curl_socklen_t salen,
                                  अक्षर *addr, uint16_t *port);
UNITTEST CURLcode sockaddr2string(संरचना sockaddr *sa, curl_socklen_t salen,
                                  अक्षर *addr, uint16_t *port)
{
  CURLcode result;
  संरचना sockaddr_in *si = रिक्त;
#ifdef USE_IPV6
  संरचना sockaddr_in6 *si6 = रिक्त;
#endif
#ifdef USE_UNIX_SOCKETS
  संरचना sockaddr_un *su = रिक्त;
#अन्यथा
  (शून्य)salen;
#endif

  निवड(sa->sa_family) {
  स्थिती AF_INET:
    si = (संरचना sockaddr_in *)(शून्य *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    जर(!result) {
      *port = ntohs(si->sin_port);
      परत result;
    }
    थांबा;
#ifdef USE_IPV6
  स्थिती AF_INET6:
    si6 = (संरचना sockaddr_in6 *)(शून्य *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    जर(!result) {
      *port = ntohs(si6->sin6_port);
      परत result;
    }
    थांबा;
#endif
#ifdef USE_UNIX_SOCKETS
  स्थिती AF_UNIX:
    जर(salen > (curl_socklen_t)आकार(CURL_SA_FAMILY_T)) {
      su = (संरचना sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_path);
    }
    अन्यथा
      addr[0] = 0; /* socket with no name */
    *port = 0;
    परत CURLE_OK;
#endif
  पूर्वनिर्धारित:
    result = CURLE_UNSUPPORTED_PROTOCOL;
    थांबा;
  }

  addr[0] = '\0';
  *port = 0;
  परत result;
}

स्थिर शून्य tcpnodelay(संरचना Curl_cfilter *cf,
                       संरचना Curl_easy *data,
                       curl_socket_t sockfd)
{
#जर defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  पूर्णांक level = IPPROTO_TCP;
  VERBOSE(अक्षर buffer[STRERROR_LEN]);

  जर(setsockopt(sockfd, level, TCP_NODELAY, (शून्य *)&onoff, आकार(onoff)) < 0)
    CURL_TRC_CF(data, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, buffer, आकार(buffer)));
#अन्यथा
  (शून्य)cf;
  (शून्य)data;
  (शून्य)sockfd;
#endif
}

#जर defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_THRESHOLD) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
#जर defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
स्थिर शून्य alive_unit(पूर्णांक *x)
{
  /* make sure this doesn't wrap */
  जर(*x < (पूर्णांक_उच्च / 1000))
    *x *= 1000;
  अन्यथा
    /* this is still almost 25 days */
    *x = पूर्णांक_उच्च;
}
#अन्यथा
#define alive_unit(x)
#endif
#endif

स्थिर शून्य tcpkeepalive(संरचना Curl_cfilter *cf,
                         संरचना Curl_easy *data,
                         curl_socket_t sockfd)
{
  पूर्णांक optval = data->set.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  जर(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (शून्य *)&optval, आकार(optval)) < 0) {
    CURL_TRC_CF(data, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  अन्यथा {
#ifdef USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    जर(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
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
      जर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (स्थिरांक अक्षर *)&optval, आकार(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      जर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (स्थिरांक अक्षर *)&optval, आकार(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepcnt);
      जर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (स्थिरांक अक्षर *)&optval, आकार(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    अन्यथा {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ifndef SIO_KEEPALIVE_VALS
#define SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      संरचना tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#endif
      संरचना tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(data->set.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      जर(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, आकार(vals),
                  रिक्त, 0, &dummy, रिक्त, रिक्त) != 0) {
        CURL_TRC_CF(data, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#अन्यथा /* !USE_WINSOCK */
#ifdef TCP_KEEPIDLE
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    जर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (शून्य *)&optval, आकार(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#किंवाजर defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    जर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (शून्य *)&optval, आकार(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#किंवाजर defined(TCP_KEEPALIVE_THRESHOLD)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    जर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_THRESHOLD,
                  (शून्य *)&optval, आकार(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPINTVL
    optval = curlx_sltosi(data->set.tcp_keepintvl);
    alive_unit(&optval);
    जर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (शून्य *)&optval, आकार(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#किंवाजर defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
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
      पूर्णांक keepcnt = curlx_sltosi(data->set.tcp_keepcnt);
      पूर्णांक keepintvl = curlx_sltosi(data->set.tcp_keepintvl);

      जर(keepcnt > 0 && keepintvl > (पूर्णांक_उच्च / keepcnt))
        optval = पूर्णांक_उच्च;
      अन्यथा
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    जर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_THRESHOLD,
                  (शून्य *)&optval, आकार(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPCNT
    optval = curlx_sltosi(data->set.tcp_keepcnt);
    जर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (शून्य *)&optval, आकार(optval)) < 0) {
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
CURLcode Curl_socket_addr_from_ai(संरचना Curl_sockaddr_ex *addr,
                                  स्थिरांक संरचना Curl_addrinfo *ai,
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
  addr->addrlen = (अचिन्हित पूर्णांक)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= आकार(addr->curl_sa_addrbuf));
  जर(addr->addrlen > आकार(addr->curl_sa_addrbuf))
    परत CURLE_TOO_LARGE;

  स_नकल(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  परत CURLE_OK;
}

#ifdef USE_SO_NOSIGPIPE
पूर्णांक Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  पूर्णांक onoff = 1;
  परत setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (शून्य *)&onoff, आकार(onoff));
}
#endif /* USE_SO_NOSIGPIPE */

#जर defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
स्थिर uint32_t get_scope_id(संरचना Curl_easy *data,
                             संरचना sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  जर(data->conn->scope_id)
    परत data->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  जर(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#जर defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    संरचना ifaddrs *ifa, *ifa_list;
    जर(getifaddrs(&ifa_list) == 0) {
      साठी(ifa = ifa_list; ifa; ifa = ifa->ifa_next) {
        जर(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          संरचना sockaddr_in6 *s6 = (शून्य *)ifa->ifa_addr;
          जर(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(data,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (अचिन्हित दीर्घ)scope_id);
            थांबा;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#endif /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  परत scope_id;
}
#endif

स्थिर CURLcode socket_open(संरचना Curl_easy *data,
                            संरचना Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  अक्षर errbuf[STRERROR_LEN];

#ifdef SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#endif

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  जर(data->set.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fopensocket);
    *sockfd = data->set.fopensocket(data->set.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (संरचना curl_sockaddr *)addr);
    CURL_CBAPI_END(&guard);
  }
  अन्यथा {
    /* opensocket callback not set, so create the socket now */
#ifdef DEBUGBUILD
    जर((addr->family == AF_INET6) && दो_पर्या("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(data, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      परत CURLE_COULDNT_CONNECT;
    }
#endif
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    जर((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      परत CURLE_OUT_OF_MEMORY;
  }

  जर(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(data, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, आकार(errbuf)));
    परत CURLE_COULDNT_CONNECT;
  }

#ifdef USE_SO_NOSIGPIPE
  जर(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(data, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, आकार(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    परत CURLE_COULDNT_CONNECT;
  }
#endif /* USE_SO_NOSIGPIPE */

#जर defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  जर(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, आकार(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    परत CURLE_COULDNT_CONNECT;
  }
#endif

#जर defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  जर(addr->family == AF_INET6) {
    संरचना sockaddr_in6 * स्थिरांक sa6 = (शून्य *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(data, sa6);
  }
#endif
  परत CURLE_OK;
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
CURLcode Curl_socket_open(संरचना Curl_easy *data,
                          स्थिरांक संरचना Curl_addrinfo *ai,
                          संरचना Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  संरचना Curl_sockaddr_ex dummy;
  CURLcode result;

  जर(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  result = Curl_socket_addr_from_ai(addr, ai, transport);
  जर(result)
    परत result;

  परत socket_open(data, addr, sockfd);
}

स्थिर पूर्णांक socket_close(संरचना Curl_easy *data, संरचना connectdata *conn,
                        पूर्णांक use_callback, curl_socket_t sock)
{
  जर(sock == CURL_SOCKET_BAD)
    परत 0;

  जर(use_callback && conn && conn->fclosesocket) {
    संरचना Curl_mapi_guard guard;
    पूर्णांक rc;
    Curl_multi_will_close(data, sock);
    CURL_CBAPI_START(&guard, data, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_END(&guard);
    परत rc;
  }

  जर(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(data, sock);

  sclose(sock);

  परत 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
पूर्णांक Curl_socket_close(संरचना Curl_easy *data, संरचना connectdata *conn,
                      curl_socket_t sock)
{
  परत socket_close(data, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(स्थिरांक अक्षर *आदान,
                              अक्षर **dev, अक्षर **iface, अक्षर **host)
{
  स्थिर स्थिरांक अक्षर if_prefix[] = "if!";
  स्थिर स्थिरांक अक्षर host_prefix[] = "host!";
  स्थिर स्थिरांक अक्षर if_host_prefix[] = "ifhost!";
  माप_प्रकार len;

  len = म_माप(आदान);
  जर(len > 512)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  जर(dev)
    *dev = रिक्त;
  जर(iface)
    *iface = रिक्त;
  जर(host)
    *host = रिक्त;

  जर(!म_भेदन(if_prefix, आदान, म_माप(if_prefix))) {
    आदान += म_माप(if_prefix);
    जर(!*आदान)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    जर(iface) {
      *iface = curlx_memdup0(आदान, len - म_माप(if_prefix));
      परत *iface ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    परत CURLE_OK;
  }
  अन्यथा जर(!म_भेदन(host_prefix, आदान, म_माप(host_prefix))) {
    आदान += म_माप(host_prefix);
    जर(!*आदान)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    जर(host) {
      *host = curlx_memdup0(आदान, len - म_माप(host_prefix));
      परत *host ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    परत CURLE_OK;
  }
  अन्यथा जर(!म_भेदन(if_host_prefix, आदान, म_माप(if_host_prefix))) {
    स्थिरांक अक्षर *host_part;
    आदान += म_माप(if_host_prefix);
    len -= म_माप(if_host_prefix);
    host_part = स_प्रथम(आदान, '!', len);
    जर(!host_part || !*(host_part + 1))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    जर(iface) {
      *iface = curlx_memdup0(आदान, host_part - आदान);
      जर(!*iface)
        परत CURLE_OUT_OF_MEMORY;
    }
    ++host_part;
    जर(host) {
      *host = curlx_memdup0(host_part, len - (host_part - आदान));
      जर(!*host) {
        जर(iface)
          curlx_safefree(*iface);
        परत CURLE_OUT_OF_MEMORY;
      }
    }
    परत CURLE_OK;
  }

  जर(!*आदान)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  जर(dev) {
    *dev = curlx_memdup0(आदान, len);
    परत *dev ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  परत CURLE_OK;
}

#ifndef CURL_DISABLE_BINDLOCAL
स्थिर CURLcode bindlocal(संरचना Curl_easy *data, संरचना connectdata *conn,
                          curl_socket_t sockfd, पूर्णांक af, अचिन्हित पूर्णांक scope,
                          uint8_t transport)
{
  संरचना Curl_sockaddr_storage sa;
  संरचना sockaddr *sock = (संरचना sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  संरचना sockaddr_in *si4 = (संरचना sockaddr_in *)&sa;
#ifdef USE_IPV6
  संरचना sockaddr_in6 *si6 = (संरचना sockaddr_in6 *)&sa;
#endif
  संरचना Curl_dns_entry *h = रिक्त;
  अचिन्हित लघु port = data->set.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  पूर्णांक portnum = data->set.localportrange;
  अक्षर *dev_in = रिक्त, *iface_in = रिक्त, *host_in = रिक्त;
  स्थिरांक अक्षर *iface = रिक्त, *host = रिक्त;
  पूर्णांक sockerr;
#ifdef IP_BIND_ADDRESS_NO_PORT
  पूर्णांक on = 1;
#endif
  CURLcode result = CURLE_OK;

#ifndef USE_IPV6
  (शून्य)scope;
#endif
  जर(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    result = Curl_parse_interface(conn->interface_opts,
                                  &dev_in, &iface_in, &host_in);
    जर(result)
      जा out;
    /* This is how we use the values */
    iface = iface_in ? iface_in : dev_in;
    host = host_in ? host_in : dev_in;
  }

  जर(!iface && !host && !port)
    जा out;

  जर(iface && (म_माप(iface) >= 255)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    जा out;
  }
  स_रखो(&sa, 0, आकार(संरचना Curl_sockaddr_storage));

  जर(iface || host) {
    अक्षर myhost[256] = "";
    पूर्णांक done = 0; /* -1 for error, 1 for address found */
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
    जर(iface &&
       setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                  iface, (curl_socklen_t)म_माप(iface) + 1) == 0 &&
       /* This is often "errno 1, error: Operation not permitted" if you are
        * not running as root or another suitable privileged user. If it
        * succeeds it means the parameter was a valid interface and not an IP
        * address. Return immediately.
        */
       !host_in) {
      infof(data, "socket successfully bound to interface '%s'", iface);
      result = CURLE_OK;
      जा out;
    }
#endif
    जर(!host_in) {
      /* Discover IP from input device, then bind to it */
      if2ip_result = Curl_if2ip(af,
#ifdef USE_IPV6
                                scope, conn->scope_id,
#endif
                                iface, myhost, आकार(myhost));
    }
    निवड(if2ip_result) {
    स्थिती IF2IP_NOT_FOUND:
      जर(iface_in && !host_in) {
        /* Do not fall back to treating it as a hostname */
        अक्षर buffer[STRERROR_LEN];
        data->state.os_errno = sockerr = SOCKERRNO;
        failf(data, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, buffer, आकार(buffer)));
        result = CURLE_INTERFACE_FAILED;
        जा out;
      }
      थांबा;
    स्थिती IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      result = CURLE_UNSUPPORTED_PROTOCOL;
      जा out;
    स्थिती IF2IP_FOUND:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(data, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      done = 1;
      थांबा;
    }
    जर(!iface_in || host_in) {
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
      जर(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#endif

      (शून्य)Curl_resolv_blocking(data, dns_queries, host, 80, transport, &h);
      जर(h) {
        पूर्णांक h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_address(h->addr, myhost, आकार(myhost));
        infof(data, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_entry_unlink(data, &h); /* this will NULL, potential free h */
        जर(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          result = CURLE_UNSUPPORTED_PROTOCOL;
          जा out;
        }
        done = 1;
      }
      अन्यथा {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        done = -1;
      }
    }

    जर(done > 0) {
#ifdef USE_IPV6
      /* IPv6 address */
      जर(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        अक्षर *scope_ptr = म_अक्षर(myhost, '%');
        जर(scope_ptr)
          *(scope_ptr++) = '\0';
#endif
        जर(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          जर(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            जर(curlx_str_संख्या((स्थिरांक अक्षर **)CURL_UNCONST(&scope_ptr),
                                &scope_id, अच_पूर्णांक_उच्च)) {
              result = CURLE_UNSUPPORTED_PROTOCOL;
              जा out;
            }
            si6->sin6_scope_id = (अचिन्हित पूर्णांक)scope_id;
          }
#endif
        }
        sizeof_sa = आकार(संरचना sockaddr_in6);
      }
      अन्यथा
#endif
      /* IPv4 address */
      जर((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(port);
        sizeof_sa = आकार(संरचना sockaddr_in);
      }
    }

    जर(done < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      अक्षर buffer[STRERROR_LEN];
      data->state.errorbuf = FALSE;
      data->state.os_errno = sockerr = SOCKERRNO;
      failf(data, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, buffer, आकार(buffer)));
      result = CURLE_INTERFACE_FAILED;
      जा out;
    }
  }
  अन्यथा {
    /* no device was given, prepare sa to match af's needs */
#ifdef USE_IPV6
    जर(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(port);
      sizeof_sa = आकार(संरचना sockaddr_in6);
    }
    अन्यथा
#endif
    जर(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(port);
      sizeof_sa = आकार(संरचना sockaddr_in);
    }
  }
#ifdef IP_BIND_ADDRESS_NO_PORT
  (शून्य)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, आकार(on));
#endif
  साठी(;;) {
    जर(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(data, "Local port: %hu", port);
      conn->bits.bound = TRUE;
      result = CURLE_OK;
      जा out;
    }

    जर(--portnum > 0) {
      port++; /* try next port */
      जर(port == 0)
        थांबा;
      infof(data, "Bind to local port %d failed, trying next", port - 1);
      /* We reuse/clobber the port variable here below */
      जर(sock->sa_family == AF_INET)
        si4->sin_port = htons(port);
#ifdef USE_IPV6
      अन्यथा
        si6->sin6_port = htons(port);
#endif
    }
    अन्यथा
      थांबा;
  }
  {
    अक्षर buffer[STRERROR_LEN];
    data->state.os_errno = sockerr = SOCKERRNO;
    failf(data, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, आकार(buffer)));
  }

  result = CURLE_INTERFACE_FAILED;

out:
  curlx_free(dev_in);
  curlx_free(iface_in);
  curlx_free(host_in);
  परत result;
}
#endif

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
स्थिर bool verifyconnect(curl_socket_t sockfd, पूर्णांक *psockerr)
{
  bool rc = TRUE;
#ifdef SO_ERROR
  पूर्णांक sockerr = 0;
  curl_socklen_t errSize = आकार(sockerr);

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

  जर(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (शून्य *)&sockerr, &errSize))
    sockerr = SOCKERRNO;
#जर defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  जर(EBADIOCTL == sockerr) {
    SET_SOCKERRNO(0);
    sockerr = 0;
  }
#endif
  जर((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  अन्यथा
    /* This was not a successful connect */
    rc = FALSE;
  जर(psockerr)
    *psockerr = sockerr;
#अन्यथा
  (शून्य)sockfd;
  जर(psockerr)
    *psockerr = SOCKERRNO;
#endif
  परत rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
स्थिर CURLcode socket_connect_result(संरचना Curl_easy *data,
                                      स्थिरांक अक्षर *ipaddress, पूर्णांक sockerr)
{
  जर(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    परत CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(अक्षर buffer[STRERROR_LEN]);
    infof(data, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, buffer, आकार(buffer)));
    NOVERBOSE((शून्य)ipaddress);
  }
  data->state.os_errno = sockerr;
  /* connect failed */
  परत CURLE_COULDNT_CONNECT;
}

संरचना cf_socket_ctx {
  संरचना Curl_peer *peer;
  संरचना Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  संरचना ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  संरचना curltime started_at;        /* when socket was created */
  संरचना curltime connected_at;      /* when socket connected/got first byte */
  संरचना curltime first_byte_at;     /* when first byte was recvd */
#ifdef USE_WINSOCK
  संरचना curltime last_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_size;                     /* the last set SO_SNDBUF size */
#endif
  पूर्णांक sockerr;                       /* socket error of last failure or 0 */
#ifdef DEBUGBUILD
  पूर्णांक connect_wait_ms;               /* ms to wait after 1st connect() */
  पूर्णांक wblock_percent;                /* percent of writes doing EAGAIN */
  पूर्णांक wpartial_percent;              /* percent of bytes written in send */
  पूर्णांक rblock_percent;                /* percent of reads doing EAGAIN */
  माप_प्रकार recv_max;                   /* max enforced read size */
#endif
  uint8_t transport;
  BIT(got_first_byte);               /* if first byte was received */
  BIT(listening);                    /* socket is listening */
  BIT(accepted);                     /* socket was accepted, not connected */
  BIT(sock_connected);               /* socket is "connected", e.g. in UDP */
  BIT(active);
  BIT(stats_reported);
};

स्थिर CURLcode cf_socket_ctx_init(संरचना cf_socket_ctx *ctx,
                                   संरचना Curl_peer *peer,
                                   संरचना Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  स_रखो(ctx, 0, आकार(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ifdef DEBUGBUILD
  {
    स्थिरांक अक्षर *p = दो_पर्या("CURL_DBG_SOCK_WBLOCK");
    जर(p) {
      curl_off_t l;
      जर(!curlx_str_संख्या(&p, &l, 100))
        ctx->wblock_percent = (पूर्णांक)l;
    }
    p = दो_पर्या("CURL_DBG_SOCK_WPARTIAL");
    जर(p) {
      curl_off_t l;
      जर(!curlx_str_संख्या(&p, &l, 100))
        ctx->wpartial_percent = (पूर्णांक)l;
    }
    p = दो_पर्या("CURL_DBG_SOCK_RBLOCK");
    जर(p) {
      curl_off_t l;
      जर(!curlx_str_संख्या(&p, &l, 100))
        ctx->rblock_percent = (पूर्णांक)l;
    }
    p = दो_पर्या("CURL_DBG_SOCK_RMAX");
    जर(p) {
      curl_off_t l;
      जर(!curlx_str_संख्या(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (माप_प्रकार)l;
    }
    p = दो_पर्या("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    जर(p) {
      curl_off_t l;
      जर(!curlx_str_संख्या(&p, &l, पूर्णांक_उच्च))
        ctx->connect_wait_ms = (पूर्णांक)l;
    }
  }
#endif

  परत CURLE_OK;
}

स्थिर शून्य cf_socket_ctx_free(संरचना cf_socket_ctx *ctx)
{
  जर(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

स्थिर CURLcode cf_socket_shutdown(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   bool *done)
{
  जर(cf->connected) {
    संरचना cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(data, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    जर(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      अचिन्हित अक्षर buf[1024];
      (शून्य)sread(ctx->sock, buf, आकार(buf));
    }
  }
  *done = TRUE;
  परत CURLE_OK;
}

स्थिर शून्य cf_socket_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  जर(ctx) {
    जर(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(data, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      जर(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(data, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

स्थिर शून्य set_local_ip(संरचना Curl_cfilter *cf,
                         संरचना Curl_easy *data)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_port = 0;

#ifdef HAVE_GETSOCKNAME
  जर((ctx->sock != CURL_SOCKET_BAD) &&
     !(data->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    संरचना Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = आकार(संरचना Curl_sockaddr_storage);

    स_रखो(&ssloc, 0, आकार(ssloc));
    जर(getsockname(ctx->sock, (संरचना sockaddr *)&ssloc, &slen)) {
      VERBOSE(अक्षर buffer[STRERROR_LEN]);
      VERBOSE(पूर्णांक sockerr = SOCKERRNO);
      infof(data, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, buffer, आकार(buffer)));
    }
    अन्यथा {
      CURLcode result = sockaddr2string((संरचना sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_port);
      जर(result)
        infof(data, "ssloc inet_ntop() failed with %d", (पूर्णांक)result);
    }
  }
#अन्यथा
  (शून्य)data;
#endif
}

स्थिर CURLcode set_remote_ip(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  CURLcode result;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  result = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_port);
  जर(result) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(data, "curl_sa_addr inet_ntop() failed with %d", (पूर्णांक)result);
    परत CURLE_FAILED_INIT;
  }
  परत CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
स्थिर पूर्णांक cf_socktype(पूर्णांक x)
{
#ifdef SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#endif
  परत x;
}

#ifdef _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ifndef SIO_TCP_INITIAL_RTO
#define SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#define TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकारव्याख्या संरचना _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#endif /* SIO_TCP_INITIAL_RTO */

#ifndef TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#define TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#endif

स्थिर bool targets_localhost(संरचना cf_socket_ctx *ctx)
{
  परत (((ctx->addr.family == AF_INET) &&
           !म_भेद(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !म_भेद(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
स्थिर शून्य tcplocalhost(संरचना Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  जर(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD bytes = 0;
    स_रखो(&rto, 0, आकार(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    जर(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    अन्यथा
      rto.MaxSynRetransmissions = 1;
    (शून्य)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, आकार(rto),
                   रिक्त, 0, &bytes, रिक्त, रिक्त);
  }
}
#अन्यथा
#define tcplocalhost(x, y)
#endif /* _WIN32 */

स्थिर CURLcode cf_socket_open(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  पूर्णांक error = 0;
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
  जर(!data->set.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#endif
  result = socket_open(data, &ctx->addr, &ctx->sock);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  जर(!data->set.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#endif
  जर(result)
    जा out;

  result = set_remote_ip(cf, data);
  जर(result)
    जा out;

#ifdef USE_IPV6
  जर(ctx->addr.family == AF_INET6) {
#ifdef USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    पूर्णांक on = 0;
    (शून्य)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (शून्य *)&on, आकार(on));
#endif
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      संरचना sockaddr_in6 *sa6 = (शून्य *)&ctx->addr.curl_sa_addr;
      जर(sa6->sin6_scope_id)
        infof(data, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_port,
              (अचिन्हित दीर्घ)sa6->sin6_scope_id);
      अन्यथा
#endif
        infof(data, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#endif
  }
  अन्यथा
#endif
    infof(data, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_port);

#ifdef USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#अन्यथा
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#endif
  जर(is_tcp) {
    जर(data->set.tcp_nodelay)
      tcpnodelay(cf, data, ctx->sock);

    जर(data->set.tcp_keepalive)
      tcpkeepalive(cf, data, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  जर(data->set.fsockopt) {
    /* activate callback for setting socket options */
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_END(&guard);

    जर(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    अन्यथा जर(error) {
      result = CURLE_ABORTED_BY_CALLBACK;
      जा out;
    }
  }

#ifndef CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  जर(ctx->addr.family == AF_INET
#ifdef USE_IPV6
     || ctx->addr.family == AF_INET6
#endif
    ) {
    result = bindlocal(data, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    जर(result) {
      जर(result == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        result = CURLE_COULDNT_CONNECT;
      }
      जा out;
    }
  }
#endif

#ifndef CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  जर(error < 0) {
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    जा out;
  }
#अन्यथा
  जर(data->set.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    जर(error < 0) {
      result = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      जा out;
    }
  }
#endif
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
out:
  जर(result) {
    जर(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  अन्यथा जर(isconnected) {
    set_local_ip(cf, data);
    ctx->connected_at = *Curl_pgrs_now(data);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(data, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (पूर्णांक)result, ctx->sock);
  परत result;
}

स्थिर पूर्णांक do_connect(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                      bool is_tcp_fastopen)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
#ifdef TCP_FASTOPEN_CONNECT
  पूर्णांक optval = 1;
#endif
  पूर्णांक rc = -1;

  (शून्य)data;
  जर(is_tcp_fastopen) {
#ifdef CONNECT_DATA_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_AVAILABLE
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    जर(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = रिक्त;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_DATA_IDEMPOTENT,
                    रिक्त, 0, रिक्त, रिक्त);
    }
    अन्यथा {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  अन्यथा
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#किंवाजर defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    जर(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (शून्य *)&optval, आकार(optval)) < 0)
      CURL_TRC_CF(data, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#किंवाजर defined(MSG_FASTOPEN) /* old Linux */
    जर(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    अन्यथा
      rc = 0; /* Do nothing */
#endif
  }
  अन्यथा {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  परत rc;
}

स्थिर CURLcode cf_tcp_connect(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               bool *done)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;
  पूर्णांक rc = 0;

  जर(cf->connected) {
    *done = TRUE;
    परत CURLE_OK;
  }

  *done = FALSE; /* a negative world view is best */
  जर(ctx->sock == CURL_SOCKET_BAD) {
    पूर्णांक sockerr;

    result = cf_socket_open(cf, data);
    जर(result)
      जा out;

    जर(cf->connected) {
      *done = TRUE;
      परत CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, data, (bool)cf->conn->bits.tcp_fastopen);
    sockerr = SOCKERRNO;
    set_local_ip(cf, data);
    CURL_TRC_CF(data, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_port);
#ifdef DEBUGBUILD
    जर(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(data, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#endif
    जर(rc == -1) {
      ctx->sockerr = sockerr;
      result = socket_connect_result(data, ctx->ip.remote_ip, sockerr);
      जा out;
    }
  }

#ifdef mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (शून्य)verifyconnect(ctx->sock, रिक्त);
#endif
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  जर(rc == 0) { /* no connection yet */
    CURL_TRC_CF(data, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    परत CURLE_OK;
  }
  अन्यथा जर(rc == CURL_CSELECT_OUT ||
#ifdef __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#endif
          cf->conn->bits.tcp_fastopen) {
    जर(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(data);
      set_local_ip(cf, data);
      *done = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(data, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      परत CURLE_OK;
    }
    result = CURLE_COULDNT_CONNECT;
  }
  अन्यथा जर(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(data, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (शून्य)verifyconnect(ctx->sock, &ctx->sockerr);
    result = CURLE_COULDNT_CONNECT;
  }

out:
  जर(result) {
    VERBOSE(अक्षर buffer[STRERROR_LEN]);
    set_local_ip(cf, data);
    जर(ctx->sockerr) {
      data->state.os_errno = ctx->sockerr;
      SET_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, buffer, आकार(buffer)));
    }
    अन्यथा {
      VERBOSE(curlx_strcopy(buffer, आकार(buffer), STRCONST("peer closed")));
    }
    जर(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(data, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_port,
          ctx->ip.local_ip, ctx->ip.local_port,
          curlx_strerror(ctx->sockerr, buffer, आकार(buffer)));
    *done = FALSE;
  }
  परत result;
}

स्थिर CURLcode cf_socket_adjust_pollset(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data,
                                         संरचना easy_pollset *ps)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  जर(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    जर(ctx->listening) {
      result = Curl_pollset_set_in_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    अन्यथा जर(!cf->connected) {
      result = Curl_pollset_set_out_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    अन्यथा जर(!ctx->active) {
      result = Curl_pollset_add_in(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  परत result;
}

#ifdef USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ifndef SIO_IDEAL_SEND_BACKLOG_QUERY
#define SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#endif

स्थिर शून्य win_update_sndbuf_size(संरचना Curl_easy *data,
                                   संरचना cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  जर(curlx_ptimediff_ms(Curl_pgrs_now(data),
                        &ctx->last_sndbuf_query_at) > 1000) {
    जर(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, आकार(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_size &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (स्थिरांक अक्षर *)&ideal, आकार(ideal))) {
      ctx->sndbuf_size = ideal;
    }
    ctx->last_sndbuf_query_at = *Curl_pgrs_now(data);
  }
}

#endif /* USE_WINSOCK */

स्थिर CURLcode cf_socket_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                               स्थिरांक uint8_t *buf, माप_प्रकार len, bool eos,
                               माप_प्रकार *pnwritten)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode result = CURLE_OK;
  VERBOSE(माप_प्रकार orig_len = len);

  (शून्य)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  जर(ctx->wblock_percent > 0) {
    अचिन्हित अक्षर c = 0;
    Curl_rand_bytes(data, FALSE, &c, 1);
    जर(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      परत CURLE_AGAIN;
    }
  }
  जर(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    जर(!len)
      len = 1;
    CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#endif

#जर defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  जर(cf->conn->bits.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->bits.tcp_fastopen = FALSE;
  }
  अन्यथा
#endif
    rv = swrite(ctx->sock, buf, len);

  जर(!curlx_sztouz(rv, pnwritten)) {
    पूर्णांक sockerr = SOCKERRNO;
    जर(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    अन्यथा {
      अक्षर buffer[STRERROR_LEN];
      failf(data, "Send failure: %s",
            curlx_strerror(sockerr, buffer, आकार(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_SEND_ERROR;
    }
  }

#ifdef USE_WINSOCK
  जर(!result)
    win_update_sndbuf_size(data, ctx);
#endif

  CURL_TRC_CF(data, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (पूर्णांक)result, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  परत result;
}

स्थिर CURLcode cf_socket_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                               अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial reads */
  जर(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    अचिन्हित अक्षर c = 0;
    Curl_rand(data, &c, 1);
    जर(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      परत CURLE_AGAIN;
    }
  }
  जर(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#endif

  rv = sread(ctx->sock, buf, len);

  जर(!curlx_sztouz(rv, pnread)) {
    पूर्णांक sockerr = SOCKERRNO;
    जर(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    अन्यथा {
      अक्षर buffer[STRERROR_LEN];
      failf(data, "Recv failure: %s",
            curlx_strerror(sockerr, buffer, आकार(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(data, cf, "recv(len=%zu) -> %d, %zu", len, (पूर्णांक)result, *pnread);
  जर(!result && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(data);
    ctx->got_first_byte = TRUE;
  }
  परत result;
}

स्थिर शून्य cf_socket_update_data(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  /* Update the IP info held in the transfer, if we have that. */
  जर(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    संरचना cf_socket_ctx *ctx = cf->ctx;
    data->info.primary = ctx->ip;
  }
}

स्थिर शून्य cf_socket_active(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  set_local_ip(cf, data);
#ifdef USE_IPV6
  जर(cf->sockindex == FIRSTSOCKET)
    cf->conn->bits.ipv6 = (ctx->addr.family == AF_INET6);
#endif
  ctx->active = TRUE;
}

स्थिर CURLcode cf_socket_cntrl(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                पूर्णांक event, पूर्णांक arg1, शून्य *arg2)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;

  (शून्य)arg1;
  (शून्य)arg2;
  निवड(event) {
  स्थिती CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_active(cf, data);
    cf_socket_update_data(cf, data);
    थांबा;
  स्थिती CF_CTRL_DATA_SETUP:
    cf_socket_update_data(cf, data);
    थांबा;
  स्थिती CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    थांबा;
  स्थिती CF_CTRL_REPORT_STATS:
    जर(cf->connected && !ctx->stats_reported) {
      संरचना curltime *ts = रिक्त;
      निवड(ctx->transport) {
      स्थिती TRNSPRT_UDP:
      स्थिती TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        जर(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        थांबा;
      पूर्वनिर्धारित:
        ts = &ctx->connected_at;
        थांबा;
      }
      जर(ts) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  परत CURLE_OK;
}

स्थिर bool cf_socket_conn_is_alive(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    bool *आदान_pending)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  संरचना pollfd pfd[1];
  पूर्णांक r;

  *आदान_pending = FALSE;

  जर(!ctx || ctx->sock == CURL_SOCKET_BAD)
    परत FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  जर(r < 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll error, assume dead");
    परत FALSE;
  }
  अन्यथा जर(r == 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll timeout, assume alive");
    परत TRUE;
  }
  अन्यथा जर(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(data, cf, "is_alive: err/hup/etc events, assume dead");
    परत FALSE;
  }

  CURL_TRC_CF(data, cf, "is_alive: valid events, looks alive");
  *आदान_pending = TRUE;
  परत TRUE;
}

स्थिर CURLcode cf_socket_query(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                पूर्णांक query, पूर्णांक *pres1, शून्य *pres2)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;

  निवड(query) {
  स्थिती CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    परत CURLE_OK;
  स्थिती CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    परत CURLE_OK;
  स्थिती CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((स्थिरांक संरचना Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : रिक्त;
    परत CURLE_OK;
  स्थिती CF_QUERY_CONNECT_REPLY_MS:
    जर(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < पूर्णांक_उच्च) ? (पूर्णांक)ms : पूर्णांक_उच्च;
    }
    अन्यथा
      *pres1 = -1;
    परत CURLE_OK;
  स्थिती CF_QUERY_IP_INFO:
#ifdef USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#अन्यथा
    *pres1 = FALSE;
#endif
    *(संरचना ip_quadruple *)pres2 = ctx->ip;
    परत CURLE_OK;
  स्थिती CF_QUERY_REALLY_CONNECTED:
    जर(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    अन्यथा
      *pres1 = ctx->got_first_byte;
    परत CURLE_OK;
  पूर्वनिर्धारित:
    थांबा;
  }
  परत cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

संरचना Curl_cftype Curl_cft_tcp = {
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

CURLcode Curl_cf_tcp_create(संरचना Curl_cfilter **pcf,
                            संरचना Curl_easy *data,
                            संरचना Curl_peer *origin,
                            संरचना Curl_peer *peer,
                            uint8_t transport_peer,
                            संरचना connectdata *conn,
                            संरचना Curl_sockaddr_ex *addr,
                            संरचना Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  संरचना cf_socket_ctx *ctx = रिक्त;
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode result;

  (शून्य)data;
  (शून्य)origin;
  (शून्य)conn;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  जर(!addr) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    जा out;
  }

  ctx = curlx_calloc(1, आकार(*ctx));
  जर(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  जर(result)
    जा out;

  result = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

out:
  *pcf = (!result) ? cf : रिक्त;
  जर(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  परत result;
}

#ifdef __linux__
स्थिर शून्य linux_quic_mtu(संरचना cf_socket_ctx *ctx)
{
  पूर्णांक val;
  निवड(ctx->addr.family) {
#ifdef IP_MTU_DISCOVER
  स्थिती AF_INET:
    val = IP_PMTUDISC_DO;
    (शून्य)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     आकार(val));
    थांबा;
#endif
#ifdef IPV6_MTU_DISCOVER
  स्थिती AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (शून्य)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     आकार(val));
    थांबा;
#endif
  }
}
#अन्यथा
#define linux_quic_mtu(x)
#endif

#जर defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
स्थिर शून्य linux_quic_gro(संरचना cf_socket_ctx *ctx)
{
  पूर्णांक one = 1;
  (शून्य)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)आकार(one));
}
#अन्यथा
#define linux_quic_gro(x)
#endif

#जर (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
स्थिर शून्य linux_quic_ecn(संरचना cf_socket_ctx *ctx)
{
  अचिन्हित पूर्णांक tos = 1;
  निवड(ctx->addr.family) {
  स्थिती AF_INET:
    (शून्य)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, आकार(tos));
    थांबा;
#ifdef IPV6_RECVTCLASS
  स्थिती AF_INET6:
    (शून्य)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, आकार(tos));
    थांबा;
#endif
  }
}
#अन्यथा
#define linux_quic_ecn(x)
#endif

#जर (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
स्थिर शून्य linux_ip_dontfrag(संरचना cf_socket_ctx *ctx)
{
  पूर्णांक val = 1;

  निवड(ctx->addr.family) {
  स्थिती AF_INET:
    (शून्य)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, आकार(val));
    थांबा;
#ifdef IPV6_DONTFRAG
  स्थिती AF_INET6:
    (शून्य)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, आकार(val));
    थांबा;
#endif
  }
}
#अन्यथा
#define linux_ip_dontfrag(x)
#endif

स्थिर CURLcode cf_udp_setup_quic(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  पूर्णांक rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  जर(rc == -1) {
    परत socket_connect_result(data, ctx->ip.remote_ip, SOCKERRNO);
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

  परत CURLE_OK;
}

स्थिर CURLcode cf_udp_connect(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               bool *done)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;

  जर(cf->connected) {
    *done = TRUE;
    परत CURLE_OK;
  }

  *done = FALSE;
  जर(ctx->sock == CURL_SOCKET_BAD) {
    result = cf_socket_open(cf, data);
    जर(result) {
      CURL_TRC_CF(data, cf, "cf_udp_connect(), open failed -> %d",
                  (पूर्णांक)result);
      जा out;
    }

    जर(ctx->transport == TRNSPRT_QUIC) {
      result = cf_udp_setup_quic(cf, data);
      जर(result)
        जा out;
      CURL_TRC_CF(data, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
    }
    *done = TRUE;
    cf->connected = TRUE;
  }
out:
  परत result;
}

संरचना Curl_cftype Curl_cft_udp = {
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

CURLcode Curl_cf_udp_create(संरचना Curl_cfilter **pcf,
                            संरचना Curl_easy *data,
                            संरचना Curl_peer *origin,
                            संरचना Curl_peer *peer,
                            uint8_t transport_peer,
                            संरचना connectdata *conn,
                            संरचना Curl_sockaddr_ex *addr,
                            संरचना Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  संरचना cf_socket_ctx *ctx = रिक्त;
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode result;

  (शून्य)data;
  (शून्य)origin;
  (शून्य)conn;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, आकार(*ctx));
  जर(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  जर(result)
    जा out;

  result = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

out:
  *pcf = (!result) ? cf : रिक्त;
  जर(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  परत result;
}

/* this is the TCP filter which can also handle this case */
संरचना Curl_cftype Curl_cft_unix = {
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

CURLcode Curl_cf_unix_create(संरचना Curl_cfilter **pcf,
                            संरचना Curl_easy *data,
                            संरचना Curl_peer *origin,
                            संरचना Curl_peer *peer,
                            uint8_t transport_peer,
                            संरचना connectdata *conn,
                            संरचना Curl_sockaddr_ex *addr,
                            संरचना Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  संरचना cf_socket_ctx *ctx = रिक्त;
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode result;

  (शून्य)data;
  (शून्य)origin;
  (शून्य)conn;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, आकार(*ctx));
  जर(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  जर(result)
    जा out;

  result = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

out:
  *pcf = (!result) ? cf : रिक्त;
  जर(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  परत result;
}

स्थिर timediff_t cf_tcp_accept_timeleft(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ifndef CURL_DISABLE_FTP
  जर(data->set.accepttimeout > 0)
    timeout_ms = data->set.accepttimeout;
#endif

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(data);
  जर(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  अन्यथा {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->started_at);
    जर(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  परत timeout_ms;
}

स्थिर शून्य cf_tcp_set_accepted_remote_ip(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
#ifdef HAVE_GETPEERNAME
  CURLcode result;
  संरचना Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  plen = आकार(ssrem);
  स_रखो(&ssrem, 0, plen);
  जर(getpeername(ctx->sock, (संरचना sockaddr *)&ssrem, &plen)) {
    अक्षर buffer[STRERROR_LEN];
    पूर्णांक sockerr = SOCKERRNO;
    failf(data, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, आकार(buffer)));
    परत;
  }
  result = sockaddr2string((संरचना sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_port);
  जर(result) {
    failf(data, "ssrem inet_ntop() failed with %d", (पूर्णांक)result);
    परत;
  }
#अन्यथा
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  (शून्य)data;
#endif
}

स्थिर CURLcode cf_tcp_accept_connect(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      bool *done)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  अक्षर errbuf[STRERROR_LEN];
#ifdef USE_IPV6
  संरचना Curl_sockaddr_storage add;
#अन्यथा
  संरचना sockaddr_in add;
#endif
  curl_socklen_t size = (curl_socklen_t)आकार(add);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  पूर्णांक socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (शून्य)data;
  जर(cf->connected) {
    *done = TRUE;
    परत CURLE_OK;
  }

  *done = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, data);
  जर(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(data, "Accept timeout occurred while waiting server connect");
    परत CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(data, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(data, cf, "socket_check -> %x", (अचिन्हित पूर्णांक)socketstate);
  निवड(socketstate) {
  स्थिती -1: /* error */
    /* let's die here */
    failf(data, "Error while waiting for server connect");
    परत CURLE_FTP_ACCEPT_FAILED;
  पूर्वनिर्धारित:
    जर(socketstate & CURL_CSELECT_IN) {
      infof(data, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    थांबा;
  }

  जर(!incoming) {
    CURL_TRC_CF(data, cf, "nothing heard from the server yet");
    परत CURLE_OK;
  }

  size = आकार(add);
#ifdef HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (संरचना sockaddr *)&add, &size,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#अन्यथा
  s_accepted = CURL_ACCEPT(ctx->sock, (संरचना sockaddr *)&add, &size);
#endif

  जर(s_accepted == CURL_SOCKET_BAD) {
    failf(data, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, आकार(errbuf)));
    परत CURLE_FTP_ACCEPT_FAILED;
  }
#ifndef HAVE_ACCEPT4
#ifdef HAVE_FCNTL
  जर(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, आकार(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    परत CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* HAVE_FCNTL */
  जर(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(data, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, आकार(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    परत CURLE_FTP_ACCEPT_FAILED;
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

  जर(data->set.fsockopt) {
    संरचना Curl_mapi_guard guard;
    पूर्णांक error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_END(&guard);

    जर(error)
      परत CURLE_ABORTED_BY_CALLBACK;
  }
  *done = TRUE;
  परत CURLE_OK;
}

संरचना Curl_cftype Curl_cft_tcp_accept = {
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

CURLcode Curl_conn_tcp_listen_set(संरचना Curl_easy *data,
                                  संरचना connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode result;
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_socket_ctx *ctx = रिक्त;

  /* replace any existing */
  Curl_conn_cf_discard_all(data, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, आकार(*ctx));
  जर(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  result = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  जर(result)
    जा out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(data);
  conn->sock[sockindex] = ctx->sock;
  set_local_ip(cf, data);
  CURL_TRC_CF(data, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_port);

out:
  जर(result) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  परत result;
}

bool Curl_conn_is_tcp_listen(संरचना Curl_easy *data,
                             int8_t sockindex)
{
  संरचना Curl_cfilter *cf = data->conn->cfilter[sockindex];
  पर्यंत(cf) {
    जर(cf->cft == &Curl_cft_tcp_accept)
      परत TRUE;
    cf = cf->next;
  }
  परत FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
स्थिर bool cf_is_socket(संरचना Curl_cfilter *cf)
{
  परत cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             curl_socket_t *psock,
                             स्थिरांक संरचना Curl_sockaddr_ex **paddr,
                             संरचना ip_quadruple *pip)
{
  (शून्य)data;
  जर(cf_is_socket(cf) && cf->ctx) {
    संरचना cf_socket_ctx *ctx = cf->ctx;

    जर(psock)
      *psock = ctx->sock;
    जर(paddr)
      *paddr = &ctx->addr;
    जर(pip)
      *pip = ctx->ip;
    परत CURLE_OK;
  }
  परत CURLE_FAILED_INIT;
}
