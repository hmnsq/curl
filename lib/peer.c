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

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_IPHLPAPI_H
#अन्तर्भाव <Iphlpapi.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#यदि_अन्तः

#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_SYS_UN_H
#अन्तर्भाव <sys/un.h>
#यदि_अन्तः

#यदि defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#यदि defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#अन्तर्भाव <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#यदि_अन्तः
#अन्तर्भाव <iphlpapi.h>
#यदि_अन्तः

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
  uint16_t पत्तनम्;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

स्थिर शून्य peer_parse_clear(संरचना peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  स्मृतिं_पूरय(pp, 0, परिमाण(*pp));
}

स्थिर CURLcode peer_create(संरचना peer_parse *pp,
                            संरचना Curl_peer **ppeer)
{
  संरचना Curl_peer *peer = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  size_t zone_alen = 0, host_alen = 0;

  यदि(!pp || !pp->scheme)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    प्रत्यागम CURLE_FAILED_INIT;

  यदि((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, परिमाण(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  यदि(!peer) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->पत्तनम् = pp->पत्तनम्;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  यदि(pp->host_user.len)
    स्मृतिं_प्रतिलिख(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  यदि(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    स्मृतिं_प्रतिलिख(peer->hostname, pp->host.str, pp->host.len);
  }

  यदि(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    स्मृतिं_प्रतिलिख(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#यदि_परिभाषितम् USE_IPV6
    /* Determine scope_id if not already provided */
    यदि(!peer->scopeid) {
      ध्रुव वर्ण *p = peer->zoneid;
      curl_off_t scope;
      यदि(!curlx_str_सङ्ख्या(&p, &scope, UINT_MAX) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#यदि_परिभाषितम् HAVE_IF_NAMETOINDEX
      अन्यथा {
        /* Zone identifier is not fully numeric */
        अचिह्नित पूर्णाङ्क idx = 0;
        idx = if_nametoindex(peer->zoneid);
        यदि(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        अन्यथा {
          /* Do we want to return an error here? */
        }
      }
#यदि_अन्तः /* HAVE_IF_NAMETOINDEX */
    }
#यदि_अन्तः /* USE_IPV6 */
  }

बहिर्गमनम्:
  यदि(!परिणाम)
    *ppeer = peer;
  अन्यथा
    Curl_peer_unlink(&peer);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode peer_parse_host(संरचना Curl_easy *दत्त,
                                संरचना peer_parse *pp,
                                bool अवलोकनम्_for_ipv6)
{
  यदि(!pp || !pp->host_user.str || !pp->host_user.len)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(pp->host_user.str[0] == '[') {
    ध्रुव वर्ण *s = pp->host_user.str + 1;
    संरचना Curl_str tmp;
    यदि(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      प्रत्यागम CURLE_URL_MALFORMAT;

    यदि(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(दत्त, "Invalid IPv6 address format in '%.*s'",
            (पूर्णाङ्क)pp->host_user.len, pp->host_user.str);
      प्रत्यागम CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  अन्यथा {
#यदि_परिभाषितम् USE_IDN
    यदि(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode परिणाम;
      यदि(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        यदि(!pp->tmp_host_user)
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      परिणाम = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      pp->host.str = pp->tmp_host;
      pp->host.len = वाक्यदैर्घ्यम्(pp->host.str);
    }
    अन्यथा
#यदि_अन्तः
    यदि(अवलोकनम्_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      यदि(pp->host_user.len < MAX_IPADR_LEN) {
        वर्ण tmp[MAX_IPADR_LEN];
        स्मृतिं_प्रतिलिख(tmp, pp->host_user.str, pp->host_user.len);
        tmp[pp->host_user.len] = 0;
        pp->ipv6 = !Curl_is_ipv4addr(tmp);
      }
      अन्यथा
        pp->ipv6 = TRUE;
    }
    अन्यथा
      pp->host = pp->host_user;
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_peer_create(संरचना Curl_easy *दत्त,
                          ध्रुव संरचना Curl_scheme *scheme,
                          ध्रुव वर्ण *hostname,
                          uint16_t पत्तनम्,
                          संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  CURLcode परिणाम;

  Curl_peer_unlink(ppeer);
  स्मृतिं_पूरय(&pp, 0, परिमाण(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = वाक्यदैर्घ्यम्(hostname);
  pp.पत्तनम् = पत्तनम्;

  परिणाम = peer_parse_host(दत्त, &pp, TRUE);
  यदि(!परिणाम)
    परिणाम = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ध्रुव संरचना Curl_scheme *scheme,
                              ध्रुव वर्ण *मार्गः,
                              bool abstract_unix_socket,
                              संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  size_t pathlen = मार्गः ? वाक्यदैर्घ्यम्(मार्गः) : 0;
  CURLcode परिणाम = CURLE_OK;

  Curl_peer_unlink(ppeer);
  स्मृतिं_पूरय(&pp, 0, परिमाण(pp));
  यदि(!scheme)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(!pathlen)
    प्रत्यागम CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = मार्गः;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  परिणाम = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  प्रत्यागम परिणाम;
}
#यदि_अन्तः /* USE_UNIX_SOCKETS */

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
  प्रत्यागम (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

स्थिर bool peer_same_hostname(संरचना Curl_peer *p1, संरचना Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  प्रत्यागम (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!वाक्यं_तुलय(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

स्थिर bool peer_same_zoneid(ध्रुव वर्ण *z1, ध्रुव वर्ण *z2)
{
  प्रत्यागम (z1 == z2) ||
         (z1 && z2 && !वाक्यं_तुलय(z1, z2));
}

bool Curl_peer_same_destination(संरचना Curl_peer *p1, संरचना Curl_peer *p2)
{
  प्रत्यागम (p1 == p2) ||
         (p1 && p2 &&
          (p1->पत्तनम् == p2->पत्तनम्) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, संरचना Curl_easy *दत्त,
                            uint16_t पत्तनम्_override,
                            uint32_t scopeid_override,
                            संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  वर्ण *zoneid = रिक्त, *scheme = रिक्त, *hostname = रिक्त;
  CURLUcode uc;
  CURLcode परिणाम;

  Curl_peer_unlink(ppeer);
  स्मृतिं_पूरय(&pp, 0, परिमाण(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  यदि(uc)
    प्रत्यागम Curl_uc_गन्तव्यम्_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  यदि(!pp.scheme) {
    failf(दत्त, "Protocol \"%s\" not supported%s", scheme,
          दत्त->state.this_is_a_follow ? " (in redirect)" : "");
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
    गच्छ बहिर्गमनम्;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  यदि(uc) {
    यदि((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    अन्यथा {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }
  अन्यथा यदि(वाक्यदैर्घ्यम्(hostname) > MAX_URL_LEN) {
    failf(दत्त, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    परिणाम = CURLE_URL_MALFORMAT;
    गच्छ बहिर्गमनम्;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = वाक्यदैर्घ्यम्(pp.host_user.str);
  यदि(pp.host_user.len) {
    परिणाम = peer_parse_host(दत्त, &pp, FALSE);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  अन्यथा
    pp.host = pp.host_user;

  यदि(पत्तनम्_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    वर्ण portbuf[16];
    curl_msnprintf(portbuf, परिमाण(portbuf), "%d", पत्तनम्_override);
    uc = curl_url_समुच्चयः(uh, CURLUPART_पत्तनम्, portbuf, 0);
    यदि(uc) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा
      pp.पत्तनम् = पत्तनम्_override;
  }
  अन्यथा {
    uc = Curl_url_get_पत्तनम्(uh, &pp.पत्तनम्);
    यदि(uc) {
      यदि(uc == CURLUE_बहिर्गमनम्_सम्बन्धि_स्मृतिः) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
      अन्यथा यदि(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        परिणाम = CURLE_URL_MALFORMAT;
        गच्छ बहिर्गमनम्;
      }
      /* no port ok when not a network scheme */
    }
  }

  यदि(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  अन्यथा {
    यदि(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_बहिर्गमनम्_सम्बन्धि_स्मृतिः) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
    यदि(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = वाक्यदैर्घ्यम्(zoneid);
    }
  }

  परिणाम = peer_create(&pp, ppeer);
  यदि(परिणाम)
    failf(दत्त, "Error %d creating peer for %s:%u",
          (पूर्णाङ्क)परिणाम, pp.host_user.str, pp.पत्तनम्);

बहिर्गमनम्:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  प्रत्यागम परिणाम;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_गन्तव्यम्(संरचना Curl_easy *दत्त,
                                   ध्रुव संरचना Curl_peer *dest,
                                   ध्रुव वर्ण *connect_गन्तव्यम्,
                                   संरचना Curl_peer **ppeer)
{
  संरचना peer_parse pp;
  ध्रुव वर्ण *portstr = रिक्त;
  bool पत्तनम्_switch = FALSE;
  CURLcode परिणाम;

  Curl_peer_unlink(ppeer);
  स्मृतिं_पूरय(&pp, 0, परिमाण(pp));
  यदि(!connect_गन्तव्यम् || !*connect_गन्तव्यम्)
    प्रत्यागम CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  यदि(connect_गन्तव्यम्[0] == '[') {
    ध्रुव वर्ण *s = strchr(connect_गन्तव्यम् + 1, ']');
    यदि(!s) {
      failf(दत्त, "Invalid IPv6 address format in '%s'", connect_गन्तव्यम्);
      परिणाम = CURLE_SETOPT_OPTION_SYNTAX;
      गच्छ बहिर्गमनम्;
    }
    portstr = strchr(s, ':');
    pp.host_user.str = connect_गन्तव्यम्;
    pp.host_user.len = s - pp.host_user.str + 1;
    pp.ipv6 = TRUE;
  }
  अन्यथा {
    portstr = strchr(connect_गन्तव्यम्, ':');
    pp.host_user.str = connect_गन्तव्यम्;
    pp.host_user.len = portstr ?
      (size_t)(portstr - connect_गन्तव्यम्) : वाक्यदैर्घ्यम्(connect_गन्तव्यम्);
  }

  यदि(!pp.host_user.len) { /* no hostname found, only port switch */
    पत्तनम्_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = वाक्यदैर्घ्यम्(dest->user_hostname);
  }

  परिणाम = peer_parse_host(दत्त, &pp, FALSE);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!पत्तनम्_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  यदि(portstr && portstr[1]) {
    ध्रुव वर्ण *p = portstr + 1;
    curl_off_t portparse;
    यदि(curlx_str_सङ्ख्या(&p, &portparse, 0xffff)) {
      failf(दत्त, "No valid port number in '%s'", connect_गन्तव्यम्);
      परिणाम = CURLE_SETOPT_OPTION_SYNTAX;
      गच्छ बहिर्गमनम्;
    }
    pp.पत्तनम् = (uint16_t)portparse; /* we know it will fit */
  }
  अन्यथा
    pp.पत्तनम् = dest->पत्तनम्;

#यदि_अपरिभाषितम् USE_IPV6
  यदि(pp.ipv6) {
    failf(दत्त, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    परिणाम = CURLE_NOT_BUILT_अन्तर्;
    गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः
  यदि(pp.ipv6 && पत्तनम्_switch) {
    यदि(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = वाक्यदैर्घ्यम्(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  परिणाम = peer_create(&pp, ppeer);
  CURL_TRC_M(दत्त, "connect-to peer_create2 -> %d", (पूर्णाङ्क)परिणाम);

बहिर्गमनम्:
  CURL_TRC_M(दत्त, "parse connect_to peer: %s -> %d", connect_गन्तव्यम्, (पूर्णाङ्क)परिणाम);
  peer_parse_clear(&pp);
  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY

#यदि_परिभाषितम् USE_UNIX_SOCKETS
#परिभाषय UNIX_SOCKET_उपसर्गः "localhost"
#यदि_अन्तः

CURLcode Curl_scheme_गन्तव्यम्_proxytype(संरचना Curl_easy *दत्त,
                                  ध्रुव वर्ण *scheme,
                                  uint8_t *proxytype, ध्रुव वर्ण *url)
{
  यदि(!scheme)
    प्रत्यागम CURLE_OK;

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
    failf(दत्त, "Unsupported proxy scheme for \'%s\'", url);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  संरचना Curl_easy *दत्त,
                                  ध्रुव वर्ण *url,
                                  uint8_t proxytype,
                                  संरचना Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  संरचना peer_parse pp;
  वर्ण *scheme = रिक्त;
  वर्ण *portptr = रिक्त;
#यदि_परिभाषितम् USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#यदि_अन्तः
  CURLUcode uc;
  CURLcode परिणाम = CURLE_OK;

  Curl_peer_unlink(ppeer);
  स्मृतिं_पूरय(&pp, 0, परिमाण(pp));
  pp.पत्तनम् = CURL_DEFAULT_PROXY_पत्तनम्;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  यदि(uc) {
    यदि(uc == CURLUE_बहिर्गमनम्_सम्बन्धि_स्मृतिः) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
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
      failf(दत्त, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
      गच्छ बहिर्गमनम्;
    }
  }
  अन्यथा {
    pp.scheme = Curl_get_scheme(scheme);
    परिणाम = Curl_scheme_गन्तव्यम्_proxytype(दत्त, scheme, &proxytype, url);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  DEBUGASSERT(pp.scheme);

  यदि(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(दत्त, SSLSUPP_HTTPS_PROXY)) {
    failf(दत्त, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    परिणाम = CURLE_NOT_BUILT_अन्तर्;
    गच्छ बहिर्गमनम्;
  }

  विकल्प(pp.scheme->family) {
  पक्ष CURLPROTO_SOCKS:
#यदि_परिभाषितम् USE_UNIX_SOCKETS
    is_socks = TRUE;
#यदि_अन्तः
    भङ्ग;
  पक्ष CURLPROTO_HTTP:
    भङ्ग;
  स्वभाव:
    failf(दत्त, "Unsupported proxy protocol for \'%s\'", url);
    परिणाम = CURLE_COULDNT_CONNECT;
    गच्छ बहिर्गमनम्;
  }

  uc = curl_url_get(uh, CURLUPART_पत्तनम्, &portptr, CURLU_NO_DEFAULT_पत्तनम्);
  यदि(uc == CURLUE_बहिर्गमनम्_सम्बन्धि_स्मृतिः) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  यदि(portptr) {
    curl_off_t अङ्कः;
    ध्रुव वर्ण *p = portptr;
    यदि(!curlx_str_सङ्ख्या(&p, &अङ्कः, UINT16_MAX))
      pp.पत्तनम् = (uint16_t)अङ्कः;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  अन्यथा {
    /* No port in URL, take the set one or the scheme's default */
    यदि(दत्त->समुच्चयः.proxyport)
      pp.पत्तनम् = दत्त->समुच्चयः.proxyport;
    अन्यथा
      pp.पत्तनम् = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  यदि(uc) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = वाक्यदैर्घ्यम्(pp.tmp_host_user);

#यदि_परिभाषितम् USE_UNIX_SOCKETS
  यदि(is_socks && curl_strequal(UNIX_SOCKET_उपसर्गः, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_मार्गः, &pp.tmp_host, CURLU_URLDECODE);
    यदि(uc) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
    /* path will be "/", if no path was found */
    यदि(वाक्यं_तुलय("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = वाक्यदैर्घ्यम्(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    अन्यथा {
      pp.host = pp.host_user;
    }
  }
#यदि_अन्तः /* USE_UNIX_SOCKETS */

  यदि(!pp.host.len) {
    परिणाम = peer_parse_host(दत्त, &pp, FALSE);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  यदि(uc == CURLUE_बहिर्गमनम्_सम्बन्धि_स्मृतिः) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  यदि(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = वाक्यदैर्घ्यम्(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  परिणाम = peer_create(&pp, ppeer);

बहिर्गमनम्:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#यदि_परिभाषितम् DEBUGBUILD
  यदि(!परिणाम)
    DEBUGASSERT(*ppeer);
#यदि_अन्तः
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_PROXY */
