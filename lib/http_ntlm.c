/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
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

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "http_ntlm.h"
#अन्तर्भाव "curl_ntlm_core.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#यदि_परिभाषितम् USE_WINDOWS_SSPI
#अन्तर्भाव "curl_sspi.h"
#यदि_अन्तः

CURLcode Curl_निवेशः_ntlm(संरचना Curl_easy *दत्त,
                         bool proxy,         /* if proxy or not */
                         ध्रुव वर्ण *शीर्षकम्) /* rest of the www-authenticate:
शीर्षकम्*/
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  यदि(checkprefix("NTLM", शीर्षकम्)) {
    संरचना ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    यदि(!ntlm)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    शीर्षकम् += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&शीर्षकम्);
    यदि(*शीर्षकम्) {
      अचिह्नित वर्ण *hdr;
      size_t hdrlen;

      परिणाम = curlx_base64_decode(शीर्षकम्, &hdr, &hdrlen);
      यदि(!परिणाम) {
        संरचना bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_समुच्चयः(&hdrbuf, hdr, hdrlen, curl_free);
        परिणाम = Curl_auth_decode_ntlm_type2_सन्देशः(दत्त, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    अन्यथा {
      यदि(*state == NTLMSTATE_चरमः) {
        infof(दत्त, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      अन्यथा यदि(*state == NTLMSTATE_TYPE3) {
        infof(दत्त, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        प्रत्यागम CURLE_REMOTE_ACCESS_DENIED;
      }
      अन्यथा यदि(*state >= NTLMSTATE_TYPE1) {
        infof(दत्त, "NTLM handshake failure (internal error)");
        प्रत्यागम CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  प्रत्यागम परिणाम;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(संरचना Curl_easy *दत्त, bool proxy)
{
  वर्ण *base64 = रिक्त;
  size_t len = 0;
  CURLcode परिणाम = CURLE_OK;
  संरचना bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  वर्ण **allocuserpwd;

  /* point to credentials and host */
  संरचना Curl_creds *creds = रिक्त;
  ध्रुव वर्ण *hostname = रिक्त;

  /* point to the correct struct with this */
  संरचना ntlmdata *ntlm;
  curlntlm *state;
  संरचना auth *authp;
  संरचना connectdata *conn = दत्त->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(दत्त);

  यदि(proxy) {
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    allocuserpwd = &दत्त->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &दत्त->state.authproxy;
#अन्यथा
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
  }
  अन्यथा {
    allocuserpwd = &दत्त->req.hd_auth;
    creds = दत्त->state.creds;
    hostname = दत्त->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &दत्त->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  यदि(!ntlm)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  authp->समाप्तम् = FALSE;

#यदि_परिभाषितम् USE_WINDOWS_SSPI
  यदि(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    परिणाम = Curl_sspi_global_init();
    यदि(!Curl_pSecFn)
      प्रत्यागम परिणाम;
  }
  ntlm->sslContext = conn->sslContext;
#यदि_अन्तः

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  यदि(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_चरमः;

  विकल्प(*state) {
  पक्ष NTLMSTATE_TYPE1:
  स्वभाव:  /* for the weird cases we (re)start here */
    यदि(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      यदि(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, दत्त->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        प्रत्यागम CURLE_FAILED_INIT;
      }
      यदि(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        प्रत्यागम CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, दत्त->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    परिणाम = Curl_auth_create_ntlm_type1_सन्देशः(दत्त, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    यदि(!परिणाम) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      परिणाम = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      यदि(!परिणाम) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        यदि(!*allocuserpwd)
          परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
    }
    भङ्ग;

  पक्ष NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    परिणाम = Curl_auth_create_ntlm_type3_सन्देशः(दत्त, creds, ntlm, &ntlmmsg);
    यदि(!परिणाम && Curl_bufref_len(&ntlmmsg)) {
      परिणाम = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      यदि(!परिणाम) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        यदि(!*allocuserpwd)
          परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        अन्यथा {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->समाप्तम् = TRUE;
        }
      }
    }
    भङ्ग;

  पक्ष NTLMSTATE_चरमः:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    यदि(proxy)
      दत्त->info.proxyauthpicked = CURLAUTH_NTLM;
    अन्यथा
      दत्त->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->समाप्तम् = TRUE;
    भङ्ग;
  }
  Curl_bufref_free(&ntlmmsg);

  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_HTTP && USE_NTLM */
