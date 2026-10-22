#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CF_CAPSULE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CF_CAPSULE_H
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

/* Insert a capsule protocol filter after `cf_at` in the filter chain.
 * The capsule filter encapsulates/decapsulates UDP datagrams using
 * the HTTP Datagram capsule format (RFC 9297). */
CURLcode Curl_cf_capsule_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_capsule;

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CF_CAPSULE_H */
