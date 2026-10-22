#ifndef HEADER_CURL_HTTP1_H
#define HEADER_CURL_HTTP1_H
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

#ifndef CURL_DISABLE_HTTP
#شامل "bufq.h"
#شامل "http.h"

#define H1_PARSE_OPT_NONE       0
#define H1_PARSE_OPT_STRICT     (1 << 0)

#define H1_PARSE_DEFAULT_MAX_سطر_LEN   DYN_HTTP_REQUEST

ساختار h1_req_parser {
  ساختار httpreq *req;
  ساختار dynbuf scratch;
  size_t scratch_skip;
  ثابت نویسه *سطر;
  size_t max_سطر_len;
  size_t سطر_len;
  BIT(done);
};

تهی Curl_h1_req_parse_init(ساختار h1_req_parser *parser, size_t max_سطر_len);
تهی Curl_h1_req_parse_free(ساختار h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(ساختار h1_req_parser *parser,
                                ثابت uint8_t *buf, size_t buflen,
                                ثابت نویسه *scheme_default,
                                ثابت نویسه *custom_method,
                                صحیح options, size_t *pnread);

CURLcode Curl_h1_req_dprint(ثابت ساختار httpreq *req,
                            ساختار dynbuf *dbuf);

CURLcode Curl_h1_req_write_head(ساختار httpreq *req, صحیح http_minor,
                                ساختار dynbuf *dbuf);

#endif /* !CURL_DISABLE_HTTP */
#endif /* HEADER_CURL_HTTP1_H */
