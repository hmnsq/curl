#यदि_अपरिभाषितम् शीर्षकम्_CURL_SOCKADDR_H
#परिभाषय शीर्षकम्_CURL_SOCKADDR_H
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

संरचना Curl_sockaddr_storage {
  सङ्घ {
    संरचना sockaddr sa;
    संरचना sockaddr_in sa_अन्तर्;
#यदि_परिभाषितम् USE_IPV6
    संरचना sockaddr_in6 sa_in6;
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_STRUCT_SOCKADDR_STORAGE
    संरचना sockaddr_storage sa_stor;
#अन्यथा
    वर्ण cbuf[256];   /* this should be big enough to fit a lot */
#यदि_अन्तः
  } बफर;
};

/*
 * The Curl_sockaddr_ex structure is libcurl's external API curl_sockaddr
 * structure with enough space available to directly hold any
 * protocol-specific address structures. The variable declared here will be
 * used to pass / receive data to/from the fopensocket callback if this has
 * been set, before that, it is initialized from parameters.
 */
संरचना Curl_sockaddr_ex {
  पूर्णाङ्क family;
  पूर्णाङ्क socktype;
  पूर्णाङ्क protocol;
  अचिह्नित पूर्णाङ्क addrlen;
  सङ्घ {
    संरचना sockaddr sa;
    संरचना Curl_sockaddr_storage buf;
  } addr;
};

#यदि_अन्तः /* HEADER_CURL_SOCKADDR_H */
