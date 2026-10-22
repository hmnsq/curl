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
#समावेश "curl_setup.h"

#अगर defined(HAVE_GSSAPI) && defined(USE_SPNEGO)

#समावेश "vauth/vauth.h"
#समावेश "curlx/base64.h"
#समावेश "curl_gssapi.h"
#समावेश "curl_trc.h"

#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wdeprecated-declarations"
#पूर्ण_अगर

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate supported by the GSS-API library.
 */
bool Curl_auth_is_spnego_supported(व्योम)
{
  वापस TRUE;
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
CURLcode Curl_auth_decode_spnego_message(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा Curl_creds *creds,
                                         स्थिर अक्षर *default_service,
                                         स्थिर अक्षर *host,
                                         स्थिर अक्षर *chlg64,
                                         काष्ठा negotiatedata *nego)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार chlglen = 0;
  अचिन्हित अक्षर *chlg = शून्य;
  OM_uint32 major_status;
  OM_uint32 minor_status;
  OM_uint32 unused_status;
  gss_बफर_desc निवेश_token = GSS_C_EMPTY_बफर;
  gss_बफर_desc output_token = GSS_C_EMPTY_बफर;
  gss_channel_bindings_t chan_bindings = GSS_C_NO_CHANNEL_BINDINGS;
#अगर_घोषित GSS_C_CHANNEL_BOUND_FLAG
  काष्ठा gss_channel_bindings_struct chan;
#पूर्ण_अगर

  (व्योम)creds;

  अगर(nego->context && nego->status == GSS_S_COMPLETE) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    वापस CURLE_LOGIN_DENIED;
  }

  अगर(!nego->spn) {
    gss_बफर_desc spn_token = GSS_C_EMPTY_बफर;

    /* Generate our SPN */
    स्थिर अक्षर *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    अक्षर *spn = Curl_auth_build_spn(service, शून्य, host);
    अगर(!spn)
      वापस CURLE_OUT_OF_MEMORY;

    /* Populate the SPN structure */
    spn_token.मूल्य = spn;
    spn_token.लंबाई = म_माप(spn);

    /* Import the SPN */
    major_status = gss_import_name(&minor_status, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE,
                                   &nego->spn);
    अगर(GSS_ERROR(major_status)) {
      Curl_gss_log_error(तथ्य, "gss_import_name() failed: ",
                         major_status, minor_status);

      curlx_free(spn);

      वापस CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  अगर(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    अगर(*chlg64 != '=') {
      परिणाम = curlx_base64_decode(chlg64, &chlg, &chlglen);
      अगर(परिणाम)
        वापस परिणाम;
    }

    /* Ensure we have a valid challenge message */
    अगर(!chlg) {
      infof(तथ्य, "SPNEGO handshake failure (empty challenge message)");
      वापस CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    निवेश_token.मूल्य = chlg;
    निवेश_token.लंबाई = chlglen;
  }

  /* Set channel binding data if available */
#अगर_घोषित GSS_C_CHANNEL_BOUND_FLAG
  अगर(curlx_dyn_len(&nego->channel_binding_तथ्य)) {
    स_रखो(&chan, 0, माप(काष्ठा gss_channel_bindings_struct));
    chan.application_तथ्य.लंबाई = curlx_dyn_len(&nego->channel_binding_तथ्य);
    chan.application_तथ्य.मूल्य = curlx_dyn_ptr(&nego->channel_binding_तथ्य);
    chan_bindings = &chan;
  }
#पूर्ण_अगर

#अगर_घोषित HAVE_GSS_SET_NEG_MECHS
  /* Acquire explicit credentials and restrict SPNEGO sub-mechanisms to
   * exclude NTLM. We enumerate all available mechanisms and filter out
   * the NTLMSSP OID, matching SSPI's "!ntlm". */
  अगर(nego->cred == GSS_C_NO_CREDENTIAL) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    अटल स्थिर gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    gss_OID_set available_mechs = GSS_C_NO_OID_SET;
    gss_OID_set filtered_mechs = GSS_C_NO_OID_SET;

    /* Acquire default credentials for SPNEGO */
    major_status = Curl_gss_acquire_cred(&minor_status, GSS_C_NO_NAME,
                                    GSS_C_INDEFINITE, GSS_C_NO_OID_SET,
                                    GSS_C_INITIATE, &nego->cred, शून्य, शून्य);
    अगर(GSS_ERROR(major_status)) {
      Curl_gss_log_error(तथ्य, "gss_acquire_cred() failed: ",
                         major_status, minor_status);
      curlx_safefree(निवेश_token.मूल्य);
      वापस CURLE_AUTH_ERROR;
    }

    /* Get all available mechanisms */
    major_status = Curl_gss_indicate_mechs(&minor_status, &available_mechs);
    अगर(!GSS_ERROR(major_status)) {
      /* Build a set excluding NTLMSSP */
      major_status = gss_create_empty_oid_set(&minor_status, &filtered_mechs);
      अगर(!GSS_ERROR(major_status)) {
        माप_प्रकार i;
        क्रम(i = 0; i < available_mechs->गिनती; i++) {
          gss_OID oid = &available_mechs->elements[i];
          अगर(oid->लंबाई != ntlmssp_oid.लंबाई ||
             स_भेद(oid->elements, ntlmssp_oid.elements, oid->लंबाई)) {
            gss_add_oid_set_member(&minor_status, oid, &filtered_mechs);
          }
        }
        /* Restrict SPNEGO to only use non-NTLM mechanisms */
        major_status = Curl_gss_set_neg_mechs(&minor_status, nego->cred,
                                              filtered_mechs);
        अगर(GSS_ERROR(major_status)) {
          Curl_gss_log_error(तथ्य, "gss_set_neg_mechs() failed: ",
                             major_status, minor_status);
        }
        gss_release_oid_set(&minor_status, &filtered_mechs);
      }
      gss_release_oid_set(&minor_status, &available_mechs);
    }
  }
#पूर्ण_अगर /* HAVE_GSS_SET_NEG_MECHS */

  /* Generate our challenge-response message */
  major_status = Curl_gss_init_sec_context(तथ्य,
                                           &minor_status,
                                           &nego->context,
                                           nego->spn,
                                           &Curl_spnego_mech_oid,
                                           chan_bindings,
                                           &निवेश_token,
                                           &output_token,
                                           TRUE,
                                           शून्य,
                                           nego->cred);

  /* Free the decoded challenge as it is not required anymore */
  curlx_safefree(निवेश_token.मूल्य);

  nego->status = major_status;
  अगर(GSS_ERROR(major_status)) {
    अगर(output_token.मूल्य)
      gss_release_बफर(&unused_status, &output_token);

    Curl_gss_log_error(तथ्य, "gss_init_sec_context() failed: ",
                       major_status, minor_status);

    वापस CURLE_AUTH_ERROR;
  }

  अगर(!output_token.मूल्य || !output_token.लंबाई) {
    अगर(output_token.मूल्य)
      gss_release_बफर(&unused_status, &output_token);

    वापस CURLE_AUTH_ERROR;
  }

  /* Check if NTLM was selected and is disallowed */
  अगर(nego->context != GSS_C_NO_CONTEXT) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    अटल स्थिर gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    OM_uint32 inquire_major, inquire_minor;
    gss_OID mech_type = GSS_C_NO_OID;

    inquire_major = Curl_gss_inquire_context(&inquire_minor,
                                             nego->context,
                                             &mech_type);
    अगर(!GSS_ERROR(inquire_major) && mech_type &&
       mech_type->लंबाई == ntlmssp_oid.लंबाई &&
       !स_भेद(mech_type->elements, ntlmssp_oid.elements,
               ntlmssp_oid.लंबाई)) {
      infof(तथ्य, "SPNEGO chose NTLM, but NTLM is not allowed");
      gss_release_बफर(&unused_status, &output_token);
      Curl_auth_cleanup_spnego(nego);
      वापस CURLE_AUTH_ERROR;
    }
  }

  /* Free previous token */
  अगर(nego->output_token.लंबाई && nego->output_token.मूल्य)
    gss_release_बफर(&unused_status, &nego->output_token);

  nego->output_token = output_token;

  वापस CURLE_OK;
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
CURLcode Curl_auth_create_spnego_message(काष्ठा negotiatedata *nego,
                                         अक्षर **outptr, माप_प्रकार *outlen)
{
  CURLcode परिणाम;
  OM_uint32 minor_status;

  /* Base64 encode the already generated response */
  परिणाम = curlx_base64_encode(nego->output_token.मूल्य,
                               nego->output_token.लंबाई,
                               outptr, outlen);

  अगर(परिणाम) {
    gss_release_बफर(&minor_status, &nego->output_token);
    nego->output_token.मूल्य = शून्य;
    nego->output_token.लंबाई = 0;

    वापस परिणाम;
  }

  अगर(!*outptr || !*outlen) {
    gss_release_बफर(&minor_status, &nego->output_token);
    nego->output_token.मूल्य = शून्य;
    nego->output_token.लंबाई = 0;

    वापस CURLE_REMOTE_ACCESS_DENIED;
  }

  वापस CURLE_OK;
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
व्योम Curl_auth_cleanup_spnego(काष्ठा negotiatedata *nego)
{
  OM_uint32 minor_status;

  /* Free our security context */
  अगर(nego->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_status, &nego->context,
                                GSS_C_NO_बफर);
    nego->context = GSS_C_NO_CONTEXT;
  }

  /* Free the output token */
  अगर(nego->output_token.मूल्य) {
    gss_release_बफर(&minor_status, &nego->output_token);
    nego->output_token.मूल्य = शून्य;
    nego->output_token.लंबाई = 0;
  }

  /* Free the SPN */
  अगर(nego->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_status, &nego->spn);
    nego->spn = GSS_C_NO_NAME;
  }

  /* Free our credentials */
  अगर(nego->cred != GSS_C_NO_CREDENTIAL) {
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

#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#आशय GCC diagnostic निकालो
#पूर्ण_अगर

#पूर्ण_अगर /* HAVE_GSSAPI && USE_SPNEGO */
