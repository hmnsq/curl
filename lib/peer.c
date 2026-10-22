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
#include "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#include <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif

#ပေႃး defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#ပေႃး defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#include <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#include <iphlpapi.h>
#endif

#include "curl_addrinfo.h"
#include "curl_trc.h"
#include "protocol.h"
#include "http_proxy.h"
#include "idn.h"
#include "curlx/strdup.h"
#include "curlx/strparse.h"
#include "peer.h"
#include "urldata.h"
#include "url.h"
#include "urlapi-int.h"
#include "vtls/vtls.h"

sa_nracanaa peer_parse {
  const sa_nracanaa Curl_scheme *scheme;
  sa_nracanaa Curl_str host_user;
  sa_nracanaa Curl_str host;
  sa_nracanaa Curl_str zoneid;
  var_na *tmp_host_user;
  var_na *tmp_host;
  var_na *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

static void peer_parse_clear(sa_nracanaa peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, parimaa_na(*pp));
}

static CURLcode peer_create(sa_nracanaa peer_parse *pp,
                            sa_nracanaa Curl_peer **ppeer)
{
  sa_nracanaa Curl_peer *peer = NULL;
  CURLcode result = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  ပေႃး(!pp || !pp->scheme)
    ပွၵ်ႈ CURLE_FAILED_INIT;
  ပေႃး(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    ပွၵ်ႈ CURLE_FAILED_INIT;

  ပေႃး((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, parimaa_na(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  ပေႃး(!peer) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  ပေႃး(pp->host_user.len)
    memcpy(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  ပေႃး(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    memcpy(peer->hostname, pp->host.str, pp->host.len);
  }

  ပေႃး(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    memcpy(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    ပေႃး(!peer->scopeid) {
      const var_na *p = peer->zoneid;
      curl_off_t scope;
      ပေႃး(!curlx_str_number(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      ပေႃးဢမ်ႇ {
        /* Zone identifier is not fully numeric */
        unsigned puur_naa_nka idx = 0;
        idx = if_nametoindex(peer->zoneid);
        ပေႃး(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        ပေႃးဢမ်ႇ {
          /* Do we want to return an error here? */
        }
      }
#endif /* HAVE_IF_NAMETOINDEX */
    }
#endif /* USE_IPV6 */
  }

out:
  ပေႃး(!result)
    *ppeer = peer;
  ပေႃးဢမ်ႇ
    Curl_peer_unlink(&peer);
  ပွၵ်ႈ result;
}

static CURLcode peer_parse_host(sa_nracanaa Curl_easy *data,
                                sa_nracanaa peer_parse *pp,
                                bool scan_for_ipv6)
{
  ပေႃး(!pp || !pp->host_user.str || !pp->host_user.len)
    ပွၵ်ႈ CURLE_FAILED_INIT;

  ပေႃး(pp->host_user.str[0] == '[') {
    const var_na *s = pp->host_user.str + 1;
    sa_nracanaa Curl_str tmp;
    ပေႃး(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      ပွၵ်ႈ CURLE_URL_MALFORMAT;

    ပေႃး(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (puur_naa_nka)pp->host_user.len, pp->host_user.str);
      ပွၵ်ႈ CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  ပေႃးဢမ်ႇ {
#ifdef USE_IDN
    ပေႃး(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      ပေႃး(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        ပေႃး(!pp->tmp_host_user)
          ပွၵ်ႈ CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      ပေႃး(result)
        ပွၵ်ႈ result;
      pp->host.str = pp->tmp_host;
      pp->host.len = strlen(pp->host.str);
    }
    ပေႃးဢမ်ႇ
#endif
    ပေႃး(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      ပေႃး(pp->host_user.len < MAX_IPADR_LEN) {
        var_na tmp[MAX_IPADR_LEN];
        memcpy(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      ပေႃးဢမ်ႇ
        pp->ipv6 = TRUE;
    }
    ပေႃးဢမ်ႇ
      pp->host = pp->host_user;
  }
  ပွၵ်ႈ CURLE_OK;
}

CURLcode Curl_peer_create(sa_nracanaa Curl_easy *data,
                          const sa_nracanaa Curl_scheme *scheme,
                          const var_na *hostname,
                          uint16_t port,
                          sa_nracanaa Curl_peer **ppeer)
{
  sa_nracanaa peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, parimaa_na(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = strlen(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  ပေႃး(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  ပွၵ်ႈ result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(const sa_nracanaa Curl_scheme *scheme,
                              const var_na *path,
                              bool abstract_unix_socket,
                              sa_nracanaa Curl_peer **ppeer)
{
  sa_nracanaa peer_parse pp;
  size_t pathlen = path ? strlen(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, parimaa_na(pp));
  ပေႃး(!scheme)
    ပွၵ်ႈ CURLE_FAILED_INIT;
  ပေႃး(!pathlen)
    ပွၵ်ႈ CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  ပွၵ်ႈ result;
}
#endif /* USE_UNIX_SOCKETS */

void Curl_peer_link(sa_nracanaa Curl_peer **pdest, sa_nracanaa Curl_peer *src)
{
  ပေႃး(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    ပေႃး(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

void Curl_peer_unlink(sa_nracanaa Curl_peer **ppeer)
{
  ပေႃး(*ppeer) {
    sa_nracanaa Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = NULL;
    ပေႃး(peer->refcount)
      peer->refcount--;
    ပေႃး(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(sa_nracanaa Curl_peer *p1, sa_nracanaa Curl_peer *p2)
{
  ပွၵ်ႈ (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

static bool peer_same_hostname(sa_nracanaa Curl_peer *p1, sa_nracanaa Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  ပွၵ်ႈ (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

static bool peer_same_zoneid(const var_na *z1, const var_na *z2)
{
  ပွၵ်ႈ (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(sa_nracanaa Curl_peer *p1, sa_nracanaa Curl_peer *p2)
{
  ပွၵ်ႈ (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, sa_nracanaa Curl_easy *data,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            sa_nracanaa Curl_peer **ppeer)
{
  sa_nracanaa peer_parse pp;
  var_na *zoneid = NULL, *scheme = NULL, *hostname = NULL;
  CURLUcode uc;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, parimaa_na(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  ပေႃး(uc)
    ပွၵ်ႈ Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  ပေႃး(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    goto out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  ပေႃး(uc) {
    ပေႃး((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    ပေႃးဢမ်ႇ {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }
  ပေႃးဢမ်ႇ ပေႃး(strlen(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    goto out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = strlen(pp.host_user.str);
  ပေႃး(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    ပေႃး(result)
      goto out;
  }
  ပေႃးဢမ်ႇ
    pp.host = pp.host_user;

  ပေႃး(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    var_na portbuf[16];
    curl_msnprintf(portbuf, parimaa_na(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    ပေႃး(uc) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
    ပေႃးဢမ်ႇ
      pp.port = port_override;
  }
  ပေႃးဢမ်ႇ {
    uc = Curl_url_get_port(uh, &pp.port);
    ပေႃး(uc) {
      ပေႃး(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
      ပေႃးဢမ်ႇ ပေႃး(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        goto out;
      }
      /* no port ok when not a network scheme */
    }
  }

  ပေႃး(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  ပေႃးဢမ်ႇ {
    ပေႃး(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
    ပေႃး(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = strlen(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  ပေႃး(result)
    failf(data, "Error %d creating peer for %s:%u",
          (puur_naa_nka)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  ပွၵ်ႈ result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(sa_nracanaa Curl_easy *data,
                                   const sa_nracanaa Curl_peer *dest,
                                   const var_na *connect_to,
                                   sa_nracanaa Curl_peer **ppeer)
{
  sa_nracanaa peer_parse pp;
  const var_na *portstr = NULL;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, parimaa_na(pp));
  ပေႃး(!connect_to || !*connect_to)
    ပွၵ်ႈ CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  ပေႃး(connect_to[0] == '[') {
    const var_na *s = strchr(connect_to + 1, ']');
    ပေႃး(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      goto out;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  ပေႃးဢမ်ႇ {
    portstr = strchr(connect_to, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_to) : strlen(connect_to);
  }

  ပေႃး(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = strlen(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  ပေႃး(result)
    goto out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  ပေႃး(portstr && portstr[1]) {
    const var_na *p = portstr + 1;
    curl_off_t portparse;
    ပေႃး(curlx_str_number(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      goto out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  ပေႃးဢမ်ႇ
    pp.port = dest->port;

#ifndef USE_IPV6
  ပေႃး(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    goto out;
  }
#endif
  ပေႃး(pp.ipv6 && port_switch) {
    ပေႃး(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = strlen(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (puur_naa_nka)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (puur_naa_nka)result);
  peer_parse_clear(&pp);
  ပွၵ်ႈ result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(sa_nracanaa Curl_easy *data,
                                  const var_na *scheme,
                                  uint8_t *proxytype, const var_na *url)
{
  ပေႃး(!scheme)
    ပွၵ်ႈ CURLE_OK;

  ပေႃး(curl_strequal("https", scheme)) {
    ပေႃး(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  ပေႃးဢမ်ႇ ပေႃး(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  ပေႃးဢမ်ႇ ပေႃး(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  ပေႃးဢမ်ႇ ပေႃး(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  ပေႃးဢမ်ႇ ပေႃး(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  ပေႃးဢမ်ႇ ပေႃး(curl_strequal("http", scheme)) {
    ပေႃး(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  ပေႃးဢမ်ႇ {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    ပွၵ်ႈ CURLE_COULDNT_CONNECT;
  }
  ပွၵ်ႈ CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  sa_nracanaa Curl_easy *data,
                                  const var_na *url,
                                  uint8_t proxytype,
                                  sa_nracanaa Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  sa_nracanaa peer_parse pp;
  var_na *scheme = NULL;
  var_na *portptr = NULL;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, parimaa_na(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  ပေႃး(uc) {
    ပေႃး(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    switch(proxytype) {
    pak_sa CURLPROXY_HTTP:
    pak_sa CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      bha_nga;
    pak_sa CURLPROXY_HTTPS:
    pak_sa CURLPROXY_HTTPS2:
    pak_sa CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      bha_nga;
    pak_sa CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      bha_nga;
    pak_sa CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      bha_nga;
    pak_sa CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      bha_nga;
    pak_sa CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      bha_nga;
    default:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      goto out;
    }
  }
  ပေႃးဢမ်ႇ {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    ပေႃး(result)
      goto out;
  }
  DEBUGASSERT(pp.scheme);

  ပေႃး(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(data, SSLSUPP_HTTPS_PROXY)) {
    failf(data, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    result = CURLE_NOT_BUILT_IN;
    goto out;
  }

  switch(pp.scheme->family) {
  pak_sa CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    bha_nga;
  pak_sa CURLPROTO_HTTP:
    bha_nga;
  default:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    goto out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  ပေႃး(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  ပေႃး(portptr) {
    curl_off_t num;
    const var_na *p = portptr;
    ပေႃး(!curlx_str_number(&p, &num, UINT16_MAX))
      pp.port = (uint16_t)num;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  ပေႃးဢမ်ႇ {
    /* No port in URL, take the set one or the scheme's default */
    ပေႃး(data->set.proxyport)
      pp.port = data->set.proxyport;
    ပေႃးဢမ်ႇ
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  ပေႃး(uc) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = strlen(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  ပေႃး(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    ပေႃး(uc) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
    /* path will be "/", if no path was found */
    ပေႃး(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = strlen(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    ပေႃးဢမ်ႇ {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  ပေႃး(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    ပေႃး(result)
      goto out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  ပေႃး(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  ပေႃး(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = strlen(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  ပေႃး(!result)
    DEBUGASSERT(*ppeer);
#endif
  ပွၵ်ႈ result;
}

#endif /* !CURL_DISABLE_PROXY */
