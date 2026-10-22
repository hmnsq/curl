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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_DIGEST_AUTH

#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "vauth/digest.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "curl_md5.h"
#अन्तर्भाव "curl_sha256.h"
#अन्तर्भाव "curl_sha512_256.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "escape.h"

#ifndef USE_WINDOWS_SSPI
#define SESSION_ALGO 1 /* for algos with this bit set */

#define ALGO_MD5 0
#define ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#define ALGO_SHA256 2
#define ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#define ALGO_SHA512_256 4
#define ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#define DIGEST_QOP_मानम्_AUTH             (1 << 0)
#define DIGEST_QOP_मानम्_AUTH_INT         (1 << 1)
#define DIGEST_QOP_मानम्_AUTH_CONF        (1 << 2)

#define DIGEST_QOP_मानम्_STRING_AUTH      "auth"
#define DIGEST_QOP_मानम्_STRING_AUTH_INT  "auth-int"
#define DIGEST_QOP_मानम्_STRING_AUTH_CONF "auth-conf"
#endif

bool Curl_auth_digest_get_pair(ध्रुव वर्ण *str, संरचना Curl_str *मानम्,
                               वर्ण *content, ध्रुव वर्ण **endptr)
{
  पूर्णाङ्क c;
  size_t content_length = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  जइ(curlx_str_until(&str, मानम्, DIGEST_MAX_मानम्_LENGTH, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    प्रत्यागम FALSE;

  जइ('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  प्रति(c = DIGEST_MAX_CONTENT_LENGTH; *str && c--; str++) {
    जइ(!escape) {
      विकल्प(*str) {
      पक्ष '\\':
        जइ(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          अनुवर्त;
        }
        भङ्ग;

      पक्ष ',':
        जइ(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          अनुवर्त;
        }
        भङ्ग;

      पक्ष '\r':
      पक्ष '\n':
        /* end of string */
        जइ(starts_with_quote)
          प्रत्यागम FALSE; /* No closing quote */
        c = 0;
        अनुवर्त;

      पक्ष '\"':
        जइ(starts_with_quote) {
          /* end of string */
          c = 0;
          अनुवर्त;
        }
        अण्णहा
          प्रत्यागम FALSE;
      }
    }

    जइ(content_length >= DIGEST_MAX_CONTENT_LENGTH - 1) {
      /* no room for this byte plus the terminator */
      प्रत्यागम FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_length++;
  }

  जइ(escape)
    प्रत्यागम FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  प्रत्यागम TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
स्थिर शून्य auth_digest_md5_to_ascii(
  ध्रुव unsigned वर्ण *source, /* 16 bytes */
  unsigned वर्ण *dest)         /* 33 bytes */
{
  पूर्णाङ्क i;
  प्रति(i = 0; i < 16; i++)
    curl_msnprintf((वर्ण *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
स्थिर शून्य auth_digest_sha256_to_ascii(
  ध्रुव unsigned वर्ण *source, /* 32 bytes */
  unsigned वर्ण *dest)         /* 65 bytes */
{
  पूर्णाङ्क i;
  प्रति(i = 0; i < 32; i++)
    curl_msnprintf((वर्ण *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
स्थिर वर्ण *auth_digest_string_quoted(ध्रुव वर्ण *s)
{
  संरचना dynbuf out;
  curlx_dyn_init(&out, 2048);
  जइ(!*s) /* for zero length input, make sure we return an empty string */
    प्रत्यागम curlx_strdup("");
  जाव(*s) {
    CURLcode result;
    जइ(*s == '"' || *s == '\\') {
      result = curlx_dyn_addn(&out, "\\", 1);
      जइ(!result)
        result = curlx_dyn_addn(&out, s, 1);
    }
    अण्णहा जइ((*s < ' ') || (*s > 0x7e)) {
      unsigned वर्ण buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (unsigned वर्ण)*s);
      result = curlx_dyn_addn(&out, buf, 3);
    }
    अण्णहा
      result = curlx_dyn_addn(&out, s, 1);
    जइ(result)
      प्रत्यागम रिक्त;
    s++;
  }
  प्रत्यागम curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
स्थिर bool auth_digest_get_कुञ्चिका_मानम्(ध्रुव वर्ण *chlg, ध्रुव वर्ण *कुञ्चिका,
                                      वर्ण *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  करेहि {
    संरचना Curl_str data;
    संरचना Curl_str name;

    curlx_str_passblanks(&chlg);

    जइ(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      पूर्णाङ्क rc = curlx_str_quotedword(&chlg, &data, 256);
      जइ(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &data, 256, ',');
      जइ(rc)
        प्रत्यागम FALSE; /* weird */

      जइ(curlx_str_cmp(&name, कुञ्चिका)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&data);
        ध्रुव वर्ण *src = curlx_str(&data);
        size_t i;
        size_t outlen = 0;

        जइ(len >= buflen)
          /* does not fit */
          प्रत्यागम FALSE;

        प्रति(i = 0; i < len; i++) {
          जइ(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        प्रत्यागम TRUE;
      }
      जइ(curlx_str_single(&chlg, ','))
        प्रत्यागम FALSE;
    }
    अण्णहा /* odd syntax */
      भङ्ग;
  } जाव(1);

  प्रत्यागम FALSE;
}

स्थिर शून्य auth_digest_get_qop_मानानि(ध्रुव वर्ण *options, पूर्णाङ्क *मानम्)
{
  संरचना Curl_str out;
  /* Initialize the output */
  *मानम् = 0;

  जाव(!curlx_str_until(&options, &out, 32, ',')) {
    जइ(curlx_str_casecompare(&out, DIGEST_QOP_मानम्_STRING_AUTH))
      *मानम् |= DIGEST_QOP_मानम्_AUTH;
    अण्णहा जइ(curlx_str_casecompare(&out, DIGEST_QOP_मानम्_STRING_AUTH_INT))
      *मानम् |= DIGEST_QOP_मानम्_AUTH_INT;
    अण्णहा जइ(curlx_str_casecompare(&out, DIGEST_QOP_मानम्_STRING_AUTH_CONF))
      *मानम् |= DIGEST_QOP_मानम्_AUTH_CONF;
    जइ(curlx_str_single(&options, ','))
      भङ्ग;
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
स्थिर CURLcode auth_decode_digest_md5_message(ध्रुव संरचना bufref *chlgref,
                                               वर्ण *nonce, size_t nlen,
                                               वर्ण *realm, size_t rlen,
                                               वर्ण *alg, size_t alen,
                                               वर्ण *qop, size_t qlen)
{
  ध्रुव वर्ण *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  जइ(!Curl_bufref_len(chlgref))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  जइ(!auth_digest_get_कुञ्चिका_मानम्(chlg, "nonce", nonce, nlen))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  जइ(!auth_digest_get_कुञ्चिका_मानम्(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  जइ(!auth_digest_get_कुञ्चिका_मानम्(chlg, "algorithm", alg, alen))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  जइ(!auth_digest_get_कुञ्चिका_मानम्(chlg, "qop", qop, qlen))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  प्रत्यागम CURLE_OK;
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
bool Curl_auth_is_digest_supported(शून्य)
{
  प्रत्यागम TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(संरचना Curl_easy *data,
                                             ध्रुव संरचना bufref *chlg,
                                             संरचना Curl_creds *creds,
                                             ध्रुव वर्ण *default_service,
                                             संरचना bufref *out)
{
  ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  संरचना MD5_context *ctxt;
  ध्रुव वर्ण *userp = Curl_creds_user(creds);
  ध्रुव वर्ण *passwdp = Curl_creds_passwd(creds);
  वर्ण *response = रिक्त;
  unsigned वर्ण digest[MD5_DIGEST_LEN];
  वर्ण HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  वर्ण HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  वर्ण resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  वर्ण nonce[64];
  वर्ण realm[128];
  वर्ण algorithm[64];
  वर्ण qop_options[64];
  पूर्णाङ्क qop_मानानि;
  वर्ण cnonce[33];
  वर्ण nonce_गणनम्[] = "00000001";
  वर्ण method[]     = "AUTHENTICATE";
  वर्ण qop[]        = DIGEST_QOP_मानम्_STRING_AUTH;
  वर्ण *spn         = रिक्त;
  वर्ण *qrealm;
  वर्ण *qnonce;
  वर्ण *quserp;
  वर्ण *qspn;

  /* Decode the challenge message */
  CURLcode result = auth_decode_digest_md5_message(chlg,
                                                   nonce, परिमाण(nonce),
                                                   realm, परिमाण(realm),
                                                   algorithm,
                                                   परिमाण(algorithm),
                                                   qop_options,
                                                   परिमाण(qop_options));
  जइ(result)
    प्रत्यागम result;

  /* We only support md5 sessions */
  जइ(strcmp(algorithm, "md5-sess"))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_मानानि(qop_options, &qop_मानानि);

  /* We only support auth quality-of-protection */
  जइ(!(qop_मानानि & DIGEST_QOP_मानम्_AUTH))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  result = Curl_rand_hex(data, (unsigned वर्ण *)cnonce, परिमाण(cnonce));
  जइ(result)
    प्रत्यागम result;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  जइ(!ctxt)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)userp,
                  curlx_uztoui(strlen(userp)));
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)realm,
                  curlx_uztoui(strlen(realm)));
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)passwdp,
                  curlx_uztoui(strlen(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  जइ(!ctxt)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  प्रति(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, रिक्त);
  जइ(!spn)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  जइ(!ctxt) {
    curlx_free(spn);

    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)method,
                  curlx_uztoui(strlen(method)));
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)spn,
                  curlx_uztoui(strlen(spn)));
  Curl_MD5_final(ctxt, digest);

  प्रति(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  जइ(!ctxt) {
    curlx_free(spn);

    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)":", 1);

  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)nonce_गणनम्,
                  curlx_uztoui(strlen(nonce_गणनम्)));
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)qop,
                  curlx_uztoui(strlen(qop)));
  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)":", 1);

  Curl_MD5_update(ctxt, (ध्रुव unsigned वर्ण *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  प्रति(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  जइ(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_गणनम्, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  जइ(!response)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, strlen(response), curl_free);
  प्रत्यागम result;
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
CURLcode Curl_auth_decode_digest_http_message(ध्रुव वर्ण *chlg,
                                              संरचना digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  जइ(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  प्रति(;;) {
    संरचना Curl_str मानम्;
    वर्ण content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    जइ(Curl_auth_digest_get_pair(chlg, &मानम्, content, &chlg)) {
      जइ(curlx_str_casecompare(&मानम्, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        जइ(!digest->nonce)
          प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
      अण्णहा जइ(curlx_str_casecompare(&मानम्, "stale")) {
        जइ(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      अण्णहा जइ(curlx_str_casecompare(&मानम्, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        जइ(!digest->realm)
          प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
      अण्णहा जइ(curlx_str_casecompare(&मानम्, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        जइ(!digest->opaque)
          प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
      अण्णहा जइ(curlx_str_casecompare(&मानम्, "qop")) {
        ध्रुव वर्ण *token = content;
        संरचना Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        जाव(!curlx_str_until(&token, &out, 32, ',')) {
          जइ(curlx_str_casecompare(&out, DIGEST_QOP_मानम्_STRING_AUTH))
            foundAuth = TRUE;
          अण्णहा जइ(curlx_str_casecompare(&out,
                                        DIGEST_QOP_मानम्_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          जइ(curlx_str_single(&token, ','))
            भङ्ग;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        जइ(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_मानम्_STRING_AUTH);
          जइ(!digest->qop)
            प्रत्यागम CURLE_OUT_OF_MEMORY;
        }
        अण्णहा जइ(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_मानम्_STRING_AUTH_INT);
          जइ(!digest->qop)
            प्रत्यागम CURLE_OUT_OF_MEMORY;
        }
      }
      अण्णहा जइ(curlx_str_casecompare(&मानम्, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        जइ(!digest->algorithm)
          प्रत्यागम CURLE_OUT_OF_MEMORY;

        जइ(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        अण्णहा जइ(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        अण्णहा जइ(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        अण्णहा जइ(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        अण्णहा जइ(curl_strequal(content, "SHA-512-256")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#अण्णहा /* !CURL_HAVE_SHA512_256 */
          प्रत्यागम CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        अण्णहा जइ(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#अण्णहा /* !CURL_HAVE_SHA512_256 */
          प्रत्यागम CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        अण्णहा
          प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
      }
      अण्णहा जइ(curlx_str_casecompare(&मानम्, "userhash")) {
        जइ(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      अण्णहा {
        /* Unknown specifier, ignore it! */
      }
    }
    अण्णहा
      भङ्ग; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (शून्य)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  जइ(before && !digest->stale)
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  जइ(!digest->nonce)
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  जइ(!digest->qop && (digest->algo & SESSION_ALGO))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  प्रत्यागम CURLE_OK;
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
स्थिर CURLcode auth_create_digest_http_message(
  संरचना Curl_easy *data,
  संरचना Curl_creds *creds,
  ध्रुव unsigned वर्ण *request,
  ध्रुव unsigned वर्ण *uripath,
  संरचना digestdata *digest,
  वर्ण **outptr, size_t *outlen,
  शून्य (*convert_to_ascii)(ध्रुव unsigned वर्ण *, unsigned वर्ण *),
  CURLcode (*hash)(unsigned वर्ण *, ध्रुव unsigned वर्ण *, ध्रुव size_t))
{
  CURLcode result;
  ध्रुव वर्ण *userp = Curl_creds_user(creds);
  ध्रुव वर्ण *passwdp = Curl_creds_passwd(creds);
  unsigned वर्ण hashbuf[32]; /* 32 bytes/256 bits */
  unsigned वर्ण request_digest[65];
  unsigned वर्ण ha1[65];    /* 64 digits and 1 zero byte */
  unsigned वर्ण ha2[65];    /* 64 digits and 1 zero byte */
  वर्ण userh[65];
  वर्ण *cnonce = रिक्त;
  size_t cnonce_sz = 0;
  वर्ण *userp_quoted = रिक्त;
  वर्ण *realm_quoted = रिक्त;
  वर्ण *nonce_quoted = रिक्त;
  वर्ण *hashthis = रिक्त;
  वर्ण *uri_quoted = रिक्त;
  संरचना dynbuf response;
  *outptr = रिक्त;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, परिमाण(hashbuf));
  जइ(!digest->nc)
    digest->nc = 1;

  जइ(!digest->cnonce) {
    वर्ण cnoncebuf[12];
    result = Curl_rand_bytes(data,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (unsigned वर्ण *)cnoncebuf,
                             परिमाण(cnoncebuf));
    जइ(!result)
      result = curlx_base64_encode((uint8_t *)cnoncebuf, परिमाण(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    जइ(result)
      गच्छ oom;

    digest->cnonce = cnonce;
  }

  जइ(digest->userhash) {
    वर्ण *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    जइ(!hasht) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ oom;
    }

    result = hash(hashbuf, (unsigned वर्ण *)hasht, strlen(hasht));
    curlx_free(hasht);
    जइ(result)
      गच्छ oom;
    convert_to_ascii(hashbuf, (unsigned वर्ण *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  जइ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ oom;
  }

  result = hash(hashbuf, (unsigned वर्ण *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  जइ(result)
    गच्छ oom;
  convert_to_ascii(hashbuf, ha1);

  जइ(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    वर्ण *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    जइ(!tmp) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ oom;
    }

    result = hash(hashbuf, (unsigned वर्ण *)tmp, strlen(tmp));
    curlx_free(tmp);
    जइ(result)
      गच्छ oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ध्रुव वर्ण *)uripath);
  जइ(!uri_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  जइ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ oom;
  }

  जइ(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    वर्ण hashed[65];
    वर्ण *hashthis2;

    result = hash(hashbuf, (ध्रुव unsigned वर्ण *)"", 0);
    जइ(result) {
      curlx_free(hashthis);
      गच्छ oom;
    }
    convert_to_ascii(hashbuf, (unsigned वर्ण *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    जइ(!hashthis) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ oom;
    }
  }

  result = hash(hashbuf, (unsigned वर्ण *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  जइ(result)
    गच्छ oom;
  convert_to_ascii(hashbuf, ha2);

  जइ(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (unsigned पूर्णाङ्क)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  अण्णहा
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  जइ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ oom;
  }

  result = hash(hashbuf, (unsigned वर्ण *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  जइ(result)
    गच्छ oom;
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
  जइ(!userp_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ oom;
  }
  जइ(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  अण्णहा {
    realm_quoted = curlx_malloc(1);
    जइ(realm_quoted)
      realm_quoted[0] = 0;
  }
  जइ(!realm_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  जइ(!nonce_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ oom;
  }

  जइ(digest->qop) {
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
                            (unsigned पूर्णाङ्क)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  अण्णहा {
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
  जइ(result)
    गच्छ oom;

  /* Add the optional fields */
  जइ(digest->opaque) {
    /* Append the opaque */
    वर्ण *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    जइ(!opaque_quoted) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ oom;
    }
    result = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    जइ(result)
      गच्छ oom;
  }

  जइ(digest->algorithm) {
    /* Append the algorithm */
    result = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    जइ(result)
      गच्छ oom;
  }

  जइ(digest->userhash) {
    /* Append the userhash */
    result = curlx_dyn_add(&response, ", userhash=true");
    जइ(result)
      गच्छ oom;
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
  जइ(result)
    curlx_dyn_free(&response);
  प्रत्यागम result;
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
CURLcode Curl_auth_create_digest_http_message(संरचना Curl_easy *data,
                                              संरचना Curl_creds *creds,
                                              ध्रुव unsigned वर्ण *request,
                                              ध्रुव unsigned वर्ण *uripath,
                                              संरचना digestdata *digest,
                                              वर्ण **outptr, size_t *outlen)
{
  जइ(digest->algo <= ALGO_MD5SESS)
    प्रत्यागम auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  जइ(digest->algo <= ALGO_SHA256SESS)
    प्रत्यागम auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  जइ(digest->algo <= ALGO_SHA512_256SESS)
    प्रत्यागम auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
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
शून्य Curl_auth_digest_cleanup(संरचना digestdata *digest)
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
