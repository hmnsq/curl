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
#شامل "curl_setup.h"

#ifndef CURL_DISABLE_DIGEST_AUTH

#شامل "vauth/vauth.h"
#شامل "vauth/digest.h"
#شامل "curlx/base64.h"
#شامل "curl_md5.h"
#شامل "curl_sha256.h"
#شامل "curl_sha512_256.h"
#شامل "curlx/strparse.h"
#شامل "rand.h"
#شامل "escape.h"

#ifndef USE_WINDOWS_SSPI
#define SESSION_ALGO 1 /* for algos with this bit set */

#define ALGO_MD5 0
#define ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#define ALGO_SHA256 2
#define ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#define ALGO_SHA512_256 4
#define ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#define DIGEST_QOP_مقدار_AUTH             (1 << 0)
#define DIGEST_QOP_مقدار_AUTH_INT         (1 << 1)
#define DIGEST_QOP_مقدار_AUTH_CONF        (1 << 2)

#define DIGEST_QOP_مقدار_STRING_AUTH      "auth"
#define DIGEST_QOP_مقدار_STRING_AUTH_INT  "auth-int"
#define DIGEST_QOP_مقدار_STRING_AUTH_CONF "auth-conf"
#endif

bool Curl_auth_digest_get_pair(ثابت نویسه *str, ساختار Curl_str *مقدار,
                               نویسه *content, ثابت نویسه **endptr)
{
  صحیح c;
  size_t content_length = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  اگر(curlx_str_until(&str, مقدار, DIGEST_MAX_مقدار_LENGTH, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    بازگشت FALSE;

  اگر('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  برای(c = DIGEST_MAX_CONTENT_LENGTH; *str && c--; str++) {
    اگر(!escape) {
      انتخاب(*str) {
      حالت '\\':
        اگر(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          ادامه;
        }
        بشکن;

      حالت ',':
        اگر(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          ادامه;
        }
        بشکن;

      حالت '\r':
      حالت '\n':
        /* end of string */
        اگر(starts_with_quote)
          بازگشت FALSE; /* No closing quote */
        c = 0;
        ادامه;

      حالت '\"':
        اگر(starts_with_quote) {
          /* end of string */
          c = 0;
          ادامه;
        }
        وگرنه
          بازگشت FALSE;
      }
    }

    اگر(content_length >= DIGEST_MAX_CONTENT_LENGTH - 1) {
      /* no room for this byte plus the terminator */
      بازگشت FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_length++;
  }

  اگر(escape)
    بازگشت FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  بازگشت TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
ایستا تهی auth_digest_md5_to_ascii(
  ثابت unsigned نویسه *source, /* 16 bytes */
  unsigned نویسه *dest)         /* 33 bytes */
{
  صحیح i;
  برای(i = 0; i < 16; i++)
    curl_msnprintf((نویسه *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
ایستا تهی auth_digest_sha256_to_ascii(
  ثابت unsigned نویسه *source, /* 32 bytes */
  unsigned نویسه *dest)         /* 65 bytes */
{
  صحیح i;
  برای(i = 0; i < 32; i++)
    curl_msnprintf((نویسه *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
ایستا نویسه *auth_digest_string_quoted(ثابت نویسه *s)
{
  ساختار dynbuf out;
  curlx_dyn_init(&out, 2048);
  اگر(!*s) /* for zero length input, make sure we return an empty string */
    بازگشت curlx_strdup("");
  وقتی(*s) {
    CURLcode result;
    اگر(*s == '"' || *s == '\\') {
      result = curlx_dyn_addn(&out, "\\", 1);
      اگر(!result)
        result = curlx_dyn_addn(&out, s, 1);
    }
    وگرنه اگر((*s < ' ') || (*s > 0x7e)) {
      unsigned نویسه buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (unsigned نویسه)*s);
      result = curlx_dyn_addn(&out, buf, 3);
    }
    وگرنه
      result = curlx_dyn_addn(&out, s, 1);
    اگر(result)
      بازگشت پوچ;
    s++;
  }
  بازگشت curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
ایستا bool auth_digest_get_کلید_مقدار(ثابت نویسه *chlg, ثابت نویسه *کلید,
                                      نویسه *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  انجامبده {
    ساختار Curl_str data;
    ساختار Curl_str name;

    curlx_str_passblanks(&chlg);

    اگر(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      صحیح rc = curlx_str_quotedword(&chlg, &data, 256);
      اگر(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &data, 256, ',');
      اگر(rc)
        بازگشت FALSE; /* weird */

      اگر(curlx_str_cmp(&name, کلید)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&data);
        ثابت نویسه *src = curlx_str(&data);
        size_t i;
        size_t outlen = 0;

        اگر(len >= buflen)
          /* does not fit */
          بازگشت FALSE;

        برای(i = 0; i < len; i++) {
          اگر(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        بازگشت TRUE;
      }
      اگر(curlx_str_single(&chlg, ','))
        بازگشت FALSE;
    }
    وگرنه /* odd syntax */
      بشکن;
  } وقتی(1);

  بازگشت FALSE;
}

ایستا تهی auth_digest_get_qop_مقادیر(ثابت نویسه *options, صحیح *مقدار)
{
  ساختار Curl_str out;
  /* Initialize the output */
  *مقدار = 0;

  وقتی(!curlx_str_until(&options, &out, 32, ',')) {
    اگر(curlx_str_casecompare(&out, DIGEST_QOP_مقدار_STRING_AUTH))
      *مقدار |= DIGEST_QOP_مقدار_AUTH;
    وگرنه اگر(curlx_str_casecompare(&out, DIGEST_QOP_مقدار_STRING_AUTH_INT))
      *مقدار |= DIGEST_QOP_مقدار_AUTH_INT;
    وگرنه اگر(curlx_str_casecompare(&out, DIGEST_QOP_مقدار_STRING_AUTH_CONF))
      *مقدار |= DIGEST_QOP_مقدار_AUTH_CONF;
    اگر(curlx_str_single(&options, ','))
      بشکن;
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
ایستا CURLcode auth_decode_digest_md5_message(ثابت ساختار bufref *chlgref,
                                               نویسه *nonce, size_t nlen,
                                               نویسه *realm, size_t rlen,
                                               نویسه *alg, size_t alen,
                                               نویسه *qop, size_t qlen)
{
  ثابت نویسه *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  اگر(!Curl_bufref_len(chlgref))
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  اگر(!auth_digest_get_کلید_مقدار(chlg, "nonce", nonce, nlen))
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  اگر(!auth_digest_get_کلید_مقدار(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  اگر(!auth_digest_get_کلید_مقدار(chlg, "algorithm", alg, alen))
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  اگر(!auth_digest_get_کلید_مقدار(chlg, "qop", qop, qlen))
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  بازگشت CURLE_OK;
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
bool Curl_auth_is_digest_supported(تهی)
{
  بازگشت TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(ساختار Curl_easy *data,
                                             ثابت ساختار bufref *chlg,
                                             ساختار Curl_creds *creds,
                                             ثابت نویسه *default_service,
                                             ساختار bufref *out)
{
  ثابت نویسه *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  ساختار MD5_context *ctxt;
  ثابت نویسه *userp = Curl_creds_user(creds);
  ثابت نویسه *passwdp = Curl_creds_passwd(creds);
  نویسه *response = پوچ;
  unsigned نویسه digest[MD5_DIGEST_LEN];
  نویسه HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  نویسه HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  نویسه resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  نویسه nonce[64];
  نویسه realm[128];
  نویسه algorithm[64];
  نویسه qop_options[64];
  صحیح qop_مقادیر;
  نویسه cnonce[33];
  نویسه nonce_شمار[] = "00000001";
  نویسه method[]     = "AUTHENTICATE";
  نویسه qop[]        = DIGEST_QOP_مقدار_STRING_AUTH;
  نویسه *spn         = پوچ;
  نویسه *qrealm;
  نویسه *qnonce;
  نویسه *quserp;
  نویسه *qspn;

  /* Decode the challenge message */
  CURLcode result = auth_decode_digest_md5_message(chlg,
                                                   nonce, اندازه(nonce),
                                                   realm, اندازه(realm),
                                                   algorithm,
                                                   اندازه(algorithm),
                                                   qop_options,
                                                   اندازه(qop_options));
  اگر(result)
    بازگشت result;

  /* We only support md5 sessions */
  اگر(strcmp(algorithm, "md5-sess"))
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_مقادیر(qop_options, &qop_مقادیر);

  /* We only support auth quality-of-protection */
  اگر(!(qop_مقادیر & DIGEST_QOP_مقدار_AUTH))
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  result = Curl_rand_hex(data, (unsigned نویسه *)cnonce, اندازه(cnonce));
  اگر(result)
    بازگشت result;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  اگر(!ctxt)
    بازگشت CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)userp,
                  curlx_uztoui(strlen(userp)));
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)realm,
                  curlx_uztoui(strlen(realm)));
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)passwdp,
                  curlx_uztoui(strlen(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  اگر(!ctxt)
    بازگشت CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  برای(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, پوچ);
  اگر(!spn)
    بازگشت CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  اگر(!ctxt) {
    curlx_free(spn);

    بازگشت CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)method,
                  curlx_uztoui(strlen(method)));
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)spn,
                  curlx_uztoui(strlen(spn)));
  Curl_MD5_final(ctxt, digest);

  برای(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  اگر(!ctxt) {
    curlx_free(spn);

    بازگشت CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)":", 1);

  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)nonce_شمار,
                  curlx_uztoui(strlen(nonce_شمار)));
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)":", 1);
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)qop,
                  curlx_uztoui(strlen(qop)));
  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)":", 1);

  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  برای(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  اگر(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_شمار, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  اگر(!response)
    بازگشت CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, strlen(response), curl_free);
  بازگشت result;
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
CURLcode Curl_auth_decode_digest_http_message(ثابت نویسه *chlg,
                                              ساختار digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  اگر(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  برای(;;) {
    ساختار Curl_str مقدار;
    نویسه content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    اگر(Curl_auth_digest_get_pair(chlg, &مقدار, content, &chlg)) {
      اگر(curlx_str_casecompare(&مقدار, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        اگر(!digest->nonce)
          بازگشت CURLE_OUT_OF_MEMORY;
      }
      وگرنه اگر(curlx_str_casecompare(&مقدار, "stale")) {
        اگر(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      وگرنه اگر(curlx_str_casecompare(&مقدار, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        اگر(!digest->realm)
          بازگشت CURLE_OUT_OF_MEMORY;
      }
      وگرنه اگر(curlx_str_casecompare(&مقدار, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        اگر(!digest->opaque)
          بازگشت CURLE_OUT_OF_MEMORY;
      }
      وگرنه اگر(curlx_str_casecompare(&مقدار, "qop")) {
        ثابت نویسه *token = content;
        ساختار Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        وقتی(!curlx_str_until(&token, &out, 32, ',')) {
          اگر(curlx_str_casecompare(&out, DIGEST_QOP_مقدار_STRING_AUTH))
            foundAuth = TRUE;
          وگرنه اگر(curlx_str_casecompare(&out,
                                        DIGEST_QOP_مقدار_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          اگر(curlx_str_single(&token, ','))
            بشکن;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        اگر(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_مقدار_STRING_AUTH);
          اگر(!digest->qop)
            بازگشت CURLE_OUT_OF_MEMORY;
        }
        وگرنه اگر(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_مقدار_STRING_AUTH_INT);
          اگر(!digest->qop)
            بازگشت CURLE_OUT_OF_MEMORY;
        }
      }
      وگرنه اگر(curlx_str_casecompare(&مقدار, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        اگر(!digest->algorithm)
          بازگشت CURLE_OUT_OF_MEMORY;

        اگر(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        وگرنه اگر(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        وگرنه اگر(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        وگرنه اگر(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        وگرنه اگر(curl_strequal(content, "SHA-512-256")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#وگرنه /* !CURL_HAVE_SHA512_256 */
          بازگشت CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        وگرنه اگر(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#وگرنه /* !CURL_HAVE_SHA512_256 */
          بازگشت CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        وگرنه
          بازگشت CURLE_BAD_CONTENT_ENCODING;
      }
      وگرنه اگر(curlx_str_casecompare(&مقدار, "userhash")) {
        اگر(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      وگرنه {
        /* Unknown specifier, ignore it! */
      }
    }
    وگرنه
      بشکن; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (تهی)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  اگر(before && !digest->stale)
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  اگر(!digest->nonce)
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  اگر(!digest->qop && (digest->algo & SESSION_ALGO))
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  بازگشت CURLE_OK;
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
ایستا CURLcode auth_create_digest_http_message(
  ساختار Curl_easy *data,
  ساختار Curl_creds *creds,
  ثابت unsigned نویسه *request,
  ثابت unsigned نویسه *uripath,
  ساختار digestdata *digest,
  نویسه **outptr, size_t *outlen,
  تهی (*convert_to_ascii)(ثابت unsigned نویسه *, unsigned نویسه *),
  CURLcode (*hash)(unsigned نویسه *, ثابت unsigned نویسه *, ثابت size_t))
{
  CURLcode result;
  ثابت نویسه *userp = Curl_creds_user(creds);
  ثابت نویسه *passwdp = Curl_creds_passwd(creds);
  unsigned نویسه hashbuf[32]; /* 32 bytes/256 bits */
  unsigned نویسه request_digest[65];
  unsigned نویسه ha1[65];    /* 64 digits and 1 zero byte */
  unsigned نویسه ha2[65];    /* 64 digits and 1 zero byte */
  نویسه userh[65];
  نویسه *cnonce = پوچ;
  size_t cnonce_sz = 0;
  نویسه *userp_quoted = پوچ;
  نویسه *realm_quoted = پوچ;
  نویسه *nonce_quoted = پوچ;
  نویسه *hashthis = پوچ;
  نویسه *uri_quoted = پوچ;
  ساختار dynbuf response;
  *outptr = پوچ;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, اندازه(hashbuf));
  اگر(!digest->nc)
    digest->nc = 1;

  اگر(!digest->cnonce) {
    نویسه cnoncebuf[12];
    result = Curl_rand_bytes(data,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (unsigned نویسه *)cnoncebuf,
                             اندازه(cnoncebuf));
    اگر(!result)
      result = curlx_base64_encode((uint8_t *)cnoncebuf, اندازه(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    اگر(result)
      برو oom;

    digest->cnonce = cnonce;
  }

  اگر(digest->userhash) {
    نویسه *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    اگر(!hasht) {
      result = CURLE_OUT_OF_MEMORY;
      برو oom;
    }

    result = hash(hashbuf, (unsigned نویسه *)hasht, strlen(hasht));
    curlx_free(hasht);
    اگر(result)
      برو oom;
    convert_to_ascii(hashbuf, (unsigned نویسه *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  اگر(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    برو oom;
  }

  result = hash(hashbuf, (unsigned نویسه *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  اگر(result)
    برو oom;
  convert_to_ascii(hashbuf, ha1);

  اگر(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    نویسه *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    اگر(!tmp) {
      result = CURLE_OUT_OF_MEMORY;
      برو oom;
    }

    result = hash(hashbuf, (unsigned نویسه *)tmp, strlen(tmp));
    curlx_free(tmp);
    اگر(result)
      برو oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ثابت نویسه *)uripath);
  اگر(!uri_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    برو oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  اگر(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    برو oom;
  }

  اگر(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    نویسه hashed[65];
    نویسه *hashthis2;

    result = hash(hashbuf, (ثابت unsigned نویسه *)"", 0);
    اگر(result) {
      curlx_free(hashthis);
      برو oom;
    }
    convert_to_ascii(hashbuf, (unsigned نویسه *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    اگر(!hashthis) {
      result = CURLE_OUT_OF_MEMORY;
      برو oom;
    }
  }

  result = hash(hashbuf, (unsigned نویسه *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  اگر(result)
    برو oom;
  convert_to_ascii(hashbuf, ha2);

  اگر(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (unsigned صحیح)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  وگرنه
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  اگر(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    برو oom;
  }

  result = hash(hashbuf, (unsigned نویسه *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  اگر(result)
    برو oom;
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
  اگر(!userp_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    برو oom;
  }
  اگر(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  وگرنه {
    realm_quoted = curlx_malloc(1);
    اگر(realm_quoted)
      realm_quoted[0] = 0;
  }
  اگر(!realm_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    برو oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  اگر(!nonce_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    برو oom;
  }

  اگر(digest->qop) {
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
                            (unsigned صحیح)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  وگرنه {
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
  اگر(result)
    برو oom;

  /* Add the optional fields */
  اگر(digest->opaque) {
    /* Append the opaque */
    نویسه *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    اگر(!opaque_quoted) {
      result = CURLE_OUT_OF_MEMORY;
      برو oom;
    }
    result = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    اگر(result)
      برو oom;
  }

  اگر(digest->algorithm) {
    /* Append the algorithm */
    result = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    اگر(result)
      برو oom;
  }

  اگر(digest->userhash) {
    /* Append the userhash */
    result = curlx_dyn_add(&response, ", userhash=true");
    اگر(result)
      برو oom;
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
  اگر(result)
    curlx_dyn_free(&response);
  بازگشت result;
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
CURLcode Curl_auth_create_digest_http_message(ساختار Curl_easy *data,
                                              ساختار Curl_creds *creds,
                                              ثابت unsigned نویسه *request,
                                              ثابت unsigned نویسه *uripath,
                                              ساختار digestdata *digest,
                                              نویسه **outptr, size_t *outlen)
{
  اگر(digest->algo <= ALGO_MD5SESS)
    بازگشت auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  اگر(digest->algo <= ALGO_SHA256SESS)
    بازگشت auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  اگر(digest->algo <= ALGO_SHA512_256SESS)
    بازگشت auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  بازگشت CURLE_BAD_CONTENT_ENCODING;
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
تهی Curl_auth_digest_cleanup(ساختار digestdata *digest)
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
