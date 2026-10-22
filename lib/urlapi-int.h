#यदि_अपरिभाषितम् शीर्षकम्_CURL_URLAPI_INT_H
#परिभाषय शीर्षकम्_CURL_URLAPI_INT_H
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
#अन्तर्भाव <curl/urlapi.h>

/* Internal representation of CURLU. Point to URL-encoded strings. */
संरचना Curl_URL {
  वर्ण *scheme;
  वर्ण *user;
  वर्ण *password;
  वर्ण *options; /* IMAP only? */
  वर्ण *host;
  वर्ण *zoneid; /* for numerical IPv6 addresses */
  वर्ण *मार्गः;
  वर्ण *query;
  वर्ण *fragment;
  uint16_t portnum; /* the numerical port if present */
  BIT(पत्तनम्_present);    /* to support missing port */
  BIT(query_present);    /* to support blank */
  BIT(fragment_present); /* to support blank */
  BIT(guessed_scheme);   /* when a URL without scheme is parsed */
};

#परिभाषय HOST_ERROR   (-1) /* out of memory */
#परिभाषय HOST_NAME    1
#परिभाषय HOST_IPV4    2
#परिभाषय HOST_IPV6    3

#परिभाषय QUERY_NO      2
#परिभाषय QUERY_NOT_YET 3 /* allow to change to query */
#परिभाषय QUERY_YES     4

size_t Curl_is_absolute_url(ध्रुव वर्ण *url, वर्ण *buf, size_t buflen,
                            bool guess_scheme);

CURLUcode Curl_url_समुच्चयः_authority(CURLU *u, ध्रुव वर्ण *authority);

CURLUcode Curl_junkscan(ध्रुव वर्ण *url, size_t *urllen, bool allowspace);

#परिभाषय U_CURLU_URLDECODE  (अचिह्नित पूर्णाङ्क)CURLU_URLDECODE
#परिभाषय U_CURLU_मार्गः_AS_IS (अचिह्नित पूर्णाङ्क)CURLU_मार्गः_AS_IS

bool Curl_url_same_origin(CURLU *आधारः, CURLU *href);

CURLUcode Curl_url_get_पत्तनम्(CURLU *u, uint16_t *pport);

#यदि_अन्तः /* HEADER_CURL_URLAPI_INT_H */
