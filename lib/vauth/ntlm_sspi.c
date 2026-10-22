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
  SECURITY_حالت حالت;

  /* Query the security package for NTLM */
  حالت = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(متن_2(SP_NAME_NTLM)),
                                     &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  إذا(حالت == SEC_E_OK) {
    Curl_pSecFn->Free_Context_بفر(SecurityPackage);
  }

  أرجع حالت == SEC_E_OK;
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
CURLcode Curl_auth_create_ntlm_type1_پیغام(بنية Curl_easy *معلومات,
                                             بنية Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             ثابت حرف *host,
                                             بنية ntlmdata *ntlm,
                                             بنية bufref *بهر)
{
  PSecPkgInfo SecurityPackage;
  Sec_بفر type_1_buf;
  Sec_بفر_Desc type_1_desc;
  SECURITY_حالت حالت;
  بینښه اوږد attrs;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_cleanup_ntlm(ntlm);

  /* Query the security package for NTLM */
  حالت = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(متن_2(SP_NAME_NTLM)),
                                     &SecurityPackage);
  إذا(حالت != SEC_E_OK) {
    failf(معلومات, "SSPI: could not get auth info");
    أرجع CURLE_AUTH_ERROR;
  }

  ntlm->token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_بفر(SecurityPackage);

  /* Allocate our output buffer */
  ntlm->output_token = curlx_malloc(ntlm->token_max);
  إذا(!ntlm->output_token)
    أرجع CURLE_بهر_د_حافظه;

  إذا(Curl_creds_has_user(creds)) {
    CURLcode پایله;

    /* Populate our identity structure */
    پایله = Curl_create_sspi_بې_اغېزه_عنصر(
      creds->user, creds->passwd, &ntlm->بې_اغېزه_عنصر);
    إذا(پایله)
      أرجع پایله;

    /* Allow proper cleanup of the identity structure */
    ntlm->p_بې_اغېزه_عنصر = &ntlm->بې_اغېزه_عنصر;
  }
  وإلا
    /* Use the current Windows user */
    ntlm->p_بې_اغېزه_عنصر = عدم;

  /* Allocate our credentials handle */
  ntlm->credentials = curlx_calloc(1, حجم(CredHandle));
  إذا(!ntlm->credentials)
    أرجع CURLE_بهر_د_حافظه;

  /* Acquire our credentials handle */
  حالت = Curl_pSecFn->AcquireCredentialsHandle(عدم,
                                     CURL_UNCONST(متن_2(SP_NAME_NTLM)),
                                     SECPKG_CRED_OUTBOUND, عدم,
                                     ntlm->p_بې_اغېزه_عنصر, عدم, عدم,
                                     ntlm->credentials, عدم);
  إذا(حالت != SEC_E_OK) {
    curlx_safefree(ntlm->credentials);
    أرجع CURLE_LOGIN_DENIED;
  }

  /* Allocate our new context handle */
  ntlm->context = curlx_calloc(1, حجم(CtxtHandle));
  إذا(!ntlm->context)
    أرجع CURLE_بهر_د_حافظه;

  ntlm->spn = Curl_auth_build_spn(service, host, عدم);
  إذا(!ntlm->spn)
    أرجع CURLE_بهر_د_حافظه;

  /* Setup the type-1 "output" security buffer */
  type_1_desc.ul_نسخه = SECBUFFER_نسخه;
  type_1_desc.cBuffers  = 1;
  type_1_desc.pBuffers  = &type_1_buf;
  type_1_buf.بفر_Type = SECBUFFER_TOKEN;
  type_1_buf.pv_بفر   = ntlm->output_token;
  type_1_buf.cb_بفر   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-1 message */
  حالت = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials, عدم,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_شبکه_DREP,
                                                  عدم, 0,
                                                  ntlm->context, &type_1_desc,
                                                  &attrs, عدم);
  إذا(حالت == SEC_I_COMPLETE_NEEDED ||
     حالت == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(ntlm->context, &type_1_desc);
  وإلا إذا(حالت == SEC_E_INSUFFICIENT_حافظه)
    أرجع CURLE_بهر_د_حافظه;
  وإلا إذا(حالت != SEC_E_OK && حالت != SEC_I_CONTINUE_NEEDED)
    أرجع CURLE_AUTH_ERROR;

  /* Return the response. */
  Curl_bufref_سېټ(بهر, ntlm->output_token, type_1_buf.cb_بفر, عدم);
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
CURLcode Curl_auth_decode_ntlm_type2_پیغام(بنية Curl_easy *معلومات,
                                             ثابت بنية bufref *type2ref,
                                             بنية ntlmdata *ntlm)
{
  /* Ensure we have a valid type-2 message */
  إذا(!Curl_bufref_len(type2ref)) {
    infof(معلومات, "NTLM handshake failure (empty type-2 message)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Store the challenge for later use */
  ntlm->ننوت_token = curlx_memdup0(Curl_bufref_ptr(type2ref),
                                    Curl_bufref_len(type2ref));
  إذا(!ntlm->ننوت_token)
    أرجع CURLE_بهر_د_حافظه;
  ntlm->ننوت_token_len = Curl_bufref_len(type2ref);

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
CURLcode Curl_auth_create_ntlm_type3_پیغام(بنية Curl_easy *معلومات,
                                             بنية Curl_creds *creds,
                                             بنية ntlmdata *ntlm,
                                             بنية bufref *بهر)
{
  CURLcode پایله = CURLE_OK;
  Sec_بفر type_2_bufs[2];
  Sec_بفر type_3_buf;
  Sec_بفر_Desc type_2_desc;
  Sec_بفر_Desc type_3_desc;
  SECURITY_حالت حالت;
  بینښه اوږد attrs;
  SecPkgContext_Bindings pkgBindings = { 0, عدم };

  (فراغ)creds;

  /* Setup the type-2 "input" security buffer */
  type_2_desc.ul_نسخه     = SECBUFFER_نسخه;
  type_2_desc.cBuffers      = 1;
  type_2_desc.pBuffers      = &type_2_bufs[0];
  type_2_bufs[0].بفر_Type = SECBUFFER_TOKEN;
  type_2_bufs[0].pv_بفر   = ntlm->ننوت_token;
  type_2_bufs[0].cb_بفر   = curlx_uztoul(ntlm->ننوت_token_len);

  /* SSL context comes from schannel.
   * When extended protection is used in IIS server,
   * we have to pass a second SecBuffer to the SecBufferDesc
   * otherwise IIS does not pass the authentication (401 response).
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  إذا(ntlm->sslContext) {
    حالت = Curl_pSecFn->Query_Context_ځانګړتیاوې(
      ntlm->sslContext,
      SECPKG_ATTR_ENDPOINT_BINDINGS,
      &pkgBindings
    );
    إذا(حالت == SEC_E_OK) {
      type_2_desc.cBuffers++;
      type_2_bufs[1].بفر_Type = SECBUFFER_CHANNEL_BINDINGS;
      type_2_bufs[1].cb_بفر = pkgBindings.Bindings_اوږدتیا;
      type_2_bufs[1].pv_بفر = pkgBindings.Bindings;
    }
  }

  /* Setup the type-3 "output" security buffer */
  type_3_desc.ul_نسخه = SECBUFFER_نسخه;
  type_3_desc.cBuffers  = 1;
  type_3_desc.pBuffers  = &type_3_buf;
  type_3_buf.بفر_Type = SECBUFFER_TOKEN;
  type_3_buf.pv_بفر   = ntlm->output_token;
  type_3_buf.cb_بفر   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-3 message */
  حالت = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials,
                                                  ntlm->context,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_شبکه_DREP,
                                                  &type_2_desc,
                                                  0, ntlm->context,
                                                  &type_3_desc,
                                                  &attrs, عدم);

  إذا(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_بفر(pkgBindings.Bindings);

  إذا(حالت != SEC_E_OK) {
    infof(معلومات, "NTLM handshake failure (type-3 message): Status=0x%08lx",
          (بینښه اوږد)حالت);

    إذا(حالت == SEC_E_INSUFFICIENT_حافظه)
      أرجع CURLE_بهر_د_حافظه;

    أرجع CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  پایله = Curl_bufref_memdup0(بهر, ntlm->output_token, type_3_buf.cb_بفر);
  Curl_auth_cleanup_ntlm(ntlm);
  أرجع پایله;
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
  Curl_sspi_free_بې_اغېزه_عنصر(ntlm->p_بې_اغېزه_عنصر);
  ntlm->p_بې_اغېزه_عنصر = عدم;

  /* Free the input and output tokens */
  curlx_safefree(ntlm->ننوت_token);
  curlx_safefree(ntlm->output_token);

  /* Reset any variables */
  ntlm->token_max = 0;

  curlx_safefree(ntlm->spn);
}

#د_که_پای /* USE_WINDOWS_SSPI && USE_NTLM */
