#अगर_अघोषित HEADER_CURL_ADDRINFO_H
#घोषणा HEADER_CURL_ADDRINFO_H
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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#  include <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#  include <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#पूर्ण_अगर

#अगर_घोषित __VMS
#  include <in.h>
#  include <inet.h>
#पूर्ण_अगर

काष्ठा Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

काष्ठा Curl_addrinfo {
  पूर्णांक                   ai_flags;
  पूर्णांक                   ai_family;
  पूर्णांक                   ai_socktype;
  पूर्णांक                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  अक्षर                 *ai_canonname;
  काष्ठा sockaddr      *ai_addr;
  काष्ठा Curl_addrinfo *ai_अग्र;
};

व्योम Curl_freeaddrinfo(काष्ठा Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
काष्ठा Curl_addrinfo *Curl_addrinfo_get(काष्ठा Curl_addrinfo *ai,
                                        पूर्णांक ai_family,
                                        अचिन्हित पूर्णांक n);

#अगर_घोषित HAVE_GETADDRINFO
पूर्णांक Curl_getaddrinfo_ex(स्थिर अक्षर *nodename,
                        स्थिर अक्षर *servname,
                        स्थिर काष्ठा addrinfo *hints,
                        काष्ठा Curl_addrinfo **परिणाम);
#पूर्ण_अगर

#अगर !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
काष्ठा Curl_addrinfo *Curl_he2ai(स्थिर काष्ठा hostent *he, पूर्णांक port);
#पूर्ण_अगर

bool Curl_is_ipv4addr(स्थिर अक्षर *address);
bool Curl_is_ipaddr(स्थिर अक्षर *address);
bool Curl_looks_like_ipv6(स्थिर अक्षर *s, माप_प्रकार len, bool maybe_url_encoded,
                          काष्ठा Curl_str *host, काष्ठा Curl_str *zone);

CURLcode Curl_str2addr(स्थिर अक्षर *dotted, uint16_t port,
                       काष्ठा Curl_addrinfo **addrp);

#अगर_घोषित USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(स्थिर अक्षर *path, bool abstract,
                        काष्ठा Curl_addrinfo **paddr);
#पूर्ण_अगर

#अगर defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
व्योम curl_dbg_freeaddrinfo(काष्ठा addrinfo *freethis, पूर्णांक पंक्ति,
                           स्थिर अक्षर *source);
#पूर्ण_अगर

#अगर defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
पूर्णांक curl_dbg_getaddrinfo(स्थिर अक्षर *hostname, स्थिर अक्षर *service,
                         स्थिर काष्ठा addrinfo *hints,
                         काष्ठा addrinfo **परिणाम, पूर्णांक पंक्ति,
                         स्थिर अक्षर *source);
#पूर्ण_अगर

#अगर_घोषित HAVE_GETADDRINFO
#अगर_घोषित USE_RESOLVE_ON_IPS
व्योम Curl_addrinfo_set_port(काष्ठा Curl_addrinfo *addrinfo, पूर्णांक port);
#अन्यथा
#घोषणा Curl_addrinfo_set_port(x, y)
#पूर्ण_अगर
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_ADDRINFO_H */
