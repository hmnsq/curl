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
#شامل "curl_setup.h"

#اگر defined(HAVE_GSSAPI) && !defined(CURL_DISABLE_PROXY)

#شامل "curl_gssapi.h"
#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "curlx/nonblock.h"
#شامل "socks.h"
#شامل "curlx/strdup.h"

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#define MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
ایستا صحیح check_gss_err(ساختار Curl_easy *data,
                         OM_uint32 major_status,
                         OM_uint32 minor_status,
                         ثابت نویسه *function)
{
  اگر(GSS_ERROR(major_status)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 پیام_ctx = 0;
    gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;
    ساختار dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    پیام_ctx = 0;
    وقتی(!پیام_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_status(&min_stat, major_status,
                                    GSS_C_GSS_CODE,
                                    GSS_C_NULL_OID,
                                    &پیام_ctx, &status_string);
      اگر(maj_stat == GSS_S_COMPLETE) {
        اگر(curlx_dyn_addn(&dbuf, status_string.مقدار, status_string.length))
          بازگشت 1; /* error */
        gss_release_buffer(&min_stat, &status_string);
        بشکن;
      }
      gss_release_buffer(&min_stat, &status_string);
    }
    اگر(curlx_dyn_addn(&dbuf, ".\n", 2))
      بازگشت 1; /* error */
    پیام_ctx = 0;
    وقتی(!پیام_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_status(&min_stat, minor_status,
                                    GSS_C_MECH_CODE,
                                    GSS_C_NULL_OID,
                                    &پیام_ctx, &status_string);
      اگر(maj_stat == GSS_S_COMPLETE) {
        اگر(curlx_dyn_addn(&dbuf, status_string.مقدار, status_string.length))
          بازگشت 1; /* error */
        gss_release_buffer(&min_stat, &status_string);
        بشکن;
      }
      gss_release_buffer(&min_stat, &status_string);
    }
    failf(data, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    بازگشت 1;
  }

  بازگشت 0;
}

ایستا CURLcode socks5_gss_create_service_name(ساختار Curl_easy *data,
                                               ساختار connectdata *conn,
                                               ثابت نویسه *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc service = GSS_C_EMPTY_BUFFER;

  /* prepare service name */
  اگر(strchr(serviceptr, '/')) {
    service.length = strlen(serviceptr);
    service.مقدار = curlx_memdup(serviceptr, service.length);
    اگر(!service.مقدار)
      بازگشت CURLE_OUT_OF_MEMORY;

    gss_major_status = gss_import_name(&gss_minor_status, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  وگرنه {
    service.مقدار = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    اگر(!service.مقدار)
      بازگشت CURLE_OUT_OF_MEMORY;
    service.length = strlen(service.مقدار);

    gss_major_status = gss_import_name(&gss_minor_status, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.مقدار);
  service.length = 0;

  اگر(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_import_name()")) {
    failf(data, "Failed to create service name.");
    gss_release_name(&gss_status, server);
    بازگشت CURLE_COULDNT_CONNECT;
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode socks5_gss_auth_loop(ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_recv_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc *gss_token = GSS_C_NO_BUFFER;
  unsigned short us_length;
  unsigned نویسه socksreq[4];
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
  برای(;;) {
    gss_major_status = Curl_gss_init_sec_context(data,
                                                 &gss_minor_status,
                                                 gss_context,
                                                 *server_ptr,
                                                 &Curl_krb5_mech_oid,
                                                 پوچ,
                                                 gss_token,
                                                 &gss_send_token,
                                                 TRUE,
                                                 gss_ret_flags,
                                                 GSS_C_NO_CREDENTIAL);

    اگر(gss_token != GSS_C_NO_BUFFER) {
      curlx_safefree(gss_recv_token.مقدار);
      gss_recv_token.length = 0;
    }
    اگر(check_gss_err(data, gss_major_status,
                     gss_minor_status, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.length > 0xffff)) {
      gss_release_name(&gss_status, server_ptr);
      gss_release_buffer(&gss_status, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      failf(data, "Failed to initialize GSS-API token.");
      بازگشت CURLE_COULDNT_CONNECT;
    }

    اگر(gss_send_token.length) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_length = htons((unsigned short)gss_send_token.length);
      memcpy(socksreq + 2, &us_length, اندازه(short));

      result = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE,
                                 &nwritten);
      اگر(result || (nwritten != 4)) {
        failf(data, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_status, server_ptr);
        gss_release_buffer(&gss_status, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
        بازگشت CURLE_COULDNT_CONNECT;
      }

      result = Curl_conn_cf_send(cf->next, data,
                                 gss_send_token.مقدار,
                                 gss_send_token.length, FALSE, &nwritten);
      اگر(result || (gss_send_token.length != nwritten)) {
        failf(data, "Failed to send GSS-API authentication token.");
        gss_release_name(&gss_status, server_ptr);
        gss_release_buffer(&gss_status, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
        بازگشت CURLE_COULDNT_CONNECT;
      }
    }

    gss_release_buffer(&gss_status, &gss_send_token);
    اگر(gss_major_status != GSS_S_CONTINUE_NEEDED)
      بشکن;

    /* analyze response */

    /*   GSS-API response looks like
     * +----+------+-----+----------------+
     * |VER | MTYP | LEN |     TOKEN      |
     * +----+------+----------------------+
     * | 1  |  1   |  2  | up to 2^16 - 1 |
     * +----+------+-----+----------------+
     */

    result = Curl_blockread_all(cf, data, (نویسه *)socksreq, 4, &actualread);
    اگر(result || (actualread != 4)) {
      failf(data, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      بازگشت CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    اگر(socksreq[1] == 255) { /* status / message type */
      failf(data, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      بازگشت CURLE_COULDNT_CONNECT;
    }

    اگر(socksreq[1] != 1) { /* status / message type */
      failf(data, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      بازگشت CURLE_COULDNT_CONNECT;
    }

    memcpy(&us_length, socksreq + 2, اندازه(short));
    us_length = ntohs(us_length);

    اگر(!us_length) {
      failf(data, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      بازگشت CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.length = us_length;
    gss_recv_token.مقدار = curlx_malloc(gss_recv_token.length);
    اگر(!gss_recv_token.مقدار) {
      failf(data,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      بازگشت CURLE_OUT_OF_MEMORY;
    }

    result = Curl_blockread_all(cf, data, (نویسه *)gss_recv_token.مقدار,
                                gss_recv_token.length, &actualread);

    اگر(result || (actualread != us_length)) {
      failf(data, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_status, server_ptr);
      curlx_safefree(gss_recv_token.مقدار);
      gss_recv_token.length = 0;
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      بازگشت CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_status, server_ptr);
  بازگشت CURLE_OK;
}

ایستا CURLcode socks5_gss_auth_verify(ساختار Curl_easy *data,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;

  /* Everything is good so far, user was authenticated! */
  gss_major_status = gss_inquire_context(&gss_minor_status, *gss_context,
                                         &gss_client_name, پوچ, پوچ, پوچ,
                                         پوچ, پوچ, پوچ);
  اگر(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
    gss_release_name(&gss_status, &gss_client_name);
    failf(data, "Failed to determine username.");
    بازگشت CURLE_COULDNT_CONNECT;
  }
  gss_major_status = gss_display_name(&gss_minor_status, gss_client_name,
                                      &gss_send_token, پوچ);
  اگر(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
    gss_release_name(&gss_status, &gss_client_name);
    gss_release_buffer(&gss_status, &gss_send_token);
    failf(data, "Failed to determine username.");
    بازگشت CURLE_COULDNT_CONNECT;
  }

  infof(data, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (صحیح)gss_send_token.length, (ثابت نویسه *)gss_send_token.مقدار);

  gss_release_name(&gss_status, &gss_client_name);
  gss_release_buffer(&gss_status, &gss_send_token);

  بازگشت CURLE_OK;
}

ایستا CURLcode socks5_gss_negotiate_enc(ساختار Curl_cfilter *cf,
                                         ساختار Curl_easy *data,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  ساختار connectdata *conn = cf->conn;
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_recv_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_w_token = GSS_C_EMPTY_BUFFER;
  unsigned short us_length;
  unsigned نویسه socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode result;
  صحیح gss_enc;

  /* Do encryption */
  socksreq[0] = 1;    /* GSS-API subnegotiation version */
  socksreq[1] = 2;    /* encryption message type */

  gss_enc = 0; /* no data protection */
  /* do confidentiality protection if supported */
  اگر(gss_ret_flags & GSS_C_CONF_FLAG)
    gss_enc = 2;
  /* else do integrity protection */
  وگرنه اگر(gss_ret_flags & GSS_C_INTEG_FLAG)
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
  اگر(data->set.socks5_gssapi_nec) {
    us_length = htons((short)1);
    memcpy(socksreq + 2, &us_length, اندازه(short));
  }
  وگرنه {
    gss_send_token.length = 1;
    gss_send_token.مقدار = curlx_memdup(&gss_enc, gss_send_token.length);
    اگر(!gss_send_token.مقدار) {
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      بازگشت CURLE_OUT_OF_MEMORY;
    }

    gss_major_status = gss_wrap(&gss_minor_status, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                پوچ, &gss_w_token);

    اگر(check_gss_err(data, gss_major_status, gss_minor_status, "gss_wrap")) {
      curlx_safefree(gss_send_token.مقدار);
      gss_send_token.length = 0;
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      failf(data, "Failed to wrap GSS-API encryption value into token.");
      بازگشت CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.مقدار);
    gss_send_token.length = 0;

    us_length = htons((unsigned short)gss_w_token.length);
    memcpy(socksreq + 2, &us_length, اندازه(short));
  }

  result = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE, &nwritten);
  اگر(result || (nwritten != 4)) {
    failf(data, "Failed to send GSS-API encryption request.");
    gss_release_buffer(&gss_status, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
    بازگشت CURLE_COULDNT_CONNECT;
  }

  اگر(data->set.socks5_gssapi_nec) {
    memcpy(socksreq, &gss_enc, 1);
    result = Curl_conn_cf_send(cf->next, data, socksreq, 1, FALSE, &nwritten);
    اگر(result || (nwritten != 1)) {
      failf(data, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      بازگشت CURLE_COULDNT_CONNECT;
    }
  }
  وگرنه {
    result = Curl_conn_cf_send(cf->next, data, gss_w_token.مقدار,
                               gss_w_token.length, FALSE, &nwritten);
    اگر(result || (gss_w_token.length != nwritten)) {
      failf(data, "Failed to send GSS-API encryption type.");
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      بازگشت CURLE_COULDNT_CONNECT;
    }
    gss_release_buffer(&gss_status, &gss_w_token);
  }

  result = Curl_blockread_all(cf, data, (نویسه *)socksreq, 4, &actualread);
  اگر(result || (actualread != 4)) {
    failf(data, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
    بازگشت CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  اگر(socksreq[1] == 255) { /* status / message type */
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
    بازگشت CURLE_COULDNT_CONNECT;
  }

  اگر(socksreq[1] != 2) { /* status / message type */
    failf(data, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
    بازگشت CURLE_COULDNT_CONNECT;
  }

  memcpy(&us_length, socksreq + 2, اندازه(short));
  us_length = ntohs(us_length);

  اگر(!us_length) {
    failf(data, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
    بازگشت CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.length = us_length;
  gss_recv_token.مقدار = curlx_malloc(gss_recv_token.length);
  اگر(!gss_recv_token.مقدار) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
    بازگشت CURLE_OUT_OF_MEMORY;
  }
  result = Curl_blockread_all(cf, data, (نویسه *)gss_recv_token.مقدار,
                              gss_recv_token.length, &actualread);

  اگر(result || (actualread != us_length)) {
    failf(data, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.مقدار);
    gss_recv_token.length = 0;
    Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
    بازگشت CURLE_COULDNT_CONNECT;
  }

  اگر(!data->set.socks5_gssapi_nec) {
    gss_major_status = gss_unwrap(&gss_minor_status, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  پوچ, پوچ);

    اگر(check_gss_err(data, gss_major_status, gss_minor_status, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.مقدار);
      gss_recv_token.length = 0;
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      failf(data, "Failed to unwrap GSS-API encryption value into token.");
      بازگشت CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.مقدار);
    gss_recv_token.length = 0;

    اگر(gss_w_token.length != 1) {
      failf(data, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.length);
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      بازگشت CURLE_COULDNT_CONNECT;
    }

    memcpy(socksreq, gss_w_token.مقدار, gss_w_token.length);
    gss_release_buffer(&gss_status, &gss_w_token);
  }
  وگرنه {
    اگر(gss_recv_token.length != 1) {
      failf(data, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.length);
      curlx_safefree(gss_recv_token.مقدار);
      gss_recv_token.length = 0;
      Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);
      بازگشت CURLE_COULDNT_CONNECT;
    }

    memcpy(socksreq, gss_recv_token.مقدار, gss_recv_token.length);
    curlx_safefree(gss_recv_token.مقدار);
    gss_recv_token.length = 0;
  }

  infof(data, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  اگر(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_status, gss_context, پوچ);

  بازگشت CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      ساختار Curl_creds *creds)
{
  ساختار connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode result;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  ثابت نویسه *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  result = socks5_gss_create_service_name(data, conn, service, &server);
  اگر(!result) {
    (تهی)curlx_nonblock(sock, FALSE);
    result = socks5_gss_auth_loop(cf, data, &server, &gss_context,
                                  &gss_ret_flags);
  }
  اگر(!result)
    result = socks5_gss_auth_verify(data, &gss_context);
  اگر(!result)
    result = socks5_gss_negotiate_enc(cf, data, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (تهی)curlx_nonblock(sock, TRUE);

  بازگشت result;
}

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic برداشت
#endif

#endif /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
