/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>.
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
 * RFC4752 The Kerberos V5 ("GSSAPI") SASL Mechanism
 *
 ***************************************************************************/
#অন্তর্ভাব "curl_setup.h"

#যদি defined(USE_WINDOWS_SSPI) && defined(USE_KERBEROS5)

#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "curl_trc.h"

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by Windows SSPI.
 */
bool Curl_auth_is_gssapi_supported(শূন্য)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for Kerberos */
  status = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  যদি(status == SEC_E_OK) {
    Curl_pSecFn->Free_Context_বাফার(SecurityPackage);
  }

  ফেরত status == SEC_E_OK;
}

/*
 * Curl_auth_create_gssapi_user_message()
 *
 * This is used to generate an already encoded GSSAPI (Kerberos V5) user token
 * message ready for sending to the recipient.
 *
 * Parameters:
 *
 * data        [in]     - The session handle.
 * userp       [in]     - The username in the format User or Domain\User.
 * passwdp     [in]     - The user's password.
 * service     [in]     - The service type such as http, smtp, pop or imap.
 * host        [in]     - The hostname.
 * mutual_auth [in]     - Flag specifying whether or not mutual authentication
 *                        is enabled.
 * chlg        [in]     - Optional challenge message.
 * krb5        [in/out] - The Kerberos 5 data struct being used and modified.
 * out         [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_gssapi_user_message(কাঠামো Curl_easy *তথ্য,
                                              কাঠামো Curl_creds *creds,
                                              ধ্রুবক অক্ষর *default_service,
                                              ধ্রুবক অক্ষর *host,
                                              ধ্রুবক bool mutual_auth,
                                              ধ্রুবক কাঠামো bufref *chlg,
                                              কাঠামো kerberos5data *krb5,
                                              কাঠামো bufref *out)
{
  CURLcode ফলাফল = CURLE_OK;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  Sec_বাফার chlg_buf;
  Sec_বাফার resp_buf;
  Sec_বাফার_Desc chlg_desc;
  Sec_বাফার_Desc resp_desc;
  SECURITY_STATUS status;
  unsigned long attrs;
  ধ্রুবক অক্ষর *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  যদি(!krb5->spn) {
    /* Generate our SPN */
    krb5->spn = Curl_auth_build_spn(service, host, ফাঁকা);
    যদি(!krb5->spn)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(!krb5->output_token) {
    /* Query the security package for Kerberos */
    status = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 &SecurityPackage);
    যদি(status != SEC_E_OK) {
      failf(তথ্য, "SSPI: could not get auth info");
      ফেরত CURLE_AUTH_ERROR;
    }

    krb5->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_বাফার(SecurityPackage);

    /* Allocate our response buffer */
    krb5->output_token = curlx_malloc(krb5->token_max);
    যদি(!krb5->output_token)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(!krb5->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    যদি(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ফলাফল = Curl_create_sspi_identity(
        creds->user, creds->passwd, &krb5->identity);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      /* Allow proper cleanup of the identity structure */
      krb5->p_identity = &krb5->identity;
    }
    অন্যথা
      /* Use the current Windows user */
      krb5->p_identity = ফাঁকা;

    /* Allocate our credentials handle */
    krb5->credentials = curlx_calloc(1, আকার(CredHandle));
    যদি(!krb5->credentials)
      ফেরত CURLE_OUT_OF_MEMORY;

    /* Acquire our credentials handle */
    status = Curl_pSecFn->AcquireCredentialsHandle(ফাঁকা,
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 SECPKG_CRED_OUTBOUND, ফাঁকা,
                                 krb5->p_identity, ফাঁকা, ফাঁকা,
                                 krb5->credentials, ফাঁকা);
    যদি(status != SEC_E_OK) {
      curlx_safefree(krb5->credentials);
      ফেরত CURLE_LOGIN_DENIED;
    }

    /* Allocate our new context handle */
    krb5->context = curlx_calloc(1, আকার(CtxtHandle));
    যদি(!krb5->context)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(chlg) {
    যদি(!Curl_bufref_len(chlg)) {
      infof(তথ্য, "GSSAPI handshake failure (empty challenge message)");
      ফেরত CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.ulVersion = SECBUFFER_VERSION;
    chlg_desc.cBuffers  = 1;
    chlg_desc.pBuffers  = &chlg_buf;
    chlg_buf.বাফার_Type = SECBUFFER_TOKEN;
    chlg_buf.pv_বাফার   = CURL_UNCONST(Curl_bufref_ptr(chlg));
    chlg_buf.cb_বাফার   = curlx_uztoul(Curl_bufref_len(chlg));
  }

  /* Setup the response "output" security buffer */
  resp_desc.ulVersion = SECBUFFER_VERSION;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.বাফার_Type = SECBUFFER_TOKEN;
  resp_buf.pv_বাফার   = krb5->output_token;
  resp_buf.cb_বাফার   = curlx_uztoul(krb5->token_max);

  /* Generate our challenge-response message */
  status = Curl_pSecFn->InitializeSecurityContext(krb5->credentials,
                                               chlg ? krb5->context : ফাঁকা,
                                               krb5->spn,
                                               (mutual_auth ?
                                                ISC_REQ_MUTUAL_AUTH : 0),
                                               0, SECURITY_NATIVE_DREP,
                                               chlg ? &chlg_desc : ফাঁকা, 0,
                                               &context,
                                               &resp_desc, &attrs, ফাঁকা);

  যদি(status == SEC_E_INSUFFICIENT_MEMORY)
    ফেরত CURLE_OUT_OF_MEMORY;

  যদি(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED)
    ফেরত CURLE_AUTH_ERROR;

  যদি(memcmp(&context, krb5->context, আকার(context))) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);

    memcpy(krb5->context, &context, আকার(context));
  }

  যদি(resp_buf.cb_বাফার) {
    ফলাফল = Curl_bufref_memdup0(out, resp_buf.pv_বাফার, resp_buf.cb_বাফার);
  }
  অন্যথা যদি(mutual_auth)
    Curl_bufref_set(out, "", 0, ফাঁকা);
  অন্যথা
    Curl_bufref_set(out, ফাঁকা, 0, ফাঁকা);

  ফেরত ফলাফল;
}

/*
 * Curl_auth_create_gssapi_security_message()
 *
 * This is used to generate an already encoded GSSAPI (Kerberos V5) security
 * token message ready for sending to the recipient.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * authzid [in]     - The authorization identity if some.
 * chlg    [in]     - The challenge message.
 * krb5    [in/out] - The Kerberos 5 data struct being used and modified.
 * out     [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_gssapi_security_message(কাঠামো Curl_easy *তথ্য,
                                                  ধ্রুবক অক্ষর *authzid,
                                                  ধ্রুবক কাঠামো bufref *chlg,
                                                  কাঠামো kerberos5data *krb5,
                                                  কাঠামো bufref *out)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t offset = 0;
  size_t messagelen = 0;
  size_t appdatalen = 0;
  unsigned অক্ষর *trailer = ফাঁকা;
  unsigned অক্ষর *message = ফাঁকা;
  unsigned অক্ষর *padding = ফাঁকা;
  unsigned অক্ষর *appdata = ফাঁকা;
  Sec_বাফার প্রবেশ_buf[2];
  Sec_বাফার wrap_buf[3];
  Sec_বাফার_Desc প্রবেশ_desc;
  Sec_বাফার_Desc wrap_desc;
  unsigned অক্ষর *indata;
  unsigned long qop = 0;
  unsigned long sec_layer = 0;
  unsigned long max_আকার = 0;
  SecPkgContext_Sizes sizes;
  SECURITY_STATUS status;

  /* Ensure we have a valid challenge message */
  DEBUGASSERT(chlg);
  যদি(!Curl_bufref_len(chlg)) {
    infof(তথ্য, "GSSAPI handshake failure (empty security message)");
    ফেরত CURLE_BAD_CONTENT_ENCODING;
  }

  /* Get our response size information */
  status = Curl_pSecFn->QueryContextAttributes(krb5->context,
                                               SECPKG_ATTR_SIZES, &sizes);

  যদি(status == SEC_E_INSUFFICIENT_MEMORY)
    ফেরত CURLE_OUT_OF_MEMORY;

  যদি(status != SEC_E_OK)
    ফেরত CURLE_AUTH_ERROR;

  /* Setup the "input" security buffer */
  প্রবেশ_desc.ulVersion = SECBUFFER_VERSION;
  প্রবেশ_desc.cBuffers = 2;
  প্রবেশ_desc.pBuffers = প্রবেশ_buf;
  প্রবেশ_buf[0].বাফার_Type = SECBUFFER_STREAM;
  প্রবেশ_buf[0].pv_বাফার = CURL_UNCONST(Curl_bufref_ptr(chlg));
  প্রবেশ_buf[0].cb_বাফার = curlx_uztoul(Curl_bufref_len(chlg));
  প্রবেশ_buf[1].বাফার_Type = SECBUFFER_তথ্য;
  প্রবেশ_buf[1].pv_বাফার = ফাঁকা;
  প্রবেশ_buf[1].cb_বাফার = 0;

  /* Decrypt the inbound challenge and obtain the qop. The encrypted message
     is decrypted in place, overwriting the original contents of its buffer.
     The SECBUFFER_DATA receives a pointer to the message in
     SECBUFFER_STREAM. */
  status = Curl_pSecFn->DecryptMessage(krb5->context, &প্রবেশ_desc, 0, &qop);
  যদি(status != SEC_E_OK) {
    infof(তথ্য, "GSSAPI handshake failure (decryption failed)");
    ফেরত CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  যদি(প্রবেশ_buf[1].cb_বাফার != 4) {
    infof(তথ্য, "GSSAPI handshake failure (invalid security data)");
    ফেরত CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = প্রবেশ_buf[1].pv_বাফার;
  sec_layer = indata[0];
  max_আকার = ((unsigned long)indata[1] << 16) |
             ((unsigned long)indata[2] << 8) | indata[3];

  /* Process the security layer */
  যদি(!(sec_layer & KERB_WRAP_NO_ENCRYPT)) {
    infof(তথ্য, "GSSAPI handshake failure (invalid security layer)");
    ফেরত CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= KERB_WRAP_NO_ENCRYPT;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  যদি(max_আকার > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_আকার = 0;
  }

  /* Allocate the trailer */
  trailer = curlx_malloc(sizes.cbSecurityTrailer);
  যদি(!trailer)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* Allocate our message */
  messagelen = 4;
  যদি(authzid)
    messagelen += strlen(authzid);
  message = curlx_malloc(messagelen);
  যদি(!message) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  /* Populate the message with the security layer and client supported receive
     message size. */
  message[0] = sec_layer & 0xFF;
  message[1] = (max_আকার >> 16) & 0xFF;
  message[2] = (max_আকার >> 8) & 0xFF;
  message[3] = max_আকার & 0xFF;

  /* If given, append the authorization identity. */

  যদি(authzid && *authzid)
    memcpy(message + 4, authzid, messagelen - 4);

  /* Allocate the padding */
  padding = curlx_malloc(sizes.cb_Block_আকার);
  যদি(!padding) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  /* Setup the "authentication data" security buffer */
  wrap_desc.ulVersion    = SECBUFFER_VERSION;
  wrap_desc.cBuffers     = 3;
  wrap_desc.pBuffers     = wrap_buf;
  wrap_buf[0].বাফার_Type = SECBUFFER_TOKEN;
  wrap_buf[0].pv_বাফার   = trailer;
  wrap_buf[0].cb_বাফার   = sizes.cbSecurityTrailer;
  wrap_buf[1].বাফার_Type = SECBUFFER_তথ্য;
  wrap_buf[1].pv_বাফার   = message;
  wrap_buf[1].cb_বাফার   = curlx_uztoul(messagelen);
  wrap_buf[2].বাফার_Type = SECBUFFER_PADDING;
  wrap_buf[2].pv_বাফার   = padding;
  wrap_buf[2].cb_বাফার   = sizes.cb_Block_আকার;

  /* Encrypt the data */
  status = Curl_pSecFn->EncryptMessage(krb5->context, KERB_WRAP_NO_ENCRYPT,
                                       &wrap_desc, 0);
  যদি(status != SEC_E_OK) {
    যদি(status == SEC_E_INSUFFICIENT_MEMORY)
      ফলাফল = CURLE_OUT_OF_MEMORY;
    অন্যথা
      ফলাফল = CURLE_AUTH_ERROR;
    যাও out;
  }

  /* Allocate the encryption (wrap) buffer */
  appdatalen = wrap_buf[0].cb_বাফার + wrap_buf[1].cb_বাফার +
               wrap_buf[2].cb_বাফার;
  appdata = curlx_malloc(appdatalen);
  যদি(!appdata) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  /* Populate the encryption buffer */
  memcpy(appdata, wrap_buf[0].pv_বাফার, wrap_buf[0].cb_বাফার);
  offset += wrap_buf[0].cb_বাফার;
  memcpy(appdata + offset, wrap_buf[1].pv_বাফার, wrap_buf[1].cb_বাফার);
  offset += wrap_buf[1].cb_বাফার;
  memcpy(appdata + offset, wrap_buf[2].pv_বাফার, wrap_buf[2].cb_বাফার);

out:
  /* Free all of our local buffers */
  curlx_free(padding);
  curlx_free(message);
  curlx_free(trailer);

  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Return the response. */
  Curl_bufref_set(out, appdata, appdatalen, curl_free);
  ফেরত CURLE_OK;
}

/*
 * Curl_auth_cleanup_gssapi()
 *
 * This is used to clean up the GSSAPI (Kerberos V5) specific data.
 *
 * Parameters:
 *
 * krb5     [in/out] - The Kerberos 5 data struct being cleaned up.
 *
 */
শূন্য Curl_auth_cleanup_gssapi(কাঠামো kerberos5data *krb5)
{
  /* Free our security context */
  যদি(krb5->context) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);
    curlx_safefree(krb5->context);
  }

  /* Free our credentials handle */
  যদি(krb5->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(krb5->credentials);
    curlx_safefree(krb5->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_identity(krb5->p_identity);
  krb5->p_identity = ফাঁকা;

  /* Free the SPN and output token */
  curlx_safefree(krb5->spn);
  curlx_safefree(krb5->output_token);

  /* Reset any variables */
  krb5->token_max = 0;
}

#endif /* USE_WINDOWS_SSPI && USE_KERBEROS5 */
