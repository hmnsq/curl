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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"

ସଂରଚନା Curl_easy;
ସଂରଚନା Curl_creds;
ସଂରଚନା connectdata;
ସଂରଚନା Curl_peer;

#ifndef CURL_DISABLE_DIGEST_AUTH
ସଂରଚନା digestdata;
#endif

#ifdef USE_NTLM
ସଂରଚନା ntlmdata;
#endif

#ଯଦି (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
ସଂରଚନା negotiatedata;
#endif

#ifdef USE_GSASL
ସଂରଚନା gsasldata;
#endif

#ifdef USE_WINDOWS_SSPI
#ଅନ୍ତର୍ଭାଵ "curl_sspi.h"
#define GSS_ERROR(status) ((status) & 0x80000000)
#endif

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_to_host(ସଂରଚନା Curl_easy *data);
bool Curl_auth_allowed_to_origin(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା Curl_peer *origin);

/* This is used to build an SPN string */
#ifndef USE_WINDOWS_SSPI
ଅକ୍ଷର *Curl_auth_build_spn(ଅଚଳ ଅକ୍ଷର *service, ଅଚଳ ଅକ୍ଷର *host,
                          ଅଚଳ ଅକ୍ଷର *realm);
#ଅନ୍ୟଥା
TCHAR *Curl_auth_build_spn(ଅଚଳ ଅକ୍ଷର *service, ଅଚଳ ଅକ୍ଷର *host,
                           ଅଚଳ ଅକ୍ଷର *realm);
#endif

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_contains_domain(ସଂରଚନା Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_message(ସଂରଚନା Curl_creds *creds,
                                        ସଂରଚନା bufref *out);

/* This is used to generate a LOGIN cleartext message */
ଶୂନ୍ୟ Curl_auth_create_login_message(ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, ସଂରଚନା bufref *out);

/* This is used to generate an EXTERNAL cleartext message */
ଶୂନ୍ୟ Curl_auth_create_external_message(ଅଚଳ ଅକ୍ଷର *user, ସଂରଚନା bufref *out);

#ifndef CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_message(ଅଚଳ ସଂରଚନା bufref *chlg,
                                           ସଂରଚନା Curl_creds *creds,
                                           ସଂରଚନା bufref *out);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(ଶୂନ୍ୟ);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_message(ସଂରଚନା Curl_easy *data,
                                             ଅଚଳ ସଂରଚନା bufref *chlg,
                                             ସଂରଚନା Curl_creds *creds,
                                             ଅଚଳ ଅକ୍ଷର *default_service,
                                             ସଂରଚନା bufref *out);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_message(ଅଚଳ ଅକ୍ଷର *chlg,
                                              ସଂରଚନା digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_message(ସଂରଚନା Curl_easy *data,
                                              ସଂରଚନା Curl_creds *creds,
                                              ଅଚଳ unsigned ଅକ୍ଷର *request,
                                              ଅଚଳ unsigned ଅକ୍ଷର *uripath,
                                              ସଂରଚନା digestdata *digest,
                                              ଅକ୍ଷର **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
ଶୂନ୍ୟ Curl_auth_digest_cleanup(ସଂରଚନା digestdata *digest);
#ଅନ୍ୟଥା
#define Curl_auth_digest_cleanup(x)
#define Curl_auth_is_digest_supported()       FALSE
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_GSASL

/* meta key for storing GSASL meta at connection */
#define CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#ଅନ୍ତର୍ଭାଵ <gsasl.h>
ସଂରଚନା gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

ସଂରଚନା gsasldata *Curl_auth_gsasl_get(ସଂରଚନା connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ ଅକ୍ଷର *mech,
                                  ସଂରଚନା gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_start(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_creds *creds,
                               ସଂରଚନା gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(ସଂରଚନା Curl_easy *data,
                               ଅଚଳ ସଂରଚନା bufref *chlg,
                               ସଂରଚନା gsasldata *gsasl,
                               ସଂରଚନା bufref *out);

/* This is used to clean up the gsasl specific data */
ଶୂନ୍ୟ Curl_auth_gsasl_cleanup(ସଂରଚନା gsasldata *gsasl);
#endif

#ifdef USE_NTLM

/* meta key for storing NTML meta at connection */
#define CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#define CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

ସଂରଚନା ntlmdata {
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
  BYTE *ନିବେଶ_token;
  size_t ନିବେଶ_token_len;
  TCHAR *spn;
#ଅନ୍ୟଥା
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;
  unsigned ଅକ୍ଷର nonce[8];
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା target_info_len;
  ଶୂନ୍ୟ *target_info; /* TargetInfo received in the NTLM type-2 message */
#endif
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(ଶୂନ୍ୟ);

ସଂରଚନା ntlmdata *Curl_auth_ntlm_get(ସଂରଚନା connectdata *conn, bool proxy);
ଶୂନ୍ୟ Curl_auth_ntlm_remove(ସଂରଚନା connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
ଶୂନ୍ୟ Curl_auth_cleanup_ntlm(ସଂରଚନା ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_message(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା Curl_creds *creds,
                                             ଅଚଳ ଅକ୍ଷର *default_service,
                                             ଅଚଳ ଅକ୍ଷର *host,
                                             ସଂରଚନା ntlmdata *ntlm,
                                             ସଂରଚନା bufref *out);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_message(ସଂରଚନା Curl_easy *data,
                                             ଅଚଳ ସଂରଚନା bufref *type2ref,
                                             ସଂରଚନା ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_message(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା Curl_creds *creds,
                                             ସଂରଚନା ntlmdata *ntlm,
                                             ସଂରଚନା bufref *out);

#ଅନ୍ୟଥା
#define Curl_auth_is_ntlm_supported()     FALSE
#endif /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_message(ସଂରଚନା Curl_creds *creds,
                                               ଅଚଳ ଅକ୍ଷର *host,
                                               ଅଚଳ long port,
                                               ସଂରଚନା bufref *out);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_message(ସଂରଚନା Curl_creds *creds,
                                                ସଂରଚନା bufref *out);

#ifdef USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#define CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

ସଂରଚନା kerberos5data {
#ifdef USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  size_t token_max;
  BYTE *output_token;
#ଅନ୍ୟଥା
  gss_ctx_id_t context;
  gss_name_t spn;
#endif
};

ସଂରଚନା kerberos5data *Curl_auth_krb5_get(ସଂରଚନା connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(ଶୂନ୍ୟ);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
   message */
CURLcode Curl_auth_create_gssapi_user_message(ସଂରଚନା Curl_easy *data,
                                              ସଂରଚନା Curl_creds *creds,
                                              ଅଚଳ ଅକ୍ଷର *default_service,
                                              ଅଚଳ ଅକ୍ଷର *host,
                                              ଅଚଳ bool mutual_auth,
                                              ଅଚଳ ସଂରଚନା bufref *chlg,
                                              ସଂରଚନା kerberos5data *krb5,
                                              ସଂରଚନା bufref *out);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_message(ସଂରଚନା Curl_easy *data,
                                                  ଅଚଳ ଅକ୍ଷର *authzid,
                                                  ଅଚଳ ସଂରଚନା bufref *chlg,
                                                  ସଂରଚନା kerberos5data *krb5,
                                                  ସଂରଚନା bufref *out);

/* This is used to clean up the GSSAPI specific data */
ଶୂନ୍ୟ Curl_auth_cleanup_gssapi(ସଂରଚନା kerberos5data *krb5);
#ଅନ୍ୟଥା
#define Curl_auth_is_gssapi_supported()       FALSE
#endif /* USE_KERBEROS5 */

#ifdef USE_SPNEGO

bool Curl_auth_is_spnego_supported(ଶୂନ୍ୟ);

/* meta key for storing NEGO meta at connection */
#define CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#define CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
ସଂରଚନା negotiatedata {
#ifdef HAVE_GSSAPI
  OM_uint32 status;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_buffer_desc output_token;
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  ସଂରଚନା dynbuf channel_binding_data;
#endif
#ଅନ୍ୟଥା
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

ସଂରଚନା negotiatedata *Curl_auth_nego_get(ସଂରଚନା connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
   message */
CURLcode Curl_auth_decode_spnego_message(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା Curl_creds *creds,
                                         ଅଚଳ ଅକ୍ଷର *default_service,
                                         ଅଚଳ ଅକ୍ଷର *host,
                                         ଅଚଳ ଅକ୍ଷର *chlg64,
                                         ସଂରଚନା negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
   message */
CURLcode Curl_auth_create_spnego_message(ସଂରଚନା negotiatedata *nego,
                                         ଅକ୍ଷର **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
ଶୂନ୍ୟ Curl_auth_cleanup_spnego(ସଂରଚନା negotiatedata *nego);

#endif /* USE_SPNEGO */

#endif /* HEADER_CURL_VAUTH_H */
