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
#ضمن "curl_setup.h"

#إذا defined(USE_WINDOWS_SSPI) && defined(USE_KERBEROS5)

#ضمن "vauth/vauth.h"
#ضمن "curl_trc.h"

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by Windows SSPI.
 */
bool Curl_auth_is_gssapi_supported(فراغ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_وضعیت وضعیت;

  /* Query the security package for Kerberos */
  وضعیت = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(متن_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  إذا(وضعیت == SEC_E_OK) {
    Curl_pSecFn->Free_Context_میانگیر(SecurityPackage);
  }

  أرجع وضعیت == SEC_E_OK;
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
CURLcode Curl_auth_create_gssapi_user_پیام(بنية Curl_easy *داده,
                                              بنية Curl_creds *creds,
                                              ثابت حرف *default_service,
                                              ثابت حرف *host,
                                              ثابت bool mutual_auth,
                                              ثابت بنية bufref *chlg,
                                              بنية kerberos5data *krb5,
                                              بنية bufref *بیرون)
{
  CURLcode نتیجه = CURLE_OK;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  Sec_میانگیر chlg_buf;
  Sec_میانگیر resp_buf;
  Sec_میانگیر_Desc chlg_desc;
  Sec_میانگیر_Desc resp_desc;
  SECURITY_وضعیت وضعیت;
  بیعلامت دراز attrs;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  إذا(!krb5->spn) {
    /* Generate our SPN */
    krb5->spn = Curl_auth_build_spn(service, host, عدم);
    إذا(!krb5->spn)
      أرجع CURLE_بیرون_از_حافظه;
  }

  إذا(!krb5->output_token) {
    /* Query the security package for Kerberos */
    وضعیت = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(متن_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);
    إذا(وضعیت != SEC_E_OK) {
      failf(داده, "SSPI: could not get auth info");
      أرجع CURLE_AUTH_ERROR;
    }

    krb5->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_میانگیر(SecurityPackage);

    /* Allocate our response buffer */
    krb5->output_token = curlx_malloc(krb5->token_max);
    إذا(!krb5->output_token)
      أرجع CURLE_بیرون_از_حافظه;
  }

  إذا(!krb5->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    إذا(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      نتیجه = Curl_create_sspi_عنصر_خنثی(
        creds->user, creds->passwd, &krb5->عنصر_خنثی);
      إذا(نتیجه)
        أرجع نتیجه;

      /* Allow proper cleanup of the identity structure */
      krb5->p_عنصر_خنثی = &krb5->عنصر_خنثی;
    }
    وإلا
      /* Use the current Windows user */
      krb5->p_عنصر_خنثی = عدم;

    /* Allocate our credentials handle */
    krb5->credentials = curlx_calloc(1, حجم(CredHandle));
    إذا(!krb5->credentials)
      أرجع CURLE_بیرون_از_حافظه;

    /* Acquire our credentials handle */
    وضعیت = Curl_pSecFn->AcquireCredentialsHandle(عدم,
                                 CURL_UNCONST(متن_2(SP_NAME_KERBEROS)),
                                 SECPKG_CRED_OUTBOUND, عدم,
                                 krb5->p_عنصر_خنثی, عدم, عدم,
                                 krb5->credentials, عدم);
    إذا(وضعیت != SEC_E_OK) {
      curlx_safefree(krb5->credentials);
      أرجع CURLE_LOGIN_DENIED;
    }

    /* Allocate our new context handle */
    krb5->context = curlx_calloc(1, حجم(CtxtHandle));
    إذا(!krb5->context)
      أرجع CURLE_بیرون_از_حافظه;
  }

  إذا(chlg) {
    إذا(!Curl_bufref_len(chlg)) {
      infof(داده, "GSSAPI handshake failure (empty challenge message)");
      أرجع CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.ul_نسخه = SECBUFFER_نسخه;
    chlg_desc.cBuffers  = 1;
    chlg_desc.pBuffers  = &chlg_buf;
    chlg_buf.میانگیر_Type = SECBUFFER_TOKEN;
    chlg_buf.pv_میانگیر   = CURL_UNCONST(Curl_bufref_ptr(chlg));
    chlg_buf.cb_میانگیر   = curlx_uztoul(Curl_bufref_len(chlg));
  }

  /* Setup the response "output" security buffer */
  resp_desc.ul_نسخه = SECBUFFER_نسخه;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.میانگیر_Type = SECBUFFER_TOKEN;
  resp_buf.pv_میانگیر   = krb5->output_token;
  resp_buf.cb_میانگیر   = curlx_uztoul(krb5->token_max);

  /* Generate our challenge-response message */
  وضعیت = Curl_pSecFn->InitializeSecurityContext(krb5->credentials,
                                               chlg ? krb5->context : عدم,
                                               krb5->spn,
                                               (mutual_auth ?
                                                ISC_REQ_MUTUAL_AUTH : 0),
                                               0, SECURITY_NATIVE_DREP,
                                               chlg ? &chlg_desc : عدم, 0,
                                               &context,
                                               &resp_desc, &attrs, عدم);

  إذا(وضعیت == SEC_E_INSUFFICIENT_حافظه)
    أرجع CURLE_بیرون_از_حافظه;

  إذا(وضعیت != SEC_E_OK && وضعیت != SEC_I_CONTINUE_NEEDED)
    أرجع CURLE_AUTH_ERROR;

  إذا(memcmp(&context, krb5->context, حجم(context))) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);

    کپی_حافظه(krb5->context, &context, حجم(context));
  }

  إذا(resp_buf.cb_میانگیر) {
    نتیجه = Curl_bufref_memdup0(بیرون, resp_buf.pv_میانگیر, resp_buf.cb_میانگیر);
  }
  وإلا إذا(mutual_auth)
    Curl_bufref_مجموعه(بیرون, "", 0, عدم);
  وإلا
    Curl_bufref_مجموعه(بیرون, عدم, 0, عدم);

  أرجع نتیجه;
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
CURLcode Curl_auth_create_gssapi_security_پیام(بنية Curl_easy *داده,
                                                  ثابت حرف *authzid,
                                                  ثابت بنية bufref *chlg,
                                                  بنية kerberos5data *krb5,
                                                  بنية bufref *بیرون)
{
  CURLcode نتیجه = CURLE_OK;
  size_t جابه_جایی = 0;
  size_t messagelen = 0;
  size_t appdatalen = 0;
  بیعلامت حرف *trailer = عدم;
  بیعلامت حرف *پیام_2 = عدم;
  بیعلامت حرف *padding = عدم;
  بیعلامت حرف *appdata = عدم;
  Sec_میانگیر درون_داد_buf[2];
  Sec_میانگیر wrap_buf[3];
  Sec_میانگیر_Desc درون_داد_desc;
  Sec_میانگیر_Desc wrap_desc;
  بیعلامت حرف *indata;
  بیعلامت دراز qop = 0;
  بیعلامت دراز sec_layer = 0;
  بیعلامت دراز max_اندازه = 0;
  SecPkgContext_Sizes sizes;
  SECURITY_وضعیت وضعیت;

  /* Ensure we have a valid challenge message */
  DEBUGASSERT(chlg);
  إذا(!Curl_bufref_len(chlg)) {
    infof(داده, "GSSAPI handshake failure (empty security message)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Get our response size information */
  وضعیت = Curl_pSecFn->Query_Context_صفات(krb5->context,
                                               SECPKG_ATTR_SIZES, &sizes);

  إذا(وضعیت == SEC_E_INSUFFICIENT_حافظه)
    أرجع CURLE_بیرون_از_حافظه;

  إذا(وضعیت != SEC_E_OK)
    أرجع CURLE_AUTH_ERROR;

  /* Setup the "input" security buffer */
  درون_داد_desc.ul_نسخه = SECBUFFER_نسخه;
  درون_داد_desc.cBuffers = 2;
  درون_داد_desc.pBuffers = درون_داد_buf;
  درون_داد_buf[0].میانگیر_Type = SECBUFFER_جریان;
  درون_داد_buf[0].pv_میانگیر = CURL_UNCONST(Curl_bufref_ptr(chlg));
  درون_داد_buf[0].cb_میانگیر = curlx_uztoul(Curl_bufref_len(chlg));
  درون_داد_buf[1].میانگیر_Type = SECBUFFER_داده;
  درون_داد_buf[1].pv_میانگیر = عدم;
  درون_داد_buf[1].cb_میانگیر = 0;

  /* Decrypt the inbound challenge and obtain the qop. The encrypted message
     is decrypted in place, overwriting the original contents of its buffer.
     The SECBUFFER_DATA receives a pointer to the message in
     SECBUFFER_STREAM. */
  وضعیت = Curl_pSecFn->Decrypt_پیام(krb5->context, &درون_داد_desc, 0, &qop);
  إذا(وضعیت != SEC_E_OK) {
    infof(داده, "GSSAPI handshake failure (decryption failed)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  إذا(درون_داد_buf[1].cb_میانگیر != 4) {
    infof(داده, "GSSAPI handshake failure (invalid security data)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = درون_داد_buf[1].pv_میانگیر;
  sec_layer = indata[0];
  max_اندازه = ((بیعلامت دراز)indata[1] << 16) |
             ((بیعلامت دراز)indata[2] << 8) | indata[3];

  /* Process the security layer */
  إذا(!(sec_layer & KERB_WRAP_NO_ENCRYPT)) {
    infof(داده, "GSSAPI handshake failure (invalid security layer)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= KERB_WRAP_NO_ENCRYPT;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  إذا(max_اندازه > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_اندازه = 0;
  }

  /* Allocate the trailer */
  trailer = curlx_malloc(sizes.cbSecurityTrailer);
  إذا(!trailer)
    أرجع CURLE_بیرون_از_حافظه;

  /* Allocate our message */
  messagelen = 4;
  إذا(authzid)
    messagelen += طول_رشته(authzid);
  پیام_2 = curlx_malloc(messagelen);
  إذا(!پیام_2) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  /* Populate the message with the security layer and client supported receive
     message size. */
  پیام_2[0] = sec_layer & 0xFF;
  پیام_2[1] = (max_اندازه >> 16) & 0xFF;
  پیام_2[2] = (max_اندازه >> 8) & 0xFF;
  پیام_2[3] = max_اندازه & 0xFF;

  /* If given, append the authorization identity. */

  إذا(authzid && *authzid)
    کپی_حافظه(پیام_2 + 4, authzid, messagelen - 4);

  /* Allocate the padding */
  padding = curlx_malloc(sizes.cb_بلوک_اندازه);
  إذا(!padding) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  /* Setup the "authentication data" security buffer */
  wrap_desc.ul_نسخه    = SECBUFFER_نسخه;
  wrap_desc.cBuffers     = 3;
  wrap_desc.pBuffers     = wrap_buf;
  wrap_buf[0].میانگیر_Type = SECBUFFER_TOKEN;
  wrap_buf[0].pv_میانگیر   = trailer;
  wrap_buf[0].cb_میانگیر   = sizes.cbSecurityTrailer;
  wrap_buf[1].میانگیر_Type = SECBUFFER_داده;
  wrap_buf[1].pv_میانگیر   = پیام_2;
  wrap_buf[1].cb_میانگیر   = curlx_uztoul(messagelen);
  wrap_buf[2].میانگیر_Type = SECBUFFER_PADDING;
  wrap_buf[2].pv_میانگیر   = padding;
  wrap_buf[2].cb_میانگیر   = sizes.cb_بلوک_اندازه;

  /* Encrypt the data */
  وضعیت = Curl_pSecFn->Encrypt_پیام(krb5->context, KERB_WRAP_NO_ENCRYPT,
                                       &wrap_desc, 0);
  إذا(وضعیت != SEC_E_OK) {
    إذا(وضعیت == SEC_E_INSUFFICIENT_حافظه)
      نتیجه = CURLE_بیرون_از_حافظه;
    وإلا
      نتیجه = CURLE_AUTH_ERROR;
    اذهب بیرون;
  }

  /* Allocate the encryption (wrap) buffer */
  appdatalen = wrap_buf[0].cb_میانگیر + wrap_buf[1].cb_میانگیر +
               wrap_buf[2].cb_میانگیر;
  appdata = curlx_malloc(appdatalen);
  إذا(!appdata) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  /* Populate the encryption buffer */
  کپی_حافظه(appdata, wrap_buf[0].pv_میانگیر, wrap_buf[0].cb_میانگیر);
  جابه_جایی += wrap_buf[0].cb_میانگیر;
  کپی_حافظه(appdata + جابه_جایی, wrap_buf[1].pv_میانگیر, wrap_buf[1].cb_میانگیر);
  جابه_جایی += wrap_buf[1].cb_میانگیر;
  کپی_حافظه(appdata + جابه_جایی, wrap_buf[2].pv_میانگیر, wrap_buf[2].cb_میانگیر);

بیرون:
  /* Free all of our local buffers */
  curlx_free(padding);
  curlx_free(پیام_2);
  curlx_free(trailer);

  إذا(نتیجه)
    أرجع نتیجه;

  /* Return the response. */
  Curl_bufref_مجموعه(بیرون, appdata, appdatalen, curl_free);
  أرجع CURLE_OK;
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
فراغ Curl_auth_cleanup_gssapi(بنية kerberos5data *krb5)
{
  /* Free our security context */
  إذا(krb5->context) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);
    curlx_safefree(krb5->context);
  }

  /* Free our credentials handle */
  إذا(krb5->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(krb5->credentials);
    curlx_safefree(krb5->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_عنصر_خنثی(krb5->p_عنصر_خنثی);
  krb5->p_عنصر_خنثی = عدم;

  /* Free the SPN and output token */
  curlx_safefree(krb5->spn);
  curlx_safefree(krb5->output_token);

  /* Reset any variables */
  krb5->token_max = 0;
}

#پایان_اگر /* USE_WINDOWS_SSPI && USE_KERBEROS5 */
