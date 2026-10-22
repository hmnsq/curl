#ifndef HEADER_CURL_SHARE_H
#define HEADER_CURL_SHARE_H
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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "curl_threads.h"
#অন্তর্ভাব "cookie.h"
#অন্তর্ভাব "psl.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "conncache.h"

কাঠামো Curl_easy;
কাঠামো Curl_ssl_scache;

#define CURL_GOOD_SHARE 0x7e117a1e
#define GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#define CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_LOCK_তথ্য_CONNECT)))

/* this struct is libcurl-private, do not export details */
কাঠামো Curl_share {
  unsigned পূর্ণসংখ্যা magic; /* CURL_GOOD_SHARE */
  unsigned পূর্ণসংখ্যা specifier;

  uint32_t ref_গুনতি;
#ifdef USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t lock;
  পূর্ণসংখ্যা has_been_shared;
#অন্যথা
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  অস্থির পূর্ণসংখ্যা has_been_shared;
#endif
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  শূন্য *clientdata;
  কাঠামো Curl_easy *admin;

  কাঠামো cpool cpool;
  কাঠামো Curl_dnscache dnscache; /* DNS cache */
#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  কাঠামো CookieInfo *cookies;
#endif
#ifdef USE_LIBPSL
  কাঠামো PslCache psl;
#endif
#ifndef CURL_DISABLE_HSTS
  কাঠামো hsts *hsts;
#endif
#ifdef USE_SSL
  কাঠামো Curl_ssl_scache *ssl_scache;
#endif
};

CURLSHcode Curl_share_lock(কাঠামো Curl_easy *তথ্য, curl_lock_তথ্য type,
                           curl_lock_access accesstype);
CURLSHcode Curl_share_unlock(কাঠামো Curl_easy *তথ্য, curl_lock_তথ্য type);

CURLSHcode Curl_share_lock_share(কাঠামো Curl_share *share,
                                 কাঠামো Curl_easy *তথ্য,
                                 curl_lock_তথ্য type,
                                 curl_lock_access accesstype);
CURLSHcode Curl_share_unlock_share(কাঠামো Curl_share *share,
                                   কাঠামো Curl_easy *তথ্য,
                                   curl_lock_তথ্য type);

/* convenience macro to check if this handle is using a shared SSL spool */
#define CURL_SHARE_ssl_scache(তথ্য)                                \
  ((তথ্য)->share &&                                                \
   ((তথ্য)->share->specifier & (1 << CURL_LOCK_তথ্য_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(কাঠামো Curl_easy *তথ্য);
CURLcode Curl_share_easy_link(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_share *share);

#endif /* HEADER_CURL_SHARE_H */
