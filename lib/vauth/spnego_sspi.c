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
#समावेश "curl_setup.h"

#अगर defined(USE_WINDOWS_SSPI) && defined(USE_SPNEGO)

#समावेश "vauth/vauth.h"
#समावेश "curlx/base64.h"
#समावेश "curl_trc.h"
#समावेश "strerror.h"

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate is supported by Windows SSPI.
 */
bool Curl_auth_is_spnego_supported(व्योम)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for Negotiate */
  status = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(TEXT(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  अगर(status == SEC_E_OK) {
    Curl_pSecFn->Free_Context_बफर(SecurityPackage);
  }

  वापस status == SEC_E_OK;
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
CURLcode Curl_auth_decode_spnego_message(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा Curl_creds *creds,
                                         स्थिर अक्षर *default_service,
                                         स्थिर अक्षर *host,
                                         स्थिर अक्षर *chlg64,
                                         काष्ठा negotiatedata *nego)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार chlglen = 0;
  अचिन्हित अक्षर *chlg = शून्य;
  PSecPkgInfo SecurityPackage;
  Sec_बफर chlg_buf[2];
  Sec_बफर resp_buf;
  Sec_बफर_Desc chlg_desc;
  Sec_बफर_Desc resp_desc;
  अचिन्हित दीर्घ attrs;
  SecPkgContext_Bindings pkgBindings = { 0, शून्य };

  अगर(nego->context && nego->status == SEC_E_OK) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    वापस CURLE_LOGIN_DENIED;
  }

  अगर(!nego->spn) {
    /* Generate our SPN */
    स्थिर अक्षर *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    nego->spn = Curl_auth_build_spn(service, host, शून्य);
    अगर(!nego->spn)
      वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(!nego->output_token) {
    /* Query the security package for Negotiate */
    nego->status = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(TEXT(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);
    अगर(nego->status != SEC_E_OK) {
      failf(तथ्य, "SSPI: could not get auth info");
      वापस CURLE_AUTH_ERROR;
    }

    nego->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_बफर(SecurityPackage);

    /* Allocate our output buffer */
    nego->output_token = curlx_malloc(nego->token_max);
    अगर(!nego->output_token)
      वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(!nego->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    अगर(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      परिणाम = Curl_create_sspi_identity(creds->user, creds->passwd,
                                         &nego->identity);
      अगर(परिणाम)
        वापस परिणाम;

      /* Allow proper cleanup of the identity structure */
      nego->p_identity = &nego->identity;
    }
    अन्यथा
      /* Use the current Windows user */
      nego->p_identity = शून्य;

    /* Exclude NTLM from SPNEGO negotiation via the PackageList field */
    अगर(!nego->p_identity) {
      स_रखो(&nego->identity, 0, माप(nego->identity));
      nego->identity.Version = SEC_WINNT_AUTH_IDENTITY_VERSION;
      nego->identity.लंबाई_2 = माप(nego->identity);
      nego->identity.Flags = CURL_SEC_WINNT_AUTH_IDENTITY;
      nego->p_identity = &nego->identity;
    }

    /* Use the special name "!ntlm" to prevent NTLM from being used:
     * https://learn.microsoft.com/windows/win32/api/sspi/ns-sspi-sec_winnt_auth_identity_exa
     */
    nego->identity.PackageList = CURL_UNCONST(TEXT("!ntlm"));
    nego->identity.Package_List_लंबाई = 5;

    /* Allocate our credentials handle */
    nego->credentials = curlx_calloc(1, माप(CredHandle));
    अगर(!nego->credentials)
      वापस CURLE_OUT_OF_MEMORY;

    /* Acquire our credentials handle */
    nego->status = Curl_pSecFn->AcquireCredentialsHandle(शून्य,
                                CURL_UNCONST(TEXT(SP_NAME_NEGOTIATE)),
                                SECPKG_CRED_OUTBOUND, शून्य,
                                nego->p_identity, शून्य, शून्य,
                                nego->credentials, शून्य);
    अगर(nego->status != SEC_E_OK) {
      curlx_safefree(nego->credentials);
      वापस CURLE_AUTH_ERROR;
    }

    /* Allocate our new context handle */
    nego->context = curlx_calloc(1, माप(CtxtHandle));
    अगर(!nego->context)
      वापस CURLE_OUT_OF_MEMORY;
  }

  chlg_desc.ulVersion = SECBUFFER_VERSION;
  chlg_desc.cBuffers = 0;
  chlg_desc.pBuffers = chlg_buf;

  अगर(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    अगर(*chlg64 != '=') {
      परिणाम = curlx_base64_decode(chlg64, &chlg, &chlglen);
      अगर(परिणाम)
        वापस परिणाम;
    }

    /* Ensure we have a valid challenge message */
    अगर(!chlg) {
      infof(तथ्य, "SPNEGO handshake failure (empty challenge message)");
      वापस CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.cBuffers     = 1;
    chlg_buf[0].बफर_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_बफर   = chlg;
    chlg_buf[0].cb_बफर   = curlx_uztoul(chlglen);
  }

  /* SSL context comes from Schannel.
   * When extended protection is used in IIS server, pass its channel
   * bindings on the initial call too. HTTP Negotiate can create and send a
   * Kerberos token before receiving a challenge from the server.
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  अगर(nego->sslContext) {
    nego->status = Curl_pSecFn->QueryContextAttributes(
        nego->sslContext,
        SECPKG_ATTR_ENDPOINT_BINDINGS,
        &pkgBindings);
    अगर(nego->status == SEC_E_OK) {
      Sec_बफर *binding_buf = &chlg_buf[chlg_desc.cBuffers++];
      binding_buf->बफर_Type = SECBUFFER_CHANNEL_BINDINGS;
      binding_buf->cb_बफर   = pkgBindings.Bindings_लंबाई;
      binding_buf->pv_बफर   = pkgBindings.Bindings;
    }
  }

  /* Setup the response "output" security buffer */
  resp_desc.ulVersion = SECBUFFER_VERSION;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.बफर_Type = SECBUFFER_TOKEN;
  resp_buf.pv_बफर   = nego->output_token;
  resp_buf.cb_बफर   = curlx_uztoul(nego->token_max);

  /* Generate our challenge-response message */
  {
    DWORD sspi_flags = ISC_REQ_CONFIDENTIALITY;
    अगर(तथ्य->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
      sspi_flags |= ISC_REQ_DELEGATE | ISC_REQ_MUTUAL_AUTH;
    nego->status =
      Curl_pSecFn->InitializeSecurityContext(nego->credentials,
                                             chlg ? nego->context : शून्य,
                                             nego->spn,
                                             sspi_flags,
                                             0, SECURITY_NATIVE_DREP,
                                             chlg_desc.cBuffers ?
                                               &chlg_desc : शून्य,
                                             0, nego->context,
                                             &resp_desc, &attrs, शून्य);
  }

  अगर(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_बफर(pkgBindings.Bindings);

  /* Free the decoded challenge as it is not required anymore */
  curlx_free(chlg);

  अगर(GSS_ERROR(nego->status)) {
    अक्षर बफर[STRERROR_LEN];
    failf(तथ्य, "InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(nego->status, बफर, माप(बफर)));

    अगर(nego->status == SEC_E_INSUFFICIENT_MEMORY)
      वापस CURLE_OUT_OF_MEMORY;

    वापस CURLE_AUTH_ERROR;
  }

  अगर(nego->status == SEC_I_COMPLETE_NEEDED ||
     nego->status == SEC_I_COMPLETE_AND_CONTINUE) {
    nego->status = Curl_pSecFn->CompleteAuthToken(nego->context, &resp_desc);
    अगर(GSS_ERROR(nego->status)) {
      अक्षर बफर[STRERROR_LEN];
      failf(तथ्य, "CompleteAuthToken failed: %s",
            Curl_sspi_strerror(nego->status, बफर, माप(बफर)));

      अगर(nego->status == SEC_E_INSUFFICIENT_MEMORY)
        वापस CURLE_OUT_OF_MEMORY;

      वापस CURLE_AUTH_ERROR;
    }
  }

  nego->output_token_लंबाई = resp_buf.cb_बफर;

  वापस परिणाम;
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
CURLcode Curl_auth_create_spnego_message(काष्ठा negotiatedata *nego,
                                         अक्षर **outptr, माप_प्रकार *outlen)
{
  /* Base64 encode the already generated response */
  CURLcode परिणाम = curlx_base64_encode(nego->output_token,
                                        nego->output_token_लंबाई, outptr,
                                        outlen);
  अगर(!परिणाम && (!*outptr || !*outlen)) {
    curlx_free(*outptr);
    परिणाम = CURLE_REMOTE_ACCESS_DENIED;
  }

  वापस परिणाम;
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
व्योम Curl_auth_cleanup_spnego(काष्ठा negotiatedata *nego)
{
  /* Free our security context */
  अगर(nego->context) {
    Curl_pSecFn->DeleteSecurityContext(nego->context);
    curlx_safefree(nego->context);
  }

  /* Free our credentials handle */
  अगर(nego->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(nego->credentials);
    curlx_safefree(nego->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_identity(nego->p_identity);
  nego->p_identity = शून्य;

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

#पूर्ण_अगर /* USE_WINDOWS_SSPI && USE_SPNEGO */
