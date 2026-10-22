#अगर_अघोषित HEADER_FAKE_ADDRINFO_H
#घोषणा HEADER_FAKE_ADDRINFO_H
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

#अगर_घोषित USE_ARES
#समावेश <ares.h>
#पूर्ण_अगर

#अगर defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(USE_ARES) && (ARES_VERSION >= 0x011a00) /* >= 1.26.0 */
#घोषणा USE_FAKE_GETADDRINFO 1
#पूर्ण_अगर

#अगर_घोषित USE_FAKE_GETADDRINFO

#अगर_घोषित HAVE_NETDB_H
#  include <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#पूर्ण_अगर

व्योम r_freeaddrinfo(काष्ठा addrinfo *cahead);
पूर्णांक r_getaddrinfo(स्थिर अक्षर *गाँठ,
                  स्थिर अक्षर *service,
                  स्थिर काष्ठा addrinfo *hints,
                  काष्ठा addrinfo **res);
#पूर्ण_अगर /* USE_FAKE_GETADDRINFO */

#पूर्ण_अगर /* HEADER_FAKE_ADDRINFO_H */
