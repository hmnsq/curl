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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(HAVE_GSSAPI) && defined(USE_KERBEROS5)

#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sasl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_gssapi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wdeprecated-declarations"
#ਜੇ_ਅੰਤ

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by the GSS-API library.
 */
bool Curl_auth_is_gssapi_supported(ਖਾਲੀ)
{
  ਵਾਪਸ TRUE;
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
CURLcode Curl_auth_create_gssapi_user_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਬਣਤਰ Curl_creds *creds,
                                              ਅਚਲ ਅੱਖਰ *default_service,
                                              ਅਚਲ ਅੱਖਰ *host,
                                              ਅਚਲ bool mutual_auth,
                                              ਅਚਲ ਬਣਤਰ bufref *chlg,
                                              ਬਣਤਰ kerberos5data *krb5,
                                              ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  OM_uint32 major_ਸਥਿਤੀ;
  OM_uint32 minor_ਸਥਿਤੀ;
  OM_uint32 ਅਣਵਰਤਿਆ_ਸਥਿਤੀ;
  gss_ਬਫਰ_desc ਨਿਵੇਸ਼_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
  gss_ਬਫਰ_desc output_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
  ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  ਜੇ(!krb5->spn) {
    gss_ਬਫਰ_desc spn_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;

    /* Generate our SPN */
    ਅੱਖਰ *spn = Curl_auth_build_spn(service, ਨਿਰਾ, host);
    ਜੇ(!spn)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    /* Populate the SPN structure */
    spn_token.ਮੁੱਲ = spn;
    spn_token.ਲੰਮਾਈ = ਲੜੀ_ਲੰਬਾਈ(spn);

    /* Import the SPN */
    major_ਸਥਿਤੀ = gss_import_name(&minor_ਸਥਿਤੀ, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE, &krb5->spn);
    ਜੇ(GSS_ERROR(major_ਸਥਿਤੀ)) {
      Curl_gss_log_error(ਅੰਕੜੇ, "gss_import_name() failed: ",
                         major_ਸਥਿਤੀ, minor_ਸਥਿਤੀ);

      curlx_free(spn);

      ਵਾਪਸ CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  ਜੇ(chlg) {
    ਜੇ(!Curl_bufref_len(chlg)) {
      infof(ਅੰਕੜੇ, "GSSAPI handshake failure (empty challenge message)");
      ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
    }
    ਨਿਵੇਸ਼_token.ਮੁੱਲ = CURL_UNCONST(Curl_bufref_ptr(chlg));
    ਨਿਵੇਸ਼_token.ਲੰਮਾਈ = Curl_bufref_len(chlg);
  }

  major_ਸਥਿਤੀ = Curl_gss_init_sec_context(ਅੰਕੜੇ,
                                           &minor_ਸਥਿਤੀ,
                                           &krb5->context,
                                           krb5->spn,
                                           &Curl_krb5_mech_oid,
                                           GSS_C_NO_CHANNEL_BINDINGS,
                                           &ਨਿਵੇਸ਼_token,
                                           &output_token,
                                           mutual_auth,
                                           ਨਿਰਾ,
                                           GSS_C_NO_CREDENTIAL);

  ਜੇ(GSS_ERROR(major_ਸਥਿਤੀ)) {
    ਜੇ(output_token.ਮੁੱਲ)
      gss_release_ਬਫਰ(&ਅਣਵਰਤਿਆ_ਸਥਿਤੀ, &output_token);

    Curl_gss_log_error(ਅੰਕੜੇ, "gss_init_sec_context() failed: ",
                       major_ਸਥਿਤੀ, minor_ਸਥਿਤੀ);

    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  ਜੇ(output_token.ਮੁੱਲ && output_token.ਲੰਮਾਈ) {
    ਨਤੀਜਾ = Curl_bufref_memdup0(ਬਾਹਰਲਾ, output_token.ਮੁੱਲ, output_token.ਲੰਮਾਈ);
    gss_release_ਬਫਰ(&ਅਣਵਰਤਿਆ_ਸਥਿਤੀ, &output_token);
  }
  ਨਹੀਂਤਾਂ
    Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, mutual_auth ? "" : ਨਿਰਾ, 0, ਨਿਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
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
CURLcode Curl_auth_create_gssapi_security_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                  ਅਚਲ ਅੱਖਰ *authzid,
                                                  ਅਚਲ ਬਣਤਰ bufref *chlg,
                                                  ਬਣਤਰ kerberos5data *krb5,
                                                  ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t messagelen = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸੁਨੇਹਾ_2 = ਨਿਰਾ;
  OM_uint32 major_ਸਥਿਤੀ;
  OM_uint32 minor_ਸਥਿਤੀ;
  OM_uint32 ਅਣਵਰਤਿਆ_ਸਥਿਤੀ;
  gss_ਬਫਰ_desc ਨਿਵੇਸ਼_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
  gss_ਬਫਰ_desc output_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *indata;
  gss_qop_t qop = GSS_C_QOP_DEFAULT;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sec_layer = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ max_ਮਾਪ = 0;

  /* Ensure we have a valid challenge message */
  ਜੇ(!Curl_bufref_len(chlg)) {
    infof(ਅੰਕੜੇ, "GSSAPI handshake failure (empty security message)");
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Setup the challenge "input" security buffer */
  ਨਿਵੇਸ਼_token.ਮੁੱਲ = CURL_UNCONST(Curl_bufref_ptr(chlg));
  ਨਿਵੇਸ਼_token.ਲੰਮਾਈ = Curl_bufref_len(chlg);

  /* Decrypt the inbound challenge and obtain the qop */
  major_ਸਥਿਤੀ = gss_unwrap(&minor_ਸਥਿਤੀ, krb5->context, &ਨਿਵੇਸ਼_token,
                            &output_token, ਨਿਰਾ, &qop);
  ਜੇ(GSS_ERROR(major_ਸਥਿਤੀ)) {
    Curl_gss_log_error(ਅੰਕੜੇ, "gss_unwrap() failed: ",
                       major_ਸਥਿਤੀ, minor_ਸਥਿਤੀ);
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  ਜੇ(output_token.ਲੰਮਾਈ != 4) {
    infof(ਅੰਕੜੇ, "GSSAPI handshake failure (invalid security data)");
    gss_release_ਬਫਰ(&ਅਣਵਰਤਿਆ_ਸਥਿਤੀ, &output_token);
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = output_token.ਮੁੱਲ;
  sec_layer = indata[0];
  max_ਮਾਪ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)indata[1] << 16) |
             ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)indata[2] << 8) | indata[3];

  /* Free the challenge as it is not required anymore */
  gss_release_ਬਫਰ(&ਅਣਵਰਤਿਆ_ਸਥਿਤੀ, &output_token);

  /* Process the security layer */
  ਜੇ(!(sec_layer & GSSAUTH_P_NONE)) {
    infof(ਅੰਕੜੇ, "GSSAPI handshake failure (invalid security layer)");

    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= GSSAUTH_P_NONE;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  ਜੇ(max_ਮਾਪ > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_ਮਾਪ = 0;
  }

  /* Allocate our message */
  messagelen = 4;
  ਜੇ(authzid)
    messagelen += ਲੜੀ_ਲੰਬਾਈ(authzid);
  ਸੁਨੇਹਾ_2 = curlx_malloc(messagelen);
  ਜੇ(!ਸੁਨੇਹਾ_2)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Populate the message with the security layer and client supported receive
     message size. */
  ਸੁਨੇਹਾ_2[0] = sec_layer & 0xFF;
  ਸੁਨੇਹਾ_2[1] = (max_ਮਾਪ >> 16) & 0xFF;
  ਸੁਨੇਹਾ_2[2] = (max_ਮਾਪ >> 8) & 0xFF;
  ਸੁਨੇਹਾ_2[3] = max_ਮਾਪ & 0xFF;

  /* If given, append the authorization identity. */

  ਜੇ(authzid && *authzid)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਸੁਨੇਹਾ_2 + 4, authzid, messagelen - 4);

  /* Setup the "authentication data" security buffer */
  ਨਿਵੇਸ਼_token.ਮੁੱਲ = ਸੁਨੇਹਾ_2;
  ਨਿਵੇਸ਼_token.ਲੰਮਾਈ = messagelen;

  /* Encrypt the data */
  major_ਸਥਿਤੀ = gss_wrap(&minor_ਸਥਿਤੀ, krb5->context, 0,
                          GSS_C_QOP_DEFAULT, &ਨਿਵੇਸ਼_token, ਨਿਰਾ,
                          &output_token);
  ਜੇ(GSS_ERROR(major_ਸਥਿਤੀ)) {
    Curl_gss_log_error(ਅੰਕੜੇ, "gss_wrap() failed: ",
                       major_ਸਥਿਤੀ, minor_ਸਥਿਤੀ);
    curlx_free(ਸੁਨੇਹਾ_2);
    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  ਨਤੀਜਾ = Curl_bufref_memdup0(ਬਾਹਰਲਾ, output_token.ਮੁੱਲ, output_token.ਲੰਮਾਈ);
  /* Free the output buffer */
  gss_release_ਬਫਰ(&ਅਣਵਰਤਿਆ_ਸਥਿਤੀ, &output_token);

  /* Free the message buffer */
  curlx_free(ਸੁਨੇਹਾ_2);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਖਾਲੀ Curl_auth_cleanup_gssapi(ਬਣਤਰ kerberos5data *krb5)
{
  OM_uint32 minor_ਸਥਿਤੀ;

  /* Free our security context */
  ਜੇ(krb5->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_ਸਥਿਤੀ, &krb5->context,
                                GSS_C_NO_ਬਫਰ);
    krb5->context = GSS_C_NO_CONTEXT;
  }

  /* Free the SPN */
  ਜੇ(krb5->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_ਸਥਿਤੀ, &krb5->spn);
    krb5->spn = GSS_C_NO_NAME;
  }
}

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HAVE_GSSAPI && USE_KERBEROS5 */
