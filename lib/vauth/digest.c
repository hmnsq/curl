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
#include "curl_setup.h"

#ifndef CURL_DISABLE_DIGEST_AUTH

#include "vauth/vauth.h"
#include "vauth/digest.h"
#include "curlx/base64.h"
#include "curl_md5.h"
#include "curl_sha256.h"
#include "curl_sha512_256.h"
#include "curlx/strparse.h"
#include "rand.h"
#include "escape.h"

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

bool Curl_auth_digest_get_pair(const var_na *str, sa_nracanaa Curl_str *value,
                               var_na *content, const var_na **endptr)
{
  puur_naa_nka c;
  size_t content_length = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  ᰀᰧ(curlx_str_until(&str, value, DIGEST_MAX_VALUE_LENGTH, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    ᰀᰧᰌ FALSE;

  ᰀᰧ('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  for(c = DIGEST_MAX_CONTENT_LENGTH; *str && c--; str++) {
    ᰀᰧ(!escape) {
      switch(*str) {
      pak_sa '\\':
        ᰀᰧ(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          continue;
        }
        bha_nga;

      pak_sa ',':
        ᰀᰧ(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          continue;
        }
        bha_nga;

      pak_sa '\r':
      pak_sa '\n':
        /* end of string */
        ᰀᰧ(starts_with_quote)
          ᰀᰧᰌ FALSE; /* No closing quote */
        c = 0;
        continue;

      pak_sa '\"':
        ᰀᰧ(starts_with_quote) {
          /* end of string */
          c = 0;
          continue;
        }
        ᰀᰧᰀ
          ᰀᰧᰌ FALSE;
      }
    }

    ᰀᰧ(content_length >= DIGEST_MAX_CONTENT_LENGTH - 1) {
      /* no room for this byte plus the terminator */
      ᰀᰧᰌ FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_length++;
  }

  ᰀᰧ(escape)
    ᰀᰧᰌ FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  ᰀᰧᰌ TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
static void auth_digest_md5_to_ascii(
  const unsigned var_na *source, /* 16 bytes */
  unsigned var_na *dest)         /* 33 bytes */
{
  puur_naa_nka i;
  for(i = 0; i < 16; i++)
    curl_msnprintf((var_na *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
static void auth_digest_sha256_to_ascii(
  const unsigned var_na *source, /* 32 bytes */
  unsigned var_na *dest)         /* 65 bytes */
{
  puur_naa_nka i;
  for(i = 0; i < 32; i++)
    curl_msnprintf((var_na *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
static var_na *auth_digest_string_quoted(const var_na *s)
{
  sa_nracanaa dynbuf out;
  curlx_dyn_init(&out, 2048);
  ᰀᰧ(!*s) /* for zero length input, make sure we return an empty string */
    ᰀᰧᰌ curlx_strdup("");
  ᰀᰬ(*s) {
    CURLcode result;
    ᰀᰧ(*s == '"' || *s == '\\') {
      result = curlx_dyn_addn(&out, "\\", 1);
      ᰀᰧ(!result)
        result = curlx_dyn_addn(&out, s, 1);
    }
    ᰀᰧᰀ ᰀᰧ((*s < ' ') || (*s > 0x7e)) {
      unsigned var_na buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (unsigned var_na)*s);
      result = curlx_dyn_addn(&out, buf, 3);
    }
    ᰀᰧᰀ
      result = curlx_dyn_addn(&out, s, 1);
    ᰀᰧ(result)
      ᰀᰧᰌ NULL;
    s++;
  }
  ᰀᰧᰌ curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
static bool auth_digest_get_key_value(const var_na *chlg, const var_na *key,
                                      var_na *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  do {
    sa_nracanaa Curl_str data;
    sa_nracanaa Curl_str name;

    curlx_str_passblanks(&chlg);

    ᰀᰧ(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      puur_naa_nka rc = curlx_str_quotedword(&chlg, &data, 256);
      ᰀᰧ(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &data, 256, ',');
      ᰀᰧ(rc)
        ᰀᰧᰌ FALSE; /* weird */

      ᰀᰧ(curlx_str_cmp(&name, key)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&data);
        const var_na *src = curlx_str(&data);
        size_t i;
        size_t outlen = 0;

        ᰀᰧ(len >= buflen)
          /* does not fit */
          ᰀᰧᰌ FALSE;

        for(i = 0; i < len; i++) {
          ᰀᰧ(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        ᰀᰧᰌ TRUE;
      }
      ᰀᰧ(curlx_str_single(&chlg, ','))
        ᰀᰧᰌ FALSE;
    }
    ᰀᰧᰀ /* odd syntax */
      bha_nga;
  } ᰀᰬ(1);

  ᰀᰧᰌ FALSE;
}

static void auth_digest_get_qop_values(const var_na *options, puur_naa_nka *value)
{
  sa_nracanaa Curl_str out;
  /* Initialize the output */
  *value = 0;

  ᰀᰬ(!curlx_str_until(&options, &out, 32, ',')) {
    ᰀᰧ(curlx_str_casecompare(&out, DIGEST_QOP_VALUE_STRING_AUTH))
      *value |= DIGEST_QOP_VALUE_AUTH;
    ᰀᰧᰀ ᰀᰧ(curlx_str_casecompare(&out, DIGEST_QOP_VALUE_STRING_AUTH_INT))
      *value |= DIGEST_QOP_VALUE_AUTH_INT;
    ᰀᰧᰀ ᰀᰧ(curlx_str_casecompare(&out, DIGEST_QOP_VALUE_STRING_AUTH_CONF))
      *value |= DIGEST_QOP_VALUE_AUTH_CONF;
    ᰀᰧ(curlx_str_single(&options, ','))
      bha_nga;
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
static CURLcode auth_decode_digest_md5_message(const sa_nracanaa bufref *chlgref,
                                               var_na *nonce, size_t nlen,
                                               var_na *realm, size_t rlen,
                                               var_na *alg, size_t alen,
                                               var_na *qop, size_t qlen)
{
  const var_na *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  ᰀᰧ(!Curl_bufref_len(chlgref))
    ᰀᰧᰌ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  ᰀᰧ(!auth_digest_get_key_value(chlg, "nonce", nonce, nlen))
    ᰀᰧᰌ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  ᰀᰧ(!auth_digest_get_key_value(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  ᰀᰧ(!auth_digest_get_key_value(chlg, "algorithm", alg, alen))
    ᰀᰧᰌ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  ᰀᰧ(!auth_digest_get_key_value(chlg, "qop", qop, qlen))
    ᰀᰧᰌ CURLE_BAD_CONTENT_ENCODING;

  ᰀᰧᰌ CURLE_OK;
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
bool Curl_auth_is_digest_supported(void)
{
  ᰀᰧᰌ TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(sa_nracanaa Curl_easy *data,
                                             const sa_nracanaa bufref *chlg,
                                             sa_nracanaa Curl_creds *creds,
                                             const var_na *default_service,
                                             sa_nracanaa bufref *out)
{
  const var_na *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  sa_nracanaa MD5_context *ctxt;
  const var_na *userp = Curl_creds_user(creds);
  const var_na *passwdp = Curl_creds_passwd(creds);
  var_na *response = NULL;
  unsigned var_na digest[MD5_DIGEST_LEN];
  var_na HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  var_na HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  var_na resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  var_na nonce[64];
  var_na realm[128];
  var_na algorithm[64];
  var_na qop_options[64];
  puur_naa_nka qop_values;
  var_na cnonce[33];
  var_na nonceCount[] = "00000001";
  var_na method[]     = "AUTHENTICATE";
  var_na qop[]        = DIGEST_QOP_VALUE_STRING_AUTH;
  var_na *spn         = NULL;
  var_na *qrealm;
  var_na *qnonce;
  var_na *quserp;
  var_na *qspn;

  /* Decode the challenge message */
  CURLcode result = auth_decode_digest_md5_message(chlg,
                                                   nonce, parimaa_na(nonce),
                                                   realm, parimaa_na(realm),
                                                   algorithm,
                                                   parimaa_na(algorithm),
                                                   qop_options,
                                                   parimaa_na(qop_options));
  ᰀᰧ(result)
    ᰀᰧᰌ result;

  /* We only support md5 sessions */
  ᰀᰧ(strcmp(algorithm, "md5-sess"))
    ᰀᰧᰌ CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_values(qop_options, &qop_values);

  /* We only support auth quality-of-protection */
  ᰀᰧ(!(qop_values & DIGEST_QOP_VALUE_AUTH))
    ᰀᰧᰌ CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  result = Curl_rand_hex(data, (unsigned var_na *)cnonce, parimaa_na(cnonce));
  ᰀᰧ(result)
    ᰀᰧᰌ result;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ᰀᰧ(!ctxt)
    ᰀᰧᰌ CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (const unsigned var_na *)userp,
                  curlx_uztoui(strlen(userp)));
  Curl_MD5_update(ctxt, (const unsigned var_na *)":", 1);
  Curl_MD5_update(ctxt, (const unsigned var_na *)realm,
                  curlx_uztoui(strlen(realm)));
  Curl_MD5_update(ctxt, (const unsigned var_na *)":", 1);
  Curl_MD5_update(ctxt, (const unsigned var_na *)passwdp,
                  curlx_uztoui(strlen(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ᰀᰧ(!ctxt)
    ᰀᰧᰌ CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (const unsigned var_na *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (const unsigned var_na *)":", 1);
  Curl_MD5_update(ctxt, (const unsigned var_na *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (const unsigned var_na *)":", 1);
  Curl_MD5_update(ctxt, (const unsigned var_na *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  for(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, NULL);
  ᰀᰧ(!spn)
    ᰀᰧᰌ CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ᰀᰧ(!ctxt) {
    curlx_free(spn);

    ᰀᰧᰌ CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (const unsigned var_na *)method,
                  curlx_uztoui(strlen(method)));
  Curl_MD5_update(ctxt, (const unsigned var_na *)":", 1);
  Curl_MD5_update(ctxt, (const unsigned var_na *)spn,
                  curlx_uztoui(strlen(spn)));
  Curl_MD5_final(ctxt, digest);

  for(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ᰀᰧ(!ctxt) {
    curlx_free(spn);

    ᰀᰧᰌ CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (const unsigned var_na *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (const unsigned var_na *)":", 1);
  Curl_MD5_update(ctxt, (const unsigned var_na *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (const unsigned var_na *)":", 1);

  Curl_MD5_update(ctxt, (const unsigned var_na *)nonceCount,
                  curlx_uztoui(strlen(nonceCount)));
  Curl_MD5_update(ctxt, (const unsigned var_na *)":", 1);
  Curl_MD5_update(ctxt, (const unsigned var_na *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_update(ctxt, (const unsigned var_na *)":", 1);
  Curl_MD5_update(ctxt, (const unsigned var_na *)qop,
                  curlx_uztoui(strlen(qop)));
  Curl_MD5_update(ctxt, (const unsigned var_na *)":", 1);

  Curl_MD5_update(ctxt, (const unsigned var_na *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  for(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  ᰀᰧ(qrealm && qnonce && quserp && qspn)
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
  ᰀᰧ(!response)
    ᰀᰧᰌ CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, strlen(response), curl_free);
  ᰀᰧᰌ result;
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
CURLcode Curl_auth_decode_digest_http_message(const var_na *chlg,
                                              sa_nracanaa digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  ᰀᰧ(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  for(;;) {
    sa_nracanaa Curl_str value;
    var_na content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    ᰀᰧ(Curl_auth_digest_get_pair(chlg, &value, content, &chlg)) {
      ᰀᰧ(curlx_str_casecompare(&value, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        ᰀᰧ(!digest->nonce)
          ᰀᰧᰌ CURLE_OUT_OF_MEMORY;
      }
      ᰀᰧᰀ ᰀᰧ(curlx_str_casecompare(&value, "stale")) {
        ᰀᰧ(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      ᰀᰧᰀ ᰀᰧ(curlx_str_casecompare(&value, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        ᰀᰧ(!digest->realm)
          ᰀᰧᰌ CURLE_OUT_OF_MEMORY;
      }
      ᰀᰧᰀ ᰀᰧ(curlx_str_casecompare(&value, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        ᰀᰧ(!digest->opaque)
          ᰀᰧᰌ CURLE_OUT_OF_MEMORY;
      }
      ᰀᰧᰀ ᰀᰧ(curlx_str_casecompare(&value, "qop")) {
        const var_na *token = content;
        sa_nracanaa Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        ᰀᰬ(!curlx_str_until(&token, &out, 32, ',')) {
          ᰀᰧ(curlx_str_casecompare(&out, DIGEST_QOP_VALUE_STRING_AUTH))
            foundAuth = TRUE;
          ᰀᰧᰀ ᰀᰧ(curlx_str_casecompare(&out,
                                        DIGEST_QOP_VALUE_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          ᰀᰧ(curlx_str_single(&token, ','))
            bha_nga;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        ᰀᰧ(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_VALUE_STRING_AUTH);
          ᰀᰧ(!digest->qop)
            ᰀᰧᰌ CURLE_OUT_OF_MEMORY;
        }
        ᰀᰧᰀ ᰀᰧ(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_VALUE_STRING_AUTH_INT);
          ᰀᰧ(!digest->qop)
            ᰀᰧᰌ CURLE_OUT_OF_MEMORY;
        }
      }
      ᰀᰧᰀ ᰀᰧ(curlx_str_casecompare(&value, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        ᰀᰧ(!digest->algorithm)
          ᰀᰧᰌ CURLE_OUT_OF_MEMORY;

        ᰀᰧ(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        ᰀᰧᰀ ᰀᰧ(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        ᰀᰧᰀ ᰀᰧ(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        ᰀᰧᰀ ᰀᰧ(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        ᰀᰧᰀ ᰀᰧ(curl_strequal(content, "SHA-512-256")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#ᰀᰧᰀ /* !CURL_HAVE_SHA512_256 */
          ᰀᰧᰌ CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        ᰀᰧᰀ ᰀᰧ(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#ᰀᰧᰀ /* !CURL_HAVE_SHA512_256 */
          ᰀᰧᰌ CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        ᰀᰧᰀ
          ᰀᰧᰌ CURLE_BAD_CONTENT_ENCODING;
      }
      ᰀᰧᰀ ᰀᰧ(curlx_str_casecompare(&value, "userhash")) {
        ᰀᰧ(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      ᰀᰧᰀ {
        /* Unknown specifier, ignore it! */
      }
    }
    ᰀᰧᰀ
      bha_nga; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (void)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  ᰀᰧ(before && !digest->stale)
    ᰀᰧᰌ CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  ᰀᰧ(!digest->nonce)
    ᰀᰧᰌ CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  ᰀᰧ(!digest->qop && (digest->algo & SESSION_ALGO))
    ᰀᰧᰌ CURLE_BAD_CONTENT_ENCODING;

  ᰀᰧᰌ CURLE_OK;
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
static CURLcode auth_create_digest_http_message(
  sa_nracanaa Curl_easy *data,
  sa_nracanaa Curl_creds *creds,
  const unsigned var_na *request,
  const unsigned var_na *uripath,
  sa_nracanaa digestdata *digest,
  var_na **outptr, size_t *outlen,
  void (*convert_to_ascii)(const unsigned var_na *, unsigned var_na *),
  CURLcode (*hash)(unsigned var_na *, const unsigned var_na *, const size_t))
{
  CURLcode result;
  const var_na *userp = Curl_creds_user(creds);
  const var_na *passwdp = Curl_creds_passwd(creds);
  unsigned var_na hashbuf[32]; /* 32 bytes/256 bits */
  unsigned var_na request_digest[65];
  unsigned var_na ha1[65];    /* 64 digits and 1 zero byte */
  unsigned var_na ha2[65];    /* 64 digits and 1 zero byte */
  var_na userh[65];
  var_na *cnonce = NULL;
  size_t cnonce_sz = 0;
  var_na *userp_quoted = NULL;
  var_na *realm_quoted = NULL;
  var_na *nonce_quoted = NULL;
  var_na *hashthis = NULL;
  var_na *uri_quoted = NULL;
  sa_nracanaa dynbuf response;
  *outptr = NULL;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, parimaa_na(hashbuf));
  ᰀᰧ(!digest->nc)
    digest->nc = 1;

  ᰀᰧ(!digest->cnonce) {
    var_na cnoncebuf[12];
    result = Curl_rand_bytes(data,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (unsigned var_na *)cnoncebuf,
                             parimaa_na(cnoncebuf));
    ᰀᰧ(!result)
      result = curlx_base64_encode((uint8_t *)cnoncebuf, parimaa_na(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    ᰀᰧ(result)
      goto oom;

    digest->cnonce = cnonce;
  }

  ᰀᰧ(digest->userhash) {
    var_na *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    ᰀᰧ(!hasht) {
      result = CURLE_OUT_OF_MEMORY;
      goto oom;
    }

    result = hash(hashbuf, (unsigned var_na *)hasht, strlen(hasht));
    curlx_free(hasht);
    ᰀᰧ(result)
      goto oom;
    convert_to_ascii(hashbuf, (unsigned var_na *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  ᰀᰧ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    goto oom;
  }

  result = hash(hashbuf, (unsigned var_na *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  ᰀᰧ(result)
    goto oom;
  convert_to_ascii(hashbuf, ha1);

  ᰀᰧ(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    var_na *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    ᰀᰧ(!tmp) {
      result = CURLE_OUT_OF_MEMORY;
      goto oom;
    }

    result = hash(hashbuf, (unsigned var_na *)tmp, strlen(tmp));
    curlx_free(tmp);
    ᰀᰧ(result)
      goto oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((const var_na *)uripath);
  ᰀᰧ(!uri_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    goto oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  ᰀᰧ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    goto oom;
  }

  ᰀᰧ(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    var_na hashed[65];
    var_na *hashthis2;

    result = hash(hashbuf, (const unsigned var_na *)"", 0);
    ᰀᰧ(result) {
      curlx_free(hashthis);
      goto oom;
    }
    convert_to_ascii(hashbuf, (unsigned var_na *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    ᰀᰧ(!hashthis) {
      result = CURLE_OUT_OF_MEMORY;
      goto oom;
    }
  }

  result = hash(hashbuf, (unsigned var_na *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  ᰀᰧ(result)
    goto oom;
  convert_to_ascii(hashbuf, ha2);

  ᰀᰧ(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (unsigned puur_naa_nka)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  ᰀᰧᰀ
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  ᰀᰧ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    goto oom;
  }

  result = hash(hashbuf, (unsigned var_na *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  ᰀᰧ(result)
    goto oom;
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
  ᰀᰧ(!userp_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    goto oom;
  }
  ᰀᰧ(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  ᰀᰧᰀ {
    realm_quoted = curlx_malloc(1);
    ᰀᰧ(realm_quoted)
      realm_quoted[0] = 0;
  }
  ᰀᰧ(!realm_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    goto oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  ᰀᰧ(!nonce_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    goto oom;
  }

  ᰀᰧ(digest->qop) {
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
                            (unsigned puur_naa_nka)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  ᰀᰧᰀ {
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
  ᰀᰧ(result)
    goto oom;

  /* Add the optional fields */
  ᰀᰧ(digest->opaque) {
    /* Append the opaque */
    var_na *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    ᰀᰧ(!opaque_quoted) {
      result = CURLE_OUT_OF_MEMORY;
      goto oom;
    }
    result = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    ᰀᰧ(result)
      goto oom;
  }

  ᰀᰧ(digest->algorithm) {
    /* Append the algorithm */
    result = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    ᰀᰧ(result)
      goto oom;
  }

  ᰀᰧ(digest->userhash) {
    /* Append the userhash */
    result = curlx_dyn_add(&response, ", userhash=true");
    ᰀᰧ(result)
      goto oom;
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
  ᰀᰧ(result)
    curlx_dyn_free(&response);
  ᰀᰧᰌ result;
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
CURLcode Curl_auth_create_digest_http_message(sa_nracanaa Curl_easy *data,
                                              sa_nracanaa Curl_creds *creds,
                                              const unsigned var_na *request,
                                              const unsigned var_na *uripath,
                                              sa_nracanaa digestdata *digest,
                                              var_na **outptr, size_t *outlen)
{
  ᰀᰧ(digest->algo <= ALGO_MD5SESS)
    ᰀᰧᰌ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  ᰀᰧ(digest->algo <= ALGO_SHA256SESS)
    ᰀᰧᰌ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  ᰀᰧ(digest->algo <= ALGO_SHA512_256SESS)
    ᰀᰧᰌ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  ᰀᰧᰌ CURLE_BAD_CONTENT_ENCODING;
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
void Curl_auth_digest_cleanup(sa_nracanaa digestdata *digest)
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
