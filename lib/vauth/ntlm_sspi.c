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
#ضمن "curl_setup.h"

#إذا defined(USE_WINDOWS_SSPI) && defined(USE_NTLM)

#ضمن "vauth/vauth.h"
#ضمن "curl_ntlm_core.h"
#ضمن "curl_trc.h"
#ضمن "curlx/strdup.h"

/*
 * Curl_auth_is_ntlm_supported()
 *
 * This is used to evaluate if NTLM is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if NTLM is supported by Windows SSPI.
 */
bool Curl_auth_is_ntlm_supported(فراغ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_وضعیت وضعیت;

  /* Query the security package for NTLM */
  وضعیت = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(متن_2(SP_NAME_NTLM)),
                                     &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  إذا(وضعیت == SEC_E_OK) {
    Curl_pSecFn->Free_Context_میانگیر(SecurityPackage);
  }

  أرجع وضعیت == SEC_E_OK;
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
CURLcode Curl_auth_create_ntlm_type1_پیام(بنية Curl_easy *داده,
                                             بنية Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             ثابت حرف *host,
                                             بنية ntlmdata *ntlm,
                                             بنية bufref *بیرون)
{
  PSecPkgInfo SecurityPackage;
  Sec_میانگیر type_1_buf;
  Sec_میانگیر_Desc type_1_desc;
  SECURITY_وضعیت وضعیت;
  بیعلامت دراز attrs;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_cleanup_ntlm(ntlm);

  /* Query the security package for NTLM */
  وضعیت = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(متن_2(SP_NAME_NTLM)),
                                     &SecurityPackage);
  إذا(وضعیت != SEC_E_OK) {
    failf(داده, "SSPI: could not get auth info");
    أرجع CURLE_AUTH_ERROR;
  }

  ntlm->token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_میانگیر(SecurityPackage);

  /* Allocate our output buffer */
  ntlm->output_token = curlx_malloc(ntlm->token_max);
  إذا(!ntlm->output_token)
    أرجع CURLE_بیرون_از_حافظه;

  إذا(Curl_creds_has_user(creds)) {
    CURLcode نتیجه;

    /* Populate our identity structure */
    نتیجه = Curl_create_sspi_عنصر_خنثی(
      creds->user, creds->passwd, &ntlm->عنصر_خنثی);
    إذا(نتیجه)
      أرجع نتیجه;

    /* Allow proper cleanup of the identity structure */
    ntlm->p_عنصر_خنثی = &ntlm->عنصر_خنثی;
  }
  وإلا
    /* Use the current Windows user */
    ntlm->p_عنصر_خنثی = عدم;

  /* Allocate our credentials handle */
  ntlm->credentials = curlx_calloc(1, حجم(CredHandle));
  إذا(!ntlm->credentials)
    أرجع CURLE_بیرون_از_حافظه;

  /* Acquire our credentials handle */
  وضعیت = Curl_pSecFn->AcquireCredentialsHandle(عدم,
                                     CURL_UNCONST(متن_2(SP_NAME_NTLM)),
                                     SECPKG_CRED_OUTBOUND, عدم,
                                     ntlm->p_عنصر_خنثی, عدم, عدم,
                                     ntlm->credentials, عدم);
  إذا(وضعیت != SEC_E_OK) {
    curlx_safefree(ntlm->credentials);
    أرجع CURLE_LOGIN_DENIED;
  }

  /* Allocate our new context handle */
  ntlm->context = curlx_calloc(1, حجم(CtxtHandle));
  إذا(!ntlm->context)
    أرجع CURLE_بیرون_از_حافظه;

  ntlm->spn = Curl_auth_build_spn(service, host, عدم);
  إذا(!ntlm->spn)
    أرجع CURLE_بیرون_از_حافظه;

  /* Setup the type-1 "output" security buffer */
  type_1_desc.ul_نسخه = SECBUFFER_نسخه;
  type_1_desc.cBuffers  = 1;
  type_1_desc.pBuffers  = &type_1_buf;
  type_1_buf.میانگیر_Type = SECBUFFER_TOKEN;
  type_1_buf.pv_میانگیر   = ntlm->output_token;
  type_1_buf.cb_میانگیر   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-1 message */
  وضعیت = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials, عدم,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_شبکه_DREP,
                                                  عدم, 0,
                                                  ntlm->context, &type_1_desc,
                                                  &attrs, عدم);
  إذا(وضعیت == SEC_I_COMPLETE_NEEDED ||
     وضعیت == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(ntlm->context, &type_1_desc);
  وإلا إذا(وضعیت == SEC_E_INSUFFICIENT_حافظه)
    أرجع CURLE_بیرون_از_حافظه;
  وإلا إذا(وضعیت != SEC_E_OK && وضعیت != SEC_I_CONTINUE_NEEDED)
    أرجع CURLE_AUTH_ERROR;

  /* Return the response. */
  Curl_bufref_مجموعه(بیرون, ntlm->output_token, type_1_buf.cb_میانگیر, عدم);
  أرجع CURLE_OK;
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
CURLcode Curl_auth_decode_ntlm_type2_پیام(بنية Curl_easy *داده,
                                             ثابت بنية bufref *type2ref,
                                             بنية ntlmdata *ntlm)
{
  /* Ensure we have a valid type-2 message */
  إذا(!Curl_bufref_len(type2ref)) {
    infof(داده, "NTLM handshake failure (empty type-2 message)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Store the challenge for later use */
  ntlm->درون_داد_token = curlx_memdup0(Curl_bufref_ptr(type2ref),
                                    Curl_bufref_len(type2ref));
  إذا(!ntlm->درون_داد_token)
    أرجع CURLE_بیرون_از_حافظه;
  ntlm->درون_داد_token_len = Curl_bufref_len(type2ref);

  أرجع CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_پیام(بنية Curl_easy *داده,
                                             بنية Curl_creds *creds,
                                             بنية ntlmdata *ntlm,
                                             بنية bufref *بیرون)
{
  CURLcode نتیجه = CURLE_OK;
  Sec_میانگیر type_2_bufs[2];
  Sec_میانگیر type_3_buf;
  Sec_میانگیر_Desc type_2_desc;
  Sec_میانگیر_Desc type_3_desc;
  SECURITY_وضعیت وضعیت;
  بیعلامت دراز attrs;
  SecPkgContext_Bindings pkgBindings = { 0, عدم };

  (فراغ)creds;

  /* Setup the type-2 "input" security buffer */
  type_2_desc.ul_نسخه     = SECBUFFER_نسخه;
  type_2_desc.cBuffers      = 1;
  type_2_desc.pBuffers      = &type_2_bufs[0];
  type_2_bufs[0].میانگیر_Type = SECBUFFER_TOKEN;
  type_2_bufs[0].pv_میانگیر   = ntlm->درون_داد_token;
  type_2_bufs[0].cb_میانگیر   = curlx_uztoul(ntlm->درون_داد_token_len);

  /* SSL context comes from schannel.
   * When extended protection is used in IIS server,
   * we have to pass a second SecBuffer to the SecBufferDesc
   * otherwise IIS does not pass the authentication (401 response).
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  إذا(ntlm->sslContext) {
    وضعیت = Curl_pSecFn->Query_Context_صفات(
      ntlm->sslContext,
      SECPKG_ATTR_ENDPOINT_BINDINGS,
      &pkgBindings
    );
    إذا(وضعیت == SEC_E_OK) {
      type_2_desc.cBuffers++;
      type_2_bufs[1].میانگیر_Type = SECBUFFER_CHANNEL_BINDINGS;
      type_2_bufs[1].cb_میانگیر = pkgBindings.Bindings_درازی;
      type_2_bufs[1].pv_میانگیر = pkgBindings.Bindings;
    }
  }

  /* Setup the type-3 "output" security buffer */
  type_3_desc.ul_نسخه = SECBUFFER_نسخه;
  type_3_desc.cBuffers  = 1;
  type_3_desc.pBuffers  = &type_3_buf;
  type_3_buf.میانگیر_Type = SECBUFFER_TOKEN;
  type_3_buf.pv_میانگیر   = ntlm->output_token;
  type_3_buf.cb_میانگیر   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-3 message */
  وضعیت = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials,
                                                  ntlm->context,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_شبکه_DREP,
                                                  &type_2_desc,
                                                  0, ntlm->context,
                                                  &type_3_desc,
                                                  &attrs, عدم);

  إذا(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_میانگیر(pkgBindings.Bindings);

  إذا(وضعیت != SEC_E_OK) {
    infof(داده, "NTLM handshake failure (type-3 message): Status=0x%08lx",
          (بیعلامت دراز)وضعیت);

    إذا(وضعیت == SEC_E_INSUFFICIENT_حافظه)
      أرجع CURLE_بیرون_از_حافظه;

    أرجع CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  نتیجه = Curl_bufref_memdup0(بیرون, ntlm->output_token, type_3_buf.cb_میانگیر);
  Curl_auth_cleanup_ntlm(ntlm);
  أرجع نتیجه;
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
فراغ Curl_auth_cleanup_ntlm(بنية ntlmdata *ntlm)
{
  /* Free our security context */
  إذا(ntlm->context) {
    Curl_pSecFn->DeleteSecurityContext(ntlm->context);
    curlx_safefree(ntlm->context);
  }

  /* Free our credentials handle */
  إذا(ntlm->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(ntlm->credentials);
    curlx_safefree(ntlm->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_عنصر_خنثی(ntlm->p_عنصر_خنثی);
  ntlm->p_عنصر_خنثی = عدم;

  /* Free the input and output tokens */
  curlx_safefree(ntlm->درون_داد_token);
  curlx_safefree(ntlm->output_token);

  /* Reset any variables */
  ntlm->token_max = 0;

  curlx_safefree(ntlm->spn);
}

#پایان_اگر /* USE_WINDOWS_SSPI && USE_NTLM */
