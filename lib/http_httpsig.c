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

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "http_httpsig.h"
#অন্তর্ভাব "curl_ed25519.h"
#অন্তর্ভাব "curl_hmac.h"
#অন্তর্ভাব "curl_sha256.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "slist.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "strcase.h"

#অন্তর্ভাব <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

গণনা httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

স্থির ধ্রুবক অক্ষর *alg_to_str(গণনা httpsig_alg alg)
{
  নির্বাচন(alg) {
  ক্ষেত্র HTTPSIG_ALG_ED25519:
    ফেরত "ed25519";
  ক্ষেত্র HTTPSIG_ALG_HMAC_SHA256:
    ফেরত "hmac-sha256";
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত ফাঁকা;
}

স্থির গণনা httpsig_alg id_to_alg(uint8_t val)
{
  নির্বাচন(val) {
  ক্ষেত্র CURLHTTPSIG_ED25519:
    ফেরত HTTPSIG_ALG_ED25519;
  ক্ষেত্র CURLHTTPSIG_HMAC_SHA256:
    ফেরত HTTPSIG_ALG_HMAC_SHA256;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত HTTPSIG_ALG_UNKNOWN;
}

স্থির CURLcode decode_hex_চাবি(কাঠামো Curl_easy *তথ্য,
                               ধ্রুবক অক্ষর *hexstr,
                               unsigned অক্ষর **keyout,
                               size_t *keylen)
{
  size_t len, i;
  unsigned অক্ষর *keybuf;

  *keyout = ফাঁকা;
  *keylen = 0;

  len = strlen(hexstr);
  যখন(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  যদি(len == 0 || (len & 1) != 0) {
    failf(তথ্য, "httpsig: invalid hex key (length %zu)", len);
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  যদি(len > CURL_MAX_প্রবেশ_দৈর্ঘ্য) {
    failf(তথ্য, "httpsig: hex key too long");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  যদি(!keybuf)
    ফেরত CURLE_OUT_OF_MEMORY;

  জন্য(i = 0; i < len; i += 2) {
    যদি(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(তথ্য, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned অক্ষর)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  ফেরত CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
স্থির CURLcode httpsig_authority(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *conn,
                                  কাঠামো dynbuf *authority_buf)
{
  ধ্রুবক অক্ষর *h = তথ্য->state.http_host;

  যদি(h && curl_strnequal(h, "host:", 5)) {
    ধ্রুবক অক্ষর *মান = h + 5;
    ধ্রুবক অক্ষর *end;

    যখন(ISBLANK(*মান))
      মান++;
    যদি(*মান) {
      CURLcode ফলাফল;

      end = মান;
      যখন(*end && !ISNEWLINE(*end))
        end++;
      যখন(end > মান && ISBLANK(end[-1]))
        end--;
      ফলাফল = curlx_dyn_addn(authority_buf, মান, (size_t)(end - মান));
      যদি(ফলাফল)
        ফেরত ফলাফল;
      ফেরত CURLE_OK;
    }
  }

  {
    ধ্রুবক অক্ষর *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    যদি((conn->origin->scheme->defport != port) && port)
      ফেরত curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    ফেরত curlx_dyn_add(authority_buf, hostname);
  }
}

স্থির CURLcode sf_append_quoted(কাঠামো dynbuf *buf, ধ্রুবক অক্ষর *str)
{
  CURLcode ফলাফল = curlx_dyn_addn(buf, "\"", 1);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  যখন(*str) {
    যদি(ISCNTRL(*str))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(*str == '\\' || *str == '"') {
      ফলাফল = curlx_dyn_addn(buf, "\\", 1);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    ফলাফল = curlx_dyn_addn(buf, str, 1);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    str++;
  }
  ফেরত curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
স্থির CURLcode sf_encode_byte_seq(ধ্রুবক unsigned অক্ষর *raw, size_t rawlen,
                                   কাঠামো dynbuf *out)
{
  CURLcode ফলাফল;
  size_t b64len;
  অক্ষর *b64;

  ফলাফল = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = curlx_dyn_addn(out, ":", 1);
  যদি(!ফলাফল)
    ফলাফল = curlx_dyn_addn(out, b64, b64len);
  যদি(!ফলাফল)
    ফলাফল = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  ফেরত ফলাফল;
}

স্থির CURLcode build_sig_params(কাঠামো dynbuf *params,
                                 ধ্রুবক অক্ষর **components, size_t গুনতি,
                                 time_t created, ধ্রুবক অক্ষর *keyid,
                                 গণনা httpsig_alg alg)
{
  CURLcode ফলাফল;
  size_t i;

  ফলাফল = curlx_dyn_addn(params, "(", 1);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  জন্য(i = 0; i < গুনতি; i++) {
    যদি(i > 0) {
      ফলাফল = curlx_dyn_addn(params, " ", 1);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    ফলাফল = sf_append_quoted(params, components[i]);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  ফলাফল = curlx_dyn_addn(params, ")", 1);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(keyid && *keyid) {
    ফলাফল = curlx_dyn_add(params, ";keyid=");
    যদি(ফলাফল)
      ফেরত ফলাফল;
    ফলাফল = sf_append_quoted(params, keyid);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  ফলাফল = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  ফেরত ফলাফল;
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
স্থির CURLcode resolve_component(ধ্রুবক অক্ষর *name,
                                  ধ্রুবক অক্ষর *method,
                                  ধ্রুবক অক্ষর *authority,
                                  ধ্রুবক অক্ষর *path,
                                  ধ্রুবক অক্ষর *query,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো dynbuf *valbuf,
                                  ধ্রুবক অক্ষর **out)
{
  *out = ফাঁকা;

  যদি(name[0] == '@') {
    যদি(curl_strequal(name, SIG_METHOD))
      *out = method;
    অন্যথা যদি(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    অন্যথা যদি(curl_strequal(name, SIG_PATH))
      *out = path;
    অন্যথা যদি(curl_strequal(name, SIG_QUERY))
      *out = query;
    অন্যথা {
      failf(তথ্য, "httpsig: unsupported derived component '%s'", name);
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }
    যদি(!*out) {
      failf(তথ্য, "httpsig: derived component '%s' has no value", name);
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ফেরত CURLE_OK;
  }
  অন্যথা {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    কাঠামো curl_slist *শীর্ষ;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    জন্য(শীর্ষ = তথ্য->set.headers; শীর্ষ; শীর্ষ = শীর্ষ->পরবর্তী) {
      যদি(curl_strnequal(শীর্ষ->তথ্য, name, namelen) &&
         Curl_headersep(শীর্ষ->তথ্য[namelen])) {
        ধ্রুবক অক্ষর *p = strchr(শীর্ষ->তথ্য, ':');
        যদি(p) {
          CURLcode ফলাফল;
          কাঠামো Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          যদি(found) {
            ফলাফল = curlx_dyn_addn(valbuf, ", ", 2);
            যদি(ফলাফল)
              ফেরত ফলাফল;
          }
          ফলাফল = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          যদি(ফলাফল)
            ফেরত ফলাফল;
          found = TRUE;
        }
      }
    }

    যদি(found) {
      *out = curlx_dyn_ptr(valbuf);
      ফেরত CURLE_OK;
    }
    failf(তথ্য, "httpsig: header '%s' not found in request", name);
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

স্থির CURLcode build_sig_base(কাঠামো dynbuf *base,
                               ধ্রুবক অক্ষর **components, size_t গুনতি,
                               ধ্রুবক অক্ষর *method,
                               ধ্রুবক অক্ষর *authority,
                               ধ্রুবক অক্ষর *path,
                               ধ্রুবক অক্ষর *query,
                               কাঠামো Curl_easy *তথ্য,
                               ধ্রুবক অক্ষর *sig_params)
{
  CURLcode ফলাফল;
  size_t i;
  কাঠামো dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  জন্য(i = 0; i < গুনতি; i++) {
    ধ্রুবক অক্ষর *val = ফাঁকা;
    ফলাফল = resolve_component(components[i], method,
                               authority, path, query, তথ্য,
                               &hdrvalbuf, &val);
    যদি(ফলাফল || !val) {
      failf(তথ্য, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      ফেরত ফলাফল ? ফলাফল : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ফলাফল = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    যদি(ফলাফল) {
      curlx_dyn_free(&hdrvalbuf);
      ফেরত ফলাফল;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  ফলাফল = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  ফেরত ফলাফল;
}

স্থির CURLcode parse_components(কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক অক্ষর *query,
                                 ধ্রুবক অক্ষর **components,
                                 size_t *ncomp_out,
                                 অক্ষর **hdrs_copy_out)
{
  ধ্রুবক অক্ষর *hdrs = CURL_EASY_STR(তথ্য, STRING_HTTPSIG_HEADERS);
  size_t ncomp = 0;

  *hdrs_copy_out = ফাঁকা;
  যদি(hdrs && *hdrs) {
    অক্ষর *p;
    অক্ষর *hdrs_copy = curlx_strdup(hdrs);
    যদি(!hdrs_copy)
      ফেরত CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    যখন(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      অক্ষর *start;
      size_t tlen = 0;
      ধ্রুবক অক্ষর *p2 = p;

      curlx_str_passblanks(&p2);
      যদি(!*p2)
        বিরতি;
      p = start = CURL_UNCONST(p2);
      যখন(*p && !ISBLANK(*p)) {
        যদি((*p == '\"') || (*p == '\\'))
          ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      যদি(*p)
        *p++ = '\0';

      যদি(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        যদি(!tlen) {
          failf(তথ্য, "httpsig: empty header component name");
          ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      অন্যথা {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        যদি(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        অন্যথা যদি(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        অন্যথা যদি(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        অন্যথা যদি(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        অন্যথা {
          failf(তথ্য, "httpsig: unknown component '%s'", start);
          ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    যদি(!ncomp) {
      failf(তথ্য, "httpsig: no signature components specified");
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }
    যদি(*p) {
      failf(তথ্য, "httpsig: too many signature components (max %u)",
            (unsigned পূর্ণসংখ্যা)HTTPSIG_MAX_COMPONENTS);
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      জন্য(i = 0; i < ncomp; i++) {
        জন্য(j = i + 1; j < ncomp; j++) {
          যদি(!strcmp(components[i], components[j])) {
            failf(তথ্য, "httpsig: duplicate signature component '%s'",
                  components[i]);
            ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  অন্যথা {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    যদি(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  ফেরত CURLE_OK;
}

স্থির time_t httpsig_get_created(শূন্য)
{
#ifdef DEBUGBUILD
  অক্ষর *force = getenv("CURL_FORCETIME");
  যদি(force && *force) {
    অক্ষর *sigts = getenv("CURL_HTTPSIG_CREATED");
    যদি(sigts && *sigts) {
      ধ্রুবক অক্ষর *p = sigts;
      curl_off_t অঙ্ক;
      যদি(!curlx_str_সংখ্যা(&p, &অঙ্ক, CURL_OFF_T_MAX))
        ফেরত (time_t)অঙ্ক;
    }
    ফেরত 0;
  }
#endif
  ফেরত time(ফাঁকা);
}

স্থির CURLcode httpsig_sign_base(কাঠামো Curl_easy *তথ্য,
                                  গণনা httpsig_alg alg,
                                  ধ্রুবক unsigned অক্ষর *keybuf,
                                  size_t keylen,
                                  ধ্রুবক কাঠামো dynbuf *sig_base,
                                  unsigned অক্ষর *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode ফলাফল;

  নির্বাচন(alg) {
  ক্ষেত্র HTTPSIG_ALG_ED25519:
    ফলাফল = Curl_ed25519_sign(
      keybuf, keylen,
      (ধ্রুবক unsigned অক্ষর *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    বিরতি;
  ক্ষেত্র HTTPSIG_ALG_HMAC_SHA256:
    ফলাফল = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ধ্রুবক unsigned অক্ষর *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    যদি(!ফলাফল)
      *raw_sig_len = CURL_SHA256_DIGEST_দৈর্ঘ্য;
    বিরতি;
  পূর্বনির্ধারিত:
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    বিরতি;
  }

  যদি(ফলাফল && ফলাফল == CURLE_NOT_BUILT_IN) {
    failf(তথ্য, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  ফেরত ফলাফল;
}

CURLcode Curl_output_httpsig(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  কাঠামো connectdata *conn = তথ্য->conn;
  ধ্রুবক অক্ষর *path;
  ধ্রুবক অক্ষর *query;
  Curl_HttpReq httpreq;
  ধ্রুবক অক্ষর *method = ফাঁকা;
  ধ্রুবক অক্ষর *hexkey = CURL_EASY_STR(তথ্য, STRING_HTTPSIG_চাবি);
  ধ্রুবক অক্ষর *keyid = CURL_EASY_STR(তথ্য, STRING_HTTPSIG_KEYID);
  গণনা httpsig_alg alg;
  time_t created;
  কাঠামো dynbuf sig_params;
  কাঠামো dynbuf sig_base;
  কাঠামো dynbuf sig_hdr;
  কাঠামো dynbuf প্রবেশ_hdr;
  কাঠামো dynbuf authority_buf;
  ধ্রুবক অক্ষর *authority;
  ধ্রুবক অক্ষর *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  unsigned অক্ষর *keybuf = ফাঁকা;
  size_t keylen = 0;
  unsigned অক্ষর raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  অক্ষর *auth_headers = ফাঁকা;
  অক্ষর *hdrs_copy = ফাঁকা;
  কাঠামো dynbuf query_dyn;

  alg = id_to_alg(তথ্য->set.httpsig_algorithm);
  যদি(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(তথ্য, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  যদি(!hexkey || !*hexkey) {
    failf(তথ্য, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
  যদি(!keyid || !*keyid) {
    failf(তথ্য, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&প্রবেশ_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  যদি(Curl_checkheaders(তথ্য, STRCONST("Signature")) ||
     Curl_checkheaders(তথ্য, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    যাও done;
  }

  ফলাফল = decode_hex_চাবি(তথ্য, hexkey, &keybuf, &keylen);
  যদি(ফলাফল)
    যাও fail;

  যদি(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(তথ্য, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও fail;
  }

  Curl_http_method(তথ্য, &method, &httpreq);

  path = তথ্য->state.up.path;
  যদি(!path || !*path)
    path = "/";

  query = তথ্য->state.up.query;

  ফলাফল = httpsig_authority(তথ্য, conn, &authority_buf);
  যদি(ফলাফল)
    যাও fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  যদি(query && *query)
    ফলাফল = curlx_dyn_addf(&query_dyn, "?%s", query);
  অন্যথা
    ফলাফল = curlx_dyn_add(&query_dyn, "?");
  যদি(ফলাফল)
    যাও fail;

  ফলাফল = parse_components(তথ্য, query, components, &ncomp, &hdrs_copy);
  যদি(ফলাফল)
    যাও fail;

  created = httpsig_get_created();

  ফলাফল = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  যদি(ফলাফল)
    যাও fail;

  infof(তথ্য, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  ফলাফল = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          তথ্য, curlx_dyn_ptr(&sig_params));
  যদি(ফলাফল)
    যাও fail;

  infof(তথ্য, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  ফলাফল = httpsig_sign_base(তথ্য, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  যদি(ফলাফল)
    যাও fail;

  ফলাফল = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  যদি(ফলাফল)
    যাও fail;
  ফলাফল = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  যদি(ফলাফল)
    যাও fail;

  ফলাফল = curlx_dyn_addf(&প্রবেশ_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  যদি(ফলাফল)
    যাও fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&প্রবেশ_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  যদি(!auth_headers)
    যাও fail;

  infof(তথ্য, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&প্রবেশ_hdr));
  infof(তথ্য, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(তথ্য->req.hd_auth);
  তথ্য->req.hd_auth = auth_headers;
  তথ্য->state.authhost.done = TRUE;
  ফলাফল = CURLE_OK;

fail:
  যদি(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, আকার(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&প্রবেশ_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
