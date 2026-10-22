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

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NET_IF_H
#شامل <net/if.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_IPHLPAPI_H
#شامل <Iphlpapi.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#اگر_ختم

#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SYS_UN_H
#شامل <sys/un.h>
#اگر_ختم

#اگر defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#اگر defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#شامل <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#اگر_ختم
#شامل <iphlpapi.h>
#اگر_ختم

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

ساخت peer_parse {
  ثابت ساخت Curl_scheme *scheme;
  ساخت Curl_str host_user;
  ساخت Curl_str host;
  ساخت Curl_str zoneid;
  حرف *tmp_host_user;
  حرف *tmp_host;
  حرف *tmp_zoneid;
  uint32_t scopeid;
  uint16_t پورٹ;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

ساکن خالی peer_parse_clear(ساخت peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, حجم(*pp));
}

ساکن CURLcode peer_create(ساخت peer_parse *pp,
                            ساخت Curl_peer **ppeer)
{
  ساخت Curl_peer *peer = تہی;
  CURLcode نتیجہ = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  اگر(!pp || !pp->scheme)
    واپس CURLE_FAILED_INIT;
  اگر(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    واپس CURLE_FAILED_INIT;

  اگر((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, حجم(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  اگر(!peer) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->پورٹ = pp->پورٹ;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  اگر(pp->host_user.len)
    حافظہ_نقل(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  اگر(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    حافظہ_نقل(peer->hostname, pp->host.str, pp->host.len);
  }

  اگر(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    حافظہ_نقل(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#اگر_تعریف_شدہ USE_IPV6
    /* Determine scope_id if not already provided */
    اگر(!peer->scopeid) {
      ثابت حرف *p = peer->zoneid;
      curl_off_t scope;
      اگر(!curlx_str_نمبر(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#اگر_تعریف_شدہ HAVE_IF_NAMETOINDEX
      ورنہ {
        /* Zone identifier is not fully numeric */
        غیرعلامتی صحیح idx = 0;
        idx = if_nametoindex(peer->zoneid);
        اگر(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        ورنہ {
          /* Do we want to return an error here? */
        }
      }
#اگر_ختم /* HAVE_IF_NAMETOINDEX */
    }
#اگر_ختم /* USE_IPV6 */
  }

برآمد:
  اگر(!نتیجہ)
    *ppeer = peer;
  ورنہ
    Curl_peer_unlink(&peer);
  واپس نتیجہ;
}

ساکن CURLcode peer_parse_host(ساخت Curl_easy *ڈیٹا,
                                ساخت peer_parse *pp,
                                bool جائزہ_for_ipv6)
{
  اگر(!pp || !pp->host_user.str || !pp->host_user.len)
    واپس CURLE_FAILED_INIT;

  اگر(pp->host_user.str[0] == '[') {
    ثابت حرف *s = pp->host_user.str + 1;
    ساخت Curl_str tmp;
    اگر(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      واپس CURLE_URL_MALFORMAT;

    اگر(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(ڈیٹا, "Invalid IPv6 address format in '%.*s'",
            (صحیح)pp->host_user.len, pp->host_user.str);
      واپس CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  ورنہ {
#اگر_تعریف_شدہ USE_IDN
    اگر(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode نتیجہ;
      اگر(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        اگر(!pp->tmp_host_user)
          واپس CURLE_برآمد_کا_حافظہ;
      }
      نتیجہ = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      اگر(نتیجہ)
        واپس نتیجہ;
      pp->host.str = pp->tmp_host;
      pp->host.len = لڑی_لمبائی(pp->host.str);
    }
    ورنہ
#اگر_ختم
    اگر(جائزہ_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      اگر(pp->host_user.len < MAX_IPADR_LEN) {
        حرف tmp[MAX_IPADR_LEN];
        حافظہ_نقل(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      ورنہ
        pp->ipv6 = TRUE;
    }
    ورنہ
      pp->host = pp->host_user;
  }
  واپس CURLE_OK;
}

CURLcode Curl_peer_create(ساخت Curl_easy *ڈیٹا,
                          ثابت ساخت Curl_scheme *scheme,
                          ثابت حرف *hostname,
                          uint16_t پورٹ,
                          ساخت Curl_peer **ppeer)
{
  ساخت peer_parse pp;
  CURLcode نتیجہ;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = لڑی_لمبائی(hostname);
  pp.پورٹ = پورٹ;

  نتیجہ = peer_parse_host(ڈیٹا, &pp, TRUE);
  اگر(!نتیجہ)
    نتیجہ = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  واپس نتیجہ;
}

#اگر_تعریف_شدہ USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ثابت ساخت Curl_scheme *scheme,
                              ثابت حرف *راستہ,
                              bool abstract_unix_socket,
                              ساخت Curl_peer **ppeer)
{
  ساخت peer_parse pp;
  size_t pathlen = راستہ ? لڑی_لمبائی(راستہ) : 0;
  CURLcode نتیجہ = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  اگر(!scheme)
    واپس CURLE_FAILED_INIT;
  اگر(!pathlen)
    واپس CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = راستہ;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  نتیجہ = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  واپس نتیجہ;
}
#اگر_ختم /* USE_UNIX_SOCKETS */

خالی Curl_peer_link(ساخت Curl_peer **pdest, ساخت Curl_peer *src)
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

خالی Curl_peer_unlink(ساخت Curl_peer **ppeer)
{
  اگر(*ppeer) {
    ساخت Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = تہی;
    اگر(peer->refcount)
      peer->refcount--;
    اگر(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(ساخت Curl_peer *p1, ساخت Curl_peer *p2)
{
  واپس (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

ساکن bool peer_same_hostname(ساخت Curl_peer *p1, ساخت Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  واپس (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

ساکن bool peer_same_zoneid(ثابت حرف *z1, ثابت حرف *z2)
{
  واپس (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(ساخت Curl_peer *p1, ساخت Curl_peer *p2)
{
  واپس (p1 == p2) ||
         (p1 && p2 &&
          (p1->پورٹ == p2->پورٹ) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, ساخت Curl_easy *ڈیٹا,
                            uint16_t پورٹ_override,
                            uint32_t scopeid_override,
                            ساخت Curl_peer **ppeer)
{
  ساخت peer_parse pp;
  حرف *zoneid = تہی, *scheme = تہی, *hostname = تہی;
  CURLUcode uc;
  CURLcode نتیجہ;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  اگر(uc)
    واپس Curl_uc_مقصد_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  اگر(!pp.scheme) {
    failf(ڈیٹا, "Protocol \"%s\" not supported%s", scheme,
          ڈیٹا->state.this_is_a_follow ? " (in redirect)" : "");
    نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
    جاؤ برآمد;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  اگر(uc) {
    اگر((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    ورنہ {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }
  ورنہ اگر(لڑی_لمبائی(hostname) > MAX_URL_LEN) {
    failf(ڈیٹا, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    نتیجہ = CURLE_URL_MALFORMAT;
    جاؤ برآمد;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = لڑی_لمبائی(pp.host_user.str);
  اگر(pp.host_user.len) {
    نتیجہ = peer_parse_host(ڈیٹا, &pp, FALSE);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  ورنہ
    pp.host = pp.host_user;

  اگر(پورٹ_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    حرف portbuf[16];
    curl_msnprintf(portbuf, حجم(portbuf), "%d", پورٹ_override);
    uc = curl_url_سیٹ(uh, CURLUPART_پورٹ, portbuf, 0);
    اگر(uc) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
    ورنہ
      pp.پورٹ = پورٹ_override;
  }
  ورنہ {
    uc = Curl_url_get_پورٹ(uh, &pp.پورٹ);
    اگر(uc) {
      اگر(uc == CURLUE_برآمد_کا_حافظہ) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
      ورنہ اگر(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        نتیجہ = CURLE_URL_MALFORMAT;
        جاؤ برآمد;
      }
      /* no port ok when not a network scheme */
    }
  }

  اگر(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  ورنہ {
    اگر(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_برآمد_کا_حافظہ) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
    اگر(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = لڑی_لمبائی(zoneid);
    }
  }

  نتیجہ = peer_create(&pp, ppeer);
  اگر(نتیجہ)
    failf(ڈیٹا, "Error %d creating peer for %s:%u",
          (صحیح)نتیجہ, pp.host_user.str, pp.پورٹ);

برآمد:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  واپس نتیجہ;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_مقصد(ساخت Curl_easy *ڈیٹا,
                                   ثابت ساخت Curl_peer *dest,
                                   ثابت حرف *connect_مقصد,
                                   ساخت Curl_peer **ppeer)
{
  ساخت peer_parse pp;
  ثابت حرف *portstr = تہی;
  bool پورٹ_switch = FALSE;
  CURLcode نتیجہ;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  اگر(!connect_مقصد || !*connect_مقصد)
    واپس CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  اگر(connect_مقصد[0] == '[') {
    ثابت حرف *s = strchr(connect_مقصد + 1, ']');
    اگر(!s) {
      failf(ڈیٹا, "Invalid IPv6 address format in '%s'", connect_مقصد);
      نتیجہ = CURLE_SETOPT_OPTION_SYNTAX;
      جاؤ برآمد;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_مقصد;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  ورنہ {
    portstr = strchr(connect_مقصد, ':');
    pp.host_user.str = connect_مقصد;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_مقصد) : لڑی_لمبائی(connect_مقصد);
  }

  اگر(!pp.host_user.len) { /* no hostname found, only port switch */
    پورٹ_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = لڑی_لمبائی(dest->user_hostname);
  }

  نتیجہ = peer_parse_host(ڈیٹا, &pp, FALSE);
  اگر(نتیجہ)
    جاؤ برآمد;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!پورٹ_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  اگر(portstr && portstr[1]) {
    ثابت حرف *p = portstr + 1;
    curl_off_t portparse;
    اگر(curlx_str_نمبر(&p, &portparse, 0xffff)) {
      failf(ڈیٹا, "No valid port number in '%s'", connect_مقصد);
      نتیجہ = CURLE_SETOPT_OPTION_SYNTAX;
      جاؤ برآمد;
    }
    pp.پورٹ = (uint16_t)portparse; /* we know it will fit */
  }
  ورنہ
    pp.پورٹ = dest->پورٹ;

#اگر_غیر_تعریف_شدہ USE_IPV6
  اگر(pp.ipv6) {
    failf(ڈیٹا, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    نتیجہ = CURLE_NOT_BUILT_اندر;
    جاؤ برآمد;
  }
#اگر_ختم
  اگر(pp.ipv6 && پورٹ_switch) {
    اگر(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = لڑی_لمبائی(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  نتیجہ = peer_create(&pp, ppeer);
  CURL_TRC_M(ڈیٹا, "connect-to peer_create2 -> %d", (صحیح)نتیجہ);

برآمد:
  CURL_TRC_M(ڈیٹا, "parse connect_to peer: %s -> %d", connect_مقصد, (صحیح)نتیجہ);
  peer_parse_clear(&pp);
  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY

#اگر_تعریف_شدہ USE_UNIX_SOCKETS
#تعریف_کرو UNIX_SOCKET_سابقہ "localhost"
#اگر_ختم

CURLcode Curl_scheme_مقصد_proxytype(ساخت Curl_easy *ڈیٹا,
                                  ثابت حرف *scheme,
                                  uint8_t *proxytype, ثابت حرف *url)
{
  اگر(!scheme)
    واپس CURLE_OK;

  اگر(curl_strequal("https", scheme)) {
    اگر(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  ورنہ اگر(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  ورنہ اگر(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  ورنہ اگر(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  ورنہ اگر(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  ورنہ اگر(curl_strequal("http", scheme)) {
    اگر(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  ورنہ {
    /* Any other xxx:// reject! */
    failf(ڈیٹا, "Unsupported proxy scheme for \'%s\'", url);
    واپس CURLE_COULDNT_CONNECT;
  }
  واپس CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  ساخت Curl_easy *ڈیٹا,
                                  ثابت حرف *url,
                                  uint8_t proxytype,
                                  ساخت Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  ساخت peer_parse pp;
  حرف *scheme = تہی;
  حرف *portptr = تہی;
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#اگر_ختم
  CURLUcode uc;
  CURLcode نتیجہ = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  pp.پورٹ = CURL_DEFAULT_PROXY_پورٹ;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  اگر(uc) {
    اگر(uc == CURLUE_برآمد_کا_حافظہ) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    انتخاب(proxytype) {
    حالت CURLPROXY_HTTP:
    حالت CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      توڑو;
    حالت CURLPROXY_HTTPS:
    حالت CURLPROXY_HTTPS2:
    حالت CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      توڑو;
    حالت CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      توڑو;
    حالت CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      توڑو;
    حالت CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      توڑو;
    حالت CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      توڑو;
    طےشدہ:
      failf(ڈیٹا, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      نتیجہ = CURLE_COULDNT_RESOLVE_PROXY;
      جاؤ برآمد;
    }
  }
  ورنہ {
    pp.scheme = Curl_get_scheme(scheme);
    نتیجہ = Curl_scheme_مقصد_proxytype(ڈیٹا, scheme, &proxytype, url);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  DEBUGASSERT(pp.scheme);

  اگر(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(ڈیٹا, SSLSUPP_HTTPS_PROXY)) {
    failf(ڈیٹا, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    نتیجہ = CURLE_NOT_BUILT_اندر;
    جاؤ برآمد;
  }

  انتخاب(pp.scheme->family) {
  حالت CURLPROTO_SOCKS:
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
    is_socks = TRUE;
#اگر_ختم
    توڑو;
  حالت CURLPROTO_HTTP:
    توڑو;
  طےشدہ:
    failf(ڈیٹا, "Unsupported proxy protocol for \'%s\'", url);
    نتیجہ = CURLE_COULDNT_CONNECT;
    جاؤ برآمد;
  }

  uc = curl_url_get(uh, CURLUPART_پورٹ, &portptr, CURLU_NO_DEFAULT_پورٹ);
  اگر(uc == CURLUE_برآمد_کا_حافظہ) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  اگر(portptr) {
    curl_off_t عدد;
    ثابت حرف *p = portptr;
    اگر(!curlx_str_نمبر(&p, &عدد, UINT16_MAX))
      pp.پورٹ = (uint16_t)عدد;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  ورنہ {
    /* No port in URL, take the set one or the scheme's default */
    اگر(ڈیٹا->سیٹ.proxyport)
      pp.پورٹ = ڈیٹا->سیٹ.proxyport;
    ورنہ
      pp.پورٹ = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  اگر(uc) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = لڑی_لمبائی(pp.tmp_host_user);

#اگر_تعریف_شدہ USE_UNIX_SOCKETS
  اگر(is_socks && curl_strequal(UNIX_SOCKET_سابقہ, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_راستہ, &pp.tmp_host, CURLU_URLDECODE);
    اگر(uc) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
    /* path will be "/", if no path was found */
    اگر(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = لڑی_لمبائی(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    ورنہ {
      pp.host = pp.host_user;
    }
  }
#اگر_ختم /* USE_UNIX_SOCKETS */

  اگر(!pp.host.len) {
    نتیجہ = peer_parse_host(ڈیٹا, &pp, FALSE);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  اگر(uc == CURLUE_برآمد_کا_حافظہ) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  اگر(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = لڑی_لمبائی(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  نتیجہ = peer_create(&pp, ppeer);

برآمد:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#اگر_تعریف_شدہ DEBUGBUILD
  اگر(!نتیجہ)
    DEBUGASSERT(*ppeer);
#اگر_ختم
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_PROXY */
