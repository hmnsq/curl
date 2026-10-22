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

#إذا defined(USE_WINDOWS_SSPI) && defined(USE_SPNEGO)

#ضمن "vauth/vauth.h"
#ضمن "curlx/base64.h"
#ضمن "curl_trc.h"
#ضمن "strerror.h"

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate is supported by Windows SSPI.
 */
bool Curl_auth_is_spnego_supported(فراغ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_حالت حالت;

  /* Query the security package for Negotiate */
  حالت = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(متن_2(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  إذا(حالت == SEC_E_OK) {
    Curl_pSecFn->Free_Context_بفر(SecurityPackage);
  }

  أرجع حالت == SEC_E_OK;
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
CURLcode Curl_auth_decode_spnego_سنیہا(بنية Curl_easy *انکڑے,
                                         بنية Curl_creds *creds,
                                         ثابت حرف *default_service,
                                         ثابت حرف *host,
                                         ثابت حرف *chlg64,
                                         بنية negotiatedata *nego)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t chlglen = 0;
  بنانشانی حرف *chlg = عدم;
  PSecPkgInfo SecurityPackage;
  Sec_بفر chlg_buf[2];
  Sec_بفر resp_buf;
  Sec_بفر_Desc chlg_desc;
  Sec_بفر_Desc resp_desc;
  بنانشانی لما attrs;
  SecPkgContext_Bindings pkgBindings = { 0, عدم };

  إذا(nego->context && nego->حالت == SEC_E_OK) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    أرجع CURLE_LOGIN_DENIED;
  }

  إذا(!nego->spn) {
    /* Generate our SPN */
    ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    nego->spn = Curl_auth_build_spn(service, host, عدم);
    إذا(!nego->spn)
      أرجع CURLE_باہرلا_دا_حافظہ;
  }

  إذا(!nego->output_token) {
    /* Query the security package for Negotiate */
    nego->حالت = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(متن_2(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);
    إذا(nego->حالت != SEC_E_OK) {
      failf(انکڑے, "SSPI: could not get auth info");
      أرجع CURLE_AUTH_ERROR;
    }

    nego->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_بفر(SecurityPackage);

    /* Allocate our output buffer */
    nego->output_token = curlx_malloc(nego->token_max);
    إذا(!nego->output_token)
      أرجع CURLE_باہرلا_دا_حافظہ;
  }

  إذا(!nego->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    إذا(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      نتیجہ = Curl_create_sspi_پچھان_تت(creds->user, creds->passwd,
                                         &nego->پچھان_تت);
      إذا(نتیجہ)
        أرجع نتیجہ;

      /* Allow proper cleanup of the identity structure */
      nego->p_پچھان_تت = &nego->پچھان_تت;
    }
    وإلا
      /* Use the current Windows user */
      nego->p_پچھان_تت = عدم;

    /* Exclude NTLM from SPNEGO negotiation via the PackageList field */
    إذا(!nego->p_پچھان_تت) {
      memset(&nego->پچھان_تت, 0, حجم(nego->پچھان_تت));
      nego->پچھان_تت.نسخہ = SEC_WINNT_AUTH_پچھان_تت_نسخہ;
      nego->پچھان_تت.لمائی_2 = حجم(nego->پچھان_تت);
      nego->پچھان_تت.Flags = CURL_SEC_WINNT_AUTH_پچھان_تت;
      nego->p_پچھان_تت = &nego->پچھان_تت;
    }

    /* Use the special name "!ntlm" to prevent NTLM from being used:
     * https://learn.microsoft.com/windows/win32/api/sspi/ns-sspi-sec_winnt_auth_identity_exa
     */
    nego->پچھان_تت.PackageList = CURL_UNCONST(متن_2("!ntlm"));
    nego->پچھان_تت.Package_List_لمائی = 5;

    /* Allocate our credentials handle */
    nego->credentials = curlx_calloc(1, حجم(CredHandle));
    إذا(!nego->credentials)
      أرجع CURLE_باہرلا_دا_حافظہ;

    /* Acquire our credentials handle */
    nego->حالت = Curl_pSecFn->AcquireCredentialsHandle(عدم,
                                CURL_UNCONST(متن_2(SP_NAME_NEGOTIATE)),
                                SECPKG_CRED_OUTBOUND, عدم,
                                nego->p_پچھان_تت, عدم, عدم,
                                nego->credentials, عدم);
    إذا(nego->حالت != SEC_E_OK) {
      curlx_safefree(nego->credentials);
      أرجع CURLE_AUTH_ERROR;
    }

    /* Allocate our new context handle */
    nego->context = curlx_calloc(1, حجم(CtxtHandle));
    إذا(!nego->context)
      أرجع CURLE_باہرلا_دا_حافظہ;
  }

  chlg_desc.ul_نسخہ = SECBUFFER_نسخہ;
  chlg_desc.cBuffers = 0;
  chlg_desc.pBuffers = chlg_buf;

  إذا(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    إذا(*chlg64 != '=') {
      نتیجہ = curlx_base64_decode(chlg64, &chlg, &chlglen);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }

    /* Ensure we have a valid challenge message */
    إذا(!chlg) {
      infof(انکڑے, "SPNEGO handshake failure (empty challenge message)");
      أرجع CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.cBuffers     = 1;
    chlg_buf[0].بفر_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_بفر   = chlg;
    chlg_buf[0].cb_بفر   = curlx_uztoul(chlglen);
  }

  /* SSL context comes from Schannel.
   * When extended protection is used in IIS server, pass its channel
   * bindings on the initial call too. HTTP Negotiate can create and send a
   * Kerberos token before receiving a challenge from the server.
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  إذا(nego->sslContext) {
    nego->حالت = Curl_pSecFn->Query_Context_گن(
        nego->sslContext,
        SECPKG_ATTR_ENDPOINT_BINDINGS,
        &pkgBindings);
    إذا(nego->حالت == SEC_E_OK) {
      Sec_بفر *binding_buf = &chlg_buf[chlg_desc.cBuffers++];
      binding_buf->بفر_Type = SECBUFFER_CHANNEL_BINDINGS;
      binding_buf->cb_بفر   = pkgBindings.Bindings_لمائی;
      binding_buf->pv_بفر   = pkgBindings.Bindings;
    }
  }

  /* Setup the response "output" security buffer */
  resp_desc.ul_نسخہ = SECBUFFER_نسخہ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.بفر_Type = SECBUFFER_TOKEN;
  resp_buf.pv_بفر   = nego->output_token;
  resp_buf.cb_بفر   = curlx_uztoul(nego->token_max);

  /* Generate our challenge-response message */
  {
    DWORD sspi_flags = ISC_REQ_CONFIDENTIALITY;
    إذا(انکڑے->سیٹ.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
      sspi_flags |= ISC_REQ_DELEGATE | ISC_REQ_MUTUAL_AUTH;
    nego->حالت =
      Curl_pSecFn->InitializeSecurityContext(nego->credentials,
                                             chlg ? nego->context : عدم,
                                             nego->spn,
                                             sspi_flags,
                                             0, SECURITY_NATIVE_DREP,
                                             chlg_desc.cBuffers ?
                                               &chlg_desc : عدم,
                                             0, nego->context,
                                             &resp_desc, &attrs, عدم);
  }

  إذا(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_بفر(pkgBindings.Bindings);

  /* Free the decoded challenge as it is not required anymore */
  curlx_free(chlg);

  إذا(GSS_ERROR(nego->حالت)) {
    حرف بفر[STRERROR_LEN];
    failf(انکڑے, "InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(nego->حالت, بفر, حجم(بفر)));

    إذا(nego->حالت == SEC_E_INSUFFICIENT_حافظہ)
      أرجع CURLE_باہرلا_دا_حافظہ;

    أرجع CURLE_AUTH_ERROR;
  }

  إذا(nego->حالت == SEC_I_COMPLETE_NEEDED ||
     nego->حالت == SEC_I_COMPLETE_AND_CONTINUE) {
    nego->حالت = Curl_pSecFn->CompleteAuthToken(nego->context, &resp_desc);
    إذا(GSS_ERROR(nego->حالت)) {
      حرف بفر[STRERROR_LEN];
      failf(انکڑے, "CompleteAuthToken failed: %s",
            Curl_sspi_strerror(nego->حالت, بفر, حجم(بفر)));

      إذا(nego->حالت == SEC_E_INSUFFICIENT_حافظہ)
        أرجع CURLE_باہرلا_دا_حافظہ;

      أرجع CURLE_AUTH_ERROR;
    }
  }

  nego->output_token_لمائی = resp_buf.cb_بفر;

  أرجع نتیجہ;
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
CURLcode Curl_auth_create_spnego_سنیہا(بنية negotiatedata *nego,
                                         حرف **outptr, size_t *outlen)
{
  /* Base64 encode the already generated response */
  CURLcode نتیجہ = curlx_base64_encode(nego->output_token,
                                        nego->output_token_لمائی, outptr,
                                        outlen);
  إذا(!نتیجہ && (!*outptr || !*outlen)) {
    curlx_free(*outptr);
    نتیجہ = CURLE_REMOTE_ACCESS_DENIED;
  }

  أرجع نتیجہ;
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
  /* Free our security context */
  إذا(nego->context) {
    Curl_pSecFn->DeleteSecurityContext(nego->context);
    curlx_safefree(nego->context);
  }

  /* Free our credentials handle */
  إذا(nego->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(nego->credentials);
    curlx_safefree(nego->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_پچھان_تت(nego->p_پچھان_تت);
  nego->p_پچھان_تت = عدم;

  /* Free the SPN and output token */
  curlx_safefree(nego->spn);
  curlx_safefree(nego->output_token);

  /* Reset any variables */
  nego->حالت = 0;
  nego->token_max = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#جے_انت /* USE_WINDOWS_SSPI && USE_SPNEGO */
