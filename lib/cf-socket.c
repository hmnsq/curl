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

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h> /* <netinet/tcp.h> may need it */
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_UN_H
#شامل <sys/un.h> /* for sockaddr_un */
#اگر_ختم
#اگر_تعریف_شدہ HAVE_LINUX_TCP_H
#شامل <linux/tcp.h>
#وگرنہ_اگر defined(HAVE_NETINET_TCP_H)
#شامل <netinet/tcp.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETINET_UDP_H
#شامل <netinet/udp.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETINET_IP_H
#شامل <netinet/ip.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_IFADDRS_H
#شامل <ifaddrs.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NET_IF_H
#شامل <net/if.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

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
#اگر_تعریف_شدہ _WIN32
#شامل <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#اگر_ختم

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(ساخت sockaddr *sa, curl_socklen_t salen,
                                  حرف *addr, uint16_t *پورٹ);
UNITTEST CURLcode sockaddr2string(ساخت sockaddr *sa, curl_socklen_t salen,
                                  حرف *addr, uint16_t *پورٹ)
{
  CURLcode نتیجہ;
  ساخت sockaddr_in *si = تہی;
#اگر_تعریف_شدہ USE_IPV6
  ساخت sockaddr_in6 *si6 = تہی;
#اگر_ختم
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
  ساخت sockaddr_un *su = تہی;
#ورنہ
  (خالی)salen;
#اگر_ختم

  انتخاب(sa->sa_family) {
  حالت AF_INET:
    si = (ساخت sockaddr_in *)(خالی *)sa;
    نتیجہ = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    اگر(!نتیجہ) {
      *پورٹ = ntohs(si->sin_port);
      واپس نتیجہ;
    }
    توڑو;
#اگر_تعریف_شدہ USE_IPV6
  حالت AF_INET6:
    si6 = (ساخت sockaddr_in6 *)(خالی *)sa;
    نتیجہ = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    اگر(!نتیجہ) {
      *پورٹ = ntohs(si6->sin6_port);
      واپس نتیجہ;
    }
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
  حالت AF_UNIX:
    اگر(salen > (curl_socklen_t)حجم(CURL_SA_FAMILY_T)) {
      su = (ساخت sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_راستہ);
    }
    ورنہ
      addr[0] = 0; /* socket with no name */
    *پورٹ = 0;
    واپس CURLE_OK;
#اگر_ختم
  طےشدہ:
    نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
    توڑو;
  }

  addr[0] = '\0';
  *پورٹ = 0;
  واپس نتیجہ;
}

ساکن خالی tcpnodelay(ساخت Curl_cfilter *cf,
                       ساخت Curl_easy *ڈیٹا,
                       curl_socket_t sockfd)
{
#اگر defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  صحیح level = IPPROTO_TCP;
  VERBOSE(حرف بفر[STRERROR_LEN]);

  اگر(setsockopt(sockfd, level, TCP_NODELAY, (خالی *)&onoff, حجم(onoff)) < 0)
    CURL_TRC_CF(ڈیٹا, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, بفر, حجم(بفر)));
#ورنہ
  (خالی)cf;
  (خالی)ڈیٹا;
  (خالی)sockfd;
#اگر_ختم
}

#اگر defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_حد) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_حد)
#اگر defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
ساکن خالی alive_unit(صحیح *x)
{
  /* make sure this doesn't wrap */
  اگر(*x < (زیادہ_سے_زیادہ_عدد / 1000))
    *x *= 1000;
  ورنہ
    /* this is still almost 25 days */
    *x = زیادہ_سے_زیادہ_عدد;
}
#ورنہ
#تعریف_کرو alive_unit(x)
#اگر_ختم
#اگر_ختم

ساکن خالی tcpkeepalive(ساخت Curl_cfilter *cf,
                         ساخت Curl_easy *ڈیٹا,
                         curl_socket_t sockfd)
{
  صحیح optval = ڈیٹا->سیٹ.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  اگر(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (خالی *)&optval, حجم(optval)) < 0) {
    CURL_TRC_CF(ڈیٹا, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  ورنہ {
#اگر_تعریف_شدہ USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    اگر(curlx_verify_windows_نسخہ(10, 0, 16299, PLATFORM_WINNT,
                                    نسخہ_GREATER_THAN_EQUAL)) {
      CURL_TRC_CF(ڈیٹا, cf, "Set TCP_KEEP* on fd=%" FMT_SOCKET_T, sockfd);
      optval = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepidle);
/* Offered by mingw-w64 v12+, MS SDK 6.0A/VS2008+ */
#اگر_غیر_تعریف_شدہ TCP_KEEPALIVE
#تعریف_کرو TCP_KEEPALIVE 3
#اگر_ختم
/* Offered by mingw-w64 v12+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#اگر_غیر_تعریف_شدہ TCP_KEEPCNT
#تعریف_کرو TCP_KEEPCNT 16
#اگر_ختم
/* Offered by mingw-w64 v12+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#اگر_غیر_تعریف_شدہ TCP_KEEPIDLE
#تعریف_کرو TCP_KEEPIDLE TCP_KEEPALIVE
#اگر_ختم
#اگر_غیر_تعریف_شدہ TCP_KEEPINTVL
#تعریف_کرو TCP_KEEPINTVL 17
#اگر_ختم
      اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (ثابت حرف *)&optval, حجم(optval)) < 0) {
        CURL_TRC_CF(ڈیٹا, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepintvl);
      اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (ثابت حرف *)&optval, حجم(optval)) < 0) {
        CURL_TRC_CF(ڈیٹا, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepcnt);
      اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (ثابت حرف *)&optval, حجم(optval)) < 0) {
        CURL_TRC_CF(ڈیٹا, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    ورنہ {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#اگر_غیر_تعریف_شدہ SIO_KEEPALIVE_VALS
#تعریف_کرو SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      ساخت tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#اگر_ختم
      ساخت tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      اگر(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, حجم(vals),
                  تہی, 0, &dummy, تہی, تہی) != 0) {
        CURL_TRC_CF(ڈیٹا, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#ورنہ /* !USE_WINSOCK */
#اگر_تعریف_شدہ TCP_KEEPIDLE
    optval = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepidle);
    alive_unit(&optval);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (خالی *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(ڈیٹا, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وگرنہ_اگر defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepidle);
    alive_unit(&optval);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (خالی *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(ڈیٹا, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وگرنہ_اگر defined(TCP_KEEPALIVE_حد)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepidle);
    alive_unit(&optval);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_حد,
                  (خالی *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(ڈیٹا, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#اگر_ختم
#اگر_تعریف_شدہ TCP_KEEPINTVL
    optval = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepintvl);
    alive_unit(&optval);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (خالی *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(ڈیٹا, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#وگرنہ_اگر defined(TCP_KEEPALIVE_ABORT_حد)
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
      صحیح keepcnt = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepcnt);
      صحیح keepintvl = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepintvl);

      اگر(keepcnt > 0 && keepintvl > (زیادہ_سے_زیادہ_عدد / keepcnt))
        optval = زیادہ_سے_زیادہ_عدد;
      ورنہ
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_حد,
                  (خالی *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(ڈیٹا, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#اگر_ختم
#اگر_تعریف_شدہ TCP_KEEPCNT
    optval = curlx_sltosi(ڈیٹا->سیٹ.tcp_keepcnt);
    اگر(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (خالی *)&optval, حجم(optval)) < 0) {
      CURL_TRC_CF(ڈیٹا, cf, "Failed to set TCP_KEEPCNT on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#اگر_ختم
#اگر_ختم /* USE_WINSOCK */
  }
}

/**
 * Assign the addrinfo `ai` to the Curl_sockaddr_ex `addr` with
 * transport determining socktype and protocol.
 */
CURLcode Curl_socket_addr_from_ai(ساخت Curl_sockaddr_ex *addr,
                                  ثابت ساخت Curl_addrinfo *ai,
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
  addr->addrlen = (غیرعلامتی صحیح)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= حجم(addr->curl_sa_addrbuf));
  اگر(addr->addrlen > حجم(addr->curl_sa_addrbuf))
    واپس CURLE_TOO_LARGE;

  حافظہ_نقل(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  واپس CURLE_OK;
}

#اگر_تعریف_شدہ USE_SO_NOSIGPIPE
صحیح Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  صحیح onoff = 1;
  واپس setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (خالی *)&onoff, حجم(onoff));
}
#اگر_ختم /* USE_SO_NOSIGPIPE */

#اگر defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ساکن uint32_t get_scope_id(ساخت Curl_easy *ڈیٹا,
                             ساخت sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  اگر(ڈیٹا->conn->scope_id)
    واپس ڈیٹا->conn->scope_id;
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
    ساخت ifaddrs *ifa, *ifa_list;
    اگر(getifaddrs(&ifa_list) == 0) {
      برائے(ifa = ifa_list; ifa; ifa = ifa->ifa_اگلا) {
        اگر(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          ساخت sockaddr_in6 *s6 = (خالی *)ifa->ifa_addr;
          اگر(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(ڈیٹا,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (غیرعلامتی طویل)scope_id);
            توڑو;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#اگر_ختم /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  واپس scope_id;
}
#اگر_ختم

ساکن CURLcode socket_open(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  حرف errbuf[STRERROR_LEN];

#اگر_تعریف_شدہ SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#اگر_ختم

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);
  اگر(ڈیٹا->سیٹ.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    ساخت Curl_mapi_guard guard;
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fopensocket);
    *sockfd = ڈیٹا->سیٹ.fopensocket(ڈیٹا->سیٹ.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (ساخت curl_sockaddr *)addr);
    CURL_CBAPI_انتہا(&guard);
  }
  ورنہ {
    /* opensocket callback not set, so create the socket now */
#اگر_تعریف_شدہ DEBUGBUILD
    اگر((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(ڈیٹا, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      واپس CURLE_COULDNT_CONNECT;
    }
#اگر_ختم
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    اگر((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      واپس CURLE_برآمد_کا_حافظہ;
  }

  اگر(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(ڈیٹا, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    واپس CURLE_COULDNT_CONNECT;
  }

#اگر_تعریف_شدہ USE_SO_NOSIGPIPE
  اگر(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(ڈیٹا, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    واپس CURLE_COULDNT_CONNECT;
  }
#اگر_ختم /* USE_SO_NOSIGPIPE */

#اگر defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  اگر(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(ڈیٹا, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    واپس CURLE_COULDNT_CONNECT;
  }
#اگر_ختم

#اگر defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  اگر(addr->family == AF_INET6) {
    ساخت sockaddr_in6 * ثابت sa6 = (خالی *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(ڈیٹا, sa6);
  }
#اگر_ختم
  واپس CURLE_OK;
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
CURLcode Curl_socket_open(ساخت Curl_easy *ڈیٹا,
                          ثابت ساخت Curl_addrinfo *ai,
                          ساخت Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  ساخت Curl_sockaddr_ex dummy;
  CURLcode نتیجہ;

  اگر(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  نتیجہ = Curl_socket_addr_from_ai(addr, ai, transport);
  اگر(نتیجہ)
    واپس نتیجہ;

  واپس socket_open(ڈیٹا, addr, sockfd);
}

ساکن صحیح socket_close(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                        صحیح use_callback, curl_socket_t sock)
{
  اگر(sock == CURL_SOCKET_BAD)
    واپس 0;

  اگر(use_callback && conn && conn->fclosesocket) {
    ساخت Curl_mapi_guard guard;
    صحیح rc;
    Curl_multi_will_close(ڈیٹا, sock);
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_انتہا(&guard);
    واپس rc;
  }

  اگر(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(ڈیٹا, sock);

  sclose(sock);

  واپس 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
صحیح Curl_socket_close(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                      curl_socket_t sock)
{
  واپس socket_close(ڈیٹا, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(ثابت حرف *داخلہ,
                              حرف **dev, حرف **iface, حرف **host)
{
  ساکن ثابت حرف if_سابقہ[] = "if!";
  ساکن ثابت حرف host_سابقہ[] = "host!";
  ساکن ثابت حرف if_host_سابقہ[] = "ifhost!";
  size_t len;

  len = لڑی_لمبائی(داخلہ);
  اگر(len > 512)
    واپس CURLE_BAD_FUNCTION_دلیل;

  اگر(dev)
    *dev = تہی;
  اگر(iface)
    *iface = تہی;
  اگر(host)
    *host = تہی;

  اگر(!لڑی_موازنہ(if_سابقہ, داخلہ, لڑی_لمبائی(if_سابقہ))) {
    داخلہ += لڑی_لمبائی(if_سابقہ);
    اگر(!*داخلہ)
      واپس CURLE_BAD_FUNCTION_دلیل;
    اگر(iface) {
      *iface = curlx_memdup0(داخلہ, len - لڑی_لمبائی(if_سابقہ));
      واپس *iface ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
    }
    واپس CURLE_OK;
  }
  ورنہ اگر(!لڑی_موازنہ(host_سابقہ, داخلہ, لڑی_لمبائی(host_سابقہ))) {
    داخلہ += لڑی_لمبائی(host_سابقہ);
    اگر(!*داخلہ)
      واپس CURLE_BAD_FUNCTION_دلیل;
    اگر(host) {
      *host = curlx_memdup0(داخلہ, len - لڑی_لمبائی(host_سابقہ));
      واپس *host ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
    }
    واپس CURLE_OK;
  }
  ورنہ اگر(!لڑی_موازنہ(if_host_سابقہ, داخلہ, لڑی_لمبائی(if_host_سابقہ))) {
    ثابت حرف *host_part;
    داخلہ += لڑی_لمبائی(if_host_سابقہ);
    len -= لڑی_لمبائی(if_host_سابقہ);
    host_part = memchr(داخلہ, '!', len);
    اگر(!host_part || !*(host_part + 1))
      واپس CURLE_BAD_FUNCTION_دلیل;
    اگر(iface) {
      *iface = curlx_memdup0(داخلہ, host_part - داخلہ);
      اگر(!*iface)
        واپس CURLE_برآمد_کا_حافظہ;
    }
    ++host_part;
    اگر(host) {
      *host = curlx_memdup0(host_part, len - (host_part - داخلہ));
      اگر(!*host) {
        اگر(iface)
          curlx_safefree(*iface);
        واپس CURLE_برآمد_کا_حافظہ;
      }
    }
    واپس CURLE_OK;
  }

  اگر(!*داخلہ)
    واپس CURLE_BAD_FUNCTION_دلیل;
  اگر(dev) {
    *dev = curlx_memdup0(داخلہ, len);
    واپس *dev ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
  }
  واپس CURLE_OK;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_BINDLOCAL
ساکن CURLcode bindlocal(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                          curl_socket_t sockfd, صحیح af, غیرعلامتی صحیح scope,
                          uint8_t transport)
{
  ساخت Curl_sockaddr_storage sa;
  ساخت sockaddr *sock = (ساخت sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  ساخت sockaddr_in *si4 = (ساخت sockaddr_in *)&sa;
#اگر_تعریف_شدہ USE_IPV6
  ساخت sockaddr_in6 *si6 = (ساخت sockaddr_in6 *)&sa;
#اگر_ختم
  ساخت Curl_dns_اندراج *h = تہی;
  غیرعلامتی مختصر پورٹ = ڈیٹا->سیٹ.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  صحیح portnum = ڈیٹا->سیٹ.localportrange;
  حرف *dev_اندر = تہی, *iface_اندر = تہی, *host_اندر = تہی;
  ثابت حرف *iface = تہی, *host = تہی;
  صحیح sockerr;
#اگر_تعریف_شدہ IP_BIND_ADDRESS_NO_PORT
  صحیح on = 1;
#اگر_ختم
  CURLcode نتیجہ = CURLE_OK;

#اگر_غیر_تعریف_شدہ USE_IPV6
  (خالی)scope;
#اگر_ختم
  اگر(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    نتیجہ = Curl_parse_interface(conn->interface_opts,
                                  &dev_اندر, &iface_اندر, &host_اندر);
    اگر(نتیجہ)
      جاؤ برآمد;
    /* This is how we use the values */
    iface = iface_اندر ? iface_اندر : dev_اندر;
    host = host_اندر ? host_اندر : dev_اندر;
  }

  اگر(!iface && !host && !پورٹ)
    جاؤ برآمد;

  اگر(iface && (لڑی_لمبائی(iface) >= 255)) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }
  memset(&sa, 0, حجم(ساخت Curl_sockaddr_storage));

  اگر(iface || host) {
    حرف myhost[256] = "";
    صحیح مکمل = 0; /* -1 for error, 1 for address found */
    if2ip_نتیجہ_t if2ip_نتیجہ = IF2_IP_NOT_ملا;

#اگر_تعریف_شدہ SO_BINDTODEVICE
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
                  iface, (curl_socklen_t)لڑی_لمبائی(iface) + 1) == 0 &&
       /* This is often "errno 1, error: Operation not permitted" if you are
        * not running as root or another suitable privileged user. If it
        * succeeds it means the parameter was a valid interface and not an IP
        * address. Return immediately.
        */
       !host_اندر) {
      infof(ڈیٹا, "socket successfully bound to interface '%s'", iface);
      نتیجہ = CURLE_OK;
      جاؤ برآمد;
    }
#اگر_ختم
    اگر(!host_اندر) {
      /* Discover IP from input device, then bind to it */
      if2ip_نتیجہ = Curl_if2ip(af,
#اگر_تعریف_شدہ USE_IPV6
                                scope, conn->scope_id,
#اگر_ختم
                                iface, myhost, حجم(myhost));
    }
    انتخاب(if2ip_نتیجہ) {
    حالت IF2_IP_NOT_ملا:
      اگر(iface_اندر && !host_اندر) {
        /* Do not fall back to treating it as a hostname */
        حرف بفر[STRERROR_LEN];
        ڈیٹا->state.os_errno = sockerr = SOCKERRNO;
        failf(ڈیٹا, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, بفر, حجم(بفر)));
        نتیجہ = CURLE_INTERFACE_FAILED;
        جاؤ برآمد;
      }
      توڑو;
    حالت IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
      جاؤ برآمد;
    حالت IF2_IP_ملا:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(ڈیٹا, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      مکمل = 1;
      توڑو;
    }
    اگر(!iface_اندر || host_اندر) {
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
#اگر_تعریف_شدہ USE_IPV6
      اگر(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#اگر_ختم

      (خالی)Curl_resolv_blocking(ڈیٹا, dns_queries, host, 80, transport, &h);
      اگر(h) {
        صحیح h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_پتہ(h->addr, myhost, حجم(myhost));
        infof(ڈیٹا, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_اندراج_unlink(ڈیٹا, &h); /* this will NULL, potential free h */
        اگر(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
          جاؤ برآمد;
        }
        مکمل = 1;
      }
      ورنہ {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        مکمل = -1;
      }
    }

    اگر(مکمل > 0) {
#اگر_تعریف_شدہ USE_IPV6
      /* IPv6 address */
      اگر(af == AF_INET6) {
#اگر_تعریف_شدہ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        حرف *scope_ptr = strchr(myhost, '%');
        اگر(scope_ptr)
          *(scope_ptr++) = '\0';
#اگر_ختم
        اگر(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(پورٹ);
#اگر_تعریف_شدہ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          اگر(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            اگر(curlx_str_نمبر((ثابت حرف **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
              جاؤ برآمد;
            }
            si6->sin6_scope_id = (غیرعلامتی صحیح)scope_id;
          }
#اگر_ختم
        }
        sizeof_sa = حجم(ساخت sockaddr_in6);
      }
      ورنہ
#اگر_ختم
      /* IPv4 address */
      اگر((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(پورٹ);
        sizeof_sa = حجم(ساخت sockaddr_in);
      }
    }

    اگر(مکمل < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      حرف بفر[STRERROR_LEN];
      ڈیٹا->state.errorbuf = FALSE;
      ڈیٹا->state.os_errno = sockerr = SOCKERRNO;
      failf(ڈیٹا, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, بفر, حجم(بفر)));
      نتیجہ = CURLE_INTERFACE_FAILED;
      جاؤ برآمد;
    }
  }
  ورنہ {
    /* no device was given, prepare sa to match af's needs */
#اگر_تعریف_شدہ USE_IPV6
    اگر(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(پورٹ);
      sizeof_sa = حجم(ساخت sockaddr_in6);
    }
    ورنہ
#اگر_ختم
    اگر(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(پورٹ);
      sizeof_sa = حجم(ساخت sockaddr_in);
    }
  }
#اگر_تعریف_شدہ IP_BIND_ADDRESS_NO_PORT
  (خالی)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, حجم(on));
#اگر_ختم
  برائے(;;) {
    اگر(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(ڈیٹا, "Local port: %hu", پورٹ);
      conn->بٹ.bound = TRUE;
      نتیجہ = CURLE_OK;
      جاؤ برآمد;
    }

    اگر(--portnum > 0) {
      پورٹ++; /* try next port */
      اگر(پورٹ == 0)
        توڑو;
      infof(ڈیٹا, "Bind to local port %d failed, trying next", پورٹ - 1);
      /* We reuse/clobber the port variable here below */
      اگر(sock->sa_family == AF_INET)
        si4->sin_port = htons(پورٹ);
#اگر_تعریف_شدہ USE_IPV6
      ورنہ
        si6->sin6_port = htons(پورٹ);
#اگر_ختم
    }
    ورنہ
      توڑو;
  }
  {
    حرف بفر[STRERROR_LEN];
    ڈیٹا->state.os_errno = sockerr = SOCKERRNO;
    failf(ڈیٹا, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, بفر, حجم(بفر)));
  }

  نتیجہ = CURLE_INTERFACE_FAILED;

برآمد:
  curlx_free(dev_اندر);
  curlx_free(iface_اندر);
  curlx_free(host_اندر);
  واپس نتیجہ;
}
#اگر_ختم

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
ساکن bool verifyconnect(curl_socket_t sockfd, صحیح *psockerr)
{
  bool rc = TRUE;
#اگر_تعریف_شدہ SO_ERROR
  صحیح sockerr = 0;
  curl_socklen_t err_سائز = حجم(sockerr);

#اگر_تعریف_شدہ _WIN32
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
#اگر_ختم

  اگر(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (خالی *)&sockerr, &err_سائز))
    sockerr = SOCKERRNO;
#اگر defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  اگر(EBADIOCTL == sockerr) {
    سیٹ_SOCKERRNO(0);
    sockerr = 0;
  }
#اگر_ختم
  اگر((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  ورنہ
    /* This was not a successful connect */
    rc = FALSE;
  اگر(psockerr)
    *psockerr = sockerr;
#ورنہ
  (خالی)sockfd;
  اگر(psockerr)
    *psockerr = SOCKERRNO;
#اگر_ختم
  واپس rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
ساکن CURLcode socket_connect_نتیجہ(ساخت Curl_easy *ڈیٹا,
                                      ثابت حرف *ipaddress, صحیح sockerr)
{
  اگر(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    واپس CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(حرف بفر[STRERROR_LEN]);
    infof(ڈیٹا, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, بفر, حجم(بفر)));
    NOVERBOSE((خالی)ipaddress);
  }
  ڈیٹا->state.os_errno = sockerr;
  /* connect failed */
  واپس CURLE_COULDNT_CONNECT;
}

ساخت cf_socket_ctx {
  ساخت Curl_peer *peer;
  ساخت Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  ساخت ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  ساخت curltime started_at;        /* when socket was created */
  ساخت curltime connected_at;      /* when socket connected/got first byte */
  ساخت curltime پہلا_byte_at;     /* when first byte was recvd */
#اگر_تعریف_شدہ USE_WINSOCK
  ساخت curltime آخری_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_سائز;                     /* the last set SO_SNDBUF size */
#اگر_ختم
  صحیح sockerr;                       /* socket error of last failure or 0 */
#اگر_تعریف_شدہ DEBUGBUILD
  صحیح connect_wait_ms;               /* ms to wait after 1st connect() */
  صحیح wblock_percent;                /* percent of writes doing EAGAIN */
  صحیح wpartial_percent;              /* percent of bytes written in send */
  صحیح rblock_percent;                /* percent of reads doing EAGAIN */
  size_t recv_max;                   /* max enforced read size */
#اگر_ختم
  uint8_t transport;
  ثنائی_ہندسہ(got_پہلا_byte);               /* if first byte was received */
  ثنائی_ہندسہ(listening);                    /* socket is listening */
  ثنائی_ہندسہ(accepted);                     /* socket was accepted, not connected */
  ثنائی_ہندسہ(sock_connected);               /* socket is "connected", e.g. in UDP */
  ثنائی_ہندسہ(فعال);
  ثنائی_ہندسہ(stats_reported);
};

ساکن CURLcode cf_socket_ctx_init(ساخت cf_socket_ctx *ctx,
                                   ساخت Curl_peer *peer,
                                   ساخت Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, حجم(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#اگر_تعریف_شدہ DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_SOCK_WBLOCK");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, 100))
        ctx->wblock_percent = (صحیح)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, 100))
        ctx->wpartial_percent = (صحیح)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, 100))
        ctx->rblock_percent = (صحیح)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, زیادہ_سے_زیادہ_عدد))
        ctx->connect_wait_ms = (صحیح)l;
    }
  }
#اگر_ختم

  واپس CURLE_OK;
}

ساکن خالی cf_socket_ctx_free(ساخت cf_socket_ctx *ctx)
{
  اگر(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

ساکن CURLcode cf_socket_shutdown(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   bool *مکمل)
{
  اگر(cf->connected) {
    ساخت cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(ڈیٹا, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    اگر(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      غیرعلامتی حرف buf[1024];
      (خالی)sread(ctx->sock, buf, حجم(buf));
    }
  }
  *مکمل = TRUE;
  واپس CURLE_OK;
}

ساکن خالی cf_socket_destroy(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ڈیٹا, cf, "destroy");
  اگر(ctx) {
    اگر(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(ڈیٹا, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      اگر(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(ڈیٹا, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

ساکن خالی سیٹ_local_ip(ساخت Curl_cfilter *cf,
                         ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_پورٹ = 0;

#اگر_تعریف_شدہ HAVE_GETSOCKNAME
  اگر((ctx->sock != CURL_SOCKET_BAD) &&
     !(ڈیٹا->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    ساخت Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = حجم(ساخت Curl_sockaddr_storage);

    memset(&ssloc, 0, حجم(ssloc));
    اگر(getsockname(ctx->sock, (ساخت sockaddr *)&ssloc, &slen)) {
      VERBOSE(حرف بفر[STRERROR_LEN]);
      VERBOSE(صحیح sockerr = SOCKERRNO);
      infof(ڈیٹا, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, بفر, حجم(بفر)));
    }
    ورنہ {
      CURLcode نتیجہ = sockaddr2string((ساخت sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_پورٹ);
      اگر(نتیجہ)
        infof(ڈیٹا, "ssloc inet_ntop() failed with %d", (صحیح)نتیجہ);
    }
  }
#ورنہ
  (خالی)ڈیٹا;
#اگر_ختم
}

ساکن CURLcode سیٹ_remote_ip(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  نتیجہ = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_پورٹ);
  اگر(نتیجہ) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(ڈیٹا, "curl_sa_addr inet_ntop() failed with %d", (صحیح)نتیجہ);
    واپس CURLE_FAILED_INIT;
  }
  واپس CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
ساکن صحیح cf_socktype(صحیح x)
{
#اگر_تعریف_شدہ SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#اگر_ختم
#اگر_تعریف_شدہ CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#اگر_ختم
  واپس x;
}

#اگر_تعریف_شدہ _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#اگر_غیر_تعریف_شدہ SIO_TCP_INITIAL_RTO
#تعریف_کرو SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#تعریف_کرو TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
قسمتعریف ساخت _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#اگر_ختم /* SIO_TCP_INITIAL_RTO */

#اگر_غیر_تعریف_شدہ TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#تعریف_کرو TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#اگر_ختم

ساکن bool targets_localhost(ساخت cf_socket_ctx *ctx)
{
  واپس (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
ساکن خالی tcplocalhost(ساخت Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  اگر(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD بائٹیں = 0;
    memset(&rto, 0, حجم(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    اگر(curlx_verify_windows_نسخہ(10, 0, 16299, PLATFORM_WINNT,
                                    نسخہ_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    ورنہ
      rto.MaxSynRetransmissions = 1;
    (خالی)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, حجم(rto),
                   تہی, 0, &بائٹیں, تہی, تہی);
  }
}
#ورنہ
#تعریف_کرو tcplocalhost(x, y)
#اگر_ختم /* _WIN32 */

ساکن CURLcode cf_socket_open(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  صحیح error = 0;
  bool isconnected = FALSE;
  CURLcode نتیجہ = CURLE_COULDNT_CONNECT;
  bool is_tcp;

  DEBUGASSERT(ctx->sock == CURL_SOCKET_BAD);
  ctx->started_at = *Curl_pgrs_now(ڈیٹا);
#اگر_تعریف_شدہ CURL_USE_SOCK_NONBLOCK
  /* Do not tuck SOCK_NONBLOCK into socktype when opensocket callback is set
   * because we would not know how socktype is about to be used in the
   * callback, SOCK_NONBLOCK might get factored out before calling socket().
   */
  اگر(!ڈیٹا->سیٹ.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#اگر_ختم
  نتیجہ = socket_open(ڈیٹا, &ctx->addr, &ctx->sock);
#اگر_تعریف_شدہ CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  اگر(!ڈیٹا->سیٹ.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#اگر_ختم
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = سیٹ_remote_ip(cf, ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

#اگر_تعریف_شدہ USE_IPV6
  اگر(ctx->addr.family == AF_INET6) {
#اگر_تعریف_شدہ USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    صحیح on = 0;
    (خالی)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (خالی *)&on, حجم(on));
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      ساخت sockaddr_in6 *sa6 = (خالی *)&ctx->addr.curl_sa_addr;
      اگر(sa6->sin6_scope_id)
        infof(ڈیٹا, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_پورٹ,
              (غیرعلامتی طویل)sa6->sin6_scope_id);
      ورنہ
#اگر_ختم
        infof(ڈیٹا, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_پورٹ);
#اگر_تعریف_شدہ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#اگر_ختم
  }
  ورنہ
#اگر_ختم
    infof(ڈیٹا, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_پورٹ);

#اگر_تعریف_شدہ USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#ورنہ
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#اگر_ختم
  اگر(is_tcp) {
    اگر(ڈیٹا->سیٹ.tcp_nodelay)
      tcpnodelay(cf, ڈیٹا, ctx->sock);

    اگر(ڈیٹا->سیٹ.tcp_keepalive)
      tcpkeepalive(cf, ڈیٹا, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  اگر(ڈیٹا->سیٹ.fsockopt) {
    /* activate callback for setting socket options */
    ساخت Curl_mapi_guard guard;
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fsockopt);
    error = ڈیٹا->سیٹ.fsockopt(ڈیٹا->سیٹ.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_انتہا(&guard);

    اگر(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    ورنہ اگر(error) {
      نتیجہ = CURLE_ABORTED_BY_CALLBACK;
      جاؤ برآمد;
    }
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  اگر(ctx->addr.family == AF_INET
#اگر_تعریف_شدہ USE_IPV6
     || ctx->addr.family == AF_INET6
#اگر_ختم
    ) {
    نتیجہ = bindlocal(ڈیٹا, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        نتیجہ = CURLE_COULDNT_CONNECT;
      }
      جاؤ برآمد;
    }
  }
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  اگر(error < 0) {
    نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    جاؤ برآمد;
  }
#ورنہ
  اگر(ڈیٹا->سیٹ.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    اگر(error < 0) {
      نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      جاؤ برآمد;
    }
  }
#اگر_ختم
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
برآمد:
  اگر(نتیجہ) {
    اگر(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(ڈیٹا, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  ورنہ اگر(isconnected) {
    سیٹ_local_ip(cf, ڈیٹا);
    ctx->connected_at = *Curl_pgrs_now(ڈیٹا);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(ڈیٹا, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (صحیح)نتیجہ, ctx->sock);
  واپس نتیجہ;
}

ساکن صحیح do_connect(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                      bool is_tcp_fastopen)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
#اگر_تعریف_شدہ TCP_FASTOPEN_CONNECT
  صحیح optval = 1;
#اگر_ختم
  صحیح rc = -1;

  (خالی)ڈیٹا;
  اگر(is_tcp_fastopen) {
#اگر_تعریف_شدہ CONNECT_ڈیٹا_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_دستیاب
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    اگر(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = تہی;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_ڈیٹا_IDEMPOTENT,
                    تہی, 0, تہی, تہی);
    }
    ورنہ {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  ورنہ
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#وگرنہ_اگر defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    اگر(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (خالی *)&optval, حجم(optval)) < 0)
      CURL_TRC_CF(ڈیٹا, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#وگرنہ_اگر defined(MSG_FASTOPEN) /* old Linux */
    اگر(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    ورنہ
      rc = 0; /* Do nothing */
#اگر_ختم
  }
  ورنہ {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  واپس rc;
}

ساکن CURLcode cf_tcp_connect(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               bool *مکمل)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_COULDNT_CONNECT;
  صحیح rc = 0;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  *مکمل = FALSE; /* a negative world view is best */
  اگر(ctx->sock == CURL_SOCKET_BAD) {
    صحیح sockerr;

    نتیجہ = cf_socket_open(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;

    اگر(cf->connected) {
      *مکمل = TRUE;
      واپس CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, ڈیٹا, (bool)cf->conn->بٹ.tcp_fastopen);
    sockerr = SOCKERRNO;
    سیٹ_local_ip(cf, ڈیٹا);
    CURL_TRC_CF(ڈیٹا, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_پورٹ);
#اگر_تعریف_شدہ DEBUGBUILD
    اگر(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(ڈیٹا, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#اگر_ختم
    اگر(rc == -1) {
      ctx->sockerr = sockerr;
      نتیجہ = socket_connect_نتیجہ(ڈیٹا, ctx->ip.remote_ip, sockerr);
      جاؤ برآمد;
    }
  }

#اگر_تعریف_شدہ mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (خالی)verifyconnect(ctx->sock, تہی);
#اگر_ختم
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  اگر(rc == 0) { /* no connection yet */
    CURL_TRC_CF(ڈیٹا, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    واپس CURLE_OK;
  }
  ورنہ اگر(rc == CURL_CSELECT_برآمد ||
#اگر_تعریف_شدہ __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#اگر_ختم
          cf->conn->بٹ.tcp_fastopen) {
    اگر(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(ڈیٹا);
      سیٹ_local_ip(cf, ڈیٹا);
      *مکمل = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(ڈیٹا, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      واپس CURLE_OK;
    }
    نتیجہ = CURLE_COULDNT_CONNECT;
  }
  ورنہ اگر(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(ڈیٹا, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (خالی)verifyconnect(ctx->sock, &ctx->sockerr);
    نتیجہ = CURLE_COULDNT_CONNECT;
  }

برآمد:
  اگر(نتیجہ) {
    VERBOSE(حرف بفر[STRERROR_LEN]);
    سیٹ_local_ip(cf, ڈیٹا);
    اگر(ctx->sockerr) {
      ڈیٹا->state.os_errno = ctx->sockerr;
      سیٹ_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, بفر, حجم(بفر)));
    }
    ورنہ {
      VERBOSE(curlx_strcopy(بفر, حجم(بفر), STRCONST("peer closed")));
    }
    اگر(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(ڈیٹا, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(ڈیٹا, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_پورٹ,
          ctx->ip.local_ip, ctx->ip.local_پورٹ,
          curlx_strerror(ctx->sockerr, بفر, حجم(بفر)));
    *مکمل = FALSE;
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_socket_adjust_pollset(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا,
                                         ساخت easy_pollset *ps)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    اگر(ctx->listening) {
      نتیجہ = Curl_pollset_سیٹ_اندر_only(ڈیٹا, ps, ctx->sock);
      CURL_TRC_CF(ڈیٹا, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ورنہ اگر(!cf->connected) {
      نتیجہ = Curl_pollset_سیٹ_برآمد_only(ڈیٹا, ps, ctx->sock);
      CURL_TRC_CF(ڈیٹا, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ورنہ اگر(!ctx->فعال) {
      نتیجہ = Curl_pollset_جوڑو_اندر(ڈیٹا, ps, ctx->sock);
      CURL_TRC_CF(ڈیٹا, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  واپس نتیجہ;
}

#اگر_تعریف_شدہ USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#اگر_غیر_تعریف_شدہ SIO_IDEAL_SEND_BACKLOG_QUERY
#تعریف_کرو SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#اگر_ختم

ساکن خالی win_update_sndbuf_سائز(ساخت Curl_easy *ڈیٹا,
                                   ساخت cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  اگر(curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا),
                        &ctx->آخری_sndbuf_query_at) > 1000) {
    اگر(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, حجم(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_سائز &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (ثابت حرف *)&ideal, حجم(ideal))) {
      ctx->sndbuf_سائز = ideal;
    }
    ctx->آخری_sndbuf_query_at = *Curl_pgrs_now(ڈیٹا);
  }
}

#اگر_ختم /* USE_WINSOCK */

ساکن CURLcode cf_socket_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode نتیجہ = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (خالی)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#اگر_تعریف_شدہ DEBUGBUILD
  /* simulate network blocking/partial writes */
  اگر(ctx->wblock_percent > 0) {
    غیرعلامتی حرف c = 0;
    Curl_rand_بائٹیں(ڈیٹا, FALSE, &c, 1);
    اگر(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(ڈیٹا, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      واپس CURLE_AGAIN;
    }
  }
  اگر(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    اگر(!len)
      len = 1;
    CURL_TRC_CF(ڈیٹا, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#اگر_ختم

#اگر defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  اگر(cf->conn->بٹ.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->بٹ.tcp_fastopen = FALSE;
  }
  ورنہ
#اگر_ختم
    rv = swrite(ctx->sock, buf, len);

  اگر(!curlx_sztouz(rv, pnwritten)) {
    صحیح sockerr = SOCKERRNO;
    اگر(SOCK_EAGAIN(sockerr)
#اگر_غیر_تعریف_شدہ USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#اگر_ختم
      ) {
      نتیجہ = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ورنہ {
      حرف بفر[STRERROR_LEN];
      failf(ڈیٹا, "Send failure: %s",
            curlx_strerror(sockerr, بفر, حجم(بفر)));
      ڈیٹا->state.os_errno = sockerr;
      نتیجہ = CURLE_SEND_ERROR;
    }
  }

#اگر_تعریف_شدہ USE_WINSOCK
  اگر(!نتیجہ)
    win_update_sndbuf_سائز(ڈیٹا, ctx);
#اگر_ختم

  CURL_TRC_CF(ڈیٹا, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (صحیح)نتیجہ, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  واپس نتیجہ;
}

ساکن CURLcode cf_socket_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                               حرف *buf, size_t len, size_t *pnread)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#اگر_تعریف_شدہ DEBUGBUILD
  /* simulate network blocking/partial reads */
  اگر(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    غیرعلامتی حرف c = 0;
    Curl_rand(ڈیٹا, &c, 1);
    اگر(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(ڈیٹا, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      واپس CURLE_AGAIN;
    }
  }
  اگر(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(ڈیٹا, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#اگر_ختم

  rv = sread(ctx->sock, buf, len);

  اگر(!curlx_sztouz(rv, pnread)) {
    صحیح sockerr = SOCKERRNO;
    اگر(SOCK_EAGAIN(sockerr)
#اگر_غیر_تعریف_شدہ USE_WINSOCK
       || (sockerr == SOCKEINTR)
#اگر_ختم
      ) {
      نتیجہ = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ورنہ {
      حرف بفر[STRERROR_LEN];
      failf(ڈیٹا, "Recv failure: %s",
            curlx_strerror(sockerr, بفر, حجم(بفر)));
      ڈیٹا->state.os_errno = sockerr;
      نتیجہ = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(ڈیٹا, cf, "recv(len=%zu) -> %d, %zu", len, (صحیح)نتیجہ, *pnread);
  اگر(!نتیجہ && !ctx->got_پہلا_byte) {
    ctx->پہلا_byte_at = *Curl_pgrs_now(ڈیٹا);
    ctx->got_پہلا_byte = TRUE;
  }
  واپس نتیجہ;
}

ساکن خالی cf_socket_update_ڈیٹا(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا)
{
  /* Update the IP info held in the transfer, if we have that. */
  اگر(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    ساخت cf_socket_ctx *ctx = cf->ctx;
    ڈیٹا->info.primary = ctx->ip;
  }
}

ساکن خالی cf_socket_فعال(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  سیٹ_local_ip(cf, ڈیٹا);
#اگر_تعریف_شدہ USE_IPV6
  اگر(cf->sockindex == FIRSTSOCKET)
    cf->conn->بٹ.ipv6 = (ctx->addr.family == AF_INET6);
#اگر_ختم
  ctx->فعال = TRUE;
}

ساکن CURLcode cf_socket_cntrl(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                صحیح event, صحیح arg1, خالی *arg2)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;

  (خالی)arg1;
  (خالی)arg2;
  انتخاب(event) {
  حالت CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_فعال(cf, ڈیٹا);
    cf_socket_update_ڈیٹا(cf, ڈیٹا);
    توڑو;
  حالت CF_CTRL_ڈیٹا_تیاری:
    cf_socket_update_ڈیٹا(cf, ڈیٹا);
    توڑو;
  حالت CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    توڑو;
  حالت CF_CTRL_REPORT_STATS:
    اگر(cf->connected && !ctx->stats_reported) {
      ساخت curltime *ts = تہی;
      انتخاب(ctx->transport) {
      حالت TRNSPRT_UDP:
      حالت TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        اگر(ctx->got_پہلا_byte)
          ts = &ctx->پہلا_byte_at;
        توڑو;
      طےشدہ:
        ts = &ctx->connected_at;
        توڑو;
      }
      اگر(ts) {
        Curl_pgrs_وقت_Was(ڈیٹا, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  واپس CURLE_OK;
}

ساکن bool cf_socket_conn_is_alive(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    bool *داخلہ_pending)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  ساخت pollfd pfd[1];
  صحیح r;

  *داخلہ_pending = FALSE;

  اگر(!ctx || ctx->sock == CURL_SOCKET_BAD)
    واپس FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  اگر(r < 0) {
    CURL_TRC_CF(ڈیٹا, cf, "is_alive: poll error, assume dead");
    واپس FALSE;
  }
  ورنہ اگر(r == 0) {
    CURL_TRC_CF(ڈیٹا, cf, "is_alive: poll timeout, assume alive");
    واپس TRUE;
  }
  ورنہ اگر(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(ڈیٹا, cf, "is_alive: err/hup/etc events, assume dead");
    واپس FALSE;
  }

  CURL_TRC_CF(ڈیٹا, cf, "is_alive: valid events, looks alive");
  *داخلہ_pending = TRUE;
  واپس TRUE;
}

ساکن CURLcode cf_socket_query(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    واپس CURLE_OK;
  حالت CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    واپس CURLE_OK;
  حالت CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((ثابت ساخت Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : تہی;
    واپس CURLE_OK;
  حالت CF_QUERY_CONNECT_جواب_MS:
    اگر(ctx->got_پہلا_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->پہلا_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < زیادہ_سے_زیادہ_عدد) ? (صحیح)ms : زیادہ_سے_زیادہ_عدد;
    }
    ورنہ
      *pres1 = -1;
    واپس CURLE_OK;
  حالت CF_QUERY_IP_INFO:
#اگر_تعریف_شدہ USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#ورنہ
    *pres1 = FALSE;
#اگر_ختم
    *(ساخت ip_quadruple *)pres2 = ctx->ip;
    واپس CURLE_OK;
  حالت CF_QUERY_REALLY_CONNECTED:
    اگر(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    ورنہ
      *pres1 = ctx->got_پہلا_byte;
    واپس CURLE_OK;
  طےشدہ:
    توڑو;
  }
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساخت Curl_cftype Curl_cft_tcp = {
  "TCP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ڈیٹا_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_tcp_create(ساخت Curl_cfilter **pcf,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_peer *origin,
                            ساخت Curl_peer *peer,
                            uint8_t transport_peer,
                            ساخت connectdata *conn,
                            ساخت Curl_sockaddr_ex *addr,
                            ساخت Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ساخت cf_socket_ctx *ctx = تہی;
  ساخت Curl_cfilter *cf = تہی;
  CURLcode نتیجہ;

  (خالی)ڈیٹا;
  (خالی)origin;
  (خالی)conn;
  (خالی)tunnel_peer;
  (خالی)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  اگر(!addr) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }

  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  نتیجہ = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

برآمد:
  *pcf = (!نتیجہ) ? cf : تہی;
  اگر(نتیجہ) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  واپس نتیجہ;
}

#اگر_تعریف_شدہ __linux__
ساکن خالی linux_quic_mtu(ساخت cf_socket_ctx *ctx)
{
  صحیح val;
  انتخاب(ctx->addr.family) {
#اگر_تعریف_شدہ IP_MTU_DISCOVER
  حالت AF_INET:
    val = IP_PMTUDISC_DO;
    (خالی)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     حجم(val));
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ IPV6_MTU_DISCOVER
  حالت AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (خالی)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     حجم(val));
    توڑو;
#اگر_ختم
  }
}
#ورنہ
#تعریف_کرو linux_quic_mtu(x)
#اگر_ختم

#اگر defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
ساکن خالی linux_quic_gro(ساخت cf_socket_ctx *ctx)
{
  صحیح one = 1;
  (خالی)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)حجم(one));
}
#ورنہ
#تعریف_کرو linux_quic_gro(x)
#اگر_ختم

#اگر (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
ساکن خالی linux_quic_ecn(ساخت cf_socket_ctx *ctx)
{
  غیرعلامتی صحیح tos = 1;
  انتخاب(ctx->addr.family) {
  حالت AF_INET:
    (خالی)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, حجم(tos));
    توڑو;
#اگر_تعریف_شدہ IPV6_RECVTCLASS
  حالت AF_INET6:
    (خالی)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, حجم(tos));
    توڑو;
#اگر_ختم
  }
}
#ورنہ
#تعریف_کرو linux_quic_ecn(x)
#اگر_ختم

#اگر (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
ساکن خالی linux_ip_dontfrag(ساخت cf_socket_ctx *ctx)
{
  صحیح val = 1;

  انتخاب(ctx->addr.family) {
  حالت AF_INET:
    (خالی)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, حجم(val));
    توڑو;
#اگر_تعریف_شدہ IPV6_DONTFRAG
  حالت AF_INET6:
    (خالی)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, حجم(val));
    توڑو;
#اگر_ختم
  }
}
#ورنہ
#تعریف_کرو linux_ip_dontfrag(x)
#اگر_ختم

ساکن CURLcode cf_udp_تیاری_quic(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  صحیح rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  اگر(rc == -1) {
    واپس socket_connect_نتیجہ(ڈیٹا, ctx->ip.remote_ip, SOCKERRNO);
  }
  ctx->sock_connected = TRUE;
  سیٹ_local_ip(cf, ڈیٹا);
  CURL_TRC_CF(ڈیٹا, cf, "%s socket %" FMT_SOCKET_T
              " connected: [%s:%d] -> [%s:%d]",
              (ctx->transport == TRNSPRT_QUIC) ? "QUIC" : "UDP",
              ctx->sock, ctx->ip.local_ip, ctx->ip.local_پورٹ,
              ctx->ip.remote_ip, ctx->ip.remote_پورٹ);

  /* Currently, cf->ctx->sock is always non-blocking because the only
   * caller to cf_udp_setup_quic() is cf_udp_connect() that passes the
   * non-blocking socket created by cf_socket_open() to it. Thus, we
   * do not need to call curlx_nonblock() in cf_udp_setup_quic() anymore.
   */
  linux_quic_ecn(ctx);
  linux_quic_mtu(ctx);
  linux_ip_dontfrag(ctx);
  linux_quic_gro(ctx);

  واپس CURLE_OK;
}

ساکن CURLcode cf_udp_connect(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               bool *مکمل)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_COULDNT_CONNECT;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  *مکمل = FALSE;
  اگر(ctx->sock == CURL_SOCKET_BAD) {
    نتیجہ = cf_socket_open(cf, ڈیٹا);
    اگر(نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "cf_udp_connect(), open failed -> %d",
                  (صحیح)نتیجہ);
      جاؤ برآمد;
    }

    اگر(ctx->transport == TRNSPRT_QUIC) {
      نتیجہ = cf_udp_تیاری_quic(cf, ڈیٹا);
      اگر(نتیجہ)
        جاؤ برآمد;
      CURL_TRC_CF(ڈیٹا, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_پورٹ);
    }
    *مکمل = TRUE;
    cf->connected = TRUE;
  }
برآمد:
  واپس نتیجہ;
}

ساخت Curl_cftype Curl_cft_udp = {
  "UDP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_udp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ڈیٹا_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_udp_create(ساخت Curl_cfilter **pcf,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_peer *origin,
                            ساخت Curl_peer *peer,
                            uint8_t transport_peer,
                            ساخت connectdata *conn,
                            ساخت Curl_sockaddr_ex *addr,
                            ساخت Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ساخت cf_socket_ctx *ctx = تہی;
  ساخت Curl_cfilter *cf = تہی;
  CURLcode نتیجہ;

  (خالی)ڈیٹا;
  (خالی)origin;
  (خالی)conn;
  (خالی)tunnel_peer;
  (خالی)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  نتیجہ = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

برآمد:
  *pcf = (!نتیجہ) ? cf : تہی;
  اگر(نتیجہ) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  واپس نتیجہ;
}

/* this is the TCP filter which can also handle this case */
ساخت Curl_cftype Curl_cft_unix = {
  "UNIX",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ڈیٹا_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_unix_create(ساخت Curl_cfilter **pcf,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_peer *origin,
                            ساخت Curl_peer *peer,
                            uint8_t transport_peer,
                            ساخت connectdata *conn,
                            ساخت Curl_sockaddr_ex *addr,
                            ساخت Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ساخت cf_socket_ctx *ctx = تہی;
  ساخت Curl_cfilter *cf = تہی;
  CURLcode نتیجہ;

  (خالی)ڈیٹا;
  (خالی)origin;
  (خالی)conn;
  (خالی)tunnel_peer;
  (خالی)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  نتیجہ = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

برآمد:
  *pcf = (!نتیجہ) ? cf : تہی;
  اگر(نتیجہ) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  واپس نتیجہ;
}

ساکن timediff_t cf_tcp_accept_timeleft(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
  اگر(ڈیٹا->سیٹ.accepttimeout > 0)
    timeout_ms = ڈیٹا->سیٹ.accepttimeout;
#اگر_ختم

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(ڈیٹا);
  اگر(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  ورنہ {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &ctx->started_at);
    اگر(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  واپس timeout_ms;
}

ساکن خالی cf_tcp_سیٹ_accepted_remote_ip(ساخت Curl_cfilter *cf,
                                          ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
#اگر_تعریف_شدہ HAVE_GETPEERNAME
  CURLcode نتیجہ;
  ساخت Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_پورٹ = 0;
  plen = حجم(ssrem);
  memset(&ssrem, 0, plen);
  اگر(getpeername(ctx->sock, (ساخت sockaddr *)&ssrem, &plen)) {
    حرف بفر[STRERROR_LEN];
    صحیح sockerr = SOCKERRNO;
    failf(ڈیٹا, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, بفر, حجم(بفر)));
    واپس;
  }
  نتیجہ = sockaddr2string((ساخت sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_پورٹ);
  اگر(نتیجہ) {
    failf(ڈیٹا, "ssrem inet_ntop() failed with %d", (صحیح)نتیجہ);
    واپس;
  }
#ورنہ
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_پورٹ = 0;
  (خالی)ڈیٹا;
#اگر_ختم
}

ساکن CURLcode cf_tcp_accept_connect(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      bool *مکمل)
{
  ساخت cf_socket_ctx *ctx = cf->ctx;
  حرف errbuf[STRERROR_LEN];
#اگر_تعریف_شدہ USE_IPV6
  ساخت Curl_sockaddr_storage جوڑو;
#ورنہ
  ساخت sockaddr_in جوڑو;
#اگر_ختم
  curl_socklen_t سائز = (curl_socklen_t)حجم(جوڑو);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  صحیح socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (خالی)ڈیٹا;
  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  *مکمل = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, ڈیٹا);
  اگر(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(ڈیٹا, "Accept timeout occurred while waiting server connect");
    واپس CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(ڈیٹا, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_پورٹ);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(ڈیٹا, cf, "socket_check -> %x", (غیرعلامتی صحیح)socketstate);
  انتخاب(socketstate) {
  حالت -1: /* error */
    /* let's die here */
    failf(ڈیٹا, "Error while waiting for server connect");
    واپس CURLE_FTP_ACCEPT_FAILED;
  طےشدہ:
    اگر(socketstate & CURL_CSELECT_اندر) {
      infof(ڈیٹا, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    توڑو;
  }

  اگر(!incoming) {
    CURL_TRC_CF(ڈیٹا, cf, "nothing heard from the server yet");
    واپس CURLE_OK;
  }

  سائز = حجم(جوڑو);
#اگر_تعریف_شدہ HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (ساخت sockaddr *)&جوڑو, &سائز,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#ورنہ
  s_accepted = CURL_ACCEPT(ctx->sock, (ساخت sockaddr *)&جوڑو, &سائز);
#اگر_ختم

  اگر(s_accepted == CURL_SOCKET_BAD) {
    failf(ڈیٹا, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    واپس CURLE_FTP_ACCEPT_FAILED;
  }
#اگر_غیر_تعریف_شدہ HAVE_ACCEPT4
#اگر_تعریف_شدہ HAVE_FCNTL
  اگر(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(ڈیٹا, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    Curl_socket_close(ڈیٹا, cf->conn, s_accepted);
    واپس CURLE_FTP_ACCEPT_FAILED;
  }
#اگر_ختم /* HAVE_FCNTL */
  اگر(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(ڈیٹا, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, حجم(errbuf)));
    Curl_socket_close(ڈیٹا, cf->conn, s_accepted);
    واپس CURLE_FTP_ACCEPT_FAILED;
  }
#اگر_ختم /* !HAVE_ACCEPT4 */
  infof(ڈیٹا, "Connection accepted from server");

  /* Replace any filter on SECONDARY with one listening on this socket */
  ctx->listening = FALSE;
  ctx->accepted = TRUE;
  socket_close(ڈیٹا, cf->conn, TRUE, ctx->sock);
  ctx->sock = s_accepted;

  cf->conn->sock[cf->sockindex] = ctx->sock;
  cf_tcp_سیٹ_accepted_remote_ip(cf, ڈیٹا);
  سیٹ_local_ip(cf, ڈیٹا);
  ctx->فعال = TRUE;
  ctx->connected_at = *Curl_pgrs_now(ڈیٹا);
  cf->connected = TRUE;
  CURL_TRC_CF(ڈیٹا, cf, "accepted_set(sock=%" FMT_SOCKET_T
              ", remote=%s port=%d)",
              ctx->sock, ctx->ip.remote_ip, ctx->ip.remote_پورٹ);

  اگر(ڈیٹا->سیٹ.fsockopt) {
    ساخت Curl_mapi_guard guard;
    صحیح error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fsockopt);
    error = ڈیٹا->سیٹ.fsockopt(ڈیٹا->سیٹ.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_انتہا(&guard);

    اگر(error)
      واپس CURLE_ABORTED_BY_CALLBACK;
  }
  *مکمل = TRUE;
  واپس CURLE_OK;
}

ساخت Curl_cftype Curl_cft_tcp_accept = {
  "TCP-ACCEPT",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_accept_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ڈیٹا_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_conn_tcp_listen_سیٹ(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode نتیجہ;
  ساخت Curl_cfilter *cf = تہی;
  ساخت cf_socket_ctx *ctx = تہی;

  /* replace any existing */
  Curl_conn_cf_discard_all(ڈیٹا, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  نتیجہ = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;
  Curl_conn_cf_جوڑو(ڈیٹا, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(ڈیٹا);
  conn->sock[sockindex] = ctx->sock;
  سیٹ_local_ip(cf, ڈیٹا);
  CURL_TRC_CF(ڈیٹا, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_پورٹ);

برآمد:
  اگر(نتیجہ) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  واپس نتیجہ;
}

bool Curl_conn_is_tcp_listen(ساخت Curl_easy *ڈیٹا,
                             int8_t sockindex)
{
  ساخت Curl_cfilter *cf = ڈیٹا->conn->cfilter[sockindex];
  جب(cf) {
    اگر(cf->cft == &Curl_cft_tcp_accept)
      واپس TRUE;
    cf = cf->اگلا;
  }
  واپس FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
ساکن bool cf_is_socket(ساخت Curl_cfilter *cf)
{
  واپس cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             curl_socket_t *psock,
                             ثابت ساخت Curl_sockaddr_ex **paddr,
                             ساخت ip_quadruple *pip)
{
  (خالی)ڈیٹا;
  اگر(cf_is_socket(cf) && cf->ctx) {
    ساخت cf_socket_ctx *ctx = cf->ctx;

    اگر(psock)
      *psock = ctx->sock;
    اگر(paddr)
      *paddr = &ctx->addr;
    اگر(pip)
      *pip = ctx->ip;
    واپس CURLE_OK;
  }
  واپس CURLE_FAILED_INIT;
}
