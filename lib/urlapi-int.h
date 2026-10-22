#अगर_अघोषित HEADER_CURL_URLAPI_INT_H
#घोषणा HEADER_CURL_URLAPI_INT_H
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
#समावेश <curl/urlapi.h>

/* Internal representation of CURLU. Point to URL-encoded strings. */
काष्ठा Curl_URL {
  अक्षर *scheme;
  अक्षर *user;
  अक्षर *password;
  अक्षर *options; /* IMAP only? */
  अक्षर *host;
  अक्षर *zoneid; /* for numerical IPv6 addresses */
  अक्षर *path;
  अक्षर *query;
  अक्षर *fragment;
  uint16_t portnum; /* the numerical port if present */
  BIT(port_present);    /* to support missing port */
  BIT(query_present);    /* to support blank */
  BIT(fragment_present); /* to support blank */
  BIT(guessed_scheme);   /* when a URL without scheme is parsed */
};

#घोषणा HOST_ERROR   (-1) /* out of memory */
#घोषणा HOST_NAME    1
#घोषणा HOST_IPV4    2
#घोषणा HOST_IPV6    3

#घोषणा QUERY_NO      2
#घोषणा QUERY_NOT_YET 3 /* allow to change to query */
#घोषणा QUERY_YES     4

माप_प्रकार Curl_is_absolute_url(स्थिर अक्षर *url, अक्षर *buf, माप_प्रकार buflen,
                            bool guess_scheme);

CURLUcode Curl_url_set_authority(CURLU *u, स्थिर अक्षर *authority);

CURLUcode Curl_junkscan(स्थिर अक्षर *url, माप_प्रकार *urllen, bool allowspace);

#घोषणा U_CURLU_URLDECODE  (अचिन्हित पूर्णांक)CURLU_URLDECODE
#घोषणा U_CURLU_PATH_AS_IS (अचिन्हित पूर्णांक)CURLU_PATH_AS_IS

bool Curl_url_same_origin(CURLU *base, CURLU *href);

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport);

#पूर्ण_अगर /* HEADER_CURL_URLAPI_INT_H */
