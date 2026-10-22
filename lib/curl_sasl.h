#که_نه_تعریف_شوی سرلیک_CURL_SASL_H
#تعریف_کړه سرلیک_CURL_SASL_H
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
#ضمن "bufref.h"

بنية Curl_easy;
بنية connectdata;

/* Authentication mechanism flags */
#تعریف_کړه SASL_MECH_LOGIN             (1 << 0)
#تعریف_کړه SASL_MECH_PLAIN             (1 << 1)
#تعریف_کړه SASL_MECH_CRAM_MD5          (1 << 2)
#تعریف_کړه SASL_MECH_DIGEST_MD5        (1 << 3)
#تعریف_کړه SASL_MECH_GSSAPI            (1 << 4)
#تعریف_کړه SASL_MECH_EXTERNAL          (1 << 5)
#تعریف_کړه SASL_MECH_NTLM              (1 << 6)
#تعریف_کړه SASL_MECH_XOAUTH2           (1 << 7)
#تعریف_کړه SASL_MECH_OAUTHBEARER       (1 << 8)
#تعریف_کړه SASL_MECH_SCRAM_SHA_1       (1 << 9)
#تعریف_کړه SASL_MECH_SCRAM_SHA_256     (1 << 10)

/* Authentication mechanism values */
#تعریف_کړه SASL_AUTH_NONE          0
#تعریف_کړه SASL_AUTH_ANY           0xffff
#تعریف_کړه SASL_AUTH_DEFAULT       (SASL_AUTH_ANY & ~SASL_MECH_EXTERNAL)

/* Authentication mechanism strings */
#تعریف_کړه SASL_MECH_STRING_LOGIN          "LOGIN"
#تعریف_کړه SASL_MECH_STRING_PLAIN          "PLAIN"
#تعریف_کړه SASL_MECH_STRING_CRAM_MD5       "CRAM-MD5"
#تعریف_کړه SASL_MECH_STRING_DIGEST_MD5     "DIGEST-MD5"
#تعریف_کړه SASL_MECH_STRING_GSSAPI         "GSSAPI"
#تعریف_کړه SASL_MECH_STRING_EXTERNAL       "EXTERNAL"
#تعریف_کړه SASL_MECH_STRING_NTLM           "NTLM"
#تعریف_کړه SASL_MECH_STRING_XOAUTH2        "XOAUTH2"
#تعریف_کړه SASL_MECH_STRING_OAUTHBEARER    "OAUTHBEARER"
#تعریف_کړه SASL_MECH_STRING_SCRAM_SHA_1    "SCRAM-SHA-1"
#تعریف_کړه SASL_MECH_STRING_SCRAM_SHA_256  "SCRAM-SHA-256"

/* SASL flags */
#تعریف_کړه SASL_FLAG_BASE64        0x0001  /* Messages are base64-encoded */

/* SASL machine states */
تعريفنوع تعداد {
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
  SASL_GSSAPI_NO_معلومات,
  SASL_OAUTH2,
  SASL_OAUTH2_RESP,
  SASL_GSASL,
  SASL_CANCEL,
  SASL_FINAL
} saslstate;

/* Progress indicator */
تعريفنوع تعداد {
  SASL_IDLE,
  SASL_INPROGRESS,
  SASL_بشپړ
} saslprogress;

/* Protocol dependent SASL parameters */
بنية SASLproto {
  ثابت حرف *service;     /* The service name */
  CURLcode (*sendauth)(بنية Curl_easy *معلومات, ثابت حرف *mech,
                       ثابت بنية bufref *ir);
                           /* Send authentication command */
  CURLcode (*contauth)(بنية Curl_easy *معلومات, ثابت حرف *mech,
                       ثابت بنية bufref *contauth);
                           /* Send authentication continuation */
  CURLcode (*cancelauth)(بنية Curl_easy *معلومات, ثابت حرف *mech);
                           /* Cancel authentication. */
  CURLcode (*getmessage)(بنية Curl_easy *معلومات, بنية bufref *بهر);
                           /* Get SASL response message */
  size_t maxirlen;         /* Maximum initial response + mechanism length,
                              or zero if no max. This is normally the max
                              command length - other characters count.
                              This has to be zero for non-base64 protocols. */
  صحيح contcode;            /* Code to receive when continuation is expected */
  صحيح finalcode;           /* Code to receive upon authentication success */
  بینښه لنډ defmechs; /* Mechanisms enabled by default */
  بینښه لنډ flags;    /* Configuration flags. */
};

/* Per-connection parameters */
بنية SASL {
  ثابت بنية SASLproto *params; /* Protocol dependent parameters */
  saslstate state;           /* Current machine state */
  ثابت حرف *curmech;       /* Current mechanism id. */
  بینښه لنډ authmechs;  /* Accepted authentication mechanisms */
  بینښه لنډ prefmech;   /* Preferred authentication mechanism */
  بینښه لنډ authused;   /* Auth mechanism used for the connection */
  بټ(resetprefs);           /* For URL auth option parsing. */
  بټ(mutual_auth);          /* Mutual authentication enabled (GSSAPI only) */
  بټ(force_ir);             /* Protocol always supports initial response */
};

/* This is used to test whether the line starts with the given mechanism */
#تعریف_کړه sasl_mech_equal(کرښه, wordlen, mech) \
  ((wordlen) == (حجم(mech) - 1) / حجم(حرف) && \
   !memcmp(کرښه, mech, wordlen))

/* Convert a mechanism name to a token */
بینښه لنډ Curl_sasl_decode_mech(ثابت حرف *ptr,
                                     size_t maxlen, size_t *len);

/* Parse the URL login options */
CURLcode Curl_sasl_parse_url_auth_option(بنية SASL *sasl,
                                         ثابت حرف *ارزښت, size_t len);

/* Initializes an SASL structure */
فراغ Curl_sasl_init(بنية SASL *sasl, بنية Curl_easy *معلومات,
                    ثابت بنية SASLproto *params);

/* Check if we have enough auth data and capabilities to authenticate */
bool Curl_sasl_can_authenticate(بنية SASL *sasl, بنية Curl_easy *معلومات);

/* Calculate the required login details for SASL authentication */
CURLcode Curl_sasl_پیل(بنية SASL *sasl, بنية Curl_easy *معلومات,
                         bool force_ir, saslprogress *پرمختګ);

/* Continue an SASL authentication */
CURLcode Curl_sasl_continue(بنية SASL *sasl, بنية Curl_easy *معلومات,
                            صحيح کوډ, saslprogress *پرمختګ);

CURLcode Curl_sasl_is_blocked(بنية SASL *sasl, بنية Curl_easy *معلومات);

#د_که_پای /* HEADER_CURL_SASL_H */
