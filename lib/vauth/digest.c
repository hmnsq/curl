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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH

#समावेश "vauth/vauth.h"
#समावेश "vauth/digest.h"
#समावेश "curlx/base64.h"
#समावेश "curl_md5.h"
#समावेश "curl_sha256.h"
#समावेश "curl_sha512_256.h"
#समावेश "curlx/strparse.h"
#समावेश "rand.h"
#समावेश "escape.h"

#अगर_अघोषित USE_WINDOWS_SSPI
#घोषणा SESSION_ALGO 1 /* for algos with this bit set */

#घोषणा ALGO_MD5 0
#घोषणा ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#घोषणा ALGO_SHA256 2
#घोषणा ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#घोषणा ALGO_SHA512_256 4
#घोषणा ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#घोषणा DIGEST_QOP_मूल्य_AUTH             (1 << 0)
#घोषणा DIGEST_QOP_मूल्य_AUTH_INT         (1 << 1)
#घोषणा DIGEST_QOP_मूल्य_AUTH_CONF        (1 << 2)

#घोषणा DIGEST_QOP_मूल्य_STRING_AUTH      "auth"
#घोषणा DIGEST_QOP_मूल्य_STRING_AUTH_INT  "auth-int"
#घोषणा DIGEST_QOP_मूल्य_STRING_AUTH_CONF "auth-conf"
#पूर्ण_अगर

bool Curl_auth_digest_get_pair(स्थिर अक्षर *str, काष्ठा Curl_str *मूल्य,
                               अक्षर *content, स्थिर अक्षर **endptr)
{
  पूर्णांक c;
  माप_प्रकार content_लंबाई = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  अगर(curlx_str_until(&str, मूल्य, DIGEST_MAX_मूल्य_लंबाई, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    वापस FALSE;

  अगर('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  क्रम(c = DIGEST_MAX_CONTENT_लंबाई; *str && c--; str++) {
    अगर(!escape) {
      चयन(*str) {
      हाल '\\':
        अगर(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          जारी;
        }
        अवरोध;

      हाल ',':
        अगर(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          जारी;
        }
        अवरोध;

      हाल '\r':
      हाल '\n':
        /* end of string */
        अगर(starts_with_quote)
          वापस FALSE; /* No closing quote */
        c = 0;
        जारी;

      हाल '\"':
        अगर(starts_with_quote) {
          /* end of string */
          c = 0;
          जारी;
        }
        अन्यथा
          वापस FALSE;
      }
    }

    अगर(content_लंबाई >= DIGEST_MAX_CONTENT_लंबाई - 1) {
      /* no room for this byte plus the terminator */
      वापस FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_लंबाई++;
  }

  अगर(escape)
    वापस FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  वापस TRUE;
}

#अगर_अघोषित USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
अटल व्योम auth_digest_md5_to_ascii(
  स्थिर अचिन्हित अक्षर *source, /* 16 bytes */
  अचिन्हित अक्षर *dest)         /* 33 bytes */
{
  पूर्णांक i;
  क्रम(i = 0; i < 16; i++)
    curl_msnprintf((अक्षर *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
अटल व्योम auth_digest_sha256_to_ascii(
  स्थिर अचिन्हित अक्षर *source, /* 32 bytes */
  अचिन्हित अक्षर *dest)         /* 65 bytes */
{
  पूर्णांक i;
  क्रम(i = 0; i < 32; i++)
    curl_msnprintf((अक्षर *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
अटल अक्षर *auth_digest_string_quoted(स्थिर अक्षर *s)
{
  काष्ठा dynbuf out;
  curlx_dyn_init(&out, 2048);
  अगर(!*s) /* for zero length input, make sure we return an empty string */
    वापस curlx_strdup("");
  जबतक(*s) {
    CURLcode परिणाम;
    अगर(*s == '"' || *s == '\\') {
      परिणाम = curlx_dyn_addn(&out, "\\", 1);
      अगर(!परिणाम)
        परिणाम = curlx_dyn_addn(&out, s, 1);
    }
    अन्यथा अगर((*s < ' ') || (*s > 0x7e)) {
      अचिन्हित अक्षर buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (अचिन्हित अक्षर)*s);
      परिणाम = curlx_dyn_addn(&out, buf, 3);
    }
    अन्यथा
      परिणाम = curlx_dyn_addn(&out, s, 1);
    अगर(परिणाम)
      वापस शून्य;
    s++;
  }
  वापस curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
अटल bool auth_digest_get_कुंजी_मूल्य(स्थिर अक्षर *chlg, स्थिर अक्षर *कुंजी,
                                      अक्षर *buf, माप_प्रकार buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  करो {
    काष्ठा Curl_str तथ्य;
    काष्ठा Curl_str name;

    curlx_str_passblanks(&chlg);

    अगर(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      पूर्णांक rc = curlx_str_quotedword(&chlg, &तथ्य, 256);
      अगर(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &तथ्य, 256, ',');
      अगर(rc)
        वापस FALSE; /* weird */

      अगर(curlx_str_cmp(&name, कुंजी)) {
        /* if this is our key, return the value */
        माप_प्रकार len = curlx_strlen(&तथ्य);
        स्थिर अक्षर *src = curlx_str(&तथ्य);
        माप_प्रकार i;
        माप_प्रकार outlen = 0;

        अगर(len >= buflen)
          /* does not fit */
          वापस FALSE;

        क्रम(i = 0; i < len; i++) {
          अगर(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        वापस TRUE;
      }
      अगर(curlx_str_single(&chlg, ','))
        वापस FALSE;
    }
    अन्यथा /* odd syntax */
      अवरोध;
  } जबतक(1);

  वापस FALSE;
}

अटल व्योम auth_digest_get_qop_मूल्यावली(स्थिर अक्षर *options, पूर्णांक *मूल्य)
{
  काष्ठा Curl_str out;
  /* Initialize the output */
  *मूल्य = 0;

  जबतक(!curlx_str_until(&options, &out, 32, ',')) {
    अगर(curlx_str_casecompare(&out, DIGEST_QOP_मूल्य_STRING_AUTH))
      *मूल्य |= DIGEST_QOP_मूल्य_AUTH;
    अन्यथा अगर(curlx_str_casecompare(&out, DIGEST_QOP_मूल्य_STRING_AUTH_INT))
      *मूल्य |= DIGEST_QOP_मूल्य_AUTH_INT;
    अन्यथा अगर(curlx_str_casecompare(&out, DIGEST_QOP_मूल्य_STRING_AUTH_CONF))
      *मूल्य |= DIGEST_QOP_मूल्य_AUTH_CONF;
    अगर(curlx_str_single(&options, ','))
      अवरोध;
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
अटल CURLcode auth_decode_digest_md5_message(स्थिर काष्ठा bufref *chlgref,
                                               अक्षर *nonce, माप_प्रकार nlen,
                                               अक्षर *realm, माप_प्रकार rlen,
                                               अक्षर *alg, माप_प्रकार alen,
                                               अक्षर *qop, माप_प्रकार qlen)
{
  स्थिर अक्षर *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  अगर(!Curl_bufref_len(chlgref))
    वापस CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  अगर(!auth_digest_get_कुंजी_मूल्य(chlg, "nonce", nonce, nlen))
    वापस CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  अगर(!auth_digest_get_कुंजी_मूल्य(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  अगर(!auth_digest_get_कुंजी_मूल्य(chlg, "algorithm", alg, alen))
    वापस CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  अगर(!auth_digest_get_कुंजी_मूल्य(chlg, "qop", qop, qlen))
    वापस CURLE_BAD_CONTENT_ENCODING;

  वापस CURLE_OK;
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
bool Curl_auth_is_digest_supported(व्योम)
{
  वापस TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(काष्ठा Curl_easy *तथ्य,
                                             स्थिर काष्ठा bufref *chlg,
                                             काष्ठा Curl_creds *creds,
                                             स्थिर अक्षर *default_service,
                                             काष्ठा bufref *out)
{
  स्थिर अक्षर *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  माप_प्रकार i;
  काष्ठा MD5_context *ctxt;
  स्थिर अक्षर *userp = Curl_creds_user(creds);
  स्थिर अक्षर *passwdp = Curl_creds_passwd(creds);
  अक्षर *response = शून्य;
  अचिन्हित अक्षर digest[MD5_DIGEST_LEN];
  अक्षर HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  अक्षर HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  अक्षर resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  अक्षर nonce[64];
  अक्षर realm[128];
  अक्षर algorithm[64];
  अक्षर qop_options[64];
  पूर्णांक qop_मूल्यावली;
  अक्षर cnonce[33];
  अक्षर nonce_गिनती[] = "00000001";
  अक्षर method[]     = "AUTHENTICATE";
  अक्षर qop[]        = DIGEST_QOP_मूल्य_STRING_AUTH;
  अक्षर *spn         = शून्य;
  अक्षर *qrealm;
  अक्षर *qnonce;
  अक्षर *quserp;
  अक्षर *qspn;

  /* Decode the challenge message */
  CURLcode परिणाम = auth_decode_digest_md5_message(chlg,
                                                   nonce, माप(nonce),
                                                   realm, माप(realm),
                                                   algorithm,
                                                   माप(algorithm),
                                                   qop_options,
                                                   माप(qop_options));
  अगर(परिणाम)
    वापस परिणाम;

  /* We only support md5 sessions */
  अगर(म_भेद(algorithm, "md5-sess"))
    वापस CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_मूल्यावली(qop_options, &qop_मूल्यावली);

  /* We only support auth quality-of-protection */
  अगर(!(qop_मूल्यावली & DIGEST_QOP_मूल्य_AUTH))
    वापस CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  परिणाम = Curl_rand_hex(तथ्य, (अचिन्हित अक्षर *)cnonce, माप(cnonce));
  अगर(परिणाम)
    वापस परिणाम;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  अगर(!ctxt)
    वापस CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)userp,
                  curlx_uztoui(म_माप(userp)));
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)realm,
                  curlx_uztoui(म_माप(realm)));
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)passwdp,
                  curlx_uztoui(म_माप(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  अगर(!ctxt)
    वापस CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)nonce,
                  curlx_uztoui(म_माप(nonce)));
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)cnonce,
                  curlx_uztoui(म_माप(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  क्रम(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, तथ्य->state.origin->hostname, शून्य);
  अगर(!spn)
    वापस CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  अगर(!ctxt) {
    curlx_free(spn);

    वापस CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)method,
                  curlx_uztoui(म_माप(method)));
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)spn,
                  curlx_uztoui(म_माप(spn)));
  Curl_MD5_final(ctxt, digest);

  क्रम(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  अगर(!ctxt) {
    curlx_free(spn);

    वापस CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)nonce,
                  curlx_uztoui(म_माप(nonce)));
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)":", 1);

  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)nonce_गिनती,
                  curlx_uztoui(म_माप(nonce_गिनती)));
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)cnonce,
                  curlx_uztoui(म_माप(cnonce)));
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)":", 1);
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)qop,
                  curlx_uztoui(म_माप(qop)));
  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)":", 1);

  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  क्रम(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  अगर(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_गिनती, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  अगर(!response)
    वापस CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, म_माप(response), curl_free);
  वापस परिणाम;
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
CURLcode Curl_auth_decode_digest_http_message(स्थिर अक्षर *chlg,
                                              काष्ठा digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  अगर(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  क्रम(;;) {
    काष्ठा Curl_str मूल्य;
    अक्षर content[DIGEST_MAX_CONTENT_लंबाई];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    अगर(Curl_auth_digest_get_pair(chlg, &मूल्य, content, &chlg)) {
      अगर(curlx_str_casecompare(&मूल्य, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        अगर(!digest->nonce)
          वापस CURLE_OUT_OF_MEMORY;
      }
      अन्यथा अगर(curlx_str_casecompare(&मूल्य, "stale")) {
        अगर(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      अन्यथा अगर(curlx_str_casecompare(&मूल्य, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        अगर(!digest->realm)
          वापस CURLE_OUT_OF_MEMORY;
      }
      अन्यथा अगर(curlx_str_casecompare(&मूल्य, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        अगर(!digest->opaque)
          वापस CURLE_OUT_OF_MEMORY;
      }
      अन्यथा अगर(curlx_str_casecompare(&मूल्य, "qop")) {
        स्थिर अक्षर *token = content;
        काष्ठा Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        जबतक(!curlx_str_until(&token, &out, 32, ',')) {
          अगर(curlx_str_casecompare(&out, DIGEST_QOP_मूल्य_STRING_AUTH))
            foundAuth = TRUE;
          अन्यथा अगर(curlx_str_casecompare(&out,
                                        DIGEST_QOP_मूल्य_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          अगर(curlx_str_single(&token, ','))
            अवरोध;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        अगर(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_मूल्य_STRING_AUTH);
          अगर(!digest->qop)
            वापस CURLE_OUT_OF_MEMORY;
        }
        अन्यथा अगर(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_मूल्य_STRING_AUTH_INT);
          अगर(!digest->qop)
            वापस CURLE_OUT_OF_MEMORY;
        }
      }
      अन्यथा अगर(curlx_str_casecompare(&मूल्य, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        अगर(!digest->algorithm)
          वापस CURLE_OUT_OF_MEMORY;

        अगर(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        अन्यथा अगर(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        अन्यथा अगर(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        अन्यथा अगर(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        अन्यथा अगर(curl_strequal(content, "SHA-512-256")) {
#अगर_घोषित CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#अन्यथा /* !CURL_HAVE_SHA512_256 */
          वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर /* CURL_HAVE_SHA512_256 */
        }
        अन्यथा अगर(curl_strequal(content, "SHA-512-256-SESS")) {
#अगर_घोषित CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#अन्यथा /* !CURL_HAVE_SHA512_256 */
          वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर /* CURL_HAVE_SHA512_256 */
        }
        अन्यथा
          वापस CURLE_BAD_CONTENT_ENCODING;
      }
      अन्यथा अगर(curlx_str_casecompare(&मूल्य, "userhash")) {
        अगर(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      अन्यथा {
        /* Unknown specifier, ignore it! */
      }
    }
    अन्यथा
      अवरोध; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (व्योम)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  अगर(before && !digest->stale)
    वापस CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  अगर(!digest->nonce)
    वापस CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  अगर(!digest->qop && (digest->algo & SESSION_ALGO))
    वापस CURLE_BAD_CONTENT_ENCODING;

  वापस CURLE_OK;
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
अटल CURLcode auth_create_digest_http_message(
  काष्ठा Curl_easy *तथ्य,
  काष्ठा Curl_creds *creds,
  स्थिर अचिन्हित अक्षर *request,
  स्थिर अचिन्हित अक्षर *uripath,
  काष्ठा digestdata *digest,
  अक्षर **outptr, माप_प्रकार *outlen,
  व्योम (*convert_to_ascii)(स्थिर अचिन्हित अक्षर *, अचिन्हित अक्षर *),
  CURLcode (*hash)(अचिन्हित अक्षर *, स्थिर अचिन्हित अक्षर *, स्थिर माप_प्रकार))
{
  CURLcode परिणाम;
  स्थिर अक्षर *userp = Curl_creds_user(creds);
  स्थिर अक्षर *passwdp = Curl_creds_passwd(creds);
  अचिन्हित अक्षर hashbuf[32]; /* 32 bytes/256 bits */
  अचिन्हित अक्षर request_digest[65];
  अचिन्हित अक्षर ha1[65];    /* 64 digits and 1 zero byte */
  अचिन्हित अक्षर ha2[65];    /* 64 digits and 1 zero byte */
  अक्षर userh[65];
  अक्षर *cnonce = शून्य;
  माप_प्रकार cnonce_sz = 0;
  अक्षर *userp_quoted = शून्य;
  अक्षर *realm_quoted = शून्य;
  अक्षर *nonce_quoted = शून्य;
  अक्षर *hashthis = शून्य;
  अक्षर *uri_quoted = शून्य;
  काष्ठा dynbuf response;
  *outptr = शून्य;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  स_रखो(hashbuf, 0, माप(hashbuf));
  अगर(!digest->nc)
    digest->nc = 1;

  अगर(!digest->cnonce) {
    अक्षर cnoncebuf[12];
    परिणाम = Curl_rand_bytes(तथ्य,
#अगर_घोषित DEBUGBUILD
                             TRUE,
#पूर्ण_अगर
                             (अचिन्हित अक्षर *)cnoncebuf,
                             माप(cnoncebuf));
    अगर(!परिणाम)
      परिणाम = curlx_base64_encode((uint8_t *)cnoncebuf, माप(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    अगर(परिणाम)
      जाओ oom;

    digest->cnonce = cnonce;
  }

  अगर(digest->userhash) {
    अक्षर *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    अगर(!hasht) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ oom;
    }

    परिणाम = hash(hashbuf, (अचिन्हित अक्षर *)hasht, म_माप(hasht));
    curlx_free(hasht);
    अगर(परिणाम)
      जाओ oom;
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
  अगर(!hashthis) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ oom;
  }

  परिणाम = hash(hashbuf, (अचिन्हित अक्षर *)hashthis, म_माप(hashthis));
  curlx_free(hashthis);
  अगर(परिणाम)
    जाओ oom;
  convert_to_ascii(hashbuf, ha1);

  अगर(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    अक्षर *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    अगर(!tmp) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ oom;
    }

    परिणाम = hash(hashbuf, (अचिन्हित अक्षर *)tmp, म_माप(tmp));
    curlx_free(tmp);
    अगर(परिणाम)
      जाओ oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((स्थिर अक्षर *)uripath);
  अगर(!uri_quoted) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  अगर(!hashthis) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ oom;
  }

  अगर(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    अक्षर hashed[65];
    अक्षर *hashthis2;

    परिणाम = hash(hashbuf, (स्थिर अचिन्हित अक्षर *)"", 0);
    अगर(परिणाम) {
      curlx_free(hashthis);
      जाओ oom;
    }
    convert_to_ascii(hashbuf, (अचिन्हित अक्षर *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    अगर(!hashthis) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ oom;
    }
  }

  परिणाम = hash(hashbuf, (अचिन्हित अक्षर *)hashthis, म_माप(hashthis));
  curlx_free(hashthis);
  अगर(परिणाम)
    जाओ oom;
  convert_to_ascii(hashbuf, ha2);

  अगर(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (अचिन्हित पूर्णांक)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  अन्यथा
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  अगर(!hashthis) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ oom;
  }

  परिणाम = hash(hashbuf, (अचिन्हित अक्षर *)hashthis, म_माप(hashthis));
  curlx_free(hashthis);
  अगर(परिणाम)
    जाओ oom;
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
  अगर(!userp_quoted) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ oom;
  }
  अगर(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  अन्यथा {
    realm_quoted = curlx_malloc(1);
    अगर(realm_quoted)
      realm_quoted[0] = 0;
  }
  अगर(!realm_quoted) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  अगर(!nonce_quoted) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ oom;
  }

  अगर(digest->qop) {
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
                            (अचिन्हित पूर्णांक)digest->nc,
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
  अगर(परिणाम)
    जाओ oom;

  /* Add the optional fields */
  अगर(digest->opaque) {
    /* Append the opaque */
    अक्षर *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    अगर(!opaque_quoted) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ oom;
    }
    परिणाम = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    अगर(परिणाम)
      जाओ oom;
  }

  अगर(digest->algorithm) {
    /* Append the algorithm */
    परिणाम = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    अगर(परिणाम)
      जाओ oom;
  }

  अगर(digest->userhash) {
    /* Append the userhash */
    परिणाम = curlx_dyn_add(&response, ", userhash=true");
    अगर(परिणाम)
      जाओ oom;
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
  अगर(परिणाम)
    curlx_dyn_free(&response);
  वापस परिणाम;
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
CURLcode Curl_auth_create_digest_http_message(काष्ठा Curl_easy *तथ्य,
                                              काष्ठा Curl_creds *creds,
                                              स्थिर अचिन्हित अक्षर *request,
                                              स्थिर अचिन्हित अक्षर *uripath,
                                              काष्ठा digestdata *digest,
                                              अक्षर **outptr, माप_प्रकार *outlen)
{
  अगर(digest->algo <= ALGO_MD5SESS)
    वापस auth_create_digest_http_message(तथ्य, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  अगर(digest->algo <= ALGO_SHA256SESS)
    वापस auth_create_digest_http_message(तथ्य, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#अगर_घोषित CURL_HAVE_SHA512_256
  अगर(digest->algo <= ALGO_SHA512_256SESS)
    वापस auth_create_digest_http_message(तथ्य, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#पूर्ण_अगर /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  वापस CURLE_BAD_CONTENT_ENCODING;
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
व्योम Curl_auth_digest_cleanup(काष्ठा digestdata *digest)
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
#पूर्ण_अगर /* !USE_WINDOWS_SSPI */

#पूर्ण_अगर /* !CURL_DISABLE_DIGEST_AUTH */
