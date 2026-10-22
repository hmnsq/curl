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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_PROXY)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strerror.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/nonblock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socks.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sspi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/multibyte.h"

/*
 * Helper SSPI error functions.
 */
ਸਥਿਰ ਪੂਰਨ check_sspi_err(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ,
                          ਅਚਲ ਅੱਖਰ *function)
{
  ਜੇ(ਸਥਿਤੀ != SEC_E_OK &&
     ਸਥਿਤੀ != SEC_I_COMPLETE_AND_CONTINUE &&
     ਸਥਿਤੀ != SEC_I_COMPLETE_NEEDED &&
     ਸਥਿਤੀ != SEC_I_CONTINUE_NEEDED) {
    ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
    failf(ਅੰਕੜੇ, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਵਾਪਸ 1;
  }
  ਵਾਪਸ 0;
}

/* This is the SSPI-using version of this function */
ਸਥਿਰ CURLcode socks5_sspi_ਤਿਆਰੀ(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ Curl_creds *creds,
                                  CredHandle *cred_handle,
                                  ਅੱਖਰ **service_namep)
{
  ਬਣਤਰ connectdata *conn = cf->conn;
  ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;

  /* prepare service name */
  ਜੇ(strchr(service, '/'))
    *service_namep = curlx_strdup(service);
  ਨਹੀਂਤਾਂ
    *service_namep = curl_maprintf("%s/%s",
                                   service, conn->socks_proxy.peer->hostname);
  ਜੇ(!*service_namep)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਸਥਿਤੀ =
    Curl_pSecFn->AcquireCredentialsHandle(ਨਿਰਾ, CURL_UNCONST(ਪਾਠ_2("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
                                          cred_handle, ਨਿਰਾ);

  ਜੇ(check_sspi_err(ਅੰਕੜੇ, ਸਥਿਤੀ, "AcquireCredentialsHandle")) {
    failf(ਅੰਕੜੇ, "Failed to acquire credentials.");
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode socks5_free_token(Sec_ਬਫਰ *send_token,
                                  CURLcode ਨਤੀਜਾ)
{
  ਜੇ(send_token->pv_ਬਫਰ) {
    Curl_pSecFn->Free_Context_ਬਫਰ(send_token->pv_ਬਫਰ);
    send_token->pv_ਬਫਰ = ਨਿਰਾ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode socks5_sspi_loop(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 ਅੱਖਰ *service_name,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *sspi_ret_flagsp)
{
  ਬਣਤਰ connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  CURLcode ਕੋਡ;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;
  Sec_ਬਫਰ sspi_send_token, sspi_recv_token;
  Sec_ਬਫਰ_Desc ਨਿਵੇਸ਼_desc, output_desc;
  PCtxtHandle context_handle = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ us_ਲੰਮਾਈ;
  size_t actualread;
  size_t written;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ socksreq[4];

  ਨਿਵੇਸ਼_desc.cBuffers = 1;
  ਨਿਵੇਸ਼_desc.pBuffers = &sspi_recv_token;
  ਨਿਵੇਸ਼_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;

  sspi_recv_token.ਬਫਰ_Type = SECBUFFER_TOKEN;
  sspi_recv_token.cb_ਬਫਰ = 0;
  sspi_recv_token.pv_ਬਫਰ = ਨਿਰਾ;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;

  sspi_send_token.ਬਫਰ_Type = SECBUFFER_TOKEN;
  sspi_send_token.cb_ਬਫਰ = 0;
  sspi_send_token.pv_ਬਫਰ = ਨਿਰਾ;

  (ਖਾਲੀ)curlx_nonblock(sock, FALSE);

  ਲਈ(;;) {
    TCHAR *sname = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(service_name);
    ਜੇ(!sname) {
      curlx_free(sspi_recv_token.pv_ਬਫਰ);
      ਵਾਪਸ socks5_free_token(&sspi_send_token, CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ);
    }

    ਸਥਿਤੀ =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_ਸਮ੍ਰਿਤੀ |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &ਨਿਵੇਸ਼_desc, 0,
                                             sspi_context,
                                             &output_desc,
                                             sspi_ret_flagsp, ਨਿਰਾ);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pv_ਬਫਰ);
    sspi_recv_token.cb_ਬਫਰ = 0;

    ਜੇ(check_sspi_err(ਅੰਕੜੇ, ਸਥਿਤੀ, "InitializeSecurityContext")) {
      failf(ਅੰਕੜੇ, "Failed to initialize security context.");
      ਵਾਪਸ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    ਜੇ(sspi_send_token.cb_ਬਫਰ) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      ਜੇ(sspi_send_token.cb_ਬਫਰ > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        ਵਾਪਸ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_ਲੰਮਾਈ = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)sspi_send_token.cb_ਬਫਰ);
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq + 2, &us_ਲੰਮਾਈ, ਆਕਾਰ(ਛੋਟਾ));

      ਕੋਡ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, socksreq, 4, FALSE, &written);
      ਜੇ(ਕੋਡ || (written != 4)) {
        failf(ਅੰਕੜੇ, "Failed to send SSPI authentication request.");
        ਵਾਪਸ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      ਕੋਡ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ,
                               sspi_send_token.pv_ਬਫਰ,
                               sspi_send_token.cb_ਬਫਰ, FALSE, &written);
      ਜੇ(ਕੋਡ || (sspi_send_token.cb_ਬਫਰ != written)) {
        failf(ਅੰਕੜੇ, "Failed to send SSPI authentication token.");
        ਵਾਪਸ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
    }

    ਜੇ(sspi_send_token.pv_ਬਫਰ)
      socks5_free_token(&sspi_send_token, CURLE_OK);
    sspi_send_token.cb_ਬਫਰ = 0;

    curlx_safefree(sspi_recv_token.pv_ਬਫਰ);
    sspi_recv_token.cb_ਬਫਰ = 0;

    ਜੇ(ਸਥਿਤੀ != SEC_I_CONTINUE_NEEDED)
      ਰੋਕੋ;

    ਨਤੀਜਾ = Curl_blockread_all(cf, ਅੰਕੜੇ, (ਅੱਖਰ *)socksreq, 4, &actualread);
    ਜੇ(ਨਤੀਜਾ || (actualread != 4)) {
      failf(ਅੰਕੜੇ, "Failed to receive SSPI authentication response.");
      ਵਾਪਸ ਨਤੀਜਾ ? ਨਤੀਜਾ : CURLE_COULDNT_CONNECT;
    }

    ਜੇ(socksreq[1] == 255) {
      failf(ਅੰਕੜੇ, "User was rejected by the SOCKS5 server (%u %u).",
            (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)socksreq[0], (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)socksreq[1]);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    ਜੇ(socksreq[1] != 1) {
      failf(ਅੰਕੜੇ, "Invalid SSPI authentication response type (%u %u).",
            (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)socksreq[0], (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)socksreq[1]);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&us_ਲੰਮਾਈ, socksreq + 2, ਆਕਾਰ(ਛੋਟਾ));
    us_ਲੰਮਾਈ = ntohs(us_ਲੰਮਾਈ);

    ਜੇ(!us_ਲੰਮਾਈ) {
      failf(ਅੰਕੜੇ, "Invalid zero-length SSPI authentication token.");
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cb_ਬਫਰ = us_ਲੰਮਾਈ;
    sspi_recv_token.pv_ਬਫਰ = curlx_malloc(us_ਲੰਮਾਈ);

    ਜੇ(!sspi_recv_token.pv_ਬਫਰ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    ਨਤੀਜਾ = Curl_blockread_all(cf, ਅੰਕੜੇ, (ਅੱਖਰ *)sspi_recv_token.pv_ਬਫਰ,
                                sspi_recv_token.cb_ਬਫਰ, &actualread);

    ਜੇ(ਨਤੀਜਾ || (actualread != us_ਲੰਮਾਈ)) {
      failf(ਅੰਕੜੇ, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pv_ਬਫਰ);
      ਵਾਪਸ ਨਤੀਜਾ ? ਨਤੀਜਾ : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode socks5_free(Sec_ਬਫਰ *sspi_w_token,
                            CURLcode ਨਤੀਜਾ)
{
  curlx_safefree(sspi_w_token[0].pv_ਬਫਰ);
  curlx_safefree(sspi_w_token[1].pv_ਬਫਰ);
  curlx_safefree(sspi_w_token[2].pv_ਬਫਰ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode socks5_sspi_encrypt(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    CtxtHandle *sspi_context,
                                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sspi_ret_flags)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  CURLcode ਕੋਡ;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ gss_enc;
  Sec_ਬਫਰ sspi_w_token[3];
  Sec_ਬਫਰ_Desc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ us_ਲੰਮਾਈ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ qop;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ socksreq[4];
  uint8_t *etbuf = ਨਿਰਾ;
  size_t etbuf_ਮਾਪ = 0;
  size_t actualread;
  size_t written;

  gss_enc = 0;
  ਜੇ(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  ਨਹੀਂਤਾਂ ਜੇ(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(ਅੰਕੜੇ, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pv_ਬਫਰ =
    sspi_w_token[1].pv_ਬਫਰ =
    sspi_w_token[2].pv_ਬਫਰ = ਨਿਰਾ;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;

  socksreq[0] = 1;
  socksreq[1] = 2;

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.socks5_gssapi_nec) {
    us_ਲੰਮਾਈ = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)1);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq + 2, &us_ਲੰਮਾਈ, ਆਕਾਰ(ਛੋਟਾ));
  }
  ਨਹੀਂਤਾਂ {
    ਸਥਿਤੀ = Curl_pSecFn->Query_Context_ਗੁਣ(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    ਜੇ(check_sspi_err(ਅੰਕੜੇ, ਸਥਿਤੀ, "QueryContextAttributes")) {
      failf(ਅੰਕੜੇ, "Failed to query security context attributes.");
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cb_ਬਫਰ = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].ਬਫਰ_Type = SECBUFFER_TOKEN;
    sspi_w_token[0].pv_ਬਫਰ = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    ਜੇ(!sspi_w_token[0].pv_ਬਫਰ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    sspi_w_token[1].cb_ਬਫਰ = 1;
    sspi_w_token[1].ਬਫਰ_Type = SECBUFFER_ਅੰਕੜੇ;
    sspi_w_token[1].pv_ਬਫਰ = curlx_malloc(1);
    ਜੇ(!sspi_w_token[1].pv_ਬਫਰ)
      ਵਾਪਸ socks5_free(sspi_w_token, CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ);

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(sspi_w_token[1].pv_ਬਫਰ, &gss_enc, 1);
    sspi_w_token[2].ਬਫਰ_Type = SECBUFFER_PADDING;
    sspi_w_token[2].cb_ਬਫਰ = sspi_sizes.cb_ਖੰਡ_ਮਾਪ;
    sspi_w_token[2].pv_ਬਫਰ = curlx_malloc(sspi_sizes.cb_ਖੰਡ_ਮਾਪ);
    ਜੇ(!sspi_w_token[2].pv_ਬਫਰ)
      ਵਾਪਸ socks5_free(sspi_w_token, CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ);

    ਸਥਿਤੀ = Curl_pSecFn->Encrypt_ਸੁਨੇਹਾ(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    ਜੇ(check_sspi_err(ਅੰਕੜੇ, ਸਥਿਤੀ, "EncryptMessage"))
      ਵਾਪਸ socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_ਮਾਪ = sspi_w_token[0].cb_ਬਫਰ + sspi_w_token[1].cb_ਬਫਰ +
      sspi_w_token[2].cb_ਬਫਰ;
    ਜੇ(etbuf_ਮਾਪ > 0xffff)
      ਵਾਪਸ socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_ਮਾਪ);
    ਜੇ(!etbuf)
      ਵਾਪਸ socks5_free(sspi_w_token, CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ);

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(etbuf, sspi_w_token[0].pv_ਬਫਰ, sspi_w_token[0].cb_ਬਫਰ);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(etbuf + sspi_w_token[0].cb_ਬਫਰ,
           sspi_w_token[1].pv_ਬਫਰ, sspi_w_token[1].cb_ਬਫਰ);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(etbuf + sspi_w_token[0].cb_ਬਫਰ + sspi_w_token[1].cb_ਬਫਰ,
           sspi_w_token[2].pv_ਬਫਰ, sspi_w_token[2].cb_ਬਫਰ);

    (ਖਾਲੀ)socks5_free(sspi_w_token, CURLE_OK);

    us_ਲੰਮਾਈ = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)etbuf_ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq + 2, &us_ਲੰਮਾਈ, ਆਕਾਰ(ਛੋਟਾ));
  }

  ਕੋਡ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, socksreq, 4, FALSE, &written);
  ਜੇ(ਕੋਡ || (written != 4)) {
    failf(ਅੰਕੜੇ, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.socks5_gssapi_nec) {
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq, &gss_enc, 1);
    ਕੋਡ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, socksreq, 1, FALSE, &written);
    ਜੇ(ਕੋਡ || (written != 1)) {
      failf(ਅੰਕੜੇ, "Failed to send SSPI encryption type.");
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }
  }
  ਨਹੀਂਤਾਂ {
    ਕੋਡ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, etbuf, etbuf_ਮਾਪ, FALSE,
                             &written);
    curlx_free(etbuf);
    ਜੇ(ਕੋਡ || (etbuf_ਮਾਪ != written)) {
      failf(ਅੰਕੜੇ, "Failed to send SSPI encryption type.");
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }
  }

  ਨਤੀਜਾ = Curl_blockread_all(cf, ਅੰਕੜੇ, (ਅੱਖਰ *)socksreq, 4, &actualread);
  ਜੇ(ਨਤੀਜਾ || (actualread != 4)) {
    failf(ਅੰਕੜੇ, "Failed to receive SSPI encryption response.");
    ਵਾਪਸ ਨਤੀਜਾ ? ਨਤੀਜਾ : CURLE_COULDNT_CONNECT;
  }

  ਜੇ(socksreq[1] == 255) {
    failf(ਅੰਕੜੇ, "User was rejected by the SOCKS5 server (%u %u).",
          (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)socksreq[0], (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)socksreq[1]);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  ਜੇ(socksreq[1] != 2) {
    failf(ਅੰਕੜੇ, "Invalid SSPI encryption response type (%u %u).",
          (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)socksreq[0], (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)socksreq[1]);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&us_ਲੰਮਾਈ, socksreq + 2, ਆਕਾਰ(ਛੋਟਾ));
  us_ਲੰਮਾਈ = ntohs(us_ਲੰਮਾਈ);

  ਜੇ(!us_ਲੰਮਾਈ) {
    failf(ਅੰਕੜੇ, "Invalid zero-length SSPI encryption token.");
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cb_ਬਫਰ = us_ਲੰਮਾਈ;
  sspi_w_token[0].pv_ਬਫਰ = curlx_malloc(us_ਲੰਮਾਈ);
  ਜੇ(!sspi_w_token[0].pv_ਬਫਰ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਨਤੀਜਾ = Curl_blockread_all(cf, ਅੰਕੜੇ, (ਅੱਖਰ *)sspi_w_token[0].pv_ਬਫਰ,
                              sspi_w_token[0].cb_ਬਫਰ, &actualread);

  ਜੇ(ਨਤੀਜਾ || (actualread != us_ਲੰਮਾਈ)) {
    failf(ਅੰਕੜੇ, "Failed to receive SSPI encryption type.");
    ਜਾਓ fail;
  }

  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].ਬਫਰ_Type = SECBUFFER_ਧਾਰਾ;
    sspi_w_token[1].ਬਫਰ_Type = SECBUFFER_ਅੰਕੜੇ;
    sspi_w_token[1].cb_ਬਫਰ = 0;
    sspi_w_token[1].pv_ਬਫਰ = ਨਿਰਾ;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    ਸਥਿਤੀ = Curl_pSecFn->Decrypt_ਸੁਨੇਹਾ(sspi_context, &wrap_desc, 0, &qop);

    ਜੇ(check_sspi_err(ਅੰਕੜੇ, ਸਥਿਤੀ, "DecryptMessage"))
      ਜਾਓ fail;

    ਜੇ(sspi_w_token[1].cb_ਬਫਰ != 1) {
      failf(ਅੰਕੜੇ, "Invalid SSPI encryption response length (%lu).",
            (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)sspi_w_token[1].cb_ਬਫਰ);
      ਜਾਓ fail;
    }

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq, sspi_w_token[1].pv_ਬਫਰ, sspi_w_token[1].cb_ਬਫਰ);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(sspi_w_token[0].cb_ਬਫਰ != 1) {
      failf(ਅੰਕੜੇ, "Invalid SSPI encryption response length (%lu).",
            (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)sspi_w_token[0].cb_ਬਫਰ);
      ਜਾਓ fail;
    }
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq, sspi_w_token[0].pv_ਬਫਰ, sspi_w_token[0].cb_ਬਫਰ);
  }
  curlx_free(sspi_w_token[0].pv_ਬਫਰ);

  infof(ਅੰਕੜੇ, "SOCKS5 access %s protection granted BUT NOT USED.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  cf->conn->socks5_gssapi_enctype = socksreq[0];
  ਵਾਪਸ CURLE_OK;

fail:
  curlx_free(sspi_w_token[0].pv_ਬਫਰ);
  ਵਾਪਸ CURLE_COULDNT_CONNECT;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ Curl_creds *creds)
{
  ਬਣਤਰ connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode ਨਤੀਜਾ;
  SECURITY_ਸਥਿਤੀ ਸਥਿਤੀ;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  Sec_Pkg_Credentials_ਨਾਂ ਨਾਂ;
  ਅੱਖਰ *service_name = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sspi_ret_flags = 0;

  memset(&cred_handle, 0, ਆਕਾਰ(cred_handle));
  memset(&sspi_context, 0, ਆਕਾਰ(sspi_context));
  ਨਾਂ.sUserName = ਨਿਰਾ;

  ਨਤੀਜਾ = socks5_sspi_ਤਿਆਰੀ(cf, ਅੰਕੜੇ, creds, &cred_handle, &service_name);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ error;

  ਨਤੀਜਾ = socks5_sspi_loop(cf, ਅੰਕੜੇ, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ error;

  curlx_safefree(service_name);

  ਸਥਿਤੀ = Curl_pSecFn->Query_Credentials_ਗੁਣ(&cred_handle,
                                                   SECPKG_CRED_ATTR_ਨਾਂ,
                                                   &ਨਾਂ);
  ਜੇ(check_sspi_err(ਅੰਕੜੇ, ਸਥਿਤੀ, "QueryCredentialAttributes")) {
    failf(ਅੰਕੜੇ, "Failed to determine username.");
    ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
    ਜਾਓ error;
  }
  ਨਹੀਂਤਾਂ {
    VERBOSE(ਅੱਖਰ *user_utf8 = curlx_convert_tchar_ਮੰਜ਼ਿਲ_UTF8(ਨਾਂ.sUserName));
    infof(ਅੰਕੜੇ, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->Free_Context_ਬਫਰ(ਨਾਂ.sUserName);
    ਨਾਂ.sUserName = ਨਿਰਾ;
  }

  ਨਤੀਜਾ = socks5_sspi_encrypt(cf, ਅੰਕੜੇ, &sspi_context, sspi_ret_flags);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ error;

  (ਖਾਲੀ)curlx_nonblock(sock, TRUE);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  ਵਾਪਸ CURLE_OK;

error:
  (ਖਾਲੀ)curlx_nonblock(sock, TRUE);
  curlx_free(service_name);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  ਜੇ(ਨਾਂ.sUserName)
    Curl_pSecFn->Free_Context_ਬਫਰ(ਨਾਂ.sUserName);
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ
