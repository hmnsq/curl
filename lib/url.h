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
#അന്തര്ഭാവ "curl_setup.h"

/* Reject URLs exceeding this length */
#define MAX_URL_LEN 0xffff

/*
 * Prototypes for library-wide functions
 */

CURLcode Curl_init_transfer(ഘടന Curl_easy *data, ഘടന connectdata *conn);
CURLcode Curl_open(ഘടന Curl_easy **curl);
ശൂന്യം Curl_init_userdefined(ഘടന Curl_easy *data);

ശൂന്യം Curl_freeset(ഘടന Curl_easy *data);
CURLcode Curl_uc_to_curlcode(CURLUcode uc);
CURLcode Curl_close(ഘടന Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(ഘടന Curl_easy *data, bool *pconnected);
ശൂന്യം Curl_conn_free(ഘടന Curl_easy *data, ഘടന connectdata *conn);
CURLcode Curl_parse_login_details(സ്ഥിരം അക്ഷരം *login, സ്ഥിരം size_t len,
                                  അക്ഷരം **userp, അക്ഷരം **passwdp,
                                  അക്ഷരം **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
തരനിർവചനം ശൂന്യം Curl_meta_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_len, ശൂന്യം *meta_data);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_set(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *താക്കോൽ,
                       ശൂന്യം *meta_data, Curl_meta_dtor *meta_dtor);
ശൂന്യം Curl_meta_remove(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *താക്കോൽ);
ശൂന്യം *Curl_meta_get(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *താക്കോൽ);
ശൂന്യം Curl_meta_reset(ഘടന Curl_easy *data);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_set(ഘടന connectdata *conn, സ്ഥിരം അക്ഷരം *താക്കോൽ,
                            ശൂന്യം *meta_data, Curl_meta_dtor *meta_dtor);
ശൂന്യം Curl_conn_meta_remove(ഘടന connectdata *conn, സ്ഥിരം അക്ഷരം *താക്കോൽ);
ശൂന്യം *Curl_conn_meta_get(ഘടന connectdata *conn, സ്ഥിരം അക്ഷരം *താക്കോൽ);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
ഘടന Curl_easy *Curl_get_admin(ഘടന Curl_easy *data);

#define CURL_DEFAULT_PROXY_PORT 1080 /* default proxy port unless specified */
#define CURL_DEFAULT_HTTPS_PROXY_PORT 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#എങ്കിൽ defined(USE_HTTP2) || defined(USE_HTTP3)
ശൂന്യം Curl_data_priority_clear_state(ഘടന Curl_easy *data);
#അല്ലെങ്കിൽ
#define Curl_data_priority_clear_state(x)
#endif /* USE_HTTP2 || USE_HTTP3 */

#ifdef USE_NGHTTP2
CURLcode Curl_data_priority_add_child(ഘടന Curl_easy *parent,
                                      ഘടന Curl_easy *child,
                                      bool exclusive);
#അല്ലെങ്കിൽ
#define Curl_data_priority_add_child(x, y, z) CURLE_NOT_BUILT_IN
#endif

#endif /* HEADER_CURL_URL_H */
