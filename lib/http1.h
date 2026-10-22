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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP
#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "http.h"

#define H1_PARSE_OPT_NONE       0
#define H1_PARSE_OPT_STRICT     (1 << 0)

#define H1_PARSE_DEFAULT_MAX_ଧାଡ଼ି_LEN   DYN_HTTP_REQUEST

ସଂରଚନା h1_req_parser {
  ସଂରଚନା httpreq *req;
  ସଂରଚନା dynbuf scratch;
  size_t scratch_skip;
  ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି;
  size_t max_ଧାଡ଼ି_len;
  size_t ଧାଡ଼ି_len;
  BIT(done);
};

ଶୂନ୍ୟ Curl_h1_req_parse_init(ସଂରଚନା h1_req_parser *parser, size_t max_ଧାଡ଼ି_len);
ଶୂନ୍ୟ Curl_h1_req_parse_free(ସଂରଚନା h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(ସଂରଚନା h1_req_parser *parser,
                                ଅଚଳ uint8_t *buf, size_t buflen,
                                ଅଚଳ ଅକ୍ଷର *scheme_default,
                                ଅଚଳ ଅକ୍ଷର *custom_method,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା options, size_t *pnread);

CURLcode Curl_h1_req_dprint(ଅଚଳ ସଂରଚନା httpreq *req,
                            ସଂରଚନା dynbuf *dbuf);

CURLcode Curl_h1_req_write_head(ସଂରଚନା httpreq *req, ପୂର୍ଣ୍ଣସଂଖ୍ୟା http_minor,
                                ସଂରଚନା dynbuf *dbuf);

#endif /* !CURL_DISABLE_HTTP */
#endif /* HEADER_CURL_HTTP1_H */
