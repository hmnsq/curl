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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#समावेश "urldata.h"
#समावेश "http_httpsig.h"
#समावेश "curl_ed25519.h"
#समावेश "curl_hmac.h"
#समावेश "curl_sha256.h"
#समावेश "http.h"
#समावेश "transfer.h"
#समावेश "curl_trc.h"
#समावेश "slist.h"
#समावेश "curlx/dynbuf.h"
#समावेश "curlx/base64.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/strparse.h"
#समावेश "strcase.h"

#समावेश <time.h>

#घोषणा HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#घोषणा HTTPSIG_MAX_COMPONENTS 16
#घोषणा HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#घोषणा HTTPSIG_DEFAULT_LABEL  "sig1"

क्रमागत httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

अटल स्थिर अक्षर *alg_to_str(क्रमागत httpsig_alg alg)
{
  चयन(alg) {
  हाल HTTPSIG_ALG_ED25519:
    वापस "ed25519";
  हाल HTTPSIG_ALG_HMAC_SHA256:
    वापस "hmac-sha256";
  शेष:
    अवरोध;
  }
  वापस शून्य;
}

अटल क्रमागत httpsig_alg id_to_alg(uint8_t val)
{
  चयन(val) {
  हाल CURLHTTPSIG_ED25519:
    वापस HTTPSIG_ALG_ED25519;
  हाल CURLHTTPSIG_HMAC_SHA256:
    वापस HTTPSIG_ALG_HMAC_SHA256;
  शेष:
    अवरोध;
  }
  वापस HTTPSIG_ALG_UNKNOWN;
}

अटल CURLcode decode_hex_कुंजी(काष्ठा Curl_easy *तथ्य,
                               स्थिर अक्षर *hexstr,
                               अचिन्हित अक्षर **keyout,
                               माप_प्रकार *keylen)
{
  माप_प्रकार len, i;
  अचिन्हित अक्षर *keybuf;

  *keyout = शून्य;
  *keylen = 0;

  len = म_माप(hexstr);
  जबतक(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  अगर(len == 0 || (len & 1) != 0) {
    failf(तथ्य, "httpsig: invalid hex key (length %zu)", len);
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  अगर(len > CURL_MAX_निवेश_लंबाई) {
    failf(तथ्य, "httpsig: hex key too long");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  अगर(!keybuf)
    वापस CURLE_OUT_OF_MEMORY;

  क्रम(i = 0; i < len; i += 2) {
    अगर(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(तथ्य, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (अचिन्हित अक्षर)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  वापस CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
अटल CURLcode httpsig_authority(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn,
                                  काष्ठा dynbuf *authority_buf)
{
  स्थिर अक्षर *h = तथ्य->state.http_host;

  अगर(h && curl_strnequal(h, "host:", 5)) {
    स्थिर अक्षर *मूल्य = h + 5;
    स्थिर अक्षर *end;

    जबतक(ISBLANK(*मूल्य))
      मूल्य++;
    अगर(*मूल्य) {
      CURLcode परिणाम;

      end = मूल्य;
      जबतक(*end && !ISNEWLINE(*end))
        end++;
      जबतक(end > मूल्य && ISBLANK(end[-1]))
        end--;
      परिणाम = curlx_dyn_addn(authority_buf, मूल्य, (माप_प्रकार)(end - मूल्य));
      अगर(परिणाम)
        वापस परिणाम;
      वापस CURLE_OK;
    }
  }

  {
    स्थिर अक्षर *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    अगर((conn->origin->scheme->defport != port) && port)
      वापस curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    वापस curlx_dyn_add(authority_buf, hostname);
  }
}

अटल CURLcode sf_append_quoted(काष्ठा dynbuf *buf, स्थिर अक्षर *str)
{
  CURLcode परिणाम = curlx_dyn_addn(buf, "\"", 1);
  अगर(परिणाम)
    वापस परिणाम;
  जबतक(*str) {
    अगर(ISCNTRL(*str))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    अगर(*str == '\\' || *str == '"') {
      परिणाम = curlx_dyn_addn(buf, "\\", 1);
      अगर(परिणाम)
        वापस परिणाम;
    }
    परिणाम = curlx_dyn_addn(buf, str, 1);
    अगर(परिणाम)
      वापस परिणाम;
    str++;
  }
  वापस curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
अटल CURLcode sf_encode_byte_seq(स्थिर अचिन्हित अक्षर *raw, माप_प्रकार rawlen,
                                   काष्ठा dynbuf *out)
{
  CURLcode परिणाम;
  माप_प्रकार b64len;
  अक्षर *b64;

  परिणाम = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = curlx_dyn_addn(out, ":", 1);
  अगर(!परिणाम)
    परिणाम = curlx_dyn_addn(out, b64, b64len);
  अगर(!परिणाम)
    परिणाम = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  वापस परिणाम;
}

अटल CURLcode build_sig_params(काष्ठा dynbuf *params,
                                 स्थिर अक्षर **components, माप_प्रकार गिनती,
                                 समय_प्रकार created, स्थिर अक्षर *keyid,
                                 क्रमागत httpsig_alg alg)
{
  CURLcode परिणाम;
  माप_प्रकार i;

  परिणाम = curlx_dyn_addn(params, "(", 1);
  अगर(परिणाम)
    वापस परिणाम;

  क्रम(i = 0; i < गिनती; i++) {
    अगर(i > 0) {
      परिणाम = curlx_dyn_addn(params, " ", 1);
      अगर(परिणाम)
        वापस परिणाम;
    }
    परिणाम = sf_append_quoted(params, components[i]);
    अगर(परिणाम)
      वापस परिणाम;
  }

  परिणाम = curlx_dyn_addn(params, ")", 1);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = curlx_dyn_addf(params, ";created=%lld", (दीर्घ दीर्घ)created);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(keyid && *keyid) {
    परिणाम = curlx_dyn_add(params, ";keyid=");
    अगर(परिणाम)
      वापस परिणाम;
    परिणाम = sf_append_quoted(params, keyid);
    अगर(परिणाम)
      वापस परिणाम;
  }

  परिणाम = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  वापस परिणाम;
}

/* strings defined by RFC 9421 */
#घोषणा SIG_METHOD    "@method"
#घोषणा SIG_AUTHORITY "@authority"
#घोषणा SIG_PATH      "@path"
#घोषणा SIG_QUERY     "@query"

/* Resolve a component identifier to its value.
 * For headers, we walk the full user-supplied header list to combine
 * duplicate field values with ", " per RFC 9421 Section 2.1. Each
 * individual value is trimmed of leading/trailing OWS and the trailing
 * \r\n. The combined result is written into the caller-provided buffer. */
अटल CURLcode resolve_component(स्थिर अक्षर *name,
                                  स्थिर अक्षर *method,
                                  स्थिर अक्षर *authority,
                                  स्थिर अक्षर *path,
                                  स्थिर अक्षर *query,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा dynbuf *valbuf,
                                  स्थिर अक्षर **out)
{
  *out = शून्य;

  अगर(name[0] == '@') {
    अगर(curl_strequal(name, SIG_METHOD))
      *out = method;
    अन्यथा अगर(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    अन्यथा अगर(curl_strequal(name, SIG_PATH))
      *out = path;
    अन्यथा अगर(curl_strequal(name, SIG_QUERY))
      *out = query;
    अन्यथा {
      failf(तथ्य, "httpsig: unsupported derived component '%s'", name);
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }
    अगर(!*out) {
      failf(तथ्य, "httpsig: derived component '%s' has no value", name);
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }
    वापस CURLE_OK;
  }
  अन्यथा {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    काष्ठा curl_slist *शीर्ष;
    माप_प्रकार namelen = म_माप(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    क्रम(शीर्ष = तथ्य->set.headers; शीर्ष; शीर्ष = शीर्ष->अग्र) {
      अगर(curl_strnequal(शीर्ष->तथ्य, name, namelen) &&
         Curl_headersep(शीर्ष->तथ्य[namelen])) {
        स्थिर अक्षर *p = म_अक्षर(शीर्ष->तथ्य, ':');
        अगर(p) {
          CURLcode परिणाम;
          काष्ठा Curl_str content;
          curlx_str_assign(&content, p + 1, म_माप(p + 1));
          curlx_str_trimblanks(&content);
          अगर(found) {
            परिणाम = curlx_dyn_addn(valbuf, ", ", 2);
            अगर(परिणाम)
              वापस परिणाम;
          }
          परिणाम = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          अगर(परिणाम)
            वापस परिणाम;
          found = TRUE;
        }
      }
    }

    अगर(found) {
      *out = curlx_dyn_ptr(valbuf);
      वापस CURLE_OK;
    }
    failf(तथ्य, "httpsig: header '%s' not found in request", name);
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

अटल CURLcode build_sig_base(काष्ठा dynbuf *base,
                               स्थिर अक्षर **components, माप_प्रकार गिनती,
                               स्थिर अक्षर *method,
                               स्थिर अक्षर *authority,
                               स्थिर अक्षर *path,
                               स्थिर अक्षर *query,
                               काष्ठा Curl_easy *तथ्य,
                               स्थिर अक्षर *sig_params)
{
  CURLcode परिणाम;
  माप_प्रकार i;
  काष्ठा dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  क्रम(i = 0; i < गिनती; i++) {
    स्थिर अक्षर *val = शून्य;
    परिणाम = resolve_component(components[i], method,
                               authority, path, query, तथ्य,
                               &hdrvalbuf, &val);
    अगर(परिणाम || !val) {
      failf(तथ्य, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      वापस परिणाम ? परिणाम : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    परिणाम = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    अगर(परिणाम) {
      curlx_dyn_free(&hdrvalbuf);
      वापस परिणाम;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  परिणाम = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  वापस परिणाम;
}

अटल CURLcode parse_components(काष्ठा Curl_easy *तथ्य,
                                 स्थिर अक्षर *query,
                                 स्थिर अक्षर **components,
                                 माप_प्रकार *ncomp_out,
                                 अक्षर **hdrs_copy_out)
{
  स्थिर अक्षर *hdrs = CURL_EASY_STR(तथ्य, STRING_HTTPSIG_HEADERS);
  माप_प्रकार ncomp = 0;

  *hdrs_copy_out = शून्य;
  अगर(hdrs && *hdrs) {
    अक्षर *p;
    अक्षर *hdrs_copy = curlx_strdup(hdrs);
    अगर(!hdrs_copy)
      वापस CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    जबतक(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      अक्षर *start;
      माप_प्रकार tlen = 0;
      स्थिर अक्षर *p2 = p;

      curlx_str_passblanks(&p2);
      अगर(!*p2)
        अवरोध;
      p = start = CURL_UNCONST(p2);
      जबतक(*p && !ISBLANK(*p)) {
        अगर((*p == '\"') || (*p == '\\'))
          वापस CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      अगर(*p)
        *p++ = '\0';

      अगर(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        अगर(!tlen) {
          failf(तथ्य, "httpsig: empty header component name");
          वापस CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      अन्यथा {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        अगर(!म_भेद(start, "method"))
          components[ncomp++] = SIG_METHOD;
        अन्यथा अगर(!म_भेद(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        अन्यथा अगर(!म_भेद(start, "path"))
          components[ncomp++] = SIG_PATH;
        अन्यथा अगर(!म_भेद(start, "query"))
          components[ncomp++] = SIG_QUERY;
        अन्यथा {
          failf(तथ्य, "httpsig: unknown component '%s'", start);
          वापस CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    अगर(!ncomp) {
      failf(तथ्य, "httpsig: no signature components specified");
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }
    अगर(*p) {
      failf(तथ्य, "httpsig: too many signature components (max %u)",
            (अचिन्हित पूर्णांक)HTTPSIG_MAX_COMPONENTS);
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      माप_प्रकार i, j;

      क्रम(i = 0; i < ncomp; i++) {
        क्रम(j = i + 1; j < ncomp; j++) {
          अगर(!म_भेद(components[i], components[j])) {
            failf(तथ्य, "httpsig: duplicate signature component '%s'",
                  components[i]);
            वापस CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  अन्यथा {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    अगर(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  वापस CURLE_OK;
}

अटल समय_प्रकार httpsig_get_created(व्योम)
{
#अगर_घोषित DEBUGBUILD
  अक्षर *force = दो_पर्या("CURL_FORCETIME");
  अगर(force && *force) {
    अक्षर *sigts = दो_पर्या("CURL_HTTPSIG_CREATED");
    अगर(sigts && *sigts) {
      स्थिर अक्षर *p = sigts;
      curl_off_t अंक;
      अगर(!curlx_str_संख्या(&p, &अंक, CURL_OFF_T_MAX))
        वापस (समय_प्रकार)अंक;
    }
    वापस 0;
  }
#पूर्ण_अगर
  वापस समय(शून्य);
}

अटल CURLcode httpsig_sign_base(काष्ठा Curl_easy *तथ्य,
                                  क्रमागत httpsig_alg alg,
                                  स्थिर अचिन्हित अक्षर *keybuf,
                                  माप_प्रकार keylen,
                                  स्थिर काष्ठा dynbuf *sig_base,
                                  अचिन्हित अक्षर *raw_sig,
                                  माप_प्रकार *raw_sig_len)
{
  CURLcode परिणाम;

  चयन(alg) {
  हाल HTTPSIG_ALG_ED25519:
    परिणाम = Curl_ed25519_sign(
      keybuf, keylen,
      (स्थिर अचिन्हित अक्षर *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    अवरोध;
  हाल HTTPSIG_ALG_HMAC_SHA256:
    परिणाम = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (स्थिर अचिन्हित अक्षर *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    अगर(!परिणाम)
      *raw_sig_len = CURL_SHA256_DIGEST_लंबाई;
    अवरोध;
  शेष:
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अवरोध;
  }

  अगर(परिणाम && परिणाम == CURLE_NOT_BUILT_IN) {
    failf(तथ्य, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  वापस परिणाम;
}

CURLcode Curl_output_httpsig(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  काष्ठा connectdata *conn = तथ्य->conn;
  स्थिर अक्षर *path;
  स्थिर अक्षर *query;
  Curl_HttpReq httpreq;
  स्थिर अक्षर *method = शून्य;
  स्थिर अक्षर *hexkey = CURL_EASY_STR(तथ्य, STRING_HTTPSIG_कुंजी);
  स्थिर अक्षर *keyid = CURL_EASY_STR(तथ्य, STRING_HTTPSIG_KEYID);
  क्रमागत httpsig_alg alg;
  समय_प्रकार created;
  काष्ठा dynbuf sig_params;
  काष्ठा dynbuf sig_base;
  काष्ठा dynbuf sig_hdr;
  काष्ठा dynbuf निवेश_hdr;
  काष्ठा dynbuf authority_buf;
  स्थिर अक्षर *authority;
  स्थिर अक्षर *components[HTTPSIG_MAX_COMPONENTS];
  माप_प्रकार ncomp = 0;
  अचिन्हित अक्षर *keybuf = शून्य;
  माप_प्रकार keylen = 0;
  अचिन्हित अक्षर raw_sig[HTTPSIG_MAX_RAW_SIG];
  माप_प्रकार raw_sig_len = 0;
  अक्षर *auth_headers = शून्य;
  अक्षर *hdrs_copy = शून्य;
  काष्ठा dynbuf query_dyn;

  alg = id_to_alg(तथ्य->set.httpsig_algorithm);
  अगर(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(तथ्य, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  अगर(!hexkey || !*hexkey) {
    failf(तथ्य, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }
  अगर(!keyid || !*keyid) {
    failf(तथ्य, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&निवेश_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  अगर(Curl_checkheaders(तथ्य, STRCONST("Signature")) ||
     Curl_checkheaders(तथ्य, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    जाओ done;
  }

  परिणाम = decode_hex_कुंजी(तथ्य, hexkey, &keybuf, &keylen);
  अगर(परिणाम)
    जाओ fail;

  अगर(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(तथ्य, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ fail;
  }

  Curl_http_method(तथ्य, &method, &httpreq);

  path = तथ्य->state.up.path;
  अगर(!path || !*path)
    path = "/";

  query = तथ्य->state.up.query;

  परिणाम = httpsig_authority(तथ्य, conn, &authority_buf);
  अगर(परिणाम)
    जाओ fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  अगर(query && *query)
    परिणाम = curlx_dyn_addf(&query_dyn, "?%s", query);
  अन्यथा
    परिणाम = curlx_dyn_add(&query_dyn, "?");
  अगर(परिणाम)
    जाओ fail;

  परिणाम = parse_components(तथ्य, query, components, &ncomp, &hdrs_copy);
  अगर(परिणाम)
    जाओ fail;

  created = httpsig_get_created();

  परिणाम = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  अगर(परिणाम)
    जाओ fail;

  infof(तथ्य, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  परिणाम = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          तथ्य, curlx_dyn_ptr(&sig_params));
  अगर(परिणाम)
    जाओ fail;

  infof(तथ्य, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  परिणाम = httpsig_sign_base(तथ्य, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  अगर(परिणाम)
    जाओ fail;

  परिणाम = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  अगर(परिणाम)
    जाओ fail;
  परिणाम = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  अगर(परिणाम)
    जाओ fail;

  परिणाम = curlx_dyn_addf(&निवेश_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  अगर(परिणाम)
    जाओ fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&निवेश_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  अगर(!auth_headers)
    जाओ fail;

  infof(तथ्य, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&निवेश_hdr));
  infof(तथ्य, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(तथ्य->req.hd_auth);
  तथ्य->req.hd_auth = auth_headers;
  तथ्य->state.authhost.done = TRUE;
  परिणाम = CURLE_OK;

fail:
  अगर(keybuf) {
    स_रखो(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  स_रखो(raw_sig, 0, माप(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&निवेश_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
