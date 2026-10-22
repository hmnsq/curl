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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ defined(HAVE_GSSAPI) && defined(USE_SPNEGO)

#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "curlx/base64.h"
#അന്തര്ഭാവ "curl_gssapi.h"
#അന്തര്ഭാവ "curl_trc.h"

#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic തള്ളുക
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
bool Curl_auth_is_spnego_supported(ശൂന്യം)
{
  തിരികെ TRUE;
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
CURLcode Curl_auth_decode_spnego_message(ഘടന Curl_easy *data,
                                         ഘടന Curl_creds *creds,
                                         സ്ഥിരം അക്ഷരം *default_service,
                                         സ്ഥിരം അക്ഷരം *host,
                                         സ്ഥിരം അക്ഷരം *chlg64,
                                         ഘടന negotiatedata *nego)
{
  CURLcode result = CURLE_OK;
  size_t chlglen = 0;
  unsigned അക്ഷരം *chlg = ശൂന്യ;
  OM_uint32 major_status;
  OM_uint32 minor_status;
  OM_uint32 unused_status;
  gss_buffer_desc നിവേശം_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;
  gss_channel_bindings_t chan_bindings = GSS_C_NO_CHANNEL_BINDINGS;
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  ഘടന gss_channel_bindings_struct chan;
#endif

  (ശൂന്യം)creds;

  എങ്കിൽ(nego->context && nego->status == GSS_S_COMPLETE) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    തിരികെ CURLE_LOGIN_DENIED;
  }

  എങ്കിൽ(!nego->spn) {
    gss_buffer_desc spn_token = GSS_C_EMPTY_BUFFER;

    /* Generate our SPN */
    സ്ഥിരം അക്ഷരം *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    അക്ഷരം *spn = Curl_auth_build_spn(service, ശൂന്യ, host);
    എങ്കിൽ(!spn)
      തിരികെ CURLE_OUT_OF_MEMORY;

    /* Populate the SPN structure */
    spn_token.മൂല്യം = spn;
    spn_token.length = strlen(spn);

    /* Import the SPN */
    major_status = gss_import_name(&minor_status, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE,
                                   &nego->spn);
    എങ്കിൽ(GSS_ERROR(major_status)) {
      Curl_gss_log_error(data, "gss_import_name() failed: ",
                         major_status, minor_status);

      curlx_free(spn);

      തിരികെ CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  എങ്കിൽ(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    എങ്കിൽ(*chlg64 != '=') {
      result = curlx_base64_decode(chlg64, &chlg, &chlglen);
      എങ്കിൽ(result)
        തിരികെ result;
    }

    /* Ensure we have a valid challenge message */
    എങ്കിൽ(!chlg) {
      infof(data, "SPNEGO handshake failure (empty challenge message)");
      തിരികെ CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    നിവേശം_token.മൂല്യം = chlg;
    നിവേശം_token.length = chlglen;
  }

  /* Set channel binding data if available */
#ifdef GSS_C_CHANNEL_BOUND_FLAG
  എങ്കിൽ(curlx_dyn_len(&nego->channel_binding_data)) {
    memset(&chan, 0, വലുപ്പം(ഘടന gss_channel_bindings_struct));
    chan.application_data.length = curlx_dyn_len(&nego->channel_binding_data);
    chan.application_data.മൂല്യം = curlx_dyn_ptr(&nego->channel_binding_data);
    chan_bindings = &chan;
  }
#endif

#ifdef HAVE_GSS_SET_NEG_MECHS
  /* Acquire explicit credentials and restrict SPNEGO sub-mechanisms to
   * exclude NTLM. We enumerate all available mechanisms and filter out
   * the NTLMSSP OID, matching SSPI's "!ntlm". */
  എങ്കിൽ(nego->cred == GSS_C_NO_CREDENTIAL) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    സ്ഥിതം സ്ഥിരം gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    gss_OID_set available_mechs = GSS_C_NO_OID_SET;
    gss_OID_set filtered_mechs = GSS_C_NO_OID_SET;

    /* Acquire default credentials for SPNEGO */
    major_status = Curl_gss_acquire_cred(&minor_status, GSS_C_NO_NAME,
                                    GSS_C_INDEFINITE, GSS_C_NO_OID_SET,
                                    GSS_C_INITIATE, &nego->cred, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ(GSS_ERROR(major_status)) {
      Curl_gss_log_error(data, "gss_acquire_cred() failed: ",
                         major_status, minor_status);
      curlx_safefree(നിവേശം_token.മൂല്യം);
      തിരികെ CURLE_AUTH_ERROR;
    }

    /* Get all available mechanisms */
    major_status = Curl_gss_indicate_mechs(&minor_status, &available_mechs);
    എങ്കിൽ(!GSS_ERROR(major_status)) {
      /* Build a set excluding NTLMSSP */
      major_status = gss_create_empty_oid_set(&minor_status, &filtered_mechs);
      എങ്കിൽ(!GSS_ERROR(major_status)) {
        size_t i;
        വേണ്ടി(i = 0; i < available_mechs->എണ്ണം; i++) {
          gss_OID oid = &available_mechs->elements[i];
          എങ്കിൽ(oid->length != ntlmssp_oid.length ||
             memcmp(oid->elements, ntlmssp_oid.elements, oid->length)) {
            gss_add_oid_set_member(&minor_status, oid, &filtered_mechs);
          }
        }
        /* Restrict SPNEGO to only use non-NTLM mechanisms */
        major_status = Curl_gss_set_neg_mechs(&minor_status, nego->cred,
                                              filtered_mechs);
        എങ്കിൽ(GSS_ERROR(major_status)) {
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
                                           &നിവേശം_token,
                                           &output_token,
                                           TRUE,
                                           ശൂന്യ,
                                           nego->cred);

  /* Free the decoded challenge as it is not required anymore */
  curlx_safefree(നിവേശം_token.മൂല്യം);

  nego->status = major_status;
  എങ്കിൽ(GSS_ERROR(major_status)) {
    എങ്കിൽ(output_token.മൂല്യം)
      gss_release_buffer(&unused_status, &output_token);

    Curl_gss_log_error(data, "gss_init_sec_context() failed: ",
                       major_status, minor_status);

    തിരികെ CURLE_AUTH_ERROR;
  }

  എങ്കിൽ(!output_token.മൂല്യം || !output_token.length) {
    എങ്കിൽ(output_token.മൂല്യം)
      gss_release_buffer(&unused_status, &output_token);

    തിരികെ CURLE_AUTH_ERROR;
  }

  /* Check if NTLM was selected and is disallowed */
  എങ്കിൽ(nego->context != GSS_C_NO_CONTEXT) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    സ്ഥിതം സ്ഥിരം gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    OM_uint32 inquire_major, inquire_minor;
    gss_OID mech_type = GSS_C_NO_OID;

    inquire_major = Curl_gss_inquire_context(&inquire_minor,
                                             nego->context,
                                             &mech_type);
    എങ്കിൽ(!GSS_ERROR(inquire_major) && mech_type &&
       mech_type->length == ntlmssp_oid.length &&
       !memcmp(mech_type->elements, ntlmssp_oid.elements,
               ntlmssp_oid.length)) {
      infof(data, "SPNEGO chose NTLM, but NTLM is not allowed");
      gss_release_buffer(&unused_status, &output_token);
      Curl_auth_cleanup_spnego(nego);
      തിരികെ CURLE_AUTH_ERROR;
    }
  }

  /* Free previous token */
  എങ്കിൽ(nego->output_token.length && nego->output_token.മൂല്യം)
    gss_release_buffer(&unused_status, &nego->output_token);

  nego->output_token = output_token;

  തിരികെ CURLE_OK;
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
CURLcode Curl_auth_create_spnego_message(ഘടന negotiatedata *nego,
                                         അക്ഷരം **outptr, size_t *outlen)
{
  CURLcode result;
  OM_uint32 minor_status;

  /* Base64 encode the already generated response */
  result = curlx_base64_encode(nego->output_token.മൂല്യം,
                               nego->output_token.length,
                               outptr, outlen);

  എങ്കിൽ(result) {
    gss_release_buffer(&minor_status, &nego->output_token);
    nego->output_token.മൂല്യം = ശൂന്യ;
    nego->output_token.length = 0;

    തിരികെ result;
  }

  എങ്കിൽ(!*outptr || !*outlen) {
    gss_release_buffer(&minor_status, &nego->output_token);
    nego->output_token.മൂല്യം = ശൂന്യ;
    nego->output_token.length = 0;

    തിരികെ CURLE_REMOTE_ACCESS_DENIED;
  }

  തിരികെ CURLE_OK;
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
ശൂന്യം Curl_auth_cleanup_spnego(ഘടന negotiatedata *nego)
{
  OM_uint32 minor_status;

  /* Free our security context */
  എങ്കിൽ(nego->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_status, &nego->context,
                                GSS_C_NO_BUFFER);
    nego->context = GSS_C_NO_CONTEXT;
  }

  /* Free the output token */
  എങ്കിൽ(nego->output_token.മൂല്യം) {
    gss_release_buffer(&minor_status, &nego->output_token);
    nego->output_token.മൂല്യം = ശൂന്യ;
    nego->output_token.length = 0;
  }

  /* Free the SPN */
  എങ്കിൽ(nego->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_status, &nego->spn);
    nego->spn = GSS_C_NO_NAME;
  }

  /* Free our credentials */
  എങ്കിൽ(nego->cred != GSS_C_NO_CREDENTIAL) {
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

#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic എടുക്കുക
#endif

#endif /* HAVE_GSSAPI && USE_SPNEGO */
