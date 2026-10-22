#यदि_अपरिभाषितम् शीर्षकम्_CURL_SHARE_H
#परिभाषय शीर्षकम्_CURL_SHARE_H
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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "curl_threads.h"
#अन्तर्भाव "cookie.h"
#अन्तर्भाव "psl.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "conncache.h"

संरचना Curl_easy;
संरचना Curl_ssl_scache;

#परिभाषय CURL_GOOD_SHARE 0x7e117a1e
#परिभाषय GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#परिभाषय CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_तालकम्_दत्त_CONNECT)))

/* this struct is libcurl-private, do not export details */
संरचना Curl_share {
  अचिह्नित पूर्णाङ्क magic; /* CURL_GOOD_SHARE */
  अचिह्नित पूर्णाङ्क specifier;

  uint32_t ref_गणनम्;
#यदि_परिभाषितम् USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t तालकम्;
  पूर्णाङ्क has_been_shared;
#अन्यथा
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  चञ्चल पूर्णाङ्क has_been_shared;
#यदि_अन्तः
  curl_तालकम्_function lockfunc;
  curl_unlock_function unlockfunc;
  शून्य *clientdata;
  संरचना Curl_easy *admin;

  संरचना cpool cpool;
  संरचना Curl_dnscache dnscache; /* DNS cache */
#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  संरचना CookieInfo *cookies;
#यदि_अन्तः
#यदि_परिभाषितम् USE_LIBPSL
  संरचना PslCache psl;
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
  संरचना hsts *hsts;
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSL
  संरचना Curl_ssl_scache *ssl_scache;
#यदि_अन्तः
};

CURLSHcode Curl_share_तालकम्(संरचना Curl_easy *दत्त, curl_तालकम्_दत्त type,
                           curl_तालकम्_access accesstype);
CURLSHcode Curl_share_unlock(संरचना Curl_easy *दत्त, curl_तालकम्_दत्त type);

CURLSHcode Curl_share_तालकम्_share(संरचना Curl_share *share,
                                 संरचना Curl_easy *दत्त,
                                 curl_तालकम्_दत्त type,
                                 curl_तालकम्_access accesstype);
CURLSHcode Curl_share_unlock_share(संरचना Curl_share *share,
                                   संरचना Curl_easy *दत्त,
                                   curl_तालकम्_दत्त type);

/* convenience macro to check if this handle is using a shared SSL spool */
#परिभाषय CURL_SHARE_ssl_scache(दत्त)                                \
  ((दत्त)->share &&                                                \
   ((दत्त)->share->specifier & (1 << CURL_तालकम्_दत्त_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(संरचना Curl_easy *दत्त);
CURLcode Curl_share_easy_link(संरचना Curl_easy *दत्त,
                              संरचना Curl_share *share);

#यदि_अन्तः /* HEADER_CURL_SHARE_H */
