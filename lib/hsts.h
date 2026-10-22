#اگر_غیر_تعریف_شدہ سرخی_CURL_HSTS_H
#تعریف_کرو سرخی_CURL_HSTS_H
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

ساخت hsts;

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#شامل "llist.h"

ساخت Curl_peer;

#تعریف_کرو MAX_HSTS_ENTRIES 10000

#اگر defined(DEBUGBUILD) || defined(UNITTESTS)
خارجی time_t deltatime;
#اگر_ختم

ساخت stsentry {
  ساخت Curl_llist_گرہ گرہ;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  ثنائی_ہندسہ(includeSubDomains);
  حرف host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
ساخت hsts {
  ساخت Curl_llist list;
  حرف *filename;
  غیرعلامتی صحیح flags;
};

ساخت hsts *Curl_hsts_init(خالی);
خالی Curl_hsts_cleanup(ساخت hsts **hp);
CURLcode Curl_hsts_copy(ساخت hsts *dst, ساخت hsts *src);
CURLcode Curl_hsts_parse(ساخت hsts *h, ثابت حرف *hostname,
                         ثابت حرف *سرخی);
CURLcode Curl_hsts_save(ساخت Curl_easy *ڈیٹا, ساخت hsts *h,
                        ثابت حرف *file);
CURLcode Curl_hsts_loadfile(ساخت Curl_easy *ڈیٹا,
                            ساخت hsts *h, ثابت حرف *file);
CURLcode Curl_hsts_loadcb(ساخت Curl_easy *ڈیٹا,
                          ساخت hsts *h);
CURLcode Curl_hsts_loadfiles(ساخت Curl_easy *ڈیٹا);

bool Curl_hsts_applies(ساخت hsts *h, ثابت ساخت Curl_peer *dest);

#ورنہ
#تعریف_کرو Curl_hsts_cleanup(x)
#تعریف_کرو Curl_hsts_loadcb(x, y) CURLE_OK
#تعریف_کرو Curl_hsts_save(x, y, z)
#تعریف_کرو Curl_hsts_loadfiles(x) CURLE_OK
#اگر_ختم /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#اگر_ختم /* HEADER_CURL_HSTS_H */
