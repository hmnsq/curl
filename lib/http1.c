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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "http1.h"
#అన్తర్భావ "urlapi-int.h"


#నిర్వచించు H1_MAX_URL_LEN (8 * 1024)

శూన్య Curl_h1_req_parse_init(నిర్మాణం h1_req_parser *parser, size_t max_పంక్తి_len)
{
  memset(parser, 0, పరిమాణం(*parser));
  parser->max_పంక్తి_len = max_పంక్తి_len;
  curlx_dyn_init(&parser->scratch, max_పంక్తి_len);
}

శూన్య Curl_h1_req_parse_free(నిర్మాణం h1_req_parser *parser)
{
  ఒకవేళ(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = ఖాళీ;
    parser->పూర్తి = FALSE;
  }
}

స్థిర CURLcode trim_పంక్తి(నిర్మాణం h1_req_parser *parser, పూర్ణాంకం options)
{
  DEBUGASSERT(parser->పంక్తి);
  ఒకవేళ(parser->పంక్తి_len) {
    ఒకవేళ(parser->పంక్తి[parser->పంక్తి_len - 1] == '\n')
      --parser->పంక్తి_len;
    ఒకవేళ(parser->పంక్తి_len) {
      ఒకవేళ(parser->పంక్తి[parser->పంక్తి_len - 1] == '\r')
        --parser->పంక్తి_len;
      లేకపోతే ఒకవేళ(options & H1_PARSE_OPT_STRICT)
        తిరిగి CURLE_URL_MALFORMAT;
    }
    లేకపోతే ఒకవేళ(options & H1_PARSE_OPT_STRICT)
      తిరిగి CURLE_URL_MALFORMAT;
  }
  లేకపోతే ఒకవేళ(options & H1_PARSE_OPT_STRICT)
    తిరిగి CURLE_URL_MALFORMAT;

  ఒకవేళ(parser->పంక్తి_len > parser->max_పంక్తి_len) {
    తిరిగి CURLE_URL_MALFORMAT;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode detect_పంక్తి(నిర్మాణం h1_req_parser *parser,
                            స్థిరరాశి uint8_t *buf, స్థిరరాశి size_t buflen,
                            size_t *pnread)
{
  స్థిరరాశి అక్షరం *పంక్తి_చివర;

  DEBUGASSERT(!parser->పంక్తి);
  *pnread = 0;
  పంక్తి_చివర = memchr(buf, '\n', buflen);
  ఒకవేళ(!పంక్తి_చివర)
    తిరిగి CURLE_AGAIN;
  parser->పంక్తి = (స్థిరరాశి అక్షరం *)buf;
  parser->పంక్తి_len = పంక్తి_చివర - parser->పంక్తి + 1;
  *pnread = parser->పంక్తి_len;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode తదుపరి_పంక్తి(నిర్మాణం h1_req_parser *parser,
                          స్థిరరాశి uint8_t *buf, స్థిరరాశి size_t buflen, పూర్ణాంకం options,
                          size_t *pnread)
{
  CURLcode ఫలితం;

  *pnread = 0;
  ఒకవేళ(parser->పంక్తి) {
    parser->పంక్తి = ఖాళీ;
    parser->పంక్తి_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  ఫలితం = detect_పంక్తి(parser, buf, buflen, pnread);
  ఒకవేళ(!ఫలితం) {
    ఒకవేళ(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      ఫలితం = curlx_dyn_addn(&parser->scratch, parser->పంక్తి,
                              parser->పంక్తి_len);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      parser->పంక్తి = curlx_dyn_ptr(&parser->scratch);
      parser->పంక్తి_len = curlx_dyn_len(&parser->scratch);
    }
    ఫలితం = trim_పంక్తి(parser, options);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  లేకపోతే ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    ఫలితం = curlx_dyn_addn(&parser->scratch, (స్థిరరాశి గుర్తులేని అక్షరం *)buf,
                            buflen);
    *pnread = buflen;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ప్రారంభం_req(నిర్మాణం h1_req_parser *parser,
                          స్థిరరాశి అక్షరం *scheme_default,
                          స్థిరరాశి అక్షరం *custom_పద్ధతి,
                          పూర్ణాంకం options)
{
  స్థిరరాశి అక్షరం *p, *m, *లక్ష్యం, *hv, *scheme, *authority, *మార్గం;
  size_t m_len, లక్ష్యం_len, hv_len, scheme_len, authority_len, మార్గం_len;
  size_t i;
  CURLU *url = ఖాళీ;
  CURLcode ఫలితం = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  ఒకవేళ(custom_పద్ధతి && custom_పద్ధతి[0] &&
     !అక్షరశ్రేణి_పోలిక(custom_పద్ధతి, parser->పంక్తి, అక్షరశ్రేణి_పొడవు(custom_పద్ధతి))) {
    p = parser->పంక్తి + అక్షరశ్రేణి_పొడవు(custom_పద్ధతి);
  }
  లేకపోతే {
    p = memchr(parser->పంక్తి, ' ', parser->పంక్తి_len);
    ఒకవేళ(!p || p == parser->పంక్తి)
      వెళ్ళు బహిర్గతం;
  }

  m = parser->పంక్తి;
  m_len = p - parser->పంక్తి;
  లక్ష్యం = p + 1;
  లక్ష్యం_len = hv_len = 0;
  hv = ఖాళీ;

  /* URL may contain spaces so scan backwards */
  కొరకు(i = parser->పంక్తి_len; i > m_len; --i) {
    ఒకవేళ(parser->పంక్తి[i] == ' ') {
      hv = &parser->పంక్తి[i + 1];
      hv_len = parser->పంక్తి_len - i;
      లక్ష్యం_len = (hv - లక్ష్యం) - 1;
      ఆపు;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  ఒకవేళ(!లక్ష్యం_len || !hv_len)
    వెళ్ళు బహిర్గతం;

  (శూన్య)hv;

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
  scheme = authority = మార్గం = ఖాళీ;
  scheme_len = authority_len = మార్గం_len = 0;

  ఒకవేళ(లక్ష్యం_len == 1 && లక్ష్యం[0] == '*') {
    /* asterisk-form */
    మార్గం = లక్ష్యం;
    మార్గం_len = లక్ష్యం_len;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక("CONNECT", m, m_len)) {
    /* authority-form */
    authority = లక్ష్యం;
    authority_len = లక్ష్యం_len;
  }
  లేకపోతే ఒకవేళ(లక్ష్యం[0] == '/') {
    /* origin-form */
    మార్గం = లక్ష్యం;
    మార్గం_len = లక్ష్యం_len;
  }
  లేకపోతే {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    అక్షరం tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    మార్గం = లక్ష్యం;
    మార్గం_len = లక్ష్యం_len;

    /* URL parser wants null-termination */
    ఒకవేళ(లక్ష్యం_len >= పరిమాణం(tmp))
      వెళ్ళు బహిర్గతం;
    జ్ఞాపకం_నకలు(tmp, లక్ష్యం, లక్ష్యం_len);
    tmp[లక్ష్యం_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    ఒకవేళ(Curl_is_absolute_url(tmp, ఖాళీ, 0, FALSE)) {
      గుర్తులేని పూర్ణాంకం url_options;

      url = curl_url();
      ఒకవేళ(!url) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_మార్గం_AS_IS |
                     CURLU_NO_DEFAULT_పోర్టు);
      ఒకవేళ(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_సమితి(url, CURLUPART_URL, tmp, url_options);
      ఒకవేళ(uc) {
        వెళ్ళు బహిర్గతం;
      }
    }

    ఒకవేళ(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      వెళ్ళు బహిర్గతం;
    }
  }

  ఒకవేళ(url) {
    ఫలితం = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  లేకపోతే {
    ఒకవేళ(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = అక్షరశ్రేణి_పొడవు(scheme_default);
    }
    ఫలితం = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, మార్గం, మార్గం_len);
  }

బహిర్గతం:
  curl_url_cleanup(url);
  తిరిగి ఫలితం;
}

CURLcode Curl_h1_req_parse_read(నిర్మాణం h1_req_parser *parser,
                                స్థిరరాశి uint8_t *buf, size_t buflen,
                                స్థిరరాశి అక్షరం *scheme_default,
                                స్థిరరాశి అక్షరం *custom_పద్ధతి,
                                పూర్ణాంకం options, size_t *pnread)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  ఒకవేళ(!buf)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  అయితే(!parser->పూర్తి) {
    ఫలితం = తదుపరి_పంక్తి(parser, buf, buflen, options, &nread);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_AGAIN)
        ఫలితం = CURLE_OK;
      వెళ్ళు బహిర్గతం;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    ఒకవేళ(!parser->పంక్తి) {
      /* consumed bytes, but line not complete */
      ఒకవేళ(!buflen)
        వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(!parser->req) {
      ఫలితం = ప్రారంభం_req(parser, scheme_default, custom_పద్ధతి, options);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(parser->పంక్తి_len == 0) {
      /* last, empty line, we are finished */
      ఒకవేళ(!parser->req) {
        ఫలితం = CURLE_URL_MALFORMAT;
        వెళ్ళు బహిర్గతం;
      }
      parser->పూర్తి = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    లేకపోతే {
      ఫలితం = Curl_dynhds_h1_కూడు_పంక్తి(&parser->req->శీర్షికలు,
                                       parser->పంక్తి, parser->పంక్తి_len);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
  }

బహిర్గతం:
  తిరిగి ఫలితం;
}

CURLcode Curl_h1_req_write_తల(నిర్మాణం httpreq *req, పూర్ణాంకం http_minor,
                                నిర్మాణం dynbuf *dbuf)
{
  CURLcode ఫలితం;

  ఫలితం = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->పద్ధతి,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->మార్గం ? req->మార్గం : "",
                          http_minor);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_dynhds_h1_dprint(&req->శీర్షికలు, dbuf);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

బహిర్గతం:
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */
