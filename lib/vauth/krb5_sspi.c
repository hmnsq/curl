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
    Curl_pSecFn->FreeContextBuffer(SecurityPackage);
  }

  প্রত্যাগম status == SEC_E_OK;
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
CURLcode Curl_auth_create_gssapi_user_message(সংরচনা Curl_easy *data,
                                              সংরচনা Curl_creds *creds,
                                              ধ্রুব বর্ণ *default_service,
                                              ধ্রুব বর্ণ *host,
                                              ধ্রুব bool mutual_auth,
                                              ধ্রুব সংরচনা bufref *chlg,
                                              সংরচনা kerberos5data *krb5,
                                              সংরচনা bufref *out)
{
  CURLcode result = CURLE_OK;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  SecBuffer chlg_buf;
  SecBuffer resp_buf;
  SecBufferDesc chlg_desc;
  SecBufferDesc resp_desc;
  SECURITY_STATUS status;
  unsigned long attrs;
  ধ্রুব বর্ণ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  যদি(!krb5->spn) {
    /* Generate our SPN */
    krb5->spn = Curl_auth_build_spn(service, host, রিক্ত);
    যদি(!krb5->spn)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  যদি(!krb5->output_token) {
    /* Query the security package for Kerberos */
    status = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 &SecurityPackage);
    যদি(status != SEC_E_OK) {
      failf(data, "SSPI: could not get auth info");
      প্রত্যাগম CURLE_AUTH_ERROR;
    }

    krb5->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->FreeContextBuffer(SecurityPackage);

    /* Allocate our response buffer */
    krb5->output_token = curlx_malloc(krb5->token_max);
    যদি(!krb5->output_token)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  যদি(!krb5->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    যদি(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      result = Curl_create_sspi_identity(
        creds->user, creds->passwd, &krb5->identity);
      যদি(result)
        প্রত্যাগম result;

      /* Allow proper cleanup of the identity structure */
      krb5->p_identity = &krb5->identity;
    }
    অন্যথা
      /* Use the current Windows user */
      krb5->p_identity = রিক্ত;

    /* Allocate our credentials handle */
    krb5->credentials = curlx_calloc(1, পরিমাণ(CredHandle));
    যদি(!krb5->credentials)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;

    /* Acquire our credentials handle */
    status = Curl_pSecFn->AcquireCredentialsHandle(রিক্ত,
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 SECPKG_CRED_OUTBOUND, রিক্ত,
                                 krb5->p_identity, রিক্ত, রিক্ত,
                                 krb5->credentials, রিক্ত);
    যদি(status != SEC_E_OK) {
      curlx_safefree(krb5->credentials);
      প্রত্যাগম CURLE_LOGIN_DENIED;
    }

    /* Allocate our new context handle */
    krb5->context = curlx_calloc(1, পরিমাণ(CtxtHandle));
    যদি(!krb5->context)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  যদি(chlg) {
    যদি(!Curl_bufref_len(chlg)) {
      infof(data, "GSSAPI handshake failure (empty challenge message)");
      প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.ulVersion = SECBUFFER_VERSION;
    chlg_desc.cBuffers  = 1;
    chlg_desc.pBuffers  = &chlg_buf;
    chlg_buf.BufferType = SECBUFFER_TOKEN;
    chlg_buf.pvBuffer   = CURL_UNCONST(Curl_bufref_ptr(chlg));
    chlg_buf.cbBuffer   = curlx_uztoul(Curl_bufref_len(chlg));
  }

  /* Setup the response "output" security buffer */
  resp_desc.ulVersion = SECBUFFER_VERSION;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.BufferType = SECBUFFER_TOKEN;
  resp_buf.pvBuffer   = krb5->output_token;
  resp_buf.cbBuffer   = curlx_uztoul(krb5->token_max);

  /* Generate our challenge-response message */
  status = Curl_pSecFn->InitializeSecurityContext(krb5->credentials,
                                               chlg ? krb5->context : রিক্ত,
                                               krb5->spn,
                                               (mutual_auth ?
                                                ISC_REQ_MUTUAL_AUTH : 0),
                                               0, SECURITY_NATIVE_DREP,
                                               chlg ? &chlg_desc : রিক্ত, 0,
                                               &context,
                                               &resp_desc, &attrs, রিক্ত);

  যদি(status == SEC_E_INSUFFICIENT_MEMORY)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  যদি(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED)
    প্রত্যাগম CURLE_AUTH_ERROR;

  যদি(memcmp(&context, krb5->context, পরিমাণ(context))) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);

    memcpy(krb5->context, &context, পরিমাণ(context));
  }

  যদি(resp_buf.cbBuffer) {
    result = Curl_bufref_memdup0(out, resp_buf.pvBuffer, resp_buf.cbBuffer);
  }
  অন্যথা যদি(mutual_auth)
    Curl_bufref_set(out, "", 0, রিক্ত);
  অন্যথা
    Curl_bufref_set(out, রিক্ত, 0, রিক্ত);

  প্রত্যাগম result;
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
CURLcode Curl_auth_create_gssapi_security_message(সংরচনা Curl_easy *data,
                                                  ধ্রুব বর্ণ *authzid,
                                                  ধ্রুব সংরচনা bufref *chlg,
                                                  সংরচনা kerberos5data *krb5,
                                                  সংরচনা bufref *out)
{
  CURLcode result = CURLE_OK;
  size_t offset = 0;
  size_t messagelen = 0;
  size_t appdatalen = 0;
  unsigned বর্ণ *trailer = রিক্ত;
  unsigned বর্ণ *message = রিক্ত;
  unsigned বর্ণ *padding = রিক্ত;
  unsigned বর্ণ *appdata = রিক্ত;
  SecBuffer প্রবেশ_buf[2];
  SecBuffer wrap_buf[3];
  SecBufferDesc প্রবেশ_desc;
  SecBufferDesc wrap_desc;
  unsigned বর্ণ *indata;
  unsigned long qop = 0;
  unsigned long sec_layer = 0;
  unsigned long max_size = 0;
  SecPkgContext_Sizes sizes;
  SECURITY_STATUS status;

  /* Ensure we have a valid challenge message */
  DEBUGASSERT(chlg);
  যদি(!Curl_bufref_len(chlg)) {
    infof(data, "GSSAPI handshake failure (empty security message)");
    প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;
  }

  /* Get our response size information */
  status = Curl_pSecFn->QueryContextAttributes(krb5->context,
                                               SECPKG_ATTR_SIZES, &sizes);

  যদি(status == SEC_E_INSUFFICIENT_MEMORY)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  যদি(status != SEC_E_OK)
    প্রত্যাগম CURLE_AUTH_ERROR;

  /* Setup the "input" security buffer */
  প্রবেশ_desc.ulVersion = SECBUFFER_VERSION;
  প্রবেশ_desc.cBuffers = 2;
  প্রবেশ_desc.pBuffers = প্রবেশ_buf;
  প্রবেশ_buf[0].BufferType = SECBUFFER_STREAM;
  প্রবেশ_buf[0].pvBuffer = CURL_UNCONST(Curl_bufref_ptr(chlg));
  প্রবেশ_buf[0].cbBuffer = curlx_uztoul(Curl_bufref_len(chlg));
  প্রবেশ_buf[1].BufferType = SECBUFFER_DATA;
  প্রবেশ_buf[1].pvBuffer = রিক্ত;
  প্রবেশ_buf[1].cbBuffer = 0;

  /* Decrypt the inbound challenge and obtain the qop. The encrypted message
     is decrypted in place, overwriting the original contents of its buffer.
     The SECBUFFER_DATA receives a pointer to the message in
     SECBUFFER_STREAM. */
  status = Curl_pSecFn->DecryptMessage(krb5->context, &প্রবেশ_desc, 0, &qop);
  যদি(status != SEC_E_OK) {
    infof(data, "GSSAPI handshake failure (decryption failed)");
    প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  যদি(প্রবেশ_buf[1].cbBuffer != 4) {
    infof(data, "GSSAPI handshake failure (invalid security data)");
    প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = প্রবেশ_buf[1].pvBuffer;
  sec_layer = indata[0];
  max_size = ((unsigned long)indata[1] << 16) |
             ((unsigned long)indata[2] << 8) | indata[3];

  /* Process the security layer */
  যদি(!(sec_layer & KERB_WRAP_NO_ENCRYPT)) {
    infof(data, "GSSAPI handshake failure (invalid security layer)");
    প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= KERB_WRAP_NO_ENCRYPT;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  যদি(max_size > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_size = 0;
  }

  /* Allocate the trailer */
  trailer = curlx_malloc(sizes.cbSecurityTrailer);
  যদি(!trailer)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  /* Allocate our message */
  messagelen = 4;
  যদি(authzid)
    messagelen += strlen(authzid);
  message = curlx_malloc(messagelen);
  যদি(!message) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  /* Populate the message with the security layer and client supported receive
     message size. */
  message[0] = sec_layer & 0xFF;
  message[1] = (max_size >> 16) & 0xFF;
  message[2] = (max_size >> 8) & 0xFF;
  message[3] = max_size & 0xFF;

  /* If given, append the authorization identity. */

  যদি(authzid && *authzid)
    memcpy(message + 4, authzid, messagelen - 4);

  /* Allocate the padding */
  padding = curlx_malloc(sizes.cbBlockSize);
  যদি(!padding) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  /* Setup the "authentication data" security buffer */
  wrap_desc.ulVersion    = SECBUFFER_VERSION;
  wrap_desc.cBuffers     = 3;
  wrap_desc.pBuffers     = wrap_buf;
  wrap_buf[0].BufferType = SECBUFFER_TOKEN;
  wrap_buf[0].pvBuffer   = trailer;
  wrap_buf[0].cbBuffer   = sizes.cbSecurityTrailer;
  wrap_buf[1].BufferType = SECBUFFER_DATA;
  wrap_buf[1].pvBuffer   = message;
  wrap_buf[1].cbBuffer   = curlx_uztoul(messagelen);
  wrap_buf[2].BufferType = SECBUFFER_PADDING;
  wrap_buf[2].pvBuffer   = padding;
  wrap_buf[2].cbBuffer   = sizes.cbBlockSize;

  /* Encrypt the data */
  status = Curl_pSecFn->EncryptMessage(krb5->context, KERB_WRAP_NO_ENCRYPT,
                                       &wrap_desc, 0);
  যদি(status != SEC_E_OK) {
    যদি(status == SEC_E_INSUFFICIENT_MEMORY)
      result = CURLE_OUT_OF_MEMORY;
    অন্যথা
      result = CURLE_AUTH_ERROR;
    গচ্ছ out;
  }

  /* Allocate the encryption (wrap) buffer */
  appdatalen = wrap_buf[0].cbBuffer + wrap_buf[1].cbBuffer +
               wrap_buf[2].cbBuffer;
  appdata = curlx_malloc(appdatalen);
  যদি(!appdata) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  /* Populate the encryption buffer */
  memcpy(appdata, wrap_buf[0].pvBuffer, wrap_buf[0].cbBuffer);
  offset += wrap_buf[0].cbBuffer;
  memcpy(appdata + offset, wrap_buf[1].pvBuffer, wrap_buf[1].cbBuffer);
  offset += wrap_buf[1].cbBuffer;
  memcpy(appdata + offset, wrap_buf[2].pvBuffer, wrap_buf[2].cbBuffer);

out:
  /* Free all of our local buffers */
  curlx_free(padding);
  curlx_free(message);
  curlx_free(trailer);

  যদি(result)
    প্রত্যাগম result;

  /* Return the response. */
  Curl_bufref_set(out, appdata, appdatalen, curl_free);
  প্রত্যাগম CURLE_OK;
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
শূন্য Curl_auth_cleanup_gssapi(সংরচনা kerberos5data *krb5)
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
  krb5->p_identity = রিক্ত;

  /* Free the SPN and output token */
  curlx_safefree(krb5->spn);
  curlx_safefree(krb5->output_token);

  /* Reset any variables */
  krb5->token_max = 0;
}

#endif /* USE_WINDOWS_SSPI && USE_KERBEROS5 */
