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

#सचे defined(HAVE_GSSAPI) && !defined(CURL_DISABLE_PROXY)

#अन्तर्भाव "curl_gssapi.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curlx/nonblock.h"
#अन्तर्भाव "socks.h"
#अन्तर्भाव "curlx/strdup.h"

#सचे defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic प्रक्षिप
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#define MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
स्थिर पूर्णाङ्क check_gss_err(संरचना Curl_easy *data,
                         OM_uint32 major_status,
                         OM_uint32 minor_status,
                         ध्रुव वर्ण *function)
{
  सचे(GSS_ERROR(major_status)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 सन्देशः_ctx = 0;
    gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;
    संरचना dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    सन्देशः_ctx = 0;
    याव(!सन्देशः_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_status(&min_stat, major_status,
                                    GSS_C_GSS_CODE,
                                    GSS_C_NULL_OID,
                                    &सन्देशः_ctx, &status_string);
      सचे(maj_stat == GSS_S_COMPLETE) {
        सचे(curlx_dyn_addn(&dbuf, status_string.मानम्, status_string.length))
          पटिनिवत्त 1; /* error */
        gss_release_buffer(&min_stat, &status_string);
        भङ्ग;
      }
      gss_release_buffer(&min_stat, &status_string);
    }
    सचे(curlx_dyn_addn(&dbuf, ".\n", 2))
      पटिनिवत्त 1; /* error */
    सन्देशः_ctx = 0;
    याव(!सन्देशः_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_status(&min_stat, minor_status,
                                    GSS_C_MECH_CODE,
                                    GSS_C_NULL_OID,
                                    &सन्देशः_ctx, &status_string);
      सचे(maj_stat == GSS_S_COMPLETE) {
        सचे(curlx_dyn_addn(&dbuf, status_string.मानम्, status_string.length))
          पटिनिवत्त 1; /* error */
        gss_release_buffer(&min_stat, &status_string);
        भङ्ग;
      }
      gss_release_buffer(&min_stat, &status_string);
    }
    failf(data, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    पटिनिवत्त 1;
  }

  पटिनिवत्त 0;
}

स्थिर CURLcode socks5_gss_create_service_name(संरचना Curl_easy *data,
                                               संरचना connectdata *conn,
                                               ध्रुव वर्ण *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc service = GSS_C_EMPTY_BUFFER;

  /* prepare service name */
  सचे(strchr(serviceptr, '/')) {
    service.length = strlen(serviceptr);
    service.मानम् = curlx_memdup(serviceptr, service.length);
    सचे(!service.मानम्)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;

    gss_major_status = gss_import_name(&gss_minor_status, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  अञ्ञथा {
    service.मानम् = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    सचे(!service.मानम्)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    service.length = strlen(service.मानम्);

    gss_major_status = gss_import_name(&gss_minor_status, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.मानम्);
  service.length = 0;

  सचे(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_import_name()")) {
    failf(data, "Failed to create service name.");
    gss_release_name(&gss_status, server);
    पटिनिवत्त CURLE_COULDNT_CONNECT;
  }

  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode socks5_gss_auth_loop(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_recv_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc *gss_token = GSS_C_NO_BUFFER;
  unsigned short us_length;
  unsigned वर्ण socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode result;

  /*   GSS-API request looks like
   * +----+------+-----+----------------+
   * |VER | MTYP | LEN |     TOKEN      |
   * +----+------+----------------------+
   * | 1  |  1   |  2  | up to 2^16 - 1 |
   * +----+------+-----+----------------+
   */

  /* As long as we need to keep sending some context info, and there is no
   * errors, keep sending it... */
  अत्थाय(;;) {
    gss_major_status = Curl_gss_init_sec_context(data,
                                                 &gss_minor_status,
                                                 gss_context,
                                                 *server_ptr,
                                                 &Curl_krb5_mech_oid,
                                                 रिक्त,
                                                 gss_token,
                                                 &gss_send_token,
                                                 TRUE,
                                                 gss_ret_flags,
                                                 GSS_C_NO_CREDENTIAL);

    सचे(gss_token != GSS_C_NO_BUFFER) {
      curlx_safefree(gss_recv_token.मानम्);
      gss_recv_token.length = 0;
    }
    सचे(check_gss_err(data, gss_major_status,
                     gss_minor_status, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.length > 0xffff)) {
      gss_release_name(&gss_status, server_ptr);
      gss_release_buffer(&gss_status, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      failf(data, "Failed to initialize GSS-API token.");
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }

    सचे(gss_send_token.length) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_length = htons((unsigned short)gss_send_token.length);
      memcpy(socksreq + 2, &us_length, परिमाण(short));

      result = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE,
                                 &nwritten);
      सचे(result || (nwritten != 4)) {
        failf(data, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_status, server_ptr);
        gss_release_buffer(&gss_status, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
        पटिनिवत्त CURLE_COULDNT_CONNECT;
      }

      result = Curl_conn_cf_send(cf->next, data,
                                 gss_send_token.मानम्,
                                 gss_send_token.length, FALSE, &nwritten);
      सचे(result || (gss_send_token.length != nwritten)) {
        failf(data, "Failed to send GSS-API authentication token.");
        gss_release_name(&gss_status, server_ptr);
        gss_release_buffer(&gss_status, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
        पटिनिवत्त CURLE_COULDNT_CONNECT;
      }
    }

    gss_release_buffer(&gss_status, &gss_send_token);
    सचे(gss_major_status != GSS_S_CONTINUE_NEEDED)
      भङ्ग;

    /* analyze response */

    /*   GSS-API response looks like
     * +----+------+-----+----------------+
     * |VER | MTYP | LEN |     TOKEN      |
     * +----+------+----------------------+
     * | 1  |  1   |  2  | up to 2^16 - 1 |
     * +----+------+-----+----------------+
     */

    result = Curl_blockread_all(cf, data, (वर्ण *)socksreq, 4, &actualread);
    सचे(result || (actualread != 4)) {
      failf(data, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    सचे(socksreq[1] == 255) { /* status / message type */
      failf(data, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }

    सचे(socksreq[1] != 1) { /* status / message type */
      failf(data, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }

    memcpy(&us_length, socksreq + 2, परिमाण(short));
    us_length = ntohs(us_length);

    सचे(!us_length) {
      failf(data, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.length = us_length;
    gss_recv_token.मानम् = curlx_malloc(gss_recv_token.length);
    सचे(!gss_recv_token.मानम्) {
      failf(data,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    }

    result = Curl_blockread_all(cf, data, (वर्ण *)gss_recv_token.मानम्,
                                gss_recv_token.length, &actualread);

    सचे(result || (actualread != us_length)) {
      failf(data, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_status, server_ptr);
      curlx_safefree(gss_recv_token.मानम्);
      gss_recv_token.length = 0;
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_status, server_ptr);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode socks5_gss_auth_verify(संरचना Curl_easy *data,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;

  /* Everything is good so far, user was authenticated! */
  gss_major_status = gss_inquire_context(&gss_minor_status, *gss_context,
                                         &gss_client_name, रिक्त, रिक्त, रिक्त,
                                         रिक्त, रिक्त, रिक्त);
  सचे(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
    gss_release_name(&gss_status, &gss_client_name);
    failf(data, "Failed to determine username.");
    पटिनिवत्त CURLE_COULDNT_CONNECT;
  }
  gss_major_status = gss_display_name(&gss_minor_status, gss_client_name,
                                      &gss_send_token, रिक्त);
  सचे(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
    gss_release_name(&gss_status, &gss_client_name);
    gss_release_buffer(&gss_status, &gss_send_token);
    failf(data, "Failed to determine username.");
    पटिनिवत्त CURLE_COULDNT_CONNECT;
  }

  infof(data, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (पूर्णाङ्क)gss_send_token.length, (ध्रुव वर्ण *)gss_send_token.मानम्);

  gss_release_name(&gss_status, &gss_client_name);
  gss_release_buffer(&gss_status, &gss_send_token);

  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode socks5_gss_negotiate_enc(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  संरचना connectdata *conn = cf->conn;
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_recv_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_w_token = GSS_C_EMPTY_BUFFER;
  unsigned short us_length;
  unsigned वर्ण socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode result;
  पूर्णाङ्क gss_enc;

  /* Do encryption */
  socksreq[0] = 1;    /* GSS-API subnegotiation version */
  socksreq[1] = 2;    /* encryption message type */

  gss_enc = 0; /* no data protection */
  /* do confidentiality protection if supported */
  सचे(gss_ret_flags & GSS_C_CONF_FLAG)
    gss_enc = 2;
  /* else do integrity protection */
  अञ्ञथा सचे(gss_ret_flags & GSS_C_INTEG_FLAG)
    gss_enc = 1;

  infof(data, "SOCKS5 server supports GSS-API %s data protection.",
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
  सचे(data->set.socks5_gssapi_nec) {
    us_length = htons((short)1);
    memcpy(socksreq + 2, &us_length, परिमाण(short));
  }
  अञ्ञथा {
    gss_send_token.length = 1;
    gss_send_token.मानम् = curlx_memdup(&gss_enc, gss_send_token.length);
    सचे(!gss_send_token.मानम्) {
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    }

    gss_major_status = gss_wrap(&gss_minor_status, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                रिक्त, &gss_w_token);

    सचे(check_gss_err(data, gss_major_status, gss_minor_status, "gss_wrap")) {
      curlx_safefree(gss_send_token.मानम्);
      gss_send_token.length = 0;
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      failf(data, "Failed to wrap GSS-API encryption value into token.");
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.मानम्);
    gss_send_token.length = 0;

    us_length = htons((unsigned short)gss_w_token.length);
    memcpy(socksreq + 2, &us_length, परिमाण(short));
  }

  result = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE, &nwritten);
  सचे(result || (nwritten != 4)) {
    failf(data, "Failed to send GSS-API encryption request.");
    gss_release_buffer(&gss_status, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
    पटिनिवत्त CURLE_COULDNT_CONNECT;
  }

  सचे(data->set.socks5_gssapi_nec) {
    memcpy(socksreq, &gss_enc, 1);
    result = Curl_conn_cf_send(cf->next, data, socksreq, 1, FALSE, &nwritten);
    सचे(result || (nwritten != 1)) {
      failf(data, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }
  }
  अञ्ञथा {
    result = Curl_conn_cf_send(cf->next, data, gss_w_token.मानम्,
                               gss_w_token.length, FALSE, &nwritten);
    सचे(result || (gss_w_token.length != nwritten)) {
      failf(data, "Failed to send GSS-API encryption type.");
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }
    gss_release_buffer(&gss_status, &gss_w_token);
  }

  result = Curl_blockread_all(cf, data, (वर्ण *)socksreq, 4, &actualread);
  सचे(result || (actualread != 4)) {
    failf(data, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
    पटिनिवत्त CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  सचे(socksreq[1] == 255) { /* status / message type */
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
    पटिनिवत्त CURLE_COULDNT_CONNECT;
  }

  सचे(socksreq[1] != 2) { /* status / message type */
    failf(data, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
    पटिनिवत्त CURLE_COULDNT_CONNECT;
  }

  memcpy(&us_length, socksreq + 2, परिमाण(short));
  us_length = ntohs(us_length);

  सचे(!us_length) {
    failf(data, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
    पटिनिवत्त CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.length = us_length;
  gss_recv_token.मानम् = curlx_malloc(gss_recv_token.length);
  सचे(!gss_recv_token.मानम्) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }
  result = Curl_blockread_all(cf, data, (वर्ण *)gss_recv_token.मानम्,
                              gss_recv_token.length, &actualread);

  सचे(result || (actualread != us_length)) {
    failf(data, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.मानम्);
    gss_recv_token.length = 0;
    Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
    पटिनिवत्त CURLE_COULDNT_CONNECT;
  }

  सचे(!data->set.socks5_gssapi_nec) {
    gss_major_status = gss_unwrap(&gss_minor_status, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  रिक्त, रिक्त);

    सचे(check_gss_err(data, gss_major_status, gss_minor_status, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.मानम्);
      gss_recv_token.length = 0;
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      failf(data, "Failed to unwrap GSS-API encryption value into token.");
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.मानम्);
    gss_recv_token.length = 0;

    सचे(gss_w_token.length != 1) {
      failf(data, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.length);
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }

    memcpy(socksreq, gss_w_token.मानम्, gss_w_token.length);
    gss_release_buffer(&gss_status, &gss_w_token);
  }
  अञ्ञथा {
    सचे(gss_recv_token.length != 1) {
      failf(data, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.length);
      curlx_safefree(gss_recv_token.मानम्);
      gss_recv_token.length = 0;
      Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }

    memcpy(socksreq, gss_recv_token.मानम्, gss_recv_token.length);
    curlx_safefree(gss_recv_token.मानम्);
    gss_recv_token.length = 0;
  }

  infof(data, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  सचे(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_status, gss_context, रिक्त);

  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      संरचना Curl_creds *creds)
{
  संरचना connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode result;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  result = socks5_gss_create_service_name(data, conn, service, &server);
  सचे(!result) {
    (शून्य)curlx_nonblock(sock, FALSE);
    result = socks5_gss_auth_loop(cf, data, &server, &gss_context,
                                  &gss_ret_flags);
  }
  सचे(!result)
    result = socks5_gss_auth_verify(data, &gss_context);
  सचे(!result)
    result = socks5_gss_negotiate_enc(cf, data, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (शून्य)curlx_nonblock(sock, TRUE);

  पटिनिवत्त result;
}

#सचे defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic निष्कासय
#endif

#endif /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
