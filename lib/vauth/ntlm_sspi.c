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
 ***************************************************************************/
#अन्तर्भाव "curl_setup.h"

#यदि defined(USE_WINDOWS_SSPI) && defined(USE_NTLM)

#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "curl_ntlm_core.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curlx/strdup.h"

/*
 * Curl_auth_is_ntlm_supported()
 *
 * This is used to evaluate if NTLM is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if NTLM is supported by Windows SSPI.
 */
bool Curl_auth_is_ntlm_supported(शून्य)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_स्थितिः स्थितिः;

  /* Query the security package for NTLM */
  स्थितिः = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(TEXT(SP_NAME_NTLM)),
                                     &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  यदि(स्थितिः == SEC_E_OK) {
    Curl_pSecFn->Free_Context_बफर(SecurityPackage);
  }

  प्रत्यागम स्थितिः == SEC_E_OK;
}

/*
 * Curl_auth_create_ntlm_type1_message()
 *
 * This is used to generate an already encoded NTLM type-1 message ready for
 * sending to the recipient.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * userp   [in]     - The username in the format User or Domain\User.
 * passwdp [in]     - The user's password.
 * service [in]     - The service type such as http, smtp, pop or imap.
 * host    [in]     - The hostname.
 * ntlm    [in/out] - The NTLM data struct being used and modified.
 * out     [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_ntlm_type1_सन्देशः(संरचना Curl_easy *दत्त,
                                             संरचना Curl_creds *creds,
                                             ध्रुव वर्ण *default_service,
                                             ध्रुव वर्ण *host,
                                             संरचना ntlmdata *ntlm,
                                             संरचना bufref *बहिर्गमनम्)
{
  PSecPkgInfo SecurityPackage;
  Sec_बफर type_1_buf;
  Sec_बफर_Desc type_1_desc;
  SECURITY_स्थितिः स्थितिः;
  अचिह्नित दीर्घ attrs;
  ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_cleanup_ntlm(ntlm);

  /* Query the security package for NTLM */
  स्थितिः = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(TEXT(SP_NAME_NTLM)),
                                     &SecurityPackage);
  यदि(स्थितिः != SEC_E_OK) {
    failf(दत्त, "SSPI: could not get auth info");
    प्रत्यागम CURLE_AUTH_ERROR;
  }

  ntlm->token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_बफर(SecurityPackage);

  /* Allocate our output buffer */
  ntlm->output_token = curlx_malloc(ntlm->token_max);
  यदि(!ntlm->output_token)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(Curl_creds_has_user(creds)) {
    CURLcode परिणाम;

    /* Populate our identity structure */
    परिणाम = Curl_create_sspi_तटस्थः(
      creds->user, creds->passwd, &ntlm->तटस्थः);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    /* Allow proper cleanup of the identity structure */
    ntlm->p_तटस्थः = &ntlm->तटस्थः;
  }
  अन्यथा
    /* Use the current Windows user */
    ntlm->p_तटस्थः = रिक्त;

  /* Allocate our credentials handle */
  ntlm->credentials = curlx_calloc(1, परिमाण(CredHandle));
  यदि(!ntlm->credentials)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Acquire our credentials handle */
  स्थितिः = Curl_pSecFn->AcquireCredentialsHandle(रिक्त,
                                     CURL_UNCONST(TEXT(SP_NAME_NTLM)),
                                     SECPKG_CRED_OUTBOUND, रिक्त,
                                     ntlm->p_तटस्थः, रिक्त, रिक्त,
                                     ntlm->credentials, रिक्त);
  यदि(स्थितिः != SEC_E_OK) {
    curlx_safefree(ntlm->credentials);
    प्रत्यागम CURLE_LOGIN_DENIED;
  }

  /* Allocate our new context handle */
  ntlm->context = curlx_calloc(1, परिमाण(CtxtHandle));
  यदि(!ntlm->context)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  ntlm->spn = Curl_auth_build_spn(service, host, रिक्त);
  यदि(!ntlm->spn)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Setup the type-1 "output" security buffer */
  type_1_desc.ul_संस्करणम् = SECBUFFER_संस्करणम्;
  type_1_desc.cBuffers  = 1;
  type_1_desc.pBuffers  = &type_1_buf;
  type_1_buf.बफर_Type = SECBUFFER_TOKEN;
  type_1_buf.pv_बफर   = ntlm->output_token;
  type_1_buf.cb_बफर   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-1 message */
  स्थितिः = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials, रिक्त,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_जालम्_DREP,
                                                  रिक्त, 0,
                                                  ntlm->context, &type_1_desc,
                                                  &attrs, रिक्त);
  यदि(स्थितिः == SEC_I_COMPLETE_NEEDED ||
     स्थितिः == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(ntlm->context, &type_1_desc);
  अन्यथा यदि(स्थितिः == SEC_E_INSUFFICIENT_स्मृतिः)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  अन्यथा यदि(स्थितिः != SEC_E_OK && स्थितिः != SEC_I_CONTINUE_NEEDED)
    प्रत्यागम CURLE_AUTH_ERROR;

  /* Return the response. */
  Curl_bufref_समुच्चयः(बहिर्गमनम्, ntlm->output_token, type_1_buf.cb_बफर, रिक्त);
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_auth_decode_ntlm_type2_message()
 *
 * This is used to decode an already encoded NTLM type-2 message.
 *
 * Parameters:
 *
 * data     [in]     - The session handle.
 * type2    [in]     - The type-2 message.
 * ntlm     [in/out] - The NTLM data struct being used and modified.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_decode_ntlm_type2_सन्देशः(संरचना Curl_easy *दत्त,
                                             ध्रुव संरचना bufref *type2ref,
                                             संरचना ntlmdata *ntlm)
{
  /* Ensure we have a valid type-2 message */
  यदि(!Curl_bufref_len(type2ref)) {
    infof(दत्त, "NTLM handshake failure (empty type-2 message)");
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Store the challenge for later use */
  ntlm->निवेशः_token = curlx_memdup0(Curl_bufref_ptr(type2ref),
                                    Curl_bufref_len(type2ref));
  यदि(!ntlm->निवेशः_token)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  ntlm->निवेशः_token_len = Curl_bufref_len(type2ref);

  प्रत्यागम CURLE_OK;
}

/*
 * Curl_auth_create_ntlm_type3_message()
 *
 * This is used to generate an already encoded NTLM type-3 message ready for
 * sending to the recipient.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * userp   [in]     - The username in the format User or Domain\User.
 * passwdp [in]     - The user's password.
 * ntlm    [in/out] - The NTLM data struct being used and modified.
 * out     [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_ntlm_type3_सन्देशः(संरचना Curl_easy *दत्त,
                                             संरचना Curl_creds *creds,
                                             संरचना ntlmdata *ntlm,
                                             संरचना bufref *बहिर्गमनम्)
{
  CURLcode परिणाम = CURLE_OK;
  Sec_बफर type_2_bufs[2];
  Sec_बफर type_3_buf;
  Sec_बफर_Desc type_2_desc;
  Sec_बफर_Desc type_3_desc;
  SECURITY_स्थितिः स्थितिः;
  अचिह्नित दीर्घ attrs;
  SecPkgContext_Bindings pkgBindings = { 0, रिक्त };

  (शून्य)creds;

  /* Setup the type-2 "input" security buffer */
  type_2_desc.ul_संस्करणम्     = SECBUFFER_संस्करणम्;
  type_2_desc.cBuffers      = 1;
  type_2_desc.pBuffers      = &type_2_bufs[0];
  type_2_bufs[0].बफर_Type = SECBUFFER_TOKEN;
  type_2_bufs[0].pv_बफर   = ntlm->निवेशः_token;
  type_2_bufs[0].cb_बफर   = curlx_uztoul(ntlm->निवेशः_token_len);

  /* SSL context comes from schannel.
   * When extended protection is used in IIS server,
   * we have to pass a second SecBuffer to the SecBufferDesc
   * otherwise IIS does not pass the authentication (401 response).
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  यदि(ntlm->sslContext) {
    स्थितिः = Curl_pSecFn->Query_Context_गुणाः(
      ntlm->sslContext,
      SECPKG_ATTR_ENDPOINT_BINDINGS,
      &pkgBindings
    );
    यदि(स्थितिः == SEC_E_OK) {
      type_2_desc.cBuffers++;
      type_2_bufs[1].बफर_Type = SECBUFFER_CHANNEL_BINDINGS;
      type_2_bufs[1].cb_बफर = pkgBindings.Bindings_दीर्घता;
      type_2_bufs[1].pv_बफर = pkgBindings.Bindings;
    }
  }

  /* Setup the type-3 "output" security buffer */
  type_3_desc.ul_संस्करणम् = SECBUFFER_संस्करणम्;
  type_3_desc.cBuffers  = 1;
  type_3_desc.pBuffers  = &type_3_buf;
  type_3_buf.बफर_Type = SECBUFFER_TOKEN;
  type_3_buf.pv_बफर   = ntlm->output_token;
  type_3_buf.cb_बफर   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-3 message */
  स्थितिः = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials,
                                                  ntlm->context,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_जालम्_DREP,
                                                  &type_2_desc,
                                                  0, ntlm->context,
                                                  &type_3_desc,
                                                  &attrs, रिक्त);

  यदि(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_बफर(pkgBindings.Bindings);

  यदि(स्थितिः != SEC_E_OK) {
    infof(दत्त, "NTLM handshake failure (type-3 message): Status=0x%08lx",
          (अचिह्नित दीर्घ)स्थितिः);

    यदि(स्थितिः == SEC_E_INSUFFICIENT_स्मृतिः)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    प्रत्यागम CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  परिणाम = Curl_bufref_memdup0(बहिर्गमनम्, ntlm->output_token, type_3_buf.cb_बफर);
  Curl_auth_cleanup_ntlm(ntlm);
  प्रत्यागम परिणाम;
}

/*
 * Curl_auth_cleanup_ntlm()
 *
 * This is used to clean up the NTLM specific data.
 *
 * Parameters:
 *
 * ntlm    [in/out] - The NTLM data struct being cleaned up.
 *
 */
शून्य Curl_auth_cleanup_ntlm(संरचना ntlmdata *ntlm)
{
  /* Free our security context */
  यदि(ntlm->context) {
    Curl_pSecFn->DeleteSecurityContext(ntlm->context);
    curlx_safefree(ntlm->context);
  }

  /* Free our credentials handle */
  यदि(ntlm->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(ntlm->credentials);
    curlx_safefree(ntlm->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_तटस्थः(ntlm->p_तटस्थः);
  ntlm->p_तटस्थः = रिक्त;

  /* Free the input and output tokens */
  curlx_safefree(ntlm->निवेशः_token);
  curlx_safefree(ntlm->output_token);

  /* Reset any variables */
  ntlm->token_max = 0;

  curlx_safefree(ntlm->spn);
}

#यदि_अन्तः /* USE_WINDOWS_SSPI && USE_NTLM */
