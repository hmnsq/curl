#اگر_غیر_تعریف_شدہ سرخی_CURL_VAUTH_H
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
#شامل "curl_setup.h"

#شامل "bufref.h"
#شامل "curlx/dynbuf.h"
#شامل "urldata.h"

ساخت Curl_easy;
ساخت Curl_creds;
ساخت connectdata;
ساخت Curl_peer;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
ساخت digestdata;
#اگر_ختم

#اگر_تعریف_شدہ USE_NTLM
ساخت ntlmdata;
#اگر_ختم

#اگر (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
ساخت negotiatedata;
#اگر_ختم

#اگر_تعریف_شدہ USE_GSASL
ساخت gsasldata;
#اگر_ختم

#اگر_تعریف_شدہ USE_WINDOWS_SSPI
#شامل "curl_sspi.h"
#تعریف_کرو GSS_ERROR(حالت_2) ((حالت_2) & 0x80000000)
#اگر_ختم

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_مقصد_host(ساخت Curl_easy *ڈیٹا);
bool Curl_auth_allowed_مقصد_origin(ساخت Curl_easy *ڈیٹا,
                                 ساخت Curl_peer *origin);

/* This is used to build an SPN string */
#اگر_غیر_تعریف_شدہ USE_WINDOWS_SSPI
حرف *Curl_auth_build_spn(ثابت حرف *service, ثابت حرف *host,
                          ثابت حرف *realm);
#ورنہ
TCHAR *Curl_auth_build_spn(ثابت حرف *service, ثابت حرف *host,
                           ثابت حرف *realm);
#اگر_ختم

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_شامل_ہے_domain(ساخت Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_پیغام(ساخت Curl_creds *creds,
                                        ساخت bufref *برآمد);

/* This is used to generate a LOGIN cleartext message */
خالی Curl_auth_create_login_پیغام(ثابت حرف *قدر, ساخت bufref *برآمد);

/* This is used to generate an EXTERNAL cleartext message */
خالی Curl_auth_create_external_پیغام(ثابت حرف *user, ساخت bufref *برآمد);

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_پیغام(ثابت ساخت bufref *chlg,
                                           ساخت Curl_creds *creds,
                                           ساخت bufref *برآمد);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(خالی);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ثابت ساخت bufref *chlg,
                                             ساخت Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             ساخت bufref *برآمد);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_پیغام(ثابت حرف *chlg,
                                              ساخت digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_پیغام(ساخت Curl_easy *ڈیٹا,
                                              ساخت Curl_creds *creds,
                                              ثابت غیرعلامتی حرف *request,
                                              ثابت غیرعلامتی حرف *uripath,
                                              ساخت digestdata *digest,
                                              حرف **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
خالی Curl_auth_digest_cleanup(ساخت digestdata *digest);
#ورنہ
#تعریف_کرو Curl_auth_digest_cleanup(x)
#تعریف_کرو Curl_auth_is_digest_supported()       FALSE
#اگر_ختم /* !CURL_DISABLE_DIGEST_AUTH */

#اگر_تعریف_شدہ USE_GSASL

/* meta key for storing GSASL meta at connection */
#تعریف_کرو CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#شامل <gsasl.h>
ساخت gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

ساخت gsasldata *Curl_auth_gsasl_get(ساخت connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(ساخت Curl_easy *ڈیٹا,
                                  ثابت حرف *mech,
                                  ساخت gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_آغاز(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_creds *creds,
                               ساخت gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(ساخت Curl_easy *ڈیٹا,
                               ثابت ساخت bufref *chlg,
                               ساخت gsasldata *gsasl,
                               ساخت bufref *برآمد);

/* This is used to clean up the gsasl specific data */
خالی Curl_auth_gsasl_cleanup(ساخت gsasldata *gsasl);
#اگر_ختم

#اگر_تعریف_شدہ USE_NTLM

/* meta key for storing NTML meta at connection */
#تعریف_کرو CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#تعریف_کرو CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

ساخت ntlmdata {
#اگر_تعریف_شدہ USE_WINDOWS_SSPI
/* The sslContext is used for the Schannel bindings. The
 * api is available on the Windows 7 SDK and later.
 */
  CtxtHandle *sslContext;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_شناختی_عنصر_EX شناختی_عنصر;
  SEC_WINNT_AUTH_شناختی_عنصر_EX *p_شناختی_عنصر;
  size_t token_max;
  BYTE *output_token;
  BYTE *داخلہ_token;
  size_t داخلہ_token_len;
  TCHAR *spn;
#ورنہ
  غیرعلامتی صحیح flags;
  غیرعلامتی حرف nonce[8];
  غیرعلامتی صحیح ہدف_info_len;
  خالی *ہدف_info; /* TargetInfo received in the NTLM type-2 message */
#اگر_ختم
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(خالی);

ساخت ntlmdata *Curl_auth_ntlm_get(ساخت connectdata *conn, bool proxy);
خالی Curl_auth_ntlm_remove(ساخت connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
خالی Curl_auth_cleanup_ntlm(ساخت ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             ثابت حرف *host,
                                             ساخت ntlmdata *ntlm,
                                             ساخت bufref *برآمد);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ثابت ساخت bufref *type2ref,
                                             ساخت ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_creds *creds,
                                             ساخت ntlmdata *ntlm,
                                             ساخت bufref *برآمد);

#ورنہ
#تعریف_کرو Curl_auth_is_ntlm_supported()     FALSE
#اگر_ختم /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_پیغام(ساخت Curl_creds *creds,
                                               ثابت حرف *host,
                                               ثابت طویل پورٹ,
                                               ساخت bufref *برآمد);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_پیغام(ساخت Curl_creds *creds,
                                                ساخت bufref *برآمد);

#اگر_تعریف_شدہ USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#تعریف_کرو CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

ساخت kerberos5data {
#اگر_تعریف_شدہ USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_شناختی_عنصر_EX شناختی_عنصر;
  SEC_WINNT_AUTH_شناختی_عنصر_EX *p_شناختی_عنصر;
  size_t token_max;
  BYTE *output_token;
#ورنہ
  gss_ctx_id_t context;
  gss_name_t spn;
#اگر_ختم
};

ساخت kerberos5data *Curl_auth_krb5_get(ساخت connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(خالی);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
پیغام*/
CURLcode Curl_auth_create_gssapi_user_پیغام(ساخت Curl_easy *ڈیٹا,
                                              ساخت Curl_creds *creds,
                                              ثابت حرف *default_service,
                                              ثابت حرف *host,
                                              ثابت bool mutual_auth,
                                              ثابت ساخت bufref *chlg,
                                              ساخت kerberos5data *krb5,
                                              ساخت bufref *برآمد);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_پیغام(ساخت Curl_easy *ڈیٹا,
                                                  ثابت حرف *authzid,
                                                  ثابت ساخت bufref *chlg,
                                                  ساخت kerberos5data *krb5,
                                                  ساخت bufref *برآمد);

/* This is used to clean up the GSSAPI specific data */
خالی Curl_auth_cleanup_gssapi(ساخت kerberos5data *krb5);
#ورنہ
#تعریف_کرو Curl_auth_is_gssapi_supported()       FALSE
#اگر_ختم /* USE_KERBEROS5 */

#اگر_تعریف_شدہ USE_SPNEGO

bool Curl_auth_is_spnego_supported(خالی);

/* meta key for storing NEGO meta at connection */
#تعریف_کرو CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#تعریف_کرو CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
ساخت negotiatedata {
#اگر_تعریف_شدہ HAVE_GSSAPI
  OM_uint32 حالت_2;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_بفر_desc output_token;
#اگر_تعریف_شدہ GSS_C_CHANNEL_BOUND_FLAG
  ساخت dynbuf channel_binding_ڈیٹا;
#اگر_ختم
#ورنہ
#اگر_تعریف_شدہ USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
  SECURITY_حالت حالت_2;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_شناختی_عنصر_EX شناختی_عنصر;
  SEC_WINNT_AUTH_شناختی_عنصر_EX *p_شناختی_عنصر;
  TCHAR *spn;
  size_t token_max;
  BYTE *output_token;
  size_t output_token_لمبائی;
#اگر_ختم
#اگر_ختم
  ثنائی_ہندسہ(noauthpersist);
  ثنائی_ہندسہ(havenoauthpersist);
  ثنائی_ہندسہ(havenegdata);
  ثنائی_ہندسہ(havemultiplerequests);
};

ساخت negotiatedata *Curl_auth_nego_get(ساخت connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
پیغام*/
CURLcode Curl_auth_decode_spnego_پیغام(ساخت Curl_easy *ڈیٹا,
                                         ساخت Curl_creds *creds,
                                         ثابت حرف *default_service,
                                         ثابت حرف *host,
                                         ثابت حرف *chlg64,
                                         ساخت negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
پیغام*/
CURLcode Curl_auth_create_spnego_پیغام(ساخت negotiatedata *nego,
                                         حرف **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
خالی Curl_auth_cleanup_spnego(ساخت negotiatedata *nego);

#اگر_ختم /* USE_SPNEGO */

#اگر_ختم /* HEADER_CURL_VAUTH_H */
