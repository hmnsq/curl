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
#অন্তর্ভাব "curl_setup.h"

#যদি defined(HAVE_GSSAPI) && defined(USE_KERBEROS5)

#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "curl_sasl.h"
#অন্তর্ভাব "curl_gssapi.h"
#অন্তর্ভাব "curl_trc.h"

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic ঠেলো
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
bool Curl_auth_is_gssapi_supported(শূন্য)
{
  ফেরত TRUE;
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
CURLcode Curl_auth_create_gssapi_user_message(কাঠামো Curl_easy *তথ্য,
                                              কাঠামো Curl_creds *creds,
                                              ধ্রুবক অক্ষর *default_service,
                                              ধ্রুবক অক্ষর *host,
                                              ধ্রুবক bool mutual_auth,
                                              ধ্রুবক কাঠামো bufref *chlg,
                                              কাঠামো kerberos5data *krb5,
                                              কাঠামো bufref *out)
{
  CURLcode ফলাফল = CURLE_OK;
  OM_uint32 major_status;
  OM_uint32 minor_status;
  OM_uint32 unused_status;
  gss_বাফার_desc প্রবেশ_token = GSS_C_EMPTY_বাফার;
  gss_বাফার_desc output_token = GSS_C_EMPTY_বাফার;
  ধ্রুবক অক্ষর *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  যদি(!krb5->spn) {
    gss_বাফার_desc spn_token = GSS_C_EMPTY_বাফার;

    /* Generate our SPN */
    অক্ষর *spn = Curl_auth_build_spn(service, ফাঁকা, host);
    যদি(!spn)
      ফেরত CURLE_OUT_OF_MEMORY;

    /* Populate the SPN structure */
    spn_token.মান = spn;
    spn_token.দৈর্ঘ্য = strlen(spn);

    /* Import the SPN */
    major_status = gss_import_name(&minor_status, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE, &krb5->spn);
    যদি(GSS_ERROR(major_status)) {
      Curl_gss_log_error(তথ্য, "gss_import_name() failed: ",
                         major_status, minor_status);

      curlx_free(spn);

      ফেরত CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  যদি(chlg) {
    যদি(!Curl_bufref_len(chlg)) {
      infof(তথ্য, "GSSAPI handshake failure (empty challenge message)");
      ফেরত CURLE_BAD_CONTENT_ENCODING;
    }
    প্রবেশ_token.মান = CURL_UNCONST(Curl_bufref_ptr(chlg));
    প্রবেশ_token.দৈর্ঘ্য = Curl_bufref_len(chlg);
  }

  major_status = Curl_gss_init_sec_context(তথ্য,
                                           &minor_status,
                                           &krb5->context,
                                           krb5->spn,
                                           &Curl_krb5_mech_oid,
                                           GSS_C_NO_CHANNEL_BINDINGS,
                                           &প্রবেশ_token,
                                           &output_token,
                                           mutual_auth,
                                           ফাঁকা,
                                           GSS_C_NO_CREDENTIAL);

  যদি(GSS_ERROR(major_status)) {
    যদি(output_token.মান)
      gss_release_বাফার(&unused_status, &output_token);

    Curl_gss_log_error(তথ্য, "gss_init_sec_context() failed: ",
                       major_status, minor_status);

    ফেরত CURLE_AUTH_ERROR;
  }

  যদি(output_token.মান && output_token.দৈর্ঘ্য) {
    ফলাফল = Curl_bufref_memdup0(out, output_token.মান, output_token.দৈর্ঘ্য);
    gss_release_বাফার(&unused_status, &output_token);
  }
  অন্যথা
    Curl_bufref_set(out, mutual_auth ? "" : ফাঁকা, 0, ফাঁকা);

  ফেরত ফলাফল;
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
CURLcode Curl_auth_create_gssapi_security_message(কাঠামো Curl_easy *তথ্য,
                                                  ধ্রুবক অক্ষর *authzid,
                                                  ধ্রুবক কাঠামো bufref *chlg,
                                                  কাঠামো kerberos5data *krb5,
                                                  কাঠামো bufref *out)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t messagelen = 0;
  unsigned অক্ষর *message = ফাঁকা;
  OM_uint32 major_status;
  OM_uint32 minor_status;
  OM_uint32 unused_status;
  gss_বাফার_desc প্রবেশ_token = GSS_C_EMPTY_বাফার;
  gss_বাফার_desc output_token = GSS_C_EMPTY_বাফার;
  unsigned অক্ষর *indata;
  gss_qop_t qop = GSS_C_QOP_DEFAULT;
  unsigned পূর্ণসংখ্যা sec_layer = 0;
  unsigned পূর্ণসংখ্যা max_আকার = 0;

  /* Ensure we have a valid challenge message */
  যদি(!Curl_bufref_len(chlg)) {
    infof(তথ্য, "GSSAPI handshake failure (empty security message)");
    ফেরত CURLE_BAD_CONTENT_ENCODING;
  }

  /* Setup the challenge "input" security buffer */
  প্রবেশ_token.মান = CURL_UNCONST(Curl_bufref_ptr(chlg));
  প্রবেশ_token.দৈর্ঘ্য = Curl_bufref_len(chlg);

  /* Decrypt the inbound challenge and obtain the qop */
  major_status = gss_unwrap(&minor_status, krb5->context, &প্রবেশ_token,
                            &output_token, ফাঁকা, &qop);
  যদি(GSS_ERROR(major_status)) {
    Curl_gss_log_error(তথ্য, "gss_unwrap() failed: ",
                       major_status, minor_status);
    ফেরত CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  যদি(output_token.দৈর্ঘ্য != 4) {
    infof(তথ্য, "GSSAPI handshake failure (invalid security data)");
    gss_release_বাফার(&unused_status, &output_token);
    ফেরত CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = output_token.মান;
  sec_layer = indata[0];
  max_আকার = ((unsigned পূর্ণসংখ্যা)indata[1] << 16) |
             ((unsigned পূর্ণসংখ্যা)indata[2] << 8) | indata[3];

  /* Free the challenge as it is not required anymore */
  gss_release_বাফার(&unused_status, &output_token);

  /* Process the security layer */
  যদি(!(sec_layer & GSSAUTH_P_NONE)) {
    infof(তথ্য, "GSSAPI handshake failure (invalid security layer)");

    ফেরত CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= GSSAUTH_P_NONE;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  যদি(max_আকার > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_আকার = 0;
  }

  /* Allocate our message */
  messagelen = 4;
  যদি(authzid)
    messagelen += strlen(authzid);
  message = curlx_malloc(messagelen);
  যদি(!message)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* Populate the message with the security layer and client supported receive
     message size. */
  message[0] = sec_layer & 0xFF;
  message[1] = (max_আকার >> 16) & 0xFF;
  message[2] = (max_আকার >> 8) & 0xFF;
  message[3] = max_আকার & 0xFF;

  /* If given, append the authorization identity. */

  যদি(authzid && *authzid)
    memcpy(message + 4, authzid, messagelen - 4);

  /* Setup the "authentication data" security buffer */
  প্রবেশ_token.মান = message;
  প্রবেশ_token.দৈর্ঘ্য = messagelen;

  /* Encrypt the data */
  major_status = gss_wrap(&minor_status, krb5->context, 0,
                          GSS_C_QOP_DEFAULT, &প্রবেশ_token, ফাঁকা,
                          &output_token);
  যদি(GSS_ERROR(major_status)) {
    Curl_gss_log_error(তথ্য, "gss_wrap() failed: ",
                       major_status, minor_status);
    curlx_free(message);
    ফেরত CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  ফলাফল = Curl_bufref_memdup0(out, output_token.মান, output_token.দৈর্ঘ্য);
  /* Free the output buffer */
  gss_release_বাফার(&unused_status, &output_token);

  /* Free the message buffer */
  curlx_free(message);

  ফেরত ফলাফল;
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
শূন্য Curl_auth_cleanup_gssapi(কাঠামো kerberos5data *krb5)
{
  OM_uint32 minor_status;

  /* Free our security context */
  যদি(krb5->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_status, &krb5->context,
                                GSS_C_NO_বাফার);
    krb5->context = GSS_C_NO_CONTEXT;
  }

  /* Free the SPN */
  যদি(krb5->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_status, &krb5->spn);
    krb5->spn = GSS_C_NO_NAME;
  }
}

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic বের_করো
#endif

#endif /* HAVE_GSSAPI && USE_KERBEROS5 */
