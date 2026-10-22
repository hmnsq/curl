#ifndef HEADER_CURL_VAUTH_H
#define HEADER_CURL_VAUTH_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"

ಸಂರಚನಾ Curl_easy;
ಸಂರಚನಾ Curl_creds;
ಸಂರಚನಾ connectdata;
ಸಂರಚನಾ Curl_peer;

#ifndef CURL_DISABLE_DIGEST_AUTH
ಸಂರಚನಾ digestdata;
#endif

#ifdef USE_NTLM
ಸಂರಚನಾ ntlmdata;
#endif

#ಯದಿ (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
ಸಂರಚನಾ negotiatedata;
#endif

#ifdef USE_GSASL
ಸಂರಚನಾ gsasldata;
#endif

#ifdef USE_WINDOWS_SSPI
#ಅನ್ತರ್ಭಾವ "curl_sspi.h"
#define GSS_ERROR(status) ((status) & 0x80000000)
#endif

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_to_host(ಸಂರಚನಾ Curl_easy *data);
bool Curl_auth_allowed_to_origin(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ Curl_peer *origin);

/* This is used to build an SPN string */
#ifndef USE_WINDOWS_SSPI
ವರ್ಣ *Curl_auth_build_spn(ಧ್ರುವ ವರ್ಣ *service, ಧ್ರುವ ವರ್ಣ *host,
                          ಧ್ರುವ ವರ್ಣ *realm);
#ಅನ್ಯಥಾ
TCHAR *Curl_auth_build_spn(ಧ್ರುವ ವರ್ಣ *service, ಧ್ರುವ ವರ್ಣ *host,
                           ಧ್ರುವ ವರ್ಣ *realm);
#endif

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_contains_domain(ಸಂರಚನಾ Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_message(ಸಂರಚನಾ Curl_creds *creds,
                                        ಸಂರಚನಾ bufref *out);

/* This is used to generate a LOGIN cleartext message */
ಶೂನ್ಯ Curl_auth_create_login_message(ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, ಸಂರಚನಾ bufref *out);

/* This is used to generate an EXTERNAL cleartext message */
ಶೂನ್ಯ Curl_auth_create_external_message(ಧ್ರುವ ವರ್ಣ *user, ಸಂರಚನಾ bufref *out);

#ifndef CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_message(ಧ್ರುವ ಸಂರಚನಾ bufref *chlg,
                                           ಸಂರಚನಾ Curl_creds *creds,
                                           ಸಂರಚನಾ bufref *out);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(ಶೂನ್ಯ);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_message(ಸಂರಚನಾ Curl_easy *data,
                                             ಧ್ರುವ ಸಂರಚನಾ bufref *chlg,
                                             ಸಂರಚನಾ Curl_creds *creds,
                                             ಧ್ರುವ ವರ್ಣ *default_service,
                                             ಸಂರಚನಾ bufref *out);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_message(ಧ್ರುವ ವರ್ಣ *chlg,
                                              ಸಂರಚನಾ digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_message(ಸಂರಚನಾ Curl_easy *data,
                                              ಸಂರಚನಾ Curl_creds *creds,
                                              ಧ್ರುವ unsigned ವರ್ಣ *request,
                                              ಧ್ರುವ unsigned ವರ್ಣ *uripath,
                                              ಸಂರಚನಾ digestdata *digest,
                                              ವರ್ಣ **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
ಶೂನ್ಯ Curl_auth_digest_cleanup(ಸಂರಚನಾ digestdata *digest);
#ಅನ್ಯಥಾ
#define Curl_auth_digest_cleanup(x)
#define Curl_auth_is_digest_supported()       FALSE
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_GSASL

/* meta key for storing GSASL meta at connection */
#define CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#ಅನ್ತರ್ಭಾವ <gsasl.h>
ಸಂರಚನಾ gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

ಸಂರಚನಾ gsasldata *Curl_auth_gsasl_get(ಸಂರಚನಾ connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ವರ್ಣ *mech,
                                  ಸಂರಚನಾ gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_start(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_creds *creds,
                               ಸಂರಚನಾ gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(ಸಂರಚನಾ Curl_easy *data,
                               ಧ್ರುವ ಸಂರಚನಾ bufref *chlg,
                               ಸಂರಚನಾ gsasldata *gsasl,
                               ಸಂರಚನಾ bufref *out);

/* This is used to clean up the gsasl specific data */
ಶೂನ್ಯ Curl_auth_gsasl_cleanup(ಸಂರಚನಾ gsasldata *gsasl);
#endif

#ifdef USE_NTLM

/* meta key for storing NTML meta at connection */
#define CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#define CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

ಸಂರಚನಾ ntlmdata {
#ifdef USE_WINDOWS_SSPI
/* The sslContext is used for the Schannel bindings. The
 * api is available on the Windows 7 SDK and later.
 */
  CtxtHandle *sslContext;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  size_t token_max;
  BYTE *output_token;
  BYTE *ಒಳಹರಿವು_token;
  size_t ಒಳಹರಿವು_token_len;
  TCHAR *spn;
#ಅನ್ಯಥಾ
  unsigned ಪೂರ್ಣಾಙ್ಕ flags;
  unsigned ವರ್ಣ nonce[8];
  unsigned ಪೂರ್ಣಾಙ್ಕ target_info_len;
  ಶೂನ್ಯ *target_info; /* TargetInfo received in the NTLM type-2 message */
#endif
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(ಶೂನ್ಯ);

ಸಂರಚನಾ ntlmdata *Curl_auth_ntlm_get(ಸಂರಚನಾ connectdata *conn, bool proxy);
ಶೂನ್ಯ Curl_auth_ntlm_remove(ಸಂರಚನಾ connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
ಶೂನ್ಯ Curl_auth_cleanup_ntlm(ಸಂರಚನಾ ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_message(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ Curl_creds *creds,
                                             ಧ್ರುವ ವರ್ಣ *default_service,
                                             ಧ್ರುವ ವರ್ಣ *host,
                                             ಸಂರಚನಾ ntlmdata *ntlm,
                                             ಸಂರಚನಾ bufref *out);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_message(ಸಂರಚನಾ Curl_easy *data,
                                             ಧ್ರುವ ಸಂರಚನಾ bufref *type2ref,
                                             ಸಂರಚನಾ ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_message(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ Curl_creds *creds,
                                             ಸಂರಚನಾ ntlmdata *ntlm,
                                             ಸಂರಚನಾ bufref *out);

#ಅನ್ಯಥಾ
#define Curl_auth_is_ntlm_supported()     FALSE
#endif /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_message(ಸಂರಚನಾ Curl_creds *creds,
                                               ಧ್ರುವ ವರ್ಣ *host,
                                               ಧ್ರುವ long port,
                                               ಸಂರಚನಾ bufref *out);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_message(ಸಂರಚನಾ Curl_creds *creds,
                                                ಸಂರಚನಾ bufref *out);

#ifdef USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#define CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

ಸಂರಚನಾ kerberos5data {
#ifdef USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  size_t token_max;
  BYTE *output_token;
#ಅನ್ಯಥಾ
  gss_ctx_id_t context;
  gss_name_t spn;
#endif
};

ಸಂರಚನಾ kerberos5data *Curl_auth_krb5_get(ಸಂರಚನಾ connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(ಶೂನ್ಯ);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
   message */
CURLcode Curl_auth_create_gssapi_user_message(ಸಂರಚನಾ Curl_easy *data,
                                              ಸಂರಚನಾ Curl_creds *creds,
                                              ಧ್ರುವ ವರ್ಣ *default_service,
                                              ಧ್ರುವ ವರ್ಣ *host,
                                              ಧ್ರುವ bool mutual_auth,
                                              ಧ್ರುವ ಸಂರಚನಾ bufref *chlg,
                                              ಸಂರಚನಾ kerberos5data *krb5,
                                              ಸಂರಚನಾ bufref *out);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_message(ಸಂರಚನಾ Curl_easy *data,
                                                  ಧ್ರುವ ವರ್ಣ *authzid,
                                                  ಧ್ರುವ ಸಂರಚನಾ bufref *chlg,
                                                  ಸಂರಚನಾ kerberos5data *krb5,
                                                  ಸಂರಚನಾ bufref *out);

/* This is used to clean up the GSSAPI specific data */
ಶೂನ್ಯ Curl_auth_cleanup_gssapi(ಸಂರಚನಾ kerberos5data *krb5);
#ಅನ್ಯಥಾ
#define Curl_auth_is_gssapi_supported()       FALSE
#endif /* USE_KERBEROS5 */

#ifdef USE_SPNEGO

bool Curl_auth_is_spnego_supported(ಶೂನ್ಯ);

/* meta key for storing NEGO meta at connection */
#define CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#define CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
ಸಂರಚನಾ negotiatedata {
#ifdef HAVE_GSSAPI
  OM_uint32 status;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_buffer_desc output_token;
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  ಸಂರಚನಾ dynbuf channel_binding_data;
#endif
#ಅನ್ಯಥಾ
#ifdef USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
  SECURITY_STATUS status;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  TCHAR *spn;
  size_t token_max;
  BYTE *output_token;
  size_t output_token_length;
#endif
#endif
  BIT(noauthpersist);
  BIT(havenoauthpersist);
  BIT(havenegdata);
  BIT(havemultiplerequests);
};

ಸಂರಚನಾ negotiatedata *Curl_auth_nego_get(ಸಂರಚನಾ connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
   message */
CURLcode Curl_auth_decode_spnego_message(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ Curl_creds *creds,
                                         ಧ್ರುವ ವರ್ಣ *default_service,
                                         ಧ್ರುವ ವರ್ಣ *host,
                                         ಧ್ರುವ ವರ್ಣ *chlg64,
                                         ಸಂರಚನಾ negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
   message */
CURLcode Curl_auth_create_spnego_message(ಸಂರಚನಾ negotiatedata *nego,
                                         ವರ್ಣ **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
ಶೂನ್ಯ Curl_auth_cleanup_spnego(ಸಂರಚನಾ negotiatedata *nego);

#endif /* USE_SPNEGO */

#endif /* HEADER_CURL_VAUTH_H */
