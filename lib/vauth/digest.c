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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_DIGEST_AUTH

#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "vauth/digest.h"
#അന്തര്ഭാവ "curlx/base64.h"
#അന്തര്ഭാവ "curl_md5.h"
#അന്തര്ഭാവ "curl_sha256.h"
#അന്തര്ഭാവ "curl_sha512_256.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "escape.h"

#ifndef USE_WINDOWS_SSPI
#define SESSION_ALGO 1 /* for algos with this bit set */

#define ALGO_MD5 0
#define ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#define ALGO_SHA256 2
#define ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#define ALGO_SHA512_256 4
#define ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#define DIGEST_QOP_മൂല്യം_AUTH             (1 << 0)
#define DIGEST_QOP_മൂല്യം_AUTH_INT         (1 << 1)
#define DIGEST_QOP_മൂല്യം_AUTH_CONF        (1 << 2)

#define DIGEST_QOP_മൂല്യം_STRING_AUTH      "auth"
#define DIGEST_QOP_മൂല്യം_STRING_AUTH_INT  "auth-int"
#define DIGEST_QOP_മൂല്യം_STRING_AUTH_CONF "auth-conf"
#endif

bool Curl_auth_digest_get_pair(സ്ഥിരം അക്ഷരം *str, ഘടന Curl_str *മൂല്യം,
                               അക്ഷരം *content, സ്ഥിരം അക്ഷരം **endptr)
{
  പൂർണ്ണസംഖ്യ c;
  size_t content_length = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  എങ്കിൽ(curlx_str_until(&str, മൂല്യം, DIGEST_MAX_മൂല്യം_LENGTH, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    തിരികെ FALSE;

  എങ്കിൽ('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  വേണ്ടി(c = DIGEST_MAX_CONTENT_LENGTH; *str && c--; str++) {
    എങ്കിൽ(!escape) {
      തിരഞ്ഞെടുപ്പ്(*str) {
      അവസ്ഥ '\\':
        എങ്കിൽ(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          തുടരുക;
        }
        നിർത്തുക;

      അവസ്ഥ ',':
        എങ്കിൽ(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          തുടരുക;
        }
        നിർത്തുക;

      അവസ്ഥ '\r':
      അവസ്ഥ '\n':
        /* end of string */
        എങ്കിൽ(starts_with_quote)
          തിരികെ FALSE; /* No closing quote */
        c = 0;
        തുടരുക;

      അവസ്ഥ '\"':
        എങ്കിൽ(starts_with_quote) {
          /* end of string */
          c = 0;
          തുടരുക;
        }
        അല്ലെങ്കിൽ
          തിരികെ FALSE;
      }
    }

    എങ്കിൽ(content_length >= DIGEST_MAX_CONTENT_LENGTH - 1) {
      /* no room for this byte plus the terminator */
      തിരികെ FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_length++;
  }

  എങ്കിൽ(escape)
    തിരികെ FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  തിരികെ TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
സ്ഥിതം ശൂന്യം auth_digest_md5_to_ascii(
  സ്ഥിരം unsigned അക്ഷരം *source, /* 16 bytes */
  unsigned അക്ഷരം *dest)         /* 33 bytes */
{
  പൂർണ്ണസംഖ്യ i;
  വേണ്ടി(i = 0; i < 16; i++)
    curl_msnprintf((അക്ഷരം *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
സ്ഥിതം ശൂന്യം auth_digest_sha256_to_ascii(
  സ്ഥിരം unsigned അക്ഷരം *source, /* 32 bytes */
  unsigned അക്ഷരം *dest)         /* 65 bytes */
{
  പൂർണ്ണസംഖ്യ i;
  വേണ്ടി(i = 0; i < 32; i++)
    curl_msnprintf((അക്ഷരം *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
സ്ഥിതം അക്ഷരം *auth_digest_string_quoted(സ്ഥിരം അക്ഷരം *s)
{
  ഘടന dynbuf out;
  curlx_dyn_init(&out, 2048);
  എങ്കിൽ(!*s) /* for zero length input, make sure we return an empty string */
    തിരികെ curlx_strdup("");
  ആകുമ്പോൾ(*s) {
    CURLcode result;
    എങ്കിൽ(*s == '"' || *s == '\\') {
      result = curlx_dyn_addn(&out, "\\", 1);
      എങ്കിൽ(!result)
        result = curlx_dyn_addn(&out, s, 1);
    }
    അല്ലെങ്കിൽ എങ്കിൽ((*s < ' ') || (*s > 0x7e)) {
      unsigned അക്ഷരം buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (unsigned അക്ഷരം)*s);
      result = curlx_dyn_addn(&out, buf, 3);
    }
    അല്ലെങ്കിൽ
      result = curlx_dyn_addn(&out, s, 1);
    എങ്കിൽ(result)
      തിരികെ ശൂന്യ;
    s++;
  }
  തിരികെ curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
സ്ഥിതം bool auth_digest_get_താക്കോൽ_മൂല്യം(സ്ഥിരം അക്ഷരം *chlg, സ്ഥിരം അക്ഷരം *താക്കോൽ,
                                      അക്ഷരം *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  ചെയ്യുക {
    ഘടന Curl_str data;
    ഘടന Curl_str name;

    curlx_str_passblanks(&chlg);

    എങ്കിൽ(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      പൂർണ്ണസംഖ്യ rc = curlx_str_quotedword(&chlg, &data, 256);
      എങ്കിൽ(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &data, 256, ',');
      എങ്കിൽ(rc)
        തിരികെ FALSE; /* weird */

      എങ്കിൽ(curlx_str_cmp(&name, താക്കോൽ)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&data);
        സ്ഥിരം അക്ഷരം *src = curlx_str(&data);
        size_t i;
        size_t outlen = 0;

        എങ്കിൽ(len >= buflen)
          /* does not fit */
          തിരികെ FALSE;

        വേണ്ടി(i = 0; i < len; i++) {
          എങ്കിൽ(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        തിരികെ TRUE;
      }
      എങ്കിൽ(curlx_str_single(&chlg, ','))
        തിരികെ FALSE;
    }
    അല്ലെങ്കിൽ /* odd syntax */
      നിർത്തുക;
  } ആകുമ്പോൾ(1);

  തിരികെ FALSE;
}

സ്ഥിതം ശൂന്യം auth_digest_get_qop_മൂല്യങ്ങൾ(സ്ഥിരം അക്ഷരം *options, പൂർണ്ണസംഖ്യ *മൂല്യം)
{
  ഘടന Curl_str out;
  /* Initialize the output */
  *മൂല്യം = 0;

  ആകുമ്പോൾ(!curlx_str_until(&options, &out, 32, ',')) {
    എങ്കിൽ(curlx_str_casecompare(&out, DIGEST_QOP_മൂല്യം_STRING_AUTH))
      *മൂല്യം |= DIGEST_QOP_മൂല്യം_AUTH;
    അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&out, DIGEST_QOP_മൂല്യം_STRING_AUTH_INT))
      *മൂല്യം |= DIGEST_QOP_മൂല്യം_AUTH_INT;
    അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&out, DIGEST_QOP_മൂല്യം_STRING_AUTH_CONF))
      *മൂല്യം |= DIGEST_QOP_മൂല്യം_AUTH_CONF;
    എങ്കിൽ(curlx_str_single(&options, ','))
      നിർത്തുക;
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
സ്ഥിതം CURLcode auth_decode_digest_md5_message(സ്ഥിരം ഘടന bufref *chlgref,
                                               അക്ഷരം *nonce, size_t nlen,
                                               അക്ഷരം *realm, size_t rlen,
                                               അക്ഷരം *alg, size_t alen,
                                               അക്ഷരം *qop, size_t qlen)
{
  സ്ഥിരം അക്ഷരം *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  എങ്കിൽ(!Curl_bufref_len(chlgref))
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  എങ്കിൽ(!auth_digest_get_താക്കോൽ_മൂല്യം(chlg, "nonce", nonce, nlen))
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  എങ്കിൽ(!auth_digest_get_താക്കോൽ_മൂല്യം(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  എങ്കിൽ(!auth_digest_get_താക്കോൽ_മൂല്യം(chlg, "algorithm", alg, alen))
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  എങ്കിൽ(!auth_digest_get_താക്കോൽ_മൂല്യം(chlg, "qop", qop, qlen))
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  തിരികെ CURLE_OK;
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
bool Curl_auth_is_digest_supported(ശൂന്യം)
{
  തിരികെ TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(ഘടന Curl_easy *data,
                                             സ്ഥിരം ഘടന bufref *chlg,
                                             ഘടന Curl_creds *creds,
                                             സ്ഥിരം അക്ഷരം *default_service,
                                             ഘടന bufref *out)
{
  സ്ഥിരം അക്ഷരം *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  ഘടന MD5_context *ctxt;
  സ്ഥിരം അക്ഷരം *userp = Curl_creds_user(creds);
  സ്ഥിരം അക്ഷരം *passwdp = Curl_creds_passwd(creds);
  അക്ഷരം *response = ശൂന്യ;
  unsigned അക്ഷരം digest[MD5_DIGEST_LEN];
  അക്ഷരം HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  അക്ഷരം HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  അക്ഷരം resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  അക്ഷരം nonce[64];
  അക്ഷരം realm[128];
  അക്ഷരം algorithm[64];
  അക്ഷരം qop_options[64];
  പൂർണ്ണസംഖ്യ qop_മൂല്യങ്ങൾ;
  അക്ഷരം cnonce[33];
  അക്ഷരം nonce_എണ്ണം[] = "00000001";
  അക്ഷരം method[]     = "AUTHENTICATE";
  അക്ഷരം qop[]        = DIGEST_QOP_മൂല്യം_STRING_AUTH;
  അക്ഷരം *spn         = ശൂന്യ;
  അക്ഷരം *qrealm;
  അക്ഷരം *qnonce;
  അക്ഷരം *quserp;
  അക്ഷരം *qspn;

  /* Decode the challenge message */
  CURLcode result = auth_decode_digest_md5_message(chlg,
                                                   nonce, വലുപ്പം(nonce),
                                                   realm, വലുപ്പം(realm),
                                                   algorithm,
                                                   വലുപ്പം(algorithm),
                                                   qop_options,
                                                   വലുപ്പം(qop_options));
  എങ്കിൽ(result)
    തിരികെ result;

  /* We only support md5 sessions */
  എങ്കിൽ(strcmp(algorithm, "md5-sess"))
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_മൂല്യങ്ങൾ(qop_options, &qop_മൂല്യങ്ങൾ);

  /* We only support auth quality-of-protection */
  എങ്കിൽ(!(qop_മൂല്യങ്ങൾ & DIGEST_QOP_മൂല്യം_AUTH))
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  result = Curl_rand_hex(data, (unsigned അക്ഷരം *)cnonce, വലുപ്പം(cnonce));
  എങ്കിൽ(result)
    തിരികെ result;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  എങ്കിൽ(!ctxt)
    തിരികെ CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)userp,
                  curlx_uztoui(strlen(userp)));
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)":", 1);
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)realm,
                  curlx_uztoui(strlen(realm)));
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)":", 1);
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)passwdp,
                  curlx_uztoui(strlen(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  എങ്കിൽ(!ctxt)
    തിരികെ CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)":", 1);
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)":", 1);
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  വേണ്ടി(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, ശൂന്യ);
  എങ്കിൽ(!spn)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  എങ്കിൽ(!ctxt) {
    curlx_free(spn);

    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)method,
                  curlx_uztoui(strlen(method)));
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)":", 1);
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)spn,
                  curlx_uztoui(strlen(spn)));
  Curl_MD5_final(ctxt, digest);

  വേണ്ടി(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  എങ്കിൽ(!ctxt) {
    curlx_free(spn);

    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)":", 1);
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)":", 1);

  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)nonce_എണ്ണം,
                  curlx_uztoui(strlen(nonce_എണ്ണം)));
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)":", 1);
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)":", 1);
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)qop,
                  curlx_uztoui(strlen(qop)));
  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)":", 1);

  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  വേണ്ടി(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  എങ്കിൽ(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_എണ്ണം, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  എങ്കിൽ(!response)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, strlen(response), curl_free);
  തിരികെ result;
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
CURLcode Curl_auth_decode_digest_http_message(സ്ഥിരം അക്ഷരം *chlg,
                                              ഘടന digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  എങ്കിൽ(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  വേണ്ടി(;;) {
    ഘടന Curl_str മൂല്യം;
    അക്ഷരം content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    എങ്കിൽ(Curl_auth_digest_get_pair(chlg, &മൂല്യം, content, &chlg)) {
      എങ്കിൽ(curlx_str_casecompare(&മൂല്യം, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        എങ്കിൽ(!digest->nonce)
          തിരികെ CURLE_OUT_OF_MEMORY;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&മൂല്യം, "stale")) {
        എങ്കിൽ(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&മൂല്യം, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        എങ്കിൽ(!digest->realm)
          തിരികെ CURLE_OUT_OF_MEMORY;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&മൂല്യം, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        എങ്കിൽ(!digest->opaque)
          തിരികെ CURLE_OUT_OF_MEMORY;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&മൂല്യം, "qop")) {
        സ്ഥിരം അക്ഷരം *token = content;
        ഘടന Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        ആകുമ്പോൾ(!curlx_str_until(&token, &out, 32, ',')) {
          എങ്കിൽ(curlx_str_casecompare(&out, DIGEST_QOP_മൂല്യം_STRING_AUTH))
            foundAuth = TRUE;
          അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&out,
                                        DIGEST_QOP_മൂല്യം_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          എങ്കിൽ(curlx_str_single(&token, ','))
            നിർത്തുക;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        എങ്കിൽ(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_മൂല്യം_STRING_AUTH);
          എങ്കിൽ(!digest->qop)
            തിരികെ CURLE_OUT_OF_MEMORY;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_മൂല്യം_STRING_AUTH_INT);
          എങ്കിൽ(!digest->qop)
            തിരികെ CURLE_OUT_OF_MEMORY;
        }
      }
      അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&മൂല്യം, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        എങ്കിൽ(!digest->algorithm)
          തിരികെ CURLE_OUT_OF_MEMORY;

        എങ്കിൽ(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(content, "SHA-512-256")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#അല്ലെങ്കിൽ /* !CURL_HAVE_SHA512_256 */
          തിരികെ CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#അല്ലെങ്കിൽ /* !CURL_HAVE_SHA512_256 */
          തിരികെ CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        അല്ലെങ്കിൽ
          തിരികെ CURLE_BAD_CONTENT_ENCODING;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&മൂല്യം, "userhash")) {
        എങ്കിൽ(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      അല്ലെങ്കിൽ {
        /* Unknown specifier, ignore it! */
      }
    }
    അല്ലെങ്കിൽ
      നിർത്തുക; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (ശൂന്യം)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  എങ്കിൽ(before && !digest->stale)
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  എങ്കിൽ(!digest->nonce)
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  എങ്കിൽ(!digest->qop && (digest->algo & SESSION_ALGO))
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  തിരികെ CURLE_OK;
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
സ്ഥിതം CURLcode auth_create_digest_http_message(
  ഘടന Curl_easy *data,
  ഘടന Curl_creds *creds,
  സ്ഥിരം unsigned അക്ഷരം *request,
  സ്ഥിരം unsigned അക്ഷരം *uripath,
  ഘടന digestdata *digest,
  അക്ഷരം **outptr, size_t *outlen,
  ശൂന്യം (*convert_to_ascii)(സ്ഥിരം unsigned അക്ഷരം *, unsigned അക്ഷരം *),
  CURLcode (*hash)(unsigned അക്ഷരം *, സ്ഥിരം unsigned അക്ഷരം *, സ്ഥിരം size_t))
{
  CURLcode result;
  സ്ഥിരം അക്ഷരം *userp = Curl_creds_user(creds);
  സ്ഥിരം അക്ഷരം *passwdp = Curl_creds_passwd(creds);
  unsigned അക്ഷരം hashbuf[32]; /* 32 bytes/256 bits */
  unsigned അക്ഷരം request_digest[65];
  unsigned അക്ഷരം ha1[65];    /* 64 digits and 1 zero byte */
  unsigned അക്ഷരം ha2[65];    /* 64 digits and 1 zero byte */
  അക്ഷരം userh[65];
  അക്ഷരം *cnonce = ശൂന്യ;
  size_t cnonce_sz = 0;
  അക്ഷരം *userp_quoted = ശൂന്യ;
  അക്ഷരം *realm_quoted = ശൂന്യ;
  അക്ഷരം *nonce_quoted = ശൂന്യ;
  അക്ഷരം *hashthis = ശൂന്യ;
  അക്ഷരം *uri_quoted = ശൂന്യ;
  ഘടന dynbuf response;
  *outptr = ശൂന്യ;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, വലുപ്പം(hashbuf));
  എങ്കിൽ(!digest->nc)
    digest->nc = 1;

  എങ്കിൽ(!digest->cnonce) {
    അക്ഷരം cnoncebuf[12];
    result = Curl_rand_bytes(data,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (unsigned അക്ഷരം *)cnoncebuf,
                             വലുപ്പം(cnoncebuf));
    എങ്കിൽ(!result)
      result = curlx_base64_encode((uint8_t *)cnoncebuf, വലുപ്പം(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    എങ്കിൽ(result)
      പോകുക oom;

    digest->cnonce = cnonce;
  }

  എങ്കിൽ(digest->userhash) {
    അക്ഷരം *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    എങ്കിൽ(!hasht) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക oom;
    }

    result = hash(hashbuf, (unsigned അക്ഷരം *)hasht, strlen(hasht));
    curlx_free(hasht);
    എങ്കിൽ(result)
      പോകുക oom;
    convert_to_ascii(hashbuf, (unsigned അക്ഷരം *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  എങ്കിൽ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക oom;
  }

  result = hash(hashbuf, (unsigned അക്ഷരം *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  എങ്കിൽ(result)
    പോകുക oom;
  convert_to_ascii(hashbuf, ha1);

  എങ്കിൽ(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    അക്ഷരം *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    എങ്കിൽ(!tmp) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക oom;
    }

    result = hash(hashbuf, (unsigned അക്ഷരം *)tmp, strlen(tmp));
    curlx_free(tmp);
    എങ്കിൽ(result)
      പോകുക oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((സ്ഥിരം അക്ഷരം *)uripath);
  എങ്കിൽ(!uri_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  എങ്കിൽ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക oom;
  }

  എങ്കിൽ(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    അക്ഷരം hashed[65];
    അക്ഷരം *hashthis2;

    result = hash(hashbuf, (സ്ഥിരം unsigned അക്ഷരം *)"", 0);
    എങ്കിൽ(result) {
      curlx_free(hashthis);
      പോകുക oom;
    }
    convert_to_ascii(hashbuf, (unsigned അക്ഷരം *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    എങ്കിൽ(!hashthis) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക oom;
    }
  }

  result = hash(hashbuf, (unsigned അക്ഷരം *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  എങ്കിൽ(result)
    പോകുക oom;
  convert_to_ascii(hashbuf, ha2);

  എങ്കിൽ(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (unsigned പൂർണ്ണസംഖ്യ)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  അല്ലെങ്കിൽ
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  എങ്കിൽ(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക oom;
  }

  result = hash(hashbuf, (unsigned അക്ഷരം *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  എങ്കിൽ(result)
    പോകുക oom;
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
  എങ്കിൽ(!userp_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക oom;
  }
  എങ്കിൽ(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  അല്ലെങ്കിൽ {
    realm_quoted = curlx_malloc(1);
    എങ്കിൽ(realm_quoted)
      realm_quoted[0] = 0;
  }
  എങ്കിൽ(!realm_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  എങ്കിൽ(!nonce_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക oom;
  }

  എങ്കിൽ(digest->qop) {
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
                            (unsigned പൂർണ്ണസംഖ്യ)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  അല്ലെങ്കിൽ {
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
  എങ്കിൽ(result)
    പോകുക oom;

  /* Add the optional fields */
  എങ്കിൽ(digest->opaque) {
    /* Append the opaque */
    അക്ഷരം *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    എങ്കിൽ(!opaque_quoted) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക oom;
    }
    result = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    എങ്കിൽ(result)
      പോകുക oom;
  }

  എങ്കിൽ(digest->algorithm) {
    /* Append the algorithm */
    result = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    എങ്കിൽ(result)
      പോകുക oom;
  }

  എങ്കിൽ(digest->userhash) {
    /* Append the userhash */
    result = curlx_dyn_add(&response, ", userhash=true");
    എങ്കിൽ(result)
      പോകുക oom;
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
  എങ്കിൽ(result)
    curlx_dyn_free(&response);
  തിരികെ result;
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
CURLcode Curl_auth_create_digest_http_message(ഘടന Curl_easy *data,
                                              ഘടന Curl_creds *creds,
                                              സ്ഥിരം unsigned അക്ഷരം *request,
                                              സ്ഥിരം unsigned അക്ഷരം *uripath,
                                              ഘടന digestdata *digest,
                                              അക്ഷരം **outptr, size_t *outlen)
{
  എങ്കിൽ(digest->algo <= ALGO_MD5SESS)
    തിരികെ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  എങ്കിൽ(digest->algo <= ALGO_SHA256SESS)
    തിരികെ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  എങ്കിൽ(digest->algo <= ALGO_SHA512_256SESS)
    തിരികെ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  തിരികെ CURLE_BAD_CONTENT_ENCODING;
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
ശൂന്യം Curl_auth_digest_cleanup(ഘടന digestdata *digest)
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
