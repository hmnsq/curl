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

#यदि defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#यदि defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
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
  अचर संरचना Curl_scheme *scheme;
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
  स_रखो(pp, 0, परिमाण(*pp));
}

स्थिर CURLcode peer_create(संरचना peer_parse *pp,
                            संरचना Curl_peer **ppeer)
{
  संरचना Curl_peer *peer = रिक्त;
  CURLcode result = CURLE_OK;
  माप_प्रकार zone_alen = 0, host_alen = 0;

  यदि(!pp || !pp->scheme)
    फर्काऊ CURLE_FAILED_INIT;
  यदि(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    फर्काऊ CURLE_FAILED_INIT;

  यदि((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, परिमाण(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  यदि(!peer) {
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

  यदि(pp->host_user.len)
    स_नकल(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  यदि(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    स_नकल(peer->hostname, pp->host.str, pp->host.len);
  }

  यदि(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    स_नकल(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    यदि(!peer->scopeid) {
      अचर वर्ण *p = peer->zoneid;
      curl_off_t scope;
      यदि(!curlx_str_सङ्ख्या(&p, &scope, अच_पूर्णांक_उच्च) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      अन्यथा {
        /* Zone identifier is not fully numeric */
        अचिन्हित पूर्णाङ्क idx = 0;
        idx = if_nametoindex(peer->zoneid);
        यदि(idx) {
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
  यदि(!result)
    *ppeer = peer;
  अन्यथा
    Curl_peer_unlink(&peer);
  फर्काऊ result;
}

स्थिर CURLcode peer_parse_host(संरचना Curl_easy *data,
                                संरचना peer_parse *pp,
                                bool scan_for_ipv6)
{
  यदि(!pp || !pp->host_user.str || !pp->host_user.len)
    फर्काऊ CURLE_FAILED_INIT;

  यदि(pp->host_user.str[0] == '[') {
    अचर वर्ण *s = pp->host_user.str + 1;
    संरचना Curl_str tmp;
    यदि(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      फर्काऊ CURLE_URL_MALFORMAT;

    यदि(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (पूर्णाङ्क)pp->host_user.len, pp->host_user.str);
      फर्काऊ CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  अन्यथा {
#ifdef USE_IDN
    यदि(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      यदि(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        यदि(!pp->tmp_host_user)
          फर्काऊ CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      यदि(result)
        फर्काऊ result;
      pp->host.str = pp->tmp_host;
      pp->host.len = म_माप(pp->host.str);
    }
    अन्यथा
#endif
    यदि(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      यदि(pp->host_user.len < MAX_IPADR_LEN) {
        वर्ण tmp[MAX_IPADR_LEN];
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
  फर्काऊ CURLE_OK;
}

CURLcode Curl_peer_create(संरचना Curl_easy *data,
                          अचर संरचना Curl_scheme *scheme,
                          अचर वर्ण *hostname,
                          uint16_t port,
                          संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, परिमाण(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = म_माप(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  यदि(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  फर्काऊ result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(अचर संरचना Curl_scheme *scheme,
                              अचर वर्ण *path,
                              bool abstract_unix_socket,
                              संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  माप_प्रकार pathlen = path ? म_माप(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, परिमाण(pp));
  यदि(!scheme)
    फर्काऊ CURLE_FAILED_INIT;
  यदि(!pathlen)
    फर्काऊ CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  फर्काऊ result;
}
#endif /* USE_UNIX_SOCKETS */

शून्य Curl_peer_link(संरचना Curl_peer **pdest, संरचना Curl_peer *src)
{
  यदि(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    यदि(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

शून्य Curl_peer_unlink(संरचना Curl_peer **ppeer)
{
  यदि(*ppeer) {
    संरचना Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = रिक्त;
    यदि(peer->refcount)
      peer->refcount--;
    यदि(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(संरचना Curl_peer *p1, संरचना Curl_peer *p2)
{
  फर्काऊ (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

स्थिर bool peer_same_hostname(संरचना Curl_peer *p1, संरचना Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  फर्काऊ (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!म_भेद(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

स्थिर bool peer_same_zoneid(अचर वर्ण *z1, अचर वर्ण *z2)
{
  फर्काऊ (z1 == z2) ||
         (z1 && z2 && !म_भेद(z1, z2));
}

bool Curl_peer_same_destination(संरचना Curl_peer *p1, संरचना Curl_peer *p2)
{
  फर्काऊ (p1 == p2) ||
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
  स_रखो(&pp, 0, परिमाण(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  यदि(uc)
    फर्काऊ Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  यदि(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    गच्छ out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  यदि(uc) {
    यदि((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    अन्यथा {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
  }
  अन्यथा यदि(म_माप(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    गच्छ out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = म_माप(pp.host_user.str);
  यदि(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    यदि(result)
      गच्छ out;
  }
  अन्यथा
    pp.host = pp.host_user;

  यदि(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    वर्ण portbuf[16];
    curl_msnprintf(portbuf, परिमाण(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    यदि(uc) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
    अन्यथा
      pp.port = port_override;
  }
  अन्यथा {
    uc = Curl_url_get_port(uh, &pp.port);
    यदि(uc) {
      यदि(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
      अन्यथा यदि(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        गच्छ out;
      }
      /* no port ok when not a network scheme */
    }
  }

  यदि(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  अन्यथा {
    यदि(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
    यदि(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = म_माप(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  यदि(result)
    failf(data, "Error %d creating peer for %s:%u",
          (पूर्णाङ्क)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  फर्काऊ result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(संरचना Curl_easy *data,
                                   अचर संरचना Curl_peer *dest,
                                   अचर वर्ण *connect_to,
                                   संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  अचर वर्ण *portstr = रिक्त;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, परिमाण(pp));
  यदि(!connect_to || !*connect_to)
    फर्काऊ CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  यदि(connect_to[0] == '[') {
    अचर वर्ण *s = म_अक्षर(connect_to + 1, ']');
    यदि(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      गच्छ out;
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

  यदि(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = म_माप(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  यदि(result)
    गच्छ out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  यदि(portstr && portstr[1]) {
    अचर वर्ण *p = portstr + 1;
    curl_off_t portparse;
    यदि(curlx_str_सङ्ख्या(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      गच्छ out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  अन्यथा
    pp.port = dest->port;

#ifndef USE_IPV6
  यदि(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    गच्छ out;
  }
#endif
  यदि(pp.ipv6 && port_switch) {
    यदि(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = म_माप(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (पूर्णाङ्क)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (पूर्णाङ्क)result);
  peer_parse_clear(&pp);
  फर्काऊ result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(संरचना Curl_easy *data,
                                  अचर वर्ण *scheme,
                                  uint8_t *proxytype, अचर वर्ण *url)
{
  यदि(!scheme)
    फर्काऊ CURLE_OK;

  यदि(curl_strequal("https", scheme)) {
    यदि(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  अन्यथा यदि(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  अन्यथा यदि(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  अन्यथा यदि(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  अन्यथा यदि(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  अन्यथा यदि(curl_strequal("http", scheme)) {
    यदि(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  अन्यथा {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    फर्काऊ CURLE_COULDNT_CONNECT;
  }
  फर्काऊ CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  संरचना Curl_easy *data,
                                  अचर वर्ण *url,
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
  स_रखो(&pp, 0, परिमाण(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  यदि(uc) {
    यदि(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    विकल्प(proxytype) {
    पक्ष CURLPROXY_HTTP:
    पक्ष CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      रोक;
    पक्ष CURLPROXY_HTTPS:
    पक्ष CURLPROXY_HTTPS2:
    पक्ष CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      रोक;
    पक्ष CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      रोक;
    पक्ष CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      रोक;
    पक्ष CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      रोक;
    पक्ष CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      रोक;
    स्वभाव:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      गच्छ out;
    }
  }
  अन्यथा {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    यदि(result)
      गच्छ out;
  }
  DEBUGASSERT(pp.scheme);

  यदि(CURL_PROXY_IS_HTTPS(proxytype) &&
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
    रोक;
  पक्ष CURLPROTO_HTTP:
    रोक;
  स्वभाव:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    गच्छ out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  यदि(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  यदि(portptr) {
    curl_off_t अङ्क;
    अचर वर्ण *p = portptr;
    यदि(!curlx_str_सङ्ख्या(&p, &अङ्क, UINT16_MAX))
      pp.port = (uint16_t)अङ्क;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  अन्यथा {
    /* No port in URL, take the set one or the scheme's default */
    यदि(data->set.proxyport)
      pp.port = data->set.proxyport;
    अन्यथा
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  यदि(uc) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = म_माप(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  यदि(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    यदि(uc) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
    /* path will be "/", if no path was found */
    यदि(म_भेद("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = म_माप(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    अन्यथा {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  यदि(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    यदि(result)
      गच्छ out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  यदि(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  यदि(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = म_माप(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  यदि(!result)
    DEBUGASSERT(*ppeer);
#endif
  फर्काऊ result;
}

#endif /* !CURL_DISABLE_PROXY */
