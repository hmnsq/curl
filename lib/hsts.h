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
#অন্তর্ভাৱ "curl_setup.h"

সংরচনা hsts;

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#অন্তর্ভাৱ "llist.h"

সংরচনা Curl_peer;

#define MAX_HSTS_ENTRIES 10000

#যদি defined(DEBUGBUILD) || defined(UNITTESTS)
বাহ্য time_t deltatime;
#endif

সংরচনা stsentry {
  সংরচনা Curl_llist_গ্ৰন্থি গ্ৰন্থি;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  ৱর্ণ host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
সংরচনা hsts {
  সংরচনা Curl_llist list;
  ৱর্ণ *filename;
  unsigned পূর্ণাঙ্ক flags;
};

সংরচনা hsts *Curl_hsts_init(শূন্য);
শূন্য Curl_hsts_cleanup(সংরচনা hsts **hp);
CURLcode Curl_hsts_copy(সংরচনা hsts *dst, সংরচনা hsts *src);
CURLcode Curl_hsts_parse(সংরচনা hsts *h, ধ্রুৱ ৱর্ণ *hostname,
                         ধ্রুৱ ৱর্ণ *header);
CURLcode Curl_hsts_save(সংরচনা Curl_easy *data, সংরচনা hsts *h,
                        ধ্রুৱ ৱর্ণ *file);
CURLcode Curl_hsts_loadfile(সংরচনা Curl_easy *data,
                            সংরচনা hsts *h, ধ্রুৱ ৱর্ণ *file);
CURLcode Curl_hsts_loadcb(সংরচনা Curl_easy *data,
                          সংরচনা hsts *h);
CURLcode Curl_hsts_loadfiles(সংরচনা Curl_easy *data);

bool Curl_hsts_applies(সংরচনা hsts *h, ধ্রুৱ সংরচনা Curl_peer *dest);

#অন্যথা
#define Curl_hsts_cleanup(x)
#define Curl_hsts_loadcb(x, y) CURLE_OK
#define Curl_hsts_save(x, y, z)
#define Curl_hsts_loadfiles(x) CURLE_OK
#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#endif /* HEADER_CURL_HSTS_H */
