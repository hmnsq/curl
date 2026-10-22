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

#ಆದರೆ defined(USE_WINDOWS_SSPI) && defined(USE_KERBEROS5)

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
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;

  /* Query the security package for Kerberos */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_OK) {
    Curl_pSecFn->Free_Context_ಬಫರ್(SecurityPackage);
  }

  ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2 == SEC_E_OK;
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
CURLcode Curl_auth_create_gssapi_user_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ರಚನೆ Curl_creds *creds,
                                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                              ಸ್ಥಿರಾಂಕ bool mutual_auth,
                                              ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                                              ರಚನೆ kerberos5data *krb5,
                                              ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  Sec_ಬಫರ್ chlg_buf;
  Sec_ಬಫರ್ resp_buf;
  Sec_ಬಫರ್_Desc chlg_desc;
  Sec_ಬಫರ್_Desc resp_desc;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ attrs;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  ಆದರೆ(!krb5->spn) {
    /* Generate our SPN */
    krb5->spn = Curl_auth_build_spn(service, host, ಖಾಲಿ);
    ಆದರೆ(!krb5->spn)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(!krb5->output_token) {
    /* Query the security package for Kerberos */
    ಸ್ಥಿತಿ_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);
    ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK) {
      failf(ದತ್ತಾಂಶ, "SSPI: could not get auth info");
      ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
    }

    krb5->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_ಬಫರ್(SecurityPackage);

    /* Allocate our response buffer */
    krb5->output_token = curlx_malloc(krb5->token_max);
    ಆದರೆ(!krb5->output_token)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(!krb5->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    ಆದರೆ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ಫಲಿತಾಂಶ = Curl_create_sspi_ತಟಸ್ಥ(
        creds->user, creds->passwd, &krb5->ತಟಸ್ಥ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      /* Allow proper cleanup of the identity structure */
      krb5->p_ತಟಸ್ಥ = &krb5->ತಟಸ್ಥ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* Use the current Windows user */
      krb5->p_ತಟಸ್ಥ = ಖಾಲಿ;

    /* Allocate our credentials handle */
    krb5->credentials = curlx_calloc(1, ಗಾತ್ರ(CredHandle));
    ಆದರೆ(!krb5->credentials)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    /* Acquire our credentials handle */
    ಸ್ಥಿತಿ_2 = Curl_pSecFn->AcquireCredentialsHandle(ಖಾಲಿ,
                                 CURL_UNCONST(ಪಠ್ಯ_2(SP_NAME_KERBEROS)),
                                 SECPKG_CRED_OUTBOUND, ಖಾಲಿ,
                                 krb5->p_ತಟಸ್ಥ, ಖಾಲಿ, ಖಾಲಿ,
                                 krb5->credentials, ಖಾಲಿ);
    ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK) {
      curlx_safefree(krb5->credentials);
      ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
    }

    /* Allocate our new context handle */
    krb5->context = curlx_calloc(1, ಗಾತ್ರ(CtxtHandle));
    ಆದರೆ(!krb5->context)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(chlg) {
    ಆದರೆ(!Curl_bufref_len(chlg)) {
      infof(ದತ್ತಾಂಶ, "GSSAPI handshake failure (empty challenge message)");
      ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;
    chlg_desc.cBuffers  = 1;
    chlg_desc.pBuffers  = &chlg_buf;
    chlg_buf.ಬಫರ್_Type = SECBUFFER_TOKEN;
    chlg_buf.pv_ಬಫರ್   = CURL_UNCONST(Curl_bufref_ptr(chlg));
    chlg_buf.cb_ಬಫರ್   = curlx_uztoul(Curl_bufref_len(chlg));
  }

  /* Setup the response "output" security buffer */
  resp_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.ಬಫರ್_Type = SECBUFFER_TOKEN;
  resp_buf.pv_ಬಫರ್   = krb5->output_token;
  resp_buf.cb_ಬಫರ್   = curlx_uztoul(krb5->token_max);

  /* Generate our challenge-response message */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->InitializeSecurityContext(krb5->credentials,
                                               chlg ? krb5->context : ಖಾಲಿ,
                                               krb5->spn,
                                               (mutual_auth ?
                                                ISC_REQ_MUTUAL_AUTH : 0),
                                               0, SECURITY_NATIVE_DREP,
                                               chlg ? &chlg_desc : ಖಾಲಿ, 0,
                                               &context,
                                               &resp_desc, &attrs, ಖಾಲಿ);

  ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK && ಸ್ಥಿತಿ_2 != SEC_I_CONTINUE_NEEDED)
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;

  ಆದರೆ(memcmp(&context, krb5->context, ಗಾತ್ರ(context))) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);

    ಸ್ಮರಣೆ_ನಕಲು(krb5->context, &context, ಗಾತ್ರ(context));
  }

  ಆದರೆ(resp_buf.cb_ಬಫರ್) {
    ಫಲಿತಾಂಶ = Curl_bufref_memdup0(ಹೊರಹೋಗು, resp_buf.pv_ಬಫರ್, resp_buf.cb_ಬಫರ್);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(mutual_auth)
    Curl_bufref_ಗಣ(ಹೊರಹೋಗು, "", 0, ಖಾಲಿ);
  ಇಲ್ಲದಿದ್ದರೆ
    Curl_bufref_ಗಣ(ಹೊರಹೋಗು, ಖಾಲಿ, 0, ಖಾಲಿ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
CURLcode Curl_auth_create_gssapi_security_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *authzid,
                                                  ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *chlg,
                                                  ರಚನೆ kerberos5data *krb5,
                                                  ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t ಸ್ಥಾನಾಂತರ = 0;
  size_t messagelen = 0;
  size_t appdatalen = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *trailer = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಸಂದೇಶ_2 = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *padding = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *appdata = ಖಾಲಿ;
  Sec_ಬಫರ್ ಒಳಹರಿವು_buf[2];
  Sec_ಬಫರ್ wrap_buf[3];
  Sec_ಬಫರ್_Desc ಒಳಹರಿವು_desc;
  Sec_ಬಫರ್_Desc wrap_desc;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *indata;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ qop = 0;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sec_layer = 0;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ max_ಗಾತ್ರ = 0;
  SecPkgContext_Sizes sizes;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;

  /* Ensure we have a valid challenge message */
  DEBUGASSERT(chlg);
  ಆದರೆ(!Curl_bufref_len(chlg)) {
    infof(ದತ್ತಾಂಶ, "GSSAPI handshake failure (empty security message)");
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Get our response size information */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->Query_Context_ಗುಣಲಕ್ಷಣಗಳು(krb5->context,
                                               SECPKG_ATTR_SIZES, &sizes);

  ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK)
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;

  /* Setup the "input" security buffer */
  ಒಳಹರಿವು_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;
  ಒಳಹರಿವು_desc.cBuffers = 2;
  ಒಳಹರಿವು_desc.pBuffers = ಒಳಹರಿವು_buf;
  ಒಳಹರಿವು_buf[0].ಬಫರ್_Type = SECBUFFER_ಹರಿವು;
  ಒಳಹರಿವು_buf[0].pv_ಬಫರ್ = CURL_UNCONST(Curl_bufref_ptr(chlg));
  ಒಳಹರಿವು_buf[0].cb_ಬಫರ್ = curlx_uztoul(Curl_bufref_len(chlg));
  ಒಳಹರಿವು_buf[1].ಬಫರ್_Type = SECBUFFER_ದತ್ತಾಂಶ;
  ಒಳಹರಿವು_buf[1].pv_ಬಫರ್ = ಖಾಲಿ;
  ಒಳಹರಿವು_buf[1].cb_ಬಫರ್ = 0;

  /* Decrypt the inbound challenge and obtain the qop. The encrypted message
     is decrypted in place, overwriting the original contents of its buffer.
     The SECBUFFER_DATA receives a pointer to the message in
     SECBUFFER_STREAM. */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->Decrypt_ಸಂದೇಶ(krb5->context, &ಒಳಹರಿವು_desc, 0, &qop);
  ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK) {
    infof(ದತ್ತಾಂಶ, "GSSAPI handshake failure (decryption failed)");
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  ಆದರೆ(ಒಳಹರಿವು_buf[1].cb_ಬಫರ್ != 4) {
    infof(ದತ್ತಾಂಶ, "GSSAPI handshake failure (invalid security data)");
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = ಒಳಹರಿವು_buf[1].pv_ಬಫರ್;
  sec_layer = indata[0];
  max_ಗಾತ್ರ = ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)indata[1] << 16) |
             ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)indata[2] << 8) | indata[3];

  /* Process the security layer */
  ಆದರೆ(!(sec_layer & KERB_WRAP_NO_ENCRYPT)) {
    infof(ದತ್ತಾಂಶ, "GSSAPI handshake failure (invalid security layer)");
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= KERB_WRAP_NO_ENCRYPT;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  ಆದರೆ(max_ಗಾತ್ರ > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_ಗಾತ್ರ = 0;
  }

  /* Allocate the trailer */
  trailer = curlx_malloc(sizes.cbSecurityTrailer);
  ಆದರೆ(!trailer)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Allocate our message */
  messagelen = 4;
  ಆದರೆ(authzid)
    messagelen += ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(authzid);
  ಸಂದೇಶ_2 = curlx_malloc(messagelen);
  ಆದರೆ(!ಸಂದೇಶ_2) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Populate the message with the security layer and client supported receive
     message size. */
  ಸಂದೇಶ_2[0] = sec_layer & 0xFF;
  ಸಂದೇಶ_2[1] = (max_ಗಾತ್ರ >> 16) & 0xFF;
  ಸಂದೇಶ_2[2] = (max_ಗಾತ್ರ >> 8) & 0xFF;
  ಸಂದೇಶ_2[3] = max_ಗಾತ್ರ & 0xFF;

  /* If given, append the authorization identity. */

  ಆದರೆ(authzid && *authzid)
    ಸ್ಮರಣೆ_ನಕಲು(ಸಂದೇಶ_2 + 4, authzid, messagelen - 4);

  /* Allocate the padding */
  padding = curlx_malloc(sizes.cb_ಖಂಡ_ಗಾತ್ರ);
  ಆದರೆ(!padding) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Setup the "authentication data" security buffer */
  wrap_desc.ul_ಆವೃತ್ತಿ    = SECBUFFER_ಆವೃತ್ತಿ;
  wrap_desc.cBuffers     = 3;
  wrap_desc.pBuffers     = wrap_buf;
  wrap_buf[0].ಬಫರ್_Type = SECBUFFER_TOKEN;
  wrap_buf[0].pv_ಬಫರ್   = trailer;
  wrap_buf[0].cb_ಬಫರ್   = sizes.cbSecurityTrailer;
  wrap_buf[1].ಬಫರ್_Type = SECBUFFER_ದತ್ತಾಂಶ;
  wrap_buf[1].pv_ಬಫರ್   = ಸಂದೇಶ_2;
  wrap_buf[1].cb_ಬಫರ್   = curlx_uztoul(messagelen);
  wrap_buf[2].ಬಫರ್_Type = SECBUFFER_PADDING;
  wrap_buf[2].pv_ಬಫರ್   = padding;
  wrap_buf[2].cb_ಬಫರ್   = sizes.cb_ಖಂಡ_ಗಾತ್ರ;

  /* Encrypt the data */
  ಸ್ಥಿತಿ_2 = Curl_pSecFn->Encrypt_ಸಂದೇಶ(krb5->context, KERB_WRAP_NO_ENCRYPT,
                                       &wrap_desc, 0);
  ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK) {
    ಆದರೆ(ಸ್ಥಿತಿ_2 == SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ)
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_AUTH_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Allocate the encryption (wrap) buffer */
  appdatalen = wrap_buf[0].cb_ಬಫರ್ + wrap_buf[1].cb_ಬಫರ್ +
               wrap_buf[2].cb_ಬಫರ್;
  appdata = curlx_malloc(appdatalen);
  ಆದರೆ(!appdata) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Populate the encryption buffer */
  ಸ್ಮರಣೆ_ನಕಲು(appdata, wrap_buf[0].pv_ಬಫರ್, wrap_buf[0].cb_ಬಫರ್);
  ಸ್ಥಾನಾಂತರ += wrap_buf[0].cb_ಬಫರ್;
  ಸ್ಮರಣೆ_ನಕಲು(appdata + ಸ್ಥಾನಾಂತರ, wrap_buf[1].pv_ಬಫರ್, wrap_buf[1].cb_ಬಫರ್);
  ಸ್ಥಾನಾಂತರ += wrap_buf[1].cb_ಬಫರ್;
  ಸ್ಮರಣೆ_ನಕಲು(appdata + ಸ್ಥಾನಾಂತರ, wrap_buf[2].pv_ಬಫರ್, wrap_buf[2].cb_ಬಫರ್);

ಹೊರಹೋಗು:
  /* Free all of our local buffers */
  curlx_free(padding);
  curlx_free(ಸಂದೇಶ_2);
  curlx_free(trailer);

  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Return the response. */
  Curl_bufref_ಗಣ(ಹೊರಹೋಗು, appdata, appdatalen, curl_free);
  ಹಿಂತಿರುಗಿ CURLE_OK;
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
ಶೂನ್ಯ Curl_auth_cleanup_gssapi(ರಚನೆ kerberos5data *krb5)
{
  /* Free our security context */
  ಆದರೆ(krb5->context) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);
    curlx_safefree(krb5->context);
  }

  /* Free our credentials handle */
  ಆದರೆ(krb5->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(krb5->credentials);
    curlx_safefree(krb5->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_ತಟಸ್ಥ(krb5->p_ತಟಸ್ಥ);
  krb5->p_ತಟಸ್ಥ = ಖಾಲಿ;

  /* Free the SPN and output token */
  curlx_safefree(krb5->spn);
  curlx_safefree(krb5->output_token);

  /* Reset any variables */
  krb5->token_max = 0;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_WINDOWS_SSPI && USE_KERBEROS5 */
