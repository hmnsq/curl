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
    Curl_pSecFn->FreeContextBuffer(SecurityPackage);
  }

  প্রত্যাগম status == SEC_E_OK;
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
CURLcode Curl_auth_create_digest_md5_message(সংরচনা Curl_easy *data,
                                             ধ্রুব সংরচনা bufref *chlg,
                                             সংরচনা Curl_creds *creds,
                                             ধ্রুব বর্ণ *default_service,
                                             সংরচনা bufref *out)
{
  CURLcode result = CURLE_OK;
  TCHAR *spn = রিক্ত;
  size_t token_max = 0;
  unsigned বর্ণ *output_token = রিক্ত;
  CredHandle credentials;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  SecBuffer chlg_buf;
  SecBuffer resp_buf;
  SecBufferDesc chlg_desc;
  SecBufferDesc resp_desc;
  SECURITY_STATUS status;
  unsigned long attrs;
  ধ্রুব বর্ণ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Ensure we have a valid challenge message */
  যদি(!Curl_bufref_len(chlg)) {
    infof(data, "DIGEST-MD5 handshake failure (empty challenge message)");
    প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;
  }

  /* Query the security package for DigestSSP */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  যদি(status != SEC_E_OK) {
    failf(data, "SSPI: could not get auth info");
    প্রত্যাগম CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->FreeContextBuffer(SecurityPackage);

  /* Allocate our response buffer */
  output_token = curlx_malloc(token_max);
  যদি(!output_token)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, রিক্ত);
  যদি(!spn) {
    curlx_free(output_token);
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  যদি(Curl_creds_has_user(creds)) {
    /* Populate our identity structure */
    result = Curl_create_sspi_identity(creds->user, creds->passwd,
                                       &identity);
    যদি(result) {
      curlx_free(spn);
      curlx_free(output_token);
      প্রত্যাগম result;
    }

    /* Allow proper cleanup of the identity structure */
    p_identity = &identity;
  }
  অন্যথা
    /* Use the current Windows user */
    p_identity = রিক্ত;

  /* Acquire our credentials handle */
  status = Curl_pSecFn->AcquireCredentialsHandle(রিক্ত,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, রিক্ত,
                                   p_identity, রিক্ত, রিক্ত,
                                   &credentials, রিক্ত);

  যদি(status != SEC_E_OK) {
    Curl_sspi_free_identity(p_identity);
    curlx_free(spn);
    curlx_free(output_token);
    প্রত্যাগম CURLE_LOGIN_DENIED;
  }

  /* Setup the challenge "input" security buffer */
  chlg_desc.ulVersion = SECBUFFER_VERSION;
  chlg_desc.cBuffers  = 1;
  chlg_desc.pBuffers  = &chlg_buf;
  chlg_buf.BufferType = SECBUFFER_TOKEN;
  chlg_buf.pvBuffer   = CURL_UNCONST(Curl_bufref_ptr(chlg));
  chlg_buf.cbBuffer   = curlx_uztoul(Curl_bufref_len(chlg));

  /* Setup the response "output" security buffer */
  resp_desc.ulVersion = SECBUFFER_VERSION;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.BufferType = SECBUFFER_TOKEN;
  resp_buf.pvBuffer   = output_token;
  resp_buf.cbBuffer   = curlx_uztoul(token_max);

  /* Generate our response message */
  status = Curl_pSecFn->InitializeSecurityContext(&credentials, রিক্ত, spn,
                                                  0, 0, 0, &chlg_desc, 0,
                                                  &context, &resp_desc, &attrs,
                                                  রিক্ত);

  যদি(status == SEC_I_COMPLETE_NEEDED ||
     status == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
  অন্যথা যদি(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
    VERBOSE(বর্ণ buffer[STRERROR_LEN]);

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_identity(p_identity);
    curlx_free(spn);
    curlx_free(output_token);

    যদি(status == SEC_E_INSUFFICIENT_MEMORY)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;

    infof(data, "schannel: InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(status, buffer, পরিমাণ(buffer)));

    প্রত্যাগম CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  Curl_bufref_set(out, output_token, resp_buf.cbBuffer, curl_free);

  /* Free our handles */
  Curl_pSecFn->DeleteSecurityContext(&context);
  Curl_pSecFn->FreeCredentialsHandle(&credentials);

  /* Free the identity structure */
  Curl_sspi_free_identity(p_identity);

  /* Free the SPN */
  curlx_free(spn);

  প্রত্যাগম result;
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
CURLcode Curl_override_sspi_http_realm(ধ্রুব বর্ণ *chlg,
                                       SEC_WINNT_AUTH_IDENTITY_EX *identity)
{
  xcharp_u domain, dup_domain;

  /* If domain is blank or unset, check challenge message for realm */
  যদি(!identity->Domain || !identity->DomainLength) {
    প্রতি(;;) {
      সংরচনা Curl_str মান;
      বর্ণ content[DIGEST_MAX_CONTENT_LENGTH];

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Extract a value=content pair */
      যদি(Curl_auth_digest_get_pair(chlg, &মান, content, &chlg)) {
        যদি(curlx_str_casecompare(&মান, "realm")) {

          /* Setup identity's domain and length */
          domain.tchar_ptr = curlx_convert_UTF8_to_tchar(content);
          যদি(!domain.tchar_ptr)
            প্রত্যাগম CURLE_OUT_OF_MEMORY;

          dup_domain.tchar_ptr = curlx_tcsdup(domain.tchar_ptr);
          যদি(!dup_domain.tchar_ptr) {
            curlx_free(domain.tchar_ptr);
            প্রত্যাগম CURLE_OUT_OF_MEMORY;
          }

          curlx_free(identity->Domain);
          identity->Domain = dup_domain.tbyte_ptr;
          identity->DomainLength = curlx_uztoul(_tcslen(dup_domain.tchar_ptr));
          dup_domain.tchar_ptr = রিক্ত;

          curlx_free(domain.tchar_ptr);
        }
        অন্যথা {
          /* Unknown specifier, ignore it! */
        }
      }
      অন্যথা
        ভঙ্গ; /* We are done here */

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Allow the list to be comma-separated */
      (শূন্য)curlx_str_single(&chlg, ',');
    }
  }

  প্রত্যাগম CURLE_OK;
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
CURLcode Curl_auth_decode_digest_http_message(ধ্রুব বর্ণ *chlg,
                                              সংরচনা digestdata *digest)
{
  size_t chlglen = strlen(chlg);

  /* We had an input token before so if there is another one now that means we
     provided bad credentials in the previous request or it is stale. */
  যদি(digest->প্রবেশ_token) {
    bool stale = FALSE;
    ধ্রুব বর্ণ *p = chlg;

    /* Check for the 'stale' directive */
    প্রতি(;;) {
      সংরচনা Curl_str মান;
      বর্ণ content[DIGEST_MAX_CONTENT_LENGTH];

      যাবত্(*p && ISBLANK(*p))
        p++;

      যদি(!Curl_auth_digest_get_pair(p, &মান, content, &p))
        ভঙ্গ;

      যদি(curlx_str_casecompare(&মান, "stale") &&
         curl_strequal(content, "true")) {
        stale = TRUE;
        ভঙ্গ;
      }

      যাবত্(*p && ISBLANK(*p))
        p++;

      যদি(',' == *p)
        p++;
    }

    যদি(stale)
      Curl_auth_digest_cleanup(digest);
    অন্যথা
      প্রত্যাগম CURLE_LOGIN_DENIED;
  }

  /* Store the challenge for use later */
  digest->প্রবেশ_token = (BYTE *)curlx_memdup(chlg, chlglen + 1);
  যদি(!digest->প্রবেশ_token)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  digest->প্রবেশ_token_len = chlglen;

  প্রত্যাগম CURLE_OK;
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
CURLcode Curl_auth_create_digest_http_message(সংরচনা Curl_easy *data,
                                              সংরচনা Curl_creds *creds,
                                              ধ্রুব unsigned বর্ণ *request,
                                              ধ্রুব unsigned বর্ণ *uripath,
                                              সংরচনা digestdata *digest,
                                              বর্ণ **outptr, size_t *outlen)
{
  size_t token_max;
  বর্ণ *resp;
  BYTE *output_token;
  size_t output_token_len = 0;
  PSecPkgInfo SecurityPackage;
  SecBuffer chlg_buf[5];
  SecBufferDesc chlg_desc;
  SECURITY_STATUS status;

  /* Query the security package for DigestSSP */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  যদি(status != SEC_E_OK) {
    failf(data, "SSPI: could not get auth info");
    প্রত্যাগম CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->FreeContextBuffer(SecurityPackage);

  /* Allocate the output buffer according to the max token size as indicated
     by the security package */
  output_token = curlx_malloc(token_max);
  যদি(!output_token) {
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
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
    chlg_buf[0].BufferType = SECBUFFER_TOKEN;
    chlg_buf[0].pvBuffer   = রিক্ত;
    chlg_buf[0].cbBuffer   = 0;
    chlg_buf[1].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pvBuffer   = CURL_UNCONST(request);
    chlg_buf[1].cbBuffer   = curlx_uztoul(strlen((ধ্রুব বর্ণ *)request));
    chlg_buf[2].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pvBuffer   = CURL_UNCONST(uripath);
    chlg_buf[2].cbBuffer   = curlx_uztoul(strlen((ধ্রুব বর্ণ *)uripath));
    chlg_buf[3].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[3].pvBuffer   = রিক্ত;
    chlg_buf[3].cbBuffer   = 0;
    chlg_buf[4].BufferType = SECBUFFER_PADDING;
    chlg_buf[4].pvBuffer   = output_token;
    chlg_buf[4].cbBuffer   = curlx_uztoul(token_max);

    status = Curl_pSecFn->MakeSignature(digest->http_context, 0, &chlg_desc,
                                        0);
    যদি(status == SEC_E_OK)
      output_token_len = chlg_buf[4].cbBuffer;
    অন্যথা { /* delete the context so a new one can be made */
      infof(data, "digest_sspi: MakeSignature failed, error 0x%08lx",
            (unsigned long)status);
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
  }

  যদি(!digest->http_context) {
    CredHandle credentials;
    SEC_WINNT_AUTH_IDENTITY_EX identity;
    SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
    SecBuffer resp_buf;
    SecBufferDesc resp_desc;
    unsigned long attrs;
    TCHAR *spn;

    /* free the credentials used to make the previous identity */
    Curl_creds_unlink(&digest->creds);

    যদি(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      যদি(Curl_create_sspi_identity(creds->user, creds->passwd, &identity)) {
        curlx_free(output_token);
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      }

      /* Populate our identity domain */
      যদি(Curl_override_sspi_http_realm((ধ্রুব বর্ণ *)digest->প্রবেশ_token,
                                       &identity)) {
        Curl_sspi_free_identity(&identity);
        curlx_free(output_token);
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      }

      /* Allow proper cleanup of the identity structure */
      p_identity = &identity;
    }
    অন্যথা
      /* Use the current Windows user */
      p_identity = রিক্ত;

    যদি(creds)
      Curl_creds_link(&digest->creds, creds);

    /* Acquire our credentials handle */
    status = Curl_pSecFn->AcquireCredentialsHandle(রিক্ত,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, রিক্ত,
                                   p_identity, রিক্ত, রিক্ত,
                                   &credentials, রিক্ত);
    যদি(status != SEC_E_OK) {
      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      প্রত্যাগম CURLE_LOGIN_DENIED;
    }

    /* Setup the challenge "input" security buffer if present */
    chlg_desc.ulVersion    = SECBUFFER_VERSION;
    chlg_desc.cBuffers     = 3;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].BufferType = SECBUFFER_TOKEN;
    chlg_buf[0].pvBuffer   = digest->প্রবেশ_token;
    chlg_buf[0].cbBuffer   = curlx_uztoul(digest->প্রবেশ_token_len);
    chlg_buf[1].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pvBuffer   = CURL_UNCONST(request);
    chlg_buf[1].cbBuffer   = curlx_uztoul(strlen((ধ্রুব বর্ণ *)request));
    chlg_buf[2].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pvBuffer   = রিক্ত;
    chlg_buf[2].cbBuffer   = 0;

    /* Setup the response "output" security buffer */
    resp_desc.ulVersion = SECBUFFER_VERSION;
    resp_desc.cBuffers  = 1;
    resp_desc.pBuffers  = &resp_buf;
    resp_buf.BufferType = SECBUFFER_TOKEN;
    resp_buf.pvBuffer   = output_token;
    resp_buf.cbBuffer   = curlx_uztoul(token_max);

    spn = curlx_convert_UTF8_to_tchar((ধ্রুব বর্ণ *)uripath);
    যদি(!spn) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }

    /* Allocate our new context handle */
    digest->http_context = curlx_calloc(1, পরিমাণ(CtxtHandle));
    যদি(!digest->http_context) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);
      curlx_free(spn);
      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }

    /* Generate our response message */
    status = Curl_pSecFn->InitializeSecurityContext(&credentials, রিক্ত,
                                                    spn,
                                                    ISC_REQ_USE_HTTP_STYLE,
                                                    0, 0,
                                                    &chlg_desc, 0,
                                                    digest->http_context,
                                                    &resp_desc, &attrs, রিক্ত);
    curlx_free(spn);

    যদি(status == SEC_I_COMPLETE_NEEDED ||
       status == SEC_I_COMPLETE_AND_CONTINUE)
      Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
    অন্যথা যদি(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
      VERBOSE(বর্ণ buffer[STRERROR_LEN]);

      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      curlx_safefree(digest->http_context);

      যদি(status == SEC_E_INSUFFICIENT_MEMORY)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;

      infof(data, "schannel: InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(status, buffer, পরিমাণ(buffer)));

      প্রত্যাগম CURLE_AUTH_ERROR;
    }

    output_token_len = resp_buf.cbBuffer;

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_identity(p_identity);
  }

  resp = curlx_memdup0((ধ্রুব বর্ণ *)output_token, output_token_len);
  curlx_free(output_token);
  যদি(!resp) {
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  /* Return the response */
  *outptr = resp;
  *outlen = output_token_len;
  প্রত্যাগম CURLE_OK;
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
শূন্য Curl_auth_digest_cleanup(সংরচনা digestdata *digest)
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
