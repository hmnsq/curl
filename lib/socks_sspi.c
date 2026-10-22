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
ساكن صحيح check_sspi_err(بنية Curl_easy *انکڑے,
                          SECURITY_حالت حالت,
                          ثابت حرف *function)
{
  إذا(حالت != SEC_E_OK &&
     حالت != SEC_I_COMPLETE_AND_CONTINUE &&
     حالت != SEC_I_COMPLETE_NEEDED &&
     حالت != SEC_I_CONTINUE_NEEDED) {
    حرف بفر[STRERROR_LEN];
    failf(انکڑے, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(حالت, بفر, حجم(بفر)));
    أرجع 1;
  }
  أرجع 0;
}

/* This is the SSPI-using version of this function */
ساكن CURLcode socks5_sspi_تیاری(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
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
    أرجع CURLE_باہرلا_دا_حافظہ;

  حالت =
    Curl_pSecFn->AcquireCredentialsHandle(عدم, CURL_UNCONST(متن_2("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          عدم, عدم, عدم, عدم,
                                          cred_handle, عدم);

  إذا(check_sspi_err(انکڑے, حالت, "AcquireCredentialsHandle")) {
    failf(انکڑے, "Failed to acquire credentials.");
    أرجع CURLE_COULDNT_CONNECT;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode socks5_free_token(Sec_بفر *send_token,
                                  CURLcode نتیجہ)
{
  إذا(send_token->pv_بفر) {
    Curl_pSecFn->Free_Context_بفر(send_token->pv_بفر);
    send_token->pv_بفر = عدم;
  }
  أرجع نتیجہ;
}

ساكن CURLcode socks5_sspi_loop(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 حرف *service_name,
                                 بنانشانی لما *sspi_ret_flagsp)
{
  بنية connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode نتیجہ = CURLE_OK;
  CURLcode کوڈ;
  SECURITY_حالت حالت;
  Sec_بفر sspi_send_token, sspi_recv_token;
  Sec_بفر_Desc داخلہ_desc, output_desc;
  PCtxtHandle context_handle = عدم;
  بنانشانی چھوٹا us_لمائی;
  size_t actualread;
  size_t written;
  بنانشانی حرف socksreq[4];

  داخلہ_desc.cBuffers = 1;
  داخلہ_desc.pBuffers = &sspi_recv_token;
  داخلہ_desc.ul_نسخہ = SECBUFFER_نسخہ;

  sspi_recv_token.بفر_Type = SECBUFFER_TOKEN;
  sspi_recv_token.cb_بفر = 0;
  sspi_recv_token.pv_بفر = عدم;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ul_نسخہ = SECBUFFER_نسخہ;

  sspi_send_token.بفر_Type = SECBUFFER_TOKEN;
  sspi_send_token.cb_بفر = 0;
  sspi_send_token.pv_بفر = عدم;

  (فراغ)curlx_nonblock(sock, FALSE);

  لكل(;;) {
    TCHAR *sname = curlx_convert_UTF8_منزل_tchar(service_name);
    إذا(!sname) {
      curlx_free(sspi_recv_token.pv_بفر);
      أرجع socks5_free_token(&sspi_send_token, CURLE_باہرلا_دا_حافظہ);
    }

    حالت =
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
                                             sspi_ret_flagsp, عدم);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pv_بفر);
    sspi_recv_token.cb_بفر = 0;

    إذا(check_sspi_err(انکڑے, حالت, "InitializeSecurityContext")) {
      failf(انکڑے, "Failed to initialize security context.");
      أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    إذا(sspi_send_token.cb_بفر) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      إذا(sspi_send_token.cb_بفر > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_لمائی = htons((بنانشانی چھوٹا)sspi_send_token.cb_بفر);
      حافظہ_نقل(socksreq + 2, &us_لمائی, حجم(چھوٹا));

      کوڈ = Curl_conn_cf_send(cf->اگلا, انکڑے, socksreq, 4, FALSE, &written);
      إذا(کوڈ || (written != 4)) {
        failf(انکڑے, "Failed to send SSPI authentication request.");
        أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      کوڈ = Curl_conn_cf_send(cf->اگلا, انکڑے,
                               sspi_send_token.pv_بفر,
                               sspi_send_token.cb_بفر, FALSE, &written);
      إذا(کوڈ || (sspi_send_token.cb_بفر != written)) {
        failf(انکڑے, "Failed to send SSPI authentication token.");
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

    نتیجہ = Curl_blockread_all(cf, انکڑے, (حرف *)socksreq, 4, &actualread);
    إذا(نتیجہ || (actualread != 4)) {
      failf(انکڑے, "Failed to receive SSPI authentication response.");
      أرجع نتیجہ ? نتیجہ : CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] == 255) {
      failf(انکڑے, "User was rejected by the SOCKS5 server (%u %u).",
            (بنانشانی صحيح)socksreq[0], (بنانشانی صحيح)socksreq[1]);
      أرجع CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] != 1) {
      failf(انکڑے, "Invalid SSPI authentication response type (%u %u).",
            (بنانشانی صحيح)socksreq[0], (بنانشانی صحيح)socksreq[1]);
      أرجع CURLE_COULDNT_CONNECT;
    }

    حافظہ_نقل(&us_لمائی, socksreq + 2, حجم(چھوٹا));
    us_لمائی = ntohs(us_لمائی);

    إذا(!us_لمائی) {
      failf(انکڑے, "Invalid zero-length SSPI authentication token.");
      أرجع CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cb_بفر = us_لمائی;
    sspi_recv_token.pv_بفر = curlx_malloc(us_لمائی);

    إذا(!sspi_recv_token.pv_بفر)
      أرجع CURLE_باہرلا_دا_حافظہ;

    نتیجہ = Curl_blockread_all(cf, انکڑے, (حرف *)sspi_recv_token.pv_بفر,
                                sspi_recv_token.cb_بفر, &actualread);

    إذا(نتیجہ || (actualread != us_لمائی)) {
      failf(انکڑے, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pv_بفر);
      أرجع نتیجہ ? نتیجہ : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode socks5_free(Sec_بفر *sspi_w_token,
                            CURLcode نتیجہ)
{
  curlx_safefree(sspi_w_token[0].pv_بفر);
  curlx_safefree(sspi_w_token[1].pv_بفر);
  curlx_safefree(sspi_w_token[2].pv_بفر);
  أرجع نتیجہ;
}

ساكن CURLcode socks5_sspi_encrypt(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    CtxtHandle *sspi_context,
                                    بنانشانی لما sspi_ret_flags)
{
  CURLcode نتیجہ = CURLE_OK;
  CURLcode کوڈ;
  SECURITY_حالت حالت;
  بنانشانی حرف gss_enc;
  Sec_بفر sspi_w_token[3];
  Sec_بفر_Desc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  بنانشانی چھوٹا us_لمائی;
  بنانشانی لما qop;
  بنانشانی حرف socksreq[4];
  uint8_t *etbuf = عدم;
  size_t etbuf_ناپ = 0;
  size_t actualread;
  size_t written;

  gss_enc = 0;
  إذا(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  وإلا إذا(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(انکڑے, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pv_بفر =
    sspi_w_token[1].pv_بفر =
    sspi_w_token[2].pv_بفر = عدم;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ul_نسخہ = SECBUFFER_نسخہ;

  socksreq[0] = 1;
  socksreq[1] = 2;

  إذا(انکڑے->سیٹ.socks5_gssapi_nec) {
    us_لمائی = htons((بنانشانی چھوٹا)1);
    حافظہ_نقل(socksreq + 2, &us_لمائی, حجم(چھوٹا));
  }
  وإلا {
    حالت = Curl_pSecFn->Query_Context_گن(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    إذا(check_sspi_err(انکڑے, حالت, "QueryContextAttributes")) {
      failf(انکڑے, "Failed to query security context attributes.");
      أرجع CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cb_بفر = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].بفر_Type = SECBUFFER_TOKEN;
    sspi_w_token[0].pv_بفر = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    إذا(!sspi_w_token[0].pv_بفر)
      أرجع CURLE_باہرلا_دا_حافظہ;

    sspi_w_token[1].cb_بفر = 1;
    sspi_w_token[1].بفر_Type = SECBUFFER_انکڑے;
    sspi_w_token[1].pv_بفر = curlx_malloc(1);
    إذا(!sspi_w_token[1].pv_بفر)
      أرجع socks5_free(sspi_w_token, CURLE_باہرلا_دا_حافظہ);

    حافظہ_نقل(sspi_w_token[1].pv_بفر, &gss_enc, 1);
    sspi_w_token[2].بفر_Type = SECBUFFER_PADDING;
    sspi_w_token[2].cb_بفر = sspi_sizes.cb_بلاک_ناپ;
    sspi_w_token[2].pv_بفر = curlx_malloc(sspi_sizes.cb_بلاک_ناپ);
    إذا(!sspi_w_token[2].pv_بفر)
      أرجع socks5_free(sspi_w_token, CURLE_باہرلا_دا_حافظہ);

    حالت = Curl_pSecFn->Encrypt_سنیہا(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    إذا(check_sspi_err(انکڑے, حالت, "EncryptMessage"))
      أرجع socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_ناپ = sspi_w_token[0].cb_بفر + sspi_w_token[1].cb_بفر +
      sspi_w_token[2].cb_بفر;
    إذا(etbuf_ناپ > 0xffff)
      أرجع socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_ناپ);
    إذا(!etbuf)
      أرجع socks5_free(sspi_w_token, CURLE_باہرلا_دا_حافظہ);

    حافظہ_نقل(etbuf, sspi_w_token[0].pv_بفر, sspi_w_token[0].cb_بفر);
    حافظہ_نقل(etbuf + sspi_w_token[0].cb_بفر,
           sspi_w_token[1].pv_بفر, sspi_w_token[1].cb_بفر);
    حافظہ_نقل(etbuf + sspi_w_token[0].cb_بفر + sspi_w_token[1].cb_بفر,
           sspi_w_token[2].pv_بفر, sspi_w_token[2].cb_بفر);

    (فراغ)socks5_free(sspi_w_token, CURLE_OK);

    us_لمائی = htons((بنانشانی چھوٹا)etbuf_ناپ);
    حافظہ_نقل(socksreq + 2, &us_لمائی, حجم(چھوٹا));
  }

  کوڈ = Curl_conn_cf_send(cf->اگلا, انکڑے, socksreq, 4, FALSE, &written);
  إذا(کوڈ || (written != 4)) {
    failf(انکڑے, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(انکڑے->سیٹ.socks5_gssapi_nec) {
    حافظہ_نقل(socksreq, &gss_enc, 1);
    کوڈ = Curl_conn_cf_send(cf->اگلا, انکڑے, socksreq, 1, FALSE, &written);
    إذا(کوڈ || (written != 1)) {
      failf(انکڑے, "Failed to send SSPI encryption type.");
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  وإلا {
    کوڈ = Curl_conn_cf_send(cf->اگلا, انکڑے, etbuf, etbuf_ناپ, FALSE,
                             &written);
    curlx_free(etbuf);
    إذا(کوڈ || (etbuf_ناپ != written)) {
      failf(انکڑے, "Failed to send SSPI encryption type.");
      أرجع CURLE_COULDNT_CONNECT;
    }
  }

  نتیجہ = Curl_blockread_all(cf, انکڑے, (حرف *)socksreq, 4, &actualread);
  إذا(نتیجہ || (actualread != 4)) {
    failf(انکڑے, "Failed to receive SSPI encryption response.");
    أرجع نتیجہ ? نتیجہ : CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] == 255) {
    failf(انکڑے, "User was rejected by the SOCKS5 server (%u %u).",
          (بنانشانی صحيح)socksreq[0], (بنانشانی صحيح)socksreq[1]);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] != 2) {
    failf(انکڑے, "Invalid SSPI encryption response type (%u %u).",
          (بنانشانی صحيح)socksreq[0], (بنانشانی صحيح)socksreq[1]);
    أرجع CURLE_COULDNT_CONNECT;
  }

  حافظہ_نقل(&us_لمائی, socksreq + 2, حجم(چھوٹا));
  us_لمائی = ntohs(us_لمائی);

  إذا(!us_لمائی) {
    failf(انکڑے, "Invalid zero-length SSPI encryption token.");
    أرجع CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cb_بفر = us_لمائی;
  sspi_w_token[0].pv_بفر = curlx_malloc(us_لمائی);
  إذا(!sspi_w_token[0].pv_بفر)
    أرجع CURLE_باہرلا_دا_حافظہ;

  نتیجہ = Curl_blockread_all(cf, انکڑے, (حرف *)sspi_w_token[0].pv_بفر,
                              sspi_w_token[0].cb_بفر, &actualread);

  إذا(نتیجہ || (actualread != us_لمائی)) {
    failf(انکڑے, "Failed to receive SSPI encryption type.");
    اذهب fail;
  }

  إذا(!انکڑے->سیٹ.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].بفر_Type = SECBUFFER_دھارا;
    sspi_w_token[1].بفر_Type = SECBUFFER_انکڑے;
    sspi_w_token[1].cb_بفر = 0;
    sspi_w_token[1].pv_بفر = عدم;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    حالت = Curl_pSecFn->Decrypt_سنیہا(sspi_context, &wrap_desc, 0, &qop);

    إذا(check_sspi_err(انکڑے, حالت, "DecryptMessage"))
      اذهب fail;

    إذا(sspi_w_token[1].cb_بفر != 1) {
      failf(انکڑے, "Invalid SSPI encryption response length (%lu).",
            (بنانشانی لما)sspi_w_token[1].cb_بفر);
      اذهب fail;
    }

    حافظہ_نقل(socksreq, sspi_w_token[1].pv_بفر, sspi_w_token[1].cb_بفر);
  }
  وإلا {
    إذا(sspi_w_token[0].cb_بفر != 1) {
      failf(انکڑے, "Invalid SSPI encryption response length (%lu).",
            (بنانشانی لما)sspi_w_token[0].cb_بفر);
      اذهب fail;
    }
    حافظہ_نقل(socksreq, sspi_w_token[0].pv_بفر, sspi_w_token[0].cb_بفر);
  }
  curlx_free(sspi_w_token[0].pv_بفر);

  infof(انکڑے, "SOCKS5 access %s protection granted BUT NOT USED.",
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
                                      بنية Curl_easy *انکڑے,
                                      بنية Curl_creds *creds)
{
  بنية connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode نتیجہ;
  SECURITY_حالت حالت;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  Sec_Pkg_Credentials_ناں ناں;
  حرف *service_name = عدم;
  بنانشانی لما sspi_ret_flags = 0;

  memset(&cred_handle, 0, حجم(cred_handle));
  memset(&sspi_context, 0, حجم(sspi_context));
  ناں.sUserName = عدم;

  نتیجہ = socks5_sspi_تیاری(cf, انکڑے, creds, &cred_handle, &service_name);
  إذا(نتیجہ)
    اذهب error;

  نتیجہ = socks5_sspi_loop(cf, انکڑے, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  إذا(نتیجہ)
    اذهب error;

  curlx_safefree(service_name);

  حالت = Curl_pSecFn->Query_Credentials_گن(&cred_handle,
                                                   SECPKG_CRED_ATTR_ناں,
                                                   &ناں);
  إذا(check_sspi_err(انکڑے, حالت, "QueryCredentialAttributes")) {
    failf(انکڑے, "Failed to determine username.");
    نتیجہ = CURLE_COULDNT_CONNECT;
    اذهب error;
  }
  وإلا {
    VERBOSE(حرف *user_utf8 = curlx_convert_tchar_منزل_UTF8(ناں.sUserName));
    infof(انکڑے, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->Free_Context_بفر(ناں.sUserName);
    ناں.sUserName = عدم;
  }

  نتیجہ = socks5_sspi_encrypt(cf, انکڑے, &sspi_context, sspi_ret_flags);
  إذا(نتیجہ)
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
  إذا(ناں.sUserName)
    Curl_pSecFn->Free_Context_بفر(ناں.sUserName);
  أرجع نتیجہ;
}
#جے_انت
