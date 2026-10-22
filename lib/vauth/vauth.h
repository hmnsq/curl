#جے_بنا_تعریف سرخی_CURL_VAUTH_H
#تعریف_کرو سرخی_CURL_VAUTH_H
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

#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
بنية digestdata;
#جے_انت

#جے_تعریف_شدہ USE_NTLM
بنية ntlmdata;
#جے_انت

#إذا (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
بنية negotiatedata;
#جے_انت

#جے_تعریف_شدہ USE_GSASL
بنية gsasldata;
#جے_انت

#جے_تعریف_شدہ USE_WINDOWS_SSPI
#ضمن "curl_sspi.h"
#تعریف_کرو GSS_ERROR(حالت) ((حالت) & 0x80000000)
#جے_انت

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_منزل_host(بنية Curl_easy *انکڑے);
bool Curl_auth_allowed_منزل_origin(بنية Curl_easy *انکڑے,
                                 بنية Curl_peer *origin);

/* This is used to build an SPN string */
#جے_بنا_تعریف USE_WINDOWS_SSPI
حرف *Curl_auth_build_spn(ثابت حرف *service, ثابت حرف *host,
                          ثابت حرف *realm);
#وإلا
TCHAR *Curl_auth_build_spn(ثابت حرف *service, ثابت حرف *host,
                           ثابت حرف *realm);
#جے_انت

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_شامل_اے_domain(بنية Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_سنیہا(بنية Curl_creds *creds,
                                        بنية bufref *باہرلا);

/* This is used to generate a LOGIN cleartext message */
فراغ Curl_auth_create_login_سنیہا(ثابت حرف *قدر, بنية bufref *باہرلا);

/* This is used to generate an EXTERNAL cleartext message */
فراغ Curl_auth_create_external_سنیہا(ثابت حرف *user, بنية bufref *باہرلا);

#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_سنیہا(ثابت بنية bufref *chlg,
                                           بنية Curl_creds *creds,
                                           بنية bufref *باہرلا);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(فراغ);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_سنیہا(بنية Curl_easy *انکڑے,
                                             ثابت بنية bufref *chlg,
                                             بنية Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             بنية bufref *باہرلا);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_سنیہا(ثابت حرف *chlg,
                                              بنية digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_سنیہا(بنية Curl_easy *انکڑے,
                                              بنية Curl_creds *creds,
                                              ثابت بنانشانی حرف *request,
                                              ثابت بنانشانی حرف *uripath,
                                              بنية digestdata *digest,
                                              حرف **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
فراغ Curl_auth_digest_cleanup(بنية digestdata *digest);
#وإلا
#تعریف_کرو Curl_auth_digest_cleanup(x)
#تعریف_کرو Curl_auth_is_digest_supported()       FALSE
#جے_انت /* !CURL_DISABLE_DIGEST_AUTH */

#جے_تعریف_شدہ USE_GSASL

/* meta key for storing GSASL meta at connection */
#تعریف_کرو CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#ضمن <gsasl.h>
بنية gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

بنية gsasldata *Curl_auth_gsasl_get(بنية connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(بنية Curl_easy *انکڑے,
                                  ثابت حرف *mech,
                                  بنية gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_شروع(بنية Curl_easy *انکڑے,
                               بنية Curl_creds *creds,
                               بنية gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(بنية Curl_easy *انکڑے,
                               ثابت بنية bufref *chlg,
                               بنية gsasldata *gsasl,
                               بنية bufref *باہرلا);

/* This is used to clean up the gsasl specific data */
فراغ Curl_auth_gsasl_cleanup(بنية gsasldata *gsasl);
#جے_انت

#جے_تعریف_شدہ USE_NTLM

/* meta key for storing NTML meta at connection */
#تعریف_کرو CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#تعریف_کرو CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

بنية ntlmdata {
#جے_تعریف_شدہ USE_WINDOWS_SSPI
/* The sslContext is used for the Schannel bindings. The
 * api is available on the Windows 7 SDK and later.
 */
  CtxtHandle *sslContext;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_پچھان_تت_EX پچھان_تت;
  SEC_WINNT_AUTH_پچھان_تت_EX *p_پچھان_تت;
  size_t token_max;
  BYTE *output_token;
  BYTE *داخلہ_token;
  size_t داخلہ_token_len;
  TCHAR *spn;
#وإلا
  بنانشانی صحيح flags;
  بنانشانی حرف nonce[8];
  بنانشانی صحيح نشانہ_info_len;
  فراغ *نشانہ_info; /* TargetInfo received in the NTLM type-2 message */
#جے_انت
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(فراغ);

بنية ntlmdata *Curl_auth_ntlm_get(بنية connectdata *conn, bool proxy);
فراغ Curl_auth_ntlm_remove(بنية connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
فراغ Curl_auth_cleanup_ntlm(بنية ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_سنیہا(بنية Curl_easy *انکڑے,
                                             بنية Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             ثابت حرف *host,
                                             بنية ntlmdata *ntlm,
                                             بنية bufref *باہرلا);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_سنیہا(بنية Curl_easy *انکڑے,
                                             ثابت بنية bufref *type2ref,
                                             بنية ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_سنیہا(بنية Curl_easy *انکڑے,
                                             بنية Curl_creds *creds,
                                             بنية ntlmdata *ntlm,
                                             بنية bufref *باہرلا);

#وإلا
#تعریف_کرو Curl_auth_is_ntlm_supported()     FALSE
#جے_انت /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_سنیہا(بنية Curl_creds *creds,
                                               ثابت حرف *host,
                                               ثابت لما پورٹ,
                                               بنية bufref *باہرلا);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_سنیہا(بنية Curl_creds *creds,
                                                بنية bufref *باہرلا);

#جے_تعریف_شدہ USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#تعریف_کرو CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

بنية kerberos5data {
#جے_تعریف_شدہ USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_پچھان_تت_EX پچھان_تت;
  SEC_WINNT_AUTH_پچھان_تت_EX *p_پچھان_تت;
  size_t token_max;
  BYTE *output_token;
#وإلا
  gss_ctx_id_t context;
  gss_name_t spn;
#جے_انت
};

بنية kerberos5data *Curl_auth_krb5_get(بنية connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(فراغ);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
سنیہا*/
CURLcode Curl_auth_create_gssapi_user_سنیہا(بنية Curl_easy *انکڑے,
                                              بنية Curl_creds *creds,
                                              ثابت حرف *default_service,
                                              ثابت حرف *host,
                                              ثابت bool mutual_auth,
                                              ثابت بنية bufref *chlg,
                                              بنية kerberos5data *krb5,
                                              بنية bufref *باہرلا);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_سنیہا(بنية Curl_easy *انکڑے,
                                                  ثابت حرف *authzid,
                                                  ثابت بنية bufref *chlg,
                                                  بنية kerberos5data *krb5,
                                                  بنية bufref *باہرلا);

/* This is used to clean up the GSSAPI specific data */
فراغ Curl_auth_cleanup_gssapi(بنية kerberos5data *krb5);
#وإلا
#تعریف_کرو Curl_auth_is_gssapi_supported()       FALSE
#جے_انت /* USE_KERBEROS5 */

#جے_تعریف_شدہ USE_SPNEGO

bool Curl_auth_is_spnego_supported(فراغ);

/* meta key for storing NEGO meta at connection */
#تعریف_کرو CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#تعریف_کرو CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
بنية negotiatedata {
#جے_تعریف_شدہ HAVE_GSSAPI
  OM_uint32 حالت;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_بفر_desc output_token;
#جے_تعریف_شدہ GSS_C_CHANNEL_BOUND_FLAG
  بنية dynbuf channel_binding_انکڑے;
#جے_انت
#وإلا
#جے_تعریف_شدہ USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
  SECURITY_حالت حالت;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_پچھان_تت_EX پچھان_تت;
  SEC_WINNT_AUTH_پچھان_تت_EX *p_پچھان_تت;
  TCHAR *spn;
  size_t token_max;
  BYTE *output_token;
  size_t output_token_لمائی;
#جے_انت
#جے_انت
  ثنائی_ہندسہ(noauthpersist);
  ثنائی_ہندسہ(havenoauthpersist);
  ثنائی_ہندسہ(havenegdata);
  ثنائی_ہندسہ(havemultiplerequests);
};

بنية negotiatedata *Curl_auth_nego_get(بنية connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
سنیہا*/
CURLcode Curl_auth_decode_spnego_سنیہا(بنية Curl_easy *انکڑے,
                                         بنية Curl_creds *creds,
                                         ثابت حرف *default_service,
                                         ثابت حرف *host,
                                         ثابت حرف *chlg64,
                                         بنية negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
سنیہا*/
CURLcode Curl_auth_create_spnego_سنیہا(بنية negotiatedata *nego,
                                         حرف **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
فراغ Curl_auth_cleanup_spnego(بنية negotiatedata *nego);

#جے_انت /* USE_SPNEGO */

#جے_انت /* HEADER_CURL_VAUTH_H */
