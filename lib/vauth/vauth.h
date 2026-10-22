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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "bufref.h"
#অন্তর্ভাৱ "curlx/dynbuf.h"
#অন্তর্ভাৱ "urldata.h"

সংরচনা Curl_easy;
সংরচনা Curl_creds;
সংরচনা connectdata;
সংরচনা Curl_peer;

#ifndef CURL_DISABLE_DIGEST_AUTH
সংরচনা digestdata;
#endif

#ifdef USE_NTLM
সংরচনা ntlmdata;
#endif

#যদি (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
সংরচনা negotiatedata;
#endif

#ifdef USE_GSASL
সংরচনা gsasldata;
#endif

#ifdef USE_WINDOWS_SSPI
#অন্তর্ভাৱ "curl_sspi.h"
#define GSS_ERROR(status) ((status) & 0x80000000)
#endif

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_to_host(সংরচনা Curl_easy *data);
bool Curl_auth_allowed_to_origin(সংরচনা Curl_easy *data,
                                 সংরচনা Curl_peer *origin);

/* This is used to build an SPN string */
#ifndef USE_WINDOWS_SSPI
ৱর্ণ *Curl_auth_build_spn(ধ্রুৱ ৱর্ণ *service, ধ্রুৱ ৱর্ণ *host,
                          ধ্রুৱ ৱর্ণ *realm);
#অন্যথা
TCHAR *Curl_auth_build_spn(ধ্রুৱ ৱর্ণ *service, ধ্রুৱ ৱর্ণ *host,
                           ধ্রুৱ ৱর্ণ *realm);
#endif

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_contains_domain(সংরচনা Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_message(সংরচনা Curl_creds *creds,
                                        সংরচনা bufref *out);

/* This is used to generate a LOGIN cleartext message */
শূন্য Curl_auth_create_login_message(ধ্রুৱ ৱর্ণ *মান, সংরচনা bufref *out);

/* This is used to generate an EXTERNAL cleartext message */
শূন্য Curl_auth_create_external_message(ধ্রুৱ ৱর্ণ *user, সংরচনা bufref *out);

#ifndef CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_message(ধ্রুৱ সংরচনা bufref *chlg,
                                           সংরচনা Curl_creds *creds,
                                           সংরচনা bufref *out);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(শূন্য);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_message(সংরচনা Curl_easy *data,
                                             ধ্রুৱ সংরচনা bufref *chlg,
                                             সংরচনা Curl_creds *creds,
                                             ধ্রুৱ ৱর্ণ *default_service,
                                             সংরচনা bufref *out);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_message(ধ্রুৱ ৱর্ণ *chlg,
                                              সংরচনা digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_message(সংরচনা Curl_easy *data,
                                              সংরচনা Curl_creds *creds,
                                              ধ্রুৱ unsigned ৱর্ণ *request,
                                              ধ্রুৱ unsigned ৱর্ণ *uripath,
                                              সংরচনা digestdata *digest,
                                              ৱর্ণ **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
শূন্য Curl_auth_digest_cleanup(সংরচনা digestdata *digest);
#অন্যথা
#define Curl_auth_digest_cleanup(x)
#define Curl_auth_is_digest_supported()       FALSE
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_GSASL

/* meta key for storing GSASL meta at connection */
#define CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#অন্তর্ভাৱ <gsasl.h>
সংরচনা gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

সংরচনা gsasldata *Curl_auth_gsasl_get(সংরচনা connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(সংরচনা Curl_easy *data,
                                  ধ্রুৱ ৱর্ণ *mech,
                                  সংরচনা gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_start(সংরচনা Curl_easy *data,
                               সংরচনা Curl_creds *creds,
                               সংরচনা gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(সংরচনা Curl_easy *data,
                               ধ্রুৱ সংরচনা bufref *chlg,
                               সংরচনা gsasldata *gsasl,
                               সংরচনা bufref *out);

/* This is used to clean up the gsasl specific data */
শূন্য Curl_auth_gsasl_cleanup(সংরচনা gsasldata *gsasl);
#endif

#ifdef USE_NTLM

/* meta key for storing NTML meta at connection */
#define CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#define CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

সংরচনা ntlmdata {
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
  BYTE *প্ৰবেশ_token;
  size_t প্ৰবেশ_token_len;
  TCHAR *spn;
#অন্যথা
  unsigned পূর্ণাঙ্ক flags;
  unsigned ৱর্ণ nonce[8];
  unsigned পূর্ণাঙ্ক target_info_len;
  শূন্য *target_info; /* TargetInfo received in the NTLM type-2 message */
#endif
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(শূন্য);

সংরচনা ntlmdata *Curl_auth_ntlm_get(সংরচনা connectdata *conn, bool proxy);
শূন্য Curl_auth_ntlm_remove(সংরচনা connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
শূন্য Curl_auth_cleanup_ntlm(সংরচনা ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_message(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_creds *creds,
                                             ধ্রুৱ ৱর্ণ *default_service,
                                             ধ্রুৱ ৱর্ণ *host,
                                             সংরচনা ntlmdata *ntlm,
                                             সংরচনা bufref *out);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_message(সংরচনা Curl_easy *data,
                                             ধ্রুৱ সংরচনা bufref *type2ref,
                                             সংরচনা ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_message(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_creds *creds,
                                             সংরচনা ntlmdata *ntlm,
                                             সংরচনা bufref *out);

#অন্যথা
#define Curl_auth_is_ntlm_supported()     FALSE
#endif /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_message(সংরচনা Curl_creds *creds,
                                               ধ্রুৱ ৱর্ণ *host,
                                               ধ্রুৱ long port,
                                               সংরচনা bufref *out);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_message(সংরচনা Curl_creds *creds,
                                                সংরচনা bufref *out);

#ifdef USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#define CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

সংরচনা kerberos5data {
#ifdef USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  size_t token_max;
  BYTE *output_token;
#অন্যথা
  gss_ctx_id_t context;
  gss_name_t spn;
#endif
};

সংরচনা kerberos5data *Curl_auth_krb5_get(সংরচনা connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(শূন্য);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
   message */
CURLcode Curl_auth_create_gssapi_user_message(সংরচনা Curl_easy *data,
                                              সংরচনা Curl_creds *creds,
                                              ধ্রুৱ ৱর্ণ *default_service,
                                              ধ্রুৱ ৱর্ণ *host,
                                              ধ্রুৱ bool mutual_auth,
                                              ধ্রুৱ সংরচনা bufref *chlg,
                                              সংরচনা kerberos5data *krb5,
                                              সংরচনা bufref *out);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_message(সংরচনা Curl_easy *data,
                                                  ধ্রুৱ ৱর্ণ *authzid,
                                                  ধ্রুৱ সংরচনা bufref *chlg,
                                                  সংরচনা kerberos5data *krb5,
                                                  সংরচনা bufref *out);

/* This is used to clean up the GSSAPI specific data */
শূন্য Curl_auth_cleanup_gssapi(সংরচনা kerberos5data *krb5);
#অন্যথা
#define Curl_auth_is_gssapi_supported()       FALSE
#endif /* USE_KERBEROS5 */

#ifdef USE_SPNEGO

bool Curl_auth_is_spnego_supported(শূন্য);

/* meta key for storing NEGO meta at connection */
#define CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#define CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
সংরচনা negotiatedata {
#ifdef HAVE_GSSAPI
  OM_uint32 status;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_buffer_desc output_token;
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  সংরচনা dynbuf channel_binding_data;
#endif
#অন্যথা
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

সংরচনা negotiatedata *Curl_auth_nego_get(সংরচনা connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
   message */
CURLcode Curl_auth_decode_spnego_message(সংরচনা Curl_easy *data,
                                         সংরচনা Curl_creds *creds,
                                         ধ্রুৱ ৱর্ণ *default_service,
                                         ধ্রুৱ ৱর্ণ *host,
                                         ধ্রুৱ ৱর্ণ *chlg64,
                                         সংরচনা negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
   message */
CURLcode Curl_auth_create_spnego_message(সংরচনা negotiatedata *nego,
                                         ৱর্ণ **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
শূন্য Curl_auth_cleanup_spnego(সংরচনা negotiatedata *nego);

#endif /* USE_SPNEGO */

#endif /* HEADER_CURL_VAUTH_H */
