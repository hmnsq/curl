#اگر_غیر_تعریف_شدہ سرخی_CURL_SHARE_H
#تعریف_کرو سرخی_CURL_SHARE_H
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

#شامل "curl_threads.h"
#شامل "cookie.h"
#شامل "psl.h"
#شامل "urldata.h"
#شامل "conncache.h"

ساخت Curl_easy;
ساخت Curl_ssl_scache;

#تعریف_کرو CURL_GOOD_SHARE 0x7e117a1e
#تعریف_کرو GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#تعریف_کرو CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_تالا_ڈیٹا_CONNECT)))

/* this struct is libcurl-private, do not export details */
ساخت Curl_share {
  غیرعلامتی صحیح magic; /* CURL_GOOD_SHARE */
  غیرعلامتی صحیح specifier;

  uint32_t ref_گنتی;
#اگر_تعریف_شدہ USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t تالا;
  صحیح has_been_shared;
#ورنہ
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  متلون صحیح has_been_shared;
#اگر_ختم
  curl_تالا_function lockfunc;
  curl_unlock_function unlockfunc;
  خالی *clientdata;
  ساخت Curl_easy *admin;

  ساخت cpool cpool;
  ساخت Curl_dnscache dnscache; /* DNS cache */
#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  ساخت CookieInfo *cookies;
#اگر_ختم
#اگر_تعریف_شدہ USE_LIBPSL
  ساخت PslCache psl;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
  ساخت hsts *hsts;
#اگر_ختم
#اگر_تعریف_شدہ USE_SSL
  ساخت Curl_ssl_scache *ssl_scache;
#اگر_ختم
};

CURLSHcode Curl_share_تالا(ساخت Curl_easy *ڈیٹا, curl_تالا_ڈیٹا type,
                           curl_تالا_access accesstype);
CURLSHcode Curl_share_unlock(ساخت Curl_easy *ڈیٹا, curl_تالا_ڈیٹا type);

CURLSHcode Curl_share_تالا_share(ساخت Curl_share *share,
                                 ساخت Curl_easy *ڈیٹا,
                                 curl_تالا_ڈیٹا type,
                                 curl_تالا_access accesstype);
CURLSHcode Curl_share_unlock_share(ساخت Curl_share *share,
                                   ساخت Curl_easy *ڈیٹا,
                                   curl_تالا_ڈیٹا type);

/* convenience macro to check if this handle is using a shared SSL spool */
#تعریف_کرو CURL_SHARE_ssl_scache(ڈیٹا)                                \
  ((ڈیٹا)->share &&                                                \
   ((ڈیٹا)->share->specifier & (1 << CURL_تالا_ڈیٹا_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(ساخت Curl_easy *ڈیٹا);
CURLcode Curl_share_easy_link(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_share *share);

#اگر_ختم /* HEADER_CURL_SHARE_H */
