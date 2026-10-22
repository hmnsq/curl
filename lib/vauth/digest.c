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
 * RFC2831 DIGEST-MD5 authentication
 * RFC7616 DIGEST-SHA256, DIGEST-SHA512-256 authentication
 *
 ***************************************************************************/
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH

#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/digest.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_md5.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sha256.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sha512_256.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SESSION_ALGO 1 /* for algos with this bit set */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALGO_MD5 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALGO_SHA256 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALGO_SHA512_256 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIGEST_QOP_ਮੁੱਲ_AUTH             (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIGEST_QOP_ਮੁੱਲ_AUTH_INT         (1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIGEST_QOP_ਮੁੱਲ_AUTH_CONF        (1 << 2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIGEST_QOP_ਮੁੱਲ_STRING_AUTH      "auth"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIGEST_QOP_ਮੁੱਲ_STRING_AUTH_INT  "auth-int"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIGEST_QOP_ਮੁੱਲ_STRING_AUTH_CONF "auth-conf"
#ਜੇ_ਅੰਤ

bool Curl_auth_digest_get_pair(ਅਚਲ ਅੱਖਰ *str, ਬਣਤਰ Curl_str *ਮੁੱਲ,
                               ਅੱਖਰ *content, ਅਚਲ ਅੱਖਰ **endptr)
{
  ਪੂਰਨ c;
  size_t content_ਲੰਮਾਈ = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  ਜੇ(curlx_str_until(&str, ਮੁੱਲ, DIGEST_MAX_ਮੁੱਲ_ਲੰਮਾਈ, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    ਵਾਪਸ FALSE;

  ਜੇ('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  ਲਈ(c = DIGEST_MAX_CONTENT_ਲੰਮਾਈ; *str && c--; str++) {
    ਜੇ(!escape) {
      ਚੋਣ(*str) {
      ਹਾਲਤ '\\':
        ਜੇ(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          ਜਾਰੀ;
        }
        ਰੋਕੋ;

      ਹਾਲਤ ',':
        ਜੇ(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          ਜਾਰੀ;
        }
        ਰੋਕੋ;

      ਹਾਲਤ '\r':
      ਹਾਲਤ '\n':
        /* end of string */
        ਜੇ(starts_with_quote)
          ਵਾਪਸ FALSE; /* No closing quote */
        c = 0;
        ਜਾਰੀ;

      ਹਾਲਤ '\"':
        ਜੇ(starts_with_quote) {
          /* end of string */
          c = 0;
          ਜਾਰੀ;
        }
        ਨਹੀਂਤਾਂ
          ਵਾਪਸ FALSE;
      }
    }

    ਜੇ(content_ਲੰਮਾਈ >= DIGEST_MAX_CONTENT_ਲੰਮਾਈ - 1) {
      /* no room for this byte plus the terminator */
      ਵਾਪਸ FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_ਲੰਮਾਈ++;
  }

  ਜੇ(escape)
    ਵਾਪਸ FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  ਵਾਪਸ TRUE;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
ਸਥਿਰ ਖਾਲੀ auth_digest_md5_ਮੰਜ਼ਿਲ_ascii(
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸਰੋਤ, /* 16 bytes */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *dest)         /* 33 bytes */
{
  ਪੂਰਨ i;
  ਲਈ(i = 0; i < 16; i++)
    curl_msnprintf((ਅੱਖਰ *)&dest[i * 2], 3, "%02x", ਸਰੋਤ[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
ਸਥਿਰ ਖਾਲੀ auth_digest_sha256_ਮੰਜ਼ਿਲ_ascii(
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸਰੋਤ, /* 32 bytes */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *dest)         /* 65 bytes */
{
  ਪੂਰਨ i;
  ਲਈ(i = 0; i < 32; i++)
    curl_msnprintf((ਅੱਖਰ *)&dest[i * 2], 3, "%02x", ਸਰੋਤ[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
ਸਥਿਰ ਅੱਖਰ *auth_digest_string_quoted(ਅਚਲ ਅੱਖਰ *s)
{
  ਬਣਤਰ dynbuf ਬਾਹਰਲਾ;
  curlx_dyn_init(&ਬਾਹਰਲਾ, 2048);
  ਜੇ(!*s) /* for zero length input, make sure we return an empty string */
    ਵਾਪਸ curlx_strdup("");
  ਜਦੋਂ(*s) {
    CURLcode ਨਤੀਜਾ;
    ਜੇ(*s == '"' || *s == '\\') {
      ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, "\\", 1);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, s, 1);
    }
    ਨਹੀਂਤਾਂ ਜੇ((*s < ' ') || (*s > 0x7e)) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*s);
      ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, buf, 3);
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, s, 1);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਿਰਾ;
    s++;
  }
  ਵਾਪਸ curlx_dyn_ptr(&ਬਾਹਰਲਾ);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
ਸਥਿਰ bool auth_digest_get_ਕੁੰਜੀ_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ *chlg, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ,
                                      ਅੱਖਰ *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  ਕਰੋ {
    ਬਣਤਰ Curl_str ਅੰਕੜੇ;
    ਬਣਤਰ Curl_str name;

    curlx_str_passblanks(&chlg);

    ਜੇ(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      ਪੂਰਨ rc = curlx_str_quotedword(&chlg, &ਅੰਕੜੇ, 256);
      ਜੇ(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &ਅੰਕੜੇ, 256, ',');
      ਜੇ(rc)
        ਵਾਪਸ FALSE; /* weird */

      ਜੇ(curlx_str_cmp(&name, ਕੁੰਜੀ)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&ਅੰਕੜੇ);
        ਅਚਲ ਅੱਖਰ *src = curlx_str(&ਅੰਕੜੇ);
        size_t i;
        size_t outlen = 0;

        ਜੇ(len >= buflen)
          /* does not fit */
          ਵਾਪਸ FALSE;

        ਲਈ(i = 0; i < len; i++) {
          ਜੇ(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        ਵਾਪਸ TRUE;
      }
      ਜੇ(curlx_str_single(&chlg, ','))
        ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ /* odd syntax */
      ਰੋਕੋ;
  } ਜਦੋਂ(1);

  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਖਾਲੀ auth_digest_get_qop_ਮੁੱਲਾਂ(ਅਚਲ ਅੱਖਰ *options, ਪੂਰਨ *ਮੁੱਲ)
{
  ਬਣਤਰ Curl_str ਬਾਹਰਲਾ;
  /* Initialize the output */
  *ਮੁੱਲ = 0;

  ਜਦੋਂ(!curlx_str_until(&options, &ਬਾਹਰਲਾ, 32, ',')) {
    ਜੇ(curlx_str_casecompare(&ਬਾਹਰਲਾ, DIGEST_QOP_ਮੁੱਲ_STRING_AUTH))
      *ਮੁੱਲ |= DIGEST_QOP_ਮੁੱਲ_AUTH;
    ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਬਾਹਰਲਾ, DIGEST_QOP_ਮੁੱਲ_STRING_AUTH_INT))
      *ਮੁੱਲ |= DIGEST_QOP_ਮੁੱਲ_AUTH_INT;
    ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਬਾਹਰਲਾ, DIGEST_QOP_ਮੁੱਲ_STRING_AUTH_CONF))
      *ਮੁੱਲ |= DIGEST_QOP_ਮੁੱਲ_AUTH_CONF;
    ਜੇ(curlx_str_single(&options, ','))
      ਰੋਕੋ;
  }
}

/*
 * auth_decode_digest_md5_message()
 *
 * This is used internally to decode an already encoded DIGEST-MD5 challenge
 * message into the separate attributes.
 *
 * Parameters:
 *
 * chlgref [in]     - The challenge message.
 * nonce   [in/out] - The buffer where the nonce is stored.
 * nlen    [in]     - The length of the nonce buffer.
 * realm   [in/out] - The buffer where the realm is stored.
 * rlen    [in]     - The length of the realm buffer.
 * alg     [in/out] - The buffer where the algorithm is stored.
 * alen    [in]     - The length of the algorithm buffer.
 * qop     [in/out] - The buffer where the qop-options is stored.
 * qlen    [in]     - The length of the qop buffer.
 *
 * Returns CURLE_OK on success.
 */
ਸਥਿਰ CURLcode auth_decode_digest_md5_ਸੁਨੇਹਾ(ਅਚਲ ਬਣਤਰ bufref *chlgref,
                                               ਅੱਖਰ *nonce, size_t nlen,
                                               ਅੱਖਰ *realm, size_t rlen,
                                               ਅੱਖਰ *alg, size_t alen,
                                               ਅੱਖਰ *qop, size_t qlen)
{
  ਅਚਲ ਅੱਖਰ *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  ਜੇ(!Curl_bufref_len(chlgref))
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  ਜੇ(!auth_digest_get_ਕੁੰਜੀ_ਮੁੱਲ(chlg, "nonce", nonce, nlen))
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  ਜੇ(!auth_digest_get_ਕੁੰਜੀ_ਮੁੱਲ(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  ਜੇ(!auth_digest_get_ਕੁੰਜੀ_ਮੁੱਲ(chlg, "algorithm", alg, alen))
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  ਜੇ(!auth_digest_get_ਕੁੰਜੀ_ਮੁੱਲ(chlg, "qop", qop, qlen))
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  ਵਾਪਸ CURLE_OK;
}

/*
 * Curl_auth_is_digest_supported()
 *
 * This is used to evaluate if DIGEST is supported.
 *
 * Parameters: None
 *
 * Returns TRUE as DIGEST as handled by libcurl.
 */
bool Curl_auth_is_digest_supported(ਖਾਲੀ)
{
  ਵਾਪਸ TRUE;
}

/*
 * Curl_auth_create_digest_md5_message()
 *
 * This is used to generate an already encoded DIGEST-MD5 response message
 * ready for sending to the recipient.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * chlg    [in]     - The challenge message.
 * userp   [in]     - The username.
 * passwdp [in]     - The user's password.
 * service [in]     - The service type such as http, smtp, pop or imap.
 * out     [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_digest_md5_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਅਚਲ ਬਣਤਰ bufref *chlg,
                                             ਬਣਤਰ Curl_creds *creds,
                                             ਅਚਲ ਅੱਖਰ *default_service,
                                             ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  ਬਣਤਰ MD5_context *ctxt;
  ਅਚਲ ਅੱਖਰ *userp = Curl_creds_user(creds);
  ਅਚਲ ਅੱਖਰ *passwdp = Curl_creds_passwd(creds);
  ਅੱਖਰ *response = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ digest[MD5_DIGEST_LEN];
  ਅੱਖਰ HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  ਅੱਖਰ HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  ਅੱਖਰ resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  ਅੱਖਰ nonce[64];
  ਅੱਖਰ realm[128];
  ਅੱਖਰ algorithm[64];
  ਅੱਖਰ qop_options[64];
  ਪੂਰਨ qop_ਮੁੱਲਾਂ;
  ਅੱਖਰ cnonce[33];
  ਅੱਖਰ nonce_ਗਿਣਤੀ[] = "00000001";
  ਅੱਖਰ ਵਿਧੀ[]     = "AUTHENTICATE";
  ਅੱਖਰ qop[]        = DIGEST_QOP_ਮੁੱਲ_STRING_AUTH;
  ਅੱਖਰ *spn         = ਨਿਰਾ;
  ਅੱਖਰ *qrealm;
  ਅੱਖਰ *qnonce;
  ਅੱਖਰ *quserp;
  ਅੱਖਰ *qspn;

  /* Decode the challenge message */
  CURLcode ਨਤੀਜਾ = auth_decode_digest_md5_ਸੁਨੇਹਾ(chlg,
                                                   nonce, ਆਕਾਰ(nonce),
                                                   realm, ਆਕਾਰ(realm),
                                                   algorithm,
                                                   ਆਕਾਰ(algorithm),
                                                   qop_options,
                                                   ਆਕਾਰ(qop_options));
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* We only support md5 sessions */
  ਜੇ(strcmp(algorithm, "md5-sess"))
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_ਮੁੱਲਾਂ(qop_options, &qop_ਮੁੱਲਾਂ);

  /* We only support auth quality-of-protection */
  ਜੇ(!(qop_ਮੁੱਲਾਂ & DIGEST_QOP_ਮੁੱਲ_AUTH))
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  ਨਤੀਜਾ = Curl_rand_hex(ਅੰਕੜੇ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)cnonce, ਆਕਾਰ(cnonce));
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ਜੇ(!ctxt)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)userp,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(userp)));
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)":", 1);
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)realm,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(realm)));
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)":", 1);
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)passwdp,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ਜੇ(!ctxt)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)":", 1);
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)nonce,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(nonce)));
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)":", 1);
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)cnonce,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  ਲਈ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, ਅੰਕੜੇ->state.origin->hostname, ਨਿਰਾ);
  ਜੇ(!spn)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ਜੇ(!ctxt) {
    curlx_free(spn);

    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਵਿਧੀ,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(ਵਿਧੀ)));
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)":", 1);
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)spn,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(spn)));
  Curl_MD5_final(ctxt, digest);

  ਲਈ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ਜੇ(!ctxt) {
    curlx_free(spn);

    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)":", 1);
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)nonce,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(nonce)));
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)":", 1);

  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)nonce_ਗਿਣਤੀ,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(nonce_ਗਿਣਤੀ)));
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)":", 1);
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)cnonce,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(cnonce)));
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)":", 1);
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)qop,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(qop)));
  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)":", 1);

  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  ਲਈ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  ਜੇ(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_ਗਿਣਤੀ, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  ਜੇ(!response)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Return the response. */
  Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, response, ਲੜੀ_ਲੰਬਾਈ(response), curl_free);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_auth_decode_digest_http_message()
 *
 * This is used to decode an HTTP DIGEST challenge message into the separate
 * attributes.
 *
 * Parameters:
 *
 * chlg    [in]     - The challenge message.
 * digest  [in/out] - The digest data struct being used and modified.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_decode_digest_http_ਸੁਨੇਹਾ(ਅਚਲ ਅੱਖਰ *chlg,
                                              ਬਣਤਰ digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  ਜੇ(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  ਲਈ(;;) {
    ਬਣਤਰ Curl_str ਮੁੱਲ;
    ਅੱਖਰ content[DIGEST_MAX_CONTENT_ਲੰਮਾਈ];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    ਜੇ(Curl_auth_digest_get_pair(chlg, &ਮੁੱਲ, content, &chlg)) {
      ਜੇ(curlx_str_casecompare(&ਮੁੱਲ, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        ਜੇ(!digest->nonce)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਮੁੱਲ, "stale")) {
        ਜੇ(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਮੁੱਲ, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        ਜੇ(!digest->realm)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਮੁੱਲ, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        ਜੇ(!digest->opaque)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਮੁੱਲ, "qop")) {
        ਅਚਲ ਅੱਖਰ *token = content;
        ਬਣਤਰ Curl_str ਬਾਹਰਲਾ;
        bool ਮਿਲਿਆ_Auth = FALSE;
        bool ਮਿਲਿਆ_Auth_Int = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        ਜਦੋਂ(!curlx_str_until(&token, &ਬਾਹਰਲਾ, 32, ',')) {
          ਜੇ(curlx_str_casecompare(&ਬਾਹਰਲਾ, DIGEST_QOP_ਮੁੱਲ_STRING_AUTH))
            ਮਿਲਿਆ_Auth = TRUE;
          ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਬਾਹਰਲਾ,
                                        DIGEST_QOP_ਮੁੱਲ_STRING_AUTH_INT))
            ਮਿਲਿਆ_Auth_Int = TRUE;
          ਜੇ(curlx_str_single(&token, ','))
            ਰੋਕੋ;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        ਜੇ(ਮਿਲਿਆ_Auth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_ਮੁੱਲ_STRING_AUTH);
          ਜੇ(!digest->qop)
            ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        }
        ਨਹੀਂਤਾਂ ਜੇ(ਮਿਲਿਆ_Auth_Int) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_ਮੁੱਲ_STRING_AUTH_INT);
          ਜੇ(!digest->qop)
            ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        }
      }
      ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਮੁੱਲ, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        ਜੇ(!digest->algorithm)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

        ਜੇ(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(content, "SHA-512-256")) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#ਨਹੀਂਤਾਂ /* !CURL_HAVE_SHA512_256 */
          ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ /* CURL_HAVE_SHA512_256 */
        }
        ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(content, "SHA-512-256-SESS")) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#ਨਹੀਂਤਾਂ /* !CURL_HAVE_SHA512_256 */
          ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ /* CURL_HAVE_SHA512_256 */
        }
        ਨਹੀਂਤਾਂ
          ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
      }
      ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਮੁੱਲ, "userhash")) {
        ਜੇ(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      ਨਹੀਂਤਾਂ {
        /* Unknown specifier, ignore it! */
      }
    }
    ਨਹੀਂਤਾਂ
      ਰੋਕੋ; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (ਖਾਲੀ)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  ਜੇ(before && !digest->stale)
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  ਜੇ(!digest->nonce)
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  ਜੇ(!digest->qop && (digest->algo & SESSION_ALGO))
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  ਵਾਪਸ CURLE_OK;
}

/*
 * auth_create_digest_http_message()
 *
 * This is used to generate an HTTP DIGEST response message ready for sending
 * to the recipient.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * creds   [in]     - The credentials
 * request [in]     - The HTTP request.
 * uripath [in]     - The path of the HTTP uri.
 * digest  [in/out] - The digest data struct being used and modified.
 * outptr  [in/out] - The address where a pointer to newly allocated memory
 *                    holding the result is stored upon completion.
 * outlen  [out]    - The length of the output message.
 *
 * Returns CURLE_OK on success.
 */
ਸਥਿਰ CURLcode auth_create_digest_http_ਸੁਨੇਹਾ(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ Curl_creds *creds,
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *request,
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *uripath,
  ਬਣਤਰ digestdata *digest,
  ਅੱਖਰ **outptr, size_t *outlen,
  ਖਾਲੀ (*convert_ਮੰਜ਼ਿਲ_ascii)(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *),
  CURLcode (*hash)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *, ਅਚਲ size_t))
{
  CURLcode ਨਤੀਜਾ;
  ਅਚਲ ਅੱਖਰ *userp = Curl_creds_user(creds);
  ਅਚਲ ਅੱਖਰ *passwdp = Curl_creds_passwd(creds);
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ hashbuf[32]; /* 32 bytes/256 bits */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ request_digest[65];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ha1[65];    /* 64 digits and 1 zero byte */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ha2[65];    /* 64 digits and 1 zero byte */
  ਅੱਖਰ userh[65];
  ਅੱਖਰ *cnonce = ਨਿਰਾ;
  size_t cnonce_sz = 0;
  ਅੱਖਰ *userp_quoted = ਨਿਰਾ;
  ਅੱਖਰ *realm_quoted = ਨਿਰਾ;
  ਅੱਖਰ *nonce_quoted = ਨਿਰਾ;
  ਅੱਖਰ *hashthis = ਨਿਰਾ;
  ਅੱਖਰ *uri_quoted = ਨਿਰਾ;
  ਬਣਤਰ dynbuf response;
  *outptr = ਨਿਰਾ;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, ਆਕਾਰ(hashbuf));
  ਜੇ(!digest->nc)
    digest->nc = 1;

  ਜੇ(!digest->cnonce) {
    ਅੱਖਰ cnoncebuf[12];
    ਨਤੀਜਾ = Curl_rand_ਬਾਈਟਾਂ(ਅੰਕੜੇ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
                             TRUE,
#ਜੇ_ਅੰਤ
                             (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)cnoncebuf,
                             ਆਕਾਰ(cnoncebuf));
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_base64_encode((uint8_t *)cnoncebuf, ਆਕਾਰ(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ oom;

    digest->cnonce = cnonce;
  }

  ਜੇ(digest->userhash) {
    ਅੱਖਰ *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    ਜੇ(!hasht) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ oom;
    }

    ਨਤੀਜਾ = hash(hashbuf, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)hasht, ਲੜੀ_ਲੰਬਾਈ(hasht));
    curlx_free(hasht);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ oom;
    convert_ਮੰਜ਼ਿਲ_ascii(hashbuf, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  ਜੇ(!hashthis) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ oom;
  }

  ਨਤੀਜਾ = hash(hashbuf, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)hashthis, ਲੜੀ_ਲੰਬਾਈ(hashthis));
  curlx_free(hashthis);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ oom;
  convert_ਮੰਜ਼ਿਲ_ascii(hashbuf, ha1);

  ਜੇ(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    ਅੱਖਰ *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    ਜੇ(!tmp) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ oom;
    }

    ਨਤੀਜਾ = hash(hashbuf, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)tmp, ਲੜੀ_ਲੰਬਾਈ(tmp));
    curlx_free(tmp);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ oom;
    convert_ਮੰਜ਼ਿਲ_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ਅਚਲ ਅੱਖਰ *)uripath);
  ਜੇ(!uri_quoted) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  ਜੇ(!hashthis) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ oom;
  }

  ਜੇ(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    ਅੱਖਰ hashed[65];
    ਅੱਖਰ *hashthis2;

    ਨਤੀਜਾ = hash(hashbuf, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)"", 0);
    ਜੇ(ਨਤੀਜਾ) {
      curlx_free(hashthis);
      ਜਾਓ oom;
    }
    convert_ਮੰਜ਼ਿਲ_ascii(hashbuf, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    ਜੇ(!hashthis) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ oom;
    }
  }

  ਨਤੀਜਾ = hash(hashbuf, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)hashthis, ਲੜੀ_ਲੰਬਾਈ(hashthis));
  curlx_free(hashthis);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ oom;
  convert_ਮੰਜ਼ਿਲ_ascii(hashbuf, ha2);

  ਜੇ(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  ਨਹੀਂਤਾਂ
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  ਜੇ(!hashthis) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ oom;
  }

  ਨਤੀਜਾ = hash(hashbuf, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)hashthis, ਲੜੀ_ਲੰਬਾਈ(hashthis));
  curlx_free(hashthis);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ oom;
  convert_ਮੰਜ਼ਿਲ_ascii(hashbuf, request_digest);

  /* For test case 64 (snooped from a Mozilla 1.3a request)

     Authorization: Digest username="testuser", realm="testrealm", \
     nonce="1053604145", uri="/64", response="c55f7f30d83d774a3d2dcacf725abaca"

     Digest parameters are all quoted strings. Username which is provided by
     the user needs double quotes and backslashes within it escaped.
     realm, nonce, and opaque needs backslashes as well as they were
     de-escaped when copied from request header. cnonce is generated with
     web-safe characters. uri is already percent encoded. nc is 8 hex
     characters. algorithm and qop with standard values only contain web-safe
     characters.
   */
  userp_quoted = auth_digest_string_quoted(digest->userhash ? userh : userp);
  ਜੇ(!userp_quoted) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ oom;
  }
  ਜੇ(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  ਨਹੀਂਤਾਂ {
    realm_quoted = curlx_malloc(1);
    ਜੇ(realm_quoted)
      realm_quoted[0] = 0;
  }
  ਜੇ(!realm_quoted) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  ਜੇ(!nonce_quoted) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ oom;
  }

  ਜੇ(digest->qop) {
    ਨਤੀਜਾ = curlx_dyn_addf(&response, "username=\"%s\", "
                            "realm=\"%s\", "
                            "nonce=\"%s\", "
                            "uri=\"%s\", "
                            "cnonce=\"%s\", "
                            "nc=%08x, "
                            "qop=%s, "
                            "response=\"%s\"",
                            userp_quoted,
                            realm_quoted,
                            nonce_quoted,
                            uri_quoted,
                            digest->cnonce,
                            (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = curlx_dyn_addf(&response, "username=\"%s\", "
                            "realm=\"%s\", "
                            "nonce=\"%s\", "
                            "uri=\"%s\", "
                            "response=\"%s\"",
                            userp_quoted,
                            realm_quoted,
                            nonce_quoted,
                            uri_quoted,
                            request_digest);
  }
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ oom;

  /* Add the optional fields */
  ਜੇ(digest->opaque) {
    /* Append the opaque */
    ਅੱਖਰ *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    ਜੇ(!opaque_quoted) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ oom;
    }
    ਨਤੀਜਾ = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ oom;
  }

  ਜੇ(digest->algorithm) {
    /* Append the algorithm */
    ਨਤੀਜਾ = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ oom;
  }

  ਜੇ(digest->userhash) {
    /* Append the userhash */
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&response, ", userhash=true");
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ oom;
  }

  /* Return the output */
  *outptr = curlx_dyn_ptr(&response);
  *outlen = curlx_dyn_len(&response);
  ਨਤੀਜਾ = CURLE_OK;

oom:
  curlx_free(nonce_quoted);
  curlx_free(realm_quoted);
  curlx_free(uri_quoted);
  curlx_free(userp_quoted);
  ਜੇ(ਨਤੀਜਾ)
    curlx_dyn_free(&response);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_auth_create_digest_http_message()
 *
 * This is used to generate an HTTP DIGEST response message ready for sending
 * to the recipient.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * userp   [in]     - The username.
 * passwdp [in]     - The user's password.
 * request [in]     - The HTTP request.
 * uripath [in]     - The path of the HTTP uri.
 * digest  [in/out] - The digest data struct being used and modified.
 * outptr  [in/out] - The address where a pointer to newly allocated memory
 *                    holding the result is stored upon completion.
 * outlen  [out]    - The length of the output message.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_digest_http_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਬਣਤਰ Curl_creds *creds,
                                              ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *request,
                                              ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *uripath,
                                              ਬਣਤਰ digestdata *digest,
                                              ਅੱਖਰ **outptr, size_t *outlen)
{
  ਜੇ(digest->algo <= ALGO_MD5SESS)
    ਵਾਪਸ auth_create_digest_http_ਸੁਨੇਹਾ(ਅੰਕੜੇ, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_ਮੰਜ਼ਿਲ_ascii,
                                           Curl_md5it);

  ਜੇ(digest->algo <= ALGO_SHA256SESS)
    ਵਾਪਸ auth_create_digest_http_ਸੁਨੇਹਾ(ਅੰਕੜੇ, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_ਮੰਜ਼ਿਲ_ascii,
                                           Curl_sha256it);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_SHA512_256
  ਜੇ(digest->algo <= ALGO_SHA512_256SESS)
    ਵਾਪਸ auth_create_digest_http_ਸੁਨੇਹਾ(ਅੰਕੜੇ, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_ਮੰਜ਼ਿਲ_ascii,
                                           Curl_sha512_256it);
#ਜੇ_ਅੰਤ /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
}

/*
 * Curl_auth_digest_cleanup()
 *
 * This is used to clean up the digest specific data.
 *
 * Parameters:
 *
 * digest    [in/out] - The digest data struct being cleaned up.
 *
 */
ਖਾਲੀ Curl_auth_digest_cleanup(ਬਣਤਰ digestdata *digest)
{
  Curl_peer_unlink(&digest->origin);
  Curl_creds_unlink(&digest->creds);
  curlx_safefree(digest->nonce);
  curlx_safefree(digest->cnonce);
  curlx_safefree(digest->realm);
  curlx_safefree(digest->opaque);
  curlx_safefree(digest->qop);
  curlx_safefree(digest->algorithm);

  digest->nc = 0;
  digest->algo = ALGO_MD5; /* default algorithm */
  digest->stale = FALSE;   /* default means normal, not stale */
  digest->userhash = FALSE;
}
#ਜੇ_ਅੰਤ /* !USE_WINDOWS_SSPI */

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_DIGEST_AUTH */
