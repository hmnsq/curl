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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_httpsig.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ed25519.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_hmac.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sha256.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "slist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPSIG_MAX_SIG_ਅਧਾਰ   CURL_MAX_HTTP_ਸਿਰਲੇਖ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPSIG_MAX_COMPONENTS 16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPSIG_DEFAULT_LABEL  "sig1"

ਗਿਣਤੀ httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *alg_ਮੰਜ਼ਿਲ_str(ਗਿਣਤੀ httpsig_alg alg)
{
  ਚੋਣ(alg) {
  ਹਾਲਤ HTTPSIG_ALG_ED25519:
    ਵਾਪਸ "ed25519";
  ਹਾਲਤ HTTPSIG_ALG_HMAC_SHA256:
    ਵਾਪਸ "hmac-sha256";
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਗਿਣਤੀ httpsig_alg id_ਮੰਜ਼ਿਲ_alg(uint8_t val)
{
  ਚੋਣ(val) {
  ਹਾਲਤ CURLHTTPSIG_ED25519:
    ਵਾਪਸ HTTPSIG_ALG_ED25519;
  ਹਾਲਤ CURLHTTPSIG_HMAC_SHA256:
    ਵਾਪਸ HTTPSIG_ALG_HMAC_SHA256;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ HTTPSIG_ALG_UNKNOWN;
}

ਸਥਿਰ CURLcode decode_hex_ਕੁੰਜੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅਚਲ ਅੱਖਰ *hexstr,
                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **keyout,
                               size_t *keylen)
{
  size_t len, i;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *keybuf;

  *keyout = ਨਿਰਾ;
  *keylen = 0;

  len = ਲੜੀ_ਲੰਬਾਈ(hexstr);
  ਜਦੋਂ(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  ਜੇ(len == 0 || (len & 1) != 0) {
    failf(ਅੰਕੜੇ, "httpsig: invalid hex key (length %zu)", len);
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  ਜੇ(len > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ) {
    failf(ਅੰਕੜੇ, "httpsig: hex key too long");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  keybuf = curlx_malloc(len / 2);
  ਜੇ(!keybuf)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਲਈ(i = 0; i < len; i += 2) {
    ਜੇ(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(ਅੰਕੜੇ, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
    keybuf[i / 2] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  ਵਾਪਸ CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ਸਥਿਰ CURLcode httpsig_authority(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn,
                                  ਬਣਤਰ dynbuf *authority_buf)
{
  ਅਚਲ ਅੱਖਰ *h = ਅੰਕੜੇ->state.http_host;

  ਜੇ(h && curl_strnequal(h, "host:", 5)) {
    ਅਚਲ ਅੱਖਰ *ਮੁੱਲ = h + 5;
    ਅਚਲ ਅੱਖਰ *ਸਿਰਾ;

    ਜਦੋਂ(ISBLANK(*ਮੁੱਲ))
      ਮੁੱਲ++;
    ਜੇ(*ਮੁੱਲ) {
      CURLcode ਨਤੀਜਾ;

      ਸਿਰਾ = ਮੁੱਲ;
      ਜਦੋਂ(*ਸਿਰਾ && !ISNEWLINE(*ਸਿਰਾ))
        ਸਿਰਾ++;
      ਜਦੋਂ(ਸਿਰਾ > ਮੁੱਲ && ISBLANK(ਸਿਰਾ[-1]))
        ਸਿਰਾ--;
      ਨਤੀਜਾ = curlx_dyn_addn(authority_buf, ਮੁੱਲ, (size_t)(ਸਿਰਾ - ਮੁੱਲ));
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      ਵਾਪਸ CURLE_OK;
    }
  }

  {
    ਅਚਲ ਅੱਖਰ *hostname = conn->origin->hostname;
    uint16_t ਪੋਰਟ = conn->origin->ਪੋਰਟ;

    ਜੇ((conn->origin->scheme->defport != ਪੋਰਟ) && ਪੋਰਟ)
      ਵਾਪਸ curlx_dyn_addf(authority_buf, "%s:%u", hostname, ਪੋਰਟ);
    ਵਾਪਸ curlx_dyn_ਜੋੜੋ(authority_buf, hostname);
  }
}

ਸਥਿਰ CURLcode sf_append_quoted(ਬਣਤਰ dynbuf *buf, ਅਚਲ ਅੱਖਰ *str)
{
  CURLcode ਨਤੀਜਾ = curlx_dyn_addn(buf, "\"", 1);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਜਦੋਂ(*str) {
    ਜੇ(ISCNTRL(*str))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜੇ(*str == '\\' || *str == '"') {
      ਨਤੀਜਾ = curlx_dyn_addn(buf, "\\", 1);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਤੀਜਾ = curlx_dyn_addn(buf, str, 1);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    str++;
  }
  ਵਾਪਸ curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ਸਥਿਰ CURLcode sf_encode_byte_seq(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *raw, size_t rawlen,
                                   ਬਣਤਰ dynbuf *ਬਾਹਰਲਾ)
{
  CURLcode ਨਤੀਜਾ;
  size_t b64len;
  ਅੱਖਰ *b64;

  ਨਤੀਜਾ = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = curlx_dyn_addn(ਬਾਹਰਲਾ, ":", 1);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = curlx_dyn_addn(ਬਾਹਰਲਾ, b64, b64len);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = curlx_dyn_addn(ਬਾਹਰਲਾ, ":", 1);

  curlx_free(b64);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode build_sig_params(ਬਣਤਰ dynbuf *params,
                                 ਅਚਲ ਅੱਖਰ **components, size_t ਗਿਣਤੀ_2,
                                 time_t created, ਅਚਲ ਅੱਖਰ *keyid,
                                 ਗਿਣਤੀ httpsig_alg alg)
{
  CURLcode ਨਤੀਜਾ;
  size_t i;

  ਨਤੀਜਾ = curlx_dyn_addn(params, "(", 1);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਲਈ(i = 0; i < ਗਿਣਤੀ_2; i++) {
    ਜੇ(i > 0) {
      ਨਤੀਜਾ = curlx_dyn_addn(params, " ", 1);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਤੀਜਾ = sf_append_quoted(params, components[i]);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਨਤੀਜਾ = curlx_dyn_addn(params, ")", 1);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = curlx_dyn_addf(params, ";created=%lld", (ਲੰਮਾ ਲੰਮਾ)created);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(keyid && *keyid) {
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(params, ";keyid=");
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਨਤੀਜਾ = sf_append_quoted(params, keyid);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਨਤੀਜਾ = curlx_dyn_addf(params, ";alg=\"%s\"", alg_ਮੰਜ਼ਿਲ_str(alg));
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* strings defined by RFC 9421 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIG_ਵਿਧੀ    "@method"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIG_AUTHORITY "@authority"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIG_ਰਸਤਾ      "@path"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIG_QUERY     "@query"

/* Resolve a component identifier to its value.
 * For headers, we walk the full user-supplied header list to combine
 * duplicate field values with ", " per RFC 9421 Section 2.1. Each
 * individual value is trimmed of leading/trailing OWS and the trailing
 * \r\n. The combined result is written into the caller-provided buffer. */
ਸਥਿਰ CURLcode resolve_component(ਅਚਲ ਅੱਖਰ *name,
                                  ਅਚਲ ਅੱਖਰ *ਵਿਧੀ,
                                  ਅਚਲ ਅੱਖਰ *authority,
                                  ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                                  ਅਚਲ ਅੱਖਰ *query,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ dynbuf *valbuf,
                                  ਅਚਲ ਅੱਖਰ **ਬਾਹਰਲਾ)
{
  *ਬਾਹਰਲਾ = ਨਿਰਾ;

  ਜੇ(name[0] == '@') {
    ਜੇ(curl_strequal(name, SIG_ਵਿਧੀ))
      *ਬਾਹਰਲਾ = ਵਿਧੀ;
    ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(name, SIG_AUTHORITY))
      *ਬਾਹਰਲਾ = authority;
    ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(name, SIG_ਰਸਤਾ))
      *ਬਾਹਰਲਾ = ਰਸਤਾ;
    ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(name, SIG_QUERY))
      *ਬਾਹਰਲਾ = query;
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "httpsig: unsupported derived component '%s'", name);
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
    ਜੇ(!*ਬਾਹਰਲਾ) {
      failf(ਅੰਕੜੇ, "httpsig: derived component '%s' has no value", name);
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
    ਵਾਪਸ CURLE_OK;
  }
  ਨਹੀਂਤਾਂ {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    ਬਣਤਰ curl_slist *ਸਿਰ;
    size_t namelen = ਲੜੀ_ਲੰਬਾਈ(name);
    bool ਮਿਲਿਆ = FALSE;

    curlx_dyn_reset(valbuf);

    ਲਈ(ਸਿਰ = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ; ਸਿਰ; ਸਿਰ = ਸਿਰ->ਅਗਲਾ) {
      ਜੇ(curl_strnequal(ਸਿਰ->ਅੰਕੜੇ, name, namelen) &&
         Curl_headersep(ਸਿਰ->ਅੰਕੜੇ[namelen])) {
        ਅਚਲ ਅੱਖਰ *p = strchr(ਸਿਰ->ਅੰਕੜੇ, ':');
        ਜੇ(p) {
          CURLcode ਨਤੀਜਾ;
          ਬਣਤਰ Curl_str content;
          curlx_str_ਸੌਂਪੋ(&content, p + 1, ਲੜੀ_ਲੰਬਾਈ(p + 1));
          curlx_str_trimblanks(&content);
          ਜੇ(ਮਿਲਿਆ) {
            ਨਤੀਜਾ = curlx_dyn_addn(valbuf, ", ", 2);
            ਜੇ(ਨਤੀਜਾ)
              ਵਾਪਸ ਨਤੀਜਾ;
          }
          ਨਤੀਜਾ = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          ਜੇ(ਨਤੀਜਾ)
            ਵਾਪਸ ਨਤੀਜਾ;
          ਮਿਲਿਆ = TRUE;
        }
      }
    }

    ਜੇ(ਮਿਲਿਆ) {
      *ਬਾਹਰਲਾ = curlx_dyn_ptr(valbuf);
      ਵਾਪਸ CURLE_OK;
    }
    failf(ਅੰਕੜੇ, "httpsig: header '%s' not found in request", name);
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
}

ਸਥਿਰ CURLcode build_sig_ਅਧਾਰ(ਬਣਤਰ dynbuf *ਅਧਾਰ,
                               ਅਚਲ ਅੱਖਰ **components, size_t ਗਿਣਤੀ_2,
                               ਅਚਲ ਅੱਖਰ *ਵਿਧੀ,
                               ਅਚਲ ਅੱਖਰ *authority,
                               ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                               ਅਚਲ ਅੱਖਰ *query,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅਚਲ ਅੱਖਰ *sig_params)
{
  CURLcode ਨਤੀਜਾ;
  size_t i;
  ਬਣਤਰ dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_ਸਿਰਲੇਖ);

  ਲਈ(i = 0; i < ਗਿਣਤੀ_2; i++) {
    ਅਚਲ ਅੱਖਰ *val = ਨਿਰਾ;
    ਨਤੀਜਾ = resolve_component(components[i], ਵਿਧੀ,
                               authority, ਰਸਤਾ, query, ਅੰਕੜੇ,
                               &hdrvalbuf, &val);
    ਜੇ(ਨਤੀਜਾ || !val) {
      failf(ਅੰਕੜੇ, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      ਵਾਪਸ ਨਤੀਜਾ ? ਨਤੀਜਾ : CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }

    ਨਤੀਜਾ = curlx_dyn_addf(ਅਧਾਰ, "\"%s\": %s\n", components[i], val);
    ਜੇ(ਨਤੀਜਾ) {
      curlx_dyn_free(&hdrvalbuf);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  ਨਤੀਜਾ = curlx_dyn_addf(ਅਧਾਰ, "\"@signature-params\": %s", sig_params);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode parse_components(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਅੱਖਰ *query,
                                 ਅਚਲ ਅੱਖਰ **components,
                                 size_t *ncomp_ਬਾਹਰਲਾ,
                                 ਅੱਖਰ **hdrs_copy_ਬਾਹਰਲਾ)
{
  ਅਚਲ ਅੱਖਰ *hdrs = CURL_EASY_STR(ਅੰਕੜੇ, STRING_HTTPSIG_ਸਿਰਲੇਖਾਂ);
  size_t ncomp = 0;

  *hdrs_copy_ਬਾਹਰਲਾ = ਨਿਰਾ;
  ਜੇ(hdrs && *hdrs) {
    ਅੱਖਰ *p;
    ਅੱਖਰ *hdrs_copy = curlx_strdup(hdrs);
    ਜੇ(!hdrs_copy)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    *hdrs_copy_ਬਾਹਰਲਾ = hdrs_copy;
    p = hdrs_copy;
    ਜਦੋਂ(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      ਅੱਖਰ *ਸ਼ੁਰੂ;
      size_t tlen = 0;
      ਅਚਲ ਅੱਖਰ *p2 = p;

      curlx_str_passblanks(&p2);
      ਜੇ(!*p2)
        ਰੋਕੋ;
      p = ਸ਼ੁਰੂ = CURL_UNCONST(p2);
      ਜਦੋਂ(*p && !ISBLANK(*p)) {
        ਜੇ((*p == '\"') || (*p == '\\'))
          ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
        p++;
        tlen++;
      }
      ਜੇ(*p)
        *p++ = '\0';

      ਜੇ(tlen && ਸ਼ੁਰੂ[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        ਸ਼ੁਰੂ[--tlen] = '\0';
        ਜੇ(!tlen) {
          failf(ਅੰਕੜੇ, "httpsig: empty header component name");
          ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
        }
        Curl_strntolower(ਸ਼ੁਰੂ, ਸ਼ੁਰੂ, tlen);
        components[ncomp++] = ਸ਼ੁਰੂ;
      }
      ਨਹੀਂਤਾਂ {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(ਸ਼ੁਰੂ, ਸ਼ੁਰੂ, tlen);
        ਜੇ(!strcmp(ਸ਼ੁਰੂ, "method"))
          components[ncomp++] = SIG_ਵਿਧੀ;
        ਨਹੀਂਤਾਂ ਜੇ(!strcmp(ਸ਼ੁਰੂ, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        ਨਹੀਂਤਾਂ ਜੇ(!strcmp(ਸ਼ੁਰੂ, "path"))
          components[ncomp++] = SIG_ਰਸਤਾ;
        ਨਹੀਂਤਾਂ ਜੇ(!strcmp(ਸ਼ੁਰੂ, "query"))
          components[ncomp++] = SIG_QUERY;
        ਨਹੀਂਤਾਂ {
          failf(ਅੰਕੜੇ, "httpsig: unknown component '%s'", ਸ਼ੁਰੂ);
          ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
        }
      }
    }
    ਜੇ(!ncomp) {
      failf(ਅੰਕੜੇ, "httpsig: no signature components specified");
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
    ਜੇ(*p) {
      failf(ਅੰਕੜੇ, "httpsig: too many signature components (max %u)",
            (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)HTTPSIG_MAX_COMPONENTS);
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      ਲਈ(i = 0; i < ncomp; i++) {
        ਲਈ(j = i + 1; j < ncomp; j++) {
          ਜੇ(!strcmp(components[i], components[j])) {
            failf(ਅੰਕੜੇ, "httpsig: duplicate signature component '%s'",
                  components[i]);
            ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
          }
        }
      }
    }
  }
  ਨਹੀਂਤਾਂ {
    components[ncomp++] = SIG_ਵਿਧੀ;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_ਰਸਤਾ;
    ਜੇ(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_ਬਾਹਰਲਾ = ncomp;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ time_t httpsig_get_created(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਅੱਖਰ *force = getenv("CURL_FORCETIME");
  ਜੇ(force && *force) {
    ਅੱਖਰ *sigts = getenv("CURL_HTTPSIG_CREATED");
    ਜੇ(sigts && *sigts) {
      ਅਚਲ ਅੱਖਰ *p = sigts;
      curl_off_t ਅੰਕ;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &ਅੰਕ, CURL_OFF_T_MAX))
        ਵਾਪਸ (time_t)ਅੰਕ;
    }
    ਵਾਪਸ 0;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ ਸਮਾਂ(ਨਿਰਾ);
}

ਸਥਿਰ CURLcode httpsig_sign_ਅਧਾਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਗਿਣਤੀ httpsig_alg alg,
                                  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *keybuf,
                                  size_t keylen,
                                  ਅਚਲ ਬਣਤਰ dynbuf *sig_ਅਧਾਰ,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode ਨਤੀਜਾ;

  ਚੋਣ(alg) {
  ਹਾਲਤ HTTPSIG_ALG_ED25519:
    ਨਤੀਜਾ = Curl_ed25519_sign(
      keybuf, keylen,
      (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)curlx_dyn_ptr(sig_ਅਧਾਰ),
      curlx_dyn_len(sig_ਅਧਾਰ),
      raw_sig, raw_sig_len);
    ਰੋਕੋ;
  ਹਾਲਤ HTTPSIG_ALG_HMAC_SHA256:
    ਨਤੀਜਾ = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)curlx_dyn_ptr(sig_ਅਧਾਰ),
                         curlx_dyn_len(sig_ਅਧਾਰ), raw_sig);
    ਜੇ(!ਨਤੀਜਾ)
      *raw_sig_len = CURL_SHA256_DIGEST_ਲੰਮਾਈ;
    ਰੋਕੋ;
  ਮੂਲ:
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਰੋਕੋ;
  }

  ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ == CURLE_NOT_BUILT_ਵਿੱਚ) {
    failf(ਅੰਕੜੇ, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_ਮੰਜ਼ਿਲ_str(alg));
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_output_httpsig(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਅਚਲ ਅੱਖਰ *ਰਸਤਾ;
  ਅਚਲ ਅੱਖਰ *query;
  Curl_HttpReq httpreq;
  ਅਚਲ ਅੱਖਰ *ਵਿਧੀ = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *hexkey = CURL_EASY_STR(ਅੰਕੜੇ, STRING_HTTPSIG_ਕੁੰਜੀ);
  ਅਚਲ ਅੱਖਰ *keyid = CURL_EASY_STR(ਅੰਕੜੇ, STRING_HTTPSIG_KEYID);
  ਗਿਣਤੀ httpsig_alg alg;
  time_t created;
  ਬਣਤਰ dynbuf sig_params;
  ਬਣਤਰ dynbuf sig_ਅਧਾਰ;
  ਬਣਤਰ dynbuf sig_hdr;
  ਬਣਤਰ dynbuf ਨਿਵੇਸ਼_hdr;
  ਬਣਤਰ dynbuf authority_buf;
  ਅਚਲ ਅੱਖਰ *authority;
  ਅਚਲ ਅੱਖਰ *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *keybuf = ਨਿਰਾ;
  size_t keylen = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  ਅੱਖਰ *auth_ਸਿਰਲੇਖਾਂ = ਨਿਰਾ;
  ਅੱਖਰ *hdrs_copy = ਨਿਰਾ;
  ਬਣਤਰ dynbuf query_dyn;

  alg = id_ਮੰਜ਼ਿਲ_alg(ਅੰਕੜੇ->ਸੈੱਟ.httpsig_algorithm);
  ਜੇ(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(ਅੰਕੜੇ, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  ਜੇ(!hexkey || !*hexkey) {
    failf(ਅੰਕੜੇ, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
  ਜੇ(!keyid || !*keyid) {
    failf(ਅੰਕੜੇ, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_ਸਿਰਲੇਖ);
  curlx_dyn_init(&sig_ਅਧਾਰ, HTTPSIG_MAX_SIG_ਅਧਾਰ);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_ਸਿਰਲੇਖ);
  curlx_dyn_init(&ਨਿਵੇਸ਼_hdr, CURL_MAX_HTTP_ਸਿਰਲੇਖ);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_ਸਿਰਲੇਖ);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_ਸਿਰਲੇਖ);

  ਜੇ(Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Signature")) ||
     Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    ਜਾਓ ਪੂਰਾ;
  }

  ਨਤੀਜਾ = decode_hex_ਕੁੰਜੀ(ਅੰਕੜੇ, hexkey, &keybuf, &keylen);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  ਜੇ(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(ਅੰਕੜੇ, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ fail;
  }

  Curl_http_ਵਿਧੀ(ਅੰਕੜੇ, &ਵਿਧੀ, &httpreq);

  ਰਸਤਾ = ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ;
  ਜੇ(!ਰਸਤਾ || !*ਰਸਤਾ)
    ਰਸਤਾ = "/";

  query = ਅੰਕੜੇ->state.ਉੱਪਰ.query;

  ਨਤੀਜਾ = httpsig_authority(ਅੰਕੜੇ, conn, &authority_buf);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  ਜੇ(query && *query)
    ਨਤੀਜਾ = curlx_dyn_addf(&query_dyn, "?%s", query);
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&query_dyn, "?");
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  ਨਤੀਜਾ = parse_components(ਅੰਕੜੇ, query, components, &ncomp, &hdrs_copy);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  created = httpsig_get_created();

  ਨਤੀਜਾ = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  infof(ਅੰਕੜੇ, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  ਨਤੀਜਾ = build_sig_ਅਧਾਰ(&sig_ਅਧਾਰ, components, ncomp,
                          ਵਿਧੀ, authority, ਰਸਤਾ,
                          curlx_dyn_ptr(&query_dyn),
                          ਅੰਕੜੇ, curlx_dyn_ptr(&sig_params));
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  infof(ਅੰਕੜੇ, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_ਅਧਾਰ));

  ਨਤੀਜਾ = httpsig_sign_ਅਧਾਰ(ਅੰਕੜੇ, alg, keybuf, keylen, &sig_ਅਧਾਰ,
                             raw_sig, &raw_sig_len);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;
  ਨਤੀਜਾ = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  ਨਤੀਜਾ = curlx_dyn_addf(&ਨਿਵੇਸ਼_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  auth_ਸਿਰਲੇਖਾਂ = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&ਨਿਵੇਸ਼_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  ਜੇ(!auth_ਸਿਰਲੇਖਾਂ)
    ਜਾਓ fail;

  infof(ਅੰਕੜੇ, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&ਨਿਵੇਸ਼_hdr));
  infof(ਅੰਕੜੇ, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
ਪੂਰਾ:
  curlx_free(ਅੰਕੜੇ->req.hd_auth);
  ਅੰਕੜੇ->req.hd_auth = auth_ਸਿਰਲੇਖਾਂ;
  ਅੰਕੜੇ->state.authhost.ਪੂਰਾ = TRUE;
  ਨਤੀਜਾ = CURLE_OK;

fail:
  ਜੇ(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, ਆਕਾਰ(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_ਅਧਾਰ);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&ਨਿਵੇਸ਼_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
