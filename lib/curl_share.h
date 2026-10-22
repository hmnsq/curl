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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "curl_threads.h"
#அந்தர்பாவ "cookie.h"
#அந்தர்பாவ "psl.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "conncache.h"

கட்டமைப்பு Curl_easy;
கட்டமைப்பு Curl_ssl_scache;

#define CURL_GOOD_SHARE 0x7e117a1e
#define GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#define CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_LOCK_DATA_CONNECT)))

/* this struct is libcurl-private, do not export details */
கட்டமைப்பு Curl_share {
  unsigned முழு magic; /* CURL_GOOD_SHARE */
  unsigned முழு specifier;

  uint32_t ref_மொத்தம்;
#ifdef USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t lock;
  முழு has_been_shared;
#இல்லையெனில்
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  நிலையற்ற முழு has_been_shared;
#endif
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  வெறுமை *clientdata;
  கட்டமைப்பு Curl_easy *admin;

  கட்டமைப்பு cpool cpool;
  கட்டமைப்பு Curl_dnscache dnscache; /* DNS cache */
#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  கட்டமைப்பு CookieInfo *cookies;
#endif
#ifdef USE_LIBPSL
  கட்டமைப்பு PslCache psl;
#endif
#ifndef CURL_DISABLE_HSTS
  கட்டமைப்பு hsts *hsts;
#endif
#ifdef USE_SSL
  கட்டமைப்பு Curl_ssl_scache *ssl_scache;
#endif
};

CURLSHcode Curl_share_lock(கட்டமைப்பு Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype);
CURLSHcode Curl_share_unlock(கட்டமைப்பு Curl_easy *data, curl_lock_data type);

CURLSHcode Curl_share_lock_share(கட்டமைப்பு Curl_share *share,
                                 கட்டமைப்பு Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype);
CURLSHcode Curl_share_unlock_share(கட்டமைப்பு Curl_share *share,
                                   கட்டமைப்பு Curl_easy *data,
                                   curl_lock_data type);

/* convenience macro to check if this handle is using a shared SSL spool */
#define CURL_SHARE_ssl_scache(data)                                \
  ((data)->share &&                                                \
   ((data)->share->specifier & (1 << CURL_LOCK_DATA_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(கட்டமைப்பு Curl_easy *data);
CURLcode Curl_share_easy_link(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_share *share);

#endif /* HEADER_CURL_SHARE_H */
