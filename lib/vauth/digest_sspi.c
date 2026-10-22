/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>.
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
 * RFC2831 DIGEST-MD5 authentication
 *
 ***************************************************************************/
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_DIGEST_AUTH)

#అన్తర్భావ "creds.h"
#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "vauth/digest.h"
#అన్తర్భావ "curlx/multibyte.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "strerror.h"

/*
 * Curl_auth_is_digest_supported()
 *
 * This is used to evaluate if DIGEST is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if DIGEST is supported by Windows SSPI.
 */
bool Curl_auth_is_digest_supported(శూన్య)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_స్థితి స్థితి_2;

  /* Query the security package for Digest */
  స్థితి_2 =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(పాఠ్యం_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ఒకవేళ(స్థితి_2 == SEC_E_OK) {
    Curl_pSecFn->Free_Context_బఫరు(SecurityPackage);
  }

  తిరిగి స్థితి_2 == SEC_E_OK;
}

/*
 * Curl_auth_create_digest_md5_message()
 *
 * This is used to generate an already encoded DIGEST-MD5 response message
 * ready for sending to the recipient.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * chlg    [in]     - The challenge message.
 * userp   [in]     - The username in the format User or Domain\User.
 * passwdp [in]     - The user's password.
 * service [in]     - The service type such as http, smtp, pop or imap.
 * out     [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_digest_md5_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             స్థిరరాశి నిర్మాణం bufref *chlg,
                                             నిర్మాణం Curl_creds *creds,
                                             స్థిరరాశి అక్షరం *default_service,
                                             నిర్మాణం bufref *బహిర్గతం)
{
  CURLcode ఫలితం = CURLE_OK;
  TCHAR *spn = ఖాళీ;
  size_t token_max = 0;
  గుర్తులేని అక్షరం *output_token = ఖాళీ;
  CredHandle credentials;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  SEC_WINNT_AUTH_తటస్థం_EX తటస్థం;
  SEC_WINNT_AUTH_తటస్థం_EX *p_తటస్థం;
  Sec_బఫరు chlg_buf;
  Sec_బఫరు resp_buf;
  Sec_బఫరు_Desc chlg_desc;
  Sec_బఫరు_Desc resp_desc;
  SECURITY_స్థితి స్థితి_2;
  గుర్తులేని దీర్ఘ attrs;
  స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Ensure we have a valid challenge message */
  ఒకవేళ(!Curl_bufref_len(chlg)) {
    infof(దత్తాంశం, "DIGEST-MD5 handshake failure (empty challenge message)");
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }

  /* Query the security package for DigestSSP */
  స్థితి_2 =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(పాఠ్యం_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  ఒకవేళ(స్థితి_2 != SEC_E_OK) {
    failf(దత్తాంశం, "SSPI: could not get auth info");
    తిరిగి CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_బఫరు(SecurityPackage);

  /* Allocate our response buffer */
  output_token = curlx_malloc(token_max);
  ఒకవేళ(!output_token)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, దత్తాంశం->state.origin->hostname, ఖాళీ);
  ఒకవేళ(!spn) {
    curlx_free(output_token);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(Curl_creds_has_user(creds)) {
    /* Populate our identity structure */
    ఫలితం = Curl_create_sspi_తటస్థం(creds->user, creds->passwd,
                                       &తటస్థం);
    ఒకవేళ(ఫలితం) {
      curlx_free(spn);
      curlx_free(output_token);
      తిరిగి ఫలితం;
    }

    /* Allow proper cleanup of the identity structure */
    p_తటస్థం = &తటస్థం;
  }
  లేకపోతే
    /* Use the current Windows user */
    p_తటస్థం = ఖాళీ;

  /* Acquire our credentials handle */
  స్థితి_2 = Curl_pSecFn->AcquireCredentialsHandle(ఖాళీ,
                                   CURL_UNCONST(పాఠ్యం_2(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ఖాళీ,
                                   p_తటస్థం, ఖాళీ, ఖాళీ,
                                   &credentials, ఖాళీ);

  ఒకవేళ(స్థితి_2 != SEC_E_OK) {
    Curl_sspi_free_తటస్థం(p_తటస్థం);
    curlx_free(spn);
    curlx_free(output_token);
    తిరిగి CURLE_LOGIN_DENIED;
  }

  /* Setup the challenge "input" security buffer */
  chlg_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;
  chlg_desc.cBuffers  = 1;
  chlg_desc.pBuffers  = &chlg_buf;
  chlg_buf.బఫరు_Type = SECBUFFER_TOKEN;
  chlg_buf.pv_బఫరు   = CURL_UNCONST(Curl_bufref_ptr(chlg));
  chlg_buf.cb_బఫరు   = curlx_uztoul(Curl_bufref_len(chlg));

  /* Setup the response "output" security buffer */
  resp_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.బఫరు_Type = SECBUFFER_TOKEN;
  resp_buf.pv_బఫరు   = output_token;
  resp_buf.cb_బఫరు   = curlx_uztoul(token_max);

  /* Generate our response message */
  స్థితి_2 = Curl_pSecFn->InitializeSecurityContext(&credentials, ఖాళీ, spn,
                                                  0, 0, 0, &chlg_desc, 0,
                                                  &context, &resp_desc, &attrs,
                                                  ఖాళీ);

  ఒకవేళ(స్థితి_2 == SEC_I_COMPLETE_NEEDED ||
     స్థితి_2 == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
  లేకపోతే ఒకవేళ(స్థితి_2 != SEC_E_OK && స్థితి_2 != SEC_I_CONTINUE_NEEDED) {
    VERBOSE(అక్షరం బఫరు[STRERROR_LEN]);

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_తటస్థం(p_తటస్థం);
    curlx_free(spn);
    curlx_free(output_token);

    ఒకవేళ(స్థితి_2 == SEC_E_INSUFFICIENT_జ్ఞాపకం)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    infof(దత్తాంశం, "schannel: InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(స్థితి_2, బఫరు, పరిమాణం(బఫరు)));

    తిరిగి CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  Curl_bufref_సమితి(బహిర్గతం, output_token, resp_buf.cb_బఫరు, curl_free);

  /* Free our handles */
  Curl_pSecFn->DeleteSecurityContext(&context);
  Curl_pSecFn->FreeCredentialsHandle(&credentials);

  /* Free the identity structure */
  Curl_sspi_free_తటస్థం(p_తటస్థం);

  /* Free the SPN */
  curlx_free(spn);

  తిరిగి ఫలితం;
}

/*
 * Curl_override_sspi_http_realm()
 *
 * This is used to populate the domain in an SSPI identity structure
 * The realm is extracted from the challenge message and used as the
 * domain if it is not already explicitly set.
 *
 * Parameters:
 *
 * chlg     [in]     - The challenge message.
 * identity [in/out] - The identity structure.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_override_sspi_http_realm(స్థిరరాశి అక్షరం *chlg,
                                       SEC_WINNT_AUTH_తటస్థం_EX *తటస్థం)
{
  xcharp_u domain, dup_domain;

  /* If domain is blank or unset, check challenge message for realm */
  ఒకవేళ(!తటస్థం->Domain || !తటస్థం->Domain_నిడివి) {
    కొరకు(;;) {
      నిర్మాణం Curl_str విలువ;
      అక్షరం content[DIGEST_MAX_CONTENT_నిడివి];

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Extract a value=content pair */
      ఒకవేళ(Curl_auth_digest_get_pair(chlg, &విలువ, content, &chlg)) {
        ఒకవేళ(curlx_str_casecompare(&విలువ, "realm")) {

          /* Setup identity's domain and length */
          domain.tchar_ptr = curlx_convert_UTF8_గమ్యం_tchar(content);
          ఒకవేళ(!domain.tchar_ptr)
            తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

          dup_domain.tchar_ptr = curlx_tcsdup(domain.tchar_ptr);
          ఒకవేళ(!dup_domain.tchar_ptr) {
            curlx_free(domain.tchar_ptr);
            తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
          }

          curlx_free(తటస్థం->Domain);
          తటస్థం->Domain = dup_domain.tbyte_ptr;
          తటస్థం->Domain_నిడివి = curlx_uztoul(_tcslen(dup_domain.tchar_ptr));
          dup_domain.tchar_ptr = ఖాళీ;

          curlx_free(domain.tchar_ptr);
        }
        లేకపోతే {
          /* Unknown specifier, ignore it! */
        }
      }
      లేకపోతే
        ఆపు; /* We are done here */

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Allow the list to be comma-separated */
      (శూన్య)curlx_str_single(&chlg, ',');
    }
  }

  తిరిగి CURLE_OK;
}

/*
 * Curl_auth_decode_digest_http_message()
 *
 * This is used to decode an HTTP DIGEST challenge message into the separate
 * attributes.
 *
 * Parameters:
 *
 * chlg    [in]     - The challenge message.
 * digest  [in/out] - The digest data struct being used and modified.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_decode_digest_http_సందేశం(స్థిరరాశి అక్షరం *chlg,
                                              నిర్మాణం digestdata *digest)
{
  size_t chlglen = అక్షరశ్రేణి_పొడవు(chlg);

  /* We had an input token before so if there is another one now that means we
     provided bad credentials in the previous request or it is stale. */
  ఒకవేళ(digest->ఆదానం_token) {
    bool stale = FALSE;
    స్థిరరాశి అక్షరం *p = chlg;

    /* Check for the 'stale' directive */
    కొరకు(;;) {
      నిర్మాణం Curl_str విలువ;
      అక్షరం content[DIGEST_MAX_CONTENT_నిడివి];

      అయితే(*p && ISBLANK(*p))
        p++;

      ఒకవేళ(!Curl_auth_digest_get_pair(p, &విలువ, content, &p))
        ఆపు;

      ఒకవేళ(curlx_str_casecompare(&విలువ, "stale") &&
         curl_strequal(content, "true")) {
        stale = TRUE;
        ఆపు;
      }

      అయితే(*p && ISBLANK(*p))
        p++;

      ఒకవేళ(',' == *p)
        p++;
    }

    ఒకవేళ(stale)
      Curl_auth_digest_cleanup(digest);
    లేకపోతే
      తిరిగి CURLE_LOGIN_DENIED;
  }

  /* Store the challenge for use later */
  digest->ఆదానం_token = (BYTE *)curlx_memdup(chlg, chlglen + 1);
  ఒకవేళ(!digest->ఆదానం_token)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  digest->ఆదానం_token_len = chlglen;

  తిరిగి CURLE_OK;
}

/*
 * Curl_auth_create_digest_http_message()
 *
 * This is used to generate an HTTP DIGEST response message ready for sending
 * to the recipient.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * userp   [in]     - The username in the format User or Domain\User.
 * passwdp [in]     - The user's password.
 * request [in]     - The HTTP request.
 * uripath [in]     - The path of the HTTP uri.
 * digest  [in/out] - The digest data struct being used and modified.
 * outptr  [in/out] - The address where a pointer to newly allocated memory
 *                    holding the result is stored upon completion.
 * outlen  [out]    - The length of the output message.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_digest_http_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                              నిర్మాణం Curl_creds *creds,
                                              స్థిరరాశి గుర్తులేని అక్షరం *request,
                                              స్థిరరాశి గుర్తులేని అక్షరం *uripath,
                                              నిర్మాణం digestdata *digest,
                                              అక్షరం **outptr, size_t *outlen)
{
  size_t token_max;
  అక్షరం *resp;
  BYTE *output_token;
  size_t output_token_len = 0;
  PSecPkgInfo SecurityPackage;
  Sec_బఫరు chlg_buf[5];
  Sec_బఫరు_Desc chlg_desc;
  SECURITY_స్థితి స్థితి_2;

  /* Query the security package for DigestSSP */
  స్థితి_2 =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(పాఠ్యం_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  ఒకవేళ(స్థితి_2 != SEC_E_OK) {
    failf(దత్తాంశం, "SSPI: could not get auth info");
    తిరిగి CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_బఫరు(SecurityPackage);

  /* Allocate the output buffer according to the max token size as indicated
     by the security package */
  output_token = curlx_malloc(token_max);
  ఒకవేళ(!output_token) {
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  /* If the user/passwd that was used to make the identity for http_context
     has changed then delete that context. */
  ఒకవేళ(!Curl_creds_same(creds, digest->creds)) {
    ఒకవేళ(digest->http_context) {
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
    Curl_creds_unlink(&digest->creds);
  }

  ఒకవేళ(digest->http_context) {
    chlg_desc.ul_సంస్కరణ    = SECBUFFER_సంస్కరణ;
    chlg_desc.cBuffers     = 5;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].బఫరు_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_బఫరు   = ఖాళీ;
    chlg_buf[0].cb_బఫరు   = 0;
    chlg_buf[1].బఫరు_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_బఫరు   = CURL_UNCONST(request);
    chlg_buf[1].cb_బఫరు   = curlx_uztoul(అక్షరశ్రేణి_పొడవు((స్థిరరాశి అక్షరం *)request));
    chlg_buf[2].బఫరు_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_బఫరు   = CURL_UNCONST(uripath);
    chlg_buf[2].cb_బఫరు   = curlx_uztoul(అక్షరశ్రేణి_పొడవు((స్థిరరాశి అక్షరం *)uripath));
    chlg_buf[3].బఫరు_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[3].pv_బఫరు   = ఖాళీ;
    chlg_buf[3].cb_బఫరు   = 0;
    chlg_buf[4].బఫరు_Type = SECBUFFER_PADDING;
    chlg_buf[4].pv_బఫరు   = output_token;
    chlg_buf[4].cb_బఫరు   = curlx_uztoul(token_max);

    స్థితి_2 = Curl_pSecFn->MakeSignature(digest->http_context, 0, &chlg_desc,
                                        0);
    ఒకవేళ(స్థితి_2 == SEC_E_OK)
      output_token_len = chlg_buf[4].cb_బఫరు;
    లేకపోతే { /* delete the context so a new one can be made */
      infof(దత్తాంశం, "digest_sspi: MakeSignature failed, error 0x%08lx",
            (గుర్తులేని దీర్ఘ)స్థితి_2);
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
  }

  ఒకవేళ(!digest->http_context) {
    CredHandle credentials;
    SEC_WINNT_AUTH_తటస్థం_EX తటస్థం;
    SEC_WINNT_AUTH_తటస్థం_EX *p_తటస్థం;
    Sec_బఫరు resp_buf;
    Sec_బఫరు_Desc resp_desc;
    గుర్తులేని దీర్ఘ attrs;
    TCHAR *spn;

    /* free the credentials used to make the previous identity */
    Curl_creds_unlink(&digest->creds);

    ఒకవేళ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ఒకవేళ(Curl_create_sspi_తటస్థం(creds->user, creds->passwd, &తటస్థం)) {
        curlx_free(output_token);
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }

      /* Populate our identity domain */
      ఒకవేళ(Curl_override_sspi_http_realm((స్థిరరాశి అక్షరం *)digest->ఆదానం_token,
                                       &తటస్థం)) {
        Curl_sspi_free_తటస్థం(&తటస్థం);
        curlx_free(output_token);
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }

      /* Allow proper cleanup of the identity structure */
      p_తటస్థం = &తటస్థం;
    }
    లేకపోతే
      /* Use the current Windows user */
      p_తటస్థం = ఖాళీ;

    ఒకవేళ(creds)
      Curl_creds_link(&digest->creds, creds);

    /* Acquire our credentials handle */
    స్థితి_2 = Curl_pSecFn->AcquireCredentialsHandle(ఖాళీ,
                                   CURL_UNCONST(పాఠ్యం_2(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ఖాళీ,
                                   p_తటస్థం, ఖాళీ, ఖాళీ,
                                   &credentials, ఖాళీ);
    ఒకవేళ(స్థితి_2 != SEC_E_OK) {
      Curl_sspi_free_తటస్థం(p_తటస్థం);
      curlx_free(output_token);

      తిరిగి CURLE_LOGIN_DENIED;
    }

    /* Setup the challenge "input" security buffer if present */
    chlg_desc.ul_సంస్కరణ    = SECBUFFER_సంస్కరణ;
    chlg_desc.cBuffers     = 3;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].బఫరు_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_బఫరు   = digest->ఆదానం_token;
    chlg_buf[0].cb_బఫరు   = curlx_uztoul(digest->ఆదానం_token_len);
    chlg_buf[1].బఫరు_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_బఫరు   = CURL_UNCONST(request);
    chlg_buf[1].cb_బఫరు   = curlx_uztoul(అక్షరశ్రేణి_పొడవు((స్థిరరాశి అక్షరం *)request));
    chlg_buf[2].బఫరు_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_బఫరు   = ఖాళీ;
    chlg_buf[2].cb_బఫరు   = 0;

    /* Setup the response "output" security buffer */
    resp_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;
    resp_desc.cBuffers  = 1;
    resp_desc.pBuffers  = &resp_buf;
    resp_buf.బఫరు_Type = SECBUFFER_TOKEN;
    resp_buf.pv_బఫరు   = output_token;
    resp_buf.cb_బఫరు   = curlx_uztoul(token_max);

    spn = curlx_convert_UTF8_గమ్యం_tchar((స్థిరరాశి అక్షరం *)uripath);
    ఒకవేళ(!spn) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_తటస్థం(p_తటస్థం);
      curlx_free(output_token);

      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    /* Allocate our new context handle */
    digest->http_context = curlx_calloc(1, పరిమాణం(CtxtHandle));
    ఒకవేళ(!digest->http_context) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);
      curlx_free(spn);
      Curl_sspi_free_తటస్థం(p_తటస్థం);
      curlx_free(output_token);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    /* Generate our response message */
    స్థితి_2 = Curl_pSecFn->InitializeSecurityContext(&credentials, ఖాళీ,
                                                    spn,
                                                    ISC_REQ_USE_HTTP_STYLE,
                                                    0, 0,
                                                    &chlg_desc, 0,
                                                    digest->http_context,
                                                    &resp_desc, &attrs, ఖాళీ);
    curlx_free(spn);

    ఒకవేళ(స్థితి_2 == SEC_I_COMPLETE_NEEDED ||
       స్థితి_2 == SEC_I_COMPLETE_AND_CONTINUE)
      Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
    లేకపోతే ఒకవేళ(స్థితి_2 != SEC_E_OK && స్థితి_2 != SEC_I_CONTINUE_NEEDED) {
      VERBOSE(అక్షరం బఫరు[STRERROR_LEN]);

      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_తటస్థం(p_తటస్థం);
      curlx_free(output_token);

      curlx_safefree(digest->http_context);

      ఒకవేళ(స్థితి_2 == SEC_E_INSUFFICIENT_జ్ఞాపకం)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      infof(దత్తాంశం, "schannel: InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(స్థితి_2, బఫరు, పరిమాణం(బఫరు)));

      తిరిగి CURLE_AUTH_ERROR;
    }

    output_token_len = resp_buf.cb_బఫరు;

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_తటస్థం(p_తటస్థం);
  }

  resp = curlx_memdup0((స్థిరరాశి అక్షరం *)output_token, output_token_len);
  curlx_free(output_token);
  ఒకవేళ(!resp) {
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  /* Return the response */
  *outptr = resp;
  *outlen = output_token_len;
  తిరిగి CURLE_OK;
}

/*
 * Curl_auth_digest_cleanup()
 *
 * This is used to clean up the digest specific data.
 *
 * Parameters:
 *
 * digest    [in/out] - The digest data struct being cleaned up.
 *
 */
శూన్య Curl_auth_digest_cleanup(నిర్మాణం digestdata *digest)
{
  /* Free the input token */
  curlx_safefree(digest->ఆదానం_token);

  /* Reset any variables */
  digest->ఆదానం_token_len = 0;

  /* Delete security context */
  ఒకవేళ(digest->http_context) {
    Curl_pSecFn->DeleteSecurityContext(digest->http_context);
    curlx_safefree(digest->http_context);
  }

  /* Free the copy of user/passwd used to make the identity for http_context */
  Curl_creds_unlink(&digest->creds);
  Curl_peer_unlink(&digest->origin);
}

#అయితే_ముగింపు /* USE_WINDOWS_SSPI && !CURL_DISABLE_DIGEST_AUTH */
