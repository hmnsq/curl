#నిర్వచితం_కాకపోతే శీర్షిక_CURL_ESCAPE_H
#నిర్వచించు శీర్షిక_CURL_ESCAPE_H
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

గణన urlreject {
  REJECT_NADA = 2,
  REJECT_CTRL,
  REJECT_ZERO
};

CURLcode Curl_urldecode(స్థిరరాశి అక్షరం *string, size_t నిడివి,
                        అక్షరం **ostring, size_t *olen,
                        గణన urlreject ctrl);

శూన్య Curl_hexencode(స్థిరరాశి గుర్తులేని అక్షరం *src, size_t len, /* input length */
                    గుర్తులేని అక్షరం *బహిర్గతం, size_t olen); /* output buffer size */

శూన్య Curl_hexbyte(గుర్తులేని అక్షరం *dest, /* must fit two bytes */
                  గుర్తులేని అక్షరం val);

#అయితే_ముగింపు /* HEADER_CURL_ESCAPE_H */
