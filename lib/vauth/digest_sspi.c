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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_DIGEST_AUTH)

#ଅନ୍ତର୍ଭାଵ "creds.h"
#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "vauth/digest.h"
#ଅନ୍ତର୍ଭାଵ "curlx/multibyte.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "strerror.h"

/*
 * Curl_auth_is_digest_supported()
 *
 * This is used to evaluate if DIGEST is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if DIGEST is supported by Windows SSPI.
 */
bool Curl_auth_is_digest_supported(ଶୂନ୍ୟ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for Digest */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ଯଦି(status == SEC_E_OK) {
    Curl_pSecFn->FreeContextBuffer(SecurityPackage);
  }

  ଫେରନ୍ତୁ status == SEC_E_OK;
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
CURLcode Curl_auth_create_digest_md5_message(ସଂରଚନା Curl_easy *data,
                                             ଅଚଳ ସଂରଚନା bufref *chlg,
                                             ସଂରଚନା Curl_creds *creds,
                                             ଅଚଳ ଅକ୍ଷର *default_service,
                                             ସଂରଚନା bufref *out)
{
  CURLcode result = CURLE_OK;
  TCHAR *spn = ଖାଲି;
  size_t token_max = 0;
  unsigned ଅକ୍ଷର *output_token = ଖାଲି;
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
  ଅଚଳ ଅକ୍ଷର *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Ensure we have a valid challenge message */
  ଯଦି(!Curl_bufref_len(chlg)) {
    infof(data, "DIGEST-MD5 handshake failure (empty challenge message)");
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Query the security package for DigestSSP */
  status =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  ଯଦି(status != SEC_E_OK) {
    failf(data, "SSPI: could not get auth info");
    ଫେରନ୍ତୁ CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->FreeContextBuffer(SecurityPackage);

  /* Allocate our response buffer */
  output_token = curlx_malloc(token_max);
  ଯଦି(!output_token)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, data->state.origin->hostname, ଖାଲି);
  ଯଦି(!spn) {
    curlx_free(output_token);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଯଦି(Curl_creds_has_user(creds)) {
    /* Populate our identity structure */
    result = Curl_create_sspi_identity(creds->user, creds->passwd,
                                       &identity);
    ଯଦି(result) {
      curlx_free(spn);
      curlx_free(output_token);
      ଫେରନ୍ତୁ result;
    }

    /* Allow proper cleanup of the identity structure */
    p_identity = &identity;
  }
  ଅନ୍ୟଥା
    /* Use the current Windows user */
    p_identity = ଖାଲି;

  /* Acquire our credentials handle */
  status = Curl_pSecFn->AcquireCredentialsHandle(ଖାଲି,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ଖାଲି,
                                   p_identity, ଖାଲି, ଖାଲି,
                                   &credentials, ଖାଲି);

  ଯଦି(status != SEC_E_OK) {
    Curl_sspi_free_identity(p_identity);
    curlx_free(spn);
    curlx_free(output_token);
    ଫେରନ୍ତୁ CURLE_LOGIN_DENIED;
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
  status = Curl_pSecFn->InitializeSecurityContext(&credentials, ଖାଲି, spn,
                                                  0, 0, 0, &chlg_desc, 0,
                                                  &context, &resp_desc, &attrs,
                                                  ଖାଲି);

  ଯଦି(status == SEC_I_COMPLETE_NEEDED ||
     status == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
  ଅନ୍ୟଥା ଯଦି(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
    VERBOSE(ଅକ୍ଷର buffer[STRERROR_LEN]);

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_identity(p_identity);
    curlx_free(spn);
    curlx_free(output_token);

    ଯଦି(status == SEC_E_INSUFFICIENT_MEMORY)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    infof(data, "schannel: InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(status, buffer, ଆକାର(buffer)));

    ଫେରନ୍ତୁ CURLE_AUTH_ERROR;
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

  ଫେରନ୍ତୁ result;
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
CURLcode Curl_override_sspi_http_realm(ଅଚଳ ଅକ୍ଷର *chlg,
                                       SEC_WINNT_AUTH_IDENTITY_EX *identity)
{
  xcharp_u domain, dup_domain;

  /* If domain is blank or unset, check challenge message for realm */
  ଯଦି(!identity->Domain || !identity->DomainLength) {
    ପାଇଁ(;;) {
      ସଂରଚନା Curl_str ମୂଲ୍ୟ;
      ଅକ୍ଷର content[DIGEST_MAX_CONTENT_LENGTH];

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Extract a value=content pair */
      ଯଦି(Curl_auth_digest_get_pair(chlg, &ମୂଲ୍ୟ, content, &chlg)) {
        ଯଦି(curlx_str_casecompare(&ମୂଲ୍ୟ, "realm")) {

          /* Setup identity's domain and length */
          domain.tchar_ptr = curlx_convert_UTF8_to_tchar(content);
          ଯଦି(!domain.tchar_ptr)
            ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

          dup_domain.tchar_ptr = curlx_tcsdup(domain.tchar_ptr);
          ଯଦି(!dup_domain.tchar_ptr) {
            curlx_free(domain.tchar_ptr);
            ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
          }

          curlx_free(identity->Domain);
          identity->Domain = dup_domain.tbyte_ptr;
          identity->DomainLength = curlx_uztoul(_tcslen(dup_domain.tchar_ptr));
          dup_domain.tchar_ptr = ଖାଲି;

          curlx_free(domain.tchar_ptr);
        }
        ଅନ୍ୟଥା {
          /* Unknown specifier, ignore it! */
        }
      }
      ଅନ୍ୟଥା
        ଭାଙ୍ଗ; /* We are done here */

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Allow the list to be comma-separated */
      (ଶୂନ୍ୟ)curlx_str_single(&chlg, ',');
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
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
CURLcode Curl_auth_decode_digest_http_message(ଅଚଳ ଅକ୍ଷର *chlg,
                                              ସଂରଚନା digestdata *digest)
{
  size_t chlglen = strlen(chlg);

  /* We had an input token before so if there is another one now that means we
     provided bad credentials in the previous request or it is stale. */
  ଯଦି(digest->ନିବେଶ_token) {
    bool stale = FALSE;
    ଅଚଳ ଅକ୍ଷର *p = chlg;

    /* Check for the 'stale' directive */
    ପାଇଁ(;;) {
      ସଂରଚନା Curl_str ମୂଲ୍ୟ;
      ଅକ୍ଷର content[DIGEST_MAX_CONTENT_LENGTH];

      ଯେତେବେଳେ(*p && ISBLANK(*p))
        p++;

      ଯଦି(!Curl_auth_digest_get_pair(p, &ମୂଲ୍ୟ, content, &p))
        ଭାଙ୍ଗ;

      ଯଦି(curlx_str_casecompare(&ମୂଲ୍ୟ, "stale") &&
         curl_strequal(content, "true")) {
        stale = TRUE;
        ଭାଙ୍ଗ;
      }

      ଯେତେବେଳେ(*p && ISBLANK(*p))
        p++;

      ଯଦି(',' == *p)
        p++;
    }

    ଯଦି(stale)
      Curl_auth_digest_cleanup(digest);
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLE_LOGIN_DENIED;
  }

  /* Store the challenge for use later */
  digest->ନିବେଶ_token = (BYTE *)curlx_memdup(chlg, chlglen + 1);
  ଯଦି(!digest->ନିବେଶ_token)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  digest->ନିବେଶ_token_len = chlglen;

  ଫେରନ୍ତୁ CURLE_OK;
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
CURLcode Curl_auth_create_digest_http_message(ସଂରଚନା Curl_easy *data,
                                              ସଂରଚନା Curl_creds *creds,
                                              ଅଚଳ unsigned ଅକ୍ଷର *request,
                                              ଅଚଳ unsigned ଅକ୍ଷର *uripath,
                                              ସଂରଚନା digestdata *digest,
                                              ଅକ୍ଷର **outptr, size_t *outlen)
{
  size_t token_max;
  ଅକ୍ଷର *resp;
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
  ଯଦି(status != SEC_E_OK) {
    failf(data, "SSPI: could not get auth info");
    ଫେରନ୍ତୁ CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->FreeContextBuffer(SecurityPackage);

  /* Allocate the output buffer according to the max token size as indicated
     by the security package */
  output_token = curlx_malloc(token_max);
  ଯଦି(!output_token) {
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  /* If the user/passwd that was used to make the identity for http_context
     has changed then delete that context. */
  ଯଦି(!Curl_creds_same(creds, digest->creds)) {
    ଯଦି(digest->http_context) {
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
    Curl_creds_unlink(&digest->creds);
  }

  ଯଦି(digest->http_context) {
    chlg_desc.ulVersion    = SECBUFFER_VERSION;
    chlg_desc.cBuffers     = 5;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].BufferType = SECBUFFER_TOKEN;
    chlg_buf[0].pvBuffer   = ଖାଲି;
    chlg_buf[0].cbBuffer   = 0;
    chlg_buf[1].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pvBuffer   = CURL_UNCONST(request);
    chlg_buf[1].cbBuffer   = curlx_uztoul(strlen((ଅଚଳ ଅକ୍ଷର *)request));
    chlg_buf[2].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pvBuffer   = CURL_UNCONST(uripath);
    chlg_buf[2].cbBuffer   = curlx_uztoul(strlen((ଅଚଳ ଅକ୍ଷର *)uripath));
    chlg_buf[3].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[3].pvBuffer   = ଖାଲି;
    chlg_buf[3].cbBuffer   = 0;
    chlg_buf[4].BufferType = SECBUFFER_PADDING;
    chlg_buf[4].pvBuffer   = output_token;
    chlg_buf[4].cbBuffer   = curlx_uztoul(token_max);

    status = Curl_pSecFn->MakeSignature(digest->http_context, 0, &chlg_desc,
                                        0);
    ଯଦି(status == SEC_E_OK)
      output_token_len = chlg_buf[4].cbBuffer;
    ଅନ୍ୟଥା { /* delete the context so a new one can be made */
      infof(data, "digest_sspi: MakeSignature failed, error 0x%08lx",
            (unsigned long)status);
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
  }

  ଯଦି(!digest->http_context) {
    CredHandle credentials;
    SEC_WINNT_AUTH_IDENTITY_EX identity;
    SEC_WINNT_AUTH_IDENTITY_EX *p_identity;
    SecBuffer resp_buf;
    SecBufferDesc resp_desc;
    unsigned long attrs;
    TCHAR *spn;

    /* free the credentials used to make the previous identity */
    Curl_creds_unlink(&digest->creds);

    ଯଦି(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ଯଦି(Curl_create_sspi_identity(creds->user, creds->passwd, &identity)) {
        curlx_free(output_token);
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }

      /* Populate our identity domain */
      ଯଦି(Curl_override_sspi_http_realm((ଅଚଳ ଅକ୍ଷର *)digest->ନିବେଶ_token,
                                       &identity)) {
        Curl_sspi_free_identity(&identity);
        curlx_free(output_token);
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }

      /* Allow proper cleanup of the identity structure */
      p_identity = &identity;
    }
    ଅନ୍ୟଥା
      /* Use the current Windows user */
      p_identity = ଖାଲି;

    ଯଦି(creds)
      Curl_creds_link(&digest->creds, creds);

    /* Acquire our credentials handle */
    status = Curl_pSecFn->AcquireCredentialsHandle(ଖାଲି,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ଖାଲି,
                                   p_identity, ଖାଲି, ଖାଲି,
                                   &credentials, ଖାଲି);
    ଯଦି(status != SEC_E_OK) {
      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      ଫେରନ୍ତୁ CURLE_LOGIN_DENIED;
    }

    /* Setup the challenge "input" security buffer if present */
    chlg_desc.ulVersion    = SECBUFFER_VERSION;
    chlg_desc.cBuffers     = 3;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].BufferType = SECBUFFER_TOKEN;
    chlg_buf[0].pvBuffer   = digest->ନିବେଶ_token;
    chlg_buf[0].cbBuffer   = curlx_uztoul(digest->ନିବେଶ_token_len);
    chlg_buf[1].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pvBuffer   = CURL_UNCONST(request);
    chlg_buf[1].cbBuffer   = curlx_uztoul(strlen((ଅଚଳ ଅକ୍ଷର *)request));
    chlg_buf[2].BufferType = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pvBuffer   = ଖାଲି;
    chlg_buf[2].cbBuffer   = 0;

    /* Setup the response "output" security buffer */
    resp_desc.ulVersion = SECBUFFER_VERSION;
    resp_desc.cBuffers  = 1;
    resp_desc.pBuffers  = &resp_buf;
    resp_buf.BufferType = SECBUFFER_TOKEN;
    resp_buf.pvBuffer   = output_token;
    resp_buf.cbBuffer   = curlx_uztoul(token_max);

    spn = curlx_convert_UTF8_to_tchar((ଅଚଳ ଅକ୍ଷର *)uripath);
    ଯଦି(!spn) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }

    /* Allocate our new context handle */
    digest->http_context = curlx_calloc(1, ଆକାର(CtxtHandle));
    ଯଦି(!digest->http_context) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);
      curlx_free(spn);
      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }

    /* Generate our response message */
    status = Curl_pSecFn->InitializeSecurityContext(&credentials, ଖାଲି,
                                                    spn,
                                                    ISC_REQ_USE_HTTP_STYLE,
                                                    0, 0,
                                                    &chlg_desc, 0,
                                                    digest->http_context,
                                                    &resp_desc, &attrs, ଖାଲି);
    curlx_free(spn);

    ଯଦି(status == SEC_I_COMPLETE_NEEDED ||
       status == SEC_I_COMPLETE_AND_CONTINUE)
      Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
    ଅନ୍ୟଥା ଯଦି(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
      VERBOSE(ଅକ୍ଷର buffer[STRERROR_LEN]);

      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_identity(p_identity);
      curlx_free(output_token);

      curlx_safefree(digest->http_context);

      ଯଦି(status == SEC_E_INSUFFICIENT_MEMORY)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

      infof(data, "schannel: InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(status, buffer, ଆକାର(buffer)));

      ଫେରନ୍ତୁ CURLE_AUTH_ERROR;
    }

    output_token_len = resp_buf.cbBuffer;

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_identity(p_identity);
  }

  resp = curlx_memdup0((ଅଚଳ ଅକ୍ଷର *)output_token, output_token_len);
  curlx_free(output_token);
  ଯଦି(!resp) {
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  /* Return the response */
  *outptr = resp;
  *outlen = output_token_len;
  ଫେରନ୍ତୁ CURLE_OK;
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
ଶୂନ୍ୟ Curl_auth_digest_cleanup(ସଂରଚନା digestdata *digest)
{
  /* Free the input token */
  curlx_safefree(digest->ନିବେଶ_token);

  /* Reset any variables */
  digest->ନିବେଶ_token_len = 0;

  /* Delete security context */
  ଯଦି(digest->http_context) {
    Curl_pSecFn->DeleteSecurityContext(digest->http_context);
    curlx_safefree(digest->http_context);
  }

  /* Free the copy of user/passwd used to make the identity for http_context */
  Curl_creds_unlink(&digest->creds);
  Curl_peer_unlink(&digest->origin);
}

#endif /* USE_WINDOWS_SSPI && !CURL_DISABLE_DIGEST_AUTH */
