#अगर_अघोषित HEADER_CURL_HEADER_H
#घोषणा HEADER_CURL_HEADER_H
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

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

काष्ठा Curl_header_store {
  काष्ठा Curl_llist_गाँठ गाँठ;
  अक्षर *name; /* points into 'buffer' */
  अक्षर *मूल्य; /* points into 'buffer' */
  माप_प्रकार nh_amount; /* cached amount for curl_easy_nextheader() */
  माप_प्रकार nh_सूचकांक; /* cached index for curl_easy_nextheader() */
  पूर्णांक request; /* 0 is the first request, then 1.. 2.. */
  अचिन्हित अक्षर type; /* CURLH_* defines */
  अक्षर बफर[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_headers_init(काष्ठा Curl_easy *तथ्य);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_headers_धकेलो(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *header,
                           माप_प्रकार hlen, अचिन्हित अक्षर type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(काष्ठा Curl_easy *तथ्य);

#अन्यथा
#घोषणा Curl_headers_init(x)          CURLE_OK
#घोषणा Curl_headers_धकेलो(x, y, z, a) \
  ((व्योम)(x), (व्योम)(y), (व्योम)(z), (व्योम)(a), CURLE_OK)
#घोषणा Curl_headers_cleanup(x)       Curl_nop_stmt
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_HEADER_H */
