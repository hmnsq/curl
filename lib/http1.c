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

#ifndef CURL_DISABLE_HTTP

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "http1.h"
#अन्तर्भाव "urlapi-int.h"


#define H1_MAX_URL_LEN (8 * 1024)

शून्य Curl_h1_req_parse_init(संरचना h1_req_parser *parser, size_t max_पङ्क्तिः_len)
{
  memset(parser, 0, परिमाण(*parser));
  parser->max_पङ्क्तिः_len = max_पङ्क्तिः_len;
  curlx_dyn_init(&parser->scratch, max_पङ्क्तिः_len);
}

शून्य Curl_h1_req_parse_free(संरचना h1_req_parser *parser)
{
  जइ(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = रिक्त;
    parser->done = FALSE;
  }
}

स्थिर CURLcode trim_पङ्क्तिः(संरचना h1_req_parser *parser, पूर्णाङ्क options)
{
  DEBUGASSERT(parser->पङ्क्तिः);
  जइ(parser->पङ्क्तिः_len) {
    जइ(parser->पङ्क्तिः[parser->पङ्क्तिः_len - 1] == '\n')
      --parser->पङ्क्तिः_len;
    जइ(parser->पङ्क्तिः_len) {
      जइ(parser->पङ्क्तिः[parser->पङ्क्तिः_len - 1] == '\r')
        --parser->पङ्क्तिः_len;
      अण्णहा जइ(options & H1_PARSE_OPT_STRICT)
        प्रत्यागम CURLE_URL_MALFORMAT;
    }
    अण्णहा जइ(options & H1_PARSE_OPT_STRICT)
      प्रत्यागम CURLE_URL_MALFORMAT;
  }
  अण्णहा जइ(options & H1_PARSE_OPT_STRICT)
    प्रत्यागम CURLE_URL_MALFORMAT;

  जइ(parser->पङ्क्तिः_len > parser->max_पङ्क्तिः_len) {
    प्रत्यागम CURLE_URL_MALFORMAT;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode detect_पङ्क्तिः(संरचना h1_req_parser *parser,
                            ध्रुव uint8_t *buf, ध्रुव size_t buflen,
                            size_t *pnread)
{
  ध्रुव वर्ण *पङ्क्तिः_end;

  DEBUGASSERT(!parser->पङ्क्तिः);
  *pnread = 0;
  पङ्क्तिः_end = memchr(buf, '\n', buflen);
  जइ(!पङ्क्तिः_end)
    प्रत्यागम CURLE_AGAIN;
  parser->पङ्क्तिः = (ध्रुव वर्ण *)buf;
  parser->पङ्क्तिः_len = पङ्क्तिः_end - parser->पङ्क्तिः + 1;
  *pnread = parser->पङ्क्तिः_len;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode next_पङ्क्तिः(संरचना h1_req_parser *parser,
                          ध्रुव uint8_t *buf, ध्रुव size_t buflen, पूर्णाङ्क options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  जइ(parser->पङ्क्तिः) {
    parser->पङ्क्तिः = रिक्त;
    parser->पङ्क्तिः_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_पङ्क्तिः(parser, buf, buflen, pnread);
  जइ(!result) {
    जइ(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->पङ्क्तिः,
                              parser->पङ्क्तिः_len);
      जइ(result)
        प्रत्यागम result;
      parser->पङ्क्तिः = curlx_dyn_ptr(&parser->scratch);
      parser->पङ्क्तिः_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_पङ्क्तिः(parser, options);
    जइ(result)
      प्रत्यागम result;
  }
  अण्णहा जइ(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (ध्रुव unsigned वर्ण *)buf,
                            buflen);
    *pnread = buflen;
  }
  प्रत्यागम result;
}

स्थिर CURLcode start_req(संरचना h1_req_parser *parser,
                          ध्रुव वर्ण *scheme_default,
                          ध्रुव वर्ण *custom_method,
                          पूर्णाङ्क options)
{
  ध्रुव वर्ण *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = रिक्त;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  जइ(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->पङ्क्तिः, strlen(custom_method))) {
    p = parser->पङ्क्तिः + strlen(custom_method);
  }
  अण्णहा {
    p = memchr(parser->पङ्क्तिः, ' ', parser->पङ्क्तिः_len);
    जइ(!p || p == parser->पङ्क्तिः)
      गच्छ out;
  }

  m = parser->पङ्क्तिः;
  m_len = p - parser->पङ्क्तिः;
  target = p + 1;
  target_len = hv_len = 0;
  hv = रिक्त;

  /* URL may contain spaces so scan backwards */
  प्रति(i = parser->पङ्क्तिः_len; i > m_len; --i) {
    जइ(parser->पङ्क्तिः[i] == ' ') {
      hv = &parser->पङ्क्तिः[i + 1];
      hv_len = parser->पङ्क्तिः_len - i;
      target_len = (hv - target) - 1;
      भङ्ग;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  जइ(!target_len || !hv_len)
    गच्छ out;

  (शून्य)hv;

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
  scheme = authority = path = रिक्त;
  scheme_len = authority_len = path_len = 0;

  जइ(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  अण्णहा जइ(!strncmp("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  अण्णहा जइ(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  अण्णहा {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    वर्ण tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    जइ(target_len >= परिमाण(tmp))
      गच्छ out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    जइ(Curl_is_absolute_url(tmp, रिक्त, 0, FALSE)) {
      unsigned पूर्णाङ्क url_options;

      url = curl_url();
      जइ(!url) {
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      जइ(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      जइ(uc) {
        गच्छ out;
      }
    }

    जइ(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      गच्छ out;
    }
  }

  जइ(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  अण्णहा {
    जइ(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = strlen(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  प्रत्यागम result;
}

CURLcode Curl_h1_req_parse_read(संरचना h1_req_parser *parser,
                                ध्रुव uint8_t *buf, size_t buflen,
                                ध्रुव वर्ण *scheme_default,
                                ध्रुव वर्ण *custom_method,
                                पूर्णाङ्क options, size_t *pnread)
{
  CURLcode result = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  जइ(!buf)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  जाव(!parser->done) {
    result = next_पङ्क्तिः(parser, buf, buflen, options, &nread);
    जइ(result) {
      जइ(result == CURLE_AGAIN)
        result = CURLE_OK;
      गच्छ out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    जइ(!parser->पङ्क्तिः) {
      /* consumed bytes, but line not complete */
      जइ(!buflen)
        गच्छ out;
    }
    अण्णहा जइ(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      जइ(result)
        गच्छ out;
    }
    अण्णहा जइ(parser->पङ्क्तिः_len == 0) {
      /* last, empty line, we are finished */
      जइ(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        गच्छ out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    अण्णहा {
      result = Curl_dynhds_h1_add_पङ्क्तिः(&parser->req->headers,
                                       parser->पङ्क्तिः, parser->पङ्क्तिः_len);
      जइ(result)
        गच्छ out;
    }
  }

out:
  प्रत्यागम result;
}

CURLcode Curl_h1_req_write_head(संरचना httpreq *req, पूर्णाङ्क http_minor,
                                संरचना dynbuf *dbuf)
{
  CURLcode result;

  result = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  जइ(result)
    गच्छ out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  जइ(result)
    गच्छ out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  प्रत्यागम result;
}

#endif /* !CURL_DISABLE_HTTP */
