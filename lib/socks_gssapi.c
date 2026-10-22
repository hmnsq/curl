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

#यदि defined(HAVE_GSSAPI) && !defined(CURL_DISABLE_PROXY)

#अन्तर्भाव "curl_gssapi.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curlx/nonblock.h"
#अन्तर्भाव "socks.h"
#अन्तर्भाव "curlx/strdup.h"

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wdeprecated-declarations"
#यदि_अन्तः

#परिभाषय MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
स्थिर पूर्णाङ्क check_gss_err(संरचना Curl_easy *दत्त,
                         OM_uint32 major_स्थितिः,
                         OM_uint32 minor_स्थितिः,
                         ध्रुव वर्ण *function)
{
  यदि(GSS_ERROR(major_स्थितिः)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 सन्देशः_ctx = 0;
    gss_बफर_desc स्थितिः_string = GSS_C_रिक्तानि_बफर;
    संरचना dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    सन्देशः_ctx = 0;
    यावत्(!सन्देशः_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_स्थितिः(&min_stat, major_स्थितिः,
                                    GSS_C_GSS_संहिता,
                                    GSS_C_NULL_OID,
                                    &सन्देशः_ctx, &स्थितिः_string);
      यदि(maj_stat == GSS_S_COMPLETE) {
        यदि(curlx_dyn_addn(&dbuf, स्थितिः_string.मानम्, स्थितिः_string.दीर्घता))
          प्रत्यागम 1; /* error */
        gss_release_बफर(&min_stat, &स्थितिः_string);
        भङ्ग;
      }
      gss_release_बफर(&min_stat, &स्थितिः_string);
    }
    यदि(curlx_dyn_addn(&dbuf, ".\n", 2))
      प्रत्यागम 1; /* error */
    सन्देशः_ctx = 0;
    यावत्(!सन्देशः_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_स्थितिः(&min_stat, minor_स्थितिः,
                                    GSS_C_MECH_संहिता,
                                    GSS_C_NULL_OID,
                                    &सन्देशः_ctx, &स्थितिः_string);
      यदि(maj_stat == GSS_S_COMPLETE) {
        यदि(curlx_dyn_addn(&dbuf, स्थितिः_string.मानम्, स्थितिः_string.दीर्घता))
          प्रत्यागम 1; /* error */
        gss_release_बफर(&min_stat, &स्थितिः_string);
        भङ्ग;
      }
      gss_release_बफर(&min_stat, &स्थितिः_string);
    }
    failf(दत्त, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    प्रत्यागम 1;
  }

  प्रत्यागम 0;
}

स्थिर CURLcode socks5_gss_create_service_name(संरचना Curl_easy *दत्त,
                                               संरचना connectdata *conn,
                                               ध्रुव वर्ण *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_स्थितिः, gss_minor_स्थितिः, gss_स्थितिः;
  gss_बफर_desc service = GSS_C_रिक्तानि_बफर;

  /* prepare service name */
  यदि(strchr(serviceptr, '/')) {
    service.दीर्घता = वाक्यदैर्घ्यम्(serviceptr);
    service.मानम् = curlx_memdup(serviceptr, service.दीर्घता);
    यदि(!service.मानम्)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    gss_major_स्थितिः = gss_import_name(&gss_minor_स्थितिः, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  अन्यथा {
    service.मानम् = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    यदि(!service.मानम्)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    service.दीर्घता = वाक्यदैर्घ्यम्(service.मानम्);

    gss_major_स्थितिः = gss_import_name(&gss_minor_स्थितिः, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.मानम्);
  service.दीर्घता = 0;

  यदि(check_gss_err(दत्त, gss_major_स्थितिः,
                   gss_minor_स्थितिः, "gss_import_name()")) {
    failf(दत्त, "Failed to create service name.");
    gss_release_name(&gss_स्थितिः, server);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode socks5_gss_auth_loop(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_स्थितिः, gss_minor_स्थितिः, gss_स्थितिः;
  gss_बफर_desc gss_send_token = GSS_C_रिक्तानि_बफर;
  gss_बफर_desc gss_recv_token = GSS_C_रिक्तानि_बफर;
  gss_बफर_desc *gss_token = GSS_C_NO_बफर;
  अचिह्नित ह्रस्व us_दीर्घता;
  अचिह्नित वर्ण socksreq[4];
  size_t actualread;
  size_t nwritten;
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
  प्रति(;;) {
    gss_major_स्थितिः = Curl_gss_init_sec_context(दत्त,
                                                 &gss_minor_स्थितिः,
                                                 gss_context,
                                                 *server_ptr,
                                                 &Curl_krb5_mech_oid,
                                                 रिक्त,
                                                 gss_token,
                                                 &gss_send_token,
                                                 TRUE,
                                                 gss_ret_flags,
                                                 GSS_C_NO_CREDENTIAL);

    यदि(gss_token != GSS_C_NO_बफर) {
      curlx_safefree(gss_recv_token.मानम्);
      gss_recv_token.दीर्घता = 0;
    }
    यदि(check_gss_err(दत्त, gss_major_स्थितिः,
                     gss_minor_स्थितिः, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.दीर्घता > 0xffff)) {
      gss_release_name(&gss_स्थितिः, server_ptr);
      gss_release_बफर(&gss_स्थितिः, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      failf(दत्त, "Failed to initialize GSS-API token.");
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    यदि(gss_send_token.दीर्घता) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_दीर्घता = htons((अचिह्नित ह्रस्व)gss_send_token.दीर्घता);
      स्मृतिं_प्रतिलिख(socksreq + 2, &us_दीर्घता, परिमाण(ह्रस्व));

      परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त, socksreq, 4, FALSE,
                                 &nwritten);
      यदि(परिणाम || (nwritten != 4)) {
        failf(दत्त, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_स्थितिः, server_ptr);
        gss_release_बफर(&gss_स्थितिः, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
        प्रत्यागम CURLE_COULDNT_CONNECT;
      }

      परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त,
                                 gss_send_token.मानम्,
                                 gss_send_token.दीर्घता, FALSE, &nwritten);
      यदि(परिणाम || (gss_send_token.दीर्घता != nwritten)) {
        failf(दत्त, "Failed to send GSS-API authentication token.");
        gss_release_name(&gss_स्थितिः, server_ptr);
        gss_release_बफर(&gss_स्थितिः, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
        प्रत्यागम CURLE_COULDNT_CONNECT;
      }
    }

    gss_release_बफर(&gss_स्थितिः, &gss_send_token);
    यदि(gss_major_स्थितिः != GSS_S_CONTINUE_NEEDED)
      भङ्ग;

    /* analyze response */

    /*   GSS-API response looks like
     * +----+------+-----+----------------+
     * |VER | MTYP | LEN |     TOKEN      |
     * +----+------+----------------------+
     * | 1  |  1   |  2  | up to 2^16 - 1 |
     * +----+------+-----+----------------+
     */

    परिणाम = Curl_blockread_all(cf, दत्त, (वर्ण *)socksreq, 4, &actualread);
    यदि(परिणाम || (actualread != 4)) {
      failf(दत्त, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_स्थितिः, server_ptr);
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    यदि(socksreq[1] == 255) { /* status / message type */
      failf(दत्त, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_स्थितिः, server_ptr);
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    यदि(socksreq[1] != 1) { /* status / message type */
      failf(दत्त, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_स्थितिः, server_ptr);
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    स्मृतिं_प्रतिलिख(&us_दीर्घता, socksreq + 2, परिमाण(ह्रस्व));
    us_दीर्घता = ntohs(us_दीर्घता);

    यदि(!us_दीर्घता) {
      failf(दत्त, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_स्थितिः, server_ptr);
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.दीर्घता = us_दीर्घता;
    gss_recv_token.मानम् = curlx_malloc(gss_recv_token.दीर्घता);
    यदि(!gss_recv_token.मानम्) {
      failf(दत्त,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_स्थितिः, server_ptr);
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    परिणाम = Curl_blockread_all(cf, दत्त, (वर्ण *)gss_recv_token.मानम्,
                                gss_recv_token.दीर्घता, &actualread);

    यदि(परिणाम || (actualread != us_दीर्घता)) {
      failf(दत्त, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_स्थितिः, server_ptr);
      curlx_safefree(gss_recv_token.मानम्);
      gss_recv_token.दीर्घता = 0;
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_स्थितिः, server_ptr);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode socks5_gss_auth_verify(संरचना Curl_easy *दत्त,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_स्थितिः, gss_minor_स्थितिः, gss_स्थितिः;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_बफर_desc gss_send_token = GSS_C_रिक्तानि_बफर;

  /* Everything is good so far, user was authenticated! */
  gss_major_स्थितिः = gss_inquire_context(&gss_minor_स्थितिः, *gss_context,
                                         &gss_client_name, रिक्त, रिक्त, रिक्त,
                                         रिक्त, रिक्त, रिक्त);
  यदि(check_gss_err(दत्त, gss_major_स्थितिः,
                   gss_minor_स्थितिः, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
    gss_release_name(&gss_स्थितिः, &gss_client_name);
    failf(दत्त, "Failed to determine username.");
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }
  gss_major_स्थितिः = gss_display_name(&gss_minor_स्थितिः, gss_client_name,
                                      &gss_send_token, रिक्त);
  यदि(check_gss_err(दत्त, gss_major_स्थितिः,
                   gss_minor_स्थितिः, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
    gss_release_name(&gss_स्थितिः, &gss_client_name);
    gss_release_बफर(&gss_स्थितिः, &gss_send_token);
    failf(दत्त, "Failed to determine username.");
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  infof(दत्त, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (पूर्णाङ्क)gss_send_token.दीर्घता, (ध्रुव वर्ण *)gss_send_token.मानम्);

  gss_release_name(&gss_स्थितिः, &gss_client_name);
  gss_release_बफर(&gss_स्थितिः, &gss_send_token);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode socks5_gss_negotiate_enc(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  संरचना connectdata *conn = cf->conn;
  OM_uint32 gss_major_स्थितिः, gss_minor_स्थितिः, gss_स्थितिः;
  gss_बफर_desc gss_send_token = GSS_C_रिक्तानि_बफर;
  gss_बफर_desc gss_recv_token = GSS_C_रिक्तानि_बफर;
  gss_बफर_desc gss_w_token = GSS_C_रिक्तानि_बफर;
  अचिह्नित ह्रस्व us_दीर्घता;
  अचिह्नित वर्ण socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode परिणाम;
  पूर्णाङ्क gss_enc;

  /* Do encryption */
  socksreq[0] = 1;    /* GSS-API subnegotiation version */
  socksreq[1] = 2;    /* encryption message type */

  gss_enc = 0; /* no data protection */
  /* do confidentiality protection if supported */
  यदि(gss_ret_flags & GSS_C_CONF_FLAG)
    gss_enc = 2;
  /* else do integrity protection */
  अन्यथा यदि(gss_ret_flags & GSS_C_INTEG_FLAG)
    gss_enc = 1;

  infof(दत्त, "SOCKS5 server supports GSS-API %s data protection.",
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
  यदि(दत्त->समुच्चयः.socks5_gssapi_nec) {
    us_दीर्घता = htons((ह्रस्व)1);
    स्मृतिं_प्रतिलिख(socksreq + 2, &us_दीर्घता, परिमाण(ह्रस्व));
  }
  अन्यथा {
    gss_send_token.दीर्घता = 1;
    gss_send_token.मानम् = curlx_memdup(&gss_enc, gss_send_token.दीर्घता);
    यदि(!gss_send_token.मानम्) {
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    gss_major_स्थितिः = gss_wrap(&gss_minor_स्थितिः, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                रिक्त, &gss_w_token);

    यदि(check_gss_err(दत्त, gss_major_स्थितिः, gss_minor_स्थितिः, "gss_wrap")) {
      curlx_safefree(gss_send_token.मानम्);
      gss_send_token.दीर्घता = 0;
      gss_release_बफर(&gss_स्थितिः, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      failf(दत्त, "Failed to wrap GSS-API encryption value into token.");
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.मानम्);
    gss_send_token.दीर्घता = 0;

    us_दीर्घता = htons((अचिह्नित ह्रस्व)gss_w_token.दीर्घता);
    स्मृतिं_प्रतिलिख(socksreq + 2, &us_दीर्घता, परिमाण(ह्रस्व));
  }

  परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त, socksreq, 4, FALSE, &nwritten);
  यदि(परिणाम || (nwritten != 4)) {
    failf(दत्त, "Failed to send GSS-API encryption request.");
    gss_release_बफर(&gss_स्थितिः, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  यदि(दत्त->समुच्चयः.socks5_gssapi_nec) {
    स्मृतिं_प्रतिलिख(socksreq, &gss_enc, 1);
    परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त, socksreq, 1, FALSE, &nwritten);
    यदि(परिणाम || (nwritten != 1)) {
      failf(दत्त, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }
  }
  अन्यथा {
    परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त, gss_w_token.मानम्,
                               gss_w_token.दीर्घता, FALSE, &nwritten);
    यदि(परिणाम || (gss_w_token.दीर्घता != nwritten)) {
      failf(दत्त, "Failed to send GSS-API encryption type.");
      gss_release_बफर(&gss_स्थितिः, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }
    gss_release_बफर(&gss_स्थितिः, &gss_w_token);
  }

  परिणाम = Curl_blockread_all(cf, दत्त, (वर्ण *)socksreq, 4, &actualread);
  यदि(परिणाम || (actualread != 4)) {
    failf(दत्त, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  यदि(socksreq[1] == 255) { /* status / message type */
    failf(दत्त, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  यदि(socksreq[1] != 2) { /* status / message type */
    failf(दत्त, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  स्मृतिं_प्रतिलिख(&us_दीर्घता, socksreq + 2, परिमाण(ह्रस्व));
  us_दीर्घता = ntohs(us_दीर्घता);

  यदि(!us_दीर्घता) {
    failf(दत्त, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.दीर्घता = us_दीर्घता;
  gss_recv_token.मानम् = curlx_malloc(gss_recv_token.दीर्घता);
  यदि(!gss_recv_token.मानम्) {
    Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  परिणाम = Curl_blockread_all(cf, दत्त, (वर्ण *)gss_recv_token.मानम्,
                              gss_recv_token.दीर्घता, &actualread);

  यदि(परिणाम || (actualread != us_दीर्घता)) {
    failf(दत्त, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.मानम्);
    gss_recv_token.दीर्घता = 0;
    Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  यदि(!दत्त->समुच्चयः.socks5_gssapi_nec) {
    gss_major_स्थितिः = gss_unwrap(&gss_minor_स्थितिः, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  रिक्त, रिक्त);

    यदि(check_gss_err(दत्त, gss_major_स्थितिः, gss_minor_स्थितिः, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.मानम्);
      gss_recv_token.दीर्घता = 0;
      gss_release_बफर(&gss_स्थितिः, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      failf(दत्त, "Failed to unwrap GSS-API encryption value into token.");
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.मानम्);
    gss_recv_token.दीर्घता = 0;

    यदि(gss_w_token.दीर्घता != 1) {
      failf(दत्त, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.दीर्घता);
      gss_release_बफर(&gss_स्थितिः, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    स्मृतिं_प्रतिलिख(socksreq, gss_w_token.मानम्, gss_w_token.दीर्घता);
    gss_release_बफर(&gss_स्थितिः, &gss_w_token);
  }
  अन्यथा {
    यदि(gss_recv_token.दीर्घता != 1) {
      failf(दत्त, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.दीर्घता);
      curlx_safefree(gss_recv_token.मानम्);
      gss_recv_token.दीर्घता = 0;
      Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }

    स्मृतिं_प्रतिलिख(socksreq, gss_recv_token.मानम्, gss_recv_token.दीर्घता);
    curlx_safefree(gss_recv_token.मानम्);
    gss_recv_token.दीर्घता = 0;
  }

  infof(दत्त, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  यदि(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_स्थितिः, gss_context, रिक्त);

  प्रत्यागम CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      संरचना Curl_creds *creds)
{
  संरचना connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode परिणाम;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  परिणाम = socks5_gss_create_service_name(दत्त, conn, service, &server);
  यदि(!परिणाम) {
    (शून्य)curlx_nonblock(sock, FALSE);
    परिणाम = socks5_gss_auth_loop(cf, दत्त, &server, &gss_context,
                                  &gss_ret_flags);
  }
  यदि(!परिणाम)
    परिणाम = socks5_gss_auth_verify(दत्त, &gss_context);
  यदि(!परिणाम)
    परिणाम = socks5_gss_negotiate_enc(cf, दत्त, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (शून्य)curlx_nonblock(sock, TRUE);

  प्रत्यागम परिणाम;
}

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः

#यदि_अन्तः /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
