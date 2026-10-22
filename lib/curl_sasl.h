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
#அந்தர்பாவ "bufref.h"

கட்டமைப்பு Curl_easy;
கட்டமைப்பு connectdata;

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
வகைவரை எண்ணிக்கை {
  SASL_STOP,
  SASL_PLAIN,
  SASL_LOGIN,
  SASL_LOGIN_PASSWD,
  SASL_EXTERNAL,
  SASL_CRAMMD5,
  SASL_DIGESTMD5,
  SASL_DIGESTMD5_RESP,
  SASL_NTLM,
  SASL_NTLM_TYPE2_செய்தி,
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
வகைவரை எண்ணிக்கை {
  SASL_IDLE,
  SASL_INPROGRESS,
  SASL_DONE
} saslprogress;

/* Protocol dependent SASL parameters */
கட்டமைப்பு SASLproto {
  மாறிலி எழுத்து *service;     /* The service name */
  CURLcode (*sendauth)(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *mech,
                       மாறிலி கட்டமைப்பு bufref *ir);
                           /* Send authentication command */
  CURLcode (*contauth)(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *mech,
                       மாறிலி கட்டமைப்பு bufref *contauth);
                           /* Send authentication continuation */
  CURLcode (*cancelauth)(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *mech);
                           /* Cancel authentication. */
  CURLcode (*getmessage)(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு bufref *out);
                           /* Get SASL response message */
  size_t maxirlen;         /* Maximum initial response + mechanism length,
                              or zero if no max. This is normally the max
                              command length - other characters count.
                              This has to be zero for non-base64 protocols. */
  முழு contcode;            /* Code to receive when continuation is expected */
  முழு finalcode;           /* Code to receive upon authentication success */
  unsigned short defmechs; /* Mechanisms enabled by default */
  unsigned short flags;    /* Configuration flags. */
};

/* Per-connection parameters */
கட்டமைப்பு SASL {
  மாறிலி கட்டமைப்பு SASLproto *params; /* Protocol dependent parameters */
  saslstate state;           /* Current machine state */
  மாறிலி எழுத்து *curmech;       /* Current mechanism id. */
  unsigned short authmechs;  /* Accepted authentication mechanisms */
  unsigned short prefmech;   /* Preferred authentication mechanism */
  unsigned short authused;   /* Auth mechanism used for the connection */
  BIT(resetprefs);           /* For URL auth option parsing. */
  BIT(mutual_auth);          /* Mutual authentication enabled (GSSAPI only) */
  BIT(force_ir);             /* Protocol always supports initial response */
};

/* This is used to test whether the line starts with the given mechanism */
#define sasl_mech_equal(வரி, wordlen, mech) \
  ((wordlen) == (அளவு(mech) - 1) / அளவு(எழுத்து) && \
   !memcmp(வரி, mech, wordlen))

/* Convert a mechanism name to a token */
unsigned short Curl_sasl_decode_mech(மாறிலி எழுத்து *ptr,
                                     size_t maxlen, size_t *len);

/* Parse the URL login options */
CURLcode Curl_sasl_parse_url_auth_option(கட்டமைப்பு SASL *sasl,
                                         மாறிலி எழுத்து *மதிப்பு, size_t len);

/* Initializes an SASL structure */
வெறுமை Curl_sasl_init(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data,
                    மாறிலி கட்டமைப்பு SASLproto *params);

/* Check if we have enough auth data and capabilities to authenticate */
bool Curl_sasl_can_authenticate(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data);

/* Calculate the required login details for SASL authentication */
CURLcode Curl_sasl_start(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data,
                         bool force_ir, saslprogress *progress);

/* Continue an SASL authentication */
CURLcode Curl_sasl_continue(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data,
                            முழு code, saslprogress *progress);

CURLcode Curl_sasl_is_blocked(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data);

#endif /* HEADER_CURL_SASL_H */
