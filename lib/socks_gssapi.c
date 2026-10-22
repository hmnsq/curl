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

#ఒకవేళ defined(HAVE_GSSAPI) && !defined(CURL_DISABLE_PROXY)

#అన్తర్భావ "curl_gssapi.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "curlx/nonblock.h"
#అన్తర్భావ "socks.h"
#అన్తర్భావ "curlx/strdup.h"

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wdeprecated-declarations"
#అయితే_ముగింపు

#నిర్వచించు MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
స్థిర పూర్ణాంకం check_gss_err(నిర్మాణం Curl_easy *దత్తాంశం,
                         OM_uint32 major_స్థితి,
                         OM_uint32 minor_స్థితి,
                         స్థిరరాశి అక్షరం *function)
{
  ఒకవేళ(GSS_ERROR(major_స్థితి)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 సందేశం_ctx = 0;
    gss_బఫరు_desc స్థితి_string = GSS_C_ఖాళీలు_బఫరు;
    నిర్మాణం dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    సందేశం_ctx = 0;
    అయితే(!సందేశం_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_స్థితి(&min_stat, major_స్థితి,
                                    GSS_C_GSS_సంకేతం,
                                    GSS_C_NULL_OID,
                                    &సందేశం_ctx, &స్థితి_string);
      ఒకవేళ(maj_stat == GSS_S_COMPLETE) {
        ఒకవేళ(curlx_dyn_addn(&dbuf, స్థితి_string.విలువ, స్థితి_string.నిడివి))
          తిరిగి 1; /* error */
        gss_release_బఫరు(&min_stat, &స్థితి_string);
        ఆపు;
      }
      gss_release_బఫరు(&min_stat, &స్థితి_string);
    }
    ఒకవేళ(curlx_dyn_addn(&dbuf, ".\n", 2))
      తిరిగి 1; /* error */
    సందేశం_ctx = 0;
    అయితే(!సందేశం_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_స్థితి(&min_stat, minor_స్థితి,
                                    GSS_C_MECH_సంకేతం,
                                    GSS_C_NULL_OID,
                                    &సందేశం_ctx, &స్థితి_string);
      ఒకవేళ(maj_stat == GSS_S_COMPLETE) {
        ఒకవేళ(curlx_dyn_addn(&dbuf, స్థితి_string.విలువ, స్థితి_string.నిడివి))
          తిరిగి 1; /* error */
        gss_release_బఫరు(&min_stat, &స్థితి_string);
        ఆపు;
      }
      gss_release_బఫరు(&min_stat, &స్థితి_string);
    }
    failf(దత్తాంశం, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    తిరిగి 1;
  }

  తిరిగి 0;
}

స్థిర CURLcode socks5_gss_create_service_name(నిర్మాణం Curl_easy *దత్తాంశం,
                                               నిర్మాణం connectdata *conn,
                                               స్థిరరాశి అక్షరం *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_స్థితి, gss_minor_స్థితి, gss_స్థితి;
  gss_బఫరు_desc service = GSS_C_ఖాళీలు_బఫరు;

  /* prepare service name */
  ఒకవేళ(strchr(serviceptr, '/')) {
    service.నిడివి = అక్షరశ్రేణి_పొడవు(serviceptr);
    service.విలువ = curlx_memdup(serviceptr, service.నిడివి);
    ఒకవేళ(!service.విలువ)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    gss_major_స్థితి = gss_import_name(&gss_minor_స్థితి, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  లేకపోతే {
    service.విలువ = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    ఒకవేళ(!service.విలువ)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    service.నిడివి = అక్షరశ్రేణి_పొడవు(service.విలువ);

    gss_major_స్థితి = gss_import_name(&gss_minor_స్థితి, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.విలువ);
  service.నిడివి = 0;

  ఒకవేళ(check_gss_err(దత్తాంశం, gss_major_స్థితి,
                   gss_minor_స్థితి, "gss_import_name()")) {
    failf(దత్తాంశం, "Failed to create service name.");
    gss_release_name(&gss_స్థితి, server);
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode socks5_gss_auth_loop(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_స్థితి, gss_minor_స్థితి, gss_స్థితి;
  gss_బఫరు_desc gss_send_token = GSS_C_ఖాళీలు_బఫరు;
  gss_బఫరు_desc gss_recv_token = GSS_C_ఖాళీలు_బఫరు;
  gss_బఫరు_desc *gss_token = GSS_C_NO_బఫరు;
  గుర్తులేని పొట్టి us_నిడివి;
  గుర్తులేని అక్షరం socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode ఫలితం;

  /*   GSS-API request looks like
   * +----+------+-----+----------------+
   * |VER | MTYP | LEN |     TOKEN      |
   * +----+------+----------------------+
   * | 1  |  1   |  2  | up to 2^16 - 1 |
   * +----+------+-----+----------------+
   */

  /* As long as we need to keep sending some context info, and there is no
   * errors, keep sending it... */
  కొరకు(;;) {
    gss_major_స్థితి = Curl_gss_init_sec_context(దత్తాంశం,
                                                 &gss_minor_స్థితి,
                                                 gss_context,
                                                 *server_ptr,
                                                 &Curl_krb5_mech_oid,
                                                 ఖాళీ,
                                                 gss_token,
                                                 &gss_send_token,
                                                 TRUE,
                                                 gss_ret_flags,
                                                 GSS_C_NO_CREDENTIAL);

    ఒకవేళ(gss_token != GSS_C_NO_బఫరు) {
      curlx_safefree(gss_recv_token.విలువ);
      gss_recv_token.నిడివి = 0;
    }
    ఒకవేళ(check_gss_err(దత్తాంశం, gss_major_స్థితి,
                     gss_minor_స్థితి, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.నిడివి > 0xffff)) {
      gss_release_name(&gss_స్థితి, server_ptr);
      gss_release_బఫరు(&gss_స్థితి, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      failf(దత్తాంశం, "Failed to initialize GSS-API token.");
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    ఒకవేళ(gss_send_token.నిడివి) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_నిడివి = htons((గుర్తులేని పొట్టి)gss_send_token.నిడివి);
      జ్ఞాపకం_నకలు(socksreq + 2, &us_నిడివి, పరిమాణం(పొట్టి));

      ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, socksreq, 4, FALSE,
                                 &nwritten);
      ఒకవేళ(ఫలితం || (nwritten != 4)) {
        failf(దత్తాంశం, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_స్థితి, server_ptr);
        gss_release_బఫరు(&gss_స్థితి, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
        తిరిగి CURLE_COULDNT_CONNECT;
      }

      ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం,
                                 gss_send_token.విలువ,
                                 gss_send_token.నిడివి, FALSE, &nwritten);
      ఒకవేళ(ఫలితం || (gss_send_token.నిడివి != nwritten)) {
        failf(దత్తాంశం, "Failed to send GSS-API authentication token.");
        gss_release_name(&gss_స్థితి, server_ptr);
        gss_release_బఫరు(&gss_స్థితి, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
        తిరిగి CURLE_COULDNT_CONNECT;
      }
    }

    gss_release_బఫరు(&gss_స్థితి, &gss_send_token);
    ఒకవేళ(gss_major_స్థితి != GSS_S_CONTINUE_NEEDED)
      ఆపు;

    /* analyze response */

    /*   GSS-API response looks like
     * +----+------+-----+----------------+
     * |VER | MTYP | LEN |     TOKEN      |
     * +----+------+----------------------+
     * | 1  |  1   |  2  | up to 2^16 - 1 |
     * +----+------+-----+----------------+
     */

    ఫలితం = Curl_blockread_all(cf, దత్తాంశం, (అక్షరం *)socksreq, 4, &actualread);
    ఒకవేళ(ఫలితం || (actualread != 4)) {
      failf(దత్తాంశం, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_స్థితి, server_ptr);
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    ఒకవేళ(socksreq[1] == 255) { /* status / message type */
      failf(దత్తాంశం, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_స్థితి, server_ptr);
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    ఒకవేళ(socksreq[1] != 1) { /* status / message type */
      failf(దత్తాంశం, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_స్థితి, server_ptr);
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    జ్ఞాపకం_నకలు(&us_నిడివి, socksreq + 2, పరిమాణం(పొట్టి));
    us_నిడివి = ntohs(us_నిడివి);

    ఒకవేళ(!us_నిడివి) {
      failf(దత్తాంశం, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_స్థితి, server_ptr);
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.నిడివి = us_నిడివి;
    gss_recv_token.విలువ = curlx_malloc(gss_recv_token.నిడివి);
    ఒకవేళ(!gss_recv_token.విలువ) {
      failf(దత్తాంశం,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_స్థితి, server_ptr);
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    ఫలితం = Curl_blockread_all(cf, దత్తాంశం, (అక్షరం *)gss_recv_token.విలువ,
                                gss_recv_token.నిడివి, &actualread);

    ఒకవేళ(ఫలితం || (actualread != us_నిడివి)) {
      failf(దత్తాంశం, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_స్థితి, server_ptr);
      curlx_safefree(gss_recv_token.విలువ);
      gss_recv_token.నిడివి = 0;
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_స్థితి, server_ptr);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode socks5_gss_auth_verify(నిర్మాణం Curl_easy *దత్తాంశం,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_స్థితి, gss_minor_స్థితి, gss_స్థితి;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_బఫరు_desc gss_send_token = GSS_C_ఖాళీలు_బఫరు;

  /* Everything is good so far, user was authenticated! */
  gss_major_స్థితి = gss_inquire_context(&gss_minor_స్థితి, *gss_context,
                                         &gss_client_name, ఖాళీ, ఖాళీ, ఖాళీ,
                                         ఖాళీ, ఖాళీ, ఖాళీ);
  ఒకవేళ(check_gss_err(దత్తాంశం, gss_major_స్థితి,
                   gss_minor_స్థితి, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
    gss_release_name(&gss_స్థితి, &gss_client_name);
    failf(దత్తాంశం, "Failed to determine username.");
    తిరిగి CURLE_COULDNT_CONNECT;
  }
  gss_major_స్థితి = gss_display_name(&gss_minor_స్థితి, gss_client_name,
                                      &gss_send_token, ఖాళీ);
  ఒకవేళ(check_gss_err(దత్తాంశం, gss_major_స్థితి,
                   gss_minor_స్థితి, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
    gss_release_name(&gss_స్థితి, &gss_client_name);
    gss_release_బఫరు(&gss_స్థితి, &gss_send_token);
    failf(దత్తాంశం, "Failed to determine username.");
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  infof(దత్తాంశం, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (పూర్ణాంకం)gss_send_token.నిడివి, (స్థిరరాశి అక్షరం *)gss_send_token.విలువ);

  gss_release_name(&gss_స్థితి, &gss_client_name);
  gss_release_బఫరు(&gss_స్థితి, &gss_send_token);

  తిరిగి CURLE_OK;
}

స్థిర CURLcode socks5_gss_negotiate_enc(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  నిర్మాణం connectdata *conn = cf->conn;
  OM_uint32 gss_major_స్థితి, gss_minor_స్థితి, gss_స్థితి;
  gss_బఫరు_desc gss_send_token = GSS_C_ఖాళీలు_బఫరు;
  gss_బఫరు_desc gss_recv_token = GSS_C_ఖాళీలు_బఫరు;
  gss_బఫరు_desc gss_w_token = GSS_C_ఖాళీలు_బఫరు;
  గుర్తులేని పొట్టి us_నిడివి;
  గుర్తులేని అక్షరం socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode ఫలితం;
  పూర్ణాంకం gss_enc;

  /* Do encryption */
  socksreq[0] = 1;    /* GSS-API subnegotiation version */
  socksreq[1] = 2;    /* encryption message type */

  gss_enc = 0; /* no data protection */
  /* do confidentiality protection if supported */
  ఒకవేళ(gss_ret_flags & GSS_C_CONF_FLAG)
    gss_enc = 2;
  /* else do integrity protection */
  లేకపోతే ఒకవేళ(gss_ret_flags & GSS_C_INTEG_FLAG)
    gss_enc = 1;

  infof(దత్తాంశం, "SOCKS5 server supports GSS-API %s data protection.",
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
  ఒకవేళ(దత్తాంశం->సమితి.socks5_gssapi_nec) {
    us_నిడివి = htons((పొట్టి)1);
    జ్ఞాపకం_నకలు(socksreq + 2, &us_నిడివి, పరిమాణం(పొట్టి));
  }
  లేకపోతే {
    gss_send_token.నిడివి = 1;
    gss_send_token.విలువ = curlx_memdup(&gss_enc, gss_send_token.నిడివి);
    ఒకవేళ(!gss_send_token.విలువ) {
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    gss_major_స్థితి = gss_wrap(&gss_minor_స్థితి, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                ఖాళీ, &gss_w_token);

    ఒకవేళ(check_gss_err(దత్తాంశం, gss_major_స్థితి, gss_minor_స్థితి, "gss_wrap")) {
      curlx_safefree(gss_send_token.విలువ);
      gss_send_token.నిడివి = 0;
      gss_release_బఫరు(&gss_స్థితి, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      failf(దత్తాంశం, "Failed to wrap GSS-API encryption value into token.");
      తిరిగి CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.విలువ);
    gss_send_token.నిడివి = 0;

    us_నిడివి = htons((గుర్తులేని పొట్టి)gss_w_token.నిడివి);
    జ్ఞాపకం_నకలు(socksreq + 2, &us_నిడివి, పరిమాణం(పొట్టి));
  }

  ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, socksreq, 4, FALSE, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten != 4)) {
    failf(దత్తాంశం, "Failed to send GSS-API encryption request.");
    gss_release_బఫరు(&gss_స్థితి, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  ఒకవేళ(దత్తాంశం->సమితి.socks5_gssapi_nec) {
    జ్ఞాపకం_నకలు(socksreq, &gss_enc, 1);
    ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, socksreq, 1, FALSE, &nwritten);
    ఒకవేళ(ఫలితం || (nwritten != 1)) {
      failf(దత్తాంశం, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      తిరిగి CURLE_COULDNT_CONNECT;
    }
  }
  లేకపోతే {
    ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, gss_w_token.విలువ,
                               gss_w_token.నిడివి, FALSE, &nwritten);
    ఒకవేళ(ఫలితం || (gss_w_token.నిడివి != nwritten)) {
      failf(దత్తాంశం, "Failed to send GSS-API encryption type.");
      gss_release_బఫరు(&gss_స్థితి, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      తిరిగి CURLE_COULDNT_CONNECT;
    }
    gss_release_బఫరు(&gss_స్థితి, &gss_w_token);
  }

  ఫలితం = Curl_blockread_all(cf, దత్తాంశం, (అక్షరం *)socksreq, 4, &actualread);
  ఒకవేళ(ఫలితం || (actualread != 4)) {
    failf(దత్తాంశం, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  ఒకవేళ(socksreq[1] == 255) { /* status / message type */
    failf(దత్తాంశం, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  ఒకవేళ(socksreq[1] != 2) { /* status / message type */
    failf(దత్తాంశం, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  జ్ఞాపకం_నకలు(&us_నిడివి, socksreq + 2, పరిమాణం(పొట్టి));
  us_నిడివి = ntohs(us_నిడివి);

  ఒకవేళ(!us_నిడివి) {
    failf(దత్తాంశం, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.నిడివి = us_నిడివి;
  gss_recv_token.విలువ = curlx_malloc(gss_recv_token.నిడివి);
  ఒకవేళ(!gss_recv_token.విలువ) {
    Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  ఫలితం = Curl_blockread_all(cf, దత్తాంశం, (అక్షరం *)gss_recv_token.విలువ,
                              gss_recv_token.నిడివి, &actualread);

  ఒకవేళ(ఫలితం || (actualread != us_నిడివి)) {
    failf(దత్తాంశం, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.విలువ);
    gss_recv_token.నిడివి = 0;
    Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  ఒకవేళ(!దత్తాంశం->సమితి.socks5_gssapi_nec) {
    gss_major_స్థితి = gss_unwrap(&gss_minor_స్థితి, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  ఖాళీ, ఖాళీ);

    ఒకవేళ(check_gss_err(దత్తాంశం, gss_major_స్థితి, gss_minor_స్థితి, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.విలువ);
      gss_recv_token.నిడివి = 0;
      gss_release_బఫరు(&gss_స్థితి, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      failf(దత్తాంశం, "Failed to unwrap GSS-API encryption value into token.");
      తిరిగి CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.విలువ);
    gss_recv_token.నిడివి = 0;

    ఒకవేళ(gss_w_token.నిడివి != 1) {
      failf(దత్తాంశం, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.నిడివి);
      gss_release_బఫరు(&gss_స్థితి, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    జ్ఞాపకం_నకలు(socksreq, gss_w_token.విలువ, gss_w_token.నిడివి);
    gss_release_బఫరు(&gss_స్థితి, &gss_w_token);
  }
  లేకపోతే {
    ఒకవేళ(gss_recv_token.నిడివి != 1) {
      failf(దత్తాంశం, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.నిడివి);
      curlx_safefree(gss_recv_token.విలువ);
      gss_recv_token.నిడివి = 0;
      Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);
      తిరిగి CURLE_COULDNT_CONNECT;
    }

    జ్ఞాపకం_నకలు(socksreq, gss_recv_token.విలువ, gss_recv_token.నిడివి);
    curlx_safefree(gss_recv_token.విలువ);
    gss_recv_token.నిడివి = 0;
  }

  infof(దత్తాంశం, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  ఒకవేళ(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_స్థితి, gss_context, ఖాళీ);

  తిరిగి CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం Curl_creds *creds)
{
  నిర్మాణం connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode ఫలితం;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  ఫలితం = socks5_gss_create_service_name(దత్తాంశం, conn, service, &server);
  ఒకవేళ(!ఫలితం) {
    (శూన్య)curlx_nonblock(sock, FALSE);
    ఫలితం = socks5_gss_auth_loop(cf, దత్తాంశం, &server, &gss_context,
                                  &gss_ret_flags);
  }
  ఒకవేళ(!ఫలితం)
    ఫలితం = socks5_gss_auth_verify(దత్తాంశం, &gss_context);
  ఒకవేళ(!ఫలితం)
    ఫలితం = socks5_gss_negotiate_enc(cf, దత్తాంశం, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (శూన్య)curlx_nonblock(sock, TRUE);

  తిరిగి ఫలితం;
}

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు

#అయితే_ముగింపు /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
