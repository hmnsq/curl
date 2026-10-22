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
#شامل "curl_setup.h"

ساختار hsts;

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#شامل "llist.h"

ساختار Curl_peer;

#define MAX_HSTS_ENTRIES 10000

#اگر defined(DEBUGBUILD) || defined(UNITTESTS)
بیرونی time_t deltatime;
#endif

ساختار stsentry {
  ساختار Curl_llist_گره گره;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  نویسه host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
ساختار hsts {
  ساختار Curl_llist list;
  نویسه *filename;
  unsigned صحیح flags;
};

ساختار hsts *Curl_hsts_init(تهی);
تهی Curl_hsts_cleanup(ساختار hsts **hp);
CURLcode Curl_hsts_copy(ساختار hsts *dst, ساختار hsts *src);
CURLcode Curl_hsts_parse(ساختار hsts *h, ثابت نویسه *hostname,
                         ثابت نویسه *header);
CURLcode Curl_hsts_save(ساختار Curl_easy *data, ساختار hsts *h,
                        ثابت نویسه *file);
CURLcode Curl_hsts_loadfile(ساختار Curl_easy *data,
                            ساختار hsts *h, ثابت نویسه *file);
CURLcode Curl_hsts_loadcb(ساختار Curl_easy *data,
                          ساختار hsts *h);
CURLcode Curl_hsts_loadfiles(ساختار Curl_easy *data);

bool Curl_hsts_applies(ساختار hsts *h, ثابت ساختار Curl_peer *dest);

#وگرنه
#define Curl_hsts_cleanup(x)
#define Curl_hsts_loadcb(x, y) CURLE_OK
#define Curl_hsts_save(x, y, z)
#define Curl_hsts_loadfiles(x) CURLE_OK
#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#endif /* HEADER_CURL_HSTS_H */
