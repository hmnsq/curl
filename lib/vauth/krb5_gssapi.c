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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ defined(HAVE_GSSAPI) && defined(USE_KERBEROS5)

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "curl_sasl.h"
#ಅನ್ತರ್ಭಾವ "curl_gssapi.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wdeprecated-declarations"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by the GSS-API library.
 */
bool Curl_auth_is_gssapi_supported(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ TRUE;
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
CURLcode Curl_auth_create_gssapi_user_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ರಚನೆ Curl_creds *creds,
                                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                              ಸ್ಥಿರಾಂಕ bool mutual_auth,
                                              ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                                              ರಚನೆ kerberos5data *krb5,
                                              ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  OM_uint32 major_ಸ್ಥಿತಿ;
  OM_uint32 minor_ಸ್ಥಿತಿ;
  OM_uint32 ಬಳಸದ_ಸ್ಥಿತಿ;
  gss_ಬಫರ್_desc ಒಳಹರಿವು_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
  gss_ಬಫರ್_desc output_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  ಆದರೆ(!krb5->spn) {
    gss_ಬಫರ್_desc spn_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;

    /* Generate our SPN */
    ಅಕ್ಷರ *spn = Curl_auth_build_spn(service, ಖಾಲಿ, host);
    ಆದರೆ(!spn)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    /* Populate the SPN structure */
    spn_token.ಮೌಲ್ಯ = spn;
    spn_token.ಉದ್ದಳತೆ = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(spn);

    /* Import the SPN */
    major_ಸ್ಥಿತಿ = gss_import_name(&minor_ಸ್ಥಿತಿ, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE, &krb5->spn);
    ಆದರೆ(GSS_ERROR(major_ಸ್ಥಿತಿ)) {
      Curl_gss_log_error(ದತ್ತಾಂಶ, "gss_import_name() failed: ",
                         major_ಸ್ಥಿತಿ, minor_ಸ್ಥಿತಿ);

      curlx_free(spn);

      ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  ಆದರೆ(chlg) {
    ಆದರೆ(!Curl_bufref_len(chlg)) {
      infof(ದತ್ತಾಂಶ, "GSSAPI handshake failure (empty challenge message)");
      ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
    }
    ಒಳಹರಿವು_token.ಮೌಲ್ಯ = CURL_UNCONST(Curl_bufref_ptr(chlg));
    ಒಳಹರಿವು_token.ಉದ್ದಳತೆ = Curl_bufref_len(chlg);
  }

  major_ಸ್ಥಿತಿ = Curl_gss_init_sec_context(ದತ್ತಾಂಶ,
                                           &minor_ಸ್ಥಿತಿ,
                                           &krb5->context,
                                           krb5->spn,
                                           &Curl_krb5_mech_oid,
                                           GSS_C_NO_CHANNEL_BINDINGS,
                                           &ಒಳಹರಿವು_token,
                                           &output_token,
                                           mutual_auth,
                                           ಖಾಲಿ,
                                           GSS_C_NO_CREDENTIAL);

  ಆದರೆ(GSS_ERROR(major_ಸ್ಥಿತಿ)) {
    ಆದರೆ(output_token.ಮೌಲ್ಯ)
      gss_release_ಬಫರ್(&ಬಳಸದ_ಸ್ಥಿತಿ, &output_token);

    Curl_gss_log_error(ದತ್ತಾಂಶ, "gss_init_sec_context() failed: ",
                       major_ಸ್ಥಿತಿ, minor_ಸ್ಥಿತಿ);

    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  ಆದರೆ(output_token.ಮೌಲ್ಯ && output_token.ಉದ್ದಳತೆ) {
    ಫಲಿತಾಂಶ = Curl_bufref_memdup0(ಹೊರಹೋಗು, output_token.ಮೌಲ್ಯ, output_token.ಉದ್ದಳತೆ);
    gss_release_ಬಫರ್(&ಬಳಸದ_ಸ್ಥಿತಿ, &output_token);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    Curl_bufref_ಗಣ(ಹೊರಹೋಗು, mutual_auth ? "" : ಖಾಲಿ, 0, ಖಾಲಿ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
CURLcode Curl_auth_create_gssapi_security_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *authzid,
                                                  ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                                                  ರಚನೆ kerberos5data *krb5,
                                                  ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t messagelen = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಸಂದೇಶ_2 = ಖಾಲಿ;
  OM_uint32 major_ಸ್ಥಿತಿ;
  OM_uint32 minor_ಸ್ಥಿತಿ;
  OM_uint32 ಬಳಸದ_ಸ್ಥಿತಿ;
  gss_ಬಫರ್_desc ಒಳಹರಿವು_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
  gss_ಬಫರ್_desc output_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *indata;
  gss_qop_t qop = GSS_C_QOP_DEFAULT;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sec_layer = 0;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ max_ಗಾತ್ರ = 0;

  /* Ensure we have a valid challenge message */
  ಆದರೆ(!Curl_bufref_len(chlg)) {
    infof(ದತ್ತಾಂಶ, "GSSAPI handshake failure (empty security message)");
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Setup the challenge "input" security buffer */
  ಒಳಹರಿವು_token.ಮೌಲ್ಯ = CURL_UNCONST(Curl_bufref_ptr(chlg));
  ಒಳಹರಿವು_token.ಉದ್ದಳತೆ = Curl_bufref_len(chlg);

  /* Decrypt the inbound challenge and obtain the qop */
  major_ಸ್ಥಿತಿ = gss_unwrap(&minor_ಸ್ಥಿತಿ, krb5->context, &ಒಳಹರಿವು_token,
                            &output_token, ಖಾಲಿ, &qop);
  ಆದರೆ(GSS_ERROR(major_ಸ್ಥಿತಿ)) {
    Curl_gss_log_error(ದತ್ತಾಂಶ, "gss_unwrap() failed: ",
                       major_ಸ್ಥಿತಿ, minor_ಸ್ಥಿತಿ);
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  ಆದರೆ(output_token.ಉದ್ದಳತೆ != 4) {
    infof(ದತ್ತಾಂಶ, "GSSAPI handshake failure (invalid security data)");
    gss_release_ಬಫರ್(&ಬಳಸದ_ಸ್ಥಿತಿ, &output_token);
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = output_token.ಮೌಲ್ಯ;
  sec_layer = indata[0];
  max_ಗಾತ್ರ = ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)indata[1] << 16) |
             ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)indata[2] << 8) | indata[3];

  /* Free the challenge as it is not required anymore */
  gss_release_ಬಫರ್(&ಬಳಸದ_ಸ್ಥಿತಿ, &output_token);

  /* Process the security layer */
  ಆದರೆ(!(sec_layer & GSSAUTH_P_NONE)) {
    infof(ದತ್ತಾಂಶ, "GSSAPI handshake failure (invalid security layer)");

    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= GSSAUTH_P_NONE;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  ಆದರೆ(max_ಗಾತ್ರ > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_ಗಾತ್ರ = 0;
  }

  /* Allocate our message */
  messagelen = 4;
  ಆದರೆ(authzid)
    messagelen += ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(authzid);
  ಸಂದೇಶ_2 = curlx_malloc(messagelen);
  ಆದರೆ(!ಸಂದೇಶ_2)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Populate the message with the security layer and client supported receive
     message size. */
  ಸಂದೇಶ_2[0] = sec_layer & 0xFF;
  ಸಂದೇಶ_2[1] = (max_ಗಾತ್ರ >> 16) & 0xFF;
  ಸಂದೇಶ_2[2] = (max_ಗಾತ್ರ >> 8) & 0xFF;
  ಸಂದೇಶ_2[3] = max_ಗಾತ್ರ & 0xFF;

  /* If given, append the authorization identity. */

  ಆದರೆ(authzid && *authzid)
    ಸ್ಮರಣೆ_ನಕಲು(ಸಂದೇಶ_2 + 4, authzid, messagelen - 4);

  /* Setup the "authentication data" security buffer */
  ಒಳಹರಿವು_token.ಮೌಲ್ಯ = ಸಂದೇಶ_2;
  ಒಳಹರಿವು_token.ಉದ್ದಳತೆ = messagelen;

  /* Encrypt the data */
  major_ಸ್ಥಿತಿ = gss_wrap(&minor_ಸ್ಥಿತಿ, krb5->context, 0,
                          GSS_C_QOP_DEFAULT, &ಒಳಹರಿವು_token, ಖಾಲಿ,
                          &output_token);
  ಆದರೆ(GSS_ERROR(major_ಸ್ಥಿತಿ)) {
    Curl_gss_log_error(ದತ್ತಾಂಶ, "gss_wrap() failed: ",
                       major_ಸ್ಥಿತಿ, minor_ಸ್ಥಿತಿ);
    curlx_free(ಸಂದೇಶ_2);
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  ಫಲಿತಾಂಶ = Curl_bufref_memdup0(ಹೊರಹೋಗು, output_token.ಮೌಲ್ಯ, output_token.ಉದ್ದಳತೆ);
  /* Free the output buffer */
  gss_release_ಬಫರ್(&ಬಳಸದ_ಸ್ಥಿತಿ, &output_token);

  /* Free the message buffer */
  curlx_free(ಸಂದೇಶ_2);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಶೂನ್ಯ Curl_auth_cleanup_gssapi(ರಚನೆ kerberos5data *krb5)
{
  OM_uint32 minor_ಸ್ಥಿತಿ;

  /* Free our security context */
  ಆದರೆ(krb5->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_ಸ್ಥಿತಿ, &krb5->context,
                                GSS_C_NO_ಬಫರ್);
    krb5->context = GSS_C_NO_CONTEXT;
  }

  /* Free the SPN */
  ಆದರೆ(krb5->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_ಸ್ಥಿತಿ, &krb5->spn);
    krb5->spn = GSS_C_NO_NAME;
  }
}

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GSSAPI && USE_KERBEROS5 */
