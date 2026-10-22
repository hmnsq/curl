#ifndef HEADER_CURL_ESCAPE_H
#define HEADER_CURL_ESCAPE_H
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

எண்ணிக்கை urlreject {
  REJECT_NADA = 2,
  REJECT_CTRL,
  REJECT_ZERO
};

CURLcode Curl_urldecode(மாறிலி எழுத்து *string, size_t length,
                        எழுத்து **ostring, size_t *olen,
                        எண்ணிக்கை urlreject ctrl);

வெறுமை Curl_hexencode(மாறிலி unsigned எழுத்து *src, size_t len, /* input length */
                    unsigned எழுத்து *out, size_t olen); /* output buffer size */

வெறுமை Curl_hexbyte(unsigned எழுத்து *dest, /* must fit two bytes */
                  unsigned எழுத்து val);

#endif /* HEADER_CURL_ESCAPE_H */
