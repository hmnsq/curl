#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_HTTP1_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_HTTP1_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H1_PARSE_OPT_NONE       0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H1_PARSE_OPT_STRICT     (1 << 0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H1_PARSE_DEFAULT_MAX_ਸਤਰ_LEN   DYN_HTTP_REQUEST

ਬਣਤਰ h1_req_parser {
  ਬਣਤਰ httpreq *req;
  ਬਣਤਰ dynbuf scratch;
  size_t scratch_skip;
  ਅਚਲ ਅੱਖਰ *ਸਤਰ;
  size_t max_ਸਤਰ_len;
  size_t ਸਤਰ_len;
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੂਰਾ);
};

ਖਾਲੀ Curl_h1_req_parse_init(ਬਣਤਰ h1_req_parser *parser, size_t max_ਸਤਰ_len);
ਖਾਲੀ Curl_h1_req_parse_free(ਬਣਤਰ h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(ਬਣਤਰ h1_req_parser *parser,
                                ਅਚਲ uint8_t *buf, size_t buflen,
                                ਅਚਲ ਅੱਖਰ *scheme_default,
                                ਅਚਲ ਅੱਖਰ *custom_ਵਿਧੀ,
                                ਪੂਰਨ options, size_t *pnread);

CURLcode Curl_h1_req_dprint(ਅਚਲ ਬਣਤਰ httpreq *req,
                            ਬਣਤਰ dynbuf *dbuf);

CURLcode Curl_h1_req_write_ਸਿਰ(ਬਣਤਰ httpreq *req, ਪੂਰਨ http_minor,
                                ਬਣਤਰ dynbuf *dbuf);

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */
#ਜੇ_ਅੰਤ /* HEADER_CURL_HTTP1_H */
