/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>.
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
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
 * RFC4752 The Kerberos V5 ("GSSAPI") SASL Mechanism
 *
 ***************************************************************************/
#ضمن "curl_setup.h"

#إذا defined(HAVE_GSSAPI) && defined(USE_KERBEROS5)

#ضمن "vauth/vauth.h"
#ضمن "curl_sasl.h"
#ضمن "curl_gssapi.h"
#ضمن "curl_trc.h"

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wdeprecated-declarations"
#جيڪڏهن_پورو

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by the GSS-API library.
 */
bool Curl_auth_is_gssapi_supported(فراغ)
{
  أرجع TRUE;
}

/*
 * Curl_auth_create_gssapi_user_message()
 *
 * This is used to generate an already encoded GSSAPI (Kerberos V5) user token
 * message ready for sending to the recipient.
 *
 * Parameters:
 *
 * data        [in]     - The session handle.
 * userp       [in]     - The username.
 * passwdp     [in]     - The user's password.
 * service     [in]     - The service type such as http, smtp, pop or imap.
 * host        [in[     - The hostname.
 * mutual_auth [in]     - Flag specifying whether or not mutual authentication
 *                        is enabled.
 * chlg        [in]     - Optional challenge message.
 * krb5        [in/out] - The Kerberos 5 data struct being used and modified.
 * out         [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_gssapi_user_پيغام(بنية Curl_easy *معلومات,
                                              بنية Curl_creds *creds,
                                              ثابت حرف *default_service,
                                              ثابت حرف *host,
                                              ثابت bool mutual_auth,
                                              ثابت بنية bufref *chlg,
                                              بنية kerberos5data *krb5,
                                              بنية bufref *ٻاهريون)
{
  CURLcode نتيجو = CURLE_OK;
  OM_uint32 major_حالت;
  OM_uint32 minor_حالت;
  OM_uint32 اڻ_استعمال_ٿيل_حالت;
  gss_بفر_desc داخلا_token = GSS_C_خالي_جايون_بفر;
  gss_بفر_desc output_token = GSS_C_خالي_جايون_بفر;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  إذا(!krb5->spn) {
    gss_بفر_desc spn_token = GSS_C_خالي_جايون_بفر;

    /* Generate our SPN */
    حرف *spn = Curl_auth_build_spn(service, عدم, host);
    إذا(!spn)
      أرجع CURLE_ٻاهريون_جو_يادگيري;

    /* Populate the SPN structure */
    spn_token.قدر = spn;
    spn_token.ڊيگهائي = لڙي_ڊيگهه(spn);

    /* Import the SPN */
    major_حالت = gss_import_name(&minor_حالت, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE, &krb5->spn);
    إذا(GSS_ERROR(major_حالت)) {
      Curl_gss_log_error(معلومات, "gss_import_name() failed: ",
                         major_حالت, minor_حالت);

      curlx_free(spn);

      أرجع CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  إذا(chlg) {
    إذا(!Curl_bufref_len(chlg)) {
      infof(معلومات, "GSSAPI handshake failure (empty challenge message)");
      أرجع CURLE_BAD_CONTENT_ENCODING;
    }
    داخلا_token.قدر = CURL_UNCONST(Curl_bufref_ptr(chlg));
    داخلا_token.ڊيگهائي = Curl_bufref_len(chlg);
  }

  major_حالت = Curl_gss_init_sec_context(معلومات,
                                           &minor_حالت,
                                           &krb5->context,
                                           krb5->spn,
                                           &Curl_krb5_mech_oid,
                                           GSS_C_NO_CHANNEL_BINDINGS,
                                           &داخلا_token,
                                           &output_token,
                                           mutual_auth,
                                           عدم,
                                           GSS_C_NO_CREDENTIAL);

  إذا(GSS_ERROR(major_حالت)) {
    إذا(output_token.قدر)
      gss_release_بفر(&اڻ_استعمال_ٿيل_حالت, &output_token);

    Curl_gss_log_error(معلومات, "gss_init_sec_context() failed: ",
                       major_حالت, minor_حالت);

    أرجع CURLE_AUTH_ERROR;
  }

  إذا(output_token.قدر && output_token.ڊيگهائي) {
    نتيجو = Curl_bufref_memdup0(ٻاهريون, output_token.قدر, output_token.ڊيگهائي);
    gss_release_بفر(&اڻ_استعمال_ٿيل_حالت, &output_token);
  }
  وإلا
    Curl_bufref_سيٽ(ٻاهريون, mutual_auth ? "" : عدم, 0, عدم);

  أرجع نتيجو;
}

/*
 * Curl_auth_create_gssapi_security_message()
 *
 * This is used to generate an already encoded GSSAPI (Kerberos V5) security
 * token message ready for sending to the recipient.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * authzid [in]     - The authorization identity if some.
 * chlg    [in]     - Optional challenge message.
 * krb5    [in/out] - The Kerberos 5 data struct being used and modified.
 * out     [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_gssapi_security_پيغام(بنية Curl_easy *معلومات,
                                                  ثابت حرف *authzid,
                                                  ثابت بنية bufref *chlg,
                                                  بنية kerberos5data *krb5,
                                                  بنية bufref *ٻاهريون)
{
  CURLcode نتيجو = CURLE_OK;
  size_t messagelen = 0;
  بنانشان حرف *پيغام = عدم;
  OM_uint32 major_حالت;
  OM_uint32 minor_حالت;
  OM_uint32 اڻ_استعمال_ٿيل_حالت;
  gss_بفر_desc داخلا_token = GSS_C_خالي_جايون_بفر;
  gss_بفر_desc output_token = GSS_C_خالي_جايون_بفر;
  بنانشان حرف *indata;
  gss_qop_t qop = GSS_C_QOP_DEFAULT;
  بنانشان صحيح sec_layer = 0;
  بنانشان صحيح max_سائيز = 0;

  /* Ensure we have a valid challenge message */
  إذا(!Curl_bufref_len(chlg)) {
    infof(معلومات, "GSSAPI handshake failure (empty security message)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Setup the challenge "input" security buffer */
  داخلا_token.قدر = CURL_UNCONST(Curl_bufref_ptr(chlg));
  داخلا_token.ڊيگهائي = Curl_bufref_len(chlg);

  /* Decrypt the inbound challenge and obtain the qop */
  major_حالت = gss_unwrap(&minor_حالت, krb5->context, &داخلا_token,
                            &output_token, عدم, &qop);
  إذا(GSS_ERROR(major_حالت)) {
    Curl_gss_log_error(معلومات, "gss_unwrap() failed: ",
                       major_حالت, minor_حالت);
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  إذا(output_token.ڊيگهائي != 4) {
    infof(معلومات, "GSSAPI handshake failure (invalid security data)");
    gss_release_بفر(&اڻ_استعمال_ٿيل_حالت, &output_token);
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = output_token.قدر;
  sec_layer = indata[0];
  max_سائيز = ((بنانشان صحيح)indata[1] << 16) |
             ((بنانشان صحيح)indata[2] << 8) | indata[3];

  /* Free the challenge as it is not required anymore */
  gss_release_بفر(&اڻ_استعمال_ٿيل_حالت, &output_token);

  /* Process the security layer */
  إذا(!(sec_layer & GSSAUTH_P_NONE)) {
    infof(معلومات, "GSSAPI handshake failure (invalid security layer)");

    أرجع CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= GSSAUTH_P_NONE;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  إذا(max_سائيز > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_سائيز = 0;
  }

  /* Allocate our message */
  messagelen = 4;
  إذا(authzid)
    messagelen += لڙي_ڊيگهه(authzid);
  پيغام = curlx_malloc(messagelen);
  إذا(!پيغام)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  /* Populate the message with the security layer and client supported receive
     message size. */
  پيغام[0] = sec_layer & 0xFF;
  پيغام[1] = (max_سائيز >> 16) & 0xFF;
  پيغام[2] = (max_سائيز >> 8) & 0xFF;
  پيغام[3] = max_سائيز & 0xFF;

  /* If given, append the authorization identity. */

  إذا(authzid && *authzid)
    يادگيري_نقل(پيغام + 4, authzid, messagelen - 4);

  /* Setup the "authentication data" security buffer */
  داخلا_token.قدر = پيغام;
  داخلا_token.ڊيگهائي = messagelen;

  /* Encrypt the data */
  major_حالت = gss_wrap(&minor_حالت, krb5->context, 0,
                          GSS_C_QOP_DEFAULT, &داخلا_token, عدم,
                          &output_token);
  إذا(GSS_ERROR(major_حالت)) {
    Curl_gss_log_error(معلومات, "gss_wrap() failed: ",
                       major_حالت, minor_حالت);
    curlx_free(پيغام);
    أرجع CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  نتيجو = Curl_bufref_memdup0(ٻاهريون, output_token.قدر, output_token.ڊيگهائي);
  /* Free the output buffer */
  gss_release_بفر(&اڻ_استعمال_ٿيل_حالت, &output_token);

  /* Free the message buffer */
  curlx_free(پيغام);

  أرجع نتيجو;
}

/*
 * Curl_auth_cleanup_gssapi()
 *
 * This is used to clean up the GSSAPI (Kerberos V5) specific data.
 *
 * Parameters:
 *
 * krb5     [in/out] - The Kerberos 5 data struct being cleaned up.
 *
 */
فراغ Curl_auth_cleanup_gssapi(بنية kerberos5data *krb5)
{
  OM_uint32 minor_حالت;

  /* Free our security context */
  إذا(krb5->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_حالت, &krb5->context,
                                GSS_C_NO_بفر);
    krb5->context = GSS_C_NO_CONTEXT;
  }

  /* Free the SPN */
  إذا(krb5->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_حالت, &krb5->spn);
    krb5->spn = GSS_C_NO_NAME;
  }
}

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HAVE_GSSAPI && USE_KERBEROS5 */
