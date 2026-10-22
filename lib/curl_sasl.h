#ifndef HEADER_CURL_SASL_H
#define HEADER_CURL_SASL_H
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
#ಅನ್ತರ್ಭಾವ "bufref.h"

ಸಂರಚನಾ Curl_easy;
ಸಂರಚನಾ connectdata;

/* Authentication mechanism flags */
#define SASL_MECH_LOGIN             (1 << 0)
#define SASL_MECH_PLAIN             (1 << 1)
#define SASL_MECH_CRAM_MD5          (1 << 2)
#define SASL_MECH_DIGEST_MD5        (1 << 3)
#define SASL_MECH_GSSAPI            (1 << 4)
#define SASL_MECH_EXTERNAL          (1 << 5)
#define SASL_MECH_NTLM              (1 << 6)
#define SASL_MECH_XOAUTH2           (1 << 7)
#define SASL_MECH_OAUTHBEARER       (1 << 8)
#define SASL_MECH_SCRAM_SHA_1       (1 << 9)
#define SASL_MECH_SCRAM_SHA_256     (1 << 10)

/* Authentication mechanism values */
#define SASL_AUTH_NONE          0
#define SASL_AUTH_ANY           0xffff
#define SASL_AUTH_DEFAULT       (SASL_AUTH_ANY & ~SASL_MECH_EXTERNAL)

/* Authentication mechanism strings */
#define SASL_MECH_STRING_LOGIN          "LOGIN"
#define SASL_MECH_STRING_PLAIN          "PLAIN"
#define SASL_MECH_STRING_CRAM_MD5       "CRAM-MD5"
#define SASL_MECH_STRING_DIGEST_MD5     "DIGEST-MD5"
#define SASL_MECH_STRING_GSSAPI         "GSSAPI"
#define SASL_MECH_STRING_EXTERNAL       "EXTERNAL"
#define SASL_MECH_STRING_NTLM           "NTLM"
#define SASL_MECH_STRING_XOAUTH2        "XOAUTH2"
#define SASL_MECH_STRING_OAUTHBEARER    "OAUTHBEARER"
#define SASL_MECH_STRING_SCRAM_SHA_1    "SCRAM-SHA-1"
#define SASL_MECH_STRING_SCRAM_SHA_256  "SCRAM-SHA-256"

/* SASL flags */
#define SASL_FLAG_BASE64        0x0001  /* Messages are base64-encoded */

/* SASL machine states */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  SASL_STOP,
  SASL_PLAIN,
  SASL_LOGIN,
  SASL_LOGIN_PASSWD,
  SASL_EXTERNAL,
  SASL_CRAMMD5,
  SASL_DIGESTMD5,
  SASL_DIGESTMD5_RESP,
  SASL_NTLM,
  SASL_NTLM_TYPE2_ಸಂದೇಶ,
  SASL_GSSAPI,
  SASL_GSSAPI_TOKEN,
  SASL_GSSAPI_NO_DATA,
  SASL_OAUTH2,
  SASL_OAUTH2_RESP,
  SASL_GSASL,
  SASL_CANCEL,
  SASL_FINAL
} saslstate;

/* Progress indicator */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  SASL_IDLE,
  SASL_INPROGRESS,
  SASL_DONE
} saslprogress;

/* Protocol dependent SASL parameters */
ಸಂರಚನಾ SASLproto {
  ಧ್ರುವ ವರ್ಣ *service;     /* The service name */
  CURLcode (*sendauth)(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *mech,
                       ಧ್ರುವ ಸಂರಚನಾ bufref *ir);
                           /* Send authentication command */
  CURLcode (*contauth)(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *mech,
                       ಧ್ರುವ ಸಂರಚನಾ bufref *contauth);
                           /* Send authentication continuation */
  CURLcode (*cancelauth)(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *mech);
                           /* Cancel authentication. */
  CURLcode (*getmessage)(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ bufref *out);
                           /* Get SASL response message */
  size_t maxirlen;         /* Maximum initial response + mechanism length,
                              or zero if no max. This is normally the max
                              command length - other characters count.
                              This has to be zero for non-base64 protocols. */
  ಪೂರ್ಣಾಙ್ಕ contcode;            /* Code to receive when continuation is expected */
  ಪೂರ್ಣಾಙ್ಕ finalcode;           /* Code to receive upon authentication success */
  unsigned short defmechs; /* Mechanisms enabled by default */
  unsigned short flags;    /* Configuration flags. */
};

/* Per-connection parameters */
ಸಂರಚನಾ SASL {
  ಧ್ರುವ ಸಂರಚನಾ SASLproto *params; /* Protocol dependent parameters */
  saslstate state;           /* Current machine state */
  ಧ್ರುವ ವರ್ಣ *curmech;       /* Current mechanism id. */
  unsigned short authmechs;  /* Accepted authentication mechanisms */
  unsigned short prefmech;   /* Preferred authentication mechanism */
  unsigned short authused;   /* Auth mechanism used for the connection */
  BIT(resetprefs);           /* For URL auth option parsing. */
  BIT(mutual_auth);          /* Mutual authentication enabled (GSSAPI only) */
  BIT(force_ir);             /* Protocol always supports initial response */
};

/* This is used to test whether the line starts with the given mechanism */
#define sasl_mech_equal(ಸಾಲು, wordlen, mech) \
  ((wordlen) == (ಪರಿಮಾಣ(mech) - 1) / ಪರಿಮಾಣ(ವರ್ಣ) && \
   !memcmp(ಸಾಲು, mech, wordlen))

/* Convert a mechanism name to a token */
unsigned short Curl_sasl_decode_mech(ಧ್ರುವ ವರ್ಣ *ptr,
                                     size_t maxlen, size_t *len);

/* Parse the URL login options */
CURLcode Curl_sasl_parse_url_auth_option(ಸಂರಚನಾ SASL *sasl,
                                         ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, size_t len);

/* Initializes an SASL structure */
ಶೂನ್ಯ Curl_sasl_init(ಸಂರಚನಾ SASL *sasl, ಸಂರಚನಾ Curl_easy *data,
                    ಧ್ರುವ ಸಂರಚನಾ SASLproto *params);

/* Check if we have enough auth data and capabilities to authenticate */
bool Curl_sasl_can_authenticate(ಸಂರಚನಾ SASL *sasl, ಸಂರಚನಾ Curl_easy *data);

/* Calculate the required login details for SASL authentication */
CURLcode Curl_sasl_start(ಸಂರಚನಾ SASL *sasl, ಸಂರಚನಾ Curl_easy *data,
                         bool force_ir, saslprogress *progress);

/* Continue an SASL authentication */
CURLcode Curl_sasl_continue(ಸಂರಚನಾ SASL *sasl, ಸಂರಚನಾ Curl_easy *data,
                            ಪೂರ್ಣಾಙ್ಕ code, saslprogress *progress);

CURLcode Curl_sasl_is_blocked(ಸಂರಚನಾ SASL *sasl, ಸಂರಚನಾ Curl_easy *data);

#endif /* HEADER_CURL_SASL_H */
