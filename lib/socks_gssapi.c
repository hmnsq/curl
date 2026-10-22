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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ defined(HAVE_GSSAPI) && !defined(CURL_DISABLE_PROXY)

#ಅನ್ತರ್ಭಾವ "curl_gssapi.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curlx/nonblock.h"
#ಅನ್ತರ್ಭಾವ "socks.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wdeprecated-declarations"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_GSS_LEN 1024

/*
 * Helper GSS-API error functions.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_gss_err(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         OM_uint32 major_ಸ್ಥಿತಿ,
                         OM_uint32 minor_ಸ್ಥಿತಿ,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *function)
{
  ಆದರೆ(GSS_ERROR(major_ಸ್ಥಿತಿ)) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 ಸಂದೇಶ_ctx = 0;
    gss_ಬಫರ್_desc ಸ್ಥಿತಿ_string = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
    ರಚನೆ dynbuf dbuf;

    curlx_dyn_init(&dbuf, MAX_GSS_LEN);
    ಸಂದೇಶ_ctx = 0;
    ಆಗ(!ಸಂದೇಶ_ctx) {
      /* convert major status code (GSS-API error) to text */
      maj_stat = gss_display_ಸ್ಥಿತಿ(&min_stat, major_ಸ್ಥಿತಿ,
                                    GSS_C_GSS_ಸಂಕೇತ,
                                    GSS_C_NULL_OID,
                                    &ಸಂದೇಶ_ctx, &ಸ್ಥಿತಿ_string);
      ಆದರೆ(maj_stat == GSS_S_COMPLETE) {
        ಆದರೆ(curlx_dyn_addn(&dbuf, ಸ್ಥಿತಿ_string.ಮೌಲ್ಯ, ಸ್ಥಿತಿ_string.ಉದ್ದಳತೆ))
          ಹಿಂತಿರುಗಿ 1; /* error */
        gss_release_ಬಫರ್(&min_stat, &ಸ್ಥಿತಿ_string);
        ನಿಲ್ಲಿಸು;
      }
      gss_release_ಬಫರ್(&min_stat, &ಸ್ಥಿತಿ_string);
    }
    ಆದರೆ(curlx_dyn_addn(&dbuf, ".\n", 2))
      ಹಿಂತಿರುಗಿ 1; /* error */
    ಸಂದೇಶ_ctx = 0;
    ಆಗ(!ಸಂದೇಶ_ctx) {
      /* convert minor status code (underlying routine error) to text */
      maj_stat = gss_display_ಸ್ಥಿತಿ(&min_stat, minor_ಸ್ಥಿತಿ,
                                    GSS_C_MECH_ಸಂಕೇತ,
                                    GSS_C_NULL_OID,
                                    &ಸಂದೇಶ_ctx, &ಸ್ಥಿತಿ_string);
      ಆದರೆ(maj_stat == GSS_S_COMPLETE) {
        ಆದರೆ(curlx_dyn_addn(&dbuf, ಸ್ಥಿತಿ_string.ಮೌಲ್ಯ, ಸ್ಥಿತಿ_string.ಉದ್ದಳತೆ))
          ಹಿಂತಿರುಗಿ 1; /* error */
        gss_release_ಬಫರ್(&min_stat, &ಸ್ಥಿತಿ_string);
        ನಿಲ್ಲಿಸು;
      }
      gss_release_ಬಫರ್(&min_stat, &ಸ್ಥಿತಿ_string);
    }
    failf(ದತ್ತಾಂಶ, "GSS-API error: %s failed: %s", function,
          curlx_dyn_ptr(&dbuf));
    curlx_dyn_free(&dbuf);
    ಹಿಂತಿರುಗಿ 1;
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ CURLcode socks5_gss_create_service_name(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                               ರಚನೆ connectdata *conn,
                                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *serviceptr,
                                               gss_name_t *server)
{
  OM_uint32 gss_major_ಸ್ಥಿತಿ, gss_minor_ಸ್ಥಿತಿ, gss_ಸ್ಥಿತಿ;
  gss_ಬಫರ್_desc service = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;

  /* prepare service name */
  ಆದರೆ(strchr(serviceptr, '/')) {
    service.ಉದ್ದಳತೆ = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(serviceptr);
    service.ಮೌಲ್ಯ = curlx_memdup(serviceptr, service.ಉದ್ದಳತೆ);
    ಆದರೆ(!service.ಮೌಲ್ಯ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    gss_major_ಸ್ಥಿತಿ = gss_import_name(&gss_minor_ಸ್ಥಿತಿ, &service,
                                       (gss_OID)GSS_C_NULL_OID, server);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    service.ಮೌಲ್ಯ = curl_maprintf("%s@%s", serviceptr,
                                  conn->socks_proxy.peer->hostname);
    ಆದರೆ(!service.ಮೌಲ್ಯ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    service.ಉದ್ದಳತೆ = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(service.ಮೌಲ್ಯ);

    gss_major_ಸ್ಥಿತಿ = gss_import_name(&gss_minor_ಸ್ಥಿತಿ, &service,
                                       GSS_C_NT_HOSTBASED_SERVICE, server);
  }

  curlx_safefree(service.ಮೌಲ್ಯ);
  service.ಉದ್ದಳತೆ = 0;

  ಆದರೆ(check_gss_err(ದತ್ತಾಂಶ, gss_major_ಸ್ಥಿತಿ,
                   gss_minor_ಸ್ಥಿತಿ, "gss_import_name()")) {
    failf(ದತ್ತಾಂಶ, "Failed to create service name.");
    gss_release_name(&gss_ಸ್ಥಿತಿ, server);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode socks5_gss_auth_loop(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     gss_name_t *server_ptr,
                                     gss_ctx_id_t *gss_context,
                                     OM_uint32 *gss_ret_flags)
{
  OM_uint32 gss_major_ಸ್ಥಿತಿ, gss_minor_ಸ್ಥಿತಿ, gss_ಸ್ಥಿತಿ;
  gss_ಬಫರ್_desc gss_send_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
  gss_ಬಫರ್_desc gss_recv_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
  gss_ಬಫರ್_desc *gss_token = GSS_C_NO_ಬಫರ್;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ us_ಉದ್ದಳತೆ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  /*   GSS-API request looks like
   * +----+------+-----+----------------+
   * |VER | MTYP | LEN |     TOKEN      |
   * +----+------+----------------------+
   * | 1  |  1   |  2  | up to 2^16 - 1 |
   * +----+------+-----+----------------+
   */

  /* As long as we need to keep sending some context info, and there is no
   * errors, keep sending it... */
  ಗಾಗಿ(;;) {
    gss_major_ಸ್ಥಿತಿ = Curl_gss_init_sec_context(ದತ್ತಾಂಶ,
                                                 &gss_minor_ಸ್ಥಿತಿ,
                                                 gss_context,
                                                 *server_ptr,
                                                 &Curl_krb5_mech_oid,
                                                 ಖಾಲಿ,
                                                 gss_token,
                                                 &gss_send_token,
                                                 TRUE,
                                                 gss_ret_flags,
                                                 GSS_C_NO_CREDENTIAL);

    ಆದರೆ(gss_token != GSS_C_NO_ಬಫರ್) {
      curlx_safefree(gss_recv_token.ಮೌಲ್ಯ);
      gss_recv_token.ಉದ್ದಳತೆ = 0;
    }
    ಆದರೆ(check_gss_err(ದತ್ತಾಂಶ, gss_major_ಸ್ಥಿತಿ,
                     gss_minor_ಸ್ಥಿತಿ, "gss_init_sec_context") ||
       /* the size needs to fit in a 16-bit field */
       (gss_send_token.ಉದ್ದಳತೆ > 0xffff)) {
      gss_release_name(&gss_ಸ್ಥಿತಿ, server_ptr);
      gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_send_token);
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      failf(ದತ್ತಾಂಶ, "Failed to initialize GSS-API token.");
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    ಆದರೆ(gss_send_token.ಉದ್ದಳತೆ) {
      socksreq[0] = 1;    /* GSS-API subnegotiation version */
      socksreq[1] = 1;    /* authentication message type */
      us_ಉದ್ದಳತೆ = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)gss_send_token.ಉದ್ದಳತೆ);
      ಸ್ಮರಣೆ_ನಕಲು(socksreq + 2, &us_ಉದ್ದಳತೆ, ಗಾತ್ರ(ಗಿಡ್ಡ));

      ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, socksreq, 4, FALSE,
                                 &nwritten);
      ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 4)) {
        failf(ದತ್ತಾಂಶ, "Failed to send GSS-API authentication request.");
        gss_release_name(&gss_ಸ್ಥಿತಿ, server_ptr);
        gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
        ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
      }

      ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ,
                                 gss_send_token.ಮೌಲ್ಯ,
                                 gss_send_token.ಉದ್ದಳತೆ, FALSE, &nwritten);
      ಆದರೆ(ಫಲಿತಾಂಶ || (gss_send_token.ಉದ್ದಳತೆ != nwritten)) {
        failf(ದತ್ತಾಂಶ, "Failed to send GSS-API authentication token.");
        gss_release_name(&gss_ಸ್ಥಿತಿ, server_ptr);
        gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_send_token);
        Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
        ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
      }
    }

    gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_send_token);
    ಆದರೆ(gss_major_ಸ್ಥಿತಿ != GSS_S_CONTINUE_NEEDED)
      ನಿಲ್ಲಿಸು;

    /* analyze response */

    /*   GSS-API response looks like
     * +----+------+-----+----------------+
     * |VER | MTYP | LEN |     TOKEN      |
     * +----+------+----------------------+
     * | 1  |  1   |  2  | up to 2^16 - 1 |
     * +----+------+-----+----------------+
     */

    ಫಲಿತಾಂಶ = Curl_blockread_all(cf, ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)socksreq, 4, &actualread);
    ಆದರೆ(ಫಲಿತಾಂಶ || (actualread != 4)) {
      failf(ದತ್ತಾಂಶ, "Failed to receive GSS-API authentication response.");
      gss_release_name(&gss_ಸ್ಥಿತಿ, server_ptr);
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    /* ignore the first (VER) byte */
    ಆದರೆ(socksreq[1] == 255) { /* status / message type */
      failf(ದತ್ತಾಂಶ, "User was rejected by the SOCKS5 server (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_ಸ್ಥಿತಿ, server_ptr);
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    ಆದರೆ(socksreq[1] != 1) { /* status / message type */
      failf(ದತ್ತಾಂಶ, "Invalid GSS-API authentication response type (%d %d).",
            socksreq[0], socksreq[1]);
      gss_release_name(&gss_ಸ್ಥಿತಿ, server_ptr);
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    ಸ್ಮರಣೆ_ನಕಲು(&us_ಉದ್ದಳತೆ, socksreq + 2, ಗಾತ್ರ(ಗಿಡ್ಡ));
    us_ಉದ್ದಳತೆ = ntohs(us_ಉದ್ದಳತೆ);

    ಆದರೆ(!us_ಉದ್ದಳತೆ) {
      failf(ದತ್ತಾಂಶ, "Invalid zero-length GSS-API authentication token.");
      gss_release_name(&gss_ಸ್ಥಿತಿ, server_ptr);
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    gss_recv_token.ಉದ್ದಳತೆ = us_ಉದ್ದಳತೆ;
    gss_recv_token.ಮೌಲ್ಯ = curlx_malloc(gss_recv_token.ಉದ್ದಳತೆ);
    ಆದರೆ(!gss_recv_token.ಮೌಲ್ಯ) {
      failf(ದತ್ತಾಂಶ,
            "Could not allocate memory for GSS-API authentication "
            "response token.");
      gss_release_name(&gss_ಸ್ಥಿತಿ, server_ptr);
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    ಫಲಿತಾಂಶ = Curl_blockread_all(cf, ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)gss_recv_token.ಮೌಲ್ಯ,
                                gss_recv_token.ಉದ್ದಳತೆ, &actualread);

    ಆದರೆ(ಫಲಿತಾಂಶ || (actualread != us_ಉದ್ದಳತೆ)) {
      failf(ದತ್ತಾಂಶ, "Failed to receive GSS-API authentication token.");
      gss_release_name(&gss_ಸ್ಥಿತಿ, server_ptr);
      curlx_safefree(gss_recv_token.ಮೌಲ್ಯ);
      gss_recv_token.ಉದ್ದಳತೆ = 0;
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    gss_token = &gss_recv_token;
  }

  gss_release_name(&gss_ಸ್ಥಿತಿ, server_ptr);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode socks5_gss_auth_verify(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       gss_ctx_id_t *gss_context)
{
  OM_uint32 gss_major_ಸ್ಥಿತಿ, gss_minor_ಸ್ಥಿತಿ, gss_ಸ್ಥಿತಿ;
  gss_name_t gss_client_name = GSS_C_NO_NAME;
  gss_ಬಫರ್_desc gss_send_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;

  /* Everything is good so far, user was authenticated! */
  gss_major_ಸ್ಥಿತಿ = gss_inquire_context(&gss_minor_ಸ್ಥಿತಿ, *gss_context,
                                         &gss_client_name, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
                                         ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
  ಆದರೆ(check_gss_err(ದತ್ತಾಂಶ, gss_major_ಸ್ಥಿತಿ,
                   gss_minor_ಸ್ಥಿತಿ, "gss_inquire_context")) {
    Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
    gss_release_name(&gss_ಸ್ಥಿತಿ, &gss_client_name);
    failf(ದತ್ತಾಂಶ, "Failed to determine username.");
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }
  gss_major_ಸ್ಥಿತಿ = gss_display_name(&gss_minor_ಸ್ಥಿತಿ, gss_client_name,
                                      &gss_send_token, ಖಾಲಿ);
  ಆದರೆ(check_gss_err(ದತ್ತಾಂಶ, gss_major_ಸ್ಥಿತಿ,
                   gss_minor_ಸ್ಥಿತಿ, "gss_display_name")) {
    Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
    gss_release_name(&gss_ಸ್ಥಿತಿ, &gss_client_name);
    gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_send_token);
    failf(ದತ್ತಾಂಶ, "Failed to determine username.");
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  infof(ದತ್ತಾಂಶ, "SOCKS5 server authenticated user %.*s with GSS-API.",
        (ಪೂರ್ಣಾಂಕ)gss_send_token.ಉದ್ದಳತೆ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)gss_send_token.ಮೌಲ್ಯ);

  gss_release_name(&gss_ಸ್ಥಿತಿ, &gss_client_name);
  gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_send_token);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode socks5_gss_negotiate_enc(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         gss_ctx_id_t *gss_context,
                                         OM_uint32 gss_ret_flags)
{
  ರಚನೆ connectdata *conn = cf->conn;
  OM_uint32 gss_major_ಸ್ಥಿತಿ, gss_minor_ಸ್ಥಿತಿ, gss_ಸ್ಥಿತಿ;
  gss_ಬಫರ್_desc gss_send_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
  gss_ಬಫರ್_desc gss_recv_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
  gss_ಬಫರ್_desc gss_w_token = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ us_ಉದ್ದಳತೆ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ socksreq[4];
  size_t actualread;
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ gss_enc;

  /* Do encryption */
  socksreq[0] = 1;    /* GSS-API subnegotiation version */
  socksreq[1] = 2;    /* encryption message type */

  gss_enc = 0; /* no data protection */
  /* do confidentiality protection if supported */
  ಆದರೆ(gss_ret_flags & GSS_C_CONF_FLAG)
    gss_enc = 2;
  /* else do integrity protection */
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(gss_ret_flags & GSS_C_INTEG_FLAG)
    gss_enc = 1;

  infof(ದತ್ತಾಂಶ, "SOCKS5 server supports GSS-API %s data protection.",
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
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.socks5_gssapi_nec) {
    us_ಉದ್ದಳತೆ = htons((ಗಿಡ್ಡ)1);
    ಸ್ಮರಣೆ_ನಕಲು(socksreq + 2, &us_ಉದ್ದಳತೆ, ಗಾತ್ರ(ಗಿಡ್ಡ));
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    gss_send_token.ಉದ್ದಳತೆ = 1;
    gss_send_token.ಮೌಲ್ಯ = curlx_memdup(&gss_enc, gss_send_token.ಉದ್ದಳತೆ);
    ಆದರೆ(!gss_send_token.ಮೌಲ್ಯ) {
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    gss_major_ಸ್ಥಿತಿ = gss_wrap(&gss_minor_ಸ್ಥಿತಿ, *gss_context, 0,
                                GSS_C_QOP_DEFAULT, &gss_send_token,
                                ಖಾಲಿ, &gss_w_token);

    ಆದರೆ(check_gss_err(ದತ್ತಾಂಶ, gss_major_ಸ್ಥಿತಿ, gss_minor_ಸ್ಥಿತಿ, "gss_wrap")) {
      curlx_safefree(gss_send_token.ಮೌಲ್ಯ);
      gss_send_token.ಉದ್ದಳತೆ = 0;
      gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      failf(ದತ್ತಾಂಶ, "Failed to wrap GSS-API encryption value into token.");
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_send_token.ಮೌಲ್ಯ);
    gss_send_token.ಉದ್ದಳತೆ = 0;

    us_ಉದ್ದಳತೆ = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)gss_w_token.ಉದ್ದಳತೆ);
    ಸ್ಮರಣೆ_ನಕಲು(socksreq + 2, &us_ಉದ್ದಳತೆ, ಗಾತ್ರ(ಗಿಡ್ಡ));
  }

  ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, socksreq, 4, FALSE, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 4)) {
    failf(ದತ್ತಾಂಶ, "Failed to send GSS-API encryption request.");
    gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_w_token);
    Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.socks5_gssapi_nec) {
    ಸ್ಮರಣೆ_ನಕಲು(socksreq, &gss_enc, 1);
    ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, socksreq, 1, FALSE, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten != 1)) {
      failf(ದತ್ತಾಂಶ, "Failed to send GSS-API encryption type.");
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, gss_w_token.ಮೌಲ್ಯ,
                               gss_w_token.ಉದ್ದಳತೆ, FALSE, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ || (gss_w_token.ಉದ್ದಳತೆ != nwritten)) {
      failf(ದತ್ತಾಂಶ, "Failed to send GSS-API encryption type.");
      gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }
    gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_w_token);
  }

  ಫಲಿತಾಂಶ = Curl_blockread_all(cf, ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)socksreq, 4, &actualread);
  ಆದರೆ(ಫಲಿತಾಂಶ || (actualread != 4)) {
    failf(ದತ್ತಾಂಶ, "Failed to receive GSS-API encryption response.");
    Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  /* ignore the first (VER) byte */
  ಆದರೆ(socksreq[1] == 255) { /* status / message type */
    failf(ದತ್ತಾಂಶ, "User was rejected by the SOCKS5 server (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  ಆದರೆ(socksreq[1] != 2) { /* status / message type */
    failf(ದತ್ತಾಂಶ, "Invalid GSS-API encryption response type (%d %d).",
          socksreq[0], socksreq[1]);
    Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  ಸ್ಮರಣೆ_ನಕಲು(&us_ಉದ್ದಳತೆ, socksreq + 2, ಗಾತ್ರ(ಗಿಡ್ಡ));
  us_ಉದ್ದಳತೆ = ntohs(us_ಉದ್ದಳತೆ);

  ಆದರೆ(!us_ಉದ್ದಳತೆ) {
    failf(ದತ್ತಾಂಶ, "Invalid zero-length GSS-API encryption token.");
    Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  gss_recv_token.ಉದ್ದಳತೆ = us_ಉದ್ದಳತೆ;
  gss_recv_token.ಮೌಲ್ಯ = curlx_malloc(gss_recv_token.ಉದ್ದಳತೆ);
  ಆದರೆ(!gss_recv_token.ಮೌಲ್ಯ) {
    Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಫಲಿತಾಂಶ = Curl_blockread_all(cf, ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)gss_recv_token.ಮೌಲ್ಯ,
                              gss_recv_token.ಉದ್ದಳತೆ, &actualread);

  ಆದರೆ(ಫಲಿತಾಂಶ || (actualread != us_ಉದ್ದಳತೆ)) {
    failf(ದತ್ತಾಂಶ, "Failed to receive GSS-API encryption type.");
    curlx_safefree(gss_recv_token.ಮೌಲ್ಯ);
    gss_recv_token.ಉದ್ದಳತೆ = 0;
    Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.socks5_gssapi_nec) {
    gss_major_ಸ್ಥಿತಿ = gss_unwrap(&gss_minor_ಸ್ಥಿತಿ, *gss_context,
                                  &gss_recv_token, &gss_w_token,
                                  ಖಾಲಿ, ಖಾಲಿ);

    ಆದರೆ(check_gss_err(ದತ್ತಾಂಶ, gss_major_ಸ್ಥಿತಿ, gss_minor_ಸ್ಥಿತಿ, "gss_unwrap")) {
      curlx_safefree(gss_recv_token.ಮೌಲ್ಯ);
      gss_recv_token.ಉದ್ದಳತೆ = 0;
      gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      failf(ದತ್ತಾಂಶ, "Failed to unwrap GSS-API encryption value into token.");
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }
    curlx_safefree(gss_recv_token.ಮೌಲ್ಯ);
    gss_recv_token.ಉದ್ದಳತೆ = 0;

    ಆದರೆ(gss_w_token.ಉದ್ದಳತೆ != 1) {
      failf(ದತ್ತಾಂಶ, "Invalid GSS-API encryption response length (%zu).",
            gss_w_token.ಉದ್ದಳತೆ);
      gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_w_token);
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    ಸ್ಮರಣೆ_ನಕಲು(socksreq, gss_w_token.ಮೌಲ್ಯ, gss_w_token.ಉದ್ದಳತೆ);
    gss_release_ಬಫರ್(&gss_ಸ್ಥಿತಿ, &gss_w_token);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(gss_recv_token.ಉದ್ದಳತೆ != 1) {
      failf(ದತ್ತಾಂಶ, "Invalid GSS-API encryption response length (%zu).",
            gss_recv_token.ಉದ್ದಳತೆ);
      curlx_safefree(gss_recv_token.ಮೌಲ್ಯ);
      gss_recv_token.ಉದ್ದಳತೆ = 0;
      Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }

    ಸ್ಮರಣೆ_ನಕಲು(socksreq, gss_recv_token.ಮೌಲ್ಯ, gss_recv_token.ಉದ್ದಳತೆ);
    curlx_safefree(gss_recv_token.ಮೌಲ್ಯ);
    gss_recv_token.ಉದ್ದಳತೆ = 0;
  }

  infof(ದತ್ತಾಂಶ, "SOCKS5 access %s protection granted.",
        (socksreq[0] == 0) ? "without GSS-API data" :
         ((socksreq[0] == 1) ? "with GSS-API integrity" :
          "with GSS-API confidentiality"));

  conn->socks5_gssapi_enctype = socksreq[0];
  ಆದರೆ(socksreq[0] == 0)
    Curl_gss_delete_sec_context(&gss_ಸ್ಥಿತಿ, gss_context, ಖಾಲಿ);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_SOCKS5_gssapi_negotiate(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ Curl_creds *creds)
{
  ರಚನೆ connectdata *conn = cf->conn;
  curl_socket_t sock = conn->sock[cf->sockindex];
  CURLcode ಫಲಿತಾಂಶ;
  OM_uint32 gss_ret_flags = 0;
  gss_name_t server = GSS_C_NO_NAME;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : "rcmd";
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;

  ಫಲಿತಾಂಶ = socks5_gss_create_service_name(ದತ್ತಾಂಶ, conn, service, &server);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    (ಶೂನ್ಯ)curlx_nonblock(sock, FALSE);
    ಫಲಿತಾಂಶ = socks5_gss_auth_loop(cf, ದತ್ತಾಂಶ, &server, &gss_context,
                                  &gss_ret_flags);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = socks5_gss_auth_verify(ದತ್ತಾಂಶ, &gss_context);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = socks5_gss_negotiate_enc(cf, ದತ್ತಾಂಶ, &gss_context, gss_ret_flags);

  /* unconditionally put it back to non-blocking */
  (ಶೂನ್ಯ)curlx_nonblock(sock, TRUE);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GSSAPI && !CURL_DISABLE_PROXY */
