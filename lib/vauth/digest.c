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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_DIGEST_AUTH

#અન્તર્ભાવ "vauth/vauth.h"
#અન્તર્ભાવ "vauth/digest.h"
#અન્તર્ભાવ "curlx/base64.h"
#અન્તર્ભાવ "curl_md5.h"
#અન્તર્ભાવ "curl_sha256.h"
#અન્તર્ભાવ "curl_sha512_256.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "escape.h"

#ifndef USE_WINDOWS_SSPI
#define SESSION_ALGO 1 /* for algos with this bit set */

#define ALGO_MD5 0
#define ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#define ALGO_SHA256 2
#define ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#define ALGO_SHA512_256 4
#define ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#define DIGEST_QOP_કિંમત_AUTH             (1 << 0)
#define DIGEST_QOP_કિંમત_AUTH_INT         (1 << 1)
#define DIGEST_QOP_કિંમત_AUTH_CONF        (1 << 2)

#define DIGEST_QOP_કિંમત_STRING_AUTH      "auth"
#define DIGEST_QOP_કિંમત_STRING_AUTH_INT  "auth-int"
#define DIGEST_QOP_કિંમત_STRING_AUTH_CONF "auth-conf"
#endif

bool Curl_auth_digest_get_pair(અચલ અક્ષર *str, સંરચના Curl_str *કિંમત,
                               અક્ષર *content, અચલ અક્ષર **endptr)
{
  પૂર્ણાંક c;
  size_t content_length = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  જો(curlx_str_until(&str, કિંમત, DIGEST_MAX_કિંમત_LENGTH, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    પાછા FALSE;

  જો('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  માટે(c = DIGEST_MAX_CONTENT_LENGTH; *str && c--; str++) {
    જો(!escape) {
      પસંદગી(*str) {
      સ્થિતિ '\\':
        જો(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          ચાલુ;
        }
        તોડો;

      સ્થિતિ ',':
        જો(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          ચાલુ;
        }
        તોડો;

      સ્થિતિ '\r':
      સ્થિતિ '\n':
        /* end of string */
        જો(starts_with_quote)
          પાછા FALSE; /* No closing quote */
        c = 0;
        ચાલુ;

      સ્થિતિ '\"':
        જો(starts_with_quote) {
          /* end of string */
          c = 0;
          ચાલુ;
        }
        નહીંતર
          પાછા FALSE;
      }
    }

    જો(content_length >= DIGEST_MAX_CONTENT_LENGTH - 1) {
      /* no room for this byte plus the terminator */
      પાછા FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_length++;
  }

  જો(escape)
    પાછા FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  પાછા TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
સ્થિર શૂન્ય auth_digest_md5_to_ascii(
  અચલ unsigned અક્ષર *source, /* 16 bytes */
  unsigned અક્ષર *dest)         /* 33 bytes */
{
  પૂર્ણાંક i;
  માટે(i = 0; i < 16; i++)
    curl_msnprintf((અક્ષર *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
સ્થિર શૂન્ય auth_digest_sha256_to_ascii(
  અચલ unsigned અક્ષર *source, /* 32 bytes */
  unsigned અક્ષર *dest)         /* 65 bytes */
{
  પૂર્ણાંક i;
  માટે(i = 0; i < 32; i++)
    curl_msnprintf((અક્ષર *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
સ્થિર અક્ષર *auth_digest_string_quoted(અચલ અક્ષર *s)
{
  સંરચના dynbuf out;
  curlx_dyn_init(&out, 2048);
  જો(!*s) /* for zero length input, make sure we return an empty string */
    પાછા curlx_strdup("");
  જ્યારે(*s) {
    CURLcode result;
    જો(*s == '"' || *s == '\\') {
      result = curlx_dyn_addn(&out, "\\", 1);
      જો(!result)
        result = curlx_dyn_addn(&out, s, 1);
    }
    નહીંતર જો((*s < ' ') || (*s > 0x7e)) {
      unsigned અક્ષર buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (unsigned અક્ષર)*s);
      result = curlx_dyn_addn(&out, buf, 3);
    }
    નહીંતર
      result = curlx_dyn_addn(&out, s, 1);
    જો(result)
      પાછા ખાલી;
    s++;
  }
  પાછા curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
સ્થિર bool auth_digest_get_ચાવી_કિંમત(અચલ અક્ષર *chlg, અચલ અક્ષર *ચાવી,
                                      અક્ષર *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  કરો {
    સંરચના Curl_str data;
    સંરચના Curl_str name;

    curlx_str_passblanks(&chlg);

    જો(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      પૂર્ણાંક rc = curlx_str_quotedword(&chlg, &data, 256);
      જો(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &data, 256, ',');
      જો(rc)
        પાછા FALSE; /* weird */

      જો(curlx_str_cmp(&name, ચાવી)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&data);
        અચલ અક્ષર *src = curlx_str(&data);
        size_t i;
        size_t outlen = 0;

        જો(len >= buflen)
          /* does not fit */
          પાછા FALSE;

        માટે(i = 0; i < len; i++) {
          જો(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        પાછા TRUE;
      }
      જો(curlx_str_single(&chlg, ','))
        પાછા FALSE;
    }
    નહીંતર /* odd syntax */
      તોડો;
  } જ્યારે(1);

  પાછા FALSE;
}

સ્થિર શૂન્ય auth_digest_get_qop_કિંમતો(અચલ અક્ષર *options, પૂર્ણાંક *કિંમત)
{
  સંરચના Curl_str out;
  /* Initialize the output */
  *કિંમત = 0;

  જ્યારે(!curlx_str_until(&options, &out, 32, ',')) {
    જો(curlx_str_casecompare(&out, DIGEST_QOP_કિંમત_STRING_AUTH))
      *કિંમત |= DIGEST_QOP_કિંમત_AUTH;
    નહીંતર જો(curlx_str_casecompare(&out, DIGEST_QOP_કિંમત_STRING_AUTH_INT))
      *કિંમત |= DIGEST_QOP_કિંમત_AUTH_INT;
    નહીંતર જો(curlx_str_casecompare(&out, DIGEST_QOP_કિંમત_STRING_AUTH_CONF))
      *કિંમત |= DIGEST_QOP_કિંમત_AUTH_CONF;
    જો(curlx_str_single(&options, ','))
      તોડો;
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
સ્થિર CURLcode auth_decode_digest_md5_message(અચલ સંરચના bufref *chlgref,
                                               અક્ષર *nonce, size_t nlen,
                                               અક્ષર *realm, size_t rlen,
                                               અક્ષર *alg, size_t alen,
                                               અક્ષર *qop, size_t qlen)
{
  અચલ અક્ષર *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  જો(!Curl_bufref_len(chlgref))
    પાછા CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  જો(!auth_digest_get_ચાવી_કિંમત(chlg, "nonce", nonce, nlen))
    પાછા CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  જો(!auth_digest_get_ચાવી_કિંમત(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  જો(!auth_digest_get_ચાવી_કિંમત(chlg, "algorithm", alg, alen))
    પાછા CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  જો(!auth_digest_get_ચાવી_કિંમત(chlg, "qop", qop, qlen))
    પાછા CURLE_BAD_CONTENT_ENCODING;

  પાછા CURLE_OK;
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
bool Curl_auth_is_digest_supported(શૂન્ય)
{
  પાછા TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(સંરચના Curl_easy *data,
                                             અચલ સંરચના bufref *chlg,
                                             સંરચના Curl_creds *creds,
                                             અચલ અક્ષર *default_service,
                                             સંરચના bufref *out)
{
  અચલ અક્ષર *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  સંરચના MD5_context *ctxt;
  અચલ અક્ષર *userp = Curl_creds_user(creds);
  અચલ અક્ષર *passwdp = Curl_creds_passwd(creds);
  અક્ષર *response = ખાલી;
  unsigned અક્ષર digest[MD5_DIGEST_LEN];
  અક્ષર HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  અક્ષર HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  અક્ષર resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  અક્ષર nonce[64];
  અક્ષર realm[128];
  અક્ષર algorithm[64];
  અક્ષર qop_options[64];
  પૂર્ણાંક qop_કિંમતો;
  અક્ષર cnonce[33];
  અક્ષર nonce_ગણતરી[] = "00000001";
  અક્ષર method[]     = "AUTHENTICATE";
  અક્ષર qop[]        = DIGEST_QOP_કિંમત_STRING_AUTH;
  અક્ષર *spn         = ખાલી;
  અક્ષર *qrealm;
  અક્ષર *qnonce;
  અક્ષર *quserp;
  અક્ષર *qspn;

  /* Decode the challenge message */
  CURLcode result = auth_decode_digest_md5_message(chlg,
                                                   nonce, કદ(nonce),
                                                   realm, કદ(realm),
                                                   algorithm,
                                                   કદ(algorithm),
                                                   qop_options,
                                                   કદ(qop_options));
  જો(result)
    પાછા result;

  /* We only support md5 sessions */
  જો(strcmp(algorithm, "md5-sess"))
    પાછા CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_કિંમતો(qop_options, &qop_કિંમતો);

  /* We only support auth quality-of-protection */
  જો(!(qop_કિંમતો & DIGEST_QOP_કિંમત_AUTH))
    પાછા CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  result = Curl_rand_hex(data, (unsigned અક્ષર *)cnonce, કદ(cnonce));
  જો(result)
    પાછા result;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  જો(!ctxt)
    પાછા CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)userp,
                  curlx_uztoui(strlen(userp)));
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)":", 1);
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)realm,
                  curlx_uztoui(strlen(realm)));
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)":", 1);
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)passwdp,
                  curlx_uztoui(strlen(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  જો(!ctxt)
    પાછા CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)":", 1);
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)":", 1);
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  માટે(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, ખાલી);
  જો(!spn)
    પાછા CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  જો(!ctxt) {
    curlx_free(spn);

    પાછા CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)method,
                  curlx_uztoui(strlen(method)));
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)":", 1);
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)spn,
                  curlx_uztoui(strlen(spn)));
  Curl_MD5_final(ctxt, digest);

  માટે(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  જો(!ctxt) {
    curlx_free(spn);

    પાછા CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)":", 1);
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)":", 1);

  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)nonce_ગણતરી,
                  curlx_uztoui(strlen(nonce_ગણતરી)));
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)":", 1);
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)":", 1);
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)qop,
                  curlx_uztoui(strlen(qop)));
  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)":", 1);

  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  માટે(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  જો(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_ગણતરી, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  જો(!response)
    પાછા CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, strlen(response), curl_free);
  પાછા result;
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
CURLcode Curl_auth_decode_digest_http_message(અચલ અક્ષર *chlg,
                                              સંરચના digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  જો(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  માટે(;;) {
    સંરચના Curl_str કિંમત;
    અક્ષર content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    જો(Curl_auth_digest_get_pair(chlg, &કિંમત, content, &chlg)) {
      જો(curlx_str_casecompare(&કિંમત, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        જો(!digest->nonce)
          પાછા CURLE_OUT_OF_MEMORY;
      }
      નહીંતર જો(curlx_str_casecompare(&કિંમત, "stale")) {
        જો(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      નહીંતર જો(curlx_str_casecompare(&કિંમત, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        જો(!digest->realm)
          પાછા CURLE_OUT_OF_MEMORY;
      }
      નહીંતર જો(curlx_str_casecompare(&કિંમત, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        જો(!digest->opaque)
          પાછા CURLE_OUT_OF_MEMORY;
      }
      નહીંતર જો(curlx_str_casecompare(&કિંમત, "qop")) {
        અચલ અક્ષર *token = content;
        સંરચના Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        જ્યારે(!curlx_str_until(&token, &out, 32, ',')) {
          જો(curlx_str_casecompare(&out, DIGEST_QOP_કિંમત_STRING_AUTH))
            foundAuth = TRUE;
          નહીંતર જો(curlx_str_casecompare(&out,
                                        DIGEST_QOP_કિંમત_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          જો(curlx_str_single(&token, ','))
            તોડો;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        જો(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_કિંમત_STRING_AUTH);
          જો(!digest->qop)
            પાછા CURLE_OUT_OF_MEMORY;
        }
        નહીંતર જો(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_કિંમત_STRING_AUTH_INT);
          જો(!digest->qop)
            પાછા CURLE_OUT_OF_MEMORY;
        }
      }
      નહીંતર જો(curlx_str_casecompare(&કિંમત, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        જો(!digest->algorithm)
          પાછા CURLE_OUT_OF_MEMORY;

        જો(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        નહીંતર જો(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        નહીંતર જો(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        નહીંતર જો(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        નહીંતર જો(curl_strequal(content, "SHA-512-256")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#નહીંતર /* !CURL_HAVE_SHA512_256 */
          પાછા CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        નહીંતર જો(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#નહીંતર /* !CURL_HAVE_SHA512_256 */
          પાછા CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        નહીંતર
          પાછા CURLE_BAD_CONTENT_ENCODING;
      }
      નહીંતર જો(curlx_str_casecompare(&કિંમત, "userhash")) {
        જો(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      નહીંતર {
        /* Unknown specifier, ignore it! */
      }
    }
    નહીંતર
      તોડો; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (શૂન્ય)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  જો(before && !digest->stale)
    પાછા CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  જો(!digest->nonce)
    પાછા CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  જો(!digest->qop && (digest->algo & SESSION_ALGO))
    પાછા CURLE_BAD_CONTENT_ENCODING;

  પાછા CURLE_OK;
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
સ્થિર CURLcode auth_create_digest_http_message(
  સંરચના Curl_easy *data,
  સંરચના Curl_creds *creds,
  અચલ unsigned અક્ષર *request,
  અચલ unsigned અક્ષર *uripath,
  સંરચના digestdata *digest,
  અક્ષર **outptr, size_t *outlen,
  શૂન્ય (*convert_to_ascii)(અચલ unsigned અક્ષર *, unsigned અક્ષર *),
  CURLcode (*hash)(unsigned અક્ષર *, અચલ unsigned અક્ષર *, અચલ size_t))
{
  CURLcode result;
  અચલ અક્ષર *userp = Curl_creds_user(creds);
  અચલ અક્ષર *passwdp = Curl_creds_passwd(creds);
  unsigned અક્ષર hashbuf[32]; /* 32 bytes/256 bits */
  unsigned અક્ષર request_digest[65];
  unsigned અક્ષર ha1[65];    /* 64 digits and 1 zero byte */
  unsigned અક્ષર ha2[65];    /* 64 digits and 1 zero byte */
  અક્ષર userh[65];
  અક્ષર *cnonce = ખાલી;
  size_t cnonce_sz = 0;
  અક્ષર *userp_quoted = ખાલી;
  અક્ષર *realm_quoted = ખાલી;
  અક્ષર *nonce_quoted = ખાલી;
  અક્ષર *hashthis = ખાલી;
  અક્ષર *uri_quoted = ખાલી;
  સંરચના dynbuf response;
  *outptr = ખાલી;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, કદ(hashbuf));
  જો(!digest->nc)
    digest->nc = 1;

  જો(!digest->cnonce) {
    અક્ષર cnoncebuf[12];
    result = Curl_rand_bytes(data,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (unsigned અક્ષર *)cnoncebuf,
                             કદ(cnoncebuf));
    જો(!result)
      result = curlx_base64_encode((uint8_t *)cnoncebuf, કદ(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    જો(result)
      જાઓ oom;

    digest->cnonce = cnonce;
  }

  જો(digest->userhash) {
    અક્ષર *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    જો(!hasht) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ oom;
    }

    result = hash(hashbuf, (unsigned અક્ષર *)hasht, strlen(hasht));
    curlx_free(hasht);
    જો(result)
      જાઓ oom;
    convert_to_ascii(hashbuf, (unsigned અક્ષર *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  જો(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ oom;
  }

  result = hash(hashbuf, (unsigned અક્ષર *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  જો(result)
    જાઓ oom;
  convert_to_ascii(hashbuf, ha1);

  જો(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    અક્ષર *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    જો(!tmp) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ oom;
    }

    result = hash(hashbuf, (unsigned અક્ષર *)tmp, strlen(tmp));
    curlx_free(tmp);
    જો(result)
      જાઓ oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((અચલ અક્ષર *)uripath);
  જો(!uri_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  જો(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ oom;
  }

  જો(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    અક્ષર hashed[65];
    અક્ષર *hashthis2;

    result = hash(hashbuf, (અચલ unsigned અક્ષર *)"", 0);
    જો(result) {
      curlx_free(hashthis);
      જાઓ oom;
    }
    convert_to_ascii(hashbuf, (unsigned અક્ષર *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    જો(!hashthis) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ oom;
    }
  }

  result = hash(hashbuf, (unsigned અક્ષર *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  જો(result)
    જાઓ oom;
  convert_to_ascii(hashbuf, ha2);

  જો(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (unsigned પૂર્ણાંક)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  નહીંતર
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  જો(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ oom;
  }

  result = hash(hashbuf, (unsigned અક્ષર *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  જો(result)
    જાઓ oom;
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
  જો(!userp_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ oom;
  }
  જો(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  નહીંતર {
    realm_quoted = curlx_malloc(1);
    જો(realm_quoted)
      realm_quoted[0] = 0;
  }
  જો(!realm_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  જો(!nonce_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ oom;
  }

  જો(digest->qop) {
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
                            (unsigned પૂર્ણાંક)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  નહીંતર {
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
  જો(result)
    જાઓ oom;

  /* Add the optional fields */
  જો(digest->opaque) {
    /* Append the opaque */
    અક્ષર *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    જો(!opaque_quoted) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ oom;
    }
    result = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    જો(result)
      જાઓ oom;
  }

  જો(digest->algorithm) {
    /* Append the algorithm */
    result = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    જો(result)
      જાઓ oom;
  }

  જો(digest->userhash) {
    /* Append the userhash */
    result = curlx_dyn_add(&response, ", userhash=true");
    જો(result)
      જાઓ oom;
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
  જો(result)
    curlx_dyn_free(&response);
  પાછા result;
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
CURLcode Curl_auth_create_digest_http_message(સંરચના Curl_easy *data,
                                              સંરચના Curl_creds *creds,
                                              અચલ unsigned અક્ષર *request,
                                              અચલ unsigned અક્ષર *uripath,
                                              સંરચના digestdata *digest,
                                              અક્ષર **outptr, size_t *outlen)
{
  જો(digest->algo <= ALGO_MD5SESS)
    પાછા auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  જો(digest->algo <= ALGO_SHA256SESS)
    પાછા auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  જો(digest->algo <= ALGO_SHA512_256SESS)
    પાછા auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  પાછા CURLE_BAD_CONTENT_ENCODING;
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
શૂન્ય Curl_auth_digest_cleanup(સંરચના digestdata *digest)
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
