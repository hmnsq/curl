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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP
#અન્તર્ભાવ "bufq.h"
#અન્તર્ભાવ "http.h"

#define H1_PARSE_OPT_NONE       0
#define H1_PARSE_OPT_STRICT     (1 << 0)

#define H1_PARSE_DEFAULT_MAX_લીટી_LEN   DYN_HTTP_REQUEST

સંરચના h1_req_parser {
  સંરચના httpreq *req;
  સંરચના dynbuf scratch;
  size_t scratch_skip;
  અચલ અક્ષર *લીટી;
  size_t max_લીટી_len;
  size_t લીટી_len;
  BIT(done);
};

શૂન્ય Curl_h1_req_parse_init(સંરચના h1_req_parser *parser, size_t max_લીટી_len);
શૂન્ય Curl_h1_req_parse_free(સંરચના h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(સંરચના h1_req_parser *parser,
                                અચલ uint8_t *buf, size_t buflen,
                                અચલ અક્ષર *scheme_default,
                                અચલ અક્ષર *custom_method,
                                પૂર્ણાંક options, size_t *pnread);

CURLcode Curl_h1_req_dprint(અચલ સંરચના httpreq *req,
                            સંરચના dynbuf *dbuf);

CURLcode Curl_h1_req_write_head(સંરચના httpreq *req, પૂર્ણાંક http_minor,
                                સંરચના dynbuf *dbuf);

#endif /* !CURL_DISABLE_HTTP */
#endif /* HEADER_CURL_HTTP1_H */
