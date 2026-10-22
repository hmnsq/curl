#यदि_अपरिभाषितम् शीर्षकम्_CURL_HTTP1_H
#परिभाषय शीर्षकम्_CURL_HTTP1_H
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
#अन्तर्भाव "curl_setup.h"

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
#अन्तर्भाव "bufq.h"
#अन्तर्भाव "http.h"

#परिभाषय H1_PARSE_OPT_NONE       0
#परिभाषय H1_PARSE_OPT_STRICT     (1 << 0)

#परिभाषय H1_PARSE_DEFAULT_MAX_पङ्क्तिः_LEN   DYN_HTTP_REQUEST

संरचना h1_req_parser {
  संरचना httpreq *req;
  संरचना dynbuf scratch;
  size_t scratch_skip;
  ध्रुव वर्ण *पङ्क्तिः;
  size_t max_पङ्क्तिः_len;
  size_t पङ्क्तिः_len;
  BIT(समाप्तम्);
};

शून्य Curl_h1_req_parse_init(संरचना h1_req_parser *parser, size_t max_पङ्क्तिः_len);
शून्य Curl_h1_req_parse_free(संरचना h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(संरचना h1_req_parser *parser,
                                ध्रुव uint8_t *buf, size_t buflen,
                                ध्रुव वर्ण *scheme_default,
                                ध्रुव वर्ण *custom_विधानम्,
                                पूर्णाङ्क options, size_t *pnread);

CURLcode Curl_h1_req_dprint(ध्रुव संरचना httpreq *req,
                            संरचना dynbuf *dbuf);

CURLcode Curl_h1_req_write_शीर्ष(संरचना httpreq *req, पूर्णाङ्क http_minor,
                                संरचना dynbuf *dbuf);

#यदि_अन्तः /* !CURL_DISABLE_HTTP */
#यदि_अन्तः /* HEADER_CURL_HTTP1_H */
