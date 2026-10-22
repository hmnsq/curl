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
#অন্তর্ভাব "curl_setup.h"

#যদি defined(USE_WINDOWS_SSPI) && defined(USE_SPNEGO)

#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strerror.h"

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate is supported by Windows SSPI.
 */
bool Curl_auth_is_spnego_supported(শূন্য)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for Negotiate */
  status = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(TEXT(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  যদি(status == SEC_E_OK) {
    Curl_pSecFn->Free_Context_বাফার(SecurityPackage);
  }

  ফেরত status == SEC_E_OK;
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
CURLcode Curl_auth_decode_spnego_message(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো Curl_creds *creds,
                                         ধ্রুবক অক্ষর *default_service,
                                         ধ্রুবক অক্ষর *host,
                                         ধ্রুবক অক্ষর *chlg64,
                                         কাঠামো negotiatedata *nego)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t chlglen = 0;
  unsigned অক্ষর *chlg = ফাঁকা;
  PSecPkgInfo SecurityPackage;
  Sec_বাফার chlg_buf[2];
  Sec_বাফার resp_buf;
  Sec_বাফার_Desc chlg_desc;
  Sec_বাফার_Desc resp_desc;
  unsigned long attrs;
  SecPkgContext_Bindings pkgBindings = { 0, ফাঁকা };

  যদি(nego->context && nego->status == SEC_E_OK) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    ফেরত CURLE_LOGIN_DENIED;
  }

  যদি(!nego->spn) {
    /* Generate our SPN */
    ধ্রুবক অক্ষর *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    nego->spn = Curl_auth_build_spn(service, host, ফাঁকা);
    যদি(!nego->spn)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(!nego->output_token) {
    /* Query the security package for Negotiate */
    nego->status = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(TEXT(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);
    যদি(nego->status != SEC_E_OK) {
      failf(তথ্য, "SSPI: could not get auth info");
      ফেরত CURLE_AUTH_ERROR;
    }

    nego->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_বাফার(SecurityPackage);

    /* Allocate our output buffer */
    nego->output_token = curlx_malloc(nego->token_max);
    যদি(!nego->output_token)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(!nego->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    যদি(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ফলাফল = Curl_create_sspi_identity(creds->user, creds->passwd,
                                         &nego->identity);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      /* Allow proper cleanup of the identity structure */
      nego->p_identity = &nego->identity;
    }
    অন্যথা
      /* Use the current Windows user */
      nego->p_identity = ফাঁকা;

    /* Exclude NTLM from SPNEGO negotiation via the PackageList field */
    যদি(!nego->p_identity) {
      memset(&nego->identity, 0, আকার(nego->identity));
      nego->identity.Version = SEC_WINNT_AUTH_IDENTITY_VERSION;
      nego->identity.দৈর্ঘ্য_2 = আকার(nego->identity);
      nego->identity.Flags = CURL_SEC_WINNT_AUTH_IDENTITY;
      nego->p_identity = &nego->identity;
    }

    /* Use the special name "!ntlm" to prevent NTLM from being used:
     * https://learn.microsoft.com/windows/win32/api/sspi/ns-sspi-sec_winnt_auth_identity_exa
     */
    nego->identity.PackageList = CURL_UNCONST(TEXT("!ntlm"));
    nego->identity.Package_List_দৈর্ঘ্য = 5;

    /* Allocate our credentials handle */
    nego->credentials = curlx_calloc(1, আকার(CredHandle));
    যদি(!nego->credentials)
      ফেরত CURLE_OUT_OF_MEMORY;

    /* Acquire our credentials handle */
    nego->status = Curl_pSecFn->AcquireCredentialsHandle(ফাঁকা,
                                CURL_UNCONST(TEXT(SP_NAME_NEGOTIATE)),
                                SECPKG_CRED_OUTBOUND, ফাঁকা,
                                nego->p_identity, ফাঁকা, ফাঁকা,
                                nego->credentials, ফাঁকা);
    যদি(nego->status != SEC_E_OK) {
      curlx_safefree(nego->credentials);
      ফেরত CURLE_AUTH_ERROR;
    }

    /* Allocate our new context handle */
    nego->context = curlx_calloc(1, আকার(CtxtHandle));
    যদি(!nego->context)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  chlg_desc.ulVersion = SECBUFFER_VERSION;
  chlg_desc.cBuffers = 0;
  chlg_desc.pBuffers = chlg_buf;

  যদি(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    যদি(*chlg64 != '=') {
      ফলাফল = curlx_base64_decode(chlg64, &chlg, &chlglen);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }

    /* Ensure we have a valid challenge message */
    যদি(!chlg) {
      infof(তথ্য, "SPNEGO handshake failure (empty challenge message)");
      ফেরত CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.cBuffers     = 1;
    chlg_buf[0].বাফার_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_বাফার   = chlg;
    chlg_buf[0].cb_বাফার   = curlx_uztoul(chlglen);
  }

  /* SSL context comes from Schannel.
   * When extended protection is used in IIS server, pass its channel
   * bindings on the initial call too. HTTP Negotiate can create and send a
   * Kerberos token before receiving a challenge from the server.
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  যদি(nego->sslContext) {
    nego->status = Curl_pSecFn->QueryContextAttributes(
        nego->sslContext,
        SECPKG_ATTR_ENDPOINT_BINDINGS,
        &pkgBindings);
    যদি(nego->status == SEC_E_OK) {
      Sec_বাফার *binding_buf = &chlg_buf[chlg_desc.cBuffers++];
      binding_buf->বাফার_Type = SECBUFFER_CHANNEL_BINDINGS;
      binding_buf->cb_বাফার   = pkgBindings.Bindings_দৈর্ঘ্য;
      binding_buf->pv_বাফার   = pkgBindings.Bindings;
    }
  }

  /* Setup the response "output" security buffer */
  resp_desc.ulVersion = SECBUFFER_VERSION;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.বাফার_Type = SECBUFFER_TOKEN;
  resp_buf.pv_বাফার   = nego->output_token;
  resp_buf.cb_বাফার   = curlx_uztoul(nego->token_max);

  /* Generate our challenge-response message */
  {
    DWORD sspi_flags = ISC_REQ_CONFIDENTIALITY;
    যদি(তথ্য->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
      sspi_flags |= ISC_REQ_DELEGATE | ISC_REQ_MUTUAL_AUTH;
    nego->status =
      Curl_pSecFn->InitializeSecurityContext(nego->credentials,
                                             chlg ? nego->context : ফাঁকা,
                                             nego->spn,
                                             sspi_flags,
                                             0, SECURITY_NATIVE_DREP,
                                             chlg_desc.cBuffers ?
                                               &chlg_desc : ফাঁকা,
                                             0, nego->context,
                                             &resp_desc, &attrs, ফাঁকা);
  }

  যদি(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_বাফার(pkgBindings.Bindings);

  /* Free the decoded challenge as it is not required anymore */
  curlx_free(chlg);

  যদি(GSS_ERROR(nego->status)) {
    অক্ষর বাফার[STRERROR_LEN];
    failf(তথ্য, "InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(nego->status, বাফার, আকার(বাফার)));

    যদি(nego->status == SEC_E_INSUFFICIENT_MEMORY)
      ফেরত CURLE_OUT_OF_MEMORY;

    ফেরত CURLE_AUTH_ERROR;
  }

  যদি(nego->status == SEC_I_COMPLETE_NEEDED ||
     nego->status == SEC_I_COMPLETE_AND_CONTINUE) {
    nego->status = Curl_pSecFn->CompleteAuthToken(nego->context, &resp_desc);
    যদি(GSS_ERROR(nego->status)) {
      অক্ষর বাফার[STRERROR_LEN];
      failf(তথ্য, "CompleteAuthToken failed: %s",
            Curl_sspi_strerror(nego->status, বাফার, আকার(বাফার)));

      যদি(nego->status == SEC_E_INSUFFICIENT_MEMORY)
        ফেরত CURLE_OUT_OF_MEMORY;

      ফেরত CURLE_AUTH_ERROR;
    }
  }

  nego->output_token_দৈর্ঘ্য = resp_buf.cb_বাফার;

  ফেরত ফলাফল;
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
CURLcode Curl_auth_create_spnego_message(কাঠামো negotiatedata *nego,
                                         অক্ষর **outptr, size_t *outlen)
{
  /* Base64 encode the already generated response */
  CURLcode ফলাফল = curlx_base64_encode(nego->output_token,
                                        nego->output_token_দৈর্ঘ্য, outptr,
                                        outlen);
  যদি(!ফলাফল && (!*outptr || !*outlen)) {
    curlx_free(*outptr);
    ফলাফল = CURLE_REMOTE_ACCESS_DENIED;
  }

  ফেরত ফলাফল;
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
শূন্য Curl_auth_cleanup_spnego(কাঠামো negotiatedata *nego)
{
  /* Free our security context */
  যদি(nego->context) {
    Curl_pSecFn->DeleteSecurityContext(nego->context);
    curlx_safefree(nego->context);
  }

  /* Free our credentials handle */
  যদি(nego->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(nego->credentials);
    curlx_safefree(nego->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_identity(nego->p_identity);
  nego->p_identity = ফাঁকা;

  /* Free the SPN and output token */
  curlx_safefree(nego->spn);
  curlx_safefree(nego->output_token);

  /* Reset any variables */
  nego->status = 0;
  nego->token_max = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#endif /* USE_WINDOWS_SSPI && USE_SPNEGO */
