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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_DIGEST_AUTH

#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "vauth/digest.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "curl_md5.h"
#অন্তর্ভাব "curl_sha256.h"
#অন্তর্ভাব "curl_sha512_256.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "escape.h"

#ifndef USE_WINDOWS_SSPI
#define SESSION_ALGO 1 /* for algos with this bit set */

#define ALGO_MD5 0
#define ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#define ALGO_SHA256 2
#define ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#define ALGO_SHA512_256 4
#define ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#define DIGEST_QOP_মান_AUTH             (1 << 0)
#define DIGEST_QOP_মান_AUTH_INT         (1 << 1)
#define DIGEST_QOP_মান_AUTH_CONF        (1 << 2)

#define DIGEST_QOP_মান_STRING_AUTH      "auth"
#define DIGEST_QOP_মান_STRING_AUTH_INT  "auth-int"
#define DIGEST_QOP_মান_STRING_AUTH_CONF "auth-conf"
#endif

bool Curl_auth_digest_get_pair(ধ্রুবক অক্ষর *str, কাঠামো Curl_str *মান,
                               অক্ষর *content, ধ্রুবক অক্ষর **endptr)
{
  পূর্ণসংখ্যা c;
  size_t content_দৈর্ঘ্য = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  যদি(curlx_str_until(&str, মান, DIGEST_MAX_মান_দৈর্ঘ্য, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    ফেরত FALSE;

  যদি('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  জন্য(c = DIGEST_MAX_CONTENT_দৈর্ঘ্য; *str && c--; str++) {
    যদি(!escape) {
      নির্বাচন(*str) {
      ক্ষেত্র '\\':
        যদি(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          চালু;
        }
        বিরতি;

      ক্ষেত্র ',':
        যদি(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          চালু;
        }
        বিরতি;

      ক্ষেত্র '\r':
      ক্ষেত্র '\n':
        /* end of string */
        যদি(starts_with_quote)
          ফেরত FALSE; /* No closing quote */
        c = 0;
        চালু;

      ক্ষেত্র '\"':
        যদি(starts_with_quote) {
          /* end of string */
          c = 0;
          চালু;
        }
        অন্যথা
          ফেরত FALSE;
      }
    }

    যদি(content_দৈর্ঘ্য >= DIGEST_MAX_CONTENT_দৈর্ঘ্য - 1) {
      /* no room for this byte plus the terminator */
      ফেরত FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_দৈর্ঘ্য++;
  }

  যদি(escape)
    ফেরত FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  ফেরত TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
স্থির শূন্য auth_digest_md5_to_ascii(
  ধ্রুবক unsigned অক্ষর *source, /* 16 bytes */
  unsigned অক্ষর *dest)         /* 33 bytes */
{
  পূর্ণসংখ্যা i;
  জন্য(i = 0; i < 16; i++)
    curl_msnprintf((অক্ষর *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
স্থির শূন্য auth_digest_sha256_to_ascii(
  ধ্রুবক unsigned অক্ষর *source, /* 32 bytes */
  unsigned অক্ষর *dest)         /* 65 bytes */
{
  পূর্ণসংখ্যা i;
  জন্য(i = 0; i < 32; i++)
    curl_msnprintf((অক্ষর *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
স্থির অক্ষর *auth_digest_string_quoted(ধ্রুবক অক্ষর *s)
{
  কাঠামো dynbuf out;
  curlx_dyn_init(&out, 2048);
  যদি(!*s) /* for zero length input, make sure we return an empty string */
    ফেরত curlx_strdup("");
  যখন(*s) {
    CURLcode ফলাফল;
    যদি(*s == '"' || *s == '\\') {
      ফলাফল = curlx_dyn_addn(&out, "\\", 1);
      যদি(!ফলাফল)
        ফলাফল = curlx_dyn_addn(&out, s, 1);
    }
    অন্যথা যদি((*s < ' ') || (*s > 0x7e)) {
      unsigned অক্ষর buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (unsigned অক্ষর)*s);
      ফলাফল = curlx_dyn_addn(&out, buf, 3);
    }
    অন্যথা
      ফলাফল = curlx_dyn_addn(&out, s, 1);
    যদি(ফলাফল)
      ফেরত ফাঁকা;
    s++;
  }
  ফেরত curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
স্থির bool auth_digest_get_চাবি_মান(ধ্রুবক অক্ষর *chlg, ধ্রুবক অক্ষর *চাবি,
                                      অক্ষর *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  করো {
    কাঠামো Curl_str তথ্য;
    কাঠামো Curl_str name;

    curlx_str_passblanks(&chlg);

    যদি(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      পূর্ণসংখ্যা rc = curlx_str_quotedword(&chlg, &তথ্য, 256);
      যদি(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &তথ্য, 256, ',');
      যদি(rc)
        ফেরত FALSE; /* weird */

      যদি(curlx_str_cmp(&name, চাবি)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&তথ্য);
        ধ্রুবক অক্ষর *src = curlx_str(&তথ্য);
        size_t i;
        size_t outlen = 0;

        যদি(len >= buflen)
          /* does not fit */
          ফেরত FALSE;

        জন্য(i = 0; i < len; i++) {
          যদি(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        ফেরত TRUE;
      }
      যদি(curlx_str_single(&chlg, ','))
        ফেরত FALSE;
    }
    অন্যথা /* odd syntax */
      বিরতি;
  } যখন(1);

  ফেরত FALSE;
}

স্থির শূন্য auth_digest_get_qop_মানসমূহ(ধ্রুবক অক্ষর *options, পূর্ণসংখ্যা *মান)
{
  কাঠামো Curl_str out;
  /* Initialize the output */
  *মান = 0;

  যখন(!curlx_str_until(&options, &out, 32, ',')) {
    যদি(curlx_str_casecompare(&out, DIGEST_QOP_মান_STRING_AUTH))
      *মান |= DIGEST_QOP_মান_AUTH;
    অন্যথা যদি(curlx_str_casecompare(&out, DIGEST_QOP_মান_STRING_AUTH_INT))
      *মান |= DIGEST_QOP_মান_AUTH_INT;
    অন্যথা যদি(curlx_str_casecompare(&out, DIGEST_QOP_মান_STRING_AUTH_CONF))
      *মান |= DIGEST_QOP_মান_AUTH_CONF;
    যদি(curlx_str_single(&options, ','))
      বিরতি;
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
স্থির CURLcode auth_decode_digest_md5_message(ধ্রুবক কাঠামো bufref *chlgref,
                                               অক্ষর *nonce, size_t nlen,
                                               অক্ষর *realm, size_t rlen,
                                               অক্ষর *alg, size_t alen,
                                               অক্ষর *qop, size_t qlen)
{
  ধ্রুবক অক্ষর *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  যদি(!Curl_bufref_len(chlgref))
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  যদি(!auth_digest_get_চাবি_মান(chlg, "nonce", nonce, nlen))
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  যদি(!auth_digest_get_চাবি_মান(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  যদি(!auth_digest_get_চাবি_মান(chlg, "algorithm", alg, alen))
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  যদি(!auth_digest_get_চাবি_মান(chlg, "qop", qop, qlen))
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  ফেরত CURLE_OK;
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
bool Curl_auth_is_digest_supported(শূন্য)
{
  ফেরত TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(কাঠামো Curl_easy *তথ্য,
                                             ধ্রুবক কাঠামো bufref *chlg,
                                             কাঠামো Curl_creds *creds,
                                             ধ্রুবক অক্ষর *default_service,
                                             কাঠামো bufref *out)
{
  ধ্রুবক অক্ষর *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  কাঠামো MD5_context *ctxt;
  ধ্রুবক অক্ষর *userp = Curl_creds_user(creds);
  ধ্রুবক অক্ষর *passwdp = Curl_creds_passwd(creds);
  অক্ষর *response = ফাঁকা;
  unsigned অক্ষর digest[MD5_DIGEST_LEN];
  অক্ষর HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  অক্ষর HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  অক্ষর resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  অক্ষর nonce[64];
  অক্ষর realm[128];
  অক্ষর algorithm[64];
  অক্ষর qop_options[64];
  পূর্ণসংখ্যা qop_মানসমূহ;
  অক্ষর cnonce[33];
  অক্ষর nonce_গুনতি[] = "00000001";
  অক্ষর method[]     = "AUTHENTICATE";
  অক্ষর qop[]        = DIGEST_QOP_মান_STRING_AUTH;
  অক্ষর *spn         = ফাঁকা;
  অক্ষর *qrealm;
  অক্ষর *qnonce;
  অক্ষর *quserp;
  অক্ষর *qspn;

  /* Decode the challenge message */
  CURLcode ফলাফল = auth_decode_digest_md5_message(chlg,
                                                   nonce, আকার(nonce),
                                                   realm, আকার(realm),
                                                   algorithm,
                                                   আকার(algorithm),
                                                   qop_options,
                                                   আকার(qop_options));
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* We only support md5 sessions */
  যদি(strcmp(algorithm, "md5-sess"))
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_মানসমূহ(qop_options, &qop_মানসমূহ);

  /* We only support auth quality-of-protection */
  যদি(!(qop_মানসমূহ & DIGEST_QOP_মান_AUTH))
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  ফলাফল = Curl_rand_hex(তথ্য, (unsigned অক্ষর *)cnonce, আকার(cnonce));
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  যদি(!ctxt)
    ফেরত CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)userp,
                  curlx_uztoui(strlen(userp)));
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)realm,
                  curlx_uztoui(strlen(realm)));
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)passwdp,
                  curlx_uztoui(strlen(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  যদি(!ctxt)
    ফেরত CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  জন্য(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, তথ্য->state.origin->hostname, ফাঁকা);
  যদি(!spn)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  যদি(!ctxt) {
    curlx_free(spn);

    ফেরত CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)method,
                  curlx_uztoui(strlen(method)));
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)spn,
                  curlx_uztoui(strlen(spn)));
  Curl_MD5_final(ctxt, digest);

  জন্য(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  যদি(!ctxt) {
    curlx_free(spn);

    ফেরত CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)":", 1);

  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)nonce_গুনতি,
                  curlx_uztoui(strlen(nonce_গুনতি)));
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)qop,
                  curlx_uztoui(strlen(qop)));
  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)":", 1);

  Curl_MD5_update(ctxt, (ধ্রুবক unsigned অক্ষর *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  জন্য(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  যদি(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_গুনতি, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  যদি(!response)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, strlen(response), curl_free);
  ফেরত ফলাফল;
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
CURLcode Curl_auth_decode_digest_http_message(ধ্রুবক অক্ষর *chlg,
                                              কাঠামো digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  যদি(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  জন্য(;;) {
    কাঠামো Curl_str মান;
    অক্ষর content[DIGEST_MAX_CONTENT_দৈর্ঘ্য];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    যদি(Curl_auth_digest_get_pair(chlg, &মান, content, &chlg)) {
      যদি(curlx_str_casecompare(&মান, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        যদি(!digest->nonce)
          ফেরত CURLE_OUT_OF_MEMORY;
      }
      অন্যথা যদি(curlx_str_casecompare(&মান, "stale")) {
        যদি(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      অন্যথা যদি(curlx_str_casecompare(&মান, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        যদি(!digest->realm)
          ফেরত CURLE_OUT_OF_MEMORY;
      }
      অন্যথা যদি(curlx_str_casecompare(&মান, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        যদি(!digest->opaque)
          ফেরত CURLE_OUT_OF_MEMORY;
      }
      অন্যথা যদি(curlx_str_casecompare(&মান, "qop")) {
        ধ্রুবক অক্ষর *token = content;
        কাঠামো Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        যখন(!curlx_str_until(&token, &out, 32, ',')) {
          যদি(curlx_str_casecompare(&out, DIGEST_QOP_মান_STRING_AUTH))
            foundAuth = TRUE;
          অন্যথা যদি(curlx_str_casecompare(&out,
                                        DIGEST_QOP_মান_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          যদি(curlx_str_single(&token, ','))
            বিরতি;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        যদি(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_মান_STRING_AUTH);
          যদি(!digest->qop)
            ফেরত CURLE_OUT_OF_MEMORY;
        }
        অন্যথা যদি(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_মান_STRING_AUTH_INT);
          যদি(!digest->qop)
            ফেরত CURLE_OUT_OF_MEMORY;
        }
      }
      অন্যথা যদি(curlx_str_casecompare(&মান, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        যদি(!digest->algorithm)
          ফেরত CURLE_OUT_OF_MEMORY;

        যদি(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        অন্যথা যদি(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        অন্যথা যদি(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        অন্যথা যদি(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        অন্যথা যদি(curl_strequal(content, "SHA-512-256")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#অন্যথা /* !CURL_HAVE_SHA512_256 */
          ফেরত CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        অন্যথা যদি(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#অন্যথা /* !CURL_HAVE_SHA512_256 */
          ফেরত CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        অন্যথা
          ফেরত CURLE_BAD_CONTENT_ENCODING;
      }
      অন্যথা যদি(curlx_str_casecompare(&মান, "userhash")) {
        যদি(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      অন্যথা {
        /* Unknown specifier, ignore it! */
      }
    }
    অন্যথা
      বিরতি; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (শূন্য)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  যদি(before && !digest->stale)
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  যদি(!digest->nonce)
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  যদি(!digest->qop && (digest->algo & SESSION_ALGO))
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  ফেরত CURLE_OK;
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
স্থির CURLcode auth_create_digest_http_message(
  কাঠামো Curl_easy *তথ্য,
  কাঠামো Curl_creds *creds,
  ধ্রুবক unsigned অক্ষর *request,
  ধ্রুবক unsigned অক্ষর *uripath,
  কাঠামো digestdata *digest,
  অক্ষর **outptr, size_t *outlen,
  শূন্য (*convert_to_ascii)(ধ্রুবক unsigned অক্ষর *, unsigned অক্ষর *),
  CURLcode (*hash)(unsigned অক্ষর *, ধ্রুবক unsigned অক্ষর *, ধ্রুবক size_t))
{
  CURLcode ফলাফল;
  ধ্রুবক অক্ষর *userp = Curl_creds_user(creds);
  ধ্রুবক অক্ষর *passwdp = Curl_creds_passwd(creds);
  unsigned অক্ষর hashbuf[32]; /* 32 bytes/256 bits */
  unsigned অক্ষর request_digest[65];
  unsigned অক্ষর ha1[65];    /* 64 digits and 1 zero byte */
  unsigned অক্ষর ha2[65];    /* 64 digits and 1 zero byte */
  অক্ষর userh[65];
  অক্ষর *cnonce = ফাঁকা;
  size_t cnonce_sz = 0;
  অক্ষর *userp_quoted = ফাঁকা;
  অক্ষর *realm_quoted = ফাঁকা;
  অক্ষর *nonce_quoted = ফাঁকা;
  অক্ষর *hashthis = ফাঁকা;
  অক্ষর *uri_quoted = ফাঁকা;
  কাঠামো dynbuf response;
  *outptr = ফাঁকা;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, আকার(hashbuf));
  যদি(!digest->nc)
    digest->nc = 1;

  যদি(!digest->cnonce) {
    অক্ষর cnoncebuf[12];
    ফলাফল = Curl_rand_bytes(তথ্য,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (unsigned অক্ষর *)cnoncebuf,
                             আকার(cnoncebuf));
    যদি(!ফলাফল)
      ফলাফল = curlx_base64_encode((uint8_t *)cnoncebuf, আকার(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    যদি(ফলাফল)
      যাও oom;

    digest->cnonce = cnonce;
  }

  যদি(digest->userhash) {
    অক্ষর *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    যদি(!hasht) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও oom;
    }

    ফলাফল = hash(hashbuf, (unsigned অক্ষর *)hasht, strlen(hasht));
    curlx_free(hasht);
    যদি(ফলাফল)
      যাও oom;
    convert_to_ascii(hashbuf, (unsigned অক্ষর *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  যদি(!hashthis) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও oom;
  }

  ফলাফল = hash(hashbuf, (unsigned অক্ষর *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  যদি(ফলাফল)
    যাও oom;
  convert_to_ascii(hashbuf, ha1);

  যদি(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    অক্ষর *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    যদি(!tmp) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও oom;
    }

    ফলাফল = hash(hashbuf, (unsigned অক্ষর *)tmp, strlen(tmp));
    curlx_free(tmp);
    যদি(ফলাফল)
      যাও oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ধ্রুবক অক্ষর *)uripath);
  যদি(!uri_quoted) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  যদি(!hashthis) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও oom;
  }

  যদি(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    অক্ষর hashed[65];
    অক্ষর *hashthis2;

    ফলাফল = hash(hashbuf, (ধ্রুবক unsigned অক্ষর *)"", 0);
    যদি(ফলাফল) {
      curlx_free(hashthis);
      যাও oom;
    }
    convert_to_ascii(hashbuf, (unsigned অক্ষর *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    যদি(!hashthis) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও oom;
    }
  }

  ফলাফল = hash(hashbuf, (unsigned অক্ষর *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  যদি(ফলাফল)
    যাও oom;
  convert_to_ascii(hashbuf, ha2);

  যদি(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (unsigned পূর্ণসংখ্যা)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  অন্যথা
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  যদি(!hashthis) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও oom;
  }

  ফলাফল = hash(hashbuf, (unsigned অক্ষর *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  যদি(ফলাফল)
    যাও oom;
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
  যদি(!userp_quoted) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও oom;
  }
  যদি(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  অন্যথা {
    realm_quoted = curlx_malloc(1);
    যদি(realm_quoted)
      realm_quoted[0] = 0;
  }
  যদি(!realm_quoted) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  যদি(!nonce_quoted) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও oom;
  }

  যদি(digest->qop) {
    ফলাফল = curlx_dyn_addf(&response, "username=\"%s\", "
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
                            (unsigned পূর্ণসংখ্যা)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  অন্যথা {
    ফলাফল = curlx_dyn_addf(&response, "username=\"%s\", "
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
  যদি(ফলাফল)
    যাও oom;

  /* Add the optional fields */
  যদি(digest->opaque) {
    /* Append the opaque */
    অক্ষর *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    যদি(!opaque_quoted) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও oom;
    }
    ফলাফল = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    যদি(ফলাফল)
      যাও oom;
  }

  যদি(digest->algorithm) {
    /* Append the algorithm */
    ফলাফল = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    যদি(ফলাফল)
      যাও oom;
  }

  যদি(digest->userhash) {
    /* Append the userhash */
    ফলাফল = curlx_dyn_add(&response, ", userhash=true");
    যদি(ফলাফল)
      যাও oom;
  }

  /* Return the output */
  *outptr = curlx_dyn_ptr(&response);
  *outlen = curlx_dyn_len(&response);
  ফলাফল = CURLE_OK;

oom:
  curlx_free(nonce_quoted);
  curlx_free(realm_quoted);
  curlx_free(uri_quoted);
  curlx_free(userp_quoted);
  যদি(ফলাফল)
    curlx_dyn_free(&response);
  ফেরত ফলাফল;
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
CURLcode Curl_auth_create_digest_http_message(কাঠামো Curl_easy *তথ্য,
                                              কাঠামো Curl_creds *creds,
                                              ধ্রুবক unsigned অক্ষর *request,
                                              ধ্রুবক unsigned অক্ষর *uripath,
                                              কাঠামো digestdata *digest,
                                              অক্ষর **outptr, size_t *outlen)
{
  যদি(digest->algo <= ALGO_MD5SESS)
    ফেরত auth_create_digest_http_message(তথ্য, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  যদি(digest->algo <= ALGO_SHA256SESS)
    ফেরত auth_create_digest_http_message(তথ্য, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  যদি(digest->algo <= ALGO_SHA512_256SESS)
    ফেরত auth_create_digest_http_message(তথ্য, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  ফেরত CURLE_BAD_CONTENT_ENCODING;
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
শূন্য Curl_auth_digest_cleanup(কাঠামো digestdata *digest)
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
