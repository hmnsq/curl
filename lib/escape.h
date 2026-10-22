#यदि_अपरिभाषितम् शीर्षकम्_CURL_ESCAPE_H
#परिभाषय शीर्षकम्_CURL_ESCAPE_H
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

गणना urlreject {
  REJECT_NADA = 2,
  REJECT_CTRL,
  REJECT_ZERO
};

CURLcode Curl_urldecode(ध्रुव वर्ण *string, size_t दीर्घता,
                        वर्ण **ostring, size_t *olen,
                        गणना urlreject ctrl);

शून्य Curl_hexencode(ध्रुव अचिह्नित वर्ण *src, size_t len, /* input length */
                    अचिह्नित वर्ण *बहिर्गमनम्, size_t olen); /* output buffer size */

शून्य Curl_hexbyte(अचिह्नित वर्ण *dest, /* must fit two bytes */
                  अचिह्नित वर्ण val);

#यदि_अन्तः /* HEADER_CURL_ESCAPE_H */
