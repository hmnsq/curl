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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "http1.h"
#ಅನ್ತರ್ಭಾವ "urlapi-int.h"


#ವ್ಯಾಖ್ಯಾನಿಸು H1_MAX_URL_LEN (8 * 1024)

ಶೂನ್ಯ Curl_h1_req_parse_init(ರಚನೆ h1_req_parser *parser, size_t max_ಸಾಲು_len)
{
  memset(parser, 0, ಗಾತ್ರ(*parser));
  parser->max_ಸಾಲು_len = max_ಸಾಲು_len;
  curlx_dyn_init(&parser->scratch, max_ಸಾಲು_len);
}

ಶೂನ್ಯ Curl_h1_req_parse_free(ರಚನೆ h1_req_parser *parser)
{
  ಆದರೆ(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = ಖಾಲಿ;
    parser->ಮುಗಿದ = FALSE;
  }
}

ಸ್ಥಿರ CURLcode trim_ಸಾಲು(ರಚನೆ h1_req_parser *parser, ಪೂರ್ಣಾಂಕ options)
{
  DEBUGASSERT(parser->ಸಾಲು);
  ಆದರೆ(parser->ಸಾಲು_len) {
    ಆದರೆ(parser->ಸಾಲು[parser->ಸಾಲು_len - 1] == '\n')
      --parser->ಸಾಲು_len;
    ಆದರೆ(parser->ಸಾಲು_len) {
      ಆದರೆ(parser->ಸಾಲು[parser->ಸಾಲು_len - 1] == '\r')
        --parser->ಸಾಲು_len;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(options & H1_PARSE_OPT_STRICT)
        ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(options & H1_PARSE_OPT_STRICT)
      ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(options & H1_PARSE_OPT_STRICT)
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;

  ಆದರೆ(parser->ಸಾಲು_len > parser->max_ಸಾಲು_len) {
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode detect_ಸಾಲು(ರಚನೆ h1_req_parser *parser,
                            ಸ್ಥಿರಾಂಕ uint8_t *buf, ಸ್ಥಿರಾಂಕ size_t buflen,
                            size_t *pnread)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು_ಕೊನೆ;

  DEBUGASSERT(!parser->ಸಾಲು);
  *pnread = 0;
  ಸಾಲು_ಕೊನೆ = memchr(buf, '\n', buflen);
  ಆದರೆ(!ಸಾಲು_ಕೊನೆ)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  parser->ಸಾಲು = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf;
  parser->ಸಾಲು_len = ಸಾಲು_ಕೊನೆ - parser->ಸಾಲು + 1;
  *pnread = parser->ಸಾಲು_len;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ಮುಂದಿನ_ಸಾಲು(ರಚನೆ h1_req_parser *parser,
                          ಸ್ಥಿರಾಂಕ uint8_t *buf, ಸ್ಥಿರಾಂಕ size_t buflen, ಪೂರ್ಣಾಂಕ options,
                          size_t *pnread)
{
  CURLcode ಫಲಿತಾಂಶ;

  *pnread = 0;
  ಆದರೆ(parser->ಸಾಲು) {
    parser->ಸಾಲು = ಖಾಲಿ;
    parser->ಸಾಲು_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  ಫಲಿತಾಂಶ = detect_ಸಾಲು(parser, buf, buflen, pnread);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಆದರೆ(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&parser->scratch, parser->ಸಾಲು,
                              parser->ಸಾಲು_len);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      parser->ಸಾಲು = curlx_dyn_ptr(&parser->scratch);
      parser->ಸಾಲು_len = curlx_dyn_len(&parser->scratch);
    }
    ಫಲಿತಾಂಶ = trim_ಸಾಲು(parser, options);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&parser->scratch, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)buf,
                            buflen);
    *pnread = buflen;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ಆರಂಭ_req(ರಚನೆ h1_req_parser *parser,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme_default,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *custom_ವಿಧಾನ,
                          ಪೂರ್ಣಾಂಕ options)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *m, *ಗುರಿ, *hv, *scheme, *authority, *ಮಾರ್ಗ;
  size_t m_len, ಗುರಿ_len, hv_len, scheme_len, authority_len, ಮಾರ್ಗ_len;
  size_t i;
  CURLU *url = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  ಆದರೆ(custom_ವಿಧಾನ && custom_ವಿಧಾನ[0] &&
     !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(custom_ವಿಧಾನ, parser->ಸಾಲು, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(custom_ವಿಧಾನ))) {
    p = parser->ಸಾಲು + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(custom_ವಿಧಾನ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    p = memchr(parser->ಸಾಲು, ' ', parser->ಸಾಲು_len);
    ಆದರೆ(!p || p == parser->ಸಾಲು)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  m = parser->ಸಾಲು;
  m_len = p - parser->ಸಾಲು;
  ಗುರಿ = p + 1;
  ಗುರಿ_len = hv_len = 0;
  hv = ಖಾಲಿ;

  /* URL may contain spaces so scan backwards */
  ಗಾಗಿ(i = parser->ಸಾಲು_len; i > m_len; --i) {
    ಆದರೆ(parser->ಸಾಲು[i] == ' ') {
      hv = &parser->ಸಾಲು[i + 1];
      hv_len = parser->ಸಾಲು_len - i;
      ಗುರಿ_len = (hv - ಗುರಿ) - 1;
      ನಿಲ್ಲಿಸು;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  ಆದರೆ(!ಗುರಿ_len || !hv_len)
    ಹೋಗು ಹೊರಹೋಗು;

  (ಶೂನ್ಯ)hv;

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
  scheme = authority = ಮಾರ್ಗ = ಖಾಲಿ;
  scheme_len = authority_len = ಮಾರ್ಗ_len = 0;

  ಆದರೆ(ಗುರಿ_len == 1 && ಗುರಿ[0] == '*') {
    /* asterisk-form */
    ಮಾರ್ಗ = ಗುರಿ;
    ಮಾರ್ಗ_len = ಗುರಿ_len;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("CONNECT", m, m_len)) {
    /* authority-form */
    authority = ಗುರಿ;
    authority_len = ಗುರಿ_len;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗುರಿ[0] == '/') {
    /* origin-form */
    ಮಾರ್ಗ = ಗುರಿ;
    ಮಾರ್ಗ_len = ಗುರಿ_len;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    ಅಕ್ಷರ tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    ಮಾರ್ಗ = ಗುರಿ;
    ಮಾರ್ಗ_len = ಗುರಿ_len;

    /* URL parser wants null-termination */
    ಆದರೆ(ಗುರಿ_len >= ಗಾತ್ರ(tmp))
      ಹೋಗು ಹೊರಹೋಗು;
    ಸ್ಮರಣೆ_ನಕಲು(tmp, ಗುರಿ, ಗುರಿ_len);
    tmp[ಗುರಿ_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    ಆದರೆ(Curl_is_absolute_url(tmp, ಖಾಲಿ, 0, FALSE)) {
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ url_options;

      url = curl_url();
      ಆದರೆ(!url) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_ಮಾರ್ಗ_AS_IS |
                     CURLU_NO_DEFAULT_ಪೋರ್ಟ್);
      ಆದರೆ(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_ಗಣ(url, CURLUPART_URL, tmp, url_options);
      ಆದರೆ(uc) {
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }

    ಆದರೆ(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಆದರೆ(url) {
    ಫಲಿತಾಂಶ = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(scheme_default);
    }
    ಫಲಿತಾಂಶ = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, ಮಾರ್ಗ, ಮಾರ್ಗ_len);
  }

ಹೊರಹೋಗು:
  curl_url_cleanup(url);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_h1_req_parse_read(ರಚನೆ h1_req_parser *parser,
                                ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t buflen,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme_default,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *custom_ವಿಧಾನ,
                                ಪೂರ್ಣಾಂಕ options, size_t *pnread)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  ಆದರೆ(!buf)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆಗ(!parser->ಮುಗಿದ) {
    ಫಲಿತಾಂಶ = ಮುಂದಿನ_ಸಾಲು(parser, buf, buflen, options, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
        ಫಲಿತಾಂಶ = CURLE_OK;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    ಆದರೆ(!parser->ಸಾಲು) {
      /* consumed bytes, but line not complete */
      ಆದರೆ(!buflen)
        ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!parser->req) {
      ಫಲಿತಾಂಶ = ಆರಂಭ_req(parser, scheme_default, custom_ವಿಧಾನ, options);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(parser->ಸಾಲು_len == 0) {
      /* last, empty line, we are finished */
      ಆದರೆ(!parser->req) {
        ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      parser->ಮುಗಿದ = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = Curl_dynhds_h1_ಕೂಡಿಸು_ಸಾಲು(&parser->req->ಶೀರ್ಷಿಕೆಗಳು,
                                       parser->ಸಾಲು, parser->ಸಾಲು_len);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
  }

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_h1_req_write_ತಲೆ(ರಚನೆ httpreq *req, ಪೂರ್ಣಾಂಕ http_minor,
                                ರಚನೆ dynbuf *dbuf)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->ವಿಧಾನ,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->ಮಾರ್ಗ ? req->ಮಾರ್ಗ : "",
                          http_minor);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_dynhds_h1_dprint(&req->ಶೀರ್ಷಿಕೆಗಳು, dbuf);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */
