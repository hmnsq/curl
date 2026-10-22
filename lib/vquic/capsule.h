#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CAPSULE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CAPSULE_H
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

#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"

/* HTTP Capsule constants */
#ವ್ಯಾಖ್ಯಾನಿಸು HTTP_CAPSULE_ಶೀರ್ಷಿಕೆ_MAX_ಗಾತ್ರ    10

/* HTTP Capsule function prototypes */

/**
 * Encapsulate UDP payload into HTTP Datagram capsule format
 * @param q     the bufq to write the capsule to
 * @param buf   Payload buffer
 * @param blen  Payload buffer length
 * @return CURLE_OK on success, error code on failure
 */
CURLcode Curl_capsule_encap_udp_datagram(ರಚನೆ bufq *q,
                                         ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t blen);

ರಚನೆ Curl_easy;
ರಚನೆ Curl_cfilter;

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
size_t Curl_capsule_process_udp_raw(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ bufq *recvbufq,
                                    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t len,
                                    CURLcode *err);

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CAPSULE_H */
