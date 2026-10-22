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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "vauth/digest.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "curl_md5.h"
#ಅನ್ತರ್ಭಾವ "curl_sha256.h"
#ಅನ್ತರ್ಭಾವ "curl_sha512_256.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "escape.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINDOWS_SSPI
#ವ್ಯಾಖ್ಯಾನಿಸು SESSION_ALGO 1 /* for algos with this bit set */

#ವ್ಯಾಖ್ಯಾನಿಸು ALGO_MD5 0
#ವ್ಯಾಖ್ಯಾನಿಸು ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#ವ್ಯಾಖ್ಯಾನಿಸು ALGO_SHA256 2
#ವ್ಯಾಖ್ಯಾನಿಸು ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#ವ್ಯಾಖ್ಯಾನಿಸು ALGO_SHA512_256 4
#ವ್ಯಾಖ್ಯಾನಿಸು ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#ವ್ಯಾಖ್ಯಾನಿಸು DIGEST_QOP_ಮೌಲ್ಯ_AUTH             (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು DIGEST_QOP_ಮೌಲ್ಯ_AUTH_INT         (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು DIGEST_QOP_ಮೌಲ್ಯ_AUTH_CONF        (1 << 2)

#ವ್ಯಾಖ್ಯಾನಿಸು DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH      "auth"
#ವ್ಯಾಖ್ಯಾನಿಸು DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_INT  "auth-int"
#ವ್ಯಾಖ್ಯಾನಿಸು DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_CONF "auth-conf"
#ಆದರೆ_ಅಂತ್ಯ

bool Curl_auth_digest_get_pair(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ರಚನೆ Curl_str *ಮೌಲ್ಯ,
                               ಅಕ್ಷರ *content, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **endptr)
{
  ಪೂರ್ಣಾಂಕ c;
  size_t content_ಉದ್ದಳತೆ = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  ಆದರೆ(curlx_str_until(&str, ಮೌಲ್ಯ, DIGEST_MAX_ಮೌಲ್ಯ_ಉದ್ದಳತೆ, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  ಗಾಗಿ(c = DIGEST_MAX_CONTENT_ಉದ್ದಳತೆ; *str && c--; str++) {
    ಆದರೆ(!escape) {
      ಆಯ್ಕೆ(*str) {
      ಸ್ಥಿತಿ '\\':
        ಆದರೆ(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          ಮುಂದುವರಿ;
        }
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ ',':
        ಆದರೆ(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          ಮುಂದುವರಿ;
        }
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ '\r':
      ಸ್ಥಿತಿ '\n':
        /* end of string */
        ಆದರೆ(starts_with_quote)
          ಹಿಂತಿರುಗಿ FALSE; /* No closing quote */
        c = 0;
        ಮುಂದುವರಿ;

      ಸ್ಥಿತಿ '\"':
        ಆದರೆ(starts_with_quote) {
          /* end of string */
          c = 0;
          ಮುಂದುವರಿ;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          ಹಿಂತಿರುಗಿ FALSE;
      }
    }

    ಆದರೆ(content_ಉದ್ದಳತೆ >= DIGEST_MAX_CONTENT_ಉದ್ದಳತೆ - 1) {
      /* no room for this byte plus the terminator */
      ಹಿಂತಿರುಗಿ FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_ಉದ್ದಳತೆ++;
  }

  ಆದರೆ(escape)
    ಹಿಂತಿರುಗಿ FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  ಹಿಂತಿರುಗಿ TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
ಸ್ಥಿರ ಶೂನ್ಯ auth_digest_md5_ಗಮ್ಯ_ascii(
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಮೂಲ, /* 16 bytes */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *dest)         /* 33 bytes */
{
  ಪೂರ್ಣಾಂಕ i;
  ಗಾಗಿ(i = 0; i < 16; i++)
    curl_msnprintf((ಅಕ್ಷರ *)&dest[i * 2], 3, "%02x", ಮೂಲ[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
ಸ್ಥಿರ ಶೂನ್ಯ auth_digest_sha256_ಗಮ್ಯ_ascii(
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಮೂಲ, /* 32 bytes */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *dest)         /* 65 bytes */
{
  ಪೂರ್ಣಾಂಕ i;
  ಗಾಗಿ(i = 0; i < 32; i++)
    curl_msnprintf((ಅಕ್ಷರ *)&dest[i * 2], 3, "%02x", ಮೂಲ[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
ಸ್ಥಿರ ಅಕ್ಷರ *auth_digest_string_quoted(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
  ರಚನೆ dynbuf ಹೊರಹೋಗು;
  curlx_dyn_init(&ಹೊರಹೋಗು, 2048);
  ಆದರೆ(!*s) /* for zero length input, make sure we return an empty string */
    ಹಿಂತಿರುಗಿ curlx_strdup("");
  ಆಗ(*s) {
    CURLcode ಫಲಿತಾಂಶ;
    ಆದರೆ(*s == '"' || *s == '\\') {
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, "\\", 1);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, s, 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((*s < ' ') || (*s > 0x7e)) {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*s);
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, buf, 3);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, s, 1);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    s++;
  }
  ಹಿಂತಿರುಗಿ curlx_dyn_ptr(&ಹೊರಹೋಗು);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
ಸ್ಥಿರ bool auth_digest_get_ಕೀಲಿ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chlg, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ,
                                      ಅಕ್ಷರ *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  ಮಾಡು {
    ರಚನೆ Curl_str ದತ್ತಾಂಶ;
    ರಚನೆ Curl_str name;

    curlx_str_passblanks(&chlg);

    ಆದರೆ(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      ಪೂರ್ಣಾಂಕ rc = curlx_str_quotedword(&chlg, &ದತ್ತಾಂಶ, 256);
      ಆದರೆ(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &ದತ್ತಾಂಶ, 256, ',');
      ಆದರೆ(rc)
        ಹಿಂತಿರುಗಿ FALSE; /* weird */

      ಆದರೆ(curlx_str_cmp(&name, ಕೀಲಿ)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&ದತ್ತಾಂಶ);
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src = curlx_str(&ದತ್ತಾಂಶ);
        size_t i;
        size_t outlen = 0;

        ಆದರೆ(len >= buflen)
          /* does not fit */
          ಹಿಂತಿರುಗಿ FALSE;

        ಗಾಗಿ(i = 0; i < len; i++) {
          ಆದರೆ(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        ಹಿಂತಿರುಗಿ TRUE;
      }
      ಆದರೆ(curlx_str_single(&chlg, ','))
        ಹಿಂತಿರುಗಿ FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ /* odd syntax */
      ನಿಲ್ಲಿಸು;
  } ಆಗ(1);

  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ auth_digest_get_qop_ಮೌಲ್ಯಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *options, ಪೂರ್ಣಾಂಕ *ಮೌಲ್ಯ)
{
  ರಚನೆ Curl_str ಹೊರಹೋಗು;
  /* Initialize the output */
  *ಮೌಲ್ಯ = 0;

  ಆಗ(!curlx_str_until(&options, &ಹೊರಹೋಗು, 32, ',')) {
    ಆದರೆ(curlx_str_casecompare(&ಹೊರಹೋಗು, DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH))
      *ಮೌಲ್ಯ |= DIGEST_QOP_ಮೌಲ್ಯ_AUTH;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಹೊರಹೋಗು, DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_INT))
      *ಮೌಲ್ಯ |= DIGEST_QOP_ಮೌಲ್ಯ_AUTH_INT;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಹೊರಹೋಗು, DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_CONF))
      *ಮೌಲ್ಯ |= DIGEST_QOP_ಮೌಲ್ಯ_AUTH_CONF;
    ಆದರೆ(curlx_str_single(&options, ','))
      ನಿಲ್ಲಿಸು;
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
ಸ್ಥಿರ CURLcode auth_decode_digest_md5_ಸಂದೇಶ(ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlgref,
                                               ಅಕ್ಷರ *nonce, size_t nlen,
                                               ಅಕ್ಷರ *realm, size_t rlen,
                                               ಅಕ್ಷರ *alg, size_t alen,
                                               ಅಕ್ಷರ *qop, size_t qlen)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  ಆದರೆ(!Curl_bufref_len(chlgref))
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  ಆದರೆ(!auth_digest_get_ಕೀಲಿ_ಮೌಲ್ಯ(chlg, "nonce", nonce, nlen))
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  ಆದರೆ(!auth_digest_get_ಕೀಲಿ_ಮೌಲ್ಯ(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  ಆದರೆ(!auth_digest_get_ಕೀಲಿ_ಮೌಲ್ಯ(chlg, "algorithm", alg, alen))
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  ಆದರೆ(!auth_digest_get_ಕೀಲಿ_ಮೌಲ್ಯ(chlg, "qop", qop, qlen))
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
bool Curl_auth_is_digest_supported(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ TRUE;
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
CURLcode Curl_auth_create_digest_md5_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                                             ರಚನೆ Curl_creds *creds,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                             ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  ರಚನೆ MD5_context *ctxt;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *userp = Curl_creds_user(creds);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwdp = Curl_creds_passwd(creds);
  ಅಕ್ಷರ *response = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ digest[MD5_DIGEST_LEN];
  ಅಕ್ಷರ HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  ಅಕ್ಷರ HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  ಅಕ್ಷರ resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  ಅಕ್ಷರ nonce[64];
  ಅಕ್ಷರ realm[128];
  ಅಕ್ಷರ algorithm[64];
  ಅಕ್ಷರ qop_options[64];
  ಪೂರ್ಣಾಂಕ qop_ಮೌಲ್ಯಗಳು;
  ಅಕ್ಷರ cnonce[33];
  ಅಕ್ಷರ nonce_ಎಣಿಕೆ[] = "00000001";
  ಅಕ್ಷರ ವಿಧಾನ[]     = "AUTHENTICATE";
  ಅಕ್ಷರ qop[]        = DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH;
  ಅಕ್ಷರ *spn         = ಖಾಲಿ;
  ಅಕ್ಷರ *qrealm;
  ಅಕ್ಷರ *qnonce;
  ಅಕ್ಷರ *quserp;
  ಅಕ್ಷರ *qspn;

  /* Decode the challenge message */
  CURLcode ಫಲಿತಾಂಶ = auth_decode_digest_md5_ಸಂದೇಶ(chlg,
                                                   nonce, ಗಾತ್ರ(nonce),
                                                   realm, ಗಾತ್ರ(realm),
                                                   algorithm,
                                                   ಗಾತ್ರ(algorithm),
                                                   qop_options,
                                                   ಗಾತ್ರ(qop_options));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* We only support md5 sessions */
  ಆದರೆ(strcmp(algorithm, "md5-sess"))
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_ಮೌಲ್ಯಗಳು(qop_options, &qop_ಮೌಲ್ಯಗಳು);

  /* We only support auth quality-of-protection */
  ಆದರೆ(!(qop_ಮೌಲ್ಯಗಳು & DIGEST_QOP_ಮೌಲ್ಯ_AUTH))
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  ಫಲಿತಾಂಶ = Curl_rand_hex(ದತ್ತಾಂಶ, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)cnonce, ಗಾತ್ರ(cnonce));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ಆದರೆ(!ctxt)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)userp,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(userp)));
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)":", 1);
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)realm,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(realm)));
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)":", 1);
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)passwdp,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ಆದರೆ(!ctxt)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)":", 1);
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)nonce,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(nonce)));
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)":", 1);
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)cnonce,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  ಗಾಗಿ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, ದತ್ತಾಂಶ->state.origin->hostname, ಖಾಲಿ);
  ಆದರೆ(!spn)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ಆದರೆ(!ctxt) {
    curlx_free(spn);

    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ವಿಧಾನ,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ವಿಧಾನ)));
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)":", 1);
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)spn,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(spn)));
  Curl_MD5_final(ctxt, digest);

  ಗಾಗಿ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ಆದರೆ(!ctxt) {
    curlx_free(spn);

    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)":", 1);
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)nonce,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(nonce)));
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)":", 1);

  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)nonce_ಎಣಿಕೆ,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(nonce_ಎಣಿಕೆ)));
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)":", 1);
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)cnonce,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cnonce)));
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)":", 1);
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)qop,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(qop)));
  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)":", 1);

  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  ಗಾಗಿ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  ಆದರೆ(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_ಎಣಿಕೆ, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  ಆದರೆ(!response)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Return the response. */
  Curl_bufref_ಗಣ(ಹೊರಹೋಗು, response, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(response), curl_free);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
CURLcode Curl_auth_decode_digest_http_ಸಂದೇಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chlg,
                                              ರಚನೆ digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  ಆದರೆ(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  ಗಾಗಿ(;;) {
    ರಚನೆ Curl_str ಮೌಲ್ಯ;
    ಅಕ್ಷರ content[DIGEST_MAX_CONTENT_ಉದ್ದಳತೆ];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    ಆದರೆ(Curl_auth_digest_get_pair(chlg, &ಮೌಲ್ಯ, content, &chlg)) {
      ಆದರೆ(curlx_str_casecompare(&ಮೌಲ್ಯ, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        ಆದರೆ(!digest->nonce)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಮೌಲ್ಯ, "stale")) {
        ಆದರೆ(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಮೌಲ್ಯ, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        ಆದರೆ(!digest->realm)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಮೌಲ್ಯ, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        ಆದರೆ(!digest->opaque)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಮೌಲ್ಯ, "qop")) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *token = content;
        ರಚನೆ Curl_str ಹೊರಹೋಗು;
        bool ಸಿಕ್ಕಿತು_Auth = FALSE;
        bool ಸಿಕ್ಕಿತು_Auth_Int = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        ಆಗ(!curlx_str_until(&token, &ಹೊರಹೋಗು, 32, ',')) {
          ಆದರೆ(curlx_str_casecompare(&ಹೊರಹೋಗು, DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH))
            ಸಿಕ್ಕಿತು_Auth = TRUE;
          ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಹೊರಹೋಗು,
                                        DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_INT))
            ಸಿಕ್ಕಿತು_Auth_Int = TRUE;
          ಆದರೆ(curlx_str_single(&token, ','))
            ನಿಲ್ಲಿಸು;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        ಆದರೆ(ಸಿಕ್ಕಿತು_Auth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH);
          ಆದರೆ(!digest->qop)
            ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸಿಕ್ಕಿತು_Auth_Int) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_INT);
          ಆದರೆ(!digest->qop)
            ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಮೌಲ್ಯ, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        ಆದರೆ(!digest->algorithm)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

        ಆದರೆ(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(content, "SHA-512-256")) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#ಇಲ್ಲದಿದ್ದರೆ /* !CURL_HAVE_SHA512_256 */
          ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ /* CURL_HAVE_SHA512_256 */
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(content, "SHA-512-256-SESS")) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#ಇಲ್ಲದಿದ್ದರೆ /* !CURL_HAVE_SHA512_256 */
          ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ /* CURL_HAVE_SHA512_256 */
        }
        ಇಲ್ಲದಿದ್ದರೆ
          ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಮೌಲ್ಯ, "userhash")) {
        ಆದರೆ(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* Unknown specifier, ignore it! */
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ನಿಲ್ಲಿಸು; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (ಶೂನ್ಯ)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  ಆದರೆ(before && !digest->stale)
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  ಆದರೆ(!digest->nonce)
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  ಆದರೆ(!digest->qop && (digest->algo & SESSION_ALGO))
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
ಸ್ಥಿರ CURLcode auth_create_digest_http_ಸಂದೇಶ(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ Curl_creds *creds,
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *request,
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *uripath,
  ರಚನೆ digestdata *digest,
  ಅಕ್ಷರ **outptr, size_t *outlen,
  ಶೂನ್ಯ (*convert_ಗಮ್ಯ_ascii)(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *),
  CURLcode (*hash)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ size_t))
{
  CURLcode ಫಲಿತಾಂಶ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *userp = Curl_creds_user(creds);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwdp = Curl_creds_passwd(creds);
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ hashbuf[32]; /* 32 bytes/256 bits */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ request_digest[65];
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ha1[65];    /* 64 digits and 1 zero byte */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ha2[65];    /* 64 digits and 1 zero byte */
  ಅಕ್ಷರ userh[65];
  ಅಕ್ಷರ *cnonce = ಖಾಲಿ;
  size_t cnonce_sz = 0;
  ಅಕ್ಷರ *userp_quoted = ಖಾಲಿ;
  ಅಕ್ಷರ *realm_quoted = ಖಾಲಿ;
  ಅಕ್ಷರ *nonce_quoted = ಖಾಲಿ;
  ಅಕ್ಷರ *hashthis = ಖಾಲಿ;
  ಅಕ್ಷರ *uri_quoted = ಖಾಲಿ;
  ರಚನೆ dynbuf response;
  *outptr = ಖಾಲಿ;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, ಗಾತ್ರ(hashbuf));
  ಆದರೆ(!digest->nc)
    digest->nc = 1;

  ಆದರೆ(!digest->cnonce) {
    ಅಕ್ಷರ cnoncebuf[12];
    ಫಲಿತಾಂಶ = Curl_rand_ಬೈಟುಗಳು(ದತ್ತಾಂಶ,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
                             TRUE,
#ಆದರೆ_ಅಂತ್ಯ
                             (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)cnoncebuf,
                             ಗಾತ್ರ(cnoncebuf));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_base64_encode((uint8_t *)cnoncebuf, ಗಾತ್ರ(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು oom;

    digest->cnonce = cnonce;
  }

  ಆದರೆ(digest->userhash) {
    ಅಕ್ಷರ *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    ಆದರೆ(!hasht) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು oom;
    }

    ಫಲಿತಾಂಶ = hash(hashbuf, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)hasht, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hasht));
    curlx_free(hasht);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು oom;
    convert_ಗಮ್ಯ_ascii(hashbuf, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  ಆದರೆ(!hashthis) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು oom;
  }

  ಫಲಿತಾಂಶ = hash(hashbuf, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)hashthis, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hashthis));
  curlx_free(hashthis);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು oom;
  convert_ಗಮ್ಯ_ascii(hashbuf, ha1);

  ಆದರೆ(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    ಅಕ್ಷರ *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    ಆದರೆ(!tmp) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು oom;
    }

    ಫಲಿತಾಂಶ = hash(hashbuf, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)tmp, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tmp));
    curlx_free(tmp);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು oom;
    convert_ಗಮ್ಯ_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)uripath);
  ಆದರೆ(!uri_quoted) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  ಆದರೆ(!hashthis) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು oom;
  }

  ಆದರೆ(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    ಅಕ್ಷರ hashed[65];
    ಅಕ್ಷರ *hashthis2;

    ಫಲಿತಾಂಶ = hash(hashbuf, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)"", 0);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      curlx_free(hashthis);
      ಹೋಗು oom;
    }
    convert_ಗಮ್ಯ_ascii(hashbuf, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    ಆದರೆ(!hashthis) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು oom;
    }
  }

  ಫಲಿತಾಂಶ = hash(hashbuf, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)hashthis, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hashthis));
  curlx_free(hashthis);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು oom;
  convert_ಗಮ್ಯ_ascii(hashbuf, ha2);

  ಆದರೆ(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  ಇಲ್ಲದಿದ್ದರೆ
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  ಆದರೆ(!hashthis) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು oom;
  }

  ಫಲಿತಾಂಶ = hash(hashbuf, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)hashthis, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hashthis));
  curlx_free(hashthis);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು oom;
  convert_ಗಮ್ಯ_ascii(hashbuf, request_digest);

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
  ಆದರೆ(!userp_quoted) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು oom;
  }
  ಆದರೆ(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  ಇಲ್ಲದಿದ್ದರೆ {
    realm_quoted = curlx_malloc(1);
    ಆದರೆ(realm_quoted)
      realm_quoted[0] = 0;
  }
  ಆದರೆ(!realm_quoted) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  ಆದರೆ(!nonce_quoted) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು oom;
  }

  ಆದರೆ(digest->qop) {
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&response, "username=\"%s\", "
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
                            (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&response, "username=\"%s\", "
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
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು oom;

  /* Add the optional fields */
  ಆದರೆ(digest->opaque) {
    /* Append the opaque */
    ಅಕ್ಷರ *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    ಆದರೆ(!opaque_quoted) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು oom;
    }
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು oom;
  }

  ಆದರೆ(digest->algorithm) {
    /* Append the algorithm */
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು oom;
  }

  ಆದರೆ(digest->userhash) {
    /* Append the userhash */
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&response, ", userhash=true");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು oom;
  }

  /* Return the output */
  *outptr = curlx_dyn_ptr(&response);
  *outlen = curlx_dyn_len(&response);
  ಫಲಿತಾಂಶ = CURLE_OK;

oom:
  curlx_free(nonce_quoted);
  curlx_free(realm_quoted);
  curlx_free(uri_quoted);
  curlx_free(userp_quoted);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    curlx_dyn_free(&response);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
CURLcode Curl_auth_create_digest_http_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ರಚನೆ Curl_creds *creds,
                                              ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *request,
                                              ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *uripath,
                                              ರಚನೆ digestdata *digest,
                                              ಅಕ್ಷರ **outptr, size_t *outlen)
{
  ಆದರೆ(digest->algo <= ALGO_MD5SESS)
    ಹಿಂತಿರುಗಿ auth_create_digest_http_ಸಂದೇಶ(ದತ್ತಾಂಶ, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_ಗಮ್ಯ_ascii,
                                           Curl_md5it);

  ಆದರೆ(digest->algo <= ALGO_SHA256SESS)
    ಹಿಂತಿರುಗಿ auth_create_digest_http_ಸಂದೇಶ(ದತ್ತಾಂಶ, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_ಗಮ್ಯ_ascii,
                                           Curl_sha256it);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_SHA512_256
  ಆದರೆ(digest->algo <= ALGO_SHA512_256SESS)
    ಹಿಂತಿರುಗಿ auth_create_digest_http_ಸಂದೇಶ(ದತ್ತಾಂಶ, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_ಗಮ್ಯ_ascii,
                                           Curl_sha512_256it);
#ಆದರೆ_ಅಂತ್ಯ /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
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
ಶೂನ್ಯ Curl_auth_digest_cleanup(ರಚನೆ digestdata *digest)
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
#ಆದರೆ_ಅಂತ್ಯ /* !USE_WINDOWS_SSPI */

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_DIGEST_AUTH */
