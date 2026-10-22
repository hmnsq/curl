#ifndef HEADER_CURL_CAPSULE_H
#define HEADER_CURL_CAPSULE_H
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

#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "bufq.h"

/* HTTP Capsule constants */
#define HTTP_CAPSULE_HEADER_MAX_SIZE    10

/* HTTP Capsule function prototypes */

/**
 * Encapsulate UDP payload into HTTP Datagram capsule format
 * @param q     the bufq to write the capsule to
 * @param buf   Payload buffer
 * @param blen  Payload buffer length
 * @return CURLE_OK on success, error code on failure
 */
CURLcode Curl_capsule_encap_udp_datagram(கட்டமைப்பு bufq *q,
                                         மாறிலி வெறுமை *buf, size_t blen);

கட்டமைப்பு Curl_easy;
கட்டமைப்பு Curl_cfilter;

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
size_t Curl_capsule_process_udp_raw(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு bufq *recvbufq,
                                    unsigned எழுத்து *buf, size_t len,
                                    CURLcode *err);

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#endif /* HEADER_CURL_CAPSULE_H */
