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
#ضمن "curl_setup.h"

بنية hsts;

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#ضمن "llist.h"

بنية Curl_peer;

#define MAX_HSTS_ENTRIES 10000

#إذا defined(DEBUGBUILD) || defined(UNITTESTS)
خارجي time_t deltatime;
#endif

بنية stsentry {
  بنية Curl_llist_عقدة عقدة;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  حرف host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
بنية hsts {
  بنية Curl_llist list;
  حرف *filename;
  unsigned صحيح flags;
};

بنية hsts *Curl_hsts_init(فراغ);
فراغ Curl_hsts_cleanup(بنية hsts **hp);
CURLcode Curl_hsts_copy(بنية hsts *dst, بنية hsts *src);
CURLcode Curl_hsts_parse(بنية hsts *h, ثابت حرف *hostname,
                         ثابت حرف *header);
CURLcode Curl_hsts_save(بنية Curl_easy *data, بنية hsts *h,
                        ثابت حرف *file);
CURLcode Curl_hsts_loadfile(بنية Curl_easy *data,
                            بنية hsts *h, ثابت حرف *file);
CURLcode Curl_hsts_loadcb(بنية Curl_easy *data,
                          بنية hsts *h);
CURLcode Curl_hsts_loadfiles(بنية Curl_easy *data);

bool Curl_hsts_applies(بنية hsts *h, ثابت بنية Curl_peer *dest);

#وإلا
#define Curl_hsts_cleanup(x)
#define Curl_hsts_loadcb(x, y) CURLE_OK
#define Curl_hsts_save(x, y, z)
#define Curl_hsts_loadfiles(x) CURLE_OK
#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#endif /* HEADER_CURL_HSTS_H */
