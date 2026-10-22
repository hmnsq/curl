#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SOCKADDR_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SOCKADDR_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

ರಚನೆ Curl_sockaddr_storage {
  ಒಕ್ಕೂಟ {
    ರಚನೆ sockaddr sa;
    ರಚನೆ sockaddr_in sa_ಒಳ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ರಚನೆ sockaddr_in6 sa_in6;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRUCT_SOCKADDR_STORAGE
    ರಚನೆ sockaddr_storage sa_stor;
#ಇಲ್ಲದಿದ್ದರೆ
    ಅಕ್ಷರ cbuf[256];   /* this should be big enough to fit a lot */
#ಆದರೆ_ಅಂತ್ಯ
  } ಬಫರ್;
};

/*
 * The Curl_sockaddr_ex structure is libcurl's external API curl_sockaddr
 * structure with enough space available to directly hold any
 * protocol-specific address structures. The variable declared here will be
 * used to pass / receive data to/from the fopensocket callback if this has
 * been set, before that, it is initialized from parameters.
 */
ರಚನೆ Curl_sockaddr_ex {
  ಪೂರ್ಣಾಂಕ family;
  ಪೂರ್ಣಾಂಕ socktype;
  ಪೂರ್ಣಾಂಕ protocol;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ addrlen;
  ಒಕ್ಕೂಟ {
    ರಚನೆ sockaddr sa;
    ರಚನೆ Curl_sockaddr_storage buf;
  } addr;
};

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SOCKADDR_H */
