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
#شامل "curl_setup.h"

#شامل "bufref.h"
#شامل "curlx/dynbuf.h"
#شامل "urldata.h"

ساختار Curl_easy;
ساختار Curl_creds;
ساختار connectdata;
ساختار Curl_peer;

#ifndef CURL_DISABLE_DIGEST_AUTH
ساختار digestdata;
#endif

#ifdef USE_NTLM
ساختار ntlmdata;
#endif

#اگر (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
ساختار negotiatedata;
#endif

#ifdef USE_GSASL
ساختار gsasldata;
#endif

#ifdef USE_WINDOWS_SSPI
#شامل "curl_sspi.h"
#define GSS_ERROR(status) ((status) & 0x80000000)
#endif

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_to_host(ساختار Curl_easy *data);
bool Curl_auth_allowed_to_origin(ساختار Curl_easy *data,
                                 ساختار Curl_peer *origin);

/* This is used to build an SPN string */
#ifndef USE_WINDOWS_SSPI
نویسه *Curl_auth_build_spn(ثابت نویسه *service, ثابت نویسه *host,
                          ثابت نویسه *realm);
#وگرنه
TCHAR *Curl_auth_build_spn(ثابت نویسه *service, ثابت نویسه *host,
                           ثابت نویسه *realm);
#endif

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_contains_domain(ساختار Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_message(ساختار Curl_creds *creds,
                                        ساختار bufref *out);

/* This is used to generate a LOGIN cleartext message */
تهی Curl_auth_create_login_message(ثابت نویسه *مقدار, ساختار bufref *out);

/* This is used to generate an EXTERNAL cleartext message */
تهی Curl_auth_create_external_message(ثابت نویسه *user, ساختار bufref *out);

#ifndef CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_message(ثابت ساختار bufref *chlg,
                                           ساختار Curl_creds *creds,
                                           ساختار bufref *out);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(تهی);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_message(ساختار Curl_easy *data,
                                             ثابت ساختار bufref *chlg,
                                             ساختار Curl_creds *creds,
                                             ثابت نویسه *default_service,
                                             ساختار bufref *out);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_message(ثابت نویسه *chlg,
                                              ساختار digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_message(ساختار Curl_easy *data,
                                              ساختار Curl_creds *creds,
                                              ثابت unsigned نویسه *request,
                                              ثابت unsigned نویسه *uripath,
                                              ساختار digestdata *digest,
                                              نویسه **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
تهی Curl_auth_digest_cleanup(ساختار digestdata *digest);
#وگرنه
#define Curl_auth_digest_cleanup(x)
#define Curl_auth_is_digest_supported()       FALSE
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_GSASL

/* meta key for storing GSASL meta at connection */
#define CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#شامل <gsasl.h>
ساختار gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

ساختار gsasldata *Curl_auth_gsasl_get(ساختار connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(ساختار Curl_easy *data,
                                  ثابت نویسه *mech,
                                  ساختار gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_start(ساختار Curl_easy *data,
                               ساختار Curl_creds *creds,
                               ساختار gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(ساختار Curl_easy *data,
                               ثابت ساختار bufref *chlg,
                               ساختار gsasldata *gsasl,
                               ساختار bufref *out);

/* This is used to clean up the gsasl specific data */
تهی Curl_auth_gsasl_cleanup(ساختار gsasldata *gsasl);
#endif

#ifdef USE_NTLM

/* meta key for storing NTML meta at connection */
#define CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#define CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

ساختار ntlmdata {
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
  BYTE *درون_داد_token;
  size_t درون_داد_token_len;
  TCHAR *spn;
#وگرنه
  unsigned صحیح flags;
  unsigned نویسه nonce[8];
  unsigned صحیح target_info_len;
  تهی *target_info; /* TargetInfo received in the NTLM type-2 message */
#endif
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(تهی);

ساختار ntlmdata *Curl_auth_ntlm_get(ساختار connectdata *conn, bool proxy);
تهی Curl_auth_ntlm_remove(ساختار connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
تهی Curl_auth_cleanup_ntlm(ساختار ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_message(ساختار Curl_easy *data,
                                             ساختار Curl_creds *creds,
                                             ثابت نویسه *default_service,
                                             ثابت نویسه *host,
                                             ساختار ntlmdata *ntlm,
                                             ساختار bufref *out);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_message(ساختار Curl_easy *data,
                                             ثابت ساختار bufref *type2ref,
                                             ساختار ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_message(ساختار Curl_easy *data,
                                             ساختار Curl_creds *creds,
                                             ساختار ntlmdata *ntlm,
                                             ساختار bufref *out);

#وگرنه
#define Curl_auth_is_ntlm_supported()     FALSE
#endif /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_message(ساختار Curl_creds *creds,
                                               ثابت نویسه *host,
                                               ثابت long port,
                                               ساختار bufref *out);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_message(ساختار Curl_creds *creds,
                                                ساختار bufref *out);

#ifdef USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#define CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

ساختار kerberos5data {
#ifdef USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  size_t token_max;
  BYTE *output_token;
#وگرنه
  gss_ctx_id_t context;
  gss_name_t spn;
#endif
};

ساختار kerberos5data *Curl_auth_krb5_get(ساختار connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(تهی);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
   message */
CURLcode Curl_auth_create_gssapi_user_message(ساختار Curl_easy *data,
                                              ساختار Curl_creds *creds,
                                              ثابت نویسه *default_service,
                                              ثابت نویسه *host,
                                              ثابت bool mutual_auth,
                                              ثابت ساختار bufref *chlg,
                                              ساختار kerberos5data *krb5,
                                              ساختار bufref *out);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_message(ساختار Curl_easy *data,
                                                  ثابت نویسه *authzid,
                                                  ثابت ساختار bufref *chlg,
                                                  ساختار kerberos5data *krb5,
                                                  ساختار bufref *out);

/* This is used to clean up the GSSAPI specific data */
تهی Curl_auth_cleanup_gssapi(ساختار kerberos5data *krb5);
#وگرنه
#define Curl_auth_is_gssapi_supported()       FALSE
#endif /* USE_KERBEROS5 */

#ifdef USE_SPNEGO

bool Curl_auth_is_spnego_supported(تهی);

/* meta key for storing NEGO meta at connection */
#define CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#define CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
ساختار negotiatedata {
#ifdef HAVE_GSSAPI
  OM_uint32 status;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_buffer_desc output_token;
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  ساختار dynbuf channel_binding_data;
#endif
#وگرنه
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

ساختار negotiatedata *Curl_auth_nego_get(ساختار connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
   message */
CURLcode Curl_auth_decode_spnego_message(ساختار Curl_easy *data,
                                         ساختار Curl_creds *creds,
                                         ثابت نویسه *default_service,
                                         ثابت نویسه *host,
                                         ثابت نویسه *chlg64,
                                         ساختار negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
   message */
CURLcode Curl_auth_create_spnego_message(ساختار negotiatedata *nego,
                                         نویسه **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
تهی Curl_auth_cleanup_spnego(ساختار negotiatedata *nego);

#endif /* USE_SPNEGO */

#endif /* HEADER_CURL_VAUTH_H */
