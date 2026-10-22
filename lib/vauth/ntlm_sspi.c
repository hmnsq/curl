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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_WINDOWS_SSPI) && defined(USE_NTLM)

#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ntlm_core.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"

/*
 * Curl_auth_is_ntlm_supported()
 *
 * This is used to evaluate if NTLM is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if NTLM is supported by Windows SSPI.
 */
bool Curl_auth_is_ntlm_supported(ਖਾਲੀ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;

  /* Query the security package for NTLM */
  ਸਥਿਤੀ = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(ਪਾਠ_2(SP_NAME_NTLM)),
                                     &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ਜੇ(ਸਥਿਤੀ == SEC_E_OK) {
    Curl_pSecFn->Free_Context_ਬਫਰ(SecurityPackage);
  }

  ਵਾਪਸ ਸਥਿਤੀ == SEC_E_OK;
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
CURLcode Curl_auth_create_ntlm_type1_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_creds *creds,
                                             ਅਚਲ ਅੱਖਰ *default_service,
                                             ਅਚਲ ਅੱਖਰ *host,
                                             ਬਣਤਰ ntlmdata *ntlm,
                                             ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  PSecPkgInfo SecurityPackage;
  Sec_ਬਫਰ type_1_buf;
  Sec_ਬਫਰ_Desc type_1_desc;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ attrs;
  ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_cleanup_ntlm(ntlm);

  /* Query the security package for NTLM */
  ਸਥਿਤੀ = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(ਪਾਠ_2(SP_NAME_NTLM)),
                                     &SecurityPackage);
  ਜੇ(ਸਥਿਤੀ != SEC_E_OK) {
    failf(ਅੰਕੜੇ, "SSPI: could not get auth info");
    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  ntlm->token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_ਬਫਰ(SecurityPackage);

  /* Allocate our output buffer */
  ntlm->output_token = curlx_malloc(ntlm->token_max);
  ਜੇ(!ntlm->output_token)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(Curl_creds_has_user(creds)) {
    CURLcode ਨਤੀਜਾ;

    /* Populate our identity structure */
    ਨਤੀਜਾ = Curl_create_sspi_ਪਛਾਣ_ਤੱਤ(
      creds->user, creds->passwd, &ntlm->ਪਛਾਣ_ਤੱਤ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    /* Allow proper cleanup of the identity structure */
    ntlm->p_ਪਛਾਣ_ਤੱਤ = &ntlm->ਪਛਾਣ_ਤੱਤ;
  }
  ਨਹੀਂਤਾਂ
    /* Use the current Windows user */
    ntlm->p_ਪਛਾਣ_ਤੱਤ = ਨਿਰਾ;

  /* Allocate our credentials handle */
  ntlm->credentials = curlx_calloc(1, ਆਕਾਰ(CredHandle));
  ਜੇ(!ntlm->credentials)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Acquire our credentials handle */
  ਸਥਿਤੀ = Curl_pSecFn->AcquireCredentialsHandle(ਨਿਰਾ,
                                     CURL_UNCONST(ਪਾਠ_2(SP_NAME_NTLM)),
                                     SECPKG_CRED_OUTBOUND, ਨਿਰਾ,
                                     ntlm->p_ਪਛਾਣ_ਤੱਤ, ਨਿਰਾ, ਨਿਰਾ,
                                     ntlm->credentials, ਨਿਰਾ);
  ਜੇ(ਸਥਿਤੀ != SEC_E_OK) {
    curlx_safefree(ntlm->credentials);
    ਵਾਪਸ CURLE_LOGIN_DENIED;
  }

  /* Allocate our new context handle */
  ntlm->context = curlx_calloc(1, ਆਕਾਰ(CtxtHandle));
  ਜੇ(!ntlm->context)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ntlm->spn = Curl_auth_build_spn(service, host, ਨਿਰਾ);
  ਜੇ(!ntlm->spn)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Setup the type-1 "output" security buffer */
  type_1_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;
  type_1_desc.cBuffers  = 1;
  type_1_desc.pBuffers  = &type_1_buf;
  type_1_buf.ਬਫਰ_Type = SECBUFFER_TOKEN;
  type_1_buf.pv_ਬਫਰ   = ntlm->output_token;
  type_1_buf.cb_ਬਫਰ   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-1 message */
  ਸਥਿਤੀ = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials, ਨਿਰਾ,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_ਜਾਲ_DREP,
                                                  ਨਿਰਾ, 0,
                                                  ntlm->context, &type_1_desc,
                                                  &attrs, ਨਿਰਾ);
  ਜੇ(ਸਥਿਤੀ == SEC_I_COMPLETE_NEEDED ||
     ਸਥਿਤੀ == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(ntlm->context, &type_1_desc);
  ਨਹੀਂਤਾਂ ਜੇ(ਸਥਿਤੀ == SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਨਹੀਂਤਾਂ ਜੇ(ਸਥਿਤੀ != SEC_E_OK && ਸਥਿਤੀ != SEC_I_CONTINUE_NEEDED)
    ਵਾਪਸ CURLE_AUTH_ERROR;

  /* Return the response. */
  Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, ntlm->output_token, type_1_buf.cb_ਬਫਰ, ਨਿਰਾ);
  ਵਾਪਸ CURLE_OK;
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
CURLcode Curl_auth_decode_ntlm_type2_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਅਚਲ ਬਣਤਰ bufref *type2ref,
                                             ਬਣਤਰ ntlmdata *ntlm)
{
  /* Ensure we have a valid type-2 message */
  ਜੇ(!Curl_bufref_len(type2ref)) {
    infof(ਅੰਕੜੇ, "NTLM handshake failure (empty type-2 message)");
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Store the challenge for later use */
  ntlm->ਨਿਵੇਸ਼_token = curlx_memdup0(Curl_bufref_ptr(type2ref),
                                    Curl_bufref_len(type2ref));
  ਜੇ(!ntlm->ਨਿਵੇਸ਼_token)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ntlm->ਨਿਵੇਸ਼_token_len = Curl_bufref_len(type2ref);

  ਵਾਪਸ CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_creds *creds,
                                             ਬਣਤਰ ntlmdata *ntlm,
                                             ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  Sec_ਬਫਰ type_2_bufs[2];
  Sec_ਬਫਰ type_3_buf;
  Sec_ਬਫਰ_Desc type_2_desc;
  Sec_ਬਫਰ_Desc type_3_desc;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ attrs;
  SecPkgContext_Bindings pkgBindings = { 0, ਨਿਰਾ };

  (ਖਾਲੀ)creds;

  /* Setup the type-2 "input" security buffer */
  type_2_desc.ul_ਸੰਸਕਰਣ     = SECBUFFER_ਸੰਸਕਰਣ;
  type_2_desc.cBuffers      = 1;
  type_2_desc.pBuffers      = &type_2_bufs[0];
  type_2_bufs[0].ਬਫਰ_Type = SECBUFFER_TOKEN;
  type_2_bufs[0].pv_ਬਫਰ   = ntlm->ਨਿਵੇਸ਼_token;
  type_2_bufs[0].cb_ਬਫਰ   = curlx_uztoul(ntlm->ਨਿਵੇਸ਼_token_len);

  /* SSL context comes from schannel.
   * When extended protection is used in IIS server,
   * we have to pass a second SecBuffer to the SecBufferDesc
   * otherwise IIS does not pass the authentication (401 response).
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  ਜੇ(ntlm->sslContext) {
    ਸਥਿਤੀ = Curl_pSecFn->Query_Context_ਗੁਣ(
      ntlm->sslContext,
      SECPKG_ATTR_ENDPOINT_BINDINGS,
      &pkgBindings
    );
    ਜੇ(ਸਥਿਤੀ == SEC_E_OK) {
      type_2_desc.cBuffers++;
      type_2_bufs[1].ਬਫਰ_Type = SECBUFFER_CHANNEL_BINDINGS;
      type_2_bufs[1].cb_ਬਫਰ = pkgBindings.Bindings_ਲੰਮਾਈ;
      type_2_bufs[1].pv_ਬਫਰ = pkgBindings.Bindings;
    }
  }

  /* Setup the type-3 "output" security buffer */
  type_3_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;
  type_3_desc.cBuffers  = 1;
  type_3_desc.pBuffers  = &type_3_buf;
  type_3_buf.ਬਫਰ_Type = SECBUFFER_TOKEN;
  type_3_buf.pv_ਬਫਰ   = ntlm->output_token;
  type_3_buf.cb_ਬਫਰ   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-3 message */
  ਸਥਿਤੀ = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials,
                                                  ntlm->context,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_ਜਾਲ_DREP,
                                                  &type_2_desc,
                                                  0, ntlm->context,
                                                  &type_3_desc,
                                                  &attrs, ਨਿਰਾ);

  ਜੇ(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_ਬਫਰ(pkgBindings.Bindings);

  ਜੇ(ਸਥਿਤੀ != SEC_E_OK) {
    infof(ਅੰਕੜੇ, "NTLM handshake failure (type-3 message): Status=0x%08lx",
          (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਸਥਿਤੀ);

    ਜੇ(ਸਥਿਤੀ == SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  ਨਤੀਜਾ = Curl_bufref_memdup0(ਬਾਹਰਲਾ, ntlm->output_token, type_3_buf.cb_ਬਫਰ);
  Curl_auth_cleanup_ntlm(ntlm);
  ਵਾਪਸ ਨਤੀਜਾ;
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
ਖਾਲੀ Curl_auth_cleanup_ntlm(ਬਣਤਰ ntlmdata *ntlm)
{
  /* Free our security context */
  ਜੇ(ntlm->context) {
    Curl_pSecFn->DeleteSecurityContext(ntlm->context);
    curlx_safefree(ntlm->context);
  }

  /* Free our credentials handle */
  ਜੇ(ntlm->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(ntlm->credentials);
    curlx_safefree(ntlm->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(ntlm->p_ਪਛਾਣ_ਤੱਤ);
  ntlm->p_ਪਛਾਣ_ਤੱਤ = ਨਿਰਾ;

  /* Free the input and output tokens */
  curlx_safefree(ntlm->ਨਿਵੇਸ਼_token);
  curlx_safefree(ntlm->output_token);

  /* Reset any variables */
  ntlm->token_max = 0;

  curlx_safefree(ntlm->spn);
}

#ਜੇ_ਅੰਤ /* USE_WINDOWS_SSPI && USE_NTLM */
