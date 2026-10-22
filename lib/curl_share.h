#که_نه_تعریف_شوی سرلیک_CURL_SHARE_H
#تعریف_کړه سرلیک_CURL_SHARE_H
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

#تعریف_کړه CURL_GOOD_SHARE 0x7e117a1e
#تعریف_کړه GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#تعریف_کړه CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_قلف_معلومات_CONNECT)))

/* this struct is libcurl-private, do not export details */
بنية Curl_share {
  بینښه صحيح magic; /* CURL_GOOD_SHARE */
  بینښه صحيح specifier;

  uint32_t ref_شمېر;
#که_تعریف_شوی USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t قلف;
  صحيح has_been_shared;
#وإلا
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  متطاير صحيح has_been_shared;
#د_که_پای
  curl_قلف_function lockfunc;
  curl_unlock_function unlockfunc;
  فراغ *clientdata;
  بنية Curl_easy *admin;

  بنية cpool cpool;
  بنية Curl_dnscache dnscache; /* DNS cache */
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  بنية CookieInfo *cookies;
#د_که_پای
#که_تعریف_شوی USE_LIBPSL
  بنية PslCache psl;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
  بنية hsts *hsts;
#د_که_پای
#که_تعریف_شوی USE_SSL
  بنية Curl_ssl_scache *ssl_scache;
#د_که_پای
};

CURLSHcode Curl_share_قلف(بنية Curl_easy *معلومات, curl_قلف_معلومات type,
                           curl_قلف_access accesstype);
CURLSHcode Curl_share_unlock(بنية Curl_easy *معلومات, curl_قلف_معلومات type);

CURLSHcode Curl_share_قلف_share(بنية Curl_share *share,
                                 بنية Curl_easy *معلومات,
                                 curl_قلف_معلومات type,
                                 curl_قلف_access accesstype);
CURLSHcode Curl_share_unlock_share(بنية Curl_share *share,
                                   بنية Curl_easy *معلومات,
                                   curl_قلف_معلومات type);

/* convenience macro to check if this handle is using a shared SSL spool */
#تعریف_کړه CURL_SHARE_ssl_scache(معلومات)                                \
  ((معلومات)->share &&                                                \
   ((معلومات)->share->specifier & (1 << CURL_قلف_معلومات_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(بنية Curl_easy *معلومات);
CURLcode Curl_share_easy_link(بنية Curl_easy *معلومات,
                              بنية Curl_share *share);

#د_که_پای /* HEADER_CURL_SHARE_H */
