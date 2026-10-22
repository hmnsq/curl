#अगर_अघोषित HEADER_CURL_SHARE_H
#घोषणा HEADER_CURL_SHARE_H
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
#समावेश "curl_setup.h"

#समावेश "curl_threads.h"
#समावेश "cookie.h"
#समावेश "psl.h"
#समावेश "urldata.h"
#समावेश "conncache.h"

काष्ठा Curl_easy;
काष्ठा Curl_ssl_scache;

#घोषणा CURL_GOOD_SHARE 0x7e117a1e
#घोषणा GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#घोषणा CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_LOCK_तथ्य_CONNECT)))

/* this struct is libcurl-private, do not export details */
काष्ठा Curl_share {
  अचिन्हित पूर्णांक magic; /* CURL_GOOD_SHARE */
  अचिन्हित पूर्णांक specifier;

  uint32_t ref_गिनती;
#अगर_घोषित USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t lock;
  पूर्णांक has_been_shared;
#अन्यथा
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  अस्थिर पूर्णांक has_been_shared;
#पूर्ण_अगर
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  व्योम *clientdata;
  काष्ठा Curl_easy *admin;

  काष्ठा cpool cpool;
  काष्ठा Curl_dnscache dnscache; /* DNS cache */
#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  काष्ठा CookieInfo *cookies;
#पूर्ण_अगर
#अगर_घोषित USE_LIBPSL
  काष्ठा PslCache psl;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HSTS
  काष्ठा hsts *hsts;
#पूर्ण_अगर
#अगर_घोषित USE_SSL
  काष्ठा Curl_ssl_scache *ssl_scache;
#पूर्ण_अगर
};

CURLSHcode Curl_share_lock(काष्ठा Curl_easy *तथ्य, curl_lock_तथ्य type,
                           curl_lock_access accesstype);
CURLSHcode Curl_share_unlock(काष्ठा Curl_easy *तथ्य, curl_lock_तथ्य type);

CURLSHcode Curl_share_lock_share(काष्ठा Curl_share *share,
                                 काष्ठा Curl_easy *तथ्य,
                                 curl_lock_तथ्य type,
                                 curl_lock_access accesstype);
CURLSHcode Curl_share_unlock_share(काष्ठा Curl_share *share,
                                   काष्ठा Curl_easy *तथ्य,
                                   curl_lock_तथ्य type);

/* convenience macro to check if this handle is using a shared SSL spool */
#घोषणा CURL_SHARE_ssl_scache(तथ्य)                                \
  ((तथ्य)->share &&                                                \
   ((तथ्य)->share->specifier & (1 << CURL_LOCK_तथ्य_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(काष्ठा Curl_easy *तथ्य);
CURLcode Curl_share_easy_link(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_share *share);

#पूर्ण_अगर /* HEADER_CURL_SHARE_H */
