#اگر_غیر_تعریف_شدہ سرخی_CURL_SASL_H
#تعریف_کرو سرخی_CURL_SASL_H
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
#شامل "bufref.h"

ساخت Curl_easy;
ساخت connectdata;

/* Authentication mechanism flags */
#تعریف_کرو SASL_MECH_LOGIN             (1 << 0)
#تعریف_کرو SASL_MECH_PLAIN             (1 << 1)
#تعریف_کرو SASL_MECH_CRAM_MD5          (1 << 2)
#تعریف_کرو SASL_MECH_DIGEST_MD5        (1 << 3)
#تعریف_کرو SASL_MECH_GSSAPI            (1 << 4)
#تعریف_کرو SASL_MECH_EXTERNAL          (1 << 5)
#تعریف_کرو SASL_MECH_NTLM              (1 << 6)
#تعریف_کرو SASL_MECH_XOAUTH2           (1 << 7)
#تعریف_کرو SASL_MECH_OAUTHBEARER       (1 << 8)
#تعریف_کرو SASL_MECH_SCRAM_SHA_1       (1 << 9)
#تعریف_کرو SASL_MECH_SCRAM_SHA_256     (1 << 10)

/* Authentication mechanism values */
#تعریف_کرو SASL_AUTH_NONE          0
#تعریف_کرو SASL_AUTH_ANY           0xffff
#تعریف_کرو SASL_AUTH_DEFAULT       (SASL_AUTH_ANY & ~SASL_MECH_EXTERNAL)

/* Authentication mechanism strings */
#تعریف_کرو SASL_MECH_STRING_LOGIN          "LOGIN"
#تعریف_کرو SASL_MECH_STRING_PLAIN          "PLAIN"
#تعریف_کرو SASL_MECH_STRING_CRAM_MD5       "CRAM-MD5"
#تعریف_کرو SASL_MECH_STRING_DIGEST_MD5     "DIGEST-MD5"
#تعریف_کرو SASL_MECH_STRING_GSSAPI         "GSSAPI"
#تعریف_کرو SASL_MECH_STRING_EXTERNAL       "EXTERNAL"
#تعریف_کرو SASL_MECH_STRING_NTLM           "NTLM"
#تعریف_کرو SASL_MECH_STRING_XOAUTH2        "XOAUTH2"
#تعریف_کرو SASL_MECH_STRING_OAUTHBEARER    "OAUTHBEARER"
#تعریف_کرو SASL_MECH_STRING_SCRAM_SHA_1    "SCRAM-SHA-1"
#تعریف_کرو SASL_MECH_STRING_SCRAM_SHA_256  "SCRAM-SHA-256"

/* SASL flags */
#تعریف_کرو SASL_FLAG_BASE64        0x0001  /* Messages are base64-encoded */

/* SASL machine states */
قسمتعریف شمار {
  SASL_STOP,
  SASL_PLAIN,
  SASL_LOGIN,
  SASL_LOGIN_PASSWD,
  SASL_EXTERNAL,
  SASL_CRAMMD5,
  SASL_DIGESTMD5,
  SASL_DIGESTMD5_RESP,
  SASL_NTLM,
  SASL_NTLM_TYPE2_پیغام,
  SASL_GSSAPI,
  SASL_GSSAPI_TOKEN,
  SASL_GSSAPI_NO_ڈیٹا,
  SASL_OAUTH2,
  SASL_OAUTH2_RESP,
  SASL_GSASL,
  SASL_CANCEL,
  SASL_FINAL
} saslstate;

/* Progress indicator */
قسمتعریف شمار {
  SASL_IDLE,
  SASL_INPROGRESS,
  SASL_مکمل
} saslprogress;

/* Protocol dependent SASL parameters */
ساخت SASLproto {
  ثابت حرف *service;     /* The service name */
  CURLcode (*sendauth)(ساخت Curl_easy *ڈیٹا, ثابت حرف *mech,
                       ثابت ساخت bufref *ir);
                           /* Send authentication command */
  CURLcode (*contauth)(ساخت Curl_easy *ڈیٹا, ثابت حرف *mech,
                       ثابت ساخت bufref *contauth);
                           /* Send authentication continuation */
  CURLcode (*cancelauth)(ساخت Curl_easy *ڈیٹا, ثابت حرف *mech);
                           /* Cancel authentication. */
  CURLcode (*getmessage)(ساخت Curl_easy *ڈیٹا, ساخت bufref *برآمد);
                           /* Get SASL response message */
  size_t maxirlen;         /* Maximum initial response + mechanism length,
                              or zero if no max. This is normally the max
                              command length - other characters count.
                              This has to be zero for non-base64 protocols. */
  صحیح contcode;            /* Code to receive when continuation is expected */
  صحیح finalcode;           /* Code to receive upon authentication success */
  غیرعلامتی مختصر defmechs; /* Mechanisms enabled by default */
  غیرعلامتی مختصر flags;    /* Configuration flags. */
};

/* Per-connection parameters */
ساخت SASL {
  ثابت ساخت SASLproto *params; /* Protocol dependent parameters */
  saslstate state;           /* Current machine state */
  ثابت حرف *curmech;       /* Current mechanism id. */
  غیرعلامتی مختصر authmechs;  /* Accepted authentication mechanisms */
  غیرعلامتی مختصر prefmech;   /* Preferred authentication mechanism */
  غیرعلامتی مختصر authused;   /* Auth mechanism used for the connection */
  ثنائی_ہندسہ(resetprefs);           /* For URL auth option parsing. */
  ثنائی_ہندسہ(mutual_auth);          /* Mutual authentication enabled (GSSAPI only) */
  ثنائی_ہندسہ(force_ir);             /* Protocol always supports initial response */
};

/* This is used to test whether the line starts with the given mechanism */
#تعریف_کرو sasl_mech_equal(سطر, wordlen, mech) \
  ((wordlen) == (حجم(mech) - 1) / حجم(حرف) && \
   !memcmp(سطر, mech, wordlen))

/* Convert a mechanism name to a token */
غیرعلامتی مختصر Curl_sasl_decode_mech(ثابت حرف *ptr,
                                     size_t maxlen, size_t *len);

/* Parse the URL login options */
CURLcode Curl_sasl_parse_url_auth_option(ساخت SASL *sasl,
                                         ثابت حرف *قدر, size_t len);

/* Initializes an SASL structure */
خالی Curl_sasl_init(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا,
                    ثابت ساخت SASLproto *params);

/* Check if we have enough auth data and capabilities to authenticate */
bool Curl_sasl_can_authenticate(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا);

/* Calculate the required login details for SASL authentication */
CURLcode Curl_sasl_آغاز(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا,
                         bool force_ir, saslprogress *پیش_رفت);

/* Continue an SASL authentication */
CURLcode Curl_sasl_continue(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا,
                            صحیح رمز, saslprogress *پیش_رفت);

CURLcode Curl_sasl_is_blocked(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا);

#اگر_ختم /* HEADER_CURL_SASL_H */
