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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "http_ntlm.h"
#অন্তর্ভাব "curl_ntlm_core.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#ifdef USE_WINDOWS_SSPI
#অন্তর্ভাব "curl_sspi.h"
#endif

CURLcode Curl_প্রবেশ_ntlm(কাঠামো Curl_easy *তথ্য,
                         bool proxy,         /* if proxy or not */
                         ধ্রুবক অক্ষর *header) /* rest of the www-authenticate:
                                                header */
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  যদি(checkprefix("NTLM", header)) {
    কাঠামো ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    যদি(!ntlm)
      ফেরত CURLE_OUT_OF_MEMORY;

    header += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&header);
    যদি(*header) {
      unsigned অক্ষর *hdr;
      size_t hdrlen;

      ফলাফল = curlx_base64_decode(header, &hdr, &hdrlen);
      যদি(!ফলাফল) {
        কাঠামো bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_set(&hdrbuf, hdr, hdrlen, curl_free);
        ফলাফল = Curl_auth_decode_ntlm_type2_message(তথ্য, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      যদি(ফলাফল)
        ফেরত ফলাফল;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    অন্যথা {
      যদি(*state == NTLMSTATE_LAST) {
        infof(তথ্য, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      অন্যথা যদি(*state == NTLMSTATE_TYPE3) {
        infof(তথ্য, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        ফেরত CURLE_REMOTE_ACCESS_DENIED;
      }
      অন্যথা যদি(*state >= NTLMSTATE_TYPE1) {
        infof(তথ্য, "NTLM handshake failure (internal error)");
        ফেরত CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  ফেরত ফলাফল;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(কাঠামো Curl_easy *তথ্য, bool proxy)
{
  অক্ষর *base64 = ফাঁকা;
  size_t len = 0;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  অক্ষর **allocuserpwd;

  /* point to credentials and host */
  কাঠামো Curl_creds *creds = ফাঁকা;
  ধ্রুবক অক্ষর *hostname = ফাঁকা;

  /* point to the correct struct with this */
  কাঠামো ntlmdata *ntlm;
  curlntlm *state;
  কাঠামো auth *authp;
  কাঠামো connectdata *conn = তথ্য->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(তথ্য);

  যদি(proxy) {
#ifndef CURL_DISABLE_PROXY
    allocuserpwd = &তথ্য->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &তথ্য->state.authproxy;
#অন্যথা
    ফেরত CURLE_NOT_BUILT_IN;
#endif
  }
  অন্যথা {
    allocuserpwd = &তথ্য->req.hd_auth;
    creds = তথ্য->state.creds;
    hostname = তথ্য->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &তথ্য->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  যদি(!ntlm)
    ফেরত CURLE_OUT_OF_MEMORY;
  authp->done = FALSE;

#ifdef USE_WINDOWS_SSPI
  যদি(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    ফলাফল = Curl_sspi_global_init();
    যদি(!Curl_pSecFn)
      ফেরত ফলাফল;
  }
  ntlm->sslContext = conn->sslContext;
#endif

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  যদি(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_LAST;

  নির্বাচন(*state) {
  ক্ষেত্র NTLMSTATE_TYPE1:
  পূর্বনির্ধারিত:  /* for the weird cases we (re)start here */
    যদি(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      যদি(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, তথ্য->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        ফেরত CURLE_FAILED_INIT;
      }
      যদি(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        ফেরত CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, তথ্য->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    ফলাফল = Curl_auth_create_ntlm_type1_message(তথ্য, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    যদি(!ফলাফল) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      ফলাফল = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      যদি(!ফলাফল) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        যদি(!*allocuserpwd)
          ফলাফল = CURLE_OUT_OF_MEMORY;
      }
    }
    বিরতি;

  ক্ষেত্র NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    ফলাফল = Curl_auth_create_ntlm_type3_message(তথ্য, creds, ntlm, &ntlmmsg);
    যদি(!ফলাফল && Curl_bufref_len(&ntlmmsg)) {
      ফলাফল = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      যদি(!ফলাফল) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        যদি(!*allocuserpwd)
          ফলাফল = CURLE_OUT_OF_MEMORY;
        অন্যথা {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->done = TRUE;
        }
      }
    }
    বিরতি;

  ক্ষেত্র NTLMSTATE_LAST:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    যদি(proxy)
      তথ্য->info.proxyauthpicked = CURLAUTH_NTLM;
    অন্যথা
      তথ্য->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->done = TRUE;
    বিরতি;
  }
  Curl_bufref_free(&ntlmmsg);

  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_HTTP && USE_NTLM */
