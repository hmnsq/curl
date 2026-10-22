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
#অন্তর্ভাৱ "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "http_httpsig.h"
#অন্তর্ভাৱ "curl_ed25519.h"
#অন্তর্ভাৱ "curl_hmac.h"
#অন্তর্ভাৱ "curl_sha256.h"
#অন্তর্ভাৱ "http.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "slist.h"
#অন্তর্ভাৱ "curlx/dynbuf.h"
#অন্তর্ভাৱ "curlx/base64.h"
#অন্তর্ভাৱ "curlx/strdup.h"
#অন্তর্ভাৱ "curlx/strparse.h"
#অন্তর্ভাৱ "strcase.h"

#অন্তর্ভাৱ <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

গণনা httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

স্থির ধ্রুৱ ৱর্ণ *alg_to_str(গণনা httpsig_alg alg)
{
  ৱিকল্প(alg) {
  পক্ষ HTTPSIG_ALG_ED25519:
    ঘূৰাই "ed25519";
  পক্ষ HTTPSIG_ALG_HMAC_SHA256:
    ঘূৰাই "hmac-sha256";
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই রিক্ত;
}

স্থির গণনা httpsig_alg id_to_alg(uint8_t val)
{
  ৱিকল্প(val) {
  পক্ষ CURLHTTPSIG_ED25519:
    ঘূৰাই HTTPSIG_ALG_ED25519;
  পক্ষ CURLHTTPSIG_HMAC_SHA256:
    ঘূৰাই HTTPSIG_ALG_HMAC_SHA256;
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই HTTPSIG_ALG_UNKNOWN;
}

স্থির CURLcode decode_hex_চাবি(সংরচনা Curl_easy *data,
                               ধ্রুৱ ৱর্ণ *hexstr,
                               unsigned ৱর্ণ **keyout,
                               size_t *keylen)
{
  size_t len, i;
  unsigned ৱর্ণ *keybuf;

  *keyout = রিক্ত;
  *keylen = 0;

  len = strlen(hexstr);
  যেতিয়ালৈ(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  যদি(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }

  যদি(len > CURL_MAX_প্ৰবেশ_LENGTH) {
    failf(data, "httpsig: hex key too long");
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  যদি(!keybuf)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  বাবে(i = 0; i < len; i += 2) {
    যদি(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned ৱর্ণ)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  ঘূৰাই CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
স্থির CURLcode httpsig_authority(সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn,
                                  সংরচনা dynbuf *authority_buf)
{
  ধ্রুৱ ৱর্ণ *h = data->state.http_host;

  যদি(h && curl_strnequal(h, "host:", 5)) {
    ধ্রুৱ ৱর্ণ *মান = h + 5;
    ধ্রুৱ ৱর্ণ *end;

    যেতিয়ালৈ(ISBLANK(*মান))
      মান++;
    যদি(*মান) {
      CURLcode result;

      end = মান;
      যেতিয়ালৈ(*end && !ISNEWLINE(*end))
        end++;
      যেতিয়ালৈ(end > মান && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, মান, (size_t)(end - মান));
      যদি(result)
        ঘূৰাই result;
      ঘূৰাই CURLE_OK;
    }
  }

  {
    ধ্রুৱ ৱর্ণ *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    যদি((conn->origin->scheme->defport != port) && port)
      ঘূৰাই curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    ঘূৰাই curlx_dyn_add(authority_buf, hostname);
  }
}

স্থির CURLcode sf_append_quoted(সংরচনা dynbuf *buf, ধ্রুৱ ৱর্ণ *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  যদি(result)
    ঘূৰাই result;
  যেতিয়ালৈ(*str) {
    যদি(ISCNTRL(*str))
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      যদি(result)
        ঘূৰাই result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    যদি(result)
      ঘূৰাই result;
    str++;
  }
  ঘূৰাই curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
স্থির CURLcode sf_encode_byte_seq(ধ্রুৱ unsigned ৱর্ণ *raw, size_t rawlen,
                                   সংরচনা dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  ৱর্ণ *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  যদি(result)
    ঘূৰাই result;

  result = curlx_dyn_addn(out, ":", 1);
  যদি(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  যদি(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  ঘূৰাই result;
}

স্থির CURLcode build_sig_params(সংরচনা dynbuf *params,
                                 ধ্রুৱ ৱর্ণ **components, size_t গুনতি,
                                 time_t created, ধ্রুৱ ৱর্ণ *keyid,
                                 গণনা httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  যদি(result)
    ঘূৰাই result;

  বাবে(i = 0; i < গুনতি; i++) {
    যদি(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      যদি(result)
        ঘূৰাই result;
    }
    result = sf_append_quoted(params, components[i]);
    যদি(result)
      ঘূৰাই result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  যদি(result)
    ঘূৰাই result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  যদি(result)
    ঘূৰাই result;

  যদি(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    যদি(result)
      ঘূৰাই result;
    result = sf_append_quoted(params, keyid);
    যদি(result)
      ঘূৰাই result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  ঘূৰাই result;
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
স্থির CURLcode resolve_component(ধ্রুৱ ৱর্ণ *name,
                                  ধ্রুৱ ৱর্ণ *method,
                                  ধ্রুৱ ৱর্ণ *authority,
                                  ধ্রুৱ ৱর্ণ *path,
                                  ধ্রুৱ ৱর্ণ *query,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা dynbuf *valbuf,
                                  ধ্রুৱ ৱর্ণ **out)
{
  *out = রিক্ত;

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
      failf(data, "httpsig: unsupported derived component '%s'", name);
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    }
    যদি(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ঘূৰাই CURLE_OK;
  }
  অন্যথা {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    সংরচনা curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    বাবে(head = data->set.headers; head; head = head->next) {
      যদি(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        ধ্রুৱ ৱর্ণ *p = strchr(head->data, ':');
        যদি(p) {
          CURLcode result;
          সংরচনা Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          যদি(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            যদি(result)
              ঘূৰাই result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          যদি(result)
            ঘূৰাই result;
          found = TRUE;
        }
      }
    }

    যদি(found) {
      *out = curlx_dyn_ptr(valbuf);
      ঘূৰাই CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

স্থির CURLcode build_sig_base(সংরচনা dynbuf *base,
                               ধ্রুৱ ৱর্ণ **components, size_t গুনতি,
                               ধ্রুৱ ৱর্ণ *method,
                               ধ্রুৱ ৱর্ণ *authority,
                               ধ্রুৱ ৱর্ণ *path,
                               ধ্রুৱ ৱর্ণ *query,
                               সংরচনা Curl_easy *data,
                               ধ্রুৱ ৱর্ণ *sig_params)
{
  CURLcode result;
  size_t i;
  সংরচনা dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  বাবে(i = 0; i < গুনতি; i++) {
    ধ্রুৱ ৱর্ণ *val = রিক্ত;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    যদি(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      ঘূৰাই result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    যদি(result) {
      curlx_dyn_free(&hdrvalbuf);
      ঘূৰাই result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  ঘূৰাই result;
}

স্থির CURLcode parse_components(সংরচনা Curl_easy *data,
                                 ধ্রুৱ ৱর্ণ *query,
                                 ধ্রুৱ ৱর্ণ **components,
                                 size_t *ncomp_out,
                                 ৱর্ণ **hdrs_copy_out)
{
  ধ্রুৱ ৱর্ণ *hdrs = CURL_EASY_STR(data, STRING_HTTPSIG_HEADERS);
  size_t ncomp = 0;

  *hdrs_copy_out = রিক্ত;
  যদি(hdrs && *hdrs) {
    ৱর্ণ *p;
    ৱর্ণ *hdrs_copy = curlx_strdup(hdrs);
    যদি(!hdrs_copy)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    যেতিয়ালৈ(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      ৱর্ণ *start;
      size_t tlen = 0;
      ধ্রুৱ ৱর্ণ *p2 = p;

      curlx_str_passblanks(&p2);
      যদি(!*p2)
        ভঙ্গ;
      p = start = CURL_UNCONST(p2);
      যেতিয়ালৈ(*p && !ISBLANK(*p)) {
        যদি((*p == '\"') || (*p == '\\'))
          ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
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
          failf(data, "httpsig: empty header component name");
          ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
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
          failf(data, "httpsig: unknown component '%s'", start);
          ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    যদি(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    }
    যদি(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned পূর্ণাঙ্ক)HTTPSIG_MAX_COMPONENTS);
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      বাবে(i = 0; i < ncomp; i++) {
        বাবে(j = i + 1; j < ncomp; j++) {
          যদি(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
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
  ঘূৰাই CURLE_OK;
}

স্থির time_t httpsig_get_created(শূন্য)
{
#ifdef DEBUGBUILD
  ৱর্ণ *force = getenv("CURL_FORCETIME");
  যদি(force && *force) {
    ৱর্ণ *sigts = getenv("CURL_HTTPSIG_CREATED");
    যদি(sigts && *sigts) {
      ধ্রুৱ ৱর্ণ *p = sigts;
      curl_off_t অঙ্ক;
      যদি(!curlx_str_সংখ্যা(&p, &অঙ্ক, CURL_OFF_T_MAX))
        ঘূৰাই (time_t)অঙ্ক;
    }
    ঘূৰাই 0;
  }
#endif
  ঘূৰাই time(রিক্ত);
}

স্থির CURLcode httpsig_sign_base(সংরচনা Curl_easy *data,
                                  গণনা httpsig_alg alg,
                                  ধ্রুৱ unsigned ৱর্ণ *keybuf,
                                  size_t keylen,
                                  ধ্রুৱ সংরচনা dynbuf *sig_base,
                                  unsigned ৱর্ণ *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode result;

  ৱিকল্প(alg) {
  পক্ষ HTTPSIG_ALG_ED25519:
    result = Curl_ed25519_sign(
      keybuf, keylen,
      (ধ্রুৱ unsigned ৱর্ণ *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    ভঙ্গ;
  পক্ষ HTTPSIG_ALG_HMAC_SHA256:
    result = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ধ্রুৱ unsigned ৱর্ণ *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    যদি(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    ভঙ্গ;
  স্ৱভাৱ:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ভঙ্গ;
  }

  যদি(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  ঘূৰাই result;
}

CURLcode Curl_output_httpsig(সংরচনা Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  সংরচনা connectdata *conn = data->conn;
  ধ্রুৱ ৱর্ণ *path;
  ধ্রুৱ ৱর্ণ *query;
  Curl_HttpReq httpreq;
  ধ্রুৱ ৱর্ণ *method = রিক্ত;
  ধ্রুৱ ৱর্ণ *hexkey = CURL_EASY_STR(data, STRING_HTTPSIG_চাবি);
  ধ্রুৱ ৱর্ণ *keyid = CURL_EASY_STR(data, STRING_HTTPSIG_KEYID);
  গণনা httpsig_alg alg;
  time_t created;
  সংরচনা dynbuf sig_params;
  সংরচনা dynbuf sig_base;
  সংরচনা dynbuf sig_hdr;
  সংরচনা dynbuf প্ৰবেশ_hdr;
  সংরচনা dynbuf authority_buf;
  ধ্রুৱ ৱর্ণ *authority;
  ধ্রুৱ ৱর্ণ *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  unsigned ৱর্ণ *keybuf = রিক্ত;
  size_t keylen = 0;
  unsigned ৱর্ণ raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  ৱর্ণ *auth_headers = রিক্ত;
  ৱর্ণ *hdrs_copy = রিক্ত;
  সংরচনা dynbuf query_dyn;

  alg = id_to_alg(data->set.httpsig_algorithm);
  যদি(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }

  যদি(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }
  যদি(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&প্ৰবেশ_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  যদি(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    গচ্ছ done;
  }

  result = decode_hex_চাবি(data, hexkey, &keybuf, &keylen);
  যদি(result)
    গচ্ছ fail;

  যদি(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    গচ্ছ fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  যদি(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  যদি(result)
    গচ্ছ fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  যদি(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  অন্যথা
    result = curlx_dyn_add(&query_dyn, "?");
  যদি(result)
    গচ্ছ fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  যদি(result)
    গচ্ছ fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  যদি(result)
    গচ্ছ fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  যদি(result)
    গচ্ছ fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  যদি(result)
    গচ্ছ fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  যদি(result)
    গচ্ছ fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  যদি(result)
    গচ্ছ fail;

  result = curlx_dyn_addf(&প্ৰবেশ_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  যদি(result)
    গচ্ছ fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&প্ৰবেশ_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  যদি(!auth_headers)
    গচ্ছ fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&প্ৰবেশ_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  যদি(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, পরিমাণ(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&প্ৰবেশ_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  ঘূৰাই result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
