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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "http_httpsig.h"
#ಅನ್ತರ್ಭಾವ "curl_ed25519.h"
#ಅನ್ತರ್ಭಾವ "curl_hmac.h"
#ಅನ್ತರ್ಭಾವ "curl_sha256.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "slist.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"

#ಅನ್ತರ್ಭಾವ <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

ಗಣನಾ httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *alg_to_str(ಗಣನಾ httpsig_alg alg)
{
  ವಿಕಲ್ಪ(alg) {
  ಪಕ್ಷ HTTPSIG_ALG_ED25519:
    ಪ್ರತ್ಯಾಗಮ "ed25519";
  ಪಕ್ಷ HTTPSIG_ALG_HMAC_SHA256:
    ಪ್ರತ್ಯಾಗಮ "hmac-sha256";
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಗಣನಾ httpsig_alg id_to_alg(uint8_t val)
{
  ವಿಕಲ್ಪ(val) {
  ಪಕ್ಷ CURLHTTPSIG_ED25519:
    ಪ್ರತ್ಯಾಗಮ HTTPSIG_ALG_ED25519;
  ಪಕ್ಷ CURLHTTPSIG_HMAC_SHA256:
    ಪ್ರತ್ಯಾಗಮ HTTPSIG_ALG_HMAC_SHA256;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ HTTPSIG_ALG_UNKNOWN;
}

ಸ್ಥಿರ CURLcode decode_hex_ಕೀಲಿ(ಸಂರಚನಾ Curl_easy *data,
                               ಧ್ರುವ ವರ್ಣ *hexstr,
                               unsigned ವರ್ಣ **keyout,
                               size_t *keylen)
{
  size_t len, i;
  unsigned ವರ್ಣ *keybuf;

  *keyout = ರಿಕ್ತ;
  *keylen = 0;

  len = strlen(hexstr);
  ಯಾವತ್(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  ಯದಿ(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ಯದಿ(len > CURL_MAX_ಒಳಹರಿವು_LENGTH) {
    failf(data, "httpsig: hex key too long");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  ಯದಿ(!keybuf)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಪ್ರತಿ(i = 0; i < len; i += 2) {
    ಯದಿ(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned ವರ್ಣ)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ಸ್ಥಿರ CURLcode httpsig_authority(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn,
                                  ಸಂರಚನಾ dynbuf *authority_buf)
{
  ಧ್ರುವ ವರ್ಣ *h = data->state.http_host;

  ಯದಿ(h && curl_strnequal(h, "host:", 5)) {
    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ = h + 5;
    ಧ್ರುವ ವರ್ಣ *end;

    ಯಾವತ್(ISBLANK(*ಮೌಲ್ಯ))
      ಮೌಲ್ಯ++;
    ಯದಿ(*ಮೌಲ್ಯ) {
      CURLcode result;

      end = ಮೌಲ್ಯ;
      ಯಾವತ್(*end && !ISNEWLINE(*end))
        end++;
      ಯಾವತ್(end > ಮೌಲ್ಯ && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, ಮೌಲ್ಯ, (size_t)(end - ಮೌಲ್ಯ));
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
  }

  {
    ಧ್ರುವ ವರ್ಣ *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    ಯದಿ((conn->origin->scheme->defport != port) && port)
      ಪ್ರತ್ಯಾಗಮ curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    ಪ್ರತ್ಯಾಗಮ curlx_dyn_add(authority_buf, hostname);
  }
}

ಸ್ಥಿರ CURLcode sf_append_quoted(ಸಂರಚನಾ dynbuf *buf, ಧ್ರುವ ವರ್ಣ *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಯಾವತ್(*str) {
    ಯದಿ(ISCNTRL(*str))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    ಯದಿ(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    str++;
  }
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ಸ್ಥಿರ CURLcode sf_encode_byte_seq(ಧ್ರುವ unsigned ವರ್ಣ *raw, size_t rawlen,
                                   ಸಂರಚನಾ dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  ವರ್ಣ *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = curlx_dyn_addn(out, ":", 1);
  ಯದಿ(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  ಯದಿ(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode build_sig_params(ಸಂರಚನಾ dynbuf *params,
                                 ಧ್ರುವ ವರ್ಣ **components, size_t ಎಣಿಕೆ,
                                 time_t created, ಧ್ರುವ ವರ್ಣ *keyid,
                                 ಗಣನಾ httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಪ್ರತಿ(i = 0; i < ಎಣಿಕೆ; i++) {
    ಯದಿ(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    result = sf_append_quoted(params, components[i]);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    result = sf_append_quoted(params, keyid);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode resolve_component(ಧ್ರುವ ವರ್ಣ *name,
                                  ಧ್ರುವ ವರ್ಣ *method,
                                  ಧ್ರುವ ವರ್ಣ *authority,
                                  ಧ್ರುವ ವರ್ಣ *path,
                                  ಧ್ರುವ ವರ್ಣ *query,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ dynbuf *valbuf,
                                  ಧ್ರುವ ವರ್ಣ **out)
{
  *out = ರಿಕ್ತ;

  ಯದಿ(name[0] == '@') {
    ಯದಿ(curl_strequal(name, SIG_METHOD))
      *out = method;
    ಅನ್ಯಥಾ ಯದಿ(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    ಅನ್ಯಥಾ ಯದಿ(curl_strequal(name, SIG_PATH))
      *out = path;
    ಅನ್ಯಥಾ ಯದಿ(curl_strequal(name, SIG_QUERY))
      *out = query;
    ಅನ್ಯಥಾ {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ಯದಿ(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಅನ್ಯಥಾ {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    ಸಂರಚನಾ curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    ಪ್ರತಿ(head = data->set.headers; head; head = head->next) {
      ಯದಿ(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        ಧ್ರುವ ವರ್ಣ *p = strchr(head->data, ':');
        ಯದಿ(p) {
          CURLcode result;
          ಸಂರಚನಾ Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          ಯದಿ(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            ಯದಿ(result)
              ಪ್ರತ್ಯಾಗಮ result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          ಯದಿ(result)
            ಪ್ರತ್ಯಾಗಮ result;
          found = TRUE;
        }
      }
    }

    ಯದಿ(found) {
      *out = curlx_dyn_ptr(valbuf);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

ಸ್ಥಿರ CURLcode build_sig_base(ಸಂರಚನಾ dynbuf *base,
                               ಧ್ರುವ ವರ್ಣ **components, size_t ಎಣಿಕೆ,
                               ಧ್ರುವ ವರ್ಣ *method,
                               ಧ್ರುವ ವರ್ಣ *authority,
                               ಧ್ರುವ ವರ್ಣ *path,
                               ಧ್ರುವ ವರ್ಣ *query,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಧ್ರುವ ವರ್ಣ *sig_params)
{
  CURLcode result;
  size_t i;
  ಸಂರಚನಾ dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  ಪ್ರತಿ(i = 0; i < ಎಣಿಕೆ; i++) {
    ಧ್ರುವ ವರ್ಣ *val = ರಿಕ್ತ;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    ಯದಿ(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      ಪ್ರತ್ಯಾಗಮ result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    ಯದಿ(result) {
      curlx_dyn_free(&hdrvalbuf);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode parse_components(ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ ವರ್ಣ *query,
                                 ಧ್ರುವ ವರ್ಣ **components,
                                 size_t *ncomp_out,
                                 ವರ್ಣ **hdrs_copy_out)
{
  ಧ್ರುವ ವರ್ಣ *hdrs = CURL_EASY_STR(data, STRING_HTTPSIG_HEADERS);
  size_t ncomp = 0;

  *hdrs_copy_out = ರಿಕ್ತ;
  ಯದಿ(hdrs && *hdrs) {
    ವರ್ಣ *p;
    ವರ್ಣ *hdrs_copy = curlx_strdup(hdrs);
    ಯದಿ(!hdrs_copy)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    ಯಾವತ್(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      ವರ್ಣ *start;
      size_t tlen = 0;
      ಧ್ರುವ ವರ್ಣ *p2 = p;

      curlx_str_passblanks(&p2);
      ಯದಿ(!*p2)
        ಭಙ್ಗ;
      p = start = CURL_UNCONST(p2);
      ಯಾವತ್(*p && !ISBLANK(*p)) {
        ಯದಿ((*p == '\"') || (*p == '\\'))
          ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      ಯದಿ(*p)
        *p++ = '\0';

      ಯದಿ(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        ಯದಿ(!tlen) {
          failf(data, "httpsig: empty header component name");
          ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      ಅನ್ಯಥಾ {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        ಯದಿ(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        ಅನ್ಯಥಾ ಯದಿ(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        ಅನ್ಯಥಾ ಯದಿ(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        ಅನ್ಯಥಾ ಯದಿ(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        ಅನ್ಯಥಾ {
          failf(data, "httpsig: unknown component '%s'", start);
          ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    ಯದಿ(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ಯದಿ(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned ಪೂರ್ಣಾಙ್ಕ)HTTPSIG_MAX_COMPONENTS);
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      ಪ್ರತಿ(i = 0; i < ncomp; i++) {
        ಪ್ರತಿ(j = i + 1; j < ncomp; j++) {
          ಯದಿ(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  ಅನ್ಯಥಾ {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    ಯದಿ(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ time_t httpsig_get_created(ಶೂನ್ಯ)
{
#ifdef DEBUGBUILD
  ವರ್ಣ *force = getenv("CURL_FORCETIME");
  ಯದಿ(force && *force) {
    ವರ್ಣ *sigts = getenv("CURL_HTTPSIG_CREATED");
    ಯದಿ(sigts && *sigts) {
      ಧ್ರುವ ವರ್ಣ *p = sigts;
      curl_off_t ಅಂಕೆ;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, CURL_OFF_T_MAX))
        ಪ್ರತ್ಯಾಗಮ (time_t)ಅಂಕೆ;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
  }
#endif
  ಪ್ರತ್ಯಾಗಮ time(ರಿಕ್ತ);
}

ಸ್ಥಿರ CURLcode httpsig_sign_base(ಸಂರಚನಾ Curl_easy *data,
                                  ಗಣನಾ httpsig_alg alg,
                                  ಧ್ರುವ unsigned ವರ್ಣ *keybuf,
                                  size_t keylen,
                                  ಧ್ರುವ ಸಂರಚನಾ dynbuf *sig_base,
                                  unsigned ವರ್ಣ *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode result;

  ವಿಕಲ್ಪ(alg) {
  ಪಕ್ಷ HTTPSIG_ALG_ED25519:
    result = Curl_ed25519_sign(
      keybuf, keylen,
      (ಧ್ರುವ unsigned ವರ್ಣ *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    ಭಙ್ಗ;
  ಪಕ್ಷ HTTPSIG_ALG_HMAC_SHA256:
    result = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ಧ್ರುವ unsigned ವರ್ಣ *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    ಯದಿ(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಭಙ್ಗ;
  }

  ಯದಿ(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_output_httpsig(ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಧ್ರುವ ವರ್ಣ *path;
  ಧ್ರುವ ವರ್ಣ *query;
  Curl_HttpReq httpreq;
  ಧ್ರುವ ವರ್ಣ *method = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *hexkey = CURL_EASY_STR(data, STRING_HTTPSIG_ಕೀಲಿ);
  ಧ್ರುವ ವರ್ಣ *keyid = CURL_EASY_STR(data, STRING_HTTPSIG_KEYID);
  ಗಣನಾ httpsig_alg alg;
  time_t created;
  ಸಂರಚನಾ dynbuf sig_params;
  ಸಂರಚನಾ dynbuf sig_base;
  ಸಂರಚನಾ dynbuf sig_hdr;
  ಸಂರಚನಾ dynbuf ಒಳಹರಿವು_hdr;
  ಸಂರಚನಾ dynbuf authority_buf;
  ಧ್ರುವ ವರ್ಣ *authority;
  ಧ್ರುವ ವರ್ಣ *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  unsigned ವರ್ಣ *keybuf = ರಿಕ್ತ;
  size_t keylen = 0;
  unsigned ವರ್ಣ raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  ವರ್ಣ *auth_headers = ರಿಕ್ತ;
  ವರ್ಣ *hdrs_copy = ರಿಕ್ತ;
  ಸಂರಚನಾ dynbuf query_dyn;

  alg = id_to_alg(data->set.httpsig_algorithm);
  ಯದಿ(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ಯದಿ(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ಯದಿ(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&ಒಳಹರಿವು_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  ಯದಿ(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    ಗಚ್ಛ done;
  }

  result = decode_hex_ಕೀಲಿ(data, hexkey, &keybuf, &keylen);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  ಯದಿ(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  ಯದಿ(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  ಯದಿ(result)
    ಗಚ್ಛ fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  ಯದಿ(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  ಅನ್ಯಥಾ
    result = curlx_dyn_add(&query_dyn, "?");
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  ಯದಿ(result)
    ಗಚ್ಛ fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  result = curlx_dyn_addf(&ಒಳಹರಿವು_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&ಒಳಹರಿವು_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  ಯದಿ(!auth_headers)
    ಗಚ್ಛ fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&ಒಳಹರಿವು_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  ಯದಿ(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, ಪರಿಮಾಣ(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&ಒಳಹರಿವು_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
