#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_VAUTH_H
#وصف_ڪريو سرو_CURL_VAUTH_H
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
#ضمن "curl_setup.h"

#ضمن "bufref.h"
#ضمن "curlx/dynbuf.h"
#ضمن "urldata.h"

بنية Curl_easy;
بنية Curl_creds;
بنية connectdata;
بنية Curl_peer;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
بنية digestdata;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_NTLM
بنية ntlmdata;
#جيڪڏهن_پورو

#إذا (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
بنية negotiatedata;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_GSASL
بنية gsasldata;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_WINDOWS_SSPI
#ضمن "curl_sspi.h"
#وصف_ڪريو GSS_ERROR(حالت) ((حالت) & 0x80000000)
#جيڪڏهن_پورو

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_منزل_host(بنية Curl_easy *معلومات);
bool Curl_auth_allowed_منزل_origin(بنية Curl_easy *معلومات,
                                 بنية Curl_peer *origin);

/* This is used to build an SPN string */
#جيڪڏهن_وصف_نه_ٿيل USE_WINDOWS_SSPI
حرف *Curl_auth_build_spn(ثابت حرف *service, ثابت حرف *host,
                          ثابت حرف *realm);
#وإلا
TCHAR *Curl_auth_build_spn(ثابت حرف *service, ثابت حرف *host,
                           ثابت حرف *realm);
#جيڪڏهن_پورو

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_شامل_آهي_domain(بنية Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_پيغام(بنية Curl_creds *creds,
                                        بنية bufref *ٻاهريون);

/* This is used to generate a LOGIN cleartext message */
فراغ Curl_auth_create_login_پيغام(ثابت حرف *قدر, بنية bufref *ٻاهريون);

/* This is used to generate an EXTERNAL cleartext message */
فراغ Curl_auth_create_external_پيغام(ثابت حرف *user, بنية bufref *ٻاهريون);

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_پيغام(ثابت بنية bufref *chlg,
                                           بنية Curl_creds *creds,
                                           بنية bufref *ٻاهريون);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(فراغ);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_پيغام(بنية Curl_easy *معلومات,
                                             ثابت بنية bufref *chlg,
                                             بنية Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             بنية bufref *ٻاهريون);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_پيغام(ثابت حرف *chlg,
                                              بنية digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_پيغام(بنية Curl_easy *معلومات,
                                              بنية Curl_creds *creds,
                                              ثابت بنانشان حرف *request,
                                              ثابت بنانشان حرف *uripath,
                                              بنية digestdata *digest,
                                              حرف **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
فراغ Curl_auth_digest_cleanup(بنية digestdata *digest);
#وإلا
#وصف_ڪريو Curl_auth_digest_cleanup(x)
#وصف_ڪريو Curl_auth_is_digest_supported()       FALSE
#جيڪڏهن_پورو /* !CURL_DISABLE_DIGEST_AUTH */

#جيڪڏهن_وصف_ٿيل USE_GSASL

/* meta key for storing GSASL meta at connection */
#وصف_ڪريو CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#ضمن <gsasl.h>
بنية gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

بنية gsasldata *Curl_auth_gsasl_get(بنية connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(بنية Curl_easy *معلومات,
                                  ثابت حرف *mech,
                                  بنية gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_شروعات(بنية Curl_easy *معلومات,
                               بنية Curl_creds *creds,
                               بنية gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(بنية Curl_easy *معلومات,
                               ثابت بنية bufref *chlg,
                               بنية gsasldata *gsasl,
                               بنية bufref *ٻاهريون);

/* This is used to clean up the gsasl specific data */
فراغ Curl_auth_gsasl_cleanup(بنية gsasldata *gsasl);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_NTLM

/* meta key for storing NTML meta at connection */
#وصف_ڪريو CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#وصف_ڪريو CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

بنية ntlmdata {
#جيڪڏهن_وصف_ٿيل USE_WINDOWS_SSPI
/* The sslContext is used for the Schannel bindings. The
 * api is available on the Windows 7 SDK and later.
 */
  CtxtHandle *sslContext;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_سڃاڻپ_عنصر_EX سڃاڻپ_عنصر;
  SEC_WINNT_AUTH_سڃاڻپ_عنصر_EX *p_سڃاڻپ_عنصر;
  size_t token_max;
  BYTE *output_token;
  BYTE *داخلا_token;
  size_t داخلا_token_len;
  TCHAR *spn;
#وإلا
  بنانشان صحيح flags;
  بنانشان حرف nonce[8];
  بنانشان صحيح هدف_info_len;
  فراغ *هدف_info; /* TargetInfo received in the NTLM type-2 message */
#جيڪڏهن_پورو
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(فراغ);

بنية ntlmdata *Curl_auth_ntlm_get(بنية connectdata *conn, bool proxy);
فراغ Curl_auth_ntlm_remove(بنية connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
فراغ Curl_auth_cleanup_ntlm(بنية ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_پيغام(بنية Curl_easy *معلومات,
                                             بنية Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             ثابت حرف *host,
                                             بنية ntlmdata *ntlm,
                                             بنية bufref *ٻاهريون);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_پيغام(بنية Curl_easy *معلومات,
                                             ثابت بنية bufref *type2ref,
                                             بنية ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_پيغام(بنية Curl_easy *معلومات,
                                             بنية Curl_creds *creds,
                                             بنية ntlmdata *ntlm,
                                             بنية bufref *ٻاهريون);

#وإلا
#وصف_ڪريو Curl_auth_is_ntlm_supported()     FALSE
#جيڪڏهن_پورو /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_پيغام(بنية Curl_creds *creds,
                                               ثابت حرف *host,
                                               ثابت ڊگهو پورٽ,
                                               بنية bufref *ٻاهريون);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_پيغام(بنية Curl_creds *creds,
                                                بنية bufref *ٻاهريون);

#جيڪڏهن_وصف_ٿيل USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#وصف_ڪريو CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

بنية kerberos5data {
#جيڪڏهن_وصف_ٿيل USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_سڃاڻپ_عنصر_EX سڃاڻپ_عنصر;
  SEC_WINNT_AUTH_سڃاڻپ_عنصر_EX *p_سڃاڻپ_عنصر;
  size_t token_max;
  BYTE *output_token;
#وإلا
  gss_ctx_id_t context;
  gss_name_t spn;
#جيڪڏهن_پورو
};

بنية kerberos5data *Curl_auth_krb5_get(بنية connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(فراغ);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
پيغام*/
CURLcode Curl_auth_create_gssapi_user_پيغام(بنية Curl_easy *معلومات,
                                              بنية Curl_creds *creds,
                                              ثابت حرف *default_service,
                                              ثابت حرف *host,
                                              ثابت bool mutual_auth,
                                              ثابت بنية bufref *chlg,
                                              بنية kerberos5data *krb5,
                                              بنية bufref *ٻاهريون);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_پيغام(بنية Curl_easy *معلومات,
                                                  ثابت حرف *authzid,
                                                  ثابت بنية bufref *chlg,
                                                  بنية kerberos5data *krb5,
                                                  بنية bufref *ٻاهريون);

/* This is used to clean up the GSSAPI specific data */
فراغ Curl_auth_cleanup_gssapi(بنية kerberos5data *krb5);
#وإلا
#وصف_ڪريو Curl_auth_is_gssapi_supported()       FALSE
#جيڪڏهن_پورو /* USE_KERBEROS5 */

#جيڪڏهن_وصف_ٿيل USE_SPNEGO

bool Curl_auth_is_spnego_supported(فراغ);

/* meta key for storing NEGO meta at connection */
#وصف_ڪريو CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#وصف_ڪريو CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
بنية negotiatedata {
#جيڪڏهن_وصف_ٿيل HAVE_GSSAPI
  OM_uint32 حالت;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_بفر_desc output_token;
#جيڪڏهن_وصف_ٿيل GSS_C_CHANNEL_BOUND_FLAG
  بنية dynbuf channel_binding_معلومات;
#جيڪڏهن_پورو
#وإلا
#جيڪڏهن_وصف_ٿيل USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
  SECURITY_حالت حالت;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_سڃاڻپ_عنصر_EX سڃاڻپ_عنصر;
  SEC_WINNT_AUTH_سڃاڻپ_عنصر_EX *p_سڃاڻپ_عنصر;
  TCHAR *spn;
  size_t token_max;
  BYTE *output_token;
  size_t output_token_ڊيگهائي;
#جيڪڏهن_پورو
#جيڪڏهن_پورو
  ٻٽو_انگ(noauthpersist);
  ٻٽو_انگ(havenoauthpersist);
  ٻٽو_انگ(havenegdata);
  ٻٽو_انگ(havemultiplerequests);
};

بنية negotiatedata *Curl_auth_nego_get(بنية connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
پيغام*/
CURLcode Curl_auth_decode_spnego_پيغام(بنية Curl_easy *معلومات,
                                         بنية Curl_creds *creds,
                                         ثابت حرف *default_service,
                                         ثابت حرف *host,
                                         ثابت حرف *chlg64,
                                         بنية negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
پيغام*/
CURLcode Curl_auth_create_spnego_پيغام(بنية negotiatedata *nego,
                                         حرف **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
فراغ Curl_auth_cleanup_spnego(بنية negotiatedata *nego);

#جيڪڏهن_پورو /* USE_SPNEGO */

#جيڪڏهن_پورو /* HEADER_CURL_VAUTH_H */
