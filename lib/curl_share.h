#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SHARE_H
#నిర్వచించు శీర్షిక_CURL_SHARE_H
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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "curl_threads.h"
#అన్తర్భావ "cookie.h"
#అన్తర్భావ "psl.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "conncache.h"

నిర్మాణం Curl_easy;
నిర్మాణం Curl_ssl_scache;

#నిర్వచించు CURL_GOOD_SHARE 0x7e117a1e
#నిర్వచించు GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#నిర్వచించు CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_తాళం_దత్తాంశం_CONNECT)))

/* this struct is libcurl-private, do not export details */
నిర్మాణం Curl_share {
  గుర్తులేని పూర్ణాంకం magic; /* CURL_GOOD_SHARE */
  గుర్తులేని పూర్ణాంకం specifier;

  uint32_t ref_లెక్క;
#నిర్వచితమైతే USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t తాళం;
  పూర్ణాంకం has_been_shared;
#లేకపోతే
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  అస్థిర పూర్ణాంకం has_been_shared;
#అయితే_ముగింపు
  curl_తాళం_function lockfunc;
  curl_unlock_function unlockfunc;
  శూన్య *clientdata;
  నిర్మాణం Curl_easy *admin;

  నిర్మాణం cpool cpool;
  నిర్మాణం Curl_dnscache dnscache; /* DNS cache */
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  నిర్మాణం CookieInfo *cookies;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_LIBPSL
  నిర్మాణం PslCache psl;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  నిర్మాణం hsts *hsts;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSL
  నిర్మాణం Curl_ssl_scache *ssl_scache;
#అయితే_ముగింపు
};

CURLSHcode Curl_share_తాళం(నిర్మాణం Curl_easy *దత్తాంశం, curl_తాళం_దత్తాంశం type,
                           curl_తాళం_access accesstype);
CURLSHcode Curl_share_unlock(నిర్మాణం Curl_easy *దత్తాంశం, curl_తాళం_దత్తాంశం type);

CURLSHcode Curl_share_తాళం_share(నిర్మాణం Curl_share *share,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 curl_తాళం_దత్తాంశం type,
                                 curl_తాళం_access accesstype);
CURLSHcode Curl_share_unlock_share(నిర్మాణం Curl_share *share,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   curl_తాళం_దత్తాంశం type);

/* convenience macro to check if this handle is using a shared SSL spool */
#నిర్వచించు CURL_SHARE_ssl_scache(దత్తాంశం)                                \
  ((దత్తాంశం)->share &&                                                \
   ((దత్తాంశం)->share->specifier & (1 << CURL_తాళం_దత్తాంశం_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(నిర్మాణం Curl_easy *దత్తాంశం);
CURLcode Curl_share_easy_link(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_share *share);

#అయితే_ముగింపు /* HEADER_CURL_SHARE_H */
