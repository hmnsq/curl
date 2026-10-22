#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VAUTH_H
#నిర్వచించు శీర్షిక_CURL_VAUTH_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>.
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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "bufref.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "urldata.h"

నిర్మాణం Curl_easy;
నిర్మాణం Curl_creds;
నిర్మాణం connectdata;
నిర్మాణం Curl_peer;

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
నిర్మాణం digestdata;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_NTLM
నిర్మాణం ntlmdata;
#అయితే_ముగింపు

#ఒకవేళ (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
నిర్మాణం negotiatedata;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_GSASL
నిర్మాణం gsasldata;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_WINDOWS_SSPI
#అన్తర్భావ "curl_sspi.h"
#నిర్వచించు GSS_ERROR(స్థితి_2) ((స్థితి_2) & 0x80000000)
#అయితే_ముగింపు

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_గమ్యం_host(నిర్మాణం Curl_easy *దత్తాంశం);
bool Curl_auth_allowed_గమ్యం_origin(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం Curl_peer *origin);

/* This is used to build an SPN string */
#నిర్వచితం_కాకపోతే USE_WINDOWS_SSPI
అక్షరం *Curl_auth_build_spn(స్థిరరాశి అక్షరం *service, స్థిరరాశి అక్షరం *host,
                          స్థిరరాశి అక్షరం *realm);
#లేకపోతే
TCHAR *Curl_auth_build_spn(స్థిరరాశి అక్షరం *service, స్థిరరాశి అక్షరం *host,
                           స్థిరరాశి అక్షరం *realm);
#అయితే_ముగింపు

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_కలిగియుంది_domain(నిర్మాణం Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_సందేశం(నిర్మాణం Curl_creds *creds,
                                        నిర్మాణం bufref *బహిర్గతం);

/* This is used to generate a LOGIN cleartext message */
శూన్య Curl_auth_create_login_సందేశం(స్థిరరాశి అక్షరం *విలువ, నిర్మాణం bufref *బహిర్గతం);

/* This is used to generate an EXTERNAL cleartext message */
శూన్య Curl_auth_create_external_సందేశం(స్థిరరాశి అక్షరం *user, నిర్మాణం bufref *బహిర్గతం);

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_సందేశం(స్థిరరాశి నిర్మాణం bufref *chlg,
                                           నిర్మాణం Curl_creds *creds,
                                           నిర్మాణం bufref *బహిర్గతం);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(శూన్య);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             స్థిరరాశి నిర్మాణం bufref *chlg,
                                             నిర్మాణం Curl_creds *creds,
                                             స్థిరరాశి అక్షరం *default_service,
                                             నిర్మాణం bufref *బహిర్గతం);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_సందేశం(స్థిరరాశి అక్షరం *chlg,
                                              నిర్మాణం digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                              నిర్మాణం Curl_creds *creds,
                                              స్థిరరాశి గుర్తులేని అక్షరం *request,
                                              స్థిరరాశి గుర్తులేని అక్షరం *uripath,
                                              నిర్మాణం digestdata *digest,
                                              అక్షరం **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
శూన్య Curl_auth_digest_cleanup(నిర్మాణం digestdata *digest);
#లేకపోతే
#నిర్వచించు Curl_auth_digest_cleanup(x)
#నిర్వచించు Curl_auth_is_digest_supported()       FALSE
#అయితే_ముగింపు /* !CURL_DISABLE_DIGEST_AUTH */

#నిర్వచితమైతే USE_GSASL

/* meta key for storing GSASL meta at connection */
#నిర్వచించు CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#అన్తర్భావ <gsasl.h>
నిర్మాణం gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

నిర్మాణం gsasldata *Curl_auth_gsasl_get(నిర్మాణం connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *mech,
                                  నిర్మాణం gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_ప్రారంభం(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_creds *creds,
                               నిర్మాణం gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(నిర్మాణం Curl_easy *దత్తాంశం,
                               స్థిరరాశి నిర్మాణం bufref *chlg,
                               నిర్మాణం gsasldata *gsasl,
                               నిర్మాణం bufref *బహిర్గతం);

/* This is used to clean up the gsasl specific data */
శూన్య Curl_auth_gsasl_cleanup(నిర్మాణం gsasldata *gsasl);
#అయితే_ముగింపు

#నిర్వచితమైతే USE_NTLM

/* meta key for storing NTML meta at connection */
#నిర్వచించు CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#నిర్వచించు CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

నిర్మాణం ntlmdata {
#నిర్వచితమైతే USE_WINDOWS_SSPI
/* The sslContext is used for the Schannel bindings. The
 * api is available on the Windows 7 SDK and later.
 */
  CtxtHandle *sslContext;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_తటస్థం_EX తటస్థం;
  SEC_WINNT_AUTH_తటస్థం_EX *p_తటస్థం;
  size_t token_max;
  BYTE *output_token;
  BYTE *ఆదానం_token;
  size_t ఆదానం_token_len;
  TCHAR *spn;
#లేకపోతే
  గుర్తులేని పూర్ణాంకం flags;
  గుర్తులేని అక్షరం nonce[8];
  గుర్తులేని పూర్ణాంకం లక్ష్యం_info_len;
  శూన్య *లక్ష్యం_info; /* TargetInfo received in the NTLM type-2 message */
#అయితే_ముగింపు
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(శూన్య);

నిర్మాణం ntlmdata *Curl_auth_ntlm_get(నిర్మాణం connectdata *conn, bool proxy);
శూన్య Curl_auth_ntlm_remove(నిర్మాణం connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
శూన్య Curl_auth_cleanup_ntlm(నిర్మాణం ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_creds *creds,
                                             స్థిరరాశి అక్షరం *default_service,
                                             స్థిరరాశి అక్షరం *host,
                                             నిర్మాణం ntlmdata *ntlm,
                                             నిర్మాణం bufref *బహిర్గతం);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             స్థిరరాశి నిర్మాణం bufref *type2ref,
                                             నిర్మాణం ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_creds *creds,
                                             నిర్మాణం ntlmdata *ntlm,
                                             నిర్మాణం bufref *బహిర్గతం);

#లేకపోతే
#నిర్వచించు Curl_auth_is_ntlm_supported()     FALSE
#అయితే_ముగింపు /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_సందేశం(నిర్మాణం Curl_creds *creds,
                                               స్థిరరాశి అక్షరం *host,
                                               స్థిరరాశి దీర్ఘ పోర్టు,
                                               నిర్మాణం bufref *బహిర్గతం);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_సందేశం(నిర్మాణం Curl_creds *creds,
                                                నిర్మాణం bufref *బహిర్గతం);

#నిర్వచితమైతే USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#నిర్వచించు CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

నిర్మాణం kerberos5data {
#నిర్వచితమైతే USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_తటస్థం_EX తటస్థం;
  SEC_WINNT_AUTH_తటస్థం_EX *p_తటస్థం;
  size_t token_max;
  BYTE *output_token;
#లేకపోతే
  gss_ctx_id_t context;
  gss_name_t spn;
#అయితే_ముగింపు
};

నిర్మాణం kerberos5data *Curl_auth_krb5_get(నిర్మాణం connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(శూన్య);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
సందేశం*/
CURLcode Curl_auth_create_gssapi_user_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                              నిర్మాణం Curl_creds *creds,
                                              స్థిరరాశి అక్షరం *default_service,
                                              స్థిరరాశి అక్షరం *host,
                                              స్థిరరాశి bool mutual_auth,
                                              స్థిరరాశి నిర్మాణం bufref *chlg,
                                              నిర్మాణం kerberos5data *krb5,
                                              నిర్మాణం bufref *బహిర్గతం);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                                  స్థిరరాశి అక్షరం *authzid,
                                                  స్థిరరాశి నిర్మాణం bufref *chlg,
                                                  నిర్మాణం kerberos5data *krb5,
                                                  నిర్మాణం bufref *బహిర్గతం);

/* This is used to clean up the GSSAPI specific data */
శూన్య Curl_auth_cleanup_gssapi(నిర్మాణం kerberos5data *krb5);
#లేకపోతే
#నిర్వచించు Curl_auth_is_gssapi_supported()       FALSE
#అయితే_ముగింపు /* USE_KERBEROS5 */

#నిర్వచితమైతే USE_SPNEGO

bool Curl_auth_is_spnego_supported(శూన్య);

/* meta key for storing NEGO meta at connection */
#నిర్వచించు CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#నిర్వచించు CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
నిర్మాణం negotiatedata {
#నిర్వచితమైతే HAVE_GSSAPI
  OM_uint32 స్థితి_2;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_బఫరు_desc output_token;
#నిర్వచితమైతే GSS_C_CHANNEL_BOUND_FLAG
  నిర్మాణం dynbuf channel_binding_దత్తాంశం;
#అయితే_ముగింపు
#లేకపోతే
#నిర్వచితమైతే USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
  SECURITY_స్థితి స్థితి_2;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_తటస్థం_EX తటస్థం;
  SEC_WINNT_AUTH_తటస్థం_EX *p_తటస్థం;
  TCHAR *spn;
  size_t token_max;
  BYTE *output_token;
  size_t output_token_నిడివి;
#అయితే_ముగింపు
#అయితే_ముగింపు
  బిట్టు(noauthpersist);
  బిట్టు(havenoauthpersist);
  బిట్టు(havenegdata);
  బిట్టు(havemultiplerequests);
};

నిర్మాణం negotiatedata *Curl_auth_nego_get(నిర్మాణం connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
సందేశం*/
CURLcode Curl_auth_decode_spnego_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం Curl_creds *creds,
                                         స్థిరరాశి అక్షరం *default_service,
                                         స్థిరరాశి అక్షరం *host,
                                         స్థిరరాశి అక్షరం *chlg64,
                                         నిర్మాణం negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
సందేశం*/
CURLcode Curl_auth_create_spnego_సందేశం(నిర్మాణం negotiatedata *nego,
                                         అక్షరం **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
శూన్య Curl_auth_cleanup_spnego(నిర్మాణం negotiatedata *nego);

#అయితే_ముగింపు /* USE_SPNEGO */

#అయితే_ముగింపు /* HEADER_CURL_VAUTH_H */
