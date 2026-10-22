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
#अन्तर्भाव "curl_setup.h"

#यदि defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_DIGEST_AUTH)

#अन्तर्भाव "creds.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "vauth/digest.h"
#अन्तर्भाव "curlx/multibyte.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "strerror.h"

/*
 * Curl_auth_is_digest_supported()
 *
 * This is used to evaluate if DIGEST is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if DIGEST is supported by Windows SSPI.
 */
bool Curl_auth_is_digest_supported(शून्य)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_स्थितिः स्थितिः;

  /* Query the security package for Digest */
  स्थितिः =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  यदि(स्थितिः == SEC_E_OK) {
    Curl_pSecFn->Free_Context_बफर(SecurityPackage);
  }

  प्रत्यागम स्थितिः == SEC_E_OK;
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
CURLcode Curl_auth_create_digest_md5_सन्देशः(संरचना Curl_easy *दत्त,
                                             ध्रुव संरचना bufref *chlg,
                                             संरचना Curl_creds *creds,
                                             ध्रुव वर्ण *default_service,
                                             संरचना bufref *बहिर्गमनम्)
{
  CURLcode परिणाम = CURLE_OK;
  TCHAR *spn = रिक्त;
  size_t token_max = 0;
  अचिह्नित वर्ण *output_token = रिक्त;
  CredHandle credentials;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  SEC_WINNT_AUTH_तटस्थः_EX तटस्थः;
  SEC_WINNT_AUTH_तटस्थः_EX *p_तटस्थः;
  Sec_बफर chlg_buf;
  Sec_बफर resp_buf;
  Sec_बफर_Desc chlg_desc;
  Sec_बफर_Desc resp_desc;
  SECURITY_स्थितिः स्थितिः;
  अचिह्नित दीर्घ attrs;
  ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  /* Ensure we have a valid challenge message */
  यदि(!Curl_bufref_len(chlg)) {
    infof(दत्त, "DIGEST-MD5 handshake failure (empty challenge message)");
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Query the security package for DigestSSP */
  स्थितिः =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  यदि(स्थितिः != SEC_E_OK) {
    failf(दत्त, "SSPI: could not get auth info");
    प्रत्यागम CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_बफर(SecurityPackage);

  /* Allocate our response buffer */
  output_token = curlx_malloc(token_max);
  यदि(!output_token)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Generate our SPN */
  spn = Curl_auth_build_spn(service, दत्त->state.origin->hostname, रिक्त);
  यदि(!spn) {
    curlx_free(output_token);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  यदि(Curl_creds_has_user(creds)) {
    /* Populate our identity structure */
    परिणाम = Curl_create_sspi_तटस्थः(creds->user, creds->passwd,
                                       &तटस्थः);
    यदि(परिणाम) {
      curlx_free(spn);
      curlx_free(output_token);
      प्रत्यागम परिणाम;
    }

    /* Allow proper cleanup of the identity structure */
    p_तटस्थः = &तटस्थः;
  }
  अन्यथा
    /* Use the current Windows user */
    p_तटस्थः = रिक्त;

  /* Acquire our credentials handle */
  स्थितिः = Curl_pSecFn->AcquireCredentialsHandle(रिक्त,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, रिक्त,
                                   p_तटस्थः, रिक्त, रिक्त,
                                   &credentials, रिक्त);

  यदि(स्थितिः != SEC_E_OK) {
    Curl_sspi_free_तटस्थः(p_तटस्थः);
    curlx_free(spn);
    curlx_free(output_token);
    प्रत्यागम CURLE_LOGIN_DENIED;
  }

  /* Setup the challenge "input" security buffer */
  chlg_desc.ul_संस्करणम् = SECBUFFER_संस्करणम्;
  chlg_desc.cBuffers  = 1;
  chlg_desc.pBuffers  = &chlg_buf;
  chlg_buf.बफर_Type = SECBUFFER_TOKEN;
  chlg_buf.pv_बफर   = CURL_UNCONST(Curl_bufref_ptr(chlg));
  chlg_buf.cb_बफर   = curlx_uztoul(Curl_bufref_len(chlg));

  /* Setup the response "output" security buffer */
  resp_desc.ul_संस्करणम् = SECBUFFER_संस्करणम्;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.बफर_Type = SECBUFFER_TOKEN;
  resp_buf.pv_बफर   = output_token;
  resp_buf.cb_बफर   = curlx_uztoul(token_max);

  /* Generate our response message */
  स्थितिः = Curl_pSecFn->InitializeSecurityContext(&credentials, रिक्त, spn,
                                                  0, 0, 0, &chlg_desc, 0,
                                                  &context, &resp_desc, &attrs,
                                                  रिक्त);

  यदि(स्थितिः == SEC_I_COMPLETE_NEEDED ||
     स्थितिः == SEC_I_COMPLETE_AND_CONTINUE)
    Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
  अन्यथा यदि(स्थितिः != SEC_E_OK && स्थितिः != SEC_I_CONTINUE_NEEDED) {
    VERBOSE(वर्ण बफर[STRERROR_LEN]);

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_तटस्थः(p_तटस्थः);
    curlx_free(spn);
    curlx_free(output_token);

    यदि(स्थितिः == SEC_E_INSUFFICIENT_स्मृतिः)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    infof(दत्त, "schannel: InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(स्थितिः, बफर, परिमाण(बफर)));

    प्रत्यागम CURLE_AUTH_ERROR;
  }

  /* Return the response. */
  Curl_bufref_समुच्चयः(बहिर्गमनम्, output_token, resp_buf.cb_बफर, curl_free);

  /* Free our handles */
  Curl_pSecFn->DeleteSecurityContext(&context);
  Curl_pSecFn->FreeCredentialsHandle(&credentials);

  /* Free the identity structure */
  Curl_sspi_free_तटस्थः(p_तटस्थः);

  /* Free the SPN */
  curlx_free(spn);

  प्रत्यागम परिणाम;
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
CURLcode Curl_override_sspi_http_realm(ध्रुव वर्ण *chlg,
                                       SEC_WINNT_AUTH_तटस्थः_EX *तटस्थः)
{
  xcharp_u domain, dup_domain;

  /* If domain is blank or unset, check challenge message for realm */
  यदि(!तटस्थः->Domain || !तटस्थः->Domain_दीर्घता) {
    प्रति(;;) {
      संरचना Curl_str मानम्;
      वर्ण content[DIGEST_MAX_CONTENT_दीर्घता];

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Extract a value=content pair */
      यदि(Curl_auth_digest_get_pair(chlg, &मानम्, content, &chlg)) {
        यदि(curlx_str_casecompare(&मानम्, "realm")) {

          /* Setup identity's domain and length */
          domain.tchar_ptr = curlx_convert_UTF8_गन्तव्यम्_tchar(content);
          यदि(!domain.tchar_ptr)
            प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

          dup_domain.tchar_ptr = curlx_tcsdup(domain.tchar_ptr);
          यदि(!dup_domain.tchar_ptr) {
            curlx_free(domain.tchar_ptr);
            प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
          }

          curlx_free(तटस्थः->Domain);
          तटस्थः->Domain = dup_domain.tbyte_ptr;
          तटस्थः->Domain_दीर्घता = curlx_uztoul(_tcslen(dup_domain.tchar_ptr));
          dup_domain.tchar_ptr = रिक्त;

          curlx_free(domain.tchar_ptr);
        }
        अन्यथा {
          /* Unknown specifier, ignore it! */
        }
      }
      अन्यथा
        भङ्ग; /* We are done here */

      /* Pass all additional spaces here */
      curlx_str_passblanks(&chlg);

      /* Allow the list to be comma-separated */
      (शून्य)curlx_str_single(&chlg, ',');
    }
  }

  प्रत्यागम CURLE_OK;
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
CURLcode Curl_auth_decode_digest_http_सन्देशः(ध्रुव वर्ण *chlg,
                                              संरचना digestdata *digest)
{
  size_t chlglen = वाक्यदैर्घ्यम्(chlg);

  /* We had an input token before so if there is another one now that means we
     provided bad credentials in the previous request or it is stale. */
  यदि(digest->निवेशः_token) {
    bool stale = FALSE;
    ध्रुव वर्ण *p = chlg;

    /* Check for the 'stale' directive */
    प्रति(;;) {
      संरचना Curl_str मानम्;
      वर्ण content[DIGEST_MAX_CONTENT_दीर्घता];

      यावत्(*p && ISBLANK(*p))
        p++;

      यदि(!Curl_auth_digest_get_pair(p, &मानम्, content, &p))
        भङ्ग;

      यदि(curlx_str_casecompare(&मानम्, "stale") &&
         curl_strequal(content, "true")) {
        stale = TRUE;
        भङ्ग;
      }

      यावत्(*p && ISBLANK(*p))
        p++;

      यदि(',' == *p)
        p++;
    }

    यदि(stale)
      Curl_auth_digest_cleanup(digest);
    अन्यथा
      प्रत्यागम CURLE_LOGIN_DENIED;
  }

  /* Store the challenge for use later */
  digest->निवेशः_token = (BYTE *)curlx_memdup(chlg, chlglen + 1);
  यदि(!digest->निवेशः_token)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  digest->निवेशः_token_len = chlglen;

  प्रत्यागम CURLE_OK;
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
CURLcode Curl_auth_create_digest_http_सन्देशः(संरचना Curl_easy *दत्त,
                                              संरचना Curl_creds *creds,
                                              ध्रुव अचिह्नित वर्ण *request,
                                              ध्रुव अचिह्नित वर्ण *uripath,
                                              संरचना digestdata *digest,
                                              वर्ण **outptr, size_t *outlen)
{
  size_t token_max;
  वर्ण *resp;
  BYTE *output_token;
  size_t output_token_len = 0;
  PSecPkgInfo SecurityPackage;
  Sec_बफर chlg_buf[5];
  Sec_बफर_Desc chlg_desc;
  SECURITY_स्थितिः स्थितिः;

  /* Query the security package for DigestSSP */
  स्थितिः =
    Curl_pSecFn->QuerySecurityPackageInfo(
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   &SecurityPackage);
  यदि(स्थितिः != SEC_E_OK) {
    failf(दत्त, "SSPI: could not get auth info");
    प्रत्यागम CURLE_AUTH_ERROR;
  }

  token_max = SecurityPackage->cbMaxToken;

  /* Release the package buffer as it is not required anymore */
  Curl_pSecFn->Free_Context_बफर(SecurityPackage);

  /* Allocate the output buffer according to the max token size as indicated
     by the security package */
  output_token = curlx_malloc(token_max);
  यदि(!output_token) {
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  /* If the user/passwd that was used to make the identity for http_context
     has changed then delete that context. */
  यदि(!Curl_creds_same(creds, digest->creds)) {
    यदि(digest->http_context) {
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
    Curl_creds_unlink(&digest->creds);
  }

  यदि(digest->http_context) {
    chlg_desc.ul_संस्करणम्    = SECBUFFER_संस्करणम्;
    chlg_desc.cBuffers     = 5;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].बफर_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_बफर   = रिक्त;
    chlg_buf[0].cb_बफर   = 0;
    chlg_buf[1].बफर_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_बफर   = CURL_UNCONST(request);
    chlg_buf[1].cb_बफर   = curlx_uztoul(वाक्यदैर्घ्यम्((ध्रुव वर्ण *)request));
    chlg_buf[2].बफर_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_बफर   = CURL_UNCONST(uripath);
    chlg_buf[2].cb_बफर   = curlx_uztoul(वाक्यदैर्घ्यम्((ध्रुव वर्ण *)uripath));
    chlg_buf[3].बफर_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[3].pv_बफर   = रिक्त;
    chlg_buf[3].cb_बफर   = 0;
    chlg_buf[4].बफर_Type = SECBUFFER_PADDING;
    chlg_buf[4].pv_बफर   = output_token;
    chlg_buf[4].cb_बफर   = curlx_uztoul(token_max);

    स्थितिः = Curl_pSecFn->MakeSignature(digest->http_context, 0, &chlg_desc,
                                        0);
    यदि(स्थितिः == SEC_E_OK)
      output_token_len = chlg_buf[4].cb_बफर;
    अन्यथा { /* delete the context so a new one can be made */
      infof(दत्त, "digest_sspi: MakeSignature failed, error 0x%08lx",
            (अचिह्नित दीर्घ)स्थितिः);
      Curl_pSecFn->DeleteSecurityContext(digest->http_context);
      curlx_safefree(digest->http_context);
    }
  }

  यदि(!digest->http_context) {
    CredHandle credentials;
    SEC_WINNT_AUTH_तटस्थः_EX तटस्थः;
    SEC_WINNT_AUTH_तटस्थः_EX *p_तटस्थः;
    Sec_बफर resp_buf;
    Sec_बफर_Desc resp_desc;
    अचिह्नित दीर्घ attrs;
    TCHAR *spn;

    /* free the credentials used to make the previous identity */
    Curl_creds_unlink(&digest->creds);

    यदि(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      यदि(Curl_create_sspi_तटस्थः(creds->user, creds->passwd, &तटस्थः)) {
        curlx_free(output_token);
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }

      /* Populate our identity domain */
      यदि(Curl_override_sspi_http_realm((ध्रुव वर्ण *)digest->निवेशः_token,
                                       &तटस्थः)) {
        Curl_sspi_free_तटस्थः(&तटस्थः);
        curlx_free(output_token);
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }

      /* Allow proper cleanup of the identity structure */
      p_तटस्थः = &तटस्थः;
    }
    अन्यथा
      /* Use the current Windows user */
      p_तटस्थः = रिक्त;

    यदि(creds)
      Curl_creds_link(&digest->creds, creds);

    /* Acquire our credentials handle */
    स्थितिः = Curl_pSecFn->AcquireCredentialsHandle(रिक्त,
                                   CURL_UNCONST(TEXT(SP_NAME_DIGEST)),
                                   SECPKG_CRED_OUTBOUND, रिक्त,
                                   p_तटस्थः, रिक्त, रिक्त,
                                   &credentials, रिक्त);
    यदि(स्थितिः != SEC_E_OK) {
      Curl_sspi_free_तटस्थः(p_तटस्थः);
      curlx_free(output_token);

      प्रत्यागम CURLE_LOGIN_DENIED;
    }

    /* Setup the challenge "input" security buffer if present */
    chlg_desc.ul_संस्करणम्    = SECBUFFER_संस्करणम्;
    chlg_desc.cBuffers     = 3;
    chlg_desc.pBuffers     = chlg_buf;
    chlg_buf[0].बफर_Type = SECBUFFER_TOKEN;
    chlg_buf[0].pv_बफर   = digest->निवेशः_token;
    chlg_buf[0].cb_बफर   = curlx_uztoul(digest->निवेशः_token_len);
    chlg_buf[1].बफर_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[1].pv_बफर   = CURL_UNCONST(request);
    chlg_buf[1].cb_बफर   = curlx_uztoul(वाक्यदैर्घ्यम्((ध्रुव वर्ण *)request));
    chlg_buf[2].बफर_Type = SECBUFFER_PKG_PARAMS;
    chlg_buf[2].pv_बफर   = रिक्त;
    chlg_buf[2].cb_बफर   = 0;

    /* Setup the response "output" security buffer */
    resp_desc.ul_संस्करणम् = SECBUFFER_संस्करणम्;
    resp_desc.cBuffers  = 1;
    resp_desc.pBuffers  = &resp_buf;
    resp_buf.बफर_Type = SECBUFFER_TOKEN;
    resp_buf.pv_बफर   = output_token;
    resp_buf.cb_बफर   = curlx_uztoul(token_max);

    spn = curlx_convert_UTF8_गन्तव्यम्_tchar((ध्रुव वर्ण *)uripath);
    यदि(!spn) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_तटस्थः(p_तटस्थः);
      curlx_free(output_token);

      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    /* Allocate our new context handle */
    digest->http_context = curlx_calloc(1, परिमाण(CtxtHandle));
    यदि(!digest->http_context) {
      Curl_pSecFn->FreeCredentialsHandle(&credentials);
      curlx_free(spn);
      Curl_sspi_free_तटस्थः(p_तटस्थः);
      curlx_free(output_token);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    /* Generate our response message */
    स्थितिः = Curl_pSecFn->InitializeSecurityContext(&credentials, रिक्त,
                                                    spn,
                                                    ISC_REQ_USE_HTTP_STYLE,
                                                    0, 0,
                                                    &chlg_desc, 0,
                                                    digest->http_context,
                                                    &resp_desc, &attrs, रिक्त);
    curlx_free(spn);

    यदि(स्थितिः == SEC_I_COMPLETE_NEEDED ||
       स्थितिः == SEC_I_COMPLETE_AND_CONTINUE)
      Curl_pSecFn->CompleteAuthToken(&credentials, &resp_desc);
    अन्यथा यदि(स्थितिः != SEC_E_OK && स्थितिः != SEC_I_CONTINUE_NEEDED) {
      VERBOSE(वर्ण बफर[STRERROR_LEN]);

      Curl_pSecFn->FreeCredentialsHandle(&credentials);

      Curl_sspi_free_तटस्थः(p_तटस्थः);
      curlx_free(output_token);

      curlx_safefree(digest->http_context);

      यदि(स्थितिः == SEC_E_INSUFFICIENT_स्मृतिः)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

      infof(दत्त, "schannel: InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(स्थितिः, बफर, परिमाण(बफर)));

      प्रत्यागम CURLE_AUTH_ERROR;
    }

    output_token_len = resp_buf.cb_बफर;

    Curl_pSecFn->FreeCredentialsHandle(&credentials);
    Curl_sspi_free_तटस्थः(p_तटस्थः);
  }

  resp = curlx_memdup0((ध्रुव वर्ण *)output_token, output_token_len);
  curlx_free(output_token);
  यदि(!resp) {
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  /* Return the response */
  *outptr = resp;
  *outlen = output_token_len;
  प्रत्यागम CURLE_OK;
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
शून्य Curl_auth_digest_cleanup(संरचना digestdata *digest)
{
  /* Free the input token */
  curlx_safefree(digest->निवेशः_token);

  /* Reset any variables */
  digest->निवेशः_token_len = 0;

  /* Delete security context */
  यदि(digest->http_context) {
    Curl_pSecFn->DeleteSecurityContext(digest->http_context);
    curlx_safefree(digest->http_context);
  }

  /* Free the copy of user/passwd used to make the identity for http_context */
  Curl_creds_unlink(&digest->creds);
  Curl_peer_unlink(&digest->origin);
}

#यदि_अन्तः /* USE_WINDOWS_SSPI && !CURL_DISABLE_DIGEST_AUTH */
