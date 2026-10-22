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
#شامل "curl_setup.h"

#اگر defined(USE_WINDOWS_SSPI) && defined(USE_NTLM)

#شامل "vauth/vauth.h"
#شامل "curl_ntlm_core.h"
#شامل "curl_trc.h"
#شامل "curlx/strdup.h"

/*
 * Curl_auth_is_ntlm_supported()
 *
 * This is used to evaluate if NTLM is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if NTLM is supported by Windows SSPI.
 */
bool Curl_auth_is_ntlm_supported(خالی)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_حالت حالت_2;

  /* Query the security package for NTLM */
  حالت_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(متن_2(SP_NAME_NTLM)),
                                     &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  اگر(حالت_2 == SEC_E_OK) {
    Curl_pSecFn->Free_Context_بفر(SecurityPackage);
  }

  واپس حالت_2 == SEC_E_OK;
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
CURLcode Curl_auth_create_ntlm_type1_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             ثابت حرف *host,
                                             ساخت ntlmdata *ntlm,
                                             ساخت bufref *برآمد)
{
  PSecPkgInfo SecurityPackage;
  Sec_بفر type_1_buf;
  Sec_بفر_Desc type_1_desc;
  SECURITY_حالت حالت_2;
  غیرعلامتی طویل attrs;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_cleanup_ntlm(ntlm);

  /* Query the security package for NTLM */
  حالت_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(متن_2(SP_NAME_NTLM)),
                                     &SecurityPackage);
  اگر(حالت_2 != SEC_E_OK) {
    failf(ڈیٹا, "SSPI: could not get auth info");
    واپس CURLE_AUTH_ERROR;
  }

  ntlm->token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_بفر(SecurityPackage);

  /* Allocate our output buffer */
  ntlm->output_token = curlx_malloc(ntlm->token_max);
  اگر(!ntlm->output_token)
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(Curl_creds_has_user(creds)) {
    CURLcode نتیجہ;

    /* Populate our identity structure */
    نتیجہ = Curl_create_sspi_شناختی_عنصر(
      creds->user, creds->passwd, &ntlm->شناختی_عنصر);
    اگر(نتیجہ)
      واپس نتیجہ;

    /* Allow proper cleanup of the identity structure */
    ntlm->p_شناختی_عنصر = &ntlm->شناختی_عنصر;
  }
  ورنہ
    /* Use the current Windows user */
    ntlm->p_شناختی_عنصر = تہی;

  /* Allocate our credentials handle */
  ntlm->credentials = curlx_calloc(1, حجم(CredHandle));
  اگر(!ntlm->credentials)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Acquire our credentials handle */
  حالت_2 = Curl_pSecFn->AcquireCredentialsHandle(تہی,
                                     CURL_UNCONST(متن_2(SP_NAME_NTLM)),
                                     SECPKG_CRED_OUTBOUND, تہی,
                                     ntlm->p_شناختی_عنصر, تہی, تہی,
                                     ntlm->credentials, تہی);
  اگر(حالت_2 != SEC_E_OK) {
    curlx_safefree(ntlm->credentials);
    واپس CURLE_LOGIN_DENIED;
  }

  /* Allocate our new context handle */
  ntlm->context = curlx_calloc(1, حجم(CtxtHandle));
  اگر(!ntlm->context)
    واپس CURLE_برآمد_کا_حافظہ;

  ntlm->spn = Curl_auth_build_spn(service, host, تہی);
  اگر(!ntlm->spn)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Setup the type-1 "output" security buffer */
  type_1_desc.ul_نسخہ = SECBUFFER_نسخہ;
  type_1_desc.cBuffers  = 1;
  type_1_desc.pBuffers  = &type_1_buf;
  type_1_buf.بفر_Type = SECBUFFER_TOKEN;
  type_1_buf.pv_بفر   = ntlm->output_token;
  type_1_buf.cb_بفر   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-1 message */
  حالت_2 = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials, تہی,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_جال_DREP,
                                                  تہی, 0,
                                                  ntlm->context, &type_1_desc,
                                                  &attrs, تہی);
  اگر(حالت_2 == SEC_I_COMPLETE_NEEDED ||
     حالت_2 == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(ntlm->context, &type_1_desc);
  ورنہ اگر(حالت_2 == SEC_E_INSUFFICIENT_حافظہ)
    واپس CURLE_برآمد_کا_حافظہ;
  ورنہ اگر(حالت_2 != SEC_E_OK && حالت_2 != SEC_I_CONTINUE_NEEDED)
    واپس CURLE_AUTH_ERROR;

  /* Return the response. */
  Curl_bufref_سیٹ(برآمد, ntlm->output_token, type_1_buf.cb_بفر, تہی);
  واپس CURLE_OK;
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
CURLcode Curl_auth_decode_ntlm_type2_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ثابت ساخت bufref *type2ref,
                                             ساخت ntlmdata *ntlm)
{
  /* Ensure we have a valid type-2 message */
  اگر(!Curl_bufref_len(type2ref)) {
    infof(ڈیٹا, "NTLM handshake failure (empty type-2 message)");
    واپس CURLE_BAD_CONTENT_ENCODING;
  }

  /* Store the challenge for later use */
  ntlm->داخلہ_token = curlx_memdup0(Curl_bufref_ptr(type2ref),
                                    Curl_bufref_len(type2ref));
  اگر(!ntlm->داخلہ_token)
    واپس CURLE_برآمد_کا_حافظہ;
  ntlm->داخلہ_token_len = Curl_bufref_len(type2ref);

  واپس CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_creds *creds,
                                             ساخت ntlmdata *ntlm,
                                             ساخت bufref *برآمد)
{
  CURLcode نتیجہ = CURLE_OK;
  Sec_بفر type_2_bufs[2];
  Sec_بفر type_3_buf;
  Sec_بفر_Desc type_2_desc;
  Sec_بفر_Desc type_3_desc;
  SECURITY_حالت حالت_2;
  غیرعلامتی طویل attrs;
  SecPkgContext_Bindings pkgBindings = { 0, تہی };

  (خالی)creds;

  /* Setup the type-2 "input" security buffer */
  type_2_desc.ul_نسخہ     = SECBUFFER_نسخہ;
  type_2_desc.cBuffers      = 1;
  type_2_desc.pBuffers      = &type_2_bufs[0];
  type_2_bufs[0].بفر_Type = SECBUFFER_TOKEN;
  type_2_bufs[0].pv_بفر   = ntlm->داخلہ_token;
  type_2_bufs[0].cb_بفر   = curlx_uztoul(ntlm->داخلہ_token_len);

  /* SSL context comes from schannel.
   * When extended protection is used in IIS server,
   * we have to pass a second SecBuffer to the SecBufferDesc
   * otherwise IIS does not pass the authentication (401 response).
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  اگر(ntlm->sslContext) {
    حالت_2 = Curl_pSecFn->Query_Context_اوصاف(
      ntlm->sslContext,
      SECPKG_ATTR_ENDPOINT_BINDINGS,
      &pkgBindings
    );
    اگر(حالت_2 == SEC_E_OK) {
      type_2_desc.cBuffers++;
      type_2_bufs[1].بفر_Type = SECBUFFER_CHANNEL_BINDINGS;
      type_2_bufs[1].cb_بفر = pkgBindings.Bindings_لمبائی;
      type_2_bufs[1].pv_بفر = pkgBindings.Bindings;
    }
  }

  /* Setup the type-3 "output" security buffer */
  type_3_desc.ul_نسخہ = SECBUFFER_نسخہ;
  type_3_desc.cBuffers  = 1;
  type_3_desc.pBuffers  = &type_3_buf;
  type_3_buf.بفر_Type = SECBUFFER_TOKEN;
  type_3_buf.pv_بفر   = ntlm->output_token;
  type_3_buf.cb_بفر   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-3 message */
  حالت_2 = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials,
                                                  ntlm->context,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_جال_DREP,
                                                  &type_2_desc,
                                                  0, ntlm->context,
                                                  &type_3_desc,
                                                  &attrs, تہی);

  اگر(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_بفر(pkgBindings.Bindings);

  اگر(حالت_2 != SEC_E_OK) {
    infof(ڈیٹا, "NTLM handshake failure (type-3 message): Status=0x%08lx",
          (غیرعلامتی طویل)حالت_2);

    اگر(حالت_2 == SEC_E_INSUFFICIENT_حافظہ)
      واپس CURLE_برآمد_کا_حافظہ;

    واپس CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  نتیجہ = Curl_bufref_memdup0(برآمد, ntlm->output_token, type_3_buf.cb_بفر);
  Curl_auth_cleanup_ntlm(ntlm);
  واپس نتیجہ;
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
خالی Curl_auth_cleanup_ntlm(ساخت ntlmdata *ntlm)
{
  /* Free our security context */
  اگر(ntlm->context) {
    Curl_pSecFn->DeleteSecurityContext(ntlm->context);
    curlx_safefree(ntlm->context);
  }

  /* Free our credentials handle */
  اگر(ntlm->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(ntlm->credentials);
    curlx_safefree(ntlm->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_شناختی_عنصر(ntlm->p_شناختی_عنصر);
  ntlm->p_شناختی_عنصر = تہی;

  /* Free the input and output tokens */
  curlx_safefree(ntlm->داخلہ_token);
  curlx_safefree(ntlm->output_token);

  /* Reset any variables */
  ntlm->token_max = 0;

  curlx_safefree(ntlm->spn);
}

#اگر_ختم /* USE_WINDOWS_SSPI && USE_NTLM */
