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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#अन्तर्भाव <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#endif

#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#अन्तर्भाव <sys/un.h>
#endif

#सचे defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#सचे defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#अन्तर्भाव <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#अन्तर्भाव <iphlpapi.h>
#endif

#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "protocol.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "idn.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "peer.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "urlapi-int.h"
#अन्तर्भाव "vtls/vtls.h"

संरचना peer_parse {
  ध्रुव संरचना Curl_scheme *scheme;
  संरचना Curl_str host_user;
  संरचना Curl_str host;
  संरचना Curl_str zoneid;
  वर्ण *tmp_host_user;
  वर्ण *tmp_host;
  वर्ण *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

स्थिर शून्य peer_parse_clear(संरचना peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, परिमाण(*pp));
}

स्थिर CURLcode peer_create(संरचना peer_parse *pp,
                            संरचना Curl_peer **ppeer)
{
  संरचना Curl_peer *peer = रिक्त;
  CURLcode result = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  सचे(!pp || !pp->scheme)
    पटिनिवत्त CURLE_FAILED_INIT;
  सचे(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    पटिनिवत्त CURLE_FAILED_INIT;

  सचे((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, परिमाण(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  सचे(!peer) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  सचे(pp->host_user.len)
    memcpy(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  सचे(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    memcpy(peer->hostname, pp->host.str, pp->host.len);
  }

  सचे(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    memcpy(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    सचे(!peer->scopeid) {
      ध्रुव वर्ण *p = peer->zoneid;
      curl_off_t scope;
      सचे(!curlx_str_सङ्ख्या(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      अञ्ञथा {
        /* Zone identifier is not fully numeric */
        unsigned पूर्णाङ्क idx = 0;
        idx = if_nametoindex(peer->zoneid);
        सचे(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        अञ्ञथा {
          /* Do we want to return an error here? */
        }
      }
#endif /* HAVE_IF_NAMETOINDEX */
    }
#endif /* USE_IPV6 */
  }

out:
  सचे(!result)
    *ppeer = peer;
  अञ्ञथा
    Curl_peer_unlink(&peer);
  पटिनिवत्त result;
}

स्थिर CURLcode peer_parse_host(संरचना Curl_easy *data,
                                संरचना peer_parse *pp,
                                bool scan_for_ipv6)
{
  सचे(!pp || !pp->host_user.str || !pp->host_user.len)
    पटिनिवत्त CURLE_FAILED_INIT;

  सचे(pp->host_user.str[0] == '[') {
    ध्रुव वर्ण *s = pp->host_user.str + 1;
    संरचना Curl_str tmp;
    सचे(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      पटिनिवत्त CURLE_URL_MALFORMAT;

    सचे(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (पूर्णाङ्क)pp->host_user.len, pp->host_user.str);
      पटिनिवत्त CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  अञ्ञथा {
#ifdef USE_IDN
    सचे(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      सचे(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        सचे(!pp->tmp_host_user)
          पटिनिवत्त CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      सचे(result)
        पटिनिवत्त result;
      pp->host.str = pp->tmp_host;
      pp->host.len = strlen(pp->host.str);
    }
    अञ्ञथा
#endif
    सचे(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      सचे(pp->host_user.len < MAX_IPADR_LEN) {
        वर्ण tmp[MAX_IPADR_LEN];
        memcpy(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      अञ्ञथा
        pp->ipv6 = TRUE;
    }
    अञ्ञथा
      pp->host = pp->host_user;
  }
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_peer_create(संरचना Curl_easy *data,
                          ध्रुव संरचना Curl_scheme *scheme,
                          ध्रुव वर्ण *hostname,
                          uint16_t port,
                          संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, परिमाण(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = strlen(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  सचे(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  पटिनिवत्त result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ध्रुव संरचना Curl_scheme *scheme,
                              ध्रुव वर्ण *path,
                              bool abstract_unix_socket,
                              संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  size_t pathlen = path ? strlen(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, परिमाण(pp));
  सचे(!scheme)
    पटिनिवत्त CURLE_FAILED_INIT;
  सचे(!pathlen)
    पटिनिवत्त CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  पटिनिवत्त result;
}
#endif /* USE_UNIX_SOCKETS */

शून्य Curl_peer_link(संरचना Curl_peer **pdest, संरचना Curl_peer *src)
{
  सचे(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    सचे(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

शून्य Curl_peer_unlink(संरचना Curl_peer **ppeer)
{
  सचे(*ppeer) {
    संरचना Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = रिक्त;
    सचे(peer->refcount)
      peer->refcount--;
    सचे(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(संरचना Curl_peer *p1, संरचना Curl_peer *p2)
{
  पटिनिवत्त (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

स्थिर bool peer_same_hostname(संरचना Curl_peer *p1, संरचना Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  पटिनिवत्त (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

स्थिर bool peer_same_zoneid(ध्रुव वर्ण *z1, ध्रुव वर्ण *z2)
{
  पटिनिवत्त (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(संरचना Curl_peer *p1, संरचना Curl_peer *p2)
{
  पटिनिवत्त (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, संरचना Curl_easy *data,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  वर्ण *zoneid = रिक्त, *scheme = रिक्त, *hostname = रिक्त;
  CURLUcode uc;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, परिमाण(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  सचे(uc)
    पटिनिवत्त Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  सचे(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    गच्छ out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  सचे(uc) {
    सचे((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    अञ्ञथा {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
  }
  अञ्ञथा सचे(strlen(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    गच्छ out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = strlen(pp.host_user.str);
  सचे(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    सचे(result)
      गच्छ out;
  }
  अञ्ञथा
    pp.host = pp.host_user;

  सचे(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    वर्ण portbuf[16];
    curl_msnprintf(portbuf, परिमाण(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    सचे(uc) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
    अञ्ञथा
      pp.port = port_override;
  }
  अञ्ञथा {
    uc = Curl_url_get_port(uh, &pp.port);
    सचे(uc) {
      सचे(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
      अञ्ञथा सचे(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        गच्छ out;
      }
      /* no port ok when not a network scheme */
    }
  }

  सचे(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  अञ्ञथा {
    सचे(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
    सचे(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = strlen(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  सचे(result)
    failf(data, "Error %d creating peer for %s:%u",
          (पूर्णाङ्क)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  पटिनिवत्त result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(संरचना Curl_easy *data,
                                   ध्रुव संरचना Curl_peer *dest,
                                   ध्रुव वर्ण *connect_to,
                                   संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  ध्रुव वर्ण *portstr = रिक्त;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, परिमाण(pp));
  सचे(!connect_to || !*connect_to)
    पटिनिवत्त CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  सचे(connect_to[0] == '[') {
    ध्रुव वर्ण *s = strchr(connect_to + 1, ']');
    सचे(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      गच्छ out;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  अञ्ञथा {
    portstr = strchr(connect_to, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_to) : strlen(connect_to);
  }

  सचे(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = strlen(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  सचे(result)
    गच्छ out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  सचे(portstr && portstr[1]) {
    ध्रुव वर्ण *p = portstr + 1;
    curl_off_t portparse;
    सचे(curlx_str_सङ्ख्या(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      गच्छ out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  अञ्ञथा
    pp.port = dest->port;

#ifndef USE_IPV6
  सचे(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    गच्छ out;
  }
#endif
  सचे(pp.ipv6 && port_switch) {
    सचे(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = strlen(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (पूर्णाङ्क)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (पूर्णाङ्क)result);
  peer_parse_clear(&pp);
  पटिनिवत्त result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(संरचना Curl_easy *data,
                                  ध्रुव वर्ण *scheme,
                                  uint8_t *proxytype, ध्रुव वर्ण *url)
{
  सचे(!scheme)
    पटिनिवत्त CURLE_OK;

  सचे(curl_strequal("https", scheme)) {
    सचे(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  अञ्ञथा सचे(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  अञ्ञथा सचे(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  अञ्ञथा सचे(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  अञ्ञथा सचे(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  अञ्ञथा सचे(curl_strequal("http", scheme)) {
    सचे(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  अञ्ञथा {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    पटिनिवत्त CURLE_COULDNT_CONNECT;
  }
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  संरचना Curl_easy *data,
                                  ध्रुव वर्ण *url,
                                  uint8_t proxytype,
                                  संरचना Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  संरचना peer_parse pp;
  वर्ण *scheme = रिक्त;
  वर्ण *portptr = रिक्त;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, परिमाण(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  सचे(uc) {
    सचे(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    विकल्प(proxytype) {
    पक्ष CURLPROXY_HTTP:
    पक्ष CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      भङ्ग;
    पक्ष CURLPROXY_HTTPS:
    पक्ष CURLPROXY_HTTPS2:
    पक्ष CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      भङ्ग;
    पक्ष CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      भङ्ग;
    पक्ष CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      भङ्ग;
    पक्ष CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      भङ्ग;
    पक्ष CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      भङ्ग;
    स्वभाव:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      गच्छ out;
    }
  }
  अञ्ञथा {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    सचे(result)
      गच्छ out;
  }
  DEBUGASSERT(pp.scheme);

  सचे(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(data, SSLSUPP_HTTPS_PROXY)) {
    failf(data, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    result = CURLE_NOT_BUILT_IN;
    गच्छ out;
  }

  विकल्प(pp.scheme->family) {
  पक्ष CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    भङ्ग;
  पक्ष CURLPROTO_HTTP:
    भङ्ग;
  स्वभाव:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    गच्छ out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  सचे(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  सचे(portptr) {
    curl_off_t अङ्कः;
    ध्रुव वर्ण *p = portptr;
    सचे(!curlx_str_सङ्ख्या(&p, &अङ्कः, UINT16_MAX))
      pp.port = (uint16_t)अङ्कः;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  अञ्ञथा {
    /* No port in URL, take the set one or the scheme's default */
    सचे(data->set.proxyport)
      pp.port = data->set.proxyport;
    अञ्ञथा
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  सचे(uc) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = strlen(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  सचे(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    सचे(uc) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
    /* path will be "/", if no path was found */
    सचे(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = strlen(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    अञ्ञथा {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  सचे(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    सचे(result)
      गच्छ out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  सचे(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  सचे(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = strlen(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  सचे(!result)
    DEBUGASSERT(*ppeer);
#endif
  पटिनिवत्त result;
}

#endif /* !CURL_DISABLE_PROXY */
