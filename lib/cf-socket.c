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

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h> /* <netinet/tcp.h> may need it */
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_UN_H
#ضمن <sys/un.h> /* for sockaddr_un */
#پایان_اگر
#اگر_تعریف_شده HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETINET_UDP_H
#ضمن <netinet/udp.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETINET_IP_H
#ضمن <netinet/ip.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_IFADDRS_H
#ضمن <ifaddrs.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NET_IF_H
#ضمن <net/if.h>
#پایان_اگر
#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

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
#اگر_تعریف_شده _WIN32
#ضمن <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#پایان_اگر

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(بنية sockaddr *sa, curl_socklen_t salen,
                                  حرف *addr, uint16_t *پورت);
UNITTEST CURLcode sockaddr2string(بنية sockaddr *sa, curl_socklen_t salen,
                                  حرف *addr, uint16_t *پورت)
{
  CURLcode نتیجه;
  بنية sockaddr_in *si = عدم;
#اگر_تعریف_شده USE_IPV6
  بنية sockaddr_in6 *si6 = عدم;
#پایان_اگر
#اگر_تعریف_شده USE_UNIX_SOCKETS
  بنية sockaddr_un *su = عدم;
#وإلا
  (فراغ)salen;
#پایان_اگر

  بدل(sa->sa_family) {
  حالة AF_INET:
    si = (بنية sockaddr_in *)(فراغ *)sa;
    نتیجه = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    إذا(!نتیجه) {
      *پورت = ntohs(si->sin_port);
      أرجع نتیجه;
    }
    اكسر;
#اگر_تعریف_شده USE_IPV6
  حالة AF_INET6:
    si6 = (بنية sockaddr_in6 *)(فراغ *)sa;
    نتیجه = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    إذا(!نتیجه) {
      *پورت = ntohs(si6->sin6_port);
      أرجع نتیجه;
    }
    اكسر;
#پایان_اگر
#اگر_تعریف_شده USE_UNIX_SOCKETS
  حالة AF_UNIX:
    إذا(salen > (curl_socklen_t)حجم(CURL_SA_FAMILY_T)) {
      su = (بنية sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_مسیر);
    }
    وإلا
      addr[0] = 0; /* socket with no name */
    *پورت = 0;
    أرجع CURLE_OK;
#پایان_اگر
  افتراضي:
    نتیجه = CURLE_UNSUPPORTED_PROTOCOL;
    اكسر;
  }

  addr[0] = '\0';
  *پورت = 0;
  أرجع نتیجه;
}

ساكن فراغ tcpnodelay(بنية Curl_cfilter *cf,
                       بنية Curl_easy *داده,
                       curl_socket_t sockfd)
{
#إذا defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  صحيح level = IPPROTO_TCP;
  VERBOSE(حرف میانگیر[STRERROR_LEN]);

  إذا(setsockopt(sockfd, level, TCP_NODELAY, (فراغ *)&onoff, حجم(onoff)) < 0)
    CURL_TRC_CF(داده, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, میانگیر, حجم(میانگیر)));
#وإلا
  (فراغ)cf;
  (فراغ)داده;
  (فراغ)sockfd;
#پایان_اگر
}

#إذا defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_آستانه) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_آستانه)
#إذا defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
ساكن فراغ alive_unit(صحيح *x)
{
  /* make sure this doesn't wrap */
  إذا(*x < (بیشترین_صحیح / 1000))
    *x *= 1000;
  وإلا
    /* this is still almost 25 days */
    *x = بیشترین_صحیح;
}
#وإلا
#تعریف_کن alive_unit(x)
#پایان_اگر
#پایان_اگر

ساكن فراغ tcpkeepalive(بنية Curl_cfilter *cf,
                         بنية Curl_easy *داده,
                         curl_socket_t sockfd)
{
  صحيح optval = داده->مجموعه.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  إذا(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (فراغ *)&optval, حجم(optval)) < 0) {
    CURL_TRC_CF(داده, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  وإلا {
#اگر_تعریف_شده USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    إذا(curlx_verify_windows_نسخه(10, 0, 16299, PLATFORM_WINNT,
                                    نسخه_GREATER_THAN_EQUAL)) {
      CURL_TRC_CF(داده, cf, "Set TCP_KEEP* on fd=%" FMT_SOCKET_T, sockfd);
      optval = curlx_sltosi(داده->مجموعه.tcp_keepidle);
/* Offered by mingw-w64 v12+, MS SDK 6.0A/VS2008+ */
#اگر_تعریف_نشده TCP_KEEPALIVE
#تعریف_کن TCP_KEEPALIVE 3
#پایان_اگر
/* Offered by mingw-w64 v12+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#اگر_تعریف_نشده TCP_KEEPCNT
#تعریف_کن TCP_KEEPCNT 16
#پایان_اگر
/* Offered by mingw-w64 v12+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#اگر_تعریف_نشده TCP_KEEPIDLE
#تعریف_کن TCP_KEEPIDLE TCP_KEEPALIVE
#پایان_اگر
#اگر_تعریف_نشده TCP_KEEPINTVL
#تعریف_کن TCP_KEEPINTVL 17
#پایان_اگر
      إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (ثابت حرف *)&optval, حجم(optval)) < 0) {
        CURL_TRC_CF(داده, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(داده->مجموعه.tcp_keepintvl);
      إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (ثابت حرف *)&optval, حجم(optval)) < 0) {
        CURL_TRC_CF(داده, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(داده->مجموعه.tcp_keepcnt);
      إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (ثابت حرف *)&optval, حجم(optval)) < 0) {
        CURL_TRC_CF(داده, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    وإلا {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#اگر_تعریف_نشده SIO_KEEPALIVE_VALS
#تعریف_کن SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      بنية tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#پایان_اگر
      بنية tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(داده->مجموعه.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(داده->مجموعه.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      إذا(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, حجم(vals),
                  عدم, 0, &dummy, عدم, عدم) != 0) {
        CURL_TRC_CF(داده, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#وإلا /* !USE_WINSOCK */
#اگر_تعریف_شده TCP_KEEPIDLE
    optval = curlx_sltosi(داده->مجموعه.tcp_keepidle);
    alive_unit(&optval);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (فراغ *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(داده, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وإلاإذا defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(داده->مجموعه.tcp_keepidle);
    alive_unit(&optval);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (فراغ *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(داده, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وإلاإذا defined(TCP_KEEPALIVE_آستانه)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(داده->مجموعه.tcp_keepidle);
    alive_unit(&optval);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_آستانه,
                  (فراغ *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(داده, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#پایان_اگر
#اگر_تعریف_شده TCP_KEEPINTVL
    optval = curlx_sltosi(داده->مجموعه.tcp_keepintvl);
    alive_unit(&optval);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (فراغ *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(داده, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وإلاإذا defined(TCP_KEEPALIVE_ABORT_آستانه)
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
      صحيح keepcnt = curlx_sltosi(داده->مجموعه.tcp_keepcnt);
      صحيح keepintvl = curlx_sltosi(داده->مجموعه.tcp_keepintvl);

      إذا(keepcnt > 0 && keepintvl > (بیشترین_صحیح / keepcnt))
        optval = بیشترین_صحیح;
      وإلا
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_آستانه,
                  (فراغ *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(داده, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#پایان_اگر
#اگر_تعریف_شده TCP_KEEPCNT
    optval = curlx_sltosi(داده->مجموعه.tcp_keepcnt);
    إذا(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (فراغ *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(داده, cf, "Failed to set TCP_KEEPCNT on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#پایان_اگر
#پایان_اگر /* USE_WINSOCK */
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
  addr->addrlen = (بیعلامت صحيح)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= حجم(addr->curl_sa_addrbuf));
  إذا(addr->addrlen > حجم(addr->curl_sa_addrbuf))
    أرجع CURLE_TOO_LARGE;

  کپی_حافظه(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  أرجع CURLE_OK;
}

#اگر_تعریف_شده USE_SO_NOSIGPIPE
صحيح Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  صحيح onoff = 1;
  أرجع setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (فراغ *)&onoff, حجم(onoff));
}
#پایان_اگر /* USE_SO_NOSIGPIPE */

#إذا defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ساكن uint32_t get_scope_id(بنية Curl_easy *داده,
                             بنية sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  إذا(داده->conn->scope_id)
    أرجع داده->conn->scope_id;
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
      لكل(ifa = ifa_list; ifa; ifa = ifa->ifa_بعدی) {
        إذا(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          بنية sockaddr_in6 *s6 = (فراغ *)ifa->ifa_addr;
          إذا(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(داده,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (بیعلامت دراز)scope_id);
            اكسر;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#پایان_اگر /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  أرجع scope_id;
}
#پایان_اگر

ساكن CURLcode socket_open(بنية Curl_easy *داده,
                            بنية Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  حرف errbuf[STRERROR_LEN];

#اگر_تعریف_شده SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#پایان_اگر

  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);
  إذا(داده->مجموعه.fopensocket) {
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
    CURL_CBAPI_شروع(&guard, داده, easy_fopensocket);
    *sockfd = داده->مجموعه.fopensocket(داده->مجموعه.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (بنية curl_sockaddr *)addr);
    CURL_CBAPI_انتها(&guard);
  }
  وإلا {
    /* opensocket callback not set, so create the socket now */
#اگر_تعریف_شده DEBUGBUILD
    إذا((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(داده, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      أرجع CURLE_COULDNT_CONNECT;
    }
#پایان_اگر
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    إذا((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      أرجع CURLE_بیرون_از_حافظه;
  }

  إذا(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(داده, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    أرجع CURLE_COULDNT_CONNECT;
  }

#اگر_تعریف_شده USE_SO_NOSIGPIPE
  إذا(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(داده, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    أرجع CURLE_COULDNT_CONNECT;
  }
#پایان_اگر /* USE_SO_NOSIGPIPE */

#إذا defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  إذا(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(داده, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    أرجع CURLE_COULDNT_CONNECT;
  }
#پایان_اگر

#إذا defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  إذا(addr->family == AF_INET6) {
    بنية sockaddr_in6 * ثابت sa6 = (فراغ *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(داده, sa6);
  }
#پایان_اگر
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
CURLcode Curl_socket_open(بنية Curl_easy *داده,
                          ثابت بنية Curl_addrinfo *ai,
                          بنية Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  بنية Curl_sockaddr_ex dummy;
  CURLcode نتیجه;

  إذا(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  نتیجه = Curl_socket_addr_from_ai(addr, ai, transport);
  إذا(نتیجه)
    أرجع نتیجه;

  أرجع socket_open(داده, addr, sockfd);
}

ساكن صحيح socket_close(بنية Curl_easy *داده, بنية connectdata *conn,
                        صحيح use_callback, curl_socket_t sock)
{
  إذا(sock == CURL_SOCKET_BAD)
    أرجع 0;

  إذا(use_callback && conn && conn->fclosesocket) {
    بنية Curl_mapi_guard guard;
    صحيح rc;
    Curl_multi_will_close(داده, sock);
    CURL_CBAPI_شروع(&guard, داده, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_انتها(&guard);
    أرجع rc;
  }

  إذا(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(داده, sock);

  sclose(sock);

  أرجع 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
صحيح Curl_socket_close(بنية Curl_easy *داده, بنية connectdata *conn,
                      curl_socket_t sock)
{
  أرجع socket_close(داده, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(ثابت حرف *درون_داد,
                              حرف **dev, حرف **iface, حرف **host)
{
  ساكن ثابت حرف if_پیشوند[] = "if!";
  ساكن ثابت حرف host_پیشوند[] = "host!";
  ساكن ثابت حرف if_host_پیشوند[] = "ifhost!";
  size_t len;

  len = طول_رشته(درون_داد);
  إذا(len > 512)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  إذا(dev)
    *dev = عدم;
  إذا(iface)
    *iface = عدم;
  إذا(host)
    *host = عدم;

  إذا(!مقایسه_رشته(if_پیشوند, درون_داد, طول_رشته(if_پیشوند))) {
    درون_داد += طول_رشته(if_پیشوند);
    إذا(!*درون_داد)
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    إذا(iface) {
      *iface = curlx_memdup0(درون_داد, len - طول_رشته(if_پیشوند));
      أرجع *iface ? CURLE_OK : CURLE_بیرون_از_حافظه;
    }
    أرجع CURLE_OK;
  }
  وإلا إذا(!مقایسه_رشته(host_پیشوند, درون_داد, طول_رشته(host_پیشوند))) {
    درون_داد += طول_رشته(host_پیشوند);
    إذا(!*درون_داد)
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    إذا(host) {
      *host = curlx_memdup0(درون_داد, len - طول_رشته(host_پیشوند));
      أرجع *host ? CURLE_OK : CURLE_بیرون_از_حافظه;
    }
    أرجع CURLE_OK;
  }
  وإلا إذا(!مقایسه_رشته(if_host_پیشوند, درون_داد, طول_رشته(if_host_پیشوند))) {
    ثابت حرف *host_part;
    درون_داد += طول_رشته(if_host_پیشوند);
    len -= طول_رشته(if_host_پیشوند);
    host_part = memchr(درون_داد, '!', len);
    إذا(!host_part || !*(host_part + 1))
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    إذا(iface) {
      *iface = curlx_memdup0(درون_داد, host_part - درون_داد);
      إذا(!*iface)
        أرجع CURLE_بیرون_از_حافظه;
    }
    ++host_part;
    إذا(host) {
      *host = curlx_memdup0(host_part, len - (host_part - درون_داد));
      إذا(!*host) {
        إذا(iface)
          curlx_safefree(*iface);
        أرجع CURLE_بیرون_از_حافظه;
      }
    }
    أرجع CURLE_OK;
  }

  إذا(!*درون_داد)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  إذا(dev) {
    *dev = curlx_memdup0(درون_داد, len);
    أرجع *dev ? CURLE_OK : CURLE_بیرون_از_حافظه;
  }
  أرجع CURLE_OK;
}

#اگر_تعریف_نشده CURL_DISABLE_BINDLOCAL
ساكن CURLcode bindlocal(بنية Curl_easy *داده, بنية connectdata *conn,
                          curl_socket_t sockfd, صحيح af, بیعلامت صحيح scope,
                          uint8_t transport)
{
  بنية Curl_sockaddr_storage sa;
  بنية sockaddr *sock = (بنية sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  بنية sockaddr_in *si4 = (بنية sockaddr_in *)&sa;
#اگر_تعریف_شده USE_IPV6
  بنية sockaddr_in6 *si6 = (بنية sockaddr_in6 *)&sa;
#پایان_اگر
  بنية Curl_dns_مدخل *h = عدم;
  بیعلامت کوتاه پورت = داده->مجموعه.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  صحيح portnum = داده->مجموعه.localportrange;
  حرف *dev_درون = عدم, *iface_درون = عدم, *host_درون = عدم;
  ثابت حرف *iface = عدم, *host = عدم;
  صحيح sockerr;
#اگر_تعریف_شده IP_BIND_ADDRESS_NO_PORT
  صحيح on = 1;
#پایان_اگر
  CURLcode نتیجه = CURLE_OK;

#اگر_تعریف_نشده USE_IPV6
  (فراغ)scope;
#پایان_اگر
  إذا(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    نتیجه = Curl_parse_interface(conn->interface_opts,
                                  &dev_درون, &iface_درون, &host_درون);
    إذا(نتیجه)
      اذهب بیرون;
    /* This is how we use the values */
    iface = iface_درون ? iface_درون : dev_درون;
    host = host_درون ? host_درون : dev_درون;
  }

  إذا(!iface && !host && !پورت)
    اذهب بیرون;

  إذا(iface && (طول_رشته(iface) >= 255)) {
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    اذهب بیرون;
  }
  memset(&sa, 0, حجم(بنية Curl_sockaddr_storage));

  إذا(iface || host) {
    حرف myhost[256] = "";
    صحيح انجامشده = 0; /* -1 for error, 1 for address found */
    if2ip_نتیجه_t if2ip_نتیجه = IF2_IP_NOT_یافت;

#اگر_تعریف_شده SO_BINDTODEVICE
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
                  iface, (curl_socklen_t)طول_رشته(iface) + 1) == 0 &&
       /* This is often "errno 1, error: Operation not permitted" if you are
        * not running as root or another suitable privileged user. If it
        * succeeds it means the parameter was a valid interface and not an IP
        * address. Return immediately.
        */
       !host_درون) {
      infof(داده, "socket successfully bound to interface '%s'", iface);
      نتیجه = CURLE_OK;
      اذهب بیرون;
    }
#پایان_اگر
    إذا(!host_درون) {
      /* Discover IP from input device, then bind to it */
      if2ip_نتیجه = Curl_if2ip(af,
#اگر_تعریف_شده USE_IPV6
                                scope, conn->scope_id,
#پایان_اگر
                                iface, myhost, حجم(myhost));
    }
    بدل(if2ip_نتیجه) {
    حالة IF2_IP_NOT_یافت:
      إذا(iface_درون && !host_درون) {
        /* Do not fall back to treating it as a hostname */
        حرف میانگیر[STRERROR_LEN];
        داده->state.os_errno = sockerr = SOCKERRNO;
        failf(داده, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));
        نتیجه = CURLE_INTERFACE_FAILED;
        اذهب بیرون;
      }
      اكسر;
    حالة IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      نتیجه = CURLE_UNSUPPORTED_PROTOCOL;
      اذهب بیرون;
    حالة IF2_IP_یافت:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(داده, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      انجامشده = 1;
      اكسر;
    }
    إذا(!iface_درون || host_درون) {
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
#اگر_تعریف_شده USE_IPV6
      إذا(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#پایان_اگر

      (فراغ)Curl_resolv_blocking(داده, dns_queries, host, 80, transport, &h);
      إذا(h) {
        صحيح h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_آدرس(h->addr, myhost, حجم(myhost));
        infof(داده, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_مدخل_unlink(داده, &h); /* this will NULL, potential free h */
        إذا(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          نتیجه = CURLE_UNSUPPORTED_PROTOCOL;
          اذهب بیرون;
        }
        انجامشده = 1;
      }
      وإلا {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        انجامشده = -1;
      }
    }

    إذا(انجامشده > 0) {
#اگر_تعریف_شده USE_IPV6
      /* IPv6 address */
      إذا(af == AF_INET6) {
#اگر_تعریف_شده HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        حرف *scope_ptr = strchr(myhost, '%');
        إذا(scope_ptr)
          *(scope_ptr++) = '\0';
#پایان_اگر
        إذا(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(پورت);
#اگر_تعریف_شده HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          إذا(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            إذا(curlx_str_عدد((ثابت حرف **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              نتیجه = CURLE_UNSUPPORTED_PROTOCOL;
              اذهب بیرون;
            }
            si6->sin6_scope_id = (بیعلامت صحيح)scope_id;
          }
#پایان_اگر
        }
        sizeof_sa = حجم(بنية sockaddr_in6);
      }
      وإلا
#پایان_اگر
      /* IPv4 address */
      إذا((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(پورت);
        sizeof_sa = حجم(بنية sockaddr_in);
      }
    }

    إذا(انجامشده < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      حرف میانگیر[STRERROR_LEN];
      داده->state.errorbuf = FALSE;
      داده->state.os_errno = sockerr = SOCKERRNO;
      failf(داده, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));
      نتیجه = CURLE_INTERFACE_FAILED;
      اذهب بیرون;
    }
  }
  وإلا {
    /* no device was given, prepare sa to match af's needs */
#اگر_تعریف_شده USE_IPV6
    إذا(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(پورت);
      sizeof_sa = حجم(بنية sockaddr_in6);
    }
    وإلا
#پایان_اگر
    إذا(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(پورت);
      sizeof_sa = حجم(بنية sockaddr_in);
    }
  }
#اگر_تعریف_شده IP_BIND_ADDRESS_NO_PORT
  (فراغ)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, حجم(on));
#پایان_اگر
  لكل(;;) {
    إذا(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(داده, "Local port: %hu", پورت);
      conn->بیتها.bound = TRUE;
      نتیجه = CURLE_OK;
      اذهب بیرون;
    }

    إذا(--portnum > 0) {
      پورت++; /* try next port */
      إذا(پورت == 0)
        اكسر;
      infof(داده, "Bind to local port %d failed, trying next", پورت - 1);
      /* We reuse/clobber the port variable here below */
      إذا(sock->sa_family == AF_INET)
        si4->sin_port = htons(پورت);
#اگر_تعریف_شده USE_IPV6
      وإلا
        si6->sin6_port = htons(پورت);
#پایان_اگر
    }
    وإلا
      اكسر;
  }
  {
    حرف میانگیر[STRERROR_LEN];
    داده->state.os_errno = sockerr = SOCKERRNO;
    failf(داده, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));
  }

  نتیجه = CURLE_INTERFACE_FAILED;

بیرون:
  curlx_free(dev_درون);
  curlx_free(iface_درون);
  curlx_free(host_درون);
  أرجع نتیجه;
}
#پایان_اگر

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
ساكن bool verifyconnect(curl_socket_t sockfd, صحيح *psockerr)
{
  bool rc = TRUE;
#اگر_تعریف_شده SO_ERROR
  صحيح sockerr = 0;
  curl_socklen_t err_اندازه = حجم(sockerr);

#اگر_تعریف_شده _WIN32
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
#پایان_اگر

  إذا(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (فراغ *)&sockerr, &err_اندازه))
    sockerr = SOCKERRNO;
#إذا defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  إذا(EBADIOCTL == sockerr) {
    مجموعه_SOCKERRNO(0);
    sockerr = 0;
  }
#پایان_اگر
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
#پایان_اگر
  أرجع rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
ساكن CURLcode socket_connect_نتیجه(بنية Curl_easy *داده,
                                      ثابت حرف *ipaddress, صحيح sockerr)
{
  إذا(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    أرجع CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(حرف میانگیر[STRERROR_LEN]);
    infof(داده, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));
    NOVERBOSE((فراغ)ipaddress);
  }
  داده->state.os_errno = sockerr;
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
  بنية curltime اولی_byte_at;     /* when first byte was recvd */
#اگر_تعریف_شده USE_WINSOCK
  بنية curltime آخرین_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_اندازه;                     /* the last set SO_SNDBUF size */
#پایان_اگر
  صحيح sockerr;                       /* socket error of last failure or 0 */
#اگر_تعریف_شده DEBUGBUILD
  صحيح connect_wait_ms;               /* ms to wait after 1st connect() */
  صحيح wblock_percent;                /* percent of writes doing EAGAIN */
  صحيح wpartial_percent;              /* percent of bytes written in send */
  صحيح rblock_percent;                /* percent of reads doing EAGAIN */
  size_t recv_max;                   /* max enforced read size */
#پایان_اگر
  uint8_t transport;
  بیت(got_اولی_byte);               /* if first byte was received */
  بیت(listening);                    /* socket is listening */
  بیت(accepted);                     /* socket was accepted, not connected */
  بیت(sock_connected);               /* socket is "connected", e.g. in UDP */
  بیت(فعال);
  بیت(stats_reported);
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

#اگر_تعریف_شده DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_SOCK_WBLOCK");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, 100))
        ctx->wblock_percent = (صحيح)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, 100))
        ctx->wpartial_percent = (صحيح)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, 100))
        ctx->rblock_percent = (صحيح)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, بیشترین_صحیح))
        ctx->connect_wait_ms = (صحيح)l;
    }
  }
#پایان_اگر

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
                                   بنية Curl_easy *داده,
                                   bool *انجامشده)
{
  إذا(cf->connected) {
    بنية cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(داده, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    إذا(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      بیعلامت حرف buf[1024];
      (فراغ)sread(ctx->sock, buf, حجم(buf));
    }
  }
  *انجامشده = TRUE;
  أرجع CURLE_OK;
}

ساكن فراغ cf_socket_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(داده, cf, "destroy");
  إذا(ctx) {
    إذا(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(داده, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      إذا(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(داده, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

ساكن فراغ مجموعه_local_ip(بنية Curl_cfilter *cf,
                         بنية Curl_easy *داده)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_پورت = 0;

#اگر_تعریف_شده HAVE_GETSOCKNAME
  إذا((ctx->sock != CURL_SOCKET_BAD) &&
     !(داده->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    بنية Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = حجم(بنية Curl_sockaddr_storage);

    memset(&ssloc, 0, حجم(ssloc));
    إذا(getsockname(ctx->sock, (بنية sockaddr *)&ssloc, &slen)) {
      VERBOSE(حرف میانگیر[STRERROR_LEN]);
      VERBOSE(صحيح sockerr = SOCKERRNO);
      infof(داده, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));
    }
    وإلا {
      CURLcode نتیجه = sockaddr2string((بنية sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_پورت);
      إذا(نتیجه)
        infof(داده, "ssloc inet_ntop() failed with %d", (صحيح)نتیجه);
    }
  }
#وإلا
  (فراغ)داده;
#پایان_اگر
}

ساكن CURLcode مجموعه_remote_ip(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  CURLcode نتیجه;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  نتیجه = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_پورت);
  إذا(نتیجه) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(داده, "curl_sa_addr inet_ntop() failed with %d", (صحيح)نتیجه);
    أرجع CURLE_FAILED_INIT;
  }
  أرجع CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
ساكن صحيح cf_socktype(صحيح x)
{
#اگر_تعریف_شده SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#پایان_اگر
#اگر_تعریف_شده CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#پایان_اگر
  أرجع x;
}

#اگر_تعریف_شده _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#اگر_تعریف_نشده SIO_TCP_INITIAL_RTO
#تعریف_کن SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#تعریف_کن TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#پایان_اگر /* SIO_TCP_INITIAL_RTO */

#اگر_تعریف_نشده TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#تعریف_کن TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#پایان_اگر

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
    DWORD بایتها = 0;
    memset(&rto, 0, حجم(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    إذا(curlx_verify_windows_نسخه(10, 0, 16299, PLATFORM_WINNT,
                                    نسخه_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    وإلا
      rto.MaxSynRetransmissions = 1;
    (فراغ)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, حجم(rto),
                   عدم, 0, &بایتها, عدم, عدم);
  }
}
#وإلا
#تعریف_کن tcplocalhost(x, y)
#پایان_اگر /* _WIN32 */

ساكن CURLcode cf_socket_open(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  صحيح error = 0;
  bool isconnected = FALSE;
  CURLcode نتیجه = CURLE_COULDNT_CONNECT;
  bool is_tcp;

  DEBUGASSERT(ctx->sock == CURL_SOCKET_BAD);
  ctx->started_at = *Curl_pgrs_now(داده);
#اگر_تعریف_شده CURL_USE_SOCK_NONBLOCK
  /* Do not tuck SOCK_NONBLOCK into socktype when opensocket callback is set
   * because we would not know how socktype is about to be used in the
   * callback, SOCK_NONBLOCK might get factored out before calling socket().
   */
  إذا(!داده->مجموعه.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#پایان_اگر
  نتیجه = socket_open(داده, &ctx->addr, &ctx->sock);
#اگر_تعریف_شده CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  إذا(!داده->مجموعه.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#پایان_اگر
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = مجموعه_remote_ip(cf, داده);
  إذا(نتیجه)
    اذهب بیرون;

#اگر_تعریف_شده USE_IPV6
  إذا(ctx->addr.family == AF_INET6) {
#اگر_تعریف_شده USE_WINSOCK
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
#پایان_اگر
#اگر_تعریف_شده HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      بنية sockaddr_in6 *sa6 = (فراغ *)&ctx->addr.curl_sa_addr;
      إذا(sa6->sin6_scope_id)
        infof(داده, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_پورت,
              (بیعلامت دراز)sa6->sin6_scope_id);
      وإلا
#پایان_اگر
        infof(داده, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_پورت);
#اگر_تعریف_شده HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#پایان_اگر
  }
  وإلا
#پایان_اگر
    infof(داده, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_پورت);

#اگر_تعریف_شده USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#وإلا
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#پایان_اگر
  إذا(is_tcp) {
    إذا(داده->مجموعه.tcp_nodelay)
      tcpnodelay(cf, داده, ctx->sock);

    إذا(داده->مجموعه.tcp_keepalive)
      tcpkeepalive(cf, داده, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  إذا(داده->مجموعه.fsockopt) {
    /* activate callback for setting socket options */
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروع(&guard, داده, easy_fsockopt);
    error = داده->مجموعه.fsockopt(داده->مجموعه.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_انتها(&guard);

    إذا(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    وإلا إذا(error) {
      نتیجه = CURLE_ABORTED_BY_CALLBACK;
      اذهب بیرون;
    }
  }

#اگر_تعریف_نشده CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  إذا(ctx->addr.family == AF_INET
#اگر_تعریف_شده USE_IPV6
     || ctx->addr.family == AF_INET6
#پایان_اگر
    ) {
    نتیجه = bindlocal(داده, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    إذا(نتیجه) {
      إذا(نتیجه == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        نتیجه = CURLE_COULDNT_CONNECT;
      }
      اذهب بیرون;
    }
  }
#پایان_اگر

#اگر_تعریف_نشده CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  إذا(error < 0) {
    نتیجه = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    اذهب بیرون;
  }
#وإلا
  إذا(داده->مجموعه.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    إذا(error < 0) {
      نتیجه = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      اذهب بیرون;
    }
  }
#پایان_اگر
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
بیرون:
  إذا(نتیجه) {
    إذا(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(داده, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  وإلا إذا(isconnected) {
    مجموعه_local_ip(cf, داده);
    ctx->connected_at = *Curl_pgrs_now(داده);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(داده, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (صحيح)نتیجه, ctx->sock);
  أرجع نتیجه;
}

ساكن صحيح do_connect(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                      bool is_tcp_fastopen)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
#اگر_تعریف_شده TCP_FASTOPEN_CONNECT
  صحيح optval = 1;
#پایان_اگر
  صحيح rc = -1;

  (فراغ)داده;
  إذا(is_tcp_fastopen) {
#اگر_تعریف_شده CONNECT_داده_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_موجود
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
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_داده_IDEMPOTENT,
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
      CURL_TRC_CF(داده, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#وإلاإذا defined(MSG_FASTOPEN) /* old Linux */
    إذا(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    وإلا
      rc = 0; /* Do nothing */
#پایان_اگر
  }
  وإلا {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  أرجع rc;
}

ساكن CURLcode cf_tcp_connect(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               bool *انجامشده)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_COULDNT_CONNECT;
  صحيح rc = 0;

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  *انجامشده = FALSE; /* a negative world view is best */
  إذا(ctx->sock == CURL_SOCKET_BAD) {
    صحيح sockerr;

    نتیجه = cf_socket_open(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;

    إذا(cf->connected) {
      *انجامشده = TRUE;
      أرجع CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, داده, (bool)cf->conn->بیتها.tcp_fastopen);
    sockerr = SOCKERRNO;
    مجموعه_local_ip(cf, داده);
    CURL_TRC_CF(داده, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_پورت);
#اگر_تعریف_شده DEBUGBUILD
    إذا(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(داده, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#پایان_اگر
    إذا(rc == -1) {
      ctx->sockerr = sockerr;
      نتیجه = socket_connect_نتیجه(داده, ctx->ip.remote_ip, sockerr);
      اذهب بیرون;
    }
  }

#اگر_تعریف_شده mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (فراغ)verifyconnect(ctx->sock, عدم);
#پایان_اگر
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  إذا(rc == 0) { /* no connection yet */
    CURL_TRC_CF(داده, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    أرجع CURLE_OK;
  }
  وإلا إذا(rc == CURL_CSELECT_بیرون ||
#اگر_تعریف_شده __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#پایان_اگر
          cf->conn->بیتها.tcp_fastopen) {
    إذا(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(داده);
      مجموعه_local_ip(cf, داده);
      *انجامشده = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(داده, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      أرجع CURLE_OK;
    }
    نتیجه = CURLE_COULDNT_CONNECT;
  }
  وإلا إذا(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(داده, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (فراغ)verifyconnect(ctx->sock, &ctx->sockerr);
    نتیجه = CURLE_COULDNT_CONNECT;
  }

بیرون:
  إذا(نتیجه) {
    VERBOSE(حرف میانگیر[STRERROR_LEN]);
    مجموعه_local_ip(cf, داده);
    إذا(ctx->sockerr) {
      داده->state.os_errno = ctx->sockerr;
      مجموعه_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, میانگیر, حجم(میانگیر)));
    }
    وإلا {
      VERBOSE(curlx_strcopy(میانگیر, حجم(میانگیر), STRCONST("peer closed")));
    }
    إذا(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(داده, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(داده, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_پورت,
          ctx->ip.local_ip, ctx->ip.local_پورت,
          curlx_strerror(ctx->sockerr, میانگیر, حجم(میانگیر)));
    *انجامشده = FALSE;
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_socket_adjust_pollset(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده,
                                         بنية easy_pollset *ps)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  إذا(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    إذا(ctx->listening) {
      نتیجه = Curl_pollset_مجموعه_درون_only(داده, ps, ctx->sock);
      CURL_TRC_CF(داده, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    وإلا إذا(!cf->connected) {
      نتیجه = Curl_pollset_مجموعه_بیرون_only(داده, ps, ctx->sock);
      CURL_TRC_CF(داده, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    وإلا إذا(!ctx->فعال) {
      نتیجه = Curl_pollset_جمع_کن_درون(داده, ps, ctx->sock);
      CURL_TRC_CF(داده, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  أرجع نتیجه;
}

#اگر_تعریف_شده USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#اگر_تعریف_نشده SIO_IDEAL_SEND_BACKLOG_QUERY
#تعریف_کن SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#پایان_اگر

ساكن فراغ win_update_sndbuf_اندازه(بنية Curl_easy *داده,
                                   بنية cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  إذا(curlx_ptimediff_ms(Curl_pgrs_now(داده),
                        &ctx->آخرین_sndbuf_query_at) > 1000) {
    إذا(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, حجم(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_اندازه &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (ثابت حرف *)&ideal, حجم(ideal))) {
      ctx->sndbuf_اندازه = ideal;
    }
    ctx->آخرین_sndbuf_query_at = *Curl_pgrs_now(داده);
  }
}

#پایان_اگر /* USE_WINSOCK */

ساكن CURLcode cf_socket_send(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode نتیجه = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (فراغ)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#اگر_تعریف_شده DEBUGBUILD
  /* simulate network blocking/partial writes */
  إذا(ctx->wblock_percent > 0) {
    بیعلامت حرف c = 0;
    Curl_rand_بایتها(داده, FALSE, &c, 1);
    إذا(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(داده, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      أرجع CURLE_AGAIN;
    }
  }
  إذا(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    إذا(!len)
      len = 1;
    CURL_TRC_CF(داده, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#پایان_اگر

#إذا defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  إذا(cf->conn->بیتها.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->بیتها.tcp_fastopen = FALSE;
  }
  وإلا
#پایان_اگر
    rv = swrite(ctx->sock, buf, len);

  إذا(!curlx_sztouz(rv, pnwritten)) {
    صحيح sockerr = SOCKERRNO;
    إذا(SOCK_EAGAIN(sockerr)
#اگر_تعریف_نشده USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#پایان_اگر
      ) {
      نتیجه = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    وإلا {
      حرف میانگیر[STRERROR_LEN];
      failf(داده, "Send failure: %s",
            curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));
      داده->state.os_errno = sockerr;
      نتیجه = CURLE_SEND_ERROR;
    }
  }

#اگر_تعریف_شده USE_WINSOCK
  إذا(!نتیجه)
    win_update_sndbuf_اندازه(داده, ctx);
#پایان_اگر

  CURL_TRC_CF(داده, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (صحيح)نتیجه, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  أرجع نتیجه;
}

ساكن CURLcode cf_socket_recv(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                               حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#اگر_تعریف_شده DEBUGBUILD
  /* simulate network blocking/partial reads */
  إذا(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    بیعلامت حرف c = 0;
    Curl_rand(داده, &c, 1);
    إذا(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(داده, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      أرجع CURLE_AGAIN;
    }
  }
  إذا(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(داده, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#پایان_اگر

  rv = sread(ctx->sock, buf, len);

  إذا(!curlx_sztouz(rv, pnread)) {
    صحيح sockerr = SOCKERRNO;
    إذا(SOCK_EAGAIN(sockerr)
#اگر_تعریف_نشده USE_WINSOCK
       || (sockerr == SOCKEINTR)
#پایان_اگر
      ) {
      نتیجه = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    وإلا {
      حرف میانگیر[STRERROR_LEN];
      failf(داده, "Recv failure: %s",
            curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));
      داده->state.os_errno = sockerr;
      نتیجه = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(داده, cf, "recv(len=%zu) -> %d, %zu", len, (صحيح)نتیجه, *pnread);
  إذا(!نتیجه && !ctx->got_اولی_byte) {
    ctx->اولی_byte_at = *Curl_pgrs_now(داده);
    ctx->got_اولی_byte = TRUE;
  }
  أرجع نتیجه;
}

ساكن فراغ cf_socket_update_داده(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده)
{
  /* Update the IP info held in the transfer, if we have that. */
  إذا(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    بنية cf_socket_ctx *ctx = cf->ctx;
    داده->info.primary = ctx->ip;
  }
}

ساكن فراغ cf_socket_فعال(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  مجموعه_local_ip(cf, داده);
#اگر_تعریف_شده USE_IPV6
  إذا(cf->sockindex == FIRSTSOCKET)
    cf->conn->بیتها.ipv6 = (ctx->addr.family == AF_INET6);
#پایان_اگر
  ctx->فعال = TRUE;
}

ساكن CURLcode cf_socket_cntrl(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_socket_ctx *ctx = cf->ctx;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_فعال(cf, داده);
    cf_socket_update_داده(cf, داده);
    اكسر;
  حالة CF_CTRL_داده_راهاندازی:
    cf_socket_update_داده(cf, داده);
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
        إذا(ctx->got_اولی_byte)
          ts = &ctx->اولی_byte_at;
        اكسر;
      افتراضي:
        ts = &ctx->connected_at;
        اكسر;
      }
      إذا(ts) {
        Curl_pgrs_زمان_Was(داده, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  أرجع CURLE_OK;
}

ساكن bool cf_socket_conn_is_alive(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    bool *درون_داد_pending)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  بنية pollfd pfd[1];
  صحيح r;

  *درون_داد_pending = FALSE;

  إذا(!ctx || ctx->sock == CURL_SOCKET_BAD)
    أرجع FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  إذا(r < 0) {
    CURL_TRC_CF(داده, cf, "is_alive: poll error, assume dead");
    أرجع FALSE;
  }
  وإلا إذا(r == 0) {
    CURL_TRC_CF(داده, cf, "is_alive: poll timeout, assume alive");
    أرجع TRUE;
  }
  وإلا إذا(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(داده, cf, "is_alive: err/hup/etc events, assume dead");
    أرجع FALSE;
  }

  CURL_TRC_CF(داده, cf, "is_alive: valid events, looks alive");
  *درون_داد_pending = TRUE;
  أرجع TRUE;
}

ساكن CURLcode cf_socket_query(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
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
  حالة CF_QUERY_CONNECT_پاسخ_MS:
    إذا(ctx->got_اولی_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->اولی_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < بیشترین_صحیح) ? (صحيح)ms : بیشترین_صحیح;
    }
    وإلا
      *pres1 = -1;
    أرجع CURLE_OK;
  حالة CF_QUERY_IP_INFO:
#اگر_تعریف_شده USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#وإلا
    *pres1 = FALSE;
#پایان_اگر
    *(بنية ip_quadruple *)pres2 = ctx->ip;
    أرجع CURLE_OK;
  حالة CF_QUERY_REALLY_CONNECTED:
    إذا(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    وإلا
      *pres1 = ctx->got_اولی_byte;
    أرجع CURLE_OK;
  افتراضي:
    اكسر;
  }
  أرجع cf->بعدی ?
    cf->بعدی->cft->query(cf->بعدی, داده, query, pres1, pres2) :
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
  Curl_cf_def_داده_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_tcp_create(بنية Curl_cfilter **pcf,
                            بنية Curl_easy *داده,
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
  CURLcode نتیجه;

  (فراغ)داده;
  (فراغ)origin;
  (فراغ)conn;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  إذا(!addr) {
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    اذهب بیرون;
  }

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  نتیجه = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

بیرون:
  *pcf = (!نتیجه) ? cf : عدم;
  إذا(نتیجه) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  أرجع نتیجه;
}

#اگر_تعریف_شده __linux__
ساكن فراغ linux_quic_mtu(بنية cf_socket_ctx *ctx)
{
  صحيح val;
  بدل(ctx->addr.family) {
#اگر_تعریف_شده IP_MTU_DISCOVER
  حالة AF_INET:
    val = IP_PMTUDISC_DO;
    (فراغ)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     حجم(val));
    اكسر;
#پایان_اگر
#اگر_تعریف_شده IPV6_MTU_DISCOVER
  حالة AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (فراغ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     حجم(val));
    اكسر;
#پایان_اگر
  }
}
#وإلا
#تعریف_کن linux_quic_mtu(x)
#پایان_اگر

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
#تعریف_کن linux_quic_gro(x)
#پایان_اگر

#إذا (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
ساكن فراغ linux_quic_ecn(بنية cf_socket_ctx *ctx)
{
  بیعلامت صحيح tos = 1;
  بدل(ctx->addr.family) {
  حالة AF_INET:
    (فراغ)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, حجم(tos));
    اكسر;
#اگر_تعریف_شده IPV6_RECVTCLASS
  حالة AF_INET6:
    (فراغ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, حجم(tos));
    اكسر;
#پایان_اگر
  }
}
#وإلا
#تعریف_کن linux_quic_ecn(x)
#پایان_اگر

#إذا (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
ساكن فراغ linux_ip_dontfrag(بنية cf_socket_ctx *ctx)
{
  صحيح val = 1;

  بدل(ctx->addr.family) {
  حالة AF_INET:
    (فراغ)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, حجم(val));
    اكسر;
#اگر_تعریف_شده IPV6_DONTFRAG
  حالة AF_INET6:
    (فراغ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, حجم(val));
    اكسر;
#پایان_اگر
  }
}
#وإلا
#تعریف_کن linux_ip_dontfrag(x)
#پایان_اگر

ساكن CURLcode cf_udp_راهاندازی_quic(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده)
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
    أرجع socket_connect_نتیجه(داده, ctx->ip.remote_ip, SOCKERRNO);
  }
  ctx->sock_connected = TRUE;
  مجموعه_local_ip(cf, داده);
  CURL_TRC_CF(داده, cf, "%s socket %" FMT_SOCKET_T
              " connected: [%s:%d] -> [%s:%d]",
              (ctx->transport == TRNSPRT_QUIC) ? "QUIC" : "UDP",
              ctx->sock, ctx->ip.local_ip, ctx->ip.local_پورت,
              ctx->ip.remote_ip, ctx->ip.remote_پورت);

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
                               بنية Curl_easy *داده,
                               bool *انجامشده)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_COULDNT_CONNECT;

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  *انجامشده = FALSE;
  إذا(ctx->sock == CURL_SOCKET_BAD) {
    نتیجه = cf_socket_open(cf, داده);
    إذا(نتیجه) {
      CURL_TRC_CF(داده, cf, "cf_udp_connect(), open failed -> %d",
                  (صحيح)نتیجه);
      اذهب بیرون;
    }

    إذا(ctx->transport == TRNSPRT_QUIC) {
      نتیجه = cf_udp_راهاندازی_quic(cf, داده);
      إذا(نتیجه)
        اذهب بیرون;
      CURL_TRC_CF(داده, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_پورت);
    }
    *انجامشده = TRUE;
    cf->connected = TRUE;
  }
بیرون:
  أرجع نتیجه;
}

بنية Curl_cftype Curl_cft_udp = {
  "UDP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_udp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_داده_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_udp_create(بنية Curl_cfilter **pcf,
                            بنية Curl_easy *داده,
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
  CURLcode نتیجه;

  (فراغ)داده;
  (فراغ)origin;
  (فراغ)conn;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  نتیجه = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

بیرون:
  *pcf = (!نتیجه) ? cf : عدم;
  إذا(نتیجه) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  أرجع نتیجه;
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
  Curl_cf_def_داده_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_unix_create(بنية Curl_cfilter **pcf,
                            بنية Curl_easy *داده,
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
  CURLcode نتیجه;

  (فراغ)داده;
  (فراغ)origin;
  (فراغ)conn;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  نتیجه = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

بیرون:
  *pcf = (!نتیجه) ? cf : عدم;
  إذا(نتیجه) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  أرجع نتیجه;
}

ساكن timediff_t cf_tcp_accept_timeleft(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#اگر_تعریف_نشده CURL_DISABLE_FTP
  إذا(داده->مجموعه.accepttimeout > 0)
    timeout_ms = داده->مجموعه.accepttimeout;
#پایان_اگر

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(داده);
  إذا(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  وإلا {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(داده), &ctx->started_at);
    إذا(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  أرجع timeout_ms;
}

ساكن فراغ cf_tcp_مجموعه_accepted_remote_ip(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *داده)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
#اگر_تعریف_شده HAVE_GETPEERNAME
  CURLcode نتیجه;
  بنية Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_پورت = 0;
  plen = حجم(ssrem);
  memset(&ssrem, 0, plen);
  إذا(getpeername(ctx->sock, (بنية sockaddr *)&ssrem, &plen)) {
    حرف میانگیر[STRERROR_LEN];
    صحيح sockerr = SOCKERRNO;
    failf(داده, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, میانگیر, حجم(میانگیر)));
    أرجع;
  }
  نتیجه = sockaddr2string((بنية sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_پورت);
  إذا(نتیجه) {
    failf(داده, "ssrem inet_ntop() failed with %d", (صحيح)نتیجه);
    أرجع;
  }
#وإلا
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_پورت = 0;
  (فراغ)داده;
#پایان_اگر
}

ساكن CURLcode cf_tcp_accept_connect(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده,
                                      bool *انجامشده)
{
  بنية cf_socket_ctx *ctx = cf->ctx;
  حرف errbuf[STRERROR_LEN];
#اگر_تعریف_شده USE_IPV6
  بنية Curl_sockaddr_storage جمع_کن;
#وإلا
  بنية sockaddr_in جمع_کن;
#پایان_اگر
  curl_socklen_t اندازه = (curl_socklen_t)حجم(جمع_کن);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  صحيح socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (فراغ)داده;
  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  *انجامشده = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, داده);
  إذا(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(داده, "Accept timeout occurred while waiting server connect");
    أرجع CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(داده, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_پورت);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(داده, cf, "socket_check -> %x", (بیعلامت صحيح)socketstate);
  بدل(socketstate) {
  حالة -1: /* error */
    /* let's die here */
    failf(داده, "Error while waiting for server connect");
    أرجع CURLE_FTP_ACCEPT_FAILED;
  افتراضي:
    إذا(socketstate & CURL_CSELECT_درون) {
      infof(داده, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    اكسر;
  }

  إذا(!incoming) {
    CURL_TRC_CF(داده, cf, "nothing heard from the server yet");
    أرجع CURLE_OK;
  }

  اندازه = حجم(جمع_کن);
#اگر_تعریف_شده HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (بنية sockaddr *)&جمع_کن, &اندازه,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#وإلا
  s_accepted = CURL_ACCEPT(ctx->sock, (بنية sockaddr *)&جمع_کن, &اندازه);
#پایان_اگر

  إذا(s_accepted == CURL_SOCKET_BAD) {
    failf(داده, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    أرجع CURLE_FTP_ACCEPT_FAILED;
  }
#اگر_تعریف_نشده HAVE_ACCEPT4
#اگر_تعریف_شده HAVE_FCNTL
  إذا(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(داده, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    Curl_socket_close(داده, cf->conn, s_accepted);
    أرجع CURLE_FTP_ACCEPT_FAILED;
  }
#پایان_اگر /* HAVE_FCNTL */
  إذا(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(داده, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    Curl_socket_close(داده, cf->conn, s_accepted);
    أرجع CURLE_FTP_ACCEPT_FAILED;
  }
#پایان_اگر /* !HAVE_ACCEPT4 */
  infof(داده, "Connection accepted from server");

  /* Replace any filter on SECONDARY with one listening on this socket */
  ctx->listening = FALSE;
  ctx->accepted = TRUE;
  socket_close(داده, cf->conn, TRUE, ctx->sock);
  ctx->sock = s_accepted;

  cf->conn->sock[cf->sockindex] = ctx->sock;
  cf_tcp_مجموعه_accepted_remote_ip(cf, داده);
  مجموعه_local_ip(cf, داده);
  ctx->فعال = TRUE;
  ctx->connected_at = *Curl_pgrs_now(داده);
  cf->connected = TRUE;
  CURL_TRC_CF(داده, cf, "accepted_set(sock=%" FMT_SOCKET_T
              ", remote=%s port=%d)",
              ctx->sock, ctx->ip.remote_ip, ctx->ip.remote_پورت);

  إذا(داده->مجموعه.fsockopt) {
    بنية Curl_mapi_guard guard;
    صحيح error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_شروع(&guard, داده, easy_fsockopt);
    error = داده->مجموعه.fsockopt(داده->مجموعه.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_انتها(&guard);

    إذا(error)
      أرجع CURLE_ABORTED_BY_CALLBACK;
  }
  *انجامشده = TRUE;
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
  Curl_cf_def_داده_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_conn_tcp_listen_مجموعه(بنية Curl_easy *داده,
                                  بنية connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode نتیجه;
  بنية Curl_cfilter *cf = عدم;
  بنية cf_socket_ctx *ctx = عدم;

  /* replace any existing */
  Curl_conn_cf_discard_all(داده, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  نتیجه = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  Curl_conn_cf_جمع_کن(داده, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(داده);
  conn->sock[sockindex] = ctx->sock;
  مجموعه_local_ip(cf, داده);
  CURL_TRC_CF(داده, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_پورت);

بیرون:
  إذا(نتیجه) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  أرجع نتیجه;
}

bool Curl_conn_is_tcp_listen(بنية Curl_easy *داده,
                             int8_t sockindex)
{
  بنية Curl_cfilter *cf = داده->conn->cfilter[sockindex];
  طالما(cf) {
    إذا(cf->cft == &Curl_cft_tcp_accept)
      أرجع TRUE;
    cf = cf->بعدی;
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
                             بنية Curl_easy *داده,
                             curl_socket_t *psock,
                             ثابت بنية Curl_sockaddr_ex **paddr,
                             بنية ip_quadruple *pip)
{
  (فراغ)داده;
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
