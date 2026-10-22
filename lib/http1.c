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

शून्य Curl_h1_req_parse_init(संरचना h1_req_parser *parser, माप_प्रकार max_पङ्क्ति_len)
{
  स_रखो(parser, 0, परिमाण(*parser));
  parser->max_पङ्क्ति_len = max_पङ्क्ति_len;
  curlx_dyn_init(&parser->scratch, max_पङ्क्ति_len);
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

स्थिर CURLcode trim_पङ्क्ति(संरचना h1_req_parser *parser, पूर्णाङ्क options)
{
  DEBUGASSERT(parser->पङ्क्ति);
  यदि(parser->पङ्क्ति_len) {
    यदि(parser->पङ्क्ति[parser->पङ्क्ति_len - 1] == '\n')
      --parser->पङ्क्ति_len;
    यदि(parser->पङ्क्ति_len) {
      यदि(parser->पङ्क्ति[parser->पङ्क्ति_len - 1] == '\r')
        --parser->पङ्क्ति_len;
      अन्यथा यदि(options & H1_PARSE_OPT_STRICT)
        फर्काऊ CURLE_URL_MALFORMAT;
    }
    अन्यथा यदि(options & H1_PARSE_OPT_STRICT)
      फर्काऊ CURLE_URL_MALFORMAT;
  }
  अन्यथा यदि(options & H1_PARSE_OPT_STRICT)
    फर्काऊ CURLE_URL_MALFORMAT;

  यदि(parser->पङ्क्ति_len > parser->max_पङ्क्ति_len) {
    फर्काऊ CURLE_URL_MALFORMAT;
  }
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode detect_पङ्क्ति(संरचना h1_req_parser *parser,
                            अचर uint8_t *buf, अचर माप_प्रकार buflen,
                            माप_प्रकार *pnread)
{
  अचर वर्ण *पङ्क्ति_end;

  DEBUGASSERT(!parser->पङ्क्ति);
  *pnread = 0;
  पङ्क्ति_end = स_प्रथम(buf, '\n', buflen);
  यदि(!पङ्क्ति_end)
    फर्काऊ CURLE_AGAIN;
  parser->पङ्क्ति = (अचर वर्ण *)buf;
  parser->पङ्क्ति_len = पङ्क्ति_end - parser->पङ्क्ति + 1;
  *pnread = parser->पङ्क्ति_len;
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode next_पङ्क्ति(संरचना h1_req_parser *parser,
                          अचर uint8_t *buf, अचर माप_प्रकार buflen, पूर्णाङ्क options,
                          माप_प्रकार *pnread)
{
  CURLcode result;

  *pnread = 0;
  यदि(parser->पङ्क्ति) {
    parser->पङ्क्ति = रिक्त;
    parser->पङ्क्ति_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_पङ्क्ति(parser, buf, buflen, pnread);
  यदि(!result) {
    यदि(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->पङ्क्ति,
                              parser->पङ्क्ति_len);
      यदि(result)
        फर्काऊ result;
      parser->पङ्क्ति = curlx_dyn_ptr(&parser->scratch);
      parser->पङ्क्ति_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_पङ्क्ति(parser, options);
    यदि(result)
      फर्काऊ result;
  }
  अन्यथा यदि(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (अचर अचिन्हित वर्ण *)buf,
                            buflen);
    *pnread = buflen;
  }
  फर्काऊ result;
}

स्थिर CURLcode start_req(संरचना h1_req_parser *parser,
                          अचर वर्ण *scheme_default,
                          अचर वर्ण *custom_method,
                          पूर्णाङ्क options)
{
  अचर वर्ण *p, *m, *target, *hv, *scheme, *authority, *path;
  माप_प्रकार m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  माप_प्रकार i;
  CURLU *url = रिक्त;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  यदि(custom_method && custom_method[0] &&
     !म_भेदन(custom_method, parser->पङ्क्ति, म_माप(custom_method))) {
    p = parser->पङ्क्ति + म_माप(custom_method);
  }
  अन्यथा {
    p = स_प्रथम(parser->पङ्क्ति, ' ', parser->पङ्क्ति_len);
    यदि(!p || p == parser->पङ्क्ति)
      गच्छ out;
  }

  m = parser->पङ्क्ति;
  m_len = p - parser->पङ्क्ति;
  target = p + 1;
  target_len = hv_len = 0;
  hv = रिक्त;

  /* URL may contain spaces so scan backwards */
  लागि(i = parser->पङ्क्ति_len; i > m_len; --i) {
    यदि(parser->पङ्क्ति[i] == ' ') {
      hv = &parser->पङ्क्ति[i + 1];
      hv_len = parser->पङ्क्ति_len - i;
      target_len = (hv - target) - 1;
      रोक;
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
  अन्यथा यदि(!म_भेदन("CONNECT", m, m_len)) {
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
    स_नकल(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    यदि(Curl_is_absolute_url(tmp, रिक्त, 0, FALSE)) {
      अचिन्हित पूर्णाङ्क url_options;

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
      scheme_len = म_माप(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  फर्काऊ result;
}

CURLcode Curl_h1_req_parse_read(संरचना h1_req_parser *parser,
                                अचर uint8_t *buf, माप_प्रकार buflen,
                                अचर वर्ण *scheme_default,
                                अचर वर्ण *custom_method,
                                पूर्णाङ्क options, माप_प्रकार *pnread)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  यदि(!buf)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  जबसम्म(!parser->done) {
    result = next_पङ्क्ति(parser, buf, buflen, options, &nread);
    यदि(result) {
      यदि(result == CURLE_AGAIN)
        result = CURLE_OK;
      गच्छ out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    यदि(!parser->पङ्क्ति) {
      /* consumed bytes, but line not complete */
      यदि(!buflen)
        गच्छ out;
    }
    अन्यथा यदि(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      यदि(result)
        गच्छ out;
    }
    अन्यथा यदि(parser->पङ्क्ति_len == 0) {
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
      result = Curl_dynhds_h1_add_पङ्क्ति(&parser->req->headers,
                                       parser->पङ्क्ति, parser->पङ्क्ति_len);
      यदि(result)
        गच्छ out;
    }
  }

out:
  फर्काऊ result;
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
  फर्काऊ result;
}

#endif /* !CURL_DISABLE_HTTP */
