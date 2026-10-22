#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_ESCAPE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_ESCAPE_H
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

ਗਿਣਤੀ urlreject {
  REJECT_NADA = 2,
  REJECT_CTRL,
  REJECT_ZERO
};

CURLcode Curl_urldecode(ਅਚਲ ਅੱਖਰ *string, size_t ਲੰਮਾਈ,
                        ਅੱਖਰ **ostring, size_t *olen,
                        ਗਿਣਤੀ urlreject ctrl);

ਖਾਲੀ Curl_hexencode(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *src, size_t len, /* input length */
                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਾਹਰਲਾ, size_t olen); /* output buffer size */

ਖਾਲੀ Curl_hexbyte(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *dest, /* must fit two bytes */
                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ val);

#ਜੇ_ਅੰਤ /* HEADER_CURL_ESCAPE_H */
