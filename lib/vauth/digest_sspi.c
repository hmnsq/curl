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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_DIGEST_AUTH)

#ಅನ್ತರ್ಭಾವ "creds.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "vauth/digest.h"
#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "strerror.h"

/*
 * Curl_auth_is_digest_supported()
 *
 * This is used to evaluate if DIGEST is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if DIGEST is supported by Windows SSPI.
 */
bool Curl_auth_is_digest_supported(ಶೂನ್ಯ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;

  /* Query the security package for Digest */
  ಸ್ಥಿತಿ_2 =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_OK) {
    Curl_pSecFn->Free_Context_ಬಫರ್(SecurityPackage);
  }

  ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2 == SEC_E_OK;
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
CURLcode Curl_auth_create_digest_md5_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                                             ರಚನೆ Curl_creds *creds,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                             ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  TCHAR *spn = ಖಾಲಿ;
  size_t token_max = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *output_token = ಖಾಲಿ;
  CredHandle credentials;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  SEC_WINNT_AUTH_ತಟಸ್ಥ_EX ತಟಸ್ಥ;
  SEC_WINNT_AUTH_ತಟಸ್ಥ_EX *p_ತಟಸ್ಥ;
  Sec_ಬಫರ್ chlg_buf;
  Sec_ಬಫರ್ resp_buf;
  Sec_ಬಫರ್_Desc chlg_desc;
  Sec_ಬಫರ್_Desc resp_desc;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ attrs;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Ensure we have a valid challenge message */
  ಆದರೆ(!Curl_bufref_len(chlg)) {
    infof(ದತ್ತಾಂಶ, "DIGEST-MD5 handshake failure (empty challenge message)");
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Query the security package for DigestSSP */
  ಸ್ಥಿತಿ_2 =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK) {
    failf(ದತ್ತಾಂಶ, "SSPI: could not get auth info");
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_ಬಫರ್(SecurityPackage);

  /* Allocate our response buffer */
  output_token = curlx_malloc(token_max);
  ಆದರೆ(!output_token)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, ದತ್ತಾಂಶ->state.origin->hostname, ಖಾಲಿ);
  ಆದರೆ(!spn) {
    curlx_free(output_token);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(Curl_creds_has_user(creds)) {
    /* Populate our identity structure */
    ಫಲಿತಾಂಶ = Curl_create_sspi_ತಟಸ್ಥ(creds->user, creds->passwd,
                                       &ತಟಸ್ಥ);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      curlx_free(spn);
      curlx_free(output_token);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    /* Allow proper cleanup of the identity structure */
    p_ತಟಸ್ಥ = &ತಟಸ್ಥ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* Use the current Windows user */
    p_ತಟಸ್ಥ = ಖಾಲಿ;

  /* Acquire our credentials handle */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->AcquireCredentialsHandle(ಖಾಲಿ,
                                   CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ಖಾಲಿ,
                                   p_ತಟಸ್ಥ, ಖಾಲಿ, ಖಾಲಿ,
                                   &credentials, ಖಾಲಿ);

  ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK) {
    Curl_sspi_free_ತಟಸ್ಥ(p_ತಟಸ್ಥ);
    curlx_free(spn);
    curlx_free(output_token);
    ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
  }

  /* Setup the challenge "input" security buffer */
  chlg_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;
  chlg_desc.cBuffers  = 1;
  chlg_desc.pBuffers  = &chlg_buf;
  chlg_buf.ಬಫರ್_Type = SECBUFFER_TOKEN;
  chlg_buf.pv_ಬಫರ್   = CURL_UNCONST(Curl_bufref_ptr(chlg));
  chlg_buf.cb_ಬಫರ್   = curlx_uztoul(Curl_bufref_len(chlg));

  /* Setup the response "output" security buffer */
  resp_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.ಬಫರ್_Type = SECBUFFER_TOKEN;
  resp_buf.pv_ಬಫರ್   = output_token;
  resp_buf.cb_ಬಫರ್   = curlx_uztoul(token_max);

  /* Generate our response message */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->InitializeSecurityContext(&credentials, ಖಾಲಿ, spn,
                                                  0, 0, 0, &chlg_desc, 0,
                                                  &context, &resp_desc, &attrs,
                                                  ಖಾಲಿ);

  ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_I_COMPLETE_NEEDED ||
     ಸ್ಥಿತಿ_2 == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK && ಸ್ಥಿತಿ_2 != SEC_I_CONTINUE_NEEDED) {
    VERBOSE(ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN]);

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_ತಟಸ್ಥ(p_ತಟಸ್ಥ);
    curlx_free(spn);
    curlx_free(output_token);

    ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    infof(ದತ್ತಾಂಶ, "schannel: InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(ಸ್ಥಿತಿ_2, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));

    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  Curl_bufref_ಗಣ(ಹೊರಹೋಗು, output_token, resp_buf.cb_ಬಫರ್, curl_free);

  /* Free our handles */
  Curl_pSecFn->DeleteSecurityContext(&context);
  Curl_pSecFn->FreeCredentialsHandle(&credentials);

  /* Free the identity structure */
  Curl_sspi_free_ತಟಸ್ಥ(p_ತಟಸ್ಥ);

  /* Free the SPN */
  curlx_free(spn);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
CURLcode Curl_override_sspi_http_realm(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chlg,
                                       SEC_WINNT_AUTH_ತಟಸ್ಥ_EX *ತಟಸ್ಥ)
{
  xcharp_u domain, dup_domain;

  /* If domain is blank or unset, check challenge message for realm */
  ಆದರೆ(!ತಟಸ್ಥ->Domain || !ತಟಸ್ಥ->Domain_ಉದ್ದಳತೆ) {
    ಗಾಗಿ(;;) {
      ರಚನೆ Curl_str ಮೌಲ್ಯ;
      ಅಕ್ಷರ content[DIGEST_MAX_CONTENT_ಉದ್ದಳತೆ];

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Extract a value=content pair */
      ಆದರೆ(Curl_auth_digest_get_pair(chlg, &ಮೌಲ್ಯ, content, &chlg)) {
        ಆದರೆ(curlx_str_casecompare(&ಮೌಲ್ಯ, "realm")) {

          /* Setup identity's domain and length */
          domain.tchar_ptr = curlx_convert_UTF8_ಗಮ್ಯ_tchar(content);
          ಆದರೆ(!domain.tchar_ptr)
            ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

          dup_domain.tchar_ptr = curlx_tcsdup(domain.tchar_ptr);
          ಆದರೆ(!dup_domain.tchar_ptr) {
            curlx_free(domain.tchar_ptr);
            ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
          }

          curlx_free(ತಟಸ್ಥ->Domain);
          ತಟಸ್ಥ->Domain = dup_domain.tbyte_ptr;
          ತಟಸ್ಥ->Domain_ಉದ್ದಳತೆ = curlx_uztoul(_tcslen(dup_domain.tchar_ptr));
          dup_domain.tchar_ptr = ಖಾಲಿ;

          curlx_free(domain.tchar_ptr);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          /* Unknown specifier, ignore it! */
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ನಿಲ್ಲಿಸು; /* We are done here */

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Allow the list to be comma-separated */
      (ಶೂನ್ಯ)curlx_str_single(&chlg, ',');
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
CURLcode Curl_auth_decode_digest_http_ಸಂದೇಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chlg,
                                              ರಚನೆ digestdata *digest)
{
  size_t chlglen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(chlg);

  /* We had an input token before so if there is another one now that means we
     provided bad credentials in the previous request or it is stale. */
  ಆದರೆ(digest->ಒಳಹರಿವು_token) {
    bool stale = FALSE;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = chlg;

    /* Check for the 'stale' directive */
    ಗಾಗಿ(;;) {
      ರಚನೆ Curl_str ಮೌಲ್ಯ;
      ಅಕ್ಷರ content[DIGEST_MAX_CONTENT_ಉದ್ದಳತೆ];

      ಆಗ(*p && ISBLANK(*p))
        p++;

      ಆದರೆ(!Curl_auth_digest_get_pair(p, &ಮೌಲ್ಯ, content, &p))
        ನಿಲ್ಲಿಸು;

      ಆದರೆ(curlx_str_casecompare(&ಮೌಲ್ಯ, "stale") &&
         curl_strequal(content, "true")) {
        stale = TRUE;
        ನಿಲ್ಲಿಸು;
      }

      ಆಗ(*p && ISBLANK(*p))
        p++;

      ಆದರೆ(',' == *p)
        p++;
    }

    ಆದರೆ(stale)
      Curl_auth_digest_cleanup(digest);
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
  }

  /* Store the challenge for use later */
  digest->ಒಳಹರಿವು_token = (BYTE *)curlx_memdup(chlg, chlglen + 1);
  ಆದರೆ(!digest->ಒಳಹರಿವು_token)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  digest->ಒಳಹರಿವು_token_len = chlglen;

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
CURLcode Curl_auth_create_digest_http_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ರಚನೆ Curl_creds *creds,
                                              ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *request,
                                              ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *uripath,
                                              ರಚನೆ digestdata *digest,
                                              ಅಕ್ಷರ **outptr, size_t *outlen)
{
  size_t token_max;
  ಅಕ್ಷರ *resp;
  BYTE *output_token;
  size_t output_token_len = 0;
  PSecPkgInfo SecurityPackage;
  Sec_ಬಫರ್ chlg_buf[5];
  Sec_ಬಫರ್_Desc chlg_desc;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;

  /* Query the security package for DigestSSP */
  ಸ್ಥಿತಿ_2 =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK) {
    failf(ದತ್ತಾಂಶ, "SSPI: could not get auth info");
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_ಬಫರ್(SecurityPackage);

  /* Allocate the output buffer according to the max token size as indicated
     by the security package */
  output_token = curlx_malloc(token_max);
  ಆದರೆ(!output_token) {
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  /* If the user/passwd that was used to make the identity for http_context
     has changed then delete that context. */
  ಆದರೆ(!Curl_creds_same(creds, digest->creds)) {
    ಆದರೆ(digest->http_context) {
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
    Curl_creds_unlink(&digest->creds);
  }

  ಆದರೆ(digest->http_context) {
    chlg_desc.ul_ಆವೃತ್ತಿ    = SECBUFFER_ಆವೃತ್ತಿ;
    chlg_desc.cBuffers     = 5;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].ಬಫರ್_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_ಬಫರ್   = ಖಾಲಿ;
    chlg_buf[0].cb_ಬಫರ್   = 0;
    chlg_buf[1].ಬಫರ್_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_ಬಫರ್   = CURL_UNCONST(request);
    chlg_buf[1].cb_ಬಫರ್   = curlx_uztoul(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)request));
    chlg_buf[2].ಬಫರ್_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_ಬಫರ್   = CURL_UNCONST(uripath);
    chlg_buf[2].cb_ಬಫರ್   = curlx_uztoul(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)uripath));
    chlg_buf[3].ಬಫರ್_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[3].pv_ಬಫರ್   = ಖಾಲಿ;
    chlg_buf[3].cb_ಬಫರ್   = 0;
    chlg_buf[4].ಬಫರ್_Type = SECBUFFER_PADDING;
    chlg_buf[4].pv_ಬಫರ್   = output_token;
    chlg_buf[4].cb_ಬಫರ್   = curlx_uztoul(token_max);

    ಸ್ಥಿತಿ_2 = Curl_pSecFn->MakeSignature(digest->http_context, 0, &chlg_desc,
                                        0);
    ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_OK)
      output_token_len = chlg_buf[4].cb_ಬಫರ್;
    ಇಲ್ಲದಿದ್ದರೆ { /* delete the context so a new one can be made */
      infof(ದತ್ತಾಂಶ, "digest_sspi: MakeSignature failed, error 0x%08lx",
            (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ಸ್ಥಿತಿ_2);
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
  }

  ಆದರೆ(!digest->http_context) {
    CredHandle credentials;
    SEC_WINNT_AUTH_ತಟಸ್ಥ_EX ತಟಸ್ಥ;
    SEC_WINNT_AUTH_ತಟಸ್ಥ_EX *p_ತಟಸ್ಥ;
    Sec_ಬಫರ್ resp_buf;
    Sec_ಬಫರ್_Desc resp_desc;
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ attrs;
    TCHAR *spn;

    /* free the credentials used to make the previous identity */
    Curl_creds_unlink(&digest->creds);

    ಆದರೆ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ಆದರೆ(Curl_create_sspi_ತಟಸ್ಥ(creds->user, creds->passwd, &ತಟಸ್ಥ)) {
        curlx_free(output_token);
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }

      /* Populate our identity domain */
      ಆದರೆ(Curl_override_sspi_http_realm((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)digest->ಒಳಹರಿವು_token,
                                       &ತಟಸ್ಥ)) {
        Curl_sspi_free_ತಟಸ್ಥ(&ತಟಸ್ಥ);
        curlx_free(output_token);
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }

      /* Allow proper cleanup of the identity structure */
      p_ತಟಸ್ಥ = &ತಟಸ್ಥ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* Use the current Windows user */
      p_ತಟಸ್ಥ = ಖಾಲಿ;

    ಆದರೆ(creds)
      Curl_creds_link(&digest->creds, creds);

    /* Acquire our credentials handle */
    ಸ್ಥಿತಿ_2 = Curl_pSecFn->AcquireCredentialsHandle(ಖಾಲಿ,
                                   CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ಖಾಲಿ,
                                   p_ತಟಸ್ಥ, ಖಾಲಿ, ಖಾಲಿ,
                                   &credentials, ಖಾಲಿ);
    ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK) {
      Curl_sspi_free_ತಟಸ್ಥ(p_ತಟಸ್ಥ);
      curlx_free(output_token);

      ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
    }

    /* Setup the challenge "input" security buffer if present */
    chlg_desc.ul_ಆವೃತ್ತಿ    = SECBUFFER_ಆವೃತ್ತಿ;
    chlg_desc.cBuffers     = 3;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].ಬಫರ್_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_ಬಫರ್   = digest->ಒಳಹರಿವು_token;
    chlg_buf[0].cb_ಬಫರ್   = curlx_uztoul(digest->ಒಳಹರಿವು_token_len);
    chlg_buf[1].ಬಫರ್_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_ಬಫರ್   = CURL_UNCONST(request);
    chlg_buf[1].cb_ಬಫರ್   = curlx_uztoul(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)request));
    chlg_buf[2].ಬಫರ್_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_ಬಫರ್   = ಖಾಲಿ;
    chlg_buf[2].cb_ಬಫರ್   = 0;

    /* Setup the response "output" security buffer */
    resp_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;
    resp_desc.cBuffers  = 1;
    resp_desc.pBuffers  = &resp_buf;
    resp_buf.ಬಫರ್_Type = SECBUFFER_TOKEN;
    resp_buf.pv_ಬಫರ್   = output_token;
    resp_buf.cb_ಬಫರ್   = curlx_uztoul(token_max);

    spn = curlx_convert_UTF8_ಗಮ್ಯ_tchar((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)uripath);
    ಆದರೆ(!spn) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_ತಟಸ್ಥ(p_ತಟಸ್ಥ);
      curlx_free(output_token);

      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    /* Allocate our new context handle */
    digest->http_context = curlx_calloc(1, ಗಾತ್ರ(CtxtHandle));
    ಆದರೆ(!digest->http_context) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);
      curlx_free(spn);
      Curl_sspi_free_ತಟಸ್ಥ(p_ತಟಸ್ಥ);
      curlx_free(output_token);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    /* Generate our response message */
    ಸ್ಥಿತಿ_2 = Curl_pSecFn->InitializeSecurityContext(&credentials, ಖಾಲಿ,
                                                    spn,
                                                    ISC_REQ_USE_HTTP_STYLE,
                                                    0, 0,
                                                    &chlg_desc, 0,
                                                    digest->http_context,
                                                    &resp_desc, &attrs, ಖಾಲಿ);
    curlx_free(spn);

    ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_I_COMPLETE_NEEDED ||
       ಸ್ಥಿತಿ_2 == SEC_I_COMPLETE_AND_CONTINUE)
      Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK && ಸ್ಥಿತಿ_2 != SEC_I_CONTINUE_NEEDED) {
      VERBOSE(ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN]);

      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_ತಟಸ್ಥ(p_ತಟಸ್ಥ);
      curlx_free(output_token);

      curlx_safefree(digest->http_context);

      ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      infof(ದತ್ತಾಂಶ, "schannel: InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(ಸ್ಥಿತಿ_2, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));

      ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
    }

    output_token_len = resp_buf.cb_ಬಫರ್;

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_ತಟಸ್ಥ(p_ತಟಸ್ಥ);
  }

  resp = curlx_memdup0((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)output_token, output_token_len);
  curlx_free(output_token);
  ಆದರೆ(!resp) {
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  /* Return the response */
  *outptr = resp;
  *outlen = output_token_len;
  ಹಿಂತಿರುಗಿ CURLE_OK;
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
ಶೂನ್ಯ Curl_auth_digest_cleanup(ರಚನೆ digestdata *digest)
{
  /* Free the input token */
  curlx_safefree(digest->ಒಳಹರಿವು_token);

  /* Reset any variables */
  digest->ಒಳಹರಿವು_token_len = 0;

  /* Delete security context */
  ಆದರೆ(digest->http_context) {
    Curl_pSecFn->DeleteSecurityContext(digest->http_context);
    curlx_safefree(digest->http_context);
  }

  /* Free the copy of user/passwd used to make the identity for http_context */
  Curl_creds_unlink(&digest->creds);
  Curl_peer_unlink(&digest->origin);
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_WINDOWS_SSPI && !CURL_DISABLE_DIGEST_AUTH */
