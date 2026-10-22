#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CF_DNS_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CF_DNS_H
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

ರಚನೆ Curl_easy;
ರಚನೆ connectdata;
ರಚನೆ Curl_dns_ನಮೂದು;
ರಚನೆ Curl_addrinfo;
ರಚನೆ Curl_peer;

CURLcode Curl_conn_dns_ಕೂಡಿಸು_addr_resolve(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ connectdata *conn,
                                        int8_t sockindex,
                                        ರಚನೆ Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_ಫಲಿತಾಂಶ(ರಚನೆ connectdata *conn,
                                   int8_t sockindex,
                                   ರಚನೆ Curl_peer *peer);

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *Curl_conn_dns_get_ai(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                 ರಚನೆ Curl_peer *peer,
                                                 int8_t sockindex,
                                                 ಪೂರ್ಣಾಂಕ ai_family,
                                                 ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
CURLcode Curl_conn_dns_ಕೂಡಿಸು_https_resolve(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ connectdata *conn,
                                         int8_t sockindex,
                                         ರಚನೆ Curl_peer *peer);

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_https_rrinfo *
Curl_conn_dns_get_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        int8_t sockindex,
                        ರಚನೆ Curl_peer *peer);
bool Curl_conn_dns_resolved_https(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  int8_t sockindex,
                                  ರಚನೆ Curl_peer *peer);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_conn_dns_get_https(a, b, c)        ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_conn_dns_resolved_https(a, b, c)   TRUE
#ಆದರೆ_ಅಂತ್ಯ

ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_dns;

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CF_DNS_H */
