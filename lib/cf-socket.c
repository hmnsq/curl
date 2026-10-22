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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h> /* <netinet/tcp.h> may need it */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_UN_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/un.h> /* for sockaddr_un */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LINUX_TCP_H
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/tcp.h>
#ਜਾਂਜੇ defined(HAVE_NETINET_TCP_H)
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/tcp.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_UDP_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/udp.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_IP_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/ip.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_IFADDRS_H
#ਅਨ੍ਤਰ੍ਭਾਵ <ifaddrs.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NET_IF_H
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "if2ip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-socket.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_ntop.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "conncache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multihandle.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sockaddr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/nonblock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/version_win32.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/wait.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#ਜੇ_ਅੰਤ

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(ਬਣਤਰ sockaddr *sa, curl_socklen_t salen,
                                  ਅੱਖਰ *addr, uint16_t *ਪੋਰਟ);
UNITTEST CURLcode sockaddr2string(ਬਣਤਰ sockaddr *sa, curl_socklen_t salen,
                                  ਅੱਖਰ *addr, uint16_t *ਪੋਰਟ)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ sockaddr_in *si = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ sockaddr_in6 *si6 = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  ਬਣਤਰ sockaddr_un *su = ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)salen;
#ਜੇ_ਅੰਤ

  ਚੋਣ(sa->sa_family) {
  ਹਾਲਤ AF_INET:
    si = (ਬਣਤਰ sockaddr_in *)(ਖਾਲੀ *)sa;
    ਨਤੀਜਾ = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    ਜੇ(!ਨਤੀਜਾ) {
      *ਪੋਰਟ = ntohs(si->sin_port);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਹਾਲਤ AF_INET6:
    si6 = (ਬਣਤਰ sockaddr_in6 *)(ਖਾਲੀ *)sa;
    ਨਤੀਜਾ = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    ਜੇ(!ਨਤੀਜਾ) {
      *ਪੋਰਟ = ntohs(si6->sin6_port);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  ਹਾਲਤ AF_UNIX:
    ਜੇ(salen > (curl_socklen_t)ਆਕਾਰ(CURL_SA_FAMILY_T)) {
      su = (ਬਣਤਰ sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_ਰਸਤਾ);
    }
    ਨਹੀਂਤਾਂ
      addr[0] = 0; /* socket with no name */
    *ਪੋਰਟ = 0;
    ਵਾਪਸ CURLE_OK;
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਨਤੀਜਾ = CURLE_UNSUPPORTED_PROTOCOL;
    ਰੋਕੋ;
  }

  addr[0] = '\0';
  *ਪੋਰਟ = 0;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ tcpnodelay(ਬਣਤਰ Curl_cfilter *cf,
                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       curl_socket_t sockfd)
{
#ਜੇ defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  ਪੂਰਨ level = IPPROTO_TCP;
  VERBOSE(ਅੱਖਰ ਬਫਰ[STRERROR_LEN]);

  ਜੇ(setsockopt(sockfd, level, TCP_NODELAY, (ਖਾਲੀ *)&onoff, ਆਕਾਰ(onoff)) < 0)
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)sockfd;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_ਹੱਦ) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_ਹੱਦ)
#ਜੇ defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
ਸਥਿਰ ਖਾਲੀ alive_unit(ਪੂਰਨ *x)
{
  /* make sure this doesn't wrap */
  ਜੇ(*x < (ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ / 1000))
    *x *= 1000;
  ਨਹੀਂਤਾਂ
    /* this is still almost 25 days */
    *x = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ alive_unit(x)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ tcpkeepalive(ਬਣਤਰ Curl_cfilter *cf,
                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         curl_socket_t sockfd)
{
  ਪੂਰਨ optval = ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  ਜੇ(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (ਖਾਲੀ *)&optval, ਆਕਾਰ(optval)) < 0) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    ਜੇ(curlx_verify_windows_ਸੰਸਕਰਣ(10, 0, 16299, PLATFORM_WINNT,
                                    ਸੰਸਕਰਣ_GREATER_THAN_EQUAL)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "Set TCP_KEEP* on fd=%" FMT_SOCKET_T, sockfd);
      optval = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepidle);
/* Offered by mingw-w64 v12+, MS SDK 6.0A/VS2008+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TCP_KEEPALIVE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCP_KEEPALIVE 3
#ਜੇ_ਅੰਤ
/* Offered by mingw-w64 v12+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TCP_KEEPCNT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCP_KEEPCNT 16
#ਜੇ_ਅੰਤ
/* Offered by mingw-w64 v12+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TCP_KEEPIDLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCP_KEEPIDLE TCP_KEEPALIVE
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TCP_KEEPINTVL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCP_KEEPINTVL 17
#ਜੇ_ਅੰਤ
      ਜੇ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (ਅਚਲ ਅੱਖਰ *)&optval, ਆਕਾਰ(optval)) < 0) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepintvl);
      ਜੇ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (ਅਚਲ ਅੱਖਰ *)&optval, ਆਕਾਰ(optval)) < 0) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepcnt);
      ਜੇ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (ਅਚਲ ਅੱਖਰ *)&optval, ਆਕਾਰ(optval)) < 0) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    ਨਹੀਂਤਾਂ {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIO_KEEPALIVE_VALS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      ਬਣਤਰ tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#ਜੇ_ਅੰਤ
      ਬਣਤਰ tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      ਜੇ(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, ਆਕਾਰ(vals),
                  ਨਿਰਾ, 0, &dummy, ਨਿਰਾ, ਨਿਰਾ) != 0) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#ਨਹੀਂਤਾਂ /* !USE_WINSOCK */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TCP_KEEPIDLE
    optval = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepidle);
    alive_unit(&optval);
    ਜੇ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (ਖਾਲੀ *)&optval, ਆਕਾਰ(optval)) < 0) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ਜਾਂਜੇ defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepidle);
    alive_unit(&optval);
    ਜੇ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (ਖਾਲੀ *)&optval, ਆਕਾਰ(optval)) < 0) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ਜਾਂਜੇ defined(TCP_KEEPALIVE_ਹੱਦ)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepidle);
    alive_unit(&optval);
    ਜੇ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ਹੱਦ,
                  (ਖਾਲੀ *)&optval, ਆਕਾਰ(optval)) < 0) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TCP_KEEPINTVL
    optval = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepintvl);
    alive_unit(&optval);
    ਜੇ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (ਖਾਲੀ *)&optval, ਆਕਾਰ(optval)) < 0) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ਜਾਂਜੇ defined(TCP_KEEPALIVE_ABORT_ਹੱਦ)
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
      ਪੂਰਨ keepcnt = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepcnt);
      ਪੂਰਨ keepintvl = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepintvl);

      ਜੇ(keepcnt > 0 && keepintvl > (ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ / keepcnt))
        optval = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
      ਨਹੀਂਤਾਂ
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    ਜੇ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_ਹੱਦ,
                  (ਖਾਲੀ *)&optval, ਆਕਾਰ(optval)) < 0) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TCP_KEEPCNT
    optval = curlx_sltosi(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepcnt);
    ਜੇ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (ਖਾਲੀ *)&optval, ਆਕਾਰ(optval)) < 0) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to set TCP_KEEPCNT on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* USE_WINSOCK */
  }
}

/**
 * Assign the addrinfo `ai` to the Curl_sockaddr_ex `addr` with
 * transport determining socktype and protocol.
 */
CURLcode Curl_socket_addr_from_ai(ਬਣਤਰ Curl_sockaddr_ex *addr,
                                  ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai,
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
  addr->addrlen = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= ਆਕਾਰ(addr->curl_sa_addrbuf));
  ਜੇ(addr->addrlen > ਆਕਾਰ(addr->curl_sa_addrbuf))
    ਵਾਪਸ CURLE_TOO_LARGE;

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SO_NOSIGPIPE
ਪੂਰਨ Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  ਪੂਰਨ onoff = 1;
  ਵਾਪਸ setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (ਖਾਲੀ *)&onoff, ਆਕਾਰ(onoff));
}
#ਜੇ_ਅੰਤ /* USE_SO_NOSIGPIPE */

#ਜੇ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ਸਥਿਰ uint32_t get_scope_id(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  ਜੇ(ਅੰਕੜੇ->conn->scope_id)
    ਵਾਪਸ ਅੰਕੜੇ->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  ਜੇ(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#ਜੇ defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    ਬਣਤਰ ifaddrs *ifa, *ifa_list;
    ਜੇ(getifaddrs(&ifa_list) == 0) {
      ਲਈ(ifa = ifa_list; ifa; ifa = ifa->ifa_ਅਗਲਾ) {
        ਜੇ(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          ਬਣਤਰ sockaddr_in6 *s6 = (ਖਾਲੀ *)ifa->ifa_addr;
          ਜੇ(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(ਅੰਕੜੇ,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)scope_id);
            ਰੋਕੋ;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#ਜੇ_ਅੰਤ /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  ਵਾਪਸ scope_id;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode socket_open(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  ਅੱਖਰ errbuf[STRERROR_LEN];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#ਜੇ_ਅੰਤ

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    ਬਣਤਰ Curl_mapi_guard guard;
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fopensocket);
    *sockfd = ਅੰਕੜੇ->ਸੈੱਟ.fopensocket(ਅੰਕੜੇ->ਸੈੱਟ.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (ਬਣਤਰ curl_sockaddr *)addr);
    CURL_CBAPI_ਸਿਰਾ(&guard);
  }
  ਨਹੀਂਤਾਂ {
    /* opensocket callback not set, so create the socket now */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    ਜੇ((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(ਅੰਕੜੇ, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }
#ਜੇ_ਅੰਤ
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    ਜੇ((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(ਅੰਕੜੇ, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, ਆਕਾਰ(errbuf)));
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SO_NOSIGPIPE
  ਜੇ(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(ਅੰਕੜੇ, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, ਆਕਾਰ(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }
#ਜੇ_ਅੰਤ /* USE_SO_NOSIGPIPE */

#ਜੇ defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  ਜੇ(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(ਅੰਕੜੇ, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, ਆਕਾਰ(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  ਜੇ(addr->family == AF_INET6) {
    ਬਣਤਰ sockaddr_in6 * ਅਚਲ sa6 = (ਖਾਲੀ *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(ਅੰਕੜੇ, sa6);
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
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
CURLcode Curl_socket_open(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai,
                          ਬਣਤਰ Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  ਬਣਤਰ Curl_sockaddr_ex dummy;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  ਨਤੀਜਾ = Curl_socket_addr_from_ai(addr, ai, transport);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਵਾਪਸ socket_open(ਅੰਕੜੇ, addr, sockfd);
}

ਸਥਿਰ ਪੂਰਨ socket_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                        ਪੂਰਨ use_callback, curl_socket_t sock)
{
  ਜੇ(sock == CURL_SOCKET_BAD)
    ਵਾਪਸ 0;

  ਜੇ(use_callback && conn && conn->fclosesocket) {
    ਬਣਤਰ Curl_mapi_guard guard;
    ਪੂਰਨ rc;
    Curl_multi_will_close(ਅੰਕੜੇ, sock);
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_ਸਿਰਾ(&guard);
    ਵਾਪਸ rc;
  }

  ਜੇ(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(ਅੰਕੜੇ, sock);

  sclose(sock);

  ਵਾਪਸ 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
ਪੂਰਨ Curl_socket_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                      curl_socket_t sock)
{
  ਵਾਪਸ socket_close(ਅੰਕੜੇ, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼,
                              ਅੱਖਰ **dev, ਅੱਖਰ **iface, ਅੱਖਰ **host)
{
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ if_ਅਗੇਤਰ[] = "if!";
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ host_ਅਗੇਤਰ[] = "host!";
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ if_host_ਅਗੇਤਰ[] = "ifhost!";
  size_t len;

  len = ਲੜੀ_ਲੰਬਾਈ(ਨਿਵੇਸ਼);
  ਜੇ(len > 512)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(dev)
    *dev = ਨਿਰਾ;
  ਜੇ(iface)
    *iface = ਨਿਰਾ;
  ਜੇ(host)
    *host = ਨਿਰਾ;

  ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(if_ਅਗੇਤਰ, ਨਿਵੇਸ਼, ਲੜੀ_ਲੰਬਾਈ(if_ਅਗੇਤਰ))) {
    ਨਿਵੇਸ਼ += ਲੜੀ_ਲੰਬਾਈ(if_ਅਗੇਤਰ);
    ਜੇ(!*ਨਿਵੇਸ਼)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜੇ(iface) {
      *iface = curlx_memdup0(ਨਿਵੇਸ਼, len - ਲੜੀ_ਲੰਬਾਈ(if_ਅਗੇਤਰ));
      ਵਾਪਸ *iface ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਵਾਪਸ CURLE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(host_ਅਗੇਤਰ, ਨਿਵੇਸ਼, ਲੜੀ_ਲੰਬਾਈ(host_ਅਗੇਤਰ))) {
    ਨਿਵੇਸ਼ += ਲੜੀ_ਲੰਬਾਈ(host_ਅਗੇਤਰ);
    ਜੇ(!*ਨਿਵੇਸ਼)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜੇ(host) {
      *host = curlx_memdup0(ਨਿਵੇਸ਼, len - ਲੜੀ_ਲੰਬਾਈ(host_ਅਗੇਤਰ));
      ਵਾਪਸ *host ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਵਾਪਸ CURLE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(if_host_ਅਗੇਤਰ, ਨਿਵੇਸ਼, ਲੜੀ_ਲੰਬਾਈ(if_host_ਅਗੇਤਰ))) {
    ਅਚਲ ਅੱਖਰ *host_part;
    ਨਿਵੇਸ਼ += ਲੜੀ_ਲੰਬਾਈ(if_host_ਅਗੇਤਰ);
    len -= ਲੜੀ_ਲੰਬਾਈ(if_host_ਅਗੇਤਰ);
    host_part = memchr(ਨਿਵੇਸ਼, '!', len);
    ਜੇ(!host_part || !*(host_part + 1))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜੇ(iface) {
      *iface = curlx_memdup0(ਨਿਵੇਸ਼, host_part - ਨਿਵੇਸ਼);
      ਜੇ(!*iface)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ++host_part;
    ਜੇ(host) {
      *host = curlx_memdup0(host_part, len - (host_part - ਨਿਵੇਸ਼));
      ਜੇ(!*host) {
        ਜੇ(iface)
          curlx_safefree(*iface);
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
    }
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(!*ਨਿਵੇਸ਼)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਜੇ(dev) {
    *dev = curlx_memdup0(ਨਿਵੇਸ਼, len);
    ਵਾਪਸ *dev ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BINDLOCAL
ਸਥਿਰ CURLcode bindlocal(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                          curl_socket_t sockfd, ਪੂਰਨ af, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ scope,
                          uint8_t transport)
{
  ਬਣਤਰ Curl_sockaddr_storage sa;
  ਬਣਤਰ sockaddr *sock = (ਬਣਤਰ sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  ਬਣਤਰ sockaddr_in *si4 = (ਬਣਤਰ sockaddr_in *)&sa;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ sockaddr_in6 *si6 = (ਬਣਤਰ sockaddr_in6 *)&sa;
#ਜੇ_ਅੰਤ
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *h = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਪੋਰਟ = ਅੰਕੜੇ->ਸੈੱਟ.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  ਪੂਰਨ portnum = ਅੰਕੜੇ->ਸੈੱਟ.localportrange;
  ਅੱਖਰ *dev_ਵਿੱਚ = ਨਿਰਾ, *iface_ਵਿੱਚ = ਨਿਰਾ, *host_ਵਿੱਚ = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *iface = ਨਿਰਾ, *host = ਨਿਰਾ;
  ਪੂਰਨ sockerr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ IP_BIND_ADDRESS_NO_PORT
  ਪੂਰਨ on = 1;
#ਜੇ_ਅੰਤ
  CURLcode ਨਤੀਜਾ = CURLE_OK;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  (ਖਾਲੀ)scope;
#ਜੇ_ਅੰਤ
  ਜੇ(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    ਨਤੀਜਾ = Curl_parse_interface(conn->interface_opts,
                                  &dev_ਵਿੱਚ, &iface_ਵਿੱਚ, &host_ਵਿੱਚ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    /* This is how we use the values */
    iface = iface_ਵਿੱਚ ? iface_ਵਿੱਚ : dev_ਵਿੱਚ;
    host = host_ਵਿੱਚ ? host_ਵਿੱਚ : dev_ਵਿੱਚ;
  }

  ਜੇ(!iface && !host && !ਪੋਰਟ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(iface && (ਲੜੀ_ਲੰਬਾਈ(iface) >= 255)) {
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  memset(&sa, 0, ਆਕਾਰ(ਬਣਤਰ Curl_sockaddr_storage));

  ਜੇ(iface || host) {
    ਅੱਖਰ myhost[256] = "";
    ਪੂਰਨ ਪੂਰਾ = 0; /* -1 for error, 1 for address found */
    if2ip_ਨਤੀਜਾ_t if2ip_ਨਤੀਜਾ = IF2_IP_NOT_ਮਿਲਿਆ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SO_BINDTODEVICE
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
    ਜੇ(iface &&
       setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                  iface, (curl_socklen_t)ਲੜੀ_ਲੰਬਾਈ(iface) + 1) == 0 &&
       /* This is often "errno 1, error: Operation not permitted" if you are
        * not running as root or another suitable privileged user. If it
        * succeeds it means the parameter was a valid interface and not an IP
        * address. Return immediately.
        */
       !host_ਵਿੱਚ) {
      infof(ਅੰਕੜੇ, "socket successfully bound to interface '%s'", iface);
      ਨਤੀਜਾ = CURLE_OK;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
#ਜੇ_ਅੰਤ
    ਜੇ(!host_ਵਿੱਚ) {
      /* Discover IP from input device, then bind to it */
      if2ip_ਨਤੀਜਾ = Curl_if2ip(af,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
                                scope, conn->scope_id,
#ਜੇ_ਅੰਤ
                                iface, myhost, ਆਕਾਰ(myhost));
    }
    ਚੋਣ(if2ip_ਨਤੀਜਾ) {
    ਹਾਲਤ IF2_IP_NOT_ਮਿਲਿਆ:
      ਜੇ(iface_ਵਿੱਚ && !host_ਵਿੱਚ) {
        /* Do not fall back to treating it as a hostname */
        ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
        ਅੰਕੜੇ->state.os_errno = sockerr = SOCKERRNO;
        failf(ਅੰਕੜੇ, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
        ਨਤੀਜਾ = CURLE_INTERFACE_FAILED;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਰੋਕੋ;
    ਹਾਲਤ IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      ਨਤੀਜਾ = CURLE_UNSUPPORTED_PROTOCOL;
      ਜਾਓ ਬਾਹਰਲਾ;
    ਹਾਲਤ IF2_IP_ਮਿਲਿਆ:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(ਅੰਕੜੇ, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      ਪੂਰਾ = 1;
      ਰੋਕੋ;
    }
    ਜੇ(!iface_ਵਿੱਚ || host_ਵਿੱਚ) {
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
      ਜੇ(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#ਜੇ_ਅੰਤ

      (ਖਾਲੀ)Curl_resolv_blocking(ਅੰਕੜੇ, dns_queries, host, 80, transport, &h);
      ਜੇ(h) {
        ਪੂਰਨ h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_ਪਤਾ(h->addr, myhost, ਆਕਾਰ(myhost));
        infof(ਅੰਕੜੇ, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_ਇੰਦਰਾਜ_unlink(ਅੰਕੜੇ, &h); /* this will NULL, potential free h */
        ਜੇ(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          ਨਤੀਜਾ = CURLE_UNSUPPORTED_PROTOCOL;
          ਜਾਓ ਬਾਹਰਲਾ;
        }
        ਪੂਰਾ = 1;
      }
      ਨਹੀਂਤਾਂ {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        ਪੂਰਾ = -1;
      }
    }

    ਜੇ(ਪੂਰਾ > 0) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
      /* IPv6 address */
      ਜੇ(af == AF_INET6) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        ਅੱਖਰ *scope_ptr = strchr(myhost, '%');
        ਜੇ(scope_ptr)
          *(scope_ptr++) = '\0';
#ਜੇ_ਅੰਤ
        ਜੇ(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(ਪੋਰਟ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          ਜੇ(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            ਜੇ(curlx_str_ਸੰਖਿਆ((ਅਚਲ ਅੱਖਰ **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              ਨਤੀਜਾ = CURLE_UNSUPPORTED_PROTOCOL;
              ਜਾਓ ਬਾਹਰਲਾ;
            }
            si6->sin6_scope_id = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)scope_id;
          }
#ਜੇ_ਅੰਤ
        }
        sizeof_sa = ਆਕਾਰ(ਬਣਤਰ sockaddr_in6);
      }
      ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
      /* IPv4 address */
      ਜੇ((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(ਪੋਰਟ);
        sizeof_sa = ਆਕਾਰ(ਬਣਤਰ sockaddr_in);
      }
    }

    ਜੇ(ਪੂਰਾ < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      ਅੰਕੜੇ->state.errorbuf = FALSE;
      ਅੰਕੜੇ->state.os_errno = sockerr = SOCKERRNO;
      failf(ਅੰਕੜੇ, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਨਤੀਜਾ = CURLE_INTERFACE_FAILED;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਹੀਂਤਾਂ {
    /* no device was given, prepare sa to match af's needs */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਜੇ(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(ਪੋਰਟ);
      sizeof_sa = ਆਕਾਰ(ਬਣਤਰ sockaddr_in6);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(ਪੋਰਟ);
      sizeof_sa = ਆਕਾਰ(ਬਣਤਰ sockaddr_in);
    }
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ IP_BIND_ADDRESS_NO_PORT
  (ਖਾਲੀ)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, ਆਕਾਰ(on));
#ਜੇ_ਅੰਤ
  ਲਈ(;;) {
    ਜੇ(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(ਅੰਕੜੇ, "Local port: %hu", ਪੋਰਟ);
      conn->ਬਿੱਟ.bound = TRUE;
      ਨਤੀਜਾ = CURLE_OK;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(--portnum > 0) {
      ਪੋਰਟ++; /* try next port */
      ਜੇ(ਪੋਰਟ == 0)
        ਰੋਕੋ;
      infof(ਅੰਕੜੇ, "Bind to local port %d failed, trying next", ਪੋਰਟ - 1);
      /* We reuse/clobber the port variable here below */
      ਜੇ(sock->sa_family == AF_INET)
        si4->sin_port = htons(ਪੋਰਟ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
      ਨਹੀਂਤਾਂ
        si6->sin6_port = htons(ਪੋਰਟ);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
      ਰੋਕੋ;
  }
  {
    ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
    ਅੰਕੜੇ->state.os_errno = sockerr = SOCKERRNO;
    failf(ਅੰਕੜੇ, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
  }

  ਨਤੀਜਾ = CURLE_INTERFACE_FAILED;

ਬਾਹਰਲਾ:
  curlx_free(dev_ਵਿੱਚ);
  curlx_free(iface_ਵਿੱਚ);
  curlx_free(host_ਵਿੱਚ);
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
ਸਥਿਰ bool verifyconnect(curl_socket_t sockfd, ਪੂਰਨ *psockerr)
{
  bool rc = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SO_ERROR
  ਪੂਰਨ sockerr = 0;
  curl_socklen_t err_ਮਾਪ = ਆਕਾਰ(sockerr);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
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
#ਜੇ_ਅੰਤ

  ਜੇ(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (ਖਾਲੀ *)&sockerr, &err_ਮਾਪ))
    sockerr = SOCKERRNO;
#ਜੇ defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  ਜੇ(EBADIOCTL == sockerr) {
    ਸੈੱਟ_SOCKERRNO(0);
    sockerr = 0;
  }
#ਜੇ_ਅੰਤ
  ਜੇ((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  ਨਹੀਂਤਾਂ
    /* This was not a successful connect */
    rc = FALSE;
  ਜੇ(psockerr)
    *psockerr = sockerr;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)sockfd;
  ਜੇ(psockerr)
    *psockerr = SOCKERRNO;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
ਸਥਿਰ CURLcode socket_connect_ਨਤੀਜਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਅਚਲ ਅੱਖਰ *ipaddress, ਪੂਰਨ sockerr)
{
  ਜੇ(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    ਵਾਪਸ CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(ਅੱਖਰ ਬਫਰ[STRERROR_LEN]);
    infof(ਅੰਕੜੇ, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    NOVERBOSE((ਖਾਲੀ)ipaddress);
  }
  ਅੰਕੜੇ->state.os_errno = sockerr;
  /* connect failed */
  ਵਾਪਸ CURLE_COULDNT_CONNECT;
}

ਬਣਤਰ cf_socket_ctx {
  ਬਣਤਰ Curl_peer *peer;
  ਬਣਤਰ Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  ਬਣਤਰ ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  ਬਣਤਰ curltime started_at;        /* when socket was created */
  ਬਣਤਰ curltime connected_at;      /* when socket connected/got first byte */
  ਬਣਤਰ curltime ਪਹਿਲਾ_byte_at;     /* when first byte was recvd */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  ਬਣਤਰ curltime ਆਖਰੀ_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_ਮਾਪ;                     /* the last set SO_SNDBUF size */
#ਜੇ_ਅੰਤ
  ਪੂਰਨ sockerr;                       /* socket error of last failure or 0 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਪੂਰਨ connect_wait_ms;               /* ms to wait after 1st connect() */
  ਪੂਰਨ wblock_percent;                /* percent of writes doing EAGAIN */
  ਪੂਰਨ wpartial_percent;              /* percent of bytes written in send */
  ਪੂਰਨ rblock_percent;                /* percent of reads doing EAGAIN */
  size_t recv_max;                   /* max enforced read size */
#ਜੇ_ਅੰਤ
  uint8_t transport;
  ਦੋਆਧਾਰੀ_ਅੰਕ(got_ਪਹਿਲਾ_byte);               /* if first byte was received */
  ਦੋਆਧਾਰੀ_ਅੰਕ(listening);                    /* socket is listening */
  ਦੋਆਧਾਰੀ_ਅੰਕ(accepted);                     /* socket was accepted, not connected */
  ਦੋਆਧਾਰੀ_ਅੰਕ(sock_connected);               /* socket is "connected", e.g. in UDP */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਸਰਗਰਮ);
  ਦੋਆਧਾਰੀ_ਅੰਕ(stats_reported);
};

ਸਥਿਰ CURLcode cf_socket_ctx_init(ਬਣਤਰ cf_socket_ctx *ctx,
                                   ਬਣਤਰ Curl_peer *peer,
                                   ਬਣਤਰ Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, ਆਕਾਰ(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  {
    ਅਚਲ ਅੱਖਰ *p = getenv("CURL_DBG_SOCK_WBLOCK");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, 100))
        ctx->wblock_percent = (ਪੂਰਨ)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, 100))
        ctx->wpartial_percent = (ਪੂਰਨ)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, 100))
        ctx->rblock_percent = (ਪੂਰਨ)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ))
        ctx->connect_wait_ms = (ਪੂਰਨ)l;
    }
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cf_socket_ctx_free(ਬਣਤਰ cf_socket_ctx *ctx)
{
  ਜੇ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

ਸਥਿਰ CURLcode cf_socket_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   bool *ਪੂਰਾ)
{
  ਜੇ(cf->connected) {
    ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    ਜੇ(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ buf[1024];
      (ਖਾਲੀ)sread(ctx->sock, buf, ਆਕਾਰ(buf));
    }
  }
  *ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cf_socket_destroy(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "destroy");
  ਜੇ(ctx) {
    ਜੇ(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      ਜੇ(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(ਅੰਕੜੇ, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_local_ip(ਬਣਤਰ Curl_cfilter *cf,
                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_ਪੋਰਟ = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GETSOCKNAME
  ਜੇ((ctx->sock != CURL_SOCKET_BAD) &&
     !(ਅੰਕੜੇ->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    ਬਣਤਰ Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = ਆਕਾਰ(ਬਣਤਰ Curl_sockaddr_storage);

    memset(&ssloc, 0, ਆਕਾਰ(ssloc));
    ਜੇ(getsockname(ctx->sock, (ਬਣਤਰ sockaddr *)&ssloc, &slen)) {
      VERBOSE(ਅੱਖਰ ਬਫਰ[STRERROR_LEN]);
      VERBOSE(ਪੂਰਨ sockerr = SOCKERRNO);
      infof(ਅੰਕੜੇ, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    }
    ਨਹੀਂਤਾਂ {
      CURLcode ਨਤੀਜਾ = sockaddr2string((ਬਣਤਰ sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_ਪੋਰਟ);
      ਜੇ(ਨਤੀਜਾ)
        infof(ਅੰਕੜੇ, "ssloc inet_ntop() failed with %d", (ਪੂਰਨ)ਨਤੀਜਾ);
    }
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ CURLcode ਸੈੱਟ_remote_ip(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  ਨਤੀਜਾ = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_ਪੋਰਟ);
  ਜੇ(ਨਤੀਜਾ) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(ਅੰਕੜੇ, "curl_sa_addr inet_ntop() failed with %d", (ਪੂਰਨ)ਨਤੀਜਾ);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ਵਾਪਸ CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
ਸਥਿਰ ਪੂਰਨ cf_socktype(ਪੂਰਨ x)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ x;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIO_TCP_INITIAL_RTO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#ਜੇ_ਅੰਤ /* SIO_TCP_INITIAL_RTO */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#ਜੇ_ਅੰਤ

ਸਥਿਰ bool targets_localhost(ਬਣਤਰ cf_socket_ctx *ctx)
{
  ਵਾਪਸ (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
ਸਥਿਰ ਖਾਲੀ tcplocalhost(ਬਣਤਰ Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  ਜੇ(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD ਬਾਈਟਾਂ = 0;
    memset(&rto, 0, ਆਕਾਰ(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    ਜੇ(curlx_verify_windows_ਸੰਸਕਰਣ(10, 0, 16299, PLATFORM_WINNT,
                                    ਸੰਸਕਰਣ_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    ਨਹੀਂਤਾਂ
      rto.MaxSynRetransmissions = 1;
    (ਖਾਲੀ)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, ਆਕਾਰ(rto),
                   ਨਿਰਾ, 0, &ਬਾਈਟਾਂ, ਨਿਰਾ, ਨਿਰਾ);
  }
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ tcplocalhost(x, y)
#ਜੇ_ਅੰਤ /* _WIN32 */

ਸਥਿਰ CURLcode cf_socket_open(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  ਪੂਰਨ error = 0;
  bool isconnected = FALSE;
  CURLcode ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
  bool is_tcp;

  DEBUGASSERT(ctx->sock == CURL_SOCKET_BAD);
  ctx->started_at = *Curl_pgrs_now(ਅੰਕੜੇ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_USE_SOCK_NONBLOCK
  /* Do not tuck SOCK_NONBLOCK into socktype when opensocket callback is set
   * because we would not know how socktype is about to be used in the
   * callback, SOCK_NONBLOCK might get factored out before calling socket().
   */
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#ਜੇ_ਅੰਤ
  ਨਤੀਜਾ = socket_open(ਅੰਕੜੇ, &ctx->addr, &ctx->sock);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#ਜੇ_ਅੰਤ
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = ਸੈੱਟ_remote_ip(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਜੇ(ctx->addr.family == AF_INET6) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    ਪੂਰਨ on = 0;
    (ਖਾਲੀ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (ਖਾਲੀ *)&on, ਆਕਾਰ(on));
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      ਬਣਤਰ sockaddr_in6 *sa6 = (ਖਾਲੀ *)&ctx->addr.curl_sa_addr;
      ਜੇ(sa6->sin6_scope_id)
        infof(ਅੰਕੜੇ, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_ਪੋਰਟ,
              (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)sa6->sin6_scope_id);
      ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
        infof(ਅੰਕੜੇ, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_ਪੋਰਟ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    infof(ਅੰਕੜੇ, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_ਪੋਰਟ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#ਨਹੀਂਤਾਂ
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#ਜੇ_ਅੰਤ
  ਜੇ(is_tcp) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.tcp_nodelay)
      tcpnodelay(cf, ਅੰਕੜੇ, ctx->sock);

    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.tcp_keepalive)
      tcpkeepalive(cf, ਅੰਕੜੇ, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fsockopt) {
    /* activate callback for setting socket options */
    ਬਣਤਰ Curl_mapi_guard guard;
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fsockopt);
    error = ਅੰਕੜੇ->ਸੈੱਟ.fsockopt(ਅੰਕੜੇ->ਸੈੱਟ.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_ਸਿਰਾ(&guard);

    ਜੇ(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ(error) {
      ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  ਜੇ(ctx->addr.family == AF_INET
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
     || ctx->addr.family == AF_INET6
#ਜੇ_ਅੰਤ
    ) {
    ਨਤੀਜਾ = bindlocal(ਅੰਕੜੇ, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
      }
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  ਜੇ(error < 0) {
    ਨਤੀਜਾ = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਨਹੀਂਤਾਂ
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    ਜੇ(error < 0) {
      ਨਤੀਜਾ = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
#ਜੇ_ਅੰਤ
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(ਅੰਕੜੇ, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(isconnected) {
    ਸੈੱਟ_local_ip(cf, ਅੰਕੜੇ);
    ctx->connected_at = *Curl_pgrs_now(ਅੰਕੜੇ);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (ਪੂਰਨ)ਨਤੀਜਾ, ctx->sock);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ do_connect(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      bool is_tcp_fastopen)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TCP_FASTOPEN_CONNECT
  ਪੂਰਨ optval = 1;
#ਜੇ_ਅੰਤ
  ਪੂਰਨ rc = -1;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(is_tcp_fastopen) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONNECT_ਅੰਕੜੇ_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_ਉਪਲਬਧ
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    ਜੇ(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = ਨਿਰਾ;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_ਅੰਕੜੇ_IDEMPOTENT,
                    ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  ਨਹੀਂਤਾਂ
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#ਜਾਂਜੇ defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    ਜੇ(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (ਖਾਲੀ *)&optval, ਆਕਾਰ(optval)) < 0)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#ਜਾਂਜੇ defined(MSG_FASTOPEN) /* old Linux */
    ਜੇ(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    ਨਹੀਂਤਾਂ
      rc = 0; /* Do nothing */
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  ਵਾਪਸ rc;
}

ਸਥਿਰ CURLcode cf_tcp_connect(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
  ਪੂਰਨ rc = 0;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  *ਪੂਰਾ = FALSE; /* a negative world view is best */
  ਜੇ(ctx->sock == CURL_SOCKET_BAD) {
    ਪੂਰਨ sockerr;

    ਨਤੀਜਾ = cf_socket_open(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    ਜੇ(cf->connected) {
      *ਪੂਰਾ = TRUE;
      ਵਾਪਸ CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, ਅੰਕੜੇ, (bool)cf->conn->ਬਿੱਟ.tcp_fastopen);
    sockerr = SOCKERRNO;
    ਸੈੱਟ_local_ip(cf, ਅੰਕੜੇ);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_ਪੋਰਟ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    ਜੇ(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#ਜੇ_ਅੰਤ
    ਜੇ(rc == -1) {
      ctx->sockerr = sockerr;
      ਨਤੀਜਾ = socket_connect_ਨਤੀਜਾ(ਅੰਕੜੇ, ctx->ip.remote_ip, sockerr);
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (ਖਾਲੀ)verifyconnect(ctx->sock, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  ਜੇ(rc == 0) { /* no connection yet */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    ਵਾਪਸ CURLE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(rc == CURL_CSELECT_ਬਾਹਰਲਾ ||
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#ਜੇ_ਅੰਤ
          cf->conn->ਬਿੱਟ.tcp_fastopen) {
    ਜੇ(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(ਅੰਕੜੇ);
      ਸੈੱਟ_local_ip(cf, ਅੰਕੜੇ);
      *ਪੂਰਾ = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      ਵਾਪਸ CURLE_OK;
    }
    ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
  }
  ਨਹੀਂਤਾਂ ਜੇ(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (ਖਾਲੀ)verifyconnect(ctx->sock, &ctx->sockerr);
    ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    VERBOSE(ਅੱਖਰ ਬਫਰ[STRERROR_LEN]);
    ਸੈੱਟ_local_ip(cf, ਅੰਕੜੇ);
    ਜੇ(ctx->sockerr) {
      ਅੰਕੜੇ->state.os_errno = ctx->sockerr;
      ਸੈੱਟ_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    }
    ਨਹੀਂਤਾਂ {
      VERBOSE(curlx_strcopy(ਬਫਰ, ਆਕਾਰ(ਬਫਰ), STRCONST("peer closed")));
    }
    ਜੇ(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(ਅੰਕੜੇ, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(ਅੰਕੜੇ, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_ਪੋਰਟ,
          ctx->ip.local_ip, ctx->ip.local_ਪੋਰਟ,
          curlx_strerror(ctx->sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    *ਪੂਰਾ = FALSE;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_socket_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    ਜੇ(ctx->listening) {
      ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ_ਵਿੱਚ_only(ਅੰਕੜੇ, ps, ctx->sock);
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ਨਹੀਂਤਾਂ ਜੇ(!cf->connected) {
      ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ_ਬਾਹਰਲਾ_only(ਅੰਕੜੇ, ps, ctx->sock);
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ਨਹੀਂਤਾਂ ਜੇ(!ctx->ਸਰਗਰਮ) {
      ਨਤੀਜਾ = Curl_pollset_ਜੋੜੋ_ਵਿੱਚ(ਅੰਕੜੇ, ps, ctx->sock);
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIO_IDEAL_SEND_BACKLOG_QUERY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ win_update_sndbuf_ਮਾਪ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  ਜੇ(curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ),
                        &ctx->ਆਖਰੀ_sndbuf_query_at) > 1000) {
    ਜੇ(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, ਆਕਾਰ(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_ਮਾਪ &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (ਅਚਲ ਅੱਖਰ *)&ideal, ਆਕਾਰ(ideal))) {
      ctx->sndbuf_ਮਾਪ = ideal;
    }
    ctx->ਆਖਰੀ_sndbuf_query_at = *Curl_pgrs_now(ਅੰਕੜੇ);
  }
}

#ਜੇ_ਅੰਤ /* USE_WINSOCK */

ਸਥਿਰ CURLcode cf_socket_send(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅਚਲ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (ਖਾਲੀ)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  /* simulate network blocking/partial writes */
  ਜੇ(ctx->wblock_percent > 0) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c = 0;
    Curl_rand_ਬਾਈਟਾਂ(ਅੰਕੜੇ, FALSE, &c, 1);
    ਜੇ(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      ਵਾਪਸ CURLE_AGAIN;
    }
  }
  ਜੇ(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    ਜੇ(!len)
      len = 1;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  ਜੇ(cf->conn->ਬਿੱਟ.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->ਬਿੱਟ.tcp_fastopen = FALSE;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    rv = swrite(ctx->sock, buf, len);

  ਜੇ(!curlx_sztouz(rv, pnwritten)) {
    ਪੂਰਨ sockerr = SOCKERRNO;
    ਜੇ(SOCK_EAGAIN(sockerr)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#ਜੇ_ਅੰਤ
      ) {
      ਨਤੀਜਾ = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ਨਹੀਂਤਾਂ {
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      failf(ਅੰਕੜੇ, "Send failure: %s",
            curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਅੰਕੜੇ->state.os_errno = sockerr;
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  ਜੇ(!ਨਤੀਜਾ)
    win_update_sndbuf_ਮਾਪ(ਅੰਕੜੇ, ctx);
#ਜੇ_ਅੰਤ

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_socket_recv(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅੱਖਰ *buf, size_t len, size_t *pnread)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  /* simulate network blocking/partial reads */
  ਜੇ(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c = 0;
    Curl_rand(ਅੰਕੜੇ, &c, 1);
    ਜੇ(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      ਵਾਪਸ CURLE_AGAIN;
    }
  }
  ਜੇ(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#ਜੇ_ਅੰਤ

  rv = sread(ctx->sock, buf, len);

  ਜੇ(!curlx_sztouz(rv, pnread)) {
    ਪੂਰਨ sockerr = SOCKERRNO;
    ਜੇ(SOCK_EAGAIN(sockerr)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
       || (sockerr == SOCKEINTR)
#ਜੇ_ਅੰਤ
      ) {
      ਨਤੀਜਾ = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ਨਹੀਂਤਾਂ {
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      failf(ਅੰਕੜੇ, "Recv failure: %s",
            curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਅੰਕੜੇ->state.os_errno = sockerr;
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "recv(len=%zu) -> %d, %zu", len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread);
  ਜੇ(!ਨਤੀਜਾ && !ctx->got_ਪਹਿਲਾ_byte) {
    ctx->ਪਹਿਲਾ_byte_at = *Curl_pgrs_now(ਅੰਕੜੇ);
    ctx->got_ਪਹਿਲਾ_byte = TRUE;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_socket_update_ਅੰਕੜੇ(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* Update the IP info held in the transfer, if we have that. */
  ਜੇ(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
    ਅੰਕੜੇ->info.primary = ctx->ip;
  }
}

ਸਥਿਰ ਖਾਲੀ cf_socket_ਸਰਗਰਮ(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  ਸੈੱਟ_local_ip(cf, ਅੰਕੜੇ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਜੇ(cf->sockindex == FIRSTSOCKET)
    cf->conn->ਬਿੱਟ.ipv6 = (ctx->addr.family == AF_INET6);
#ਜੇ_ਅੰਤ
  ctx->ਸਰਗਰਮ = TRUE;
}

ਸਥਿਰ CURLcode cf_socket_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;

  (ਖਾਲੀ)arg1;
  (ਖਾਲੀ)arg2;
  ਚੋਣ(event) {
  ਹਾਲਤ CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_ਸਰਗਰਮ(cf, ਅੰਕੜੇ);
    cf_socket_update_ਅੰਕੜੇ(cf, ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_ਤਿਆਰੀ:
    cf_socket_update_ਅੰਕੜੇ(cf, ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_REPORT_STATS:
    ਜੇ(cf->connected && !ctx->stats_reported) {
      ਬਣਤਰ curltime *ts = ਨਿਰਾ;
      ਚੋਣ(ctx->transport) {
      ਹਾਲਤ TRNSPRT_UDP:
      ਹਾਲਤ TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        ਜੇ(ctx->got_ਪਹਿਲਾ_byte)
          ts = &ctx->ਪਹਿਲਾ_byte_at;
        ਰੋਕੋ;
      ਮੂਲ:
        ts = &ctx->connected_at;
        ਰੋਕੋ;
      }
      ਜੇ(ts) {
        Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool cf_socket_conn_is_alive(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    bool *ਨਿਵੇਸ਼_pending)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  ਬਣਤਰ pollfd pfd[1];
  ਪੂਰਨ r;

  *ਨਿਵੇਸ਼_pending = FALSE;

  ਜੇ(!ctx || ctx->sock == CURL_SOCKET_BAD)
    ਵਾਪਸ FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  ਜੇ(r < 0) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "is_alive: poll error, assume dead");
    ਵਾਪਸ FALSE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(r == 0) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "is_alive: poll timeout, assume alive");
    ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "is_alive: err/hup/etc events, assume dead");
    ਵਾਪਸ FALSE;
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "is_alive: valid events, looks alive");
  *ਨਿਵੇਸ਼_pending = TRUE;
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ CURLcode cf_socket_query(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;

  ਚੋਣ(query) {
  ਹਾਲਤ CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : ਨਿਰਾ;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_CONNECT_ਜਵਾਬ_MS:
    ਜੇ(ctx->got_ਪਹਿਲਾ_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->ਪਹਿਲਾ_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? (ਪੂਰਨ)ms : ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
    }
    ਨਹੀਂਤਾਂ
      *pres1 = -1;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_IP_INFO:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#ਨਹੀਂਤਾਂ
    *pres1 = FALSE;
#ਜੇ_ਅੰਤ
    *(ਬਣਤਰ ip_quadruple *)pres2 = ctx->ip;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_REALLY_CONNECTED:
    ਜੇ(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    ਨਹੀਂਤਾਂ
      *pres1 = ctx->got_ਪਹਿਲਾ_byte;
    ਵਾਪਸ CURLE_OK;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ਬਣਤਰ Curl_cftype Curl_cft_tcp = {
  "TCP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ਅੰਕੜੇ_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_tcp_create(ਬਣਤਰ Curl_cfilter **pcf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_peer *origin,
                            ਬਣਤਰ Curl_peer *peer,
                            uint8_t transport_peer,
                            ਬਣਤਰ connectdata *conn,
                            ਬਣਤਰ Curl_sockaddr_ex *addr,
                            ਬਣਤਰ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ਬਣਤਰ cf_socket_ctx *ctx = ਨਿਰਾ;
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)origin;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)tunnel_peer;
  (ਖਾਲੀ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  ਜੇ(!addr) {
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

ਬਾਹਰਲਾ:
  *pcf = (!ਨਤੀਜਾ) ? cf : ਨਿਰਾ;
  ਜੇ(ਨਤੀਜਾ) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __linux__
ਸਥਿਰ ਖਾਲੀ linux_quic_mtu(ਬਣਤਰ cf_socket_ctx *ctx)
{
  ਪੂਰਨ val;
  ਚੋਣ(ctx->addr.family) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ IP_MTU_DISCOVER
  ਹਾਲਤ AF_INET:
    val = IP_PMTUDISC_DO;
    (ਖਾਲੀ)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     ਆਕਾਰ(val));
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ IPV6_MTU_DISCOVER
  ਹਾਲਤ AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (ਖਾਲੀ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     ਆਕਾਰ(val));
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  }
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ linux_quic_mtu(x)
#ਜੇ_ਅੰਤ

#ਜੇ defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
ਸਥਿਰ ਖਾਲੀ linux_quic_gro(ਬਣਤਰ cf_socket_ctx *ctx)
{
  ਪੂਰਨ one = 1;
  (ਖਾਲੀ)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)ਆਕਾਰ(one));
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ linux_quic_gro(x)
#ਜੇ_ਅੰਤ

#ਜੇ (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
ਸਥਿਰ ਖਾਲੀ linux_quic_ecn(ਬਣਤਰ cf_socket_ctx *ctx)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ tos = 1;
  ਚੋਣ(ctx->addr.family) {
  ਹਾਲਤ AF_INET:
    (ਖਾਲੀ)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, ਆਕਾਰ(tos));
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ IPV6_RECVTCLASS
  ਹਾਲਤ AF_INET6:
    (ਖਾਲੀ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, ਆਕਾਰ(tos));
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  }
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ linux_quic_ecn(x)
#ਜੇ_ਅੰਤ

#ਜੇ (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
ਸਥਿਰ ਖਾਲੀ linux_ip_dontfrag(ਬਣਤਰ cf_socket_ctx *ctx)
{
  ਪੂਰਨ val = 1;

  ਚੋਣ(ctx->addr.family) {
  ਹਾਲਤ AF_INET:
    (ਖਾਲੀ)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, ਆਕਾਰ(val));
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ IPV6_DONTFRAG
  ਹਾਲਤ AF_INET6:
    (ਖਾਲੀ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, ਆਕਾਰ(val));
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  }
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ linux_ip_dontfrag(x)
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode cf_udp_ਤਿਆਰੀ_quic(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  ਪੂਰਨ rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  ਜੇ(rc == -1) {
    ਵਾਪਸ socket_connect_ਨਤੀਜਾ(ਅੰਕੜੇ, ctx->ip.remote_ip, SOCKERRNO);
  }
  ctx->sock_connected = TRUE;
  ਸੈੱਟ_local_ip(cf, ਅੰਕੜੇ);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "%s socket %" FMT_SOCKET_T
              " connected: [%s:%d] -> [%s:%d]",
              (ctx->transport == TRNSPRT_QUIC) ? "QUIC" : "UDP",
              ctx->sock, ctx->ip.local_ip, ctx->ip.local_ਪੋਰਟ,
              ctx->ip.remote_ip, ctx->ip.remote_ਪੋਰਟ);

  /* Currently, cf->ctx->sock is always non-blocking because the only
   * caller to cf_udp_setup_quic() is cf_udp_connect() that passes the
   * non-blocking socket created by cf_socket_open() to it. Thus, we
   * do not need to call curlx_nonblock() in cf_udp_setup_quic() anymore.
   */
  linux_quic_ecn(ctx);
  linux_quic_mtu(ctx);
  linux_ip_dontfrag(ctx);
  linux_quic_gro(ctx);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_udp_connect(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  *ਪੂਰਾ = FALSE;
  ਜੇ(ctx->sock == CURL_SOCKET_BAD) {
    ਨਤੀਜਾ = cf_socket_open(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_udp_connect(), open failed -> %d",
                  (ਪੂਰਨ)ਨਤੀਜਾ);
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(ctx->transport == TRNSPRT_QUIC) {
      ਨਤੀਜਾ = cf_udp_ਤਿਆਰੀ_quic(cf, ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_ਪੋਰਟ);
    }
    *ਪੂਰਾ = TRUE;
    cf->connected = TRUE;
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਬਣਤਰ Curl_cftype Curl_cft_udp = {
  "UDP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_udp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ਅੰਕੜੇ_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_udp_create(ਬਣਤਰ Curl_cfilter **pcf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_peer *origin,
                            ਬਣਤਰ Curl_peer *peer,
                            uint8_t transport_peer,
                            ਬਣਤਰ connectdata *conn,
                            ਬਣਤਰ Curl_sockaddr_ex *addr,
                            ਬਣਤਰ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ਬਣਤਰ cf_socket_ctx *ctx = ਨਿਰਾ;
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)origin;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)tunnel_peer;
  (ਖਾਲੀ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

ਬਾਹਰਲਾ:
  *pcf = (!ਨਤੀਜਾ) ? cf : ਨਿਰਾ;
  ਜੇ(ਨਤੀਜਾ) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* this is the TCP filter which can also handle this case */
ਬਣਤਰ Curl_cftype Curl_cft_unix = {
  "UNIX",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ਅੰਕੜੇ_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_unix_create(ਬਣਤਰ Curl_cfilter **pcf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_peer *origin,
                            ਬਣਤਰ Curl_peer *peer,
                            uint8_t transport_peer,
                            ਬਣਤਰ connectdata *conn,
                            ਬਣਤਰ Curl_sockaddr_ex *addr,
                            ਬਣਤਰ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ਬਣਤਰ cf_socket_ctx *ctx = ਨਿਰਾ;
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)origin;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)tunnel_peer;
  (ਖਾਲੀ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

ਬਾਹਰਲਾ:
  *pcf = (!ਨਤੀਜਾ) ? cf : ਨਿਰਾ;
  ਜੇ(ਨਤੀਜਾ) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ timediff_t cf_tcp_accept_timeleft(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.accepttimeout > 0)
    timeout_ms = ਅੰਕੜੇ->ਸੈੱਟ.accepttimeout;
#ਜੇ_ਅੰਤ

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
  ਜੇ(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  ਨਹੀਂਤਾਂ {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &ctx->started_at);
    ਜੇ(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  ਵਾਪਸ timeout_ms;
}

ਸਥਿਰ ਖਾਲੀ cf_tcp_ਸੈੱਟ_accepted_remote_ip(ਬਣਤਰ Curl_cfilter *cf,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GETPEERNAME
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_ਪੋਰਟ = 0;
  plen = ਆਕਾਰ(ssrem);
  memset(&ssrem, 0, plen);
  ਜੇ(getpeername(ctx->sock, (ਬਣਤਰ sockaddr *)&ssrem, &plen)) {
    ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
    ਪੂਰਨ sockerr = SOCKERRNO;
    failf(ਅੰਕੜੇ, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਵਾਪਸ;
  }
  ਨਤੀਜਾ = sockaddr2string((ਬਣਤਰ sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_ਪੋਰਟ);
  ਜੇ(ਨਤੀਜਾ) {
    failf(ਅੰਕੜੇ, "ssrem inet_ntop() failed with %d", (ਪੂਰਨ)ਨਤੀਜਾ);
    ਵਾਪਸ;
  }
#ਨਹੀਂਤਾਂ
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_ਪੋਰਟ = 0;
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ CURLcode cf_tcp_accept_connect(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;
  ਅੱਖਰ errbuf[STRERROR_LEN];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ Curl_sockaddr_storage ਜੋੜੋ;
#ਨਹੀਂਤਾਂ
  ਬਣਤਰ sockaddr_in ਜੋੜੋ;
#ਜੇ_ਅੰਤ
  curl_socklen_t ਮਾਪ = (curl_socklen_t)ਆਕਾਰ(ਜੋੜੋ);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  ਪੂਰਨ socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  *ਪੂਰਾ = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, ਅੰਕੜੇ);
  ਜੇ(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(ਅੰਕੜੇ, "Accept timeout occurred while waiting server connect");
    ਵਾਪਸ CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_ਪੋਰਟ);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "socket_check -> %x", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)socketstate);
  ਚੋਣ(socketstate) {
  ਹਾਲਤ -1: /* error */
    /* let's die here */
    failf(ਅੰਕੜੇ, "Error while waiting for server connect");
    ਵਾਪਸ CURLE_FTP_ACCEPT_FAILED;
  ਮੂਲ:
    ਜੇ(socketstate & CURL_CSELECT_ਵਿੱਚ) {
      infof(ਅੰਕੜੇ, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    ਰੋਕੋ;
  }

  ਜੇ(!incoming) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "nothing heard from the server yet");
    ਵਾਪਸ CURLE_OK;
  }

  ਮਾਪ = ਆਕਾਰ(ਜੋੜੋ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (ਬਣਤਰ sockaddr *)&ਜੋੜੋ, &ਮਾਪ,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#ਨਹੀਂਤਾਂ
  s_accepted = CURL_ACCEPT(ctx->sock, (ਬਣਤਰ sockaddr *)&ਜੋੜੋ, &ਮਾਪ);
#ਜੇ_ਅੰਤ

  ਜੇ(s_accepted == CURL_SOCKET_BAD) {
    failf(ਅੰਕੜੇ, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, ਆਕਾਰ(errbuf)));
    ਵਾਪਸ CURLE_FTP_ACCEPT_FAILED;
  }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_ACCEPT4
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCNTL
  ਜੇ(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(ਅੰਕੜੇ, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, ਆਕਾਰ(errbuf)));
    Curl_socket_close(ਅੰਕੜੇ, cf->conn, s_accepted);
    ਵਾਪਸ CURLE_FTP_ACCEPT_FAILED;
  }
#ਜੇ_ਅੰਤ /* HAVE_FCNTL */
  ਜੇ(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(ਅੰਕੜੇ, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, ਆਕਾਰ(errbuf)));
    Curl_socket_close(ਅੰਕੜੇ, cf->conn, s_accepted);
    ਵਾਪਸ CURLE_FTP_ACCEPT_FAILED;
  }
#ਜੇ_ਅੰਤ /* !HAVE_ACCEPT4 */
  infof(ਅੰਕੜੇ, "Connection accepted from server");

  /* Replace any filter on SECONDARY with one listening on this socket */
  ctx->listening = FALSE;
  ctx->accepted = TRUE;
  socket_close(ਅੰਕੜੇ, cf->conn, TRUE, ctx->sock);
  ctx->sock = s_accepted;

  cf->conn->sock[cf->sockindex] = ctx->sock;
  cf_tcp_ਸੈੱਟ_accepted_remote_ip(cf, ਅੰਕੜੇ);
  ਸੈੱਟ_local_ip(cf, ਅੰਕੜੇ);
  ctx->ਸਰਗਰਮ = TRUE;
  ctx->connected_at = *Curl_pgrs_now(ਅੰਕੜੇ);
  cf->connected = TRUE;
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "accepted_set(sock=%" FMT_SOCKET_T
              ", remote=%s port=%d)",
              ctx->sock, ctx->ip.remote_ip, ctx->ip.remote_ਪੋਰਟ);

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fsockopt) {
    ਬਣਤਰ Curl_mapi_guard guard;
    ਪੂਰਨ error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fsockopt);
    error = ਅੰਕੜੇ->ਸੈੱਟ.fsockopt(ਅੰਕੜੇ->ਸੈੱਟ.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_ਸਿਰਾ(&guard);

    ਜੇ(error)
      ਵਾਪਸ CURLE_ABORTED_BY_CALLBACK;
  }
  *ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLE_OK;
}

ਬਣਤਰ Curl_cftype Curl_cft_tcp_accept = {
  "TCP-ACCEPT",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_accept_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ਅੰਕੜੇ_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_conn_tcp_listen_ਸੈੱਟ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ cf_socket_ctx *ctx = ਨਿਰਾ;

  /* replace any existing */
  Curl_conn_cf_discard_all(ਅੰਕੜੇ, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  Curl_conn_cf_ਜੋੜੋ(ਅੰਕੜੇ, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(ਅੰਕੜੇ);
  conn->sock[sockindex] = ctx->sock;
  ਸੈੱਟ_local_ip(cf, ਅੰਕੜੇ);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_ਪੋਰਟ);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_conn_is_tcp_listen(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             int8_t sockindex)
{
  ਬਣਤਰ Curl_cfilter *cf = ਅੰਕੜੇ->conn->cfilter[sockindex];
  ਜਦੋਂ(cf) {
    ਜੇ(cf->cft == &Curl_cft_tcp_accept)
      ਵਾਪਸ TRUE;
    cf = cf->ਅਗਲਾ;
  }
  ਵਾਪਸ FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
ਸਥਿਰ bool cf_is_socket(ਬਣਤਰ Curl_cfilter *cf)
{
  ਵਾਪਸ cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             curl_socket_t *psock,
                             ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex **paddr,
                             ਬਣਤਰ ip_quadruple *pip)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(cf_is_socket(cf) && cf->ctx) {
    ਬਣਤਰ cf_socket_ctx *ctx = cf->ctx;

    ਜੇ(psock)
      *psock = ctx->sock;
    ਜੇ(paddr)
      *paddr = &ctx->addr;
    ਜੇ(pip)
      *pip = ctx->ip;
    ਵਾਪਸ CURLE_OK;
  }
  ਵਾਪਸ CURLE_FAILED_INIT;
}
