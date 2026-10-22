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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "http1.h"
#অন্তর্ভাব "urlapi-int.h"


#define H1_MAX_URL_LEN (8 * 1024)

শূন্য Curl_h1_req_parse_init(সংরচনা h1_req_parser *parser, size_t max_সারি_len)
{
  memset(parser, 0, পরিমাণ(*parser));
  parser->max_সারি_len = max_সারি_len;
  curlx_dyn_init(&parser->scratch, max_সারি_len);
}

শূন্য Curl_h1_req_parse_free(সংরচনা h1_req_parser *parser)
{
  যদি(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = রিক্ত;
    parser->done = FALSE;
  }
}

স্থির CURLcode trim_সারি(সংরচনা h1_req_parser *parser, পূর্ণাঙ্ক options)
{
  DEBUGASSERT(parser->সারি);
  যদি(parser->সারি_len) {
    যদি(parser->সারি[parser->সারি_len - 1] == '\n')
      --parser->সারি_len;
    যদি(parser->সারি_len) {
      যদি(parser->সারি[parser->সারি_len - 1] == '\r')
        --parser->সারি_len;
      অন্যথা যদি(options & H1_PARSE_OPT_STRICT)
        প্রত্যাগম CURLE_URL_MALFORMAT;
    }
    অন্যথা যদি(options & H1_PARSE_OPT_STRICT)
      প্রত্যাগম CURLE_URL_MALFORMAT;
  }
  অন্যথা যদি(options & H1_PARSE_OPT_STRICT)
    প্রত্যাগম CURLE_URL_MALFORMAT;

  যদি(parser->সারি_len > parser->max_সারি_len) {
    প্রত্যাগম CURLE_URL_MALFORMAT;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode detect_সারি(সংরচনা h1_req_parser *parser,
                            ধ্রুব uint8_t *buf, ধ্রুব size_t buflen,
                            size_t *pnread)
{
  ধ্রুব বর্ণ *সারি_end;

  DEBUGASSERT(!parser->সারি);
  *pnread = 0;
  সারি_end = memchr(buf, '\n', buflen);
  যদি(!সারি_end)
    প্রত্যাগম CURLE_AGAIN;
  parser->সারি = (ধ্রুব বর্ণ *)buf;
  parser->সারি_len = সারি_end - parser->সারি + 1;
  *pnread = parser->সারি_len;
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode next_সারি(সংরচনা h1_req_parser *parser,
                          ধ্রুব uint8_t *buf, ধ্রুব size_t buflen, পূর্ণাঙ্ক options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  যদি(parser->সারি) {
    parser->সারি = রিক্ত;
    parser->সারি_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_সারি(parser, buf, buflen, pnread);
  যদি(!result) {
    যদি(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->সারি,
                              parser->সারি_len);
      যদি(result)
        প্রত্যাগম result;
      parser->সারি = curlx_dyn_ptr(&parser->scratch);
      parser->সারি_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_সারি(parser, options);
    যদি(result)
      প্রত্যাগম result;
  }
  অন্যথা যদি(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (ধ্রুব unsigned বর্ণ *)buf,
                            buflen);
    *pnread = buflen;
  }
  প্রত্যাগম result;
}

স্থির CURLcode start_req(সংরচনা h1_req_parser *parser,
                          ধ্রুব বর্ণ *scheme_default,
                          ধ্রুব বর্ণ *custom_method,
                          পূর্ণাঙ্ক options)
{
  ধ্রুব বর্ণ *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = রিক্ত;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  যদি(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->সারি, strlen(custom_method))) {
    p = parser->সারি + strlen(custom_method);
  }
  অন্যথা {
    p = memchr(parser->সারি, ' ', parser->সারি_len);
    যদি(!p || p == parser->সারি)
      গচ্ছ out;
  }

  m = parser->সারি;
  m_len = p - parser->সারি;
  target = p + 1;
  target_len = hv_len = 0;
  hv = রিক্ত;

  /* URL may contain spaces so scan backwards */
  প্রতি(i = parser->সারি_len; i > m_len; --i) {
    যদি(parser->সারি[i] == ' ') {
      hv = &parser->সারি[i + 1];
      hv_len = parser->সারি_len - i;
      target_len = (hv - target) - 1;
      ভঙ্গ;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  যদি(!target_len || !hv_len)
    গচ্ছ out;

  (শূন্য)hv;

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
  scheme = authority = path = রিক্ত;
  scheme_len = authority_len = path_len = 0;

  যদি(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  অন্যথা যদি(!strncmp("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  অন্যথা যদি(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  অন্যথা {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    বর্ণ tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    যদি(target_len >= পরিমাণ(tmp))
      গচ্ছ out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    যদি(Curl_is_absolute_url(tmp, রিক্ত, 0, FALSE)) {
      unsigned পূর্ণাঙ্ক url_options;

      url = curl_url();
      যদি(!url) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      যদি(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      যদি(uc) {
        গচ্ছ out;
      }
    }

    যদি(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      গচ্ছ out;
    }
  }

  যদি(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  অন্যথা {
    যদি(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = strlen(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  প্রত্যাগম result;
}

CURLcode Curl_h1_req_parse_read(সংরচনা h1_req_parser *parser,
                                ধ্রুব uint8_t *buf, size_t buflen,
                                ধ্রুব বর্ণ *scheme_default,
                                ধ্রুব বর্ণ *custom_method,
                                পূর্ণাঙ্ক options, size_t *pnread)
{
  CURLcode result = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  যদি(!buf)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  যাবত্(!parser->done) {
    result = next_সারি(parser, buf, buflen, options, &nread);
    যদি(result) {
      যদি(result == CURLE_AGAIN)
        result = CURLE_OK;
      গচ্ছ out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    যদি(!parser->সারি) {
      /* consumed bytes, but line not complete */
      যদি(!buflen)
        গচ্ছ out;
    }
    অন্যথা যদি(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      যদি(result)
        গচ্ছ out;
    }
    অন্যথা যদি(parser->সারি_len == 0) {
      /* last, empty line, we are finished */
      যদি(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        গচ্ছ out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    অন্যথা {
      result = Curl_dynhds_h1_add_সারি(&parser->req->headers,
                                       parser->সারি, parser->সারি_len);
      যদি(result)
        গচ্ছ out;
    }
  }

out:
  প্রত্যাগম result;
}

CURLcode Curl_h1_req_write_head(সংরচনা httpreq *req, পূর্ণাঙ্ক http_minor,
                                সংরচনা dynbuf *dbuf)
{
  CURLcode result;

  result = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  যদি(result)
    গচ্ছ out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  যদি(result)
    গচ্ছ out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  প্রত্যাগম result;
}

#endif /* !CURL_DISABLE_HTTP */
