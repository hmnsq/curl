#अगर_अघोषित HEADER_CURL_CAPSULE_H
#घोषणा HEADER_CURL_CAPSULE_H
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

#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#समावेश "curlx/dynbuf.h"
#समावेश "bufq.h"

/* HTTP Capsule constants */
#घोषणा HTTP_CAPSULE_HEADER_MAX_आकार    10

/* HTTP Capsule function prototypes */

/**
 * Encapsulate UDP payload into HTTP Datagram capsule format
 * @param q     the bufq to write the capsule to
 * @param buf   Payload buffer
 * @param blen  Payload buffer length
 * @return CURLE_OK on success, error code on failure
 */
CURLcode Curl_capsule_encap_udp_datagram(काष्ठा bufq *q,
                                         स्थिर व्योम *buf, माप_प्रकार blen);

काष्ठा Curl_easy;
काष्ठा Curl_cfilter;

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
माप_प्रकार Curl_capsule_process_udp_raw(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा bufq *recvbufq,
                                    अचिन्हित अक्षर *buf, माप_प्रकार len,
                                    CURLcode *err);

#पूर्ण_अगर /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#पूर्ण_अगर /* HEADER_CURL_CAPSULE_H */
