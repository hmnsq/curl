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
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wdeprecated-declarations"
#پایان_اگر

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
CURLcode Curl_auth_decode_spnego_پیام(بنية Curl_easy *داده,
                                         بنية Curl_creds *creds,
                                         ثابت حرف *default_service,
                                         ثابت حرف *host,
                                         ثابت حرف *chlg64,
                                         بنية negotiatedata *nego)
{
  CURLcode نتیجه = CURLE_OK;
  size_t chlglen = 0;
  بیعلامت حرف *chlg = عدم;
  OM_uint32 major_وضعیت;
  OM_uint32 minor_وضعیت;
  OM_uint32 استفادهنشده_وضعیت;
  gss_میانگیر_desc درون_داد_token = GSS_C_خالی_میانگیر;
  gss_میانگیر_desc output_token = GSS_C_خالی_میانگیر;
  gss_channel_bindings_t chan_bindings = GSS_C_NO_CHANNEL_BINDINGS;
#اگر_تعریف_شده GSS_C_CHANNEL_BOUND_FLAG
  بنية gss_channel_bindings_struct chan;
#پایان_اگر

  (فراغ)creds;

  إذا(nego->context && nego->وضعیت == GSS_S_COMPLETE) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    أرجع CURLE_LOGIN_DENIED;
  }

  إذا(!nego->spn) {
    gss_میانگیر_desc spn_token = GSS_C_خالی_میانگیر;

    /* Generate our SPN */
    ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    حرف *spn = Curl_auth_build_spn(service, عدم, host);
    إذا(!spn)
      أرجع CURLE_بیرون_از_حافظه;

    /* Populate the SPN structure */
    spn_token.مقدار = spn;
    spn_token.درازی = طول_رشته(spn);

    /* Import the SPN */
    major_وضعیت = gss_import_name(&minor_وضعیت, &spn_token,
                                   GSS_C_NT_HOSTBASED_SERVICE,
                                   &nego->spn);
    إذا(GSS_ERROR(major_وضعیت)) {
      Curl_gss_log_error(داده, "gss_import_name() failed: ",
                         major_وضعیت, minor_وضعیت);

      curlx_free(spn);

      أرجع CURLE_AUTH_ERROR;
    }

    curlx_free(spn);
  }

  إذا(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    إذا(*chlg64 != '=') {
      نتیجه = curlx_base64_decode(chlg64, &chlg, &chlglen);
      إذا(نتیجه)
        أرجع نتیجه;
    }

    /* Ensure we have a valid challenge message */
    إذا(!chlg) {
      infof(داده, "SPNEGO handshake failure (empty challenge message)");
      أرجع CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    درون_داد_token.مقدار = chlg;
    درون_داد_token.درازی = chlglen;
  }

  /* Set channel binding data if available */
#اگر_تعریف_شده GSS_C_CHANNEL_BOUND_FLAG
  إذا(curlx_dyn_len(&nego->channel_binding_داده)) {
    memset(&chan, 0, حجم(بنية gss_channel_bindings_struct));
    chan.application_داده.درازی = curlx_dyn_len(&nego->channel_binding_داده);
    chan.application_داده.مقدار = curlx_dyn_ptr(&nego->channel_binding_داده);
    chan_bindings = &chan;
  }
#پایان_اگر

#اگر_تعریف_شده HAVE_GSS_مجموعه_NEG_MECHS
  /* Acquire explicit credentials and restrict SPNEGO sub-mechanisms to
   * exclude NTLM. We enumerate all available mechanisms and filter out
   * the NTLMSSP OID, matching SSPI's "!ntlm". */
  إذا(nego->cred == GSS_C_NO_CREDENTIAL) {
    /* OID 1.3.6.1.4.1.311.2.2.10 (NTLMSSP) */
    ساكن ثابت gss_OID_desc ntlmssp_oid = {
      10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
    };
    gss_OID_مجموعه موجود_mechs = GSS_C_NO_OID_مجموعه;
    gss_OID_مجموعه filtered_mechs = GSS_C_NO_OID_مجموعه;

    /* Acquire default credentials for SPNEGO */
    major_وضعیت = Curl_gss_acquire_cred(&minor_وضعیت, GSS_C_NO_NAME,
                                    GSS_C_INDEFINITE, GSS_C_NO_OID_مجموعه,
                                    GSS_C_INITIATE, &nego->cred, عدم, عدم);
    إذا(GSS_ERROR(major_وضعیت)) {
      Curl_gss_log_error(داده, "gss_acquire_cred() failed: ",
                         major_وضعیت, minor_وضعیت);
      curlx_safefree(درون_داد_token.مقدار);
      أرجع CURLE_AUTH_ERROR;
    }

    /* Get all available mechanisms */
    major_وضعیت = Curl_gss_indicate_mechs(&minor_وضعیت, &موجود_mechs);
    إذا(!GSS_ERROR(major_وضعیت)) {
      /* Build a set excluding NTLMSSP */
      major_وضعیت = gss_create_خالی_oid_مجموعه(&minor_وضعیت, &filtered_mechs);
      إذا(!GSS_ERROR(major_وضعیت)) {
        size_t i;
        لكل(i = 0; i < موجود_mechs->شمار; i++) {
          gss_OID oid = &موجود_mechs->elements[i];
          إذا(oid->درازی != ntlmssp_oid.درازی ||
             memcmp(oid->elements, ntlmssp_oid.elements, oid->درازی)) {
            gss_جمع_کن_oid_مجموعه_member(&minor_وضعیت, oid, &filtered_mechs);
          }
        }
        /* Restrict SPNEGO to only use non-NTLM mechanisms */
        major_وضعیت = Curl_gss_مجموعه_neg_mechs(&minor_وضعیت, nego->cred,
                                              filtered_mechs);
        إذا(GSS_ERROR(major_وضعیت)) {
          Curl_gss_log_error(داده, "gss_set_neg_mechs() failed: ",
                             major_وضعیت, minor_وضعیت);
        }
        gss_release_oid_مجموعه(&minor_وضعیت, &filtered_mechs);
      }
      gss_release_oid_مجموعه(&minor_وضعیت, &موجود_mechs);
    }
  }
#پایان_اگر /* HAVE_GSS_SET_NEG_MECHS */

  /* Generate our challenge-response message */
  major_وضعیت = Curl_gss_init_sec_context(داده,
                                           &minor_وضعیت,
                                           &nego->context,
                                           nego->spn,
                                           &Curl_spnego_mech_oid,
                                           chan_bindings,
                                           &درون_داد_token,
                                           &output_token,
                                           TRUE,
                                           عدم,
                                           nego->cred);

  /* Free the decoded challenge as it is not required anymore */
  curlx_safefree(درون_داد_token.مقدار);

  nego->وضعیت = major_وضعیت;
  إذا(GSS_ERROR(major_وضعیت)) {
    إذا(output_token.مقدار)
      gss_release_میانگیر(&استفادهنشده_وضعیت, &output_token);

    Curl_gss_log_error(داده, "gss_init_sec_context() failed: ",
                       major_وضعیت, minor_وضعیت);

    أرجع CURLE_AUTH_ERROR;
  }

  إذا(!output_token.مقدار || !output_token.درازی) {
    إذا(output_token.مقدار)
      gss_release_میانگیر(&استفادهنشده_وضعیت, &output_token);

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
       mech_type->درازی == ntlmssp_oid.درازی &&
       !memcmp(mech_type->elements, ntlmssp_oid.elements,
               ntlmssp_oid.درازی)) {
      infof(داده, "SPNEGO chose NTLM, but NTLM is not allowed");
      gss_release_میانگیر(&استفادهنشده_وضعیت, &output_token);
      Curl_auth_cleanup_spnego(nego);
      أرجع CURLE_AUTH_ERROR;
    }
  }

  /* Free previous token */
  إذا(nego->output_token.درازی && nego->output_token.مقدار)
    gss_release_میانگیر(&استفادهنشده_وضعیت, &nego->output_token);

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
CURLcode Curl_auth_create_spnego_پیام(بنية negotiatedata *nego,
                                         حرف **outptr, size_t *outlen)
{
  CURLcode نتیجه;
  OM_uint32 minor_وضعیت;

  /* Base64 encode the already generated response */
  نتیجه = curlx_base64_encode(nego->output_token.مقدار,
                               nego->output_token.درازی,
                               outptr, outlen);

  إذا(نتیجه) {
    gss_release_میانگیر(&minor_وضعیت, &nego->output_token);
    nego->output_token.مقدار = عدم;
    nego->output_token.درازی = 0;

    أرجع نتیجه;
  }

  إذا(!*outptr || !*outlen) {
    gss_release_میانگیر(&minor_وضعیت, &nego->output_token);
    nego->output_token.مقدار = عدم;
    nego->output_token.درازی = 0;

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
  OM_uint32 minor_وضعیت;

  /* Free our security context */
  إذا(nego->context != GSS_C_NO_CONTEXT) {
    Curl_gss_delete_sec_context(&minor_وضعیت, &nego->context,
                                GSS_C_NO_میانگیر);
    nego->context = GSS_C_NO_CONTEXT;
  }

  /* Free the output token */
  إذا(nego->output_token.مقدار) {
    gss_release_میانگیر(&minor_وضعیت, &nego->output_token);
    nego->output_token.مقدار = عدم;
    nego->output_token.درازی = 0;
  }

  /* Free the SPN */
  إذا(nego->spn != GSS_C_NO_NAME) {
    gss_release_name(&minor_وضعیت, &nego->spn);
    nego->spn = GSS_C_NO_NAME;
  }

  /* Free our credentials */
  إذا(nego->cred != GSS_C_NO_CREDENTIAL) {
    Curl_gss_release_cred(&minor_وضعیت, &nego->cred);
    nego->cred = GSS_C_NO_CREDENTIAL;
  }

  /* Reset any variables */
  nego->وضعیت = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#رهنمود GCC diagnostic برداشت
#پایان_اگر

#پایان_اگر /* HAVE_GSSAPI && USE_SPNEGO */
