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

গণনা urlreject {
  REJECT_NADA = 2,
  REJECT_CTRL,
  REJECT_ZERO
};

CURLcode Curl_urldecode(ধ্রুবক অক্ষর *string, size_t দৈর্ঘ্য,
                        অক্ষর **ostring, size_t *olen,
                        গণনা urlreject ctrl);

শূন্য Curl_hexencode(ধ্রুবক unsigned অক্ষর *src, size_t len, /* input length */
                    unsigned অক্ষর *out, size_t olen); /* output buffer size */

শূন্য Curl_hexbyte(unsigned অক্ষর *dest, /* must fit two bytes */
                  unsigned অক্ষর val);

#endif /* HEADER_CURL_ESCAPE_H */
