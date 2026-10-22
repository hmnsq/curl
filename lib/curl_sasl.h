#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SASL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SASL_H
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
 ***************************************************************************/
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"

ਬਣਤਰ Curl_easy;
ਬਣਤਰ connectdata;

/* Authentication mechanism flags */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_LOGIN             (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_PLAIN             (1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_CRAM_MD5          (1 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_DIGEST_MD5        (1 << 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_GSSAPI            (1 << 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_EXTERNAL          (1 << 5)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_NTLM              (1 << 6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_XOAUTH2           (1 << 7)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_OAUTHBEARER       (1 << 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_SCRAM_SHA_1       (1 << 9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_SCRAM_SHA_256     (1 << 10)

/* Authentication mechanism values */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_AUTH_NONE          0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_AUTH_ANY           0xffff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_AUTH_DEFAULT       (SASL_AUTH_ANY & ~SASL_MECH_EXTERNAL)

/* Authentication mechanism strings */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_STRING_LOGIN          "LOGIN"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_STRING_PLAIN          "PLAIN"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_STRING_CRAM_MD5       "CRAM-MD5"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_STRING_DIGEST_MD5     "DIGEST-MD5"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_STRING_GSSAPI         "GSSAPI"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_STRING_EXTERNAL       "EXTERNAL"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_STRING_NTLM           "NTLM"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_STRING_XOAUTH2        "XOAUTH2"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_STRING_OAUTHBEARER    "OAUTHBEARER"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_STRING_SCRAM_SHA_1    "SCRAM-SHA-1"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_MECH_STRING_SCRAM_SHA_256  "SCRAM-SHA-256"

/* SASL flags */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SASL_FLAG_BASE64        0x0001  /* Messages are base64-encoded */

/* SASL machine states */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  SASL_STOP,
  SASL_PLAIN,
  SASL_LOGIN,
  SASL_LOGIN_PASSWD,
  SASL_EXTERNAL,
  SASL_CRAMMD5,
  SASL_DIGESTMD5,
  SASL_DIGESTMD5_RESP,
  SASL_NTLM,
  SASL_NTLM_TYPE2_ਸੁਨੇਹਾ,
  SASL_GSSAPI,
  SASL_GSSAPI_TOKEN,
  SASL_GSSAPI_NO_ਅੰਕੜੇ,
  SASL_OAUTH2,
  SASL_OAUTH2_RESP,
  SASL_GSASL,
  SASL_CANCEL,
  SASL_FINAL
} saslstate;

/* Progress indicator */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  SASL_IDLE,
  SASL_INPROGRESS,
  SASL_ਪੂਰਾ
} saslprogress;

/* Protocol dependent SASL parameters */
ਬਣਤਰ SASLproto {
  ਅਚਲ ਅੱਖਰ *service;     /* The service name */
  CURLcode (*sendauth)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *mech,
                       ਅਚਲ ਬਣਤਰ bufref *ir);
                           /* Send authentication command */
  CURLcode (*contauth)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *mech,
                       ਅਚਲ ਬਣਤਰ bufref *contauth);
                           /* Send authentication continuation */
  CURLcode (*cancelauth)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *mech);
                           /* Cancel authentication. */
  CURLcode (*getmessage)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ bufref *ਬਾਹਰਲਾ);
                           /* Get SASL response message */
  size_t maxirlen;         /* Maximum initial response + mechanism length,
                              or zero if no max. This is normally the max
                              command length - other characters count.
                              This has to be zero for non-base64 protocols. */
  ਪੂਰਨ contcode;            /* Code to receive when continuation is expected */
  ਪੂਰਨ finalcode;           /* Code to receive upon authentication success */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ defmechs; /* Mechanisms enabled by default */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ flags;    /* Configuration flags. */
};

/* Per-connection parameters */
ਬਣਤਰ SASL {
  ਅਚਲ ਬਣਤਰ SASLproto *params; /* Protocol dependent parameters */
  saslstate state;           /* Current machine state */
  ਅਚਲ ਅੱਖਰ *curmech;       /* Current mechanism id. */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ authmechs;  /* Accepted authentication mechanisms */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ prefmech;   /* Preferred authentication mechanism */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ authused;   /* Auth mechanism used for the connection */
  ਦੋਆਧਾਰੀ_ਅੰਕ(resetprefs);           /* For URL auth option parsing. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(mutual_auth);          /* Mutual authentication enabled (GSSAPI only) */
  ਦੋਆਧਾਰੀ_ਅੰਕ(force_ir);             /* Protocol always supports initial response */
};

/* This is used to test whether the line starts with the given mechanism */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sasl_mech_equal(ਸਤਰ, wordlen, mech) \
  ((wordlen) == (ਆਕਾਰ(mech) - 1) / ਆਕਾਰ(ਅੱਖਰ) && \
   !memcmp(ਸਤਰ, mech, wordlen))

/* Convert a mechanism name to a token */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ Curl_sasl_decode_mech(ਅਚਲ ਅੱਖਰ *ptr,
                                     size_t maxlen, size_t *len);

/* Parse the URL login options */
CURLcode Curl_sasl_parse_url_auth_option(ਬਣਤਰ SASL *sasl,
                                         ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, size_t len);

/* Initializes an SASL structure */
ਖਾਲੀ Curl_sasl_init(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                    ਅਚਲ ਬਣਤਰ SASLproto *params);

/* Check if we have enough auth data and capabilities to authenticate */
bool Curl_sasl_can_authenticate(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* Calculate the required login details for SASL authentication */
CURLcode Curl_sasl_ਸ਼ੁਰੂ(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         bool force_ir, saslprogress *ਤਰੱਕੀ);

/* Continue an SASL authentication */
CURLcode Curl_sasl_continue(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਪੂਰਨ ਕੋਡ, saslprogress *ਤਰੱਕੀ);

CURLcode Curl_sasl_is_blocked(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

#ਜੇ_ਅੰਤ /* HEADER_CURL_SASL_H */
