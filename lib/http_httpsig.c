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

#تعریف_کړه HTTPSIG_MAX_SIG_بنسټ   CURL_MAX_HTTP_سرلیک
#تعریف_کړه HTTPSIG_MAX_COMPONENTS 16
#تعریف_کړه HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#تعریف_کړه HTTPSIG_DEFAULT_LABEL  "sig1"

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

ساكن CURLcode decode_hex_کلی(بنية Curl_easy *معلومات,
                               ثابت حرف *hexstr,
                               بینښه حرف **keyout,
                               size_t *keylen)
{
  size_t len, i;
  بینښه حرف *keybuf;

  *keyout = عدم;
  *keylen = 0;

  len = مزي_اوږدوالی(hexstr);
  طالما(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  إذا(len == 0 || (len & 1) != 0) {
    failf(معلومات, "httpsig: invalid hex key (length %zu)", len);
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  إذا(len > CURL_MAX_ننوت_اوږدتیا) {
    failf(معلومات, "httpsig: hex key too long");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  keybuf = curlx_malloc(len / 2);
  إذا(!keybuf)
    أرجع CURLE_بهر_د_حافظه;

  لكل(i = 0; i < len; i += 2) {
    إذا(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(معلومات, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
    keybuf[i / 2] = (بینښه حرف)((curlx_hexval(hexstr[i]) << 4) |
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
    ثابت حرف *ارزښت = h + 5;
    ثابت حرف *پای_ټکی;

    طالما(ISBLANK(*ارزښت))
      ارزښت++;
    إذا(*ارزښت) {
      CURLcode پایله;

      پای_ټکی = ارزښت;
      طالما(*پای_ټکی && !ISNEWLINE(*پای_ټکی))
        پای_ټکی++;
      طالما(پای_ټکی > ارزښت && ISBLANK(پای_ټکی[-1]))
        پای_ټکی--;
      پایله = curlx_dyn_addn(authority_buf, ارزښت, (size_t)(پای_ټکی - ارزښت));
      إذا(پایله)
        أرجع پایله;
      أرجع CURLE_OK;
    }
  }

  {
    ثابت حرف *hostname = conn->origin->hostname;
    uint16_t پورټ = conn->origin->پورټ;

    إذا((conn->origin->scheme->defport != پورټ) && پورټ)
      أرجع curlx_dyn_addf(authority_buf, "%s:%u", hostname, پورټ);
    أرجع curlx_dyn_جمع_کړه(authority_buf, hostname);
  }
}

ساكن CURLcode sf_append_quoted(بنية dynbuf *buf, ثابت حرف *str)
{
  CURLcode پایله = curlx_dyn_addn(buf, "\"", 1);
  إذا(پایله)
    أرجع پایله;
  طالما(*str) {
    إذا(ISCNTRL(*str))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    إذا(*str == '\\' || *str == '"') {
      پایله = curlx_dyn_addn(buf, "\\", 1);
      إذا(پایله)
        أرجع پایله;
    }
    پایله = curlx_dyn_addn(buf, str, 1);
    إذا(پایله)
      أرجع پایله;
    str++;
  }
  أرجع curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ساكن CURLcode sf_encode_byte_seq(ثابت بینښه حرف *raw, size_t rawlen,
                                   بنية dynbuf *بهر)
{
  CURLcode پایله;
  size_t b64len;
  حرف *b64;

  پایله = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  إذا(پایله)
    أرجع پایله;

  پایله = curlx_dyn_addn(بهر, ":", 1);
  إذا(!پایله)
    پایله = curlx_dyn_addn(بهر, b64, b64len);
  إذا(!پایله)
    پایله = curlx_dyn_addn(بهر, ":", 1);

  curlx_free(b64);
  أرجع پایله;
}

ساكن CURLcode build_sig_params(بنية dynbuf *params,
                                 ثابت حرف **components, size_t شمېر,
                                 time_t created, ثابت حرف *keyid,
                                 تعداد httpsig_alg alg)
{
  CURLcode پایله;
  size_t i;

  پایله = curlx_dyn_addn(params, "(", 1);
  إذا(پایله)
    أرجع پایله;

  لكل(i = 0; i < شمېر; i++) {
    إذا(i > 0) {
      پایله = curlx_dyn_addn(params, " ", 1);
      إذا(پایله)
        أرجع پایله;
    }
    پایله = sf_append_quoted(params, components[i]);
    إذا(پایله)
      أرجع پایله;
  }

  پایله = curlx_dyn_addn(params, ")", 1);
  إذا(پایله)
    أرجع پایله;

  پایله = curlx_dyn_addf(params, ";created=%lld", (اوږد اوږد)created);
  إذا(پایله)
    أرجع پایله;

  إذا(keyid && *keyid) {
    پایله = curlx_dyn_جمع_کړه(params, ";keyid=");
    إذا(پایله)
      أرجع پایله;
    پایله = sf_append_quoted(params, keyid);
    إذا(پایله)
      أرجع پایله;
  }

  پایله = curlx_dyn_addf(params, ";alg=\"%s\"", alg_منزل_str(alg));
  أرجع پایله;
}

/* strings defined by RFC 9421 */
#تعریف_کړه SIG_لاره    "@method"
#تعریف_کړه SIG_AUTHORITY "@authority"
#تعریف_کړه SIG_لار      "@path"
#تعریف_کړه SIG_QUERY     "@query"

/* Resolve a component identifier to its value.
 * For headers, we walk the full user-supplied header list to combine
 * duplicate field values with ", " per RFC 9421 Section 2.1. Each
 * individual value is trimmed of leading/trailing OWS and the trailing
 * \r\n. The combined result is written into the caller-provided buffer. */
ساكن CURLcode resolve_component(ثابت حرف *name,
                                  ثابت حرف *لاره,
                                  ثابت حرف *authority,
                                  ثابت حرف *لار,
                                  ثابت حرف *query,
                                  بنية Curl_easy *معلومات,
                                  بنية dynbuf *valbuf,
                                  ثابت حرف **بهر)
{
  *بهر = عدم;

  إذا(name[0] == '@') {
    إذا(curl_strequal(name, SIG_لاره))
      *بهر = لاره;
    وإلا إذا(curl_strequal(name, SIG_AUTHORITY))
      *بهر = authority;
    وإلا إذا(curl_strequal(name, SIG_لار))
      *بهر = لار;
    وإلا إذا(curl_strequal(name, SIG_QUERY))
      *بهر = query;
    وإلا {
      failf(معلومات, "httpsig: unsupported derived component '%s'", name);
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
    إذا(!*بهر) {
      failf(معلومات, "httpsig: derived component '%s' has no value", name);
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
    أرجع CURLE_OK;
  }
  وإلا {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    بنية curl_slist *سر;
    size_t namelen = مزي_اوږدوالی(name);
    bool وموندل = FALSE;

    curlx_dyn_reset(valbuf);

    لكل(سر = معلومات->سېټ.سرلیکونه; سر; سر = سر->بل) {
      إذا(curl_strnequal(سر->معلومات, name, namelen) &&
         Curl_headersep(سر->معلومات[namelen])) {
        ثابت حرف *p = strchr(سر->معلومات, ':');
        إذا(p) {
          CURLcode پایله;
          بنية Curl_str content;
          curlx_str_ټاکل(&content, p + 1, مزي_اوږدوالی(p + 1));
          curlx_str_trimblanks(&content);
          إذا(وموندل) {
            پایله = curlx_dyn_addn(valbuf, ", ", 2);
            إذا(پایله)
              أرجع پایله;
          }
          پایله = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          إذا(پایله)
            أرجع پایله;
          وموندل = TRUE;
        }
      }
    }

    إذا(وموندل) {
      *بهر = curlx_dyn_ptr(valbuf);
      أرجع CURLE_OK;
    }
    failf(معلومات, "httpsig: header '%s' not found in request", name);
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
}

ساكن CURLcode build_sig_بنسټ(بنية dynbuf *بنسټ,
                               ثابت حرف **components, size_t شمېر,
                               ثابت حرف *لاره,
                               ثابت حرف *authority,
                               ثابت حرف *لار,
                               ثابت حرف *query,
                               بنية Curl_easy *معلومات,
                               ثابت حرف *sig_params)
{
  CURLcode پایله;
  size_t i;
  بنية dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_سرلیک);

  لكل(i = 0; i < شمېر; i++) {
    ثابت حرف *val = عدم;
    پایله = resolve_component(components[i], لاره,
                               authority, لار, query, معلومات,
                               &hdrvalbuf, &val);
    إذا(پایله || !val) {
      failf(معلومات, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      أرجع پایله ? پایله : CURLE_BAD_FUNCTION_دلیل;
    }

    پایله = curlx_dyn_addf(بنسټ, "\"%s\": %s\n", components[i], val);
    إذا(پایله) {
      curlx_dyn_free(&hdrvalbuf);
      أرجع پایله;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  پایله = curlx_dyn_addf(بنسټ, "\"@signature-params\": %s", sig_params);
  أرجع پایله;
}

ساكن CURLcode parse_components(بنية Curl_easy *معلومات,
                                 ثابت حرف *query,
                                 ثابت حرف **components,
                                 size_t *ncomp_بهر,
                                 حرف **hdrs_copy_بهر)
{
  ثابت حرف *hdrs = CURL_EASY_STR(معلومات, STRING_HTTPSIG_سرلیکونه);
  size_t ncomp = 0;

  *hdrs_copy_بهر = عدم;
  إذا(hdrs && *hdrs) {
    حرف *p;
    حرف *hdrs_copy = curlx_strdup(hdrs);
    إذا(!hdrs_copy)
      أرجع CURLE_بهر_د_حافظه;
    *hdrs_copy_بهر = hdrs_copy;
    p = hdrs_copy;
    طالما(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      حرف *پیل;
      size_t tlen = 0;
      ثابت حرف *p2 = p;

      curlx_str_passblanks(&p2);
      إذا(!*p2)
        اكسر;
      p = پیل = CURL_UNCONST(p2);
      طالما(*p && !ISBLANK(*p)) {
        إذا((*p == '\"') || (*p == '\\'))
          أرجع CURLE_BAD_FUNCTION_دلیل;
        p++;
        tlen++;
      }
      إذا(*p)
        *p++ = '\0';

      إذا(tlen && پیل[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        پیل[--tlen] = '\0';
        إذا(!tlen) {
          failf(معلومات, "httpsig: empty header component name");
          أرجع CURLE_BAD_FUNCTION_دلیل;
        }
        Curl_strntolower(پیل, پیل, tlen);
        components[ncomp++] = پیل;
      }
      وإلا {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(پیل, پیل, tlen);
        إذا(!strcmp(پیل, "method"))
          components[ncomp++] = SIG_لاره;
        وإلا إذا(!strcmp(پیل, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        وإلا إذا(!strcmp(پیل, "path"))
          components[ncomp++] = SIG_لار;
        وإلا إذا(!strcmp(پیل, "query"))
          components[ncomp++] = SIG_QUERY;
        وإلا {
          failf(معلومات, "httpsig: unknown component '%s'", پیل);
          أرجع CURLE_BAD_FUNCTION_دلیل;
        }
      }
    }
    إذا(!ncomp) {
      failf(معلومات, "httpsig: no signature components specified");
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
    إذا(*p) {
      failf(معلومات, "httpsig: too many signature components (max %u)",
            (بینښه صحيح)HTTPSIG_MAX_COMPONENTS);
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      لكل(i = 0; i < ncomp; i++) {
        لكل(j = i + 1; j < ncomp; j++) {
          إذا(!strcmp(components[i], components[j])) {
            failf(معلومات, "httpsig: duplicate signature component '%s'",
                  components[i]);
            أرجع CURLE_BAD_FUNCTION_دلیل;
          }
        }
      }
    }
  }
  وإلا {
    components[ncomp++] = SIG_لاره;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_لار;
    إذا(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_بهر = ncomp;
  أرجع CURLE_OK;
}

ساكن time_t httpsig_get_created(فراغ)
{
#که_تعریف_شوی DEBUGBUILD
  حرف *force = getenv("CURL_FORCETIME");
  إذا(force && *force) {
    حرف *sigts = getenv("CURL_HTTPSIG_CREATED");
    إذا(sigts && *sigts) {
      ثابت حرف *p = sigts;
      curl_off_t عدد;
      إذا(!curlx_str_شمېره(&p, &عدد, CURL_OFF_T_MAX))
        أرجع (time_t)عدد;
    }
    أرجع 0;
  }
#د_که_پای
  أرجع وخت(عدم);
}

ساكن CURLcode httpsig_sign_بنسټ(بنية Curl_easy *معلومات,
                                  تعداد httpsig_alg alg,
                                  ثابت بینښه حرف *keybuf,
                                  size_t keylen,
                                  ثابت بنية dynbuf *sig_بنسټ,
                                  بینښه حرف *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode پایله;

  بدل(alg) {
  حالة HTTPSIG_ALG_ED25519:
    پایله = Curl_ed25519_sign(
      keybuf, keylen,
      (ثابت بینښه حرف *)curlx_dyn_ptr(sig_بنسټ),
      curlx_dyn_len(sig_بنسټ),
      raw_sig, raw_sig_len);
    اكسر;
  حالة HTTPSIG_ALG_HMAC_SHA256:
    پایله = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ثابت بینښه حرف *)curlx_dyn_ptr(sig_بنسټ),
                         curlx_dyn_len(sig_بنسټ), raw_sig);
    إذا(!پایله)
      *raw_sig_len = CURL_SHA256_DIGEST_اوږدتیا;
    اكسر;
  افتراضي:
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اكسر;
  }

  إذا(پایله && پایله == CURLE_NOT_BUILT_دننه) {
    failf(معلومات, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_منزل_str(alg));
  }
  أرجع پایله;
}

CURLcode Curl_output_httpsig(بنية Curl_easy *معلومات)
{
  CURLcode پایله = CURLE_بهر_د_حافظه;
  بنية connectdata *conn = معلومات->conn;
  ثابت حرف *لار;
  ثابت حرف *query;
  Curl_HttpReq httpreq;
  ثابت حرف *لاره = عدم;
  ثابت حرف *hexkey = CURL_EASY_STR(معلومات, STRING_HTTPSIG_کلی);
  ثابت حرف *keyid = CURL_EASY_STR(معلومات, STRING_HTTPSIG_KEYID);
  تعداد httpsig_alg alg;
  time_t created;
  بنية dynbuf sig_params;
  بنية dynbuf sig_بنسټ;
  بنية dynbuf sig_hdr;
  بنية dynbuf ننوت_hdr;
  بنية dynbuf authority_buf;
  ثابت حرف *authority;
  ثابت حرف *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  بینښه حرف *keybuf = عدم;
  size_t keylen = 0;
  بینښه حرف raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  حرف *auth_سرلیکونه = عدم;
  حرف *hdrs_copy = عدم;
  بنية dynbuf query_dyn;

  alg = id_منزل_alg(معلومات->سېټ.httpsig_algorithm);
  إذا(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(معلومات, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  إذا(!hexkey || !*hexkey) {
    failf(معلومات, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
  إذا(!keyid || !*keyid) {
    failf(معلومات, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_سرلیک);
  curlx_dyn_init(&sig_بنسټ, HTTPSIG_MAX_SIG_بنسټ);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_سرلیک);
  curlx_dyn_init(&ننوت_hdr, CURL_MAX_HTTP_سرلیک);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_سرلیک);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_سرلیک);

  إذا(Curl_checkheaders(معلومات, STRCONST("Signature")) ||
     Curl_checkheaders(معلومات, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    اذهب بشپړ;
  }

  پایله = decode_hex_کلی(معلومات, hexkey, &keybuf, &keylen);
  إذا(پایله)
    اذهب fail;

  إذا(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(معلومات, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب fail;
  }

  Curl_http_لاره(معلومات, &لاره, &httpreq);

  لار = معلومات->state.پورته.لار;
  إذا(!لار || !*لار)
    لار = "/";

  query = معلومات->state.پورته.query;

  پایله = httpsig_authority(معلومات, conn, &authority_buf);
  إذا(پایله)
    اذهب fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  إذا(query && *query)
    پایله = curlx_dyn_addf(&query_dyn, "?%s", query);
  وإلا
    پایله = curlx_dyn_جمع_کړه(&query_dyn, "?");
  إذا(پایله)
    اذهب fail;

  پایله = parse_components(معلومات, query, components, &ncomp, &hdrs_copy);
  إذا(پایله)
    اذهب fail;

  created = httpsig_get_created();

  پایله = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  إذا(پایله)
    اذهب fail;

  infof(معلومات, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  پایله = build_sig_بنسټ(&sig_بنسټ, components, ncomp,
                          لاره, authority, لار,
                          curlx_dyn_ptr(&query_dyn),
                          معلومات, curlx_dyn_ptr(&sig_params));
  إذا(پایله)
    اذهب fail;

  infof(معلومات, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_بنسټ));

  پایله = httpsig_sign_بنسټ(معلومات, alg, keybuf, keylen, &sig_بنسټ,
                             raw_sig, &raw_sig_len);
  إذا(پایله)
    اذهب fail;

  پایله = curlx_dyn_جمع_کړه(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  إذا(پایله)
    اذهب fail;
  پایله = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  إذا(پایله)
    اذهب fail;

  پایله = curlx_dyn_addf(&ننوت_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  إذا(پایله)
    اذهب fail;

  auth_سرلیکونه = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&ننوت_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  إذا(!auth_سرلیکونه)
    اذهب fail;

  infof(معلومات, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&ننوت_hdr));
  infof(معلومات, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
بشپړ:
  curlx_free(معلومات->req.hd_auth);
  معلومات->req.hd_auth = auth_سرلیکونه;
  معلومات->state.authhost.بشپړ = TRUE;
  پایله = CURLE_OK;

fail:
  إذا(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, حجم(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_بنسټ);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&ننوت_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  أرجع پایله;
}

#د_که_پای /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
