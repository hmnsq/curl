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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_PROXY)

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "strerror.h"
#అన్తర్భావ "curlx/nonblock.h"
#అన్తర్భావ "socks.h"
#అన్తర్భావ "curl_sspi.h"
#అన్తర్భావ "curlx/multibyte.h"

/*
 * Helper SSPI error functions.
 */
స్థిర పూర్ణాంకం check_sspi_err(నిర్మాణం Curl_easy *దత్తాంశం,
                          SECURITY_స్థితి స్థితి_2,
                          స్థిరరాశి అక్షరం *function)
{
  ఒకవేళ(స్థితి_2 != SEC_E_OK &&
     స్థితి_2 != SEC_I_COMPLETE_AND_CONTINUE &&
     స్థితి_2 != SEC_I_COMPLETE_NEEDED &&
     స్థితి_2 != SEC_I_CONTINUE_NEEDED) {
    అక్షరం బఫరు[STRERROR_LEN];
    failf(దత్తాంశం, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(స్థితి_2, బఫరు, పరిమాణం(బఫరు)));
    తిరిగి 1;
  }
  తిరిగి 0;
}

/* This is the SSPI-using version of this function */
స్థిర CURLcode socks5_sspi_అమరిక(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం Curl_creds *creds,
                                  CredHandle *cred_handle,
                                  అక్షరం **service_namep)
{
  నిర్మాణం connectdata *conn = cf->conn;
  స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  SECURITY_స్థితి స్థితి_2;

  /* prepare service name */
  ఒకవేళ(strchr(service, '/'))
    *service_namep = curlx_strdup(service);
  లేకపోతే
    *service_namep = curl_maprintf("%s/%s",
                                   service, conn->socks_proxy.peer->hostname);
  ఒకవేళ(!*service_namep)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  స్థితి_2 =
    Curl_pSecFn->AcquireCredentialsHandle(ఖాళీ, CURL_UNCONST(పాఠ్యం_2("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ,
                                          cred_handle, ఖాళీ);

  ఒకవేళ(check_sspi_err(దత్తాంశం, స్థితి_2, "AcquireCredentialsHandle")) {
    failf(దత్తాంశం, "Failed to acquire credentials.");
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode socks5_free_token(Sec_బఫరు *send_token,
                                  CURLcode ఫలితం)
{
  ఒకవేళ(send_token->pv_బఫరు) {
    Curl_pSecFn->Free_Context_బఫరు(send_token->pv_బఫరు);
    send_token->pv_బఫరు = ఖాళీ;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode socks5_sspi_loop(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 అక్షరం *service_name,
                                 గుర్తులేని దీర్ఘ *sspi_ret_flagsp)
{
  నిర్మాణం connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode ఫలితం = CURLE_OK;
  CURLcode సంకేతం_2;
  SECURITY_స్థితి స్థితి_2;
  Sec_బఫరు sspi_send_token, sspi_recv_token;
  Sec_బఫరు_Desc ఆదానం_desc, output_desc;
  PCtxtHandle context_handle = ఖాళీ;
  గుర్తులేని పొట్టి us_నిడివి;
  size_t actualread;
  size_t written;
  గుర్తులేని అక్షరం socksreq[4];

  ఆదానం_desc.cBuffers = 1;
  ఆదానం_desc.pBuffers = &sspi_recv_token;
  ఆదానం_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;

  sspi_recv_token.బఫరు_Type = SECBUFFER_TOKEN;
  sspi_recv_token.cb_బఫరు = 0;
  sspi_recv_token.pv_బఫరు = ఖాళీ;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;

  sspi_send_token.బఫరు_Type = SECBUFFER_TOKEN;
  sspi_send_token.cb_బఫరు = 0;
  sspi_send_token.pv_బఫరు = ఖాళీ;

  (శూన్య)curlx_nonblock(sock, FALSE);

  కొరకు(;;) {
    TCHAR *sname = curlx_convert_UTF8_గమ్యం_tchar(service_name);
    ఒకవేళ(!sname) {
      curlx_free(sspi_recv_token.pv_బఫరు);
      తిరిగి socks5_free_token(&sspi_send_token, CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం);
    }

    స్థితి_2 =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_జ్ఞాపకం |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &ఆదానం_desc, 0,
                                             sspi_context,
                                             &output_desc,
                                             sspi_ret_flagsp, ఖాళీ);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pv_బఫరు);
    sspi_recv_token.cb_బఫరు = 0;

    ఒకవేళ(check_sspi_err(దత్తాంశం, స్థితి_2, "InitializeSecurityContext")) {
      failf(దత్తాంశం, "Failed to initialize security context.");
      తిరిగి socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    ఒకవేళ(sspi_send_token.cb_బఫరు) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      ఒకవేళ(sspi_send_token.cb_బఫరు > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        తిరిగి socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_నిడివి = htons((గుర్తులేని పొట్టి)sspi_send_token.cb_బఫరు);
      జ్ఞాపకం_నకలు(socksreq + 2, &us_నిడివి, పరిమాణం(పొట్టి));

      సంకేతం_2 = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, socksreq, 4, FALSE, &written);
      ఒకవేళ(సంకేతం_2 || (written != 4)) {
        failf(దత్తాంశం, "Failed to send SSPI authentication request.");
        తిరిగి socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      సంకేతం_2 = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం,
                               sspi_send_token.pv_బఫరు,
                               sspi_send_token.cb_బఫరు, FALSE, &written);
      ఒకవేళ(సంకేతం_2 || (sspi_send_token.cb_బఫరు != written)) {
        failf(దత్తాంశం, "Failed to send SSPI authentication token.");
        తిరిగి socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
    }

    ఒకవేళ(sspi_send_token.pv_బఫరు)
      socks5_free_token(&sspi_send_token, CURLE_OK);
    sspi_send_token.cb_బఫరు = 0;

    curlx_safefree(sspi_recv_token.pv_బఫరు);
    sspi_recv_token.cb_బఫరు = 0;

    ఒకవేళ(స్థితి_2 != SEC_I_CONTINUE_NEEDED)
      ఆపు;

    ఫలితం = Curl_blockread_all(cf, దత్తాంశం, (అక్షరం *)socksreq, 4, &actualread);
    ఒకవేళ(ఫలితం || (actualread != 4)) {
      failf(దత్తాంశం, "Failed to receive SSPI authentication response.");
      తిరిగి ఫలితం ? ఫలితం : CURLE_COULDNT_CONNECT;
    }

    ఒకవేళ(socksreq[1] == 255) {
      failf(దత్తాంశం, "User was rejected by the SOCKS5 server (%u %u).",
            (గుర్తులేని పూర్ణాంకం)socksreq[0], (గుర్తులేని పూర్ణాంకం)socksreq[1]);
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    ఒకవేళ(socksreq[1] != 1) {
      failf(దత్తాంశం, "Invalid SSPI authentication response type (%u %u).",
            (గుర్తులేని పూర్ణాంకం)socksreq[0], (గుర్తులేని పూర్ణాంకం)socksreq[1]);
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    జ్ఞాపకం_నకలు(&us_నిడివి, socksreq + 2, పరిమాణం(పొట్టి));
    us_నిడివి = ntohs(us_నిడివి);

    ఒకవేళ(!us_నిడివి) {
      failf(దత్తాంశం, "Invalid zero-length SSPI authentication token.");
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cb_బఫరు = us_నిడివి;
    sspi_recv_token.pv_బఫరు = curlx_malloc(us_నిడివి);

    ఒకవేళ(!sspi_recv_token.pv_బఫరు)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    ఫలితం = Curl_blockread_all(cf, దత్తాంశం, (అక్షరం *)sspi_recv_token.pv_బఫరు,
                                sspi_recv_token.cb_బఫరు, &actualread);

    ఒకవేళ(ఫలితం || (actualread != us_నిడివి)) {
      failf(దత్తాంశం, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pv_బఫరు);
      తిరిగి ఫలితం ? ఫలితం : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode socks5_free(Sec_బఫరు *sspi_w_token,
                            CURLcode ఫలితం)
{
  curlx_safefree(sspi_w_token[0].pv_బఫరు);
  curlx_safefree(sspi_w_token[1].pv_బఫరు);
  curlx_safefree(sspi_w_token[2].pv_బఫరు);
  తిరిగి ఫలితం;
}

స్థిర CURLcode socks5_sspi_encrypt(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    CtxtHandle *sspi_context,
                                    గుర్తులేని దీర్ఘ sspi_ret_flags)
{
  CURLcode ఫలితం = CURLE_OK;
  CURLcode సంకేతం_2;
  SECURITY_స్థితి స్థితి_2;
  గుర్తులేని అక్షరం gss_enc;
  Sec_బఫరు sspi_w_token[3];
  Sec_బఫరు_Desc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  గుర్తులేని పొట్టి us_నిడివి;
  గుర్తులేని దీర్ఘ qop;
  గుర్తులేని అక్షరం socksreq[4];
  uint8_t *etbuf = ఖాళీ;
  size_t etbuf_కొలత = 0;
  size_t actualread;
  size_t written;

  gss_enc = 0;
  ఒకవేళ(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  లేకపోతే ఒకవేళ(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(దత్తాంశం, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pv_బఫరు =
    sspi_w_token[1].pv_బఫరు =
    sspi_w_token[2].pv_బఫరు = ఖాళీ;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ul_సంస్కరణ = SECBUFFER_సంస్కరణ;

  socksreq[0] = 1;
  socksreq[1] = 2;

  ఒకవేళ(దత్తాంశం->సమితి.socks5_gssapi_nec) {
    us_నిడివి = htons((గుర్తులేని పొట్టి)1);
    జ్ఞాపకం_నకలు(socksreq + 2, &us_నిడివి, పరిమాణం(పొట్టి));
  }
  లేకపోతే {
    స్థితి_2 = Curl_pSecFn->Query_Context_లక్షణాలు(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    ఒకవేళ(check_sspi_err(దత్తాంశం, స్థితి_2, "QueryContextAttributes")) {
      failf(దత్తాంశం, "Failed to query security context attributes.");
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cb_బఫరు = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].బఫరు_Type = SECBUFFER_TOKEN;
    sspi_w_token[0].pv_బఫరు = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    ఒకవేళ(!sspi_w_token[0].pv_బఫరు)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    sspi_w_token[1].cb_బఫరు = 1;
    sspi_w_token[1].బఫరు_Type = SECBUFFER_దత్తాంశం;
    sspi_w_token[1].pv_బఫరు = curlx_malloc(1);
    ఒకవేళ(!sspi_w_token[1].pv_బఫరు)
      తిరిగి socks5_free(sspi_w_token, CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం);

    జ్ఞాపకం_నకలు(sspi_w_token[1].pv_బఫరు, &gss_enc, 1);
    sspi_w_token[2].బఫరు_Type = SECBUFFER_PADDING;
    sspi_w_token[2].cb_బఫరు = sspi_sizes.cb_ఖండం_కొలత;
    sspi_w_token[2].pv_బఫరు = curlx_malloc(sspi_sizes.cb_ఖండం_కొలత);
    ఒకవేళ(!sspi_w_token[2].pv_బఫరు)
      తిరిగి socks5_free(sspi_w_token, CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం);

    స్థితి_2 = Curl_pSecFn->Encrypt_సందేశం(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    ఒకవేళ(check_sspi_err(దత్తాంశం, స్థితి_2, "EncryptMessage"))
      తిరిగి socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_కొలత = sspi_w_token[0].cb_బఫరు + sspi_w_token[1].cb_బఫరు +
      sspi_w_token[2].cb_బఫరు;
    ఒకవేళ(etbuf_కొలత > 0xffff)
      తిరిగి socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_కొలత);
    ఒకవేళ(!etbuf)
      తిరిగి socks5_free(sspi_w_token, CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం);

    జ్ఞాపకం_నకలు(etbuf, sspi_w_token[0].pv_బఫరు, sspi_w_token[0].cb_బఫరు);
    జ్ఞాపకం_నకలు(etbuf + sspi_w_token[0].cb_బఫరు,
           sspi_w_token[1].pv_బఫరు, sspi_w_token[1].cb_బఫరు);
    జ్ఞాపకం_నకలు(etbuf + sspi_w_token[0].cb_బఫరు + sspi_w_token[1].cb_బఫరు,
           sspi_w_token[2].pv_బఫరు, sspi_w_token[2].cb_బఫరు);

    (శూన్య)socks5_free(sspi_w_token, CURLE_OK);

    us_నిడివి = htons((గుర్తులేని పొట్టి)etbuf_కొలత);
    జ్ఞాపకం_నకలు(socksreq + 2, &us_నిడివి, పరిమాణం(పొట్టి));
  }

  సంకేతం_2 = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, socksreq, 4, FALSE, &written);
  ఒకవేళ(సంకేతం_2 || (written != 4)) {
    failf(దత్తాంశం, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  ఒకవేళ(దత్తాంశం->సమితి.socks5_gssapi_nec) {
    జ్ఞాపకం_నకలు(socksreq, &gss_enc, 1);
    సంకేతం_2 = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, socksreq, 1, FALSE, &written);
    ఒకవేళ(సంకేతం_2 || (written != 1)) {
      failf(దత్తాంశం, "Failed to send SSPI encryption type.");
      తిరిగి CURLE_COULDNT_CONNECT;
    }
  }
  లేకపోతే {
    సంకేతం_2 = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, etbuf, etbuf_కొలత, FALSE,
                             &written);
    curlx_free(etbuf);
    ఒకవేళ(సంకేతం_2 || (etbuf_కొలత != written)) {
      failf(దత్తాంశం, "Failed to send SSPI encryption type.");
      తిరిగి CURLE_COULDNT_CONNECT;
    }
  }

  ఫలితం = Curl_blockread_all(cf, దత్తాంశం, (అక్షరం *)socksreq, 4, &actualread);
  ఒకవేళ(ఫలితం || (actualread != 4)) {
    failf(దత్తాంశం, "Failed to receive SSPI encryption response.");
    తిరిగి ఫలితం ? ఫలితం : CURLE_COULDNT_CONNECT;
  }

  ఒకవేళ(socksreq[1] == 255) {
    failf(దత్తాంశం, "User was rejected by the SOCKS5 server (%u %u).",
          (గుర్తులేని పూర్ణాంకం)socksreq[0], (గుర్తులేని పూర్ణాంకం)socksreq[1]);
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  ఒకవేళ(socksreq[1] != 2) {
    failf(దత్తాంశం, "Invalid SSPI encryption response type (%u %u).",
          (గుర్తులేని పూర్ణాంకం)socksreq[0], (గుర్తులేని పూర్ణాంకం)socksreq[1]);
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  జ్ఞాపకం_నకలు(&us_నిడివి, socksreq + 2, పరిమాణం(పొట్టి));
  us_నిడివి = ntohs(us_నిడివి);

  ఒకవేళ(!us_నిడివి) {
    failf(దత్తాంశం, "Invalid zero-length SSPI encryption token.");
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cb_బఫరు = us_నిడివి;
  sspi_w_token[0].pv_బఫరు = curlx_malloc(us_నిడివి);
  ఒకవేళ(!sspi_w_token[0].pv_బఫరు)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఫలితం = Curl_blockread_all(cf, దత్తాంశం, (అక్షరం *)sspi_w_token[0].pv_బఫరు,
                              sspi_w_token[0].cb_బఫరు, &actualread);

  ఒకవేళ(ఫలితం || (actualread != us_నిడివి)) {
    failf(దత్తాంశం, "Failed to receive SSPI encryption type.");
    వెళ్ళు fail;
  }

  ఒకవేళ(!దత్తాంశం->సమితి.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].బఫరు_Type = SECBUFFER_ప్రవాహం;
    sspi_w_token[1].బఫరు_Type = SECBUFFER_దత్తాంశం;
    sspi_w_token[1].cb_బఫరు = 0;
    sspi_w_token[1].pv_బఫరు = ఖాళీ;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    స్థితి_2 = Curl_pSecFn->Decrypt_సందేశం(sspi_context, &wrap_desc, 0, &qop);

    ఒకవేళ(check_sspi_err(దత్తాంశం, స్థితి_2, "DecryptMessage"))
      వెళ్ళు fail;

    ఒకవేళ(sspi_w_token[1].cb_బఫరు != 1) {
      failf(దత్తాంశం, "Invalid SSPI encryption response length (%lu).",
            (గుర్తులేని దీర్ఘ)sspi_w_token[1].cb_బఫరు);
      వెళ్ళు fail;
    }

    జ్ఞాపకం_నకలు(socksreq, sspi_w_token[1].pv_బఫరు, sspi_w_token[1].cb_బఫరు);
  }
  లేకపోతే {
    ఒకవేళ(sspi_w_token[0].cb_బఫరు != 1) {
      failf(దత్తాంశం, "Invalid SSPI encryption response length (%lu).",
            (గుర్తులేని దీర్ఘ)sspi_w_token[0].cb_బఫరు);
      వెళ్ళు fail;
    }
    జ్ఞాపకం_నకలు(socksreq, sspi_w_token[0].pv_బఫరు, sspi_w_token[0].cb_బఫరు);
  }
  curlx_free(sspi_w_token[0].pv_బఫరు);

  infof(దత్తాంశం, "SOCKS5 access %s protection granted BUT NOT USED.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  cf->conn->socks5_gssapi_enctype = socksreq[0];
  తిరిగి CURLE_OK;

fail:
  curlx_free(sspi_w_token[0].pv_బఫరు);
  తిరిగి CURLE_COULDNT_CONNECT;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం Curl_creds *creds)
{
  నిర్మాణం connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode ఫలితం;
  SECURITY_స్థితి స్థితి_2;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  Sec_Pkg_Credentials_పేర్లు పేర్లు;
  అక్షరం *service_name = ఖాళీ;
  గుర్తులేని దీర్ఘ sspi_ret_flags = 0;

  memset(&cred_handle, 0, పరిమాణం(cred_handle));
  memset(&sspi_context, 0, పరిమాణం(sspi_context));
  పేర్లు.sUserName = ఖాళీ;

  ఫలితం = socks5_sspi_అమరిక(cf, దత్తాంశం, creds, &cred_handle, &service_name);
  ఒకవేళ(ఫలితం)
    వెళ్ళు error;

  ఫలితం = socks5_sspi_loop(cf, దత్తాంశం, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  ఒకవేళ(ఫలితం)
    వెళ్ళు error;

  curlx_safefree(service_name);

  స్థితి_2 = Curl_pSecFn->Query_Credentials_లక్షణాలు(&cred_handle,
                                                   SECPKG_CRED_ATTR_పేర్లు,
                                                   &పేర్లు);
  ఒకవేళ(check_sspi_err(దత్తాంశం, స్థితి_2, "QueryCredentialAttributes")) {
    failf(దత్తాంశం, "Failed to determine username.");
    ఫలితం = CURLE_COULDNT_CONNECT;
    వెళ్ళు error;
  }
  లేకపోతే {
    VERBOSE(అక్షరం *user_utf8 = curlx_convert_tchar_గమ్యం_UTF8(పేర్లు.sUserName));
    infof(దత్తాంశం, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->Free_Context_బఫరు(పేర్లు.sUserName);
    పేర్లు.sUserName = ఖాళీ;
  }

  ఫలితం = socks5_sspi_encrypt(cf, దత్తాంశం, &sspi_context, sspi_ret_flags);
  ఒకవేళ(ఫలితం)
    వెళ్ళు error;

  (శూన్య)curlx_nonblock(sock, TRUE);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  తిరిగి CURLE_OK;

error:
  (శూన్య)curlx_nonblock(sock, TRUE);
  curlx_free(service_name);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  ఒకవేళ(పేర్లు.sUserName)
    Curl_pSecFn->Free_Context_బఫరు(పేర్లు.sUserName);
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు
