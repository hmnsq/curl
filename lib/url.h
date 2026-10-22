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
#அந்தர்பாவ "curl_setup.h"

/* Reject URLs exceeding this length */
#define MAX_URL_LEN 0xffff

/*
 * Prototypes for library-wide functions
 */

CURLcode Curl_init_transfer(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn);
CURLcode Curl_open(கட்டமைப்பு Curl_easy **curl);
வெறுமை Curl_init_userdefined(கட்டமைப்பு Curl_easy *data);

வெறுமை Curl_freeset(கட்டமைப்பு Curl_easy *data);
CURLcode Curl_uc_to_curlcode(CURLUcode uc);
CURLcode Curl_close(கட்டமைப்பு Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(கட்டமைப்பு Curl_easy *data, bool *pconnected);
வெறுமை Curl_conn_free(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn);
CURLcode Curl_parse_login_details(மாறிலி எழுத்து *login, மாறிலி size_t len,
                                  எழுத்து **userp, எழுத்து **passwdp,
                                  எழுத்து **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
வகைவரை வெறுமை Curl_meta_dtor(மாறிலி வெறுமை *சாவி, size_t சாவி_len, வெறுமை *meta_data);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_set(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *சாவி,
                       வெறுமை *meta_data, Curl_meta_dtor *meta_dtor);
வெறுமை Curl_meta_remove(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *சாவி);
வெறுமை *Curl_meta_get(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *சாவி);
வெறுமை Curl_meta_reset(கட்டமைப்பு Curl_easy *data);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_set(கட்டமைப்பு connectdata *conn, மாறிலி எழுத்து *சாவி,
                            வெறுமை *meta_data, Curl_meta_dtor *meta_dtor);
வெறுமை Curl_conn_meta_remove(கட்டமைப்பு connectdata *conn, மாறிலி எழுத்து *சாவி);
வெறுமை *Curl_conn_meta_get(கட்டமைப்பு connectdata *conn, மாறிலி எழுத்து *சாவி);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
கட்டமைப்பு Curl_easy *Curl_get_admin(கட்டமைப்பு Curl_easy *data);

#define CURL_DEFAULT_PROXY_PORT 1080 /* default proxy port unless specified */
#define CURL_DEFAULT_HTTPS_PROXY_PORT 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#எனில் defined(USE_HTTP2) || defined(USE_HTTP3)
வெறுமை Curl_data_priority_clear_state(கட்டமைப்பு Curl_easy *data);
#இல்லையெனில்
#define Curl_data_priority_clear_state(x)
#endif /* USE_HTTP2 || USE_HTTP3 */

#ifdef USE_NGHTTP2
CURLcode Curl_data_priority_add_child(கட்டமைப்பு Curl_easy *parent,
                                      கட்டமைப்பு Curl_easy *child,
                                      bool exclusive);
#இல்லையெனில்
#define Curl_data_priority_add_child(x, y, z) CURLE_NOT_BUILT_IN
#endif

#endif /* HEADER_CURL_URL_H */
