#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_URLAPI_INT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_URLAPI_INT_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ <curl/urlapi.h>

/* Internal representation of CURLU. Point to URL-encoded strings. */
ਬਣਤਰ Curl_URL {
  ਅੱਖਰ *scheme;
  ਅੱਖਰ *user;
  ਅੱਖਰ *password;
  ਅੱਖਰ *options; /* IMAP only? */
  ਅੱਖਰ *host;
  ਅੱਖਰ *zoneid; /* for numerical IPv6 addresses */
  ਅੱਖਰ *ਰਸਤਾ;
  ਅੱਖਰ *query;
  ਅੱਖਰ *fragment;
  uint16_t portnum; /* the numerical port if present */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੋਰਟ_present);    /* to support missing port */
  ਦੋਆਧਾਰੀ_ਅੰਕ(query_present);    /* to support blank */
  ਦੋਆਧਾਰੀ_ਅੰਕ(fragment_present); /* to support blank */
  ਦੋਆਧਾਰੀ_ਅੰਕ(guessed_scheme);   /* when a URL without scheme is parsed */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HOST_ERROR   (-1) /* out of memory */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HOST_NAME    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HOST_IPV4    2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HOST_IPV6    3

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QUERY_NO      2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QUERY_NOT_YET 3 /* allow to change to query */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QUERY_YES     4

size_t Curl_is_absolute_url(ਅਚਲ ਅੱਖਰ *url, ਅੱਖਰ *buf, size_t buflen,
                            bool guess_scheme);

CURLUcode Curl_url_ਸੈੱਟ_authority(CURLU *u, ਅਚਲ ਅੱਖਰ *authority);

CURLUcode Curl_junkscan(ਅਚਲ ਅੱਖਰ *url, size_t *urllen, bool allowspace);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_CURLU_URLDECODE  (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)CURLU_URLDECODE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_CURLU_ਰਸਤਾ_AS_IS (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)CURLU_ਰਸਤਾ_AS_IS

bool Curl_url_same_origin(CURLU *ਅਧਾਰ, CURLU *href);

CURLUcode Curl_url_get_ਪੋਰਟ(CURLU *u, uint16_t *pport);

#ਜੇ_ਅੰਤ /* HEADER_CURL_URLAPI_INT_H */
