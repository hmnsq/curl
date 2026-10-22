#अगर_अघोषित HEADER_CURL_SOCKADDR_H
#घोषणा HEADER_CURL_SOCKADDR_H
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

काष्ठा Curl_sockaddr_storage {
  जोड़ {
    काष्ठा sockaddr sa;
    काष्ठा sockaddr_in sa_in;
#अगर_घोषित USE_IPV6
    काष्ठा sockaddr_in6 sa_in6;
#पूर्ण_अगर
#अगर_घोषित HAVE_STRUCT_SOCKADDR_STORAGE
    काष्ठा sockaddr_storage sa_stor;
#अन्यथा
    अक्षर cbuf[256];   /* this should be big enough to fit a lot */
#पूर्ण_अगर
  } बफर;
};

/*
 * The Curl_sockaddr_ex structure is libcurl's external API curl_sockaddr
 * structure with enough space available to directly hold any
 * protocol-specific address structures. The variable declared here will be
 * used to pass / receive data to/from the fopensocket callback if this has
 * been set, before that, it is initialized from parameters.
 */
काष्ठा Curl_sockaddr_ex {
  पूर्णांक family;
  पूर्णांक socktype;
  पूर्णांक protocol;
  अचिन्हित पूर्णांक addrlen;
  जोड़ {
    काष्ठा sockaddr sa;
    काष्ठा Curl_sockaddr_storage buf;
  } addr;
};

#पूर्ण_अगर /* HEADER_CURL_SOCKADDR_H */
