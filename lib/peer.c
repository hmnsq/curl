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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NET_IF_H
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_IPHLPAPI_H
#ਅਨ੍ਤਰ੍ਭਾਵ <Iphlpapi.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_UN_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/un.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#ਜੇ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ਅਨ੍ਤਰ੍ਭਾਵ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <iphlpapi.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "idn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "peer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urlapi-int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"

ਬਣਤਰ peer_parse {
  ਅਚਲ ਬਣਤਰ Curl_scheme *scheme;
  ਬਣਤਰ Curl_str host_user;
  ਬਣਤਰ Curl_str host;
  ਬਣਤਰ Curl_str zoneid;
  ਅੱਖਰ *tmp_host_user;
  ਅੱਖਰ *tmp_host;
  ਅੱਖਰ *tmp_zoneid;
  uint32_t scopeid;
  uint16_t ਪੋਰਟ;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

ਸਥਿਰ ਖਾਲੀ peer_parse_clear(ਬਣਤਰ peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, ਆਕਾਰ(*pp));
}

ਸਥਿਰ CURLcode peer_create(ਬਣਤਰ peer_parse *pp,
                            ਬਣਤਰ Curl_peer **ppeer)
{
  ਬਣਤਰ Curl_peer *peer = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  ਜੇ(!pp || !pp->scheme)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, ਆਕਾਰ(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  ਜੇ(!peer) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->ਪੋਰਟ = pp->ਪੋਰਟ;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  ਜੇ(pp->host_user.len)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  ਜੇ(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(peer->hostname, pp->host.str, pp->host.len);
  }

  ਜੇ(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    /* Determine scope_id if not already provided */
    ਜੇ(!peer->scopeid) {
      ਅਚਲ ਅੱਖਰ *p = peer->zoneid;
      curl_off_t scope;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_IF_NAMETOINDEX
      ਨਹੀਂਤਾਂ {
        /* Zone identifier is not fully numeric */
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ idx = 0;
        idx = if_nametoindex(peer->zoneid);
        ਜੇ(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        ਨਹੀਂਤਾਂ {
          /* Do we want to return an error here? */
        }
      }
#ਜੇ_ਅੰਤ /* HAVE_IF_NAMETOINDEX */
    }
#ਜੇ_ਅੰਤ /* USE_IPV6 */
  }

ਬਾਹਰਲਾ:
  ਜੇ(!ਨਤੀਜਾ)
    *ppeer = peer;
  ਨਹੀਂਤਾਂ
    Curl_peer_unlink(&peer);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode peer_parse_host(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ peer_parse *pp,
                                bool ਜਾਂਚ_for_ipv6)
{
  ਜੇ(!pp || !pp->host_user.str || !pp->host_user.len)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(pp->host_user.str[0] == '[') {
    ਅਚਲ ਅੱਖਰ *s = pp->host_user.str + 1;
    ਬਣਤਰ Curl_str tmp;
    ਜੇ(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      ਵਾਪਸ CURLE_URL_MALFORMAT;

    ਜੇ(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(ਅੰਕੜੇ, "Invalid IPv6 address format in '%.*s'",
            (ਪੂਰਨ)pp->host_user.len, pp->host_user.str);
      ਵਾਪਸ CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IDN
    ਜੇ(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode ਨਤੀਜਾ;
      ਜੇ(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        ਜੇ(!pp->tmp_host_user)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      ਨਤੀਜਾ = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      pp->host.str = pp->tmp_host;
      pp->host.len = ਲੜੀ_ਲੰਬਾਈ(pp->host.str);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ(ਜਾਂਚ_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      ਜੇ(pp->host_user.len < MAX_IPADR_LEN) {
        ਅੱਖਰ tmp[MAX_IPADR_LEN];
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      ਨਹੀਂਤਾਂ
        pp->ipv6 = TRUE;
    }
    ਨਹੀਂਤਾਂ
      pp->host = pp->host_user;
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_peer_create(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ ਬਣਤਰ Curl_scheme *scheme,
                          ਅਚਲ ਅੱਖਰ *hostname,
                          uint16_t ਪੋਰਟ,
                          ਬਣਤਰ Curl_peer **ppeer)
{
  ਬਣਤਰ peer_parse pp;
  CURLcode ਨਤੀਜਾ;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ਆਕਾਰ(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = ਲੜੀ_ਲੰਬਾਈ(hostname);
  pp.ਪੋਰਟ = ਪੋਰਟ;

  ਨਤੀਜਾ = peer_parse_host(ਅੰਕੜੇ, &pp, TRUE);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ਅਚਲ ਬਣਤਰ Curl_scheme *scheme,
                              ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                              bool abstract_unix_socket,
                              ਬਣਤਰ Curl_peer **ppeer)
{
  ਬਣਤਰ peer_parse pp;
  size_t pathlen = ਰਸਤਾ ? ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ) : 0;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ਆਕਾਰ(pp));
  ਜੇ(!scheme)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ(!pathlen)
    ਵਾਪਸ CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = ਰਸਤਾ;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  ਨਤੀਜਾ = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* USE_UNIX_SOCKETS */

ਖਾਲੀ Curl_peer_link(ਬਣਤਰ Curl_peer **pdest, ਬਣਤਰ Curl_peer *src)
{
  ਜੇ(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    ਜੇ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ਖਾਲੀ Curl_peer_unlink(ਬਣਤਰ Curl_peer **ppeer)
{
  ਜੇ(*ppeer) {
    ਬਣਤਰ Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = ਨਿਰਾ;
    ਜੇ(peer->refcount)
      peer->refcount--;
    ਜੇ(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(ਬਣਤਰ Curl_peer *p1, ਬਣਤਰ Curl_peer *p2)
{
  ਵਾਪਸ (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

ਸਥਿਰ bool peer_same_hostname(ਬਣਤਰ Curl_peer *p1, ਬਣਤਰ Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  ਵਾਪਸ (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

ਸਥਿਰ bool peer_same_zoneid(ਅਚਲ ਅੱਖਰ *z1, ਅਚਲ ਅੱਖਰ *z2)
{
  ਵਾਪਸ (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(ਬਣਤਰ Curl_peer *p1, ਬਣਤਰ Curl_peer *p2)
{
  ਵਾਪਸ (p1 == p2) ||
         (p1 && p2 &&
          (p1->ਪੋਰਟ == p2->ਪੋਰਟ) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            uint16_t ਪੋਰਟ_override,
                            uint32_t scopeid_override,
                            ਬਣਤਰ Curl_peer **ppeer)
{
  ਬਣਤਰ peer_parse pp;
  ਅੱਖਰ *zoneid = ਨਿਰਾ, *scheme = ਨਿਰਾ, *hostname = ਨਿਰਾ;
  CURLUcode uc;
  CURLcode ਨਤੀਜਾ;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ਆਕਾਰ(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  ਜੇ(uc)
    ਵਾਪਸ Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  ਜੇ(!pp.scheme) {
    failf(ਅੰਕੜੇ, "Protocol \"%s\" not supported%s", scheme,
          ਅੰਕੜੇ->state.this_is_a_follow ? " (in redirect)" : "");
    ਨਤੀਜਾ = CURLE_UNSUPPORTED_PROTOCOL;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  ਜੇ(uc) {
    ਜੇ((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਲੜੀ_ਲੰਬਾਈ(hostname) > MAX_URL_LEN) {
    failf(ਅੰਕੜੇ, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = ਲੜੀ_ਲੰਬਾਈ(pp.host_user.str);
  ਜੇ(pp.host_user.len) {
    ਨਤੀਜਾ = peer_parse_host(ਅੰਕੜੇ, &pp, FALSE);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ
    pp.host = pp.host_user;

  ਜੇ(ਪੋਰਟ_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    ਅੱਖਰ portbuf[16];
    curl_msnprintf(portbuf, ਆਕਾਰ(portbuf), "%d", ਪੋਰਟ_override);
    uc = curl_url_ਸੈੱਟ(uh, CURLUPART_ਪੋਰਟ, portbuf, 0);
    ਜੇ(uc) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ
      pp.ਪੋਰਟ = ਪੋਰਟ_override;
  }
  ਨਹੀਂਤਾਂ {
    uc = Curl_url_get_ਪੋਰਟ(uh, &pp.ਪੋਰਟ);
    ਜੇ(uc) {
      ਜੇ(uc == CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      /* no port ok when not a network scheme */
    }
  }

  ਜੇ(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  ਨਹੀਂਤਾਂ {
    ਜੇ(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਜੇ(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = ਲੜੀ_ਲੰਬਾਈ(zoneid);
    }
  }

  ਨਤੀਜਾ = peer_create(&pp, ppeer);
  ਜੇ(ਨਤੀਜਾ)
    failf(ਅੰਕੜੇ, "Error %d creating peer for %s:%u",
          (ਪੂਰਨ)ਨਤੀਜਾ, pp.host_user.str, pp.ਪੋਰਟ);

ਬਾਹਰਲਾ:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_ਮੰਜ਼ਿਲ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਅਚਲ ਬਣਤਰ Curl_peer *dest,
                                   ਅਚਲ ਅੱਖਰ *connect_ਮੰਜ਼ਿਲ,
                                   ਬਣਤਰ Curl_peer **ppeer)
{
  ਬਣਤਰ peer_parse pp;
  ਅਚਲ ਅੱਖਰ *portstr = ਨਿਰਾ;
  bool ਪੋਰਟ_switch = FALSE;
  CURLcode ਨਤੀਜਾ;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ਆਕਾਰ(pp));
  ਜੇ(!connect_ਮੰਜ਼ਿਲ || !*connect_ਮੰਜ਼ਿਲ)
    ਵਾਪਸ CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  ਜੇ(connect_ਮੰਜ਼ਿਲ[0] == '[') {
    ਅਚਲ ਅੱਖਰ *s = strchr(connect_ਮੰਜ਼ਿਲ + 1, ']');
    ਜੇ(!s) {
      failf(ਅੰਕੜੇ, "Invalid IPv6 address format in '%s'", connect_ਮੰਜ਼ਿਲ);
      ਨਤੀਜਾ = CURLE_SETOPT_OPTION_SYNTAX;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_ਮੰਜ਼ਿਲ;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    portstr = strchr(connect_ਮੰਜ਼ਿਲ, ':');
    pp.host_user.str = connect_ਮੰਜ਼ਿਲ;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_ਮੰਜ਼ਿਲ) : ਲੜੀ_ਲੰਬਾਈ(connect_ਮੰਜ਼ਿਲ);
  }

  ਜੇ(!pp.host_user.len) { /* no hostname found, only port switch */
    ਪੋਰਟ_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = ਲੜੀ_ਲੰਬਾਈ(dest->user_hostname);
  }

  ਨਤੀਜਾ = peer_parse_host(ਅੰਕੜੇ, &pp, FALSE);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!ਪੋਰਟ_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  ਜੇ(portstr && portstr[1]) {
    ਅਚਲ ਅੱਖਰ *p = portstr + 1;
    curl_off_t portparse;
    ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &portparse, 0xffff)) {
      failf(ਅੰਕੜੇ, "No valid port number in '%s'", connect_ਮੰਜ਼ਿਲ);
      ਨਤੀਜਾ = CURLE_SETOPT_OPTION_SYNTAX;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    pp.ਪੋਰਟ = (uint16_t)portparse; /* we know it will fit */
  }
  ਨਹੀਂਤਾਂ
    pp.ਪੋਰਟ = dest->ਪੋਰਟ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਜੇ(pp.ipv6) {
    failf(ਅੰਕੜੇ, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    ਨਤੀਜਾ = CURLE_NOT_BUILT_ਵਿੱਚ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ
  ਜੇ(pp.ipv6 && ਪੋਰਟ_switch) {
    ਜੇ(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = ਲੜੀ_ਲੰਬਾਈ(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  ਨਤੀਜਾ = peer_create(&pp, ppeer);
  CURL_TRC_M(ਅੰਕੜੇ, "connect-to peer_create2 -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);

ਬਾਹਰਲਾ:
  CURL_TRC_M(ਅੰਕੜੇ, "parse connect_to peer: %s -> %d", connect_ਮੰਜ਼ਿਲ, (ਪੂਰਨ)ਨਤੀਜਾ);
  peer_parse_clear(&pp);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNIX_SOCKET_ਅਗੇਤਰ "localhost"
#ਜੇ_ਅੰਤ

CURLcode Curl_scheme_ਮੰਜ਼ਿਲ_proxytype(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *scheme,
                                  uint8_t *proxytype, ਅਚਲ ਅੱਖਰ *url)
{
  ਜੇ(!scheme)
    ਵਾਪਸ CURLE_OK;

  ਜੇ(curl_strequal("https", scheme)) {
    ਜੇ(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal("http", scheme)) {
    ਜੇ(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  ਨਹੀਂਤਾਂ {
    /* Any other xxx:// reject! */
    failf(ਅੰਕੜੇ, "Unsupported proxy scheme for \'%s\'", url);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *url,
                                  uint8_t proxytype,
                                  ਬਣਤਰ Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  ਬਣਤਰ peer_parse pp;
  ਅੱਖਰ *scheme = ਨਿਰਾ;
  ਅੱਖਰ *portptr = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#ਜੇ_ਅੰਤ
  CURLUcode uc;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ਆਕਾਰ(pp));
  pp.ਪੋਰਟ = CURL_DEFAULT_PROXY_ਪੋਰਟ;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  ਜੇ(uc) {
    ਜੇ(uc == CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    ਚੋਣ(proxytype) {
    ਹਾਲਤ CURLPROXY_HTTP:
    ਹਾਲਤ CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      ਰੋਕੋ;
    ਹਾਲਤ CURLPROXY_HTTPS:
    ਹਾਲਤ CURLPROXY_HTTPS2:
    ਹਾਲਤ CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      ਰੋਕੋ;
    ਹਾਲਤ CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      ਰੋਕੋ;
    ਹਾਲਤ CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      ਰੋਕੋ;
    ਹਾਲਤ CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      ਰੋਕੋ;
    ਹਾਲਤ CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      ਰੋਕੋ;
    ਮੂਲ:
      failf(ਅੰਕੜੇ, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      ਨਤੀਜਾ = CURLE_COULDNT_RESOLVE_PROXY;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਹੀਂਤਾਂ {
    pp.scheme = Curl_get_scheme(scheme);
    ਨਤੀਜਾ = Curl_scheme_ਮੰਜ਼ਿਲ_proxytype(ਅੰਕੜੇ, scheme, &proxytype, url);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  DEBUGASSERT(pp.scheme);

  ਜੇ(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_HTTPS_PROXY)) {
    failf(ਅੰਕੜੇ, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    ਨਤੀਜਾ = CURLE_NOT_BUILT_ਵਿੱਚ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਚੋਣ(pp.scheme->family) {
  ਹਾਲਤ CURLPROTO_SOCKS:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
    is_socks = TRUE;
#ਜੇ_ਅੰਤ
    ਰੋਕੋ;
  ਹਾਲਤ CURLPROTO_HTTP:
    ਰੋਕੋ;
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "Unsupported proxy protocol for \'%s\'", url);
    ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  uc = curl_url_get(uh, CURLUPART_ਪੋਰਟ, &portptr, CURLU_NO_DEFAULT_ਪੋਰਟ);
  ਜੇ(uc == CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(portptr) {
    curl_off_t ਅੰਕ;
    ਅਚਲ ਅੱਖਰ *p = portptr;
    ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &ਅੰਕ, UINT16_MAX))
      pp.ਪੋਰਟ = (uint16_t)ਅੰਕ;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  ਨਹੀਂਤਾਂ {
    /* No port in URL, take the set one or the scheme's default */
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.proxyport)
      pp.ਪੋਰਟ = ਅੰਕੜੇ->ਸੈੱਟ.proxyport;
    ਨਹੀਂਤਾਂ
      pp.ਪੋਰਟ = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  ਜੇ(uc) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = ਲੜੀ_ਲੰਬਾਈ(pp.tmp_host_user);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  ਜੇ(is_socks && curl_strequal(UNIX_SOCKET_ਅਗੇਤਰ, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_ਰਸਤਾ, &pp.tmp_host, CURLU_URLDECODE);
    ਜੇ(uc) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    /* path will be "/", if no path was found */
    ਜੇ(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = ਲੜੀ_ਲੰਬਾਈ(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    ਨਹੀਂਤਾਂ {
      pp.host = pp.host_user;
    }
  }
#ਜੇ_ਅੰਤ /* USE_UNIX_SOCKETS */

  ਜੇ(!pp.host.len) {
    ਨਤੀਜਾ = peer_parse_host(ਅੰਕੜੇ, &pp, FALSE);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  ਜੇ(uc == CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = ਲੜੀ_ਲੰਬਾਈ(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  ਨਤੀਜਾ = peer_create(&pp, ppeer);

ਬਾਹਰਲਾ:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(!ਨਤੀਜਾ)
    DEBUGASSERT(*ppeer);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY */
