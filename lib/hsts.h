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
#অন্তর্ভাব "curl_setup.h"

কাঠামো hsts;

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#অন্তর্ভাব "llist.h"

কাঠামো Curl_peer;

#define MAX_HSTS_ENTRIES 10000

#যদি defined(DEBUGBUILD) || defined(UNITTESTS)
বাহ্যিক time_t deltatime;
#endif

কাঠামো stsentry {
  কাঠামো Curl_llist_গ্রন্থি গ্রন্থি;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  অক্ষর host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
কাঠামো hsts {
  কাঠামো Curl_llist list;
  অক্ষর *filename;
  unsigned পূর্ণসংখ্যা flags;
};

কাঠামো hsts *Curl_hsts_init(শূন্য);
শূন্য Curl_hsts_cleanup(কাঠামো hsts **hp);
CURLcode Curl_hsts_copy(কাঠামো hsts *dst, কাঠামো hsts *src);
CURLcode Curl_hsts_parse(কাঠামো hsts *h, ধ্রুবক অক্ষর *hostname,
                         ধ্রুবক অক্ষর *header);
CURLcode Curl_hsts_save(কাঠামো Curl_easy *তথ্য, কাঠামো hsts *h,
                        ধ্রুবক অক্ষর *file);
CURLcode Curl_hsts_loadfile(কাঠামো Curl_easy *তথ্য,
                            কাঠামো hsts *h, ধ্রুবক অক্ষর *file);
CURLcode Curl_hsts_loadcb(কাঠামো Curl_easy *তথ্য,
                          কাঠামো hsts *h);
CURLcode Curl_hsts_loadfiles(কাঠামো Curl_easy *তথ্য);

bool Curl_hsts_applies(কাঠামো hsts *h, ধ্রুবক কাঠামো Curl_peer *dest);

#অন্যথা
#define Curl_hsts_cleanup(x)
#define Curl_hsts_loadcb(x, y) CURLE_OK
#define Curl_hsts_save(x, y, z)
#define Curl_hsts_loadfiles(x) CURLE_OK
#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#endif /* HEADER_CURL_HSTS_H */
