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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(USE_WINDOWS_SSPI) && defined(USE_SPNEGO)

#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "strerror.h"

/*
 * Curl_auth_is_spnego_supported()
 *
 * This is used to evaluate if SPNEGO (Negotiate) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Negotiate is supported by Windows SSPI.
 */
bool Curl_auth_is_spnego_supported(శూన్య)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_స్థితి స్థితి_2;

  /* Query the security package for Negotiate */
  స్థితి_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(పాఠ్యం_2(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ఒకవేళ(స్థితి_2 == SEC_E_OK) {
    Curl_pSecFn->Free_Context_బఫరు(SecurityPackage);
  }

  తిరిగి స్థితి_2 == SEC_E_OK;
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
CURLcode Curl_auth_decode_spnego_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం Curl_creds *creds,
                                         స్థిరరాశి అక్షరం *default_service,
                                         స్థిరరాశి అక్షరం *host,
                                         స్థిరరాశి అక్షరం *chlg64,
                                         నిర్మాణం negotiatedata *nego)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t chlglen = 0;
  గుర్తులేని అక్షరం *chlg = ఖాళీ;
  PSecPkgInfo SecurityPackage;
  Sec_బఫరు chlg_buf[2];
  Sec_బఫరు resp_buf;
  Sec_బఫరు_Desc chlg_desc;
  Sec_బఫరు_Desc resp_desc;
  గుర్తులేని దీర్ఘ attrs;
  SecPkgContext_Bindings pkgBindings = { 0, ఖాళీ };

  ఒకవేళ(nego->context && nego->స్థితి_2 == SEC_E_OK) {
    /* We finished successfully our part of authentication, but server
     * rejected it (since we are again here). Exit with an error since we
     * cannot invent anything better */
    Curl_auth_cleanup_spnego(nego);
    తిరిగి CURLE_LOGIN_DENIED;
  }

  ఒకవేళ(!nego->spn) {
    /* Generate our SPN */
    స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(creds) ?
      Curl_creds_sasl_service(creds) : default_service;
    nego->spn = Curl_auth_build_spn(service, host, ఖాళీ);
    ఒకవేళ(!nego->spn)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(!nego->output_token) {
    /* Query the security package for Negotiate */
    nego->స్థితి_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                CURL_UNCONST(పాఠ్యం_2(SP_NAME_NEGOTIATE)),
                                &SecurityPackage);
    ఒకవేళ(nego->స్థితి_2 != SEC_E_OK) {
      failf(దత్తాంశం, "SSPI: could not get auth info");
      తిరిగి CURLE_AUTH_ERROR;
    }

    nego->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_బఫరు(SecurityPackage);

    /* Allocate our output buffer */
    nego->output_token = curlx_malloc(nego->token_max);
    ఒకవేళ(!nego->output_token)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(!nego->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    ఒకవేళ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ఫలితం = Curl_create_sspi_తటస్థం(creds->user, creds->passwd,
                                         &nego->తటస్థం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      /* Allow proper cleanup of the identity structure */
      nego->p_తటస్థం = &nego->తటస్థం;
    }
    లేకపోతే
      /* Use the current Windows user */
      nego->p_తటస్థం = ఖాళీ;

    /* Exclude NTLM from SPNEGO negotiation via the PackageList field */
    ఒకవేళ(!nego->p_తటస్థం) {
      memset(&nego->తటస్థం, 0, పరిమాణం(nego->తటస్థం));
      nego->తటస్థం.సంస్కరణ = SEC_WINNT_AUTH_తటస్థం_సంస్కరణ;
      nego->తటస్థం.నిడివి_2 = పరిమాణం(nego->తటస్థం);
      nego->తటస్థం.Flags = CURL_SEC_WINNT_AUTH_తటస్థం;
      nego->p_తటస్థం = &nego->తటస్థం;
    }

    /* Use the special name "!ntlm" to prevent NTLM from being used:
     * https://learn.microsoft.com/windows/win32/api/sspi/ns-sspi-sec_winnt_auth_identity_exa
     */
    nego->తటస్థం.PackageList = CURL_UNCONST(పాఠ్యం_2("!ntlm"));
    nego->తటస్థం.Package_List_నిడివి = 5;

    /* Allocate our credentials handle */
    nego->credentials = curlx_calloc(1, పరిమాణం(CredHandle));
    ఒకవేళ(!nego->credentials)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    /* Acquire our credentials handle */
    nego->స్థితి_2 = Curl_pSecFn->AcquireCredentialsHandle(ఖాళీ,
                                CURL_UNCONST(పాఠ్యం_2(SP_NAME_NEGOTIATE)),
                                SECPKG_CRED_OUTBOUND, ఖాళీ,
                                nego->p_తటస్థం, ఖాళీ, ఖాళీ,
                                nego->credentials, ఖాళీ);
    ఒకవేళ(nego->స్థితి_2 != SEC_E_OK) {
      curlx_safefree(nego->credentials);
      తిరిగి CURLE_AUTH_ERROR;
    }

    /* Allocate our new context handle */
    nego->context = curlx_calloc(1, పరిమాణం(CtxtHandle));
    ఒకవేళ(!nego->context)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  chlg_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;
  chlg_desc.cBuffers = 0;
  chlg_desc.pBuffers = chlg_buf;

  ఒకవేళ(chlg64 && *chlg64) {
    /* Decode the base-64 encoded challenge message */
    ఒకవేళ(*chlg64 != '=') {
      ఫలితం = curlx_base64_decode(chlg64, &chlg, &chlglen);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }

    /* Ensure we have a valid challenge message */
    ఒకవేళ(!chlg) {
      infof(దత్తాంశం, "SPNEGO handshake failure (empty challenge message)");
      తిరిగి CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.cBuffers     = 1;
    chlg_buf[0].బఫరు_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_బఫరు   = chlg;
    chlg_buf[0].cb_బఫరు   = curlx_uztoul(chlglen);
  }

  /* SSL context comes from Schannel.
   * When extended protection is used in IIS server, pass its channel
   * bindings on the initial call too. HTTP Negotiate can create and send a
   * Kerberos token before receiving a challenge from the server.
   * Minimum supported version is Windows 7.
   * https://learn.microsoft.com/security-updates/SecurityAdvisories/2009/973811
   */
  ఒకవేళ(nego->sslContext) {
    nego->స్థితి_2 = Curl_pSecFn->Query_Context_లక్షణాలు(
        nego->sslContext,
        SECPKG_ATTR_ENDPOINT_BINDINGS,
        &pkgBindings);
    ఒకవేళ(nego->స్థితి_2 == SEC_E_OK) {
      Sec_బఫరు *binding_buf = &chlg_buf[chlg_desc.cBuffers++];
      binding_buf->బఫరు_Type = SECBUFFER_CHANNEL_BINDINGS;
      binding_buf->cb_బఫరు   = pkgBindings.Bindings_నిడివి;
      binding_buf->pv_బఫరు   = pkgBindings.Bindings;
    }
  }

  /* Setup the response "output" security buffer */
  resp_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.బఫరు_Type = SECBUFFER_TOKEN;
  resp_buf.pv_బఫరు   = nego->output_token;
  resp_buf.cb_బఫరు   = curlx_uztoul(nego->token_max);

  /* Generate our challenge-response message */
  {
    DWORD sspi_flags = ISC_REQ_CONFIDENTIALITY;
    ఒకవేళ(దత్తాంశం->సమితి.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
      sspi_flags |= ISC_REQ_DELEGATE | ISC_REQ_MUTUAL_AUTH;
    nego->స్థితి_2 =
      Curl_pSecFn->InitializeSecurityContext(nego->credentials,
                                             chlg ? nego->context : ఖాళీ,
                                             nego->spn,
                                             sspi_flags,
                                             0, SECURITY_NATIVE_DREP,
                                             chlg_desc.cBuffers ?
                                               &chlg_desc : ఖాళీ,
                                             0, nego->context,
                                             &resp_desc, &attrs, ఖాళీ);
  }

  ఒకవేళ(pkgBindings.Bindings)
    Curl_pSecFn->Free_Context_బఫరు(pkgBindings.Bindings);

  /* Free the decoded challenge as it is not required anymore */
  curlx_free(chlg);

  ఒకవేళ(GSS_ERROR(nego->స్థితి_2)) {
    అక్షరం బఫరు[STRERROR_LEN];
    failf(దత్తాంశం, "InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(nego->స్థితి_2, బఫరు, పరిమాణం(బఫరు)));

    ఒకవేళ(nego->స్థితి_2 == SEC_E_INSUFFICIENT_జ్ఞాపకం)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    తిరిగి CURLE_AUTH_ERROR;
  }

  ఒకవేళ(nego->స్థితి_2 == SEC_I_COMPLETE_NEEDED ||
     nego->స్థితి_2 == SEC_I_COMPLETE_AND_CONTINUE) {
    nego->స్థితి_2 = Curl_pSecFn->CompleteAuthToken(nego->context, &resp_desc);
    ఒకవేళ(GSS_ERROR(nego->స్థితి_2)) {
      అక్షరం బఫరు[STRERROR_LEN];
      failf(దత్తాంశం, "CompleteAuthToken failed: %s",
            Curl_sspi_strerror(nego->స్థితి_2, బఫరు, పరిమాణం(బఫరు)));

      ఒకవేళ(nego->స్థితి_2 == SEC_E_INSUFFICIENT_జ్ఞాపకం)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      తిరిగి CURLE_AUTH_ERROR;
    }
  }

  nego->output_token_నిడివి = resp_buf.cb_బఫరు;

  తిరిగి ఫలితం;
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
CURLcode Curl_auth_create_spnego_సందేశం(నిర్మాణం negotiatedata *nego,
                                         అక్షరం **outptr, size_t *outlen)
{
  /* Base64 encode the already generated response */
  CURLcode ఫలితం = curlx_base64_encode(nego->output_token,
                                        nego->output_token_నిడివి, outptr,
                                        outlen);
  ఒకవేళ(!ఫలితం && (!*outptr || !*outlen)) {
    curlx_free(*outptr);
    ఫలితం = CURLE_REMOTE_ACCESS_DENIED;
  }

  తిరిగి ఫలితం;
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
శూన్య Curl_auth_cleanup_spnego(నిర్మాణం negotiatedata *nego)
{
  /* Free our security context */
  ఒకవేళ(nego->context) {
    Curl_pSecFn->DeleteSecurityContext(nego->context);
    curlx_safefree(nego->context);
  }

  /* Free our credentials handle */
  ఒకవేళ(nego->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(nego->credentials);
    curlx_safefree(nego->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_తటస్థం(nego->p_తటస్థం);
  nego->p_తటస్థం = ఖాళీ;

  /* Free the SPN and output token */
  curlx_safefree(nego->spn);
  curlx_safefree(nego->output_token);

  /* Reset any variables */
  nego->స్థితి_2 = 0;
  nego->token_max = 0;
  nego->noauthpersist = FALSE;
  nego->havenoauthpersist = FALSE;
  nego->havenegdata = FALSE;
  nego->havemultiplerequests = FALSE;
}

#అయితే_ముగింపు /* USE_WINDOWS_SSPI && USE_SPNEGO */
