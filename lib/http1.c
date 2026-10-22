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

शून्य Curl_h1_req_parse_init(संरचना h1_req_parser *parser, size_t max_ओळ_len)
{
  memset(parser, 0, परिमाण(*parser));
  parser->max_ओळ_len = max_ओळ_len;
  curlx_dyn_init(&parser->scratch, max_ओळ_len);
}

शून्य Curl_h1_req_parse_free(संरचना h1_req_parser *parser)
{
  यदि(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = रिक्त;
    parser->done = FALSE;
  }
}

स्थिर CURLcode trim_ओळ(संरचना h1_req_parser *parser, पूर्णाङ्क options)
{
  DEBUGASSERT(parser->ओळ);
  यदि(parser->ओळ_len) {
    यदि(parser->ओळ[parser->ओळ_len - 1] == '\n')
      --parser->ओळ_len;
    यदि(parser->ओळ_len) {
      यदि(parser->ओळ[parser->ओळ_len - 1] == '\r')
        --parser->ओळ_len;
      अन्यथा यदि(options & H1_PARSE_OPT_STRICT)
        प्रत्यागम CURLE_URL_MALFORMAT;
    }
    अन्यथा यदि(options & H1_PARSE_OPT_STRICT)
      प्रत्यागम CURLE_URL_MALFORMAT;
  }
  अन्यथा यदि(options & H1_PARSE_OPT_STRICT)
    प्रत्यागम CURLE_URL_MALFORMAT;

  यदि(parser->ओळ_len > parser->max_ओळ_len) {
    प्रत्यागम CURLE_URL_MALFORMAT;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode detect_ओळ(संरचना h1_req_parser *parser,
                            ध्रुव uint8_t *buf, ध्रुव size_t buflen,
                            size_t *pnread)
{
  ध्रुव वर्ण *ओळ_end;

  DEBUGASSERT(!parser->ओळ);
  *pnread = 0;
  ओळ_end = memchr(buf, '\n', buflen);
  यदि(!ओळ_end)
    प्रत्यागम CURLE_AGAIN;
  parser->ओळ = (ध्रुव वर्ण *)buf;
  parser->ओळ_len = ओळ_end - parser->ओळ + 1;
  *pnread = parser->ओळ_len;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode next_ओळ(संरचना h1_req_parser *parser,
                          ध्रुव uint8_t *buf, ध्रुव size_t buflen, पूर्णाङ्क options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  यदि(parser->ओळ) {
    parser->ओळ = रिक्त;
    parser->ओळ_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_ओळ(parser, buf, buflen, pnread);
  यदि(!result) {
    यदि(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->ओळ,
                              parser->ओळ_len);
      यदि(result)
        प्रत्यागम result;
      parser->ओळ = curlx_dyn_ptr(&parser->scratch);
      parser->ओळ_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_ओळ(parser, options);
    यदि(result)
      प्रत्यागम result;
  }
  अन्यथा यदि(result == CURLE_AGAIN) {
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
  यदि(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->ओळ, strlen(custom_method))) {
    p = parser->ओळ + strlen(custom_method);
  }
  अन्यथा {
    p = memchr(parser->ओळ, ' ', parser->ओळ_len);
    यदि(!p || p == parser->ओळ)
      गच्छ out;
  }

  m = parser->ओळ;
  m_len = p - parser->ओळ;
  target = p + 1;
  target_len = hv_len = 0;
  hv = रिक्त;

  /* URL may contain spaces so scan backwards */
  प्रति(i = parser->ओळ_len; i > m_len; --i) {
    यदि(parser->ओळ[i] == ' ') {
      hv = &parser->ओळ[i + 1];
      hv_len = parser->ओळ_len - i;
      target_len = (hv - target) - 1;
      भङ्ग;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  यदि(!target_len || !hv_len)
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

  यदि(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  अन्यथा यदि(!strncmp("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  अन्यथा यदि(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  अन्यथा {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    वर्ण tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    यदि(target_len >= परिमाण(tmp))
      गच्छ out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    यदि(Curl_is_absolute_url(tmp, रिक्त, 0, FALSE)) {
      unsigned पूर्णाङ्क url_options;

      url = curl_url();
      यदि(!url) {
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      यदि(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      यदि(uc) {
        गच्छ out;
      }
    }

    यदि(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      गच्छ out;
    }
  }

  यदि(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  अन्यथा {
    यदि(!scheme && scheme_default) {
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
  यदि(!buf)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  यावत्(!parser->done) {
    result = next_ओळ(parser, buf, buflen, options, &nread);
    यदि(result) {
      यदि(result == CURLE_AGAIN)
        result = CURLE_OK;
      गच्छ out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    यदि(!parser->ओळ) {
      /* consumed bytes, but line not complete */
      यदि(!buflen)
        गच्छ out;
    }
    अन्यथा यदि(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      यदि(result)
        गच्छ out;
    }
    अन्यथा यदि(parser->ओळ_len == 0) {
      /* last, empty line, we are finished */
      यदि(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        गच्छ out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    अन्यथा {
      result = Curl_dynhds_h1_add_ओळ(&parser->req->headers,
                                       parser->ओळ, parser->ओळ_len);
      यदि(result)
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
  यदि(result)
    गच्छ out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  यदि(result)
    गच्छ out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  प्रत्यागम result;
}

#endif /* !CURL_DISABLE_HTTP */
