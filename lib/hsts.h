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
#અન્તર્ભાવ "curl_setup.h"

સંરચના hsts;

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#અન્તર્ભાવ "llist.h"

સંરચના Curl_peer;

#define MAX_HSTS_ENTRIES 10000

#જો defined(DEBUGBUILD) || defined(UNITTESTS)
બાહ્ય time_t deltatime;
#endif

સંરચના stsentry {
  સંરચના Curl_llist_ગાંઠ ગાંઠ;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  અક્ષર host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
સંરચના hsts {
  સંરચના Curl_llist list;
  અક્ષર *filename;
  unsigned પૂર્ણાંક flags;
};

સંરચના hsts *Curl_hsts_init(શૂન્ય);
શૂન્ય Curl_hsts_cleanup(સંરચના hsts **hp);
CURLcode Curl_hsts_copy(સંરચના hsts *dst, સંરચના hsts *src);
CURLcode Curl_hsts_parse(સંરચના hsts *h, અચલ અક્ષર *hostname,
                         અચલ અક્ષર *header);
CURLcode Curl_hsts_save(સંરચના Curl_easy *data, સંરચના hsts *h,
                        અચલ અક્ષર *file);
CURLcode Curl_hsts_loadfile(સંરચના Curl_easy *data,
                            સંરચના hsts *h, અચલ અક્ષર *file);
CURLcode Curl_hsts_loadcb(સંરચના Curl_easy *data,
                          સંરચના hsts *h);
CURLcode Curl_hsts_loadfiles(સંરચના Curl_easy *data);

bool Curl_hsts_applies(સંરચના hsts *h, અચલ સંરચના Curl_peer *dest);

#નહીંતર
#define Curl_hsts_cleanup(x)
#define Curl_hsts_loadcb(x, y) CURLE_OK
#define Curl_hsts_save(x, y, z)
#define Curl_hsts_loadfiles(x) CURLE_OK
#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#endif /* HEADER_CURL_HSTS_H */
