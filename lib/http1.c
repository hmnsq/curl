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

#جے_بنا_تعریف CURL_DISABLE_HTTP

#ضمن "urldata.h"
#ضمن "http.h"
#ضمن "http1.h"
#ضمن "urlapi-int.h"


#تعریف_کرو H1_MAX_URL_LEN (8 * 1024)

فراغ Curl_h1_req_parse_init(بنية h1_req_parser *parser, size_t max_سطر_len)
{
  memset(parser, 0, حجم(*parser));
  parser->max_سطر_len = max_سطر_len;
  curlx_dyn_init(&parser->scratch, max_سطر_len);
}

فراغ Curl_h1_req_parse_free(بنية h1_req_parser *parser)
{
  إذا(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = عدم;
    parser->پورا = FALSE;
  }
}

ساكن CURLcode trim_سطر(بنية h1_req_parser *parser, صحيح options)
{
  DEBUGASSERT(parser->سطر);
  إذا(parser->سطر_len) {
    إذا(parser->سطر[parser->سطر_len - 1] == '\n')
      --parser->سطر_len;
    إذا(parser->سطر_len) {
      إذا(parser->سطر[parser->سطر_len - 1] == '\r')
        --parser->سطر_len;
      وإلا إذا(options & H1_PARSE_OPT_STRICT)
        أرجع CURLE_URL_MALFORMAT;
    }
    وإلا إذا(options & H1_PARSE_OPT_STRICT)
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا إذا(options & H1_PARSE_OPT_STRICT)
    أرجع CURLE_URL_MALFORMAT;

  إذا(parser->سطر_len > parser->max_سطر_len) {
    أرجع CURLE_URL_MALFORMAT;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode detect_سطر(بنية h1_req_parser *parser,
                            ثابت uint8_t *buf, ثابت size_t buflen,
                            size_t *pnread)
{
  ثابت حرف *سطر_سرا;

  DEBUGASSERT(!parser->سطر);
  *pnread = 0;
  سطر_سرا = memchr(buf, '\n', buflen);
  إذا(!سطر_سرا)
    أرجع CURLE_AGAIN;
  parser->سطر = (ثابت حرف *)buf;
  parser->سطر_len = سطر_سرا - parser->سطر + 1;
  *pnread = parser->سطر_len;
  أرجع CURLE_OK;
}

ساكن CURLcode اگلا_سطر(بنية h1_req_parser *parser,
                          ثابت uint8_t *buf, ثابت size_t buflen, صحيح options,
                          size_t *pnread)
{
  CURLcode نتیجہ;

  *pnread = 0;
  إذا(parser->سطر) {
    parser->سطر = عدم;
    parser->سطر_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  نتیجہ = detect_سطر(parser, buf, buflen, pnread);
  إذا(!نتیجہ) {
    إذا(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      نتیجہ = curlx_dyn_addn(&parser->scratch, parser->سطر,
                              parser->سطر_len);
      إذا(نتیجہ)
        أرجع نتیجہ;
      parser->سطر = curlx_dyn_ptr(&parser->scratch);
      parser->سطر_len = curlx_dyn_len(&parser->scratch);
    }
    نتیجہ = trim_سطر(parser, options);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  وإلا إذا(نتیجہ == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    نتیجہ = curlx_dyn_addn(&parser->scratch, (ثابت بنانشانی حرف *)buf,
                            buflen);
    *pnread = buflen;
  }
  أرجع نتیجہ;
}

ساكن CURLcode شروع_req(بنية h1_req_parser *parser,
                          ثابت حرف *scheme_default,
                          ثابت حرف *custom_طریقہ,
                          صحيح options)
{
  ثابت حرف *p, *m, *نشانہ, *hv, *scheme, *authority, *رستہ;
  size_t m_len, نشانہ_len, hv_len, scheme_len, authority_len, رستہ_len;
  size_t i;
  CURLU *url = عدم;
  CURLcode نتیجہ = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  إذا(custom_طریقہ && custom_طریقہ[0] &&
     !لڑی_مقابلہ(custom_طریقہ, parser->سطر, لڑی_لمبائی(custom_طریقہ))) {
    p = parser->سطر + لڑی_لمبائی(custom_طریقہ);
  }
  وإلا {
    p = memchr(parser->سطر, ' ', parser->سطر_len);
    إذا(!p || p == parser->سطر)
      اذهب باہرلا;
  }

  m = parser->سطر;
  m_len = p - parser->سطر;
  نشانہ = p + 1;
  نشانہ_len = hv_len = 0;
  hv = عدم;

  /* URL may contain spaces so scan backwards */
  لكل(i = parser->سطر_len; i > m_len; --i) {
    إذا(parser->سطر[i] == ' ') {
      hv = &parser->سطر[i + 1];
      hv_len = parser->سطر_len - i;
      نشانہ_len = (hv - نشانہ) - 1;
      اكسر;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  إذا(!نشانہ_len || !hv_len)
    اذهب باہرلا;

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
  scheme = authority = رستہ = عدم;
  scheme_len = authority_len = رستہ_len = 0;

  إذا(نشانہ_len == 1 && نشانہ[0] == '*') {
    /* asterisk-form */
    رستہ = نشانہ;
    رستہ_len = نشانہ_len;
  }
  وإلا إذا(!لڑی_مقابلہ("CONNECT", m, m_len)) {
    /* authority-form */
    authority = نشانہ;
    authority_len = نشانہ_len;
  }
  وإلا إذا(نشانہ[0] == '/') {
    /* origin-form */
    رستہ = نشانہ;
    رستہ_len = نشانہ_len;
  }
  وإلا {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    حرف tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    رستہ = نشانہ;
    رستہ_len = نشانہ_len;

    /* URL parser wants null-termination */
    إذا(نشانہ_len >= حجم(tmp))
      اذهب باہرلا;
    حافظہ_نقل(tmp, نشانہ, نشانہ_len);
    tmp[نشانہ_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    إذا(Curl_is_absolute_url(tmp, عدم, 0, FALSE)) {
      بنانشانی صحيح url_options;

      url = curl_url();
      إذا(!url) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_رستہ_AS_IS |
                     CURLU_NO_DEFAULT_پورٹ);
      إذا(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_سیٹ(url, CURLUPART_URL, tmp, url_options);
      إذا(uc) {
        اذهب باہرلا;
      }
    }

    إذا(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      اذهب باہرلا;
    }
  }

  إذا(url) {
    نتیجہ = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  وإلا {
    إذا(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = لڑی_لمبائی(scheme_default);
    }
    نتیجہ = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, رستہ, رستہ_len);
  }

باہرلا:
  curl_url_cleanup(url);
  أرجع نتیجہ;
}

CURLcode Curl_h1_req_parse_read(بنية h1_req_parser *parser,
                                ثابت uint8_t *buf, size_t buflen,
                                ثابت حرف *scheme_default,
                                ثابت حرف *custom_طریقہ,
                                صحيح options, size_t *pnread)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  إذا(!buf)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  طالما(!parser->پورا) {
    نتیجہ = اگلا_سطر(parser, buf, buflen, options, &nread);
    إذا(نتیجہ) {
      إذا(نتیجہ == CURLE_AGAIN)
        نتیجہ = CURLE_OK;
      اذهب باہرلا;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    إذا(!parser->سطر) {
      /* consumed bytes, but line not complete */
      إذا(!buflen)
        اذهب باہرلا;
    }
    وإلا إذا(!parser->req) {
      نتیجہ = شروع_req(parser, scheme_default, custom_طریقہ, options);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
    وإلا إذا(parser->سطر_len == 0) {
      /* last, empty line, we are finished */
      إذا(!parser->req) {
        نتیجہ = CURLE_URL_MALFORMAT;
        اذهب باہرلا;
      }
      parser->پورا = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    وإلا {
      نتیجہ = Curl_dynhds_h1_جوڑو_سطر(&parser->req->سرخیاں,
                                       parser->سطر, parser->سطر_len);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
  }

باہرلا:
  أرجع نتیجہ;
}

CURLcode Curl_h1_req_write_سر(بنية httpreq *req, صحيح http_minor,
                                بنية dynbuf *dbuf)
{
  CURLcode نتیجہ;

  نتیجہ = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->طریقہ,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->رستہ ? req->رستہ : "",
                          http_minor);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = Curl_dynhds_h1_dprint(&req->سرخیاں, dbuf);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

باہرلا:
  أرجع نتیجہ;
}

#جے_انت /* !CURL_DISABLE_HTTP */
