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
#समावेश "curl_setup.h"

#अगर defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_DIGEST_AUTH)

#समावेश "creds.h"
#समावेश "vauth/vauth.h"
#समावेश "vauth/digest.h"
#समावेश "curlx/multibyte.h"
#समावेश "curl_trc.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/strparse.h"
#समावेश "strcase.h"
#समावेश "strerror.h"

/*
 * Curl_auth_is_digest_supported()
 *
 * This is used to evaluate if DIGEST is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if DIGEST is supported by Windows SSPI.
 */
bool Curl_auth_is_digest_supported(व्योम)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for Digest */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  अगर(status == SEC_E_OK) {
    Curl_pSecFn->Free_Context_बफर(SecurityPackage);
  }

  वापस status == SEC_E_OK;
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
CURLcode Curl_auth_create_digest_md5_message(काष्ठा Curl_easy *तथ्य,
                                             स्थिर काष्ठा bufref *chlg,
                                             काष्ठा Curl_creds *creds,
                                             स्थिर अक्षर *default_service,
                                             काष्ठा bufref *out)
{
  CURLcode परिणाम = CURLE_OK;
  TCHAR *spn = शून्य;
  माप_प्रकार token_max = 0;
  अचिन्हित अक्षर *output_token = शून्य;
  CredHandle credentials;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  SEC_WINNT_AUTH_IDENTITY_EX identity;
  SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
  Sec_बफर chlg_buf;
  Sec_बफर resp_buf;
  Sec_बफर_Desc chlg_desc;
  Sec_बफर_Desc resp_desc;
  SECURITY_STATUS status;
  अचिन्हित दीर्घ attrs;
  स्थिर अक्षर *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Ensure we have a valid challenge message */
  अगर(!Curl_bufref_len(chlg)) {
    infof(तथ्य, "DIGEST-MD5 handshake failure (empty challenge message)");
    वापस CURLE_BAD_CONTENT_ENCODING;
  }

  /* Query the security package for DigestSSP */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  अगर(status != SEC_E_OK) {
    failf(तथ्य, "SSPI: could not get auth info");
    वापस CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_बफर(SecurityPackage);

  /* Allocate our response buffer */
  output_token = curlx_malloc(token_max);
  अगर(!output_token)
    वापस CURLE_OUT_OF_MEMORY;

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, तथ्य->state.origin->hostname, शून्य);
  अगर(!spn) {
    curlx_free(output_token);
    वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(Curl_creds_has_user(creds)) {
    /* Populate our identity structure */
    परिणाम = Curl_create_sspi_identity(creds->user, creds->passwd,
                                       &identity);
    अगर(परिणाम) {
      curlx_free(spn);
      curlx_free(output_token);
      वापस परिणाम;
    }

    /* Allow proper cleanup of the identity structure */
    p_identity = &identity;
  }
  अन्यथा
    /* Use the current Windows user */
    p_identity = शून्य;

  /* Acquire our credentials handle */
  status = Curl_pSecFn->AcquireCredentialsHandle(शून्य,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, शून्य,
                                   p_identity, शून्य, शून्य,
                                   &credentials, शून्य);

  अगर(status != SEC_E_OK) {
    Curl_sspi_free_identity(p_identity);
    curlx_free(spn);
    curlx_free(output_token);
    वापस CURLE_LOGIN_DENIED;
  }

  /* Setup the challenge "input" security buffer */
  chlg_desc.ulVersion = SECBUFFER_VERSION;
  chlg_desc.cBuffers  = 1;
  chlg_desc.pBuffers  = &chlg_buf;
  chlg_buf.बफर_Type = SECBUFFER_TOKEN;
  chlg_buf.pv_बफर   = CURL_UNCONST(Curl_bufref_ptr(chlg));
  chlg_buf.cb_बफर   = curlx_uztoul(Curl_bufref_len(chlg));

  /* Setup the response "output" security buffer */
  resp_desc.ulVersion = SECBUFFER_VERSION;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.बफर_Type = SECBUFFER_TOKEN;
  resp_buf.pv_बफर   = output_token;
  resp_buf.cb_बफर   = curlx_uztoul(token_max);

  /* Generate our response message */
  status = Curl_pSecFn->InitializeSecurityContext(&credentials, शून्य, spn,
                                                  0, 0, 0, &chlg_desc, 0,
                                                  &context, &resp_desc, &attrs,
                                                  शून्य);

  अगर(status == SEC_I_COMPLETE_NEEDED ||
     status == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
  अन्यथा अगर(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
    VERBOSE(अक्षर बफर[STRERROR_LEN]);

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_identity(p_identity);
    curlx_free(spn);
    curlx_free(output_token);

    अगर(status == SEC_E_INSUFFICIENT_MEMORY)
      वापस CURLE_OUT_OF_MEMORY;

    infof(तथ्य, "schannel: InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(status, बफर, माप(बफर)));

    वापस CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  Curl_bufref_set(out, output_token, resp_buf.cb_बफर, curl_free);

  /* Free our handles */
  Curl_pSecFn->DeleteSecurityContext(&context);
  Curl_pSecFn->FreeCredentialsHandle(&credentials);

  /* Free the identity structure */
  Curl_sspi_free_identity(p_identity);

  /* Free the SPN */
  curlx_free(spn);

  वापस परिणाम;
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
CURLcode Curl_override_sspi_http_realm(स्थिर अक्षर *chlg,
                                       SEC_WINNT_AUTH_IDENTITY_EX *identity)
{
  xcharp_u domain, dup_domain;

  /* If domain is blank or unset, check challenge message for realm */
  अगर(!identity->Domain || !identity->Domain_लंबाई) {
    क्रम(;;) {
      काष्ठा Curl_str मूल्य;
      अक्षर content[DIGEST_MAX_CONTENT_लंबाई];

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Extract a value=content pair */
      अगर(Curl_auth_digest_get_pair(chlg, &मूल्य, content, &chlg)) {
        अगर(curlx_str_casecompare(&मूल्य, "realm")) {

          /* Setup identity's domain and length */
          domain.tchar_ptr = curlx_convert_UTF8_to_tchar(content);
          अगर(!domain.tchar_ptr)
            वापस CURLE_OUT_OF_MEMORY;

          dup_domain.tchar_ptr = curlx_tcsdup(domain.tchar_ptr);
          अगर(!dup_domain.tchar_ptr) {
            curlx_free(domain.tchar_ptr);
            वापस CURLE_OUT_OF_MEMORY;
          }

          curlx_free(identity->Domain);
          identity->Domain = dup_domain.tbyte_ptr;
          identity->Domain_लंबाई = curlx_uztoul(_tcslen(dup_domain.tchar_ptr));
          dup_domain.tchar_ptr = शून्य;

          curlx_free(domain.tchar_ptr);
        }
        अन्यथा {
          /* Unknown specifier, ignore it! */
        }
      }
      अन्यथा
        अवरोध; /* We are done here */

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Allow the list to be comma-separated */
      (व्योम)curlx_str_single(&chlg, ',');
    }
  }

  वापस CURLE_OK;
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
CURLcode Curl_auth_decode_digest_http_message(स्थिर अक्षर *chlg,
                                              काष्ठा digestdata *digest)
{
  माप_प्रकार chlglen = म_माप(chlg);

  /* We had an input token before so if there is another one now that means we
     provided bad credentials in the previous request or it is stale. */
  अगर(digest->निवेश_token) {
    bool stale = FALSE;
    स्थिर अक्षर *p = chlg;

    /* Check for the 'stale' directive */
    क्रम(;;) {
      काष्ठा Curl_str मूल्य;
      अक्षर content[DIGEST_MAX_CONTENT_लंबाई];

      जबतक(*p && ISBLANK(*p))
        p++;

      अगर(!Curl_auth_digest_get_pair(p, &मूल्य, content, &p))
        अवरोध;

      अगर(curlx_str_casecompare(&मूल्य, "stale") &&
         curl_strequal(content, "true")) {
        stale = TRUE;
        अवरोध;
      }

      जबतक(*p && ISBLANK(*p))
        p++;

      अगर(',' == *p)
        p++;
    }

    अगर(stale)
      Curl_auth_digest_cleanup(digest);
    अन्यथा
      वापस CURLE_LOGIN_DENIED;
  }

  /* Store the challenge for use later */
  digest->निवेश_token = (BYTE *)curlx_memdup(chlg, chlglen + 1);
  अगर(!digest->निवेश_token)
    वापस CURLE_OUT_OF_MEMORY;

  digest->निवेश_token_len = chlglen;

  वापस CURLE_OK;
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
CURLcode Curl_auth_create_digest_http_message(काष्ठा Curl_easy *तथ्य,
                                              काष्ठा Curl_creds *creds,
                                              स्थिर अचिन्हित अक्षर *request,
                                              स्थिर अचिन्हित अक्षर *uripath,
                                              काष्ठा digestdata *digest,
                                              अक्षर **outptr, माप_प्रकार *outlen)
{
  माप_प्रकार token_max;
  अक्षर *resp;
  BYTE *output_token;
  माप_प्रकार output_token_len = 0;
  PSecPkgInfo SecurityPackage;
  Sec_बफर chlg_buf[5];
  Sec_बफर_Desc chlg_desc;
  SECURITY_STATUS status;

  /* Query the security package for DigestSSP */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  अगर(status != SEC_E_OK) {
    failf(तथ्य, "SSPI: could not get auth info");
    वापस CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_बफर(SecurityPackage);

  /* Allocate the output buffer according to the max token size as indicated
     by the security package */
  output_token = curlx_malloc(token_max);
  अगर(!output_token) {
    वापस CURLE_OUT_OF_MEMORY;
  }

  /* If the user/passwd that was used to make the identity for http_context
     has changed then delete that context. */
  अगर(!Curl_creds_same(creds, digest->creds)) {
    अगर(digest->http_context) {
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
    Curl_creds_unlink(&digest->creds);
  }

  अगर(digest->http_context) {
    chlg_desc.ulVersion    = SECBUFFER_VERSION;
    chlg_desc.cBuffers     = 5;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].बफर_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_बफर   = शून्य;
    chlg_buf[0].cb_बफर   = 0;
    chlg_buf[1].बफर_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_बफर   = CURL_UNCONST(request);
    chlg_buf[1].cb_बफर   = curlx_uztoul(म_माप((स्थिर अक्षर *)request));
    chlg_buf[2].बफर_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_बफर   = CURL_UNCONST(uripath);
    chlg_buf[2].cb_बफर   = curlx_uztoul(म_माप((स्थिर अक्षर *)uripath));
    chlg_buf[3].बफर_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[3].pv_बफर   = शून्य;
    chlg_buf[3].cb_बफर   = 0;
    chlg_buf[4].बफर_Type = SECBUFFER_PADDING;
    chlg_buf[4].pv_बफर   = output_token;
    chlg_buf[4].cb_बफर   = curlx_uztoul(token_max);

    status = Curl_pSecFn->MakeSignature(digest->http_context, 0, &chlg_desc,
                                        0);
    अगर(status == SEC_E_OK)
      output_token_len = chlg_buf[4].cb_बफर;
    अन्यथा { /* delete the context so a new one can be made */
      infof(तथ्य, "digest_sspi: MakeSignature failed, error 0x%08lx",
            (अचिन्हित दीर्घ)status);
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
  }

  अगर(!digest->http_context) {
    CredHandle credentials;
    SEC_WINNT_AUTH_IDENTITY_EX identity;
    SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
    Sec_बफर resp_buf;
    Sec_बफर_Desc resp_desc;
    अचिन्हित दीर्घ attrs;
    TCHAR *spn;

    /* free the credentials used to make the previous identity */
    Curl_creds_unlink(&digest->creds);

    अगर(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      अगर(Curl_create_sspi_identity(creds->user, creds->passwd, &identity)) {
        curlx_free(output_token);
        वापस CURLE_OUT_OF_MEMORY;
      }

      /* Populate our identity domain */
      अगर(Curl_override_sspi_http_realm((स्थिर अक्षर *)digest->निवेश_token,
                                       &identity)) {
        Curl_sspi_free_identity(&identity);
        curlx_free(output_token);
        वापस CURLE_OUT_OF_MEMORY;
      }

      /* Allow proper cleanup of the identity structure */
      p_identity = &identity;
    }
    अन्यथा
      /* Use the current Windows user */
      p_identity = शून्य;

    अगर(creds)
      Curl_creds_link(&digest->creds, creds);

    /* Acquire our credentials handle */
    status = Curl_pSecFn->AcquireCredentialsHandle(शून्य,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, शून्य,
                                   p_identity, शून्य, शून्य,
                                   &credentials, शून्य);
    अगर(status != SEC_E_OK) {
      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      वापस CURLE_LOGIN_DENIED;
    }

    /* Setup the challenge "input" security buffer if present */
    chlg_desc.ulVersion    = SECBUFFER_VERSION;
    chlg_desc.cBuffers     = 3;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].बफर_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_बफर   = digest->निवेश_token;
    chlg_buf[0].cb_बफर   = curlx_uztoul(digest->निवेश_token_len);
    chlg_buf[1].बफर_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_बफर   = CURL_UNCONST(request);
    chlg_buf[1].cb_बफर   = curlx_uztoul(म_माप((स्थिर अक्षर *)request));
    chlg_buf[2].बफर_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_बफर   = शून्य;
    chlg_buf[2].cb_बफर   = 0;

    /* Setup the response "output" security buffer */
    resp_desc.ulVersion = SECBUFFER_VERSION;
    resp_desc.cBuffers  = 1;
    resp_desc.pBuffers  = &resp_buf;
    resp_buf.बफर_Type = SECBUFFER_TOKEN;
    resp_buf.pv_बफर   = output_token;
    resp_buf.cb_बफर   = curlx_uztoul(token_max);

    spn = curlx_convert_UTF8_to_tchar((स्थिर अक्षर *)uripath);
    अगर(!spn) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      वापस CURLE_OUT_OF_MEMORY;
    }

    /* Allocate our new context handle */
    digest->http_context = curlx_calloc(1, माप(CtxtHandle));
    अगर(!digest->http_context) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);
      curlx_free(spn);
      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);
      वापस CURLE_OUT_OF_MEMORY;
    }

    /* Generate our response message */
    status = Curl_pSecFn->InitializeSecurityContext(&credentials, शून्य,
                                                    spn,
                                                    ISC_REQ_USE_HTTP_STYLE,
                                                    0, 0,
                                                    &chlg_desc, 0,
                                                    digest->http_context,
                                                    &resp_desc, &attrs, शून्य);
    curlx_free(spn);

    अगर(status == SEC_I_COMPLETE_NEEDED ||
       status == SEC_I_COMPLETE_AND_CONTINUE)
      Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
    अन्यथा अगर(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
      VERBOSE(अक्षर बफर[STRERROR_LEN]);

      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      curlx_safefree(digest->http_context);

      अगर(status == SEC_E_INSUFFICIENT_MEMORY)
        वापस CURLE_OUT_OF_MEMORY;

      infof(तथ्य, "schannel: InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(status, बफर, माप(बफर)));

      वापस CURLE_AUTH_ERROR;
    }

    output_token_len = resp_buf.cb_बफर;

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_identity(p_identity);
  }

  resp = curlx_memdup0((स्थिर अक्षर *)output_token, output_token_len);
  curlx_free(output_token);
  अगर(!resp) {
    वापस CURLE_OUT_OF_MEMORY;
  }

  /* Return the response */
  *outptr = resp;
  *outlen = output_token_len;
  वापस CURLE_OK;
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
व्योम Curl_auth_digest_cleanup(काष्ठा digestdata *digest)
{
  /* Free the input token */
  curlx_safefree(digest->निवेश_token);

  /* Reset any variables */
  digest->निवेश_token_len = 0;

  /* Delete security context */
  अगर(digest->http_context) {
    Curl_pSecFn->DeleteSecurityContext(digest->http_context);
    curlx_safefree(digest->http_context);
  }

  /* Free the copy of user/passwd used to make the identity for http_context */
  Curl_creds_unlink(&digest->creds);
  Curl_peer_unlink(&digest->origin);
}

#पूर्ण_अगर /* USE_WINDOWS_SSPI && !CURL_DISABLE_DIGEST_AUTH */
