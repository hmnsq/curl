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
#ضمن "curl_setup.h"

#إذا defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_PROXY)

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "strerror.h"
#ضمن "curlx/nonblock.h"
#ضمن "socks.h"
#ضمن "curl_sspi.h"
#ضمن "curlx/multibyte.h"

/*
 * Helper SSPI error functions.
 */
ساكن صحيح check_sspi_err(بنية Curl_easy *data,
                          SECURITY_STATUS status,
                          ثابت حرف *function)
{
  إذا(status != SEC_E_OK &&
     status != SEC_I_COMPLETE_AND_CONTINUE &&
     status != SEC_I_COMPLETE_NEEDED &&
     status != SEC_I_CONTINUE_NEEDED) {
    حرف buffer[STRERROR_LEN];
    failf(data, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(status, buffer, حجم(buffer)));
    إرجاع 1;
  }
  إرجاع 0;
}

/* This is the SSPI-using version of this function */
ساكن CURLcode socks5_sspi_setup(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  بنية Curl_creds *creds,
                                  CredHandle *cred_handle,
                                  حرف **service_namep)
{
  بنية connectdata *conn = cf->conn;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  SECURITY_STATUS status;

  /* prepare service name */
  إذا(strchr(service, '/'))
    *service_namep = curlx_strdup(service);
  وإلا
    *service_namep = curl_maprintf("%s/%s",
                                   service, conn->socks_proxy.peer->hostname);
  إذا(!*service_namep)
    إرجاع CURLE_OUT_OF_MEMORY;

  status =
    Curl_pSecFn->AcquireCredentialsHandle(عدم, CURL_UNCONST(TEXT("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          عدم, عدم, عدم, عدم,
                                          cred_handle, عدم);

  إذا(check_sspi_err(data, status, "AcquireCredentialsHandle")) {
    failf(data, "Failed to acquire credentials.");
    إرجاع CURLE_COULDNT_CONNECT;
  }

  إرجاع CURLE_OK;
}

ساكن CURLcode socks5_free_token(SecBuffer *send_token,
                                  CURLcode result)
{
  إذا(send_token->pvBuffer) {
    Curl_pSecFn->FreeContextBuffer(send_token->pvBuffer);
    send_token->pvBuffer = عدم;
  }
  إرجاع result;
}

ساكن CURLcode socks5_sspi_loop(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 حرف *service_name,
                                 unsigned long *sspi_ret_flagsp)
{
  بنية connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode result = CURLE_OK;
  CURLcode code;
  SECURITY_STATUS status;
  SecBuffer sspi_send_token, sspi_recv_token;
  SecBufferDesc إدخال_desc, output_desc;
  PCtxtHandle context_handle = عدم;
  unsigned short us_length;
  size_t actualread;
  size_t written;
  unsigned حرف socksreq[4];

  إدخال_desc.cBuffers = 1;
  إدخال_desc.pBuffers = &sspi_recv_token;
  إدخال_desc.ulVersion = SECBUFFER_VERSION;

  sspi_recv_token.BufferType = SECBUFFER_TOKEN;
  sspi_recv_token.cbBuffer = 0;
  sspi_recv_token.pvBuffer = عدم;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ulVersion = SECBUFFER_VERSION;

  sspi_send_token.BufferType = SECBUFFER_TOKEN;
  sspi_send_token.cbBuffer = 0;
  sspi_send_token.pvBuffer = عدم;

  (فراغ)curlx_nonblock(sock, FALSE);

  لكل(;;) {
    TCHAR *sname = curlx_convert_UTF8_to_tchar(service_name);
    إذا(!sname) {
      curlx_free(sspi_recv_token.pvBuffer);
      إرجاع socks5_free_token(&sspi_send_token, CURLE_OUT_OF_MEMORY);
    }

    status =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_MEMORY |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &إدخال_desc, 0,
                                             sspi_context,
                                             &output_desc,
                                             sspi_ret_flagsp, عدم);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pvBuffer);
    sspi_recv_token.cbBuffer = 0;

    إذا(check_sspi_err(data, status, "InitializeSecurityContext")) {
      failf(data, "Failed to initialize security context.");
      إرجاع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    إذا(sspi_send_token.cbBuffer) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      إذا(sspi_send_token.cbBuffer > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        إرجاع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_length = htons((unsigned short)sspi_send_token.cbBuffer);
      memcpy(socksreq + 2, &us_length, حجم(short));

      code = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE, &written);
      إذا(code || (written != 4)) {
        failf(data, "Failed to send SSPI authentication request.");
        إرجاع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      code = Curl_conn_cf_send(cf->next, data,
                               sspi_send_token.pvBuffer,
                               sspi_send_token.cbBuffer, FALSE, &written);
      إذا(code || (sspi_send_token.cbBuffer != written)) {
        failf(data, "Failed to send SSPI authentication token.");
        إرجاع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
    }

    إذا(sspi_send_token.pvBuffer)
      socks5_free_token(&sspi_send_token, CURLE_OK);
    sspi_send_token.cbBuffer = 0;

    curlx_safefree(sspi_recv_token.pvBuffer);
    sspi_recv_token.cbBuffer = 0;

    إذا(status != SEC_I_CONTINUE_NEEDED)
      اكسر;

    result = Curl_blockread_all(cf, data, (حرف *)socksreq, 4, &actualread);
    إذا(result || (actualread != 4)) {
      failf(data, "Failed to receive SSPI authentication response.");
      إرجاع result ? result : CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] == 255) {
      failf(data, "User was rejected by the SOCKS5 server (%u %u).",
            (unsigned صحيح)socksreq[0], (unsigned صحيح)socksreq[1]);
      إرجاع CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] != 1) {
      failf(data, "Invalid SSPI authentication response type (%u %u).",
            (unsigned صحيح)socksreq[0], (unsigned صحيح)socksreq[1]);
      إرجاع CURLE_COULDNT_CONNECT;
    }

    memcpy(&us_length, socksreq + 2, حجم(short));
    us_length = ntohs(us_length);

    إذا(!us_length) {
      failf(data, "Invalid zero-length SSPI authentication token.");
      إرجاع CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cbBuffer = us_length;
    sspi_recv_token.pvBuffer = curlx_malloc(us_length);

    إذا(!sspi_recv_token.pvBuffer)
      إرجاع CURLE_OUT_OF_MEMORY;

    result = Curl_blockread_all(cf, data, (حرف *)sspi_recv_token.pvBuffer,
                                sspi_recv_token.cbBuffer, &actualread);

    إذا(result || (actualread != us_length)) {
      failf(data, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pvBuffer);
      إرجاع result ? result : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  إرجاع CURLE_OK;
}

ساكن CURLcode socks5_free(SecBuffer *sspi_w_token,
                            CURLcode result)
{
  curlx_safefree(sspi_w_token[0].pvBuffer);
  curlx_safefree(sspi_w_token[1].pvBuffer);
  curlx_safefree(sspi_w_token[2].pvBuffer);
  إرجاع result;
}

ساكن CURLcode socks5_sspi_encrypt(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    CtxtHandle *sspi_context,
                                    unsigned long sspi_ret_flags)
{
  CURLcode result = CURLE_OK;
  CURLcode code;
  SECURITY_STATUS status;
  unsigned حرف gss_enc;
  SecBuffer sspi_w_token[3];
  SecBufferDesc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  unsigned short us_length;
  unsigned long qop;
  unsigned حرف socksreq[4];
  uint8_t *etbuf = عدم;
  size_t etbuf_size = 0;
  size_t actualread;
  size_t written;

  gss_enc = 0;
  إذا(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  وإلا إذا(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(data, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pvBuffer =
    sspi_w_token[1].pvBuffer =
    sspi_w_token[2].pvBuffer = عدم;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ulVersion = SECBUFFER_VERSION;

  socksreq[0] = 1;
  socksreq[1] = 2;

  إذا(data->set.socks5_gssapi_nec) {
    us_length = htons((unsigned short)1);
    memcpy(socksreq + 2, &us_length, حجم(short));
  }
  وإلا {
    status = Curl_pSecFn->QueryContextAttributes(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    إذا(check_sspi_err(data, status, "QueryContextAttributes")) {
      failf(data, "Failed to query security context attributes.");
      إرجاع CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cbBuffer = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].BufferType = SECBUFFER_TOKEN;
    sspi_w_token[0].pvBuffer = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    إذا(!sspi_w_token[0].pvBuffer)
      إرجاع CURLE_OUT_OF_MEMORY;

    sspi_w_token[1].cbBuffer = 1;
    sspi_w_token[1].BufferType = SECBUFFER_DATA;
    sspi_w_token[1].pvBuffer = curlx_malloc(1);
    إذا(!sspi_w_token[1].pvBuffer)
      إرجاع socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    memcpy(sspi_w_token[1].pvBuffer, &gss_enc, 1);
    sspi_w_token[2].BufferType = SECBUFFER_PADDING;
    sspi_w_token[2].cbBuffer = sspi_sizes.cbBlockSize;
    sspi_w_token[2].pvBuffer = curlx_malloc(sspi_sizes.cbBlockSize);
    إذا(!sspi_w_token[2].pvBuffer)
      إرجاع socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    status = Curl_pSecFn->EncryptMessage(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    إذا(check_sspi_err(data, status, "EncryptMessage"))
      إرجاع socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_size = sspi_w_token[0].cbBuffer + sspi_w_token[1].cbBuffer +
      sspi_w_token[2].cbBuffer;
    إذا(etbuf_size > 0xffff)
      إرجاع socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_size);
    إذا(!etbuf)
      إرجاع socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    memcpy(etbuf, sspi_w_token[0].pvBuffer, sspi_w_token[0].cbBuffer);
    memcpy(etbuf + sspi_w_token[0].cbBuffer,
           sspi_w_token[1].pvBuffer, sspi_w_token[1].cbBuffer);
    memcpy(etbuf + sspi_w_token[0].cbBuffer + sspi_w_token[1].cbBuffer,
           sspi_w_token[2].pvBuffer, sspi_w_token[2].cbBuffer);

    (فراغ)socks5_free(sspi_w_token, CURLE_OK);

    us_length = htons((unsigned short)etbuf_size);
    memcpy(socksreq + 2, &us_length, حجم(short));
  }

  code = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE, &written);
  إذا(code || (written != 4)) {
    failf(data, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    إرجاع CURLE_COULDNT_CONNECT;
  }

  إذا(data->set.socks5_gssapi_nec) {
    memcpy(socksreq, &gss_enc, 1);
    code = Curl_conn_cf_send(cf->next, data, socksreq, 1, FALSE, &written);
    إذا(code || (written != 1)) {
      failf(data, "Failed to send SSPI encryption type.");
      إرجاع CURLE_COULDNT_CONNECT;
    }
  }
  وإلا {
    code = Curl_conn_cf_send(cf->next, data, etbuf, etbuf_size, FALSE,
                             &written);
    curlx_free(etbuf);
    إذا(code || (etbuf_size != written)) {
      failf(data, "Failed to send SSPI encryption type.");
      إرجاع CURLE_COULDNT_CONNECT;
    }
  }

  result = Curl_blockread_all(cf, data, (حرف *)socksreq, 4, &actualread);
  إذا(result || (actualread != 4)) {
    failf(data, "Failed to receive SSPI encryption response.");
    إرجاع result ? result : CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] == 255) {
    failf(data, "User was rejected by the SOCKS5 server (%u %u).",
          (unsigned صحيح)socksreq[0], (unsigned صحيح)socksreq[1]);
    إرجاع CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] != 2) {
    failf(data, "Invalid SSPI encryption response type (%u %u).",
          (unsigned صحيح)socksreq[0], (unsigned صحيح)socksreq[1]);
    إرجاع CURLE_COULDNT_CONNECT;
  }

  memcpy(&us_length, socksreq + 2, حجم(short));
  us_length = ntohs(us_length);

  إذا(!us_length) {
    failf(data, "Invalid zero-length SSPI encryption token.");
    إرجاع CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cbBuffer = us_length;
  sspi_w_token[0].pvBuffer = curlx_malloc(us_length);
  إذا(!sspi_w_token[0].pvBuffer)
    إرجاع CURLE_OUT_OF_MEMORY;

  result = Curl_blockread_all(cf, data, (حرف *)sspi_w_token[0].pvBuffer,
                              sspi_w_token[0].cbBuffer, &actualread);

  إذا(result || (actualread != us_length)) {
    failf(data, "Failed to receive SSPI encryption type.");
    اذهب fail;
  }

  إذا(!data->set.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].BufferType = SECBUFFER_STREAM;
    sspi_w_token[1].BufferType = SECBUFFER_DATA;
    sspi_w_token[1].cbBuffer = 0;
    sspi_w_token[1].pvBuffer = عدم;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    status = Curl_pSecFn->DecryptMessage(sspi_context, &wrap_desc, 0, &qop);

    إذا(check_sspi_err(data, status, "DecryptMessage"))
      اذهب fail;

    إذا(sspi_w_token[1].cbBuffer != 1) {
      failf(data, "Invalid SSPI encryption response length (%lu).",
            (unsigned long)sspi_w_token[1].cbBuffer);
      اذهب fail;
    }

    memcpy(socksreq, sspi_w_token[1].pvBuffer, sspi_w_token[1].cbBuffer);
  }
  وإلا {
    إذا(sspi_w_token[0].cbBuffer != 1) {
      failf(data, "Invalid SSPI encryption response length (%lu).",
            (unsigned long)sspi_w_token[0].cbBuffer);
      اذهب fail;
    }
    memcpy(socksreq, sspi_w_token[0].pvBuffer, sspi_w_token[0].cbBuffer);
  }
  curlx_free(sspi_w_token[0].pvBuffer);

  infof(data, "SOCKS5 access %s protection granted BUT NOT USED.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  cf->conn->socks5_gssapi_enctype = socksreq[0];
  إرجاع CURLE_OK;

fail:
  curlx_free(sspi_w_token[0].pvBuffer);
  إرجاع CURLE_COULDNT_CONNECT;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      بنية Curl_creds *creds)
{
  بنية connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode result;
  SECURITY_STATUS status;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  SecPkgCredentials_Names names;
  حرف *service_name = عدم;
  unsigned long sspi_ret_flags = 0;

  memset(&cred_handle, 0, حجم(cred_handle));
  memset(&sspi_context, 0, حجم(sspi_context));
  names.sUserName = عدم;

  result = socks5_sspi_setup(cf, data, creds, &cred_handle, &service_name);
  إذا(result)
    اذهب error;

  result = socks5_sspi_loop(cf, data, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  إذا(result)
    اذهب error;

  curlx_safefree(service_name);

  status = Curl_pSecFn->QueryCredentialsAttributes(&cred_handle,
                                                   SECPKG_CRED_ATTR_NAMES,
                                                   &names);
  إذا(check_sspi_err(data, status, "QueryCredentialAttributes")) {
    failf(data, "Failed to determine username.");
    result = CURLE_COULDNT_CONNECT;
    اذهب error;
  }
  وإلا {
    VERBOSE(حرف *user_utf8 = curlx_convert_tchar_to_UTF8(names.sUserName));
    infof(data, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->FreeContextBuffer(names.sUserName);
    names.sUserName = عدم;
  }

  result = socks5_sspi_encrypt(cf, data, &sspi_context, sspi_ret_flags);
  إذا(result)
    اذهب error;

  (فراغ)curlx_nonblock(sock, TRUE);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  إرجاع CURLE_OK;

error:
  (فراغ)curlx_nonblock(sock, TRUE);
  curlx_free(service_name);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  إذا(names.sUserName)
    Curl_pSecFn->FreeContextBuffer(names.sUserName);
  إرجاع result;
}
#endif
