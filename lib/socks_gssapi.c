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
#include "curl_setup.h"

#ᬬᬾᬦ᭄ defined(HAVE_GSSAPI) && !defined(CURL_DISABLE_PROXY)

#include "curl_gssapi.h"
#include "urldata.h"
#include "curl_trc.h"
#include "cfilters.h"
#include "connect.h"
#include "curlx/nonblock.h"
#include "socks.h"
#include "curlx/strdup.h"

#ᬬᬾᬦ᭄ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#define MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
static puur_naa_nka check_gss_err(sa_nracanaa Curl_easy *data,
                         OM_uint32 major_status,
                         OM_uint32 minor_status,
                         const var_na *function)
{
  ᬬᬾᬦ᭄(GSS_ERROR(major_status)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 msg_ctx = 0;
    gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;
    sa_nracanaa dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    msg_ctx = 0;
    ᬲᭂᬤᭂᬮ(!msg_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_status(&min_stat, major_status,
                                    GSS_C_GSS_CODE,
                                    GSS_C_NULL_OID,
                                    &msg_ctx, &status_string);
      ᬬᬾᬦ᭄(maj_stat == GSS_S_COMPLETE) {
        ᬬᬾᬦ᭄(curlx_dyn_addn(&dbuf, status_string.value, status_string.length))
          ᬯᬮᬶ 1; /* error */
        gss_release_buffer(&min_stat, &status_string);
        bha_nga;
      }
      gss_release_buffer(&min_stat, &status_string);
    }
    ᬬᬾᬦ᭄(curlx_dyn_addn(&dbuf, ".\n", 2))
      ᬯᬮᬶ 1; /* error */
    msg_ctx = 0;
    ᬲᭂᬤᭂᬮ(!msg_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_status(&min_stat, minor_status,
                                    GSS_C_MECH_CODE,
                                    GSS_C_NULL_OID,
                                    &msg_ctx, &status_string);
      ᬬᬾᬦ᭄(maj_stat == GSS_S_COMPLETE) {
        ᬬᬾᬦ᭄(curlx_dyn_addn(&dbuf, status_string.value, status_string.length))
          ᬯᬮᬶ 1; /* error */
        gss_release_buffer(&min_stat, &status_string);
        bha_nga;
      }
      gss_release_buffer(&min_stat, &status_string);
    }
    failf(data, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    ᬯᬮᬶ 1;
  }

  ᬯᬮᬶ 0;
}

static CURLcode socks5_gss_create_service_name(sa_nracanaa Curl_easy *data,
                                               sa_nracanaa connectdata *conn,
                                               const var_na *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc service = GSS_C_EMPTY_BUFFER;

  /* prepare service name */
  ᬬᬾᬦ᭄(strchr(serviceptr, '/')) {
    service.length = strlen(serviceptr);
    service.value = curlx_memdup(serviceptr, service.length);
    ᬬᬾᬦ᭄(!service.value)
      ᬯᬮᬶ CURLE_OUT_OF_MEMORY;

    gss_major_status = gss_import_name(&gss_minor_status, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  ᬮᬶᬬ {
    service.value = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    ᬬᬾᬦ᭄(!service.value)
      ᬯᬮᬶ CURLE_OUT_OF_MEMORY;
    service.length = strlen(service.value);

    gss_major_status = gss_import_name(&gss_minor_status, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.value);
  service.length = 0;

  ᬬᬾᬦ᭄(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_import_name()")) {
    failf(data, "Failed to create service name.");
    gss_release_name(&gss_status, server);
    ᬯᬮᬶ CURLE_COULDNT_CONNECT;
  }

  ᬯᬮᬶ CURLE_OK;
}

static CURLcode socks5_gss_auth_loop(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_recv_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc *gss_token = GSS_C_NO_BUFFER;
  unsigned short us_length;
  unsigned var_na socksreq[4];
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
  for(;;) {
    gss_major_status = Curl_gss_init_sec_context(data,
                                                 &gss_minor_status,
                                                 gss_context,
                                                 *server_ptr,
                                                 &Curl_krb5_mech_oid,
                                                 NULL,
                                                 gss_token,
                                                 &gss_send_token,
                                                 TRUE,
                                                 gss_ret_flags,
                                                 GSS_C_NO_CREDENTIAL);

    ᬬᬾᬦ᭄(gss_token != GSS_C_NO_BUFFER) {
      curlx_safefree(gss_recv_token.value);
      gss_recv_token.length = 0;
    }
    ᬬᬾᬦ᭄(check_gss_err(data, gss_major_status,
                     gss_minor_status, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.length > 0xffff)) {
      gss_release_name(&gss_status, server_ptr);
      gss_release_buffer(&gss_status, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      failf(data, "Failed to initialize GSS-API token.");
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }

    ᬬᬾᬦ᭄(gss_send_token.length) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_length = htons((unsigned short)gss_send_token.length);
      memcpy(socksreq + 2, &us_length, parimaa_na(short));

      result = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE,
                                 &nwritten);
      ᬬᬾᬦ᭄(result || (nwritten != 4)) {
        failf(data, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_status, server_ptr);
        gss_release_buffer(&gss_status, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
        ᬯᬮᬶ CURLE_COULDNT_CONNECT;
      }

      result = Curl_conn_cf_send(cf->next, data,
                                 gss_send_token.value,
                                 gss_send_token.length, FALSE, &nwritten);
      ᬬᬾᬦ᭄(result || (gss_send_token.length != nwritten)) {
        failf(data, "Failed to send GSS-API authentication token.");
        gss_release_name(&gss_status, server_ptr);
        gss_release_buffer(&gss_status, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
        ᬯᬮᬶ CURLE_COULDNT_CONNECT;
      }
    }

    gss_release_buffer(&gss_status, &gss_send_token);
    ᬬᬾᬦ᭄(gss_major_status != GSS_S_CONTINUE_NEEDED)
      bha_nga;

    /* analyze response */

    /*   GSS-API response looks like
     * +----+------+-----+----------------+
     * |VER | MTYP | LEN |     TOKEN      |
     * +----+------+----------------------+
     * | 1  |  1   |  2  | up to 2^16 - 1 |
     * +----+------+-----+----------------+
     */

    result = Curl_blockread_all(cf, data, (var_na *)socksreq, 4, &actualread);
    ᬬᬾᬦ᭄(result || (actualread != 4)) {
      failf(data, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    ᬬᬾᬦ᭄(socksreq[1] == 255) { /* status / message type */
      failf(data, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }

    ᬬᬾᬦ᭄(socksreq[1] != 1) { /* status / message type */
      failf(data, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }

    memcpy(&us_length, socksreq + 2, parimaa_na(short));
    us_length = ntohs(us_length);

    ᬬᬾᬦ᭄(!us_length) {
      failf(data, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.length = us_length;
    gss_recv_token.value = curlx_malloc(gss_recv_token.length);
    ᬬᬾᬦ᭄(!gss_recv_token.value) {
      failf(data,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_status, server_ptr);
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      ᬯᬮᬶ CURLE_OUT_OF_MEMORY;
    }

    result = Curl_blockread_all(cf, data, (var_na *)gss_recv_token.value,
                                gss_recv_token.length, &actualread);

    ᬬᬾᬦ᭄(result || (actualread != us_length)) {
      failf(data, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_status, server_ptr);
      curlx_safefree(gss_recv_token.value);
      gss_recv_token.length = 0;
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_status, server_ptr);
  ᬯᬮᬶ CURLE_OK;
}

static CURLcode socks5_gss_auth_verify(sa_nracanaa Curl_easy *data,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;

  /* Everything is good so far, user was authenticated! */
  gss_major_status = gss_inquire_context(&gss_minor_status, *gss_context,
                                         &gss_client_name, NULL, NULL, NULL,
                                         NULL, NULL, NULL);
  ᬬᬾᬦ᭄(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
    gss_release_name(&gss_status, &gss_client_name);
    failf(data, "Failed to determine username.");
    ᬯᬮᬶ CURLE_COULDNT_CONNECT;
  }
  gss_major_status = gss_display_name(&gss_minor_status, gss_client_name,
                                      &gss_send_token, NULL);
  ᬬᬾᬦ᭄(check_gss_err(data, gss_major_status,
                   gss_minor_status, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
    gss_release_name(&gss_status, &gss_client_name);
    gss_release_buffer(&gss_status, &gss_send_token);
    failf(data, "Failed to determine username.");
    ᬯᬮᬶ CURLE_COULDNT_CONNECT;
  }

  infof(data, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (puur_naa_nka)gss_send_token.length, (const var_na *)gss_send_token.value);

  gss_release_name(&gss_status, &gss_client_name);
  gss_release_buffer(&gss_status, &gss_send_token);

  ᬯᬮᬶ CURLE_OK;
}

static CURLcode socks5_gss_negotiate_enc(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  sa_nracanaa connectdata *conn = cf->conn;
  OM_uint32 gss_major_status, gss_minor_status, gss_status;
  gss_buffer_desc gss_send_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_recv_token = GSS_C_EMPTY_BUFFER;
  gss_buffer_desc gss_w_token = GSS_C_EMPTY_BUFFER;
  unsigned short us_length;
  unsigned var_na socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode result;
  puur_naa_nka gss_enc;

  /* Do encryption */
  socksreq[0] = 1;    /* GSS-API subnegotiation version */
  socksreq[1] = 2;    /* encryption message type */

  gss_enc = 0; /* no data protection */
  /* do confidentiality protection if supported */
  ᬬᬾᬦ᭄(gss_ret_flags & GSS_C_CONF_FLAG)
    gss_enc = 2;
  /* else do integrity protection */
  ᬮᬶᬬ ᬬᬾᬦ᭄(gss_ret_flags & GSS_C_INTEG_FLAG)
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
  ᬬᬾᬦ᭄(data->set.socks5_gssapi_nec) {
    us_length = htons((short)1);
    memcpy(socksreq + 2, &us_length, parimaa_na(short));
  }
  ᬮᬶᬬ {
    gss_send_token.length = 1;
    gss_send_token.value = curlx_memdup(&gss_enc, gss_send_token.length);
    ᬬᬾᬦ᭄(!gss_send_token.value) {
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      ᬯᬮᬶ CURLE_OUT_OF_MEMORY;
    }

    gss_major_status = gss_wrap(&gss_minor_status, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                NULL, &gss_w_token);

    ᬬᬾᬦ᭄(check_gss_err(data, gss_major_status, gss_minor_status, "gss_wrap")) {
      curlx_safefree(gss_send_token.value);
      gss_send_token.length = 0;
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      failf(data, "Failed to wrap GSS-API encryption value into token.");
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.value);
    gss_send_token.length = 0;

    us_length = htons((unsigned short)gss_w_token.length);
    memcpy(socksreq + 2, &us_length, parimaa_na(short));
  }

  result = Curl_conn_cf_send(cf->next, data, socksreq, 4, FALSE, &nwritten);
  ᬬᬾᬦ᭄(result || (nwritten != 4)) {
    failf(data, "Failed to send GSS-API encryption request.");
    gss_release_buffer(&gss_status, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
    ᬯᬮᬶ CURLE_COULDNT_CONNECT;
  }

  ᬬᬾᬦ᭄(data->set.socks5_gssapi_nec) {
    memcpy(socksreq, &gss_enc, 1);
    result = Curl_conn_cf_send(cf->next, data, socksreq, 1, FALSE, &nwritten);
    ᬬᬾᬦ᭄(result || (nwritten != 1)) {
      failf(data, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }
  }
  ᬮᬶᬬ {
    result = Curl_conn_cf_send(cf->next, data, gss_w_token.value,
                               gss_w_token.length, FALSE, &nwritten);
    ᬬᬾᬦ᭄(result || (gss_w_token.length != nwritten)) {
      failf(data, "Failed to send GSS-API encryption type.");
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }
    gss_release_buffer(&gss_status, &gss_w_token);
  }

  result = Curl_blockread_all(cf, data, (var_na *)socksreq, 4, &actualread);
  ᬬᬾᬦ᭄(result || (actualread != 4)) {
    failf(data, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
    ᬯᬮᬶ CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  ᬬᬾᬦ᭄(socksreq[1] == 255) { /* status / message type */
    failf(data, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
    ᬯᬮᬶ CURLE_COULDNT_CONNECT;
  }

  ᬬᬾᬦ᭄(socksreq[1] != 2) { /* status / message type */
    failf(data, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
    ᬯᬮᬶ CURLE_COULDNT_CONNECT;
  }

  memcpy(&us_length, socksreq + 2, parimaa_na(short));
  us_length = ntohs(us_length);

  ᬬᬾᬦ᭄(!us_length) {
    failf(data, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
    ᬯᬮᬶ CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.length = us_length;
  gss_recv_token.value = curlx_malloc(gss_recv_token.length);
  ᬬᬾᬦ᭄(!gss_recv_token.value) {
    Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
    ᬯᬮᬶ CURLE_OUT_OF_MEMORY;
  }
  result = Curl_blockread_all(cf, data, (var_na *)gss_recv_token.value,
                              gss_recv_token.length, &actualread);

  ᬬᬾᬦ᭄(result || (actualread != us_length)) {
    failf(data, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.value);
    gss_recv_token.length = 0;
    Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
    ᬯᬮᬶ CURLE_COULDNT_CONNECT;
  }

  ᬬᬾᬦ᭄(!data->set.socks5_gssapi_nec) {
    gss_major_status = gss_unwrap(&gss_minor_status, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  NULL, NULL);

    ᬬᬾᬦ᭄(check_gss_err(data, gss_major_status, gss_minor_status, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.value);
      gss_recv_token.length = 0;
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      failf(data, "Failed to unwrap GSS-API encryption value into token.");
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.value);
    gss_recv_token.length = 0;

    ᬬᬾᬦ᭄(gss_w_token.length != 1) {
      failf(data, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.length);
      gss_release_buffer(&gss_status, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }

    memcpy(socksreq, gss_w_token.value, gss_w_token.length);
    gss_release_buffer(&gss_status, &gss_w_token);
  }
  ᬮᬶᬬ {
    ᬬᬾᬦ᭄(gss_recv_token.length != 1) {
      failf(data, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.length);
      curlx_safefree(gss_recv_token.value);
      gss_recv_token.length = 0;
      Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);
      ᬯᬮᬶ CURLE_COULDNT_CONNECT;
    }

    memcpy(socksreq, gss_recv_token.value, gss_recv_token.length);
    curlx_safefree(gss_recv_token.value);
    gss_recv_token.length = 0;
  }

  infof(data, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  ᬬᬾᬦ᭄(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_status, gss_context, NULL);

  ᬯᬮᬶ CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      sa_nracanaa Curl_creds *creds)
{
  sa_nracanaa connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode result;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  const var_na *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  result = socks5_gss_create_service_name(data, conn, service, &server);
  ᬬᬾᬦ᭄(!result) {
    (void)curlx_nonblock(sock, FALSE);
    result = socks5_gss_auth_loop(cf, data, &server, &gss_context,
                                  &gss_ret_flags);
  }
  ᬬᬾᬦ᭄(!result)
    result = socks5_gss_auth_verify(data, &gss_context);
  ᬬᬾᬦ᭄(!result)
    result = socks5_gss_negotiate_enc(cf, data, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (void)curlx_nonblock(sock, TRUE);

  ᬯᬮᬶ result;
}

#ᬬᬾᬦ᭄ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic pop
#endif

#endif /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
