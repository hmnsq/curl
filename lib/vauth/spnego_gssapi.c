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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ defined(HAVE_GSSAPI) && defined(USE_SPNEGO)

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "curl_gssapi.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wdeprecated-declarations"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate supported by the GSS-API library.
 */
bool Curl_auth_is_spnego_supported(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ TRUE;
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
CURLcode Curl_auth_decode_spnego_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ Curl_creds *creds,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chlg64,
                                         ರಚನೆ negotiatedata *nego)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t chlglen = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *chlg = ಖಾಲಿ;
  OM_uint32 major_ಸ್ಥಿತಿ;
  OM_uint32 minor_ಸ್ಥಿತಿ;
  OM_uint32 ಬಳಸದ_ಸ್ಥಿತಿ;
  gss_ಬಫರ್_desc ಒಳಹರಿವು_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
  gss_ಬಫರ್_desc output_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
  gss_channel_bindings_t chan_bindings = GSS_C_NO_CHANNEL_BINDINGS;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GSS_C_CHANNEL_BOUND_FLAG
  ರಚನೆ gss_channel_bindings_struct chan;
#ಆದರೆ_ಅಂತ್ಯ

  (ಶೂನ್ಯ)creds;

  ಆದರೆ(nego->context && nego->ಸ್ಥಿತಿ_2 == GSS_S_COMPLETE) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
  }

  ಆದರೆ(!nego->spn) {
    gss_ಬಫರ್_desc spn_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;

    /* Generate our SPN */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    ಅಕ್ಷರ *spn = Curl_auth_build_spn(service, ಖಾಲಿ, host);
    ಆದರೆ(!spn)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    /* Populate the SPN structure */
    spn_token.ಮೌಲ್ಯ = spn;
    spn_token.ಉದ್ದಳತೆ = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(spn);

    /* Import the SPN */
    major_ಸ್ಥಿತಿ = gss_import_name(&minor_ಸ್ಥಿತಿ, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE,
                                   &nego->spn);
    ಆದರೆ(GSS_ERROR(major_ಸ್ಥಿತಿ)) {
      Curl_gss_log_error(ದತ್ತಾಂಶ, "gss_import_name() failed: ",
                         major_ಸ್ಥಿತಿ, minor_ಸ್ಥಿತಿ);

      curlx_free(spn);

      ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  ಆದರೆ(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    ಆದರೆ(*chlg64 != '=') {
      ಫಲಿತಾಂಶ = curlx_base64_decode(chlg64, &chlg, &chlglen);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    /* Ensure we have a valid challenge message */
    ಆದರೆ(!chlg) {
      infof(ದತ್ತಾಂಶ, "SPNEGO handshake failure (empty challenge message)");
      ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    ಒಳಹರಿವು_token.ಮೌಲ್ಯ = chlg;
    ಒಳಹರಿವು_token.ಉದ್ದಳತೆ = chlglen;
  }

  /* Set channel binding data if available */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GSS_C_CHANNEL_BOUND_FLAG
  ಆದರೆ(curlx_dyn_len(&nego->channel_binding_ದತ್ತಾಂಶ)) {
    memset(&chan, 0, ಗಾತ್ರ(ರಚನೆ gss_channel_bindings_struct));
    chan.application_ದತ್ತಾಂಶ.ಉದ್ದಳತೆ = curlx_dyn_len(&nego->channel_binding_ದತ್ತಾಂಶ);
    chan.application_ದತ್ತಾಂಶ.ಮೌಲ್ಯ = curlx_dyn_ptr(&nego->channel_binding_ದತ್ತಾಂಶ);
    chan_bindings = &chan;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSS_ಗಣ_NEG_MECHS
  /* Acquire explicit credentials and restrict SPNEGO sub-mechanisms to
   * exclude NTLM. We enumerate all available mechanisms and filter out
   * the NTLMSSP OID, matching SSPI's "!ntlm". */
  ಆದರೆ(nego->cred == GSS_C_NO_CREDENTIAL) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    gss_OID_ಗಣ ಲಭ್ಯ_mechs = GSS_C_NO_OID_ಗಣ;
    gss_OID_ಗಣ filtered_mechs = GSS_C_NO_OID_ಗಣ;

    /* Acquire default credentials for SPNEGO */
    major_ಸ್ಥಿತಿ = Curl_gss_acquire_cred(&minor_ಸ್ಥಿತಿ, GSS_C_NO_NAME,
                                    GSS_C_INDEFINITE, GSS_C_NO_OID_ಗಣ,
                                    GSS_C_INITIATE, &nego->cred, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ(GSS_ERROR(major_ಸ್ಥಿತಿ)) {
      Curl_gss_log_error(ದತ್ತಾಂಶ, "gss_acquire_cred() failed: ",
                         major_ಸ್ಥಿತಿ, minor_ಸ್ಥಿತಿ);
      curlx_safefree(ಒಳಹರಿವು_token.ಮೌಲ್ಯ);
      ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
    }

    /* Get all available mechanisms */
    major_ಸ್ಥಿತಿ = Curl_gss_indicate_mechs(&minor_ಸ್ಥಿತಿ, &ಲಭ್ಯ_mechs);
    ಆದರೆ(!GSS_ERROR(major_ಸ್ಥಿತಿ)) {
      /* Build a set excluding NTLMSSP */
      major_ಸ್ಥಿತಿ = gss_create_ಖಾಲಿಗಳು_oid_ಗಣ(&minor_ಸ್ಥಿತಿ, &filtered_mechs);
      ಆದರೆ(!GSS_ERROR(major_ಸ್ಥಿತಿ)) {
        size_t i;
        ಗಾಗಿ(i = 0; i < ಲಭ್ಯ_mechs->ಎಣಿಕೆ_2; i++) {
          gss_OID oid = &ಲಭ್ಯ_mechs->elements[i];
          ಆದರೆ(oid->ಉದ್ದಳತೆ != ntlmssp_oid.ಉದ್ದಳತೆ ||
             memcmp(oid->elements, ntlmssp_oid.elements, oid->ಉದ್ದಳತೆ)) {
            gss_ಕೂಡಿಸು_oid_ಗಣ_member(&minor_ಸ್ಥಿತಿ, oid, &filtered_mechs);
          }
        }
        /* Restrict SPNEGO to only use non-NTLM mechanisms */
        major_ಸ್ಥಿತಿ = Curl_gss_ಗಣ_neg_mechs(&minor_ಸ್ಥಿತಿ, nego->cred,
                                              filtered_mechs);
        ಆದರೆ(GSS_ERROR(major_ಸ್ಥಿತಿ)) {
          Curl_gss_log_error(ದತ್ತಾಂಶ, "gss_set_neg_mechs() failed: ",
                             major_ಸ್ಥಿತಿ, minor_ಸ್ಥಿತಿ);
        }
        gss_release_oid_ಗಣ(&minor_ಸ್ಥಿತಿ, &filtered_mechs);
      }
      gss_release_oid_ಗಣ(&minor_ಸ್ಥಿತಿ, &ಲಭ್ಯ_mechs);
    }
  }
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GSS_SET_NEG_MECHS */

  /* Generate our challenge-response message */
  major_ಸ್ಥಿತಿ = Curl_gss_init_sec_context(ದತ್ತಾಂಶ,
                                           &minor_ಸ್ಥಿತಿ,
                                           &nego->context,
                                           nego->spn,
                                           &Curl_spnego_mech_oid,
                                           chan_bindings,
                                           &ಒಳಹರಿವು_token,
                                           &output_token,
                                           TRUE,
                                           ಖಾಲಿ,
                                           nego->cred);

  /* Free the decoded challenge as it is not required anymore */
  curlx_safefree(ಒಳಹರಿವು_token.ಮೌಲ್ಯ);

  nego->ಸ್ಥಿತಿ_2 = major_ಸ್ಥಿತಿ;
  ಆದರೆ(GSS_ERROR(major_ಸ್ಥಿತಿ)) {
    ಆದರೆ(output_token.ಮೌಲ್ಯ)
      gss_release_ಬಫರ್(&ಬಳಸದ_ಸ್ಥಿತಿ, &output_token);

    Curl_gss_log_error(ದತ್ತಾಂಶ, "gss_init_sec_context() failed: ",
                       major_ಸ್ಥಿತಿ, minor_ಸ್ಥಿತಿ);

    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  ಆದರೆ(!output_token.ಮೌಲ್ಯ || !output_token.ಉದ್ದಳತೆ) {
    ಆದರೆ(output_token.ಮೌಲ್ಯ)
      gss_release_ಬಫರ್(&ಬಳಸದ_ಸ್ಥಿತಿ, &output_token);

    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  /* Check if NTLM was selected and is disallowed */
  ಆದರೆ(nego->context != GSS_C_NO_CONTEXT) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    OM_uint32 inquire_major, inquire_minor;
    gss_OID mech_type = GSS_C_NO_OID;

    inquire_major = Curl_gss_inquire_context(&inquire_minor,
                                             nego->context,
                                             &mech_type);
    ಆದರೆ(!GSS_ERROR(inquire_major) && mech_type &&
       mech_type->ಉದ್ದಳತೆ == ntlmssp_oid.ಉದ್ದಳತೆ &&
       !memcmp(mech_type->elements, ntlmssp_oid.elements,
               ntlmssp_oid.ಉದ್ದಳತೆ)) {
      infof(ದತ್ತಾಂಶ, "SPNEGO chose NTLM, but NTLM is not allowed");
      gss_release_ಬಫರ್(&ಬಳಸದ_ಸ್ಥಿತಿ, &output_token);
      Curl_auth_cleanup_spnego(nego);
      ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
    }
  }

  /* Free previous token */
  ಆದರೆ(nego->output_token.ಉದ್ದಳತೆ && nego->output_token.ಮೌಲ್ಯ)
    gss_release_ಬಫರ್(&ಬಳಸದ_ಸ್ಥಿತಿ, &nego->output_token);

  nego->output_token = output_token;

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
CURLcode Curl_auth_create_spnego_ಸಂದೇಶ(ರಚನೆ negotiatedata *nego,
                                         ಅಕ್ಷರ **outptr, size_t *outlen)
{
  CURLcode ಫಲಿತಾಂಶ;
  OM_uint32 minor_ಸ್ಥಿತಿ;

  /* Base64 encode the already generated response */
  ಫಲಿತಾಂಶ = curlx_base64_encode(nego->output_token.ಮೌಲ್ಯ,
                               nego->output_token.ಉದ್ದಳತೆ,
                               outptr, outlen);

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    gss_release_ಬಫರ್(&minor_ಸ್ಥಿತಿ, &nego->output_token);
    nego->output_token.ಮೌಲ್ಯ = ಖಾಲಿ;
    nego->output_token.ಉದ್ದಳತೆ = 0;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(!*outptr || !*outlen) {
    gss_release_ಬಫರ್(&minor_ಸ್ಥಿತಿ, &nego->output_token);
    nego->output_token.ಮೌಲ್ಯ = ಖಾಲಿ;
    nego->output_token.ಉದ್ದಳತೆ = 0;

    ಹಿಂತಿರುಗಿ CURLE_REMOTE_ACCESS_DENIED;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
ಶೂನ್ಯ Curl_auth_cleanup_spnego(ರಚನೆ negotiatedata *nego)
{
  OM_uint32 minor_ಸ್ಥಿತಿ;

  /* Free our security context */
  ಆದರೆ(nego->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_ಸ್ಥಿತಿ, &nego->context,
                                GSS_C_NO_ಬಫರ್);
    nego->context = GSS_C_NO_CONTEXT;
  }

  /* Free the output token */
  ಆದರೆ(nego->output_token.ಮೌಲ್ಯ) {
    gss_release_ಬಫರ್(&minor_ಸ್ಥಿತಿ, &nego->output_token);
    nego->output_token.ಮೌಲ್ಯ = ಖಾಲಿ;
    nego->output_token.ಉದ್ದಳತೆ = 0;
  }

  /* Free the SPN */
  ಆದರೆ(nego->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_ಸ್ಥಿತಿ, &nego->spn);
    nego->spn = GSS_C_NO_NAME;
  }

  /* Free our credentials */
  ಆದರೆ(nego->cred != GSS_C_NO_CREDENTIAL) {
    Curl_gss_release_cred(&minor_ಸ್ಥಿತಿ, &nego->cred);
    nego->cred = GSS_C_NO_CREDENTIAL;
  }

  /* Reset any variables */
  nego->ಸ್ಥಿತಿ_2 = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GSSAPI && USE_SPNEGO */
