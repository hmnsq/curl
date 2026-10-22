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

#إذا defined(HAVE_GSSAPI) && !defined(CURL_DISABLE_PROXY)

#ضمن "curl_gssapi.h"
#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "curlx/nonblock.h"
#ضمن "socks.h"
#ضمن "curlx/strdup.h"

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wdeprecated-declarations"
#جے_انت

#تعریف_کرو MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
ساكن صحيح check_gss_err(بنية Curl_easy *انکڑے,
                         OM_uint32 major_حالت,
                         OM_uint32 minor_حالت,
                         ثابت حرف *function)
{
  إذا(GSS_ERROR(major_حالت)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 پیغام_ctx = 0;
    gss_بفر_desc حالت_string = GSS_C_خالی_تھاواں_بفر;
    بنية dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    پیغام_ctx = 0;
    طالما(!پیغام_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_حالت(&min_stat, major_حالت,
                                    GSS_C_GSS_کوڈ,
                                    GSS_C_NULL_OID,
                                    &پیغام_ctx, &حالت_string);
      إذا(maj_stat == GSS_S_COMPLETE) {
        إذا(curlx_dyn_addn(&dbuf, حالت_string.قدر, حالت_string.لمائی))
          أرجع 1; /* error */
        gss_release_بفر(&min_stat, &حالت_string);
        اكسر;
      }
      gss_release_بفر(&min_stat, &حالت_string);
    }
    إذا(curlx_dyn_addn(&dbuf, ".\n", 2))
      أرجع 1; /* error */
    پیغام_ctx = 0;
    طالما(!پیغام_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_حالت(&min_stat, minor_حالت,
                                    GSS_C_MECH_کوڈ,
                                    GSS_C_NULL_OID,
                                    &پیغام_ctx, &حالت_string);
      إذا(maj_stat == GSS_S_COMPLETE) {
        إذا(curlx_dyn_addn(&dbuf, حالت_string.قدر, حالت_string.لمائی))
          أرجع 1; /* error */
        gss_release_بفر(&min_stat, &حالت_string);
        اكسر;
      }
      gss_release_بفر(&min_stat, &حالت_string);
    }
    failf(انکڑے, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    أرجع 1;
  }

  أرجع 0;
}

ساكن CURLcode socks5_gss_create_service_name(بنية Curl_easy *انکڑے,
                                               بنية connectdata *conn,
                                               ثابت حرف *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_بفر_desc service = GSS_C_خالی_تھاواں_بفر;

  /* prepare service name */
  إذا(strchr(serviceptr, '/')) {
    service.لمائی = لڑی_لمبائی(serviceptr);
    service.قدر = curlx_memdup(serviceptr, service.لمائی);
    إذا(!service.قدر)
      أرجع CURLE_باہرلا_دا_حافظہ;

    gss_major_حالت = gss_import_name(&gss_minor_حالت, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  وإلا {
    service.قدر = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    إذا(!service.قدر)
      أرجع CURLE_باہرلا_دا_حافظہ;
    service.لمائی = لڑی_لمبائی(service.قدر);

    gss_major_حالت = gss_import_name(&gss_minor_حالت, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.قدر);
  service.لمائی = 0;

  إذا(check_gss_err(انکڑے, gss_major_حالت,
                   gss_minor_حالت, "gss_import_name()")) {
    failf(انکڑے, "Failed to create service name.");
    gss_release_name(&gss_حالت, server);
    أرجع CURLE_COULDNT_CONNECT;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode socks5_gss_auth_loop(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *انکڑے,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_بفر_desc gss_send_token = GSS_C_خالی_تھاواں_بفر;
  gss_بفر_desc gss_recv_token = GSS_C_خالی_تھاواں_بفر;
  gss_بفر_desc *gss_token = GSS_C_NO_بفر;
  بنانشانی چھوٹا us_لمائی;
  بنانشانی حرف socksreq[4];
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
  لكل(;;) {
    gss_major_حالت = Curl_gss_init_sec_context(انکڑے,
                                                 &gss_minor_حالت,
                                                 gss_context,
                                                 *server_ptr,
                                                 &Curl_krb5_mech_oid,
                                                 عدم,
                                                 gss_token,
                                                 &gss_send_token,
                                                 TRUE,
                                                 gss_ret_flags,
                                                 GSS_C_NO_CREDENTIAL);

    إذا(gss_token != GSS_C_NO_بفر) {
      curlx_safefree(gss_recv_token.قدر);
      gss_recv_token.لمائی = 0;
    }
    إذا(check_gss_err(انکڑے, gss_major_حالت,
                     gss_minor_حالت, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.لمائی > 0xffff)) {
      gss_release_name(&gss_حالت, server_ptr);
      gss_release_بفر(&gss_حالت, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      failf(انکڑے, "Failed to initialize GSS-API token.");
      أرجع CURLE_COULDNT_CONNECT;
    }

    إذا(gss_send_token.لمائی) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_لمائی = htons((بنانشانی چھوٹا)gss_send_token.لمائی);
      حافظہ_نقل(socksreq + 2, &us_لمائی, حجم(چھوٹا));

      نتیجہ = Curl_conn_cf_send(cf->اگلا, انکڑے, socksreq, 4, FALSE,
                                 &nwritten);
      إذا(نتیجہ || (nwritten != 4)) {
        failf(انکڑے, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_حالت, server_ptr);
        gss_release_بفر(&gss_حالت, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
        أرجع CURLE_COULDNT_CONNECT;
      }

      نتیجہ = Curl_conn_cf_send(cf->اگلا, انکڑے,
                                 gss_send_token.قدر,
                                 gss_send_token.لمائی, FALSE, &nwritten);
      إذا(نتیجہ || (gss_send_token.لمائی != nwritten)) {
        failf(انکڑے, "Failed to send GSS-API authentication token.");
        gss_release_name(&gss_حالت, server_ptr);
        gss_release_بفر(&gss_حالت, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
        أرجع CURLE_COULDNT_CONNECT;
      }
    }

    gss_release_بفر(&gss_حالت, &gss_send_token);
    إذا(gss_major_حالت != GSS_S_CONTINUE_NEEDED)
      اكسر;

    /* analyze response */

    /*   GSS-API response looks like
     * +----+------+-----+----------------+
     * |VER | MTYP | LEN |     TOKEN      |
     * +----+------+----------------------+
     * | 1  |  1   |  2  | up to 2^16 - 1 |
     * +----+------+-----+----------------+
     */

    نتیجہ = Curl_blockread_all(cf, انکڑے, (حرف *)socksreq, 4, &actualread);
    إذا(نتیجہ || (actualread != 4)) {
      failf(انکڑے, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    إذا(socksreq[1] == 255) { /* status / message type */
      failf(انکڑے, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] != 1) { /* status / message type */
      failf(انکڑے, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    حافظہ_نقل(&us_لمائی, socksreq + 2, حجم(چھوٹا));
    us_لمائی = ntohs(us_لمائی);

    إذا(!us_لمائی) {
      failf(انکڑے, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.لمائی = us_لمائی;
    gss_recv_token.قدر = curlx_malloc(gss_recv_token.لمائی);
    إذا(!gss_recv_token.قدر) {
      failf(انکڑے,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }

    نتیجہ = Curl_blockread_all(cf, انکڑے, (حرف *)gss_recv_token.قدر,
                                gss_recv_token.لمائی, &actualread);

    إذا(نتیجہ || (actualread != us_لمائی)) {
      failf(انکڑے, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_حالت, server_ptr);
      curlx_safefree(gss_recv_token.قدر);
      gss_recv_token.لمائی = 0;
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_حالت, server_ptr);
  أرجع CURLE_OK;
}

ساكن CURLcode socks5_gss_auth_verify(بنية Curl_easy *انکڑے,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_بفر_desc gss_send_token = GSS_C_خالی_تھاواں_بفر;

  /* Everything is good so far, user was authenticated! */
  gss_major_حالت = gss_inquire_context(&gss_minor_حالت, *gss_context,
                                         &gss_client_name, عدم, عدم, عدم,
                                         عدم, عدم, عدم);
  إذا(check_gss_err(انکڑے, gss_major_حالت,
                   gss_minor_حالت, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    gss_release_name(&gss_حالت, &gss_client_name);
    failf(انکڑے, "Failed to determine username.");
    أرجع CURLE_COULDNT_CONNECT;
  }
  gss_major_حالت = gss_display_name(&gss_minor_حالت, gss_client_name,
                                      &gss_send_token, عدم);
  إذا(check_gss_err(انکڑے, gss_major_حالت,
                   gss_minor_حالت, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    gss_release_name(&gss_حالت, &gss_client_name);
    gss_release_بفر(&gss_حالت, &gss_send_token);
    failf(انکڑے, "Failed to determine username.");
    أرجع CURLE_COULDNT_CONNECT;
  }

  infof(انکڑے, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (صحيح)gss_send_token.لمائی, (ثابت حرف *)gss_send_token.قدر);

  gss_release_name(&gss_حالت, &gss_client_name);
  gss_release_بفر(&gss_حالت, &gss_send_token);

  أرجع CURLE_OK;
}

ساكن CURLcode socks5_gss_negotiate_enc(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *انکڑے,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  بنية connectdata *conn = cf->conn;
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_بفر_desc gss_send_token = GSS_C_خالی_تھاواں_بفر;
  gss_بفر_desc gss_recv_token = GSS_C_خالی_تھاواں_بفر;
  gss_بفر_desc gss_w_token = GSS_C_خالی_تھاواں_بفر;
  بنانشانی چھوٹا us_لمائی;
  بنانشانی حرف socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode نتیجہ;
  صحيح gss_enc;

  /* Do encryption */
  socksreq[0] = 1;    /* GSS-API subnegotiation version */
  socksreq[1] = 2;    /* encryption message type */

  gss_enc = 0; /* no data protection */
  /* do confidentiality protection if supported */
  إذا(gss_ret_flags & GSS_C_CONF_FLAG)
    gss_enc = 2;
  /* else do integrity protection */
  وإلا إذا(gss_ret_flags & GSS_C_INTEG_FLAG)
    gss_enc = 1;

  infof(انکڑے, "SOCKS5 server supports GSS-API %s data protection.",
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
  إذا(انکڑے->سیٹ.socks5_gssapi_nec) {
    us_لمائی = htons((چھوٹا)1);
    حافظہ_نقل(socksreq + 2, &us_لمائی, حجم(چھوٹا));
  }
  وإلا {
    gss_send_token.لمائی = 1;
    gss_send_token.قدر = curlx_memdup(&gss_enc, gss_send_token.لمائی);
    إذا(!gss_send_token.قدر) {
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }

    gss_major_حالت = gss_wrap(&gss_minor_حالت, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                عدم, &gss_w_token);

    إذا(check_gss_err(انکڑے, gss_major_حالت, gss_minor_حالت, "gss_wrap")) {
      curlx_safefree(gss_send_token.قدر);
      gss_send_token.لمائی = 0;
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      failf(انکڑے, "Failed to wrap GSS-API encryption value into token.");
      أرجع CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.قدر);
    gss_send_token.لمائی = 0;

    us_لمائی = htons((بنانشانی چھوٹا)gss_w_token.لمائی);
    حافظہ_نقل(socksreq + 2, &us_لمائی, حجم(چھوٹا));
  }

  نتیجہ = Curl_conn_cf_send(cf->اگلا, انکڑے, socksreq, 4, FALSE, &nwritten);
  إذا(نتیجہ || (nwritten != 4)) {
    failf(انکڑے, "Failed to send GSS-API encryption request.");
    gss_release_بفر(&gss_حالت, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(انکڑے->سیٹ.socks5_gssapi_nec) {
    حافظہ_نقل(socksreq, &gss_enc, 1);
    نتیجہ = Curl_conn_cf_send(cf->اگلا, انکڑے, socksreq, 1, FALSE, &nwritten);
    إذا(نتیجہ || (nwritten != 1)) {
      failf(انکڑے, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  وإلا {
    نتیجہ = Curl_conn_cf_send(cf->اگلا, انکڑے, gss_w_token.قدر,
                               gss_w_token.لمائی, FALSE, &nwritten);
    إذا(نتیجہ || (gss_w_token.لمائی != nwritten)) {
      failf(انکڑے, "Failed to send GSS-API encryption type.");
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }
    gss_release_بفر(&gss_حالت, &gss_w_token);
  }

  نتیجہ = Curl_blockread_all(cf, انکڑے, (حرف *)socksreq, 4, &actualread);
  إذا(نتیجہ || (actualread != 4)) {
    failf(انکڑے, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  إذا(socksreq[1] == 255) { /* status / message type */
    failf(انکڑے, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] != 2) { /* status / message type */
    failf(انکڑے, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  حافظہ_نقل(&us_لمائی, socksreq + 2, حجم(چھوٹا));
  us_لمائی = ntohs(us_لمائی);

  إذا(!us_لمائی) {
    failf(انکڑے, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.لمائی = us_لمائی;
  gss_recv_token.قدر = curlx_malloc(gss_recv_token.لمائی);
  إذا(!gss_recv_token.قدر) {
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_باہرلا_دا_حافظہ;
  }
  نتیجہ = Curl_blockread_all(cf, انکڑے, (حرف *)gss_recv_token.قدر,
                              gss_recv_token.لمائی, &actualread);

  إذا(نتیجہ || (actualread != us_لمائی)) {
    failf(انکڑے, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.قدر);
    gss_recv_token.لمائی = 0;
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(!انکڑے->سیٹ.socks5_gssapi_nec) {
    gss_major_حالت = gss_unwrap(&gss_minor_حالت, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  عدم, عدم);

    إذا(check_gss_err(انکڑے, gss_major_حالت, gss_minor_حالت, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.قدر);
      gss_recv_token.لمائی = 0;
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      failf(انکڑے, "Failed to unwrap GSS-API encryption value into token.");
      أرجع CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.قدر);
    gss_recv_token.لمائی = 0;

    إذا(gss_w_token.لمائی != 1) {
      failf(انکڑے, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.لمائی);
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    حافظہ_نقل(socksreq, gss_w_token.قدر, gss_w_token.لمائی);
    gss_release_بفر(&gss_حالت, &gss_w_token);
  }
  وإلا {
    إذا(gss_recv_token.لمائی != 1) {
      failf(انکڑے, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.لمائی);
      curlx_safefree(gss_recv_token.قدر);
      gss_recv_token.لمائی = 0;
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    حافظہ_نقل(socksreq, gss_recv_token.قدر, gss_recv_token.لمائی);
    curlx_safefree(gss_recv_token.قدر);
    gss_recv_token.لمائی = 0;
  }

  infof(انکڑے, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  إذا(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);

  أرجع CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے,
                                      بنية Curl_creds *creds)
{
  بنية connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode نتیجہ;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  نتیجہ = socks5_gss_create_service_name(انکڑے, conn, service, &server);
  إذا(!نتیجہ) {
    (فراغ)curlx_nonblock(sock, FALSE);
    نتیجہ = socks5_gss_auth_loop(cf, انکڑے, &server, &gss_context,
                                  &gss_ret_flags);
  }
  إذا(!نتیجہ)
    نتیجہ = socks5_gss_auth_verify(انکڑے, &gss_context);
  إذا(!نتیجہ)
    نتیجہ = socks5_gss_negotiate_enc(cf, انکڑے, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (فراغ)curlx_nonblock(sock, TRUE);

  أرجع نتیجہ;
}

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ہدایت GCC diagnostic نکالو
#جے_انت

#جے_انت /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
