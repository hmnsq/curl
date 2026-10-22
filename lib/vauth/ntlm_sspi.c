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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ defined(USE_WINDOWS_SSPI) && defined(USE_NTLM)

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "curl_ntlm_core.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"

/*
 * Curl_auth_is_ntlm_supported()
 *
 * This is used to evaluate if NTLM is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if NTLM is supported by Windows SSPI.
 */
bool Curl_auth_is_ntlm_supported(ಶೂನ್ಯ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;

  /* Query the security package for NTLM */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_NTLM)),
                                     &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_OK) {
    Curl_pSecFn->Free_Context_ಬಫರ್(SecurityPackage);
  }

  ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2 == SEC_E_OK;
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
CURLcode Curl_auth_create_ntlm_type1_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_creds *creds,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                             ರಚನೆ ntlmdata *ntlm,
                                             ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  PSecPkgInfo SecurityPackage;
  Sec_ಬಫರ್ type_1_buf;
  Sec_ಬಫರ್_Desc type_1_desc;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ attrs;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_cleanup_ntlm(ntlm);

  /* Query the security package for NTLM */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_NTLM)),
                                     &SecurityPackage);
  ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK) {
    failf(ದತ್ತಾಂಶ, "SSPI: could not get auth info");
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  ntlm->token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_ಬಫರ್(SecurityPackage);

  /* Allocate our output buffer */
  ntlm->output_token = curlx_malloc(ntlm->token_max);
  ಆದರೆ(!ntlm->output_token)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(Curl_creds_has_user(creds)) {
    CURLcode ಫಲಿತಾಂಶ;

    /* Populate our identity structure */
    ಫಲಿತಾಂಶ = Curl_create_sspi_ತಟಸ್ಥ(
      creds->user, creds->passwd, &ntlm->ತಟಸ್ಥ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    /* Allow proper cleanup of the identity structure */
    ntlm->p_ತಟಸ್ಥ = &ntlm->ತಟಸ್ಥ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* Use the current Windows user */
    ntlm->p_ತಟಸ್ಥ = ಖಾಲಿ;

  /* Allocate our credentials handle */
  ntlm->credentials = curlx_calloc(1, ಗಾತ್ರ(CredHandle));
  ಆದರೆ(!ntlm->credentials)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Acquire our credentials handle */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->AcquireCredentialsHandle(ಖಾಲಿ,
                                     CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_NTLM)),
                                     SECPKG_CRED_OUTBOUND, ಖಾಲಿ,
                                     ntlm->p_ತಟಸ್ಥ, ಖಾಲಿ, ಖಾಲಿ,
                                     ntlm->credentials, ಖಾಲಿ);
  ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK) {
    curlx_safefree(ntlm->credentials);
    ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
  }

  /* Allocate our new context handle */
  ntlm->context = curlx_calloc(1, ಗಾತ್ರ(CtxtHandle));
  ಆದರೆ(!ntlm->context)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ntlm->spn = Curl_auth_build_spn(service, host, ಖಾಲಿ);
  ಆದರೆ(!ntlm->spn)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Setup the type-1 "output" security buffer */
  type_1_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;
  type_1_desc.cBuffers  = 1;
  type_1_desc.pBuffers  = &type_1_buf;
  type_1_buf.ಬಫರ್_Type = SECBUFFER_TOKEN;
  type_1_buf.pv_ಬಫರ್   = ntlm->output_token;
  type_1_buf.cb_ಬಫರ್   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-1 message */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials, ಖಾಲಿ,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_ಜಾಲ_DREP,
                                                  ಖಾಲಿ, 0,
                                                  ntlm->context, &type_1_desc,
                                                  &attrs, ಖಾಲಿ);
  ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_I_COMPLETE_NEEDED ||
     ಸ್ಥಿತಿ_2 == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(ntlm->context, &type_1_desc);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK && ಸ್ಥಿತಿ_2 != SEC_I_CONTINUE_NEEDED)
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;

  /* Return the response. */
  Curl_bufref_ಗಣ(ಹೊರಹೋಗು, ntlm->output_token, type_1_buf.cb_ಬಫರ್, ಖಾಲಿ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
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
CURLcode Curl_auth_decode_ntlm_type2_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *type2ref,
                                             ರಚನೆ ntlmdata *ntlm)
{
  /* Ensure we have a valid type-2 message */
  ಆದರೆ(!Curl_bufref_len(type2ref)) {
    infof(ದತ್ತಾಂಶ, "NTLM handshake failure (empty type-2 message)");
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Store the challenge for later use */
  ntlm->ಒಳಹರಿವು_token = curlx_memdup0(Curl_bufref_ptr(type2ref),
                                    Curl_bufref_len(type2ref));
  ಆದರೆ(!ntlm->ಒಳಹರಿವು_token)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ntlm->ಒಳಹರಿವು_token_len = Curl_bufref_len(type2ref);

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_creds *creds,
                                             ರಚನೆ ntlmdata *ntlm,
                                             ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  Sec_ಬಫರ್ type_2_bufs[2];
  Sec_ಬಫರ್ type_3_buf;
  Sec_ಬಫರ್_Desc type_2_desc;
  Sec_ಬಫರ್_Desc type_3_desc;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ attrs;
  SecPkgContext_Bindings pkgBindings = { 0, ಖಾಲಿ };

  (ಶೂನ್ಯ)creds;

  /* Setup the type-2 "input" security buffer */
  type_2_desc.ul_ಆವೃತ್ತಿ     = SECBUFFER_ಆವೃತ್ತಿ;
  type_2_desc.cBuffers      = 1;
  type_2_desc.pBuffers      = &type_2_bufs[0];
  type_2_bufs[0].ಬಫರ್_Type = SECBUFFER_TOKEN;
  type_2_bufs[0].pv_ಬಫರ್   = ntlm->ಒಳಹರಿವು_token;
  type_2_bufs[0].cb_ಬಫರ್   = curlx_uztoul(ntlm->ಒಳಹರಿವು_token_len);

  /* SSL context comes from schannel.
   * When extended protection is used in IIS server,
   * we have to pass a second SecBuffer to the SecBufferDesc
   * otherwise IIS does not pass the authentication (401 response).
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  ಆದರೆ(ntlm->sslContext) {
    ಸ್ಥಿತಿ_2 = Curl_pSecFn->Query_Context_ಗುಣಲಕ್ಷಣಗಳು(
      ntlm->sslContext,
      SECPKG_ATTR_ENDPOINT_BINDINGS,
      &pkgBindings
    );
    ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_OK) {
      type_2_desc.cBuffers++;
      type_2_bufs[1].ಬಫರ್_Type = SECBUFFER_CHANNEL_BINDINGS;
      type_2_bufs[1].cb_ಬಫರ್ = pkgBindings.Bindings_ಉದ್ದಳತೆ;
      type_2_bufs[1].pv_ಬಫರ್ = pkgBindings.Bindings;
    }
  }

  /* Setup the type-3 "output" security buffer */
  type_3_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;
  type_3_desc.cBuffers  = 1;
  type_3_desc.pBuffers  = &type_3_buf;
  type_3_buf.ಬಫರ್_Type = SECBUFFER_TOKEN;
  type_3_buf.pv_ಬಫರ್   = ntlm->output_token;
  type_3_buf.cb_ಬಫರ್   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-3 message */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials,
                                                  ntlm->context,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_ಜಾಲ_DREP,
                                                  &type_2_desc,
                                                  0, ntlm->context,
                                                  &type_3_desc,
                                                  &attrs, ಖಾಲಿ);

  ಆದರೆ(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_ಬಫರ್(pkgBindings.Bindings);

  ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK) {
    infof(ದತ್ತಾಂಶ, "NTLM handshake failure (type-3 message): Status=0x%08lx",
          (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ಸ್ಥಿತಿ_2);

    ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  ಫಲಿತಾಂಶ = Curl_bufref_memdup0(ಹೊರಹೋಗು, ntlm->output_token, type_3_buf.cb_ಬಫರ್);
  Curl_auth_cleanup_ntlm(ntlm);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಶೂನ್ಯ Curl_auth_cleanup_ntlm(ರಚನೆ ntlmdata *ntlm)
{
  /* Free our security context */
  ಆದರೆ(ntlm->context) {
    Curl_pSecFn->DeleteSecurityContext(ntlm->context);
    curlx_safefree(ntlm->context);
  }

  /* Free our credentials handle */
  ಆದರೆ(ntlm->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(ntlm->credentials);
    curlx_safefree(ntlm->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_ತಟಸ್ಥ(ntlm->p_ತಟಸ್ಥ);
  ntlm->p_ತಟಸ್ಥ = ಖಾಲಿ;

  /* Free the input and output tokens */
  curlx_safefree(ntlm->ಒಳಹರಿವು_token);
  curlx_safefree(ntlm->output_token);

  /* Reset any variables */
  ntlm->token_max = 0;

  curlx_safefree(ntlm->spn);
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_WINDOWS_SSPI && USE_NTLM */
