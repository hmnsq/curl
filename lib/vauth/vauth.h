#यदि_अपरिभाषितम् शीर्षकम्_CURL_VAUTH_H
#परिभाषय शीर्षकम्_CURL_VAUTH_H
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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "urldata.h"

संरचना Curl_easy;
संरचना Curl_creds;
संरचना connectdata;
संरचना Curl_peer;

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
संरचना digestdata;
#यदि_अन्तः

#यदि_परिभाषितम् USE_NTLM
संरचना ntlmdata;
#यदि_अन्तः

#यदि (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
संरचना negotiatedata;
#यदि_अन्तः

#यदि_परिभाषितम् USE_GSASL
संरचना gsasldata;
#यदि_अन्तः

#यदि_परिभाषितम् USE_WINDOWS_SSPI
#अन्तर्भाव "curl_sspi.h"
#परिभाषय GSS_ERROR(स्थितिः) ((स्थितिः) & 0x80000000)
#यदि_अन्तः

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_गन्तव्यम्_host(संरचना Curl_easy *दत्त);
bool Curl_auth_allowed_गन्तव्यम्_origin(संरचना Curl_easy *दत्त,
                                 संरचना Curl_peer *origin);

/* This is used to build an SPN string */
#यदि_अपरिभाषितम् USE_WINDOWS_SSPI
वर्ण *Curl_auth_build_spn(ध्रुव वर्ण *service, ध्रुव वर्ण *host,
                          ध्रुव वर्ण *realm);
#अन्यथा
TCHAR *Curl_auth_build_spn(ध्रुव वर्ण *service, ध्रुव वर्ण *host,
                           ध्रुव वर्ण *realm);
#यदि_अन्तः

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_अन्तर्भवति_domain(संरचना Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_सन्देशः(संरचना Curl_creds *creds,
                                        संरचना bufref *बहिर्गमनम्);

/* This is used to generate a LOGIN cleartext message */
शून्य Curl_auth_create_login_सन्देशः(ध्रुव वर्ण *मानम्, संरचना bufref *बहिर्गमनम्);

/* This is used to generate an EXTERNAL cleartext message */
शून्य Curl_auth_create_external_सन्देशः(ध्रुव वर्ण *user, संरचना bufref *बहिर्गमनम्);

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_सन्देशः(ध्रुव संरचना bufref *chlg,
                                           संरचना Curl_creds *creds,
                                           संरचना bufref *बहिर्गमनम्);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(शून्य);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_सन्देशः(संरचना Curl_easy *दत्त,
                                             ध्रुव संरचना bufref *chlg,
                                             संरचना Curl_creds *creds,
                                             ध्रुव वर्ण *default_service,
                                             संरचना bufref *बहिर्गमनम्);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_सन्देशः(ध्रुव वर्ण *chlg,
                                              संरचना digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_सन्देशः(संरचना Curl_easy *दत्त,
                                              संरचना Curl_creds *creds,
                                              ध्रुव अचिह्नित वर्ण *request,
                                              ध्रुव अचिह्नित वर्ण *uripath,
                                              संरचना digestdata *digest,
                                              वर्ण **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
शून्य Curl_auth_digest_cleanup(संरचना digestdata *digest);
#अन्यथा
#परिभाषय Curl_auth_digest_cleanup(x)
#परिभाषय Curl_auth_is_digest_supported()       FALSE
#यदि_अन्तः /* !CURL_DISABLE_DIGEST_AUTH */

#यदि_परिभाषितम् USE_GSASL

/* meta key for storing GSASL meta at connection */
#परिभाषय CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#अन्तर्भाव <gsasl.h>
संरचना gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

संरचना gsasldata *Curl_auth_gsasl_get(संरचना connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(संरचना Curl_easy *दत्त,
                                  ध्रुव वर्ण *mech,
                                  संरचना gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_start(संरचना Curl_easy *दत्त,
                               संरचना Curl_creds *creds,
                               संरचना gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(संरचना Curl_easy *दत्त,
                               ध्रुव संरचना bufref *chlg,
                               संरचना gsasldata *gsasl,
                               संरचना bufref *बहिर्गमनम्);

/* This is used to clean up the gsasl specific data */
शून्य Curl_auth_gsasl_cleanup(संरचना gsasldata *gsasl);
#यदि_अन्तः

#यदि_परिभाषितम् USE_NTLM

/* meta key for storing NTML meta at connection */
#परिभाषय CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#परिभाषय CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

संरचना ntlmdata {
#यदि_परिभाषितम् USE_WINDOWS_SSPI
/* The sslContext is used for the Schannel bindings. The
 * api is available on the Windows 7 SDK and later.
 */
  CtxtHandle *sslContext;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_तटस्थः_EX तटस्थः;
  SEC_WINNT_AUTH_तटस्थः_EX *p_तटस्थः;
  size_t token_max;
  BYTE *output_token;
  BYTE *निवेशः_token;
  size_t निवेशः_token_len;
  TCHAR *spn;
#अन्यथा
  अचिह्नित पूर्णाङ्क flags;
  अचिह्नित वर्ण nonce[8];
  अचिह्नित पूर्णाङ्क target_info_len;
  शून्य *target_info; /* TargetInfo received in the NTLM type-2 message */
#यदि_अन्तः
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(शून्य);

संरचना ntlmdata *Curl_auth_ntlm_get(संरचना connectdata *conn, bool proxy);
शून्य Curl_auth_ntlm_remove(संरचना connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
शून्य Curl_auth_cleanup_ntlm(संरचना ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_सन्देशः(संरचना Curl_easy *दत्त,
                                             संरचना Curl_creds *creds,
                                             ध्रुव वर्ण *default_service,
                                             ध्रुव वर्ण *host,
                                             संरचना ntlmdata *ntlm,
                                             संरचना bufref *बहिर्गमनम्);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_सन्देशः(संरचना Curl_easy *दत्त,
                                             ध्रुव संरचना bufref *type2ref,
                                             संरचना ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_सन्देशः(संरचना Curl_easy *दत्त,
                                             संरचना Curl_creds *creds,
                                             संरचना ntlmdata *ntlm,
                                             संरचना bufref *बहिर्गमनम्);

#अन्यथा
#परिभाषय Curl_auth_is_ntlm_supported()     FALSE
#यदि_अन्तः /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_सन्देशः(संरचना Curl_creds *creds,
                                               ध्रुव वर्ण *host,
                                               ध्रुव दीर्घ पत्तनम्,
                                               संरचना bufref *बहिर्गमनम्);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_सन्देशः(संरचना Curl_creds *creds,
                                                संरचना bufref *बहिर्गमनम्);

#यदि_परिभाषितम् USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#परिभाषय CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

संरचना kerberos5data {
#यदि_परिभाषितम् USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_तटस्थः_EX तटस्थः;
  SEC_WINNT_AUTH_तटस्थः_EX *p_तटस्थः;
  size_t token_max;
  BYTE *output_token;
#अन्यथा
  gss_ctx_id_t context;
  gss_name_t spn;
#यदि_अन्तः
};

संरचना kerberos5data *Curl_auth_krb5_get(संरचना connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(शून्य);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
सन्देशः*/
CURLcode Curl_auth_create_gssapi_user_सन्देशः(संरचना Curl_easy *दत्त,
                                              संरचना Curl_creds *creds,
                                              ध्रुव वर्ण *default_service,
                                              ध्रुव वर्ण *host,
                                              ध्रुव bool mutual_auth,
                                              ध्रुव संरचना bufref *chlg,
                                              संरचना kerberos5data *krb5,
                                              संरचना bufref *बहिर्गमनम्);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_सन्देशः(संरचना Curl_easy *दत्त,
                                                  ध्रुव वर्ण *authzid,
                                                  ध्रुव संरचना bufref *chlg,
                                                  संरचना kerberos5data *krb5,
                                                  संरचना bufref *बहिर्गमनम्);

/* This is used to clean up the GSSAPI specific data */
शून्य Curl_auth_cleanup_gssapi(संरचना kerberos5data *krb5);
#अन्यथा
#परिभाषय Curl_auth_is_gssapi_supported()       FALSE
#यदि_अन्तः /* USE_KERBEROS5 */

#यदि_परिभाषितम् USE_SPNEGO

bool Curl_auth_is_spnego_supported(शून्य);

/* meta key for storing NEGO meta at connection */
#परिभाषय CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#परिभाषय CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
संरचना negotiatedata {
#यदि_परिभाषितम् HAVE_GSSAPI
  OM_uint32 स्थितिः;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_बफर_desc output_token;
#यदि_परिभाषितम् GSS_C_CHANNEL_BOUND_FLAG
  संरचना dynbuf channel_binding_दत्त;
#यदि_अन्तः
#अन्यथा
#यदि_परिभाषितम् USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
  SECURITY_स्थितिः स्थितिः;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_तटस्थः_EX तटस्थः;
  SEC_WINNT_AUTH_तटस्थः_EX *p_तटस्थः;
  TCHAR *spn;
  size_t token_max;
  BYTE *output_token;
  size_t output_token_दीर्घता;
#यदि_अन्तः
#यदि_अन्तः
  BIT(noauthpersist);
  BIT(havenoauthpersist);
  BIT(havenegdata);
  BIT(havemultiplerequests);
};

संरचना negotiatedata *Curl_auth_nego_get(संरचना connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
सन्देशः*/
CURLcode Curl_auth_decode_spnego_सन्देशः(संरचना Curl_easy *दत्त,
                                         संरचना Curl_creds *creds,
                                         ध्रुव वर्ण *default_service,
                                         ध्रुव वर्ण *host,
                                         ध्रुव वर्ण *chlg64,
                                         संरचना negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
सन्देशः*/
CURLcode Curl_auth_create_spnego_सन्देशः(संरचना negotiatedata *nego,
                                         वर्ण **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
शून्य Curl_auth_cleanup_spnego(संरचना negotiatedata *nego);

#यदि_अन्तः /* USE_SPNEGO */

#यदि_अन्तः /* HEADER_CURL_VAUTH_H */
