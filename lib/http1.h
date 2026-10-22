#अगर_अघोषित HEADER_CURL_HTTP1_H
#घोषणा HEADER_CURL_HTTP1_H
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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_HTTP
#समावेश "bufq.h"
#समावेश "http.h"

#घोषणा H1_PARSE_OPT_NONE       0
#घोषणा H1_PARSE_OPT_STRICT     (1 << 0)

#घोषणा H1_PARSE_DEFAULT_MAX_पंक्ति_LEN   DYN_HTTP_REQUEST

काष्ठा h1_req_parser {
  काष्ठा httpreq *req;
  काष्ठा dynbuf scratch;
  माप_प्रकार scratch_skip;
  स्थिर अक्षर *पंक्ति;
  माप_प्रकार max_पंक्ति_len;
  माप_प्रकार पंक्ति_len;
  BIT(done);
};

व्योम Curl_h1_req_parse_init(काष्ठा h1_req_parser *parser, माप_प्रकार max_पंक्ति_len);
व्योम Curl_h1_req_parse_free(काष्ठा h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(काष्ठा h1_req_parser *parser,
                                स्थिर uint8_t *buf, माप_प्रकार buflen,
                                स्थिर अक्षर *scheme_default,
                                स्थिर अक्षर *custom_method,
                                पूर्णांक options, माप_प्रकार *pnread);

CURLcode Curl_h1_req_dprint(स्थिर काष्ठा httpreq *req,
                            काष्ठा dynbuf *dbuf);

CURLcode Curl_h1_req_write_शीर्ष(काष्ठा httpreq *req, पूर्णांक http_minor,
                                काष्ठा dynbuf *dbuf);

#पूर्ण_अगर /* !CURL_DISABLE_HTTP */
#पूर्ण_अगर /* HEADER_CURL_HTTP1_H */
