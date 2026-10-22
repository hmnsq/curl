#अगर_अघोषित HEADER_CURL_VAUTH_H
#घोषणा HEADER_CURL_VAUTH_H
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
#समावेश "curl_setup.h"

#समावेश "bufref.h"
#समावेश "curlx/dynbuf.h"
#समावेश "urldata.h"

काष्ठा Curl_easy;
काष्ठा Curl_creds;
काष्ठा connectdata;
काष्ठा Curl_peer;

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
काष्ठा digestdata;
#पूर्ण_अगर

#अगर_घोषित USE_NTLM
काष्ठा ntlmdata;
#पूर्ण_अगर

#अगर (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
काष्ठा negotiatedata;
#पूर्ण_अगर

#अगर_घोषित USE_GSASL
काष्ठा gsasldata;
#पूर्ण_अगर

#अगर_घोषित USE_WINDOWS_SSPI
#समावेश "curl_sspi.h"
#घोषणा GSS_ERROR(status) ((status) & 0x80000000)
#पूर्ण_अगर

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_to_host(काष्ठा Curl_easy *तथ्य);
bool Curl_auth_allowed_to_origin(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा Curl_peer *origin);

/* This is used to build an SPN string */
#अगर_अघोषित USE_WINDOWS_SSPI
अक्षर *Curl_auth_build_spn(स्थिर अक्षर *service, स्थिर अक्षर *host,
                          स्थिर अक्षर *realm);
#अन्यथा
TCHAR *Curl_auth_build_spn(स्थिर अक्षर *service, स्थिर अक्षर *host,
                           स्थिर अक्षर *realm);
#पूर्ण_अगर

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_contains_domain(काष्ठा Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_message(काष्ठा Curl_creds *creds,
                                        काष्ठा bufref *out);

/* This is used to generate a LOGIN cleartext message */
व्योम Curl_auth_create_login_message(स्थिर अक्षर *मूल्य, काष्ठा bufref *out);

/* This is used to generate an EXTERNAL cleartext message */
व्योम Curl_auth_create_external_message(स्थिर अक्षर *user, काष्ठा bufref *out);

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_message(स्थिर काष्ठा bufref *chlg,
                                           काष्ठा Curl_creds *creds,
                                           काष्ठा bufref *out);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(व्योम);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_message(काष्ठा Curl_easy *तथ्य,
                                             स्थिर काष्ठा bufref *chlg,
                                             काष्ठा Curl_creds *creds,
                                             स्थिर अक्षर *default_service,
                                             काष्ठा bufref *out);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_message(स्थिर अक्षर *chlg,
                                              काष्ठा digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_message(काष्ठा Curl_easy *तथ्य,
                                              काष्ठा Curl_creds *creds,
                                              स्थिर अचिन्हित अक्षर *request,
                                              स्थिर अचिन्हित अक्षर *uripath,
                                              काष्ठा digestdata *digest,
                                              अक्षर **outptr, माप_प्रकार *outlen);

/* This is used to clean up the digest specific data */
व्योम Curl_auth_digest_cleanup(काष्ठा digestdata *digest);
#अन्यथा
#घोषणा Curl_auth_digest_cleanup(x)
#घोषणा Curl_auth_is_digest_supported()       FALSE
#पूर्ण_अगर /* !CURL_DISABLE_DIGEST_AUTH */

#अगर_घोषित USE_GSASL

/* meta key for storing GSASL meta at connection */
#घोषणा CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#समावेश <gsasl.h>
काष्ठा gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

काष्ठा gsasldata *Curl_auth_gsasl_get(काष्ठा connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *mech,
                                  काष्ठा gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_start(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_creds *creds,
                               काष्ठा gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(काष्ठा Curl_easy *तथ्य,
                               स्थिर काष्ठा bufref *chlg,
                               काष्ठा gsasldata *gsasl,
                               काष्ठा bufref *out);

/* This is used to clean up the gsasl specific data */
व्योम Curl_auth_gsasl_cleanup(काष्ठा gsasldata *gsasl);
#पूर्ण_अगर

#अगर_घोषित USE_NTLM

/* meta key for storing NTML meta at connection */
#घोषणा CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#घोषणा CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

काष्ठा ntlmdata {
#अगर_घोषित USE_WINDOWS_SSPI
/* The sslContext is used for the Schannel bindings. The
 * api is available on the Windows 7 SDK and later.
 */
  CtxtHandle *sslContext;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  माप_प्रकार token_max;
  BYTE *output_token;
  BYTE *निवेश_token;
  माप_प्रकार निवेश_token_len;
  TCHAR *spn;
#अन्यथा
  अचिन्हित पूर्णांक flags;
  अचिन्हित अक्षर nonce[8];
  अचिन्हित पूर्णांक target_info_len;
  व्योम *target_info; /* TargetInfo received in the NTLM type-2 message */
#पूर्ण_अगर
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(व्योम);

काष्ठा ntlmdata *Curl_auth_ntlm_get(काष्ठा connectdata *conn, bool proxy);
व्योम Curl_auth_ntlm_remove(काष्ठा connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
व्योम Curl_auth_cleanup_ntlm(काष्ठा ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_message(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_creds *creds,
                                             स्थिर अक्षर *default_service,
                                             स्थिर अक्षर *host,
                                             काष्ठा ntlmdata *ntlm,
                                             काष्ठा bufref *out);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_message(काष्ठा Curl_easy *तथ्य,
                                             स्थिर काष्ठा bufref *type2ref,
                                             काष्ठा ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_message(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_creds *creds,
                                             काष्ठा ntlmdata *ntlm,
                                             काष्ठा bufref *out);

#अन्यथा
#घोषणा Curl_auth_is_ntlm_supported()     FALSE
#पूर्ण_अगर /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_message(काष्ठा Curl_creds *creds,
                                               स्थिर अक्षर *host,
                                               स्थिर दीर्घ port,
                                               काष्ठा bufref *out);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_message(काष्ठा Curl_creds *creds,
                                                काष्ठा bufref *out);

#अगर_घोषित USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#घोषणा CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

काष्ठा kerberos5data {
#अगर_घोषित USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  माप_प्रकार token_max;
  BYTE *output_token;
#अन्यथा
  gss_ctx_id_t context;
  gss_name_t spn;
#पूर्ण_अगर
};

काष्ठा kerberos5data *Curl_auth_krb5_get(काष्ठा connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(व्योम);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
   message */
CURLcode Curl_auth_create_gssapi_user_message(काष्ठा Curl_easy *तथ्य,
                                              काष्ठा Curl_creds *creds,
                                              स्थिर अक्षर *default_service,
                                              स्थिर अक्षर *host,
                                              स्थिर bool mutual_auth,
                                              स्थिर काष्ठा bufref *chlg,
                                              काष्ठा kerberos5data *krb5,
                                              काष्ठा bufref *out);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_message(काष्ठा Curl_easy *तथ्य,
                                                  स्थिर अक्षर *authzid,
                                                  स्थिर काष्ठा bufref *chlg,
                                                  काष्ठा kerberos5data *krb5,
                                                  काष्ठा bufref *out);

/* This is used to clean up the GSSAPI specific data */
व्योम Curl_auth_cleanup_gssapi(काष्ठा kerberos5data *krb5);
#अन्यथा
#घोषणा Curl_auth_is_gssapi_supported()       FALSE
#पूर्ण_अगर /* USE_KERBEROS5 */

#अगर_घोषित USE_SPNEGO

bool Curl_auth_is_spnego_supported(व्योम);

/* meta key for storing NEGO meta at connection */
#घोषणा CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#घोषणा CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
काष्ठा negotiatedata {
#अगर_घोषित HAVE_GSSAPI
  OM_uint32 status;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_बफर_desc output_token;
#अगर_घोषित GSS_C_CHANNEL_BOUND_FLAG
  काष्ठा dynbuf channel_binding_तथ्य;
#पूर्ण_अगर
#अन्यथा
#अगर_घोषित USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
  SECURITY_STATUS status;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  TCHAR *spn;
  माप_प्रकार token_max;
  BYTE *output_token;
  माप_प्रकार output_token_लंबाई;
#पूर्ण_अगर
#पूर्ण_अगर
  BIT(noauthpersist);
  BIT(havenoauthpersist);
  BIT(havenegdata);
  BIT(havemultiplerequests);
};

काष्ठा negotiatedata *Curl_auth_nego_get(काष्ठा connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
   message */
CURLcode Curl_auth_decode_spnego_message(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा Curl_creds *creds,
                                         स्थिर अक्षर *default_service,
                                         स्थिर अक्षर *host,
                                         स्थिर अक्षर *chlg64,
                                         काष्ठा negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
   message */
CURLcode Curl_auth_create_spnego_message(काष्ठा negotiatedata *nego,
                                         अक्षर **outptr, माप_प्रकार *outlen);

/* This is used to clean up the SPNEGO specific data */
व्योम Curl_auth_cleanup_spnego(काष्ठा negotiatedata *nego);

#पूर्ण_अगर /* USE_SPNEGO */

#पूर्ण_अगर /* HEADER_CURL_VAUTH_H */
