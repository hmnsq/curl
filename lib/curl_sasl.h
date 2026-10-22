#अगर_अघोषित HEADER_CURL_SASL_H
#घोषणा HEADER_CURL_SASL_H
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
#समावेश "bufref.h"

काष्ठा Curl_easy;
काष्ठा connectdata;

/* Authentication mechanism flags */
#घोषणा SASL_MECH_LOGIN             (1 << 0)
#घोषणा SASL_MECH_PLAIN             (1 << 1)
#घोषणा SASL_MECH_CRAM_MD5          (1 << 2)
#घोषणा SASL_MECH_DIGEST_MD5        (1 << 3)
#घोषणा SASL_MECH_GSSAPI            (1 << 4)
#घोषणा SASL_MECH_EXTERNAL          (1 << 5)
#घोषणा SASL_MECH_NTLM              (1 << 6)
#घोषणा SASL_MECH_XOAUTH2           (1 << 7)
#घोषणा SASL_MECH_OAUTHBEARER       (1 << 8)
#घोषणा SASL_MECH_SCRAM_SHA_1       (1 << 9)
#घोषणा SASL_MECH_SCRAM_SHA_256     (1 << 10)

/* Authentication mechanism values */
#घोषणा SASL_AUTH_NONE          0
#घोषणा SASL_AUTH_ANY           0xffff
#घोषणा SASL_AUTH_DEFAULT       (SASL_AUTH_ANY & ~SASL_MECH_EXTERNAL)

/* Authentication mechanism strings */
#घोषणा SASL_MECH_STRING_LOGIN          "LOGIN"
#घोषणा SASL_MECH_STRING_PLAIN          "PLAIN"
#घोषणा SASL_MECH_STRING_CRAM_MD5       "CRAM-MD5"
#घोषणा SASL_MECH_STRING_DIGEST_MD5     "DIGEST-MD5"
#घोषणा SASL_MECH_STRING_GSSAPI         "GSSAPI"
#घोषणा SASL_MECH_STRING_EXTERNAL       "EXTERNAL"
#घोषणा SASL_MECH_STRING_NTLM           "NTLM"
#घोषणा SASL_MECH_STRING_XOAUTH2        "XOAUTH2"
#घोषणा SASL_MECH_STRING_OAUTHBEARER    "OAUTHBEARER"
#घोषणा SASL_MECH_STRING_SCRAM_SHA_1    "SCRAM-SHA-1"
#घोषणा SASL_MECH_STRING_SCRAM_SHA_256  "SCRAM-SHA-256"

/* SASL flags */
#घोषणा SASL_FLAG_BASE64        0x0001  /* Messages are base64-encoded */

/* SASL machine states */
प्रकार क्रमागत {
  SASL_STOP,
  SASL_PLAIN,
  SASL_LOGIN,
  SASL_LOGIN_PASSWD,
  SASL_EXTERNAL,
  SASL_CRAMMD5,
  SASL_DIGESTMD5,
  SASL_DIGESTMD5_RESP,
  SASL_NTLM,
  SASL_NTLM_TYPE2_संदेश,
  SASL_GSSAPI,
  SASL_GSSAPI_TOKEN,
  SASL_GSSAPI_NO_तथ्य,
  SASL_OAUTH2,
  SASL_OAUTH2_RESP,
  SASL_GSASL,
  SASL_CANCEL,
  SASL_FINAL
} saslstate;

/* Progress indicator */
प्रकार क्रमागत {
  SASL_IDLE,
  SASL_INPROGRESS,
  SASL_DONE
} saslprogress;

/* Protocol dependent SASL parameters */
काष्ठा SASLproto {
  स्थिर अक्षर *service;     /* The service name */
  CURLcode (*sendauth)(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *mech,
                       स्थिर काष्ठा bufref *ir);
                           /* Send authentication command */
  CURLcode (*contauth)(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *mech,
                       स्थिर काष्ठा bufref *contauth);
                           /* Send authentication continuation */
  CURLcode (*cancelauth)(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *mech);
                           /* Cancel authentication. */
  CURLcode (*getmessage)(काष्ठा Curl_easy *तथ्य, काष्ठा bufref *out);
                           /* Get SASL response message */
  माप_प्रकार maxirlen;         /* Maximum initial response + mechanism length,
                              or zero if no max. This is normally the max
                              command length - other characters count.
                              This has to be zero for non-base64 protocols. */
  पूर्णांक contcode;            /* Code to receive when continuation is expected */
  पूर्णांक finalcode;           /* Code to receive upon authentication success */
  अचिन्हित लघु defmechs; /* Mechanisms enabled by default */
  अचिन्हित लघु flags;    /* Configuration flags. */
};

/* Per-connection parameters */
काष्ठा SASL {
  स्थिर काष्ठा SASLproto *params; /* Protocol dependent parameters */
  saslstate state;           /* Current machine state */
  स्थिर अक्षर *curmech;       /* Current mechanism id. */
  अचिन्हित लघु authmechs;  /* Accepted authentication mechanisms */
  अचिन्हित लघु prefmech;   /* Preferred authentication mechanism */
  अचिन्हित लघु authused;   /* Auth mechanism used for the connection */
  BIT(resetprefs);           /* For URL auth option parsing. */
  BIT(mutual_auth);          /* Mutual authentication enabled (GSSAPI only) */
  BIT(force_ir);             /* Protocol always supports initial response */
};

/* This is used to test whether the line starts with the given mechanism */
#घोषणा sasl_mech_equal(पंक्ति, wordlen, mech) \
  ((wordlen) == (माप(mech) - 1) / माप(अक्षर) && \
   !स_भेद(पंक्ति, mech, wordlen))

/* Convert a mechanism name to a token */
अचिन्हित लघु Curl_sasl_decode_mech(स्थिर अक्षर *ptr,
                                     माप_प्रकार maxlen, माप_प्रकार *len);

/* Parse the URL login options */
CURLcode Curl_sasl_parse_url_auth_option(काष्ठा SASL *sasl,
                                         स्थिर अक्षर *मूल्य, माप_प्रकार len);

/* Initializes an SASL structure */
व्योम Curl_sasl_init(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य,
                    स्थिर काष्ठा SASLproto *params);

/* Check if we have enough auth data and capabilities to authenticate */
bool Curl_sasl_can_authenticate(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य);

/* Calculate the required login details for SASL authentication */
CURLcode Curl_sasl_start(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य,
                         bool force_ir, saslprogress *progress);

/* Continue an SASL authentication */
CURLcode Curl_sasl_continue(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य,
                            पूर्णांक code, saslprogress *progress);

CURLcode Curl_sasl_is_blocked(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य);

#पूर्ण_अगर /* HEADER_CURL_SASL_H */
