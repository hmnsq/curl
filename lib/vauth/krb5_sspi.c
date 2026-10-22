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
  SECURITY_حالت حالت;

  /* Query the security package for Kerberos */
  حالت = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(متن_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  إذا(حالت == SEC_E_OK) {
    Curl_pSecFn->Free_Context_بفر(SecurityPackage);
  }

  أرجع حالت == SEC_E_OK;
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
CURLcode Curl_auth_create_gssapi_user_پیغام(بنية Curl_easy *معلومات,
                                              بنية Curl_creds *creds,
                                              ثابت حرف *default_service,
                                              ثابت حرف *host,
                                              ثابت bool mutual_auth,
                                              ثابت بنية bufref *chlg,
                                              بنية kerberos5data *krb5,
                                              بنية bufref *بهر)
{
  CURLcode پایله = CURLE_OK;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  Sec_بفر chlg_buf;
  Sec_بفر resp_buf;
  Sec_بفر_Desc chlg_desc;
  Sec_بفر_Desc resp_desc;
  SECURITY_حالت حالت;
  بینښه اوږد attrs;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  إذا(!krb5->spn) {
    /* Generate our SPN */
    krb5->spn = Curl_auth_build_spn(service, host, عدم);
    إذا(!krb5->spn)
      أرجع CURLE_بهر_د_حافظه;
  }

  إذا(!krb5->output_token) {
    /* Query the security package for Kerberos */
    حالت = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(متن_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);
    إذا(حالت != SEC_E_OK) {
      failf(معلومات, "SSPI: could not get auth info");
      أرجع CURLE_AUTH_ERROR;
    }

    krb5->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_بفر(SecurityPackage);

    /* Allocate our response buffer */
    krb5->output_token = curlx_malloc(krb5->token_max);
    إذا(!krb5->output_token)
      أرجع CURLE_بهر_د_حافظه;
  }

  إذا(!krb5->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    إذا(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      پایله = Curl_create_sspi_بې_اغېزه_عنصر(
        creds->user, creds->passwd, &krb5->بې_اغېزه_عنصر);
      إذا(پایله)
        أرجع پایله;

      /* Allow proper cleanup of the identity structure */
      krb5->p_بې_اغېزه_عنصر = &krb5->بې_اغېزه_عنصر;
    }
    وإلا
      /* Use the current Windows user */
      krb5->p_بې_اغېزه_عنصر = عدم;

    /* Allocate our credentials handle */
    krb5->credentials = curlx_calloc(1, حجم(CredHandle));
    إذا(!krb5->credentials)
      أرجع CURLE_بهر_د_حافظه;

    /* Acquire our credentials handle */
    حالت = Curl_pSecFn->AcquireCredentialsHandle(عدم,
                                 CURL_UNCONST(متن_2(SP_NAME_KERBEROS)),
                                 SECPKG_CRED_OUTBOUND, عدم,
                                 krb5->p_بې_اغېزه_عنصر, عدم, عدم,
                                 krb5->credentials, عدم);
    إذا(حالت != SEC_E_OK) {
      curlx_safefree(krb5->credentials);
      أرجع CURLE_LOGIN_DENIED;
    }

    /* Allocate our new context handle */
    krb5->context = curlx_calloc(1, حجم(CtxtHandle));
    إذا(!krb5->context)
      أرجع CURLE_بهر_د_حافظه;
  }

  إذا(chlg) {
    إذا(!Curl_bufref_len(chlg)) {
      infof(معلومات, "GSSAPI handshake failure (empty challenge message)");
      أرجع CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.ul_نسخه = SECBUFFER_نسخه;
    chlg_desc.cBuffers  = 1;
    chlg_desc.pBuffers  = &chlg_buf;
    chlg_buf.بفر_Type = SECBUFFER_TOKEN;
    chlg_buf.pv_بفر   = CURL_UNCONST(Curl_bufref_ptr(chlg));
    chlg_buf.cb_بفر   = curlx_uztoul(Curl_bufref_len(chlg));
  }

  /* Setup the response "output" security buffer */
  resp_desc.ul_نسخه = SECBUFFER_نسخه;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.بفر_Type = SECBUFFER_TOKEN;
  resp_buf.pv_بفر   = krb5->output_token;
  resp_buf.cb_بفر   = curlx_uztoul(krb5->token_max);

  /* Generate our challenge-response message */
  حالت = Curl_pSecFn->InitializeSecurityContext(krb5->credentials,
                                               chlg ? krb5->context : عدم,
                                               krb5->spn,
                                               (mutual_auth ?
                                                ISC_REQ_MUTUAL_AUTH : 0),
                                               0, SECURITY_NATIVE_DREP,
                                               chlg ? &chlg_desc : عدم, 0,
                                               &context,
                                               &resp_desc, &attrs, عدم);

  إذا(حالت == SEC_E_INSUFFICIENT_حافظه)
    أرجع CURLE_بهر_د_حافظه;

  إذا(حالت != SEC_E_OK && حالت != SEC_I_CONTINUE_NEEDED)
    أرجع CURLE_AUTH_ERROR;

  إذا(memcmp(&context, krb5->context, حجم(context))) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);

    حافظه_نقل(krb5->context, &context, حجم(context));
  }

  إذا(resp_buf.cb_بفر) {
    پایله = Curl_bufref_memdup0(بهر, resp_buf.pv_بفر, resp_buf.cb_بفر);
  }
  وإلا إذا(mutual_auth)
    Curl_bufref_سېټ(بهر, "", 0, عدم);
  وإلا
    Curl_bufref_سېټ(بهر, عدم, 0, عدم);

  أرجع پایله;
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
CURLcode Curl_auth_create_gssapi_security_پیغام(بنية Curl_easy *معلومات,
                                                  ثابت حرف *authzid,
                                                  ثابت بنية bufref *chlg,
                                                  بنية kerberos5data *krb5,
                                                  بنية bufref *بهر)
{
  CURLcode پایله = CURLE_OK;
  size_t دننه_واټن = 0;
  size_t messagelen = 0;
  size_t appdatalen = 0;
  بینښه حرف *trailer = عدم;
  بینښه حرف *پیغام_2 = عدم;
  بینښه حرف *padding = عدم;
  بینښه حرف *appdata = عدم;
  Sec_بفر ننوت_buf[2];
  Sec_بفر wrap_buf[3];
  Sec_بفر_Desc ننوت_desc;
  Sec_بفر_Desc wrap_desc;
  بینښه حرف *indata;
  بینښه اوږد qop = 0;
  بینښه اوږد sec_layer = 0;
  بینښه اوږد max_اندازه = 0;
  SecPkgContext_Sizes sizes;
  SECURITY_حالت حالت;

  /* Ensure we have a valid challenge message */
  DEBUGASSERT(chlg);
  إذا(!Curl_bufref_len(chlg)) {
    infof(معلومات, "GSSAPI handshake failure (empty security message)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Get our response size information */
  حالت = Curl_pSecFn->Query_Context_ځانګړتیاوې(krb5->context,
                                               SECPKG_ATTR_SIZES, &sizes);

  إذا(حالت == SEC_E_INSUFFICIENT_حافظه)
    أرجع CURLE_بهر_د_حافظه;

  إذا(حالت != SEC_E_OK)
    أرجع CURLE_AUTH_ERROR;

  /* Setup the "input" security buffer */
  ننوت_desc.ul_نسخه = SECBUFFER_نسخه;
  ننوت_desc.cBuffers = 2;
  ننوت_desc.pBuffers = ننوت_buf;
  ننوت_buf[0].بفر_Type = SECBUFFER_بهیر;
  ننوت_buf[0].pv_بفر = CURL_UNCONST(Curl_bufref_ptr(chlg));
  ننوت_buf[0].cb_بفر = curlx_uztoul(Curl_bufref_len(chlg));
  ننوت_buf[1].بفر_Type = SECBUFFER_معلومات;
  ننوت_buf[1].pv_بفر = عدم;
  ننوت_buf[1].cb_بفر = 0;

  /* Decrypt the inbound challenge and obtain the qop. The encrypted message
     is decrypted in place, overwriting the original contents of its buffer.
     The SECBUFFER_DATA receives a pointer to the message in
     SECBUFFER_STREAM. */
  حالت = Curl_pSecFn->Decrypt_پیغام(krb5->context, &ننوت_desc, 0, &qop);
  إذا(حالت != SEC_E_OK) {
    infof(معلومات, "GSSAPI handshake failure (decryption failed)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  إذا(ننوت_buf[1].cb_بفر != 4) {
    infof(معلومات, "GSSAPI handshake failure (invalid security data)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = ننوت_buf[1].pv_بفر;
  sec_layer = indata[0];
  max_اندازه = ((بینښه اوږد)indata[1] << 16) |
             ((بینښه اوږد)indata[2] << 8) | indata[3];

  /* Process the security layer */
  إذا(!(sec_layer & KERB_WRAP_NO_ENCRYPT)) {
    infof(معلومات, "GSSAPI handshake failure (invalid security layer)");
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
    أرجع CURLE_بهر_د_حافظه;

  /* Allocate our message */
  messagelen = 4;
  إذا(authzid)
    messagelen += مزي_اوږدوالی(authzid);
  پیغام_2 = curlx_malloc(messagelen);
  إذا(!پیغام_2) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  /* Populate the message with the security layer and client supported receive
     message size. */
  پیغام_2[0] = sec_layer & 0xFF;
  پیغام_2[1] = (max_اندازه >> 16) & 0xFF;
  پیغام_2[2] = (max_اندازه >> 8) & 0xFF;
  پیغام_2[3] = max_اندازه & 0xFF;

  /* If given, append the authorization identity. */

  إذا(authzid && *authzid)
    حافظه_نقل(پیغام_2 + 4, authzid, messagelen - 4);

  /* Allocate the padding */
  padding = curlx_malloc(sizes.cb_بلاک_اندازه);
  إذا(!padding) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  /* Setup the "authentication data" security buffer */
  wrap_desc.ul_نسخه    = SECBUFFER_نسخه;
  wrap_desc.cBuffers     = 3;
  wrap_desc.pBuffers     = wrap_buf;
  wrap_buf[0].بفر_Type = SECBUFFER_TOKEN;
  wrap_buf[0].pv_بفر   = trailer;
  wrap_buf[0].cb_بفر   = sizes.cbSecurityTrailer;
  wrap_buf[1].بفر_Type = SECBUFFER_معلومات;
  wrap_buf[1].pv_بفر   = پیغام_2;
  wrap_buf[1].cb_بفر   = curlx_uztoul(messagelen);
  wrap_buf[2].بفر_Type = SECBUFFER_PADDING;
  wrap_buf[2].pv_بفر   = padding;
  wrap_buf[2].cb_بفر   = sizes.cb_بلاک_اندازه;

  /* Encrypt the data */
  حالت = Curl_pSecFn->Encrypt_پیغام(krb5->context, KERB_WRAP_NO_ENCRYPT,
                                       &wrap_desc, 0);
  إذا(حالت != SEC_E_OK) {
    إذا(حالت == SEC_E_INSUFFICIENT_حافظه)
      پایله = CURLE_بهر_د_حافظه;
    وإلا
      پایله = CURLE_AUTH_ERROR;
    اذهب بهر;
  }

  /* Allocate the encryption (wrap) buffer */
  appdatalen = wrap_buf[0].cb_بفر + wrap_buf[1].cb_بفر +
               wrap_buf[2].cb_بفر;
  appdata = curlx_malloc(appdatalen);
  إذا(!appdata) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  /* Populate the encryption buffer */
  حافظه_نقل(appdata, wrap_buf[0].pv_بفر, wrap_buf[0].cb_بفر);
  دننه_واټن += wrap_buf[0].cb_بفر;
  حافظه_نقل(appdata + دننه_واټن, wrap_buf[1].pv_بفر, wrap_buf[1].cb_بفر);
  دننه_واټن += wrap_buf[1].cb_بفر;
  حافظه_نقل(appdata + دننه_واټن, wrap_buf[2].pv_بفر, wrap_buf[2].cb_بفر);

بهر:
  /* Free all of our local buffers */
  curlx_free(padding);
  curlx_free(پیغام_2);
  curlx_free(trailer);

  إذا(پایله)
    أرجع پایله;

  /* Return the response. */
  Curl_bufref_سېټ(بهر, appdata, appdatalen, curl_free);
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
  Curl_sspi_free_بې_اغېزه_عنصر(krb5->p_بې_اغېزه_عنصر);
  krb5->p_بې_اغېزه_عنصر = عدم;

  /* Free the SPN and output token */
  curlx_safefree(krb5->spn);
  curlx_safefree(krb5->output_token);

  /* Reset any variables */
  krb5->token_max = 0;
}

#د_که_پای /* USE_WINDOWS_SSPI && USE_KERBEROS5 */
