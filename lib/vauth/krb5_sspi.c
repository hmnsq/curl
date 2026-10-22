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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ defined(USE_WINDOWS_SSPI) && defined(USE_KERBEROS5)

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by Windows SSPI.
 */
bool Curl_auth_is_gssapi_supported(ಶೂನ್ಯ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for Kerberos */
  status = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ಯದಿ(status == SEC_E_OK) {
    Curl_pSecFn->FreeContextBuffer(SecurityPackage);
  }

  ಪ್ರತ್ಯಾಗಮ status == SEC_E_OK;
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
CURLcode Curl_auth_create_gssapi_user_message(ಸಂರಚನಾ Curl_easy *data,
                                              ಸಂರಚನಾ Curl_creds *creds,
                                              ಧ್ರುವ ವರ್ಣ *default_service,
                                              ಧ್ರುವ ವರ್ಣ *host,
                                              ಧ್ರುವ bool mutual_auth,
                                              ಧ್ರುವ ಸಂರಚನಾ bufref *chlg,
                                              ಸಂರಚನಾ kerberos5data *krb5,
                                              ಸಂರಚನಾ bufref *out)
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
  ಧ್ರುವ ವರ್ಣ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  ಯದಿ(!krb5->spn) {
    /* Generate our SPN */
    krb5->spn = Curl_auth_build_spn(service, host, ರಿಕ್ತ);
    ಯದಿ(!krb5->spn)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಯದಿ(!krb5->output_token) {
    /* Query the security package for Kerberos */
    status = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 &SecurityPackage);
    ಯದಿ(status != SEC_E_OK) {
      failf(data, "SSPI: could not get auth info");
      ಪ್ರತ್ಯಾಗಮ CURLE_AUTH_ERROR;
    }

    krb5->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->FreeContextBuffer(SecurityPackage);

    /* Allocate our response buffer */
    krb5->output_token = curlx_malloc(krb5->token_max);
    ಯದಿ(!krb5->output_token)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಯದಿ(!krb5->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    ಯದಿ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      result = Curl_create_sspi_identity(
        creds->user, creds->passwd, &krb5->identity);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      /* Allow proper cleanup of the identity structure */
      krb5->p_identity = &krb5->identity;
    }
    ಅನ್ಯಥಾ
      /* Use the current Windows user */
      krb5->p_identity = ರಿಕ್ತ;

    /* Allocate our credentials handle */
    krb5->credentials = curlx_calloc(1, ಪರಿಮಾಣ(CredHandle));
    ಯದಿ(!krb5->credentials)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

    /* Acquire our credentials handle */
    status = Curl_pSecFn->AcquireCredentialsHandle(ರಿಕ್ತ,
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 SECPKG_CRED_OUTBOUND, ರಿಕ್ತ,
                                 krb5->p_identity, ರಿಕ್ತ, ರಿಕ್ತ,
                                 krb5->credentials, ರಿಕ್ತ);
    ಯದಿ(status != SEC_E_OK) {
      curlx_safefree(krb5->credentials);
      ಪ್ರತ್ಯಾಗಮ CURLE_LOGIN_DENIED;
    }

    /* Allocate our new context handle */
    krb5->context = curlx_calloc(1, ಪರಿಮಾಣ(CtxtHandle));
    ಯದಿ(!krb5->context)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಯದಿ(chlg) {
    ಯದಿ(!Curl_bufref_len(chlg)) {
      infof(data, "GSSAPI handshake failure (empty challenge message)");
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
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
                                               chlg ? krb5->context : ರಿಕ್ತ,
                                               krb5->spn,
                                               (mutual_auth ?
                                                ISC_REQ_MUTUAL_AUTH : 0),
                                               0, SECURITY_NATIVE_DREP,
                                               chlg ? &chlg_desc : ರಿಕ್ತ, 0,
                                               &context,
                                               &resp_desc, &attrs, ರಿಕ್ತ);

  ಯದಿ(status == SEC_E_INSUFFICIENT_MEMORY)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಯದಿ(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED)
    ಪ್ರತ್ಯಾಗಮ CURLE_AUTH_ERROR;

  ಯದಿ(memcmp(&context, krb5->context, ಪರಿಮಾಣ(context))) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);

    memcpy(krb5->context, &context, ಪರಿಮಾಣ(context));
  }

  ಯದಿ(resp_buf.cbBuffer) {
    result = Curl_bufref_memdup0(out, resp_buf.pvBuffer, resp_buf.cbBuffer);
  }
  ಅನ್ಯಥಾ ಯದಿ(mutual_auth)
    Curl_bufref_set(out, "", 0, ರಿಕ್ತ);
  ಅನ್ಯಥಾ
    Curl_bufref_set(out, ರಿಕ್ತ, 0, ರಿಕ್ತ);

  ಪ್ರತ್ಯಾಗಮ result;
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
CURLcode Curl_auth_create_gssapi_security_message(ಸಂರಚನಾ Curl_easy *data,
                                                  ಧ್ರುವ ವರ್ಣ *authzid,
                                                  ಧ್ರುವ ಸಂರಚನಾ bufref *chlg,
                                                  ಸಂರಚನಾ kerberos5data *krb5,
                                                  ಸಂರಚನಾ bufref *out)
{
  CURLcode result = CURLE_OK;
  size_t offset = 0;
  size_t messagelen = 0;
  size_t appdatalen = 0;
  unsigned ವರ್ಣ *trailer = ರಿಕ್ತ;
  unsigned ವರ್ಣ *message = ರಿಕ್ತ;
  unsigned ವರ್ಣ *padding = ರಿಕ್ತ;
  unsigned ವರ್ಣ *appdata = ರಿಕ್ತ;
  SecBuffer ಒಳಹರಿವು_buf[2];
  SecBuffer wrap_buf[3];
  SecBufferDesc ಒಳಹರಿವು_desc;
  SecBufferDesc wrap_desc;
  unsigned ವರ್ಣ *indata;
  unsigned long qop = 0;
  unsigned long sec_layer = 0;
  unsigned long max_size = 0;
  SecPkgContext_Sizes sizes;
  SECURITY_STATUS status;

  /* Ensure we have a valid challenge message */
  DEBUGASSERT(chlg);
  ಯದಿ(!Curl_bufref_len(chlg)) {
    infof(data, "GSSAPI handshake failure (empty security message)");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Get our response size information */
  status = Curl_pSecFn->QueryContextAttributes(krb5->context,
                                               SECPKG_ATTR_SIZES, &sizes);

  ಯದಿ(status == SEC_E_INSUFFICIENT_MEMORY)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಯದಿ(status != SEC_E_OK)
    ಪ್ರತ್ಯಾಗಮ CURLE_AUTH_ERROR;

  /* Setup the "input" security buffer */
  ಒಳಹರಿವು_desc.ulVersion = SECBUFFER_VERSION;
  ಒಳಹರಿವು_desc.cBuffers = 2;
  ಒಳಹರಿವು_desc.pBuffers = ಒಳಹರಿವು_buf;
  ಒಳಹರಿವು_buf[0].BufferType = SECBUFFER_STREAM;
  ಒಳಹರಿವು_buf[0].pvBuffer = CURL_UNCONST(Curl_bufref_ptr(chlg));
  ಒಳಹರಿವು_buf[0].cbBuffer = curlx_uztoul(Curl_bufref_len(chlg));
  ಒಳಹರಿವು_buf[1].BufferType = SECBUFFER_DATA;
  ಒಳಹರಿವು_buf[1].pvBuffer = ರಿಕ್ತ;
  ಒಳಹರಿವು_buf[1].cbBuffer = 0;

  /* Decrypt the inbound challenge and obtain the qop. The encrypted message
     is decrypted in place, overwriting the original contents of its buffer.
     The SECBUFFER_DATA receives a pointer to the message in
     SECBUFFER_STREAM. */
  status = Curl_pSecFn->DecryptMessage(krb5->context, &ಒಳಹರಿವು_desc, 0, &qop);
  ಯದಿ(status != SEC_E_OK) {
    infof(data, "GSSAPI handshake failure (decryption failed)");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  ಯದಿ(ಒಳಹರಿವು_buf[1].cbBuffer != 4) {
    infof(data, "GSSAPI handshake failure (invalid security data)");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = ಒಳಹರಿವು_buf[1].pvBuffer;
  sec_layer = indata[0];
  max_size = ((unsigned long)indata[1] << 16) |
             ((unsigned long)indata[2] << 8) | indata[3];

  /* Process the security layer */
  ಯದಿ(!(sec_layer & KERB_WRAP_NO_ENCRYPT)) {
    infof(data, "GSSAPI handshake failure (invalid security layer)");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= KERB_WRAP_NO_ENCRYPT;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  ಯದಿ(max_size > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_size = 0;
  }

  /* Allocate the trailer */
  trailer = curlx_malloc(sizes.cbSecurityTrailer);
  ಯದಿ(!trailer)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* Allocate our message */
  messagelen = 4;
  ಯದಿ(authzid)
    messagelen += strlen(authzid);
  message = curlx_malloc(messagelen);
  ಯದಿ(!message) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  /* Populate the message with the security layer and client supported receive
     message size. */
  message[0] = sec_layer & 0xFF;
  message[1] = (max_size >> 16) & 0xFF;
  message[2] = (max_size >> 8) & 0xFF;
  message[3] = max_size & 0xFF;

  /* If given, append the authorization identity. */

  ಯದಿ(authzid && *authzid)
    memcpy(message + 4, authzid, messagelen - 4);

  /* Allocate the padding */
  padding = curlx_malloc(sizes.cbBlockSize);
  ಯದಿ(!padding) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
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
  ಯದಿ(status != SEC_E_OK) {
    ಯದಿ(status == SEC_E_INSUFFICIENT_MEMORY)
      result = CURLE_OUT_OF_MEMORY;
    ಅನ್ಯಥಾ
      result = CURLE_AUTH_ERROR;
    ಗಚ್ಛ out;
  }

  /* Allocate the encryption (wrap) buffer */
  appdatalen = wrap_buf[0].cbBuffer + wrap_buf[1].cbBuffer +
               wrap_buf[2].cbBuffer;
  appdata = curlx_malloc(appdatalen);
  ಯದಿ(!appdata) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
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

  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Return the response. */
  Curl_bufref_set(out, appdata, appdatalen, curl_free);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
ಶೂನ್ಯ Curl_auth_cleanup_gssapi(ಸಂರಚನಾ kerberos5data *krb5)
{
  /* Free our security context */
  ಯದಿ(krb5->context) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);
    curlx_safefree(krb5->context);
  }

  /* Free our credentials handle */
  ಯದಿ(krb5->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(krb5->credentials);
    curlx_safefree(krb5->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_identity(krb5->p_identity);
  krb5->p_identity = ರಿಕ್ತ;

  /* Free the SPN and output token */
  curlx_safefree(krb5->spn);
  curlx_safefree(krb5->output_token);

  /* Reset any variables */
  krb5->token_max = 0;
}

#endif /* USE_WINDOWS_SSPI && USE_KERBEROS5 */
