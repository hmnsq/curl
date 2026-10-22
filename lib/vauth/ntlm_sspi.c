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
#समावेश "curl_setup.h"

#अगर defined(USE_WINDOWS_SSPI) && defined(USE_NTLM)

#समावेश "vauth/vauth.h"
#समावेश "curl_ntlm_core.h"
#समावेश "curl_trc.h"
#समावेश "curlx/strdup.h"

/*
 * Curl_auth_is_ntlm_supported()
 *
 * This is used to evaluate if NTLM is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if NTLM is supported by Windows SSPI.
 */
bool Curl_auth_is_ntlm_supported(व्योम)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for NTLM */
  status = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(TEXT(SP_NAME_NTLM)),
                                     &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  अगर(status == SEC_E_OK) {
    Curl_pSecFn->Free_Context_बफर(SecurityPackage);
  }

  वापस status == SEC_E_OK;
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
CURLcode Curl_auth_create_ntlm_type1_message(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_creds *creds,
                                             स्थिर अक्षर *default_service,
                                             स्थिर अक्षर *host,
                                             काष्ठा ntlmdata *ntlm,
                                             काष्ठा bufref *out)
{
  PSecPkgInfo SecurityPackage;
  Sec_बफर type_1_buf;
  Sec_बफर_Desc type_1_desc;
  SECURITY_STATUS status;
  अचिन्हित दीर्घ attrs;
  स्थिर अक्षर *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_cleanup_ntlm(ntlm);

  /* Query the security package for NTLM */
  status = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(TEXT(SP_NAME_NTLM)),
                                     &SecurityPackage);
  अगर(status != SEC_E_OK) {
    failf(तथ्य, "SSPI: could not get auth info");
    वापस CURLE_AUTH_ERROR;
  }

  ntlm->token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_बफर(SecurityPackage);

  /* Allocate our output buffer */
  ntlm->output_token = curlx_malloc(ntlm->token_max);
  अगर(!ntlm->output_token)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(Curl_creds_has_user(creds)) {
    CURLcode परिणाम;

    /* Populate our identity structure */
    परिणाम = Curl_create_sspi_identity(
      creds->user, creds->passwd, &ntlm->identity);
    अगर(परिणाम)
      वापस परिणाम;

    /* Allow proper cleanup of the identity structure */
    ntlm->p_identity = &ntlm->identity;
  }
  अन्यथा
    /* Use the current Windows user */
    ntlm->p_identity = शून्य;

  /* Allocate our credentials handle */
  ntlm->credentials = curlx_calloc(1, माप(CredHandle));
  अगर(!ntlm->credentials)
    वापस CURLE_OUT_OF_MEMORY;

  /* Acquire our credentials handle */
  status = Curl_pSecFn->AcquireCredentialsHandle(शून्य,
                                     CURL_UNCONST(TEXT(SP_NAME_NTLM)),
                                     SECPKG_CRED_OUTBOUND, शून्य,
                                     ntlm->p_identity, शून्य, शून्य,
                                     ntlm->credentials, शून्य);
  अगर(status != SEC_E_OK) {
    curlx_safefree(ntlm->credentials);
    वापस CURLE_LOGIN_DENIED;
  }

  /* Allocate our new context handle */
  ntlm->context = curlx_calloc(1, माप(CtxtHandle));
  अगर(!ntlm->context)
    वापस CURLE_OUT_OF_MEMORY;

  ntlm->spn = Curl_auth_build_spn(service, host, शून्य);
  अगर(!ntlm->spn)
    वापस CURLE_OUT_OF_MEMORY;

  /* Setup the type-1 "output" security buffer */
  type_1_desc.ulVersion = SECBUFFER_VERSION;
  type_1_desc.cBuffers  = 1;
  type_1_desc.pBuffers  = &type_1_buf;
  type_1_buf.बफर_Type = SECBUFFER_TOKEN;
  type_1_buf.pv_बफर   = ntlm->output_token;
  type_1_buf.cb_बफर   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-1 message */
  status = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials, शून्य,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_NETWORK_DREP,
                                                  शून्य, 0,
                                                  ntlm->context, &type_1_desc,
                                                  &attrs, शून्य);
  अगर(status == SEC_I_COMPLETE_NEEDED ||
     status == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(ntlm->context, &type_1_desc);
  अन्यथा अगर(status == SEC_E_INSUFFICIENT_MEMORY)
    वापस CURLE_OUT_OF_MEMORY;
  अन्यथा अगर(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED)
    वापस CURLE_AUTH_ERROR;

  /* Return the response. */
  Curl_bufref_set(out, ntlm->output_token, type_1_buf.cb_बफर, शून्य);
  वापस CURLE_OK;
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
CURLcode Curl_auth_decode_ntlm_type2_message(काष्ठा Curl_easy *तथ्य,
                                             स्थिर काष्ठा bufref *type2ref,
                                             काष्ठा ntlmdata *ntlm)
{
  /* Ensure we have a valid type-2 message */
  अगर(!Curl_bufref_len(type2ref)) {
    infof(तथ्य, "NTLM handshake failure (empty type-2 message)");
    वापस CURLE_BAD_CONTENT_ENCODING;
  }

  /* Store the challenge for later use */
  ntlm->निवेश_token = curlx_memdup0(Curl_bufref_ptr(type2ref),
                                    Curl_bufref_len(type2ref));
  अगर(!ntlm->निवेश_token)
    वापस CURLE_OUT_OF_MEMORY;
  ntlm->निवेश_token_len = Curl_bufref_len(type2ref);

  वापस CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_message(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_creds *creds,
                                             काष्ठा ntlmdata *ntlm,
                                             काष्ठा bufref *out)
{
  CURLcode परिणाम = CURLE_OK;
  Sec_बफर type_2_bufs[2];
  Sec_बफर type_3_buf;
  Sec_बफर_Desc type_2_desc;
  Sec_बफर_Desc type_3_desc;
  SECURITY_STATUS status;
  अचिन्हित दीर्घ attrs;
  SecPkgContext_Bindings pkgBindings = { 0, शून्य };

  (व्योम)creds;

  /* Setup the type-2 "input" security buffer */
  type_2_desc.ulVersion     = SECBUFFER_VERSION;
  type_2_desc.cBuffers      = 1;
  type_2_desc.pBuffers      = &type_2_bufs[0];
  type_2_bufs[0].बफर_Type = SECBUFFER_TOKEN;
  type_2_bufs[0].pv_बफर   = ntlm->निवेश_token;
  type_2_bufs[0].cb_बफर   = curlx_uztoul(ntlm->निवेश_token_len);

  /* SSL context comes from schannel.
   * When extended protection is used in IIS server,
   * we have to pass a second SecBuffer to the SecBufferDesc
   * otherwise IIS does not pass the authentication (401 response).
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  अगर(ntlm->sslContext) {
    status = Curl_pSecFn->QueryContextAttributes(
      ntlm->sslContext,
      SECPKG_ATTR_ENDPOINT_BINDINGS,
      &pkgBindings
    );
    अगर(status == SEC_E_OK) {
      type_2_desc.cBuffers++;
      type_2_bufs[1].बफर_Type = SECBUFFER_CHANNEL_BINDINGS;
      type_2_bufs[1].cb_बफर = pkgBindings.Bindings_लंबाई;
      type_2_bufs[1].pv_बफर = pkgBindings.Bindings;
    }
  }

  /* Setup the type-3 "output" security buffer */
  type_3_desc.ulVersion = SECBUFFER_VERSION;
  type_3_desc.cBuffers  = 1;
  type_3_desc.pBuffers  = &type_3_buf;
  type_3_buf.बफर_Type = SECBUFFER_TOKEN;
  type_3_buf.pv_बफर   = ntlm->output_token;
  type_3_buf.cb_बफर   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-3 message */
  status = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials,
                                                  ntlm->context,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_NETWORK_DREP,
                                                  &type_2_desc,
                                                  0, ntlm->context,
                                                  &type_3_desc,
                                                  &attrs, शून्य);

  अगर(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_बफर(pkgBindings.Bindings);

  अगर(status != SEC_E_OK) {
    infof(तथ्य, "NTLM handshake failure (type-3 message): Status=0x%08lx",
          (अचिन्हित दीर्घ)status);

    अगर(status == SEC_E_INSUFFICIENT_MEMORY)
      वापस CURLE_OUT_OF_MEMORY;

    वापस CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  परिणाम = Curl_bufref_memdup0(out, ntlm->output_token, type_3_buf.cb_बफर);
  Curl_auth_cleanup_ntlm(ntlm);
  वापस परिणाम;
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
व्योम Curl_auth_cleanup_ntlm(काष्ठा ntlmdata *ntlm)
{
  /* Free our security context */
  अगर(ntlm->context) {
    Curl_pSecFn->DeleteSecurityContext(ntlm->context);
    curlx_safefree(ntlm->context);
  }

  /* Free our credentials handle */
  अगर(ntlm->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(ntlm->credentials);
    curlx_safefree(ntlm->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_identity(ntlm->p_identity);
  ntlm->p_identity = शून्य;

  /* Free the input and output tokens */
  curlx_safefree(ntlm->निवेश_token);
  curlx_safefree(ntlm->output_token);

  /* Reset any variables */
  ntlm->token_max = 0;

  curlx_safefree(ntlm->spn);
}

#पूर्ण_अगर /* USE_WINDOWS_SSPI && USE_NTLM */
