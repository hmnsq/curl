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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_DIGEST_AUTH

#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "vauth/digest.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#ଅନ୍ତର୍ଭାଵ "curl_md5.h"
#ଅନ୍ତର୍ଭାଵ "curl_sha256.h"
#ଅନ୍ତର୍ଭାଵ "curl_sha512_256.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"

#ifndef USE_WINDOWS_SSPI
#define SESSION_ALGO 1 /* for algos with this bit set */

#define ALGO_MD5 0
#define ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#define ALGO_SHA256 2
#define ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#define ALGO_SHA512_256 4
#define ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#define DIGEST_QOP_ମୂଲ୍ୟ_AUTH             (1 << 0)
#define DIGEST_QOP_ମୂଲ୍ୟ_AUTH_INT         (1 << 1)
#define DIGEST_QOP_ମୂଲ୍ୟ_AUTH_CONF        (1 << 2)

#define DIGEST_QOP_ମୂଲ୍ୟ_STRING_AUTH      "auth"
#define DIGEST_QOP_ମୂଲ୍ୟ_STRING_AUTH_INT  "auth-int"
#define DIGEST_QOP_ମୂଲ୍ୟ_STRING_AUTH_CONF "auth-conf"
#endif

bool Curl_auth_digest_get_pair(ଅଚଳ ଅକ୍ଷର *str, ସଂରଚନା Curl_str *ମୂଲ୍ୟ,
                               ଅକ୍ଷର *content, ଅଚଳ ଅକ୍ଷର **endptr)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
  size_t content_length = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  ଯଦି(curlx_str_until(&str, ମୂଲ୍ୟ, DIGEST_MAX_ମୂଲ୍ୟ_LENGTH, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    ଫେରନ୍ତୁ FALSE;

  ଯଦି('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  ପାଇଁ(c = DIGEST_MAX_CONTENT_LENGTH; *str && c--; str++) {
    ଯଦି(!escape) {
      ବାଛ(*str) {
      ସ୍ଥିତି '\\':
        ଯଦି(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          ଜାରି;
        }
        ଭାଙ୍ଗ;

      ସ୍ଥିତି ',':
        ଯଦି(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          ଜାରି;
        }
        ଭାଙ୍ଗ;

      ସ୍ଥିତି '\r':
      ସ୍ଥିତି '\n':
        /* end of string */
        ଯଦି(starts_with_quote)
          ଫେରନ୍ତୁ FALSE; /* No closing quote */
        c = 0;
        ଜାରି;

      ସ୍ଥିତି '\"':
        ଯଦି(starts_with_quote) {
          /* end of string */
          c = 0;
          ଜାରି;
        }
        ଅନ୍ୟଥା
          ଫେରନ୍ତୁ FALSE;
      }
    }

    ଯଦି(content_length >= DIGEST_MAX_CONTENT_LENGTH - 1) {
      /* no room for this byte plus the terminator */
      ଫେରନ୍ତୁ FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_length++;
  }

  ଯଦି(escape)
    ଫେରନ୍ତୁ FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  ଫେରନ୍ତୁ TRUE;
}

#ifndef USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
ସ୍ଥିର ଶୂନ୍ୟ auth_digest_md5_to_ascii(
  ଅଚଳ unsigned ଅକ୍ଷର *source, /* 16 bytes */
  unsigned ଅକ୍ଷର *dest)         /* 33 bytes */
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ପାଇଁ(i = 0; i < 16; i++)
    curl_msnprintf((ଅକ୍ଷର *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
ସ୍ଥିର ଶୂନ୍ୟ auth_digest_sha256_to_ascii(
  ଅଚଳ unsigned ଅକ୍ଷର *source, /* 32 bytes */
  unsigned ଅକ୍ଷର *dest)         /* 65 bytes */
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ପାଇଁ(i = 0; i < 32; i++)
    curl_msnprintf((ଅକ୍ଷର *)&dest[i * 2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
ସ୍ଥିର ଅକ୍ଷର *auth_digest_string_quoted(ଅଚଳ ଅକ୍ଷର *s)
{
  ସଂରଚନା dynbuf out;
  curlx_dyn_init(&out, 2048);
  ଯଦି(!*s) /* for zero length input, make sure we return an empty string */
    ଫେରନ୍ତୁ curlx_strdup("");
  ଯେତେବେଳେ(*s) {
    CURLcode result;
    ଯଦି(*s == '"' || *s == '\\') {
      result = curlx_dyn_addn(&out, "\\", 1);
      ଯଦି(!result)
        result = curlx_dyn_addn(&out, s, 1);
    }
    ଅନ୍ୟଥା ଯଦି((*s < ' ') || (*s > 0x7e)) {
      unsigned ଅକ୍ଷର buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (unsigned ଅକ୍ଷର)*s);
      result = curlx_dyn_addn(&out, buf, 3);
    }
    ଅନ୍ୟଥା
      result = curlx_dyn_addn(&out, s, 1);
    ଯଦି(result)
      ଫେରନ୍ତୁ ଖାଲି;
    s++;
  }
  ଫେରନ୍ତୁ curlx_dyn_ptr(&out);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
ସ୍ଥିର bool auth_digest_get_ଚାବି_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର *chlg, ଅଚଳ ଅକ୍ଷର *ଚାବି,
                                      ଅକ୍ଷର *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  କର {
    ସଂରଚନା Curl_str data;
    ସଂରଚନା Curl_str name;

    curlx_str_passblanks(&chlg);

    ଯଦି(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = curlx_str_quotedword(&chlg, &data, 256);
      ଯଦି(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &data, 256, ',');
      ଯଦି(rc)
        ଫେରନ୍ତୁ FALSE; /* weird */

      ଯଦି(curlx_str_cmp(&name, ଚାବି)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&data);
        ଅଚଳ ଅକ୍ଷର *src = curlx_str(&data);
        size_t i;
        size_t outlen = 0;

        ଯଦି(len >= buflen)
          /* does not fit */
          ଫେରନ୍ତୁ FALSE;

        ପାଇଁ(i = 0; i < len; i++) {
          ଯଦି(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        ଫେରନ୍ତୁ TRUE;
      }
      ଯଦି(curlx_str_single(&chlg, ','))
        ଫେରନ୍ତୁ FALSE;
    }
    ଅନ୍ୟଥା /* odd syntax */
      ଭାଙ୍ଗ;
  } ଯେତେବେଳେ(1);

  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ଶୂନ୍ୟ auth_digest_get_qop_ମୂଲ୍ୟଗୁଡ଼ିକ(ଅଚଳ ଅକ୍ଷର *options, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ମୂଲ୍ୟ)
{
  ସଂରଚନା Curl_str out;
  /* Initialize the output */
  *ମୂଲ୍ୟ = 0;

  ଯେତେବେଳେ(!curlx_str_until(&options, &out, 32, ',')) {
    ଯଦି(curlx_str_casecompare(&out, DIGEST_QOP_ମୂଲ୍ୟ_STRING_AUTH))
      *ମୂଲ୍ୟ |= DIGEST_QOP_ମୂଲ୍ୟ_AUTH;
    ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&out, DIGEST_QOP_ମୂଲ୍ୟ_STRING_AUTH_INT))
      *ମୂଲ୍ୟ |= DIGEST_QOP_ମୂଲ୍ୟ_AUTH_INT;
    ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&out, DIGEST_QOP_ମୂଲ୍ୟ_STRING_AUTH_CONF))
      *ମୂଲ୍ୟ |= DIGEST_QOP_ମୂଲ୍ୟ_AUTH_CONF;
    ଯଦି(curlx_str_single(&options, ','))
      ଭାଙ୍ଗ;
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
ସ୍ଥିର CURLcode auth_decode_digest_md5_message(ଅଚଳ ସଂରଚନା bufref *chlgref,
                                               ଅକ୍ଷର *nonce, size_t nlen,
                                               ଅକ୍ଷର *realm, size_t rlen,
                                               ଅକ୍ଷର *alg, size_t alen,
                                               ଅକ୍ଷର *qop, size_t qlen)
{
  ଅଚଳ ଅକ୍ଷର *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  ଯଦି(!Curl_bufref_len(chlgref))
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  ଯଦି(!auth_digest_get_ଚାବି_ମୂଲ୍ୟ(chlg, "nonce", nonce, nlen))
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  ଯଦି(!auth_digest_get_ଚାବି_ମୂଲ୍ୟ(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  ଯଦି(!auth_digest_get_ଚାବି_ମୂଲ୍ୟ(chlg, "algorithm", alg, alen))
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  ଯଦି(!auth_digest_get_ଚାବି_ମୂଲ୍ୟ(chlg, "qop", qop, qlen))
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  ଫେରନ୍ତୁ CURLE_OK;
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
bool Curl_auth_is_digest_supported(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ TRUE;
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
CURLcode Curl_auth_create_digest_md5_message(ସଂରଚନା Curl_easy *data,
                                             ଅଚଳ ସଂରଚନା bufref *chlg,
                                             ସଂରଚନା Curl_creds *creds,
                                             ଅଚଳ ଅକ୍ଷର *default_service,
                                             ସଂରଚନା bufref *out)
{
  ଅଚଳ ଅକ୍ଷର *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  ସଂରଚନା MD5_context *ctxt;
  ଅଚଳ ଅକ୍ଷର *userp = Curl_creds_user(creds);
  ଅଚଳ ଅକ୍ଷର *passwdp = Curl_creds_passwd(creds);
  ଅକ୍ଷର *response = ଖାଲି;
  unsigned ଅକ୍ଷର digest[MD5_DIGEST_LEN];
  ଅକ୍ଷର HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  ଅକ୍ଷର HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  ଅକ୍ଷର resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  ଅକ୍ଷର nonce[64];
  ଅକ୍ଷର realm[128];
  ଅକ୍ଷର algorithm[64];
  ଅକ୍ଷର qop_options[64];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା qop_ମୂଲ୍ୟଗୁଡ଼ିକ;
  ଅକ୍ଷର cnonce[33];
  ଅକ୍ଷର nonce_ଗଣନା[] = "00000001";
  ଅକ୍ଷର method[]     = "AUTHENTICATE";
  ଅକ୍ଷର qop[]        = DIGEST_QOP_ମୂଲ୍ୟ_STRING_AUTH;
  ଅକ୍ଷର *spn         = ଖାଲି;
  ଅକ୍ଷର *qrealm;
  ଅକ୍ଷର *qnonce;
  ଅକ୍ଷର *quserp;
  ଅକ୍ଷର *qspn;

  /* Decode the challenge message */
  CURLcode result = auth_decode_digest_md5_message(chlg,
                                                   nonce, ଆକାର(nonce),
                                                   realm, ଆକାର(realm),
                                                   algorithm,
                                                   ଆକାର(algorithm),
                                                   qop_options,
                                                   ଆକାର(qop_options));
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* We only support md5 sessions */
  ଯଦି(strcmp(algorithm, "md5-sess"))
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_ମୂଲ୍ୟଗୁଡ଼ିକ(qop_options, &qop_ମୂଲ୍ୟଗୁଡ଼ିକ);

  /* We only support auth quality-of-protection */
  ଯଦି(!(qop_ମୂଲ୍ୟଗୁଡ଼ିକ & DIGEST_QOP_ମୂଲ୍ୟ_AUTH))
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  result = Curl_rand_hex(data, (unsigned ଅକ୍ଷର *)cnonce, ଆକାର(cnonce));
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ଯଦି(!ctxt)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)userp,
                  curlx_uztoui(strlen(userp)));
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)":", 1);
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)realm,
                  curlx_uztoui(strlen(realm)));
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)":", 1);
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)passwdp,
                  curlx_uztoui(strlen(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ଯଦି(!ctxt)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)":", 1);
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)":", 1);
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  ପାଇଁ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, ଖାଲି);
  ଯଦି(!spn)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ଯଦି(!ctxt) {
    curlx_free(spn);

    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)method,
                  curlx_uztoui(strlen(method)));
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)":", 1);
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)spn,
                  curlx_uztoui(strlen(spn)));
  Curl_MD5_final(ctxt, digest);

  ପାଇଁ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ଯଦି(!ctxt) {
    curlx_free(spn);

    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)":", 1);
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)nonce,
                  curlx_uztoui(strlen(nonce)));
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)":", 1);

  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)nonce_ଗଣନା,
                  curlx_uztoui(strlen(nonce_ଗଣନା)));
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)":", 1);
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)cnonce,
                  curlx_uztoui(strlen(cnonce)));
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)":", 1);
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)qop,
                  curlx_uztoui(strlen(qop)));
  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)":", 1);

  Curl_MD5_update(ctxt, (ଅଚଳ unsigned ଅକ୍ଷର *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  ପାଇଁ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  ଯଦି(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_ଗଣନା, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  ଯଦି(!response)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* Return the response. */
  Curl_bufref_set(out, response, strlen(response), curl_free);
  ଫେରନ୍ତୁ result;
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
CURLcode Curl_auth_decode_digest_http_message(ଅଚଳ ଅକ୍ଷର *chlg,
                                              ସଂରଚନା digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  ଯଦି(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  ପାଇଁ(;;) {
    ସଂରଚନା Curl_str ମୂଲ୍ୟ;
    ଅକ୍ଷର content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    ଯଦି(Curl_auth_digest_get_pair(chlg, &ମୂଲ୍ୟ, content, &chlg)) {
      ଯଦି(curlx_str_casecompare(&ମୂଲ୍ୟ, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        ଯଦି(!digest->nonce)
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
      ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&ମୂଲ୍ୟ, "stale")) {
        ଯଦି(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&ମୂଲ୍ୟ, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        ଯଦି(!digest->realm)
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
      ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&ମୂଲ୍ୟ, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        ଯଦି(!digest->opaque)
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
      ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&ମୂଲ୍ୟ, "qop")) {
        ଅଚଳ ଅକ୍ଷର *token = content;
        ସଂରଚନା Curl_str out;
        bool foundAuth = FALSE;
        bool foundAuthInt = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        ଯେତେବେଳେ(!curlx_str_until(&token, &out, 32, ',')) {
          ଯଦି(curlx_str_casecompare(&out, DIGEST_QOP_ମୂଲ୍ୟ_STRING_AUTH))
            foundAuth = TRUE;
          ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&out,
                                        DIGEST_QOP_ମୂଲ୍ୟ_STRING_AUTH_INT))
            foundAuthInt = TRUE;
          ଯଦି(curlx_str_single(&token, ','))
            ଭାଙ୍ଗ;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        ଯଦି(foundAuth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_ମୂଲ୍ୟ_STRING_AUTH);
          ଯଦି(!digest->qop)
            ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
        }
        ଅନ୍ୟଥା ଯଦି(foundAuthInt) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_ମୂଲ୍ୟ_STRING_AUTH_INT);
          ଯଦି(!digest->qop)
            ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
        }
      }
      ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&ମୂଲ୍ୟ, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        ଯଦି(!digest->algorithm)
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

        ଯଦି(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        ଅନ୍ୟଥା ଯଦି(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        ଅନ୍ୟଥା ଯଦି(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        ଅନ୍ୟଥା ଯଦି(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        ଅନ୍ୟଥା ଯଦି(curl_strequal(content, "SHA-512-256")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#ଅନ୍ୟଥା /* !CURL_HAVE_SHA512_256 */
          ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        ଅନ୍ୟଥା ଯଦି(curl_strequal(content, "SHA-512-256-SESS")) {
#ifdef CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#ଅନ୍ୟଥା /* !CURL_HAVE_SHA512_256 */
          ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif /* CURL_HAVE_SHA512_256 */
        }
        ଅନ୍ୟଥା
          ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;
      }
      ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&ମୂଲ୍ୟ, "userhash")) {
        ଯଦି(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      ଅନ୍ୟଥା {
        /* Unknown specifier, ignore it! */
      }
    }
    ଅନ୍ୟଥା
      ଭାଙ୍ଗ; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (ଶୂନ୍ୟ)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  ଯଦି(before && !digest->stale)
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  ଯଦି(!digest->nonce)
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  ଯଦି(!digest->qop && (digest->algo & SESSION_ALGO))
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  ଫେରନ୍ତୁ CURLE_OK;
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
ସ୍ଥିର CURLcode auth_create_digest_http_message(
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା Curl_creds *creds,
  ଅଚଳ unsigned ଅକ୍ଷର *request,
  ଅଚଳ unsigned ଅକ୍ଷର *uripath,
  ସଂରଚନା digestdata *digest,
  ଅକ୍ଷର **outptr, size_t *outlen,
  ଶୂନ୍ୟ (*convert_to_ascii)(ଅଚଳ unsigned ଅକ୍ଷର *, unsigned ଅକ୍ଷର *),
  CURLcode (*hash)(unsigned ଅକ୍ଷର *, ଅଚଳ unsigned ଅକ୍ଷର *, ଅଚଳ size_t))
{
  CURLcode result;
  ଅଚଳ ଅକ୍ଷର *userp = Curl_creds_user(creds);
  ଅଚଳ ଅକ୍ଷର *passwdp = Curl_creds_passwd(creds);
  unsigned ଅକ୍ଷର hashbuf[32]; /* 32 bytes/256 bits */
  unsigned ଅକ୍ଷର request_digest[65];
  unsigned ଅକ୍ଷର ha1[65];    /* 64 digits and 1 zero byte */
  unsigned ଅକ୍ଷର ha2[65];    /* 64 digits and 1 zero byte */
  ଅକ୍ଷର userh[65];
  ଅକ୍ଷର *cnonce = ଖାଲି;
  size_t cnonce_sz = 0;
  ଅକ୍ଷର *userp_quoted = ଖାଲି;
  ଅକ୍ଷର *realm_quoted = ଖାଲି;
  ଅକ୍ଷର *nonce_quoted = ଖାଲି;
  ଅକ୍ଷର *hashthis = ଖାଲି;
  ଅକ୍ଷର *uri_quoted = ଖାଲି;
  ସଂରଚନା dynbuf response;
  *outptr = ଖାଲି;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, ଆକାର(hashbuf));
  ଯଦି(!digest->nc)
    digest->nc = 1;

  ଯଦି(!digest->cnonce) {
    ଅକ୍ଷର cnoncebuf[12];
    result = Curl_rand_bytes(data,
#ifdef DEBUGBUILD
                             TRUE,
#endif
                             (unsigned ଅକ୍ଷର *)cnoncebuf,
                             ଆକାର(cnoncebuf));
    ଯଦି(!result)
      result = curlx_base64_encode((uint8_t *)cnoncebuf, ଆକାର(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    ଯଦି(result)
      ଯାଅ oom;

    digest->cnonce = cnonce;
  }

  ଯଦି(digest->userhash) {
    ଅକ୍ଷର *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    ଯଦି(!hasht) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ oom;
    }

    result = hash(hashbuf, (unsigned ଅକ୍ଷର *)hasht, strlen(hasht));
    curlx_free(hasht);
    ଯଦି(result)
      ଯାଅ oom;
    convert_to_ascii(hashbuf, (unsigned ଅକ୍ଷର *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  ଯଦି(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ oom;
  }

  result = hash(hashbuf, (unsigned ଅକ୍ଷର *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  ଯଦି(result)
    ଯାଅ oom;
  convert_to_ascii(hashbuf, ha1);

  ଯଦି(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    ଅକ୍ଷର *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    ଯଦି(!tmp) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ oom;
    }

    result = hash(hashbuf, (unsigned ଅକ୍ଷର *)tmp, strlen(tmp));
    curlx_free(tmp);
    ଯଦି(result)
      ଯାଅ oom;
    convert_to_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((ଅଚଳ ଅକ୍ଷର *)uripath);
  ଯଦି(!uri_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  ଯଦି(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ oom;
  }

  ଯଦି(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    ଅକ୍ଷର hashed[65];
    ଅକ୍ଷର *hashthis2;

    result = hash(hashbuf, (ଅଚଳ unsigned ଅକ୍ଷର *)"", 0);
    ଯଦି(result) {
      curlx_free(hashthis);
      ଯାଅ oom;
    }
    convert_to_ascii(hashbuf, (unsigned ଅକ୍ଷର *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    ଯଦି(!hashthis) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ oom;
    }
  }

  result = hash(hashbuf, (unsigned ଅକ୍ଷର *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  ଯଦି(result)
    ଯାଅ oom;
  convert_to_ascii(hashbuf, ha2);

  ଯଦି(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  ଅନ୍ୟଥା
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  ଯଦି(!hashthis) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ oom;
  }

  result = hash(hashbuf, (unsigned ଅକ୍ଷର *)hashthis, strlen(hashthis));
  curlx_free(hashthis);
  ଯଦି(result)
    ଯାଅ oom;
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
  ଯଦି(!userp_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ oom;
  }
  ଯଦି(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  ଅନ୍ୟଥା {
    realm_quoted = curlx_malloc(1);
    ଯଦି(realm_quoted)
      realm_quoted[0] = 0;
  }
  ଯଦି(!realm_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  ଯଦି(!nonce_quoted) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ oom;
  }

  ଯଦି(digest->qop) {
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
                            (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  ଅନ୍ୟଥା {
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
  ଯଦି(result)
    ଯାଅ oom;

  /* Add the optional fields */
  ଯଦି(digest->opaque) {
    /* Append the opaque */
    ଅକ୍ଷର *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    ଯଦି(!opaque_quoted) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ oom;
    }
    result = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    ଯଦି(result)
      ଯାଅ oom;
  }

  ଯଦି(digest->algorithm) {
    /* Append the algorithm */
    result = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    ଯଦି(result)
      ଯାଅ oom;
  }

  ଯଦି(digest->userhash) {
    /* Append the userhash */
    result = curlx_dyn_add(&response, ", userhash=true");
    ଯଦି(result)
      ଯାଅ oom;
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
  ଯଦି(result)
    curlx_dyn_free(&response);
  ଫେରନ୍ତୁ result;
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
CURLcode Curl_auth_create_digest_http_message(ସଂରଚନା Curl_easy *data,
                                              ସଂରଚନା Curl_creds *creds,
                                              ଅଚଳ unsigned ଅକ୍ଷର *request,
                                              ଅଚଳ unsigned ଅକ୍ଷର *uripath,
                                              ସଂରଚନା digestdata *digest,
                                              ଅକ୍ଷର **outptr, size_t *outlen)
{
  ଯଦି(digest->algo <= ALGO_MD5SESS)
    ଫେରନ୍ତୁ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_to_ascii,
                                           Curl_md5it);

  ଯଦି(digest->algo <= ALGO_SHA256SESS)
    ଫେରନ୍ତୁ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha256it);
#ifdef CURL_HAVE_SHA512_256
  ଯଦି(digest->algo <= ALGO_SHA512_256SESS)
    ଫେରନ୍ତୁ auth_create_digest_http_message(data, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_to_ascii,
                                           Curl_sha512_256it);
#endif /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;
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
ଶୂନ୍ୟ Curl_auth_digest_cleanup(ସଂରଚନା digestdata *digest)
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
