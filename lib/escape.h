#که_نه_تعریف_شوی سرلیک_CURL_ESCAPE_H
#تعریف_کړه سرلیک_CURL_ESCAPE_H
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

تعداد urlreject {
  REJECT_NADA = 2,
  REJECT_CTRL,
  REJECT_ZERO
};

CURLcode Curl_urldecode(ثابت حرف *string, size_t اوږدتیا,
                        حرف **ostring, size_t *olen,
                        تعداد urlreject ctrl);

فراغ Curl_hexencode(ثابت بینښه حرف *src, size_t len, /* input length */
                    بینښه حرف *بهر, size_t olen); /* output buffer size */

فراغ Curl_hexbyte(بینښه حرف *dest, /* must fit two bytes */
                  بینښه حرف val);

#د_که_پای /* HEADER_CURL_ESCAPE_H */
