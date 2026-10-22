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

शून्य Curl_h1_req_parse_init(संरचना h1_req_parser *parser, माप_प्रकार max_ओळ_len)
{
  स_रखो(parser, 0, आकार(*parser));
  parser->max_ओळ_len = max_ओळ_len;
  curlx_dyn_init(&parser->scratch, max_ओळ_len);
}

शून्य Curl_h1_req_parse_free(संरचना h1_req_parser *parser)
{
  जर(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = रिक्त;
    parser->done = FALSE;
  }
}

स्थिर CURLcode trim_ओळ(संरचना h1_req_parser *parser, पूर्णांक options)
{
  DEBUGASSERT(parser->ओळ);
  जर(parser->ओळ_len) {
    जर(parser->ओळ[parser->ओळ_len - 1] == '\n')
      --parser->ओळ_len;
    जर(parser->ओळ_len) {
      जर(parser->ओळ[parser->ओळ_len - 1] == '\r')
        --parser->ओळ_len;
      अन्यथा जर(options & H1_PARSE_OPT_STRICT)
        परत CURLE_URL_MALFORMAT;
    }
    अन्यथा जर(options & H1_PARSE_OPT_STRICT)
      परत CURLE_URL_MALFORMAT;
  }
  अन्यथा जर(options & H1_PARSE_OPT_STRICT)
    परत CURLE_URL_MALFORMAT;

  जर(parser->ओळ_len > parser->max_ओळ_len) {
    परत CURLE_URL_MALFORMAT;
  }
  परत CURLE_OK;
}

स्थिर CURLcode detect_ओळ(संरचना h1_req_parser *parser,
                            स्थिरांक uint8_t *buf, स्थिरांक माप_प्रकार buflen,
                            माप_प्रकार *pnread)
{
  स्थिरांक अक्षर *ओळ_end;

  DEBUGASSERT(!parser->ओळ);
  *pnread = 0;
  ओळ_end = स_प्रथम(buf, '\n', buflen);
  जर(!ओळ_end)
    परत CURLE_AGAIN;
  parser->ओळ = (स्थिरांक अक्षर *)buf;
  parser->ओळ_len = ओळ_end - parser->ओळ + 1;
  *pnread = parser->ओळ_len;
  परत CURLE_OK;
}

स्थिर CURLcode next_ओळ(संरचना h1_req_parser *parser,
                          स्थिरांक uint8_t *buf, स्थिरांक माप_प्रकार buflen, पूर्णांक options,
                          माप_प्रकार *pnread)
{
  CURLcode result;

  *pnread = 0;
  जर(parser->ओळ) {
    parser->ओळ = रिक्त;
    parser->ओळ_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_ओळ(parser, buf, buflen, pnread);
  जर(!result) {
    जर(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->ओळ,
                              parser->ओळ_len);
      जर(result)
        परत result;
      parser->ओळ = curlx_dyn_ptr(&parser->scratch);
      parser->ओळ_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_ओळ(parser, options);
    जर(result)
      परत result;
  }
  अन्यथा जर(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (स्थिरांक अचिन्हित अक्षर *)buf,
                            buflen);
    *pnread = buflen;
  }
  परत result;
}

स्थिर CURLcode start_req(संरचना h1_req_parser *parser,
                          स्थिरांक अक्षर *scheme_default,
                          स्थिरांक अक्षर *custom_method,
                          पूर्णांक options)
{
  स्थिरांक अक्षर *p, *m, *target, *hv, *scheme, *authority, *path;
  माप_प्रकार m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  माप_प्रकार i;
  CURLU *url = रिक्त;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  जर(custom_method && custom_method[0] &&
     !म_भेदन(custom_method, parser->ओळ, म_माप(custom_method))) {
    p = parser->ओळ + म_माप(custom_method);
  }
  अन्यथा {
    p = स_प्रथम(parser->ओळ, ' ', parser->ओळ_len);
    जर(!p || p == parser->ओळ)
      जा out;
  }

  m = parser->ओळ;
  m_len = p - parser->ओळ;
  target = p + 1;
  target_len = hv_len = 0;
  hv = रिक्त;

  /* URL may contain spaces so scan backwards */
  साठी(i = parser->ओळ_len; i > m_len; --i) {
    जर(parser->ओळ[i] == ' ') {
      hv = &parser->ओळ[i + 1];
      hv_len = parser->ओळ_len - i;
      target_len = (hv - target) - 1;
      थांबा;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  जर(!target_len || !hv_len)
    जा out;

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

  जर(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  अन्यथा जर(!म_भेदन("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  अन्यथा जर(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  अन्यथा {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    अक्षर tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    जर(target_len >= आकार(tmp))
      जा out;
    स_नकल(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    जर(Curl_is_absolute_url(tmp, रिक्त, 0, FALSE)) {
      अचिन्हित पूर्णांक url_options;

      url = curl_url();
      जर(!url) {
        result = CURLE_OUT_OF_MEMORY;
        जा out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      जर(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      जर(uc) {
        जा out;
      }
    }

    जर(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      जा out;
    }
  }

  जर(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  अन्यथा {
    जर(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = म_माप(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  परत result;
}

CURLcode Curl_h1_req_parse_read(संरचना h1_req_parser *parser,
                                स्थिरांक uint8_t *buf, माप_प्रकार buflen,
                                स्थिरांक अक्षर *scheme_default,
                                स्थिरांक अक्षर *custom_method,
                                पूर्णांक options, माप_प्रकार *pnread)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  जर(!buf)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  पर्यंत(!parser->done) {
    result = next_ओळ(parser, buf, buflen, options, &nread);
    जर(result) {
      जर(result == CURLE_AGAIN)
        result = CURLE_OK;
      जा out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    जर(!parser->ओळ) {
      /* consumed bytes, but line not complete */
      जर(!buflen)
        जा out;
    }
    अन्यथा जर(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      जर(result)
        जा out;
    }
    अन्यथा जर(parser->ओळ_len == 0) {
      /* last, empty line, we are finished */
      जर(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        जा out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    अन्यथा {
      result = Curl_dynhds_h1_add_ओळ(&parser->req->headers,
                                       parser->ओळ, parser->ओळ_len);
      जर(result)
        जा out;
    }
  }

out:
  परत result;
}

CURLcode Curl_h1_req_write_head(संरचना httpreq *req, पूर्णांक http_minor,
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
  जर(result)
    जा out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  जर(result)
    जा out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  परत result;
}

#endif /* !CURL_DISABLE_HTTP */
