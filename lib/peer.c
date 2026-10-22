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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NET_IF_H
#అన్తర్భావ <net/if.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_IPHLPAPI_H
#అన్తర్భావ <Iphlpapi.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_PARAM_H
#అన్తర్భావ <sys/param.h>
#అయితే_ముగింపు

#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_UN_H
#అన్తర్భావ <sys/un.h>
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#ఒకవేళ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#అన్తర్భావ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#అయితే_ముగింపు
#అన్తర్భావ <iphlpapi.h>
#అయితే_ముగింపు

#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "protocol.h"
#అన్తర్భావ "http_proxy.h"
#అన్తర్భావ "idn.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "peer.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "urlapi-int.h"
#అన్తర్భావ "vtls/vtls.h"

నిర్మాణం peer_parse {
  స్థిరరాశి నిర్మాణం Curl_scheme *scheme;
  నిర్మాణం Curl_str host_user;
  నిర్మాణం Curl_str host;
  నిర్మాణం Curl_str zoneid;
  అక్షరం *tmp_host_user;
  అక్షరం *tmp_host;
  అక్షరం *tmp_zoneid;
  uint32_t scopeid;
  uint16_t పోర్టు;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

స్థిర శూన్య peer_parse_clear(నిర్మాణం peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  memset(pp, 0, పరిమాణం(*pp));
}

స్థిర CURLcode peer_create(నిర్మాణం peer_parse *pp,
                            నిర్మాణం Curl_peer **ppeer)
{
  నిర్మాణం Curl_peer *peer = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  ఒకవేళ(!pp || !pp->scheme)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, పరిమాణం(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  ఒకవేళ(!peer) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->పోర్టు = pp->పోర్టు;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  ఒకవేళ(pp->host_user.len)
    జ్ఞాపకం_నకలు(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  ఒకవేళ(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    జ్ఞాపకం_నకలు(peer->hostname, pp->host.str, pp->host.len);
  }

  ఒకవేళ(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    జ్ఞాపకం_నకలు(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#నిర్వచితమైతే USE_IPV6
    /* Determine scope_id if not already provided */
    ఒకవేళ(!peer->scopeid) {
      స్థిరరాశి అక్షరం *p = peer->zoneid;
      curl_off_t scope;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#నిర్వచితమైతే HAVE_IF_NAMETOINDEX
      లేకపోతే {
        /* Zone identifier is not fully numeric */
        గుర్తులేని పూర్ణాంకం idx = 0;
        idx = if_nametoindex(peer->zoneid);
        ఒకవేళ(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        లేకపోతే {
          /* Do we want to return an error here? */
        }
      }
#అయితే_ముగింపు /* HAVE_IF_NAMETOINDEX */
    }
#అయితే_ముగింపు /* USE_IPV6 */
  }

బహిర్గతం:
  ఒకవేళ(!ఫలితం)
    *ppeer = peer;
  లేకపోతే
    Curl_peer_unlink(&peer);
  తిరిగి ఫలితం;
}

స్థిర CURLcode peer_parse_host(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం peer_parse *pp,
                                bool పరిశీలన_for_ipv6)
{
  ఒకవేళ(!pp || !pp->host_user.str || !pp->host_user.len)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(pp->host_user.str[0] == '[') {
    స్థిరరాశి అక్షరం *s = pp->host_user.str + 1;
    నిర్మాణం Curl_str tmp;
    ఒకవేళ(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      తిరిగి CURLE_URL_MALFORMAT;

    ఒకవేళ(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(దత్తాంశం, "Invalid IPv6 address format in '%.*s'",
            (పూర్ణాంకం)pp->host_user.len, pp->host_user.str);
      తిరిగి CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  లేకపోతే {
#నిర్వచితమైతే USE_IDN
    ఒకవేళ(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode ఫలితం;
      ఒకవేళ(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        ఒకవేళ(!pp->tmp_host_user)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      ఫలితం = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      pp->host.str = pp->tmp_host;
      pp->host.len = అక్షరశ్రేణి_పొడవు(pp->host.str);
    }
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ(పరిశీలన_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      ఒకవేళ(pp->host_user.len < MAX_IPADR_LEN) {
        అక్షరం tmp[MAX_IPADR_LEN];
        జ్ఞాపకం_నకలు(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      లేకపోతే
        pp->ipv6 = TRUE;
    }
    లేకపోతే
      pp->host = pp->host_user;
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_peer_create(నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి నిర్మాణం Curl_scheme *scheme,
                          స్థిరరాశి అక్షరం *hostname,
                          uint16_t పోర్టు,
                          నిర్మాణం Curl_peer **ppeer)
{
  నిర్మాణం peer_parse pp;
  CURLcode ఫలితం;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, పరిమాణం(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = అక్షరశ్రేణి_పొడవు(hostname);
  pp.పోర్టు = పోర్టు;

  ఫలితం = peer_parse_host(దత్తాంశం, &pp, TRUE);
  ఒకవేళ(!ఫలితం)
    ఫలితం = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(స్థిరరాశి నిర్మాణం Curl_scheme *scheme,
                              స్థిరరాశి అక్షరం *మార్గం,
                              bool abstract_unix_socket,
                              నిర్మాణం Curl_peer **ppeer)
{
  నిర్మాణం peer_parse pp;
  size_t pathlen = మార్గం ? అక్షరశ్రేణి_పొడవు(మార్గం) : 0;
  CURLcode ఫలితం = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, పరిమాణం(pp));
  ఒకవేళ(!scheme)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ(!pathlen)
    తిరిగి CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = మార్గం;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  ఫలితం = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* USE_UNIX_SOCKETS */

శూన్య Curl_peer_link(నిర్మాణం Curl_peer **pdest, నిర్మాణం Curl_peer *src)
{
  ఒకవేళ(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    ఒకవేళ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

శూన్య Curl_peer_unlink(నిర్మాణం Curl_peer **ppeer)
{
  ఒకవేళ(*ppeer) {
    నిర్మాణం Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = ఖాళీ;
    ఒకవేళ(peer->refcount)
      peer->refcount--;
    ఒకవేళ(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(నిర్మాణం Curl_peer *p1, నిర్మాణం Curl_peer *p2)
{
  తిరిగి (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

స్థిర bool peer_same_hostname(నిర్మాణం Curl_peer *p1, నిర్మాణం Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  తిరిగి (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!strcmp(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

స్థిర bool peer_same_zoneid(స్థిరరాశి అక్షరం *z1, స్థిరరాశి అక్షరం *z2)
{
  తిరిగి (z1 == z2) ||
         (z1 && z2 && !strcmp(z1, z2));
}

bool Curl_peer_same_destination(నిర్మాణం Curl_peer *p1, నిర్మాణం Curl_peer *p2)
{
  తిరిగి (p1 == p2) ||
         (p1 && p2 &&
          (p1->పోర్టు == p2->పోర్టు) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, నిర్మాణం Curl_easy *దత్తాంశం,
                            uint16_t పోర్టు_override,
                            uint32_t scopeid_override,
                            నిర్మాణం Curl_peer **ppeer)
{
  నిర్మాణం peer_parse pp;
  అక్షరం *zoneid = ఖాళీ, *scheme = ఖాళీ, *hostname = ఖాళీ;
  CURLUcode uc;
  CURLcode ఫలితం;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, పరిమాణం(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  ఒకవేళ(uc)
    తిరిగి Curl_uc_గమ్యం_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  ఒకవేళ(!pp.scheme) {
    failf(దత్తాంశం, "Protocol \"%s\" not supported%s", scheme,
          దత్తాంశం->state.this_is_a_follow ? " (in redirect)" : "");
    ఫలితం = CURLE_UNSUPPORTED_PROTOCOL;
    వెళ్ళు బహిర్గతం;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  ఒకవేళ(uc) {
    ఒకవేళ((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    లేకపోతే {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }
  లేకపోతే ఒకవేళ(అక్షరశ్రేణి_పొడవు(hostname) > MAX_URL_LEN) {
    failf(దత్తాంశం, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    ఫలితం = CURLE_URL_MALFORMAT;
    వెళ్ళు బహిర్గతం;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = అక్షరశ్రేణి_పొడవు(pp.host_user.str);
  ఒకవేళ(pp.host_user.len) {
    ఫలితం = peer_parse_host(దత్తాంశం, &pp, FALSE);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  లేకపోతే
    pp.host = pp.host_user;

  ఒకవేళ(పోర్టు_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    అక్షరం portbuf[16];
    curl_msnprintf(portbuf, పరిమాణం(portbuf), "%d", పోర్టు_override);
    uc = curl_url_సమితి(uh, CURLUPART_పోర్టు, portbuf, 0);
    ఒకవేళ(uc) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే
      pp.పోర్టు = పోర్టు_override;
  }
  లేకపోతే {
    uc = Curl_url_get_పోర్టు(uh, &pp.పోర్టు);
    ఒకవేళ(uc) {
      ఒకవేళ(uc == CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
      లేకపోతే ఒకవేళ(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        ఫలితం = CURLE_URL_MALFORMAT;
        వెళ్ళు బహిర్గతం;
      }
      /* no port ok when not a network scheme */
    }
  }

  ఒకవేళ(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  లేకపోతే {
    ఒకవేళ(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
    ఒకవేళ(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = అక్షరశ్రేణి_పొడవు(zoneid);
    }
  }

  ఫలితం = peer_create(&pp, ppeer);
  ఒకవేళ(ఫలితం)
    failf(దత్తాంశం, "Error %d creating peer for %s:%u",
          (పూర్ణాంకం)ఫలితం, pp.host_user.str, pp.పోర్టు);

బహిర్గతం:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  తిరిగి ఫలితం;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_గమ్యం(నిర్మాణం Curl_easy *దత్తాంశం,
                                   స్థిరరాశి నిర్మాణం Curl_peer *dest,
                                   స్థిరరాశి అక్షరం *connect_గమ్యం,
                                   నిర్మాణం Curl_peer **ppeer)
{
  నిర్మాణం peer_parse pp;
  స్థిరరాశి అక్షరం *portstr = ఖాళీ;
  bool పోర్టు_switch = FALSE;
  CURLcode ఫలితం;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, పరిమాణం(pp));
  ఒకవేళ(!connect_గమ్యం || !*connect_గమ్యం)
    తిరిగి CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  ఒకవేళ(connect_గమ్యం[0] == '[') {
    స్థిరరాశి అక్షరం *s = strchr(connect_గమ్యం + 1, ']');
    ఒకవేళ(!s) {
      failf(దత్తాంశం, "Invalid IPv6 address format in '%s'", connect_గమ్యం);
      ఫలితం = CURLE_SETOPT_OPTION_SYNTAX;
      వెళ్ళు బహిర్గతం;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_గమ్యం;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  లేకపోతే {
    portstr = strchr(connect_గమ్యం, ':');
    pp.host_user.str = connect_గమ్యం;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_గమ్యం) : అక్షరశ్రేణి_పొడవు(connect_గమ్యం);
  }

  ఒకవేళ(!pp.host_user.len) { /* no hostname found, only port switch */
    పోర్టు_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = అక్షరశ్రేణి_పొడవు(dest->user_hostname);
  }

  ఫలితం = peer_parse_host(దత్తాంశం, &pp, FALSE);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!పోర్టు_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  ఒకవేళ(portstr && portstr[1]) {
    స్థిరరాశి అక్షరం *p = portstr + 1;
    curl_off_t portparse;
    ఒకవేళ(curlx_str_సంఖ్య(&p, &portparse, 0xffff)) {
      failf(దత్తాంశం, "No valid port number in '%s'", connect_గమ్యం);
      ఫలితం = CURLE_SETOPT_OPTION_SYNTAX;
      వెళ్ళు బహిర్గతం;
    }
    pp.పోర్టు = (uint16_t)portparse; /* we know it will fit */
  }
  లేకపోతే
    pp.పోర్టు = dest->పోర్టు;

#నిర్వచితం_కాకపోతే USE_IPV6
  ఒకవేళ(pp.ipv6) {
    failf(దత్తాంశం, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    ఫలితం = CURLE_NOT_BUILT_లోపల;
    వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు
  ఒకవేళ(pp.ipv6 && పోర్టు_switch) {
    ఒకవేళ(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = అక్షరశ్రేణి_పొడవు(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  ఫలితం = peer_create(&pp, ppeer);
  CURL_TRC_M(దత్తాంశం, "connect-to peer_create2 -> %d", (పూర్ణాంకం)ఫలితం);

బహిర్గతం:
  CURL_TRC_M(దత్తాంశం, "parse connect_to peer: %s -> %d", connect_గమ్యం, (పూర్ణాంకం)ఫలితం);
  peer_parse_clear(&pp);
  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY

#నిర్వచితమైతే USE_UNIX_SOCKETS
#నిర్వచించు UNIX_SOCKET_ఉపసర్గ "localhost"
#అయితే_ముగింపు

CURLcode Curl_scheme_గమ్యం_proxytype(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *scheme,
                                  uint8_t *proxytype, స్థిరరాశి అక్షరం *url)
{
  ఒకవేళ(!scheme)
    తిరిగి CURLE_OK;

  ఒకవేళ(curl_strequal("https", scheme)) {
    ఒకవేళ(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  లేకపోతే ఒకవేళ(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  లేకపోతే ఒకవేళ(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  లేకపోతే ఒకవేళ(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  లేకపోతే ఒకవేళ(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  లేకపోతే ఒకవేళ(curl_strequal("http", scheme)) {
    ఒకవేళ(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  లేకపోతే {
    /* Any other xxx:// reject! */
    failf(దత్తాంశం, "Unsupported proxy scheme for \'%s\'", url);
    తిరిగి CURLE_COULDNT_CONNECT;
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *url,
                                  uint8_t proxytype,
                                  నిర్మాణం Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  నిర్మాణం peer_parse pp;
  అక్షరం *scheme = ఖాళీ;
  అక్షరం *portptr = ఖాళీ;
#నిర్వచితమైతే USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#అయితే_ముగింపు
  CURLUcode uc;
  CURLcode ఫలితం = CURLE_OK;

  Curl_peer_unlink(ppeer);
  memset(&pp, 0, పరిమాణం(pp));
  pp.పోర్టు = CURL_DEFAULT_PROXY_పోర్టు;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  ఒకవేళ(uc) {
    ఒకవేళ(uc == CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    ఎంపిక(proxytype) {
    స్థితి CURLPROXY_HTTP:
    స్థితి CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      ఆపు;
    స్థితి CURLPROXY_HTTPS:
    స్థితి CURLPROXY_HTTPS2:
    స్థితి CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      ఆపు;
    స్థితి CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      ఆపు;
    స్థితి CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      ఆపు;
    స్థితి CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      ఆపు;
    స్థితి CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      ఆపు;
    అప్రమేయ:
      failf(దత్తాంశం, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      ఫలితం = CURLE_COULDNT_RESOLVE_PROXY;
      వెళ్ళు బహిర్గతం;
    }
  }
  లేకపోతే {
    pp.scheme = Curl_get_scheme(scheme);
    ఫలితం = Curl_scheme_గమ్యం_proxytype(దత్తాంశం, scheme, &proxytype, url);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  DEBUGASSERT(pp.scheme);

  ఒకవేళ(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(దత్తాంశం, SSLSUPP_HTTPS_PROXY)) {
    failf(దత్తాంశం, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    ఫలితం = CURLE_NOT_BUILT_లోపల;
    వెళ్ళు బహిర్గతం;
  }

  ఎంపిక(pp.scheme->family) {
  స్థితి CURLPROTO_SOCKS:
#నిర్వచితమైతే USE_UNIX_SOCKETS
    is_socks = TRUE;
#అయితే_ముగింపు
    ఆపు;
  స్థితి CURLPROTO_HTTP:
    ఆపు;
  అప్రమేయ:
    failf(దత్తాంశం, "Unsupported proxy protocol for \'%s\'", url);
    ఫలితం = CURLE_COULDNT_CONNECT;
    వెళ్ళు బహిర్గతం;
  }

  uc = curl_url_get(uh, CURLUPART_పోర్టు, &portptr, CURLU_NO_DEFAULT_పోర్టు);
  ఒకవేళ(uc == CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(portptr) {
    curl_off_t అంకె;
    స్థిరరాశి అక్షరం *p = portptr;
    ఒకవేళ(!curlx_str_సంఖ్య(&p, &అంకె, UINT16_MAX))
      pp.పోర్టు = (uint16_t)అంకె;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  లేకపోతే {
    /* No port in URL, take the set one or the scheme's default */
    ఒకవేళ(దత్తాంశం->సమితి.proxyport)
      pp.పోర్టు = దత్తాంశం->సమితి.proxyport;
    లేకపోతే
      pp.పోర్టు = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  ఒకవేళ(uc) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = అక్షరశ్రేణి_పొడవు(pp.tmp_host_user);

#నిర్వచితమైతే USE_UNIX_SOCKETS
  ఒకవేళ(is_socks && curl_strequal(UNIX_SOCKET_ఉపసర్గ, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_మార్గం, &pp.tmp_host, CURLU_URLDECODE);
    ఒకవేళ(uc) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
    /* path will be "/", if no path was found */
    ఒకవేళ(strcmp("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = అక్షరశ్రేణి_పొడవు(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    లేకపోతే {
      pp.host = pp.host_user;
    }
  }
#అయితే_ముగింపు /* USE_UNIX_SOCKETS */

  ఒకవేళ(!pp.host.len) {
    ఫలితం = peer_parse_host(దత్తాంశం, &pp, FALSE);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  ఒకవేళ(uc == CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = అక్షరశ్రేణి_పొడవు(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  ఫలితం = peer_create(&pp, ppeer);

బహిర్గతం:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(!ఫలితం)
    DEBUGASSERT(*ppeer);
#అయితే_ముగింపు
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_PROXY */
