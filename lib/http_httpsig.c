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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "http_httpsig.h"
#અન્તર્ભાવ "curl_ed25519.h"
#અન્તર્ભાવ "curl_hmac.h"
#અન્તર્ભાવ "curl_sha256.h"
#અન્તર્ભાવ "http.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "slist.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "curlx/base64.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "strcase.h"

#અન્તર્ભાવ <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

ગણના httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

સ્થિર અચલ અક્ષર *alg_to_str(ગણના httpsig_alg alg)
{
  પસંદગી(alg) {
  સ્થિતિ HTTPSIG_ALG_ED25519:
    પાછા "ed25519";
  સ્થિતિ HTTPSIG_ALG_HMAC_SHA256:
    પાછા "hmac-sha256";
  મૂળભૂત:
    તોડો;
  }
  પાછા ખાલી;
}

સ્થિર ગણના httpsig_alg id_to_alg(uint8_t val)
{
  પસંદગી(val) {
  સ્થિતિ CURLHTTPSIG_ED25519:
    પાછા HTTPSIG_ALG_ED25519;
  સ્થિતિ CURLHTTPSIG_HMAC_SHA256:
    પાછા HTTPSIG_ALG_HMAC_SHA256;
  મૂળભૂત:
    તોડો;
  }
  પાછા HTTPSIG_ALG_UNKNOWN;
}

સ્થિર CURLcode decode_hex_ચાવી(સંરચના Curl_easy *data,
                               અચલ અક્ષર *hexstr,
                               unsigned અક્ષર **keyout,
                               size_t *keylen)
{
  size_t len, i;
  unsigned અક્ષર *keybuf;

  *keyout = ખાલી;
  *keylen = 0;

  len = strlen(hexstr);
  જ્યારે(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  જો(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  જો(len > CURL_MAX_આગત_LENGTH) {
    failf(data, "httpsig: hex key too long");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  જો(!keybuf)
    પાછા CURLE_OUT_OF_MEMORY;

  માટે(i = 0; i < len; i += 2) {
    જો(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (unsigned અક્ષર)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  પાછા CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
સ્થિર CURLcode httpsig_authority(સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn,
                                  સંરચના dynbuf *authority_buf)
{
  અચલ અક્ષર *h = data->state.http_host;

  જો(h && curl_strnequal(h, "host:", 5)) {
    અચલ અક્ષર *કિંમત = h + 5;
    અચલ અક્ષર *end;

    જ્યારે(ISBLANK(*કિંમત))
      કિંમત++;
    જો(*કિંમત) {
      CURLcode result;

      end = કિંમત;
      જ્યારે(*end && !ISNEWLINE(*end))
        end++;
      જ્યારે(end > કિંમત && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, કિંમત, (size_t)(end - કિંમત));
      જો(result)
        પાછા result;
      પાછા CURLE_OK;
    }
  }

  {
    અચલ અક્ષર *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    જો((conn->origin->scheme->defport != port) && port)
      પાછા curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    પાછા curlx_dyn_add(authority_buf, hostname);
  }
}

સ્થિર CURLcode sf_append_quoted(સંરચના dynbuf *buf, અચલ અક્ષર *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  જો(result)
    પાછા result;
  જ્યારે(*str) {
    જો(ISCNTRL(*str))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    જો(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      જો(result)
        પાછા result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    જો(result)
      પાછા result;
    str++;
  }
  પાછા curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
સ્થિર CURLcode sf_encode_byte_seq(અચલ unsigned અક્ષર *raw, size_t rawlen,
                                   સંરચના dynbuf *out)
{
  CURLcode result;
  size_t b64len;
  અક્ષર *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  જો(result)
    પાછા result;

  result = curlx_dyn_addn(out, ":", 1);
  જો(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  જો(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  પાછા result;
}

સ્થિર CURLcode build_sig_params(સંરચના dynbuf *params,
                                 અચલ અક્ષર **components, size_t ગણતરી,
                                 time_t created, અચલ અક્ષર *keyid,
                                 ગણના httpsig_alg alg)
{
  CURLcode result;
  size_t i;

  result = curlx_dyn_addn(params, "(", 1);
  જો(result)
    પાછા result;

  માટે(i = 0; i < ગણતરી; i++) {
    જો(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      જો(result)
        પાછા result;
    }
    result = sf_append_quoted(params, components[i]);
    જો(result)
      પાછા result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  જો(result)
    પાછા result;

  result = curlx_dyn_addf(params, ";created=%lld", (long long)created);
  જો(result)
    પાછા result;

  જો(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    જો(result)
      પાછા result;
    result = sf_append_quoted(params, keyid);
    જો(result)
      પાછા result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  પાછા result;
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
સ્થિર CURLcode resolve_component(અચલ અક્ષર *name,
                                  અચલ અક્ષર *method,
                                  અચલ અક્ષર *authority,
                                  અચલ અક્ષર *path,
                                  અચલ અક્ષર *query,
                                  સંરચના Curl_easy *data,
                                  સંરચના dynbuf *valbuf,
                                  અચલ અક્ષર **out)
{
  *out = ખાલી;

  જો(name[0] == '@') {
    જો(curl_strequal(name, SIG_METHOD))
      *out = method;
    નહીંતર જો(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    નહીંતર જો(curl_strequal(name, SIG_PATH))
      *out = path;
    નહીંતર જો(curl_strequal(name, SIG_QUERY))
      *out = query;
    નહીંતર {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }
    જો(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }
    પાછા CURLE_OK;
  }
  નહીંતર {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    સંરચના curl_slist *head;
    size_t namelen = strlen(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    માટે(head = data->set.headers; head; head = head->next) {
      જો(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        અચલ અક્ષર *p = strchr(head->data, ':');
        જો(p) {
          CURLcode result;
          સંરચના Curl_str content;
          curlx_str_assign(&content, p + 1, strlen(p + 1));
          curlx_str_trimblanks(&content);
          જો(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            જો(result)
              પાછા result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          જો(result)
            પાછા result;
          found = TRUE;
        }
      }
    }

    જો(found) {
      *out = curlx_dyn_ptr(valbuf);
      પાછા CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

સ્થિર CURLcode build_sig_base(સંરચના dynbuf *base,
                               અચલ અક્ષર **components, size_t ગણતરી,
                               અચલ અક્ષર *method,
                               અચલ અક્ષર *authority,
                               અચલ અક્ષર *path,
                               અચલ અક્ષર *query,
                               સંરચના Curl_easy *data,
                               અચલ અક્ષર *sig_params)
{
  CURLcode result;
  size_t i;
  સંરચના dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  માટે(i = 0; i < ગણતરી; i++) {
    અચલ અક્ષર *val = ખાલી;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    જો(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      પાછા result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    જો(result) {
      curlx_dyn_free(&hdrvalbuf);
      પાછા result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  પાછા result;
}

સ્થિર CURLcode parse_components(સંરચના Curl_easy *data,
                                 અચલ અક્ષર *query,
                                 અચલ અક્ષર **components,
                                 size_t *ncomp_out,
                                 અક્ષર **hdrs_copy_out)
{
  અચલ અક્ષર *hdrs = CURL_EASY_STR(data, STRING_HTTPSIG_HEADERS);
  size_t ncomp = 0;

  *hdrs_copy_out = ખાલી;
  જો(hdrs && *hdrs) {
    અક્ષર *p;
    અક્ષર *hdrs_copy = curlx_strdup(hdrs);
    જો(!hdrs_copy)
      પાછા CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    જ્યારે(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      અક્ષર *start;
      size_t tlen = 0;
      અચલ અક્ષર *p2 = p;

      curlx_str_passblanks(&p2);
      જો(!*p2)
        તોડો;
      p = start = CURL_UNCONST(p2);
      જ્યારે(*p && !ISBLANK(*p)) {
        જો((*p == '\"') || (*p == '\\'))
          પાછા CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      જો(*p)
        *p++ = '\0';

      જો(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        જો(!tlen) {
          failf(data, "httpsig: empty header component name");
          પાછા CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      નહીંતર {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        જો(!strcmp(start, "method"))
          components[ncomp++] = SIG_METHOD;
        નહીંતર જો(!strcmp(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        નહીંતર જો(!strcmp(start, "path"))
          components[ncomp++] = SIG_PATH;
        નહીંતર જો(!strcmp(start, "query"))
          components[ncomp++] = SIG_QUERY;
        નહીંતર {
          failf(data, "httpsig: unknown component '%s'", start);
          પાછા CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    જો(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }
    જો(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (unsigned પૂર્ણાંક)HTTPSIG_MAX_COMPONENTS);
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      માટે(i = 0; i < ncomp; i++) {
        માટે(j = i + 1; j < ncomp; j++) {
          જો(!strcmp(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            પાછા CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  નહીંતર {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    જો(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  પાછા CURLE_OK;
}

સ્થિર time_t httpsig_get_created(શૂન્ય)
{
#ifdef DEBUGBUILD
  અક્ષર *force = getenv("CURL_FORCETIME");
  જો(force && *force) {
    અક્ષર *sigts = getenv("CURL_HTTPSIG_CREATED");
    જો(sigts && *sigts) {
      અચલ અક્ષર *p = sigts;
      curl_off_t અંક;
      જો(!curlx_str_સંખ્યા(&p, &અંક, CURL_OFF_T_MAX))
        પાછા (time_t)અંક;
    }
    પાછા 0;
  }
#endif
  પાછા time(ખાલી);
}

સ્થિર CURLcode httpsig_sign_base(સંરચના Curl_easy *data,
                                  ગણના httpsig_alg alg,
                                  અચલ unsigned અક્ષર *keybuf,
                                  size_t keylen,
                                  અચલ સંરચના dynbuf *sig_base,
                                  unsigned અક્ષર *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode result;

  પસંદગી(alg) {
  સ્થિતિ HTTPSIG_ALG_ED25519:
    result = Curl_ed25519_sign(
      keybuf, keylen,
      (અચલ unsigned અક્ષર *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    તોડો;
  સ્થિતિ HTTPSIG_ALG_HMAC_SHA256:
    result = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (અચલ unsigned અક્ષર *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    જો(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    તોડો;
  મૂળભૂત:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    તોડો;
  }

  જો(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  પાછા result;
}

CURLcode Curl_output_httpsig(સંરચના Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  સંરચના connectdata *conn = data->conn;
  અચલ અક્ષર *path;
  અચલ અક્ષર *query;
  Curl_HttpReq httpreq;
  અચલ અક્ષર *method = ખાલી;
  અચલ અક્ષર *hexkey = CURL_EASY_STR(data, STRING_HTTPSIG_ચાવી);
  અચલ અક્ષર *keyid = CURL_EASY_STR(data, STRING_HTTPSIG_KEYID);
  ગણના httpsig_alg alg;
  time_t created;
  સંરચના dynbuf sig_params;
  સંરચના dynbuf sig_base;
  સંરચના dynbuf sig_hdr;
  સંરચના dynbuf આગત_hdr;
  સંરચના dynbuf authority_buf;
  અચલ અક્ષર *authority;
  અચલ અક્ષર *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  unsigned અક્ષર *keybuf = ખાલી;
  size_t keylen = 0;
  unsigned અક્ષર raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  અક્ષર *auth_headers = ખાલી;
  અક્ષર *hdrs_copy = ખાલી;
  સંરચના dynbuf query_dyn;

  alg = id_to_alg(data->set.httpsig_algorithm);
  જો(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  જો(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }
  જો(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&આગત_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  જો(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    જાઓ done;
  }

  result = decode_hex_ચાવી(data, hexkey, &keybuf, &keylen);
  જો(result)
    જાઓ fail;

  જો(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    જાઓ fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  જો(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  જો(result)
    જાઓ fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  જો(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  નહીંતર
    result = curlx_dyn_add(&query_dyn, "?");
  જો(result)
    જાઓ fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  જો(result)
    જાઓ fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  જો(result)
    જાઓ fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  જો(result)
    જાઓ fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  જો(result)
    જાઓ fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  જો(result)
    જાઓ fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  જો(result)
    જાઓ fail;

  result = curlx_dyn_addf(&આગત_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  જો(result)
    જાઓ fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&આગત_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  જો(!auth_headers)
    જાઓ fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&આગત_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  જો(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, કદ(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&આગત_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  પાછા result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
