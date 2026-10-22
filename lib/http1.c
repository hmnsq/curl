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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "http.h"
#ଅନ୍ତର୍ଭାଵ "http1.h"
#ଅନ୍ତର୍ଭାଵ "urlapi-int.h"


#define H1_MAX_URL_LEN (8 * 1024)

ଶୂନ୍ୟ Curl_h1_req_parse_init(ସଂରଚନା h1_req_parser *parser, size_t max_ଧାଡ଼ି_len)
{
  memset(parser, 0, ଆକାର(*parser));
  parser->max_ଧାଡ଼ି_len = max_ଧାଡ଼ି_len;
  curlx_dyn_init(&parser->scratch, max_ଧାଡ଼ି_len);
}

ଶୂନ୍ୟ Curl_h1_req_parse_free(ସଂରଚନା h1_req_parser *parser)
{
  ଯଦି(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = ଖାଲି;
    parser->done = FALSE;
  }
}

ସ୍ଥିର CURLcode trim_ଧାଡ଼ି(ସଂରଚନା h1_req_parser *parser, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
  DEBUGASSERT(parser->ଧାଡ଼ି);
  ଯଦି(parser->ଧାଡ଼ି_len) {
    ଯଦି(parser->ଧାଡ଼ି[parser->ଧାଡ଼ି_len - 1] == '\n')
      --parser->ଧାଡ଼ି_len;
    ଯଦି(parser->ଧାଡ଼ି_len) {
      ଯଦି(parser->ଧାଡ଼ି[parser->ଧାଡ଼ି_len - 1] == '\r')
        --parser->ଧାଡ଼ି_len;
      ଅନ୍ୟଥା ଯଦି(options & H1_PARSE_OPT_STRICT)
        ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
    }
    ଅନ୍ୟଥା ଯଦି(options & H1_PARSE_OPT_STRICT)
      ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }
  ଅନ୍ୟଥା ଯଦି(options & H1_PARSE_OPT_STRICT)
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;

  ଯଦି(parser->ଧାଡ଼ି_len > parser->max_ଧାଡ଼ି_len) {
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode detect_ଧାଡ଼ି(ସଂରଚନା h1_req_parser *parser,
                            ଅଚଳ uint8_t *buf, ଅଚଳ size_t buflen,
                            size_t *pnread)
{
  ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି_end;

  DEBUGASSERT(!parser->ଧାଡ଼ି);
  *pnread = 0;
  ଧାଡ଼ି_end = memchr(buf, '\n', buflen);
  ଯଦି(!ଧାଡ଼ି_end)
    ଫେରନ୍ତୁ CURLE_AGAIN;
  parser->ଧାଡ଼ି = (ଅଚଳ ଅକ୍ଷର *)buf;
  parser->ଧାଡ଼ି_len = ଧାଡ଼ି_end - parser->ଧାଡ଼ି + 1;
  *pnread = parser->ଧାଡ଼ି_len;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode next_ଧାଡ଼ି(ସଂରଚନା h1_req_parser *parser,
                          ଅଚଳ uint8_t *buf, ଅଚଳ size_t buflen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  ଯଦି(parser->ଧାଡ଼ି) {
    parser->ଧାଡ଼ି = ଖାଲି;
    parser->ଧାଡ଼ି_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_ଧାଡ଼ି(parser, buf, buflen, pnread);
  ଯଦି(!result) {
    ଯଦି(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->ଧାଡ଼ି,
                              parser->ଧାଡ଼ି_len);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      parser->ଧାଡ଼ି = curlx_dyn_ptr(&parser->scratch);
      parser->ଧାଡ଼ି_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_ଧାଡ଼ି(parser, options);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା ଯଦି(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (ଅଚଳ unsigned ଅକ୍ଷର *)buf,
                            buflen);
    *pnread = buflen;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode start_req(ସଂରଚନା h1_req_parser *parser,
                          ଅଚଳ ଅକ୍ଷର *scheme_default,
                          ଅଚଳ ଅକ୍ଷର *custom_method,
                          ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
  ଅଚଳ ଅକ୍ଷର *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = ଖାଲି;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  ଯଦି(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->ଧାଡ଼ି, strlen(custom_method))) {
    p = parser->ଧାଡ଼ି + strlen(custom_method);
  }
  ଅନ୍ୟଥା {
    p = memchr(parser->ଧାଡ଼ି, ' ', parser->ଧାଡ଼ି_len);
    ଯଦି(!p || p == parser->ଧାଡ଼ି)
      ଯାଅ out;
  }

  m = parser->ଧାଡ଼ି;
  m_len = p - parser->ଧାଡ଼ି;
  target = p + 1;
  target_len = hv_len = 0;
  hv = ଖାଲି;

  /* URL may contain spaces so scan backwards */
  ପାଇଁ(i = parser->ଧାଡ଼ି_len; i > m_len; --i) {
    ଯଦି(parser->ଧାଡ଼ି[i] == ' ') {
      hv = &parser->ଧାଡ଼ି[i + 1];
      hv_len = parser->ଧାଡ଼ି_len - i;
      target_len = (hv - target) - 1;
      ଭାଙ୍ଗ;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  ଯଦି(!target_len || !hv_len)
    ଯାଅ out;

  (ଶୂନ୍ୟ)hv;

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
  scheme = authority = path = ଖାଲି;
  scheme_len = authority_len = path_len = 0;

  ଯଦି(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  ଅନ୍ୟଥା ଯଦି(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  ଅନ୍ୟଥା {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    ଅକ୍ଷର tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    ଯଦି(target_len >= ଆକାର(tmp))
      ଯାଅ out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    ଯଦି(Curl_is_absolute_url(tmp, ଖାଲି, 0, FALSE)) {
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା url_options;

      url = curl_url();
      ଯଦି(!url) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      ଯଦି(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      ଯଦି(uc) {
        ଯାଅ out;
      }
    }

    ଯଦି(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      ଯାଅ out;
    }
  }

  ଯଦି(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  ଅନ୍ୟଥା {
    ଯଦି(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = strlen(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_h1_req_parse_read(ସଂରଚନା h1_req_parser *parser,
                                ଅଚଳ uint8_t *buf, size_t buflen,
                                ଅଚଳ ଅକ୍ଷର *scheme_default,
                                ଅଚଳ ଅକ୍ଷର *custom_method,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା options, size_t *pnread)
{
  CURLcode result = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  ଯଦି(!buf)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯେତେବେଳେ(!parser->done) {
    result = next_ଧାଡ଼ି(parser, buf, buflen, options, &nread);
    ଯଦି(result) {
      ଯଦି(result == CURLE_AGAIN)
        result = CURLE_OK;
      ଯାଅ out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    ଯଦି(!parser->ଧାଡ଼ି) {
      /* consumed bytes, but line not complete */
      ଯଦି(!buflen)
        ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      ଯଦି(result)
        ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(parser->ଧାଡ଼ି_len == 0) {
      /* last, empty line, we are finished */
      ଯଦି(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        ଯାଅ out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    ଅନ୍ୟଥା {
      result = Curl_dynhds_h1_add_ଧାଡ଼ି(&parser->req->headers,
                                       parser->ଧାଡ଼ି, parser->ଧାଡ଼ି_len);
      ଯଦି(result)
        ଯାଅ out;
    }
  }

out:
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_h1_req_write_head(ସଂରଚନା httpreq *req, ପୂର୍ଣ୍ଣସଂଖ୍ୟା http_minor,
                                ସଂରଚନା dynbuf *dbuf)
{
  CURLcode result;

  result = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  ଯଦି(result)
    ଯାଅ out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  ଯଦି(result)
    ଯାଅ out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_HTTP */
