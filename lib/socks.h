#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SOCKS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SOCKS_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY

ਬਣਤਰ Curl_peer;
ਬਣਤਰ Curl_creds;

/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior
 */
CURLcode Curl_blockread_all(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅੱਖਰ *buf,
                            size_t blen,
                            size_t *pnread);

#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * This function handles the SOCKS5 GSS-API negotiation and initialization
 */
CURLcode Curl_SOCKS5_gssapi_negotiate(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ Curl_creds *creds);
#ਜੇ_ਅੰਤ

/* Insert a SOCKS filter after `cf_at` for connecting to `dest`.
 * Credentials are optional and NOT duplicated and are
 * expected to exist during connect phase.
 */
CURLcode Curl_cf_socks_proxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ Curl_peer *dest,
                                          uint8_t ip_ਸੰਸਕਰਣ,
                                          uint8_t proxy_type,
                                          ਬਣਤਰ Curl_creds *creds);

ਬਾਹਰੀ ਬਣਤਰ Curl_cftype Curl_cft_socks_proxy;

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY */

#ਜੇ_ਅੰਤ /* HEADER_CURL_SOCKS_H */
