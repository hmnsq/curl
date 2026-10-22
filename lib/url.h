#اگر_غیر_تعریف_شدہ سرخی_CURL_URL_H
#تعریف_کرو سرخی_CURL_URL_H
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
#شامل "curl_setup.h"

/* Reject URLs exceeding this length */
#تعریف_کرو MAX_URL_LEN 0xffff

/*
 * Prototypes for library-wide functions
 */

CURLcode Curl_init_transfer(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn);
CURLcode Curl_open(ساخت Curl_easy **curl);
خالی Curl_init_userdefined(ساخت Curl_easy *ڈیٹا);

خالی Curl_freeset(ساخت Curl_easy *ڈیٹا);
CURLcode Curl_uc_مقصد_curlcode(CURLUcode uc);
CURLcode Curl_close(ساخت Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(ساخت Curl_easy *ڈیٹا, bool *pconnected);
خالی Curl_conn_free(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn);
CURLcode Curl_parse_login_details(ثابت حرف *login, ثابت size_t len,
                                  حرف **userp, حرف **passwdp,
                                  حرف **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
قسمتعریف خالی Curl_meta_dtor(ثابت خالی *کلید, size_t کلید_len, خالی *meta_ڈیٹا);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_سیٹ(ساخت Curl_easy *ڈیٹا, ثابت حرف *کلید,
                       خالی *meta_ڈیٹا, Curl_meta_dtor *meta_dtor);
خالی Curl_meta_remove(ساخت Curl_easy *ڈیٹا, ثابت حرف *کلید);
خالی *Curl_meta_get(ساخت Curl_easy *ڈیٹا, ثابت حرف *کلید);
خالی Curl_meta_reset(ساخت Curl_easy *ڈیٹا);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_سیٹ(ساخت connectdata *conn, ثابت حرف *کلید,
                            خالی *meta_ڈیٹا, Curl_meta_dtor *meta_dtor);
خالی Curl_conn_meta_remove(ساخت connectdata *conn, ثابت حرف *کلید);
خالی *Curl_conn_meta_get(ساخت connectdata *conn, ثابت حرف *کلید);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
ساخت Curl_easy *Curl_get_admin(ساخت Curl_easy *ڈیٹا);

#تعریف_کرو CURL_DEFAULT_PROXY_پورٹ 1080 /* default proxy port unless specified */
#تعریف_کرو CURL_DEFAULT_HTTPS_PROXY_پورٹ 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#اگر defined(USE_HTTP2) || defined(USE_HTTP3)
خالی Curl_ڈیٹا_ترجیح_clear_state(ساخت Curl_easy *ڈیٹا);
#ورنہ
#تعریف_کرو Curl_ڈیٹا_ترجیح_clear_state(x)
#اگر_ختم /* USE_HTTP2 || USE_HTTP3 */

#اگر_تعریف_شدہ USE_NGHTTP2
CURLcode Curl_ڈیٹا_ترجیح_جوڑو_بچہ(ساخت Curl_easy *والد,
                                      ساخت Curl_easy *بچہ,
                                      bool exclusive);
#ورنہ
#تعریف_کرو Curl_ڈیٹا_ترجیح_جوڑو_بچہ(x, y, z) CURLE_NOT_BUILT_اندر
#اگر_ختم

#اگر_ختم /* HEADER_CURL_URL_H */
