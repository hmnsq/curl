#यदि_अपरिभाषितम् शीर्षकम्_CURL_DIGEST_H
#परिभाषय शीर्षकम्_CURL_DIGEST_H
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

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH

#परिभाषय DIGEST_MAX_मानम्_दीर्घता           256
#परिभाषय DIGEST_MAX_CONTENT_दीर्घता         1024

संरचना Curl_str;

/* This is used to extract the realm from a challenge message */
bool Curl_auth_digest_get_pair(ध्रुव वर्ण *str, संरचना Curl_str *मानम्,
                               वर्ण *content, ध्रुव वर्ण **endptr);

#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_DIGEST_H */
