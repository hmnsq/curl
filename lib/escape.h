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

गणना urlreject {
  REJECT_NADA = 2,
  REJECT_CTRL,
  REJECT_ZERO
};

CURLcode Curl_urldecode(अचर वर्ण *string, माप_प्रकार length,
                        वर्ण **ostring, माप_प्रकार *olen,
                        गणना urlreject ctrl);

शून्य Curl_hexencode(अचर अचिन्हित वर्ण *src, माप_प्रकार len, /* input length */
                    अचिन्हित वर्ण *out, माप_प्रकार olen); /* output buffer size */

शून्य Curl_hexbyte(अचिन्हित वर्ण *dest, /* must fit two bytes */
                  अचिन्हित वर्ण val);

#endif /* HEADER_CURL_ESCAPE_H */
