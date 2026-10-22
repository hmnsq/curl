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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(HAVE_GSSAPI) && defined(USE_SPNEGO)

#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "curl_gssapi.h"
#అన్తర్భావ "curl_trc.h"

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wdeprecated-declarations"
#అయితే_ముగింపు

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate supported by the GSS-API library.
 */
bool Curl_auth_is_spnego_supported(శూన్య)
{
  తిరిగి TRUE;
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
CURLcode Curl_auth_decode_spnego_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం Curl_creds *creds,
                                         స్థిరరాశి అక్షరం *default_service,
                                         స్థిరరాశి అక్షరం *host,
                                         స్థిరరాశి అక్షరం *chlg64,
                                         నిర్మాణం negotiatedata *nego)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t chlglen = 0;
  గుర్తులేని అక్షరం *chlg = ఖాళీ;
  OM_uint32 major_స్థితి;
  OM_uint32 minor_స్థితి;
  OM_uint32 వాడని_స్థితి;
  gss_బఫరు_desc ఆదానం_token = GSS_C_ఖాళీలు_బఫరు;
  gss_బఫరు_desc output_token = GSS_C_ఖాళీలు_బఫరు;
  gss_channel_bindings_t chan_bindings = GSS_C_NO_CHANNEL_BINDINGS;
#నిర్వచితమైతే GSS_C_CHANNEL_BOUND_FLAG
  నిర్మాణం gss_channel_bindings_struct chan;
#అయితే_ముగింపు

  (శూన్య)creds;

  ఒకవేళ(nego->context && nego->స్థితి_2 == GSS_S_COMPLETE) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    తిరిగి CURLE_LOGIN_DENIED;
  }

  ఒకవేళ(!nego->spn) {
    gss_బఫరు_desc spn_token = GSS_C_ఖాళీలు_బఫరు;

    /* Generate our SPN */
    స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    అక్షరం *spn = Curl_auth_build_spn(service, ఖాళీ, host);
    ఒకవేళ(!spn)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    /* Populate the SPN structure */
    spn_token.విలువ = spn;
    spn_token.నిడివి = అక్షరశ్రేణి_పొడవు(spn);

    /* Import the SPN */
    major_స్థితి = gss_import_name(&minor_స్థితి, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE,
                                   &nego->spn);
    ఒకవేళ(GSS_ERROR(major_స్థితి)) {
      Curl_gss_log_error(దత్తాంశం, "gss_import_name() failed: ",
                         major_స్థితి, minor_స్థితి);

      curlx_free(spn);

      తిరిగి CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  ఒకవేళ(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    ఒకవేళ(*chlg64 != '=') {
      ఫలితం = curlx_base64_decode(chlg64, &chlg, &chlglen);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }

    /* Ensure we have a valid challenge message */
    ఒకవేళ(!chlg) {
      infof(దత్తాంశం, "SPNEGO handshake failure (empty challenge message)");
      తిరిగి CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    ఆదానం_token.విలువ = chlg;
    ఆదానం_token.నిడివి = chlglen;
  }

  /* Set channel binding data if available */
#నిర్వచితమైతే GSS_C_CHANNEL_BOUND_FLAG
  ఒకవేళ(curlx_dyn_len(&nego->channel_binding_దత్తాంశం)) {
    memset(&chan, 0, పరిమాణం(నిర్మాణం gss_channel_bindings_struct));
    chan.application_దత్తాంశం.నిడివి = curlx_dyn_len(&nego->channel_binding_దత్తాంశం);
    chan.application_దత్తాంశం.విలువ = curlx_dyn_ptr(&nego->channel_binding_దత్తాంశం);
    chan_bindings = &chan;
  }
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_GSS_సమితి_NEG_MECHS
  /* Acquire explicit credentials and restrict SPNEGO sub-mechanisms to
   * exclude NTLM. We enumerate all available mechanisms and filter out
   * the NTLMSSP OID, matching SSPI's "!ntlm". */
  ఒకవేళ(nego->cred == GSS_C_NO_CREDENTIAL) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    స్థిర స్థిరరాశి gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    gss_OID_సమితి అందుబాటు_mechs = GSS_C_NO_OID_సమితి;
    gss_OID_సమితి filtered_mechs = GSS_C_NO_OID_సమితి;

    /* Acquire default credentials for SPNEGO */
    major_స్థితి = Curl_gss_acquire_cred(&minor_స్థితి, GSS_C_NO_NAME,
                                    GSS_C_INDEFINITE, GSS_C_NO_OID_సమితి,
                                    GSS_C_INITIATE, &nego->cred, ఖాళీ, ఖాళీ);
    ఒకవేళ(GSS_ERROR(major_స్థితి)) {
      Curl_gss_log_error(దత్తాంశం, "gss_acquire_cred() failed: ",
                         major_స్థితి, minor_స్థితి);
      curlx_safefree(ఆదానం_token.విలువ);
      తిరిగి CURLE_AUTH_ERROR;
    }

    /* Get all available mechanisms */
    major_స్థితి = Curl_gss_indicate_mechs(&minor_స్థితి, &అందుబాటు_mechs);
    ఒకవేళ(!GSS_ERROR(major_స్థితి)) {
      /* Build a set excluding NTLMSSP */
      major_స్థితి = gss_create_ఖాళీలు_oid_సమితి(&minor_స్థితి, &filtered_mechs);
      ఒకవేళ(!GSS_ERROR(major_స్థితి)) {
        size_t i;
        కొరకు(i = 0; i < అందుబాటు_mechs->లెక్క; i++) {
          gss_OID oid = &అందుబాటు_mechs->elements[i];
          ఒకవేళ(oid->నిడివి != ntlmssp_oid.నిడివి ||
             memcmp(oid->elements, ntlmssp_oid.elements, oid->నిడివి)) {
            gss_కూడు_oid_సమితి_member(&minor_స్థితి, oid, &filtered_mechs);
          }
        }
        /* Restrict SPNEGO to only use non-NTLM mechanisms */
        major_స్థితి = Curl_gss_సమితి_neg_mechs(&minor_స్థితి, nego->cred,
                                              filtered_mechs);
        ఒకవేళ(GSS_ERROR(major_స్థితి)) {
          Curl_gss_log_error(దత్తాంశం, "gss_set_neg_mechs() failed: ",
                             major_స్థితి, minor_స్థితి);
        }
        gss_release_oid_సమితి(&minor_స్థితి, &filtered_mechs);
      }
      gss_release_oid_సమితి(&minor_స్థితి, &అందుబాటు_mechs);
    }
  }
#అయితే_ముగింపు /* HAVE_GSS_SET_NEG_MECHS */

  /* Generate our challenge-response message */
  major_స్థితి = Curl_gss_init_sec_context(దత్తాంశం,
                                           &minor_స్థితి,
                                           &nego->context,
                                           nego->spn,
                                           &Curl_spnego_mech_oid,
                                           chan_bindings,
                                           &ఆదానం_token,
                                           &output_token,
                                           TRUE,
                                           ఖాళీ,
                                           nego->cred);

  /* Free the decoded challenge as it is not required anymore */
  curlx_safefree(ఆదానం_token.విలువ);

  nego->స్థితి_2 = major_స్థితి;
  ఒకవేళ(GSS_ERROR(major_స్థితి)) {
    ఒకవేళ(output_token.విలువ)
      gss_release_బఫరు(&వాడని_స్థితి, &output_token);

    Curl_gss_log_error(దత్తాంశం, "gss_init_sec_context() failed: ",
                       major_స్థితి, minor_స్థితి);

    తిరిగి CURLE_AUTH_ERROR;
  }

  ఒకవేళ(!output_token.విలువ || !output_token.నిడివి) {
    ఒకవేళ(output_token.విలువ)
      gss_release_బఫరు(&వాడని_స్థితి, &output_token);

    తిరిగి CURLE_AUTH_ERROR;
  }

  /* Check if NTLM was selected and is disallowed */
  ఒకవేళ(nego->context != GSS_C_NO_CONTEXT) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    స్థిర స్థిరరాశి gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    OM_uint32 inquire_major, inquire_minor;
    gss_OID mech_type = GSS_C_NO_OID;

    inquire_major = Curl_gss_inquire_context(&inquire_minor,
                                             nego->context,
                                             &mech_type);
    ఒకవేళ(!GSS_ERROR(inquire_major) && mech_type &&
       mech_type->నిడివి == ntlmssp_oid.నిడివి &&
       !memcmp(mech_type->elements, ntlmssp_oid.elements,
               ntlmssp_oid.నిడివి)) {
      infof(దత్తాంశం, "SPNEGO chose NTLM, but NTLM is not allowed");
      gss_release_బఫరు(&వాడని_స్థితి, &output_token);
      Curl_auth_cleanup_spnego(nego);
      తిరిగి CURLE_AUTH_ERROR;
    }
  }

  /* Free previous token */
  ఒకవేళ(nego->output_token.నిడివి && nego->output_token.విలువ)
    gss_release_బఫరు(&వాడని_స్థితి, &nego->output_token);

  nego->output_token = output_token;

  తిరిగి CURLE_OK;
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
CURLcode Curl_auth_create_spnego_సందేశం(నిర్మాణం negotiatedata *nego,
                                         అక్షరం **outptr, size_t *outlen)
{
  CURLcode ఫలితం;
  OM_uint32 minor_స్థితి;

  /* Base64 encode the already generated response */
  ఫలితం = curlx_base64_encode(nego->output_token.విలువ,
                               nego->output_token.నిడివి,
                               outptr, outlen);

  ఒకవేళ(ఫలితం) {
    gss_release_బఫరు(&minor_స్థితి, &nego->output_token);
    nego->output_token.విలువ = ఖాళీ;
    nego->output_token.నిడివి = 0;

    తిరిగి ఫలితం;
  }

  ఒకవేళ(!*outptr || !*outlen) {
    gss_release_బఫరు(&minor_స్థితి, &nego->output_token);
    nego->output_token.విలువ = ఖాళీ;
    nego->output_token.నిడివి = 0;

    తిరిగి CURLE_REMOTE_ACCESS_DENIED;
  }

  తిరిగి CURLE_OK;
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
శూన్య Curl_auth_cleanup_spnego(నిర్మాణం negotiatedata *nego)
{
  OM_uint32 minor_స్థితి;

  /* Free our security context */
  ఒకవేళ(nego->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_స్థితి, &nego->context,
                                GSS_C_NO_బఫరు);
    nego->context = GSS_C_NO_CONTEXT;
  }

  /* Free the output token */
  ఒకవేళ(nego->output_token.విలువ) {
    gss_release_బఫరు(&minor_స్థితి, &nego->output_token);
    nego->output_token.విలువ = ఖాళీ;
    nego->output_token.నిడివి = 0;
  }

  /* Free the SPN */
  ఒకవేళ(nego->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_స్థితి, &nego->spn);
    nego->spn = GSS_C_NO_NAME;
  }

  /* Free our credentials */
  ఒకవేళ(nego->cred != GSS_C_NO_CREDENTIAL) {
    Curl_gss_release_cred(&minor_స్థితి, &nego->cred);
    nego->cred = GSS_C_NO_CREDENTIAL;
  }

  /* Reset any variables */
  nego->స్థితి_2 = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు

#అయితే_ముగింపు /* HAVE_GSSAPI && USE_SPNEGO */
