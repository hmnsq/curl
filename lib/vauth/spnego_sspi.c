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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ defined(USE_WINDOWS_SSPI) && defined(USE_SPNEGO)

#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "curlx/base64.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "strerror.h"

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate is supported by Windows SSPI.
 */
bool Curl_auth_is_spnego_supported(ശൂന്യം)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for Negotiate */
  status = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(TEXT(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  എങ്കിൽ(status == SEC_E_OK) {
    Curl_pSecFn->FreeContextBuffer(SecurityPackage);
  }

  തിരികെ status == SEC_E_OK;
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
CURLcode Curl_auth_decode_spnego_message(ഘടന Curl_easy *data,
                                         ഘടന Curl_creds *creds,
                                         സ്ഥിരം അക്ഷരം *default_service,
                                         സ്ഥിരം അക്ഷരം *host,
                                         സ്ഥിരം അക്ഷരം *chlg64,
                                         ഘടന negotiatedata *nego)
{
  CURLcode result = CURLE_OK;
  size_t chlglen = 0;
  unsigned അക്ഷരം *chlg = ശൂന്യ;
  PSecPkgInfo SecurityPackage;
  SecBuffer chlg_buf[2];
  SecBuffer resp_buf;
  SecBufferDesc chlg_desc;
  SecBufferDesc resp_desc;
  unsigned long attrs;
  SecPkgContext_Bindings pkgBindings = { 0, ശൂന്യ };

  എങ്കിൽ(nego->context && nego->status == SEC_E_OK) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    തിരികെ CURLE_LOGIN_DENIED;
  }

  എങ്കിൽ(!nego->spn) {
    /* Generate our SPN */
    സ്ഥിരം അക്ഷരം *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    nego->spn = Curl_auth_build_spn(service, host, ശൂന്യ);
    എങ്കിൽ(!nego->spn)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  എങ്കിൽ(!nego->output_token) {
    /* Query the security package for Negotiate */
    nego->status = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(TEXT(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);
    എങ്കിൽ(nego->status != SEC_E_OK) {
      failf(data, "SSPI: could not get auth info");
      തിരികെ CURLE_AUTH_ERROR;
    }

    nego->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->FreeContextBuffer(SecurityPackage);

    /* Allocate our output buffer */
    nego->output_token = curlx_malloc(nego->token_max);
    എങ്കിൽ(!nego->output_token)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  എങ്കിൽ(!nego->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    എങ്കിൽ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      result = Curl_create_sspi_identity(creds->user, creds->passwd,
                                         &nego->identity);
      എങ്കിൽ(result)
        തിരികെ result;

      /* Allow proper cleanup of the identity structure */
      nego->p_identity = &nego->identity;
    }
    അല്ലെങ്കിൽ
      /* Use the current Windows user */
      nego->p_identity = ശൂന്യ;

    /* Exclude NTLM from SPNEGO negotiation via the PackageList field */
    എങ്കിൽ(!nego->p_identity) {
      memset(&nego->identity, 0, വലുപ്പം(nego->identity));
      nego->identity.Version = SEC_WINNT_AUTH_IDENTITY_VERSION;
      nego->identity.Length = വലുപ്പം(nego->identity);
      nego->identity.Flags = CURL_SEC_WINNT_AUTH_IDENTITY;
      nego->p_identity = &nego->identity;
    }

    /* Use the special name "!ntlm" to prevent NTLM from being used:
     * https://learn.microsoft.com/windows/win32/api/sspi/ns-sspi-sec_winnt_auth_identity_exa
     */
    nego->identity.PackageList = CURL_UNCONST(TEXT("!ntlm"));
    nego->identity.PackageListLength = 5;

    /* Allocate our credentials handle */
    nego->credentials = curlx_calloc(1, വലുപ്പം(CredHandle));
    എങ്കിൽ(!nego->credentials)
      തിരികെ CURLE_OUT_OF_MEMORY;

    /* Acquire our credentials handle */
    nego->status = Curl_pSecFn->AcquireCredentialsHandle(ശൂന്യ,
                                CURL_UNCONST(TEXT(SP_NAME_NEGOTIATE)),
                                SECPKG_CRED_OUTBOUND, ശൂന്യ,
                                nego->p_identity, ശൂന്യ, ശൂന്യ,
                                nego->credentials, ശൂന്യ);
    എങ്കിൽ(nego->status != SEC_E_OK) {
      curlx_safefree(nego->credentials);
      തിരികെ CURLE_AUTH_ERROR;
    }

    /* Allocate our new context handle */
    nego->context = curlx_calloc(1, വലുപ്പം(CtxtHandle));
    എങ്കിൽ(!nego->context)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  chlg_desc.ulVersion = SECBUFFER_VERSION;
  chlg_desc.cBuffers = 0;
  chlg_desc.pBuffers = chlg_buf;

  എങ്കിൽ(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    എങ്കിൽ(*chlg64 != '=') {
      result = curlx_base64_decode(chlg64, &chlg, &chlglen);
      എങ്കിൽ(result)
        തിരികെ result;
    }

    /* Ensure we have a valid challenge message */
    എങ്കിൽ(!chlg) {
      infof(data, "SPNEGO handshake failure (empty challenge message)");
      തിരികെ CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.cBuffers     = 1;
    chlg_buf[0].BufferType = SECBUFFER_TOKEN;
    chlg_buf[0].pvBuffer   = chlg;
    chlg_buf[0].cbBuffer   = curlx_uztoul(chlglen);
  }

  /* SSL context comes from Schannel.
   * When extended protection is used in IIS server, pass its channel
   * bindings on the initial call too. HTTP Negotiate can create and send a
   * Kerberos token before receiving a challenge from the server.
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  എങ്കിൽ(nego->sslContext) {
    nego->status = Curl_pSecFn->QueryContextAttributes(
        nego->sslContext,
        SECPKG_ATTR_ENDPOINT_BINDINGS,
        &pkgBindings);
    എങ്കിൽ(nego->status == SEC_E_OK) {
      SecBuffer *binding_buf = &chlg_buf[chlg_desc.cBuffers++];
      binding_buf->BufferType = SECBUFFER_CHANNEL_BINDINGS;
      binding_buf->cbBuffer   = pkgBindings.BindingsLength;
      binding_buf->pvBuffer   = pkgBindings.Bindings;
    }
  }

  /* Setup the response "output" security buffer */
  resp_desc.ulVersion = SECBUFFER_VERSION;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.BufferType = SECBUFFER_TOKEN;
  resp_buf.pvBuffer   = nego->output_token;
  resp_buf.cbBuffer   = curlx_uztoul(nego->token_max);

  /* Generate our challenge-response message */
  {
    DWORD sspi_flags = ISC_REQ_CONFIDENTIALITY;
    എങ്കിൽ(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
      sspi_flags |= ISC_REQ_DELEGATE | ISC_REQ_MUTUAL_AUTH;
    nego->status =
      Curl_pSecFn->InitializeSecurityContext(nego->credentials,
                                             chlg ? nego->context : ശൂന്യ,
                                             nego->spn,
                                             sspi_flags,
                                             0, SECURITY_NATIVE_DREP,
                                             chlg_desc.cBuffers ?
                                               &chlg_desc : ശൂന്യ,
                                             0, nego->context,
                                             &resp_desc, &attrs, ശൂന്യ);
  }

  എങ്കിൽ(pkgBindings.Bindings)
    Curl_pSecFn->FreeContextBuffer(pkgBindings.Bindings);

  /* Free the decoded challenge as it is not required anymore */
  curlx_free(chlg);

  എങ്കിൽ(GSS_ERROR(nego->status)) {
    അക്ഷരം buffer[STRERROR_LEN];
    failf(data, "InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(nego->status, buffer, വലുപ്പം(buffer)));

    എങ്കിൽ(nego->status == SEC_E_INSUFFICIENT_MEMORY)
      തിരികെ CURLE_OUT_OF_MEMORY;

    തിരികെ CURLE_AUTH_ERROR;
  }

  എങ്കിൽ(nego->status == SEC_I_COMPLETE_NEEDED ||
     nego->status == SEC_I_COMPLETE_AND_CONTINUE) {
    nego->status = Curl_pSecFn->CompleteAuthToken(nego->context, &resp_desc);
    എങ്കിൽ(GSS_ERROR(nego->status)) {
      അക്ഷരം buffer[STRERROR_LEN];
      failf(data, "CompleteAuthToken failed: %s",
            Curl_sspi_strerror(nego->status, buffer, വലുപ്പം(buffer)));

      എങ്കിൽ(nego->status == SEC_E_INSUFFICIENT_MEMORY)
        തിരികെ CURLE_OUT_OF_MEMORY;

      തിരികെ CURLE_AUTH_ERROR;
    }
  }

  nego->output_token_length = resp_buf.cbBuffer;

  തിരികെ result;
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
CURLcode Curl_auth_create_spnego_message(ഘടന negotiatedata *nego,
                                         അക്ഷരം **outptr, size_t *outlen)
{
  /* Base64 encode the already generated response */
  CURLcode result = curlx_base64_encode(nego->output_token,
                                        nego->output_token_length, outptr,
                                        outlen);
  എങ്കിൽ(!result && (!*outptr || !*outlen)) {
    curlx_free(*outptr);
    result = CURLE_REMOTE_ACCESS_DENIED;
  }

  തിരികെ result;
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
ശൂന്യം Curl_auth_cleanup_spnego(ഘടന negotiatedata *nego)
{
  /* Free our security context */
  എങ്കിൽ(nego->context) {
    Curl_pSecFn->DeleteSecurityContext(nego->context);
    curlx_safefree(nego->context);
  }

  /* Free our credentials handle */
  എങ്കിൽ(nego->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(nego->credentials);
    curlx_safefree(nego->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_identity(nego->p_identity);
  nego->p_identity = ശൂന്യ;

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
