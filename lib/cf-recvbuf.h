#اگر_غیر_تعریف_شدہ سرخی_CURL_CF_RECVBUF_H
#تعریف_کرو سرخی_CURL_CF_RECVBUF_H
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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

CURLcode Curl_cf_recvbuf_جوڑو(ساخت Curl_easy *ڈیٹا,
                             ساخت connectdata *conn,
                             int8_t sockindex,
                             ثابت uint8_t *buf, size_t blen);

خارجی ساخت Curl_cftype Curl_cft_recvbuf;

#اگر_ختم /* !CURL_DISABLE_WEBSOCKETS */

#اگر_ختم /* HEADER_CURL_CF_RECVBUF_H */
