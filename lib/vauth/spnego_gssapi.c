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
#अन्तर्भाव "curl_setup.h"

#यदि defined(HAVE_GSSAPI) && defined(USE_SPNEGO)

#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "curl_gssapi.h"
#अन्तर्भाव "curl_trc.h"

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wdeprecated-declarations"
#यदि_अन्तः

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate supported by the GSS-API library.
 */
bool Curl_auth_is_spnego_supported(शून्य)
{
  प्रत्यागम TRUE;
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
CURLcode Curl_auth_decode_spnego_सन्देशः(संरचना Curl_easy *दत्त,
                                         संरचना Curl_creds *creds,
                                         ध्रुव वर्ण *default_service,
                                         ध्रुव वर्ण *host,
                                         ध्रुव वर्ण *chlg64,
                                         संरचना negotiatedata *nego)
{
  CURLcode परिणाम = CURLE_OK;
  size_t chlglen = 0;
  अचिह्नित वर्ण *chlg = रिक्त;
  OM_uint32 major_स्थितिः;
  OM_uint32 minor_स्थितिः;
  OM_uint32 अप्रयुक्तम्_स्थितिः;
  gss_बफर_desc निवेशः_token = GSS_C_रिक्तानि_बफर;
  gss_बफर_desc output_token = GSS_C_रिक्तानि_बफर;
  gss_channel_bindings_t chan_bindings = GSS_C_NO_CHANNEL_BINDINGS;
#यदि_परिभाषितम् GSS_C_CHANNEL_BOUND_FLAG
  संरचना gss_channel_bindings_struct chan;
#यदि_अन्तः

  (शून्य)creds;

  यदि(nego->context && nego->स्थितिः == GSS_S_COMPLETE) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    प्रत्यागम CURLE_LOGIN_DENIED;
  }

  यदि(!nego->spn) {
    gss_बफर_desc spn_token = GSS_C_रिक्तानि_बफर;

    /* Generate our SPN */
    ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    वर्ण *spn = Curl_auth_build_spn(service, रिक्त, host);
    यदि(!spn)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    /* Populate the SPN structure */
    spn_token.मानम् = spn;
    spn_token.दीर्घता = वाक्यदैर्घ्यम्(spn);

    /* Import the SPN */
    major_स्थितिः = gss_import_name(&minor_स्थितिः, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE,
                                   &nego->spn);
    यदि(GSS_ERROR(major_स्थितिः)) {
      Curl_gss_log_error(दत्त, "gss_import_name() failed: ",
                         major_स्थितिः, minor_स्थितिः);

      curlx_free(spn);

      प्रत्यागम CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  यदि(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    यदि(*chlg64 != '=') {
      परिणाम = curlx_base64_decode(chlg64, &chlg, &chlglen);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }

    /* Ensure we have a valid challenge message */
    यदि(!chlg) {
      infof(दत्त, "SPNEGO handshake failure (empty challenge message)");
      प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    निवेशः_token.मानम् = chlg;
    निवेशः_token.दीर्घता = chlglen;
  }

  /* Set channel binding data if available */
#यदि_परिभाषितम् GSS_C_CHANNEL_BOUND_FLAG
  यदि(curlx_dyn_len(&nego->channel_binding_दत्त)) {
    स्मृतिं_पूरय(&chan, 0, परिमाण(संरचना gss_channel_bindings_struct));
    chan.application_दत्त.दीर्घता = curlx_dyn_len(&nego->channel_binding_दत्त);
    chan.application_दत्त.मानम् = curlx_dyn_ptr(&nego->channel_binding_दत्त);
    chan_bindings = &chan;
  }
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_GSS_समुच्चयः_NEG_MECHS
  /* Acquire explicit credentials and restrict SPNEGO sub-mechanisms to
   * exclude NTLM. We enumerate all available mechanisms and filter out
   * the NTLMSSP OID, matching SSPI's "!ntlm". */
  यदि(nego->cred == GSS_C_NO_CREDENTIAL) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    स्थिर ध्रुव gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    gss_OID_समुच्चयः उपलब्धम्_mechs = GSS_C_NO_OID_समुच्चयः;
    gss_OID_समुच्चयः filtered_mechs = GSS_C_NO_OID_समुच्चयः;

    /* Acquire default credentials for SPNEGO */
    major_स्थितिः = Curl_gss_acquire_cred(&minor_स्थितिः, GSS_C_NO_NAME,
                                    GSS_C_INDEFINITE, GSS_C_NO_OID_समुच्चयः,
                                    GSS_C_INITIATE, &nego->cred, रिक्त, रिक्त);
    यदि(GSS_ERROR(major_स्थितिः)) {
      Curl_gss_log_error(दत्त, "gss_acquire_cred() failed: ",
                         major_स्थितिः, minor_स्थितिः);
      curlx_safefree(निवेशः_token.मानम्);
      प्रत्यागम CURLE_AUTH_ERROR;
    }

    /* Get all available mechanisms */
    major_स्थितिः = Curl_gss_indicate_mechs(&minor_स्थितिः, &उपलब्धम्_mechs);
    यदि(!GSS_ERROR(major_स्थितिः)) {
      /* Build a set excluding NTLMSSP */
      major_स्थितिः = gss_create_रिक्तानि_oid_समुच्चयः(&minor_स्थितिः, &filtered_mechs);
      यदि(!GSS_ERROR(major_स्थितिः)) {
        size_t i;
        प्रति(i = 0; i < उपलब्धम्_mechs->गणनम्; i++) {
          gss_OID oid = &उपलब्धम्_mechs->elements[i];
          यदि(oid->दीर्घता != ntlmssp_oid.दीर्घता ||
             memcmp(oid->elements, ntlmssp_oid.elements, oid->दीर्घता)) {
            gss_सङ्कलय_oid_समुच्चयः_member(&minor_स्थितिः, oid, &filtered_mechs);
          }
        }
        /* Restrict SPNEGO to only use non-NTLM mechanisms */
        major_स्थितिः = Curl_gss_समुच्चयः_neg_mechs(&minor_स्थितिः, nego->cred,
                                              filtered_mechs);
        यदि(GSS_ERROR(major_स्थितिः)) {
          Curl_gss_log_error(दत्त, "gss_set_neg_mechs() failed: ",
                             major_स्थितिः, minor_स्थितिः);
        }
        gss_release_oid_समुच्चयः(&minor_स्थितिः, &filtered_mechs);
      }
      gss_release_oid_समुच्चयः(&minor_स्थितिः, &उपलब्धम्_mechs);
    }
  }
#यदि_अन्तः /* HAVE_GSS_SET_NEG_MECHS */

  /* Generate our challenge-response message */
  major_स्थितिः = Curl_gss_init_sec_context(दत्त,
                                           &minor_स्थितिः,
                                           &nego->context,
                                           nego->spn,
                                           &Curl_spnego_mech_oid,
                                           chan_bindings,
                                           &निवेशः_token,
                                           &output_token,
                                           TRUE,
                                           रिक्त,
                                           nego->cred);

  /* Free the decoded challenge as it is not required anymore */
  curlx_safefree(निवेशः_token.मानम्);

  nego->स्थितिः = major_स्थितिः;
  यदि(GSS_ERROR(major_स्थितिः)) {
    यदि(output_token.मानम्)
      gss_release_बफर(&अप्रयुक्तम्_स्थितिः, &output_token);

    Curl_gss_log_error(दत्त, "gss_init_sec_context() failed: ",
                       major_स्थितिः, minor_स्थितिः);

    प्रत्यागम CURLE_AUTH_ERROR;
  }

  यदि(!output_token.मानम् || !output_token.दीर्घता) {
    यदि(output_token.मानम्)
      gss_release_बफर(&अप्रयुक्तम्_स्थितिः, &output_token);

    प्रत्यागम CURLE_AUTH_ERROR;
  }

  /* Check if NTLM was selected and is disallowed */
  यदि(nego->context != GSS_C_NO_CONTEXT) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    स्थिर ध्रुव gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    OM_uint32 inquire_major, inquire_minor;
    gss_OID mech_type = GSS_C_NO_OID;

    inquire_major = Curl_gss_inquire_context(&inquire_minor,
                                             nego->context,
                                             &mech_type);
    यदि(!GSS_ERROR(inquire_major) && mech_type &&
       mech_type->दीर्घता == ntlmssp_oid.दीर्घता &&
       !memcmp(mech_type->elements, ntlmssp_oid.elements,
               ntlmssp_oid.दीर्घता)) {
      infof(दत्त, "SPNEGO chose NTLM, but NTLM is not allowed");
      gss_release_बफर(&अप्रयुक्तम्_स्थितिः, &output_token);
      Curl_auth_cleanup_spnego(nego);
      प्रत्यागम CURLE_AUTH_ERROR;
    }
  }

  /* Free previous token */
  यदि(nego->output_token.दीर्घता && nego->output_token.मानम्)
    gss_release_बफर(&अप्रयुक्तम्_स्थितिः, &nego->output_token);

  nego->output_token = output_token;

  प्रत्यागम CURLE_OK;
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
CURLcode Curl_auth_create_spnego_सन्देशः(संरचना negotiatedata *nego,
                                         वर्ण **outptr, size_t *outlen)
{
  CURLcode परिणाम;
  OM_uint32 minor_स्थितिः;

  /* Base64 encode the already generated response */
  परिणाम = curlx_base64_encode(nego->output_token.मानम्,
                               nego->output_token.दीर्घता,
                               outptr, outlen);

  यदि(परिणाम) {
    gss_release_बफर(&minor_स्थितिः, &nego->output_token);
    nego->output_token.मानम् = रिक्त;
    nego->output_token.दीर्घता = 0;

    प्रत्यागम परिणाम;
  }

  यदि(!*outptr || !*outlen) {
    gss_release_बफर(&minor_स्थितिः, &nego->output_token);
    nego->output_token.मानम् = रिक्त;
    nego->output_token.दीर्घता = 0;

    प्रत्यागम CURLE_REMOTE_ACCESS_DENIED;
  }

  प्रत्यागम CURLE_OK;
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
शून्य Curl_auth_cleanup_spnego(संरचना negotiatedata *nego)
{
  OM_uint32 minor_स्थितिः;

  /* Free our security context */
  यदि(nego->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_स्थितिः, &nego->context,
                                GSS_C_NO_बफर);
    nego->context = GSS_C_NO_CONTEXT;
  }

  /* Free the output token */
  यदि(nego->output_token.मानम्) {
    gss_release_बफर(&minor_स्थितिः, &nego->output_token);
    nego->output_token.मानम् = रिक्त;
    nego->output_token.दीर्घता = 0;
  }

  /* Free the SPN */
  यदि(nego->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_स्थितिः, &nego->spn);
    nego->spn = GSS_C_NO_NAME;
  }

  /* Free our credentials */
  यदि(nego->cred != GSS_C_NO_CREDENTIAL) {
    Curl_gss_release_cred(&minor_स्थितिः, &nego->cred);
    nego->cred = GSS_C_NO_CREDENTIAL;
  }

  /* Reset any variables */
  nego->स्थितिः = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः

#यदि_अन्तः /* HAVE_GSSAPI && USE_SPNEGO */
