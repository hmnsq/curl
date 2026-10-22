#ifndef HEADER_CURL_SOCKS_H
#define HEADER_CURL_SOCKS_H
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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

સંરચના Curl_peer;
સંરચના Curl_creds;

/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior
 */
CURLcode Curl_blockread_all(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            અક્ષર *buf,
                            size_t blen,
                            size_t *pnread);

#જો defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * This function handles the SOCKS5 GSS-API negotiation and initialization
 */
CURLcode Curl_SOCKS5_gssapi_negotiate(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      સંરચના Curl_creds *creds);
#endif

/* Insert a SOCKS filter after `cf_at` for connecting to `dest`.
 * Credentials are optional and NOT duplicated and are
 * expected to exist during connect phase.
 */
CURLcode Curl_cf_socks_proxy_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                          સંરચના Curl_easy *data,
                                          સંરચના Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          સંરચના Curl_creds *creds);

બાહ્ય સંરચના Curl_cftype Curl_cft_socks_proxy;

#endif /* !CURL_DISABLE_PROXY */

#endif /* HEADER_CURL_SOCKS_H */
