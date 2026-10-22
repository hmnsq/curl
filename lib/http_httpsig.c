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

#ܐܢ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#ضمن "urldata.h"
#ضمن "http_httpsig.h"
#ضمن "curl_ed25519.h"
#ضمن "curl_hmac.h"
#ضمن "curl_sha256.h"
#ضمن "http.h"
#ضمن "transfer.h"
#ضمن "curl_trc.h"
#ضمن "slist.h"
#ضمن "curlx/dynbuf.h"
#ضمن "curlx/base64.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/strparse.h"
#ضمن "strcase.h"

#ضمن <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

تعداد httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

ساكن ثابت حرف *alg_to_str(تعداد httpsig_alg alg)
{
  بدل(alg) {
  حالة HTTPSIG_ALG_ED25519:
    ܦܢܐ "ed25519";
  حالة HTTPSIG_ALG_HMAC_SHA256:
    ܦܢܐ "hmac-sha256";
  افتراضي:
    اكسر;
  }
  ܦܢܐ عدم;
}

ساكن تعداد httpsig_alg id_to_alg(uint8_t val)
{
  بدل(val) {
  حالة CURLHTTPSIG_ED25519:
    ܦܢܐ HTTPSIG_ALG_ED25519;
  حالة CURLHTTPSIG_HMAC_SHA256:
    ܦܢܐ HTTPSIG_ALG_HMAC_SHA256;
  افتراضي:
    اكسر;
  }
  ܦܢܐ HTTPSIG_ALG_UNKNOWN;
}

ساكن CURLcode decode_hex_key(بنية Curl_easy *data,
                               ثابت حرف *hexstr,
                               unsigned حرف **keyout,
                               size_t *keylen)
{
  size_t len, i;
  unsigned حرف *keybuf;

  *keyout = عدم;
  *keylen = 0;

  len = strlen(hexstr);
  ܟܡܐ(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  ܐܢ(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ܐܢ(len > CURL_MAX_INPUT_LENGTH) {
    failf(data, "httpsig: hex key too long");
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  ܐܢ(!keybuf)
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  لكل(i = 0; i < len; i += 2) {
    ܐܢ(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned حرف)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  ܦܢܐ CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ساكن CURLcode httpsig_authority(بنية Curl_easy *data,
                                  بنية connectdata *conn,
                                  بنية dynbuf *authority_buf)
{
  ثابت حرف *h = data->state.http_host;

  ܐܢ(h && curl_strnequal(h, "host:", 5)) {
    ثابت حرف *value = h + 5;
    ثابت حرف *end;

    ܟܡܐ(ISBLANK(*value))
      value++;
    ܐܢ(*value) {
      CURLcode result;

      end = value;
      ܟܡܐ(*end && !ISNEWLINE(*end))
        end++;
      ܟܡܐ(end > value && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, value, (size_t)(end - value));
      ܐܢ(result)
        ܦܢܐ result;
      ܦܢܐ CURLE_OK;
    }
  }

  {
    ثابت حرف *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    ܐܢ((conn->origin->scheme->defport != port) && port)
      ܦܢܐ curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    ܦܢܐ curlx_dyn_add(authority_buf, hostname);
  }
}

ساكن CURLcode sf_append_quoted(بنية dynbuf *buf, ثابت حرف *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  ܐܢ(result)
    ܦܢܐ result;
  ܟܡܐ(*str) {
    ܐܢ(ISCNTRL(*str))
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܢ(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      ܐܢ(result)
        ܦܢܐ result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    ܐܢ(result)
      ܦܢܐ result;
    str++;
  }
  ܦܢܐ curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ساكن CURLcode sf_encode_byte_seq(ثابت unsigned حرف *raw, size_t rawlen,
                                   بنية dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  حرف *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  ܐܢ(result)
    ܦܢܐ result;

  result = curlx_dyn_addn(out, ":", 1);
  ܐܢ(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  ܐܢ(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  ܦܢܐ result;
}

ساكن CURLcode build_sig_params(بنية dynbuf *params,
                                 ثابت حرف **components, size_t count,
                                 time_t created, ثابت حرف *keyid,
                                 تعداد httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  ܐܢ(result)
    ܦܢܐ result;

  لكل(i = 0; i < count; i++) {
    ܐܢ(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      ܐܢ(result)
        ܦܢܐ result;
    }
    result = sf_append_quoted(params, components[i]);
    ܐܢ(result)
      ܦܢܐ result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  ܐܢ(result)
    ܦܢܐ result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  ܐܢ(result)
    ܦܢܐ result;

  ܐܢ(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    ܐܢ(result)
      ܦܢܐ result;
    result = sf_append_quoted(params, keyid);
    ܐܢ(result)
      ܦܢܐ result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  ܦܢܐ result;
}

/* strings defined by RFC 9421 */
#define SIG_METHOD    "@method"
#define SIG_AUTHORITY "@authority"
#define SIG_PATH      "@path"
#define SIG_QUERY     "@query"

/* Resolve a component identifier to its value.
 * For headers, we walk the full user-supplied header list to combine
 * duplicate field values with ", " per RFC 9421 Section 2.1. Each
 * individual value is trimmed of leading/trailing OWS and the trailing
 * \r\n. The combined result is written into the caller-provided buffer. */
ساكن CURLcode resolve_component(ثابت حرف *name,
                                  ثابت حرف *method,
                                  ثابت حرف *authority,
                                  ثابت حرف *path,
                                  ثابت حرف *query,
                                  بنية Curl_easy *data,
                                  بنية dynbuf *valbuf,
                                  ثابت حرف **out)
{
  *out = عدم;

  ܐܢ(name[0] == '@') {
    ܐܢ(curl_strequal(name, SIG_METHOD))
      *out = method;
    ܐܚܪܢܐ ܐܢ(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    ܐܚܪܢܐ ܐܢ(curl_strequal(name, SIG_PATH))
      *out = path;
    ܐܚܪܢܐ ܐܢ(curl_strequal(name, SIG_QUERY))
      *out = query;
    ܐܚܪܢܐ {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ܐܢ(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ܦܢܐ CURLE_OK;
  }
  ܐܚܪܢܐ {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    بنية curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    لكل(head = data->set.headers; head; head = head->next) {
      ܐܢ(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        ثابت حرف *p = strchr(head->data, ':');
        ܐܢ(p) {
          CURLcode result;
          بنية Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          ܐܢ(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            ܐܢ(result)
              ܦܢܐ result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          ܐܢ(result)
            ܦܢܐ result;
          found = TRUE;
        }
      }
    }

    ܐܢ(found) {
      *out = curlx_dyn_ptr(valbuf);
      ܦܢܐ CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

ساكن CURLcode build_sig_base(بنية dynbuf *base,
                               ثابت حرف **components, size_t count,
                               ثابت حرف *method,
                               ثابت حرف *authority,
                               ثابت حرف *path,
                               ثابت حرف *query,
                               بنية Curl_easy *data,
                               ثابت حرف *sig_params)
{
  CURLcode result;
  size_t i;
  بنية dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  لكل(i = 0; i < count; i++) {
    ثابت حرف *val = عدم;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    ܐܢ(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      ܦܢܐ result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    ܐܢ(result) {
      curlx_dyn_free(&hdrvalbuf);
      ܦܢܐ result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  ܦܢܐ result;
}

ساكن CURLcode parse_components(بنية Curl_easy *data,
                                 ثابت حرف *query,
                                 ثابت حرف **components,
                                 size_t *ncomp_out,
                                 حرف **hdrs_copy_out)
{
  ثابت حرف *hdrs = CURL_EASY_STR(data, STRING_HTTPSIG_HEADERS);
  size_t ncomp = 0;

  *hdrs_copy_out = عدم;
  ܐܢ(hdrs && *hdrs) {
    حرف *p;
    حرف *hdrs_copy = curlx_strdup(hdrs);
    ܐܢ(!hdrs_copy)
      ܦܢܐ CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    ܟܡܐ(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      حرف *start;
      size_t tlen = 0;
      ثابت حرف *p2 = p;

      curlx_str_passblanks(&p2);
      ܐܢ(!*p2)
        اكسر;
      p = start = CURL_UNCONST(p2);
      ܟܡܐ(*p && !ISBLANK(*p)) {
        ܐܢ((*p == '\"') || (*p == '\\'))
          ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      ܐܢ(*p)
        *p++ = '\0';

      ܐܢ(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        ܐܢ(!tlen) {
          failf(data, "httpsig: empty header component name");
          ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      ܐܚܪܢܐ {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        ܐܢ(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        ܐܚܪܢܐ ܐܢ(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        ܐܚܪܢܐ ܐܢ(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        ܐܚܪܢܐ ܐܢ(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        ܐܚܪܢܐ {
          failf(data, "httpsig: unknown component '%s'", start);
          ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    ܐܢ(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ܐܢ(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned صحيح)HTTPSIG_MAX_COMPONENTS);
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      لكل(i = 0; i < ncomp; i++) {
        لكل(j = i + 1; j < ncomp; j++) {
          ܐܢ(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  ܐܚܪܢܐ {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    ܐܢ(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  ܦܢܐ CURLE_OK;
}

ساكن time_t httpsig_get_created(فراغ)
{
#ifdef DEBUGBUILD
  حرف *force = getenv("CURL_FORCETIME");
  ܐܢ(force && *force) {
    حرف *sigts = getenv("CURL_HTTPSIG_CREATED");
    ܐܢ(sigts && *sigts) {
      ثابت حرف *p = sigts;
      curl_off_t num;
      ܐܢ(!curlx_str_number(&p, &num, CURL_OFF_T_MAX))
        ܦܢܐ (time_t)num;
    }
    ܦܢܐ 0;
  }
#endif
  ܦܢܐ time(عدم);
}

ساكن CURLcode httpsig_sign_base(بنية Curl_easy *data,
                                  تعداد httpsig_alg alg,
                                  ثابت unsigned حرف *keybuf,
                                  size_t keylen,
                                  ثابت بنية dynbuf *sig_base,
                                  unsigned حرف *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode result;

  بدل(alg) {
  حالة HTTPSIG_ALG_ED25519:
    result = Curl_ed25519_sign(
      keybuf, keylen,
      (ثابت unsigned حرف *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    اكسر;
  حالة HTTPSIG_ALG_HMAC_SHA256:
    result = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ثابت unsigned حرف *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    ܐܢ(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    اكسر;
  افتراضي:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اكسر;
  }

  ܐܢ(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  ܦܢܐ result;
}

CURLcode Curl_output_httpsig(بنية Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  بنية connectdata *conn = data->conn;
  ثابت حرف *path;
  ثابت حرف *query;
  Curl_HttpReq httpreq;
  ثابت حرف *method = عدم;
  ثابت حرف *hexkey = CURL_EASY_STR(data, STRING_HTTPSIG_KEY);
  ثابت حرف *keyid = CURL_EASY_STR(data, STRING_HTTPSIG_KEYID);
  تعداد httpsig_alg alg;
  time_t created;
  بنية dynbuf sig_params;
  بنية dynbuf sig_base;
  بنية dynbuf sig_hdr;
  بنية dynbuf input_hdr;
  بنية dynbuf authority_buf;
  ثابت حرف *authority;
  ثابت حرف *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  unsigned حرف *keybuf = عدم;
  size_t keylen = 0;
  unsigned حرف raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  حرف *auth_headers = عدم;
  حرف *hdrs_copy = عدم;
  بنية dynbuf query_dyn;

  alg = id_to_alg(data->set.httpsig_algorithm);
  ܐܢ(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ܐܢ(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ܐܢ(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&input_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  ܐܢ(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    اذهب done;
  }

  result = decode_hex_key(data, hexkey, &keybuf, &keylen);
  ܐܢ(result)
    اذهب fail;

  ܐܢ(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  ܐܢ(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  ܐܢ(result)
    اذهب fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  ܐܢ(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  ܐܚܪܢܐ
    result = curlx_dyn_add(&query_dyn, "?");
  ܐܢ(result)
    اذهب fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  ܐܢ(result)
    اذهب fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  ܐܢ(result)
    اذهب fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  ܐܢ(result)
    اذهب fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  ܐܢ(result)
    اذهب fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  ܐܢ(result)
    اذهب fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  ܐܢ(result)
    اذهب fail;

  result = curlx_dyn_addf(&input_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  ܐܢ(result)
    اذهب fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&input_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  ܐܢ(!auth_headers)
    اذهب fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&input_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  ܐܢ(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, حجم(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&input_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  ܦܢܐ result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
