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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "http.h"
#അന്തര്ഭാവ "http1.h"
#അന്തര്ഭാവ "urlapi-int.h"


#define H1_MAX_URL_LEN (8 * 1024)

ശൂന്യം Curl_h1_req_parse_init(ഘടന h1_req_parser *parser, size_t max_വരി_len)
{
  memset(parser, 0, വലുപ്പം(*parser));
  parser->max_വരി_len = max_വരി_len;
  curlx_dyn_init(&parser->scratch, max_വരി_len);
}

ശൂന്യം Curl_h1_req_parse_free(ഘടന h1_req_parser *parser)
{
  എങ്കിൽ(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = ശൂന്യ;
    parser->done = FALSE;
  }
}

സ്ഥിതം CURLcode trim_വരി(ഘടന h1_req_parser *parser, പൂർണ്ണസംഖ്യ options)
{
  DEBUGASSERT(parser->വരി);
  എങ്കിൽ(parser->വരി_len) {
    എങ്കിൽ(parser->വരി[parser->വരി_len - 1] == '\n')
      --parser->വരി_len;
    എങ്കിൽ(parser->വരി_len) {
      എങ്കിൽ(parser->വരി[parser->വരി_len - 1] == '\r')
        --parser->വരി_len;
      അല്ലെങ്കിൽ എങ്കിൽ(options & H1_PARSE_OPT_STRICT)
        തിരികെ CURLE_URL_MALFORMAT;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(options & H1_PARSE_OPT_STRICT)
      തിരികെ CURLE_URL_MALFORMAT;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(options & H1_PARSE_OPT_STRICT)
    തിരികെ CURLE_URL_MALFORMAT;

  എങ്കിൽ(parser->വരി_len > parser->max_വരി_len) {
    തിരികെ CURLE_URL_MALFORMAT;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode detect_വരി(ഘടന h1_req_parser *parser,
                            സ്ഥിരം uint8_t *buf, സ്ഥിരം size_t buflen,
                            size_t *pnread)
{
  സ്ഥിരം അക്ഷരം *വരി_end;

  DEBUGASSERT(!parser->വരി);
  *pnread = 0;
  വരി_end = memchr(buf, '\n', buflen);
  എങ്കിൽ(!വരി_end)
    തിരികെ CURLE_AGAIN;
  parser->വരി = (സ്ഥിരം അക്ഷരം *)buf;
  parser->വരി_len = വരി_end - parser->വരി + 1;
  *pnread = parser->വരി_len;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode next_വരി(ഘടന h1_req_parser *parser,
                          സ്ഥിരം uint8_t *buf, സ്ഥിരം size_t buflen, പൂർണ്ണസംഖ്യ options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  എങ്കിൽ(parser->വരി) {
    parser->വരി = ശൂന്യ;
    parser->വരി_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_വരി(parser, buf, buflen, pnread);
  എങ്കിൽ(!result) {
    എങ്കിൽ(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->വരി,
                              parser->വരി_len);
      എങ്കിൽ(result)
        തിരികെ result;
      parser->വരി = curlx_dyn_ptr(&parser->scratch);
      parser->വരി_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_വരി(parser, options);
    എങ്കിൽ(result)
      തിരികെ result;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (സ്ഥിരം unsigned അക്ഷരം *)buf,
                            buflen);
    *pnread = buflen;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode start_req(ഘടന h1_req_parser *parser,
                          സ്ഥിരം അക്ഷരം *scheme_default,
                          സ്ഥിരം അക്ഷരം *custom_method,
                          പൂർണ്ണസംഖ്യ options)
{
  സ്ഥിരം അക്ഷരം *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = ശൂന്യ;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  എങ്കിൽ(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->വരി, strlen(custom_method))) {
    p = parser->വരി + strlen(custom_method);
  }
  അല്ലെങ്കിൽ {
    p = memchr(parser->വരി, ' ', parser->വരി_len);
    എങ്കിൽ(!p || p == parser->വരി)
      പോകുക out;
  }

  m = parser->വരി;
  m_len = p - parser->വരി;
  target = p + 1;
  target_len = hv_len = 0;
  hv = ശൂന്യ;

  /* URL may contain spaces so scan backwards */
  വേണ്ടി(i = parser->വരി_len; i > m_len; --i) {
    എങ്കിൽ(parser->വരി[i] == ' ') {
      hv = &parser->വരി[i + 1];
      hv_len = parser->വരി_len - i;
      target_len = (hv - target) - 1;
      നിർത്തുക;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  എങ്കിൽ(!target_len || !hv_len)
    പോകുക out;

  (ശൂന്യം)hv;

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
  scheme = authority = path = ശൂന്യ;
  scheme_len = authority_len = path_len = 0;

  എങ്കിൽ(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  അല്ലെങ്കിൽ {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    അക്ഷരം tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    എങ്കിൽ(target_len >= വലുപ്പം(tmp))
      പോകുക out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    എങ്കിൽ(Curl_is_absolute_url(tmp, ശൂന്യ, 0, FALSE)) {
      unsigned പൂർണ്ണസംഖ്യ url_options;

      url = curl_url();
      എങ്കിൽ(!url) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      എങ്കിൽ(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      എങ്കിൽ(uc) {
        പോകുക out;
      }
    }

    എങ്കിൽ(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      പോകുക out;
    }
  }

  എങ്കിൽ(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = strlen(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  തിരികെ result;
}

CURLcode Curl_h1_req_parse_read(ഘടന h1_req_parser *parser,
                                സ്ഥിരം uint8_t *buf, size_t buflen,
                                സ്ഥിരം അക്ഷരം *scheme_default,
                                സ്ഥിരം അക്ഷരം *custom_method,
                                പൂർണ്ണസംഖ്യ options, size_t *pnread)
{
  CURLcode result = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  എങ്കിൽ(!buf)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  ആകുമ്പോൾ(!parser->done) {
    result = next_വരി(parser, buf, buflen, options, &nread);
    എങ്കിൽ(result) {
      എങ്കിൽ(result == CURLE_AGAIN)
        result = CURLE_OK;
      പോകുക out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    എങ്കിൽ(!parser->വരി) {
      /* consumed bytes, but line not complete */
      എങ്കിൽ(!buflen)
        പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      എങ്കിൽ(result)
        പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(parser->വരി_len == 0) {
      /* last, empty line, we are finished */
      എങ്കിൽ(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        പോകുക out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    അല്ലെങ്കിൽ {
      result = Curl_dynhds_h1_add_വരി(&parser->req->headers,
                                       parser->വരി, parser->വരി_len);
      എങ്കിൽ(result)
        പോകുക out;
    }
  }

out:
  തിരികെ result;
}

CURLcode Curl_h1_req_write_head(ഘടന httpreq *req, പൂർണ്ണസംഖ്യ http_minor,
                                ഘടന dynbuf *dbuf)
{
  CURLcode result;

  result = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  എങ്കിൽ(result)
    പോകുക out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  തിരികെ result;
}

#endif /* !CURL_DISABLE_HTTP */
