#ifndef HEADER_CURL_URL_H
#define HEADER_CURL_URL_H
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
#অন্তর্ভাব "curl_setup.h"

/* Reject URLs exceeding this length */
#define MAX_URL_LEN 0xffff

/*
 * Prototypes for library-wide functions
 */

CURLcode Curl_init_transfer(সংরচনা Curl_easy *data, সংরচনা connectdata *conn);
CURLcode Curl_open(সংরচনা Curl_easy **curl);
শূন্য Curl_init_userdefined(সংরচনা Curl_easy *data);

শূন্য Curl_freeset(সংরচনা Curl_easy *data);
CURLcode Curl_uc_to_curlcode(CURLUcode uc);
CURLcode Curl_close(সংরচনা Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(সংরচনা Curl_easy *data, bool *pconnected);
শূন্য Curl_conn_free(সংরচনা Curl_easy *data, সংরচনা connectdata *conn);
CURLcode Curl_parse_login_details(ধ্রুব বর্ণ *login, ধ্রুব size_t len,
                                  বর্ণ **userp, বর্ণ **passwdp,
                                  বর্ণ **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
প্রকারনির্দেশ শূন্য Curl_meta_dtor(ধ্রুব শূন্য *চাবি, size_t চাবি_len, শূন্য *meta_data);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_set(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *চাবি,
                       শূন্য *meta_data, Curl_meta_dtor *meta_dtor);
শূন্য Curl_meta_remove(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *চাবি);
শূন্য *Curl_meta_get(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *চাবি);
শূন্য Curl_meta_reset(সংরচনা Curl_easy *data);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_set(সংরচনা connectdata *conn, ধ্রুব বর্ণ *চাবি,
                            শূন্য *meta_data, Curl_meta_dtor *meta_dtor);
শূন্য Curl_conn_meta_remove(সংরচনা connectdata *conn, ধ্রুব বর্ণ *চাবি);
শূন্য *Curl_conn_meta_get(সংরচনা connectdata *conn, ধ্রুব বর্ণ *চাবি);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
সংরচনা Curl_easy *Curl_get_admin(সংরচনা Curl_easy *data);

#define CURL_DEFAULT_PROXY_PORT 1080 /* default proxy port unless specified */
#define CURL_DEFAULT_HTTPS_PROXY_PORT 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#যদি defined(USE_HTTP2) || defined(USE_HTTP3)
শূন্য Curl_data_priority_clear_state(সংরচনা Curl_easy *data);
#অন্যথা
#define Curl_data_priority_clear_state(x)
#endif /* USE_HTTP2 || USE_HTTP3 */

#ifdef USE_NGHTTP2
CURLcode Curl_data_priority_add_child(সংরচনা Curl_easy *parent,
                                      সংরচনা Curl_easy *child,
                                      bool exclusive);
#অন্যথা
#define Curl_data_priority_add_child(x, y, z) CURLE_NOT_BUILT_IN
#endif

#endif /* HEADER_CURL_URL_H */
