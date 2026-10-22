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

#אם !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

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
    החזר "ed25519";
  حالة HTTPSIG_ALG_HMAC_SHA256:
    החזר "hmac-sha256";
  افتراضي:
    اكسر;
  }
  החזר عدم;
}

ساكن تعداد httpsig_alg id_to_alg(uint8_t val)
{
  بدل(val) {
  حالة CURLHTTPSIG_ED25519:
    החזר HTTPSIG_ALG_ED25519;
  حالة CURLHTTPSIG_HMAC_SHA256:
    החזר HTTPSIG_ALG_HMAC_SHA256;
  افتراضي:
    اكسر;
  }
  החזר HTTPSIG_ALG_UNKNOWN;
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
  כלעוד(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  אם(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }

  אם(len > CURL_MAX_INPUT_LENGTH) {
    failf(data, "httpsig: hex key too long");
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  אם(!keybuf)
    החזר CURLE_OUT_OF_MEMORY;

  עבור(i = 0; i < len; i += 2) {
    אם(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned حرف)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  החזר CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ساكن CURLcode httpsig_authority(بنية Curl_easy *data,
                                  بنية connectdata *conn,
                                  بنية dynbuf *authority_buf)
{
  ثابت حرف *h = data->state.http_host;

  אם(h && curl_strnequal(h, "host:", 5)) {
    ثابت حرف *value = h + 5;
    ثابت حرف *end;

    כלעוד(ISBLANK(*value))
      value++;
    אם(*value) {
      CURLcode result;

      end = value;
      כלעוד(*end && !ISNEWLINE(*end))
        end++;
      כלעוד(end > value && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, value, (size_t)(end - value));
      אם(result)
        החזר result;
      החזר CURLE_OK;
    }
  }

  {
    ثابت حرف *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    אם((conn->origin->scheme->defport != port) && port)
      החזר curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    החזר curlx_dyn_add(authority_buf, hostname);
  }
}

ساكن CURLcode sf_append_quoted(بنية dynbuf *buf, ثابت حرف *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  אם(result)
    החזר result;
  כלעוד(*str) {
    אם(ISCNTRL(*str))
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    אם(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      אם(result)
        החזר result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    אם(result)
      החזר result;
    str++;
  }
  החזר curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ساكن CURLcode sf_encode_byte_seq(ثابت unsigned حرف *raw, size_t rawlen,
                                   بنية dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  حرف *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  אם(result)
    החזר result;

  result = curlx_dyn_addn(out, ":", 1);
  אם(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  אם(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  החזר result;
}

ساكن CURLcode build_sig_params(بنية dynbuf *params,
                                 ثابت حرف **components, size_t count,
                                 time_t created, ثابت حرف *keyid,
                                 تعداد httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  אם(result)
    החזר result;

  עבור(i = 0; i < count; i++) {
    אם(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      אם(result)
        החזר result;
    }
    result = sf_append_quoted(params, components[i]);
    אם(result)
      החזר result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  אם(result)
    החזר result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  אם(result)
    החזר result;

  אם(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    אם(result)
      החזר result;
    result = sf_append_quoted(params, keyid);
    אם(result)
      החזר result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  החזר result;
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

  אם(name[0] == '@') {
    אם(curl_strequal(name, SIG_METHOD))
      *out = method;
    אחרת אם(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    אחרת אם(curl_strequal(name, SIG_PATH))
      *out = path;
    אחרת אם(curl_strequal(name, SIG_QUERY))
      *out = query;
    אחרת {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    }
    אם(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    }
    החזר CURLE_OK;
  }
  אחרת {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    بنية curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    עבור(head = data->set.headers; head; head = head->next) {
      אם(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        ثابت حرف *p = strchr(head->data, ':');
        אם(p) {
          CURLcode result;
          بنية Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          אם(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            אם(result)
              החזר result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          אם(result)
            החזר result;
          found = TRUE;
        }
      }
    }

    אם(found) {
      *out = curlx_dyn_ptr(valbuf);
      החזר CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
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

  עבור(i = 0; i < count; i++) {
    ثابت حرف *val = عدم;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    אם(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      החזר result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    אם(result) {
      curlx_dyn_free(&hdrvalbuf);
      החזר result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  החזר result;
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
  אם(hdrs && *hdrs) {
    حرف *p;
    حرف *hdrs_copy = curlx_strdup(hdrs);
    אם(!hdrs_copy)
      החזר CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    כלעוד(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      حرف *start;
      size_t tlen = 0;
      ثابت حرف *p2 = p;

      curlx_str_passblanks(&p2);
      אם(!*p2)
        اكسر;
      p = start = CURL_UNCONST(p2);
      כלעוד(*p && !ISBLANK(*p)) {
        אם((*p == '\"') || (*p == '\\'))
          החזר CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      אם(*p)
        *p++ = '\0';

      אם(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        אם(!tlen) {
          failf(data, "httpsig: empty header component name");
          החזר CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      אחרת {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        אם(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        אחרת אם(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        אחרת אם(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        אחרת אם(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        אחרת {
          failf(data, "httpsig: unknown component '%s'", start);
          החזר CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    אם(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    }
    אם(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned صحيح)HTTPSIG_MAX_COMPONENTS);
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      עבור(i = 0; i < ncomp; i++) {
        עבור(j = i + 1; j < ncomp; j++) {
          אם(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            החזר CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  אחרת {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    אם(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  החזר CURLE_OK;
}

ساكن time_t httpsig_get_created(فراغ)
{
#ifdef DEBUGBUILD
  حرف *force = getenv("CURL_FORCETIME");
  אם(force && *force) {
    حرف *sigts = getenv("CURL_HTTPSIG_CREATED");
    אם(sigts && *sigts) {
      ثابت حرف *p = sigts;
      curl_off_t num;
      אם(!curlx_str_number(&p, &num, CURL_OFF_T_MAX))
        החזר (time_t)num;
    }
    החזר 0;
  }
#endif
  החזר time(عدم);
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
    אם(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    اكسر;
  افتراضي:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اكسر;
  }

  אם(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  החזר result;
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
  אם(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }

  אם(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }
  אם(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&input_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  אם(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    اذهب done;
  }

  result = decode_hex_key(data, hexkey, &keybuf, &keylen);
  אם(result)
    اذهب fail;

  אם(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  אם(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  אם(result)
    اذهب fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  אם(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  אחרת
    result = curlx_dyn_add(&query_dyn, "?");
  אם(result)
    اذهب fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  אם(result)
    اذهب fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  אם(result)
    اذهب fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  אם(result)
    اذهب fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  אם(result)
    اذهب fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  אם(result)
    اذهب fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  אם(result)
    اذهب fail;

  result = curlx_dyn_addf(&input_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  אם(result)
    اذهب fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&input_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  אם(!auth_headers)
    اذهب fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&input_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  אם(keybuf) {
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
  החזר result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
