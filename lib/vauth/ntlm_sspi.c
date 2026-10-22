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
#অন্তর্ভাব "curl_setup.h"

#যদি defined(USE_WINDOWS_SSPI) && defined(USE_NTLM)

#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "curl_ntlm_core.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "curlx/strdup.h"

/*
 * Curl_auth_is_ntlm_supported()
 *
 * This is used to evaluate if NTLM is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if NTLM is supported by Windows SSPI.
 */
bool Curl_auth_is_ntlm_supported(শূন্য)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for NTLM */
  status = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(TEXT(SP_NAME_NTLM)),
                                     &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  যদি(status == SEC_E_OK) {
    Curl_pSecFn->Free_Context_বাফার(SecurityPackage);
  }

  ফেরত status == SEC_E_OK;
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
CURLcode Curl_auth_create_ntlm_type1_message(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো Curl_creds *creds,
                                             ধ্রুবক অক্ষর *default_service,
                                             ধ্রুবক অক্ষর *host,
                                             কাঠামো ntlmdata *ntlm,
                                             কাঠামো bufref *out)
{
  PSecPkgInfo SecurityPackage;
  Sec_বাফার type_1_buf;
  Sec_বাফার_Desc type_1_desc;
  SECURITY_STATUS status;
  unsigned long attrs;
  ধ্রুবক অক্ষর *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_cleanup_ntlm(ntlm);

  /* Query the security package for NTLM */
  status = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(TEXT(SP_NAME_NTLM)),
                                     &SecurityPackage);
  যদি(status != SEC_E_OK) {
    failf(তথ্য, "SSPI: could not get auth info");
    ফেরত CURLE_AUTH_ERROR;
  }

  ntlm->token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_বাফার(SecurityPackage);

  /* Allocate our output buffer */
  ntlm->output_token = curlx_malloc(ntlm->token_max);
  যদি(!ntlm->output_token)
    ফেরত CURLE_OUT_OF_MEMORY;

  যদি(Curl_creds_has_user(creds)) {
    CURLcode ফলাফল;

    /* Populate our identity structure */
    ফলাফল = Curl_create_sspi_identity(
      creds->user, creds->passwd, &ntlm->identity);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    /* Allow proper cleanup of the identity structure */
    ntlm->p_identity = &ntlm->identity;
  }
  অন্যথা
    /* Use the current Windows user */
    ntlm->p_identity = ফাঁকা;

  /* Allocate our credentials handle */
  ntlm->credentials = curlx_calloc(1, আকার(CredHandle));
  যদি(!ntlm->credentials)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* Acquire our credentials handle */
  status = Curl_pSecFn->AcquireCredentialsHandle(ফাঁকা,
                                     CURL_UNCONST(TEXT(SP_NAME_NTLM)),
                                     SECPKG_CRED_OUTBOUND, ফাঁকা,
                                     ntlm->p_identity, ফাঁকা, ফাঁকা,
                                     ntlm->credentials, ফাঁকা);
  যদি(status != SEC_E_OK) {
    curlx_safefree(ntlm->credentials);
    ফেরত CURLE_LOGIN_DENIED;
  }

  /* Allocate our new context handle */
  ntlm->context = curlx_calloc(1, আকার(CtxtHandle));
  যদি(!ntlm->context)
    ফেরত CURLE_OUT_OF_MEMORY;

  ntlm->spn = Curl_auth_build_spn(service, host, ফাঁকা);
  যদি(!ntlm->spn)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* Setup the type-1 "output" security buffer */
  type_1_desc.ulVersion = SECBUFFER_VERSION;
  type_1_desc.cBuffers  = 1;
  type_1_desc.pBuffers  = &type_1_buf;
  type_1_buf.বাফার_Type = SECBUFFER_TOKEN;
  type_1_buf.pv_বাফার   = ntlm->output_token;
  type_1_buf.cb_বাফার   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-1 message */
  status = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials, ফাঁকা,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_NETWORK_DREP,
                                                  ফাঁকা, 0,
                                                  ntlm->context, &type_1_desc,
                                                  &attrs, ফাঁকা);
  যদি(status == SEC_I_COMPLETE_NEEDED ||
     status == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(ntlm->context, &type_1_desc);
  অন্যথা যদি(status == SEC_E_INSUFFICIENT_MEMORY)
    ফেরত CURLE_OUT_OF_MEMORY;
  অন্যথা যদি(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED)
    ফেরত CURLE_AUTH_ERROR;

  /* Return the response. */
  Curl_bufref_set(out, ntlm->output_token, type_1_buf.cb_বাফার, ফাঁকা);
  ফেরত CURLE_OK;
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
CURLcode Curl_auth_decode_ntlm_type2_message(কাঠামো Curl_easy *তথ্য,
                                             ধ্রুবক কাঠামো bufref *type2ref,
                                             কাঠামো ntlmdata *ntlm)
{
  /* Ensure we have a valid type-2 message */
  যদি(!Curl_bufref_len(type2ref)) {
    infof(তথ্য, "NTLM handshake failure (empty type-2 message)");
    ফেরত CURLE_BAD_CONTENT_ENCODING;
  }

  /* Store the challenge for later use */
  ntlm->প্রবেশ_token = curlx_memdup0(Curl_bufref_ptr(type2ref),
                                    Curl_bufref_len(type2ref));
  যদি(!ntlm->প্রবেশ_token)
    ফেরত CURLE_OUT_OF_MEMORY;
  ntlm->প্রবেশ_token_len = Curl_bufref_len(type2ref);

  ফেরত CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_message(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো Curl_creds *creds,
                                             কাঠামো ntlmdata *ntlm,
                                             কাঠামো bufref *out)
{
  CURLcode ফলাফল = CURLE_OK;
  Sec_বাফার type_2_bufs[2];
  Sec_বাফার type_3_buf;
  Sec_বাফার_Desc type_2_desc;
  Sec_বাফার_Desc type_3_desc;
  SECURITY_STATUS status;
  unsigned long attrs;
  SecPkgContext_Bindings pkgBindings = { 0, ফাঁকা };

  (শূন্য)creds;

  /* Setup the type-2 "input" security buffer */
  type_2_desc.ulVersion     = SECBUFFER_VERSION;
  type_2_desc.cBuffers      = 1;
  type_2_desc.pBuffers      = &type_2_bufs[0];
  type_2_bufs[0].বাফার_Type = SECBUFFER_TOKEN;
  type_2_bufs[0].pv_বাফার   = ntlm->প্রবেশ_token;
  type_2_bufs[0].cb_বাফার   = curlx_uztoul(ntlm->প্রবেশ_token_len);

  /* SSL context comes from schannel.
   * When extended protection is used in IIS server,
   * we have to pass a second SecBuffer to the SecBufferDesc
   * otherwise IIS does not pass the authentication (401 response).
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  যদি(ntlm->sslContext) {
    status = Curl_pSecFn->QueryContextAttributes(
      ntlm->sslContext,
      SECPKG_ATTR_ENDPOINT_BINDINGS,
      &pkgBindings
    );
    যদি(status == SEC_E_OK) {
      type_2_desc.cBuffers++;
      type_2_bufs[1].বাফার_Type = SECBUFFER_CHANNEL_BINDINGS;
      type_2_bufs[1].cb_বাফার = pkgBindings.Bindings_দৈর্ঘ্য;
      type_2_bufs[1].pv_বাফার = pkgBindings.Bindings;
    }
  }

  /* Setup the type-3 "output" security buffer */
  type_3_desc.ulVersion = SECBUFFER_VERSION;
  type_3_desc.cBuffers  = 1;
  type_3_desc.pBuffers  = &type_3_buf;
  type_3_buf.বাফার_Type = SECBUFFER_TOKEN;
  type_3_buf.pv_বাফার   = ntlm->output_token;
  type_3_buf.cb_বাফার   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-3 message */
  status = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials,
                                                  ntlm->context,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_NETWORK_DREP,
                                                  &type_2_desc,
                                                  0, ntlm->context,
                                                  &type_3_desc,
                                                  &attrs, ফাঁকা);

  যদি(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_বাফার(pkgBindings.Bindings);

  যদি(status != SEC_E_OK) {
    infof(তথ্য, "NTLM handshake failure (type-3 message): Status=0x%08lx",
          (unsigned long)status);

    যদি(status == SEC_E_INSUFFICIENT_MEMORY)
      ফেরত CURLE_OUT_OF_MEMORY;

    ফেরত CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  ফলাফল = Curl_bufref_memdup0(out, ntlm->output_token, type_3_buf.cb_বাফার);
  Curl_auth_cleanup_ntlm(ntlm);
  ফেরত ফলাফল;
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
শূন্য Curl_auth_cleanup_ntlm(কাঠামো ntlmdata *ntlm)
{
  /* Free our security context */
  যদি(ntlm->context) {
    Curl_pSecFn->DeleteSecurityContext(ntlm->context);
    curlx_safefree(ntlm->context);
  }

  /* Free our credentials handle */
  যদি(ntlm->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(ntlm->credentials);
    curlx_safefree(ntlm->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_identity(ntlm->p_identity);
  ntlm->p_identity = ফাঁকা;

  /* Free the input and output tokens */
  curlx_safefree(ntlm->প্রবেশ_token);
  curlx_safefree(ntlm->output_token);

  /* Reset any variables */
  ntlm->token_max = 0;

  curlx_safefree(ntlm->spn);
}

#endif /* USE_WINDOWS_SSPI && USE_NTLM */
