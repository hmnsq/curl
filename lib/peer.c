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

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#ಅನ್ತರ್ಭಾವ <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#endif

#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#ಅನ್ತರ್ಭಾವ <sys/un.h>
#endif

#ಯದಿ defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#ಯದಿ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ಅನ್ತರ್ಭಾವ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#ಅನ್ತರ್ಭಾವ <iphlpapi.h>
#endif

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

ಸಂರಚನಾ peer_parse {
  ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *scheme;
  ಸಂರಚನಾ Curl_str host_user;
  ಸಂರಚನಾ Curl_str host;
  ಸಂರಚನಾ Curl_str zoneid;
  ವರ್ಣ *tmp_host_user;
  ವರ್ಣ *tmp_host;
  ವರ್ಣ *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

ಸ್ಥಿರ ಶೂನ್ಯ peer_parse_clear(ಸಂರಚನಾ peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, ಪರಿಮಾಣ(*pp));
}

ಸ್ಥಿರ CURLcode peer_create(ಸಂರಚನಾ peer_parse *pp,
                            ಸಂರಚನಾ Curl_peer **ppeer)
{
  ಸಂರಚನಾ Curl_peer *peer = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  ಯದಿ(!pp || !pp->scheme)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, ಪರಿಮಾಣ(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  ಯದಿ(!peer) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  ಯದಿ(pp->host_user.len)
    memcpy(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  ಯದಿ(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    memcpy(peer->hostname, pp->host.str, pp->host.len);
  }

  ಯದಿ(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    memcpy(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#ifdef USE_IPV6
    /* Determine scope_id if not already provided */
    ಯದಿ(!peer->scopeid) {
      ಧ್ರುವ ವರ್ಣ *p = peer->zoneid;
      curl_off_t scope;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#ifdef HAVE_IF_NAMETOINDEX
      ಅನ್ಯಥಾ {
        /* Zone identifier is not fully numeric */
        unsigned ಪೂರ್ಣಾಙ್ಕ idx = 0;
        idx = if_nametoindex(peer->zoneid);
        ಯದಿ(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        ಅನ್ಯಥಾ {
          /* Do we want to return an error here? */
        }
      }
#endif /* HAVE_IF_NAMETOINDEX */
    }
#endif /* USE_IPV6 */
  }

out:
  ಯದಿ(!result)
    *ppeer = peer;
  ಅನ್ಯಥಾ
    Curl_peer_unlink(&peer);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode peer_parse_host(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ peer_parse *pp,
                                bool scan_for_ipv6)
{
  ಯದಿ(!pp || !pp->host_user.str || !pp->host_user.len)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(pp->host_user.str[0] == '[') {
    ಧ್ರುವ ವರ್ಣ *s = pp->host_user.str + 1;
    ಸಂರಚನಾ Curl_str tmp;
    ಯದಿ(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;

    ಯದಿ(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(data, "Invalid IPv6 address format in '%.*s'",
            (ಪೂರ್ಣಾಙ್ಕ)pp->host_user.len, pp->host_user.str);
      ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  ಅನ್ಯಥಾ {
#ifdef USE_IDN
    ಯದಿ(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode result;
      ಯದಿ(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        ಯದಿ(!pp->tmp_host_user)
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
      result = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      pp->host.str = pp->tmp_host;
      pp->host.len = strlen(pp->host.str);
    }
    ಅನ್ಯಥಾ
#endif
    ಯದಿ(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      ಯದಿ(pp->host_user.len < MAX_IPADR_LEN) {
        ವರ್ಣ tmp[MAX_IPADR_LEN];
        memcpy(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      ಅನ್ಯಥಾ
        pp->ipv6 = TRUE;
    }
    ಅನ್ಯಥಾ
      pp->host = pp->host_user;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_peer_create(ಸಂರಚನಾ Curl_easy *data,
                          ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *scheme,
                          ಧ್ರುವ ವರ್ಣ *hostname,
                          uint16_t port,
                          ಸಂರಚನಾ Curl_peer **ppeer)
{
  ಸಂರಚನಾ peer_parse pp;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ಪರಿಮಾಣ(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = strlen(hostname);
  pp.port = port;

  result = peer_parse_host(data, &pp, TRUE);
  ಯದಿ(!result)
    result = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *scheme,
                              ಧ್ರುವ ವರ್ಣ *path,
                              bool abstract_unix_socket,
                              ಸಂರಚನಾ Curl_peer **ppeer)
{
  ಸಂರಚನಾ peer_parse pp;
  size_t pathlen = path ? strlen(path) : 0;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ಪರಿಮಾಣ(pp));
  ಯದಿ(!scheme)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ(!pathlen)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  result = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif /* USE_UNIX_SOCKETS */

ಶೂನ್ಯ Curl_peer_link(ಸಂರಚನಾ Curl_peer **pdest, ಸಂರಚನಾ Curl_peer *src)
{
  ಯದಿ(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    ಯದಿ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ಶೂನ್ಯ Curl_peer_unlink(ಸಂರಚನಾ Curl_peer **ppeer)
{
  ಯದಿ(*ppeer) {
    ಸಂರಚನಾ Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = ರಿಕ್ತ;
    ಯದಿ(peer->refcount)
      peer->refcount--;
    ಯದಿ(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(ಸಂರಚನಾ Curl_peer *p1, ಸಂರಚನಾ Curl_peer *p2)
{
  ಪ್ರತ್ಯಾಗಮ (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

ಸ್ಥಿರ bool peer_same_hostname(ಸಂರಚನಾ Curl_peer *p1, ಸಂರಚನಾ Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  ಪ್ರತ್ಯಾಗಮ (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

ಸ್ಥಿರ bool peer_same_zoneid(ಧ್ರುವ ವರ್ಣ *z1, ಧ್ರುವ ವರ್ಣ *z2)
{
  ಪ್ರತ್ಯಾಗಮ (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(ಸಂರಚನಾ Curl_peer *p1, ಸಂರಚನಾ Curl_peer *p2)
{
  ಪ್ರತ್ಯಾಗಮ (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, ಸಂರಚನಾ Curl_easy *data,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            ಸಂರಚನಾ Curl_peer **ppeer)
{
  ಸಂರಚನಾ peer_parse pp;
  ವರ್ಣ *zoneid = ರಿಕ್ತ, *scheme = ರಿಕ್ತ, *hostname = ರಿಕ್ತ;
  CURLUcode uc;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ಪರಿಮಾಣ(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  ಯದಿ(uc)
    ಪ್ರತ್ಯಾಗಮ Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  ಯದಿ(!pp.scheme) {
    failf(data, "Protocol \"%s\" not supported%s", scheme,
          data->state.this_is_a_follow ? " (in redirect)" : "");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ಗಚ್ಛ out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  ಯದಿ(uc) {
    ಯದಿ((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    ಅನ್ಯಥಾ {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(strlen(hostname) > MAX_URL_LEN) {
    failf(data, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    result = CURLE_URL_MALFORMAT;
    ಗಚ್ಛ out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = strlen(pp.host_user.str);
  ಯದಿ(pp.host_user.len) {
    result = peer_parse_host(data, &pp, FALSE);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ
    pp.host = pp.host_user;

  ಯದಿ(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    ವರ್ಣ portbuf[16];
    curl_msnprintf(portbuf, ಪರಿಮಾಣ(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    ಯದಿ(uc) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ
      pp.port = port_override;
  }
  ಅನ್ಯಥಾ {
    uc = Curl_url_get_port(uh, &pp.port);
    ಯದಿ(uc) {
      ಯದಿ(uc == CURLUE_OUT_OF_MEMORY) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
      ಅನ್ಯಥಾ ಯದಿ(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        result = CURLE_URL_MALFORMAT;
        ಗಚ್ಛ out;
      }
      /* no port ok when not a network scheme */
    }
  }

  ಯದಿ(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  ಅನ್ಯಥಾ {
    ಯದಿ(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
    ಯದಿ(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = strlen(zoneid);
    }
  }

  result = peer_create(&pp, ppeer);
  ಯದಿ(result)
    failf(data, "Error %d creating peer for %s:%u",
          (ಪೂರ್ಣಾಙ್ಕ)result, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(ಸಂರಚನಾ Curl_easy *data,
                                   ಧ್ರುವ ಸಂರಚನಾ Curl_peer *dest,
                                   ಧ್ರುವ ವರ್ಣ *connect_to,
                                   ಸಂರಚನಾ Curl_peer **ppeer)
{
  ಸಂರಚನಾ peer_parse pp;
  ಧ್ರುವ ವರ್ಣ *portstr = ರಿಕ್ತ;
  bool port_switch = FALSE;
  CURLcode result;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ಪರಿಮಾಣ(pp));
  ಯದಿ(!connect_to || !*connect_to)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  ಯದಿ(connect_to[0] == '[') {
    ಧ್ರುವ ವರ್ಣ *s = strchr(connect_to + 1, ']');
    ಯದಿ(!s) {
      failf(data, "Invalid IPv6 address format in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      ಗಚ್ಛ out;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  ಅನ್ಯಥಾ {
    portstr = strchr(connect_to, ':');
    pp.host_user.str = connect_to;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_to) : strlen(connect_to);
  }

  ಯದಿ(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = strlen(dest->user_hostname);
  }

  result = peer_parse_host(data, &pp, FALSE);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  ಯದಿ(portstr && portstr[1]) {
    ಧ್ರುವ ವರ್ಣ *p = portstr + 1;
    curl_off_t portparse;
    ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&p, &portparse, 0xffff)) {
      failf(data, "No valid port number in '%s'", connect_to);
      result = CURLE_SETOPT_OPTION_SYNTAX;
      ಗಚ್ಛ out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  ಅನ್ಯಥಾ
    pp.port = dest->port;

#ifndef USE_IPV6
  ಯದಿ(pp.ipv6) {
    failf(data, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    result = CURLE_NOT_BUILT_IN;
    ಗಚ್ಛ out;
  }
#endif
  ಯದಿ(pp.ipv6 && port_switch) {
    ಯದಿ(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = strlen(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  result = peer_create(&pp, ppeer);
  CURL_TRC_M(data, "connect-to peer_create2 -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);

out:
  CURL_TRC_M(data, "parse connect_to peer: %s -> %d", connect_to, (ಪೂರ್ಣಾಙ್ಕ)result);
  peer_parse_clear(&pp);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifndef CURL_DISABLE_PROXY

#ifdef USE_UNIX_SOCKETS
#define UNIX_SOCKET_PREFIX "localhost"
#endif

CURLcode Curl_scheme_to_proxytype(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ವರ್ಣ *scheme,
                                  uint8_t *proxytype, ಧ್ರುವ ವರ್ಣ *url)
{
  ಯದಿ(!scheme)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(curl_strequal("https", scheme)) {
    ಯದಿ(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal("http", scheme)) {
    ಯದಿ(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  ಅನ್ಯಥಾ {
    /* Any other xxx:// reject! */
    failf(data, "Unsupported proxy scheme for \'%s\'", url);
    ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ವರ್ಣ *url,
                                  uint8_t proxytype,
                                  ಸಂರಚನಾ Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  ಸಂರಚನಾ peer_parse pp;
  ವರ್ಣ *scheme = ರಿಕ್ತ;
  ವರ್ಣ *portptr = ರಿಕ್ತ;
#ifdef USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#endif
  CURLUcode uc;
  CURLcode result = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, ಪರಿಮಾಣ(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  ಯದಿ(uc) {
    ಯದಿ(uc == CURLUE_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    ವಿಕಲ್ಪ(proxytype) {
    ಪಕ್ಷ CURLPROXY_HTTP:
    ಪಕ್ಷ CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLPROXY_HTTPS:
    ಪಕ್ಷ CURLPROXY_HTTPS2:
    ಪಕ್ಷ CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      ಗಚ್ಛ out;
    }
  }
  ಅನ್ಯಥಾ {
    pp.scheme = Curl_get_scheme(scheme);
    result = Curl_scheme_to_proxytype(data, scheme, &proxytype, url);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  DEBUGASSERT(pp.scheme);

  ಯದಿ(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(data, SSLSUPP_HTTPS_PROXY)) {
    failf(data, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    result = CURLE_NOT_BUILT_IN;
    ಗಚ್ಛ out;
  }

  ವಿಕಲ್ಪ(pp.scheme->family) {
  ಪಕ್ಷ CURLPROTO_SOCKS:
#ifdef USE_UNIX_SOCKETS
    is_socks = TRUE;
#endif
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLPROTO_HTTP:
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    failf(data, "Unsupported proxy protocol for \'%s\'", url);
    result = CURLE_COULDNT_CONNECT;
    ಗಚ್ಛ out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  ಯದಿ(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  ಯದಿ(portptr) {
    curl_off_t ಅಂಕೆ;
    ಧ್ರುವ ವರ್ಣ *p = portptr;
    ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, UINT16_MAX))
      pp.port = (uint16_t)ಅಂಕೆ;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  ಅನ್ಯಥಾ {
    /* No port in URL, take the set one or the scheme's default */
    ಯದಿ(data->set.proxyport)
      pp.port = data->set.proxyport;
    ಅನ್ಯಥಾ
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  ಯದಿ(uc) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = strlen(pp.tmp_host_user);

#ifdef USE_UNIX_SOCKETS
  ಯದಿ(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    ಯದಿ(uc) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
    /* path will be "/", if no path was found */
    ಯದಿ(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = strlen(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    ಅನ್ಯಥಾ {
      pp.host = pp.host_user;
    }
  }
#endif /* USE_UNIX_SOCKETS */

  ಯದಿ(!pp.host.len) {
    result = peer_parse_host(data, &pp, FALSE);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  ಯದಿ(uc == CURLUE_OUT_OF_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  ಯದಿ(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = strlen(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  result = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#ifdef DEBUGBUILD
  ಯದಿ(!result)
    DEBUGASSERT(*ppeer);
#endif
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_PROXY */
