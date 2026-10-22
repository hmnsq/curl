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
#ضمن "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#ضمن "urldata.h"
#ضمن "http.h"
#ضمن "http1.h"
#ضمن "urlapi-int.h"


#define H1_MAX_URL_LEN (8 * 1024)

فراغ Curl_h1_req_parse_init(بنية h1_req_parser *parser, size_t max_line_len)
{
  memset(parser, 0, حجم(*parser));
  parser->max_line_len = max_line_len;
  curlx_dyn_init(&parser->scratch, max_line_len);
}

فراغ Curl_h1_req_parse_free(بنية h1_req_parser *parser)
{
  𐡄𐡍(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = عدم;
    parser->done = FALSE;
  }
}

ساكن CURLcode trim_line(بنية h1_req_parser *parser, صحيح options)
{
  DEBUGASSERT(parser->line);
  𐡄𐡍(parser->line_len) {
    𐡄𐡍(parser->line[parser->line_len - 1] == '\n')
      --parser->line_len;
    𐡄𐡍(parser->line_len) {
      𐡄𐡍(parser->line[parser->line_len - 1] == '\r')
        --parser->line_len;
      وإلا 𐡄𐡍(options & H1_PARSE_OPT_STRICT)
        أرجع CURLE_URL_MALFORMAT;
    }
    وإلا 𐡄𐡍(options & H1_PARSE_OPT_STRICT)
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا 𐡄𐡍(options & H1_PARSE_OPT_STRICT)
    أرجع CURLE_URL_MALFORMAT;

  𐡄𐡍(parser->line_len > parser->max_line_len) {
    أرجع CURLE_URL_MALFORMAT;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode detect_line(بنية h1_req_parser *parser,
                            ثابت uint8_t *buf, ثابت size_t buflen,
                            size_t *pnread)
{
  ثابت حرف *line_end;

  DEBUGASSERT(!parser->line);
  *pnread = 0;
  line_end = memchr(buf, '\n', buflen);
  𐡄𐡍(!line_end)
    أرجع CURLE_AGAIN;
  parser->line = (ثابت حرف *)buf;
  parser->line_len = line_end - parser->line + 1;
  *pnread = parser->line_len;
  أرجع CURLE_OK;
}

ساكن CURLcode next_line(بنية h1_req_parser *parser,
                          ثابت uint8_t *buf, ثابت size_t buflen, صحيح options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  𐡄𐡍(parser->line) {
    parser->line = عدم;
    parser->line_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_line(parser, buf, buflen, pnread);
  𐡄𐡍(!result) {
    𐡄𐡍(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->line,
                              parser->line_len);
      𐡄𐡍(result)
        أرجع result;
      parser->line = curlx_dyn_ptr(&parser->scratch);
      parser->line_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_line(parser, options);
    𐡄𐡍(result)
      أرجع result;
  }
  وإلا 𐡄𐡍(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (ثابت unsigned حرف *)buf,
                            buflen);
    *pnread = buflen;
  }
  أرجع result;
}

ساكن CURLcode start_req(بنية h1_req_parser *parser,
                          ثابت حرف *scheme_default,
                          ثابت حرف *custom_method,
                          صحيح options)
{
  ثابت حرف *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = عدم;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  𐡄𐡍(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->line, strlen(custom_method))) {
    p = parser->line + strlen(custom_method);
  }
  وإلا {
    p = memchr(parser->line, ' ', parser->line_len);
    𐡄𐡍(!p || p == parser->line)
      اذهب out;
  }

  m = parser->line;
  m_len = p - parser->line;
  target = p + 1;
  target_len = hv_len = 0;
  hv = عدم;

  /* URL may contain spaces so scan backwards */
  لكل(i = parser->line_len; i > m_len; --i) {
    𐡄𐡍(parser->line[i] == ' ') {
      hv = &parser->line[i + 1];
      hv_len = parser->line_len - i;
      target_len = (hv - target) - 1;
      اكسر;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  𐡄𐡍(!target_len || !hv_len)
    اذهب out;

  (فراغ)hv;

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
  scheme = authority = path = عدم;
  scheme_len = authority_len = path_len = 0;

  𐡄𐡍(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  وإلا 𐡄𐡍(!strncmp("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  وإلا 𐡄𐡍(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  وإلا {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    حرف tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    𐡄𐡍(target_len >= حجم(tmp))
      اذهب out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    𐡄𐡍(Curl_is_absolute_url(tmp, عدم, 0, FALSE)) {
      unsigned صحيح url_options;

      url = curl_url();
      𐡄𐡍(!url) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      𐡄𐡍(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      𐡄𐡍(uc) {
        اذهب out;
      }
    }

    𐡄𐡍(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      اذهب out;
    }
  }

  𐡄𐡍(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  وإلا {
    𐡄𐡍(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = strlen(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  أرجع result;
}

CURLcode Curl_h1_req_parse_read(بنية h1_req_parser *parser,
                                ثابت uint8_t *buf, size_t buflen,
                                ثابت حرف *scheme_default,
                                ثابت حرف *custom_method,
                                صحيح options, size_t *pnread)
{
  CURLcode result = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  𐡄𐡍(!buf)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  طالما(!parser->done) {
    result = next_line(parser, buf, buflen, options, &nread);
    𐡄𐡍(result) {
      𐡄𐡍(result == CURLE_AGAIN)
        result = CURLE_OK;
      اذهب out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    𐡄𐡍(!parser->line) {
      /* consumed bytes, but line not complete */
      𐡄𐡍(!buflen)
        اذهب out;
    }
    وإلا 𐡄𐡍(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      𐡄𐡍(result)
        اذهب out;
    }
    وإلا 𐡄𐡍(parser->line_len == 0) {
      /* last, empty line, we are finished */
      𐡄𐡍(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        اذهب out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    وإلا {
      result = Curl_dynhds_h1_add_line(&parser->req->headers,
                                       parser->line, parser->line_len);
      𐡄𐡍(result)
        اذهب out;
    }
  }

out:
  أرجع result;
}

CURLcode Curl_h1_req_write_head(بنية httpreq *req, صحيح http_minor,
                                بنية dynbuf *dbuf)
{
  CURLcode result;

  result = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  𐡄𐡍(result)
    اذهب out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  𐡄𐡍(result)
    اذهب out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  أرجع result;
}

#endif /* !CURL_DISABLE_HTTP */
