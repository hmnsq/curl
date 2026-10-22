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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_IPHLPAPI_H
#ಅನ್ತರ್ಭಾವ <Iphlpapi.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_UN_H
#ಅನ್ತರ್ಭಾವ <sys/un.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#ಆದರೆ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ಅನ್ತರ್ಭಾವ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <iphlpapi.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "protocol.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "idn.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "peer.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "urlapi-int.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"

ರಚನೆ peer_parse {
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *scheme;
  ರಚನೆ Curl_str host_user;
  ರಚನೆ Curl_str host;
  ರಚನೆ Curl_str zoneid;
  ಅಕ್ಷರ *tmp_host_user;
  ಅಕ್ಷರ *tmp_host;
  ಅಕ್ಷರ *tmp_zoneid;
  uint32_t scopeid;
  uint16_t ಪೋರ್ಟ್;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

ಸ್ಥಿರ ಶೂನ್ಯ peer_parse_clear(ರಚನೆ peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, ಗಾತ್ರ(*pp));
}

ಸ್ಥಿರ CURLcode peer_create(ರಚನೆ peer_parse *pp,
                            ರಚನೆ Curl_peer **ppeer)
{
  ರಚನೆ Curl_peer *peer = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  ಆದರೆ(!pp || !pp->scheme)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, ಗಾತ್ರ(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  ಆದರೆ(!peer) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->ಪೋರ್ಟ್ = pp->ಪೋರ್ಟ್;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  ಆದರೆ(pp->host_user.len)
    ಸ್ಮರಣೆ_ನಕಲು(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  ಆದರೆ(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    ಸ್ಮರಣೆ_ನಕಲು(peer->hostname, pp->host.str, pp->host.len);
  }

  ಆದರೆ(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    ಸ್ಮರಣೆ_ನಕಲು(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    /* Determine scope_id if not already provided */
    ಆದರೆ(!peer->scopeid) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = peer->zoneid;
      curl_off_t scope;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_IF_NAMETOINDEX
      ಇಲ್ಲದಿದ್ದರೆ {
        /* Zone identifier is not fully numeric */
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ idx = 0;
        idx = if_nametoindex(peer->zoneid);
        ಆದರೆ(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          /* Do we want to return an error here? */
        }
      }
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_IF_NAMETOINDEX */
    }
#ಆದರೆ_ಅಂತ್ಯ /* USE_IPV6 */
  }

ಹೊರಹೋಗು:
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    *ppeer = peer;
  ಇಲ್ಲದಿದ್ದರೆ
    Curl_peer_unlink(&peer);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode peer_parse_host(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ peer_parse *pp,
                                bool ಪರಿಶೀಲನೆ_for_ipv6)
{
  ಆದರೆ(!pp || !pp->host_user.str || !pp->host_user.len)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(pp->host_user.str[0] == '[') {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = pp->host_user.str + 1;
    ರಚನೆ Curl_str tmp;
    ಆದರೆ(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;

    ಆದರೆ(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(ದತ್ತಾಂಶ, "Invalid IPv6 address format in '%.*s'",
            (ಪೂರ್ಣಾಂಕ)pp->host_user.len, pp->host_user.str);
      ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IDN
    ಆದರೆ(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode ಫಲಿತಾಂಶ;
      ಆದರೆ(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        ಆದರೆ(!pp->tmp_host_user)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      ಫಲಿತಾಂಶ = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      pp->host.str = pp->tmp_host;
      pp->host.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(pp->host.str);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(ಪರಿಶೀಲನೆ_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      ಆದರೆ(pp->host_user.len < MAX_IPADR_LEN) {
        ಅಕ್ಷರ tmp[MAX_IPADR_LEN];
        ಸ್ಮರಣೆ_ನಕಲು(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        pp->ipv6 = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      pp->host = pp->host_user;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_peer_create(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *scheme,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                          uint16_t ಪೋರ್ಟ್,
                          ರಚನೆ Curl_peer **ppeer)
{
  ರಚನೆ peer_parse pp;
  CURLcode ಫಲಿತಾಂಶ;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ಗಾತ್ರ(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hostname);
  pp.ಪೋರ್ಟ್ = ಪೋರ್ಟ್;

  ಫಲಿತಾಂಶ = peer_parse_host(ದತ್ತಾಂಶ, &pp, TRUE);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *scheme,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                              bool abstract_unix_socket,
                              ರಚನೆ Curl_peer **ppeer)
{
  ರಚನೆ peer_parse pp;
  size_t pathlen = ಮಾರ್ಗ ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ) : 0;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ಗಾತ್ರ(pp));
  ಆದರೆ(!scheme)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ(!pathlen)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = ಮಾರ್ಗ;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  ಫಲಿತಾಂಶ = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_UNIX_SOCKETS */

ಶೂನ್ಯ Curl_peer_link(ರಚನೆ Curl_peer **pdest, ರಚನೆ Curl_peer *src)
{
  ಆದರೆ(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    ಆದರೆ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ಶೂನ್ಯ Curl_peer_unlink(ರಚನೆ Curl_peer **ppeer)
{
  ಆದರೆ(*ppeer) {
    ರಚನೆ Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = ಖಾಲಿ;
    ಆದರೆ(peer->refcount)
      peer->refcount--;
    ಆದರೆ(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(ರಚನೆ Curl_peer *p1, ರಚನೆ Curl_peer *p2)
{
  ಹಿಂತಿರುಗಿ (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

ಸ್ಥಿರ bool peer_same_hostname(ರಚನೆ Curl_peer *p1, ರಚನೆ Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  ಹಿಂತಿರುಗಿ (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

ಸ್ಥಿರ bool peer_same_zoneid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *z1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *z2)
{
  ಹಿಂತಿರುಗಿ (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(ರಚನೆ Curl_peer *p1, ರಚನೆ Curl_peer *p2)
{
  ಹಿಂತಿರುಗಿ (p1 == p2) ||
         (p1 && p2 &&
          (p1->ಪೋರ್ಟ್ == p2->ಪೋರ್ಟ್) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            uint16_t ಪೋರ್ಟ್_override,
                            uint32_t scopeid_override,
                            ರಚನೆ Curl_peer **ppeer)
{
  ರಚನೆ peer_parse pp;
  ಅಕ್ಷರ *zoneid = ಖಾಲಿ, *scheme = ಖಾಲಿ, *hostname = ಖಾಲಿ;
  CURLUcode uc;
  CURLcode ಫಲಿತಾಂಶ;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ಗಾತ್ರ(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  ಆದರೆ(uc)
    ಹಿಂತಿರುಗಿ Curl_uc_ಗಮ್ಯ_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  ಆದರೆ(!pp.scheme) {
    failf(ದತ್ತಾಂಶ, "Protocol \"%s\" not supported%s", scheme,
          ದತ್ತಾಂಶ->state.this_is_a_follow ? " (in redirect)" : "");
    ಫಲಿತಾಂಶ = CURLE_UNSUPPORTED_PROTOCOL;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  ಆದರೆ(uc) {
    ಆದರೆ((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hostname) > MAX_URL_LEN) {
    failf(ದತ್ತಾಂಶ, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(pp.host_user.str);
  ಆದರೆ(pp.host_user.len) {
    ಫಲಿತಾಂಶ = peer_parse_host(ದತ್ತಾಂಶ, &pp, FALSE);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    pp.host = pp.host_user;

  ಆದರೆ(ಪೋರ್ಟ್_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    ಅಕ್ಷರ portbuf[16];
    curl_msnprintf(portbuf, ಗಾತ್ರ(portbuf), "%d", ಪೋರ್ಟ್_override);
    uc = curl_url_ಗಣ(uh, CURLUPART_ಪೋರ್ಟ್, portbuf, 0);
    ಆದರೆ(uc) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      pp.ಪೋರ್ಟ್ = ಪೋರ್ಟ್_override;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    uc = Curl_url_get_ಪೋರ್ಟ್(uh, &pp.ಪೋರ್ಟ್);
    ಆದರೆ(uc) {
      ಆದರೆ(uc == CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      /* no port ok when not a network scheme */
    }
  }

  ಆದರೆ(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಆದರೆ(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(zoneid);
    }
  }

  ಫಲಿತಾಂಶ = peer_create(&pp, ppeer);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    failf(ದತ್ತಾಂಶ, "Error %d creating peer for %s:%u",
          (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, pp.host_user.str, pp.ಪೋರ್ಟ್);

ಹೊರಹೋಗು:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_ಗಮ್ಯ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_peer *dest,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *connect_ಗಮ್ಯ,
                                   ರಚನೆ Curl_peer **ppeer)
{
  ರಚನೆ peer_parse pp;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *portstr = ಖಾಲಿ;
  bool ಪೋರ್ಟ್_switch = FALSE;
  CURLcode ಫಲಿತಾಂಶ;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ಗಾತ್ರ(pp));
  ಆದರೆ(!connect_ಗಮ್ಯ || !*connect_ಗಮ್ಯ)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  ಆದರೆ(connect_ಗಮ್ಯ[0] == '[') {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = strchr(connect_ಗಮ್ಯ + 1, ']');
    ಆದರೆ(!s) {
      failf(ದತ್ತಾಂಶ, "Invalid IPv6 address format in '%s'", connect_ಗಮ್ಯ);
      ಫಲಿತಾಂಶ = CURLE_SETOPT_OPTION_SYNTAX;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_ಗಮ್ಯ;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    portstr = strchr(connect_ಗಮ್ಯ, ':');
    pp.host_user.str = connect_ಗಮ್ಯ;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_ಗಮ್ಯ) : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(connect_ಗಮ್ಯ);
  }

  ಆದರೆ(!pp.host_user.len) { /* no hostname found, only port switch */
    ಪೋರ್ಟ್_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(dest->user_hostname);
  }

  ಫಲಿತಾಂಶ = peer_parse_host(ದತ್ತಾಂಶ, &pp, FALSE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!ಪೋರ್ಟ್_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  ಆದರೆ(portstr && portstr[1]) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = portstr + 1;
    curl_off_t portparse;
    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &portparse, 0xffff)) {
      failf(ದತ್ತಾಂಶ, "No valid port number in '%s'", connect_ಗಮ್ಯ);
      ಫಲಿತಾಂಶ = CURLE_SETOPT_OPTION_SYNTAX;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    pp.ಪೋರ್ಟ್ = (uint16_t)portparse; /* we know it will fit */
  }
  ಇಲ್ಲದಿದ್ದರೆ
    pp.ಪೋರ್ಟ್ = dest->ಪೋರ್ಟ್;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_IPV6
  ಆದರೆ(pp.ipv6) {
    failf(ದತ್ತಾಂಶ, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    ಫಲಿತಾಂಶ = CURLE_NOT_BUILT_ಒಳ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(pp.ipv6 && ಪೋರ್ಟ್_switch) {
    ಆದರೆ(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  ಫಲಿತಾಂಶ = peer_create(&pp, ppeer);
  CURL_TRC_M(ದತ್ತಾಂಶ, "connect-to peer_create2 -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);

ಹೊರಹೋಗು:
  CURL_TRC_M(ದತ್ತಾಂಶ, "parse connect_to peer: %s -> %d", connect_ಗಮ್ಯ, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  peer_parse_clear(&pp);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
#ವ್ಯಾಖ್ಯಾನಿಸು UNIX_SOCKET_ಪೂರ್ವಪ್ರತ್ಯಯ "localhost"
#ಆದರೆ_ಅಂತ್ಯ

CURLcode Curl_scheme_ಗಮ್ಯ_proxytype(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme,
                                  uint8_t *proxytype, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url)
{
  ಆದರೆ(!scheme)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(curl_strequal("https", scheme)) {
    ಆದರೆ(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal("http", scheme)) {
    ಆದರೆ(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Any other xxx:// reject! */
    failf(ದತ್ತಾಂಶ, "Unsupported proxy scheme for \'%s\'", url);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url,
                                  uint8_t proxytype,
                                  ರಚನೆ Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  ರಚನೆ peer_parse pp;
  ಅಕ್ಷರ *scheme = ಖಾಲಿ;
  ಅಕ್ಷರ *portptr = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
  CURLUcode uc;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ಗಾತ್ರ(pp));
  pp.ಪೋರ್ಟ್ = CURL_DEFAULT_PROXY_ಪೋರ್ಟ್;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  ಆದರೆ(uc) {
    ಆದರೆ(uc == CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    ಆಯ್ಕೆ(proxytype) {
    ಸ್ಥಿತಿ CURLPROXY_HTTP:
    ಸ್ಥಿತಿ CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLPROXY_HTTPS:
    ಸ್ಥಿತಿ CURLPROXY_HTTPS2:
    ಸ್ಥಿತಿ CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      ಫಲಿತಾಂಶ = CURLE_COULDNT_RESOLVE_PROXY;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    pp.scheme = Curl_get_scheme(scheme);
    ಫಲಿತಾಂಶ = Curl_scheme_ಗಮ್ಯ_proxytype(ದತ್ತಾಂಶ, scheme, &proxytype, url);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  DEBUGASSERT(pp.scheme);

  ಆದರೆ(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_HTTPS_PROXY)) {
    failf(ದತ್ತಾಂಶ, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    ಫಲಿತಾಂಶ = CURLE_NOT_BUILT_ಒಳ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆಯ್ಕೆ(pp.scheme->family) {
  ಸ್ಥಿತಿ CURLPROTO_SOCKS:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
    is_socks = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLPROTO_HTTP:
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "Unsupported proxy protocol for \'%s\'", url);
    ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  uc = curl_url_get(uh, CURLUPART_ಪೋರ್ಟ್, &portptr, CURLU_NO_DEFAULT_ಪೋರ್ಟ್);
  ಆದರೆ(uc == CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(portptr) {
    curl_off_t ಅಂಕೆ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = portptr;
    ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, UINT16_MAX))
      pp.ಪೋರ್ಟ್ = (uint16_t)ಅಂಕೆ;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* No port in URL, take the set one or the scheme's default */
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.proxyport)
      pp.ಪೋರ್ಟ್ = ದತ್ತಾಂಶ->ಗಣ.proxyport;
    ಇಲ್ಲದಿದ್ದರೆ
      pp.ಪೋರ್ಟ್ = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  ಆದರೆ(uc) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(pp.tmp_host_user);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  ಆದರೆ(is_socks && curl_strequal(UNIX_SOCKET_ಪೂರ್ವಪ್ರತ್ಯಯ, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_ಮಾರ್ಗ, &pp.tmp_host, CURLU_URLDECODE);
    ಆದರೆ(uc) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    /* path will be "/", if no path was found */
    ಆದರೆ(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      pp.host = pp.host_user;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ /* USE_UNIX_SOCKETS */

  ಆದರೆ(!pp.host.len) {
    ಫಲಿತಾಂಶ = peer_parse_host(ದತ್ತಾಂಶ, &pp, FALSE);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  ಆದರೆ(uc == CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  ಫಲಿತಾಂಶ = peer_create(&pp, ppeer);

ಹೊರಹೋಗು:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    DEBUGASSERT(*ppeer);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY */
