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

#که_نه_تعریف_شوی CURL_DISABLE_HTTP

#ضمن "urldata.h"
#ضمن "http.h"
#ضمن "http1.h"
#ضمن "urlapi-int.h"


#تعریف_کړه H1_MAX_URL_LEN (8 * 1024)

فراغ Curl_h1_req_parse_init(بنية h1_req_parser *parser, size_t max_کرښه_len)
{
  memset(parser, 0, حجم(*parser));
  parser->max_کرښه_len = max_کرښه_len;
  curlx_dyn_init(&parser->scratch, max_کرښه_len);
}

فراغ Curl_h1_req_parse_free(بنية h1_req_parser *parser)
{
  إذا(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = عدم;
    parser->بشپړ = FALSE;
  }
}

ساكن CURLcode trim_کرښه(بنية h1_req_parser *parser, صحيح options)
{
  DEBUGASSERT(parser->کرښه);
  إذا(parser->کرښه_len) {
    إذا(parser->کرښه[parser->کرښه_len - 1] == '\n')
      --parser->کرښه_len;
    إذا(parser->کرښه_len) {
      إذا(parser->کرښه[parser->کرښه_len - 1] == '\r')
        --parser->کرښه_len;
      وإلا إذا(options & H1_PARSE_OPT_STRICT)
        أرجع CURLE_URL_MALFORMAT;
    }
    وإلا إذا(options & H1_PARSE_OPT_STRICT)
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا إذا(options & H1_PARSE_OPT_STRICT)
    أرجع CURLE_URL_MALFORMAT;

  إذا(parser->کرښه_len > parser->max_کرښه_len) {
    أرجع CURLE_URL_MALFORMAT;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode detect_کرښه(بنية h1_req_parser *parser,
                            ثابت uint8_t *buf, ثابت size_t buflen,
                            size_t *pnread)
{
  ثابت حرف *کرښه_پای_ټکی;

  DEBUGASSERT(!parser->کرښه);
  *pnread = 0;
  کرښه_پای_ټکی = memchr(buf, '\n', buflen);
  إذا(!کرښه_پای_ټکی)
    أرجع CURLE_AGAIN;
  parser->کرښه = (ثابت حرف *)buf;
  parser->کرښه_len = کرښه_پای_ټکی - parser->کرښه + 1;
  *pnread = parser->کرښه_len;
  أرجع CURLE_OK;
}

ساكن CURLcode بل_کرښه(بنية h1_req_parser *parser,
                          ثابت uint8_t *buf, ثابت size_t buflen, صحيح options,
                          size_t *pnread)
{
  CURLcode پایله;

  *pnread = 0;
  إذا(parser->کرښه) {
    parser->کرښه = عدم;
    parser->کرښه_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  پایله = detect_کرښه(parser, buf, buflen, pnread);
  إذا(!پایله) {
    إذا(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      پایله = curlx_dyn_addn(&parser->scratch, parser->کرښه,
                              parser->کرښه_len);
      إذا(پایله)
        أرجع پایله;
      parser->کرښه = curlx_dyn_ptr(&parser->scratch);
      parser->کرښه_len = curlx_dyn_len(&parser->scratch);
    }
    پایله = trim_کرښه(parser, options);
    إذا(پایله)
      أرجع پایله;
  }
  وإلا إذا(پایله == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    پایله = curlx_dyn_addn(&parser->scratch, (ثابت بینښه حرف *)buf,
                            buflen);
    *pnread = buflen;
  }
  أرجع پایله;
}

ساكن CURLcode پیل_req(بنية h1_req_parser *parser,
                          ثابت حرف *scheme_default,
                          ثابت حرف *custom_لاره,
                          صحيح options)
{
  ثابت حرف *p, *m, *موخه, *hv, *scheme, *authority, *لار;
  size_t m_len, موخه_len, hv_len, scheme_len, authority_len, لار_len;
  size_t i;
  CURLU *url = عدم;
  CURLcode پایله = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  إذا(custom_لاره && custom_لاره[0] &&
     !مزي_پرتله(custom_لاره, parser->کرښه, مزي_اوږدوالی(custom_لاره))) {
    p = parser->کرښه + مزي_اوږدوالی(custom_لاره);
  }
  وإلا {
    p = memchr(parser->کرښه, ' ', parser->کرښه_len);
    إذا(!p || p == parser->کرښه)
      اذهب بهر;
  }

  m = parser->کرښه;
  m_len = p - parser->کرښه;
  موخه = p + 1;
  موخه_len = hv_len = 0;
  hv = عدم;

  /* URL may contain spaces so scan backwards */
  لكل(i = parser->کرښه_len; i > m_len; --i) {
    إذا(parser->کرښه[i] == ' ') {
      hv = &parser->کرښه[i + 1];
      hv_len = parser->کرښه_len - i;
      موخه_len = (hv - موخه) - 1;
      اكسر;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  إذا(!موخه_len || !hv_len)
    اذهب بهر;

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
  scheme = authority = لار = عدم;
  scheme_len = authority_len = لار_len = 0;

  إذا(موخه_len == 1 && موخه[0] == '*') {
    /* asterisk-form */
    لار = موخه;
    لار_len = موخه_len;
  }
  وإلا إذا(!مزي_پرتله("CONNECT", m, m_len)) {
    /* authority-form */
    authority = موخه;
    authority_len = موخه_len;
  }
  وإلا إذا(موخه[0] == '/') {
    /* origin-form */
    لار = موخه;
    لار_len = موخه_len;
  }
  وإلا {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    حرف tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    لار = موخه;
    لار_len = موخه_len;

    /* URL parser wants null-termination */
    إذا(موخه_len >= حجم(tmp))
      اذهب بهر;
    حافظه_نقل(tmp, موخه, موخه_len);
    tmp[موخه_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    إذا(Curl_is_absolute_url(tmp, عدم, 0, FALSE)) {
      بینښه صحيح url_options;

      url = curl_url();
      إذا(!url) {
        پایله = CURLE_بهر_د_حافظه;
        اذهب بهر;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_لار_AS_IS |
                     CURLU_NO_DEFAULT_پورټ);
      إذا(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_سېټ(url, CURLUPART_URL, tmp, url_options);
      إذا(uc) {
        اذهب بهر;
      }
    }

    إذا(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      اذهب بهر;
    }
  }

  إذا(url) {
    پایله = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  وإلا {
    إذا(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = مزي_اوږدوالی(scheme_default);
    }
    پایله = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, لار, لار_len);
  }

بهر:
  curl_url_cleanup(url);
  أرجع پایله;
}

CURLcode Curl_h1_req_parse_read(بنية h1_req_parser *parser,
                                ثابت uint8_t *buf, size_t buflen,
                                ثابت حرف *scheme_default,
                                ثابت حرف *custom_لاره,
                                صحيح options, size_t *pnread)
{
  CURLcode پایله = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  إذا(!buf)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  طالما(!parser->بشپړ) {
    پایله = بل_کرښه(parser, buf, buflen, options, &nread);
    إذا(پایله) {
      إذا(پایله == CURLE_AGAIN)
        پایله = CURLE_OK;
      اذهب بهر;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    إذا(!parser->کرښه) {
      /* consumed bytes, but line not complete */
      إذا(!buflen)
        اذهب بهر;
    }
    وإلا إذا(!parser->req) {
      پایله = پیل_req(parser, scheme_default, custom_لاره, options);
      إذا(پایله)
        اذهب بهر;
    }
    وإلا إذا(parser->کرښه_len == 0) {
      /* last, empty line, we are finished */
      إذا(!parser->req) {
        پایله = CURLE_URL_MALFORMAT;
        اذهب بهر;
      }
      parser->بشپړ = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    وإلا {
      پایله = Curl_dynhds_h1_جمع_کړه_کرښه(&parser->req->سرلیکونه,
                                       parser->کرښه, parser->کرښه_len);
      إذا(پایله)
        اذهب بهر;
    }
  }

بهر:
  أرجع پایله;
}

CURLcode Curl_h1_req_write_سر(بنية httpreq *req, صحيح http_minor,
                                بنية dynbuf *dbuf)
{
  CURLcode پایله;

  پایله = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->لاره,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->لار ? req->لار : "",
                          http_minor);
  إذا(پایله)
    اذهب بهر;

  پایله = Curl_dynhds_h1_dprint(&req->سرلیکونه, dbuf);
  إذا(پایله)
    اذهب بهر;

  پایله = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

بهر:
  أرجع پایله;
}

#د_که_پای /* !CURL_DISABLE_HTTP */
