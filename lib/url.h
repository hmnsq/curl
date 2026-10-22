#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_URL_H
#وصف_ڪريو سرو_CURL_URL_H
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
#ضمن "curl_setup.h"

/* Reject URLs exceeding this length */
#وصف_ڪريو MAX_URL_LEN 0xffff

/*
 * Prototypes for library-wide functions
 */

CURLcode Curl_init_transfer(بنية Curl_easy *معلومات, بنية connectdata *conn);
CURLcode Curl_open(بنية Curl_easy **curl);
فراغ Curl_init_userdefined(بنية Curl_easy *معلومات);

فراغ Curl_freeset(بنية Curl_easy *معلومات);
CURLcode Curl_uc_منزل_curlcode(CURLUcode uc);
CURLcode Curl_close(بنية Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(بنية Curl_easy *معلومات, bool *pconnected);
فراغ Curl_conn_free(بنية Curl_easy *معلومات, بنية connectdata *conn);
CURLcode Curl_parse_login_details(ثابت حرف *login, ثابت size_t len,
                                  حرف **userp, حرف **passwdp,
                                  حرف **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
تعريفنوع فراغ Curl_meta_dtor(ثابت فراغ *ڪنجي, size_t ڪنجي_len, فراغ *meta_معلومات);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_سيٽ(بنية Curl_easy *معلومات, ثابت حرف *ڪنجي,
                       فراغ *meta_معلومات, Curl_meta_dtor *meta_dtor);
فراغ Curl_meta_remove(بنية Curl_easy *معلومات, ثابت حرف *ڪنجي);
فراغ *Curl_meta_get(بنية Curl_easy *معلومات, ثابت حرف *ڪنجي);
فراغ Curl_meta_reset(بنية Curl_easy *معلومات);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_سيٽ(بنية connectdata *conn, ثابت حرف *ڪنجي,
                            فراغ *meta_معلومات, Curl_meta_dtor *meta_dtor);
فراغ Curl_conn_meta_remove(بنية connectdata *conn, ثابت حرف *ڪنجي);
فراغ *Curl_conn_meta_get(بنية connectdata *conn, ثابت حرف *ڪنجي);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
بنية Curl_easy *Curl_get_admin(بنية Curl_easy *معلومات);

#وصف_ڪريو CURL_DEFAULT_PROXY_پورٽ 1080 /* default proxy port unless specified */
#وصف_ڪريو CURL_DEFAULT_HTTPS_PROXY_پورٽ 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#إذا defined(USE_HTTP2) || defined(USE_HTTP3)
فراغ Curl_معلومات_ترجيح_clear_state(بنية Curl_easy *معلومات);
#وإلا
#وصف_ڪريو Curl_معلومات_ترجيح_clear_state(x)
#جيڪڏهن_پورو /* USE_HTTP2 || USE_HTTP3 */

#جيڪڏهن_وصف_ٿيل USE_NGHTTP2
CURLcode Curl_معلومات_ترجيح_جوڙيو_ٻار(بنية Curl_easy *والد,
                                      بنية Curl_easy *ٻار,
                                      bool exclusive);
#وإلا
#وصف_ڪريو Curl_معلومات_ترجيح_جوڙيو_ٻار(x, y, z) CURLE_NOT_BUILT_اندريون
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_URL_H */
