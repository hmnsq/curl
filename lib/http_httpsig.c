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
#include "curl_setup.h"

#အကယ်၍ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#include "urldata.h"
#include "http_httpsig.h"
#include "curl_ed25519.h"
#include "curl_hmac.h"
#include "curl_sha256.h"
#include "http.h"
#include "transfer.h"
#include "curl_trc.h"
#include "slist.h"
#include "curlx/dynbuf.h"
#include "curlx/base64.h"
#include "curlx/strdup.h"
#include "curlx/strparse.h"
#include "strcase.h"

#include <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

ga_nanaa httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

static const var_na *alg_to_str(ga_nanaa httpsig_alg alg)
{
  switch(alg) {
  pak_sa HTTPSIG_ALG_ED25519:
    ပြန်ပေး "ed25519";
  pak_sa HTTPSIG_ALG_HMAC_SHA256:
    ပြန်ပေး "hmac-sha256";
  default:
    bha_nga;
  }
  ပြန်ပေး NULL;
}

static ga_nanaa httpsig_alg id_to_alg(uint8_t val)
{
  switch(val) {
  pak_sa CURLHTTPSIG_ED25519:
    ပြန်ပေး HTTPSIG_ALG_ED25519;
  pak_sa CURLHTTPSIG_HMAC_SHA256:
    ပြန်ပေး HTTPSIG_ALG_HMAC_SHA256;
  default:
    bha_nga;
  }
  ပြန်ပေး HTTPSIG_ALG_UNKNOWN;
}

static CURLcode decode_hex_key(sa_nracanaa Curl_easy *data,
                               const var_na *hexstr,
                               unsigned var_na **keyout,
                               size_t *keylen)
{
  size_t len, i;
  unsigned var_na *keybuf;

  *keyout = NULL;
  *keylen = 0;

  len = strlen(hexstr);
  နေစဉ်(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  အကယ်၍(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  }

  အကယ်၍(len > CURL_MAX_INPUT_LENGTH) {
    failf(data, "httpsig: hex key too long");
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  အကယ်၍(!keybuf)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;

  အတွက်(i = 0; i < len; i += 2) {
    အကယ်၍(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned var_na)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  ပြန်ပေး CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
static CURLcode httpsig_authority(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn,
                                  sa_nracanaa dynbuf *authority_buf)
{
  const var_na *h = data->state.http_host;

  အကယ်၍(h && curl_strnequal(h, "host:", 5)) {
    const var_na *value = h + 5;
    const var_na *end;

    နေစဉ်(ISBLANK(*value))
      value++;
    အကယ်၍(*value) {
      CURLcode result;

      end = value;
      နေစဉ်(*end && !ISNEWLINE(*end))
        end++;
      နေစဉ်(end > value && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, value, (size_t)(end - value));
      အကယ်၍(result)
        ပြန်ပေး result;
      ပြန်ပေး CURLE_OK;
    }
  }

  {
    const var_na *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    အကယ်၍((conn->origin->scheme->defport != port) && port)
      ပြန်ပေး curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    ပြန်ပေး curlx_dyn_add(authority_buf, hostname);
  }
}

static CURLcode sf_append_quoted(sa_nracanaa dynbuf *buf, const var_na *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  အကယ်၍(result)
    ပြန်ပေး result;
  နေစဉ်(*str) {
    အကယ်၍(ISCNTRL(*str))
      ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
    အကယ်၍(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      အကယ်၍(result)
        ပြန်ပေး result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    အကယ်၍(result)
      ပြန်ပေး result;
    str++;
  }
  ပြန်ပေး curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
static CURLcode sf_encode_byte_seq(const unsigned var_na *raw, size_t rawlen,
                                   sa_nracanaa dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  var_na *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  အကယ်၍(result)
    ပြန်ပေး result;

  result = curlx_dyn_addn(out, ":", 1);
  အကယ်၍(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  အကယ်၍(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  ပြန်ပေး result;
}

static CURLcode build_sig_params(sa_nracanaa dynbuf *params,
                                 const var_na **components, size_t count,
                                 time_t created, const var_na *keyid,
                                 ga_nanaa httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  အကယ်၍(result)
    ပြန်ပေး result;

  အတွက်(i = 0; i < count; i++) {
    အကယ်၍(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      အကယ်၍(result)
        ပြန်ပေး result;
    }
    result = sf_append_quoted(params, components[i]);
    အကယ်၍(result)
      ပြန်ပေး result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  အကယ်၍(result)
    ပြန်ပေး result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  အကယ်၍(result)
    ပြန်ပေး result;

  အကယ်၍(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    အကယ်၍(result)
      ပြန်ပေး result;
    result = sf_append_quoted(params, keyid);
    အကယ်၍(result)
      ပြန်ပေး result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  ပြန်ပေး result;
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
static CURLcode resolve_component(const var_na *name,
                                  const var_na *method,
                                  const var_na *authority,
                                  const var_na *path,
                                  const var_na *query,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa dynbuf *valbuf,
                                  const var_na **out)
{
  *out = NULL;

  အကယ်၍(name[0] == '@') {
    အကယ်၍(curl_strequal(name, SIG_METHOD))
      *out = method;
    မဟုတ်ပါက အကယ်၍(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    မဟုတ်ပါက အကယ်၍(curl_strequal(name, SIG_PATH))
      *out = path;
    မဟုတ်ပါက အကယ်၍(curl_strequal(name, SIG_QUERY))
      *out = query;
    မဟုတ်ပါက {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
    }
    အကယ်၍(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
    }
    ပြန်ပေး CURLE_OK;
  }
  မဟုတ်ပါက {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    sa_nracanaa curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    အတွက်(head = data->set.headers; head; head = head->next) {
      အကယ်၍(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        const var_na *p = strchr(head->data, ':');
        အကယ်၍(p) {
          CURLcode result;
          sa_nracanaa Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          အကယ်၍(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            အကယ်၍(result)
              ပြန်ပေး result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          အကယ်၍(result)
            ပြန်ပေး result;
          found = TRUE;
        }
      }
    }

    အကယ်၍(found) {
      *out = curlx_dyn_ptr(valbuf);
      ပြန်ပေး CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

static CURLcode build_sig_base(sa_nracanaa dynbuf *base,
                               const var_na **components, size_t count,
                               const var_na *method,
                               const var_na *authority,
                               const var_na *path,
                               const var_na *query,
                               sa_nracanaa Curl_easy *data,
                               const var_na *sig_params)
{
  CURLcode result;
  size_t i;
  sa_nracanaa dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  အတွက်(i = 0; i < count; i++) {
    const var_na *val = NULL;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    အကယ်၍(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      ပြန်ပေး result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    အကယ်၍(result) {
      curlx_dyn_free(&hdrvalbuf);
      ပြန်ပေး result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  ပြန်ပေး result;
}

static CURLcode parse_components(sa_nracanaa Curl_easy *data,
                                 const var_na *query,
                                 const var_na **components,
                                 size_t *ncomp_out,
                                 var_na **hdrs_copy_out)
{
  const var_na *hdrs = CURL_EASY_STR(data, STRING_HTTPSIG_HEADERS);
  size_t ncomp = 0;

  *hdrs_copy_out = NULL;
  အကယ်၍(hdrs && *hdrs) {
    var_na *p;
    var_na *hdrs_copy = curlx_strdup(hdrs);
    အကယ်၍(!hdrs_copy)
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    နေစဉ်(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      var_na *start;
      size_t tlen = 0;
      const var_na *p2 = p;

      curlx_str_passblanks(&p2);
      အကယ်၍(!*p2)
        bha_nga;
      p = start = CURL_UNCONST(p2);
      နေစဉ်(*p && !ISBLANK(*p)) {
        အကယ်၍((*p == '\"') || (*p == '\\'))
          ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      အကယ်၍(*p)
        *p++ = '\0';

      အကယ်၍(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        အကယ်၍(!tlen) {
          failf(data, "httpsig: empty header component name");
          ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      မဟုတ်ပါက {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        အကယ်၍(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        မဟုတ်ပါက အကယ်၍(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        မဟုတ်ပါက အကယ်၍(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        မဟုတ်ပါက အကယ်၍(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        မဟုတ်ပါက {
          failf(data, "httpsig: unknown component '%s'", start);
          ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    အကယ်၍(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
    }
    အကယ်၍(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned ကိန်းပြည့်)HTTPSIG_MAX_COMPONENTS);
      ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      အတွက်(i = 0; i < ncomp; i++) {
        အတွက်(j = i + 1; j < ncomp; j++) {
          အကယ်၍(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  မဟုတ်ပါက {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    အကယ်၍(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  ပြန်ပေး CURLE_OK;
}

static time_t httpsig_get_created(void)
{
#ifdef DEBUGBUILD
  var_na *force = getenv("CURL_FORCETIME");
  အကယ်၍(force && *force) {
    var_na *sigts = getenv("CURL_HTTPSIG_CREATED");
    အကယ်၍(sigts && *sigts) {
      const var_na *p = sigts;
      curl_off_t num;
      အကယ်၍(!curlx_str_number(&p, &num, CURL_OFF_T_MAX))
        ပြန်ပေး (time_t)num;
    }
    ပြန်ပေး 0;
  }
#endif
  ပြန်ပေး time(NULL);
}

static CURLcode httpsig_sign_base(sa_nracanaa Curl_easy *data,
                                  ga_nanaa httpsig_alg alg,
                                  const unsigned var_na *keybuf,
                                  size_t keylen,
                                  const sa_nracanaa dynbuf *sig_base,
                                  unsigned var_na *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode result;

  switch(alg) {
  pak_sa HTTPSIG_ALG_ED25519:
    result = Curl_ed25519_sign(
      keybuf, keylen,
      (const unsigned var_na *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    bha_nga;
  pak_sa HTTPSIG_ALG_HMAC_SHA256:
    result = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (const unsigned var_na *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    အကယ်၍(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    bha_nga;
  default:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    bha_nga;
  }

  အကယ်၍(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  ပြန်ပေး result;
}

CURLcode Curl_output_httpsig(sa_nracanaa Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  sa_nracanaa connectdata *conn = data->conn;
  const var_na *path;
  const var_na *query;
  Curl_HttpReq httpreq;
  const var_na *method = NULL;
  const var_na *hexkey = CURL_EASY_STR(data, STRING_HTTPSIG_KEY);
  const var_na *keyid = CURL_EASY_STR(data, STRING_HTTPSIG_KEYID);
  ga_nanaa httpsig_alg alg;
  time_t created;
  sa_nracanaa dynbuf sig_params;
  sa_nracanaa dynbuf sig_base;
  sa_nracanaa dynbuf sig_hdr;
  sa_nracanaa dynbuf input_hdr;
  sa_nracanaa dynbuf authority_buf;
  const var_na *authority;
  const var_na *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  unsigned var_na *keybuf = NULL;
  size_t keylen = 0;
  unsigned var_na raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  var_na *auth_headers = NULL;
  var_na *hdrs_copy = NULL;
  sa_nracanaa dynbuf query_dyn;

  alg = id_to_alg(data->set.httpsig_algorithm);
  အကယ်၍(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  }

  အကယ်၍(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  }
  အကယ်၍(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&input_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  အကယ်၍(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    goto done;
  }

  result = decode_hex_key(data, hexkey, &keybuf, &keylen);
  အကယ်၍(result)
    goto fail;

  အကယ်၍(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  အကယ်၍(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  အကယ်၍(result)
    goto fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  အကယ်၍(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  မဟုတ်ပါက
    result = curlx_dyn_add(&query_dyn, "?");
  အကယ်၍(result)
    goto fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  အကယ်၍(result)
    goto fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  အကယ်၍(result)
    goto fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  အကယ်၍(result)
    goto fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  အကယ်၍(result)
    goto fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  အကယ်၍(result)
    goto fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  အကယ်၍(result)
    goto fail;

  result = curlx_dyn_addf(&input_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  အကယ်၍(result)
    goto fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&input_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  အကယ်၍(!auth_headers)
    goto fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&input_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  အကယ်၍(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, parimaa_na(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&input_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  ပြန်ပေး result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
