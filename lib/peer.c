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
/*
 * IDN conversions
 */
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#അന്തര്ഭാവ <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#അന്തര്ഭാവ <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#അന്തര്ഭാവ <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#അന്തര്ഭാവ <sys/param.h>
#endif

#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#അന്തര്ഭാവ <sys/un.h>
#endif

#എങ്കിൽ defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#എങ്കിൽ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#അന്തര്ഭാവ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#അന്തര്ഭാവ <iphlpapi.h>
#endif

#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "protocol.h"
#അന്തര്ഭാവ "http_proxy.h"
#അന്തര്ഭാവ "idn.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "peer.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "urlapi-int.h"
#അന്തര്ഭാവ "vtls/vtls.h"

ഘടന peer_parse {
  സ്ഥിരം ഘടന Curl_scheme *scheme;
  ഘടന Curl_str host_user;
  ഘടന Curl_str host;
  ഘടന Curl_str zoneid;
  അക്ഷരം *tmp_host_user;
  അക്ഷരം *tmp_host;
  അക്ഷരം *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

സ്ഥിതം ശൂന്യം peer_parse_clear(ഘടന peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, വലുപ്പം(*pp));
}

സ്ഥിതം CURLcode peer_create(ഘടന peer_parse *pp,
                            ഘടന Curl_peer **ppeer)
{
  ഘടന Curl_peer *peer = ശൂന്യ;
  CURLcode result = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  എങ്കിൽ(!pp || !pp->scheme)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, വലുപ്പം(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  എങ്കിൽ(!peer) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  എങ്കിൽ(pp->host_user.len)
    memcpy(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  എങ്കിൽ(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    memcpy(peer->hostname, pp->host.str, pp->host.len);
  }

  എങ്കിൽ(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    memcpy(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    എങ്കിൽ(!peer->scopeid) {
      സ്ഥിരം അക്ഷരം *p = peer->zoneid;
      curl_off_t scope;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      അല്ലെങ്കിൽ {
        /* Zone identifier is not fully numeric */
        unsigned പൂർണ്ണസംഖ്യ idx = 0;
        idx = if_nametoindex(peer->zoneid);
        എങ്കിൽ(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        അല്ലെങ്കിൽ {
          /* Do we want to return an error here? */
        }
      }
#endif /* HAVE_IF_NAMETOINDEX */
    }
#endif /* USE_IPV6 */
  }

out:
  എങ്കിൽ(!result)
    *ppeer = peer;
  അല്ലെങ്കിൽ
    Curl_peer_unlink(&peer);
  തിരികെ result;
}

സ്ഥിതം CURLcode peer_parse_host(ഘടന Curl_easy *data,
                                ഘടന peer_parse *pp,
                                bool scan_for_ipv6)
{
  എങ്കിൽ(!pp || !pp->host_user.str || !pp->host_user.len)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(pp->host_user.str[0] == '[') {
    സ്ഥിരം അക്ഷരം *s = pp->host_user.str + 1;
    ഘടന Curl_str tmp;
    എങ്കിൽ(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      തിരികെ CURLE_URL_MALFORMAT;

    എങ്കിൽ(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (പൂർണ്ണസംഖ്യ)pp->host_user.len, pp->host_user.str);
      തിരികെ CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  അല്ലെങ്കിൽ {
#ifdef USE_IDN
    എങ്കിൽ(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      എങ്കിൽ(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        എങ്കിൽ(!pp->tmp_host_user)
          തിരികെ CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      എങ്കിൽ(result)
        തിരികെ result;
      pp->host.str = pp->tmp_host;
      pp->host.len = strlen(pp->host.str);
    }
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      എങ്കിൽ(pp->host_user.len < MAX_IPADR_LEN) {
        അക്ഷരം tmp[MAX_IPADR_LEN];
        memcpy(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      അല്ലെങ്കിൽ
        pp->ipv6 = TRUE;
    }
    അല്ലെങ്കിൽ
      pp->host = pp->host_user;
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_peer_create(ഘടന Curl_easy *data,
                          സ്ഥിരം ഘടന Curl_scheme *scheme,
                          സ്ഥിരം അക്ഷരം *hostname,
                          uint16_t port,
                          ഘടന Curl_peer **ppeer)
{
  ഘടന peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, വലുപ്പം(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = strlen(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  എങ്കിൽ(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  തിരികെ result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(സ്ഥിരം ഘടന Curl_scheme *scheme,
                              സ്ഥിരം അക്ഷരം *path,
                              bool abstract_unix_socket,
                              ഘടന Curl_peer **ppeer)
{
  ഘടന peer_parse pp;
  size_t pathlen = path ? strlen(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, വലുപ്പം(pp));
  എങ്കിൽ(!scheme)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ(!pathlen)
    തിരികെ CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  തിരികെ result;
}
#endif /* USE_UNIX_SOCKETS */

ശൂന്യം Curl_peer_link(ഘടന Curl_peer **pdest, ഘടന Curl_peer *src)
{
  എങ്കിൽ(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    എങ്കിൽ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ശൂന്യം Curl_peer_unlink(ഘടന Curl_peer **ppeer)
{
  എങ്കിൽ(*ppeer) {
    ഘടന Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = ശൂന്യ;
    എങ്കിൽ(peer->refcount)
      peer->refcount--;
    എങ്കിൽ(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(ഘടന Curl_peer *p1, ഘടന Curl_peer *p2)
{
  തിരികെ (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

സ്ഥിതം bool peer_same_hostname(ഘടന Curl_peer *p1, ഘടന Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  തിരികെ (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

സ്ഥിതം bool peer_same_zoneid(സ്ഥിരം അക്ഷരം *z1, സ്ഥിരം അക്ഷരം *z2)
{
  തിരികെ (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(ഘടന Curl_peer *p1, ഘടന Curl_peer *p2)
{
  തിരികെ (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, ഘടന Curl_easy *data,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            ഘടന Curl_peer **ppeer)
{
  ഘടന peer_parse pp;
  അക്ഷരം *zoneid = ശൂന്യ, *scheme = ശൂന്യ, *hostname = ശൂന്യ;
  CURLUcode uc;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, വലുപ്പം(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  എങ്കിൽ(uc)
    തിരികെ Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  എങ്കിൽ(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    പോകുക out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  എങ്കിൽ(uc) {
    എങ്കിൽ((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    അല്ലെങ്കിൽ {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(strlen(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    പോകുക out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = strlen(pp.host_user.str);
  എങ്കിൽ(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    എങ്കിൽ(result)
      പോകുക out;
  }
  അല്ലെങ്കിൽ
    pp.host = pp.host_user;

  എങ്കിൽ(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    അക്ഷരം portbuf[16];
    curl_msnprintf(portbuf, വലുപ്പം(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    എങ്കിൽ(uc) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
    അല്ലെങ്കിൽ
      pp.port = port_override;
  }
  അല്ലെങ്കിൽ {
    uc = Curl_url_get_port(uh, &pp.port);
    എങ്കിൽ(uc) {
      എങ്കിൽ(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        പോകുക out;
      }
      /* no port ok when not a network scheme */
    }
  }

  എങ്കിൽ(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  അല്ലെങ്കിൽ {
    എങ്കിൽ(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
    എങ്കിൽ(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = strlen(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  എങ്കിൽ(result)
    failf(data, "Error %d creating peer for %s:%u",
          (പൂർണ്ണസംഖ്യ)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  തിരികെ result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(ഘടന Curl_easy *data,
                                   സ്ഥിരം ഘടന Curl_peer *dest,
                                   സ്ഥിരം അക്ഷരം *connect_to,
                                   ഘടന Curl_peer **ppeer)
{
  ഘടന peer_parse pp;
  സ്ഥിരം അക്ഷരം *portstr = ശൂന്യ;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, വലുപ്പം(pp));
  എങ്കിൽ(!connect_to || !*connect_to)
    തിരികെ CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  എങ്കിൽ(connect_to[0] == '[') {
    സ്ഥിരം അക്ഷരം *s = strchr(connect_to + 1, ']');
    എങ്കിൽ(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      പോകുക out;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  അല്ലെങ്കിൽ {
    portstr = strchr(connect_to, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_to) : strlen(connect_to);
  }

  എങ്കിൽ(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = strlen(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  എങ്കിൽ(result)
    പോകുക out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  എങ്കിൽ(portstr && portstr[1]) {
    സ്ഥിരം അക്ഷരം *p = portstr + 1;
    curl_off_t portparse;
    എങ്കിൽ(curlx_str_സംഖ്യ(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      പോകുക out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  അല്ലെങ്കിൽ
    pp.port = dest->port;

#ifndef USE_IPV6
  എങ്കിൽ(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    പോകുക out;
  }
#endif
  എങ്കിൽ(pp.ipv6 && port_switch) {
    എങ്കിൽ(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = strlen(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (പൂർണ്ണസംഖ്യ)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (പൂർണ്ണസംഖ്യ)result);
  peer_parse_clear(&pp);
  തിരികെ result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(ഘടന Curl_easy *data,
                                  സ്ഥിരം അക്ഷരം *scheme,
                                  uint8_t *proxytype, സ്ഥിരം അക്ഷരം *url)
{
  എങ്കിൽ(!scheme)
    തിരികെ CURLE_OK;

  എങ്കിൽ(curl_strequal("https", scheme)) {
    എങ്കിൽ(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal("http", scheme)) {
    എങ്കിൽ(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  അല്ലെങ്കിൽ {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    തിരികെ CURLE_COULDNT_CONNECT;
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  ഘടന Curl_easy *data,
                                  സ്ഥിരം അക്ഷരം *url,
                                  uint8_t proxytype,
                                  ഘടന Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  ഘടന peer_parse pp;
  അക്ഷരം *scheme = ശൂന്യ;
  അക്ഷരം *portptr = ശൂന്യ;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, വലുപ്പം(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  എങ്കിൽ(uc) {
    എങ്കിൽ(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    തിരഞ്ഞെടുപ്പ്(proxytype) {
    അവസ്ഥ CURLPROXY_HTTP:
    അവസ്ഥ CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      നിർത്തുക;
    അവസ്ഥ CURLPROXY_HTTPS:
    അവസ്ഥ CURLPROXY_HTTPS2:
    അവസ്ഥ CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      നിർത്തുക;
    അവസ്ഥ CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      നിർത്തുക;
    അവസ്ഥ CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      നിർത്തുക;
    അവസ്ഥ CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      നിർത്തുക;
    അവസ്ഥ CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      പോകുക out;
    }
  }
  അല്ലെങ്കിൽ {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    എങ്കിൽ(result)
      പോകുക out;
  }
  DEBUGASSERT(pp.scheme);

  എങ്കിൽ(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(data, SSLSUPP_HTTPS_PROXY)) {
    failf(data, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    result = CURLE_NOT_BUILT_IN;
    പോകുക out;
  }

  തിരഞ്ഞെടുപ്പ്(pp.scheme->family) {
  അവസ്ഥ CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    നിർത്തുക;
  അവസ്ഥ CURLPROTO_HTTP:
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    പോകുക out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  എങ്കിൽ(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  എങ്കിൽ(portptr) {
    curl_off_t അക്കം;
    സ്ഥിരം അക്ഷരം *p = portptr;
    എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &അക്കം, UINT16_MAX))
      pp.port = (uint16_t)അക്കം;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  അല്ലെങ്കിൽ {
    /* No port in URL, take the set one or the scheme's default */
    എങ്കിൽ(data->set.proxyport)
      pp.port = data->set.proxyport;
    അല്ലെങ്കിൽ
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  എങ്കിൽ(uc) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = strlen(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  എങ്കിൽ(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    എങ്കിൽ(uc) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
    /* path will be "/", if no path was found */
    എങ്കിൽ(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = strlen(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    അല്ലെങ്കിൽ {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  എങ്കിൽ(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    എങ്കിൽ(result)
      പോകുക out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  എങ്കിൽ(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  എങ്കിൽ(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = strlen(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  എങ്കിൽ(!result)
    DEBUGASSERT(*ppeer);
#endif
  തിരികെ result;
}

#endif /* !CURL_DISABLE_PROXY */
