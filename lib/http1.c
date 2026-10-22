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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "http.h"
#અન્તર્ભાવ "http1.h"
#અન્તર્ભાવ "urlapi-int.h"


#define H1_MAX_URL_LEN (8 * 1024)

શૂન્ય Curl_h1_req_parse_init(સંરચના h1_req_parser *parser, size_t max_લીટી_len)
{
  memset(parser, 0, કદ(*parser));
  parser->max_લીટી_len = max_લીટી_len;
  curlx_dyn_init(&parser->scratch, max_લીટી_len);
}

શૂન્ય Curl_h1_req_parse_free(સંરચના h1_req_parser *parser)
{
  જો(parser) {
    Curl_http_req_free(parser->req);
    curlx_dyn_free(&parser->scratch);
    parser->req = ખાલી;
    parser->done = FALSE;
  }
}

સ્થિર CURLcode trim_લીટી(સંરચના h1_req_parser *parser, પૂર્ણાંક options)
{
  DEBUGASSERT(parser->લીટી);
  જો(parser->લીટી_len) {
    જો(parser->લીટી[parser->લીટી_len - 1] == '\n')
      --parser->લીટી_len;
    જો(parser->લીટી_len) {
      જો(parser->લીટી[parser->લીટી_len - 1] == '\r')
        --parser->લીટી_len;
      નહીંતર જો(options & H1_PARSE_OPT_STRICT)
        પાછા CURLE_URL_MALFORMAT;
    }
    નહીંતર જો(options & H1_PARSE_OPT_STRICT)
      પાછા CURLE_URL_MALFORMAT;
  }
  નહીંતર જો(options & H1_PARSE_OPT_STRICT)
    પાછા CURLE_URL_MALFORMAT;

  જો(parser->લીટી_len > parser->max_લીટી_len) {
    પાછા CURLE_URL_MALFORMAT;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode detect_લીટી(સંરચના h1_req_parser *parser,
                            અચલ uint8_t *buf, અચલ size_t buflen,
                            size_t *pnread)
{
  અચલ અક્ષર *લીટી_end;

  DEBUGASSERT(!parser->લીટી);
  *pnread = 0;
  લીટી_end = memchr(buf, '\n', buflen);
  જો(!લીટી_end)
    પાછા CURLE_AGAIN;
  parser->લીટી = (અચલ અક્ષર *)buf;
  parser->લીટી_len = લીટી_end - parser->લીટી + 1;
  *pnread = parser->લીટી_len;
  પાછા CURLE_OK;
}

સ્થિર CURLcode next_લીટી(સંરચના h1_req_parser *parser,
                          અચલ uint8_t *buf, અચલ size_t buflen, પૂર્ણાંક options,
                          size_t *pnread)
{
  CURLcode result;

  *pnread = 0;
  જો(parser->લીટી) {
    parser->લીટી = ખાલી;
    parser->લીટી_len = 0;
    curlx_dyn_reset(&parser->scratch);
  }

  result = detect_લીટી(parser, buf, buflen, pnread);
  જો(!result) {
    જો(curlx_dyn_len(&parser->scratch)) {
      /* append detected line to scratch to have the complete line */
      result = curlx_dyn_addn(&parser->scratch, parser->લીટી,
                              parser->લીટી_len);
      જો(result)
        પાછા result;
      parser->લીટી = curlx_dyn_ptr(&parser->scratch);
      parser->લીટી_len = curlx_dyn_len(&parser->scratch);
    }
    result = trim_લીટી(parser, options);
    જો(result)
      પાછા result;
  }
  નહીંતર જો(result == CURLE_AGAIN) {
    /* no line end in `buf`, add it to our scratch */
    result = curlx_dyn_addn(&parser->scratch, (અચલ unsigned અક્ષર *)buf,
                            buflen);
    *pnread = buflen;
  }
  પાછા result;
}

સ્થિર CURLcode start_req(સંરચના h1_req_parser *parser,
                          અચલ અક્ષર *scheme_default,
                          અચલ અક્ષર *custom_method,
                          પૂર્ણાંક options)
{
  અચલ અક્ષર *p, *m, *target, *hv, *scheme, *authority, *path;
  size_t m_len, target_len, hv_len, scheme_len, authority_len, path_len;
  size_t i;
  CURLU *url = ખાલી;
  CURLcode result = CURLE_URL_MALFORMAT; /* Use this as default fail */

  DEBUGASSERT(!parser->req);
  /* line must match: "METHOD TARGET HTTP_VERSION" */
  જો(custom_method && custom_method[0] &&
     !strncmp(custom_method, parser->લીટી, strlen(custom_method))) {
    p = parser->લીટી + strlen(custom_method);
  }
  નહીંતર {
    p = memchr(parser->લીટી, ' ', parser->લીટી_len);
    જો(!p || p == parser->લીટી)
      જાઓ out;
  }

  m = parser->લીટી;
  m_len = p - parser->લીટી;
  target = p + 1;
  target_len = hv_len = 0;
  hv = ખાલી;

  /* URL may contain spaces so scan backwards */
  માટે(i = parser->લીટી_len; i > m_len; --i) {
    જો(parser->લીટી[i] == ' ') {
      hv = &parser->લીટી[i + 1];
      hv_len = parser->લીટી_len - i;
      target_len = (hv - target) - 1;
      તોડો;
    }
  }
  /* no SPACE found or empty TARGET or empty HTTP_VERSION */
  જો(!target_len || !hv_len)
    જાઓ out;

  (શૂન્ય)hv;

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
  scheme = authority = path = ખાલી;
  scheme_len = authority_len = path_len = 0;

  જો(target_len == 1 && target[0] == '*') {
    /* asterisk-form */
    path = target;
    path_len = target_len;
  }
  નહીંતર જો(!strncmp("CONNECT", m, m_len)) {
    /* authority-form */
    authority = target;
    authority_len = target_len;
  }
  નહીંતર જો(target[0] == '/') {
    /* origin-form */
    path = target;
    path_len = target_len;
  }
  નહીંતર {
    /* origin-form OR absolute-form */
    CURLUcode uc;
    અક્ષર tmp[H1_MAX_URL_LEN];

    /* default, unless we see an absolute URL */
    path = target;
    path_len = target_len;

    /* URL parser wants null-termination */
    જો(target_len >= કદ(tmp))
      જાઓ out;
    memcpy(tmp, target, target_len);
    tmp[target_len] = '\0';
    /* See if treating TARGET as an absolute URL makes sense */
    જો(Curl_is_absolute_url(tmp, ખાલી, 0, FALSE)) {
      unsigned પૂર્ણાંક url_options;

      url = curl_url();
      જો(!url) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
      url_options = (CURLU_NON_SUPPORT_SCHEME |
                     CURLU_PATH_AS_IS |
                     CURLU_NO_DEFAULT_PORT);
      જો(!(options & H1_PARSE_OPT_STRICT))
        url_options |= CURLU_ALLOW_SPACE;
      uc = curl_url_set(url, CURLUPART_URL, tmp, url_options);
      જો(uc) {
        જાઓ out;
      }
    }

    જો(!url && (options & H1_PARSE_OPT_STRICT)) {
      /* we should have an absolute URL or have seen `/` earlier */
      જાઓ out;
    }
  }

  જો(url) {
    result = Curl_http_req_make2(&parser->req, m, m_len, url, scheme_default);
  }
  નહીંતર {
    જો(!scheme && scheme_default) {
      scheme = scheme_default;
      scheme_len = strlen(scheme_default);
    }
    result = Curl_http_req_make(&parser->req, m, m_len, scheme, scheme_len,
                                authority, authority_len, path, path_len);
  }

out:
  curl_url_cleanup(url);
  પાછા result;
}

CURLcode Curl_h1_req_parse_read(સંરચના h1_req_parser *parser,
                                અચલ uint8_t *buf, size_t buflen,
                                અચલ અક્ષર *scheme_default,
                                અચલ અક્ષર *custom_method,
                                પૂર્ણાંક options, size_t *pnread)
{
  CURLcode result = CURLE_OK;
  size_t nread;

  *pnread = 0;

  DEBUGASSERT(buf);
  જો(!buf)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જ્યારે(!parser->done) {
    result = next_લીટી(parser, buf, buflen, options, &nread);
    જો(result) {
      જો(result == CURLE_AGAIN)
        result = CURLE_OK;
      જાઓ out;
    }

    /* Consume this line */
    *pnread += nread;
    buf += nread;
    buflen -= nread;

    જો(!parser->લીટી) {
      /* consumed bytes, but line not complete */
      જો(!buflen)
        જાઓ out;
    }
    નહીંતર જો(!parser->req) {
      result = start_req(parser, scheme_default, custom_method, options);
      જો(result)
        જાઓ out;
    }
    નહીંતર જો(parser->લીટી_len == 0) {
      /* last, empty line, we are finished */
      જો(!parser->req) {
        result = CURLE_URL_MALFORMAT;
        જાઓ out;
      }
      parser->done = TRUE;
      curlx_dyn_reset(&parser->scratch);
      /* last chance adjustments */
    }
    નહીંતર {
      result = Curl_dynhds_h1_add_લીટી(&parser->req->headers,
                                       parser->લીટી, parser->લીટી_len);
      જો(result)
        જાઓ out;
    }
  }

out:
  પાછા result;
}

CURLcode Curl_h1_req_write_head(સંરચના httpreq *req, પૂર્ણાંક http_minor,
                                સંરચના dynbuf *dbuf)
{
  CURLcode result;

  result = curlx_dyn_addf(dbuf, "%s %s%s%s%s HTTP/1.%d\r\n",
                          req->method,
                          req->scheme ? req->scheme : "",
                          req->scheme ? "://" : "",
                          req->authority ? req->authority : "",
                          req->path ? req->path : "",
                          http_minor);
  જો(result)
    જાઓ out;

  result = Curl_dynhds_h1_dprint(&req->headers, dbuf);
  જો(result)
    જાઓ out;

  result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));

out:
  પાછા result;
}

#endif /* !CURL_DISABLE_HTTP */
