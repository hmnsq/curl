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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাব <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#অন্তর্ভাব <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাব <sys/param.h>
#endif

#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#অন্তর্ভাব <sys/un.h>
#endif

#যদি defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#যদি defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#অন্তর্ভাব <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#অন্তর্ভাব <iphlpapi.h>
#endif

#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "protocol.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "idn.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "peer.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "urlapi-int.h"
#অন্তর্ভাব "vtls/vtls.h"

কাঠামো peer_parse {
  ধ্রুবক কাঠামো Curl_scheme *scheme;
  কাঠামো Curl_str host_user;
  কাঠামো Curl_str host;
  কাঠামো Curl_str zoneid;
  অক্ষর *tmp_host_user;
  অক্ষর *tmp_host;
  অক্ষর *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

স্থির শূন্য peer_parse_clear(কাঠামো peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, আকার(*pp));
}

স্থির CURLcode peer_create(কাঠামো peer_parse *pp,
                            কাঠামো Curl_peer **ppeer)
{
  কাঠামো Curl_peer *peer = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  যদি(!pp || !pp->scheme)
    ফেরত CURLE_FAILED_INIT;
  যদি(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    ফেরত CURLE_FAILED_INIT;

  যদি((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, আকার(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  যদি(!peer) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
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
      ধ্রুবক অক্ষর *p = peer->zoneid;
      curl_off_t scope;
      যদি(!curlx_str_সংখ্যা(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      অন্যথা {
        /* Zone identifier is not fully numeric */
        unsigned পূর্ণসংখ্যা idx = 0;
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
  যদি(!ফলাফল)
    *ppeer = peer;
  অন্যথা
    Curl_peer_unlink(&peer);
  ফেরত ফলাফল;
}

স্থির CURLcode peer_parse_host(কাঠামো Curl_easy *তথ্য,
                                কাঠামো peer_parse *pp,
                                bool scan_for_ipv6)
{
  যদি(!pp || !pp->host_user.str || !pp->host_user.len)
    ফেরত CURLE_FAILED_INIT;

  যদি(pp->host_user.str[0] == '[') {
    ধ্রুবক অক্ষর *s = pp->host_user.str + 1;
    কাঠামো Curl_str tmp;
    যদি(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      ফেরত CURLE_URL_MALFORMAT;

    যদি(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(তথ্য, "Invalid IPv6 address format in '%.*s'",
            (পূর্ণসংখ্যা)pp->host_user.len, pp->host_user.str);
      ফেরত CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  অন্যথা {
#ifdef USE_IDN
    যদি(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode ফলাফল;
      যদি(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        যদি(!pp->tmp_host_user)
          ফেরত CURLE_OUT_OF_MEMORY;
      }
      ফলাফল = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      pp->host.str = pp->tmp_host;
      pp->host.len = strlen(pp->host.str);
    }
    অন্যথা
#endif
    যদি(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      যদি(pp->host_user.len < MAX_IPADR_LEN) {
        অক্ষর tmp[MAX_IPADR_LEN];
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
  ফেরত CURLE_OK;
}

CURLcode Curl_peer_create(কাঠামো Curl_easy *তথ্য,
                          ধ্রুবক কাঠামো Curl_scheme *scheme,
                          ধ্রুবক অক্ষর *hostname,
                          uint16_t port,
                          কাঠামো Curl_peer **ppeer)
{
  কাঠামো peer_parse pp;
  CURLcode ফলাফল;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, আকার(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = strlen(hostname);
  pp.port = port;

  ফলাফল = peer_parse_host(তথ্য, &pp, TRUE);
  যদি(!ফলাফল)
    ফলাফল = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  ফেরত ফলাফল;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ধ্রুবক কাঠামো Curl_scheme *scheme,
                              ধ্রুবক অক্ষর *path,
                              bool abstract_unix_socket,
                              কাঠামো Curl_peer **ppeer)
{
  কাঠামো peer_parse pp;
  size_t pathlen = path ? strlen(path) : 0;
  CURLcode ফলাফল = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, আকার(pp));
  যদি(!scheme)
    ফেরত CURLE_FAILED_INIT;
  যদি(!pathlen)
    ফেরত CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  ফলাফল = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  ফেরত ফলাফল;
}
#endif /* USE_UNIX_SOCKETS */

শূন্য Curl_peer_link(কাঠামো Curl_peer **pdest, কাঠামো Curl_peer *src)
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

শূন্য Curl_peer_unlink(কাঠামো Curl_peer **ppeer)
{
  যদি(*ppeer) {
    কাঠামো Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = ফাঁকা;
    যদি(peer->refcount)
      peer->refcount--;
    যদি(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(কাঠামো Curl_peer *p1, কাঠামো Curl_peer *p2)
{
  ফেরত (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

স্থির bool peer_same_hostname(কাঠামো Curl_peer *p1, কাঠামো Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  ফেরত (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

স্থির bool peer_same_zoneid(ধ্রুবক অক্ষর *z1, ধ্রুবক অক্ষর *z2)
{
  ফেরত (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(কাঠামো Curl_peer *p1, কাঠামো Curl_peer *p2)
{
  ফেরত (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, কাঠামো Curl_easy *তথ্য,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            কাঠামো Curl_peer **ppeer)
{
  কাঠামো peer_parse pp;
  অক্ষর *zoneid = ফাঁকা, *scheme = ফাঁকা, *hostname = ফাঁকা;
  CURLUcode uc;
  CURLcode ফলাফল;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, আকার(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  যদি(uc)
    ফেরত Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  যদি(!pp.scheme) {
    failf(তথ্য, "Protocol \"%s\" not supported%s", scheme,
          তথ্য->state.this_is_a_follow ? " (in redirect)" : "");
    ফলাফল = CURLE_UNSUPPORTED_PROTOCOL;
    যাও out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  যদি(uc) {
    যদি((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    অন্যথা {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }
  অন্যথা যদি(strlen(hostname) > MAX_URL_LEN) {
    failf(তথ্য, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    ফলাফল = CURLE_URL_MALFORMAT;
    যাও out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = strlen(pp.host_user.str);
  যদি(pp.host_user.len) {
    ফলাফল = peer_parse_host(তথ্য, &pp, FALSE);
    যদি(ফলাফল)
      যাও out;
  }
  অন্যথা
    pp.host = pp.host_user;

  যদি(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    অক্ষর portbuf[16];
    curl_msnprintf(portbuf, আকার(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    যদি(uc) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
    অন্যথা
      pp.port = port_override;
  }
  অন্যথা {
    uc = Curl_url_get_port(uh, &pp.port);
    যদি(uc) {
      যদি(uc == CURLUE_OUT_OF_MEMORY) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
      অন্যথা যদি(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        ফলাফল = CURLE_URL_MALFORMAT;
        যাও out;
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
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
    যদি(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = strlen(zoneid);
    }
  }

  ফলাফল = peer_create(&pp, ppeer);
  যদি(ফলাফল)
    failf(তথ্য, "Error %d creating peer for %s:%u",
          (পূর্ণসংখ্যা)ফলাফল, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  ফেরত ফলাফল;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(কাঠামো Curl_easy *তথ্য,
                                   ধ্রুবক কাঠামো Curl_peer *dest,
                                   ধ্রুবক অক্ষর *connect_to,
                                   কাঠামো Curl_peer **ppeer)
{
  কাঠামো peer_parse pp;
  ধ্রুবক অক্ষর *portstr = ফাঁকা;
  bool port_switch = FALSE;
  CURLcode ফলাফল;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, আকার(pp));
  যদি(!connect_to || !*connect_to)
    ফেরত CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  যদি(connect_to[0] == '[') {
    ধ্রুবক অক্ষর *s = strchr(connect_to + 1, ']');
    যদি(!s) {
      failf(তথ্য, "Invalid IPv6 address format in '%s'", connect_to);
      ফলাফল = CURLE_SETOPT_OPTION_SYNTAX;
      যাও out;
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

  ফলাফল = peer_parse_host(তথ্য, &pp, FALSE);
  যদি(ফলাফল)
    যাও out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  যদি(portstr && portstr[1]) {
    ধ্রুবক অক্ষর *p = portstr + 1;
    curl_off_t portparse;
    যদি(curlx_str_সংখ্যা(&p, &portparse, 0xffff)) {
      failf(তথ্য, "No valid port number in '%s'", connect_to);
      ফলাফল = CURLE_SETOPT_OPTION_SYNTAX;
      যাও out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  অন্যথা
    pp.port = dest->port;

#ifndef USE_IPV6
  যদি(pp.ipv6) {
    failf(তথ্য, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    ফলাফল = CURLE_NOT_BUILT_IN;
    যাও out;
  }
#endif
  যদি(pp.ipv6 && port_switch) {
    যদি(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = strlen(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  ফলাফল = peer_create(&pp, ppeer);
  CURL_TRC_M(তথ্য, "connect-to peer_create2 -> %d", (পূর্ণসংখ্যা)ফলাফল);

out:
  CURL_TRC_M(তথ্য, "parse connect_to peer: %s -> %d", connect_to, (পূর্ণসংখ্যা)ফলাফল);
  peer_parse_clear(&pp);
  ফেরত ফলাফল;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক অক্ষর *scheme,
                                  uint8_t *proxytype, ধ্রুবক অক্ষর *url)
{
  যদি(!scheme)
    ফেরত CURLE_OK;

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
    failf(তথ্য, "Unsupported proxy scheme for \'%s\'", url);
    ফেরত CURLE_COULDNT_CONNECT;
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক অক্ষর *url,
                                  uint8_t proxytype,
                                  কাঠামো Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  কাঠামো peer_parse pp;
  অক্ষর *scheme = ফাঁকা;
  অক্ষর *portptr = ফাঁকা;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode ফলাফল = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, আকার(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  যদি(uc) {
    যদি(uc == CURLUE_OUT_OF_MEMORY) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    নির্বাচন(proxytype) {
    ক্ষেত্র CURLPROXY_HTTP:
    ক্ষেত্র CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      বিরতি;
    ক্ষেত্র CURLPROXY_HTTPS:
    ক্ষেত্র CURLPROXY_HTTPS2:
    ক্ষেত্র CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      বিরতি;
    ক্ষেত্র CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      বিরতি;
    ক্ষেত্র CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      বিরতি;
    ক্ষেত্র CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      বিরতি;
    ক্ষেত্র CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      বিরতি;
    পূর্বনির্ধারিত:
      failf(তথ্য, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      ফলাফল = CURLE_COULDNT_RESOLVE_PROXY;
      যাও out;
    }
  }
  অন্যথা {
    pp.scheme = Curl_get_scheme(scheme);
    ফলাফল = Curl_scheme_to_proxytype(তথ্য, scheme, &proxytype, url);
    যদি(ফলাফল)
      যাও out;
  }
  DEBUGASSERT(pp.scheme);

  যদি(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(তথ্য, SSLSUPP_HTTPS_PROXY)) {
    failf(তথ্য, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    ফলাফল = CURLE_NOT_BUILT_IN;
    যাও out;
  }

  নির্বাচন(pp.scheme->family) {
  ক্ষেত্র CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    বিরতি;
  ক্ষেত্র CURLPROTO_HTTP:
    বিরতি;
  পূর্বনির্ধারিত:
    failf(তথ্য, "Unsupported proxy protocol for \'%s\'", url);
    ফলাফল = CURLE_COULDNT_CONNECT;
    যাও out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  যদি(uc == CURLUE_OUT_OF_MEMORY) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  যদি(portptr) {
    curl_off_t অঙ্ক;
    ধ্রুবক অক্ষর *p = portptr;
    যদি(!curlx_str_সংখ্যা(&p, &অঙ্ক, UINT16_MAX))
      pp.port = (uint16_t)অঙ্ক;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  অন্যথা {
    /* No port in URL, take the set one or the scheme's default */
    যদি(তথ্য->set.proxyport)
      pp.port = তথ্য->set.proxyport;
    অন্যথা
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  যদি(uc) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = strlen(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  যদি(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    যদি(uc) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
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
    ফলাফল = peer_parse_host(তথ্য, &pp, FALSE);
    যদি(ফলাফল)
      যাও out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  যদি(uc == CURLUE_OUT_OF_MEMORY) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  যদি(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = strlen(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  ফলাফল = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  যদি(!ফলাফল)
    DEBUGASSERT(*ppeer);
#endif
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_PROXY */
