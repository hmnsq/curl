#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SOCKADDR_H
#నిర్వచించు శీర్షిక_CURL_SOCKADDR_H
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
#అన్తర్భావ "curl_setup.h"

నిర్మాణం Curl_sockaddr_storage {
  సంఘం {
    నిర్మాణం sockaddr sa;
    నిర్మాణం sockaddr_in sa_లోపల;
#నిర్వచితమైతే USE_IPV6
    నిర్మాణం sockaddr_in6 sa_in6;
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_STRUCT_SOCKADDR_STORAGE
    నిర్మాణం sockaddr_storage sa_stor;
#లేకపోతే
    అక్షరం cbuf[256];   /* this should be big enough to fit a lot */
#అయితే_ముగింపు
  } బఫరు;
};

/*
 * The Curl_sockaddr_ex structure is libcurl's external API curl_sockaddr
 * structure with enough space available to directly hold any
 * protocol-specific address structures. The variable declared here will be
 * used to pass / receive data to/from the fopensocket callback if this has
 * been set, before that, it is initialized from parameters.
 */
నిర్మాణం Curl_sockaddr_ex {
  పూర్ణాంకం family;
  పూర్ణాంకం socktype;
  పూర్ణాంకం protocol;
  గుర్తులేని పూర్ణాంకం addrlen;
  సంఘం {
    నిర్మాణం sockaddr sa;
    నిర్మాణం Curl_sockaddr_storage buf;
  } addr;
};

#అయితే_ముగింపు /* HEADER_CURL_SOCKADDR_H */
