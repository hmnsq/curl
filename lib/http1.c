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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "http.h"
#அந்தர்பாவ "http1.h"
#அந்தர்பாவ "urlapi-int.h"


#define H1_MAX_URL_LEN (8 * 1024)

வெறுமை Curl_h1_req_parse_init(கட்டமைப்பு h1_req_parser *parser, size_t max_வரி_len)
{
  memset(parser, 0, அளவு(*parser));
  parser->max_வரி_len = max_வரி_len;
  curlx_dyn_init(&parser->scratch, max_வரி_len);
}

வெறுமை Curl_h1_req_parse_free(கட்டமைப்பு h1_req_parser *parser)
{
  எனில்(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = வெற்று;
    parser->done = FALSE;
  }
}

நிலையான CURLcode trim_வரி(கட்டமைப்பு h1_req_parser *parser, முழு options)
{
  DEBUGASSERT(parser->வரி);
  எனில்(parser->வரி_len) {
    எனில்(parser->வரி[parser->வரி_len - 1] == '\n')
      --parser->வரி_len;
    எனில்(parser->வரி_len) {
      எனில்(parser->வரி[parser->வரி_len - 1] == '\r')
        --parser->வரி_len;
      இல்லையெனில் எனில்(options & H1_PARSE_OPT_STRICT)
        திரும்பு CURLE_URL_MALFORMAT;
    }
    இல்லையெனில் எனில்(options & H1_PARSE_OPT_STRICT)
      திரும்பு CURLE_URL_MALFORMAT;
  }
  இல்லையெனில் எனில்(options & H1_PARSE_OPT_STRICT)
    திரும்பு CURLE_URL_MALFORMAT;

  எனில்(parser->வரி_len > parser->max_வரி_len) {
    திரும்பு CURLE_URL_MALFORMAT;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode detect_வரி(கட்டமைப்பு h1_req_parser *parser,
                            மாறிலி uint8_t *buf, மாறிலி size_t buflen,
                            size_t *pnread)
{
  மாறிலி எழுத்து *வரி_end;

  DEBUGASSERT(!parser->வரி);
  *pnread = 0;
  வரி_end = memchr(buf, '\n', buflen);
  எனில்(!வரி_end)
    திரும்பு CURLE_AGAIN;
  parser->வரி = (மாறிலி எழுத்து *)buf;
  parser->வரி_len = வரி_end - parser->வரி + 1;
  *pnread = parser->வரி_len;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode next_வரி(கட்டமைப்பு h1_req_parser *parser,
                          மாறிலி uint8_t *buf, மாறிலி size_t buflen, முழு options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  எனில்(parser->வரி) {
    parser->வரி = வெற்று;
    parser->வரி_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_வரி(parser, buf, buflen, pnread);
  எனில்(!result) {
    எனில்(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->வரி,
                              parser->வரி_len);
      எனில்(result)
        திரும்பு result;
      parser->வரி = curlx_dyn_ptr(&parser->scratch);
      parser->வரி_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_வரி(parser, options);
    எனில்(result)
      திரும்பு result;
  }
  இல்லையெனில் எனில்(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (மாறிலி unsigned எழுத்து *)buf,
                            buflen);
    *pnread = buflen;
  }
  திரும்பு result;
}

நிலையான CURLcode start_req(கட்டமைப்பு h1_req_parser *parser,
                          மாறிலி எழுத்து *scheme_default,
                          மாறிலி எழுத்து *custom_method,
                          முழு options)
{
  மாறிலி எழுத்து *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = வெற்று;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  எனில்(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->வரி, strlen(custom_method))) {
    p = parser->வரி + strlen(custom_method);
  }
  இல்லையெனில் {
    p = memchr(parser->வரி, ' ', parser->வரி_len);
    எனில்(!p || p == parser->வரி)
      செல் out;
  }

  m = parser->வரி;
  m_len = p - parser->வரி;
  target = p + 1;
  target_len = hv_len = 0;
  hv = வெற்று;

  /* URL may contain spaces so scan backwards */
  க்கு(i = parser->வரி_len; i > m_len; --i) {
    எனில்(parser->வரி[i] == ' ') {
      hv = &parser->வரி[i + 1];
      hv_len = parser->வரி_len - i;
      target_len = (hv - target) - 1;
      நிறுத்து;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  எனில்(!target_len || !hv_len)
    செல் out;

  (வெறுமை)hv;

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
  scheme = authority = path = வெற்று;
  scheme_len = authority_len = path_len = 0;

  எனில்(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  இல்லையெனில் எனில்(!strncmp("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  இல்லையெனில் எனில்(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  இல்லையெனில் {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    எழுத்து tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    எனில்(target_len >= அளவு(tmp))
      செல் out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    எனில்(Curl_is_absolute_url(tmp, வெற்று, 0, FALSE)) {
      unsigned முழு url_options;

      url = curl_url();
      எனில்(!url) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      எனில்(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      எனில்(uc) {
        செல் out;
      }
    }

    எனில்(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      செல் out;
    }
  }

  எனில்(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  இல்லையெனில் {
    எனில்(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = strlen(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  திரும்பு result;
}

CURLcode Curl_h1_req_parse_read(கட்டமைப்பு h1_req_parser *parser,
                                மாறிலி uint8_t *buf, size_t buflen,
                                மாறிலி எழுத்து *scheme_default,
                                மாறிலி எழுத்து *custom_method,
                                முழு options, size_t *pnread)
{
  CURLcode result = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  எனில்(!buf)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  போது(!parser->done) {
    result = next_வரி(parser, buf, buflen, options, &nread);
    எனில்(result) {
      எனில்(result == CURLE_AGAIN)
        result = CURLE_OK;
      செல் out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    எனில்(!parser->வரி) {
      /* consumed bytes, but line not complete */
      எனில்(!buflen)
        செல் out;
    }
    இல்லையெனில் எனில்(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      எனில்(result)
        செல் out;
    }
    இல்லையெனில் எனில்(parser->வரி_len == 0) {
      /* last, empty line, we are finished */
      எனில்(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        செல் out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    இல்லையெனில் {
      result = Curl_dynhds_h1_add_வரி(&parser->req->headers,
                                       parser->வரி, parser->வரி_len);
      எனில்(result)
        செல் out;
    }
  }

out:
  திரும்பு result;
}

CURLcode Curl_h1_req_write_head(கட்டமைப்பு httpreq *req, முழு http_minor,
                                கட்டமைப்பு dynbuf *dbuf)
{
  CURLcode result;

  result = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  எனில்(result)
    செல் out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  எனில்(result)
    செல் out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  திரும்பு result;
}

#endif /* !CURL_DISABLE_HTTP */
