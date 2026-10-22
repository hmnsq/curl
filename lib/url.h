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
#شامل "curl_setup.h"

/* Reject URLs exceeding this length */
#define MAX_URL_LEN 0xffff

/*
 * Prototypes for library-wide functions
 */

CURLcode Curl_init_transfer(ساختار Curl_easy *data, ساختار connectdata *conn);
CURLcode Curl_open(ساختار Curl_easy **curl);
تهی Curl_init_userdefined(ساختار Curl_easy *data);

تهی Curl_freeset(ساختار Curl_easy *data);
CURLcode Curl_uc_to_curlcode(CURLUcode uc);
CURLcode Curl_close(ساختار Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(ساختار Curl_easy *data, bool *pconnected);
تهی Curl_conn_free(ساختار Curl_easy *data, ساختار connectdata *conn);
CURLcode Curl_parse_login_details(ثابت نویسه *login, ثابت size_t len,
                                  نویسه **userp, نویسه **passwdp,
                                  نویسه **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
تعریفنوع تهی Curl_meta_dtor(ثابت تهی *کلید, size_t کلید_len, تهی *meta_data);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_set(ساختار Curl_easy *data, ثابت نویسه *کلید,
                       تهی *meta_data, Curl_meta_dtor *meta_dtor);
تهی Curl_meta_remove(ساختار Curl_easy *data, ثابت نویسه *کلید);
تهی *Curl_meta_get(ساختار Curl_easy *data, ثابت نویسه *کلید);
تهی Curl_meta_reset(ساختار Curl_easy *data);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_set(ساختار connectdata *conn, ثابت نویسه *کلید,
                            تهی *meta_data, Curl_meta_dtor *meta_dtor);
تهی Curl_conn_meta_remove(ساختار connectdata *conn, ثابت نویسه *کلید);
تهی *Curl_conn_meta_get(ساختار connectdata *conn, ثابت نویسه *کلید);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
ساختار Curl_easy *Curl_get_admin(ساختار Curl_easy *data);

#define CURL_DEFAULT_PROXY_PORT 1080 /* default proxy port unless specified */
#define CURL_DEFAULT_HTTPS_PROXY_PORT 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#اگر defined(USE_HTTP2) || defined(USE_HTTP3)
تهی Curl_data_priority_clear_state(ساختار Curl_easy *data);
#وگرنه
#define Curl_data_priority_clear_state(x)
#endif /* USE_HTTP2 || USE_HTTP3 */

#ifdef USE_NGHTTP2
CURLcode Curl_data_priority_add_child(ساختار Curl_easy *parent,
                                      ساختار Curl_easy *child,
                                      bool exclusive);
#وگرنه
#define Curl_data_priority_add_child(x, y, z) CURLE_NOT_BUILT_IN
#endif

#endif /* HEADER_CURL_URL_H */
