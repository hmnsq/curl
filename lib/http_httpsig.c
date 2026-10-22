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

#𐡄𐡍 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

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
    أرجع "ed25519";
  حالة HTTPSIG_ALG_HMAC_SHA256:
    أرجع "hmac-sha256";
  افتراضي:
    اكسر;
  }
  أرجع عدم;
}

ساكن تعداد httpsig_alg id_to_alg(uint8_t val)
{
  بدل(val) {
  حالة CURLHTTPSIG_ED25519:
    أرجع HTTPSIG_ALG_ED25519;
  حالة CURLHTTPSIG_HMAC_SHA256:
    أرجع HTTPSIG_ALG_HMAC_SHA256;
  افتراضي:
    اكسر;
  }
  أرجع HTTPSIG_ALG_UNKNOWN;
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
  طالما(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  𐡄𐡍(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }

  𐡄𐡍(len > CURL_MAX_INPUT_LENGTH) {
    failf(data, "httpsig: hex key too long");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  𐡄𐡍(!keybuf)
    أرجع CURLE_OUT_OF_MEMORY;

  لكل(i = 0; i < len; i += 2) {
    𐡄𐡍(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned حرف)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  أرجع CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ساكن CURLcode httpsig_authority(بنية Curl_easy *data,
                                  بنية connectdata *conn,
                                  بنية dynbuf *authority_buf)
{
  ثابت حرف *h = data->state.http_host;

  𐡄𐡍(h && curl_strnequal(h, "host:", 5)) {
    ثابت حرف *value = h + 5;
    ثابت حرف *end;

    طالما(ISBLANK(*value))
      value++;
    𐡄𐡍(*value) {
      CURLcode result;

      end = value;
      طالما(*end && !ISNEWLINE(*end))
        end++;
      طالما(end > value && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, value, (size_t)(end - value));
      𐡄𐡍(result)
        أرجع result;
      أرجع CURLE_OK;
    }
  }

  {
    ثابت حرف *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    𐡄𐡍((conn->origin->scheme->defport != port) && port)
      أرجع curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    أرجع curlx_dyn_add(authority_buf, hostname);
  }
}

ساكن CURLcode sf_append_quoted(بنية dynbuf *buf, ثابت حرف *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  𐡄𐡍(result)
    أرجع result;
  طالما(*str) {
    𐡄𐡍(ISCNTRL(*str))
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    𐡄𐡍(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      𐡄𐡍(result)
        أرجع result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    𐡄𐡍(result)
      أرجع result;
    str++;
  }
  أرجع curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ساكن CURLcode sf_encode_byte_seq(ثابت unsigned حرف *raw, size_t rawlen,
                                   بنية dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  حرف *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  𐡄𐡍(result)
    أرجع result;

  result = curlx_dyn_addn(out, ":", 1);
  𐡄𐡍(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  𐡄𐡍(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  أرجع result;
}

ساكن CURLcode build_sig_params(بنية dynbuf *params,
                                 ثابت حرف **components, size_t count,
                                 time_t created, ثابت حرف *keyid,
                                 تعداد httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  𐡄𐡍(result)
    أرجع result;

  لكل(i = 0; i < count; i++) {
    𐡄𐡍(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      𐡄𐡍(result)
        أرجع result;
    }
    result = sf_append_quoted(params, components[i]);
    𐡄𐡍(result)
      أرجع result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  𐡄𐡍(result)
    أرجع result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  𐡄𐡍(result)
    أرجع result;

  𐡄𐡍(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    𐡄𐡍(result)
      أرجع result;
    result = sf_append_quoted(params, keyid);
    𐡄𐡍(result)
      أرجع result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  أرجع result;
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

  𐡄𐡍(name[0] == '@') {
    𐡄𐡍(curl_strequal(name, SIG_METHOD))
      *out = method;
    وإلا 𐡄𐡍(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    وإلا 𐡄𐡍(curl_strequal(name, SIG_PATH))
      *out = path;
    وإلا 𐡄𐡍(curl_strequal(name, SIG_QUERY))
      *out = query;
    وإلا {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    }
    𐡄𐡍(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    }
    أرجع CURLE_OK;
  }
  وإلا {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    بنية curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    لكل(head = data->set.headers; head; head = head->next) {
      𐡄𐡍(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        ثابت حرف *p = strchr(head->data, ':');
        𐡄𐡍(p) {
          CURLcode result;
          بنية Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          𐡄𐡍(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            𐡄𐡍(result)
              أرجع result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          𐡄𐡍(result)
            أرجع result;
          found = TRUE;
        }
      }
    }

    𐡄𐡍(found) {
      *out = curlx_dyn_ptr(valbuf);
      أرجع CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
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
    𐡄𐡍(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      أرجع result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    𐡄𐡍(result) {
      curlx_dyn_free(&hdrvalbuf);
      أرجع result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  أرجع result;
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
  𐡄𐡍(hdrs && *hdrs) {
    حرف *p;
    حرف *hdrs_copy = curlx_strdup(hdrs);
    𐡄𐡍(!hdrs_copy)
      أرجع CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    طالما(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      حرف *start;
      size_t tlen = 0;
      ثابت حرف *p2 = p;

      curlx_str_passblanks(&p2);
      𐡄𐡍(!*p2)
        اكسر;
      p = start = CURL_UNCONST(p2);
      طالما(*p && !ISBLANK(*p)) {
        𐡄𐡍((*p == '\"') || (*p == '\\'))
          أرجع CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      𐡄𐡍(*p)
        *p++ = '\0';

      𐡄𐡍(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        𐡄𐡍(!tlen) {
          failf(data, "httpsig: empty header component name");
          أرجع CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      وإلا {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        𐡄𐡍(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        وإلا 𐡄𐡍(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        وإلا 𐡄𐡍(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        وإلا 𐡄𐡍(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        وإلا {
          failf(data, "httpsig: unknown component '%s'", start);
          أرجع CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    𐡄𐡍(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    }
    𐡄𐡍(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned صحيح)HTTPSIG_MAX_COMPONENTS);
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      لكل(i = 0; i < ncomp; i++) {
        لكل(j = i + 1; j < ncomp; j++) {
          𐡄𐡍(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            أرجع CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  وإلا {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    𐡄𐡍(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  أرجع CURLE_OK;
}

ساكن time_t httpsig_get_created(فراغ)
{
#ifdef DEBUGBUILD
  حرف *force = getenv("CURL_FORCETIME");
  𐡄𐡍(force && *force) {
    حرف *sigts = getenv("CURL_HTTPSIG_CREATED");
    𐡄𐡍(sigts && *sigts) {
      ثابت حرف *p = sigts;
      curl_off_t num;
      𐡄𐡍(!curlx_str_number(&p, &num, CURL_OFF_T_MAX))
        أرجع (time_t)num;
    }
    أرجع 0;
  }
#endif
  أرجع time(عدم);
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
    𐡄𐡍(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    اكسر;
  افتراضي:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اكسر;
  }

  𐡄𐡍(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  أرجع result;
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
  𐡄𐡍(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }

  𐡄𐡍(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }
  𐡄𐡍(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&input_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  𐡄𐡍(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    اذهب done;
  }

  result = decode_hex_key(data, hexkey, &keybuf, &keylen);
  𐡄𐡍(result)
    اذهب fail;

  𐡄𐡍(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  𐡄𐡍(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  𐡄𐡍(result)
    اذهب fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  𐡄𐡍(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  وإلا
    result = curlx_dyn_add(&query_dyn, "?");
  𐡄𐡍(result)
    اذهب fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  𐡄𐡍(result)
    اذهب fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  𐡄𐡍(result)
    اذهب fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  𐡄𐡍(result)
    اذهب fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  𐡄𐡍(result)
    اذهب fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  𐡄𐡍(result)
    اذهب fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  𐡄𐡍(result)
    اذهب fail;

  result = curlx_dyn_addf(&input_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  𐡄𐡍(result)
    اذهب fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&input_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  𐡄𐡍(!auth_headers)
    اذهب fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&input_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  𐡄𐡍(keybuf) {
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
  أرجع result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
