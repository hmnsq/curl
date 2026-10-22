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
#ضمن "curl_setup.h"

#ifndef CURL_DISABLE_DIGEST_AUTH

#ضمن "vauth/vauth.h"
#ضمن "vauth/digest.h"
#ضمن "curlx/base64.h"
#ضمن "curl_md5.h"
#ضمن "curl_sha256.h"
#ضمن "curl_sha512_256.h"
#ضمن "curlx/strparse.h"
#ضمن "rand.h"
#ضمن "escape.h"

#ifndef USE_WINDOWS_SSPI
#define SESSION_ALGO 1 /* for algos with this bit set */

#define ALGO_MD5 0
#define ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#define ALGO_SHA256 2
#define ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#define ALGO_SHA512_256 4
#define ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#define DIGEST_QOP_VALUE_AUTH             (1 << 0)
#define DIGEST_QOP_VALUE_AUTH_INT         (1 << 1)
#define DIGEST_QOP_VALUE_AUTH_CONF        (1 << 2)

#define DIGEST_QOP_VALUE_STRING_AUTH      "auth"
#define DIGEST_QOP_VALUE_STRING_AUTH_INT  "auth-int"
#define DIGEST_QOP_VALUE_STRING_AUTH_CONF "auth-conf"
#endif

bool Curl_auth_digest_get_pair(ثابت حرف *str, بنية Curl_str *value,
                               حرف *content, ثابت حرف **endptr)
{
  صحيح c;
  size_t content_length = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  𐡄𐡍(curlx_str_until(&str, value, DIGEST_MAX_VALUE_LENGTH, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    أرجع FALSE;

  𐡄𐡍('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  لكل(c = DIGEST_MAX_CONTENT_LENGTH; *str && c--; str++) {
    𐡄𐡍(!escape) {
      بدل(*str) {
      حالة '\\':
        𐡄𐡍(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          استمر;
        }
        اكسر;

      حالة ',':
        𐡄𐡍(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          استمر;
        }
        اكسر;

      حالة '\r':
      حالة '\n':
        /* end of string */
        𐡄𐡍(starts_with_quote)
          أرجع FALSE; /* No closing quote */
        c = 0;
        استمر;

      حالة '\"':
        𐡄𐡍(starts_with_quote) {
          /* end of string */
          c = 0;
          استمر;
        }
        وإلا
          أرجع FALSE;
      }
    }

    𐡄𐡍(content_length >= DIGEST_MAX_CONTENT_LENGTH - 1) {
      /* no room for this byte plus the terminator */
      أرجع FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_length++;
  }

  𐡄𐡍(escape)
    أرجع FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  أرجع TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
ساكن فراغ auth_digest_md5_to_ascii(
  ثابت unsigned حرف *source, /* 16 bytes */
  unsigned حرف *dest)         /* 33 bytes */
{
  صحيح i;
  لكل(i = 0; i < 16; i++)
    curl_msnprintf((حرف *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
ساكن فراغ auth_digest_sha256_to_ascii(
  ثابت unsigned حرف *source, /* 32 bytes */
  unsigned حرف *dest)         /* 65 bytes */
{
  صحيح i;
  لكل(i = 0; i < 32; i++)
    curl_msnprintf((حرف *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
ساكن حرف *auth_digest_string_quoted(ثابت حرف *s)
{
  بنية dynbuf out;
  curlx_dyn_init(&out, 2048);
  𐡄𐡍(!*s) /* for zero length input, make sure we return an empty string */
    أرجع curlx_strdup("");
  طالما(*s) {
    CURLcode result;
    𐡄𐡍(*s == '"' || *s == '\\') {
      result = curlx_dyn_addn(&out, "\\", 1);
      𐡄𐡍(!result)
        result = curlx_dyn_addn(&out, s, 1);
    }
    وإلا 𐡄𐡍((*s < ' ') || (*s > 0x7e)) {
      unsigned حرف buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (unsigned حرف)*s);
      result = curlx_dyn_addn(&out, buf, 3);
    }
    وإلا
      result = curlx_dyn_addn(&out, s, 1);
    𐡄𐡍(result)
      أرجع عدم;
    s++;
  }
  أرجع curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
ساكن bool auth_digest_get_key_value(ثابت حرف *chlg, ثابت حرف *key,
                                      حرف *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  𐡏𐡁𐡃 {
    بنية Curl_str data;
    بنية Curl_str name;

    curlx_str_passblanks(&chlg);

    𐡄𐡍(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      صحيح rc = curlx_str_quotedword(&chlg, &data, 256);
      𐡄𐡍(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &data, 256, ',');
      𐡄𐡍(rc)
        أرجع FALSE; /* weird */

      𐡄𐡍(curlx_str_cmp(&name, key)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&data);
        ثابت حرف *src = curlx_str(&data);
        size_t i;
        size_t outlen = 0;

        𐡄𐡍(len >= buflen)
          /* does not fit */
          أرجع FALSE;

        لكل(i = 0; i < len; i++) {
          𐡄𐡍(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        أرجع TRUE;
      }
      𐡄𐡍(curlx_str_single(&chlg, ','))
        أرجع FALSE;
    }
    وإلا /* odd syntax */
      اكسر;
  } طالما(1);

  أرجع FALSE;
}

ساكن فراغ auth_digest_get_qop_values(ثابت حرف *options, صحيح *value)
{
  بنية Curl_str out;
  /* Initialize the output */
  *value = 0;

  طالما(!curlx_str_until(&options, &out, 32, ',')) {
    𐡄𐡍(curlx_str_casecompare(&out, DIGEST_QOP_VALUE_STRING_AUTH))
      *value |= DIGEST_QOP_VALUE_AUTH;
    وإلا 𐡄𐡍(curlx_str_casecompare(&out, DIGEST_QOP_VALUE_STRING_AUTH_INT))
      *value |= DIGEST_QOP_VALUE_AUTH_INT;
    وإلا 𐡄𐡍(curlx_str_casecompare(&out, DIGEST_QOP_VALUE_STRING_AUTH_CONF))
      *value |= DIGEST_QOP_VALUE_AUTH_CONF;
    𐡄𐡍(curlx_str_single(&options, ','))
      اكسر;
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
ساكن CURLcode auth_decode_digest_md5_message(ثابت بنية bufref *chlgref,
                                               حرف *nonce, size_t nlen,
                                               حرف *realm, size_t rlen,
                                               حرف *alg, size_t alen,
                                               حرف *qop, size_t qlen)
{
  ثابت حرف *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  𐡄𐡍(!Curl_bufref_len(chlgref))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  𐡄𐡍(!auth_digest_get_key_value(chlg, "nonce", nonce, nlen))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  𐡄𐡍(!auth_digest_get_key_value(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  𐡄𐡍(!auth_digest_get_key_value(chlg, "algorithm", alg, alen))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  𐡄𐡍(!auth_digest_get_key_value(chlg, "qop", qop, qlen))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  أرجع CURLE_OK;
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
bool Curl_auth_is_digest_supported(فراغ)
{
  أرجع TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(بنية Curl_easy *data,
                                             ثابت بنية bufref *chlg,
                                             بنية Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             بنية bufref *out)
{
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  بنية MD5_context *ctxt;
  ثابت حرف *userp = Curl_creds_user(creds);
  ثابت حرف *passwdp = Curl_creds_passwd(creds);
  حرف *response = عدم;
  unsigned حرف digest[MD5_DIGEST_LEN];
  حرف HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  حرف HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  حرف resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  حرف nonce[64];
  حرف realm[128];
  حرف algorithm[64];
  حرف qop_options[64];
  صحيح qop_values;
  حرف cnonce[33];
  حرف nonceCount[] = "00000001";
  حرف method[]     = "AUTHENTICATE";
  حرف qop[]        = DIGEST_QOP_VALUE_STRING_AUTH;
  حرف *spn         = عدم;
  حرف *qrealm;
  حرف *qnonce;
  حرف *quserp;
  حرف *qspn;

  /* Decode the challenge message */
  CURLcode result = auth_decode_digest_md5_message(chlg,
                                                   nonce, حجم(nonce),
                                                   realm, حجم(realm),
                                                   algorithm,
                                                   حجم(algorithm),
                                                   qop_options,
                                                   حجم(qop_options));
  𐡄𐡍(result)
    أرجع result;

  /* We only support md5 sessions */
  𐡄𐡍(strcmp(algorithm, "md5-sess"))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_values(qop_options, &qop_values);

  /* We only support auth quality-of-protection */
  𐡄𐡍(!(qop_values & DIGEST_QOP_VALUE_AUTH))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  result = Curl_rand_hex(data, (unsigned حرف *)cnonce, حجم(cnonce));
  𐡄𐡍(result)
    أرجع result;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  𐡄𐡍(!ctxt)
    أرجع CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)userp,
                  curlx_uztoui(strlen(userp)));
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)realm,
                  curlx_uztoui(strlen(realm)));
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)passwdp,
                  curlx_uztoui(strlen(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  𐡄𐡍(!ctxt)
    أرجع CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  لكل(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, عدم);
  𐡄𐡍(!spn)
    أرجع CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  𐡄𐡍(!ctxt) {
    curlx_free(spn);

    أرجع CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)method,
                  curlx_uztoui(strlen(method)));
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)spn,
                  curlx_uztoui(strlen(spn)));
  Curl_MD5_final(ctxt, digest);

  لكل(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  𐡄𐡍(!ctxt) {
    curlx_free(spn);

    أرجع CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)":", 1);

  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)nonceCount,
                  curlx_uztoui(strlen(nonceCount)));
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)qop,
                  curlx_uztoui(strlen(qop)));
  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)":", 1);

  Curl_MD5_update(ctxt, (ثابت unsigned حرف *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  لكل(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  𐡄𐡍(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonceCount, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  𐡄𐡍(!response)
    أرجع CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, strlen(response), curl_free);
  أرجع result;
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
CURLcode Curl_auth_decode_digest_http_message(ثابت حرف *chlg,
                                              بنية digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  𐡄𐡍(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  لكل(;;) {
    بنية Curl_str value;
    حرف content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    𐡄𐡍(Curl_auth_digest_get_pair(chlg, &value, content, &chlg)) {
      𐡄𐡍(curlx_str_casecompare(&value, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        𐡄𐡍(!digest->nonce)
          أرجع CURLE_OUT_OF_MEMORY;
      }
      وإلا 𐡄𐡍(curlx_str_casecompare(&value, "stale")) {
        𐡄𐡍(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      وإلا 𐡄𐡍(curlx_str_casecompare(&value, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        𐡄𐡍(!digest->realm)
          أرجع CURLE_OUT_OF_MEMORY;
      }
      وإلا 𐡄𐡍(curlx_str_casecompare(&value, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        𐡄𐡍(!digest->opaque)
          أرجع CURLE_OUT_OF_MEMORY;
      }
      وإلا 𐡄𐡍(curlx_str_casecompare(&value, "qop")) {
        ثابت حرف *token = content;
        بنية Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        طالما(!curlx_str_until(&token, &out, 32, ',')) {
          𐡄𐡍(curlx_str_casecompare(&out, DIGEST_QOP_VALUE_STRING_AUTH))
            foundAuth = TRUE;
          وإلا 𐡄𐡍(curlx_str_casecompare(&out,
                                        DIGEST_QOP_VALUE_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          𐡄𐡍(curlx_str_single(&token, ','))
            اكسر;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        𐡄𐡍(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_VALUE_STRING_AUTH);
          𐡄𐡍(!digest->qop)
            أرجع CURLE_OUT_OF_MEMORY;
        }
        وإلا 𐡄𐡍(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_VALUE_STRING_AUTH_INT);
          𐡄𐡍(!digest->qop)
            أرجع CURLE_OUT_OF_MEMORY;
        }
      }
      وإلا 𐡄𐡍(curlx_str_casecompare(&value, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        𐡄𐡍(!digest->algorithm)
          أرجع CURLE_OUT_OF_MEMORY;

        𐡄𐡍(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        وإلا 𐡄𐡍(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        وإلا 𐡄𐡍(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        وإلا 𐡄𐡍(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        وإلا 𐡄𐡍(curl_strequal(content, "SHA-512-256")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#وإلا /* !CURL_HAVE_SHA512_256 */
          أرجع CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        وإلا 𐡄𐡍(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#وإلا /* !CURL_HAVE_SHA512_256 */
          أرجع CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        وإلا
          أرجع CURLE_BAD_CONTENT_ENCODING;
      }
      وإلا 𐡄𐡍(curlx_str_casecompare(&value, "userhash")) {
        𐡄𐡍(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      وإلا {
        /* Unknown specifier, ignore it! */
      }
    }
    وإلا
      اكسر; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (فراغ)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  𐡄𐡍(before && !digest->stale)
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  𐡄𐡍(!digest->nonce)
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  𐡄𐡍(!digest->qop && (digest->algo & SESSION_ALGO))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  أرجع CURLE_OK;
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
ساكن CURLcode auth_create_digest_http_message(
  بنية Curl_easy *data,
  بنية Curl_creds *creds,
  ثابت unsigned حرف *request,
  ثابت unsigned حرف *uripath,
  بنية digestdata *digest,
  حرف **outptr, size_t *outlen,
  فراغ (*convert_to_ascii)(ثابت unsigned حرف *, unsigned حرف *),
  CURLcode (*hash)(unsigned حرف *, ثابت unsigned حرف *, ثابت size_t))
{
  CURLcode result;
  ثابت حرف *userp = Curl_creds_user(creds);
  ثابت حرف *passwdp = Curl_creds_passwd(creds);
  unsigned حرف hashbuf[32]; /* 32 bytes/256 bits */
  unsigned حرف request_digest[65];
  unsigned حرف ha1[65];    /* 64 digits and 1 zero byte */
  unsigned حرف ha2[65];    /* 64 digits and 1 zero byte */
  حرف userh[65];
  حرف *cnonce = عدم;
  size_t cnonce_sz = 0;
  حرف *userp_quoted = عدم;
  حرف *realm_quoted = عدم;
  حرف *nonce_quoted = عدم;
  حرف *hashthis = عدم;
  حرف *uri_quoted = عدم;
  بنية dynbuf response;
  *outptr = عدم;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, حجم(hashbuf));
  𐡄𐡍(!digest->nc)
    digest->nc = 1;

  𐡄𐡍(!digest->cnonce) {
    حرف cnoncebuf[12];
    result = Curl_rand_bytes(data,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (unsigned حرف *)cnoncebuf,
                             حجم(cnoncebuf));
    𐡄𐡍(!result)
      result = curlx_base64_encode((uint8_t *)cnoncebuf, حجم(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    𐡄𐡍(result)
      اذهب oom;

    digest->cnonce = cnonce;
  }

  𐡄𐡍(digest->userhash) {
    حرف *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    𐡄𐡍(!hasht) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب oom;
    }

    result = hash(hashbuf, (unsigned حرف *)hasht, strlen(hasht));
    curlx_free(hasht);
    𐡄𐡍(result)
      اذهب oom;
    convert_to_ascii(hashbuf, (unsigned حرف *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  𐡄𐡍(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب oom;
  }

  result = hash(hashbuf, (unsigned حرف *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  𐡄𐡍(result)
    اذهب oom;
  convert_to_ascii(hashbuf, ha1);

  𐡄𐡍(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    حرف *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    𐡄𐡍(!tmp) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب oom;
    }

    result = hash(hashbuf, (unsigned حرف *)tmp, strlen(tmp));
    curlx_free(tmp);
    𐡄𐡍(result)
      اذهب oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ثابت حرف *)uripath);
  𐡄𐡍(!uri_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  𐡄𐡍(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب oom;
  }

  𐡄𐡍(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    حرف hashed[65];
    حرف *hashthis2;

    result = hash(hashbuf, (ثابت unsigned حرف *)"", 0);
    𐡄𐡍(result) {
      curlx_free(hashthis);
      اذهب oom;
    }
    convert_to_ascii(hashbuf, (unsigned حرف *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    𐡄𐡍(!hashthis) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب oom;
    }
  }

  result = hash(hashbuf, (unsigned حرف *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  𐡄𐡍(result)
    اذهب oom;
  convert_to_ascii(hashbuf, ha2);

  𐡄𐡍(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (unsigned صحيح)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  وإلا
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  𐡄𐡍(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب oom;
  }

  result = hash(hashbuf, (unsigned حرف *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  𐡄𐡍(result)
    اذهب oom;
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
  𐡄𐡍(!userp_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب oom;
  }
  𐡄𐡍(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  وإلا {
    realm_quoted = curlx_malloc(1);
    𐡄𐡍(realm_quoted)
      realm_quoted[0] = 0;
  }
  𐡄𐡍(!realm_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  𐡄𐡍(!nonce_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب oom;
  }

  𐡄𐡍(digest->qop) {
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
                            (unsigned صحيح)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  وإلا {
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
  𐡄𐡍(result)
    اذهب oom;

  /* Add the optional fields */
  𐡄𐡍(digest->opaque) {
    /* Append the opaque */
    حرف *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    𐡄𐡍(!opaque_quoted) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب oom;
    }
    result = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    𐡄𐡍(result)
      اذهب oom;
  }

  𐡄𐡍(digest->algorithm) {
    /* Append the algorithm */
    result = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    𐡄𐡍(result)
      اذهب oom;
  }

  𐡄𐡍(digest->userhash) {
    /* Append the userhash */
    result = curlx_dyn_add(&response, ", userhash=true");
    𐡄𐡍(result)
      اذهب oom;
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
  𐡄𐡍(result)
    curlx_dyn_free(&response);
  أرجع result;
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
CURLcode Curl_auth_create_digest_http_message(بنية Curl_easy *data,
                                              بنية Curl_creds *creds,
                                              ثابت unsigned حرف *request,
                                              ثابت unsigned حرف *uripath,
                                              بنية digestdata *digest,
                                              حرف **outptr, size_t *outlen)
{
  𐡄𐡍(digest->algo <= ALGO_MD5SESS)
    أرجع auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  𐡄𐡍(digest->algo <= ALGO_SHA256SESS)
    أرجع auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  𐡄𐡍(digest->algo <= ALGO_SHA512_256SESS)
    أرجع auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  أرجع CURLE_BAD_CONTENT_ENCODING;
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
فراغ Curl_auth_digest_cleanup(بنية digestdata *digest)
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
