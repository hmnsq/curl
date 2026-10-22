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
#অন্তর্ভাৱ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP
#অন্তর্ভাৱ "bufq.h"
#অন্তর্ভাৱ "http.h"

#define H1_PARSE_OPT_NONE       0
#define H1_PARSE_OPT_STRICT     (1 << 0)

#define H1_PARSE_DEFAULT_MAX_সাৰি_LEN   DYN_HTTP_REQUEST

সংরচনা h1_req_parser {
  সংরচনা httpreq *req;
  সংরচনা dynbuf scratch;
  size_t scratch_skip;
  ধ্রুৱ ৱর্ণ *সাৰি;
  size_t max_সাৰি_len;
  size_t সাৰি_len;
  BIT(done);
};

শূন্য Curl_h1_req_parse_init(সংরচনা h1_req_parser *parser, size_t max_সাৰি_len);
শূন্য Curl_h1_req_parse_free(সংরচনা h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(সংরচনা h1_req_parser *parser,
                                ধ্রুৱ uint8_t *buf, size_t buflen,
                                ধ্রুৱ ৱর্ণ *scheme_default,
                                ধ্রুৱ ৱর্ণ *custom_method,
                                পূর্ণাঙ্ক options, size_t *pnread);

CURLcode Curl_h1_req_dprint(ধ্রুৱ সংরচনা httpreq *req,
                            সংরচনা dynbuf *dbuf);

CURLcode Curl_h1_req_write_head(সংরচনা httpreq *req, পূর্ণাঙ্ক http_minor,
                                সংরচনা dynbuf *dbuf);

#endif /* !CURL_DISABLE_HTTP */
#endif /* HEADER_CURL_HTTP1_H */
