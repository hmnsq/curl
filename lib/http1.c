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
#شامل "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#شامل "urldata.h"
#شامل "http.h"
#شامل "http1.h"
#شامل "urlapi-int.h"


#define H1_MAX_URL_LEN (8 * 1024)

تهی Curl_h1_req_parse_init(ساختار h1_req_parser *parser, size_t max_سطر_len)
{
  memset(parser, 0, اندازه(*parser));
  parser->max_سطر_len = max_سطر_len;
  curlx_dyn_init(&parser->scratch, max_سطر_len);
}

تهی Curl_h1_req_parse_free(ساختار h1_req_parser *parser)
{
  اگر(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = پوچ;
    parser->done = FALSE;
  }
}

ایستا CURLcode trim_سطر(ساختار h1_req_parser *parser, صحیح options)
{
  DEBUGASSERT(parser->سطر);
  اگر(parser->سطر_len) {
    اگر(parser->سطر[parser->سطر_len - 1] == '\n')
      --parser->سطر_len;
    اگر(parser->سطر_len) {
      اگر(parser->سطر[parser->سطر_len - 1] == '\r')
        --parser->سطر_len;
      وگرنه اگر(options & H1_PARSE_OPT_STRICT)
        بازگشت CURLE_URL_MALFORMAT;
    }
    وگرنه اگر(options & H1_PARSE_OPT_STRICT)
      بازگشت CURLE_URL_MALFORMAT;
  }
  وگرنه اگر(options & H1_PARSE_OPT_STRICT)
    بازگشت CURLE_URL_MALFORMAT;

  اگر(parser->سطر_len > parser->max_سطر_len) {
    بازگشت CURLE_URL_MALFORMAT;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode detect_سطر(ساختار h1_req_parser *parser,
                            ثابت uint8_t *buf, ثابت size_t buflen,
                            size_t *pnread)
{
  ثابت نویسه *سطر_end;

  DEBUGASSERT(!parser->سطر);
  *pnread = 0;
  سطر_end = memchr(buf, '\n', buflen);
  اگر(!سطر_end)
    بازگشت CURLE_AGAIN;
  parser->سطر = (ثابت نویسه *)buf;
  parser->سطر_len = سطر_end - parser->سطر + 1;
  *pnread = parser->سطر_len;
  بازگشت CURLE_OK;
}

ایستا CURLcode next_سطر(ساختار h1_req_parser *parser,
                          ثابت uint8_t *buf, ثابت size_t buflen, صحیح options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  اگر(parser->سطر) {
    parser->سطر = پوچ;
    parser->سطر_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_سطر(parser, buf, buflen, pnread);
  اگر(!result) {
    اگر(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->سطر,
                              parser->سطر_len);
      اگر(result)
        بازگشت result;
      parser->سطر = curlx_dyn_ptr(&parser->scratch);
      parser->سطر_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_سطر(parser, options);
    اگر(result)
      بازگشت result;
  }
  وگرنه اگر(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (ثابت unsigned نویسه *)buf,
                            buflen);
    *pnread = buflen;
  }
  بازگشت result;
}

ایستا CURLcode start_req(ساختار h1_req_parser *parser,
                          ثابت نویسه *scheme_default,
                          ثابت نویسه *custom_method,
                          صحیح options)
{
  ثابت نویسه *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = پوچ;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  اگر(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->سطر, strlen(custom_method))) {
    p = parser->سطر + strlen(custom_method);
  }
  وگرنه {
    p = memchr(parser->سطر, ' ', parser->سطر_len);
    اگر(!p || p == parser->سطر)
      برو out;
  }

  m = parser->سطر;
  m_len = p - parser->سطر;
  target = p + 1;
  target_len = hv_len = 0;
  hv = پوچ;

  /* URL may contain spaces so scan backwards */
  برای(i = parser->سطر_len; i > m_len; --i) {
    اگر(parser->سطر[i] == ' ') {
      hv = &parser->سطر[i + 1];
      hv_len = parser->سطر_len - i;
      target_len = (hv - target) - 1;
      بشکن;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  اگر(!target_len || !hv_len)
    برو out;

  (تهی)hv;

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
  scheme = authority = path = پوچ;
  scheme_len = authority_len = path_len = 0;

  اگر(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  وگرنه اگر(!strncmp("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  وگرنه اگر(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  وگرنه {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    نویسه tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    اگر(target_len >= اندازه(tmp))
      برو out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    اگر(Curl_is_absolute_url(tmp, پوچ, 0, FALSE)) {
      unsigned صحیح url_options;

      url = curl_url();
      اگر(!url) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      اگر(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      اگر(uc) {
        برو out;
      }
    }

    اگر(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      برو out;
    }
  }

  اگر(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  وگرنه {
    اگر(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = strlen(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  بازگشت result;
}

CURLcode Curl_h1_req_parse_read(ساختار h1_req_parser *parser,
                                ثابت uint8_t *buf, size_t buflen,
                                ثابت نویسه *scheme_default,
                                ثابت نویسه *custom_method,
                                صحیح options, size_t *pnread)
{
  CURLcode result = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  اگر(!buf)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  وقتی(!parser->done) {
    result = next_سطر(parser, buf, buflen, options, &nread);
    اگر(result) {
      اگر(result == CURLE_AGAIN)
        result = CURLE_OK;
      برو out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    اگر(!parser->سطر) {
      /* consumed bytes, but line not complete */
      اگر(!buflen)
        برو out;
    }
    وگرنه اگر(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      اگر(result)
        برو out;
    }
    وگرنه اگر(parser->سطر_len == 0) {
      /* last, empty line, we are finished */
      اگر(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        برو out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    وگرنه {
      result = Curl_dynhds_h1_add_سطر(&parser->req->headers,
                                       parser->سطر, parser->سطر_len);
      اگر(result)
        برو out;
    }
  }

out:
  بازگشت result;
}

CURLcode Curl_h1_req_write_head(ساختار httpreq *req, صحیح http_minor,
                                ساختار dynbuf *dbuf)
{
  CURLcode result;

  result = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  اگر(result)
    برو out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  اگر(result)
    برو out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  بازگشت result;
}

#endif /* !CURL_DISABLE_HTTP */
