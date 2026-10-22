#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_HSTS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_HSTS_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

ਬਣਤਰ hsts;

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#ਅਨ੍ਤਰ੍ਭਾਵ "llist.h"

ਬਣਤਰ Curl_peer;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_HSTS_ENTRIES 10000

#ਜੇ defined(DEBUGBUILD) || defined(UNITTESTS)
ਬਾਹਰੀ time_t deltatime;
#ਜੇ_ਅੰਤ

ਬਣਤਰ stsentry {
  ਬਣਤਰ Curl_llist_ਗੰਢ ਗੰਢ;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  ਦੋਆਧਾਰੀ_ਅੰਕ(includeSubDomains);
  ਅੱਖਰ host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
ਬਣਤਰ hsts {
  ਬਣਤਰ Curl_llist list;
  ਅੱਖਰ *filename;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags;
};

ਬਣਤਰ hsts *Curl_hsts_init(ਖਾਲੀ);
ਖਾਲੀ Curl_hsts_cleanup(ਬਣਤਰ hsts **hp);
CURLcode Curl_hsts_copy(ਬਣਤਰ hsts *dst, ਬਣਤਰ hsts *src);
CURLcode Curl_hsts_parse(ਬਣਤਰ hsts *h, ਅਚਲ ਅੱਖਰ *hostname,
                         ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ);
CURLcode Curl_hsts_save(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ hsts *h,
                        ਅਚਲ ਅੱਖਰ *file);
CURLcode Curl_hsts_loadfile(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ hsts *h, ਅਚਲ ਅੱਖਰ *file);
CURLcode Curl_hsts_loadcb(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ hsts *h);
CURLcode Curl_hsts_loadfiles(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

bool Curl_hsts_applies(ਬਣਤਰ hsts *h, ਅਚਲ ਬਣਤਰ Curl_peer *dest);

#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_hsts_cleanup(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_hsts_loadcb(x, y) CURLE_OK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_hsts_save(x, y, z)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_hsts_loadfiles(x) CURLE_OK
#ਜੇ_ਅੰਤ /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#ਜੇ_ਅੰਤ /* HEADER_CURL_HSTS_H */
