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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "http_ntlm.h"
#అన్తర్భావ "curl_ntlm_core.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#నిర్వచితమైతే USE_WINDOWS_SSPI
#అన్తర్భావ "curl_sspi.h"
#అయితే_ముగింపు

CURLcode Curl_ఆదానం_ntlm(నిర్మాణం Curl_easy *దత్తాంశం,
                         bool proxy,         /* if proxy or not */
                         స్థిరరాశి అక్షరం *శీర్షిక) /* rest of the www-authenticate:
శీర్షిక*/
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  ఒకవేళ(checkprefix("NTLM", శీర్షిక)) {
    నిర్మాణం ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    ఒకవేళ(!ntlm)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    శీర్షిక += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&శీర్షిక);
    ఒకవేళ(*శీర్షిక) {
      గుర్తులేని అక్షరం *hdr;
      size_t hdrlen;

      ఫలితం = curlx_base64_decode(శీర్షిక, &hdr, &hdrlen);
      ఒకవేళ(!ఫలితం) {
        నిర్మాణం bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_సమితి(&hdrbuf, hdr, hdrlen, curl_free);
        ఫలితం = Curl_auth_decode_ntlm_type2_సందేశం(దత్తాంశం, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    లేకపోతే {
      ఒకవేళ(*state == NTLMSTATE_చివరి) {
        infof(దత్తాంశం, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      లేకపోతే ఒకవేళ(*state == NTLMSTATE_TYPE3) {
        infof(దత్తాంశం, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        తిరిగి CURLE_REMOTE_ACCESS_DENIED;
      }
      లేకపోతే ఒకవేళ(*state >= NTLMSTATE_TYPE1) {
        infof(దత్తాంశం, "NTLM handshake failure (internal error)");
        తిరిగి CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  తిరిగి ఫలితం;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(నిర్మాణం Curl_easy *దత్తాంశం, bool proxy)
{
  అక్షరం *base64 = ఖాళీ;
  size_t len = 0;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  అక్షరం **allocuserpwd;

  /* point to credentials and host */
  నిర్మాణం Curl_creds *creds = ఖాళీ;
  స్థిరరాశి అక్షరం *hostname = ఖాళీ;

  /* point to the correct struct with this */
  నిర్మాణం ntlmdata *ntlm;
  curlntlm *state;
  నిర్మాణం auth *authp;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(దత్తాంశం);

  ఒకవేళ(proxy) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    allocuserpwd = &దత్తాంశం->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &దత్తాంశం->state.authproxy;
#లేకపోతే
    తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
  }
  లేకపోతే {
    allocuserpwd = &దత్తాంశం->req.hd_auth;
    creds = దత్తాంశం->state.creds;
    hostname = దత్తాంశం->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &దత్తాంశం->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  ఒకవేళ(!ntlm)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  authp->పూర్తి = FALSE;

#నిర్వచితమైతే USE_WINDOWS_SSPI
  ఒకవేళ(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    ఫలితం = Curl_sspi_global_init();
    ఒకవేళ(!Curl_pSecFn)
      తిరిగి ఫలితం;
  }
  ntlm->sslContext = conn->sslContext;
#అయితే_ముగింపు

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  ఒకవేళ(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_చివరి;

  ఎంపిక(*state) {
  స్థితి NTLMSTATE_TYPE1:
  అప్రమేయ:  /* for the weird cases we (re)start here */
    ఒకవేళ(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      ఒకవేళ(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, దత్తాంశం->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        తిరిగి CURLE_FAILED_INIT;
      }
      ఒకవేళ(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        తిరిగి CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, దత్తాంశం->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    ఫలితం = Curl_auth_create_ntlm_type1_సందేశం(దత్తాంశం, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    ఒకవేళ(!ఫలితం) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      ఫలితం = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      ఒకవేళ(!ఫలితం) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        ఒకవేళ(!*allocuserpwd)
          ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
    }
    ఆపు;

  స్థితి NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    ఫలితం = Curl_auth_create_ntlm_type3_సందేశం(దత్తాంశం, creds, ntlm, &ntlmmsg);
    ఒకవేళ(!ఫలితం && Curl_bufref_len(&ntlmmsg)) {
      ఫలితం = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      ఒకవేళ(!ఫలితం) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        ఒకవేళ(!*allocuserpwd)
          ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        లేకపోతే {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->పూర్తి = TRUE;
        }
      }
    }
    ఆపు;

  స్థితి NTLMSTATE_చివరి:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    ఒకవేళ(proxy)
      దత్తాంశం->info.proxyauthpicked = CURLAUTH_NTLM;
    లేకపోతే
      దత్తాంశం->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->పూర్తి = TRUE;
    ఆపు;
  }
  Curl_bufref_free(&ntlmmsg);

  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && USE_NTLM */
