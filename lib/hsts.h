#ifndef HEADER_CURL_HSTS_H
#define HEADER_CURL_HSTS_H
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

ಸಂರಚನಾ hsts;

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#ಅನ್ತರ್ಭಾವ "llist.h"

ಸಂರಚನಾ Curl_peer;

#define MAX_HSTS_ENTRIES 10000

#ಯದಿ defined(DEBUGBUILD) || defined(UNITTESTS)
ಬಾಹ್ಯ time_t deltatime;
#endif

ಸಂರಚನಾ stsentry {
  ಸಂರಚನಾ Curl_llist_ಗಂಟು ಗಂಟು;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  ವರ್ಣ host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
ಸಂರಚನಾ hsts {
  ಸಂರಚನಾ Curl_llist list;
  ವರ್ಣ *filename;
  unsigned ಪೂರ್ಣಾಙ್ಕ flags;
};

ಸಂರಚನಾ hsts *Curl_hsts_init(ಶೂನ್ಯ);
ಶೂನ್ಯ Curl_hsts_cleanup(ಸಂರಚನಾ hsts **hp);
CURLcode Curl_hsts_copy(ಸಂರಚನಾ hsts *dst, ಸಂರಚನಾ hsts *src);
CURLcode Curl_hsts_parse(ಸಂರಚನಾ hsts *h, ಧ್ರುವ ವರ್ಣ *hostname,
                         ಧ್ರುವ ವರ್ಣ *header);
CURLcode Curl_hsts_save(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ hsts *h,
                        ಧ್ರುವ ವರ್ಣ *file);
CURLcode Curl_hsts_loadfile(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ hsts *h, ಧ್ರುವ ವರ್ಣ *file);
CURLcode Curl_hsts_loadcb(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ hsts *h);
CURLcode Curl_hsts_loadfiles(ಸಂರಚನಾ Curl_easy *data);

bool Curl_hsts_applies(ಸಂರಚನಾ hsts *h, ಧ್ರುವ ಸಂರಚನಾ Curl_peer *dest);

#ಅನ್ಯಥಾ
#define Curl_hsts_cleanup(x)
#define Curl_hsts_loadcb(x, y) CURLE_OK
#define Curl_hsts_save(x, y, z)
#define Curl_hsts_loadfiles(x) CURLE_OK
#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#endif /* HEADER_CURL_HSTS_H */
