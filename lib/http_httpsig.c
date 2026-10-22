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

#تعریف_کن HTTPSIG_MAX_SIG_پایه   CURL_MAX_HTTP_سرآیند
#تعریف_کن HTTPSIG_MAX_COMPONENTS 16
#تعریف_کن HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#تعریف_کن HTTPSIG_DEFAULT_LABEL  "sig1"

تعداد httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

ساكن ثابت حرف *alg_مقصد_str(تعداد httpsig_alg alg)
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

ساكن تعداد httpsig_alg id_مقصد_alg(uint8_t val)
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

ساكن CURLcode decode_hex_کلید(بنية Curl_easy *داده,
                               ثابت حرف *hexstr,
                               بیعلامت حرف **keyout,
                               size_t *keylen)
{
  size_t len, i;
  بیعلامت حرف *keybuf;

  *keyout = عدم;
  *keylen = 0;

  len = طول_رشته(hexstr);
  طالما(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  إذا(len == 0 || (len & 1) != 0) {
    failf(داده, "httpsig: invalid hex key (length %zu)", len);
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  إذا(len > CURL_MAX_درون_داد_درازی) {
    failf(داده, "httpsig: hex key too long");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  keybuf = curlx_malloc(len / 2);
  إذا(!keybuf)
    أرجع CURLE_بیرون_از_حافظه;

  لكل(i = 0; i < len; i += 2) {
    إذا(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(داده, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }
    keybuf[i / 2] = (بیعلامت حرف)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  أرجع CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ساكن CURLcode httpsig_authority(بنية Curl_easy *داده,
                                  بنية connectdata *conn,
                                  بنية dynbuf *authority_buf)
{
  ثابت حرف *h = داده->state.http_host;

  إذا(h && curl_strnequal(h, "host:", 5)) {
    ثابت حرف *مقدار = h + 5;
    ثابت حرف *انتها;

    طالما(ISBLANK(*مقدار))
      مقدار++;
    إذا(*مقدار) {
      CURLcode نتیجه;

      انتها = مقدار;
      طالما(*انتها && !ISNEWLINE(*انتها))
        انتها++;
      طالما(انتها > مقدار && ISBLANK(انتها[-1]))
        انتها--;
      نتیجه = curlx_dyn_addn(authority_buf, مقدار, (size_t)(انتها - مقدار));
      إذا(نتیجه)
        أرجع نتیجه;
      أرجع CURLE_OK;
    }
  }

  {
    ثابت حرف *hostname = conn->origin->hostname;
    uint16_t پورت = conn->origin->پورت;

    إذا((conn->origin->scheme->defport != پورت) && پورت)
      أرجع curlx_dyn_addf(authority_buf, "%s:%u", hostname, پورت);
    أرجع curlx_dyn_جمع_کن(authority_buf, hostname);
  }
}

ساكن CURLcode sf_append_quoted(بنية dynbuf *buf, ثابت حرف *str)
{
  CURLcode نتیجه = curlx_dyn_addn(buf, "\"", 1);
  إذا(نتیجه)
    أرجع نتیجه;
  طالما(*str) {
    إذا(ISCNTRL(*str))
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    إذا(*str == '\\' || *str == '"') {
      نتیجه = curlx_dyn_addn(buf, "\\", 1);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    نتیجه = curlx_dyn_addn(buf, str, 1);
    إذا(نتیجه)
      أرجع نتیجه;
    str++;
  }
  أرجع curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ساكن CURLcode sf_encode_byte_seq(ثابت بیعلامت حرف *raw, size_t rawlen,
                                   بنية dynbuf *بیرون)
{
  CURLcode نتیجه;
  size_t b64len;
  حرف *b64;

  نتیجه = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = curlx_dyn_addn(بیرون, ":", 1);
  إذا(!نتیجه)
    نتیجه = curlx_dyn_addn(بیرون, b64, b64len);
  إذا(!نتیجه)
    نتیجه = curlx_dyn_addn(بیرون, ":", 1);

  curlx_free(b64);
  أرجع نتیجه;
}

ساكن CURLcode build_sig_params(بنية dynbuf *params,
                                 ثابت حرف **components, size_t شمار,
                                 time_t created, ثابت حرف *keyid,
                                 تعداد httpsig_alg alg)
{
  CURLcode نتیجه;
  size_t i;

  نتیجه = curlx_dyn_addn(params, "(", 1);
  إذا(نتیجه)
    أرجع نتیجه;

  لكل(i = 0; i < شمار; i++) {
    إذا(i > 0) {
      نتیجه = curlx_dyn_addn(params, " ", 1);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    نتیجه = sf_append_quoted(params, components[i]);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  نتیجه = curlx_dyn_addn(params, ")", 1);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = curlx_dyn_addf(params, ";created=%lld", (دراز دراز)created);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(keyid && *keyid) {
    نتیجه = curlx_dyn_جمع_کن(params, ";keyid=");
    إذا(نتیجه)
      أرجع نتیجه;
    نتیجه = sf_append_quoted(params, keyid);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  نتیجه = curlx_dyn_addf(params, ";alg=\"%s\"", alg_مقصد_str(alg));
  أرجع نتیجه;
}

/* strings defined by RFC 9421 */
#تعریف_کن SIG_روش    "@method"
#تعریف_کن SIG_AUTHORITY "@authority"
#تعریف_کن SIG_مسیر      "@path"
#تعریف_کن SIG_QUERY     "@query"

/* Resolve a component identifier to its value.
 * For headers, we walk the full user-supplied header list to combine
 * duplicate field values with ", " per RFC 9421 Section 2.1. Each
 * individual value is trimmed of leading/trailing OWS and the trailing
 * \r\n. The combined result is written into the caller-provided buffer. */
ساكن CURLcode resolve_component(ثابت حرف *name,
                                  ثابت حرف *روش,
                                  ثابت حرف *authority,
                                  ثابت حرف *مسیر,
                                  ثابت حرف *query,
                                  بنية Curl_easy *داده,
                                  بنية dynbuf *valbuf,
                                  ثابت حرف **بیرون)
{
  *بیرون = عدم;

  إذا(name[0] == '@') {
    إذا(curl_strequal(name, SIG_روش))
      *بیرون = روش;
    وإلا إذا(curl_strequal(name, SIG_AUTHORITY))
      *بیرون = authority;
    وإلا إذا(curl_strequal(name, SIG_مسیر))
      *بیرون = مسیر;
    وإلا إذا(curl_strequal(name, SIG_QUERY))
      *بیرون = query;
    وإلا {
      failf(داده, "httpsig: unsupported derived component '%s'", name);
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }
    إذا(!*بیرون) {
      failf(داده, "httpsig: derived component '%s' has no value", name);
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }
    أرجع CURLE_OK;
  }
  وإلا {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    بنية curl_slist *سر;
    size_t namelen = طول_رشته(name);
    bool یافت = FALSE;

    curlx_dyn_reset(valbuf);

    لكل(سر = داده->مجموعه.سرآیندها; سر; سر = سر->بعدی) {
      إذا(curl_strnequal(سر->داده, name, namelen) &&
         Curl_headersep(سر->داده[namelen])) {
        ثابت حرف *p = strchr(سر->داده, ':');
        إذا(p) {
          CURLcode نتیجه;
          بنية Curl_str content;
          curlx_str_تخصیص_بده(&content, p + 1, طول_رشته(p + 1));
          curlx_str_trimblanks(&content);
          إذا(یافت) {
            نتیجه = curlx_dyn_addn(valbuf, ", ", 2);
            إذا(نتیجه)
              أرجع نتیجه;
          }
          نتیجه = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          إذا(نتیجه)
            أرجع نتیجه;
          یافت = TRUE;
        }
      }
    }

    إذا(یافت) {
      *بیرون = curlx_dyn_ptr(valbuf);
      أرجع CURLE_OK;
    }
    failf(داده, "httpsig: header '%s' not found in request", name);
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }
}

ساكن CURLcode build_sig_پایه(بنية dynbuf *پایه,
                               ثابت حرف **components, size_t شمار,
                               ثابت حرف *روش,
                               ثابت حرف *authority,
                               ثابت حرف *مسیر,
                               ثابت حرف *query,
                               بنية Curl_easy *داده,
                               ثابت حرف *sig_params)
{
  CURLcode نتیجه;
  size_t i;
  بنية dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_سرآیند);

  لكل(i = 0; i < شمار; i++) {
    ثابت حرف *val = عدم;
    نتیجه = resolve_component(components[i], روش,
                               authority, مسیر, query, داده,
                               &hdrvalbuf, &val);
    إذا(نتیجه || !val) {
      failf(داده, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      أرجع نتیجه ? نتیجه : CURLE_BAD_FUNCTION_آرگومان;
    }

    نتیجه = curlx_dyn_addf(پایه, "\"%s\": %s\n", components[i], val);
    إذا(نتیجه) {
      curlx_dyn_free(&hdrvalbuf);
      أرجع نتیجه;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  نتیجه = curlx_dyn_addf(پایه, "\"@signature-params\": %s", sig_params);
  أرجع نتیجه;
}

ساكن CURLcode parse_components(بنية Curl_easy *داده,
                                 ثابت حرف *query,
                                 ثابت حرف **components,
                                 size_t *ncomp_بیرون,
                                 حرف **hdrs_copy_بیرون)
{
  ثابت حرف *hdrs = CURL_EASY_STR(داده, STRING_HTTPSIG_سرآیندها);
  size_t ncomp = 0;

  *hdrs_copy_بیرون = عدم;
  إذا(hdrs && *hdrs) {
    حرف *p;
    حرف *hdrs_copy = curlx_strdup(hdrs);
    إذا(!hdrs_copy)
      أرجع CURLE_بیرون_از_حافظه;
    *hdrs_copy_بیرون = hdrs_copy;
    p = hdrs_copy;
    طالما(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      حرف *شروع;
      size_t tlen = 0;
      ثابت حرف *p2 = p;

      curlx_str_passblanks(&p2);
      إذا(!*p2)
        اكسر;
      p = شروع = CURL_UNCONST(p2);
      طالما(*p && !ISBLANK(*p)) {
        إذا((*p == '\"') || (*p == '\\'))
          أرجع CURLE_BAD_FUNCTION_آرگومان;
        p++;
        tlen++;
      }
      إذا(*p)
        *p++ = '\0';

      إذا(tlen && شروع[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        شروع[--tlen] = '\0';
        إذا(!tlen) {
          failf(داده, "httpsig: empty header component name");
          أرجع CURLE_BAD_FUNCTION_آرگومان;
        }
        Curl_strntolower(شروع, شروع, tlen);
        components[ncomp++] = شروع;
      }
      وإلا {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(شروع, شروع, tlen);
        إذا(!strcmp(شروع, "method"))
          components[ncomp++] = SIG_روش;
        وإلا إذا(!strcmp(شروع, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        وإلا إذا(!strcmp(شروع, "path"))
          components[ncomp++] = SIG_مسیر;
        وإلا إذا(!strcmp(شروع, "query"))
          components[ncomp++] = SIG_QUERY;
        وإلا {
          failf(داده, "httpsig: unknown component '%s'", شروع);
          أرجع CURLE_BAD_FUNCTION_آرگومان;
        }
      }
    }
    إذا(!ncomp) {
      failf(داده, "httpsig: no signature components specified");
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }
    إذا(*p) {
      failf(داده, "httpsig: too many signature components (max %u)",
            (بیعلامت صحيح)HTTPSIG_MAX_COMPONENTS);
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      لكل(i = 0; i < ncomp; i++) {
        لكل(j = i + 1; j < ncomp; j++) {
          إذا(!strcmp(components[i], components[j])) {
            failf(داده, "httpsig: duplicate signature component '%s'",
                  components[i]);
            أرجع CURLE_BAD_FUNCTION_آرگومان;
          }
        }
      }
    }
  }
  وإلا {
    components[ncomp++] = SIG_روش;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_مسیر;
    إذا(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_بیرون = ncomp;
  أرجع CURLE_OK;
}

ساكن time_t httpsig_get_created(فراغ)
{
#اگر_تعریف_شده DEBUGBUILD
  حرف *force = getenv("CURL_FORCETIME");
  إذا(force && *force) {
    حرف *sigts = getenv("CURL_HTTPSIG_CREATED");
    إذا(sigts && *sigts) {
      ثابت حرف *p = sigts;
      curl_off_t رقم;
      إذا(!curlx_str_عدد(&p, &رقم, CURL_OFF_T_MAX))
        أرجع (time_t)رقم;
    }
    أرجع 0;
  }
#پایان_اگر
  أرجع زمان(عدم);
}

ساكن CURLcode httpsig_sign_پایه(بنية Curl_easy *داده,
                                  تعداد httpsig_alg alg,
                                  ثابت بیعلامت حرف *keybuf,
                                  size_t keylen,
                                  ثابت بنية dynbuf *sig_پایه,
                                  بیعلامت حرف *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode نتیجه;

  بدل(alg) {
  حالة HTTPSIG_ALG_ED25519:
    نتیجه = Curl_ed25519_sign(
      keybuf, keylen,
      (ثابت بیعلامت حرف *)curlx_dyn_ptr(sig_پایه),
      curlx_dyn_len(sig_پایه),
      raw_sig, raw_sig_len);
    اكسر;
  حالة HTTPSIG_ALG_HMAC_SHA256:
    نتیجه = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ثابت بیعلامت حرف *)curlx_dyn_ptr(sig_پایه),
                         curlx_dyn_len(sig_پایه), raw_sig);
    إذا(!نتیجه)
      *raw_sig_len = CURL_SHA256_DIGEST_درازی;
    اكسر;
  افتراضي:
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    اكسر;
  }

  إذا(نتیجه && نتیجه == CURLE_NOT_BUILT_درون) {
    failf(داده, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_مقصد_str(alg));
  }
  أرجع نتیجه;
}

CURLcode Curl_output_httpsig(بنية Curl_easy *داده)
{
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  بنية connectdata *conn = داده->conn;
  ثابت حرف *مسیر;
  ثابت حرف *query;
  Curl_HttpReq httpreq;
  ثابت حرف *روش = عدم;
  ثابت حرف *hexkey = CURL_EASY_STR(داده, STRING_HTTPSIG_کلید);
  ثابت حرف *keyid = CURL_EASY_STR(داده, STRING_HTTPSIG_KEYID);
  تعداد httpsig_alg alg;
  time_t created;
  بنية dynbuf sig_params;
  بنية dynbuf sig_پایه;
  بنية dynbuf sig_hdr;
  بنية dynbuf درون_داد_hdr;
  بنية dynbuf authority_buf;
  ثابت حرف *authority;
  ثابت حرف *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  بیعلامت حرف *keybuf = عدم;
  size_t keylen = 0;
  بیعلامت حرف raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  حرف *auth_سرآیندها = عدم;
  حرف *hdrs_copy = عدم;
  بنية dynbuf query_dyn;

  alg = id_مقصد_alg(داده->مجموعه.httpsig_algorithm);
  إذا(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(داده, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  إذا(!hexkey || !*hexkey) {
    failf(داده, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }
  إذا(!keyid || !*keyid) {
    failf(داده, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_سرآیند);
  curlx_dyn_init(&sig_پایه, HTTPSIG_MAX_SIG_پایه);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_سرآیند);
  curlx_dyn_init(&درون_داد_hdr, CURL_MAX_HTTP_سرآیند);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_سرآیند);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_سرآیند);

  إذا(Curl_checkheaders(داده, STRCONST("Signature")) ||
     Curl_checkheaders(داده, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    اذهب انجامشده;
  }

  نتیجه = decode_hex_کلید(داده, hexkey, &keybuf, &keylen);
  إذا(نتیجه)
    اذهب fail;

  إذا(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(داده, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    اذهب fail;
  }

  Curl_http_روش(داده, &روش, &httpreq);

  مسیر = داده->state.به_بالا.مسیر;
  إذا(!مسیر || !*مسیر)
    مسیر = "/";

  query = داده->state.به_بالا.query;

  نتیجه = httpsig_authority(داده, conn, &authority_buf);
  إذا(نتیجه)
    اذهب fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  إذا(query && *query)
    نتیجه = curlx_dyn_addf(&query_dyn, "?%s", query);
  وإلا
    نتیجه = curlx_dyn_جمع_کن(&query_dyn, "?");
  إذا(نتیجه)
    اذهب fail;

  نتیجه = parse_components(داده, query, components, &ncomp, &hdrs_copy);
  إذا(نتیجه)
    اذهب fail;

  created = httpsig_get_created();

  نتیجه = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  إذا(نتیجه)
    اذهب fail;

  infof(داده, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  نتیجه = build_sig_پایه(&sig_پایه, components, ncomp,
                          روش, authority, مسیر,
                          curlx_dyn_ptr(&query_dyn),
                          داده, curlx_dyn_ptr(&sig_params));
  إذا(نتیجه)
    اذهب fail;

  infof(داده, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_پایه));

  نتیجه = httpsig_sign_پایه(داده, alg, keybuf, keylen, &sig_پایه,
                             raw_sig, &raw_sig_len);
  إذا(نتیجه)
    اذهب fail;

  نتیجه = curlx_dyn_جمع_کن(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  إذا(نتیجه)
    اذهب fail;
  نتیجه = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  إذا(نتیجه)
    اذهب fail;

  نتیجه = curlx_dyn_addf(&درون_داد_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  إذا(نتیجه)
    اذهب fail;

  auth_سرآیندها = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&درون_داد_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  إذا(!auth_سرآیندها)
    اذهب fail;

  infof(داده, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&درون_داد_hdr));
  infof(داده, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
انجامشده:
  curlx_free(داده->req.hd_auth);
  داده->req.hd_auth = auth_سرآیندها;
  داده->state.authhost.انجامشده = TRUE;
  نتیجه = CURLE_OK;

fail:
  إذا(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, حجم(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_پایه);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&درون_داد_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
