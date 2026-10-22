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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "urldata.h"

સંરચના Curl_easy;
સંરચના Curl_creds;
સંરચના connectdata;
સંરચના Curl_peer;

#ifndef CURL_DISABLE_DIGEST_AUTH
સંરચના digestdata;
#endif

#ifdef USE_NTLM
સંરચના ntlmdata;
#endif

#જો (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
સંરચના negotiatedata;
#endif

#ifdef USE_GSASL
સંરચના gsasldata;
#endif

#ifdef USE_WINDOWS_SSPI
#અન્તર્ભાવ "curl_sspi.h"
#define GSS_ERROR(status) ((status) & 0x80000000)
#endif

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_to_host(સંરચના Curl_easy *data);
bool Curl_auth_allowed_to_origin(સંરચના Curl_easy *data,
                                 સંરચના Curl_peer *origin);

/* This is used to build an SPN string */
#ifndef USE_WINDOWS_SSPI
અક્ષર *Curl_auth_build_spn(અચલ અક્ષર *service, અચલ અક્ષર *host,
                          અચલ અક્ષર *realm);
#નહીંતર
TCHAR *Curl_auth_build_spn(અચલ અક્ષર *service, અચલ અક્ષર *host,
                           અચલ અક્ષર *realm);
#endif

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_contains_domain(સંરચના Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_message(સંરચના Curl_creds *creds,
                                        સંરચના bufref *out);

/* This is used to generate a LOGIN cleartext message */
શૂન્ય Curl_auth_create_login_message(અચલ અક્ષર *કિંમત, સંરચના bufref *out);

/* This is used to generate an EXTERNAL cleartext message */
શૂન્ય Curl_auth_create_external_message(અચલ અક્ષર *user, સંરચના bufref *out);

#ifndef CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_message(અચલ સંરચના bufref *chlg,
                                           સંરચના Curl_creds *creds,
                                           સંરચના bufref *out);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(શૂન્ય);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_message(સંરચના Curl_easy *data,
                                             અચલ સંરચના bufref *chlg,
                                             સંરચના Curl_creds *creds,
                                             અચલ અક્ષર *default_service,
                                             સંરચના bufref *out);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_message(અચલ અક્ષર *chlg,
                                              સંરચના digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_message(સંરચના Curl_easy *data,
                                              સંરચના Curl_creds *creds,
                                              અચલ unsigned અક્ષર *request,
                                              અચલ unsigned અક્ષર *uripath,
                                              સંરચના digestdata *digest,
                                              અક્ષર **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
શૂન્ય Curl_auth_digest_cleanup(સંરચના digestdata *digest);
#નહીંતર
#define Curl_auth_digest_cleanup(x)
#define Curl_auth_is_digest_supported()       FALSE
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_GSASL

/* meta key for storing GSASL meta at connection */
#define CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#અન્તર્ભાવ <gsasl.h>
સંરચના gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

સંરચના gsasldata *Curl_auth_gsasl_get(સંરચના connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(સંરચના Curl_easy *data,
                                  અચલ અક્ષર *mech,
                                  સંરચના gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_start(સંરચના Curl_easy *data,
                               સંરચના Curl_creds *creds,
                               સંરચના gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(સંરચના Curl_easy *data,
                               અચલ સંરચના bufref *chlg,
                               સંરચના gsasldata *gsasl,
                               સંરચના bufref *out);

/* This is used to clean up the gsasl specific data */
શૂન્ય Curl_auth_gsasl_cleanup(સંરચના gsasldata *gsasl);
#endif

#ifdef USE_NTLM

/* meta key for storing NTML meta at connection */
#define CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#define CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

સંરચના ntlmdata {
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
  BYTE *આગત_token;
  size_t આગત_token_len;
  TCHAR *spn;
#નહીંતર
  unsigned પૂર્ણાંક flags;
  unsigned અક્ષર nonce[8];
  unsigned પૂર્ણાંક target_info_len;
  શૂન્ય *target_info; /* TargetInfo received in the NTLM type-2 message */
#endif
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(શૂન્ય);

સંરચના ntlmdata *Curl_auth_ntlm_get(સંરચના connectdata *conn, bool proxy);
શૂન્ય Curl_auth_ntlm_remove(સંરચના connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
શૂન્ય Curl_auth_cleanup_ntlm(સંરચના ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_message(સંરચના Curl_easy *data,
                                             સંરચના Curl_creds *creds,
                                             અચલ અક્ષર *default_service,
                                             અચલ અક્ષર *host,
                                             સંરચના ntlmdata *ntlm,
                                             સંરચના bufref *out);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_message(સંરચના Curl_easy *data,
                                             અચલ સંરચના bufref *type2ref,
                                             સંરચના ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_message(સંરચના Curl_easy *data,
                                             સંરચના Curl_creds *creds,
                                             સંરચના ntlmdata *ntlm,
                                             સંરચના bufref *out);

#નહીંતર
#define Curl_auth_is_ntlm_supported()     FALSE
#endif /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_message(સંરચના Curl_creds *creds,
                                               અચલ અક્ષર *host,
                                               અચલ long port,
                                               સંરચના bufref *out);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_message(સંરચના Curl_creds *creds,
                                                સંરચના bufref *out);

#ifdef USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#define CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

સંરચના kerberos5data {
#ifdef USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  size_t token_max;
  BYTE *output_token;
#નહીંતર
  gss_ctx_id_t context;
  gss_name_t spn;
#endif
};

સંરચના kerberos5data *Curl_auth_krb5_get(સંરચના connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(શૂન્ય);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
   message */
CURLcode Curl_auth_create_gssapi_user_message(સંરચના Curl_easy *data,
                                              સંરચના Curl_creds *creds,
                                              અચલ અક્ષર *default_service,
                                              અચલ અક્ષર *host,
                                              અચલ bool mutual_auth,
                                              અચલ સંરચના bufref *chlg,
                                              સંરચના kerberos5data *krb5,
                                              સંરચના bufref *out);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_message(સંરચના Curl_easy *data,
                                                  અચલ અક્ષર *authzid,
                                                  અચલ સંરચના bufref *chlg,
                                                  સંરચના kerberos5data *krb5,
                                                  સંરચના bufref *out);

/* This is used to clean up the GSSAPI specific data */
શૂન્ય Curl_auth_cleanup_gssapi(સંરચના kerberos5data *krb5);
#નહીંતર
#define Curl_auth_is_gssapi_supported()       FALSE
#endif /* USE_KERBEROS5 */

#ifdef USE_SPNEGO

bool Curl_auth_is_spnego_supported(શૂન્ય);

/* meta key for storing NEGO meta at connection */
#define CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#define CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
સંરચના negotiatedata {
#ifdef HAVE_GSSAPI
  OM_uint32 status;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_buffer_desc output_token;
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  સંરચના dynbuf channel_binding_data;
#endif
#નહીંતર
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

સંરચના negotiatedata *Curl_auth_nego_get(સંરચના connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
   message */
CURLcode Curl_auth_decode_spnego_message(સંરચના Curl_easy *data,
                                         સંરચના Curl_creds *creds,
                                         અચલ અક્ષર *default_service,
                                         અચલ અક્ષર *host,
                                         અચલ અક્ષર *chlg64,
                                         સંરચના negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
   message */
CURLcode Curl_auth_create_spnego_message(સંરચના negotiatedata *nego,
                                         અક્ષર **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
શૂન્ય Curl_auth_cleanup_spnego(સંરચના negotiatedata *nego);

#endif /* USE_SPNEGO */

#endif /* HEADER_CURL_VAUTH_H */
