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

#समावेश "urldata.h"
#समावेश "http.h"
#समावेश "http1.h"
#समावेश "urlapi-int.h"


#घोषणा H1_MAX_URL_LEN (8 * 1024)

व्योम Curl_h1_req_parse_init(काष्ठा h1_req_parser *parser, माप_प्रकार max_पंक्ति_len)
{
  स_रखो(parser, 0, माप(*parser));
  parser->max_पंक्ति_len = max_पंक्ति_len;
  curlx_dyn_init(&parser->scratch, max_पंक्ति_len);
}

व्योम Curl_h1_req_parse_free(काष्ठा h1_req_parser *parser)
{
  अगर(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = शून्य;
    parser->done = FALSE;
  }
}

अटल CURLcode trim_पंक्ति(काष्ठा h1_req_parser *parser, पूर्णांक options)
{
  DEBUGASSERT(parser->पंक्ति);
  अगर(parser->पंक्ति_len) {
    अगर(parser->पंक्ति[parser->पंक्ति_len - 1] == '\n')
      --parser->पंक्ति_len;
    अगर(parser->पंक्ति_len) {
      अगर(parser->पंक्ति[parser->पंक्ति_len - 1] == '\r')
        --parser->पंक्ति_len;
      अन्यथा अगर(options & H1_PARSE_OPT_STRICT)
        वापस CURLE_URL_MALFORMAT;
    }
    अन्यथा अगर(options & H1_PARSE_OPT_STRICT)
      वापस CURLE_URL_MALFORMAT;
  }
  अन्यथा अगर(options & H1_PARSE_OPT_STRICT)
    वापस CURLE_URL_MALFORMAT;

  अगर(parser->पंक्ति_len > parser->max_पंक्ति_len) {
    वापस CURLE_URL_MALFORMAT;
  }
  वापस CURLE_OK;
}

अटल CURLcode detect_पंक्ति(काष्ठा h1_req_parser *parser,
                            स्थिर uint8_t *buf, स्थिर माप_प्रकार buflen,
                            माप_प्रकार *pnread)
{
  स्थिर अक्षर *पंक्ति_end;

  DEBUGASSERT(!parser->पंक्ति);
  *pnread = 0;
  पंक्ति_end = स_प्रथम(buf, '\n', buflen);
  अगर(!पंक्ति_end)
    वापस CURLE_AGAIN;
  parser->पंक्ति = (स्थिर अक्षर *)buf;
  parser->पंक्ति_len = पंक्ति_end - parser->पंक्ति + 1;
  *pnread = parser->पंक्ति_len;
  वापस CURLE_OK;
}

अटल CURLcode अग्र_पंक्ति(काष्ठा h1_req_parser *parser,
                          स्थिर uint8_t *buf, स्थिर माप_प्रकार buflen, पूर्णांक options,
                          माप_प्रकार *pnread)
{
  CURLcode परिणाम;

  *pnread = 0;
  अगर(parser->पंक्ति) {
    parser->पंक्ति = शून्य;
    parser->पंक्ति_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  परिणाम = detect_पंक्ति(parser, buf, buflen, pnread);
  अगर(!परिणाम) {
    अगर(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      परिणाम = curlx_dyn_addn(&parser->scratch, parser->पंक्ति,
                              parser->पंक्ति_len);
      अगर(परिणाम)
        वापस परिणाम;
      parser->पंक्ति = curlx_dyn_ptr(&parser->scratch);
      parser->पंक्ति_len = curlx_dyn_len(&parser->scratch);
    }
    परिणाम = trim_पंक्ति(parser, options);
    अगर(परिणाम)
      वापस परिणाम;
  }
  अन्यथा अगर(परिणाम == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    परिणाम = curlx_dyn_addn(&parser->scratch, (स्थिर अचिन्हित अक्षर *)buf,
                            buflen);
    *pnread = buflen;
  }
  वापस परिणाम;
}

अटल CURLcode start_req(काष्ठा h1_req_parser *parser,
                          स्थिर अक्षर *scheme_default,
                          स्थिर अक्षर *custom_method,
                          पूर्णांक options)
{
  स्थिर अक्षर *p, *m, *target, *hv, *scheme, *authority, *path;
  माप_प्रकार m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  माप_प्रकार i;
  CURLU *url = शून्य;
  CURLcode परिणाम = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  अगर(custom_method && custom_method[0] &&
     !म_भेदन(custom_method, parser->पंक्ति, म_माप(custom_method))) {
    p = parser->पंक्ति + म_माप(custom_method);
  }
  अन्यथा {
    p = स_प्रथम(parser->पंक्ति, ' ', parser->पंक्ति_len);
    अगर(!p || p == parser->पंक्ति)
      जाओ out;
  }

  m = parser->पंक्ति;
  m_len = p - parser->पंक्ति;
  target = p + 1;
  target_len = hv_len = 0;
  hv = शून्य;

  /* URL may contain spaces so scan backwards */
  क्रम(i = parser->पंक्ति_len; i > m_len; --i) {
    अगर(parser->पंक्ति[i] == ' ') {
      hv = &parser->पंक्ति[i + 1];
      hv_len = parser->पंक्ति_len - i;
      target_len = (hv - target) - 1;
      अवरोध;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  अगर(!target_len || !hv_len)
    जाओ out;

  (व्योम)hv;

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
  scheme = authority = path = शून्य;
  scheme_len = authority_len = path_len = 0;

  अगर(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  अन्यथा अगर(!म_भेदन("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  अन्यथा अगर(target[0] == '/') {
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
    अगर(target_len >= माप(tmp))
      जाओ out;
    स_नकल(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    अगर(Curl_is_absolute_url(tmp, शून्य, 0, FALSE)) {
      अचिन्हित पूर्णांक url_options;

      url = curl_url();
      अगर(!url) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      अगर(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      अगर(uc) {
        जाओ out;
      }
    }

    अगर(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      जाओ out;
    }
  }

  अगर(url) {
    परिणाम = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  अन्यथा {
    अगर(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = म_माप(scheme_default);
    }
    परिणाम = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  वापस परिणाम;
}

CURLcode Curl_h1_req_parse_read(काष्ठा h1_req_parser *parser,
                                स्थिर uint8_t *buf, माप_प्रकार buflen,
                                स्थिर अक्षर *scheme_default,
                                स्थिर अक्षर *custom_method,
                                पूर्णांक options, माप_प्रकार *pnread)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  अगर(!buf)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  जबतक(!parser->done) {
    परिणाम = अग्र_पंक्ति(parser, buf, buflen, options, &nread);
    अगर(परिणाम) {
      अगर(परिणाम == CURLE_AGAIN)
        परिणाम = CURLE_OK;
      जाओ out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    अगर(!parser->पंक्ति) {
      /* consumed bytes, but line not complete */
      अगर(!buflen)
        जाओ out;
    }
    अन्यथा अगर(!parser->req) {
      परिणाम = start_req(parser, scheme_default, custom_method, options);
      अगर(परिणाम)
        जाओ out;
    }
    अन्यथा अगर(parser->पंक्ति_len == 0) {
      /* last, empty line, we are finished */
      अगर(!parser->req) {
        परिणाम = CURLE_URL_MALFORMAT;
        जाओ out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    अन्यथा {
      परिणाम = Curl_dynhds_h1_add_पंक्ति(&parser->req->headers,
                                       parser->पंक्ति, parser->पंक्ति_len);
      अगर(परिणाम)
        जाओ out;
    }
  }

out:
  वापस परिणाम;
}

CURLcode Curl_h1_req_write_शीर्ष(काष्ठा httpreq *req, पूर्णांक http_minor,
                                काष्ठा dynbuf *dbuf)
{
  CURLcode परिणाम;

  परिणाम = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  अगर(परिणाम)
    जाओ out;

  परिणाम = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP */
