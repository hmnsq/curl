#ifndef HEADER_CURL_ADDRINFO_H
#define HEADER_CURL_ADDRINFO_H
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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#  include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#endif

#ifdef __VMS
#  include <in.h>
#  include <inet.h>
#endif

संरचना Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

संरचना Curl_addrinfo {
  पूर्णाङ्क                   ai_flags;
  पूर्णाङ्क                   ai_family;
  पूर्णाङ्क                   ai_socktype;
  पूर्णाङ्क                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  वर्ण                 *ai_canonname;
  संरचना sockaddr      *ai_addr;
  संरचना Curl_addrinfo *ai_next;
};

शून्य Curl_freeaddrinfo(संरचना Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
संरचना Curl_addrinfo *Curl_addrinfo_get(संरचना Curl_addrinfo *ai,
                                        पूर्णाङ्क ai_family,
                                        unsigned पूर्णाङ्क n);

#ifdef HAVE_GETADDRINFO
पूर्णाङ्क Curl_getaddrinfo_ex(ध्रुव वर्ण *nodename,
                        ध्रुव वर्ण *servname,
                        ध्रुव संरचना addrinfo *hints,
                        संरचना Curl_addrinfo **result);
#endif

#जइ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
संरचना Curl_addrinfo *Curl_he2ai(ध्रुव संरचना hostent *he, पूर्णाङ्क port);
#endif

bool Curl_is_ipv4addr(ध्रुव वर्ण *address);
bool Curl_is_ipaddr(ध्रुव वर्ण *address);
bool Curl_looks_like_ipv6(ध्रुव वर्ण *s, size_t len, bool maybe_url_encoded,
                          संरचना Curl_str *host, संरचना Curl_str *zone);

CURLcode Curl_str2addr(ध्रुव वर्ण *dotted, uint16_t port,
                       संरचना Curl_addrinfo **addrp);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ध्रुव वर्ण *path, bool abstract,
                        संरचना Curl_addrinfo **paddr);
#endif

#जइ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
शून्य curl_dbg_freeaddrinfo(संरचना addrinfo *freethis, पूर्णाङ्क पङ्क्तिः,
                           ध्रुव वर्ण *source);
#endif

#जइ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
पूर्णाङ्क curl_dbg_getaddrinfo(ध्रुव वर्ण *hostname, ध्रुव वर्ण *service,
                         ध्रुव संरचना addrinfo *hints,
                         संरचना addrinfo **result, पूर्णाङ्क पङ्क्तिः,
                         ध्रुव वर्ण *source);
#endif

#ifdef HAVE_GETADDRINFO
#ifdef USE_RESOLVE_ON_IPS
शून्य Curl_addrinfo_set_port(संरचना Curl_addrinfo *addrinfo, पूर्णाङ्क port);
#अण्णहा
#define Curl_addrinfo_set_port(x, y)
#endif
#endif

#endif /* HEADER_CURL_ADDRINFO_H */
