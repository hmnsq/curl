#यदि_अपरिभाषितम् शीर्षकम्_CURL_CIPHER_SUITE_H
#परिभाषय शीर्षकम्_CURL_CIPHER_SUITE_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Jan Venekamp, <jan@venekamp.net>
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
#अन्तर्भाव "curl_setup.h"

#यदि defined(USE_MBEDTLS) || defined(USE_RUSTLS)

/* Lookup IANA id for cipher suite string, returns 0 if not recognized */
uint16_t Curl_cipher_suite_lookup_id(ध्रुव वर्ण *cs_str, size_t cs_len);

/* Walk over cipher suite string, update str and end pointers to next
   cipher suite in string, returns IANA id of that suite if recognized */
uint16_t Curl_cipher_suite_walk_str(ध्रुव वर्ण **str, ध्रुव वर्ण **अन्तः);

/* Copy openssl or RFC name for cipher suite in supplied buffer.
   Caller is responsible to supply sufficiently large buffer (size
   of 64 should suffice), excess bytes are silently truncated. */
पूर्णाङ्क Curl_cipher_suite_get_str(uint16_t id, वर्ण *buf, size_t buf_आकार,
                              bool prefer_rfc);

#यदि_अन्तः /* defined(USE_MBEDTLS) || defined(USE_RUSTLS) */
#यदि_अन्तः /* HEADER_CURL_CIPHER_SUITE_H */
