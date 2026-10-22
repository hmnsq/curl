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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_PROXY)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "strerror.h"
#ಅನ್ತರ್ಭಾವ "curlx/nonblock.h"
#ಅನ್ತರ್ಭಾವ "socks.h"
#ಅನ್ತರ್ಭಾವ "curl_sspi.h"
#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"

/*
 * Helper SSPI error functions.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_sspi_err(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *function)
{
  ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_E_OK &&
     ಸ್ಥಿತಿ_2 != SEC_I_COMPLETE_AND_CONTINUE &&
     ಸ್ಥಿತಿ_2 != SEC_I_COMPLETE_NEEDED &&
     ಸ್ಥಿತಿ_2 != SEC_I_CONTINUE_NEEDED) {
    ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
    failf(ದತ್ತಾಂಶ, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(ಸ್ಥಿತಿ_2, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಹಿಂತಿರುಗಿ 1;
  }
  ಹಿಂತಿರುಗಿ 0;
}

/* This is the SSPI-using version of this function */
ಸ್ಥಿರ CURLcode socks5_sspi_ಸಿದ್ಧತೆ(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ Curl_creds *creds,
                                  CredHandle *cred_handle,
                                  ಅಕ್ಷರ **service_namep)
{
  ರಚನೆ connectdata *conn = cf->conn;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;

  /* prepare service name */
  ಆದರೆ(strchr(service, '/'))
    *service_namep = curlx_strdup(service);
  ಇಲ್ಲದಿದ್ದರೆ
    *service_namep = curl_maprintf("%s/%s",
                                   service, conn->socks_proxy.peer->hostname);
  ಆದರೆ(!*service_namep)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಸ್ಥಿತಿ_2 =
    Curl_pSecFn->AcquireCredentialsHandle(ಖಾಲಿ, CURL_UNCONST(ಪಠ್ಯ_2("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
                                          cred_handle, ಖಾಲಿ);

  ಆದರೆ(check_sspi_err(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2, "AcquireCredentialsHandle")) {
    failf(ದತ್ತಾಂಶ, "Failed to acquire credentials.");
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode socks5_free_token(Sec_ಬಫರ್ *send_token,
                                  CURLcode ಫಲಿತಾಂಶ)
{
  ಆದರೆ(send_token->pv_ಬಫರ್) {
    Curl_pSecFn->Free_Context_ಬಫರ್(send_token->pv_ಬಫರ್);
    send_token->pv_ಬಫರ್ = ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode socks5_sspi_loop(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 ಅಕ್ಷರ *service_name,
                                 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *sspi_ret_flagsp)
{
  ರಚನೆ connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  CURLcode ಸಂಕೇತ_2;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;
  Sec_ಬಫರ್ sspi_send_token, sspi_recv_token;
  Sec_ಬಫರ್_Desc ಒಳಹರಿವು_desc, output_desc;
  PCtxtHandle context_handle = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ us_ಉದ್ದಳತೆ;
  size_t actualread;
  size_t written;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ socksreq[4];

  ಒಳಹರಿವು_desc.cBuffers = 1;
  ಒಳಹರಿವು_desc.pBuffers = &sspi_recv_token;
  ಒಳಹರಿವು_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;

  sspi_recv_token.ಬಫರ್_Type = SECBUFFER_TOKEN;
  sspi_recv_token.cb_ಬಫರ್ = 0;
  sspi_recv_token.pv_ಬಫರ್ = ಖಾಲಿ;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;

  sspi_send_token.ಬಫರ್_Type = SECBUFFER_TOKEN;
  sspi_send_token.cb_ಬಫರ್ = 0;
  sspi_send_token.pv_ಬಫರ್ = ಖಾಲಿ;

  (ಶೂನ್ಯ)curlx_nonblock(sock, FALSE);

  ಗಾಗಿ(;;) {
    TCHAR *sname = curlx_convert_UTF8_ಗಮ್ಯ_tchar(service_name);
    ಆದರೆ(!sname) {
      curlx_free(sspi_recv_token.pv_ಬಫರ್);
      ಹಿಂತಿರುಗಿ socks5_free_token(&sspi_send_token, CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ);
    }

    ಸ್ಥಿತಿ_2 =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_ಸ್ಮರಣೆ |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &ಒಳಹರಿವು_desc, 0,
                                             sspi_context,
                                             &output_desc,
                                             sspi_ret_flagsp, ಖಾಲಿ);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pv_ಬಫರ್);
    sspi_recv_token.cb_ಬಫರ್ = 0;

    ಆದರೆ(check_sspi_err(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2, "InitializeSecurityContext")) {
      failf(ದತ್ತಾಂಶ, "Failed to initialize security context.");
      ಹಿಂತಿರುಗಿ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    ಆದರೆ(sspi_send_token.cb_ಬಫರ್) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      ಆದರೆ(sspi_send_token.cb_ಬಫರ್ > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        ಹಿಂತಿರುಗಿ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_ಉದ್ದಳತೆ = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)sspi_send_token.cb_ಬಫರ್);
      ಸ್ಮರಣೆ_ನಕಲು(socksreq + 2, &us_ಉದ್ದಳತೆ, ಗಾತ್ರ(ಗಿಡ್ಡ));

      ಸಂಕೇತ_2 = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, socksreq, 4, FALSE, &written);
      ಆದರೆ(ಸಂಕೇತ_2 || (written != 4)) {
        failf(ದತ್ತಾಂಶ, "Failed to send SSPI authentication request.");
        ಹಿಂತಿರುಗಿ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      ಸಂಕೇತ_2 = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ,
                               sspi_send_token.pv_ಬಫರ್,
                               sspi_send_token.cb_ಬಫರ್, FALSE, &written);
      ಆದರೆ(ಸಂಕೇತ_2 || (sspi_send_token.cb_ಬಫರ್ != written)) {
        failf(ದತ್ತಾಂಶ, "Failed to send SSPI authentication token.");
        ಹಿಂತಿರುಗಿ socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
    }

    ಆದರೆ(sspi_send_token.pv_ಬಫರ್)
      socks5_free_token(&sspi_send_token, CURLE_OK);
    sspi_send_token.cb_ಬಫರ್ = 0;

    curlx_safefree(sspi_recv_token.pv_ಬಫರ್);
    sspi_recv_token.cb_ಬಫರ್ = 0;

    ಆದರೆ(ಸ್ಥಿತಿ_2 != SEC_I_CONTINUE_NEEDED)
      ನಿಲ್ಲಿಸು;

    ಫಲಿತಾಂಶ = Curl_blockread_all(cf, ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)socksreq, 4, &actualread);
    ಆದರೆ(ಫಲಿತಾಂಶ || (actualread != 4)) {
      failf(ದತ್ತಾಂಶ, "Failed to receive SSPI authentication response.");
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ ? ಫಲಿತಾಂಶ : CURLE_COULDNT_CONNECT;
    }

    ಆದರೆ(socksreq[1] == 255) {
      failf(ದತ್ತಾಂಶ, "User was rejected by the SOCKS5 server (%u %u).",
            (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)socksreq[0], (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)socksreq[1]);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    ಆದರೆ(socksreq[1] != 1) {
      failf(ದತ್ತಾಂಶ, "Invalid SSPI authentication response type (%u %u).",
            (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)socksreq[0], (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)socksreq[1]);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    ಸ್ಮರಣೆ_ನಕಲು(&us_ಉದ್ದಳತೆ, socksreq + 2, ಗಾತ್ರ(ಗಿಡ್ಡ));
    us_ಉದ್ದಳತೆ = ntohs(us_ಉದ್ದಳತೆ);

    ಆದರೆ(!us_ಉದ್ದಳತೆ) {
      failf(ದತ್ತಾಂಶ, "Invalid zero-length SSPI authentication token.");
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cb_ಬಫರ್ = us_ಉದ್ದಳತೆ;
    sspi_recv_token.pv_ಬಫರ್ = curlx_malloc(us_ಉದ್ದಳತೆ);

    ಆದರೆ(!sspi_recv_token.pv_ಬಫರ್)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    ಫಲಿತಾಂಶ = Curl_blockread_all(cf, ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)sspi_recv_token.pv_ಬಫರ್,
                                sspi_recv_token.cb_ಬಫರ್, &actualread);

    ಆದರೆ(ಫಲಿತಾಂಶ || (actualread != us_ಉದ್ದಳತೆ)) {
      failf(ದತ್ತಾಂಶ, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pv_ಬಫರ್);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ ? ಫಲಿತಾಂಶ : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode socks5_free(Sec_ಬಫರ್ *sspi_w_token,
                            CURLcode ಫಲಿತಾಂಶ)
{
  curlx_safefree(sspi_w_token[0].pv_ಬಫರ್);
  curlx_safefree(sspi_w_token[1].pv_ಬಫರ್);
  curlx_safefree(sspi_w_token[2].pv_ಬಫರ್);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode socks5_sspi_encrypt(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    CtxtHandle *sspi_context,
                                    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sspi_ret_flags)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  CURLcode ಸಂಕೇತ_2;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ gss_enc;
  Sec_ಬಫರ್ sspi_w_token[3];
  Sec_ಬಫರ್_Desc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ us_ಉದ್ದಳತೆ;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ qop;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ socksreq[4];
  uint8_t *etbuf = ಖಾಲಿ;
  size_t etbuf_ಗಾತ್ರ = 0;
  size_t actualread;
  size_t written;

  gss_enc = 0;
  ಆದರೆ(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(ದತ್ತಾಂಶ, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pv_ಬಫರ್ =
    sspi_w_token[1].pv_ಬಫರ್ =
    sspi_w_token[2].pv_ಬಫರ್ = ಖಾಲಿ;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;

  socksreq[0] = 1;
  socksreq[1] = 2;

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.socks5_gssapi_nec) {
    us_ಉದ್ದಳತೆ = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)1);
    ಸ್ಮರಣೆ_ನಕಲು(socksreq + 2, &us_ಉದ್ದಳತೆ, ಗಾತ್ರ(ಗಿಡ್ಡ));
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿತಿ_2 = Curl_pSecFn->Query_Context_ಗುಣಲಕ್ಷಣಗಳು(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    ಆದರೆ(check_sspi_err(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2, "QueryContextAttributes")) {
      failf(ದತ್ತಾಂಶ, "Failed to query security context attributes.");
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cb_ಬಫರ್ = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].ಬಫರ್_Type = SECBUFFER_TOKEN;
    sspi_w_token[0].pv_ಬಫರ್ = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    ಆದರೆ(!sspi_w_token[0].pv_ಬಫರ್)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    sspi_w_token[1].cb_ಬಫರ್ = 1;
    sspi_w_token[1].ಬಫರ್_Type = SECBUFFER_ದತ್ತಾಂಶ;
    sspi_w_token[1].pv_ಬಫರ್ = curlx_malloc(1);
    ಆದರೆ(!sspi_w_token[1].pv_ಬಫರ್)
      ಹಿಂತಿರುಗಿ socks5_free(sspi_w_token, CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ);

    ಸ್ಮರಣೆ_ನಕಲು(sspi_w_token[1].pv_ಬಫರ್, &gss_enc, 1);
    sspi_w_token[2].ಬಫರ್_Type = SECBUFFER_PADDING;
    sspi_w_token[2].cb_ಬಫರ್ = sspi_sizes.cb_ಖಂಡ_ಗಾತ್ರ;
    sspi_w_token[2].pv_ಬಫರ್ = curlx_malloc(sspi_sizes.cb_ಖಂಡ_ಗಾತ್ರ);
    ಆದರೆ(!sspi_w_token[2].pv_ಬಫರ್)
      ಹಿಂತಿರುಗಿ socks5_free(sspi_w_token, CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ);

    ಸ್ಥಿತಿ_2 = Curl_pSecFn->Encrypt_ಸಂದೇಶ(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    ಆದರೆ(check_sspi_err(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2, "EncryptMessage"))
      ಹಿಂತಿರುಗಿ socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_ಗಾತ್ರ = sspi_w_token[0].cb_ಬಫರ್ + sspi_w_token[1].cb_ಬಫರ್ +
      sspi_w_token[2].cb_ಬಫರ್;
    ಆದರೆ(etbuf_ಗಾತ್ರ > 0xffff)
      ಹಿಂತಿರುಗಿ socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_ಗಾತ್ರ);
    ಆದರೆ(!etbuf)
      ಹಿಂತಿರುಗಿ socks5_free(sspi_w_token, CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ);

    ಸ್ಮರಣೆ_ನಕಲು(etbuf, sspi_w_token[0].pv_ಬಫರ್, sspi_w_token[0].cb_ಬಫರ್);
    ಸ್ಮರಣೆ_ನಕಲು(etbuf + sspi_w_token[0].cb_ಬಫರ್,
           sspi_w_token[1].pv_ಬಫರ್, sspi_w_token[1].cb_ಬಫರ್);
    ಸ್ಮರಣೆ_ನಕಲು(etbuf + sspi_w_token[0].cb_ಬಫರ್ + sspi_w_token[1].cb_ಬಫರ್,
           sspi_w_token[2].pv_ಬಫರ್, sspi_w_token[2].cb_ಬಫರ್);

    (ಶೂನ್ಯ)socks5_free(sspi_w_token, CURLE_OK);

    us_ಉದ್ದಳತೆ = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)etbuf_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_ನಕಲು(socksreq + 2, &us_ಉದ್ದಳತೆ, ಗಾತ್ರ(ಗಿಡ್ಡ));
  }

  ಸಂಕೇತ_2 = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, socksreq, 4, FALSE, &written);
  ಆದರೆ(ಸಂಕೇತ_2 || (written != 4)) {
    failf(ದತ್ತಾಂಶ, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.socks5_gssapi_nec) {
    ಸ್ಮರಣೆ_ನಕಲು(socksreq, &gss_enc, 1);
    ಸಂಕೇತ_2 = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, socksreq, 1, FALSE, &written);
    ಆದರೆ(ಸಂಕೇತ_2 || (written != 1)) {
      failf(ದತ್ತಾಂಶ, "Failed to send SSPI encryption type.");
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸಂಕೇತ_2 = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, etbuf, etbuf_ಗಾತ್ರ, FALSE,
                             &written);
    curlx_free(etbuf);
    ಆದರೆ(ಸಂಕೇತ_2 || (etbuf_ಗಾತ್ರ != written)) {
      failf(ದತ್ತಾಂಶ, "Failed to send SSPI encryption type.");
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }
  }

  ಫಲಿತಾಂಶ = Curl_blockread_all(cf, ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)socksreq, 4, &actualread);
  ಆದರೆ(ಫಲಿತಾಂಶ || (actualread != 4)) {
    failf(ದತ್ತಾಂಶ, "Failed to receive SSPI encryption response.");
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ ? ಫಲಿತಾಂಶ : CURLE_COULDNT_CONNECT;
  }

  ಆದರೆ(socksreq[1] == 255) {
    failf(ದತ್ತಾಂಶ, "User was rejected by the SOCKS5 server (%u %u).",
          (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)socksreq[0], (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)socksreq[1]);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  ಆದರೆ(socksreq[1] != 2) {
    failf(ದತ್ತಾಂಶ, "Invalid SSPI encryption response type (%u %u).",
          (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)socksreq[0], (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)socksreq[1]);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  ಸ್ಮರಣೆ_ನಕಲು(&us_ಉದ್ದಳತೆ, socksreq + 2, ಗಾತ್ರ(ಗಿಡ್ಡ));
  us_ಉದ್ದಳತೆ = ntohs(us_ಉದ್ದಳತೆ);

  ಆದರೆ(!us_ಉದ್ದಳತೆ) {
    failf(ದತ್ತಾಂಶ, "Invalid zero-length SSPI encryption token.");
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cb_ಬಫರ್ = us_ಉದ್ದಳತೆ;
  sspi_w_token[0].pv_ಬಫರ್ = curlx_malloc(us_ಉದ್ದಳತೆ);
  ಆದರೆ(!sspi_w_token[0].pv_ಬಫರ್)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಫಲಿತಾಂಶ = Curl_blockread_all(cf, ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)sspi_w_token[0].pv_ಬಫರ್,
                              sspi_w_token[0].cb_ಬಫರ್, &actualread);

  ಆದರೆ(ಫಲಿತಾಂಶ || (actualread != us_ಉದ್ದಳತೆ)) {
    failf(ದತ್ತಾಂಶ, "Failed to receive SSPI encryption type.");
    ಹೋಗು fail;
  }

  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].ಬಫರ್_Type = SECBUFFER_ಹರಿವು;
    sspi_w_token[1].ಬಫರ್_Type = SECBUFFER_ದತ್ತಾಂಶ;
    sspi_w_token[1].cb_ಬಫರ್ = 0;
    sspi_w_token[1].pv_ಬಫರ್ = ಖಾಲಿ;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    ಸ್ಥಿತಿ_2 = Curl_pSecFn->Decrypt_ಸಂದೇಶ(sspi_context, &wrap_desc, 0, &qop);

    ಆದರೆ(check_sspi_err(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2, "DecryptMessage"))
      ಹೋಗು fail;

    ಆದರೆ(sspi_w_token[1].cb_ಬಫರ್ != 1) {
      failf(ದತ್ತಾಂಶ, "Invalid SSPI encryption response length (%lu).",
            (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)sspi_w_token[1].cb_ಬಫರ್);
      ಹೋಗು fail;
    }

    ಸ್ಮರಣೆ_ನಕಲು(socksreq, sspi_w_token[1].pv_ಬಫರ್, sspi_w_token[1].cb_ಬಫರ್);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(sspi_w_token[0].cb_ಬಫರ್ != 1) {
      failf(ದತ್ತಾಂಶ, "Invalid SSPI encryption response length (%lu).",
            (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)sspi_w_token[0].cb_ಬಫರ್);
      ಹೋಗು fail;
    }
    ಸ್ಮರಣೆ_ನಕಲು(socksreq, sspi_w_token[0].pv_ಬಫರ್, sspi_w_token[0].cb_ಬಫರ್);
  }
  curlx_free(sspi_w_token[0].pv_ಬಫರ್);

  infof(ದತ್ತಾಂಶ, "SOCKS5 access %s protection granted BUT NOT USED.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  cf->conn->socks5_gssapi_enctype = socksreq[0];
  ಹಿಂತಿರುಗಿ CURLE_OK;

fail:
  curlx_free(sspi_w_token[0].pv_ಬಫರ್);
  ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ Curl_creds *creds)
{
  ರಚನೆ connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode ಫಲಿತಾಂಶ;
  SECURITY_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  Sec_Pkg_Credentials_ಹೆಸರುಗಳು ಹೆಸರುಗಳು;
  ಅಕ್ಷರ *service_name = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sspi_ret_flags = 0;

  memset(&cred_handle, 0, ಗಾತ್ರ(cred_handle));
  memset(&sspi_context, 0, ಗಾತ್ರ(sspi_context));
  ಹೆಸರುಗಳು.sUserName = ಖಾಲಿ;

  ಫಲಿತಾಂಶ = socks5_sspi_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ, creds, &cred_handle, &service_name);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು error;

  ಫಲಿತಾಂಶ = socks5_sspi_loop(cf, ದತ್ತಾಂಶ, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು error;

  curlx_safefree(service_name);

  ಸ್ಥಿತಿ_2 = Curl_pSecFn->Query_Credentials_ಗುಣಲಕ್ಷಣಗಳು(&cred_handle,
                                                   SECPKG_CRED_ATTR_ಹೆಸರುಗಳು,
                                                   &ಹೆಸರುಗಳು);
  ಆದರೆ(check_sspi_err(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2, "QueryCredentialAttributes")) {
    failf(ದತ್ತಾಂಶ, "Failed to determine username.");
    ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
    ಹೋಗು error;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    VERBOSE(ಅಕ್ಷರ *user_utf8 = curlx_convert_tchar_ಗಮ್ಯ_UTF8(ಹೆಸರುಗಳು.sUserName));
    infof(ದತ್ತಾಂಶ, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->Free_Context_ಬಫರ್(ಹೆಸರುಗಳು.sUserName);
    ಹೆಸರುಗಳು.sUserName = ಖಾಲಿ;
  }

  ಫಲಿತಾಂಶ = socks5_sspi_encrypt(cf, ದತ್ತಾಂಶ, &sspi_context, sspi_ret_flags);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು error;

  (ಶೂನ್ಯ)curlx_nonblock(sock, TRUE);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  ಹಿಂತಿರುಗಿ CURLE_OK;

error:
  (ಶೂನ್ಯ)curlx_nonblock(sock, TRUE);
  curlx_free(service_name);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  ಆದರೆ(ಹೆಸರುಗಳು.sUserName)
    Curl_pSecFn->Free_Context_ಬಫರ್(ಹೆಸರುಗಳು.sUserName);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ
