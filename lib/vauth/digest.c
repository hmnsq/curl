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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_DIGEST_AUTH

#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "vauth/digest.h"
#அந்தர்பாவ "curlx/base64.h"
#அந்தர்பாவ "curl_md5.h"
#அந்தர்பாவ "curl_sha256.h"
#அந்தர்பாவ "curl_sha512_256.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "escape.h"

#ifndef USE_WINDOWS_SSPI
#define SESSION_ALGO 1 /* for algos with this bit set */

#define ALGO_MD5 0
#define ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#define ALGO_SHA256 2
#define ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#define ALGO_SHA512_256 4
#define ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#define DIGEST_QOP_மதிப்பு_AUTH             (1 << 0)
#define DIGEST_QOP_மதிப்பு_AUTH_INT         (1 << 1)
#define DIGEST_QOP_மதிப்பு_AUTH_CONF        (1 << 2)

#define DIGEST_QOP_மதிப்பு_STRING_AUTH      "auth"
#define DIGEST_QOP_மதிப்பு_STRING_AUTH_INT  "auth-int"
#define DIGEST_QOP_மதிப்பு_STRING_AUTH_CONF "auth-conf"
#endif

bool Curl_auth_digest_get_pair(மாறிலி எழுத்து *str, கட்டமைப்பு Curl_str *மதிப்பு,
                               எழுத்து *content, மாறிலி எழுத்து **endptr)
{
  முழு c;
  size_t content_length = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  எனில்(curlx_str_until(&str, மதிப்பு, DIGEST_MAX_மதிப்பு_LENGTH, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    திரும்பு FALSE;

  எனில்('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  க்கு(c = DIGEST_MAX_CONTENT_LENGTH; *str && c--; str++) {
    எனில்(!escape) {
      தேர்வு(*str) {
      நிலை '\\':
        எனில்(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          தொடர்;
        }
        நிறுத்து;

      நிலை ',':
        எனில்(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          தொடர்;
        }
        நிறுத்து;

      நிலை '\r':
      நிலை '\n':
        /* end of string */
        எனில்(starts_with_quote)
          திரும்பு FALSE; /* No closing quote */
        c = 0;
        தொடர்;

      நிலை '\"':
        எனில்(starts_with_quote) {
          /* end of string */
          c = 0;
          தொடர்;
        }
        இல்லையெனில்
          திரும்பு FALSE;
      }
    }

    எனில்(content_length >= DIGEST_MAX_CONTENT_LENGTH - 1) {
      /* no room for this byte plus the terminator */
      திரும்பு FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_length++;
  }

  எனில்(escape)
    திரும்பு FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  திரும்பு TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
நிலையான வெறுமை auth_digest_md5_to_ascii(
  மாறிலி unsigned எழுத்து *source, /* 16 bytes */
  unsigned எழுத்து *dest)         /* 33 bytes */
{
  முழு i;
  க்கு(i = 0; i < 16; i++)
    curl_msnprintf((எழுத்து *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
நிலையான வெறுமை auth_digest_sha256_to_ascii(
  மாறிலி unsigned எழுத்து *source, /* 32 bytes */
  unsigned எழுத்து *dest)         /* 65 bytes */
{
  முழு i;
  க்கு(i = 0; i < 32; i++)
    curl_msnprintf((எழுத்து *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
நிலையான எழுத்து *auth_digest_string_quoted(மாறிலி எழுத்து *s)
{
  கட்டமைப்பு dynbuf out;
  curlx_dyn_init(&out, 2048);
  எனில்(!*s) /* for zero length input, make sure we return an empty string */
    திரும்பு curlx_strdup("");
  போது(*s) {
    CURLcode result;
    எனில்(*s == '"' || *s == '\\') {
      result = curlx_dyn_addn(&out, "\\", 1);
      எனில்(!result)
        result = curlx_dyn_addn(&out, s, 1);
    }
    இல்லையெனில் எனில்((*s < ' ') || (*s > 0x7e)) {
      unsigned எழுத்து buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (unsigned எழுத்து)*s);
      result = curlx_dyn_addn(&out, buf, 3);
    }
    இல்லையெனில்
      result = curlx_dyn_addn(&out, s, 1);
    எனில்(result)
      திரும்பு வெற்று;
    s++;
  }
  திரும்பு curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
நிலையான bool auth_digest_get_சாவி_மதிப்பு(மாறிலி எழுத்து *chlg, மாறிலி எழுத்து *சாவி,
                                      எழுத்து *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  செய் {
    கட்டமைப்பு Curl_str data;
    கட்டமைப்பு Curl_str name;

    curlx_str_passblanks(&chlg);

    எனில்(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      முழு rc = curlx_str_quotedword(&chlg, &data, 256);
      எனில்(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &data, 256, ',');
      எனில்(rc)
        திரும்பு FALSE; /* weird */

      எனில்(curlx_str_cmp(&name, சாவி)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&data);
        மாறிலி எழுத்து *src = curlx_str(&data);
        size_t i;
        size_t outlen = 0;

        எனில்(len >= buflen)
          /* does not fit */
          திரும்பு FALSE;

        க்கு(i = 0; i < len; i++) {
          எனில்(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        திரும்பு TRUE;
      }
      எனில்(curlx_str_single(&chlg, ','))
        திரும்பு FALSE;
    }
    இல்லையெனில் /* odd syntax */
      நிறுத்து;
  } போது(1);

  திரும்பு FALSE;
}

நிலையான வெறுமை auth_digest_get_qop_மதிப்புகள்(மாறிலி எழுத்து *options, முழு *மதிப்பு)
{
  கட்டமைப்பு Curl_str out;
  /* Initialize the output */
  *மதிப்பு = 0;

  போது(!curlx_str_until(&options, &out, 32, ',')) {
    எனில்(curlx_str_casecompare(&out, DIGEST_QOP_மதிப்பு_STRING_AUTH))
      *மதிப்பு |= DIGEST_QOP_மதிப்பு_AUTH;
    இல்லையெனில் எனில்(curlx_str_casecompare(&out, DIGEST_QOP_மதிப்பு_STRING_AUTH_INT))
      *மதிப்பு |= DIGEST_QOP_மதிப்பு_AUTH_INT;
    இல்லையெனில் எனில்(curlx_str_casecompare(&out, DIGEST_QOP_மதிப்பு_STRING_AUTH_CONF))
      *மதிப்பு |= DIGEST_QOP_மதிப்பு_AUTH_CONF;
    எனில்(curlx_str_single(&options, ','))
      நிறுத்து;
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
நிலையான CURLcode auth_decode_digest_md5_message(மாறிலி கட்டமைப்பு bufref *chlgref,
                                               எழுத்து *nonce, size_t nlen,
                                               எழுத்து *realm, size_t rlen,
                                               எழுத்து *alg, size_t alen,
                                               எழுத்து *qop, size_t qlen)
{
  மாறிலி எழுத்து *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  எனில்(!Curl_bufref_len(chlgref))
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  எனில்(!auth_digest_get_சாவி_மதிப்பு(chlg, "nonce", nonce, nlen))
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  எனில்(!auth_digest_get_சாவி_மதிப்பு(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  எனில்(!auth_digest_get_சாவி_மதிப்பு(chlg, "algorithm", alg, alen))
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  எனில்(!auth_digest_get_சாவி_மதிப்பு(chlg, "qop", qop, qlen))
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  திரும்பு CURLE_OK;
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
bool Curl_auth_is_digest_supported(வெறுமை)
{
  திரும்பு TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(கட்டமைப்பு Curl_easy *data,
                                             மாறிலி கட்டமைப்பு bufref *chlg,
                                             கட்டமைப்பு Curl_creds *creds,
                                             மாறிலி எழுத்து *default_service,
                                             கட்டமைப்பு bufref *out)
{
  மாறிலி எழுத்து *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  கட்டமைப்பு MD5_context *ctxt;
  மாறிலி எழுத்து *userp = Curl_creds_user(creds);
  மாறிலி எழுத்து *passwdp = Curl_creds_passwd(creds);
  எழுத்து *response = வெற்று;
  unsigned எழுத்து digest[MD5_DIGEST_LEN];
  எழுத்து HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  எழுத்து HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  எழுத்து resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  எழுத்து nonce[64];
  எழுத்து realm[128];
  எழுத்து algorithm[64];
  எழுத்து qop_options[64];
  முழு qop_மதிப்புகள்;
  எழுத்து cnonce[33];
  எழுத்து nonce_மொத்தம்[] = "00000001";
  எழுத்து method[]     = "AUTHENTICATE";
  எழுத்து qop[]        = DIGEST_QOP_மதிப்பு_STRING_AUTH;
  எழுத்து *spn         = வெற்று;
  எழுத்து *qrealm;
  எழுத்து *qnonce;
  எழுத்து *quserp;
  எழுத்து *qspn;

  /* Decode the challenge message */
  CURLcode result = auth_decode_digest_md5_message(chlg,
                                                   nonce, அளவு(nonce),
                                                   realm, அளவு(realm),
                                                   algorithm,
                                                   அளவு(algorithm),
                                                   qop_options,
                                                   அளவு(qop_options));
  எனில்(result)
    திரும்பு result;

  /* We only support md5 sessions */
  எனில்(strcmp(algorithm, "md5-sess"))
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_மதிப்புகள்(qop_options, &qop_மதிப்புகள்);

  /* We only support auth quality-of-protection */
  எனில்(!(qop_மதிப்புகள் & DIGEST_QOP_மதிப்பு_AUTH))
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  result = Curl_rand_hex(data, (unsigned எழுத்து *)cnonce, அளவு(cnonce));
  எனில்(result)
    திரும்பு result;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  எனில்(!ctxt)
    திரும்பு CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)userp,
                  curlx_uztoui(strlen(userp)));
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)":", 1);
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)realm,
                  curlx_uztoui(strlen(realm)));
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)":", 1);
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)passwdp,
                  curlx_uztoui(strlen(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  எனில்(!ctxt)
    திரும்பு CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)":", 1);
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)":", 1);
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  க்கு(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, வெற்று);
  எனில்(!spn)
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  எனில்(!ctxt) {
    curlx_free(spn);

    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)method,
                  curlx_uztoui(strlen(method)));
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)":", 1);
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)spn,
                  curlx_uztoui(strlen(spn)));
  Curl_MD5_final(ctxt, digest);

  க்கு(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  எனில்(!ctxt) {
    curlx_free(spn);

    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)":", 1);
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)":", 1);

  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)nonce_மொத்தம்,
                  curlx_uztoui(strlen(nonce_மொத்தம்)));
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)":", 1);
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)":", 1);
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)qop,
                  curlx_uztoui(strlen(qop)));
  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)":", 1);

  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  க்கு(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  எனில்(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_மொத்தம், qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  எனில்(!response)
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, strlen(response), curl_free);
  திரும்பு result;
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
CURLcode Curl_auth_decode_digest_http_message(மாறிலி எழுத்து *chlg,
                                              கட்டமைப்பு digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  எனில்(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  க்கு(;;) {
    கட்டமைப்பு Curl_str மதிப்பு;
    எழுத்து content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    எனில்(Curl_auth_digest_get_pair(chlg, &மதிப்பு, content, &chlg)) {
      எனில்(curlx_str_casecompare(&மதிப்பு, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        எனில்(!digest->nonce)
          திரும்பு CURLE_OUT_OF_MEMORY;
      }
      இல்லையெனில் எனில்(curlx_str_casecompare(&மதிப்பு, "stale")) {
        எனில்(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      இல்லையெனில் எனில்(curlx_str_casecompare(&மதிப்பு, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        எனில்(!digest->realm)
          திரும்பு CURLE_OUT_OF_MEMORY;
      }
      இல்லையெனில் எனில்(curlx_str_casecompare(&மதிப்பு, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        எனில்(!digest->opaque)
          திரும்பு CURLE_OUT_OF_MEMORY;
      }
      இல்லையெனில் எனில்(curlx_str_casecompare(&மதிப்பு, "qop")) {
        மாறிலி எழுத்து *token = content;
        கட்டமைப்பு Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        போது(!curlx_str_until(&token, &out, 32, ',')) {
          எனில்(curlx_str_casecompare(&out, DIGEST_QOP_மதிப்பு_STRING_AUTH))
            foundAuth = TRUE;
          இல்லையெனில் எனில்(curlx_str_casecompare(&out,
                                        DIGEST_QOP_மதிப்பு_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          எனில்(curlx_str_single(&token, ','))
            நிறுத்து;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        எனில்(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_மதிப்பு_STRING_AUTH);
          எனில்(!digest->qop)
            திரும்பு CURLE_OUT_OF_MEMORY;
        }
        இல்லையெனில் எனில்(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_மதிப்பு_STRING_AUTH_INT);
          எனில்(!digest->qop)
            திரும்பு CURLE_OUT_OF_MEMORY;
        }
      }
      இல்லையெனில் எனில்(curlx_str_casecompare(&மதிப்பு, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        எனில்(!digest->algorithm)
          திரும்பு CURLE_OUT_OF_MEMORY;

        எனில்(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        இல்லையெனில் எனில்(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        இல்லையெனில் எனில்(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        இல்லையெனில் எனில்(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        இல்லையெனில் எனில்(curl_strequal(content, "SHA-512-256")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#இல்லையெனில் /* !CURL_HAVE_SHA512_256 */
          திரும்பு CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        இல்லையெனில் எனில்(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#இல்லையெனில் /* !CURL_HAVE_SHA512_256 */
          திரும்பு CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        இல்லையெனில்
          திரும்பு CURLE_BAD_CONTENT_ENCODING;
      }
      இல்லையெனில் எனில்(curlx_str_casecompare(&மதிப்பு, "userhash")) {
        எனில்(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      இல்லையெனில் {
        /* Unknown specifier, ignore it! */
      }
    }
    இல்லையெனில்
      நிறுத்து; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (வெறுமை)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  எனில்(before && !digest->stale)
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  எனில்(!digest->nonce)
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  எனில்(!digest->qop && (digest->algo & SESSION_ALGO))
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  திரும்பு CURLE_OK;
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
நிலையான CURLcode auth_create_digest_http_message(
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு Curl_creds *creds,
  மாறிலி unsigned எழுத்து *request,
  மாறிலி unsigned எழுத்து *uripath,
  கட்டமைப்பு digestdata *digest,
  எழுத்து **outptr, size_t *outlen,
  வெறுமை (*convert_to_ascii)(மாறிலி unsigned எழுத்து *, unsigned எழுத்து *),
  CURLcode (*hash)(unsigned எழுத்து *, மாறிலி unsigned எழுத்து *, மாறிலி size_t))
{
  CURLcode result;
  மாறிலி எழுத்து *userp = Curl_creds_user(creds);
  மாறிலி எழுத்து *passwdp = Curl_creds_passwd(creds);
  unsigned எழுத்து hashbuf[32]; /* 32 bytes/256 bits */
  unsigned எழுத்து request_digest[65];
  unsigned எழுத்து ha1[65];    /* 64 digits and 1 zero byte */
  unsigned எழுத்து ha2[65];    /* 64 digits and 1 zero byte */
  எழுத்து userh[65];
  எழுத்து *cnonce = வெற்று;
  size_t cnonce_sz = 0;
  எழுத்து *userp_quoted = வெற்று;
  எழுத்து *realm_quoted = வெற்று;
  எழுத்து *nonce_quoted = வெற்று;
  எழுத்து *hashthis = வெற்று;
  எழுத்து *uri_quoted = வெற்று;
  கட்டமைப்பு dynbuf response;
  *outptr = வெற்று;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, அளவு(hashbuf));
  எனில்(!digest->nc)
    digest->nc = 1;

  எனில்(!digest->cnonce) {
    எழுத்து cnoncebuf[12];
    result = Curl_rand_bytes(data,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (unsigned எழுத்து *)cnoncebuf,
                             அளவு(cnoncebuf));
    எனில்(!result)
      result = curlx_base64_encode((uint8_t *)cnoncebuf, அளவு(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    எனில்(result)
      செல் oom;

    digest->cnonce = cnonce;
  }

  எனில்(digest->userhash) {
    எழுத்து *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    எனில்(!hasht) {
      result = CURLE_OUT_OF_MEMORY;
      செல் oom;
    }

    result = hash(hashbuf, (unsigned எழுத்து *)hasht, strlen(hasht));
    curlx_free(hasht);
    எனில்(result)
      செல் oom;
    convert_to_ascii(hashbuf, (unsigned எழுத்து *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  எனில்(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    செல் oom;
  }

  result = hash(hashbuf, (unsigned எழுத்து *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  எனில்(result)
    செல் oom;
  convert_to_ascii(hashbuf, ha1);

  எனில்(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    எழுத்து *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    எனில்(!tmp) {
      result = CURLE_OUT_OF_MEMORY;
      செல் oom;
    }

    result = hash(hashbuf, (unsigned எழுத்து *)tmp, strlen(tmp));
    curlx_free(tmp);
    எனில்(result)
      செல் oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((மாறிலி எழுத்து *)uripath);
  எனில்(!uri_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    செல் oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  எனில்(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    செல் oom;
  }

  எனில்(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    எழுத்து hashed[65];
    எழுத்து *hashthis2;

    result = hash(hashbuf, (மாறிலி unsigned எழுத்து *)"", 0);
    எனில்(result) {
      curlx_free(hashthis);
      செல் oom;
    }
    convert_to_ascii(hashbuf, (unsigned எழுத்து *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    எனில்(!hashthis) {
      result = CURLE_OUT_OF_MEMORY;
      செல் oom;
    }
  }

  result = hash(hashbuf, (unsigned எழுத்து *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  எனில்(result)
    செல் oom;
  convert_to_ascii(hashbuf, ha2);

  எனில்(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (unsigned முழு)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  இல்லையெனில்
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  எனில்(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    செல் oom;
  }

  result = hash(hashbuf, (unsigned எழுத்து *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  எனில்(result)
    செல் oom;
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
  எனில்(!userp_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    செல் oom;
  }
  எனில்(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  இல்லையெனில் {
    realm_quoted = curlx_malloc(1);
    எனில்(realm_quoted)
      realm_quoted[0] = 0;
  }
  எனில்(!realm_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    செல் oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  எனில்(!nonce_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    செல் oom;
  }

  எனில்(digest->qop) {
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
                            (unsigned முழு)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  இல்லையெனில் {
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
  எனில்(result)
    செல் oom;

  /* Add the optional fields */
  எனில்(digest->opaque) {
    /* Append the opaque */
    எழுத்து *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    எனில்(!opaque_quoted) {
      result = CURLE_OUT_OF_MEMORY;
      செல் oom;
    }
    result = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    எனில்(result)
      செல் oom;
  }

  எனில்(digest->algorithm) {
    /* Append the algorithm */
    result = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    எனில்(result)
      செல் oom;
  }

  எனில்(digest->userhash) {
    /* Append the userhash */
    result = curlx_dyn_add(&response, ", userhash=true");
    எனில்(result)
      செல் oom;
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
  எனில்(result)
    curlx_dyn_free(&response);
  திரும்பு result;
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
CURLcode Curl_auth_create_digest_http_message(கட்டமைப்பு Curl_easy *data,
                                              கட்டமைப்பு Curl_creds *creds,
                                              மாறிலி unsigned எழுத்து *request,
                                              மாறிலி unsigned எழுத்து *uripath,
                                              கட்டமைப்பு digestdata *digest,
                                              எழுத்து **outptr, size_t *outlen)
{
  எனில்(digest->algo <= ALGO_MD5SESS)
    திரும்பு auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  எனில்(digest->algo <= ALGO_SHA256SESS)
    திரும்பு auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  எனில்(digest->algo <= ALGO_SHA512_256SESS)
    திரும்பு auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  திரும்பு CURLE_BAD_CONTENT_ENCODING;
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
வெறுமை Curl_auth_digest_cleanup(கட்டமைப்பு digestdata *digest)
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
