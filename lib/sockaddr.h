#ifndef HEADER_CURL_SOCKADDR_H
#define HEADER_CURL_SOCKADDR_H
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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

ସଂରଚନା Curl_sockaddr_storage {
  ସଂଘ {
    ସଂରଚନା sockaddr sa;
    ସଂରଚନା sockaddr_in sa_in;
#ifdef USE_IPV6
    ସଂରଚନା sockaddr_in6 sa_in6;
#endif
#ifdef HAVE_STRUCT_SOCKADDR_STORAGE
    ସଂରଚନା sockaddr_storage sa_stor;
#ଅନ୍ୟଥା
    ଅକ୍ଷର cbuf[256];   /* this should be big enough to fit a lot */
#endif
  } buffer;
};

/*
 * The Curl_sockaddr_ex structure is libcurl's external API curl_sockaddr
 * structure with enough space available to directly hold any
 * protocol-specific address structures. The variable declared here will be
 * used to pass / receive data to/from the fopensocket callback if this has
 * been set, before that, it is initialized from parameters.
 */
ସଂରଚନା Curl_sockaddr_ex {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା family;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା socktype;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା protocol;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା addrlen;
  ସଂଘ {
    ସଂରଚନା sockaddr sa;
    ସଂରଚନା Curl_sockaddr_storage buf;
  } addr;
};

#endif /* HEADER_CURL_SOCKADDR_H */
