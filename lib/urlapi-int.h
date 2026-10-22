#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_URLAPI_INT_H
#وصف_ڪريو سرو_CURL_URLAPI_INT_H
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
#ضمن "curl_setup.h"
#ضمن <curl/urlapi.h>

/* Internal representation of CURLU. Point to URL-encoded strings. */
بنية Curl_URL {
  حرف *scheme;
  حرف *user;
  حرف *password;
  حرف *options; /* IMAP only? */
  حرف *host;
  حرف *zoneid; /* for numerical IPv6 addresses */
  حرف *رستو;
  حرف *query;
  حرف *fragment;
  uint16_t portnum; /* the numerical port if present */
  ٻٽو_انگ(پورٽ_present);    /* to support missing port */
  ٻٽو_انگ(query_present);    /* to support blank */
  ٻٽو_انگ(fragment_present); /* to support blank */
  ٻٽو_انگ(guessed_scheme);   /* when a URL without scheme is parsed */
};

#وصف_ڪريو HOST_ERROR   (-1) /* out of memory */
#وصف_ڪريو HOST_NAME    1
#وصف_ڪريو HOST_IPV4    2
#وصف_ڪريو HOST_IPV6    3

#وصف_ڪريو QUERY_NO      2
#وصف_ڪريو QUERY_NOT_YET 3 /* allow to change to query */
#وصف_ڪريو QUERY_YES     4

size_t Curl_is_absolute_url(ثابت حرف *url, حرف *buf, size_t buflen,
                            bool guess_scheme);

CURLUcode Curl_url_سيٽ_authority(CURLU *u, ثابت حرف *authority);

CURLUcode Curl_junkscan(ثابت حرف *url, size_t *urllen, bool allowspace);

#وصف_ڪريو U_CURLU_URLDECODE  (بنانشان صحيح)CURLU_URLDECODE
#وصف_ڪريو U_CURLU_رستو_AS_IS (بنانشان صحيح)CURLU_رستو_AS_IS

bool Curl_url_same_origin(CURLU *بنياد, CURLU *href);

CURLUcode Curl_url_get_پورٽ(CURLU *u, uint16_t *pport);

#جيڪڏهن_پورو /* HEADER_CURL_URLAPI_INT_H */
