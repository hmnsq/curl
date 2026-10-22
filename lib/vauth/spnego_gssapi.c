/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
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
 * RFC4178 Simple and Protected GSS-API Negotiation Mechanism
 *
 ***************************************************************************/
#ضمن "curl_setup.h"

#إذا defined(HAVE_GSSAPI) && defined(USE_SPNEGO)

#ضمن "vauth/vauth.h"
#ضمن "curlx/base64.h"
#ضمن "curl_gssapi.h"
#ضمن "curl_trc.h"

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wdeprecated-declarations"
#جے_انت

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate supported by the GSS-API library.
 */
bool Curl_auth_is_spnego_supported(فراغ)
{
  أرجع TRUE;
}

/*
 * Curl_auth_decode_spnego_message()
 *
 * This is used to decode an already encoded SPNEGO (Negotiate) challenge
 * message.
 *
 * Parameters:
 *
 * data        [in]     - The session handle.
 * userp       [in]     - The username in the format User or Domain\User.
 * passwdp     [in]     - The user's password.
 * service     [in]     - The service type such as http, smtp, pop or imap.
 * host        [in]     - The hostname.
 * chlg64      [in]     - The optional base64 encoded challenge message.
 * nego        [in/out] - The Negotiate data struct being used and modified.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_decode_spnego_سنیہا(بنية Curl_easy *انکڑے,
                                         بنية Curl_creds *creds,
                                         ثابت حرف *default_service,
                                         ثابت حرف *host,
                                         ثابت حرف *chlg64,
                                         بنية negotiatedata *nego)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t chlglen = 0;
  بنانشانی حرف *chlg = عدم;
  OM_uint32 major_حالت;
  OM_uint32 minor_حالت;
  OM_uint32 بن_ورتیا_حالت;
  gss_بفر_desc داخلہ_token = GSS_C_خالی_تھاواں_بفر;
  gss_بفر_desc output_token = GSS_C_خالی_تھاواں_بفر;
  gss_channel_bindings_t chan_bindings = GSS_C_NO_CHANNEL_BINDINGS;
#جے_تعریف_شدہ GSS_C_CHANNEL_BOUND_FLAG
  بنية gss_channel_bindings_struct chan;
#جے_انت

  (فراغ)creds;

  إذا(nego->context && nego->حالت == GSS_S_COMPLETE) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    أرجع CURLE_LOGIN_DENIED;
  }

  إذا(!nego->spn) {
    gss_بفر_desc spn_token = GSS_C_خالی_تھاواں_بفر;

    /* Generate our SPN */
    ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    حرف *spn = Curl_auth_build_spn(service, عدم, host);
    إذا(!spn)
      أرجع CURLE_باہرلا_دا_حافظہ;

    /* Populate the SPN structure */
    spn_token.قدر = spn;
    spn_token.لمائی = لڑی_لمبائی(spn);

    /* Import the SPN */
    major_حالت = gss_import_name(&minor_حالت, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE,
                                   &nego->spn);
    إذا(GSS_ERROR(major_حالت)) {
      Curl_gss_log_error(انکڑے, "gss_import_name() failed: ",
                         major_حالت, minor_حالت);

      curlx_free(spn);

      أرجع CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  إذا(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    إذا(*chlg64 != '=') {
      نتیجہ = curlx_base64_decode(chlg64, &chlg, &chlglen);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }

    /* Ensure we have a valid challenge message */
    إذا(!chlg) {
      infof(انکڑے, "SPNEGO handshake failure (empty challenge message)");
      أرجع CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    داخلہ_token.قدر = chlg;
    داخلہ_token.لمائی = chlglen;
  }

  /* Set channel binding data if available */
#جے_تعریف_شدہ GSS_C_CHANNEL_BOUND_FLAG
  إذا(curlx_dyn_len(&nego->channel_binding_انکڑے)) {
    memset(&chan, 0, حجم(بنية gss_channel_bindings_struct));
    chan.application_انکڑے.لمائی = curlx_dyn_len(&nego->channel_binding_انکڑے);
    chan.application_انکڑے.قدر = curlx_dyn_ptr(&nego->channel_binding_انکڑے);
    chan_bindings = &chan;
  }
#جے_انت

#جے_تعریف_شدہ HAVE_GSS_سیٹ_NEG_MECHS
  /* Acquire explicit credentials and restrict SPNEGO sub-mechanisms to
   * exclude NTLM. We enumerate all available mechanisms and filter out
   * the NTLMSSP OID, matching SSPI's "!ntlm". */
  إذا(nego->cred == GSS_C_NO_CREDENTIAL) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    ساكن ثابت gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    gss_OID_سیٹ دستیاب_mechs = GSS_C_NO_OID_سیٹ;
    gss_OID_سیٹ filtered_mechs = GSS_C_NO_OID_سیٹ;

    /* Acquire default credentials for SPNEGO */
    major_حالت = Curl_gss_acquire_cred(&minor_حالت, GSS_C_NO_NAME,
                                    GSS_C_INDEFINITE, GSS_C_NO_OID_سیٹ,
                                    GSS_C_INITIATE, &nego->cred, عدم, عدم);
    إذا(GSS_ERROR(major_حالت)) {
      Curl_gss_log_error(انکڑے, "gss_acquire_cred() failed: ",
                         major_حالت, minor_حالت);
      curlx_safefree(داخلہ_token.قدر);
      أرجع CURLE_AUTH_ERROR;
    }

    /* Get all available mechanisms */
    major_حالت = Curl_gss_indicate_mechs(&minor_حالت, &دستیاب_mechs);
    إذا(!GSS_ERROR(major_حالت)) {
      /* Build a set excluding NTLMSSP */
      major_حالت = gss_create_خالی_تھاواں_oid_سیٹ(&minor_حالت, &filtered_mechs);
      إذا(!GSS_ERROR(major_حالت)) {
        size_t i;
        لكل(i = 0; i < دستیاب_mechs->گنتی; i++) {
          gss_OID oid = &دستیاب_mechs->elements[i];
          إذا(oid->لمائی != ntlmssp_oid.لمائی ||
             memcmp(oid->elements, ntlmssp_oid.elements, oid->لمائی)) {
            gss_جوڑو_oid_سیٹ_member(&minor_حالت, oid, &filtered_mechs);
          }
        }
        /* Restrict SPNEGO to only use non-NTLM mechanisms */
        major_حالت = Curl_gss_سیٹ_neg_mechs(&minor_حالت, nego->cred,
                                              filtered_mechs);
        إذا(GSS_ERROR(major_حالت)) {
          Curl_gss_log_error(انکڑے, "gss_set_neg_mechs() failed: ",
                             major_حالت, minor_حالت);
        }
        gss_release_oid_سیٹ(&minor_حالت, &filtered_mechs);
      }
      gss_release_oid_سیٹ(&minor_حالت, &دستیاب_mechs);
    }
  }
#جے_انت /* HAVE_GSS_SET_NEG_MECHS */

  /* Generate our challenge-response message */
  major_حالت = Curl_gss_init_sec_context(انکڑے,
                                           &minor_حالت,
                                           &nego->context,
                                           nego->spn,
                                           &Curl_spnego_mech_oid,
                                           chan_bindings,
                                           &داخلہ_token,
                                           &output_token,
                                           TRUE,
                                           عدم,
                                           nego->cred);

  /* Free the decoded challenge as it is not required anymore */
  curlx_safefree(داخلہ_token.قدر);

  nego->حالت = major_حالت;
  إذا(GSS_ERROR(major_حالت)) {
    إذا(output_token.قدر)
      gss_release_بفر(&بن_ورتیا_حالت, &output_token);

    Curl_gss_log_error(انکڑے, "gss_init_sec_context() failed: ",
                       major_حالت, minor_حالت);

    أرجع CURLE_AUTH_ERROR;
  }

  إذا(!output_token.قدر || !output_token.لمائی) {
    إذا(output_token.قدر)
      gss_release_بفر(&بن_ورتیا_حالت, &output_token);

    أرجع CURLE_AUTH_ERROR;
  }

  /* Check if NTLM was selected and is disallowed */
  إذا(nego->context != GSS_C_NO_CONTEXT) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    ساكن ثابت gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    OM_uint32 inquire_major, inquire_minor;
    gss_OID mech_type = GSS_C_NO_OID;

    inquire_major = Curl_gss_inquire_context(&inquire_minor,
                                             nego->context,
                                             &mech_type);
    إذا(!GSS_ERROR(inquire_major) && mech_type &&
       mech_type->لمائی == ntlmssp_oid.لمائی &&
       !memcmp(mech_type->elements, ntlmssp_oid.elements,
               ntlmssp_oid.لمائی)) {
      infof(انکڑے, "SPNEGO chose NTLM, but NTLM is not allowed");
      gss_release_بفر(&بن_ورتیا_حالت, &output_token);
      Curl_auth_cleanup_spnego(nego);
      أرجع CURLE_AUTH_ERROR;
    }
  }

  /* Free previous token */
  إذا(nego->output_token.لمائی && nego->output_token.قدر)
    gss_release_بفر(&بن_ورتیا_حالت, &nego->output_token);

  nego->output_token = output_token;

  أرجع CURLE_OK;
}

/*
 * Curl_auth_create_spnego_message()
 *
 * This is used to generate an already encoded SPNEGO (Negotiate) response
 * message ready for sending to the recipient.
 *
 * Parameters:
 *
 * data        [in]     - The session handle.
 * nego        [in/out] - The Negotiate data struct being used and modified.
 * outptr      [in/out] - The address where a pointer to newly allocated memory
 *                        holding the result is stored upon completion.
 * outlen      [out]    - The length of the output message.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_spnego_سنیہا(بنية negotiatedata *nego,
                                         حرف **outptr, size_t *outlen)
{
  CURLcode نتیجہ;
  OM_uint32 minor_حالت;

  /* Base64 encode the already generated response */
  نتیجہ = curlx_base64_encode(nego->output_token.قدر,
                               nego->output_token.لمائی,
                               outptr, outlen);

  إذا(نتیجہ) {
    gss_release_بفر(&minor_حالت, &nego->output_token);
    nego->output_token.قدر = عدم;
    nego->output_token.لمائی = 0;

    أرجع نتیجہ;
  }

  إذا(!*outptr || !*outlen) {
    gss_release_بفر(&minor_حالت, &nego->output_token);
    nego->output_token.قدر = عدم;
    nego->output_token.لمائی = 0;

    أرجع CURLE_REMOTE_ACCESS_DENIED;
  }

  أرجع CURLE_OK;
}

/*
 * Curl_auth_cleanup_spnego()
 *
 * This is used to clean up the SPNEGO (Negotiate) specific data.
 *
 * Parameters:
 *
 * nego     [in/out] - The Negotiate data struct being cleaned up.
 *
 */
فراغ Curl_auth_cleanup_spnego(بنية negotiatedata *nego)
{
  OM_uint32 minor_حالت;

  /* Free our security context */
  إذا(nego->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_حالت, &nego->context,
                                GSS_C_NO_بفر);
    nego->context = GSS_C_NO_CONTEXT;
  }

  /* Free the output token */
  إذا(nego->output_token.قدر) {
    gss_release_بفر(&minor_حالت, &nego->output_token);
    nego->output_token.قدر = عدم;
    nego->output_token.لمائی = 0;
  }

  /* Free the SPN */
  إذا(nego->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_حالت, &nego->spn);
    nego->spn = GSS_C_NO_NAME;
  }

  /* Free our credentials */
  إذا(nego->cred != GSS_C_NO_CREDENTIAL) {
    Curl_gss_release_cred(&minor_حالت, &nego->cred);
    nego->cred = GSS_C_NO_CREDENTIAL;
  }

  /* Reset any variables */
  nego->حالت = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ہدایت GCC diagnostic نکالو
#جے_انت

#جے_انت /* HAVE_GSSAPI && USE_SPNEGO */
