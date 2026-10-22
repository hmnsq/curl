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
#अन्तर्भाव "curl_setup.h"

#जइ defined(HAVE_GSSAPI) && defined(USE_KERBEROS5)

#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "curl_sasl.h"
#अन्तर्भाव "curl_gssapi.h"
#अन्तर्भाव "curl_trc.h"

#जइ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic प्रक्षिप
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
bool Curl_auth_is_gssapi_supported(शून्य)
{
  प्रत्यागम TRUE;
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
CURLcode Curl_auth_create_gssapi_user_message(संरचना Curl_easy *data,
                                              संरचना Curl_creds *creds,
                                              ध्रुव वर्ण *default_service,
                                              ध्रुव वर्ण *host,
                                              ध्रुव bool mutual_auth,
                                              ध्रुव संरचना bufref *chlg,
                                              संरचना kerberos5data *krb5,
                                              संरचना bufref *out)
{
  CURLcode result = CURLE_OK;
  OM_uint32 major_status;
  OM_uint32 minor_status;
  OM_uint32 unused_status;
  gss_buffer_desc निवेशः_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;
  ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  जइ(!krb5->spn) {
    gss_buffer_desc spn_token = GSS_C_EMPTY_BUFFER;

    /* Generate our SPN */
    वर्ण *spn = Curl_auth_build_spn(service, रिक्त, host);
    जइ(!spn)
      प्रत्यागम CURLE_OUT_OF_MEMORY;

    /* Populate the SPN structure */
    spn_token.मानम् = spn;
    spn_token.length = strlen(spn);

    /* Import the SPN */
    major_status = gss_import_name(&minor_status, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE, &krb5->spn);
    जइ(GSS_ERROR(major_status)) {
      Curl_gss_log_error(data, "gss_import_name() failed: ",
                         major_status, minor_status);

      curlx_free(spn);

      प्रत्यागम CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  जइ(chlg) {
    जइ(!Curl_bufref_len(chlg)) {
      infof(data, "GSSAPI handshake failure (empty challenge message)");
      प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
    }
    निवेशः_token.मानम् = CURL_UNCONST(Curl_bufref_ptr(chlg));
    निवेशः_token.length = Curl_bufref_len(chlg);
  }

  major_status = Curl_gss_init_sec_context(data,
                                           &minor_status,
                                           &krb5->context,
                                           krb5->spn,
                                           &Curl_krb5_mech_oid,
                                           GSS_C_NO_CHANNEL_BINDINGS,
                                           &निवेशः_token,
                                           &output_token,
                                           mutual_auth,
                                           रिक्त,
                                           GSS_C_NO_CREDENTIAL);

  जइ(GSS_ERROR(major_status)) {
    जइ(output_token.मानम्)
      gss_release_buffer(&unused_status, &output_token);

    Curl_gss_log_error(data, "gss_init_sec_context() failed: ",
                       major_status, minor_status);

    प्रत्यागम CURLE_AUTH_ERROR;
  }

  जइ(output_token.मानम् && output_token.length) {
    result = Curl_bufref_memdup0(out, output_token.मानम्, output_token.length);
    gss_release_buffer(&unused_status, &output_token);
  }
  अण्णहा
    Curl_bufref_set(out, mutual_auth ? "" : रिक्त, 0, रिक्त);

  प्रत्यागम result;
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
CURLcode Curl_auth_create_gssapi_security_message(संरचना Curl_easy *data,
                                                  ध्रुव वर्ण *authzid,
                                                  ध्रुव संरचना bufref *chlg,
                                                  संरचना kerberos5data *krb5,
                                                  संरचना bufref *out)
{
  CURLcode result = CURLE_OK;
  size_t messagelen = 0;
  unsigned वर्ण *message = रिक्त;
  OM_uint32 major_status;
  OM_uint32 minor_status;
  OM_uint32 unused_status;
  gss_buffer_desc निवेशः_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;
  unsigned वर्ण *indata;
  gss_qop_t qop = GSS_C_QOP_DEFAULT;
  unsigned पूर्णाङ्क sec_layer = 0;
  unsigned पूर्णाङ्क max_size = 0;

  /* Ensure we have a valid challenge message */
  जइ(!Curl_bufref_len(chlg)) {
    infof(data, "GSSAPI handshake failure (empty security message)");
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Setup the challenge "input" security buffer */
  निवेशः_token.मानम् = CURL_UNCONST(Curl_bufref_ptr(chlg));
  निवेशः_token.length = Curl_bufref_len(chlg);

  /* Decrypt the inbound challenge and obtain the qop */
  major_status = gss_unwrap(&minor_status, krb5->context, &निवेशः_token,
                            &output_token, रिक्त, &qop);
  जइ(GSS_ERROR(major_status)) {
    Curl_gss_log_error(data, "gss_unwrap() failed: ",
                       major_status, minor_status);
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  जइ(output_token.length != 4) {
    infof(data, "GSSAPI handshake failure (invalid security data)");
    gss_release_buffer(&unused_status, &output_token);
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = output_token.मानम्;
  sec_layer = indata[0];
  max_size = ((unsigned पूर्णाङ्क)indata[1] << 16) |
             ((unsigned पूर्णाङ्क)indata[2] << 8) | indata[3];

  /* Free the challenge as it is not required anymore */
  gss_release_buffer(&unused_status, &output_token);

  /* Process the security layer */
  जइ(!(sec_layer & GSSAUTH_P_NONE)) {
    infof(data, "GSSAPI handshake failure (invalid security layer)");

    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= GSSAUTH_P_NONE;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  जइ(max_size > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_size = 0;
  }

  /* Allocate our message */
  messagelen = 4;
  जइ(authzid)
    messagelen += strlen(authzid);
  message = curlx_malloc(messagelen);
  जइ(!message)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  /* Populate the message with the security layer and client supported receive
     message size. */
  message[0] = sec_layer & 0xFF;
  message[1] = (max_size >> 16) & 0xFF;
  message[2] = (max_size >> 8) & 0xFF;
  message[3] = max_size & 0xFF;

  /* If given, append the authorization identity. */

  जइ(authzid && *authzid)
    memcpy(message + 4, authzid, messagelen - 4);

  /* Setup the "authentication data" security buffer */
  निवेशः_token.मानम् = message;
  निवेशः_token.length = messagelen;

  /* Encrypt the data */
  major_status = gss_wrap(&minor_status, krb5->context, 0,
                          GSS_C_QOP_DEFAULT, &निवेशः_token, रिक्त,
                          &output_token);
  जइ(GSS_ERROR(major_status)) {
    Curl_gss_log_error(data, "gss_wrap() failed: ",
                       major_status, minor_status);
    curlx_free(message);
    प्रत्यागम CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  result = Curl_bufref_memdup0(out, output_token.मानम्, output_token.length);
  /* Free the output buffer */
  gss_release_buffer(&unused_status, &output_token);

  /* Free the message buffer */
  curlx_free(message);

  प्रत्यागम result;
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
शून्य Curl_auth_cleanup_gssapi(संरचना kerberos5data *krb5)
{
  OM_uint32 minor_status;

  /* Free our security context */
  जइ(krb5->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_status, &krb5->context,
                                GSS_C_NO_BUFFER);
    krb5->context = GSS_C_NO_CONTEXT;
  }

  /* Free the SPN */
  जइ(krb5->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_status, &krb5->spn);
    krb5->spn = GSS_C_NO_NAME;
  }
}

#जइ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic निष्कासय
#endif

#endif /* HAVE_GSSAPI && USE_KERBEROS5 */
