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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH

#شامل "vauth/vauth.h"
#شامل "vauth/digest.h"
#شامل "curlx/base64.h"
#شامل "curl_md5.h"
#شامل "curl_sha256.h"
#شامل "curl_sha512_256.h"
#شامل "curlx/strparse.h"
#شامل "rand.h"
#شامل "escape.h"

#اگر_غیر_تعریف_شدہ USE_WINDOWS_SSPI
#تعریف_کرو SESSION_ALGO 1 /* for algos with this bit set */

#تعریف_کرو ALGO_MD5 0
#تعریف_کرو ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#تعریف_کرو ALGO_SHA256 2
#تعریف_کرو ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#تعریف_کرو ALGO_SHA512_256 4
#تعریف_کرو ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#تعریف_کرو DIGEST_QOP_قدر_AUTH             (1 << 0)
#تعریف_کرو DIGEST_QOP_قدر_AUTH_INT         (1 << 1)
#تعریف_کرو DIGEST_QOP_قدر_AUTH_CONF        (1 << 2)

#تعریف_کرو DIGEST_QOP_قدر_STRING_AUTH      "auth"
#تعریف_کرو DIGEST_QOP_قدر_STRING_AUTH_INT  "auth-int"
#تعریف_کرو DIGEST_QOP_قدر_STRING_AUTH_CONF "auth-conf"
#اگر_ختم

bool Curl_auth_digest_get_pair(ثابت حرف *str, ساخت Curl_str *قدر,
                               حرف *content, ثابت حرف **endptr)
{
  صحیح c;
  size_t content_لمبائی = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  اگر(curlx_str_until(&str, قدر, DIGEST_MAX_قدر_لمبائی, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    واپس FALSE;

  اگر('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  برائے(c = DIGEST_MAX_CONTENT_لمبائی; *str && c--; str++) {
    اگر(!escape) {
      انتخاب(*str) {
      حالت '\\':
        اگر(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          جاری;
        }
        توڑو;

      حالت ',':
        اگر(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          جاری;
        }
        توڑو;

      حالت '\r':
      حالت '\n':
        /* end of string */
        اگر(starts_with_quote)
          واپس FALSE; /* No closing quote */
        c = 0;
        جاری;

      حالت '\"':
        اگر(starts_with_quote) {
          /* end of string */
          c = 0;
          جاری;
        }
        ورنہ
          واپس FALSE;
      }
    }

    اگر(content_لمبائی >= DIGEST_MAX_CONTENT_لمبائی - 1) {
      /* no room for this byte plus the terminator */
      واپس FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_لمبائی++;
  }

  اگر(escape)
    واپس FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  واپس TRUE;
}

#اگر_غیر_تعریف_شدہ USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
ساکن خالی auth_digest_md5_مقصد_ascii(
  ثابت غیرعلامتی حرف *ماخذ, /* 16 bytes */
  غیرعلامتی حرف *dest)         /* 33 bytes */
{
  صحیح i;
  برائے(i = 0; i < 16; i++)
    curl_msnprintf((حرف *)&dest[i * 2], 3, "%02x", ماخذ[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
ساکن خالی auth_digest_sha256_مقصد_ascii(
  ثابت غیرعلامتی حرف *ماخذ, /* 32 bytes */
  غیرعلامتی حرف *dest)         /* 65 bytes */
{
  صحیح i;
  برائے(i = 0; i < 32; i++)
    curl_msnprintf((حرف *)&dest[i * 2], 3, "%02x", ماخذ[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
ساکن حرف *auth_digest_string_quoted(ثابت حرف *s)
{
  ساخت dynbuf برآمد;
  curlx_dyn_init(&برآمد, 2048);
  اگر(!*s) /* for zero length input, make sure we return an empty string */
    واپس curlx_strdup("");
  جب(*s) {
    CURLcode نتیجہ;
    اگر(*s == '"' || *s == '\\') {
      نتیجہ = curlx_dyn_addn(&برآمد, "\\", 1);
      اگر(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&برآمد, s, 1);
    }
    ورنہ اگر((*s < ' ') || (*s > 0x7e)) {
      غیرعلامتی حرف buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (غیرعلامتی حرف)*s);
      نتیجہ = curlx_dyn_addn(&برآمد, buf, 3);
    }
    ورنہ
      نتیجہ = curlx_dyn_addn(&برآمد, s, 1);
    اگر(نتیجہ)
      واپس تہی;
    s++;
  }
  واپس curlx_dyn_ptr(&برآمد);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
ساکن bool auth_digest_get_کلید_قدر(ثابت حرف *chlg, ثابت حرف *کلید,
                                      حرف *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  کرو {
    ساخت Curl_str ڈیٹا;
    ساخت Curl_str name;

    curlx_str_passblanks(&chlg);

    اگر(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      صحیح rc = curlx_str_quotedword(&chlg, &ڈیٹا, 256);
      اگر(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &ڈیٹا, 256, ',');
      اگر(rc)
        واپس FALSE; /* weird */

      اگر(curlx_str_cmp(&name, کلید)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&ڈیٹا);
        ثابت حرف *src = curlx_str(&ڈیٹا);
        size_t i;
        size_t outlen = 0;

        اگر(len >= buflen)
          /* does not fit */
          واپس FALSE;

        برائے(i = 0; i < len; i++) {
          اگر(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        واپس TRUE;
      }
      اگر(curlx_str_single(&chlg, ','))
        واپس FALSE;
    }
    ورنہ /* odd syntax */
      توڑو;
  } جب(1);

  واپس FALSE;
}

ساکن خالی auth_digest_get_qop_اقدار(ثابت حرف *options, صحیح *قدر)
{
  ساخت Curl_str برآمد;
  /* Initialize the output */
  *قدر = 0;

  جب(!curlx_str_until(&options, &برآمد, 32, ',')) {
    اگر(curlx_str_casecompare(&برآمد, DIGEST_QOP_قدر_STRING_AUTH))
      *قدر |= DIGEST_QOP_قدر_AUTH;
    ورنہ اگر(curlx_str_casecompare(&برآمد, DIGEST_QOP_قدر_STRING_AUTH_INT))
      *قدر |= DIGEST_QOP_قدر_AUTH_INT;
    ورنہ اگر(curlx_str_casecompare(&برآمد, DIGEST_QOP_قدر_STRING_AUTH_CONF))
      *قدر |= DIGEST_QOP_قدر_AUTH_CONF;
    اگر(curlx_str_single(&options, ','))
      توڑو;
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
ساکن CURLcode auth_decode_digest_md5_پیغام(ثابت ساخت bufref *chlgref,
                                               حرف *nonce, size_t nlen,
                                               حرف *realm, size_t rlen,
                                               حرف *alg, size_t alen,
                                               حرف *qop, size_t qlen)
{
  ثابت حرف *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  اگر(!Curl_bufref_len(chlgref))
    واپس CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  اگر(!auth_digest_get_کلید_قدر(chlg, "nonce", nonce, nlen))
    واپس CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  اگر(!auth_digest_get_کلید_قدر(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  اگر(!auth_digest_get_کلید_قدر(chlg, "algorithm", alg, alen))
    واپس CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  اگر(!auth_digest_get_کلید_قدر(chlg, "qop", qop, qlen))
    واپس CURLE_BAD_CONTENT_ENCODING;

  واپس CURLE_OK;
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
bool Curl_auth_is_digest_supported(خالی)
{
  واپس TRUE;
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
CURLcode Curl_auth_create_digest_md5_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ثابت ساخت bufref *chlg,
                                             ساخت Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             ساخت bufref *برآمد)
{
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  ساخت MD5_context *ctxt;
  ثابت حرف *userp = Curl_creds_user(creds);
  ثابت حرف *passwdp = Curl_creds_passwd(creds);
  حرف *response = تہی;
  غیرعلامتی حرف digest[MD5_DIGEST_LEN];
  حرف HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  حرف HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  حرف resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  حرف nonce[64];
  حرف realm[128];
  حرف algorithm[64];
  حرف qop_options[64];
  صحیح qop_اقدار;
  حرف cnonce[33];
  حرف nonce_گنتی[] = "00000001";
  حرف طریقہ[]     = "AUTHENTICATE";
  حرف qop[]        = DIGEST_QOP_قدر_STRING_AUTH;
  حرف *spn         = تہی;
  حرف *qrealm;
  حرف *qnonce;
  حرف *quserp;
  حرف *qspn;

  /* Decode the challenge message */
  CURLcode نتیجہ = auth_decode_digest_md5_پیغام(chlg,
                                                   nonce, حجم(nonce),
                                                   realm, حجم(realm),
                                                   algorithm,
                                                   حجم(algorithm),
                                                   qop_options,
                                                   حجم(qop_options));
  اگر(نتیجہ)
    واپس نتیجہ;

  /* We only support md5 sessions */
  اگر(strcmp(algorithm, "md5-sess"))
    واپس CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_اقدار(qop_options, &qop_اقدار);

  /* We only support auth quality-of-protection */
  اگر(!(qop_اقدار & DIGEST_QOP_قدر_AUTH))
    واپس CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  نتیجہ = Curl_rand_hex(ڈیٹا, (غیرعلامتی حرف *)cnonce, حجم(cnonce));
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  اگر(!ctxt)
    واپس CURLE_برآمد_کا_حافظہ;

  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)userp,
                  curlx_uztoui(لڑی_لمبائی(userp)));
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)realm,
                  curlx_uztoui(لڑی_لمبائی(realm)));
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)passwdp,
                  curlx_uztoui(لڑی_لمبائی(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  اگر(!ctxt)
    واپس CURLE_برآمد_کا_حافظہ;

  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)nonce,
                  curlx_uztoui(لڑی_لمبائی(nonce)));
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)cnonce,
                  curlx_uztoui(لڑی_لمبائی(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  برائے(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, ڈیٹا->state.origin->hostname, تہی);
  اگر(!spn)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  اگر(!ctxt) {
    curlx_free(spn);

    واپس CURLE_برآمد_کا_حافظہ;
  }

  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)طریقہ,
                  curlx_uztoui(لڑی_لمبائی(طریقہ)));
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)spn,
                  curlx_uztoui(لڑی_لمبائی(spn)));
  Curl_MD5_final(ctxt, digest);

  برائے(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  اگر(!ctxt) {
    curlx_free(spn);

    واپس CURLE_برآمد_کا_حافظہ;
  }

  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)nonce,
                  curlx_uztoui(لڑی_لمبائی(nonce)));
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)":", 1);

  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)nonce_گنتی,
                  curlx_uztoui(لڑی_لمبائی(nonce_گنتی)));
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)cnonce,
                  curlx_uztoui(لڑی_لمبائی(cnonce)));
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)qop,
                  curlx_uztoui(لڑی_لمبائی(qop)));
  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)":", 1);

  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  برائے(i = 0; i < MD5_DIGEST_LEN; i++)
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
                             cnonce, nonce_گنتی, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  اگر(!response)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Return the response. */
  Curl_bufref_سیٹ(برآمد, response, لڑی_لمبائی(response), curl_free);
  واپس نتیجہ;
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
CURLcode Curl_auth_decode_digest_http_پیغام(ثابت حرف *chlg,
                                              ساخت digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  اگر(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  برائے(;;) {
    ساخت Curl_str قدر;
    حرف content[DIGEST_MAX_CONTENT_لمبائی];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    اگر(Curl_auth_digest_get_pair(chlg, &قدر, content, &chlg)) {
      اگر(curlx_str_casecompare(&قدر, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        اگر(!digest->nonce)
          واپس CURLE_برآمد_کا_حافظہ;
      }
      ورنہ اگر(curlx_str_casecompare(&قدر, "stale")) {
        اگر(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      ورنہ اگر(curlx_str_casecompare(&قدر, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        اگر(!digest->realm)
          واپس CURLE_برآمد_کا_حافظہ;
      }
      ورنہ اگر(curlx_str_casecompare(&قدر, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        اگر(!digest->opaque)
          واپس CURLE_برآمد_کا_حافظہ;
      }
      ورنہ اگر(curlx_str_casecompare(&قدر, "qop")) {
        ثابت حرف *token = content;
        ساخت Curl_str برآمد;
        bool ملا_Auth = FALSE;
        bool ملا_Auth_Int = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        جب(!curlx_str_until(&token, &برآمد, 32, ',')) {
          اگر(curlx_str_casecompare(&برآمد, DIGEST_QOP_قدر_STRING_AUTH))
            ملا_Auth = TRUE;
          ورنہ اگر(curlx_str_casecompare(&برآمد,
                                        DIGEST_QOP_قدر_STRING_AUTH_INT))
            ملا_Auth_Int = TRUE;
          اگر(curlx_str_single(&token, ','))
            توڑو;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        اگر(ملا_Auth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_قدر_STRING_AUTH);
          اگر(!digest->qop)
            واپس CURLE_برآمد_کا_حافظہ;
        }
        ورنہ اگر(ملا_Auth_Int) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_قدر_STRING_AUTH_INT);
          اگر(!digest->qop)
            واپس CURLE_برآمد_کا_حافظہ;
        }
      }
      ورنہ اگر(curlx_str_casecompare(&قدر, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        اگر(!digest->algorithm)
          واپس CURLE_برآمد_کا_حافظہ;

        اگر(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        ورنہ اگر(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        ورنہ اگر(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        ورنہ اگر(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        ورنہ اگر(curl_strequal(content, "SHA-512-256")) {
#اگر_تعریف_شدہ CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#ورنہ /* !CURL_HAVE_SHA512_256 */
          واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم /* CURL_HAVE_SHA512_256 */
        }
        ورنہ اگر(curl_strequal(content, "SHA-512-256-SESS")) {
#اگر_تعریف_شدہ CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#ورنہ /* !CURL_HAVE_SHA512_256 */
          واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم /* CURL_HAVE_SHA512_256 */
        }
        ورنہ
          واپس CURLE_BAD_CONTENT_ENCODING;
      }
      ورنہ اگر(curlx_str_casecompare(&قدر, "userhash")) {
        اگر(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      ورنہ {
        /* Unknown specifier, ignore it! */
      }
    }
    ورنہ
      توڑو; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (خالی)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  اگر(before && !digest->stale)
    واپس CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  اگر(!digest->nonce)
    واپس CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  اگر(!digest->qop && (digest->algo & SESSION_ALGO))
    واپس CURLE_BAD_CONTENT_ENCODING;

  واپس CURLE_OK;
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
ساکن CURLcode auth_create_digest_http_پیغام(
  ساخت Curl_easy *ڈیٹا,
  ساخت Curl_creds *creds,
  ثابت غیرعلامتی حرف *request,
  ثابت غیرعلامتی حرف *uripath,
  ساخت digestdata *digest,
  حرف **outptr, size_t *outlen,
  خالی (*convert_مقصد_ascii)(ثابت غیرعلامتی حرف *, غیرعلامتی حرف *),
  CURLcode (*hash)(غیرعلامتی حرف *, ثابت غیرعلامتی حرف *, ثابت size_t))
{
  CURLcode نتیجہ;
  ثابت حرف *userp = Curl_creds_user(creds);
  ثابت حرف *passwdp = Curl_creds_passwd(creds);
  غیرعلامتی حرف hashbuf[32]; /* 32 bytes/256 bits */
  غیرعلامتی حرف request_digest[65];
  غیرعلامتی حرف ha1[65];    /* 64 digits and 1 zero byte */
  غیرعلامتی حرف ha2[65];    /* 64 digits and 1 zero byte */
  حرف userh[65];
  حرف *cnonce = تہی;
  size_t cnonce_sz = 0;
  حرف *userp_quoted = تہی;
  حرف *realm_quoted = تہی;
  حرف *nonce_quoted = تہی;
  حرف *hashthis = تہی;
  حرف *uri_quoted = تہی;
  ساخت dynbuf response;
  *outptr = تہی;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, حجم(hashbuf));
  اگر(!digest->nc)
    digest->nc = 1;

  اگر(!digest->cnonce) {
    حرف cnoncebuf[12];
    نتیجہ = Curl_rand_بائٹیں(ڈیٹا,
#اگر_تعریف_شدہ DEBUGBUILD
                             TRUE,
#اگر_ختم
                             (غیرعلامتی حرف *)cnoncebuf,
                             حجم(cnoncebuf));
    اگر(!نتیجہ)
      نتیجہ = curlx_base64_encode((uint8_t *)cnoncebuf, حجم(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    اگر(نتیجہ)
      جاؤ oom;

    digest->cnonce = cnonce;
  }

  اگر(digest->userhash) {
    حرف *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    اگر(!hasht) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ oom;
    }

    نتیجہ = hash(hashbuf, (غیرعلامتی حرف *)hasht, لڑی_لمبائی(hasht));
    curlx_free(hasht);
    اگر(نتیجہ)
      جاؤ oom;
    convert_مقصد_ascii(hashbuf, (غیرعلامتی حرف *)userh);
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
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ oom;
  }

  نتیجہ = hash(hashbuf, (غیرعلامتی حرف *)hashthis, لڑی_لمبائی(hashthis));
  curlx_free(hashthis);
  اگر(نتیجہ)
    جاؤ oom;
  convert_مقصد_ascii(hashbuf, ha1);

  اگر(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    حرف *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    اگر(!tmp) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ oom;
    }

    نتیجہ = hash(hashbuf, (غیرعلامتی حرف *)tmp, لڑی_لمبائی(tmp));
    curlx_free(tmp);
    اگر(نتیجہ)
      جاؤ oom;
    convert_مقصد_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ثابت حرف *)uripath);
  اگر(!uri_quoted) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  اگر(!hashthis) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ oom;
  }

  اگر(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    حرف hashed[65];
    حرف *hashthis2;

    نتیجہ = hash(hashbuf, (ثابت غیرعلامتی حرف *)"", 0);
    اگر(نتیجہ) {
      curlx_free(hashthis);
      جاؤ oom;
    }
    convert_مقصد_ascii(hashbuf, (غیرعلامتی حرف *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    اگر(!hashthis) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ oom;
    }
  }

  نتیجہ = hash(hashbuf, (غیرعلامتی حرف *)hashthis, لڑی_لمبائی(hashthis));
  curlx_free(hashthis);
  اگر(نتیجہ)
    جاؤ oom;
  convert_مقصد_ascii(hashbuf, ha2);

  اگر(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (غیرعلامتی صحیح)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  ورنہ
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  اگر(!hashthis) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ oom;
  }

  نتیجہ = hash(hashbuf, (غیرعلامتی حرف *)hashthis, لڑی_لمبائی(hashthis));
  curlx_free(hashthis);
  اگر(نتیجہ)
    جاؤ oom;
  convert_مقصد_ascii(hashbuf, request_digest);

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
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ oom;
  }
  اگر(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  ورنہ {
    realm_quoted = curlx_malloc(1);
    اگر(realm_quoted)
      realm_quoted[0] = 0;
  }
  اگر(!realm_quoted) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  اگر(!nonce_quoted) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ oom;
  }

  اگر(digest->qop) {
    نتیجہ = curlx_dyn_addf(&response, "username=\"%s\", "
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
                            (غیرعلامتی صحیح)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  ورنہ {
    نتیجہ = curlx_dyn_addf(&response, "username=\"%s\", "
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
  اگر(نتیجہ)
    جاؤ oom;

  /* Add the optional fields */
  اگر(digest->opaque) {
    /* Append the opaque */
    حرف *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    اگر(!opaque_quoted) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ oom;
    }
    نتیجہ = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    اگر(نتیجہ)
      جاؤ oom;
  }

  اگر(digest->algorithm) {
    /* Append the algorithm */
    نتیجہ = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    اگر(نتیجہ)
      جاؤ oom;
  }

  اگر(digest->userhash) {
    /* Append the userhash */
    نتیجہ = curlx_dyn_جوڑو(&response, ", userhash=true");
    اگر(نتیجہ)
      جاؤ oom;
  }

  /* Return the output */
  *outptr = curlx_dyn_ptr(&response);
  *outlen = curlx_dyn_len(&response);
  نتیجہ = CURLE_OK;

oom:
  curlx_free(nonce_quoted);
  curlx_free(realm_quoted);
  curlx_free(uri_quoted);
  curlx_free(userp_quoted);
  اگر(نتیجہ)
    curlx_dyn_free(&response);
  واپس نتیجہ;
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
CURLcode Curl_auth_create_digest_http_پیغام(ساخت Curl_easy *ڈیٹا,
                                              ساخت Curl_creds *creds,
                                              ثابت غیرعلامتی حرف *request,
                                              ثابت غیرعلامتی حرف *uripath,
                                              ساخت digestdata *digest,
                                              حرف **outptr, size_t *outlen)
{
  اگر(digest->algo <= ALGO_MD5SESS)
    واپس auth_create_digest_http_پیغام(ڈیٹا, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_مقصد_ascii,
                                           Curl_md5it);

  اگر(digest->algo <= ALGO_SHA256SESS)
    واپس auth_create_digest_http_پیغام(ڈیٹا, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_مقصد_ascii,
                                           Curl_sha256it);
#اگر_تعریف_شدہ CURL_HAVE_SHA512_256
  اگر(digest->algo <= ALGO_SHA512_256SESS)
    واپس auth_create_digest_http_پیغام(ڈیٹا, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_مقصد_ascii,
                                           Curl_sha512_256it);
#اگر_ختم /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  واپس CURLE_BAD_CONTENT_ENCODING;
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
خالی Curl_auth_digest_cleanup(ساخت digestdata *digest)
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
#اگر_ختم /* !USE_WINDOWS_SSPI */

#اگر_ختم /* !CURL_DISABLE_DIGEST_AUTH */
