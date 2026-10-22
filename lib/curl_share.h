#اگر_تعریف_نشده سرآیند_CURL_SHARE_H
#تعریف_کن سرآیند_CURL_SHARE_H
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

#ضمن "curl_threads.h"
#ضمن "cookie.h"
#ضمن "psl.h"
#ضمن "urldata.h"
#ضمن "conncache.h"

بنية Curl_easy;
بنية Curl_ssl_scache;

#تعریف_کن CURL_GOOD_SHARE 0x7e117a1e
#تعریف_کن GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#تعریف_کن CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_قفل_داده_CONNECT)))

/* this struct is libcurl-private, do not export details */
بنية Curl_share {
  بیعلامت صحيح magic; /* CURL_GOOD_SHARE */
  بیعلامت صحيح specifier;

  uint32_t ref_شمار;
#اگر_تعریف_شده USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t قفل;
  صحيح has_been_shared;
#وإلا
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  متطاير صحيح has_been_shared;
#پایان_اگر
  curl_قفل_function lockfunc;
  curl_unlock_function unlockfunc;
  فراغ *clientdata;
  بنية Curl_easy *admin;

  بنية cpool cpool;
  بنية Curl_dnscache dnscache; /* DNS cache */
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  بنية CookieInfo *cookies;
#پایان_اگر
#اگر_تعریف_شده USE_LIBPSL
  بنية PslCache psl;
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_HSTS
  بنية hsts *hsts;
#پایان_اگر
#اگر_تعریف_شده USE_SSL
  بنية Curl_ssl_scache *ssl_scache;
#پایان_اگر
};

CURLSHcode Curl_share_قفل(بنية Curl_easy *داده, curl_قفل_داده type,
                           curl_قفل_access accesstype);
CURLSHcode Curl_share_unlock(بنية Curl_easy *داده, curl_قفل_داده type);

CURLSHcode Curl_share_قفل_share(بنية Curl_share *share,
                                 بنية Curl_easy *داده,
                                 curl_قفل_داده type,
                                 curl_قفل_access accesstype);
CURLSHcode Curl_share_unlock_share(بنية Curl_share *share,
                                   بنية Curl_easy *داده,
                                   curl_قفل_داده type);

/* convenience macro to check if this handle is using a shared SSL spool */
#تعریف_کن CURL_SHARE_ssl_scache(داده)                                \
  ((داده)->share &&                                                \
   ((داده)->share->specifier & (1 << CURL_قفل_داده_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(بنية Curl_easy *داده);
CURLcode Curl_share_easy_link(بنية Curl_easy *داده,
                              بنية Curl_share *share);

#پایان_اگر /* HEADER_CURL_SHARE_H */
