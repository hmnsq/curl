#ifndef HEADER_CURL_URLAPI_INT_H
#define HEADER_CURL_URLAPI_INT_H
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
  वर्ण *path;
  वर्ण *query;
  वर्ण *fragment;
  uint16_t portnum; /* the numerical port if present */
  BIT(port_present);    /* to support missing port */
  BIT(query_present);    /* to support blank */
  BIT(fragment_present); /* to support blank */
  BIT(guessed_scheme);   /* when a URL without scheme is parsed */
};

#define HOST_ERROR   (-1) /* out of memory */
#define HOST_NAME    1
#define HOST_IPV4    2
#define HOST_IPV6    3

#define QUERY_NO      2
#define QUERY_NOT_YET 3 /* allow to change to query */
#define QUERY_YES     4

size_t Curl_is_absolute_url(ध्रुव वर्ण *url, वर्ण *buf, size_t buflen,
                            bool guess_scheme);

CURLUcode Curl_url_set_authority(CURLU *u, ध्रुव वर्ण *authority);

CURLUcode Curl_junkscan(ध्रुव वर्ण *url, size_t *urllen, bool allowspace);

#define U_CURLU_URLDECODE  (unsigned पूर्णाङ्क)CURLU_URLDECODE
#define U_CURLU_PATH_AS_IS (unsigned पूर्णाङ्क)CURLU_PATH_AS_IS

bool Curl_url_same_origin(CURLU *base, CURLU *href);

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport);

#endif /* HEADER_CURL_URLAPI_INT_H */
