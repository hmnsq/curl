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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ଅନ୍ତର୍ଭାଵ <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#ଅନ୍ତର୍ଭାଵ <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#ଅନ୍ତର୍ଭାଵ <sys/param.h>
#endif

#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#ଅନ୍ତର୍ଭାଵ <sys/un.h>
#endif

#ଯଦି defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#ଯଦି defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ଅନ୍ତର୍ଭାଵ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#ଅନ୍ତର୍ଭାଵ <iphlpapi.h>
#endif

#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "protocol.h"
#ଅନ୍ତର୍ଭାଵ "http_proxy.h"
#ଅନ୍ତର୍ଭାଵ "idn.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "peer.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "urlapi-int.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"

ସଂରଚନା peer_parse {
  ଅଚଳ ସଂରଚନା Curl_scheme *scheme;
  ସଂରଚନା Curl_str host_user;
  ସଂରଚନା Curl_str host;
  ସଂରଚନା Curl_str zoneid;
  ଅକ୍ଷର *tmp_host_user;
  ଅକ୍ଷର *tmp_host;
  ଅକ୍ଷର *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

ସ୍ଥିର ଶୂନ୍ୟ peer_parse_clear(ସଂରଚନା peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, ଆକାର(*pp));
}

ସ୍ଥିର CURLcode peer_create(ସଂରଚନା peer_parse *pp,
                            ସଂରଚନା Curl_peer **ppeer)
{
  ସଂରଚନା Curl_peer *peer = ଖାଲି;
  CURLcode result = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  ଯଦି(!pp || !pp->scheme)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, ଆକାର(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  ଯଦି(!peer) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  ଯଦି(pp->host_user.len)
    memcpy(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  ଯଦି(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    memcpy(peer->hostname, pp->host.str, pp->host.len);
  }

  ଯଦି(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    memcpy(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    ଯଦି(!peer->scopeid) {
      ଅଚଳ ଅକ୍ଷର *p = peer->zoneid;
      curl_off_t scope;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      ଅନ୍ୟଥା {
        /* Zone identifier is not fully numeric */
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0;
        idx = if_nametoindex(peer->zoneid);
        ଯଦି(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        ଅନ୍ୟଥା {
          /* Do we want to return an error here? */
        }
      }
#endif /* HAVE_IF_NAMETOINDEX */
    }
#endif /* USE_IPV6 */
  }

out:
  ଯଦି(!result)
    *ppeer = peer;
  ଅନ୍ୟଥା
    Curl_peer_unlink(&peer);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode peer_parse_host(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା peer_parse *pp,
                                bool scan_for_ipv6)
{
  ଯଦି(!pp || !pp->host_user.str || !pp->host_user.len)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(pp->host_user.str[0] == '[') {
    ଅଚଳ ଅକ୍ଷର *s = pp->host_user.str + 1;
    ସଂରଚନା Curl_str tmp;
    ଯଦି(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;

    ଯଦି(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pp->host_user.len, pp->host_user.str);
      ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  ଅନ୍ୟଥା {
#ifdef USE_IDN
    ଯଦି(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      ଯଦି(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        ଯଦି(!pp->tmp_host_user)
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      pp->host.str = pp->tmp_host;
      pp->host.len = strlen(pp->host.str);
    }
    ଅନ୍ୟଥା
#endif
    ଯଦି(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      ଯଦି(pp->host_user.len < MAX_IPADR_LEN) {
        ଅକ୍ଷର tmp[MAX_IPADR_LEN];
        memcpy(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      ଅନ୍ୟଥା
        pp->ipv6 = TRUE;
    }
    ଅନ୍ୟଥା
      pp->host = pp->host_user;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_peer_create(ସଂରଚନା Curl_easy *data,
                          ଅଚଳ ସଂରଚନା Curl_scheme *scheme,
                          ଅଚଳ ଅକ୍ଷର *hostname,
                          uint16_t port,
                          ସଂରଚନା Curl_peer **ppeer)
{
  ସଂରଚନା peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ଆକାର(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = strlen(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  ଯଦି(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  ଫେରନ୍ତୁ result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ଅଚଳ ସଂରଚନା Curl_scheme *scheme,
                              ଅଚଳ ଅକ୍ଷର *path,
                              bool abstract_unix_socket,
                              ସଂରଚନା Curl_peer **ppeer)
{
  ସଂରଚନା peer_parse pp;
  size_t pathlen = path ? strlen(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ଆକାର(pp));
  ଯଦି(!scheme)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି(!pathlen)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  ଫେରନ୍ତୁ result;
}
#endif /* USE_UNIX_SOCKETS */

ଶୂନ୍ୟ Curl_peer_link(ସଂରଚନା Curl_peer **pdest, ସଂରଚନା Curl_peer *src)
{
  ଯଦି(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    ଯଦି(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ଶୂନ୍ୟ Curl_peer_unlink(ସଂରଚନା Curl_peer **ppeer)
{
  ଯଦି(*ppeer) {
    ସଂରଚନା Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = ଖାଲି;
    ଯଦି(peer->refcount)
      peer->refcount--;
    ଯଦି(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(ସଂରଚନା Curl_peer *p1, ସଂରଚନା Curl_peer *p2)
{
  ଫେରନ୍ତୁ (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

ସ୍ଥିର bool peer_same_hostname(ସଂରଚନା Curl_peer *p1, ସଂରଚନା Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  ଫେରନ୍ତୁ (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

ସ୍ଥିର bool peer_same_zoneid(ଅଚଳ ଅକ୍ଷର *z1, ଅଚଳ ଅକ୍ଷର *z2)
{
  ଫେରନ୍ତୁ (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(ସଂରଚନା Curl_peer *p1, ସଂରଚନା Curl_peer *p2)
{
  ଫେରନ୍ତୁ (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, ସଂରଚନା Curl_easy *data,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            ସଂରଚନା Curl_peer **ppeer)
{
  ସଂରଚନା peer_parse pp;
  ଅକ୍ଷର *zoneid = ଖାଲି, *scheme = ଖାଲି, *hostname = ଖାଲି;
  CURLUcode uc;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ଆକାର(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  ଯଦି(uc)
    ଫେରନ୍ତୁ Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  ଯଦି(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ଯାଅ out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  ଯଦି(uc) {
    ଯଦି((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    ଅନ୍ୟଥା {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
  }
  ଅନ୍ୟଥା ଯଦି(strlen(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    ଯାଅ out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = strlen(pp.host_user.str);
  ଯଦି(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଅନ୍ୟଥା
    pp.host = pp.host_user;

  ଯଦି(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    ଅକ୍ଷର portbuf[16];
    curl_msnprintf(portbuf, ଆକାର(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    ଯଦି(uc) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା
      pp.port = port_override;
  }
  ଅନ୍ୟଥା {
    uc = Curl_url_get_port(uh, &pp.port);
    ଯଦି(uc) {
      ଯଦି(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
      ଅନ୍ୟଥା ଯଦି(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        ଯାଅ out;
      }
      /* no port ok when not a network scheme */
    }
  }

  ଯଦି(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  ଅନ୍ୟଥା {
    ଯଦି(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
    ଯଦି(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = strlen(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  ଯଦି(result)
    failf(data, "Error %d creating peer for %s:%u",
          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  ଫେରନ୍ତୁ result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(ସଂରଚନା Curl_easy *data,
                                   ଅଚଳ ସଂରଚନା Curl_peer *dest,
                                   ଅଚଳ ଅକ୍ଷର *connect_to,
                                   ସଂରଚନା Curl_peer **ppeer)
{
  ସଂରଚନା peer_parse pp;
  ଅଚଳ ଅକ୍ଷର *portstr = ଖାଲି;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ଆକାର(pp));
  ଯଦି(!connect_to || !*connect_to)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  ଯଦି(connect_to[0] == '[') {
    ଅଚଳ ଅକ୍ଷର *s = strchr(connect_to + 1, ']');
    ଯଦି(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      ଯାଅ out;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  ଅନ୍ୟଥା {
    portstr = strchr(connect_to, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_to) : strlen(connect_to);
  }

  ଯଦି(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = strlen(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  ଯଦି(result)
    ଯାଅ out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  ଯଦି(portstr && portstr[1]) {
    ଅଚଳ ଅକ୍ଷର *p = portstr + 1;
    curl_off_t portparse;
    ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      ଯାଅ out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  ଅନ୍ୟଥା
    pp.port = dest->port;

#ifndef USE_IPV6
  ଯଦି(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    ଯାଅ out;
  }
#endif
  ଯଦି(pp.ipv6 && port_switch) {
    ଯଦି(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = strlen(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  peer_parse_clear(&pp);
  ଫେରନ୍ତୁ result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ ଅକ୍ଷର *scheme,
                                  uint8_t *proxytype, ଅଚଳ ଅକ୍ଷର *url)
{
  ଯଦି(!scheme)
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(curl_strequal("https", scheme)) {
    ଯଦି(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  ଅନ୍ୟଥା ଯଦି(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  ଅନ୍ୟଥା ଯଦି(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  ଅନ୍ୟଥା ଯଦି(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  ଅନ୍ୟଥା ଯଦି(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  ଅନ୍ୟଥା ଯଦି(curl_strequal("http", scheme)) {
    ଯଦି(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  ଅନ୍ୟଥା {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ ଅକ୍ଷର *url,
                                  uint8_t proxytype,
                                  ସଂରଚନା Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  ସଂରଚନା peer_parse pp;
  ଅକ୍ଷର *scheme = ଖାଲି;
  ଅକ୍ଷର *portptr = ଖାଲି;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ଆକାର(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  ଯଦି(uc) {
    ଯଦି(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    ବାଛ(proxytype) {
    ସ୍ଥିତି CURLPROXY_HTTP:
    ସ୍ଥିତି CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLPROXY_HTTPS:
    ସ୍ଥିତି CURLPROXY_HTTPS2:
    ସ୍ଥିତି CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      ଯାଅ out;
    }
  }
  ଅନ୍ୟଥା {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    ଯଦି(result)
      ଯାଅ out;
  }
  DEBUGASSERT(pp.scheme);

  ଯଦି(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(data, SSLSUPP_HTTPS_PROXY)) {
    failf(data, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    result = CURLE_NOT_BUILT_IN;
    ଯାଅ out;
  }

  ବାଛ(pp.scheme->family) {
  ସ୍ଥିତି CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLPROTO_HTTP:
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    ଯାଅ out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  ଯଦି(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  ଯଦି(portptr) {
    curl_off_t ଅଙ୍କ;
    ଅଚଳ ଅକ୍ଷର *p = portptr;
    ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &ଅଙ୍କ, UINT16_MAX))
      pp.port = (uint16_t)ଅଙ୍କ;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  ଅନ୍ୟଥା {
    /* No port in URL, take the set one or the scheme's default */
    ଯଦି(data->set.proxyport)
      pp.port = data->set.proxyport;
    ଅନ୍ୟଥା
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  ଯଦି(uc) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = strlen(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  ଯଦି(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    ଯଦି(uc) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
    /* path will be "/", if no path was found */
    ଯଦି(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = strlen(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    ଅନ୍ୟଥା {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  ଯଦି(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    ଯଦି(result)
      ଯାଅ out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  ଯଦି(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  ଯଦି(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = strlen(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  ଯଦି(!result)
    DEBUGASSERT(*ppeer);
#endif
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_PROXY */
