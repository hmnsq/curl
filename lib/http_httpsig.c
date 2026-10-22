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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "http_httpsig.h"
#அந்தர்பாவ "curl_ed25519.h"
#அந்தர்பாவ "curl_hmac.h"
#அந்தர்பாவ "curl_sha256.h"
#அந்தர்பாவ "http.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "slist.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "curlx/base64.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "strcase.h"

#அந்தர்பாவ <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

எண்ணிக்கை httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

நிலையான மாறிலி எழுத்து *alg_to_str(எண்ணிக்கை httpsig_alg alg)
{
  தேர்வு(alg) {
  நிலை HTTPSIG_ALG_ED25519:
    திரும்பு "ed25519";
  நிலை HTTPSIG_ALG_HMAC_SHA256:
    திரும்பு "hmac-sha256";
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு வெற்று;
}

நிலையான எண்ணிக்கை httpsig_alg id_to_alg(uint8_t val)
{
  தேர்வு(val) {
  நிலை CURLHTTPSIG_ED25519:
    திரும்பு HTTPSIG_ALG_ED25519;
  நிலை CURLHTTPSIG_HMAC_SHA256:
    திரும்பு HTTPSIG_ALG_HMAC_SHA256;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு HTTPSIG_ALG_UNKNOWN;
}

நிலையான CURLcode decode_hex_சாவி(கட்டமைப்பு Curl_easy *data,
                               மாறிலி எழுத்து *hexstr,
                               unsigned எழுத்து **keyout,
                               size_t *keylen)
{
  size_t len, i;
  unsigned எழுத்து *keybuf;

  *keyout = வெற்று;
  *keylen = 0;

  len = strlen(hexstr);
  போது(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  எனில்(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  எனில்(len > CURL_MAX_உள்ளீடு_LENGTH) {
    failf(data, "httpsig: hex key too long");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  எனில்(!keybuf)
    திரும்பு CURLE_OUT_OF_MEMORY;

  க்கு(i = 0; i < len; i += 2) {
    எனில்(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned எழுத்து)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  திரும்பு CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
நிலையான CURLcode httpsig_authority(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn,
                                  கட்டமைப்பு dynbuf *authority_buf)
{
  மாறிலி எழுத்து *h = data->state.http_host;

  எனில்(h && curl_strnequal(h, "host:", 5)) {
    மாறிலி எழுத்து *மதிப்பு = h + 5;
    மாறிலி எழுத்து *end;

    போது(ISBLANK(*மதிப்பு))
      மதிப்பு++;
    எனில்(*மதிப்பு) {
      CURLcode result;

      end = மதிப்பு;
      போது(*end && !ISNEWLINE(*end))
        end++;
      போது(end > மதிப்பு && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, மதிப்பு, (size_t)(end - மதிப்பு));
      எனில்(result)
        திரும்பு result;
      திரும்பு CURLE_OK;
    }
  }

  {
    மாறிலி எழுத்து *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    எனில்((conn->origin->scheme->defport != port) && port)
      திரும்பு curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    திரும்பு curlx_dyn_add(authority_buf, hostname);
  }
}

நிலையான CURLcode sf_append_quoted(கட்டமைப்பு dynbuf *buf, மாறிலி எழுத்து *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  எனில்(result)
    திரும்பு result;
  போது(*str) {
    எனில்(ISCNTRL(*str))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    எனில்(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      எனில்(result)
        திரும்பு result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    எனில்(result)
      திரும்பு result;
    str++;
  }
  திரும்பு curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
நிலையான CURLcode sf_encode_byte_seq(மாறிலி unsigned எழுத்து *raw, size_t rawlen,
                                   கட்டமைப்பு dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  எழுத்து *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  எனில்(result)
    திரும்பு result;

  result = curlx_dyn_addn(out, ":", 1);
  எனில்(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  எனில்(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  திரும்பு result;
}

நிலையான CURLcode build_sig_params(கட்டமைப்பு dynbuf *params,
                                 மாறிலி எழுத்து **components, size_t மொத்தம்,
                                 time_t created, மாறிலி எழுத்து *keyid,
                                 எண்ணிக்கை httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  எனில்(result)
    திரும்பு result;

  க்கு(i = 0; i < மொத்தம்; i++) {
    எனில்(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      எனில்(result)
        திரும்பு result;
    }
    result = sf_append_quoted(params, components[i]);
    எனில்(result)
      திரும்பு result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  எனில்(result)
    திரும்பு result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  எனில்(result)
    திரும்பு result;

  எனில்(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    எனில்(result)
      திரும்பு result;
    result = sf_append_quoted(params, keyid);
    எனில்(result)
      திரும்பு result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  திரும்பு result;
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
நிலையான CURLcode resolve_component(மாறிலி எழுத்து *name,
                                  மாறிலி எழுத்து *method,
                                  மாறிலி எழுத்து *authority,
                                  மாறிலி எழுத்து *path,
                                  மாறிலி எழுத்து *query,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு dynbuf *valbuf,
                                  மாறிலி எழுத்து **out)
{
  *out = வெற்று;

  எனில்(name[0] == '@') {
    எனில்(curl_strequal(name, SIG_METHOD))
      *out = method;
    இல்லையெனில் எனில்(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    இல்லையெனில் எனில்(curl_strequal(name, SIG_PATH))
      *out = path;
    இல்லையெனில் எனில்(curl_strequal(name, SIG_QUERY))
      *out = query;
    இல்லையெனில் {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }
    எனில்(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }
    திரும்பு CURLE_OK;
  }
  இல்லையெனில் {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    கட்டமைப்பு curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    க்கு(head = data->set.headers; head; head = head->next) {
      எனில்(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        மாறிலி எழுத்து *p = strchr(head->data, ':');
        எனில்(p) {
          CURLcode result;
          கட்டமைப்பு Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          எனில்(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            எனில்(result)
              திரும்பு result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          எனில்(result)
            திரும்பு result;
          found = TRUE;
        }
      }
    }

    எனில்(found) {
      *out = curlx_dyn_ptr(valbuf);
      திரும்பு CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

நிலையான CURLcode build_sig_base(கட்டமைப்பு dynbuf *base,
                               மாறிலி எழுத்து **components, size_t மொத்தம்,
                               மாறிலி எழுத்து *method,
                               மாறிலி எழுத்து *authority,
                               மாறிலி எழுத்து *path,
                               மாறிலி எழுத்து *query,
                               கட்டமைப்பு Curl_easy *data,
                               மாறிலி எழுத்து *sig_params)
{
  CURLcode result;
  size_t i;
  கட்டமைப்பு dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  க்கு(i = 0; i < மொத்தம்; i++) {
    மாறிலி எழுத்து *val = வெற்று;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    எனில்(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      திரும்பு result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    எனில்(result) {
      curlx_dyn_free(&hdrvalbuf);
      திரும்பு result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  திரும்பு result;
}

நிலையான CURLcode parse_components(கட்டமைப்பு Curl_easy *data,
                                 மாறிலி எழுத்து *query,
                                 மாறிலி எழுத்து **components,
                                 size_t *ncomp_out,
                                 எழுத்து **hdrs_copy_out)
{
  மாறிலி எழுத்து *hdrs = CURL_EASY_STR(data, STRING_HTTPSIG_HEADERS);
  size_t ncomp = 0;

  *hdrs_copy_out = வெற்று;
  எனில்(hdrs && *hdrs) {
    எழுத்து *p;
    எழுத்து *hdrs_copy = curlx_strdup(hdrs);
    எனில்(!hdrs_copy)
      திரும்பு CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    போது(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      எழுத்து *start;
      size_t tlen = 0;
      மாறிலி எழுத்து *p2 = p;

      curlx_str_passblanks(&p2);
      எனில்(!*p2)
        நிறுத்து;
      p = start = CURL_UNCONST(p2);
      போது(*p && !ISBLANK(*p)) {
        எனில்((*p == '\"') || (*p == '\\'))
          திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      எனில்(*p)
        *p++ = '\0';

      எனில்(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        எனில்(!tlen) {
          failf(data, "httpsig: empty header component name");
          திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      இல்லையெனில் {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        எனில்(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        இல்லையெனில் எனில்(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        இல்லையெனில் எனில்(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        இல்லையெனில் எனில்(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        இல்லையெனில் {
          failf(data, "httpsig: unknown component '%s'", start);
          திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    எனில்(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }
    எனில்(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned முழு)HTTPSIG_MAX_COMPONENTS);
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      க்கு(i = 0; i < ncomp; i++) {
        க்கு(j = i + 1; j < ncomp; j++) {
          எனில்(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  இல்லையெனில் {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    எனில்(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  திரும்பு CURLE_OK;
}

நிலையான time_t httpsig_get_created(வெறுமை)
{
#ifdef DEBUGBUILD
  எழுத்து *force = getenv("CURL_FORCETIME");
  எனில்(force && *force) {
    எழுத்து *sigts = getenv("CURL_HTTPSIG_CREATED");
    எனில்(sigts && *sigts) {
      மாறிலி எழுத்து *p = sigts;
      curl_off_t இலக்கம்;
      எனில்(!curlx_str_எண்(&p, &இலக்கம், CURL_OFF_T_MAX))
        திரும்பு (time_t)இலக்கம்;
    }
    திரும்பு 0;
  }
#endif
  திரும்பு time(வெற்று);
}

நிலையான CURLcode httpsig_sign_base(கட்டமைப்பு Curl_easy *data,
                                  எண்ணிக்கை httpsig_alg alg,
                                  மாறிலி unsigned எழுத்து *keybuf,
                                  size_t keylen,
                                  மாறிலி கட்டமைப்பு dynbuf *sig_base,
                                  unsigned எழுத்து *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode result;

  தேர்வு(alg) {
  நிலை HTTPSIG_ALG_ED25519:
    result = Curl_ed25519_sign(
      keybuf, keylen,
      (மாறிலி unsigned எழுத்து *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    நிறுத்து;
  நிலை HTTPSIG_ALG_HMAC_SHA256:
    result = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (மாறிலி unsigned எழுத்து *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    எனில்(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    நிறுத்து;
  இயல்பு:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    நிறுத்து;
  }

  எனில்(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  திரும்பு result;
}

CURLcode Curl_output_httpsig(கட்டமைப்பு Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  கட்டமைப்பு connectdata *conn = data->conn;
  மாறிலி எழுத்து *path;
  மாறிலி எழுத்து *query;
  Curl_HttpReq httpreq;
  மாறிலி எழுத்து *method = வெற்று;
  மாறிலி எழுத்து *hexkey = CURL_EASY_STR(data, STRING_HTTPSIG_சாவி);
  மாறிலி எழுத்து *keyid = CURL_EASY_STR(data, STRING_HTTPSIG_KEYID);
  எண்ணிக்கை httpsig_alg alg;
  time_t created;
  கட்டமைப்பு dynbuf sig_params;
  கட்டமைப்பு dynbuf sig_base;
  கட்டமைப்பு dynbuf sig_hdr;
  கட்டமைப்பு dynbuf உள்ளீடு_hdr;
  கட்டமைப்பு dynbuf authority_buf;
  மாறிலி எழுத்து *authority;
  மாறிலி எழுத்து *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  unsigned எழுத்து *keybuf = வெற்று;
  size_t keylen = 0;
  unsigned எழுத்து raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  எழுத்து *auth_headers = வெற்று;
  எழுத்து *hdrs_copy = வெற்று;
  கட்டமைப்பு dynbuf query_dyn;

  alg = id_to_alg(data->set.httpsig_algorithm);
  எனில்(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  எனில்(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
  எனில்(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&உள்ளீடு_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  எனில்(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    செல் done;
  }

  result = decode_hex_சாவி(data, hexkey, &keybuf, &keylen);
  எனில்(result)
    செல் fail;

  எனில்(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  எனில்(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  எனில்(result)
    செல் fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  எனில்(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  இல்லையெனில்
    result = curlx_dyn_add(&query_dyn, "?");
  எனில்(result)
    செல் fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  எனில்(result)
    செல் fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  எனில்(result)
    செல் fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  எனில்(result)
    செல் fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  எனில்(result)
    செல் fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  எனில்(result)
    செல் fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  எனில்(result)
    செல் fail;

  result = curlx_dyn_addf(&உள்ளீடு_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  எனில்(result)
    செல் fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&உள்ளீடு_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  எனில்(!auth_headers)
    செல் fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&உள்ளீடு_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  எனில்(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, அளவு(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&உள்ளீடு_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  திரும்பு result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
