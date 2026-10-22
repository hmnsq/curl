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
#अन्तर्भाव "curl_setup.h"

संरचना Curl_cwriter;

/* returns an allocated string or NULL */
अक्षर *Curl_get_content_encodings(शून्य);

CURLcode Curl_build_unencoding_चळत(संरचना Curl_easy *data,
                                     स्थिरांक अक्षर *enclist, पूर्णांक is_transfer);
#endif /* HEADER_CURL_CONTENT_ENCODING_H */
