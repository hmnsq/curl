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
#അന്തര്ഭാവ "curl_setup.h"

ഘടന hsts;

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#അന്തര്ഭാവ "llist.h"

ഘടന Curl_peer;

#define MAX_HSTS_ENTRIES 10000

#എങ്കിൽ defined(DEBUGBUILD) || defined(UNITTESTS)
ബാഹ്യ time_t deltatime;
#endif

ഘടന stsentry {
  ഘടന Curl_llist_കണ്ണി കണ്ണി;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  അക്ഷരം host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
ഘടന hsts {
  ഘടന Curl_llist list;
  അക്ഷരം *filename;
  unsigned പൂർണ്ണസംഖ്യ flags;
};

ഘടന hsts *Curl_hsts_init(ശൂന്യം);
ശൂന്യം Curl_hsts_cleanup(ഘടന hsts **hp);
CURLcode Curl_hsts_copy(ഘടന hsts *dst, ഘടന hsts *src);
CURLcode Curl_hsts_parse(ഘടന hsts *h, സ്ഥിരം അക്ഷരം *hostname,
                         സ്ഥിരം അക്ഷരം *header);
CURLcode Curl_hsts_save(ഘടന Curl_easy *data, ഘടന hsts *h,
                        സ്ഥിരം അക്ഷരം *file);
CURLcode Curl_hsts_loadfile(ഘടന Curl_easy *data,
                            ഘടന hsts *h, സ്ഥിരം അക്ഷരം *file);
CURLcode Curl_hsts_loadcb(ഘടന Curl_easy *data,
                          ഘടന hsts *h);
CURLcode Curl_hsts_loadfiles(ഘടന Curl_easy *data);

bool Curl_hsts_applies(ഘടന hsts *h, സ്ഥിരം ഘടന Curl_peer *dest);

#അല്ലെങ്കിൽ
#define Curl_hsts_cleanup(x)
#define Curl_hsts_loadcb(x, y) CURLE_OK
#define Curl_hsts_save(x, y, z)
#define Curl_hsts_loadfiles(x) CURLE_OK
#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#endif /* HEADER_CURL_HSTS_H */
