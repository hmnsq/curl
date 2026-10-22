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

#यदि defined(HAVE_GSSAPI) && defined(USE_KERBEROS5)

#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "curl_sasl.h"
#अन्तर्भाव "curl_gssapi.h"
#अन्तर्भाव "curl_trc.h"

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wdeprecated-declarations"
#यदि_अन्तः

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
CURLcode Curl_auth_create_gssapi_user_सन्देशः(संरचना Curl_easy *दत्त,
                                              संरचना Curl_creds *creds,
                                              ध्रुव वर्ण *default_service,
                                              ध्रुव वर्ण *host,
                                              ध्रुव bool mutual_auth,
                                              ध्रुव संरचना bufref *chlg,
                                              संरचना kerberos5data *krb5,
                                              संरचना bufref *बहिर्गमनम्)
{
  CURLcode परिणाम = CURLE_OK;
  OM_uint32 major_स्थितिः;
  OM_uint32 minor_स्थितिः;
  OM_uint32 अप्रयुक्तम्_स्थितिः;
  gss_बफर_desc निवेशः_token = GSS_C_रिक्तानि_बफर;
  gss_बफर_desc output_token = GSS_C_रिक्तानि_बफर;
  ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  यदि(!krb5->spn) {
    gss_बफर_desc spn_token = GSS_C_रिक्तानि_बफर;

    /* Generate our SPN */
    वर्ण *spn = Curl_auth_build_spn(service, रिक्त, host);
    यदि(!spn)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    /* Populate the SPN structure */
    spn_token.मानम् = spn;
    spn_token.दीर्घता = वाक्यदैर्घ्यम्(spn);

    /* Import the SPN */
    major_स्थितिः = gss_import_name(&minor_स्थितिः, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE, &krb5->spn);
    यदि(GSS_ERROR(major_स्थितिः)) {
      Curl_gss_log_error(दत्त, "gss_import_name() failed: ",
                         major_स्थितिः, minor_स्थितिः);

      curlx_free(spn);

      प्रत्यागम CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  यदि(chlg) {
    यदि(!Curl_bufref_len(chlg)) {
      infof(दत्त, "GSSAPI handshake failure (empty challenge message)");
      प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
    }
    निवेशः_token.मानम् = CURL_UNCONST(Curl_bufref_ptr(chlg));
    निवेशः_token.दीर्घता = Curl_bufref_len(chlg);
  }

  major_स्थितिः = Curl_gss_init_sec_context(दत्त,
                                           &minor_स्थितिः,
                                           &krb5->context,
                                           krb5->spn,
                                           &Curl_krb5_mech_oid,
                                           GSS_C_NO_CHANNEL_BINDINGS,
                                           &निवेशः_token,
                                           &output_token,
                                           mutual_auth,
                                           रिक्त,
                                           GSS_C_NO_CREDENTIAL);

  यदि(GSS_ERROR(major_स्थितिः)) {
    यदि(output_token.मानम्)
      gss_release_बफर(&अप्रयुक्तम्_स्थितिः, &output_token);

    Curl_gss_log_error(दत्त, "gss_init_sec_context() failed: ",
                       major_स्थितिः, minor_स्थितिः);

    प्रत्यागम CURLE_AUTH_ERROR;
  }

  यदि(output_token.मानम् && output_token.दीर्घता) {
    परिणाम = Curl_bufref_memdup0(बहिर्गमनम्, output_token.मानम्, output_token.दीर्घता);
    gss_release_बफर(&अप्रयुक्तम्_स्थितिः, &output_token);
  }
  अन्यथा
    Curl_bufref_समुच्चयः(बहिर्गमनम्, mutual_auth ? "" : रिक्त, 0, रिक्त);

  प्रत्यागम परिणाम;
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
CURLcode Curl_auth_create_gssapi_security_सन्देशः(संरचना Curl_easy *दत्त,
                                                  ध्रुव वर्ण *authzid,
                                                  ध्रुव संरचना bufref *chlg,
                                                  संरचना kerberos5data *krb5,
                                                  संरचना bufref *बहिर्गमनम्)
{
  CURLcode परिणाम = CURLE_OK;
  size_t messagelen = 0;
  अचिह्नित वर्ण *सन्देशः_2 = रिक्त;
  OM_uint32 major_स्थितिः;
  OM_uint32 minor_स्थितिः;
  OM_uint32 अप्रयुक्तम्_स्थितिः;
  gss_बफर_desc निवेशः_token = GSS_C_रिक्तानि_बफर;
  gss_बफर_desc output_token = GSS_C_रिक्तानि_बफर;
  अचिह्नित वर्ण *indata;
  gss_qop_t qop = GSS_C_QOP_DEFAULT;
  अचिह्नित पूर्णाङ्क sec_layer = 0;
  अचिह्नित पूर्णाङ्क max_आकार = 0;

  /* Ensure we have a valid challenge message */
  यदि(!Curl_bufref_len(chlg)) {
    infof(दत्त, "GSSAPI handshake failure (empty security message)");
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Setup the challenge "input" security buffer */
  निवेशः_token.मानम् = CURL_UNCONST(Curl_bufref_ptr(chlg));
  निवेशः_token.दीर्घता = Curl_bufref_len(chlg);

  /* Decrypt the inbound challenge and obtain the qop */
  major_स्थितिः = gss_unwrap(&minor_स्थितिः, krb5->context, &निवेशः_token,
                            &output_token, रिक्त, &qop);
  यदि(GSS_ERROR(major_स्थितिः)) {
    Curl_gss_log_error(दत्त, "gss_unwrap() failed: ",
                       major_स्थितिः, minor_स्थितिः);
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  यदि(output_token.दीर्घता != 4) {
    infof(दत्त, "GSSAPI handshake failure (invalid security data)");
    gss_release_बफर(&अप्रयुक्तम्_स्थितिः, &output_token);
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = output_token.मानम्;
  sec_layer = indata[0];
  max_आकार = ((अचिह्नित पूर्णाङ्क)indata[1] << 16) |
             ((अचिह्नित पूर्णाङ्क)indata[2] << 8) | indata[3];

  /* Free the challenge as it is not required anymore */
  gss_release_बफर(&अप्रयुक्तम्_स्थितिः, &output_token);

  /* Process the security layer */
  यदि(!(sec_layer & GSSAUTH_P_NONE)) {
    infof(दत्त, "GSSAPI handshake failure (invalid security layer)");

    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= GSSAUTH_P_NONE;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  यदि(max_आकार > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_आकार = 0;
  }

  /* Allocate our message */
  messagelen = 4;
  यदि(authzid)
    messagelen += वाक्यदैर्घ्यम्(authzid);
  सन्देशः_2 = curlx_malloc(messagelen);
  यदि(!सन्देशः_2)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Populate the message with the security layer and client supported receive
     message size. */
  सन्देशः_2[0] = sec_layer & 0xFF;
  सन्देशः_2[1] = (max_आकार >> 16) & 0xFF;
  सन्देशः_2[2] = (max_आकार >> 8) & 0xFF;
  सन्देशः_2[3] = max_आकार & 0xFF;

  /* If given, append the authorization identity. */

  यदि(authzid && *authzid)
    स्मृतिं_प्रतिलिख(सन्देशः_2 + 4, authzid, messagelen - 4);

  /* Setup the "authentication data" security buffer */
  निवेशः_token.मानम् = सन्देशः_2;
  निवेशः_token.दीर्घता = messagelen;

  /* Encrypt the data */
  major_स्थितिः = gss_wrap(&minor_स्थितिः, krb5->context, 0,
                          GSS_C_QOP_DEFAULT, &निवेशः_token, रिक्त,
                          &output_token);
  यदि(GSS_ERROR(major_स्थितिः)) {
    Curl_gss_log_error(दत्त, "gss_wrap() failed: ",
                       major_स्थितिः, minor_स्थितिः);
    curlx_free(सन्देशः_2);
    प्रत्यागम CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  परिणाम = Curl_bufref_memdup0(बहिर्गमनम्, output_token.मानम्, output_token.दीर्घता);
  /* Free the output buffer */
  gss_release_बफर(&अप्रयुक्तम्_स्थितिः, &output_token);

  /* Free the message buffer */
  curlx_free(सन्देशः_2);

  प्रत्यागम परिणाम;
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
  OM_uint32 minor_स्थितिः;

  /* Free our security context */
  यदि(krb5->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_स्थितिः, &krb5->context,
                                GSS_C_NO_बफर);
    krb5->context = GSS_C_NO_CONTEXT;
  }

  /* Free the SPN */
  यदि(krb5->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_स्थितिः, &krb5->spn);
    krb5->spn = GSS_C_NO_NAME;
  }
}

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः

#यदि_अन्तः /* HAVE_GSSAPI && USE_KERBEROS5 */
