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
#شامل "curl_setup.h"

#اگر defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_DIGEST_AUTH)

#شامل "creds.h"
#شامل "vauth/vauth.h"
#شامل "vauth/digest.h"
#شامل "curlx/multibyte.h"
#شامل "curl_trc.h"
#شامل "curlx/strdup.h"
#شامل "curlx/strparse.h"
#شامل "strcase.h"
#شامل "strerror.h"

/*
 * Curl_auth_is_digest_supported()
 *
 * This is used to evaluate if DIGEST is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if DIGEST is supported by Windows SSPI.
 */
bool Curl_auth_is_digest_supported(خالی)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_حالت حالت_2;

  /* Query the security package for Digest */
  حالت_2 =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(متن_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  اگر(حالت_2 == SEC_E_OK) {
    Curl_pSecFn->Free_Context_بفر(SecurityPackage);
  }

  واپس حالت_2 == SEC_E_OK;
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
CURLcode Curl_auth_create_digest_md5_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ثابت ساخت bufref *chlg,
                                             ساخت Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             ساخت bufref *برآمد)
{
  CURLcode نتیجہ = CURLE_OK;
  TCHAR *spn = تہی;
  size_t token_max = 0;
  غیرعلامتی حرف *output_token = تہی;
  CredHandle credentials;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  SEC_WINNT_AUTH_شناختی_عنصر_EX شناختی_عنصر;
  SEC_WINNT_AUTH_شناختی_عنصر_EX *p_شناختی_عنصر;
  Sec_بفر chlg_buf;
  Sec_بفر resp_buf;
  Sec_بفر_Desc chlg_desc;
  Sec_بفر_Desc resp_desc;
  SECURITY_حالت حالت_2;
  غیرعلامتی طویل attrs;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Ensure we have a valid challenge message */
  اگر(!Curl_bufref_len(chlg)) {
    infof(ڈیٹا, "DIGEST-MD5 handshake failure (empty challenge message)");
    واپس CURLE_BAD_CONTENT_ENCODING;
  }

  /* Query the security package for DigestSSP */
  حالت_2 =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(متن_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  اگر(حالت_2 != SEC_E_OK) {
    failf(ڈیٹا, "SSPI: could not get auth info");
    واپس CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_بفر(SecurityPackage);

  /* Allocate our response buffer */
  output_token = curlx_malloc(token_max);
  اگر(!output_token)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, ڈیٹا->state.origin->hostname, تہی);
  اگر(!spn) {
    curlx_free(output_token);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  اگر(Curl_creds_has_user(creds)) {
    /* Populate our identity structure */
    نتیجہ = Curl_create_sspi_شناختی_عنصر(creds->user, creds->passwd,
                                       &شناختی_عنصر);
    اگر(نتیجہ) {
      curlx_free(spn);
      curlx_free(output_token);
      واپس نتیجہ;
    }

    /* Allow proper cleanup of the identity structure */
    p_شناختی_عنصر = &شناختی_عنصر;
  }
  ورنہ
    /* Use the current Windows user */
    p_شناختی_عنصر = تہی;

  /* Acquire our credentials handle */
  حالت_2 = Curl_pSecFn->AcquireCredentialsHandle(تہی,
                                   CURL_UNCONST(متن_2(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, تہی,
                                   p_شناختی_عنصر, تہی, تہی,
                                   &credentials, تہی);

  اگر(حالت_2 != SEC_E_OK) {
    Curl_sspi_free_شناختی_عنصر(p_شناختی_عنصر);
    curlx_free(spn);
    curlx_free(output_token);
    واپس CURLE_LOGIN_DENIED;
  }

  /* Setup the challenge "input" security buffer */
  chlg_desc.ul_نسخہ = SECBUFFER_نسخہ;
  chlg_desc.cBuffers  = 1;
  chlg_desc.pBuffers  = &chlg_buf;
  chlg_buf.بفر_Type = SECBUFFER_TOKEN;
  chlg_buf.pv_بفر   = CURL_UNCONST(Curl_bufref_ptr(chlg));
  chlg_buf.cb_بفر   = curlx_uztoul(Curl_bufref_len(chlg));

  /* Setup the response "output" security buffer */
  resp_desc.ul_نسخہ = SECBUFFER_نسخہ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.بفر_Type = SECBUFFER_TOKEN;
  resp_buf.pv_بفر   = output_token;
  resp_buf.cb_بفر   = curlx_uztoul(token_max);

  /* Generate our response message */
  حالت_2 = Curl_pSecFn->InitializeSecurityContext(&credentials, تہی, spn,
                                                  0, 0, 0, &chlg_desc, 0,
                                                  &context, &resp_desc, &attrs,
                                                  تہی);

  اگر(حالت_2 == SEC_I_COMPLETE_NEEDED ||
     حالت_2 == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
  ورنہ اگر(حالت_2 != SEC_E_OK && حالت_2 != SEC_I_CONTINUE_NEEDED) {
    VERBOSE(حرف بفر[STRERROR_LEN]);

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_شناختی_عنصر(p_شناختی_عنصر);
    curlx_free(spn);
    curlx_free(output_token);

    اگر(حالت_2 == SEC_E_INSUFFICIENT_حافظہ)
      واپس CURLE_برآمد_کا_حافظہ;

    infof(ڈیٹا, "schannel: InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(حالت_2, بفر, حجم(بفر)));

    واپس CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  Curl_bufref_سیٹ(برآمد, output_token, resp_buf.cb_بفر, curl_free);

  /* Free our handles */
  Curl_pSecFn->DeleteSecurityContext(&context);
  Curl_pSecFn->FreeCredentialsHandle(&credentials);

  /* Free the identity structure */
  Curl_sspi_free_شناختی_عنصر(p_شناختی_عنصر);

  /* Free the SPN */
  curlx_free(spn);

  واپس نتیجہ;
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
                                       SEC_WINNT_AUTH_شناختی_عنصر_EX *شناختی_عنصر)
{
  xcharp_u domain, dup_domain;

  /* If domain is blank or unset, check challenge message for realm */
  اگر(!شناختی_عنصر->Domain || !شناختی_عنصر->Domain_لمبائی) {
    برائے(;;) {
      ساخت Curl_str قدر;
      حرف content[DIGEST_MAX_CONTENT_لمبائی];

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Extract a value=content pair */
      اگر(Curl_auth_digest_get_pair(chlg, &قدر, content, &chlg)) {
        اگر(curlx_str_casecompare(&قدر, "realm")) {

          /* Setup identity's domain and length */
          domain.tchar_ptr = curlx_convert_UTF8_مقصد_tchar(content);
          اگر(!domain.tchar_ptr)
            واپس CURLE_برآمد_کا_حافظہ;

          dup_domain.tchar_ptr = curlx_tcsdup(domain.tchar_ptr);
          اگر(!dup_domain.tchar_ptr) {
            curlx_free(domain.tchar_ptr);
            واپس CURLE_برآمد_کا_حافظہ;
          }

          curlx_free(شناختی_عنصر->Domain);
          شناختی_عنصر->Domain = dup_domain.tbyte_ptr;
          شناختی_عنصر->Domain_لمبائی = curlx_uztoul(_tcslen(dup_domain.tchar_ptr));
          dup_domain.tchar_ptr = تہی;

          curlx_free(domain.tchar_ptr);
        }
        ورنہ {
          /* Unknown specifier, ignore it! */
        }
      }
      ورنہ
        توڑو; /* We are done here */

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Allow the list to be comma-separated */
      (خالی)curlx_str_single(&chlg, ',');
    }
  }

  واپس CURLE_OK;
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
CURLcode Curl_auth_decode_digest_http_پیغام(ثابت حرف *chlg,
                                              ساخت digestdata *digest)
{
  size_t chlglen = لڑی_لمبائی(chlg);

  /* We had an input token before so if there is another one now that means we
     provided bad credentials in the previous request or it is stale. */
  اگر(digest->داخلہ_token) {
    bool stale = FALSE;
    ثابت حرف *p = chlg;

    /* Check for the 'stale' directive */
    برائے(;;) {
      ساخت Curl_str قدر;
      حرف content[DIGEST_MAX_CONTENT_لمبائی];

      جب(*p && ISBLANK(*p))
        p++;

      اگر(!Curl_auth_digest_get_pair(p, &قدر, content, &p))
        توڑو;

      اگر(curlx_str_casecompare(&قدر, "stale") &&
         curl_strequal(content, "true")) {
        stale = TRUE;
        توڑو;
      }

      جب(*p && ISBLANK(*p))
        p++;

      اگر(',' == *p)
        p++;
    }

    اگر(stale)
      Curl_auth_digest_cleanup(digest);
    ورنہ
      واپس CURLE_LOGIN_DENIED;
  }

  /* Store the challenge for use later */
  digest->داخلہ_token = (BYTE *)curlx_memdup(chlg, chlglen + 1);
  اگر(!digest->داخلہ_token)
    واپس CURLE_برآمد_کا_حافظہ;

  digest->داخلہ_token_len = chlglen;

  واپس CURLE_OK;
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
CURLcode Curl_auth_create_digest_http_پیغام(ساخت Curl_easy *ڈیٹا,
                                              ساخت Curl_creds *creds,
                                              ثابت غیرعلامتی حرف *request,
                                              ثابت غیرعلامتی حرف *uripath,
                                              ساخت digestdata *digest,
                                              حرف **outptr, size_t *outlen)
{
  size_t token_max;
  حرف *resp;
  BYTE *output_token;
  size_t output_token_len = 0;
  PSecPkgInfo SecurityPackage;
  Sec_بفر chlg_buf[5];
  Sec_بفر_Desc chlg_desc;
  SECURITY_حالت حالت_2;

  /* Query the security package for DigestSSP */
  حالت_2 =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(متن_2(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  اگر(حالت_2 != SEC_E_OK) {
    failf(ڈیٹا, "SSPI: could not get auth info");
    واپس CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_بفر(SecurityPackage);

  /* Allocate the output buffer according to the max token size as indicated
     by the security package */
  output_token = curlx_malloc(token_max);
  اگر(!output_token) {
    واپس CURLE_برآمد_کا_حافظہ;
  }

  /* If the user/passwd that was used to make the identity for http_context
     has changed then delete that context. */
  اگر(!Curl_creds_same(creds, digest->creds)) {
    اگر(digest->http_context) {
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
    Curl_creds_unlink(&digest->creds);
  }

  اگر(digest->http_context) {
    chlg_desc.ul_نسخہ    = SECBUFFER_نسخہ;
    chlg_desc.cBuffers     = 5;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].بفر_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_بفر   = تہی;
    chlg_buf[0].cb_بفر   = 0;
    chlg_buf[1].بفر_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_بفر   = CURL_UNCONST(request);
    chlg_buf[1].cb_بفر   = curlx_uztoul(لڑی_لمبائی((ثابت حرف *)request));
    chlg_buf[2].بفر_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_بفر   = CURL_UNCONST(uripath);
    chlg_buf[2].cb_بفر   = curlx_uztoul(لڑی_لمبائی((ثابت حرف *)uripath));
    chlg_buf[3].بفر_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[3].pv_بفر   = تہی;
    chlg_buf[3].cb_بفر   = 0;
    chlg_buf[4].بفر_Type = SECBUFFER_PADDING;
    chlg_buf[4].pv_بفر   = output_token;
    chlg_buf[4].cb_بفر   = curlx_uztoul(token_max);

    حالت_2 = Curl_pSecFn->MakeSignature(digest->http_context, 0, &chlg_desc,
                                        0);
    اگر(حالت_2 == SEC_E_OK)
      output_token_len = chlg_buf[4].cb_بفر;
    ورنہ { /* delete the context so a new one can be made */
      infof(ڈیٹا, "digest_sspi: MakeSignature failed, error 0x%08lx",
            (غیرعلامتی طویل)حالت_2);
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
  }

  اگر(!digest->http_context) {
    CredHandle credentials;
    SEC_WINNT_AUTH_شناختی_عنصر_EX شناختی_عنصر;
    SEC_WINNT_AUTH_شناختی_عنصر_EX *p_شناختی_عنصر;
    Sec_بفر resp_buf;
    Sec_بفر_Desc resp_desc;
    غیرعلامتی طویل attrs;
    TCHAR *spn;

    /* free the credentials used to make the previous identity */
    Curl_creds_unlink(&digest->creds);

    اگر(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      اگر(Curl_create_sspi_شناختی_عنصر(creds->user, creds->passwd, &شناختی_عنصر)) {
        curlx_free(output_token);
        واپس CURLE_برآمد_کا_حافظہ;
      }

      /* Populate our identity domain */
      اگر(Curl_override_sspi_http_realm((ثابت حرف *)digest->داخلہ_token,
                                       &شناختی_عنصر)) {
        Curl_sspi_free_شناختی_عنصر(&شناختی_عنصر);
        curlx_free(output_token);
        واپس CURLE_برآمد_کا_حافظہ;
      }

      /* Allow proper cleanup of the identity structure */
      p_شناختی_عنصر = &شناختی_عنصر;
    }
    ورنہ
      /* Use the current Windows user */
      p_شناختی_عنصر = تہی;

    اگر(creds)
      Curl_creds_link(&digest->creds, creds);

    /* Acquire our credentials handle */
    حالت_2 = Curl_pSecFn->AcquireCredentialsHandle(تہی,
                                   CURL_UNCONST(متن_2(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, تہی,
                                   p_شناختی_عنصر, تہی, تہی,
                                   &credentials, تہی);
    اگر(حالت_2 != SEC_E_OK) {
      Curl_sspi_free_شناختی_عنصر(p_شناختی_عنصر);
      curlx_free(output_token);

      واپس CURLE_LOGIN_DENIED;
    }

    /* Setup the challenge "input" security buffer if present */
    chlg_desc.ul_نسخہ    = SECBUFFER_نسخہ;
    chlg_desc.cBuffers     = 3;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].بفر_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_بفر   = digest->داخلہ_token;
    chlg_buf[0].cb_بفر   = curlx_uztoul(digest->داخلہ_token_len);
    chlg_buf[1].بفر_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_بفر   = CURL_UNCONST(request);
    chlg_buf[1].cb_بفر   = curlx_uztoul(لڑی_لمبائی((ثابت حرف *)request));
    chlg_buf[2].بفر_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_بفر   = تہی;
    chlg_buf[2].cb_بفر   = 0;

    /* Setup the response "output" security buffer */
    resp_desc.ul_نسخہ = SECBUFFER_نسخہ;
    resp_desc.cBuffers  = 1;
    resp_desc.pBuffers  = &resp_buf;
    resp_buf.بفر_Type = SECBUFFER_TOKEN;
    resp_buf.pv_بفر   = output_token;
    resp_buf.cb_بفر   = curlx_uztoul(token_max);

    spn = curlx_convert_UTF8_مقصد_tchar((ثابت حرف *)uripath);
    اگر(!spn) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_شناختی_عنصر(p_شناختی_عنصر);
      curlx_free(output_token);

      واپس CURLE_برآمد_کا_حافظہ;
    }

    /* Allocate our new context handle */
    digest->http_context = curlx_calloc(1, حجم(CtxtHandle));
    اگر(!digest->http_context) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);
      curlx_free(spn);
      Curl_sspi_free_شناختی_عنصر(p_شناختی_عنصر);
      curlx_free(output_token);
      واپس CURLE_برآمد_کا_حافظہ;
    }

    /* Generate our response message */
    حالت_2 = Curl_pSecFn->InitializeSecurityContext(&credentials, تہی,
                                                    spn,
                                                    ISC_REQ_USE_HTTP_STYLE,
                                                    0, 0,
                                                    &chlg_desc, 0,
                                                    digest->http_context,
                                                    &resp_desc, &attrs, تہی);
    curlx_free(spn);

    اگر(حالت_2 == SEC_I_COMPLETE_NEEDED ||
       حالت_2 == SEC_I_COMPLETE_AND_CONTINUE)
      Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
    ورنہ اگر(حالت_2 != SEC_E_OK && حالت_2 != SEC_I_CONTINUE_NEEDED) {
      VERBOSE(حرف بفر[STRERROR_LEN]);

      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_شناختی_عنصر(p_شناختی_عنصر);
      curlx_free(output_token);

      curlx_safefree(digest->http_context);

      اگر(حالت_2 == SEC_E_INSUFFICIENT_حافظہ)
        واپس CURLE_برآمد_کا_حافظہ;

      infof(ڈیٹا, "schannel: InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(حالت_2, بفر, حجم(بفر)));

      واپس CURLE_AUTH_ERROR;
    }

    output_token_len = resp_buf.cb_بفر;

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_شناختی_عنصر(p_شناختی_عنصر);
  }

  resp = curlx_memdup0((ثابت حرف *)output_token, output_token_len);
  curlx_free(output_token);
  اگر(!resp) {
    واپس CURLE_برآمد_کا_حافظہ;
  }

  /* Return the response */
  *outptr = resp;
  *outlen = output_token_len;
  واپس CURLE_OK;
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
خالی Curl_auth_digest_cleanup(ساخت digestdata *digest)
{
  /* Free the input token */
  curlx_safefree(digest->داخلہ_token);

  /* Reset any variables */
  digest->داخلہ_token_len = 0;

  /* Delete security context */
  اگر(digest->http_context) {
    Curl_pSecFn->DeleteSecurityContext(digest->http_context);
    curlx_safefree(digest->http_context);
  }

  /* Free the copy of user/passwd used to make the identity for http_context */
  Curl_creds_unlink(&digest->creds);
  Curl_peer_unlink(&digest->origin);
}

#اگر_ختم /* USE_WINDOWS_SSPI && !CURL_DISABLE_DIGEST_AUTH */
