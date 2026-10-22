/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 * Copyright (C) Markus Moeller, <markus_moeller@compuserve.com>
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
 ***************************************************************************/
#समावेश "curl_setup.h"

#अगर defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_PROXY)

#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "strerror.h"
#समावेश "curlx/nonblock.h"
#समावेश "socks.h"
#समावेश "curl_sspi.h"
#समावेश "curlx/multibyte.h"

/*
 * Helper SSPI error functions.
 */
अटल पूर्णांक check_sspi_err(काष्ठा Curl_easy *तथ्य,
                          SECURITY_STATUS status,
                          स्थिर अक्षर *function)
{
  अगर(status != SEC_E_OK &&
     status != SEC_I_COMPLETE_AND_CONTINUE &&
     status != SEC_I_COMPLETE_NEEDED &&
     status != SEC_I_CONTINUE_NEEDED) {
    अक्षर बफर[STRERROR_LEN];
    failf(तथ्य, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(status, बफर, माप(बफर)));
    वापस 1;
  }
  वापस 0;
}

/* This is the SSPI-using version of this function */
अटल CURLcode socks5_sspi_setup(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा Curl_creds *creds,
                                  CredHandle *cred_handle,
                                  अक्षर **service_namep)
{
  काष्ठा connectdata *conn = cf->conn;
  स्थिर अक्षर *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  SECURITY_STATUS status;

  /* prepare service name */
  अगर(म_अक्षर(service, '/'))
    *service_namep = curlx_strdup(service);
  अन्यथा
    *service_namep = curl_maprintf("%s/%s",
                                   service, conn->socks_proxy.peer->hostname);
  अगर(!*service_namep)
    वापस CURLE_OUT_OF_MEMORY;

  status =
    Curl_pSecFn->AcquireCredentialsHandle(शून्य, CURL_UNCONST(TEXT("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          शून्य, शून्य, शून्य, शून्य,
                                          cred_handle, शून्य);

  अगर(check_sspi_err(तथ्य, status, "AcquireCredentialsHandle")) {
    failf(तथ्य, "Failed to acquire credentials.");
    वापस CURLE_COULDNT_CONNECT;
  }

  वापस CURLE_OK;
}

अटल CURLcode socks5_free_token(Sec_बफर *send_token,
                                  CURLcode परिणाम)
{
  अगर(send_token->pv_बफर) {
    Curl_pSecFn->Free_Context_बफर(send_token->pv_बफर);
    send_token->pv_बफर = शून्य;
  }
  वापस परिणाम;
}

अटल CURLcode socks5_sspi_loop(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 अक्षर *service_name,
                                 अचिन्हित दीर्घ *sspi_ret_flagsp)
{
  काष्ठा connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode परिणाम = CURLE_OK;
  CURLcode code;
  SECURITY_STATUS status;
  Sec_बफर sspi_send_token, sspi_recv_token;
  Sec_बफर_Desc निवेश_desc, output_desc;
  PCtxtHandle context_handle = शून्य;
  अचिन्हित लघु us_लंबाई;
  माप_प्रकार actualread;
  माप_प्रकार written;
  अचिन्हित अक्षर socksreq[4];

  निवेश_desc.cBuffers = 1;
  निवेश_desc.pBuffers = &sspi_recv_token;
  निवेश_desc.ulVersion = SECBUFFER_VERSION;

  sspi_recv_token.बफर_Type = SECBUFFER_TOKEN;
  sspi_recv_token.cb_बफर = 0;
  sspi_recv_token.pv_बफर = शून्य;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ulVersion = SECBUFFER_VERSION;

  sspi_send_token.बफर_Type = SECBUFFER_TOKEN;
  sspi_send_token.cb_बफर = 0;
  sspi_send_token.pv_बफर = शून्य;

  (व्योम)curlx_nonblock(sock, FALSE);

  क्रम(;;) {
    TCHAR *sname = curlx_convert_UTF8_to_tchar(service_name);
    अगर(!sname) {
      curlx_free(sspi_recv_token.pv_बफर);
      वापस socks5_free_token(&sspi_send_token, CURLE_OUT_OF_MEMORY);
    }

    status =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_MEMORY |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &निवेश_desc, 0,
                                             sspi_context,
                                             &output_desc,
                                             sspi_ret_flagsp, शून्य);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pv_बफर);
    sspi_recv_token.cb_बफर = 0;

    अगर(check_sspi_err(तथ्य, status, "InitializeSecurityContext")) {
      failf(तथ्य, "Failed to initialize security context.");
      वापस socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    अगर(sspi_send_token.cb_बफर) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      अगर(sspi_send_token.cb_बफर > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        वापस socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_लंबाई = htons((अचिन्हित लघु)sspi_send_token.cb_बफर);
      स_नकल(socksreq + 2, &us_लंबाई, माप(लघु));

      code = Curl_conn_cf_send(cf->अग्र, तथ्य, socksreq, 4, FALSE, &written);
      अगर(code || (written != 4)) {
        failf(तथ्य, "Failed to send SSPI authentication request.");
        वापस socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      code = Curl_conn_cf_send(cf->अग्र, तथ्य,
                               sspi_send_token.pv_बफर,
                               sspi_send_token.cb_बफर, FALSE, &written);
      अगर(code || (sspi_send_token.cb_बफर != written)) {
        failf(तथ्य, "Failed to send SSPI authentication token.");
        वापस socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
    }

    अगर(sspi_send_token.pv_बफर)
      socks5_free_token(&sspi_send_token, CURLE_OK);
    sspi_send_token.cb_बफर = 0;

    curlx_safefree(sspi_recv_token.pv_बफर);
    sspi_recv_token.cb_बफर = 0;

    अगर(status != SEC_I_CONTINUE_NEEDED)
      अवरोध;

    परिणाम = Curl_blockread_all(cf, तथ्य, (अक्षर *)socksreq, 4, &actualread);
    अगर(परिणाम || (actualread != 4)) {
      failf(तथ्य, "Failed to receive SSPI authentication response.");
      वापस परिणाम ? परिणाम : CURLE_COULDNT_CONNECT;
    }

    अगर(socksreq[1] == 255) {
      failf(तथ्य, "User was rejected by the SOCKS5 server (%u %u).",
            (अचिन्हित पूर्णांक)socksreq[0], (अचिन्हित पूर्णांक)socksreq[1]);
      वापस CURLE_COULDNT_CONNECT;
    }

    अगर(socksreq[1] != 1) {
      failf(तथ्य, "Invalid SSPI authentication response type (%u %u).",
            (अचिन्हित पूर्णांक)socksreq[0], (अचिन्हित पूर्णांक)socksreq[1]);
      वापस CURLE_COULDNT_CONNECT;
    }

    स_नकल(&us_लंबाई, socksreq + 2, माप(लघु));
    us_लंबाई = ntohs(us_लंबाई);

    अगर(!us_लंबाई) {
      failf(तथ्य, "Invalid zero-length SSPI authentication token.");
      वापस CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cb_बफर = us_लंबाई;
    sspi_recv_token.pv_बफर = curlx_malloc(us_लंबाई);

    अगर(!sspi_recv_token.pv_बफर)
      वापस CURLE_OUT_OF_MEMORY;

    परिणाम = Curl_blockread_all(cf, तथ्य, (अक्षर *)sspi_recv_token.pv_बफर,
                                sspi_recv_token.cb_बफर, &actualread);

    अगर(परिणाम || (actualread != us_लंबाई)) {
      failf(तथ्य, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pv_बफर);
      वापस परिणाम ? परिणाम : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  वापस CURLE_OK;
}

अटल CURLcode socks5_free(Sec_बफर *sspi_w_token,
                            CURLcode परिणाम)
{
  curlx_safefree(sspi_w_token[0].pv_बफर);
  curlx_safefree(sspi_w_token[1].pv_बफर);
  curlx_safefree(sspi_w_token[2].pv_बफर);
  वापस परिणाम;
}

अटल CURLcode socks5_sspi_encrypt(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    CtxtHandle *sspi_context,
                                    अचिन्हित दीर्घ sspi_ret_flags)
{
  CURLcode परिणाम = CURLE_OK;
  CURLcode code;
  SECURITY_STATUS status;
  अचिन्हित अक्षर gss_enc;
  Sec_बफर sspi_w_token[3];
  Sec_बफर_Desc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  अचिन्हित लघु us_लंबाई;
  अचिन्हित दीर्घ qop;
  अचिन्हित अक्षर socksreq[4];
  uint8_t *etbuf = शून्य;
  माप_प्रकार etbuf_आकार = 0;
  माप_प्रकार actualread;
  माप_प्रकार written;

  gss_enc = 0;
  अगर(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  अन्यथा अगर(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(तथ्य, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pv_बफर =
    sspi_w_token[1].pv_बफर =
    sspi_w_token[2].pv_बफर = शून्य;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ulVersion = SECBUFFER_VERSION;

  socksreq[0] = 1;
  socksreq[1] = 2;

  अगर(तथ्य->set.socks5_gssapi_nec) {
    us_लंबाई = htons((अचिन्हित लघु)1);
    स_नकल(socksreq + 2, &us_लंबाई, माप(लघु));
  }
  अन्यथा {
    status = Curl_pSecFn->QueryContextAttributes(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    अगर(check_sspi_err(तथ्य, status, "QueryContextAttributes")) {
      failf(तथ्य, "Failed to query security context attributes.");
      वापस CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cb_बफर = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].बफर_Type = SECBUFFER_TOKEN;
    sspi_w_token[0].pv_बफर = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    अगर(!sspi_w_token[0].pv_बफर)
      वापस CURLE_OUT_OF_MEMORY;

    sspi_w_token[1].cb_बफर = 1;
    sspi_w_token[1].बफर_Type = SECBUFFER_तथ्य;
    sspi_w_token[1].pv_बफर = curlx_malloc(1);
    अगर(!sspi_w_token[1].pv_बफर)
      वापस socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    स_नकल(sspi_w_token[1].pv_बफर, &gss_enc, 1);
    sspi_w_token[2].बफर_Type = SECBUFFER_PADDING;
    sspi_w_token[2].cb_बफर = sspi_sizes.cb_Block_आकार;
    sspi_w_token[2].pv_बफर = curlx_malloc(sspi_sizes.cb_Block_आकार);
    अगर(!sspi_w_token[2].pv_बफर)
      वापस socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    status = Curl_pSecFn->EncryptMessage(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    अगर(check_sspi_err(तथ्य, status, "EncryptMessage"))
      वापस socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_आकार = sspi_w_token[0].cb_बफर + sspi_w_token[1].cb_बफर +
      sspi_w_token[2].cb_बफर;
    अगर(etbuf_आकार > 0xffff)
      वापस socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_आकार);
    अगर(!etbuf)
      वापस socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    स_नकल(etbuf, sspi_w_token[0].pv_बफर, sspi_w_token[0].cb_बफर);
    स_नकल(etbuf + sspi_w_token[0].cb_बफर,
           sspi_w_token[1].pv_बफर, sspi_w_token[1].cb_बफर);
    स_नकल(etbuf + sspi_w_token[0].cb_बफर + sspi_w_token[1].cb_बफर,
           sspi_w_token[2].pv_बफर, sspi_w_token[2].cb_बफर);

    (व्योम)socks5_free(sspi_w_token, CURLE_OK);

    us_लंबाई = htons((अचिन्हित लघु)etbuf_आकार);
    स_नकल(socksreq + 2, &us_लंबाई, माप(लघु));
  }

  code = Curl_conn_cf_send(cf->अग्र, तथ्य, socksreq, 4, FALSE, &written);
  अगर(code || (written != 4)) {
    failf(तथ्य, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    वापस CURLE_COULDNT_CONNECT;
  }

  अगर(तथ्य->set.socks5_gssapi_nec) {
    स_नकल(socksreq, &gss_enc, 1);
    code = Curl_conn_cf_send(cf->अग्र, तथ्य, socksreq, 1, FALSE, &written);
    अगर(code || (written != 1)) {
      failf(तथ्य, "Failed to send SSPI encryption type.");
      वापस CURLE_COULDNT_CONNECT;
    }
  }
  अन्यथा {
    code = Curl_conn_cf_send(cf->अग्र, तथ्य, etbuf, etbuf_आकार, FALSE,
                             &written);
    curlx_free(etbuf);
    अगर(code || (etbuf_आकार != written)) {
      failf(तथ्य, "Failed to send SSPI encryption type.");
      वापस CURLE_COULDNT_CONNECT;
    }
  }

  परिणाम = Curl_blockread_all(cf, तथ्य, (अक्षर *)socksreq, 4, &actualread);
  अगर(परिणाम || (actualread != 4)) {
    failf(तथ्य, "Failed to receive SSPI encryption response.");
    वापस परिणाम ? परिणाम : CURLE_COULDNT_CONNECT;
  }

  अगर(socksreq[1] == 255) {
    failf(तथ्य, "User was rejected by the SOCKS5 server (%u %u).",
          (अचिन्हित पूर्णांक)socksreq[0], (अचिन्हित पूर्णांक)socksreq[1]);
    वापस CURLE_COULDNT_CONNECT;
  }

  अगर(socksreq[1] != 2) {
    failf(तथ्य, "Invalid SSPI encryption response type (%u %u).",
          (अचिन्हित पूर्णांक)socksreq[0], (अचिन्हित पूर्णांक)socksreq[1]);
    वापस CURLE_COULDNT_CONNECT;
  }

  स_नकल(&us_लंबाई, socksreq + 2, माप(लघु));
  us_लंबाई = ntohs(us_लंबाई);

  अगर(!us_लंबाई) {
    failf(तथ्य, "Invalid zero-length SSPI encryption token.");
    वापस CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cb_बफर = us_लंबाई;
  sspi_w_token[0].pv_बफर = curlx_malloc(us_लंबाई);
  अगर(!sspi_w_token[0].pv_बफर)
    वापस CURLE_OUT_OF_MEMORY;

  परिणाम = Curl_blockread_all(cf, तथ्य, (अक्षर *)sspi_w_token[0].pv_बफर,
                              sspi_w_token[0].cb_बफर, &actualread);

  अगर(परिणाम || (actualread != us_लंबाई)) {
    failf(तथ्य, "Failed to receive SSPI encryption type.");
    जाओ fail;
  }

  अगर(!तथ्य->set.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].बफर_Type = SECBUFFER_STREAM;
    sspi_w_token[1].बफर_Type = SECBUFFER_तथ्य;
    sspi_w_token[1].cb_बफर = 0;
    sspi_w_token[1].pv_बफर = शून्य;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    status = Curl_pSecFn->DecryptMessage(sspi_context, &wrap_desc, 0, &qop);

    अगर(check_sspi_err(तथ्य, status, "DecryptMessage"))
      जाओ fail;

    अगर(sspi_w_token[1].cb_बफर != 1) {
      failf(तथ्य, "Invalid SSPI encryption response length (%lu).",
            (अचिन्हित दीर्घ)sspi_w_token[1].cb_बफर);
      जाओ fail;
    }

    स_नकल(socksreq, sspi_w_token[1].pv_बफर, sspi_w_token[1].cb_बफर);
  }
  अन्यथा {
    अगर(sspi_w_token[0].cb_बफर != 1) {
      failf(तथ्य, "Invalid SSPI encryption response length (%lu).",
            (अचिन्हित दीर्घ)sspi_w_token[0].cb_बफर);
      जाओ fail;
    }
    स_नकल(socksreq, sspi_w_token[0].pv_बफर, sspi_w_token[0].cb_बफर);
  }
  curlx_free(sspi_w_token[0].pv_बफर);

  infof(तथ्य, "SOCKS5 access %s protection granted BUT NOT USED.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  cf->conn->socks5_gssapi_enctype = socksreq[0];
  वापस CURLE_OK;

fail:
  curlx_free(sspi_w_token[0].pv_बफर);
  वापस CURLE_COULDNT_CONNECT;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा Curl_creds *creds)
{
  काष्ठा connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode परिणाम;
  SECURITY_STATUS status;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  SecPkgCredentials_Names names;
  अक्षर *service_name = शून्य;
  अचिन्हित दीर्घ sspi_ret_flags = 0;

  स_रखो(&cred_handle, 0, माप(cred_handle));
  स_रखो(&sspi_context, 0, माप(sspi_context));
  names.sUserName = शून्य;

  परिणाम = socks5_sspi_setup(cf, तथ्य, creds, &cred_handle, &service_name);
  अगर(परिणाम)
    जाओ error;

  परिणाम = socks5_sspi_loop(cf, तथ्य, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  अगर(परिणाम)
    जाओ error;

  curlx_safefree(service_name);

  status = Curl_pSecFn->QueryCredentialsAttributes(&cred_handle,
                                                   SECPKG_CRED_ATTR_NAMES,
                                                   &names);
  अगर(check_sspi_err(तथ्य, status, "QueryCredentialAttributes")) {
    failf(तथ्य, "Failed to determine username.");
    परिणाम = CURLE_COULDNT_CONNECT;
    जाओ error;
  }
  अन्यथा {
    VERBOSE(अक्षर *user_utf8 = curlx_convert_tchar_to_UTF8(names.sUserName));
    infof(तथ्य, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->Free_Context_बफर(names.sUserName);
    names.sUserName = शून्य;
  }

  परिणाम = socks5_sspi_encrypt(cf, तथ्य, &sspi_context, sspi_ret_flags);
  अगर(परिणाम)
    जाओ error;

  (व्योम)curlx_nonblock(sock, TRUE);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  वापस CURLE_OK;

error:
  (व्योम)curlx_nonblock(sock, TRUE);
  curlx_free(service_name);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  अगर(names.sUserName)
    Curl_pSecFn->Free_Context_बफर(names.sUserName);
  वापस परिणाम;
}
#पूर्ण_अगर
