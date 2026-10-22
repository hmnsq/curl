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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_DIGEST_AUTH)

#അന്തര്ഭാവ "creds.h"
#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "vauth/digest.h"
#അന്തര്ഭാവ "curlx/multibyte.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "strerror.h"

/*
 * Curl_auth_is_digest_supported()
 *
 * This is used to evaluate if DIGEST is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if DIGEST is supported by Windows SSPI.
 */
bool Curl_auth_is_digest_supported(ശൂന്യം)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for Digest */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  എങ്കിൽ(status == SEC_E_OK) {
    Curl_pSecFn->FreeContextBuffer(SecurityPackage);
  }

  തിരികെ status == SEC_E_OK;
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
CURLcode Curl_auth_create_digest_md5_message(ഘടന Curl_easy *data,
                                             സ്ഥിരം ഘടന bufref *chlg,
                                             ഘടന Curl_creds *creds,
                                             സ്ഥിരം അക്ഷരം *default_service,
                                             ഘടന bufref *out)
{
  CURLcode result = CURLE_OK;
  TCHAR *spn = ശൂന്യ;
  size_t token_max = 0;
  unsigned അക്ഷരം *output_token = ശൂന്യ;
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
  സ്ഥിരം അക്ഷരം *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Ensure we have a valid challenge message */
  എങ്കിൽ(!Curl_bufref_len(chlg)) {
    infof(data, "DIGEST-MD5 handshake failure (empty challenge message)");
    തിരികെ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Query the security package for DigestSSP */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  എങ്കിൽ(status != SEC_E_OK) {
    failf(data, "SSPI: could not get auth info");
    തിരികെ CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->FreeContextBuffer(SecurityPackage);

  /* Allocate our response buffer */
  output_token = curlx_malloc(token_max);
  എങ്കിൽ(!output_token)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, ശൂന്യ);
  എങ്കിൽ(!spn) {
    curlx_free(output_token);
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  എങ്കിൽ(Curl_creds_has_user(creds)) {
    /* Populate our identity structure */
    result = Curl_create_sspi_identity(creds->user, creds->passwd,
                                       &identity);
    എങ്കിൽ(result) {
      curlx_free(spn);
      curlx_free(output_token);
      തിരികെ result;
    }

    /* Allow proper cleanup of the identity structure */
    p_identity = &identity;
  }
  അല്ലെങ്കിൽ
    /* Use the current Windows user */
    p_identity = ശൂന്യ;

  /* Acquire our credentials handle */
  status = Curl_pSecFn->AcquireCredentialsHandle(ശൂന്യ,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ശൂന്യ,
                                   p_identity, ശൂന്യ, ശൂന്യ,
                                   &credentials, ശൂന്യ);

  എങ്കിൽ(status != SEC_E_OK) {
    Curl_sspi_free_identity(p_identity);
    curlx_free(spn);
    curlx_free(output_token);
    തിരികെ CURLE_LOGIN_DENIED;
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
  status = Curl_pSecFn->InitializeSecurityContext(&credentials, ശൂന്യ, spn,
                                                  0, 0, 0, &chlg_desc, 0,
                                                  &context, &resp_desc, &attrs,
                                                  ശൂന്യ);

  എങ്കിൽ(status == SEC_I_COMPLETE_NEEDED ||
     status == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
  അല്ലെങ്കിൽ എങ്കിൽ(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
    VERBOSE(അക്ഷരം buffer[STRERROR_LEN]);

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_identity(p_identity);
    curlx_free(spn);
    curlx_free(output_token);

    എങ്കിൽ(status == SEC_E_INSUFFICIENT_MEMORY)
      തിരികെ CURLE_OUT_OF_MEMORY;

    infof(data, "schannel: InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(status, buffer, വലുപ്പം(buffer)));

    തിരികെ CURLE_AUTH_ERROR;
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

  തിരികെ result;
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
CURLcode Curl_override_sspi_http_realm(സ്ഥിരം അക്ഷരം *chlg,
                                       SEC_WINNT_AUTH_IDENTITY_EX *identity)
{
  xcharp_u domain, dup_domain;

  /* If domain is blank or unset, check challenge message for realm */
  എങ്കിൽ(!identity->Domain || !identity->DomainLength) {
    വേണ്ടി(;;) {
      ഘടന Curl_str മൂല്യം;
      അക്ഷരം content[DIGEST_MAX_CONTENT_LENGTH];

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Extract a value=content pair */
      എങ്കിൽ(Curl_auth_digest_get_pair(chlg, &മൂല്യം, content, &chlg)) {
        എങ്കിൽ(curlx_str_casecompare(&മൂല്യം, "realm")) {

          /* Setup identity's domain and length */
          domain.tchar_ptr = curlx_convert_UTF8_to_tchar(content);
          എങ്കിൽ(!domain.tchar_ptr)
            തിരികെ CURLE_OUT_OF_MEMORY;

          dup_domain.tchar_ptr = curlx_tcsdup(domain.tchar_ptr);
          എങ്കിൽ(!dup_domain.tchar_ptr) {
            curlx_free(domain.tchar_ptr);
            തിരികെ CURLE_OUT_OF_MEMORY;
          }

          curlx_free(identity->Domain);
          identity->Domain = dup_domain.tbyte_ptr;
          identity->DomainLength = curlx_uztoul(_tcslen(dup_domain.tchar_ptr));
          dup_domain.tchar_ptr = ശൂന്യ;

          curlx_free(domain.tchar_ptr);
        }
        അല്ലെങ്കിൽ {
          /* Unknown specifier, ignore it! */
        }
      }
      അല്ലെങ്കിൽ
        നിർത്തുക; /* We are done here */

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Allow the list to be comma-separated */
      (ശൂന്യം)curlx_str_single(&chlg, ',');
    }
  }

  തിരികെ CURLE_OK;
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
CURLcode Curl_auth_decode_digest_http_message(സ്ഥിരം അക്ഷരം *chlg,
                                              ഘടന digestdata *digest)
{
  size_t chlglen = strlen(chlg);

  /* We had an input token before so if there is another one now that means we
     provided bad credentials in the previous request or it is stale. */
  എങ്കിൽ(digest->നിവേശം_token) {
    bool stale = FALSE;
    സ്ഥിരം അക്ഷരം *p = chlg;

    /* Check for the 'stale' directive */
    വേണ്ടി(;;) {
      ഘടന Curl_str മൂല്യം;
      അക്ഷരം content[DIGEST_MAX_CONTENT_LENGTH];

      ആകുമ്പോൾ(*p && ISBLANK(*p))
        p++;

      എങ്കിൽ(!Curl_auth_digest_get_pair(p, &മൂല്യം, content, &p))
        നിർത്തുക;

      എങ്കിൽ(curlx_str_casecompare(&മൂല്യം, "stale") &&
         curl_strequal(content, "true")) {
        stale = TRUE;
        നിർത്തുക;
      }

      ആകുമ്പോൾ(*p && ISBLANK(*p))
        p++;

      എങ്കിൽ(',' == *p)
        p++;
    }

    എങ്കിൽ(stale)
      Curl_auth_digest_cleanup(digest);
    അല്ലെങ്കിൽ
      തിരികെ CURLE_LOGIN_DENIED;
  }

  /* Store the challenge for use later */
  digest->നിവേശം_token = (BYTE *)curlx_memdup(chlg, chlglen + 1);
  എങ്കിൽ(!digest->നിവേശം_token)
    തിരികെ CURLE_OUT_OF_MEMORY;

  digest->നിവേശം_token_len = chlglen;

  തിരികെ CURLE_OK;
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
CURLcode Curl_auth_create_digest_http_message(ഘടന Curl_easy *data,
                                              ഘടന Curl_creds *creds,
                                              സ്ഥിരം unsigned അക്ഷരം *request,
                                              സ്ഥിരം unsigned അക്ഷരം *uripath,
                                              ഘടന digestdata *digest,
                                              അക്ഷരം **outptr, size_t *outlen)
{
  size_t token_max;
  അക്ഷരം *resp;
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
  എങ്കിൽ(status != SEC_E_OK) {
    failf(data, "SSPI: could not get auth info");
    തിരികെ CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->FreeContextBuffer(SecurityPackage);

  /* Allocate the output buffer according to the max token size as indicated
     by the security package */
  output_token = curlx_malloc(token_max);
  എങ്കിൽ(!output_token) {
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  /* If the user/passwd that was used to make the identity for http_context
     has changed then delete that context. */
  എങ്കിൽ(!Curl_creds_same(creds, digest->creds)) {
    എങ്കിൽ(digest->http_context) {
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
    Curl_creds_unlink(&digest->creds);
  }

  എങ്കിൽ(digest->http_context) {
    chlg_desc.ulVersion    = SECBUFFER_VERSION;
    chlg_desc.cBuffers     = 5;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].BufferType = SECBUFFER_TOKEN;
    chlg_buf[0].pvBuffer   = ശൂന്യ;
    chlg_buf[0].cbBuffer   = 0;
    chlg_buf[1].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pvBuffer   = CURL_UNCONST(request);
    chlg_buf[1].cbBuffer   = curlx_uztoul(strlen((സ്ഥിരം അക്ഷരം *)request));
    chlg_buf[2].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pvBuffer   = CURL_UNCONST(uripath);
    chlg_buf[2].cbBuffer   = curlx_uztoul(strlen((സ്ഥിരം അക്ഷരം *)uripath));
    chlg_buf[3].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[3].pvBuffer   = ശൂന്യ;
    chlg_buf[3].cbBuffer   = 0;
    chlg_buf[4].BufferType = SECBUFFER_PADDING;
    chlg_buf[4].pvBuffer   = output_token;
    chlg_buf[4].cbBuffer   = curlx_uztoul(token_max);

    status = Curl_pSecFn->MakeSignature(digest->http_context, 0, &chlg_desc,
                                        0);
    എങ്കിൽ(status == SEC_E_OK)
      output_token_len = chlg_buf[4].cbBuffer;
    അല്ലെങ്കിൽ { /* delete the context so a new one can be made */
      infof(data, "digest_sspi: MakeSignature failed, error 0x%08lx",
            (unsigned long)status);
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
  }

  എങ്കിൽ(!digest->http_context) {
    CredHandle credentials;
    SEC_WINNT_AUTH_IDENTITY_EX identity;
    SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
    SecBuffer resp_buf;
    SecBufferDesc resp_desc;
    unsigned long attrs;
    TCHAR *spn;

    /* free the credentials used to make the previous identity */
    Curl_creds_unlink(&digest->creds);

    എങ്കിൽ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      എങ്കിൽ(Curl_create_sspi_identity(creds->user, creds->passwd, &identity)) {
        curlx_free(output_token);
        തിരികെ CURLE_OUT_OF_MEMORY;
      }

      /* Populate our identity domain */
      എങ്കിൽ(Curl_override_sspi_http_realm((സ്ഥിരം അക്ഷരം *)digest->നിവേശം_token,
                                       &identity)) {
        Curl_sspi_free_identity(&identity);
        curlx_free(output_token);
        തിരികെ CURLE_OUT_OF_MEMORY;
      }

      /* Allow proper cleanup of the identity structure */
      p_identity = &identity;
    }
    അല്ലെങ്കിൽ
      /* Use the current Windows user */
      p_identity = ശൂന്യ;

    എങ്കിൽ(creds)
      Curl_creds_link(&digest->creds, creds);

    /* Acquire our credentials handle */
    status = Curl_pSecFn->AcquireCredentialsHandle(ശൂന്യ,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ശൂന്യ,
                                   p_identity, ശൂന്യ, ശൂന്യ,
                                   &credentials, ശൂന്യ);
    എങ്കിൽ(status != SEC_E_OK) {
      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      തിരികെ CURLE_LOGIN_DENIED;
    }

    /* Setup the challenge "input" security buffer if present */
    chlg_desc.ulVersion    = SECBUFFER_VERSION;
    chlg_desc.cBuffers     = 3;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].BufferType = SECBUFFER_TOKEN;
    chlg_buf[0].pvBuffer   = digest->നിവേശം_token;
    chlg_buf[0].cbBuffer   = curlx_uztoul(digest->നിവേശം_token_len);
    chlg_buf[1].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pvBuffer   = CURL_UNCONST(request);
    chlg_buf[1].cbBuffer   = curlx_uztoul(strlen((സ്ഥിരം അക്ഷരം *)request));
    chlg_buf[2].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pvBuffer   = ശൂന്യ;
    chlg_buf[2].cbBuffer   = 0;

    /* Setup the response "output" security buffer */
    resp_desc.ulVersion = SECBUFFER_VERSION;
    resp_desc.cBuffers  = 1;
    resp_desc.pBuffers  = &resp_buf;
    resp_buf.BufferType = SECBUFFER_TOKEN;
    resp_buf.pvBuffer   = output_token;
    resp_buf.cbBuffer   = curlx_uztoul(token_max);

    spn = curlx_convert_UTF8_to_tchar((സ്ഥിരം അക്ഷരം *)uripath);
    എങ്കിൽ(!spn) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      തിരികെ CURLE_OUT_OF_MEMORY;
    }

    /* Allocate our new context handle */
    digest->http_context = curlx_calloc(1, വലുപ്പം(CtxtHandle));
    എങ്കിൽ(!digest->http_context) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);
      curlx_free(spn);
      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }

    /* Generate our response message */
    status = Curl_pSecFn->InitializeSecurityContext(&credentials, ശൂന്യ,
                                                    spn,
                                                    ISC_REQ_USE_HTTP_STYLE,
                                                    0, 0,
                                                    &chlg_desc, 0,
                                                    digest->http_context,
                                                    &resp_desc, &attrs, ശൂന്യ);
    curlx_free(spn);

    എങ്കിൽ(status == SEC_I_COMPLETE_NEEDED ||
       status == SEC_I_COMPLETE_AND_CONTINUE)
      Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
    അല്ലെങ്കിൽ എങ്കിൽ(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
      VERBOSE(അക്ഷരം buffer[STRERROR_LEN]);

      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      curlx_safefree(digest->http_context);

      എങ്കിൽ(status == SEC_E_INSUFFICIENT_MEMORY)
        തിരികെ CURLE_OUT_OF_MEMORY;

      infof(data, "schannel: InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(status, buffer, വലുപ്പം(buffer)));

      തിരികെ CURLE_AUTH_ERROR;
    }

    output_token_len = resp_buf.cbBuffer;

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_identity(p_identity);
  }

  resp = curlx_memdup0((സ്ഥിരം അക്ഷരം *)output_token, output_token_len);
  curlx_free(output_token);
  എങ്കിൽ(!resp) {
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  /* Return the response */
  *outptr = resp;
  *outlen = output_token_len;
  തിരികെ CURLE_OK;
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
ശൂന്യം Curl_auth_digest_cleanup(ഘടന digestdata *digest)
{
  /* Free the input token */
  curlx_safefree(digest->നിവേശം_token);

  /* Reset any variables */
  digest->നിവേശം_token_len = 0;

  /* Delete security context */
  എങ്കിൽ(digest->http_context) {
    Curl_pSecFn->DeleteSecurityContext(digest->http_context);
    curlx_safefree(digest->http_context);
  }

  /* Free the copy of user/passwd used to make the identity for http_context */
  Curl_creds_unlink(&digest->creds);
  Curl_peer_unlink(&digest->origin);
}

#endif /* USE_WINDOWS_SSPI && !CURL_DISABLE_DIGEST_AUTH */
