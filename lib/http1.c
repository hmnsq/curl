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

#ifndef CURL_DISABLE_HTTP

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "http1.h"
#ಅನ್ತರ್ಭಾವ "urlapi-int.h"


#define H1_MAX_URL_LEN (8 * 1024)

ಶೂನ್ಯ Curl_h1_req_parse_init(ಸಂರಚನಾ h1_req_parser *parser, size_t max_ಸಾಲು_len)
{
  memset(parser, 0, ಪರಿಮಾಣ(*parser));
  parser->max_ಸಾಲು_len = max_ಸಾಲು_len;
  curlx_dyn_init(&parser->scratch, max_ಸಾಲು_len);
}

ಶೂನ್ಯ Curl_h1_req_parse_free(ಸಂರಚನಾ h1_req_parser *parser)
{
  ಯದಿ(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = ರಿಕ್ತ;
    parser->done = FALSE;
  }
}

ಸ್ಥಿರ CURLcode trim_ಸಾಲು(ಸಂರಚನಾ h1_req_parser *parser, ಪೂರ್ಣಾಙ್ಕ options)
{
  DEBUGASSERT(parser->ಸಾಲು);
  ಯದಿ(parser->ಸಾಲು_len) {
    ಯದಿ(parser->ಸಾಲು[parser->ಸಾಲು_len - 1] == '\n')
      --parser->ಸಾಲು_len;
    ಯದಿ(parser->ಸಾಲು_len) {
      ಯದಿ(parser->ಸಾಲು[parser->ಸಾಲು_len - 1] == '\r')
        --parser->ಸಾಲು_len;
      ಅನ್ಯಥಾ ಯದಿ(options & H1_PARSE_OPT_STRICT)
        ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
    }
    ಅನ್ಯಥಾ ಯದಿ(options & H1_PARSE_OPT_STRICT)
      ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
  }
  ಅನ್ಯಥಾ ಯದಿ(options & H1_PARSE_OPT_STRICT)
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;

  ಯದಿ(parser->ಸಾಲು_len > parser->max_ಸಾಲು_len) {
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode detect_ಸಾಲು(ಸಂರಚನಾ h1_req_parser *parser,
                            ಧ್ರುವ uint8_t *buf, ಧ್ರುವ size_t buflen,
                            size_t *pnread)
{
  ಧ್ರುವ ವರ್ಣ *ಸಾಲು_end;

  DEBUGASSERT(!parser->ಸಾಲು);
  *pnread = 0;
  ಸಾಲು_end = memchr(buf, '\n', buflen);
  ಯದಿ(!ಸಾಲು_end)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  parser->ಸಾಲು = (ಧ್ರುವ ವರ್ಣ *)buf;
  parser->ಸಾಲು_len = ಸಾಲು_end - parser->ಸಾಲು + 1;
  *pnread = parser->ಸಾಲು_len;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode next_ಸಾಲು(ಸಂರಚನಾ h1_req_parser *parser,
                          ಧ್ರುವ uint8_t *buf, ಧ್ರುವ size_t buflen, ಪೂರ್ಣಾಙ್ಕ options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  ಯದಿ(parser->ಸಾಲು) {
    parser->ಸಾಲು = ರಿಕ್ತ;
    parser->ಸಾಲು_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_ಸಾಲು(parser, buf, buflen, pnread);
  ಯದಿ(!result) {
    ಯದಿ(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->ಸಾಲು,
                              parser->ಸಾಲು_len);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      parser->ಸಾಲು = curlx_dyn_ptr(&parser->scratch);
      parser->ಸಾಲು_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_ಸಾಲು(parser, options);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ ಯದಿ(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (ಧ್ರುವ unsigned ವರ್ಣ *)buf,
                            buflen);
    *pnread = buflen;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode start_req(ಸಂರಚನಾ h1_req_parser *parser,
                          ಧ್ರುವ ವರ್ಣ *scheme_default,
                          ಧ್ರುವ ವರ್ಣ *custom_method,
                          ಪೂರ್ಣಾಙ್ಕ options)
{
  ಧ್ರುವ ವರ್ಣ *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = ರಿಕ್ತ;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  ಯದಿ(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->ಸಾಲು, strlen(custom_method))) {
    p = parser->ಸಾಲು + strlen(custom_method);
  }
  ಅನ್ಯಥಾ {
    p = memchr(parser->ಸಾಲು, ' ', parser->ಸಾಲು_len);
    ಯದಿ(!p || p == parser->ಸಾಲು)
      ಗಚ್ಛ out;
  }

  m = parser->ಸಾಲು;
  m_len = p - parser->ಸಾಲು;
  target = p + 1;
  target_len = hv_len = 0;
  hv = ರಿಕ್ತ;

  /* URL may contain spaces so scan backwards */
  ಪ್ರತಿ(i = parser->ಸಾಲು_len; i > m_len; --i) {
    ಯದಿ(parser->ಸಾಲು[i] == ' ') {
      hv = &parser->ಸಾಲು[i + 1];
      hv_len = parser->ಸಾಲು_len - i;
      target_len = (hv - target) - 1;
      ಭಙ್ಗ;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  ಯದಿ(!target_len || !hv_len)
    ಗಚ್ಛ out;

  (ಶೂನ್ಯ)hv;

  /* The TARGET can be (rfc 9112, ch. 3.2):
   * origin-form:     path + optional query
   * absolute-form:   absolute URI
   * authority-form:  host+port for CONNECT
   * asterisk-form:   '*' for OPTIONS
   *
   * from TARGET, we derive `scheme` `authority` `path`
   * origin-form            --        --          TARGET
   * absolute-form          URL*      URL*        URL*
   * authority-form         --        TARGET      --
   * asterisk-form          --        --          TARGET
   */
  scheme = authority = path = ರಿಕ್ತ;
  scheme_len = authority_len = path_len = 0;

  ಯದಿ(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  ಅನ್ಯಥಾ ಯದಿ(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  ಅನ್ಯಥಾ {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    ವರ್ಣ tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    ಯದಿ(target_len >= ಪರಿಮಾಣ(tmp))
      ಗಚ್ಛ out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    ಯದಿ(Curl_is_absolute_url(tmp, ರಿಕ್ತ, 0, FALSE)) {
      unsigned ಪೂರ್ಣಾಙ್ಕ url_options;

      url = curl_url();
      ಯದಿ(!url) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      ಯದಿ(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      ಯದಿ(uc) {
        ಗಚ್ಛ out;
      }
    }

    ಯದಿ(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      ಗಚ್ಛ out;
    }
  }

  ಯದಿ(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = strlen(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_h1_req_parse_read(ಸಂರಚನಾ h1_req_parser *parser,
                                ಧ್ರುವ uint8_t *buf, size_t buflen,
                                ಧ್ರುವ ವರ್ಣ *scheme_default,
                                ಧ್ರುವ ವರ್ಣ *custom_method,
                                ಪೂರ್ಣಾಙ್ಕ options, size_t *pnread)
{
  CURLcode result = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  ಯದಿ(!buf)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಯಾವತ್(!parser->done) {
    result = next_ಸಾಲು(parser, buf, buflen, options, &nread);
    ಯದಿ(result) {
      ಯದಿ(result == CURLE_AGAIN)
        result = CURLE_OK;
      ಗಚ್ಛ out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    ಯದಿ(!parser->ಸಾಲು) {
      /* consumed bytes, but line not complete */
      ಯದಿ(!buflen)
        ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(parser->ಸಾಲು_len == 0) {
      /* last, empty line, we are finished */
      ಯದಿ(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        ಗಚ್ಛ out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    ಅನ್ಯಥಾ {
      result = Curl_dynhds_h1_add_ಸಾಲು(&parser->req->headers,
                                       parser->ಸಾಲು, parser->ಸಾಲು_len);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
  }

out:
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_h1_req_write_head(ಸಂರಚನಾ httpreq *req, ಪೂರ್ಣಾಙ್ಕ http_minor,
                                ಸಂರಚನಾ dynbuf *dbuf)
{
  CURLcode result;

  result = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_HTTP */
