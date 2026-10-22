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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NET_IF_H
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "formdata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mime.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cookie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_digest.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_ntlm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_negotiate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_aws_sigv4.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_httpsig.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urlapi-int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dynhds.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "headers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parsedate.h" /* for the week day and month names */
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "content_encoding.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "altsvc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hsts.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rtsp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ws.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

ਖਾਲੀ Curl_http_neg_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ http_negotiation *neg)
{
  memset(neg, 0, ਆਕਾਰ(*neg));
  neg->accept_09 = ਅੰਕੜੇ->ਸੈੱਟ.http09_allowed;
  ਚੋਣ(ਅੰਕੜੇ->ਸੈੱਟ.httpwant) {
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_2_TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_2_ਪੂਰਵ_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    ਅੰਕੜੇ->state.http_neg.h2_ਪੂਰਵ_knowledge = TRUE;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_3_ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_NONE:
  ਮੂਲ:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    ਰੋਕੋ;
  }
}

CURLcode Curl_http_ਤਿਆਰੀ_conn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  ਜੇ(ਅੰਕੜੇ->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode ਨਤੀਜਾ = Curl_conn_may_http3(ਅੰਕੜੇ, conn);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
/*
 * checkProxyHeaders() checks the linked list of custom proxy headers
 * if proxy headers are not available, then it will lookup into http header
 * link list
 *
 * It takes a connectdata struct as input to see if this is a proxy request or
 * not, as it then might check a different header list. Provide the header
 * prefix without colon!
 */
ਅੱਖਰ *Curl_checkProxyheaders(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਬਣਤਰ connectdata *conn,
                             ਅਚਲ ਅੱਖਰ *thisheader,
                             ਅਚਲ size_t thislen)
{
  ਬਣਤਰ curl_slist *ਸਿਰ;

  ਲਈ(ਸਿਰ = (conn->http_proxy.peer && ਅੰਕੜੇ->ਸੈੱਟ.sep_ਸਿਰਲੇਖਾਂ) ?
        ਅੰਕੜੇ->ਸੈੱਟ.proxyheaders : ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ;
      ਸਿਰ; ਸਿਰ = ਸਿਰ->ਅਗਲਾ) {
    ਜੇ(curl_strnequal(ਸਿਰ->ਅੰਕੜੇ, thisheader, thislen) &&
       Curl_headersep(ਸਿਰ->ਅੰਕੜੇ[thislen]))
      ਵਾਪਸ ਸਿਰ->ਅੰਕੜੇ;
  }

  ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
ਸਥਿਰ bool ਸਿਰਲੇਖ_has_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ **headerp, ਬਣਤਰ Curl_str *outp)
{
  bool ਮੁੱਲ = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  ਜੇ(ਮੁੱਲ) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  ਵਾਪਸ ਮੁੱਲ;
}

ਸਥਿਰ bool http_ਸਿਰਲੇਖ_is_ਖਾਲੀ_ਥਾਵਾਂ(ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ)
{
  ਬਣਤਰ Curl_str ਬਾਹਰਲਾ;

  ਜੇ(ਸਿਰਲੇਖ_has_ਮੁੱਲ(&ਸਿਰਲੇਖ, &ਬਾਹਰਲਾ)) {
    ਵਾਪਸ curlx_strlen(&ਬਾਹਰਲਾ) == 0;
  }
  ਵਾਪਸ TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
ਸਥਿਰ CURLcode copy_custom_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ, ਅੱਖਰ **valp)
{
  ਬਣਤਰ Curl_str ਬਾਹਰਲਾ = { 0 };

  /* find the end of the header name */
  ਜੇ(ਸਿਰਲੇਖ_has_ਮੁੱਲ(&ਸਿਰਲੇਖ, &ਬਾਹਰਲਾ)) {
    *valp = curlx_memdup0(curlx_str(&ਬਾਹਰਲਾ), curlx_strlen(&ਬਾਹਰਲਾ));
    ਜੇ(*valp)
      ਵਾਪਸ CURLE_OK;
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  /* bad input */
  *valp = ਨਿਰਾ;
  ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
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
ਅੱਖਰ *Curl_copy_ਸਿਰਲੇਖ_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ)
{
  ਬਣਤਰ Curl_str ਬਾਹਰਲਾ;

  /* find the end of the header name */
  ਜੇ(!curlx_str_until(&ਸਿਰਲੇਖ, &ਬਾਹਰਲਾ, MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਮਾਪ, ':') &&
     !curlx_str_single(&ਸਿਰਲੇਖ, ':')) {
    curlx_str_untilnl(&ਸਿਰਲੇਖ, &ਬਾਹਰਲਾ, MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਮਾਪ);
    curlx_str_trimblanks(&ਬਾਹਰਲਾ);
    ਵਾਪਸ curlx_memdup0(curlx_str(&ਬਾਹਰਲਾ), curlx_strlen(&ਬਾਹਰਲਾ));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP_AUTH

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
ਸਥਿਰ CURLcode http_output_basic(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn, bool proxy)
{
  size_t ਮਾਪ = 0;
  ਅੱਖਰ *authorization = ਨਿਰਾ;
  ਅੱਖਰ **p_hd;
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ Curl_creds *creds = ਨਿਰਾ;
  ਅੱਖਰ *ਬਾਹਰਲਾ;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  ਜੇ(proxy) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    p_hd = &ਅੰਕੜੇ->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#ਨਹੀਂਤਾਂ
    (ਖਾਲੀ)conn;
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ {
    p_hd = &ਅੰਕੜੇ->req.hd_auth;
    creds = ਅੰਕੜੇ->state.creds;
  }

  ਜੇ(!creds) {
    DEBUGASSERT(0);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਬਾਹਰਲਾ = curl_maprintf("%s:%s", creds->user, creds->passwd);
  ਜੇ(!ਬਾਹਰਲਾ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਨਤੀਜਾ = curlx_base64_encode((uint8_t *)ਬਾਹਰਲਾ, ਲੜੀ_ਲੰਬਾਈ(ਬਾਹਰਲਾ),
                               &authorization, &ਮਾਪ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  ਜੇ(!authorization) {
    ਨਤੀਜਾ = CURLE_REMOTE_ACCESS_DENIED;
    ਜਾਓ fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  ਜੇ(!*p_hd) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ fail;
  }

fail:
  curlx_free(ਬਾਹਰਲਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
ਸਥਿਰ CURLcode http_output_bearer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅੱਖਰ **userp;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(ਅੰਕੜੇ->state.creds));
  userp = &ਅੰਕੜੇ->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(ਅੰਕੜੇ->state.creds));

  ਜੇ(!*userp) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ fail;
  }

fail:
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
ਸਥਿਰ bool pickoneauth(ਬਣਤਰ auth *pick, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ,
                        ਬਣਤਰ Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ avail = pick->avail & pick->want & ਨਕਾਬ;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  ਜੇ(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BEARER_AUTH
  ਨਹੀਂਤਾਂ ਜੇ((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
  ਨਹੀਂਤਾਂ ਜੇ((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#ਜੇ_ਅੰਤ
  ਨਹੀਂਤਾਂ ਜੇ(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BASIC_AUTH
  ਨਹੀਂਤਾਂ ਜੇ((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_AWS
  ਨਹੀਂਤਾਂ ਜੇ(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTPSIG
  ਨਹੀਂਤਾਂ ਜੇ(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#ਜੇ_ਅੰਤ
  ਨਹੀਂਤਾਂ {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  ਵਾਪਸ picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
ਸਥਿਰ CURLcode http_perhapsrewind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ connectdata *conn)
{
  curl_off_t bytessent = ਅੰਕੜੇ->req.writebytecount;
  curl_off_t expectsend = Curl_creader_ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(ਅੰਕੜੇ);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!ਅੰਕੜੇ->req.upload_ਪੂਰਾ && !little_upload_remains);
  VERBOSE(ਅਚਲ ਅੱਖਰ *ongoing_auth = ਨਿਰਾ);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  ਜੇ(needs_rewind) {
    infof(ਅੰਕੜੇ, "Need to rewind upload for next request");
    Curl_creader_ਸੈੱਟ_rewind(ਅੰਕੜੇ, TRUE);
  }

  ਜੇ(conn->ਬਿੱਟ.close)
    /* If we already decided to close this connection, we cannot veto. */
    ਵਾਪਸ CURLE_OK;

  ਜੇ(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
    ਜੇ((ਅੰਕੜੇ->state.authproxy.picked == CURLAUTH_NTLM) ||
       (ਅੰਕੜੇ->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      ਜੇ((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
    /* There is still data left to send */
    ਜੇ((ਅੰਕੜੇ->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (ਅੰਕੜੇ->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      ਜੇ((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#ਜੇ_ਅੰਤ
  }

  ਜੇ(abort_upload) {
    ਜੇ(upload_remain >= 0)
      infof(ਅੰਕੜੇ, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    ਅੰਕੜੇ->req.ਮਾਪ = 0; /* do not download any more than 0 bytes */
    ਅੰਕੜੇ->req.http_bodyless = TRUE;
  }
  ਵਾਪਸ CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
ਸਥਿਰ bool http_should_fail(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ httpcode)
{
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.http_fail_on_error)
    ਵਾਪਸ FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  ਜੇ(httpcode < 400)
    ਵਾਪਸ FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  ਜੇ(ਅੰਕੜੇ->state.resume_from && ਅੰਕੜੇ->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    ਵਾਪਸ FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  ਜੇ((httpcode != 401) && (httpcode != 407))
    ਵਾਪਸ TRUE;

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
  ਜੇ((httpcode == 401) && !ਅੰਕੜੇ->state.creds)
    ਵਾਪਸ TRUE;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ((httpcode == 407) && !ਅੰਕੜੇ->conn->http_proxy.creds)
    ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ

  ਵਾਪਸ (bool)ਅੰਕੜੇ->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ authmask = ~0UL;

  ਜੇ(!Curl_creds_has_oauth_bearer(ਅੰਕੜੇ->state.creds))
    authmask &= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)~CURLAUTH_BEARER;

  ਜੇ(100 <= ਅੰਕੜੇ->req.httpcode && ਅੰਕੜੇ->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    ਵਾਪਸ CURLE_OK;

  ਜੇ(ਅੰਕੜੇ->state.authproblem)
    ਵਾਪਸ ਅੰਕੜੇ->ਸੈੱਟ.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  ਜੇ(ਅੰਕੜੇ->state.creds &&
     ((ਅੰਕੜੇ->req.httpcode == 401) ||
      (ਅੰਕੜੇ->req.authneg && ਅੰਕੜੇ->req.httpcode < 300))) {
    pickhost = pickoneauth(&ਅੰਕੜੇ->state.authhost, authmask, ਅੰਕੜੇ->state.creds);
    ਜੇ(!pickhost)
      ਅੰਕੜੇ->state.authproblem = TRUE;
    ਨਹੀਂਤਾਂ
      ਅੰਕੜੇ->info.httpauthpicked = ਅੰਕੜੇ->state.authhost.picked;
    ਜੇ(ਅੰਕੜੇ->state.authhost.picked == CURLAUTH_NTLM &&
       (ਅੰਕੜੇ->req.httpversion_ਭੇਜਿਆ > 11)) {
      infof(ਅੰਕੜੇ, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      ਅੰਕੜੇ->state.http_neg.wanted = CURL_HTTP_V1x;
      ਅੰਕੜੇ->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ(conn->http_proxy.creds &&
     ((ਅੰਕੜੇ->req.httpcode == 407) ||
      (ਅੰਕੜੇ->req.authneg && ਅੰਕੜੇ->req.httpcode < 300))) {
    pickproxy = pickoneauth(&ਅੰਕੜੇ->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    ਜੇ(!pickproxy)
      ਅੰਕੜੇ->state.authproblem = TRUE;
    ਨਹੀਂਤਾਂ
      ਅੰਕੜੇ->info.proxyauthpicked = ਅੰਕੜੇ->state.authproxy.picked;
  }
#ਜੇ_ਅੰਤ

  ਜੇ(pickhost || pickproxy) {
    ਨਤੀਜਾ = http_perhapsrewind(ਅੰਕੜੇ, conn);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(ਅੰਕੜੇ->req.newurl);
    /* clone URL */
    ਅੰਕੜੇ->req.newurl = Curl_bufref_dup(&ਅੰਕੜੇ->state.url);
    ਜੇ(!ਅੰਕੜੇ->req.newurl)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਨਹੀਂਤਾਂ ਜੇ((ਅੰਕੜੇ->req.httpcode < 300) &&
          !ਅੰਕੜੇ->state.authhost.ਪੂਰਾ &&
          ਅੰਕੜੇ->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (ਅੰਕੜੇ->state.httpreq != HTTPREQ_GET) &&
          (ਅੰਕੜੇ->state.httpreq != HTTPREQ_ਸਿਰ)) {
    /* clone URL */
    ਅੰਕੜੇ->req.newurl = Curl_bufref_dup(&ਅੰਕੜੇ->state.url);
    ਜੇ(!ਅੰਕੜੇ->req.newurl)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਅੰਕੜੇ->state.authhost.ਪੂਰਾ = TRUE;
  }
  ਜੇ(http_should_fail(ਅੰਕੜੇ, ਅੰਕੜੇ->req.httpcode)) {
    failf(ਅੰਕੜੇ, "The requested URL returned error: %d",
          ਅੰਕੜੇ->req.httpcode);
    ਨਤੀਜਾ = CURLE_HTTP_RETURNED_ERROR;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
ਸਥਿਰ CURLcode output_auth_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ connectdata *conn,
                                    ਬਣਤਰ auth *authstatus,
                                    ਅਚਲ ਅੱਖਰ *request,
                                    ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                                    bool proxy)
{
  ਅਚਲ ਅੱਖਰ *auth = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)conn;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
  (ਖਾਲੀ)request;
  (ਖਾਲੀ)ਰਸਤਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_AWS
  ਜੇ((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    ਨਤੀਜਾ = Curl_output_aws_sigv4(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTPSIG
  ਜੇ((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    ਜੇ(Curl_auth_allowed_ਮੰਜ਼ਿਲ_host(ਅੰਕੜੇ)) {
      auth = "HTTPSIG";
      ਨਤੀਜਾ = Curl_output_httpsig(ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ
      authstatus->ਪੂਰਾ = TRUE;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
  ਜੇ(authstatus->picked == CURLAUTH_NEGOTIATE) {
    ਜੇ(
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(ਅੰਕੜੇ, conn,
                                        STRCONST("Proxy-authorization"))) ||
#ਜੇ_ਅੰਤ
      (!proxy && !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Authorization")) &&
       Curl_auth_allowed_ਮੰਜ਼ਿਲ_host(ਅੰਕੜੇ))) {
      auth = "Negotiate";
      ਨਤੀਜਾ = Curl_output_negotiate(ਅੰਕੜੇ, conn, proxy);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ
      authstatus->ਪੂਰਾ = TRUE;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
  ਜੇ(authstatus->picked == CURLAUTH_NTLM) {
    ਜੇ(proxy || Curl_auth_allowed_ਮੰਜ਼ਿਲ_host(ਅੰਕੜੇ)) {
      auth = "NTLM";
      ਨਤੀਜਾ = Curl_output_ntlm(ਅੰਕੜੇ, proxy);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ
      authstatus->ਪੂਰਾ = TRUE;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
  ਜੇ(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    ਨਤੀਜਾ = Curl_output_digest(ਅੰਕੜੇ,
                                proxy,
                                (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)request,
                                (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਰਸਤਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BASIC_AUTH
  ਜੇ(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    ਜੇ(
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
       (proxy && conn->http_proxy.creds &&
        Curl_creds_has_user_or_pass(conn->http_proxy.creds) &&
        !Curl_checkProxyheaders(ਅੰਕੜੇ, conn,
                                STRCONST("Proxy-authorization"))) ||
#ਜੇ_ਅੰਤ
       (!proxy && ਅੰਕੜੇ->state.creds &&
        Curl_creds_has_user_or_pass(ਅੰਕੜੇ->state.creds) &&
        !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Authorization")))) {
      auth = "Basic";
      ਨਤੀਜਾ = http_output_basic(ਅੰਕੜੇ, conn, proxy);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->ਪੂਰਾ = TRUE;
  }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BEARER_AUTH
  ਜੇ(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    ਜੇ(!proxy && Curl_creds_has_oauth_bearer(ਅੰਕੜੇ->state.creds) &&
       !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Authorization"))) {
      auth = "Bearer";
      ਨਤੀਜਾ = http_output_bearer(ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->ਪੂਰਾ = TRUE;
  }
#ਜੇ_ਅੰਤ

  ਜੇ(auth) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    ਜੇ(proxy)
      ਅੰਕੜੇ->info.proxyauthpicked = authstatus->picked;
    ਨਹੀਂਤਾਂ
      ਅੰਕੜੇ->info.httpauthpicked = authstatus->picked;
    infof(ਅੰਕੜੇ, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (ਅੰਕੜੇ->state.creds ?
           ਅੰਕੜੇ->state.creds->user : ""));
#ਨਹੀਂਤਾਂ
    (ਖਾਲੀ)proxy;
    infof(ਅੰਕੜੇ, "Server auth using %s with user '%s'",
          auth, ਅੰਕੜੇ->state.creds ?
          ਅੰਕੜੇ->state.creds->user : "");
#ਜੇ_ਅੰਤ
    authstatus->multipass = !authstatus->ਪੂਰਾ;
  }
  ਨਹੀਂਤਾਂ {
    authstatus->multipass = FALSE;
    ਜੇ(proxy)
      ਅੰਕੜੇ->info.proxyauthpicked = 0;
    ਨਹੀਂਤਾਂ
      ਅੰਕੜੇ->info.httpauthpicked = 0;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_http_output_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn,
                               ਅਚਲ ਅੱਖਰ *request,
                               Curl_HttpReq httpreq,
                               ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                               ਅਚਲ ਅੱਖਰ *query,
                               bool is_connect)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ auth *authhost;
  ਬਣਤਰ auth *authproxy;
  ਅਚਲ ਅੱਖਰ *ਰਸਤਾ_and_query = ਰਸਤਾ;
  ਅੱਖਰ *tmp_str = ਨਿਰਾ;

  DEBUGASSERT(ਅੰਕੜੇ);
  authhost = &ਅੰਕੜੇ->state.authhost;
  authproxy = &ਅੰਕੜੇ->state.authproxy;

  ਜੇ(
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    (!conn->http_proxy.peer || !conn->http_proxy.creds) &&
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
    !(authhost->want & CURLAUTH_NEGOTIATE) &&
    !(authproxy->want & CURLAUTH_NEGOTIATE) &&
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTPSIG
    !(authhost->want & CURLAUTH_HTTPSIG) &&
#ਜੇ_ਅੰਤ
    !ਅੰਕੜੇ->state.creds) {
    /* no authentication with no user or password */
    authhost->ਪੂਰਾ = TRUE;
    authproxy->ਪੂਰਾ = TRUE;
    ਨਤੀਜਾ = CURLE_OK;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(query) {
    tmp_str = curl_maprintf("%s?%s", ਰਸਤਾ, query);
    ਜੇ(!tmp_str) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਰਸਤਾ_and_query = tmp_str;
  }

  ਜੇ(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  ਜੇ(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  ਜੇ(conn->ਬਿੱਟ.origin_is_proxy || is_connect) {
    ਨਤੀਜਾ = output_auth_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, conn, authproxy, request,
                                 ਰਸਤਾ_and_query, TRUE);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)is_connect;
#ਜੇ_ਅੰਤ /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->ਪੂਰਾ = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  ਜੇ(ਅੰਕੜੇ->state.creds || Curl_auth_allowed_ਮੰਜ਼ਿਲ_host(ਅੰਕੜੇ))
    ਨਤੀਜਾ = output_auth_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, conn, authhost, request,
                                 ਰਸਤਾ_and_query, FALSE);
  ਨਹੀਂਤਾਂ
    authhost->ਪੂਰਾ = TRUE;

  ਜੇ(((authhost->multipass && !authhost->ਪੂਰਾ) ||
      (authproxy->multipass && !authproxy->ਪੂਰਾ)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_ਸਿਰ)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    ਅੰਕੜੇ->req.authneg = TRUE;
  }
  ਨਹੀਂਤਾਂ
    ਅੰਕੜੇ->req.authneg = FALSE;

ਬਾਹਰਲਾ:
  curlx_free(tmp_str);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਨਹੀਂਤਾਂ /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn,
                               ਅਚਲ ਅੱਖਰ *request,
                               Curl_HttpReq httpreq,
                               ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                               ਅਚਲ ਅੱਖਰ *query,
                               bool is_connect)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)request;
  (ਖਾਲੀ)httpreq;
  (ਖਾਲੀ)ਰਸਤਾ;
  (ਖਾਲੀ)query;
  (ਖਾਲੀ)is_connect;
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP_AUTH, else */

#ਜੇ defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
ਸਥਿਰ bool authcmp(ਅਚਲ ਅੱਖਰ *auth, ਅਚਲ ਅੱਖਰ *ਸਤਰ)
{
  /* the auth string must not have an alnum following */
  size_t n = ਲੜੀ_ਲੰਬਾਈ(auth);
  ਵਾਪਸ curl_strnequal(auth, ਸਤਰ, n) && !ISALNUM(ਸਤਰ[n]);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
ਸਥਿਰ CURLcode auth_spnego(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            bool proxy,
                            ਅਚਲ ਅੱਖਰ *auth,
                            ਬਣਤਰ auth *authp,
                            uint32_t *availp)
{
  ਜੇ((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    ਜੇ(authp->picked == CURLAUTH_NEGOTIATE) {
      ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
      CURLcode ਨਤੀਜਾ = Curl_ਨਿਵੇਸ਼_negotiate(ਅੰਕੜੇ, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      ਜੇ(!ਨਤੀਜਾ) {
        curlx_free(ਅੰਕੜੇ->req.newurl);
        ਅੰਕੜੇ->req.newurl = Curl_bufref_dup(&ਅੰਕੜੇ->state.url);
        ਜੇ(!ਅੰਕੜੇ->req.newurl)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਅੰਕੜੇ->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      ਨਹੀਂਤਾਂ
        ਅੰਕੜੇ->state.authproblem = TRUE;
    }
  }
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
ਸਥਿਰ CURLcode auth_ntlm(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          bool proxy,
                          ਅਚਲ ਅੱਖਰ *auth,
                          ਬਣਤਰ auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  ਜੇ((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    ਜੇ(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode ਨਤੀਜਾ = Curl_ਨਿਵੇਸ਼_ntlm(ਅੰਕੜੇ, proxy, auth);
      ਜੇ(!ਨਤੀਜਾ)
        ਅੰਕੜੇ->state.authproblem = FALSE;
      ਨਹੀਂਤਾਂ {
        ਜੇ(ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
          ਵਾਪਸ ਨਤੀਜਾ;
        infof(ਅੰਕੜੇ, "NTLM authentication problem, ignoring.");
        ਅੰਕੜੇ->state.authproblem = TRUE;
      }
    }
  }
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
ਸਥਿਰ CURLcode auth_digest(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            bool proxy,
                            ਅਚਲ ਅੱਖਰ *auth,
                            ਬਣਤਰ auth *authp,
                            uint32_t *availp)
{
  ਜੇ(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(ਅੰਕੜੇ, "Ignoring duplicate digest auth header.");
  }
  ਨਹੀਂਤਾਂ ਜੇ(Curl_auth_is_digest_supported()) {
    CURLcode ਨਤੀਜਾ;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    ਨਤੀਜਾ = Curl_ਨਿਵੇਸ਼_digest(ਅੰਕੜੇ, proxy, auth);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
        ਵਾਪਸ ਨਤੀਜਾ;
      infof(ਅੰਕੜੇ, "Digest authentication problem, ignoring.");
      ਅੰਕੜੇ->state.authproblem = TRUE;
    }
  }
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BASIC_AUTH
ਸਥਿਰ CURLcode auth_basic(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  ਜੇ(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(ਅੰਕੜੇ, "Basic authentication problem, ignoring.");
    ਅੰਕੜੇ->state.authproblem = TRUE;
  }
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BEARER_AUTH
ਸਥਿਰ CURLcode auth_bearer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  ਜੇ(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(ਅੰਕੜੇ, "Bearer authentication problem, ignoring.");
    ਅੰਕੜੇ->state.authproblem = TRUE;
  }
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_ਨਿਵੇਸ਼_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool proxy,
                              ਅਚਲ ਅੱਖਰ *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#ਜੇ defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  ਬਣਤਰ auth *authp;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(ਅੰਕੜੇ);

  ਜੇ(proxy) {
    availp = &ਅੰਕੜੇ->info.proxyauthavail;
    authp = &ਅੰਕੜੇ->state.authproxy;
  }
  ਨਹੀਂਤਾਂ {
    availp = &ਅੰਕੜੇ->info.httpauthavail;
    authp = &ਅੰਕੜੇ->state.authhost;
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

  ਜਦੋਂ(*auth) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
    ਜੇ(authcmp("Negotiate", auth))
      ਨਤੀਜਾ = auth_spnego(ਅੰਕੜੇ, proxy, auth, authp, availp);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
    ਜੇ(!ਨਤੀਜਾ && authcmp("NTLM", auth))
      ਨਤੀਜਾ = auth_ntlm(ਅੰਕੜੇ, proxy, auth, authp, availp);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
    ਜੇ(!ਨਤੀਜਾ && authcmp("Digest", auth))
      ਨਤੀਜਾ = auth_digest(ਅੰਕੜੇ, proxy, auth, authp, availp);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BASIC_AUTH
    ਜੇ(!ਨਤੀਜਾ && authcmp("Basic", auth))
      ਨਤੀਜਾ = auth_basic(ਅੰਕੜੇ, authp, availp);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BEARER_AUTH
    ਜੇ(authcmp("Bearer", auth))
      ਨਤੀਜਾ = auth_bearer(ਅੰਕੜੇ, authp, availp);
#ਜੇ_ਅੰਤ

    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    ਜੇ(auth) /* if we are on a comma, skip it */
      auth++;
    ਨਹੀਂਤਾਂ
      ਰੋਕੋ;
    curlx_str_passblanks(&auth);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)proxy;
  (ਖਾਲੀ)auth;
  /* nothing to do when disabled */
  ਵਾਪਸ CURLE_OK;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ http_switch_ਮੰਜ਼ਿਲ_get(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ ਕੋਡ)
{
  ਅਚਲ ਅੱਖਰ *req = CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST);

  ਜੇ((req || ਅੰਕੜੇ->state.httpreq != HTTPREQ_GET) &&
     (ਅੰਕੜੇ->ਸੈੱਟ.http_follow_ਬਹੁਲਕ == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((ਖਾਲੀ)ਕੋਡ);
    infof(ਅੰਕੜੇ, "Switch to GET because of %d response", ਕੋਡ);
    ਅੰਕੜੇ->state.http_ignorecustom = TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(req && (ਅੰਕੜੇ->ਸੈੱਟ.http_follow_ਬਹੁਲਕ != CURLFOLLOW_FIRSTONLY))
    infof(ਅੰਕੜੇ, "Stick to %s instead of GET", req);

  ਅੰਕੜੇ->state.httpreq = HTTPREQ_GET;
  Curl_creader_ਸੈੱਟ_rewind(ਅੰਕੜੇ, FALSE);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPREQ_IS_POST(ਅੰਕੜੇ)                    \
  ((ਅੰਕੜੇ)->state.httpreq == HTTPREQ_POST ||      \
   (ਅੰਕੜੇ)->state.httpreq == HTTPREQ_POST_FORM || \
   (ਅੰਕੜੇ)->state.httpreq == HTTPREQ_POST_MIME)

ਸਥਿਰ CURLcode ਸੈੱਟ_auto_referer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLU *u;
  ਅੱਖਰ *referer = ਨਿਰਾ;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&ਅੰਕੜੇ->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  ਜੇ(!u)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  uc = curl_url_ਸੈੱਟ(u, CURLUPART_URL,
                    Curl_bufref_ptr(&ਅੰਕੜੇ->state.url), 0);
  ਜੇ(!uc)
    uc = curl_url_ਸੈੱਟ(u, CURLUPART_FRAGMENT, ਨਿਰਾ, 0);
  ਜੇ(!uc)
    uc = curl_url_ਸੈੱਟ(u, CURLUPART_USER, ਨਿਰਾ, 0);
  ਜੇ(!uc)
    uc = curl_url_ਸੈੱਟ(u, CURLUPART_PASSWORD, ਨਿਰਾ, 0);
  ਜੇ(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  ਜੇ(uc || !referer)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  Curl_bufref_ਸੈੱਟ(&ਅੰਕੜੇ->state.referer, referer, 0, curl_free);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode follow_parse_url(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *newurl,
                                 followtype type, ਅੱਖਰ **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(ਅੰਕੜੇ->state.uh);
  uc = curl_url_ਸੈੱਟ(ਅੰਕੜੇ->state.uh, CURLUPART_URL, newurl, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (ਅੰਕੜੇ->ਸੈੱਟ.ਰਸਤਾ_as_is ? CURLU_ਰਸਤਾ_AS_IS : 0)));
  *ucp = uc;
  ਜੇ(uc) {
    ਜੇ((uc == CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) || (type != FOLLOW_FAKE)) {
      failf(ਅੰਕੜੇ, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      ਵਾਪਸ Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    ਜੇ(!*follow_urlp)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਨਹੀਂਤਾਂ {
    CURLU *u = curl_url();
    ਜੇ(!u)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    uc = curl_url_ਸੈੱਟ(u, CURLUPART_URL,
                      Curl_bufref_ptr(&ਅੰਕੜੇ->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    ਜੇ(!uc)
      uc = curl_url_get(ਅੰਕੜੇ->state.uh, CURLUPART_URL, follow_urlp, 0);
    ਜੇ(uc) {
      curl_url_cleanup(u);
      ਵਾਪਸ Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
    }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, ਅੰਕੜੇ->state.uh);
      curl_url_cleanup(u);
      ਜੇ(!same_origin)
        Curl_auth_digest_cleanup(&ਅੰਕੜੇ->state.digest);
    }
#ਨਹੀਂਤਾਂ
    curl_url_cleanup(u);
#ਜੇ_ਅੰਤ
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode follow_fake(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  ਜੇ(!uc) {
    CURLU *u = curl_url();
    ਅੱਖਰ *nocred = ਨਿਰਾ;

    ਜੇ(!u) {
      curlx_free(follow_url);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਜੇ(!curl_url_ਸੈੱਟ(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (ਅੰਕੜੇ->ਸੈੱਟ.ਰਸਤਾ_as_is ? CURLU_ਰਸਤਾ_AS_IS : 0)) &&
       !curl_url_ਸੈੱਟ(u, CURLUPART_USER, ਨਿਰਾ, 0) &&
       !curl_url_ਸੈੱਟ(u, CURLUPART_PASSWORD, ਨਿਰਾ, 0))
      (ਖਾਲੀ)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_ਖਾਲੀ_ਥਾਵਾਂ);
    curl_url_cleanup(u);
    ਜੇ(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  ਅੰਕੜੇ->info.wouldredirect = follow_url;

  ਜੇ(reachedmax) {
    failf(ਅੰਕੜੇ, "Maximum (%d) redirects followed", ਅੰਕੜੇ->ਸੈੱਟ.maxredirs);
    ਵਾਪਸ CURLE_TOO_MANY_REDIRECTS;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool follow_switch_get(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  ਚੋਣ(ਅੰਕੜੇ->info.httpcode) {
  ਹਾਲਤ 301: /* Moved Permanently */
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
    ਜੇ(HTTPREQ_IS_POST(ਅੰਕੜੇ) && !ਅੰਕੜੇ->ਸੈੱਟ.post301) {
      http_switch_ਮੰਜ਼ਿਲ_get(ਅੰਕੜੇ, 301);
      ਵਾਪਸ TRUE;
    }
    ਰੋਕੋ;
  ਹਾਲਤ 302: /*ਮਿਲਿਆ*/
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
    ਜੇ(HTTPREQ_IS_POST(ਅੰਕੜੇ) && !ਅੰਕੜੇ->ਸੈੱਟ.post302) {
      http_switch_ਮੰਜ਼ਿਲ_get(ਅੰਕੜੇ, 302);
      ਵਾਪਸ TRUE;
    }
    ਰੋਕੋ;
  ਹਾਲਤ 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    ਜੇ(!HTTPREQ_IS_POST(ਅੰਕੜੇ) || !ਅੰਕੜੇ->ਸੈੱਟ.post303) {
      http_switch_ਮੰਜ਼ਿਲ_get(ਅੰਕੜੇ, 303);
      ਵਾਪਸ TRUE;
    }
    ਰੋਕੋ;

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
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ FALSE;
}

CURLcode Curl_http_follow(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  ਅੱਖਰ *follow_url = ਨਿਰਾ;
  CURLUcode uc;
  CURLcode ਨਤੀਜਾ;
  CURLcode rewind_ਨਤੀਜਾ;
  bool switch_ਮੰਜ਼ਿਲ_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  ਜੇ(type != FOLLOW_FAKE)
    ਅੰਕੜੇ->state.ਬੇਨਤੀਆਂ++; /* count all real follows */
  ਜੇ(type == FOLLOW_REDIR) {
    ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.maxredirs != -1) &&
       (ਅੰਕੜੇ->state.followlocation >= ਅੰਕੜੇ->ਸੈੱਟ.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    ਨਹੀਂਤਾਂ {
      ਅੰਕੜੇ->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.http_auto_referer) {
        ਨਤੀਜਾ = ਸੈੱਟ_auto_referer(ਅੰਕੜੇ);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }
    }
  }

  ਜੇ((type != FOLLOW_RETRY) &&
     (ਅੰਕੜੇ->req.httpcode != 401) && (ਅੰਕੜੇ->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, ਨਿਰਾ, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  ਨਤੀਜਾ = follow_parse_url(ਅੰਕੜੇ, newurl, type, &follow_url, &uc);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  DEBUGASSERT(follow_url);

  ਜੇ(type == FOLLOW_FAKE)
    ਵਾਪਸ follow_fake(ਅੰਕੜੇ, follow_url, uc, reachedmax);

  ਜੇ(disallowport)
    ਅੰਕੜੇ->state.allow_ਪੋਰਟ = FALSE;

  Curl_bufref_ਸੈੱਟ(&ਅੰਕੜੇ->state.url, follow_url, 0, curl_free);
  rewind_ਨਤੀਜਾ = Curl_req_soft_reset(&ਅੰਕੜੇ->req, ਅੰਕੜੇ);
  infof(ਅੰਕੜੇ, "Issue another request to this URL: '%s'", follow_url);
  ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.http_follow_ਬਹੁਲਕ == CURLFOLLOW_FIRSTONLY) &&
     !ਅੰਕੜੇ->state.http_ignorecustom &&
     CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST)) {
    ਅੰਕੜੇ->state.http_ignorecustom = TRUE;
    infof(ਅੰਕੜੇ, "Drop custom request method for next request");
  }

  switch_ਮੰਜ਼ਿਲ_get = follow_switch_get(ਅੰਕੜੇ);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  ਜੇ(rewind_ਨਤੀਜਾ && !switch_ਮੰਜ਼ਿਲ_get)
    ਵਾਪਸ rewind_ਨਤੀਜਾ;

  Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(ਅੰਕੜੇ);

  ਵਾਪਸ CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(ਅਚਲ ਅੱਖਰ *headerline, /* line to check */
                        ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ, /* header keyword _with_ colon */
                        ਅਚਲ size_t hlen, /* len of the keyword in bytes */
                        ਅਚਲ ਅੱਖਰ *content, /* content string to find */
                        ਅਚਲ size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  ਅਚਲ ਅੱਖਰ *p;
  ਬਣਤਰ Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(ਸਿਰਲੇਖ);
  DEBUGASSERT(content);

  ਜੇ(!curl_strnequal(headerline, ਸਿਰਲੇਖ, hlen))
    ਵਾਪਸ FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  ਜੇ(curlx_str_cspn(&p, &val, "\r\n"))
    ਵਾਪਸ FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  ਜੇ(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    ਲਈ(len = curlx_strlen(&val); len >= clen;) {
      ਬਣਤਰ Curl_str ਅਗਲਾ;
      ਅਚਲ ਅੱਖਰ *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      ਜੇ(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        ਵਾਪਸ TRUE; /* match! */
      /* advance to the next comma */
      ਜੇ(curlx_str_until(&p, &ਅਗਲਾ, len, ',') ||
         curlx_str_single(&p, ','))
        ਰੋਕੋ; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      ਕਰੋ
        curlx_str_passblanks(&p);
      ਜਦੋਂ(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      ਜੇ((size_t)(p - o) > len)
        ਰੋਕੋ;
      len -= (p - o);
    }
  }
  ਵਾਪਸ FALSE; /* no match */
}

ਬਣਤਰ cr_exp100_ctx {
  ਬਣਤਰ Curl_creader super;
  ਬਣਤਰ curltime ਸ਼ੁਰੂ; /* time started waiting */
  ਗਿਣਤੀ expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

ਸਥਿਰ ਖਾਲੀ http_exp100_continue(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_exp100_ctx *ctx = reader->ctx;
  ਜੇ(ctx->state > EXP100_SEND_ਅੰਕੜੇ) {
    ctx->state = EXP100_SEND_ਅੰਕੜੇ;
    Curl_expire_clear(ਅੰਕੜੇ, EXPIRE_100_TIMEOUT);
  }
}

ਸਥਿਰ CURLcode cr_exp100_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_creader *reader,
                               ਅੱਖਰ *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ਬਣਤਰ cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  ਚੋਣ(ctx->state) {
  ਹਾਲਤ EXP100_SENDING_REQUEST:
    ਜੇ(!Curl_req_sendbuf_ਖਾਲੀ_ਥਾਵਾਂ(ਅੰਕੜੇ)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(ਅੰਕੜੇ, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      ਵਾਪਸ CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(ਅੰਕੜੇ, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", ਅੰਕੜੇ->ਸੈੱਟ.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->ਸ਼ੁਰੂ = *Curl_pgrs_now(ਅੰਕੜੇ);
    Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_100_TIMEOUT,
                    ਅੰਕੜੇ->ਸੈੱਟ.expect_100_timeout, &ctx->ਸ਼ੁਰੂ);
    *nread = 0;
    *eos = FALSE;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ EXP100_FAILED:
    DEBUGF(infof(ਅੰਕੜੇ, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    ਵਾਪਸ CURLE_READ_ERROR;
  ਹਾਲਤ EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &ctx->ਸ਼ੁਰੂ);
    ਜੇ(ms < ਅੰਕੜੇ->ਸੈੱਟ.expect_100_timeout) {
      DEBUGF(infof(ਅੰਕੜੇ, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      ਵਾਪਸ CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(ਅੰਕੜੇ, reader);
    infof(ਅੰਕੜੇ, "Done waiting for 100-continue");
    FALLTHROUGH();
  ਮੂਲ:
    DEBUGF(infof(ਅੰਕੜੇ, "cr_exp100_read, pass through"));
    ਵਾਪਸ Curl_creader_read(ਅੰਕੜੇ, reader->ਅਗਲਾ, buf, blen, nread, eos);
  }
}

ਸਥਿਰ ਖਾਲੀ cr_exp100_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ Curl_creader *reader, ਪੂਰਨ premature)
{
  ਬਣਤਰ cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_ਅੰਕੜੇ;
  Curl_expire_clear(ਅੰਕੜੇ, EXPIRE_100_TIMEOUT);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_ਕੁੱਲ_ਲੰਮਾਈ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_ਪੂਰਾ,
  ਆਕਾਰ(ਬਣਤਰ cr_exp100_ctx)
};

ਸਥਿਰ CURLcode http_exp100_ਜੋੜੋ_reader(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *reader = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_creader_create(&reader, ਅੰਕੜੇ, &cr_exp100, CURL_CR_PROTOCOL);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_creader_ਜੋੜੋ(ਅੰਕੜੇ, reader);
  ਜੇ(!ਨਤੀਜਾ) {
    ਬਣਤਰ cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  ਜੇ(ਨਤੀਜਾ && reader)
    Curl_creader_free(ਅੰਕੜੇ, reader);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ http_exp100_got100(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *r = Curl_creader_get_by_type(ਅੰਕੜੇ, &cr_exp100);
  ਜੇ(r)
    http_exp100_continue(ਅੰਕੜੇ, r);
}

ਸਥਿਰ bool http_exp100_is_ਉਡੀਕ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *r = Curl_creader_get_by_type(ਅੰਕੜੇ, &cr_exp100);
  ਜੇ(r) {
    ਬਣਤਰ cr_exp100_ctx *ctx = r->ctx;
    ਵਾਪਸ ctx->state == EXP100_AWAITING_CONTINUE;
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਖਾਲੀ http_exp100_send_anyway(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *r = Curl_creader_get_by_type(ਅੰਕੜੇ, &cr_exp100);
  ਜੇ(r)
    http_exp100_continue(ਅੰਕੜੇ, r);
}

ਸਥਿਰ bool http_exp100_is_selected(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *r = Curl_creader_get_by_type(ਅੰਕੜੇ, &cr_exp100);
  ਵਾਪਸ !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ easy_pollset *ps)
{
  /* write mode */
  ਵਾਪਸ Curl_pollset_ਜੋੜੋ_ਬਾਹਰਲਾ(ਅੰਕੜੇ, ps, ਅੰਕੜੇ->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(CURL_REQ_WANT_RECV(ਅੰਕੜੇ)) {
    ਨਤੀਜਾ = Curl_pollset_ਜੋੜੋ_ਵਿੱਚ(ਅੰਕੜੇ, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  ਜੇ(!ਨਤੀਜਾ && Curl_req_want_send(ਅੰਕੜੇ) && !http_exp100_is_ਉਡੀਕ(ਅੰਕੜੇ)) {
    ਨਤੀਜਾ = Curl_pollset_ਜੋੜੋ_ਬਾਹਰਲਾ(ਅੰਕੜੇ, ps, conn->sock[FIRSTSOCKET]);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode http_write_ਸਿਰਲੇਖ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *hd, size_t hdlen)
{
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ, hd, hdlen);

  writetype = CLIENTWRITE_ਸਿਰਲੇਖ |
    ((ਅੰਕੜੇ->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, writetype, hd, hdlen);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_bump_headersize(ਅੰਕੜੇ, hdlen, FALSE);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਅੰਕੜੇ->req.deductheadercount = (100 <= ਅੰਕੜੇ->req.httpcode &&
                                 199 >= ਅੰਕੜੇ->req.httpcode) ?
    ਅੰਕੜੇ->req.headerbytecount : 0;
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        CURLcode ਸਥਿਤੀ, bool premature)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  ਅੰਕੜੇ->state.authhost.multipass = FALSE;
  ਅੰਕੜੇ->state.authproxy.multipass = FALSE;

  ਜੇ(curlx_dyn_len(&ਅੰਕੜੇ->state.headerb)) {
    (ਖਾਲੀ)http_write_ਸਿਰਲੇਖ(ਅੰਕੜੇ, curlx_dyn_ptr(&ਅੰਕੜੇ->state.headerb),
                            curlx_dyn_len(&ਅੰਕੜੇ->state.headerb));
  }
  curlx_dyn_reset(&ਅੰਕੜੇ->state.headerb);

  ਜੇ(ਸਥਿਤੀ)
    ਵਾਪਸ ਸਥਿਤੀ;

  ਜੇ(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->ਬਿੱਟ.retry &&
     !ਅੰਕੜੇ->ਸੈੱਟ.connect_only &&
     (ਅੰਕੜੇ->req.bytecount +
      ਅੰਕੜੇ->req.headerbytecount -
      ਅੰਕੜੇ->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(ਅੰਕੜੇ, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    ਵਾਪਸ CURLE_GOT_NOTHING;
  }

  ਵਾਪਸ CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
ਸਥਿਰ bool http_may_use_1_1(ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  ਜੇ(ਅੰਕੜੇ->state.http_neg.rcvd_min == 10)
    ਵਾਪਸ FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  ਜੇ(conn && conn->httpversion_ਵੇਖਿਆ == 10)
    ਵਾਪਸ FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  ਜੇ(ਅੰਕੜੇ->state.http_neg.only_10 &&
     (!conn || conn->httpversion_ਵੇਖਿਆ <= 10))
    ਵਾਪਸ FALSE;
  /* We are not restricted to use 1.0 only. */
  ਵਾਪਸ !ਅੰਕੜੇ->state.http_neg.only_10;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ http_request_ਸੰਸਕਰਣ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ v = Curl_conn_http_ਸੰਸਕਰਣ(ਅੰਕੜੇ, ਅੰਕੜੇ->conn);
  ਜੇ(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(ਅੰਕੜੇ) ? 11 : 10;
  }
  ਵਾਪਸ v;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *get_http_string(ਪੂਰਨ httpversion)
{
  ਚੋਣ(httpversion) {
  ਹਾਲਤ 30:
    ਵਾਪਸ "3";
  ਹਾਲਤ 20:
    ਵਾਪਸ "2";
  ਹਾਲਤ 11:
    ਵਾਪਸ "1.1";
  ਮੂਲ:
    ਵਾਪਸ "1.0";
  }
}

CURLcode Curl_ਜੋੜੋ_custom_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 bool is_connect, ਪੂਰਨ httpversion,
                                 ਬਣਤਰ dynbuf *req)
{
  ਬਣਤਰ curl_slist *h[2];
  ਬਣਤਰ curl_slist *ਸਿਰਲੇਖਾਂ;
  ਪੂਰਨ numlists = 1; /* by default */
  ਪੂਰਨ i;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਗਿਣਤੀ Curl_proxy_use proxy;

  ਜੇ(is_connect)
    proxy = ਸਿਰਲੇਖ_CONNECT;
  ਨਹੀਂਤਾਂ
    proxy = ਅੰਕੜੇ->conn->ਬਿੱਟ.origin_is_proxy ? ਸਿਰਲੇਖ_PROXY : ਸਿਰਲੇਖ_SERVER;

  ਚੋਣ(proxy) {
  ਹਾਲਤ ਸਿਰਲੇਖ_SERVER:
    h[0] = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ;
    ਰੋਕੋ;
  ਹਾਲਤ ਸਿਰਲੇਖ_PROXY:
    h[0] = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ;
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.sep_ਸਿਰਲੇਖਾਂ) {
      h[1] = ਅੰਕੜੇ->ਸੈੱਟ.proxyheaders;
      numlists++;
    }
    ਰੋਕੋ;
  ਹਾਲਤ ਸਿਰਲੇਖ_CONNECT:
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.sep_ਸਿਰਲੇਖਾਂ)
      h[0] = ਅੰਕੜੇ->ਸੈੱਟ.proxyheaders;
    ਨਹੀਂਤਾਂ
      h[0] = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ;
    ਰੋਕੋ;
  ਹਾਲਤ ਸਿਰਲੇਖ_CONNECT_UDP:
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.sep_ਸਿਰਲੇਖਾਂ)
      h[0] = ਅੰਕੜੇ->ਸੈੱਟ.proxyheaders;
    ਨਹੀਂਤਾਂ
      h[0] = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ;
    ਰੋਕੋ;
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)is_connect;
  h[0] = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ;
#ਜੇ_ਅੰਤ

  /* loop through one or two lists */
  ਲਈ(i = 0; i < numlists; i++) {
    ਲਈ(ਸਿਰਲੇਖਾਂ = h[i]; ਸਿਰਲੇਖਾਂ; ਸਿਰਲੇਖਾਂ = ਸਿਰਲੇਖਾਂ->ਅਗਲਾ) {
      CURLcode ਨਤੀਜਾ = CURLE_OK;
      bool blankheader = FALSE;
      ਬਣਤਰ Curl_str name;
      ਅਚਲ ਅੱਖਰ *p = ਸਿਰਲੇਖਾਂ->ਅੰਕੜੇ;
      ਅਚਲ ਅੱਖਰ *origp = p;
      size_t hlen = ਲੜੀ_ਲੰਬਾਈ(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      ਜੇ(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      ਨਹੀਂਤਾਂ {
        p = origp;
        ਜੇ(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          ਬਣਤਰ Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          ਜੇ(!curlx_strlen(&val))
            /* no content, do not send this */
            ਜਾਰੀ;
        }
        ਨਹੀਂਤਾਂ
          /* no colon */
          ਜਾਰੀ;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      ਜੇ(ਅੰਕੜੇ->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      ਨਹੀਂਤਾਂ ਜੇ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      ਨਹੀਂਤਾਂ ਜੇ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_ਮੰਜ਼ਿਲ_host(ਅੰਕੜੇ))
        ;
      ਨਹੀਂਤਾਂ ਜੇ(blankheader) {
        ਨਤੀਜਾ = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        ਜੇ(!ਨਤੀਜਾ)
          ਨਤੀਜਾ = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = curlx_dyn_addf(req, "%s\r\n", origp);

      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PARSEDATE
CURLcode Curl_ਜੋੜੋ_timecondition(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ dynbuf *req)
{
  ਅਚਲ ਬਣਤਰ tm *tm;
  ਬਣਤਰ tm keeptime;
  CURLcode ਨਤੀਜਾ;
  ਅੱਖਰ datestr[80];
  ਅਚਲ ਅੱਖਰ *condp;
  size_t len;

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    ਵਾਪਸ CURLE_OK;

  ਨਤੀਜਾ = curlx_gmtime(ਅੰਕੜੇ->ਸੈੱਟ.timevalue, &keeptime);
  ਜੇ(ਨਤੀਜਾ) {
    failf(ਅੰਕੜੇ, "Invalid TIMEVALUE");
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  tm = &keeptime;

  ਚੋਣ(ਅੰਕੜੇ->ਸੈੱਟ.timecondition) {
  ਮੂਲ:
    DEBUGF(infof(ਅੰਕੜੇ, "invalid time condition"));
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਹਾਲਤ CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    ਰੋਕੋ;
  }

  ਜੇ(Curl_checkheaders(ਅੰਕੜੇ, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    ਵਾਪਸ CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, ਆਕਾਰ(datestr),
                 "%s: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                 condp,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);

  ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(req, datestr);
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਨਹੀਂਤਾਂ
/* disabled */
CURLcode Curl_ਜੋੜੋ_timecondition(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ dynbuf *req)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)req;
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ Curl_http_ਵਿਧੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਅਚਲ ਅੱਖਰ **ਵਿਧੀ, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)ਅੰਕੜੇ->state.httpreq;
  ਅਚਲ ਅੱਖਰ *request;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS
  ਜੇ(ਅੰਕੜੇ->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  ਜੇ((ਅੰਕੜੇ->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     ਅੰਕੜੇ->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  ਜੇ(!ਅੰਕੜੇ->state.http_ignorecustom &&
     CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(ਅੰਕੜੇ->req.no_body)
      request = "HEAD";
    ਨਹੀਂਤਾਂ {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_ਸਿਰ));
      ਚੋਣ(httpreq) {
      ਹਾਲਤ HTTPREQ_POST:
      ਹਾਲਤ HTTPREQ_POST_FORM:
      ਹਾਲਤ HTTPREQ_POST_MIME:
        request = "POST";
        ਰੋਕੋ;
      ਹਾਲਤ HTTPREQ_PUT:
        request = "PUT";
        ਰੋਕੋ;
      ਮੂਲ: /* this should never happen */
      ਹਾਲਤ HTTPREQ_GET:
        request = "GET";
        ਰੋਕੋ;
      ਹਾਲਤ HTTPREQ_ਸਿਰ:
        request = "HEAD";
        ਰੋਕੋ;
      }
    }
  }
  *ਵਿਧੀ = request;
  *reqp = httpreq;
}

ਸਥਿਰ CURLcode http_ਸੈੱਟ_aptr_host(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਅਚਲ ਅੱਖਰ *ptr = ਨਿਰਾ;

  curlx_safefree(ਅੰਕੜੇ->state.http_host);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  curlx_safefree(ਅੰਕੜੇ->req.cookiehost);
#ਜੇ_ਅੰਤ

  ਜੇ(Curl_peer_equal(ਅੰਕੜੇ->state.initial_origin, ਅੰਕੜੇ->state.origin))
    ptr = Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Host"));

  ਜੇ(ptr) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    ਅੱਖਰ *cookiehost;
    CURLcode ਨਤੀਜਾ = copy_custom_ਮੁੱਲ(ptr, &cookiehost);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਜੇ(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    ਨਹੀਂਤਾਂ {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      ਜੇ(*cookiehost == '[') {
        ਅੱਖਰ *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, ਲੜੀ_ਲੰਬਾਈ(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        ਜੇ(closingbracket)
          *closingbracket = 0;
      }
      ਨਹੀਂਤਾਂ {
        ਪੂਰਨ startsearch = 0;
        ਅੱਖਰ *colon = strchr(cookiehost + startsearch, ':');
        ਜੇ(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      ਅੰਕੜੇ->req.cookiehost = cookiehost;
    }
#ਜੇ_ਅੰਤ

    ਜੇ(!curl_strequal("Host:", ptr)) {
      ਅੰਕੜੇ->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      ਜੇ(!ਅੰਕੜੇ->state.http_host)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }
  ਨਹੀਂਤਾਂ {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)ਅੰਕੜੇ->state.origin->ipv6;
    ਬਣਤਰ dynbuf tmp;
    size_t hlen;
    CURLcode ਨਤੀਜਾ;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    ਨਤੀਜਾ = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    ਜੇ(!ਨਤੀਜਾ && ipv6)
      ਨਤੀਜਾ = curlx_dyn_addn(&tmp, STRCONST("["));
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&tmp, ਅੰਕੜੇ->state.origin->hostname);
    ਜੇ(!ਨਤੀਜਾ && ipv6)
      ਨਤੀਜਾ = curlx_dyn_addn(&tmp, STRCONST("]"));
    ਜੇ(!ਨਤੀਜਾ &&
       ((ਅੰਕੜੇ->state.origin->ਪੋਰਟ != ਅੰਕੜੇ->state.origin->scheme->defport) ||
       (ਅੰਕੜੇ->state.origin->scheme->family != conn->scheme->family))) {
      ਨਤੀਜਾ = curlx_dyn_addf(&tmp, ":%u", ਅੰਕੜੇ->state.origin->ਪੋਰਟ);
    }

    ਅੰਕੜੇ->state.http_host = ਨਤੀਜਾ ? ਨਿਰਾ : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
ਸਥਿਰ CURLcode http_ਨਿਸ਼ਾਨਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ dynbuf *r)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ;
  ਅਚਲ ਅੱਖਰ *query = ਅੰਕੜੇ->state.ਉੱਪਰ.query;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
#ਜੇ_ਅੰਤ

  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਨਿਸ਼ਾਨਾ)) {
    ਰਸਤਾ = CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਨਿਸ਼ਾਨਾ);
    query = ਨਿਰਾ;
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ(conn->ਬਿੱਟ.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    ਅੱਖਰ *url;
    CURLU *h = curl_url_dup(ਅੰਕੜੇ->state.uh);
    ਜੇ(!h)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    ਜੇ(!ਅੰਕੜੇ->state.origin->ipv6 &&
       (ਅੰਕੜੇ->state.origin->user_hostname != ਅੰਕੜੇ->state.origin->hostname)) {
      uc = curl_url_ਸੈੱਟ(h, CURLUPART_HOST, ਅੰਕੜੇ->state.origin->hostname, 0);
      ਜੇ(uc) {
        curl_url_cleanup(h);
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
    }
    uc = curl_url_ਸੈੱਟ(h, CURLUPART_FRAGMENT, ਨਿਰਾ, 0);
    ਜੇ(uc) {
      curl_url_cleanup(h);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    ਜੇ(ਅੰਕੜੇ->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_ਸੈੱਟ(h, CURLUPART_USER, ਨਿਰਾ, 0);
      ਜੇ(uc) {
        curl_url_cleanup(h);
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      uc = curl_url_ਸੈੱਟ(h, CURLUPART_PASSWORD, ਨਿਰਾ, 0);
      ਜੇ(uc) {
        curl_url_cleanup(h);
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.creds && (ਅੰਕੜੇ->state.creds->ਸਰੋਤ != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_ਸੈੱਟ(h, CURLUPART_USER,
                        ਅੰਕੜੇ->state.creds->user, CURLU_URLENCODE);
      ਜੇ(!uc)
        uc = curl_url_ਸੈੱਟ(h, CURLUPART_PASSWORD,
                          ਅੰਕੜੇ->state.creds->passwd, CURLU_URLENCODE);
      ਜੇ(uc) {
        curl_url_cleanup(h);
        ਵਾਪਸ Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_ਪੋਰਟ);
    ਜੇ(uc) {
      curl_url_cleanup(h);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    curl_url_cleanup(h);

    /* target or URL */
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(r, CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਨਿਸ਼ਾਨਾ) ?
      CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਨਿਸ਼ਾਨਾ) : url);
    curlx_free(url);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਜੇ((ਅੰਕੜੇ->state.origin->scheme == &Curl_scheme_ftp) &&
       ਅੰਕੜੇ->ਸੈੱਟ.proxy_transfer_ਬਹੁਲਕ) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ);
      bool type_present = FALSE;
      ਜੇ((len >= 7) && !memcmp(&ਰਸਤਾ[len - 7], ";type=", 6)) {
        ਚੋਣ(Curl_raw_toupper(ਰਸਤਾ[len - 1])) {
        ਹਾਲਤ 'A':
        ਹਾਲਤ 'D':
        ਹਾਲਤ 'I':
          type_present = TRUE;
          ਰੋਕੋ;
        }
      }
      ਜੇ(!type_present) {
        ਨਤੀਜਾ = curlx_dyn_addf(r, ";type=%c",
                                ਅੰਕੜੇ->state.prefer_ascii ? 'a' : 'i');
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }
    }
  }

  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  {
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(r, ਰਸਤਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਜੇ(query)
      ਨਤੀਜਾ = curlx_dyn_addf(r, "?%s", query);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
ਸਥਿਰ CURLcode ਸੈੱਟ_post_reader(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, Curl_HttpReq httpreq)
{
  CURLcode ਨਤੀਜਾ;

  ਚੋਣ(httpreq) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MIME
  ਹਾਲਤ HTTPREQ_POST_MIME:
    ਅੰਕੜੇ->state.mimepost = ਅੰਕੜੇ->ਸੈੱਟ.mimepostp;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FORM_API
  ਹਾਲਤ HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    ਜੇ(!ਅੰਕੜੇ->state.formp) {
      ਅੰਕੜੇ->state.formp = curlx_calloc(1, ਆਕਾਰ(curl_mimepart));
      ਜੇ(!ਅੰਕੜੇ->state.formp)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      Curl_mime_cleanpart(ਅੰਕੜੇ->state.formp);
      ਨਤੀਜਾ = Curl_getformdata(ਅੰਕੜੇ, ਅੰਕੜੇ->state.formp, ਅੰਕੜੇ->ਸੈੱਟ.httppost,
                                ਅੰਕੜੇ->state.fread_func);
      ਜੇ(ਨਤੀਜਾ) {
        curlx_safefree(ਅੰਕੜੇ->state.formp);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਅੰਕੜੇ->state.mimepost = ਅੰਕੜੇ->state.formp;
    }
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਅੰਕੜੇ->state.mimepost = ਨਿਰਾ;
    ਰੋਕੋ;
  }

  ਚੋਣ(httpreq) {
  ਹਾਲਤ HTTPREQ_POST_FORM:
  ਹਾਲਤ HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MIME
    ਜੇ(ਅੰਕੜੇ->state.mimepost) {
      ਅਚਲ ਅੱਖਰ *cthdr = Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Content-Type"));

      /* Read and seek body only. */
      ਅੰਕੜੇ->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      ਜੇ(cthdr)
        ਲਈ(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.mimepost->ਕਿਸਮ == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ->state.mimepost, ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ, 0);
      ਨਤੀਜਾ = Curl_mime_prepare_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, ਅੰਕੜੇ->state.mimepost, cthdr,
                                         ਨਿਰਾ, MIMESTRATEGY_FORM);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      curl_mime_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ->state.mimepost, ਨਿਰਾ, 0);
      ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_mime(ਅੰਕੜੇ, ਅੰਕੜੇ->state.mimepost);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
      ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);
    }
    ਅੰਕੜੇ->state.infilesize = Curl_creader_ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ);
    ਵਾਪਸ ਨਤੀਜਾ;

  ਮੂਲ:
    ਵਾਪਸ Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);
  }
  /* never reached */
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode ਸੈੱਟ_reader(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, Curl_HttpReq httpreq)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curl_off_t postsize = ਅੰਕੜੇ->state.infilesize;

  DEBUGASSERT(ਅੰਕੜੇ->conn);

  ਜੇ(ਅੰਕੜੇ->req.authneg) {
    ਵਾਪਸ Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);
  }

  ਚੋਣ(httpreq) {
  ਹਾਲਤ HTTPREQ_PUT: /* Let's PUT the data to the server! */
    ਵਾਪਸ postsize ? Curl_creader_ਸੈੱਟ_fread(ਅੰਕੜੇ, postsize) :
      Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);

#ਜੇ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ਹਾਲਤ HTTPREQ_POST_FORM:
  ਹਾਲਤ HTTPREQ_POST_MIME:
    ਵਾਪਸ ਸੈੱਟ_post_reader(ਅੰਕੜੇ, httpreq);
#ਜੇ_ਅੰਤ

  ਹਾਲਤ HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    ਜੇ(!postsize) {
      ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      ਜੇ(plen == SIZE_MAX)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਨਹੀਂਤਾਂ ਜੇ(plen)
        ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_buf(ਅੰਕੜੇ, ਅੰਕੜੇ->ਸੈੱਟ.postfields, plen);
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);
    }
    ਨਹੀਂਤਾਂ {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      ਅੱਖਰ *ptr = Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Transfer-Encoding"));
      ਜੇ(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_fread(ਅੰਕੜੇ, chunked ? -1 : postsize);
    }
    ਵਾਪਸ ਨਤੀਜਾ;

  ਮੂਲ:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    ਅੰਕੜੇ->state.infilesize = 0;
    ਵਾਪਸ Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);
  }
  /* not reached */
}

ਸਥਿਰ CURLcode http_resume(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, Curl_HttpReq httpreq)
{
  ਜੇ((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     ਅੰਕੜੇ->state.resume_from) {
    failf(ਅੰਕੜੇ, "HTTP upload cannot be resumed");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode http_req_ਸੈੱਟ_TE(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ dynbuf *req,
                                ਪੂਰਨ httpversion)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ptr;

  ptr = Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Transfer-Encoding"));
  ਜੇ(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    ਅੰਕੜੇ->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    ਜੇ(ਅੰਕੜੇ->req.upload_chunky && (httpversion >= 20)) {
      infof(ਅੰਕੜੇ, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      ਅੰਕੜੇ->req.upload_chunky = FALSE;
    }
  }
  ਨਹੀਂਤਾਂ {
    curl_off_t req_clen = Curl_creader_ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ);

    ਜੇ(req_clen < 0) {
      /* indeterminate request content length */
      ਜੇ(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        ਅੰਕੜੇ->req.upload_chunky = (httpversion < 20);
      }
      ਨਹੀਂਤਾਂ {
        failf(ਅੰਕੜੇ, "Chunky upload is not supported by HTTP 1.0");
        ਵਾਪਸ CURLE_UPLOAD_FAILED;
      }
    }
    ਨਹੀਂਤਾਂ {
      /* else, no chunky upload */
      ਅੰਕੜੇ->req.upload_chunky = FALSE;
    }

    ਜੇ(ਅੰਕੜੇ->req.upload_chunky)
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(req, "Transfer-Encoding: chunked\r\n");
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode addexpect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ dynbuf *r,
                          ਪੂਰਨ httpversion, bool *announced_exp100)
{
  CURLcode ਨਤੀਜਾ;
  ਅੱਖਰ *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  ਜੇ(ਅੰਕੜੇ->req.upgr101 != UPGR101_NONE)
    ਵਾਪਸ CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Expect"));
  ਜੇ(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਅੰਕੜੇ->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_ਲੰਮਾਈ(ਅੰਕੜੇ);
    ਜੇ(client_len > EXPECT_100_ਹੱਦ || client_len < 0) {
      ਨਤੀਜਾ = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      *announced_exp100 = TRUE;
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode http_ਜੋੜੋ_content_hds(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ dynbuf *r,
                                     ਪੂਰਨ httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(ਅੰਕੜੇ->conn);
  ਜੇ(ਅੰਕੜੇ->req.upload_chunky) {
    ਨਤੀਜਾ = Curl_httpchunk_ਜੋੜੋ_reader(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ);
  ਚੋਣ(httpreq) {
  ਹਾਲਤ HTTPREQ_PUT:
  ਹਾਲਤ HTTPREQ_POST:
#ਜੇ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ਹਾਲਤ HTTPREQ_POST_FORM:
  ਹਾਲਤ HTTPREQ_POST_MIME:
#ਜੇ_ਅੰਤ
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    ਜੇ(req_clen >= 0 && !ਅੰਕੜੇ->req.upload_chunky &&
       (ਅੰਕੜੇ->req.authneg ||
        !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      ਨਤੀਜਾ = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    ਜੇ(ਅੰਕੜੇ->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      ਬਣਤਰ curl_slist *hdr;

      ਲਈ(hdr = ਅੰਕੜੇ->state.mimepost->curlheaders; hdr; hdr = hdr->ਅਗਲਾ) {
        ਨਤੀਜਾ = curlx_dyn_addf(r, "%s\r\n", hdr->ਅੰਕੜੇ);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
#ਜੇ_ਅੰਤ
    ਜੇ(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Content-Type"))) {
      ਨਤੀਜਾ = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਤੀਜਾ = addexpect(ਅੰਕੜੇ, r, httpversion, &announced_exp100);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }

  Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, req_clen);
  ਜੇ(announced_exp100)
    ਨਤੀਜਾ = http_exp100_ਜੋੜੋ_reader(ਅੰਕੜੇ);

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES

ਸਥਿਰ CURLcode http_cookies(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ dynbuf *r)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *addcookies = ਨਿਰਾ;
  bool linecap = FALSE;
  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_COOKIE) &&
     !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Cookie")) &&
     Curl_auth_allowed_ਮੰਜ਼ਿਲ_host(ਅੰਕੜੇ))
    addcookies = CURL_EASY_STR(ਅੰਕੜੇ, STRING_COOKIE);

  ਜੇ(ਅੰਕੜੇ->cookies || addcookies) {
    ਬਣਤਰ Curl_llist list;
    ਪੂਰਨ ਗਿਣਤੀ_2 = 0;

    ਜੇ(ਅੰਕੜੇ->cookies && ਅੰਕੜੇ->state.cookie_engine) {
      bool okay;
      ਅਚਲ ਅੱਖਰ *host = ਅੰਕੜੇ->req.cookiehost ?
        ਅੰਕੜੇ->req.cookiehost : ਅੰਕੜੇ->state.origin->hostname;
      Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE, CURL_ਤਾਲਾ_ACCESS_SINGLE);
      ਨਤੀਜਾ = Curl_cookie_getlist(ਅੰਕੜੇ, &okay, host, &list);
      ਜੇ(!ਨਤੀਜਾ && okay) {
        ਬਣਤਰ Curl_llist_ਗੰਢ *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        ਲਈ(n = Curl_llist_ਸਿਰ(&list); n; n = Curl_ਗੰਢ_ਅਗਲਾ(n)) {
          ਬਣਤਰ Cookie *co = Curl_ਗੰਢ_elem(n);
          ਜੇ(co->ਮੁੱਲ) {
            size_t ਜੋੜੋ;
            ਜੇ(!ਗਿਣਤੀ_2) {
              ਨਤੀਜਾ = curlx_dyn_addn(r, STRCONST("Cookie: "));
              ਜੇ(ਨਤੀਜਾ)
                ਰੋਕੋ;
            }
            ਜੋੜੋ = ਲੜੀ_ਲੰਬਾਈ(co->name) + ਲੜੀ_ਲੰਬਾਈ(co->ਮੁੱਲ) + 1;
            ਜੇ(clen + ਜੋੜੋ >= MAX_COOKIE_ਸਿਰਲੇਖ_LEN) {
              infof(ਅੰਕੜੇ, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              ਰੋਕੋ;
            }
            ਨਤੀਜਾ = curlx_dyn_addf(r, "%s%s=%s", ਗਿਣਤੀ_2 ? "; " : "",
                                    co->name, co->ਮੁੱਲ);
            ਜੇ(ਨਤੀਜਾ)
              ਰੋਕੋ;
            clen += ਜੋੜੋ + (ਗਿਣਤੀ_2 ? 2 : 0);
            ਗਿਣਤੀ_2++;
          }
        }
        Curl_llist_destroy(&list, ਨਿਰਾ);
      }
      Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE);
    }
    ਜੇ(addcookies && !ਨਤੀਜਾ && !linecap) {
      ਜੇ(!ਗਿਣਤੀ_2)
        ਨਤੀਜਾ = curlx_dyn_addn(r, STRCONST("Cookie: "));
      ਜੇ(!ਨਤੀਜਾ) {
        ਨਤੀਜਾ = curlx_dyn_addf(r, "%s%s", ਗਿਣਤੀ_2 ? "; " : "", addcookies);
        ਗਿਣਤੀ_2++;
      }
    }
    ਜੇ(ਗਿਣਤੀ_2 && !ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(r, STRCONST("\r\n"));

    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ http_cookies(a, b) CURLE_OK
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode http_range(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           Curl_HttpReq httpreq)
{
  ਜੇ(ਅੰਕੜੇ->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    ਜੇ(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_ਸਿਰ)) &&
       !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(ਅੰਕੜੇ->state.rangeline);
      ਅੰਕੜੇ->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 ਅੰਕੜੇ->state.range);
      ਜੇ(!ਅੰਕੜੇ->state.rangeline)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(ਅੰਕੜੇ->state.rangeline);

      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ਸੈੱਟ_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        ਅੰਕੜੇ->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t ਕੁੱਲ_len = ਅੰਕੜੇ->req.authneg ?
                               ਅੰਕੜੇ->state.infilesize :
                               (ਅੰਕੜੇ->state.resume_from + req_clen);
        ਅੰਕੜੇ->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        ਅੰਕੜੇ->state.range, ਕੁੱਲ_len - 1, ਕੁੱਲ_len);
      }
      ਨਹੀਂਤਾਂ {
        /* Range was selected and then we pass the incoming range and append
           total size */
        ਅੰਕੜੇ->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        ਅੰਕੜੇ->state.range, req_clen);
      }
      ਜੇ(!ਅੰਕੜੇ->state.rangeline)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode http_firstwrite(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;

  ਜੇ(ਅੰਕੜੇ->req.newurl) {
    ਜੇ(conn->ਬਿੱਟ.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(ਅੰਕੜੇ);
      k->ਪੂਰਾ = TRUE;
      ਵਾਪਸ CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(ਅੰਕੜੇ, "Ignoring the response-body");
  }
  ਜੇ(ਅੰਕੜੇ->state.resume_from && !k->content_range &&
     (ਅੰਕੜੇ->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    ਜੇ(k->ਮਾਪ == ਅੰਕੜੇ->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(ਅੰਕੜੇ, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(ਅੰਕੜੇ);
      k->ਪੂਰਾ = TRUE;
      ਵਾਪਸ CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(ਅੰਕੜੇ, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    ਵਾਪਸ CURLE_RANGE_ERROR;
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.timecondition && !ਅੰਕੜੇ->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(ਅੰਕੜੇ, k->timeofdoc)) {
    k->ਪੂਰਾ = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    ਅੰਕੜੇ->info.httpcode = 304;
    infof(ਅੰਕੜੇ, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    ਵਾਪਸ CURLE_OK;
  } /* we have a time condition */

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode http_check_new_conn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਅਚਲ ਅੱਖਰ *info_ਸੰਸਕਰਣ = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *alpn;
  CURLcode ਨਤੀਜਾ;

  alpn = Curl_conn_get_alpn_negotiated(ਅੰਕੜੇ, conn);
  ਜੇ(alpn && !strcmp("h3", alpn)) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    ਜੇ(!conn->ਬਿੱਟ.origin_is_proxy)
#ਜੇ_ਅੰਤ
      DEBUGASSERT(Curl_conn_http_ਸੰਸਕਰਣ(ਅੰਕੜੇ, conn) == 30);
    info_ਸੰਸਕਰਣ = "HTTP/3";
  }
  ਨਹੀਂਤਾਂ ਜੇ(alpn && !strcmp("h2", alpn)) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    ਜੇ((Curl_conn_http_ਸੰਸਕਰਣ(ਅੰਕੜੇ, conn) != 20) &&
       conn->ਬਿੱਟ.origin_is_proxy) {
      ਨਤੀਜਾ = Curl_http2_switch(ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    DEBUGASSERT(Curl_conn_http_ਸੰਸਕਰਣ(ਅੰਕੜੇ, conn) == 20);
    info_ਸੰਸਕਰਣ = "HTTP/2";
  }
  ਨਹੀਂਤਾਂ {
    /* Check if user wants to use HTTP/2 with clear TCP */
    ਜੇ(Curl_http2_may_switch(ਅੰਕੜੇ)) {
      DEBUGF(infof(ਅੰਕੜੇ, "HTTP/2 over clean TCP"));
      ਨਤੀਜਾ = Curl_http2_switch(ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      info_ਸੰਸਕਰਣ = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_ਵੇਖਿਆ = 20;
      Curl_conn_ਸੈੱਟ_multiplex(conn);
    }
    ਨਹੀਂਤਾਂ
      info_ਸੰਸਕਰਣ = "HTTP/1.x";
  }

  ਜੇ(info_ਸੰਸਕਰਣ)
    infof(ਅੰਕੜੇ, "using %s", info_ਸੰਸਕਰਣ);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode http_ਜੋੜੋ_connection_hd(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ dynbuf *req)
{
  ਬਣਤਰ curl_slist *ਸਿਰ;
  ਅਚਲ ਅੱਖਰ *sep = "Connection: ";
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  ਲਈ(ਸਿਰ = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ; ਸਿਰ; ਸਿਰ = ਸਿਰ->ਅਗਲਾ) {
    ਜੇ(curl_strnequal(ਸਿਰ->ਅੰਕੜੇ, "Connection", 10) &&
       Curl_headersep(ਸਿਰ->ਅੰਕੜੇ[10]) &&
       !http_ਸਿਰਲੇਖ_is_ਖਾਲੀ_ਥਾਵਾਂ(ਸਿਰ->ਅੰਕੜੇ)) {
      ਅੱਖਰ *ਮੁੱਲ;
      ਨਤੀਜਾ = copy_custom_ਮੁੱਲ(ਸਿਰ->ਅੰਕੜੇ, &ਮੁੱਲ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      ਨਤੀਜਾ = curlx_dyn_addf(req, "%s%s", sep, ਮੁੱਲ);
      sep = ", ";
      curlx_free(ਮੁੱਲ);
      ਰੋਕੋ; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  ਜੇ(!ਨਤੀਜਾ && ਅੰਕੜੇ->state.http_hd_te) {
    ਨਤੀਜਾ = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  ਜੇ(!ਨਤੀਜਾ && ਅੰਕੜੇ->state.http_hd_upgrade) {
    ਨਤੀਜਾ = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  ਜੇ(!ਨਤੀਜਾ && ਅੰਕੜੇ->state.http_hd_h2_settings) {
    ਨਤੀਜਾ = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  ਜੇ(!ਨਤੀਜਾ && (rlen < curlx_dyn_len(req)))
    ਨਤੀਜਾ = curlx_dyn_addn(req, STRCONST("\r\n"));
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  ਲਈ(ਸਿਰ = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ; ਸਿਰ; ਸਿਰ = ਸਿਰ->ਅਗਲਾ) {
    ਜੇ(curl_strnequal(ਸਿਰ->ਅੰਕੜੇ, "Connection", 10) &&
       Curl_headersep(ਸਿਰ->ਅੰਕੜੇ[10]) &&
       !http_ਸਿਰਲੇਖ_is_ਖਾਲੀ_ਥਾਵਾਂ(ਸਿਰ->ਅੰਕੜੇ)) {
      ਜੇ(skip) {
        skip = FALSE;
        ਜਾਰੀ;
      }
      ਨਤੀਜਾ = curlx_dyn_addf(req, "%s\r\n", ਸਿਰ->ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  ਵਾਪਸ CURLE_OK;
}

/* Header identifier in order we send them by default */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  H1_HD_REQUEST,
  H1_HD_HOST,
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  H1_HD_PROXY_AUTH,
#ਜੇ_ਅੰਤ
  H1_HD_AUTH,
  H1_HD_RANGE,
  H1_HD_USER_AGENT,
  H1_HD_ACCEPT,
  H1_HD_TE,
  H1_HD_ACCEPT_ENCODING,
  H1_HD_REFERER,
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  H1_HD_PROXY_CONNECTION,
#ਜੇ_ਅੰਤ
  H1_HD_TRANSFER_ENCODING,
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ALTSVC
  H1_HD_ALT_ਵਰਤਿਆ,
#ਜੇ_ਅੰਤ
  H1_HD_UPGRADE,
  H1_HD_COOKIES,
  H1_HD_CONDITIONALS,
  H1_HD_CUSTOM,
  H1_HD_CONTENT,
  H1_HD_CONNECTION,
  H1_HD_ਆਖਰੀ  /* the last, empty header line */
} http_hd_t;

ਸਥਿਰ CURLcode http_ਜੋੜੋ_hd(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ dynbuf *req,
                            http_hd_t id,
                            ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ httpversion,
                            ਅਚਲ ਅੱਖਰ *ਵਿਧੀ,
                            Curl_HttpReq httpreq)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
#ਜੇ !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
#ਜੇ_ਅੰਤ
  ਚੋਣ(id) {
  ਹਾਲਤ H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    ਨਤੀਜਾ = curlx_dyn_addf(req, "%s ", ਵਿਧੀ);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = http_ਨਿਸ਼ਾਨਾ(ਅੰਕੜੇ, req);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    ਰੋਕੋ;

  ਹਾਲਤ H1_HD_HOST:
    ਜੇ(ਅੰਕੜੇ->state.http_host) {
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(req, ਅੰਕੜੇ->state.http_host);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    ਰੋਕੋ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਹਾਲਤ H1_HD_PROXY_AUTH:
    ਜੇ(ਅੰਕੜੇ->req.hd_proxy_auth)
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(req, ਅੰਕੜੇ->req.hd_proxy_auth);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

  ਹਾਲਤ H1_HD_AUTH:
    ਜੇ(ਅੰਕੜੇ->req.hd_auth)
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(req, ਅੰਕੜੇ->req.hd_auth);
    ਰੋਕੋ;

  ਹਾਲਤ H1_HD_RANGE:
    ਜੇ(ਅੰਕੜੇ->state.use_range && ਅੰਕੜੇ->state.rangeline)
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(req, ਅੰਕੜੇ->state.rangeline);
    ਰੋਕੋ;

  ਹਾਲਤ H1_HD_USER_AGENT: {
    ਅਚਲ ਅੱਖਰ *ua = CURL_EASY_STR(ਅੰਕੜੇ, STRING_USERAGENT);
    ਜੇ(ua && *ua && !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("User-Agent")))
      ਨਤੀਜਾ = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    ਰੋਕੋ;
  }

  ਹਾਲਤ H1_HD_ACCEPT:
    ਜੇ(!Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Accept")))
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(req, "Accept: */*\r\n");
    ਰੋਕੋ;

  ਹਾਲਤ H1_HD_TE:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LIBZ
    ਜੇ(!Curl_checkheaders(ਅੰਕੜੇ, STRCONST("TE")) &&
       ਅੰਕੜੇ->ਸੈੱਟ.http_transfer_encoding) {
      ਅੰਕੜੇ->state.http_hd_te = TRUE;
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(req, "TE: gzip\r\n");
    }
#ਜੇ_ਅੰਤ
    ਰੋਕੋ;

  ਹਾਲਤ H1_HD_ACCEPT_ENCODING: {
    ਅਚਲ ਅੱਖਰ *enc = CURL_EASY_STR(ਅੰਕੜੇ, STRING_ENCODING);
    ਜੇ(enc && !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Accept-Encoding")))
      ਨਤੀਜਾ = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    ਰੋਕੋ;
  }

  ਹਾਲਤ H1_HD_REFERER:
    ਜੇ(Curl_bufref_ptr(&ਅੰਕੜੇ->state.referer) &&
       !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Referer")))
      ਨਤੀਜਾ = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&ਅੰਕੜੇ->state.referer));
    ਰੋਕੋ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਹਾਲਤ H1_HD_PROXY_CONNECTION:
    ਜੇ(conn->ਬਿੱਟ.origin_is_proxy &&
       !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, STRCONST("Proxy-Connection")))
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(req, "Proxy-Connection: Keep-Alive\r\n");
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

  ਹਾਲਤ H1_HD_TRANSFER_ENCODING:
    ਨਤੀਜਾ = http_req_ਸੈੱਟ_TE(ਅੰਕੜੇ, req, httpversion);
    ਰੋਕੋ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ALTSVC
  ਹਾਲਤ H1_HD_ALT_ਵਰਤਿਆ:
    ਜੇ(conn->ਬਿੱਟ.altused && conn->ਰਾਹੀਂ_peer &&
       !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Alt-Used")))
      ਨਤੀਜਾ = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->ਰਾਹੀਂ_peer->hostname, conn->ਰਾਹੀਂ_peer->ਪੋਰਟ);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

  ਹਾਲਤ H1_HD_UPGRADE:
    ਜੇ(!Curl_conn_is_ssl(ਅੰਕੜੇ->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (ਅੰਕੜੇ->state.http_neg.wanted & CURL_HTTP_V2x) &&
       ਅੰਕੜੇ->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      ਨਤੀਜਾ = Curl_http2_request_upgrade(req, ਅੰਕੜੇ);
    }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS
    ਜੇ(!ਨਤੀਜਾ && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      ਨਤੀਜਾ = Curl_ws_request(ਅੰਕੜੇ, req);
#ਜੇ_ਅੰਤ
    ਰੋਕੋ;

  ਹਾਲਤ H1_HD_COOKIES:
    ਨਤੀਜਾ = http_cookies(ਅੰਕੜੇ, req);
    ਰੋਕੋ;

  ਹਾਲਤ H1_HD_CONDITIONALS:
    ਨਤੀਜਾ = Curl_ਜੋੜੋ_timecondition(ਅੰਕੜੇ, req);
    ਰੋਕੋ;

  ਹਾਲਤ H1_HD_CUSTOM:
    ਨਤੀਜਾ = Curl_ਜੋੜੋ_custom_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, FALSE, httpversion, req);
    ਰੋਕੋ;

  ਹਾਲਤ H1_HD_CONTENT:
    ਨਤੀਜਾ = http_ਜੋੜੋ_content_hds(ਅੰਕੜੇ, req, httpversion, httpreq);
    ਰੋਕੋ;

  ਹਾਲਤ H1_HD_CONNECTION: {
    ਨਤੀਜਾ = http_ਜੋੜੋ_connection_hd(ਅੰਕੜੇ, req);
    ਰੋਕੋ;
  }

  ਹਾਲਤ H1_HD_ਆਖਰੀ:
    ਨਤੀਜਾ = curlx_dyn_addn(req, STRCONST("\r\n"));
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  Curl_HttpReq httpreq;
  ਅਚਲ ਅੱਖਰ *ਵਿਧੀ;
  ਬਣਤਰ dynbuf req;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ httpversion;
  size_t hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *ਪੂਰਾ = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&ਅੰਕੜੇ->state.headerb);
  ਅੰਕੜੇ->state.maybe_folded = FALSE;

  ਜੇ(!ਅੰਕੜੇ->conn->ਬਿੱਟ.reuse) {
    ਨਤੀਜਾ = http_check_new_conn(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  ਨਤੀਜਾ = Curl_ਸਿਰਲੇਖਾਂ_init(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਅੰਕੜੇ->state.http_hd_te = FALSE;
  ਅੰਕੜੇ->state.http_hd_upgrade = FALSE;
  ਅੰਕੜੇ->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_ਵਿਧੀ(ਅੰਕੜੇ, &ਵਿਧੀ, &httpreq);

  /* select host to send */
  ਨਤੀਜਾ = http_ਸੈੱਟ_aptr_host(ਅੰਕੜੇ);
  /* setup the authentication headers, how that method and host are known */
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_http_output_auth(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, ਵਿਧੀ, httpreq,
                                   ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ,
                                   ਅੰਕੜੇ->state.ਉੱਪਰ.query, FALSE);
  /* Setup input reader, resume information and ranges */
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = ਸੈੱਟ_reader(ਅੰਕੜੇ, httpreq);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = http_resume(ਅੰਕੜੇ, httpreq);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = http_range(ਅੰਕੜੇ, httpreq);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  httpversion = http_request_ਸੰਸਕਰਣ(ਅੰਕੜੇ);
  /* Add request line and all headers to `req` */
  ਲਈ(hd_id = 0; hd_id <= H1_HD_ਆਖਰੀ; ++hd_id) {
    ਨਤੀਜਾ = http_ਜੋੜੋ_hd(ਅੰਕੜੇ, &req, (http_hd_t)hd_id,
                         httpversion, ਵਿਧੀ, httpreq);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_ਤਿਆਰੀ_sendrecv(ਅੰਕੜੇ, FIRSTSOCKET, -1);
  ਨਤੀਜਾ = Curl_req_send(ਅੰਕੜੇ, &req, httpversion);

  ਜੇ((httpversion >= 20) && ਅੰਕੜੇ->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    ਅੰਕੜੇ->req.upload_chunky = FALSE;

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ == CURLE_TOO_LARGE)
    failf(ਅੰਕੜੇ, "HTTP request too large");

  curlx_dyn_free(&req);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  ਸਥਿਤੀ_UNKNOWN, /* not enough data to tell yet */
  ਸਥਿਤੀ_ਪੂਰਾ, /* a status line was read */
  ਸਥਿਤੀ_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
ਸਥਿਰ bool checkprefixmax(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਬਫਰ, size_t len)
{
  size_t ch = CURLMIN(ਲੜੀ_ਲੰਬਾਈ(ਅਗੇਤਰ), len);
  ਵਾਪਸ curl_strnequal(ਅਗੇਤਰ, ਬਫਰ, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
ਸਥਿਰ statusline checkhttpprefix(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *s, size_t len)
{
  ਬਣਤਰ curl_slist *ਸਿਰ = ਅੰਕੜੇ->ਸੈੱਟ.http200aliases;
  statusline rc = ਸਥਿਤੀ_BAD;
  statusline onmatch = len >= 5 ? ਸਥਿਤੀ_ਪੂਰਾ : ਸਥਿਤੀ_UNKNOWN;

  ਜਦੋਂ(ਸਿਰ) {
    ਜੇ(checkprefixmax(ਸਿਰ->ਅੰਕੜੇ, s, len)) {
      rc = onmatch;
      ਰੋਕੋ;
    }
    ਸਿਰ = ਸਿਰ->ਅਗਲਾ;
  }

  ਜੇ((rc != ਸਥਿਤੀ_ਪੂਰਾ) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  ਵਾਪਸ rc;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
ਸਥਿਰ statusline checkrtspprefix(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *s, size_t len)
{
  statusline ਸਥਿਤੀ = ਸਥਿਤੀ_BAD;
  statusline onmatch = len >= 5 ? ਸਥਿਤੀ_ਪੂਰਾ : ਸਥਿਤੀ_UNKNOWN;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(checkprefixmax("RTSP/", s, len))
    ਸਥਿਤੀ = onmatch;

  ਵਾਪਸ ਸਥਿਤੀ;
}
#ਜੇ_ਅੰਤ /* CURL_DISABLE_RTSP */

ਸਥਿਰ statusline checkprotoprefix(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ connectdata *conn,
                                   ਅਚਲ ਅੱਖਰ *s, size_t len)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  ਜੇ(conn->scheme->protocol & CURLPROTO_RTSP)
    ਵਾਪਸ checkrtspprefix(ਅੰਕੜੇ, s, len);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)conn;
#ਜੇ_ਅੰਤ /* CURL_DISABLE_RTSP */

  ਵਾਪਸ checkhttpprefix(ਅੰਕੜੇ, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HD_IS(hd, hdlen, n) \
  (((hdlen) >= (ਆਕਾਰ(n) - 1)) && curl_strnequal(n, hd, ਆਕਾਰ(n) - 1))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (ਆਕਾਰ(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, ਆਕਾਰ(n) - 1)) ? ((hd) + (ਆਕਾਰ(n) - 1)) : ਨਿਰਾ)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((ਆਕਾਰ(n) - 1) + (ਆਕਾਰ(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
ਸਥਿਰ CURLcode http_ਸਿਰਲੇਖ_a(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *hd, size_t hdlen)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ALTSVC
  ਅਚਲ ਅੱਖਰ *v;
  v = (ਅੰਕੜੇ->asi &&
       (Curl_xfer_is_secure(ਅੰਕੜੇ) ||
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#ਨਹੀਂਤਾਂ
        0
#ਜੇ_ਅੰਤ
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : ਨਿਰਾ;
  ਜੇ(v) {
    /* the ALPN of the current request */
    ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
    ਗਿਣਤੀ alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    ਵਾਪਸ Curl_altsvc_parse(ਅੰਕੜੇ, ਅੰਕੜੇ->asi, v, ਅੰਕੜੇ->state.origin, id);
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)hd;
  (ਖਾਲੀ)hdlen;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
ਸਥਿਰ CURLcode http_ਸਿਰਲੇਖ_c(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *hd, size_t hdlen)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  ਅਚਲ ਅੱਖਰ *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !ਅੰਕੜੇ->ਸੈੱਟ.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : ਨਿਰਾ;
  ਜੇ(v) {
    ਕਰੋ {
      curl_off_t contentlength;
      ਪੂਰਨ offt = curlx_str_numblanks(&v, &contentlength);

      ਜੇ(offt == STRE_ਉਛਾਲ) {
        /* out of range */
        ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.max_filesize) {
          failf(ਅੰਕੜੇ, "Maximum file size exceeded");
          ਵਾਪਸ CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(ਅੰਕੜੇ, "Overflow Content-Length: value");
        ਵਾਪਸ CURLE_OK;
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ((offt == STRE_OK) &&
           ((k->ਮਾਪ == -1) || /* not set to something before */
            (k->ਮਾਪ == contentlength))) { /* or the same value */

          k->ਮਾਪ = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          ਜੇ(!curlx_str_single(&v, ','))
            ਜਾਰੀ;

          ਜੇ(!curlx_str_newline(&v)) {
            k->maxdownload = k->ਮਾਪ;
            ਵਾਪਸ CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(ਅੰਕੜੇ, "Invalid Content-Length: value");
        ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
      }
    } ਜਦੋਂ(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(ਅੰਕੜੇ, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : ਨਿਰਾ;
  ਜੇ(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    ਵਾਪਸ Curl_build_unencoding_ਢੇਰ(ਅੰਕੜੇ, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  ਜੇ(v) {
    ਅੱਖਰ *contenttype = Curl_copy_ਸਿਰਲੇਖ_ਮੁੱਲ(hd);
    ਜੇ(!contenttype)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜੇ(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    ਨਹੀਂਤਾਂ {
      curlx_free(ਅੰਕੜੇ->info.contenttype);
      ਅੰਕੜੇ->info.contenttype = contenttype;
    }
    ਵਾਪਸ CURLE_OK;
  }
  ਜੇ((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    ਵਾਪਸ CURLE_OK;
  }
  ਜੇ((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(ਅੰਕੜੇ, "HTTP/1.0 connection set to keep alive");
    ਵਾਪਸ CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : ਨਿਰਾ;
  ਜੇ(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    ਅਚਲ ਅੱਖਰ *ptr = v;

    /* Move forward until first digit or asterisk */
    ਜਦੋਂ(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    ਜੇ(ISDIGIT(*ptr)) {
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&ptr, &k->ਵਿੱਥ, CURL_OFF_T_MAX) &&
         (ਅੰਕੜੇ->state.resume_from == k->ਵਿੱਥ))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ(k->httpcode < 300)
      ਅੰਕੜੇ->state.resume_from = 0; /* get everything */
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
ਸਥਿਰ CURLcode http_ਸਿਰਲੇਖ_l(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *hd, size_t hdlen)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  ਅਚਲ ਅੱਖਰ *v = (!k->http_bodyless &&
                   (ਅੰਕੜੇ->ਸੈੱਟ.timecondition || ਅੰਕੜੇ->ਸੈੱਟ.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : ਨਿਰਾ;
  ਜੇ(v) {
    ਜੇ(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.get_filetime)
      ਅੰਕੜੇ->info.filetime = k->timeofdoc;
    ਵਾਪਸ CURLE_OK;
  }
  ਜੇ(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    ਅੱਖਰ *location = Curl_copy_ਸਿਰਲੇਖ_ਮੁੱਲ(hd);
    ਜੇ(!location)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜੇ(!*location ||
       (ਅੰਕੜੇ->req.location && !strcmp(ਅੰਕੜੇ->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      ਵਾਪਸ CURLE_OK;
    }
    ਨਹੀਂਤਾਂ {
      /* has value and is not an exact repeat */
      ਜੇ(ਅੰਕੜੇ->req.location) {
        failf(ਅੰਕੜੇ, "Multiple Location headers");
        curlx_free(location);
        ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
      }
      ਅੰਕੜੇ->req.location = location;

      ਜੇ((k->httpcode >= 300 && k->httpcode < 400) &&
         ਅੰਕੜੇ->ਸੈੱਟ.http_follow_ਬਹੁਲਕ) {
        CURLcode ਨਤੀਜਾ;
        DEBUGASSERT(!ਅੰਕੜੇ->req.newurl);
        ਅੰਕੜੇ->req.newurl = curlx_strdup(ਅੰਕੜੇ->req.location); /* clone */
        ਜੇ(!ਅੰਕੜੇ->req.newurl)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        ਨਤੀਜਾ = http_perhapsrewind(ਅੰਕੜੇ, conn);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;

        /* mark the next request as a followed location: */
        ਅੰਕੜੇ->state.this_is_a_follow = TRUE;
      }
    }
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
ਸਥਿਰ CURLcode http_ਸਿਰਲੇਖ_p(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *hd, size_t hdlen)
{
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਅਚਲ ਅੱਖਰ *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  ਜੇ(v) {
    ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
    ਜੇ((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(ਅੰਕੜੇ, "HTTP/1.0 proxy connection set to keep alive");
    }
    ਨਹੀਂਤਾਂ ਜੇ((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(ਅੰਕੜੇ, "HTTP/1.1 proxy connection set close");
    }
    ਵਾਪਸ CURLE_OK;
  }
#ਜੇ_ਅੰਤ
  ਜੇ((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    ਅੱਖਰ *auth;
    CURLcode ਨਤੀਜਾ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    ਜੇ(!ਅੰਕੜੇ->conn->ਬਿੱਟ.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      ਵਾਪਸ CURLE_OK;
#ਜੇ_ਅੰਤ
    auth = Curl_copy_ਸਿਰਲੇਖ_ਮੁੱਲ(hd);
    ਨਤੀਜਾ = auth ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜੇ(!ਨਤੀਜਾ) {
      ਨਤੀਜਾ = Curl_http_ਨਿਵੇਸ਼_auth(ਅੰਕੜੇ, TRUE, auth);
      curlx_free(auth);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
  ਜੇ(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
    ਬਣਤਰ negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    ਬਣਤਰ auth *authp = &ਅੰਕੜੇ->state.authhost;
    ਜੇ(!negdata)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜੇ(authp->picked == CURLAUTH_NEGOTIATE) {
      ਅੱਖਰ *persistentauth = Curl_copy_ਸਿਰਲੇਖ_ਮੁੱਲ(hd);
      ਜੇ(!persistentauth)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(ਅੰਕੜੇ, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
ਸਥਿਰ CURLcode http_ਸਿਰਲੇਖ_r(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *hd, size_t hdlen)
{
  ਅਚਲ ਅੱਖਰ *v = HD_VAL(hd, hdlen, "Retry-After:");
  ਜੇ(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    ਜੇ(!Curl_getdate_capped(v, &date)) {
      time_t current = ਸਮਾਂ(ਨਿਰਾ);
      ਜੇ(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    ਨਹੀਂਤਾਂ
      /* Try it as a decimal number, ignore errors */
      (ਖਾਲੀ)curlx_str_ਸੰਖਿਆ(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    ਜੇ(retry_after > 21600)
      retry_after = 21600;
    ਅੰਕੜੇ->info.retry_after = retry_after;
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
ਸਥਿਰ CURLcode http_ਸਿਰਲੇਖ_s(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *hd, size_t hdlen)
{
#ਜੇ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ਅਚਲ ਅੱਖਰ *v;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)hd;
  (ਖਾਲੀ)hdlen;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  v = (ਅੰਕੜੇ->cookies && ਅੰਕੜੇ->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : ਨਿਰਾ;
  ਜੇ(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ਅਚਲ ਅੱਖਰ *host = ਅੰਕੜੇ->req.cookiehost ?
      ਅੰਕੜੇ->req.cookiehost : ਅੰਕੜੇ->state.origin->hostname;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ secure_context = Curl_secure_context(ਅੰਕੜੇ, host) ?
      COOKIE_SECURE : 0;
    CURLcode ਨਤੀਜਾ;
    Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE, CURL_ਤਾਲਾ_ACCESS_SINGLE);
    ਨਤੀਜਾ = Curl_cookie_ਜੋੜੋ(ਅੰਕੜੇ, ਅੰਕੜੇ->cookies, v, host,
                             ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (ਅੰਕੜੇ->hsts &&
       (Curl_xfer_is_secure(ਅੰਕੜੇ) ||
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#ਨਹੀਂਤਾਂ
        0
#ਜੇ_ਅੰਤ
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : ਨਿਰਾ;
  ਜੇ(v) {
    CURLcode ਨਤੀਜਾ = Curl_hsts_parse(
      ਅੰਕੜੇ->hsts, ਅੰਕੜੇ->state.origin->hostname, v);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
        ਵਾਪਸ ਨਤੀਜਾ;
      infof(ਅੰਕੜੇ, "Illegal STS header skipped");
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "Parsed STS header fine (%zu entries)",
            Curl_llist_ਗਿਣਤੀ(&ਅੰਕੜੇ->hsts->list));
#ਜੇ_ਅੰਤ
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
ਸਥਿਰ CURLcode http_ਸਿਰਲੇਖ_t(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *hd, size_t hdlen)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;

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
  ਅਚਲ ਅੱਖਰ *v = (!k->http_bodyless &&
                   (ਅੰਕੜੇ->state.httpreq != HTTPREQ_ਸਿਰ) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : ਨਿਰਾ;
  ਜੇ(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode ਨਤੀਜਾ = Curl_build_unencoding_ਢੇਰ(ਅੰਕੜੇ, v, TRUE);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਜੇ(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(ਅੰਕੜੇ, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    ਵਾਪਸ CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  ਜੇ(v) {
    ਅੰਕੜੇ->req.resp_trailer = TRUE;
    ਵਾਪਸ CURLE_OK;
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
ਸਥਿਰ CURLcode http_ਸਿਰਲੇਖ_w(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *hd, size_t hdlen)
{
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    ਅੱਖਰ *auth = Curl_copy_ਸਿਰਲੇਖ_ਮੁੱਲ(hd);
    ਜੇ(!auth)
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = Curl_http_ਨਿਵੇਸ਼_auth(ਅੰਕੜੇ, FALSE, auth);
      curlx_free(auth);
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * http_header() parses a single response header.
 */
ਸਥਿਰ CURLcode http_ਸਿਰਲੇਖ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ ਅੱਖਰ *hd, size_t hdlen)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਚੋਣ(hd[0]) {
  ਹਾਲਤ 'a':
  ਹਾਲਤ 'A':
    ਨਤੀਜਾ = http_ਸਿਰਲੇਖ_a(ਅੰਕੜੇ, hd, hdlen);
    ਰੋਕੋ;
  ਹਾਲਤ 'c':
  ਹਾਲਤ 'C':
    ਨਤੀਜਾ = http_ਸਿਰਲੇਖ_c(ਅੰਕੜੇ, hd, hdlen);
    ਰੋਕੋ;
  ਹਾਲਤ 'l':
  ਹਾਲਤ 'L':
    ਨਤੀਜਾ = http_ਸਿਰਲੇਖ_l(ਅੰਕੜੇ, hd, hdlen);
    ਰੋਕੋ;
  ਹਾਲਤ 'p':
  ਹਾਲਤ 'P':
    ਨਤੀਜਾ = http_ਸਿਰਲੇਖ_p(ਅੰਕੜੇ, hd, hdlen);
    ਰੋਕੋ;
  ਹਾਲਤ 'r':
  ਹਾਲਤ 'R':
    ਨਤੀਜਾ = http_ਸਿਰਲੇਖ_r(ਅੰਕੜੇ, hd, hdlen);
    ਰੋਕੋ;
  ਹਾਲਤ 's':
  ਹਾਲਤ 'S':
    ਨਤੀਜਾ = http_ਸਿਰਲੇਖ_s(ਅੰਕੜੇ, hd, hdlen);
    ਰੋਕੋ;
  ਹਾਲਤ 't':
  ਹਾਲਤ 'T':
    ਨਤੀਜਾ = http_ਸਿਰਲੇਖ_t(ਅੰਕੜੇ, hd, hdlen);
    ਰੋਕੋ;
  ਹਾਲਤ 'w':
  ਹਾਲਤ 'W':
    ਨਤੀਜਾ = http_ਸਿਰਲੇਖ_w(ਅੰਕੜੇ, hd, hdlen);
    ਰੋਕੋ;
  }

  ਜੇ(!ਨਤੀਜਾ) {
    ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
    ਜੇ(conn->scheme->protocol & CURLPROTO_RTSP)
      ਨਤੀਜਾ = Curl_rtsp_parseheader(ਅੰਕੜੇ, hd);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
ਸਥਿਰ CURLcode http_statusline(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;

  ਚੋਣ(k->httpversion) {
  ਹਾਲਤ 10:
  ਹਾਲਤ 11:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP2
  ਹਾਲਤ 20:
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP3
  ਹਾਲਤ 30:
#ਜੇ_ਅੰਤ
    /* no major version switch mid-connection */
    ਜੇ(k->httpversion_ਭੇਜਿਆ &&
       (k->httpversion / 10 != k->httpversion_ਭੇਜਿਆ / 10)) {
      failf(ਅੰਕੜੇ, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_ਭੇਜਿਆ / 10, k->httpversion / 10);
      ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
    }
    ਰੋਕੋ;
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
  }

  ਅੰਕੜੇ->info.httpcode = k->httpcode;
  ਅੰਕੜੇ->info.httpversion = k->httpversion;
  conn->httpversion_ਵੇਖਿਆ = k->httpversion;

  ਜੇ(!ਅੰਕੜੇ->state.http_neg.rcvd_min ||
     ਅੰਕੜੇ->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    ਅੰਕੜੇ->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  ਜੇ(ਅੰਕੜੇ->state.resume_from && ਅੰਕੜੇ->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  ਜੇ(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(ਅੰਕੜੇ, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  ਚੋਣ(k->httpcode) {
  ਹਾਲਤ 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.timecondition)
      ਅੰਕੜੇ->info.timecond = TRUE;
    FALLTHROUGH();
  ਹਾਲਤ 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->ਮਾਪ = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
ਸਥਿਰ CURLcode http_ਮਾਪ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  ਜੇ(ਅੰਕੜੇ->req.ignore_cl || k->chunk) {
    k->ਮਾਪ = k->maxdownload = -1;
  }
  ਨਹੀਂਤਾਂ ਜੇ(k->ਮਾਪ != -1) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.max_filesize &&
       !k->ignorebody &&
       (k->ਮਾਪ > ਅੰਕੜੇ->ਸੈੱਟ.max_filesize)) {
      failf(ਅੰਕੜੇ, "Maximum file size exceeded");
      ਵਾਪਸ CURLE_FILESIZE_EXCEEDED;
    }
    ਜੇ(k->ignorebody)
      infof(ਅੰਕੜੇ, "setting size while ignoring");
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, k->ਮਾਪ);
    k->maxdownload = k->ਮਾਪ;
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_verify_ਸਿਰਲੇਖ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ ਅੱਖਰ *hd, size_t hdlen)
{
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  size_t i;
  bool has_colon = FALSE;
  ਲਈ(i = 0; i < hdlen; i++) {
    ਚੋਣ(hd[i]) {
    ਹਾਲਤ '\0':
      /* this is bad, bail out */
      failf(ਅੰਕੜੇ, "NUL byte in header");
      ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
    ਹਾਲਤ '\r':
      ਜੇ(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(ਅੰਕੜੇ, "Carriage return found in header");
        ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
      }
      ਰੋਕੋ;
    ਹਾਲਤ ':':
      has_colon = TRUE;
      ਰੋਕੋ;
    }
  }
  ਜੇ(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    ਵਾਪਸ CURLE_OK;
  ਜੇ(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  ਨਹੀਂਤਾਂ ਜੇ(!has_colon) {
    /* this is bad, bail out */
    failf(ਅੰਕੜੇ, "Header without colon");
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_bump_headersize(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ max = MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਮਾਪ;
  ਜੇ(delta < MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਮਾਪ) {
    ਅੰਕੜੇ->info.ਸਿਰਲੇਖ_ਮਾਪ += (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)delta;
    ਅੰਕੜੇ->req.allheadercount += (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)delta;
    ਜੇ(!connect_only)
      ਅੰਕੜੇ->req.headerbytecount += (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)delta;
    ਜੇ(ਅੰਕੜੇ->req.allheadercount > max)
      bad = ਅੰਕੜੇ->req.allheadercount;
    ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->info.ਸਿਰਲੇਖ_ਮਾਪ > (max * 20)) {
      bad = ਅੰਕੜੇ->info.ਸਿਰਲੇਖ_ਮਾਪ;
      max *= 20;
    }
  }
  ਨਹੀਂਤਾਂ
    bad = ਅੰਕੜੇ->req.allheadercount + delta;
  ਜੇ(bad) {
    failf(ਅੰਕੜੇ, "Too large response headers: %zu > %u", bad, max);
    ਵਾਪਸ CURLE_RECV_ERROR;
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
ਸਥਿਰ CURLcode http_on_101_upgrade(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_ਬਦਲਿਆ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;

#ਜੇ !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (ਖਾਲੀ)buf;
  (ਖਾਲੀ)blen;
  (ਖਾਲੀ)pconsumed;
#ਨਹੀਂਤਾਂ
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ upgr101_requested = k->upgr101;
#ਜੇ_ਅੰਤ

  ਜੇ(k->httpversion_ਭੇਜਿਆ != 11) {
    /* invalid for other HTTP versions */
    failf(ਅੰਕੜੇ, "server sent 101 response while not talking HTTP/1.1");
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->ਬਿੱਟ.upgrade_ਵਿੱਚ_ਤਰੱਕੀ = FALSE;
  *conn_ਬਦਲਿਆ = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2
  ਜੇ(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(ਅੰਕੜੇ, "Received 101, Switching to HTTP/2");
    ਨਤੀਜਾ = Curl_http2_upgrade(ਅੰਕੜੇ, conn, FIRSTSOCKET, buf, blen);
    ਜੇ(!ਨਤੀਜਾ)
      *pconsumed += blen;
    ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS
  ਜੇ(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(ਅੰਕੜੇ, "Received 101, Switching to WebSocket");
    ਨਤੀਜਾ = Curl_ws_accept(ਅੰਕੜੇ, buf, blen);
    ਜੇ(!ਨਤੀਜਾ)
      *pconsumed += blen; /* ws accept handled the data */
    ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਜੇ_ਅੰਤ
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->ਸਿਰਲੇਖ = FALSE;
  ਵਾਪਸ CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
ਸਥਿਰ CURLcode http_on_1xx_response(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_ਬਦਲਿਆ)
{
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->ਸਿਰਲੇਖ = TRUE;
  k->headerline = 0; /* restart the header line counter */

  ਚੋਣ(k->httpcode) {
  ਹਾਲਤ 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ 101:
    ਵਾਪਸ http_on_101_upgrade(ਅੰਕੜੇ, buf, blen, pconsumed, conn_ਬਦਲਿਆ);
  ਮੂਲ:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
ਸਥਿਰ ਖਾਲੀ http_check_auth_ਘੇਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
  ਜੇ(conn->ਬਿੱਟ.close &&
     (((ਅੰਕੜੇ->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((ਅੰਕੜੇ->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(ਅੰਕੜੇ, "Connection closure while negotiating auth (HTTP 1.0?)");
    ਅੰਕੜੇ->state.authproblem = TRUE;
  }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
  ਜੇ(conn->ਬਿੱਟ.close &&
    (((ਅੰਕੜੇ->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((ਅੰਕੜੇ->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(ਅੰਕੜੇ, "Connection closure while negotiating auth (HTTP 1.0?)");
    ਅੰਕੜੇ->state.authproblem = TRUE;
  }
  ਜੇ((conn->http_negotiate_state == CURL_NEGO_AUTH_ਪੂਰਾ) &&
     (ਅੰਕੜੇ->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  ਜੇ((conn->proxy_negotiate_state == CURL_NEGO_AUTH_ਪੂਰਾ) &&
     (ਅੰਕੜੇ->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#ਜੇ_ਅੰਤ
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ http_check_auth_ਘੇਰਾ(x, y) /*ਖਾਲੀ_ਥਾਵਾਂ*/
#ਜੇ_ਅੰਤ

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
ਸਥਿਰ CURLcode http_handle_send_error(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!ਅੰਕੜੇ->req.authneg && !conn->ਬਿੱਟ.close &&
     !Curl_creader_will_rewind(ਅੰਕੜੇ)) {
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

    ਚੋਣ(ਅੰਕੜੇ->state.httpreq) {
    ਹਾਲਤ HTTPREQ_PUT:
    ਹਾਲਤ HTTPREQ_POST:
    ਹਾਲਤ HTTPREQ_POST_FORM:
    ਹਾਲਤ HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      ਜੇ(!Curl_req_ਪੂਰਾ_sending(ਅੰਕੜੇ)) {
        ਜੇ((k->httpcode == 417) && http_exp100_is_selected(ਅੰਕੜੇ)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          ਜੇ(!k->writebytecount && http_exp100_is_ਉਡੀਕ(ਅੰਕੜੇ)) {
            infof(ਅੰਕੜੇ, "Got HTTP failure 417 while waiting for a 100");
          }
          ਨਹੀਂਤਾਂ {
            infof(ਅੰਕੜੇ, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            ਨਤੀਜਾ = http_perhapsrewind(ਅੰਕੜੇ, conn);
            ਜੇ(ਨਤੀਜਾ)
              ਵਾਪਸ ਨਤੀਜਾ;
          }
          ਅੰਕੜੇ->state.disableexpect = TRUE;
          Curl_req_abort_sending(ਅੰਕੜੇ);
          DEBUGASSERT(!ਅੰਕੜੇ->req.newurl);
          ਅੰਕੜੇ->req.newurl = Curl_bufref_dup(&ਅੰਕੜੇ->state.url);
          ਜੇ(!ਅੰਕੜੇ->req.newurl)
            ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        }
        ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.http_keep_sending_on_error) {
          infof(ਅੰਕੜੇ, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(ਅੰਕੜੇ);
        }
        ਨਹੀਂਤਾਂ {
          infof(ਅੰਕੜੇ, "HTTP error before end of send, stop sending");
          streamclose(conn);
          ਨਤੀਜਾ = Curl_req_abort_sending(ਅੰਕੜੇ);
          ਜੇ(ਨਤੀਜਾ)
            ਵਾਪਸ ਨਤੀਜਾ;
        }
      }
      ਰੋਕੋ;

    ਮੂਲ: /* default label present to avoid compiler warnings */
      ਰੋਕੋ;
    }
  }

  ਜੇ(Curl_creader_will_rewind(ਅੰਕੜੇ) && !Curl_req_ਪੂਰਾ_sending(ਅੰਕੜੇ)) {
    /* We rewind before next send, continue sending now */
    infof(ਅੰਕੜੇ, "Keep sending data to get tossed away");
    CURL_REQ_ਸੈੱਟ_SEND(ਅੰਕੜੇ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode http_on_response(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਅੱਖਰ *ਆਖਰੀ_hd, size_t ਆਖਰੀ_hd_len,
                                 ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                                 size_t *pconsumed)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  bool conn_ਬਦਲਿਆ = FALSE;

  (ਖਾਲੀ)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  ਜੇ(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    ਜੇ(ਅੰਕੜੇ->req.httpversion != 20)
      infof(ਅੰਕੜੇ, "Lying server, not serving HTTP/2");
  }

  ਜੇ(k->httpcode < 200 && ਆਖਰੀ_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    ਨਤੀਜਾ = http_write_ਸਿਰਲੇਖ(ਅੰਕੜੇ, ਆਖਰੀ_hd, ਆਖਰੀ_hd_len);
    ਆਖਰੀ_hd = ਨਿਰਾ; /* handled it */
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(k->httpcode < 100) {
    failf(ਅੰਕੜੇ, "Unsupported response code in HTTP response");
    ਨਤੀਜਾ = CURLE_UNSUPPORTED_PROTOCOL;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(k->httpcode < 200) {
    ਨਤੀਜਾ = http_on_1xx_response(ਅੰਕੜੇ, buf, blen, pconsumed, &conn_ਬਦਲਿਆ);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* k->httpcode >= 200, final response */
  k->ਸਿਰਲੇਖ = FALSE;
  ਜੇ(conn->ਬਿੱਟ.upgrade_ਵਿੱਚ_ਤਰੱਕੀ) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->ਬਿੱਟ.upgrade_ਵਿੱਚ_ਤਰੱਕੀ = FALSE;
    conn_ਬਦਲਿਆ = TRUE;
  }

  ਜੇ((k->ਮਾਪ == -1) && !k->chunk && !conn->ਬਿੱਟ.close &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     ਅੰਕੜੇ->state.httpreq != HTTPREQ_ਸਿਰ) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(ਅੰਕੜੇ, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_ਘੇਰਾ(ਅੰਕੜੇ, conn);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  ਜੇ(ਅੰਕੜੇ->req.upgr101 == UPGR101_WS) {
    failf(ਅੰਕੜੇ, "Refused WebSocket upgrade: %d", k->httpcode);
    ਨਤੀਜਾ = CURLE_HTTP_RETURNED_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ

  /* Check if this response means the transfer errored. */
  ਜੇ(http_should_fail(ਅੰਕੜੇ, ਅੰਕੜੇ->req.httpcode)) {
    failf(ਅੰਕੜੇ, "The requested URL returned error: %d",
          k->httpcode);
    ਨਤੀਜਾ = CURLE_HTTP_RETURNED_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  ਨਤੀਜਾ = Curl_http_auth_act(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(k->httpcode >= 300) {
    ਨਤੀਜਾ = http_handle_send_error(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* final response without error, prepare to receive the body */
  ਨਤੀਜਾ = http_firstwrite(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  ਨਤੀਜਾ = http_ਮਾਪ(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  ਜੇ((conn->scheme->protocol & CURLPROTO_RTSP) && ਅੰਕੜੇ->req.no_body &&
     !ਅੰਕੜੇ->ਸੈੱਟ.opt_no_body && (k->ਮਾਪ > 0))
    ਅੰਕੜੇ->req.no_body = FALSE;
#ਜੇ_ਅੰਤ

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  ਜੇ(ਅੰਕੜੇ->req.no_body)
    k->download_ਪੂਰਾ = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  ਜੇ((k->maxdownload == 0) && (k->httpversion_ਭੇਜਿਆ < 20))
    k->download_ਪੂਰਾ = TRUE;

ਬਾਹਰਲਾ:
  ਜੇ(ਆਖਰੀ_hd)
    /* if not written yet, write it now */
    ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ,
                            http_write_ਸਿਰਲੇਖ(ਅੰਕੜੇ, ਆਖਰੀ_hd, ਆਖਰੀ_hd_len));
  ਜੇ(conn_ਬਦਲਿਆ)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(ਅੰਕੜੇ->multi);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode http_rw_hd(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਅਚਲ ਅੱਖਰ *hd, size_t hdlen,
                           ਅਚਲ ਅੱਖਰ *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  ਪੂਰਨ writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  ਜੇ((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    ਬਣਤਰ dynbuf ਆਖਰੀ_ਸਿਰਲੇਖ;
    size_t consumed;

    curlx_dyn_init(&ਆਖਰੀ_ਸਿਰਲੇਖ, hdlen + 1);
    ਨਤੀਜਾ = curlx_dyn_addn(&ਆਖਰੀ_ਸਿਰਲੇਖ, hd, hdlen);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&ਅੰਕੜੇ->state.headerb);
    ਨਤੀਜਾ = http_on_response(ਅੰਕੜੇ, curlx_dyn_ptr(&ਆਖਰੀ_ਸਿਰਲੇਖ),
                              curlx_dyn_len(&ਆਖਰੀ_ਸਿਰਲੇਖ),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&ਆਖਰੀ_ਸਿਰਲੇਖ);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_ਸਿਰਲੇਖ;
  ਜੇ(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    ਜੇ(ਅੰਕੜੇ->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ਅਚਲ ਅੱਖਰ *p = hd;

      curlx_str_passblanks(&p);
      ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(p, "HTTP/", 5)) {
        p += 5;
        ਚੋਣ(*p) {
        ਹਾਲਤ '1':
          p++;
          ਜੇ((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            ਜੇ(ISBLANK(p[2])) {
              k->httpversion = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(10 + (p[1] - '0'));
              p += 3;
              ਜੇ(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          ਜੇ(!fine_statusline) {
            failf(ਅੰਕੜੇ, "Unsupported HTTP/1 subversion in response");
            ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
          }
          ਰੋਕੋ;
        ਹਾਲਤ '2':
        ਹਾਲਤ '3':
          ਜੇ(!ISBLANK(p[1]))
            ਰੋਕੋ;
          k->httpversion = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((*p - '0') * 10);
          p += 2;
          ਜੇ(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            ਜੇ(!ISBLANK(*p))
              ਰੋਕੋ;
            fine_statusline = TRUE;
          }
          ਰੋਕੋ;
        ਮੂਲ: /* unsupported */
          failf(ਅੰਕੜੇ, "Unsupported HTTP version in response");
          ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      ਜੇ(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(ਅੰਕੜੇ, hd, hdlen);
        ਜੇ(check == ਸਥਿਤੀ_ਪੂਰਾ) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->conn->scheme->protocol & CURLPROTO_RTSP) {
      ਅਚਲ ਅੱਖਰ *p = hd;
      ਬਣਤਰ Curl_str ver;
      curl_off_t ਸਥਿਤੀ;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      ਜੇ(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_ਸੰਖਿਆ(&p, &ਸਥਿਤੀ, 999)) {
        curlx_str_trimblanks(&ver);
        ਜੇ(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (ਪੂਰਨ)ਸਥਿਤੀ;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      ਜੇ(!fine_statusline)
        ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
    }

    ਜੇ(fine_statusline) {
      ਨਤੀਜਾ = http_statusline(ਅੰਕੜੇ, ਅੰਕੜੇ->conn);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      writetype |= CLIENTWRITE_ਸਥਿਤੀ;
    }
    ਨਹੀਂਤਾਂ {
      k->ਸਿਰਲੇਖ = FALSE;   /* this is not a header line */
      ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
    }
  }

  ਨਤੀਜਾ = Curl_verify_ਸਿਰਲੇਖ(ਅੰਕੜੇ, hd, hdlen);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = http_ਸਿਰਲੇਖ(ਅੰਕੜੇ, hd, hdlen);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ, hd, hdlen);

  ਜੇ(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, writetype, hd, hdlen);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_bump_headersize(ਅੰਕੜੇ, hdlen, FALSE);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਵਾਪਸ CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
ਖਾਲੀ Curl_http_ਮੰਜ਼ਿਲ_fold(ਬਣਤਰ dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  ਅਚਲ ਅੱਖਰ *hd = curlx_dyn_ptr(bf);
  ਜੇ(len && (hd[len - 1] == '\n'))
    len--;
  ਜੇ(len && (hd[len - 1] == '\r'))
    len--;
  ਜਦੋਂ(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

ਸਥਿਰ ਖਾਲੀ unfold_ਸਿਰਲੇਖ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  Curl_http_ਮੰਜ਼ਿਲ_fold(&ਅੰਕੜੇ->state.headerb);
  ਅੰਕੜੇ->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
ਸਥਿਰ CURLcode http_parse_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                                   size_t *pconsumed)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  ਅਚਲ ਅੱਖਰ *ਸਿਰਾ_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  ਜੇ(ਅੰਕੜੇ->state.maybe_folded && blen) {
    ਜੇ(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_ਸਿਰਲੇਖ(ਅੰਕੜੇ);
    }
    ਨਹੀਂਤਾਂ {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      ਨਤੀਜਾ = http_rw_hd(ਅੰਕੜੇ, curlx_dyn_ptr(&ਅੰਕੜੇ->state.headerb),
                          curlx_dyn_len(&ਅੰਕੜੇ->state.headerb),
                          ਨਿਰਾ, 0, &ignore_this);
      curlx_dyn_reset(&ਅੰਕੜੇ->state.headerb);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਅੰਕੜੇ->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  ਜਦੋਂ(blen && k->ਸਿਰਲੇਖ) {
    size_t consumed;
    size_t hlen;
    ਅਚਲ ਅੱਖਰ *hd;
    size_t unfold_len = 0;

    ਜੇ(ਅੰਕੜੇ->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      ਜਦੋਂ(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      ਜੇ(blen) {
        /* insert a single space */
        ਨਤੀਜਾ = curlx_dyn_addn(&ਅੰਕੜੇ->state.headerb, " ", 1);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
        ਅੰਕੜੇ->state.leading_unfold = FALSE; /* done now */
      }
    }

    ਸਿਰਾ_ptr = memchr(buf, '\n', blen);
    ਜੇ(!ਸਿਰਾ_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      ਨਤੀਜਾ = curlx_dyn_addn(&ਅੰਕੜੇ->state.headerb, buf, blen);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      *pconsumed += blen + unfold_len;

      ਜੇ(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(ਅੰਕੜੇ, conn,
                           curlx_dyn_ptr(&ਅੰਕੜੇ->state.headerb),
                           curlx_dyn_len(&ਅੰਕੜੇ->state.headerb));

        ਜੇ(st == ਸਥਿਤੀ_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->ਸਿਰਲੇਖ = FALSE;
          streamclose(conn);
          ਜੇ((k->httpversion >= 10) || conn->ਬਿੱਟ.reuse) {
            failf(ਅੰਕੜੇ, "Invalid status line");
            ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
          }
          ਜੇ(!ਅੰਕੜੇ->state.http_neg.accept_09) {
            failf(ਅੰਕੜੇ, "Received HTTP/0.9 when not allowed");
            ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          ਜਾਓ ਬਾਹਰਲਾ;
        }
      }
      ਜਾਓ ਬਾਹਰਲਾ; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (ਸਿਰਾ_ptr - buf) + 1;

    ਨਤੀਜਾ = curlx_dyn_addn(&ਅੰਕੜੇ->state.headerb, buf, consumed);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&ਅੰਕੜੇ->state.headerb);
    hd = curlx_dyn_ptr(&ਅੰਕੜੇ->state.headerb);

    ਜੇ(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(ਅੰਕੜੇ, conn, hd, hlen);
      ਜੇ(st == ਸਥਿਤੀ_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        ਜੇ((k->httpversion >= 10) || conn->ਬਿੱਟ.reuse) {
          failf(ਅੰਕੜੇ, "Invalid status line");
          ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
        }
        ਜੇ(!ਅੰਕੜੇ->state.http_neg.accept_09) {
          failf(ਅੰਕੜੇ, "Received HTTP/0.9 when not allowed");
          ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->ਸਿਰਲੇਖ = FALSE;
        leftover_body = TRUE;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        ਜੇ(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_ਸਿਰਲੇਖ(ਅੰਕੜੇ);
          ਜਾਰੀ;
        }
        ਨਹੀਂਤਾਂ ਜੇ(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          ਅੰਕੜੇ->state.maybe_folded = TRUE;
          ਰੋਕੋ;
        }
      }
    }

    ਨਤੀਜਾ = http_rw_hd(ਅੰਕੜੇ, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&ਅੰਕੜੇ->state.headerb);
    ਜੇ(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
ਬਾਹਰਲਾ:
  ਜੇ(!k->ਸਿਰਲੇਖ && !leftover_body) {
    curlx_dyn_free(&ਅੰਕੜੇ->state.headerb);
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਅੱਖਰ *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode ਨਤੀਜਾ;
  size_t consumed;
  ਅੱਖਰ tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  ਨਤੀਜਾ = http_rw_hd(ਅੰਕੜੇ, hd, hdlen, &tmp, 0, &consumed);
  ਜੇ(!ਨਤੀਜਾ && is_eos) {
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                                  size_t *pconsumed)
{
  ਜੇ(!ਅੰਕੜੇ->req.ਸਿਰਲੇਖ) {
    *pconsumed = 0;
    ਵਾਪਸ CURLE_OK;
  }
  ਨਹੀਂਤਾਂ {
    CURLcode ਨਤੀਜਾ;

    ਨਤੀਜਾ = http_parse_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, buf, blen, pconsumed);
    ਜੇ(!ਨਤੀਜਾ && !ਅੰਕੜੇ->req.ਸਿਰਲੇਖ) {
      ਜੇ(!ਅੰਕੜੇ->req.no_body && curlx_dyn_len(&ਅੰਕੜੇ->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&ਅੰਕੜੇ->state.headerb),
                                   curlx_dyn_len(&ਅੰਕੜੇ->state.headerb));
      }
      curlx_dyn_free(&ਅੰਕੜੇ->state.headerb);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
  }
}

CURLcode Curl_http_write_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                              bool is_eos)
{
  CURLcode ਨਤੀਜਾ;
  size_t consumed;
  ਪੂਰਨ flags;

  ਨਤੀਜਾ = Curl_http_write_resp_hds(ਅੰਕੜੇ, buf, blen, &consumed);
  ਜੇ(ਨਤੀਜਾ || ਅੰਕੜੇ->req.ਪੂਰਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !ਅੰਕੜੇ->req.ਸਿਰਲੇਖ);
  ਜੇ(!ਅੰਕੜੇ->req.ਸਿਰਲੇਖ && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    ਜੇ(is_eos)
      flags |= CLIENTWRITE_EOS;
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, flags, buf, blen);
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_ਸਥਿਤੀ(int32_t *pstatus, ਅਚਲ ਅੱਖਰ *s, size_t len)
{
  CURLcode ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
  int32_t ਸਥਿਤੀ = 0;
  ਪੂਰਨ i;

  ਜੇ(len != 3)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਲਈ(i = 0; i < 3; ++i) {
    ਅੱਖਰ c = s[i];

    ਜੇ(c < '0' || c > '9')
      ਜਾਓ ਬਾਹਰਲਾ;

    ਸਥਿਤੀ *= 10;
    ਸਥਿਤੀ += c - '0';
  }
  ਨਤੀਜਾ = CURLE_OK;
ਬਾਹਰਲਾ:
  *pstatus = ਨਤੀਜਾ ? -1 : ਸਥਿਤੀ;
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_http_req_make(ਬਣਤਰ httpreq **preq,
                            ਅਚਲ ਅੱਖਰ *ਵਿਧੀ, size_t m_len,
                            ਅਚਲ ਅੱਖਰ *scheme, size_t s_len,
                            ਅਚਲ ਅੱਖਰ *authority, size_t a_len,
                            ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, size_t p_len)
{
  ਬਣਤਰ httpreq *req;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  DEBUGASSERT(ਵਿਧੀ && m_len);

  req = curlx_calloc(1, ਆਕਾਰ(*req) + m_len);
  ਜੇ(!req)
    ਜਾਓ ਬਾਹਰਲਾ;
#ਜੇ defined(__GNUC__) && __GNUC__ >= 13
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#ਹਦਾਇਤ GCC diagnostic ignored "-Warray-bounds"
#ਜੇ_ਅੰਤ
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(req->ਵਿਧੀ, ਵਿਧੀ, m_len);
#ਜੇ defined(__GNUC__) && __GNUC__ >= 13
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
  ਜੇ(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    ਜੇ(!req->scheme)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    ਜੇ(!req->authority)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ਰਸਤਾ) {
    req->ਰਸਤਾ = curlx_memdup0(ਰਸਤਾ, p_len);
    ਜੇ(!req->ਰਸਤਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  Curl_dynhds_init(&req->ਸਿਰਲੇਖਾਂ, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ && req)
    Curl_http_req_free(req);
  *preq = ਨਤੀਜਾ ? ਨਿਰਾ : req;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode req_ਸੌਂਪੋ_url_authority(ਬਣਤਰ httpreq *req, CURLU *url)
{
  ਅੱਖਰ *host, *ਪੋਰਟ;
  ਬਣਤਰ dynbuf buf;
  CURLUcode uc;
  CURLcode ਨਤੀਜਾ = CURLE_URL_MALFORMAT;

  host = ਪੋਰਟ = ਨਿਰਾ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  ਜੇ(uc && uc != CURLUE_NO_HOST)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(!host) {
    req->authority = ਨਿਰਾ;
    ਨਤੀਜਾ = CURLE_OK;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  uc = curl_url_get(url, CURLUPART_ਪੋਰਟ, &ਪੋਰਟ, CURLU_NO_DEFAULT_ਪੋਰਟ);
  ਜੇ(uc && uc != CURLUE_NO_ਪੋਰਟ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&buf, host);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(ਪੋਰਟ) {
    ਨਤੀਜਾ = curlx_dyn_addf(&buf, ":%s", ਪੋਰਟ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  req->authority = curlx_dyn_ptr(&buf);
ਬਾਹਰਲਾ:
  curlx_free(host);
  curlx_free(ਪੋਰਟ);
  ਜੇ(ਨਤੀਜਾ)
    curlx_dyn_free(&buf);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode req_ਸੌਂਪੋ_url_ਰਸਤਾ(ਬਣਤਰ httpreq *req, CURLU *url)
{
  ਅੱਖਰ *ਰਸਤਾ, *query;
  ਬਣਤਰ dynbuf buf;
  CURLUcode uc;
  CURLcode ਨਤੀਜਾ = CURLE_URL_MALFORMAT;

  ਰਸਤਾ = query = ਨਿਰਾ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_ਰਸਤਾ, &ਰਸਤਾ, 0);
  ਜੇ(uc)
    ਜਾਓ ਬਾਹਰਲਾ;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  ਜੇ(uc && uc != CURLUE_NO_QUERY)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(!query) {
    req->ਰਸਤਾ = ਰਸਤਾ;
    ਰਸਤਾ = ਨਿਰਾ;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&buf, ਰਸਤਾ);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addf(&buf, "?%s", query);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    req->ਰਸਤਾ = curlx_dyn_ptr(&buf);
  }
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  curlx_free(ਰਸਤਾ);
  curlx_free(query);
  ਜੇ(ਨਤੀਜਾ)
    curlx_dyn_free(&buf);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_http_req_make2(ਬਣਤਰ httpreq **preq,
                             ਅਚਲ ਅੱਖਰ *ਵਿਧੀ, size_t m_len,
                             CURLU *url, ਅਚਲ ਅੱਖਰ *scheme_default)
{
  ਬਣਤਰ httpreq *req;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  CURLUcode uc;

  DEBUGASSERT(ਵਿਧੀ && m_len);

  req = curlx_calloc(1, ਆਕਾਰ(*req) + m_len);
  ਜੇ(!req)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(req->ਵਿਧੀ, ਵਿਧੀ, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  ਜੇ(uc && uc != CURLUE_NO_SCHEME)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    ਜੇ(!req->scheme)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = req_ਸੌਂਪੋ_url_authority(req, url);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਨਤੀਜਾ = req_ਸੌਂਪੋ_url_ਰਸਤਾ(req, url);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  Curl_dynhds_init(&req->ਸਿਰਲੇਖਾਂ, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ && req)
    Curl_http_req_free(req);
  *preq = ਨਤੀਜਾ ? ਨਿਰਾ : req;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_http_req_free(ਬਣਤਰ httpreq *req)
{
  ਜੇ(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->ਰਸਤਾ);
    Curl_dynhds_free(&req->ਸਿਰਲੇਖਾਂ);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

ਬਣਤਰ name_const {
  ਅਚਲ ਅੱਖਰ *name;
  size_t namelen;
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ name_const H2_NON_ਖੇਤਰ[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

ਸਥਿਰ bool h2_permissible_ਖੇਤਰ(ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *e)
{
  size_t i;
  ਲਈ(i = 0; i < CURL_ARRAYSIZE(H2_NON_ਖੇਤਰ); ++i) {
    ਜੇ(e->namelen == H2_NON_ਖੇਤਰ[i].namelen &&
       curl_strnequal(H2_NON_ਖੇਤਰ[i].name, e->name, e->namelen))
      ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool http_TE_has_token(ਅਚਲ ਅੱਖਰ *fvalue, ਅਚਲ ਅੱਖਰ *token)
{
  ਜਦੋਂ(*fvalue) {
    ਬਣਤਰ Curl_str name;

    /* skip to first token */
    ਜਦੋਂ(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    ਜੇ(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      ਵਾਪਸ FALSE;
    ਜੇ(curlx_str_casecompare(&name, token))
      ਵਾਪਸ TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    ਜਦੋਂ(*fvalue && *fvalue != ',') {
      ਜੇ(*fvalue == '"') {
        ਬਣਤਰ Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        ਜੇ(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_ਸਿਰਲੇਖ))
          ਵਾਪਸ FALSE;
      }
      ਨਹੀਂਤਾਂ
        fvalue++;
    }
  }
  ਵਾਪਸ FALSE;
}

CURLcode Curl_http_req_ਮੰਜ਼ਿਲ_h2(ਬਣਤਰ dynhds *h2_ਸਿਰਲੇਖਾਂ,
                             ਬਣਤਰ httpreq *req, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਅੱਖਰ *scheme = ਨਿਰਾ, *authority = ਨਿਰਾ;
  ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *e;
  size_t i;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_ਸਿਰਲੇਖਾਂ);

  ਜੇ(req->scheme) {
    scheme = req->scheme;
  }
  ਨਹੀਂਤਾਂ ਜੇ(strcmp("CONNECT", req->ਵਿਧੀ)) {
    scheme = Curl_checkheaders(ਅੰਕੜੇ, STRCONST(HTTP_PSEUDO_SCHEME));
    ਜੇ(scheme) {
      scheme += ਆਕਾਰ(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(ਅੰਕੜੇ, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    ਨਹੀਂਤਾਂ {
      scheme = ਅੰਕੜੇ->state.origin->scheme->name;
    }
  }

  ਜੇ(req->authority) {
    authority = req->authority;
  }
  ਨਹੀਂਤਾਂ {
    e = Curl_dynhds_get(&req->ਸਿਰਲੇਖਾਂ, STRCONST("Host"));
    ਜੇ(e)
      authority = e->ਮੁੱਲ;
  }

  Curl_dynhds_reset(h2_ਸਿਰਲੇਖਾਂ);
  Curl_dynhds_ਸੈੱਟ_opts(h2_ਸਿਰਲੇਖਾਂ, DYNHDS_OPT_LOWERCASE);
  ਨਤੀਜਾ = Curl_dynhds_ਜੋੜੋ(h2_ਸਿਰਲੇਖਾਂ, STRCONST(HTTP_PSEUDO_ਵਿਧੀ),
                           req->ਵਿਧੀ, ਲੜੀ_ਲੰਬਾਈ(req->ਵਿਧੀ));
  ਜੇ(!ਨਤੀਜਾ && scheme) {
    ਨਤੀਜਾ = Curl_dynhds_ਜੋੜੋ(h2_ਸਿਰਲੇਖਾਂ, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, ਲੜੀ_ਲੰਬਾਈ(scheme));
  }
  ਜੇ(!ਨਤੀਜਾ && authority) {
    ਨਤੀਜਾ = Curl_dynhds_ਜੋੜੋ(h2_ਸਿਰਲੇਖਾਂ, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, ਲੜੀ_ਲੰਬਾਈ(authority));
  }
  ਜੇ(!ਨਤੀਜਾ && req->ਰਸਤਾ) {
    ਨਤੀਜਾ = Curl_dynhds_ਜੋੜੋ(h2_ਸਿਰਲੇਖਾਂ, STRCONST(HTTP_PSEUDO_ਰਸਤਾ),
                             req->ਰਸਤਾ, ਲੜੀ_ਲੰਬਾਈ(req->ਰਸਤਾ));
  }
  ਲਈ(i = 0; !ਨਤੀਜਾ && i < Curl_dynhds_ਗਿਣਤੀ(&req->ਸਿਰਲੇਖਾਂ); ++i) {
    e = Curl_dynhds_getn(&req->ਸਿਰਲੇਖਾਂ, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    ਜੇ(e->namelen == 2 && curl_strequal("TE", e->name)) {
      ਜੇ(http_TE_has_token(e->ਮੁੱਲ, "trailers"))
        ਨਤੀਜਾ = Curl_dynhds_ਜੋੜੋ(h2_ਸਿਰਲੇਖਾਂ, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    ਨਹੀਂਤਾਂ ਜੇ(h2_permissible_ਖੇਤਰ(e)) {
      ਨਤੀਜਾ = Curl_dynhds_ਜੋੜੋ(h2_ਸਿਰਲੇਖਾਂ, e->name, e->namelen,
                               e->ਮੁੱਲ, e->valuelen);
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_http_resp_make(ਬਣਤਰ http_resp **presp,
                             ਪੂਰਨ ਸਥਿਤੀ,
                             ਅਚਲ ਅੱਖਰ *description)
{
  ਬਣਤਰ http_resp *resp;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  resp = curlx_calloc(1, ਆਕਾਰ(*resp));
  ਜੇ(!resp)
    ਜਾਓ ਬਾਹਰਲਾ;

  resp->ਸਥਿਤੀ = ਸਥਿਤੀ;
  ਜੇ(description) {
    resp->description = curlx_strdup(description);
    ਜੇ(!resp->description)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  Curl_dynhds_init(&resp->ਸਿਰਲੇਖਾਂ, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ && resp)
    Curl_http_resp_free(resp);
  *presp = ਨਤੀਜਾ ? ਨਿਰਾ : resp;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_http_resp_free(ਬਣਤਰ http_resp *resp)
{
  ਜੇ(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->ਸਿਰਲੇਖਾਂ);
    Curl_dynhds_free(&resp->trailers);
    ਜੇ(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_http = {
  Curl_http_ਤਿਆਰੀ_conn,                 /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_ਪੂਰਾ,                       /* done */
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

#ਜੇ_ਅੰਤ /* CURL_DISABLE_HTTP */
