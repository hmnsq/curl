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

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h> /* <netinet/tcp.h> may need it */
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_UN_H
#अन्तर्भाव <sys/un.h> /* for sockaddr_un */
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_LINUX_TCP_H
#अन्तर्भाव <linux/tcp.h>
#नोचेद्यदि defined(HAVE_NETINET_TCP_H)
#अन्तर्भाव <netinet/tcp.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETINET_UDP_H
#अन्तर्भाव <netinet/udp.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETINET_IP_H
#अन्तर्भाव <netinet/ip.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_IFADDRS_H
#अन्तर्भाव <ifaddrs.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#यदि_अन्तः
#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

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
#यदि_परिभाषितम् _WIN32
#अन्तर्भाव <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#यदि_अन्तः

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(संरचना sockaddr *sa, curl_socklen_t salen,
                                  वर्ण *addr, uint16_t *पत्तनम्);
UNITTEST CURLcode sockaddr2string(संरचना sockaddr *sa, curl_socklen_t salen,
                                  वर्ण *addr, uint16_t *पत्तनम्)
{
  CURLcode परिणाम;
  संरचना sockaddr_in *si = रिक्त;
#यदि_परिभाषितम् USE_IPV6
  संरचना sockaddr_in6 *si6 = रिक्त;
#यदि_अन्तः
#यदि_परिभाषितम् USE_UNIX_SOCKETS
  संरचना sockaddr_un *su = रिक्त;
#अन्यथा
  (शून्य)salen;
#यदि_अन्तः

  विकल्प(sa->sa_family) {
  पक्ष AF_INET:
    si = (संरचना sockaddr_in *)(शून्य *)sa;
    परिणाम = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    यदि(!परिणाम) {
      *पत्तनम् = ntohs(si->sin_port);
      प्रत्यागम परिणाम;
    }
    भङ्ग;
#यदि_परिभाषितम् USE_IPV6
  पक्ष AF_INET6:
    si6 = (संरचना sockaddr_in6 *)(शून्य *)sa;
    परिणाम = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    यदि(!परिणाम) {
      *पत्तनम् = ntohs(si6->sin6_port);
      प्रत्यागम परिणाम;
    }
    भङ्ग;
#यदि_अन्तः
#यदि_परिभाषितम् USE_UNIX_SOCKETS
  पक्ष AF_UNIX:
    यदि(salen > (curl_socklen_t)परिमाण(CURL_SA_FAMILY_T)) {
      su = (संरचना sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_मार्गः);
    }
    अन्यथा
      addr[0] = 0; /* socket with no name */
    *पत्तनम् = 0;
    प्रत्यागम CURLE_OK;
#यदि_अन्तः
  स्वभाव:
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
    भङ्ग;
  }

  addr[0] = '\0';
  *पत्तनम् = 0;
  प्रत्यागम परिणाम;
}

स्थिर शून्य tcpnodelay(संरचना Curl_cfilter *cf,
                       संरचना Curl_easy *दत्त,
                       curl_socket_t sockfd)
{
#यदि defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  पूर्णाङ्क level = IPPROTO_TCP;
  VERBOSE(वर्ण बफर[STRERROR_LEN]);

  यदि(setsockopt(sockfd, level, TCP_NODELAY, (शून्य *)&onoff, परिमाण(onoff)) < 0)
    CURL_TRC_CF(दत्त, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, बफर, परिमाण(बफर)));
#अन्यथा
  (शून्य)cf;
  (शून्य)दत्त;
  (शून्य)sockfd;
#यदि_अन्तः
}

#यदि defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_देहली) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_देहली)
#यदि defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
स्थिर शून्य alive_unit(पूर्णाङ्क *x)
{
  /* make sure this doesn't wrap */
  यदि(*x < (अधिकतम_पूर्णाङ्कः / 1000))
    *x *= 1000;
  अन्यथा
    /* this is still almost 25 days */
    *x = अधिकतम_पूर्णाङ्कः;
}
#अन्यथा
#परिभाषय alive_unit(x)
#यदि_अन्तः
#यदि_अन्तः

स्थिर शून्य tcpkeepalive(संरचना Curl_cfilter *cf,
                         संरचना Curl_easy *दत्त,
                         curl_socket_t sockfd)
{
  पूर्णाङ्क optval = दत्त->समुच्चयः.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  यदि(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (शून्य *)&optval, परिमाण(optval)) < 0) {
    CURL_TRC_CF(दत्त, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  अन्यथा {
#यदि_परिभाषितम् USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    यदि(curlx_verify_windows_संस्करणम्(10, 0, 16299, PLATFORM_WINNT,
                                    संस्करणम्_GREATER_THAN_EQUAL)) {
      CURL_TRC_CF(दत्त, cf, "Set TCP_KEEP* on fd=%" FMT_SOCKET_T, sockfd);
      optval = curlx_sltosi(दत्त->समुच्चयः.tcp_keepidle);
/* Offered by mingw-w64 v12+, MS SDK 6.0A/VS2008+ */
#यदि_अपरिभाषितम् TCP_KEEPALIVE
#परिभाषय TCP_KEEPALIVE 3
#यदि_अन्तः
/* Offered by mingw-w64 v12+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#यदि_अपरिभाषितम् TCP_KEEPCNT
#परिभाषय TCP_KEEPCNT 16
#यदि_अन्तः
/* Offered by mingw-w64 v12+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#यदि_अपरिभाषितम् TCP_KEEPIDLE
#परिभाषय TCP_KEEPIDLE TCP_KEEPALIVE
#यदि_अन्तः
#यदि_अपरिभाषितम् TCP_KEEPINTVL
#परिभाषय TCP_KEEPINTVL 17
#यदि_अन्तः
      यदि(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (ध्रुव वर्ण *)&optval, परिमाण(optval)) < 0) {
        CURL_TRC_CF(दत्त, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(दत्त->समुच्चयः.tcp_keepintvl);
      यदि(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (ध्रुव वर्ण *)&optval, परिमाण(optval)) < 0) {
        CURL_TRC_CF(दत्त, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(दत्त->समुच्चयः.tcp_keepcnt);
      यदि(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (ध्रुव वर्ण *)&optval, परिमाण(optval)) < 0) {
        CURL_TRC_CF(दत्त, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    अन्यथा {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#यदि_अपरिभाषितम् SIO_KEEPALIVE_VALS
#परिभाषय SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      संरचना tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#यदि_अन्तः
      संरचना tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(दत्त->समुच्चयः.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(दत्त->समुच्चयः.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      यदि(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, परिमाण(vals),
                  रिक्त, 0, &dummy, रिक्त, रिक्त) != 0) {
        CURL_TRC_CF(दत्त, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#अन्यथा /* !USE_WINSOCK */
#यदि_परिभाषितम् TCP_KEEPIDLE
    optval = curlx_sltosi(दत्त->समुच्चयः.tcp_keepidle);
    alive_unit(&optval);
    यदि(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (शून्य *)&optval, परिमाण(optval)) < 0) {
      CURL_TRC_CF(दत्त, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#नोचेद्यदि defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(दत्त->समुच्चयः.tcp_keepidle);
    alive_unit(&optval);
    यदि(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (शून्य *)&optval, परिमाण(optval)) < 0) {
      CURL_TRC_CF(दत्त, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#नोचेद्यदि defined(TCP_KEEPALIVE_देहली)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(दत्त->समुच्चयः.tcp_keepidle);
    alive_unit(&optval);
    यदि(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_देहली,
                  (शून्य *)&optval, परिमाण(optval)) < 0) {
      CURL_TRC_CF(दत्त, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#यदि_अन्तः
#यदि_परिभाषितम् TCP_KEEPINTVL
    optval = curlx_sltosi(दत्त->समुच्चयः.tcp_keepintvl);
    alive_unit(&optval);
    यदि(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (शून्य *)&optval, परिमाण(optval)) < 0) {
      CURL_TRC_CF(दत्त, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#नोचेद्यदि defined(TCP_KEEPALIVE_ABORT_देहली)
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
      पूर्णाङ्क keepcnt = curlx_sltosi(दत्त->समुच्चयः.tcp_keepcnt);
      पूर्णाङ्क keepintvl = curlx_sltosi(दत्त->समुच्चयः.tcp_keepintvl);

      यदि(keepcnt > 0 && keepintvl > (अधिकतम_पूर्णाङ्कः / keepcnt))
        optval = अधिकतम_पूर्णाङ्कः;
      अन्यथा
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    यदि(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_देहली,
                  (शून्य *)&optval, परिमाण(optval)) < 0) {
      CURL_TRC_CF(दत्त, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#यदि_अन्तः
#यदि_परिभाषितम् TCP_KEEPCNT
    optval = curlx_sltosi(दत्त->समुच्चयः.tcp_keepcnt);
    यदि(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (शून्य *)&optval, परिमाण(optval)) < 0) {
      CURL_TRC_CF(दत्त, cf, "Failed to set TCP_KEEPCNT on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#यदि_अन्तः
#यदि_अन्तः /* USE_WINSOCK */
  }
}

/**
 * Assign the addrinfo `ai` to the Curl_sockaddr_ex `addr` with
 * transport determining socktype and protocol.
 */
CURLcode Curl_socket_addr_from_ai(संरचना Curl_sockaddr_ex *addr,
                                  ध्रुव संरचना Curl_addrinfo *ai,
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
  addr->addrlen = (अचिह्नित पूर्णाङ्क)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= परिमाण(addr->curl_sa_addrbuf));
  यदि(addr->addrlen > परिमाण(addr->curl_sa_addrbuf))
    प्रत्यागम CURLE_TOO_LARGE;

  स्मृतिं_प्रतिलिख(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  प्रत्यागम CURLE_OK;
}

#यदि_परिभाषितम् USE_SO_NOSIGPIPE
पूर्णाङ्क Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  पूर्णाङ्क onoff = 1;
  प्रत्यागम setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (शून्य *)&onoff, परिमाण(onoff));
}
#यदि_अन्तः /* USE_SO_NOSIGPIPE */

#यदि defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
स्थिर uint32_t get_scope_id(संरचना Curl_easy *दत्त,
                             संरचना sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  यदि(दत्त->conn->scope_id)
    प्रत्यागम दत्त->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  यदि(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#यदि defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    संरचना ifaddrs *ifa, *ifa_list;
    यदि(getifaddrs(&ifa_list) == 0) {
      प्रति(ifa = ifa_list; ifa; ifa = ifa->ifa_अग्र) {
        यदि(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          संरचना sockaddr_in6 *s6 = (शून्य *)ifa->ifa_addr;
          यदि(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(दत्त,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (अचिह्नित दीर्घ)scope_id);
            भङ्ग;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#यदि_अन्तः /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  प्रत्यागम scope_id;
}
#यदि_अन्तः

स्थिर CURLcode socket_open(संरचना Curl_easy *दत्त,
                            संरचना Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  वर्ण errbuf[STRERROR_LEN];

#यदि_परिभाषितम् SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#यदि_अन्तः

  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);
  यदि(दत्त->समुच्चयः.fopensocket) {
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
    CURL_CBAPI_START(&guard, दत्त, easy_fopensocket);
    *sockfd = दत्त->समुच्चयः.fopensocket(दत्त->समुच्चयः.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (संरचना curl_sockaddr *)addr);
    CURL_CBAPI_अन्तः(&guard);
  }
  अन्यथा {
    /* opensocket callback not set, so create the socket now */
#यदि_परिभाषितम् DEBUGBUILD
    यदि((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(दत्त, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }
#यदि_अन्तः
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    यदि((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  यदि(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(दत्त, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, परिमाण(errbuf)));
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

#यदि_परिभाषितम् USE_SO_NOSIGPIPE
  यदि(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(दत्त, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, परिमाण(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }
#यदि_अन्तः /* USE_SO_NOSIGPIPE */

#यदि defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  यदि(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(दत्त, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, परिमाण(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }
#यदि_अन्तः

#यदि defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  यदि(addr->family == AF_INET6) {
    संरचना sockaddr_in6 * ध्रुव sa6 = (शून्य *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(दत्त, sa6);
  }
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
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
CURLcode Curl_socket_open(संरचना Curl_easy *दत्त,
                          ध्रुव संरचना Curl_addrinfo *ai,
                          संरचना Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  संरचना Curl_sockaddr_ex dummy;
  CURLcode परिणाम;

  यदि(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  परिणाम = Curl_socket_addr_from_ai(addr, ai, transport);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  प्रत्यागम socket_open(दत्त, addr, sockfd);
}

स्थिर पूर्णाङ्क socket_close(संरचना Curl_easy *दत्त, संरचना connectdata *conn,
                        पूर्णाङ्क use_callback, curl_socket_t sock)
{
  यदि(sock == CURL_SOCKET_BAD)
    प्रत्यागम 0;

  यदि(use_callback && conn && conn->fclosesocket) {
    संरचना Curl_mapi_guard guard;
    पूर्णाङ्क rc;
    Curl_multi_will_close(दत्त, sock);
    CURL_CBAPI_START(&guard, दत्त, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_अन्तः(&guard);
    प्रत्यागम rc;
  }

  यदि(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(दत्त, sock);

  sclose(sock);

  प्रत्यागम 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
पूर्णाङ्क Curl_socket_close(संरचना Curl_easy *दत्त, संरचना connectdata *conn,
                      curl_socket_t sock)
{
  प्रत्यागम socket_close(दत्त, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(ध्रुव वर्ण *निवेशः,
                              वर्ण **dev, वर्ण **iface, वर्ण **host)
{
  स्थिर ध्रुव वर्ण if_उपसर्गः[] = "if!";
  स्थिर ध्रुव वर्ण host_उपसर्गः[] = "host!";
  स्थिर ध्रुव वर्ण if_host_उपसर्गः[] = "ifhost!";
  size_t len;

  len = वाक्यदैर्घ्यम्(निवेशः);
  यदि(len > 512)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  यदि(dev)
    *dev = रिक्त;
  यदि(iface)
    *iface = रिक्त;
  यदि(host)
    *host = रिक्त;

  यदि(!वाक्ये_तुलय(if_उपसर्गः, निवेशः, वाक्यदैर्घ्यम्(if_उपसर्गः))) {
    निवेशः += वाक्यदैर्घ्यम्(if_उपसर्गः);
    यदि(!*निवेशः)
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    यदि(iface) {
      *iface = curlx_memdup0(निवेशः, len - वाक्यदैर्घ्यम्(if_उपसर्गः));
      प्रत्यागम *iface ? CURLE_OK : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    प्रत्यागम CURLE_OK;
  }
  अन्यथा यदि(!वाक्ये_तुलय(host_उपसर्गः, निवेशः, वाक्यदैर्घ्यम्(host_उपसर्गः))) {
    निवेशः += वाक्यदैर्घ्यम्(host_उपसर्गः);
    यदि(!*निवेशः)
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    यदि(host) {
      *host = curlx_memdup0(निवेशः, len - वाक्यदैर्घ्यम्(host_उपसर्गः));
      प्रत्यागम *host ? CURLE_OK : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    प्रत्यागम CURLE_OK;
  }
  अन्यथा यदि(!वाक्ये_तुलय(if_host_उपसर्गः, निवेशः, वाक्यदैर्घ्यम्(if_host_उपसर्गः))) {
    ध्रुव वर्ण *host_part;
    निवेशः += वाक्यदैर्घ्यम्(if_host_उपसर्गः);
    len -= वाक्यदैर्घ्यम्(if_host_उपसर्गः);
    host_part = memchr(निवेशः, '!', len);
    यदि(!host_part || !*(host_part + 1))
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    यदि(iface) {
      *iface = curlx_memdup0(निवेशः, host_part - निवेशः);
      यदि(!*iface)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    ++host_part;
    यदि(host) {
      *host = curlx_memdup0(host_part, len - (host_part - निवेशः));
      यदि(!*host) {
        यदि(iface)
          curlx_safefree(*iface);
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
    }
    प्रत्यागम CURLE_OK;
  }

  यदि(!*निवेशः)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  यदि(dev) {
    *dev = curlx_memdup0(निवेशः, len);
    प्रत्यागम *dev ? CURLE_OK : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  प्रत्यागम CURLE_OK;
}

#यदि_अपरिभाषितम् CURL_DISABLE_BINDLOCAL
स्थिर CURLcode bindlocal(संरचना Curl_easy *दत्त, संरचना connectdata *conn,
                          curl_socket_t sockfd, पूर्णाङ्क af, अचिह्नित पूर्णाङ्क scope,
                          uint8_t transport)
{
  संरचना Curl_sockaddr_storage sa;
  संरचना sockaddr *sock = (संरचना sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  संरचना sockaddr_in *si4 = (संरचना sockaddr_in *)&sa;
#यदि_परिभाषितम् USE_IPV6
  संरचना sockaddr_in6 *si6 = (संरचना sockaddr_in6 *)&sa;
#यदि_अन्तः
  संरचना Curl_dns_प्रविष्टिः *h = रिक्त;
  अचिह्नित ह्रस्व पत्तनम् = दत्त->समुच्चयः.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  पूर्णाङ्क portnum = दत्त->समुच्चयः.localportrange;
  वर्ण *dev_अन्तर् = रिक्त, *iface_अन्तर् = रिक्त, *host_अन्तर् = रिक्त;
  ध्रुव वर्ण *iface = रिक्त, *host = रिक्त;
  पूर्णाङ्क sockerr;
#यदि_परिभाषितम् IP_BIND_ADDRESS_NO_PORT
  पूर्णाङ्क on = 1;
#यदि_अन्तः
  CURLcode परिणाम = CURLE_OK;

#यदि_अपरिभाषितम् USE_IPV6
  (शून्य)scope;
#यदि_अन्तः
  यदि(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    परिणाम = Curl_parse_interface(conn->interface_opts,
                                  &dev_अन्तर्, &iface_अन्तर्, &host_अन्तर्);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    /* This is how we use the values */
    iface = iface_अन्तर् ? iface_अन्तर् : dev_अन्तर्;
    host = host_अन्तर् ? host_अन्तर् : dev_अन्तर्;
  }

  यदि(!iface && !host && !पत्तनम्)
    गच्छ बहिर्गमनम्;

  यदि(iface && (वाक्यदैर्घ्यम्(iface) >= 255)) {
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }
  स्मृतिं_पूरय(&sa, 0, परिमाण(संरचना Curl_sockaddr_storage));

  यदि(iface || host) {
    वर्ण myhost[256] = "";
    पूर्णाङ्क समाप्तम् = 0; /* -1 for error, 1 for address found */
    if2ip_परिणाम_t if2ip_परिणाम = IF2_IP_NOT_प्राप्तम्;

#यदि_परिभाषितम् SO_BINDTODEVICE
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
    यदि(iface &&
       setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                  iface, (curl_socklen_t)वाक्यदैर्घ्यम्(iface) + 1) == 0 &&
       /* This is often "errno 1, error: Operation not permitted" if you are
        * not running as root or another suitable privileged user. If it
        * succeeds it means the parameter was a valid interface and not an IP
        * address. Return immediately.
        */
       !host_अन्तर्) {
      infof(दत्त, "socket successfully bound to interface '%s'", iface);
      परिणाम = CURLE_OK;
      गच्छ बहिर्गमनम्;
    }
#यदि_अन्तः
    यदि(!host_अन्तर्) {
      /* Discover IP from input device, then bind to it */
      if2ip_परिणाम = Curl_if2ip(af,
#यदि_परिभाषितम् USE_IPV6
                                scope, conn->scope_id,
#यदि_अन्तः
                                iface, myhost, परिमाण(myhost));
    }
    विकल्प(if2ip_परिणाम) {
    पक्ष IF2_IP_NOT_प्राप्तम्:
      यदि(iface_अन्तर् && !host_अन्तर्) {
        /* Do not fall back to treating it as a hostname */
        वर्ण बफर[STRERROR_LEN];
        दत्त->state.os_errno = sockerr = SOCKERRNO;
        failf(दत्त, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, बफर, परिमाण(बफर)));
        परिणाम = CURLE_INTERFACE_FAILED;
        गच्छ बहिर्गमनम्;
      }
      भङ्ग;
    पक्ष IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
      गच्छ बहिर्गमनम्;
    पक्ष IF2_IP_प्राप्तम्:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(दत्त, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      समाप्तम् = 1;
      भङ्ग;
    }
    यदि(!iface_अन्तर् || host_अन्तर्) {
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
#यदि_परिभाषितम् USE_IPV6
      यदि(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#यदि_अन्तः

      (शून्य)Curl_resolv_blocking(दत्त, dns_queries, host, 80, transport, &h);
      यदि(h) {
        पूर्णाङ्क h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_स्थानाङ्कः(h->addr, myhost, परिमाण(myhost));
        infof(दत्त, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_प्रविष्टिः_unlink(दत्त, &h); /* this will NULL, potential free h */
        यदि(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
          गच्छ बहिर्गमनम्;
        }
        समाप्तम् = 1;
      }
      अन्यथा {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        समाप्तम् = -1;
      }
    }

    यदि(समाप्तम् > 0) {
#यदि_परिभाषितम् USE_IPV6
      /* IPv6 address */
      यदि(af == AF_INET6) {
#यदि_परिभाषितम् HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        वर्ण *scope_ptr = strchr(myhost, '%');
        यदि(scope_ptr)
          *(scope_ptr++) = '\0';
#यदि_अन्तः
        यदि(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(पत्तनम्);
#यदि_परिभाषितम् HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          यदि(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            यदि(curlx_str_सङ्ख्या((ध्रुव वर्ण **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
              गच्छ बहिर्गमनम्;
            }
            si6->sin6_scope_id = (अचिह्नित पूर्णाङ्क)scope_id;
          }
#यदि_अन्तः
        }
        sizeof_sa = परिमाण(संरचना sockaddr_in6);
      }
      अन्यथा
#यदि_अन्तः
      /* IPv4 address */
      यदि((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(पत्तनम्);
        sizeof_sa = परिमाण(संरचना sockaddr_in);
      }
    }

    यदि(समाप्तम् < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      वर्ण बफर[STRERROR_LEN];
      दत्त->state.errorbuf = FALSE;
      दत्त->state.os_errno = sockerr = SOCKERRNO;
      failf(दत्त, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, बफर, परिमाण(बफर)));
      परिणाम = CURLE_INTERFACE_FAILED;
      गच्छ बहिर्गमनम्;
    }
  }
  अन्यथा {
    /* no device was given, prepare sa to match af's needs */
#यदि_परिभाषितम् USE_IPV6
    यदि(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(पत्तनम्);
      sizeof_sa = परिमाण(संरचना sockaddr_in6);
    }
    अन्यथा
#यदि_अन्तः
    यदि(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(पत्तनम्);
      sizeof_sa = परिमाण(संरचना sockaddr_in);
    }
  }
#यदि_परिभाषितम् IP_BIND_ADDRESS_NO_PORT
  (शून्य)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, परिमाण(on));
#यदि_अन्तः
  प्रति(;;) {
    यदि(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(दत्त, "Local port: %hu", पत्तनम्);
      conn->द्वयङ्काः.bound = TRUE;
      परिणाम = CURLE_OK;
      गच्छ बहिर्गमनम्;
    }

    यदि(--portnum > 0) {
      पत्तनम्++; /* try next port */
      यदि(पत्तनम् == 0)
        भङ्ग;
      infof(दत्त, "Bind to local port %d failed, trying next", पत्तनम् - 1);
      /* We reuse/clobber the port variable here below */
      यदि(sock->sa_family == AF_INET)
        si4->sin_port = htons(पत्तनम्);
#यदि_परिभाषितम् USE_IPV6
      अन्यथा
        si6->sin6_port = htons(पत्तनम्);
#यदि_अन्तः
    }
    अन्यथा
      भङ्ग;
  }
  {
    वर्ण बफर[STRERROR_LEN];
    दत्त->state.os_errno = sockerr = SOCKERRNO;
    failf(दत्त, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, बफर, परिमाण(बफर)));
  }

  परिणाम = CURLE_INTERFACE_FAILED;

बहिर्गमनम्:
  curlx_free(dev_अन्तर्);
  curlx_free(iface_अन्तर्);
  curlx_free(host_अन्तर्);
  प्रत्यागम परिणाम;
}
#यदि_अन्तः

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
स्थिर bool verifyconnect(curl_socket_t sockfd, पूर्णाङ्क *psockerr)
{
  bool rc = TRUE;
#यदि_परिभाषितम् SO_ERROR
  पूर्णाङ्क sockerr = 0;
  curl_socklen_t err_आकार = परिमाण(sockerr);

#यदि_परिभाषितम् _WIN32
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
#यदि_अन्तः

  यदि(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (शून्य *)&sockerr, &err_आकार))
    sockerr = SOCKERRNO;
#यदि defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  यदि(EBADIOCTL == sockerr) {
    समुच्चयः_SOCKERRNO(0);
    sockerr = 0;
  }
#यदि_अन्तः
  यदि((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  अन्यथा
    /* This was not a successful connect */
    rc = FALSE;
  यदि(psockerr)
    *psockerr = sockerr;
#अन्यथा
  (शून्य)sockfd;
  यदि(psockerr)
    *psockerr = SOCKERRNO;
#यदि_अन्तः
  प्रत्यागम rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
स्थिर CURLcode socket_connect_परिणाम(संरचना Curl_easy *दत्त,
                                      ध्रुव वर्ण *ipaddress, पूर्णाङ्क sockerr)
{
  यदि(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    प्रत्यागम CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(वर्ण बफर[STRERROR_LEN]);
    infof(दत्त, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, बफर, परिमाण(बफर)));
    NOVERBOSE((शून्य)ipaddress);
  }
  दत्त->state.os_errno = sockerr;
  /* connect failed */
  प्रत्यागम CURLE_COULDNT_CONNECT;
}

संरचना cf_socket_ctx {
  संरचना Curl_peer *peer;
  संरचना Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  संरचना ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  संरचना curltime started_at;        /* when socket was created */
  संरचना curltime connected_at;      /* when socket connected/got first byte */
  संरचना curltime first_byte_at;     /* when first byte was recvd */
#यदि_परिभाषितम् USE_WINSOCK
  संरचना curltime चरमः_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_आकार;                     /* the last set SO_SNDBUF size */
#यदि_अन्तः
  पूर्णाङ्क sockerr;                       /* socket error of last failure or 0 */
#यदि_परिभाषितम् DEBUGBUILD
  पूर्णाङ्क connect_wait_ms;               /* ms to wait after 1st connect() */
  पूर्णाङ्क wblock_percent;                /* percent of writes doing EAGAIN */
  पूर्णाङ्क wpartial_percent;              /* percent of bytes written in send */
  पूर्णाङ्क rblock_percent;                /* percent of reads doing EAGAIN */
  size_t recv_max;                   /* max enforced read size */
#यदि_अन्तः
  uint8_t transport;
  BIT(got_first_byte);               /* if first byte was received */
  BIT(listening);                    /* socket is listening */
  BIT(accepted);                     /* socket was accepted, not connected */
  BIT(sock_connected);               /* socket is "connected", e.g. in UDP */
  BIT(सक्रिय);
  BIT(stats_reported);
};

स्थिर CURLcode cf_socket_ctx_init(संरचना cf_socket_ctx *ctx,
                                   संरचना Curl_peer *peer,
                                   संरचना Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  स्मृतिं_पूरय(ctx, 0, परिमाण(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#यदि_परिभाषितम् DEBUGBUILD
  {
    ध्रुव वर्ण *p = getenv("CURL_DBG_SOCK_WBLOCK");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, 100))
        ctx->wblock_percent = (पूर्णाङ्क)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, 100))
        ctx->wpartial_percent = (पूर्णाङ्क)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, 100))
        ctx->rblock_percent = (पूर्णाङ्क)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, अधिकतम_पूर्णाङ्कः))
        ctx->connect_wait_ms = (पूर्णाङ्क)l;
    }
  }
#यदि_अन्तः

  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_socket_ctx_free(संरचना cf_socket_ctx *ctx)
{
  यदि(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

स्थिर CURLcode cf_socket_shutdown(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   bool *समाप्तम्)
{
  यदि(cf->connected) {
    संरचना cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(दत्त, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    यदि(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      अचिह्नित वर्ण buf[1024];
      (शून्य)sread(ctx->sock, buf, परिमाण(buf));
    }
  }
  *समाप्तम् = TRUE;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_socket_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(दत्त, cf, "destroy");
  यदि(ctx) {
    यदि(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(दत्त, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      यदि(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(दत्त, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

स्थिर शून्य समुच्चयः_local_ip(संरचना Curl_cfilter *cf,
                         संरचना Curl_easy *दत्त)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_पत्तनम् = 0;

#यदि_परिभाषितम् HAVE_GETSOCKNAME
  यदि((ctx->sock != CURL_SOCKET_BAD) &&
     !(दत्त->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    संरचना Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = परिमाण(संरचना Curl_sockaddr_storage);

    स्मृतिं_पूरय(&ssloc, 0, परिमाण(ssloc));
    यदि(getsockname(ctx->sock, (संरचना sockaddr *)&ssloc, &slen)) {
      VERBOSE(वर्ण बफर[STRERROR_LEN]);
      VERBOSE(पूर्णाङ्क sockerr = SOCKERRNO);
      infof(दत्त, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, बफर, परिमाण(बफर)));
    }
    अन्यथा {
      CURLcode परिणाम = sockaddr2string((संरचना sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_पत्तनम्);
      यदि(परिणाम)
        infof(दत्त, "ssloc inet_ntop() failed with %d", (पूर्णाङ्क)परिणाम);
    }
  }
#अन्यथा
  (शून्य)दत्त;
#यदि_अन्तः
}

स्थिर CURLcode समुच्चयः_remote_ip(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  CURLcode परिणाम;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  परिणाम = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_पत्तनम्);
  यदि(परिणाम) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(दत्त, "curl_sa_addr inet_ntop() failed with %d", (पूर्णाङ्क)परिणाम);
    प्रत्यागम CURLE_FAILED_INIT;
  }
  प्रत्यागम CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
स्थिर पूर्णाङ्क cf_socktype(पूर्णाङ्क x)
{
#यदि_परिभाषितम् SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#यदि_अन्तः
#यदि_परिभाषितम् CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#यदि_अन्तः
  प्रत्यागम x;
}

#यदि_परिभाषितम् _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#यदि_अपरिभाषितम् SIO_TCP_INITIAL_RTO
#परिभाषय SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#परिभाषय TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकारनिर्देश संरचना _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#यदि_अन्तः /* SIO_TCP_INITIAL_RTO */

#यदि_अपरिभाषितम् TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#परिभाषय TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#यदि_अन्तः

स्थिर bool targets_localhost(संरचना cf_socket_ctx *ctx)
{
  प्रत्यागम (((ctx->addr.family == AF_INET) &&
           !वाक्यं_तुलय(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !वाक्यं_तुलय(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
स्थिर शून्य tcplocalhost(संरचना Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  यदि(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD अष्टकानि = 0;
    स्मृतिं_पूरय(&rto, 0, परिमाण(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    यदि(curlx_verify_windows_संस्करणम्(10, 0, 16299, PLATFORM_WINNT,
                                    संस्करणम्_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    अन्यथा
      rto.MaxSynRetransmissions = 1;
    (शून्य)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, परिमाण(rto),
                   रिक्त, 0, &अष्टकानि, रिक्त, रिक्त);
  }
}
#अन्यथा
#परिभाषय tcplocalhost(x, y)
#यदि_अन्तः /* _WIN32 */

स्थिर CURLcode cf_socket_open(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  पूर्णाङ्क error = 0;
  bool isconnected = FALSE;
  CURLcode परिणाम = CURLE_COULDNT_CONNECT;
  bool is_tcp;

  DEBUGASSERT(ctx->sock == CURL_SOCKET_BAD);
  ctx->started_at = *Curl_pgrs_now(दत्त);
#यदि_परिभाषितम् CURL_USE_SOCK_NONBLOCK
  /* Do not tuck SOCK_NONBLOCK into socktype when opensocket callback is set
   * because we would not know how socktype is about to be used in the
   * callback, SOCK_NONBLOCK might get factored out before calling socket().
   */
  यदि(!दत्त->समुच्चयः.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#यदि_अन्तः
  परिणाम = socket_open(दत्त, &ctx->addr, &ctx->sock);
#यदि_परिभाषितम् CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  यदि(!दत्त->समुच्चयः.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#यदि_अन्तः
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = समुच्चयः_remote_ip(cf, दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

#यदि_परिभाषितम् USE_IPV6
  यदि(ctx->addr.family == AF_INET6) {
#यदि_परिभाषितम् USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    पूर्णाङ्क on = 0;
    (शून्य)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (शून्य *)&on, परिमाण(on));
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      संरचना sockaddr_in6 *sa6 = (शून्य *)&ctx->addr.curl_sa_addr;
      यदि(sa6->sin6_scope_id)
        infof(दत्त, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_पत्तनम्,
              (अचिह्नित दीर्घ)sa6->sin6_scope_id);
      अन्यथा
#यदि_अन्तः
        infof(दत्त, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_पत्तनम्);
#यदि_परिभाषितम् HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#यदि_अन्तः
  }
  अन्यथा
#यदि_अन्तः
    infof(दत्त, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_पत्तनम्);

#यदि_परिभाषितम् USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#अन्यथा
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#यदि_अन्तः
  यदि(is_tcp) {
    यदि(दत्त->समुच्चयः.tcp_nodelay)
      tcpnodelay(cf, दत्त, ctx->sock);

    यदि(दत्त->समुच्चयः.tcp_keepalive)
      tcpkeepalive(cf, दत्त, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  यदि(दत्त->समुच्चयः.fsockopt) {
    /* activate callback for setting socket options */
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, दत्त, easy_fsockopt);
    error = दत्त->समुच्चयः.fsockopt(दत्त->समुच्चयः.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_अन्तः(&guard);

    यदि(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    अन्यथा यदि(error) {
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
      गच्छ बहिर्गमनम्;
    }
  }

#यदि_अपरिभाषितम् CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  यदि(ctx->addr.family == AF_INET
#यदि_परिभाषितम् USE_IPV6
     || ctx->addr.family == AF_INET6
#यदि_अन्तः
    ) {
    परिणाम = bindlocal(दत्त, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    यदि(परिणाम) {
      यदि(परिणाम == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        परिणाम = CURLE_COULDNT_CONNECT;
      }
      गच्छ बहिर्गमनम्;
    }
  }
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  यदि(error < 0) {
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    गच्छ बहिर्गमनम्;
  }
#अन्यथा
  यदि(दत्त->समुच्चयः.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    यदि(error < 0) {
      परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      गच्छ बहिर्गमनम्;
    }
  }
#यदि_अन्तः
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
बहिर्गमनम्:
  यदि(परिणाम) {
    यदि(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(दत्त, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  अन्यथा यदि(isconnected) {
    समुच्चयः_local_ip(cf, दत्त);
    ctx->connected_at = *Curl_pgrs_now(दत्त);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(दत्त, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (पूर्णाङ्क)परिणाम, ctx->sock);
  प्रत्यागम परिणाम;
}

स्थिर पूर्णाङ्क do_connect(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                      bool is_tcp_fastopen)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
#यदि_परिभाषितम् TCP_FASTOPEN_CONNECT
  पूर्णाङ्क optval = 1;
#यदि_अन्तः
  पूर्णाङ्क rc = -1;

  (शून्य)दत्त;
  यदि(is_tcp_fastopen) {
#यदि_परिभाषितम् CONNECT_दत्त_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_उपलब्धम्
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    यदि(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = रिक्त;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_दत्त_IDEMPOTENT,
                    रिक्त, 0, रिक्त, रिक्त);
    }
    अन्यथा {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  अन्यथा
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#नोचेद्यदि defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    यदि(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (शून्य *)&optval, परिमाण(optval)) < 0)
      CURL_TRC_CF(दत्त, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#नोचेद्यदि defined(MSG_FASTOPEN) /* old Linux */
    यदि(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    अन्यथा
      rc = 0; /* Do nothing */
#यदि_अन्तः
  }
  अन्यथा {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  प्रत्यागम rc;
}

स्थिर CURLcode cf_tcp_connect(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               bool *समाप्तम्)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_COULDNT_CONNECT;
  पूर्णाङ्क rc = 0;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *समाप्तम् = FALSE; /* a negative world view is best */
  यदि(ctx->sock == CURL_SOCKET_BAD) {
    पूर्णाङ्क sockerr;

    परिणाम = cf_socket_open(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    यदि(cf->connected) {
      *समाप्तम् = TRUE;
      प्रत्यागम CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, दत्त, (bool)cf->conn->द्वयङ्काः.tcp_fastopen);
    sockerr = SOCKERRNO;
    समुच्चयः_local_ip(cf, दत्त);
    CURL_TRC_CF(दत्त, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_पत्तनम्);
#यदि_परिभाषितम् DEBUGBUILD
    यदि(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(दत्त, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#यदि_अन्तः
    यदि(rc == -1) {
      ctx->sockerr = sockerr;
      परिणाम = socket_connect_परिणाम(दत्त, ctx->ip.remote_ip, sockerr);
      गच्छ बहिर्गमनम्;
    }
  }

#यदि_परिभाषितम् mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (शून्य)verifyconnect(ctx->sock, रिक्त);
#यदि_अन्तः
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  यदि(rc == 0) { /* no connection yet */
    CURL_TRC_CF(दत्त, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    प्रत्यागम CURLE_OK;
  }
  अन्यथा यदि(rc == CURL_CSELECT_बहिर्गमनम् ||
#यदि_परिभाषितम् __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#यदि_अन्तः
          cf->conn->द्वयङ्काः.tcp_fastopen) {
    यदि(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(दत्त);
      समुच्चयः_local_ip(cf, दत्त);
      *समाप्तम् = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(दत्त, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      प्रत्यागम CURLE_OK;
    }
    परिणाम = CURLE_COULDNT_CONNECT;
  }
  अन्यथा यदि(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(दत्त, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (शून्य)verifyconnect(ctx->sock, &ctx->sockerr);
    परिणाम = CURLE_COULDNT_CONNECT;
  }

बहिर्गमनम्:
  यदि(परिणाम) {
    VERBOSE(वर्ण बफर[STRERROR_LEN]);
    समुच्चयः_local_ip(cf, दत्त);
    यदि(ctx->sockerr) {
      दत्त->state.os_errno = ctx->sockerr;
      समुच्चयः_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, बफर, परिमाण(बफर)));
    }
    अन्यथा {
      VERBOSE(curlx_strcopy(बफर, परिमाण(बफर), STRCONST("peer closed")));
    }
    यदि(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(दत्त, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(दत्त, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_पत्तनम्,
          ctx->ip.local_ip, ctx->ip.local_पत्तनम्,
          curlx_strerror(ctx->sockerr, बफर, परिमाण(बफर)));
    *समाप्तम् = FALSE;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_socket_adjust_pollset(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त,
                                         संरचना easy_pollset *ps)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    यदि(ctx->listening) {
      परिणाम = Curl_pollset_समुच्चयः_अन्तर्_only(दत्त, ps, ctx->sock);
      CURL_TRC_CF(दत्त, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    अन्यथा यदि(!cf->connected) {
      परिणाम = Curl_pollset_समुच्चयः_बहिर्गमनम्_only(दत्त, ps, ctx->sock);
      CURL_TRC_CF(दत्त, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    अन्यथा यदि(!ctx->सक्रिय) {
      परिणाम = Curl_pollset_सङ्कलय_अन्तर्(दत्त, ps, ctx->sock);
      CURL_TRC_CF(दत्त, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#यदि_अपरिभाषितम् SIO_IDEAL_SEND_BACKLOG_QUERY
#परिभाषय SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#यदि_अन्तः

स्थिर शून्य win_update_sndbuf_आकार(संरचना Curl_easy *दत्त,
                                   संरचना cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  यदि(curlx_ptimediff_ms(Curl_pgrs_now(दत्त),
                        &ctx->चरमः_sndbuf_query_at) > 1000) {
    यदि(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, परिमाण(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_आकार &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (ध्रुव वर्ण *)&ideal, परिमाण(ideal))) {
      ctx->sndbuf_आकार = ideal;
    }
    ctx->चरमः_sndbuf_query_at = *Curl_pgrs_now(दत्त);
  }
}

#यदि_अन्तः /* USE_WINSOCK */

स्थिर CURLcode cf_socket_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                               ध्रुव uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode परिणाम = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (शून्य)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#यदि_परिभाषितम् DEBUGBUILD
  /* simulate network blocking/partial writes */
  यदि(ctx->wblock_percent > 0) {
    अचिह्नित वर्ण c = 0;
    Curl_rand_अष्टकानि(दत्त, FALSE, &c, 1);
    यदि(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(दत्त, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      प्रत्यागम CURLE_AGAIN;
    }
  }
  यदि(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    यदि(!len)
      len = 1;
    CURL_TRC_CF(दत्त, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#यदि_अन्तः

#यदि defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  यदि(cf->conn->द्वयङ्काः.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->द्वयङ्काः.tcp_fastopen = FALSE;
  }
  अन्यथा
#यदि_अन्तः
    rv = swrite(ctx->sock, buf, len);

  यदि(!curlx_sztouz(rv, pnwritten)) {
    पूर्णाङ्क sockerr = SOCKERRNO;
    यदि(SOCK_EAGAIN(sockerr)
#यदि_अपरिभाषितम् USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#यदि_अन्तः
      ) {
      परिणाम = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    अन्यथा {
      वर्ण बफर[STRERROR_LEN];
      failf(दत्त, "Send failure: %s",
            curlx_strerror(sockerr, बफर, परिमाण(बफर)));
      दत्त->state.os_errno = sockerr;
      परिणाम = CURLE_SEND_ERROR;
    }
  }

#यदि_परिभाषितम् USE_WINSOCK
  यदि(!परिणाम)
    win_update_sndbuf_आकार(दत्त, ctx);
#यदि_अन्तः

  CURL_TRC_CF(दत्त, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (पूर्णाङ्क)परिणाम, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_socket_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                               वर्ण *buf, size_t len, size_t *pnread)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#यदि_परिभाषितम् DEBUGBUILD
  /* simulate network blocking/partial reads */
  यदि(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    अचिह्नित वर्ण c = 0;
    Curl_rand(दत्त, &c, 1);
    यदि(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(दत्त, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      प्रत्यागम CURLE_AGAIN;
    }
  }
  यदि(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(दत्त, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#यदि_अन्तः

  rv = sread(ctx->sock, buf, len);

  यदि(!curlx_sztouz(rv, pnread)) {
    पूर्णाङ्क sockerr = SOCKERRNO;
    यदि(SOCK_EAGAIN(sockerr)
#यदि_अपरिभाषितम् USE_WINSOCK
       || (sockerr == SOCKEINTR)
#यदि_अन्तः
      ) {
      परिणाम = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    अन्यथा {
      वर्ण बफर[STRERROR_LEN];
      failf(दत्त, "Recv failure: %s",
            curlx_strerror(sockerr, बफर, परिमाण(बफर)));
      दत्त->state.os_errno = sockerr;
      परिणाम = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(दत्त, cf, "recv(len=%zu) -> %d, %zu", len, (पूर्णाङ्क)परिणाम, *pnread);
  यदि(!परिणाम && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(दत्त);
    ctx->got_first_byte = TRUE;
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_socket_update_दत्त(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त)
{
  /* Update the IP info held in the transfer, if we have that. */
  यदि(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    संरचना cf_socket_ctx *ctx = cf->ctx;
    दत्त->info.primary = ctx->ip;
  }
}

स्थिर शून्य cf_socket_सक्रिय(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  समुच्चयः_local_ip(cf, दत्त);
#यदि_परिभाषितम् USE_IPV6
  यदि(cf->sockindex == FIRSTSOCKET)
    cf->conn->द्वयङ्काः.ipv6 = (ctx->addr.family == AF_INET6);
#यदि_अन्तः
  ctx->सक्रिय = TRUE;
}

स्थिर CURLcode cf_socket_cntrl(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;

  (शून्य)arg1;
  (शून्य)arg2;
  विकल्प(event) {
  पक्ष CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_सक्रिय(cf, दत्त);
    cf_socket_update_दत्त(cf, दत्त);
    भङ्ग;
  पक्ष CF_CTRL_दत्त_सज्जता:
    cf_socket_update_दत्त(cf, दत्त);
    भङ्ग;
  पक्ष CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    भङ्ग;
  पक्ष CF_CTRL_REPORT_STATS:
    यदि(cf->connected && !ctx->stats_reported) {
      संरचना curltime *ts = रिक्त;
      विकल्प(ctx->transport) {
      पक्ष TRNSPRT_UDP:
      पक्ष TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        यदि(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        भङ्ग;
      स्वभाव:
        ts = &ctx->connected_at;
        भङ्ग;
      }
      यदि(ts) {
        Curl_pgrsTimeWas(दत्त, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर bool cf_socket_conn_is_alive(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    bool *निवेशः_pending)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  संरचना pollfd pfd[1];
  पूर्णाङ्क r;

  *निवेशः_pending = FALSE;

  यदि(!ctx || ctx->sock == CURL_SOCKET_BAD)
    प्रत्यागम FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  यदि(r < 0) {
    CURL_TRC_CF(दत्त, cf, "is_alive: poll error, assume dead");
    प्रत्यागम FALSE;
  }
  अन्यथा यदि(r == 0) {
    CURL_TRC_CF(दत्त, cf, "is_alive: poll timeout, assume alive");
    प्रत्यागम TRUE;
  }
  अन्यथा यदि(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(दत्त, cf, "is_alive: err/hup/etc events, assume dead");
    प्रत्यागम FALSE;
  }

  CURL_TRC_CF(दत्त, cf, "is_alive: valid events, looks alive");
  *निवेशः_pending = TRUE;
  प्रत्यागम TRUE;
}

स्थिर CURLcode cf_socket_query(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;

  विकल्प(query) {
  पक्ष CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((ध्रुव संरचना Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : रिक्त;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_CONNECT_प्रत्युत्तरम्_MS:
    यदि(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < अधिकतम_पूर्णाङ्कः) ? (पूर्णाङ्क)ms : अधिकतम_पूर्णाङ्कः;
    }
    अन्यथा
      *pres1 = -1;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_IP_INFO:
#यदि_परिभाषितम् USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#अन्यथा
    *pres1 = FALSE;
#यदि_अन्तः
    *(संरचना ip_quadruple *)pres2 = ctx->ip;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_REALLY_CONNECTED:
    यदि(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    अन्यथा
      *pres1 = ctx->got_first_byte;
    प्रत्यागम CURLE_OK;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
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
  Curl_cf_def_दत्त_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_tcp_create(संरचना Curl_cfilter **pcf,
                            संरचना Curl_easy *दत्त,
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
  CURLcode परिणाम;

  (शून्य)दत्त;
  (शून्य)origin;
  (शून्य)conn;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  यदि(!addr) {
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

बहिर्गमनम्:
  *pcf = (!परिणाम) ? cf : रिक्त;
  यदि(परिणाम) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् __linux__
स्थिर शून्य linux_quic_mtu(संरचना cf_socket_ctx *ctx)
{
  पूर्णाङ्क val;
  विकल्प(ctx->addr.family) {
#यदि_परिभाषितम् IP_MTU_DISCOVER
  पक्ष AF_INET:
    val = IP_PMTUDISC_DO;
    (शून्य)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     परिमाण(val));
    भङ्ग;
#यदि_अन्तः
#यदि_परिभाषितम् IPV6_MTU_DISCOVER
  पक्ष AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (शून्य)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     परिमाण(val));
    भङ्ग;
#यदि_अन्तः
  }
}
#अन्यथा
#परिभाषय linux_quic_mtu(x)
#यदि_अन्तः

#यदि defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
स्थिर शून्य linux_quic_gro(संरचना cf_socket_ctx *ctx)
{
  पूर्णाङ्क one = 1;
  (शून्य)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)परिमाण(one));
}
#अन्यथा
#परिभाषय linux_quic_gro(x)
#यदि_अन्तः

#यदि (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
स्थिर शून्य linux_quic_ecn(संरचना cf_socket_ctx *ctx)
{
  अचिह्नित पूर्णाङ्क tos = 1;
  विकल्प(ctx->addr.family) {
  पक्ष AF_INET:
    (शून्य)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, परिमाण(tos));
    भङ्ग;
#यदि_परिभाषितम् IPV6_RECVTCLASS
  पक्ष AF_INET6:
    (शून्य)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, परिमाण(tos));
    भङ्ग;
#यदि_अन्तः
  }
}
#अन्यथा
#परिभाषय linux_quic_ecn(x)
#यदि_अन्तः

#यदि (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
स्थिर शून्य linux_ip_dontfrag(संरचना cf_socket_ctx *ctx)
{
  पूर्णाङ्क val = 1;

  विकल्प(ctx->addr.family) {
  पक्ष AF_INET:
    (शून्य)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, परिमाण(val));
    भङ्ग;
#यदि_परिभाषितम् IPV6_DONTFRAG
  पक्ष AF_INET6:
    (शून्य)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, परिमाण(val));
    भङ्ग;
#यदि_अन्तः
  }
}
#अन्यथा
#परिभाषय linux_ip_dontfrag(x)
#यदि_अन्तः

स्थिर CURLcode cf_udp_सज्जता_quic(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  पूर्णाङ्क rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  यदि(rc == -1) {
    प्रत्यागम socket_connect_परिणाम(दत्त, ctx->ip.remote_ip, SOCKERRNO);
  }
  ctx->sock_connected = TRUE;
  समुच्चयः_local_ip(cf, दत्त);
  CURL_TRC_CF(दत्त, cf, "%s socket %" FMT_SOCKET_T
              " connected: [%s:%d] -> [%s:%d]",
              (ctx->transport == TRNSPRT_QUIC) ? "QUIC" : "UDP",
              ctx->sock, ctx->ip.local_ip, ctx->ip.local_पत्तनम्,
              ctx->ip.remote_ip, ctx->ip.remote_पत्तनम्);

  /* Currently, cf->ctx->sock is always non-blocking because the only
   * caller to cf_udp_setup_quic() is cf_udp_connect() that passes the
   * non-blocking socket created by cf_socket_open() to it. Thus, we
   * do not need to call curlx_nonblock() in cf_udp_setup_quic() anymore.
   */
  linux_quic_ecn(ctx);
  linux_quic_mtu(ctx);
  linux_ip_dontfrag(ctx);
  linux_quic_gro(ctx);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_udp_connect(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               bool *समाप्तम्)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_COULDNT_CONNECT;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *समाप्तम् = FALSE;
  यदि(ctx->sock == CURL_SOCKET_BAD) {
    परिणाम = cf_socket_open(cf, दत्त);
    यदि(परिणाम) {
      CURL_TRC_CF(दत्त, cf, "cf_udp_connect(), open failed -> %d",
                  (पूर्णाङ्क)परिणाम);
      गच्छ बहिर्गमनम्;
    }

    यदि(ctx->transport == TRNSPRT_QUIC) {
      परिणाम = cf_udp_सज्जता_quic(cf, दत्त);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      CURL_TRC_CF(दत्त, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_पत्तनम्);
    }
    *समाप्तम् = TRUE;
    cf->connected = TRUE;
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

संरचना Curl_cftype Curl_cft_udp = {
  "UDP",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_udp_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_दत्त_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_udp_create(संरचना Curl_cfilter **pcf,
                            संरचना Curl_easy *दत्त,
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
  CURLcode परिणाम;

  (शून्य)दत्त;
  (शून्य)origin;
  (शून्य)conn;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

बहिर्गमनम्:
  *pcf = (!परिणाम) ? cf : रिक्त;
  यदि(परिणाम) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  प्रत्यागम परिणाम;
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
  Curl_cf_def_दत्त_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_cf_unix_create(संरचना Curl_cfilter **pcf,
                            संरचना Curl_easy *दत्त,
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
  CURLcode परिणाम;

  (शून्य)दत्त;
  (शून्य)origin;
  (शून्य)conn;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

बहिर्गमनम्:
  *pcf = (!परिणाम) ? cf : रिक्त;
  यदि(परिणाम) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  प्रत्यागम परिणाम;
}

स्थिर timediff_t cf_tcp_accept_timeleft(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  यदि(दत्त->समुच्चयः.accepttimeout > 0)
    timeout_ms = दत्त->समुच्चयः.accepttimeout;
#यदि_अन्तः

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(दत्त);
  यदि(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  अन्यथा {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &ctx->started_at);
    यदि(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  प्रत्यागम timeout_ms;
}

स्थिर शून्य cf_tcp_समुच्चयः_accepted_remote_ip(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *दत्त)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
#यदि_परिभाषितम् HAVE_GETPEERNAME
  CURLcode परिणाम;
  संरचना Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_पत्तनम् = 0;
  plen = परिमाण(ssrem);
  स्मृतिं_पूरय(&ssrem, 0, plen);
  यदि(getpeername(ctx->sock, (संरचना sockaddr *)&ssrem, &plen)) {
    वर्ण बफर[STRERROR_LEN];
    पूर्णाङ्क sockerr = SOCKERRNO;
    failf(दत्त, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, बफर, परिमाण(बफर)));
    प्रत्यागम;
  }
  परिणाम = sockaddr2string((संरचना sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_पत्तनम्);
  यदि(परिणाम) {
    failf(दत्त, "ssrem inet_ntop() failed with %d", (पूर्णाङ्क)परिणाम);
    प्रत्यागम;
  }
#अन्यथा
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_पत्तनम् = 0;
  (शून्य)दत्त;
#यदि_अन्तः
}

स्थिर CURLcode cf_tcp_accept_connect(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      bool *समाप्तम्)
{
  संरचना cf_socket_ctx *ctx = cf->ctx;
  वर्ण errbuf[STRERROR_LEN];
#यदि_परिभाषितम् USE_IPV6
  संरचना Curl_sockaddr_storage सङ्कलय;
#अन्यथा
  संरचना sockaddr_in सङ्कलय;
#यदि_अन्तः
  curl_socklen_t आकार = (curl_socklen_t)परिमाण(सङ्कलय);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  पूर्णाङ्क socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (शून्य)दत्त;
  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *समाप्तम् = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, दत्त);
  यदि(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(दत्त, "Accept timeout occurred while waiting server connect");
    प्रत्यागम CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(दत्त, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_पत्तनम्);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(दत्त, cf, "socket_check -> %x", (अचिह्नित पूर्णाङ्क)socketstate);
  विकल्प(socketstate) {
  पक्ष -1: /* error */
    /* let's die here */
    failf(दत्त, "Error while waiting for server connect");
    प्रत्यागम CURLE_FTP_ACCEPT_FAILED;
  स्वभाव:
    यदि(socketstate & CURL_CSELECT_अन्तर्) {
      infof(दत्त, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    भङ्ग;
  }

  यदि(!incoming) {
    CURL_TRC_CF(दत्त, cf, "nothing heard from the server yet");
    प्रत्यागम CURLE_OK;
  }

  आकार = परिमाण(सङ्कलय);
#यदि_परिभाषितम् HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (संरचना sockaddr *)&सङ्कलय, &आकार,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#अन्यथा
  s_accepted = CURL_ACCEPT(ctx->sock, (संरचना sockaddr *)&सङ्कलय, &आकार);
#यदि_अन्तः

  यदि(s_accepted == CURL_SOCKET_BAD) {
    failf(दत्त, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, परिमाण(errbuf)));
    प्रत्यागम CURLE_FTP_ACCEPT_FAILED;
  }
#यदि_अपरिभाषितम् HAVE_ACCEPT4
#यदि_परिभाषितम् HAVE_FCNTL
  यदि(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(दत्त, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, परिमाण(errbuf)));
    Curl_socket_close(दत्त, cf->conn, s_accepted);
    प्रत्यागम CURLE_FTP_ACCEPT_FAILED;
  }
#यदि_अन्तः /* HAVE_FCNTL */
  यदि(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(दत्त, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, परिमाण(errbuf)));
    Curl_socket_close(दत्त, cf->conn, s_accepted);
    प्रत्यागम CURLE_FTP_ACCEPT_FAILED;
  }
#यदि_अन्तः /* !HAVE_ACCEPT4 */
  infof(दत्त, "Connection accepted from server");

  /* Replace any filter on SECONDARY with one listening on this socket */
  ctx->listening = FALSE;
  ctx->accepted = TRUE;
  socket_close(दत्त, cf->conn, TRUE, ctx->sock);
  ctx->sock = s_accepted;

  cf->conn->sock[cf->sockindex] = ctx->sock;
  cf_tcp_समुच्चयः_accepted_remote_ip(cf, दत्त);
  समुच्चयः_local_ip(cf, दत्त);
  ctx->सक्रिय = TRUE;
  ctx->connected_at = *Curl_pgrs_now(दत्त);
  cf->connected = TRUE;
  CURL_TRC_CF(दत्त, cf, "accepted_set(sock=%" FMT_SOCKET_T
              ", remote=%s port=%d)",
              ctx->sock, ctx->ip.remote_ip, ctx->ip.remote_पत्तनम्);

  यदि(दत्त->समुच्चयः.fsockopt) {
    संरचना Curl_mapi_guard guard;
    पूर्णाङ्क error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, दत्त, easy_fsockopt);
    error = दत्त->समुच्चयः.fsockopt(दत्त->समुच्चयः.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_अन्तः(&guard);

    यदि(error)
      प्रत्यागम CURLE_ABORTED_BY_CALLBACK;
  }
  *समाप्तम् = TRUE;
  प्रत्यागम CURLE_OK;
}

संरचना Curl_cftype Curl_cft_tcp_accept = {
  "TCP-ACCEPT",
  CF_TYPE_IP_CONNECT,
  CURL_LOG_LVL_NONE,
  cf_socket_destroy,
  cf_tcp_accept_connect,
  cf_socket_shutdown,
  cf_socket_adjust_pollset,
  Curl_cf_def_दत्त_pending,
  cf_socket_send,
  cf_socket_recv,
  cf_socket_cntrl,
  cf_socket_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_socket_query,
};

CURLcode Curl_conn_tcp_listen_समुच्चयः(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode परिणाम;
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_socket_ctx *ctx = रिक्त;

  /* replace any existing */
  Curl_conn_cf_discard_all(दत्त, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  परिणाम = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  Curl_conn_cf_सङ्कलय(दत्त, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(दत्त);
  conn->sock[sockindex] = ctx->sock;
  समुच्चयः_local_ip(cf, दत्त);
  CURL_TRC_CF(दत्त, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_पत्तनम्);

बहिर्गमनम्:
  यदि(परिणाम) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  प्रत्यागम परिणाम;
}

bool Curl_conn_is_tcp_listen(संरचना Curl_easy *दत्त,
                             int8_t sockindex)
{
  संरचना Curl_cfilter *cf = दत्त->conn->cfilter[sockindex];
  यावत्(cf) {
    यदि(cf->cft == &Curl_cft_tcp_accept)
      प्रत्यागम TRUE;
    cf = cf->अग्र;
  }
  प्रत्यागम FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
स्थिर bool cf_is_socket(संरचना Curl_cfilter *cf)
{
  प्रत्यागम cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             curl_socket_t *psock,
                             ध्रुव संरचना Curl_sockaddr_ex **paddr,
                             संरचना ip_quadruple *pip)
{
  (शून्य)दत्त;
  यदि(cf_is_socket(cf) && cf->ctx) {
    संरचना cf_socket_ctx *ctx = cf->ctx;

    यदि(psock)
      *psock = ctx->sock;
    यदि(paddr)
      *paddr = &ctx->addr;
    यदि(pip)
      *pip = ctx->ip;
    प्रत्यागम CURLE_OK;
  }
  प्रत्यागम CURLE_FAILED_INIT;
}
