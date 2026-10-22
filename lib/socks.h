#अगर_अघोषित HEADER_CURL_SOCKS_H
#घोषणा HEADER_CURL_SOCKS_H
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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_PROXY

काष्ठा Curl_peer;
काष्ठा Curl_creds;

/*
 * Helper read-from-socket functions. Does the same as Curl_read() but it
 * blocks until all bytes amount of buffersize will be read. No more, no less.
 *
 * This is STUPID BLOCKING behavior
 */
CURLcode Curl_blockread_all(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            अक्षर *buf,
                            माप_प्रकार blen,
                            माप_प्रकार *pnread);

#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
/*
 * This function handles the SOCKS5 GSS-API negotiation and initialization
 */
CURLcode Curl_SOCKS5_gssapi_negotiate(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा Curl_creds *creds);
#पूर्ण_अगर

/* Insert a SOCKS filter after `cf_at` for connecting to `dest`.
 * Credentials are optional and NOT duplicated and are
 * expected to exist during connect phase.
 */
CURLcode Curl_cf_socks_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                          काष्ठा Curl_easy *तथ्य,
                                          काष्ठा Curl_peer *dest,
                                          uint8_t ip_version,
                                          uint8_t proxy_type,
                                          काष्ठा Curl_creds *creds);

बाह्य काष्ठा Curl_cftype Curl_cft_socks_proxy;

#पूर्ण_अगर /* !CURL_DISABLE_PROXY */

#पूर्ण_अगर /* HEADER_CURL_SOCKS_H */
