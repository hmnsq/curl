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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(USE_WINDOWS_SSPI) && defined(USE_NTLM)

#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "curl_ntlm_core.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "curlx/strdup.h"

/*
 * Curl_auth_is_ntlm_supported()
 *
 * This is used to evaluate if NTLM is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if NTLM is supported by Windows SSPI.
 */
bool Curl_auth_is_ntlm_supported(శూన్య)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_స్థితి స్థితి_2;

  /* Query the security package for NTLM */
  స్థితి_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(పాఠ్యం_2(SP_NAME_NTLM)),
                                     &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ఒకవేళ(స్థితి_2 == SEC_E_OK) {
    Curl_pSecFn->Free_Context_బఫరు(SecurityPackage);
  }

  తిరిగి స్థితి_2 == SEC_E_OK;
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
CURLcode Curl_auth_create_ntlm_type1_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_creds *creds,
                                             స్థిరరాశి అక్షరం *default_service,
                                             స్థిరరాశి అక్షరం *host,
                                             నిర్మాణం ntlmdata *ntlm,
                                             నిర్మాణం bufref *బహిర్గతం)
{
  PSecPkgInfo SecurityPackage;
  Sec_బఫరు type_1_buf;
  Sec_బఫరు_Desc type_1_desc;
  SECURITY_స్థితి స్థితి_2;
  గుర్తులేని దీర్ఘ attrs;
  స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_cleanup_ntlm(ntlm);

  /* Query the security package for NTLM */
  స్థితి_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                     CURL_UNCONST(పాఠ్యం_2(SP_NAME_NTLM)),
                                     &SecurityPackage);
  ఒకవేళ(స్థితి_2 != SEC_E_OK) {
    failf(దత్తాంశం, "SSPI: could not get auth info");
    తిరిగి CURLE_AUTH_ERROR;
  }

  ntlm->token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_బఫరు(SecurityPackage);

  /* Allocate our output buffer */
  ntlm->output_token = curlx_malloc(ntlm->token_max);
  ఒకవేళ(!ntlm->output_token)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(Curl_creds_has_user(creds)) {
    CURLcode ఫలితం;

    /* Populate our identity structure */
    ఫలితం = Curl_create_sspi_తటస్థం(
      creds->user, creds->passwd, &ntlm->తటస్థం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    /* Allow proper cleanup of the identity structure */
    ntlm->p_తటస్థం = &ntlm->తటస్థం;
  }
  లేకపోతే
    /* Use the current Windows user */
    ntlm->p_తటస్థం = ఖాళీ;

  /* Allocate our credentials handle */
  ntlm->credentials = curlx_calloc(1, పరిమాణం(CredHandle));
  ఒకవేళ(!ntlm->credentials)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Acquire our credentials handle */
  స్థితి_2 = Curl_pSecFn->AcquireCredentialsHandle(ఖాళీ,
                                     CURL_UNCONST(పాఠ్యం_2(SP_NAME_NTLM)),
                                     SECPKG_CRED_OUTBOUND, ఖాళీ,
                                     ntlm->p_తటస్థం, ఖాళీ, ఖాళీ,
                                     ntlm->credentials, ఖాళీ);
  ఒకవేళ(స్థితి_2 != SEC_E_OK) {
    curlx_safefree(ntlm->credentials);
    తిరిగి CURLE_LOGIN_DENIED;
  }

  /* Allocate our new context handle */
  ntlm->context = curlx_calloc(1, పరిమాణం(CtxtHandle));
  ఒకవేళ(!ntlm->context)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ntlm->spn = Curl_auth_build_spn(service, host, ఖాళీ);
  ఒకవేళ(!ntlm->spn)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Setup the type-1 "output" security buffer */
  type_1_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;
  type_1_desc.cBuffers  = 1;
  type_1_desc.pBuffers  = &type_1_buf;
  type_1_buf.బఫరు_Type = SECBUFFER_TOKEN;
  type_1_buf.pv_బఫరు   = ntlm->output_token;
  type_1_buf.cb_బఫరు   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-1 message */
  స్థితి_2 = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials, ఖాళీ,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_జాలం_DREP,
                                                  ఖాళీ, 0,
                                                  ntlm->context, &type_1_desc,
                                                  &attrs, ఖాళీ);
  ఒకవేళ(స్థితి_2 == SEC_I_COMPLETE_NEEDED ||
     స్థితి_2 == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(ntlm->context, &type_1_desc);
  లేకపోతే ఒకవేళ(స్థితి_2 == SEC_E_INSUFFICIENT_జ్ఞాపకం)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  లేకపోతే ఒకవేళ(స్థితి_2 != SEC_E_OK && స్థితి_2 != SEC_I_CONTINUE_NEEDED)
    తిరిగి CURLE_AUTH_ERROR;

  /* Return the response. */
  Curl_bufref_సమితి(బహిర్గతం, ntlm->output_token, type_1_buf.cb_బఫరు, ఖాళీ);
  తిరిగి CURLE_OK;
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
CURLcode Curl_auth_decode_ntlm_type2_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             స్థిరరాశి నిర్మాణం bufref *type2ref,
                                             నిర్మాణం ntlmdata *ntlm)
{
  /* Ensure we have a valid type-2 message */
  ఒకవేళ(!Curl_bufref_len(type2ref)) {
    infof(దత్తాంశం, "NTLM handshake failure (empty type-2 message)");
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }

  /* Store the challenge for later use */
  ntlm->ఆదానం_token = curlx_memdup0(Curl_bufref_ptr(type2ref),
                                    Curl_bufref_len(type2ref));
  ఒకవేళ(!ntlm->ఆదానం_token)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  ntlm->ఆదానం_token_len = Curl_bufref_len(type2ref);

  తిరిగి CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_creds *creds,
                                             నిర్మాణం ntlmdata *ntlm,
                                             నిర్మాణం bufref *బహిర్గతం)
{
  CURLcode ఫలితం = CURLE_OK;
  Sec_బఫరు type_2_bufs[2];
  Sec_బఫరు type_3_buf;
  Sec_బఫరు_Desc type_2_desc;
  Sec_బఫరు_Desc type_3_desc;
  SECURITY_స్థితి స్థితి_2;
  గుర్తులేని దీర్ఘ attrs;
  SecPkgContext_Bindings pkgBindings = { 0, ఖాళీ };

  (శూన్య)creds;

  /* Setup the type-2 "input" security buffer */
  type_2_desc.ul_సంస్కరణ     = SECBUFFER_సంస్కరణ;
  type_2_desc.cBuffers      = 1;
  type_2_desc.pBuffers      = &type_2_bufs[0];
  type_2_bufs[0].బఫరు_Type = SECBUFFER_TOKEN;
  type_2_bufs[0].pv_బఫరు   = ntlm->ఆదానం_token;
  type_2_bufs[0].cb_బఫరు   = curlx_uztoul(ntlm->ఆదానం_token_len);

  /* SSL context comes from schannel.
   * When extended protection is used in IIS server,
   * we have to pass a second SecBuffer to the SecBufferDesc
   * otherwise IIS does not pass the authentication (401 response).
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  ఒకవేళ(ntlm->sslContext) {
    స్థితి_2 = Curl_pSecFn->Query_Context_లక్షణాలు(
      ntlm->sslContext,
      SECPKG_ATTR_ENDPOINT_BINDINGS,
      &pkgBindings
    );
    ఒకవేళ(స్థితి_2 == SEC_E_OK) {
      type_2_desc.cBuffers++;
      type_2_bufs[1].బఫరు_Type = SECBUFFER_CHANNEL_BINDINGS;
      type_2_bufs[1].cb_బఫరు = pkgBindings.Bindings_నిడివి;
      type_2_bufs[1].pv_బఫరు = pkgBindings.Bindings;
    }
  }

  /* Setup the type-3 "output" security buffer */
  type_3_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;
  type_3_desc.cBuffers  = 1;
  type_3_desc.pBuffers  = &type_3_buf;
  type_3_buf.బఫరు_Type = SECBUFFER_TOKEN;
  type_3_buf.pv_బఫరు   = ntlm->output_token;
  type_3_buf.cb_బఫరు   = curlx_uztoul(ntlm->token_max);

  /* Generate our type-3 message */
  స్థితి_2 = Curl_pSecFn->InitializeSecurityContext(ntlm->credentials,
                                                  ntlm->context,
                                                  ntlm->spn,
                                                  0, 0, SECURITY_జాలం_DREP,
                                                  &type_2_desc,
                                                  0, ntlm->context,
                                                  &type_3_desc,
                                                  &attrs, ఖాళీ);

  ఒకవేళ(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_బఫరు(pkgBindings.Bindings);

  ఒకవేళ(స్థితి_2 != SEC_E_OK) {
    infof(దత్తాంశం, "NTLM handshake failure (type-3 message): Status=0x%08lx",
          (గుర్తులేని దీర్ఘ)స్థితి_2);

    ఒకవేళ(స్థితి_2 == SEC_E_INSUFFICIENT_జ్ఞాపకం)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    తిరిగి CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  ఫలితం = Curl_bufref_memdup0(బహిర్గతం, ntlm->output_token, type_3_buf.cb_బఫరు);
  Curl_auth_cleanup_ntlm(ntlm);
  తిరిగి ఫలితం;
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
శూన్య Curl_auth_cleanup_ntlm(నిర్మాణం ntlmdata *ntlm)
{
  /* Free our security context */
  ఒకవేళ(ntlm->context) {
    Curl_pSecFn->DeleteSecurityContext(ntlm->context);
    curlx_safefree(ntlm->context);
  }

  /* Free our credentials handle */
  ఒకవేళ(ntlm->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(ntlm->credentials);
    curlx_safefree(ntlm->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_తటస్థం(ntlm->p_తటస్థం);
  ntlm->p_తటస్థం = ఖాళీ;

  /* Free the input and output tokens */
  curlx_safefree(ntlm->ఆదానం_token);
  curlx_safefree(ntlm->output_token);

  /* Reset any variables */
  ntlm->token_max = 0;

  curlx_safefree(ntlm->spn);
}

#అయితే_ముగింపు /* USE_WINDOWS_SSPI && USE_NTLM */
