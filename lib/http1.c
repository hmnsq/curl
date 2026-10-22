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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP

#شامل "urldata.h"
#شامل "http.h"
#شامل "http1.h"
#شامل "urlapi-int.h"


#تعریف_کرو H1_MAX_URL_LEN (8 * 1024)

خالی Curl_h1_req_parse_init(ساخت h1_req_parser *parser, size_t max_سطر_len)
{
  memset(parser, 0, حجم(*parser));
  parser->max_سطر_len = max_سطر_len;
  curlx_dyn_init(&parser->scratch, max_سطر_len);
}

خالی Curl_h1_req_parse_free(ساخت h1_req_parser *parser)
{
  اگر(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = تہی;
    parser->مکمل = FALSE;
  }
}

ساکن CURLcode trim_سطر(ساخت h1_req_parser *parser, صحیح options)
{
  DEBUGASSERT(parser->سطر);
  اگر(parser->سطر_len) {
    اگر(parser->سطر[parser->سطر_len - 1] == '\n')
      --parser->سطر_len;
    اگر(parser->سطر_len) {
      اگر(parser->سطر[parser->سطر_len - 1] == '\r')
        --parser->سطر_len;
      ورنہ اگر(options & H1_PARSE_OPT_STRICT)
        واپس CURLE_URL_MALFORMAT;
    }
    ورنہ اگر(options & H1_PARSE_OPT_STRICT)
      واپس CURLE_URL_MALFORMAT;
  }
  ورنہ اگر(options & H1_PARSE_OPT_STRICT)
    واپس CURLE_URL_MALFORMAT;

  اگر(parser->سطر_len > parser->max_سطر_len) {
    واپس CURLE_URL_MALFORMAT;
  }
  واپس CURLE_OK;
}

ساکن CURLcode detect_سطر(ساخت h1_req_parser *parser,
                            ثابت uint8_t *buf, ثابت size_t buflen,
                            size_t *pnread)
{
  ثابت حرف *سطر_انتہا;

  DEBUGASSERT(!parser->سطر);
  *pnread = 0;
  سطر_انتہا = memchr(buf, '\n', buflen);
  اگر(!سطر_انتہا)
    واپس CURLE_AGAIN;
  parser->سطر = (ثابت حرف *)buf;
  parser->سطر_len = سطر_انتہا - parser->سطر + 1;
  *pnread = parser->سطر_len;
  واپس CURLE_OK;
}

ساکن CURLcode اگلا_سطر(ساخت h1_req_parser *parser,
                          ثابت uint8_t *buf, ثابت size_t buflen, صحیح options,
                          size_t *pnread)
{
  CURLcode نتیجہ;

  *pnread = 0;
  اگر(parser->سطر) {
    parser->سطر = تہی;
    parser->سطر_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  نتیجہ = detect_سطر(parser, buf, buflen, pnread);
  اگر(!نتیجہ) {
    اگر(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      نتیجہ = curlx_dyn_addn(&parser->scratch, parser->سطر,
                              parser->سطر_len);
      اگر(نتیجہ)
        واپس نتیجہ;
      parser->سطر = curlx_dyn_ptr(&parser->scratch);
      parser->سطر_len = curlx_dyn_len(&parser->scratch);
    }
    نتیجہ = trim_سطر(parser, options);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  ورنہ اگر(نتیجہ == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    نتیجہ = curlx_dyn_addn(&parser->scratch, (ثابت غیرعلامتی حرف *)buf,
                            buflen);
    *pnread = buflen;
  }
  واپس نتیجہ;
}

ساکن CURLcode آغاز_req(ساخت h1_req_parser *parser,
                          ثابت حرف *scheme_default,
                          ثابت حرف *custom_طریقہ,
                          صحیح options)
{
  ثابت حرف *p, *m, *ہدف, *hv, *scheme, *authority, *راستہ;
  size_t m_len, ہدف_len, hv_len, scheme_len, authority_len, راستہ_len;
  size_t i;
  CURLU *url = تہی;
  CURLcode نتیجہ = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  اگر(custom_طریقہ && custom_طریقہ[0] &&
     !لڑی_موازنہ(custom_طریقہ, parser->سطر, لڑی_لمبائی(custom_طریقہ))) {
    p = parser->سطر + لڑی_لمبائی(custom_طریقہ);
  }
  ورنہ {
    p = memchr(parser->سطر, ' ', parser->سطر_len);
    اگر(!p || p == parser->سطر)
      جاؤ برآمد;
  }

  m = parser->سطر;
  m_len = p - parser->سطر;
  ہدف = p + 1;
  ہدف_len = hv_len = 0;
  hv = تہی;

  /* URL may contain spaces so scan backwards */
  برائے(i = parser->سطر_len; i > m_len; --i) {
    اگر(parser->سطر[i] == ' ') {
      hv = &parser->سطر[i + 1];
      hv_len = parser->سطر_len - i;
      ہدف_len = (hv - ہدف) - 1;
      توڑو;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  اگر(!ہدف_len || !hv_len)
    جاؤ برآمد;

  (خالی)hv;

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
  scheme = authority = راستہ = تہی;
  scheme_len = authority_len = راستہ_len = 0;

  اگر(ہدف_len == 1 && ہدف[0] == '*') {
    /* asterisk-form */
    راستہ = ہدف;
    راستہ_len = ہدف_len;
  }
  ورنہ اگر(!لڑی_موازنہ("CONNECT", m, m_len)) {
    /* authority-form */
    authority = ہدف;
    authority_len = ہدف_len;
  }
  ورنہ اگر(ہدف[0] == '/') {
    /* origin-form */
    راستہ = ہدف;
    راستہ_len = ہدف_len;
  }
  ورنہ {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    حرف tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    راستہ = ہدف;
    راستہ_len = ہدف_len;

    /* URL parser wants null-termination */
    اگر(ہدف_len >= حجم(tmp))
      جاؤ برآمد;
    حافظہ_نقل(tmp, ہدف, ہدف_len);
    tmp[ہدف_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    اگر(Curl_is_absolute_url(tmp, تہی, 0, FALSE)) {
      غیرعلامتی صحیح url_options;

      url = curl_url();
      اگر(!url) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_راستہ_AS_IS |
                     CURLU_NO_DEFAULT_پورٹ);
      اگر(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_سیٹ(url, CURLUPART_URL, tmp, url_options);
      اگر(uc) {
        جاؤ برآمد;
      }
    }

    اگر(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      جاؤ برآمد;
    }
  }

  اگر(url) {
    نتیجہ = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  ورنہ {
    اگر(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = لڑی_لمبائی(scheme_default);
    }
    نتیجہ = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, راستہ, راستہ_len);
  }

برآمد:
  curl_url_cleanup(url);
  واپس نتیجہ;
}

CURLcode Curl_h1_req_parse_read(ساخت h1_req_parser *parser,
                                ثابت uint8_t *buf, size_t buflen,
                                ثابت حرف *scheme_default,
                                ثابت حرف *custom_طریقہ,
                                صحیح options, size_t *pnread)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  اگر(!buf)
    واپس CURLE_BAD_FUNCTION_دلیل;

  جب(!parser->مکمل) {
    نتیجہ = اگلا_سطر(parser, buf, buflen, options, &nread);
    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_AGAIN)
        نتیجہ = CURLE_OK;
      جاؤ برآمد;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    اگر(!parser->سطر) {
      /* consumed bytes, but line not complete */
      اگر(!buflen)
        جاؤ برآمد;
    }
    ورنہ اگر(!parser->req) {
      نتیجہ = آغاز_req(parser, scheme_default, custom_طریقہ, options);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
    ورنہ اگر(parser->سطر_len == 0) {
      /* last, empty line, we are finished */
      اگر(!parser->req) {
        نتیجہ = CURLE_URL_MALFORMAT;
        جاؤ برآمد;
      }
      parser->مکمل = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    ورنہ {
      نتیجہ = Curl_dynhds_h1_جوڑو_سطر(&parser->req->سرخیاں,
                                       parser->سطر, parser->سطر_len);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
  }

برآمد:
  واپس نتیجہ;
}

CURLcode Curl_h1_req_write_سر(ساخت httpreq *req, صحیح http_minor,
                                ساخت dynbuf *dbuf)
{
  CURLcode نتیجہ;

  نتیجہ = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->طریقہ,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->راستہ ? req->راستہ : "",
                          http_minor);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_dynhds_h1_dprint(&req->سرخیاں, dbuf);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

برآمد:
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_HTTP */
