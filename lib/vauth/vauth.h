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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "urldata.h"

संरचना Curl_easy;
संरचना Curl_creds;
संरचना connectdata;
संरचना Curl_peer;

#ifndef CURL_DISABLE_DIGEST_AUTH
संरचना digestdata;
#endif

#ifdef USE_NTLM
संरचना ntlmdata;
#endif

#यदि (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)) && defined(USE_SPNEGO)
संरचना negotiatedata;
#endif

#ifdef USE_GSASL
संरचना gsasldata;
#endif

#ifdef USE_WINDOWS_SSPI
#अन्तर्भाव "curl_sspi.h"
#define GSS_ERROR(status) ((status) & 0x80000000)
#endif

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can (still) be sent to this host.
 */
bool Curl_auth_allowed_to_host(संरचना Curl_easy *data);
bool Curl_auth_allowed_to_origin(संरचना Curl_easy *data,
                                 संरचना Curl_peer *origin);

/* This is used to build an SPN string */
#ifndef USE_WINDOWS_SSPI
वर्ण *Curl_auth_build_spn(अचर वर्ण *service, अचर वर्ण *host,
                          अचर वर्ण *realm);
#अन्यथा
TCHAR *Curl_auth_build_spn(अचर वर्ण *service, अचर वर्ण *host,
                           अचर वर्ण *realm);
#endif

/* This is used to test if the user contains a Windows domain name */
bool Curl_auth_user_contains_domain(संरचना Curl_creds *creds);

/* This is used to generate a PLAIN cleartext message */
CURLcode Curl_auth_create_plain_message(संरचना Curl_creds *creds,
                                        संरचना bufref *out);

/* This is used to generate a LOGIN cleartext message */
शून्य Curl_auth_create_login_message(अचर वर्ण *मान, संरचना bufref *out);

/* This is used to generate an EXTERNAL cleartext message */
शून्य Curl_auth_create_external_message(अचर वर्ण *user, संरचना bufref *out);

#ifndef CURL_DISABLE_DIGEST_AUTH
/* This is used to generate a CRAM-MD5 response message */
CURLcode Curl_auth_create_cram_md5_message(अचर संरचना bufref *chlg,
                                           संरचना Curl_creds *creds,
                                           संरचना bufref *out);

/* This is used to evaluate if DIGEST is supported */
bool Curl_auth_is_digest_supported(शून्य);

/* This is used to generate a base64 encoded DIGEST-MD5 response message */
CURLcode Curl_auth_create_digest_md5_message(संरचना Curl_easy *data,
                                             अचर संरचना bufref *chlg,
                                             संरचना Curl_creds *creds,
                                             अचर वर्ण *default_service,
                                             संरचना bufref *out);

/* This is used to decode an HTTP DIGEST challenge message */
CURLcode Curl_auth_decode_digest_http_message(अचर वर्ण *chlg,
                                              संरचना digestdata *digest);

/* This is used to generate an HTTP DIGEST response message */
CURLcode Curl_auth_create_digest_http_message(संरचना Curl_easy *data,
                                              संरचना Curl_creds *creds,
                                              अचर अचिन्हित वर्ण *request,
                                              अचर अचिन्हित वर्ण *uripath,
                                              संरचना digestdata *digest,
                                              वर्ण **outptr, माप_प्रकार *outlen);

/* This is used to clean up the digest specific data */
शून्य Curl_auth_digest_cleanup(संरचना digestdata *digest);
#अन्यथा
#define Curl_auth_digest_cleanup(x)
#define Curl_auth_is_digest_supported()       FALSE
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_GSASL

/* meta key for storing GSASL meta at connection */
#define CURL_META_GSASL_CONN   "meta:auth:gsasl:conn"

#अन्तर्भाव <gsasl.h>
संरचना gsasldata {
  Gsasl *ctx;
  Gsasl_session *client;
};

संरचना gsasldata *Curl_auth_gsasl_get(संरचना connectdata *conn);

/* This is used to evaluate if MECH is supported by gsasl */
bool Curl_auth_gsasl_is_supported(संरचना Curl_easy *data,
                                  अचर वर्ण *mech,
                                  संरचना gsasldata *gsasl);
/* This is used to start a gsasl method */
CURLcode Curl_auth_gsasl_start(संरचना Curl_easy *data,
                               संरचना Curl_creds *creds,
                               संरचना gsasldata *gsasl);

/* This is used to process and generate a new SASL token */
CURLcode Curl_auth_gsasl_token(संरचना Curl_easy *data,
                               अचर संरचना bufref *chlg,
                               संरचना gsasldata *gsasl,
                               संरचना bufref *out);

/* This is used to clean up the gsasl specific data */
शून्य Curl_auth_gsasl_cleanup(संरचना gsasldata *gsasl);
#endif

#ifdef USE_NTLM

/* meta key for storing NTML meta at connection */
#define CURL_META_NTLM_CONN   "meta:auth:ntml:conn"
/* meta key for storing NTML-PROXY meta at connection */
#define CURL_META_NTLM_PROXY_CONN   "meta:auth:ntml-proxy:conn"

संरचना ntlmdata {
#ifdef USE_WINDOWS_SSPI
/* The sslContext is used for the Schannel bindings. The
 * api is available on the Windows 7 SDK and later.
 */
  CtxtHandle *sslContext;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  माप_प्रकार token_max;
  BYTE *output_token;
  BYTE *आगत_token;
  माप_प्रकार आगत_token_len;
  TCHAR *spn;
#अन्यथा
  अचिन्हित पूर्णाङ्क flags;
  अचिन्हित वर्ण nonce[8];
  अचिन्हित पूर्णाङ्क target_info_len;
  शून्य *target_info; /* TargetInfo received in the NTLM type-2 message */
#endif
};

/* This is used to evaluate if NTLM is supported */
bool Curl_auth_is_ntlm_supported(शून्य);

संरचना ntlmdata *Curl_auth_ntlm_get(संरचना connectdata *conn, bool proxy);
शून्य Curl_auth_ntlm_remove(संरचना connectdata *conn, bool proxy);

/* This is used to clean up the NTLM specific data */
शून्य Curl_auth_cleanup_ntlm(संरचना ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-1 message */
CURLcode Curl_auth_create_ntlm_type1_message(संरचना Curl_easy *data,
                                             संरचना Curl_creds *creds,
                                             अचर वर्ण *default_service,
                                             अचर वर्ण *host,
                                             संरचना ntlmdata *ntlm,
                                             संरचना bufref *out);

/* This is used to decode a base64 encoded NTLM type-2 message */
CURLcode Curl_auth_decode_ntlm_type2_message(संरचना Curl_easy *data,
                                             अचर संरचना bufref *type2ref,
                                             संरचना ntlmdata *ntlm);

/* This is used to generate a base64 encoded NTLM type-3 message */
CURLcode Curl_auth_create_ntlm_type3_message(संरचना Curl_easy *data,
                                             संरचना Curl_creds *creds,
                                             संरचना ntlmdata *ntlm,
                                             संरचना bufref *out);

#अन्यथा
#define Curl_auth_is_ntlm_supported()     FALSE
#endif /* USE_NTLM */

/* This is used to generate a base64 encoded OAuth 2.0 message */
CURLcode Curl_auth_create_oauth_bearer_message(संरचना Curl_creds *creds,
                                               अचर वर्ण *host,
                                               अचर दीर्घ port,
                                               संरचना bufref *out);

/* This is used to generate a base64 encoded XOAuth 2.0 message */
CURLcode Curl_auth_create_xoauth_bearer_message(संरचना Curl_creds *creds,
                                                संरचना bufref *out);

#ifdef USE_KERBEROS5

/* meta key for storing KRB5 meta at connection */
#define CURL_META_KRB5_CONN   "meta:auth:krb5:conn"

संरचना kerberos5data {
#ifdef USE_WINDOWS_SSPI
  CredHandle *credentials;
  CtxtHandle *context;
  TCHAR *spn;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  माप_प्रकार token_max;
  BYTE *output_token;
#अन्यथा
  gss_ctx_id_t context;
  gss_name_t spn;
#endif
};

संरचना kerberos5data *Curl_auth_krb5_get(संरचना connectdata *conn);

/* This is used to evaluate if GSSAPI (Kerberos V5) is supported */
bool Curl_auth_is_gssapi_supported(शून्य);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) user token
   message */
CURLcode Curl_auth_create_gssapi_user_message(संरचना Curl_easy *data,
                                              संरचना Curl_creds *creds,
                                              अचर वर्ण *default_service,
                                              अचर वर्ण *host,
                                              अचर bool mutual_auth,
                                              अचर संरचना bufref *chlg,
                                              संरचना kerberos5data *krb5,
                                              संरचना bufref *out);

/* This is used to generate a base64 encoded GSSAPI (Kerberos V5) security
   token message */
CURLcode Curl_auth_create_gssapi_security_message(संरचना Curl_easy *data,
                                                  अचर वर्ण *authzid,
                                                  अचर संरचना bufref *chlg,
                                                  संरचना kerberos5data *krb5,
                                                  संरचना bufref *out);

/* This is used to clean up the GSSAPI specific data */
शून्य Curl_auth_cleanup_gssapi(संरचना kerberos5data *krb5);
#अन्यथा
#define Curl_auth_is_gssapi_supported()       FALSE
#endif /* USE_KERBEROS5 */

#ifdef USE_SPNEGO

bool Curl_auth_is_spnego_supported(शून्य);

/* meta key for storing NEGO meta at connection */
#define CURL_META_NEGO_CONN         "meta:auth:nego:conn"
/* meta key for storing NEGO PROXY meta at connection */
#define CURL_META_NEGO_PROXY_CONN   "meta:auth:nego-proxy:conn"

/* Struct used for Negotiate (SPNEGO) authentication */
संरचना negotiatedata {
#ifdef HAVE_GSSAPI
  OM_uint32 status;
  gss_ctx_id_t context;
  gss_name_t spn;
  gss_cred_id_t cred;
  gss_buffer_desc output_token;
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  संरचना dynbuf channel_binding_data;
#endif
#अन्यथा
#ifdef USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
  SECURITY_STATUS status;
  CredHandle *credentials;
  CtxtHandle *context;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  TCHAR *spn;
  माप_प्रकार token_max;
  BYTE *output_token;
  माप_प्रकार output_token_length;
#endif
#endif
  BIT(noauthpersist);
  BIT(havenoauthpersist);
  BIT(havenegdata);
  BIT(havemultiplerequests);
};

संरचना negotiatedata *Curl_auth_nego_get(संरचना connectdata *conn, bool proxy);

/* This is used to decode a base64 encoded SPNEGO (Negotiate) challenge
   message */
CURLcode Curl_auth_decode_spnego_message(संरचना Curl_easy *data,
                                         संरचना Curl_creds *creds,
                                         अचर वर्ण *default_service,
                                         अचर वर्ण *host,
                                         अचर वर्ण *chlg64,
                                         संरचना negotiatedata *nego);

/* This is used to generate a base64 encoded SPNEGO (Negotiate) response
   message */
CURLcode Curl_auth_create_spnego_message(संरचना negotiatedata *nego,
                                         वर्ण **outptr, माप_प्रकार *outlen);

/* This is used to clean up the SPNEGO specific data */
शून्य Curl_auth_cleanup_spnego(संरचना negotiatedata *nego);

#endif /* USE_SPNEGO */

#endif /* HEADER_CURL_VAUTH_H */
