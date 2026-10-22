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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "urldata.h"

ഘടന Curl_easy;
ഘടന Curl_creds;
ഘടന connectdata;
ഘടന Curl_peer;

#ifndef CURL_DISABLE_DIGEST_AUTH
ഘടന digestdata;
#endif

#ifdef USE_NTLM
ഘടന ntlmdata;
#endif

#എങ്കിൽ (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
ഘടന negotiatedata;
#endif

#ifdef USE_GSASL
ഘടന gsasldata;
#endif

#ifdef USE_WINDOWS_SSPI
#അന്തര്ഭാവ "curl_sspi.h"
#define GSS_ERROR(status) ((status) & 0x80000000)
#endif

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_to_host(ഘടന Curl_easy *data);
bool Curl_auth_allowed_to_origin(ഘടന Curl_easy *data,
                                 ഘടന Curl_peer *origin);

/* This is used to build an SPN string */
#ifndef USE_WINDOWS_SSPI
അക്ഷരം *Curl_auth_build_spn(സ്ഥിരം അക്ഷരം *service, സ്ഥിരം അക്ഷരം *host,
                          സ്ഥിരം അക്ഷരം *realm);
#അല്ലെങ്കിൽ
TCHAR *Curl_auth_build_spn(സ്ഥിരം അക്ഷരം *service, സ്ഥിരം അക്ഷരം *host,
                           സ്ഥിരം അക്ഷരം *realm);
#endif

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_contains_domain(ഘടന Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_message(ഘടന Curl_creds *creds,
                                        ഘടന bufref *out);

/* This is used to generate a LOGIN cleartext message */
ശൂന്യം Curl_auth_create_login_message(സ്ഥിരം അക്ഷരം *മൂല്യം, ഘടന bufref *out);

/* This is used to generate an EXTERNAL cleartext message */
ശൂന്യം Curl_auth_create_external_message(സ്ഥിരം അക്ഷരം *user, ഘടന bufref *out);

#ifndef CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_message(സ്ഥിരം ഘടന bufref *chlg,
                                           ഘടന Curl_creds *creds,
                                           ഘടന bufref *out);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(ശൂന്യം);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_message(ഘടന Curl_easy *data,
                                             സ്ഥിരം ഘടന bufref *chlg,
                                             ഘടന Curl_creds *creds,
                                             സ്ഥിരം അക്ഷരം *default_service,
                                             ഘടന bufref *out);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_message(സ്ഥിരം അക്ഷരം *chlg,
                                              ഘടന digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_message(ഘടന Curl_easy *data,
                                              ഘടന Curl_creds *creds,
                                              സ്ഥിരം unsigned അക്ഷരം *request,
                                              സ്ഥിരം unsigned അക്ഷരം *uripath,
                                              ഘടന digestdata *digest,
                                              അക്ഷരം **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
ശൂന്യം Curl_auth_digest_cleanup(ഘടന digestdata *digest);
#അല്ലെങ്കിൽ
#define Curl_auth_digest_cleanup(x)
#define Curl_auth_is_digest_supported()       FALSE
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_GSASL

/* meta key for storing GSASL meta at connection */
#define CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#അന്തര്ഭാവ <gsasl.h>
ഘടന gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

ഘടന gsasldata *Curl_auth_gsasl_get(ഘടന connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(ഘടന Curl_easy *data,
                                  സ്ഥിരം അക്ഷരം *mech,
                                  ഘടന gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_start(ഘടന Curl_easy *data,
                               ഘടന Curl_creds *creds,
                               ഘടന gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(ഘടന Curl_easy *data,
                               സ്ഥിരം ഘടന bufref *chlg,
                               ഘടന gsasldata *gsasl,
                               ഘടന bufref *out);

/* This is used to clean up the gsasl specific data */
ശൂന്യം Curl_auth_gsasl_cleanup(ഘടന gsasldata *gsasl);
#endif

#ifdef USE_NTLM

/* meta key for storing NTML meta at connection */
#define CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#define CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

ഘടന ntlmdata {
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
  BYTE *നിവേശം_token;
  size_t നിവേശം_token_len;
  TCHAR *spn;
#അല്ലെങ്കിൽ
  unsigned പൂർണ്ണസംഖ്യ flags;
  unsigned അക്ഷരം nonce[8];
  unsigned പൂർണ്ണസംഖ്യ target_info_len;
  ശൂന്യം *target_info; /* TargetInfo received in the NTLM type-2 message */
#endif
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(ശൂന്യം);

ഘടന ntlmdata *Curl_auth_ntlm_get(ഘടന connectdata *conn, bool proxy);
ശൂന്യം Curl_auth_ntlm_remove(ഘടന connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
ശൂന്യം Curl_auth_cleanup_ntlm(ഘടന ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_message(ഘടന Curl_easy *data,
                                             ഘടന Curl_creds *creds,
                                             സ്ഥിരം അക്ഷരം *default_service,
                                             സ്ഥിരം അക്ഷരം *host,
                                             ഘടന ntlmdata *ntlm,
                                             ഘടന bufref *out);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_message(ഘടന Curl_easy *data,
                                             സ്ഥിരം ഘടന bufref *type2ref,
                                             ഘടന ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_message(ഘടന Curl_easy *data,
                                             ഘടന Curl_creds *creds,
                                             ഘടന ntlmdata *ntlm,
                                             ഘടന bufref *out);

#അല്ലെങ്കിൽ
#define Curl_auth_is_ntlm_supported()     FALSE
#endif /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_message(ഘടന Curl_creds *creds,
                                               സ്ഥിരം അക്ഷരം *host,
                                               സ്ഥിരം long port,
                                               ഘടന bufref *out);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_message(ഘടന Curl_creds *creds,
                                                ഘടന bufref *out);

#ifdef USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#define CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

ഘടന kerberos5data {
#ifdef USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  size_t token_max;
  BYTE *output_token;
#അല്ലെങ്കിൽ
  gss_ctx_id_t context;
  gss_name_t spn;
#endif
};

ഘടന kerberos5data *Curl_auth_krb5_get(ഘടന connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(ശൂന്യം);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
   message */
CURLcode Curl_auth_create_gssapi_user_message(ഘടന Curl_easy *data,
                                              ഘടന Curl_creds *creds,
                                              സ്ഥിരം അക്ഷരം *default_service,
                                              സ്ഥിരം അക്ഷരം *host,
                                              സ്ഥിരം bool mutual_auth,
                                              സ്ഥിരം ഘടന bufref *chlg,
                                              ഘടന kerberos5data *krb5,
                                              ഘടന bufref *out);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_message(ഘടന Curl_easy *data,
                                                  സ്ഥിരം അക്ഷരം *authzid,
                                                  സ്ഥിരം ഘടന bufref *chlg,
                                                  ഘടന kerberos5data *krb5,
                                                  ഘടന bufref *out);

/* This is used to clean up the GSSAPI specific data */
ശൂന്യം Curl_auth_cleanup_gssapi(ഘടന kerberos5data *krb5);
#അല്ലെങ്കിൽ
#define Curl_auth_is_gssapi_supported()       FALSE
#endif /* USE_KERBEROS5 */

#ifdef USE_SPNEGO

bool Curl_auth_is_spnego_supported(ശൂന്യം);

/* meta key for storing NEGO meta at connection */
#define CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#define CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
ഘടന negotiatedata {
#ifdef HAVE_GSSAPI
  OM_uint32 status;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_buffer_desc output_token;
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  ഘടന dynbuf channel_binding_data;
#endif
#അല്ലെങ്കിൽ
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

ഘടന negotiatedata *Curl_auth_nego_get(ഘടന connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
   message */
CURLcode Curl_auth_decode_spnego_message(ഘടന Curl_easy *data,
                                         ഘടന Curl_creds *creds,
                                         സ്ഥിരം അക്ഷരം *default_service,
                                         സ്ഥിരം അക്ഷരം *host,
                                         സ്ഥിരം അക്ഷരം *chlg64,
                                         ഘടന negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
   message */
CURLcode Curl_auth_create_spnego_message(ഘടന negotiatedata *nego,
                                         അക്ഷരം **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
ശൂന്യം Curl_auth_cleanup_spnego(ഘടന negotiatedata *nego);

#endif /* USE_SPNEGO */

#endif /* HEADER_CURL_VAUTH_H */
