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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#અન્તર્ભાવ <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#અન્તર્ભાવ <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#અન્તર્ભાવ <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#અન્તર્ભાવ <sys/param.h>
#endif

#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#અન્તર્ભાવ <sys/un.h>
#endif

#જો defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#જો defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#અન્તર્ભાવ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#અન્તર્ભાવ <iphlpapi.h>
#endif

#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "protocol.h"
#અન્તર્ભાવ "http_proxy.h"
#અન્તર્ભાવ "idn.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "peer.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "urlapi-int.h"
#અન્તર્ભાવ "vtls/vtls.h"

સંરચના peer_parse {
  અચલ સંરચના Curl_scheme *scheme;
  સંરચના Curl_str host_user;
  સંરચના Curl_str host;
  સંરચના Curl_str zoneid;
  અક્ષર *tmp_host_user;
  અક્ષર *tmp_host;
  અક્ષર *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

સ્થિર શૂન્ય peer_parse_clear(સંરચના peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, કદ(*pp));
}

સ્થિર CURLcode peer_create(સંરચના peer_parse *pp,
                            સંરચના Curl_peer **ppeer)
{
  સંરચના Curl_peer *peer = ખાલી;
  CURLcode result = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  જો(!pp || !pp->scheme)
    પાછા CURLE_FAILED_INIT;
  જો(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    પાછા CURLE_FAILED_INIT;

  જો((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, કદ(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  જો(!peer) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  જો(pp->host_user.len)
    memcpy(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  જો(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    memcpy(peer->hostname, pp->host.str, pp->host.len);
  }

  જો(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    memcpy(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    જો(!peer->scopeid) {
      અચલ અક્ષર *p = peer->zoneid;
      curl_off_t scope;
      જો(!curlx_str_સંખ્યા(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      નહીંતર {
        /* Zone identifier is not fully numeric */
        unsigned પૂર્ણાંક idx = 0;
        idx = if_nametoindex(peer->zoneid);
        જો(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        નહીંતર {
          /* Do we want to return an error here? */
        }
      }
#endif /* HAVE_IF_NAMETOINDEX */
    }
#endif /* USE_IPV6 */
  }

out:
  જો(!result)
    *ppeer = peer;
  નહીંતર
    Curl_peer_unlink(&peer);
  પાછા result;
}

સ્થિર CURLcode peer_parse_host(સંરચના Curl_easy *data,
                                સંરચના peer_parse *pp,
                                bool scan_for_ipv6)
{
  જો(!pp || !pp->host_user.str || !pp->host_user.len)
    પાછા CURLE_FAILED_INIT;

  જો(pp->host_user.str[0] == '[') {
    અચલ અક્ષર *s = pp->host_user.str + 1;
    સંરચના Curl_str tmp;
    જો(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      પાછા CURLE_URL_MALFORMAT;

    જો(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (પૂર્ણાંક)pp->host_user.len, pp->host_user.str);
      પાછા CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  નહીંતર {
#ifdef USE_IDN
    જો(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      જો(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        જો(!pp->tmp_host_user)
          પાછા CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      જો(result)
        પાછા result;
      pp->host.str = pp->tmp_host;
      pp->host.len = strlen(pp->host.str);
    }
    નહીંતર
#endif
    જો(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      જો(pp->host_user.len < MAX_IPADR_LEN) {
        અક્ષર tmp[MAX_IPADR_LEN];
        memcpy(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      નહીંતર
        pp->ipv6 = TRUE;
    }
    નહીંતર
      pp->host = pp->host_user;
  }
  પાછા CURLE_OK;
}

CURLcode Curl_peer_create(સંરચના Curl_easy *data,
                          અચલ સંરચના Curl_scheme *scheme,
                          અચલ અક્ષર *hostname,
                          uint16_t port,
                          સંરચના Curl_peer **ppeer)
{
  સંરચના peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, કદ(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = strlen(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  જો(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  પાછા result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(અચલ સંરચના Curl_scheme *scheme,
                              અચલ અક્ષર *path,
                              bool abstract_unix_socket,
                              સંરચના Curl_peer **ppeer)
{
  સંરચના peer_parse pp;
  size_t pathlen = path ? strlen(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, કદ(pp));
  જો(!scheme)
    પાછા CURLE_FAILED_INIT;
  જો(!pathlen)
    પાછા CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  પાછા result;
}
#endif /* USE_UNIX_SOCKETS */

શૂન્ય Curl_peer_link(સંરચના Curl_peer **pdest, સંરચના Curl_peer *src)
{
  જો(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    જો(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

શૂન્ય Curl_peer_unlink(સંરચના Curl_peer **ppeer)
{
  જો(*ppeer) {
    સંરચના Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = ખાલી;
    જો(peer->refcount)
      peer->refcount--;
    જો(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(સંરચના Curl_peer *p1, સંરચના Curl_peer *p2)
{
  પાછા (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

સ્થિર bool peer_same_hostname(સંરચના Curl_peer *p1, સંરચના Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  પાછા (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

સ્થિર bool peer_same_zoneid(અચલ અક્ષર *z1, અચલ અક્ષર *z2)
{
  પાછા (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(સંરચના Curl_peer *p1, સંરચના Curl_peer *p2)
{
  પાછા (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, સંરચના Curl_easy *data,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            સંરચના Curl_peer **ppeer)
{
  સંરચના peer_parse pp;
  અક્ષર *zoneid = ખાલી, *scheme = ખાલી, *hostname = ખાલી;
  CURLUcode uc;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, કદ(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  જો(uc)
    પાછા Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  જો(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    જાઓ out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  જો(uc) {
    જો((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    નહીંતર {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }
  નહીંતર જો(strlen(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    જાઓ out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = strlen(pp.host_user.str);
  જો(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    જો(result)
      જાઓ out;
  }
  નહીંતર
    pp.host = pp.host_user;

  જો(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    અક્ષર portbuf[16];
    curl_msnprintf(portbuf, કદ(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    જો(uc) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
    નહીંતર
      pp.port = port_override;
  }
  નહીંતર {
    uc = Curl_url_get_port(uh, &pp.port);
    જો(uc) {
      જો(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
      નહીંતર જો(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        જાઓ out;
      }
      /* no port ok when not a network scheme */
    }
  }

  જો(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  નહીંતર {
    જો(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
    જો(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = strlen(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  જો(result)
    failf(data, "Error %d creating peer for %s:%u",
          (પૂર્ણાંક)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  પાછા result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(સંરચના Curl_easy *data,
                                   અચલ સંરચના Curl_peer *dest,
                                   અચલ અક્ષર *connect_to,
                                   સંરચના Curl_peer **ppeer)
{
  સંરચના peer_parse pp;
  અચલ અક્ષર *portstr = ખાલી;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, કદ(pp));
  જો(!connect_to || !*connect_to)
    પાછા CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  જો(connect_to[0] == '[') {
    અચલ અક્ષર *s = strchr(connect_to + 1, ']');
    જો(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      જાઓ out;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  નહીંતર {
    portstr = strchr(connect_to, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_to) : strlen(connect_to);
  }

  જો(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = strlen(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  જો(result)
    જાઓ out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  જો(portstr && portstr[1]) {
    અચલ અક્ષર *p = portstr + 1;
    curl_off_t portparse;
    જો(curlx_str_સંખ્યા(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      જાઓ out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  નહીંતર
    pp.port = dest->port;

#ifndef USE_IPV6
  જો(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    જાઓ out;
  }
#endif
  જો(pp.ipv6 && port_switch) {
    જો(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = strlen(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (પૂર્ણાંક)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (પૂર્ણાંક)result);
  peer_parse_clear(&pp);
  પાછા result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(સંરચના Curl_easy *data,
                                  અચલ અક્ષર *scheme,
                                  uint8_t *proxytype, અચલ અક્ષર *url)
{
  જો(!scheme)
    પાછા CURLE_OK;

  જો(curl_strequal("https", scheme)) {
    જો(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  નહીંતર જો(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  નહીંતર જો(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  નહીંતર જો(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  નહીંતર જો(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  નહીંતર જો(curl_strequal("http", scheme)) {
    જો(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  નહીંતર {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    પાછા CURLE_COULDNT_CONNECT;
  }
  પાછા CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  સંરચના Curl_easy *data,
                                  અચલ અક્ષર *url,
                                  uint8_t proxytype,
                                  સંરચના Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  સંરચના peer_parse pp;
  અક્ષર *scheme = ખાલી;
  અક્ષર *portptr = ખાલી;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, કદ(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  જો(uc) {
    જો(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    પસંદગી(proxytype) {
    સ્થિતિ CURLPROXY_HTTP:
    સ્થિતિ CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      તોડો;
    સ્થિતિ CURLPROXY_HTTPS:
    સ્થિતિ CURLPROXY_HTTPS2:
    સ્થિતિ CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      તોડો;
    સ્થિતિ CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      તોડો;
    સ્થિતિ CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      તોડો;
    સ્થિતિ CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      તોડો;
    સ્થિતિ CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      તોડો;
    મૂળભૂત:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      જાઓ out;
    }
  }
  નહીંતર {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    જો(result)
      જાઓ out;
  }
  DEBUGASSERT(pp.scheme);

  જો(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(data, SSLSUPP_HTTPS_PROXY)) {
    failf(data, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    result = CURLE_NOT_BUILT_IN;
    જાઓ out;
  }

  પસંદગી(pp.scheme->family) {
  સ્થિતિ CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    તોડો;
  સ્થિતિ CURLPROTO_HTTP:
    તોડો;
  મૂળભૂત:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    જાઓ out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  જો(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  જો(portptr) {
    curl_off_t અંક;
    અચલ અક્ષર *p = portptr;
    જો(!curlx_str_સંખ્યા(&p, &અંક, UINT16_MAX))
      pp.port = (uint16_t)અંક;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  નહીંતર {
    /* No port in URL, take the set one or the scheme's default */
    જો(data->set.proxyport)
      pp.port = data->set.proxyport;
    નહીંતર
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  જો(uc) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = strlen(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  જો(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    જો(uc) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
    /* path will be "/", if no path was found */
    જો(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = strlen(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    નહીંતર {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  જો(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    જો(result)
      જાઓ out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  જો(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  જો(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = strlen(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  જો(!result)
    DEBUGASSERT(*ppeer);
#endif
  પાછા result;
}

#endif /* !CURL_DISABLE_PROXY */
