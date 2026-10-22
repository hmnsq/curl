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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "http_httpsig.h"
#अन्तर्भाव "curl_ed25519.h"
#अन्तर्भाव "curl_hmac.h"
#अन्तर्भाव "curl_sha256.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "slist.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "strcase.h"

#अन्तर्भाव <time.h>

#define HTTPSIG_MAX_SIG_BASE   CURL_MAX_HTTP_HEADER
#define HTTPSIG_MAX_COMPONENTS 16
#define HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#define HTTPSIG_DEFAULT_LABEL  "sig1"

गणना httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

स्थिर अचर वर्ण *alg_to_str(गणना httpsig_alg alg)
{
  विकल्प(alg) {
  पक्ष HTTPSIG_ALG_ED25519:
    फर्काऊ "ed25519";
  पक्ष HTTPSIG_ALG_HMAC_SHA256:
    फर्काऊ "hmac-sha256";
  स्वभाव:
    रोक;
  }
  फर्काऊ रिक्त;
}

स्थिर गणना httpsig_alg id_to_alg(uint8_t val)
{
  विकल्प(val) {
  पक्ष CURLHTTPSIG_ED25519:
    फर्काऊ HTTPSIG_ALG_ED25519;
  पक्ष CURLHTTPSIG_HMAC_SHA256:
    फर्काऊ HTTPSIG_ALG_HMAC_SHA256;
  स्वभाव:
    रोक;
  }
  फर्काऊ HTTPSIG_ALG_UNKNOWN;
}

स्थिर CURLcode decode_hex_साँचो(संरचना Curl_easy *data,
                               अचर वर्ण *hexstr,
                               अचिन्हित वर्ण **keyout,
                               माप_प्रकार *keylen)
{
  माप_प्रकार len, i;
  अचिन्हित वर्ण *keybuf;

  *keyout = रिक्त;
  *keylen = 0;

  len = म_माप(hexstr);
  जबसम्म(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  यदि(len == 0 || (len & 1) != 0) {
    failf(data, "httpsig: invalid hex key (length %zu)", len);
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  यदि(len > CURL_MAX_आगत_LENGTH) {
    failf(data, "httpsig: hex key too long");
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  keybuf = curlx_malloc(len / 2);
  यदि(!keybuf)
    फर्काऊ CURLE_OUT_OF_MEMORY;

  लागि(i = 0; i < len; i += 2) {
    यदि(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(data, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    keybuf[i / 2] = (अचिन्हित वर्ण)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  फर्काऊ CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
स्थिर CURLcode httpsig_authority(संरचना Curl_easy *data,
                                  संरचना connectdata *conn,
                                  संरचना dynbuf *authority_buf)
{
  अचर वर्ण *h = data->state.http_host;

  यदि(h && curl_strnequal(h, "host:", 5)) {
    अचर वर्ण *मान = h + 5;
    अचर वर्ण *end;

    जबसम्म(ISBLANK(*मान))
      मान++;
    यदि(*मान) {
      CURLcode result;

      end = मान;
      जबसम्म(*end && !ISNEWLINE(*end))
        end++;
      जबसम्म(end > मान && ISBLANK(end[-1]))
        end--;
      result = curlx_dyn_addn(authority_buf, मान, (माप_प्रकार)(end - मान));
      यदि(result)
        फर्काऊ result;
      फर्काऊ CURLE_OK;
    }
  }

  {
    अचर वर्ण *hostname = conn->origin->hostname;
    uint16_t port = conn->origin->port;

    यदि((conn->origin->scheme->defport != port) && port)
      फर्काऊ curlx_dyn_addf(authority_buf, "%s:%u", hostname, port);
    फर्काऊ curlx_dyn_add(authority_buf, hostname);
  }
}

स्थिर CURLcode sf_append_quoted(संरचना dynbuf *buf, अचर वर्ण *str)
{
  CURLcode result = curlx_dyn_addn(buf, "\"", 1);
  यदि(result)
    फर्काऊ result;
  जबसम्म(*str) {
    यदि(ISCNTRL(*str))
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    यदि(*str == '\\' || *str == '"') {
      result = curlx_dyn_addn(buf, "\\", 1);
      यदि(result)
        फर्काऊ result;
    }
    result = curlx_dyn_addn(buf, str, 1);
    यदि(result)
      फर्काऊ result;
    str++;
  }
  फर्काऊ curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
स्थिर CURLcode sf_encode_byte_seq(अचर अचिन्हित वर्ण *raw, माप_प्रकार rawlen,
                                   संरचना dynbuf *out)
{
  CURLcode result;
  माप_प्रकार b64len;
  वर्ण *b64;

  result = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  यदि(result)
    फर्काऊ result;

  result = curlx_dyn_addn(out, ":", 1);
  यदि(!result)
    result = curlx_dyn_addn(out, b64, b64len);
  यदि(!result)
    result = curlx_dyn_addn(out, ":", 1);

  curlx_free(b64);
  फर्काऊ result;
}

स्थिर CURLcode build_sig_params(संरचना dynbuf *params,
                                 अचर वर्ण **components, माप_प्रकार गन्ती,
                                 समय_प्रकार created, अचर वर्ण *keyid,
                                 गणना httpsig_alg alg)
{
  CURLcode result;
  माप_प्रकार i;

  result = curlx_dyn_addn(params, "(", 1);
  यदि(result)
    फर्काऊ result;

  लागि(i = 0; i < गन्ती; i++) {
    यदि(i > 0) {
      result = curlx_dyn_addn(params, " ", 1);
      यदि(result)
        फर्काऊ result;
    }
    result = sf_append_quoted(params, components[i]);
    यदि(result)
      फर्काऊ result;
  }

  result = curlx_dyn_addn(params, ")", 1);
  यदि(result)
    फर्काऊ result;

  result = curlx_dyn_addf(params, ";created=%lld", (दीर्घ दीर्घ)created);
  यदि(result)
    फर्काऊ result;

  यदि(keyid && *keyid) {
    result = curlx_dyn_add(params, ";keyid=");
    यदि(result)
      फर्काऊ result;
    result = sf_append_quoted(params, keyid);
    यदि(result)
      फर्काऊ result;
  }

  result = curlx_dyn_addf(params, ";alg=\"%s\"", alg_to_str(alg));
  फर्काऊ result;
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
स्थिर CURLcode resolve_component(अचर वर्ण *name,
                                  अचर वर्ण *method,
                                  अचर वर्ण *authority,
                                  अचर वर्ण *path,
                                  अचर वर्ण *query,
                                  संरचना Curl_easy *data,
                                  संरचना dynbuf *valbuf,
                                  अचर वर्ण **out)
{
  *out = रिक्त;

  यदि(name[0] == '@') {
    यदि(curl_strequal(name, SIG_METHOD))
      *out = method;
    अन्यथा यदि(curl_strequal(name, SIG_AUTHORITY))
      *out = authority;
    अन्यथा यदि(curl_strequal(name, SIG_PATH))
      *out = path;
    अन्यथा यदि(curl_strequal(name, SIG_QUERY))
      *out = query;
    अन्यथा {
      failf(data, "httpsig: unsupported derived component '%s'", name);
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    यदि(!*out) {
      failf(data, "httpsig: derived component '%s' has no value", name);
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    फर्काऊ CURLE_OK;
  }
  अन्यथा {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    संरचना curl_slist *head;
    माप_प्रकार namelen = म_माप(name);
    bool found = FALSE;

    curlx_dyn_reset(valbuf);

    लागि(head = data->set.headers; head; head = head->next) {
      यदि(curl_strnequal(head->data, name, namelen) &&
         Curl_headersep(head->data[namelen])) {
        अचर वर्ण *p = म_अक्षर(head->data, ':');
        यदि(p) {
          CURLcode result;
          संरचना Curl_str content;
          curlx_str_assign(&content, p + 1, म_माप(p + 1));
          curlx_str_trimblanks(&content);
          यदि(found) {
            result = curlx_dyn_addn(valbuf, ", ", 2);
            यदि(result)
              फर्काऊ result;
          }
          result = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          यदि(result)
            फर्काऊ result;
          found = TRUE;
        }
      }
    }

    यदि(found) {
      *out = curlx_dyn_ptr(valbuf);
      फर्काऊ CURLE_OK;
    }
    failf(data, "httpsig: header '%s' not found in request", name);
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }
}

स्थिर CURLcode build_sig_base(संरचना dynbuf *base,
                               अचर वर्ण **components, माप_प्रकार गन्ती,
                               अचर वर्ण *method,
                               अचर वर्ण *authority,
                               अचर वर्ण *path,
                               अचर वर्ण *query,
                               संरचना Curl_easy *data,
                               अचर वर्ण *sig_params)
{
  CURLcode result;
  माप_प्रकार i;
  संरचना dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_HEADER);

  लागि(i = 0; i < गन्ती; i++) {
    अचर वर्ण *val = रिक्त;
    result = resolve_component(components[i], method,
                               authority, path, query, data,
                               &hdrvalbuf, &val);
    यदि(result || !val) {
      failf(data, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      फर्काऊ result ? result : CURLE_BAD_FUNCTION_ARGUMENT;
    }

    result = curlx_dyn_addf(base, "\"%s\": %s\n", components[i], val);
    यदि(result) {
      curlx_dyn_free(&hdrvalbuf);
      फर्काऊ result;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  result = curlx_dyn_addf(base, "\"@signature-params\": %s", sig_params);
  फर्काऊ result;
}

स्थिर CURLcode parse_components(संरचना Curl_easy *data,
                                 अचर वर्ण *query,
                                 अचर वर्ण **components,
                                 माप_प्रकार *ncomp_out,
                                 वर्ण **hdrs_copy_out)
{
  अचर वर्ण *hdrs = CURL_EASY_STR(data, STRING_HTTPSIG_HEADERS);
  माप_प्रकार ncomp = 0;

  *hdrs_copy_out = रिक्त;
  यदि(hdrs && *hdrs) {
    वर्ण *p;
    वर्ण *hdrs_copy = curlx_strdup(hdrs);
    यदि(!hdrs_copy)
      फर्काऊ CURLE_OUT_OF_MEMORY;
    *hdrs_copy_out = hdrs_copy;
    p = hdrs_copy;
    जबसम्म(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      वर्ण *start;
      माप_प्रकार tlen = 0;
      अचर वर्ण *p2 = p;

      curlx_str_passblanks(&p2);
      यदि(!*p2)
        रोक;
      p = start = CURL_UNCONST(p2);
      जबसम्म(*p && !ISBLANK(*p)) {
        यदि((*p == '\"') || (*p == '\\'))
          फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
        p++;
        tlen++;
      }
      यदि(*p)
        *p++ = '\0';

      यदि(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        यदि(!tlen) {
          failf(data, "httpsig: empty header component name");
          फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      अन्यथा {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        यदि(!म_भेद(start, "method"))
          components[ncomp++] = SIG_METHOD;
        अन्यथा यदि(!म_भेद(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        अन्यथा यदि(!म_भेद(start, "path"))
          components[ncomp++] = SIG_PATH;
        अन्यथा यदि(!म_भेद(start, "query"))
          components[ncomp++] = SIG_QUERY;
        अन्यथा {
          failf(data, "httpsig: unknown component '%s'", start);
          फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
        }
      }
    }
    यदि(!ncomp) {
      failf(data, "httpsig: no signature components specified");
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    यदि(*p) {
      failf(data, "httpsig: too many signature components (max %u)",
            (अचिन्हित पूर्णाङ्क)HTTPSIG_MAX_COMPONENTS);
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      माप_प्रकार i, j;

      लागि(i = 0; i < ncomp; i++) {
        लागि(j = i + 1; j < ncomp; j++) {
          यदि(!म_भेद(components[i], components[j])) {
            failf(data, "httpsig: duplicate signature component '%s'",
                  components[i]);
            फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
          }
        }
      }
    }
  }
  अन्यथा {
    components[ncomp++] = SIG_METHOD;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_PATH;
    यदि(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_out = ncomp;
  फर्काऊ CURLE_OK;
}

स्थिर समय_प्रकार httpsig_get_created(शून्य)
{
#ifdef DEBUGBUILD
  वर्ण *force = दो_पर्या("CURL_FORCETIME");
  यदि(force && *force) {
    वर्ण *sigts = दो_पर्या("CURL_HTTPSIG_CREATED");
    यदि(sigts && *sigts) {
      अचर वर्ण *p = sigts;
      curl_off_t अङ्क;
      यदि(!curlx_str_सङ्ख्या(&p, &अङ्क, CURL_OFF_T_MAX))
        फर्काऊ (समय_प्रकार)अङ्क;
    }
    फर्काऊ 0;
  }
#endif
  फर्काऊ समय(रिक्त);
}

स्थिर CURLcode httpsig_sign_base(संरचना Curl_easy *data,
                                  गणना httpsig_alg alg,
                                  अचर अचिन्हित वर्ण *keybuf,
                                  माप_प्रकार keylen,
                                  अचर संरचना dynbuf *sig_base,
                                  अचिन्हित वर्ण *raw_sig,
                                  माप_प्रकार *raw_sig_len)
{
  CURLcode result;

  विकल्प(alg) {
  पक्ष HTTPSIG_ALG_ED25519:
    result = Curl_ed25519_sign(
      keybuf, keylen,
      (अचर अचिन्हित वर्ण *)curlx_dyn_ptr(sig_base),
      curlx_dyn_len(sig_base),
      raw_sig, raw_sig_len);
    रोक;
  पक्ष HTTPSIG_ALG_HMAC_SHA256:
    result = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (अचर अचिन्हित वर्ण *)curlx_dyn_ptr(sig_base),
                         curlx_dyn_len(sig_base), raw_sig);
    यदि(!result)
      *raw_sig_len = CURL_SHA256_DIGEST_LENGTH;
    रोक;
  स्वभाव:
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    रोक;
  }

  यदि(result && result == CURLE_NOT_BUILT_IN) {
    failf(data, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_to_str(alg));
  }
  फर्काऊ result;
}

CURLcode Curl_output_httpsig(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  संरचना connectdata *conn = data->conn;
  अचर वर्ण *path;
  अचर वर्ण *query;
  Curl_HttpReq httpreq;
  अचर वर्ण *method = रिक्त;
  अचर वर्ण *hexkey = CURL_EASY_STR(data, STRING_HTTPSIG_साँचो);
  अचर वर्ण *keyid = CURL_EASY_STR(data, STRING_HTTPSIG_KEYID);
  गणना httpsig_alg alg;
  समय_प्रकार created;
  संरचना dynbuf sig_params;
  संरचना dynbuf sig_base;
  संरचना dynbuf sig_hdr;
  संरचना dynbuf आगत_hdr;
  संरचना dynbuf authority_buf;
  अचर वर्ण *authority;
  अचर वर्ण *components[HTTPSIG_MAX_COMPONENTS];
  माप_प्रकार ncomp = 0;
  अचिन्हित वर्ण *keybuf = रिक्त;
  माप_प्रकार keylen = 0;
  अचिन्हित वर्ण raw_sig[HTTPSIG_MAX_RAW_SIG];
  माप_प्रकार raw_sig_len = 0;
  वर्ण *auth_headers = रिक्त;
  वर्ण *hdrs_copy = रिक्त;
  संरचना dynbuf query_dyn;

  alg = id_to_alg(data->set.httpsig_algorithm);
  यदि(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  यदि(!hexkey || !*hexkey) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  यदि(!keyid || !*keyid) {
    failf(data, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&sig_base, HTTPSIG_MAX_SIG_BASE);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&आगत_hdr, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_HEADER);

  यदि(Curl_checkheaders(data, STRCONST("Signature")) ||
     Curl_checkheaders(data, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    गच्छ done;
  }

  result = decode_hex_साँचो(data, hexkey, &keybuf, &keylen);
  यदि(result)
    गच्छ fail;

  यदि(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(data, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ fail;
  }

  Curl_http_method(data, &method, &httpreq);

  path = data->state.up.path;
  यदि(!path || !*path)
    path = "/";

  query = data->state.up.query;

  result = httpsig_authority(data, conn, &authority_buf);
  यदि(result)
    गच्छ fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  यदि(query && *query)
    result = curlx_dyn_addf(&query_dyn, "?%s", query);
  अन्यथा
    result = curlx_dyn_add(&query_dyn, "?");
  यदि(result)
    गच्छ fail;

  result = parse_components(data, query, components, &ncomp, &hdrs_copy);
  यदि(result)
    गच्छ fail;

  created = httpsig_get_created();

  result = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  यदि(result)
    गच्छ fail;

  infof(data, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  result = build_sig_base(&sig_base, components, ncomp,
                          method, authority, path,
                          curlx_dyn_ptr(&query_dyn),
                          data, curlx_dyn_ptr(&sig_params));
  यदि(result)
    गच्छ fail;

  infof(data, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_base));

  result = httpsig_sign_base(data, alg, keybuf, keylen, &sig_base,
                             raw_sig, &raw_sig_len);
  यदि(result)
    गच्छ fail;

  result = curlx_dyn_add(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  यदि(result)
    गच्छ fail;
  result = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  यदि(result)
    गच्छ fail;

  result = curlx_dyn_addf(&आगत_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  यदि(result)
    गच्छ fail;

  auth_headers = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&आगत_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  यदि(!auth_headers)
    गच्छ fail;

  infof(data, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&आगत_hdr));
  infof(data, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
done:
  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  यदि(keybuf) {
    स_रखो(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  स_रखो(raw_sig, 0, परिमाण(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_base);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&आगत_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  फर्काऊ result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
