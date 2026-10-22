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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাৱ <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#অন্তর্ভাৱ <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাৱ <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাৱ <sys/param.h>
#endif

#ifdef __VMS
#অন্তর্ভাৱ <in.h>
#অন্তর্ভাৱ <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#অন্তর্ভাৱ <sys/un.h>
#endif

#যদি defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#যদি defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#অন্তর্ভাৱ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#অন্তর্ভাৱ <iphlpapi.h>
#endif

#অন্তর্ভাৱ "curl_addrinfo.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "protocol.h"
#অন্তর্ভাৱ "http_proxy.h"
#অন্তর্ভাৱ "idn.h"
#অন্তর্ভাৱ "curlx/strdup.h"
#অন্তর্ভাৱ "curlx/strparse.h"
#অন্তর্ভাৱ "peer.h"
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "urlapi-int.h"
#অন্তর্ভাৱ "vtls/vtls.h"

সংরচনা peer_parse {
  ধ্রুৱ সংরচনা Curl_scheme *scheme;
  সংরচনা Curl_str host_user;
  সংরচনা Curl_str host;
  সংরচনা Curl_str zoneid;
  ৱর্ণ *tmp_host_user;
  ৱর্ণ *tmp_host;
  ৱর্ণ *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

স্থির শূন্য peer_parse_clear(সংরচনা peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, পরিমাণ(*pp));
}

স্থির CURLcode peer_create(সংরচনা peer_parse *pp,
                            সংরচনা Curl_peer **ppeer)
{
  সংরচনা Curl_peer *peer = রিক্ত;
  CURLcode result = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  যদি(!pp || !pp->scheme)
    ঘূৰাই CURLE_FAILED_INIT;
  যদি(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    ঘূৰাই CURLE_FAILED_INIT;

  যদি((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, পরিমাণ(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  যদি(!peer) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  যদি(pp->host_user.len)
    memcpy(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  যদি(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    memcpy(peer->hostname, pp->host.str, pp->host.len);
  }

  যদি(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    memcpy(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    যদি(!peer->scopeid) {
      ধ্রুৱ ৱর্ণ *p = peer->zoneid;
      curl_off_t scope;
      যদি(!curlx_str_সংখ্যা(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      অন্যথা {
        /* Zone identifier is not fully numeric */
        unsigned পূর্ণাঙ্ক idx = 0;
        idx = if_nametoindex(peer->zoneid);
        যদি(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        অন্যথা {
          /* Do we want to return an error here? */
        }
      }
#endif /* HAVE_IF_NAMETOINDEX */
    }
#endif /* USE_IPV6 */
  }

out:
  যদি(!result)
    *ppeer = peer;
  অন্যথা
    Curl_peer_unlink(&peer);
  ঘূৰাই result;
}

স্থির CURLcode peer_parse_host(সংরচনা Curl_easy *data,
                                সংরচনা peer_parse *pp,
                                bool scan_for_ipv6)
{
  যদি(!pp || !pp->host_user.str || !pp->host_user.len)
    ঘূৰাই CURLE_FAILED_INIT;

  যদি(pp->host_user.str[0] == '[') {
    ধ্রুৱ ৱর্ণ *s = pp->host_user.str + 1;
    সংরচনা Curl_str tmp;
    যদি(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      ঘূৰাই CURLE_URL_MALFORMAT;

    যদি(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (পূর্ণাঙ্ক)pp->host_user.len, pp->host_user.str);
      ঘূৰাই CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  অন্যথা {
#ifdef USE_IDN
    যদি(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      যদি(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        যদি(!pp->tmp_host_user)
          ঘূৰাই CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      যদি(result)
        ঘূৰাই result;
      pp->host.str = pp->tmp_host;
      pp->host.len = strlen(pp->host.str);
    }
    অন্যথা
#endif
    যদি(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      যদি(pp->host_user.len < MAX_IPADR_LEN) {
        ৱর্ণ tmp[MAX_IPADR_LEN];
        memcpy(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      অন্যথা
        pp->ipv6 = TRUE;
    }
    অন্যথা
      pp->host = pp->host_user;
  }
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_peer_create(সংরচনা Curl_easy *data,
                          ধ্রুৱ সংরচনা Curl_scheme *scheme,
                          ধ্রুৱ ৱর্ণ *hostname,
                          uint16_t port,
                          সংরচনা Curl_peer **ppeer)
{
  সংরচনা peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, পরিমাণ(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = strlen(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  যদি(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  ঘূৰাই result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ধ্রুৱ সংরচনা Curl_scheme *scheme,
                              ধ্রুৱ ৱর্ণ *path,
                              bool abstract_unix_socket,
                              সংরচনা Curl_peer **ppeer)
{
  সংরচনা peer_parse pp;
  size_t pathlen = path ? strlen(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, পরিমাণ(pp));
  যদি(!scheme)
    ঘূৰাই CURLE_FAILED_INIT;
  যদি(!pathlen)
    ঘূৰাই CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  ঘূৰাই result;
}
#endif /* USE_UNIX_SOCKETS */

শূন্য Curl_peer_link(সংরচনা Curl_peer **pdest, সংরচনা Curl_peer *src)
{
  যদি(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    যদি(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

শূন্য Curl_peer_unlink(সংরচনা Curl_peer **ppeer)
{
  যদি(*ppeer) {
    সংরচনা Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = রিক্ত;
    যদি(peer->refcount)
      peer->refcount--;
    যদি(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(সংরচনা Curl_peer *p1, সংরচনা Curl_peer *p2)
{
  ঘূৰাই (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

স্থির bool peer_same_hostname(সংরচনা Curl_peer *p1, সংরচনা Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  ঘূৰাই (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

স্থির bool peer_same_zoneid(ধ্রুৱ ৱর্ণ *z1, ধ্রুৱ ৱর্ণ *z2)
{
  ঘূৰাই (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(সংরচনা Curl_peer *p1, সংরচনা Curl_peer *p2)
{
  ঘূৰাই (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, সংরচনা Curl_easy *data,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            সংরচনা Curl_peer **ppeer)
{
  সংরচনা peer_parse pp;
  ৱর্ণ *zoneid = রিক্ত, *scheme = রিক্ত, *hostname = রিক্ত;
  CURLUcode uc;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, পরিমাণ(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  যদি(uc)
    ঘূৰাই Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  যদি(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    গচ্ছ out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  যদি(uc) {
    যদি((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    অন্যথা {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
  }
  অন্যথা যদি(strlen(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    গচ্ছ out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = strlen(pp.host_user.str);
  যদি(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    যদি(result)
      গচ্ছ out;
  }
  অন্যথা
    pp.host = pp.host_user;

  যদি(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    ৱর্ণ portbuf[16];
    curl_msnprintf(portbuf, পরিমাণ(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    যদি(uc) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
    অন্যথা
      pp.port = port_override;
  }
  অন্যথা {
    uc = Curl_url_get_port(uh, &pp.port);
    যদি(uc) {
      যদি(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
      অন্যথা যদি(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        গচ্ছ out;
      }
      /* no port ok when not a network scheme */
    }
  }

  যদি(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  অন্যথা {
    যদি(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
    যদি(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = strlen(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  যদি(result)
    failf(data, "Error %d creating peer for %s:%u",
          (পূর্ণাঙ্ক)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  ঘূৰাই result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(সংরচনা Curl_easy *data,
                                   ধ্রুৱ সংরচনা Curl_peer *dest,
                                   ধ্রুৱ ৱর্ণ *connect_to,
                                   সংরচনা Curl_peer **ppeer)
{
  সংরচনা peer_parse pp;
  ধ্রুৱ ৱর্ণ *portstr = রিক্ত;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, পরিমাণ(pp));
  যদি(!connect_to || !*connect_to)
    ঘূৰাই CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  যদি(connect_to[0] == '[') {
    ধ্রুৱ ৱর্ণ *s = strchr(connect_to + 1, ']');
    যদি(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      গচ্ছ out;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  অন্যথা {
    portstr = strchr(connect_to, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_to) : strlen(connect_to);
  }

  যদি(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = strlen(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  যদি(result)
    গচ্ছ out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  যদি(portstr && portstr[1]) {
    ধ্রুৱ ৱর্ণ *p = portstr + 1;
    curl_off_t portparse;
    যদি(curlx_str_সংখ্যা(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      গচ্ছ out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  অন্যথা
    pp.port = dest->port;

#ifndef USE_IPV6
  যদি(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    গচ্ছ out;
  }
#endif
  যদি(pp.ipv6 && port_switch) {
    যদি(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = strlen(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (পূর্ণাঙ্ক)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (পূর্ণাঙ্ক)result);
  peer_parse_clear(&pp);
  ঘূৰাই result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(সংরচনা Curl_easy *data,
                                  ধ্রুৱ ৱর্ণ *scheme,
                                  uint8_t *proxytype, ধ্রুৱ ৱর্ণ *url)
{
  যদি(!scheme)
    ঘূৰাই CURLE_OK;

  যদি(curl_strequal("https", scheme)) {
    যদি(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  অন্যথা যদি(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  অন্যথা যদি(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  অন্যথা যদি(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  অন্যথা যদি(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  অন্যথা যদি(curl_strequal("http", scheme)) {
    যদি(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  অন্যথা {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    ঘূৰাই CURLE_COULDNT_CONNECT;
  }
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  সংরচনা Curl_easy *data,
                                  ধ্রুৱ ৱর্ণ *url,
                                  uint8_t proxytype,
                                  সংরচনা Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  সংরচনা peer_parse pp;
  ৱর্ণ *scheme = রিক্ত;
  ৱর্ণ *portptr = রিক্ত;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, পরিমাণ(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  যদি(uc) {
    যদি(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    ৱিকল্প(proxytype) {
    পক্ষ CURLPROXY_HTTP:
    পক্ষ CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      ভঙ্গ;
    পক্ষ CURLPROXY_HTTPS:
    পক্ষ CURLPROXY_HTTPS2:
    পক্ষ CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      ভঙ্গ;
    পক্ষ CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      ভঙ্গ;
    পক্ষ CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      ভঙ্গ;
    পক্ষ CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      ভঙ্গ;
    পক্ষ CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      ভঙ্গ;
    স্ৱভাৱ:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      গচ্ছ out;
    }
  }
  অন্যথা {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    যদি(result)
      গচ্ছ out;
  }
  DEBUGASSERT(pp.scheme);

  যদি(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(data, SSLSUPP_HTTPS_PROXY)) {
    failf(data, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    result = CURLE_NOT_BUILT_IN;
    গচ্ছ out;
  }

  ৱিকল্প(pp.scheme->family) {
  পক্ষ CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    ভঙ্গ;
  পক্ষ CURLPROTO_HTTP:
    ভঙ্গ;
  স্ৱভাৱ:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    গচ্ছ out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  যদি(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  যদি(portptr) {
    curl_off_t অঙ্ক;
    ধ্রুৱ ৱর্ণ *p = portptr;
    যদি(!curlx_str_সংখ্যা(&p, &অঙ্ক, UINT16_MAX))
      pp.port = (uint16_t)অঙ্ক;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  অন্যথা {
    /* No port in URL, take the set one or the scheme's default */
    যদি(data->set.proxyport)
      pp.port = data->set.proxyport;
    অন্যথা
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  যদি(uc) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = strlen(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  যদি(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    যদি(uc) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
    /* path will be "/", if no path was found */
    যদি(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = strlen(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    অন্যথা {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  যদি(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    যদি(result)
      গচ্ছ out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  যদি(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  যদি(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = strlen(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  যদি(!result)
    DEBUGASSERT(*ppeer);
#endif
  ঘূৰাই result;
}

#endif /* !CURL_DISABLE_PROXY */
