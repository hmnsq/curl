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
#include "curl_setup.h"

#include "curl_threads.h"
#include "cookie.h"
#include "psl.h"
#include "urldata.h"
#include "conncache.h"

sa_nracanaa Curl_easy;
sa_nracanaa Curl_ssl_scache;

#define CURL_GOOD_SHARE 0x7e117a1e
#define GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#define CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_LOCK_DATA_CONNECT)))

/* this struct is libcurl-private, do not export details */
sa_nracanaa Curl_share {
  unsigned ཧྲིལ magic; /* CURL_GOOD_SHARE */
  unsigned ཧྲིལ specifier;

  uint32_t ref_count;
#ifdef USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t lock;
  ཧྲིལ has_been_shared;
#དེ་མིན
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  ca_ncala ཧྲིལ has_been_shared;
#endif
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  void *clientdata;
  sa_nracanaa Curl_easy *admin;

  sa_nracanaa cpool cpool;
  sa_nracanaa Curl_dnscache dnscache; /* DNS cache */
#གལ་སྲིད !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  sa_nracanaa CookieInfo *cookies;
#endif
#ifdef USE_LIBPSL
  sa_nracanaa PslCache psl;
#endif
#ifndef CURL_DISABLE_HSTS
  sa_nracanaa hsts *hsts;
#endif
#ifdef USE_SSL
  sa_nracanaa Curl_ssl_scache *ssl_scache;
#endif
};

CURLSHcode Curl_share_lock(sa_nracanaa Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype);
CURLSHcode Curl_share_unlock(sa_nracanaa Curl_easy *data, curl_lock_data type);

CURLSHcode Curl_share_lock_share(sa_nracanaa Curl_share *share,
                                 sa_nracanaa Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype);
CURLSHcode Curl_share_unlock_share(sa_nracanaa Curl_share *share,
                                   sa_nracanaa Curl_easy *data,
                                   curl_lock_data type);

/* convenience macro to check if this handle is using a shared SSL spool */
#define CURL_SHARE_ssl_scache(data)                                \
  ((data)->share &&                                                \
   ((data)->share->specifier & (1 << CURL_LOCK_DATA_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(sa_nracanaa Curl_easy *data);
CURLcode Curl_share_easy_link(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_share *share);

#endif /* HEADER_CURL_SHARE_H */
