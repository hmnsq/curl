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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(HAVE_GSSAPI) && defined(USE_KERBEROS5)

#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "curl_sasl.h"
#అన్తర్భావ "curl_gssapi.h"
#అన్తర్భావ "curl_trc.h"

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wdeprecated-declarations"
#అయితే_ముగింపు

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by the GSS-API library.
 */
bool Curl_auth_is_gssapi_supported(శూన్య)
{
  తిరిగి TRUE;
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
CURLcode Curl_auth_create_gssapi_user_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                              నిర్మాణం Curl_creds *creds,
                                              స్థిరరాశి అక్షరం *default_service,
                                              స్థిరరాశి అక్షరం *host,
                                              స్థిరరాశి bool mutual_auth,
                                              స్థిరరాశి నిర్మాణం bufref *chlg,
                                              నిర్మాణం kerberos5data *krb5,
                                              నిర్మాణం bufref *బహిర్గతం)
{
  CURLcode ఫలితం = CURLE_OK;
  OM_uint32 major_స్థితి;
  OM_uint32 minor_స్థితి;
  OM_uint32 వాడని_స్థితి;
  gss_బఫరు_desc ఆదానం_token = GSS_C_ఖాళీలు_బఫరు;
  gss_బఫరు_desc output_token = GSS_C_ఖాళీలు_బఫరు;
  స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  ఒకవేళ(!krb5->spn) {
    gss_బఫరు_desc spn_token = GSS_C_ఖాళీలు_బఫరు;

    /* Generate our SPN */
    అక్షరం *spn = Curl_auth_build_spn(service, ఖాళీ, host);
    ఒకవేళ(!spn)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    /* Populate the SPN structure */
    spn_token.విలువ = spn;
    spn_token.నిడివి = అక్షరశ్రేణి_పొడవు(spn);

    /* Import the SPN */
    major_స్థితి = gss_import_name(&minor_స్థితి, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE, &krb5->spn);
    ఒకవేళ(GSS_ERROR(major_స్థితి)) {
      Curl_gss_log_error(దత్తాంశం, "gss_import_name() failed: ",
                         major_స్థితి, minor_స్థితి);

      curlx_free(spn);

      తిరిగి CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  ఒకవేళ(chlg) {
    ఒకవేళ(!Curl_bufref_len(chlg)) {
      infof(దత్తాంశం, "GSSAPI handshake failure (empty challenge message)");
      తిరిగి CURLE_BAD_CONTENT_ENCODING;
    }
    ఆదానం_token.విలువ = CURL_UNCONST(Curl_bufref_ptr(chlg));
    ఆదానం_token.నిడివి = Curl_bufref_len(chlg);
  }

  major_స్థితి = Curl_gss_init_sec_context(దత్తాంశం,
                                           &minor_స్థితి,
                                           &krb5->context,
                                           krb5->spn,
                                           &Curl_krb5_mech_oid,
                                           GSS_C_NO_CHANNEL_BINDINGS,
                                           &ఆదానం_token,
                                           &output_token,
                                           mutual_auth,
                                           ఖాళీ,
                                           GSS_C_NO_CREDENTIAL);

  ఒకవేళ(GSS_ERROR(major_స్థితి)) {
    ఒకవేళ(output_token.విలువ)
      gss_release_బఫరు(&వాడని_స్థితి, &output_token);

    Curl_gss_log_error(దత్తాంశం, "gss_init_sec_context() failed: ",
                       major_స్థితి, minor_స్థితి);

    తిరిగి CURLE_AUTH_ERROR;
  }

  ఒకవేళ(output_token.విలువ && output_token.నిడివి) {
    ఫలితం = Curl_bufref_memdup0(బహిర్గతం, output_token.విలువ, output_token.నిడివి);
    gss_release_బఫరు(&వాడని_స్థితి, &output_token);
  }
  లేకపోతే
    Curl_bufref_సమితి(బహిర్గతం, mutual_auth ? "" : ఖాళీ, 0, ఖాళీ);

  తిరిగి ఫలితం;
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
CURLcode Curl_auth_create_gssapi_security_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                                  స్థిరరాశి అక్షరం *authzid,
                                                  స్థిరరాశి నిర్మాణం bufref *chlg,
                                                  నిర్మాణం kerberos5data *krb5,
                                                  నిర్మాణం bufref *బహిర్గతం)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t messagelen = 0;
  గుర్తులేని అక్షరం *సందేశం_2 = ఖాళీ;
  OM_uint32 major_స్థితి;
  OM_uint32 minor_స్థితి;
  OM_uint32 వాడని_స్థితి;
  gss_బఫరు_desc ఆదానం_token = GSS_C_ఖాళీలు_బఫరు;
  gss_బఫరు_desc output_token = GSS_C_ఖాళీలు_బఫరు;
  గుర్తులేని అక్షరం *indata;
  gss_qop_t qop = GSS_C_QOP_DEFAULT;
  గుర్తులేని పూర్ణాంకం sec_layer = 0;
  గుర్తులేని పూర్ణాంకం max_కొలత = 0;

  /* Ensure we have a valid challenge message */
  ఒకవేళ(!Curl_bufref_len(chlg)) {
    infof(దత్తాంశం, "GSSAPI handshake failure (empty security message)");
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }

  /* Setup the challenge "input" security buffer */
  ఆదానం_token.విలువ = CURL_UNCONST(Curl_bufref_ptr(chlg));
  ఆదానం_token.నిడివి = Curl_bufref_len(chlg);

  /* Decrypt the inbound challenge and obtain the qop */
  major_స్థితి = gss_unwrap(&minor_స్థితి, krb5->context, &ఆదానం_token,
                            &output_token, ఖాళీ, &qop);
  ఒకవేళ(GSS_ERROR(major_స్థితి)) {
    Curl_gss_log_error(దత్తాంశం, "gss_unwrap() failed: ",
                       major_స్థితి, minor_స్థితి);
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  ఒకవేళ(output_token.నిడివి != 4) {
    infof(దత్తాంశం, "GSSAPI handshake failure (invalid security data)");
    gss_release_బఫరు(&వాడని_స్థితి, &output_token);
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = output_token.విలువ;
  sec_layer = indata[0];
  max_కొలత = ((గుర్తులేని పూర్ణాంకం)indata[1] << 16) |
             ((గుర్తులేని పూర్ణాంకం)indata[2] << 8) | indata[3];

  /* Free the challenge as it is not required anymore */
  gss_release_బఫరు(&వాడని_స్థితి, &output_token);

  /* Process the security layer */
  ఒకవేళ(!(sec_layer & GSSAUTH_P_NONE)) {
    infof(దత్తాంశం, "GSSAPI handshake failure (invalid security layer)");

    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= GSSAUTH_P_NONE;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  ఒకవేళ(max_కొలత > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_కొలత = 0;
  }

  /* Allocate our message */
  messagelen = 4;
  ఒకవేళ(authzid)
    messagelen += అక్షరశ్రేణి_పొడవు(authzid);
  సందేశం_2 = curlx_malloc(messagelen);
  ఒకవేళ(!సందేశం_2)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Populate the message with the security layer and client supported receive
     message size. */
  సందేశం_2[0] = sec_layer & 0xFF;
  సందేశం_2[1] = (max_కొలత >> 16) & 0xFF;
  సందేశం_2[2] = (max_కొలత >> 8) & 0xFF;
  సందేశం_2[3] = max_కొలత & 0xFF;

  /* If given, append the authorization identity. */

  ఒకవేళ(authzid && *authzid)
    జ్ఞాపకం_నకలు(సందేశం_2 + 4, authzid, messagelen - 4);

  /* Setup the "authentication data" security buffer */
  ఆదానం_token.విలువ = సందేశం_2;
  ఆదానం_token.నిడివి = messagelen;

  /* Encrypt the data */
  major_స్థితి = gss_wrap(&minor_స్థితి, krb5->context, 0,
                          GSS_C_QOP_DEFAULT, &ఆదానం_token, ఖాళీ,
                          &output_token);
  ఒకవేళ(GSS_ERROR(major_స్థితి)) {
    Curl_gss_log_error(దత్తాంశం, "gss_wrap() failed: ",
                       major_స్థితి, minor_స్థితి);
    curlx_free(సందేశం_2);
    తిరిగి CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  ఫలితం = Curl_bufref_memdup0(బహిర్గతం, output_token.విలువ, output_token.నిడివి);
  /* Free the output buffer */
  gss_release_బఫరు(&వాడని_స్థితి, &output_token);

  /* Free the message buffer */
  curlx_free(సందేశం_2);

  తిరిగి ఫలితం;
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
శూన్య Curl_auth_cleanup_gssapi(నిర్మాణం kerberos5data *krb5)
{
  OM_uint32 minor_స్థితి;

  /* Free our security context */
  ఒకవేళ(krb5->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_స్థితి, &krb5->context,
                                GSS_C_NO_బఫరు);
    krb5->context = GSS_C_NO_CONTEXT;
  }

  /* Free the SPN */
  ఒకవేళ(krb5->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_స్థితి, &krb5->spn);
    krb5->spn = GSS_C_NO_NAME;
  }
}

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు

#అయితే_ముగింపు /* HAVE_GSSAPI && USE_KERBEROS5 */
