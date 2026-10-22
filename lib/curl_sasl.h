#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SASL_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SASL_H
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

ರಚನೆ Curl_easy;
ರಚನೆ connectdata;

/* Authentication mechanism flags */
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_LOGIN             (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_PLAIN             (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_CRAM_MD5          (1 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_DIGEST_MD5        (1 << 3)
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_GSSAPI            (1 << 4)
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_EXTERNAL          (1 << 5)
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_NTLM              (1 << 6)
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_XOAUTH2           (1 << 7)
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_OAUTHBEARER       (1 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_SCRAM_SHA_1       (1 << 9)
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_SCRAM_SHA_256     (1 << 10)

/* Authentication mechanism values */
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_AUTH_NONE          0
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_AUTH_ANY           0xffff
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_AUTH_DEFAULT       (SASL_AUTH_ANY & ~SASL_MECH_EXTERNAL)

/* Authentication mechanism strings */
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_STRING_LOGIN          "LOGIN"
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_STRING_PLAIN          "PLAIN"
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_STRING_CRAM_MD5       "CRAM-MD5"
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_STRING_DIGEST_MD5     "DIGEST-MD5"
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_STRING_GSSAPI         "GSSAPI"
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_STRING_EXTERNAL       "EXTERNAL"
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_STRING_NTLM           "NTLM"
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_STRING_XOAUTH2        "XOAUTH2"
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_STRING_OAUTHBEARER    "OAUTHBEARER"
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_STRING_SCRAM_SHA_1    "SCRAM-SHA-1"
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_MECH_STRING_SCRAM_SHA_256  "SCRAM-SHA-256"

/* SASL flags */
#ವ್ಯಾಖ್ಯಾನಿಸು SASL_FLAG_BASE64        0x0001  /* Messages are base64-encoded */

/* SASL machine states */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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
  SASL_GSSAPI_NO_ದತ್ತಾಂಶ,
  SASL_OAUTH2,
  SASL_OAUTH2_RESP,
  SASL_GSASL,
  SASL_CANCEL,
  SASL_FINAL
} saslstate;

/* Progress indicator */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  SASL_IDLE,
  SASL_INPROGRESS,
  SASL_ಮುಗಿದ
} saslprogress;

/* Protocol dependent SASL parameters */
ರಚನೆ SASLproto {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service;     /* The service name */
  CURLcode (*sendauth)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                       ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *ir);
                           /* Send authentication command */
  CURLcode (*contauth)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                       ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *contauth);
                           /* Send authentication continuation */
  CURLcode (*cancelauth)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech);
                           /* Cancel authentication. */
  CURLcode (*getmessage)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ bufref *ಹೊರಹೋಗು);
                           /* Get SASL response message */
  size_t maxirlen;         /* Maximum initial response + mechanism length,
                              or zero if no max. This is normally the max
                              command length - other characters count.
                              This has to be zero for non-base64 protocols. */
  ಪೂರ್ಣಾಂಕ contcode;            /* Code to receive when continuation is expected */
  ಪೂರ್ಣಾಂಕ finalcode;           /* Code to receive upon authentication success */
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ defmechs; /* Mechanisms enabled by default */
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ flags;    /* Configuration flags. */
};

/* Per-connection parameters */
ರಚನೆ SASL {
  ಸ್ಥಿರಾಂಕ ರಚನೆ SASLproto *params; /* Protocol dependent parameters */
  saslstate state;           /* Current machine state */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *curmech;       /* Current mechanism id. */
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ authmechs;  /* Accepted authentication mechanisms */
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ prefmech;   /* Preferred authentication mechanism */
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ authused;   /* Auth mechanism used for the connection */
  ಬಿಟ್(resetprefs);           /* For URL auth option parsing. */
  ಬಿಟ್(mutual_auth);          /* Mutual authentication enabled (GSSAPI only) */
  ಬಿಟ್(force_ir);             /* Protocol always supports initial response */
};

/* This is used to test whether the line starts with the given mechanism */
#ವ್ಯಾಖ್ಯಾನಿಸು sasl_mech_equal(ಸಾಲು, wordlen, mech) \
  ((wordlen) == (ಗಾತ್ರ(mech) - 1) / ಗಾತ್ರ(ಅಕ್ಷರ) && \
   !memcmp(ಸಾಲು, mech, wordlen))

/* Convert a mechanism name to a token */
ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ Curl_sasl_decode_mech(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr,
                                     size_t maxlen, size_t *len);

/* Parse the URL login options */
CURLcode Curl_sasl_parse_url_auth_option(ರಚನೆ SASL *sasl,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, size_t len);

/* Initializes an SASL structure */
ಶೂನ್ಯ Curl_sasl_init(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                    ಸ್ಥಿರಾಂಕ ರಚನೆ SASLproto *params);

/* Check if we have enough auth data and capabilities to authenticate */
bool Curl_sasl_can_authenticate(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Calculate the required login details for SASL authentication */
CURLcode Curl_sasl_ಆರಂಭ(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         bool force_ir, saslprogress *ಪ್ರಗತಿ);

/* Continue an SASL authentication */
CURLcode Curl_sasl_continue(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2, saslprogress *ಪ್ರಗತಿ);

CURLcode Curl_sasl_is_blocked(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SASL_H */
