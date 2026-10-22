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
#ضمن "curl_setup.h"

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NET_IF_H
#ضمن <net/if.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_IPHLPAPI_H
#ضمن <Iphlpapi.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#پایان_اگر

#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_SYS_UN_H
#ضمن <sys/un.h>
#پایان_اگر

#إذا defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ضمن <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#پایان_اگر
#ضمن <iphlpapi.h>
#پایان_اگر

#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "protocol.h"
#ضمن "http_proxy.h"
#ضمن "idn.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/strparse.h"
#ضمن "peer.h"
#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "urlapi-int.h"
#ضمن "vtls/vtls.h"

بنية peer_parse {
  ثابت بنية Curl_scheme *scheme;
  بنية Curl_str host_user;
  بنية Curl_str host;
  بنية Curl_str zoneid;
  حرف *tmp_host_user;
  حرف *tmp_host;
  حرف *tmp_zoneid;
  uint32_t scopeid;
  uint16_t پورت;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

ساكن فراغ peer_parse_clear(بنية peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, حجم(*pp));
}

ساكن CURLcode peer_create(بنية peer_parse *pp,
                            بنية Curl_peer **ppeer)
{
  بنية Curl_peer *peer = عدم;
  CURLcode نتیجه = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  إذا(!pp || !pp->scheme)
    أرجع CURLE_FAILED_INIT;
  إذا(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    أرجع CURLE_FAILED_INIT;

  إذا((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, حجم(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  إذا(!peer) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->پورت = pp->پورت;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  إذا(pp->host_user.len)
    کپی_حافظه(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  إذا(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    کپی_حافظه(peer->hostname, pp->host.str, pp->host.len);
  }

  إذا(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    کپی_حافظه(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#اگر_تعریف_شده USE_IPV6
    /* Determine scope_id if not already provided */
    إذا(!peer->scopeid) {
      ثابت حرف *p = peer->zoneid;
      curl_off_t scope;
      إذا(!curlx_str_عدد(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#اگر_تعریف_شده HAVE_IF_NAMETOINDEX
      وإلا {
        /* Zone identifier is not fully numeric */
        بیعلامت صحيح idx = 0;
        idx = if_nametoindex(peer->zoneid);
        إذا(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        وإلا {
          /* Do we want to return an error here? */
        }
      }
#پایان_اگر /* HAVE_IF_NAMETOINDEX */
    }
#پایان_اگر /* USE_IPV6 */
  }

بیرون:
  إذا(!نتیجه)
    *ppeer = peer;
  وإلا
    Curl_peer_unlink(&peer);
  أرجع نتیجه;
}

ساكن CURLcode peer_parse_host(بنية Curl_easy *داده,
                                بنية peer_parse *pp,
                                bool پیمایش_for_ipv6)
{
  إذا(!pp || !pp->host_user.str || !pp->host_user.len)
    أرجع CURLE_FAILED_INIT;

  إذا(pp->host_user.str[0] == '[') {
    ثابت حرف *s = pp->host_user.str + 1;
    بنية Curl_str tmp;
    إذا(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      أرجع CURLE_URL_MALFORMAT;

    إذا(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(داده, "Invalid IPv6 address format in '%.*s'",
            (صحيح)pp->host_user.len, pp->host_user.str);
      أرجع CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  وإلا {
#اگر_تعریف_شده USE_IDN
    إذا(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode نتیجه;
      إذا(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        إذا(!pp->tmp_host_user)
          أرجع CURLE_بیرون_از_حافظه;
      }
      نتیجه = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      إذا(نتیجه)
        أرجع نتیجه;
      pp->host.str = pp->tmp_host;
      pp->host.len = طول_رشته(pp->host.str);
    }
    وإلا
#پایان_اگر
    إذا(پیمایش_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      إذا(pp->host_user.len < MAX_IPADR_LEN) {
        حرف tmp[MAX_IPADR_LEN];
        کپی_حافظه(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      وإلا
        pp->ipv6 = TRUE;
    }
    وإلا
      pp->host = pp->host_user;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_peer_create(بنية Curl_easy *داده,
                          ثابت بنية Curl_scheme *scheme,
                          ثابت حرف *hostname,
                          uint16_t پورت,
                          بنية Curl_peer **ppeer)
{
  بنية peer_parse pp;
  CURLcode نتیجه;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = طول_رشته(hostname);
  pp.پورت = پورت;

  نتیجه = peer_parse_host(داده, &pp, TRUE);
  إذا(!نتیجه)
    نتیجه = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  أرجع نتیجه;
}

#اگر_تعریف_شده USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ثابت بنية Curl_scheme *scheme,
                              ثابت حرف *مسیر,
                              bool abstract_unix_socket,
                              بنية Curl_peer **ppeer)
{
  بنية peer_parse pp;
  size_t pathlen = مسیر ? طول_رشته(مسیر) : 0;
  CURLcode نتیجه = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  إذا(!scheme)
    أرجع CURLE_FAILED_INIT;
  إذا(!pathlen)
    أرجع CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = مسیر;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  نتیجه = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  أرجع نتیجه;
}
#پایان_اگر /* USE_UNIX_SOCKETS */

فراغ Curl_peer_link(بنية Curl_peer **pdest, بنية Curl_peer *src)
{
  إذا(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    إذا(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

فراغ Curl_peer_unlink(بنية Curl_peer **ppeer)
{
  إذا(*ppeer) {
    بنية Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = عدم;
    إذا(peer->refcount)
      peer->refcount--;
    إذا(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(بنية Curl_peer *p1, بنية Curl_peer *p2)
{
  أرجع (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

ساكن bool peer_same_hostname(بنية Curl_peer *p1, بنية Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  أرجع (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

ساكن bool peer_same_zoneid(ثابت حرف *z1, ثابت حرف *z2)
{
  أرجع (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(بنية Curl_peer *p1, بنية Curl_peer *p2)
{
  أرجع (p1 == p2) ||
         (p1 && p2 &&
          (p1->پورت == p2->پورت) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, بنية Curl_easy *داده,
                            uint16_t پورت_override,
                            uint32_t scopeid_override,
                            بنية Curl_peer **ppeer)
{
  بنية peer_parse pp;
  حرف *zoneid = عدم, *scheme = عدم, *hostname = عدم;
  CURLUcode uc;
  CURLcode نتیجه;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  إذا(uc)
    أرجع Curl_uc_مقصد_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  إذا(!pp.scheme) {
    failf(داده, "Protocol \"%s\" not supported%s", scheme,
          داده->state.this_is_a_follow ? " (in redirect)" : "");
    نتیجه = CURLE_UNSUPPORTED_PROTOCOL;
    اذهب بیرون;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  إذا(uc) {
    إذا((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    وإلا {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }
  وإلا إذا(طول_رشته(hostname) > MAX_URL_LEN) {
    failf(داده, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    نتیجه = CURLE_URL_MALFORMAT;
    اذهب بیرون;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = طول_رشته(pp.host_user.str);
  إذا(pp.host_user.len) {
    نتیجه = peer_parse_host(داده, &pp, FALSE);
    إذا(نتیجه)
      اذهب بیرون;
  }
  وإلا
    pp.host = pp.host_user;

  إذا(پورت_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    حرف portbuf[16];
    curl_msnprintf(portbuf, حجم(portbuf), "%d", پورت_override);
    uc = curl_url_مجموعه(uh, CURLUPART_پورت, portbuf, 0);
    إذا(uc) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
    وإلا
      pp.پورت = پورت_override;
  }
  وإلا {
    uc = Curl_url_get_پورت(uh, &pp.پورت);
    إذا(uc) {
      إذا(uc == CURLUE_بیرون_از_حافظه) {
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
      }
      وإلا إذا(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        نتیجه = CURLE_URL_MALFORMAT;
        اذهب بیرون;
      }
      /* no port ok when not a network scheme */
    }
  }

  إذا(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  وإلا {
    إذا(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_بیرون_از_حافظه) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
    إذا(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = طول_رشته(zoneid);
    }
  }

  نتیجه = peer_create(&pp, ppeer);
  إذا(نتیجه)
    failf(داده, "Error %d creating peer for %s:%u",
          (صحيح)نتیجه, pp.host_user.str, pp.پورت);

بیرون:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  أرجع نتیجه;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_مقصد(بنية Curl_easy *داده,
                                   ثابت بنية Curl_peer *dest,
                                   ثابت حرف *connect_مقصد,
                                   بنية Curl_peer **ppeer)
{
  بنية peer_parse pp;
  ثابت حرف *portstr = عدم;
  bool پورت_switch = FALSE;
  CURLcode نتیجه;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  إذا(!connect_مقصد || !*connect_مقصد)
    أرجع CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  إذا(connect_مقصد[0] == '[') {
    ثابت حرف *s = strchr(connect_مقصد + 1, ']');
    إذا(!s) {
      failf(داده, "Invalid IPv6 address format in '%s'", connect_مقصد);
      نتیجه = CURLE_SETOPT_OPTION_SYNTAX;
      اذهب بیرون;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_مقصد;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  وإلا {
    portstr = strchr(connect_مقصد, ':');
    pp.host_user.str = connect_مقصد;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_مقصد) : طول_رشته(connect_مقصد);
  }

  إذا(!pp.host_user.len) { /* no hostname found, only port switch */
    پورت_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = طول_رشته(dest->user_hostname);
  }

  نتیجه = peer_parse_host(داده, &pp, FALSE);
  إذا(نتیجه)
    اذهب بیرون;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!پورت_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  إذا(portstr && portstr[1]) {
    ثابت حرف *p = portstr + 1;
    curl_off_t portparse;
    إذا(curlx_str_عدد(&p, &portparse, 0xffff)) {
      failf(داده, "No valid port number in '%s'", connect_مقصد);
      نتیجه = CURLE_SETOPT_OPTION_SYNTAX;
      اذهب بیرون;
    }
    pp.پورت = (uint16_t)portparse; /* we know it will fit */
  }
  وإلا
    pp.پورت = dest->پورت;

#اگر_تعریف_نشده USE_IPV6
  إذا(pp.ipv6) {
    failf(داده, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    نتیجه = CURLE_NOT_BUILT_درون;
    اذهب بیرون;
  }
#پایان_اگر
  إذا(pp.ipv6 && پورت_switch) {
    إذا(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = طول_رشته(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  نتیجه = peer_create(&pp, ppeer);
  CURL_TRC_M(داده, "connect-to peer_create2 -> %d", (صحيح)نتیجه);

بیرون:
  CURL_TRC_M(داده, "parse connect_to peer: %s -> %d", connect_مقصد, (صحيح)نتیجه);
  peer_parse_clear(&pp);
  أرجع نتیجه;
}

#اگر_تعریف_نشده CURL_DISABLE_PROXY

#اگر_تعریف_شده USE_UNIX_SOCKETS
#تعریف_کن UNIX_SOCKET_پیشوند "localhost"
#پایان_اگر

CURLcode Curl_scheme_مقصد_proxytype(بنية Curl_easy *داده,
                                  ثابت حرف *scheme,
                                  uint8_t *proxytype, ثابت حرف *url)
{
  إذا(!scheme)
    أرجع CURLE_OK;

  إذا(curl_strequal("https", scheme)) {
    إذا(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  وإلا إذا(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  وإلا إذا(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  وإلا إذا(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  وإلا إذا(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  وإلا إذا(curl_strequal("http", scheme)) {
    إذا(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  وإلا {
    /* Any other xxx:// reject! */
    failf(داده, "Unsupported proxy scheme for \'%s\'", url);
    أرجع CURLE_COULDNT_CONNECT;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  بنية Curl_easy *داده,
                                  ثابت حرف *url,
                                  uint8_t proxytype,
                                  بنية Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  بنية peer_parse pp;
  حرف *scheme = عدم;
  حرف *portptr = عدم;
#اگر_تعریف_شده USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#پایان_اگر
  CURLUcode uc;
  CURLcode نتیجه = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, حجم(pp));
  pp.پورت = CURL_DEFAULT_PROXY_پورت;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  إذا(uc) {
    إذا(uc == CURLUE_بیرون_از_حافظه) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    بدل(proxytype) {
    حالة CURLPROXY_HTTP:
    حالة CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      اكسر;
    حالة CURLPROXY_HTTPS:
    حالة CURLPROXY_HTTPS2:
    حالة CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      اكسر;
    حالة CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      اكسر;
    حالة CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      اكسر;
    حالة CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      اكسر;
    حالة CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      اكسر;
    افتراضي:
      failf(داده, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      نتیجه = CURLE_COULDNT_RESOLVE_PROXY;
      اذهب بیرون;
    }
  }
  وإلا {
    pp.scheme = Curl_get_scheme(scheme);
    نتیجه = Curl_scheme_مقصد_proxytype(داده, scheme, &proxytype, url);
    إذا(نتیجه)
      اذهب بیرون;
  }
  DEBUGASSERT(pp.scheme);

  إذا(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(داده, SSLSUPP_HTTPS_PROXY)) {
    failf(داده, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    نتیجه = CURLE_NOT_BUILT_درون;
    اذهب بیرون;
  }

  بدل(pp.scheme->family) {
  حالة CURLPROTO_SOCKS:
#اگر_تعریف_شده USE_UNIX_SOCKETS
    is_socks = TRUE;
#پایان_اگر
    اكسر;
  حالة CURLPROTO_HTTP:
    اكسر;
  افتراضي:
    failf(داده, "Unsupported proxy protocol for \'%s\'", url);
    نتیجه = CURLE_COULDNT_CONNECT;
    اذهب بیرون;
  }

  uc = curl_url_get(uh, CURLUPART_پورت, &portptr, CURLU_NO_DEFAULT_پورت);
  إذا(uc == CURLUE_بیرون_از_حافظه) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  إذا(portptr) {
    curl_off_t رقم;
    ثابت حرف *p = portptr;
    إذا(!curlx_str_عدد(&p, &رقم, UINT16_MAX))
      pp.پورت = (uint16_t)رقم;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  وإلا {
    /* No port in URL, take the set one or the scheme's default */
    إذا(داده->مجموعه.proxyport)
      pp.پورت = داده->مجموعه.proxyport;
    وإلا
      pp.پورت = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  إذا(uc) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = طول_رشته(pp.tmp_host_user);

#اگر_تعریف_شده USE_UNIX_SOCKETS
  إذا(is_socks && curl_strequal(UNIX_SOCKET_پیشوند, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_مسیر, &pp.tmp_host, CURLU_URLDECODE);
    إذا(uc) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
    /* path will be "/", if no path was found */
    إذا(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = طول_رشته(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    وإلا {
      pp.host = pp.host_user;
    }
  }
#پایان_اگر /* USE_UNIX_SOCKETS */

  إذا(!pp.host.len) {
    نتیجه = peer_parse_host(داده, &pp, FALSE);
    إذا(نتیجه)
      اذهب بیرون;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  إذا(uc == CURLUE_بیرون_از_حافظه) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  إذا(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = طول_رشته(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  نتیجه = peer_create(&pp, ppeer);

بیرون:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#اگر_تعریف_شده DEBUGBUILD
  إذا(!نتیجه)
    DEBUGASSERT(*ppeer);
#پایان_اگر
  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_PROXY */
