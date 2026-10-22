#که_نه_تعریف_شوی سرلیک_CURL_CAPSULE_H
#تعریف_کړه سرلیک_CURL_CAPSULE_H
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

#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ضمن "curlx/dynbuf.h"
#ضمن "bufq.h"

/* HTTP Capsule constants */
#تعریف_کړه HTTP_CAPSULE_سرلیک_MAX_اندازه    10

/* HTTP Capsule function prototypes */

/**
 * Encapsulate UDP payload into HTTP Datagram capsule format
 * @param q     the bufq to write the capsule to
 * @param buf   Payload buffer
 * @param blen  Payload buffer length
 * @return CURLE_OK on success, error code on failure
 */
CURLcode Curl_capsule_encap_udp_datagram(بنية bufq *q,
                                         ثابت فراغ *buf, size_t blen);

بنية Curl_easy;
بنية Curl_cfilter;

/**
 * Process one UDP capsule from buffer into raw datagram payload bytes.
 * @param cf        Connection filter
 * @param data      Easy handle
 * @param recvbufq  Buffer queue containing capsule data
 * @param buf       Output buffer for one datagram payload
 * @param len       Size of output buffer in bytes
 * @param err       Error code output
 * @return Number of payload bytes written. Check `err` for status.
 */
size_t Curl_capsule_process_udp_raw(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية bufq *recvbufq,
                                    بینښه حرف *buf, size_t len,
                                    CURLcode *err);

#د_که_پای /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#د_که_پای /* HEADER_CURL_CAPSULE_H */
