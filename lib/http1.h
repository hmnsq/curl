#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_HTTP1_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_HTTP1_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "http.h"

#ವ್ಯಾಖ್ಯಾನಿಸು H1_PARSE_OPT_NONE       0
#ವ್ಯಾಖ್ಯಾನಿಸು H1_PARSE_OPT_STRICT     (1 << 0)

#ವ್ಯಾಖ್ಯಾನಿಸು H1_PARSE_DEFAULT_MAX_ಸಾಲು_LEN   DYN_HTTP_REQUEST

ರಚನೆ h1_req_parser {
  ರಚನೆ httpreq *req;
  ರಚನೆ dynbuf scratch;
  size_t scratch_skip;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು;
  size_t max_ಸಾಲು_len;
  size_t ಸಾಲು_len;
  ಬಿಟ್(ಮುಗಿದ);
};

ಶೂನ್ಯ Curl_h1_req_parse_init(ರಚನೆ h1_req_parser *parser, size_t max_ಸಾಲು_len);
ಶೂನ್ಯ Curl_h1_req_parse_free(ರಚನೆ h1_req_parser *parser);

CURLcode Curl_h1_req_parse_read(ರಚನೆ h1_req_parser *parser,
                                ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t buflen,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme_default,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *custom_ವಿಧಾನ,
                                ಪೂರ್ಣಾಂಕ options, size_t *pnread);

CURLcode Curl_h1_req_dprint(ಸ್ಥಿರಾಂಕ ರಚನೆ httpreq *req,
                            ರಚನೆ dynbuf *dbuf);

CURLcode Curl_h1_req_write_ತಲೆ(ರಚನೆ httpreq *req, ಪೂರ್ಣಾಂಕ http_minor,
                                ರಚನೆ dynbuf *dbuf);

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_HTTP1_H */
