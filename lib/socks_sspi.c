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
#অন্তর্ভাব "curl_setup.h"

#যদি defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_PROXY)

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "strerror.h"
#অন্তর্ভাব "curlx/nonblock.h"
#অন্তর্ভাব "socks.h"
#অন্তর্ভাব "curl_sspi.h"
#অন্তর্ভাব "curlx/multibyte.h"

/*
 * Helper SSPI error functions.
 */
স্থির পূর্ণসংখ্যা check_sspi_err(কাঠামো Curl_easy *তথ্য,
                          SECURITY_STATUS status,
                          ধ্রুবক অক্ষর *function)
{
  যদি(status != SEC_E_OK &&
     status != SEC_I_COMPLETE_AND_CONTINUE &&
     status != SEC_I_COMPLETE_NEEDED &&
     status != SEC_I_CONTINUE_NEEDED) {
    অক্ষর বাফার[STRERROR_LEN];
    failf(তথ্য, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(status, বাফার, আকার(বাফার)));
    ফেরত 1;
  }
  ফেরত 0;
}

/* This is the SSPI-using version of this function */
স্থির CURLcode socks5_sspi_setup(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো Curl_creds *creds,
                                  CredHandle *cred_handle,
                                  অক্ষর **service_namep)
{
  কাঠামো connectdata *conn = cf->conn;
  ধ্রুবক অক্ষর *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  SECURITY_STATUS status;

  /* prepare service name */
  যদি(strchr(service, '/'))
    *service_namep = curlx_strdup(service);
  অন্যথা
    *service_namep = curl_maprintf("%s/%s",
                                   service, conn->socks_proxy.peer->hostname);
  যদি(!*service_namep)
    ফেরত CURLE_OUT_OF_MEMORY;

  status =
    Curl_pSecFn->AcquireCredentialsHandle(ফাঁকা, CURL_UNCONST(TEXT("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা,
                                          cred_handle, ফাঁকা);

  যদি(check_sspi_err(তথ্য, status, "AcquireCredentialsHandle")) {
    failf(তথ্য, "Failed to acquire credentials.");
    ফেরত CURLE_COULDNT_CONNECT;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode socks5_free_token(Sec_বাফার *send_token,
                                  CURLcode ফলাফল)
{
  যদি(send_token->pv_বাফার) {
    Curl_pSecFn->Free_Context_বাফার(send_token->pv_বাফার);
    send_token->pv_বাফার = ফাঁকা;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode socks5_sspi_loop(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 অক্ষর *service_name,
                                 unsigned long *sspi_ret_flagsp)
{
  কাঠামো connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode ফলাফল = CURLE_OK;
  CURLcode code;
  SECURITY_STATUS status;
  Sec_বাফার sspi_send_token, sspi_recv_token;
  Sec_বাফার_Desc প্রবেশ_desc, output_desc;
  PCtxtHandle context_handle = ফাঁকা;
  unsigned short us_দৈর্ঘ্য;
  size_t actualread;
  size_t written;
  unsigned অক্ষর socksreq[4];

  প্রবেশ_desc.cBuffers = 1;
  প্রবেশ_desc.pBuffers = &sspi_recv_token;
  প্রবেশ_desc.ulVersion = SECBUFFER_VERSION;

  sspi_recv_token.বাফার_Type = SECBUFFER_TOKEN;
  sspi_recv_token.cb_বাফার = 0;
  sspi_recv_token.pv_বাফার = ফাঁকা;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ulVersion = SECBUFFER_VERSION;

  sspi_send_token.বাফার_Type = SECBUFFER_TOKEN;
  sspi_send_token.cb_বাফার = 0;
  sspi_send_token.pv_বাফার = ফাঁকা;

  (শূন্য)curlx_nonblock(sock, FALSE);

  জন্য(;;) {
    TCHAR *sname = curlx_convert_UTF8_to_tchar(service_name);
    যদি(!sname) {
      curlx_free(sspi_recv_token.pv_বাফার);
      ফেরত socks5_free_token(&sspi_send_token, CURLE_OUT_OF_MEMORY);
    }

    status =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_MEMORY |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &প্রবেশ_desc, 0,
                                             sspi_context,
                                             &output_desc,
                                             sspi_ret_flagsp, ফাঁকা);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pv_বাফার);
    sspi_recv_token.cb_বাফার = 0;

    যদি(check_sspi_err(তথ্য, status, "InitializeSecurityContext")) {
      failf(তথ্য, "Failed to initialize security context.");
      ফেরত socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    যদি(sspi_send_token.cb_বাফার) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      যদি(sspi_send_token.cb_বাফার > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        ফেরত socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_দৈর্ঘ্য = htons((unsigned short)sspi_send_token.cb_বাফার);
      memcpy(socksreq + 2, &us_দৈর্ঘ্য, আকার(short));

      code = Curl_conn_cf_send(cf->পরবর্তী, তথ্য, socksreq, 4, FALSE, &written);
      যদি(code || (written != 4)) {
        failf(তথ্য, "Failed to send SSPI authentication request.");
        ফেরত socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      code = Curl_conn_cf_send(cf->পরবর্তী, তথ্য,
                               sspi_send_token.pv_বাফার,
                               sspi_send_token.cb_বাফার, FALSE, &written);
      যদি(code || (sspi_send_token.cb_বাফার != written)) {
        failf(তথ্য, "Failed to send SSPI authentication token.");
        ফেরত socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
    }

    যদি(sspi_send_token.pv_বাফার)
      socks5_free_token(&sspi_send_token, CURLE_OK);
    sspi_send_token.cb_বাফার = 0;

    curlx_safefree(sspi_recv_token.pv_বাফার);
    sspi_recv_token.cb_বাফার = 0;

    যদি(status != SEC_I_CONTINUE_NEEDED)
      বিরতি;

    ফলাফল = Curl_blockread_all(cf, তথ্য, (অক্ষর *)socksreq, 4, &actualread);
    যদি(ফলাফল || (actualread != 4)) {
      failf(তথ্য, "Failed to receive SSPI authentication response.");
      ফেরত ফলাফল ? ফলাফল : CURLE_COULDNT_CONNECT;
    }

    যদি(socksreq[1] == 255) {
      failf(তথ্য, "User was rejected by the SOCKS5 server (%u %u).",
            (unsigned পূর্ণসংখ্যা)socksreq[0], (unsigned পূর্ণসংখ্যা)socksreq[1]);
      ফেরত CURLE_COULDNT_CONNECT;
    }

    যদি(socksreq[1] != 1) {
      failf(তথ্য, "Invalid SSPI authentication response type (%u %u).",
            (unsigned পূর্ণসংখ্যা)socksreq[0], (unsigned পূর্ণসংখ্যা)socksreq[1]);
      ফেরত CURLE_COULDNT_CONNECT;
    }

    memcpy(&us_দৈর্ঘ্য, socksreq + 2, আকার(short));
    us_দৈর্ঘ্য = ntohs(us_দৈর্ঘ্য);

    যদি(!us_দৈর্ঘ্য) {
      failf(তথ্য, "Invalid zero-length SSPI authentication token.");
      ফেরত CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cb_বাফার = us_দৈর্ঘ্য;
    sspi_recv_token.pv_বাফার = curlx_malloc(us_দৈর্ঘ্য);

    যদি(!sspi_recv_token.pv_বাফার)
      ফেরত CURLE_OUT_OF_MEMORY;

    ফলাফল = Curl_blockread_all(cf, তথ্য, (অক্ষর *)sspi_recv_token.pv_বাফার,
                                sspi_recv_token.cb_বাফার, &actualread);

    যদি(ফলাফল || (actualread != us_দৈর্ঘ্য)) {
      failf(তথ্য, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pv_বাফার);
      ফেরত ফলাফল ? ফলাফল : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode socks5_free(Sec_বাফার *sspi_w_token,
                            CURLcode ফলাফল)
{
  curlx_safefree(sspi_w_token[0].pv_বাফার);
  curlx_safefree(sspi_w_token[1].pv_বাফার);
  curlx_safefree(sspi_w_token[2].pv_বাফার);
  ফেরত ফলাফল;
}

স্থির CURLcode socks5_sspi_encrypt(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    CtxtHandle *sspi_context,
                                    unsigned long sspi_ret_flags)
{
  CURLcode ফলাফল = CURLE_OK;
  CURLcode code;
  SECURITY_STATUS status;
  unsigned অক্ষর gss_enc;
  Sec_বাফার sspi_w_token[3];
  Sec_বাফার_Desc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  unsigned short us_দৈর্ঘ্য;
  unsigned long qop;
  unsigned অক্ষর socksreq[4];
  uint8_t *etbuf = ফাঁকা;
  size_t etbuf_আকার = 0;
  size_t actualread;
  size_t written;

  gss_enc = 0;
  যদি(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  অন্যথা যদি(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(তথ্য, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pv_বাফার =
    sspi_w_token[1].pv_বাফার =
    sspi_w_token[2].pv_বাফার = ফাঁকা;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ulVersion = SECBUFFER_VERSION;

  socksreq[0] = 1;
  socksreq[1] = 2;

  যদি(তথ্য->set.socks5_gssapi_nec) {
    us_দৈর্ঘ্য = htons((unsigned short)1);
    memcpy(socksreq + 2, &us_দৈর্ঘ্য, আকার(short));
  }
  অন্যথা {
    status = Curl_pSecFn->QueryContextAttributes(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    যদি(check_sspi_err(তথ্য, status, "QueryContextAttributes")) {
      failf(তথ্য, "Failed to query security context attributes.");
      ফেরত CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cb_বাফার = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].বাফার_Type = SECBUFFER_TOKEN;
    sspi_w_token[0].pv_বাফার = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    যদি(!sspi_w_token[0].pv_বাফার)
      ফেরত CURLE_OUT_OF_MEMORY;

    sspi_w_token[1].cb_বাফার = 1;
    sspi_w_token[1].বাফার_Type = SECBUFFER_তথ্য;
    sspi_w_token[1].pv_বাফার = curlx_malloc(1);
    যদি(!sspi_w_token[1].pv_বাফার)
      ফেরত socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    memcpy(sspi_w_token[1].pv_বাফার, &gss_enc, 1);
    sspi_w_token[2].বাফার_Type = SECBUFFER_PADDING;
    sspi_w_token[2].cb_বাফার = sspi_sizes.cb_Block_আকার;
    sspi_w_token[2].pv_বাফার = curlx_malloc(sspi_sizes.cb_Block_আকার);
    যদি(!sspi_w_token[2].pv_বাফার)
      ফেরত socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    status = Curl_pSecFn->EncryptMessage(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    যদি(check_sspi_err(তথ্য, status, "EncryptMessage"))
      ফেরত socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_আকার = sspi_w_token[0].cb_বাফার + sspi_w_token[1].cb_বাফার +
      sspi_w_token[2].cb_বাফার;
    যদি(etbuf_আকার > 0xffff)
      ফেরত socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_আকার);
    যদি(!etbuf)
      ফেরত socks5_free(sspi_w_token, CURLE_OUT_OF_MEMORY);

    memcpy(etbuf, sspi_w_token[0].pv_বাফার, sspi_w_token[0].cb_বাফার);
    memcpy(etbuf + sspi_w_token[0].cb_বাফার,
           sspi_w_token[1].pv_বাফার, sspi_w_token[1].cb_বাফার);
    memcpy(etbuf + sspi_w_token[0].cb_বাফার + sspi_w_token[1].cb_বাফার,
           sspi_w_token[2].pv_বাফার, sspi_w_token[2].cb_বাফার);

    (শূন্য)socks5_free(sspi_w_token, CURLE_OK);

    us_দৈর্ঘ্য = htons((unsigned short)etbuf_আকার);
    memcpy(socksreq + 2, &us_দৈর্ঘ্য, আকার(short));
  }

  code = Curl_conn_cf_send(cf->পরবর্তী, তথ্য, socksreq, 4, FALSE, &written);
  যদি(code || (written != 4)) {
    failf(তথ্য, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    ফেরত CURLE_COULDNT_CONNECT;
  }

  যদি(তথ্য->set.socks5_gssapi_nec) {
    memcpy(socksreq, &gss_enc, 1);
    code = Curl_conn_cf_send(cf->পরবর্তী, তথ্য, socksreq, 1, FALSE, &written);
    যদি(code || (written != 1)) {
      failf(তথ্য, "Failed to send SSPI encryption type.");
      ফেরত CURLE_COULDNT_CONNECT;
    }
  }
  অন্যথা {
    code = Curl_conn_cf_send(cf->পরবর্তী, তথ্য, etbuf, etbuf_আকার, FALSE,
                             &written);
    curlx_free(etbuf);
    যদি(code || (etbuf_আকার != written)) {
      failf(তথ্য, "Failed to send SSPI encryption type.");
      ফেরত CURLE_COULDNT_CONNECT;
    }
  }

  ফলাফল = Curl_blockread_all(cf, তথ্য, (অক্ষর *)socksreq, 4, &actualread);
  যদি(ফলাফল || (actualread != 4)) {
    failf(তথ্য, "Failed to receive SSPI encryption response.");
    ফেরত ফলাফল ? ফলাফল : CURLE_COULDNT_CONNECT;
  }

  যদি(socksreq[1] == 255) {
    failf(তথ্য, "User was rejected by the SOCKS5 server (%u %u).",
          (unsigned পূর্ণসংখ্যা)socksreq[0], (unsigned পূর্ণসংখ্যা)socksreq[1]);
    ফেরত CURLE_COULDNT_CONNECT;
  }

  যদি(socksreq[1] != 2) {
    failf(তথ্য, "Invalid SSPI encryption response type (%u %u).",
          (unsigned পূর্ণসংখ্যা)socksreq[0], (unsigned পূর্ণসংখ্যা)socksreq[1]);
    ফেরত CURLE_COULDNT_CONNECT;
  }

  memcpy(&us_দৈর্ঘ্য, socksreq + 2, আকার(short));
  us_দৈর্ঘ্য = ntohs(us_দৈর্ঘ্য);

  যদি(!us_দৈর্ঘ্য) {
    failf(তথ্য, "Invalid zero-length SSPI encryption token.");
    ফেরত CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cb_বাফার = us_দৈর্ঘ্য;
  sspi_w_token[0].pv_বাফার = curlx_malloc(us_দৈর্ঘ্য);
  যদি(!sspi_w_token[0].pv_বাফার)
    ফেরত CURLE_OUT_OF_MEMORY;

  ফলাফল = Curl_blockread_all(cf, তথ্য, (অক্ষর *)sspi_w_token[0].pv_বাফার,
                              sspi_w_token[0].cb_বাফার, &actualread);

  যদি(ফলাফল || (actualread != us_দৈর্ঘ্য)) {
    failf(তথ্য, "Failed to receive SSPI encryption type.");
    যাও fail;
  }

  যদি(!তথ্য->set.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].বাফার_Type = SECBUFFER_STREAM;
    sspi_w_token[1].বাফার_Type = SECBUFFER_তথ্য;
    sspi_w_token[1].cb_বাফার = 0;
    sspi_w_token[1].pv_বাফার = ফাঁকা;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    status = Curl_pSecFn->DecryptMessage(sspi_context, &wrap_desc, 0, &qop);

    যদি(check_sspi_err(তথ্য, status, "DecryptMessage"))
      যাও fail;

    যদি(sspi_w_token[1].cb_বাফার != 1) {
      failf(তথ্য, "Invalid SSPI encryption response length (%lu).",
            (unsigned long)sspi_w_token[1].cb_বাফার);
      যাও fail;
    }

    memcpy(socksreq, sspi_w_token[1].pv_বাফার, sspi_w_token[1].cb_বাফার);
  }
  অন্যথা {
    যদি(sspi_w_token[0].cb_বাফার != 1) {
      failf(তথ্য, "Invalid SSPI encryption response length (%lu).",
            (unsigned long)sspi_w_token[0].cb_বাফার);
      যাও fail;
    }
    memcpy(socksreq, sspi_w_token[0].pv_বাফার, sspi_w_token[0].cb_বাফার);
  }
  curlx_free(sspi_w_token[0].pv_বাফার);

  infof(তথ্য, "SOCKS5 access %s protection granted BUT NOT USED.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  cf->conn->socks5_gssapi_enctype = socksreq[0];
  ফেরত CURLE_OK;

fail:
  curlx_free(sspi_w_token[0].pv_বাফার);
  ফেরত CURLE_COULDNT_CONNECT;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      কাঠামো Curl_creds *creds)
{
  কাঠামো connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode ফলাফল;
  SECURITY_STATUS status;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  SecPkgCredentials_Names names;
  অক্ষর *service_name = ফাঁকা;
  unsigned long sspi_ret_flags = 0;

  memset(&cred_handle, 0, আকার(cred_handle));
  memset(&sspi_context, 0, আকার(sspi_context));
  names.sUserName = ফাঁকা;

  ফলাফল = socks5_sspi_setup(cf, তথ্য, creds, &cred_handle, &service_name);
  যদি(ফলাফল)
    যাও error;

  ফলাফল = socks5_sspi_loop(cf, তথ্য, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  যদি(ফলাফল)
    যাও error;

  curlx_safefree(service_name);

  status = Curl_pSecFn->QueryCredentialsAttributes(&cred_handle,
                                                   SECPKG_CRED_ATTR_NAMES,
                                                   &names);
  যদি(check_sspi_err(তথ্য, status, "QueryCredentialAttributes")) {
    failf(তথ্য, "Failed to determine username.");
    ফলাফল = CURLE_COULDNT_CONNECT;
    যাও error;
  }
  অন্যথা {
    VERBOSE(অক্ষর *user_utf8 = curlx_convert_tchar_to_UTF8(names.sUserName));
    infof(তথ্য, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->Free_Context_বাফার(names.sUserName);
    names.sUserName = ফাঁকা;
  }

  ফলাফল = socks5_sspi_encrypt(cf, তথ্য, &sspi_context, sspi_ret_flags);
  যদি(ফলাফল)
    যাও error;

  (শূন্য)curlx_nonblock(sock, TRUE);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  ফেরত CURLE_OK;

error:
  (শূন্য)curlx_nonblock(sock, TRUE);
  curlx_free(service_name);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  যদি(names.sUserName)
    Curl_pSecFn->Free_Context_বাফার(names.sUserName);
  ফেরত ফলাফল;
}
#endif
