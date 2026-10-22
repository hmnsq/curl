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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h> /* <netinet/tcp.h> may need it */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_UN_H
#ಅನ್ತರ್ಭಾವ <sys/un.h> /* for sockaddr_un */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LINUX_TCP_H
#ಅನ್ತರ್ಭಾವ <linux/tcp.h>
#ಅಥವಾಆದರೆ defined(HAVE_NETINET_TCP_H)
#ಅನ್ತರ್ಭಾವ <netinet/tcp.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_UDP_H
#ಅನ್ತರ್ಭಾವ <netinet/udp.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_IP_H
#ಅನ್ತರ್ಭಾವ <netinet/ip.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_IFADDRS_H
#ಅನ್ತರ್ಭಾವ <ifaddrs.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#ಅನ್ತರ್ಭಾವ <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#ಆದರೆ_ಅಂತ್ಯ

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(ರಚನೆ sockaddr *sa, curl_socklen_t salen,
                                  ಅಕ್ಷರ *addr, uint16_t *ಪೋರ್ಟ್);
UNITTEST CURLcode sockaddr2string(ರಚನೆ sockaddr *sa, curl_socklen_t salen,
                                  ಅಕ್ಷರ *addr, uint16_t *ಪೋರ್ಟ್)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ sockaddr_in *si = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ sockaddr_in6 *si6 = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  ರಚನೆ sockaddr_un *su = ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)salen;
#ಆದರೆ_ಅಂತ್ಯ

  ಆಯ್ಕೆ(sa->sa_family) {
  ಸ್ಥಿತಿ AF_INET:
    si = (ರಚನೆ sockaddr_in *)(ಶೂನ್ಯ *)sa;
    ಫಲಿತಾಂಶ = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      *ಪೋರ್ಟ್ = ntohs(si->sin_port);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಸ್ಥಿತಿ AF_INET6:
    si6 = (ರಚನೆ sockaddr_in6 *)(ಶೂನ್ಯ *)sa;
    ಫಲಿತಾಂಶ = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      *ಪೋರ್ಟ್ = ntohs(si6->sin6_port);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  ಸ್ಥಿತಿ AF_UNIX:
    ಆದರೆ(salen > (curl_socklen_t)ಗಾತ್ರ(CURL_SA_FAMILY_T)) {
      su = (ರಚನೆ sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_ಮಾರ್ಗ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      addr[0] = 0; /* socket with no name */
    *ಪೋರ್ಟ್ = 0;
    ಹಿಂತಿರುಗಿ CURLE_OK;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಫಲಿತಾಂಶ = CURLE_UNSUPPORTED_PROTOCOL;
    ನಿಲ್ಲಿಸು;
  }

  addr[0] = '\0';
  *ಪೋರ್ಟ್ = 0;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ tcpnodelay(ರಚನೆ Curl_cfilter *cf,
                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       curl_socket_t sockfd)
{
#ಆದರೆ defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  ಪೂರ್ಣಾಂಕ level = IPPROTO_TCP;
  VERBOSE(ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN]);

  ಆದರೆ(setsockopt(sockfd, level, TCP_NODELAY, (ಶೂನ್ಯ *)&onoff, ಗಾತ್ರ(onoff)) < 0)
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)sockfd;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_ಮಿತಿ) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_ಮಿತಿ)
#ಆದರೆ defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
ಸ್ಥಿರ ಶೂನ್ಯ alive_unit(ಪೂರ್ಣಾಂಕ *x)
{
  /* make sure this doesn't wrap */
  ಆದರೆ(*x < (ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ / 1000))
    *x *= 1000;
  ಇಲ್ಲದಿದ್ದರೆ
    /* this is still almost 25 days */
    *x = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು alive_unit(x)
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ tcpkeepalive(ರಚನೆ Curl_cfilter *cf,
                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         curl_socket_t sockfd)
{
  ಪೂರ್ಣಾಂಕ optval = ದತ್ತಾಂಶ->ಗಣ.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  ಆದರೆ(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (ಶೂನ್ಯ *)&optval, ಗಾತ್ರ(optval)) < 0) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    ಆದರೆ(curlx_verify_windows_ಆವೃತ್ತಿ(10, 0, 16299, PLATFORM_WINNT,
                                    ಆವೃತ್ತಿ_GREATER_THAN_EQUAL)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Set TCP_KEEP* on fd=%" FMT_SOCKET_T, sockfd);
      optval = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepidle);
/* Offered by mingw-w64 v12+, MS SDK 6.0A/VS2008+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TCP_KEEPALIVE
#ವ್ಯಾಖ್ಯಾನಿಸು TCP_KEEPALIVE 3
#ಆದರೆ_ಅಂತ್ಯ
/* Offered by mingw-w64 v12+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TCP_KEEPCNT
#ವ್ಯಾಖ್ಯಾನಿಸು TCP_KEEPCNT 16
#ಆದರೆ_ಅಂತ್ಯ
/* Offered by mingw-w64 v12+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TCP_KEEPIDLE
#ವ್ಯಾಖ್ಯಾನಿಸು TCP_KEEPIDLE TCP_KEEPALIVE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TCP_KEEPINTVL
#ವ್ಯಾಖ್ಯಾನಿಸು TCP_KEEPINTVL 17
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&optval, ಗಾತ್ರ(optval)) < 0) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepintvl);
      ಆದರೆ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&optval, ಗಾತ್ರ(optval)) < 0) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepcnt);
      ಆದರೆ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&optval, ಗಾತ್ರ(optval)) < 0) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIO_KEEPALIVE_VALS
#ವ್ಯಾಖ್ಯಾನಿಸು SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      ರಚನೆ tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#ಆದರೆ_ಅಂತ್ಯ
      ರಚನೆ tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      ಆದರೆ(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, ಗಾತ್ರ(vals),
                  ಖಾಲಿ, 0, &dummy, ಖಾಲಿ, ಖಾಲಿ) != 0) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#ಇಲ್ಲದಿದ್ದರೆ /* !USE_WINSOCK */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TCP_KEEPIDLE
    optval = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepidle);
    alive_unit(&optval);
    ಆದರೆ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (ಶೂನ್ಯ *)&optval, ಗಾತ್ರ(optval)) < 0) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ಅಥವಾಆದರೆ defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepidle);
    alive_unit(&optval);
    ಆದರೆ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (ಶೂನ್ಯ *)&optval, ಗಾತ್ರ(optval)) < 0) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ಅಥವಾಆದರೆ defined(TCP_KEEPALIVE_ಮಿತಿ)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepidle);
    alive_unit(&optval);
    ಆದರೆ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ಮಿತಿ,
                  (ಶೂನ್ಯ *)&optval, ಗಾತ್ರ(optval)) < 0) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TCP_KEEPINTVL
    optval = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepintvl);
    alive_unit(&optval);
    ಆದರೆ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (ಶೂನ್ಯ *)&optval, ಗಾತ್ರ(optval)) < 0) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ಅಥವಾಆದರೆ defined(TCP_KEEPALIVE_ABORT_ಮಿತಿ)
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
      ಪೂರ್ಣಾಂಕ keepcnt = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepcnt);
      ಪೂರ್ಣಾಂಕ keepintvl = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepintvl);

      ಆದರೆ(keepcnt > 0 && keepintvl > (ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ / keepcnt))
        optval = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
      ಇಲ್ಲದಿದ್ದರೆ
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    ಆದರೆ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_ಮಿತಿ,
                  (ಶೂನ್ಯ *)&optval, ಗಾತ್ರ(optval)) < 0) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TCP_KEEPCNT
    optval = curlx_sltosi(ದತ್ತಾಂಶ->ಗಣ.tcp_keepcnt);
    ಆದರೆ(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (ಶೂನ್ಯ *)&optval, ಗಾತ್ರ(optval)) < 0) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to set TCP_KEEPCNT on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* USE_WINSOCK */
  }
}

/**
 * Assign the addrinfo `ai` to the Curl_sockaddr_ex `addr` with
 * transport determining socktype and protocol.
 */
CURLcode Curl_socket_addr_from_ai(ರಚನೆ Curl_sockaddr_ex *addr,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai,
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
  addr->addrlen = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= ಗಾತ್ರ(addr->curl_sa_addrbuf));
  ಆದರೆ(addr->addrlen > ಗಾತ್ರ(addr->curl_sa_addrbuf))
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;

  ಸ್ಮರಣೆ_ನಕಲು(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SO_NOSIGPIPE
ಪೂರ್ಣಾಂಕ Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  ಪೂರ್ಣಾಂಕ onoff = 1;
  ಹಿಂತಿರುಗಿ setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (ಶೂನ್ಯ *)&onoff, ಗಾತ್ರ(onoff));
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_SO_NOSIGPIPE */

#ಆದರೆ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
ಸ್ಥಿರ uint32_t get_scope_id(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  ಆದರೆ(ದತ್ತಾಂಶ->conn->scope_id)
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  ಆದರೆ(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#ಆದರೆ defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    ರಚನೆ ifaddrs *ifa, *ifa_list;
    ಆದರೆ(getifaddrs(&ifa_list) == 0) {
      ಗಾಗಿ(ifa = ifa_list; ifa; ifa = ifa->ifa_ಮುಂದಿನ) {
        ಆದರೆ(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          ರಚನೆ sockaddr_in6 *s6 = (ಶೂನ್ಯ *)ifa->ifa_addr;
          ಆದರೆ(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(ದತ್ತಾಂಶ,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)scope_id);
            ನಿಲ್ಲಿಸು;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  ಹಿಂತಿರುಗಿ scope_id;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode socket_open(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  ಅಕ್ಷರ errbuf[STRERROR_LEN];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#ಆದರೆ_ಅಂತ್ಯ

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    ರಚನೆ Curl_mapi_guard guard;
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fopensocket);
    *sockfd = ದತ್ತಾಂಶ->ಗಣ.fopensocket(ದತ್ತಾಂಶ->ಗಣ.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (ರಚನೆ curl_sockaddr *)addr);
    CURL_CBAPI_ಕೊನೆ(&guard);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* opensocket callback not set, so create the socket now */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    ಆದರೆ((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(ದತ್ತಾಂಶ, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }
#ಆದರೆ_ಅಂತ್ಯ
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    ಆದರೆ((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(ದತ್ತಾಂಶ, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಗಾತ್ರ(errbuf)));
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SO_NOSIGPIPE
  ಆದರೆ(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(ದತ್ತಾಂಶ, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಗಾತ್ರ(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }
#ಆದರೆ_ಅಂತ್ಯ /* USE_SO_NOSIGPIPE */

#ಆದರೆ defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  ಆದರೆ(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(ದತ್ತಾಂಶ, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಗಾತ್ರ(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  ಆದರೆ(addr->family == AF_INET6) {
    ರಚನೆ sockaddr_in6 * ಸ್ಥಿರಾಂಕ sa6 = (ಶೂನ್ಯ *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(ದತ್ತಾಂಶ, sa6);
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
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
CURLcode Curl_socket_open(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai,
                          ರಚನೆ Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  ರಚನೆ Curl_sockaddr_ex dummy;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  ಫಲಿತಾಂಶ = Curl_socket_addr_from_ai(addr, ai, transport);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಹಿಂತಿರುಗಿ socket_open(ದತ್ತಾಂಶ, addr, sockfd);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ socket_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn,
                        ಪೂರ್ಣಾಂಕ use_callback, curl_socket_t sock)
{
  ಆದರೆ(sock == CURL_SOCKET_BAD)
    ಹಿಂತಿರುಗಿ 0;

  ಆದರೆ(use_callback && conn && conn->fclosesocket) {
    ರಚನೆ Curl_mapi_guard guard;
    ಪೂರ್ಣಾಂಕ rc;
    Curl_multi_will_close(ದತ್ತಾಂಶ, sock);
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_ಕೊನೆ(&guard);
    ಹಿಂತಿರುಗಿ rc;
  }

  ಆದರೆ(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(ದತ್ತಾಂಶ, sock);

  sclose(sock);

  ಹಿಂತಿರುಗಿ 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
ಪೂರ್ಣಾಂಕ Curl_socket_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn,
                      curl_socket_t sock)
{
  ಹಿಂತಿರುಗಿ socket_close(ದತ್ತಾಂಶ, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು,
                              ಅಕ್ಷರ **dev, ಅಕ್ಷರ **iface, ಅಕ್ಷರ **host)
{
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ if_ಪೂರ್ವಪ್ರತ್ಯಯ[] = "if!";
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ host_ಪೂರ್ವಪ್ರತ್ಯಯ[] = "host!";
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ if_host_ಪೂರ್ವಪ್ರತ್ಯಯ[] = "ifhost!";
  size_t len;

  len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಒಳಹರಿವು);
  ಆದರೆ(len > 512)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(dev)
    *dev = ಖಾಲಿ;
  ಆದರೆ(iface)
    *iface = ಖಾಲಿ;
  ಆದರೆ(host)
    *host = ಖಾಲಿ;

  ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(if_ಪೂರ್ವಪ್ರತ್ಯಯ, ಒಳಹರಿವು, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(if_ಪೂರ್ವಪ್ರತ್ಯಯ))) {
    ಒಳಹರಿವು += ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(if_ಪೂರ್ವಪ್ರತ್ಯಯ);
    ಆದರೆ(!*ಒಳಹರಿವು)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಆದರೆ(iface) {
      *iface = curlx_memdup0(ಒಳಹರಿವು, len - ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(if_ಪೂರ್ವಪ್ರತ್ಯಯ));
      ಹಿಂತಿರುಗಿ *iface ? CURLE_OK : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(host_ಪೂರ್ವಪ್ರತ್ಯಯ, ಒಳಹರಿವು, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(host_ಪೂರ್ವಪ್ರತ್ಯಯ))) {
    ಒಳಹರಿವು += ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(host_ಪೂರ್ವಪ್ರತ್ಯಯ);
    ಆದರೆ(!*ಒಳಹರಿವು)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಆದರೆ(host) {
      *host = curlx_memdup0(ಒಳಹರಿವು, len - ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(host_ಪೂರ್ವಪ್ರತ್ಯಯ));
      ಹಿಂತಿರುಗಿ *host ? CURLE_OK : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(if_host_ಪೂರ್ವಪ್ರತ್ಯಯ, ಒಳಹರಿವು, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(if_host_ಪೂರ್ವಪ್ರತ್ಯಯ))) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host_part;
    ಒಳಹರಿವು += ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(if_host_ಪೂರ್ವಪ್ರತ್ಯಯ);
    len -= ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(if_host_ಪೂರ್ವಪ್ರತ್ಯಯ);
    host_part = memchr(ಒಳಹರಿವು, '!', len);
    ಆದರೆ(!host_part || !*(host_part + 1))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಆದರೆ(iface) {
      *iface = curlx_memdup0(ಒಳಹರಿವು, host_part - ಒಳಹರಿವು);
      ಆದರೆ(!*iface)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ++host_part;
    ಆದರೆ(host) {
      *host = curlx_memdup0(host_part, len - (host_part - ಒಳಹರಿವು));
      ಆದರೆ(!*host) {
        ಆದರೆ(iface)
          curlx_safefree(*iface);
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
    }
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(!*ಒಳಹರಿವು)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಆದರೆ(dev) {
    *dev = curlx_memdup0(ಒಳಹರಿವು, len);
    ಹಿಂತಿರುಗಿ *dev ? CURLE_OK : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BINDLOCAL
ಸ್ಥಿರ CURLcode bindlocal(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn,
                          curl_socket_t sockfd, ಪೂರ್ಣಾಂಕ af, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ scope,
                          uint8_t transport)
{
  ರಚನೆ Curl_sockaddr_storage sa;
  ರಚನೆ sockaddr *sock = (ರಚನೆ sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  ರಚನೆ sockaddr_in *si4 = (ರಚನೆ sockaddr_in *)&sa;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ sockaddr_in6 *si6 = (ರಚನೆ sockaddr_in6 *)&sa;
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ Curl_dns_ನಮೂದು *h = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಪೋರ್ಟ್ = ದತ್ತಾಂಶ->ಗಣ.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  ಪೂರ್ಣಾಂಕ portnum = ದತ್ತಾಂಶ->ಗಣ.localportrange;
  ಅಕ್ಷರ *dev_ಒಳ = ಖಾಲಿ, *iface_ಒಳ = ಖಾಲಿ, *host_ಒಳ = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *iface = ಖಾಲಿ, *host = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ sockerr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ IP_BIND_ADDRESS_NO_PORT
  ಪೂರ್ಣಾಂಕ on = 1;
#ಆದರೆ_ಅಂತ್ಯ
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_IPV6
  (ಶೂನ್ಯ)scope;
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    ಫಲಿತಾಂಶ = Curl_parse_interface(conn->interface_opts,
                                  &dev_ಒಳ, &iface_ಒಳ, &host_ಒಳ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    /* This is how we use the values */
    iface = iface_ಒಳ ? iface_ಒಳ : dev_ಒಳ;
    host = host_ಒಳ ? host_ಒಳ : dev_ಒಳ;
  }

  ಆದರೆ(!iface && !host && !ಪೋರ್ಟ್)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(iface && (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(iface) >= 255)) {
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  memset(&sa, 0, ಗಾತ್ರ(ರಚನೆ Curl_sockaddr_storage));

  ಆದರೆ(iface || host) {
    ಅಕ್ಷರ myhost[256] = "";
    ಪೂರ್ಣಾಂಕ ಮುಗಿದ = 0; /* -1 for error, 1 for address found */
    if2ip_ಫಲಿತಾಂಶ_t if2ip_ಫಲಿತಾಂಶ = IF2_IP_NOT_ಸಿಕ್ಕಿತು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SO_BINDTODEVICE
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
    ಆದರೆ(iface &&
       setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                  iface, (curl_socklen_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(iface) + 1) == 0 &&
       /* This is often "errno 1, error: Operation not permitted" if you are
        * not running as root or another suitable privileged user. If it
        * succeeds it means the parameter was a valid interface and not an IP
        * address. Return immediately.
        */
       !host_ಒಳ) {
      infof(ದತ್ತಾಂಶ, "socket successfully bound to interface '%s'", iface);
      ಫಲಿತಾಂಶ = CURLE_OK;
      ಹೋಗು ಹೊರಹೋಗು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(!host_ಒಳ) {
      /* Discover IP from input device, then bind to it */
      if2ip_ಫಲಿತಾಂಶ = Curl_if2ip(af,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
                                scope, conn->scope_id,
#ಆದರೆ_ಅಂತ್ಯ
                                iface, myhost, ಗಾತ್ರ(myhost));
    }
    ಆಯ್ಕೆ(if2ip_ಫಲಿತಾಂಶ) {
    ಸ್ಥಿತಿ IF2_IP_NOT_ಸಿಕ್ಕಿತು:
      ಆದರೆ(iface_ಒಳ && !host_ಒಳ) {
        /* Do not fall back to treating it as a hostname */
        ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
        ದತ್ತಾಂಶ->state.os_errno = sockerr = SOCKERRNO;
        failf(ದತ್ತಾಂಶ, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
        ಫಲಿತಾಂಶ = CURLE_INTERFACE_FAILED;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      ಫಲಿತಾಂಶ = CURLE_UNSUPPORTED_PROTOCOL;
      ಹೋಗು ಹೊರಹೋಗು;
    ಸ್ಥಿತಿ IF2_IP_ಸಿಕ್ಕಿತು:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(ದತ್ತಾಂಶ, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      ಮುಗಿದ = 1;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(!iface_ಒಳ || host_ಒಳ) {
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
      ಆದರೆ(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#ಆದರೆ_ಅಂತ್ಯ

      (ಶೂನ್ಯ)Curl_resolv_blocking(ದತ್ತಾಂಶ, dns_queries, host, 80, transport, &h);
      ಆದರೆ(h) {
        ಪೂರ್ಣಾಂಕ h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_ವಿಳಾಸ(h->addr, myhost, ಗಾತ್ರ(myhost));
        infof(ದತ್ತಾಂಶ, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_ನಮೂದು_unlink(ದತ್ತಾಂಶ, &h); /* this will NULL, potential free h */
        ಆದರೆ(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          ಫಲಿತಾಂಶ = CURLE_UNSUPPORTED_PROTOCOL;
          ಹೋಗು ಹೊರಹೋಗು;
        }
        ಮುಗಿದ = 1;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        ಮುಗಿದ = -1;
      }
    }

    ಆದರೆ(ಮುಗಿದ > 0) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
      /* IPv6 address */
      ಆದರೆ(af == AF_INET6) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        ಅಕ್ಷರ *scope_ptr = strchr(myhost, '%');
        ಆದರೆ(scope_ptr)
          *(scope_ptr++) = '\0';
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(ಪೋರ್ಟ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          ಆದರೆ(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              ಫಲಿತಾಂಶ = CURLE_UNSUPPORTED_PROTOCOL;
              ಹೋಗು ಹೊರಹೋಗು;
            }
            si6->sin6_scope_id = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)scope_id;
          }
#ಆದರೆ_ಅಂತ್ಯ
        }
        sizeof_sa = ಗಾತ್ರ(ರಚನೆ sockaddr_in6);
      }
      ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
      /* IPv4 address */
      ಆದರೆ((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(ಪೋರ್ಟ್);
        sizeof_sa = ಗಾತ್ರ(ರಚನೆ sockaddr_in);
      }
    }

    ಆದರೆ(ಮುಗಿದ < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      ದತ್ತಾಂಶ->state.errorbuf = FALSE;
      ದತ್ತಾಂಶ->state.os_errno = sockerr = SOCKERRNO;
      failf(ದತ್ತಾಂಶ, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಫಲಿತಾಂಶ = CURLE_INTERFACE_FAILED;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* no device was given, prepare sa to match af's needs */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಆದರೆ(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(ಪೋರ್ಟ್);
      sizeof_sa = ಗಾತ್ರ(ರಚನೆ sockaddr_in6);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(ಪೋರ್ಟ್);
      sizeof_sa = ಗಾತ್ರ(ರಚನೆ sockaddr_in);
    }
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ IP_BIND_ADDRESS_NO_PORT
  (ಶೂನ್ಯ)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, ಗಾತ್ರ(on));
#ಆದರೆ_ಅಂತ್ಯ
  ಗಾಗಿ(;;) {
    ಆದರೆ(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(ದತ್ತಾಂಶ, "Local port: %hu", ಪೋರ್ಟ್);
      conn->ಬಿಟ್ಟುಗಳು.bound = TRUE;
      ಫಲಿತಾಂಶ = CURLE_OK;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(--portnum > 0) {
      ಪೋರ್ಟ್++; /* try next port */
      ಆದರೆ(ಪೋರ್ಟ್ == 0)
        ನಿಲ್ಲಿಸು;
      infof(ದತ್ತಾಂಶ, "Bind to local port %d failed, trying next", ಪೋರ್ಟ್ - 1);
      /* We reuse/clobber the port variable here below */
      ಆದರೆ(sock->sa_family == AF_INET)
        si4->sin_port = htons(ಪೋರ್ಟ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
      ಇಲ್ಲದಿದ್ದರೆ
        si6->sin6_port = htons(ಪೋರ್ಟ್);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ನಿಲ್ಲಿಸು;
  }
  {
    ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
    ದತ್ತಾಂಶ->state.os_errno = sockerr = SOCKERRNO;
    failf(ದತ್ತಾಂಶ, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
  }

  ಫಲಿತಾಂಶ = CURLE_INTERFACE_FAILED;

ಹೊರಹೋಗು:
  curlx_free(dev_ಒಳ);
  curlx_free(iface_ಒಳ);
  curlx_free(host_ಒಳ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
ಸ್ಥಿರ bool verifyconnect(curl_socket_t sockfd, ಪೂರ್ಣಾಂಕ *psockerr)
{
  bool rc = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SO_ERROR
  ಪೂರ್ಣಾಂಕ sockerr = 0;
  curl_socklen_t err_ಗಾತ್ರ = ಗಾತ್ರ(sockerr);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
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
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (ಶೂನ್ಯ *)&sockerr, &err_ಗಾತ್ರ))
    sockerr = SOCKERRNO;
#ಆದರೆ defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  ಆದರೆ(EBADIOCTL == sockerr) {
    ಗಣ_SOCKERRNO(0);
    sockerr = 0;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  ಇಲ್ಲದಿದ್ದರೆ
    /* This was not a successful connect */
    rc = FALSE;
  ಆದರೆ(psockerr)
    *psockerr = sockerr;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)sockfd;
  ಆದರೆ(psockerr)
    *psockerr = SOCKERRNO;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
ಸ್ಥಿರ CURLcode socket_connect_ಫಲಿತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ipaddress, ಪೂರ್ಣಾಂಕ sockerr)
{
  ಆದರೆ(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN]);
    infof(ದತ್ತಾಂಶ, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    NOVERBOSE((ಶೂನ್ಯ)ipaddress);
  }
  ದತ್ತಾಂಶ->state.os_errno = sockerr;
  /* connect failed */
  ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
}

ರಚನೆ cf_socket_ctx {
  ರಚನೆ Curl_peer *peer;
  ರಚನೆ Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  ರಚನೆ ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  ರಚನೆ curltime started_at;        /* when socket was created */
  ರಚನೆ curltime connected_at;      /* when socket connected/got first byte */
  ರಚನೆ curltime ಮೊದಲ_byte_at;     /* when first byte was recvd */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  ರಚನೆ curltime ಕೊನೆಯ_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_ಗಾತ್ರ;                     /* the last set SO_SNDBUF size */
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ sockerr;                       /* socket error of last failure or 0 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಪೂರ್ಣಾಂಕ connect_wait_ms;               /* ms to wait after 1st connect() */
  ಪೂರ್ಣಾಂಕ wblock_percent;                /* percent of writes doing EAGAIN */
  ಪೂರ್ಣಾಂಕ wpartial_percent;              /* percent of bytes written in send */
  ಪೂರ್ಣಾಂಕ rblock_percent;                /* percent of reads doing EAGAIN */
  size_t recv_max;                   /* max enforced read size */
#ಆದರೆ_ಅಂತ್ಯ
  uint8_t transport;
  ಬಿಟ್(got_ಮೊದಲ_byte);               /* if first byte was received */
  ಬಿಟ್(listening);                    /* socket is listening */
  ಬಿಟ್(accepted);                     /* socket was accepted, not connected */
  ಬಿಟ್(sock_connected);               /* socket is "connected", e.g. in UDP */
  ಬಿಟ್(ಸಕ್ರಿಯ);
  ಬಿಟ್(stats_reported);
};

ಸ್ಥಿರ CURLcode cf_socket_ctx_init(ರಚನೆ cf_socket_ctx *ctx,
                                   ರಚನೆ Curl_peer *peer,
                                   ರಚನೆ Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, ಗಾತ್ರ(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = getenv("CURL_DBG_SOCK_WBLOCK");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 100))
        ctx->wblock_percent = (ಪೂರ್ಣಾಂಕ)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 100))
        ctx->wpartial_percent = (ಪೂರ್ಣಾಂಕ)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 100))
        ctx->rblock_percent = (ಪೂರ್ಣಾಂಕ)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ))
        ctx->connect_wait_ms = (ಪೂರ್ಣಾಂಕ)l;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_socket_ctx_free(ರಚನೆ cf_socket_ctx *ctx)
{
  ಆದರೆ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_socket_shutdown(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   bool *ಮುಗಿದ)
{
  ಆದರೆ(cf->connected) {
    ರಚನೆ cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    ಆದರೆ(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ buf[1024];
      (ಶೂನ್ಯ)sread(ctx->sock, buf, ಗಾತ್ರ(buf));
    }
  }
  *ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_socket_destroy(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "destroy");
  ಆದರೆ(ctx) {
    ಆದರೆ(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      ಆದರೆ(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(ದತ್ತಾಂಶ, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_local_ip(ರಚನೆ Curl_cfilter *cf,
                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_ಪೋರ್ಟ್ = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GETSOCKNAME
  ಆದರೆ((ctx->sock != CURL_SOCKET_BAD) &&
     !(ದತ್ತಾಂಶ->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    ರಚನೆ Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = ಗಾತ್ರ(ರಚನೆ Curl_sockaddr_storage);

    memset(&ssloc, 0, ಗಾತ್ರ(ssloc));
    ಆದರೆ(getsockname(ctx->sock, (ರಚನೆ sockaddr *)&ssloc, &slen)) {
      VERBOSE(ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN]);
      VERBOSE(ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO);
      infof(ದತ್ತಾಂಶ, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      CURLcode ಫಲಿತಾಂಶ = sockaddr2string((ರಚನೆ sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_ಪೋರ್ಟ್);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        infof(ದತ್ತಾಂಶ, "ssloc inet_ntop() failed with %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ CURLcode ಗಣ_remote_ip(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  ಫಲಿತಾಂಶ = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_ಪೋರ್ಟ್);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(ದತ್ತಾಂಶ, "curl_sa_addr inet_ntop() failed with %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cf_socktype(ಪೂರ್ಣಾಂಕ x)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ x;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIO_TCP_INITIAL_RTO
#ವ್ಯಾಖ್ಯಾನಿಸು SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#ವ್ಯಾಖ್ಯಾನಿಸು TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#ಆದರೆ_ಅಂತ್ಯ /* SIO_TCP_INITIAL_RTO */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#ವ್ಯಾಖ್ಯಾನಿಸು TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ bool targets_localhost(ರಚನೆ cf_socket_ctx *ctx)
{
  ಹಿಂತಿರುಗಿ (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
ಸ್ಥಿರ ಶೂನ್ಯ tcplocalhost(ರಚನೆ Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  ಆದರೆ(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD ಬೈಟುಗಳು = 0;
    memset(&rto, 0, ಗಾತ್ರ(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    ಆದರೆ(curlx_verify_windows_ಆವೃತ್ತಿ(10, 0, 16299, PLATFORM_WINNT,
                                    ಆವೃತ್ತಿ_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    ಇಲ್ಲದಿದ್ದರೆ
      rto.MaxSynRetransmissions = 1;
    (ಶೂನ್ಯ)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, ಗಾತ್ರ(rto),
                   ಖಾಲಿ, 0, &ಬೈಟುಗಳು, ಖಾಲಿ, ಖಾಲಿ);
  }
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು tcplocalhost(x, y)
#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */

ಸ್ಥಿರ CURLcode cf_socket_open(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಂಕ error = 0;
  bool isconnected = FALSE;
  CURLcode ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
  bool is_tcp;

  DEBUGASSERT(ctx->sock == CURL_SOCKET_BAD);
  ctx->started_at = *Curl_pgrs_now(ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_USE_SOCK_NONBLOCK
  /* Do not tuck SOCK_NONBLOCK into socktype when opensocket callback is set
   * because we would not know how socktype is about to be used in the
   * callback, SOCK_NONBLOCK might get factored out before calling socket().
   */
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#ಆದರೆ_ಅಂತ್ಯ
  ಫಲಿತಾಂಶ = socket_open(ದತ್ತಾಂಶ, &ctx->addr, &ctx->sock);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = ಗಣ_remote_ip(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಆದರೆ(ctx->addr.family == AF_INET6) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    ಪೂರ್ಣಾಂಕ on = 0;
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (ಶೂನ್ಯ *)&on, ಗಾತ್ರ(on));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      ರಚನೆ sockaddr_in6 *sa6 = (ಶೂನ್ಯ *)&ctx->addr.curl_sa_addr;
      ಆದರೆ(sa6->sin6_scope_id)
        infof(ದತ್ತಾಂಶ, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_ಪೋರ್ಟ್,
              (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)sa6->sin6_scope_id);
      ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
        infof(ದತ್ತಾಂಶ, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_ಪೋರ್ಟ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    infof(ದತ್ತಾಂಶ, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_ಪೋರ್ಟ್);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#ಇಲ್ಲದಿದ್ದರೆ
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(is_tcp) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.tcp_nodelay)
      tcpnodelay(cf, ದತ್ತಾಂಶ, ctx->sock);

    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.tcp_keepalive)
      tcpkeepalive(cf, ದತ್ತಾಂಶ, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fsockopt) {
    /* activate callback for setting socket options */
    ರಚನೆ Curl_mapi_guard guard;
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fsockopt);
    error = ದತ್ತಾಂಶ->ಗಣ.fsockopt(ದತ್ತಾಂಶ->ಗಣ.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_ಕೊನೆ(&guard);

    ಆದರೆ(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(error) {
      ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  ಆದರೆ(ctx->addr.family == AF_INET
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
     || ctx->addr.family == AF_INET6
#ಆದರೆ_ಅಂತ್ಯ
    ) {
    ಫಲಿತಾಂಶ = bindlocal(ದತ್ತಾಂಶ, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
      }
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  ಆದರೆ(error < 0) {
    ಫಲಿತಾಂಶ = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    ಹೋಗು ಹೊರಹೋಗು;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    ಆದರೆ(error < 0) {
      ಫಲಿತಾಂಶ = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(ದತ್ತಾಂಶ, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(isconnected) {
    ಗಣ_local_ip(cf, ದತ್ತಾಂಶ);
    ctx->connected_at = *Curl_pgrs_now(ದತ್ತಾಂಶ);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, ctx->sock);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_connect(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      bool is_tcp_fastopen)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TCP_FASTOPEN_CONNECT
  ಪೂರ್ಣಾಂಕ optval = 1;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ rc = -1;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(is_tcp_fastopen) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONNECT_ದತ್ತಾಂಶ_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_ಲಭ್ಯ
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    ಆದರೆ(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = ಖಾಲಿ;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_ದತ್ತಾಂಶ_IDEMPOTENT,
                    ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  ಇಲ್ಲದಿದ್ದರೆ
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#ಅಥವಾಆದರೆ defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    ಆದರೆ(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (ಶೂನ್ಯ *)&optval, ಗಾತ್ರ(optval)) < 0)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#ಅಥವಾಆದರೆ defined(MSG_FASTOPEN) /* old Linux */
    ಆದರೆ(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    ಇಲ್ಲದಿದ್ದರೆ
      rc = 0; /* Do nothing */
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ CURLcode cf_tcp_connect(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               bool *ಮುಗಿದ)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
  ಪೂರ್ಣಾಂಕ rc = 0;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *ಮುಗಿದ = FALSE; /* a negative world view is best */
  ಆದರೆ(ctx->sock == CURL_SOCKET_BAD) {
    ಪೂರ್ಣಾಂಕ sockerr;

    ಫಲಿತಾಂಶ = cf_socket_open(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    ಆದರೆ(cf->connected) {
      *ಮುಗಿದ = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, ದತ್ತಾಂಶ, (bool)cf->conn->ಬಿಟ್ಟುಗಳು.tcp_fastopen);
    sockerr = SOCKERRNO;
    ಗಣ_local_ip(cf, ದತ್ತಾಂಶ);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_ಪೋರ್ಟ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    ಆದರೆ(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(rc == -1) {
      ctx->sockerr = sockerr;
      ಫಲಿತಾಂಶ = socket_connect_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ, ctx->ip.remote_ip, sockerr);
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (ಶೂನ್ಯ)verifyconnect(ctx->sock, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  ಆದರೆ(rc == 0) { /* no connection yet */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == CURL_CSELECT_ಹೊರಹೋಗು ||
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#ಆದರೆ_ಅಂತ್ಯ
          cf->conn->ಬಿಟ್ಟುಗಳು.tcp_fastopen) {
    ಆದರೆ(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(ದತ್ತಾಂಶ);
      ಗಣ_local_ip(cf, ದತ್ತಾಂಶ);
      *ಮುಗಿದ = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (ಶೂನ್ಯ)verifyconnect(ctx->sock, &ctx->sockerr);
    ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    VERBOSE(ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN]);
    ಗಣ_local_ip(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ctx->sockerr) {
      ದತ್ತಾಂಶ->state.os_errno = ctx->sockerr;
      ಗಣ_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      VERBOSE(curlx_strcopy(ಬಫರ್, ಗಾತ್ರ(ಬಫರ್), STRCONST("peer closed")));
    }
    ಆದರೆ(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(ದತ್ತಾಂಶ, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(ದತ್ತಾಂಶ, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_ಪೋರ್ಟ್,
          ctx->ip.local_ip, ctx->ip.local_ಪೋರ್ಟ್,
          curlx_strerror(ctx->sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    *ಮುಗಿದ = FALSE;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_socket_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    ಆದರೆ(ctx->listening) {
      ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ_ಒಳ_only(ದತ್ತಾಂಶ, ps, ctx->sock);
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!cf->connected) {
      ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ_ಹೊರಹೋಗು_only(ದತ್ತಾಂಶ, ps, ctx->sock);
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ctx->ಸಕ್ರಿಯ) {
      ಫಲಿತಾಂಶ = Curl_pollset_ಕೂಡಿಸು_ಒಳ(ದತ್ತಾಂಶ, ps, ctx->sock);
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIO_IDEAL_SEND_BACKLOG_QUERY
#ವ್ಯಾಖ್ಯಾನಿಸು SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ win_update_sndbuf_ಗಾತ್ರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  ಆದರೆ(curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ),
                        &ctx->ಕೊನೆಯ_sndbuf_query_at) > 1000) {
    ಆದರೆ(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, ಗಾತ್ರ(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_ಗಾತ್ರ &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&ideal, ಗಾತ್ರ(ideal))) {
      ctx->sndbuf_ಗಾತ್ರ = ideal;
    }
    ctx->ಕೊನೆಯ_sndbuf_query_at = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  }
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_WINSOCK */

ಸ್ಥಿರ CURLcode cf_socket_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  /* simulate network blocking/partial writes */
  ಆದರೆ(ctx->wblock_percent > 0) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c = 0;
    Curl_rand_ಬೈಟುಗಳು(ದತ್ತಾಂಶ, FALSE, &c, 1);
    ಆದರೆ(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
  }
  ಆದರೆ(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    ಆದರೆ(!len)
      len = 1;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  ಆದರೆ(cf->conn->ಬಿಟ್ಟುಗಳು.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->ಬಿಟ್ಟುಗಳು.tcp_fastopen = FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    rv = swrite(ctx->sock, buf, len);

  ಆದರೆ(!curlx_sztouz(rv, pnwritten)) {
    ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;
    ಆದರೆ(SOCK_EAGAIN(sockerr)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#ಆದರೆ_ಅಂತ್ಯ
      ) {
      ಫಲಿತಾಂಶ = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      failf(ದತ್ತಾಂಶ, "Send failure: %s",
            curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ದತ್ತಾಂಶ->state.os_errno = sockerr;
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    win_update_sndbuf_ಗಾತ್ರ(ದತ್ತಾಂಶ, ctx);
#ಆದರೆ_ಅಂತ್ಯ

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_socket_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಅಕ್ಷರ *buf, size_t len, size_t *pnread)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  /* simulate network blocking/partial reads */
  ಆದರೆ(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c = 0;
    Curl_rand(ದತ್ತಾಂಶ, &c, 1);
    ಆದರೆ(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
  }
  ಆದರೆ(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#ಆದರೆ_ಅಂತ್ಯ

  rv = sread(ctx->sock, buf, len);

  ಆದರೆ(!curlx_sztouz(rv, pnread)) {
    ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;
    ಆದರೆ(SOCK_EAGAIN(sockerr)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINSOCK
       || (sockerr == SOCKEINTR)
#ಆದರೆ_ಅಂತ್ಯ
      ) {
      ಫಲಿತಾಂಶ = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      failf(ದತ್ತಾಂಶ, "Recv failure: %s",
            curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ದತ್ತಾಂಶ->state.os_errno = sockerr;
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "recv(len=%zu) -> %d, %zu", len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread);
  ಆದರೆ(!ಫಲಿತಾಂಶ && !ctx->got_ಮೊದಲ_byte) {
    ctx->ಮೊದಲ_byte_at = *Curl_pgrs_now(ದತ್ತಾಂಶ);
    ctx->got_ಮೊದಲ_byte = TRUE;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_socket_update_ದತ್ತಾಂಶ(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* Update the IP info held in the transfer, if we have that. */
  ಆದರೆ(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
    ದತ್ತಾಂಶ->info.primary = ctx->ip;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_socket_ಸಕ್ರಿಯ(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  ಗಣ_local_ip(cf, ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಆದರೆ(cf->sockindex == FIRSTSOCKET)
    cf->conn->ಬಿಟ್ಟುಗಳು.ipv6 = (ctx->addr.family == AF_INET6);
#ಆದರೆ_ಅಂತ್ಯ
  ctx->ಸಕ್ರಿಯ = TRUE;
}

ಸ್ಥಿರ CURLcode cf_socket_cntrl(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ಆಯ್ಕೆ(event) {
  ಸ್ಥಿತಿ CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_ಸಕ್ರಿಯ(cf, ದತ್ತಾಂಶ);
    cf_socket_update_ದತ್ತಾಂಶ(cf, ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ:
    cf_socket_update_ದತ್ತಾಂಶ(cf, ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_REPORT_STATS:
    ಆದರೆ(cf->connected && !ctx->stats_reported) {
      ರಚನೆ curltime *ts = ಖಾಲಿ;
      ಆಯ್ಕೆ(ctx->transport) {
      ಸ್ಥಿತಿ TRNSPRT_UDP:
      ಸ್ಥಿತಿ TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        ಆದರೆ(ctx->got_ಮೊದಲ_byte)
          ts = &ctx->ಮೊದಲ_byte_at;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        ts = &ctx->connected_at;
        ನಿಲ್ಲಿಸು;
      }
      ಆದರೆ(ts) {
        Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool cf_socket_conn_is_alive(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    bool *ಒಳಹರಿವು_pending)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  ರಚನೆ pollfd pfd[1];
  ಪೂರ್ಣಾಂಕ r;

  *ಒಳಹರಿವು_pending = FALSE;

  ಆದರೆ(!ctx || ctx->sock == CURL_SOCKET_BAD)
    ಹಿಂತಿರುಗಿ FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  ಆದರೆ(r < 0) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "is_alive: poll error, assume dead");
    ಹಿಂತಿರುಗಿ FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(r == 0) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "is_alive: poll timeout, assume alive");
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "is_alive: err/hup/etc events, assume dead");
    ಹಿಂತಿರುಗಿ FALSE;
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "is_alive: valid events, looks alive");
  *ಒಳಹರಿವು_pending = TRUE;
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ CURLcode cf_socket_query(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;

  ಆಯ್ಕೆ(query) {
  ಸ್ಥಿತಿ CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_CONNECT_ಉತ್ತರ_MS:
    ಆದರೆ(ctx->got_ಮೊದಲ_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->ಮೊದಲ_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? (ಪೂರ್ಣಾಂಕ)ms : ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      *pres1 = -1;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_IP_INFO:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#ಇಲ್ಲದಿದ್ದರೆ
    *pres1 = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    *(ರಚನೆ ip_quadruple *)pres2 = ctx->ip;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_REALLY_CONNECTED:
    ಆದರೆ(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    ಇಲ್ಲದಿದ್ದರೆ
      *pres1 = ctx->got_ಮೊದಲ_byte;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ರಚನೆ Curl_cftype Curl_cft_tcp = {
  "TCP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ದತ್ತಾಂಶ_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_tcp_create(ರಚನೆ Curl_cfilter **pcf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_peer *origin,
                            ರಚನೆ Curl_peer *peer,
                            uint8_t transport_peer,
                            ರಚನೆ connectdata *conn,
                            ರಚನೆ Curl_sockaddr_ex *addr,
                            ರಚನೆ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ರಚನೆ cf_socket_ctx *ctx = ಖಾಲಿ;
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  ಆದರೆ(!addr) {
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

ಹೊರಹೋಗು:
  *pcf = (!ಫಲಿತಾಂಶ) ? cf : ಖಾಲಿ;
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __linux__
ಸ್ಥಿರ ಶೂನ್ಯ linux_quic_mtu(ರಚನೆ cf_socket_ctx *ctx)
{
  ಪೂರ್ಣಾಂಕ val;
  ಆಯ್ಕೆ(ctx->addr.family) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ IP_MTU_DISCOVER
  ಸ್ಥಿತಿ AF_INET:
    val = IP_PMTUDISC_DO;
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     ಗಾತ್ರ(val));
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ IPV6_MTU_DISCOVER
  ಸ್ಥಿತಿ AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     ಗಾತ್ರ(val));
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  }
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು linux_quic_mtu(x)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
ಸ್ಥಿರ ಶೂನ್ಯ linux_quic_gro(ರಚನೆ cf_socket_ctx *ctx)
{
  ಪೂರ್ಣಾಂಕ one = 1;
  (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)ಗಾತ್ರ(one));
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು linux_quic_gro(x)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
ಸ್ಥಿರ ಶೂನ್ಯ linux_quic_ecn(ರಚನೆ cf_socket_ctx *ctx)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ tos = 1;
  ಆಯ್ಕೆ(ctx->addr.family) {
  ಸ್ಥಿತಿ AF_INET:
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, ಗಾತ್ರ(tos));
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ IPV6_RECVTCLASS
  ಸ್ಥಿತಿ AF_INET6:
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, ಗಾತ್ರ(tos));
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  }
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು linux_quic_ecn(x)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
ಸ್ಥಿರ ಶೂನ್ಯ linux_ip_dontfrag(ರಚನೆ cf_socket_ctx *ctx)
{
  ಪೂರ್ಣಾಂಕ val = 1;

  ಆಯ್ಕೆ(ctx->addr.family) {
  ಸ್ಥಿತಿ AF_INET:
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, ಗಾತ್ರ(val));
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ IPV6_DONTFRAG
  ಸ್ಥಿತಿ AF_INET6:
    (ಶೂನ್ಯ)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, ಗಾತ್ರ(val));
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  }
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು linux_ip_dontfrag(x)
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode cf_udp_ಸಿದ್ಧತೆ_quic(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಂಕ rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  ಆದರೆ(rc == -1) {
    ಹಿಂತಿರುಗಿ socket_connect_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ, ctx->ip.remote_ip, SOCKERRNO);
  }
  ctx->sock_connected = TRUE;
  ಗಣ_local_ip(cf, ದತ್ತಾಂಶ);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "%s socket %" FMT_SOCKET_T
              " connected: [%s:%d] -> [%s:%d]",
              (ctx->transport == TRNSPRT_QUIC) ? "QUIC" : "UDP",
              ctx->sock, ctx->ip.local_ip, ctx->ip.local_ಪೋರ್ಟ್,
              ctx->ip.remote_ip, ctx->ip.remote_ಪೋರ್ಟ್);

  /* Currently, cf->ctx->sock is always non-blocking because the only
   * caller to cf_udp_setup_quic() is cf_udp_connect() that passes the
   * non-blocking socket created by cf_socket_open() to it. Thus, we
   * do not need to call curlx_nonblock() in cf_udp_setup_quic() anymore.
   */
  linux_quic_ecn(ctx);
  linux_quic_mtu(ctx);
  linux_ip_dontfrag(ctx);
  linux_quic_gro(ctx);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_udp_connect(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               bool *ಮುಗಿದ)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *ಮುಗಿದ = FALSE;
  ಆದರೆ(ctx->sock == CURL_SOCKET_BAD) {
    ಫಲಿತಾಂಶ = cf_socket_open(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_udp_connect(), open failed -> %d",
                  (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(ctx->transport == TRNSPRT_QUIC) {
      ಫಲಿತಾಂಶ = cf_udp_ಸಿದ್ಧತೆ_quic(cf, ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_ಪೋರ್ಟ್);
    }
    *ಮುಗಿದ = TRUE;
    cf->connected = TRUE;
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ರಚನೆ Curl_cftype Curl_cft_udp = {
  "UDP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_udp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ದತ್ತಾಂಶ_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_udp_create(ರಚನೆ Curl_cfilter **pcf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_peer *origin,
                            ರಚನೆ Curl_peer *peer,
                            uint8_t transport_peer,
                            ರಚನೆ connectdata *conn,
                            ರಚನೆ Curl_sockaddr_ex *addr,
                            ರಚನೆ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ರಚನೆ cf_socket_ctx *ctx = ಖಾಲಿ;
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

ಹೊರಹೋಗು:
  *pcf = (!ಫಲಿತಾಂಶ) ? cf : ಖಾಲಿ;
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* this is the TCP filter which can also handle this case */
ರಚನೆ Curl_cftype Curl_cft_unix = {
  "UNIX",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ದತ್ತಾಂಶ_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_unix_create(ರಚನೆ Curl_cfilter **pcf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_peer *origin,
                            ರಚನೆ Curl_peer *peer,
                            uint8_t transport_peer,
                            ರಚನೆ connectdata *conn,
                            ರಚನೆ Curl_sockaddr_ex *addr,
                            ರಚನೆ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  ರಚನೆ cf_socket_ctx *ctx = ಖಾಲಿ;
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

ಹೊರಹೋಗು:
  *pcf = (!ಫಲಿತಾಂಶ) ? cf : ಖಾಲಿ;
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ timediff_t cf_tcp_accept_timeleft(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.accepttimeout > 0)
    timeout_ms = ದತ್ತಾಂಶ->ಗಣ.accepttimeout;
#ಆದರೆ_ಅಂತ್ಯ

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
  ಆದರೆ(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  ಇಲ್ಲದಿದ್ದರೆ {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &ctx->started_at);
    ಆದರೆ(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  ಹಿಂತಿರುಗಿ timeout_ms;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_tcp_ಗಣ_accepted_remote_ip(ರಚನೆ Curl_cfilter *cf,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GETPEERNAME
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_ಪೋರ್ಟ್ = 0;
  plen = ಗಾತ್ರ(ssrem);
  memset(&ssrem, 0, plen);
  ಆದರೆ(getpeername(ctx->sock, (ರಚನೆ sockaddr *)&ssrem, &plen)) {
    ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
    ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;
    failf(ದತ್ತಾಂಶ, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಹಿಂತಿರುಗಿ;
  }
  ಫಲಿತಾಂಶ = sockaddr2string((ರಚನೆ sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_ಪೋರ್ಟ್);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    failf(ದತ್ತಾಂಶ, "ssrem inet_ntop() failed with %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_ಪೋರ್ಟ್ = 0;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ CURLcode cf_tcp_accept_connect(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      bool *ಮುಗಿದ)
{
  ರಚನೆ cf_socket_ctx *ctx = cf->ctx;
  ಅಕ್ಷರ errbuf[STRERROR_LEN];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ Curl_sockaddr_storage ಕೂಡಿಸು;
#ಇಲ್ಲದಿದ್ದರೆ
  ರಚನೆ sockaddr_in ಕೂಡಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  curl_socklen_t ಗಾತ್ರ_2 = (curl_socklen_t)ಗಾತ್ರ(ಕೂಡಿಸು);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  ಪೂರ್ಣಾಂಕ socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *ಮುಗಿದ = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(ದತ್ತಾಂಶ, "Accept timeout occurred while waiting server connect");
    ಹಿಂತಿರುಗಿ CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_ಪೋರ್ಟ್);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "socket_check -> %x", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)socketstate);
  ಆಯ್ಕೆ(socketstate) {
  ಸ್ಥಿತಿ -1: /* error */
    /* let's die here */
    failf(ದತ್ತಾಂಶ, "Error while waiting for server connect");
    ಹಿಂತಿರುಗಿ CURLE_FTP_ACCEPT_FAILED;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ(socketstate & CURL_CSELECT_ಒಳ) {
      infof(ದತ್ತಾಂಶ, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(!incoming) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "nothing heard from the server yet");
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಗಾತ್ರ_2 = ಗಾತ್ರ(ಕೂಡಿಸು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (ರಚನೆ sockaddr *)&ಕೂಡಿಸು, &ಗಾತ್ರ_2,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#ಇಲ್ಲದಿದ್ದರೆ
  s_accepted = CURL_ACCEPT(ctx->sock, (ರಚನೆ sockaddr *)&ಕೂಡಿಸು, &ಗಾತ್ರ_2);
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(s_accepted == CURL_SOCKET_BAD) {
    failf(ದತ್ತಾಂಶ, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಗಾತ್ರ(errbuf)));
    ಹಿಂತಿರುಗಿ CURLE_FTP_ACCEPT_FAILED;
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_ACCEPT4
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCNTL
  ಆದರೆ(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(ದತ್ತಾಂಶ, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಗಾತ್ರ(errbuf)));
    Curl_socket_close(ದತ್ತಾಂಶ, cf->conn, s_accepted);
    ಹಿಂತಿರುಗಿ CURLE_FTP_ACCEPT_FAILED;
  }
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_FCNTL */
  ಆದರೆ(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(ದತ್ತಾಂಶ, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, ಗಾತ್ರ(errbuf)));
    Curl_socket_close(ದತ್ತಾಂಶ, cf->conn, s_accepted);
    ಹಿಂತಿರುಗಿ CURLE_FTP_ACCEPT_FAILED;
  }
#ಆದರೆ_ಅಂತ್ಯ /* !HAVE_ACCEPT4 */
  infof(ದತ್ತಾಂಶ, "Connection accepted from server");

  /* Replace any filter on SECONDARY with one listening on this socket */
  ctx->listening = FALSE;
  ctx->accepted = TRUE;
  socket_close(ದತ್ತಾಂಶ, cf->conn, TRUE, ctx->sock);
  ctx->sock = s_accepted;

  cf->conn->sock[cf->sockindex] = ctx->sock;
  cf_tcp_ಗಣ_accepted_remote_ip(cf, ದತ್ತಾಂಶ);
  ಗಣ_local_ip(cf, ದತ್ತಾಂಶ);
  ctx->ಸಕ್ರಿಯ = TRUE;
  ctx->connected_at = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  cf->connected = TRUE;
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "accepted_set(sock=%" FMT_SOCKET_T
              ", remote=%s port=%d)",
              ctx->sock, ctx->ip.remote_ip, ctx->ip.remote_ಪೋರ್ಟ್);

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fsockopt) {
    ರಚನೆ Curl_mapi_guard guard;
    ಪೂರ್ಣಾಂಕ error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fsockopt);
    error = ದತ್ತಾಂಶ->ಗಣ.fsockopt(ದತ್ತಾಂಶ->ಗಣ.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_ಕೊನೆ(&guard);

    ಆದರೆ(error)
      ಹಿಂತಿರುಗಿ CURLE_ABORTED_BY_CALLBACK;
  }
  *ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ರಚನೆ Curl_cftype Curl_cft_tcp_accept = {
  "TCP-ACCEPT",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_accept_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_ದತ್ತಾಂಶ_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_conn_tcp_listen_ಗಣ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ cf_socket_ctx *ctx = ಖಾಲಿ;

  /* replace any existing */
  Curl_conn_cf_discard_all(ದತ್ತಾಂಶ, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  Curl_conn_cf_ಕೂಡಿಸು(ದತ್ತಾಂಶ, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  conn->sock[sockindex] = ctx->sock;
  ಗಣ_local_ip(cf, ದತ್ತಾಂಶ);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_ಪೋರ್ಟ್);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_conn_is_tcp_listen(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             int8_t sockindex)
{
  ರಚನೆ Curl_cfilter *cf = ದತ್ತಾಂಶ->conn->cfilter[sockindex];
  ಆಗ(cf) {
    ಆದರೆ(cf->cft == &Curl_cft_tcp_accept)
      ಹಿಂತಿರುಗಿ TRUE;
    cf = cf->ಮುಂದಿನ;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
ಸ್ಥಿರ bool cf_is_socket(ರಚನೆ Curl_cfilter *cf)
{
  ಹಿಂತಿರುಗಿ cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             curl_socket_t *psock,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex **paddr,
                             ರಚನೆ ip_quadruple *pip)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(cf_is_socket(cf) && cf->ctx) {
    ರಚನೆ cf_socket_ctx *ctx = cf->ctx;

    ಆದರೆ(psock)
      *psock = ctx->sock;
    ಆದರೆ(paddr)
      *paddr = &ctx->addr;
    ಆದರೆ(pip)
      *pip = ctx->ip;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
}
