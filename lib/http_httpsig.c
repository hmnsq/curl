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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "http_httpsig.h"
#ଅନ୍ତର୍ଭାଵ "curl_ed25519.h"
#ଅନ୍ତର୍ଭାଵ "curl_hmac.h"
#ଅନ୍ତର୍ଭାଵ "curl_sha256.h"
#ଅନ୍ତର୍ଭାଵ "http.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "slist.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"

#ଅନ୍ତର୍ଭାଵ <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

ଗଣନା httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *alg_to_str(ଗଣନା httpsig_alg alg)
{
  ବାଛ(alg) {
  ସ୍ଥିତି HTTPSIG_ALG_ED25519:
    ଫେରନ୍ତୁ "ed25519";
  ସ୍ଥିତି HTTPSIG_ALG_HMAC_SHA256:
    ଫେରନ୍ତୁ "hmac-sha256";
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଗଣନା httpsig_alg id_to_alg(uint8_t val)
{
  ବାଛ(val) {
  ସ୍ଥିତି CURLHTTPSIG_ED25519:
    ଫେରନ୍ତୁ HTTPSIG_ALG_ED25519;
  ସ୍ଥିତି CURLHTTPSIG_HMAC_SHA256:
    ଫେରନ୍ତୁ HTTPSIG_ALG_HMAC_SHA256;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ HTTPSIG_ALG_UNKNOWN;
}

ସ୍ଥିର CURLcode decode_hex_ଚାବି(ସଂରଚନା Curl_easy *data,
                               ଅଚଳ ଅକ୍ଷର *hexstr,
                               unsigned ଅକ୍ଷର **keyout,
                               size_t *keylen)
{
  size_t len, i;
  unsigned ଅକ୍ଷର *keybuf;

  *keyout = ଖାଲି;
  *keylen = 0;

  len = strlen(hexstr);
  ଯେତେବେଳେ(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  ଯଦି(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ଯଦି(len > CURL_MAX_ନିବେଶ_LENGTH) {
    failf(data, "httpsig: hex key too long");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  ଯଦି(!keybuf)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ପାଇଁ(i = 0; i < len; i += 2) {
    ଯଦି(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned ଅକ୍ଷର)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  ଫେରନ୍ତୁ CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ସ୍ଥିର CURLcode httpsig_authority(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata *conn,
                                  ସଂରଚନା dynbuf *authority_buf)
{
  ଅଚଳ ଅକ୍ଷର *h = data->state.http_host;

  ଯଦି(h && curl_strnequal(h, "host:", 5)) {
    ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ = h + 5;
    ଅଚଳ ଅକ୍ଷର *end;

    ଯେତେବେଳେ(ISBLANK(*ମୂଲ୍ୟ))
      ମୂଲ୍ୟ++;
    ଯଦି(*ମୂଲ୍ୟ) {
      CURLcode result;

      end = ମୂଲ୍ୟ;
      ଯେତେବେଳେ(*end && !ISNEWLINE(*end))
        end++;
      ଯେତେବେଳେ(end > ମୂଲ୍ୟ && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, ମୂଲ୍ୟ, (size_t)(end - ମୂଲ୍ୟ));
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      ଫେରନ୍ତୁ CURLE_OK;
    }
  }

  {
    ଅଚଳ ଅକ୍ଷର *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    ଯଦି((conn->origin->scheme->defport != port) && port)
      ଫେରନ୍ତୁ curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    ଫେରନ୍ତୁ curlx_dyn_add(authority_buf, hostname);
  }
}

ସ୍ଥିର CURLcode sf_append_quoted(ସଂରଚନା dynbuf *buf, ଅଚଳ ଅକ୍ଷର *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଯେତେବେଳେ(*str) {
    ଯଦି(ISCNTRL(*str))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    ଯଦି(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    str++;
  }
  ଫେରନ୍ତୁ curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ସ୍ଥିର CURLcode sf_encode_byte_seq(ଅଚଳ unsigned ଅକ୍ଷର *raw, size_t rawlen,
                                   ସଂରଚନା dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  ଅକ୍ଷର *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = curlx_dyn_addn(out, ":", 1);
  ଯଦି(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  ଯଦି(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode build_sig_params(ସଂରଚନା dynbuf *params,
                                 ଅଚଳ ଅକ୍ଷର **components, size_t ଗଣନା_2,
                                 time_t created, ଅଚଳ ଅକ୍ଷର *keyid,
                                 ଗଣନା httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ପାଇଁ(i = 0; i < ଗଣନା_2; i++) {
    ଯଦି(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    result = sf_append_quoted(params, components[i]);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    result = sf_append_quoted(params, keyid);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode resolve_component(ଅଚଳ ଅକ୍ଷର *name,
                                  ଅଚଳ ଅକ୍ଷର *method,
                                  ଅଚଳ ଅକ୍ଷର *authority,
                                  ଅଚଳ ଅକ୍ଷର *path,
                                  ଅଚଳ ଅକ୍ଷର *query,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା dynbuf *valbuf,
                                  ଅଚଳ ଅକ୍ଷର **out)
{
  *out = ଖାଲି;

  ଯଦି(name[0] == '@') {
    ଯଦି(curl_strequal(name, SIG_METHOD))
      *out = method;
    ଅନ୍ୟଥା ଯଦି(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    ଅନ୍ୟଥା ଯଦି(curl_strequal(name, SIG_PATH))
      *out = path;
    ଅନ୍ୟଥା ଯଦି(curl_strequal(name, SIG_QUERY))
      *out = query;
    ଅନ୍ୟଥା {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ଯଦି(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଅନ୍ୟଥା {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    ସଂରଚନା curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    ପାଇଁ(head = data->set.headers; head; head = head->next) {
      ଯଦି(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        ଅଚଳ ଅକ୍ଷର *p = strchr(head->data, ':');
        ଯଦି(p) {
          CURLcode result;
          ସଂରଚନା Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          ଯଦି(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            ଯଦି(result)
              ଫେରନ୍ତୁ result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          ଯଦି(result)
            ଫେରନ୍ତୁ result;
          found = TRUE;
        }
      }
    }

    ଯଦି(found) {
      *out = curlx_dyn_ptr(valbuf);
      ଫେରନ୍ତୁ CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

ସ୍ଥିର CURLcode build_sig_base(ସଂରଚନା dynbuf *base,
                               ଅଚଳ ଅକ୍ଷର **components, size_t ଗଣନା_2,
                               ଅଚଳ ଅକ୍ଷର *method,
                               ଅଚଳ ଅକ୍ଷର *authority,
                               ଅଚଳ ଅକ୍ଷର *path,
                               ଅଚଳ ଅକ୍ଷର *query,
                               ସଂରଚନା Curl_easy *data,
                               ଅଚଳ ଅକ୍ଷର *sig_params)
{
  CURLcode result;
  size_t i;
  ସଂରଚନା dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  ପାଇଁ(i = 0; i < ଗଣନା_2; i++) {
    ଅଚଳ ଅକ୍ଷର *val = ଖାଲି;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    ଯଦି(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      ଫେରନ୍ତୁ result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    ଯଦି(result) {
      curlx_dyn_free(&hdrvalbuf);
      ଫେରନ୍ତୁ result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode parse_components(ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଅକ୍ଷର *query,
                                 ଅଚଳ ଅକ୍ଷର **components,
                                 size_t *ncomp_out,
                                 ଅକ୍ଷର **hdrs_copy_out)
{
  ଅଚଳ ଅକ୍ଷର *hdrs = CURL_EASY_STR(data, STRING_HTTPSIG_HEADERS);
  size_t ncomp = 0;

  *hdrs_copy_out = ଖାଲି;
  ଯଦି(hdrs && *hdrs) {
    ଅକ୍ଷର *p;
    ଅକ୍ଷର *hdrs_copy = curlx_strdup(hdrs);
    ଯଦି(!hdrs_copy)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    ଯେତେବେଳେ(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      ଅକ୍ଷର *start;
      size_t tlen = 0;
      ଅଚଳ ଅକ୍ଷର *p2 = p;

      curlx_str_passblanks(&p2);
      ଯଦି(!*p2)
        ଭାଙ୍ଗ;
      p = start = CURL_UNCONST(p2);
      ଯେତେବେଳେ(*p && !ISBLANK(*p)) {
        ଯଦି((*p == '\"') || (*p == '\\'))
          ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      ଯଦି(*p)
        *p++ = '\0';

      ଯଦି(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        ଯଦି(!tlen) {
          failf(data, "httpsig: empty header component name");
          ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      ଅନ୍ୟଥା {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        ଯଦି(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        ଅନ୍ୟଥା ଯଦି(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        ଅନ୍ୟଥା ଯଦି(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        ଅନ୍ୟଥା ଯଦି(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        ଅନ୍ୟଥା {
          failf(data, "httpsig: unknown component '%s'", start);
          ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    ଯଦି(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ଯଦି(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)HTTPSIG_MAX_COMPONENTS);
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      ପାଇଁ(i = 0; i < ncomp; i++) {
        ପାଇଁ(j = i + 1; j < ncomp; j++) {
          ଯଦି(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  ଅନ୍ୟଥା {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    ଯଦି(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର time_t httpsig_get_created(ଶୂନ୍ୟ)
{
#ifdef DEBUGBUILD
  ଅକ୍ଷର *force = getenv("CURL_FORCETIME");
  ଯଦି(force && *force) {
    ଅକ୍ଷର *sigts = getenv("CURL_HTTPSIG_CREATED");
    ଯଦି(sigts && *sigts) {
      ଅଚଳ ଅକ୍ଷର *p = sigts;
      curl_off_t ଅଙ୍କ;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &ଅଙ୍କ, CURL_OFF_T_MAX))
        ଫେରନ୍ତୁ (time_t)ଅଙ୍କ;
    }
    ଫେରନ୍ତୁ 0;
  }
#endif
  ଫେରନ୍ତୁ time(ଖାଲି);
}

ସ୍ଥିର CURLcode httpsig_sign_base(ସଂରଚନା Curl_easy *data,
                                  ଗଣନା httpsig_alg alg,
                                  ଅଚଳ unsigned ଅକ୍ଷର *keybuf,
                                  size_t keylen,
                                  ଅଚଳ ସଂରଚନା dynbuf *sig_base,
                                  unsigned ଅକ୍ଷର *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode result;

  ବାଛ(alg) {
  ସ୍ଥିତି HTTPSIG_ALG_ED25519:
    result = Curl_ed25519_sign(
      keybuf, keylen,
      (ଅଚଳ unsigned ଅକ୍ଷର *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HTTPSIG_ALG_HMAC_SHA256:
    result = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ଅଚଳ unsigned ଅକ୍ଷର *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    ଯଦି(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଭାଙ୍ଗ;
  }

  ଯଦି(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_output_httpsig(ସଂରଚନା Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ସଂରଚନା connectdata *conn = data->conn;
  ଅଚଳ ଅକ୍ଷର *path;
  ଅଚଳ ଅକ୍ଷର *query;
  Curl_HttpReq httpreq;
  ଅଚଳ ଅକ୍ଷର *method = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *hexkey = CURL_EASY_STR(data, STRING_HTTPSIG_ଚାବି);
  ଅଚଳ ଅକ୍ଷର *keyid = CURL_EASY_STR(data, STRING_HTTPSIG_KEYID);
  ଗଣନା httpsig_alg alg;
  time_t created;
  ସଂରଚନା dynbuf sig_params;
  ସଂରଚନା dynbuf sig_base;
  ସଂରଚନା dynbuf sig_hdr;
  ସଂରଚନା dynbuf ନିବେଶ_hdr;
  ସଂରଚନା dynbuf authority_buf;
  ଅଚଳ ଅକ୍ଷର *authority;
  ଅଚଳ ଅକ୍ଷର *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  unsigned ଅକ୍ଷର *keybuf = ଖାଲି;
  size_t keylen = 0;
  unsigned ଅକ୍ଷର raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  ଅକ୍ଷର *auth_headers = ଖାଲି;
  ଅକ୍ଷର *hdrs_copy = ଖାଲି;
  ସଂରଚନା dynbuf query_dyn;

  alg = id_to_alg(data->set.httpsig_algorithm);
  ଯଦି(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ଯଦି(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ଯଦି(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&ନିବେଶ_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  ଯଦି(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    ଯାଅ done;
  }

  result = decode_hex_ଚାବି(data, hexkey, &keybuf, &keylen);
  ଯଦି(result)
    ଯାଅ fail;

  ଯଦି(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  ଯଦି(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  ଯଦି(result)
    ଯାଅ fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  ଯଦି(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  ଅନ୍ୟଥା
    result = curlx_dyn_add(&query_dyn, "?");
  ଯଦି(result)
    ଯାଅ fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  ଯଦି(result)
    ଯାଅ fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  ଯଦି(result)
    ଯାଅ fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  ଯଦି(result)
    ଯାଅ fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  ଯଦି(result)
    ଯାଅ fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  ଯଦି(result)
    ଯାଅ fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  ଯଦି(result)
    ଯାଅ fail;

  result = curlx_dyn_addf(&ନିବେଶ_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  ଯଦି(result)
    ଯାଅ fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&ନିବେଶ_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  ଯଦି(!auth_headers)
    ଯାଅ fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&ନିବେଶ_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  ଯଦି(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, ଆକାର(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&ନିବେଶ_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
