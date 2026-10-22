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

#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH

#ضمن "vauth/vauth.h"
#ضمن "vauth/digest.h"
#ضمن "curlx/base64.h"
#ضمن "curl_md5.h"
#ضمن "curl_sha256.h"
#ضمن "curl_sha512_256.h"
#ضمن "curlx/strparse.h"
#ضمن "rand.h"
#ضمن "escape.h"

#جے_بنا_تعریف USE_WINDOWS_SSPI
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
#جے_انت

bool Curl_auth_digest_get_pair(ثابت حرف *str, بنية Curl_str *قدر,
                               حرف *content, ثابت حرف **endptr)
{
  صحيح c;
  size_t content_لمائی = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  إذا(curlx_str_until(&str, قدر, DIGEST_MAX_قدر_لمائی, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    أرجع FALSE;

  إذا('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  لكل(c = DIGEST_MAX_CONTENT_لمائی; *str && c--; str++) {
    إذا(!escape) {
      بدل(*str) {
      حالة '\\':
        إذا(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          استمر;
        }
        اكسر;

      حالة ',':
        إذا(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          استمر;
        }
        اكسر;

      حالة '\r':
      حالة '\n':
        /* end of string */
        إذا(starts_with_quote)
          أرجع FALSE; /* No closing quote */
        c = 0;
        استمر;

      حالة '\"':
        إذا(starts_with_quote) {
          /* end of string */
          c = 0;
          استمر;
        }
        وإلا
          أرجع FALSE;
      }
    }

    إذا(content_لمائی >= DIGEST_MAX_CONTENT_لمائی - 1) {
      /* no room for this byte plus the terminator */
      أرجع FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_لمائی++;
  }

  إذا(escape)
    أرجع FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  أرجع TRUE;
}

#جے_بنا_تعریف USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
ساكن فراغ auth_digest_md5_منزل_ascii(
  ثابت بنانشانی حرف *سروت, /* 16 bytes */
  بنانشانی حرف *dest)         /* 33 bytes */
{
  صحيح i;
  لكل(i = 0; i < 16; i++)
    curl_msnprintf((حرف *)&dest[i * 2], 3, "%02x", سروت[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
ساكن فراغ auth_digest_sha256_منزل_ascii(
  ثابت بنانشانی حرف *سروت, /* 32 bytes */
  بنانشانی حرف *dest)         /* 65 bytes */
{
  صحيح i;
  لكل(i = 0; i < 32; i++)
    curl_msnprintf((حرف *)&dest[i * 2], 3, "%02x", سروت[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
ساكن حرف *auth_digest_string_quoted(ثابت حرف *s)
{
  بنية dynbuf باہرلا;
  curlx_dyn_init(&باہرلا, 2048);
  إذا(!*s) /* for zero length input, make sure we return an empty string */
    أرجع curlx_strdup("");
  طالما(*s) {
    CURLcode نتیجہ;
    إذا(*s == '"' || *s == '\\') {
      نتیجہ = curlx_dyn_addn(&باہرلا, "\\", 1);
      إذا(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&باہرلا, s, 1);
    }
    وإلا إذا((*s < ' ') || (*s > 0x7e)) {
      بنانشانی حرف buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (بنانشانی حرف)*s);
      نتیجہ = curlx_dyn_addn(&باہرلا, buf, 3);
    }
    وإلا
      نتیجہ = curlx_dyn_addn(&باہرلا, s, 1);
    إذا(نتیجہ)
      أرجع عدم;
    s++;
  }
  أرجع curlx_dyn_ptr(&باہرلا);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
ساكن bool auth_digest_get_کلید_قدر(ثابت حرف *chlg, ثابت حرف *کلید,
                                      حرف *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  افعل {
    بنية Curl_str انکڑے;
    بنية Curl_str name;

    curlx_str_passblanks(&chlg);

    إذا(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      صحيح rc = curlx_str_quotedword(&chlg, &انکڑے, 256);
      إذا(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &انکڑے, 256, ',');
      إذا(rc)
        أرجع FALSE; /* weird */

      إذا(curlx_str_cmp(&name, کلید)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&انکڑے);
        ثابت حرف *src = curlx_str(&انکڑے);
        size_t i;
        size_t outlen = 0;

        إذا(len >= buflen)
          /* does not fit */
          أرجع FALSE;

        لكل(i = 0; i < len; i++) {
          إذا(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        أرجع TRUE;
      }
      إذا(curlx_str_single(&chlg, ','))
        أرجع FALSE;
    }
    وإلا /* odd syntax */
      اكسر;
  } طالما(1);

  أرجع FALSE;
}

ساكن فراغ auth_digest_get_qop_اقدار(ثابت حرف *options, صحيح *قدر)
{
  بنية Curl_str باہرلا;
  /* Initialize the output */
  *قدر = 0;

  طالما(!curlx_str_until(&options, &باہرلا, 32, ',')) {
    إذا(curlx_str_casecompare(&باہرلا, DIGEST_QOP_قدر_STRING_AUTH))
      *قدر |= DIGEST_QOP_قدر_AUTH;
    وإلا إذا(curlx_str_casecompare(&باہرلا, DIGEST_QOP_قدر_STRING_AUTH_INT))
      *قدر |= DIGEST_QOP_قدر_AUTH_INT;
    وإلا إذا(curlx_str_casecompare(&باہرلا, DIGEST_QOP_قدر_STRING_AUTH_CONF))
      *قدر |= DIGEST_QOP_قدر_AUTH_CONF;
    إذا(curlx_str_single(&options, ','))
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
ساكن CURLcode auth_decode_digest_md5_سنیہا(ثابت بنية bufref *chlgref,
                                               حرف *nonce, size_t nlen,
                                               حرف *realm, size_t rlen,
                                               حرف *alg, size_t alen,
                                               حرف *qop, size_t qlen)
{
  ثابت حرف *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  إذا(!Curl_bufref_len(chlgref))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  إذا(!auth_digest_get_کلید_قدر(chlg, "nonce", nonce, nlen))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  إذا(!auth_digest_get_کلید_قدر(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  إذا(!auth_digest_get_کلید_قدر(chlg, "algorithm", alg, alen))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  إذا(!auth_digest_get_کلید_قدر(chlg, "qop", qop, qlen))
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
CURLcode Curl_auth_create_digest_md5_سنیہا(بنية Curl_easy *انکڑے,
                                             ثابت بنية bufref *chlg,
                                             بنية Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             بنية bufref *باہرلا)
{
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  بنية MD5_context *ctxt;
  ثابت حرف *userp = Curl_creds_user(creds);
  ثابت حرف *passwdp = Curl_creds_passwd(creds);
  حرف *response = عدم;
  بنانشانی حرف digest[MD5_DIGEST_LEN];
  حرف HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  حرف HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  حرف resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  حرف nonce[64];
  حرف realm[128];
  حرف algorithm[64];
  حرف qop_options[64];
  صحيح qop_اقدار;
  حرف cnonce[33];
  حرف nonce_گنتی[] = "00000001";
  حرف طریقہ[]     = "AUTHENTICATE";
  حرف qop[]        = DIGEST_QOP_قدر_STRING_AUTH;
  حرف *spn         = عدم;
  حرف *qrealm;
  حرف *qnonce;
  حرف *quserp;
  حرف *qspn;

  /* Decode the challenge message */
  CURLcode نتیجہ = auth_decode_digest_md5_سنیہا(chlg,
                                                   nonce, حجم(nonce),
                                                   realm, حجم(realm),
                                                   algorithm,
                                                   حجم(algorithm),
                                                   qop_options,
                                                   حجم(qop_options));
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* We only support md5 sessions */
  إذا(strcmp(algorithm, "md5-sess"))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_اقدار(qop_options, &qop_اقدار);

  /* We only support auth quality-of-protection */
  إذا(!(qop_اقدار & DIGEST_QOP_قدر_AUTH))
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  نتیجہ = Curl_rand_hex(انکڑے, (بنانشانی حرف *)cnonce, حجم(cnonce));
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  إذا(!ctxt)
    أرجع CURLE_باہرلا_دا_حافظہ;

  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)userp,
                  curlx_uztoui(لڑی_لمبائی(userp)));
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)realm,
                  curlx_uztoui(لڑی_لمبائی(realm)));
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)passwdp,
                  curlx_uztoui(لڑی_لمبائی(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  إذا(!ctxt)
    أرجع CURLE_باہرلا_دا_حافظہ;

  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)nonce,
                  curlx_uztoui(لڑی_لمبائی(nonce)));
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)cnonce,
                  curlx_uztoui(لڑی_لمبائی(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  لكل(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, انکڑے->state.origin->hostname, عدم);
  إذا(!spn)
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  إذا(!ctxt) {
    curlx_free(spn);

    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)طریقہ,
                  curlx_uztoui(لڑی_لمبائی(طریقہ)));
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)spn,
                  curlx_uztoui(لڑی_لمبائی(spn)));
  Curl_MD5_final(ctxt, digest);

  لكل(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  إذا(!ctxt) {
    curlx_free(spn);

    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)nonce,
                  curlx_uztoui(لڑی_لمبائی(nonce)));
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)":", 1);

  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)nonce_گنتی,
                  curlx_uztoui(لڑی_لمبائی(nonce_گنتی)));
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)cnonce,
                  curlx_uztoui(لڑی_لمبائی(cnonce)));
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)":", 1);
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)qop,
                  curlx_uztoui(لڑی_لمبائی(qop)));
  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)":", 1);

  Curl_MD5_update(ctxt, (ثابت بنانشانی حرف *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  لكل(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  إذا(qrealm && qnonce && quserp && qspn)
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
  إذا(!response)
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* Return the response. */
  Curl_bufref_سیٹ(باہرلا, response, لڑی_لمبائی(response), curl_free);
  أرجع نتیجہ;
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
CURLcode Curl_auth_decode_digest_http_سنیہا(ثابت حرف *chlg,
                                              بنية digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  إذا(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  لكل(;;) {
    بنية Curl_str قدر;
    حرف content[DIGEST_MAX_CONTENT_لمائی];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    إذا(Curl_auth_digest_get_pair(chlg, &قدر, content, &chlg)) {
      إذا(curlx_str_casecompare(&قدر, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        إذا(!digest->nonce)
          أرجع CURLE_باہرلا_دا_حافظہ;
      }
      وإلا إذا(curlx_str_casecompare(&قدر, "stale")) {
        إذا(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      وإلا إذا(curlx_str_casecompare(&قدر, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        إذا(!digest->realm)
          أرجع CURLE_باہرلا_دا_حافظہ;
      }
      وإلا إذا(curlx_str_casecompare(&قدر, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        إذا(!digest->opaque)
          أرجع CURLE_باہرلا_دا_حافظہ;
      }
      وإلا إذا(curlx_str_casecompare(&قدر, "qop")) {
        ثابت حرف *token = content;
        بنية Curl_str باہرلا;
        bool لبھیا_Auth = FALSE;
        bool لبھیا_Auth_Int = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        طالما(!curlx_str_until(&token, &باہرلا, 32, ',')) {
          إذا(curlx_str_casecompare(&باہرلا, DIGEST_QOP_قدر_STRING_AUTH))
            لبھیا_Auth = TRUE;
          وإلا إذا(curlx_str_casecompare(&باہرلا,
                                        DIGEST_QOP_قدر_STRING_AUTH_INT))
            لبھیا_Auth_Int = TRUE;
          إذا(curlx_str_single(&token, ','))
            اكسر;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        إذا(لبھیا_Auth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_قدر_STRING_AUTH);
          إذا(!digest->qop)
            أرجع CURLE_باہرلا_دا_حافظہ;
        }
        وإلا إذا(لبھیا_Auth_Int) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_قدر_STRING_AUTH_INT);
          إذا(!digest->qop)
            أرجع CURLE_باہرلا_دا_حافظہ;
        }
      }
      وإلا إذا(curlx_str_casecompare(&قدر, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        إذا(!digest->algorithm)
          أرجع CURLE_باہرلا_دا_حافظہ;

        إذا(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        وإلا إذا(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        وإلا إذا(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        وإلا إذا(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        وإلا إذا(curl_strequal(content, "SHA-512-256")) {
#جے_تعریف_شدہ CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#وإلا /* !CURL_HAVE_SHA512_256 */
          أرجع CURLE_NOT_BUILT_وچ;
#جے_انت /* CURL_HAVE_SHA512_256 */
        }
        وإلا إذا(curl_strequal(content, "SHA-512-256-SESS")) {
#جے_تعریف_شدہ CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#وإلا /* !CURL_HAVE_SHA512_256 */
          أرجع CURLE_NOT_BUILT_وچ;
#جے_انت /* CURL_HAVE_SHA512_256 */
        }
        وإلا
          أرجع CURLE_BAD_CONTENT_ENCODING;
      }
      وإلا إذا(curlx_str_casecompare(&قدر, "userhash")) {
        إذا(curl_strequal(content, "true")) {
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
  إذا(before && !digest->stale)
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  إذا(!digest->nonce)
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  إذا(!digest->qop && (digest->algo & SESSION_ALGO))
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
ساكن CURLcode auth_create_digest_http_سنیہا(
  بنية Curl_easy *انکڑے,
  بنية Curl_creds *creds,
  ثابت بنانشانی حرف *request,
  ثابت بنانشانی حرف *uripath,
  بنية digestdata *digest,
  حرف **outptr, size_t *outlen,
  فراغ (*convert_منزل_ascii)(ثابت بنانشانی حرف *, بنانشانی حرف *),
  CURLcode (*hash)(بنانشانی حرف *, ثابت بنانشانی حرف *, ثابت size_t))
{
  CURLcode نتیجہ;
  ثابت حرف *userp = Curl_creds_user(creds);
  ثابت حرف *passwdp = Curl_creds_passwd(creds);
  بنانشانی حرف hashbuf[32]; /* 32 bytes/256 bits */
  بنانشانی حرف request_digest[65];
  بنانشانی حرف ha1[65];    /* 64 digits and 1 zero byte */
  بنانشانی حرف ha2[65];    /* 64 digits and 1 zero byte */
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
  إذا(!digest->nc)
    digest->nc = 1;

  إذا(!digest->cnonce) {
    حرف cnoncebuf[12];
    نتیجہ = Curl_rand_بائٹاں(انکڑے,
#جے_تعریف_شدہ DEBUGBUILD
                             TRUE,
#جے_انت
                             (بنانشانی حرف *)cnoncebuf,
                             حجم(cnoncebuf));
    إذا(!نتیجہ)
      نتیجہ = curlx_base64_encode((uint8_t *)cnoncebuf, حجم(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    إذا(نتیجہ)
      اذهب oom;

    digest->cnonce = cnonce;
  }

  إذا(digest->userhash) {
    حرف *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    إذا(!hasht) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب oom;
    }

    نتیجہ = hash(hashbuf, (بنانشانی حرف *)hasht, لڑی_لمبائی(hasht));
    curlx_free(hasht);
    إذا(نتیجہ)
      اذهب oom;
    convert_منزل_ascii(hashbuf, (بنانشانی حرف *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  إذا(!hashthis) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب oom;
  }

  نتیجہ = hash(hashbuf, (بنانشانی حرف *)hashthis, لڑی_لمبائی(hashthis));
  curlx_free(hashthis);
  إذا(نتیجہ)
    اذهب oom;
  convert_منزل_ascii(hashbuf, ha1);

  إذا(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    حرف *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    إذا(!tmp) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب oom;
    }

    نتیجہ = hash(hashbuf, (بنانشانی حرف *)tmp, لڑی_لمبائی(tmp));
    curlx_free(tmp);
    إذا(نتیجہ)
      اذهب oom;
    convert_منزل_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ثابت حرف *)uripath);
  إذا(!uri_quoted) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  إذا(!hashthis) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب oom;
  }

  إذا(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    حرف hashed[65];
    حرف *hashthis2;

    نتیجہ = hash(hashbuf, (ثابت بنانشانی حرف *)"", 0);
    إذا(نتیجہ) {
      curlx_free(hashthis);
      اذهب oom;
    }
    convert_منزل_ascii(hashbuf, (بنانشانی حرف *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    إذا(!hashthis) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب oom;
    }
  }

  نتیجہ = hash(hashbuf, (بنانشانی حرف *)hashthis, لڑی_لمبائی(hashthis));
  curlx_free(hashthis);
  إذا(نتیجہ)
    اذهب oom;
  convert_منزل_ascii(hashbuf, ha2);

  إذا(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (بنانشانی صحيح)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  وإلا
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  إذا(!hashthis) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب oom;
  }

  نتیجہ = hash(hashbuf, (بنانشانی حرف *)hashthis, لڑی_لمبائی(hashthis));
  curlx_free(hashthis);
  إذا(نتیجہ)
    اذهب oom;
  convert_منزل_ascii(hashbuf, request_digest);

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
  إذا(!userp_quoted) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب oom;
  }
  إذا(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  وإلا {
    realm_quoted = curlx_malloc(1);
    إذا(realm_quoted)
      realm_quoted[0] = 0;
  }
  إذا(!realm_quoted) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  إذا(!nonce_quoted) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب oom;
  }

  إذا(digest->qop) {
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
                            (بنانشانی صحيح)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  وإلا {
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
  إذا(نتیجہ)
    اذهب oom;

  /* Add the optional fields */
  إذا(digest->opaque) {
    /* Append the opaque */
    حرف *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    إذا(!opaque_quoted) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب oom;
    }
    نتیجہ = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    إذا(نتیجہ)
      اذهب oom;
  }

  إذا(digest->algorithm) {
    /* Append the algorithm */
    نتیجہ = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    إذا(نتیجہ)
      اذهب oom;
  }

  إذا(digest->userhash) {
    /* Append the userhash */
    نتیجہ = curlx_dyn_جوڑو(&response, ", userhash=true");
    إذا(نتیجہ)
      اذهب oom;
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
  إذا(نتیجہ)
    curlx_dyn_free(&response);
  أرجع نتیجہ;
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
CURLcode Curl_auth_create_digest_http_سنیہا(بنية Curl_easy *انکڑے,
                                              بنية Curl_creds *creds,
                                              ثابت بنانشانی حرف *request,
                                              ثابت بنانشانی حرف *uripath,
                                              بنية digestdata *digest,
                                              حرف **outptr, size_t *outlen)
{
  إذا(digest->algo <= ALGO_MD5SESS)
    أرجع auth_create_digest_http_سنیہا(انکڑے, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_منزل_ascii,
                                           Curl_md5it);

  إذا(digest->algo <= ALGO_SHA256SESS)
    أرجع auth_create_digest_http_سنیہا(انکڑے, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_منزل_ascii,
                                           Curl_sha256it);
#جے_تعریف_شدہ CURL_HAVE_SHA512_256
  إذا(digest->algo <= ALGO_SHA512_256SESS)
    أرجع auth_create_digest_http_سنیہا(انکڑے, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_منزل_ascii,
                                           Curl_sha512_256it);
#جے_انت /* CURL_HAVE_SHA512_256 */

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
#جے_انت /* !USE_WINDOWS_SSPI */

#جے_انت /* !CURL_DISABLE_DIGEST_AUTH */
