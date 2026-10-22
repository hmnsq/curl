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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP

#ضمن "urldata.h"
#ضمن "http.h"
#ضمن "http1.h"
#ضمن "urlapi-int.h"


#وصف_ڪريو H1_MAX_URL_LEN (8 * 1024)

فراغ Curl_h1_req_parse_init(بنية h1_req_parser *parser, size_t max_سٽ_len)
{
  memset(parser, 0, حجم(*parser));
  parser->max_سٽ_len = max_سٽ_len;
  curlx_dyn_init(&parser->scratch, max_سٽ_len);
}

فراغ Curl_h1_req_parse_free(بنية h1_req_parser *parser)
{
  إذا(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = عدم;
    parser->پورو = FALSE;
  }
}

ساكن CURLcode trim_سٽ(بنية h1_req_parser *parser, صحيح options)
{
  DEBUGASSERT(parser->سٽ);
  إذا(parser->سٽ_len) {
    إذا(parser->سٽ[parser->سٽ_len - 1] == '\n')
      --parser->سٽ_len;
    إذا(parser->سٽ_len) {
      إذا(parser->سٽ[parser->سٽ_len - 1] == '\r')
        --parser->سٽ_len;
      وإلا إذا(options & H1_PARSE_OPT_STRICT)
        أرجع CURLE_URL_MALFORMAT;
    }
    وإلا إذا(options & H1_PARSE_OPT_STRICT)
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا إذا(options & H1_PARSE_OPT_STRICT)
    أرجع CURLE_URL_MALFORMAT;

  إذا(parser->سٽ_len > parser->max_سٽ_len) {
    أرجع CURLE_URL_MALFORMAT;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode detect_سٽ(بنية h1_req_parser *parser,
                            ثابت uint8_t *buf, ثابت size_t buflen,
                            size_t *pnread)
{
  ثابت حرف *سٽ_پڇاڙي;

  DEBUGASSERT(!parser->سٽ);
  *pnread = 0;
  سٽ_پڇاڙي = memchr(buf, '\n', buflen);
  إذا(!سٽ_پڇاڙي)
    أرجع CURLE_AGAIN;
  parser->سٽ = (ثابت حرف *)buf;
  parser->سٽ_len = سٽ_پڇاڙي - parser->سٽ + 1;
  *pnread = parser->سٽ_len;
  أرجع CURLE_OK;
}

ساكن CURLcode ايندڙ_سٽ(بنية h1_req_parser *parser,
                          ثابت uint8_t *buf, ثابت size_t buflen, صحيح options,
                          size_t *pnread)
{
  CURLcode نتيجو;

  *pnread = 0;
  إذا(parser->سٽ) {
    parser->سٽ = عدم;
    parser->سٽ_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  نتيجو = detect_سٽ(parser, buf, buflen, pnread);
  إذا(!نتيجو) {
    إذا(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      نتيجو = curlx_dyn_addn(&parser->scratch, parser->سٽ,
                              parser->سٽ_len);
      إذا(نتيجو)
        أرجع نتيجو;
      parser->سٽ = curlx_dyn_ptr(&parser->scratch);
      parser->سٽ_len = curlx_dyn_len(&parser->scratch);
    }
    نتيجو = trim_سٽ(parser, options);
    إذا(نتيجو)
      أرجع نتيجو;
  }
  وإلا إذا(نتيجو == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    نتيجو = curlx_dyn_addn(&parser->scratch, (ثابت بنانشان حرف *)buf,
                            buflen);
    *pnread = buflen;
  }
  أرجع نتيجو;
}

ساكن CURLcode شروعات_req(بنية h1_req_parser *parser,
                          ثابت حرف *scheme_default,
                          ثابت حرف *custom_طريقو,
                          صحيح options)
{
  ثابت حرف *p, *m, *هدف, *hv, *scheme, *authority, *رستو;
  size_t m_len, هدف_len, hv_len, scheme_len, authority_len, رستو_len;
  size_t i;
  CURLU *url = عدم;
  CURLcode نتيجو = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  إذا(custom_طريقو && custom_طريقو[0] &&
     !لڙي_ڀيٽ(custom_طريقو, parser->سٽ, لڙي_ڊيگهه(custom_طريقو))) {
    p = parser->سٽ + لڙي_ڊيگهه(custom_طريقو);
  }
  وإلا {
    p = memchr(parser->سٽ, ' ', parser->سٽ_len);
    إذا(!p || p == parser->سٽ)
      اذهب ٻاهريون;
  }

  m = parser->سٽ;
  m_len = p - parser->سٽ;
  هدف = p + 1;
  هدف_len = hv_len = 0;
  hv = عدم;

  /* URL may contain spaces so scan backwards */
  لكل(i = parser->سٽ_len; i > m_len; --i) {
    إذا(parser->سٽ[i] == ' ') {
      hv = &parser->سٽ[i + 1];
      hv_len = parser->سٽ_len - i;
      هدف_len = (hv - هدف) - 1;
      اكسر;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  إذا(!هدف_len || !hv_len)
    اذهب ٻاهريون;

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
  scheme = authority = رستو = عدم;
  scheme_len = authority_len = رستو_len = 0;

  إذا(هدف_len == 1 && هدف[0] == '*') {
    /* asterisk-form */
    رستو = هدف;
    رستو_len = هدف_len;
  }
  وإلا إذا(!لڙي_ڀيٽ("CONNECT", m, m_len)) {
    /* authority-form */
    authority = هدف;
    authority_len = هدف_len;
  }
  وإلا إذا(هدف[0] == '/') {
    /* origin-form */
    رستو = هدف;
    رستو_len = هدف_len;
  }
  وإلا {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    حرف tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    رستو = هدف;
    رستو_len = هدف_len;

    /* URL parser wants null-termination */
    إذا(هدف_len >= حجم(tmp))
      اذهب ٻاهريون;
    يادگيري_نقل(tmp, هدف, هدف_len);
    tmp[هدف_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    إذا(Curl_is_absolute_url(tmp, عدم, 0, FALSE)) {
      بنانشان صحيح url_options;

      url = curl_url();
      إذا(!url) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        اذهب ٻاهريون;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_رستو_AS_IS |
                     CURLU_NO_DEFAULT_پورٽ);
      إذا(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_سيٽ(url, CURLUPART_URL, tmp, url_options);
      إذا(uc) {
        اذهب ٻاهريون;
      }
    }

    إذا(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      اذهب ٻاهريون;
    }
  }

  إذا(url) {
    نتيجو = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  وإلا {
    إذا(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = لڙي_ڊيگهه(scheme_default);
    }
    نتيجو = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, رستو, رستو_len);
  }

ٻاهريون:
  curl_url_cleanup(url);
  أرجع نتيجو;
}

CURLcode Curl_h1_req_parse_read(بنية h1_req_parser *parser,
                                ثابت uint8_t *buf, size_t buflen,
                                ثابت حرف *scheme_default,
                                ثابت حرف *custom_طريقو,
                                صحيح options, size_t *pnread)
{
  CURLcode نتيجو = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  إذا(!buf)
    أرجع CURLE_BAD_FUNCTION_دليل;

  طالما(!parser->پورو) {
    نتيجو = ايندڙ_سٽ(parser, buf, buflen, options, &nread);
    إذا(نتيجو) {
      إذا(نتيجو == CURLE_AGAIN)
        نتيجو = CURLE_OK;
      اذهب ٻاهريون;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    إذا(!parser->سٽ) {
      /* consumed bytes, but line not complete */
      إذا(!buflen)
        اذهب ٻاهريون;
    }
    وإلا إذا(!parser->req) {
      نتيجو = شروعات_req(parser, scheme_default, custom_طريقو, options);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
    وإلا إذا(parser->سٽ_len == 0) {
      /* last, empty line, we are finished */
      إذا(!parser->req) {
        نتيجو = CURLE_URL_MALFORMAT;
        اذهب ٻاهريون;
      }
      parser->پورو = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    وإلا {
      نتيجو = Curl_dynhds_h1_جوڙيو_سٽ(&parser->req->سرا,
                                       parser->سٽ, parser->سٽ_len);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
  }

ٻاهريون:
  أرجع نتيجو;
}

CURLcode Curl_h1_req_write_سر(بنية httpreq *req, صحيح http_minor,
                                بنية dynbuf *dbuf)
{
  CURLcode نتيجو;

  نتيجو = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->طريقو,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->رستو ? req->رستو : "",
                          http_minor);
  إذا(نتيجو)
    اذهب ٻاهريون;

  نتيجو = Curl_dynhds_h1_dprint(&req->سرا, dbuf);
  إذا(نتيجو)
    اذهب ٻاهريون;

  نتيجو = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

ٻاهريون:
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP */
