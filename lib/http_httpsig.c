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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#شامل "urldata.h"
#شامل "http_httpsig.h"
#شامل "curl_ed25519.h"
#شامل "curl_hmac.h"
#شامل "curl_sha256.h"
#شامل "http.h"
#شامل "transfer.h"
#شامل "curl_trc.h"
#شامل "slist.h"
#شامل "curlx/dynbuf.h"
#شامل "curlx/base64.h"
#شامل "curlx/strdup.h"
#شامل "curlx/strparse.h"
#شامل "strcase.h"

#شامل <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

شمارش httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

ایستا ثابت نویسه *alg_to_str(شمارش httpsig_alg alg)
{
  انتخاب(alg) {
  حالت HTTPSIG_ALG_ED25519:
    بازگشت "ed25519";
  حالت HTTPSIG_ALG_HMAC_SHA256:
    بازگشت "hmac-sha256";
  پیشفرض:
    بشکن;
  }
  بازگشت پوچ;
}

ایستا شمارش httpsig_alg id_to_alg(uint8_t val)
{
  انتخاب(val) {
  حالت CURLHTTPSIG_ED25519:
    بازگشت HTTPSIG_ALG_ED25519;
  حالت CURLHTTPSIG_HMAC_SHA256:
    بازگشت HTTPSIG_ALG_HMAC_SHA256;
  پیشفرض:
    بشکن;
  }
  بازگشت HTTPSIG_ALG_UNKNOWN;
}

ایستا CURLcode decode_hex_کلید(ساختار Curl_easy *data,
                               ثابت نویسه *hexstr,
                               unsigned نویسه **keyout,
                               size_t *keylen)
{
  size_t len, i;
  unsigned نویسه *keybuf;

  *keyout = پوچ;
  *keylen = 0;

  len = strlen(hexstr);
  وقتی(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  اگر(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }

  اگر(len > CURL_MAX_درون_داد_LENGTH) {
    failf(data, "httpsig: hex key too long");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  اگر(!keybuf)
    بازگشت CURLE_OUT_OF_MEMORY;

  برای(i = 0; i < len; i += 2) {
    اگر(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned نویسه)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  بازگشت CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ایستا CURLcode httpsig_authority(ساختار Curl_easy *data,
                                  ساختار connectdata *conn,
                                  ساختار dynbuf *authority_buf)
{
  ثابت نویسه *h = data->state.http_host;

  اگر(h && curl_strnequal(h, "host:", 5)) {
    ثابت نویسه *مقدار = h + 5;
    ثابت نویسه *end;

    وقتی(ISBLANK(*مقدار))
      مقدار++;
    اگر(*مقدار) {
      CURLcode result;

      end = مقدار;
      وقتی(*end && !ISNEWLINE(*end))
        end++;
      وقتی(end > مقدار && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, مقدار, (size_t)(end - مقدار));
      اگر(result)
        بازگشت result;
      بازگشت CURLE_OK;
    }
  }

  {
    ثابت نویسه *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    اگر((conn->origin->scheme->defport != port) && port)
      بازگشت curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    بازگشت curlx_dyn_add(authority_buf, hostname);
  }
}

ایستا CURLcode sf_append_quoted(ساختار dynbuf *buf, ثابت نویسه *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  اگر(result)
    بازگشت result;
  وقتی(*str) {
    اگر(ISCNTRL(*str))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    اگر(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      اگر(result)
        بازگشت result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    اگر(result)
      بازگشت result;
    str++;
  }
  بازگشت curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ایستا CURLcode sf_encode_byte_seq(ثابت unsigned نویسه *raw, size_t rawlen,
                                   ساختار dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  نویسه *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  اگر(result)
    بازگشت result;

  result = curlx_dyn_addn(out, ":", 1);
  اگر(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  اگر(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  بازگشت result;
}

ایستا CURLcode build_sig_params(ساختار dynbuf *params,
                                 ثابت نویسه **components, size_t شمار,
                                 time_t created, ثابت نویسه *keyid,
                                 شمارش httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  اگر(result)
    بازگشت result;

  برای(i = 0; i < شمار; i++) {
    اگر(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      اگر(result)
        بازگشت result;
    }
    result = sf_append_quoted(params, components[i]);
    اگر(result)
      بازگشت result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  اگر(result)
    بازگشت result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  اگر(result)
    بازگشت result;

  اگر(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    اگر(result)
      بازگشت result;
    result = sf_append_quoted(params, keyid);
    اگر(result)
      بازگشت result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  بازگشت result;
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
ایستا CURLcode resolve_component(ثابت نویسه *name,
                                  ثابت نویسه *method,
                                  ثابت نویسه *authority,
                                  ثابت نویسه *path,
                                  ثابت نویسه *query,
                                  ساختار Curl_easy *data,
                                  ساختار dynbuf *valbuf,
                                  ثابت نویسه **out)
{
  *out = پوچ;

  اگر(name[0] == '@') {
    اگر(curl_strequal(name, SIG_METHOD))
      *out = method;
    وگرنه اگر(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    وگرنه اگر(curl_strequal(name, SIG_PATH))
      *out = path;
    وگرنه اگر(curl_strequal(name, SIG_QUERY))
      *out = query;
    وگرنه {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }
    اگر(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }
    بازگشت CURLE_OK;
  }
  وگرنه {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    ساختار curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    برای(head = data->set.headers; head; head = head->next) {
      اگر(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        ثابت نویسه *p = strchr(head->data, ':');
        اگر(p) {
          CURLcode result;
          ساختار Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          اگر(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            اگر(result)
              بازگشت result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          اگر(result)
            بازگشت result;
          found = TRUE;
        }
      }
    }

    اگر(found) {
      *out = curlx_dyn_ptr(valbuf);
      بازگشت CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

ایستا CURLcode build_sig_base(ساختار dynbuf *base,
                               ثابت نویسه **components, size_t شمار,
                               ثابت نویسه *method,
                               ثابت نویسه *authority,
                               ثابت نویسه *path,
                               ثابت نویسه *query,
                               ساختار Curl_easy *data,
                               ثابت نویسه *sig_params)
{
  CURLcode result;
  size_t i;
  ساختار dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  برای(i = 0; i < شمار; i++) {
    ثابت نویسه *val = پوچ;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    اگر(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      بازگشت result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    اگر(result) {
      curlx_dyn_free(&hdrvalbuf);
      بازگشت result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  بازگشت result;
}

ایستا CURLcode parse_components(ساختار Curl_easy *data,
                                 ثابت نویسه *query,
                                 ثابت نویسه **components,
                                 size_t *ncomp_out,
                                 نویسه **hdrs_copy_out)
{
  ثابت نویسه *hdrs = CURL_EASY_STR(data, STRING_HTTPSIG_HEADERS);
  size_t ncomp = 0;

  *hdrs_copy_out = پوچ;
  اگر(hdrs && *hdrs) {
    نویسه *p;
    نویسه *hdrs_copy = curlx_strdup(hdrs);
    اگر(!hdrs_copy)
      بازگشت CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    وقتی(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      نویسه *start;
      size_t tlen = 0;
      ثابت نویسه *p2 = p;

      curlx_str_passblanks(&p2);
      اگر(!*p2)
        بشکن;
      p = start = CURL_UNCONST(p2);
      وقتی(*p && !ISBLANK(*p)) {
        اگر((*p == '\"') || (*p == '\\'))
          بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      اگر(*p)
        *p++ = '\0';

      اگر(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        اگر(!tlen) {
          failf(data, "httpsig: empty header component name");
          بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      وگرنه {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        اگر(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        وگرنه اگر(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        وگرنه اگر(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        وگرنه اگر(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        وگرنه {
          failf(data, "httpsig: unknown component '%s'", start);
          بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    اگر(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }
    اگر(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned صحیح)HTTPSIG_MAX_COMPONENTS);
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      برای(i = 0; i < ncomp; i++) {
        برای(j = i + 1; j < ncomp; j++) {
          اگر(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  وگرنه {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    اگر(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  بازگشت CURLE_OK;
}

ایستا time_t httpsig_get_created(تهی)
{
#ifdef DEBUGBUILD
  نویسه *force = getenv("CURL_FORCETIME");
  اگر(force && *force) {
    نویسه *sigts = getenv("CURL_HTTPSIG_CREATED");
    اگر(sigts && *sigts) {
      ثابت نویسه *p = sigts;
      curl_off_t رقم;
      اگر(!curlx_str_عدد(&p, &رقم, CURL_OFF_T_MAX))
        بازگشت (time_t)رقم;
    }
    بازگشت 0;
  }
#endif
  بازگشت time(پوچ);
}

ایستا CURLcode httpsig_sign_base(ساختار Curl_easy *data,
                                  شمارش httpsig_alg alg,
                                  ثابت unsigned نویسه *keybuf,
                                  size_t keylen,
                                  ثابت ساختار dynbuf *sig_base,
                                  unsigned نویسه *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode result;

  انتخاب(alg) {
  حالت HTTPSIG_ALG_ED25519:
    result = Curl_ed25519_sign(
      keybuf, keylen,
      (ثابت unsigned نویسه *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    بشکن;
  حالت HTTPSIG_ALG_HMAC_SHA256:
    result = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ثابت unsigned نویسه *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    اگر(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    بشکن;
  پیشفرض:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    بشکن;
  }

  اگر(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  بازگشت result;
}

CURLcode Curl_output_httpsig(ساختار Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ساختار connectdata *conn = data->conn;
  ثابت نویسه *path;
  ثابت نویسه *query;
  Curl_HttpReq httpreq;
  ثابت نویسه *method = پوچ;
  ثابت نویسه *hexkey = CURL_EASY_STR(data, STRING_HTTPSIG_کلید);
  ثابت نویسه *keyid = CURL_EASY_STR(data, STRING_HTTPSIG_KEYID);
  شمارش httpsig_alg alg;
  time_t created;
  ساختار dynbuf sig_params;
  ساختار dynbuf sig_base;
  ساختار dynbuf sig_hdr;
  ساختار dynbuf درون_داد_hdr;
  ساختار dynbuf authority_buf;
  ثابت نویسه *authority;
  ثابت نویسه *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  unsigned نویسه *keybuf = پوچ;
  size_t keylen = 0;
  unsigned نویسه raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  نویسه *auth_headers = پوچ;
  نویسه *hdrs_copy = پوچ;
  ساختار dynbuf query_dyn;

  alg = id_to_alg(data->set.httpsig_algorithm);
  اگر(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }

  اگر(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }
  اگر(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&درون_داد_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  اگر(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    برو done;
  }

  result = decode_hex_کلید(data, hexkey, &keybuf, &keylen);
  اگر(result)
    برو fail;

  اگر(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  اگر(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  اگر(result)
    برو fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  اگر(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  وگرنه
    result = curlx_dyn_add(&query_dyn, "?");
  اگر(result)
    برو fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  اگر(result)
    برو fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  اگر(result)
    برو fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  اگر(result)
    برو fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  اگر(result)
    برو fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  اگر(result)
    برو fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  اگر(result)
    برو fail;

  result = curlx_dyn_addf(&درون_داد_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  اگر(result)
    برو fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&درون_داد_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  اگر(!auth_headers)
    برو fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&درون_داد_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  اگر(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, اندازه(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&درون_داد_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  بازگشت result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
