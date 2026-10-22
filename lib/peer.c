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

#जर defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#जर defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
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
  स्थिरांक संरचना Curl_scheme *scheme;
  संरचना Curl_str host_user;
  संरचना Curl_str host;
  संरचना Curl_str zoneid;
  अक्षर *tmp_host_user;
  अक्षर *tmp_host;
  अक्षर *tmp_zoneid;
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
  स_रखो(pp, 0, आकार(*pp));
}

स्थिर CURLcode peer_create(संरचना peer_parse *pp,
                            संरचना Curl_peer **ppeer)
{
  संरचना Curl_peer *peer = रिक्त;
  CURLcode result = CURLE_OK;
  माप_प्रकार zone_alen = 0, host_alen = 0;

  जर(!pp || !pp->scheme)
    परत CURLE_FAILED_INIT;
  जर(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    परत CURLE_FAILED_INIT;

  जर((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, आकार(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  जर(!peer) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  जर(pp->host_user.len)
    स_नकल(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  जर(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    स_नकल(peer->hostname, pp->host.str, pp->host.len);
  }

  जर(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    स_नकल(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    जर(!peer->scopeid) {
      स्थिरांक अक्षर *p = peer->zoneid;
      curl_off_t scope;
      जर(!curlx_str_संख्या(&p, &scope, अच_पूर्णांक_उच्च) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      अन्यथा {
        /* Zone identifier is not fully numeric */
        अचिन्हित पूर्णांक idx = 0;
        idx = if_nametoindex(peer->zoneid);
        जर(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        अन्यथा {
          /* Do we want to return an error here? */
        }
      }
#endif /* HAVE_IF_NAMETOINDEX */
    }
#endif /* USE_IPV6 */
  }

out:
  जर(!result)
    *ppeer = peer;
  अन्यथा
    Curl_peer_unlink(&peer);
  परत result;
}

स्थिर CURLcode peer_parse_host(संरचना Curl_easy *data,
                                संरचना peer_parse *pp,
                                bool scan_for_ipv6)
{
  जर(!pp || !pp->host_user.str || !pp->host_user.len)
    परत CURLE_FAILED_INIT;

  जर(pp->host_user.str[0] == '[') {
    स्थिरांक अक्षर *s = pp->host_user.str + 1;
    संरचना Curl_str tmp;
    जर(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      परत CURLE_URL_MALFORMAT;

    जर(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (पूर्णांक)pp->host_user.len, pp->host_user.str);
      परत CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  अन्यथा {
#ifdef USE_IDN
    जर(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      जर(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        जर(!pp->tmp_host_user)
          परत CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      जर(result)
        परत result;
      pp->host.str = pp->tmp_host;
      pp->host.len = म_माप(pp->host.str);
    }
    अन्यथा
#endif
    जर(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      जर(pp->host_user.len < MAX_IPADR_LEN) {
        अक्षर tmp[MAX_IPADR_LEN];
        स_नकल(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      अन्यथा
        pp->ipv6 = TRUE;
    }
    अन्यथा
      pp->host = pp->host_user;
  }
  परत CURLE_OK;
}

CURLcode Curl_peer_create(संरचना Curl_easy *data,
                          स्थिरांक संरचना Curl_scheme *scheme,
                          स्थिरांक अक्षर *hostname,
                          uint16_t port,
                          संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, आकार(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = म_माप(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  जर(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  परत result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(स्थिरांक संरचना Curl_scheme *scheme,
                              स्थिरांक अक्षर *path,
                              bool abstract_unix_socket,
                              संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  माप_प्रकार pathlen = path ? म_माप(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, आकार(pp));
  जर(!scheme)
    परत CURLE_FAILED_INIT;
  जर(!pathlen)
    परत CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  परत result;
}
#endif /* USE_UNIX_SOCKETS */

शून्य Curl_peer_link(संरचना Curl_peer **pdest, संरचना Curl_peer *src)
{
  जर(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    जर(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

शून्य Curl_peer_unlink(संरचना Curl_peer **ppeer)
{
  जर(*ppeer) {
    संरचना Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = रिक्त;
    जर(peer->refcount)
      peer->refcount--;
    जर(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(संरचना Curl_peer *p1, संरचना Curl_peer *p2)
{
  परत (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

स्थिर bool peer_same_hostname(संरचना Curl_peer *p1, संरचना Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  परत (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!म_भेद(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

स्थिर bool peer_same_zoneid(स्थिरांक अक्षर *z1, स्थिरांक अक्षर *z2)
{
  परत (z1 == z2) ||
         (z1 && z2 && !म_भेद(z1, z2));
}

bool Curl_peer_same_destination(संरचना Curl_peer *p1, संरचना Curl_peer *p2)
{
  परत (p1 == p2) ||
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
  अक्षर *zoneid = रिक्त, *scheme = रिक्त, *hostname = रिक्त;
  CURLUcode uc;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, आकार(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  जर(uc)
    परत Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  जर(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    जा out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  जर(uc) {
    जर((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    अन्यथा {
      result = CURLE_OUT_OF_MEMORY;
      जा out;
    }
  }
  अन्यथा जर(म_माप(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    जा out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = म_माप(pp.host_user.str);
  जर(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    जर(result)
      जा out;
  }
  अन्यथा
    pp.host = pp.host_user;

  जर(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    अक्षर portbuf[16];
    curl_msnprintf(portbuf, आकार(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    जर(uc) {
      result = CURLE_OUT_OF_MEMORY;
      जा out;
    }
    अन्यथा
      pp.port = port_override;
  }
  अन्यथा {
    uc = Curl_url_get_port(uh, &pp.port);
    जर(uc) {
      जर(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        जा out;
      }
      अन्यथा जर(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        जा out;
      }
      /* no port ok when not a network scheme */
    }
  }

  जर(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  अन्यथा {
    जर(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      जा out;
    }
    जर(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = म_माप(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  जर(result)
    failf(data, "Error %d creating peer for %s:%u",
          (पूर्णांक)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  परत result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(संरचना Curl_easy *data,
                                   स्थिरांक संरचना Curl_peer *dest,
                                   स्थिरांक अक्षर *connect_to,
                                   संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  स्थिरांक अक्षर *portstr = रिक्त;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, आकार(pp));
  जर(!connect_to || !*connect_to)
    परत CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  जर(connect_to[0] == '[') {
    स्थिरांक अक्षर *s = म_अक्षर(connect_to + 1, ']');
    जर(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      जा out;
    }
    portstr = म_अक्षर(s, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  अन्यथा {
    portstr = म_अक्षर(connect_to, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = portstr ?
      (माप_प्रकार)(portstr - connect_to) : म_माप(connect_to);
  }

  जर(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = म_माप(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  जर(result)
    जा out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  जर(portstr && portstr[1]) {
    स्थिरांक अक्षर *p = portstr + 1;
    curl_off_t portparse;
    जर(curlx_str_संख्या(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      जा out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  अन्यथा
    pp.port = dest->port;

#ifndef USE_IPV6
  जर(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    जा out;
  }
#endif
  जर(pp.ipv6 && port_switch) {
    जर(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = म_माप(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (पूर्णांक)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (पूर्णांक)result);
  peer_parse_clear(&pp);
  परत result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(संरचना Curl_easy *data,
                                  स्थिरांक अक्षर *scheme,
                                  uint8_t *proxytype, स्थिरांक अक्षर *url)
{
  जर(!scheme)
    परत CURLE_OK;

  जर(curl_strequal("https", scheme)) {
    जर(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  अन्यथा जर(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  अन्यथा जर(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  अन्यथा जर(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  अन्यथा जर(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  अन्यथा जर(curl_strequal("http", scheme)) {
    जर(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  अन्यथा {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    परत CURLE_COULDNT_CONNECT;
  }
  परत CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  संरचना Curl_easy *data,
                                  स्थिरांक अक्षर *url,
                                  uint8_t proxytype,
                                  संरचना Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  संरचना peer_parse pp;
  अक्षर *scheme = रिक्त;
  अक्षर *portptr = रिक्त;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, आकार(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  जर(uc) {
    जर(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      जा out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    निवड(proxytype) {
    स्थिती CURLPROXY_HTTP:
    स्थिती CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      थांबा;
    स्थिती CURLPROXY_HTTPS:
    स्थिती CURLPROXY_HTTPS2:
    स्थिती CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      थांबा;
    स्थिती CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      थांबा;
    स्थिती CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      थांबा;
    स्थिती CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      थांबा;
    स्थिती CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      थांबा;
    पूर्वनिर्धारित:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      जा out;
    }
  }
  अन्यथा {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    जर(result)
      जा out;
  }
  DEBUGASSERT(pp.scheme);

  जर(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(data, SSLSUPP_HTTPS_PROXY)) {
    failf(data, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    result = CURLE_NOT_BUILT_IN;
    जा out;
  }

  निवड(pp.scheme->family) {
  स्थिती CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    थांबा;
  स्थिती CURLPROTO_HTTP:
    थांबा;
  पूर्वनिर्धारित:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    जा out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  जर(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  जर(portptr) {
    curl_off_t अंक;
    स्थिरांक अक्षर *p = portptr;
    जर(!curlx_str_संख्या(&p, &अंक, UINT16_MAX))
      pp.port = (uint16_t)अंक;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  अन्यथा {
    /* No port in URL, take the set one or the scheme's default */
    जर(data->set.proxyport)
      pp.port = data->set.proxyport;
    अन्यथा
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  जर(uc) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = म_माप(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  जर(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    जर(uc) {
      result = CURLE_OUT_OF_MEMORY;
      जा out;
    }
    /* path will be "/", if no path was found */
    जर(म_भेद("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = म_माप(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    अन्यथा {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  जर(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    जर(result)
      जा out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  जर(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  जर(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = म_माप(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  जर(!result)
    DEBUGASSERT(*ppeer);
#endif
  परत result;
}

#endif /* !CURL_DISABLE_PROXY */
