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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(USE_WINDOWS_SSPI) && defined(USE_KERBEROS5)

#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "curl_trc.h"

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by Windows SSPI.
 */
bool Curl_auth_is_gssapi_supported(శూన్య)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_స్థితి స్థితి_2;

  /* Query the security package for Kerberos */
  స్థితి_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(పాఠ్యం_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  ఒకవేళ(స్థితి_2 == SEC_E_OK) {
    Curl_pSecFn->Free_Context_బఫరు(SecurityPackage);
  }

  తిరిగి స్థితి_2 == SEC_E_OK;
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
CURLcode Curl_auth_create_gssapi_user_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                              నిర్మాణం Curl_creds *creds,
                                              స్థిరరాశి అక్షరం *default_service,
                                              స్థిరరాశి అక్షరం *host,
                                              స్థిరరాశి bool mutual_auth,
                                              స్థిరరాశి నిర్మాణం bufref *chlg,
                                              నిర్మాణం kerberos5data *krb5,
                                              నిర్మాణం bufref *బహిర్గతం)
{
  CURLcode ఫలితం = CURLE_OK;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  Sec_బఫరు chlg_buf;
  Sec_బఫరు resp_buf;
  Sec_బఫరు_Desc chlg_desc;
  Sec_బఫరు_Desc resp_desc;
  SECURITY_స్థితి స్థితి_2;
  గుర్తులేని దీర్ఘ attrs;
  స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  ఒకవేళ(!krb5->spn) {
    /* Generate our SPN */
    krb5->spn = Curl_auth_build_spn(service, host, ఖాళీ);
    ఒకవేళ(!krb5->spn)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(!krb5->output_token) {
    /* Query the security package for Kerberos */
    స్థితి_2 = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(పాఠ్యం_2(SP_NAME_KERBEROS)),
                                 &SecurityPackage);
    ఒకవేళ(స్థితి_2 != SEC_E_OK) {
      failf(దత్తాంశం, "SSPI: could not get auth info");
      తిరిగి CURLE_AUTH_ERROR;
    }

    krb5->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_బఫరు(SecurityPackage);

    /* Allocate our response buffer */
    krb5->output_token = curlx_malloc(krb5->token_max);
    ఒకవేళ(!krb5->output_token)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(!krb5->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    ఒకవేళ(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      ఫలితం = Curl_create_sspi_తటస్థం(
        creds->user, creds->passwd, &krb5->తటస్థం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      /* Allow proper cleanup of the identity structure */
      krb5->p_తటస్థం = &krb5->తటస్థం;
    }
    లేకపోతే
      /* Use the current Windows user */
      krb5->p_తటస్థం = ఖాళీ;

    /* Allocate our credentials handle */
    krb5->credentials = curlx_calloc(1, పరిమాణం(CredHandle));
    ఒకవేళ(!krb5->credentials)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    /* Acquire our credentials handle */
    స్థితి_2 = Curl_pSecFn->AcquireCredentialsHandle(ఖాళీ,
                                 CURL_UNCONST(పాఠ్యం_2(SP_NAME_KERBEROS)),
                                 SECPKG_CRED_OUTBOUND, ఖాళీ,
                                 krb5->p_తటస్థం, ఖాళీ, ఖాళీ,
                                 krb5->credentials, ఖాళీ);
    ఒకవేళ(స్థితి_2 != SEC_E_OK) {
      curlx_safefree(krb5->credentials);
      తిరిగి CURLE_LOGIN_DENIED;
    }

    /* Allocate our new context handle */
    krb5->context = curlx_calloc(1, పరిమాణం(CtxtHandle));
    ఒకవేళ(!krb5->context)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(chlg) {
    ఒకవేళ(!Curl_bufref_len(chlg)) {
      infof(దత్తాంశం, "GSSAPI handshake failure (empty challenge message)");
      తిరిగి CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;
    chlg_desc.cBuffers  = 1;
    chlg_desc.pBuffers  = &chlg_buf;
    chlg_buf.బఫరు_Type = SECBUFFER_TOKEN;
    chlg_buf.pv_బఫరు   = CURL_UNCONST(Curl_bufref_ptr(chlg));
    chlg_buf.cb_బఫరు   = curlx_uztoul(Curl_bufref_len(chlg));
  }

  /* Setup the response "output" security buffer */
  resp_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.బఫరు_Type = SECBUFFER_TOKEN;
  resp_buf.pv_బఫరు   = krb5->output_token;
  resp_buf.cb_బఫరు   = curlx_uztoul(krb5->token_max);

  /* Generate our challenge-response message */
  స్థితి_2 = Curl_pSecFn->InitializeSecurityContext(krb5->credentials,
                                               chlg ? krb5->context : ఖాళీ,
                                               krb5->spn,
                                               (mutual_auth ?
                                                ISC_REQ_MUTUAL_AUTH : 0),
                                               0, SECURITY_NATIVE_DREP,
                                               chlg ? &chlg_desc : ఖాళీ, 0,
                                               &context,
                                               &resp_desc, &attrs, ఖాళీ);

  ఒకవేళ(స్థితి_2 == SEC_E_INSUFFICIENT_జ్ఞాపకం)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(స్థితి_2 != SEC_E_OK && స్థితి_2 != SEC_I_CONTINUE_NEEDED)
    తిరిగి CURLE_AUTH_ERROR;

  ఒకవేళ(memcmp(&context, krb5->context, పరిమాణం(context))) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);

    జ్ఞాపకం_నకలు(krb5->context, &context, పరిమాణం(context));
  }

  ఒకవేళ(resp_buf.cb_బఫరు) {
    ఫలితం = Curl_bufref_memdup0(బహిర్గతం, resp_buf.pv_బఫరు, resp_buf.cb_బఫరు);
  }
  లేకపోతే ఒకవేళ(mutual_auth)
    Curl_bufref_సమితి(బహిర్గతం, "", 0, ఖాళీ);
  లేకపోతే
    Curl_bufref_సమితి(బహిర్గతం, ఖాళీ, 0, ఖాళీ);

  తిరిగి ఫలితం;
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
CURLcode Curl_auth_create_gssapi_security_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                                  స్థిరరాశి అక్షరం *authzid,
                                                  స్థిరరాశి నిర్మాణం bufref *chlg,
                                                  నిర్మాణం kerberos5data *krb5,
                                                  నిర్మాణం bufref *బహిర్గతం)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t స్థానభ్రంశం = 0;
  size_t messagelen = 0;
  size_t appdatalen = 0;
  గుర్తులేని అక్షరం *trailer = ఖాళీ;
  గుర్తులేని అక్షరం *సందేశం_2 = ఖాళీ;
  గుర్తులేని అక్షరం *padding = ఖాళీ;
  గుర్తులేని అక్షరం *appdata = ఖాళీ;
  Sec_బఫరు ఆదానం_buf[2];
  Sec_బఫరు wrap_buf[3];
  Sec_బఫరు_Desc ఆదానం_desc;
  Sec_బఫరు_Desc wrap_desc;
  గుర్తులేని అక్షరం *indata;
  గుర్తులేని దీర్ఘ qop = 0;
  గుర్తులేని దీర్ఘ sec_layer = 0;
  గుర్తులేని దీర్ఘ max_కొలత = 0;
  SecPkgContext_Sizes sizes;
  SECURITY_స్థితి స్థితి_2;

  /* Ensure we have a valid challenge message */
  DEBUGASSERT(chlg);
  ఒకవేళ(!Curl_bufref_len(chlg)) {
    infof(దత్తాంశం, "GSSAPI handshake failure (empty security message)");
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }

  /* Get our response size information */
  స్థితి_2 = Curl_pSecFn->Query_Context_లక్షణాలు(krb5->context,
                                               SECPKG_ATTR_SIZES, &sizes);

  ఒకవేళ(స్థితి_2 == SEC_E_INSUFFICIENT_జ్ఞాపకం)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(స్థితి_2 != SEC_E_OK)
    తిరిగి CURLE_AUTH_ERROR;

  /* Setup the "input" security buffer */
  ఆదానం_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;
  ఆదానం_desc.cBuffers = 2;
  ఆదానం_desc.pBuffers = ఆదానం_buf;
  ఆదానం_buf[0].బఫరు_Type = SECBUFFER_ప్రవాహం;
  ఆదానం_buf[0].pv_బఫరు = CURL_UNCONST(Curl_bufref_ptr(chlg));
  ఆదానం_buf[0].cb_బఫరు = curlx_uztoul(Curl_bufref_len(chlg));
  ఆదానం_buf[1].బఫరు_Type = SECBUFFER_దత్తాంశం;
  ఆదానం_buf[1].pv_బఫరు = ఖాళీ;
  ఆదానం_buf[1].cb_బఫరు = 0;

  /* Decrypt the inbound challenge and obtain the qop. The encrypted message
     is decrypted in place, overwriting the original contents of its buffer.
     The SECBUFFER_DATA receives a pointer to the message in
     SECBUFFER_STREAM. */
  స్థితి_2 = Curl_pSecFn->Decrypt_సందేశం(krb5->context, &ఆదానం_desc, 0, &qop);
  ఒకవేళ(స్థితి_2 != SEC_E_OK) {
    infof(దత్తాంశం, "GSSAPI handshake failure (decryption failed)");
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  ఒకవేళ(ఆదానం_buf[1].cb_బఫరు != 4) {
    infof(దత్తాంశం, "GSSAPI handshake failure (invalid security data)");
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = ఆదానం_buf[1].pv_బఫరు;
  sec_layer = indata[0];
  max_కొలత = ((గుర్తులేని దీర్ఘ)indata[1] << 16) |
             ((గుర్తులేని దీర్ఘ)indata[2] << 8) | indata[3];

  /* Process the security layer */
  ఒకవేళ(!(sec_layer & KERB_WRAP_NO_ENCRYPT)) {
    infof(దత్తాంశం, "GSSAPI handshake failure (invalid security layer)");
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= KERB_WRAP_NO_ENCRYPT;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  ఒకవేళ(max_కొలత > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_కొలత = 0;
  }

  /* Allocate the trailer */
  trailer = curlx_malloc(sizes.cbSecurityTrailer);
  ఒకవేళ(!trailer)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Allocate our message */
  messagelen = 4;
  ఒకవేళ(authzid)
    messagelen += అక్షరశ్రేణి_పొడవు(authzid);
  సందేశం_2 = curlx_malloc(messagelen);
  ఒకవేళ(!సందేశం_2) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  /* Populate the message with the security layer and client supported receive
     message size. */
  సందేశం_2[0] = sec_layer & 0xFF;
  సందేశం_2[1] = (max_కొలత >> 16) & 0xFF;
  సందేశం_2[2] = (max_కొలత >> 8) & 0xFF;
  సందేశం_2[3] = max_కొలత & 0xFF;

  /* If given, append the authorization identity. */

  ఒకవేళ(authzid && *authzid)
    జ్ఞాపకం_నకలు(సందేశం_2 + 4, authzid, messagelen - 4);

  /* Allocate the padding */
  padding = curlx_malloc(sizes.cb_ఖండం_కొలత);
  ఒకవేళ(!padding) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  /* Setup the "authentication data" security buffer */
  wrap_desc.ul_సంస్కరణ    = SECBUFFER_సంస్కరణ;
  wrap_desc.cBuffers     = 3;
  wrap_desc.pBuffers     = wrap_buf;
  wrap_buf[0].బఫరు_Type = SECBUFFER_TOKEN;
  wrap_buf[0].pv_బఫరు   = trailer;
  wrap_buf[0].cb_బఫరు   = sizes.cbSecurityTrailer;
  wrap_buf[1].బఫరు_Type = SECBUFFER_దత్తాంశం;
  wrap_buf[1].pv_బఫరు   = సందేశం_2;
  wrap_buf[1].cb_బఫరు   = curlx_uztoul(messagelen);
  wrap_buf[2].బఫరు_Type = SECBUFFER_PADDING;
  wrap_buf[2].pv_బఫరు   = padding;
  wrap_buf[2].cb_బఫరు   = sizes.cb_ఖండం_కొలత;

  /* Encrypt the data */
  స్థితి_2 = Curl_pSecFn->Encrypt_సందేశం(krb5->context, KERB_WRAP_NO_ENCRYPT,
                                       &wrap_desc, 0);
  ఒకవేళ(స్థితి_2 != SEC_E_OK) {
    ఒకవేళ(స్థితి_2 == SEC_E_INSUFFICIENT_జ్ఞాపకం)
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    లేకపోతే
      ఫలితం = CURLE_AUTH_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  /* Allocate the encryption (wrap) buffer */
  appdatalen = wrap_buf[0].cb_బఫరు + wrap_buf[1].cb_బఫరు +
               wrap_buf[2].cb_బఫరు;
  appdata = curlx_malloc(appdatalen);
  ఒకవేళ(!appdata) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  /* Populate the encryption buffer */
  జ్ఞాపకం_నకలు(appdata, wrap_buf[0].pv_బఫరు, wrap_buf[0].cb_బఫరు);
  స్థానభ్రంశం += wrap_buf[0].cb_బఫరు;
  జ్ఞాపకం_నకలు(appdata + స్థానభ్రంశం, wrap_buf[1].pv_బఫరు, wrap_buf[1].cb_బఫరు);
  స్థానభ్రంశం += wrap_buf[1].cb_బఫరు;
  జ్ఞాపకం_నకలు(appdata + స్థానభ్రంశం, wrap_buf[2].pv_బఫరు, wrap_buf[2].cb_బఫరు);

బహిర్గతం:
  /* Free all of our local buffers */
  curlx_free(padding);
  curlx_free(సందేశం_2);
  curlx_free(trailer);

  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Return the response. */
  Curl_bufref_సమితి(బహిర్గతం, appdata, appdatalen, curl_free);
  తిరిగి CURLE_OK;
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
శూన్య Curl_auth_cleanup_gssapi(నిర్మాణం kerberos5data *krb5)
{
  /* Free our security context */
  ఒకవేళ(krb5->context) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);
    curlx_safefree(krb5->context);
  }

  /* Free our credentials handle */
  ఒకవేళ(krb5->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(krb5->credentials);
    curlx_safefree(krb5->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_తటస్థం(krb5->p_తటస్థం);
  krb5->p_తటస్థం = ఖాళీ;

  /* Free the SPN and output token */
  curlx_safefree(krb5->spn);
  curlx_safefree(krb5->output_token);

  /* Reset any variables */
  krb5->token_max = 0;
}

#అయితే_ముగింపు /* USE_WINDOWS_SSPI && USE_KERBEROS5 */
