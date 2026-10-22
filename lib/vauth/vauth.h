#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VAUTH_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VAUTH_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

ਬਣਤਰ Curl_easy;
ਬਣਤਰ Curl_creds;
ਬਣਤਰ connectdata;
ਬਣਤਰ Curl_peer;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
ਬਣਤਰ digestdata;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
ਬਣਤਰ ntlmdata;
#ਜੇ_ਅੰਤ

#ਜੇ (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
ਬਣਤਰ negotiatedata;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL
ਬਣਤਰ gsasldata;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sspi.h"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GSS_ERROR(ਸਥਿਤੀ) ((ਸਥਿਤੀ) & 0x80000000)
#ਜੇ_ਅੰਤ

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_ਮੰਜ਼ਿਲ_host(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
bool Curl_auth_allowed_ਮੰਜ਼ਿਲ_origin(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ Curl_peer *origin);

/* This is used to build an SPN string */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
ਅੱਖਰ *Curl_auth_build_spn(ਅਚਲ ਅੱਖਰ *service, ਅਚਲ ਅੱਖਰ *host,
                          ਅਚਲ ਅੱਖਰ *realm);
#ਨਹੀਂਤਾਂ
TCHAR *Curl_auth_build_spn(ਅਚਲ ਅੱਖਰ *service, ਅਚਲ ਅੱਖਰ *host,
                           ਅਚਲ ਅੱਖਰ *realm);
#ਜੇ_ਅੰਤ

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_ਸ਼ਾਮਲ_ਹੈ_domain(ਬਣਤਰ Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_creds *creds,
                                        ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

/* This is used to generate a LOGIN cleartext message */
ਖਾਲੀ Curl_auth_create_login_ਸੁਨੇਹਾ(ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

/* This is used to generate an EXTERNAL cleartext message */
ਖਾਲੀ Curl_auth_create_external_ਸੁਨੇਹਾ(ਅਚਲ ਅੱਖਰ *user, ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_ਸੁਨੇਹਾ(ਅਚਲ ਬਣਤਰ bufref *chlg,
                                           ਬਣਤਰ Curl_creds *creds,
                                           ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(ਖਾਲੀ);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਅਚਲ ਬਣਤਰ bufref *chlg,
                                             ਬਣਤਰ Curl_creds *creds,
                                             ਅਚਲ ਅੱਖਰ *default_service,
                                             ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_ਸੁਨੇਹਾ(ਅਚਲ ਅੱਖਰ *chlg,
                                              ਬਣਤਰ digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਬਣਤਰ Curl_creds *creds,
                                              ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *request,
                                              ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *uripath,
                                              ਬਣਤਰ digestdata *digest,
                                              ਅੱਖਰ **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
ਖਾਲੀ Curl_auth_digest_cleanup(ਬਣਤਰ digestdata *digest);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_auth_digest_cleanup(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_auth_is_digest_supported()       FALSE
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_DIGEST_AUTH */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL

/* meta key for storing GSASL meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#ਅਨ੍ਤਰ੍ਭਾਵ <gsasl.h>
ਬਣਤਰ gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

ਬਣਤਰ gsasldata *Curl_auth_gsasl_get(ਬਣਤਰ connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *mech,
                                  ਬਣਤਰ gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_creds *creds,
                               ਬਣਤਰ gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅਚਲ ਬਣਤਰ bufref *chlg,
                               ਬਣਤਰ gsasldata *gsasl,
                               ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

/* This is used to clean up the gsasl specific data */
ਖਾਲੀ Curl_auth_gsasl_cleanup(ਬਣਤਰ gsasldata *gsasl);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM

/* meta key for storing NTML meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

ਬਣਤਰ ntlmdata {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
/* The sslContext is used for the Schannel bindings. The
 * api is available on the Windows 7 SDK and later.
 */
  CtxtHandle *sslContext;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX ਪਛਾਣ_ਤੱਤ;
  SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX *p_ਪਛਾਣ_ਤੱਤ;
  size_t token_max;
  BYTE *output_token;
  BYTE *ਨਿਵੇਸ਼_token;
  size_t ਨਿਵੇਸ਼_token_len;
  TCHAR *spn;
#ਨਹੀਂਤਾਂ
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ nonce[8];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਨਿਸ਼ਾਨਾ_info_len;
  ਖਾਲੀ *ਨਿਸ਼ਾਨਾ_info; /* TargetInfo received in the NTLM type-2 message */
#ਜੇ_ਅੰਤ
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(ਖਾਲੀ);

ਬਣਤਰ ntlmdata *Curl_auth_ntlm_get(ਬਣਤਰ connectdata *conn, bool proxy);
ਖਾਲੀ Curl_auth_ntlm_remove(ਬਣਤਰ connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
ਖਾਲੀ Curl_auth_cleanup_ntlm(ਬਣਤਰ ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_creds *creds,
                                             ਅਚਲ ਅੱਖਰ *default_service,
                                             ਅਚਲ ਅੱਖਰ *host,
                                             ਬਣਤਰ ntlmdata *ntlm,
                                             ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਅਚਲ ਬਣਤਰ bufref *type2ref,
                                             ਬਣਤਰ ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_creds *creds,
                                             ਬਣਤਰ ntlmdata *ntlm,
                                             ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_auth_is_ntlm_supported()     FALSE
#ਜੇ_ਅੰਤ /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_creds *creds,
                                               ਅਚਲ ਅੱਖਰ *host,
                                               ਅਚਲ ਲੰਮਾ ਪੋਰਟ,
                                               ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_creds *creds,
                                                ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

ਬਣਤਰ kerberos5data {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX ਪਛਾਣ_ਤੱਤ;
  SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX *p_ਪਛਾਣ_ਤੱਤ;
  size_t token_max;
  BYTE *output_token;
#ਨਹੀਂਤਾਂ
  gss_ctx_id_t context;
  gss_name_t spn;
#ਜੇ_ਅੰਤ
};

ਬਣਤਰ kerberos5data *Curl_auth_krb5_get(ਬਣਤਰ connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(ਖਾਲੀ);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
ਸੁਨੇਹਾ*/
CURLcode Curl_auth_create_gssapi_user_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਬਣਤਰ Curl_creds *creds,
                                              ਅਚਲ ਅੱਖਰ *default_service,
                                              ਅਚਲ ਅੱਖਰ *host,
                                              ਅਚਲ bool mutual_auth,
                                              ਅਚਲ ਬਣਤਰ bufref *chlg,
                                              ਬਣਤਰ kerberos5data *krb5,
                                              ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                  ਅਚਲ ਅੱਖਰ *authzid,
                                                  ਅਚਲ ਬਣਤਰ bufref *chlg,
                                                  ਬਣਤਰ kerberos5data *krb5,
                                                  ਬਣਤਰ bufref *ਬਾਹਰਲਾ);

/* This is used to clean up the GSSAPI specific data */
ਖਾਲੀ Curl_auth_cleanup_gssapi(ਬਣਤਰ kerberos5data *krb5);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_auth_is_gssapi_supported()       FALSE
#ਜੇ_ਅੰਤ /* USE_KERBEROS5 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO

bool Curl_auth_is_spnego_supported(ਖਾਲੀ);

/* meta key for storing NEGO meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
ਬਣਤਰ negotiatedata {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSSAPI
  OM_uint32 ਸਥਿਤੀ;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_ਬਫਰ_desc output_token;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GSS_C_CHANNEL_BOUND_FLAG
  ਬਣਤਰ dynbuf channel_binding_ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX ਪਛਾਣ_ਤੱਤ;
  SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX *p_ਪਛਾਣ_ਤੱਤ;
  TCHAR *spn;
  size_t token_max;
  BYTE *output_token;
  size_t output_token_ਲੰਮਾਈ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(noauthpersist);
  ਦੋਆਧਾਰੀ_ਅੰਕ(havenoauthpersist);
  ਦੋਆਧਾਰੀ_ਅੰਕ(havenegdata);
  ਦੋਆਧਾਰੀ_ਅੰਕ(havemultiplerequests);
};

ਬਣਤਰ negotiatedata *Curl_auth_nego_get(ਬਣਤਰ connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
ਸੁਨੇਹਾ*/
CURLcode Curl_auth_decode_spnego_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ Curl_creds *creds,
                                         ਅਚਲ ਅੱਖਰ *default_service,
                                         ਅਚਲ ਅੱਖਰ *host,
                                         ਅਚਲ ਅੱਖਰ *chlg64,
                                         ਬਣਤਰ negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
ਸੁਨੇਹਾ*/
CURLcode Curl_auth_create_spnego_ਸੁਨੇਹਾ(ਬਣਤਰ negotiatedata *nego,
                                         ਅੱਖਰ **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
ਖਾਲੀ Curl_auth_cleanup_spnego(ਬਣਤਰ negotiatedata *nego);

#ਜੇ_ਅੰਤ /* USE_SPNEGO */

#ਜੇ_ਅੰਤ /* HEADER_CURL_VAUTH_H */
