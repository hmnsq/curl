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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "http_httpsig.h"
#ಅನ್ತರ್ಭಾವ "curl_ed25519.h"
#ಅನ್ತರ್ಭಾವ "curl_hmac.h"
#ಅನ್ತರ್ಭಾವ "curl_sha256.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "slist.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"

#ಅನ್ತರ್ಭಾವ <time.h>

#ವ್ಯಾಖ್ಯಾನಿಸು HTTPSIG_MAX_SIG_ತಳ   CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPSIG_MAX_COMPONENTS 16
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPSIG_DEFAULT_LABEL  "sig1"

ಎಣಿಕೆ httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alg_ಗಮ್ಯ_str(ಎಣಿಕೆ httpsig_alg alg)
{
  ಆಯ್ಕೆ(alg) {
  ಸ್ಥಿತಿ HTTPSIG_ALG_ED25519:
    ಹಿಂತಿರುಗಿ "ed25519";
  ಸ್ಥಿತಿ HTTPSIG_ALG_HMAC_SHA256:
    ಹಿಂತಿರುಗಿ "hmac-sha256";
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಎಣಿಕೆ httpsig_alg id_ಗಮ್ಯ_alg(uint8_t val)
{
  ಆಯ್ಕೆ(val) {
  ಸ್ಥಿತಿ CURLHTTPSIG_ED25519:
    ಹಿಂತಿರುಗಿ HTTPSIG_ALG_ED25519;
  ಸ್ಥಿತಿ CURLHTTPSIG_HMAC_SHA256:
    ಹಿಂತಿರುಗಿ HTTPSIG_ALG_HMAC_SHA256;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ HTTPSIG_ALG_UNKNOWN;
}

ಸ್ಥಿರ CURLcode decode_hex_ಕೀಲಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hexstr,
                               ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **keyout,
                               size_t *keylen)
{
  size_t len, i;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *keybuf;

  *keyout = ಖಾಲಿ;
  *keylen = 0;

  len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hexstr);
  ಆಗ(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  ಆದರೆ(len == 0 || (len & 1) != 0) {
    failf(ದತ್ತಾಂಶ, "httpsig: invalid hex key (length %zu)", len);
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  ಆದರೆ(len > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ) {
    failf(ದತ್ತಾಂಶ, "httpsig: hex key too long");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  keybuf = curlx_malloc(len / 2);
  ಆದರೆ(!keybuf)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಗಾಗಿ(i = 0; i < len; i += 2) {
    ಆದರೆ(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(ದತ್ತಾಂಶ, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }
    keybuf[i / 2] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
ಸ್ಥಿರ CURLcode httpsig_authority(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn,
                                  ರಚನೆ dynbuf *authority_buf)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *h = ದತ್ತಾಂಶ->state.http_host;

  ಆದರೆ(h && curl_strnequal(h, "host:", 5)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ = h + 5;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ;

    ಆಗ(ISBLANK(*ಮೌಲ್ಯ))
      ಮೌಲ್ಯ++;
    ಆದರೆ(*ಮೌಲ್ಯ) {
      CURLcode ಫಲಿತಾಂಶ;

      ಕೊನೆ = ಮೌಲ್ಯ;
      ಆಗ(*ಕೊನೆ && !ISNEWLINE(*ಕೊನೆ))
        ಕೊನೆ++;
      ಆಗ(ಕೊನೆ > ಮೌಲ್ಯ && ISBLANK(ಕೊನೆ[-1]))
        ಕೊನೆ--;
      ಫಲಿತಾಂಶ = curlx_dyn_addn(authority_buf, ಮೌಲ್ಯ, (size_t)(ಕೊನೆ - ಮೌಲ್ಯ));
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
  }

  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname = conn->origin->hostname;
    uint16_t ಪೋರ್ಟ್ = conn->origin->ಪೋರ್ಟ್;

    ಆದರೆ((conn->origin->scheme->defport != ಪೋರ್ಟ್) && ಪೋರ್ಟ್)
      ಹಿಂತಿರುಗಿ curlx_dyn_addf(authority_buf, "%s:%u", hostname, ಪೋರ್ಟ್);
    ಹಿಂತಿರುಗಿ curlx_dyn_ಕೂಡಿಸು(authority_buf, hostname);
  }
}

ಸ್ಥಿರ CURLcode sf_append_quoted(ರಚನೆ dynbuf *buf, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
  CURLcode ಫಲಿತಾಂಶ = curlx_dyn_addn(buf, "\"", 1);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಆಗ(*str) {
    ಆದರೆ(ISCNTRL(*str))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಆದರೆ(*str == '\\' || *str == '"') {
      ಫಲಿತಾಂಶ = curlx_dyn_addn(buf, "\\", 1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಫಲಿತಾಂಶ = curlx_dyn_addn(buf, str, 1);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    str++;
  }
  ಹಿಂತಿರುಗಿ curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
ಸ್ಥಿರ CURLcode sf_encode_byte_seq(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *raw, size_t rawlen,
                                   ರಚನೆ dynbuf *ಹೊರಹೋಗು)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t b64len;
  ಅಕ್ಷರ *b64;

  ಫಲಿತಾಂಶ = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = curlx_dyn_addn(ಹೊರಹೋಗು, ":", 1);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = curlx_dyn_addn(ಹೊರಹೋಗು, b64, b64len);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = curlx_dyn_addn(ಹೊರಹೋಗು, ":", 1);

  curlx_free(b64);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode build_sig_params(ರಚನೆ dynbuf *params,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **components, size_t ಎಣಿಕೆ_2,
                                 time_t created, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *keyid,
                                 ಎಣಿಕೆ httpsig_alg alg)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t i;

  ಫಲಿತಾಂಶ = curlx_dyn_addn(params, "(", 1);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಗಾಗಿ(i = 0; i < ಎಣಿಕೆ_2; i++) {
    ಆದರೆ(i > 0) {
      ಫಲಿತಾಂಶ = curlx_dyn_addn(params, " ", 1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಫಲಿತಾಂಶ = sf_append_quoted(params, components[i]);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಫಲಿತಾಂಶ = curlx_dyn_addn(params, ")", 1);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = curlx_dyn_addf(params, ";created=%lld", (ದೀರ್ಘ ದೀರ್ಘ)created);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(keyid && *keyid) {
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(params, ";keyid=");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಫಲಿತಾಂಶ = sf_append_quoted(params, keyid);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಫಲಿತಾಂಶ = curlx_dyn_addf(params, ";alg=\"%s\"", alg_ಗಮ್ಯ_str(alg));
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* strings defined by RFC 9421 */
#ವ್ಯಾಖ್ಯಾನಿಸು SIG_ವಿಧಾನ    "@method"
#ವ್ಯಾಖ್ಯಾನಿಸು SIG_AUTHORITY "@authority"
#ವ್ಯಾಖ್ಯಾನಿಸು SIG_ಮಾರ್ಗ      "@path"
#ವ್ಯಾಖ್ಯಾನಿಸು SIG_QUERY     "@query"

/* Resolve a component identifier to its value.
 * For headers, we walk the full user-supplied header list to combine
 * duplicate field values with ", " per RFC 9421 Section 2.1. Each
 * individual value is trimmed of leading/trailing OWS and the trailing
 * \r\n. The combined result is written into the caller-provided buffer. */
ಸ್ಥಿರ CURLcode resolve_component(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *authority,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ dynbuf *valbuf,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಹೊರಹೋಗು)
{
  *ಹೊರಹೋಗು = ಖಾಲಿ;

  ಆದರೆ(name[0] == '@') {
    ಆದರೆ(curl_strequal(name, SIG_ವಿಧಾನ))
      *ಹೊರಹೋಗು = ವಿಧಾನ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(name, SIG_AUTHORITY))
      *ಹೊರಹೋಗು = authority;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(name, SIG_ಮಾರ್ಗ))
      *ಹೊರಹೋಗು = ಮಾರ್ಗ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(name, SIG_QUERY))
      *ಹೊರಹೋಗು = query;
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "httpsig: unsupported derived component '%s'", name);
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }
    ಆದರೆ(!*ಹೊರಹೋಗು) {
      failf(ದತ್ತಾಂಶ, "httpsig: derived component '%s' has no value", name);
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    ರಚನೆ curl_slist *ತಲೆ;
    size_t namelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
    bool ಸಿಕ್ಕಿತು = FALSE;

    curlx_dyn_reset(valbuf);

    ಗಾಗಿ(ತಲೆ = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು; ತಲೆ; ತಲೆ = ತಲೆ->ಮುಂದಿನ) {
      ಆದರೆ(curl_strnequal(ತಲೆ->ದತ್ತಾಂಶ, name, namelen) &&
         Curl_headersep(ತಲೆ->ದತ್ತಾಂಶ[namelen])) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = strchr(ತಲೆ->ದತ್ತಾಂಶ, ':');
        ಆದರೆ(p) {
          CURLcode ಫಲಿತಾಂಶ;
          ರಚನೆ Curl_str content;
          curlx_str_ನಿಯೋಜಿಸು(&content, p + 1, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p + 1));
          curlx_str_trimblanks(&content);
          ಆದರೆ(ಸಿಕ್ಕಿತು) {
            ಫಲಿತಾಂಶ = curlx_dyn_addn(valbuf, ", ", 2);
            ಆದರೆ(ಫಲಿತಾಂಶ)
              ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
          }
          ಫಲಿತಾಂಶ = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
          ಸಿಕ್ಕಿತು = TRUE;
        }
      }
    }

    ಆದರೆ(ಸಿಕ್ಕಿತು) {
      *ಹೊರಹೋಗು = curlx_dyn_ptr(valbuf);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    failf(ದತ್ತಾಂಶ, "httpsig: header '%s' not found in request", name);
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
}

ಸ್ಥಿರ CURLcode build_sig_ತಳ(ರಚನೆ dynbuf *ತಳ,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **components, size_t ಎಣಿಕೆ_2,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *authority,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sig_params)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t i;
  ರಚನೆ dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);

  ಗಾಗಿ(i = 0; i < ಎಣಿಕೆ_2; i++) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val = ಖಾಲಿ;
    ಫಲಿತಾಂಶ = resolve_component(components[i], ವಿಧಾನ,
                               authority, ಮಾರ್ಗ, query, ದತ್ತಾಂಶ,
                               &hdrvalbuf, &val);
    ಆದರೆ(ಫಲಿತಾಂಶ || !val) {
      failf(ದತ್ತಾಂಶ, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ ? ಫಲಿತಾಂಶ : CURLE_BAD_FUNCTION_ವಾದ;
    }

    ಫಲಿತಾಂಶ = curlx_dyn_addf(ತಳ, "\"%s\": %s\n", components[i], val);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      curlx_dyn_free(&hdrvalbuf);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  ಫಲಿತಾಂಶ = curlx_dyn_addf(ತಳ, "\"@signature-params\": %s", sig_params);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode parse_components(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **components,
                                 size_t *ncomp_ಹೊರಹೋಗು,
                                 ಅಕ್ಷರ **hdrs_copy_ಹೊರಹೋಗು)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hdrs = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_HTTPSIG_ಶೀರ್ಷಿಕೆಗಳು);
  size_t ncomp = 0;

  *hdrs_copy_ಹೊರಹೋಗು = ಖಾಲಿ;
  ಆದರೆ(hdrs && *hdrs) {
    ಅಕ್ಷರ *p;
    ಅಕ್ಷರ *hdrs_copy = curlx_strdup(hdrs);
    ಆದರೆ(!hdrs_copy)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    *hdrs_copy_ಹೊರಹೋಗು = hdrs_copy;
    p = hdrs_copy;
    ಆಗ(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      ಅಕ್ಷರ *ಆರಂಭ;
      size_t tlen = 0;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p2 = p;

      curlx_str_passblanks(&p2);
      ಆದರೆ(!*p2)
        ನಿಲ್ಲಿಸು;
      p = ಆರಂಭ = CURL_UNCONST(p2);
      ಆಗ(*p && !ISBLANK(*p)) {
        ಆದರೆ((*p == '\"') || (*p == '\\'))
          ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
        p++;
        tlen++;
      }
      ಆದರೆ(*p)
        *p++ = '\0';

      ಆದರೆ(tlen && ಆರಂಭ[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        ಆರಂಭ[--tlen] = '\0';
        ಆದರೆ(!tlen) {
          failf(ದತ್ತಾಂಶ, "httpsig: empty header component name");
          ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
        }
        Curl_strntolower(ಆರಂಭ, ಆರಂಭ, tlen);
        components[ncomp++] = ಆರಂಭ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(ಆರಂಭ, ಆರಂಭ, tlen);
        ಆದರೆ(!strcmp(ಆರಂಭ, "method"))
          components[ncomp++] = SIG_ವಿಧಾನ;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(ಆರಂಭ, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(ಆರಂಭ, "path"))
          components[ncomp++] = SIG_ಮಾರ್ಗ;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(ಆರಂಭ, "query"))
          components[ncomp++] = SIG_QUERY;
        ಇಲ್ಲದಿದ್ದರೆ {
          failf(ದತ್ತಾಂಶ, "httpsig: unknown component '%s'", ಆರಂಭ);
          ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
        }
      }
    }
    ಆದರೆ(!ncomp) {
      failf(ದತ್ತಾಂಶ, "httpsig: no signature components specified");
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }
    ಆದರೆ(*p) {
      failf(ದತ್ತಾಂಶ, "httpsig: too many signature components (max %u)",
            (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)HTTPSIG_MAX_COMPONENTS);
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      ಗಾಗಿ(i = 0; i < ncomp; i++) {
        ಗಾಗಿ(j = i + 1; j < ncomp; j++) {
          ಆದರೆ(!strcmp(components[i], components[j])) {
            failf(ದತ್ತಾಂಶ, "httpsig: duplicate signature component '%s'",
                  components[i]);
            ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
          }
        }
      }
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    components[ncomp++] = SIG_ವಿಧಾನ;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_ಮಾರ್ಗ;
    ಆದರೆ(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_ಹೊರಹೋಗು = ncomp;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ time_t httpsig_get_created(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಅಕ್ಷರ *force = getenv("CURL_FORCETIME");
  ಆದರೆ(force && *force) {
    ಅಕ್ಷರ *sigts = getenv("CURL_HTTPSIG_CREATED");
    ಆದರೆ(sigts && *sigts) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = sigts;
      curl_off_t ಅಂಕೆ;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, CURL_OFF_T_MAX))
        ಹಿಂತಿರುಗಿ (time_t)ಅಂಕೆ;
    }
    ಹಿಂತಿರುಗಿ 0;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ ಸಮಯ(ಖಾಲಿ);
}

ಸ್ಥಿರ CURLcode httpsig_sign_ತಳ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಎಣಿಕೆ httpsig_alg alg,
                                  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *keybuf,
                                  size_t keylen,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ dynbuf *sig_ತಳ,
                                  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆಯ್ಕೆ(alg) {
  ಸ್ಥಿತಿ HTTPSIG_ALG_ED25519:
    ಫಲಿತಾಂಶ = Curl_ed25519_sign(
      keybuf, keylen,
      (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)curlx_dyn_ptr(sig_ತಳ),
      curlx_dyn_len(sig_ತಳ),
      raw_sig, raw_sig_len);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ HTTPSIG_ALG_HMAC_SHA256:
    ಫಲಿತಾಂಶ = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)curlx_dyn_ptr(sig_ತಳ),
                         curlx_dyn_len(sig_ತಳ), raw_sig);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      *raw_sig_len = CURL_SHA256_DIGEST_ಉದ್ದಳತೆ;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ == CURLE_NOT_BUILT_ಒಳ) {
    failf(ದತ್ತಾಂಶ, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_ಗಮ್ಯ_str(alg));
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_output_httpsig(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query;
  Curl_HttpReq httpreq;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hexkey = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_HTTPSIG_ಕೀಲಿ);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *keyid = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_HTTPSIG_KEYID);
  ಎಣಿಕೆ httpsig_alg alg;
  time_t created;
  ರಚನೆ dynbuf sig_params;
  ರಚನೆ dynbuf sig_ತಳ;
  ರಚನೆ dynbuf sig_hdr;
  ರಚನೆ dynbuf ಒಳಹರಿವು_hdr;
  ರಚನೆ dynbuf authority_buf;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *authority;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *keybuf = ಖಾಲಿ;
  size_t keylen = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  ಅಕ್ಷರ *auth_ಶೀರ್ಷಿಕೆಗಳು = ಖಾಲಿ;
  ಅಕ್ಷರ *hdrs_copy = ಖಾಲಿ;
  ರಚನೆ dynbuf query_dyn;

  alg = id_ಗಮ್ಯ_alg(ದತ್ತಾಂಶ->ಗಣ.httpsig_algorithm);
  ಆದರೆ(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(ದತ್ತಾಂಶ, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  ಆದರೆ(!hexkey || !*hexkey) {
    failf(ದತ್ತಾಂಶ, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
  ಆದರೆ(!keyid || !*keyid) {
    failf(ದತ್ತಾಂಶ, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);
  curlx_dyn_init(&sig_ತಳ, HTTPSIG_MAX_SIG_ತಳ);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);
  curlx_dyn_init(&ಒಳಹರಿವು_hdr, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);

  ಆದರೆ(Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Signature")) ||
     Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    ಹೋಗು ಮುಗಿದ;
  }

  ಫಲಿತಾಂಶ = decode_hex_ಕೀಲಿ(ದತ್ತಾಂಶ, hexkey, &keybuf, &keylen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  ಆದರೆ(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(ದತ್ತಾಂಶ, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು fail;
  }

  Curl_http_ವಿಧಾನ(ದತ್ತಾಂಶ, &ವಿಧಾನ, &httpreq);

  ಮಾರ್ಗ = ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ;
  ಆದರೆ(!ಮಾರ್ಗ || !*ಮಾರ್ಗ)
    ಮಾರ್ಗ = "/";

  query = ದತ್ತಾಂಶ->state.ಮೇಲೆ.query;

  ಫಲಿತಾಂಶ = httpsig_authority(ದತ್ತಾಂಶ, conn, &authority_buf);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  ಆದರೆ(query && *query)
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&query_dyn, "?%s", query);
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&query_dyn, "?");
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  ಫಲಿತಾಂಶ = parse_components(ದತ್ತಾಂಶ, query, components, &ncomp, &hdrs_copy);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  created = httpsig_get_created();

  ಫಲಿತಾಂಶ = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  infof(ದತ್ತಾಂಶ, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  ಫಲಿತಾಂಶ = build_sig_ತಳ(&sig_ತಳ, components, ncomp,
                          ವಿಧಾನ, authority, ಮಾರ್ಗ,
                          curlx_dyn_ptr(&query_dyn),
                          ದತ್ತಾಂಶ, curlx_dyn_ptr(&sig_params));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  infof(ದತ್ತಾಂಶ, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_ತಳ));

  ಫಲಿತಾಂಶ = httpsig_sign_ತಳ(ದತ್ತಾಂಶ, alg, keybuf, keylen, &sig_ತಳ,
                             raw_sig, &raw_sig_len);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;
  ಫಲಿತಾಂಶ = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  ಫಲಿತಾಂಶ = curlx_dyn_addf(&ಒಳಹರಿವು_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  auth_ಶೀರ್ಷಿಕೆಗಳು = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&ಒಳಹರಿವು_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  ಆದರೆ(!auth_ಶೀರ್ಷಿಕೆಗಳು)
    ಹೋಗು fail;

  infof(ದತ್ತಾಂಶ, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&ಒಳಹರಿವು_hdr));
  infof(ದತ್ತಾಂಶ, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
ಮುಗಿದ:
  curlx_free(ದತ್ತಾಂಶ->req.hd_auth);
  ದತ್ತಾಂಶ->req.hd_auth = auth_ಶೀರ್ಷಿಕೆಗಳು;
  ದತ್ತಾಂಶ->state.authhost.ಮುಗಿದ = TRUE;
  ಫಲಿತಾಂಶ = CURLE_OK;

fail:
  ಆದರೆ(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, ಗಾತ್ರ(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_ತಳ);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&ಒಳಹರಿವು_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
