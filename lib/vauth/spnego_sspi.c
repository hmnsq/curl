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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_WINDOWS_SSPI) && defined(USE_SPNEGO)

#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strerror.h"

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate is supported by Windows SSPI.
 */
bool Curl_auth_is_spnego_supported(ਖਾਲੀ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;

  /* Query the security package for Negotiate */
  ਸਥਿਤੀ = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(ਪਾਠ_2(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ਜੇ(ਸਥਿਤੀ == SEC_E_OK) {
    Curl_pSecFn->Free_Context_ਬਫਰ(SecurityPackage);
  }

  ਵਾਪਸ ਸਥਿਤੀ == SEC_E_OK;
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
CURLcode Curl_auth_decode_spnego_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ Curl_creds *creds,
                                         ਅਚਲ ਅੱਖਰ *default_service,
                                         ਅਚਲ ਅੱਖਰ *host,
                                         ਅਚਲ ਅੱਖਰ *chlg64,
                                         ਬਣਤਰ negotiatedata *nego)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t chlglen = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *chlg = ਨਿਰਾ;
  PSecPkgInfo SecurityPackage;
  Sec_ਬਫਰ chlg_buf[2];
  Sec_ਬਫਰ resp_buf;
  Sec_ਬਫਰ_Desc chlg_desc;
  Sec_ਬਫਰ_Desc resp_desc;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ attrs;
  SecPkgContext_Bindings pkgBindings = { 0, ਨਿਰਾ };

  ਜੇ(nego->context && nego->ਸਥਿਤੀ == SEC_E_OK) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    ਵਾਪਸ CURLE_LOGIN_DENIED;
  }

  ਜੇ(!nego->spn) {
    /* Generate our SPN */
    ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    nego->spn = Curl_auth_build_spn(service, host, ਨਿਰਾ);
    ਜੇ(!nego->spn)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(!nego->output_token) {
    /* Query the security package for Negotiate */
    nego->ਸਥਿਤੀ = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(ਪਾਠ_2(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);
    ਜੇ(nego->ਸਥਿਤੀ != SEC_E_OK) {
      failf(ਅੰਕੜੇ, "SSPI: could not get auth info");
      ਵਾਪਸ CURLE_AUTH_ERROR;
    }

    nego->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_ਬਫਰ(SecurityPackage);

    /* Allocate our output buffer */
    nego->output_token = curlx_malloc(nego->token_max);
    ਜੇ(!nego->output_token)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(!nego->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    ਜੇ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ਨਤੀਜਾ = Curl_create_sspi_ਪਛਾਣ_ਤੱਤ(creds->user, creds->passwd,
                                         &nego->ਪਛਾਣ_ਤੱਤ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      /* Allow proper cleanup of the identity structure */
      nego->p_ਪਛਾਣ_ਤੱਤ = &nego->ਪਛਾਣ_ਤੱਤ;
    }
    ਨਹੀਂਤਾਂ
      /* Use the current Windows user */
      nego->p_ਪਛਾਣ_ਤੱਤ = ਨਿਰਾ;

    /* Exclude NTLM from SPNEGO negotiation via the PackageList field */
    ਜੇ(!nego->p_ਪਛਾਣ_ਤੱਤ) {
      memset(&nego->ਪਛਾਣ_ਤੱਤ, 0, ਆਕਾਰ(nego->ਪਛਾਣ_ਤੱਤ));
      nego->ਪਛਾਣ_ਤੱਤ.ਸੰਸਕਰਣ = SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_ਸੰਸਕਰਣ;
      nego->ਪਛਾਣ_ਤੱਤ.ਲੰਮਾਈ_2 = ਆਕਾਰ(nego->ਪਛਾਣ_ਤੱਤ);
      nego->ਪਛਾਣ_ਤੱਤ.Flags = CURL_SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ;
      nego->p_ਪਛਾਣ_ਤੱਤ = &nego->ਪਛਾਣ_ਤੱਤ;
    }

    /* Use the special name "!ntlm" to prevent NTLM from being used:
     * https://learn.microsoft.com/windows/win32/api/sspi/ns-sspi-sec_winnt_auth_identity_exa
     */
    nego->ਪਛਾਣ_ਤੱਤ.PackageList = CURL_UNCONST(ਪਾਠ_2("!ntlm"));
    nego->ਪਛਾਣ_ਤੱਤ.Package_List_ਲੰਮਾਈ = 5;

    /* Allocate our credentials handle */
    nego->credentials = curlx_calloc(1, ਆਕਾਰ(CredHandle));
    ਜੇ(!nego->credentials)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    /* Acquire our credentials handle */
    nego->ਸਥਿਤੀ = Curl_pSecFn->AcquireCredentialsHandle(ਨਿਰਾ,
                                CURL_UNCONST(ਪਾਠ_2(SP_NAME_NEGOTIATE)),
                                SECPKG_CRED_OUTBOUND, ਨਿਰਾ,
                                nego->p_ਪਛਾਣ_ਤੱਤ, ਨਿਰਾ, ਨਿਰਾ,
                                nego->credentials, ਨਿਰਾ);
    ਜੇ(nego->ਸਥਿਤੀ != SEC_E_OK) {
      curlx_safefree(nego->credentials);
      ਵਾਪਸ CURLE_AUTH_ERROR;
    }

    /* Allocate our new context handle */
    nego->context = curlx_calloc(1, ਆਕਾਰ(CtxtHandle));
    ਜੇ(!nego->context)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  chlg_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;
  chlg_desc.cBuffers = 0;
  chlg_desc.pBuffers = chlg_buf;

  ਜੇ(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    ਜੇ(*chlg64 != '=') {
      ਨਤੀਜਾ = curlx_base64_decode(chlg64, &chlg, &chlglen);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    /* Ensure we have a valid challenge message */
    ਜੇ(!chlg) {
      infof(ਅੰਕੜੇ, "SPNEGO handshake failure (empty challenge message)");
      ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.cBuffers     = 1;
    chlg_buf[0].ਬਫਰ_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_ਬਫਰ   = chlg;
    chlg_buf[0].cb_ਬਫਰ   = curlx_uztoul(chlglen);
  }

  /* SSL context comes from Schannel.
   * When extended protection is used in IIS server, pass its channel
   * bindings on the initial call too. HTTP Negotiate can create and send a
   * Kerberos token before receiving a challenge from the server.
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  ਜੇ(nego->sslContext) {
    nego->ਸਥਿਤੀ = Curl_pSecFn->Query_Context_ਗੁਣ(
        nego->sslContext,
        SECPKG_ATTR_ENDPOINT_BINDINGS,
        &pkgBindings);
    ਜੇ(nego->ਸਥਿਤੀ == SEC_E_OK) {
      Sec_ਬਫਰ *binding_buf = &chlg_buf[chlg_desc.cBuffers++];
      binding_buf->ਬਫਰ_Type = SECBUFFER_CHANNEL_BINDINGS;
      binding_buf->cb_ਬਫਰ   = pkgBindings.Bindings_ਲੰਮਾਈ;
      binding_buf->pv_ਬਫਰ   = pkgBindings.Bindings;
    }
  }

  /* Setup the response "output" security buffer */
  resp_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.ਬਫਰ_Type = SECBUFFER_TOKEN;
  resp_buf.pv_ਬਫਰ   = nego->output_token;
  resp_buf.cb_ਬਫਰ   = curlx_uztoul(nego->token_max);

  /* Generate our challenge-response message */
  {
    DWORD sspi_flags = ISC_REQ_CONFIDENTIALITY;
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
      sspi_flags |= ISC_REQ_DELEGATE | ISC_REQ_MUTUAL_AUTH;
    nego->ਸਥਿਤੀ =
      Curl_pSecFn->InitializeSecurityContext(nego->credentials,
                                             chlg ? nego->context : ਨਿਰਾ,
                                             nego->spn,
                                             sspi_flags,
                                             0, SECURITY_NATIVE_DREP,
                                             chlg_desc.cBuffers ?
                                               &chlg_desc : ਨਿਰਾ,
                                             0, nego->context,
                                             &resp_desc, &attrs, ਨਿਰਾ);
  }

  ਜੇ(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_ਬਫਰ(pkgBindings.Bindings);

  /* Free the decoded challenge as it is not required anymore */
  curlx_free(chlg);

  ਜੇ(GSS_ERROR(nego->ਸਥਿਤੀ)) {
    ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
    failf(ਅੰਕੜੇ, "InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(nego->ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));

    ਜੇ(nego->ਸਥਿਤੀ == SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  ਜੇ(nego->ਸਥਿਤੀ == SEC_I_COMPLETE_NEEDED ||
     nego->ਸਥਿਤੀ == SEC_I_COMPLETE_AND_CONTINUE) {
    nego->ਸਥਿਤੀ = Curl_pSecFn->CompleteAuthToken(nego->context, &resp_desc);
    ਜੇ(GSS_ERROR(nego->ਸਥਿਤੀ)) {
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      failf(ਅੰਕੜੇ, "CompleteAuthToken failed: %s",
            Curl_sspi_strerror(nego->ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));

      ਜੇ(nego->ਸਥਿਤੀ == SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      ਵਾਪਸ CURLE_AUTH_ERROR;
    }
  }

  nego->output_token_ਲੰਮਾਈ = resp_buf.cb_ਬਫਰ;

  ਵਾਪਸ ਨਤੀਜਾ;
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
CURLcode Curl_auth_create_spnego_ਸੁਨੇਹਾ(ਬਣਤਰ negotiatedata *nego,
                                         ਅੱਖਰ **outptr, size_t *outlen)
{
  /* Base64 encode the already generated response */
  CURLcode ਨਤੀਜਾ = curlx_base64_encode(nego->output_token,
                                        nego->output_token_ਲੰਮਾਈ, outptr,
                                        outlen);
  ਜੇ(!ਨਤੀਜਾ && (!*outptr || !*outlen)) {
    curlx_free(*outptr);
    ਨਤੀਜਾ = CURLE_REMOTE_ACCESS_DENIED;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਖਾਲੀ Curl_auth_cleanup_spnego(ਬਣਤਰ negotiatedata *nego)
{
  /* Free our security context */
  ਜੇ(nego->context) {
    Curl_pSecFn->DeleteSecurityContext(nego->context);
    curlx_safefree(nego->context);
  }

  /* Free our credentials handle */
  ਜੇ(nego->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(nego->credentials);
    curlx_safefree(nego->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(nego->p_ਪਛਾਣ_ਤੱਤ);
  nego->p_ਪਛਾਣ_ਤੱਤ = ਨਿਰਾ;

  /* Free the SPN and output token */
  curlx_safefree(nego->spn);
  curlx_safefree(nego->output_token);

  /* Reset any variables */
  nego->ਸਥਿਤੀ = 0;
  nego->token_max = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#ਜੇ_ਅੰਤ /* USE_WINDOWS_SSPI && USE_SPNEGO */
