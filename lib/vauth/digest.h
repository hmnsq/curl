#अगर_अघोषित HEADER_CURL_DIGEST_H
#घोषणा HEADER_CURL_DIGEST_H
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

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH

#घोषणा DIGEST_MAX_मूल्य_लंबाई           256
#घोषणा DIGEST_MAX_CONTENT_लंबाई         1024

काष्ठा Curl_str;

/* This is used to extract the realm from a challenge message */
bool Curl_auth_digest_get_pair(स्थिर अक्षर *str, काष्ठा Curl_str *मूल्य,
                               अक्षर *content, स्थिर अक्षर **endptr);

#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_DIGEST_H */
