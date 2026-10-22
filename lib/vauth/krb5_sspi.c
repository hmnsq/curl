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
#समावेश "curl_setup.h"

#अगर defined(USE_WINDOWS_SSPI) && defined(USE_KERBEROS5)

#समावेश "vauth/vauth.h"
#समावेश "curl_trc.h"

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by Windows SSPI.
 */
bool Curl_auth_is_gssapi_supported(व्योम)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_STATUS status;

  /* Query the security package for Kerberos */
  status = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  अगर(status == SEC_E_OK) {
    Curl_pSecFn->Free_Context_बफर(SecurityPackage);
  }

  वापस status == SEC_E_OK;
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
CURLcode Curl_auth_create_gssapi_user_message(काष्ठा Curl_easy *तथ्य,
                                              काष्ठा Curl_creds *creds,
                                              स्थिर अक्षर *default_service,
                                              स्थिर अक्षर *host,
                                              स्थिर bool mutual_auth,
                                              स्थिर काष्ठा bufref *chlg,
                                              काष्ठा kerberos5data *krb5,
                                              काष्ठा bufref *out)
{
  CURLcode परिणाम = CURLE_OK;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  Sec_बफर chlg_buf;
  Sec_बफर resp_buf;
  Sec_बफर_Desc chlg_desc;
  Sec_बफर_Desc resp_desc;
  SECURITY_STATUS status;
  अचिन्हित दीर्घ attrs;
  स्थिर अक्षर *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  अगर(!krb5->spn) {
    /* Generate our SPN */
    krb5->spn = Curl_auth_build_spn(service, host, शून्य);
    अगर(!krb5->spn)
      वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(!krb5->output_token) {
    /* Query the security package for Kerberos */
    status = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 &SecurityPackage);
    अगर(status != SEC_E_OK) {
      failf(तथ्य, "SSPI: could not get auth info");
      वापस CURLE_AUTH_ERROR;
    }

    krb5->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_बफर(SecurityPackage);

    /* Allocate our response buffer */
    krb5->output_token = curlx_malloc(krb5->token_max);
    अगर(!krb5->output_token)
      वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(!krb5->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    अगर(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      परिणाम = Curl_create_sspi_identity(
        creds->user, creds->passwd, &krb5->identity);
      अगर(परिणाम)
        वापस परिणाम;

      /* Allow proper cleanup of the identity structure */
      krb5->p_identity = &krb5->identity;
    }
    अन्यथा
      /* Use the current Windows user */
      krb5->p_identity = शून्य;

    /* Allocate our credentials handle */
    krb5->credentials = curlx_calloc(1, माप(CredHandle));
    अगर(!krb5->credentials)
      वापस CURLE_OUT_OF_MEMORY;

    /* Acquire our credentials handle */
    status = Curl_pSecFn->AcquireCredentialsHandle(शून्य,
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 SECPKG_CRED_OUTBOUND, शून्य,
                                 krb5->p_identity, शून्य, शून्य,
                                 krb5->credentials, शून्य);
    अगर(status != SEC_E_OK) {
      curlx_safefree(krb5->credentials);
      वापस CURLE_LOGIN_DENIED;
    }

    /* Allocate our new context handle */
    krb5->context = curlx_calloc(1, माप(CtxtHandle));
    अगर(!krb5->context)
      वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(chlg) {
    अगर(!Curl_bufref_len(chlg)) {
      infof(तथ्य, "GSSAPI handshake failure (empty challenge message)");
      वापस CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.ulVersion = SECBUFFER_VERSION;
    chlg_desc.cBuffers  = 1;
    chlg_desc.pBuffers  = &chlg_buf;
    chlg_buf.बफर_Type = SECBUFFER_TOKEN;
    chlg_buf.pv_बफर   = CURL_UNCONST(Curl_bufref_ptr(chlg));
    chlg_buf.cb_बफर   = curlx_uztoul(Curl_bufref_len(chlg));
  }

  /* Setup the response "output" security buffer */
  resp_desc.ulVersion = SECBUFFER_VERSION;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.बफर_Type = SECBUFFER_TOKEN;
  resp_buf.pv_बफर   = krb5->output_token;
  resp_buf.cb_बफर   = curlx_uztoul(krb5->token_max);

  /* Generate our challenge-response message */
  status = Curl_pSecFn->InitializeSecurityContext(krb5->credentials,
                                               chlg ? krb5->context : शून्य,
                                               krb5->spn,
                                               (mutual_auth ?
                                                ISC_REQ_MUTUAL_AUTH : 0),
                                               0, SECURITY_NATIVE_DREP,
                                               chlg ? &chlg_desc : शून्य, 0,
                                               &context,
                                               &resp_desc, &attrs, शून्य);

  अगर(status == SEC_E_INSUFFICIENT_MEMORY)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED)
    वापस CURLE_AUTH_ERROR;

  अगर(स_भेद(&context, krb5->context, माप(context))) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);

    स_नकल(krb5->context, &context, माप(context));
  }

  अगर(resp_buf.cb_बफर) {
    परिणाम = Curl_bufref_memdup0(out, resp_buf.pv_बफर, resp_buf.cb_बफर);
  }
  अन्यथा अगर(mutual_auth)
    Curl_bufref_set(out, "", 0, शून्य);
  अन्यथा
    Curl_bufref_set(out, शून्य, 0, शून्य);

  वापस परिणाम;
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
CURLcode Curl_auth_create_gssapi_security_message(काष्ठा Curl_easy *तथ्य,
                                                  स्थिर अक्षर *authzid,
                                                  स्थिर काष्ठा bufref *chlg,
                                                  काष्ठा kerberos5data *krb5,
                                                  काष्ठा bufref *out)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार offset = 0;
  माप_प्रकार messagelen = 0;
  माप_प्रकार appdatalen = 0;
  अचिन्हित अक्षर *trailer = शून्य;
  अचिन्हित अक्षर *message = शून्य;
  अचिन्हित अक्षर *padding = शून्य;
  अचिन्हित अक्षर *appdata = शून्य;
  Sec_बफर निवेश_buf[2];
  Sec_बफर wrap_buf[3];
  Sec_बफर_Desc निवेश_desc;
  Sec_बफर_Desc wrap_desc;
  अचिन्हित अक्षर *indata;
  अचिन्हित दीर्घ qop = 0;
  अचिन्हित दीर्घ sec_layer = 0;
  अचिन्हित दीर्घ max_आकार = 0;
  SecPkgContext_Sizes sizes;
  SECURITY_STATUS status;

  /* Ensure we have a valid challenge message */
  DEBUGASSERT(chlg);
  अगर(!Curl_bufref_len(chlg)) {
    infof(तथ्य, "GSSAPI handshake failure (empty security message)");
    वापस CURLE_BAD_CONTENT_ENCODING;
  }

  /* Get our response size information */
  status = Curl_pSecFn->QueryContextAttributes(krb5->context,
                                               SECPKG_ATTR_SIZES, &sizes);

  अगर(status == SEC_E_INSUFFICIENT_MEMORY)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(status != SEC_E_OK)
    वापस CURLE_AUTH_ERROR;

  /* Setup the "input" security buffer */
  निवेश_desc.ulVersion = SECBUFFER_VERSION;
  निवेश_desc.cBuffers = 2;
  निवेश_desc.pBuffers = निवेश_buf;
  निवेश_buf[0].बफर_Type = SECBUFFER_STREAM;
  निवेश_buf[0].pv_बफर = CURL_UNCONST(Curl_bufref_ptr(chlg));
  निवेश_buf[0].cb_बफर = curlx_uztoul(Curl_bufref_len(chlg));
  निवेश_buf[1].बफर_Type = SECBUFFER_तथ्य;
  निवेश_buf[1].pv_बफर = शून्य;
  निवेश_buf[1].cb_बफर = 0;

  /* Decrypt the inbound challenge and obtain the qop. The encrypted message
     is decrypted in place, overwriting the original contents of its buffer.
     The SECBUFFER_DATA receives a pointer to the message in
     SECBUFFER_STREAM. */
  status = Curl_pSecFn->DecryptMessage(krb5->context, &निवेश_desc, 0, &qop);
  अगर(status != SEC_E_OK) {
    infof(तथ्य, "GSSAPI handshake failure (decryption failed)");
    वापस CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  अगर(निवेश_buf[1].cb_बफर != 4) {
    infof(तथ्य, "GSSAPI handshake failure (invalid security data)");
    वापस CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = निवेश_buf[1].pv_बफर;
  sec_layer = indata[0];
  max_आकार = ((अचिन्हित दीर्घ)indata[1] << 16) |
             ((अचिन्हित दीर्घ)indata[2] << 8) | indata[3];

  /* Process the security layer */
  अगर(!(sec_layer & KERB_WRAP_NO_ENCRYPT)) {
    infof(तथ्य, "GSSAPI handshake failure (invalid security layer)");
    वापस CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= KERB_WRAP_NO_ENCRYPT;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  अगर(max_आकार > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_आकार = 0;
  }

  /* Allocate the trailer */
  trailer = curlx_malloc(sizes.cbSecurityTrailer);
  अगर(!trailer)
    वापस CURLE_OUT_OF_MEMORY;

  /* Allocate our message */
  messagelen = 4;
  अगर(authzid)
    messagelen += म_माप(authzid);
  message = curlx_malloc(messagelen);
  अगर(!message) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  /* Populate the message with the security layer and client supported receive
     message size. */
  message[0] = sec_layer & 0xFF;
  message[1] = (max_आकार >> 16) & 0xFF;
  message[2] = (max_आकार >> 8) & 0xFF;
  message[3] = max_आकार & 0xFF;

  /* If given, append the authorization identity. */

  अगर(authzid && *authzid)
    स_नकल(message + 4, authzid, messagelen - 4);

  /* Allocate the padding */
  padding = curlx_malloc(sizes.cb_Block_आकार);
  अगर(!padding) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  /* Setup the "authentication data" security buffer */
  wrap_desc.ulVersion    = SECBUFFER_VERSION;
  wrap_desc.cBuffers     = 3;
  wrap_desc.pBuffers     = wrap_buf;
  wrap_buf[0].बफर_Type = SECBUFFER_TOKEN;
  wrap_buf[0].pv_बफर   = trailer;
  wrap_buf[0].cb_बफर   = sizes.cbSecurityTrailer;
  wrap_buf[1].बफर_Type = SECBUFFER_तथ्य;
  wrap_buf[1].pv_बफर   = message;
  wrap_buf[1].cb_बफर   = curlx_uztoul(messagelen);
  wrap_buf[2].बफर_Type = SECBUFFER_PADDING;
  wrap_buf[2].pv_बफर   = padding;
  wrap_buf[2].cb_बफर   = sizes.cb_Block_आकार;

  /* Encrypt the data */
  status = Curl_pSecFn->EncryptMessage(krb5->context, KERB_WRAP_NO_ENCRYPT,
                                       &wrap_desc, 0);
  अगर(status != SEC_E_OK) {
    अगर(status == SEC_E_INSUFFICIENT_MEMORY)
      परिणाम = CURLE_OUT_OF_MEMORY;
    अन्यथा
      परिणाम = CURLE_AUTH_ERROR;
    जाओ out;
  }

  /* Allocate the encryption (wrap) buffer */
  appdatalen = wrap_buf[0].cb_बफर + wrap_buf[1].cb_बफर +
               wrap_buf[2].cb_बफर;
  appdata = curlx_malloc(appdatalen);
  अगर(!appdata) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  /* Populate the encryption buffer */
  स_नकल(appdata, wrap_buf[0].pv_बफर, wrap_buf[0].cb_बफर);
  offset += wrap_buf[0].cb_बफर;
  स_नकल(appdata + offset, wrap_buf[1].pv_बफर, wrap_buf[1].cb_बफर);
  offset += wrap_buf[1].cb_बफर;
  स_नकल(appdata + offset, wrap_buf[2].pv_बफर, wrap_buf[2].cb_बफर);

out:
  /* Free all of our local buffers */
  curlx_free(padding);
  curlx_free(message);
  curlx_free(trailer);

  अगर(परिणाम)
    वापस परिणाम;

  /* Return the response. */
  Curl_bufref_set(out, appdata, appdatalen, curl_free);
  वापस CURLE_OK;
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
व्योम Curl_auth_cleanup_gssapi(काष्ठा kerberos5data *krb5)
{
  /* Free our security context */
  अगर(krb5->context) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);
    curlx_safefree(krb5->context);
  }

  /* Free our credentials handle */
  अगर(krb5->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(krb5->credentials);
    curlx_safefree(krb5->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_identity(krb5->p_identity);
  krb5->p_identity = शून्य;

  /* Free the SPN and output token */
  curlx_safefree(krb5->spn);
  curlx_safefree(krb5->output_token);

  /* Reset any variables */
  krb5->token_max = 0;
}

#पूर्ण_अगर /* USE_WINDOWS_SSPI && USE_KERBEROS5 */
