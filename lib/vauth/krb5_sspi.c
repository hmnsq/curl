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
#अन्तर्भाव "curl_setup.h"

#यदि defined(USE_WINDOWS_SSPI) && defined(USE_KERBEROS5)

#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "curl_trc.h"

/*
 * Curl_auth_is_gssapi_supported()
 *
 * This is used to evaluate if GSSAPI (Kerberos V5) is supported.
 *
 * Parameters: None
 *
 * Returns TRUE if Kerberos V5 is supported by Windows SSPI.
 */
bool Curl_auth_is_gssapi_supported(शून्य)
{
  PSecPkgInfo SecurityPackage;
  SECURITY_स्थितिः स्थितिः;

  /* Query the security package for Kerberos */
  स्थितिः = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 &SecurityPackage);

  /* Release the package buffer as it is not required anymore */
  यदि(स्थितिः == SEC_E_OK) {
    Curl_pSecFn->Free_Context_बफर(SecurityPackage);
  }

  प्रत्यागम स्थितिः == SEC_E_OK;
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
CURLcode Curl_auth_create_gssapi_user_सन्देशः(संरचना Curl_easy *दत्त,
                                              संरचना Curl_creds *creds,
                                              ध्रुव वर्ण *default_service,
                                              ध्रुव वर्ण *host,
                                              ध्रुव bool mutual_auth,
                                              ध्रुव संरचना bufref *chlg,
                                              संरचना kerberos5data *krb5,
                                              संरचना bufref *बहिर्गमनम्)
{
  CURLcode परिणाम = CURLE_OK;
  CtxtHandle context;
  PSecPkgInfo SecurityPackage;
  Sec_बफर chlg_buf;
  Sec_बफर resp_buf;
  Sec_बफर_Desc chlg_desc;
  Sec_बफर_Desc resp_desc;
  SECURITY_स्थितिः स्थितिः;
  अचिह्नित दीर्घ attrs;
  ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;

  यदि(!krb5->spn) {
    /* Generate our SPN */
    krb5->spn = Curl_auth_build_spn(service, host, रिक्त);
    यदि(!krb5->spn)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  यदि(!krb5->output_token) {
    /* Query the security package for Kerberos */
    स्थितिः = Curl_pSecFn->QuerySecurityPackageInfo(
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 &SecurityPackage);
    यदि(स्थितिः != SEC_E_OK) {
      failf(दत्त, "SSPI: could not get auth info");
      प्रत्यागम CURLE_AUTH_ERROR;
    }

    krb5->token_max = SecurityPackage->cbMaxToken;

    /* Release the package buffer as it is not required anymore */
    Curl_pSecFn->Free_Context_बफर(SecurityPackage);

    /* Allocate our response buffer */
    krb5->output_token = curlx_malloc(krb5->token_max);
    यदि(!krb5->output_token)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  यदि(!krb5->credentials) {
    /* Do we have credentials to use or are we using single sign-on? */
    यदि(Curl_creds_has_user(creds)) {
      /* Populate our identity structure */
      परिणाम = Curl_create_sspi_तटस्थः(
        creds->user, creds->passwd, &krb5->तटस्थः);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      /* Allow proper cleanup of the identity structure */
      krb5->p_तटस्थः = &krb5->तटस्थः;
    }
    अन्यथा
      /* Use the current Windows user */
      krb5->p_तटस्थः = रिक्त;

    /* Allocate our credentials handle */
    krb5->credentials = curlx_calloc(1, परिमाण(CredHandle));
    यदि(!krb5->credentials)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    /* Acquire our credentials handle */
    स्थितिः = Curl_pSecFn->AcquireCredentialsHandle(रिक्त,
                                 CURL_UNCONST(TEXT(SP_NAME_KERBEROS)),
                                 SECPKG_CRED_OUTBOUND, रिक्त,
                                 krb5->p_तटस्थः, रिक्त, रिक्त,
                                 krb5->credentials, रिक्त);
    यदि(स्थितिः != SEC_E_OK) {
      curlx_safefree(krb5->credentials);
      प्रत्यागम CURLE_LOGIN_DENIED;
    }

    /* Allocate our new context handle */
    krb5->context = curlx_calloc(1, परिमाण(CtxtHandle));
    यदि(!krb5->context)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  यदि(chlg) {
    यदि(!Curl_bufref_len(chlg)) {
      infof(दत्त, "GSSAPI handshake failure (empty challenge message)");
      प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
    }

    /* Setup the challenge "input" security buffer */
    chlg_desc.ul_संस्करणम् = SECBUFFER_संस्करणम्;
    chlg_desc.cBuffers  = 1;
    chlg_desc.pBuffers  = &chlg_buf;
    chlg_buf.बफर_Type = SECBUFFER_TOKEN;
    chlg_buf.pv_बफर   = CURL_UNCONST(Curl_bufref_ptr(chlg));
    chlg_buf.cb_बफर   = curlx_uztoul(Curl_bufref_len(chlg));
  }

  /* Setup the response "output" security buffer */
  resp_desc.ul_संस्करणम् = SECBUFFER_संस्करणम्;
  resp_desc.cBuffers  = 1;
  resp_desc.pBuffers  = &resp_buf;
  resp_buf.बफर_Type = SECBUFFER_TOKEN;
  resp_buf.pv_बफर   = krb5->output_token;
  resp_buf.cb_बफर   = curlx_uztoul(krb5->token_max);

  /* Generate our challenge-response message */
  स्थितिः = Curl_pSecFn->InitializeSecurityContext(krb5->credentials,
                                               chlg ? krb5->context : रिक्त,
                                               krb5->spn,
                                               (mutual_auth ?
                                                ISC_REQ_MUTUAL_AUTH : 0),
                                               0, SECURITY_NATIVE_DREP,
                                               chlg ? &chlg_desc : रिक्त, 0,
                                               &context,
                                               &resp_desc, &attrs, रिक्त);

  यदि(स्थितिः == SEC_E_INSUFFICIENT_स्मृतिः)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(स्थितिः != SEC_E_OK && स्थितिः != SEC_I_CONTINUE_NEEDED)
    प्रत्यागम CURLE_AUTH_ERROR;

  यदि(memcmp(&context, krb5->context, परिमाण(context))) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);

    स्मृतिं_प्रतिलिख(krb5->context, &context, परिमाण(context));
  }

  यदि(resp_buf.cb_बफर) {
    परिणाम = Curl_bufref_memdup0(बहिर्गमनम्, resp_buf.pv_बफर, resp_buf.cb_बफर);
  }
  अन्यथा यदि(mutual_auth)
    Curl_bufref_समुच्चयः(बहिर्गमनम्, "", 0, रिक्त);
  अन्यथा
    Curl_bufref_समुच्चयः(बहिर्गमनम्, रिक्त, 0, रिक्त);

  प्रत्यागम परिणाम;
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
CURLcode Curl_auth_create_gssapi_security_सन्देशः(संरचना Curl_easy *दत्त,
                                                  ध्रुव वर्ण *authzid,
                                                  ध्रुव संरचना bufref *chlg,
                                                  संरचना kerberos5data *krb5,
                                                  संरचना bufref *बहिर्गमनम्)
{
  CURLcode परिणाम = CURLE_OK;
  size_t विस्थापनम् = 0;
  size_t messagelen = 0;
  size_t appdatalen = 0;
  अचिह्नित वर्ण *trailer = रिक्त;
  अचिह्नित वर्ण *सन्देशः_2 = रिक्त;
  अचिह्नित वर्ण *padding = रिक्त;
  अचिह्नित वर्ण *appdata = रिक्त;
  Sec_बफर निवेशः_buf[2];
  Sec_बफर wrap_buf[3];
  Sec_बफर_Desc निवेशः_desc;
  Sec_बफर_Desc wrap_desc;
  अचिह्नित वर्ण *indata;
  अचिह्नित दीर्घ qop = 0;
  अचिह्नित दीर्घ sec_layer = 0;
  अचिह्नित दीर्घ max_आकार = 0;
  SecPkgContext_Sizes sizes;
  SECURITY_स्थितिः स्थितिः;

  /* Ensure we have a valid challenge message */
  DEBUGASSERT(chlg);
  यदि(!Curl_bufref_len(chlg)) {
    infof(दत्त, "GSSAPI handshake failure (empty security message)");
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Get our response size information */
  स्थितिः = Curl_pSecFn->Query_Context_गुणाः(krb5->context,
                                               SECPKG_ATTR_SIZES, &sizes);

  यदि(स्थितिः == SEC_E_INSUFFICIENT_स्मृतिः)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(स्थितिः != SEC_E_OK)
    प्रत्यागम CURLE_AUTH_ERROR;

  /* Setup the "input" security buffer */
  निवेशः_desc.ul_संस्करणम् = SECBUFFER_संस्करणम्;
  निवेशः_desc.cBuffers = 2;
  निवेशः_desc.pBuffers = निवेशः_buf;
  निवेशः_buf[0].बफर_Type = SECBUFFER_प्रवाहः;
  निवेशः_buf[0].pv_बफर = CURL_UNCONST(Curl_bufref_ptr(chlg));
  निवेशः_buf[0].cb_बफर = curlx_uztoul(Curl_bufref_len(chlg));
  निवेशः_buf[1].बफर_Type = SECBUFFER_दत्त;
  निवेशः_buf[1].pv_बफर = रिक्त;
  निवेशः_buf[1].cb_बफर = 0;

  /* Decrypt the inbound challenge and obtain the qop. The encrypted message
     is decrypted in place, overwriting the original contents of its buffer.
     The SECBUFFER_DATA receives a pointer to the message in
     SECBUFFER_STREAM. */
  स्थितिः = Curl_pSecFn->Decrypt_सन्देशः(krb5->context, &निवेशः_desc, 0, &qop);
  यदि(स्थितिः != SEC_E_OK) {
    infof(दत्त, "GSSAPI handshake failure (decryption failed)");
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Not 4 octets long so fail as per RFC4752 Section 3.1 */
  यदि(निवेशः_buf[1].cb_बफर != 4) {
    infof(दत्त, "GSSAPI handshake failure (invalid security data)");
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Extract the security layer and the maximum message size */
  indata = निवेशः_buf[1].pv_बफर;
  sec_layer = indata[0];
  max_आकार = ((अचिह्नित दीर्घ)indata[1] << 16) |
             ((अचिह्नित दीर्घ)indata[2] << 8) | indata[3];

  /* Process the security layer */
  यदि(!(sec_layer & KERB_WRAP_NO_ENCRYPT)) {
    infof(दत्त, "GSSAPI handshake failure (invalid security layer)");
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }
  sec_layer &= KERB_WRAP_NO_ENCRYPT;  /* We do not support a security layer */

  /* Process the maximum message size the server can receive */
  यदि(max_आकार > 0) {
    /* The server has told us it supports a maximum receive buffer, but as we
       do not require one unless we are encrypting data, we tell the server
       our receive buffer is zero. */
    max_आकार = 0;
  }

  /* Allocate the trailer */
  trailer = curlx_malloc(sizes.cbSecurityTrailer);
  यदि(!trailer)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Allocate our message */
  messagelen = 4;
  यदि(authzid)
    messagelen += वाक्यदैर्घ्यम्(authzid);
  सन्देशः_2 = curlx_malloc(messagelen);
  यदि(!सन्देशः_2) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  /* Populate the message with the security layer and client supported receive
     message size. */
  सन्देशः_2[0] = sec_layer & 0xFF;
  सन्देशः_2[1] = (max_आकार >> 16) & 0xFF;
  सन्देशः_2[2] = (max_आकार >> 8) & 0xFF;
  सन्देशः_2[3] = max_आकार & 0xFF;

  /* If given, append the authorization identity. */

  यदि(authzid && *authzid)
    स्मृतिं_प्रतिलिख(सन्देशः_2 + 4, authzid, messagelen - 4);

  /* Allocate the padding */
  padding = curlx_malloc(sizes.cb_खण्डम्_आकार);
  यदि(!padding) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  /* Setup the "authentication data" security buffer */
  wrap_desc.ul_संस्करणम्    = SECBUFFER_संस्करणम्;
  wrap_desc.cBuffers     = 3;
  wrap_desc.pBuffers     = wrap_buf;
  wrap_buf[0].बफर_Type = SECBUFFER_TOKEN;
  wrap_buf[0].pv_बफर   = trailer;
  wrap_buf[0].cb_बफर   = sizes.cbSecurityTrailer;
  wrap_buf[1].बफर_Type = SECBUFFER_दत्त;
  wrap_buf[1].pv_बफर   = सन्देशः_2;
  wrap_buf[1].cb_बफर   = curlx_uztoul(messagelen);
  wrap_buf[2].बफर_Type = SECBUFFER_PADDING;
  wrap_buf[2].pv_बफर   = padding;
  wrap_buf[2].cb_बफर   = sizes.cb_खण्डम्_आकार;

  /* Encrypt the data */
  स्थितिः = Curl_pSecFn->Encrypt_सन्देशः(krb5->context, KERB_WRAP_NO_ENCRYPT,
                                       &wrap_desc, 0);
  यदि(स्थितिः != SEC_E_OK) {
    यदि(स्थितिः == SEC_E_INSUFFICIENT_स्मृतिः)
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    अन्यथा
      परिणाम = CURLE_AUTH_ERROR;
    गच्छ बहिर्गमनम्;
  }

  /* Allocate the encryption (wrap) buffer */
  appdatalen = wrap_buf[0].cb_बफर + wrap_buf[1].cb_बफर +
               wrap_buf[2].cb_बफर;
  appdata = curlx_malloc(appdatalen);
  यदि(!appdata) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  /* Populate the encryption buffer */
  स्मृतिं_प्रतिलिख(appdata, wrap_buf[0].pv_बफर, wrap_buf[0].cb_बफर);
  विस्थापनम् += wrap_buf[0].cb_बफर;
  स्मृतिं_प्रतिलिख(appdata + विस्थापनम्, wrap_buf[1].pv_बफर, wrap_buf[1].cb_बफर);
  विस्थापनम् += wrap_buf[1].cb_बफर;
  स्मृतिं_प्रतिलिख(appdata + विस्थापनम्, wrap_buf[2].pv_बफर, wrap_buf[2].cb_बफर);

बहिर्गमनम्:
  /* Free all of our local buffers */
  curlx_free(padding);
  curlx_free(सन्देशः_2);
  curlx_free(trailer);

  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Return the response. */
  Curl_bufref_समुच्चयः(बहिर्गमनम्, appdata, appdatalen, curl_free);
  प्रत्यागम CURLE_OK;
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
शून्य Curl_auth_cleanup_gssapi(संरचना kerberos5data *krb5)
{
  /* Free our security context */
  यदि(krb5->context) {
    Curl_pSecFn->DeleteSecurityContext(krb5->context);
    curlx_safefree(krb5->context);
  }

  /* Free our credentials handle */
  यदि(krb5->credentials) {
    Curl_pSecFn->FreeCredentialsHandle(krb5->credentials);
    curlx_safefree(krb5->credentials);
  }

  /* Free our identity */
  Curl_sspi_free_तटस्थः(krb5->p_तटस्थः);
  krb5->p_तटस्थः = रिक्त;

  /* Free the SPN and output token */
  curlx_safefree(krb5->spn);
  curlx_safefree(krb5->output_token);

  /* Reset any variables */
  krb5->token_max = 0;
}

#यदि_अन्तः /* USE_WINDOWS_SSPI && USE_KERBEROS5 */
