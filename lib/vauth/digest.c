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

#define DIGEST_QOP_मूल्य_AUTH             (1 << 0)
#define DIGEST_QOP_मूल्य_AUTH_INT         (1 << 1)
#define DIGEST_QOP_मूल्य_AUTH_CONF        (1 << 2)

#define DIGEST_QOP_मूल्य_STRING_AUTH      "auth"
#define DIGEST_QOP_मूल्य_STRING_AUTH_INT  "auth-int"
#define DIGEST_QOP_मूल्य_STRING_AUTH_CONF "auth-conf"
#endif

bool Curl_auth_digest_get_pair(स्थिरांक अक्षर *str, संरचना Curl_str *मूल्य,
                               अक्षर *content, स्थिरांक अक्षर **endptr)
{
  पूर्णांक c;
  माप_प्रकार content_length = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  जर(curlx_str_until(&str, मूल्य, DIGEST_MAX_मूल्य_LENGTH, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    परत FALSE;

  जर('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  साठी(c = DIGEST_MAX_CONTENT_LENGTH; *str && c--; str++) {
    जर(!escape) {
      निवड(*str) {
      स्थिती '\\':
        जर(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          सुरूठेवा;
        }
        थांबा;

      स्थिती ',':
        जर(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          सुरूठेवा;
        }
        थांबा;

      स्थिती '\r':
      स्थिती '\n':
        /* end of string */
        जर(starts_with_quote)
          परत FALSE; /* No closing quote */
        c = 0;
        सुरूठेवा;

      स्थिती '\"':
        जर(starts_with_quote) {
          /* end of string */
          c = 0;
          सुरूठेवा;
        }
        अन्यथा
          परत FALSE;
      }
    }

    जर(content_length >= DIGEST_MAX_CONTENT_LENGTH - 1) {
      /* no room for this byte plus the terminator */
      परत FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_length++;
  }

  जर(escape)
    परत FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  परत TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
स्थिर शून्य auth_digest_md5_to_ascii(
  स्थिरांक अचिन्हित अक्षर *source, /* 16 bytes */
  अचिन्हित अक्षर *dest)         /* 33 bytes */
{
  पूर्णांक i;
  साठी(i = 0; i < 16; i++)
    curl_msnprintf((अक्षर *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
स्थिर शून्य auth_digest_sha256_to_ascii(
  स्थिरांक अचिन्हित अक्षर *source, /* 32 bytes */
  अचिन्हित अक्षर *dest)         /* 65 bytes */
{
  पूर्णांक i;
  साठी(i = 0; i < 32; i++)
    curl_msnprintf((अक्षर *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
स्थिर अक्षर *auth_digest_string_quoted(स्थिरांक अक्षर *s)
{
  संरचना dynbuf out;
  curlx_dyn_init(&out, 2048);
  जर(!*s) /* for zero length input, make sure we return an empty string */
    परत curlx_strdup("");
  पर्यंत(*s) {
    CURLcode result;
    जर(*s == '"' || *s == '\\') {
      result = curlx_dyn_addn(&out, "\\", 1);
      जर(!result)
        result = curlx_dyn_addn(&out, s, 1);
    }
    अन्यथा जर((*s < ' ') || (*s > 0x7e)) {
      अचिन्हित अक्षर buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (अचिन्हित अक्षर)*s);
      result = curlx_dyn_addn(&out, buf, 3);
    }
    अन्यथा
      result = curlx_dyn_addn(&out, s, 1);
    जर(result)
      परत रिक्त;
    s++;
  }
  परत curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
स्थिर bool auth_digest_get_किल्ली_मूल्य(स्थिरांक अक्षर *chlg, स्थिरांक अक्षर *किल्ली,
                                      अक्षर *buf, माप_प्रकार buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  करा {
    संरचना Curl_str data;
    संरचना Curl_str name;

    curlx_str_passblanks(&chlg);

    जर(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      पूर्णांक rc = curlx_str_quotedword(&chlg, &data, 256);
      जर(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &data, 256, ',');
      जर(rc)
        परत FALSE; /* weird */

      जर(curlx_str_cmp(&name, किल्ली)) {
        /* if this is our key, return the value */
        माप_प्रकार len = curlx_strlen(&data);
        स्थिरांक अक्षर *src = curlx_str(&data);
        माप_प्रकार i;
        माप_प्रकार outlen = 0;

        जर(len >= buflen)
          /* does not fit */
          परत FALSE;

        साठी(i = 0; i < len; i++) {
          जर(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        परत TRUE;
      }
      जर(curlx_str_single(&chlg, ','))
        परत FALSE;
    }
    अन्यथा /* odd syntax */
      थांबा;
  } पर्यंत(1);

  परत FALSE;
}

स्थिर शून्य auth_digest_get_qop_मूल्ये(स्थिरांक अक्षर *options, पूर्णांक *मूल्य)
{
  संरचना Curl_str out;
  /* Initialize the output */
  *मूल्य = 0;

  पर्यंत(!curlx_str_until(&options, &out, 32, ',')) {
    जर(curlx_str_casecompare(&out, DIGEST_QOP_मूल्य_STRING_AUTH))
      *मूल्य |= DIGEST_QOP_मूल्य_AUTH;
    अन्यथा जर(curlx_str_casecompare(&out, DIGEST_QOP_मूल्य_STRING_AUTH_INT))
      *मूल्य |= DIGEST_QOP_मूल्य_AUTH_INT;
    अन्यथा जर(curlx_str_casecompare(&out, DIGEST_QOP_मूल्य_STRING_AUTH_CONF))
      *मूल्य |= DIGEST_QOP_मूल्य_AUTH_CONF;
    जर(curlx_str_single(&options, ','))
      थांबा;
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
स्थिर CURLcode auth_decode_digest_md5_message(स्थिरांक संरचना bufref *chlgref,
                                               अक्षर *nonce, माप_प्रकार nlen,
                                               अक्षर *realm, माप_प्रकार rlen,
                                               अक्षर *alg, माप_प्रकार alen,
                                               अक्षर *qop, माप_प्रकार qlen)
{
  स्थिरांक अक्षर *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  जर(!Curl_bufref_len(chlgref))
    परत CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  जर(!auth_digest_get_किल्ली_मूल्य(chlg, "nonce", nonce, nlen))
    परत CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  जर(!auth_digest_get_किल्ली_मूल्य(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  जर(!auth_digest_get_किल्ली_मूल्य(chlg, "algorithm", alg, alen))
    परत CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  जर(!auth_digest_get_किल्ली_मूल्य(chlg, "qop", qop, qlen))
    परत CURLE_BAD_CONTENT_ENCODING;

  परत CURLE_OK;
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
  परत TRUE;
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
                                             स्थिरांक संरचना bufref *chlg,
                                             संरचना Curl_creds *creds,
                                             स्थिरांक अक्षर *default_service,
                                             संरचना bufref *out)
{
  स्थिरांक अक्षर *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  माप_प्रकार i;
  संरचना MD5_context *ctxt;
  स्थिरांक अक्षर *userp = Curl_creds_user(creds);
  स्थिरांक अक्षर *passwdp = Curl_creds_passwd(creds);
  अक्षर *response = रिक्त;
  अचिन्हित अक्षर digest[MD5_DIGEST_LEN];
  अक्षर HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  अक्षर HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  अक्षर resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  अक्षर nonce[64];
  अक्षर realm[128];
  अक्षर algorithm[64];
  अक्षर qop_options[64];
  पूर्णांक qop_मूल्ये;
  अक्षर cnonce[33];
  अक्षर nonce_मोजणी[] = "00000001";
  अक्षर method[]     = "AUTHENTICATE";
  अक्षर qop[]        = DIGEST_QOP_मूल्य_STRING_AUTH;
  अक्षर *spn         = रिक्त;
  अक्षर *qrealm;
  अक्षर *qnonce;
  अक्षर *quserp;
  अक्षर *qspn;

  /* Decode the challenge message */
  CURLcode result = auth_decode_digest_md5_message(chlg,
                                                   nonce, आकार(nonce),
                                                   realm, आकार(realm),
                                                   algorithm,
                                                   आकार(algorithm),
                                                   qop_options,
                                                   आकार(qop_options));
  जर(result)
    परत result;

  /* We only support md5 sessions */
  जर(म_भेद(algorithm, "md5-sess"))
    परत CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_मूल्ये(qop_options, &qop_मूल्ये);

  /* We only support auth quality-of-protection */
  जर(!(qop_मूल्ये & DIGEST_QOP_मूल्य_AUTH))
    परत CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  result = Curl_rand_hex(data, (अचिन्हित अक्षर *)cnonce, आकार(cnonce));
  जर(result)
    परत result;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  जर(!ctxt)
    परत CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)userp,
                  curlx_uztoui(म_माप(userp)));
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)realm,
                  curlx_uztoui(म_माप(realm)));
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)passwdp,
                  curlx_uztoui(म_माप(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  जर(!ctxt)
    परत CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)nonce,
                  curlx_uztoui(म_माप(nonce)));
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)cnonce,
                  curlx_uztoui(म_माप(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  साठी(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, रिक्त);
  जर(!spn)
    परत CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  जर(!ctxt) {
    curlx_free(spn);

    परत CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)method,
                  curlx_uztoui(म_माप(method)));
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)spn,
                  curlx_uztoui(म_माप(spn)));
  Curl_MD5_final(ctxt, digest);

  साठी(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  जर(!ctxt) {
    curlx_free(spn);

    परत CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)nonce,
                  curlx_uztoui(म_माप(nonce)));
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)":", 1);

  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)nonce_मोजणी,
                  curlx_uztoui(म_माप(nonce_मोजणी)));
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)cnonce,
                  curlx_uztoui(म_माप(cnonce)));
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)qop,
                  curlx_uztoui(म_माप(qop)));
  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)":", 1);

  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  साठी(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  जर(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_मोजणी, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  जर(!response)
    परत CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, म_माप(response), curl_free);
  परत result;
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
CURLcode Curl_auth_decode_digest_http_message(स्थिरांक अक्षर *chlg,
                                              संरचना digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  जर(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  साठी(;;) {
    संरचना Curl_str मूल्य;
    अक्षर content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    जर(Curl_auth_digest_get_pair(chlg, &मूल्य, content, &chlg)) {
      जर(curlx_str_casecompare(&मूल्य, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        जर(!digest->nonce)
          परत CURLE_OUT_OF_MEMORY;
      }
      अन्यथा जर(curlx_str_casecompare(&मूल्य, "stale")) {
        जर(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      अन्यथा जर(curlx_str_casecompare(&मूल्य, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        जर(!digest->realm)
          परत CURLE_OUT_OF_MEMORY;
      }
      अन्यथा जर(curlx_str_casecompare(&मूल्य, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        जर(!digest->opaque)
          परत CURLE_OUT_OF_MEMORY;
      }
      अन्यथा जर(curlx_str_casecompare(&मूल्य, "qop")) {
        स्थिरांक अक्षर *token = content;
        संरचना Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        पर्यंत(!curlx_str_until(&token, &out, 32, ',')) {
          जर(curlx_str_casecompare(&out, DIGEST_QOP_मूल्य_STRING_AUTH))
            foundAuth = TRUE;
          अन्यथा जर(curlx_str_casecompare(&out,
                                        DIGEST_QOP_मूल्य_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          जर(curlx_str_single(&token, ','))
            थांबा;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        जर(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_मूल्य_STRING_AUTH);
          जर(!digest->qop)
            परत CURLE_OUT_OF_MEMORY;
        }
        अन्यथा जर(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_मूल्य_STRING_AUTH_INT);
          जर(!digest->qop)
            परत CURLE_OUT_OF_MEMORY;
        }
      }
      अन्यथा जर(curlx_str_casecompare(&मूल्य, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        जर(!digest->algorithm)
          परत CURLE_OUT_OF_MEMORY;

        जर(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        अन्यथा जर(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        अन्यथा जर(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        अन्यथा जर(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        अन्यथा जर(curl_strequal(content, "SHA-512-256")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#अन्यथा /* !CURL_HAVE_SHA512_256 */
          परत CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        अन्यथा जर(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#अन्यथा /* !CURL_HAVE_SHA512_256 */
          परत CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        अन्यथा
          परत CURLE_BAD_CONTENT_ENCODING;
      }
      अन्यथा जर(curlx_str_casecompare(&मूल्य, "userhash")) {
        जर(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      अन्यथा {
        /* Unknown specifier, ignore it! */
      }
    }
    अन्यथा
      थांबा; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (शून्य)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  जर(before && !digest->stale)
    परत CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  जर(!digest->nonce)
    परत CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  जर(!digest->qop && (digest->algo & SESSION_ALGO))
    परत CURLE_BAD_CONTENT_ENCODING;

  परत CURLE_OK;
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
  स्थिरांक अचिन्हित अक्षर *request,
  स्थिरांक अचिन्हित अक्षर *uripath,
  संरचना digestdata *digest,
  अक्षर **outptr, माप_प्रकार *outlen,
  शून्य (*convert_to_ascii)(स्थिरांक अचिन्हित अक्षर *, अचिन्हित अक्षर *),
  CURLcode (*hash)(अचिन्हित अक्षर *, स्थिरांक अचिन्हित अक्षर *, स्थिरांक माप_प्रकार))
{
  CURLcode result;
  स्थिरांक अक्षर *userp = Curl_creds_user(creds);
  स्थिरांक अक्षर *passwdp = Curl_creds_passwd(creds);
  अचिन्हित अक्षर hashbuf[32]; /* 32 bytes/256 bits */
  अचिन्हित अक्षर request_digest[65];
  अचिन्हित अक्षर ha1[65];    /* 64 digits and 1 zero byte */
  अचिन्हित अक्षर ha2[65];    /* 64 digits and 1 zero byte */
  अक्षर userh[65];
  अक्षर *cnonce = रिक्त;
  माप_प्रकार cnonce_sz = 0;
  अक्षर *userp_quoted = रिक्त;
  अक्षर *realm_quoted = रिक्त;
  अक्षर *nonce_quoted = रिक्त;
  अक्षर *hashthis = रिक्त;
  अक्षर *uri_quoted = रिक्त;
  संरचना dynbuf response;
  *outptr = रिक्त;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  स_रखो(hashbuf, 0, आकार(hashbuf));
  जर(!digest->nc)
    digest->nc = 1;

  जर(!digest->cnonce) {
    अक्षर cnoncebuf[12];
    result = Curl_rand_bytes(data,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (अचिन्हित अक्षर *)cnoncebuf,
                             आकार(cnoncebuf));
    जर(!result)
      result = curlx_base64_encode((uint8_t *)cnoncebuf, आकार(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    जर(result)
      जा oom;

    digest->cnonce = cnonce;
  }

  जर(digest->userhash) {
    अक्षर *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    जर(!hasht) {
      result = CURLE_OUT_OF_MEMORY;
      जा oom;
    }

    result = hash(hashbuf, (अचिन्हित अक्षर *)hasht, म_माप(hasht));
    curlx_free(hasht);
    जर(result)
      जा oom;
    convert_to_ascii(hashbuf, (अचिन्हित अक्षर *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  जर(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    जा oom;
  }

  result = hash(hashbuf, (अचिन्हित अक्षर *)hashthis, म_माप(hashthis));
  curlx_free(hashthis);
  जर(result)
    जा oom;
  convert_to_ascii(hashbuf, ha1);

  जर(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    अक्षर *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    जर(!tmp) {
      result = CURLE_OUT_OF_MEMORY;
      जा oom;
    }

    result = hash(hashbuf, (अचिन्हित अक्षर *)tmp, म_माप(tmp));
    curlx_free(tmp);
    जर(result)
      जा oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((स्थिरांक अक्षर *)uripath);
  जर(!uri_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    जा oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  जर(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    जा oom;
  }

  जर(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    अक्षर hashed[65];
    अक्षर *hashthis2;

    result = hash(hashbuf, (स्थिरांक अचिन्हित अक्षर *)"", 0);
    जर(result) {
      curlx_free(hashthis);
      जा oom;
    }
    convert_to_ascii(hashbuf, (अचिन्हित अक्षर *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    जर(!hashthis) {
      result = CURLE_OUT_OF_MEMORY;
      जा oom;
    }
  }

  result = hash(hashbuf, (अचिन्हित अक्षर *)hashthis, म_माप(hashthis));
  curlx_free(hashthis);
  जर(result)
    जा oom;
  convert_to_ascii(hashbuf, ha2);

  जर(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (अचिन्हित पूर्णांक)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  अन्यथा
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  जर(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    जा oom;
  }

  result = hash(hashbuf, (अचिन्हित अक्षर *)hashthis, म_माप(hashthis));
  curlx_free(hashthis);
  जर(result)
    जा oom;
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
  जर(!userp_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    जा oom;
  }
  जर(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  अन्यथा {
    realm_quoted = curlx_malloc(1);
    जर(realm_quoted)
      realm_quoted[0] = 0;
  }
  जर(!realm_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    जा oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  जर(!nonce_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    जा oom;
  }

  जर(digest->qop) {
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
                            (अचिन्हित पूर्णांक)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  अन्यथा {
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
  जर(result)
    जा oom;

  /* Add the optional fields */
  जर(digest->opaque) {
    /* Append the opaque */
    अक्षर *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    जर(!opaque_quoted) {
      result = CURLE_OUT_OF_MEMORY;
      जा oom;
    }
    result = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    जर(result)
      जा oom;
  }

  जर(digest->algorithm) {
    /* Append the algorithm */
    result = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    जर(result)
      जा oom;
  }

  जर(digest->userhash) {
    /* Append the userhash */
    result = curlx_dyn_add(&response, ", userhash=true");
    जर(result)
      जा oom;
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
  जर(result)
    curlx_dyn_free(&response);
  परत result;
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
                                              स्थिरांक अचिन्हित अक्षर *request,
                                              स्थिरांक अचिन्हित अक्षर *uripath,
                                              संरचना digestdata *digest,
                                              अक्षर **outptr, माप_प्रकार *outlen)
{
  जर(digest->algo <= ALGO_MD5SESS)
    परत auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  जर(digest->algo <= ALGO_SHA256SESS)
    परत auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  जर(digest->algo <= ALGO_SHA512_256SESS)
    परत auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  परत CURLE_BAD_CONTENT_ENCODING;
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
