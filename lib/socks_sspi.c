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
ساكن صحيح check_sspi_err(بنية Curl_easy *معلومات,
                          SECURITY_حالت حالت,
                          ثابت حرف *function)
{
  إذا(حالت != SEC_E_OK &&
     حالت != SEC_I_COMPLETE_AND_CONTINUE &&
     حالت != SEC_I_COMPLETE_NEEDED &&
     حالت != SEC_I_CONTINUE_NEEDED) {
    حرف بفر[STRERROR_LEN];
    failf(معلومات, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(حالت, بفر, حجم(بفر)));
    أرجع 1;
  }
  أرجع 0;
}

/* This is the SSPI-using version of this function */
ساكن CURLcode socks5_sspi_تیاری(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  بنية Curl_creds *creds,
                                  CredHandle *cred_handle,
                                  حرف **service_namep)
{
  بنية connectdata *conn = cf->conn;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  SECURITY_حالت حالت;

  /* prepare service name */
  إذا(strchr(service, '/'))
    *service_namep = curlx_strdup(service);
  وإلا
    *service_namep = curl_maprintf("%s/%s",
                                   service, conn->socks_proxy.peer->hostname);
  إذا(!*service_namep)
    أرجع CURLE_بهر_د_حافظه;

  حالت =
    Curl_pSecFn->AcquireCredentialsHandle(عدم, CURL_UNCONST(متن_2("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          عدم, عدم, عدم, عدم,
                                          cred_handle, عدم);

  إذا(check_sspi_err(معلومات, حالت, "AcquireCredentialsHandle")) {
    failf(معلومات, "Failed to acquire credentials.");
    أرجع CURLE_COULDNT_CONNECT;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode socks5_free_token(Sec_بفر *send_token,
                                  CURLcode پایله)
{
  إذا(send_token->pv_بفر) {
    Curl_pSecFn->Free_Context_بفر(send_token->pv_بفر);
    send_token->pv_بفر = عدم;
  }
  أرجع پایله;
}

ساكن CURLcode socks5_sspi_loop(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 حرف *service_name,
                                 بینښه اوږد *sspi_ret_flagsp)
{
  بنية connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode پایله = CURLE_OK;
  CURLcode کوډ;
  SECURITY_حالت حالت;
  Sec_بفر sspi_send_token, sspi_recv_token;
  Sec_بفر_Desc ننوت_desc, output_desc;
  PCtxtHandle context_handle = عدم;
  بینښه لنډ us_اوږدتیا;
  size_t actualread;
  size_t written;
  بینښه حرف socksreq[4];

  ننوت_desc.cBuffers = 1;
  ننوت_desc.pBuffers = &sspi_recv_token;
  ننوت_desc.ul_نسخه = SECBUFFER_نسخه;

  sspi_recv_token.بفر_Type = SECBUFFER_TOKEN;
  sspi_recv_token.cb_بفر = 0;
  sspi_recv_token.pv_بفر = عدم;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ul_نسخه = SECBUFFER_نسخه;

  sspi_send_token.بفر_Type = SECBUFFER_TOKEN;
  sspi_send_token.cb_بفر = 0;
  sspi_send_token.pv_بفر = عدم;

  (فراغ)curlx_nonblock(sock, FALSE);

  لكل(;;) {
    TCHAR *sname = curlx_convert_UTF8_منزل_tchar(service_name);
    إذا(!sname) {
      curlx_free(sspi_recv_token.pv_بفر);
      أرجع socks5_free_token(&sspi_send_token, CURLE_بهر_د_حافظه);
    }

    حالت =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_حافظه |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &ننوت_desc, 0,
                                             sspi_context,
                                             &output_desc,
                                             sspi_ret_flagsp, عدم);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pv_بفر);
    sspi_recv_token.cb_بفر = 0;

    إذا(check_sspi_err(معلومات, حالت, "InitializeSecurityContext")) {
      failf(معلومات, "Failed to initialize security context.");
      أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    إذا(sspi_send_token.cb_بفر) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      إذا(sspi_send_token.cb_بفر > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_اوږدتیا = htons((بینښه لنډ)sspi_send_token.cb_بفر);
      حافظه_نقل(socksreq + 2, &us_اوږدتیا, حجم(لنډ));

      کوډ = Curl_conn_cf_send(cf->بل, معلومات, socksreq, 4, FALSE, &written);
      إذا(کوډ || (written != 4)) {
        failf(معلومات, "Failed to send SSPI authentication request.");
        أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      کوډ = Curl_conn_cf_send(cf->بل, معلومات,
                               sspi_send_token.pv_بفر,
                               sspi_send_token.cb_بفر, FALSE, &written);
      إذا(کوډ || (sspi_send_token.cb_بفر != written)) {
        failf(معلومات, "Failed to send SSPI authentication token.");
        أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
    }

    إذا(sspi_send_token.pv_بفر)
      socks5_free_token(&sspi_send_token, CURLE_OK);
    sspi_send_token.cb_بفر = 0;

    curlx_safefree(sspi_recv_token.pv_بفر);
    sspi_recv_token.cb_بفر = 0;

    إذا(حالت != SEC_I_CONTINUE_NEEDED)
      اكسر;

    پایله = Curl_blockread_all(cf, معلومات, (حرف *)socksreq, 4, &actualread);
    إذا(پایله || (actualread != 4)) {
      failf(معلومات, "Failed to receive SSPI authentication response.");
      أرجع پایله ? پایله : CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] == 255) {
      failf(معلومات, "User was rejected by the SOCKS5 server (%u %u).",
            (بینښه صحيح)socksreq[0], (بینښه صحيح)socksreq[1]);
      أرجع CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] != 1) {
      failf(معلومات, "Invalid SSPI authentication response type (%u %u).",
            (بینښه صحيح)socksreq[0], (بینښه صحيح)socksreq[1]);
      أرجع CURLE_COULDNT_CONNECT;
    }

    حافظه_نقل(&us_اوږدتیا, socksreq + 2, حجم(لنډ));
    us_اوږدتیا = ntohs(us_اوږدتیا);

    إذا(!us_اوږدتیا) {
      failf(معلومات, "Invalid zero-length SSPI authentication token.");
      أرجع CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cb_بفر = us_اوږدتیا;
    sspi_recv_token.pv_بفر = curlx_malloc(us_اوږدتیا);

    إذا(!sspi_recv_token.pv_بفر)
      أرجع CURLE_بهر_د_حافظه;

    پایله = Curl_blockread_all(cf, معلومات, (حرف *)sspi_recv_token.pv_بفر,
                                sspi_recv_token.cb_بفر, &actualread);

    إذا(پایله || (actualread != us_اوږدتیا)) {
      failf(معلومات, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pv_بفر);
      أرجع پایله ? پایله : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode socks5_free(Sec_بفر *sspi_w_token,
                            CURLcode پایله)
{
  curlx_safefree(sspi_w_token[0].pv_بفر);
  curlx_safefree(sspi_w_token[1].pv_بفر);
  curlx_safefree(sspi_w_token[2].pv_بفر);
  أرجع پایله;
}

ساكن CURLcode socks5_sspi_encrypt(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    CtxtHandle *sspi_context,
                                    بینښه اوږد sspi_ret_flags)
{
  CURLcode پایله = CURLE_OK;
  CURLcode کوډ;
  SECURITY_حالت حالت;
  بینښه حرف gss_enc;
  Sec_بفر sspi_w_token[3];
  Sec_بفر_Desc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  بینښه لنډ us_اوږدتیا;
  بینښه اوږد qop;
  بینښه حرف socksreq[4];
  uint8_t *etbuf = عدم;
  size_t etbuf_اندازه = 0;
  size_t actualread;
  size_t written;

  gss_enc = 0;
  إذا(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  وإلا إذا(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(معلومات, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pv_بفر =
    sspi_w_token[1].pv_بفر =
    sspi_w_token[2].pv_بفر = عدم;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ul_نسخه = SECBUFFER_نسخه;

  socksreq[0] = 1;
  socksreq[1] = 2;

  إذا(معلومات->سېټ.socks5_gssapi_nec) {
    us_اوږدتیا = htons((بینښه لنډ)1);
    حافظه_نقل(socksreq + 2, &us_اوږدتیا, حجم(لنډ));
  }
  وإلا {
    حالت = Curl_pSecFn->Query_Context_ځانګړتیاوې(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    إذا(check_sspi_err(معلومات, حالت, "QueryContextAttributes")) {
      failf(معلومات, "Failed to query security context attributes.");
      أرجع CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cb_بفر = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].بفر_Type = SECBUFFER_TOKEN;
    sspi_w_token[0].pv_بفر = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    إذا(!sspi_w_token[0].pv_بفر)
      أرجع CURLE_بهر_د_حافظه;

    sspi_w_token[1].cb_بفر = 1;
    sspi_w_token[1].بفر_Type = SECBUFFER_معلومات;
    sspi_w_token[1].pv_بفر = curlx_malloc(1);
    إذا(!sspi_w_token[1].pv_بفر)
      أرجع socks5_free(sspi_w_token, CURLE_بهر_د_حافظه);

    حافظه_نقل(sspi_w_token[1].pv_بفر, &gss_enc, 1);
    sspi_w_token[2].بفر_Type = SECBUFFER_PADDING;
    sspi_w_token[2].cb_بفر = sspi_sizes.cb_بلاک_اندازه;
    sspi_w_token[2].pv_بفر = curlx_malloc(sspi_sizes.cb_بلاک_اندازه);
    إذا(!sspi_w_token[2].pv_بفر)
      أرجع socks5_free(sspi_w_token, CURLE_بهر_د_حافظه);

    حالت = Curl_pSecFn->Encrypt_پیغام(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    إذا(check_sspi_err(معلومات, حالت, "EncryptMessage"))
      أرجع socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_اندازه = sspi_w_token[0].cb_بفر + sspi_w_token[1].cb_بفر +
      sspi_w_token[2].cb_بفر;
    إذا(etbuf_اندازه > 0xffff)
      أرجع socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_اندازه);
    إذا(!etbuf)
      أرجع socks5_free(sspi_w_token, CURLE_بهر_د_حافظه);

    حافظه_نقل(etbuf, sspi_w_token[0].pv_بفر, sspi_w_token[0].cb_بفر);
    حافظه_نقل(etbuf + sspi_w_token[0].cb_بفر,
           sspi_w_token[1].pv_بفر, sspi_w_token[1].cb_بفر);
    حافظه_نقل(etbuf + sspi_w_token[0].cb_بفر + sspi_w_token[1].cb_بفر,
           sspi_w_token[2].pv_بفر, sspi_w_token[2].cb_بفر);

    (فراغ)socks5_free(sspi_w_token, CURLE_OK);

    us_اوږدتیا = htons((بینښه لنډ)etbuf_اندازه);
    حافظه_نقل(socksreq + 2, &us_اوږدتیا, حجم(لنډ));
  }

  کوډ = Curl_conn_cf_send(cf->بل, معلومات, socksreq, 4, FALSE, &written);
  إذا(کوډ || (written != 4)) {
    failf(معلومات, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(معلومات->سېټ.socks5_gssapi_nec) {
    حافظه_نقل(socksreq, &gss_enc, 1);
    کوډ = Curl_conn_cf_send(cf->بل, معلومات, socksreq, 1, FALSE, &written);
    إذا(کوډ || (written != 1)) {
      failf(معلومات, "Failed to send SSPI encryption type.");
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  وإلا {
    کوډ = Curl_conn_cf_send(cf->بل, معلومات, etbuf, etbuf_اندازه, FALSE,
                             &written);
    curlx_free(etbuf);
    إذا(کوډ || (etbuf_اندازه != written)) {
      failf(معلومات, "Failed to send SSPI encryption type.");
      أرجع CURLE_COULDNT_CONNECT;
    }
  }

  پایله = Curl_blockread_all(cf, معلومات, (حرف *)socksreq, 4, &actualread);
  إذا(پایله || (actualread != 4)) {
    failf(معلومات, "Failed to receive SSPI encryption response.");
    أرجع پایله ? پایله : CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] == 255) {
    failf(معلومات, "User was rejected by the SOCKS5 server (%u %u).",
          (بینښه صحيح)socksreq[0], (بینښه صحيح)socksreq[1]);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] != 2) {
    failf(معلومات, "Invalid SSPI encryption response type (%u %u).",
          (بینښه صحيح)socksreq[0], (بینښه صحيح)socksreq[1]);
    أرجع CURLE_COULDNT_CONNECT;
  }

  حافظه_نقل(&us_اوږدتیا, socksreq + 2, حجم(لنډ));
  us_اوږدتیا = ntohs(us_اوږدتیا);

  إذا(!us_اوږدتیا) {
    failf(معلومات, "Invalid zero-length SSPI encryption token.");
    أرجع CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cb_بفر = us_اوږدتیا;
  sspi_w_token[0].pv_بفر = curlx_malloc(us_اوږدتیا);
  إذا(!sspi_w_token[0].pv_بفر)
    أرجع CURLE_بهر_د_حافظه;

  پایله = Curl_blockread_all(cf, معلومات, (حرف *)sspi_w_token[0].pv_بفر,
                              sspi_w_token[0].cb_بفر, &actualread);

  إذا(پایله || (actualread != us_اوږدتیا)) {
    failf(معلومات, "Failed to receive SSPI encryption type.");
    اذهب fail;
  }

  إذا(!معلومات->سېټ.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].بفر_Type = SECBUFFER_بهیر;
    sspi_w_token[1].بفر_Type = SECBUFFER_معلومات;
    sspi_w_token[1].cb_بفر = 0;
    sspi_w_token[1].pv_بفر = عدم;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    حالت = Curl_pSecFn->Decrypt_پیغام(sspi_context, &wrap_desc, 0, &qop);

    إذا(check_sspi_err(معلومات, حالت, "DecryptMessage"))
      اذهب fail;

    إذا(sspi_w_token[1].cb_بفر != 1) {
      failf(معلومات, "Invalid SSPI encryption response length (%lu).",
            (بینښه اوږد)sspi_w_token[1].cb_بفر);
      اذهب fail;
    }

    حافظه_نقل(socksreq, sspi_w_token[1].pv_بفر, sspi_w_token[1].cb_بفر);
  }
  وإلا {
    إذا(sspi_w_token[0].cb_بفر != 1) {
      failf(معلومات, "Invalid SSPI encryption response length (%lu).",
            (بینښه اوږد)sspi_w_token[0].cb_بفر);
      اذهب fail;
    }
    حافظه_نقل(socksreq, sspi_w_token[0].pv_بفر, sspi_w_token[0].cb_بفر);
  }
  curlx_free(sspi_w_token[0].pv_بفر);

  infof(معلومات, "SOCKS5 access %s protection granted BUT NOT USED.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  cf->conn->socks5_gssapi_enctype = socksreq[0];
  أرجع CURLE_OK;

fail:
  curlx_free(sspi_w_token[0].pv_بفر);
  أرجع CURLE_COULDNT_CONNECT;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      بنية Curl_creds *creds)
{
  بنية connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode پایله;
  SECURITY_حالت حالت;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  Sec_Pkg_Credentials_نومونه نومونه;
  حرف *service_name = عدم;
  بینښه اوږد sspi_ret_flags = 0;

  memset(&cred_handle, 0, حجم(cred_handle));
  memset(&sspi_context, 0, حجم(sspi_context));
  نومونه.sUserName = عدم;

  پایله = socks5_sspi_تیاری(cf, معلومات, creds, &cred_handle, &service_name);
  إذا(پایله)
    اذهب error;

  پایله = socks5_sspi_loop(cf, معلومات, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  إذا(پایله)
    اذهب error;

  curlx_safefree(service_name);

  حالت = Curl_pSecFn->Query_Credentials_ځانګړتیاوې(&cred_handle,
                                                   SECPKG_CRED_ATTR_نومونه,
                                                   &نومونه);
  إذا(check_sspi_err(معلومات, حالت, "QueryCredentialAttributes")) {
    failf(معلومات, "Failed to determine username.");
    پایله = CURLE_COULDNT_CONNECT;
    اذهب error;
  }
  وإلا {
    VERBOSE(حرف *user_utf8 = curlx_convert_tchar_منزل_UTF8(نومونه.sUserName));
    infof(معلومات, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->Free_Context_بفر(نومونه.sUserName);
    نومونه.sUserName = عدم;
  }

  پایله = socks5_sspi_encrypt(cf, معلومات, &sspi_context, sspi_ret_flags);
  إذا(پایله)
    اذهب error;

  (فراغ)curlx_nonblock(sock, TRUE);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  أرجع CURLE_OK;

error:
  (فراغ)curlx_nonblock(sock, TRUE);
  curlx_free(service_name);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  إذا(نومونه.sUserName)
    Curl_pSecFn->Free_Context_بفر(نومونه.sUserName);
  أرجع پایله;
}
#د_که_پای
