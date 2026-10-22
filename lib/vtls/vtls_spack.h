#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VTLS_SPACK_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VTLS_SPACK_H
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

#ಆದರೆ defined(USE_SSL) && defined(USE_SSLS_EXPORT)

ರಚನೆ dynbuf;
ರಚನೆ Curl_ssl_session;

CURLcode Curl_ssl_session_pack(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_ssl_session *s,
                               ರಚನೆ dynbuf *buf);

CURLcode Curl_ssl_session_unpack(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *bufv, size_t buflen,
                                 ರಚನೆ Curl_ssl_session **ps);

#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL && USE_SSLS_EXPORT */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VTLS_SPACK_H */
