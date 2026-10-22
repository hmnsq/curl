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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h> /* <netinet/tcp.h> may need it */
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_UN_H
#समावेश <sys/un.h> /* for sockaddr_un */
#पूर्ण_अगर
#अगर_घोषित HAVE_LINUX_TCP_H
#समावेश <linux/tcp.h>
#या_अगर defined(HAVE_NETINET_TCP_H)
#समावेश <netinet/tcp.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETINET_UDP_H
#समावेश <netinet/udp.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETINET_IP_H
#समावेश <netinet/ip.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_IFADDRS_H
#समावेश <ifaddrs.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NET_IF_H
#समावेश <net/if.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "if2ip.h"
#समावेश "cfilters.h"
#समावेश "cf-socket.h"
#समावेश "connect.h"
#समावेश "curl_addrinfo.h"
#समावेश "select.h"
#समावेश "multiif.h"
#समावेश "curlx/inet_ntop.h"
#समावेश "curlx/inet_pton.h"
#समावेश "progress.h"
#समावेश "conncache.h"
#समावेश "multihandle.h"
#समावेश "rand.h"
#समावेश "sockaddr.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/nonblock.h"
#समावेश "curlx/strcopy.h"
#समावेश "curlx/version_win32.h"
#समावेश "curlx/strerr.h"
#समावेश "curlx/strparse.h"
#समावेश "curlx/wait.h"
#अगर_घोषित _WIN32
#समावेश <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#पूर्ण_अगर

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(काष्ठा sockaddr *sa, curl_socklen_t salen,
                                  अक्षर *addr, uint16_t *port);
UNITTEST CURLcode sockaddr2string(काष्ठा sockaddr *sa, curl_socklen_t salen,
                                  अक्षर *addr, uint16_t *port)
{
  CURLcode परिणाम;
  काष्ठा sockaddr_in *si = शून्य;
#अगर_घोषित USE_IPV6
  काष्ठा sockaddr_in6 *si6 = शून्य;
#पूर्ण_अगर
#अगर_घोषित USE_UNIX_SOCKETS
  काष्ठा sockaddr_un *su = शून्य;
#अन्यथा
  (व्योम)salen;
#पूर्ण_अगर

  चयन(sa->sa_family) {
  हाल AF_INET:
    si = (काष्ठा sockaddr_in *)(व्योम *)sa;
    परिणाम = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    अगर(!परिणाम) {
      *port = ntohs(si->sin_port);
      वापस परिणाम;
    }
    अवरोध;
#अगर_घोषित USE_IPV6
  हाल AF_INET6:
    si6 = (काष्ठा sockaddr_in6 *)(व्योम *)sa;
    परिणाम = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    अगर(!परिणाम) {
      *port = ntohs(si6->sin6_port);
      वापस परिणाम;
    }
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित USE_UNIX_SOCKETS
  हाल AF_UNIX:
    अगर(salen > (curl_socklen_t)माप(CURL_SA_FAMILY_T)) {
      su = (काष्ठा sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_path);
    }
    अन्यथा
      addr[0] = 0; /* socket with no name */
    *port = 0;
    वापस CURLE_OK;
#पूर्ण_अगर
  शेष:
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
    अवरोध;
  }

  addr[0] = '\0';
  *port = 0;
  वापस परिणाम;
}

अटल व्योम tcpnodelay(काष्ठा Curl_cfilter *cf,
                       काष्ठा Curl_easy *तथ्य,
                       curl_socket_t sockfd)
{
#अगर defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  पूर्णांक level = IPPROTO_TCP;
  VERBOSE(अक्षर बफर[STRERROR_LEN]);

  अगर(setsockopt(sockfd, level, TCP_NODELAY, (व्योम *)&onoff, माप(onoff)) < 0)
    CURL_TRC_CF(तथ्य, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, बफर, माप(बफर)));
#अन्यथा
  (व्योम)cf;
  (व्योम)तथ्य;
  (व्योम)sockfd;
#पूर्ण_अगर
}

#अगर defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_THRESHOLD) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
#अगर defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
अटल व्योम alive_unit(पूर्णांक *x)
{
  /* make sure this doesn't wrap */
  अगर(*x < (पूर्णांक_उच्च / 1000))
    *x *= 1000;
  अन्यथा
    /* this is still almost 25 days */
    *x = पूर्णांक_उच्च;
}
#अन्यथा
#घोषणा alive_unit(x)
#पूर्ण_अगर
#पूर्ण_अगर

अटल व्योम tcpkeepalive(काष्ठा Curl_cfilter *cf,
                         काष्ठा Curl_easy *तथ्य,
                         curl_socket_t sockfd)
{
  पूर्णांक optval = तथ्य->set.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  अगर(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (व्योम *)&optval, माप(optval)) < 0) {
    CURL_TRC_CF(तथ्य, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  अन्यथा {
#अगर_घोषित USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    अगर(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      CURL_TRC_CF(तथ्य, cf, "Set TCP_KEEP* on fd=%" FMT_SOCKET_T, sockfd);
      optval = curlx_sltosi(तथ्य->set.tcp_keepidle);
/* Offered by mingw-w64 v12+, MS SDK 6.0A/VS2008+ */
#अगर_अघोषित TCP_KEEPALIVE
#घोषणा TCP_KEEPALIVE 3
#पूर्ण_अगर
/* Offered by mingw-w64 v12+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#अगर_अघोषित TCP_KEEPCNT
#घोषणा TCP_KEEPCNT 16
#पूर्ण_अगर
/* Offered by mingw-w64 v12+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#अगर_अघोषित TCP_KEEPIDLE
#घोषणा TCP_KEEPIDLE TCP_KEEPALIVE
#पूर्ण_अगर
#अगर_अघोषित TCP_KEEPINTVL
#घोषणा TCP_KEEPINTVL 17
#पूर्ण_अगर
      अगर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (स्थिर अक्षर *)&optval, माप(optval)) < 0) {
        CURL_TRC_CF(तथ्य, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(तथ्य->set.tcp_keepintvl);
      अगर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (स्थिर अक्षर *)&optval, माप(optval)) < 0) {
        CURL_TRC_CF(तथ्य, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(तथ्य->set.tcp_keepcnt);
      अगर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (स्थिर अक्षर *)&optval, माप(optval)) < 0) {
        CURL_TRC_CF(तथ्य, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    अन्यथा {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#अगर_अघोषित SIO_KEEPALIVE_VALS
#घोषणा SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      काष्ठा tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#पूर्ण_अगर
      काष्ठा tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(तथ्य->set.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(तथ्य->set.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      अगर(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, माप(vals),
                  शून्य, 0, &dummy, शून्य, शून्य) != 0) {
        CURL_TRC_CF(तथ्य, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#अन्यथा /* !USE_WINSOCK */
#अगर_घोषित TCP_KEEPIDLE
    optval = curlx_sltosi(तथ्य->set.tcp_keepidle);
    alive_unit(&optval);
    अगर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (व्योम *)&optval, माप(optval)) < 0) {
      CURL_TRC_CF(तथ्य, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#या_अगर defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(तथ्य->set.tcp_keepidle);
    alive_unit(&optval);
    अगर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (व्योम *)&optval, माप(optval)) < 0) {
      CURL_TRC_CF(तथ्य, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#या_अगर defined(TCP_KEEPALIVE_THRESHOLD)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(तथ्य->set.tcp_keepidle);
    alive_unit(&optval);
    अगर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_THRESHOLD,
                  (व्योम *)&optval, माप(optval)) < 0) {
      CURL_TRC_CF(तथ्य, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#पूर्ण_अगर
#अगर_घोषित TCP_KEEPINTVL
    optval = curlx_sltosi(तथ्य->set.tcp_keepintvl);
    alive_unit(&optval);
    अगर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (व्योम *)&optval, माप(optval)) < 0) {
      CURL_TRC_CF(तथ्य, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#या_अगर defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
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
      पूर्णांक keepcnt = curlx_sltosi(तथ्य->set.tcp_keepcnt);
      पूर्णांक keepintvl = curlx_sltosi(तथ्य->set.tcp_keepintvl);

      अगर(keepcnt > 0 && keepintvl > (पूर्णांक_उच्च / keepcnt))
        optval = पूर्णांक_उच्च;
      अन्यथा
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    अगर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_THRESHOLD,
                  (व्योम *)&optval, माप(optval)) < 0) {
      CURL_TRC_CF(तथ्य, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#पूर्ण_अगर
#अगर_घोषित TCP_KEEPCNT
    optval = curlx_sltosi(तथ्य->set.tcp_keepcnt);
    अगर(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (व्योम *)&optval, माप(optval)) < 0) {
      CURL_TRC_CF(तथ्य, cf, "Failed to set TCP_KEEPCNT on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#पूर्ण_अगर
#पूर्ण_अगर /* USE_WINSOCK */
  }
}

/**
 * Assign the addrinfo `ai` to the Curl_sockaddr_ex `addr` with
 * transport determining socktype and protocol.
 */
CURLcode Curl_socket_addr_from_ai(काष्ठा Curl_sockaddr_ex *addr,
                                  स्थिर काष्ठा Curl_addrinfo *ai,
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

  DEBUGASSERT(addr->addrlen <= माप(addr->curl_sa_addrbuf));
  अगर(addr->addrlen > माप(addr->curl_sa_addrbuf))
    वापस CURLE_TOO_LARGE;

  स_नकल(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  वापस CURLE_OK;
}

#अगर_घोषित USE_SO_NOSIGPIPE
पूर्णांक Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  पूर्णांक onoff = 1;
  वापस setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (व्योम *)&onoff, माप(onoff));
}
#पूर्ण_अगर /* USE_SO_NOSIGPIPE */

#अगर defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
अटल uint32_t get_scope_id(काष्ठा Curl_easy *तथ्य,
                             काष्ठा sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  अगर(तथ्य->conn->scope_id)
    वापस तथ्य->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  अगर(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#अगर defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    काष्ठा ifaddrs *ifa, *ifa_list;
    अगर(getifaddrs(&ifa_list) == 0) {
      क्रम(ifa = ifa_list; ifa; ifa = ifa->ifa_अग्र) {
        अगर(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          काष्ठा sockaddr_in6 *s6 = (व्योम *)ifa->ifa_addr;
          अगर(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(तथ्य,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (अचिन्हित दीर्घ)scope_id);
            अवरोध;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#पूर्ण_अगर /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  वापस scope_id;
}
#पूर्ण_अगर

अटल CURLcode socket_open(काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  अक्षर errbuf[STRERROR_LEN];

#अगर_घोषित SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#पूर्ण_अगर

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);
  अगर(तथ्य->set.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    काष्ठा Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, तथ्य, easy_fopensocket);
    *sockfd = तथ्य->set.fopensocket(तथ्य->set.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (काष्ठा curl_sockaddr *)addr);
    CURL_CBAPI_END(&guard);
  }
  अन्यथा {
    /* opensocket callback not set, so create the socket now */
#अगर_घोषित DEBUGBUILD
    अगर((addr->family == AF_INET6) && दो_पर्या("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(तथ्य, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      वापस CURLE_COULDNT_CONNECT;
    }
#पूर्ण_अगर
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    अगर((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(तथ्य, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, माप(errbuf)));
    वापस CURLE_COULDNT_CONNECT;
  }

#अगर_घोषित USE_SO_NOSIGPIPE
  अगर(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(तथ्य, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, माप(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    वापस CURLE_COULDNT_CONNECT;
  }
#पूर्ण_अगर /* USE_SO_NOSIGPIPE */

#अगर defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  अगर(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(तथ्य, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, माप(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    वापस CURLE_COULDNT_CONNECT;
  }
#पूर्ण_अगर

#अगर defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  अगर(addr->family == AF_INET6) {
    काष्ठा sockaddr_in6 * स्थिर sa6 = (व्योम *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(तथ्य, sa6);
  }
#पूर्ण_अगर
  वापस CURLE_OK;
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
CURLcode Curl_socket_open(काष्ठा Curl_easy *तथ्य,
                          स्थिर काष्ठा Curl_addrinfo *ai,
                          काष्ठा Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  काष्ठा Curl_sockaddr_ex dummy;
  CURLcode परिणाम;

  अगर(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  परिणाम = Curl_socket_addr_from_ai(addr, ai, transport);
  अगर(परिणाम)
    वापस परिणाम;

  वापस socket_open(तथ्य, addr, sockfd);
}

अटल पूर्णांक socket_close(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                        पूर्णांक use_callback, curl_socket_t sock)
{
  अगर(sock == CURL_SOCKET_BAD)
    वापस 0;

  अगर(use_callback && conn && conn->fclosesocket) {
    काष्ठा Curl_mapi_guard guard;
    पूर्णांक rc;
    Curl_multi_will_close(तथ्य, sock);
    CURL_CBAPI_START(&guard, तथ्य, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_END(&guard);
    वापस rc;
  }

  अगर(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(तथ्य, sock);

  sclose(sock);

  वापस 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
पूर्णांक Curl_socket_close(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                      curl_socket_t sock)
{
  वापस socket_close(तथ्य, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(स्थिर अक्षर *निवेश,
                              अक्षर **dev, अक्षर **iface, अक्षर **host)
{
  अटल स्थिर अक्षर if_prefix[] = "if!";
  अटल स्थिर अक्षर host_prefix[] = "host!";
  अटल स्थिर अक्षर if_host_prefix[] = "ifhost!";
  माप_प्रकार len;

  len = म_माप(निवेश);
  अगर(len > 512)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  अगर(dev)
    *dev = शून्य;
  अगर(iface)
    *iface = शून्य;
  अगर(host)
    *host = शून्य;

  अगर(!म_भेदन(if_prefix, निवेश, म_माप(if_prefix))) {
    निवेश += म_माप(if_prefix);
    अगर(!*निवेश)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    अगर(iface) {
      *iface = curlx_memdup0(निवेश, len - म_माप(if_prefix));
      वापस *iface ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    वापस CURLE_OK;
  }
  अन्यथा अगर(!म_भेदन(host_prefix, निवेश, म_माप(host_prefix))) {
    निवेश += म_माप(host_prefix);
    अगर(!*निवेश)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    अगर(host) {
      *host = curlx_memdup0(निवेश, len - म_माप(host_prefix));
      वापस *host ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    वापस CURLE_OK;
  }
  अन्यथा अगर(!म_भेदन(if_host_prefix, निवेश, म_माप(if_host_prefix))) {
    स्थिर अक्षर *host_part;
    निवेश += म_माप(if_host_prefix);
    len -= म_माप(if_host_prefix);
    host_part = स_प्रथम(निवेश, '!', len);
    अगर(!host_part || !*(host_part + 1))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    अगर(iface) {
      *iface = curlx_memdup0(निवेश, host_part - निवेश);
      अगर(!*iface)
        वापस CURLE_OUT_OF_MEMORY;
    }
    ++host_part;
    अगर(host) {
      *host = curlx_memdup0(host_part, len - (host_part - निवेश));
      अगर(!*host) {
        अगर(iface)
          curlx_safefree(*iface);
        वापस CURLE_OUT_OF_MEMORY;
      }
    }
    वापस CURLE_OK;
  }

  अगर(!*निवेश)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  अगर(dev) {
    *dev = curlx_memdup0(निवेश, len);
    वापस *dev ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  वापस CURLE_OK;
}

#अगर_अघोषित CURL_DISABLE_BINDLOCAL
अटल CURLcode bindlocal(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                          curl_socket_t sockfd, पूर्णांक af, अचिन्हित पूर्णांक scope,
                          uint8_t transport)
{
  काष्ठा Curl_sockaddr_storage sa;
  काष्ठा sockaddr *sock = (काष्ठा sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  काष्ठा sockaddr_in *si4 = (काष्ठा sockaddr_in *)&sa;
#अगर_घोषित USE_IPV6
  काष्ठा sockaddr_in6 *si6 = (काष्ठा sockaddr_in6 *)&sa;
#पूर्ण_अगर
  काष्ठा Curl_dns_entry *h = शून्य;
  अचिन्हित लघु port = तथ्य->set.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  पूर्णांक portnum = तथ्य->set.localportrange;
  अक्षर *dev_in = शून्य, *iface_in = शून्य, *host_in = शून्य;
  स्थिर अक्षर *iface = शून्य, *host = शून्य;
  पूर्णांक sockerr;
#अगर_घोषित IP_BIND_ADDRESS_NO_PORT
  पूर्णांक on = 1;
#पूर्ण_अगर
  CURLcode परिणाम = CURLE_OK;

#अगर_अघोषित USE_IPV6
  (व्योम)scope;
#पूर्ण_अगर
  अगर(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    परिणाम = Curl_parse_interface(conn->interface_opts,
                                  &dev_in, &iface_in, &host_in);
    अगर(परिणाम)
      जाओ out;
    /* This is how we use the values */
    iface = iface_in ? iface_in : dev_in;
    host = host_in ? host_in : dev_in;
  }

  अगर(!iface && !host && !port)
    जाओ out;

  अगर(iface && (म_माप(iface) >= 255)) {
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }
  स_रखो(&sa, 0, माप(काष्ठा Curl_sockaddr_storage));

  अगर(iface || host) {
    अक्षर myhost[256] = "";
    पूर्णांक done = 0; /* -1 for error, 1 for address found */
    if2ip_परिणाम_t if2ip_परिणाम = IF2IP_NOT_FOUND;

#अगर_घोषित SO_BINDTODEVICE
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
    अगर(iface &&
       setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                  iface, (curl_socklen_t)म_माप(iface) + 1) == 0 &&
       /* This is often "errno 1, error: Operation not permitted" if you are
        * not running as root or another suitable privileged user. If it
        * succeeds it means the parameter was a valid interface and not an IP
        * address. Return immediately.
        */
       !host_in) {
      infof(तथ्य, "socket successfully bound to interface '%s'", iface);
      परिणाम = CURLE_OK;
      जाओ out;
    }
#पूर्ण_अगर
    अगर(!host_in) {
      /* Discover IP from input device, then bind to it */
      if2ip_परिणाम = Curl_if2ip(af,
#अगर_घोषित USE_IPV6
                                scope, conn->scope_id,
#पूर्ण_अगर
                                iface, myhost, माप(myhost));
    }
    चयन(if2ip_परिणाम) {
    हाल IF2IP_NOT_FOUND:
      अगर(iface_in && !host_in) {
        /* Do not fall back to treating it as a hostname */
        अक्षर बफर[STRERROR_LEN];
        तथ्य->state.os_errno = sockerr = SOCKERRNO;
        failf(तथ्य, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, बफर, माप(बफर)));
        परिणाम = CURLE_INTERFACE_FAILED;
        जाओ out;
      }
      अवरोध;
    हाल IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
      जाओ out;
    हाल IF2IP_FOUND:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(तथ्य, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      done = 1;
      अवरोध;
    }
    अगर(!iface_in || host_in) {
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
#अगर_घोषित USE_IPV6
      अगर(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#पूर्ण_अगर

      (व्योम)Curl_resolv_blocking(तथ्य, dns_queries, host, 80, transport, &h);
      अगर(h) {
        पूर्णांक h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_address(h->addr, myhost, माप(myhost));
        infof(तथ्य, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_entry_unlink(तथ्य, &h); /* this will NULL, potential free h */
        अगर(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
          जाओ out;
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

    अगर(done > 0) {
#अगर_घोषित USE_IPV6
      /* IPv6 address */
      अगर(af == AF_INET6) {
#अगर_घोषित HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        अक्षर *scope_ptr = म_अक्षर(myhost, '%');
        अगर(scope_ptr)
          *(scope_ptr++) = '\0';
#पूर्ण_अगर
        अगर(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(port);
#अगर_घोषित HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          अगर(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            अगर(curlx_str_संख्या((स्थिर अक्षर **)CURL_UNCONST(&scope_ptr),
                                &scope_id, अच_पूर्णांक_उच्च)) {
              परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
              जाओ out;
            }
            si6->sin6_scope_id = (अचिन्हित पूर्णांक)scope_id;
          }
#पूर्ण_अगर
        }
        sizeof_sa = माप(काष्ठा sockaddr_in6);
      }
      अन्यथा
#पूर्ण_अगर
      /* IPv4 address */
      अगर((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(port);
        sizeof_sa = माप(काष्ठा sockaddr_in);
      }
    }

    अगर(done < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      अक्षर बफर[STRERROR_LEN];
      तथ्य->state.errorbuf = FALSE;
      तथ्य->state.os_errno = sockerr = SOCKERRNO;
      failf(तथ्य, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, बफर, माप(बफर)));
      परिणाम = CURLE_INTERFACE_FAILED;
      जाओ out;
    }
  }
  अन्यथा {
    /* no device was given, prepare sa to match af's needs */
#अगर_घोषित USE_IPV6
    अगर(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(port);
      sizeof_sa = माप(काष्ठा sockaddr_in6);
    }
    अन्यथा
#पूर्ण_अगर
    अगर(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(port);
      sizeof_sa = माप(काष्ठा sockaddr_in);
    }
  }
#अगर_घोषित IP_BIND_ADDRESS_NO_PORT
  (व्योम)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, माप(on));
#पूर्ण_अगर
  क्रम(;;) {
    अगर(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(तथ्य, "Local port: %hu", port);
      conn->bits.bound = TRUE;
      परिणाम = CURLE_OK;
      जाओ out;
    }

    अगर(--portnum > 0) {
      port++; /* try next port */
      अगर(port == 0)
        अवरोध;
      infof(तथ्य, "Bind to local port %d failed, trying next", port - 1);
      /* We reuse/clobber the port variable here below */
      अगर(sock->sa_family == AF_INET)
        si4->sin_port = htons(port);
#अगर_घोषित USE_IPV6
      अन्यथा
        si6->sin6_port = htons(port);
#पूर्ण_अगर
    }
    अन्यथा
      अवरोध;
  }
  {
    अक्षर बफर[STRERROR_LEN];
    तथ्य->state.os_errno = sockerr = SOCKERRNO;
    failf(तथ्य, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, बफर, माप(बफर)));
  }

  परिणाम = CURLE_INTERFACE_FAILED;

out:
  curlx_free(dev_in);
  curlx_free(iface_in);
  curlx_free(host_in);
  वापस परिणाम;
}
#पूर्ण_अगर

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
अटल bool verifyconnect(curl_socket_t sockfd, पूर्णांक *psockerr)
{
  bool rc = TRUE;
#अगर_घोषित SO_ERROR
  पूर्णांक sockerr = 0;
  curl_socklen_t err_आकार = माप(sockerr);

#अगर_घोषित _WIN32
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
#पूर्ण_अगर

  अगर(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (व्योम *)&sockerr, &err_आकार))
    sockerr = SOCKERRNO;
#अगर defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  अगर(EBADIOCTL == sockerr) {
    SET_SOCKERRNO(0);
    sockerr = 0;
  }
#पूर्ण_अगर
  अगर((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  अन्यथा
    /* This was not a successful connect */
    rc = FALSE;
  अगर(psockerr)
    *psockerr = sockerr;
#अन्यथा
  (व्योम)sockfd;
  अगर(psockerr)
    *psockerr = SOCKERRNO;
#पूर्ण_अगर
  वापस rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
अटल CURLcode socket_connect_परिणाम(काष्ठा Curl_easy *तथ्य,
                                      स्थिर अक्षर *ipaddress, पूर्णांक sockerr)
{
  अगर(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    वापस CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(अक्षर बफर[STRERROR_LEN]);
    infof(तथ्य, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, बफर, माप(बफर)));
    NOVERBOSE((व्योम)ipaddress);
  }
  तथ्य->state.os_errno = sockerr;
  /* connect failed */
  वापस CURLE_COULDNT_CONNECT;
}

काष्ठा cf_socket_ctx {
  काष्ठा Curl_peer *peer;
  काष्ठा Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  काष्ठा ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  काष्ठा curltime started_at;        /* when socket was created */
  काष्ठा curltime connected_at;      /* when socket connected/got first byte */
  काष्ठा curltime first_byte_at;     /* when first byte was recvd */
#अगर_घोषित USE_WINSOCK
  काष्ठा curltime last_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_आकार;                     /* the last set SO_SNDBUF size */
#पूर्ण_अगर
  पूर्णांक sockerr;                       /* socket error of last failure or 0 */
#अगर_घोषित DEBUGBUILD
  पूर्णांक connect_wait_ms;               /* ms to wait after 1st connect() */
  पूर्णांक wblock_percent;                /* percent of writes doing EAGAIN */
  पूर्णांक wpartial_percent;              /* percent of bytes written in send */
  पूर्णांक rblock_percent;                /* percent of reads doing EAGAIN */
  माप_प्रकार recv_max;                   /* max enforced read size */
#पूर्ण_अगर
  uint8_t transport;
  BIT(got_first_byte);               /* if first byte was received */
  BIT(listening);                    /* socket is listening */
  BIT(accepted);                     /* socket was accepted, not connected */
  BIT(sock_connected);               /* socket is "connected", e.g. in UDP */
  BIT(active);
  BIT(stats_reported);
};

अटल CURLcode cf_socket_ctx_init(काष्ठा cf_socket_ctx *ctx,
                                   काष्ठा Curl_peer *peer,
                                   काष्ठा Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  स_रखो(ctx, 0, माप(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#अगर_घोषित DEBUGBUILD
  {
    स्थिर अक्षर *p = दो_पर्या("CURL_DBG_SOCK_WBLOCK");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, 100))
        ctx->wblock_percent = (पूर्णांक)l;
    }
    p = दो_पर्या("CURL_DBG_SOCK_WPARTIAL");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, 100))
        ctx->wpartial_percent = (पूर्णांक)l;
    }
    p = दो_पर्या("CURL_DBG_SOCK_RBLOCK");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, 100))
        ctx->rblock_percent = (पूर्णांक)l;
    }
    p = दो_पर्या("CURL_DBG_SOCK_RMAX");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (माप_प्रकार)l;
    }
    p = दो_पर्या("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, पूर्णांक_उच्च))
        ctx->connect_wait_ms = (पूर्णांक)l;
    }
  }
#पूर्ण_अगर

  वापस CURLE_OK;
}

अटल व्योम cf_socket_ctx_free(काष्ठा cf_socket_ctx *ctx)
{
  अगर(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

अटल CURLcode cf_socket_shutdown(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   bool *done)
{
  अगर(cf->connected) {
    काष्ठा cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(तथ्य, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    अगर(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      अचिन्हित अक्षर buf[1024];
      (व्योम)sread(ctx->sock, buf, माप(buf));
    }
  }
  *done = TRUE;
  वापस CURLE_OK;
}

अटल व्योम cf_socket_destroy(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(तथ्य, cf, "destroy");
  अगर(ctx) {
    अगर(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(तथ्य, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      अगर(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(तथ्य, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

अटल व्योम set_local_ip(काष्ठा Curl_cfilter *cf,
                         काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_port = 0;

#अगर_घोषित HAVE_GETSOCKNAME
  अगर((ctx->sock != CURL_SOCKET_BAD) &&
     !(तथ्य->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    काष्ठा Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = माप(काष्ठा Curl_sockaddr_storage);

    स_रखो(&ssloc, 0, माप(ssloc));
    अगर(getsockname(ctx->sock, (काष्ठा sockaddr *)&ssloc, &slen)) {
      VERBOSE(अक्षर बफर[STRERROR_LEN]);
      VERBOSE(पूर्णांक sockerr = SOCKERRNO);
      infof(तथ्य, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, बफर, माप(बफर)));
    }
    अन्यथा {
      CURLcode परिणाम = sockaddr2string((काष्ठा sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_port);
      अगर(परिणाम)
        infof(तथ्य, "ssloc inet_ntop() failed with %d", (पूर्णांक)परिणाम);
    }
  }
#अन्यथा
  (व्योम)तथ्य;
#पूर्ण_अगर
}

अटल CURLcode set_remote_ip(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  CURLcode परिणाम;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  परिणाम = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_port);
  अगर(परिणाम) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(तथ्य, "curl_sa_addr inet_ntop() failed with %d", (पूर्णांक)परिणाम);
    वापस CURLE_FAILED_INIT;
  }
  वापस CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
अटल पूर्णांक cf_socktype(पूर्णांक x)
{
#अगर_घोषित SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#पूर्ण_अगर
#अगर_घोषित CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#पूर्ण_अगर
  वापस x;
}

#अगर_घोषित _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#अगर_अघोषित SIO_TCP_INITIAL_RTO
#घोषणा SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#घोषणा TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकार काष्ठा _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#पूर्ण_अगर /* SIO_TCP_INITIAL_RTO */

#अगर_अघोषित TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#घोषणा TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#पूर्ण_अगर

अटल bool targets_localhost(काष्ठा cf_socket_ctx *ctx)
{
  वापस (((ctx->addr.family == AF_INET) &&
           !म_भेद(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !म_भेद(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
अटल व्योम tcplocalhost(काष्ठा Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  अगर(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD bytes = 0;
    स_रखो(&rto, 0, माप(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    अगर(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    अन्यथा
      rto.MaxSynRetransmissions = 1;
    (व्योम)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, माप(rto),
                   शून्य, 0, &bytes, शून्य, शून्य);
  }
}
#अन्यथा
#घोषणा tcplocalhost(x, y)
#पूर्ण_अगर /* _WIN32 */

अटल CURLcode cf_socket_open(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  पूर्णांक error = 0;
  bool isconnected = FALSE;
  CURLcode परिणाम = CURLE_COULDNT_CONNECT;
  bool is_tcp;

  DEBUGASSERT(ctx->sock == CURL_SOCKET_BAD);
  ctx->started_at = *Curl_pgrs_now(तथ्य);
#अगर_घोषित CURL_USE_SOCK_NONBLOCK
  /* Do not tuck SOCK_NONBLOCK into socktype when opensocket callback is set
   * because we would not know how socktype is about to be used in the
   * callback, SOCK_NONBLOCK might get factored out before calling socket().
   */
  अगर(!तथ्य->set.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#पूर्ण_अगर
  परिणाम = socket_open(तथ्य, &ctx->addr, &ctx->sock);
#अगर_घोषित CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  अगर(!तथ्य->set.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#पूर्ण_अगर
  अगर(परिणाम)
    जाओ out;

  परिणाम = set_remote_ip(cf, तथ्य);
  अगर(परिणाम)
    जाओ out;

#अगर_घोषित USE_IPV6
  अगर(ctx->addr.family == AF_INET6) {
#अगर_घोषित USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    पूर्णांक on = 0;
    (व्योम)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (व्योम *)&on, माप(on));
#पूर्ण_अगर
#अगर_घोषित HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      काष्ठा sockaddr_in6 *sa6 = (व्योम *)&ctx->addr.curl_sa_addr;
      अगर(sa6->sin6_scope_id)
        infof(तथ्य, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_port,
              (अचिन्हित दीर्घ)sa6->sin6_scope_id);
      अन्यथा
#पूर्ण_अगर
        infof(तथ्य, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_port);
#अगर_घोषित HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#पूर्ण_अगर
  }
  अन्यथा
#पूर्ण_अगर
    infof(तथ्य, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_port);

#अगर_घोषित USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#अन्यथा
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#पूर्ण_अगर
  अगर(is_tcp) {
    अगर(तथ्य->set.tcp_nodelay)
      tcpnodelay(cf, तथ्य, ctx->sock);

    अगर(तथ्य->set.tcp_keepalive)
      tcpkeepalive(cf, तथ्य, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  अगर(तथ्य->set.fsockopt) {
    /* activate callback for setting socket options */
    काष्ठा Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, तथ्य, easy_fsockopt);
    error = तथ्य->set.fsockopt(तथ्य->set.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_END(&guard);

    अगर(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    अन्यथा अगर(error) {
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
      जाओ out;
    }
  }

#अगर_अघोषित CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  अगर(ctx->addr.family == AF_INET
#अगर_घोषित USE_IPV6
     || ctx->addr.family == AF_INET6
#पूर्ण_अगर
    ) {
    परिणाम = bindlocal(तथ्य, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    अगर(परिणाम) {
      अगर(परिणाम == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        परिणाम = CURLE_COULDNT_CONNECT;
      }
      जाओ out;
    }
  }
#पूर्ण_अगर

#अगर_अघोषित CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  अगर(error < 0) {
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    जाओ out;
  }
#अन्यथा
  अगर(तथ्य->set.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    अगर(error < 0) {
      परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      जाओ out;
    }
  }
#पूर्ण_अगर
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
out:
  अगर(परिणाम) {
    अगर(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(तथ्य, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  अन्यथा अगर(isconnected) {
    set_local_ip(cf, तथ्य);
    ctx->connected_at = *Curl_pgrs_now(तथ्य);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(तथ्य, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (पूर्णांक)परिणाम, ctx->sock);
  वापस परिणाम;
}

अटल पूर्णांक do_connect(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                      bool is_tcp_fastopen)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
#अगर_घोषित TCP_FASTOPEN_CONNECT
  पूर्णांक optval = 1;
#पूर्ण_अगर
  पूर्णांक rc = -1;

  (व्योम)तथ्य;
  अगर(is_tcp_fastopen) {
#अगर_घोषित CONNECT_तथ्य_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_AVAILABLE
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    अगर(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = शून्य;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_तथ्य_IDEMPOTENT,
                    शून्य, 0, शून्य, शून्य);
    }
    अन्यथा {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  अन्यथा
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#या_अगर defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    अगर(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (व्योम *)&optval, माप(optval)) < 0)
      CURL_TRC_CF(तथ्य, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#या_अगर defined(MSG_FASTOPEN) /* old Linux */
    अगर(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    अन्यथा
      rc = 0; /* Do nothing */
#पूर्ण_अगर
  }
  अन्यथा {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  वापस rc;
}

अटल CURLcode cf_tcp_connect(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               bool *done)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_COULDNT_CONNECT;
  पूर्णांक rc = 0;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  *done = FALSE; /* a negative world view is best */
  अगर(ctx->sock == CURL_SOCKET_BAD) {
    पूर्णांक sockerr;

    परिणाम = cf_socket_open(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;

    अगर(cf->connected) {
      *done = TRUE;
      वापस CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, तथ्य, (bool)cf->conn->bits.tcp_fastopen);
    sockerr = SOCKERRNO;
    set_local_ip(cf, तथ्य);
    CURL_TRC_CF(तथ्य, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_port);
#अगर_घोषित DEBUGBUILD
    अगर(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(तथ्य, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#पूर्ण_अगर
    अगर(rc == -1) {
      ctx->sockerr = sockerr;
      परिणाम = socket_connect_परिणाम(तथ्य, ctx->ip.remote_ip, sockerr);
      जाओ out;
    }
  }

#अगर_घोषित mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (व्योम)verifyconnect(ctx->sock, शून्य);
#पूर्ण_अगर
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  अगर(rc == 0) { /* no connection yet */
    CURL_TRC_CF(तथ्य, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    वापस CURLE_OK;
  }
  अन्यथा अगर(rc == CURL_CSELECT_OUT ||
#अगर_घोषित __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#पूर्ण_अगर
          cf->conn->bits.tcp_fastopen) {
    अगर(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(तथ्य);
      set_local_ip(cf, तथ्य);
      *done = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(तथ्य, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      वापस CURLE_OK;
    }
    परिणाम = CURLE_COULDNT_CONNECT;
  }
  अन्यथा अगर(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(तथ्य, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (व्योम)verifyconnect(ctx->sock, &ctx->sockerr);
    परिणाम = CURLE_COULDNT_CONNECT;
  }

out:
  अगर(परिणाम) {
    VERBOSE(अक्षर बफर[STRERROR_LEN]);
    set_local_ip(cf, तथ्य);
    अगर(ctx->sockerr) {
      तथ्य->state.os_errno = ctx->sockerr;
      SET_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, बफर, माप(बफर)));
    }
    अन्यथा {
      VERBOSE(curlx_strcopy(बफर, माप(बफर), STRCONST("peer closed")));
    }
    अगर(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(तथ्य, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(तथ्य, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_port,
          ctx->ip.local_ip, ctx->ip.local_port,
          curlx_strerror(ctx->sockerr, बफर, माप(बफर)));
    *done = FALSE;
  }
  वापस परिणाम;
}

अटल CURLcode cf_socket_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य,
                                         काष्ठा easy_pollset *ps)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    अगर(ctx->listening) {
      परिणाम = Curl_pollset_set_in_only(तथ्य, ps, ctx->sock);
      CURL_TRC_CF(तथ्य, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    अन्यथा अगर(!cf->connected) {
      परिणाम = Curl_pollset_set_out_only(तथ्य, ps, ctx->sock);
      CURL_TRC_CF(तथ्य, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    अन्यथा अगर(!ctx->active) {
      परिणाम = Curl_pollset_add_in(तथ्य, ps, ctx->sock);
      CURL_TRC_CF(तथ्य, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  वापस परिणाम;
}

#अगर_घोषित USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#अगर_अघोषित SIO_IDEAL_SEND_BACKLOG_QUERY
#घोषणा SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#पूर्ण_अगर

अटल व्योम win_update_sndbuf_आकार(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  अगर(curlx_ptimediff_ms(Curl_pgrs_now(तथ्य),
                        &ctx->last_sndbuf_query_at) > 1000) {
    अगर(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, माप(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_आकार &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (स्थिर अक्षर *)&ideal, माप(ideal))) {
      ctx->sndbuf_आकार = ideal;
    }
    ctx->last_sndbuf_query_at = *Curl_pgrs_now(तथ्य);
  }
}

#पूर्ण_अगर /* USE_WINSOCK */

अटल CURLcode cf_socket_send(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                               स्थिर uint8_t *buf, माप_प्रकार len, bool eos,
                               माप_प्रकार *pnwritten)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode परिणाम = CURLE_OK;
  VERBOSE(माप_प्रकार orig_len = len);

  (व्योम)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#अगर_घोषित DEBUGBUILD
  /* simulate network blocking/partial writes */
  अगर(ctx->wblock_percent > 0) {
    अचिन्हित अक्षर c = 0;
    Curl_rand_bytes(तथ्य, FALSE, &c, 1);
    अगर(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(तथ्य, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      वापस CURLE_AGAIN;
    }
  }
  अगर(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    अगर(!len)
      len = 1;
    CURL_TRC_CF(तथ्य, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#पूर्ण_अगर

#अगर defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  अगर(cf->conn->bits.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->bits.tcp_fastopen = FALSE;
  }
  अन्यथा
#पूर्ण_अगर
    rv = swrite(ctx->sock, buf, len);

  अगर(!curlx_sztouz(rv, pnwritten)) {
    पूर्णांक sockerr = SOCKERRNO;
    अगर(SOCK_EAGAIN(sockerr)
#अगर_अघोषित USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#पूर्ण_अगर
      ) {
      परिणाम = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    अन्यथा {
      अक्षर बफर[STRERROR_LEN];
      failf(तथ्य, "Send failure: %s",
            curlx_strerror(sockerr, बफर, माप(बफर)));
      तथ्य->state.os_errno = sockerr;
      परिणाम = CURLE_SEND_ERROR;
    }
  }

#अगर_घोषित USE_WINSOCK
  अगर(!परिणाम)
    win_update_sndbuf_आकार(तथ्य, ctx);
#पूर्ण_अगर

  CURL_TRC_CF(तथ्य, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (पूर्णांक)परिणाम, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  वापस परिणाम;
}

अटल CURLcode cf_socket_recv(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                               अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#अगर_घोषित DEBUGBUILD
  /* simulate network blocking/partial reads */
  अगर(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    अचिन्हित अक्षर c = 0;
    Curl_rand(तथ्य, &c, 1);
    अगर(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(तथ्य, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      वापस CURLE_AGAIN;
    }
  }
  अगर(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(तथ्य, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#पूर्ण_अगर

  rv = sread(ctx->sock, buf, len);

  अगर(!curlx_sztouz(rv, pnread)) {
    पूर्णांक sockerr = SOCKERRNO;
    अगर(SOCK_EAGAIN(sockerr)
#अगर_अघोषित USE_WINSOCK
       || (sockerr == SOCKEINTR)
#पूर्ण_अगर
      ) {
      परिणाम = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    अन्यथा {
      अक्षर बफर[STRERROR_LEN];
      failf(तथ्य, "Recv failure: %s",
            curlx_strerror(sockerr, बफर, माप(बफर)));
      तथ्य->state.os_errno = sockerr;
      परिणाम = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(तथ्य, cf, "recv(len=%zu) -> %d, %zu", len, (पूर्णांक)परिणाम, *pnread);
  अगर(!परिणाम && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(तथ्य);
    ctx->got_first_byte = TRUE;
  }
  वापस परिणाम;
}

अटल व्योम cf_socket_update_तथ्य(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य)
{
  /* Update the IP info held in the transfer, if we have that. */
  अगर(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    काष्ठा cf_socket_ctx *ctx = cf->ctx;
    तथ्य->info.primary = ctx->ip;
  }
}

अटल व्योम cf_socket_active(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  set_local_ip(cf, तथ्य);
#अगर_घोषित USE_IPV6
  अगर(cf->sockindex == FIRSTSOCKET)
    cf->conn->bits.ipv6 = (ctx->addr.family == AF_INET6);
#पूर्ण_अगर
  ctx->active = TRUE;
}

अटल CURLcode cf_socket_cntrl(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;

  (व्योम)arg1;
  (व्योम)arg2;
  चयन(event) {
  हाल CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_active(cf, तथ्य);
    cf_socket_update_तथ्य(cf, तथ्य);
    अवरोध;
  हाल CF_CTRL_तथ्य_SETUP:
    cf_socket_update_तथ्य(cf, तथ्य);
    अवरोध;
  हाल CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    अवरोध;
  हाल CF_CTRL_REPORT_STATS:
    अगर(cf->connected && !ctx->stats_reported) {
      काष्ठा curltime *ts = शून्य;
      चयन(ctx->transport) {
      हाल TRNSPRT_UDP:
      हाल TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        अगर(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        अवरोध;
      शेष:
        ts = &ctx->connected_at;
        अवरोध;
      }
      अगर(ts) {
        Curl_pgrsTimeWas(तथ्य, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  वापस CURLE_OK;
}

अटल bool cf_socket_conn_is_alive(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    bool *निवेश_pending)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  काष्ठा pollfd pfd[1];
  पूर्णांक r;

  *निवेश_pending = FALSE;

  अगर(!ctx || ctx->sock == CURL_SOCKET_BAD)
    वापस FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  अगर(r < 0) {
    CURL_TRC_CF(तथ्य, cf, "is_alive: poll error, assume dead");
    वापस FALSE;
  }
  अन्यथा अगर(r == 0) {
    CURL_TRC_CF(तथ्य, cf, "is_alive: poll timeout, assume alive");
    वापस TRUE;
  }
  अन्यथा अगर(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(तथ्य, cf, "is_alive: err/hup/etc events, assume dead");
    वापस FALSE;
  }

  CURL_TRC_CF(तथ्य, cf, "is_alive: valid events, looks alive");
  *निवेश_pending = TRUE;
  वापस TRUE;
}

अटल CURLcode cf_socket_query(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;

  चयन(query) {
  हाल CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    वापस CURLE_OK;
  हाल CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    वापस CURLE_OK;
  हाल CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((स्थिर काष्ठा Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : शून्य;
    वापस CURLE_OK;
  हाल CF_QUERY_CONNECT_REPLY_MS:
    अगर(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < पूर्णांक_उच्च) ? (पूर्णांक)ms : पूर्णांक_उच्च;
    }
    अन्यथा
      *pres1 = -1;
    वापस CURLE_OK;
  हाल CF_QUERY_IP_INFO:
#अगर_घोषित USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#अन्यथा
    *pres1 = FALSE;
#पूर्ण_अगर
    *(काष्ठा ip_quadruple *)pres2 = ctx->ip;
    वापस CURLE_OK;
  हाल CF_QUERY_REALLY_CONNECTED:
    अगर(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    अन्यथा
      *pres1 = ctx->got_first_byte;
    वापस CURLE_OK;
  शेष:
    अवरोध;
  }
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

काष्ठा Curl_cftype Curl_cft_tcp = {
  "TCP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_तथ्य_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_tcp_create(काष्ठा Curl_cfilter **pcf,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_peer *origin,
                            काष्ठा Curl_peer *peer,
                            uint8_t transport_peer,
                            काष्ठा connectdata *conn,
                            काष्ठा Curl_sockaddr_ex *addr,
                            काष्ठा Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  काष्ठा cf_socket_ctx *ctx = शून्य;
  काष्ठा Curl_cfilter *cf = शून्य;
  CURLcode परिणाम;

  (व्योम)तथ्य;
  (व्योम)origin;
  (व्योम)conn;
  (व्योम)tunnel_peer;
  (व्योम)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  अगर(!addr) {
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }

  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  परिणाम = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

out:
  *pcf = (!परिणाम) ? cf : शून्य;
  अगर(परिणाम) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  वापस परिणाम;
}

#अगर_घोषित __linux__
अटल व्योम linux_quic_mtu(काष्ठा cf_socket_ctx *ctx)
{
  पूर्णांक val;
  चयन(ctx->addr.family) {
#अगर_घोषित IP_MTU_DISCOVER
  हाल AF_INET:
    val = IP_PMTUDISC_DO;
    (व्योम)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     माप(val));
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित IPV6_MTU_DISCOVER
  हाल AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (व्योम)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     माप(val));
    अवरोध;
#पूर्ण_अगर
  }
}
#अन्यथा
#घोषणा linux_quic_mtu(x)
#पूर्ण_अगर

#अगर defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
अटल व्योम linux_quic_gro(काष्ठा cf_socket_ctx *ctx)
{
  पूर्णांक one = 1;
  (व्योम)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)माप(one));
}
#अन्यथा
#घोषणा linux_quic_gro(x)
#पूर्ण_अगर

#अगर (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
अटल व्योम linux_quic_ecn(काष्ठा cf_socket_ctx *ctx)
{
  अचिन्हित पूर्णांक tos = 1;
  चयन(ctx->addr.family) {
  हाल AF_INET:
    (व्योम)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, माप(tos));
    अवरोध;
#अगर_घोषित IPV6_RECVTCLASS
  हाल AF_INET6:
    (व्योम)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, माप(tos));
    अवरोध;
#पूर्ण_अगर
  }
}
#अन्यथा
#घोषणा linux_quic_ecn(x)
#पूर्ण_अगर

#अगर (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
अटल व्योम linux_ip_dontfrag(काष्ठा cf_socket_ctx *ctx)
{
  पूर्णांक val = 1;

  चयन(ctx->addr.family) {
  हाल AF_INET:
    (व्योम)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, माप(val));
    अवरोध;
#अगर_घोषित IPV6_DONTFRAG
  हाल AF_INET6:
    (व्योम)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, माप(val));
    अवरोध;
#पूर्ण_अगर
  }
}
#अन्यथा
#घोषणा linux_ip_dontfrag(x)
#पूर्ण_अगर

अटल CURLcode cf_udp_setup_quic(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  पूर्णांक rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  अगर(rc == -1) {
    वापस socket_connect_परिणाम(तथ्य, ctx->ip.remote_ip, SOCKERRNO);
  }
  ctx->sock_connected = TRUE;
  set_local_ip(cf, तथ्य);
  CURL_TRC_CF(तथ्य, cf, "%s socket %" FMT_SOCKET_T
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

  वापस CURLE_OK;
}

अटल CURLcode cf_udp_connect(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               bool *done)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_COULDNT_CONNECT;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  *done = FALSE;
  अगर(ctx->sock == CURL_SOCKET_BAD) {
    परिणाम = cf_socket_open(cf, तथ्य);
    अगर(परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "cf_udp_connect(), open failed -> %d",
                  (पूर्णांक)परिणाम);
      जाओ out;
    }

    अगर(ctx->transport == TRNSPRT_QUIC) {
      परिणाम = cf_udp_setup_quic(cf, तथ्य);
      अगर(परिणाम)
        जाओ out;
      CURL_TRC_CF(तथ्य, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
    }
    *done = TRUE;
    cf->connected = TRUE;
  }
out:
  वापस परिणाम;
}

काष्ठा Curl_cftype Curl_cft_udp = {
  "UDP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_udp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_तथ्य_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_udp_create(काष्ठा Curl_cfilter **pcf,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_peer *origin,
                            काष्ठा Curl_peer *peer,
                            uint8_t transport_peer,
                            काष्ठा connectdata *conn,
                            काष्ठा Curl_sockaddr_ex *addr,
                            काष्ठा Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  काष्ठा cf_socket_ctx *ctx = शून्य;
  काष्ठा Curl_cfilter *cf = शून्य;
  CURLcode परिणाम;

  (व्योम)तथ्य;
  (व्योम)origin;
  (व्योम)conn;
  (व्योम)tunnel_peer;
  (व्योम)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  परिणाम = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

out:
  *pcf = (!परिणाम) ? cf : शून्य;
  अगर(परिणाम) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  वापस परिणाम;
}

/* this is the TCP filter which can also handle this case */
काष्ठा Curl_cftype Curl_cft_unix = {
  "UNIX",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_तथ्य_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_unix_create(काष्ठा Curl_cfilter **pcf,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_peer *origin,
                            काष्ठा Curl_peer *peer,
                            uint8_t transport_peer,
                            काष्ठा connectdata *conn,
                            काष्ठा Curl_sockaddr_ex *addr,
                            काष्ठा Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  काष्ठा cf_socket_ctx *ctx = शून्य;
  काष्ठा Curl_cfilter *cf = शून्य;
  CURLcode परिणाम;

  (व्योम)तथ्य;
  (व्योम)origin;
  (व्योम)conn;
  (व्योम)tunnel_peer;
  (व्योम)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  परिणाम = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

out:
  *pcf = (!परिणाम) ? cf : शून्य;
  अगर(परिणाम) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  वापस परिणाम;
}

अटल timediff_t cf_tcp_accept_timeleft(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#अगर_अघोषित CURL_DISABLE_FTP
  अगर(तथ्य->set.accepttimeout > 0)
    timeout_ms = तथ्य->set.accepttimeout;
#पूर्ण_अगर

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(तथ्य);
  अगर(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  अन्यथा {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &ctx->started_at);
    अगर(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  वापस timeout_ms;
}

अटल व्योम cf_tcp_set_accepted_remote_ip(काष्ठा Curl_cfilter *cf,
                                          काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
#अगर_घोषित HAVE_GETPEERNAME
  CURLcode परिणाम;
  काष्ठा Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  plen = माप(ssrem);
  स_रखो(&ssrem, 0, plen);
  अगर(getpeername(ctx->sock, (काष्ठा sockaddr *)&ssrem, &plen)) {
    अक्षर बफर[STRERROR_LEN];
    पूर्णांक sockerr = SOCKERRNO;
    failf(तथ्य, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, बफर, माप(बफर)));
    वापस;
  }
  परिणाम = sockaddr2string((काष्ठा sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_port);
  अगर(परिणाम) {
    failf(तथ्य, "ssrem inet_ntop() failed with %d", (पूर्णांक)परिणाम);
    वापस;
  }
#अन्यथा
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  (व्योम)तथ्य;
#पूर्ण_अगर
}

अटल CURLcode cf_tcp_accept_connect(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      bool *done)
{
  काष्ठा cf_socket_ctx *ctx = cf->ctx;
  अक्षर errbuf[STRERROR_LEN];
#अगर_घोषित USE_IPV6
  काष्ठा Curl_sockaddr_storage add;
#अन्यथा
  काष्ठा sockaddr_in add;
#पूर्ण_अगर
  curl_socklen_t आकार = (curl_socklen_t)माप(add);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  पूर्णांक socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (व्योम)तथ्य;
  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  *done = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, तथ्य);
  अगर(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(तथ्य, "Accept timeout occurred while waiting server connect");
    वापस CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(तथ्य, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(तथ्य, cf, "socket_check -> %x", (अचिन्हित पूर्णांक)socketstate);
  चयन(socketstate) {
  हाल -1: /* error */
    /* let's die here */
    failf(तथ्य, "Error while waiting for server connect");
    वापस CURLE_FTP_ACCEPT_FAILED;
  शेष:
    अगर(socketstate & CURL_CSELECT_IN) {
      infof(तथ्य, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    अवरोध;
  }

  अगर(!incoming) {
    CURL_TRC_CF(तथ्य, cf, "nothing heard from the server yet");
    वापस CURLE_OK;
  }

  आकार = माप(add);
#अगर_घोषित HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (काष्ठा sockaddr *)&add, &आकार,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#अन्यथा
  s_accepted = CURL_ACCEPT(ctx->sock, (काष्ठा sockaddr *)&add, &आकार);
#पूर्ण_अगर

  अगर(s_accepted == CURL_SOCKET_BAD) {
    failf(तथ्य, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, माप(errbuf)));
    वापस CURLE_FTP_ACCEPT_FAILED;
  }
#अगर_अघोषित HAVE_ACCEPT4
#अगर_घोषित HAVE_FCNTL
  अगर(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(तथ्य, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, माप(errbuf)));
    Curl_socket_close(तथ्य, cf->conn, s_accepted);
    वापस CURLE_FTP_ACCEPT_FAILED;
  }
#पूर्ण_अगर /* HAVE_FCNTL */
  अगर(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(तथ्य, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, माप(errbuf)));
    Curl_socket_close(तथ्य, cf->conn, s_accepted);
    वापस CURLE_FTP_ACCEPT_FAILED;
  }
#पूर्ण_अगर /* !HAVE_ACCEPT4 */
  infof(तथ्य, "Connection accepted from server");

  /* Replace any filter on SECONDARY with one listening on this socket */
  ctx->listening = FALSE;
  ctx->accepted = TRUE;
  socket_close(तथ्य, cf->conn, TRUE, ctx->sock);
  ctx->sock = s_accepted;

  cf->conn->sock[cf->sockindex] = ctx->sock;
  cf_tcp_set_accepted_remote_ip(cf, तथ्य);
  set_local_ip(cf, तथ्य);
  ctx->active = TRUE;
  ctx->connected_at = *Curl_pgrs_now(तथ्य);
  cf->connected = TRUE;
  CURL_TRC_CF(तथ्य, cf, "accepted_set(sock=%" FMT_SOCKET_T
              ", remote=%s port=%d)",
              ctx->sock, ctx->ip.remote_ip, ctx->ip.remote_port);

  अगर(तथ्य->set.fsockopt) {
    काष्ठा Curl_mapi_guard guard;
    पूर्णांक error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, तथ्य, easy_fsockopt);
    error = तथ्य->set.fsockopt(तथ्य->set.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_END(&guard);

    अगर(error)
      वापस CURLE_ABORTED_BY_CALLBACK;
  }
  *done = TRUE;
  वापस CURLE_OK;
}

काष्ठा Curl_cftype Curl_cft_tcp_accept = {
  "TCP-ACCEPT",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_accept_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_तथ्य_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_conn_tcp_listen_set(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode परिणाम;
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा cf_socket_ctx *ctx = शून्य;

  /* replace any existing */
  Curl_conn_cf_discard_all(तथ्य, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  परिणाम = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  अगर(परिणाम)
    जाओ out;
  Curl_conn_cf_add(तथ्य, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(तथ्य);
  conn->sock[sockindex] = ctx->sock;
  set_local_ip(cf, तथ्य);
  CURL_TRC_CF(तथ्य, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_port);

out:
  अगर(परिणाम) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  वापस परिणाम;
}

bool Curl_conn_is_tcp_listen(काष्ठा Curl_easy *तथ्य,
                             int8_t sockindex)
{
  काष्ठा Curl_cfilter *cf = तथ्य->conn->cfilter[sockindex];
  जबतक(cf) {
    अगर(cf->cft == &Curl_cft_tcp_accept)
      वापस TRUE;
    cf = cf->अग्र;
  }
  वापस FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
अटल bool cf_is_socket(काष्ठा Curl_cfilter *cf)
{
  वापस cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             curl_socket_t *psock,
                             स्थिर काष्ठा Curl_sockaddr_ex **paddr,
                             काष्ठा ip_quadruple *pip)
{
  (व्योम)तथ्य;
  अगर(cf_is_socket(cf) && cf->ctx) {
    काष्ठा cf_socket_ctx *ctx = cf->ctx;

    अगर(psock)
      *psock = ctx->sock;
    अगर(paddr)
      *paddr = &ctx->addr;
    अगर(pip)
      *pip = ctx->ip;
    वापस CURLE_OK;
  }
  वापस CURLE_FAILED_INIT;
}
