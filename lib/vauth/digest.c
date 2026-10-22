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
#অন্তর্ভাৱ "curl_setup.h"

#ifndef CURL_DISABLE_DIGEST_AUTH

#অন্তর্ভাৱ "vauth/vauth.h"
#অন্তর্ভাৱ "vauth/digest.h"
#অন্তর্ভাৱ "curlx/base64.h"
#অন্তর্ভাৱ "curl_md5.h"
#অন্তর্ভাৱ "curl_sha256.h"
#অন্তর্ভাৱ "curl_sha512_256.h"
#অন্তর্ভাৱ "curlx/strparse.h"
#অন্তর্ভাৱ "rand.h"
#অন্তর্ভাৱ "escape.h"

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

bool Curl_auth_digest_get_pair(ধ্রুৱ ৱর্ণ *str, সংরচনা Curl_str *মান,
                               ৱর্ণ *content, ধ্রুৱ ৱর্ণ **endptr)
{
  পূর্ণাঙ্ক c;
  size_t content_length = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  যদি(curlx_str_until(&str, মান, DIGEST_MAX_মান_LENGTH, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    ঘূৰাই FALSE;

  যদি('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  বাবে(c = DIGEST_MAX_CONTENT_LENGTH; *str && c--; str++) {
    যদি(!escape) {
      ৱিকল্প(*str) {
      পক্ষ '\\':
        যদি(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          অনুৱর্ত;
        }
        ভঙ্গ;

      পক্ষ ',':
        যদি(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          অনুৱর্ত;
        }
        ভঙ্গ;

      পক্ষ '\r':
      পক্ষ '\n':
        /* end of string */
        যদি(starts_with_quote)
          ঘূৰাই FALSE; /* No closing quote */
        c = 0;
        অনুৱর্ত;

      পক্ষ '\"':
        যদি(starts_with_quote) {
          /* end of string */
          c = 0;
          অনুৱর্ত;
        }
        অন্যথা
          ঘূৰাই FALSE;
      }
    }

    যদি(content_length >= DIGEST_MAX_CONTENT_LENGTH - 1) {
      /* no room for this byte plus the terminator */
      ঘূৰাই FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_length++;
  }

  যদি(escape)
    ঘূৰাই FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  ঘূৰাই TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
স্থির শূন্য auth_digest_md5_to_ascii(
  ধ্রুৱ unsigned ৱর্ণ *source, /* 16 bytes */
  unsigned ৱর্ণ *dest)         /* 33 bytes */
{
  পূর্ণাঙ্ক i;
  বাবে(i = 0; i < 16; i++)
    curl_msnprintf((ৱর্ণ *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
স্থির শূন্য auth_digest_sha256_to_ascii(
  ধ্রুৱ unsigned ৱর্ণ *source, /* 32 bytes */
  unsigned ৱর্ণ *dest)         /* 65 bytes */
{
  পূর্ণাঙ্ক i;
  বাবে(i = 0; i < 32; i++)
    curl_msnprintf((ৱর্ণ *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
স্থির ৱর্ণ *auth_digest_string_quoted(ধ্রুৱ ৱর্ণ *s)
{
  সংরচনা dynbuf out;
  curlx_dyn_init(&out, 2048);
  যদি(!*s) /* for zero length input, make sure we return an empty string */
    ঘূৰাই curlx_strdup("");
  যেতিয়ালৈ(*s) {
    CURLcode result;
    যদি(*s == '"' || *s == '\\') {
      result = curlx_dyn_addn(&out, "\\", 1);
      যদি(!result)
        result = curlx_dyn_addn(&out, s, 1);
    }
    অন্যথা যদি((*s < ' ') || (*s > 0x7e)) {
      unsigned ৱর্ণ buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (unsigned ৱর্ণ)*s);
      result = curlx_dyn_addn(&out, buf, 3);
    }
    অন্যথা
      result = curlx_dyn_addn(&out, s, 1);
    যদি(result)
      ঘূৰাই রিক্ত;
    s++;
  }
  ঘূৰাই curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
স্থির bool auth_digest_get_চাবি_মান(ধ্রুৱ ৱর্ণ *chlg, ধ্রুৱ ৱর্ণ *চাবি,
                                      ৱর্ণ *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  কৰা {
    সংরচনা Curl_str data;
    সংরচনা Curl_str name;

    curlx_str_passblanks(&chlg);

    যদি(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      পূর্ণাঙ্ক rc = curlx_str_quotedword(&chlg, &data, 256);
      যদি(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &data, 256, ',');
      যদি(rc)
        ঘূৰাই FALSE; /* weird */

      যদি(curlx_str_cmp(&name, চাবি)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&data);
        ধ্রুৱ ৱর্ণ *src = curlx_str(&data);
        size_t i;
        size_t outlen = 0;

        যদি(len >= buflen)
          /* does not fit */
          ঘূৰাই FALSE;

        বাবে(i = 0; i < len; i++) {
          যদি(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        ঘূৰাই TRUE;
      }
      যদি(curlx_str_single(&chlg, ','))
        ঘূৰাই FALSE;
    }
    অন্যথা /* odd syntax */
      ভঙ্গ;
  } যেতিয়ালৈ(1);

  ঘূৰাই FALSE;
}

স্থির শূন্য auth_digest_get_qop_মানসমূহ(ধ্রুৱ ৱর্ণ *options, পূর্ণাঙ্ক *মান)
{
  সংরচনা Curl_str out;
  /* Initialize the output */
  *মান = 0;

  যেতিয়ালৈ(!curlx_str_until(&options, &out, 32, ',')) {
    যদি(curlx_str_casecompare(&out, DIGEST_QOP_মান_STRING_AUTH))
      *মান |= DIGEST_QOP_মান_AUTH;
    অন্যথা যদি(curlx_str_casecompare(&out, DIGEST_QOP_মান_STRING_AUTH_INT))
      *মান |= DIGEST_QOP_মান_AUTH_INT;
    অন্যথা যদি(curlx_str_casecompare(&out, DIGEST_QOP_মান_STRING_AUTH_CONF))
      *মান |= DIGEST_QOP_মান_AUTH_CONF;
    যদি(curlx_str_single(&options, ','))
      ভঙ্গ;
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
স্থির CURLcode auth_decode_digest_md5_message(ধ্রুৱ সংরচনা bufref *chlgref,
                                               ৱর্ণ *nonce, size_t nlen,
                                               ৱর্ণ *realm, size_t rlen,
                                               ৱর্ণ *alg, size_t alen,
                                               ৱর্ণ *qop, size_t qlen)
{
  ধ্রুৱ ৱর্ণ *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  যদি(!Curl_bufref_len(chlgref))
    ঘূৰাই CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  যদি(!auth_digest_get_চাবি_মান(chlg, "nonce", nonce, nlen))
    ঘূৰাই CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  যদি(!auth_digest_get_চাবি_মান(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  যদি(!auth_digest_get_চাবি_মান(chlg, "algorithm", alg, alen))
    ঘূৰাই CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  যদি(!auth_digest_get_চাবি_মান(chlg, "qop", qop, qlen))
    ঘূৰাই CURLE_BAD_CONTENT_ENCODING;

  ঘূৰাই CURLE_OK;
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
  ঘূৰাই TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(সংরচনা Curl_easy *data,
                                             ধ্রুৱ সংরচনা bufref *chlg,
                                             সংরচনা Curl_creds *creds,
                                             ধ্রুৱ ৱর্ণ *default_service,
                                             সংরচনা bufref *out)
{
  ধ্রুৱ ৱর্ণ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  সংরচনা MD5_context *ctxt;
  ধ্রুৱ ৱর্ণ *userp = Curl_creds_user(creds);
  ধ্রুৱ ৱর্ণ *passwdp = Curl_creds_passwd(creds);
  ৱর্ণ *response = রিক্ত;
  unsigned ৱর্ণ digest[MD5_DIGEST_LEN];
  ৱর্ণ HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  ৱর্ণ HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  ৱর্ণ resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  ৱর্ণ nonce[64];
  ৱর্ণ realm[128];
  ৱর্ণ algorithm[64];
  ৱর্ণ qop_options[64];
  পূর্ণাঙ্ক qop_মানসমূহ;
  ৱর্ণ cnonce[33];
  ৱর্ণ nonce_গুনতি[] = "00000001";
  ৱর্ণ method[]     = "AUTHENTICATE";
  ৱর্ণ qop[]        = DIGEST_QOP_মান_STRING_AUTH;
  ৱর্ণ *spn         = রিক্ত;
  ৱর্ণ *qrealm;
  ৱর্ণ *qnonce;
  ৱর্ণ *quserp;
  ৱর্ণ *qspn;

  /* Decode the challenge message */
  CURLcode result = auth_decode_digest_md5_message(chlg,
                                                   nonce, পরিমাণ(nonce),
                                                   realm, পরিমাণ(realm),
                                                   algorithm,
                                                   পরিমাণ(algorithm),
                                                   qop_options,
                                                   পরিমাণ(qop_options));
  যদি(result)
    ঘূৰাই result;

  /* We only support md5 sessions */
  যদি(strcmp(algorithm, "md5-sess"))
    ঘূৰাই CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_মানসমূহ(qop_options, &qop_মানসমূহ);

  /* We only support auth quality-of-protection */
  যদি(!(qop_মানসমূহ & DIGEST_QOP_মান_AUTH))
    ঘূৰাই CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  result = Curl_rand_hex(data, (unsigned ৱর্ণ *)cnonce, পরিমাণ(cnonce));
  যদি(result)
    ঘূৰাই result;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  যদি(!ctxt)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)userp,
                  curlx_uztoui(strlen(userp)));
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)realm,
                  curlx_uztoui(strlen(realm)));
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)passwdp,
                  curlx_uztoui(strlen(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  যদি(!ctxt)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  বাবে(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, রিক্ত);
  যদি(!spn)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  যদি(!ctxt) {
    curlx_free(spn);

    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)method,
                  curlx_uztoui(strlen(method)));
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)spn,
                  curlx_uztoui(strlen(spn)));
  Curl_MD5_final(ctxt, digest);

  বাবে(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  যদি(!ctxt) {
    curlx_free(spn);

    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)":", 1);

  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)nonce_গুনতি,
                  curlx_uztoui(strlen(nonce_গুনতি)));
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)":", 1);
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)qop,
                  curlx_uztoui(strlen(qop)));
  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)":", 1);

  Curl_MD5_update(ctxt, (ধ্রুৱ unsigned ৱর্ণ *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  বাবে(i = 0; i < MD5_DIGEST_LEN; i++)
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
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, strlen(response), curl_free);
  ঘূৰাই result;
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
CURLcode Curl_auth_decode_digest_http_message(ধ্রুৱ ৱর্ণ *chlg,
                                              সংরচনা digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  যদি(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  বাবে(;;) {
    সংরচনা Curl_str মান;
    ৱর্ণ content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    যদি(Curl_auth_digest_get_pair(chlg, &মান, content, &chlg)) {
      যদি(curlx_str_casecompare(&মান, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        যদি(!digest->nonce)
          ঘূৰাই CURLE_OUT_OF_MEMORY;
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
          ঘূৰাই CURLE_OUT_OF_MEMORY;
      }
      অন্যথা যদি(curlx_str_casecompare(&মান, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        যদি(!digest->opaque)
          ঘূৰাই CURLE_OUT_OF_MEMORY;
      }
      অন্যথা যদি(curlx_str_casecompare(&মান, "qop")) {
        ধ্রুৱ ৱর্ণ *token = content;
        সংরচনা Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        যেতিয়ালৈ(!curlx_str_until(&token, &out, 32, ',')) {
          যদি(curlx_str_casecompare(&out, DIGEST_QOP_মান_STRING_AUTH))
            foundAuth = TRUE;
          অন্যথা যদি(curlx_str_casecompare(&out,
                                        DIGEST_QOP_মান_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          যদি(curlx_str_single(&token, ','))
            ভঙ্গ;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        যদি(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_মান_STRING_AUTH);
          যদি(!digest->qop)
            ঘূৰাই CURLE_OUT_OF_MEMORY;
        }
        অন্যথা যদি(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_মান_STRING_AUTH_INT);
          যদি(!digest->qop)
            ঘূৰাই CURLE_OUT_OF_MEMORY;
        }
      }
      অন্যথা যদি(curlx_str_casecompare(&মান, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        যদি(!digest->algorithm)
          ঘূৰাই CURLE_OUT_OF_MEMORY;

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
          ঘূৰাই CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        অন্যথা যদি(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#অন্যথা /* !CURL_HAVE_SHA512_256 */
          ঘূৰাই CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        অন্যথা
          ঘূৰাই CURLE_BAD_CONTENT_ENCODING;
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
      ভঙ্গ; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (শূন্য)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  যদি(before && !digest->stale)
    ঘূৰাই CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  যদি(!digest->nonce)
    ঘূৰাই CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  যদি(!digest->qop && (digest->algo & SESSION_ALGO))
    ঘূৰাই CURLE_BAD_CONTENT_ENCODING;

  ঘূৰাই CURLE_OK;
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
  সংরচনা Curl_easy *data,
  সংরচনা Curl_creds *creds,
  ধ্রুৱ unsigned ৱর্ণ *request,
  ধ্রুৱ unsigned ৱর্ণ *uripath,
  সংরচনা digestdata *digest,
  ৱর্ণ **outptr, size_t *outlen,
  শূন্য (*convert_to_ascii)(ধ্রুৱ unsigned ৱর্ণ *, unsigned ৱর্ণ *),
  CURLcode (*hash)(unsigned ৱর্ণ *, ধ্রুৱ unsigned ৱর্ণ *, ধ্রুৱ size_t))
{
  CURLcode result;
  ধ্রুৱ ৱর্ণ *userp = Curl_creds_user(creds);
  ধ্রুৱ ৱর্ণ *passwdp = Curl_creds_passwd(creds);
  unsigned ৱর্ণ hashbuf[32]; /* 32 bytes/256 bits */
  unsigned ৱর্ণ request_digest[65];
  unsigned ৱর্ণ ha1[65];    /* 64 digits and 1 zero byte */
  unsigned ৱর্ণ ha2[65];    /* 64 digits and 1 zero byte */
  ৱর্ণ userh[65];
  ৱর্ণ *cnonce = রিক্ত;
  size_t cnonce_sz = 0;
  ৱর্ণ *userp_quoted = রিক্ত;
  ৱর্ণ *realm_quoted = রিক্ত;
  ৱর্ণ *nonce_quoted = রিক্ত;
  ৱর্ণ *hashthis = রিক্ত;
  ৱর্ণ *uri_quoted = রিক্ত;
  সংরচনা dynbuf response;
  *outptr = রিক্ত;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, পরিমাণ(hashbuf));
  যদি(!digest->nc)
    digest->nc = 1;

  যদি(!digest->cnonce) {
    ৱর্ণ cnoncebuf[12];
    result = Curl_rand_bytes(data,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (unsigned ৱর্ণ *)cnoncebuf,
                             পরিমাণ(cnoncebuf));
    যদি(!result)
      result = curlx_base64_encode((uint8_t *)cnoncebuf, পরিমাণ(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    যদি(result)
      গচ্ছ oom;

    digest->cnonce = cnonce;
  }

  যদি(digest->userhash) {
    ৱর্ণ *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    যদি(!hasht) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ oom;
    }

    result = hash(hashbuf, (unsigned ৱর্ণ *)hasht, strlen(hasht));
    curlx_free(hasht);
    যদি(result)
      গচ্ছ oom;
    convert_to_ascii(hashbuf, (unsigned ৱর্ণ *)userh);
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
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ oom;
  }

  result = hash(hashbuf, (unsigned ৱর্ণ *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  যদি(result)
    গচ্ছ oom;
  convert_to_ascii(hashbuf, ha1);

  যদি(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    ৱর্ণ *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    যদি(!tmp) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ oom;
    }

    result = hash(hashbuf, (unsigned ৱর্ণ *)tmp, strlen(tmp));
    curlx_free(tmp);
    যদি(result)
      গচ্ছ oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ধ্রুৱ ৱর্ণ *)uripath);
  যদি(!uri_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  যদি(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ oom;
  }

  যদি(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    ৱর্ণ hashed[65];
    ৱর্ণ *hashthis2;

    result = hash(hashbuf, (ধ্রুৱ unsigned ৱর্ণ *)"", 0);
    যদি(result) {
      curlx_free(hashthis);
      গচ্ছ oom;
    }
    convert_to_ascii(hashbuf, (unsigned ৱর্ণ *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    যদি(!hashthis) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ oom;
    }
  }

  result = hash(hashbuf, (unsigned ৱর্ণ *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  যদি(result)
    গচ্ছ oom;
  convert_to_ascii(hashbuf, ha2);

  যদি(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (unsigned পূর্ণাঙ্ক)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  অন্যথা
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  যদি(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ oom;
  }

  result = hash(hashbuf, (unsigned ৱর্ণ *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  যদি(result)
    গচ্ছ oom;
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
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ oom;
  }
  যদি(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  অন্যথা {
    realm_quoted = curlx_malloc(1);
    যদি(realm_quoted)
      realm_quoted[0] = 0;
  }
  যদি(!realm_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  যদি(!nonce_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ oom;
  }

  যদি(digest->qop) {
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
                            (unsigned পূর্ণাঙ্ক)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  অন্যথা {
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
  যদি(result)
    গচ্ছ oom;

  /* Add the optional fields */
  যদি(digest->opaque) {
    /* Append the opaque */
    ৱর্ণ *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    যদি(!opaque_quoted) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ oom;
    }
    result = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    যদি(result)
      গচ্ছ oom;
  }

  যদি(digest->algorithm) {
    /* Append the algorithm */
    result = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    যদি(result)
      গচ্ছ oom;
  }

  যদি(digest->userhash) {
    /* Append the userhash */
    result = curlx_dyn_add(&response, ", userhash=true");
    যদি(result)
      গচ্ছ oom;
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
  যদি(result)
    curlx_dyn_free(&response);
  ঘূৰাই result;
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
CURLcode Curl_auth_create_digest_http_message(সংরচনা Curl_easy *data,
                                              সংরচনা Curl_creds *creds,
                                              ধ্রুৱ unsigned ৱর্ণ *request,
                                              ধ্রুৱ unsigned ৱর্ণ *uripath,
                                              সংরচনা digestdata *digest,
                                              ৱর্ণ **outptr, size_t *outlen)
{
  যদি(digest->algo <= ALGO_MD5SESS)
    ঘূৰাই auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  যদি(digest->algo <= ALGO_SHA256SESS)
    ঘূৰাই auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  যদি(digest->algo <= ALGO_SHA512_256SESS)
    ঘূৰাই auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  ঘূৰাই CURLE_BAD_CONTENT_ENCODING;
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
শূন্য Curl_auth_digest_cleanup(সংরচনা digestdata *digest)
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
