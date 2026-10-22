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
ساكن CURLcode socks5_sspi_تياري(بنية Curl_cfilter *cf,
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
    أرجع CURLE_ٻاهريون_جو_يادگيري;

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
                                  CURLcode نتيجو)
{
  إذا(send_token->pv_بفر) {
    Curl_pSecFn->Free_Context_بفر(send_token->pv_بفر);
    send_token->pv_بفر = عدم;
  }
  أرجع نتيجو;
}

ساكن CURLcode socks5_sspi_loop(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 حرف *service_name,
                                 بنانشان ڊگهو *sspi_ret_flagsp)
{
  بنية connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode نتيجو = CURLE_OK;
  CURLcode ڪوڊ;
  SECURITY_حالت حالت;
  Sec_بفر sspi_send_token, sspi_recv_token;
  Sec_بفر_Desc داخلا_desc, output_desc;
  PCtxtHandle context_handle = عدم;
  بنانشان ننڍو us_ڊيگهائي;
  size_t actualread;
  size_t written;
  بنانشان حرف socksreq[4];

  داخلا_desc.cBuffers = 1;
  داخلا_desc.pBuffers = &sspi_recv_token;
  داخلا_desc.ul_نسخو = SECBUFFER_نسخو;

  sspi_recv_token.بفر_Type = SECBUFFER_TOKEN;
  sspi_recv_token.cb_بفر = 0;
  sspi_recv_token.pv_بفر = عدم;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ul_نسخو = SECBUFFER_نسخو;

  sspi_send_token.بفر_Type = SECBUFFER_TOKEN;
  sspi_send_token.cb_بفر = 0;
  sspi_send_token.pv_بفر = عدم;

  (فراغ)curlx_nonblock(sock, FALSE);

  لكل(;;) {
    TCHAR *sname = curlx_convert_UTF8_منزل_tchar(service_name);
    إذا(!sname) {
      curlx_free(sspi_recv_token.pv_بفر);
      أرجع socks5_free_token(&sspi_send_token, CURLE_ٻاهريون_جو_يادگيري);
    }

    حالت =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_يادگيري |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &داخلا_desc, 0,
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
      us_ڊيگهائي = htons((بنانشان ننڍو)sspi_send_token.cb_بفر);
      يادگيري_نقل(socksreq + 2, &us_ڊيگهائي, حجم(ننڍو));

      ڪوڊ = Curl_conn_cf_send(cf->ايندڙ, معلومات, socksreq, 4, FALSE, &written);
      إذا(ڪوڊ || (written != 4)) {
        failf(معلومات, "Failed to send SSPI authentication request.");
        أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      ڪوڊ = Curl_conn_cf_send(cf->ايندڙ, معلومات,
                               sspi_send_token.pv_بفر,
                               sspi_send_token.cb_بفر, FALSE, &written);
      إذا(ڪوڊ || (sspi_send_token.cb_بفر != written)) {
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

    نتيجو = Curl_blockread_all(cf, معلومات, (حرف *)socksreq, 4, &actualread);
    إذا(نتيجو || (actualread != 4)) {
      failf(معلومات, "Failed to receive SSPI authentication response.");
      أرجع نتيجو ? نتيجو : CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] == 255) {
      failf(معلومات, "User was rejected by the SOCKS5 server (%u %u).",
            (بنانشان صحيح)socksreq[0], (بنانشان صحيح)socksreq[1]);
      أرجع CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] != 1) {
      failf(معلومات, "Invalid SSPI authentication response type (%u %u).",
            (بنانشان صحيح)socksreq[0], (بنانشان صحيح)socksreq[1]);
      أرجع CURLE_COULDNT_CONNECT;
    }

    يادگيري_نقل(&us_ڊيگهائي, socksreq + 2, حجم(ننڍو));
    us_ڊيگهائي = ntohs(us_ڊيگهائي);

    إذا(!us_ڊيگهائي) {
      failf(معلومات, "Invalid zero-length SSPI authentication token.");
      أرجع CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cb_بفر = us_ڊيگهائي;
    sspi_recv_token.pv_بفر = curlx_malloc(us_ڊيگهائي);

    إذا(!sspi_recv_token.pv_بفر)
      أرجع CURLE_ٻاهريون_جو_يادگيري;

    نتيجو = Curl_blockread_all(cf, معلومات, (حرف *)sspi_recv_token.pv_بفر,
                                sspi_recv_token.cb_بفر, &actualread);

    إذا(نتيجو || (actualread != us_ڊيگهائي)) {
      failf(معلومات, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pv_بفر);
      أرجع نتيجو ? نتيجو : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode socks5_free(Sec_بفر *sspi_w_token,
                            CURLcode نتيجو)
{
  curlx_safefree(sspi_w_token[0].pv_بفر);
  curlx_safefree(sspi_w_token[1].pv_بفر);
  curlx_safefree(sspi_w_token[2].pv_بفر);
  أرجع نتيجو;
}

ساكن CURLcode socks5_sspi_encrypt(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    CtxtHandle *sspi_context,
                                    بنانشان ڊگهو sspi_ret_flags)
{
  CURLcode نتيجو = CURLE_OK;
  CURLcode ڪوڊ;
  SECURITY_حالت حالت;
  بنانشان حرف gss_enc;
  Sec_بفر sspi_w_token[3];
  Sec_بفر_Desc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  بنانشان ننڍو us_ڊيگهائي;
  بنانشان ڊگهو qop;
  بنانشان حرف socksreq[4];
  uint8_t *etbuf = عدم;
  size_t etbuf_سائيز = 0;
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
  wrap_desc.ul_نسخو = SECBUFFER_نسخو;

  socksreq[0] = 1;
  socksreq[1] = 2;

  إذا(معلومات->سيٽ.socks5_gssapi_nec) {
    us_ڊيگهائي = htons((بنانشان ننڍو)1);
    يادگيري_نقل(socksreq + 2, &us_ڊيگهائي, حجم(ننڍو));
  }
  وإلا {
    حالت = Curl_pSecFn->Query_Context_خاصيتون(sspi_context,
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
      أرجع CURLE_ٻاهريون_جو_يادگيري;

    sspi_w_token[1].cb_بفر = 1;
    sspi_w_token[1].بفر_Type = SECBUFFER_معلومات;
    sspi_w_token[1].pv_بفر = curlx_malloc(1);
    إذا(!sspi_w_token[1].pv_بفر)
      أرجع socks5_free(sspi_w_token, CURLE_ٻاهريون_جو_يادگيري);

    يادگيري_نقل(sspi_w_token[1].pv_بفر, &gss_enc, 1);
    sspi_w_token[2].بفر_Type = SECBUFFER_PADDING;
    sspi_w_token[2].cb_بفر = sspi_sizes.cb_بلاڪ_سائيز;
    sspi_w_token[2].pv_بفر = curlx_malloc(sspi_sizes.cb_بلاڪ_سائيز);
    إذا(!sspi_w_token[2].pv_بفر)
      أرجع socks5_free(sspi_w_token, CURLE_ٻاهريون_جو_يادگيري);

    حالت = Curl_pSecFn->Encrypt_پيغام(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    إذا(check_sspi_err(معلومات, حالت, "EncryptMessage"))
      أرجع socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_سائيز = sspi_w_token[0].cb_بفر + sspi_w_token[1].cb_بفر +
      sspi_w_token[2].cb_بفر;
    إذا(etbuf_سائيز > 0xffff)
      أرجع socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_سائيز);
    إذا(!etbuf)
      أرجع socks5_free(sspi_w_token, CURLE_ٻاهريون_جو_يادگيري);

    يادگيري_نقل(etbuf, sspi_w_token[0].pv_بفر, sspi_w_token[0].cb_بفر);
    يادگيري_نقل(etbuf + sspi_w_token[0].cb_بفر,
           sspi_w_token[1].pv_بفر, sspi_w_token[1].cb_بفر);
    يادگيري_نقل(etbuf + sspi_w_token[0].cb_بفر + sspi_w_token[1].cb_بفر,
           sspi_w_token[2].pv_بفر, sspi_w_token[2].cb_بفر);

    (فراغ)socks5_free(sspi_w_token, CURLE_OK);

    us_ڊيگهائي = htons((بنانشان ننڍو)etbuf_سائيز);
    يادگيري_نقل(socksreq + 2, &us_ڊيگهائي, حجم(ننڍو));
  }

  ڪوڊ = Curl_conn_cf_send(cf->ايندڙ, معلومات, socksreq, 4, FALSE, &written);
  إذا(ڪوڊ || (written != 4)) {
    failf(معلومات, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(معلومات->سيٽ.socks5_gssapi_nec) {
    يادگيري_نقل(socksreq, &gss_enc, 1);
    ڪوڊ = Curl_conn_cf_send(cf->ايندڙ, معلومات, socksreq, 1, FALSE, &written);
    إذا(ڪوڊ || (written != 1)) {
      failf(معلومات, "Failed to send SSPI encryption type.");
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  وإلا {
    ڪوڊ = Curl_conn_cf_send(cf->ايندڙ, معلومات, etbuf, etbuf_سائيز, FALSE,
                             &written);
    curlx_free(etbuf);
    إذا(ڪوڊ || (etbuf_سائيز != written)) {
      failf(معلومات, "Failed to send SSPI encryption type.");
      أرجع CURLE_COULDNT_CONNECT;
    }
  }

  نتيجو = Curl_blockread_all(cf, معلومات, (حرف *)socksreq, 4, &actualread);
  إذا(نتيجو || (actualread != 4)) {
    failf(معلومات, "Failed to receive SSPI encryption response.");
    أرجع نتيجو ? نتيجو : CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] == 255) {
    failf(معلومات, "User was rejected by the SOCKS5 server (%u %u).",
          (بنانشان صحيح)socksreq[0], (بنانشان صحيح)socksreq[1]);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] != 2) {
    failf(معلومات, "Invalid SSPI encryption response type (%u %u).",
          (بنانشان صحيح)socksreq[0], (بنانشان صحيح)socksreq[1]);
    أرجع CURLE_COULDNT_CONNECT;
  }

  يادگيري_نقل(&us_ڊيگهائي, socksreq + 2, حجم(ننڍو));
  us_ڊيگهائي = ntohs(us_ڊيگهائي);

  إذا(!us_ڊيگهائي) {
    failf(معلومات, "Invalid zero-length SSPI encryption token.");
    أرجع CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cb_بفر = us_ڊيگهائي;
  sspi_w_token[0].pv_بفر = curlx_malloc(us_ڊيگهائي);
  إذا(!sspi_w_token[0].pv_بفر)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  نتيجو = Curl_blockread_all(cf, معلومات, (حرف *)sspi_w_token[0].pv_بفر,
                              sspi_w_token[0].cb_بفر, &actualread);

  إذا(نتيجو || (actualread != us_ڊيگهائي)) {
    failf(معلومات, "Failed to receive SSPI encryption type.");
    اذهب fail;
  }

  إذا(!معلومات->سيٽ.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].بفر_Type = SECBUFFER_وهڪرو;
    sspi_w_token[1].بفر_Type = SECBUFFER_معلومات;
    sspi_w_token[1].cb_بفر = 0;
    sspi_w_token[1].pv_بفر = عدم;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    حالت = Curl_pSecFn->Decrypt_پيغام(sspi_context, &wrap_desc, 0, &qop);

    إذا(check_sspi_err(معلومات, حالت, "DecryptMessage"))
      اذهب fail;

    إذا(sspi_w_token[1].cb_بفر != 1) {
      failf(معلومات, "Invalid SSPI encryption response length (%lu).",
            (بنانشان ڊگهو)sspi_w_token[1].cb_بفر);
      اذهب fail;
    }

    يادگيري_نقل(socksreq, sspi_w_token[1].pv_بفر, sspi_w_token[1].cb_بفر);
  }
  وإلا {
    إذا(sspi_w_token[0].cb_بفر != 1) {
      failf(معلومات, "Invalid SSPI encryption response length (%lu).",
            (بنانشان ڊگهو)sspi_w_token[0].cb_بفر);
      اذهب fail;
    }
    يادگيري_نقل(socksreq, sspi_w_token[0].pv_بفر, sspi_w_token[0].cb_بفر);
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
  CURLcode نتيجو;
  SECURITY_حالت حالت;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  Sec_Pkg_Credentials_نالا نالا;
  حرف *service_name = عدم;
  بنانشان ڊگهو sspi_ret_flags = 0;

  memset(&cred_handle, 0, حجم(cred_handle));
  memset(&sspi_context, 0, حجم(sspi_context));
  نالا.sUserName = عدم;

  نتيجو = socks5_sspi_تياري(cf, معلومات, creds, &cred_handle, &service_name);
  إذا(نتيجو)
    اذهب error;

  نتيجو = socks5_sspi_loop(cf, معلومات, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  إذا(نتيجو)
    اذهب error;

  curlx_safefree(service_name);

  حالت = Curl_pSecFn->Query_Credentials_خاصيتون(&cred_handle,
                                                   SECPKG_CRED_ATTR_نالا,
                                                   &نالا);
  إذا(check_sspi_err(معلومات, حالت, "QueryCredentialAttributes")) {
    failf(معلومات, "Failed to determine username.");
    نتيجو = CURLE_COULDNT_CONNECT;
    اذهب error;
  }
  وإلا {
    VERBOSE(حرف *user_utf8 = curlx_convert_tchar_منزل_UTF8(نالا.sUserName));
    infof(معلومات, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->Free_Context_بفر(نالا.sUserName);
    نالا.sUserName = عدم;
  }

  نتيجو = socks5_sspi_encrypt(cf, معلومات, &sspi_context, sspi_ret_flags);
  إذا(نتيجو)
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
  إذا(نالا.sUserName)
    Curl_pSecFn->Free_Context_بفر(نالا.sUserName);
  أرجع نتيجو;
}
#جيڪڏهن_پورو
