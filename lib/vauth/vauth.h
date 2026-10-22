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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "urldata.h"

கட்டமைப்பு Curl_easy;
கட்டமைப்பு Curl_creds;
கட்டமைப்பு connectdata;
கட்டமைப்பு Curl_peer;

#ifndef CURL_DISABLE_DIGEST_AUTH
கட்டமைப்பு digestdata;
#endif

#ifdef USE_NTLM
கட்டமைப்பு ntlmdata;
#endif

#எனில் (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
கட்டமைப்பு negotiatedata;
#endif

#ifdef USE_GSASL
கட்டமைப்பு gsasldata;
#endif

#ifdef USE_WINDOWS_SSPI
#அந்தர்பாவ "curl_sspi.h"
#define GSS_ERROR(status) ((status) & 0x80000000)
#endif

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_to_host(கட்டமைப்பு Curl_easy *data);
bool Curl_auth_allowed_to_origin(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு Curl_peer *origin);

/* This is used to build an SPN string */
#ifndef USE_WINDOWS_SSPI
எழுத்து *Curl_auth_build_spn(மாறிலி எழுத்து *service, மாறிலி எழுத்து *host,
                          மாறிலி எழுத்து *realm);
#இல்லையெனில்
TCHAR *Curl_auth_build_spn(மாறிலி எழுத்து *service, மாறிலி எழுத்து *host,
                           மாறிலி எழுத்து *realm);
#endif

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_contains_domain(கட்டமைப்பு Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_message(கட்டமைப்பு Curl_creds *creds,
                                        கட்டமைப்பு bufref *out);

/* This is used to generate a LOGIN cleartext message */
வெறுமை Curl_auth_create_login_message(மாறிலி எழுத்து *மதிப்பு, கட்டமைப்பு bufref *out);

/* This is used to generate an EXTERNAL cleartext message */
வெறுமை Curl_auth_create_external_message(மாறிலி எழுத்து *user, கட்டமைப்பு bufref *out);

#ifndef CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_message(மாறிலி கட்டமைப்பு bufref *chlg,
                                           கட்டமைப்பு Curl_creds *creds,
                                           கட்டமைப்பு bufref *out);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(வெறுமை);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_message(கட்டமைப்பு Curl_easy *data,
                                             மாறிலி கட்டமைப்பு bufref *chlg,
                                             கட்டமைப்பு Curl_creds *creds,
                                             மாறிலி எழுத்து *default_service,
                                             கட்டமைப்பு bufref *out);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_message(மாறிலி எழுத்து *chlg,
                                              கட்டமைப்பு digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_message(கட்டமைப்பு Curl_easy *data,
                                              கட்டமைப்பு Curl_creds *creds,
                                              மாறிலி unsigned எழுத்து *request,
                                              மாறிலி unsigned எழுத்து *uripath,
                                              கட்டமைப்பு digestdata *digest,
                                              எழுத்து **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
வெறுமை Curl_auth_digest_cleanup(கட்டமைப்பு digestdata *digest);
#இல்லையெனில்
#define Curl_auth_digest_cleanup(x)
#define Curl_auth_is_digest_supported()       FALSE
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_GSASL

/* meta key for storing GSASL meta at connection */
#define CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#அந்தர்பாவ <gsasl.h>
கட்டமைப்பு gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

கட்டமைப்பு gsasldata *Curl_auth_gsasl_get(கட்டமைப்பு connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி எழுத்து *mech,
                                  கட்டமைப்பு gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_start(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_creds *creds,
                               கட்டமைப்பு gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(கட்டமைப்பு Curl_easy *data,
                               மாறிலி கட்டமைப்பு bufref *chlg,
                               கட்டமைப்பு gsasldata *gsasl,
                               கட்டமைப்பு bufref *out);

/* This is used to clean up the gsasl specific data */
வெறுமை Curl_auth_gsasl_cleanup(கட்டமைப்பு gsasldata *gsasl);
#endif

#ifdef USE_NTLM

/* meta key for storing NTML meta at connection */
#define CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#define CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

கட்டமைப்பு ntlmdata {
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
  BYTE *உள்ளீடு_token;
  size_t உள்ளீடு_token_len;
  TCHAR *spn;
#இல்லையெனில்
  unsigned முழு flags;
  unsigned எழுத்து nonce[8];
  unsigned முழு target_info_len;
  வெறுமை *target_info; /* TargetInfo received in the NTLM type-2 message */
#endif
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(வெறுமை);

கட்டமைப்பு ntlmdata *Curl_auth_ntlm_get(கட்டமைப்பு connectdata *conn, bool proxy);
வெறுமை Curl_auth_ntlm_remove(கட்டமைப்பு connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
வெறுமை Curl_auth_cleanup_ntlm(கட்டமைப்பு ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_message(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு Curl_creds *creds,
                                             மாறிலி எழுத்து *default_service,
                                             மாறிலி எழுத்து *host,
                                             கட்டமைப்பு ntlmdata *ntlm,
                                             கட்டமைப்பு bufref *out);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_message(கட்டமைப்பு Curl_easy *data,
                                             மாறிலி கட்டமைப்பு bufref *type2ref,
                                             கட்டமைப்பு ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_message(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு Curl_creds *creds,
                                             கட்டமைப்பு ntlmdata *ntlm,
                                             கட்டமைப்பு bufref *out);

#இல்லையெனில்
#define Curl_auth_is_ntlm_supported()     FALSE
#endif /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_message(கட்டமைப்பு Curl_creds *creds,
                                               மாறிலி எழுத்து *host,
                                               மாறிலி long port,
                                               கட்டமைப்பு bufref *out);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_message(கட்டமைப்பு Curl_creds *creds,
                                                கட்டமைப்பு bufref *out);

#ifdef USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#define CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

கட்டமைப்பு kerberos5data {
#ifdef USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  size_t token_max;
  BYTE *output_token;
#இல்லையெனில்
  gss_ctx_id_t context;
  gss_name_t spn;
#endif
};

கட்டமைப்பு kerberos5data *Curl_auth_krb5_get(கட்டமைப்பு connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(வெறுமை);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
   message */
CURLcode Curl_auth_create_gssapi_user_message(கட்டமைப்பு Curl_easy *data,
                                              கட்டமைப்பு Curl_creds *creds,
                                              மாறிலி எழுத்து *default_service,
                                              மாறிலி எழுத்து *host,
                                              மாறிலி bool mutual_auth,
                                              மாறிலி கட்டமைப்பு bufref *chlg,
                                              கட்டமைப்பு kerberos5data *krb5,
                                              கட்டமைப்பு bufref *out);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_message(கட்டமைப்பு Curl_easy *data,
                                                  மாறிலி எழுத்து *authzid,
                                                  மாறிலி கட்டமைப்பு bufref *chlg,
                                                  கட்டமைப்பு kerberos5data *krb5,
                                                  கட்டமைப்பு bufref *out);

/* This is used to clean up the GSSAPI specific data */
வெறுமை Curl_auth_cleanup_gssapi(கட்டமைப்பு kerberos5data *krb5);
#இல்லையெனில்
#define Curl_auth_is_gssapi_supported()       FALSE
#endif /* USE_KERBEROS5 */

#ifdef USE_SPNEGO

bool Curl_auth_is_spnego_supported(வெறுமை);

/* meta key for storing NEGO meta at connection */
#define CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#define CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
கட்டமைப்பு negotiatedata {
#ifdef HAVE_GSSAPI
  OM_uint32 status;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_buffer_desc output_token;
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  கட்டமைப்பு dynbuf channel_binding_data;
#endif
#இல்லையெனில்
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

கட்டமைப்பு negotiatedata *Curl_auth_nego_get(கட்டமைப்பு connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
   message */
CURLcode Curl_auth_decode_spnego_message(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு Curl_creds *creds,
                                         மாறிலி எழுத்து *default_service,
                                         மாறிலி எழுத்து *host,
                                         மாறிலி எழுத்து *chlg64,
                                         கட்டமைப்பு negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
   message */
CURLcode Curl_auth_create_spnego_message(கட்டமைப்பு negotiatedata *nego,
                                         எழுத்து **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
வெறுமை Curl_auth_cleanup_spnego(கட்டமைப்பு negotiatedata *nego);

#endif /* USE_SPNEGO */

#endif /* HEADER_CURL_VAUTH_H */
