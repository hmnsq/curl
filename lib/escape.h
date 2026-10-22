#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_ESCAPE_H
#وصف_ڪريو سرو_CURL_ESCAPE_H
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

CURLcode Curl_urldecode(ثابت حرف *string, size_t ڊيگهائي,
                        حرف **ostring, size_t *olen,
                        تعداد urlreject ctrl);

فراغ Curl_hexencode(ثابت بنانشان حرف *src, size_t len, /* input length */
                    بنانشان حرف *ٻاهريون, size_t olen); /* output buffer size */

فراغ Curl_hexbyte(بنانشان حرف *dest, /* must fit two bytes */
                  بنانشان حرف val);

#جيڪڏهن_پورو /* HEADER_CURL_ESCAPE_H */
