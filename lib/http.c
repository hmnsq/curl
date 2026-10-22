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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "formdata.h"
#ಅನ್ತರ್ಭಾವ "mime.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "cookie.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "http_digest.h"
#ಅನ್ತರ್ಭಾವ "http_ntlm.h"
#ಅನ್ತರ್ಭಾವ "http_negotiate.h"
#ಅನ್ತರ್ಭಾವ "http_aws_sigv4.h"
#ಅನ್ತರ್ಭಾವ "http_httpsig.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "urlapi-int.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "dynhds.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "headers.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h" /* for the week day and month names */
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "content_encoding.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "http2.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "altsvc.h"
#ಅನ್ತರ್ಭಾವ "hsts.h"
#ಅನ್ತರ್ಭಾವ "rtsp.h"
#ಅನ್ತರ್ಭಾವ "ws.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

ಶೂನ್ಯ Curl_http_neg_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ http_negotiation *neg)
{
  memset(neg, 0, ಗಾತ್ರ(*neg));
  neg->accept_09 = ದತ್ತಾಂಶ->ಗಣ.http09_allowed;
  ಆಯ್ಕೆ(ದತ್ತಾಂಶ->ಗಣ.httpwant) {
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_2_TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_2_ಪೂರ್ವ_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    ದತ್ತಾಂಶ->state.http_neg.h2_ಪೂರ್ವ_knowledge = TRUE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_3_ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_NONE:
  ಪೂರ್ವನಿಯೋಜಿತ:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    ನಿಲ್ಲಿಸು;
  }
}

CURLcode Curl_http_ಸಿದ್ಧತೆ_conn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  ಆದರೆ(ದತ್ತಾಂಶ->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode ಫಲಿತಾಂಶ = Curl_conn_may_http3(ದತ್ತಾಂಶ, conn);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
/*
 * checkProxyHeaders() checks the linked list of custom proxy headers
 * if proxy headers are not available, then it will lookup into http header
 * link list
 *
 * It takes a connectdata struct as input to see if this is a proxy request or
 * not, as it then might check a different header list. Provide the header
 * prefix without colon!
 */
ಅಕ್ಷರ *Curl_checkProxyheaders(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ connectdata *conn,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *thisheader,
                             ಸ್ಥಿರಾಂಕ size_t thislen)
{
  ರಚನೆ curl_slist *ತಲೆ;

  ಗಾಗಿ(ತಲೆ = (conn->http_proxy.peer && ದತ್ತಾಂಶ->ಗಣ.sep_ಶೀರ್ಷಿಕೆಗಳು) ?
        ದತ್ತಾಂಶ->ಗಣ.proxyheaders : ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು;
      ತಲೆ; ತಲೆ = ತಲೆ->ಮುಂದಿನ) {
    ಆದರೆ(curl_strnequal(ತಲೆ->ದತ್ತಾಂಶ, thisheader, thislen) &&
       Curl_headersep(ತಲೆ->ದತ್ತಾಂಶ[thislen]))
      ಹಿಂತಿರುಗಿ ತಲೆ->ದತ್ತಾಂಶ;
  }

  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
ಸ್ಥಿರ bool ಶೀರ್ಷಿಕೆ_has_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **headerp, ರಚನೆ Curl_str *outp)
{
  bool ಮೌಲ್ಯ = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  ಆದರೆ(ಮೌಲ್ಯ) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
}

ಸ್ಥಿರ bool http_ಶೀರ್ಷಿಕೆ_is_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ)
{
  ರಚನೆ Curl_str ಹೊರಹೋಗು;

  ಆದರೆ(ಶೀರ್ಷಿಕೆ_has_ಮೌಲ್ಯ(&ಶೀರ್ಷಿಕೆ, &ಹೊರಹೋಗು)) {
    ಹಿಂತಿರುಗಿ curlx_strlen(&ಹೊರಹೋಗು) == 0;
  }
  ಹಿಂತಿರುಗಿ TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
ಸ್ಥಿರ CURLcode copy_custom_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ, ಅಕ್ಷರ **valp)
{
  ರಚನೆ Curl_str ಹೊರಹೋಗು = { 0 };

  /* find the end of the header name */
  ಆದರೆ(ಶೀರ್ಷಿಕೆ_has_ಮೌಲ್ಯ(&ಶೀರ್ಷಿಕೆ, &ಹೊರಹೋಗು)) {
    *valp = curlx_memdup0(curlx_str(&ಹೊರಹೋಗು), curlx_strlen(&ಹೊರಹೋಗು));
    ಆದರೆ(*valp)
      ಹಿಂತಿರುಗಿ CURLE_OK;
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  /* bad input */
  *valp = ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * This function MUST be used after the header has already been confirmed to
 * lead with "word:".
 *
 * @unittest: 1626
 */
ಅಕ್ಷರ *Curl_copy_ಶೀರ್ಷಿಕೆ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ)
{
  ರಚನೆ Curl_str ಹೊರಹೋಗು;

  /* find the end of the header name */
  ಆದರೆ(!curlx_str_until(&ಶೀರ್ಷಿಕೆ, &ಹೊರಹೋಗು, MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ, ':') &&
     !curlx_str_single(&ಶೀರ್ಷಿಕೆ, ':')) {
    curlx_str_untilnl(&ಶೀರ್ಷಿಕೆ, &ಹೊರಹೋಗು, MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
    curlx_str_trimblanks(&ಹೊರಹೋಗು);
    ಹಿಂತಿರುಗಿ curlx_memdup0(curlx_str(&ಹೊರಹೋಗು), curlx_strlen(&ಹೊರಹೋಗು));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP_AUTH

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
ಸ್ಥಿರ CURLcode http_output_basic(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn, bool proxy)
{
  size_t ಗಾತ್ರ_2 = 0;
  ಅಕ್ಷರ *authorization = ಖಾಲಿ;
  ಅಕ್ಷರ **p_hd;
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ Curl_creds *creds = ಖಾಲಿ;
  ಅಕ್ಷರ *ಹೊರಹೋಗು;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  ಆದರೆ(proxy) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    p_hd = &ದತ್ತಾಂಶ->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#ಇಲ್ಲದಿದ್ದರೆ
    (ಶೂನ್ಯ)conn;
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    p_hd = &ದತ್ತಾಂಶ->req.hd_auth;
    creds = ದತ್ತಾಂಶ->state.creds;
  }

  ಆದರೆ(!creds) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಹೊರಹೋಗು = curl_maprintf("%s:%s", creds->user, creds->passwd);
  ಆದರೆ(!ಹೊರಹೋಗು)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಫಲಿತಾಂಶ = curlx_base64_encode((uint8_t *)ಹೊರಹೋಗು, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಹೊರಹೋಗು),
                               &authorization, &ಗಾತ್ರ_2);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  ಆದರೆ(!authorization) {
    ಫಲಿತಾಂಶ = CURLE_REMOTE_ACCESS_DENIED;
    ಹೋಗು fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  ಆದರೆ(!*p_hd) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು fail;
  }

fail:
  curlx_free(ಹೊರಹೋಗು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
ಸ್ಥಿರ CURLcode http_output_bearer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಅಕ್ಷರ **userp;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(ದತ್ತಾಂಶ->state.creds));
  userp = &ದತ್ತಾಂಶ->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(ದತ್ತಾಂಶ->state.creds));

  ಆದರೆ(!*userp) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು fail;
  }

fail:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
ಸ್ಥಿರ bool pickoneauth(ರಚನೆ auth *pick, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ,
                        ರಚನೆ Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ avail = pick->avail & pick->want & ಮುಖವಾಡ;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  ಆದರೆ(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BEARER_AUTH
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#ಆದರೆ_ಅಂತ್ಯ
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BASIC_AUTH
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_AWS
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTPSIG
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#ಆದರೆ_ಅಂತ್ಯ
  ಇಲ್ಲದಿದ್ದರೆ {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  ಹಿಂತಿರುಗಿ picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
ಸ್ಥಿರ CURLcode http_perhapsrewind(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ connectdata *conn)
{
  curl_off_t bytessent = ದತ್ತಾಂಶ->req.writebytecount;
  curl_off_t expectsend = Curl_creader_ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(ದತ್ತಾಂಶ);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!ದತ್ತಾಂಶ->req.upload_ಮುಗಿದ && !little_upload_remains);
  VERBOSE(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ongoing_auth = ಖಾಲಿ);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  ಆದರೆ(needs_rewind) {
    infof(ದತ್ತಾಂಶ, "Need to rewind upload for next request");
    Curl_creader_ಗಣ_rewind(ದತ್ತಾಂಶ, TRUE);
  }

  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.close)
    /* If we already decided to close this connection, we cannot veto. */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
    ಆದರೆ((ದತ್ತಾಂಶ->state.authproxy.picked == CURLAUTH_NTLM) ||
       (ದತ್ತಾಂಶ->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      ಆದರೆ((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
    /* There is still data left to send */
    ಆದರೆ((ದತ್ತಾಂಶ->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (ದತ್ತಾಂಶ->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      ಆದರೆ((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಆದರೆ(abort_upload) {
    ಆದರೆ(upload_remain >= 0)
      infof(ದತ್ತಾಂಶ, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = 0; /* do not download any more than 0 bytes */
    ದತ್ತಾಂಶ->req.http_bodyless = TRUE;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
ಸ್ಥಿರ bool http_should_fail(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ httpcode)
{
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.http_fail_on_error)
    ಹಿಂತಿರುಗಿ FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  ಆದರೆ(httpcode < 400)
    ಹಿಂತಿರುಗಿ FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from && ದತ್ತಾಂಶ->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    ಹಿಂತಿರುಗಿ FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  ಆದರೆ((httpcode != 401) && (httpcode != 407))
    ಹಿಂತಿರುಗಿ TRUE;

  /*
   * All we have left to deal with is 401 and 407
   */
  DEBUGASSERT((httpcode == 401) || (httpcode == 407));

  /*
   * Examine the current authentication state to see if this is an error. The
   * idea is for this function to get called after processing all the headers
   * in a response message. If we have been asked to authenticate at
   * a particular stage, and we have done it, we are OK. If we are already
   * completely authenticated, it is not OK to get another 401 or 407.
   *
   * It is possible for authentication to go stale such that the client needs
   * to reauthenticate. Once that info is available, use it here.
   */

  /*
   * Either we are not authenticating, or we are supposed to be authenticating
   * something else. This is an error.
   */
  ಆದರೆ((httpcode == 401) && !ದತ್ತಾಂಶ->state.creds)
    ಹಿಂತಿರುಗಿ TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ((httpcode == 407) && !ದತ್ತಾಂಶ->conn->http_proxy.creds)
    ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ (bool)ದತ್ತಾಂಶ->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ authmask = ~0UL;

  ಆದರೆ(!Curl_creds_has_oauth_bearer(ದತ್ತಾಂಶ->state.creds))
    authmask &= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)~CURLAUTH_BEARER;

  ಆದರೆ(100 <= ದತ್ತಾಂಶ->req.httpcode && ದತ್ತಾಂಶ->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(ದತ್ತಾಂಶ->state.authproblem)
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->ಗಣ.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  ಆದರೆ(ದತ್ತಾಂಶ->state.creds &&
     ((ದತ್ತಾಂಶ->req.httpcode == 401) ||
      (ದತ್ತಾಂಶ->req.authneg && ದತ್ತಾಂಶ->req.httpcode < 300))) {
    pickhost = pickoneauth(&ದತ್ತಾಂಶ->state.authhost, authmask, ದತ್ತಾಂಶ->state.creds);
    ಆದರೆ(!pickhost)
      ದತ್ತಾಂಶ->state.authproblem = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
      ದತ್ತಾಂಶ->info.httpauthpicked = ದತ್ತಾಂಶ->state.authhost.picked;
    ಆದರೆ(ದತ್ತಾಂಶ->state.authhost.picked == CURLAUTH_NTLM &&
       (ದತ್ತಾಂಶ->req.httpversion_ಕಳುಹಿಸಿದ > 11)) {
      infof(ದತ್ತಾಂಶ, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      ದತ್ತಾಂಶ->state.http_neg.wanted = CURL_HTTP_V1x;
      ದತ್ತಾಂಶ->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ(conn->http_proxy.creds &&
     ((ದತ್ತಾಂಶ->req.httpcode == 407) ||
      (ದತ್ತಾಂಶ->req.authneg && ದತ್ತಾಂಶ->req.httpcode < 300))) {
    pickproxy = pickoneauth(&ದತ್ತಾಂಶ->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    ಆದರೆ(!pickproxy)
      ದತ್ತಾಂಶ->state.authproblem = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
      ದತ್ತಾಂಶ->info.proxyauthpicked = ದತ್ತಾಂಶ->state.authproxy.picked;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(pickhost || pickproxy) {
    ಫಲಿತಾಂಶ = http_perhapsrewind(ದತ್ತಾಂಶ, conn);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(ದತ್ತಾಂಶ->req.newurl);
    /* clone URL */
    ದತ್ತಾಂಶ->req.newurl = Curl_bufref_dup(&ದತ್ತಾಂಶ->state.url);
    ಆದರೆ(!ದತ್ತಾಂಶ->req.newurl)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ದತ್ತಾಂಶ->req.httpcode < 300) &&
          !ದತ್ತಾಂಶ->state.authhost.ಮುಗಿದ &&
          ದತ್ತಾಂಶ->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (ದತ್ತಾಂಶ->state.httpreq != HTTPREQ_GET) &&
          (ದತ್ತಾಂಶ->state.httpreq != HTTPREQ_ತಲೆ)) {
    /* clone URL */
    ದತ್ತಾಂಶ->req.newurl = Curl_bufref_dup(&ದತ್ತಾಂಶ->state.url);
    ಆದರೆ(!ದತ್ತಾಂಶ->req.newurl)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ದತ್ತಾಂಶ->state.authhost.ಮುಗಿದ = TRUE;
  }
  ಆದರೆ(http_should_fail(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->req.httpcode)) {
    failf(ದತ್ತಾಂಶ, "The requested URL returned error: %d",
          ದತ್ತಾಂಶ->req.httpcode);
    ಫಲಿತಾಂಶ = CURLE_HTTP_RETURNED_ERROR;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
ಸ್ಥಿರ CURLcode output_auth_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ connectdata *conn,
                                    ರಚನೆ auth *authstatus,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *request,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                                    bool proxy)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *auth = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)conn;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_DIGEST_AUTH
  (ಶೂನ್ಯ)request;
  (ಶೂನ್ಯ)ಮಾರ್ಗ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_AWS
  ಆದರೆ((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    ಫಲಿತಾಂಶ = Curl_output_aws_sigv4(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTPSIG
  ಆದರೆ((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    ಆದರೆ(Curl_auth_allowed_ಗಮ್ಯ_host(ದತ್ತಾಂಶ)) {
      auth = "HTTPSIG";
      ಫಲಿತಾಂಶ = Curl_output_httpsig(ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      authstatus->ಮುಗಿದ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
  ಆದರೆ(authstatus->picked == CURLAUTH_NEGOTIATE) {
    ಆದರೆ(
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(ದತ್ತಾಂಶ, conn,
                                        STRCONST("Proxy-authorization"))) ||
#ಆದರೆ_ಅಂತ್ಯ
      (!proxy && !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Authorization")) &&
       Curl_auth_allowed_ಗಮ್ಯ_host(ದತ್ತಾಂಶ))) {
      auth = "Negotiate";
      ಫಲಿತಾಂಶ = Curl_output_negotiate(ದತ್ತಾಂಶ, conn, proxy);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      authstatus->ಮುಗಿದ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
  ಆದರೆ(authstatus->picked == CURLAUTH_NTLM) {
    ಆದರೆ(proxy || Curl_auth_allowed_ಗಮ್ಯ_host(ದತ್ತಾಂಶ)) {
      auth = "NTLM";
      ಫಲಿತಾಂಶ = Curl_output_ntlm(ದತ್ತಾಂಶ, proxy);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      authstatus->ಮುಗಿದ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
  ಆದರೆ(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    ಫಲಿತಾಂಶ = Curl_output_digest(ದತ್ತಾಂಶ,
                                proxy,
                                (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)request,
                                (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಮಾರ್ಗ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BASIC_AUTH
  ಆದರೆ(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    ಆದರೆ(
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
       (proxy && conn->http_proxy.creds &&
        Curl_creds_has_user_or_pass(conn->http_proxy.creds) &&
        !Curl_checkProxyheaders(ದತ್ತಾಂಶ, conn,
                                STRCONST("Proxy-authorization"))) ||
#ಆದರೆ_ಅಂತ್ಯ
       (!proxy && ದತ್ತಾಂಶ->state.creds &&
        Curl_creds_has_user_or_pass(ದತ್ತಾಂಶ->state.creds) &&
        !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Authorization")))) {
      auth = "Basic";
      ಫಲಿತಾಂಶ = http_output_basic(ದತ್ತಾಂಶ, conn, proxy);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->ಮುಗಿದ = TRUE;
  }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BEARER_AUTH
  ಆದರೆ(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    ಆದರೆ(!proxy && Curl_creds_has_oauth_bearer(ದತ್ತಾಂಶ->state.creds) &&
       !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Authorization"))) {
      auth = "Bearer";
      ಫಲಿತಾಂಶ = http_output_bearer(ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->ಮುಗಿದ = TRUE;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(auth) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    ಆದರೆ(proxy)
      ದತ್ತಾಂಶ->info.proxyauthpicked = authstatus->picked;
    ಇಲ್ಲದಿದ್ದರೆ
      ದತ್ತಾಂಶ->info.httpauthpicked = authstatus->picked;
    infof(ದತ್ತಾಂಶ, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (ದತ್ತಾಂಶ->state.creds ?
           ದತ್ತಾಂಶ->state.creds->user : ""));
#ಇಲ್ಲದಿದ್ದರೆ
    (ಶೂನ್ಯ)proxy;
    infof(ದತ್ತಾಂಶ, "Server auth using %s with user '%s'",
          auth, ದತ್ತಾಂಶ->state.creds ?
          ದತ್ತಾಂಶ->state.creds->user : "");
#ಆದರೆ_ಅಂತ್ಯ
    authstatus->multipass = !authstatus->ಮುಗಿದ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    authstatus->multipass = FALSE;
    ಆದರೆ(proxy)
      ದತ್ತಾಂಶ->info.proxyauthpicked = 0;
    ಇಲ್ಲದಿದ್ದರೆ
      ದತ್ತಾಂಶ->info.httpauthpicked = 0;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_http_output_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *request,
                               Curl_HttpReq httpreq,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query,
                               bool is_connect)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ auth *authhost;
  ರಚನೆ auth *authproxy;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ_and_query = ಮಾರ್ಗ;
  ಅಕ್ಷರ *tmp_str = ಖಾಲಿ;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  authhost = &ದತ್ತಾಂಶ->state.authhost;
  authproxy = &ದತ್ತಾಂಶ->state.authproxy;

  ಆದರೆ(
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    (!conn->http_proxy.peer || !conn->http_proxy.creds) &&
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
    !(authhost->want & CURLAUTH_NEGOTIATE) &&
    !(authproxy->want & CURLAUTH_NEGOTIATE) &&
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTPSIG
    !(authhost->want & CURLAUTH_HTTPSIG) &&
#ಆದರೆ_ಅಂತ್ಯ
    !ದತ್ತಾಂಶ->state.creds) {
    /* no authentication with no user or password */
    authhost->ಮುಗಿದ = TRUE;
    authproxy->ಮುಗಿದ = TRUE;
    ಫಲಿತಾಂಶ = CURLE_OK;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(query) {
    tmp_str = curl_maprintf("%s?%s", ಮಾರ್ಗ, query);
    ಆದರೆ(!tmp_str) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಮಾರ್ಗ_and_query = tmp_str;
  }

  ಆದರೆ(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  ಆದರೆ(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy || is_connect) {
    ಫಲಿತಾಂಶ = output_auth_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, conn, authproxy, request,
                                 ಮಾರ್ಗ_and_query, TRUE);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)is_connect;
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->ಮುಗಿದ = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  ಆದರೆ(ದತ್ತಾಂಶ->state.creds || Curl_auth_allowed_ಗಮ್ಯ_host(ದತ್ತಾಂಶ))
    ಫಲಿತಾಂಶ = output_auth_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, conn, authhost, request,
                                 ಮಾರ್ಗ_and_query, FALSE);
  ಇಲ್ಲದಿದ್ದರೆ
    authhost->ಮುಗಿದ = TRUE;

  ಆದರೆ(((authhost->multipass && !authhost->ಮುಗಿದ) ||
      (authproxy->multipass && !authproxy->ಮುಗಿದ)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_ತಲೆ)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    ದತ್ತಾಂಶ->req.authneg = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ದತ್ತಾಂಶ->req.authneg = FALSE;

ಹೊರಹೋಗು:
  curlx_free(tmp_str);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಇಲ್ಲದಿದ್ದರೆ /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *request,
                               Curl_HttpReq httpreq,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query,
                               bool is_connect)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)request;
  (ಶೂನ್ಯ)httpreq;
  (ಶೂನ್ಯ)ಮಾರ್ಗ;
  (ಶೂನ್ಯ)query;
  (ಶೂನ್ಯ)is_connect;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP_AUTH, else */

#ಆದರೆ defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
ಸ್ಥಿರ bool authcmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *auth, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು)
{
  /* the auth string must not have an alnum following */
  size_t n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(auth);
  ಹಿಂತಿರುಗಿ curl_strnequal(auth, ಸಾಲು, n) && !ISALNUM(ಸಾಲು[n]);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
ಸ್ಥಿರ CURLcode auth_spnego(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            bool proxy,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *auth,
                            ರಚನೆ auth *authp,
                            uint32_t *availp)
{
  ಆದರೆ((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    ಆದರೆ(authp->picked == CURLAUTH_NEGOTIATE) {
      ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
      CURLcode ಫಲಿತಾಂಶ = Curl_ಒಳಹರಿವು_negotiate(ದತ್ತಾಂಶ, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        curlx_free(ದತ್ತಾಂಶ->req.newurl);
        ದತ್ತಾಂಶ->req.newurl = Curl_bufref_dup(&ದತ್ತಾಂಶ->state.url);
        ಆದರೆ(!ದತ್ತಾಂಶ->req.newurl)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ದತ್ತಾಂಶ->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ದತ್ತಾಂಶ->state.authproblem = TRUE;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
ಸ್ಥಿರ CURLcode auth_ntlm(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          bool proxy,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *auth,
                          ರಚನೆ auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  ಆದರೆ((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    ಆದರೆ(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode ಫಲಿತಾಂಶ = Curl_ಒಳಹರಿವು_ntlm(ದತ್ತಾಂಶ, proxy, auth);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ದತ್ತಾಂಶ->state.authproblem = FALSE;
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        infof(ದತ್ತಾಂಶ, "NTLM authentication problem, ignoring.");
        ದತ್ತಾಂಶ->state.authproblem = TRUE;
      }
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
ಸ್ಥಿರ CURLcode auth_digest(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            bool proxy,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *auth,
                            ರಚನೆ auth *authp,
                            uint32_t *availp)
{
  ಆದರೆ(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(ದತ್ತಾಂಶ, "Ignoring duplicate digest auth header.");
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Curl_auth_is_digest_supported()) {
    CURLcode ಫಲಿತಾಂಶ;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    ಫಲಿತಾಂಶ = Curl_ಒಳಹರಿವು_digest(ದತ್ತಾಂಶ, proxy, auth);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      infof(ದತ್ತಾಂಶ, "Digest authentication problem, ignoring.");
      ದತ್ತಾಂಶ->state.authproblem = TRUE;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BASIC_AUTH
ಸ್ಥಿರ CURLcode auth_basic(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  ಆದರೆ(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(ದತ್ತಾಂಶ, "Basic authentication problem, ignoring.");
    ದತ್ತಾಂಶ->state.authproblem = TRUE;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BEARER_AUTH
ಸ್ಥಿರ CURLcode auth_bearer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  ಆದರೆ(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(ದತ್ತಾಂಶ, "Bearer authentication problem, ignoring.");
    ದತ್ತಾಂಶ->state.authproblem = TRUE;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_ಒಳಹರಿವು_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool proxy,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#ಆದರೆ defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  ರಚನೆ auth *authp;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(ದತ್ತಾಂಶ);

  ಆದರೆ(proxy) {
    availp = &ದತ್ತಾಂಶ->info.proxyauthavail;
    authp = &ದತ್ತಾಂಶ->state.authproxy;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    availp = &ದತ್ತಾಂಶ->info.httpauthavail;
    authp = &ದತ್ತಾಂಶ->state.authhost;
  }

  /*
   * Here we check if we want the specific single authentication (using ==) and
   * if we do, we initiate usage of it.
   *
   * If the provided authentication is wanted as one out of several accepted
   * types (using &), we OR this authentication type to the authavail
   * variable.
   *
   * Note:
   *
   * ->picked is first set to the 'want' value (one or more bits) before the
   * request is sent, and then it is again set _after_ all response 401/407
   * headers have been received but then only to a single preferred method
   * (bit).
   */

  ಆಗ(*auth) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
    ಆದರೆ(authcmp("Negotiate", auth))
      ಫಲಿತಾಂಶ = auth_spnego(ದತ್ತಾಂಶ, proxy, auth, authp, availp);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
    ಆದರೆ(!ಫಲಿತಾಂಶ && authcmp("NTLM", auth))
      ಫಲಿತಾಂಶ = auth_ntlm(ದತ್ತಾಂಶ, proxy, auth, authp, availp);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
    ಆದರೆ(!ಫಲಿತಾಂಶ && authcmp("Digest", auth))
      ಫಲಿತಾಂಶ = auth_digest(ದತ್ತಾಂಶ, proxy, auth, authp, availp);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BASIC_AUTH
    ಆದರೆ(!ಫಲಿತಾಂಶ && authcmp("Basic", auth))
      ಫಲಿತಾಂಶ = auth_basic(ದತ್ತಾಂಶ, authp, availp);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BEARER_AUTH
    ಆದರೆ(authcmp("Bearer", auth))
      ಫಲಿತಾಂಶ = auth_bearer(ದತ್ತಾಂಶ, authp, availp);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    ಆದರೆ(auth) /* if we are on a comma, skip it */
      auth++;
    ಇಲ್ಲದಿದ್ದರೆ
      ನಿಲ್ಲಿಸು;
    curlx_str_passblanks(&auth);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)proxy;
  (ಶೂನ್ಯ)auth;
  /* nothing to do when disabled */
  ಹಿಂತಿರುಗಿ CURLE_OK;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ http_switch_ಗಮ್ಯ_get(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *req = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST);

  ಆದರೆ((req || ದತ್ತಾಂಶ->state.httpreq != HTTPREQ_GET) &&
     (ದತ್ತಾಂಶ->ಗಣ.http_follow_ಬಹುಲಕ == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((ಶೂನ್ಯ)ಸಂಕೇತ_2);
    infof(ದತ್ತಾಂಶ, "Switch to GET because of %d response", ಸಂಕೇತ_2);
    ದತ್ತಾಂಶ->state.http_ignorecustom = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(req && (ದತ್ತಾಂಶ->ಗಣ.http_follow_ಬಹುಲಕ != CURLFOLLOW_FIRSTONLY))
    infof(ದತ್ತಾಂಶ, "Stick to %s instead of GET", req);

  ದತ್ತಾಂಶ->state.httpreq = HTTPREQ_GET;
  Curl_creader_ಗಣ_rewind(ದತ್ತಾಂಶ, FALSE);
}

#ವ್ಯಾಖ್ಯಾನಿಸು HTTPREQ_IS_POST(ದತ್ತಾಂಶ)                    \
  ((ದತ್ತಾಂಶ)->state.httpreq == HTTPREQ_POST ||      \
   (ದತ್ತಾಂಶ)->state.httpreq == HTTPREQ_POST_FORM || \
   (ದತ್ತಾಂಶ)->state.httpreq == HTTPREQ_POST_MIME)

ಸ್ಥಿರ CURLcode ಗಣ_auto_referer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLU *u;
  ಅಕ್ಷರ *referer = ಖಾಲಿ;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&ದತ್ತಾಂಶ->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  ಆದರೆ(!u)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  uc = curl_url_ಗಣ(u, CURLUPART_URL,
                    Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url), 0);
  ಆದರೆ(!uc)
    uc = curl_url_ಗಣ(u, CURLUPART_FRAGMENT, ಖಾಲಿ, 0);
  ಆದರೆ(!uc)
    uc = curl_url_ಗಣ(u, CURLUPART_USER, ಖಾಲಿ, 0);
  ಆದರೆ(!uc)
    uc = curl_url_ಗಣ(u, CURLUPART_PASSWORD, ಖಾಲಿ, 0);
  ಆದರೆ(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  ಆದರೆ(uc || !referer)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  Curl_bufref_ಗಣ(&ದತ್ತಾಂಶ->state.referer, referer, 0, curl_free);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode follow_parse_url(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newurl,
                                 followtype type, ಅಕ್ಷರ **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(ದತ್ತಾಂಶ->state.uh);
  uc = curl_url_ಗಣ(ದತ್ತಾಂಶ->state.uh, CURLUPART_URL, newurl, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (ದತ್ತಾಂಶ->ಗಣ.ಮಾರ್ಗ_as_is ? CURLU_ಮಾರ್ಗ_AS_IS : 0)));
  *ucp = uc;
  ಆದರೆ(uc) {
    ಆದರೆ((uc == CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) || (type != FOLLOW_FAKE)) {
      failf(ದತ್ತಾಂಶ, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      ಹಿಂತಿರುಗಿ Curl_uc_ಗಮ್ಯ_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    ಆದರೆ(!*follow_urlp)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURLU *u = curl_url();
    ಆದರೆ(!u)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    uc = curl_url_ಗಣ(u, CURLUPART_URL,
                      Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    ಆದರೆ(!uc)
      uc = curl_url_get(ದತ್ತಾಂಶ->state.uh, CURLUPART_URL, follow_urlp, 0);
    ಆದರೆ(uc) {
      curl_url_cleanup(u);
      ಹಿಂತಿರುಗಿ Curl_uc_ಗಮ್ಯ_curlcode(uc);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, ದತ್ತಾಂಶ->state.uh);
      curl_url_cleanup(u);
      ಆದರೆ(!same_origin)
        Curl_auth_digest_cleanup(&ದತ್ತಾಂಶ->state.digest);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    curl_url_cleanup(u);
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode follow_fake(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  ಆದರೆ(!uc) {
    CURLU *u = curl_url();
    ಅಕ್ಷರ *nocred = ಖಾಲಿ;

    ಆದರೆ(!u) {
      curlx_free(follow_url);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಆದರೆ(!curl_url_ಗಣ(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (ದತ್ತಾಂಶ->ಗಣ.ಮಾರ್ಗ_as_is ? CURLU_ಮಾರ್ಗ_AS_IS : 0)) &&
       !curl_url_ಗಣ(u, CURLUPART_USER, ಖಾಲಿ, 0) &&
       !curl_url_ಗಣ(u, CURLUPART_PASSWORD, ಖಾಲಿ, 0))
      (ಶೂನ್ಯ)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_ಖಾಲಿಗಳು);
    curl_url_cleanup(u);
    ಆದರೆ(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  ದತ್ತಾಂಶ->info.wouldredirect = follow_url;

  ಆದರೆ(reachedmax) {
    failf(ದತ್ತಾಂಶ, "Maximum (%d) redirects followed", ದತ್ತಾಂಶ->ಗಣ.maxredirs);
    ಹಿಂತಿರುಗಿ CURLE_TOO_MANY_REDIRECTS;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool follow_switch_get(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  ಆಯ್ಕೆ(ದತ್ತಾಂಶ->info.httpcode) {
  ಸ್ಥಿತಿ 301: /* Moved Permanently */
    /* (quote from RFC7231, section 6.4.2)
     *
     * Note: For historical reasons, a user agent MAY change the request
     * method from POST to GET for the subsequent request. If this
     * behavior is undesired, the 307 (Temporary Redirect) status code
     * can be used instead.
     *
     * ----
     *
     * Many webservers expect this, so these servers often answers to a POST
     * request with an error page. To be sure that libcurl gets the page that
     * most user agents would get, libcurl has to force GET.
     *
     * This behavior is forbidden by RFC1945 and the obsolete RFC2616, and
     * can be overridden with CURLOPT_POSTREDIR.
     */
    ಆದರೆ(HTTPREQ_IS_POST(ದತ್ತಾಂಶ) && !ದತ್ತಾಂಶ->ಗಣ.post301) {
      http_switch_ಗಮ್ಯ_get(ದತ್ತಾಂಶ, 301);
      ಹಿಂತಿರುಗಿ TRUE;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 302: /*ಸಿಕ್ಕಿತು*/
    /* (quote from RFC7231, section 6.4.3)
     *
     * Note: For historical reasons, a user agent MAY change the request
     * method from POST to GET for the subsequent request. If this
     * behavior is undesired, the 307 (Temporary Redirect) status code
     * can be used instead.
     *
     * ----
     *
     * Many webservers expect this, so these servers often answers to a POST
     * request with an error page. To be sure that libcurl gets the page that
     * most user agents would get, libcurl has to force GET.
     *
     * This behavior is forbidden by RFC1945 and the obsolete RFC2616, and
     * can be overridden with CURLOPT_POSTREDIR.
     */
    ಆದರೆ(HTTPREQ_IS_POST(ದತ್ತಾಂಶ) && !ದತ್ತಾಂಶ->ಗಣ.post302) {
      http_switch_ಗಮ್ಯ_get(ದತ್ತಾಂಶ, 302);
      ಹಿಂತಿರುಗಿ TRUE;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    ಆದರೆ(!HTTPREQ_IS_POST(ದತ್ತಾಂಶ) || !ದತ್ತಾಂಶ->ಗಣ.post303) {
      http_switch_ಗಮ್ಯ_get(ದತ್ತಾಂಶ, 303);
      ಹಿಂತಿರುಗಿ TRUE;
    }
    ನಿಲ್ಲಿಸು;

    /* 304 means we did a conditional request and it was "Not modified".
     * We should not get any Location: header in this response!
     */

    /* 305 (quote from RFC2616, section 10.3.6):
     * "The requested resource MUST be accessed through the proxy given
     * by the Location field. The Location field gives the URI of the
     * proxy. The recipient is expected to repeat this single request
     * via the proxy. 305 responses MUST only be generated by origin
     * servers."
     */
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

CURLcode Curl_http_follow(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  ಅಕ್ಷರ *follow_url = ಖಾಲಿ;
  CURLUcode uc;
  CURLcode ಫಲಿತಾಂಶ;
  CURLcode rewind_ಫಲಿತಾಂಶ;
  bool switch_ಗಮ್ಯ_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  ಆದರೆ(type != FOLLOW_FAKE)
    ದತ್ತಾಂಶ->state.ವಿನಂತಿಗಳು++; /* count all real follows */
  ಆದರೆ(type == FOLLOW_REDIR) {
    ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.maxredirs != -1) &&
       (ದತ್ತಾಂಶ->state.followlocation >= ದತ್ತಾಂಶ->ಗಣ.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ದತ್ತಾಂಶ->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.http_auto_referer) {
        ಫಲಿತಾಂಶ = ಗಣ_auto_referer(ದತ್ತಾಂಶ);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
    }
  }

  ಆದರೆ((type != FOLLOW_RETRY) &&
     (ದತ್ತಾಂಶ->req.httpcode != 401) && (ದತ್ತಾಂಶ->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, ಖಾಲಿ, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  ಫಲಿತಾಂಶ = follow_parse_url(ದತ್ತಾಂಶ, newurl, type, &follow_url, &uc);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  DEBUGASSERT(follow_url);

  ಆದರೆ(type == FOLLOW_FAKE)
    ಹಿಂತಿರುಗಿ follow_fake(ದತ್ತಾಂಶ, follow_url, uc, reachedmax);

  ಆದರೆ(disallowport)
    ದತ್ತಾಂಶ->state.allow_ಪೋರ್ಟ್ = FALSE;

  Curl_bufref_ಗಣ(&ದತ್ತಾಂಶ->state.url, follow_url, 0, curl_free);
  rewind_ಫಲಿತಾಂಶ = Curl_req_soft_reset(&ದತ್ತಾಂಶ->req, ದತ್ತಾಂಶ);
  infof(ದತ್ತಾಂಶ, "Issue another request to this URL: '%s'", follow_url);
  ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.http_follow_ಬಹುಲಕ == CURLFOLLOW_FIRSTONLY) &&
     !ದತ್ತಾಂಶ->state.http_ignorecustom &&
     CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST)) {
    ದತ್ತಾಂಶ->state.http_ignorecustom = TRUE;
    infof(ದತ್ತಾಂಶ, "Drop custom request method for next request");
  }

  switch_ಗಮ್ಯ_get = follow_switch_get(ದತ್ತಾಂಶ);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  ಆದರೆ(rewind_ಫಲಿತಾಂಶ && !switch_ಗಮ್ಯ_get)
    ಹಿಂತಿರುಗಿ rewind_ಫಲಿತಾಂಶ;

  Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(ದತ್ತಾಂಶ);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *headerline, /* line to check */
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ, /* header keyword _with_ colon */
                        ಸ್ಥಿರಾಂಕ size_t hlen, /* len of the keyword in bytes */
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *content, /* content string to find */
                        ಸ್ಥಿರಾಂಕ size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
  ರಚನೆ Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(ಶೀರ್ಷಿಕೆ);
  DEBUGASSERT(content);

  ಆದರೆ(!curl_strnequal(headerline, ಶೀರ್ಷಿಕೆ, hlen))
    ಹಿಂತಿರುಗಿ FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  ಆದರೆ(curlx_str_cspn(&p, &val, "\r\n"))
    ಹಿಂತಿರುಗಿ FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  ಆದರೆ(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    ಗಾಗಿ(len = curlx_strlen(&val); len >= clen;) {
      ರಚನೆ Curl_str ಮುಂದಿನ;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      ಆದರೆ(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        ಹಿಂತಿರುಗಿ TRUE; /* match! */
      /* advance to the next comma */
      ಆದರೆ(curlx_str_until(&p, &ಮುಂದಿನ, len, ',') ||
         curlx_str_single(&p, ','))
        ನಿಲ್ಲಿಸು; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      ಮಾಡು
        curlx_str_passblanks(&p);
      ಆಗ(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      ಆದರೆ((size_t)(p - o) > len)
        ನಿಲ್ಲಿಸು;
      len -= (p - o);
    }
  }
  ಹಿಂತಿರುಗಿ FALSE; /* no match */
}

ರಚನೆ cr_exp100_ctx {
  ರಚನೆ Curl_creader super;
  ರಚನೆ curltime ಆರಂಭ; /* time started waiting */
  ಎಣಿಕೆ expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

ಸ್ಥಿರ ಶೂನ್ಯ http_exp100_continue(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_exp100_ctx *ctx = reader->ctx;
  ಆದರೆ(ctx->state > EXP100_SEND_ದತ್ತಾಂಶ) {
    ctx->state = EXP100_SEND_ದತ್ತಾಂಶ;
    Curl_expire_clear(ದತ್ತಾಂಶ, EXPIRE_100_TIMEOUT);
  }
}

ಸ್ಥಿರ CURLcode cr_exp100_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_creader *reader,
                               ಅಕ್ಷರ *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ರಚನೆ cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  ಆಯ್ಕೆ(ctx->state) {
  ಸ್ಥಿತಿ EXP100_SENDING_REQUEST:
    ಆದರೆ(!Curl_req_sendbuf_ಖಾಲಿಗಳು(ದತ್ತಾಂಶ)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(ದತ್ತಾಂಶ, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(ದತ್ತಾಂಶ, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", ದತ್ತಾಂಶ->ಗಣ.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->ಆರಂಭ = *Curl_pgrs_now(ದತ್ತಾಂಶ);
    Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_100_TIMEOUT,
                    ದತ್ತಾಂಶ->ಗಣ.expect_100_timeout, &ctx->ಆರಂಭ);
    *nread = 0;
    *eos = FALSE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ EXP100_FAILED:
    DEBUGF(infof(ದತ್ತಾಂಶ, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;
  ಸ್ಥಿತಿ EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &ctx->ಆರಂಭ);
    ಆದರೆ(ms < ದತ್ತಾಂಶ->ಗಣ.expect_100_timeout) {
      DEBUGF(infof(ದತ್ತಾಂಶ, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(ದತ್ತಾಂಶ, reader);
    infof(ದತ್ತಾಂಶ, "Done waiting for 100-continue");
    FALLTHROUGH();
  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGF(infof(ದತ್ತಾಂಶ, "cr_exp100_read, pass through"));
    ಹಿಂತಿರುಗಿ Curl_creader_read(ದತ್ತಾಂಶ, reader->ಮುಂದಿನ, buf, blen, nread, eos);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_exp100_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ Curl_creader *reader, ಪೂರ್ಣಾಂಕ premature)
{
  ರಚನೆ cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_ದತ್ತಾಂಶ;
  Curl_expire_clear(ದತ್ತಾಂಶ, EXPIRE_100_TIMEOUT);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_ಒಟ್ಟು_ಉದ್ದಳತೆ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_ಮುಗಿದ,
  ಗಾತ್ರ(ರಚನೆ cr_exp100_ctx)
};

ಸ್ಥಿರ CURLcode http_exp100_ಕೂಡಿಸು_reader(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *reader = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_creader_create(&reader, ದತ್ತಾಂಶ, &cr_exp100, CURL_CR_PROTOCOL);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_creader_ಕೂಡಿಸು(ದತ್ತಾಂಶ, reader);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ರಚನೆ cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  ಆದರೆ(ಫಲಿತಾಂಶ && reader)
    Curl_creader_free(ದತ್ತಾಂಶ, reader);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ http_exp100_got100(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *r = Curl_creader_get_by_type(ದತ್ತಾಂಶ, &cr_exp100);
  ಆದರೆ(r)
    http_exp100_continue(ದತ್ತಾಂಶ, r);
}

ಸ್ಥಿರ bool http_exp100_is_ಕಾಯುವಿಕೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *r = Curl_creader_get_by_type(ದತ್ತಾಂಶ, &cr_exp100);
  ಆದರೆ(r) {
    ರಚನೆ cr_exp100_ctx *ctx = r->ctx;
    ಹಿಂತಿರುಗಿ ctx->state == EXP100_AWAITING_CONTINUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ http_exp100_send_anyway(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *r = Curl_creader_get_by_type(ದತ್ತಾಂಶ, &cr_exp100);
  ಆದರೆ(r)
    http_exp100_continue(ದತ್ತಾಂಶ, r);
}

ಸ್ಥಿರ bool http_exp100_is_selected(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *r = Curl_creader_get_by_type(ದತ್ತಾಂಶ, &cr_exp100);
  ಹಿಂತಿರುಗಿ !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ easy_pollset *ps)
{
  /* write mode */
  ಹಿಂತಿರುಗಿ Curl_pollset_ಕೂಡಿಸು_ಹೊರಹೋಗು(ದತ್ತಾಂಶ, ps, ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(CURL_REQ_WANT_RECV(ದತ್ತಾಂಶ)) {
    ಫಲಿತಾಂಶ = Curl_pollset_ಕೂಡಿಸು_ಒಳ(ದತ್ತಾಂಶ, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  ಆದರೆ(!ಫಲಿತಾಂಶ && Curl_req_want_send(ದತ್ತಾಂಶ) && !http_exp100_is_ಕಾಯುವಿಕೆ(ದತ್ತಾಂಶ)) {
    ಫಲಿತಾಂಶ = Curl_pollset_ಕೂಡಿಸು_ಹೊರಹೋಗು(ದತ್ತಾಂಶ, ps, conn->sock[FIRSTSOCKET]);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode http_write_ಶೀರ್ಷಿಕೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ, hd, hdlen);

  writetype = CLIENTWRITE_ಶೀರ್ಷಿಕೆ |
    ((ದತ್ತಾಂಶ->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, writetype, hd, hdlen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_bump_headersize(ದತ್ತಾಂಶ, hdlen, FALSE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ದತ್ತಾಂಶ->req.deductheadercount = (100 <= ದತ್ತಾಂಶ->req.httpcode &&
                                 199 >= ದತ್ತಾಂಶ->req.httpcode) ?
    ದತ್ತಾಂಶ->req.headerbytecount : 0;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        CURLcode ಸ್ಥಿತಿ_2, bool premature)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  ದತ್ತಾಂಶ->state.authhost.multipass = FALSE;
  ದತ್ತಾಂಶ->state.authproxy.multipass = FALSE;

  ಆದರೆ(curlx_dyn_len(&ದತ್ತಾಂಶ->state.headerb)) {
    (ಶೂನ್ಯ)http_write_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ, curlx_dyn_ptr(&ದತ್ತಾಂಶ->state.headerb),
                            curlx_dyn_len(&ದತ್ತಾಂಶ->state.headerb));
  }
  curlx_dyn_reset(&ದತ್ತಾಂಶ->state.headerb);

  ಆದರೆ(ಸ್ಥಿತಿ_2)
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;

  ಆದರೆ(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->ಬಿಟ್ಟುಗಳು.retry &&
     !ದತ್ತಾಂಶ->ಗಣ.connect_only &&
     (ದತ್ತಾಂಶ->req.bytecount +
      ದತ್ತಾಂಶ->req.headerbytecount -
      ದತ್ತಾಂಶ->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(ದತ್ತಾಂಶ, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    ಹಿಂತಿರುಗಿ CURLE_GOT_NOTHING;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
ಸ್ಥಿರ bool http_may_use_1_1(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  ಆದರೆ(ದತ್ತಾಂಶ->state.http_neg.rcvd_min == 10)
    ಹಿಂತಿರುಗಿ FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  ಆದರೆ(conn && conn->httpversion_ನೋಡಿದ == 10)
    ಹಿಂತಿರುಗಿ FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  ಆದರೆ(ದತ್ತಾಂಶ->state.http_neg.only_10 &&
     (!conn || conn->httpversion_ನೋಡಿದ <= 10))
    ಹಿಂತಿರುಗಿ FALSE;
  /* We are not restricted to use 1.0 only. */
  ಹಿಂತಿರುಗಿ !ದತ್ತಾಂಶ->state.http_neg.only_10;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ http_request_ಆವೃತ್ತಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ v = Curl_conn_http_ಆವೃತ್ತಿ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn);
  ಆದರೆ(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(ದತ್ತಾಂಶ) ? 11 : 10;
  }
  ಹಿಂತಿರುಗಿ v;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *get_http_string(ಪೂರ್ಣಾಂಕ httpversion)
{
  ಆಯ್ಕೆ(httpversion) {
  ಸ್ಥಿತಿ 30:
    ಹಿಂತಿರುಗಿ "3";
  ಸ್ಥಿತಿ 20:
    ಹಿಂತಿರುಗಿ "2";
  ಸ್ಥಿತಿ 11:
    ಹಿಂತಿರುಗಿ "1.1";
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ "1.0";
  }
}

CURLcode Curl_ಕೂಡಿಸು_custom_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 bool is_connect, ಪೂರ್ಣಾಂಕ httpversion,
                                 ರಚನೆ dynbuf *req)
{
  ರಚನೆ curl_slist *h[2];
  ರಚನೆ curl_slist *ಶೀರ್ಷಿಕೆಗಳು;
  ಪೂರ್ಣಾಂಕ numlists = 1; /* by default */
  ಪೂರ್ಣಾಂಕ i;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಎಣಿಕೆ Curl_proxy_use proxy;

  ಆದರೆ(is_connect)
    proxy = ಶೀರ್ಷಿಕೆ_CONNECT;
  ಇಲ್ಲದಿದ್ದರೆ
    proxy = ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy ? ಶೀರ್ಷಿಕೆ_PROXY : ಶೀರ್ಷಿಕೆ_SERVER;

  ಆಯ್ಕೆ(proxy) {
  ಸ್ಥಿತಿ ಶೀರ್ಷಿಕೆ_SERVER:
    h[0] = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ ಶೀರ್ಷಿಕೆ_PROXY:
    h[0] = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು;
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.sep_ಶೀರ್ಷಿಕೆಗಳು) {
      h[1] = ದತ್ತಾಂಶ->ಗಣ.proxyheaders;
      numlists++;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ ಶೀರ್ಷಿಕೆ_CONNECT:
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.sep_ಶೀರ್ಷಿಕೆಗಳು)
      h[0] = ದತ್ತಾಂಶ->ಗಣ.proxyheaders;
    ಇಲ್ಲದಿದ್ದರೆ
      h[0] = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ ಶೀರ್ಷಿಕೆ_CONNECT_UDP:
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.sep_ಶೀರ್ಷಿಕೆಗಳು)
      h[0] = ದತ್ತಾಂಶ->ಗಣ.proxyheaders;
    ಇಲ್ಲದಿದ್ದರೆ
      h[0] = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು;
    ನಿಲ್ಲಿಸು;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)is_connect;
  h[0] = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು;
#ಆದರೆ_ಅಂತ್ಯ

  /* loop through one or two lists */
  ಗಾಗಿ(i = 0; i < numlists; i++) {
    ಗಾಗಿ(ಶೀರ್ಷಿಕೆಗಳು = h[i]; ಶೀರ್ಷಿಕೆಗಳು; ಶೀರ್ಷಿಕೆಗಳು = ಶೀರ್ಷಿಕೆಗಳು->ಮುಂದಿನ) {
      CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
      bool blankheader = FALSE;
      ರಚನೆ Curl_str name;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಶೀರ್ಷಿಕೆಗಳು->ದತ್ತಾಂಶ;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *origp = p;
      size_t hlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      ಆದರೆ(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      ಇಲ್ಲದಿದ್ದರೆ {
        p = origp;
        ಆದರೆ(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          ರಚನೆ Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          ಆದರೆ(!curlx_strlen(&val))
            /* no content, do not send this */
            ಮುಂದುವರಿ;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          /* no colon */
          ಮುಂದುವರಿ;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      ಆದರೆ(ದತ್ತಾಂಶ->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_ಗಮ್ಯ_host(ದತ್ತಾಂಶ))
        ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(blankheader) {
        ಫಲಿತಾಂಶ = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        ಆದರೆ(!ಫಲಿತಾಂಶ)
          ಫಲಿತಾಂಶ = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "%s\r\n", origp);

      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PARSEDATE
CURLcode Curl_ಕೂಡಿಸು_timecondition(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ dynbuf *req)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ tm *tm;
  ರಚನೆ tm keeptime;
  CURLcode ಫಲಿತಾಂಶ;
  ಅಕ್ಷರ datestr[80];
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *condp;
  size_t len;

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಫಲಿತಾಂಶ = curlx_gmtime(ದತ್ತಾಂಶ->ಗಣ.timevalue, &keeptime);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    failf(ದತ್ತಾಂಶ, "Invalid TIMEVALUE");
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  tm = &keeptime;

  ಆಯ್ಕೆ(ದತ್ತಾಂಶ->ಗಣ.timecondition) {
  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGF(infof(ದತ್ತಾಂಶ, "invalid time condition"));
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಸ್ಥಿತಿ CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(Curl_checkheaders(ದತ್ತಾಂಶ, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, ಗಾತ್ರ(datestr),
                 "%s: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                 condp,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);

  ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(req, datestr);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಇಲ್ಲದಿದ್ದರೆ
/* disabled */
CURLcode Curl_ಕೂಡಿಸು_timecondition(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ dynbuf *req)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)req;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ Curl_http_ವಿಧಾನ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ವಿಧಾನ, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)ದತ್ತಾಂಶ->state.httpreq;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *request;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS
  ಆದರೆ(ದತ್ತಾಂಶ->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ((ದತ್ತಾಂಶ->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     ದತ್ತಾಂಶ->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  ಆದರೆ(!ದತ್ತಾಂಶ->state.http_ignorecustom &&
     CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(ದತ್ತಾಂಶ->req.no_body)
      request = "HEAD";
    ಇಲ್ಲದಿದ್ದರೆ {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_ತಲೆ));
      ಆಯ್ಕೆ(httpreq) {
      ಸ್ಥಿತಿ HTTPREQ_POST:
      ಸ್ಥಿತಿ HTTPREQ_POST_FORM:
      ಸ್ಥಿತಿ HTTPREQ_POST_MIME:
        request = "POST";
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ HTTPREQ_PUT:
        request = "PUT";
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ: /* this should never happen */
      ಸ್ಥಿತಿ HTTPREQ_GET:
        request = "GET";
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ HTTPREQ_ತಲೆ:
        request = "HEAD";
        ನಿಲ್ಲಿಸು;
      }
    }
  }
  *ವಿಧಾನ = request;
  *reqp = httpreq;
}

ಸ್ಥಿರ CURLcode http_ಗಣ_aptr_host(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = ಖಾಲಿ;

  curlx_safefree(ದತ್ತಾಂಶ->state.http_host);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  curlx_safefree(ದತ್ತಾಂಶ->req.cookiehost);
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(Curl_peer_equal(ದತ್ತಾಂಶ->state.initial_origin, ದತ್ತಾಂಶ->state.origin))
    ptr = Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Host"));

  ಆದರೆ(ptr) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    ಅಕ್ಷರ *cookiehost;
    CURLcode ಫಲಿತಾಂಶ = copy_custom_ಮೌಲ್ಯ(ptr, &cookiehost);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಆದರೆ(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    ಇಲ್ಲದಿದ್ದರೆ {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      ಆದರೆ(*cookiehost == '[') {
        ಅಕ್ಷರ *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        ಆದರೆ(closingbracket)
          *closingbracket = 0;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಪೂರ್ಣಾಂಕ startsearch = 0;
        ಅಕ್ಷರ *colon = strchr(cookiehost + startsearch, ':');
        ಆದರೆ(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      ದತ್ತಾಂಶ->req.cookiehost = cookiehost;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(!curl_strequal("Host:", ptr)) {
      ದತ್ತಾಂಶ->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      ಆದರೆ(!ದತ್ತಾಂಶ->state.http_host)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)ದತ್ತಾಂಶ->state.origin->ipv6;
    ರಚನೆ dynbuf tmp;
    size_t hlen;
    CURLcode ಫಲಿತಾಂಶ;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    ಆದರೆ(!ಫಲಿತಾಂಶ && ipv6)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&tmp, STRCONST("["));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&tmp, ದತ್ತಾಂಶ->state.origin->hostname);
    ಆದರೆ(!ಫಲಿತಾಂಶ && ipv6)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&tmp, STRCONST("]"));
    ಆದರೆ(!ಫಲಿತಾಂಶ &&
       ((ದತ್ತಾಂಶ->state.origin->ಪೋರ್ಟ್ != ದತ್ತಾಂಶ->state.origin->scheme->defport) ||
       (ದತ್ತಾಂಶ->state.origin->scheme->family != conn->scheme->family))) {
      ಫಲಿತಾಂಶ = curlx_dyn_addf(&tmp, ":%u", ದತ್ತಾಂಶ->state.origin->ಪೋರ್ಟ್);
    }

    ದತ್ತಾಂಶ->state.http_host = ಫಲಿತಾಂಶ ? ಖಾಲಿ : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
ಸ್ಥಿರ CURLcode http_ಗುರಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ dynbuf *r)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query = ದತ್ತಾಂಶ->state.ಮೇಲೆ.query;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಗುರಿ)) {
    ಮಾರ್ಗ = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಗುರಿ);
    query = ಖಾಲಿ;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    ಅಕ್ಷರ *url;
    CURLU *h = curl_url_dup(ದತ್ತಾಂಶ->state.uh);
    ಆದರೆ(!h)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    ಆದರೆ(!ದತ್ತಾಂಶ->state.origin->ipv6 &&
       (ದತ್ತಾಂಶ->state.origin->user_hostname != ದತ್ತಾಂಶ->state.origin->hostname)) {
      uc = curl_url_ಗಣ(h, CURLUPART_HOST, ದತ್ತಾಂಶ->state.origin->hostname, 0);
      ಆದರೆ(uc) {
        curl_url_cleanup(h);
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
    }
    uc = curl_url_ಗಣ(h, CURLUPART_FRAGMENT, ಖಾಲಿ, 0);
    ಆದರೆ(uc) {
      curl_url_cleanup(h);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    ಆದರೆ(ದತ್ತಾಂಶ->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_ಗಣ(h, CURLUPART_USER, ಖಾಲಿ, 0);
      ಆದರೆ(uc) {
        curl_url_cleanup(h);
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      uc = curl_url_ಗಣ(h, CURLUPART_PASSWORD, ಖಾಲಿ, 0);
      ಆದರೆ(uc) {
        curl_url_cleanup(h);
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.creds && (ದತ್ತಾಂಶ->state.creds->ಮೂಲ != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_ಗಣ(h, CURLUPART_USER,
                        ದತ್ತಾಂಶ->state.creds->user, CURLU_URLENCODE);
      ಆದರೆ(!uc)
        uc = curl_url_ಗಣ(h, CURLUPART_PASSWORD,
                          ದತ್ತಾಂಶ->state.creds->passwd, CURLU_URLENCODE);
      ಆದರೆ(uc) {
        curl_url_cleanup(h);
        ಹಿಂತಿರುಗಿ Curl_uc_ಗಮ್ಯ_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_ಪೋರ್ಟ್);
    ಆದರೆ(uc) {
      curl_url_cleanup(h);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    curl_url_cleanup(h);

    /* target or URL */
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(r, CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಗುರಿ) ?
      CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಗುರಿ) : url);
    curlx_free(url);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಆದರೆ((ದತ್ತಾಂಶ->state.origin->scheme == &Curl_scheme_ftp) &&
       ದತ್ತಾಂಶ->ಗಣ.proxy_transfer_ಬಹುಲಕ) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ);
      bool type_present = FALSE;
      ಆದರೆ((len >= 7) && !memcmp(&ಮಾರ್ಗ[len - 7], ";type=", 6)) {
        ಆಯ್ಕೆ(Curl_raw_toupper(ಮಾರ್ಗ[len - 1])) {
        ಸ್ಥಿತಿ 'A':
        ಸ್ಥಿತಿ 'D':
        ಸ್ಥಿತಿ 'I':
          type_present = TRUE;
          ನಿಲ್ಲಿಸು;
        }
      }
      ಆದರೆ(!type_present) {
        ಫಲಿತಾಂಶ = curlx_dyn_addf(r, ";type=%c",
                                ದತ್ತಾಂಶ->state.prefer_ascii ? 'a' : 'i');
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
    }
  }

  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  {
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(r, ಮಾರ್ಗ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಆದರೆ(query)
      ಫಲಿತಾಂಶ = curlx_dyn_addf(r, "?%s", query);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
ಸ್ಥಿರ CURLcode ಗಣ_post_reader(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, Curl_HttpReq httpreq)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆಯ್ಕೆ(httpreq) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MIME
  ಸ್ಥಿತಿ HTTPREQ_POST_MIME:
    ದತ್ತಾಂಶ->state.mimepost = ದತ್ತಾಂಶ->ಗಣ.mimepostp;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FORM_API
  ಸ್ಥಿತಿ HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    ಆದರೆ(!ದತ್ತಾಂಶ->state.formp) {
      ದತ್ತಾಂಶ->state.formp = curlx_calloc(1, ಗಾತ್ರ(curl_mimepart));
      ಆದರೆ(!ದತ್ತಾಂಶ->state.formp)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      Curl_mime_cleanpart(ದತ್ತಾಂಶ->state.formp);
      ಫಲಿತಾಂಶ = Curl_getformdata(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.formp, ದತ್ತಾಂಶ->ಗಣ.httppost,
                                ದತ್ತಾಂಶ->state.fread_func);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        curlx_safefree(ದತ್ತಾಂಶ->state.formp);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ದತ್ತಾಂಶ->state.mimepost = ದತ್ತಾಂಶ->state.formp;
    }
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ದತ್ತಾಂಶ->state.mimepost = ಖಾಲಿ;
    ನಿಲ್ಲಿಸು;
  }

  ಆಯ್ಕೆ(httpreq) {
  ಸ್ಥಿತಿ HTTPREQ_POST_FORM:
  ಸ್ಥಿತಿ HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MIME
    ಆದರೆ(ದತ್ತಾಂಶ->state.mimepost) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cthdr = Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Content-Type"));

      /* Read and seek body only. */
      ದತ್ತಾಂಶ->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      ಆದರೆ(cthdr)
        ಗಾಗಿ(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.mimepost->ಬಗೆ == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ->state.mimepost, ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು, 0);
      ಫಲಿತಾಂಶ = Curl_mime_prepare_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.mimepost, cthdr,
                                         ಖಾಲಿ, MIMESTRATEGY_FORM);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      curl_mime_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ->state.mimepost, ಖಾಲಿ, 0);
      ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_mime(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.mimepost);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
      ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);
    }
    ದತ್ತಾಂಶ->state.infilesize = Curl_creader_ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);
  }
  /* never reached */
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode ಗಣ_reader(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, Curl_HttpReq httpreq)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curl_off_t postsize = ದತ್ತಾಂಶ->state.infilesize;

  DEBUGASSERT(ದತ್ತಾಂಶ->conn);

  ಆದರೆ(ದತ್ತಾಂಶ->req.authneg) {
    ಹಿಂತಿರುಗಿ Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);
  }

  ಆಯ್ಕೆ(httpreq) {
  ಸ್ಥಿತಿ HTTPREQ_PUT: /* Let's PUT the data to the server! */
    ಹಿಂತಿರುಗಿ postsize ? Curl_creader_ಗಣ_fread(ದತ್ತಾಂಶ, postsize) :
      Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);

#ಆದರೆ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ಸ್ಥಿತಿ HTTPREQ_POST_FORM:
  ಸ್ಥಿತಿ HTTPREQ_POST_MIME:
    ಹಿಂತಿರುಗಿ ಗಣ_post_reader(ದತ್ತಾಂಶ, httpreq);
#ಆದರೆ_ಅಂತ್ಯ

  ಸ್ಥಿತಿ HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    ಆದರೆ(!postsize) {
      ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      ಆದರೆ(plen == SIZE_MAX)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(plen)
        ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_buf(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->ಗಣ.postfields, plen);
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      ಅಕ್ಷರ *ptr = Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Transfer-Encoding"));
      ಆದರೆ(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_fread(ದತ್ತಾಂಶ, chunked ? -1 : postsize);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಪೂರ್ವನಿಯೋಜಿತ:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    ದತ್ತಾಂಶ->state.infilesize = 0;
    ಹಿಂತಿರುಗಿ Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);
  }
  /* not reached */
}

ಸ್ಥಿರ CURLcode http_resume(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, Curl_HttpReq httpreq)
{
  ಆದರೆ((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     ದತ್ತಾಂಶ->state.resume_from) {
    failf(ದತ್ತಾಂಶ, "HTTP upload cannot be resumed");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode http_req_ಗಣ_TE(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ dynbuf *req,
                                ಪೂರ್ಣಾಂಕ httpversion)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr;

  ptr = Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Transfer-Encoding"));
  ಆದರೆ(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    ದತ್ತಾಂಶ->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    ಆದರೆ(ದತ್ತಾಂಶ->req.upload_chunky && (httpversion >= 20)) {
      infof(ದತ್ತಾಂಶ, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      ದತ್ತಾಂಶ->req.upload_chunky = FALSE;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    curl_off_t req_clen = Curl_creader_ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ);

    ಆದರೆ(req_clen < 0) {
      /* indeterminate request content length */
      ಆದರೆ(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        ದತ್ತಾಂಶ->req.upload_chunky = (httpversion < 20);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        failf(ದತ್ತಾಂಶ, "Chunky upload is not supported by HTTP 1.0");
        ಹಿಂತಿರುಗಿ CURLE_UPLOAD_FAILED;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* else, no chunky upload */
      ದತ್ತಾಂಶ->req.upload_chunky = FALSE;
    }

    ಆದರೆ(ದತ್ತಾಂಶ->req.upload_chunky)
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(req, "Transfer-Encoding: chunked\r\n");
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode addexpect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ dynbuf *r,
                          ಪೂರ್ಣಾಂಕ httpversion, bool *announced_exp100)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಅಕ್ಷರ *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  ಆದರೆ(ದತ್ತಾಂಶ->req.upgr101 != UPGR101_NONE)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Expect"));
  ಆದರೆ(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ದತ್ತಾಂಶ->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ);
    ಆದರೆ(client_len > EXPECT_100_ಮಿತಿ || client_len < 0) {
      ಫಲಿತಾಂಶ = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      *announced_exp100 = TRUE;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode http_ಕೂಡಿಸು_content_hds(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ dynbuf *r,
                                     ಪೂರ್ಣಾಂಕ httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  ಆದರೆ(ದತ್ತಾಂಶ->req.upload_chunky) {
    ಫಲಿತಾಂಶ = Curl_httpchunk_ಕೂಡಿಸು_reader(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ);
  ಆಯ್ಕೆ(httpreq) {
  ಸ್ಥಿತಿ HTTPREQ_PUT:
  ಸ್ಥಿತಿ HTTPREQ_POST:
#ಆದರೆ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ಸ್ಥಿತಿ HTTPREQ_POST_FORM:
  ಸ್ಥಿತಿ HTTPREQ_POST_MIME:
#ಆದರೆ_ಅಂತ್ಯ
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    ಆದರೆ(req_clen >= 0 && !ದತ್ತಾಂಶ->req.upload_chunky &&
       (ದತ್ತಾಂಶ->req.authneg ||
        !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      ಫಲಿತಾಂಶ = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    ಆದರೆ(ದತ್ತಾಂಶ->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      ರಚನೆ curl_slist *hdr;

      ಗಾಗಿ(hdr = ದತ್ತಾಂಶ->state.mimepost->curlheaders; hdr; hdr = hdr->ಮುಂದಿನ) {
        ಫಲಿತಾಂಶ = curlx_dyn_addf(r, "%s\r\n", hdr->ದತ್ತಾಂಶ);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
      }
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Content-Type"))) {
      ಫಲಿತಾಂಶ = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
    ಫಲಿತಾಂಶ = addexpect(ದತ್ತಾಂಶ, r, httpversion, &announced_exp100);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }

  Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, req_clen);
  ಆದರೆ(announced_exp100)
    ಫಲಿತಾಂಶ = http_exp100_ಕೂಡಿಸು_reader(ದತ್ತಾಂಶ);

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES

ಸ್ಥಿರ CURLcode http_cookies(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ dynbuf *r)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *addcookies = ಖಾಲಿ;
  bool linecap = FALSE;
  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_COOKIE) &&
     !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Cookie")) &&
     Curl_auth_allowed_ಗಮ್ಯ_host(ದತ್ತಾಂಶ))
    addcookies = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_COOKIE);

  ಆದರೆ(ದತ್ತಾಂಶ->cookies || addcookies) {
    ರಚನೆ Curl_llist list;
    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;

    ಆದರೆ(ದತ್ತಾಂಶ->cookies && ದತ್ತಾಂಶ->state.cookie_engine) {
      bool okay;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host = ದತ್ತಾಂಶ->req.cookiehost ?
        ದತ್ತಾಂಶ->req.cookiehost : ದತ್ತಾಂಶ->state.origin->hostname;
      Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE, CURL_ಬೀಗ_ACCESS_SINGLE);
      ಫಲಿತಾಂಶ = Curl_cookie_getlist(ದತ್ತಾಂಶ, &okay, host, &list);
      ಆದರೆ(!ಫಲಿತಾಂಶ && okay) {
        ರಚನೆ Curl_llist_ಗಂಟು *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        ಗಾಗಿ(n = Curl_llist_ತಲೆ(&list); n; n = Curl_ಗಂಟು_ಮುಂದಿನ(n)) {
          ರಚನೆ Cookie *co = Curl_ಗಂಟು_elem(n);
          ಆದರೆ(co->ಮೌಲ್ಯ) {
            size_t ಕೂಡಿಸು;
            ಆದರೆ(!ಎಣಿಕೆ_2) {
              ಫಲಿತಾಂಶ = curlx_dyn_addn(r, STRCONST("Cookie: "));
              ಆದರೆ(ಫಲಿತಾಂಶ)
                ನಿಲ್ಲಿಸು;
            }
            ಕೂಡಿಸು = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(co->name) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(co->ಮೌಲ್ಯ) + 1;
            ಆದರೆ(clen + ಕೂಡಿಸು >= MAX_COOKIE_ಶೀರ್ಷಿಕೆ_LEN) {
              infof(ದತ್ತಾಂಶ, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              ನಿಲ್ಲಿಸು;
            }
            ಫಲಿತಾಂಶ = curlx_dyn_addf(r, "%s%s=%s", ಎಣಿಕೆ_2 ? "; " : "",
                                    co->name, co->ಮೌಲ್ಯ);
            ಆದರೆ(ಫಲಿತಾಂಶ)
              ನಿಲ್ಲಿಸು;
            clen += ಕೂಡಿಸು + (ಎಣಿಕೆ_2 ? 2 : 0);
            ಎಣಿಕೆ_2++;
          }
        }
        Curl_llist_destroy(&list, ಖಾಲಿ);
      }
      Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE);
    }
    ಆದರೆ(addcookies && !ಫಲಿತಾಂಶ && !linecap) {
      ಆದರೆ(!ಎಣಿಕೆ_2)
        ಫಲಿತಾಂಶ = curlx_dyn_addn(r, STRCONST("Cookie: "));
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        ಫಲಿತಾಂಶ = curlx_dyn_addf(r, "%s%s", ಎಣಿಕೆ_2 ? "; " : "", addcookies);
        ಎಣಿಕೆ_2++;
      }
    }
    ಆದರೆ(ಎಣಿಕೆ_2 && !ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(r, STRCONST("\r\n"));

    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು http_cookies(a, b) CURLE_OK
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode http_range(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           Curl_HttpReq httpreq)
{
  ಆದರೆ(ದತ್ತಾಂಶ->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    ಆದರೆ(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_ತಲೆ)) &&
       !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(ದತ್ತಾಂಶ->state.rangeline);
      ದತ್ತಾಂಶ->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 ದತ್ತಾಂಶ->state.range);
      ಆದರೆ(!ದತ್ತಾಂಶ->state.rangeline)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(ದತ್ತಾಂಶ->state.rangeline);

      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ಗಣ_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        ದತ್ತಾಂಶ->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t ಒಟ್ಟು_len = ದತ್ತಾಂಶ->req.authneg ?
                               ದತ್ತಾಂಶ->state.infilesize :
                               (ದತ್ತಾಂಶ->state.resume_from + req_clen);
        ದತ್ತಾಂಶ->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        ದತ್ತಾಂಶ->state.range, ಒಟ್ಟು_len - 1, ಒಟ್ಟು_len);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* Range was selected and then we pass the incoming range and append
           total size */
        ದತ್ತಾಂಶ->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        ದತ್ತಾಂಶ->state.range, req_clen);
      }
      ಆದರೆ(!ದತ್ತಾಂಶ->state.rangeline)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode http_firstwrite(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;

  ಆದರೆ(ದತ್ತಾಂಶ->req.newurl) {
    ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(ದತ್ತಾಂಶ);
      k->ಮುಗಿದ = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(ದತ್ತಾಂಶ, "Ignoring the response-body");
  }
  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from && !k->content_range &&
     (ದತ್ತಾಂಶ->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    ಆದರೆ(k->ಗಾತ್ರ_2 == ದತ್ತಾಂಶ->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(ದತ್ತಾಂಶ, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(ದತ್ತಾಂಶ);
      k->ಮುಗಿದ = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(ದತ್ತಾಂಶ, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    ಹಿಂತಿರುಗಿ CURLE_RANGE_ERROR;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.timecondition && !ದತ್ತಾಂಶ->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(ದತ್ತಾಂಶ, k->timeofdoc)) {
    k->ಮುಗಿದ = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    ದತ್ತಾಂಶ->info.httpcode = 304;
    infof(ದತ್ತಾಂಶ, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  } /* we have a time condition */

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode http_check_new_conn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *info_ಆವೃತ್ತಿ = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alpn;
  CURLcode ಫಲಿತಾಂಶ;

  alpn = Curl_conn_get_alpn_negotiated(ದತ್ತಾಂಶ, conn);
  ಆದರೆ(alpn && !strcmp("h3", alpn)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    ಆದರೆ(!conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy)
#ಆದರೆ_ಅಂತ್ಯ
      DEBUGASSERT(Curl_conn_http_ಆವೃತ್ತಿ(ದತ್ತಾಂಶ, conn) == 30);
    info_ಆವೃತ್ತಿ = "HTTP/3";
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(alpn && !strcmp("h2", alpn)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    ಆದರೆ((Curl_conn_http_ಆವೃತ್ತಿ(ದತ್ತಾಂಶ, conn) != 20) &&
       conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy) {
      ಫಲಿತಾಂಶ = Curl_http2_switch(ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    DEBUGASSERT(Curl_conn_http_ಆವೃತ್ತಿ(ದತ್ತಾಂಶ, conn) == 20);
    info_ಆವೃತ್ತಿ = "HTTP/2";
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Check if user wants to use HTTP/2 with clear TCP */
    ಆದರೆ(Curl_http2_may_switch(ದತ್ತಾಂಶ)) {
      DEBUGF(infof(ದತ್ತಾಂಶ, "HTTP/2 over clean TCP"));
      ಫಲಿತಾಂಶ = Curl_http2_switch(ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      info_ಆವೃತ್ತಿ = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_ನೋಡಿದ = 20;
      Curl_conn_ಗಣ_multiplex(conn);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      info_ಆವೃತ್ತಿ = "HTTP/1.x";
  }

  ಆದರೆ(info_ಆವೃತ್ತಿ)
    infof(ದತ್ತಾಂಶ, "using %s", info_ಆವೃತ್ತಿ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode http_ಕೂಡಿಸು_connection_hd(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ dynbuf *req)
{
  ರಚನೆ curl_slist *ತಲೆ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = "Connection: ";
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  ಗಾಗಿ(ತಲೆ = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು; ತಲೆ; ತಲೆ = ತಲೆ->ಮುಂದಿನ) {
    ಆದರೆ(curl_strnequal(ತಲೆ->ದತ್ತಾಂಶ, "Connection", 10) &&
       Curl_headersep(ತಲೆ->ದತ್ತಾಂಶ[10]) &&
       !http_ಶೀರ್ಷಿಕೆ_is_ಖಾಲಿಗಳು(ತಲೆ->ದತ್ತಾಂಶ)) {
      ಅಕ್ಷರ *ಮೌಲ್ಯ;
      ಫಲಿತಾಂಶ = copy_custom_ಮೌಲ್ಯ(ತಲೆ->ದತ್ತಾಂಶ, &ಮೌಲ್ಯ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "%s%s", sep, ಮೌಲ್ಯ);
      sep = ", ";
      curlx_free(ಮೌಲ್ಯ);
      ನಿಲ್ಲಿಸು; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  ಆದರೆ(!ಫಲಿತಾಂಶ && ದತ್ತಾಂಶ->state.http_hd_te) {
    ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && ದತ್ತಾಂಶ->state.http_hd_upgrade) {
    ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && ದತ್ತಾಂಶ->state.http_hd_h2_settings) {
    ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && (rlen < curlx_dyn_len(req)))
    ಫಲಿತಾಂಶ = curlx_dyn_addn(req, STRCONST("\r\n"));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  ಗಾಗಿ(ತಲೆ = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು; ತಲೆ; ತಲೆ = ತಲೆ->ಮುಂದಿನ) {
    ಆದರೆ(curl_strnequal(ತಲೆ->ದತ್ತಾಂಶ, "Connection", 10) &&
       Curl_headersep(ತಲೆ->ದತ್ತಾಂಶ[10]) &&
       !http_ಶೀರ್ಷಿಕೆ_is_ಖಾಲಿಗಳು(ತಲೆ->ದತ್ತಾಂಶ)) {
      ಆದರೆ(skip) {
        skip = FALSE;
        ಮುಂದುವರಿ;
      }
      ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "%s\r\n", ತಲೆ->ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Header identifier in order we send them by default */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  H1_HD_REQUEST,
  H1_HD_HOST,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  H1_HD_PROXY_AUTH,
#ಆದರೆ_ಅಂತ್ಯ
  H1_HD_AUTH,
  H1_HD_RANGE,
  H1_HD_USER_AGENT,
  H1_HD_ACCEPT,
  H1_HD_TE,
  H1_HD_ACCEPT_ENCODING,
  H1_HD_REFERER,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  H1_HD_PROXY_CONNECTION,
#ಆದರೆ_ಅಂತ್ಯ
  H1_HD_TRANSFER_ENCODING,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ALTSVC
  H1_HD_ALT_ಬಳಸಿದ,
#ಆದರೆ_ಅಂತ್ಯ
  H1_HD_UPGRADE,
  H1_HD_COOKIES,
  H1_HD_CONDITIONALS,
  H1_HD_CUSTOM,
  H1_HD_CONTENT,
  H1_HD_CONNECTION,
  H1_HD_ಕೊನೆಯ  /* the last, empty header line */
} http_hd_t;

ಸ್ಥಿರ CURLcode http_ಕೂಡಿಸು_hd(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ dynbuf *req,
                            http_hd_t id,
                            ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ httpversion,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ,
                            Curl_HttpReq httpreq)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
#ಆದರೆ !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
#ಆದರೆ_ಅಂತ್ಯ
  ಆಯ್ಕೆ(id) {
  ಸ್ಥಿತಿ H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "%s ", ವಿಧಾನ);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = http_ಗುರಿ(ದತ್ತಾಂಶ, req);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_HD_HOST:
    ಆದರೆ(ದತ್ತಾಂಶ->state.http_host) {
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(req, ದತ್ತಾಂಶ->state.http_host);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಸ್ಥಿತಿ H1_HD_PROXY_AUTH:
    ಆದರೆ(ದತ್ತಾಂಶ->req.hd_proxy_auth)
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(req, ದತ್ತಾಂಶ->req.hd_proxy_auth);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

  ಸ್ಥಿತಿ H1_HD_AUTH:
    ಆದರೆ(ದತ್ತಾಂಶ->req.hd_auth)
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(req, ದತ್ತಾಂಶ->req.hd_auth);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_HD_RANGE:
    ಆದರೆ(ದತ್ತಾಂಶ->state.use_range && ದತ್ತಾಂಶ->state.rangeline)
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(req, ದತ್ತಾಂಶ->state.rangeline);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_HD_USER_AGENT: {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ua = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_USERAGENT);
    ಆದರೆ(ua && *ua && !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("User-Agent")))
      ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    ನಿಲ್ಲಿಸು;
  }

  ಸ್ಥಿತಿ H1_HD_ACCEPT:
    ಆದರೆ(!Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Accept")))
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(req, "Accept: */*\r\n");
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_HD_TE:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LIBZ
    ಆದರೆ(!Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("TE")) &&
       ದತ್ತಾಂಶ->ಗಣ.http_transfer_encoding) {
      ದತ್ತಾಂಶ->state.http_hd_te = TRUE;
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(req, "TE: gzip\r\n");
    }
#ಆದರೆ_ಅಂತ್ಯ
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_HD_ACCEPT_ENCODING: {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *enc = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ENCODING);
    ಆದರೆ(enc && !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Accept-Encoding")))
      ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    ನಿಲ್ಲಿಸು;
  }

  ಸ್ಥಿತಿ H1_HD_REFERER:
    ಆದರೆ(Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.referer) &&
       !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Referer")))
      ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.referer));
    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಸ್ಥಿತಿ H1_HD_PROXY_CONNECTION:
    ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy &&
       !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, STRCONST("Proxy-Connection")))
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(req, "Proxy-Connection: Keep-Alive\r\n");
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

  ಸ್ಥಿತಿ H1_HD_TRANSFER_ENCODING:
    ಫಲಿತಾಂಶ = http_req_ಗಣ_TE(ದತ್ತಾಂಶ, req, httpversion);
    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ALTSVC
  ಸ್ಥಿತಿ H1_HD_ALT_ಬಳಸಿದ:
    ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.altused && conn->ಮೂಲಕ_peer &&
       !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Alt-Used")))
      ಫಲಿತಾಂಶ = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->ಮೂಲಕ_peer->hostname, conn->ಮೂಲಕ_peer->ಪೋರ್ಟ್);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

  ಸ್ಥಿತಿ H1_HD_UPGRADE:
    ಆದರೆ(!Curl_conn_is_ssl(ದತ್ತಾಂಶ->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (ದತ್ತಾಂಶ->state.http_neg.wanted & CURL_HTTP_V2x) &&
       ದತ್ತಾಂಶ->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      ಫಲಿತಾಂಶ = Curl_http2_request_upgrade(req, ದತ್ತಾಂಶ);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS
    ಆದರೆ(!ಫಲಿತಾಂಶ && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      ಫಲಿತಾಂಶ = Curl_ws_request(ದತ್ತಾಂಶ, req);
#ಆದರೆ_ಅಂತ್ಯ
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_HD_COOKIES:
    ಫಲಿತಾಂಶ = http_cookies(ದತ್ತಾಂಶ, req);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_HD_CONDITIONALS:
    ಫಲಿತಾಂಶ = Curl_ಕೂಡಿಸು_timecondition(ದತ್ತಾಂಶ, req);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_HD_CUSTOM:
    ಫಲಿತಾಂಶ = Curl_ಕೂಡಿಸು_custom_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, FALSE, httpversion, req);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_HD_CONTENT:
    ಫಲಿತಾಂಶ = http_ಕೂಡಿಸು_content_hds(ದತ್ತಾಂಶ, req, httpversion, httpreq);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_HD_CONNECTION: {
    ಫಲಿತಾಂಶ = http_ಕೂಡಿಸು_connection_hd(ದತ್ತಾಂಶ, req);
    ನಿಲ್ಲಿಸು;
  }

  ಸ್ಥಿತಿ H1_HD_ಕೊನೆಯ:
    ಫಲಿತಾಂಶ = curlx_dyn_addn(req, STRCONST("\r\n"));
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  Curl_HttpReq httpreq;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ;
  ರಚನೆ dynbuf req;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ httpversion;
  size_t hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *ಮುಗಿದ = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&ದತ್ತಾಂಶ->state.headerb);
  ದತ್ತಾಂಶ->state.maybe_folded = FALSE;

  ಆದರೆ(!ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.reuse) {
    ಫಲಿತಾಂಶ = http_check_new_conn(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  ಫಲಿತಾಂಶ = Curl_ಶೀರ್ಷಿಕೆಗಳು_init(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ದತ್ತಾಂಶ->state.http_hd_te = FALSE;
  ದತ್ತಾಂಶ->state.http_hd_upgrade = FALSE;
  ದತ್ತಾಂಶ->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_ವಿಧಾನ(ದತ್ತಾಂಶ, &ವಿಧಾನ, &httpreq);

  /* select host to send */
  ಫಲಿತಾಂಶ = http_ಗಣ_aptr_host(ದತ್ತಾಂಶ);
  /* setup the authentication headers, how that method and host are known */
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_http_output_auth(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, ವಿಧಾನ, httpreq,
                                   ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ,
                                   ದತ್ತಾಂಶ->state.ಮೇಲೆ.query, FALSE);
  /* Setup input reader, resume information and ranges */
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = ಗಣ_reader(ದತ್ತಾಂಶ, httpreq);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = http_resume(ದತ್ತಾಂಶ, httpreq);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = http_range(ದತ್ತಾಂಶ, httpreq);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  httpversion = http_request_ಆವೃತ್ತಿ(ದತ್ತಾಂಶ);
  /* Add request line and all headers to `req` */
  ಗಾಗಿ(hd_id = 0; hd_id <= H1_HD_ಕೊನೆಯ; ++hd_id) {
    ಫಲಿತಾಂಶ = http_ಕೂಡಿಸು_hd(ದತ್ತಾಂಶ, &req, (http_hd_t)hd_id,
                         httpversion, ವಿಧಾನ, httpreq);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_ಸಿದ್ಧತೆ_sendrecv(ದತ್ತಾಂಶ, FIRSTSOCKET, -1);
  ಫಲಿತಾಂಶ = Curl_req_send(ದತ್ತಾಂಶ, &req, httpversion);

  ಆದರೆ((httpversion >= 20) && ದತ್ತಾಂಶ->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    ದತ್ತಾಂಶ->req.upload_chunky = FALSE;

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_TOO_LARGE)
    failf(ದತ್ತಾಂಶ, "HTTP request too large");

  curlx_dyn_free(&req);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  ಸ್ಥಿತಿ_UNKNOWN, /* not enough data to tell yet */
  ಸ್ಥಿತಿ_ಮುಗಿದ, /* a status line was read */
  ಸ್ಥಿತಿ_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
ಸ್ಥಿರ bool checkprefixmax(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್, size_t len)
{
  size_t ch = CURLMIN(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪೂರ್ವಪ್ರತ್ಯಯ), len);
  ಹಿಂತಿರುಗಿ curl_strnequal(ಪೂರ್ವಪ್ರತ್ಯಯ, ಬಫರ್, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
ಸ್ಥಿರ statusline checkhttpprefix(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len)
{
  ರಚನೆ curl_slist *ತಲೆ = ದತ್ತಾಂಶ->ಗಣ.http200aliases;
  statusline rc = ಸ್ಥಿತಿ_BAD;
  statusline onmatch = len >= 5 ? ಸ್ಥಿತಿ_ಮುಗಿದ : ಸ್ಥಿತಿ_UNKNOWN;

  ಆಗ(ತಲೆ) {
    ಆದರೆ(checkprefixmax(ತಲೆ->ದತ್ತಾಂಶ, s, len)) {
      rc = onmatch;
      ನಿಲ್ಲಿಸು;
    }
    ತಲೆ = ತಲೆ->ಮುಂದಿನ;
  }

  ಆದರೆ((rc != ಸ್ಥಿತಿ_ಮುಗಿದ) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  ಹಿಂತಿರುಗಿ rc;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
ಸ್ಥಿರ statusline checkrtspprefix(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len)
{
  statusline ಸ್ಥಿತಿ_2 = ಸ್ಥಿತಿ_BAD;
  statusline onmatch = len >= 5 ? ಸ್ಥಿತಿ_ಮುಗಿದ : ಸ್ಥಿತಿ_UNKNOWN;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(checkprefixmax("RTSP/", s, len))
    ಸ್ಥಿತಿ_2 = onmatch;

  ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_RTSP */

ಸ್ಥಿರ statusline checkprotoprefix(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ connectdata *conn,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  ಆದರೆ(conn->scheme->protocol & CURLPROTO_RTSP)
    ಹಿಂತಿರುಗಿ checkrtspprefix(ದತ್ತಾಂಶ, s, len);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)conn;
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_RTSP */

  ಹಿಂತಿರುಗಿ checkhttpprefix(ದತ್ತಾಂಶ, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#ವ್ಯಾಖ್ಯಾನಿಸು HD_IS(hd, hdlen, n) \
  (((hdlen) >= (ಗಾತ್ರ(n) - 1)) && curl_strnequal(n, hd, ಗಾತ್ರ(n) - 1))

#ವ್ಯಾಖ್ಯಾನಿಸು HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (ಗಾತ್ರ(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, ಗಾತ್ರ(n) - 1)) ? ((hd) + (ಗಾತ್ರ(n) - 1)) : ಖಾಲಿ)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#ವ್ಯಾಖ್ಯಾನಿಸು HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((ಗಾತ್ರ(n) - 1) + (ಗಾತ್ರ(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
ಸ್ಥಿರ CURLcode http_ಶೀರ್ಷಿಕೆ_a(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ALTSVC
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v;
  v = (ದತ್ತಾಂಶ->asi &&
       (Curl_xfer_is_secure(ದತ್ತಾಂಶ) ||
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#ಇಲ್ಲದಿದ್ದರೆ
        0
#ಆದರೆ_ಅಂತ್ಯ
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : ಖಾಲಿ;
  ಆದರೆ(v) {
    /* the ALPN of the current request */
    ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
    ಎಣಿಕೆ alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    ಹಿಂತಿರುಗಿ Curl_altsvc_parse(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->asi, v, ದತ್ತಾಂಶ->state.origin, id);
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)hd;
  (ಶೂನ್ಯ)hdlen;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
ಸ್ಥಿರ CURLcode http_ಶೀರ್ಷಿಕೆ_c(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !ದತ್ತಾಂಶ->ಗಣ.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : ಖಾಲಿ;
  ಆದರೆ(v) {
    ಮಾಡು {
      curl_off_t contentlength;
      ಪೂರ್ಣಾಂಕ offt = curlx_str_numblanks(&v, &contentlength);

      ಆದರೆ(offt == STRE_ಅತಿಪ್ರವಾಹ) {
        /* out of range */
        ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.max_filesize) {
          failf(ದತ್ತಾಂಶ, "Maximum file size exceeded");
          ಹಿಂತಿರುಗಿ CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(ದತ್ತಾಂಶ, "Overflow Content-Length: value");
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ((offt == STRE_OK) &&
           ((k->ಗಾತ್ರ_2 == -1) || /* not set to something before */
            (k->ಗಾತ್ರ_2 == contentlength))) { /* or the same value */

          k->ಗಾತ್ರ_2 = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          ಆದರೆ(!curlx_str_single(&v, ','))
            ಮುಂದುವರಿ;

          ಆದರೆ(!curlx_str_newline(&v)) {
            k->maxdownload = k->ಗಾತ್ರ_2;
            ಹಿಂತಿರುಗಿ CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(ದತ್ತಾಂಶ, "Invalid Content-Length: value");
        ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
      }
    } ಆಗ(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : ಖಾಲಿ;
  ಆದರೆ(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    ಹಿಂತಿರುಗಿ Curl_build_unencoding_ರಾಶಿ(ದತ್ತಾಂಶ, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  ಆದರೆ(v) {
    ಅಕ್ಷರ *contenttype = Curl_copy_ಶೀರ್ಷಿಕೆ_ಮೌಲ್ಯ(hd);
    ಆದರೆ(!contenttype)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಆದರೆ(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    ಇಲ್ಲದಿದ್ದರೆ {
      curlx_free(ದತ್ತಾಂಶ->info.contenttype);
      ದತ್ತಾಂಶ->info.contenttype = contenttype;
    }
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಆದರೆ((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಆದರೆ((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(ದತ್ತಾಂಶ, "HTTP/1.0 connection set to keep alive");
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : ಖಾಲಿ;
  ಆದರೆ(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = v;

    /* Move forward until first digit or asterisk */
    ಆಗ(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    ಆದರೆ(ISDIGIT(*ptr)) {
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&ptr, &k->ಸ್ಥಾನಾಂತರ, CURL_OFF_T_MAX) &&
         (ದತ್ತಾಂಶ->state.resume_from == k->ಸ್ಥಾನಾಂತರ))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(k->httpcode < 300)
      ದತ್ತಾಂಶ->state.resume_from = 0; /* get everything */
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
ಸ್ಥಿರ CURLcode http_ಶೀರ್ಷಿಕೆ_l(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v = (!k->http_bodyless &&
                   (ದತ್ತಾಂಶ->ಗಣ.timecondition || ದತ್ತಾಂಶ->ಗಣ.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : ಖಾಲಿ;
  ಆದರೆ(v) {
    ಆದರೆ(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.get_filetime)
      ದತ್ತಾಂಶ->info.filetime = k->timeofdoc;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಆದರೆ(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    ಅಕ್ಷರ *location = Curl_copy_ಶೀರ್ಷಿಕೆ_ಮೌಲ್ಯ(hd);
    ಆದರೆ(!location)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಆದರೆ(!*location ||
       (ದತ್ತಾಂಶ->req.location && !strcmp(ದತ್ತಾಂಶ->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* has value and is not an exact repeat */
      ಆದರೆ(ದತ್ತಾಂಶ->req.location) {
        failf(ದತ್ತಾಂಶ, "Multiple Location headers");
        curlx_free(location);
        ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
      }
      ದತ್ತಾಂಶ->req.location = location;

      ಆದರೆ((k->httpcode >= 300 && k->httpcode < 400) &&
         ದತ್ತಾಂಶ->ಗಣ.http_follow_ಬಹುಲಕ) {
        CURLcode ಫಲಿತಾಂಶ;
        DEBUGASSERT(!ದತ್ತಾಂಶ->req.newurl);
        ದತ್ತಾಂಶ->req.newurl = curlx_strdup(ದತ್ತಾಂಶ->req.location); /* clone */
        ಆದರೆ(!ದತ್ತಾಂಶ->req.newurl)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        ಫಲಿತಾಂಶ = http_perhapsrewind(ದತ್ತಾಂಶ, conn);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

        /* mark the next request as a followed location: */
        ದತ್ತಾಂಶ->state.this_is_a_follow = TRUE;
      }
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
ಸ್ಥಿರ CURLcode http_ಶೀರ್ಷಿಕೆ_p(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen)
{
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  ಆದರೆ(v) {
    ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
    ಆದರೆ((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(ದತ್ತಾಂಶ, "HTTP/1.0 proxy connection set to keep alive");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(ದತ್ತಾಂಶ, "HTTP/1.1 proxy connection set close");
    }
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    ಅಕ್ಷರ *auth;
    CURLcode ಫಲಿತಾಂಶ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    ಆದರೆ(!ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      ಹಿಂತಿರುಗಿ CURLE_OK;
#ಆದರೆ_ಅಂತ್ಯ
    auth = Curl_copy_ಶೀರ್ಷಿಕೆ_ಮೌಲ್ಯ(hd);
    ಫಲಿತಾಂಶ = auth ? CURLE_OK : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಫಲಿತಾಂಶ = Curl_http_ಒಳಹರಿವು_auth(ದತ್ತಾಂಶ, TRUE, auth);
      curlx_free(auth);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
  ಆದರೆ(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
    ರಚನೆ negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    ರಚನೆ auth *authp = &ದತ್ತಾಂಶ->state.authhost;
    ಆದರೆ(!negdata)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಆದರೆ(authp->picked == CURLAUTH_NEGOTIATE) {
      ಅಕ್ಷರ *persistentauth = Curl_copy_ಶೀರ್ಷಿಕೆ_ಮೌಲ್ಯ(hd);
      ಆದರೆ(!persistentauth)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(ದತ್ತಾಂಶ, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
ಸ್ಥಿರ CURLcode http_ಶೀರ್ಷಿಕೆ_r(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v = HD_VAL(hd, hdlen, "Retry-After:");
  ಆದರೆ(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    ಆದರೆ(!Curl_getdate_capped(v, &date)) {
      time_t current = ಸಮಯ(ಖಾಲಿ);
      ಆದರೆ(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* Try it as a decimal number, ignore errors */
      (ಶೂನ್ಯ)curlx_str_ಸಂಖ್ಯೆ(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    ಆದರೆ(retry_after > 21600)
      retry_after = 21600;
    ದತ್ತಾಂಶ->info.retry_after = retry_after;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
ಸ್ಥಿರ CURLcode http_ಶೀರ್ಷಿಕೆ_s(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen)
{
#ಆದರೆ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)hd;
  (ಶೂನ್ಯ)hdlen;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  v = (ದತ್ತಾಂಶ->cookies && ದತ್ತಾಂಶ->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : ಖಾಲಿ;
  ಆದರೆ(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host = ದತ್ತಾಂಶ->req.cookiehost ?
      ದತ್ತಾಂಶ->req.cookiehost : ದತ್ತಾಂಶ->state.origin->hostname;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ secure_context = Curl_secure_context(ದತ್ತಾಂಶ, host) ?
      COOKIE_SECURE : 0;
    CURLcode ಫಲಿತಾಂಶ;
    Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE, CURL_ಬೀಗ_ACCESS_SINGLE);
    ಫಲಿತಾಂಶ = Curl_cookie_ಕೂಡಿಸು(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->cookies, v, host,
                             ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (ದತ್ತಾಂಶ->hsts &&
       (Curl_xfer_is_secure(ದತ್ತಾಂಶ) ||
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#ಇಲ್ಲದಿದ್ದರೆ
        0
#ಆದರೆ_ಅಂತ್ಯ
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : ಖಾಲಿ;
  ಆದರೆ(v) {
    CURLcode ಫಲಿತಾಂಶ = Curl_hsts_parse(
      ದತ್ತಾಂಶ->hsts, ದತ್ತಾಂಶ->state.origin->hostname, v);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      infof(ದತ್ತಾಂಶ, "Illegal STS header skipped");
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "Parsed STS header fine (%zu entries)",
            Curl_llist_ಎಣಿಕೆ(&ದತ್ತಾಂಶ->hsts->list));
#ಆದರೆ_ಅಂತ್ಯ
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
ಸ್ಥಿರ CURLcode http_ಶೀರ್ಷಿಕೆ_t(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;

  /* RFC 9112, ch. 6.1
   * "Transfer-Encoding MAY be sent in a response to a HEAD request or
   *  in a 304 (Not Modified) response (Section 15.4.5 of [HTTP]) to a
   *  GET request, neither of which includes a message body, to indicate
   *  that the origin server would have applied a transfer coding to the
   *  message body if the request had been an unconditional GET."
   *
   * Read: in these cases the 'Transfer-Encoding' does not apply
   * to any data following the response headers. Do not add any decoders.
   */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v = (!k->http_bodyless &&
                   (ದತ್ತಾಂಶ->state.httpreq != HTTPREQ_ತಲೆ) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : ಖಾಲಿ;
  ಆದರೆ(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode ಫಲಿತಾಂಶ = Curl_build_unencoding_ರಾಶಿ(ದತ್ತಾಂಶ, v, TRUE);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಆದರೆ(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(ದತ್ತಾಂಶ, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  ಆದರೆ(v) {
    ದತ್ತಾಂಶ->req.resp_trailer = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
ಸ್ಥಿರ CURLcode http_ಶೀರ್ಷಿಕೆ_w(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen)
{
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    ಅಕ್ಷರ *auth = Curl_copy_ಶೀರ್ಷಿಕೆ_ಮೌಲ್ಯ(hd);
    ಆದರೆ(!auth)
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = Curl_http_ಒಳಹರಿವು_auth(ದತ್ತಾಂಶ, FALSE, auth);
      curlx_free(auth);
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * http_header() parses a single response header.
 */
ಸ್ಥಿರ CURLcode http_ಶೀರ್ಷಿಕೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಯ್ಕೆ(hd[0]) {
  ಸ್ಥಿತಿ 'a':
  ಸ್ಥಿತಿ 'A':
    ಫಲಿತಾಂಶ = http_ಶೀರ್ಷಿಕೆ_a(ದತ್ತಾಂಶ, hd, hdlen);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'c':
  ಸ್ಥಿತಿ 'C':
    ಫಲಿತಾಂಶ = http_ಶೀರ್ಷಿಕೆ_c(ದತ್ತಾಂಶ, hd, hdlen);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'l':
  ಸ್ಥಿತಿ 'L':
    ಫಲಿತಾಂಶ = http_ಶೀರ್ಷಿಕೆ_l(ದತ್ತಾಂಶ, hd, hdlen);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'p':
  ಸ್ಥಿತಿ 'P':
    ಫಲಿತಾಂಶ = http_ಶೀರ್ಷಿಕೆ_p(ದತ್ತಾಂಶ, hd, hdlen);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'r':
  ಸ್ಥಿತಿ 'R':
    ಫಲಿತಾಂಶ = http_ಶೀರ್ಷಿಕೆ_r(ದತ್ತಾಂಶ, hd, hdlen);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 's':
  ಸ್ಥಿತಿ 'S':
    ಫಲಿತಾಂಶ = http_ಶೀರ್ಷಿಕೆ_s(ದತ್ತಾಂಶ, hd, hdlen);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 't':
  ಸ್ಥಿತಿ 'T':
    ಫಲಿತಾಂಶ = http_ಶೀರ್ಷಿಕೆ_t(ದತ್ತಾಂಶ, hd, hdlen);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'w':
  ಸ್ಥಿತಿ 'W':
    ಫಲಿತಾಂಶ = http_ಶೀರ್ಷಿಕೆ_w(ದತ್ತಾಂಶ, hd, hdlen);
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
    ಆದರೆ(conn->scheme->protocol & CURLPROTO_RTSP)
      ಫಲಿತಾಂಶ = Curl_rtsp_parseheader(ದತ್ತಾಂಶ, hd);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
ಸ್ಥಿರ CURLcode http_statusline(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *conn)
{
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;

  ಆಯ್ಕೆ(k->httpversion) {
  ಸ್ಥಿತಿ 10:
  ಸ್ಥಿತಿ 11:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP2
  ಸ್ಥಿತಿ 20:
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP3
  ಸ್ಥಿತಿ 30:
#ಆದರೆ_ಅಂತ್ಯ
    /* no major version switch mid-connection */
    ಆದರೆ(k->httpversion_ಕಳುಹಿಸಿದ &&
       (k->httpversion / 10 != k->httpversion_ಕಳುಹಿಸಿದ / 10)) {
      failf(ದತ್ತಾಂಶ, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_ಕಳುಹಿಸಿದ / 10, k->httpversion / 10);
      ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
  }

  ದತ್ತಾಂಶ->info.httpcode = k->httpcode;
  ದತ್ತಾಂಶ->info.httpversion = k->httpversion;
  conn->httpversion_ನೋಡಿದ = k->httpversion;

  ಆದರೆ(!ದತ್ತಾಂಶ->state.http_neg.rcvd_min ||
     ದತ್ತಾಂಶ->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    ದತ್ತಾಂಶ->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from && ದತ್ತಾಂಶ->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  ಆದರೆ(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(ದತ್ತಾಂಶ, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  ಆಯ್ಕೆ(k->httpcode) {
  ಸ್ಥಿತಿ 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.timecondition)
      ದತ್ತಾಂಶ->info.timecond = TRUE;
    FALLTHROUGH();
  ಸ್ಥಿತಿ 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->ಗಾತ್ರ_2 = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
ಸ್ಥಿರ CURLcode http_ಗಾತ್ರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  ಆದರೆ(ದತ್ತಾಂಶ->req.ignore_cl || k->chunk) {
    k->ಗಾತ್ರ_2 = k->maxdownload = -1;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(k->ಗಾತ್ರ_2 != -1) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.max_filesize &&
       !k->ignorebody &&
       (k->ಗಾತ್ರ_2 > ದತ್ತಾಂಶ->ಗಣ.max_filesize)) {
      failf(ದತ್ತಾಂಶ, "Maximum file size exceeded");
      ಹಿಂತಿರುಗಿ CURLE_FILESIZE_EXCEEDED;
    }
    ಆದರೆ(k->ignorebody)
      infof(ದತ್ತಾಂಶ, "setting size while ignoring");
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, k->ಗಾತ್ರ_2);
    k->maxdownload = k->ಗಾತ್ರ_2;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_verify_ಶೀರ್ಷಿಕೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen)
{
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  size_t i;
  bool has_colon = FALSE;
  ಗಾಗಿ(i = 0; i < hdlen; i++) {
    ಆಯ್ಕೆ(hd[i]) {
    ಸ್ಥಿತಿ '\0':
      /* this is bad, bail out */
      failf(ದತ್ತಾಂಶ, "NUL byte in header");
      ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
    ಸ್ಥಿತಿ '\r':
      ಆದರೆ(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(ದತ್ತಾಂಶ, "Carriage return found in header");
        ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ':':
      has_colon = TRUE;
      ನಿಲ್ಲಿಸು;
    }
  }
  ಆದರೆ(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಆದರೆ(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!has_colon) {
    /* this is bad, bail out */
    failf(ದತ್ತಾಂಶ, "Header without colon");
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_bump_headersize(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ max = MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
  ಆದರೆ(delta < MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ) {
    ದತ್ತಾಂಶ->info.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ += (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)delta;
    ದತ್ತಾಂಶ->req.allheadercount += (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)delta;
    ಆದರೆ(!connect_only)
      ದತ್ತಾಂಶ->req.headerbytecount += (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)delta;
    ಆದರೆ(ದತ್ತಾಂಶ->req.allheadercount > max)
      bad = ದತ್ತಾಂಶ->req.allheadercount;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->info.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ > (max * 20)) {
      bad = ದತ್ತಾಂಶ->info.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
      max *= 20;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    bad = ದತ್ತಾಂಶ->req.allheadercount + delta;
  ಆದರೆ(bad) {
    failf(ದತ್ತಾಂಶ, "Too large response headers: %zu > %u", bad, max);
    ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
ಸ್ಥಿರ CURLcode http_on_101_upgrade(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_ಬದಲಾಯಿತು)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;

#ಆದರೆ !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)blen;
  (ಶೂನ್ಯ)pconsumed;
#ಇಲ್ಲದಿದ್ದರೆ
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ upgr101_requested = k->upgr101;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(k->httpversion_ಕಳುಹಿಸಿದ != 11) {
    /* invalid for other HTTP versions */
    failf(ದತ್ತಾಂಶ, "server sent 101 response while not talking HTTP/1.1");
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->ಬಿಟ್ಟುಗಳು.upgrade_ಒಳ_ಪ್ರಗತಿ = FALSE;
  *conn_ಬದಲಾಯಿತು = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2
  ಆದರೆ(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(ದತ್ತಾಂಶ, "Received 101, Switching to HTTP/2");
    ಫಲಿತಾಂಶ = Curl_http2_upgrade(ದತ್ತಾಂಶ, conn, FIRSTSOCKET, buf, blen);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      *pconsumed += blen;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS
  ಆದರೆ(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(ದತ್ತಾಂಶ, "Received 101, Switching to WebSocket");
    ಫಲಿತಾಂಶ = Curl_ws_accept(ದತ್ತಾಂಶ, buf, blen);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      *pconsumed += blen; /* ws accept handled the data */
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ಆದರೆ_ಅಂತ್ಯ
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->ಶೀರ್ಷಿಕೆ = FALSE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
ಸ್ಥಿರ CURLcode http_on_1xx_response(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_ಬದಲಾಯಿತು)
{
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->ಶೀರ್ಷಿಕೆ = TRUE;
  k->headerline = 0; /* restart the header line counter */

  ಆಯ್ಕೆ(k->httpcode) {
  ಸ್ಥಿತಿ 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 101:
    ಹಿಂತಿರುಗಿ http_on_101_upgrade(ದತ್ತಾಂಶ, buf, blen, pconsumed, conn_ಬದಲಾಯಿತು);
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
ಸ್ಥಿರ ಶೂನ್ಯ http_check_auth_ಆವರಣ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.close &&
     (((ದತ್ತಾಂಶ->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((ದತ್ತಾಂಶ->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(ದತ್ತಾಂಶ, "Connection closure while negotiating auth (HTTP 1.0?)");
    ದತ್ತಾಂಶ->state.authproblem = TRUE;
  }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.close &&
    (((ದತ್ತಾಂಶ->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((ದತ್ತಾಂಶ->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(ದತ್ತಾಂಶ, "Connection closure while negotiating auth (HTTP 1.0?)");
    ದತ್ತಾಂಶ->state.authproblem = TRUE;
  }
  ಆದರೆ((conn->http_negotiate_state == CURL_NEGO_AUTH_ಮುಗಿದ) &&
     (ದತ್ತಾಂಶ->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  ಆದರೆ((conn->proxy_negotiate_state == CURL_NEGO_AUTH_ಮುಗಿದ) &&
     (ದತ್ತಾಂಶ->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#ಆದರೆ_ಅಂತ್ಯ
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು http_check_auth_ಆವರಣ(x, y) /*ಖಾಲಿಗಳು*/
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
ಸ್ಥಿರ CURLcode http_handle_send_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!ದತ್ತಾಂಶ->req.authneg && !conn->ಬಿಟ್ಟುಗಳು.close &&
     !Curl_creader_will_rewind(ದತ್ತಾಂಶ)) {
    /*
     * General treatment of errors when about to send data.
     * Including: "417 Expectation Failed", while waiting for
     * 100-continue.
     *
     * The check for close above is done because if something
     * else has already deemed the connection to get closed then
     * something else should have considered the big picture and
     * we avoid this check.
     */

    ಆಯ್ಕೆ(ದತ್ತಾಂಶ->state.httpreq) {
    ಸ್ಥಿತಿ HTTPREQ_PUT:
    ಸ್ಥಿತಿ HTTPREQ_POST:
    ಸ್ಥಿತಿ HTTPREQ_POST_FORM:
    ಸ್ಥಿತಿ HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      ಆದರೆ(!Curl_req_ಮುಗಿದ_sending(ದತ್ತಾಂಶ)) {
        ಆದರೆ((k->httpcode == 417) && http_exp100_is_selected(ದತ್ತಾಂಶ)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          ಆದರೆ(!k->writebytecount && http_exp100_is_ಕಾಯುವಿಕೆ(ದತ್ತಾಂಶ)) {
            infof(ದತ್ತಾಂಶ, "Got HTTP failure 417 while waiting for a 100");
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            infof(ದತ್ತಾಂಶ, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            ಫಲಿತಾಂಶ = http_perhapsrewind(ದತ್ತಾಂಶ, conn);
            ಆದರೆ(ಫಲಿತಾಂಶ)
              ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
          }
          ದತ್ತಾಂಶ->state.disableexpect = TRUE;
          Curl_req_abort_sending(ದತ್ತಾಂಶ);
          DEBUGASSERT(!ದತ್ತಾಂಶ->req.newurl);
          ದತ್ತಾಂಶ->req.newurl = Curl_bufref_dup(&ದತ್ತಾಂಶ->state.url);
          ಆದರೆ(!ದತ್ತಾಂಶ->req.newurl)
            ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.http_keep_sending_on_error) {
          infof(ದತ್ತಾಂಶ, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(ದತ್ತಾಂಶ);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          infof(ದತ್ತಾಂಶ, "HTTP error before end of send, stop sending");
          streamclose(conn);
          ಫಲಿತಾಂಶ = Curl_req_abort_sending(ದತ್ತಾಂಶ);
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
      }
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ: /* default label present to avoid compiler warnings */
      ನಿಲ್ಲಿಸು;
    }
  }

  ಆದರೆ(Curl_creader_will_rewind(ದತ್ತಾಂಶ) && !Curl_req_ಮುಗಿದ_sending(ದತ್ತಾಂಶ)) {
    /* We rewind before next send, continue sending now */
    infof(ದತ್ತಾಂಶ, "Keep sending data to get tossed away");
    CURL_REQ_ಗಣ_SEND(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode http_on_response(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆಯ_hd, size_t ಕೊನೆಯ_hd_len,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                                 size_t *pconsumed)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  bool conn_ಬದಲಾಯಿತು = FALSE;

  (ಶೂನ್ಯ)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  ಆದರೆ(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    ಆದರೆ(ದತ್ತಾಂಶ->req.httpversion != 20)
      infof(ದತ್ತಾಂಶ, "Lying server, not serving HTTP/2");
  }

  ಆದರೆ(k->httpcode < 200 && ಕೊನೆಯ_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    ಫಲಿತಾಂಶ = http_write_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ, ಕೊನೆಯ_hd, ಕೊನೆಯ_hd_len);
    ಕೊನೆಯ_hd = ಖಾಲಿ; /* handled it */
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(k->httpcode < 100) {
    failf(ದತ್ತಾಂಶ, "Unsupported response code in HTTP response");
    ಫಲಿತಾಂಶ = CURLE_UNSUPPORTED_PROTOCOL;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(k->httpcode < 200) {
    ಫಲಿತಾಂಶ = http_on_1xx_response(ದತ್ತಾಂಶ, buf, blen, pconsumed, &conn_ಬದಲಾಯಿತು);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* k->httpcode >= 200, final response */
  k->ಶೀರ್ಷಿಕೆ = FALSE;
  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.upgrade_ಒಳ_ಪ್ರಗತಿ) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->ಬಿಟ್ಟುಗಳು.upgrade_ಒಳ_ಪ್ರಗತಿ = FALSE;
    conn_ಬದಲಾಯಿತು = TRUE;
  }

  ಆದರೆ((k->ಗಾತ್ರ_2 == -1) && !k->chunk && !conn->ಬಿಟ್ಟುಗಳು.close &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     ದತ್ತಾಂಶ->state.httpreq != HTTPREQ_ತಲೆ) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(ದತ್ತಾಂಶ, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_ಆವರಣ(ದತ್ತಾಂಶ, conn);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  ಆದರೆ(ದತ್ತಾಂಶ->req.upgr101 == UPGR101_WS) {
    failf(ದತ್ತಾಂಶ, "Refused WebSocket upgrade: %d", k->httpcode);
    ಫಲಿತಾಂಶ = CURLE_HTTP_RETURNED_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* Check if this response means the transfer errored. */
  ಆದರೆ(http_should_fail(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->req.httpcode)) {
    failf(ದತ್ತಾಂಶ, "The requested URL returned error: %d",
          k->httpcode);
    ಫಲಿತಾಂಶ = CURLE_HTTP_RETURNED_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  ಫಲಿತಾಂಶ = Curl_http_auth_act(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(k->httpcode >= 300) {
    ಫಲಿತಾಂಶ = http_handle_send_error(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  /* final response without error, prepare to receive the body */
  ಫಲಿತಾಂಶ = http_firstwrite(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  ಫಲಿತಾಂಶ = http_ಗಾತ್ರ(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  ಆದರೆ((conn->scheme->protocol & CURLPROTO_RTSP) && ದತ್ತಾಂಶ->req.no_body &&
     !ದತ್ತಾಂಶ->ಗಣ.opt_no_body && (k->ಗಾತ್ರ_2 > 0))
    ದತ್ತಾಂಶ->req.no_body = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  ಆದರೆ(ದತ್ತಾಂಶ->req.no_body)
    k->download_ಮುಗಿದ = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  ಆದರೆ((k->maxdownload == 0) && (k->httpversion_ಕಳುಹಿಸಿದ < 20))
    k->download_ಮುಗಿದ = TRUE;

ಹೊರಹೋಗು:
  ಆದರೆ(ಕೊನೆಯ_hd)
    /* if not written yet, write it now */
    ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ,
                            http_write_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ, ಕೊನೆಯ_hd, ಕೊನೆಯ_hd_len));
  ಆದರೆ(conn_ಬದಲಾಯಿತು)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(ದತ್ತಾಂಶ->multi);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode http_rw_hd(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  ಪೂರ್ಣಾಂಕ writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  ಆದರೆ((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    ರಚನೆ dynbuf ಕೊನೆಯ_ಶೀರ್ಷಿಕೆ;
    size_t consumed;

    curlx_dyn_init(&ಕೊನೆಯ_ಶೀರ್ಷಿಕೆ, hdlen + 1);
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಕೊನೆಯ_ಶೀರ್ಷಿಕೆ, hd, hdlen);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&ದತ್ತಾಂಶ->state.headerb);
    ಫಲಿತಾಂಶ = http_on_response(ದತ್ತಾಂಶ, curlx_dyn_ptr(&ಕೊನೆಯ_ಶೀರ್ಷಿಕೆ),
                              curlx_dyn_len(&ಕೊನೆಯ_ಶೀರ್ಷಿಕೆ),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&ಕೊನೆಯ_ಶೀರ್ಷಿಕೆ);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_ಶೀರ್ಷಿಕೆ;
  ಆದರೆ(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    ಆದರೆ(ದತ್ತಾಂಶ->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = hd;

      curlx_str_passblanks(&p);
      ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p, "HTTP/", 5)) {
        p += 5;
        ಆಯ್ಕೆ(*p) {
        ಸ್ಥಿತಿ '1':
          p++;
          ಆದರೆ((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            ಆದರೆ(ISBLANK(p[2])) {
              k->httpversion = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(10 + (p[1] - '0'));
              p += 3;
              ಆದರೆ(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          ಆದರೆ(!fine_statusline) {
            failf(ದತ್ತಾಂಶ, "Unsupported HTTP/1 subversion in response");
            ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
          }
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ '2':
        ಸ್ಥಿತಿ '3':
          ಆದರೆ(!ISBLANK(p[1]))
            ನಿಲ್ಲಿಸು;
          k->httpversion = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((*p - '0') * 10);
          p += 2;
          ಆದರೆ(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            ಆದರೆ(!ISBLANK(*p))
              ನಿಲ್ಲಿಸು;
            fine_statusline = TRUE;
          }
          ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ: /* unsupported */
          failf(ದತ್ತಾಂಶ, "Unsupported HTTP version in response");
          ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      ಆದರೆ(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(ದತ್ತಾಂಶ, hd, hdlen);
        ಆದರೆ(check == ಸ್ಥಿತಿ_ಮುಗಿದ) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->conn->scheme->protocol & CURLPROTO_RTSP) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = hd;
      ರಚನೆ Curl_str ver;
      curl_off_t ಸ್ಥಿತಿ_2;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      ಆದರೆ(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_ಸಂಖ್ಯೆ(&p, &ಸ್ಥಿತಿ_2, 999)) {
        curlx_str_trimblanks(&ver);
        ಆದರೆ(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (ಪೂರ್ಣಾಂಕ)ಸ್ಥಿತಿ_2;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      ಆದರೆ(!fine_statusline)
        ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
    }

    ಆದರೆ(fine_statusline) {
      ಫಲಿತಾಂಶ = http_statusline(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      writetype |= CLIENTWRITE_ಸ್ಥಿತಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      k->ಶೀರ್ಷಿಕೆ = FALSE;   /* this is not a header line */
      ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
    }
  }

  ಫಲಿತಾಂಶ = Curl_verify_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ, hd, hdlen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = http_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ, hd, hdlen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ, hd, hdlen);

  ಆದರೆ(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, writetype, hd, hdlen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_bump_headersize(ದತ್ತಾಂಶ, hdlen, FALSE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
ಶೂನ್ಯ Curl_http_ಗಮ್ಯ_fold(ರಚನೆ dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd = curlx_dyn_ptr(bf);
  ಆದರೆ(len && (hd[len - 1] == '\n'))
    len--;
  ಆದರೆ(len && (hd[len - 1] == '\r'))
    len--;
  ಆಗ(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ unfold_ಶೀರ್ಷಿಕೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  Curl_http_ಗಮ್ಯ_fold(&ದತ್ತಾಂಶ->state.headerb);
  ದತ್ತಾಂಶ->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
ಸ್ಥಿರ CURLcode http_parse_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                                   size_t *pconsumed)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  ಆದರೆ(ದತ್ತಾಂಶ->state.maybe_folded && blen) {
    ಆದರೆ(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      ಫಲಿತಾಂಶ = http_rw_hd(ದತ್ತಾಂಶ, curlx_dyn_ptr(&ದತ್ತಾಂಶ->state.headerb),
                          curlx_dyn_len(&ದತ್ತಾಂಶ->state.headerb),
                          ಖಾಲಿ, 0, &ignore_this);
      curlx_dyn_reset(&ದತ್ತಾಂಶ->state.headerb);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ದತ್ತಾಂಶ->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  ಆಗ(blen && k->ಶೀರ್ಷಿಕೆ) {
    size_t consumed;
    size_t hlen;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd;
    size_t unfold_len = 0;

    ಆದರೆ(ದತ್ತಾಂಶ->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      ಆಗ(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      ಆದರೆ(blen) {
        /* insert a single space */
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&ದತ್ತಾಂಶ->state.headerb, " ", 1);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        ದತ್ತಾಂಶ->state.leading_unfold = FALSE; /* done now */
      }
    }

    ಕೊನೆ_ptr = memchr(buf, '\n', blen);
    ಆದರೆ(!ಕೊನೆ_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ದತ್ತಾಂಶ->state.headerb, buf, blen);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      *pconsumed += blen + unfold_len;

      ಆದರೆ(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(ದತ್ತಾಂಶ, conn,
                           curlx_dyn_ptr(&ದತ್ತಾಂಶ->state.headerb),
                           curlx_dyn_len(&ದತ್ತಾಂಶ->state.headerb));

        ಆದರೆ(st == ಸ್ಥಿತಿ_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->ಶೀರ್ಷಿಕೆ = FALSE;
          streamclose(conn);
          ಆದರೆ((k->httpversion >= 10) || conn->ಬಿಟ್ಟುಗಳು.reuse) {
            failf(ದತ್ತಾಂಶ, "Invalid status line");
            ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
          }
          ಆದರೆ(!ದತ್ತಾಂಶ->state.http_neg.accept_09) {
            failf(ದತ್ತಾಂಶ, "Received HTTP/0.9 when not allowed");
            ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          ಹೋಗು ಹೊರಹೋಗು;
        }
      }
      ಹೋಗು ಹೊರಹೋಗು; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (ಕೊನೆ_ptr - buf) + 1;

    ಫಲಿತಾಂಶ = curlx_dyn_addn(&ದತ್ತಾಂಶ->state.headerb, buf, consumed);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&ದತ್ತಾಂಶ->state.headerb);
    hd = curlx_dyn_ptr(&ದತ್ತಾಂಶ->state.headerb);

    ಆದರೆ(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(ದತ್ತಾಂಶ, conn, hd, hlen);
      ಆದರೆ(st == ಸ್ಥಿತಿ_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        ಆದರೆ((k->httpversion >= 10) || conn->ಬಿಟ್ಟುಗಳು.reuse) {
          failf(ದತ್ತಾಂಶ, "Invalid status line");
          ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
        }
        ಆದರೆ(!ದತ್ತಾಂಶ->state.http_neg.accept_09) {
          failf(ದತ್ತಾಂಶ, "Received HTTP/0.9 when not allowed");
          ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->ಶೀರ್ಷಿಕೆ = FALSE;
        leftover_body = TRUE;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        ಆದರೆ(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ);
          ಮುಂದುವರಿ;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          ದತ್ತಾಂಶ->state.maybe_folded = TRUE;
          ನಿಲ್ಲಿಸು;
        }
      }
    }

    ಫಲಿತಾಂಶ = http_rw_hd(ದತ್ತಾಂಶ, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&ದತ್ತಾಂಶ->state.headerb);
    ಆದರೆ(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
ಹೊರಹೋಗು:
  ಆದರೆ(!k->ಶೀರ್ಷಿಕೆ && !leftover_body) {
    curlx_dyn_free(&ದತ್ತಾಂಶ->state.headerb);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t consumed;
  ಅಕ್ಷರ tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  ಫಲಿತಾಂಶ = http_rw_hd(ದತ್ತಾಂಶ, hd, hdlen, &tmp, 0, &consumed);
  ಆದರೆ(!ಫಲಿತಾಂಶ && is_eos) {
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                                  size_t *pconsumed)
{
  ಆದರೆ(!ದತ್ತಾಂಶ->req.ಶೀರ್ಷಿಕೆ) {
    *pconsumed = 0;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURLcode ಫಲಿತಾಂಶ;

    ಫಲಿತಾಂಶ = http_parse_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, buf, blen, pconsumed);
    ಆದರೆ(!ಫಲಿತಾಂಶ && !ದತ್ತಾಂಶ->req.ಶೀರ್ಷಿಕೆ) {
      ಆದರೆ(!ದತ್ತಾಂಶ->req.no_body && curlx_dyn_len(&ದತ್ತಾಂಶ->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&ದತ್ತಾಂಶ->state.headerb),
                                   curlx_dyn_len(&ದತ್ತಾಂಶ->state.headerb));
      }
      curlx_dyn_free(&ದತ್ತಾಂಶ->state.headerb);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
}

CURLcode Curl_http_write_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                              bool is_eos)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t consumed;
  ಪೂರ್ಣಾಂಕ flags;

  ಫಲಿತಾಂಶ = Curl_http_write_resp_hds(ದತ್ತಾಂಶ, buf, blen, &consumed);
  ಆದರೆ(ಫಲಿತಾಂಶ || ದತ್ತಾಂಶ->req.ಮುಗಿದ)
    ಹೋಗು ಹೊರಹೋಗು;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !ದತ್ತಾಂಶ->req.ಶೀರ್ಷಿಕೆ);
  ಆದರೆ(!ದತ್ತಾಂಶ->req.ಶೀರ್ಷಿಕೆ && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    ಆದರೆ(is_eos)
      flags |= CLIENTWRITE_EOS;
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, flags, buf, blen);
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_ಸ್ಥಿತಿ(int32_t *pstatus, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
  int32_t ಸ್ಥಿತಿ_2 = 0;
  ಪೂರ್ಣಾಂಕ i;

  ಆದರೆ(len != 3)
    ಹೋಗು ಹೊರಹೋಗು;

  ಗಾಗಿ(i = 0; i < 3; ++i) {
    ಅಕ್ಷರ c = s[i];

    ಆದರೆ(c < '0' || c > '9')
      ಹೋಗು ಹೊರಹೋಗು;

    ಸ್ಥಿತಿ_2 *= 10;
    ಸ್ಥಿತಿ_2 += c - '0';
  }
  ಫಲಿತಾಂಶ = CURLE_OK;
ಹೊರಹೋಗು:
  *pstatus = ಫಲಿತಾಂಶ ? -1 : ಸ್ಥಿತಿ_2;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_http_req_make(ರಚನೆ httpreq **preq,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ, size_t m_len,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme, size_t s_len,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *authority, size_t a_len,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, size_t p_len)
{
  ರಚನೆ httpreq *req;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  DEBUGASSERT(ವಿಧಾನ && m_len);

  req = curlx_calloc(1, ಗಾತ್ರ(*req) + m_len);
  ಆದರೆ(!req)
    ಹೋಗು ಹೊರಹೋಗು;
#ಆದರೆ defined(__GNUC__) && __GNUC__ >= 13
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#ಸೂಚನೆ GCC diagnostic ignored "-Warray-bounds"
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಮರಣೆ_ನಕಲು(req->ವಿಧಾನ, ವಿಧಾನ, m_len);
#ಆದರೆ defined(__GNUC__) && __GNUC__ >= 13
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    ಆದರೆ(!req->scheme)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    ಆದರೆ(!req->authority)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ಮಾರ್ಗ) {
    req->ಮಾರ್ಗ = curlx_memdup0(ಮಾರ್ಗ, p_len);
    ಆದರೆ(!req->ಮಾರ್ಗ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  Curl_dynhds_init(&req->ಶೀರ್ಷಿಕೆಗಳು, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ && req)
    Curl_http_req_free(req);
  *preq = ಫಲಿತಾಂಶ ? ಖಾಲಿ : req;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode req_ನಿಯೋಜಿಸು_url_authority(ರಚನೆ httpreq *req, CURLU *url)
{
  ಅಕ್ಷರ *host, *ಪೋರ್ಟ್;
  ರಚನೆ dynbuf buf;
  CURLUcode uc;
  CURLcode ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;

  host = ಪೋರ್ಟ್ = ಖಾಲಿ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  ಆದರೆ(uc && uc != CURLUE_NO_HOST)
    ಹೋಗು ಹೊರಹೋಗು;
  ಆದರೆ(!host) {
    req->authority = ಖಾಲಿ;
    ಫಲಿತಾಂಶ = CURLE_OK;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  uc = curl_url_get(url, CURLUPART_ಪೋರ್ಟ್, &ಪೋರ್ಟ್, CURLU_NO_DEFAULT_ಪೋರ್ಟ್);
  ಆದರೆ(uc && uc != CURLUE_NO_ಪೋರ್ಟ್)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&buf, host);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಆದರೆ(ಪೋರ್ಟ್) {
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&buf, ":%s", ಪೋರ್ಟ್);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  req->authority = curlx_dyn_ptr(&buf);
ಹೊರಹೋಗು:
  curlx_free(host);
  curlx_free(ಪೋರ್ಟ್);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    curlx_dyn_free(&buf);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode req_ನಿಯೋಜಿಸು_url_ಮಾರ್ಗ(ರಚನೆ httpreq *req, CURLU *url)
{
  ಅಕ್ಷರ *ಮಾರ್ಗ, *query;
  ರಚನೆ dynbuf buf;
  CURLUcode uc;
  CURLcode ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;

  ಮಾರ್ಗ = query = ಖಾಲಿ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_ಮಾರ್ಗ, &ಮಾರ್ಗ, 0);
  ಆದರೆ(uc)
    ಹೋಗು ಹೊರಹೋಗು;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  ಆದರೆ(uc && uc != CURLUE_NO_QUERY)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(!query) {
    req->ಮಾರ್ಗ = ಮಾರ್ಗ;
    ಮಾರ್ಗ = ಖಾಲಿ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&buf, ಮಾರ್ಗ);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addf(&buf, "?%s", query);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    req->ಮಾರ್ಗ = curlx_dyn_ptr(&buf);
  }
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  curlx_free(ಮಾರ್ಗ);
  curlx_free(query);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    curlx_dyn_free(&buf);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_http_req_make2(ರಚನೆ httpreq **preq,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ, size_t m_len,
                             CURLU *url, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme_default)
{
  ರಚನೆ httpreq *req;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  CURLUcode uc;

  DEBUGASSERT(ವಿಧಾನ && m_len);

  req = curlx_calloc(1, ಗಾತ್ರ(*req) + m_len);
  ಆದರೆ(!req)
    ಹೋಗು ಹೊರಹೋಗು;
  ಸ್ಮರಣೆ_ನಕಲು(req->ವಿಧಾನ, ವಿಧಾನ, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  ಆದರೆ(uc && uc != CURLUE_NO_SCHEME)
    ಹೋಗು ಹೊರಹೋಗು;
  ಆದರೆ(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    ಆದರೆ(!req->scheme)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = req_ನಿಯೋಜಿಸು_url_authority(req, url);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಫಲಿತಾಂಶ = req_ನಿಯೋಜಿಸು_url_ಮಾರ್ಗ(req, url);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  Curl_dynhds_init(&req->ಶೀರ್ಷಿಕೆಗಳು, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ && req)
    Curl_http_req_free(req);
  *preq = ಫಲಿತಾಂಶ ? ಖಾಲಿ : req;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_http_req_free(ರಚನೆ httpreq *req)
{
  ಆದರೆ(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->ಮಾರ್ಗ);
    Curl_dynhds_free(&req->ಶೀರ್ಷಿಕೆಗಳು);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

ರಚನೆ name_const {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
  size_t namelen;
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ name_const H2_NON_ಕ್ಷೇತ್ರ[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

ಸ್ಥಿರ bool h2_permissible_ಕ್ಷೇತ್ರ(ರಚನೆ dynhds_ನಮೂದು *e)
{
  size_t i;
  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(H2_NON_ಕ್ಷೇತ್ರ); ++i) {
    ಆದರೆ(e->namelen == H2_NON_ಕ್ಷೇತ್ರ[i].namelen &&
       curl_strnequal(H2_NON_ಕ್ಷೇತ್ರ[i].name, e->name, e->namelen))
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool http_TE_has_token(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fvalue, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *token)
{
  ಆಗ(*fvalue) {
    ರಚನೆ Curl_str name;

    /* skip to first token */
    ಆಗ(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    ಆದರೆ(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ(curlx_str_casecompare(&name, token))
      ಹಿಂತಿರುಗಿ TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    ಆಗ(*fvalue && *fvalue != ',') {
      ಆದರೆ(*fvalue == '"') {
        ರಚನೆ Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        ಆದರೆ(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ))
          ಹಿಂತಿರುಗಿ FALSE;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        fvalue++;
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

CURLcode Curl_http_req_ಗಮ್ಯ_h2(ರಚನೆ dynhds *h2_ಶೀರ್ಷಿಕೆಗಳು,
                             ರಚನೆ httpreq *req, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme = ಖಾಲಿ, *authority = ಖಾಲಿ;
  ರಚನೆ dynhds_ನಮೂದು *e;
  size_t i;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_ಶೀರ್ಷಿಕೆಗಳು);

  ಆದರೆ(req->scheme) {
    scheme = req->scheme;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strcmp("CONNECT", req->ವಿಧಾನ)) {
    scheme = Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST(HTTP_PSEUDO_SCHEME));
    ಆದರೆ(scheme) {
      scheme += ಗಾತ್ರ(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(ದತ್ತಾಂಶ, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      scheme = ದತ್ತಾಂಶ->state.origin->scheme->name;
    }
  }

  ಆದರೆ(req->authority) {
    authority = req->authority;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    e = Curl_dynhds_get(&req->ಶೀರ್ಷಿಕೆಗಳು, STRCONST("Host"));
    ಆದರೆ(e)
      authority = e->ಮೌಲ್ಯ;
  }

  Curl_dynhds_reset(h2_ಶೀರ್ಷಿಕೆಗಳು);
  Curl_dynhds_ಗಣ_opts(h2_ಶೀರ್ಷಿಕೆಗಳು, DYNHDS_OPT_LOWERCASE);
  ಫಲಿತಾಂಶ = Curl_dynhds_ಕೂಡಿಸು(h2_ಶೀರ್ಷಿಕೆಗಳು, STRCONST(HTTP_PSEUDO_ವಿಧಾನ),
                           req->ವಿಧಾನ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(req->ವಿಧಾನ));
  ಆದರೆ(!ಫಲಿತಾಂಶ && scheme) {
    ಫಲಿತಾಂಶ = Curl_dynhds_ಕೂಡಿಸು(h2_ಶೀರ್ಷಿಕೆಗಳು, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(scheme));
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && authority) {
    ಫಲಿತಾಂಶ = Curl_dynhds_ಕೂಡಿಸು(h2_ಶೀರ್ಷಿಕೆಗಳು, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(authority));
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && req->ಮಾರ್ಗ) {
    ಫಲಿತಾಂಶ = Curl_dynhds_ಕೂಡಿಸು(h2_ಶೀರ್ಷಿಕೆಗಳು, STRCONST(HTTP_PSEUDO_ಮಾರ್ಗ),
                             req->ಮಾರ್ಗ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(req->ಮಾರ್ಗ));
  }
  ಗಾಗಿ(i = 0; !ಫಲಿತಾಂಶ && i < Curl_dynhds_ಎಣಿಕೆ(&req->ಶೀರ್ಷಿಕೆಗಳು); ++i) {
    e = Curl_dynhds_getn(&req->ಶೀರ್ಷಿಕೆಗಳು, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    ಆದರೆ(e->namelen == 2 && curl_strequal("TE", e->name)) {
      ಆದರೆ(http_TE_has_token(e->ಮೌಲ್ಯ, "trailers"))
        ಫಲಿತಾಂಶ = Curl_dynhds_ಕೂಡಿಸು(h2_ಶೀರ್ಷಿಕೆಗಳು, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(h2_permissible_ಕ್ಷೇತ್ರ(e)) {
      ಫಲಿತಾಂಶ = Curl_dynhds_ಕೂಡಿಸು(h2_ಶೀರ್ಷಿಕೆಗಳು, e->name, e->namelen,
                               e->ಮೌಲ್ಯ, e->valuelen);
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_http_resp_make(ರಚನೆ http_resp **presp,
                             ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *description)
{
  ರಚನೆ http_resp *resp;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  resp = curlx_calloc(1, ಗಾತ್ರ(*resp));
  ಆದರೆ(!resp)
    ಹೋಗು ಹೊರಹೋಗು;

  resp->ಸ್ಥಿತಿ_2 = ಸ್ಥಿತಿ_2;
  ಆದರೆ(description) {
    resp->description = curlx_strdup(description);
    ಆದರೆ(!resp->description)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  Curl_dynhds_init(&resp->ಶೀರ್ಷಿಕೆಗಳು, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ && resp)
    Curl_http_resp_free(resp);
  *presp = ಫಲಿತಾಂಶ ? ಖಾಲಿ : resp;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_http_resp_free(ರಚನೆ http_resp *resp)
{
  ಆದರೆ(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->ಶೀರ್ಷಿಕೆಗಳು);
    Curl_dynhds_free(&resp->trailers);
    ಆದರೆ(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_http = {
  Curl_http_ಸಿದ್ಧತೆ_conn,                 /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_ಮುಗಿದ,                       /* done */
  ZERO_NULL,                            /* do_more */
  ZERO_NULL,                            /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  Curl_http_doing_pollset,              /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_http_perform_pollset,            /* perform_pollset */
  ZERO_NULL,                            /* disconnect */
  Curl_http_write_resp,                 /* write_resp */
  Curl_http_write_resp_hd,              /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  Curl_http_follow,                     /* follow */
};

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_HTTP */
