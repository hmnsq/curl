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

#যদি defined(HAVE_GSSAPI) && !defined(CURL_DISABLE_PROXY)

#অন্তর্ভাব "curl_gssapi.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curlx/nonblock.h"
#অন্তর্ভাব "socks.h"
#অন্তর্ভাব "curlx/strdup.h"

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#define MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
স্থির পূর্ণাঙ্ক check_gss_err(সংরচনা Curl_easy *data,
                         OM_uint32 major_status,
                         OM_uint32 minor_status,
                         ধ্রুব বর্ণ *function)
{
  যদি(GSS_ERROR(major_status)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 বার্তা_ctx = 0;
    gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;
    সংরচনা dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    বার্তা_ctx = 0;
    যাবত্(!বার্তা_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_status(&min_stat, major_status,
                                    GSS_C_GSS_CODE,
                                    GSS_C_NULL_OID,
                                    &বার্তা_ctx, &status_string);
      যদি(maj_stat == GSS_S_COMPLETE) {
        যদি(curlx_dyn_addn(&dbuf, status_string.মান, status_string.length))
          প্রত্যাগম 1; /* error */
        gss_release_buffer(&min_stat, &status_string);
        ভঙ্গ;
      }
      gss_release_buffer(&min_stat, &status_string);
    }
    যদি(curlx_dyn_addn(&dbuf, ".\n", 2))
      প্রত্যাগম 1; /* error */
    বার্তা_ctx = 0;
    যাবত্(!বার্তা_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_status(&min_stat, minor_status,
                                    GSS_C_MECH_CODE,
                                    GSS_C_NULL_OID,
                                    &বার্তা_ctx, &status_string);
      যদি(maj_stat == GSS_S_COMPLETE) {
        যদি(curlx_dyn_addn(&dbuf, status_string.মান, status_string.length))
          প্রত্যাগম 1; /* error */
        gss_release_buffer(&min_stat, &status_string);
        ভঙ্গ;
      }
      gss_release_buffer(&min_stat, &status_string);
    }
    failf(data, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    প্রত্যাগম 1;
  }

  প্রত্যাগম 0;
}

স্থির CURLcode socks5_gss_create_service_name(সংরচনা Curl_easy *data,
                                               সংরচনা connectdata *conn,
                                               ধ্রুব বর্ণ *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc service = GSS_C_EMPTY_BUFFER;

  /* prepare service name */
  যদি(strchr(serviceptr, '/')) {
    service.length = strlen(serviceptr);
    service.মান = curlx_memdup(serviceptr, service.length);
    যদি(!service.মান)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;

    gss_major_status = gss_import_name(&gss_minor_status, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  অন্যথা {
    service.মান = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    যদি(!service.মান)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    service.length = strlen(service.মান);

    gss_major_status = gss_import_name(&gss_minor_status, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.মান);
  service.length = 0;

  যদি(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_import_name()")) {
    failf(data, "Failed to create service name.");
    gss_release_name(&gss_status, server);
    প্রত্যাগম CURLE_COULDNT_CONNECT;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode socks5_gss_auth_loop(সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_recv_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc *gss_token = GSS_C_NO_BUFFER;
  unsigned short us_length;
  unsigned বর্ণ socksreq[4];
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
  প্রতি(;;) {
    gss_major_status = Curl_gss_init_sec_context(data,
                                                 &gss_minor_status,
                                                 gss_context,
                                                 *server_ptr,
                                                 &Curl_krb5_mech_oid,
                                                 রিক্ত,
                                                 gss_token,
                                                 &gss_send_token,
                                                 TRUE,
                                                 gss_ret_flags,
                                                 GSS_C_NO_CREDENTIAL);

    যদি(gss_token != GSS_C_NO_BUFFER) {
      curlx_safefree(gss_recv_token.মান);
      gss_recv_token.length = 0;
    }
    যদি(check_gss_err(data, gss_major_status,
                     gss_minor_status, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.length > 0xffff)) {
      gss_release_name(&gss_status, server_ptr);
      gss_release_buffer(&gss_status, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      failf(data, "Failed to initialize GSS-API token.");
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }

    যদি(gss_send_token.length) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_length = htons((unsigned short)gss_send_token.length);
      memcpy(socksreq + 2, &us_length, পরিমাণ(short));

      result = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE,
                                 &nwritten);
      যদি(result || (nwritten != 4)) {
        failf(data, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_status, server_ptr);
        gss_release_buffer(&gss_status, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
        প্রত্যাগম CURLE_COULDNT_CONNECT;
      }

      result = Curl_conn_cf_send(cf->next, data,
                                 gss_send_token.মান,
                                 gss_send_token.length, FALSE, &nwritten);
      যদি(result || (gss_send_token.length != nwritten)) {
        failf(data, "Failed to send GSS-API authentication token.");
        gss_release_name(&gss_status, server_ptr);
        gss_release_buffer(&gss_status, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
        প্রত্যাগম CURLE_COULDNT_CONNECT;
      }
    }

    gss_release_buffer(&gss_status, &gss_send_token);
    যদি(gss_major_status != GSS_S_CONTINUE_NEEDED)
      ভঙ্গ;

    /* analyze response */

    /*   GSS-API response looks like
     * +----+------+-----+----------------+
     * |VER | MTYP | LEN |     TOKEN      |
     * +----+------+----------------------+
     * | 1  |  1   |  2  | up to 2^16 - 1 |
     * +----+------+-----+----------------+
     */

    result = Curl_blockread_all(cf, data, (বর্ণ *)socksreq, 4, &actualread);
    যদি(result || (actualread != 4)) {
      failf(data, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    যদি(socksreq[1] == 255) { /* status / message type */
      failf(data, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }

    যদি(socksreq[1] != 1) { /* status / message type */
      failf(data, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }

    memcpy(&us_length, socksreq + 2, পরিমাণ(short));
    us_length = ntohs(us_length);

    যদি(!us_length) {
      failf(data, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.length = us_length;
    gss_recv_token.মান = curlx_malloc(gss_recv_token.length);
    যদি(!gss_recv_token.মান) {
      failf(data,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }

    result = Curl_blockread_all(cf, data, (বর্ণ *)gss_recv_token.মান,
                                gss_recv_token.length, &actualread);

    যদি(result || (actualread != us_length)) {
      failf(data, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_status, server_ptr);
      curlx_safefree(gss_recv_token.মান);
      gss_recv_token.length = 0;
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_status, server_ptr);
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode socks5_gss_auth_verify(সংরচনা Curl_easy *data,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;

  /* Everything is good so far, user was authenticated! */
  gss_major_status = gss_inquire_context(&gss_minor_status, *gss_context,
                                         &gss_client_name, রিক্ত, রিক্ত, রিক্ত,
                                         রিক্ত, রিক্ত, রিক্ত);
  যদি(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
    gss_release_name(&gss_status, &gss_client_name);
    failf(data, "Failed to determine username.");
    প্রত্যাগম CURLE_COULDNT_CONNECT;
  }
  gss_major_status = gss_display_name(&gss_minor_status, gss_client_name,
                                      &gss_send_token, রিক্ত);
  যদি(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
    gss_release_name(&gss_status, &gss_client_name);
    gss_release_buffer(&gss_status, &gss_send_token);
    failf(data, "Failed to determine username.");
    প্রত্যাগম CURLE_COULDNT_CONNECT;
  }

  infof(data, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (পূর্ণাঙ্ক)gss_send_token.length, (ধ্রুব বর্ণ *)gss_send_token.মান);

  gss_release_name(&gss_status, &gss_client_name);
  gss_release_buffer(&gss_status, &gss_send_token);

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode socks5_gss_negotiate_enc(সংরচনা Curl_cfilter *cf,
                                         সংরচনা Curl_easy *data,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  সংরচনা connectdata *conn = cf->conn;
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_recv_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_w_token = GSS_C_EMPTY_BUFFER;
  unsigned short us_length;
  unsigned বর্ণ socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode result;
  পূর্ণাঙ্ক gss_enc;

  /* Do encryption */
  socksreq[0] = 1;    /* GSS-API subnegotiation version */
  socksreq[1] = 2;    /* encryption message type */

  gss_enc = 0; /* no data protection */
  /* do confidentiality protection if supported */
  যদি(gss_ret_flags & GSS_C_CONF_FLAG)
    gss_enc = 2;
  /* else do integrity protection */
  অন্যথা যদি(gss_ret_flags & GSS_C_INTEG_FLAG)
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
  যদি(data->set.socks5_gssapi_nec) {
    us_length = htons((short)1);
    memcpy(socksreq + 2, &us_length, পরিমাণ(short));
  }
  অন্যথা {
    gss_send_token.length = 1;
    gss_send_token.মান = curlx_memdup(&gss_enc, gss_send_token.length);
    যদি(!gss_send_token.মান) {
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }

    gss_major_status = gss_wrap(&gss_minor_status, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                রিক্ত, &gss_w_token);

    যদি(check_gss_err(data, gss_major_status, gss_minor_status, "gss_wrap")) {
      curlx_safefree(gss_send_token.মান);
      gss_send_token.length = 0;
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      failf(data, "Failed to wrap GSS-API encryption value into token.");
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.মান);
    gss_send_token.length = 0;

    us_length = htons((unsigned short)gss_w_token.length);
    memcpy(socksreq + 2, &us_length, পরিমাণ(short));
  }

  result = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE, &nwritten);
  যদি(result || (nwritten != 4)) {
    failf(data, "Failed to send GSS-API encryption request.");
    gss_release_buffer(&gss_status, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
    প্রত্যাগম CURLE_COULDNT_CONNECT;
  }

  যদি(data->set.socks5_gssapi_nec) {
    memcpy(socksreq, &gss_enc, 1);
    result = Curl_conn_cf_send(cf->next, data, socksreq, 1, FALSE, &nwritten);
    যদি(result || (nwritten != 1)) {
      failf(data, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }
  }
  অন্যথা {
    result = Curl_conn_cf_send(cf->next, data, gss_w_token.মান,
                               gss_w_token.length, FALSE, &nwritten);
    যদি(result || (gss_w_token.length != nwritten)) {
      failf(data, "Failed to send GSS-API encryption type.");
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }
    gss_release_buffer(&gss_status, &gss_w_token);
  }

  result = Curl_blockread_all(cf, data, (বর্ণ *)socksreq, 4, &actualread);
  যদি(result || (actualread != 4)) {
    failf(data, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
    প্রত্যাগম CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  যদি(socksreq[1] == 255) { /* status / message type */
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
    প্রত্যাগম CURLE_COULDNT_CONNECT;
  }

  যদি(socksreq[1] != 2) { /* status / message type */
    failf(data, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
    প্রত্যাগম CURLE_COULDNT_CONNECT;
  }

  memcpy(&us_length, socksreq + 2, পরিমাণ(short));
  us_length = ntohs(us_length);

  যদি(!us_length) {
    failf(data, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
    প্রত্যাগম CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.length = us_length;
  gss_recv_token.মান = curlx_malloc(gss_recv_token.length);
  যদি(!gss_recv_token.মান) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }
  result = Curl_blockread_all(cf, data, (বর্ণ *)gss_recv_token.মান,
                              gss_recv_token.length, &actualread);

  যদি(result || (actualread != us_length)) {
    failf(data, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.মান);
    gss_recv_token.length = 0;
    Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
    প্রত্যাগম CURLE_COULDNT_CONNECT;
  }

  যদি(!data->set.socks5_gssapi_nec) {
    gss_major_status = gss_unwrap(&gss_minor_status, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  রিক্ত, রিক্ত);

    যদি(check_gss_err(data, gss_major_status, gss_minor_status, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.মান);
      gss_recv_token.length = 0;
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      failf(data, "Failed to unwrap GSS-API encryption value into token.");
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.মান);
    gss_recv_token.length = 0;

    যদি(gss_w_token.length != 1) {
      failf(data, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.length);
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }

    memcpy(socksreq, gss_w_token.মান, gss_w_token.length);
    gss_release_buffer(&gss_status, &gss_w_token);
  }
  অন্যথা {
    যদি(gss_recv_token.length != 1) {
      failf(data, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.length);
      curlx_safefree(gss_recv_token.মান);
      gss_recv_token.length = 0;
      Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }

    memcpy(socksreq, gss_recv_token.মান, gss_recv_token.length);
    curlx_safefree(gss_recv_token.মান);
    gss_recv_token.length = 0;
  }

  infof(data, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  যদি(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_status, gss_context, রিক্ত);

  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      সংরচনা Curl_creds *creds)
{
  সংরচনা connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode result;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  ধ্রুব বর্ণ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  result = socks5_gss_create_service_name(data, conn, service, &server);
  যদি(!result) {
    (শূন্য)curlx_nonblock(sock, FALSE);
    result = socks5_gss_auth_loop(cf, data, &server, &gss_context,
                                  &gss_ret_flags);
  }
  যদি(!result)
    result = socks5_gss_auth_verify(data, &gss_context);
  যদি(!result)
    result = socks5_gss_negotiate_enc(cf, data, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (শূন্য)curlx_nonblock(sock, TRUE);

  প্রত্যাগম result;
}

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic বের_করো
#endif

#endif /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
