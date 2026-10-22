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
#अन्तर्भाव "curl_setup.h"

#यदि defined(USE_WINDOWS_SSPI) && !defined(CURL_DISABLE_PROXY)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "strerror.h"
#अन्तर्भाव "curlx/nonblock.h"
#अन्तर्भाव "socks.h"
#अन्तर्भाव "curl_sspi.h"
#अन्तर्भाव "curlx/multibyte.h"

/*
 * Helper SSPI error functions.
 */
स्थिर पूर्णाङ्क check_sspi_err(संरचना Curl_easy *दत्त,
                          SECURITY_स्थितिः स्थितिः,
                          ध्रुव वर्ण *function)
{
  यदि(स्थितिः != SEC_E_OK &&
     स्थितिः != SEC_I_COMPLETE_AND_CONTINUE &&
     स्थितिः != SEC_I_COMPLETE_NEEDED &&
     स्थितिः != SEC_I_CONTINUE_NEEDED) {
    वर्ण बफर[STRERROR_LEN];
    failf(दत्त, "SSPI error: %s failed: %s", function,
          Curl_sspi_strerror(स्थितिः, बफर, परिमाण(बफर)));
    प्रत्यागम 1;
  }
  प्रत्यागम 0;
}

/* This is the SSPI-using version of this function */
स्थिर CURLcode socks5_sspi_सज्जता(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना Curl_creds *creds,
                                  CredHandle *cred_handle,
                                  वर्ण **service_namep)
{
  संरचना connectdata *conn = cf->conn;
  ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  SECURITY_स्थितिः स्थितिः;

  /* prepare service name */
  यदि(strchr(service, '/'))
    *service_namep = curlx_strdup(service);
  अन्यथा
    *service_namep = curl_maprintf("%s/%s",
                                   service, conn->socks_proxy.peer->hostname);
  यदि(!*service_namep)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  स्थितिः =
    Curl_pSecFn->AcquireCredentialsHandle(रिक्त, CURL_UNCONST(TEXT("Kerberos")),
                                          SECPKG_CRED_OUTBOUND,
                                          रिक्त, रिक्त, रिक्त, रिक्त,
                                          cred_handle, रिक्त);

  यदि(check_sspi_err(दत्त, स्थितिः, "AcquireCredentialsHandle")) {
    failf(दत्त, "Failed to acquire credentials.");
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode socks5_free_token(Sec_बफर *send_token,
                                  CURLcode परिणाम)
{
  यदि(send_token->pv_बफर) {
    Curl_pSecFn->Free_Context_बफर(send_token->pv_बफर);
    send_token->pv_बफर = रिक्त;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode socks5_sspi_loop(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 CredHandle *cred_handle,
                                 CtxtHandle *sspi_context,
                                 वर्ण *service_name,
                                 अचिह्नित दीर्घ *sspi_ret_flagsp)
{
  संरचना connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode परिणाम = CURLE_OK;
  CURLcode संहिता;
  SECURITY_स्थितिः स्थितिः;
  Sec_बफर sspi_send_token, sspi_recv_token;
  Sec_बफर_Desc निवेशः_desc, output_desc;
  PCtxtHandle context_handle = रिक्त;
  अचिह्नित ह्रस्व us_दीर्घता;
  size_t actualread;
  size_t written;
  अचिह्नित वर्ण socksreq[4];

  निवेशः_desc.cBuffers = 1;
  निवेशः_desc.pBuffers = &sspi_recv_token;
  निवेशः_desc.ul_संस्करणम् = SECBUFFER_संस्करणम्;

  sspi_recv_token.बफर_Type = SECBUFFER_TOKEN;
  sspi_recv_token.cb_बफर = 0;
  sspi_recv_token.pv_बफर = रिक्त;

  output_desc.cBuffers = 1;
  output_desc.pBuffers = &sspi_send_token;
  output_desc.ul_संस्करणम् = SECBUFFER_संस्करणम्;

  sspi_send_token.बफर_Type = SECBUFFER_TOKEN;
  sspi_send_token.cb_बफर = 0;
  sspi_send_token.pv_बफर = रिक्त;

  (शून्य)curlx_nonblock(sock, FALSE);

  प्रति(;;) {
    TCHAR *sname = curlx_convert_UTF8_गन्तव्यम्_tchar(service_name);
    यदि(!sname) {
      curlx_free(sspi_recv_token.pv_बफर);
      प्रत्यागम socks5_free_token(&sspi_send_token, CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः);
    }

    स्थितिः =
      Curl_pSecFn->InitializeSecurityContext(cred_handle, context_handle,
                                             sname,
                                             ISC_REQ_MUTUAL_AUTH |
                                             ISC_REQ_ALLOCATE_स्मृतिः |
                                             ISC_REQ_CONFIDENTIALITY |
                                             ISC_REQ_REPLAY_DETECT,
                                             0, SECURITY_NATIVE_DREP,
                                             &निवेशः_desc, 0,
                                             sspi_context,
                                             &output_desc,
                                             sspi_ret_flagsp, रिक्त);

    curlx_free(sname);
    curlx_safefree(sspi_recv_token.pv_बफर);
    sspi_recv_token.cb_बफर = 0;

    यदि(check_sspi_err(दत्त, स्थितिः, "InitializeSecurityContext")) {
      failf(दत्त, "Failed to initialize security context.");
      प्रत्यागम socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
    }

    यदि(sspi_send_token.cb_बफर) {
      socksreq[0] = 1; /* GSS-API subnegotiation version */
      socksreq[1] = 1; /* authentication message type */
      यदि(sspi_send_token.cb_बफर > 0xffff) {
        /* needs to fit in an unsigned 16-bit field */
        प्रत्यागम socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
      us_दीर्घता = htons((अचिह्नित ह्रस्व)sspi_send_token.cb_बफर);
      स्मृतिं_प्रतिलिख(socksreq + 2, &us_दीर्घता, परिमाण(ह्रस्व));

      संहिता = Curl_conn_cf_send(cf->अग्र, दत्त, socksreq, 4, FALSE, &written);
      यदि(संहिता || (written != 4)) {
        failf(दत्त, "Failed to send SSPI authentication request.");
        प्रत्यागम socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }

      संहिता = Curl_conn_cf_send(cf->अग्र, दत्त,
                               sspi_send_token.pv_बफर,
                               sspi_send_token.cb_बफर, FALSE, &written);
      यदि(संहिता || (sspi_send_token.cb_बफर != written)) {
        failf(दत्त, "Failed to send SSPI authentication token.");
        प्रत्यागम socks5_free_token(&sspi_send_token, CURLE_COULDNT_CONNECT);
      }
    }

    यदि(sspi_send_token.pv_बफर)
      socks5_free_token(&sspi_send_token, CURLE_OK);
    sspi_send_token.cb_बफर = 0;

    curlx_safefree(sspi_recv_token.pv_बफर);
    sspi_recv_token.cb_बफर = 0;

    यदि(स्थितिः != SEC_I_CONTINUE_NEEDED)
      भङ्ग;

    परिणाम = Curl_blockread_all(cf, दत्त, (वर्ण *)socksreq, 4, &actualread);
    यदि(परिणाम || (actualread != 4)) {
      failf(दत्त, "Failed to receive SSPI authentication response.");
      प्रत्यागम परिणाम ? परिणाम : CURLE_COULDNT_CONNECT;
    }

    यदि(socksreq[1] == 255) {
      failf(दत्त, "User was rejected by the SOCKS5 server (%u %u).",
            (अचिह्नित पूर्णाङ्क)socksreq[0], (अचिह्नित पूर्णाङ्क)socksreq[1]);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    यदि(socksreq[1] != 1) {
      failf(दत्त, "Invalid SSPI authentication response type (%u %u).",
            (अचिह्नित पूर्णाङ्क)socksreq[0], (अचिह्नित पूर्णाङ्क)socksreq[1]);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    स्मृतिं_प्रतिलिख(&us_दीर्घता, socksreq + 2, परिमाण(ह्रस्व));
    us_दीर्घता = ntohs(us_दीर्घता);

    यदि(!us_दीर्घता) {
      failf(दत्त, "Invalid zero-length SSPI authentication token.");
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    sspi_recv_token.cb_बफर = us_दीर्घता;
    sspi_recv_token.pv_बफर = curlx_malloc(us_दीर्घता);

    यदि(!sspi_recv_token.pv_बफर)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    परिणाम = Curl_blockread_all(cf, दत्त, (वर्ण *)sspi_recv_token.pv_बफर,
                                sspi_recv_token.cb_बफर, &actualread);

    यदि(परिणाम || (actualread != us_दीर्घता)) {
      failf(दत्त, "Failed to receive SSPI authentication token.");
      curlx_free(sspi_recv_token.pv_बफर);
      प्रत्यागम परिणाम ? परिणाम : CURLE_COULDNT_CONNECT;
    }

    context_handle = sspi_context;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode socks5_free(Sec_बफर *sspi_w_token,
                            CURLcode परिणाम)
{
  curlx_safefree(sspi_w_token[0].pv_बफर);
  curlx_safefree(sspi_w_token[1].pv_बफर);
  curlx_safefree(sspi_w_token[2].pv_बफर);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode socks5_sspi_encrypt(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    CtxtHandle *sspi_context,
                                    अचिह्नित दीर्घ sspi_ret_flags)
{
  CURLcode परिणाम = CURLE_OK;
  CURLcode संहिता;
  SECURITY_स्थितिः स्थितिः;
  अचिह्नित वर्ण gss_enc;
  Sec_बफर sspi_w_token[3];
  Sec_बफर_Desc wrap_desc;
  SecPkgContext_Sizes sspi_sizes;
  अचिह्नित ह्रस्व us_दीर्घता;
  अचिह्नित दीर्घ qop;
  अचिह्नित वर्ण socksreq[4];
  uint8_t *etbuf = रिक्त;
  size_t etbuf_आकार = 0;
  size_t actualread;
  size_t written;

  gss_enc = 0;
  यदि(sspi_ret_flags & ISC_REQ_CONFIDENTIALITY)
    gss_enc = 2;
  अन्यथा यदि(sspi_ret_flags & ISC_REQ_INTEGRITY)
    gss_enc = 1;

  infof(दत्त, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  sspi_w_token[0].pv_बफर =
    sspi_w_token[1].pv_बफर =
    sspi_w_token[2].pv_बफर = रिक्त;

  wrap_desc.cBuffers = 3;
  wrap_desc.pBuffers = sspi_w_token;
  wrap_desc.ul_संस्करणम् = SECBUFFER_संस्करणम्;

  socksreq[0] = 1;
  socksreq[1] = 2;

  यदि(दत्त->समुच्चयः.socks5_gssapi_nec) {
    us_दीर्घता = htons((अचिह्नित ह्रस्व)1);
    स्मृतिं_प्रतिलिख(socksreq + 2, &us_दीर्घता, परिमाण(ह्रस्व));
  }
  अन्यथा {
    स्थितिः = Curl_pSecFn->Query_Context_गुणाः(sspi_context,
                                                 SECPKG_ATTR_SIZES,
                                                 &sspi_sizes);
    यदि(check_sspi_err(दत्त, स्थितिः, "QueryContextAttributes")) {
      failf(दत्त, "Failed to query security context attributes.");
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    sspi_w_token[0].cb_बफर = sspi_sizes.cbSecurityTrailer;
    sspi_w_token[0].बफर_Type = SECBUFFER_TOKEN;
    sspi_w_token[0].pv_बफर = curlx_malloc(sspi_sizes.cbSecurityTrailer);

    यदि(!sspi_w_token[0].pv_बफर)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    sspi_w_token[1].cb_बफर = 1;
    sspi_w_token[1].बफर_Type = SECBUFFER_दत्त;
    sspi_w_token[1].pv_बफर = curlx_malloc(1);
    यदि(!sspi_w_token[1].pv_बफर)
      प्रत्यागम socks5_free(sspi_w_token, CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः);

    स्मृतिं_प्रतिलिख(sspi_w_token[1].pv_बफर, &gss_enc, 1);
    sspi_w_token[2].बफर_Type = SECBUFFER_PADDING;
    sspi_w_token[2].cb_बफर = sspi_sizes.cb_खण्डम्_आकार;
    sspi_w_token[2].pv_बफर = curlx_malloc(sspi_sizes.cb_खण्डम्_आकार);
    यदि(!sspi_w_token[2].pv_बफर)
      प्रत्यागम socks5_free(sspi_w_token, CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः);

    स्थितिः = Curl_pSecFn->Encrypt_सन्देशः(sspi_context,
                                         KERB_WRAP_NO_ENCRYPT,
                                         &wrap_desc, 0);
    यदि(check_sspi_err(दत्त, स्थितिः, "EncryptMessage"))
      प्रत्यागम socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf_आकार = sspi_w_token[0].cb_बफर + sspi_w_token[1].cb_बफर +
      sspi_w_token[2].cb_बफर;
    यदि(etbuf_आकार > 0xffff)
      प्रत्यागम socks5_free(sspi_w_token, CURLE_COULDNT_CONNECT);

    etbuf = curlx_malloc(etbuf_आकार);
    यदि(!etbuf)
      प्रत्यागम socks5_free(sspi_w_token, CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः);

    स्मृतिं_प्रतिलिख(etbuf, sspi_w_token[0].pv_बफर, sspi_w_token[0].cb_बफर);
    स्मृतिं_प्रतिलिख(etbuf + sspi_w_token[0].cb_बफर,
           sspi_w_token[1].pv_बफर, sspi_w_token[1].cb_बफर);
    स्मृतिं_प्रतिलिख(etbuf + sspi_w_token[0].cb_बफर + sspi_w_token[1].cb_बफर,
           sspi_w_token[2].pv_बफर, sspi_w_token[2].cb_बफर);

    (शून्य)socks5_free(sspi_w_token, CURLE_OK);

    us_दीर्घता = htons((अचिह्नित ह्रस्व)etbuf_आकार);
    स्मृतिं_प्रतिलिख(socksreq + 2, &us_दीर्घता, परिमाण(ह्रस्व));
  }

  संहिता = Curl_conn_cf_send(cf->अग्र, दत्त, socksreq, 4, FALSE, &written);
  यदि(संहिता || (written != 4)) {
    failf(दत्त, "Failed to send SSPI encryption request.");
    curlx_free(etbuf);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  यदि(दत्त->समुच्चयः.socks5_gssapi_nec) {
    स्मृतिं_प्रतिलिख(socksreq, &gss_enc, 1);
    संहिता = Curl_conn_cf_send(cf->अग्र, दत्त, socksreq, 1, FALSE, &written);
    यदि(संहिता || (written != 1)) {
      failf(दत्त, "Failed to send SSPI encryption type.");
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }
  }
  अन्यथा {
    संहिता = Curl_conn_cf_send(cf->अग्र, दत्त, etbuf, etbuf_आकार, FALSE,
                             &written);
    curlx_free(etbuf);
    यदि(संहिता || (etbuf_आकार != written)) {
      failf(दत्त, "Failed to send SSPI encryption type.");
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }
  }

  परिणाम = Curl_blockread_all(cf, दत्त, (वर्ण *)socksreq, 4, &actualread);
  यदि(परिणाम || (actualread != 4)) {
    failf(दत्त, "Failed to receive SSPI encryption response.");
    प्रत्यागम परिणाम ? परिणाम : CURLE_COULDNT_CONNECT;
  }

  यदि(socksreq[1] == 255) {
    failf(दत्त, "User was rejected by the SOCKS5 server (%u %u).",
          (अचिह्नित पूर्णाङ्क)socksreq[0], (अचिह्नित पूर्णाङ्क)socksreq[1]);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  यदि(socksreq[1] != 2) {
    failf(दत्त, "Invalid SSPI encryption response type (%u %u).",
          (अचिह्नित पूर्णाङ्क)socksreq[0], (अचिह्नित पूर्णाङ्क)socksreq[1]);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  स्मृतिं_प्रतिलिख(&us_दीर्घता, socksreq + 2, परिमाण(ह्रस्व));
  us_दीर्घता = ntohs(us_दीर्घता);

  यदि(!us_दीर्घता) {
    failf(दत्त, "Invalid zero-length SSPI encryption token.");
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  sspi_w_token[0].cb_बफर = us_दीर्घता;
  sspi_w_token[0].pv_बफर = curlx_malloc(us_दीर्घता);
  यदि(!sspi_w_token[0].pv_बफर)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  परिणाम = Curl_blockread_all(cf, दत्त, (वर्ण *)sspi_w_token[0].pv_बफर,
                              sspi_w_token[0].cb_बफर, &actualread);

  यदि(परिणाम || (actualread != us_दीर्घता)) {
    failf(दत्त, "Failed to receive SSPI encryption type.");
    गच्छ fail;
  }

  यदि(!दत्त->समुच्चयः.socks5_gssapi_nec) {
    wrap_desc.cBuffers = 2;
    sspi_w_token[0].बफर_Type = SECBUFFER_प्रवाहः;
    sspi_w_token[1].बफर_Type = SECBUFFER_दत्त;
    sspi_w_token[1].cb_बफर = 0;
    sspi_w_token[1].pv_बफर = रिक्त;

    /* At least one of the descriptors must be of type SECBUFFER_DATA. The
       message is decrypted in place so the SECBUFFER_DATA receives a pointer
       to the message in SECBUFFER_STREAM. */
    स्थितिः = Curl_pSecFn->Decrypt_सन्देशः(sspi_context, &wrap_desc, 0, &qop);

    यदि(check_sspi_err(दत्त, स्थितिः, "DecryptMessage"))
      गच्छ fail;

    यदि(sspi_w_token[1].cb_बफर != 1) {
      failf(दत्त, "Invalid SSPI encryption response length (%lu).",
            (अचिह्नित दीर्घ)sspi_w_token[1].cb_बफर);
      गच्छ fail;
    }

    स्मृतिं_प्रतिलिख(socksreq, sspi_w_token[1].pv_बफर, sspi_w_token[1].cb_बफर);
  }
  अन्यथा {
    यदि(sspi_w_token[0].cb_बफर != 1) {
      failf(दत्त, "Invalid SSPI encryption response length (%lu).",
            (अचिह्नित दीर्घ)sspi_w_token[0].cb_बफर);
      गच्छ fail;
    }
    स्मृतिं_प्रतिलिख(socksreq, sspi_w_token[0].pv_बफर, sspi_w_token[0].cb_बफर);
  }
  curlx_free(sspi_w_token[0].pv_बफर);

  infof(दत्त, "SOCKS5 access %s protection granted BUT NOT USED.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  cf->conn->socks5_gssapi_enctype = socksreq[0];
  प्रत्यागम CURLE_OK;

fail:
  curlx_free(sspi_w_token[0].pv_बफर);
  प्रत्यागम CURLE_COULDNT_CONNECT;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      संरचना Curl_creds *creds)
{
  संरचना connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode परिणाम;
  SECURITY_स्थितिः स्थितिः;
  CredHandle cred_handle;
  CtxtHandle sspi_context;
  Sec_Pkg_Credentials_नामानि नामानि;
  वर्ण *service_name = रिक्त;
  अचिह्नित दीर्घ sspi_ret_flags = 0;

  स्मृतिं_पूरय(&cred_handle, 0, परिमाण(cred_handle));
  स्मृतिं_पूरय(&sspi_context, 0, परिमाण(sspi_context));
  नामानि.sUserName = रिक्त;

  परिणाम = socks5_sspi_सज्जता(cf, दत्त, creds, &cred_handle, &service_name);
  यदि(परिणाम)
    गच्छ error;

  परिणाम = socks5_sspi_loop(cf, दत्त, &cred_handle, &sspi_context,
                            service_name, &sspi_ret_flags);
  यदि(परिणाम)
    गच्छ error;

  curlx_safefree(service_name);

  स्थितिः = Curl_pSecFn->Query_Credentials_गुणाः(&cred_handle,
                                                   SECPKG_CRED_ATTR_नामानि,
                                                   &नामानि);
  यदि(check_sspi_err(दत्त, स्थितिः, "QueryCredentialAttributes")) {
    failf(दत्त, "Failed to determine username.");
    परिणाम = CURLE_COULDNT_CONNECT;
    गच्छ error;
  }
  अन्यथा {
    VERBOSE(वर्ण *user_utf8 = curlx_convert_tchar_गन्तव्यम्_UTF8(नामानि.sUserName));
    infof(दत्त, "SOCKS5 server authenticated user %s with GSS-API.",
          (user_utf8 ? user_utf8 : "(unknown)"));
    VERBOSE(curlx_free(user_utf8));
    Curl_pSecFn->Free_Context_बफर(नामानि.sUserName);
    नामानि.sUserName = रिक्त;
  }

  परिणाम = socks5_sspi_encrypt(cf, दत्त, &sspi_context, sspi_ret_flags);
  यदि(परिणाम)
    गच्छ error;

  (शून्य)curlx_nonblock(sock, TRUE);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  प्रत्यागम CURLE_OK;

error:
  (शून्य)curlx_nonblock(sock, TRUE);
  curlx_free(service_name);
  Curl_pSecFn->DeleteSecurityContext(&sspi_context);
  Curl_pSecFn->FreeCredentialsHandle(&cred_handle);
  यदि(नामानि.sUserName)
    Curl_pSecFn->Free_Context_बफर(नामानि.sUserName);
  प्रत्यागम परिणाम;
}
#यदि_अन्तः
