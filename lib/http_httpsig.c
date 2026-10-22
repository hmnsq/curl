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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#شامل "urldata.h"
#شامل "http_httpsig.h"
#شامل "curl_ed25519.h"
#شامل "curl_hmac.h"
#شامل "curl_sha256.h"
#شامل "http.h"
#شامل "transfer.h"
#شامل "curl_trc.h"
#شامل "slist.h"
#شامل "curlx/dynbuf.h"
#شامل "curlx/base64.h"
#شامل "curlx/strdup.h"
#شامل "curlx/strparse.h"
#شامل "strcase.h"

#شامل <time.h>

#تعریف_کرو HTTPSIG_MAX_SIG_بنیاد   CURL_MAX_HTTP_سرخی
#تعریف_کرو HTTPSIG_MAX_COMPONENTS 16
#تعریف_کرو HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#تعریف_کرو HTTPSIG_DEFAULT_LABEL  "sig1"

شمار httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

ساکن ثابت حرف *alg_مقصد_str(شمار httpsig_alg alg)
{
  انتخاب(alg) {
  حالت HTTPSIG_ALG_ED25519:
    واپس "ed25519";
  حالت HTTPSIG_ALG_HMAC_SHA256:
    واپس "hmac-sha256";
  طےشدہ:
    توڑو;
  }
  واپس تہی;
}

ساکن شمار httpsig_alg id_مقصد_alg(uint8_t val)
{
  انتخاب(val) {
  حالت CURLHTTPSIG_ED25519:
    واپس HTTPSIG_ALG_ED25519;
  حالت CURLHTTPSIG_HMAC_SHA256:
    واپس HTTPSIG_ALG_HMAC_SHA256;
  طےشدہ:
    توڑو;
  }
  واپس HTTPSIG_ALG_UNKNOWN;
}

ساکن CURLcode decode_hex_کلید(ساخت Curl_easy *ڈیٹا,
                               ثابت حرف *hexstr,
                               غیرعلامتی حرف **keyout,
                               size_t *keylen)
{
  size_t len, i;
  غیرعلامتی حرف *keybuf;

  *keyout = تہی;
  *keylen = 0;

  len = لڑی_لمبائی(hexstr);
  جب(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  اگر(len == 0 || (len & 1) != 0) {
    failf(ڈیٹا, "httpsig: invalid hex key (length %zu)", len);
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  اگر(len > CURL_MAX_داخلہ_لمبائی) {
    failf(ڈیٹا, "httpsig: hex key too long");
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  keybuf = curlx_malloc(len / 2);
  اگر(!keybuf)
    واپس CURLE_برآمد_کا_حافظہ;

  برائے(i = 0; i < len; i += 2) {
    اگر(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(ڈیٹا, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      واپس CURLE_BAD_FUNCTION_دلیل;
    }
    keybuf[i / 2] = (غیرعلامتی حرف)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  واپس CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ساکن CURLcode httpsig_authority(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn,
                                  ساخت dynbuf *authority_buf)
{
  ثابت حرف *h = ڈیٹا->state.http_host;

  اگر(h && curl_strnequal(h, "host:", 5)) {
    ثابت حرف *قدر = h + 5;
    ثابت حرف *انتہا;

    جب(ISBLANK(*قدر))
      قدر++;
    اگر(*قدر) {
      CURLcode نتیجہ;

      انتہا = قدر;
      جب(*انتہا && !ISNEWLINE(*انتہا))
        انتہا++;
      جب(انتہا > قدر && ISBLANK(انتہا[-1]))
        انتہا--;
      نتیجہ = curlx_dyn_addn(authority_buf, قدر, (size_t)(انتہا - قدر));
      اگر(نتیجہ)
        واپس نتیجہ;
      واپس CURLE_OK;
    }
  }

  {
    ثابت حرف *hostname = conn->origin->hostname;
    uint16_t پورٹ = conn->origin->پورٹ;

    اگر((conn->origin->scheme->defport != پورٹ) && پورٹ)
      واپس curlx_dyn_addf(authority_buf, "%s:%u", hostname, پورٹ);
    واپس curlx_dyn_جوڑو(authority_buf, hostname);
  }
}

ساکن CURLcode sf_append_quoted(ساخت dynbuf *buf, ثابت حرف *str)
{
  CURLcode نتیجہ = curlx_dyn_addn(buf, "\"", 1);
  اگر(نتیجہ)
    واپس نتیجہ;
  جب(*str) {
    اگر(ISCNTRL(*str))
      واپس CURLE_BAD_FUNCTION_دلیل;
    اگر(*str == '\\' || *str == '"') {
      نتیجہ = curlx_dyn_addn(buf, "\\", 1);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    نتیجہ = curlx_dyn_addn(buf, str, 1);
    اگر(نتیجہ)
      واپس نتیجہ;
    str++;
  }
  واپس curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ساکن CURLcode sf_encode_byte_seq(ثابت غیرعلامتی حرف *raw, size_t rawlen,
                                   ساخت dynbuf *برآمد)
{
  CURLcode نتیجہ;
  size_t b64len;
  حرف *b64;

  نتیجہ = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = curlx_dyn_addn(برآمد, ":", 1);
  اگر(!نتیجہ)
    نتیجہ = curlx_dyn_addn(برآمد, b64, b64len);
  اگر(!نتیجہ)
    نتیجہ = curlx_dyn_addn(برآمد, ":", 1);

  curlx_free(b64);
  واپس نتیجہ;
}

ساکن CURLcode build_sig_params(ساخت dynbuf *params,
                                 ثابت حرف **components, size_t گنتی,
                                 time_t created, ثابت حرف *keyid,
                                 شمار httpsig_alg alg)
{
  CURLcode نتیجہ;
  size_t i;

  نتیجہ = curlx_dyn_addn(params, "(", 1);
  اگر(نتیجہ)
    واپس نتیجہ;

  برائے(i = 0; i < گنتی; i++) {
    اگر(i > 0) {
      نتیجہ = curlx_dyn_addn(params, " ", 1);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    نتیجہ = sf_append_quoted(params, components[i]);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  نتیجہ = curlx_dyn_addn(params, ")", 1);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = curlx_dyn_addf(params, ";created=%lld", (طویل طویل)created);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(keyid && *keyid) {
    نتیجہ = curlx_dyn_جوڑو(params, ";keyid=");
    اگر(نتیجہ)
      واپس نتیجہ;
    نتیجہ = sf_append_quoted(params, keyid);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  نتیجہ = curlx_dyn_addf(params, ";alg=\"%s\"", alg_مقصد_str(alg));
  واپس نتیجہ;
}

/* strings defined by RFC 9421 */
#تعریف_کرو SIG_طریقہ    "@method"
#تعریف_کرو SIG_AUTHORITY "@authority"
#تعریف_کرو SIG_راستہ      "@path"
#تعریف_کرو SIG_QUERY     "@query"

/* Resolve a component identifier to its value.
 * For headers, we walk the full user-supplied header list to combine
 * duplicate field values with ", " per RFC 9421 Section 2.1. Each
 * individual value is trimmed of leading/trailing OWS and the trailing
 * \r\n. The combined result is written into the caller-provided buffer. */
ساکن CURLcode resolve_component(ثابت حرف *name,
                                  ثابت حرف *طریقہ,
                                  ثابت حرف *authority,
                                  ثابت حرف *راستہ,
                                  ثابت حرف *query,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت dynbuf *valbuf,
                                  ثابت حرف **برآمد)
{
  *برآمد = تہی;

  اگر(name[0] == '@') {
    اگر(curl_strequal(name, SIG_طریقہ))
      *برآمد = طریقہ;
    ورنہ اگر(curl_strequal(name, SIG_AUTHORITY))
      *برآمد = authority;
    ورنہ اگر(curl_strequal(name, SIG_راستہ))
      *برآمد = راستہ;
    ورنہ اگر(curl_strequal(name, SIG_QUERY))
      *برآمد = query;
    ورنہ {
      failf(ڈیٹا, "httpsig: unsupported derived component '%s'", name);
      واپس CURLE_BAD_FUNCTION_دلیل;
    }
    اگر(!*برآمد) {
      failf(ڈیٹا, "httpsig: derived component '%s' has no value", name);
      واپس CURLE_BAD_FUNCTION_دلیل;
    }
    واپس CURLE_OK;
  }
  ورنہ {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    ساخت curl_slist *سر;
    size_t namelen = لڑی_لمبائی(name);
    bool ملا = FALSE;

    curlx_dyn_reset(valbuf);

    برائے(سر = ڈیٹا->سیٹ.سرخیاں; سر; سر = سر->اگلا) {
      اگر(curl_strnequal(سر->ڈیٹا, name, namelen) &&
         Curl_headersep(سر->ڈیٹا[namelen])) {
        ثابت حرف *p = strchr(سر->ڈیٹا, ':');
        اگر(p) {
          CURLcode نتیجہ;
          ساخت Curl_str content;
          curlx_str_تفویض(&content, p + 1, لڑی_لمبائی(p + 1));
          curlx_str_trimblanks(&content);
          اگر(ملا) {
            نتیجہ = curlx_dyn_addn(valbuf, ", ", 2);
            اگر(نتیجہ)
              واپس نتیجہ;
          }
          نتیجہ = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          اگر(نتیجہ)
            واپس نتیجہ;
          ملا = TRUE;
        }
      }
    }

    اگر(ملا) {
      *برآمد = curlx_dyn_ptr(valbuf);
      واپس CURLE_OK;
    }
    failf(ڈیٹا, "httpsig: header '%s' not found in request", name);
    واپس CURLE_BAD_FUNCTION_دلیل;
  }
}

ساکن CURLcode build_sig_بنیاد(ساخت dynbuf *بنیاد,
                               ثابت حرف **components, size_t گنتی,
                               ثابت حرف *طریقہ,
                               ثابت حرف *authority,
                               ثابت حرف *راستہ,
                               ثابت حرف *query,
                               ساخت Curl_easy *ڈیٹا,
                               ثابت حرف *sig_params)
{
  CURLcode نتیجہ;
  size_t i;
  ساخت dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_سرخی);

  برائے(i = 0; i < گنتی; i++) {
    ثابت حرف *val = تہی;
    نتیجہ = resolve_component(components[i], طریقہ,
                               authority, راستہ, query, ڈیٹا,
                               &hdrvalbuf, &val);
    اگر(نتیجہ || !val) {
      failf(ڈیٹا, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      واپس نتیجہ ? نتیجہ : CURLE_BAD_FUNCTION_دلیل;
    }

    نتیجہ = curlx_dyn_addf(بنیاد, "\"%s\": %s\n", components[i], val);
    اگر(نتیجہ) {
      curlx_dyn_free(&hdrvalbuf);
      واپس نتیجہ;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  نتیجہ = curlx_dyn_addf(بنیاد, "\"@signature-params\": %s", sig_params);
  واپس نتیجہ;
}

ساکن CURLcode parse_components(ساخت Curl_easy *ڈیٹا,
                                 ثابت حرف *query,
                                 ثابت حرف **components,
                                 size_t *ncomp_برآمد,
                                 حرف **hdrs_copy_برآمد)
{
  ثابت حرف *hdrs = CURL_EASY_STR(ڈیٹا, STRING_HTTPSIG_سرخیاں);
  size_t ncomp = 0;

  *hdrs_copy_برآمد = تہی;
  اگر(hdrs && *hdrs) {
    حرف *p;
    حرف *hdrs_copy = curlx_strdup(hdrs);
    اگر(!hdrs_copy)
      واپس CURLE_برآمد_کا_حافظہ;
    *hdrs_copy_برآمد = hdrs_copy;
    p = hdrs_copy;
    جب(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      حرف *آغاز;
      size_t tlen = 0;
      ثابت حرف *p2 = p;

      curlx_str_passblanks(&p2);
      اگر(!*p2)
        توڑو;
      p = آغاز = CURL_UNCONST(p2);
      جب(*p && !ISBLANK(*p)) {
        اگر((*p == '\"') || (*p == '\\'))
          واپس CURLE_BAD_FUNCTION_دلیل;
        p++;
        tlen++;
      }
      اگر(*p)
        *p++ = '\0';

      اگر(tlen && آغاز[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        آغاز[--tlen] = '\0';
        اگر(!tlen) {
          failf(ڈیٹا, "httpsig: empty header component name");
          واپس CURLE_BAD_FUNCTION_دلیل;
        }
        Curl_strntolower(آغاز, آغاز, tlen);
        components[ncomp++] = آغاز;
      }
      ورنہ {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(آغاز, آغاز, tlen);
        اگر(!strcmp(آغاز, "method"))
          components[ncomp++] = SIG_طریقہ;
        ورنہ اگر(!strcmp(آغاز, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        ورنہ اگر(!strcmp(آغاز, "path"))
          components[ncomp++] = SIG_راستہ;
        ورنہ اگر(!strcmp(آغاز, "query"))
          components[ncomp++] = SIG_QUERY;
        ورنہ {
          failf(ڈیٹا, "httpsig: unknown component '%s'", آغاز);
          واپس CURLE_BAD_FUNCTION_دلیل;
        }
      }
    }
    اگر(!ncomp) {
      failf(ڈیٹا, "httpsig: no signature components specified");
      واپس CURLE_BAD_FUNCTION_دلیل;
    }
    اگر(*p) {
      failf(ڈیٹا, "httpsig: too many signature components (max %u)",
            (غیرعلامتی صحیح)HTTPSIG_MAX_COMPONENTS);
      واپس CURLE_BAD_FUNCTION_دلیل;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      برائے(i = 0; i < ncomp; i++) {
        برائے(j = i + 1; j < ncomp; j++) {
          اگر(!strcmp(components[i], components[j])) {
            failf(ڈیٹا, "httpsig: duplicate signature component '%s'",
                  components[i]);
            واپس CURLE_BAD_FUNCTION_دلیل;
          }
        }
      }
    }
  }
  ورنہ {
    components[ncomp++] = SIG_طریقہ;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_راستہ;
    اگر(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_برآمد = ncomp;
  واپس CURLE_OK;
}

ساکن time_t httpsig_get_created(خالی)
{
#اگر_تعریف_شدہ DEBUGBUILD
  حرف *force = getenv("CURL_FORCETIME");
  اگر(force && *force) {
    حرف *sigts = getenv("CURL_HTTPSIG_CREATED");
    اگر(sigts && *sigts) {
      ثابت حرف *p = sigts;
      curl_off_t عدد;
      اگر(!curlx_str_نمبر(&p, &عدد, CURL_OFF_T_MAX))
        واپس (time_t)عدد;
    }
    واپس 0;
  }
#اگر_ختم
  واپس وقت(تہی);
}

ساکن CURLcode httpsig_sign_بنیاد(ساخت Curl_easy *ڈیٹا,
                                  شمار httpsig_alg alg,
                                  ثابت غیرعلامتی حرف *keybuf,
                                  size_t keylen,
                                  ثابت ساخت dynbuf *sig_بنیاد,
                                  غیرعلامتی حرف *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode نتیجہ;

  انتخاب(alg) {
  حالت HTTPSIG_ALG_ED25519:
    نتیجہ = Curl_ed25519_sign(
      keybuf, keylen,
      (ثابت غیرعلامتی حرف *)curlx_dyn_ptr(sig_بنیاد),
      curlx_dyn_len(sig_بنیاد),
      raw_sig, raw_sig_len);
    توڑو;
  حالت HTTPSIG_ALG_HMAC_SHA256:
    نتیجہ = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ثابت غیرعلامتی حرف *)curlx_dyn_ptr(sig_بنیاد),
                         curlx_dyn_len(sig_بنیاد), raw_sig);
    اگر(!نتیجہ)
      *raw_sig_len = CURL_SHA256_DIGEST_لمبائی;
    توڑو;
  طےشدہ:
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    توڑو;
  }

  اگر(نتیجہ && نتیجہ == CURLE_NOT_BUILT_اندر) {
    failf(ڈیٹا, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_مقصد_str(alg));
  }
  واپس نتیجہ;
}

CURLcode Curl_output_httpsig(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  ساخت connectdata *conn = ڈیٹا->conn;
  ثابت حرف *راستہ;
  ثابت حرف *query;
  Curl_HttpReq httpreq;
  ثابت حرف *طریقہ = تہی;
  ثابت حرف *hexkey = CURL_EASY_STR(ڈیٹا, STRING_HTTPSIG_کلید);
  ثابت حرف *keyid = CURL_EASY_STR(ڈیٹا, STRING_HTTPSIG_KEYID);
  شمار httpsig_alg alg;
  time_t created;
  ساخت dynbuf sig_params;
  ساخت dynbuf sig_بنیاد;
  ساخت dynbuf sig_hdr;
  ساخت dynbuf داخلہ_hdr;
  ساخت dynbuf authority_buf;
  ثابت حرف *authority;
  ثابت حرف *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  غیرعلامتی حرف *keybuf = تہی;
  size_t keylen = 0;
  غیرعلامتی حرف raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  حرف *auth_سرخیاں = تہی;
  حرف *hdrs_copy = تہی;
  ساخت dynbuf query_dyn;

  alg = id_مقصد_alg(ڈیٹا->سیٹ.httpsig_algorithm);
  اگر(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(ڈیٹا, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  اگر(!hexkey || !*hexkey) {
    failf(ڈیٹا, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    واپس CURLE_BAD_FUNCTION_دلیل;
  }
  اگر(!keyid || !*keyid) {
    failf(ڈیٹا, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&sig_بنیاد, HTTPSIG_MAX_SIG_بنیاد);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&داخلہ_hdr, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_سرخی);

  اگر(Curl_checkheaders(ڈیٹا, STRCONST("Signature")) ||
     Curl_checkheaders(ڈیٹا, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    جاؤ مکمل;
  }

  نتیجہ = decode_hex_کلید(ڈیٹا, hexkey, &keybuf, &keylen);
  اگر(نتیجہ)
    جاؤ fail;

  اگر(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(ڈیٹا, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ fail;
  }

  Curl_http_طریقہ(ڈیٹا, &طریقہ, &httpreq);

  راستہ = ڈیٹا->state.اوپر.راستہ;
  اگر(!راستہ || !*راستہ)
    راستہ = "/";

  query = ڈیٹا->state.اوپر.query;

  نتیجہ = httpsig_authority(ڈیٹا, conn, &authority_buf);
  اگر(نتیجہ)
    جاؤ fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  اگر(query && *query)
    نتیجہ = curlx_dyn_addf(&query_dyn, "?%s", query);
  ورنہ
    نتیجہ = curlx_dyn_جوڑو(&query_dyn, "?");
  اگر(نتیجہ)
    جاؤ fail;

  نتیجہ = parse_components(ڈیٹا, query, components, &ncomp, &hdrs_copy);
  اگر(نتیجہ)
    جاؤ fail;

  created = httpsig_get_created();

  نتیجہ = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  اگر(نتیجہ)
    جاؤ fail;

  infof(ڈیٹا, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  نتیجہ = build_sig_بنیاد(&sig_بنیاد, components, ncomp,
                          طریقہ, authority, راستہ,
                          curlx_dyn_ptr(&query_dyn),
                          ڈیٹا, curlx_dyn_ptr(&sig_params));
  اگر(نتیجہ)
    جاؤ fail;

  infof(ڈیٹا, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_بنیاد));

  نتیجہ = httpsig_sign_بنیاد(ڈیٹا, alg, keybuf, keylen, &sig_بنیاد,
                             raw_sig, &raw_sig_len);
  اگر(نتیجہ)
    جاؤ fail;

  نتیجہ = curlx_dyn_جوڑو(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  اگر(نتیجہ)
    جاؤ fail;
  نتیجہ = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  اگر(نتیجہ)
    جاؤ fail;

  نتیجہ = curlx_dyn_addf(&داخلہ_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  اگر(نتیجہ)
    جاؤ fail;

  auth_سرخیاں = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&داخلہ_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  اگر(!auth_سرخیاں)
    جاؤ fail;

  infof(ڈیٹا, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&داخلہ_hdr));
  infof(ڈیٹا, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
مکمل:
  curlx_free(ڈیٹا->req.hd_auth);
  ڈیٹا->req.hd_auth = auth_سرخیاں;
  ڈیٹا->state.authhost.مکمل = TRUE;
  نتیجہ = CURLE_OK;

fail:
  اگر(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, حجم(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_بنیاد);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&داخلہ_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
