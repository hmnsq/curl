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
#شامل "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#شامل <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#شامل <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#endif

#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#شامل <sys/un.h>
#endif

#اگر defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#اگر defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#شامل <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#شامل <iphlpapi.h>
#endif

#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "protocol.h"
#شامل "http_proxy.h"
#شامل "idn.h"
#شامل "curlx/strdup.h"
#شامل "curlx/strparse.h"
#شامل "peer.h"
#شامل "urldata.h"
#شامل "url.h"
#شامل "urlapi-int.h"
#شامل "vtls/vtls.h"

ساختار peer_parse {
  ثابت ساختار Curl_scheme *scheme;
  ساختار Curl_str host_user;
  ساختار Curl_str host;
  ساختار Curl_str zoneid;
  نویسه *tmp_host_user;
  نویسه *tmp_host;
  نویسه *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

ایستا تهی peer_parse_clear(ساختار peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, اندازه(*pp));
}

ایستا CURLcode peer_create(ساختار peer_parse *pp,
                            ساختار Curl_peer **ppeer)
{
  ساختار Curl_peer *peer = پوچ;
  CURLcode result = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  اگر(!pp || !pp->scheme)
    بازگشت CURLE_FAILED_INIT;
  اگر(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    بازگشت CURLE_FAILED_INIT;

  اگر((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, اندازه(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  اگر(!peer) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  اگر(pp->host_user.len)
    memcpy(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  اگر(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    memcpy(peer->hostname, pp->host.str, pp->host.len);
  }

  اگر(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    memcpy(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    اگر(!peer->scopeid) {
      ثابت نویسه *p = peer->zoneid;
      curl_off_t scope;
      اگر(!curlx_str_عدد(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      وگرنه {
        /* Zone identifier is not fully numeric */
        unsigned صحیح idx = 0;
        idx = if_nametoindex(peer->zoneid);
        اگر(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        وگرنه {
          /* Do we want to return an error here? */
        }
      }
#endif /* HAVE_IF_NAMETOINDEX */
    }
#endif /* USE_IPV6 */
  }

out:
  اگر(!result)
    *ppeer = peer;
  وگرنه
    Curl_peer_unlink(&peer);
  بازگشت result;
}

ایستا CURLcode peer_parse_host(ساختار Curl_easy *data,
                                ساختار peer_parse *pp,
                                bool scan_for_ipv6)
{
  اگر(!pp || !pp->host_user.str || !pp->host_user.len)
    بازگشت CURLE_FAILED_INIT;

  اگر(pp->host_user.str[0] == '[') {
    ثابت نویسه *s = pp->host_user.str + 1;
    ساختار Curl_str tmp;
    اگر(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      بازگشت CURLE_URL_MALFORMAT;

    اگر(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (صحیح)pp->host_user.len, pp->host_user.str);
      بازگشت CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  وگرنه {
#ifdef USE_IDN
    اگر(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      اگر(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        اگر(!pp->tmp_host_user)
          بازگشت CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      اگر(result)
        بازگشت result;
      pp->host.str = pp->tmp_host;
      pp->host.len = strlen(pp->host.str);
    }
    وگرنه
#endif
    اگر(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      اگر(pp->host_user.len < MAX_IPADR_LEN) {
        نویسه tmp[MAX_IPADR_LEN];
        memcpy(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      وگرنه
        pp->ipv6 = TRUE;
    }
    وگرنه
      pp->host = pp->host_user;
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_peer_create(ساختار Curl_easy *data,
                          ثابت ساختار Curl_scheme *scheme,
                          ثابت نویسه *hostname,
                          uint16_t port,
                          ساختار Curl_peer **ppeer)
{
  ساختار peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, اندازه(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = strlen(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  اگر(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  بازگشت result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ثابت ساختار Curl_scheme *scheme,
                              ثابت نویسه *path,
                              bool abstract_unix_socket,
                              ساختار Curl_peer **ppeer)
{
  ساختار peer_parse pp;
  size_t pathlen = path ? strlen(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, اندازه(pp));
  اگر(!scheme)
    بازگشت CURLE_FAILED_INIT;
  اگر(!pathlen)
    بازگشت CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  بازگشت result;
}
#endif /* USE_UNIX_SOCKETS */

تهی Curl_peer_link(ساختار Curl_peer **pdest, ساختار Curl_peer *src)
{
  اگر(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    اگر(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

تهی Curl_peer_unlink(ساختار Curl_peer **ppeer)
{
  اگر(*ppeer) {
    ساختار Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = پوچ;
    اگر(peer->refcount)
      peer->refcount--;
    اگر(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(ساختار Curl_peer *p1, ساختار Curl_peer *p2)
{
  بازگشت (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

ایستا bool peer_same_hostname(ساختار Curl_peer *p1, ساختار Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  بازگشت (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

ایستا bool peer_same_zoneid(ثابت نویسه *z1, ثابت نویسه *z2)
{
  بازگشت (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(ساختار Curl_peer *p1, ساختار Curl_peer *p2)
{
  بازگشت (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, ساختار Curl_easy *data,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            ساختار Curl_peer **ppeer)
{
  ساختار peer_parse pp;
  نویسه *zoneid = پوچ, *scheme = پوچ, *hostname = پوچ;
  CURLUcode uc;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, اندازه(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  اگر(uc)
    بازگشت Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  اگر(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    برو out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  اگر(uc) {
    اگر((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    وگرنه {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
  }
  وگرنه اگر(strlen(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    برو out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = strlen(pp.host_user.str);
  اگر(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    اگر(result)
      برو out;
  }
  وگرنه
    pp.host = pp.host_user;

  اگر(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    نویسه portbuf[16];
    curl_msnprintf(portbuf, اندازه(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    اگر(uc) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
    وگرنه
      pp.port = port_override;
  }
  وگرنه {
    uc = Curl_url_get_port(uh, &pp.port);
    اگر(uc) {
      اگر(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
      وگرنه اگر(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        برو out;
      }
      /* no port ok when not a network scheme */
    }
  }

  اگر(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  وگرنه {
    اگر(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
    اگر(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = strlen(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  اگر(result)
    failf(data, "Error %d creating peer for %s:%u",
          (صحیح)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  بازگشت result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(ساختار Curl_easy *data,
                                   ثابت ساختار Curl_peer *dest,
                                   ثابت نویسه *connect_to,
                                   ساختار Curl_peer **ppeer)
{
  ساختار peer_parse pp;
  ثابت نویسه *portstr = پوچ;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, اندازه(pp));
  اگر(!connect_to || !*connect_to)
    بازگشت CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  اگر(connect_to[0] == '[') {
    ثابت نویسه *s = strchr(connect_to + 1, ']');
    اگر(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      برو out;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  وگرنه {
    portstr = strchr(connect_to, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_to) : strlen(connect_to);
  }

  اگر(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = strlen(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  اگر(result)
    برو out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  اگر(portstr && portstr[1]) {
    ثابت نویسه *p = portstr + 1;
    curl_off_t portparse;
    اگر(curlx_str_عدد(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      برو out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  وگرنه
    pp.port = dest->port;

#ifndef USE_IPV6
  اگر(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    برو out;
  }
#endif
  اگر(pp.ipv6 && port_switch) {
    اگر(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = strlen(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (صحیح)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (صحیح)result);
  peer_parse_clear(&pp);
  بازگشت result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(ساختار Curl_easy *data,
                                  ثابت نویسه *scheme,
                                  uint8_t *proxytype, ثابت نویسه *url)
{
  اگر(!scheme)
    بازگشت CURLE_OK;

  اگر(curl_strequal("https", scheme)) {
    اگر(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  وگرنه اگر(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  وگرنه اگر(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  وگرنه اگر(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  وگرنه اگر(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  وگرنه اگر(curl_strequal("http", scheme)) {
    اگر(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  وگرنه {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    بازگشت CURLE_COULDNT_CONNECT;
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  ساختار Curl_easy *data,
                                  ثابت نویسه *url,
                                  uint8_t proxytype,
                                  ساختار Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  ساختار peer_parse pp;
  نویسه *scheme = پوچ;
  نویسه *portptr = پوچ;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, اندازه(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  اگر(uc) {
    اگر(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    انتخاب(proxytype) {
    حالت CURLPROXY_HTTP:
    حالت CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      بشکن;
    حالت CURLPROXY_HTTPS:
    حالت CURLPROXY_HTTPS2:
    حالت CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      بشکن;
    حالت CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      بشکن;
    حالت CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      بشکن;
    حالت CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      بشکن;
    حالت CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      بشکن;
    پیشفرض:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      برو out;
    }
  }
  وگرنه {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    اگر(result)
      برو out;
  }
  DEBUGASSERT(pp.scheme);

  اگر(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(data, SSLSUPP_HTTPS_PROXY)) {
    failf(data, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    result = CURLE_NOT_BUILT_IN;
    برو out;
  }

  انتخاب(pp.scheme->family) {
  حالت CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    بشکن;
  حالت CURLPROTO_HTTP:
    بشکن;
  پیشفرض:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    برو out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  اگر(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  اگر(portptr) {
    curl_off_t رقم;
    ثابت نویسه *p = portptr;
    اگر(!curlx_str_عدد(&p, &رقم, UINT16_MAX))
      pp.port = (uint16_t)رقم;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  وگرنه {
    /* No port in URL, take the set one or the scheme's default */
    اگر(data->set.proxyport)
      pp.port = data->set.proxyport;
    وگرنه
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  اگر(uc) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = strlen(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  اگر(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    اگر(uc) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
    /* path will be "/", if no path was found */
    اگر(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = strlen(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    وگرنه {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  اگر(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    اگر(result)
      برو out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  اگر(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  اگر(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = strlen(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  اگر(!result)
    DEBUGASSERT(*ppeer);
#endif
  بازگشت result;
}

#endif /* !CURL_DISABLE_PROXY */
