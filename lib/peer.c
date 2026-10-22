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
#ضمن "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ضمن <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#ضمن <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#endif

#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#ضمن <sys/un.h>
#endif

#إذا defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ضمن <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#ضمن <iphlpapi.h>
#endif

#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "protocol.h"
#ضمن "http_proxy.h"
#ضمن "idn.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/strparse.h"
#ضمن "peer.h"
#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "urlapi-int.h"
#ضمن "vtls/vtls.h"

بنية peer_parse {
  ثابت بنية Curl_scheme *scheme;
  بنية Curl_str host_user;
  بنية Curl_str host;
  بنية Curl_str zoneid;
  حرف *tmp_host_user;
  حرف *tmp_host;
  حرف *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

ساكن فراغ peer_parse_clear(بنية peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, حجم(*pp));
}

ساكن CURLcode peer_create(بنية peer_parse *pp,
                            بنية Curl_peer **ppeer)
{
  بنية Curl_peer *peer = عدم;
  CURLcode result = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  إذا(!pp || !pp->scheme)
    أرجع CURLE_FAILED_INIT;
  إذا(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    أرجع CURLE_FAILED_INIT;

  إذا((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, حجم(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  إذا(!peer) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  إذا(pp->host_user.len)
    memcpy(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  إذا(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    memcpy(peer->hostname, pp->host.str, pp->host.len);
  }

  إذا(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    memcpy(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    إذا(!peer->scopeid) {
      ثابت حرف *p = peer->zoneid;
      curl_off_t scope;
      إذا(!curlx_str_لمب(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      وإلا {
        /* Zone identifier is not fully numeric */
        unsigned صحيح idx = 0;
        idx = if_nametoindex(peer->zoneid);
        إذا(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        وإلا {
          /* Do we want to return an error here? */
        }
      }
#endif /* HAVE_IF_NAMETOINDEX */
    }
#endif /* USE_IPV6 */
  }

out:
  إذا(!result)
    *ppeer = peer;
  وإلا
    Curl_peer_unlink(&peer);
  أرجع result;
}

ساكن CURLcode peer_parse_host(بنية Curl_easy *data,
                                بنية peer_parse *pp,
                                bool scan_for_ipv6)
{
  إذا(!pp || !pp->host_user.str || !pp->host_user.len)
    أرجع CURLE_FAILED_INIT;

  إذا(pp->host_user.str[0] == '[') {
    ثابت حرف *s = pp->host_user.str + 1;
    بنية Curl_str tmp;
    إذا(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      أرجع CURLE_URL_MALFORMAT;

    إذا(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (صحيح)pp->host_user.len, pp->host_user.str);
      أرجع CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  وإلا {
#ifdef USE_IDN
    إذا(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      إذا(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        إذا(!pp->tmp_host_user)
          أرجع CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      إذا(result)
        أرجع result;
      pp->host.str = pp->tmp_host;
      pp->host.len = strlen(pp->host.str);
    }
    وإلا
#endif
    إذا(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      إذا(pp->host_user.len < MAX_IPADR_LEN) {
        حرف tmp[MAX_IPADR_LEN];
        memcpy(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      وإلا
        pp->ipv6 = TRUE;
    }
    وإلا
      pp->host = pp->host_user;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_peer_create(بنية Curl_easy *data,
                          ثابت بنية Curl_scheme *scheme,
                          ثابت حرف *hostname,
                          uint16_t port,
                          بنية Curl_peer **ppeer)
{
  بنية peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = strlen(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  إذا(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  أرجع result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ثابت بنية Curl_scheme *scheme,
                              ثابت حرف *path,
                              bool abstract_unix_socket,
                              بنية Curl_peer **ppeer)
{
  بنية peer_parse pp;
  size_t pathlen = path ? strlen(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  إذا(!scheme)
    أرجع CURLE_FAILED_INIT;
  إذا(!pathlen)
    أرجع CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  أرجع result;
}
#endif /* USE_UNIX_SOCKETS */

فراغ Curl_peer_link(بنية Curl_peer **pdest, بنية Curl_peer *src)
{
  إذا(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    إذا(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

فراغ Curl_peer_unlink(بنية Curl_peer **ppeer)
{
  إذا(*ppeer) {
    بنية Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = عدم;
    إذا(peer->refcount)
      peer->refcount--;
    إذا(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(بنية Curl_peer *p1, بنية Curl_peer *p2)
{
  أرجع (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

ساكن bool peer_same_hostname(بنية Curl_peer *p1, بنية Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  أرجع (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

ساكن bool peer_same_zoneid(ثابت حرف *z1, ثابت حرف *z2)
{
  أرجع (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(بنية Curl_peer *p1, بنية Curl_peer *p2)
{
  أرجع (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, بنية Curl_easy *data,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            بنية Curl_peer **ppeer)
{
  بنية peer_parse pp;
  حرف *zoneid = عدم, *scheme = عدم, *hostname = عدم;
  CURLUcode uc;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  إذا(uc)
    أرجع Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  إذا(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    اذهب out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  إذا(uc) {
    إذا((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    وإلا {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
  }
  وإلا إذا(strlen(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    اذهب out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = strlen(pp.host_user.str);
  إذا(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    إذا(result)
      اذهب out;
  }
  وإلا
    pp.host = pp.host_user;

  إذا(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    حرف portbuf[16];
    curl_msnprintf(portbuf, حجم(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    إذا(uc) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
    وإلا
      pp.port = port_override;
  }
  وإلا {
    uc = Curl_url_get_port(uh, &pp.port);
    إذا(uc) {
      إذا(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
      وإلا إذا(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        اذهب out;
      }
      /* no port ok when not a network scheme */
    }
  }

  إذا(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  وإلا {
    إذا(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
    إذا(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = strlen(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  إذا(result)
    failf(data, "Error %d creating peer for %s:%u",
          (صحيح)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  أرجع result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(بنية Curl_easy *data,
                                   ثابت بنية Curl_peer *dest,
                                   ثابت حرف *connect_to,
                                   بنية Curl_peer **ppeer)
{
  بنية peer_parse pp;
  ثابت حرف *portstr = عدم;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  إذا(!connect_to || !*connect_to)
    أرجع CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  إذا(connect_to[0] == '[') {
    ثابت حرف *s = strchr(connect_to + 1, ']');
    إذا(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      اذهب out;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  وإلا {
    portstr = strchr(connect_to, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_to) : strlen(connect_to);
  }

  إذا(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = strlen(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  إذا(result)
    اذهب out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  إذا(portstr && portstr[1]) {
    ثابت حرف *p = portstr + 1;
    curl_off_t portparse;
    إذا(curlx_str_لمب(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      اذهب out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  وإلا
    pp.port = dest->port;

#ifndef USE_IPV6
  إذا(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    اذهب out;
  }
#endif
  إذا(pp.ipv6 && port_switch) {
    إذا(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = strlen(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (صحيح)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (صحيح)result);
  peer_parse_clear(&pp);
  أرجع result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(بنية Curl_easy *data,
                                  ثابت حرف *scheme,
                                  uint8_t *proxytype, ثابت حرف *url)
{
  إذا(!scheme)
    أرجع CURLE_OK;

  إذا(curl_strequal("https", scheme)) {
    إذا(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  وإلا إذا(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  وإلا إذا(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  وإلا إذا(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  وإلا إذا(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  وإلا إذا(curl_strequal("http", scheme)) {
    إذا(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  وإلا {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    أرجع CURLE_COULDNT_CONNECT;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  بنية Curl_easy *data,
                                  ثابت حرف *url,
                                  uint8_t proxytype,
                                  بنية Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  بنية peer_parse pp;
  حرف *scheme = عدم;
  حرف *portptr = عدم;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  إذا(uc) {
    إذا(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    بدل(proxytype) {
    حالة CURLPROXY_HTTP:
    حالة CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      اكسر;
    حالة CURLPROXY_HTTPS:
    حالة CURLPROXY_HTTPS2:
    حالة CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      اكسر;
    حالة CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      اكسر;
    حالة CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      اكسر;
    حالة CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      اكسر;
    حالة CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      اكسر;
    افتراضي:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      اذهب out;
    }
  }
  وإلا {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    إذا(result)
      اذهب out;
  }
  DEBUGASSERT(pp.scheme);

  إذا(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(data, SSLSUPP_HTTPS_PROXY)) {
    failf(data, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    result = CURLE_NOT_BUILT_IN;
    اذهب out;
  }

  بدل(pp.scheme->family) {
  حالة CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    اكسر;
  حالة CURLPROTO_HTTP:
    اكسر;
  افتراضي:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    اذهب out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  إذا(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  إذا(portptr) {
    curl_off_t لمب;
    ثابت حرف *p = portptr;
    إذا(!curlx_str_لمب(&p, &لمب, UINT16_MAX))
      pp.port = (uint16_t)لمب;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  وإلا {
    /* No port in URL, take the set one or the scheme's default */
    إذا(data->set.proxyport)
      pp.port = data->set.proxyport;
    وإلا
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  إذا(uc) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = strlen(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  إذا(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    إذا(uc) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
    /* path will be "/", if no path was found */
    إذا(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = strlen(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    وإلا {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  إذا(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    إذا(result)
      اذهب out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  إذا(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  إذا(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = strlen(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  إذا(!result)
    DEBUGASSERT(*ppeer);
#endif
  أرجع result;
}

#endif /* !CURL_DISABLE_PROXY */
