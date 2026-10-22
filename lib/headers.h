#यदि_अपरिभाषितम् शीर्षकम्_CURL_शीर्षकम्_H
#परिभाषय शीर्षकम्_CURL_शीर्षकम्_H
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

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_शीर्षकाणि_API)

संरचना Curl_शीर्षकम्_store {
  संरचना Curl_llist_ग्रन्थिः ग्रन्थिः;
  वर्ण *name; /* points into 'buffer' */
  वर्ण *मानम्; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_सूचकांक; /* cached index for curl_easy_nextheader() */
  पूर्णाङ्क request; /* 0 is the first request, then 1.. 2.. */
  अचिह्नित वर्ण type; /* CURLH_* defines */
  वर्ण बफर[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_शीर्षकाणि_init(संरचना Curl_easy *दत्त);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_शीर्षकाणि_प्रक्षिप(संरचना Curl_easy *दत्त, ध्रुव वर्ण *शीर्षकम्,
                           size_t hlen, अचिह्नित वर्ण type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_शीर्षकाणि_cleanup(संरचना Curl_easy *दत्त);

#अन्यथा
#परिभाषय Curl_शीर्षकाणि_init(x)          CURLE_OK
#परिभाषय Curl_शीर्षकाणि_प्रक्षिप(x, y, z, a) \
  ((शून्य)(x), (शून्य)(y), (शून्य)(z), (शून्य)(a), CURLE_OK)
#परिभाषय Curl_शीर्षकाणि_cleanup(x)       Curl_nop_stmt
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_HEADER_H */
