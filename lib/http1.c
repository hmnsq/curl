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
#অন্তর্ভাৱ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "http.h"
#অন্তর্ভাৱ "http1.h"
#অন্তর্ভাৱ "urlapi-int.h"


#define H1_MAX_URL_LEN (8 * 1024)

শূন্য Curl_h1_req_parse_init(সংরচনা h1_req_parser *parser, size_t max_সাৰি_len)
{
  memset(parser, 0, পরিমাণ(*parser));
  parser->max_সাৰি_len = max_সাৰি_len;
  curlx_dyn_init(&parser->scratch, max_সাৰি_len);
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

স্থির CURLcode trim_সাৰি(সংরচনা h1_req_parser *parser, পূর্ণাঙ্ক options)
{
  DEBUGASSERT(parser->সাৰি);
  যদি(parser->সাৰি_len) {
    যদি(parser->সাৰি[parser->সাৰি_len - 1] == '\n')
      --parser->সাৰি_len;
    যদি(parser->সাৰি_len) {
      যদি(parser->সাৰি[parser->সাৰি_len - 1] == '\r')
        --parser->সাৰি_len;
      অন্যথা যদি(options & H1_PARSE_OPT_STRICT)
        ঘূৰাই CURLE_URL_MALFORMAT;
    }
    অন্যথা যদি(options & H1_PARSE_OPT_STRICT)
      ঘূৰাই CURLE_URL_MALFORMAT;
  }
  অন্যথা যদি(options & H1_PARSE_OPT_STRICT)
    ঘূৰাই CURLE_URL_MALFORMAT;

  যদি(parser->সাৰি_len > parser->max_সাৰি_len) {
    ঘূৰাই CURLE_URL_MALFORMAT;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode detect_সাৰি(সংরচনা h1_req_parser *parser,
                            ধ্রুৱ uint8_t *buf, ধ্রুৱ size_t buflen,
                            size_t *pnread)
{
  ধ্রুৱ ৱর্ণ *সাৰি_end;

  DEBUGASSERT(!parser->সাৰি);
  *pnread = 0;
  সাৰি_end = memchr(buf, '\n', buflen);
  যদি(!সাৰি_end)
    ঘূৰাই CURLE_AGAIN;
  parser->সাৰি = (ধ্রুৱ ৱর্ণ *)buf;
  parser->সাৰি_len = সাৰি_end - parser->সাৰি + 1;
  *pnread = parser->সাৰি_len;
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode next_সাৰি(সংরচনা h1_req_parser *parser,
                          ধ্রুৱ uint8_t *buf, ধ্রুৱ size_t buflen, পূর্ণাঙ্ক options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  যদি(parser->সাৰি) {
    parser->সাৰি = রিক্ত;
    parser->সাৰি_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_সাৰি(parser, buf, buflen, pnread);
  যদি(!result) {
    যদি(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->সাৰি,
                              parser->সাৰি_len);
      যদি(result)
        ঘূৰাই result;
      parser->সাৰি = curlx_dyn_ptr(&parser->scratch);
      parser->সাৰি_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_সাৰি(parser, options);
    যদি(result)
      ঘূৰাই result;
  }
  অন্যথা যদি(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (ধ্রুৱ unsigned ৱর্ণ *)buf,
                            buflen);
    *pnread = buflen;
  }
  ঘূৰাই result;
}

স্থির CURLcode start_req(সংরচনা h1_req_parser *parser,
                          ধ্রুৱ ৱর্ণ *scheme_default,
                          ধ্রুৱ ৱর্ণ *custom_method,
                          পূর্ণাঙ্ক options)
{
  ধ্রুৱ ৱর্ণ *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = রিক্ত;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  যদি(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->সাৰি, strlen(custom_method))) {
    p = parser->সাৰি + strlen(custom_method);
  }
  অন্যথা {
    p = memchr(parser->সাৰি, ' ', parser->সাৰি_len);
    যদি(!p || p == parser->সাৰি)
      গচ্ছ out;
  }

  m = parser->সাৰি;
  m_len = p - parser->সাৰি;
  target = p + 1;
  target_len = hv_len = 0;
  hv = রিক্ত;

  /* URL may contain spaces so scan backwards */
  বাবে(i = parser->সাৰি_len; i > m_len; --i) {
    যদি(parser->সাৰি[i] == ' ') {
      hv = &parser->সাৰি[i + 1];
      hv_len = parser->সাৰি_len - i;
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
    ৱর্ণ tmp[H1_MAX_URL_LEN];

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
  ঘূৰাই result;
}

CURLcode Curl_h1_req_parse_read(সংরচনা h1_req_parser *parser,
                                ধ্রুৱ uint8_t *buf, size_t buflen,
                                ধ্রুৱ ৱর্ণ *scheme_default,
                                ধ্রুৱ ৱর্ণ *custom_method,
                                পূর্ণাঙ্ক options, size_t *pnread)
{
  CURLcode result = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  যদি(!buf)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  যেতিয়ালৈ(!parser->done) {
    result = next_সাৰি(parser, buf, buflen, options, &nread);
    যদি(result) {
      যদি(result == CURLE_AGAIN)
        result = CURLE_OK;
      গচ্ছ out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    যদি(!parser->সাৰি) {
      /* consumed bytes, but line not complete */
      যদি(!buflen)
        গচ্ছ out;
    }
    অন্যথা যদি(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      যদি(result)
        গচ্ছ out;
    }
    অন্যথা যদি(parser->সাৰি_len == 0) {
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
      result = Curl_dynhds_h1_add_সাৰি(&parser->req->headers,
                                       parser->সাৰি, parser->সাৰি_len);
      যদি(result)
        গচ্ছ out;
    }
  }

out:
  ঘূৰাই result;
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
  ঘূৰাই result;
}

#endif /* !CURL_DISABLE_HTTP */
