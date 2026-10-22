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
#شامل "curl_setup.h"

#اگر defined(HAVE_GSSAPI) && defined(USE_KERBEROS5)

#شامل "vauth/vauth.h"
#شامل "curl_sasl.h"
#شامل "curl_gssapi.h"
#شامل "curl_trc.h"

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wdeprecated-declarations"
#اگر_ختم

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by the GSS-API library.
 */
bool Curl_auth_is_gssapi_supported(خالی)
{
  واپس TRUE;
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
CURLcode Curl_auth_create_gssapi_user_پیغام(ساخت Curl_easy *ڈیٹا,
                                              ساخت Curl_creds *creds,
                                              ثابت حرف *default_service,
                                              ثابت حرف *host,
                                              ثابت bool mutual_auth,
                                              ثابت ساخت bufref *chlg,
                                              ساخت kerberos5data *krb5,
                                              ساخت bufref *برآمد)
{
  CURLcode نتیجہ = CURLE_OK;
  OM_uint32 major_حالت;
  OM_uint32 minor_حالت;
  OM_uint32 غیر_مستعمل_حالت;
  gss_بفر_desc داخلہ_token = GSS_C_خالی_خانے_بفر;
  gss_بفر_desc output_token = GSS_C_خالی_خانے_بفر;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  اگر(!krb5->spn) {
    gss_بفر_desc spn_token = GSS_C_خالی_خانے_بفر;

    /* Generate our SPN */
    حرف *spn = Curl_auth_build_spn(service, تہی, host);
    اگر(!spn)
      واپس CURLE_برآمد_کا_حافظہ;

    /* Populate the SPN structure */
    spn_token.قدر = spn;
    spn_token.لمبائی = لڑی_لمبائی(spn);

    /* Import the SPN */
    major_حالت = gss_import_name(&minor_حالت, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE, &krb5->spn);
    اگر(GSS_ERROR(major_حالت)) {
      Curl_gss_log_error(ڈیٹا, "gss_import_name() failed: ",
                         major_حالت, minor_حالت);

      curlx_free(spn);

      واپس CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  اگر(chlg) {
    اگر(!Curl_bufref_len(chlg)) {
      infof(ڈیٹا, "GSSAPI handshake failure (empty challenge message)");
      واپس CURLE_BAD_CONTENT_ENCODING;
    }
    داخلہ_token.قدر = CURL_UNCONST(Curl_bufref_ptr(chlg));
    داخلہ_token.لمبائی = Curl_bufref_len(chlg);
  }

  major_حالت = Curl_gss_init_sec_context(ڈیٹا,
                                           &minor_حالت,
                                           &krb5->context,
                                           krb5->spn,
                                           &Curl_krb5_mech_oid,
                                           GSS_C_NO_CHANNEL_BINDINGS,
                                           &داخلہ_token,
                                           &output_token,
                                           mutual_auth,
                                           تہی,
                                           GSS_C_NO_CREDENTIAL);

  اگر(GSS_ERROR(major_حالت)) {
    اگر(output_token.قدر)
      gss_release_بفر(&غیر_مستعمل_حالت, &output_token);

    Curl_gss_log_error(ڈیٹا, "gss_init_sec_context() failed: ",
                       major_حالت, minor_حالت);

    واپس CURLE_AUTH_ERROR;
  }

  اگر(output_token.قدر && output_token.لمبائی) {
    نتیجہ = Curl_bufref_memdup0(برآمد, output_token.قدر, output_token.لمبائی);
    gss_release_بفر(&غیر_مستعمل_حالت, &output_token);
  }
  ورنہ
    Curl_bufref_سیٹ(برآمد, mutual_auth ? "" : تہی, 0, تہی);

  واپس نتیجہ;
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
CURLcode Curl_auth_create_gssapi_security_پیغام(ساخت Curl_easy *ڈیٹا,
                                                  ثابت حرف *authzid,
                                                  ثابت ساخت bufref *chlg,
                                                  ساخت kerberos5data *krb5,
                                                  ساخت bufref *برآمد)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t messagelen = 0;
  غیرعلامتی حرف *پیغام_2 = تہی;
  OM_uint32 major_حالت;
  OM_uint32 minor_حالت;
  OM_uint32 غیر_مستعمل_حالت;
  gss_بفر_desc داخلہ_token = GSS_C_خالی_خانے_بفر;
  gss_بفر_desc output_token = GSS_C_خالی_خانے_بفر;
  غیرعلامتی حرف *indata;
  gss_qop_t qop = GSS_C_QOP_DEFAULT;
  غیرعلامتی صحیح sec_layer = 0;
  غیرعلامتی صحیح max_سائز = 0;

  /* Ensure we have a valid challenge message */
  اگر(!Curl_bufref_len(chlg)) {
    infof(ڈیٹا, "GSSAPI handshake failure (empty security message)");
    واپس CURLE_BAD_CONTENT_ENCODING;
  }

  /* Setup the challenge "input" security buffer */
  داخلہ_token.قدر = CURL_UNCONST(Curl_bufref_ptr(chlg));
  داخلہ_token.لمبائی = Curl_bufref_len(chlg);

  /* Decrypt the inbound challenge and obtain the qop */
  major_حالت = gss_unwrap(&minor_حالت, krb5->context, &داخلہ_token,
                            &output_token, تہی, &qop);
  اگر(GSS_ERROR(major_حالت)) {
    Curl_gss_log_error(ڈیٹا, "gss_unwrap() failed: ",
                       major_حالت, minor_حالت);
    واپس CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  اگر(output_token.لمبائی != 4) {
    infof(ڈیٹا, "GSSAPI handshake failure (invalid security data)");
    gss_release_بفر(&غیر_مستعمل_حالت, &output_token);
    واپس CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = output_token.قدر;
  sec_layer = indata[0];
  max_سائز = ((غیرعلامتی صحیح)indata[1] << 16) |
             ((غیرعلامتی صحیح)indata[2] << 8) | indata[3];

  /* Free the challenge as it is not required anymore */
  gss_release_بفر(&غیر_مستعمل_حالت, &output_token);

  /* Process the security layer */
  اگر(!(sec_layer & GSSAUTH_P_NONE)) {
    infof(ڈیٹا, "GSSAPI handshake failure (invalid security layer)");

    واپس CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= GSSAUTH_P_NONE;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  اگر(max_سائز > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_سائز = 0;
  }

  /* Allocate our message */
  messagelen = 4;
  اگر(authzid)
    messagelen += لڑی_لمبائی(authzid);
  پیغام_2 = curlx_malloc(messagelen);
  اگر(!پیغام_2)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Populate the message with the security layer and client supported receive
     message size. */
  پیغام_2[0] = sec_layer & 0xFF;
  پیغام_2[1] = (max_سائز >> 16) & 0xFF;
  پیغام_2[2] = (max_سائز >> 8) & 0xFF;
  پیغام_2[3] = max_سائز & 0xFF;

  /* If given, append the authorization identity. */

  اگر(authzid && *authzid)
    حافظہ_نقل(پیغام_2 + 4, authzid, messagelen - 4);

  /* Setup the "authentication data" security buffer */
  داخلہ_token.قدر = پیغام_2;
  داخلہ_token.لمبائی = messagelen;

  /* Encrypt the data */
  major_حالت = gss_wrap(&minor_حالت, krb5->context, 0,
                          GSS_C_QOP_DEFAULT, &داخلہ_token, تہی,
                          &output_token);
  اگر(GSS_ERROR(major_حالت)) {
    Curl_gss_log_error(ڈیٹا, "gss_wrap() failed: ",
                       major_حالت, minor_حالت);
    curlx_free(پیغام_2);
    واپس CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  نتیجہ = Curl_bufref_memdup0(برآمد, output_token.قدر, output_token.لمبائی);
  /* Free the output buffer */
  gss_release_بفر(&غیر_مستعمل_حالت, &output_token);

  /* Free the message buffer */
  curlx_free(پیغام_2);

  واپس نتیجہ;
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
خالی Curl_auth_cleanup_gssapi(ساخت kerberos5data *krb5)
{
  OM_uint32 minor_حالت;

  /* Free our security context */
  اگر(krb5->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_حالت, &krb5->context,
                                GSS_C_NO_بفر);
    krb5->context = GSS_C_NO_CONTEXT;
  }

  /* Free the SPN */
  اگر(krb5->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_حالت, &krb5->spn);
    krb5->spn = GSS_C_NO_NAME;
  }
}

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ہدایت GCC diagnostic نکالو
#اگر_ختم

#اگر_ختم /* HAVE_GSSAPI && USE_KERBEROS5 */
