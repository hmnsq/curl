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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(HAVE_GSSAPI) && !defined(CURL_DISABLE_PROXY)

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_gssapi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/nonblock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socks.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wdeprecated-declarations"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
ਸਥਿਰ ਪੂਰਨ check_gss_err(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         OM_uint32 major_ਸਥਿਤੀ,
                         OM_uint32 minor_ਸਥਿਤੀ,
                         ਅਚਲ ਅੱਖਰ *function)
{
  ਜੇ(GSS_ERROR(major_ਸਥਿਤੀ)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 ਸੁਨੇਹਾ_ctx = 0;
    gss_ਬਫਰ_desc ਸਥਿਤੀ_string = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
    ਬਣਤਰ dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    ਸੁਨੇਹਾ_ctx = 0;
    ਜਦੋਂ(!ਸੁਨੇਹਾ_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_ਸਥਿਤੀ(&min_stat, major_ਸਥਿਤੀ,
                                    GSS_C_GSS_ਕੋਡ,
                                    GSS_C_NULL_OID,
                                    &ਸੁਨੇਹਾ_ctx, &ਸਥਿਤੀ_string);
      ਜੇ(maj_stat == GSS_S_COMPLETE) {
        ਜੇ(curlx_dyn_addn(&dbuf, ਸਥਿਤੀ_string.ਮੁੱਲ, ਸਥਿਤੀ_string.ਲੰਮਾਈ))
          ਵਾਪਸ 1; /* error */
        gss_release_ਬਫਰ(&min_stat, &ਸਥਿਤੀ_string);
        ਰੋਕੋ;
      }
      gss_release_ਬਫਰ(&min_stat, &ਸਥਿਤੀ_string);
    }
    ਜੇ(curlx_dyn_addn(&dbuf, ".\n", 2))
      ਵਾਪਸ 1; /* error */
    ਸੁਨੇਹਾ_ctx = 0;
    ਜਦੋਂ(!ਸੁਨੇਹਾ_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_ਸਥਿਤੀ(&min_stat, minor_ਸਥਿਤੀ,
                                    GSS_C_MECH_ਕੋਡ,
                                    GSS_C_NULL_OID,
                                    &ਸੁਨੇਹਾ_ctx, &ਸਥਿਤੀ_string);
      ਜੇ(maj_stat == GSS_S_COMPLETE) {
        ਜੇ(curlx_dyn_addn(&dbuf, ਸਥਿਤੀ_string.ਮੁੱਲ, ਸਥਿਤੀ_string.ਲੰਮਾਈ))
          ਵਾਪਸ 1; /* error */
        gss_release_ਬਫਰ(&min_stat, &ਸਥਿਤੀ_string);
        ਰੋਕੋ;
      }
      gss_release_ਬਫਰ(&min_stat, &ਸਥਿਤੀ_string);
    }
    failf(ਅੰਕੜੇ, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    ਵਾਪਸ 1;
  }

  ਵਾਪਸ 0;
}

ਸਥਿਰ CURLcode socks5_gss_create_service_name(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                               ਬਣਤਰ connectdata *conn,
                                               ਅਚਲ ਅੱਖਰ *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_ਸਥਿਤੀ, gss_minor_ਸਥਿਤੀ, gss_ਸਥਿਤੀ;
  gss_ਬਫਰ_desc service = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;

  /* prepare service name */
  ਜੇ(strchr(serviceptr, '/')) {
    service.ਲੰਮਾਈ = ਲੜੀ_ਲੰਬਾਈ(serviceptr);
    service.ਮੁੱਲ = curlx_memdup(serviceptr, service.ਲੰਮਾਈ);
    ਜੇ(!service.ਮੁੱਲ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    gss_major_ਸਥਿਤੀ = gss_import_name(&gss_minor_ਸਥਿਤੀ, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  ਨਹੀਂਤਾਂ {
    service.ਮੁੱਲ = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    ਜੇ(!service.ਮੁੱਲ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    service.ਲੰਮਾਈ = ਲੜੀ_ਲੰਬਾਈ(service.ਮੁੱਲ);

    gss_major_ਸਥਿਤੀ = gss_import_name(&gss_minor_ਸਥਿਤੀ, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.ਮੁੱਲ);
  service.ਲੰਮਾਈ = 0;

  ਜੇ(check_gss_err(ਅੰਕੜੇ, gss_major_ਸਥਿਤੀ,
                   gss_minor_ਸਥਿਤੀ, "gss_import_name()")) {
    failf(ਅੰਕੜੇ, "Failed to create service name.");
    gss_release_name(&gss_ਸਥਿਤੀ, server);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode socks5_gss_auth_loop(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_ਸਥਿਤੀ, gss_minor_ਸਥਿਤੀ, gss_ਸਥਿਤੀ;
  gss_ਬਫਰ_desc gss_send_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
  gss_ਬਫਰ_desc gss_recv_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
  gss_ਬਫਰ_desc *gss_token = GSS_C_NO_ਬਫਰ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ us_ਲੰਮਾਈ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;

  /*   GSS-API request looks like
   * +----+------+-----+----------------+
   * |VER | MTYP | LEN |     TOKEN      |
   * +----+------+----------------------+
   * | 1  |  1   |  2  | up to 2^16 - 1 |
   * +----+------+-----+----------------+
   */

  /* As long as we need to keep sending some context info, and there is no
   * errors, keep sending it... */
  ਲਈ(;;) {
    gss_major_ਸਥਿਤੀ = Curl_gss_init_sec_context(ਅੰਕੜੇ,
                                                 &gss_minor_ਸਥਿਤੀ,
                                                 gss_context,
                                                 *server_ptr,
                                                 &Curl_krb5_mech_oid,
                                                 ਨਿਰਾ,
                                                 gss_token,
                                                 &gss_send_token,
                                                 TRUE,
                                                 gss_ret_flags,
                                                 GSS_C_NO_CREDENTIAL);

    ਜੇ(gss_token != GSS_C_NO_ਬਫਰ) {
      curlx_safefree(gss_recv_token.ਮੁੱਲ);
      gss_recv_token.ਲੰਮਾਈ = 0;
    }
    ਜੇ(check_gss_err(ਅੰਕੜੇ, gss_major_ਸਥਿਤੀ,
                     gss_minor_ਸਥਿਤੀ, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.ਲੰਮਾਈ > 0xffff)) {
      gss_release_name(&gss_ਸਥਿਤੀ, server_ptr);
      gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      failf(ਅੰਕੜੇ, "Failed to initialize GSS-API token.");
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    ਜੇ(gss_send_token.ਲੰਮਾਈ) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_ਲੰਮਾਈ = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)gss_send_token.ਲੰਮਾਈ);
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq + 2, &us_ਲੰਮਾਈ, ਆਕਾਰ(ਛੋਟਾ));

      ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, socksreq, 4, FALSE,
                                 &nwritten);
      ਜੇ(ਨਤੀਜਾ || (nwritten != 4)) {
        failf(ਅੰਕੜੇ, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_ਸਥਿਤੀ, server_ptr);
        gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
        ਵਾਪਸ CURLE_COULDNT_CONNECT;
      }

      ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ,
                                 gss_send_token.ਮੁੱਲ,
                                 gss_send_token.ਲੰਮਾਈ, FALSE, &nwritten);
      ਜੇ(ਨਤੀਜਾ || (gss_send_token.ਲੰਮਾਈ != nwritten)) {
        failf(ਅੰਕੜੇ, "Failed to send GSS-API authentication token.");
        gss_release_name(&gss_ਸਥਿਤੀ, server_ptr);
        gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
        ਵਾਪਸ CURLE_COULDNT_CONNECT;
      }
    }

    gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_send_token);
    ਜੇ(gss_major_ਸਥਿਤੀ != GSS_S_CONTINUE_NEEDED)
      ਰੋਕੋ;

    /* analyze response */

    /*   GSS-API response looks like
     * +----+------+-----+----------------+
     * |VER | MTYP | LEN |     TOKEN      |
     * +----+------+----------------------+
     * | 1  |  1   |  2  | up to 2^16 - 1 |
     * +----+------+-----+----------------+
     */

    ਨਤੀਜਾ = Curl_blockread_all(cf, ਅੰਕੜੇ, (ਅੱਖਰ *)socksreq, 4, &actualread);
    ਜੇ(ਨਤੀਜਾ || (actualread != 4)) {
      failf(ਅੰਕੜੇ, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_ਸਥਿਤੀ, server_ptr);
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    ਜੇ(socksreq[1] == 255) { /* status / message type */
      failf(ਅੰਕੜੇ, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_ਸਥਿਤੀ, server_ptr);
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    ਜੇ(socksreq[1] != 1) { /* status / message type */
      failf(ਅੰਕੜੇ, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_ਸਥਿਤੀ, server_ptr);
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&us_ਲੰਮਾਈ, socksreq + 2, ਆਕਾਰ(ਛੋਟਾ));
    us_ਲੰਮਾਈ = ntohs(us_ਲੰਮਾਈ);

    ਜੇ(!us_ਲੰਮਾਈ) {
      failf(ਅੰਕੜੇ, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_ਸਥਿਤੀ, server_ptr);
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.ਲੰਮਾਈ = us_ਲੰਮਾਈ;
    gss_recv_token.ਮੁੱਲ = curlx_malloc(gss_recv_token.ਲੰਮਾਈ);
    ਜੇ(!gss_recv_token.ਮੁੱਲ) {
      failf(ਅੰਕੜੇ,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_ਸਥਿਤੀ, server_ptr);
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    ਨਤੀਜਾ = Curl_blockread_all(cf, ਅੰਕੜੇ, (ਅੱਖਰ *)gss_recv_token.ਮੁੱਲ,
                                gss_recv_token.ਲੰਮਾਈ, &actualread);

    ਜੇ(ਨਤੀਜਾ || (actualread != us_ਲੰਮਾਈ)) {
      failf(ਅੰਕੜੇ, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_ਸਥਿਤੀ, server_ptr);
      curlx_safefree(gss_recv_token.ਮੁੱਲ);
      gss_recv_token.ਲੰਮਾਈ = 0;
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_ਸਥਿਤੀ, server_ptr);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode socks5_gss_auth_verify(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_ਸਥਿਤੀ, gss_minor_ਸਥਿਤੀ, gss_ਸਥਿਤੀ;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_ਬਫਰ_desc gss_send_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;

  /* Everything is good so far, user was authenticated! */
  gss_major_ਸਥਿਤੀ = gss_inquire_context(&gss_minor_ਸਥਿਤੀ, *gss_context,
                                         &gss_client_name, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
                                         ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
  ਜੇ(check_gss_err(ਅੰਕੜੇ, gss_major_ਸਥਿਤੀ,
                   gss_minor_ਸਥਿਤੀ, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
    gss_release_name(&gss_ਸਥਿਤੀ, &gss_client_name);
    failf(ਅੰਕੜੇ, "Failed to determine username.");
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }
  gss_major_ਸਥਿਤੀ = gss_display_name(&gss_minor_ਸਥਿਤੀ, gss_client_name,
                                      &gss_send_token, ਨਿਰਾ);
  ਜੇ(check_gss_err(ਅੰਕੜੇ, gss_major_ਸਥਿਤੀ,
                   gss_minor_ਸਥਿਤੀ, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
    gss_release_name(&gss_ਸਥਿਤੀ, &gss_client_name);
    gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_send_token);
    failf(ਅੰਕੜੇ, "Failed to determine username.");
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  infof(ਅੰਕੜੇ, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (ਪੂਰਨ)gss_send_token.ਲੰਮਾਈ, (ਅਚਲ ਅੱਖਰ *)gss_send_token.ਮੁੱਲ);

  gss_release_name(&gss_ਸਥਿਤੀ, &gss_client_name);
  gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_send_token);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode socks5_gss_negotiate_enc(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  ਬਣਤਰ connectdata *conn = cf->conn;
  OM_uint32 gss_major_ਸਥਿਤੀ, gss_minor_ਸਥਿਤੀ, gss_ਸਥਿਤੀ;
  gss_ਬਫਰ_desc gss_send_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
  gss_ਬਫਰ_desc gss_recv_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
  gss_ਬਫਰ_desc gss_w_token = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ us_ਲੰਮਾਈ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ gss_enc;

  /* Do encryption */
  socksreq[0] = 1;    /* GSS-API subnegotiation version */
  socksreq[1] = 2;    /* encryption message type */

  gss_enc = 0; /* no data protection */
  /* do confidentiality protection if supported */
  ਜੇ(gss_ret_flags & GSS_C_CONF_FLAG)
    gss_enc = 2;
  /* else do integrity protection */
  ਨਹੀਂਤਾਂ ਜੇ(gss_ret_flags & GSS_C_INTEG_FLAG)
    gss_enc = 1;

  infof(ਅੰਕੜੇ, "SOCKS5 server supports GSS-API %s data protection.",
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
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.socks5_gssapi_nec) {
    us_ਲੰਮਾਈ = htons((ਛੋਟਾ)1);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq + 2, &us_ਲੰਮਾਈ, ਆਕਾਰ(ਛੋਟਾ));
  }
  ਨਹੀਂਤਾਂ {
    gss_send_token.ਲੰਮਾਈ = 1;
    gss_send_token.ਮੁੱਲ = curlx_memdup(&gss_enc, gss_send_token.ਲੰਮਾਈ);
    ਜੇ(!gss_send_token.ਮੁੱਲ) {
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    gss_major_ਸਥਿਤੀ = gss_wrap(&gss_minor_ਸਥਿਤੀ, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                ਨਿਰਾ, &gss_w_token);

    ਜੇ(check_gss_err(ਅੰਕੜੇ, gss_major_ਸਥਿਤੀ, gss_minor_ਸਥਿਤੀ, "gss_wrap")) {
      curlx_safefree(gss_send_token.ਮੁੱਲ);
      gss_send_token.ਲੰਮਾਈ = 0;
      gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      failf(ਅੰਕੜੇ, "Failed to wrap GSS-API encryption value into token.");
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.ਮੁੱਲ);
    gss_send_token.ਲੰਮਾਈ = 0;

    us_ਲੰਮਾਈ = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)gss_w_token.ਲੰਮਾਈ);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq + 2, &us_ਲੰਮਾਈ, ਆਕਾਰ(ਛੋਟਾ));
  }

  ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, socksreq, 4, FALSE, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten != 4)) {
    failf(ਅੰਕੜੇ, "Failed to send GSS-API encryption request.");
    gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.socks5_gssapi_nec) {
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq, &gss_enc, 1);
    ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, socksreq, 1, FALSE, &nwritten);
    ਜੇ(ਨਤੀਜਾ || (nwritten != 1)) {
      failf(ਅੰਕੜੇ, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, gss_w_token.ਮੁੱਲ,
                               gss_w_token.ਲੰਮਾਈ, FALSE, &nwritten);
    ਜੇ(ਨਤੀਜਾ || (gss_w_token.ਲੰਮਾਈ != nwritten)) {
      failf(ਅੰਕੜੇ, "Failed to send GSS-API encryption type.");
      gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }
    gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_w_token);
  }

  ਨਤੀਜਾ = Curl_blockread_all(cf, ਅੰਕੜੇ, (ਅੱਖਰ *)socksreq, 4, &actualread);
  ਜੇ(ਨਤੀਜਾ || (actualread != 4)) {
    failf(ਅੰਕੜੇ, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  ਜੇ(socksreq[1] == 255) { /* status / message type */
    failf(ਅੰਕੜੇ, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  ਜੇ(socksreq[1] != 2) { /* status / message type */
    failf(ਅੰਕੜੇ, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&us_ਲੰਮਾਈ, socksreq + 2, ਆਕਾਰ(ਛੋਟਾ));
  us_ਲੰਮਾਈ = ntohs(us_ਲੰਮਾਈ);

  ਜੇ(!us_ਲੰਮਾਈ) {
    failf(ਅੰਕੜੇ, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.ਲੰਮਾਈ = us_ਲੰਮਾਈ;
  gss_recv_token.ਮੁੱਲ = curlx_malloc(gss_recv_token.ਲੰਮਾਈ);
  ਜੇ(!gss_recv_token.ਮੁੱਲ) {
    Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਨਤੀਜਾ = Curl_blockread_all(cf, ਅੰਕੜੇ, (ਅੱਖਰ *)gss_recv_token.ਮੁੱਲ,
                              gss_recv_token.ਲੰਮਾਈ, &actualread);

  ਜੇ(ਨਤੀਜਾ || (actualread != us_ਲੰਮਾਈ)) {
    failf(ਅੰਕੜੇ, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.ਮੁੱਲ);
    gss_recv_token.ਲੰਮਾਈ = 0;
    Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.socks5_gssapi_nec) {
    gss_major_ਸਥਿਤੀ = gss_unwrap(&gss_minor_ਸਥਿਤੀ, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  ਨਿਰਾ, ਨਿਰਾ);

    ਜੇ(check_gss_err(ਅੰਕੜੇ, gss_major_ਸਥਿਤੀ, gss_minor_ਸਥਿਤੀ, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.ਮੁੱਲ);
      gss_recv_token.ਲੰਮਾਈ = 0;
      gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      failf(ਅੰਕੜੇ, "Failed to unwrap GSS-API encryption value into token.");
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.ਮੁੱਲ);
    gss_recv_token.ਲੰਮਾਈ = 0;

    ਜੇ(gss_w_token.ਲੰਮਾਈ != 1) {
      failf(ਅੰਕੜੇ, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.ਲੰਮਾਈ);
      gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq, gss_w_token.ਮੁੱਲ, gss_w_token.ਲੰਮਾਈ);
    gss_release_ਬਫਰ(&gss_ਸਥਿਤੀ, &gss_w_token);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(gss_recv_token.ਲੰਮਾਈ != 1) {
      failf(ਅੰਕੜੇ, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.ਲੰਮਾਈ);
      curlx_safefree(gss_recv_token.ਮੁੱਲ);
      gss_recv_token.ਲੰਮਾਈ = 0;
      Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(socksreq, gss_recv_token.ਮੁੱਲ, gss_recv_token.ਲੰਮਾਈ);
    curlx_safefree(gss_recv_token.ਮੁੱਲ);
    gss_recv_token.ਲੰਮਾਈ = 0;
  }

  infof(ਅੰਕੜੇ, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  ਜੇ(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_ਸਥਿਤੀ, gss_context, ਨਿਰਾ);

  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ Curl_creds *creds)
{
  ਬਣਤਰ connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode ਨਤੀਜਾ;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  ਨਤੀਜਾ = socks5_gss_create_service_name(ਅੰਕੜੇ, conn, service, &server);
  ਜੇ(!ਨਤੀਜਾ) {
    (ਖਾਲੀ)curlx_nonblock(sock, FALSE);
    ਨਤੀਜਾ = socks5_gss_auth_loop(cf, ਅੰਕੜੇ, &server, &gss_context,
                                  &gss_ret_flags);
  }
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = socks5_gss_auth_verify(ਅੰਕੜੇ, &gss_context);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = socks5_gss_negotiate_enc(cf, ਅੰਕੜੇ, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (ਖਾਲੀ)curlx_nonblock(sock, TRUE);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
