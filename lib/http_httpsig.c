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

#जइ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "http_httpsig.h"
#अन्तर्भाव "curl_ed25519.h"
#अन्तर्भाव "curl_hmac.h"
#अन्तर्भाव "curl_sha256.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "slist.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "strcase.h"

#अन्तर्भाव <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

गणना httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

स्थिर ध्रुव वर्ण *alg_to_str(गणना httpsig_alg alg)
{
  विकल्प(alg) {
  पक्ष HTTPSIG_ALG_ED25519:
    प्रत्यागम "ed25519";
  पक्ष HTTPSIG_ALG_HMAC_SHA256:
    प्रत्यागम "hmac-sha256";
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम रिक्त;
}

स्थिर गणना httpsig_alg id_to_alg(uint8_t val)
{
  विकल्प(val) {
  पक्ष CURLHTTPSIG_ED25519:
    प्रत्यागम HTTPSIG_ALG_ED25519;
  पक्ष CURLHTTPSIG_HMAC_SHA256:
    प्रत्यागम HTTPSIG_ALG_HMAC_SHA256;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम HTTPSIG_ALG_UNKNOWN;
}

स्थिर CURLcode decode_hex_कुञ्चिका(संरचना Curl_easy *data,
                               ध्रुव वर्ण *hexstr,
                               unsigned वर्ण **keyout,
                               size_t *keylen)
{
  size_t len, i;
  unsigned वर्ण *keybuf;

  *keyout = रिक्त;
  *keylen = 0;

  len = strlen(hexstr);
  जाव(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  जइ(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }

  जइ(len > CURL_MAX_निवेशः_LENGTH) {
    failf(data, "httpsig: hex key too long");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  जइ(!keybuf)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  प्रति(i = 0; i < len; i += 2) {
    जइ(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned वर्ण)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  प्रत्यागम CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
स्थिर CURLcode httpsig_authority(संरचना Curl_easy *data,
                                  संरचना connectdata *conn,
                                  संरचना dynbuf *authority_buf)
{
  ध्रुव वर्ण *h = data->state.http_host;

  जइ(h && curl_strnequal(h, "host:", 5)) {
    ध्रुव वर्ण *मानम् = h + 5;
    ध्रुव वर्ण *end;

    जाव(ISBLANK(*मानम्))
      मानम्++;
    जइ(*मानम्) {
      CURLcode result;

      end = मानम्;
      जाव(*end && !ISNEWLINE(*end))
        end++;
      जाव(end > मानम् && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, मानम्, (size_t)(end - मानम्));
      जइ(result)
        प्रत्यागम result;
      प्रत्यागम CURLE_OK;
    }
  }

  {
    ध्रुव वर्ण *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    जइ((conn->origin->scheme->defport != port) && port)
      प्रत्यागम curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    प्रत्यागम curlx_dyn_add(authority_buf, hostname);
  }
}

स्थिर CURLcode sf_append_quoted(संरचना dynbuf *buf, ध्रुव वर्ण *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  जइ(result)
    प्रत्यागम result;
  जाव(*str) {
    जइ(ISCNTRL(*str))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    जइ(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      जइ(result)
        प्रत्यागम result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    जइ(result)
      प्रत्यागम result;
    str++;
  }
  प्रत्यागम curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
स्थिर CURLcode sf_encode_byte_seq(ध्रुव unsigned वर्ण *raw, size_t rawlen,
                                   संरचना dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  वर्ण *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  जइ(result)
    प्रत्यागम result;

  result = curlx_dyn_addn(out, ":", 1);
  जइ(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  जइ(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  प्रत्यागम result;
}

स्थिर CURLcode build_sig_params(संरचना dynbuf *params,
                                 ध्रुव वर्ण **components, size_t गणनम्,
                                 time_t created, ध्रुव वर्ण *keyid,
                                 गणना httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  जइ(result)
    प्रत्यागम result;

  प्रति(i = 0; i < गणनम्; i++) {
    जइ(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      जइ(result)
        प्रत्यागम result;
    }
    result = sf_append_quoted(params, components[i]);
    जइ(result)
      प्रत्यागम result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  जइ(result)
    प्रत्यागम result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  जइ(result)
    प्रत्यागम result;

  जइ(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    जइ(result)
      प्रत्यागम result;
    result = sf_append_quoted(params, keyid);
    जइ(result)
      प्रत्यागम result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  प्रत्यागम result;
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
स्थिर CURLcode resolve_component(ध्रुव वर्ण *name,
                                  ध्रुव वर्ण *method,
                                  ध्रुव वर्ण *authority,
                                  ध्रुव वर्ण *path,
                                  ध्रुव वर्ण *query,
                                  संरचना Curl_easy *data,
                                  संरचना dynbuf *valbuf,
                                  ध्रुव वर्ण **out)
{
  *out = रिक्त;

  जइ(name[0] == '@') {
    जइ(curl_strequal(name, SIG_METHOD))
      *out = method;
    अण्णहा जइ(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    अण्णहा जइ(curl_strequal(name, SIG_PATH))
      *out = path;
    अण्णहा जइ(curl_strequal(name, SIG_QUERY))
      *out = query;
    अण्णहा {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    }
    जइ(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    }
    प्रत्यागम CURLE_OK;
  }
  अण्णहा {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    संरचना curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    प्रति(head = data->set.headers; head; head = head->next) {
      जइ(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        ध्रुव वर्ण *p = strchr(head->data, ':');
        जइ(p) {
          CURLcode result;
          संरचना Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          जइ(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            जइ(result)
              प्रत्यागम result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          जइ(result)
            प्रत्यागम result;
          found = TRUE;
        }
      }
    }

    जइ(found) {
      *out = curlx_dyn_ptr(valbuf);
      प्रत्यागम CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

स्थिर CURLcode build_sig_base(संरचना dynbuf *base,
                               ध्रुव वर्ण **components, size_t गणनम्,
                               ध्रुव वर्ण *method,
                               ध्रुव वर्ण *authority,
                               ध्रुव वर्ण *path,
                               ध्रुव वर्ण *query,
                               संरचना Curl_easy *data,
                               ध्रुव वर्ण *sig_params)
{
  CURLcode result;
  size_t i;
  संरचना dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  प्रति(i = 0; i < गणनम्; i++) {
    ध्रुव वर्ण *val = रिक्त;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    जइ(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      प्रत्यागम result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    जइ(result) {
      curlx_dyn_free(&hdrvalbuf);
      प्रत्यागम result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  प्रत्यागम result;
}

स्थिर CURLcode parse_components(संरचना Curl_easy *data,
                                 ध्रुव वर्ण *query,
                                 ध्रुव वर्ण **components,
                                 size_t *ncomp_out,
                                 वर्ण **hdrs_copy_out)
{
  ध्रुव वर्ण *hdrs = CURL_EASY_STR(data, STRING_HTTPSIG_HEADERS);
  size_t ncomp = 0;

  *hdrs_copy_out = रिक्त;
  जइ(hdrs && *hdrs) {
    वर्ण *p;
    वर्ण *hdrs_copy = curlx_strdup(hdrs);
    जइ(!hdrs_copy)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    जाव(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      वर्ण *start;
      size_t tlen = 0;
      ध्रुव वर्ण *p2 = p;

      curlx_str_passblanks(&p2);
      जइ(!*p2)
        भङ्ग;
      p = start = CURL_UNCONST(p2);
      जाव(*p && !ISBLANK(*p)) {
        जइ((*p == '\"') || (*p == '\\'))
          प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      जइ(*p)
        *p++ = '\0';

      जइ(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        जइ(!tlen) {
          failf(data, "httpsig: empty header component name");
          प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      अण्णहा {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        जइ(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        अण्णहा जइ(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        अण्णहा जइ(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        अण्णहा जइ(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        अण्णहा {
          failf(data, "httpsig: unknown component '%s'", start);
          प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    जइ(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    }
    जइ(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned पूर्णाङ्क)HTTPSIG_MAX_COMPONENTS);
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      प्रति(i = 0; i < ncomp; i++) {
        प्रति(j = i + 1; j < ncomp; j++) {
          जइ(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  अण्णहा {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    जइ(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  प्रत्यागम CURLE_OK;
}

स्थिर time_t httpsig_get_created(शून्य)
{
#ifdef DEBUGBUILD
  वर्ण *force = getenv("CURL_FORCETIME");
  जइ(force && *force) {
    वर्ण *sigts = getenv("CURL_HTTPSIG_CREATED");
    जइ(sigts && *sigts) {
      ध्रुव वर्ण *p = sigts;
      curl_off_t अङ्कः;
      जइ(!curlx_str_सङ्ख्या(&p, &अङ्कः, CURL_OFF_T_MAX))
        प्रत्यागम (time_t)अङ्कः;
    }
    प्रत्यागम 0;
  }
#endif
  प्रत्यागम time(रिक्त);
}

स्थिर CURLcode httpsig_sign_base(संरचना Curl_easy *data,
                                  गणना httpsig_alg alg,
                                  ध्रुव unsigned वर्ण *keybuf,
                                  size_t keylen,
                                  ध्रुव संरचना dynbuf *sig_base,
                                  unsigned वर्ण *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode result;

  विकल्प(alg) {
  पक्ष HTTPSIG_ALG_ED25519:
    result = Curl_ed25519_sign(
      keybuf, keylen,
      (ध्रुव unsigned वर्ण *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    भङ्ग;
  पक्ष HTTPSIG_ALG_HMAC_SHA256:
    result = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ध्रुव unsigned वर्ण *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    जइ(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    भङ्ग;
  स्वभाव:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    भङ्ग;
  }

  जइ(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  प्रत्यागम result;
}

CURLcode Curl_output_httpsig(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  संरचना connectdata *conn = data->conn;
  ध्रुव वर्ण *path;
  ध्रुव वर्ण *query;
  Curl_HttpReq httpreq;
  ध्रुव वर्ण *method = रिक्त;
  ध्रुव वर्ण *hexkey = CURL_EASY_STR(data, STRING_HTTPSIG_कुञ्चिका);
  ध्रुव वर्ण *keyid = CURL_EASY_STR(data, STRING_HTTPSIG_KEYID);
  गणना httpsig_alg alg;
  time_t created;
  संरचना dynbuf sig_params;
  संरचना dynbuf sig_base;
  संरचना dynbuf sig_hdr;
  संरचना dynbuf निवेशः_hdr;
  संरचना dynbuf authority_buf;
  ध्रुव वर्ण *authority;
  ध्रुव वर्ण *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  unsigned वर्ण *keybuf = रिक्त;
  size_t keylen = 0;
  unsigned वर्ण raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  वर्ण *auth_headers = रिक्त;
  वर्ण *hdrs_copy = रिक्त;
  संरचना dynbuf query_dyn;

  alg = id_to_alg(data->set.httpsig_algorithm);
  जइ(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }

  जइ(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }
  जइ(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&निवेशः_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  जइ(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    गच्छ done;
  }

  result = decode_hex_कुञ्चिका(data, hexkey, &keybuf, &keylen);
  जइ(result)
    गच्छ fail;

  जइ(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  जइ(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  जइ(result)
    गच्छ fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  जइ(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  अण्णहा
    result = curlx_dyn_add(&query_dyn, "?");
  जइ(result)
    गच्छ fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  जइ(result)
    गच्छ fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  जइ(result)
    गच्छ fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  जइ(result)
    गच्छ fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  जइ(result)
    गच्छ fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  जइ(result)
    गच्छ fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  जइ(result)
    गच्छ fail;

  result = curlx_dyn_addf(&निवेशः_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  जइ(result)
    गच्छ fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&निवेशः_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  जइ(!auth_headers)
    गच्छ fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&निवेशः_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  जइ(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, परिमाण(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&निवेशः_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  प्रत्यागम result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
