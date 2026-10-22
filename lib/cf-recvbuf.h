#अगर_अघोषित HEADER_CURL_CF_RECVBUF_H
#घोषणा HEADER_CURL_CF_RECVBUF_H
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

#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

CURLcode Curl_cf_recvbuf_add(काष्ठा Curl_easy *तथ्य,
                             काष्ठा connectdata *conn,
                             int8_t sockindex,
                             स्थिर uint8_t *buf, माप_प्रकार blen);

बाह्य काष्ठा Curl_cftype Curl_cft_recvbuf;

#पूर्ण_अगर /* !CURL_DISABLE_WEBSOCKETS */

#पूर्ण_अगर /* HEADER_CURL_CF_RECVBUF_H */
