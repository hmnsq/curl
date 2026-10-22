#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VAUTH_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VAUTH_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"

ರಚನೆ Curl_easy;
ರಚನೆ Curl_creds;
ರಚನೆ connectdata;
ರಚನೆ Curl_peer;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
ರಚನೆ digestdata;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
ರಚನೆ ntlmdata;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
ರಚನೆ negotiatedata;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL
ರಚನೆ gsasldata;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
#ಅನ್ತರ್ಭಾವ "curl_sspi.h"
#ವ್ಯಾಖ್ಯಾನಿಸು GSS_ERROR(ಸ್ಥಿತಿ_2) ((ಸ್ಥಿತಿ_2) & 0x80000000)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_ಗಮ್ಯ_host(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
bool Curl_auth_allowed_ಗಮ್ಯ_origin(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ Curl_peer *origin);

/* This is used to build an SPN string */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINDOWS_SSPI
ಅಕ್ಷರ *Curl_auth_build_spn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *realm);
#ಇಲ್ಲದಿದ್ದರೆ
TCHAR *Curl_auth_build_spn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *realm);
#ಆದರೆ_ಅಂತ್ಯ

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_ಒಳಗೊಂಡಿದೆ_domain(ರಚನೆ Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_ಸಂದೇಶ(ರಚನೆ Curl_creds *creds,
                                        ರಚನೆ bufref *ಹೊರಹೋಗು);

/* This is used to generate a LOGIN cleartext message */
ಶೂನ್ಯ Curl_auth_create_login_ಸಂದೇಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, ರಚನೆ bufref *ಹೊರಹೋಗು);

/* This is used to generate an EXTERNAL cleartext message */
ಶೂನ್ಯ Curl_auth_create_external_ಸಂದೇಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user, ರಚನೆ bufref *ಹೊರಹೋಗು);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_ಸಂದೇಶ(ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                                           ರಚನೆ Curl_creds *creds,
                                           ರಚನೆ bufref *ಹೊರಹೋಗು);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(ಶೂನ್ಯ);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                                             ರಚನೆ Curl_creds *creds,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                             ರಚನೆ bufref *ಹೊರಹೋಗು);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_ಸಂದೇಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chlg,
                                              ರಚನೆ digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ರಚನೆ Curl_creds *creds,
                                              ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *request,
                                              ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *uripath,
                                              ರಚನೆ digestdata *digest,
                                              ಅಕ್ಷರ **outptr, size_t *outlen);

/* This is used to clean up the digest specific data */
ಶೂನ್ಯ Curl_auth_digest_cleanup(ರಚನೆ digestdata *digest);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_auth_digest_cleanup(x)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_auth_is_digest_supported()       FALSE
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_DIGEST_AUTH */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL

/* meta key for storing GSASL meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#ಅನ್ತರ್ಭಾವ <gsasl.h>
ರಚನೆ gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

ರಚನೆ gsasldata *Curl_auth_gsasl_get(ರಚನೆ connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                                  ರಚನೆ gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_ಆರಂಭ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_creds *creds,
                               ರಚನೆ gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                               ರಚನೆ gsasldata *gsasl,
                               ರಚನೆ bufref *ಹೊರಹೋಗು);

/* This is used to clean up the gsasl specific data */
ಶೂನ್ಯ Curl_auth_gsasl_cleanup(ರಚನೆ gsasldata *gsasl);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM

/* meta key for storing NTML meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

ರಚನೆ ntlmdata {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
/* The sslContext is used for the Schannel bindings. The
 * api is available on the Windows 7 SDK and later.
 */
  CtxtHandle *sslContext;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_ತಟಸ್ಥ_EX ತಟಸ್ಥ;
  SEC_WINNT_AUTH_ತಟಸ್ಥ_EX *p_ತಟಸ್ಥ;
  size_t token_max;
  BYTE *output_token;
  BYTE *ಒಳಹರಿವು_token;
  size_t ಒಳಹರಿವು_token_len;
  TCHAR *spn;
#ಇಲ್ಲದಿದ್ದರೆ
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ nonce[8];
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಗುರಿ_info_len;
  ಶೂನ್ಯ *ಗುರಿ_info; /* TargetInfo received in the NTLM type-2 message */
#ಆದರೆ_ಅಂತ್ಯ
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(ಶೂನ್ಯ);

ರಚನೆ ntlmdata *Curl_auth_ntlm_get(ರಚನೆ connectdata *conn, bool proxy);
ಶೂನ್ಯ Curl_auth_ntlm_remove(ರಚನೆ connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
ಶೂನ್ಯ Curl_auth_cleanup_ntlm(ರಚನೆ ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_creds *creds,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                             ರಚನೆ ntlmdata *ntlm,
                                             ರಚನೆ bufref *ಹೊರಹೋಗು);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *type2ref,
                                             ರಚನೆ ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_creds *creds,
                                             ರಚನೆ ntlmdata *ntlm,
                                             ರಚನೆ bufref *ಹೊರಹೋಗು);

#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_auth_is_ntlm_supported()     FALSE
#ಆದರೆ_ಅಂತ್ಯ /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_ಸಂದೇಶ(ರಚನೆ Curl_creds *creds,
                                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                               ಸ್ಥಿರಾಂಕ ದೀರ್ಘ ಪೋರ್ಟ್,
                                               ರಚನೆ bufref *ಹೊರಹೋಗು);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_ಸಂದೇಶ(ರಚನೆ Curl_creds *creds,
                                                ರಚನೆ bufref *ಹೊರಹೋಗು);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

ರಚನೆ kerberos5data {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_ತಟಸ್ಥ_EX ತಟಸ್ಥ;
  SEC_WINNT_AUTH_ತಟಸ್ಥ_EX *p_ತಟಸ್ಥ;
  size_t token_max;
  BYTE *output_token;
#ಇಲ್ಲದಿದ್ದರೆ
  gss_ctx_id_t context;
  gss_name_t spn;
#ಆದರೆ_ಅಂತ್ಯ
};

ರಚನೆ kerberos5data *Curl_auth_krb5_get(ರಚನೆ connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(ಶೂನ್ಯ);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
ಸಂದೇಶ*/
CURLcode Curl_auth_create_gssapi_user_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ರಚನೆ Curl_creds *creds,
                                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                              ಸ್ಥಿರಾಂಕ bool mutual_auth,
                                              ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                                              ರಚನೆ kerberos5data *krb5,
                                              ರಚನೆ bufref *ಹೊರಹೋಗು);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *authzid,
                                                  ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                                                  ರಚನೆ kerberos5data *krb5,
                                                  ರಚನೆ bufref *ಹೊರಹೋಗು);

/* This is used to clean up the GSSAPI specific data */
ಶೂನ್ಯ Curl_auth_cleanup_gssapi(ರಚನೆ kerberos5data *krb5);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_auth_is_gssapi_supported()       FALSE
#ಆದರೆ_ಅಂತ್ಯ /* USE_KERBEROS5 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO

bool Curl_auth_is_spnego_supported(ಶೂನ್ಯ);

/* meta key for storing NEGO meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
ರಚನೆ negotiatedata {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSSAPI
  OM_uint32 ಸ್ಥಿತಿ_2;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_ಬಫರ್_desc output_token;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GSS_C_CHANNEL_BOUND_FLAG
  ರಚನೆ dynbuf channel_binding_ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_ತಟಸ್ಥ_EX ತಟಸ್ಥ;
  SEC_WINNT_AUTH_ತಟಸ್ಥ_EX *p_ತಟಸ್ಥ;
  TCHAR *spn;
  size_t token_max;
  BYTE *output_token;
  size_t output_token_ಉದ್ದಳತೆ;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(noauthpersist);
  ಬಿಟ್(havenoauthpersist);
  ಬಿಟ್(havenegdata);
  ಬಿಟ್(havemultiplerequests);
};

ರಚನೆ negotiatedata *Curl_auth_nego_get(ರಚನೆ connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
ಸಂದೇಶ*/
CURLcode Curl_auth_decode_spnego_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ Curl_creds *creds,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chlg64,
                                         ರಚನೆ negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
ಸಂದೇಶ*/
CURLcode Curl_auth_create_spnego_ಸಂದೇಶ(ರಚನೆ negotiatedata *nego,
                                         ಅಕ್ಷರ **outptr, size_t *outlen);

/* This is used to clean up the SPNEGO specific data */
ಶೂನ್ಯ Curl_auth_cleanup_spnego(ರಚನೆ negotiatedata *nego);

#ಆದರೆ_ಅಂತ್ಯ /* USE_SPNEGO */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VAUTH_H */
