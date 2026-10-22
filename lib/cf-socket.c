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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_SYS_UN_H
#અન્તર્ભાવ <sys/un.h> /* for sockaddr_un */
#endif
#ifdef HAVE_LINUX_TCP_H
#અન્તર્ભાવ <linux/tcp.h>
#અથવાજો defined(HAVE_NETINET_TCP_H)
#અન્તર્ભાવ <netinet/tcp.h>
#endif
#ifdef HAVE_NETINET_UDP_H
#અન્તર્ભાવ <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#અન્તર્ભાવ <netinet/ip.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#અન્તર્ભાવ <sys/ioctl.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif

#ifdef HAVE_IFADDRS_H
#અન્તર્ભાવ <ifaddrs.h>
#endif
#ifdef HAVE_NET_IF_H
#અન્તર્ભાવ <net/if.h>
#endif
#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "if2ip.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-socket.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "curlx/inet_ntop.h"
#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "conncache.h"
#અન્તર્ભાવ "multihandle.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "sockaddr.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "curlx/nonblock.h"
#અન્તર્ભાવ "curlx/strcopy.h"
#અન્તર્ભાવ "curlx/version_win32.h"
#અન્તર્ભાવ "curlx/strerr.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "curlx/wait.h"
#ifdef _WIN32
#અન્તર્ભાવ <mstcpip.h> /* for TCP_INITIAL_RTO_PARAMETERS */
#endif

/* retrieves ip address and port from a sockaddr structure. note it calls
 * curlx_inet_ntop() and returns CURLcode.
 * @unittest 1607
 */
UNITTEST CURLcode sockaddr2string(સંરચના sockaddr *sa, curl_socklen_t salen,
                                  અક્ષર *addr, uint16_t *port);
UNITTEST CURLcode sockaddr2string(સંરચના sockaddr *sa, curl_socklen_t salen,
                                  અક્ષર *addr, uint16_t *port)
{
  CURLcode result;
  સંરચના sockaddr_in *si = ખાલી;
#ifdef USE_IPV6
  સંરચના sockaddr_in6 *si6 = ખાલી;
#endif
#ifdef USE_UNIX_SOCKETS
  સંરચના sockaddr_un *su = ખાલી;
#નહીંતર
  (શૂન્ય)salen;
#endif

  પસંદગી(sa->sa_family) {
  સ્થિતિ AF_INET:
    si = (સંરચના sockaddr_in *)(શૂન્ય *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si->sin_addr, addr,
                             MAX_IPADR_LEN);
    જો(!result) {
      *port = ntohs(si->sin_port);
      પાછા result;
    }
    તોડો;
#ifdef USE_IPV6
  સ્થિતિ AF_INET6:
    si6 = (સંરચના sockaddr_in6 *)(શૂન્ય *)sa;
    result = curlx_inet_ntop(sa->sa_family, &si6->sin6_addr, addr,
                             MAX_IPADR_LEN);
    જો(!result) {
      *port = ntohs(si6->sin6_port);
      પાછા result;
    }
    તોડો;
#endif
#ifdef USE_UNIX_SOCKETS
  સ્થિતિ AF_UNIX:
    જો(salen > (curl_socklen_t)કદ(CURL_SA_FAMILY_T)) {
      su = (સંરચના sockaddr_un *)sa;
      curl_msnprintf(addr, MAX_IPADR_LEN, "%s", su->sun_path);
    }
    નહીંતર
      addr[0] = 0; /* socket with no name */
    *port = 0;
    પાછા CURLE_OK;
#endif
  મૂળભૂત:
    result = CURLE_UNSUPPORTED_PROTOCOL;
    તોડો;
  }

  addr[0] = '\0';
  *port = 0;
  પાછા result;
}

સ્થિર શૂન્ય tcpnodelay(સંરચના Curl_cfilter *cf,
                       સંરચના Curl_easy *data,
                       curl_socket_t sockfd)
{
#જો defined(TCP_NODELAY) && defined(CURL_TCP_NODELAY_SUPPORTED)
  curl_socklen_t onoff = (curl_socklen_t)1;
  પૂર્ણાંક level = IPPROTO_TCP;
  VERBOSE(અક્ષર buffer[STRERROR_LEN]);

  જો(setsockopt(sockfd, level, TCP_NODELAY, (શૂન્ય *)&onoff, કદ(onoff)) < 0)
    CURL_TRC_CF(data, cf, "Could not set TCP_NODELAY: %s",
                curlx_strerror(SOCKERRNO, buffer, કદ(buffer)));
#નહીંતર
  (શૂન્ય)cf;
  (શૂન્ય)data;
  (શૂન્ય)sockfd;
#endif
}

#જો defined(USE_WINSOCK) || defined(TCP_KEEPIDLE) ||               \
  defined(TCP_KEEPALIVE) || defined(TCP_KEEPALIVE_THRESHOLD) ||    \
  defined(TCP_KEEPINTVL) || defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
#જો defined(USE_WINSOCK) || \
  (defined(__sun) && !defined(TCP_KEEPIDLE)) || \
  (defined(__DragonFly__) && __DragonFly_version < 500702) || \
  (defined(_WIN32) && !defined(TCP_KEEPIDLE))
/* Solaris < 11.4, DragonFlyBSD < 500702 and Windows < 10.0.16299
 * use millisecond units. */
સ્થિર શૂન્ય alive_unit(પૂર્ણાંક *x)
{
  /* make sure this doesn't wrap */
  જો(*x < (INT_MAX / 1000))
    *x *= 1000;
  નહીંતર
    /* this is still almost 25 days */
    *x = INT_MAX;
}
#નહીંતર
#define alive_unit(x)
#endif
#endif

સ્થિર શૂન્ય tcpkeepalive(સંરચના Curl_cfilter *cf,
                         સંરચના Curl_easy *data,
                         curl_socket_t sockfd)
{
  પૂર્ણાંક optval = data->set.tcp_keepalive ? 1 : 0;

  /* only set IDLE and INTVL if setting KEEPALIVE is successful */
  જો(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                (શૂન્ય *)&optval, કદ(optval)) < 0) {
    CURL_TRC_CF(data, cf, "Failed to set SO_KEEPALIVE on fd "
                "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
  }
  નહીંતર {
#ifdef USE_WINSOCK
    /* Windows 10, version 1709 (10.0.16299) and later versions can use
       setsockopt() TCP_KEEP*. Older versions return with failure. */
    જો(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
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
      જો(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                    (અચલ અક્ષર *)&optval, કદ(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      જો(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                    (અચલ અક્ષર *)&optval, કદ(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
      optval = curlx_sltosi(data->set.tcp_keepcnt);
      જો(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                    (અચલ અક્ષર *)&optval, કદ(optval)) < 0) {
        CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPCNT on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
    નહીંતર {
/* Offered by mingw-w64 and MS SDK. Latter only when targeting Win7+. */
#ifndef SIO_KEEPALIVE_VALS
#define SIO_KEEPALIVE_VALS  _WSAIOW(IOC_VENDOR, 4)
      સંરચના tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
      };
#endif
      સંરચના tcp_keepalive vals;
      DWORD dummy;
      vals.onoff = 1;
      optval = curlx_sltosi(data->set.tcp_keepidle);
      alive_unit(&optval);
      vals.keepalivetime = (u_long)optval;
      optval = curlx_sltosi(data->set.tcp_keepintvl);
      alive_unit(&optval);
      vals.keepaliveinterval = (u_long)optval;
      જો(WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, (LPVOID)&vals, કદ(vals),
                  ખાલી, 0, &dummy, ખાલી, ખાલી) != 0) {
        CURL_TRC_CF(data, cf, "Failed to set SIO_KEEPALIVE_VALS on fd "
                    "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
      }
    }
#નહીંતર /* !USE_WINSOCK */
#ifdef TCP_KEEPIDLE
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    જો(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (શૂન્ય *)&optval, કદ(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPIDLE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#અથવાજો defined(TCP_KEEPALIVE)
    /* macOS style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    જો(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE,
                  (શૂન્ય *)&optval, કદ(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#અથવાજો defined(TCP_KEEPALIVE_THRESHOLD)
    /* Solaris <11.4 style */
    optval = curlx_sltosi(data->set.tcp_keepidle);
    alive_unit(&optval);
    જો(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_THRESHOLD,
                  (શૂન્ય *)&optval, કદ(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_THRESHOLD on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPINTVL
    optval = curlx_sltosi(data->set.tcp_keepintvl);
    alive_unit(&optval);
    જો(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (શૂન્ય *)&optval, કદ(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPINTVL on fd "
                  "%" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#અથવાજો defined(TCP_KEEPALIVE_ABORT_THRESHOLD)
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
      પૂર્ણાંક keepcnt = curlx_sltosi(data->set.tcp_keepcnt);
      પૂર્ણાંક keepintvl = curlx_sltosi(data->set.tcp_keepintvl);

      જો(keepcnt > 0 && keepintvl > (INT_MAX / keepcnt))
        optval = INT_MAX;
      નહીંતર
        optval = keepcnt * keepintvl;
    }
    alive_unit(&optval);
    જો(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPALIVE_ABORT_THRESHOLD,
                  (શૂન્ય *)&optval, કદ(optval)) < 0) {
      CURL_TRC_CF(data, cf, "Failed to set TCP_KEEPALIVE_ABORT_THRESHOLD"
                  " on fd %" FMT_SOCKET_T ": errno %d", sockfd, SOCKERRNO);
    }
#endif
#ifdef TCP_KEEPCNT
    optval = curlx_sltosi(data->set.tcp_keepcnt);
    જો(setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
                  (શૂન્ય *)&optval, કદ(optval)) < 0) {
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
CURLcode Curl_socket_addr_from_ai(સંરચના Curl_sockaddr_ex *addr,
                                  અચલ સંરચના Curl_addrinfo *ai,
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
  addr->addrlen = (unsigned પૂર્ણાંક)ai->ai_addrlen;

  DEBUGASSERT(addr->addrlen <= કદ(addr->curl_sa_addrbuf));
  જો(addr->addrlen > કદ(addr->curl_sa_addrbuf))
    પાછા CURLE_TOO_LARGE;

  memcpy(&addr->curl_sa_addrbuf, ai->ai_addr, addr->addrlen);
  પાછા CURLE_OK;
}

#ifdef USE_SO_NOSIGPIPE
પૂર્ણાંક Curl_sock_nosigpipe(curl_socket_t sockfd)
{
  પૂર્ણાંક onoff = 1;
  પાછા setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE,
                    (શૂન્ય *)&onoff, કદ(onoff));
}
#endif /* USE_SO_NOSIGPIPE */

#જો defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
સ્થિર uint32_t get_scope_id(સંરચના Curl_easy *data,
                             સંરચના sockaddr_in6 *sa6)
{
  uint32_t scope_id = 0;
  જો(data->conn->scope_id)
    પાછા data->conn->scope_id;
  /* NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Assign) */
  scope_id = sa6->sin6_scope_id;
  જો(!scope_id && IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
    /* The resolver did not set scope_id for this link-local address.
     * Try to determine it from the system's network interfaces.
     * Without a scope_id, connect() to a link-local address fails
     * with EINVAL on Linux.
     * NOTE: On multi-homed hosts with several interfaces having
     * link-local addresses, this picks the first one found, which
     * may not be the correct outgoing interface. */
#જો defined(HAVE_GETIFADDRS) && defined(HAVE_NET_IF_H)
    સંરચના ifaddrs *ifa, *ifa_list;
    જો(getifaddrs(&ifa_list) == 0) {
      માટે(ifa = ifa_list; ifa; ifa = ifa->ifa_next) {
        જો(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6 &&
           (ifa->ifa_flags & IFF_UP) &&
           !(ifa->ifa_flags & IFF_LOOPBACK)) {
          સંરચના sockaddr_in6 *s6 = (શૂન્ય *)ifa->ifa_addr;
          જો(IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) && s6->sin6_scope_id) {
            scope_id = s6->sin6_scope_id;
            infof(data,
                  "determined scope_id=%lu for link-local address "
                  "from local interface",
                  (unsigned long)scope_id);
            તોડો;
          }
        }
      }
      freeifaddrs(ifa_list);
    }
#endif /* HAVE_GETIFADDRS && HAVE_NET_IF_H */
  }
  પાછા scope_id;
}
#endif

સ્થિર CURLcode socket_open(સંરચના Curl_easy *data,
                            સંરચના Curl_sockaddr_ex *addr,
                            curl_socket_t *sockfd)
{
  અક્ષર errbuf[STRERROR_LEN];

#ifdef SOCK_CLOEXEC
  addr->socktype |= SOCK_CLOEXEC;
#endif

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  જો(data->set.fopensocket) {
    /*
     * If the opensocket callback is set, all the destination address
     * information is passed to the callback. Depending on this information the
     * callback may opt to abort the connection, this is indicated returning
     * CURL_SOCKET_BAD; otherwise it will return a not-connected socket. When
     * the callback returns a valid socket the destination address information
     * might have been changed and this 'new' address will actually be used
     * here to connect.
     */
    સંરચના Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fopensocket);
    *sockfd = data->set.fopensocket(data->set.opensocket_client,
                                    CURLSOCKTYPE_IPCXN,
                                    (સંરચના curl_sockaddr *)addr);
    CURL_CBAPI_END(&guard);
  }
  નહીંતર {
    /* opensocket callback not set, so create the socket now */
#ifdef DEBUGBUILD
    જો((addr->family == AF_INET6) && getenv("CURL_DBG_SOCK_FAIL_IPV6")) {
      failf(data, "CURL_DBG_SOCK_FAIL_IPV6: failed to open socket");
      પાછા CURLE_COULDNT_CONNECT;
    }
#endif
    *sockfd = CURL_SOCKET(addr->family, addr->socktype, addr->protocol);
    જો((*sockfd == CURL_SOCKET_BAD) && (SOCKERRNO == SOCKENOMEM))
      પાછા CURLE_OUT_OF_MEMORY;
  }

  જો(*sockfd == CURL_SOCKET_BAD) {
    /* no socket, no connection */
    failf(data, "failed to open socket: %s",
          curlx_strerror(SOCKERRNO, errbuf, કદ(errbuf)));
    પાછા CURLE_COULDNT_CONNECT;
  }

#ifdef USE_SO_NOSIGPIPE
  જો(Curl_sock_nosigpipe(*sockfd) < 0) {
    failf(data, "setsockopt enable SO_NOSIGPIPE: %s",
          curlx_strerror(SOCKERRNO, errbuf, કદ(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    પાછા CURLE_COULDNT_CONNECT;
  }
#endif /* USE_SO_NOSIGPIPE */

#જો defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC)
  જો(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, કદ(errbuf)));
    sclose(*sockfd);
    *sockfd = CURL_SOCKET_BAD;
    પાછા CURLE_COULDNT_CONNECT;
  }
#endif

#જો defined(USE_IPV6) && defined(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID)
  જો(addr->family == AF_INET6) {
    સંરચના sockaddr_in6 * અચલ sa6 = (શૂન્ય *)&addr->curl_sa_addr;
    sa6->sin6_scope_id = get_scope_id(data, sa6);
  }
#endif
  પાછા CURLE_OK;
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
CURLcode Curl_socket_open(સંરચના Curl_easy *data,
                          અચલ સંરચના Curl_addrinfo *ai,
                          સંરચના Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd)
{
  સંરચના Curl_sockaddr_ex dummy;
  CURLcode result;

  જો(!addr)
    /* if the caller does not want info back, use a local temp copy */
    addr = &dummy;

  result = Curl_socket_addr_from_ai(addr, ai, transport);
  જો(result)
    પાછા result;

  પાછા socket_open(data, addr, sockfd);
}

સ્થિર પૂર્ણાંક socket_close(સંરચના Curl_easy *data, સંરચના connectdata *conn,
                        પૂર્ણાંક use_callback, curl_socket_t sock)
{
  જો(sock == CURL_SOCKET_BAD)
    પાછા 0;

  જો(use_callback && conn && conn->fclosesocket) {
    સંરચના Curl_mapi_guard guard;
    પૂર્ણાંક rc;
    Curl_multi_will_close(data, sock);
    CURL_CBAPI_START(&guard, data, easy_closesocket);
    rc = conn->fclosesocket(conn->closesocket_client, sock);
    CURL_CBAPI_END(&guard);
    પાછા rc;
  }

  જો(conn)
    /* tell the multi-socket code about this */
    Curl_multi_will_close(data, sock);

  sclose(sock);

  પાછા 0;
}

/*
 * Close a socket.
 *
 * 'conn' can be NULL, beware!
 */
પૂર્ણાંક Curl_socket_close(સંરચના Curl_easy *data, સંરચના connectdata *conn,
                      curl_socket_t sock)
{
  પાછા socket_close(data, conn, FALSE, sock);
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
CURLcode Curl_parse_interface(અચલ અક્ષર *આગત,
                              અક્ષર **dev, અક્ષર **iface, અક્ષર **host)
{
  સ્થિર અચલ અક્ષર if_prefix[] = "if!";
  સ્થિર અચલ અક્ષર host_prefix[] = "host!";
  સ્થિર અચલ અક્ષર if_host_prefix[] = "ifhost!";
  size_t len;

  len = strlen(આગત);
  જો(len > 512)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જો(dev)
    *dev = ખાલી;
  જો(iface)
    *iface = ખાલી;
  જો(host)
    *host = ખાલી;

  જો(!strncmp(if_prefix, આગત, strlen(if_prefix))) {
    આગત += strlen(if_prefix);
    જો(!*આગત)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    જો(iface) {
      *iface = curlx_memdup0(આગત, len - strlen(if_prefix));
      પાછા *iface ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    પાછા CURLE_OK;
  }
  નહીંતર જો(!strncmp(host_prefix, આગત, strlen(host_prefix))) {
    આગત += strlen(host_prefix);
    જો(!*આગત)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    જો(host) {
      *host = curlx_memdup0(આગત, len - strlen(host_prefix));
      પાછા *host ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    }
    પાછા CURLE_OK;
  }
  નહીંતર જો(!strncmp(if_host_prefix, આગત, strlen(if_host_prefix))) {
    અચલ અક્ષર *host_part;
    આગત += strlen(if_host_prefix);
    len -= strlen(if_host_prefix);
    host_part = memchr(આગત, '!', len);
    જો(!host_part || !*(host_part + 1))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    જો(iface) {
      *iface = curlx_memdup0(આગત, host_part - આગત);
      જો(!*iface)
        પાછા CURLE_OUT_OF_MEMORY;
    }
    ++host_part;
    જો(host) {
      *host = curlx_memdup0(host_part, len - (host_part - આગત));
      જો(!*host) {
        જો(iface)
          curlx_safefree(*iface);
        પાછા CURLE_OUT_OF_MEMORY;
      }
    }
    પાછા CURLE_OK;
  }

  જો(!*આગત)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  જો(dev) {
    *dev = curlx_memdup0(આગત, len);
    પાછા *dev ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  પાછા CURLE_OK;
}

#ifndef CURL_DISABLE_BINDLOCAL
સ્થિર CURLcode bindlocal(સંરચના Curl_easy *data, સંરચના connectdata *conn,
                          curl_socket_t sockfd, પૂર્ણાંક af, unsigned પૂર્ણાંક scope,
                          uint8_t transport)
{
  સંરચના Curl_sockaddr_storage sa;
  સંરચના sockaddr *sock = (સંરચના sockaddr *)&sa;  /* bind to this address */
  curl_socklen_t sizeof_sa = 0; /* size of the data sock points to */
  સંરચના sockaddr_in *si4 = (સંરચના sockaddr_in *)&sa;
#ifdef USE_IPV6
  સંરચના sockaddr_in6 *si6 = (સંરચના sockaddr_in6 *)&sa;
#endif
  સંરચના Curl_dns_entry *h = ખાલી;
  unsigned short port = data->set.localport; /* use this port number, 0 for
                                                "random" */
  /* how many port numbers to try to bind to, increasing one at a time */
  પૂર્ણાંક portnum = data->set.localportrange;
  અક્ષર *dev_in = ખાલી, *iface_in = ખાલી, *host_in = ખાલી;
  અચલ અક્ષર *iface = ખાલી, *host = ખાલી;
  પૂર્ણાંક sockerr;
#ifdef IP_BIND_ADDRESS_NO_PORT
  પૂર્ણાંક on = 1;
#endif
  CURLcode result = CURLE_OK;

#ifndef USE_IPV6
  (શૂન્ય)scope;
#endif
  જો(conn->interface_opts) {
    /*************************************************************
     * Select device to bind socket to
     *************************************************************/
    result = Curl_parse_interface(conn->interface_opts,
                                  &dev_in, &iface_in, &host_in);
    જો(result)
      જાઓ out;
    /* This is how we use the values */
    iface = iface_in ? iface_in : dev_in;
    host = host_in ? host_in : dev_in;
  }

  જો(!iface && !host && !port)
    જાઓ out;

  જો(iface && (strlen(iface) >= 255)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    જાઓ out;
  }
  memset(&sa, 0, કદ(સંરચના Curl_sockaddr_storage));

  જો(iface || host) {
    અક્ષર myhost[256] = "";
    પૂર્ણાંક done = 0; /* -1 for error, 1 for address found */
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
    જો(iface &&
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
      જાઓ out;
    }
#endif
    જો(!host_in) {
      /* Discover IP from input device, then bind to it */
      if2ip_result = Curl_if2ip(af,
#ifdef USE_IPV6
                                scope, conn->scope_id,
#endif
                                iface, myhost, કદ(myhost));
    }
    પસંદગી(if2ip_result) {
    સ્થિતિ IF2IP_NOT_FOUND:
      જો(iface_in && !host_in) {
        /* Do not fall back to treating it as a hostname */
        અક્ષર buffer[STRERROR_LEN];
        data->state.os_errno = sockerr = SOCKERRNO;
        failf(data, "Could not bind to interface '%s' with errno %d: %s",
              iface, sockerr, curlx_strerror(sockerr, buffer, કદ(buffer)));
        result = CURLE_INTERFACE_FAILED;
        જાઓ out;
      }
      તોડો;
    સ્થિતિ IF2IP_AF_NOT_SUPPORTED:
      /* Signal the caller to try another address family if available */
      result = CURLE_UNSUPPORTED_PROTOCOL;
      જાઓ out;
    સ્થિતિ IF2IP_FOUND:
      /*
       * We now have the numerical IP address in the 'myhost' buffer
       */
      host = myhost;
      infof(data, "Local Interface %s is ip %s using address family %d",
            iface, host, af);
      done = 1;
      તોડો;
    }
    જો(!iface_in || host_in) {
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
      જો(af == AF_INET6)
        dns_queries = CURL_DNSQ_AAAA;
#endif

      (શૂન્ય)Curl_resolv_blocking(data, dns_queries, host, 80, transport, &h);
      જો(h) {
        પૂર્ણાંક h_af = h->addr->ai_family;
        /* convert the resolved address, sizeof myhost >= INET_ADDRSTRLEN */
        Curl_printable_address(h->addr, myhost, કદ(myhost));
        infof(data, "Name '%s' family %d resolved to '%s' family %d",
              host, af, myhost, h_af);
        Curl_dns_entry_unlink(data, &h); /* this will NULL, potential free h */
        જો(af != h_af) {
          /* bad IP version combo, signal the caller to try another address
             family if available */
          result = CURLE_UNSUPPORTED_PROTOCOL;
          જાઓ out;
        }
        done = 1;
      }
      નહીંતર {
        /*
         * provided dev was no interface (or interfaces are not supported
         * e.g. Solaris) no ip address and no domain we fail here
         */
        done = -1;
      }
    }

    જો(done > 0) {
#ifdef USE_IPV6
      /* IPv6 address */
      જો(af == AF_INET6) {
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
        અક્ષર *scope_ptr = strchr(myhost, '%');
        જો(scope_ptr)
          *(scope_ptr++) = '\0';
#endif
        જો(curlx_inet_pton(AF_INET6, myhost, &si6->sin6_addr) > 0) {
          si6->sin6_family = AF_INET6;
          si6->sin6_port = htons(port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
          જો(scope_ptr) {
            /* The "myhost" string either comes from Curl_if2ip or from
               Curl_printable_address. The latter returns only numeric scope
               IDs and the former returns none at all. Making the scope ID,
               if present, known to be numeric */
            curl_off_t scope_id;
            જો(curlx_str_સંખ્યા((અચલ અક્ષર **)CURL_UNCONST(&scope_ptr),
                                &scope_id, UINT_MAX)) {
              result = CURLE_UNSUPPORTED_PROTOCOL;
              જાઓ out;
            }
            si6->sin6_scope_id = (unsigned પૂર્ણાંક)scope_id;
          }
#endif
        }
        sizeof_sa = કદ(સંરચના sockaddr_in6);
      }
      નહીંતર
#endif
      /* IPv4 address */
      જો((af == AF_INET) &&
         (curlx_inet_pton(AF_INET, myhost, &si4->sin_addr) > 0)) {
        si4->sin_family = AF_INET;
        si4->sin_port = htons(port);
        sizeof_sa = કદ(સંરચના sockaddr_in);
      }
    }

    જો(done < 1) {
      /* errorbuf is set false so failf will overwrite any message already in
         the error buffer, so the user receives this error message instead of a
         generic resolve error. */
      અક્ષર buffer[STRERROR_LEN];
      data->state.errorbuf = FALSE;
      data->state.os_errno = sockerr = SOCKERRNO;
      failf(data, "Could not bind to '%s' with errno %d: %s", host,
            sockerr, curlx_strerror(sockerr, buffer, કદ(buffer)));
      result = CURLE_INTERFACE_FAILED;
      જાઓ out;
    }
  }
  નહીંતર {
    /* no device was given, prepare sa to match af's needs */
#ifdef USE_IPV6
    જો(af == AF_INET6) {
      si6->sin6_family = AF_INET6;
      si6->sin6_port = htons(port);
      sizeof_sa = કદ(સંરચના sockaddr_in6);
    }
    નહીંતર
#endif
    જો(af == AF_INET) {
      si4->sin_family = AF_INET;
      si4->sin_port = htons(port);
      sizeof_sa = કદ(સંરચના sockaddr_in);
    }
  }
#ifdef IP_BIND_ADDRESS_NO_PORT
  (શૂન્ય)setsockopt(sockfd, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &on, કદ(on));
#endif
  માટે(;;) {
    જો(bind(sockfd, sock, sizeof_sa) >= 0) {
      /* we succeeded to bind */
      infof(data, "Local port: %hu", port);
      conn->bits.bound = TRUE;
      result = CURLE_OK;
      જાઓ out;
    }

    જો(--portnum > 0) {
      port++; /* try next port */
      જો(port == 0)
        તોડો;
      infof(data, "Bind to local port %d failed, trying next", port - 1);
      /* We reuse/clobber the port variable here below */
      જો(sock->sa_family == AF_INET)
        si4->sin_port = htons(port);
#ifdef USE_IPV6
      નહીંતર
        si6->sin6_port = htons(port);
#endif
    }
    નહીંતર
      તોડો;
  }
  {
    અક્ષર buffer[STRERROR_LEN];
    data->state.os_errno = sockerr = SOCKERRNO;
    failf(data, "bind failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, કદ(buffer)));
  }

  result = CURLE_INTERFACE_FAILED;

out:
  curlx_free(dev_in);
  curlx_free(iface_in);
  curlx_free(host_in);
  પાછા result;
}
#endif

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
સ્થિર bool verifyconnect(curl_socket_t sockfd, પૂર્ણાંક *psockerr)
{
  bool rc = TRUE;
#ifdef SO_ERROR
  પૂર્ણાંક sockerr = 0;
  curl_socklen_t errSize = કદ(sockerr);

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

  જો(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (શૂન્ય *)&sockerr, &errSize))
    sockerr = SOCKERRNO;
#જો defined(EBADIOCTL) && defined(__minix)
  /* Minix 3.1.x does not support getsockopt on UDP sockets */
  જો(EBADIOCTL == sockerr) {
    SET_SOCKERRNO(0);
    sockerr = 0;
  }
#endif
  જો((sockerr == 0) || (SOCKEISCONN == sockerr))
    /* we are connected, awesome! */
    rc = TRUE;
  નહીંતર
    /* This was not a successful connect */
    rc = FALSE;
  જો(psockerr)
    *psockerr = sockerr;
#નહીંતર
  (શૂન્ય)sockfd;
  જો(psockerr)
    *psockerr = SOCKERRNO;
#endif
  પાછા rc;
}

/**
 * Determine the curl code for a socket connect() == -1 with errno.
 */
સ્થિર CURLcode socket_connect_result(સંરચના Curl_easy *data,
                                      અચલ અક્ષર *ipaddress, પૂર્ણાંક sockerr)
{
  જો(sockerr == SOCKEINPROGRESS || SOCK_EAGAIN(sockerr))
    પાછા CURLE_OK;

  /* unknown error, fallthrough and try another address! */
  {
    VERBOSE(અક્ષર buffer[STRERROR_LEN]);
    infof(data, "Immediate connect fail for %s: %s", ipaddress,
          curlx_strerror(sockerr, buffer, કદ(buffer)));
    NOVERBOSE((શૂન્ય)ipaddress);
  }
  data->state.os_errno = sockerr;
  /* connect failed */
  પાછા CURLE_COULDNT_CONNECT;
}

સંરચના cf_socket_ctx {
  સંરચના Curl_peer *peer;
  સંરચના Curl_sockaddr_ex addr;      /* address to connect to */
  curl_socket_t sock;                /* current attempt socket */
  સંરચના ip_quadruple ip;            /* The IP quadruple 2x(addr+port) */
  સંરચના curltime started_at;        /* when socket was created */
  સંરચના curltime connected_at;      /* when socket connected/got first byte */
  સંરચના curltime first_byte_at;     /* when first byte was recvd */
#ifdef USE_WINSOCK
  સંરચના curltime last_sndbuf_query_at;  /* when SO_SNDBUF last queried */
  ULONG sndbuf_size;                     /* the last set SO_SNDBUF size */
#endif
  પૂર્ણાંક sockerr;                       /* socket error of last failure or 0 */
#ifdef DEBUGBUILD
  પૂર્ણાંક connect_wait_ms;               /* ms to wait after 1st connect() */
  પૂર્ણાંક wblock_percent;                /* percent of writes doing EAGAIN */
  પૂર્ણાંક wpartial_percent;              /* percent of bytes written in send */
  પૂર્ણાંક rblock_percent;                /* percent of reads doing EAGAIN */
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

સ્થિર CURLcode cf_socket_ctx_init(સંરચના cf_socket_ctx *ctx,
                                   સંરચના Curl_peer *peer,
                                   સંરચના Curl_sockaddr_ex *addr,
                                   uint8_t transport)
{
  memset(ctx, 0, કદ(*ctx));
  Curl_peer_link(&ctx->peer, peer);
  ctx->sock = CURL_SOCKET_BAD;
  ctx->transport = transport;
  ctx->addr = *addr;

#ifdef DEBUGBUILD
  {
    અચલ અક્ષર *p = getenv("CURL_DBG_SOCK_WBLOCK");
    જો(p) {
      curl_off_t l;
      જો(!curlx_str_સંખ્યા(&p, &l, 100))
        ctx->wblock_percent = (પૂર્ણાંક)l;
    }
    p = getenv("CURL_DBG_SOCK_WPARTIAL");
    જો(p) {
      curl_off_t l;
      જો(!curlx_str_સંખ્યા(&p, &l, 100))
        ctx->wpartial_percent = (પૂર્ણાંક)l;
    }
    p = getenv("CURL_DBG_SOCK_RBLOCK");
    જો(p) {
      curl_off_t l;
      જો(!curlx_str_સંખ્યા(&p, &l, 100))
        ctx->rblock_percent = (પૂર્ણાંક)l;
    }
    p = getenv("CURL_DBG_SOCK_RMAX");
    જો(p) {
      curl_off_t l;
      જો(!curlx_str_સંખ્યા(&p, &l, CURL_OFF_T_MAX))
        ctx->recv_max = (size_t)l;
    }
    p = getenv("CURL_DBG_SOCK_CONNECT_WAIT_MS");
    જો(p) {
      curl_off_t l;
      જો(!curlx_str_સંખ્યા(&p, &l, INT_MAX))
        ctx->connect_wait_ms = (પૂર્ણાંક)l;
    }
  }
#endif

  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cf_socket_ctx_free(સંરચના cf_socket_ctx *ctx)
{
  જો(ctx) {
    Curl_peer_unlink(&ctx->peer);
    curlx_free(ctx);
  }
}

સ્થિર CURLcode cf_socket_shutdown(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   bool *done)
{
  જો(cf->connected) {
    સંરચના cf_socket_ctx *ctx = cf->ctx;

    CURL_TRC_CF(data, cf, "cf_socket_shutdown, fd=%" FMT_SOCKET_T, ctx->sock);
    /* On TCP, and when the socket looks well and non-blocking mode
     * can be enabled, receive dangling bytes before close to avoid
     * entering RST states unnecessarily. */
    જો(ctx->sock != CURL_SOCKET_BAD &&
       ctx->transport == TRNSPRT_TCP &&
       (curlx_nonblock(ctx->sock, TRUE) >= 0)) {
      unsigned અક્ષર buf[1024];
      (શૂન્ય)sread(ctx->sock, buf, કદ(buf));
    }
  }
  *done = TRUE;
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cf_socket_destroy(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  જો(ctx) {
    જો(ctx->sock != CURL_SOCKET_BAD) {
      CURL_TRC_CF(data, cf, "cf_socket_close, fd=%" FMT_SOCKET_T, ctx->sock);
      જો(ctx->sock == cf->conn->sock[cf->sockindex])
        cf->conn->sock[cf->sockindex] = CURL_SOCKET_BAD;
      socket_close(data, cf->conn, !ctx->accepted, ctx->sock);
    }
    cf_socket_ctx_free(ctx);
  }
}

સ્થિર શૂન્ય set_local_ip(સંરચના Curl_cfilter *cf,
                         સંરચના Curl_easy *data)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  ctx->ip.local_ip[0] = 0;
  ctx->ip.local_port = 0;

#ifdef HAVE_GETSOCKNAME
  જો((ctx->sock != CURL_SOCKET_BAD) &&
     !(data->conn->scheme->protocol & CURLPROTO_TFTP)) {
    /* TFTP does not connect, so it cannot get the IP like this */
    સંરચના Curl_sockaddr_storage ssloc;
    curl_socklen_t slen = કદ(સંરચના Curl_sockaddr_storage);

    memset(&ssloc, 0, કદ(ssloc));
    જો(getsockname(ctx->sock, (સંરચના sockaddr *)&ssloc, &slen)) {
      VERBOSE(અક્ષર buffer[STRERROR_LEN]);
      VERBOSE(પૂર્ણાંક sockerr = SOCKERRNO);
      infof(data, "getsockname() failed with errno %d: %s",
            sockerr, curlx_strerror(sockerr, buffer, કદ(buffer)));
    }
    નહીંતર {
      CURLcode result = sockaddr2string((સંરચના sockaddr *)&ssloc, slen,
                                        ctx->ip.local_ip, &ctx->ip.local_port);
      જો(result)
        infof(data, "ssloc inet_ntop() failed with %d", (પૂર્ણાંક)result);
    }
  }
#નહીંતર
  (શૂન્ય)data;
#endif
}

સ્થિર CURLcode set_remote_ip(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  CURLcode result;

  /* store remote address and port used in this connection attempt */
  ctx->ip.transport = ctx->transport;
  result = sockaddr2string(&ctx->addr.curl_sa_addr,
                           (curl_socklen_t)ctx->addr.addrlen,
                           ctx->ip.remote_ip, &ctx->ip.remote_port);
  જો(result) {
    ctx->sockerr = SOCKEAFNOSUPPORT;
    /* malformed address or bug in inet_ntop, try next address */
    failf(data, "curl_sa_addr inet_ntop() failed with %d", (પૂર્ણાંક)result);
    પાછા CURLE_FAILED_INIT;
  }
  પાછા CURLE_OK;
}

/* to figure out the type of the socket safely, remove the possibly ORed
   bits before comparing */
સ્થિર પૂર્ણાંક cf_socktype(પૂર્ણાંક x)
{
#ifdef SOCK_CLOEXEC
  x &= ~SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  x &= ~SOCK_NONBLOCK;
#endif
  પાછા x;
}

#ifdef _WIN32
/* Offered by mingw-w64 v10+, MS SDK 8.0/~VS2012+ */
#ifndef SIO_TCP_INITIAL_RTO
#define SIO_TCP_INITIAL_RTO _WSAIOW(IOC_VENDOR, 17)
#define TCP_INITIAL_RTO_DEFAULT_RTT 0

/* !checksrc! disable TYPEDEFSTRUCT 1 */
પ્રકારવ્યાખ્યા સંરચના _TCP_INITIAL_RTO_PARAMETERS {
  USHORT Rtt;
  UCHAR MaxSynRetransmissions;
} TCP_INITIAL_RTO_PARAMETERS;
#endif /* SIO_TCP_INITIAL_RTO */

#ifndef TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS
#define TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS 0xFE /* -2 */
#endif

સ્થિર bool targets_localhost(સંરચના cf_socket_ctx *ctx)
{
  પાછા (((ctx->addr.family == AF_INET) &&
           !strcmp(ctx->ip.remote_ip, "127.0.0.1")) ||
          ((ctx->addr.family == AF_INET6) &&
           !strcmp(ctx->ip.remote_ip, "::1")));
}

/* disable TCP SYN retransmissions for localhost connection on Windows to
   detect problems faster */
સ્થિર શૂન્ય tcplocalhost(સંરચના Curl_cfilter *cf,
                         curl_socket_t sockfd)
{
  જો(targets_localhost(cf->ctx)) {
    TCP_INITIAL_RTO_PARAMETERS rto;
    DWORD bytes = 0;
    memset(&rto, 0, કદ(rto));
    rto.Rtt = TCP_INITIAL_RTO_DEFAULT_RTT;
    /* Windows 10, version 1709 (10.0.16299) and later versions can set max
       retransmissions to zero, earlier versions could only set 1 */
    જો(curlx_verify_windows_version(10, 0, 16299, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL))
      rto.MaxSynRetransmissions = TCP_INITIAL_RTO_NO_SYN_RETRANSMISSIONS;
    નહીંતર
      rto.MaxSynRetransmissions = 1;
    (શૂન્ય)WSAIoctl(sockfd, SIO_TCP_INITIAL_RTO, &rto, કદ(rto),
                   ખાલી, 0, &bytes, ખાલી, ખાલી);
  }
}
#નહીંતર
#define tcplocalhost(x, y)
#endif /* _WIN32 */

સ્થિર CURLcode cf_socket_open(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  પૂર્ણાંક error = 0;
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
  જો(!data->set.fopensocket)
    ctx->addr.socktype |= SOCK_NONBLOCK;
#endif
  result = socket_open(data, &ctx->addr, &ctx->sock);
#ifdef CURL_USE_SOCK_NONBLOCK
  /* Restore the socktype after the socket is created. */
  જો(!data->set.fopensocket)
    ctx->addr.socktype &= ~SOCK_NONBLOCK;
#endif
  જો(result)
    જાઓ out;

  result = set_remote_ip(cf, data);
  જો(result)
    જાઓ out;

#ifdef USE_IPV6
  જો(ctx->addr.family == AF_INET6) {
#ifdef USE_WINSOCK
    /* Turn on support for IPv4-mapped IPv6 addresses.
     * Linux kernel, NetBSD, FreeBSD, Darwin, lwIP: default is off;
     * Windows Vista and later: default is on;
     * DragonFly BSD: acts like off, and dummy setting;
     * OpenBSD and earlier Windows: unsupported.
     * Linux: controlled by /proc/sys/net/ipv6/bindv6only.
     */
    પૂર્ણાંક on = 0;
    (શૂન્ય)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY,
                     (શૂન્ય *)&on, કદ(on));
#endif
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    {
      સંરચના sockaddr_in6 *sa6 = (શૂન્ય *)&ctx->addr.curl_sa_addr;
      જો(sa6->sin6_scope_id)
        infof(data, "  Trying [%s]:%d scope_id=%lu...",
              ctx->ip.remote_ip, ctx->ip.remote_port,
              (unsigned long)sa6->sin6_scope_id);
      નહીંતર
#endif
        infof(data, "  Trying [%s]:%d...",
              ctx->ip.remote_ip, ctx->ip.remote_port);
#ifdef HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID
    }
#endif
  }
  નહીંતર
#endif
    infof(data, "  Trying %s:%d...", ctx->ip.remote_ip, ctx->ip.remote_port);

#ifdef USE_IPV6
  is_tcp = (ctx->addr.family == AF_INET ||
            ctx->addr.family == AF_INET6) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#નહીંતર
  is_tcp = (ctx->addr.family == AF_INET) &&
    cf_socktype(ctx->addr.socktype) == SOCK_STREAM;
#endif
  જો(is_tcp) {
    જો(data->set.tcp_nodelay)
      tcpnodelay(cf, data, ctx->sock);

    જો(data->set.tcp_keepalive)
      tcpkeepalive(cf, data, ctx->sock);

    tcplocalhost(cf, ctx->sock);
  }

  જો(data->set.fsockopt) {
    /* activate callback for setting socket options */
    સંરચના Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock,
                               CURLSOCKTYPE_IPCXN);
    CURL_CBAPI_END(&guard);

    જો(error == CURL_SOCKOPT_ALREADY_CONNECTED)
      isconnected = TRUE;
    નહીંતર જો(error) {
      result = CURLE_ABORTED_BY_CALLBACK;
      જાઓ out;
    }
  }

#ifndef CURL_DISABLE_BINDLOCAL
  /* possibly bind the local end to an IP, interface or port */
  જો(ctx->addr.family == AF_INET
#ifdef USE_IPV6
     || ctx->addr.family == AF_INET6
#endif
    ) {
    result = bindlocal(data, cf->conn, ctx->sock, ctx->addr.family,
                       Curl_ipv6_scope(&ctx->addr.curl_sa_addr),
                       ctx->transport);
    જો(result) {
      જો(result == CURLE_UNSUPPORTED_PROTOCOL) {
        /* The address family is not supported on this interface.
           We can continue trying addresses */
        result = CURLE_COULDNT_CONNECT;
      }
      જાઓ out;
    }
  }
#endif

#ifndef CURL_USE_SOCK_NONBLOCK
  /* Set socket non-blocking, must be a non-blocking socket for
   * a non-blocking connect. */
  error = curlx_nonblock(ctx->sock, TRUE);
  જો(error < 0) {
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ctx->sockerr = SOCKERRNO;
    જાઓ out;
  }
#નહીંતર
  જો(data->set.fopensocket) {
    /* Set socket non-blocking, must be a non-blocking socket for
     * a non-blocking connect. */
    error = curlx_nonblock(ctx->sock, TRUE);
    જો(error < 0) {
      result = CURLE_UNSUPPORTED_PROTOCOL;
      ctx->sockerr = SOCKERRNO;
      જાઓ out;
    }
  }
#endif
  ctx->sock_connected = (cf_socktype(ctx->addr.socktype) != SOCK_DGRAM);
out:
  જો(result) {
    જો(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
  }
  નહીંતર જો(isconnected) {
    set_local_ip(cf, data);
    ctx->connected_at = *Curl_pgrs_now(data);
    cf->connected = TRUE;
  }
  CURL_TRC_CF(data, cf, "cf_socket_open() -> %d, fd=%" FMT_SOCKET_T,
              (પૂર્ણાંક)result, ctx->sock);
  પાછા result;
}

સ્થિર પૂર્ણાંક do_connect(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                      bool is_tcp_fastopen)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
#ifdef TCP_FASTOPEN_CONNECT
  પૂર્ણાંક optval = 1;
#endif
  પૂર્ણાંક rc = -1;

  (શૂન્ય)data;
  જો(is_tcp_fastopen) {
#ifdef CONNECT_DATA_IDEMPOTENT /* Darwin */
#  ifdef HAVE_BUILTIN_AVAILABLE
    /* while connectx function is available since macOS 10.11 / iOS 9,
       it did not have the interface declared correctly until
       Xcode 9 / macOS SDK 10.13 */
    જો(__builtin_available(macOS 10.11, iOS 9, tvOS 9, watchOS 2, *)) {
      sa_endpoints_t endpoints;
      endpoints.sae_srcif = 0;
      endpoints.sae_srcaddr = ખાલી;
      endpoints.sae_srcaddrlen = 0;
      endpoints.sae_dstaddr = &ctx->addr.curl_sa_addr;
      endpoints.sae_dstaddrlen = ctx->addr.addrlen;

      rc = connectx(ctx->sock, &endpoints, SAE_ASSOCID_ANY,
                    CONNECT_RESUME_ON_READ_WRITE | CONNECT_DATA_IDEMPOTENT,
                    ખાલી, 0, ખાલી, ખાલી);
    }
    નહીંતર {
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    }
#  નહીંતર
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#  endif /* HAVE_BUILTIN_AVAILABLE */
#અથવાજો defined(TCP_FASTOPEN_CONNECT) /* Linux >= 4.11 */
    જો(setsockopt(ctx->sock, IPPROTO_TCP, TCP_FASTOPEN_CONNECT,
                  (શૂન્ય *)&optval, કદ(optval)) < 0)
      CURL_TRC_CF(data, cf, "Failed to enable TCP Fast Open on fd %"
                  FMT_SOCKET_T, ctx->sock);

    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
#અથવાજો defined(MSG_FASTOPEN) /* old Linux */
    જો(Curl_conn_is_ssl(cf->conn, cf->sockindex))
      rc = connect(ctx->sock, &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    નહીંતર
      rc = 0; /* Do nothing */
#endif
  }
  નહીંતર {
    rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
                 (curl_socklen_t)ctx->addr.addrlen);
  }
  પાછા rc;
}

સ્થિર CURLcode cf_tcp_connect(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               bool *done)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;
  પૂર્ણાંક rc = 0;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  *done = FALSE; /* a negative world view is best */
  જો(ctx->sock == CURL_SOCKET_BAD) {
    પૂર્ણાંક sockerr;

    result = cf_socket_open(cf, data);
    જો(result)
      જાઓ out;

    જો(cf->connected) {
      *done = TRUE;
      પાછા CURLE_OK;
    }

    /* Connect TCP socket */
    rc = do_connect(cf, data, (bool)cf->conn->bits.tcp_fastopen);
    sockerr = SOCKERRNO;
    set_local_ip(cf, data);
    CURL_TRC_CF(data, cf, "local address %s port %d",
                ctx->ip.local_ip, ctx->ip.local_port);
#ifdef DEBUGBUILD
    જો(ctx->connect_wait_ms > 0) {
      CURL_TRC_CF(data, cf, "debug connect wait %d ms", ctx->connect_wait_ms);
      curlx_wait_ms(ctx->connect_wait_ms);
    }
#endif
    જો(rc == -1) {
      ctx->sockerr = sockerr;
      result = socket_connect_result(data, ctx->ip.remote_ip, sockerr);
      જાઓ out;
    }
  }

#ifdef mpeix
  /* Call this function once now, and ignore the results. We do this to
     "clear" the error state on the socket so that we can later read it
     reliably. This is reported necessary on the MPE/iX operating
     system. */
  (શૂન્ય)verifyconnect(ctx->sock, ખાલી);
#endif
  /* check socket for connect */
  rc = SOCKET_WRITABLE(ctx->sock, 0);

  જો(rc == 0) { /* no connection yet */
    CURL_TRC_CF(data, cf, "not connected yet on fd=%" FMT_SOCKET_T, ctx->sock);
    પાછા CURLE_OK;
  }
  નહીંતર જો(rc == CURL_CSELECT_OUT ||
#ifdef __APPLE__
          /* macOS reports select errors when the peer already
           * sent us data and closed the connection normally. Do
           * not treat that as error. See #23482 and #23445 */
          (rc > 0 && (rc & CURL_CSELECT_ERR)) ||
#endif
          cf->conn->bits.tcp_fastopen) {
    જો(verifyconnect(ctx->sock, &ctx->sockerr)) {
      /* we are connected with TCP, awesome! */
      ctx->connected_at = *Curl_pgrs_now(data);
      set_local_ip(cf, data);
      *done = TRUE;
      cf->connected = TRUE;
      CURL_TRC_CF(data, cf, "connected on fd=%" FMT_SOCKET_T, ctx->sock);
      પાછા CURLE_OK;
    }
    result = CURLE_COULDNT_CONNECT;
  }
  નહીંતર જો(rc & CURL_CSELECT_ERR) {
    CURL_TRC_CF(data, cf, "poll/select error on fd=%" FMT_SOCKET_T, ctx->sock);
    (શૂન્ય)verifyconnect(ctx->sock, &ctx->sockerr);
    result = CURLE_COULDNT_CONNECT;
  }

out:
  જો(result) {
    VERBOSE(અક્ષર buffer[STRERROR_LEN]);
    set_local_ip(cf, data);
    જો(ctx->sockerr) {
      data->state.os_errno = ctx->sockerr;
      SET_SOCKERRNO(ctx->sockerr);
      VERBOSE(curlx_strerror(ctx->sockerr, buffer, કદ(buffer)));
    }
    નહીંતર {
      VERBOSE(curlx_strcopy(buffer, કદ(buffer), STRCONST("peer closed")));
    }
    જો(ctx->sock != CURL_SOCKET_BAD) {
      socket_close(data, cf->conn, TRUE, ctx->sock);
      ctx->sock = CURL_SOCKET_BAD;
    }
    infof(data, "connect to %s port %u from %s port %d failed: %s",
          ctx->ip.remote_ip, ctx->ip.remote_port,
          ctx->ip.local_ip, ctx->ip.local_port,
          curlx_strerror(ctx->sockerr, buffer, કદ(buffer)));
    *done = FALSE;
  }
  પાછા result;
}

સ્થિર CURLcode cf_socket_adjust_pollset(સંરચના Curl_cfilter *cf,
                                         સંરચના Curl_easy *data,
                                         સંરચના easy_pollset *ps)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  જો(ctx->sock != CURL_SOCKET_BAD) {
    /* A listening socket filter needs to be connected before the accept
     * for some weird FTP interaction. This should be rewritten, so that
     * FTP no longer does the socket checks and accept calls and delegates
     * all that to the filter. */
    જો(ctx->listening) {
      result = Curl_pollset_set_in_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, listening, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    નહીંતર જો(!cf->connected) {
      result = Curl_pollset_set_out_only(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !connected, POLLOUT fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
    નહીંતર જો(!ctx->active) {
      result = Curl_pollset_add_in(data, ps, ctx->sock);
      CURL_TRC_CF(data, cf, "adjust_pollset, !active, POLLIN fd=%"
                  FMT_SOCKET_T, ctx->sock);
    }
  }
  પાછા result;
}

#ifdef USE_WINSOCK

/* Offered by mingw-w64 v13+, MS SDK 7.0A/VS2010+ */
#ifndef SIO_IDEAL_SEND_BACKLOG_QUERY
#define SIO_IDEAL_SEND_BACKLOG_QUERY 0x4004747B
#endif

સ્થિર શૂન્ય win_update_sndbuf_size(સંરચના Curl_easy *data,
                                   સંરચના cf_socket_ctx *ctx)
{
  ULONG ideal;
  DWORD ideallen;

  જો(curlx_ptimediff_ms(Curl_pgrs_now(data),
                        &ctx->last_sndbuf_query_at) > 1000) {
    જો(!WSAIoctl(ctx->sock, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                 &ideal, કદ(ideal), &ideallen, 0, 0) &&
       ideal != ctx->sndbuf_size &&
       !setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF,
                   (અચલ અક્ષર *)&ideal, કદ(ideal))) {
      ctx->sndbuf_size = ideal;
    }
    ctx->last_sndbuf_query_at = *Curl_pgrs_now(data);
  }
}

#endif /* USE_WINSOCK */

સ્થિર CURLcode cf_socket_send(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                               અચલ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  curl_socket_t fdsave;
  ssize_t rv;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t orig_len = len);

  (શૂન્ય)eos;
  *pnwritten = 0;
  fdsave = cf->conn->sock[cf->sockindex];
  cf->conn->sock[cf->sockindex] = ctx->sock;

#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  જો(ctx->wblock_percent > 0) {
    unsigned અક્ષર c = 0;
    Curl_rand_bytes(data, FALSE, &c, 1);
    જો(c >= ((100 - ctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE EWOULDBLOCK", orig_len);
      cf->conn->sock[cf->sockindex] = fdsave;
      પાછા CURLE_AGAIN;
    }
  }
  જો(cf->cft != &Curl_cft_udp && ctx->wpartial_percent > 0 && len > 8) {
    len = len * ctx->wpartial_percent / 100;
    જો(!len)
      len = 1;
    CURL_TRC_CF(data, cf, "send(len=%zu) SIMULATE partial write of %zu bytes",
                orig_len, len);
  }
#endif

#જો defined(MSG_FASTOPEN) && !defined(TCP_FASTOPEN_CONNECT) /* Linux */
  જો(cf->conn->bits.tcp_fastopen) {
    rv = sendto(ctx->sock, buf, len, MSG_FASTOPEN,
                &ctx->addr.curl_sa_addr, ctx->addr.addrlen);
    cf->conn->bits.tcp_fastopen = FALSE;
  }
  નહીંતર
#endif
    rv = swrite(ctx->sock, buf, len);

  જો(!curlx_sztouz(rv, pnwritten)) {
    પૂર્ણાંક sockerr = SOCKERRNO;
    જો(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    નહીંતર {
      અક્ષર buffer[STRERROR_LEN];
      failf(data, "Send failure: %s",
            curlx_strerror(sockerr, buffer, કદ(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_SEND_ERROR;
    }
  }

#ifdef USE_WINSOCK
  જો(!result)
    win_update_sndbuf_size(data, ctx);
#endif

  CURL_TRC_CF(data, cf, "send(len=%zu) -> %d, %zu",
              orig_len, (પૂર્ણાંક)result, *pnwritten);
  cf->conn->sock[cf->sockindex] = fdsave;
  પાછા result;
}

સ્થિર CURLcode cf_socket_recv(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                               અક્ષર *buf, size_t len, size_t *pnread)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ssize_t rv;

  *pnread = 0;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial reads */
  જો(cf->cft != &Curl_cft_udp && ctx->rblock_percent > 0) {
    unsigned અક્ષર c = 0;
    Curl_rand(data, &c, 1);
    જો(c >= ((100 - ctx->rblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE EWOULDBLOCK", len);
      પાછા CURLE_AGAIN;
    }
  }
  જો(cf->cft != &Curl_cft_udp && ctx->recv_max && ctx->recv_max < len) {
    CURL_TRC_CF(data, cf, "recv(len=%zu) SIMULATE max read of %zu bytes",
                len, ctx->recv_max);
    len = ctx->recv_max;
  }
#endif

  rv = sread(ctx->sock, buf, len);

  જો(!curlx_sztouz(rv, pnread)) {
    પૂર્ણાંક sockerr = SOCKERRNO;
    જો(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
       || (sockerr == SOCKEINTR)
#endif
      ) {
      result = CURLE_AGAIN;  /* EWOULDBLOCK */
    }
    નહીંતર {
      અક્ષર buffer[STRERROR_LEN];
      failf(data, "Recv failure: %s",
            curlx_strerror(sockerr, buffer, કદ(buffer)));
      data->state.os_errno = sockerr;
      result = CURLE_RECV_ERROR;
    }
  }

  CURL_TRC_CF(data, cf, "recv(len=%zu) -> %d, %zu", len, (પૂર્ણાંક)result, *pnread);
  જો(!result && !ctx->got_first_byte) {
    ctx->first_byte_at = *Curl_pgrs_now(data);
    ctx->got_first_byte = TRUE;
  }
  પાછા result;
}

સ્થિર શૂન્ય cf_socket_update_data(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data)
{
  /* Update the IP info held in the transfer, if we have that. */
  જો(cf->connected && (cf->sockindex == FIRSTSOCKET)) {
    સંરચના cf_socket_ctx *ctx = cf->ctx;
    data->info.primary = ctx->ip;
  }
}

સ્થિર શૂન્ય cf_socket_active(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;

  /* use this socket from now on */
  cf->conn->sock[cf->sockindex] = ctx->sock;
  set_local_ip(cf, data);
#ifdef USE_IPV6
  જો(cf->sockindex == FIRSTSOCKET)
    cf->conn->bits.ipv6 = (ctx->addr.family == AF_INET6);
#endif
  ctx->active = TRUE;
}

સ્થિર CURLcode cf_socket_cntrl(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;

  (શૂન્ય)arg1;
  (શૂન્ય)arg2;
  પસંદગી(event) {
  સ્થિતિ CF_CTRL_CONN_INFO_UPDATE:
    cf_socket_active(cf, data);
    cf_socket_update_data(cf, data);
    તોડો;
  સ્થિતિ CF_CTRL_DATA_SETUP:
    cf_socket_update_data(cf, data);
    તોડો;
  સ્થિતિ CF_CTRL_FORGET_SOCKET:
    ctx->sock = CURL_SOCKET_BAD;
    તોડો;
  સ્થિતિ CF_CTRL_REPORT_STATS:
    જો(cf->connected && !ctx->stats_reported) {
      સંરચના curltime *ts = ખાલી;
      પસંદગી(ctx->transport) {
      સ્થિતિ TRNSPRT_UDP:
      સ્થિતિ TRNSPRT_QUIC:
        /* Since UDP connected sockets work different from TCP, we use the
         * time of the first byte from the peer as the "connect" time. */
        જો(ctx->got_first_byte)
          ts = &ctx->first_byte_at;
        તોડો;
      મૂળભૂત:
        ts = &ctx->connected_at;
        તોડો;
      }
      જો(ts) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, *ts);
        ctx->stats_reported = TRUE;
      }
    }
  }
  પાછા CURLE_OK;
}

સ્થિર bool cf_socket_conn_is_alive(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    bool *આગત_pending)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  સંરચના pollfd pfd[1];
  પૂર્ણાંક r;

  *આગત_pending = FALSE;

  જો(!ctx || ctx->sock == CURL_SOCKET_BAD)
    પાછા FALSE;

  /* Check with 0 timeout if there are any events pending on the socket */
  pfd[0].fd = ctx->sock;
  pfd[0].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
  pfd[0].revents = 0;

  r = Curl_poll(pfd, 1, 0);
  જો(r < 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll error, assume dead");
    પાછા FALSE;
  }
  નહીંતર જો(r == 0) {
    CURL_TRC_CF(data, cf, "is_alive: poll timeout, assume alive");
    પાછા TRUE;
  }
  નહીંતર જો(pfd[0].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL)) {
    CURL_TRC_CF(data, cf, "is_alive: err/hup/etc events, assume dead");
    પાછા FALSE;
  }

  CURL_TRC_CF(data, cf, "is_alive: valid events, looks alive");
  *આગત_pending = TRUE;
  પાછા TRUE;
}

સ્થિર CURLcode cf_socket_query(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;

  પસંદગી(query) {
  સ્થિતિ CF_QUERY_SOCKET:
    DEBUGASSERT(pres2);
    *((curl_socket_t *)pres2) = ctx->sock;
    પાછા CURLE_OK;
  સ્થિતિ CF_QUERY_TRANSPORT:
    DEBUGASSERT(pres1);
    *pres1 = ctx->transport;
    પાછા CURLE_OK;
  સ્થિતિ CF_QUERY_REMOTE_ADDR:
    DEBUGASSERT(pres2);
    *((અચલ સંરચના Curl_sockaddr_ex **)pres2) = cf->connected ?
                                                 &ctx->addr : ખાલી;
    પાછા CURLE_OK;
  સ્થિતિ CF_QUERY_CONNECT_REPLY_MS:
    જો(ctx->got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (પૂર્ણાંક)ms : INT_MAX;
    }
    નહીંતર
      *pres1 = -1;
    પાછા CURLE_OK;
  સ્થિતિ CF_QUERY_IP_INFO:
#ifdef USE_IPV6
    *pres1 = (ctx->addr.family == AF_INET6);
#નહીંતર
    *pres1 = FALSE;
#endif
    *(સંરચના ip_quadruple *)pres2 = ctx->ip;
    પાછા CURLE_OK;
  સ્થિતિ CF_QUERY_REALLY_CONNECTED:
    જો(cf->cft != &Curl_cft_udp)
      *pres1 = cf->connected;
    નહીંતર
      *pres1 = ctx->got_first_byte;
    પાછા CURLE_OK;
  મૂળભૂત:
    તોડો;
  }
  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

સંરચના Curl_cftype Curl_cft_tcp = {
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

CURLcode Curl_cf_tcp_create(સંરચના Curl_cfilter **pcf,
                            સંરચના Curl_easy *data,
                            સંરચના Curl_peer *origin,
                            સંરચના Curl_peer *peer,
                            uint8_t transport_peer,
                            સંરચના connectdata *conn,
                            સંરચના Curl_sockaddr_ex *addr,
                            સંરચના Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  સંરચના cf_socket_ctx *ctx = ખાલી;
  સંરચના Curl_cfilter *cf = ખાલી;
  CURLcode result;

  (શૂન્ય)data;
  (શૂન્ય)origin;
  (શૂન્ય)conn;
  (શૂન્ય)tunnel_peer;
  (શૂન્ય)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_TCP);
  જો(!addr) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    જાઓ out;
  }

  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  જો(result)
    જાઓ out;

  result = Curl_cf_create(&cf, &Curl_cft_tcp, ctx);

out:
  *pcf = (!result) ? cf : ખાલી;
  જો(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  પાછા result;
}

#ifdef __linux__
સ્થિર શૂન્ય linux_quic_mtu(સંરચના cf_socket_ctx *ctx)
{
  પૂર્ણાંક val;
  પસંદગી(ctx->addr.family) {
#ifdef IP_MTU_DISCOVER
  સ્થિતિ AF_INET:
    val = IP_PMTUDISC_DO;
    (શૂન્ય)setsockopt(ctx->sock, IPPROTO_IP, IP_MTU_DISCOVER, &val,
                     કદ(val));
    તોડો;
#endif
#ifdef IPV6_MTU_DISCOVER
  સ્થિતિ AF_INET6:
    val = IPV6_PMTUDISC_DO;
    (શૂન્ય)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &val,
                     કદ(val));
    તોડો;
#endif
  }
}
#નહીંતર
#define linux_quic_mtu(x)
#endif

#જો defined(UDP_GRO) &&                                                 \
  (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) &&                  \
  ((defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE))
સ્થિર શૂન્ય linux_quic_gro(સંરચના cf_socket_ctx *ctx)
{
  પૂર્ણાંક one = 1;
  (શૂન્ય)setsockopt(ctx->sock, IPPROTO_UDP, UDP_GRO, &one,
                   (socklen_t)કદ(one));
}
#નહીંતર
#define linux_quic_gro(x)
#endif

#જો (defined(__linux__) || defined(__APPLE__)) && defined(IP_RECVTOS)
સ્થિર શૂન્ય linux_quic_ecn(સંરચના cf_socket_ctx *ctx)
{
  unsigned પૂર્ણાંક tos = 1;
  પસંદગી(ctx->addr.family) {
  સ્થિતિ AF_INET:
    (શૂન્ય)setsockopt(ctx->sock, IPPROTO_IP, IP_RECVTOS, &tos, કદ(tos));
    તોડો;
#ifdef IPV6_RECVTCLASS
  સ્થિતિ AF_INET6:
    (શૂન્ય)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_RECVTCLASS,
                     &tos, કદ(tos));
    તોડો;
#endif
  }
}
#નહીંતર
#define linux_quic_ecn(x)
#endif

#જો (defined(__linux__) || defined(__APPLE__)) && defined(IP_DONTFRAG)
સ્થિર શૂન્ય linux_ip_dontfrag(સંરચના cf_socket_ctx *ctx)
{
  પૂર્ણાંક val = 1;

  પસંદગી(ctx->addr.family) {
  સ્થિતિ AF_INET:
    (શૂન્ય)setsockopt(ctx->sock, IPPROTO_IP, IP_DONTFRAG, &val, કદ(val));
    તોડો;
#ifdef IPV6_DONTFRAG
  સ્થિતિ AF_INET6:
    (શૂન્ય)setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_DONTFRAG,
                     &val, કદ(val));
    તોડો;
#endif
  }
}
#નહીંતર
#define linux_ip_dontfrag(x)
#endif

સ્થિર CURLcode cf_udp_setup_quic(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  પૂર્ણાંક rc;

  /* QUIC needs a connected socket, nonblocking */
  DEBUGASSERT(ctx->sock != CURL_SOCKET_BAD);

  /* error: The 1st argument to 'connect' is -1 but should be >= 0
     NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions) */
  rc = connect(ctx->sock, &ctx->addr.curl_sa_addr,
               (curl_socklen_t)ctx->addr.addrlen);
  જો(rc == -1) {
    પાછા socket_connect_result(data, ctx->ip.remote_ip, SOCKERRNO);
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

  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_udp_connect(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               bool *done)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_COULDNT_CONNECT;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  *done = FALSE;
  જો(ctx->sock == CURL_SOCKET_BAD) {
    result = cf_socket_open(cf, data);
    જો(result) {
      CURL_TRC_CF(data, cf, "cf_udp_connect(), open failed -> %d",
                  (પૂર્ણાંક)result);
      જાઓ out;
    }

    જો(ctx->transport == TRNSPRT_QUIC) {
      result = cf_udp_setup_quic(cf, data);
      જો(result)
        જાઓ out;
      CURL_TRC_CF(data, cf, "cf_udp_connect(), opened socket=%"
                  FMT_SOCKET_T " (%s:%d)",
                  ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
    }
    *done = TRUE;
    cf->connected = TRUE;
  }
out:
  પાછા result;
}

સંરચના Curl_cftype Curl_cft_udp = {
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

CURLcode Curl_cf_udp_create(સંરચના Curl_cfilter **pcf,
                            સંરચના Curl_easy *data,
                            સંરચના Curl_peer *origin,
                            સંરચના Curl_peer *peer,
                            uint8_t transport_peer,
                            સંરચના connectdata *conn,
                            સંરચના Curl_sockaddr_ex *addr,
                            સંરચના Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  સંરચના cf_socket_ctx *ctx = ખાલી;
  સંરચના Curl_cfilter *cf = ખાલી;
  CURLcode result;

  (શૂન્ય)data;
  (શૂન્ય)origin;
  (શૂન્ય)conn;
  (શૂન્ય)tunnel_peer;
  (શૂન્ય)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UDP || transport_peer == TRNSPRT_QUIC);
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  જો(result)
    જાઓ out;

  result = Curl_cf_create(&cf, &Curl_cft_udp, ctx);

out:
  *pcf = (!result) ? cf : ખાલી;
  જો(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  પાછા result;
}

/* this is the TCP filter which can also handle this case */
સંરચના Curl_cftype Curl_cft_unix = {
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

CURLcode Curl_cf_unix_create(સંરચના Curl_cfilter **pcf,
                            સંરચના Curl_easy *data,
                            સંરચના Curl_peer *origin,
                            સંરચના Curl_peer *peer,
                            uint8_t transport_peer,
                            સંરચના connectdata *conn,
                            સંરચના Curl_sockaddr_ex *addr,
                            સંરચના Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport)
{
  સંરચના cf_socket_ctx *ctx = ખાલી;
  સંરચના Curl_cfilter *cf = ખાલી;
  CURLcode result;

  (શૂન્ય)data;
  (શૂન્ય)origin;
  (શૂન્ય)conn;
  (શૂન્ય)tunnel_peer;
  (શૂન્ય)tunnel_transport;
  DEBUGASSERT(transport_peer == TRNSPRT_UNIX);
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  result = cf_socket_ctx_init(ctx, peer, addr, transport_peer);
  જો(result)
    જાઓ out;

  result = Curl_cf_create(&cf, &Curl_cft_unix, ctx);

out:
  *pcf = (!result) ? cf : ખાલી;
  જો(result) {
    curlx_safefree(cf);
    cf_socket_ctx_free(ctx);
  }

  પાછા result;
}

સ્થિર timediff_t cf_tcp_accept_timeleft(સંરચના Curl_cfilter *cf,
                                         સંરચના Curl_easy *data)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  timediff_t timeout_ms = DEFAULT_ACCEPT_TIMEOUT;
  timediff_t other_ms;

#ifndef CURL_DISABLE_FTP
  જો(data->set.accepttimeout > 0)
    timeout_ms = data->set.accepttimeout;
#endif

  /* check if the generic timeout possibly is set shorter */
  other_ms = Curl_timeleft_ms(data);
  જો(other_ms && (other_ms < timeout_ms))
    /* note that this also works fine for when other_ms happens to be negative
       due to it already having elapsed */
    timeout_ms = other_ms;
  નહીંતર {
    /* subtract elapsed time */
    timeout_ms -= curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->started_at);
    જો(!timeout_ms)
      /* avoid returning 0 as that means no timeout! */
      timeout_ms = -1;
  }
  પાછા timeout_ms;
}

સ્થિર શૂન્ય cf_tcp_set_accepted_remote_ip(સંરચના Curl_cfilter *cf,
                                          સંરચના Curl_easy *data)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
#ifdef HAVE_GETPEERNAME
  CURLcode result;
  સંરચના Curl_sockaddr_storage ssrem;
  curl_socklen_t plen;

  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  plen = કદ(ssrem);
  memset(&ssrem, 0, plen);
  જો(getpeername(ctx->sock, (સંરચના sockaddr *)&ssrem, &plen)) {
    અક્ષર buffer[STRERROR_LEN];
    પૂર્ણાંક sockerr = SOCKERRNO;
    failf(data, "getpeername() failed with errno %d: %s",
          sockerr, curlx_strerror(sockerr, buffer, કદ(buffer)));
    પાછા;
  }
  result = sockaddr2string((સંરચના sockaddr *)&ssrem, plen,
                            ctx->ip.remote_ip, &ctx->ip.remote_port);
  જો(result) {
    failf(data, "ssrem inet_ntop() failed with %d", (પૂર્ણાંક)result);
    પાછા;
  }
#નહીંતર
  ctx->ip.remote_ip[0] = 0;
  ctx->ip.remote_port = 0;
  (શૂન્ય)data;
#endif
}

સ્થિર CURLcode cf_tcp_accept_connect(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      bool *done)
{
  સંરચના cf_socket_ctx *ctx = cf->ctx;
  અક્ષર errbuf[STRERROR_LEN];
#ifdef USE_IPV6
  સંરચના Curl_sockaddr_storage add;
#નહીંતર
  સંરચના sockaddr_in add;
#endif
  curl_socklen_t size = (curl_socklen_t)કદ(add);
  curl_socket_t s_accepted = CURL_SOCKET_BAD;
  timediff_t timeout_ms;
  પૂર્ણાંક socketstate = 0;
  bool incoming = FALSE;

  /* we start accepted, if we ever close, we cannot go on */
  (શૂન્ય)data;
  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  *done = FALSE;
  timeout_ms = cf_tcp_accept_timeleft(cf, data);
  જો(timeout_ms < 0) {
    /* if a timeout was already reached, bail out */
    failf(data, "Accept timeout occurred while waiting server connect");
    પાછા CURLE_FTP_ACCEPT_TIMEOUT;
  }

  CURL_TRC_CF(data, cf, "Checking for incoming on fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock, ctx->ip.local_ip, ctx->ip.local_port);
  socketstate = SOCKET_READABLE(ctx->sock, 0);
  CURL_TRC_CF(data, cf, "socket_check -> %x", (unsigned પૂર્ણાંક)socketstate);
  પસંદગી(socketstate) {
  સ્થિતિ -1: /* error */
    /* let's die here */
    failf(data, "Error while waiting for server connect");
    પાછા CURLE_FTP_ACCEPT_FAILED;
  મૂળભૂત:
    જો(socketstate & CURL_CSELECT_IN) {
      infof(data, "Ready to accept data connection from server");
      incoming = TRUE;
    }
    તોડો;
  }

  જો(!incoming) {
    CURL_TRC_CF(data, cf, "nothing heard from the server yet");
    પાછા CURLE_OK;
  }

  size = કદ(add);
#ifdef HAVE_ACCEPT4
  s_accepted = CURL_ACCEPT4(ctx->sock, (સંરચના sockaddr *)&add, &size,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
#નહીંતર
  s_accepted = CURL_ACCEPT(ctx->sock, (સંરચના sockaddr *)&add, &size);
#endif

  જો(s_accepted == CURL_SOCKET_BAD) {
    failf(data, "Error accept()ing server connect: %s",
          curlx_strerror(SOCKERRNO, errbuf, કદ(errbuf)));
    પાછા CURLE_FTP_ACCEPT_FAILED;
  }
#ifndef HAVE_ACCEPT4
#ifdef HAVE_FCNTL
  જો(fcntl(s_accepted, F_SETFD, FD_CLOEXEC) < 0) {
    failf(data, "fcntl set CLOEXEC: %s",
          curlx_strerror(SOCKERRNO, errbuf, કદ(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    પાછા CURLE_FTP_ACCEPT_FAILED;
  }
#endif /* HAVE_FCNTL */
  જો(curlx_nonblock(s_accepted, TRUE) < 0) {
    failf(data, "set socket NONBLOCK: %s",
          curlx_strerror(SOCKERRNO, errbuf, કદ(errbuf)));
    Curl_socket_close(data, cf->conn, s_accepted);
    પાછા CURLE_FTP_ACCEPT_FAILED;
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

  જો(data->set.fsockopt) {
    સંરચના Curl_mapi_guard guard;
    પૂર્ણાંક error = 0;

    /* activate callback for setting socket options */
    CURL_CBAPI_START(&guard, data, easy_fsockopt);
    error = data->set.fsockopt(data->set.sockopt_client,
                               ctx->sock, CURLSOCKTYPE_ACCEPT);
    CURL_CBAPI_END(&guard);

    જો(error)
      પાછા CURLE_ABORTED_BY_CALLBACK;
  }
  *done = TRUE;
  પાછા CURLE_OK;
}

સંરચના Curl_cftype Curl_cft_tcp_accept = {
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

CURLcode Curl_conn_tcp_listen_set(સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn,
                                  int8_t sockindex, curl_socket_t *s)
{
  CURLcode result;
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના cf_socket_ctx *ctx = ખાલી;

  /* replace any existing */
  Curl_conn_cf_discard_all(data, conn, sockindex);
  DEBUGASSERT(conn->sock[sockindex] == CURL_SOCKET_BAD);

  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  ctx->transport = TRNSPRT_TCP;
  ctx->sock = *s;
  ctx->listening = TRUE;
  ctx->accepted = FALSE;
  result = Curl_cf_create(&cf, &Curl_cft_tcp_accept, ctx);
  જો(result)
    જાઓ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

  ctx->started_at = *Curl_pgrs_now(data);
  conn->sock[sockindex] = ctx->sock;
  set_local_ip(cf, data);
  CURL_TRC_CF(data, cf, "set filter for listen socket fd=%" FMT_SOCKET_T
              " ip=%s:%d", ctx->sock,
              ctx->ip.local_ip, ctx->ip.local_port);

out:
  જો(result) {
    curlx_safefree(cf);
    curlx_safefree(ctx);
  }
  પાછા result;
}

bool Curl_conn_is_tcp_listen(સંરચના Curl_easy *data,
                             int8_t sockindex)
{
  સંરચના Curl_cfilter *cf = data->conn->cfilter[sockindex];
  જ્યારે(cf) {
    જો(cf->cft == &Curl_cft_tcp_accept)
      પાછા TRUE;
    cf = cf->next;
  }
  પાછા FALSE;
}

/**
 * Return TRUE iff `cf` is a socket filter.
 */
સ્થિર bool cf_is_socket(સંરચના Curl_cfilter *cf)
{
  પાછા cf && (cf->cft == &Curl_cft_tcp ||
                cf->cft == &Curl_cft_udp ||
                cf->cft == &Curl_cft_unix ||
                cf->cft == &Curl_cft_tcp_accept);
}

CURLcode Curl_cf_socket_peek(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             curl_socket_t *psock,
                             અચલ સંરચના Curl_sockaddr_ex **paddr,
                             સંરચના ip_quadruple *pip)
{
  (શૂન્ય)data;
  જો(cf_is_socket(cf) && cf->ctx) {
    સંરચના cf_socket_ctx *ctx = cf->ctx;

    જો(psock)
      *psock = ctx->sock;
    જો(paddr)
      *paddr = &ctx->addr;
    જો(pip)
      *pip = ctx->ip;
    પાછા CURLE_OK;
  }
  પાછા CURLE_FAILED_INIT;
}
