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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ defined(USE_WINDOWS_SSPI) && defined(USE_SPNEGO)

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "strerror.h"

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate is supported by Windows SSPI.
 */
bool Curl_auth_is_spnego_supported(ಶೂನ್ಯ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;

  /* Query the security package for Negotiate */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_OK) {
    Curl_pSecFn->Free_Context_ಬಫರ್(SecurityPackage);
  }

  ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2 == SEC_E_OK;
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
 * user        [in]     - The username in the format User or Domain\User.
 * password    [in]     - The user's password.
 * service     [in]     - The service type such as http, smtp, pop or imap.
 * host        [in]     - The hostname.
 * chlg64      [in]     - The optional base64 encoded challenge message.
 * nego        [in/out] - The Negotiate data struct being used and modified.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_decode_spnego_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ Curl_creds *creds,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chlg64,
                                         ರಚನೆ negotiatedata *nego)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t chlglen = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *chlg = ಖಾಲಿ;
  PSecPkgInfo SecurityPackage;
  Sec_ಬಫರ್ chlg_buf[2];
  Sec_ಬಫರ್ resp_buf;
  Sec_ಬಫರ್_Desc chlg_desc;
  Sec_ಬಫರ್_Desc resp_desc;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ attrs;
  SecPkgContext_Bindings pkgBindings = { 0, ಖಾಲಿ };

  ಆದರೆ(nego->context && nego->ಸ್ಥಿತಿ_2 == SEC_E_OK) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
  }

  ಆದರೆ(!nego->spn) {
    /* Generate our SPN */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    nego->spn = Curl_auth_build_spn(service, host, ಖಾಲಿ);
    ಆದರೆ(!nego->spn)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(!nego->output_token) {
    /* Query the security package for Negotiate */
    nego->ಸ್ಥಿತಿ_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);
    ಆದರೆ(nego->ಸ್ಥಿತಿ_2 != SEC_E_OK) {
      failf(ದತ್ತಾಂಶ, "SSPI: could not get auth info");
      ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
    }

    nego->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_ಬಫರ್(SecurityPackage);

    /* Allocate our output buffer */
    nego->output_token = curlx_malloc(nego->token_max);
    ಆದರೆ(!nego->output_token)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(!nego->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    ಆದರೆ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ಫಲಿತಾಂಶ = Curl_create_sspi_ತಟಸ್ಥ(creds->user, creds->passwd,
                                         &nego->ತಟಸ್ಥ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      /* Allow proper cleanup of the identity structure */
      nego->p_ತಟಸ್ಥ = &nego->ತಟಸ್ಥ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* Use the current Windows user */
      nego->p_ತಟಸ್ಥ = ಖಾಲಿ;

    /* Exclude NTLM from SPNEGO negotiation via the PackageList field */
    ಆದರೆ(!nego->p_ತಟಸ್ಥ) {
      memset(&nego->ತಟಸ್ಥ, 0, ಗಾತ್ರ(nego->ತಟಸ್ಥ));
      nego->ತಟಸ್ಥ.ಆವೃತ್ತಿ = SEC_WINNT_AUTH_ತಟಸ್ಥ_ಆವೃತ್ತಿ;
      nego->ತಟಸ್ಥ.ಉದ್ದಳತೆ_2 = ಗಾತ್ರ(nego->ತಟಸ್ಥ);
      nego->ತಟಸ್ಥ.Flags = CURL_SEC_WINNT_AUTH_ತಟಸ್ಥ;
      nego->p_ತಟಸ್ಥ = &nego->ತಟಸ್ಥ;
    }

    /* Use the special name "!ntlm" to prevent NTLM from being used:
     * https://learn.microsoft.com/windows/win32/api/sspi/ns-sspi-sec_winnt_auth_identity_exa
     */
    nego->ತಟಸ್ಥ.PackageList = CURL_UNCONST(ಪಠ್ಯ_2("!ntlm"));
    nego->ತಟಸ್ಥ.Package_List_ಉದ್ದಳತೆ = 5;

    /* Allocate our credentials handle */
    nego->credentials = curlx_calloc(1, ಗಾತ್ರ(CredHandle));
    ಆದರೆ(!nego->credentials)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    /* Acquire our credentials handle */
    nego->ಸ್ಥಿತಿ_2 = Curl_pSecFn->AcquireCredentialsHandle(ಖಾಲಿ,
                                CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_NEGOTIATE)),
                                SECPKG_CRED_OUTBOUND, ಖಾಲಿ,
                                nego->p_ತಟಸ್ಥ, ಖಾಲಿ, ಖಾಲಿ,
                                nego->credentials, ಖಾಲಿ);
    ಆದರೆ(nego->ಸ್ಥಿತಿ_2 != SEC_E_OK) {
      curlx_safefree(nego->credentials);
      ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
    }

    /* Allocate our new context handle */
    nego->context = curlx_calloc(1, ಗಾತ್ರ(CtxtHandle));
    ಆದರೆ(!nego->context)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  chlg_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;
  chlg_desc.cBuffers = 0;
  chlg_desc.pBuffers = chlg_buf;

  ಆದರೆ(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    ಆದರೆ(*chlg64 != '=') {
      ಫಲಿತಾಂಶ = curlx_base64_decode(chlg64, &chlg, &chlglen);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    /* Ensure we have a valid challenge message */
    ಆದರೆ(!chlg) {
      infof(ದತ್ತಾಂಶ, "SPNEGO handshake failure (empty challenge message)");
      ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.cBuffers     = 1;
    chlg_buf[0].ಬಫರ್_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_ಬಫರ್   = chlg;
    chlg_buf[0].cb_ಬಫರ್   = curlx_uztoul(chlglen);
  }

  /* SSL context comes from Schannel.
   * When extended protection is used in IIS server, pass its channel
   * bindings on the initial call too. HTTP Negotiate can create and send a
   * Kerberos token before receiving a challenge from the server.
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  ಆದರೆ(nego->sslContext) {
    nego->ಸ್ಥಿತಿ_2 = Curl_pSecFn->Query_Context_ಗುಣಲಕ್ಷಣಗಳು(
        nego->sslContext,
        SECPKG_ATTR_ENDPOINT_BINDINGS,
        &pkgBindings);
    ಆದರೆ(nego->ಸ್ಥಿತಿ_2 == SEC_E_OK) {
      Sec_ಬಫರ್ *binding_buf = &chlg_buf[chlg_desc.cBuffers++];
      binding_buf->ಬಫರ್_Type = SECBUFFER_CHANNEL_BINDINGS;
      binding_buf->cb_ಬಫರ್   = pkgBindings.Bindings_ಉದ್ದಳತೆ;
      binding_buf->pv_ಬಫರ್   = pkgBindings.Bindings;
    }
  }

  /* Setup the response "output" security buffer */
  resp_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.ಬಫರ್_Type = SECBUFFER_TOKEN;
  resp_buf.pv_ಬಫರ್   = nego->output_token;
  resp_buf.cb_ಬಫರ್   = curlx_uztoul(nego->token_max);

  /* Generate our challenge-response message */
  {
    DWORD sspi_flags = ISC_REQ_CONFIDENTIALITY;
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
      sspi_flags |= ISC_REQ_DELEGATE | ISC_REQ_MUTUAL_AUTH;
    nego->ಸ್ಥಿತಿ_2 =
      Curl_pSecFn->InitializeSecurityContext(nego->credentials,
                                             chlg ? nego->context : ಖಾಲಿ,
                                             nego->spn,
                                             sspi_flags,
                                             0, SECURITY_NATIVE_DREP,
                                             chlg_desc.cBuffers ?
                                               &chlg_desc : ಖಾಲಿ,
                                             0, nego->context,
                                             &resp_desc, &attrs, ಖಾಲಿ);
  }

  ಆದರೆ(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_ಬಫರ್(pkgBindings.Bindings);

  /* Free the decoded challenge as it is not required anymore */
  curlx_free(chlg);

  ಆದರೆ(GSS_ERROR(nego->ಸ್ಥಿತಿ_2)) {
    ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
    failf(ದತ್ತಾಂಶ, "InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(nego->ಸ್ಥಿತಿ_2, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));

    ಆದರೆ(nego->ಸ್ಥಿತಿ_2 == SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  ಆದರೆ(nego->ಸ್ಥಿತಿ_2 == SEC_I_COMPLETE_NEEDED ||
     nego->ಸ್ಥಿತಿ_2 == SEC_I_COMPLETE_AND_CONTINUE) {
    nego->ಸ್ಥಿತಿ_2 = Curl_pSecFn->CompleteAuthToken(nego->context, &resp_desc);
    ಆದರೆ(GSS_ERROR(nego->ಸ್ಥಿತಿ_2)) {
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      failf(ದತ್ತಾಂಶ, "CompleteAuthToken failed: %s",
            Curl_sspi_strerror(nego->ಸ್ಥಿತಿ_2, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));

      ಆದರೆ(nego->ಸ್ಥಿತಿ_2 == SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
    }
  }

  nego->output_token_ಉದ್ದಳತೆ = resp_buf.cb_ಬಫರ್;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
CURLcode Curl_auth_create_spnego_ಸಂದೇಶ(ರಚನೆ negotiatedata *nego,
                                         ಅಕ್ಷರ **outptr, size_t *outlen)
{
  /* Base64 encode the already generated response */
  CURLcode ಫಲಿತಾಂಶ = curlx_base64_encode(nego->output_token,
                                        nego->output_token_ಉದ್ದಳತೆ, outptr,
                                        outlen);
  ಆದರೆ(!ಫಲಿತಾಂಶ && (!*outptr || !*outlen)) {
    curlx_free(*outptr);
    ಫಲಿತಾಂಶ = CURLE_REMOTE_ACCESS_DENIED;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಶೂನ್ಯ Curl_auth_cleanup_spnego(ರಚನೆ negotiatedata *nego)
{
  /* Free our security context */
  ಆದರೆ(nego->context) {
    Curl_pSecFn->DeleteSecurityContext(nego->context);
    curlx_safefree(nego->context);
  }

  /* Free our credentials handle */
  ಆದರೆ(nego->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(nego->credentials);
    curlx_safefree(nego->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_ತಟಸ್ಥ(nego->p_ತಟಸ್ಥ);
  nego->p_ತಟಸ್ಥ = ಖಾಲಿ;

  /* Free the SPN and output token */
  curlx_safefree(nego->spn);
  curlx_safefree(nego->output_token);

  /* Reset any variables */
  nego->ಸ್ಥಿತಿ_2 = 0;
  nego->token_max = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_WINDOWS_SSPI && USE_SPNEGO */
