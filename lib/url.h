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

CURLcode Curl_init_transfer(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn);
CURLcode Curl_open(কাঠামো Curl_easy **curl);
শূন্য Curl_init_userdefined(কাঠামো Curl_easy *তথ্য);

শূন্য Curl_freeset(কাঠামো Curl_easy *তথ্য);
CURLcode Curl_uc_to_curlcode(CURLUcode uc);
CURLcode Curl_close(কাঠামো Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(কাঠামো Curl_easy *তথ্য, bool *pconnected);
শূন্য Curl_conn_free(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn);
CURLcode Curl_parse_login_details(ধ্রুবক অক্ষর *login, ধ্রুবক size_t len,
                                  অক্ষর **userp, অক্ষর **passwdp,
                                  অক্ষর **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
প্রকারলেখ শূন্য Curl_meta_dtor(ধ্রুবক শূন্য *চাবি, size_t চাবি_len, শূন্য *meta_তথ্য);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_set(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *চাবি,
                       শূন্য *meta_তথ্য, Curl_meta_dtor *meta_dtor);
শূন্য Curl_meta_remove(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *চাবি);
শূন্য *Curl_meta_get(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *চাবি);
শূন্য Curl_meta_reset(কাঠামো Curl_easy *তথ্য);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_set(কাঠামো connectdata *conn, ধ্রুবক অক্ষর *চাবি,
                            শূন্য *meta_তথ্য, Curl_meta_dtor *meta_dtor);
শূন্য Curl_conn_meta_remove(কাঠামো connectdata *conn, ধ্রুবক অক্ষর *চাবি);
শূন্য *Curl_conn_meta_get(কাঠামো connectdata *conn, ধ্রুবক অক্ষর *চাবি);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
কাঠামো Curl_easy *Curl_get_admin(কাঠামো Curl_easy *তথ্য);

#define CURL_DEFAULT_PROXY_PORT 1080 /* default proxy port unless specified */
#define CURL_DEFAULT_HTTPS_PROXY_PORT 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#যদি defined(USE_HTTP2) || defined(USE_HTTP3)
শূন্য Curl_তথ্য_priority_clear_state(কাঠামো Curl_easy *তথ্য);
#অন্যথা
#define Curl_তথ্য_priority_clear_state(x)
#endif /* USE_HTTP2 || USE_HTTP3 */

#ifdef USE_NGHTTP2
CURLcode Curl_তথ্য_priority_add_child(কাঠামো Curl_easy *parent,
                                      কাঠামো Curl_easy *child,
                                      bool exclusive);
#অন্যথা
#define Curl_তথ্য_priority_add_child(x, y, z) CURLE_NOT_BUILT_IN
#endif

#endif /* HEADER_CURL_URL_H */
