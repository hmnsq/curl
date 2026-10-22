#اگر_غیر_تعریف_شدہ سرخی_CURL_HTTP1_H
#تعریف_کرو سرخی_CURL_HTTP1_H
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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
#شامل "bufq.h"
#شامل "http.h"

#تعریف_کرو H1_PARSE_OPT_NONE       0
#تعریف_کرو H1_PARSE_OPT_STRICT     (1 << 0)

#تعریف_کرو H1_PARSE_DEFAULT_MAX_سطر_LEN   DYN_HTTP_REQUEST

ساخت h1_req_parser {
  ساخت httpreq *req;
  ساخت dynbuf scratch;
  size_t scratch_skip;
  ثابت حرف *سطر;
  size_t max_سطر_len;
  size_t سطر_len;
  ثنائی_ہندسہ(مکمل);
};

خالی Curl_h1_req_parse_init(ساخت h1_req_parser *parser, size_t max_سطر_len);
خالی Curl_h1_req_parse_free(ساخت h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(ساخت h1_req_parser *parser,
                                ثابت uint8_t *buf, size_t buflen,
                                ثابت حرف *scheme_default,
                                ثابت حرف *custom_طریقہ,
                                صحیح options, size_t *pnread);

CURLcode Curl_h1_req_dprint(ثابت ساخت httpreq *req,
                            ساخت dynbuf *dbuf);

CURLcode Curl_h1_req_write_سر(ساخت httpreq *req, صحیح http_minor,
                                ساخت dynbuf *dbuf);

#اگر_ختم /* !CURL_DISABLE_HTTP */
#اگر_ختم /* HEADER_CURL_HTTP1_H */
