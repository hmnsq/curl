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
#अन्तर्भाव "curl_setup.h"

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "http1.h"
#अन्तर्भाव "urlapi-int.h"


#परिभाषय H1_MAX_URL_LEN (8 * 1024)

शून्य Curl_h1_req_parse_init(संरचना h1_req_parser *parser, size_t max_पङ्क्तिः_len)
{
  स्मृतिं_पूरय(parser, 0, परिमाण(*parser));
  parser->max_पङ्क्तिः_len = max_पङ्क्तिः_len;
  curlx_dyn_init(&parser->scratch, max_पङ्क्तिः_len);
}

शून्य Curl_h1_req_parse_free(संरचना h1_req_parser *parser)
{
  यदि(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = रिक्त;
    parser->समाप्तम् = FALSE;
  }
}

स्थिर CURLcode trim_पङ्क्तिः(संरचना h1_req_parser *parser, पूर्णाङ्क options)
{
  DEBUGASSERT(parser->पङ्क्तिः);
  यदि(parser->पङ्क्तिः_len) {
    यदि(parser->पङ्क्तिः[parser->पङ्क्तिः_len - 1] == '\n')
      --parser->पङ्क्तिः_len;
    यदि(parser->पङ्क्तिः_len) {
      यदि(parser->पङ्क्तिः[parser->पङ्क्तिः_len - 1] == '\r')
        --parser->पङ्क्तिः_len;
      अन्यथा यदि(options & H1_PARSE_OPT_STRICT)
        प्रत्यागम CURLE_URL_MALFORMAT;
    }
    अन्यथा यदि(options & H1_PARSE_OPT_STRICT)
      प्रत्यागम CURLE_URL_MALFORMAT;
  }
  अन्यथा यदि(options & H1_PARSE_OPT_STRICT)
    प्रत्यागम CURLE_URL_MALFORMAT;

  यदि(parser->पङ्क्तिः_len > parser->max_पङ्क्तिः_len) {
    प्रत्यागम CURLE_URL_MALFORMAT;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode detect_पङ्क्तिः(संरचना h1_req_parser *parser,
                            ध्रुव uint8_t *buf, ध्रुव size_t buflen,
                            size_t *pnread)
{
  ध्रुव वर्ण *पङ्क्तिः_अन्तः;

  DEBUGASSERT(!parser->पङ्क्तिः);
  *pnread = 0;
  पङ्क्तिः_अन्तः = memchr(buf, '\n', buflen);
  यदि(!पङ्क्तिः_अन्तः)
    प्रत्यागम CURLE_AGAIN;
  parser->पङ्क्तिः = (ध्रुव वर्ण *)buf;
  parser->पङ्क्तिः_len = पङ्क्तिः_अन्तः - parser->पङ्क्तिः + 1;
  *pnread = parser->पङ्क्तिः_len;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode अग्र_पङ्क्तिः(संरचना h1_req_parser *parser,
                          ध्रुव uint8_t *buf, ध्रुव size_t buflen, पूर्णाङ्क options,
                          size_t *pnread)
{
  CURLcode परिणाम;

  *pnread = 0;
  यदि(parser->पङ्क्तिः) {
    parser->पङ्क्तिः = रिक्त;
    parser->पङ्क्तिः_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  परिणाम = detect_पङ्क्तिः(parser, buf, buflen, pnread);
  यदि(!परिणाम) {
    यदि(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      परिणाम = curlx_dyn_addn(&parser->scratch, parser->पङ्क्तिः,
                              parser->पङ्क्तिः_len);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      parser->पङ्क्तिः = curlx_dyn_ptr(&parser->scratch);
      parser->पङ्क्तिः_len = curlx_dyn_len(&parser->scratch);
    }
    परिणाम = trim_पङ्क्तिः(parser, options);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  अन्यथा यदि(परिणाम == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    परिणाम = curlx_dyn_addn(&parser->scratch, (ध्रुव अचिह्नित वर्ण *)buf,
                            buflen);
    *pnread = buflen;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode start_req(संरचना h1_req_parser *parser,
                          ध्रुव वर्ण *scheme_default,
                          ध्रुव वर्ण *custom_विधानम्,
                          पूर्णाङ्क options)
{
  ध्रुव वर्ण *p, *m, *target, *hv, *scheme, *authority, *मार्गः;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, मार्गः_len;
  size_t i;
  CURLU *url = रिक्त;
  CURLcode परिणाम = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  यदि(custom_विधानम् && custom_विधानम्[0] &&
     !वाक्ये_तुलय(custom_विधानम्, parser->पङ्क्तिः, वाक्यदैर्घ्यम्(custom_विधानम्))) {
    p = parser->पङ्क्तिः + वाक्यदैर्घ्यम्(custom_विधानम्);
  }
  अन्यथा {
    p = memchr(parser->पङ्क्तिः, ' ', parser->पङ्क्तिः_len);
    यदि(!p || p == parser->पङ्क्तिः)
      गच्छ बहिर्गमनम्;
  }

  m = parser->पङ्क्तिः;
  m_len = p - parser->पङ्क्तिः;
  target = p + 1;
  target_len = hv_len = 0;
  hv = रिक्त;

  /* URL may contain spaces so scan backwards */
  प्रति(i = parser->पङ्क्तिः_len; i > m_len; --i) {
    यदि(parser->पङ्क्तिः[i] == ' ') {
      hv = &parser->पङ्क्तिः[i + 1];
      hv_len = parser->पङ्क्तिः_len - i;
      target_len = (hv - target) - 1;
      भङ्ग;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  यदि(!target_len || !hv_len)
    गच्छ बहिर्गमनम्;

  (शून्य)hv;

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
  scheme = authority = मार्गः = रिक्त;
  scheme_len = authority_len = मार्गः_len = 0;

  यदि(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    मार्गः = target;
    मार्गः_len = target_len;
  }
  अन्यथा यदि(!वाक्ये_तुलय("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  अन्यथा यदि(target[0] == '/') {
    /* origin-form */
    मार्गः = target;
    मार्गः_len = target_len;
  }
  अन्यथा {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    वर्ण tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    मार्गः = target;
    मार्गः_len = target_len;

    /* URL parser wants null-termination */
    यदि(target_len >= परिमाण(tmp))
      गच्छ बहिर्गमनम्;
    स्मृतिं_प्रतिलिख(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    यदि(Curl_is_absolute_url(tmp, रिक्त, 0, FALSE)) {
      अचिह्नित पूर्णाङ्क url_options;

      url = curl_url();
      यदि(!url) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_मार्गः_AS_IS |
                     CURLU_NO_DEFAULT_पत्तनम्);
      यदि(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_समुच्चयः(url, CURLUPART_URL, tmp, url_options);
      यदि(uc) {
        गच्छ बहिर्गमनम्;
      }
    }

    यदि(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      गच्छ बहिर्गमनम्;
    }
  }

  यदि(url) {
    परिणाम = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  अन्यथा {
    यदि(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = वाक्यदैर्घ्यम्(scheme_default);
    }
    परिणाम = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, मार्गः, मार्गः_len);
  }

बहिर्गमनम्:
  curl_url_cleanup(url);
  प्रत्यागम परिणाम;
}

CURLcode Curl_h1_req_parse_read(संरचना h1_req_parser *parser,
                                ध्रुव uint8_t *buf, size_t buflen,
                                ध्रुव वर्ण *scheme_default,
                                ध्रुव वर्ण *custom_विधानम्,
                                पूर्णाङ्क options, size_t *pnread)
{
  CURLcode परिणाम = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  यदि(!buf)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  यावत्(!parser->समाप्तम्) {
    परिणाम = अग्र_पङ्क्तिः(parser, buf, buflen, options, &nread);
    यदि(परिणाम) {
      यदि(परिणाम == CURLE_AGAIN)
        परिणाम = CURLE_OK;
      गच्छ बहिर्गमनम्;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    यदि(!parser->पङ्क्तिः) {
      /* consumed bytes, but line not complete */
      यदि(!buflen)
        गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(!parser->req) {
      परिणाम = start_req(parser, scheme_default, custom_विधानम्, options);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(parser->पङ्क्तिः_len == 0) {
      /* last, empty line, we are finished */
      यदि(!parser->req) {
        परिणाम = CURLE_URL_MALFORMAT;
        गच्छ बहिर्गमनम्;
      }
      parser->समाप्तम् = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    अन्यथा {
      परिणाम = Curl_dynhds_h1_सङ्कलय_पङ्क्तिः(&parser->req->शीर्षकाणि,
                                       parser->पङ्क्तिः, parser->पङ्क्तिः_len);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
  }

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

CURLcode Curl_h1_req_write_शीर्ष(संरचना httpreq *req, पूर्णाङ्क http_minor,
                                संरचना dynbuf *dbuf)
{
  CURLcode परिणाम;

  परिणाम = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->विधानम्,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->मार्गः ? req->मार्गः : "",
                          http_minor);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_dynhds_h1_dprint(&req->शीर्षकाणि, dbuf);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_HTTP */
