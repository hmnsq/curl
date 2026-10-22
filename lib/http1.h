#جے_بنا_تعریف سرخی_CURL_HTTP1_H
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
#ضمن "curl_setup.h"

#جے_بنا_تعریف CURL_DISABLE_HTTP
#ضمن "bufq.h"
#ضمن "http.h"

#تعریف_کرو H1_PARSE_OPT_NONE       0
#تعریف_کرو H1_PARSE_OPT_STRICT     (1 << 0)

#تعریف_کرو H1_PARSE_DEFAULT_MAX_سطر_LEN   DYN_HTTP_REQUEST

بنية h1_req_parser {
  بنية httpreq *req;
  بنية dynbuf scratch;
  size_t scratch_skip;
  ثابت حرف *سطر;
  size_t max_سطر_len;
  size_t سطر_len;
  ثنائی_ہندسہ(پورا);
};

فراغ Curl_h1_req_parse_init(بنية h1_req_parser *parser, size_t max_سطر_len);
فراغ Curl_h1_req_parse_free(بنية h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(بنية h1_req_parser *parser,
                                ثابت uint8_t *buf, size_t buflen,
                                ثابت حرف *scheme_default,
                                ثابت حرف *custom_طریقہ,
                                صحيح options, size_t *pnread);

CURLcode Curl_h1_req_dprint(ثابت بنية httpreq *req,
                            بنية dynbuf *dbuf);

CURLcode Curl_h1_req_write_سر(بنية httpreq *req, صحيح http_minor,
                                بنية dynbuf *dbuf);

#جے_انت /* !CURL_DISABLE_HTTP */
#جے_انت /* HEADER_CURL_HTTP1_H */
