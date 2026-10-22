#ifndef HEADER_CURL_CONTENT_ENCODING_H
#define HEADER_CURL_CONTENT_ENCODING_H
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

ಸಂರಚನಾ Curl_cwriter;

/* returns an allocated string or NULL */
ವರ್ಣ *Curl_get_content_encodings(ಶೂನ್ಯ);

CURLcode Curl_build_unencoding_ರಾಶಿ(ಸಂರಚನಾ Curl_easy *data,
                                     ಧ್ರುವ ವರ್ಣ *enclist, ಪೂರ್ಣಾಙ್ಕ is_transfer);
#endif /* HEADER_CURL_CONTENT_ENCODING_H */
