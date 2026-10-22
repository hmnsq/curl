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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "cookie.h"
#ಅನ್ತರ್ಭಾವ "psl.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "conncache.h"

ಸಂರಚನಾ Curl_easy;
ಸಂರಚನಾ Curl_ssl_scache;

#define CURL_GOOD_SHARE 0x7e117a1e
#define GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#define CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_LOCK_DATA_CONNECT)))

/* this struct is libcurl-private, do not export details */
ಸಂರಚನಾ Curl_share {
  unsigned ಪೂರ್ಣಾಙ್ಕ magic; /* CURL_GOOD_SHARE */
  unsigned ಪೂರ್ಣಾಙ್ಕ specifier;

  uint32_t ref_ಎಣಿಕೆ;
#ifdef USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t lock;
  ಪೂರ್ಣಾಙ್ಕ has_been_shared;
#ಅನ್ಯಥಾ
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ has_been_shared;
#endif
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  ಶೂನ್ಯ *clientdata;
  ಸಂರಚನಾ Curl_easy *admin;

  ಸಂರಚನಾ cpool cpool;
  ಸಂರಚನಾ Curl_dnscache dnscache; /* DNS cache */
#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  ಸಂರಚನಾ CookieInfo *cookies;
#endif
#ifdef USE_LIBPSL
  ಸಂರಚನಾ PslCache psl;
#endif
#ifndef CURL_DISABLE_HSTS
  ಸಂರಚನಾ hsts *hsts;
#endif
#ifdef USE_SSL
  ಸಂರಚನಾ Curl_ssl_scache *ssl_scache;
#endif
};

CURLSHcode Curl_share_lock(ಸಂರಚನಾ Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype);
CURLSHcode Curl_share_unlock(ಸಂರಚನಾ Curl_easy *data, curl_lock_data type);

CURLSHcode Curl_share_lock_share(ಸಂರಚನಾ Curl_share *share,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype);
CURLSHcode Curl_share_unlock_share(ಸಂರಚನಾ Curl_share *share,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   curl_lock_data type);

/* convenience macro to check if this handle is using a shared SSL spool */
#define CURL_SHARE_ssl_scache(data)                                \
  ((data)->share &&                                                \
   ((data)->share->specifier & (1 << CURL_LOCK_DATA_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(ಸಂರಚನಾ Curl_easy *data);
CURLcode Curl_share_easy_link(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_share *share);

#endif /* HEADER_CURL_SHARE_H */
