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
#شامل "curl_setup.h"

#اگر defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_PROXY)

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "strerror.h"
#شامل "curlx/nonblock.h"
#شامل "socks.h"
#شامل "curl_sspi.h"
#شامل "curlx/multibyte.h"

/*
 * Helper SSPI error functions.
 */
ساکن صحیح check_sspi_err(ساخت Curl_easy *ڈیٹا,
                          SECURITY_حالت حالت_2,
                          ثابت حرف *function)
{
  اگر(حالت_2 != SEC_E_OK &&
     حالت_2 != SEC_I_COMPLETE_AND_CONTINUE &&
     حالت_2 != SEC_I_COMPLETE_NEEDED &&
     حالت_2 != SEC_I_CONTINUE_NEEDED) {
    حرف بفر[STRERROR_LEN];
    failf(ڈیٹا, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(حالت_2, بفر, حجم(بفر)));
    واپس 1;
  }
  واپس 0;
}

/* This is the SSPI-using version of this function */
ساکن CURLcode socks5_sspi_تیاری(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت Curl_creds *creds,
                                  CredHandle *cred_handle,
                                  حرف **service_namep)
{
  ساخت connectdata *conn = cf->conn;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  SECURITY_حالت حالت_2;

  /* prepare service name */
  اگر(strchr(service, '/'))
    *service_namep = curlx_strdup(service);
  ورنہ
    *service_namep = curl_maprintf("%s/%s",
                                   service, conn->socks_proxy.peer->hostname);
  اگر(!*service_namep)
    واپس CURLE_برآمد_کا_حافظہ;

  حالت_2 =
    Curl_pSecFn->AcquireCredentialsHandle(تہی, CURL_UNCONST(متن_2("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          تہی, تہی, تہی, تہی,
                                          cred_handle, تہی);

  اگر(check_sspi_err(ڈیٹا, حالت_2, "AcquireCredentialsHandle")) {
    failf(ڈیٹا, "Failed to acquire credentials.");
    واپس CURLE_COULDNT_CONNECT;
  }

  واپس CURLE_OK;
}

ساکن CURLcode socks5_free_token(Sec_بفر *send_token,
                                  CURLcode نتیجہ)
{
  اگر(send_token->pv_بفر) {
    Curl_pSecFn->Free_Context_بفر(send_token->pv_بفر);
    send_token->pv_بفر = تہی;
  }
  واپس نتیجہ;
}

ساکن CURLcode socks5_sspi_loop(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 حرف *service_name,
                                 غیرعلامتی طویل *sspi_ret_flagsp)
{
  ساخت connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode نتیجہ = CURLE_OK;
  CURLcode رمز;
  SECURITY_حالت حالت_2;
  Sec_بفر sspi_send_token, sspi_recv_token;
  Sec_بفر_Desc داخلہ_desc, output_desc;
  PCtxtHandle context_handle = تہی;
  غیرعلامتی مختصر us_لمبائی;
  size_t actualread;
  size_t written;
  غیرعلامتی حرف socksreq[4];

  داخلہ_desc.cBuffers = 1;
  داخلہ_desc.pBuffers = &sspi_recv_token;
  داخلہ_desc.ul_نسخہ = SECBUFFER_نسخہ;

  sspi_recv_token.بفر_Type = SECBUFFER_TOKEN;
  sspi_recv_token.cb_بفر = 0;
  sspi_recv_token.pv_بفر = تہی;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ul_نسخہ = SECBUFFER_نسخہ;

  sspi_send_token.بفر_Type = SECBUFFER_TOKEN;
  sspi_send_token.cb_بفر = 0;
  sspi_send_token.pv_بفر = تہی;

  (خالی)curlx_nonblock(sock, FALSE);

  برائے(;;) {
    TCHAR *sname = curlx_convert_UTF8_مقصد_tchar(service_name);
    اگر(!sname) {
      curlx_free(sspi_recv_token.pv_بفر);
      واپس socks5_free_token(&sspi_send_token, CURLE_برآمد_کا_حافظہ);
    }

    حالت_2 =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_حافظہ |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &داخلہ_desc, 0,
                                             sspi_context,
                                             &output_desc,
                                             sspi_ret_flagsp, تہی);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pv_بفر);
    sspi_recv_token.cb_بفر = 0;

    اگر(check_sspi_err(ڈیٹا, حالت_2, "InitializeSecurityContext")) {
      failf(ڈیٹا, "Failed to initialize security context.");
      واپس socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    اگر(sspi_send_token.cb_بفر) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      اگر(sspi_send_token.cb_بفر > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        واپس socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_لمبائی = htons((غیرعلامتی مختصر)sspi_send_token.cb_بفر);
      حافظہ_نقل(socksreq + 2, &us_لمبائی, حجم(مختصر));

      رمز = Curl_conn_cf_send(cf->اگلا, ڈیٹا, socksreq, 4, FALSE, &written);
      اگر(رمز || (written != 4)) {
        failf(ڈیٹا, "Failed to send SSPI authentication request.");
        واپس socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      رمز = Curl_conn_cf_send(cf->اگلا, ڈیٹا,
                               sspi_send_token.pv_بفر,
                               sspi_send_token.cb_بفر, FALSE, &written);
      اگر(رمز || (sspi_send_token.cb_بفر != written)) {
        failf(ڈیٹا, "Failed to send SSPI authentication token.");
        واپس socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
    }

    اگر(sspi_send_token.pv_بفر)
      socks5_free_token(&sspi_send_token, CURLE_OK);
    sspi_send_token.cb_بفر = 0;

    curlx_safefree(sspi_recv_token.pv_بفر);
    sspi_recv_token.cb_بفر = 0;

    اگر(حالت_2 != SEC_I_CONTINUE_NEEDED)
      توڑو;

    نتیجہ = Curl_blockread_all(cf, ڈیٹا, (حرف *)socksreq, 4, &actualread);
    اگر(نتیجہ || (actualread != 4)) {
      failf(ڈیٹا, "Failed to receive SSPI authentication response.");
      واپس نتیجہ ? نتیجہ : CURLE_COULDNT_CONNECT;
    }

    اگر(socksreq[1] == 255) {
      failf(ڈیٹا, "User was rejected by the SOCKS5 server (%u %u).",
            (غیرعلامتی صحیح)socksreq[0], (غیرعلامتی صحیح)socksreq[1]);
      واپس CURLE_COULDNT_CONNECT;
    }

    اگر(socksreq[1] != 1) {
      failf(ڈیٹا, "Invalid SSPI authentication response type (%u %u).",
            (غیرعلامتی صحیح)socksreq[0], (غیرعلامتی صحیح)socksreq[1]);
      واپس CURLE_COULDNT_CONNECT;
    }

    حافظہ_نقل(&us_لمبائی, socksreq + 2, حجم(مختصر));
    us_لمبائی = ntohs(us_لمبائی);

    اگر(!us_لمبائی) {
      failf(ڈیٹا, "Invalid zero-length SSPI authentication token.");
      واپس CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cb_بفر = us_لمبائی;
    sspi_recv_token.pv_بفر = curlx_malloc(us_لمبائی);

    اگر(!sspi_recv_token.pv_بفر)
      واپس CURLE_برآمد_کا_حافظہ;

    نتیجہ = Curl_blockread_all(cf, ڈیٹا, (حرف *)sspi_recv_token.pv_بفر,
                                sspi_recv_token.cb_بفر, &actualread);

    اگر(نتیجہ || (actualread != us_لمبائی)) {
      failf(ڈیٹا, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pv_بفر);
      واپس نتیجہ ? نتیجہ : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  واپس CURLE_OK;
}

ساکن CURLcode socks5_free(Sec_بفر *sspi_w_token,
                            CURLcode نتیجہ)
{
  curlx_safefree(sspi_w_token[0].pv_بفر);
  curlx_safefree(sspi_w_token[1].pv_بفر);
  curlx_safefree(sspi_w_token[2].pv_بفر);
  واپس نتیجہ;
}

ساکن CURLcode socks5_sspi_encrypt(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    CtxtHandle *sspi_context,
                                    غیرعلامتی طویل sspi_ret_flags)
{
  CURLcode نتیجہ = CURLE_OK;
  CURLcode رمز;
  SECURITY_حالت حالت_2;
  غیرعلامتی حرف gss_enc;
  Sec_بفر sspi_w_token[3];
  Sec_بفر_Desc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  غیرعلامتی مختصر us_لمبائی;
  غیرعلامتی طویل qop;
  غیرعلامتی حرف socksreq[4];
  uint8_t *etbuf = تہی;
  size_t etbuf_سائز = 0;
  size_t actualread;
  size_t written;

  gss_enc = 0;
  اگر(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  ورنہ اگر(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(ڈیٹا, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pv_بفر =
    sspi_w_token[1].pv_بفر =
    sspi_w_token[2].pv_بفر = تہی;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ul_نسخہ = SECBUFFER_نسخہ;

  socksreq[0] = 1;
  socksreq[1] = 2;

  اگر(ڈیٹا->سیٹ.socks5_gssapi_nec) {
    us_لمبائی = htons((غیرعلامتی مختصر)1);
    حافظہ_نقل(socksreq + 2, &us_لمبائی, حجم(مختصر));
  }
  ورنہ {
    حالت_2 = Curl_pSecFn->Query_Context_اوصاف(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    اگر(check_sspi_err(ڈیٹا, حالت_2, "QueryContextAttributes")) {
      failf(ڈیٹا, "Failed to query security context attributes.");
      واپس CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cb_بفر = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].بفر_Type = SECBUFFER_TOKEN;
    sspi_w_token[0].pv_بفر = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    اگر(!sspi_w_token[0].pv_بفر)
      واپس CURLE_برآمد_کا_حافظہ;

    sspi_w_token[1].cb_بفر = 1;
    sspi_w_token[1].بفر_Type = SECBUFFER_ڈیٹا;
    sspi_w_token[1].pv_بفر = curlx_malloc(1);
    اگر(!sspi_w_token[1].pv_بفر)
      واپس socks5_free(sspi_w_token, CURLE_برآمد_کا_حافظہ);

    حافظہ_نقل(sspi_w_token[1].pv_بفر, &gss_enc, 1);
    sspi_w_token[2].بفر_Type = SECBUFFER_PADDING;
    sspi_w_token[2].cb_بفر = sspi_sizes.cb_بلاک_سائز;
    sspi_w_token[2].pv_بفر = curlx_malloc(sspi_sizes.cb_بلاک_سائز);
    اگر(!sspi_w_token[2].pv_بفر)
      واپس socks5_free(sspi_w_token, CURLE_برآمد_کا_حافظہ);

    حالت_2 = Curl_pSecFn->Encrypt_پیغام(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    اگر(check_sspi_err(ڈیٹا, حالت_2, "EncryptMessage"))
      واپس socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_سائز = sspi_w_token[0].cb_بفر + sspi_w_token[1].cb_بفر +
      sspi_w_token[2].cb_بفر;
    اگر(etbuf_سائز > 0xffff)
      واپس socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_سائز);
    اگر(!etbuf)
      واپس socks5_free(sspi_w_token, CURLE_برآمد_کا_حافظہ);

    حافظہ_نقل(etbuf, sspi_w_token[0].pv_بفر, sspi_w_token[0].cb_بفر);
    حافظہ_نقل(etbuf + sspi_w_token[0].cb_بفر,
           sspi_w_token[1].pv_بفر, sspi_w_token[1].cb_بفر);
    حافظہ_نقل(etbuf + sspi_w_token[0].cb_بفر + sspi_w_token[1].cb_بفر,
           sspi_w_token[2].pv_بفر, sspi_w_token[2].cb_بفر);

    (خالی)socks5_free(sspi_w_token, CURLE_OK);

    us_لمبائی = htons((غیرعلامتی مختصر)etbuf_سائز);
    حافظہ_نقل(socksreq + 2, &us_لمبائی, حجم(مختصر));
  }

  رمز = Curl_conn_cf_send(cf->اگلا, ڈیٹا, socksreq, 4, FALSE, &written);
  اگر(رمز || (written != 4)) {
    failf(ڈیٹا, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    واپس CURLE_COULDNT_CONNECT;
  }

  اگر(ڈیٹا->سیٹ.socks5_gssapi_nec) {
    حافظہ_نقل(socksreq, &gss_enc, 1);
    رمز = Curl_conn_cf_send(cf->اگلا, ڈیٹا, socksreq, 1, FALSE, &written);
    اگر(رمز || (written != 1)) {
      failf(ڈیٹا, "Failed to send SSPI encryption type.");
      واپس CURLE_COULDNT_CONNECT;
    }
  }
  ورنہ {
    رمز = Curl_conn_cf_send(cf->اگلا, ڈیٹا, etbuf, etbuf_سائز, FALSE,
                             &written);
    curlx_free(etbuf);
    اگر(رمز || (etbuf_سائز != written)) {
      failf(ڈیٹا, "Failed to send SSPI encryption type.");
      واپس CURLE_COULDNT_CONNECT;
    }
  }

  نتیجہ = Curl_blockread_all(cf, ڈیٹا, (حرف *)socksreq, 4, &actualread);
  اگر(نتیجہ || (actualread != 4)) {
    failf(ڈیٹا, "Failed to receive SSPI encryption response.");
    واپس نتیجہ ? نتیجہ : CURLE_COULDNT_CONNECT;
  }

  اگر(socksreq[1] == 255) {
    failf(ڈیٹا, "User was rejected by the SOCKS5 server (%u %u).",
          (غیرعلامتی صحیح)socksreq[0], (غیرعلامتی صحیح)socksreq[1]);
    واپس CURLE_COULDNT_CONNECT;
  }

  اگر(socksreq[1] != 2) {
    failf(ڈیٹا, "Invalid SSPI encryption response type (%u %u).",
          (غیرعلامتی صحیح)socksreq[0], (غیرعلامتی صحیح)socksreq[1]);
    واپس CURLE_COULDNT_CONNECT;
  }

  حافظہ_نقل(&us_لمبائی, socksreq + 2, حجم(مختصر));
  us_لمبائی = ntohs(us_لمبائی);

  اگر(!us_لمبائی) {
    failf(ڈیٹا, "Invalid zero-length SSPI encryption token.");
    واپس CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cb_بفر = us_لمبائی;
  sspi_w_token[0].pv_بفر = curlx_malloc(us_لمبائی);
  اگر(!sspi_w_token[0].pv_بفر)
    واپس CURLE_برآمد_کا_حافظہ;

  نتیجہ = Curl_blockread_all(cf, ڈیٹا, (حرف *)sspi_w_token[0].pv_بفر,
                              sspi_w_token[0].cb_بفر, &actualread);

  اگر(نتیجہ || (actualread != us_لمبائی)) {
    failf(ڈیٹا, "Failed to receive SSPI encryption type.");
    جاؤ fail;
  }

  اگر(!ڈیٹا->سیٹ.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].بفر_Type = SECBUFFER_دھارا;
    sspi_w_token[1].بفر_Type = SECBUFFER_ڈیٹا;
    sspi_w_token[1].cb_بفر = 0;
    sspi_w_token[1].pv_بفر = تہی;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    حالت_2 = Curl_pSecFn->Decrypt_پیغام(sspi_context, &wrap_desc, 0, &qop);

    اگر(check_sspi_err(ڈیٹا, حالت_2, "DecryptMessage"))
      جاؤ fail;

    اگر(sspi_w_token[1].cb_بفر != 1) {
      failf(ڈیٹا, "Invalid SSPI encryption response length (%lu).",
            (غیرعلامتی طویل)sspi_w_token[1].cb_بفر);
      جاؤ fail;
    }

    حافظہ_نقل(socksreq, sspi_w_token[1].pv_بفر, sspi_w_token[1].cb_بفر);
  }
  ورنہ {
    اگر(sspi_w_token[0].cb_بفر != 1) {
      failf(ڈیٹا, "Invalid SSPI encryption response length (%lu).",
            (غیرعلامتی طویل)sspi_w_token[0].cb_بفر);
      جاؤ fail;
    }
    حافظہ_نقل(socksreq, sspi_w_token[0].pv_بفر, sspi_w_token[0].cb_بفر);
  }
  curlx_free(sspi_w_token[0].pv_بفر);

  infof(ڈیٹا, "SOCKS5 access %s protection granted BUT NOT USED.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  cf->conn->socks5_gssapi_enctype = socksreq[0];
  واپس CURLE_OK;

fail:
  curlx_free(sspi_w_token[0].pv_بفر);
  واپس CURLE_COULDNT_CONNECT;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      ساخت Curl_creds *creds)
{
  ساخت connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode نتیجہ;
  SECURITY_حالت حالت_2;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  Sec_Pkg_Credentials_نام نام;
  حرف *service_name = تہی;
  غیرعلامتی طویل sspi_ret_flags = 0;

  memset(&cred_handle, 0, حجم(cred_handle));
  memset(&sspi_context, 0, حجم(sspi_context));
  نام.sUserName = تہی;

  نتیجہ = socks5_sspi_تیاری(cf, ڈیٹا, creds, &cred_handle, &service_name);
  اگر(نتیجہ)
    جاؤ error;

  نتیجہ = socks5_sspi_loop(cf, ڈیٹا, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  اگر(نتیجہ)
    جاؤ error;

  curlx_safefree(service_name);

  حالت_2 = Curl_pSecFn->Query_Credentials_اوصاف(&cred_handle,
                                                   SECPKG_CRED_ATTR_نام,
                                                   &نام);
  اگر(check_sspi_err(ڈیٹا, حالت_2, "QueryCredentialAttributes")) {
    failf(ڈیٹا, "Failed to determine username.");
    نتیجہ = CURLE_COULDNT_CONNECT;
    جاؤ error;
  }
  ورنہ {
    VERBOSE(حرف *user_utf8 = curlx_convert_tchar_مقصد_UTF8(نام.sUserName));
    infof(ڈیٹا, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->Free_Context_بفر(نام.sUserName);
    نام.sUserName = تہی;
  }

  نتیجہ = socks5_sspi_encrypt(cf, ڈیٹا, &sspi_context, sspi_ret_flags);
  اگر(نتیجہ)
    جاؤ error;

  (خالی)curlx_nonblock(sock, TRUE);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  واپس CURLE_OK;

error:
  (خالی)curlx_nonblock(sock, TRUE);
  curlx_free(service_name);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  اگر(نام.sUserName)
    Curl_pSecFn->Free_Context_بفر(نام.sUserName);
  واپس نتیجہ;
}
#اگر_ختم
