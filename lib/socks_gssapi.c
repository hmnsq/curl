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
#समावेश "curl_setup.h"

#अगर defined(HAVE_GSSAPI) && !defined(CURL_DISABLE_PROXY)

#समावेश "curl_gssapi.h"
#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "curlx/nonblock.h"
#समावेश "socks.h"
#समावेश "curlx/strdup.h"

#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wdeprecated-declarations"
#पूर्ण_अगर

#घोषणा MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
अटल पूर्णांक check_gss_err(काष्ठा Curl_easy *तथ्य,
                         OM_uint32 major_status,
                         OM_uint32 minor_status,
                         स्थिर अक्षर *function)
{
  अगर(GSS_ERROR(major_status)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 संदेश_ctx = 0;
    gss_बफर_desc status_string = GSS_C_EMPTY_बफर;
    काष्ठा dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    संदेश_ctx = 0;
    जबतक(!संदेश_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_status(&min_stat, major_status,
                                    GSS_C_GSS_CODE,
                                    GSS_C_NULL_OID,
                                    &संदेश_ctx, &status_string);
      अगर(maj_stat == GSS_S_COMPLETE) {
        अगर(curlx_dyn_addn(&dbuf, status_string.मूल्य, status_string.लंबाई))
          वापस 1; /* error */
        gss_release_बफर(&min_stat, &status_string);
        अवरोध;
      }
      gss_release_बफर(&min_stat, &status_string);
    }
    अगर(curlx_dyn_addn(&dbuf, ".\n", 2))
      वापस 1; /* error */
    संदेश_ctx = 0;
    जबतक(!संदेश_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_status(&min_stat, minor_status,
                                    GSS_C_MECH_CODE,
                                    GSS_C_NULL_OID,
                                    &संदेश_ctx, &status_string);
      अगर(maj_stat == GSS_S_COMPLETE) {
        अगर(curlx_dyn_addn(&dbuf, status_string.मूल्य, status_string.लंबाई))
          वापस 1; /* error */
        gss_release_बफर(&min_stat, &status_string);
        अवरोध;
      }
      gss_release_बफर(&min_stat, &status_string);
    }
    failf(तथ्य, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    वापस 1;
  }

  वापस 0;
}

अटल CURLcode socks5_gss_create_service_name(काष्ठा Curl_easy *तथ्य,
                                               काष्ठा connectdata *conn,
                                               स्थिर अक्षर *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_बफर_desc service = GSS_C_EMPTY_बफर;

  /* prepare service name */
  अगर(म_अक्षर(serviceptr, '/')) {
    service.लंबाई = म_माप(serviceptr);
    service.मूल्य = curlx_memdup(serviceptr, service.लंबाई);
    अगर(!service.मूल्य)
      वापस CURLE_OUT_OF_MEMORY;

    gss_major_status = gss_import_name(&gss_minor_status, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  अन्यथा {
    service.मूल्य = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    अगर(!service.मूल्य)
      वापस CURLE_OUT_OF_MEMORY;
    service.लंबाई = म_माप(service.मूल्य);

    gss_major_status = gss_import_name(&gss_minor_status, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.मूल्य);
  service.लंबाई = 0;

  अगर(check_gss_err(तथ्य, gss_major_status,
                   gss_minor_status, "gss_import_name()")) {
    failf(तथ्य, "Failed to create service name.");
    gss_release_name(&gss_status, server);
    वापस CURLE_COULDNT_CONNECT;
  }

  वापस CURLE_OK;
}

अटल CURLcode socks5_gss_auth_loop(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_बफर_desc gss_send_token = GSS_C_EMPTY_बफर;
  gss_बफर_desc gss_recv_token = GSS_C_EMPTY_बफर;
  gss_बफर_desc *gss_token = GSS_C_NO_बफर;
  अचिन्हित लघु us_लंबाई;
  अचिन्हित अक्षर socksreq[4];
  माप_प्रकार actualread;
  माप_प्रकार nwritten;
  CURLcode परिणाम;

  /*   GSS-API request looks like
   * +----+------+-----+----------------+
   * |VER | MTYP | LEN |     TOKEN      |
   * +----+------+----------------------+
   * | 1  |  1   |  2  | up to 2^16 - 1 |
   * +----+------+-----+----------------+
   */

  /* As long as we need to keep sending some context info, and there is no
   * errors, keep sending it... */
  क्रम(;;) {
    gss_major_status = Curl_gss_init_sec_context(तथ्य,
                                                 &gss_minor_status,
                                                 gss_context,
                                                 *server_ptr,
                                                 &Curl_krb5_mech_oid,
                                                 शून्य,
                                                 gss_token,
                                                 &gss_send_token,
                                                 TRUE,
                                                 gss_ret_flags,
                                                 GSS_C_NO_CREDENTIAL);

    अगर(gss_token != GSS_C_NO_बफर) {
      curlx_safefree(gss_recv_token.मूल्य);
      gss_recv_token.लंबाई = 0;
    }
    अगर(check_gss_err(तथ्य, gss_major_status,
                     gss_minor_status, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.लंबाई > 0xffff)) {
      gss_release_name(&gss_status, server_ptr);
      gss_release_बफर(&gss_status, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      failf(तथ्य, "Failed to initialize GSS-API token.");
      वापस CURLE_COULDNT_CONNECT;
    }

    अगर(gss_send_token.लंबाई) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_लंबाई = htons((अचिन्हित लघु)gss_send_token.लंबाई);
      स_नकल(socksreq + 2, &us_लंबाई, माप(लघु));

      परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य, socksreq, 4, FALSE,
                                 &nwritten);
      अगर(परिणाम || (nwritten != 4)) {
        failf(तथ्य, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_status, server_ptr);
        gss_release_बफर(&gss_status, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
        वापस CURLE_COULDNT_CONNECT;
      }

      परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य,
                                 gss_send_token.मूल्य,
                                 gss_send_token.लंबाई, FALSE, &nwritten);
      अगर(परिणाम || (gss_send_token.लंबाई != nwritten)) {
        failf(तथ्य, "Failed to send GSS-API authentication token.");
        gss_release_name(&gss_status, server_ptr);
        gss_release_बफर(&gss_status, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
        वापस CURLE_COULDNT_CONNECT;
      }
    }

    gss_release_बफर(&gss_status, &gss_send_token);
    अगर(gss_major_status != GSS_S_CONTINUE_NEEDED)
      अवरोध;

    /* analyze response */

    /*   GSS-API response looks like
     * +----+------+-----+----------------+
     * |VER | MTYP | LEN |     TOKEN      |
     * +----+------+----------------------+
     * | 1  |  1   |  2  | up to 2^16 - 1 |
     * +----+------+-----+----------------+
     */

    परिणाम = Curl_blockread_all(cf, तथ्य, (अक्षर *)socksreq, 4, &actualread);
    अगर(परिणाम || (actualread != 4)) {
      failf(तथ्य, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      वापस CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    अगर(socksreq[1] == 255) { /* status / message type */
      failf(तथ्य, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      वापस CURLE_COULDNT_CONNECT;
    }

    अगर(socksreq[1] != 1) { /* status / message type */
      failf(तथ्य, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      वापस CURLE_COULDNT_CONNECT;
    }

    स_नकल(&us_लंबाई, socksreq + 2, माप(लघु));
    us_लंबाई = ntohs(us_लंबाई);

    अगर(!us_लंबाई) {
      failf(तथ्य, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      वापस CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.लंबाई = us_लंबाई;
    gss_recv_token.मूल्य = curlx_malloc(gss_recv_token.लंबाई);
    अगर(!gss_recv_token.मूल्य) {
      failf(तथ्य,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      वापस CURLE_OUT_OF_MEMORY;
    }

    परिणाम = Curl_blockread_all(cf, तथ्य, (अक्षर *)gss_recv_token.मूल्य,
                                gss_recv_token.लंबाई, &actualread);

    अगर(परिणाम || (actualread != us_लंबाई)) {
      failf(तथ्य, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_status, server_ptr);
      curlx_safefree(gss_recv_token.मूल्य);
      gss_recv_token.लंबाई = 0;
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      वापस CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_status, server_ptr);
  वापस CURLE_OK;
}

अटल CURLcode socks5_gss_auth_verify(काष्ठा Curl_easy *तथ्य,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_बफर_desc gss_send_token = GSS_C_EMPTY_बफर;

  /* Everything is good so far, user was authenticated! */
  gss_major_status = gss_inquire_context(&gss_minor_status, *gss_context,
                                         &gss_client_name, शून्य, शून्य, शून्य,
                                         शून्य, शून्य, शून्य);
  अगर(check_gss_err(तथ्य, gss_major_status,
                   gss_minor_status, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
    gss_release_name(&gss_status, &gss_client_name);
    failf(तथ्य, "Failed to determine username.");
    वापस CURLE_COULDNT_CONNECT;
  }
  gss_major_status = gss_display_name(&gss_minor_status, gss_client_name,
                                      &gss_send_token, शून्य);
  अगर(check_gss_err(तथ्य, gss_major_status,
                   gss_minor_status, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
    gss_release_name(&gss_status, &gss_client_name);
    gss_release_बफर(&gss_status, &gss_send_token);
    failf(तथ्य, "Failed to determine username.");
    वापस CURLE_COULDNT_CONNECT;
  }

  infof(तथ्य, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (पूर्णांक)gss_send_token.लंबाई, (स्थिर अक्षर *)gss_send_token.मूल्य);

  gss_release_name(&gss_status, &gss_client_name);
  gss_release_बफर(&gss_status, &gss_send_token);

  वापस CURLE_OK;
}

अटल CURLcode socks5_gss_negotiate_enc(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  काष्ठा connectdata *conn = cf->conn;
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_बफर_desc gss_send_token = GSS_C_EMPTY_बफर;
  gss_बफर_desc gss_recv_token = GSS_C_EMPTY_बफर;
  gss_बफर_desc gss_w_token = GSS_C_EMPTY_बफर;
  अचिन्हित लघु us_लंबाई;
  अचिन्हित अक्षर socksreq[4];
  माप_प्रकार actualread;
  माप_प्रकार nwritten;
  CURLcode परिणाम;
  पूर्णांक gss_enc;

  /* Do encryption */
  socksreq[0] = 1;    /* GSS-API subnegotiation version */
  socksreq[1] = 2;    /* encryption message type */

  gss_enc = 0; /* no data protection */
  /* do confidentiality protection if supported */
  अगर(gss_ret_flags & GSS_C_CONF_FLAG)
    gss_enc = 2;
  /* else do integrity protection */
  अन्यथा अगर(gss_ret_flags & GSS_C_INTEG_FLAG)
    gss_enc = 1;

  infof(तथ्य, "SOCKS5 server supports GSS-API %s data protection.",
        (gss_enc == 0) ? "no" :
        ((gss_enc == 1) ? "integrity" : "confidentiality"));

  /*
   * Sending the encryption type in clear seems wrong. It should be
   * protected with gss_seal()/gss_wrap(). See RFC1961 extract below
   * The NEC reference implementations on which this is based is
   * therefore at fault
   *
   *  +------+------+------+.......................+
   *  + ver  | mtyp | len  |   token               |
   *  +------+------+------+.......................+
   *  + 0x01 | 0x02 | 0x02 | up to 2^16 - 1 octets |
   *  +------+------+------+.......................+
   *
   *   Where:
   *
   *  - "ver" is the protocol version number, here 1 to represent the
   *    first version of the SOCKS/GSS-API protocol
   *
   *  - "mtyp" is the message type, here 2 to represent a protection
   *    -level negotiation message
   *
   *  - "len" is the length of the "token" field in octets
   *
   *  - "token" is the GSS-API encapsulated protection level
   *
   * The token is produced by encapsulating an octet containing the
   * required protection level using gss_seal()/gss_wrap() with conf_req
   * set to FALSE. The token is verified using gss_unseal()/
   * gss_unwrap().
   *
   */
  अगर(तथ्य->set.socks5_gssapi_nec) {
    us_लंबाई = htons((लघु)1);
    स_नकल(socksreq + 2, &us_लंबाई, माप(लघु));
  }
  अन्यथा {
    gss_send_token.लंबाई = 1;
    gss_send_token.मूल्य = curlx_memdup(&gss_enc, gss_send_token.लंबाई);
    अगर(!gss_send_token.मूल्य) {
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      वापस CURLE_OUT_OF_MEMORY;
    }

    gss_major_status = gss_wrap(&gss_minor_status, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                शून्य, &gss_w_token);

    अगर(check_gss_err(तथ्य, gss_major_status, gss_minor_status, "gss_wrap")) {
      curlx_safefree(gss_send_token.मूल्य);
      gss_send_token.लंबाई = 0;
      gss_release_बफर(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      failf(तथ्य, "Failed to wrap GSS-API encryption value into token.");
      वापस CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.मूल्य);
    gss_send_token.लंबाई = 0;

    us_लंबाई = htons((अचिन्हित लघु)gss_w_token.लंबाई);
    स_नकल(socksreq + 2, &us_लंबाई, माप(लघु));
  }

  परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य, socksreq, 4, FALSE, &nwritten);
  अगर(परिणाम || (nwritten != 4)) {
    failf(तथ्य, "Failed to send GSS-API encryption request.");
    gss_release_बफर(&gss_status, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
    वापस CURLE_COULDNT_CONNECT;
  }

  अगर(तथ्य->set.socks5_gssapi_nec) {
    स_नकल(socksreq, &gss_enc, 1);
    परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य, socksreq, 1, FALSE, &nwritten);
    अगर(परिणाम || (nwritten != 1)) {
      failf(तथ्य, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      वापस CURLE_COULDNT_CONNECT;
    }
  }
  अन्यथा {
    परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य, gss_w_token.मूल्य,
                               gss_w_token.लंबाई, FALSE, &nwritten);
    अगर(परिणाम || (gss_w_token.लंबाई != nwritten)) {
      failf(तथ्य, "Failed to send GSS-API encryption type.");
      gss_release_बफर(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      वापस CURLE_COULDNT_CONNECT;
    }
    gss_release_बफर(&gss_status, &gss_w_token);
  }

  परिणाम = Curl_blockread_all(cf, तथ्य, (अक्षर *)socksreq, 4, &actualread);
  अगर(परिणाम || (actualread != 4)) {
    failf(तथ्य, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
    वापस CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  अगर(socksreq[1] == 255) { /* status / message type */
    failf(तथ्य, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
    वापस CURLE_COULDNT_CONNECT;
  }

  अगर(socksreq[1] != 2) { /* status / message type */
    failf(तथ्य, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
    वापस CURLE_COULDNT_CONNECT;
  }

  स_नकल(&us_लंबाई, socksreq + 2, माप(लघु));
  us_लंबाई = ntohs(us_लंबाई);

  अगर(!us_लंबाई) {
    failf(तथ्य, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
    वापस CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.लंबाई = us_लंबाई;
  gss_recv_token.मूल्य = curlx_malloc(gss_recv_token.लंबाई);
  अगर(!gss_recv_token.मूल्य) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
    वापस CURLE_OUT_OF_MEMORY;
  }
  परिणाम = Curl_blockread_all(cf, तथ्य, (अक्षर *)gss_recv_token.मूल्य,
                              gss_recv_token.लंबाई, &actualread);

  अगर(परिणाम || (actualread != us_लंबाई)) {
    failf(तथ्य, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.मूल्य);
    gss_recv_token.लंबाई = 0;
    Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
    वापस CURLE_COULDNT_CONNECT;
  }

  अगर(!तथ्य->set.socks5_gssapi_nec) {
    gss_major_status = gss_unwrap(&gss_minor_status, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  शून्य, शून्य);

    अगर(check_gss_err(तथ्य, gss_major_status, gss_minor_status, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.मूल्य);
      gss_recv_token.लंबाई = 0;
      gss_release_बफर(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      failf(तथ्य, "Failed to unwrap GSS-API encryption value into token.");
      वापस CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.मूल्य);
    gss_recv_token.लंबाई = 0;

    अगर(gss_w_token.लंबाई != 1) {
      failf(तथ्य, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.लंबाई);
      gss_release_बफर(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      वापस CURLE_COULDNT_CONNECT;
    }

    स_नकल(socksreq, gss_w_token.मूल्य, gss_w_token.लंबाई);
    gss_release_बफर(&gss_status, &gss_w_token);
  }
  अन्यथा {
    अगर(gss_recv_token.लंबाई != 1) {
      failf(तथ्य, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.लंबाई);
      curlx_safefree(gss_recv_token.मूल्य);
      gss_recv_token.लंबाई = 0;
      Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);
      वापस CURLE_COULDNT_CONNECT;
    }

    स_नकल(socksreq, gss_recv_token.मूल्य, gss_recv_token.लंबाई);
    curlx_safefree(gss_recv_token.मूल्य);
    gss_recv_token.लंबाई = 0;
  }

  infof(तथ्य, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  अगर(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_status, gss_context, शून्य);

  वापस CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा Curl_creds *creds)
{
  काष्ठा connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode परिणाम;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  स्थिर अक्षर *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  परिणाम = socks5_gss_create_service_name(तथ्य, conn, service, &server);
  अगर(!परिणाम) {
    (व्योम)curlx_nonblock(sock, FALSE);
    परिणाम = socks5_gss_auth_loop(cf, तथ्य, &server, &gss_context,
                                  &gss_ret_flags);
  }
  अगर(!परिणाम)
    परिणाम = socks5_gss_auth_verify(तथ्य, &gss_context);
  अगर(!परिणाम)
    परिणाम = socks5_gss_negotiate_enc(cf, तथ्य, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (व्योम)curlx_nonblock(sock, TRUE);

  वापस परिणाम;
}

#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#आशय GCC diagnostic निकालो
#पूर्ण_अगर

#पूर्ण_अगर /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
