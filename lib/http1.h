#నిర్వచితం_కాకపోతే శీర్షిక_CURL_HTTP1_H
#నిర్వచించు శీర్షిక_CURL_HTTP1_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
#అన్తర్భావ "bufq.h"
#అన్తర్భావ "http.h"

#నిర్వచించు H1_PARSE_OPT_NONE       0
#నిర్వచించు H1_PARSE_OPT_STRICT     (1 << 0)

#నిర్వచించు H1_PARSE_DEFAULT_MAX_పంక్తి_LEN   DYN_HTTP_REQUEST

నిర్మాణం h1_req_parser {
  నిర్మాణం httpreq *req;
  నిర్మాణం dynbuf scratch;
  size_t scratch_skip;
  స్థిరరాశి అక్షరం *పంక్తి;
  size_t max_పంక్తి_len;
  size_t పంక్తి_len;
  బిట్టు(పూర్తి);
};

శూన్య Curl_h1_req_parse_init(నిర్మాణం h1_req_parser *parser, size_t max_పంక్తి_len);
శూన్య Curl_h1_req_parse_free(నిర్మాణం h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(నిర్మాణం h1_req_parser *parser,
                                స్థిరరాశి uint8_t *buf, size_t buflen,
                                స్థిరరాశి అక్షరం *scheme_default,
                                స్థిరరాశి అక్షరం *custom_పద్ధతి,
                                పూర్ణాంకం options, size_t *pnread);

CURLcode Curl_h1_req_dprint(స్థిరరాశి నిర్మాణం httpreq *req,
                            నిర్మాణం dynbuf *dbuf);

CURLcode Curl_h1_req_write_తల(నిర్మాణం httpreq *req, పూర్ణాంకం http_minor,
                                నిర్మాణం dynbuf *dbuf);

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */
#అయితే_ముగింపు /* HEADER_CURL_HTTP1_H */
