#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_URL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_URL_H
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

/* Reject URLs exceeding this length */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_URL_LEN 0xffff

/*
 * Prototypes for library-wide functions
 */

CURLcode Curl_init_transfer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn);
CURLcode Curl_open(ਬਣਤਰ Curl_easy **curl);
ਖਾਲੀ Curl_init_userdefined(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

ਖਾਲੀ Curl_freeset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
CURLcode Curl_uc_ਮੰਜ਼ਿਲ_curlcode(CURLUcode uc);
CURLcode Curl_close(ਬਣਤਰ Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *pconnected);
ਖਾਲੀ Curl_conn_free(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn);
CURLcode Curl_parse_login_details(ਅਚਲ ਅੱਖਰ *login, ਅਚਲ size_t len,
                                  ਅੱਖਰ **userp, ਅੱਖਰ **passwdp,
                                  ਅੱਖਰ **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ Curl_meta_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len, ਖਾਲੀ *meta_ਅੰਕੜੇ);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_ਸੈੱਟ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ,
                       ਖਾਲੀ *meta_ਅੰਕੜੇ, Curl_meta_dtor *meta_dtor);
ਖਾਲੀ Curl_meta_remove(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ);
ਖਾਲੀ *Curl_meta_get(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ);
ਖਾਲੀ Curl_meta_reset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_ਸੈੱਟ(ਬਣਤਰ connectdata *conn, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ,
                            ਖਾਲੀ *meta_ਅੰਕੜੇ, Curl_meta_dtor *meta_dtor);
ਖਾਲੀ Curl_conn_meta_remove(ਬਣਤਰ connectdata *conn, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ);
ਖਾਲੀ *Curl_conn_meta_get(ਬਣਤਰ connectdata *conn, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
ਬਣਤਰ Curl_easy *Curl_get_admin(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DEFAULT_PROXY_ਪੋਰਟ 1080 /* default proxy port unless specified */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DEFAULT_HTTPS_PROXY_ਪੋਰਟ 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#ਜੇ defined(USE_HTTP2) || defined(USE_HTTP3)
ਖਾਲੀ Curl_ਅੰਕੜੇ_ਤਰਜੀਹ_clear_state(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ਅੰਕੜੇ_ਤਰਜੀਹ_clear_state(x)
#ਜੇ_ਅੰਤ /* USE_HTTP2 || USE_HTTP3 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2
CURLcode Curl_ਅੰਕੜੇ_ਤਰਜੀਹ_ਜੋੜੋ_ਬੱਚਾ(ਬਣਤਰ Curl_easy *ਮਾਪਾ,
                                      ਬਣਤਰ Curl_easy *ਬੱਚਾ,
                                      bool exclusive);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ਅੰਕੜੇ_ਤਰਜੀਹ_ਜੋੜੋ_ਬੱਚਾ(x, y, z) CURLE_NOT_BUILT_ਵਿੱਚ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_URL_H */
