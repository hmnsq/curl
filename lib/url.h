#यदि_अपरिभाषितम् शीर्षकम्_CURL_URL_H
#परिभाषय शीर्षकम्_CURL_URL_H
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
#अन्तर्भाव "curl_setup.h"

/* Reject URLs exceeding this length */
#परिभाषय MAX_URL_LEN 0xffff

/*
 * Prototypes for library-wide functions
 */

CURLcode Curl_init_transfer(संरचना Curl_easy *दत्त, संरचना connectdata *conn);
CURLcode Curl_open(संरचना Curl_easy **curl);
शून्य Curl_init_userdefined(संरचना Curl_easy *दत्त);

शून्य Curl_freeset(संरचना Curl_easy *दत्त);
CURLcode Curl_uc_गन्तव्यम्_curlcode(CURLUcode uc);
CURLcode Curl_close(संरचना Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(संरचना Curl_easy *दत्त, bool *pconnected);
शून्य Curl_conn_free(संरचना Curl_easy *दत्त, संरचना connectdata *conn);
CURLcode Curl_parse_login_details(ध्रुव वर्ण *login, ध्रुव size_t len,
                                  वर्ण **userp, वर्ण **passwdp,
                                  वर्ण **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
प्रकारनिर्देश शून्य Curl_meta_dtor(ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_len, शून्य *meta_दत्त);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_समुच्चयः(संरचना Curl_easy *दत्त, ध्रुव वर्ण *कुञ्चिका,
                       शून्य *meta_दत्त, Curl_meta_dtor *meta_dtor);
शून्य Curl_meta_remove(संरचना Curl_easy *दत्त, ध्रुव वर्ण *कुञ्चिका);
शून्य *Curl_meta_get(संरचना Curl_easy *दत्त, ध्रुव वर्ण *कुञ्चिका);
शून्य Curl_meta_reset(संरचना Curl_easy *दत्त);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_समुच्चयः(संरचना connectdata *conn, ध्रुव वर्ण *कुञ्चिका,
                            शून्य *meta_दत्त, Curl_meta_dtor *meta_dtor);
शून्य Curl_conn_meta_remove(संरचना connectdata *conn, ध्रुव वर्ण *कुञ्चिका);
शून्य *Curl_conn_meta_get(संरचना connectdata *conn, ध्रुव वर्ण *कुञ्चिका);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
संरचना Curl_easy *Curl_get_admin(संरचना Curl_easy *दत्त);

#परिभाषय CURL_DEFAULT_PROXY_पत्तनम् 1080 /* default proxy port unless specified */
#परिभाषय CURL_DEFAULT_HTTPS_PROXY_पत्तनम् 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#यदि defined(USE_HTTP2) || defined(USE_HTTP3)
शून्य Curl_दत्त_प्राथमिकता_clear_state(संरचना Curl_easy *दत्त);
#अन्यथा
#परिभाषय Curl_दत्त_प्राथमिकता_clear_state(x)
#यदि_अन्तः /* USE_HTTP2 || USE_HTTP3 */

#यदि_परिभाषितम् USE_NGHTTP2
CURLcode Curl_दत्त_प्राथमिकता_सङ्कलय_child(संरचना Curl_easy *parent,
                                      संरचना Curl_easy *child,
                                      bool exclusive);
#अन्यथा
#परिभाषय Curl_दत्त_प्राथमिकता_सङ्कलय_child(x, y, z) CURLE_NOT_BUILT_अन्तर्
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_URL_H */
