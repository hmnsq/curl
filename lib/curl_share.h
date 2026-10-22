#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SHARE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SHARE_H
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

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_threads.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cookie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "psl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "conncache.h"

ਬਣਤਰ Curl_easy;
ਬਣਤਰ Curl_ssl_scache;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_GOOD_SHARE 0x7e117a1e
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_ਤਾਲਾ_ਅੰਕੜੇ_CONNECT)))

/* this struct is libcurl-private, do not export details */
ਬਣਤਰ Curl_share {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ magic; /* CURL_GOOD_SHARE */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ specifier;

  uint32_t ref_ਗਿਣਤੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t ਤਾਲਾ;
  ਪੂਰਨ has_been_shared;
#ਨਹੀਂਤਾਂ
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  ਅਸਥਿਰ ਪੂਰਨ has_been_shared;
#ਜੇ_ਅੰਤ
  curl_ਤਾਲਾ_function lockfunc;
  curl_unlock_function unlockfunc;
  ਖਾਲੀ *clientdata;
  ਬਣਤਰ Curl_easy *admin;

  ਬਣਤਰ cpool cpool;
  ਬਣਤਰ Curl_dnscache dnscache; /* DNS cache */
#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  ਬਣਤਰ CookieInfo *cookies;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
  ਬਣਤਰ PslCache psl;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  ਬਣਤਰ hsts *hsts;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  ਬਣਤਰ Curl_ssl_scache *ssl_scache;
#ਜੇ_ਅੰਤ
};

CURLSHcode Curl_share_ਤਾਲਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_ਤਾਲਾ_ਅੰਕੜੇ type,
                           curl_ਤਾਲਾ_access accesstype);
CURLSHcode Curl_share_unlock(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_ਤਾਲਾ_ਅੰਕੜੇ type);

CURLSHcode Curl_share_ਤਾਲਾ_share(ਬਣਤਰ Curl_share *share,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 curl_ਤਾਲਾ_ਅੰਕੜੇ type,
                                 curl_ਤਾਲਾ_access accesstype);
CURLSHcode Curl_share_unlock_share(ਬਣਤਰ Curl_share *share,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   curl_ਤਾਲਾ_ਅੰਕੜੇ type);

/* convenience macro to check if this handle is using a shared SSL spool */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SHARE_ssl_scache(ਅੰਕੜੇ)                                \
  ((ਅੰਕੜੇ)->share &&                                                \
   ((ਅੰਕੜੇ)->share->specifier & (1 << CURL_ਤਾਲਾ_ਅੰਕੜੇ_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
CURLcode Curl_share_easy_link(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_share *share);

#ਜੇ_ਅੰਤ /* HEADER_CURL_SHARE_H */
