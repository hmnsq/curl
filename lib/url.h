#अगर_अघोषित HEADER_CURL_URL_H
#घोषणा HEADER_CURL_URL_H
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

/* Reject URLs exceeding this length */
#घोषणा MAX_URL_LEN 0xffff

/*
 * Prototypes for library-wide functions
 */

CURLcode Curl_init_transfer(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn);
CURLcode Curl_open(काष्ठा Curl_easy **curl);
व्योम Curl_init_userdefined(काष्ठा Curl_easy *तथ्य);

व्योम Curl_freeset(काष्ठा Curl_easy *तथ्य);
CURLcode Curl_uc_to_curlcode(CURLUcode uc);
CURLcode Curl_close(काष्ठा Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(काष्ठा Curl_easy *तथ्य, bool *pconnected);
व्योम Curl_conn_free(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn);
CURLcode Curl_parse_login_details(स्थिर अक्षर *login, स्थिर माप_प्रकार len,
                                  अक्षर **userp, अक्षर **passwdp,
                                  अक्षर **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
प्रकार व्योम Curl_meta_dtor(स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len, व्योम *meta_तथ्य);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_set(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *कुंजी,
                       व्योम *meta_तथ्य, Curl_meta_dtor *meta_dtor);
व्योम Curl_meta_remove(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *कुंजी);
व्योम *Curl_meta_get(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *कुंजी);
व्योम Curl_meta_reset(काष्ठा Curl_easy *तथ्य);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_set(काष्ठा connectdata *conn, स्थिर अक्षर *कुंजी,
                            व्योम *meta_तथ्य, Curl_meta_dtor *meta_dtor);
व्योम Curl_conn_meta_remove(काष्ठा connectdata *conn, स्थिर अक्षर *कुंजी);
व्योम *Curl_conn_meta_get(काष्ठा connectdata *conn, स्थिर अक्षर *कुंजी);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
काष्ठा Curl_easy *Curl_get_admin(काष्ठा Curl_easy *तथ्य);

#घोषणा CURL_DEFAULT_PROXY_PORT 1080 /* default proxy port unless specified */
#घोषणा CURL_DEFAULT_HTTPS_PROXY_PORT 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#अगर defined(USE_HTTP2) || defined(USE_HTTP3)
व्योम Curl_तथ्य_priority_clear_state(काष्ठा Curl_easy *तथ्य);
#अन्यथा
#घोषणा Curl_तथ्य_priority_clear_state(x)
#पूर्ण_अगर /* USE_HTTP2 || USE_HTTP3 */

#अगर_घोषित USE_NGHTTP2
CURLcode Curl_तथ्य_priority_add_child(काष्ठा Curl_easy *parent,
                                      काष्ठा Curl_easy *child,
                                      bool exclusive);
#अन्यथा
#घोषणा Curl_तथ्य_priority_add_child(x, y, z) CURLE_NOT_BUILT_IN
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_URL_H */
