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
#شامل "curl_setup.h"

#اگر defined(USE_WINDOWS_SSPI) && defined(USE_KERBEROS5)

#شامل "vauth/vauth.h"
#شامل "curl_trc.h"

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by Windows SSPI.
 */
bool Curl_auth_is_gssapi_supported(خالی)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_حالت حالت_2;

  /* Query the security package for Kerberos */
  حالت_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(متن_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  اگر(حالت_2 == SEC_E_OK) {
    Curl_pSecFn->Free_Context_بفر(SecurityPackage);
  }

  واپس حالت_2 == SEC_E_OK;
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
CURLcode Curl_auth_create_gssapi_user_پیغام(ساخت Curl_easy *ڈیٹا,
                                              ساخت Curl_creds *creds,
                                              ثابت حرف *default_service,
                                              ثابت حرف *host,
                                              ثابت bool mutual_auth,
                                              ثابت ساخت bufref *chlg,
                                              ساخت kerberos5data *krb5,
                                              ساخت bufref *برآمد)
{
  CURLcode نتیجہ = CURLE_OK;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  Sec_بفر chlg_buf;
  Sec_بفر resp_buf;
  Sec_بفر_Desc chlg_desc;
  Sec_بفر_Desc resp_desc;
  SECURITY_حالت حالت_2;
  غیرعلامتی طویل attrs;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  اگر(!krb5->spn) {
    /* Generate our SPN */
    krb5->spn = Curl_auth_build_spn(service, host, تہی);
    اگر(!krb5->spn)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  اگر(!krb5->output_token) {
    /* Query the security package for Kerberos */
    حالت_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(متن_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);
    اگر(حالت_2 != SEC_E_OK) {
      failf(ڈیٹا, "SSPI: could not get auth info");
      واپس CURLE_AUTH_ERROR;
    }

    krb5->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_بفر(SecurityPackage);

    /* Allocate our response buffer */
    krb5->output_token = curlx_malloc(krb5->token_max);
    اگر(!krb5->output_token)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  اگر(!krb5->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    اگر(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      نتیجہ = Curl_create_sspi_شناختی_عنصر(
        creds->user, creds->passwd, &krb5->شناختی_عنصر);
      اگر(نتیجہ)
        واپس نتیجہ;

      /* Allow proper cleanup of the identity structure */
      krb5->p_شناختی_عنصر = &krb5->شناختی_عنصر;
    }
    ورنہ
      /* Use the current Windows user */
      krb5->p_شناختی_عنصر = تہی;

    /* Allocate our credentials handle */
    krb5->credentials = curlx_calloc(1, حجم(CredHandle));
    اگر(!krb5->credentials)
      واپس CURLE_برآمد_کا_حافظہ;

    /* Acquire our credentials handle */
    حالت_2 = Curl_pSecFn->AcquireCredentialsHandle(تہی,
                                 CURL_UNCONST(متن_2(SP_NAME_KERBEROS)),
                                 SECPKG_CRED_OUTBOUND, تہی,
                                 krb5->p_شناختی_عنصر, تہی, تہی,
                                 krb5->credentials, تہی);
    اگر(حالت_2 != SEC_E_OK) {
      curlx_safefree(krb5->credentials);
      واپس CURLE_LOGIN_DENIED;
    }

    /* Allocate our new context handle */
    krb5->context = curlx_calloc(1, حجم(CtxtHandle));
    اگر(!krb5->context)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  اگر(chlg) {
    اگر(!Curl_bufref_len(chlg)) {
      infof(ڈیٹا, "GSSAPI handshake failure (empty challenge message)");
      واپس CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.ul_نسخہ = SECBUFFER_نسخہ;
    chlg_desc.cBuffers  = 1;
    chlg_desc.pBuffers  = &chlg_buf;
    chlg_buf.بفر_Type = SECBUFFER_TOKEN;
    chlg_buf.pv_بفر   = CURL_UNCONST(Curl_bufref_ptr(chlg));
    chlg_buf.cb_بفر   = curlx_uztoul(Curl_bufref_len(chlg));
  }

  /* Setup the response "output" security buffer */
  resp_desc.ul_نسخہ = SECBUFFER_نسخہ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.بفر_Type = SECBUFFER_TOKEN;
  resp_buf.pv_بفر   = krb5->output_token;
  resp_buf.cb_بفر   = curlx_uztoul(krb5->token_max);

  /* Generate our challenge-response message */
  حالت_2 = Curl_pSecFn->InitializeSecurityContext(krb5->credentials,
                                               chlg ? krb5->context : تہی,
                                               krb5->spn,
                                               (mutual_auth ?
                                                ISC_REQ_MUTUAL_AUTH : 0),
                                               0, SECURITY_NATIVE_DREP,
                                               chlg ? &chlg_desc : تہی, 0,
                                               &context,
                                               &resp_desc, &attrs, تہی);

  اگر(حالت_2 == SEC_E_INSUFFICIENT_حافظہ)
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(حالت_2 != SEC_E_OK && حالت_2 != SEC_I_CONTINUE_NEEDED)
    واپس CURLE_AUTH_ERROR;

  اگر(memcmp(&context, krb5->context, حجم(context))) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);

    حافظہ_نقل(krb5->context, &context, حجم(context));
  }

  اگر(resp_buf.cb_بفر) {
    نتیجہ = Curl_bufref_memdup0(برآمد, resp_buf.pv_بفر, resp_buf.cb_بفر);
  }
  ورنہ اگر(mutual_auth)
    Curl_bufref_سیٹ(برآمد, "", 0, تہی);
  ورنہ
    Curl_bufref_سیٹ(برآمد, تہی, 0, تہی);

  واپس نتیجہ;
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
CURLcode Curl_auth_create_gssapi_security_پیغام(ساخت Curl_easy *ڈیٹا,
                                                  ثابت حرف *authzid,
                                                  ثابت ساخت bufref *chlg,
                                                  ساخت kerberos5data *krb5,
                                                  ساخت bufref *برآمد)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t انحراف = 0;
  size_t messagelen = 0;
  size_t appdatalen = 0;
  غیرعلامتی حرف *trailer = تہی;
  غیرعلامتی حرف *پیغام_2 = تہی;
  غیرعلامتی حرف *padding = تہی;
  غیرعلامتی حرف *appdata = تہی;
  Sec_بفر داخلہ_buf[2];
  Sec_بفر wrap_buf[3];
  Sec_بفر_Desc داخلہ_desc;
  Sec_بفر_Desc wrap_desc;
  غیرعلامتی حرف *indata;
  غیرعلامتی طویل qop = 0;
  غیرعلامتی طویل sec_layer = 0;
  غیرعلامتی طویل max_سائز = 0;
  SecPkgContext_Sizes sizes;
  SECURITY_حالت حالت_2;

  /* Ensure we have a valid challenge message */
  DEBUGASSERT(chlg);
  اگر(!Curl_bufref_len(chlg)) {
    infof(ڈیٹا, "GSSAPI handshake failure (empty security message)");
    واپس CURLE_BAD_CONTENT_ENCODING;
  }

  /* Get our response size information */
  حالت_2 = Curl_pSecFn->Query_Context_اوصاف(krb5->context,
                                               SECPKG_ATTR_SIZES, &sizes);

  اگر(حالت_2 == SEC_E_INSUFFICIENT_حافظہ)
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(حالت_2 != SEC_E_OK)
    واپس CURLE_AUTH_ERROR;

  /* Setup the "input" security buffer */
  داخلہ_desc.ul_نسخہ = SECBUFFER_نسخہ;
  داخلہ_desc.cBuffers = 2;
  داخلہ_desc.pBuffers = داخلہ_buf;
  داخلہ_buf[0].بفر_Type = SECBUFFER_دھارا;
  داخلہ_buf[0].pv_بفر = CURL_UNCONST(Curl_bufref_ptr(chlg));
  داخلہ_buf[0].cb_بفر = curlx_uztoul(Curl_bufref_len(chlg));
  داخلہ_buf[1].بفر_Type = SECBUFFER_ڈیٹا;
  داخلہ_buf[1].pv_بفر = تہی;
  داخلہ_buf[1].cb_بفر = 0;

  /* Decrypt the inbound challenge and obtain the qop. The encrypted message
     is decrypted in place, overwriting the original contents of its buffer.
     The SECBUFFER_DATA receives a pointer to the message in
     SECBUFFER_STREAM. */
  حالت_2 = Curl_pSecFn->Decrypt_پیغام(krb5->context, &داخلہ_desc, 0, &qop);
  اگر(حالت_2 != SEC_E_OK) {
    infof(ڈیٹا, "GSSAPI handshake failure (decryption failed)");
    واپس CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  اگر(داخلہ_buf[1].cb_بفر != 4) {
    infof(ڈیٹا, "GSSAPI handshake failure (invalid security data)");
    واپس CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = داخلہ_buf[1].pv_بفر;
  sec_layer = indata[0];
  max_سائز = ((غیرعلامتی طویل)indata[1] << 16) |
             ((غیرعلامتی طویل)indata[2] << 8) | indata[3];

  /* Process the security layer */
  اگر(!(sec_layer & KERB_WRAP_NO_ENCRYPT)) {
    infof(ڈیٹا, "GSSAPI handshake failure (invalid security layer)");
    واپس CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= KERB_WRAP_NO_ENCRYPT;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  اگر(max_سائز > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_سائز = 0;
  }

  /* Allocate the trailer */
  trailer = curlx_malloc(sizes.cbSecurityTrailer);
  اگر(!trailer)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Allocate our message */
  messagelen = 4;
  اگر(authzid)
    messagelen += لڑی_لمبائی(authzid);
  پیغام_2 = curlx_malloc(messagelen);
  اگر(!پیغام_2) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  /* Populate the message with the security layer and client supported receive
     message size. */
  پیغام_2[0] = sec_layer & 0xFF;
  پیغام_2[1] = (max_سائز >> 16) & 0xFF;
  پیغام_2[2] = (max_سائز >> 8) & 0xFF;
  پیغام_2[3] = max_سائز & 0xFF;

  /* If given, append the authorization identity. */

  اگر(authzid && *authzid)
    حافظہ_نقل(پیغام_2 + 4, authzid, messagelen - 4);

  /* Allocate the padding */
  padding = curlx_malloc(sizes.cb_بلاک_سائز);
  اگر(!padding) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  /* Setup the "authentication data" security buffer */
  wrap_desc.ul_نسخہ    = SECBUFFER_نسخہ;
  wrap_desc.cBuffers     = 3;
  wrap_desc.pBuffers     = wrap_buf;
  wrap_buf[0].بفر_Type = SECBUFFER_TOKEN;
  wrap_buf[0].pv_بفر   = trailer;
  wrap_buf[0].cb_بفر   = sizes.cbSecurityTrailer;
  wrap_buf[1].بفر_Type = SECBUFFER_ڈیٹا;
  wrap_buf[1].pv_بفر   = پیغام_2;
  wrap_buf[1].cb_بفر   = curlx_uztoul(messagelen);
  wrap_buf[2].بفر_Type = SECBUFFER_PADDING;
  wrap_buf[2].pv_بفر   = padding;
  wrap_buf[2].cb_بفر   = sizes.cb_بلاک_سائز;

  /* Encrypt the data */
  حالت_2 = Curl_pSecFn->Encrypt_پیغام(krb5->context, KERB_WRAP_NO_ENCRYPT,
                                       &wrap_desc, 0);
  اگر(حالت_2 != SEC_E_OK) {
    اگر(حالت_2 == SEC_E_INSUFFICIENT_حافظہ)
      نتیجہ = CURLE_برآمد_کا_حافظہ;
    ورنہ
      نتیجہ = CURLE_AUTH_ERROR;
    جاؤ برآمد;
  }

  /* Allocate the encryption (wrap) buffer */
  appdatalen = wrap_buf[0].cb_بفر + wrap_buf[1].cb_بفر +
               wrap_buf[2].cb_بفر;
  appdata = curlx_malloc(appdatalen);
  اگر(!appdata) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  /* Populate the encryption buffer */
  حافظہ_نقل(appdata, wrap_buf[0].pv_بفر, wrap_buf[0].cb_بفر);
  انحراف += wrap_buf[0].cb_بفر;
  حافظہ_نقل(appdata + انحراف, wrap_buf[1].pv_بفر, wrap_buf[1].cb_بفر);
  انحراف += wrap_buf[1].cb_بفر;
  حافظہ_نقل(appdata + انحراف, wrap_buf[2].pv_بفر, wrap_buf[2].cb_بفر);

برآمد:
  /* Free all of our local buffers */
  curlx_free(padding);
  curlx_free(پیغام_2);
  curlx_free(trailer);

  اگر(نتیجہ)
    واپس نتیجہ;

  /* Return the response. */
  Curl_bufref_سیٹ(برآمد, appdata, appdatalen, curl_free);
  واپس CURLE_OK;
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
خالی Curl_auth_cleanup_gssapi(ساخت kerberos5data *krb5)
{
  /* Free our security context */
  اگر(krb5->context) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);
    curlx_safefree(krb5->context);
  }

  /* Free our credentials handle */
  اگر(krb5->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(krb5->credentials);
    curlx_safefree(krb5->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_شناختی_عنصر(krb5->p_شناختی_عنصر);
  krb5->p_شناختی_عنصر = تہی;

  /* Free the SPN and output token */
  curlx_safefree(krb5->spn);
  curlx_safefree(krb5->output_token);

  /* Reset any variables */
  krb5->token_max = 0;
}

#اگر_ختم /* USE_WINDOWS_SSPI && USE_KERBEROS5 */
