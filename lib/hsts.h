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
#include "curl_setup.h"

sa_nracanaa hsts;

#ถ้า !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#include "llist.h"

sa_nracanaa Curl_peer;

#define MAX_HSTS_ENTRIES 10000

#ถ้า defined(DEBUGBUILD) || defined(UNITTESTS)
extern time_t deltatime;
#endif

sa_nracanaa stsentry {
  sa_nracanaa Curl_llist_node node;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  var_na host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
sa_nracanaa hsts {
  sa_nracanaa Curl_llist list;
  var_na *filename;
  unsigned จำนวนเต็ม flags;
};

sa_nracanaa hsts *Curl_hsts_init(void);
void Curl_hsts_cleanup(sa_nracanaa hsts **hp);
CURLcode Curl_hsts_copy(sa_nracanaa hsts *dst, sa_nracanaa hsts *src);
CURLcode Curl_hsts_parse(sa_nracanaa hsts *h, const var_na *hostname,
                         const var_na *header);
CURLcode Curl_hsts_save(sa_nracanaa Curl_easy *data, sa_nracanaa hsts *h,
                        const var_na *file);
CURLcode Curl_hsts_loadfile(sa_nracanaa Curl_easy *data,
                            sa_nracanaa hsts *h, const var_na *file);
CURLcode Curl_hsts_loadcb(sa_nracanaa Curl_easy *data,
                          sa_nracanaa hsts *h);
CURLcode Curl_hsts_loadfiles(sa_nracanaa Curl_easy *data);

bool Curl_hsts_applies(sa_nracanaa hsts *h, const sa_nracanaa Curl_peer *dest);

#มิฉะนั้น
#define Curl_hsts_cleanup(x)
#define Curl_hsts_loadcb(x, y) CURLE_OK
#define Curl_hsts_save(x, y, z)
#define Curl_hsts_loadfiles(x) CURLE_OK
#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#endif /* HEADER_CURL_HSTS_H */
