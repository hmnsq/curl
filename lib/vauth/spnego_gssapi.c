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
#অন্তর্ভাব "curl_setup.h"

#যদি defined(HAVE_GSSAPI) && defined(USE_SPNEGO)

#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "curl_gssapi.h"
#অন্তর্ভাব "curl_trc.h"

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate supported by the GSS-API library.
 */
bool Curl_auth_is_spnego_supported(শূন্য)
{
  প্রত্যাগম TRUE;
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
CURLcode Curl_auth_decode_spnego_message(সংরচনা Curl_easy *data,
                                         সংরচনা Curl_creds *creds,
                                         ধ্রুব বর্ণ *default_service,
                                         ধ্রুব বর্ণ *host,
                                         ধ্রুব বর্ণ *chlg64,
                                         সংরচনা negotiatedata *nego)
{
  CURLcode result = CURLE_OK;
  size_t chlglen = 0;
  unsigned বর্ণ *chlg = রিক্ত;
  OM_uint32 major_status;
  OM_uint32 minor_status;
  OM_uint32 unused_status;
  gss_buffer_desc প্রবেশ_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;
  gss_channel_bindings_t chan_bindings = GSS_C_NO_CHANNEL_BINDINGS;
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  সংরচনা gss_channel_bindings_struct chan;
#endif

  (শূন্য)creds;

  যদি(nego->context && nego->status == GSS_S_COMPLETE) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    প্রত্যাগম CURLE_LOGIN_DENIED;
  }

  যদি(!nego->spn) {
    gss_buffer_desc spn_token = GSS_C_EMPTY_BUFFER;

    /* Generate our SPN */
    ধ্রুব বর্ণ *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    বর্ণ *spn = Curl_auth_build_spn(service, রিক্ত, host);
    যদি(!spn)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;

    /* Populate the SPN structure */
    spn_token.মান = spn;
    spn_token.length = strlen(spn);

    /* Import the SPN */
    major_status = gss_import_name(&minor_status, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE,
                                   &nego->spn);
    যদি(GSS_ERROR(major_status)) {
      Curl_gss_log_error(data, "gss_import_name() failed: ",
                         major_status, minor_status);

      curlx_free(spn);

      প্রত্যাগম CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  যদি(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    যদি(*chlg64 != '=') {
      result = curlx_base64_decode(chlg64, &chlg, &chlglen);
      যদি(result)
        প্রত্যাগম result;
    }

    /* Ensure we have a valid challenge message */
    যদি(!chlg) {
      infof(data, "SPNEGO handshake failure (empty challenge message)");
      প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    প্রবেশ_token.মান = chlg;
    প্রবেশ_token.length = chlglen;
  }

  /* Set channel binding data if available */
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  যদি(curlx_dyn_len(&nego->channel_binding_data)) {
    memset(&chan, 0, পরিমাণ(সংরচনা gss_channel_bindings_struct));
    chan.application_data.length = curlx_dyn_len(&nego->channel_binding_data);
    chan.application_data.মান = curlx_dyn_ptr(&nego->channel_binding_data);
    chan_bindings = &chan;
  }
#endif

#ifdef HAVE_GSS_SET_NEG_MECHS
  /* Acquire explicit credentials and restrict SPNEGO sub-mechanisms to
   * exclude NTLM. We enumerate all available mechanisms and filter out
   * the NTLMSSP OID, matching SSPI's "!ntlm". */
  যদি(nego->cred == GSS_C_NO_CREDENTIAL) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    স্থির ধ্রুব gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    gss_OID_set available_mechs = GSS_C_NO_OID_SET;
    gss_OID_set filtered_mechs = GSS_C_NO_OID_SET;

    /* Acquire default credentials for SPNEGO */
    major_status = Curl_gss_acquire_cred(&minor_status, GSS_C_NO_NAME,
                                    GSS_C_INDEFINITE, GSS_C_NO_OID_SET,
                                    GSS_C_INITIATE, &nego->cred, রিক্ত, রিক্ত);
    যদি(GSS_ERROR(major_status)) {
      Curl_gss_log_error(data, "gss_acquire_cred() failed: ",
                         major_status, minor_status);
      curlx_safefree(প্রবেশ_token.মান);
      প্রত্যাগম CURLE_AUTH_ERROR;
    }

    /* Get all available mechanisms */
    major_status = Curl_gss_indicate_mechs(&minor_status, &available_mechs);
    যদি(!GSS_ERROR(major_status)) {
      /* Build a set excluding NTLMSSP */
      major_status = gss_create_empty_oid_set(&minor_status, &filtered_mechs);
      যদি(!GSS_ERROR(major_status)) {
        size_t i;
        প্রতি(i = 0; i < available_mechs->গুনতি; i++) {
          gss_OID oid = &available_mechs->elements[i];
          যদি(oid->length != ntlmssp_oid.length ||
             memcmp(oid->elements, ntlmssp_oid.elements, oid->length)) {
            gss_add_oid_set_member(&minor_status, oid, &filtered_mechs);
          }
        }
        /* Restrict SPNEGO to only use non-NTLM mechanisms */
        major_status = Curl_gss_set_neg_mechs(&minor_status, nego->cred,
                                              filtered_mechs);
        যদি(GSS_ERROR(major_status)) {
          Curl_gss_log_error(data, "gss_set_neg_mechs() failed: ",
                             major_status, minor_status);
        }
        gss_release_oid_set(&minor_status, &filtered_mechs);
      }
      gss_release_oid_set(&minor_status, &available_mechs);
    }
  }
#endif /* HAVE_GSS_SET_NEG_MECHS */

  /* Generate our challenge-response message */
  major_status = Curl_gss_init_sec_context(data,
                                           &minor_status,
                                           &nego->context,
                                           nego->spn,
                                           &Curl_spnego_mech_oid,
                                           chan_bindings,
                                           &প্রবেশ_token,
                                           &output_token,
                                           TRUE,
                                           রিক্ত,
                                           nego->cred);

  /* Free the decoded challenge as it is not required anymore */
  curlx_safefree(প্রবেশ_token.মান);

  nego->status = major_status;
  যদি(GSS_ERROR(major_status)) {
    যদি(output_token.মান)
      gss_release_buffer(&unused_status, &output_token);

    Curl_gss_log_error(data, "gss_init_sec_context() failed: ",
                       major_status, minor_status);

    প্রত্যাগম CURLE_AUTH_ERROR;
  }

  যদি(!output_token.মান || !output_token.length) {
    যদি(output_token.মান)
      gss_release_buffer(&unused_status, &output_token);

    প্রত্যাগম CURLE_AUTH_ERROR;
  }

  /* Check if NTLM was selected and is disallowed */
  যদি(nego->context != GSS_C_NO_CONTEXT) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    স্থির ধ্রুব gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    OM_uint32 inquire_major, inquire_minor;
    gss_OID mech_type = GSS_C_NO_OID;

    inquire_major = Curl_gss_inquire_context(&inquire_minor,
                                             nego->context,
                                             &mech_type);
    যদি(!GSS_ERROR(inquire_major) && mech_type &&
       mech_type->length == ntlmssp_oid.length &&
       !memcmp(mech_type->elements, ntlmssp_oid.elements,
               ntlmssp_oid.length)) {
      infof(data, "SPNEGO chose NTLM, but NTLM is not allowed");
      gss_release_buffer(&unused_status, &output_token);
      Curl_auth_cleanup_spnego(nego);
      প্রত্যাগম CURLE_AUTH_ERROR;
    }
  }

  /* Free previous token */
  যদি(nego->output_token.length && nego->output_token.মান)
    gss_release_buffer(&unused_status, &nego->output_token);

  nego->output_token = output_token;

  প্রত্যাগম CURLE_OK;
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
CURLcode Curl_auth_create_spnego_message(সংরচনা negotiatedata *nego,
                                         বর্ণ **outptr, size_t *outlen)
{
  CURLcode result;
  OM_uint32 minor_status;

  /* Base64 encode the already generated response */
  result = curlx_base64_encode(nego->output_token.মান,
                               nego->output_token.length,
                               outptr, outlen);

  যদি(result) {
    gss_release_buffer(&minor_status, &nego->output_token);
    nego->output_token.মান = রিক্ত;
    nego->output_token.length = 0;

    প্রত্যাগম result;
  }

  যদি(!*outptr || !*outlen) {
    gss_release_buffer(&minor_status, &nego->output_token);
    nego->output_token.মান = রিক্ত;
    nego->output_token.length = 0;

    প্রত্যাগম CURLE_REMOTE_ACCESS_DENIED;
  }

  প্রত্যাগম CURLE_OK;
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
শূন্য Curl_auth_cleanup_spnego(সংরচনা negotiatedata *nego)
{
  OM_uint32 minor_status;

  /* Free our security context */
  যদি(nego->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_status, &nego->context,
                                GSS_C_NO_BUFFER);
    nego->context = GSS_C_NO_CONTEXT;
  }

  /* Free the output token */
  যদি(nego->output_token.মান) {
    gss_release_buffer(&minor_status, &nego->output_token);
    nego->output_token.মান = রিক্ত;
    nego->output_token.length = 0;
  }

  /* Free the SPN */
  যদি(nego->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_status, &nego->spn);
    nego->spn = GSS_C_NO_NAME;
  }

  /* Free our credentials */
  যদি(nego->cred != GSS_C_NO_CREDENTIAL) {
    Curl_gss_release_cred(&minor_status, &nego->cred);
    nego->cred = GSS_C_NO_CREDENTIAL;
  }

  /* Reset any variables */
  nego->status = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic বের_করো
#endif

#endif /* HAVE_GSSAPI && USE_SPNEGO */
