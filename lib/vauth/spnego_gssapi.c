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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(HAVE_GSSAPI) && defined(USE_SPNEGO)

#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_gssapi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wdeprecated-declarations"
#ਜੇ_ਅੰਤ

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate supported by the GSS-API library.
 */
bool Curl_auth_is_spnego_supported(ਖਾਲੀ)
{
  ਵਾਪਸ TRUE;
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
CURLcode Curl_auth_decode_spnego_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ Curl_creds *creds,
                                         ਅਚਲ ਅੱਖਰ *default_service,
                                         ਅਚਲ ਅੱਖਰ *host,
                                         ਅਚਲ ਅੱਖਰ *chlg64,
                                         ਬਣਤਰ negotiatedata *nego)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t chlglen = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *chlg = ਨਿਰਾ;
  OM_uint32 major_ਸਥਿਤੀ;
  OM_uint32 minor_ਸਥਿਤੀ;
  OM_uint32 ਅਣਵਰਤਿਆ_ਸਥਿਤੀ;
  gss_ਬਫਰ_desc ਨਿਵੇਸ਼_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
  gss_ਬਫਰ_desc output_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
  gss_channel_bindings_t chan_bindings = GSS_C_NO_CHANNEL_BINDINGS;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GSS_C_CHANNEL_BOUND_FLAG
  ਬਣਤਰ gss_channel_bindings_struct chan;
#ਜੇ_ਅੰਤ

  (ਖਾਲੀ)creds;

  ਜੇ(nego->context && nego->ਸਥਿਤੀ == GSS_S_COMPLETE) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    ਵਾਪਸ CURLE_LOGIN_DENIED;
  }

  ਜੇ(!nego->spn) {
    gss_ਬਫਰ_desc spn_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;

    /* Generate our SPN */
    ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    ਅੱਖਰ *spn = Curl_auth_build_spn(service, ਨਿਰਾ, host);
    ਜੇ(!spn)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    /* Populate the SPN structure */
    spn_token.ਮੁੱਲ = spn;
    spn_token.ਲੰਮਾਈ = ਲੜੀ_ਲੰਬਾਈ(spn);

    /* Import the SPN */
    major_ਸਥਿਤੀ = gss_import_name(&minor_ਸਥਿਤੀ, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE,
                                   &nego->spn);
    ਜੇ(GSS_ERROR(major_ਸਥਿਤੀ)) {
      Curl_gss_log_error(ਅੰਕੜੇ, "gss_import_name() failed: ",
                         major_ਸਥਿਤੀ, minor_ਸਥਿਤੀ);

      curlx_free(spn);

      ਵਾਪਸ CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  ਜੇ(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    ਜੇ(*chlg64 != '=') {
      ਨਤੀਜਾ = curlx_base64_decode(chlg64, &chlg, &chlglen);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    /* Ensure we have a valid challenge message */
    ਜੇ(!chlg) {
      infof(ਅੰਕੜੇ, "SPNEGO handshake failure (empty challenge message)");
      ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    ਨਿਵੇਸ਼_token.ਮੁੱਲ = chlg;
    ਨਿਵੇਸ਼_token.ਲੰਮਾਈ = chlglen;
  }

  /* Set channel binding data if available */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GSS_C_CHANNEL_BOUND_FLAG
  ਜੇ(curlx_dyn_len(&nego->channel_binding_ਅੰਕੜੇ)) {
    memset(&chan, 0, ਆਕਾਰ(ਬਣਤਰ gss_channel_bindings_struct));
    chan.application_ਅੰਕੜੇ.ਲੰਮਾਈ = curlx_dyn_len(&nego->channel_binding_ਅੰਕੜੇ);
    chan.application_ਅੰਕੜੇ.ਮੁੱਲ = curlx_dyn_ptr(&nego->channel_binding_ਅੰਕੜੇ);
    chan_bindings = &chan;
  }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSS_ਸੈੱਟ_NEG_MECHS
  /* Acquire explicit credentials and restrict SPNEGO sub-mechanisms to
   * exclude NTLM. We enumerate all available mechanisms and filter out
   * the NTLMSSP OID, matching SSPI's "!ntlm". */
  ਜੇ(nego->cred == GSS_C_NO_CREDENTIAL) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    ਸਥਿਰ ਅਚਲ gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    gss_OID_ਸੈੱਟ ਉਪਲਬਧ_mechs = GSS_C_NO_OID_ਸੈੱਟ;
    gss_OID_ਸੈੱਟ filtered_mechs = GSS_C_NO_OID_ਸੈੱਟ;

    /* Acquire default credentials for SPNEGO */
    major_ਸਥਿਤੀ = Curl_gss_acquire_cred(&minor_ਸਥਿਤੀ, GSS_C_NO_NAME,
                                    GSS_C_INDEFINITE, GSS_C_NO_OID_ਸੈੱਟ,
                                    GSS_C_INITIATE, &nego->cred, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ(GSS_ERROR(major_ਸਥਿਤੀ)) {
      Curl_gss_log_error(ਅੰਕੜੇ, "gss_acquire_cred() failed: ",
                         major_ਸਥਿਤੀ, minor_ਸਥਿਤੀ);
      curlx_safefree(ਨਿਵੇਸ਼_token.ਮੁੱਲ);
      ਵਾਪਸ CURLE_AUTH_ERROR;
    }

    /* Get all available mechanisms */
    major_ਸਥਿਤੀ = Curl_gss_indicate_mechs(&minor_ਸਥਿਤੀ, &ਉਪਲਬਧ_mechs);
    ਜੇ(!GSS_ERROR(major_ਸਥਿਤੀ)) {
      /* Build a set excluding NTLMSSP */
      major_ਸਥਿਤੀ = gss_create_ਖਾਲੀ_ਥਾਵਾਂ_oid_ਸੈੱਟ(&minor_ਸਥਿਤੀ, &filtered_mechs);
      ਜੇ(!GSS_ERROR(major_ਸਥਿਤੀ)) {
        size_t i;
        ਲਈ(i = 0; i < ਉਪਲਬਧ_mechs->ਗਿਣਤੀ_2; i++) {
          gss_OID oid = &ਉਪਲਬਧ_mechs->elements[i];
          ਜੇ(oid->ਲੰਮਾਈ != ntlmssp_oid.ਲੰਮਾਈ ||
             memcmp(oid->elements, ntlmssp_oid.elements, oid->ਲੰਮਾਈ)) {
            gss_ਜੋੜੋ_oid_ਸੈੱਟ_member(&minor_ਸਥਿਤੀ, oid, &filtered_mechs);
          }
        }
        /* Restrict SPNEGO to only use non-NTLM mechanisms */
        major_ਸਥਿਤੀ = Curl_gss_ਸੈੱਟ_neg_mechs(&minor_ਸਥਿਤੀ, nego->cred,
                                              filtered_mechs);
        ਜੇ(GSS_ERROR(major_ਸਥਿਤੀ)) {
          Curl_gss_log_error(ਅੰਕੜੇ, "gss_set_neg_mechs() failed: ",
                             major_ਸਥਿਤੀ, minor_ਸਥਿਤੀ);
        }
        gss_release_oid_ਸੈੱਟ(&minor_ਸਥਿਤੀ, &filtered_mechs);
      }
      gss_release_oid_ਸੈੱਟ(&minor_ਸਥਿਤੀ, &ਉਪਲਬਧ_mechs);
    }
  }
#ਜੇ_ਅੰਤ /* HAVE_GSS_SET_NEG_MECHS */

  /* Generate our challenge-response message */
  major_ਸਥਿਤੀ = Curl_gss_init_sec_context(ਅੰਕੜੇ,
                                           &minor_ਸਥਿਤੀ,
                                           &nego->context,
                                           nego->spn,
                                           &Curl_spnego_mech_oid,
                                           chan_bindings,
                                           &ਨਿਵੇਸ਼_token,
                                           &output_token,
                                           TRUE,
                                           ਨਿਰਾ,
                                           nego->cred);

  /* Free the decoded challenge as it is not required anymore */
  curlx_safefree(ਨਿਵੇਸ਼_token.ਮੁੱਲ);

  nego->ਸਥਿਤੀ = major_ਸਥਿਤੀ;
  ਜੇ(GSS_ERROR(major_ਸਥਿਤੀ)) {
    ਜੇ(output_token.ਮੁੱਲ)
      gss_release_ਬਫਰ(&ਅਣਵਰਤਿਆ_ਸਥਿਤੀ, &output_token);

    Curl_gss_log_error(ਅੰਕੜੇ, "gss_init_sec_context() failed: ",
                       major_ਸਥਿਤੀ, minor_ਸਥਿਤੀ);

    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  ਜੇ(!output_token.ਮੁੱਲ || !output_token.ਲੰਮਾਈ) {
    ਜੇ(output_token.ਮੁੱਲ)
      gss_release_ਬਫਰ(&ਅਣਵਰਤਿਆ_ਸਥਿਤੀ, &output_token);

    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  /* Check if NTLM was selected and is disallowed */
  ਜੇ(nego->context != GSS_C_NO_CONTEXT) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    ਸਥਿਰ ਅਚਲ gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    OM_uint32 inquire_major, inquire_minor;
    gss_OID mech_type = GSS_C_NO_OID;

    inquire_major = Curl_gss_inquire_context(&inquire_minor,
                                             nego->context,
                                             &mech_type);
    ਜੇ(!GSS_ERROR(inquire_major) && mech_type &&
       mech_type->ਲੰਮਾਈ == ntlmssp_oid.ਲੰਮਾਈ &&
       !memcmp(mech_type->elements, ntlmssp_oid.elements,
               ntlmssp_oid.ਲੰਮਾਈ)) {
      infof(ਅੰਕੜੇ, "SPNEGO chose NTLM, but NTLM is not allowed");
      gss_release_ਬਫਰ(&ਅਣਵਰਤਿਆ_ਸਥਿਤੀ, &output_token);
      Curl_auth_cleanup_spnego(nego);
      ਵਾਪਸ CURLE_AUTH_ERROR;
    }
  }

  /* Free previous token */
  ਜੇ(nego->output_token.ਲੰਮਾਈ && nego->output_token.ਮੁੱਲ)
    gss_release_ਬਫਰ(&ਅਣਵਰਤਿਆ_ਸਥਿਤੀ, &nego->output_token);

  nego->output_token = output_token;

  ਵਾਪਸ CURLE_OK;
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
CURLcode Curl_auth_create_spnego_ਸੁਨੇਹਾ(ਬਣਤਰ negotiatedata *nego,
                                         ਅੱਖਰ **outptr, size_t *outlen)
{
  CURLcode ਨਤੀਜਾ;
  OM_uint32 minor_ਸਥਿਤੀ;

  /* Base64 encode the already generated response */
  ਨਤੀਜਾ = curlx_base64_encode(nego->output_token.ਮੁੱਲ,
                               nego->output_token.ਲੰਮਾਈ,
                               outptr, outlen);

  ਜੇ(ਨਤੀਜਾ) {
    gss_release_ਬਫਰ(&minor_ਸਥਿਤੀ, &nego->output_token);
    nego->output_token.ਮੁੱਲ = ਨਿਰਾ;
    nego->output_token.ਲੰਮਾਈ = 0;

    ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(!*outptr || !*outlen) {
    gss_release_ਬਫਰ(&minor_ਸਥਿਤੀ, &nego->output_token);
    nego->output_token.ਮੁੱਲ = ਨਿਰਾ;
    nego->output_token.ਲੰਮਾਈ = 0;

    ਵਾਪਸ CURLE_REMOTE_ACCESS_DENIED;
  }

  ਵਾਪਸ CURLE_OK;
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
ਖਾਲੀ Curl_auth_cleanup_spnego(ਬਣਤਰ negotiatedata *nego)
{
  OM_uint32 minor_ਸਥਿਤੀ;

  /* Free our security context */
  ਜੇ(nego->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_ਸਥਿਤੀ, &nego->context,
                                GSS_C_NO_ਬਫਰ);
    nego->context = GSS_C_NO_CONTEXT;
  }

  /* Free the output token */
  ਜੇ(nego->output_token.ਮੁੱਲ) {
    gss_release_ਬਫਰ(&minor_ਸਥਿਤੀ, &nego->output_token);
    nego->output_token.ਮੁੱਲ = ਨਿਰਾ;
    nego->output_token.ਲੰਮਾਈ = 0;
  }

  /* Free the SPN */
  ਜੇ(nego->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_ਸਥਿਤੀ, &nego->spn);
    nego->spn = GSS_C_NO_NAME;
  }

  /* Free our credentials */
  ਜੇ(nego->cred != GSS_C_NO_CREDENTIAL) {
    Curl_gss_release_cred(&minor_ਸਥਿਤੀ, &nego->cred);
    nego->cred = GSS_C_NO_CREDENTIAL;
  }

  /* Reset any variables */
  nego->ਸਥਿਤੀ = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HAVE_GSSAPI && USE_SPNEGO */
