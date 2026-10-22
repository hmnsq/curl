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

#ifndef CURL_DISABLE_DIGEST_AUTH

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "vauth/digest.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "curl_md5.h"
#ಅನ್ತರ್ಭಾವ "curl_sha256.h"
#ಅನ್ತರ್ಭಾವ "curl_sha512_256.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "escape.h"

#ifndef USE_WINDOWS_SSPI
#define SESSION_ALGO 1 /* for algos with this bit set */

#define ALGO_MD5 0
#define ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#define ALGO_SHA256 2
#define ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#define ALGO_SHA512_256 4
#define ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#define DIGEST_QOP_ಮೌಲ್ಯ_AUTH             (1 << 0)
#define DIGEST_QOP_ಮೌಲ್ಯ_AUTH_INT         (1 << 1)
#define DIGEST_QOP_ಮೌಲ್ಯ_AUTH_CONF        (1 << 2)

#define DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH      "auth"
#define DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_INT  "auth-int"
#define DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_CONF "auth-conf"
#endif

bool Curl_auth_digest_get_pair(ಧ್ರುವ ವರ್ಣ *str, ಸಂರಚನಾ Curl_str *ಮೌಲ್ಯ,
                               ವರ್ಣ *content, ಧ್ರುವ ವರ್ಣ **endptr)
{
  ಪೂರ್ಣಾಙ್ಕ c;
  size_t content_length = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  ಯದಿ(curlx_str_until(&str, ಮೌಲ್ಯ, DIGEST_MAX_ಮೌಲ್ಯ_LENGTH, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಯದಿ('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  ಪ್ರತಿ(c = DIGEST_MAX_CONTENT_LENGTH; *str && c--; str++) {
    ಯದಿ(!escape) {
      ವಿಕಲ್ಪ(*str) {
      ಪಕ್ಷ '\\':
        ಯದಿ(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          ಅನುವರ್ತ;
        }
        ಭಙ್ಗ;

      ಪಕ್ಷ ',':
        ಯದಿ(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          ಅನುವರ್ತ;
        }
        ಭಙ್ಗ;

      ಪಕ್ಷ '\r':
      ಪಕ್ಷ '\n':
        /* end of string */
        ಯದಿ(starts_with_quote)
          ಪ್ರತ್ಯಾಗಮ FALSE; /* No closing quote */
        c = 0;
        ಅನುವರ್ತ;

      ಪಕ್ಷ '\"':
        ಯದಿ(starts_with_quote) {
          /* end of string */
          c = 0;
          ಅನುವರ್ತ;
        }
        ಅನ್ಯಥಾ
          ಪ್ರತ್ಯಾಗಮ FALSE;
      }
    }

    ಯದಿ(content_length >= DIGEST_MAX_CONTENT_LENGTH - 1) {
      /* no room for this byte plus the terminator */
      ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_length++;
  }

  ಯದಿ(escape)
    ಪ್ರತ್ಯಾಗಮ FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
ಸ್ಥಿರ ಶೂನ್ಯ auth_digest_md5_to_ascii(
  ಧ್ರುವ unsigned ವರ್ಣ *source, /* 16 bytes */
  unsigned ವರ್ಣ *dest)         /* 33 bytes */
{
  ಪೂರ್ಣಾಙ್ಕ i;
  ಪ್ರತಿ(i = 0; i < 16; i++)
    curl_msnprintf((ವರ್ಣ *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
ಸ್ಥಿರ ಶೂನ್ಯ auth_digest_sha256_to_ascii(
  ಧ್ರುವ unsigned ವರ್ಣ *source, /* 32 bytes */
  unsigned ವರ್ಣ *dest)         /* 65 bytes */
{
  ಪೂರ್ಣಾಙ್ಕ i;
  ಪ್ರತಿ(i = 0; i < 32; i++)
    curl_msnprintf((ವರ್ಣ *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
ಸ್ಥಿರ ವರ್ಣ *auth_digest_string_quoted(ಧ್ರುವ ವರ್ಣ *s)
{
  ಸಂರಚನಾ dynbuf out;
  curlx_dyn_init(&out, 2048);
  ಯದಿ(!*s) /* for zero length input, make sure we return an empty string */
    ಪ್ರತ್ಯಾಗಮ curlx_strdup("");
  ಯಾವತ್(*s) {
    CURLcode result;
    ಯದಿ(*s == '"' || *s == '\\') {
      result = curlx_dyn_addn(&out, "\\", 1);
      ಯದಿ(!result)
        result = curlx_dyn_addn(&out, s, 1);
    }
    ಅನ್ಯಥಾ ಯದಿ((*s < ' ') || (*s > 0x7e)) {
      unsigned ವರ್ಣ buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (unsigned ವರ್ಣ)*s);
      result = curlx_dyn_addn(&out, buf, 3);
    }
    ಅನ್ಯಥಾ
      result = curlx_dyn_addn(&out, s, 1);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    s++;
  }
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
ಸ್ಥಿರ bool auth_digest_get_ಕೀಲಿ_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ *chlg, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ,
                                      ವರ್ಣ *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  ಕುರು {
    ಸಂರಚನಾ Curl_str data;
    ಸಂರಚನಾ Curl_str name;

    curlx_str_passblanks(&chlg);

    ಯದಿ(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      ಪೂರ್ಣಾಙ್ಕ rc = curlx_str_quotedword(&chlg, &data, 256);
      ಯದಿ(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &data, 256, ',');
      ಯದಿ(rc)
        ಪ್ರತ್ಯಾಗಮ FALSE; /* weird */

      ಯದಿ(curlx_str_cmp(&name, ಕೀಲಿ)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&data);
        ಧ್ರುವ ವರ್ಣ *src = curlx_str(&data);
        size_t i;
        size_t outlen = 0;

        ಯದಿ(len >= buflen)
          /* does not fit */
          ಪ್ರತ್ಯಾಗಮ FALSE;

        ಪ್ರತಿ(i = 0; i < len; i++) {
          ಯದಿ(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        ಪ್ರತ್ಯಾಗಮ TRUE;
      }
      ಯದಿ(curlx_str_single(&chlg, ','))
        ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಅನ್ಯಥಾ /* odd syntax */
      ಭಙ್ಗ;
  } ಯಾವತ್(1);

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ auth_digest_get_qop_ಮೌಲ್ಯಗಳು(ಧ್ರುವ ವರ್ಣ *options, ಪೂರ್ಣಾಙ್ಕ *ಮೌಲ್ಯ)
{
  ಸಂರಚನಾ Curl_str out;
  /* Initialize the output */
  *ಮೌಲ್ಯ = 0;

  ಯಾವತ್(!curlx_str_until(&options, &out, 32, ',')) {
    ಯದಿ(curlx_str_casecompare(&out, DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH))
      *ಮೌಲ್ಯ |= DIGEST_QOP_ಮೌಲ್ಯ_AUTH;
    ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&out, DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_INT))
      *ಮೌಲ್ಯ |= DIGEST_QOP_ಮೌಲ್ಯ_AUTH_INT;
    ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&out, DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_CONF))
      *ಮೌಲ್ಯ |= DIGEST_QOP_ಮೌಲ್ಯ_AUTH_CONF;
    ಯದಿ(curlx_str_single(&options, ','))
      ಭಙ್ಗ;
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
ಸ್ಥಿರ CURLcode auth_decode_digest_md5_message(ಧ್ರುವ ಸಂರಚನಾ bufref *chlgref,
                                               ವರ್ಣ *nonce, size_t nlen,
                                               ವರ್ಣ *realm, size_t rlen,
                                               ವರ್ಣ *alg, size_t alen,
                                               ವರ್ಣ *qop, size_t qlen)
{
  ಧ್ರುವ ವರ್ಣ *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  ಯದಿ(!Curl_bufref_len(chlgref))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  ಯದಿ(!auth_digest_get_ಕೀಲಿ_ಮೌಲ್ಯ(chlg, "nonce", nonce, nlen))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  ಯದಿ(!auth_digest_get_ಕೀಲಿ_ಮೌಲ್ಯ(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  ಯದಿ(!auth_digest_get_ಕೀಲಿ_ಮೌಲ್ಯ(chlg, "algorithm", alg, alen))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  ಯದಿ(!auth_digest_get_ಕೀಲಿ_ಮೌಲ್ಯ(chlg, "qop", qop, qlen))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
  ಪ್ರತ್ಯಾಗಮ TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(ಸಂರಚನಾ Curl_easy *data,
                                             ಧ್ರುವ ಸಂರಚನಾ bufref *chlg,
                                             ಸಂರಚನಾ Curl_creds *creds,
                                             ಧ್ರುವ ವರ್ಣ *default_service,
                                             ಸಂರಚನಾ bufref *out)
{
  ಧ್ರುವ ವರ್ಣ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  ಸಂರಚನಾ MD5_context *ctxt;
  ಧ್ರುವ ವರ್ಣ *userp = Curl_creds_user(creds);
  ಧ್ರುವ ವರ್ಣ *passwdp = Curl_creds_passwd(creds);
  ವರ್ಣ *response = ರಿಕ್ತ;
  unsigned ವರ್ಣ digest[MD5_DIGEST_LEN];
  ವರ್ಣ HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  ವರ್ಣ HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  ವರ್ಣ resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  ವರ್ಣ nonce[64];
  ವರ್ಣ realm[128];
  ವರ್ಣ algorithm[64];
  ವರ್ಣ qop_options[64];
  ಪೂರ್ಣಾಙ್ಕ qop_ಮೌಲ್ಯಗಳು;
  ವರ್ಣ cnonce[33];
  ವರ್ಣ nonce_ಎಣಿಕೆ[] = "00000001";
  ವರ್ಣ method[]     = "AUTHENTICATE";
  ವರ್ಣ qop[]        = DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH;
  ವರ್ಣ *spn         = ರಿಕ್ತ;
  ವರ್ಣ *qrealm;
  ವರ್ಣ *qnonce;
  ವರ್ಣ *quserp;
  ವರ್ಣ *qspn;

  /* Decode the challenge message */
  CURLcode result = auth_decode_digest_md5_message(chlg,
                                                   nonce, ಪರಿಮಾಣ(nonce),
                                                   realm, ಪರಿಮಾಣ(realm),
                                                   algorithm,
                                                   ಪರಿಮಾಣ(algorithm),
                                                   qop_options,
                                                   ಪರಿಮಾಣ(qop_options));
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* We only support md5 sessions */
  ಯದಿ(strcmp(algorithm, "md5-sess"))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_ಮೌಲ್ಯಗಳು(qop_options, &qop_ಮೌಲ್ಯಗಳು);

  /* We only support auth quality-of-protection */
  ಯದಿ(!(qop_ಮೌಲ್ಯಗಳು & DIGEST_QOP_ಮೌಲ್ಯ_AUTH))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  result = Curl_rand_hex(data, (unsigned ವರ್ಣ *)cnonce, ಪರಿಮಾಣ(cnonce));
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ಯದಿ(!ctxt)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)userp,
                  curlx_uztoui(strlen(userp)));
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)":", 1);
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)realm,
                  curlx_uztoui(strlen(realm)));
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)":", 1);
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)passwdp,
                  curlx_uztoui(strlen(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ಯದಿ(!ctxt)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)":", 1);
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)":", 1);
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  ಪ್ರತಿ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, ರಿಕ್ತ);
  ಯದಿ(!spn)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ಯದಿ(!ctxt) {
    curlx_free(spn);

    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)method,
                  curlx_uztoui(strlen(method)));
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)":", 1);
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)spn,
                  curlx_uztoui(strlen(spn)));
  Curl_MD5_final(ctxt, digest);

  ಪ್ರತಿ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ಯದಿ(!ctxt) {
    curlx_free(spn);

    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)":", 1);
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)":", 1);

  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)nonce_ಎಣಿಕೆ,
                  curlx_uztoui(strlen(nonce_ಎಣಿಕೆ)));
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)":", 1);
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)":", 1);
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)qop,
                  curlx_uztoui(strlen(qop)));
  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)":", 1);

  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  ಪ್ರತಿ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  ಯದಿ(qrealm && qnonce && quserp && qspn)
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
  ಯದಿ(!response)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, strlen(response), curl_free);
  ಪ್ರತ್ಯಾಗಮ result;
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
CURLcode Curl_auth_decode_digest_http_message(ಧ್ರುವ ವರ್ಣ *chlg,
                                              ಸಂರಚನಾ digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  ಯದಿ(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  ಪ್ರತಿ(;;) {
    ಸಂರಚನಾ Curl_str ಮೌಲ್ಯ;
    ವರ್ಣ content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    ಯದಿ(Curl_auth_digest_get_pair(chlg, &ಮೌಲ್ಯ, content, &chlg)) {
      ಯದಿ(curlx_str_casecompare(&ಮೌಲ್ಯ, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        ಯದಿ(!digest->nonce)
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
      ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&ಮೌಲ್ಯ, "stale")) {
        ಯದಿ(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&ಮೌಲ್ಯ, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        ಯದಿ(!digest->realm)
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
      ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&ಮೌಲ್ಯ, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        ಯದಿ(!digest->opaque)
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
      ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&ಮೌಲ್ಯ, "qop")) {
        ಧ್ರುವ ವರ್ಣ *token = content;
        ಸಂರಚನಾ Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        ಯಾವತ್(!curlx_str_until(&token, &out, 32, ',')) {
          ಯದಿ(curlx_str_casecompare(&out, DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH))
            foundAuth = TRUE;
          ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&out,
                                        DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          ಯದಿ(curlx_str_single(&token, ','))
            ಭಙ್ಗ;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        ಯದಿ(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH);
          ಯದಿ(!digest->qop)
            ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
        }
        ಅನ್ಯಥಾ ಯದಿ(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_ಮೌಲ್ಯ_STRING_AUTH_INT);
          ಯದಿ(!digest->qop)
            ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
        }
      }
      ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&ಮೌಲ್ಯ, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        ಯದಿ(!digest->algorithm)
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

        ಯದಿ(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        ಅನ್ಯಥಾ ಯದಿ(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        ಅನ್ಯಥಾ ಯದಿ(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        ಅನ್ಯಥಾ ಯದಿ(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        ಅನ್ಯಥಾ ಯದಿ(curl_strequal(content, "SHA-512-256")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#ಅನ್ಯಥಾ /* !CURL_HAVE_SHA512_256 */
          ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        ಅನ್ಯಥಾ ಯದಿ(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#ಅನ್ಯಥಾ /* !CURL_HAVE_SHA512_256 */
          ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        ಅನ್ಯಥಾ
          ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
      }
      ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&ಮೌಲ್ಯ, "userhash")) {
        ಯದಿ(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      ಅನ್ಯಥಾ {
        /* Unknown specifier, ignore it! */
      }
    }
    ಅನ್ಯಥಾ
      ಭಙ್ಗ; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (ಶೂನ್ಯ)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  ಯದಿ(before && !digest->stale)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  ಯದಿ(!digest->nonce)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  ಯದಿ(!digest->qop && (digest->algo & SESSION_ALGO))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
ಸ್ಥಿರ CURLcode auth_create_digest_http_message(
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ Curl_creds *creds,
  ಧ್ರುವ unsigned ವರ್ಣ *request,
  ಧ್ರುವ unsigned ವರ್ಣ *uripath,
  ಸಂರಚನಾ digestdata *digest,
  ವರ್ಣ **outptr, size_t *outlen,
  ಶೂನ್ಯ (*convert_to_ascii)(ಧ್ರುವ unsigned ವರ್ಣ *, unsigned ವರ್ಣ *),
  CURLcode (*hash)(unsigned ವರ್ಣ *, ಧ್ರುವ unsigned ವರ್ಣ *, ಧ್ರುವ size_t))
{
  CURLcode result;
  ಧ್ರುವ ವರ್ಣ *userp = Curl_creds_user(creds);
  ಧ್ರುವ ವರ್ಣ *passwdp = Curl_creds_passwd(creds);
  unsigned ವರ್ಣ hashbuf[32]; /* 32 bytes/256 bits */
  unsigned ವರ್ಣ request_digest[65];
  unsigned ವರ್ಣ ha1[65];    /* 64 digits and 1 zero byte */
  unsigned ವರ್ಣ ha2[65];    /* 64 digits and 1 zero byte */
  ವರ್ಣ userh[65];
  ವರ್ಣ *cnonce = ರಿಕ್ತ;
  size_t cnonce_sz = 0;
  ವರ್ಣ *userp_quoted = ರಿಕ್ತ;
  ವರ್ಣ *realm_quoted = ರಿಕ್ತ;
  ವರ್ಣ *nonce_quoted = ರಿಕ್ತ;
  ವರ್ಣ *hashthis = ರಿಕ್ತ;
  ವರ್ಣ *uri_quoted = ರಿಕ್ತ;
  ಸಂರಚನಾ dynbuf response;
  *outptr = ರಿಕ್ತ;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, ಪರಿಮಾಣ(hashbuf));
  ಯದಿ(!digest->nc)
    digest->nc = 1;

  ಯದಿ(!digest->cnonce) {
    ವರ್ಣ cnoncebuf[12];
    result = Curl_rand_bytes(data,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (unsigned ವರ್ಣ *)cnoncebuf,
                             ಪರಿಮಾಣ(cnoncebuf));
    ಯದಿ(!result)
      result = curlx_base64_encode((uint8_t *)cnoncebuf, ಪರಿಮಾಣ(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    ಯದಿ(result)
      ಗಚ್ಛ oom;

    digest->cnonce = cnonce;
  }

  ಯದಿ(digest->userhash) {
    ವರ್ಣ *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    ಯದಿ(!hasht) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ oom;
    }

    result = hash(hashbuf, (unsigned ವರ್ಣ *)hasht, strlen(hasht));
    curlx_free(hasht);
    ಯದಿ(result)
      ಗಚ್ಛ oom;
    convert_to_ascii(hashbuf, (unsigned ವರ್ಣ *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  ಯದಿ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ oom;
  }

  result = hash(hashbuf, (unsigned ವರ್ಣ *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  ಯದಿ(result)
    ಗಚ್ಛ oom;
  convert_to_ascii(hashbuf, ha1);

  ಯದಿ(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    ವರ್ಣ *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    ಯದಿ(!tmp) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ oom;
    }

    result = hash(hashbuf, (unsigned ವರ್ಣ *)tmp, strlen(tmp));
    curlx_free(tmp);
    ಯದಿ(result)
      ಗಚ್ಛ oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ಧ್ರುವ ವರ್ಣ *)uripath);
  ಯದಿ(!uri_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  ಯದಿ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ oom;
  }

  ಯದಿ(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    ವರ್ಣ hashed[65];
    ವರ್ಣ *hashthis2;

    result = hash(hashbuf, (ಧ್ರುವ unsigned ವರ್ಣ *)"", 0);
    ಯದಿ(result) {
      curlx_free(hashthis);
      ಗಚ್ಛ oom;
    }
    convert_to_ascii(hashbuf, (unsigned ವರ್ಣ *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    ಯದಿ(!hashthis) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ oom;
    }
  }

  result = hash(hashbuf, (unsigned ವರ್ಣ *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  ಯದಿ(result)
    ಗಚ್ಛ oom;
  convert_to_ascii(hashbuf, ha2);

  ಯದಿ(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (unsigned ಪೂರ್ಣಾಙ್ಕ)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  ಅನ್ಯಥಾ
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  ಯದಿ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ oom;
  }

  result = hash(hashbuf, (unsigned ವರ್ಣ *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  ಯದಿ(result)
    ಗಚ್ಛ oom;
  convert_to_ascii(hashbuf, request_digest);

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
  ಯದಿ(!userp_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ oom;
  }
  ಯದಿ(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  ಅನ್ಯಥಾ {
    realm_quoted = curlx_malloc(1);
    ಯದಿ(realm_quoted)
      realm_quoted[0] = 0;
  }
  ಯದಿ(!realm_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  ಯದಿ(!nonce_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ oom;
  }

  ಯದಿ(digest->qop) {
    result = curlx_dyn_addf(&response, "username=\"%s\", "
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
                            (unsigned ಪೂರ್ಣಾಙ್ಕ)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  ಅನ್ಯಥಾ {
    result = curlx_dyn_addf(&response, "username=\"%s\", "
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
  ಯದಿ(result)
    ಗಚ್ಛ oom;

  /* Add the optional fields */
  ಯದಿ(digest->opaque) {
    /* Append the opaque */
    ವರ್ಣ *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    ಯದಿ(!opaque_quoted) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ oom;
    }
    result = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    ಯದಿ(result)
      ಗಚ್ಛ oom;
  }

  ಯದಿ(digest->algorithm) {
    /* Append the algorithm */
    result = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    ಯದಿ(result)
      ಗಚ್ಛ oom;
  }

  ಯದಿ(digest->userhash) {
    /* Append the userhash */
    result = curlx_dyn_add(&response, ", userhash=true");
    ಯದಿ(result)
      ಗಚ್ಛ oom;
  }

  /* Return the output */
  *outptr = curlx_dyn_ptr(&response);
  *outlen = curlx_dyn_len(&response);
  result = CURLE_OK;

oom:
  curlx_free(nonce_quoted);
  curlx_free(realm_quoted);
  curlx_free(uri_quoted);
  curlx_free(userp_quoted);
  ಯದಿ(result)
    curlx_dyn_free(&response);
  ಪ್ರತ್ಯಾಗಮ result;
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
CURLcode Curl_auth_create_digest_http_message(ಸಂರಚನಾ Curl_easy *data,
                                              ಸಂರಚನಾ Curl_creds *creds,
                                              ಧ್ರುವ unsigned ವರ್ಣ *request,
                                              ಧ್ರುವ unsigned ವರ್ಣ *uripath,
                                              ಸಂರಚನಾ digestdata *digest,
                                              ವರ್ಣ **outptr, size_t *outlen)
{
  ಯದಿ(digest->algo <= ALGO_MD5SESS)
    ಪ್ರತ್ಯಾಗಮ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  ಯದಿ(digest->algo <= ALGO_SHA256SESS)
    ಪ್ರತ್ಯಾಗಮ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  ಯದಿ(digest->algo <= ALGO_SHA512_256SESS)
    ಪ್ರತ್ಯಾಗಮ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
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
ಶೂನ್ಯ Curl_auth_digest_cleanup(ಸಂರಚನಾ digestdata *digest)
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
#endif /* !USE_WINDOWS_SSPI */

#endif /* !CURL_DISABLE_DIGEST_AUTH */
