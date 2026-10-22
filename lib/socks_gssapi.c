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
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wdeprecated-declarations"
#اگر_ختم

#تعریف_کرو MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
ساکن صحیح check_gss_err(ساخت Curl_easy *ڈیٹا,
                         OM_uint32 major_حالت,
                         OM_uint32 minor_حالت,
                         ثابت حرف *function)
{
  اگر(GSS_ERROR(major_حالت)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 پیغام_ctx = 0;
    gss_بفر_desc حالت_string = GSS_C_خالی_خانے_بفر;
    ساخت dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    پیغام_ctx = 0;
    جب(!پیغام_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_حالت(&min_stat, major_حالت,
                                    GSS_C_GSS_رمز,
                                    GSS_C_NULL_OID,
                                    &پیغام_ctx, &حالت_string);
      اگر(maj_stat == GSS_S_COMPLETE) {
        اگر(curlx_dyn_addn(&dbuf, حالت_string.قدر, حالت_string.لمبائی))
          واپس 1; /* error */
        gss_release_بفر(&min_stat, &حالت_string);
        توڑو;
      }
      gss_release_بفر(&min_stat, &حالت_string);
    }
    اگر(curlx_dyn_addn(&dbuf, ".\n", 2))
      واپس 1; /* error */
    پیغام_ctx = 0;
    جب(!پیغام_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_حالت(&min_stat, minor_حالت,
                                    GSS_C_MECH_رمز,
                                    GSS_C_NULL_OID,
                                    &پیغام_ctx, &حالت_string);
      اگر(maj_stat == GSS_S_COMPLETE) {
        اگر(curlx_dyn_addn(&dbuf, حالت_string.قدر, حالت_string.لمبائی))
          واپس 1; /* error */
        gss_release_بفر(&min_stat, &حالت_string);
        توڑو;
      }
      gss_release_بفر(&min_stat, &حالت_string);
    }
    failf(ڈیٹا, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    واپس 1;
  }

  واپس 0;
}

ساکن CURLcode socks5_gss_create_service_name(ساخت Curl_easy *ڈیٹا,
                                               ساخت connectdata *conn,
                                               ثابت حرف *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_بفر_desc service = GSS_C_خالی_خانے_بفر;

  /* prepare service name */
  اگر(strchr(serviceptr, '/')) {
    service.لمبائی = لڑی_لمبائی(serviceptr);
    service.قدر = curlx_memdup(serviceptr, service.لمبائی);
    اگر(!service.قدر)
      واپس CURLE_برآمد_کا_حافظہ;

    gss_major_حالت = gss_import_name(&gss_minor_حالت, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  ورنہ {
    service.قدر = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    اگر(!service.قدر)
      واپس CURLE_برآمد_کا_حافظہ;
    service.لمبائی = لڑی_لمبائی(service.قدر);

    gss_major_حالت = gss_import_name(&gss_minor_حالت, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.قدر);
  service.لمبائی = 0;

  اگر(check_gss_err(ڈیٹا, gss_major_حالت,
                   gss_minor_حالت, "gss_import_name()")) {
    failf(ڈیٹا, "Failed to create service name.");
    gss_release_name(&gss_حالت, server);
    واپس CURLE_COULDNT_CONNECT;
  }

  واپس CURLE_OK;
}

ساکن CURLcode socks5_gss_auth_loop(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_بفر_desc gss_send_token = GSS_C_خالی_خانے_بفر;
  gss_بفر_desc gss_recv_token = GSS_C_خالی_خانے_بفر;
  gss_بفر_desc *gss_token = GSS_C_NO_بفر;
  غیرعلامتی مختصر us_لمبائی;
  غیرعلامتی حرف socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode نتیجہ;

  /*   GSS-API request looks like
   * +----+------+-----+----------------+
   * |VER | MTYP | LEN |     TOKEN      |
   * +----+------+----------------------+
   * | 1  |  1   |  2  | up to 2^16 - 1 |
   * +----+------+-----+----------------+
   */

  /* As long as we need to keep sending some context info, and there is no
   * errors, keep sending it... */
  برائے(;;) {
    gss_major_حالت = Curl_gss_init_sec_context(ڈیٹا,
                                                 &gss_minor_حالت,
                                                 gss_context,
                                                 *server_ptr,
                                                 &Curl_krb5_mech_oid,
                                                 تہی,
                                                 gss_token,
                                                 &gss_send_token,
                                                 TRUE,
                                                 gss_ret_flags,
                                                 GSS_C_NO_CREDENTIAL);

    اگر(gss_token != GSS_C_NO_بفر) {
      curlx_safefree(gss_recv_token.قدر);
      gss_recv_token.لمبائی = 0;
    }
    اگر(check_gss_err(ڈیٹا, gss_major_حالت,
                     gss_minor_حالت, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.لمبائی > 0xffff)) {
      gss_release_name(&gss_حالت, server_ptr);
      gss_release_بفر(&gss_حالت, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      failf(ڈیٹا, "Failed to initialize GSS-API token.");
      واپس CURLE_COULDNT_CONNECT;
    }

    اگر(gss_send_token.لمبائی) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_لمبائی = htons((غیرعلامتی مختصر)gss_send_token.لمبائی);
      حافظہ_نقل(socksreq + 2, &us_لمبائی, حجم(مختصر));

      نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا, socksreq, 4, FALSE,
                                 &nwritten);
      اگر(نتیجہ || (nwritten != 4)) {
        failf(ڈیٹا, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_حالت, server_ptr);
        gss_release_بفر(&gss_حالت, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
        واپس CURLE_COULDNT_CONNECT;
      }

      نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا,
                                 gss_send_token.قدر,
                                 gss_send_token.لمبائی, FALSE, &nwritten);
      اگر(نتیجہ || (gss_send_token.لمبائی != nwritten)) {
        failf(ڈیٹا, "Failed to send GSS-API authentication token.");
        gss_release_name(&gss_حالت, server_ptr);
        gss_release_بفر(&gss_حالت, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
        واپس CURLE_COULDNT_CONNECT;
      }
    }

    gss_release_بفر(&gss_حالت, &gss_send_token);
    اگر(gss_major_حالت != GSS_S_CONTINUE_NEEDED)
      توڑو;

    /* analyze response */

    /*   GSS-API response looks like
     * +----+------+-----+----------------+
     * |VER | MTYP | LEN |     TOKEN      |
     * +----+------+----------------------+
     * | 1  |  1   |  2  | up to 2^16 - 1 |
     * +----+------+-----+----------------+
     */

    نتیجہ = Curl_blockread_all(cf, ڈیٹا, (حرف *)socksreq, 4, &actualread);
    اگر(نتیجہ || (actualread != 4)) {
      failf(ڈیٹا, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      واپس CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    اگر(socksreq[1] == 255) { /* status / message type */
      failf(ڈیٹا, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      واپس CURLE_COULDNT_CONNECT;
    }

    اگر(socksreq[1] != 1) { /* status / message type */
      failf(ڈیٹا, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      واپس CURLE_COULDNT_CONNECT;
    }

    حافظہ_نقل(&us_لمبائی, socksreq + 2, حجم(مختصر));
    us_لمبائی = ntohs(us_لمبائی);

    اگر(!us_لمبائی) {
      failf(ڈیٹا, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      واپس CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.لمبائی = us_لمبائی;
    gss_recv_token.قدر = curlx_malloc(gss_recv_token.لمبائی);
    اگر(!gss_recv_token.قدر) {
      failf(ڈیٹا,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      واپس CURLE_برآمد_کا_حافظہ;
    }

    نتیجہ = Curl_blockread_all(cf, ڈیٹا, (حرف *)gss_recv_token.قدر,
                                gss_recv_token.لمبائی, &actualread);

    اگر(نتیجہ || (actualread != us_لمبائی)) {
      failf(ڈیٹا, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_حالت, server_ptr);
      curlx_safefree(gss_recv_token.قدر);
      gss_recv_token.لمبائی = 0;
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      واپس CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_حالت, server_ptr);
  واپس CURLE_OK;
}

ساکن CURLcode socks5_gss_auth_verify(ساخت Curl_easy *ڈیٹا,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_بفر_desc gss_send_token = GSS_C_خالی_خانے_بفر;

  /* Everything is good so far, user was authenticated! */
  gss_major_حالت = gss_inquire_context(&gss_minor_حالت, *gss_context,
                                         &gss_client_name, تہی, تہی, تہی,
                                         تہی, تہی, تہی);
  اگر(check_gss_err(ڈیٹا, gss_major_حالت,
                   gss_minor_حالت, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
    gss_release_name(&gss_حالت, &gss_client_name);
    failf(ڈیٹا, "Failed to determine username.");
    واپس CURLE_COULDNT_CONNECT;
  }
  gss_major_حالت = gss_display_name(&gss_minor_حالت, gss_client_name,
                                      &gss_send_token, تہی);
  اگر(check_gss_err(ڈیٹا, gss_major_حالت,
                   gss_minor_حالت, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
    gss_release_name(&gss_حالت, &gss_client_name);
    gss_release_بفر(&gss_حالت, &gss_send_token);
    failf(ڈیٹا, "Failed to determine username.");
    واپس CURLE_COULDNT_CONNECT;
  }

  infof(ڈیٹا, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (صحیح)gss_send_token.لمبائی, (ثابت حرف *)gss_send_token.قدر);

  gss_release_name(&gss_حالت, &gss_client_name);
  gss_release_بفر(&gss_حالت, &gss_send_token);

  واپس CURLE_OK;
}

ساکن CURLcode socks5_gss_negotiate_enc(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  ساخت connectdata *conn = cf->conn;
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_بفر_desc gss_send_token = GSS_C_خالی_خانے_بفر;
  gss_بفر_desc gss_recv_token = GSS_C_خالی_خانے_بفر;
  gss_بفر_desc gss_w_token = GSS_C_خالی_خانے_بفر;
  غیرعلامتی مختصر us_لمبائی;
  غیرعلامتی حرف socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode نتیجہ;
  صحیح gss_enc;

  /* Do encryption */
  socksreq[0] = 1;    /* GSS-API subnegotiation version */
  socksreq[1] = 2;    /* encryption message type */

  gss_enc = 0; /* no data protection */
  /* do confidentiality protection if supported */
  اگر(gss_ret_flags & GSS_C_CONF_FLAG)
    gss_enc = 2;
  /* else do integrity protection */
  ورنہ اگر(gss_ret_flags & GSS_C_INTEG_FLAG)
    gss_enc = 1;

  infof(ڈیٹا, "SOCKS5 server supports GSS-API %s data protection.",
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
  اگر(ڈیٹا->سیٹ.socks5_gssapi_nec) {
    us_لمبائی = htons((مختصر)1);
    حافظہ_نقل(socksreq + 2, &us_لمبائی, حجم(مختصر));
  }
  ورنہ {
    gss_send_token.لمبائی = 1;
    gss_send_token.قدر = curlx_memdup(&gss_enc, gss_send_token.لمبائی);
    اگر(!gss_send_token.قدر) {
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      واپس CURLE_برآمد_کا_حافظہ;
    }

    gss_major_حالت = gss_wrap(&gss_minor_حالت, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                تہی, &gss_w_token);

    اگر(check_gss_err(ڈیٹا, gss_major_حالت, gss_minor_حالت, "gss_wrap")) {
      curlx_safefree(gss_send_token.قدر);
      gss_send_token.لمبائی = 0;
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      failf(ڈیٹا, "Failed to wrap GSS-API encryption value into token.");
      واپس CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.قدر);
    gss_send_token.لمبائی = 0;

    us_لمبائی = htons((غیرعلامتی مختصر)gss_w_token.لمبائی);
    حافظہ_نقل(socksreq + 2, &us_لمبائی, حجم(مختصر));
  }

  نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا, socksreq, 4, FALSE, &nwritten);
  اگر(نتیجہ || (nwritten != 4)) {
    failf(ڈیٹا, "Failed to send GSS-API encryption request.");
    gss_release_بفر(&gss_حالت, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
    واپس CURLE_COULDNT_CONNECT;
  }

  اگر(ڈیٹا->سیٹ.socks5_gssapi_nec) {
    حافظہ_نقل(socksreq, &gss_enc, 1);
    نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا, socksreq, 1, FALSE, &nwritten);
    اگر(نتیجہ || (nwritten != 1)) {
      failf(ڈیٹا, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      واپس CURLE_COULDNT_CONNECT;
    }
  }
  ورنہ {
    نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا, gss_w_token.قدر,
                               gss_w_token.لمبائی, FALSE, &nwritten);
    اگر(نتیجہ || (gss_w_token.لمبائی != nwritten)) {
      failf(ڈیٹا, "Failed to send GSS-API encryption type.");
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      واپس CURLE_COULDNT_CONNECT;
    }
    gss_release_بفر(&gss_حالت, &gss_w_token);
  }

  نتیجہ = Curl_blockread_all(cf, ڈیٹا, (حرف *)socksreq, 4, &actualread);
  اگر(نتیجہ || (actualread != 4)) {
    failf(ڈیٹا, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
    واپس CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  اگر(socksreq[1] == 255) { /* status / message type */
    failf(ڈیٹا, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
    واپس CURLE_COULDNT_CONNECT;
  }

  اگر(socksreq[1] != 2) { /* status / message type */
    failf(ڈیٹا, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
    واپس CURLE_COULDNT_CONNECT;
  }

  حافظہ_نقل(&us_لمبائی, socksreq + 2, حجم(مختصر));
  us_لمبائی = ntohs(us_لمبائی);

  اگر(!us_لمبائی) {
    failf(ڈیٹا, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
    واپس CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.لمبائی = us_لمبائی;
  gss_recv_token.قدر = curlx_malloc(gss_recv_token.لمبائی);
  اگر(!gss_recv_token.قدر) {
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
    واپس CURLE_برآمد_کا_حافظہ;
  }
  نتیجہ = Curl_blockread_all(cf, ڈیٹا, (حرف *)gss_recv_token.قدر,
                              gss_recv_token.لمبائی, &actualread);

  اگر(نتیجہ || (actualread != us_لمبائی)) {
    failf(ڈیٹا, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.قدر);
    gss_recv_token.لمبائی = 0;
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
    واپس CURLE_COULDNT_CONNECT;
  }

  اگر(!ڈیٹا->سیٹ.socks5_gssapi_nec) {
    gss_major_حالت = gss_unwrap(&gss_minor_حالت, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  تہی, تہی);

    اگر(check_gss_err(ڈیٹا, gss_major_حالت, gss_minor_حالت, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.قدر);
      gss_recv_token.لمبائی = 0;
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      failf(ڈیٹا, "Failed to unwrap GSS-API encryption value into token.");
      واپس CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.قدر);
    gss_recv_token.لمبائی = 0;

    اگر(gss_w_token.لمبائی != 1) {
      failf(ڈیٹا, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.لمبائی);
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      واپس CURLE_COULDNT_CONNECT;
    }

    حافظہ_نقل(socksreq, gss_w_token.قدر, gss_w_token.لمبائی);
    gss_release_بفر(&gss_حالت, &gss_w_token);
  }
  ورنہ {
    اگر(gss_recv_token.لمبائی != 1) {
      failf(ڈیٹا, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.لمبائی);
      curlx_safefree(gss_recv_token.قدر);
      gss_recv_token.لمبائی = 0;
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);
      واپس CURLE_COULDNT_CONNECT;
    }

    حافظہ_نقل(socksreq, gss_recv_token.قدر, gss_recv_token.لمبائی);
    curlx_safefree(gss_recv_token.قدر);
    gss_recv_token.لمبائی = 0;
  }

  infof(ڈیٹا, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  اگر(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, تہی);

  واپس CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      ساخت Curl_creds *creds)
{
  ساخت connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode نتیجہ;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  نتیجہ = socks5_gss_create_service_name(ڈیٹا, conn, service, &server);
  اگر(!نتیجہ) {
    (خالی)curlx_nonblock(sock, FALSE);
    نتیجہ = socks5_gss_auth_loop(cf, ڈیٹا, &server, &gss_context,
                                  &gss_ret_flags);
  }
  اگر(!نتیجہ)
    نتیجہ = socks5_gss_auth_verify(ڈیٹا, &gss_context);
  اگر(!نتیجہ)
    نتیجہ = socks5_gss_negotiate_enc(cf, ڈیٹا, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (خالی)curlx_nonblock(sock, TRUE);

  واپس نتیجہ;
}

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ہدایت GCC diagnostic نکالو
#اگر_ختم

#اگر_ختم /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
