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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_PROXY)

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "strerror.h"
#ଅନ୍ତର୍ଭାଵ "curlx/nonblock.h"
#ଅନ୍ତର୍ଭାଵ "socks.h"
#ଅନ୍ତର୍ଭାଵ "curl_sspi.h"
#ଅନ୍ତର୍ଭାଵ "curlx/multibyte.h"

/*
 * Helper SSPI error functions.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_sspi_err(ସଂରଚନା Curl_easy *data,
                          SECURITY_STATUS status,
                          ଅଚଳ ଅକ୍ଷର *function)
{
  ଯଦି(status != SEC_E_OK &&
     status != SEC_I_COMPLETE_AND_CONTINUE &&
     status != SEC_I_COMPLETE_NEEDED &&
     status != SEC_I_CONTINUE_NEEDED) {
    ଅକ୍ଷର buffer[STRERROR_LEN];
    failf(data, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(status, buffer, ଆକାର(buffer)));
    ଫେରନ୍ତୁ 1;
  }
  ଫେରନ୍ତୁ 0;
}

/* This is the SSPI-using version of this function */
ସ୍ଥିର CURLcode socks5_sspi_setup(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା Curl_creds *creds,
                                  CredHandle *cred_handle,
                                  ଅକ୍ଷର **service_namep)
{
  ସଂରଚନା connectdata *conn = cf->conn;
  ଅଚଳ ଅକ୍ଷର *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  SECURITY_STATUS status;

  /* prepare service name */
  ଯଦି(strchr(service, '/'))
    *service_namep = curlx_strdup(service);
  ଅନ୍ୟଥା
    *service_namep = curl_maprintf("%s/%s",
                                   service, conn->socks_proxy.peer->hostname);
  ଯଦି(!*service_namep)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  status =
    Curl_pSecFn->AcquireCredentialsHandle(ଖାଲି, CURL_UNCONST(TEXT("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି,
                                          cred_handle, ଖାଲି);

  ଯଦି(check_sspi_err(data, status, "AcquireCredentialsHandle")) {
    failf(data, "Failed to acquire credentials.");
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode socks5_free_token(SecBuffer *send_token,
                                  CURLcode result)
{
  ଯଦି(send_token->pvBuffer) {
    Curl_pSecFn->FreeContextBuffer(send_token->pvBuffer);
    send_token->pvBuffer = ଖାଲି;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode socks5_sspi_loop(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 ଅକ୍ଷର *service_name,
                                 unsigned long *sspi_ret_flagsp)
{
  ସଂରଚନା connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode result = CURLE_OK;
  CURLcode code;
  SECURITY_STATUS status;
  SecBuffer sspi_send_token, sspi_recv_token;
  SecBufferDesc ନିବେଶ_desc, output_desc;
  PCtxtHandle context_handle = ଖାଲି;
  unsigned short us_length;
  size_t actualread;
  size_t written;
  unsigned ଅକ୍ଷର socksreq[4];

  ନିବେଶ_desc.cBuffers = 1;
  ନିବେଶ_desc.pBuffers = &sspi_recv_token;
  ନିବେଶ_desc.ulVersion = SECBUFFER_VERSION;

  sspi_recv_token.BufferType = SECBUFFER_TOKEN;
  sspi_recv_token.cbBuffer = 0;
  sspi_recv_token.pvBuffer = ଖାଲି;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ulVersion = SECBUFFER_VERSION;

  sspi_send_token.BufferType = SECBUFFER_TOKEN;
  sspi_send_token.cbBuffer = 0;
  sspi_send_token.pvBuffer = ଖାଲି;

  (ଶୂନ୍ୟ)curlx_nonblock(sock, FALSE);

  ପାଇଁ(;;) {
    TCHAR *sname = curlx_convert_UTF8_to_tchar(service_name);
    ଯଦି(!sname) {
      curlx_free(sspi_recv_token.pvBuffer);
      ଫେରନ୍ତୁ socks5_free_token(&sspi_send_token, CURLE_OUT_OF_MEMORY);
    }

    status =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_MEMORY |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &ନିବେଶ_desc, 0,
                                             sspi_context,
                                             &output_desc,
                                             sspi_ret_flagsp, ଖାଲି);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pvBuffer);
    sspi_recv_token.cbBuffer = 0;

    ଯଦି(check_sspi_err(data, status, "InitializeSecurityContext")) {
      failf(data, "Failed to initialize security context.");
      ଫେରନ୍ତୁ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    ଯଦି(sspi_send_token.cbBuffer) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      ଯଦି(sspi_send_token.cbBuffer > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        ଫେରନ୍ତୁ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_length = htons((unsigned short)sspi_send_token.cbBuffer);
      memcpy(socksreq + 2, &us_length, ଆକାର(short));

      code = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE, &written);
      ଯଦି(code || (written != 4)) {
        failf(data, "Failed to send SSPI authentication request.");
        ଫେରନ୍ତୁ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      code = Curl_conn_cf_send(cf->next, data,
                               sspi_send_token.pvBuffer,
                               sspi_send_token.cbBuffer, FALSE, &written);
      ଯଦି(code || (sspi_send_token.cbBuffer != written)) {
        failf(data, "Failed to send SSPI authentication token.");
        ଫେରନ୍ତୁ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
    }

    ଯଦି(sspi_send_token.pvBuffer)
      socks5_free_token(&sspi_send_token, CURLE_OK);
    sspi_send_token.cbBuffer = 0;

    curlx_safefree(sspi_recv_token.pvBuffer);
    sspi_recv_token.cbBuffer = 0;

    ଯଦି(status != SEC_I_CONTINUE_NEEDED)
      ଭାଙ୍ଗ;

    result = Curl_blockread_all(cf, data, (ଅକ୍ଷର *)socksreq, 4, &actualread);
    ଯଦି(result || (actualread != 4)) {
      failf(data, "Failed to receive SSPI authentication response.");
      ଫେରନ୍ତୁ result ? result : CURLE_COULDNT_CONNECT;
    }

    ଯଦି(socksreq[1] == 255) {
      failf(data, "User was rejected by the SOCKS5 server (%u %u).",
            (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)socksreq[0], (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)socksreq[1]);
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
    }

    ଯଦି(socksreq[1] != 1) {
      failf(data, "Invalid SSPI authentication response type (%u %u).",
            (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)socksreq[0], (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)socksreq[1]);
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
    }

    memcpy(&us_length, socksreq + 2, ଆକାର(short));
    us_length = ntohs(us_length);

    ଯଦି(!us_length) {
      failf(data, "Invalid zero-length SSPI authentication token.");
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cbBuffer = us_length;
    sspi_recv_token.pvBuffer = curlx_malloc(us_length);

    ଯଦି(!sspi_recv_token.pvBuffer)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    result = Curl_blockread_all(cf, data, (ଅକ୍ଷର *)sspi_recv_token.pvBuffer,
                                sspi_recv_token.cbBuffer, &actualread);

    ଯଦି(result || (actualread != us_length)) {
      failf(data, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pvBuffer);
      ଫେରନ୍ତୁ result ? result : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode socks5_free(SecBuffer *sspi_w_token,
                            CURLcode result)
{
  curlx_safefree(sspi_w_token[0].pvBuffer);
  curlx_safefree(sspi_w_token[1].pvBuffer);
  curlx_safefree(sspi_w_token[2].pvBuffer);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode socks5_sspi_encrypt(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    CtxtHandle *sspi_context,
                                    unsigned long sspi_ret_flags)
{
  CURLcode result = CURLE_OK;
  CURLcode code;
  SECURITY_STATUS status;
  unsigned ଅକ୍ଷର gss_enc;
  SecBuffer sspi_w_token[3];
  SecBufferDesc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  unsigned short us_length;
  unsigned long qop;
  unsigned ଅକ୍ଷର socksreq[4];
  uint8_t *etbuf = ଖାଲି;
  size_t etbuf_size = 0;
  size_t actualread;
  size_t written;

  gss_enc = 0;
  ଯଦି(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  ଅନ୍ୟଥା ଯଦି(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(data, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pvBuffer =
    sspi_w_token[1].pvBuffer =
    sspi_w_token[2].pvBuffer = ଖାଲି;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ulVersion = SECBUFFER_VERSION;

  socksreq[0] = 1;
  socksreq[1] = 2;

  ଯଦି(data->set.socks5_gssapi_nec) {
    us_length = htons((unsigned short)1);
    memcpy(socksreq + 2, &us_length, ଆକାର(short));
  }
  ଅନ୍ୟଥା {
    status = Curl_pSecFn->QueryContextAttributes(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    ଯଦି(check_sspi_err(data, status, "QueryContextAttributes")) {
      failf(data, "Failed to query security context attributes.");
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cbBuffer = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].BufferType = SECBUFFER_TOKEN;
    sspi_w_token[0].pvBuffer = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    ଯଦି(!sspi_w_token[0].pvBuffer)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    sspi_w_token[1].cbBuffer = 1;
    sspi_w_token[1].BufferType = SECBUFFER_DATA;
    sspi_w_token[1].pvBuffer = curlx_malloc(1);
    ଯଦି(!sspi_w_token[1].pvBuffer)
      ଫେରନ୍ତୁ socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    memcpy(sspi_w_token[1].pvBuffer, &gss_enc, 1);
    sspi_w_token[2].BufferType = SECBUFFER_PADDING;
    sspi_w_token[2].cbBuffer = sspi_sizes.cbBlockSize;
    sspi_w_token[2].pvBuffer = curlx_malloc(sspi_sizes.cbBlockSize);
    ଯଦି(!sspi_w_token[2].pvBuffer)
      ଫେରନ୍ତୁ socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    status = Curl_pSecFn->EncryptMessage(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    ଯଦି(check_sspi_err(data, status, "EncryptMessage"))
      ଫେରନ୍ତୁ socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_size = sspi_w_token[0].cbBuffer + sspi_w_token[1].cbBuffer +
      sspi_w_token[2].cbBuffer;
    ଯଦି(etbuf_size > 0xffff)
      ଫେରନ୍ତୁ socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_size);
    ଯଦି(!etbuf)
      ଫେରନ୍ତୁ socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    memcpy(etbuf, sspi_w_token[0].pvBuffer, sspi_w_token[0].cbBuffer);
    memcpy(etbuf + sspi_w_token[0].cbBuffer,
           sspi_w_token[1].pvBuffer, sspi_w_token[1].cbBuffer);
    memcpy(etbuf + sspi_w_token[0].cbBuffer + sspi_w_token[1].cbBuffer,
           sspi_w_token[2].pvBuffer, sspi_w_token[2].cbBuffer);

    (ଶୂନ୍ୟ)socks5_free(sspi_w_token, CURLE_OK);

    us_length = htons((unsigned short)etbuf_size);
    memcpy(socksreq + 2, &us_length, ଆକାର(short));
  }

  code = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE, &written);
  ଯଦି(code || (written != 4)) {
    failf(data, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }

  ଯଦି(data->set.socks5_gssapi_nec) {
    memcpy(socksreq, &gss_enc, 1);
    code = Curl_conn_cf_send(cf->next, data, socksreq, 1, FALSE, &written);
    ଯଦି(code || (written != 1)) {
      failf(data, "Failed to send SSPI encryption type.");
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
    }
  }
  ଅନ୍ୟଥା {
    code = Curl_conn_cf_send(cf->next, data, etbuf, etbuf_size, FALSE,
                             &written);
    curlx_free(etbuf);
    ଯଦି(code || (etbuf_size != written)) {
      failf(data, "Failed to send SSPI encryption type.");
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
    }
  }

  result = Curl_blockread_all(cf, data, (ଅକ୍ଷର *)socksreq, 4, &actualread);
  ଯଦି(result || (actualread != 4)) {
    failf(data, "Failed to receive SSPI encryption response.");
    ଫେରନ୍ତୁ result ? result : CURLE_COULDNT_CONNECT;
  }

  ଯଦି(socksreq[1] == 255) {
    failf(data, "User was rejected by the SOCKS5 server (%u %u).",
          (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)socksreq[0], (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)socksreq[1]);
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }

  ଯଦି(socksreq[1] != 2) {
    failf(data, "Invalid SSPI encryption response type (%u %u).",
          (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)socksreq[0], (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)socksreq[1]);
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }

  memcpy(&us_length, socksreq + 2, ଆକାର(short));
  us_length = ntohs(us_length);

  ଯଦି(!us_length) {
    failf(data, "Invalid zero-length SSPI encryption token.");
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cbBuffer = us_length;
  sspi_w_token[0].pvBuffer = curlx_malloc(us_length);
  ଯଦି(!sspi_w_token[0].pvBuffer)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  result = Curl_blockread_all(cf, data, (ଅକ୍ଷର *)sspi_w_token[0].pvBuffer,
                              sspi_w_token[0].cbBuffer, &actualread);

  ଯଦି(result || (actualread != us_length)) {
    failf(data, "Failed to receive SSPI encryption type.");
    ଯାଅ fail;
  }

  ଯଦି(!data->set.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].BufferType = SECBUFFER_STREAM;
    sspi_w_token[1].BufferType = SECBUFFER_DATA;
    sspi_w_token[1].cbBuffer = 0;
    sspi_w_token[1].pvBuffer = ଖାଲି;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    status = Curl_pSecFn->DecryptMessage(sspi_context, &wrap_desc, 0, &qop);

    ଯଦି(check_sspi_err(data, status, "DecryptMessage"))
      ଯାଅ fail;

    ଯଦି(sspi_w_token[1].cbBuffer != 1) {
      failf(data, "Invalid SSPI encryption response length (%lu).",
            (unsigned long)sspi_w_token[1].cbBuffer);
      ଯାଅ fail;
    }

    memcpy(socksreq, sspi_w_token[1].pvBuffer, sspi_w_token[1].cbBuffer);
  }
  ଅନ୍ୟଥା {
    ଯଦି(sspi_w_token[0].cbBuffer != 1) {
      failf(data, "Invalid SSPI encryption response length (%lu).",
            (unsigned long)sspi_w_token[0].cbBuffer);
      ଯାଅ fail;
    }
    memcpy(socksreq, sspi_w_token[0].pvBuffer, sspi_w_token[0].cbBuffer);
  }
  curlx_free(sspi_w_token[0].pvBuffer);

  infof(data, "SOCKS5 access %s protection granted BUT NOT USED.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  cf->conn->socks5_gssapi_enctype = socksreq[0];
  ଫେରନ୍ତୁ CURLE_OK;

fail:
  curlx_free(sspi_w_token[0].pvBuffer);
  ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା Curl_creds *creds)
{
  ସଂରଚନା connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode result;
  SECURITY_STATUS status;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  SecPkgCredentials_Names names;
  ଅକ୍ଷର *service_name = ଖାଲି;
  unsigned long sspi_ret_flags = 0;

  memset(&cred_handle, 0, ଆକାର(cred_handle));
  memset(&sspi_context, 0, ଆକାର(sspi_context));
  names.sUserName = ଖାଲି;

  result = socks5_sspi_setup(cf, data, creds, &cred_handle, &service_name);
  ଯଦି(result)
    ଯାଅ error;

  result = socks5_sspi_loop(cf, data, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  ଯଦି(result)
    ଯାଅ error;

  curlx_safefree(service_name);

  status = Curl_pSecFn->QueryCredentialsAttributes(&cred_handle,
                                                   SECPKG_CRED_ATTR_NAMES,
                                                   &names);
  ଯଦି(check_sspi_err(data, status, "QueryCredentialAttributes")) {
    failf(data, "Failed to determine username.");
    result = CURLE_COULDNT_CONNECT;
    ଯାଅ error;
  }
  ଅନ୍ୟଥା {
    VERBOSE(ଅକ୍ଷର *user_utf8 = curlx_convert_tchar_to_UTF8(names.sUserName));
    infof(data, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->FreeContextBuffer(names.sUserName);
    names.sUserName = ଖାଲି;
  }

  result = socks5_sspi_encrypt(cf, data, &sspi_context, sspi_ret_flags);
  ଯଦି(result)
    ଯାଅ error;

  (ଶୂନ୍ୟ)curlx_nonblock(sock, TRUE);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  ଫେରନ୍ତୁ CURLE_OK;

error:
  (ଶୂନ୍ୟ)curlx_nonblock(sock, TRUE);
  curlx_free(service_name);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  ଯଦି(names.sUserName)
    Curl_pSecFn->FreeContextBuffer(names.sUserName);
  ଫେରନ୍ତୁ result;
}
#endif
