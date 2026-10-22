#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SHARE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SHARE_H
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

ರಚನೆ Curl_easy;
ರಚನೆ Curl_ssl_scache;

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_GOOD_SHARE 0x7e117a1e
#ವ್ಯಾಖ್ಯಾನಿಸು GOOD_SHARE_HANDLE(x) ((x) && (x)->magic == CURL_GOOD_SHARE)

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SHARE_KEEP_CONNECT(s)                          \
  ((s) && ((s)->specifier & (1 << CURL_ಬೀಗ_ದತ್ತಾಂಶ_CONNECT)))

/* this struct is libcurl-private, do not export details */
ರಚನೆ Curl_share {
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ magic; /* CURL_GOOD_SHARE */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ specifier;

  uint32_t ref_ಎಣಿಕೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
   /* do `ref_count` and `has_been_shared` checks using this mutex. */
  curl_mutex_t ಬೀಗ;
  ಪೂರ್ಣಾಂಕ has_been_shared;
#ಇಲ್ಲದಿದ್ದರೆ
  /* this only ever goes from FALSE -> TRUE once. We need to check
   * this without being able to use the `lockfunc`. */
  ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ has_been_shared;
#ಆದರೆ_ಅಂತ್ಯ
  curl_ಬೀಗ_function lockfunc;
  curl_unlock_function unlockfunc;
  ಶೂನ್ಯ *clientdata;
  ರಚನೆ Curl_easy *admin;

  ರಚನೆ cpool cpool;
  ರಚನೆ Curl_dnscache dnscache; /* DNS cache */
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  ರಚನೆ CookieInfo *cookies;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
  ರಚನೆ PslCache psl;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  ರಚನೆ hsts *hsts;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  ರಚನೆ Curl_ssl_scache *ssl_scache;
#ಆದರೆ_ಅಂತ್ಯ
};

CURLSHcode Curl_share_ಬೀಗ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_ಬೀಗ_ದತ್ತಾಂಶ type,
                           curl_ಬೀಗ_access accesstype);
CURLSHcode Curl_share_unlock(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_ಬೀಗ_ದತ್ತಾಂಶ type);

CURLSHcode Curl_share_ಬೀಗ_share(ರಚನೆ Curl_share *share,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 curl_ಬೀಗ_ದತ್ತಾಂಶ type,
                                 curl_ಬೀಗ_access accesstype);
CURLSHcode Curl_share_unlock_share(ರಚನೆ Curl_share *share,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   curl_ಬೀಗ_ದತ್ತಾಂಶ type);

/* convenience macro to check if this handle is using a shared SSL spool */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SHARE_ssl_scache(ದತ್ತಾಂಶ)                                \
  ((ದತ್ತಾಂಶ)->share &&                                                \
   ((ದತ್ತಾಂಶ)->share->specifier & (1 << CURL_ಬೀಗ_ದತ್ತಾಂಶ_SSL_SESSION)))

CURLcode Curl_share_easy_unlink(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
CURLcode Curl_share_easy_link(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_share *share);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SHARE_H */
