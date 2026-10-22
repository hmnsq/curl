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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "http_httpsig.h"
#അന്തര്ഭാവ "curl_ed25519.h"
#അന്തര്ഭാവ "curl_hmac.h"
#അന്തര്ഭാവ "curl_sha256.h"
#അന്തര്ഭാവ "http.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "slist.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "curlx/base64.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "strcase.h"

#അന്തര്ഭാവ <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

എണ്ണൽ httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

സ്ഥിതം സ്ഥിരം അക്ഷരം *alg_to_str(എണ്ണൽ httpsig_alg alg)
{
  തിരഞ്ഞെടുപ്പ്(alg) {
  അവസ്ഥ HTTPSIG_ALG_ED25519:
    തിരികെ "ed25519";
  അവസ്ഥ HTTPSIG_ALG_HMAC_SHA256:
    തിരികെ "hmac-sha256";
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ ശൂന്യ;
}

സ്ഥിതം എണ്ണൽ httpsig_alg id_to_alg(uint8_t val)
{
  തിരഞ്ഞെടുപ്പ്(val) {
  അവസ്ഥ CURLHTTPSIG_ED25519:
    തിരികെ HTTPSIG_ALG_ED25519;
  അവസ്ഥ CURLHTTPSIG_HMAC_SHA256:
    തിരികെ HTTPSIG_ALG_HMAC_SHA256;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ HTTPSIG_ALG_UNKNOWN;
}

സ്ഥിതം CURLcode decode_hex_താക്കോൽ(ഘടന Curl_easy *data,
                               സ്ഥിരം അക്ഷരം *hexstr,
                               unsigned അക്ഷരം **keyout,
                               size_t *keylen)
{
  size_t len, i;
  unsigned അക്ഷരം *keybuf;

  *keyout = ശൂന്യ;
  *keylen = 0;

  len = strlen(hexstr);
  ആകുമ്പോൾ(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  എങ്കിൽ(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  എങ്കിൽ(len > CURL_MAX_നിവേശം_LENGTH) {
    failf(data, "httpsig: hex key too long");
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  എങ്കിൽ(!keybuf)
    തിരികെ CURLE_OUT_OF_MEMORY;

  വേണ്ടി(i = 0; i < len; i += 2) {
    എങ്കിൽ(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned അക്ഷരം)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  തിരികെ CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
സ്ഥിതം CURLcode httpsig_authority(ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn,
                                  ഘടന dynbuf *authority_buf)
{
  സ്ഥിരം അക്ഷരം *h = data->state.http_host;

  എങ്കിൽ(h && curl_strnequal(h, "host:", 5)) {
    സ്ഥിരം അക്ഷരം *മൂല്യം = h + 5;
    സ്ഥിരം അക്ഷരം *end;

    ആകുമ്പോൾ(ISBLANK(*മൂല്യം))
      മൂല്യം++;
    എങ്കിൽ(*മൂല്യം) {
      CURLcode result;

      end = മൂല്യം;
      ആകുമ്പോൾ(*end && !ISNEWLINE(*end))
        end++;
      ആകുമ്പോൾ(end > മൂല്യം && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, മൂല്യം, (size_t)(end - മൂല്യം));
      എങ്കിൽ(result)
        തിരികെ result;
      തിരികെ CURLE_OK;
    }
  }

  {
    സ്ഥിരം അക്ഷരം *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    എങ്കിൽ((conn->origin->scheme->defport != port) && port)
      തിരികെ curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    തിരികെ curlx_dyn_add(authority_buf, hostname);
  }
}

സ്ഥിതം CURLcode sf_append_quoted(ഘടന dynbuf *buf, സ്ഥിരം അക്ഷരം *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  എങ്കിൽ(result)
    തിരികെ result;
  ആകുമ്പോൾ(*str) {
    എങ്കിൽ(ISCNTRL(*str))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    എങ്കിൽ(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    എങ്കിൽ(result)
      തിരികെ result;
    str++;
  }
  തിരികെ curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
സ്ഥിതം CURLcode sf_encode_byte_seq(സ്ഥിരം unsigned അക്ഷരം *raw, size_t rawlen,
                                   ഘടന dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  അക്ഷരം *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  എങ്കിൽ(result)
    തിരികെ result;

  result = curlx_dyn_addn(out, ":", 1);
  എങ്കിൽ(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  എങ്കിൽ(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  തിരികെ result;
}

സ്ഥിതം CURLcode build_sig_params(ഘടന dynbuf *params,
                                 സ്ഥിരം അക്ഷരം **components, size_t എണ്ണം,
                                 time_t created, സ്ഥിരം അക്ഷരം *keyid,
                                 എണ്ണൽ httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  എങ്കിൽ(result)
    തിരികെ result;

  വേണ്ടി(i = 0; i < എണ്ണം; i++) {
    എങ്കിൽ(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    result = sf_append_quoted(params, components[i]);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  എങ്കിൽ(result)
    തിരികെ result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    എങ്കിൽ(result)
      തിരികെ result;
    result = sf_append_quoted(params, keyid);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  തിരികെ result;
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
സ്ഥിതം CURLcode resolve_component(സ്ഥിരം അക്ഷരം *name,
                                  സ്ഥിരം അക്ഷരം *method,
                                  സ്ഥിരം അക്ഷരം *authority,
                                  സ്ഥിരം അക്ഷരം *path,
                                  സ്ഥിരം അക്ഷരം *query,
                                  ഘടന Curl_easy *data,
                                  ഘടന dynbuf *valbuf,
                                  സ്ഥിരം അക്ഷരം **out)
{
  *out = ശൂന്യ;

  എങ്കിൽ(name[0] == '@') {
    എങ്കിൽ(curl_strequal(name, SIG_METHOD))
      *out = method;
    അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(name, SIG_PATH))
      *out = path;
    അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(name, SIG_QUERY))
      *out = query;
    അല്ലെങ്കിൽ {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    എങ്കിൽ(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    തിരികെ CURLE_OK;
  }
  അല്ലെങ്കിൽ {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    ഘടന curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    വേണ്ടി(head = data->set.headers; head; head = head->next) {
      എങ്കിൽ(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        സ്ഥിരം അക്ഷരം *p = strchr(head->data, ':');
        എങ്കിൽ(p) {
          CURLcode result;
          ഘടന Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          എങ്കിൽ(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            എങ്കിൽ(result)
              തിരികെ result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          എങ്കിൽ(result)
            തിരികെ result;
          found = TRUE;
        }
      }
    }

    എങ്കിൽ(found) {
      *out = curlx_dyn_ptr(valbuf);
      തിരികെ CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

സ്ഥിതം CURLcode build_sig_base(ഘടന dynbuf *base,
                               സ്ഥിരം അക്ഷരം **components, size_t എണ്ണം,
                               സ്ഥിരം അക്ഷരം *method,
                               സ്ഥിരം അക്ഷരം *authority,
                               സ്ഥിരം അക്ഷരം *path,
                               സ്ഥിരം അക്ഷരം *query,
                               ഘടന Curl_easy *data,
                               സ്ഥിരം അക്ഷരം *sig_params)
{
  CURLcode result;
  size_t i;
  ഘടന dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  വേണ്ടി(i = 0; i < എണ്ണം; i++) {
    സ്ഥിരം അക്ഷരം *val = ശൂന്യ;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    എങ്കിൽ(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      തിരികെ result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    എങ്കിൽ(result) {
      curlx_dyn_free(&hdrvalbuf);
      തിരികെ result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  തിരികെ result;
}

സ്ഥിതം CURLcode parse_components(ഘടന Curl_easy *data,
                                 സ്ഥിരം അക്ഷരം *query,
                                 സ്ഥിരം അക്ഷരം **components,
                                 size_t *ncomp_out,
                                 അക്ഷരം **hdrs_copy_out)
{
  സ്ഥിരം അക്ഷരം *hdrs = CURL_EASY_STR(data, STRING_HTTPSIG_HEADERS);
  size_t ncomp = 0;

  *hdrs_copy_out = ശൂന്യ;
  എങ്കിൽ(hdrs && *hdrs) {
    അക്ഷരം *p;
    അക്ഷരം *hdrs_copy = curlx_strdup(hdrs);
    എങ്കിൽ(!hdrs_copy)
      തിരികെ CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    ആകുമ്പോൾ(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      അക്ഷരം *start;
      size_t tlen = 0;
      സ്ഥിരം അക്ഷരം *p2 = p;

      curlx_str_passblanks(&p2);
      എങ്കിൽ(!*p2)
        നിർത്തുക;
      p = start = CURL_UNCONST(p2);
      ആകുമ്പോൾ(*p && !ISBLANK(*p)) {
        എങ്കിൽ((*p == '\"') || (*p == '\\'))
          തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      എങ്കിൽ(*p)
        *p++ = '\0';

      എങ്കിൽ(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        എങ്കിൽ(!tlen) {
          failf(data, "httpsig: empty header component name");
          തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      അല്ലെങ്കിൽ {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        എങ്കിൽ(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        അല്ലെങ്കിൽ {
          failf(data, "httpsig: unknown component '%s'", start);
          തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    എങ്കിൽ(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    എങ്കിൽ(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned പൂർണ്ണസംഖ്യ)HTTPSIG_MAX_COMPONENTS);
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      വേണ്ടി(i = 0; i < ncomp; i++) {
        വേണ്ടി(j = i + 1; j < ncomp; j++) {
          എങ്കിൽ(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  അല്ലെങ്കിൽ {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    എങ്കിൽ(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  തിരികെ CURLE_OK;
}

സ്ഥിതം time_t httpsig_get_created(ശൂന്യം)
{
#ifdef DEBUGBUILD
  അക്ഷരം *force = getenv("CURL_FORCETIME");
  എങ്കിൽ(force && *force) {
    അക്ഷരം *sigts = getenv("CURL_HTTPSIG_CREATED");
    എങ്കിൽ(sigts && *sigts) {
      സ്ഥിരം അക്ഷരം *p = sigts;
      curl_off_t അക്കം;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &അക്കം, CURL_OFF_T_MAX))
        തിരികെ (time_t)അക്കം;
    }
    തിരികെ 0;
  }
#endif
  തിരികെ time(ശൂന്യ);
}

സ്ഥിതം CURLcode httpsig_sign_base(ഘടന Curl_easy *data,
                                  എണ്ണൽ httpsig_alg alg,
                                  സ്ഥിരം unsigned അക്ഷരം *keybuf,
                                  size_t keylen,
                                  സ്ഥിരം ഘടന dynbuf *sig_base,
                                  unsigned അക്ഷരം *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode result;

  തിരഞ്ഞെടുപ്പ്(alg) {
  അവസ്ഥ HTTPSIG_ALG_ED25519:
    result = Curl_ed25519_sign(
      keybuf, keylen,
      (സ്ഥിരം unsigned അക്ഷരം *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    നിർത്തുക;
  അവസ്ഥ HTTPSIG_ALG_HMAC_SHA256:
    result = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (സ്ഥിരം unsigned അക്ഷരം *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    എങ്കിൽ(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    നിർത്തുക;
  }

  എങ്കിൽ(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  തിരികെ result;
}

CURLcode Curl_output_httpsig(ഘടന Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ഘടന connectdata *conn = data->conn;
  സ്ഥിരം അക്ഷരം *path;
  സ്ഥിരം അക്ഷരം *query;
  Curl_HttpReq httpreq;
  സ്ഥിരം അക്ഷരം *method = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *hexkey = CURL_EASY_STR(data, STRING_HTTPSIG_താക്കോൽ);
  സ്ഥിരം അക്ഷരം *keyid = CURL_EASY_STR(data, STRING_HTTPSIG_KEYID);
  എണ്ണൽ httpsig_alg alg;
  time_t created;
  ഘടന dynbuf sig_params;
  ഘടന dynbuf sig_base;
  ഘടന dynbuf sig_hdr;
  ഘടന dynbuf നിവേശം_hdr;
  ഘടന dynbuf authority_buf;
  സ്ഥിരം അക്ഷരം *authority;
  സ്ഥിരം അക്ഷരം *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  unsigned അക്ഷരം *keybuf = ശൂന്യ;
  size_t keylen = 0;
  unsigned അക്ഷരം raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  അക്ഷരം *auth_headers = ശൂന്യ;
  അക്ഷരം *hdrs_copy = ശൂന്യ;
  ഘടന dynbuf query_dyn;

  alg = id_to_alg(data->set.httpsig_algorithm);
  എങ്കിൽ(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  എങ്കിൽ(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  എങ്കിൽ(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&നിവേശം_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  എങ്കിൽ(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    പോകുക done;
  }

  result = decode_hex_താക്കോൽ(data, hexkey, &keybuf, &keylen);
  എങ്കിൽ(result)
    പോകുക fail;

  എങ്കിൽ(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  എങ്കിൽ(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  എങ്കിൽ(result)
    പോകുക fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  എങ്കിൽ(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  അല്ലെങ്കിൽ
    result = curlx_dyn_add(&query_dyn, "?");
  എങ്കിൽ(result)
    പോകുക fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  എങ്കിൽ(result)
    പോകുക fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  എങ്കിൽ(result)
    പോകുക fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  എങ്കിൽ(result)
    പോകുക fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  എങ്കിൽ(result)
    പോകുക fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  എങ്കിൽ(result)
    പോകുക fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  എങ്കിൽ(result)
    പോകുക fail;

  result = curlx_dyn_addf(&നിവേശം_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  എങ്കിൽ(result)
    പോകുക fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&നിവേശം_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  എങ്കിൽ(!auth_headers)
    പോകുക fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&നിവേശം_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  എങ്കിൽ(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, വലുപ്പം(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&നിവേശം_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  തിരികെ result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
