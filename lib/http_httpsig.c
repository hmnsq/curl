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

#تعریف_کرو HTTPSIG_MAX_SIG_ادھار   CURL_MAX_HTTP_سرخی
#تعریف_کرو HTTPSIG_MAX_COMPONENTS 16
#تعریف_کرو HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#تعریف_کرو HTTPSIG_DEFAULT_LABEL  "sig1"

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

ساكن CURLcode decode_hex_کلید(بنية Curl_easy *انکڑے,
                               ثابت حرف *hexstr,
                               بنانشانی حرف **keyout,
                               size_t *keylen)
{
  size_t len, i;
  بنانشانی حرف *keybuf;

  *keyout = عدم;
  *keylen = 0;

  len = لڑی_لمبائی(hexstr);
  طالما(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  إذا(len == 0 || (len & 1) != 0) {
    failf(انکڑے, "httpsig: invalid hex key (length %zu)", len);
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  إذا(len > CURL_MAX_داخلہ_لمائی) {
    failf(انکڑے, "httpsig: hex key too long");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  keybuf = curlx_malloc(len / 2);
  إذا(!keybuf)
    أرجع CURLE_باہرلا_دا_حافظہ;

  لكل(i = 0; i < len; i += 2) {
    إذا(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(انکڑے, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
    keybuf[i / 2] = (بنانشانی حرف)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  أرجع CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ساكن CURLcode httpsig_authority(بنية Curl_easy *انکڑے,
                                  بنية connectdata *conn,
                                  بنية dynbuf *authority_buf)
{
  ثابت حرف *h = انکڑے->state.http_host;

  إذا(h && curl_strnequal(h, "host:", 5)) {
    ثابت حرف *قدر = h + 5;
    ثابت حرف *سرا;

    طالما(ISBLANK(*قدر))
      قدر++;
    إذا(*قدر) {
      CURLcode نتیجہ;

      سرا = قدر;
      طالما(*سرا && !ISNEWLINE(*سرا))
        سرا++;
      طالما(سرا > قدر && ISBLANK(سرا[-1]))
        سرا--;
      نتیجہ = curlx_dyn_addn(authority_buf, قدر, (size_t)(سرا - قدر));
      إذا(نتیجہ)
        أرجع نتیجہ;
      أرجع CURLE_OK;
    }
  }

  {
    ثابت حرف *hostname = conn->origin->hostname;
    uint16_t پورٹ = conn->origin->پورٹ;

    إذا((conn->origin->scheme->defport != پورٹ) && پورٹ)
      أرجع curlx_dyn_addf(authority_buf, "%s:%u", hostname, پورٹ);
    أرجع curlx_dyn_جوڑو(authority_buf, hostname);
  }
}

ساكن CURLcode sf_append_quoted(بنية dynbuf *buf, ثابت حرف *str)
{
  CURLcode نتیجہ = curlx_dyn_addn(buf, "\"", 1);
  إذا(نتیجہ)
    أرجع نتیجہ;
  طالما(*str) {
    إذا(ISCNTRL(*str))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    إذا(*str == '\\' || *str == '"') {
      نتیجہ = curlx_dyn_addn(buf, "\\", 1);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    نتیجہ = curlx_dyn_addn(buf, str, 1);
    إذا(نتیجہ)
      أرجع نتیجہ;
    str++;
  }
  أرجع curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ساكن CURLcode sf_encode_byte_seq(ثابت بنانشانی حرف *raw, size_t rawlen,
                                   بنية dynbuf *باہرلا)
{
  CURLcode نتیجہ;
  size_t b64len;
  حرف *b64;

  نتیجہ = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = curlx_dyn_addn(باہرلا, ":", 1);
  إذا(!نتیجہ)
    نتیجہ = curlx_dyn_addn(باہرلا, b64, b64len);
  إذا(!نتیجہ)
    نتیجہ = curlx_dyn_addn(باہرلا, ":", 1);

  curlx_free(b64);
  أرجع نتیجہ;
}

ساكن CURLcode build_sig_params(بنية dynbuf *params,
                                 ثابت حرف **components, size_t گنتی,
                                 time_t created, ثابت حرف *keyid,
                                 تعداد httpsig_alg alg)
{
  CURLcode نتیجہ;
  size_t i;

  نتیجہ = curlx_dyn_addn(params, "(", 1);
  إذا(نتیجہ)
    أرجع نتیجہ;

  لكل(i = 0; i < گنتی; i++) {
    إذا(i > 0) {
      نتیجہ = curlx_dyn_addn(params, " ", 1);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    نتیجہ = sf_append_quoted(params, components[i]);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  نتیجہ = curlx_dyn_addn(params, ")", 1);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = curlx_dyn_addf(params, ";created=%lld", (لما لما)created);
  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(keyid && *keyid) {
    نتیجہ = curlx_dyn_جوڑو(params, ";keyid=");
    إذا(نتیجہ)
      أرجع نتیجہ;
    نتیجہ = sf_append_quoted(params, keyid);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  نتیجہ = curlx_dyn_addf(params, ";alg=\"%s\"", alg_منزل_str(alg));
  أرجع نتیجہ;
}

/* strings defined by RFC 9421 */
#تعریف_کرو SIG_طریقہ    "@method"
#تعریف_کرو SIG_AUTHORITY "@authority"
#تعریف_کرو SIG_رستہ      "@path"
#تعریف_کرو SIG_QUERY     "@query"

/* Resolve a component identifier to its value.
 * For headers, we walk the full user-supplied header list to combine
 * duplicate field values with ", " per RFC 9421 Section 2.1. Each
 * individual value is trimmed of leading/trailing OWS and the trailing
 * \r\n. The combined result is written into the caller-provided buffer. */
ساكن CURLcode resolve_component(ثابت حرف *name,
                                  ثابت حرف *طریقہ,
                                  ثابت حرف *authority,
                                  ثابت حرف *رستہ,
                                  ثابت حرف *query,
                                  بنية Curl_easy *انکڑے,
                                  بنية dynbuf *valbuf,
                                  ثابت حرف **باہرلا)
{
  *باہرلا = عدم;

  إذا(name[0] == '@') {
    إذا(curl_strequal(name, SIG_طریقہ))
      *باہرلا = طریقہ;
    وإلا إذا(curl_strequal(name, SIG_AUTHORITY))
      *باہرلا = authority;
    وإلا إذا(curl_strequal(name, SIG_رستہ))
      *باہرلا = رستہ;
    وإلا إذا(curl_strequal(name, SIG_QUERY))
      *باہرلا = query;
    وإلا {
      failf(انکڑے, "httpsig: unsupported derived component '%s'", name);
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
    إذا(!*باہرلا) {
      failf(انکڑے, "httpsig: derived component '%s' has no value", name);
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
    أرجع CURLE_OK;
  }
  وإلا {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    بنية curl_slist *سر;
    size_t namelen = لڑی_لمبائی(name);
    bool لبھیا = FALSE;

    curlx_dyn_reset(valbuf);

    لكل(سر = انکڑے->سیٹ.سرخیاں; سر; سر = سر->اگلا) {
      إذا(curl_strnequal(سر->انکڑے, name, namelen) &&
         Curl_headersep(سر->انکڑے[namelen])) {
        ثابت حرف *p = strchr(سر->انکڑے, ':');
        إذا(p) {
          CURLcode نتیجہ;
          بنية Curl_str content;
          curlx_str_سونپو(&content, p + 1, لڑی_لمبائی(p + 1));
          curlx_str_trimblanks(&content);
          إذا(لبھیا) {
            نتیجہ = curlx_dyn_addn(valbuf, ", ", 2);
            إذا(نتیجہ)
              أرجع نتیجہ;
          }
          نتیجہ = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          إذا(نتیجہ)
            أرجع نتیجہ;
          لبھیا = TRUE;
        }
      }
    }

    إذا(لبھیا) {
      *باہرلا = curlx_dyn_ptr(valbuf);
      أرجع CURLE_OK;
    }
    failf(انکڑے, "httpsig: header '%s' not found in request", name);
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
}

ساكن CURLcode build_sig_ادھار(بنية dynbuf *ادھار,
                               ثابت حرف **components, size_t گنتی,
                               ثابت حرف *طریقہ,
                               ثابت حرف *authority,
                               ثابت حرف *رستہ,
                               ثابت حرف *query,
                               بنية Curl_easy *انکڑے,
                               ثابت حرف *sig_params)
{
  CURLcode نتیجہ;
  size_t i;
  بنية dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_سرخی);

  لكل(i = 0; i < گنتی; i++) {
    ثابت حرف *val = عدم;
    نتیجہ = resolve_component(components[i], طریقہ,
                               authority, رستہ, query, انکڑے,
                               &hdrvalbuf, &val);
    إذا(نتیجہ || !val) {
      failf(انکڑے, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      أرجع نتیجہ ? نتیجہ : CURLE_BAD_FUNCTION_دلیل;
    }

    نتیجہ = curlx_dyn_addf(ادھار, "\"%s\": %s\n", components[i], val);
    إذا(نتیجہ) {
      curlx_dyn_free(&hdrvalbuf);
      أرجع نتیجہ;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  نتیجہ = curlx_dyn_addf(ادھار, "\"@signature-params\": %s", sig_params);
  أرجع نتیجہ;
}

ساكن CURLcode parse_components(بنية Curl_easy *انکڑے,
                                 ثابت حرف *query,
                                 ثابت حرف **components,
                                 size_t *ncomp_باہرلا,
                                 حرف **hdrs_copy_باہرلا)
{
  ثابت حرف *hdrs = CURL_EASY_STR(انکڑے, STRING_HTTPSIG_سرخیاں);
  size_t ncomp = 0;

  *hdrs_copy_باہرلا = عدم;
  إذا(hdrs && *hdrs) {
    حرف *p;
    حرف *hdrs_copy = curlx_strdup(hdrs);
    إذا(!hdrs_copy)
      أرجع CURLE_باہرلا_دا_حافظہ;
    *hdrs_copy_باہرلا = hdrs_copy;
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
          أرجع CURLE_BAD_FUNCTION_دلیل;
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
          failf(انکڑے, "httpsig: empty header component name");
          أرجع CURLE_BAD_FUNCTION_دلیل;
        }
        Curl_strntolower(شروع, شروع, tlen);
        components[ncomp++] = شروع;
      }
      وإلا {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(شروع, شروع, tlen);
        إذا(!strcmp(شروع, "method"))
          components[ncomp++] = SIG_طریقہ;
        وإلا إذا(!strcmp(شروع, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        وإلا إذا(!strcmp(شروع, "path"))
          components[ncomp++] = SIG_رستہ;
        وإلا إذا(!strcmp(شروع, "query"))
          components[ncomp++] = SIG_QUERY;
        وإلا {
          failf(انکڑے, "httpsig: unknown component '%s'", شروع);
          أرجع CURLE_BAD_FUNCTION_دلیل;
        }
      }
    }
    إذا(!ncomp) {
      failf(انکڑے, "httpsig: no signature components specified");
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
    إذا(*p) {
      failf(انکڑے, "httpsig: too many signature components (max %u)",
            (بنانشانی صحيح)HTTPSIG_MAX_COMPONENTS);
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      لكل(i = 0; i < ncomp; i++) {
        لكل(j = i + 1; j < ncomp; j++) {
          إذا(!strcmp(components[i], components[j])) {
            failf(انکڑے, "httpsig: duplicate signature component '%s'",
                  components[i]);
            أرجع CURLE_BAD_FUNCTION_دلیل;
          }
        }
      }
    }
  }
  وإلا {
    components[ncomp++] = SIG_طریقہ;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_رستہ;
    إذا(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_باہرلا = ncomp;
  أرجع CURLE_OK;
}

ساكن time_t httpsig_get_created(فراغ)
{
#جے_تعریف_شدہ DEBUGBUILD
  حرف *force = getenv("CURL_FORCETIME");
  إذا(force && *force) {
    حرف *sigts = getenv("CURL_HTTPSIG_CREATED");
    إذا(sigts && *sigts) {
      ثابت حرف *p = sigts;
      curl_off_t عدد;
      إذا(!curlx_str_نمبر(&p, &عدد, CURL_OFF_T_MAX))
        أرجع (time_t)عدد;
    }
    أرجع 0;
  }
#جے_انت
  أرجع ویلا(عدم);
}

ساكن CURLcode httpsig_sign_ادھار(بنية Curl_easy *انکڑے,
                                  تعداد httpsig_alg alg,
                                  ثابت بنانشانی حرف *keybuf,
                                  size_t keylen,
                                  ثابت بنية dynbuf *sig_ادھار,
                                  بنانشانی حرف *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode نتیجہ;

  بدل(alg) {
  حالة HTTPSIG_ALG_ED25519:
    نتیجہ = Curl_ed25519_sign(
      keybuf, keylen,
      (ثابت بنانشانی حرف *)curlx_dyn_ptr(sig_ادھار),
      curlx_dyn_len(sig_ادھار),
      raw_sig, raw_sig_len);
    اكسر;
  حالة HTTPSIG_ALG_HMAC_SHA256:
    نتیجہ = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ثابت بنانشانی حرف *)curlx_dyn_ptr(sig_ادھار),
                         curlx_dyn_len(sig_ادھار), raw_sig);
    إذا(!نتیجہ)
      *raw_sig_len = CURL_SHA256_DIGEST_لمائی;
    اكسر;
  افتراضي:
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اكسر;
  }

  إذا(نتیجہ && نتیجہ == CURLE_NOT_BUILT_وچ) {
    failf(انکڑے, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_منزل_str(alg));
  }
  أرجع نتیجہ;
}

CURLcode Curl_output_httpsig(بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  بنية connectdata *conn = انکڑے->conn;
  ثابت حرف *رستہ;
  ثابت حرف *query;
  Curl_HttpReq httpreq;
  ثابت حرف *طریقہ = عدم;
  ثابت حرف *hexkey = CURL_EASY_STR(انکڑے, STRING_HTTPSIG_کلید);
  ثابت حرف *keyid = CURL_EASY_STR(انکڑے, STRING_HTTPSIG_KEYID);
  تعداد httpsig_alg alg;
  time_t created;
  بنية dynbuf sig_params;
  بنية dynbuf sig_ادھار;
  بنية dynbuf sig_hdr;
  بنية dynbuf داخلہ_hdr;
  بنية dynbuf authority_buf;
  ثابت حرف *authority;
  ثابت حرف *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  بنانشانی حرف *keybuf = عدم;
  size_t keylen = 0;
  بنانشانی حرف raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  حرف *auth_سرخیاں = عدم;
  حرف *hdrs_copy = عدم;
  بنية dynbuf query_dyn;

  alg = id_منزل_alg(انکڑے->سیٹ.httpsig_algorithm);
  إذا(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(انکڑے, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  إذا(!hexkey || !*hexkey) {
    failf(انکڑے, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
  إذا(!keyid || !*keyid) {
    failf(انکڑے, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&sig_ادھار, HTTPSIG_MAX_SIG_ادھار);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&داخلہ_hdr, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_سرخی);

  إذا(Curl_checkheaders(انکڑے, STRCONST("Signature")) ||
     Curl_checkheaders(انکڑے, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    اذهب پورا;
  }

  نتیجہ = decode_hex_کلید(انکڑے, hexkey, &keybuf, &keylen);
  إذا(نتیجہ)
    اذهب fail;

  إذا(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(انکڑے, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اذهب fail;
  }

  Curl_http_طریقہ(انکڑے, &طریقہ, &httpreq);

  رستہ = انکڑے->state.اتے.رستہ;
  إذا(!رستہ || !*رستہ)
    رستہ = "/";

  query = انکڑے->state.اتے.query;

  نتیجہ = httpsig_authority(انکڑے, conn, &authority_buf);
  إذا(نتیجہ)
    اذهب fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  إذا(query && *query)
    نتیجہ = curlx_dyn_addf(&query_dyn, "?%s", query);
  وإلا
    نتیجہ = curlx_dyn_جوڑو(&query_dyn, "?");
  إذا(نتیجہ)
    اذهب fail;

  نتیجہ = parse_components(انکڑے, query, components, &ncomp, &hdrs_copy);
  إذا(نتیجہ)
    اذهب fail;

  created = httpsig_get_created();

  نتیجہ = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  إذا(نتیجہ)
    اذهب fail;

  infof(انکڑے, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  نتیجہ = build_sig_ادھار(&sig_ادھار, components, ncomp,
                          طریقہ, authority, رستہ,
                          curlx_dyn_ptr(&query_dyn),
                          انکڑے, curlx_dyn_ptr(&sig_params));
  إذا(نتیجہ)
    اذهب fail;

  infof(انکڑے, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_ادھار));

  نتیجہ = httpsig_sign_ادھار(انکڑے, alg, keybuf, keylen, &sig_ادھار,
                             raw_sig, &raw_sig_len);
  إذا(نتیجہ)
    اذهب fail;

  نتیجہ = curlx_dyn_جوڑو(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  إذا(نتیجہ)
    اذهب fail;
  نتیجہ = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  إذا(نتیجہ)
    اذهب fail;

  نتیجہ = curlx_dyn_addf(&داخلہ_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  إذا(نتیجہ)
    اذهب fail;

  auth_سرخیاں = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&داخلہ_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  إذا(!auth_سرخیاں)
    اذهب fail;

  infof(انکڑے, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&داخلہ_hdr));
  infof(انکڑے, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
پورا:
  curlx_free(انکڑے->req.hd_auth);
  انکڑے->req.hd_auth = auth_سرخیاں;
  انکڑے->state.authhost.پورا = TRUE;
  نتیجہ = CURLE_OK;

fail:
  إذا(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, حجم(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_ادھار);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&داخلہ_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  أرجع نتیجہ;
}

#جے_انت /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
