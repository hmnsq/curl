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

#اگر_تعریف_نشده CURL_DISABLE_HTTP

#ضمن "urldata.h"
#ضمن "http.h"
#ضمن "http1.h"
#ضمن "urlapi-int.h"


#تعریف_کن H1_MAX_URL_LEN (8 * 1024)

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
    parser->انجامشده = FALSE;
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
  ثابت حرف *سطر_انتها;

  DEBUGASSERT(!parser->سطر);
  *pnread = 0;
  سطر_انتها = memchr(buf, '\n', buflen);
  إذا(!سطر_انتها)
    أرجع CURLE_AGAIN;
  parser->سطر = (ثابت حرف *)buf;
  parser->سطر_len = سطر_انتها - parser->سطر + 1;
  *pnread = parser->سطر_len;
  أرجع CURLE_OK;
}

ساكن CURLcode بعدی_سطر(بنية h1_req_parser *parser,
                          ثابت uint8_t *buf, ثابت size_t buflen, صحيح options,
                          size_t *pnread)
{
  CURLcode نتیجه;

  *pnread = 0;
  إذا(parser->سطر) {
    parser->سطر = عدم;
    parser->سطر_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  نتیجه = detect_سطر(parser, buf, buflen, pnread);
  إذا(!نتیجه) {
    إذا(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      نتیجه = curlx_dyn_addn(&parser->scratch, parser->سطر,
                              parser->سطر_len);
      إذا(نتیجه)
        أرجع نتیجه;
      parser->سطر = curlx_dyn_ptr(&parser->scratch);
      parser->سطر_len = curlx_dyn_len(&parser->scratch);
    }
    نتیجه = trim_سطر(parser, options);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  وإلا إذا(نتیجه == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    نتیجه = curlx_dyn_addn(&parser->scratch, (ثابت بیعلامت حرف *)buf,
                            buflen);
    *pnread = buflen;
  }
  أرجع نتیجه;
}

ساكن CURLcode شروع_req(بنية h1_req_parser *parser,
                          ثابت حرف *scheme_default,
                          ثابت حرف *custom_روش,
                          صحيح options)
{
  ثابت حرف *p, *m, *هدف, *hv, *scheme, *authority, *مسیر;
  size_t m_len, هدف_len, hv_len, scheme_len, authority_len, مسیر_len;
  size_t i;
  CURLU *url = عدم;
  CURLcode نتیجه = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  إذا(custom_روش && custom_روش[0] &&
     !مقایسه_رشته(custom_روش, parser->سطر, طول_رشته(custom_روش))) {
    p = parser->سطر + طول_رشته(custom_روش);
  }
  وإلا {
    p = memchr(parser->سطر, ' ', parser->سطر_len);
    إذا(!p || p == parser->سطر)
      اذهب بیرون;
  }

  m = parser->سطر;
  m_len = p - parser->سطر;
  هدف = p + 1;
  هدف_len = hv_len = 0;
  hv = عدم;

  /* URL may contain spaces so scan backwards */
  لكل(i = parser->سطر_len; i > m_len; --i) {
    إذا(parser->سطر[i] == ' ') {
      hv = &parser->سطر[i + 1];
      hv_len = parser->سطر_len - i;
      هدف_len = (hv - هدف) - 1;
      اكسر;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  إذا(!هدف_len || !hv_len)
    اذهب بیرون;

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
  scheme = authority = مسیر = عدم;
  scheme_len = authority_len = مسیر_len = 0;

  إذا(هدف_len == 1 && هدف[0] == '*') {
    /* asterisk-form */
    مسیر = هدف;
    مسیر_len = هدف_len;
  }
  وإلا إذا(!مقایسه_رشته("CONNECT", m, m_len)) {
    /* authority-form */
    authority = هدف;
    authority_len = هدف_len;
  }
  وإلا إذا(هدف[0] == '/') {
    /* origin-form */
    مسیر = هدف;
    مسیر_len = هدف_len;
  }
  وإلا {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    حرف tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    مسیر = هدف;
    مسیر_len = هدف_len;

    /* URL parser wants null-termination */
    إذا(هدف_len >= حجم(tmp))
      اذهب بیرون;
    کپی_حافظه(tmp, هدف, هدف_len);
    tmp[هدف_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    إذا(Curl_is_absolute_url(tmp, عدم, 0, FALSE)) {
      بیعلامت صحيح url_options;

      url = curl_url();
      إذا(!url) {
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_مسیر_AS_IS |
                     CURLU_NO_DEFAULT_پورت);
      إذا(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_مجموعه(url, CURLUPART_URL, tmp, url_options);
      إذا(uc) {
        اذهب بیرون;
      }
    }

    إذا(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      اذهب بیرون;
    }
  }

  إذا(url) {
    نتیجه = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  وإلا {
    إذا(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = طول_رشته(scheme_default);
    }
    نتیجه = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, مسیر, مسیر_len);
  }

بیرون:
  curl_url_cleanup(url);
  أرجع نتیجه;
}

CURLcode Curl_h1_req_parse_read(بنية h1_req_parser *parser,
                                ثابت uint8_t *buf, size_t buflen,
                                ثابت حرف *scheme_default,
                                ثابت حرف *custom_روش,
                                صحيح options, size_t *pnread)
{
  CURLcode نتیجه = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  إذا(!buf)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  طالما(!parser->انجامشده) {
    نتیجه = بعدی_سطر(parser, buf, buflen, options, &nread);
    إذا(نتیجه) {
      إذا(نتیجه == CURLE_AGAIN)
        نتیجه = CURLE_OK;
      اذهب بیرون;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    إذا(!parser->سطر) {
      /* consumed bytes, but line not complete */
      إذا(!buflen)
        اذهب بیرون;
    }
    وإلا إذا(!parser->req) {
      نتیجه = شروع_req(parser, scheme_default, custom_روش, options);
      إذا(نتیجه)
        اذهب بیرون;
    }
    وإلا إذا(parser->سطر_len == 0) {
      /* last, empty line, we are finished */
      إذا(!parser->req) {
        نتیجه = CURLE_URL_MALFORMAT;
        اذهب بیرون;
      }
      parser->انجامشده = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    وإلا {
      نتیجه = Curl_dynhds_h1_جمع_کن_سطر(&parser->req->سرآیندها,
                                       parser->سطر, parser->سطر_len);
      إذا(نتیجه)
        اذهب بیرون;
    }
  }

بیرون:
  أرجع نتیجه;
}

CURLcode Curl_h1_req_write_سر(بنية httpreq *req, صحيح http_minor,
                                بنية dynbuf *dbuf)
{
  CURLcode نتیجه;

  نتیجه = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->روش,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->مسیر ? req->مسیر : "",
                          http_minor);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = Curl_dynhds_h1_dprint(&req->سرآیندها, dbuf);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

بیرون:
  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_HTTP */
