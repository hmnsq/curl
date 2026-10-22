#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SOCKS_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SOCKS_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY

ರಚನೆ Curl_peer;
ರಚನೆ Curl_creds;

/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior
 */
CURLcode Curl_blockread_all(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಅಕ್ಷರ *buf,
                            size_t blen,
                            size_t *pnread);

#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * This function handles the SOCKS5 GSS-API negotiation and initialization
 */
CURLcode Curl_SOCKS5_gssapi_negotiate(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ Curl_creds *creds);
#ಆದರೆ_ಅಂತ್ಯ

/* Insert a SOCKS filter after `cf_at` for connecting to `dest`.
 * Credentials are optional and NOT duplicated and are
 * expected to exist during connect phase.
 */
CURLcode Curl_cf_socks_proxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ Curl_peer *dest,
                                          uint8_t ip_ಆವೃತ್ತಿ,
                                          uint8_t proxy_type,
                                          ರಚನೆ Curl_creds *creds);

ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_socks_proxy;

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SOCKS_H */
