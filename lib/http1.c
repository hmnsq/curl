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

শূন্য Curl_h1_req_parse_init(কাঠামো h1_req_parser *parser, size_t max_সারি_len)
{
  memset(parser, 0, আকার(*parser));
  parser->max_সারি_len = max_সারি_len;
  curlx_dyn_init(&parser->scratch, max_সারি_len);
}

শূন্য Curl_h1_req_parse_free(কাঠামো h1_req_parser *parser)
{
  যদি(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = ফাঁকা;
    parser->done = FALSE;
  }
}

স্থির CURLcode trim_সারি(কাঠামো h1_req_parser *parser, পূর্ণসংখ্যা options)
{
  DEBUGASSERT(parser->সারি);
  যদি(parser->সারি_len) {
    যদি(parser->সারি[parser->সারি_len - 1] == '\n')
      --parser->সারি_len;
    যদি(parser->সারি_len) {
      যদি(parser->সারি[parser->সারি_len - 1] == '\r')
        --parser->সারি_len;
      অন্যথা যদি(options & H1_PARSE_OPT_STRICT)
        ফেরত CURLE_URL_MALFORMAT;
    }
    অন্যথা যদি(options & H1_PARSE_OPT_STRICT)
      ফেরত CURLE_URL_MALFORMAT;
  }
  অন্যথা যদি(options & H1_PARSE_OPT_STRICT)
    ফেরত CURLE_URL_MALFORMAT;

  যদি(parser->সারি_len > parser->max_সারি_len) {
    ফেরত CURLE_URL_MALFORMAT;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode detect_সারি(কাঠামো h1_req_parser *parser,
                            ধ্রুবক uint8_t *buf, ধ্রুবক size_t buflen,
                            size_t *pnread)
{
  ধ্রুবক অক্ষর *সারি_end;

  DEBUGASSERT(!parser->সারি);
  *pnread = 0;
  সারি_end = memchr(buf, '\n', buflen);
  যদি(!সারি_end)
    ফেরত CURLE_AGAIN;
  parser->সারি = (ধ্রুবক অক্ষর *)buf;
  parser->সারি_len = সারি_end - parser->সারি + 1;
  *pnread = parser->সারি_len;
  ফেরত CURLE_OK;
}

স্থির CURLcode পরবর্তী_সারি(কাঠামো h1_req_parser *parser,
                          ধ্রুবক uint8_t *buf, ধ্রুবক size_t buflen, পূর্ণসংখ্যা options,
                          size_t *pnread)
{
  CURLcode ফলাফল;

  *pnread = 0;
  যদি(parser->সারি) {
    parser->সারি = ফাঁকা;
    parser->সারি_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  ফলাফল = detect_সারি(parser, buf, buflen, pnread);
  যদি(!ফলাফল) {
    যদি(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      ফলাফল = curlx_dyn_addn(&parser->scratch, parser->সারি,
                              parser->সারি_len);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      parser->সারি = curlx_dyn_ptr(&parser->scratch);
      parser->সারি_len = curlx_dyn_len(&parser->scratch);
    }
    ফলাফল = trim_সারি(parser, options);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  অন্যথা যদি(ফলাফল == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    ফলাফল = curlx_dyn_addn(&parser->scratch, (ধ্রুবক unsigned অক্ষর *)buf,
                            buflen);
    *pnread = buflen;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode start_req(কাঠামো h1_req_parser *parser,
                          ধ্রুবক অক্ষর *scheme_default,
                          ধ্রুবক অক্ষর *custom_method,
                          পূর্ণসংখ্যা options)
{
  ধ্রুবক অক্ষর *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = ফাঁকা;
  CURLcode ফলাফল = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  যদি(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->সারি, strlen(custom_method))) {
    p = parser->সারি + strlen(custom_method);
  }
  অন্যথা {
    p = memchr(parser->সারি, ' ', parser->সারি_len);
    যদি(!p || p == parser->সারি)
      যাও out;
  }

  m = parser->সারি;
  m_len = p - parser->সারি;
  target = p + 1;
  target_len = hv_len = 0;
  hv = ফাঁকা;

  /* URL may contain spaces so scan backwards */
  জন্য(i = parser->সারি_len; i > m_len; --i) {
    যদি(parser->সারি[i] == ' ') {
      hv = &parser->সারি[i + 1];
      hv_len = parser->সারি_len - i;
      target_len = (hv - target) - 1;
      বিরতি;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  যদি(!target_len || !hv_len)
    যাও out;

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
  scheme = authority = path = ফাঁকা;
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
    অক্ষর tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    যদি(target_len >= আকার(tmp))
      যাও out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    যদি(Curl_is_absolute_url(tmp, ফাঁকা, 0, FALSE)) {
      unsigned পূর্ণসংখ্যা url_options;

      url = curl_url();
      যদি(!url) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      যদি(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      যদি(uc) {
        যাও out;
      }
    }

    যদি(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      যাও out;
    }
  }

  যদি(url) {
    ফলাফল = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  অন্যথা {
    যদি(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = strlen(scheme_default);
    }
    ফলাফল = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  ফেরত ফলাফল;
}

CURLcode Curl_h1_req_parse_read(কাঠামো h1_req_parser *parser,
                                ধ্রুবক uint8_t *buf, size_t buflen,
                                ধ্রুবক অক্ষর *scheme_default,
                                ধ্রুবক অক্ষর *custom_method,
                                পূর্ণসংখ্যা options, size_t *pnread)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  যদি(!buf)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  যখন(!parser->done) {
    ফলাফল = পরবর্তী_সারি(parser, buf, buflen, options, &nread);
    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_AGAIN)
        ফলাফল = CURLE_OK;
      যাও out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    যদি(!parser->সারি) {
      /* consumed bytes, but line not complete */
      যদি(!buflen)
        যাও out;
    }
    অন্যথা যদি(!parser->req) {
      ফলাফল = start_req(parser, scheme_default, custom_method, options);
      যদি(ফলাফল)
        যাও out;
    }
    অন্যথা যদি(parser->সারি_len == 0) {
      /* last, empty line, we are finished */
      যদি(!parser->req) {
        ফলাফল = CURLE_URL_MALFORMAT;
        যাও out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    অন্যথা {
      ফলাফল = Curl_dynhds_h1_add_সারি(&parser->req->headers,
                                       parser->সারি, parser->সারি_len);
      যদি(ফলাফল)
        যাও out;
    }
  }

out:
  ফেরত ফলাফল;
}

CURLcode Curl_h1_req_write_শীর্ষ(কাঠামো httpreq *req, পূর্ণসংখ্যা http_minor,
                                কাঠামো dynbuf *dbuf)
{
  CURLcode ফলাফল;

  ফলাফল = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_HTTP */
