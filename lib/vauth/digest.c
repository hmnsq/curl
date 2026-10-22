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

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH

#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "vauth/digest.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "curl_md5.h"
#अन्तर्भाव "curl_sha256.h"
#अन्तर्भाव "curl_sha512_256.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "escape.h"

#यदि_अपरिभाषितम् USE_WINDOWS_SSPI
#परिभाषय SESSION_ALGO 1 /* for algos with this bit set */

#परिभाषय ALGO_MD5 0
#परिभाषय ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#परिभाषय ALGO_SHA256 2
#परिभाषय ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#परिभाषय ALGO_SHA512_256 4
#परिभाषय ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#परिभाषय DIGEST_QOP_मानम्_AUTH             (1 << 0)
#परिभाषय DIGEST_QOP_मानम्_AUTH_INT         (1 << 1)
#परिभाषय DIGEST_QOP_मानम्_AUTH_CONF        (1 << 2)

#परिभाषय DIGEST_QOP_मानम्_STRING_AUTH      "auth"
#परिभाषय DIGEST_QOP_मानम्_STRING_AUTH_INT  "auth-int"
#परिभाषय DIGEST_QOP_मानम्_STRING_AUTH_CONF "auth-conf"
#यदि_अन्तः

bool Curl_auth_digest_get_pair(ध्रुव वर्ण *str, संरचना Curl_str *मानम्,
                               वर्ण *content, ध्रुव वर्ण **endptr)
{
  पूर्णाङ्क c;
  size_t content_दीर्घता = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  यदि(curlx_str_until(&str, मानम्, DIGEST_MAX_मानम्_दीर्घता, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    प्रत्यागम FALSE;

  यदि('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  प्रति(c = DIGEST_MAX_CONTENT_दीर्घता; *str && c--; str++) {
    यदि(!escape) {
      विकल्प(*str) {
      पक्ष '\\':
        यदि(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          अनुवर्त;
        }
        भङ्ग;

      पक्ष ',':
        यदि(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          अनुवर्त;
        }
        भङ्ग;

      पक्ष '\r':
      पक्ष '\n':
        /* end of string */
        यदि(starts_with_quote)
          प्रत्यागम FALSE; /* No closing quote */
        c = 0;
        अनुवर्त;

      पक्ष '\"':
        यदि(starts_with_quote) {
          /* end of string */
          c = 0;
          अनुवर्त;
        }
        अन्यथा
          प्रत्यागम FALSE;
      }
    }

    यदि(content_दीर्घता >= DIGEST_MAX_CONTENT_दीर्घता - 1) {
      /* no room for this byte plus the terminator */
      प्रत्यागम FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_दीर्घता++;
  }

  यदि(escape)
    प्रत्यागम FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  प्रत्यागम TRUE;
}

#यदि_अपरिभाषितम् USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
स्थिर शून्य auth_digest_md5_गन्तव्यम्_ascii(
  ध्रुव अचिह्नित वर्ण *source, /* 16 bytes */
  अचिह्नित वर्ण *dest)         /* 33 bytes */
{
  पूर्णाङ्क i;
  प्रति(i = 0; i < 16; i++)
    curl_msnprintf((वर्ण *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
स्थिर शून्य auth_digest_sha256_गन्तव्यम्_ascii(
  ध्रुव अचिह्नित वर्ण *source, /* 32 bytes */
  अचिह्नित वर्ण *dest)         /* 65 bytes */
{
  पूर्णाङ्क i;
  प्रति(i = 0; i < 32; i++)
    curl_msnprintf((वर्ण *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
स्थिर वर्ण *auth_digest_string_quoted(ध्रुव वर्ण *s)
{
  संरचना dynbuf बहिर्गमनम्;
  curlx_dyn_init(&बहिर्गमनम्, 2048);
  यदि(!*s) /* for zero length input, make sure we return an empty string */
    प्रत्यागम curlx_strdup("");
  यावत्(*s) {
    CURLcode परिणाम;
    यदि(*s == '"' || *s == '\\') {
      परिणाम = curlx_dyn_addn(&बहिर्गमनम्, "\\", 1);
      यदि(!परिणाम)
        परिणाम = curlx_dyn_addn(&बहिर्गमनम्, s, 1);
    }
    अन्यथा यदि((*s < ' ') || (*s > 0x7e)) {
      अचिह्नित वर्ण buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (अचिह्नित वर्ण)*s);
      परिणाम = curlx_dyn_addn(&बहिर्गमनम्, buf, 3);
    }
    अन्यथा
      परिणाम = curlx_dyn_addn(&बहिर्गमनम्, s, 1);
    यदि(परिणाम)
      प्रत्यागम रिक्त;
    s++;
  }
  प्रत्यागम curlx_dyn_ptr(&बहिर्गमनम्);
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

  कुरु {
    संरचना Curl_str दत्त;
    संरचना Curl_str name;

    curlx_str_passblanks(&chlg);

    यदि(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      पूर्णाङ्क rc = curlx_str_quotedword(&chlg, &दत्त, 256);
      यदि(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &दत्त, 256, ',');
      यदि(rc)
        प्रत्यागम FALSE; /* weird */

      यदि(curlx_str_cmp(&name, कुञ्चिका)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&दत्त);
        ध्रुव वर्ण *src = curlx_str(&दत्त);
        size_t i;
        size_t outlen = 0;

        यदि(len >= buflen)
          /* does not fit */
          प्रत्यागम FALSE;

        प्रति(i = 0; i < len; i++) {
          यदि(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        प्रत्यागम TRUE;
      }
      यदि(curlx_str_single(&chlg, ','))
        प्रत्यागम FALSE;
    }
    अन्यथा /* odd syntax */
      भङ्ग;
  } यावत्(1);

  प्रत्यागम FALSE;
}

स्थिर शून्य auth_digest_get_qop_मानानि(ध्रुव वर्ण *options, पूर्णाङ्क *मानम्)
{
  संरचना Curl_str बहिर्गमनम्;
  /* Initialize the output */
  *मानम् = 0;

  यावत्(!curlx_str_until(&options, &बहिर्गमनम्, 32, ',')) {
    यदि(curlx_str_casecompare(&बहिर्गमनम्, DIGEST_QOP_मानम्_STRING_AUTH))
      *मानम् |= DIGEST_QOP_मानम्_AUTH;
    अन्यथा यदि(curlx_str_casecompare(&बहिर्गमनम्, DIGEST_QOP_मानम्_STRING_AUTH_INT))
      *मानम् |= DIGEST_QOP_मानम्_AUTH_INT;
    अन्यथा यदि(curlx_str_casecompare(&बहिर्गमनम्, DIGEST_QOP_मानम्_STRING_AUTH_CONF))
      *मानम् |= DIGEST_QOP_मानम्_AUTH_CONF;
    यदि(curlx_str_single(&options, ','))
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
स्थिर CURLcode auth_decode_digest_md5_सन्देशः(ध्रुव संरचना bufref *chlgref,
                                               वर्ण *nonce, size_t nlen,
                                               वर्ण *realm, size_t rlen,
                                               वर्ण *alg, size_t alen,
                                               वर्ण *qop, size_t qlen)
{
  ध्रुव वर्ण *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  यदि(!Curl_bufref_len(chlgref))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  यदि(!auth_digest_get_कुञ्चिका_मानम्(chlg, "nonce", nonce, nlen))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  यदि(!auth_digest_get_कुञ्चिका_मानम्(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  यदि(!auth_digest_get_कुञ्चिका_मानम्(chlg, "algorithm", alg, alen))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  यदि(!auth_digest_get_कुञ्चिका_मानम्(chlg, "qop", qop, qlen))
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
CURLcode Curl_auth_create_digest_md5_सन्देशः(संरचना Curl_easy *दत्त,
                                             ध्रुव संरचना bufref *chlg,
                                             संरचना Curl_creds *creds,
                                             ध्रुव वर्ण *default_service,
                                             संरचना bufref *बहिर्गमनम्)
{
  ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  संरचना MD5_context *ctxt;
  ध्रुव वर्ण *userp = Curl_creds_user(creds);
  ध्रुव वर्ण *passwdp = Curl_creds_passwd(creds);
  वर्ण *response = रिक्त;
  अचिह्नित वर्ण digest[MD5_DIGEST_LEN];
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
  वर्ण विधानम्[]     = "AUTHENTICATE";
  वर्ण qop[]        = DIGEST_QOP_मानम्_STRING_AUTH;
  वर्ण *spn         = रिक्त;
  वर्ण *qrealm;
  वर्ण *qnonce;
  वर्ण *quserp;
  वर्ण *qspn;

  /* Decode the challenge message */
  CURLcode परिणाम = auth_decode_digest_md5_सन्देशः(chlg,
                                                   nonce, परिमाण(nonce),
                                                   realm, परिमाण(realm),
                                                   algorithm,
                                                   परिमाण(algorithm),
                                                   qop_options,
                                                   परिमाण(qop_options));
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* We only support md5 sessions */
  यदि(वाक्यं_तुलय(algorithm, "md5-sess"))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_मानानि(qop_options, &qop_मानानि);

  /* We only support auth quality-of-protection */
  यदि(!(qop_मानानि & DIGEST_QOP_मानम्_AUTH))
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  परिणाम = Curl_rand_hex(दत्त, (अचिह्नित वर्ण *)cnonce, परिमाण(cnonce));
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  यदि(!ctxt)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)userp,
                  curlx_uztoui(वाक्यदैर्घ्यम्(userp)));
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)realm,
                  curlx_uztoui(वाक्यदैर्घ्यम्(realm)));
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)passwdp,
                  curlx_uztoui(वाक्यदैर्घ्यम्(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  यदि(!ctxt)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)nonce,
                  curlx_uztoui(वाक्यदैर्घ्यम्(nonce)));
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)cnonce,
                  curlx_uztoui(वाक्यदैर्घ्यम्(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  प्रति(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, दत्त->state.origin->hostname, रिक्त);
  यदि(!spn)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  यदि(!ctxt) {
    curlx_free(spn);

    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)विधानम्,
                  curlx_uztoui(वाक्यदैर्घ्यम्(विधानम्)));
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)spn,
                  curlx_uztoui(वाक्यदैर्घ्यम्(spn)));
  Curl_MD5_final(ctxt, digest);

  प्रति(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  यदि(!ctxt) {
    curlx_free(spn);

    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)nonce,
                  curlx_uztoui(वाक्यदैर्घ्यम्(nonce)));
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)":", 1);

  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)nonce_गणनम्,
                  curlx_uztoui(वाक्यदैर्घ्यम्(nonce_गणनम्)));
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)cnonce,
                  curlx_uztoui(वाक्यदैर्घ्यम्(cnonce)));
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)":", 1);
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)qop,
                  curlx_uztoui(वाक्यदैर्घ्यम्(qop)));
  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)":", 1);

  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  प्रति(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  यदि(qrealm && qnonce && quserp && qspn)
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
  यदि(!response)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Return the response. */
  Curl_bufref_समुच्चयः(बहिर्गमनम्, response, वाक्यदैर्घ्यम्(response), curl_free);
  प्रत्यागम परिणाम;
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
CURLcode Curl_auth_decode_digest_http_सन्देशः(ध्रुव वर्ण *chlg,
                                              संरचना digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  यदि(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  प्रति(;;) {
    संरचना Curl_str मानम्;
    वर्ण content[DIGEST_MAX_CONTENT_दीर्घता];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    यदि(Curl_auth_digest_get_pair(chlg, &मानम्, content, &chlg)) {
      यदि(curlx_str_casecompare(&मानम्, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        यदि(!digest->nonce)
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      अन्यथा यदि(curlx_str_casecompare(&मानम्, "stale")) {
        यदि(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      अन्यथा यदि(curlx_str_casecompare(&मानम्, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        यदि(!digest->realm)
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      अन्यथा यदि(curlx_str_casecompare(&मानम्, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        यदि(!digest->opaque)
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      अन्यथा यदि(curlx_str_casecompare(&मानम्, "qop")) {
        ध्रुव वर्ण *token = content;
        संरचना Curl_str बहिर्गमनम्;
        bool प्राप्तम्_Auth = FALSE;
        bool प्राप्तम्_Auth_Int = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        यावत्(!curlx_str_until(&token, &बहिर्गमनम्, 32, ',')) {
          यदि(curlx_str_casecompare(&बहिर्गमनम्, DIGEST_QOP_मानम्_STRING_AUTH))
            प्राप्तम्_Auth = TRUE;
          अन्यथा यदि(curlx_str_casecompare(&बहिर्गमनम्,
                                        DIGEST_QOP_मानम्_STRING_AUTH_INT))
            प्राप्तम्_Auth_Int = TRUE;
          यदि(curlx_str_single(&token, ','))
            भङ्ग;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        यदि(प्राप्तम्_Auth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_मानम्_STRING_AUTH);
          यदि(!digest->qop)
            प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        }
        अन्यथा यदि(प्राप्तम्_Auth_Int) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_मानम्_STRING_AUTH_INT);
          यदि(!digest->qop)
            प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        }
      }
      अन्यथा यदि(curlx_str_casecompare(&मानम्, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        यदि(!digest->algorithm)
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

        यदि(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        अन्यथा यदि(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        अन्यथा यदि(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        अन्यथा यदि(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        अन्यथा यदि(curl_strequal(content, "SHA-512-256")) {
#यदि_परिभाषितम् CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#अन्यथा /* !CURL_HAVE_SHA512_256 */
          प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः /* CURL_HAVE_SHA512_256 */
        }
        अन्यथा यदि(curl_strequal(content, "SHA-512-256-SESS")) {
#यदि_परिभाषितम् CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#अन्यथा /* !CURL_HAVE_SHA512_256 */
          प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः /* CURL_HAVE_SHA512_256 */
        }
        अन्यथा
          प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
      }
      अन्यथा यदि(curlx_str_casecompare(&मानम्, "userhash")) {
        यदि(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      अन्यथा {
        /* Unknown specifier, ignore it! */
      }
    }
    अन्यथा
      भङ्ग; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (शून्य)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  यदि(before && !digest->stale)
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  यदि(!digest->nonce)
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  यदि(!digest->qop && (digest->algo & SESSION_ALGO))
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
स्थिर CURLcode auth_create_digest_http_सन्देशः(
  संरचना Curl_easy *दत्त,
  संरचना Curl_creds *creds,
  ध्रुव अचिह्नित वर्ण *request,
  ध्रुव अचिह्नित वर्ण *uripath,
  संरचना digestdata *digest,
  वर्ण **outptr, size_t *outlen,
  शून्य (*convert_गन्तव्यम्_ascii)(ध्रुव अचिह्नित वर्ण *, अचिह्नित वर्ण *),
  CURLcode (*hash)(अचिह्नित वर्ण *, ध्रुव अचिह्नित वर्ण *, ध्रुव size_t))
{
  CURLcode परिणाम;
  ध्रुव वर्ण *userp = Curl_creds_user(creds);
  ध्रुव वर्ण *passwdp = Curl_creds_passwd(creds);
  अचिह्नित वर्ण hashbuf[32]; /* 32 bytes/256 bits */
  अचिह्नित वर्ण request_digest[65];
  अचिह्नित वर्ण ha1[65];    /* 64 digits and 1 zero byte */
  अचिह्नित वर्ण ha2[65];    /* 64 digits and 1 zero byte */
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

  स्मृतिं_पूरय(hashbuf, 0, परिमाण(hashbuf));
  यदि(!digest->nc)
    digest->nc = 1;

  यदि(!digest->cnonce) {
    वर्ण cnoncebuf[12];
    परिणाम = Curl_rand_अष्टकानि(दत्त,
#यदि_परिभाषितम् DEBUGBUILD
                             TRUE,
#यदि_अन्तः
                             (अचिह्नित वर्ण *)cnoncebuf,
                             परिमाण(cnoncebuf));
    यदि(!परिणाम)
      परिणाम = curlx_base64_encode((uint8_t *)cnoncebuf, परिमाण(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    यदि(परिणाम)
      गच्छ oom;

    digest->cnonce = cnonce;
  }

  यदि(digest->userhash) {
    वर्ण *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    यदि(!hasht) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ oom;
    }

    परिणाम = hash(hashbuf, (अचिह्नित वर्ण *)hasht, वाक्यदैर्घ्यम्(hasht));
    curlx_free(hasht);
    यदि(परिणाम)
      गच्छ oom;
    convert_गन्तव्यम्_ascii(hashbuf, (अचिह्नित वर्ण *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  यदि(!hashthis) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ oom;
  }

  परिणाम = hash(hashbuf, (अचिह्नित वर्ण *)hashthis, वाक्यदैर्घ्यम्(hashthis));
  curlx_free(hashthis);
  यदि(परिणाम)
    गच्छ oom;
  convert_गन्तव्यम्_ascii(hashbuf, ha1);

  यदि(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    वर्ण *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    यदि(!tmp) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ oom;
    }

    परिणाम = hash(hashbuf, (अचिह्नित वर्ण *)tmp, वाक्यदैर्घ्यम्(tmp));
    curlx_free(tmp);
    यदि(परिणाम)
      गच्छ oom;
    convert_गन्तव्यम्_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ध्रुव वर्ण *)uripath);
  यदि(!uri_quoted) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  यदि(!hashthis) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ oom;
  }

  यदि(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    वर्ण hashed[65];
    वर्ण *hashthis2;

    परिणाम = hash(hashbuf, (ध्रुव अचिह्नित वर्ण *)"", 0);
    यदि(परिणाम) {
      curlx_free(hashthis);
      गच्छ oom;
    }
    convert_गन्तव्यम्_ascii(hashbuf, (अचिह्नित वर्ण *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    यदि(!hashthis) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ oom;
    }
  }

  परिणाम = hash(hashbuf, (अचिह्नित वर्ण *)hashthis, वाक्यदैर्घ्यम्(hashthis));
  curlx_free(hashthis);
  यदि(परिणाम)
    गच्छ oom;
  convert_गन्तव्यम्_ascii(hashbuf, ha2);

  यदि(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (अचिह्नित पूर्णाङ्क)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  अन्यथा
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  यदि(!hashthis) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ oom;
  }

  परिणाम = hash(hashbuf, (अचिह्नित वर्ण *)hashthis, वाक्यदैर्घ्यम्(hashthis));
  curlx_free(hashthis);
  यदि(परिणाम)
    गच्छ oom;
  convert_गन्तव्यम्_ascii(hashbuf, request_digest);

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
  यदि(!userp_quoted) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ oom;
  }
  यदि(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  अन्यथा {
    realm_quoted = curlx_malloc(1);
    यदि(realm_quoted)
      realm_quoted[0] = 0;
  }
  यदि(!realm_quoted) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  यदि(!nonce_quoted) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ oom;
  }

  यदि(digest->qop) {
    परिणाम = curlx_dyn_addf(&response, "username=\"%s\", "
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
                            (अचिह्नित पूर्णाङ्क)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  अन्यथा {
    परिणाम = curlx_dyn_addf(&response, "username=\"%s\", "
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
  यदि(परिणाम)
    गच्छ oom;

  /* Add the optional fields */
  यदि(digest->opaque) {
    /* Append the opaque */
    वर्ण *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    यदि(!opaque_quoted) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ oom;
    }
    परिणाम = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    यदि(परिणाम)
      गच्छ oom;
  }

  यदि(digest->algorithm) {
    /* Append the algorithm */
    परिणाम = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    यदि(परिणाम)
      गच्छ oom;
  }

  यदि(digest->userhash) {
    /* Append the userhash */
    परिणाम = curlx_dyn_सङ्कलय(&response, ", userhash=true");
    यदि(परिणाम)
      गच्छ oom;
  }

  /* Return the output */
  *outptr = curlx_dyn_ptr(&response);
  *outlen = curlx_dyn_len(&response);
  परिणाम = CURLE_OK;

oom:
  curlx_free(nonce_quoted);
  curlx_free(realm_quoted);
  curlx_free(uri_quoted);
  curlx_free(userp_quoted);
  यदि(परिणाम)
    curlx_dyn_free(&response);
  प्रत्यागम परिणाम;
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
CURLcode Curl_auth_create_digest_http_सन्देशः(संरचना Curl_easy *दत्त,
                                              संरचना Curl_creds *creds,
                                              ध्रुव अचिह्नित वर्ण *request,
                                              ध्रुव अचिह्नित वर्ण *uripath,
                                              संरचना digestdata *digest,
                                              वर्ण **outptr, size_t *outlen)
{
  यदि(digest->algo <= ALGO_MD5SESS)
    प्रत्यागम auth_create_digest_http_सन्देशः(दत्त, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_गन्तव्यम्_ascii,
                                           Curl_md5it);

  यदि(digest->algo <= ALGO_SHA256SESS)
    प्रत्यागम auth_create_digest_http_सन्देशः(दत्त, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_गन्तव्यम्_ascii,
                                           Curl_sha256it);
#यदि_परिभाषितम् CURL_HAVE_SHA512_256
  यदि(digest->algo <= ALGO_SHA512_256SESS)
    प्रत्यागम auth_create_digest_http_सन्देशः(दत्त, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_गन्तव्यम्_ascii,
                                           Curl_sha512_256it);
#यदि_अन्तः /* CURL_HAVE_SHA512_256 */

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
#यदि_अन्तः /* !USE_WINDOWS_SSPI */

#यदि_अन्तः /* !CURL_DISABLE_DIGEST_AUTH */
