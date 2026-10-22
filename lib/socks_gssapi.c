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
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wdeprecated-declarations"
#جيڪڏهن_پورو

#وصف_ڪريو MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
ساكن صحيح check_gss_err(بنية Curl_easy *معلومات,
                         OM_uint32 major_حالت,
                         OM_uint32 minor_حالت,
                         ثابت حرف *function)
{
  إذا(GSS_ERROR(major_حالت)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 نياپو_ctx = 0;
    gss_بفر_desc حالت_string = GSS_C_خالي_جايون_بفر;
    بنية dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    نياپو_ctx = 0;
    طالما(!نياپو_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_حالت(&min_stat, major_حالت,
                                    GSS_C_GSS_ڪوڊ,
                                    GSS_C_NULL_OID,
                                    &نياپو_ctx, &حالت_string);
      إذا(maj_stat == GSS_S_COMPLETE) {
        إذا(curlx_dyn_addn(&dbuf, حالت_string.قدر, حالت_string.ڊيگهائي))
          أرجع 1; /* error */
        gss_release_بفر(&min_stat, &حالت_string);
        اكسر;
      }
      gss_release_بفر(&min_stat, &حالت_string);
    }
    إذا(curlx_dyn_addn(&dbuf, ".\n", 2))
      أرجع 1; /* error */
    نياپو_ctx = 0;
    طالما(!نياپو_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_حالت(&min_stat, minor_حالت,
                                    GSS_C_MECH_ڪوڊ,
                                    GSS_C_NULL_OID,
                                    &نياپو_ctx, &حالت_string);
      إذا(maj_stat == GSS_S_COMPLETE) {
        إذا(curlx_dyn_addn(&dbuf, حالت_string.قدر, حالت_string.ڊيگهائي))
          أرجع 1; /* error */
        gss_release_بفر(&min_stat, &حالت_string);
        اكسر;
      }
      gss_release_بفر(&min_stat, &حالت_string);
    }
    failf(معلومات, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    أرجع 1;
  }

  أرجع 0;
}

ساكن CURLcode socks5_gss_create_service_name(بنية Curl_easy *معلومات,
                                               بنية connectdata *conn,
                                               ثابت حرف *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_بفر_desc service = GSS_C_خالي_جايون_بفر;

  /* prepare service name */
  إذا(strchr(serviceptr, '/')) {
    service.ڊيگهائي = لڙي_ڊيگهه(serviceptr);
    service.قدر = curlx_memdup(serviceptr, service.ڊيگهائي);
    إذا(!service.قدر)
      أرجع CURLE_ٻاهريون_جو_يادگيري;

    gss_major_حالت = gss_import_name(&gss_minor_حالت, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  وإلا {
    service.قدر = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    إذا(!service.قدر)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    service.ڊيگهائي = لڙي_ڊيگهه(service.قدر);

    gss_major_حالت = gss_import_name(&gss_minor_حالت, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.قدر);
  service.ڊيگهائي = 0;

  إذا(check_gss_err(معلومات, gss_major_حالت,
                   gss_minor_حالت, "gss_import_name()")) {
    failf(معلومات, "Failed to create service name.");
    gss_release_name(&gss_حالت, server);
    أرجع CURLE_COULDNT_CONNECT;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode socks5_gss_auth_loop(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_بفر_desc gss_send_token = GSS_C_خالي_جايون_بفر;
  gss_بفر_desc gss_recv_token = GSS_C_خالي_جايون_بفر;
  gss_بفر_desc *gss_token = GSS_C_NO_بفر;
  بنانشان ننڍو us_ڊيگهائي;
  بنانشان حرف socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode نتيجو;

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
    gss_major_حالت = Curl_gss_init_sec_context(معلومات,
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
      gss_recv_token.ڊيگهائي = 0;
    }
    إذا(check_gss_err(معلومات, gss_major_حالت,
                     gss_minor_حالت, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.ڊيگهائي > 0xffff)) {
      gss_release_name(&gss_حالت, server_ptr);
      gss_release_بفر(&gss_حالت, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      failf(معلومات, "Failed to initialize GSS-API token.");
      أرجع CURLE_COULDNT_CONNECT;
    }

    إذا(gss_send_token.ڊيگهائي) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_ڊيگهائي = htons((بنانشان ننڍو)gss_send_token.ڊيگهائي);
      يادگيري_نقل(socksreq + 2, &us_ڊيگهائي, حجم(ننڍو));

      نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات, socksreq, 4, FALSE,
                                 &nwritten);
      إذا(نتيجو || (nwritten != 4)) {
        failf(معلومات, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_حالت, server_ptr);
        gss_release_بفر(&gss_حالت, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
        أرجع CURLE_COULDNT_CONNECT;
      }

      نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات,
                                 gss_send_token.قدر,
                                 gss_send_token.ڊيگهائي, FALSE, &nwritten);
      إذا(نتيجو || (gss_send_token.ڊيگهائي != nwritten)) {
        failf(معلومات, "Failed to send GSS-API authentication token.");
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

    نتيجو = Curl_blockread_all(cf, معلومات, (حرف *)socksreq, 4, &actualread);
    إذا(نتيجو || (actualread != 4)) {
      failf(معلومات, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    إذا(socksreq[1] == 255) { /* status / message type */
      failf(معلومات, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    إذا(socksreq[1] != 1) { /* status / message type */
      failf(معلومات, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    يادگيري_نقل(&us_ڊيگهائي, socksreq + 2, حجم(ننڍو));
    us_ڊيگهائي = ntohs(us_ڊيگهائي);

    إذا(!us_ڊيگهائي) {
      failf(معلومات, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.ڊيگهائي = us_ڊيگهائي;
    gss_recv_token.قدر = curlx_malloc(gss_recv_token.ڊيگهائي);
    إذا(!gss_recv_token.قدر) {
      failf(معلومات,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_حالت, server_ptr);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }

    نتيجو = Curl_blockread_all(cf, معلومات, (حرف *)gss_recv_token.قدر,
                                gss_recv_token.ڊيگهائي, &actualread);

    إذا(نتيجو || (actualread != us_ڊيگهائي)) {
      failf(معلومات, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_حالت, server_ptr);
      curlx_safefree(gss_recv_token.قدر);
      gss_recv_token.ڊيگهائي = 0;
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_حالت, server_ptr);
  أرجع CURLE_OK;
}

ساكن CURLcode socks5_gss_auth_verify(بنية Curl_easy *معلومات,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_بفر_desc gss_send_token = GSS_C_خالي_جايون_بفر;

  /* Everything is good so far, user was authenticated! */
  gss_major_حالت = gss_inquire_context(&gss_minor_حالت, *gss_context,
                                         &gss_client_name, عدم, عدم, عدم,
                                         عدم, عدم, عدم);
  إذا(check_gss_err(معلومات, gss_major_حالت,
                   gss_minor_حالت, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    gss_release_name(&gss_حالت, &gss_client_name);
    failf(معلومات, "Failed to determine username.");
    أرجع CURLE_COULDNT_CONNECT;
  }
  gss_major_حالت = gss_display_name(&gss_minor_حالت, gss_client_name,
                                      &gss_send_token, عدم);
  إذا(check_gss_err(معلومات, gss_major_حالت,
                   gss_minor_حالت, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    gss_release_name(&gss_حالت, &gss_client_name);
    gss_release_بفر(&gss_حالت, &gss_send_token);
    failf(معلومات, "Failed to determine username.");
    أرجع CURLE_COULDNT_CONNECT;
  }

  infof(معلومات, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (صحيح)gss_send_token.ڊيگهائي, (ثابت حرف *)gss_send_token.قدر);

  gss_release_name(&gss_حالت, &gss_client_name);
  gss_release_بفر(&gss_حالت, &gss_send_token);

  أرجع CURLE_OK;
}

ساكن CURLcode socks5_gss_negotiate_enc(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  بنية connectdata *conn = cf->conn;
  OM_uint32 gss_major_حالت, gss_minor_حالت, gss_حالت;
  gss_بفر_desc gss_send_token = GSS_C_خالي_جايون_بفر;
  gss_بفر_desc gss_recv_token = GSS_C_خالي_جايون_بفر;
  gss_بفر_desc gss_w_token = GSS_C_خالي_جايون_بفر;
  بنانشان ننڍو us_ڊيگهائي;
  بنانشان حرف socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode نتيجو;
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

  infof(معلومات, "SOCKS5 server supports GSS-API %s data protection.",
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
  إذا(معلومات->سيٽ.socks5_gssapi_nec) {
    us_ڊيگهائي = htons((ننڍو)1);
    يادگيري_نقل(socksreq + 2, &us_ڊيگهائي, حجم(ننڍو));
  }
  وإلا {
    gss_send_token.ڊيگهائي = 1;
    gss_send_token.قدر = curlx_memdup(&gss_enc, gss_send_token.ڊيگهائي);
    إذا(!gss_send_token.قدر) {
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }

    gss_major_حالت = gss_wrap(&gss_minor_حالت, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                عدم, &gss_w_token);

    إذا(check_gss_err(معلومات, gss_major_حالت, gss_minor_حالت, "gss_wrap")) {
      curlx_safefree(gss_send_token.قدر);
      gss_send_token.ڊيگهائي = 0;
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      failf(معلومات, "Failed to wrap GSS-API encryption value into token.");
      أرجع CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.قدر);
    gss_send_token.ڊيگهائي = 0;

    us_ڊيگهائي = htons((بنانشان ننڍو)gss_w_token.ڊيگهائي);
    يادگيري_نقل(socksreq + 2, &us_ڊيگهائي, حجم(ننڍو));
  }

  نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات, socksreq, 4, FALSE, &nwritten);
  إذا(نتيجو || (nwritten != 4)) {
    failf(معلومات, "Failed to send GSS-API encryption request.");
    gss_release_بفر(&gss_حالت, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(معلومات->سيٽ.socks5_gssapi_nec) {
    يادگيري_نقل(socksreq, &gss_enc, 1);
    نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات, socksreq, 1, FALSE, &nwritten);
    إذا(نتيجو || (nwritten != 1)) {
      failf(معلومات, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }
  }
  وإلا {
    نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات, gss_w_token.قدر,
                               gss_w_token.ڊيگهائي, FALSE, &nwritten);
    إذا(نتيجو || (gss_w_token.ڊيگهائي != nwritten)) {
      failf(معلومات, "Failed to send GSS-API encryption type.");
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }
    gss_release_بفر(&gss_حالت, &gss_w_token);
  }

  نتيجو = Curl_blockread_all(cf, معلومات, (حرف *)socksreq, 4, &actualread);
  إذا(نتيجو || (actualread != 4)) {
    failf(معلومات, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  إذا(socksreq[1] == 255) { /* status / message type */
    failf(معلومات, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(socksreq[1] != 2) { /* status / message type */
    failf(معلومات, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  يادگيري_نقل(&us_ڊيگهائي, socksreq + 2, حجم(ننڍو));
  us_ڊيگهائي = ntohs(us_ڊيگهائي);

  إذا(!us_ڊيگهائي) {
    failf(معلومات, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.ڊيگهائي = us_ڊيگهائي;
  gss_recv_token.قدر = curlx_malloc(gss_recv_token.ڊيگهائي);
  إذا(!gss_recv_token.قدر) {
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  نتيجو = Curl_blockread_all(cf, معلومات, (حرف *)gss_recv_token.قدر,
                              gss_recv_token.ڊيگهائي, &actualread);

  إذا(نتيجو || (actualread != us_ڊيگهائي)) {
    failf(معلومات, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.قدر);
    gss_recv_token.ڊيگهائي = 0;
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
    أرجع CURLE_COULDNT_CONNECT;
  }

  إذا(!معلومات->سيٽ.socks5_gssapi_nec) {
    gss_major_حالت = gss_unwrap(&gss_minor_حالت, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  عدم, عدم);

    إذا(check_gss_err(معلومات, gss_major_حالت, gss_minor_حالت, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.قدر);
      gss_recv_token.ڊيگهائي = 0;
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      failf(معلومات, "Failed to unwrap GSS-API encryption value into token.");
      أرجع CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.قدر);
    gss_recv_token.ڊيگهائي = 0;

    إذا(gss_w_token.ڊيگهائي != 1) {
      failf(معلومات, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.ڊيگهائي);
      gss_release_بفر(&gss_حالت, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    يادگيري_نقل(socksreq, gss_w_token.قدر, gss_w_token.ڊيگهائي);
    gss_release_بفر(&gss_حالت, &gss_w_token);
  }
  وإلا {
    إذا(gss_recv_token.ڊيگهائي != 1) {
      failf(معلومات, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.ڊيگهائي);
      curlx_safefree(gss_recv_token.قدر);
      gss_recv_token.ڊيگهائي = 0;
      Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);
      أرجع CURLE_COULDNT_CONNECT;
    }

    يادگيري_نقل(socksreq, gss_recv_token.قدر, gss_recv_token.ڊيگهائي);
    curlx_safefree(gss_recv_token.قدر);
    gss_recv_token.ڊيگهائي = 0;
  }

  infof(معلومات, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  إذا(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_حالت, gss_context, عدم);

  أرجع CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      بنية Curl_creds *creds)
{
  بنية connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode نتيجو;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  نتيجو = socks5_gss_create_service_name(معلومات, conn, service, &server);
  إذا(!نتيجو) {
    (فراغ)curlx_nonblock(sock, FALSE);
    نتيجو = socks5_gss_auth_loop(cf, معلومات, &server, &gss_context,
                                  &gss_ret_flags);
  }
  إذا(!نتيجو)
    نتيجو = socks5_gss_auth_verify(معلومات, &gss_context);
  إذا(!نتيجو)
    نتيجو = socks5_gss_negotiate_enc(cf, معلومات, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (فراغ)curlx_nonblock(sock, TRUE);

  أرجع نتيجو;
}

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
