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
#pragma GCC diagnostic ادفع
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

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
CURLcode Curl_auth_create_gssapi_user_message(بنية Curl_easy *data,
                                              بنية Curl_creds *creds,
                                              ثابت حرف *default_service,
                                              ثابت حرف *host,
                                              ثابت bool mutual_auth,
                                              ثابت بنية bufref *chlg,
                                              بنية kerberos5data *krb5,
                                              بنية bufref *out)
{
  CURLcode result = CURLE_OK;
  OM_uint32 major_status;
  OM_uint32 minor_status;
  OM_uint32 unused_status;
  gss_buffer_desc مدخل_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  إذا(!krb5->spn) {
    gss_buffer_desc spn_token = GSS_C_EMPTY_BUFFER;

    /* Generate our SPN */
    حرف *spn = Curl_auth_build_spn(service, عدم, host);
    إذا(!spn)
      أرجع CURLE_OUT_OF_MEMORY;

    /* Populate the SPN structure */
    spn_token.قيمة = spn;
    spn_token.length = strlen(spn);

    /* Import the SPN */
    major_status = gss_import_name(&minor_status, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE, &krb5->spn);
    إذا(GSS_ERROR(major_status)) {
      Curl_gss_log_error(data, "gss_import_name() failed: ",
                         major_status, minor_status);

      curlx_free(spn);

      أرجع CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  إذا(chlg) {
    إذا(!Curl_bufref_len(chlg)) {
      infof(data, "GSSAPI handshake failure (empty challenge message)");
      أرجع CURLE_BAD_CONTENT_ENCODING;
    }
    مدخل_token.قيمة = CURL_UNCONST(Curl_bufref_ptr(chlg));
    مدخل_token.length = Curl_bufref_len(chlg);
  }

  major_status = Curl_gss_init_sec_context(data,
                                           &minor_status,
                                           &krb5->context,
                                           krb5->spn,
                                           &Curl_krb5_mech_oid,
                                           GSS_C_NO_CHANNEL_BINDINGS,
                                           &مدخل_token,
                                           &output_token,
                                           mutual_auth,
                                           عدم,
                                           GSS_C_NO_CREDENTIAL);

  إذا(GSS_ERROR(major_status)) {
    إذا(output_token.قيمة)
      gss_release_buffer(&unused_status, &output_token);

    Curl_gss_log_error(data, "gss_init_sec_context() failed: ",
                       major_status, minor_status);

    أرجع CURLE_AUTH_ERROR;
  }

  إذا(output_token.قيمة && output_token.length) {
    result = Curl_bufref_memdup0(out, output_token.قيمة, output_token.length);
    gss_release_buffer(&unused_status, &output_token);
  }
  وإلا
    Curl_bufref_set(out, mutual_auth ? "" : عدم, 0, عدم);

  أرجع result;
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
CURLcode Curl_auth_create_gssapi_security_message(بنية Curl_easy *data,
                                                  ثابت حرف *authzid,
                                                  ثابت بنية bufref *chlg,
                                                  بنية kerberos5data *krb5,
                                                  بنية bufref *out)
{
  CURLcode result = CURLE_OK;
  size_t messagelen = 0;
  unsigned حرف *message = عدم;
  OM_uint32 major_status;
  OM_uint32 minor_status;
  OM_uint32 unused_status;
  gss_buffer_desc مدخل_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;
  unsigned حرف *indata;
  gss_qop_t qop = GSS_C_QOP_DEFAULT;
  unsigned صحيح sec_layer = 0;
  unsigned صحيح max_size = 0;

  /* Ensure we have a valid challenge message */
  إذا(!Curl_bufref_len(chlg)) {
    infof(data, "GSSAPI handshake failure (empty security message)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Setup the challenge "input" security buffer */
  مدخل_token.قيمة = CURL_UNCONST(Curl_bufref_ptr(chlg));
  مدخل_token.length = Curl_bufref_len(chlg);

  /* Decrypt the inbound challenge and obtain the qop */
  major_status = gss_unwrap(&minor_status, krb5->context, &مدخل_token,
                            &output_token, عدم, &qop);
  إذا(GSS_ERROR(major_status)) {
    Curl_gss_log_error(data, "gss_unwrap() failed: ",
                       major_status, minor_status);
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  إذا(output_token.length != 4) {
    infof(data, "GSSAPI handshake failure (invalid security data)");
    gss_release_buffer(&unused_status, &output_token);
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = output_token.قيمة;
  sec_layer = indata[0];
  max_size = ((unsigned صحيح)indata[1] << 16) |
             ((unsigned صحيح)indata[2] << 8) | indata[3];

  /* Free the challenge as it is not required anymore */
  gss_release_buffer(&unused_status, &output_token);

  /* Process the security layer */
  إذا(!(sec_layer & GSSAUTH_P_NONE)) {
    infof(data, "GSSAPI handshake failure (invalid security layer)");

    أرجع CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= GSSAUTH_P_NONE;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  إذا(max_size > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_size = 0;
  }

  /* Allocate our message */
  messagelen = 4;
  إذا(authzid)
    messagelen += strlen(authzid);
  message = curlx_malloc(messagelen);
  إذا(!message)
    أرجع CURLE_OUT_OF_MEMORY;

  /* Populate the message with the security layer and client supported receive
     message size. */
  message[0] = sec_layer & 0xFF;
  message[1] = (max_size >> 16) & 0xFF;
  message[2] = (max_size >> 8) & 0xFF;
  message[3] = max_size & 0xFF;

  /* If given, append the authorization identity. */

  إذا(authzid && *authzid)
    memcpy(message + 4, authzid, messagelen - 4);

  /* Setup the "authentication data" security buffer */
  مدخل_token.قيمة = message;
  مدخل_token.length = messagelen;

  /* Encrypt the data */
  major_status = gss_wrap(&minor_status, krb5->context, 0,
                          GSS_C_QOP_DEFAULT, &مدخل_token, عدم,
                          &output_token);
  إذا(GSS_ERROR(major_status)) {
    Curl_gss_log_error(data, "gss_wrap() failed: ",
                       major_status, minor_status);
    curlx_free(message);
    أرجع CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  result = Curl_bufref_memdup0(out, output_token.قيمة, output_token.length);
  /* Free the output buffer */
  gss_release_buffer(&unused_status, &output_token);

  /* Free the message buffer */
  curlx_free(message);

  أرجع result;
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
  OM_uint32 minor_status;

  /* Free our security context */
  إذا(krb5->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_status, &krb5->context,
                                GSS_C_NO_BUFFER);
    krb5->context = GSS_C_NO_CONTEXT;
  }

  /* Free the SPN */
  إذا(krb5->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_status, &krb5->spn);
    krb5->spn = GSS_C_NO_NAME;
  }
}

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic اسحب
#endif

#endif /* HAVE_GSSAPI && USE_KERBEROS5 */
