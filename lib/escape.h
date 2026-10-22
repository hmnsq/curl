#अगर_अघोषित HEADER_CURL_ESCAPE_H
#घोषणा HEADER_CURL_ESCAPE_H
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
/* Escape and unescape URL encoding in strings. The functions return a new
 * allocated string or NULL if an error occurred. */

क्रमागत urlreject {
  REJECT_NADA = 2,
  REJECT_CTRL,
  REJECT_ZERO
};

CURLcode Curl_urldecode(स्थिर अक्षर *string, माप_प्रकार लंबाई,
                        अक्षर **ostring, माप_प्रकार *olen,
                        क्रमागत urlreject ctrl);

व्योम Curl_hexencode(स्थिर अचिन्हित अक्षर *src, माप_प्रकार len, /* input length */
                    अचिन्हित अक्षर *out, माप_प्रकार olen); /* output buffer size */

व्योम Curl_hexbyte(अचिन्हित अक्षर *dest, /* must fit two bytes */
                  अचिन्हित अक्षर val);

#पूर्ण_अगर /* HEADER_CURL_ESCAPE_H */
