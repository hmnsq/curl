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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_DIGEST_AUTH)

#ਅਨ੍ਤਰ੍ਭਾਵ "creds.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/digest.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/multibyte.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strerror.h"

/*
 * Curl_auth_is_digest_supported()
 *
 * This is used to evaluate if DIGEST is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if DIGEST is supported by Windows SSPI.
 */
bool Curl_auth_is_digest_supported(ਖਾਲੀ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;

  /* Query the security package for Digest */
  ਸਥਿਤੀ =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(ਪਾਠ_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ਜੇ(ਸਥਿਤੀ == SEC_E_OK) {
    Curl_pSecFn->Free_Context_ਬਫਰ(SecurityPackage);
  }

  ਵਾਪਸ ਸਥਿਤੀ == SEC_E_OK;
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
CURLcode Curl_auth_create_digest_md5_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਅਚਲ ਬਣਤਰ bufref *chlg,
                                             ਬਣਤਰ Curl_creds *creds,
                                             ਅਚਲ ਅੱਖਰ *default_service,
                                             ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  TCHAR *spn = ਨਿਰਾ;
  size_t token_max = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output_token = ਨਿਰਾ;
  CredHandle credentials;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX ਪਛਾਣ_ਤੱਤ;
  SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX *p_ਪਛਾਣ_ਤੱਤ;
  Sec_ਬਫਰ chlg_buf;
  Sec_ਬਫਰ resp_buf;
  Sec_ਬਫਰ_Desc chlg_desc;
  Sec_ਬਫਰ_Desc resp_desc;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ attrs;
  ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Ensure we have a valid challenge message */
  ਜੇ(!Curl_bufref_len(chlg)) {
    infof(ਅੰਕੜੇ, "DIGEST-MD5 handshake failure (empty challenge message)");
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Query the security package for DigestSSP */
  ਸਥਿਤੀ =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(ਪਾਠ_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  ਜੇ(ਸਥਿਤੀ != SEC_E_OK) {
    failf(ਅੰਕੜੇ, "SSPI: could not get auth info");
    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_ਬਫਰ(SecurityPackage);

  /* Allocate our response buffer */
  output_token = curlx_malloc(token_max);
  ਜੇ(!output_token)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, ਅੰਕੜੇ->state.origin->hostname, ਨਿਰਾ);
  ਜੇ(!spn) {
    curlx_free(output_token);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(Curl_creds_has_user(creds)) {
    /* Populate our identity structure */
    ਨਤੀਜਾ = Curl_create_sspi_ਪਛਾਣ_ਤੱਤ(creds->user, creds->passwd,
                                       &ਪਛਾਣ_ਤੱਤ);
    ਜੇ(ਨਤੀਜਾ) {
      curlx_free(spn);
      curlx_free(output_token);
      ਵਾਪਸ ਨਤੀਜਾ;
    }

    /* Allow proper cleanup of the identity structure */
    p_ਪਛਾਣ_ਤੱਤ = &ਪਛਾਣ_ਤੱਤ;
  }
  ਨਹੀਂਤਾਂ
    /* Use the current Windows user */
    p_ਪਛਾਣ_ਤੱਤ = ਨਿਰਾ;

  /* Acquire our credentials handle */
  ਸਥਿਤੀ = Curl_pSecFn->AcquireCredentialsHandle(ਨਿਰਾ,
                                   CURL_UNCONST(ਪਾਠ_2(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ਨਿਰਾ,
                                   p_ਪਛਾਣ_ਤੱਤ, ਨਿਰਾ, ਨਿਰਾ,
                                   &credentials, ਨਿਰਾ);

  ਜੇ(ਸਥਿਤੀ != SEC_E_OK) {
    Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(p_ਪਛਾਣ_ਤੱਤ);
    curlx_free(spn);
    curlx_free(output_token);
    ਵਾਪਸ CURLE_LOGIN_DENIED;
  }

  /* Setup the challenge "input" security buffer */
  chlg_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;
  chlg_desc.cBuffers  = 1;
  chlg_desc.pBuffers  = &chlg_buf;
  chlg_buf.ਬਫਰ_Type = SECBUFFER_TOKEN;
  chlg_buf.pv_ਬਫਰ   = CURL_UNCONST(Curl_bufref_ptr(chlg));
  chlg_buf.cb_ਬਫਰ   = curlx_uztoul(Curl_bufref_len(chlg));

  /* Setup the response "output" security buffer */
  resp_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.ਬਫਰ_Type = SECBUFFER_TOKEN;
  resp_buf.pv_ਬਫਰ   = output_token;
  resp_buf.cb_ਬਫਰ   = curlx_uztoul(token_max);

  /* Generate our response message */
  ਸਥਿਤੀ = Curl_pSecFn->InitializeSecurityContext(&credentials, ਨਿਰਾ, spn,
                                                  0, 0, 0, &chlg_desc, 0,
                                                  &context, &resp_desc, &attrs,
                                                  ਨਿਰਾ);

  ਜੇ(ਸਥਿਤੀ == SEC_I_COMPLETE_NEEDED ||
     ਸਥਿਤੀ == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
  ਨਹੀਂਤਾਂ ਜੇ(ਸਥਿਤੀ != SEC_E_OK && ਸਥਿਤੀ != SEC_I_CONTINUE_NEEDED) {
    VERBOSE(ਅੱਖਰ ਬਫਰ[STRERROR_LEN]);

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(p_ਪਛਾਣ_ਤੱਤ);
    curlx_free(spn);
    curlx_free(output_token);

    ਜੇ(ਸਥਿਤੀ == SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    infof(ਅੰਕੜੇ, "schannel: InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));

    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, output_token, resp_buf.cb_ਬਫਰ, curl_free);

  /* Free our handles */
  Curl_pSecFn->DeleteSecurityContext(&context);
  Curl_pSecFn->FreeCredentialsHandle(&credentials);

  /* Free the identity structure */
  Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(p_ਪਛਾਣ_ਤੱਤ);

  /* Free the SPN */
  curlx_free(spn);

  ਵਾਪਸ ਨਤੀਜਾ;
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
CURLcode Curl_override_sspi_http_realm(ਅਚਲ ਅੱਖਰ *chlg,
                                       SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX *ਪਛਾਣ_ਤੱਤ)
{
  xcharp_u domain, dup_domain;

  /* If domain is blank or unset, check challenge message for realm */
  ਜੇ(!ਪਛਾਣ_ਤੱਤ->Domain || !ਪਛਾਣ_ਤੱਤ->Domain_ਲੰਮਾਈ) {
    ਲਈ(;;) {
      ਬਣਤਰ Curl_str ਮੁੱਲ;
      ਅੱਖਰ content[DIGEST_MAX_CONTENT_ਲੰਮਾਈ];

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Extract a value=content pair */
      ਜੇ(Curl_auth_digest_get_pair(chlg, &ਮੁੱਲ, content, &chlg)) {
        ਜੇ(curlx_str_casecompare(&ਮੁੱਲ, "realm")) {

          /* Setup identity's domain and length */
          domain.tchar_ptr = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(content);
          ਜੇ(!domain.tchar_ptr)
            ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

          dup_domain.tchar_ptr = curlx_tcsdup(domain.tchar_ptr);
          ਜੇ(!dup_domain.tchar_ptr) {
            curlx_free(domain.tchar_ptr);
            ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
          }

          curlx_free(ਪਛਾਣ_ਤੱਤ->Domain);
          ਪਛਾਣ_ਤੱਤ->Domain = dup_domain.tbyte_ptr;
          ਪਛਾਣ_ਤੱਤ->Domain_ਲੰਮਾਈ = curlx_uztoul(_tcslen(dup_domain.tchar_ptr));
          dup_domain.tchar_ptr = ਨਿਰਾ;

          curlx_free(domain.tchar_ptr);
        }
        ਨਹੀਂਤਾਂ {
          /* Unknown specifier, ignore it! */
        }
      }
      ਨਹੀਂਤਾਂ
        ਰੋਕੋ; /* We are done here */

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Allow the list to be comma-separated */
      (ਖਾਲੀ)curlx_str_single(&chlg, ',');
    }
  }

  ਵਾਪਸ CURLE_OK;
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
CURLcode Curl_auth_decode_digest_http_ਸੁਨੇਹਾ(ਅਚਲ ਅੱਖਰ *chlg,
                                              ਬਣਤਰ digestdata *digest)
{
  size_t chlglen = ਲੜੀ_ਲੰਬਾਈ(chlg);

  /* We had an input token before so if there is another one now that means we
     provided bad credentials in the previous request or it is stale. */
  ਜੇ(digest->ਨਿਵੇਸ਼_token) {
    bool stale = FALSE;
    ਅਚਲ ਅੱਖਰ *p = chlg;

    /* Check for the 'stale' directive */
    ਲਈ(;;) {
      ਬਣਤਰ Curl_str ਮੁੱਲ;
      ਅੱਖਰ content[DIGEST_MAX_CONTENT_ਲੰਮਾਈ];

      ਜਦੋਂ(*p && ISBLANK(*p))
        p++;

      ਜੇ(!Curl_auth_digest_get_pair(p, &ਮੁੱਲ, content, &p))
        ਰੋਕੋ;

      ਜੇ(curlx_str_casecompare(&ਮੁੱਲ, "stale") &&
         curl_strequal(content, "true")) {
        stale = TRUE;
        ਰੋਕੋ;
      }

      ਜਦੋਂ(*p && ISBLANK(*p))
        p++;

      ਜੇ(',' == *p)
        p++;
    }

    ਜੇ(stale)
      Curl_auth_digest_cleanup(digest);
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_LOGIN_DENIED;
  }

  /* Store the challenge for use later */
  digest->ਨਿਵੇਸ਼_token = (BYTE *)curlx_memdup(chlg, chlglen + 1);
  ਜੇ(!digest->ਨਿਵੇਸ਼_token)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  digest->ਨਿਵੇਸ਼_token_len = chlglen;

  ਵਾਪਸ CURLE_OK;
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
CURLcode Curl_auth_create_digest_http_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਬਣਤਰ Curl_creds *creds,
                                              ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *request,
                                              ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *uripath,
                                              ਬਣਤਰ digestdata *digest,
                                              ਅੱਖਰ **outptr, size_t *outlen)
{
  size_t token_max;
  ਅੱਖਰ *resp;
  BYTE *output_token;
  size_t output_token_len = 0;
  PSecPkgInfo SecurityPackage;
  Sec_ਬਫਰ chlg_buf[5];
  Sec_ਬਫਰ_Desc chlg_desc;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;

  /* Query the security package for DigestSSP */
  ਸਥਿਤੀ =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(ਪਾਠ_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  ਜੇ(ਸਥਿਤੀ != SEC_E_OK) {
    failf(ਅੰਕੜੇ, "SSPI: could not get auth info");
    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_ਬਫਰ(SecurityPackage);

  /* Allocate the output buffer according to the max token size as indicated
     by the security package */
  output_token = curlx_malloc(token_max);
  ਜੇ(!output_token) {
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  /* If the user/passwd that was used to make the identity for http_context
     has changed then delete that context. */
  ਜੇ(!Curl_creds_same(creds, digest->creds)) {
    ਜੇ(digest->http_context) {
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
    Curl_creds_unlink(&digest->creds);
  }

  ਜੇ(digest->http_context) {
    chlg_desc.ul_ਸੰਸਕਰਣ    = SECBUFFER_ਸੰਸਕਰਣ;
    chlg_desc.cBuffers     = 5;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].ਬਫਰ_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_ਬਫਰ   = ਨਿਰਾ;
    chlg_buf[0].cb_ਬਫਰ   = 0;
    chlg_buf[1].ਬਫਰ_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_ਬਫਰ   = CURL_UNCONST(request);
    chlg_buf[1].cb_ਬਫਰ   = curlx_uztoul(ਲੜੀ_ਲੰਬਾਈ((ਅਚਲ ਅੱਖਰ *)request));
    chlg_buf[2].ਬਫਰ_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_ਬਫਰ   = CURL_UNCONST(uripath);
    chlg_buf[2].cb_ਬਫਰ   = curlx_uztoul(ਲੜੀ_ਲੰਬਾਈ((ਅਚਲ ਅੱਖਰ *)uripath));
    chlg_buf[3].ਬਫਰ_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[3].pv_ਬਫਰ   = ਨਿਰਾ;
    chlg_buf[3].cb_ਬਫਰ   = 0;
    chlg_buf[4].ਬਫਰ_Type = SECBUFFER_PADDING;
    chlg_buf[4].pv_ਬਫਰ   = output_token;
    chlg_buf[4].cb_ਬਫਰ   = curlx_uztoul(token_max);

    ਸਥਿਤੀ = Curl_pSecFn->MakeSignature(digest->http_context, 0, &chlg_desc,
                                        0);
    ਜੇ(ਸਥਿਤੀ == SEC_E_OK)
      output_token_len = chlg_buf[4].cb_ਬਫਰ;
    ਨਹੀਂਤਾਂ { /* delete the context so a new one can be made */
      infof(ਅੰਕੜੇ, "digest_sspi: MakeSignature failed, error 0x%08lx",
            (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਸਥਿਤੀ);
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
  }

  ਜੇ(!digest->http_context) {
    CredHandle credentials;
    SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX ਪਛਾਣ_ਤੱਤ;
    SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX *p_ਪਛਾਣ_ਤੱਤ;
    Sec_ਬਫਰ resp_buf;
    Sec_ਬਫਰ_Desc resp_desc;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ attrs;
    TCHAR *spn;

    /* free the credentials used to make the previous identity */
    Curl_creds_unlink(&digest->creds);

    ਜੇ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ਜੇ(Curl_create_sspi_ਪਛਾਣ_ਤੱਤ(creds->user, creds->passwd, &ਪਛਾਣ_ਤੱਤ)) {
        curlx_free(output_token);
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }

      /* Populate our identity domain */
      ਜੇ(Curl_override_sspi_http_realm((ਅਚਲ ਅੱਖਰ *)digest->ਨਿਵੇਸ਼_token,
                                       &ਪਛਾਣ_ਤੱਤ)) {
        Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(&ਪਛਾਣ_ਤੱਤ);
        curlx_free(output_token);
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }

      /* Allow proper cleanup of the identity structure */
      p_ਪਛਾਣ_ਤੱਤ = &ਪਛਾਣ_ਤੱਤ;
    }
    ਨਹੀਂਤਾਂ
      /* Use the current Windows user */
      p_ਪਛਾਣ_ਤੱਤ = ਨਿਰਾ;

    ਜੇ(creds)
      Curl_creds_link(&digest->creds, creds);

    /* Acquire our credentials handle */
    ਸਥਿਤੀ = Curl_pSecFn->AcquireCredentialsHandle(ਨਿਰਾ,
                                   CURL_UNCONST(ਪਾਠ_2(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, ਨਿਰਾ,
                                   p_ਪਛਾਣ_ਤੱਤ, ਨਿਰਾ, ਨਿਰਾ,
                                   &credentials, ਨਿਰਾ);
    ਜੇ(ਸਥਿਤੀ != SEC_E_OK) {
      Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(p_ਪਛਾਣ_ਤੱਤ);
      curlx_free(output_token);

      ਵਾਪਸ CURLE_LOGIN_DENIED;
    }

    /* Setup the challenge "input" security buffer if present */
    chlg_desc.ul_ਸੰਸਕਰਣ    = SECBUFFER_ਸੰਸਕਰਣ;
    chlg_desc.cBuffers     = 3;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].ਬਫਰ_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_ਬਫਰ   = digest->ਨਿਵੇਸ਼_token;
    chlg_buf[0].cb_ਬਫਰ   = curlx_uztoul(digest->ਨਿਵੇਸ਼_token_len);
    chlg_buf[1].ਬਫਰ_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_ਬਫਰ   = CURL_UNCONST(request);
    chlg_buf[1].cb_ਬਫਰ   = curlx_uztoul(ਲੜੀ_ਲੰਬਾਈ((ਅਚਲ ਅੱਖਰ *)request));
    chlg_buf[2].ਬਫਰ_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_ਬਫਰ   = ਨਿਰਾ;
    chlg_buf[2].cb_ਬਫਰ   = 0;

    /* Setup the response "output" security buffer */
    resp_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;
    resp_desc.cBuffers  = 1;
    resp_desc.pBuffers  = &resp_buf;
    resp_buf.ਬਫਰ_Type = SECBUFFER_TOKEN;
    resp_buf.pv_ਬਫਰ   = output_token;
    resp_buf.cb_ਬਫਰ   = curlx_uztoul(token_max);

    spn = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar((ਅਚਲ ਅੱਖਰ *)uripath);
    ਜੇ(!spn) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(p_ਪਛਾਣ_ਤੱਤ);
      curlx_free(output_token);

      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    /* Allocate our new context handle */
    digest->http_context = curlx_calloc(1, ਆਕਾਰ(CtxtHandle));
    ਜੇ(!digest->http_context) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);
      curlx_free(spn);
      Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(p_ਪਛਾਣ_ਤੱਤ);
      curlx_free(output_token);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    /* Generate our response message */
    ਸਥਿਤੀ = Curl_pSecFn->InitializeSecurityContext(&credentials, ਨਿਰਾ,
                                                    spn,
                                                    ISC_REQ_USE_HTTP_STYLE,
                                                    0, 0,
                                                    &chlg_desc, 0,
                                                    digest->http_context,
                                                    &resp_desc, &attrs, ਨਿਰਾ);
    curlx_free(spn);

    ਜੇ(ਸਥਿਤੀ == SEC_I_COMPLETE_NEEDED ||
       ਸਥਿਤੀ == SEC_I_COMPLETE_AND_CONTINUE)
      Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
    ਨਹੀਂਤਾਂ ਜੇ(ਸਥਿਤੀ != SEC_E_OK && ਸਥਿਤੀ != SEC_I_CONTINUE_NEEDED) {
      VERBOSE(ਅੱਖਰ ਬਫਰ[STRERROR_LEN]);

      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(p_ਪਛਾਣ_ਤੱਤ);
      curlx_free(output_token);

      curlx_safefree(digest->http_context);

      ਜੇ(ਸਥਿਤੀ == SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      infof(ਅੰਕੜੇ, "schannel: InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));

      ਵਾਪਸ CURLE_AUTH_ERROR;
    }

    output_token_len = resp_buf.cb_ਬਫਰ;

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(p_ਪਛਾਣ_ਤੱਤ);
  }

  resp = curlx_memdup0((ਅਚਲ ਅੱਖਰ *)output_token, output_token_len);
  curlx_free(output_token);
  ਜੇ(!resp) {
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  /* Return the response */
  *outptr = resp;
  *outlen = output_token_len;
  ਵਾਪਸ CURLE_OK;
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
ਖਾਲੀ Curl_auth_digest_cleanup(ਬਣਤਰ digestdata *digest)
{
  /* Free the input token */
  curlx_safefree(digest->ਨਿਵੇਸ਼_token);

  /* Reset any variables */
  digest->ਨਿਵੇਸ਼_token_len = 0;

  /* Delete security context */
  ਜੇ(digest->http_context) {
    Curl_pSecFn->DeleteSecurityContext(digest->http_context);
    curlx_safefree(digest->http_context);
  }

  /* Free the copy of user/passwd used to make the identity for http_context */
  Curl_creds_unlink(&digest->creds);
  Curl_peer_unlink(&digest->origin);
}

#ਜੇ_ਅੰਤ /* USE_WINDOWS_SSPI && !CURL_DISABLE_DIGEST_AUTH */
