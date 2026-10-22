#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SASL_H
#నిర్వచించు శీర్షిక_CURL_SASL_H
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
#అన్తర్భావ "bufref.h"

నిర్మాణం Curl_easy;
నిర్మాణం connectdata;

/* Authentication mechanism flags */
#నిర్వచించు SASL_MECH_LOGIN             (1 << 0)
#నిర్వచించు SASL_MECH_PLAIN             (1 << 1)
#నిర్వచించు SASL_MECH_CRAM_MD5          (1 << 2)
#నిర్వచించు SASL_MECH_DIGEST_MD5        (1 << 3)
#నిర్వచించు SASL_MECH_GSSAPI            (1 << 4)
#నిర్వచించు SASL_MECH_EXTERNAL          (1 << 5)
#నిర్వచించు SASL_MECH_NTLM              (1 << 6)
#నిర్వచించు SASL_MECH_XOAUTH2           (1 << 7)
#నిర్వచించు SASL_MECH_OAUTHBEARER       (1 << 8)
#నిర్వచించు SASL_MECH_SCRAM_SHA_1       (1 << 9)
#నిర్వచించు SASL_MECH_SCRAM_SHA_256     (1 << 10)

/* Authentication mechanism values */
#నిర్వచించు SASL_AUTH_NONE          0
#నిర్వచించు SASL_AUTH_ANY           0xffff
#నిర్వచించు SASL_AUTH_DEFAULT       (SASL_AUTH_ANY & ~SASL_MECH_EXTERNAL)

/* Authentication mechanism strings */
#నిర్వచించు SASL_MECH_STRING_LOGIN          "LOGIN"
#నిర్వచించు SASL_MECH_STRING_PLAIN          "PLAIN"
#నిర్వచించు SASL_MECH_STRING_CRAM_MD5       "CRAM-MD5"
#నిర్వచించు SASL_MECH_STRING_DIGEST_MD5     "DIGEST-MD5"
#నిర్వచించు SASL_MECH_STRING_GSSAPI         "GSSAPI"
#నిర్వచించు SASL_MECH_STRING_EXTERNAL       "EXTERNAL"
#నిర్వచించు SASL_MECH_STRING_NTLM           "NTLM"
#నిర్వచించు SASL_MECH_STRING_XOAUTH2        "XOAUTH2"
#నిర్వచించు SASL_MECH_STRING_OAUTHBEARER    "OAUTHBEARER"
#నిర్వచించు SASL_MECH_STRING_SCRAM_SHA_1    "SCRAM-SHA-1"
#నిర్వచించు SASL_MECH_STRING_SCRAM_SHA_256  "SCRAM-SHA-256"

/* SASL flags */
#నిర్వచించు SASL_FLAG_BASE64        0x0001  /* Messages are base64-encoded */

/* SASL machine states */
రకనిర్వచనం గణన {
  SASL_STOP,
  SASL_PLAIN,
  SASL_LOGIN,
  SASL_LOGIN_PASSWD,
  SASL_EXTERNAL,
  SASL_CRAMMD5,
  SASL_DIGESTMD5,
  SASL_DIGESTMD5_RESP,
  SASL_NTLM,
  SASL_NTLM_TYPE2_సందేశం,
  SASL_GSSAPI,
  SASL_GSSAPI_TOKEN,
  SASL_GSSAPI_NO_దత్తాంశం,
  SASL_OAUTH2,
  SASL_OAUTH2_RESP,
  SASL_GSASL,
  SASL_CANCEL,
  SASL_FINAL
} saslstate;

/* Progress indicator */
రకనిర్వచనం గణన {
  SASL_IDLE,
  SASL_INPROGRESS,
  SASL_పూర్తి
} saslprogress;

/* Protocol dependent SASL parameters */
నిర్మాణం SASLproto {
  స్థిరరాశి అక్షరం *service;     /* The service name */
  CURLcode (*sendauth)(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *mech,
                       స్థిరరాశి నిర్మాణం bufref *ir);
                           /* Send authentication command */
  CURLcode (*contauth)(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *mech,
                       స్థిరరాశి నిర్మాణం bufref *contauth);
                           /* Send authentication continuation */
  CURLcode (*cancelauth)(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *mech);
                           /* Cancel authentication. */
  CURLcode (*getmessage)(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం bufref *బహిర్గతం);
                           /* Get SASL response message */
  size_t maxirlen;         /* Maximum initial response + mechanism length,
                              or zero if no max. This is normally the max
                              command length - other characters count.
                              This has to be zero for non-base64 protocols. */
  పూర్ణాంకం contcode;            /* Code to receive when continuation is expected */
  పూర్ణాంకం finalcode;           /* Code to receive upon authentication success */
  గుర్తులేని పొట్టి defmechs; /* Mechanisms enabled by default */
  గుర్తులేని పొట్టి flags;    /* Configuration flags. */
};

/* Per-connection parameters */
నిర్మాణం SASL {
  స్థిరరాశి నిర్మాణం SASLproto *params; /* Protocol dependent parameters */
  saslstate state;           /* Current machine state */
  స్థిరరాశి అక్షరం *curmech;       /* Current mechanism id. */
  గుర్తులేని పొట్టి authmechs;  /* Accepted authentication mechanisms */
  గుర్తులేని పొట్టి prefmech;   /* Preferred authentication mechanism */
  గుర్తులేని పొట్టి authused;   /* Auth mechanism used for the connection */
  బిట్టు(resetprefs);           /* For URL auth option parsing. */
  బిట్టు(mutual_auth);          /* Mutual authentication enabled (GSSAPI only) */
  బిట్టు(force_ir);             /* Protocol always supports initial response */
};

/* This is used to test whether the line starts with the given mechanism */
#నిర్వచించు sasl_mech_equal(పంక్తి, wordlen, mech) \
  ((wordlen) == (పరిమాణం(mech) - 1) / పరిమాణం(అక్షరం) && \
   !memcmp(పంక్తి, mech, wordlen))

/* Convert a mechanism name to a token */
గుర్తులేని పొట్టి Curl_sasl_decode_mech(స్థిరరాశి అక్షరం *ptr,
                                     size_t maxlen, size_t *len);

/* Parse the URL login options */
CURLcode Curl_sasl_parse_url_auth_option(నిర్మాణం SASL *sasl,
                                         స్థిరరాశి అక్షరం *విలువ, size_t len);

/* Initializes an SASL structure */
శూన్య Curl_sasl_init(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం,
                    స్థిరరాశి నిర్మాణం SASLproto *params);

/* Check if we have enough auth data and capabilities to authenticate */
bool Curl_sasl_can_authenticate(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం);

/* Calculate the required login details for SASL authentication */
CURLcode Curl_sasl_ప్రారంభం(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం,
                         bool force_ir, saslprogress *పురోగతి);

/* Continue an SASL authentication */
CURLcode Curl_sasl_continue(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం,
                            పూర్ణాంకం సంకేతం_2, saslprogress *పురోగతి);

CURLcode Curl_sasl_is_blocked(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం);

#అయితే_ముగింపు /* HEADER_CURL_SASL_H */
