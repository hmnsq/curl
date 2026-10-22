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
#અન્તર્ભાવ "curl_setup.h"

સંરચના Curl_sockaddr_storage {
  સંઘ {
    સંરચના sockaddr sa;
    સંરચના sockaddr_in sa_in;
#ifdef USE_IPV6
    સંરચના sockaddr_in6 sa_in6;
#endif
#ifdef HAVE_STRUCT_SOCKADDR_STORAGE
    સંરચના sockaddr_storage sa_stor;
#નહીંતર
    અક્ષર cbuf[256];   /* this should be big enough to fit a lot */
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
સંરચના Curl_sockaddr_ex {
  પૂર્ણાંક family;
  પૂર્ણાંક socktype;
  પૂર્ણાંક protocol;
  unsigned પૂર્ણાંક addrlen;
  સંઘ {
    સંરચના sockaddr sa;
    સંરચના Curl_sockaddr_storage buf;
  } addr;
};

#endif /* HEADER_CURL_SOCKADDR_H */
