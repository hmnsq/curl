#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_SOCKADDR_H
#وصف_ڪريو سرو_CURL_SOCKADDR_H
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
#ضمن "curl_setup.h"

بنية Curl_sockaddr_storage {
  اتحاد {
    بنية sockaddr sa;
    بنية sockaddr_in sa_اندريون;
#جيڪڏهن_وصف_ٿيل USE_IPV6
    بنية sockaddr_in6 sa_in6;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_STRUCT_SOCKADDR_STORAGE
    بنية sockaddr_storage sa_stor;
#وإلا
    حرف cbuf[256];   /* this should be big enough to fit a lot */
#جيڪڏهن_پورو
  } بفر;
};

/*
 * The Curl_sockaddr_ex structure is libcurl's external API curl_sockaddr
 * structure with enough space available to directly hold any
 * protocol-specific address structures. The variable declared here will be
 * used to pass / receive data to/from the fopensocket callback if this has
 * been set, before that, it is initialized from parameters.
 */
بنية Curl_sockaddr_ex {
  صحيح family;
  صحيح socktype;
  صحيح protocol;
  بنانشان صحيح addrlen;
  اتحاد {
    بنية sockaddr sa;
    بنية Curl_sockaddr_storage buf;
  } addr;
};

#جيڪڏهن_پورو /* HEADER_CURL_SOCKADDR_H */
