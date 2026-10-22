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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP
#അന്തര്ഭാവ "bufq.h"
#അന്തര്ഭാവ "http.h"

#define H1_PARSE_OPT_NONE       0
#define H1_PARSE_OPT_STRICT     (1 << 0)

#define H1_PARSE_DEFAULT_MAX_വരി_LEN   DYN_HTTP_REQUEST

ഘടന h1_req_parser {
  ഘടന httpreq *req;
  ഘടന dynbuf scratch;
  size_t scratch_skip;
  സ്ഥിരം അക്ഷരം *വരി;
  size_t max_വരി_len;
  size_t വരി_len;
  BIT(done);
};

ശൂന്യം Curl_h1_req_parse_init(ഘടന h1_req_parser *parser, size_t max_വരി_len);
ശൂന്യം Curl_h1_req_parse_free(ഘടന h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(ഘടന h1_req_parser *parser,
                                സ്ഥിരം uint8_t *buf, size_t buflen,
                                സ്ഥിരം അക്ഷരം *scheme_default,
                                സ്ഥിരം അക്ഷരം *custom_method,
                                പൂർണ്ണസംഖ്യ options, size_t *pnread);

CURLcode Curl_h1_req_dprint(സ്ഥിരം ഘടന httpreq *req,
                            ഘടന dynbuf *dbuf);

CURLcode Curl_h1_req_write_head(ഘടന httpreq *req, പൂർണ്ണസംഖ്യ http_minor,
                                ഘടന dynbuf *dbuf);

#endif /* !CURL_DISABLE_HTTP */
#endif /* HEADER_CURL_HTTP1_H */
