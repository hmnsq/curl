#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_URL_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_URL_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

/* Reject URLs exceeding this length */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_URL_LEN 0xffff

/*
 * Prototypes for library-wide functions
 */

CURLcode Curl_init_transfer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn);
CURLcode Curl_open(ರಚನೆ Curl_easy **curl);
ಶೂನ್ಯ Curl_init_userdefined(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಶೂನ್ಯ Curl_freeset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
CURLcode Curl_uc_ಗಮ್ಯ_curlcode(CURLUcode uc);
CURLcode Curl_close(ರಚನೆ Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *pconnected);
ಶೂನ್ಯ Curl_conn_free(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn);
CURLcode Curl_parse_login_details(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *login, ಸ್ಥಿರಾಂಕ size_t len,
                                  ಅಕ್ಷರ **userp, ಅಕ್ಷರ **passwdp,
                                  ಅಕ್ಷರ **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ Curl_meta_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *meta_ದತ್ತಾಂಶ);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_ಗಣ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ,
                       ಶೂನ್ಯ *meta_ದತ್ತಾಂಶ, Curl_meta_dtor *meta_dtor);
ಶೂನ್ಯ Curl_meta_remove(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ);
ಶೂನ್ಯ *Curl_meta_get(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ);
ಶೂನ್ಯ Curl_meta_reset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_ಗಣ(ರಚನೆ connectdata *conn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ,
                            ಶೂನ್ಯ *meta_ದತ್ತಾಂಶ, Curl_meta_dtor *meta_dtor);
ಶೂನ್ಯ Curl_conn_meta_remove(ರಚನೆ connectdata *conn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ);
ಶೂನ್ಯ *Curl_conn_meta_get(ರಚನೆ connectdata *conn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
ರಚನೆ Curl_easy *Curl_get_admin(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DEFAULT_PROXY_ಪೋರ್ಟ್ 1080 /* default proxy port unless specified */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DEFAULT_HTTPS_PROXY_ಪೋರ್ಟ್ 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#ಆದರೆ defined(USE_HTTP2) || defined(USE_HTTP3)
ಶೂನ್ಯ Curl_ದತ್ತಾಂಶ_ಆದ್ಯತೆ_clear_state(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ದತ್ತಾಂಶ_ಆದ್ಯತೆ_clear_state(x)
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTP2 || USE_HTTP3 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2
CURLcode Curl_ದತ್ತಾಂಶ_ಆದ್ಯತೆ_ಕೂಡಿಸು_ಮಗು(ರಚನೆ Curl_easy *ಪೋಷಕ,
                                      ರಚನೆ Curl_easy *ಮಗು,
                                      bool exclusive);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ದತ್ತಾಂಶ_ಆದ್ಯತೆ_ಕೂಡಿಸು_ಮಗು(x, y, z) CURLE_NOT_BUILT_ಒಳ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_URL_H */
