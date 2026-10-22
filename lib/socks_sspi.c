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
ساكن صحيح check_sspi_err(بنية Curl_easy *داده,
                          SECURITY_وضعیت وضعیت,
                          ثابت حرف *function)
{
  إذا(وضعیت != SEC_E_OK &&
     وضعیت != SEC_I_COMPLETE_AND_CONTINUE &&
     وضعیت != SEC_I_COMPLETE_NEEDED &&
     وضعیت != SEC_I_CONTINUE_NEEDED) {
    حرف میانگیر[STRERROR_LEN];
    failf(داده, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(وضعیت, میانگیر, حجم(میانگیر)));
    أرجع 1;
  }
  أرجع 0;
}

/* This is the SSPI-using version of this function */
ساكن CURLcode socks5_sspi_راهاندازی(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  بنية Curl_creds *creds,
                                  CredHandle *cred_handle,
                                  حرف **service_namep)
{
  بنية connectdata *conn = cf->conn;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  SECURITY_وضعیت وضعیت;

  /* prepare service name */
  إذا(strchr(service, '/'))
    *service_namep = curlx_strdup(service);
  وإلا
    *service_namep = curl_maprintf("%s/%s",
                                   service, conn->socks_proxy.peer->hostname);
  إذا(!*service_namep)
    أرجع CURLE_بیرون_از_حافظه;

  وضعیت =
    Curl_pSecFn->AcquireCredentialsHandle(عدم, CURL_UNCONST(متن_2("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          عدم, عدم, عدم, عدم,
                                          cred_handle, عدم);

  إذا(check_sspi_err(داده, وضعیت, "AcquireCredentialsHandle")) {
    failf(داده, "Failed to acquire credentials.");
    أرجع CURLE_COULDNT_CONNECT;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode socks5_free_token(Sec_میانگیر *send_token,
                                  CURLcode نتیجه)
{
  إذا(send_token->pv_میانگیر) {
    Curl_pSecFn->Free_Context_میانگیر(send_token->pv_میانگیر);
    send_token->pv_میانگیر = عدم;
  }
  أرجع نتیجه;
}

ساكن CURLcode socks5_sspi_loop(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 حرف *service_name,
                                 بیعلامت دراز *sspi_ret_flagsp)
{
  بنية connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode نتیجه = CURLE_OK;
  CURLcode کد;
  SECURITY_وضعیت وضعیت;
  Sec_میانگیر sspi_send_token, sspi_recv_token;
  Sec_میانگیر_Desc درون_داد_desc, output_desc;
  PCtxtHandle context_handle = عدم;
  بیعلامت کوتاه us_درازی;
  size_t actualread;
  size_t written;
  بیعلامت حرف socksreq[4];

  درون_داد_desc.cBuffers = 1;
  درون_داد_desc.pBuffers = &sspi_recv_token;
  درون_داد_desc.ul_نسخه = SECBUFFER_نسخه;

  sspi_recv_token.میانگیر_Type = SECBUFFER_TOKEN;
  sspi_recv_token.cb_میانگیر = 0;
  sspi_recv_token.pv_میانگیر = عدم;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ul_نسخه = SECBUFFER_نسخه;

  sspi_send_token.میانگیر_Type = SECBUFFER_TOKEN;
  sspi_send_token.cb_میانگیر = 0;
  sspi_send_token.pv_میانگیر = عدم;

  (فراغ)curlx_nonblock(sock, FALSE);

  لكل(;;) {
    TCHAR *sname = curlx_convert_UTF8_مقصد_tchar(service_name);
    إذا(!sname) {
      curlx_free(sspi_recv_token.pv_میانگیر);
      أرجع socks5_free_token(&sspi_send_token, CURLE_بیرون_از_حافظه);
    }

    وضعیت =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_حافظه |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &درون_داد_desc, 0,
                                             sspi_context,
                                             &output_desc,
                                             sspi_ret_flagsp, عدم);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pv_میانگیر);
    sspi_recv_token.cb_میانگیر = 0;

    إذا(check_sspi_err(داده, وضعیت, "InitializeSecurityContext")) {
      failf(داده, "Failed to initialize security context.");
      أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    إذا(sspi_send_token.cb_میانگیر) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      إذا(sspi_send_token.cb_میانگیر > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_درازی = htons((بیعلامت کوتاه)sspi_send_token.cb_میانگیر);
      کپی_حافظه(socksreq + 2, &us_درازی, حجم(کوتاه));

      کد = Curl_conn_cf_send(cf->بعدی, داده, socksreq, 4, FALSE, &written);
      إذا(کد || (written != 4)) {
        failf(داده, "Failed to send SSPI authentication request.");
        أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      کد = Curl_conn_cf_send(cf->بعدی, داده,
                               sspi_send_token.pv_میانگیر,
                               sspi_send_token.cb_میانگیر, FALSE, &written);
      إذا(کد || (sspi_send_token.cb_میانگیر != written)) {
        failf(داده, "Failed to send SSPI authentication token.");
        أرجع socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
    }

    إذا(sspi_send_token.pv_میانگیر)
      socks5_free_token(&sspi_send_token, CURLE_OK);
    sspi_send_token.cb_میانگیر = 0;

    curlx_safefree(sspi_recv_token.pv_میانگیر);
    sspi_recv_token.cb_میانگیر = 0;

    إذا(وضعیت != SEC_I_CONTINUE_NEEDED)
      اكسر;

    نتیجه = Curl_blockread_all(cf, داده, (حرف *)socksreq, 4, &actualread);
    إذا(نتیجه || (actualread != 4)) {
      failf(داده, "Failed to receive SSPI authentication response.");
      أرجع نتیجه ? نتیجه : CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] == 255) {
      failf(داده, "User was rejected by the SOCKS5 server (%u %u).",
            (بیعلامت صحيح)socksreq[0], (بیعلامت صحيح)socksreq[1]);
      أرجع CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] != 1) {
      failf(داده, "Invalid SSPI authentication response type (%u %u).",
            (بیعلامت صحيح)socksreq[0], (بیعلامت صحيح)socksreq[1]);
      أرجع CURLE_COULDNT_CONNECT;
    }

    کپی_حافظه(&us_درازی, socksreq + 2, حجم(کوتاه));
    us_درازی = ntohs(us_درازی);

    إذا(!us_درازی) {
      failf(داده, "Invalid zero-length SSPI authentication token.");
      أرجع CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cb_میانگیر = us_درازی;
    sspi_recv_token.pv_میانگیر = curlx_malloc(us_درازی);

    إذا(!sspi_recv_token.pv_میانگیر)
      أرجع CURLE_بیرون_از_حافظه;

    نتیجه = Curl_blockread_all(cf, داده, (حرف *)sspi_recv_token.pv_میانگیر,
                                sspi_recv_token.cb_میانگیر, &actualread);

    إذا(نتیجه || (actualread != us_درازی)) {
      failf(داده, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pv_میانگیر);
      أرجع نتیجه ? نتیجه : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode socks5_free(Sec_میانگیر *sspi_w_token,
                            CURLcode نتیجه)
{
  curlx_safefree(sspi_w_token[0].pv_میانگیر);
  curlx_safefree(sspi_w_token[1].pv_میانگیر);
  curlx_safefree(sspi_w_token[2].pv_میانگیر);
  أرجع نتیجه;
}

ساكن CURLcode socks5_sspi_encrypt(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    CtxtHandle *sspi_context,
                                    بیعلامت دراز sspi_ret_flags)
{
  CURLcode نتیجه = CURLE_OK;
  CURLcode کد;
  SECURITY_وضعیت وضعیت;
  بیعلامت حرف gss_enc;
  Sec_میانگیر sspi_w_token[3];
  Sec_میانگیر_Desc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  بیعلامت کوتاه us_درازی;
  بیعلامت دراز qop;
  بیعلامت حرف socksreq[4];
  uint8_t *etbuf = عدم;
  size_t etbuf_اندازه = 0;
  size_t actualread;
  size_t written;

  gss_enc = 0;
  إذا(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  وإلا إذا(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(داده, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pv_میانگیر =
    sspi_w_token[1].pv_میانگیر =
    sspi_w_token[2].pv_میانگیر = عدم;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ul_نسخه = SECBUFFER_نسخه;

  socksreq[0] = 1;
  socksreq[1] = 2;

  إذا(داده->مجموعه.socks5_gssapi_nec) {
    us_درازی = htons((بیعلامت کوتاه)1);
    کپی_حافظه(socksreq + 2, &us_درازی, حجم(کوتاه));
  }
  وإلا {
    وضعیت = Curl_pSecFn->Query_Context_صفات(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    إذا(check_sspi_err(داده, وضعیت, "QueryContextAttributes")) {
      failf(داده, "Failed to query security context attributes.");
      أرجع CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cb_میانگیر = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].میانگیر_Type = SECBUFFER_TOKEN;
    sspi_w_token[0].pv_میانگیر = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    إذا(!sspi_w_token[0].pv_میانگیر)
      أرجع CURLE_بیرون_از_حافظه;

    sspi_w_token[1].cb_میانگیر = 1;
    sspi_w_token[1].میانگیر_Type = SECBUFFER_داده;
    sspi_w_token[1].pv_میانگیر = curlx_malloc(1);
    إذا(!sspi_w_token[1].pv_میانگیر)
      أرجع socks5_free(sspi_w_token, CURLE_بیرون_از_حافظه);

    کپی_حافظه(sspi_w_token[1].pv_میانگیر, &gss_enc, 1);
    sspi_w_token[2].میانگیر_Type = SECBUFFER_PADDING;
    sspi_w_token[2].cb_میانگیر = sspi_sizes.cb_بلوک_اندازه;
    sspi_w_token[2].pv_میانگیر = curlx_malloc(sspi_sizes.cb_بلوک_اندازه);
    إذا(!sspi_w_token[2].pv_میانگیر)
      أرجع socks5_free(sspi_w_token, CURLE_بیرون_از_حافظه);

    وضعیت = Curl_pSecFn->Encrypt_پیام(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    إذا(check_sspi_err(داده, وضعیت, "EncryptMessage"))
      أرجع socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_اندازه = sspi_w_token[0].cb_میانگیر + sspi_w_token[1].cb_میانگیر +
      sspi_w_token[2].cb_میانگیر;
    إذا(etbuf_اندازه > 0xffff)
      أرجع socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_اندازه);
    إذا(!etbuf)
      أرجع socks5_free(sspi_w_token, CURLE_بیرون_از_حافظه);

    کپی_حافظه(etbuf, sspi_w_token[0].pv_میانگیر, sspi_w_token[0].cb_میانگیر);
    کپی_حافظه(etbuf + sspi_w_token[0].cb_میانگیر,
           sspi_w_token[1].pv_میانگیر, sspi_w_token[1].cb_میانگیر);
    کپی_حافظه(etbuf + sspi_w_token[0].cb_میانگیر + sspi_w_token[1].cb_میانگیر,
           sspi_w_token[2].pv_میانگیر, sspi_w_token[2].cb_میانگیر);

    (فراغ)socks5_free(sspi_w_token, CURLE_OK);

    us_درازی = htons((بیعلامت کوتاه)etbuf_اندازه);
    کپی_حافظه(socksreq + 2, &us_درازی, حجم(کوتاه));
  }

  کد = Curl_conn_cf_send(cf->بعدی, داده, socksreq, 4, FALSE, &written);
  إذا(کد || (written != 4)) {
    failf(داده, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(داده->مجموعه.socks5_gssapi_nec) {
    کپی_حافظه(socksreq, &gss_enc, 1);
    کد = Curl_conn_cf_send(cf->بعدی, داده, socksreq, 1, FALSE, &written);
    إذا(کد || (written != 1)) {
      failf(داده, "Failed to send SSPI encryption type.");
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  وإلا {
    کد = Curl_conn_cf_send(cf->بعدی, داده, etbuf, etbuf_اندازه, FALSE,
                             &written);
    curlx_free(etbuf);
    إذا(کد || (etbuf_اندازه != written)) {
      failf(داده, "Failed to send SSPI encryption type.");
      أرجع CURLE_COULDNT_CONNECT;
    }
  }

  نتیجه = Curl_blockread_all(cf, داده, (حرف *)socksreq, 4, &actualread);
  إذا(نتیجه || (actualread != 4)) {
    failf(داده, "Failed to receive SSPI encryption response.");
    أرجع نتیجه ? نتیجه : CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] == 255) {
    failf(داده, "User was rejected by the SOCKS5 server (%u %u).",
          (بیعلامت صحيح)socksreq[0], (بیعلامت صحيح)socksreq[1]);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] != 2) {
    failf(داده, "Invalid SSPI encryption response type (%u %u).",
          (بیعلامت صحيح)socksreq[0], (بیعلامت صحيح)socksreq[1]);
    أرجع CURLE_COULDNT_CONNECT;
  }

  کپی_حافظه(&us_درازی, socksreq + 2, حجم(کوتاه));
  us_درازی = ntohs(us_درازی);

  إذا(!us_درازی) {
    failf(داده, "Invalid zero-length SSPI encryption token.");
    أرجع CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cb_میانگیر = us_درازی;
  sspi_w_token[0].pv_میانگیر = curlx_malloc(us_درازی);
  إذا(!sspi_w_token[0].pv_میانگیر)
    أرجع CURLE_بیرون_از_حافظه;

  نتیجه = Curl_blockread_all(cf, داده, (حرف *)sspi_w_token[0].pv_میانگیر,
                              sspi_w_token[0].cb_میانگیر, &actualread);

  إذا(نتیجه || (actualread != us_درازی)) {
    failf(داده, "Failed to receive SSPI encryption type.");
    اذهب fail;
  }

  إذا(!داده->مجموعه.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].میانگیر_Type = SECBUFFER_جریان;
    sspi_w_token[1].میانگیر_Type = SECBUFFER_داده;
    sspi_w_token[1].cb_میانگیر = 0;
    sspi_w_token[1].pv_میانگیر = عدم;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    وضعیت = Curl_pSecFn->Decrypt_پیام(sspi_context, &wrap_desc, 0, &qop);

    إذا(check_sspi_err(داده, وضعیت, "DecryptMessage"))
      اذهب fail;

    إذا(sspi_w_token[1].cb_میانگیر != 1) {
      failf(داده, "Invalid SSPI encryption response length (%lu).",
            (بیعلامت دراز)sspi_w_token[1].cb_میانگیر);
      اذهب fail;
    }

    کپی_حافظه(socksreq, sspi_w_token[1].pv_میانگیر, sspi_w_token[1].cb_میانگیر);
  }
  وإلا {
    إذا(sspi_w_token[0].cb_میانگیر != 1) {
      failf(داده, "Invalid SSPI encryption response length (%lu).",
            (بیعلامت دراز)sspi_w_token[0].cb_میانگیر);
      اذهب fail;
    }
    کپی_حافظه(socksreq, sspi_w_token[0].pv_میانگیر, sspi_w_token[0].cb_میانگیر);
  }
  curlx_free(sspi_w_token[0].pv_میانگیر);

  infof(داده, "SOCKS5 access %s protection granted BUT NOT USED.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  cf->conn->socks5_gssapi_enctype = socksreq[0];
  أرجع CURLE_OK;

fail:
  curlx_free(sspi_w_token[0].pv_میانگیر);
  أرجع CURLE_COULDNT_CONNECT;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده,
                                      بنية Curl_creds *creds)
{
  بنية connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode نتیجه;
  SECURITY_وضعیت وضعیت;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  Sec_Pkg_Credentials_نامها نامها;
  حرف *service_name = عدم;
  بیعلامت دراز sspi_ret_flags = 0;

  memset(&cred_handle, 0, حجم(cred_handle));
  memset(&sspi_context, 0, حجم(sspi_context));
  نامها.sUserName = عدم;

  نتیجه = socks5_sspi_راهاندازی(cf, داده, creds, &cred_handle, &service_name);
  إذا(نتیجه)
    اذهب error;

  نتیجه = socks5_sspi_loop(cf, داده, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  إذا(نتیجه)
    اذهب error;

  curlx_safefree(service_name);

  وضعیت = Curl_pSecFn->Query_Credentials_صفات(&cred_handle,
                                                   SECPKG_CRED_ATTR_نامها,
                                                   &نامها);
  إذا(check_sspi_err(داده, وضعیت, "QueryCredentialAttributes")) {
    failf(داده, "Failed to determine username.");
    نتیجه = CURLE_COULDNT_CONNECT;
    اذهب error;
  }
  وإلا {
    VERBOSE(حرف *user_utf8 = curlx_convert_tchar_مقصد_UTF8(نامها.sUserName));
    infof(داده, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->Free_Context_میانگیر(نامها.sUserName);
    نامها.sUserName = عدم;
  }

  نتیجه = socks5_sspi_encrypt(cf, داده, &sspi_context, sspi_ret_flags);
  إذا(نتیجه)
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
  إذا(نامها.sUserName)
    Curl_pSecFn->Free_Context_میانگیر(نامها.sUserName);
  أرجع نتیجه;
}
#پایان_اگر
