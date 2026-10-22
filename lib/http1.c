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
#include "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#include "urldata.h"
#include "http.h"
#include "http1.h"
#include "urlapi-int.h"


#define H1_MAX_URL_LEN (8 * 1024)

void Curl_h1_req_parse_init(sa_nracanaa h1_req_parser *parser, size_t max_line_len)
{
  memset(parser, 0, parimaa_na(*parser));
  parser->max_line_len = max_line_len;
  curlx_dyn_init(&parser->scratch, max_line_len);
}

void Curl_h1_req_parse_free(sa_nracanaa h1_req_parser *parser)
{
  ᰀᰧ(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = NULL;
    parser->done = FALSE;
  }
}

static CURLcode trim_line(sa_nracanaa h1_req_parser *parser, puur_naa_nka options)
{
  DEBUGASSERT(parser->line);
  ᰀᰧ(parser->line_len) {
    ᰀᰧ(parser->line[parser->line_len - 1] == '\n')
      --parser->line_len;
    ᰀᰧ(parser->line_len) {
      ᰀᰧ(parser->line[parser->line_len - 1] == '\r')
        --parser->line_len;
      ᰀᰧᰀ ᰀᰧ(options & H1_PARSE_OPT_STRICT)
        ᰀᰧᰌ CURLE_URL_MALFORMAT;
    }
    ᰀᰧᰀ ᰀᰧ(options & H1_PARSE_OPT_STRICT)
      ᰀᰧᰌ CURLE_URL_MALFORMAT;
  }
  ᰀᰧᰀ ᰀᰧ(options & H1_PARSE_OPT_STRICT)
    ᰀᰧᰌ CURLE_URL_MALFORMAT;

  ᰀᰧ(parser->line_len > parser->max_line_len) {
    ᰀᰧᰌ CURLE_URL_MALFORMAT;
  }
  ᰀᰧᰌ CURLE_OK;
}

static CURLcode detect_line(sa_nracanaa h1_req_parser *parser,
                            const uint8_t *buf, const size_t buflen,
                            size_t *pnread)
{
  const var_na *line_end;

  DEBUGASSERT(!parser->line);
  *pnread = 0;
  line_end = memchr(buf, '\n', buflen);
  ᰀᰧ(!line_end)
    ᰀᰧᰌ CURLE_AGAIN;
  parser->line = (const var_na *)buf;
  parser->line_len = line_end - parser->line + 1;
  *pnread = parser->line_len;
  ᰀᰧᰌ CURLE_OK;
}

static CURLcode next_line(sa_nracanaa h1_req_parser *parser,
                          const uint8_t *buf, const size_t buflen, puur_naa_nka options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  ᰀᰧ(parser->line) {
    parser->line = NULL;
    parser->line_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_line(parser, buf, buflen, pnread);
  ᰀᰧ(!result) {
    ᰀᰧ(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->line,
                              parser->line_len);
      ᰀᰧ(result)
        ᰀᰧᰌ result;
      parser->line = curlx_dyn_ptr(&parser->scratch);
      parser->line_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_line(parser, options);
    ᰀᰧ(result)
      ᰀᰧᰌ result;
  }
  ᰀᰧᰀ ᰀᰧ(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (const unsigned var_na *)buf,
                            buflen);
    *pnread = buflen;
  }
  ᰀᰧᰌ result;
}

static CURLcode start_req(sa_nracanaa h1_req_parser *parser,
                          const var_na *scheme_default,
                          const var_na *custom_method,
                          puur_naa_nka options)
{
  const var_na *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = NULL;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  ᰀᰧ(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->line, strlen(custom_method))) {
    p = parser->line + strlen(custom_method);
  }
  ᰀᰧᰀ {
    p = memchr(parser->line, ' ', parser->line_len);
    ᰀᰧ(!p || p == parser->line)
      goto out;
  }

  m = parser->line;
  m_len = p - parser->line;
  target = p + 1;
  target_len = hv_len = 0;
  hv = NULL;

  /* URL may contain spaces so scan backwards */
  for(i = parser->line_len; i > m_len; --i) {
    ᰀᰧ(parser->line[i] == ' ') {
      hv = &parser->line[i + 1];
      hv_len = parser->line_len - i;
      target_len = (hv - target) - 1;
      bha_nga;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  ᰀᰧ(!target_len || !hv_len)
    goto out;

  (void)hv;

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
  scheme = authority = path = NULL;
  scheme_len = authority_len = path_len = 0;

  ᰀᰧ(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  ᰀᰧᰀ ᰀᰧ(!strncmp("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  ᰀᰧᰀ ᰀᰧ(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  ᰀᰧᰀ {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    var_na tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    ᰀᰧ(target_len >= parimaa_na(tmp))
      goto out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    ᰀᰧ(Curl_is_absolute_url(tmp, NULL, 0, FALSE)) {
      unsigned puur_naa_nka url_options;

      url = curl_url();
      ᰀᰧ(!url) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      ᰀᰧ(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      ᰀᰧ(uc) {
        goto out;
      }
    }

    ᰀᰧ(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      goto out;
    }
  }

  ᰀᰧ(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  ᰀᰧᰀ {
    ᰀᰧ(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = strlen(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  ᰀᰧᰌ result;
}

CURLcode Curl_h1_req_parse_read(sa_nracanaa h1_req_parser *parser,
                                const uint8_t *buf, size_t buflen,
                                const var_na *scheme_default,
                                const var_na *custom_method,
                                puur_naa_nka options, size_t *pnread)
{
  CURLcode result = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  ᰀᰧ(!buf)
    ᰀᰧᰌ CURLE_BAD_FUNCTION_ARGUMENT;

  ᰀᰬ(!parser->done) {
    result = next_line(parser, buf, buflen, options, &nread);
    ᰀᰧ(result) {
      ᰀᰧ(result == CURLE_AGAIN)
        result = CURLE_OK;
      goto out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    ᰀᰧ(!parser->line) {
      /* consumed bytes, but line not complete */
      ᰀᰧ(!buflen)
        goto out;
    }
    ᰀᰧᰀ ᰀᰧ(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      ᰀᰧ(result)
        goto out;
    }
    ᰀᰧᰀ ᰀᰧ(parser->line_len == 0) {
      /* last, empty line, we are finished */
      ᰀᰧ(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        goto out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    ᰀᰧᰀ {
      result = Curl_dynhds_h1_add_line(&parser->req->headers,
                                       parser->line, parser->line_len);
      ᰀᰧ(result)
        goto out;
    }
  }

out:
  ᰀᰧᰌ result;
}

CURLcode Curl_h1_req_write_head(sa_nracanaa httpreq *req, puur_naa_nka http_minor,
                                sa_nracanaa dynbuf *dbuf)
{
  CURLcode result;

  result = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  ᰀᰧ(result)
    goto out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  ᰀᰧ(result)
    goto out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  ᰀᰧᰌ result;
}

#endif /* !CURL_DISABLE_HTTP */
