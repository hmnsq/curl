#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VQUIC_QUIC_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VQUIC_QUIC_H
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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
ರಚನೆ Curl_cfilter;
ರಚನೆ Curl_easy;
ರಚನೆ connectdata;
ರಚನೆ Curl_addrinfo;

ಶೂನ್ಯ Curl_quic_ver(ಅಕ್ಷರ *p, size_t len);
ಪೂರ್ಣಾಂಕ Curl_vquic_init(ಶೂನ್ಯ);
ಶೂನ್ಯ Curl_vquic_cleanup(ಶೂನ್ಯ);

CURLcode Curl_qlogdir(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *scid,
                      size_t scidlen,
                      ಪೂರ್ಣಾಂಕ *qlogfdp);

CURLcode Curl_cf_quic_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ Curl_peer *origin,
                                   ರಚನೆ Curl_peer *peer);

CURLcode Curl_cf_quic_create(ರಚನೆ Curl_cfilter **pcf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_peer *origin,
                             ರಚನೆ Curl_peer *peer,
                             uint8_t transport_peer,
                             ರಚನೆ connectdata *conn,
                             ರಚನೆ Curl_sockaddr_ex *addr,
                             ರಚನೆ Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_http3;

#ಆದರೆ !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ Curl_peer *origin,
                                       ರಚನೆ Curl_peer *peer,
                                       ರಚನೆ Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(ರಚನೆ Curl_cfilter **pcf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ Curl_peer *origin,
                                 ರಚನೆ Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ರಚನೆ connectdata *conn,
                                 ರಚನೆ Curl_sockaddr_ex *addr,
                                 ರಚನೆ Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_h3_proxy;

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_vquic_init() 1
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_vquic_cleanup()
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ connectdata *conn);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VQUIC_QUIC_H */
