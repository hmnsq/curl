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

എണ്ണൽ urlreject {
  REJECT_NADA = 2,
  REJECT_CTRL,
  REJECT_ZERO
};

CURLcode Curl_urldecode(സ്ഥിരം അക്ഷരം *string, size_t length,
                        അക്ഷരം **ostring, size_t *olen,
                        എണ്ണൽ urlreject ctrl);

ശൂന്യം Curl_hexencode(സ്ഥിരം unsigned അക്ഷരം *src, size_t len, /* input length */
                    unsigned അക്ഷരം *out, size_t olen); /* output buffer size */

ശൂന്യം Curl_hexbyte(unsigned അക്ഷരം *dest, /* must fit two bytes */
                  unsigned അക്ഷരം val);

#endif /* HEADER_CURL_ESCAPE_H */
