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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH

#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "vauth/digest.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "curl_md5.h"
#అన్తర్భావ "curl_sha256.h"
#అన్తర్భావ "curl_sha512_256.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "escape.h"

#నిర్వచితం_కాకపోతే USE_WINDOWS_SSPI
#నిర్వచించు SESSION_ALGO 1 /* for algos with this bit set */

#నిర్వచించు ALGO_MD5 0
#నిర్వచించు ALGO_MD5SESS (ALGO_MD5 | SESSION_ALGO)
#నిర్వచించు ALGO_SHA256 2
#నిర్వచించు ALGO_SHA256SESS (ALGO_SHA256 | SESSION_ALGO)
#నిర్వచించు ALGO_SHA512_256 4
#నిర్వచించు ALGO_SHA512_256SESS (ALGO_SHA512_256 | SESSION_ALGO)

#నిర్వచించు DIGEST_QOP_విలువ_AUTH             (1 << 0)
#నిర్వచించు DIGEST_QOP_విలువ_AUTH_INT         (1 << 1)
#నిర్వచించు DIGEST_QOP_విలువ_AUTH_CONF        (1 << 2)

#నిర్వచించు DIGEST_QOP_విలువ_STRING_AUTH      "auth"
#నిర్వచించు DIGEST_QOP_విలువ_STRING_AUTH_INT  "auth-int"
#నిర్వచించు DIGEST_QOP_విలువ_STRING_AUTH_CONF "auth-conf"
#అయితే_ముగింపు

bool Curl_auth_digest_get_pair(స్థిరరాశి అక్షరం *str, నిర్మాణం Curl_str *విలువ,
                               అక్షరం *content, స్థిరరాశి అక్షరం **endptr)
{
  పూర్ణాంకం c;
  size_t content_నిడివి = 0;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  ఒకవేళ(curlx_str_until(&str, విలువ, DIGEST_MAX_విలువ_నిడివి, '=') ||
     curlx_str_single(&str, '='))
    /* eek, no match */
    తిరిగి FALSE;

  ఒకవేళ('\"' == *str) {
    /* This starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  కొరకు(c = DIGEST_MAX_CONTENT_నిడివి; *str && c--; str++) {
    ఒకవేళ(!escape) {
      ఎంపిక(*str) {
      స్థితి '\\':
        ఒకవేళ(starts_with_quote) {
          /* the start of an escaped quote */
          escape = TRUE;
          కొనసాగు;
        }
        ఆపు;

      స్థితి ',':
        ఒకవేళ(!starts_with_quote) {
          /* This signals the end of the content if we did not get a starting
             quote and then we do "sloppy" parsing */
          c = 0; /* the end */
          కొనసాగు;
        }
        ఆపు;

      స్థితి '\r':
      స్థితి '\n':
        /* end of string */
        ఒకవేళ(starts_with_quote)
          తిరిగి FALSE; /* No closing quote */
        c = 0;
        కొనసాగు;

      స్థితి '\"':
        ఒకవేళ(starts_with_quote) {
          /* end of string */
          c = 0;
          కొనసాగు;
        }
        లేకపోతే
          తిరిగి FALSE;
      }
    }

    ఒకవేళ(content_నిడివి >= DIGEST_MAX_CONTENT_నిడివి - 1) {
      /* no room for this byte plus the terminator */
      తిరిగి FALSE;
    }

    escape = FALSE;
    *content++ = *str;
    content_నిడివి++;
  }

  ఒకవేళ(escape)
    తిరిగి FALSE; /* No character after backslash */

  *content = 0;
  *endptr = str;

  తిరిగి TRUE;
}

#నిర్వచితం_కాకపోతే USE_WINDOWS_SSPI
/* Convert MD5 chunk to RFC2617 (section 3.1.3) -suitable ASCII string */
స్థిర శూన్య auth_digest_md5_గమ్యం_ascii(
  స్థిరరాశి గుర్తులేని అక్షరం *ఆధారం, /* 16 bytes */
  గుర్తులేని అక్షరం *dest)         /* 33 bytes */
{
  పూర్ణాంకం i;
  కొరకు(i = 0; i < 16; i++)
    curl_msnprintf((అక్షరం *)&dest[i * 2], 3, "%02x", ఆధారం[i]);
}

/* Convert sha256 or SHA-512/256 chunk to RFC7616 -suitable ASCII string */
స్థిర శూన్య auth_digest_sha256_గమ్యం_ascii(
  స్థిరరాశి గుర్తులేని అక్షరం *ఆధారం, /* 32 bytes */
  గుర్తులేని అక్షరం *dest)         /* 65 bytes */
{
  పూర్ణాంకం i;
  కొరకు(i = 0; i < 32; i++)
    curl_msnprintf((అక్షరం *)&dest[i * 2], 3, "%02x", ఆధారం[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
స్థిర అక్షరం *auth_digest_string_quoted(స్థిరరాశి అక్షరం *s)
{
  నిర్మాణం dynbuf బహిర్గతం;
  curlx_dyn_init(&బహిర్గతం, 2048);
  ఒకవేళ(!*s) /* for zero length input, make sure we return an empty string */
    తిరిగి curlx_strdup("");
  అయితే(*s) {
    CURLcode ఫలితం;
    ఒకవేళ(*s == '"' || *s == '\\') {
      ఫలితం = curlx_dyn_addn(&బహిర్గతం, "\\", 1);
      ఒకవేళ(!ఫలితం)
        ఫలితం = curlx_dyn_addn(&బహిర్గతం, s, 1);
    }
    లేకపోతే ఒకవేళ((*s < ' ') || (*s > 0x7e)) {
      గుర్తులేని అక్షరం buf[3] = { '%' };
      Curl_hexbyte(&buf[1], (గుర్తులేని అక్షరం)*s);
      ఫలితం = curlx_dyn_addn(&బహిర్గతం, buf, 3);
    }
    లేకపోతే
      ఫలితం = curlx_dyn_addn(&బహిర్గతం, s, 1);
    ఒకవేళ(ఫలితం)
      తిరిగి ఖాళీ;
    s++;
  }
  తిరిగి curlx_dyn_ptr(&బహిర్గతం);
}

/* Retrieves the value for a corresponding key from the challenge string
 * returns TRUE if the key could be found, FALSE if it does not exist
 */
స్థిర bool auth_digest_get_కీలకం_విలువ(స్థిరరాశి అక్షరం *chlg, స్థిరరాశి అక్షరం *కీలకం,
                                      అక్షరం *buf, size_t buflen)
{
  /* keyword=[value],keyword2=[value]
     The values may or may not be quoted.
   */

  చేయి {
    నిర్మాణం Curl_str దత్తాంశం;
    నిర్మాణం Curl_str name;

    curlx_str_passblanks(&chlg);

    ఒకవేళ(!curlx_str_until(&chlg, &name, 64, '=') &&
       !curlx_str_single(&chlg, '=')) {
      /* this is the key, get the value, possibly quoted */
      పూర్ణాంకం rc = curlx_str_quotedword(&chlg, &దత్తాంశం, 256);
      ఒకవేళ(rc == STRE_BEGQUOTE)
        /* try unquoted until comma */
        rc = curlx_str_until(&chlg, &దత్తాంశం, 256, ',');
      ఒకవేళ(rc)
        తిరిగి FALSE; /* weird */

      ఒకవేళ(curlx_str_cmp(&name, కీలకం)) {
        /* if this is our key, return the value */
        size_t len = curlx_strlen(&దత్తాంశం);
        స్థిరరాశి అక్షరం *src = curlx_str(&దత్తాంశం);
        size_t i;
        size_t outlen = 0;

        ఒకవేళ(len >= buflen)
          /* does not fit */
          తిరిగి FALSE;

        కొరకు(i = 0; i < len; i++) {
          ఒకవేళ(src[i] == '\\' && i + 1 < len) {
            i++; /* skip backslash */
          }
          buf[outlen++] = src[i];
        }
        buf[outlen] = 0;
        తిరిగి TRUE;
      }
      ఒకవేళ(curlx_str_single(&chlg, ','))
        తిరిగి FALSE;
    }
    లేకపోతే /* odd syntax */
      ఆపు;
  } అయితే(1);

  తిరిగి FALSE;
}

స్థిర శూన్య auth_digest_get_qop_విలువలు(స్థిరరాశి అక్షరం *options, పూర్ణాంకం *విలువ)
{
  నిర్మాణం Curl_str బహిర్గతం;
  /* Initialize the output */
  *విలువ = 0;

  అయితే(!curlx_str_until(&options, &బహిర్గతం, 32, ',')) {
    ఒకవేళ(curlx_str_casecompare(&బహిర్గతం, DIGEST_QOP_విలువ_STRING_AUTH))
      *విలువ |= DIGEST_QOP_విలువ_AUTH;
    లేకపోతే ఒకవేళ(curlx_str_casecompare(&బహిర్గతం, DIGEST_QOP_విలువ_STRING_AUTH_INT))
      *విలువ |= DIGEST_QOP_విలువ_AUTH_INT;
    లేకపోతే ఒకవేళ(curlx_str_casecompare(&బహిర్గతం, DIGEST_QOP_విలువ_STRING_AUTH_CONF))
      *విలువ |= DIGEST_QOP_విలువ_AUTH_CONF;
    ఒకవేళ(curlx_str_single(&options, ','))
      ఆపు;
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
స్థిర CURLcode auth_decode_digest_md5_సందేశం(స్థిరరాశి నిర్మాణం bufref *chlgref,
                                               అక్షరం *nonce, size_t nlen,
                                               అక్షరం *realm, size_t rlen,
                                               అక్షరం *alg, size_t alen,
                                               అక్షరం *qop, size_t qlen)
{
  స్థిరరాశి అక్షరం *chlg = Curl_bufref_ptr(chlgref);

  /* Ensure we have a valid challenge message */
  ఒకవేళ(!Curl_bufref_len(chlgref))
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve nonce string from the challenge */
  ఒకవేళ(!auth_digest_get_కీలకం_విలువ(chlg, "nonce", nonce, nlen))
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve realm string from the challenge */
  ఒకవేళ(!auth_digest_get_కీలకం_విలువ(chlg, "realm", realm, rlen)) {
    /* Challenge does not have a realm, set empty string [RFC2831] page 6 */
    *realm = '\0';
  }

  /* Retrieve algorithm string from the challenge */
  ఒకవేళ(!auth_digest_get_కీలకం_విలువ(chlg, "algorithm", alg, alen))
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  /* Retrieve qop-options string from the challenge */
  ఒకవేళ(!auth_digest_get_కీలకం_విలువ(chlg, "qop", qop, qlen))
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  తిరిగి CURLE_OK;
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
bool Curl_auth_is_digest_supported(శూన్య)
{
  తిరిగి TRUE;
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
CURLcode Curl_auth_create_digest_md5_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             స్థిరరాశి నిర్మాణం bufref *chlg,
                                             నిర్మాణం Curl_creds *creds,
                                             స్థిరరాశి అక్షరం *default_service,
                                             నిర్మాణం bufref *బహిర్గతం)
{
  స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t i;
  నిర్మాణం MD5_context *ctxt;
  స్థిరరాశి అక్షరం *userp = Curl_creds_user(creds);
  స్థిరరాశి అక్షరం *passwdp = Curl_creds_passwd(creds);
  అక్షరం *response = ఖాళీ;
  గుర్తులేని అక్షరం digest[MD5_DIGEST_LEN];
  అక్షరం HA1_hex[(2 * MD5_DIGEST_LEN) + 1];
  అక్షరం HA2_hex[(2 * MD5_DIGEST_LEN) + 1];
  అక్షరం resp_hash_hex[(2 * MD5_DIGEST_LEN) + 1];
  అక్షరం nonce[64];
  అక్షరం realm[128];
  అక్షరం algorithm[64];
  అక్షరం qop_options[64];
  పూర్ణాంకం qop_విలువలు;
  అక్షరం cnonce[33];
  అక్షరం nonce_లెక్క[] = "00000001";
  అక్షరం పద్ధతి[]     = "AUTHENTICATE";
  అక్షరం qop[]        = DIGEST_QOP_విలువ_STRING_AUTH;
  అక్షరం *spn         = ఖాళీ;
  అక్షరం *qrealm;
  అక్షరం *qnonce;
  అక్షరం *quserp;
  అక్షరం *qspn;

  /* Decode the challenge message */
  CURLcode ఫలితం = auth_decode_digest_md5_సందేశం(chlg,
                                                   nonce, పరిమాణం(nonce),
                                                   realm, పరిమాణం(realm),
                                                   algorithm,
                                                   పరిమాణం(algorithm),
                                                   qop_options,
                                                   పరిమాణం(qop_options));
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* We only support md5 sessions */
  ఒకవేళ(strcmp(algorithm, "md5-sess"))
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  /* Get the qop-values from the qop-options */
  auth_digest_get_qop_విలువలు(qop_options, &qop_విలువలు);

  /* We only support auth quality-of-protection */
  ఒకవేళ(!(qop_విలువలు & DIGEST_QOP_విలువ_AUTH))
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  /* Generate 32 random hex chars, 32 bytes + 1 null-termination */
  ఫలితం = Curl_rand_hex(దత్తాంశం, (గుర్తులేని అక్షరం *)cnonce, పరిమాణం(cnonce));
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Good so far, now calculate A1 and H(A1) according to RFC 2831 */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ఒకవేళ(!ctxt)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)userp,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(userp)));
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)":", 1);
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)realm,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(realm)));
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)":", 1);
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)passwdp,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(passwdp)));
  Curl_MD5_final(ctxt, digest);

  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ఒకవేళ(!ctxt)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)digest, MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)":", 1);
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)nonce,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(nonce)));
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)":", 1);
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)cnonce,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(cnonce)));
  Curl_MD5_final(ctxt, digest);

  /* Convert calculated 16 octet hex into 32 bytes string */
  కొరకు(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA1_hex[2 * i], 3, "%02x", digest[i]);

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, దత్తాంశం->state.origin->hostname, ఖాళీ);
  ఒకవేళ(!spn)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Calculate H(A2) */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ఒకవేళ(!ctxt) {
    curlx_free(spn);

    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)పద్ధతి,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(పద్ధతి)));
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)":", 1);
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)spn,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(spn)));
  Curl_MD5_final(ctxt, digest);

  కొరకు(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&HA2_hex[2 * i], 3, "%02x", digest[i]);

  /* Now calculate the response hash */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ఒకవేళ(!ctxt) {
    curlx_free(spn);

    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)HA1_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)":", 1);
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)nonce,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(nonce)));
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)":", 1);

  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)nonce_లెక్క,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(nonce_లెక్క)));
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)":", 1);
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)cnonce,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(cnonce)));
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)":", 1);
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)qop,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(qop)));
  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)":", 1);

  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)HA2_hex, 2 * MD5_DIGEST_LEN);
  Curl_MD5_final(ctxt, digest);

  కొరకు(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&resp_hash_hex[2 * i], 3, "%02x", digest[i]);

  /* escape double quotes and backslashes in the username, realm, nonce and
     spn as necessary */
  qrealm = auth_digest_string_quoted(realm);
  qnonce = auth_digest_string_quoted(nonce);
  quserp = auth_digest_string_quoted(userp);
  qspn = auth_digest_string_quoted(spn);
  ఒకవేళ(qrealm && qnonce && quserp && qspn)
    /* Generate the response */
    response = curl_maprintf("username=\"%s\",realm=\"%s\",nonce=\"%s\","
                             "cnonce=\"%s\",nc=\"%s\",digest-uri=\"%s\","
                             "response=%s,qop=%s",
                             quserp, qrealm, qnonce,
                             cnonce, nonce_లెక్క, qspn, resp_hash_hex, qop);

  curlx_free(qrealm);
  curlx_free(qnonce);
  curlx_free(quserp);
  curlx_free(spn);
  curlx_free(qspn);
  ఒకవేళ(!response)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Return the response. */
  Curl_bufref_సమితి(బహిర్గతం, response, అక్షరశ్రేణి_పొడవు(response), curl_free);
  తిరిగి ఫలితం;
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
CURLcode Curl_auth_decode_digest_http_సందేశం(స్థిరరాశి అక్షరం *chlg,
                                              నిర్మాణం digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */

  /* If we already have received a nonce, keep that in mind */
  ఒకవేళ(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_digest_cleanup(digest);

  కొరకు(;;) {
    నిర్మాణం Curl_str విలువ;
    అక్షరం content[DIGEST_MAX_CONTENT_నిడివి];

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Extract a value=content pair */
    ఒకవేళ(Curl_auth_digest_get_pair(chlg, &విలువ, content, &chlg)) {
      ఒకవేళ(curlx_str_casecompare(&విలువ, "nonce")) {
        curlx_free(digest->nonce);
        digest->nonce = curlx_strdup(content);
        ఒకవేళ(!digest->nonce)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      లేకపోతే ఒకవేళ(curlx_str_casecompare(&విలువ, "stale")) {
        ఒకవేళ(curl_strequal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      లేకపోతే ఒకవేళ(curlx_str_casecompare(&విలువ, "realm")) {
        curlx_free(digest->realm);
        digest->realm = curlx_strdup(content);
        ఒకవేళ(!digest->realm)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      లేకపోతే ఒకవేళ(curlx_str_casecompare(&విలువ, "opaque")) {
        curlx_free(digest->opaque);
        digest->opaque = curlx_strdup(content);
        ఒకవేళ(!digest->opaque)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      లేకపోతే ఒకవేళ(curlx_str_casecompare(&విలువ, "qop")) {
        స్థిరరాశి అక్షరం *token = content;
        నిర్మాణం Curl_str బహిర్గతం;
        bool దొరికింది_Auth = FALSE;
        bool దొరికింది_Auth_Int = FALSE;
        /* Pass leading spaces */
        curlx_str_passblanks(&token);

        అయితే(!curlx_str_until(&token, &బహిర్గతం, 32, ',')) {
          ఒకవేళ(curlx_str_casecompare(&బహిర్గతం, DIGEST_QOP_విలువ_STRING_AUTH))
            దొరికింది_Auth = TRUE;
          లేకపోతే ఒకవేళ(curlx_str_casecompare(&బహిర్గతం,
                                        DIGEST_QOP_విలువ_STRING_AUTH_INT))
            దొరికింది_Auth_Int = TRUE;
          ఒకవేళ(curlx_str_single(&token, ','))
            ఆపు;
          curlx_str_passblanks(&token);
        }

        /* Select only auth or auth-int. Otherwise, ignore */
        ఒకవేళ(దొరికింది_Auth) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_విలువ_STRING_AUTH);
          ఒకవేళ(!digest->qop)
            తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        }
        లేకపోతే ఒకవేళ(దొరికింది_Auth_Int) {
          curlx_free(digest->qop);
          digest->qop = curlx_strdup(DIGEST_QOP_విలువ_STRING_AUTH_INT);
          ఒకవేళ(!digest->qop)
            తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        }
      }
      లేకపోతే ఒకవేళ(curlx_str_casecompare(&విలువ, "algorithm")) {
        curlx_free(digest->algorithm);
        digest->algorithm = curlx_strdup(content);
        ఒకవేళ(!digest->algorithm)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

        ఒకవేళ(curl_strequal(content, "MD5-sess"))
          digest->algo = ALGO_MD5SESS;
        లేకపోతే ఒకవేళ(curl_strequal(content, "MD5"))
          digest->algo = ALGO_MD5;
        లేకపోతే ఒకవేళ(curl_strequal(content, "SHA-256"))
          digest->algo = ALGO_SHA256;
        లేకపోతే ఒకవేళ(curl_strequal(content, "SHA-256-SESS"))
          digest->algo = ALGO_SHA256SESS;
        లేకపోతే ఒకవేళ(curl_strequal(content, "SHA-512-256")) {
#నిర్వచితమైతే CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256;
#లేకపోతే /* !CURL_HAVE_SHA512_256 */
          తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు /* CURL_HAVE_SHA512_256 */
        }
        లేకపోతే ఒకవేళ(curl_strequal(content, "SHA-512-256-SESS")) {
#నిర్వచితమైతే CURL_HAVE_SHA512_256
          digest->algo = ALGO_SHA512_256SESS;
#లేకపోతే /* !CURL_HAVE_SHA512_256 */
          తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు /* CURL_HAVE_SHA512_256 */
        }
        లేకపోతే
          తిరిగి CURLE_BAD_CONTENT_ENCODING;
      }
      లేకపోతే ఒకవేళ(curlx_str_casecompare(&విలువ, "userhash")) {
        ఒకవేళ(curl_strequal(content, "true")) {
          digest->userhash = TRUE;
        }
      }
      లేకపోతే {
        /* Unknown specifier, ignore it! */
      }
    }
    లేకపోతే
      ఆపు; /* We are done here */

    /* Pass all additional spaces here */
    curlx_str_passblanks(&chlg);

    /* Allow the list to be comma-separated */
    (శూన్య)curlx_str_single(&chlg, ',');
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  ఒకవేళ(before && !digest->stale)
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that is a bad Digest line! */
  ఒకవేళ(!digest->nonce)
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  /* "<algo>-sess" protocol versions require "auth" or "auth-int" qop */
  ఒకవేళ(!digest->qop && (digest->algo & SESSION_ALGO))
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  తిరిగి CURLE_OK;
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
స్థిర CURLcode auth_create_digest_http_సందేశం(
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం Curl_creds *creds,
  స్థిరరాశి గుర్తులేని అక్షరం *request,
  స్థిరరాశి గుర్తులేని అక్షరం *uripath,
  నిర్మాణం digestdata *digest,
  అక్షరం **outptr, size_t *outlen,
  శూన్య (*convert_గమ్యం_ascii)(స్థిరరాశి గుర్తులేని అక్షరం *, గుర్తులేని అక్షరం *),
  CURLcode (*hash)(గుర్తులేని అక్షరం *, స్థిరరాశి గుర్తులేని అక్షరం *, స్థిరరాశి size_t))
{
  CURLcode ఫలితం;
  స్థిరరాశి అక్షరం *userp = Curl_creds_user(creds);
  స్థిరరాశి అక్షరం *passwdp = Curl_creds_passwd(creds);
  గుర్తులేని అక్షరం hashbuf[32]; /* 32 bytes/256 bits */
  గుర్తులేని అక్షరం request_digest[65];
  గుర్తులేని అక్షరం ha1[65];    /* 64 digits and 1 zero byte */
  గుర్తులేని అక్షరం ha2[65];    /* 64 digits and 1 zero byte */
  అక్షరం userh[65];
  అక్షరం *cnonce = ఖాళీ;
  size_t cnonce_sz = 0;
  అక్షరం *userp_quoted = ఖాళీ;
  అక్షరం *realm_quoted = ఖాళీ;
  అక్షరం *nonce_quoted = ఖాళీ;
  అక్షరం *hashthis = ఖాళీ;
  అక్షరం *uri_quoted = ఖాళీ;
  నిర్మాణం dynbuf response;
  *outptr = ఖాళీ;

  curlx_dyn_init(&response, 4096); /* arbitrary max */

  memset(hashbuf, 0, పరిమాణం(hashbuf));
  ఒకవేళ(!digest->nc)
    digest->nc = 1;

  ఒకవేళ(!digest->cnonce) {
    అక్షరం cnoncebuf[12];
    ఫలితం = Curl_rand_బైట్లు(దత్తాంశం,
#నిర్వచితమైతే DEBUGBUILD
                             TRUE,
#అయితే_ముగింపు
                             (గుర్తులేని అక్షరం *)cnoncebuf,
                             పరిమాణం(cnoncebuf));
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_base64_encode((uint8_t *)cnoncebuf, పరిమాణం(cnoncebuf),
                                   &cnonce, &cnonce_sz);
    ఒకవేళ(ఫలితం)
      వెళ్ళు oom;

    digest->cnonce = cnonce;
  }

  ఒకవేళ(digest->userhash) {
    అక్షరం *hasht = curl_maprintf("%s:%s", userp,
                                digest->realm ? digest->realm : "");
    ఒకవేళ(!hasht) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు oom;
    }

    ఫలితం = hash(hashbuf, (గుర్తులేని అక్షరం *)hasht, అక్షరశ్రేణి_పొడవు(hasht));
    curlx_free(hasht);
    ఒకవేళ(ఫలితం)
      వెళ్ళు oom;
    convert_గమ్యం_ascii(hashbuf, (గుర్తులేని అక్షరం *)userh);
  }

  /* If the algorithm is "MD5" or unspecified (which then defaults to MD5):

       A1 = unq(username-value) ":" unq(realm-value) ":" passwd

     If the algorithm is "MD5-sess" then:

       A1 = H(unq(username-value) ":" unq(realm-value) ":" passwd) ":"
            unq(nonce-value) ":" unq(cnonce-value)
   */

  hashthis = curl_maprintf("%s:%s:%s", userp, digest->realm ?
                           digest->realm : "", passwdp);
  ఒకవేళ(!hashthis) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు oom;
  }

  ఫలితం = hash(hashbuf, (గుర్తులేని అక్షరం *)hashthis, అక్షరశ్రేణి_పొడవు(hashthis));
  curlx_free(hashthis);
  ఒకవేళ(ఫలితం)
    వెళ్ళు oom;
  convert_గమ్యం_ascii(hashbuf, ha1);

  ఒకవేళ(digest->algo & SESSION_ALGO) {
    /* nonce and cnonce are OUTSIDE the hash */
    అక్షరం *tmp = curl_maprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    ఒకవేళ(!tmp) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు oom;
    }

    ఫలితం = hash(hashbuf, (గుర్తులేని అక్షరం *)tmp, అక్షరశ్రేణి_పొడవు(tmp));
    curlx_free(tmp);
    ఒకవేళ(ఫలితం)
      వెళ్ళు oom;
    convert_గమ్యం_ascii(hashbuf, ha1);
  }

  /* If the "qop" directive's value is "auth" or is unspecified, then A2 is:

       A2 = Method ":" digest-uri-value

     If the "qop" value is "auth-int", then A2 is:

       A2 = Method ":" digest-uri-value ":" H(entity-body)

     (The "Method" value is the HTTP request method as specified in section
     5.1.1 of RFC 2616)
   */

  uri_quoted = auth_digest_string_quoted((స్థిరరాశి అక్షరం *)uripath);
  ఒకవేళ(!uri_quoted) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు oom;
  }

  hashthis = curl_maprintf("%s:%s", request, uripath);
  ఒకవేళ(!hashthis) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు oom;
  }

  ఒకవేళ(digest->qop && curl_strequal(digest->qop, "auth-int")) {
    /* We do not support auth-int for PUT or POST */
    అక్షరం hashed[65];
    అక్షరం *hashthis2;

    ఫలితం = hash(hashbuf, (స్థిరరాశి గుర్తులేని అక్షరం *)"", 0);
    ఒకవేళ(ఫలితం) {
      curlx_free(hashthis);
      వెళ్ళు oom;
    }
    convert_గమ్యం_ascii(hashbuf, (గుర్తులేని అక్షరం *)hashed);

    hashthis2 = curl_maprintf("%s:%s", hashthis, hashed);
    curlx_free(hashthis);
    hashthis = hashthis2;
    ఒకవేళ(!hashthis) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు oom;
    }
  }

  ఫలితం = hash(hashbuf, (గుర్తులేని అక్షరం *)hashthis, అక్షరశ్రేణి_పొడవు(hashthis));
  curlx_free(hashthis);
  ఒకవేళ(ఫలితం)
    వెళ్ళు oom;
  convert_గమ్యం_ascii(hashbuf, ha2);

  ఒకవేళ(digest->qop)
    hashthis = curl_maprintf("%s:%s:%08x:%s:%s:%s", ha1, digest->nonce,
                             (గుర్తులేని పూర్ణాంకం)digest->nc, digest->cnonce,
                             digest->qop, ha2);
  లేకపోతే
    hashthis = curl_maprintf("%s:%s:%s", ha1, digest->nonce, ha2);

  ఒకవేళ(!hashthis) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు oom;
  }

  ఫలితం = hash(hashbuf, (గుర్తులేని అక్షరం *)hashthis, అక్షరశ్రేణి_పొడవు(hashthis));
  curlx_free(hashthis);
  ఒకవేళ(ఫలితం)
    వెళ్ళు oom;
  convert_గమ్యం_ascii(hashbuf, request_digest);

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
  ఒకవేళ(!userp_quoted) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు oom;
  }
  ఒకవేళ(digest->realm)
    realm_quoted = auth_digest_string_quoted(digest->realm);
  లేకపోతే {
    realm_quoted = curlx_malloc(1);
    ఒకవేళ(realm_quoted)
      realm_quoted[0] = 0;
  }
  ఒకవేళ(!realm_quoted) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు oom;
  }

  nonce_quoted = auth_digest_string_quoted(digest->nonce);
  ఒకవేళ(!nonce_quoted) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు oom;
  }

  ఒకవేళ(digest->qop) {
    ఫలితం = curlx_dyn_addf(&response, "username=\"%s\", "
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
                            (గుర్తులేని పూర్ణాంకం)digest->nc,
                            digest->qop,
                            request_digest);

    /* Increment nonce-count to use another nc value for the next request */
    digest->nc++;
  }
  లేకపోతే {
    ఫలితం = curlx_dyn_addf(&response, "username=\"%s\", "
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
  ఒకవేళ(ఫలితం)
    వెళ్ళు oom;

  /* Add the optional fields */
  ఒకవేళ(digest->opaque) {
    /* Append the opaque */
    అక్షరం *opaque_quoted = auth_digest_string_quoted(digest->opaque);
    ఒకవేళ(!opaque_quoted) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు oom;
    }
    ఫలితం = curlx_dyn_addf(&response, ", opaque=\"%s\"", opaque_quoted);
    curlx_free(opaque_quoted);
    ఒకవేళ(ఫలితం)
      వెళ్ళు oom;
  }

  ఒకవేళ(digest->algorithm) {
    /* Append the algorithm */
    ఫలితం = curlx_dyn_addf(&response, ", algorithm=%s", digest->algorithm);
    ఒకవేళ(ఫలితం)
      వెళ్ళు oom;
  }

  ఒకవేళ(digest->userhash) {
    /* Append the userhash */
    ఫలితం = curlx_dyn_కూడు(&response, ", userhash=true");
    ఒకవేళ(ఫలితం)
      వెళ్ళు oom;
  }

  /* Return the output */
  *outptr = curlx_dyn_ptr(&response);
  *outlen = curlx_dyn_len(&response);
  ఫలితం = CURLE_OK;

oom:
  curlx_free(nonce_quoted);
  curlx_free(realm_quoted);
  curlx_free(uri_quoted);
  curlx_free(userp_quoted);
  ఒకవేళ(ఫలితం)
    curlx_dyn_free(&response);
  తిరిగి ఫలితం;
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
CURLcode Curl_auth_create_digest_http_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                              నిర్మాణం Curl_creds *creds,
                                              స్థిరరాశి గుర్తులేని అక్షరం *request,
                                              స్థిరరాశి గుర్తులేని అక్షరం *uripath,
                                              నిర్మాణం digestdata *digest,
                                              అక్షరం **outptr, size_t *outlen)
{
  ఒకవేళ(digest->algo <= ALGO_MD5SESS)
    తిరిగి auth_create_digest_http_సందేశం(దత్తాంశం, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_md5_గమ్యం_ascii,
                                           Curl_md5it);

  ఒకవేళ(digest->algo <= ALGO_SHA256SESS)
    తిరిగి auth_create_digest_http_సందేశం(దత్తాంశం, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_గమ్యం_ascii,
                                           Curl_sha256it);
#నిర్వచితమైతే CURL_HAVE_SHA512_256
  ఒకవేళ(digest->algo <= ALGO_SHA512_256SESS)
    తిరిగి auth_create_digest_http_సందేశం(దత్తాంశం, creds,
                                           request, uripath, digest,
                                           outptr, outlen,
                                           auth_digest_sha256_గమ్యం_ascii,
                                           Curl_sha512_256it);
#అయితే_ముగింపు /* CURL_HAVE_SHA512_256 */

  /* Should be unreachable */
  తిరిగి CURLE_BAD_CONTENT_ENCODING;
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
శూన్య Curl_auth_digest_cleanup(నిర్మాణం digestdata *digest)
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
#అయితే_ముగింపు /* !USE_WINDOWS_SSPI */

#అయితే_ముగింపు /* !CURL_DISABLE_DIGEST_AUTH */
