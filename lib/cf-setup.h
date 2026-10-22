#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CF_ಸಿದ್ಧತೆ_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CF_ಸಿದ್ಧತೆ_H
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

ರಚನೆ Curl_dns_ನಮೂದು;
ರಚನೆ ip_quadruple;
ರಚನೆ Curl_peer;
ರಚನೆ Curl_str;

CURLcode Curl_cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           ಪೂರ್ಣಾಂಕ ssl_ಬಹುಲಕ);

CURLcode Curl_cf_ಸಿದ್ಧತೆ_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    uint8_t transport,
                                    ಪೂರ್ಣಾಂಕ ssl_ಬಹುಲಕ);

ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_ಸಿದ್ಧತೆ;

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CF_SETUP_H */
