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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

ସଂରଚନା hsts;

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#ଅନ୍ତର୍ଭାଵ "llist.h"

ସଂରଚନା Curl_peer;

#define MAX_HSTS_ENTRIES 10000

#ଯଦି defined(DEBUGBUILD) || defined(UNITTESTS)
ବାହ୍ୟ time_t deltatime;
#endif

ସଂରଚନା stsentry {
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି ଗଣ୍ଠି;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  ଅକ୍ଷର host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
ସଂରଚନା hsts {
  ସଂରଚନା Curl_llist list;
  ଅକ୍ଷର *filename;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;
};

ସଂରଚନା hsts *Curl_hsts_init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ Curl_hsts_cleanup(ସଂରଚନା hsts **hp);
CURLcode Curl_hsts_copy(ସଂରଚନା hsts *dst, ସଂରଚନା hsts *src);
CURLcode Curl_hsts_parse(ସଂରଚନା hsts *h, ଅଚଳ ଅକ୍ଷର *hostname,
                         ଅଚଳ ଅକ୍ଷର *header);
CURLcode Curl_hsts_save(ସଂରଚନା Curl_easy *data, ସଂରଚନା hsts *h,
                        ଅଚଳ ଅକ୍ଷର *file);
CURLcode Curl_hsts_loadfile(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା hsts *h, ଅଚଳ ଅକ୍ଷର *file);
CURLcode Curl_hsts_loadcb(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା hsts *h);
CURLcode Curl_hsts_loadfiles(ସଂରଚନା Curl_easy *data);

bool Curl_hsts_applies(ସଂରଚନା hsts *h, ଅଚଳ ସଂରଚନା Curl_peer *dest);

#ଅନ୍ୟଥା
#define Curl_hsts_cleanup(x)
#define Curl_hsts_loadcb(x, y) CURLE_OK
#define Curl_hsts_save(x, y, z)
#define Curl_hsts_loadfiles(x) CURLE_OK
#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#endif /* HEADER_CURL_HSTS_H */
