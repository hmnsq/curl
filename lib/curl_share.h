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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "curl_threads.h"
#ଅନ୍ତର୍ଭାଵ "cookie.h"
#ଅନ୍ତର୍ଭାଵ "psl.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "conncache.h"

ସଂରଚନା Curl_easy;
ସଂରଚନା Curl_ssl_scache;

#define CURL_GOOD_SHARE 0x7e117a1e
#define GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#define CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_LOCK_DATA_CONNECT)))

/* this struct is libcurl-private, do not export details */
ସଂରଚନା Curl_share {
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା magic; /* CURL_GOOD_SHARE */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା specifier;

  uint32_t ref_ଗଣନା;
#ifdef USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t lock;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_been_shared;
#ଅନ୍ୟଥା
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_been_shared;
#endif
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  ଶୂନ୍ୟ *clientdata;
  ସଂରଚନା Curl_easy *admin;

  ସଂରଚନା cpool cpool;
  ସଂରଚନା Curl_dnscache dnscache; /* DNS cache */
#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  ସଂରଚନା CookieInfo *cookies;
#endif
#ifdef USE_LIBPSL
  ସଂରଚନା PslCache psl;
#endif
#ifndef CURL_DISABLE_HSTS
  ସଂରଚନା hsts *hsts;
#endif
#ifdef USE_SSL
  ସଂରଚନା Curl_ssl_scache *ssl_scache;
#endif
};

CURLSHcode Curl_share_lock(ସଂରଚନା Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype);
CURLSHcode Curl_share_unlock(ସଂରଚନା Curl_easy *data, curl_lock_data type);

CURLSHcode Curl_share_lock_share(ସଂରଚନା Curl_share *share,
                                 ସଂରଚନା Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype);
CURLSHcode Curl_share_unlock_share(ସଂରଚନା Curl_share *share,
                                   ସଂରଚନା Curl_easy *data,
                                   curl_lock_data type);

/* convenience macro to check if this handle is using a shared SSL spool */
#define CURL_SHARE_ssl_scache(data)                                \
  ((data)->share &&                                                \
   ((data)->share->specifier & (1 << CURL_LOCK_DATA_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(ସଂରଚନା Curl_easy *data);
CURLcode Curl_share_easy_link(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_share *share);

#endif /* HEADER_CURL_SHARE_H */
