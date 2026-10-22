#اگر_غیر_تعریف_شدہ سرخی_CURL_SOCKADDR_H
#تعریف_کرو سرخی_CURL_SOCKADDR_H
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
#شامل "curl_setup.h"

ساخت Curl_sockaddr_storage {
  اتحاد {
    ساخت sockaddr sa;
    ساخت sockaddr_in sa_اندر;
#اگر_تعریف_شدہ USE_IPV6
    ساخت sockaddr_in6 sa_in6;
#اگر_ختم
#اگر_تعریف_شدہ HAVE_STRUCT_SOCKADDR_STORAGE
    ساخت sockaddr_storage sa_stor;
#ورنہ
    حرف cbuf[256];   /* this should be big enough to fit a lot */
#اگر_ختم
  } بفر;
};

/*
 * The Curl_sockaddr_ex structure is libcurl's external API curl_sockaddr
 * structure with enough space available to directly hold any
 * protocol-specific address structures. The variable declared here will be
 * used to pass / receive data to/from the fopensocket callback if this has
 * been set, before that, it is initialized from parameters.
 */
ساخت Curl_sockaddr_ex {
  صحیح family;
  صحیح socktype;
  صحیح protocol;
  غیرعلامتی صحیح addrlen;
  اتحاد {
    ساخت sockaddr sa;
    ساخت Curl_sockaddr_storage buf;
  } addr;
};

#اگر_ختم /* HEADER_CURL_SOCKADDR_H */
