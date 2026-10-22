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
#ضمن "curl_setup.h"

#إذا defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_DIGEST_AUTH)

#ضمن "creds.h"
#ضمن "vauth/vauth.h"
#ضمن "vauth/digest.h"
#ضمن "curlx/multibyte.h"
#ضمن "curl_trc.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/strparse.h"
#ضمن "strcase.h"
#ضمن "strerror.h"

/*
 * Curl_auth_is_digest_supported()
 *
 * This is used to evaluate if DIGEST is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if DIGEST is supported by Windows SSPI.
 */
bool Curl_auth_is_digest_supported(فراغ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_وضعیت وضعیت;

  /* Query the security package for Digest */
  وضعیت =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(متن_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  إذا(وضعیت == SEC_E_OK) {
    Curl_pSecFn->Free_Context_میانگیر(SecurityPackage);
  }

  أرجع وضعیت == SEC_E_OK;
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
CURLcode Curl_auth_create_digest_md5_پیام(بنية Curl_easy *داده,
                                             ثابت بنية bufref *chlg,
                                             بنية Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             بنية bufref *بیرون)
{
  CURLcode نتیجه = CURLE_OK;
  TCHAR *spn = عدم;
  size_t token_max = 0;
  بیعلامت حرف *output_token = عدم;
  CredHandle credentials;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  SEC_WINNT_AUTH_عنصر_خنثی_EX عنصر_خنثی;
  SEC_WINNT_AUTH_عنصر_خنثی_EX *p_عنصر_خنثی;
  Sec_میانگیر chlg_buf;
  Sec_میانگیر resp_buf;
  Sec_میانگیر_Desc chlg_desc;
  Sec_میانگیر_Desc resp_desc;
  SECURITY_وضعیت وضعیت;
  بیعلامت دراز attrs;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Ensure we have a valid challenge message */
  إذا(!Curl_bufref_len(chlg)) {
    infof(داده, "DIGEST-MD5 handshake failure (empty challenge message)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Query the security package for DigestSSP */
  وضعیت =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(متن_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  إذا(وضعیت != SEC_E_OK) {
    failf(داده, "SSPI: could not get auth info");
    أرجع CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_میانگیر(SecurityPackage);

  /* Allocate our response buffer */
  output_token = curlx_malloc(token_max);
  إذا(!output_token)
    أرجع CURLE_بیرون_از_حافظه;

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, داده->state.origin->hostname, عدم);
  إذا(!spn) {
    curlx_free(output_token);
    أرجع CURLE_بیرون_از_حافظه;
  }

  إذا(Curl_creds_has_user(creds)) {
    /* Populate our identity structure */
    نتیجه = Curl_create_sspi_عنصر_خنثی(creds->user, creds->passwd,
                                       &عنصر_خنثی);
    إذا(نتیجه) {
      curlx_free(spn);
      curlx_free(output_token);
      أرجع نتیجه;
    }

    /* Allow proper cleanup of the identity structure */
    p_عنصر_خنثی = &عنصر_خنثی;
  }
  وإلا
    /* Use the current Windows user */
    p_عنصر_خنثی = عدم;

  /* Acquire our credentials handle */
  وضعیت = Curl_pSecFn->AcquireCredentialsHandle(عدم,
                                   CURL_UNCONST(متن_2(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, عدم,
                                   p_عنصر_خنثی, عدم, عدم,
                                   &credentials, عدم);

  إذا(وضعیت != SEC_E_OK) {
    Curl_sspi_free_عنصر_خنثی(p_عنصر_خنثی);
    curlx_free(spn);
    curlx_free(output_token);
    أرجع CURLE_LOGIN_DENIED;
  }

  /* Setup the challenge "input" security buffer */
  chlg_desc.ul_نسخه = SECBUFFER_نسخه;
  chlg_desc.cBuffers  = 1;
  chlg_desc.pBuffers  = &chlg_buf;
  chlg_buf.میانگیر_Type = SECBUFFER_TOKEN;
  chlg_buf.pv_میانگیر   = CURL_UNCONST(Curl_bufref_ptr(chlg));
  chlg_buf.cb_میانگیر   = curlx_uztoul(Curl_bufref_len(chlg));

  /* Setup the response "output" security buffer */
  resp_desc.ul_نسخه = SECBUFFER_نسخه;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.میانگیر_Type = SECBUFFER_TOKEN;
  resp_buf.pv_میانگیر   = output_token;
  resp_buf.cb_میانگیر   = curlx_uztoul(token_max);

  /* Generate our response message */
  وضعیت = Curl_pSecFn->InitializeSecurityContext(&credentials, عدم, spn,
                                                  0, 0, 0, &chlg_desc, 0,
                                                  &context, &resp_desc, &attrs,
                                                  عدم);

  إذا(وضعیت == SEC_I_COMPLETE_NEEDED ||
     وضعیت == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
  وإلا إذا(وضعیت != SEC_E_OK && وضعیت != SEC_I_CONTINUE_NEEDED) {
    VERBOSE(حرف میانگیر[STRERROR_LEN]);

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_عنصر_خنثی(p_عنصر_خنثی);
    curlx_free(spn);
    curlx_free(output_token);

    إذا(وضعیت == SEC_E_INSUFFICIENT_حافظه)
      أرجع CURLE_بیرون_از_حافظه;

    infof(داده, "schannel: InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(وضعیت, میانگیر, حجم(میانگیر)));

    أرجع CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  Curl_bufref_مجموعه(بیرون, output_token, resp_buf.cb_میانگیر, curl_free);

  /* Free our handles */
  Curl_pSecFn->DeleteSecurityContext(&context);
  Curl_pSecFn->FreeCredentialsHandle(&credentials);

  /* Free the identity structure */
  Curl_sspi_free_عنصر_خنثی(p_عنصر_خنثی);

  /* Free the SPN */
  curlx_free(spn);

  أرجع نتیجه;
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
CURLcode Curl_override_sspi_http_realm(ثابت حرف *chlg,
                                       SEC_WINNT_AUTH_عنصر_خنثی_EX *عنصر_خنثی)
{
  xcharp_u domain, dup_domain;

  /* If domain is blank or unset, check challenge message for realm */
  إذا(!عنصر_خنثی->Domain || !عنصر_خنثی->Domain_درازی) {
    لكل(;;) {
      بنية Curl_str مقدار;
      حرف content[DIGEST_MAX_CONTENT_درازی];

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Extract a value=content pair */
      إذا(Curl_auth_digest_get_pair(chlg, &مقدار, content, &chlg)) {
        إذا(curlx_str_casecompare(&مقدار, "realm")) {

          /* Setup identity's domain and length */
          domain.tchar_ptr = curlx_convert_UTF8_مقصد_tchar(content);
          إذا(!domain.tchar_ptr)
            أرجع CURLE_بیرون_از_حافظه;

          dup_domain.tchar_ptr = curlx_tcsdup(domain.tchar_ptr);
          إذا(!dup_domain.tchar_ptr) {
            curlx_free(domain.tchar_ptr);
            أرجع CURLE_بیرون_از_حافظه;
          }

          curlx_free(عنصر_خنثی->Domain);
          عنصر_خنثی->Domain = dup_domain.tbyte_ptr;
          عنصر_خنثی->Domain_درازی = curlx_uztoul(_tcslen(dup_domain.tchar_ptr));
          dup_domain.tchar_ptr = عدم;

          curlx_free(domain.tchar_ptr);
        }
        وإلا {
          /* Unknown specifier, ignore it! */
        }
      }
      وإلا
        اكسر; /* We are done here */

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Allow the list to be comma-separated */
      (فراغ)curlx_str_single(&chlg, ',');
    }
  }

  أرجع CURLE_OK;
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
CURLcode Curl_auth_decode_digest_http_پیام(ثابت حرف *chlg,
                                              بنية digestdata *digest)
{
  size_t chlglen = طول_رشته(chlg);

  /* We had an input token before so if there is another one now that means we
     provided bad credentials in the previous request or it is stale. */
  إذا(digest->درون_داد_token) {
    bool stale = FALSE;
    ثابت حرف *p = chlg;

    /* Check for the 'stale' directive */
    لكل(;;) {
      بنية Curl_str مقدار;
      حرف content[DIGEST_MAX_CONTENT_درازی];

      طالما(*p && ISBLANK(*p))
        p++;

      إذا(!Curl_auth_digest_get_pair(p, &مقدار, content, &p))
        اكسر;

      إذا(curlx_str_casecompare(&مقدار, "stale") &&
         curl_strequal(content, "true")) {
        stale = TRUE;
        اكسر;
      }

      طالما(*p && ISBLANK(*p))
        p++;

      إذا(',' == *p)
        p++;
    }

    إذا(stale)
      Curl_auth_digest_cleanup(digest);
    وإلا
      أرجع CURLE_LOGIN_DENIED;
  }

  /* Store the challenge for use later */
  digest->درون_داد_token = (BYTE *)curlx_memdup(chlg, chlglen + 1);
  إذا(!digest->درون_داد_token)
    أرجع CURLE_بیرون_از_حافظه;

  digest->درون_داد_token_len = chlglen;

  أرجع CURLE_OK;
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
CURLcode Curl_auth_create_digest_http_پیام(بنية Curl_easy *داده,
                                              بنية Curl_creds *creds,
                                              ثابت بیعلامت حرف *request,
                                              ثابت بیعلامت حرف *uripath,
                                              بنية digestdata *digest,
                                              حرف **outptr, size_t *outlen)
{
  size_t token_max;
  حرف *resp;
  BYTE *output_token;
  size_t output_token_len = 0;
  PSecPkgInfo SecurityPackage;
  Sec_میانگیر chlg_buf[5];
  Sec_میانگیر_Desc chlg_desc;
  SECURITY_وضعیت وضعیت;

  /* Query the security package for DigestSSP */
  وضعیت =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(متن_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  إذا(وضعیت != SEC_E_OK) {
    failf(داده, "SSPI: could not get auth info");
    أرجع CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_میانگیر(SecurityPackage);

  /* Allocate the output buffer according to the max token size as indicated
     by the security package */
  output_token = curlx_malloc(token_max);
  إذا(!output_token) {
    أرجع CURLE_بیرون_از_حافظه;
  }

  /* If the user/passwd that was used to make the identity for http_context
     has changed then delete that context. */
  إذا(!Curl_creds_same(creds, digest->creds)) {
    إذا(digest->http_context) {
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
    Curl_creds_unlink(&digest->creds);
  }

  إذا(digest->http_context) {
    chlg_desc.ul_نسخه    = SECBUFFER_نسخه;
    chlg_desc.cBuffers     = 5;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].میانگیر_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_میانگیر   = عدم;
    chlg_buf[0].cb_میانگیر   = 0;
    chlg_buf[1].میانگیر_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_میانگیر   = CURL_UNCONST(request);
    chlg_buf[1].cb_میانگیر   = curlx_uztoul(طول_رشته((ثابت حرف *)request));
    chlg_buf[2].میانگیر_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_میانگیر   = CURL_UNCONST(uripath);
    chlg_buf[2].cb_میانگیر   = curlx_uztoul(طول_رشته((ثابت حرف *)uripath));
    chlg_buf[3].میانگیر_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[3].pv_میانگیر   = عدم;
    chlg_buf[3].cb_میانگیر   = 0;
    chlg_buf[4].میانگیر_Type = SECBUFFER_PADDING;
    chlg_buf[4].pv_میانگیر   = output_token;
    chlg_buf[4].cb_میانگیر   = curlx_uztoul(token_max);

    وضعیت = Curl_pSecFn->MakeSignature(digest->http_context, 0, &chlg_desc,
                                        0);
    إذا(وضعیت == SEC_E_OK)
      output_token_len = chlg_buf[4].cb_میانگیر;
    وإلا { /* delete the context so a new one can be made */
      infof(داده, "digest_sspi: MakeSignature failed, error 0x%08lx",
            (بیعلامت دراز)وضعیت);
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
  }

  إذا(!digest->http_context) {
    CredHandle credentials;
    SEC_WINNT_AUTH_عنصر_خنثی_EX عنصر_خنثی;
    SEC_WINNT_AUTH_عنصر_خنثی_EX *p_عنصر_خنثی;
    Sec_میانگیر resp_buf;
    Sec_میانگیر_Desc resp_desc;
    بیعلامت دراز attrs;
    TCHAR *spn;

    /* free the credentials used to make the previous identity */
    Curl_creds_unlink(&digest->creds);

    إذا(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      إذا(Curl_create_sspi_عنصر_خنثی(creds->user, creds->passwd, &عنصر_خنثی)) {
        curlx_free(output_token);
        أرجع CURLE_بیرون_از_حافظه;
      }

      /* Populate our identity domain */
      إذا(Curl_override_sspi_http_realm((ثابت حرف *)digest->درون_داد_token,
                                       &عنصر_خنثی)) {
        Curl_sspi_free_عنصر_خنثی(&عنصر_خنثی);
        curlx_free(output_token);
        أرجع CURLE_بیرون_از_حافظه;
      }

      /* Allow proper cleanup of the identity structure */
      p_عنصر_خنثی = &عنصر_خنثی;
    }
    وإلا
      /* Use the current Windows user */
      p_عنصر_خنثی = عدم;

    إذا(creds)
      Curl_creds_link(&digest->creds, creds);

    /* Acquire our credentials handle */
    وضعیت = Curl_pSecFn->AcquireCredentialsHandle(عدم,
                                   CURL_UNCONST(متن_2(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, عدم,
                                   p_عنصر_خنثی, عدم, عدم,
                                   &credentials, عدم);
    إذا(وضعیت != SEC_E_OK) {
      Curl_sspi_free_عنصر_خنثی(p_عنصر_خنثی);
      curlx_free(output_token);

      أرجع CURLE_LOGIN_DENIED;
    }

    /* Setup the challenge "input" security buffer if present */
    chlg_desc.ul_نسخه    = SECBUFFER_نسخه;
    chlg_desc.cBuffers     = 3;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].میانگیر_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_میانگیر   = digest->درون_داد_token;
    chlg_buf[0].cb_میانگیر   = curlx_uztoul(digest->درون_داد_token_len);
    chlg_buf[1].میانگیر_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_میانگیر   = CURL_UNCONST(request);
    chlg_buf[1].cb_میانگیر   = curlx_uztoul(طول_رشته((ثابت حرف *)request));
    chlg_buf[2].میانگیر_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_میانگیر   = عدم;
    chlg_buf[2].cb_میانگیر   = 0;

    /* Setup the response "output" security buffer */
    resp_desc.ul_نسخه = SECBUFFER_نسخه;
    resp_desc.cBuffers  = 1;
    resp_desc.pBuffers  = &resp_buf;
    resp_buf.میانگیر_Type = SECBUFFER_TOKEN;
    resp_buf.pv_میانگیر   = output_token;
    resp_buf.cb_میانگیر   = curlx_uztoul(token_max);

    spn = curlx_convert_UTF8_مقصد_tchar((ثابت حرف *)uripath);
    إذا(!spn) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_عنصر_خنثی(p_عنصر_خنثی);
      curlx_free(output_token);

      أرجع CURLE_بیرون_از_حافظه;
    }

    /* Allocate our new context handle */
    digest->http_context = curlx_calloc(1, حجم(CtxtHandle));
    إذا(!digest->http_context) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);
      curlx_free(spn);
      Curl_sspi_free_عنصر_خنثی(p_عنصر_خنثی);
      curlx_free(output_token);
      أرجع CURLE_بیرون_از_حافظه;
    }

    /* Generate our response message */
    وضعیت = Curl_pSecFn->InitializeSecurityContext(&credentials, عدم,
                                                    spn,
                                                    ISC_REQ_USE_HTTP_STYLE,
                                                    0, 0,
                                                    &chlg_desc, 0,
                                                    digest->http_context,
                                                    &resp_desc, &attrs, عدم);
    curlx_free(spn);

    إذا(وضعیت == SEC_I_COMPLETE_NEEDED ||
       وضعیت == SEC_I_COMPLETE_AND_CONTINUE)
      Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
    وإلا إذا(وضعیت != SEC_E_OK && وضعیت != SEC_I_CONTINUE_NEEDED) {
      VERBOSE(حرف میانگیر[STRERROR_LEN]);

      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_عنصر_خنثی(p_عنصر_خنثی);
      curlx_free(output_token);

      curlx_safefree(digest->http_context);

      إذا(وضعیت == SEC_E_INSUFFICIENT_حافظه)
        أرجع CURLE_بیرون_از_حافظه;

      infof(داده, "schannel: InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(وضعیت, میانگیر, حجم(میانگیر)));

      أرجع CURLE_AUTH_ERROR;
    }

    output_token_len = resp_buf.cb_میانگیر;

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_عنصر_خنثی(p_عنصر_خنثی);
  }

  resp = curlx_memdup0((ثابت حرف *)output_token, output_token_len);
  curlx_free(output_token);
  إذا(!resp) {
    أرجع CURLE_بیرون_از_حافظه;
  }

  /* Return the response */
  *outptr = resp;
  *outlen = output_token_len;
  أرجع CURLE_OK;
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
فراغ Curl_auth_digest_cleanup(بنية digestdata *digest)
{
  /* Free the input token */
  curlx_safefree(digest->درون_داد_token);

  /* Reset any variables */
  digest->درون_داد_token_len = 0;

  /* Delete security context */
  إذا(digest->http_context) {
    Curl_pSecFn->DeleteSecurityContext(digest->http_context);
    curlx_safefree(digest->http_context);
  }

  /* Free the copy of user/passwd used to make the identity for http_context */
  Curl_creds_unlink(&digest->creds);
  Curl_peer_unlink(&digest->origin);
}

#پایان_اگر /* USE_WINDOWS_SSPI && !CURL_DISABLE_DIGEST_AUTH */
