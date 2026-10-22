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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#அந்தர்பாவ <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#அந்தர்பாவ <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#அந்தர்பாவ <sys/param.h>
#endif

#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#அந்தர்பாவ <sys/un.h>
#endif

#எனில் defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#எனில் defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#அந்தர்பாவ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#அந்தர்பாவ <iphlpapi.h>
#endif

#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "protocol.h"
#அந்தர்பாவ "http_proxy.h"
#அந்தர்பாவ "idn.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "peer.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "urlapi-int.h"
#அந்தர்பாவ "vtls/vtls.h"

கட்டமைப்பு peer_parse {
  மாறிலி கட்டமைப்பு Curl_scheme *scheme;
  கட்டமைப்பு Curl_str host_user;
  கட்டமைப்பு Curl_str host;
  கட்டமைப்பு Curl_str zoneid;
  எழுத்து *tmp_host_user;
  எழுத்து *tmp_host;
  எழுத்து *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

நிலையான வெறுமை peer_parse_clear(கட்டமைப்பு peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, அளவு(*pp));
}

நிலையான CURLcode peer_create(கட்டமைப்பு peer_parse *pp,
                            கட்டமைப்பு Curl_peer **ppeer)
{
  கட்டமைப்பு Curl_peer *peer = வெற்று;
  CURLcode result = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  எனில்(!pp || !pp->scheme)
    திரும்பு CURLE_FAILED_INIT;
  எனில்(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    திரும்பு CURLE_FAILED_INIT;

  எனில்((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, அளவு(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  எனில்(!peer) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  எனில்(pp->host_user.len)
    memcpy(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  எனில்(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    memcpy(peer->hostname, pp->host.str, pp->host.len);
  }

  எனில்(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    memcpy(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    எனில்(!peer->scopeid) {
      மாறிலி எழுத்து *p = peer->zoneid;
      curl_off_t scope;
      எனில்(!curlx_str_எண்(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      இல்லையெனில் {
        /* Zone identifier is not fully numeric */
        unsigned முழு idx = 0;
        idx = if_nametoindex(peer->zoneid);
        எனில்(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        இல்லையெனில் {
          /* Do we want to return an error here? */
        }
      }
#endif /* HAVE_IF_NAMETOINDEX */
    }
#endif /* USE_IPV6 */
  }

out:
  எனில்(!result)
    *ppeer = peer;
  இல்லையெனில்
    Curl_peer_unlink(&peer);
  திரும்பு result;
}

நிலையான CURLcode peer_parse_host(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு peer_parse *pp,
                                bool scan_for_ipv6)
{
  எனில்(!pp || !pp->host_user.str || !pp->host_user.len)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(pp->host_user.str[0] == '[') {
    மாறிலி எழுத்து *s = pp->host_user.str + 1;
    கட்டமைப்பு Curl_str tmp;
    எனில்(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      திரும்பு CURLE_URL_MALFORMAT;

    எனில்(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (முழு)pp->host_user.len, pp->host_user.str);
      திரும்பு CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  இல்லையெனில் {
#ifdef USE_IDN
    எனில்(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      எனில்(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        எனில்(!pp->tmp_host_user)
          திரும்பு CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      எனில்(result)
        திரும்பு result;
      pp->host.str = pp->tmp_host;
      pp->host.len = strlen(pp->host.str);
    }
    இல்லையெனில்
#endif
    எனில்(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      எனில்(pp->host_user.len < MAX_IPADR_LEN) {
        எழுத்து tmp[MAX_IPADR_LEN];
        memcpy(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      இல்லையெனில்
        pp->ipv6 = TRUE;
    }
    இல்லையெனில்
      pp->host = pp->host_user;
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_peer_create(கட்டமைப்பு Curl_easy *data,
                          மாறிலி கட்டமைப்பு Curl_scheme *scheme,
                          மாறிலி எழுத்து *hostname,
                          uint16_t port,
                          கட்டமைப்பு Curl_peer **ppeer)
{
  கட்டமைப்பு peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, அளவு(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = strlen(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  எனில்(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  திரும்பு result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(மாறிலி கட்டமைப்பு Curl_scheme *scheme,
                              மாறிலி எழுத்து *path,
                              bool abstract_unix_socket,
                              கட்டமைப்பு Curl_peer **ppeer)
{
  கட்டமைப்பு peer_parse pp;
  size_t pathlen = path ? strlen(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, அளவு(pp));
  எனில்(!scheme)
    திரும்பு CURLE_FAILED_INIT;
  எனில்(!pathlen)
    திரும்பு CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  திரும்பு result;
}
#endif /* USE_UNIX_SOCKETS */

வெறுமை Curl_peer_link(கட்டமைப்பு Curl_peer **pdest, கட்டமைப்பு Curl_peer *src)
{
  எனில்(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    எனில்(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

வெறுமை Curl_peer_unlink(கட்டமைப்பு Curl_peer **ppeer)
{
  எனில்(*ppeer) {
    கட்டமைப்பு Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = வெற்று;
    எனில்(peer->refcount)
      peer->refcount--;
    எனில்(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(கட்டமைப்பு Curl_peer *p1, கட்டமைப்பு Curl_peer *p2)
{
  திரும்பு (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

நிலையான bool peer_same_hostname(கட்டமைப்பு Curl_peer *p1, கட்டமைப்பு Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  திரும்பு (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

நிலையான bool peer_same_zoneid(மாறிலி எழுத்து *z1, மாறிலி எழுத்து *z2)
{
  திரும்பு (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(கட்டமைப்பு Curl_peer *p1, கட்டமைப்பு Curl_peer *p2)
{
  திரும்பு (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, கட்டமைப்பு Curl_easy *data,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            கட்டமைப்பு Curl_peer **ppeer)
{
  கட்டமைப்பு peer_parse pp;
  எழுத்து *zoneid = வெற்று, *scheme = வெற்று, *hostname = வெற்று;
  CURLUcode uc;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, அளவு(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  எனில்(uc)
    திரும்பு Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  எனில்(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    செல் out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  எனில்(uc) {
    எனில்((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    இல்லையெனில் {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }
  இல்லையெனில் எனில்(strlen(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    செல் out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = strlen(pp.host_user.str);
  எனில்(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    எனில்(result)
      செல் out;
  }
  இல்லையெனில்
    pp.host = pp.host_user;

  எனில்(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    எழுத்து portbuf[16];
    curl_msnprintf(portbuf, அளவு(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    எனில்(uc) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
    இல்லையெனில்
      pp.port = port_override;
  }
  இல்லையெனில் {
    uc = Curl_url_get_port(uh, &pp.port);
    எனில்(uc) {
      எனில்(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
      இல்லையெனில் எனில்(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        செல் out;
      }
      /* no port ok when not a network scheme */
    }
  }

  எனில்(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  இல்லையெனில் {
    எனில்(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
    எனில்(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = strlen(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  எனில்(result)
    failf(data, "Error %d creating peer for %s:%u",
          (முழு)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  திரும்பு result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(கட்டமைப்பு Curl_easy *data,
                                   மாறிலி கட்டமைப்பு Curl_peer *dest,
                                   மாறிலி எழுத்து *connect_to,
                                   கட்டமைப்பு Curl_peer **ppeer)
{
  கட்டமைப்பு peer_parse pp;
  மாறிலி எழுத்து *portstr = வெற்று;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, அளவு(pp));
  எனில்(!connect_to || !*connect_to)
    திரும்பு CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  எனில்(connect_to[0] == '[') {
    மாறிலி எழுத்து *s = strchr(connect_to + 1, ']');
    எனில்(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      செல் out;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  இல்லையெனில் {
    portstr = strchr(connect_to, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_to) : strlen(connect_to);
  }

  எனில்(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = strlen(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  எனில்(result)
    செல் out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  எனில்(portstr && portstr[1]) {
    மாறிலி எழுத்து *p = portstr + 1;
    curl_off_t portparse;
    எனில்(curlx_str_எண்(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      செல் out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  இல்லையெனில்
    pp.port = dest->port;

#ifndef USE_IPV6
  எனில்(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    செல் out;
  }
#endif
  எனில்(pp.ipv6 && port_switch) {
    எனில்(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = strlen(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (முழு)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (முழு)result);
  peer_parse_clear(&pp);
  திரும்பு result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி எழுத்து *scheme,
                                  uint8_t *proxytype, மாறிலி எழுத்து *url)
{
  எனில்(!scheme)
    திரும்பு CURLE_OK;

  எனில்(curl_strequal("https", scheme)) {
    எனில்(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  இல்லையெனில் எனில்(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  இல்லையெனில் எனில்(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  இல்லையெனில் எனில்(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  இல்லையெனில் எனில்(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  இல்லையெனில் எனில்(curl_strequal("http", scheme)) {
    எனில்(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  இல்லையெனில் {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    திரும்பு CURLE_COULDNT_CONNECT;
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  கட்டமைப்பு Curl_easy *data,
                                  மாறிலி எழுத்து *url,
                                  uint8_t proxytype,
                                  கட்டமைப்பு Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  கட்டமைப்பு peer_parse pp;
  எழுத்து *scheme = வெற்று;
  எழுத்து *portptr = வெற்று;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, அளவு(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  எனில்(uc) {
    எனில்(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    தேர்வு(proxytype) {
    நிலை CURLPROXY_HTTP:
    நிலை CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      நிறுத்து;
    நிலை CURLPROXY_HTTPS:
    நிலை CURLPROXY_HTTPS2:
    நிலை CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      நிறுத்து;
    நிலை CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      நிறுத்து;
    நிலை CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      நிறுத்து;
    நிலை CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      நிறுத்து;
    நிலை CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      நிறுத்து;
    இயல்பு:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      செல் out;
    }
  }
  இல்லையெனில் {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    எனில்(result)
      செல் out;
  }
  DEBUGASSERT(pp.scheme);

  எனில்(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(data, SSLSUPP_HTTPS_PROXY)) {
    failf(data, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    result = CURLE_NOT_BUILT_IN;
    செல் out;
  }

  தேர்வு(pp.scheme->family) {
  நிலை CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    நிறுத்து;
  நிலை CURLPROTO_HTTP:
    நிறுத்து;
  இயல்பு:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    செல் out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  எனில்(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  எனில்(portptr) {
    curl_off_t இலக்கம்;
    மாறிலி எழுத்து *p = portptr;
    எனில்(!curlx_str_எண்(&p, &இலக்கம், UINT16_MAX))
      pp.port = (uint16_t)இலக்கம்;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  இல்லையெனில் {
    /* No port in URL, take the set one or the scheme's default */
    எனில்(data->set.proxyport)
      pp.port = data->set.proxyport;
    இல்லையெனில்
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  எனில்(uc) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = strlen(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  எனில்(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    எனில்(uc) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
    /* path will be "/", if no path was found */
    எனில்(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = strlen(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    இல்லையெனில் {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  எனில்(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    எனில்(result)
      செல் out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  எனில்(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  எனில்(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = strlen(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  எனில்(!result)
    DEBUGASSERT(*ppeer);
#endif
  திரும்பு result;
}

#endif /* !CURL_DISABLE_PROXY */
