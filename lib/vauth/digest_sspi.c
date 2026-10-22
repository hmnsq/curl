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
#অন্তর্ভাব "curl_setup.h"

#যদি defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_DIGEST_AUTH)

#অন্তর্ভাব "creds.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "vauth/digest.h"
#অন্তর্ভাব "curlx/multibyte.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "strerror.h"

/*
 * Curl_auth_is_digest_supported()
 *
 * This is used to evaluate if DIGEST is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if DIGEST is supported by Windows SSPI.
 */
bool Curl_auth_is_digest_supported(শূন্য)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for Digest */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  যদি(status == SEC_E_OK) {
    Curl_pSecFn->Free_Context_বাফার(SecurityPackage);
  }

  ফেরত status == SEC_E_OK;
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
CURLcode Curl_auth_create_digest_md5_message(কাঠামো Curl_easy *তথ্য,
                                             ধ্রুবক কাঠামো bufref *chlg,
                                             কাঠামো Curl_creds *creds,
                                             ধ্রুবক অক্ষর *default_service,
                                             কাঠামো bufref *out)
{
  CURLcode ফলাফল = CURLE_OK;
  TCHAR *spn = ফাঁকা;
  size_t token_max = 0;
  unsigned অক্ষর *output_token = ফাঁকা;
  CredHandle credentials;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  Sec_বাফার chlg_buf;
  Sec_বাফার resp_buf;
  Sec_বাফার_Desc chlg_desc;
  Sec_বাফার_Desc resp_desc;
  SECURITY_STATUS status;
  unsigned long attrs;
  ধ্রুবক অক্ষর *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Ensure we have a valid challenge message */
  যদি(!Curl_bufref_len(chlg)) {
    infof(তথ্য, "DIGEST-MD5 handshake failure (empty challenge message)");
    ফেরত CURLE_BAD_CONTENT_ENCODING;
  }

  /* Query the security package for DigestSSP */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  যদি(status != SEC_E_OK) {
    failf(তথ্য, "SSPI: could not get auth info");
    ফেরত CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_বাফার(SecurityPackage);

  /* Allocate our response buffer */
  output_token = curlx_malloc(token_max);
  যদি(!output_token)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, তথ্য->state.origin->hostname, ফাঁকা);
  যদি(!spn) {
    curlx_free(output_token);
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(Curl_creds_has_user(creds)) {
    /* Populate our identity structure */
    ফলাফল = Curl_create_sspi_identity(creds->user, creds->passwd,
                                       &identity);
    যদি(ফলাফল) {
      curlx_free(spn);
      curlx_free(output_token);
      ফেরত ফলাফল;
    }

    /* Allow proper cleanup of the identity structure */
    p_identity = &identity;
  }
  অন্যথা
    /* Use the current Windows user */
    p_identity = ফাঁকা;

  /* Acquire our credentials handle */
  status = Curl_pSecFn->AcquireCredentialsHandle(ফাঁকা,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ফাঁকা,
                                   p_identity, ফাঁকা, ফাঁকা,
                                   &credentials, ফাঁকা);

  যদি(status != SEC_E_OK) {
    Curl_sspi_free_identity(p_identity);
    curlx_free(spn);
    curlx_free(output_token);
    ফেরত CURLE_LOGIN_DENIED;
  }

  /* Setup the challenge "input" security buffer */
  chlg_desc.ulVersion = SECBUFFER_VERSION;
  chlg_desc.cBuffers  = 1;
  chlg_desc.pBuffers  = &chlg_buf;
  chlg_buf.বাফার_Type = SECBUFFER_TOKEN;
  chlg_buf.pv_বাফার   = CURL_UNCONST(Curl_bufref_ptr(chlg));
  chlg_buf.cb_বাফার   = curlx_uztoul(Curl_bufref_len(chlg));

  /* Setup the response "output" security buffer */
  resp_desc.ulVersion = SECBUFFER_VERSION;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.বাফার_Type = SECBUFFER_TOKEN;
  resp_buf.pv_বাফার   = output_token;
  resp_buf.cb_বাফার   = curlx_uztoul(token_max);

  /* Generate our response message */
  status = Curl_pSecFn->InitializeSecurityContext(&credentials, ফাঁকা, spn,
                                                  0, 0, 0, &chlg_desc, 0,
                                                  &context, &resp_desc, &attrs,
                                                  ফাঁকা);

  যদি(status == SEC_I_COMPLETE_NEEDED ||
     status == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
  অন্যথা যদি(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
    VERBOSE(অক্ষর বাফার[STRERROR_LEN]);

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_identity(p_identity);
    curlx_free(spn);
    curlx_free(output_token);

    যদি(status == SEC_E_INSUFFICIENT_MEMORY)
      ফেরত CURLE_OUT_OF_MEMORY;

    infof(তথ্য, "schannel: InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(status, বাফার, আকার(বাফার)));

    ফেরত CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  Curl_bufref_set(out, output_token, resp_buf.cb_বাফার, curl_free);

  /* Free our handles */
  Curl_pSecFn->DeleteSecurityContext(&context);
  Curl_pSecFn->FreeCredentialsHandle(&credentials);

  /* Free the identity structure */
  Curl_sspi_free_identity(p_identity);

  /* Free the SPN */
  curlx_free(spn);

  ফেরত ফলাফল;
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
CURLcode Curl_override_sspi_http_realm(ধ্রুবক অক্ষর *chlg,
                                       SEC_WINNT_AUTH_IDENTITY_EX *identity)
{
  xcharp_u domain, dup_domain;

  /* If domain is blank or unset, check challenge message for realm */
  যদি(!identity->Domain || !identity->Domain_দৈর্ঘ্য) {
    জন্য(;;) {
      কাঠামো Curl_str মান;
      অক্ষর content[DIGEST_MAX_CONTENT_দৈর্ঘ্য];

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Extract a value=content pair */
      যদি(Curl_auth_digest_get_pair(chlg, &মান, content, &chlg)) {
        যদি(curlx_str_casecompare(&মান, "realm")) {

          /* Setup identity's domain and length */
          domain.tchar_ptr = curlx_convert_UTF8_to_tchar(content);
          যদি(!domain.tchar_ptr)
            ফেরত CURLE_OUT_OF_MEMORY;

          dup_domain.tchar_ptr = curlx_tcsdup(domain.tchar_ptr);
          যদি(!dup_domain.tchar_ptr) {
            curlx_free(domain.tchar_ptr);
            ফেরত CURLE_OUT_OF_MEMORY;
          }

          curlx_free(identity->Domain);
          identity->Domain = dup_domain.tbyte_ptr;
          identity->Domain_দৈর্ঘ্য = curlx_uztoul(_tcslen(dup_domain.tchar_ptr));
          dup_domain.tchar_ptr = ফাঁকা;

          curlx_free(domain.tchar_ptr);
        }
        অন্যথা {
          /* Unknown specifier, ignore it! */
        }
      }
      অন্যথা
        বিরতি; /* We are done here */

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Allow the list to be comma-separated */
      (শূন্য)curlx_str_single(&chlg, ',');
    }
  }

  ফেরত CURLE_OK;
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
CURLcode Curl_auth_decode_digest_http_message(ধ্রুবক অক্ষর *chlg,
                                              কাঠামো digestdata *digest)
{
  size_t chlglen = strlen(chlg);

  /* We had an input token before so if there is another one now that means we
     provided bad credentials in the previous request or it is stale. */
  যদি(digest->প্রবেশ_token) {
    bool stale = FALSE;
    ধ্রুবক অক্ষর *p = chlg;

    /* Check for the 'stale' directive */
    জন্য(;;) {
      কাঠামো Curl_str মান;
      অক্ষর content[DIGEST_MAX_CONTENT_দৈর্ঘ্য];

      যখন(*p && ISBLANK(*p))
        p++;

      যদি(!Curl_auth_digest_get_pair(p, &মান, content, &p))
        বিরতি;

      যদি(curlx_str_casecompare(&মান, "stale") &&
         curl_strequal(content, "true")) {
        stale = TRUE;
        বিরতি;
      }

      যখন(*p && ISBLANK(*p))
        p++;

      যদি(',' == *p)
        p++;
    }

    যদি(stale)
      Curl_auth_digest_cleanup(digest);
    অন্যথা
      ফেরত CURLE_LOGIN_DENIED;
  }

  /* Store the challenge for use later */
  digest->প্রবেশ_token = (BYTE *)curlx_memdup(chlg, chlglen + 1);
  যদি(!digest->প্রবেশ_token)
    ফেরত CURLE_OUT_OF_MEMORY;

  digest->প্রবেশ_token_len = chlglen;

  ফেরত CURLE_OK;
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
CURLcode Curl_auth_create_digest_http_message(কাঠামো Curl_easy *তথ্য,
                                              কাঠামো Curl_creds *creds,
                                              ধ্রুবক unsigned অক্ষর *request,
                                              ধ্রুবক unsigned অক্ষর *uripath,
                                              কাঠামো digestdata *digest,
                                              অক্ষর **outptr, size_t *outlen)
{
  size_t token_max;
  অক্ষর *resp;
  BYTE *output_token;
  size_t output_token_len = 0;
  PSecPkgInfo SecurityPackage;
  Sec_বাফার chlg_buf[5];
  Sec_বাফার_Desc chlg_desc;
  SECURITY_STATUS status;

  /* Query the security package for DigestSSP */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  যদি(status != SEC_E_OK) {
    failf(তথ্য, "SSPI: could not get auth info");
    ফেরত CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_বাফার(SecurityPackage);

  /* Allocate the output buffer according to the max token size as indicated
     by the security package */
  output_token = curlx_malloc(token_max);
  যদি(!output_token) {
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  /* If the user/passwd that was used to make the identity for http_context
     has changed then delete that context. */
  যদি(!Curl_creds_same(creds, digest->creds)) {
    যদি(digest->http_context) {
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
    Curl_creds_unlink(&digest->creds);
  }

  যদি(digest->http_context) {
    chlg_desc.ulVersion    = SECBUFFER_VERSION;
    chlg_desc.cBuffers     = 5;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].বাফার_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_বাফার   = ফাঁকা;
    chlg_buf[0].cb_বাফার   = 0;
    chlg_buf[1].বাফার_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_বাফার   = CURL_UNCONST(request);
    chlg_buf[1].cb_বাফার   = curlx_uztoul(strlen((ধ্রুবক অক্ষর *)request));
    chlg_buf[2].বাফার_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_বাফার   = CURL_UNCONST(uripath);
    chlg_buf[2].cb_বাফার   = curlx_uztoul(strlen((ধ্রুবক অক্ষর *)uripath));
    chlg_buf[3].বাফার_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[3].pv_বাফার   = ফাঁকা;
    chlg_buf[3].cb_বাফার   = 0;
    chlg_buf[4].বাফার_Type = SECBUFFER_PADDING;
    chlg_buf[4].pv_বাফার   = output_token;
    chlg_buf[4].cb_বাফার   = curlx_uztoul(token_max);

    status = Curl_pSecFn->MakeSignature(digest->http_context, 0, &chlg_desc,
                                        0);
    যদি(status == SEC_E_OK)
      output_token_len = chlg_buf[4].cb_বাফার;
    অন্যথা { /* delete the context so a new one can be made */
      infof(তথ্য, "digest_sspi: MakeSignature failed, error 0x%08lx",
            (unsigned long)status);
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
  }

  যদি(!digest->http_context) {
    CredHandle credentials;
    SEC_WINNT_AUTH_IDENTITY_EX identity;
    SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
    Sec_বাফার resp_buf;
    Sec_বাফার_Desc resp_desc;
    unsigned long attrs;
    TCHAR *spn;

    /* free the credentials used to make the previous identity */
    Curl_creds_unlink(&digest->creds);

    যদি(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      যদি(Curl_create_sspi_identity(creds->user, creds->passwd, &identity)) {
        curlx_free(output_token);
        ফেরত CURLE_OUT_OF_MEMORY;
      }

      /* Populate our identity domain */
      যদি(Curl_override_sspi_http_realm((ধ্রুবক অক্ষর *)digest->প্রবেশ_token,
                                       &identity)) {
        Curl_sspi_free_identity(&identity);
        curlx_free(output_token);
        ফেরত CURLE_OUT_OF_MEMORY;
      }

      /* Allow proper cleanup of the identity structure */
      p_identity = &identity;
    }
    অন্যথা
      /* Use the current Windows user */
      p_identity = ফাঁকা;

    যদি(creds)
      Curl_creds_link(&digest->creds, creds);

    /* Acquire our credentials handle */
    status = Curl_pSecFn->AcquireCredentialsHandle(ফাঁকা,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ফাঁকা,
                                   p_identity, ফাঁকা, ফাঁকা,
                                   &credentials, ফাঁকা);
    যদি(status != SEC_E_OK) {
      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      ফেরত CURLE_LOGIN_DENIED;
    }

    /* Setup the challenge "input" security buffer if present */
    chlg_desc.ulVersion    = SECBUFFER_VERSION;
    chlg_desc.cBuffers     = 3;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].বাফার_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_বাফার   = digest->প্রবেশ_token;
    chlg_buf[0].cb_বাফার   = curlx_uztoul(digest->প্রবেশ_token_len);
    chlg_buf[1].বাফার_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_বাফার   = CURL_UNCONST(request);
    chlg_buf[1].cb_বাফার   = curlx_uztoul(strlen((ধ্রুবক অক্ষর *)request));
    chlg_buf[2].বাফার_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_বাফার   = ফাঁকা;
    chlg_buf[2].cb_বাফার   = 0;

    /* Setup the response "output" security buffer */
    resp_desc.ulVersion = SECBUFFER_VERSION;
    resp_desc.cBuffers  = 1;
    resp_desc.pBuffers  = &resp_buf;
    resp_buf.বাফার_Type = SECBUFFER_TOKEN;
    resp_buf.pv_বাফার   = output_token;
    resp_buf.cb_বাফার   = curlx_uztoul(token_max);

    spn = curlx_convert_UTF8_to_tchar((ধ্রুবক অক্ষর *)uripath);
    যদি(!spn) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      ফেরত CURLE_OUT_OF_MEMORY;
    }

    /* Allocate our new context handle */
    digest->http_context = curlx_calloc(1, আকার(CtxtHandle));
    যদি(!digest->http_context) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);
      curlx_free(spn);
      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);
      ফেরত CURLE_OUT_OF_MEMORY;
    }

    /* Generate our response message */
    status = Curl_pSecFn->InitializeSecurityContext(&credentials, ফাঁকা,
                                                    spn,
                                                    ISC_REQ_USE_HTTP_STYLE,
                                                    0, 0,
                                                    &chlg_desc, 0,
                                                    digest->http_context,
                                                    &resp_desc, &attrs, ফাঁকা);
    curlx_free(spn);

    যদি(status == SEC_I_COMPLETE_NEEDED ||
       status == SEC_I_COMPLETE_AND_CONTINUE)
      Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
    অন্যথা যদি(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
      VERBOSE(অক্ষর বাফার[STRERROR_LEN]);

      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      curlx_safefree(digest->http_context);

      যদি(status == SEC_E_INSUFFICIENT_MEMORY)
        ফেরত CURLE_OUT_OF_MEMORY;

      infof(তথ্য, "schannel: InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(status, বাফার, আকার(বাফার)));

      ফেরত CURLE_AUTH_ERROR;
    }

    output_token_len = resp_buf.cb_বাফার;

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_identity(p_identity);
  }

  resp = curlx_memdup0((ধ্রুবক অক্ষর *)output_token, output_token_len);
  curlx_free(output_token);
  যদি(!resp) {
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  /* Return the response */
  *outptr = resp;
  *outlen = output_token_len;
  ফেরত CURLE_OK;
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
শূন্য Curl_auth_digest_cleanup(কাঠামো digestdata *digest)
{
  /* Free the input token */
  curlx_safefree(digest->প্রবেশ_token);

  /* Reset any variables */
  digest->প্রবেশ_token_len = 0;

  /* Delete security context */
  যদি(digest->http_context) {
    Curl_pSecFn->DeleteSecurityContext(digest->http_context);
    curlx_safefree(digest->http_context);
  }

  /* Free the copy of user/passwd used to make the identity for http_context */
  Curl_creds_unlink(&digest->creds);
  Curl_peer_unlink(&digest->origin);
}

#endif /* USE_WINDOWS_SSPI && !CURL_DISABLE_DIGEST_AUTH */
