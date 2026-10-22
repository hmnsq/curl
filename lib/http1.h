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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP
#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "http.h"

#define H1_PARSE_OPT_NONE       0
#define H1_PARSE_OPT_STRICT     (1 << 0)

#define H1_PARSE_DEFAULT_MAX_வரி_LEN   DYN_HTTP_REQUEST

கட்டமைப்பு h1_req_parser {
  கட்டமைப்பு httpreq *req;
  கட்டமைப்பு dynbuf scratch;
  size_t scratch_skip;
  மாறிலி எழுத்து *வரி;
  size_t max_வரி_len;
  size_t வரி_len;
  BIT(done);
};

வெறுமை Curl_h1_req_parse_init(கட்டமைப்பு h1_req_parser *parser, size_t max_வரி_len);
வெறுமை Curl_h1_req_parse_free(கட்டமைப்பு h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(கட்டமைப்பு h1_req_parser *parser,
                                மாறிலி uint8_t *buf, size_t buflen,
                                மாறிலி எழுத்து *scheme_default,
                                மாறிலி எழுத்து *custom_method,
                                முழு options, size_t *pnread);

CURLcode Curl_h1_req_dprint(மாறிலி கட்டமைப்பு httpreq *req,
                            கட்டமைப்பு dynbuf *dbuf);

CURLcode Curl_h1_req_write_head(கட்டமைப்பு httpreq *req, முழு http_minor,
                                கட்டமைப்பு dynbuf *dbuf);

#endif /* !CURL_DISABLE_HTTP */
#endif /* HEADER_CURL_HTTP1_H */
