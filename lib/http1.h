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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_HTTP
#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "http.h"

#define H1_PARSE_OPT_NONE       0
#define H1_PARSE_OPT_STRICT     (1 << 0)

#define H1_PARSE_DEFAULT_MAX_সারি_LEN   DYN_HTTP_REQUEST

কাঠামো h1_req_parser {
  কাঠামো httpreq *req;
  কাঠামো dynbuf scratch;
  size_t scratch_skip;
  ধ্রুবক অক্ষর *সারি;
  size_t max_সারি_len;
  size_t সারি_len;
  BIT(done);
};

শূন্য Curl_h1_req_parse_init(কাঠামো h1_req_parser *parser, size_t max_সারি_len);
শূন্য Curl_h1_req_parse_free(কাঠামো h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(কাঠামো h1_req_parser *parser,
                                ধ্রুবক uint8_t *buf, size_t buflen,
                                ধ্রুবক অক্ষর *scheme_default,
                                ধ্রুবক অক্ষর *custom_method,
                                পূর্ণসংখ্যা options, size_t *pnread);

CURLcode Curl_h1_req_dprint(ধ্রুবক কাঠামো httpreq *req,
                            কাঠামো dynbuf *dbuf);

CURLcode Curl_h1_req_write_শীর্ষ(কাঠামো httpreq *req, পূর্ণসংখ্যা http_minor,
                                কাঠামো dynbuf *dbuf);

#endif /* !CURL_DISABLE_HTTP */
#endif /* HEADER_CURL_HTTP1_H */
