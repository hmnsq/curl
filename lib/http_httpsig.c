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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#ضمن "urldata.h"
#ضمن "http_httpsig.h"
#ضمن "curl_ed25519.h"
#ضمن "curl_hmac.h"
#ضمن "curl_sha256.h"
#ضمن "http.h"
#ضمن "transfer.h"
#ضمن "curl_trc.h"
#ضمن "slist.h"
#ضمن "curlx/dynbuf.h"
#ضمن "curlx/base64.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/strparse.h"
#ضمن "strcase.h"

#ضمن <time.h>

#وصف_ڪريو HTTPSIG_MAX_SIG_بنياد   CURL_MAX_HTTP_سرو
#وصف_ڪريو HTTPSIG_MAX_COMPONENTS 16
#وصف_ڪريو HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#وصف_ڪريو HTTPSIG_DEFAULT_LABEL  "sig1"

تعداد httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

ساكن ثابت حرف *alg_منزل_str(تعداد httpsig_alg alg)
{
  بدل(alg) {
  حالة HTTPSIG_ALG_ED25519:
    أرجع "ed25519";
  حالة HTTPSIG_ALG_HMAC_SHA256:
    أرجع "hmac-sha256";
  افتراضي:
    اكسر;
  }
  أرجع عدم;
}

ساكن تعداد httpsig_alg id_منزل_alg(uint8_t val)
{
  بدل(val) {
  حالة CURLHTTPSIG_ED25519:
    أرجع HTTPSIG_ALG_ED25519;
  حالة CURLHTTPSIG_HMAC_SHA256:
    أرجع HTTPSIG_ALG_HMAC_SHA256;
  افتراضي:
    اكسر;
  }
  أرجع HTTPSIG_ALG_UNKNOWN;
}

ساكن CURLcode decode_hex_ڪنجي(بنية Curl_easy *معلومات,
                               ثابت حرف *hexstr,
                               بنانشان حرف **keyout,
                               size_t *keylen)
{
  size_t len, i;
  بنانشان حرف *keybuf;

  *keyout = عدم;
  *keylen = 0;

  len = لڙي_ڊيگهه(hexstr);
  طالما(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  إذا(len == 0 || (len & 1) != 0) {
    failf(معلومات, "httpsig: invalid hex key (length %zu)", len);
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  إذا(len > CURL_MAX_داخلا_ڊيگهائي) {
    failf(معلومات, "httpsig: hex key too long");
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  keybuf = curlx_malloc(len / 2);
  إذا(!keybuf)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  لكل(i = 0; i < len; i += 2) {
    إذا(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(معلومات, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      أرجع CURLE_BAD_FUNCTION_دليل;
    }
    keybuf[i / 2] = (بنانشان حرف)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  أرجع CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ساكن CURLcode httpsig_authority(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn,
                                  بنية dynbuf *authority_buf)
{
  ثابت حرف *h = معلومات->state.http_host;

  إذا(h && curl_strnequal(h, "host:", 5)) {
    ثابت حرف *قدر = h + 5;
    ثابت حرف *پڇاڙي;

    طالما(ISBLANK(*قدر))
      قدر++;
    إذا(*قدر) {
      CURLcode نتيجو;

      پڇاڙي = قدر;
      طالما(*پڇاڙي && !ISNEWLINE(*پڇاڙي))
        پڇاڙي++;
      طالما(پڇاڙي > قدر && ISBLANK(پڇاڙي[-1]))
        پڇاڙي--;
      نتيجو = curlx_dyn_addn(authority_buf, قدر, (size_t)(پڇاڙي - قدر));
      إذا(نتيجو)
        أرجع نتيجو;
      أرجع CURLE_OK;
    }
  }

  {
    ثابت حرف *hostname = conn->origin->hostname;
    uint16_t پورٽ = conn->origin->پورٽ;

    إذا((conn->origin->scheme->defport != پورٽ) && پورٽ)
      أرجع curlx_dyn_addf(authority_buf, "%s:%u", hostname, پورٽ);
    أرجع curlx_dyn_جوڙيو(authority_buf, hostname);
  }
}

ساكن CURLcode sf_append_quoted(بنية dynbuf *buf, ثابت حرف *str)
{
  CURLcode نتيجو = curlx_dyn_addn(buf, "\"", 1);
  إذا(نتيجو)
    أرجع نتيجو;
  طالما(*str) {
    إذا(ISCNTRL(*str))
      أرجع CURLE_BAD_FUNCTION_دليل;
    إذا(*str == '\\' || *str == '"') {
      نتيجو = curlx_dyn_addn(buf, "\\", 1);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    نتيجو = curlx_dyn_addn(buf, str, 1);
    إذا(نتيجو)
      أرجع نتيجو;
    str++;
  }
  أرجع curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ساكن CURLcode sf_encode_byte_seq(ثابت بنانشان حرف *raw, size_t rawlen,
                                   بنية dynbuf *ٻاهريون)
{
  CURLcode نتيجو;
  size_t b64len;
  حرف *b64;

  نتيجو = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = curlx_dyn_addn(ٻاهريون, ":", 1);
  إذا(!نتيجو)
    نتيجو = curlx_dyn_addn(ٻاهريون, b64, b64len);
  إذا(!نتيجو)
    نتيجو = curlx_dyn_addn(ٻاهريون, ":", 1);

  curlx_free(b64);
  أرجع نتيجو;
}

ساكن CURLcode build_sig_params(بنية dynbuf *params,
                                 ثابت حرف **components, size_t ڳڻپ,
                                 time_t created, ثابت حرف *keyid,
                                 تعداد httpsig_alg alg)
{
  CURLcode نتيجو;
  size_t i;

  نتيجو = curlx_dyn_addn(params, "(", 1);
  إذا(نتيجو)
    أرجع نتيجو;

  لكل(i = 0; i < ڳڻپ; i++) {
    إذا(i > 0) {
      نتيجو = curlx_dyn_addn(params, " ", 1);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    نتيجو = sf_append_quoted(params, components[i]);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  نتيجو = curlx_dyn_addn(params, ")", 1);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = curlx_dyn_addf(params, ";created=%lld", (ڊگهو ڊگهو)created);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(keyid && *keyid) {
    نتيجو = curlx_dyn_جوڙيو(params, ";keyid=");
    إذا(نتيجو)
      أرجع نتيجو;
    نتيجو = sf_append_quoted(params, keyid);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  نتيجو = curlx_dyn_addf(params, ";alg=\"%s\"", alg_منزل_str(alg));
  أرجع نتيجو;
}

/* strings defined by RFC 9421 */
#وصف_ڪريو SIG_طريقو    "@method"
#وصف_ڪريو SIG_AUTHORITY "@authority"
#وصف_ڪريو SIG_رستو      "@path"
#وصف_ڪريو SIG_QUERY     "@query"

/* Resolve a component identifier to its value.
 * For headers, we walk the full user-supplied header list to combine
 * duplicate field values with ", " per RFC 9421 Section 2.1. Each
 * individual value is trimmed of leading/trailing OWS and the trailing
 * \r\n. The combined result is written into the caller-provided buffer. */
ساكن CURLcode resolve_component(ثابت حرف *name,
                                  ثابت حرف *طريقو,
                                  ثابت حرف *authority,
                                  ثابت حرف *رستو,
                                  ثابت حرف *query,
                                  بنية Curl_easy *معلومات,
                                  بنية dynbuf *valbuf,
                                  ثابت حرف **ٻاهريون)
{
  *ٻاهريون = عدم;

  إذا(name[0] == '@') {
    إذا(curl_strequal(name, SIG_طريقو))
      *ٻاهريون = طريقو;
    وإلا إذا(curl_strequal(name, SIG_AUTHORITY))
      *ٻاهريون = authority;
    وإلا إذا(curl_strequal(name, SIG_رستو))
      *ٻاهريون = رستو;
    وإلا إذا(curl_strequal(name, SIG_QUERY))
      *ٻاهريون = query;
    وإلا {
      failf(معلومات, "httpsig: unsupported derived component '%s'", name);
      أرجع CURLE_BAD_FUNCTION_دليل;
    }
    إذا(!*ٻاهريون) {
      failf(معلومات, "httpsig: derived component '%s' has no value", name);
      أرجع CURLE_BAD_FUNCTION_دليل;
    }
    أرجع CURLE_OK;
  }
  وإلا {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    بنية curl_slist *سر;
    size_t namelen = لڙي_ڊيگهه(name);
    bool مليو = FALSE;

    curlx_dyn_reset(valbuf);

    لكل(سر = معلومات->سيٽ.سرا; سر; سر = سر->ايندڙ) {
      إذا(curl_strnequal(سر->معلومات, name, namelen) &&
         Curl_headersep(سر->معلومات[namelen])) {
        ثابت حرف *p = strchr(سر->معلومات, ':');
        إذا(p) {
          CURLcode نتيجو;
          بنية Curl_str content;
          curlx_str_سونپيو(&content, p + 1, لڙي_ڊيگهه(p + 1));
          curlx_str_trimblanks(&content);
          إذا(مليو) {
            نتيجو = curlx_dyn_addn(valbuf, ", ", 2);
            إذا(نتيجو)
              أرجع نتيجو;
          }
          نتيجو = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          إذا(نتيجو)
            أرجع نتيجو;
          مليو = TRUE;
        }
      }
    }

    إذا(مليو) {
      *ٻاهريون = curlx_dyn_ptr(valbuf);
      أرجع CURLE_OK;
    }
    failf(معلومات, "httpsig: header '%s' not found in request", name);
    أرجع CURLE_BAD_FUNCTION_دليل;
  }
}

ساكن CURLcode build_sig_بنياد(بنية dynbuf *بنياد,
                               ثابت حرف **components, size_t ڳڻپ,
                               ثابت حرف *طريقو,
                               ثابت حرف *authority,
                               ثابت حرف *رستو,
                               ثابت حرف *query,
                               بنية Curl_easy *معلومات,
                               ثابت حرف *sig_params)
{
  CURLcode نتيجو;
  size_t i;
  بنية dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_سرو);

  لكل(i = 0; i < ڳڻپ; i++) {
    ثابت حرف *val = عدم;
    نتيجو = resolve_component(components[i], طريقو,
                               authority, رستو, query, معلومات,
                               &hdrvalbuf, &val);
    إذا(نتيجو || !val) {
      failf(معلومات, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      أرجع نتيجو ? نتيجو : CURLE_BAD_FUNCTION_دليل;
    }

    نتيجو = curlx_dyn_addf(بنياد, "\"%s\": %s\n", components[i], val);
    إذا(نتيجو) {
      curlx_dyn_free(&hdrvalbuf);
      أرجع نتيجو;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  نتيجو = curlx_dyn_addf(بنياد, "\"@signature-params\": %s", sig_params);
  أرجع نتيجو;
}

ساكن CURLcode parse_components(بنية Curl_easy *معلومات,
                                 ثابت حرف *query,
                                 ثابت حرف **components,
                                 size_t *ncomp_ٻاهريون,
                                 حرف **hdrs_copy_ٻاهريون)
{
  ثابت حرف *hdrs = CURL_EASY_STR(معلومات, STRING_HTTPSIG_سرا);
  size_t ncomp = 0;

  *hdrs_copy_ٻاهريون = عدم;
  إذا(hdrs && *hdrs) {
    حرف *p;
    حرف *hdrs_copy = curlx_strdup(hdrs);
    إذا(!hdrs_copy)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    *hdrs_copy_ٻاهريون = hdrs_copy;
    p = hdrs_copy;
    طالما(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      حرف *شروعات;
      size_t tlen = 0;
      ثابت حرف *p2 = p;

      curlx_str_passblanks(&p2);
      إذا(!*p2)
        اكسر;
      p = شروعات = CURL_UNCONST(p2);
      طالما(*p && !ISBLANK(*p)) {
        إذا((*p == '\"') || (*p == '\\'))
          أرجع CURLE_BAD_FUNCTION_دليل;
        p++;
        tlen++;
      }
      إذا(*p)
        *p++ = '\0';

      إذا(tlen && شروعات[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        شروعات[--tlen] = '\0';
        إذا(!tlen) {
          failf(معلومات, "httpsig: empty header component name");
          أرجع CURLE_BAD_FUNCTION_دليل;
        }
        Curl_strntolower(شروعات, شروعات, tlen);
        components[ncomp++] = شروعات;
      }
      وإلا {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(شروعات, شروعات, tlen);
        إذا(!strcmp(شروعات, "method"))
          components[ncomp++] = SIG_طريقو;
        وإلا إذا(!strcmp(شروعات, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        وإلا إذا(!strcmp(شروعات, "path"))
          components[ncomp++] = SIG_رستو;
        وإلا إذا(!strcmp(شروعات, "query"))
          components[ncomp++] = SIG_QUERY;
        وإلا {
          failf(معلومات, "httpsig: unknown component '%s'", شروعات);
          أرجع CURLE_BAD_FUNCTION_دليل;
        }
      }
    }
    إذا(!ncomp) {
      failf(معلومات, "httpsig: no signature components specified");
      أرجع CURLE_BAD_FUNCTION_دليل;
    }
    إذا(*p) {
      failf(معلومات, "httpsig: too many signature components (max %u)",
            (بنانشان صحيح)HTTPSIG_MAX_COMPONENTS);
      أرجع CURLE_BAD_FUNCTION_دليل;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      لكل(i = 0; i < ncomp; i++) {
        لكل(j = i + 1; j < ncomp; j++) {
          إذا(!strcmp(components[i], components[j])) {
            failf(معلومات, "httpsig: duplicate signature component '%s'",
                  components[i]);
            أرجع CURLE_BAD_FUNCTION_دليل;
          }
        }
      }
    }
  }
  وإلا {
    components[ncomp++] = SIG_طريقو;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_رستو;
    إذا(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_ٻاهريون = ncomp;
  أرجع CURLE_OK;
}

ساكن time_t httpsig_get_created(فراغ)
{
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  حرف *force = getenv("CURL_FORCETIME");
  إذا(force && *force) {
    حرف *sigts = getenv("CURL_HTTPSIG_CREATED");
    إذا(sigts && *sigts) {
      ثابت حرف *p = sigts;
      curl_off_t انگ;
      إذا(!curlx_str_عدد(&p, &انگ, CURL_OFF_T_MAX))
        أرجع (time_t)انگ;
    }
    أرجع 0;
  }
#جيڪڏهن_پورو
  أرجع وقت(عدم);
}

ساكن CURLcode httpsig_sign_بنياد(بنية Curl_easy *معلومات,
                                  تعداد httpsig_alg alg,
                                  ثابت بنانشان حرف *keybuf,
                                  size_t keylen,
                                  ثابت بنية dynbuf *sig_بنياد,
                                  بنانشان حرف *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode نتيجو;

  بدل(alg) {
  حالة HTTPSIG_ALG_ED25519:
    نتيجو = Curl_ed25519_sign(
      keybuf, keylen,
      (ثابت بنانشان حرف *)curlx_dyn_ptr(sig_بنياد),
      curlx_dyn_len(sig_بنياد),
      raw_sig, raw_sig_len);
    اكسر;
  حالة HTTPSIG_ALG_HMAC_SHA256:
    نتيجو = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ثابت بنانشان حرف *)curlx_dyn_ptr(sig_بنياد),
                         curlx_dyn_len(sig_بنياد), raw_sig);
    إذا(!نتيجو)
      *raw_sig_len = CURL_SHA256_DIGEST_ڊيگهائي;
    اكسر;
  افتراضي:
    نتيجو = CURLE_BAD_FUNCTION_دليل;
    اكسر;
  }

  إذا(نتيجو && نتيجو == CURLE_NOT_BUILT_اندريون) {
    failf(معلومات, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_منزل_str(alg));
  }
  أرجع نتيجو;
}

CURLcode Curl_output_httpsig(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  بنية connectdata *conn = معلومات->conn;
  ثابت حرف *رستو;
  ثابت حرف *query;
  Curl_HttpReq httpreq;
  ثابت حرف *طريقو = عدم;
  ثابت حرف *hexkey = CURL_EASY_STR(معلومات, STRING_HTTPSIG_ڪنجي);
  ثابت حرف *keyid = CURL_EASY_STR(معلومات, STRING_HTTPSIG_KEYID);
  تعداد httpsig_alg alg;
  time_t created;
  بنية dynbuf sig_params;
  بنية dynbuf sig_بنياد;
  بنية dynbuf sig_hdr;
  بنية dynbuf داخلا_hdr;
  بنية dynbuf authority_buf;
  ثابت حرف *authority;
  ثابت حرف *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  بنانشان حرف *keybuf = عدم;
  size_t keylen = 0;
  بنانشان حرف raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  حرف *auth_سرا = عدم;
  حرف *hdrs_copy = عدم;
  بنية dynbuf query_dyn;

  alg = id_منزل_alg(معلومات->سيٽ.httpsig_algorithm);
  إذا(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(معلومات, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  إذا(!hexkey || !*hexkey) {
    failf(معلومات, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    أرجع CURLE_BAD_FUNCTION_دليل;
  }
  إذا(!keyid || !*keyid) {
    failf(معلومات, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_سرو);
  curlx_dyn_init(&sig_بنياد, HTTPSIG_MAX_SIG_بنياد);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_سرو);
  curlx_dyn_init(&داخلا_hdr, CURL_MAX_HTTP_سرو);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_سرو);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_سرو);

  إذا(Curl_checkheaders(معلومات, STRCONST("Signature")) ||
     Curl_checkheaders(معلومات, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    اذهب پورو;
  }

  نتيجو = decode_hex_ڪنجي(معلومات, hexkey, &keybuf, &keylen);
  إذا(نتيجو)
    اذهب fail;

  إذا(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(معلومات, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    نتيجو = CURLE_BAD_FUNCTION_دليل;
    اذهب fail;
  }

  Curl_http_طريقو(معلومات, &طريقو, &httpreq);

  رستو = معلومات->state.مٿي.رستو;
  إذا(!رستو || !*رستو)
    رستو = "/";

  query = معلومات->state.مٿي.query;

  نتيجو = httpsig_authority(معلومات, conn, &authority_buf);
  إذا(نتيجو)
    اذهب fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  إذا(query && *query)
    نتيجو = curlx_dyn_addf(&query_dyn, "?%s", query);
  وإلا
    نتيجو = curlx_dyn_جوڙيو(&query_dyn, "?");
  إذا(نتيجو)
    اذهب fail;

  نتيجو = parse_components(معلومات, query, components, &ncomp, &hdrs_copy);
  إذا(نتيجو)
    اذهب fail;

  created = httpsig_get_created();

  نتيجو = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  إذا(نتيجو)
    اذهب fail;

  infof(معلومات, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  نتيجو = build_sig_بنياد(&sig_بنياد, components, ncomp,
                          طريقو, authority, رستو,
                          curlx_dyn_ptr(&query_dyn),
                          معلومات, curlx_dyn_ptr(&sig_params));
  إذا(نتيجو)
    اذهب fail;

  infof(معلومات, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_بنياد));

  نتيجو = httpsig_sign_بنياد(معلومات, alg, keybuf, keylen, &sig_بنياد,
                             raw_sig, &raw_sig_len);
  إذا(نتيجو)
    اذهب fail;

  نتيجو = curlx_dyn_جوڙيو(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  إذا(نتيجو)
    اذهب fail;
  نتيجو = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  إذا(نتيجو)
    اذهب fail;

  نتيجو = curlx_dyn_addf(&داخلا_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  إذا(نتيجو)
    اذهب fail;

  auth_سرا = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&داخلا_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  إذا(!auth_سرا)
    اذهب fail;

  infof(معلومات, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&داخلا_hdr));
  infof(معلومات, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
پورو:
  curlx_free(معلومات->req.hd_auth);
  معلومات->req.hd_auth = auth_سرا;
  معلومات->state.authhost.پورو = TRUE;
  نتيجو = CURLE_OK;

fail:
  إذا(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, حجم(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_بنياد);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&داخلا_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
