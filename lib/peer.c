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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NET_IF_H
#समावेश <net/if.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_IPHLPAPI_H
#समावेश <Iphlpapi.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_PARAM_H
#समावेश <sys/param.h>
#पूर्ण_अगर

#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_SYS_UN_H
#समावेश <sys/un.h>
#पूर्ण_अगर

#अगर defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#अगर defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#समावेश <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#पूर्ण_अगर
#समावेश <iphlpapi.h>
#पूर्ण_अगर

#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "protocol.h"
#समावेश "http_proxy.h"
#समावेश "idn.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/strparse.h"
#समावेश "peer.h"
#समावेश "urldata.h"
#समावेश "url.h"
#समावेश "urlapi-int.h"
#समावेश "vtls/vtls.h"

काष्ठा peer_parse {
  स्थिर काष्ठा Curl_scheme *scheme;
  काष्ठा Curl_str host_user;
  काष्ठा Curl_str host;
  काष्ठा Curl_str zoneid;
  अक्षर *tmp_host_user;
  अक्षर *tmp_host;
  अक्षर *tmp_zoneid;
  uint32_t scopeid;
  uint16_t port;
  bool ipv6;
  bool unix_socket;
  bool abstract_uds;
};

अटल व्योम peer_parse_clear(काष्ठा peer_parse *pp)
{
  curlx_free(pp->tmp_host_user);
  curlx_free(pp->tmp_host);
  curlx_free(pp->tmp_zoneid);
  स_रखो(pp, 0, माप(*pp));
}

अटल CURLcode peer_create(काष्ठा peer_parse *pp,
                            काष्ठा Curl_peer **ppeer)
{
  काष्ठा Curl_peer *peer = शून्य;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार zone_alen = 0, host_alen = 0;

  अगर(!pp || !pp->scheme)
    वापस CURLE_FAILED_INIT;
  अगर(!pp->host.len && !(pp->scheme->flags & PROTOPT_NONETWORK))
    वापस CURLE_FAILED_INIT;

  अगर((pp->host.str != pp->host_user.str) ||
     (pp->host.len != pp->host_user.len)) {
    host_alen = pp->host.len + 1;
  }
  zone_alen = pp->zoneid.len ? (pp->zoneid.len + 1) : 0;

  /* null-terminator already part of struct */
  peer = curlx_calloc(1, माप(*peer) +
                         pp->host_user.len + host_alen + zone_alen);
  अगर(!peer) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  peer->refcount = 1;
  peer->scheme = pp->scheme;
  peer->hostname = peer->user_hostname;
  peer->port = pp->port;
  peer->scopeid = pp->scopeid;
  peer->ipv6 = pp->ipv6;
  peer->unix_socket = pp->unix_socket;
  peer->abstract_uds = pp->abstract_uds;

  अगर(pp->host_user.len)
    स_नकल(peer->user_hostname, pp->host_user.str, pp->host_user.len);

  अगर(host_alen) {
    peer->hostname = peer->user_hostname + pp->host_user.len + 1;
    स_नकल(peer->hostname, pp->host.str, pp->host.len);
  }

  अगर(zone_alen) {
    peer->zoneid = peer->user_hostname + pp->host_user.len + 1 + host_alen;
    स_नकल(peer->zoneid, pp->zoneid.str, pp->zoneid.len);
#अगर_घोषित USE_IPV6
    /* Determine scope_id if not already provided */
    अगर(!peer->scopeid) {
      स्थिर अक्षर *p = peer->zoneid;
      curl_off_t scope;
      अगर(!curlx_str_संख्या(&p, &scope, अच_पूर्णांक_उच्च) && !*p) {
        /* A plain number, use it directly as a scope id. */
        peer->scopeid = (uint32_t)scope;
      }
#अगर_घोषित HAVE_IF_NAMETOINDEX
      अन्यथा {
        /* Zone identifier is not fully numeric */
        अचिन्हित पूर्णांक idx = 0;
        idx = if_nametoindex(peer->zoneid);
        अगर(idx) {
          peer->scopeid = (uint32_t)idx;
        }
        अन्यथा {
          /* Do we want to return an error here? */
        }
      }
#पूर्ण_अगर /* HAVE_IF_NAMETOINDEX */
    }
#पूर्ण_अगर /* USE_IPV6 */
  }

out:
  अगर(!परिणाम)
    *ppeer = peer;
  अन्यथा
    Curl_peer_unlink(&peer);
  वापस परिणाम;
}

अटल CURLcode peer_parse_host(काष्ठा Curl_easy *तथ्य,
                                काष्ठा peer_parse *pp,
                                bool scan_for_ipv6)
{
  अगर(!pp || !pp->host_user.str || !pp->host_user.len)
    वापस CURLE_FAILED_INIT;

  अगर(pp->host_user.str[0] == '[') {
    स्थिर अक्षर *s = pp->host_user.str + 1;
    काष्ठा Curl_str tmp;
    अगर(curlx_str_until(&s, &tmp, pp->host_user.len - 1, ']'))
      वापस CURLE_URL_MALFORMAT;

    अगर(!Curl_looks_like_ipv6(tmp.str, tmp.len, TRUE,
                             &pp->host, &pp->zoneid)) {
      failf(तथ्य, "Invalid IPv6 address format in '%.*s'",
            (पूर्णांक)pp->host_user.len, pp->host_user.str);
      वापस CURLE_URL_MALFORMAT;
    }
    pp->ipv6 = TRUE;
  }
  अन्यथा {
#अगर_घोषित USE_IDN
    अगर(!Curl_is_ASCII_str(&pp->host_user)) {
      CURLcode परिणाम;
      अगर(!pp->tmp_host_user) {
        /* need a null-terminated string for IDN */
        pp->tmp_host_user = curlx_memdup0(pp->host_user.str,
                                          pp->host_user.len);
        अगर(!pp->tmp_host_user)
          वापस CURLE_OUT_OF_MEMORY;
      }
      परिणाम = Curl_idn_decode(pp->tmp_host_user, &pp->tmp_host);
      अगर(परिणाम)
        वापस परिणाम;
      pp->host.str = pp->tmp_host;
      pp->host.len = म_माप(pp->host.str);
    }
    अन्यथा
#पूर्ण_अगर
    अगर(scan_for_ipv6 &&
       Curl_looks_like_ipv6(pp->host_user.str, pp->host_user.len, TRUE,
                            &pp->host, &pp->zoneid)) {
      अगर(pp->host_user.len < MAX_IPADR_LEN) {
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
  वापस CURLE_OK;
}

CURLcode Curl_peer_create(काष्ठा Curl_easy *तथ्य,
                          स्थिर काष्ठा Curl_scheme *scheme,
                          स्थिर अक्षर *hostname,
                          uint16_t port,
                          काष्ठा Curl_peer **ppeer)
{
  काष्ठा peer_parse pp;
  CURLcode परिणाम;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, माप(pp));
  pp.scheme = scheme;
  pp.host_user.str = hostname;
  pp.host_user.len = म_माप(hostname);
  pp.port = port;

  परिणाम = peer_parse_host(तथ्य, &pp, TRUE);
  अगर(!परिणाम)
    परिणाम = peer_create(&pp, ppeer);

  peer_parse_clear(&pp);
  वापस परिणाम;
}

#अगर_घोषित USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(स्थिर काष्ठा Curl_scheme *scheme,
                              स्थिर अक्षर *path,
                              bool abstract_unix_socket,
                              काष्ठा Curl_peer **ppeer)
{
  काष्ठा peer_parse pp;
  माप_प्रकार pathlen = path ? म_माप(path) : 0;
  CURLcode परिणाम = CURLE_OK;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, माप(pp));
  अगर(!scheme)
    वापस CURLE_FAILED_INIT;
  अगर(!pathlen)
    वापस CURLE_FAILED_INIT;

  pp.scheme = scheme;
  pp.host_user.str = pp.host.str = path;
  pp.host_user.len = pp.host.len = pathlen;
  pp.unix_socket = TRUE;
  pp.abstract_uds = abstract_unix_socket;

  परिणाम = peer_create(&pp, ppeer);
  peer_parse_clear(&pp);
  वापस परिणाम;
}
#पूर्ण_अगर /* USE_UNIX_SOCKETS */

व्योम Curl_peer_link(काष्ठा Curl_peer **pdest, काष्ठा Curl_peer *src)
{
  अगर(*pdest != src) {
    Curl_peer_unlink(pdest);
    *pdest = src;
    अगर(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

व्योम Curl_peer_unlink(काष्ठा Curl_peer **ppeer)
{
  अगर(*ppeer) {
    काष्ठा Curl_peer *peer = *ppeer;

    DEBUGASSERT(peer->refcount);
    *ppeer = शून्य;
    अगर(peer->refcount)
      peer->refcount--;
    अगर(!peer->refcount) {
      curlx_free(peer);
    }
  }
}

bool Curl_peer_equal(काष्ठा Curl_peer *p1, काष्ठा Curl_peer *p2)
{
  वापस (p1 == p2) ||
         (p1 && p2 &&
          (p1->scheme == p2->scheme) &&
          Curl_peer_same_destination(p1, p2));
}

अटल bool peer_same_hostname(काष्ठा Curl_peer *p1, काष्ठा Curl_peer *p2)
{
  /* UNIX domain socket paths must be compared case-sensitive,
   * as many filesystem are like that. */
  वापस (p1->unix_socket == p2->unix_socket) &&
         (p1->abstract_uds == p2->abstract_uds) &&
         (p1->ipv6 == p2->ipv6) &&
         /* unix_socket paths are case-sensitive, hostnames are not.
          * Do strcmp() first on both, since we do a lot of matches
          * on the same names and platform strcmp is way faster. */
         (!म_भेद(p1->hostname, p2->hostname) ||
          (!p1->unix_socket &&
           curl_strequal(p1->hostname, p2->hostname)));
}

अटल bool peer_same_zoneid(स्थिर अक्षर *z1, स्थिर अक्षर *z2)
{
  वापस (z1 == z2) ||
         (z1 && z2 && !म_भेद(z1, z2));
}

bool Curl_peer_same_destination(काष्ठा Curl_peer *p1, काष्ठा Curl_peer *p2)
{
  वापस (p1 == p2) ||
         (p1 && p2 &&
          (p1->port == p2->port) &&
          peer_same_hostname(p1, p2) &&
          (p1->scopeid == p2->scopeid) &&
          (p1->scopeid || peer_same_zoneid(p1->zoneid, p2->zoneid)));
}

CURLcode Curl_peer_from_url(CURLU *uh, काष्ठा Curl_easy *तथ्य,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            काष्ठा Curl_peer **ppeer)
{
  काष्ठा peer_parse pp;
  अक्षर *zoneid = शून्य, *scheme = शून्य, *hostname = शून्य;
  CURLUcode uc;
  CURLcode परिणाम;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, माप(pp));

  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme, 0);
  अगर(uc)
    वापस Curl_uc_to_curlcode(uc);
  pp.scheme = Curl_get_scheme(scheme);
  अगर(!pp.scheme) {
    failf(तथ्य, "Protocol \"%s\" not supported%s", scheme,
          तथ्य->state.this_is_a_follow ? " (in redirect)" : "");
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
    जाओ out;
  }

  uc = curl_url_get(uh, CURLUPART_HOST, &hostname, 0);
  अगर(uc) {
    अगर((uc == CURLUE_NO_HOST) && (pp.scheme->flags & PROTOPT_NONETWORK))
      ; /* acceptable */
    अन्यथा {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }
  अन्यथा अगर(म_माप(hostname) > MAX_URL_LEN) {
    failf(तथ्य, "Too long hostname (maximum is %d)", MAX_URL_LEN);
    परिणाम = CURLE_URL_MALFORMAT;
    जाओ out;
  }

  pp.host_user.str = hostname ? hostname : "";
  pp.host_user.len = म_माप(pp.host_user.str);
  अगर(pp.host_user.len) {
    परिणाम = peer_parse_host(तथ्य, &pp, FALSE);
    अगर(परिणाम)
      जाओ out;
  }
  अन्यथा
    pp.host = pp.host_user;

  अगर(port_override) {
    /* if set, we use this instead of the port possibly given in the URL */
    अक्षर portbuf[16];
    curl_msnprintf(portbuf, माप(portbuf), "%d", port_override);
    uc = curl_url_set(uh, CURLUPART_PORT, portbuf, 0);
    अगर(uc) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
    अन्यथा
      pp.port = port_override;
  }
  अन्यथा {
    uc = Curl_url_get_port(uh, &pp.port);
    अगर(uc) {
      अगर(uc == CURLUE_OUT_OF_MEMORY) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
      अन्यथा अगर(!(pp.scheme->flags & PROTOPT_NONETWORK)) {
        परिणाम = CURLE_URL_MALFORMAT;
        जाओ out;
      }
      /* no port ok when not a network scheme */
    }
  }

  अगर(scopeid_override)
    /* Override any scope id from an URL zone. */
    pp.scopeid = scopeid_override;
  अन्यथा {
    अगर(curl_url_get(uh, CURLUPART_ZONEID, &zoneid, 0) ==
       CURLUE_OUT_OF_MEMORY) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
    अगर(zoneid) {
      pp.zoneid.str = zoneid;
      pp.zoneid.len = म_माप(zoneid);
    }
  }

  परिणाम = peer_create(&pp, ppeer);
  अगर(परिणाम)
    failf(तथ्य, "Error %d creating peer for %s:%u",
          (पूर्णांक)परिणाम, pp.host_user.str, pp.port);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
  curlx_free(hostname);
  curlx_free(zoneid);
  वापस परिणाम;
}

/* Parse a "host:port" string to connect to into a peer.
 * IPv6 addresses might appear in brackets or without them. */
CURLcode Curl_peer_from_connect_to(काष्ठा Curl_easy *तथ्य,
                                   स्थिर काष्ठा Curl_peer *dest,
                                   स्थिर अक्षर *connect_to,
                                   काष्ठा Curl_peer **ppeer)
{
  काष्ठा peer_parse pp;
  स्थिर अक्षर *portstr = शून्य;
  bool port_switch = FALSE;
  CURLcode परिणाम;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, माप(pp));
  अगर(!connect_to || !*connect_to)
    वापस CURLE_FAILED_INIT;

  pp.scheme = dest->scheme;

  /* detect and extract RFC6874-style IPv6-addresses */
  अगर(connect_to[0] == '[') {
    स्थिर अक्षर *s = म_अक्षर(connect_to + 1, ']');
    अगर(!s) {
      failf(तथ्य, "Invalid IPv6 address format in '%s'", connect_to);
      परिणाम = CURLE_SETOPT_OPTION_SYNTAX;
      जाओ out;
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

  अगर(!pp.host_user.len) { /* no hostname found, only port switch */
    port_switch = TRUE;
    pp.host_user.str = dest->user_hostname;
    pp.host_user.len = म_माप(dest->user_hostname);
  }

  परिणाम = peer_parse_host(तथ्य, &pp, FALSE);
  अगर(परिणाम)
    जाओ out;
  /* On a port switch, ipv6 must stay the same */
  DEBUGASSERT(!port_switch || ((bool)pp.ipv6 == (bool)dest->ipv6));

  अगर(portstr && portstr[1]) {
    स्थिर अक्षर *p = portstr + 1;
    curl_off_t portparse;
    अगर(curlx_str_संख्या(&p, &portparse, 0xffff)) {
      failf(तथ्य, "No valid port number in '%s'", connect_to);
      परिणाम = CURLE_SETOPT_OPTION_SYNTAX;
      जाओ out;
    }
    pp.port = (uint16_t)portparse; /* we know it will fit */
  }
  अन्यथा
    pp.port = dest->port;

#अगर_अघोषित USE_IPV6
  अगर(pp.ipv6) {
    failf(तथ्य, "Use of IPv6 in *_CONNECT_TO without IPv6 support built-in");
    परिणाम = CURLE_NOT_BUILT_IN;
    जाओ out;
  }
#पूर्ण_अगर
  अगर(pp.ipv6 && port_switch) {
    अगर(dest->zoneid) {
      pp.zoneid.str = dest->zoneid;
      pp.zoneid.len = म_माप(dest->zoneid);
    }
    pp.scopeid = dest->scopeid;
  }

  परिणाम = peer_create(&pp, ppeer);
  CURL_TRC_M(तथ्य, "connect-to peer_create2 -> %d", (पूर्णांक)परिणाम);

out:
  CURL_TRC_M(तथ्य, "parse connect_to peer: %s -> %d", connect_to, (पूर्णांक)परिणाम);
  peer_parse_clear(&pp);
  वापस परिणाम;
}

#अगर_अघोषित CURL_DISABLE_PROXY

#अगर_घोषित USE_UNIX_SOCKETS
#घोषणा UNIX_SOCKET_PREFIX "localhost"
#पूर्ण_अगर

CURLcode Curl_scheme_to_proxytype(काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *scheme,
                                  uint8_t *proxytype, स्थिर अक्षर *url)
{
  अगर(!scheme)
    वापस CURLE_OK;

  अगर(curl_strequal("https", scheme)) {
    अगर(*proxytype != CURLPROXY_HTTPS2 && *proxytype != CURLPROXY_HTTPS3)
      *proxytype = CURLPROXY_HTTPS;
  }
  अन्यथा अगर(curl_strequal("socks5h", scheme))
    *proxytype = CURLPROXY_SOCKS5_HOSTNAME;
  अन्यथा अगर(curl_strequal("socks5", scheme))
    *proxytype = CURLPROXY_SOCKS5;
  अन्यथा अगर(curl_strequal("socks4a", scheme))
    *proxytype = CURLPROXY_SOCKS4A;
  अन्यथा अगर(curl_strequal("socks4", scheme) || curl_strequal("socks", scheme))
    *proxytype = CURLPROXY_SOCKS4;
  अन्यथा अगर(curl_strequal("http", scheme)) {
    अगर(*proxytype != CURLPROXY_HTTP_1_0)
      *proxytype = CURLPROXY_HTTP;
  }
  अन्यथा {
    /* Any other xxx:// reject! */
    failf(तथ्य, "Unsupported proxy scheme for \'%s\'", url);
    वापस CURLE_COULDNT_CONNECT;
  }
  वापस CURLE_OK;
}

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *url,
                                  uint8_t proxytype,
                                  काष्ठा Curl_peer **ppeer,
                                  uint8_t *pproxytype)
{
  काष्ठा peer_parse pp;
  अक्षर *scheme = शून्य;
  अक्षर *portptr = शून्य;
#अगर_घोषित USE_UNIX_SOCKETS
  bool is_socks = FALSE;
#पूर्ण_अगर
  CURLUcode uc;
  CURLcode परिणाम = CURLE_OK;

  Curl_peer_unlink(ppeer);
  स_रखो(&pp, 0, माप(pp));
  pp.port = CURL_DEFAULT_PROXY_PORT;
  uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_NO_GUESS_SCHEME);
  अगर(uc) {
    अगर(uc == CURLUE_OUT_OF_MEMORY) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
    /* URL came without scheme, the passed `proxytype` determines it */
    चयन(proxytype) {
    हाल CURLPROXY_HTTP:
    हाल CURLPROXY_HTTP_1_0:
      pp.scheme = &Curl_scheme_http;
      अवरोध;
    हाल CURLPROXY_HTTPS:
    हाल CURLPROXY_HTTPS2:
    हाल CURLPROXY_HTTPS3:
      pp.scheme = &Curl_scheme_https;
      अवरोध;
    हाल CURLPROXY_SOCKS4:
      pp.scheme = &Curl_scheme_socks4;
      अवरोध;
    हाल CURLPROXY_SOCKS4A:
      pp.scheme = &Curl_scheme_socks4a;
      अवरोध;
    हाल CURLPROXY_SOCKS5:
      pp.scheme = &Curl_scheme_socks5;
      अवरोध;
    हाल CURLPROXY_SOCKS5_HOSTNAME:
      pp.scheme = &Curl_scheme_socks5h;
      अवरोध;
    शेष:
      failf(तथ्य, "Unsupported proxy type %u for \'%s\'", proxytype, url);
      परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
      जाओ out;
    }
  }
  अन्यथा {
    pp.scheme = Curl_get_scheme(scheme);
    परिणाम = Curl_scheme_to_proxytype(तथ्य, scheme, &proxytype, url);
    अगर(परिणाम)
      जाओ out;
  }
  DEBUGASSERT(pp.scheme);

  अगर(CURL_PROXY_IS_HTTPS(proxytype) &&
     !Curl_ssl_supports(तथ्य, SSLSUPP_HTTPS_PROXY)) {
    failf(तथ्य, "Unsupported proxy \'%s\', libcurl is built without the "
          "HTTPS-proxy support.", url);
    परिणाम = CURLE_NOT_BUILT_IN;
    जाओ out;
  }

  चयन(pp.scheme->family) {
  हाल CURLPROTO_SOCKS:
#अगर_घोषित USE_UNIX_SOCKETS
    is_socks = TRUE;
#पूर्ण_अगर
    अवरोध;
  हाल CURLPROTO_HTTP:
    अवरोध;
  शेष:
    failf(तथ्य, "Unsupported proxy protocol for \'%s\'", url);
    परिणाम = CURLE_COULDNT_CONNECT;
    जाओ out;
  }

  uc = curl_url_get(uh, CURLUPART_PORT, &portptr, CURLU_NO_DEFAULT_PORT);
  अगर(uc == CURLUE_OUT_OF_MEMORY) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  अगर(portptr) {
    curl_off_t अंक;
    स्थिर अक्षर *p = portptr;
    अगर(!curlx_str_संख्या(&p, &अंक, UINT16_MAX))
      pp.port = (uint16_t)अंक;
    /* Should we not error out when the port number is invalid? */
    curlx_free(portptr);
  }
  अन्यथा {
    /* No port in URL, take the set one or the scheme's default */
    अगर(तथ्य->set.proxyport)
      pp.port = तथ्य->set.proxyport;
    अन्यथा
      pp.port = pp.scheme->defport;
  }

  /* now, clone the proxy hostname */
  uc = curl_url_get(uh, CURLUPART_HOST, &pp.tmp_host_user, CURLU_URLDECODE);
  अगर(uc) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  pp.host_user.str = pp.tmp_host_user;
  pp.host_user.len = म_माप(pp.tmp_host_user);

#अगर_घोषित USE_UNIX_SOCKETS
  अगर(is_socks && curl_strequal(UNIX_SOCKET_PREFIX, pp.tmp_host_user)) {
    uc = curl_url_get(uh, CURLUPART_PATH, &pp.tmp_host, CURLU_URLDECODE);
    अगर(uc) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
    /* path will be "/", if no path was found */
    अगर(म_भेद("/", pp.tmp_host)) {
      pp.host.str = pp.tmp_host;
      pp.host.len = म_माप(pp.tmp_host);
      pp.unix_socket = TRUE;
    }
    अन्यथा {
      pp.host = pp.host_user;
    }
  }
#पूर्ण_अगर /* USE_UNIX_SOCKETS */

  अगर(!pp.host.len) {
    परिणाम = peer_parse_host(तथ्य, &pp, FALSE);
    अगर(परिणाम)
      जाओ out;
  }

  uc = curl_url_get(uh, CURLUPART_ZONEID, &pp.tmp_zoneid, 0);
  अगर(uc == CURLUE_OUT_OF_MEMORY) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  अगर(pp.tmp_zoneid) {
    pp.zoneid.str = pp.tmp_zoneid;
    pp.zoneid.len = म_माप(pp.tmp_zoneid);
  }

  *pproxytype = proxytype;
  परिणाम = peer_create(&pp, ppeer);

out:
  peer_parse_clear(&pp);
  curlx_free(scheme);
#अगर_घोषित DEBUGBUILD
  अगर(!परिणाम)
    DEBUGASSERT(*ppeer);
#पूर्ण_अगर
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_PROXY */
