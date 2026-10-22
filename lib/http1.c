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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http1.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urlapi-int.h"


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H1_MAX_URL_LEN (8 * 1024)

ਖਾਲੀ Curl_h1_req_parse_init(ਬਣਤਰ h1_req_parser *parser, size_t max_ਸਤਰ_len)
{
  memset(parser, 0, ਆਕਾਰ(*parser));
  parser->max_ਸਤਰ_len = max_ਸਤਰ_len;
  curlx_dyn_init(&parser->scratch, max_ਸਤਰ_len);
}

ਖਾਲੀ Curl_h1_req_parse_free(ਬਣਤਰ h1_req_parser *parser)
{
  ਜੇ(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = ਨਿਰਾ;
    parser->ਪੂਰਾ = FALSE;
  }
}

ਸਥਿਰ CURLcode trim_ਸਤਰ(ਬਣਤਰ h1_req_parser *parser, ਪੂਰਨ options)
{
  DEBUGASSERT(parser->ਸਤਰ);
  ਜੇ(parser->ਸਤਰ_len) {
    ਜੇ(parser->ਸਤਰ[parser->ਸਤਰ_len - 1] == '\n')
      --parser->ਸਤਰ_len;
    ਜੇ(parser->ਸਤਰ_len) {
      ਜੇ(parser->ਸਤਰ[parser->ਸਤਰ_len - 1] == '\r')
        --parser->ਸਤਰ_len;
      ਨਹੀਂਤਾਂ ਜੇ(options & H1_PARSE_OPT_STRICT)
        ਵਾਪਸ CURLE_URL_MALFORMAT;
    }
    ਨਹੀਂਤਾਂ ਜੇ(options & H1_PARSE_OPT_STRICT)
      ਵਾਪਸ CURLE_URL_MALFORMAT;
  }
  ਨਹੀਂਤਾਂ ਜੇ(options & H1_PARSE_OPT_STRICT)
    ਵਾਪਸ CURLE_URL_MALFORMAT;

  ਜੇ(parser->ਸਤਰ_len > parser->max_ਸਤਰ_len) {
    ਵਾਪਸ CURLE_URL_MALFORMAT;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode detect_ਸਤਰ(ਬਣਤਰ h1_req_parser *parser,
                            ਅਚਲ uint8_t *buf, ਅਚਲ size_t buflen,
                            size_t *pnread)
{
  ਅਚਲ ਅੱਖਰ *ਸਤਰ_ਸਿਰਾ;

  DEBUGASSERT(!parser->ਸਤਰ);
  *pnread = 0;
  ਸਤਰ_ਸਿਰਾ = memchr(buf, '\n', buflen);
  ਜੇ(!ਸਤਰ_ਸਿਰਾ)
    ਵਾਪਸ CURLE_AGAIN;
  parser->ਸਤਰ = (ਅਚਲ ਅੱਖਰ *)buf;
  parser->ਸਤਰ_len = ਸਤਰ_ਸਿਰਾ - parser->ਸਤਰ + 1;
  *pnread = parser->ਸਤਰ_len;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ਅਗਲਾ_ਸਤਰ(ਬਣਤਰ h1_req_parser *parser,
                          ਅਚਲ uint8_t *buf, ਅਚਲ size_t buflen, ਪੂਰਨ options,
                          size_t *pnread)
{
  CURLcode ਨਤੀਜਾ;

  *pnread = 0;
  ਜੇ(parser->ਸਤਰ) {
    parser->ਸਤਰ = ਨਿਰਾ;
    parser->ਸਤਰ_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  ਨਤੀਜਾ = detect_ਸਤਰ(parser, buf, buflen, pnread);
  ਜੇ(!ਨਤੀਜਾ) {
    ਜੇ(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      ਨਤੀਜਾ = curlx_dyn_addn(&parser->scratch, parser->ਸਤਰ,
                              parser->ਸਤਰ_len);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      parser->ਸਤਰ = curlx_dyn_ptr(&parser->scratch);
      parser->ਸਤਰ_len = curlx_dyn_len(&parser->scratch);
    }
    ਨਤੀਜਾ = trim_ਸਤਰ(parser, options);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    ਨਤੀਜਾ = curlx_dyn_addn(&parser->scratch, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)buf,
                            buflen);
    *pnread = buflen;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ਸ਼ੁਰੂ_req(ਬਣਤਰ h1_req_parser *parser,
                          ਅਚਲ ਅੱਖਰ *scheme_default,
                          ਅਚਲ ਅੱਖਰ *custom_ਵਿਧੀ,
                          ਪੂਰਨ options)
{
  ਅਚਲ ਅੱਖਰ *p, *m, *ਨਿਸ਼ਾਨਾ, *hv, *scheme, *authority, *ਰਸਤਾ;
  size_t m_len, ਨਿਸ਼ਾਨਾ_len, hv_len, scheme_len, authority_len, ਰਸਤਾ_len;
  size_t i;
  CURLU *url = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  ਜੇ(custom_ਵਿਧੀ && custom_ਵਿਧੀ[0] &&
     !ਲੜੀ_ਤੁਲਨਾ(custom_ਵਿਧੀ, parser->ਸਤਰ, ਲੜੀ_ਲੰਬਾਈ(custom_ਵਿਧੀ))) {
    p = parser->ਸਤਰ + ਲੜੀ_ਲੰਬਾਈ(custom_ਵਿਧੀ);
  }
  ਨਹੀਂਤਾਂ {
    p = memchr(parser->ਸਤਰ, ' ', parser->ਸਤਰ_len);
    ਜੇ(!p || p == parser->ਸਤਰ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  m = parser->ਸਤਰ;
  m_len = p - parser->ਸਤਰ;
  ਨਿਸ਼ਾਨਾ = p + 1;
  ਨਿਸ਼ਾਨਾ_len = hv_len = 0;
  hv = ਨਿਰਾ;

  /* URL may contain spaces so scan backwards */
  ਲਈ(i = parser->ਸਤਰ_len; i > m_len; --i) {
    ਜੇ(parser->ਸਤਰ[i] == ' ') {
      hv = &parser->ਸਤਰ[i + 1];
      hv_len = parser->ਸਤਰ_len - i;
      ਨਿਸ਼ਾਨਾ_len = (hv - ਨਿਸ਼ਾਨਾ) - 1;
      ਰੋਕੋ;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  ਜੇ(!ਨਿਸ਼ਾਨਾ_len || !hv_len)
    ਜਾਓ ਬਾਹਰਲਾ;

  (ਖਾਲੀ)hv;

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
  scheme = authority = ਰਸਤਾ = ਨਿਰਾ;
  scheme_len = authority_len = ਰਸਤਾ_len = 0;

  ਜੇ(ਨਿਸ਼ਾਨਾ_len == 1 && ਨਿਸ਼ਾਨਾ[0] == '*') {
    /* asterisk-form */
    ਰਸਤਾ = ਨਿਸ਼ਾਨਾ;
    ਰਸਤਾ_len = ਨਿਸ਼ਾਨਾ_len;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ("CONNECT", m, m_len)) {
    /* authority-form */
    authority = ਨਿਸ਼ਾਨਾ;
    authority_len = ਨਿਸ਼ਾਨਾ_len;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਨਿਸ਼ਾਨਾ[0] == '/') {
    /* origin-form */
    ਰਸਤਾ = ਨਿਸ਼ਾਨਾ;
    ਰਸਤਾ_len = ਨਿਸ਼ਾਨਾ_len;
  }
  ਨਹੀਂਤਾਂ {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    ਅੱਖਰ tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    ਰਸਤਾ = ਨਿਸ਼ਾਨਾ;
    ਰਸਤਾ_len = ਨਿਸ਼ਾਨਾ_len;

    /* URL parser wants null-termination */
    ਜੇ(ਨਿਸ਼ਾਨਾ_len >= ਆਕਾਰ(tmp))
      ਜਾਓ ਬਾਹਰਲਾ;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(tmp, ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_len);
    tmp[ਨਿਸ਼ਾਨਾ_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    ਜੇ(Curl_is_absolute_url(tmp, ਨਿਰਾ, 0, FALSE)) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ url_options;

      url = curl_url();
      ਜੇ(!url) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_ਰਸਤਾ_AS_IS |
                     CURLU_NO_DEFAULT_ਪੋਰਟ);
      ਜੇ(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_ਸੈੱਟ(url, CURLUPART_URL, tmp, url_options);
      ਜੇ(uc) {
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }

    ਜੇ(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਜੇ(url) {
    ਨਤੀਜਾ = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = ਲੜੀ_ਲੰਬਾਈ(scheme_default);
    }
    ਨਤੀਜਾ = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, ਰਸਤਾ, ਰਸਤਾ_len);
  }

ਬਾਹਰਲਾ:
  curl_url_cleanup(url);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_h1_req_parse_read(ਬਣਤਰ h1_req_parser *parser,
                                ਅਚਲ uint8_t *buf, size_t buflen,
                                ਅਚਲ ਅੱਖਰ *scheme_default,
                                ਅਚਲ ਅੱਖਰ *custom_ਵਿਧੀ,
                                ਪੂਰਨ options, size_t *pnread)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  ਜੇ(!buf)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜਦੋਂ(!parser->ਪੂਰਾ) {
    ਨਤੀਜਾ = ਅਗਲਾ_ਸਤਰ(parser, buf, buflen, options, &nread);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
        ਨਤੀਜਾ = CURLE_OK;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    ਜੇ(!parser->ਸਤਰ) {
      /* consumed bytes, but line not complete */
      ਜੇ(!buflen)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!parser->req) {
      ਨਤੀਜਾ = ਸ਼ੁਰੂ_req(parser, scheme_default, custom_ਵਿਧੀ, options);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(parser->ਸਤਰ_len == 0) {
      /* last, empty line, we are finished */
      ਜੇ(!parser->req) {
        ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      parser->ਪੂਰਾ = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = Curl_dynhds_h1_ਜੋੜੋ_ਸਤਰ(&parser->req->ਸਿਰਲੇਖਾਂ,
                                       parser->ਸਤਰ, parser->ਸਤਰ_len);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_h1_req_write_ਸਿਰ(ਬਣਤਰ httpreq *req, ਪੂਰਨ http_minor,
                                ਬਣਤਰ dynbuf *dbuf)
{
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->ਵਿਧੀ,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->ਰਸਤਾ ? req->ਰਸਤਾ : "",
                          http_minor);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_dynhds_h1_dprint(&req->ਸਿਰਲੇਖਾਂ, dbuf);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */
