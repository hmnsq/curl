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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_WINDOWS_SSPI) && defined(USE_KERBEROS5)

#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by Windows SSPI.
 */
bool Curl_auth_is_gssapi_supported(ਖਾਲੀ)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;

  /* Query the security package for Kerberos */
  ਸਥਿਤੀ = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(ਪਾਠ_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ਜੇ(ਸਥਿਤੀ == SEC_E_OK) {
    Curl_pSecFn->Free_Context_ਬਫਰ(SecurityPackage);
  }

  ਵਾਪਸ ਸਥਿਤੀ == SEC_E_OK;
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
CURLcode Curl_auth_create_gssapi_user_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਬਣਤਰ Curl_creds *creds,
                                              ਅਚਲ ਅੱਖਰ *default_service,
                                              ਅਚਲ ਅੱਖਰ *host,
                                              ਅਚਲ bool mutual_auth,
                                              ਅਚਲ ਬਣਤਰ bufref *chlg,
                                              ਬਣਤਰ kerberos5data *krb5,
                                              ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  Sec_ਬਫਰ chlg_buf;
  Sec_ਬਫਰ resp_buf;
  Sec_ਬਫਰ_Desc chlg_desc;
  Sec_ਬਫਰ_Desc resp_desc;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ attrs;
  ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  ਜੇ(!krb5->spn) {
    /* Generate our SPN */
    krb5->spn = Curl_auth_build_spn(service, host, ਨਿਰਾ);
    ਜੇ(!krb5->spn)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(!krb5->output_token) {
    /* Query the security package for Kerberos */
    ਸਥਿਤੀ = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(ਪਾਠ_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);
    ਜੇ(ਸਥਿਤੀ != SEC_E_OK) {
      failf(ਅੰਕੜੇ, "SSPI: could not get auth info");
      ਵਾਪਸ CURLE_AUTH_ERROR;
    }

    krb5->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_ਬਫਰ(SecurityPackage);

    /* Allocate our response buffer */
    krb5->output_token = curlx_malloc(krb5->token_max);
    ਜੇ(!krb5->output_token)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(!krb5->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    ਜੇ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ਨਤੀਜਾ = Curl_create_sspi_ਪਛਾਣ_ਤੱਤ(
        creds->user, creds->passwd, &krb5->ਪਛਾਣ_ਤੱਤ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      /* Allow proper cleanup of the identity structure */
      krb5->p_ਪਛਾਣ_ਤੱਤ = &krb5->ਪਛਾਣ_ਤੱਤ;
    }
    ਨਹੀਂਤਾਂ
      /* Use the current Windows user */
      krb5->p_ਪਛਾਣ_ਤੱਤ = ਨਿਰਾ;

    /* Allocate our credentials handle */
    krb5->credentials = curlx_calloc(1, ਆਕਾਰ(CredHandle));
    ਜੇ(!krb5->credentials)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    /* Acquire our credentials handle */
    ਸਥਿਤੀ = Curl_pSecFn->AcquireCredentialsHandle(ਨਿਰਾ,
                                 CURL_UNCONST(ਪਾਠ_2(SP_NAME_KERBEROS)),
                                 SECPKG_CRED_OUTBOUND, ਨਿਰਾ,
                                 krb5->p_ਪਛਾਣ_ਤੱਤ, ਨਿਰਾ, ਨਿਰਾ,
                                 krb5->credentials, ਨਿਰਾ);
    ਜੇ(ਸਥਿਤੀ != SEC_E_OK) {
      curlx_safefree(krb5->credentials);
      ਵਾਪਸ CURLE_LOGIN_DENIED;
    }

    /* Allocate our new context handle */
    krb5->context = curlx_calloc(1, ਆਕਾਰ(CtxtHandle));
    ਜੇ(!krb5->context)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(chlg) {
    ਜੇ(!Curl_bufref_len(chlg)) {
      infof(ਅੰਕੜੇ, "GSSAPI handshake failure (empty challenge message)");
      ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;
    chlg_desc.cBuffers  = 1;
    chlg_desc.pBuffers  = &chlg_buf;
    chlg_buf.ਬਫਰ_Type = SECBUFFER_TOKEN;
    chlg_buf.pv_ਬਫਰ   = CURL_UNCONST(Curl_bufref_ptr(chlg));
    chlg_buf.cb_ਬਫਰ   = curlx_uztoul(Curl_bufref_len(chlg));
  }

  /* Setup the response "output" security buffer */
  resp_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.ਬਫਰ_Type = SECBUFFER_TOKEN;
  resp_buf.pv_ਬਫਰ   = krb5->output_token;
  resp_buf.cb_ਬਫਰ   = curlx_uztoul(krb5->token_max);

  /* Generate our challenge-response message */
  ਸਥਿਤੀ = Curl_pSecFn->InitializeSecurityContext(krb5->credentials,
                                               chlg ? krb5->context : ਨਿਰਾ,
                                               krb5->spn,
                                               (mutual_auth ?
                                                ISC_REQ_MUTUAL_AUTH : 0),
                                               0, SECURITY_NATIVE_DREP,
                                               chlg ? &chlg_desc : ਨਿਰਾ, 0,
                                               &context,
                                               &resp_desc, &attrs, ਨਿਰਾ);

  ਜੇ(ਸਥਿਤੀ == SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(ਸਥਿਤੀ != SEC_E_OK && ਸਥਿਤੀ != SEC_I_CONTINUE_NEEDED)
    ਵਾਪਸ CURLE_AUTH_ERROR;

  ਜੇ(memcmp(&context, krb5->context, ਆਕਾਰ(context))) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(krb5->context, &context, ਆਕਾਰ(context));
  }

  ਜੇ(resp_buf.cb_ਬਫਰ) {
    ਨਤੀਜਾ = Curl_bufref_memdup0(ਬਾਹਰਲਾ, resp_buf.pv_ਬਫਰ, resp_buf.cb_ਬਫਰ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(mutual_auth)
    Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, "", 0, ਨਿਰਾ);
  ਨਹੀਂਤਾਂ
    Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, ਨਿਰਾ, 0, ਨਿਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
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
CURLcode Curl_auth_create_gssapi_security_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                  ਅਚਲ ਅੱਖਰ *authzid,
                                                  ਅਚਲ ਬਣਤਰ bufref *chlg,
                                                  ਬਣਤਰ kerberos5data *krb5,
                                                  ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t ਵਿੱਥ = 0;
  size_t messagelen = 0;
  size_t appdatalen = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *trailer = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸੁਨੇਹਾ_2 = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *padding = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *appdata = ਨਿਰਾ;
  Sec_ਬਫਰ ਨਿਵੇਸ਼_buf[2];
  Sec_ਬਫਰ wrap_buf[3];
  Sec_ਬਫਰ_Desc ਨਿਵੇਸ਼_desc;
  Sec_ਬਫਰ_Desc wrap_desc;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *indata;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ qop = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sec_layer = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ max_ਮਾਪ = 0;
  SecPkgContext_Sizes sizes;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;

  /* Ensure we have a valid challenge message */
  DEBUGASSERT(chlg);
  ਜੇ(!Curl_bufref_len(chlg)) {
    infof(ਅੰਕੜੇ, "GSSAPI handshake failure (empty security message)");
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Get our response size information */
  ਸਥਿਤੀ = Curl_pSecFn->Query_Context_ਗੁਣ(krb5->context,
                                               SECPKG_ATTR_SIZES, &sizes);

  ਜੇ(ਸਥਿਤੀ == SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(ਸਥਿਤੀ != SEC_E_OK)
    ਵਾਪਸ CURLE_AUTH_ERROR;

  /* Setup the "input" security buffer */
  ਨਿਵੇਸ਼_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;
  ਨਿਵੇਸ਼_desc.cBuffers = 2;
  ਨਿਵੇਸ਼_desc.pBuffers = ਨਿਵੇਸ਼_buf;
  ਨਿਵੇਸ਼_buf[0].ਬਫਰ_Type = SECBUFFER_ਧਾਰਾ;
  ਨਿਵੇਸ਼_buf[0].pv_ਬਫਰ = CURL_UNCONST(Curl_bufref_ptr(chlg));
  ਨਿਵੇਸ਼_buf[0].cb_ਬਫਰ = curlx_uztoul(Curl_bufref_len(chlg));
  ਨਿਵੇਸ਼_buf[1].ਬਫਰ_Type = SECBUFFER_ਅੰਕੜੇ;
  ਨਿਵੇਸ਼_buf[1].pv_ਬਫਰ = ਨਿਰਾ;
  ਨਿਵੇਸ਼_buf[1].cb_ਬਫਰ = 0;

  /* Decrypt the inbound challenge and obtain the qop. The encrypted message
     is decrypted in place, overwriting the original contents of its buffer.
     The SECBUFFER_DATA receives a pointer to the message in
     SECBUFFER_STREAM. */
  ਸਥਿਤੀ = Curl_pSecFn->Decrypt_ਸੁਨੇਹਾ(krb5->context, &ਨਿਵੇਸ਼_desc, 0, &qop);
  ਜੇ(ਸਥਿਤੀ != SEC_E_OK) {
    infof(ਅੰਕੜੇ, "GSSAPI handshake failure (decryption failed)");
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  ਜੇ(ਨਿਵੇਸ਼_buf[1].cb_ਬਫਰ != 4) {
    infof(ਅੰਕੜੇ, "GSSAPI handshake failure (invalid security data)");
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = ਨਿਵੇਸ਼_buf[1].pv_ਬਫਰ;
  sec_layer = indata[0];
  max_ਮਾਪ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)indata[1] << 16) |
             ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)indata[2] << 8) | indata[3];

  /* Process the security layer */
  ਜੇ(!(sec_layer & KERB_WRAP_NO_ENCRYPT)) {
    infof(ਅੰਕੜੇ, "GSSAPI handshake failure (invalid security layer)");
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= KERB_WRAP_NO_ENCRYPT;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  ਜੇ(max_ਮਾਪ > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_ਮਾਪ = 0;
  }

  /* Allocate the trailer */
  trailer = curlx_malloc(sizes.cbSecurityTrailer);
  ਜੇ(!trailer)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Allocate our message */
  messagelen = 4;
  ਜੇ(authzid)
    messagelen += ਲੜੀ_ਲੰਬਾਈ(authzid);
  ਸੁਨੇਹਾ_2 = curlx_malloc(messagelen);
  ਜੇ(!ਸੁਨੇਹਾ_2) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Populate the message with the security layer and client supported receive
     message size. */
  ਸੁਨੇਹਾ_2[0] = sec_layer & 0xFF;
  ਸੁਨੇਹਾ_2[1] = (max_ਮਾਪ >> 16) & 0xFF;
  ਸੁਨੇਹਾ_2[2] = (max_ਮਾਪ >> 8) & 0xFF;
  ਸੁਨੇਹਾ_2[3] = max_ਮਾਪ & 0xFF;

  /* If given, append the authorization identity. */

  ਜੇ(authzid && *authzid)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਸੁਨੇਹਾ_2 + 4, authzid, messagelen - 4);

  /* Allocate the padding */
  padding = curlx_malloc(sizes.cb_ਖੰਡ_ਮਾਪ);
  ਜੇ(!padding) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Setup the "authentication data" security buffer */
  wrap_desc.ul_ਸੰਸਕਰਣ    = SECBUFFER_ਸੰਸਕਰਣ;
  wrap_desc.cBuffers     = 3;
  wrap_desc.pBuffers     = wrap_buf;
  wrap_buf[0].ਬਫਰ_Type = SECBUFFER_TOKEN;
  wrap_buf[0].pv_ਬਫਰ   = trailer;
  wrap_buf[0].cb_ਬਫਰ   = sizes.cbSecurityTrailer;
  wrap_buf[1].ਬਫਰ_Type = SECBUFFER_ਅੰਕੜੇ;
  wrap_buf[1].pv_ਬਫਰ   = ਸੁਨੇਹਾ_2;
  wrap_buf[1].cb_ਬਫਰ   = curlx_uztoul(messagelen);
  wrap_buf[2].ਬਫਰ_Type = SECBUFFER_PADDING;
  wrap_buf[2].pv_ਬਫਰ   = padding;
  wrap_buf[2].cb_ਬਫਰ   = sizes.cb_ਖੰਡ_ਮਾਪ;

  /* Encrypt the data */
  ਸਥਿਤੀ = Curl_pSecFn->Encrypt_ਸੁਨੇਹਾ(krb5->context, KERB_WRAP_NO_ENCRYPT,
                                       &wrap_desc, 0);
  ਜੇ(ਸਥਿਤੀ != SEC_E_OK) {
    ਜੇ(ਸਥਿਤੀ == SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ)
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_AUTH_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Allocate the encryption (wrap) buffer */
  appdatalen = wrap_buf[0].cb_ਬਫਰ + wrap_buf[1].cb_ਬਫਰ +
               wrap_buf[2].cb_ਬਫਰ;
  appdata = curlx_malloc(appdatalen);
  ਜੇ(!appdata) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Populate the encryption buffer */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(appdata, wrap_buf[0].pv_ਬਫਰ, wrap_buf[0].cb_ਬਫਰ);
  ਵਿੱਥ += wrap_buf[0].cb_ਬਫਰ;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(appdata + ਵਿੱਥ, wrap_buf[1].pv_ਬਫਰ, wrap_buf[1].cb_ਬਫਰ);
  ਵਿੱਥ += wrap_buf[1].cb_ਬਫਰ;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(appdata + ਵਿੱਥ, wrap_buf[2].pv_ਬਫਰ, wrap_buf[2].cb_ਬਫਰ);

ਬਾਹਰਲਾ:
  /* Free all of our local buffers */
  curlx_free(padding);
  curlx_free(ਸੁਨੇਹਾ_2);
  curlx_free(trailer);

  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Return the response. */
  Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, appdata, appdatalen, curl_free);
  ਵਾਪਸ CURLE_OK;
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
ਖਾਲੀ Curl_auth_cleanup_gssapi(ਬਣਤਰ kerberos5data *krb5)
{
  /* Free our security context */
  ਜੇ(krb5->context) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);
    curlx_safefree(krb5->context);
  }

  /* Free our credentials handle */
  ਜੇ(krb5->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(krb5->credentials);
    curlx_safefree(krb5->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(krb5->p_ਪਛਾਣ_ਤੱਤ);
  krb5->p_ਪਛਾਣ_ਤੱਤ = ਨਿਰਾ;

  /* Free the SPN and output token */
  curlx_safefree(krb5->spn);
  curlx_safefree(krb5->output_token);

  /* Reset any variables */
  krb5->token_max = 0;
}

#ਜੇ_ਅੰਤ /* USE_WINDOWS_SSPI && USE_KERBEROS5 */
