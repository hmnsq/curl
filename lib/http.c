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
#అన్తర్భావ "urldata.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NET_IF_H
#అన్తర్భావ <net/if.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_PARAM_H
#అన్తర్భావ <sys/param.h>
#అయితే_ముగింపు

#అన్తర్భావ "transfer.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "formdata.h"
#అన్తర్భావ "mime.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "cookie.h"
#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "vquic/vquic.h"
#అన్తర్భావ "http_digest.h"
#అన్తర్భావ "http_ntlm.h"
#అన్తర్భావ "http_negotiate.h"
#అన్తర్భావ "http_aws_sigv4.h"
#అన్తర్భావ "http_httpsig.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "urlapi-int.h"
#అన్తర్భావ "curl_share.h"
#అన్తర్భావ "dynhds.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "headers.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "parsedate.h" /* for the week day and month names */
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "content_encoding.h"
#అన్తర్భావ "http_proxy.h"
#అన్తర్భావ "http2.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "altsvc.h"
#అన్తర్భావ "hsts.h"
#అన్తర్భావ "rtsp.h"
#అన్తర్భావ "ws.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "curlx/strparse.h"

శూన్య Curl_http_neg_init(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం http_negotiation *neg)
{
  memset(neg, 0, పరిమాణం(*neg));
  neg->accept_09 = దత్తాంశం->సమితి.http09_allowed;
  ఎంపిక(దత్తాంశం->సమితి.httpwant) {
  స్థితి CURL_HTTP_సంస్కరణ_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    ఆపు;
  స్థితి CURL_HTTP_సంస్కరణ_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    ఆపు;
  స్థితి CURL_HTTP_సంస్కరణ_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    ఆపు;
  స్థితి CURL_HTTP_సంస్కరణ_2_TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    ఆపు;
  స్థితి CURL_HTTP_సంస్కరణ_2_పూర్వ_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    దత్తాంశం->state.http_neg.h2_పూర్వ_knowledge = TRUE;
    ఆపు;
  స్థితి CURL_HTTP_సంస్కరణ_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    ఆపు;
  స్థితి CURL_HTTP_సంస్కరణ_3_ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    ఆపు;
  స్థితి CURL_HTTP_సంస్కరణ_NONE:
  అప్రమేయ:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    ఆపు;
  }
}

CURLcode Curl_http_అమరిక_conn(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  ఒకవేళ(దత్తాంశం->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode ఫలితం = Curl_conn_may_http3(దత్తాంశం, conn);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  తిరిగి CURLE_OK;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
/*
 * checkProxyHeaders() checks the linked list of custom proxy headers
 * if proxy headers are not available, then it will lookup into http header
 * link list
 *
 * It takes a connectdata struct as input to see if this is a proxy request or
 * not, as it then might check a different header list. Provide the header
 * prefix without colon!
 */
అక్షరం *Curl_checkProxyheaders(నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి నిర్మాణం connectdata *conn,
                             స్థిరరాశి అక్షరం *thisheader,
                             స్థిరరాశి size_t thislen)
{
  నిర్మాణం curl_slist *తల;

  కొరకు(తల = (conn->http_proxy.peer && దత్తాంశం->సమితి.sep_శీర్షికలు) ?
        దత్తాంశం->సమితి.proxyheaders : దత్తాంశం->సమితి.శీర్షికలు;
      తల; తల = తల->తదుపరి) {
    ఒకవేళ(curl_strnequal(తల->దత్తాంశం, thisheader, thislen) &&
       Curl_headersep(తల->దత్తాంశం[thislen]))
      తిరిగి తల->దత్తాంశం;
  }

  తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
స్థిర bool శీర్షిక_has_విలువ(స్థిరరాశి అక్షరం **headerp, నిర్మాణం Curl_str *outp)
{
  bool విలువ = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  ఒకవేళ(విలువ) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  తిరిగి విలువ;
}

స్థిర bool http_శీర్షిక_is_ఖాళీలు(స్థిరరాశి అక్షరం *శీర్షిక)
{
  నిర్మాణం Curl_str బహిర్గతం;

  ఒకవేళ(శీర్షిక_has_విలువ(&శీర్షిక, &బహిర్గతం)) {
    తిరిగి curlx_strlen(&బహిర్గతం) == 0;
  }
  తిరిగి TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
స్థిర CURLcode copy_custom_విలువ(స్థిరరాశి అక్షరం *శీర్షిక, అక్షరం **valp)
{
  నిర్మాణం Curl_str బహిర్గతం = { 0 };

  /* find the end of the header name */
  ఒకవేళ(శీర్షిక_has_విలువ(&శీర్షిక, &బహిర్గతం)) {
    *valp = curlx_memdup0(curlx_str(&బహిర్గతం), curlx_strlen(&బహిర్గతం));
    ఒకవేళ(*valp)
      తిరిగి CURLE_OK;
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  /* bad input */
  *valp = ఖాళీ;
  తిరిగి CURLE_BAD_FUNCTION_వాదం;
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
అక్షరం *Curl_copy_శీర్షిక_విలువ(స్థిరరాశి అక్షరం *శీర్షిక)
{
  నిర్మాణం Curl_str బహిర్గతం;

  /* find the end of the header name */
  ఒకవేళ(!curlx_str_until(&శీర్షిక, &బహిర్గతం, MAX_HTTP_RESP_శీర్షిక_కొలత, ':') &&
     !curlx_str_single(&శీర్షిక, ':')) {
    curlx_str_untilnl(&శీర్షిక, &బహిర్గతం, MAX_HTTP_RESP_శీర్షిక_కొలత);
    curlx_str_trimblanks(&బహిర్గతం);
    తిరిగి curlx_memdup0(curlx_str(&బహిర్గతం), curlx_strlen(&బహిర్గతం));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  తిరిగి ఖాళీ;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP_AUTH

#నిర్వచితం_కాకపోతే CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
స్థిర CURLcode http_output_basic(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn, bool proxy)
{
  size_t కొలత = 0;
  అక్షరం *authorization = ఖాళీ;
  అక్షరం **p_hd;
  CURLcode ఫలితం;
  నిర్మాణం Curl_creds *creds = ఖాళీ;
  అక్షరం *బహిర్గతం;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  ఒకవేళ(proxy) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    p_hd = &దత్తాంశం->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#లేకపోతే
    (శూన్య)conn;
    తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
  }
  లేకపోతే {
    p_hd = &దత్తాంశం->req.hd_auth;
    creds = దత్తాంశం->state.creds;
  }

  ఒకవేళ(!creds) {
    DEBUGASSERT(0);
    తిరిగి CURLE_FAILED_INIT;
  }

  బహిర్గతం = curl_maprintf("%s:%s", creds->user, creds->passwd);
  ఒకవేళ(!బహిర్గతం)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఫలితం = curlx_base64_encode((uint8_t *)బహిర్గతం, అక్షరశ్రేణి_పొడవు(బహిర్గతం),
                               &authorization, &కొలత);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  ఒకవేళ(!authorization) {
    ఫలితం = CURLE_REMOTE_ACCESS_DENIED;
    వెళ్ళు fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  ఒకవేళ(!*p_hd) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు fail;
  }

fail:
  curlx_free(బహిర్గతం);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
స్థిర CURLcode http_output_bearer(నిర్మాణం Curl_easy *దత్తాంశం)
{
  అక్షరం **userp;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(దత్తాంశం->state.creds));
  userp = &దత్తాంశం->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(దత్తాంశం->state.creds));

  ఒకవేళ(!*userp) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు fail;
  }

fail:
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

#అయితే_ముగింపు

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
స్థిర bool pickoneauth(నిర్మాణం auth *pick, గుర్తులేని దీర్ఘ ముసుగు,
                        నిర్మాణం Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  గుర్తులేని దీర్ఘ avail = pick->avail & pick->want & ముసుగు;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  ఒకవేళ(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#నిర్వచితం_కాకపోతే CURL_DISABLE_BEARER_AUTH
  లేకపోతే ఒకవేళ((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
  లేకపోతే ఒకవేళ((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#అయితే_ముగింపు
  లేకపోతే ఒకవేళ(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#నిర్వచితం_కాకపోతే CURL_DISABLE_BASIC_AUTH
  లేకపోతే ఒకవేళ((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_AWS
  లేకపోతే ఒకవేళ(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTPSIG
  లేకపోతే ఒకవేళ(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#అయితే_ముగింపు
  లేకపోతే {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  తిరిగి picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
స్థిర CURLcode http_perhapsrewind(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం connectdata *conn)
{
  curl_off_t bytessent = దత్తాంశం->req.writebytecount;
  curl_off_t expectsend = Curl_creader_మొత్తం_నిడివి(దత్తాంశం);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(దత్తాంశం);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!దత్తాంశం->req.upload_పూర్తి && !little_upload_remains);
  VERBOSE(స్థిరరాశి అక్షరం *ongoing_auth = ఖాళీ);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  ఒకవేళ(needs_rewind) {
    infof(దత్తాంశం, "Need to rewind upload for next request");
    Curl_creader_సమితి_rewind(దత్తాంశం, TRUE);
  }

  ఒకవేళ(conn->బిట్లు.close)
    /* If we already decided to close this connection, we cannot veto. */
    తిరిగి CURLE_OK;

  ఒకవేళ(abort_upload) {
    /* We would like to abort the upload - but should we? */
#నిర్వచితమైతే USE_NTLM
    ఒకవేళ((దత్తాంశం->state.authproxy.picked == CURLAUTH_NTLM) ||
       (దత్తాంశం->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      ఒకవేళ((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SPNEGO
    /* There is still data left to send */
    ఒకవేళ((దత్తాంశం->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (దత్తాంశం->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      ఒకవేళ((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#అయితే_ముగింపు
  }

  ఒకవేళ(abort_upload) {
    ఒకవేళ(upload_remain >= 0)
      infof(దత్తాంశం, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    లేకపోతే
      infof(దత్తాంశం, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    దత్తాంశం->req.కొలత = 0; /* do not download any more than 0 bytes */
    దత్తాంశం->req.http_bodyless = TRUE;
  }
  తిరిగి CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
స్థిర bool http_should_fail(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం httpcode)
{
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  ఒకవేళ(!దత్తాంశం->సమితి.http_fail_on_error)
    తిరిగి FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  ఒకవేళ(httpcode < 400)
    తిరిగి FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  ఒకవేళ(దత్తాంశం->state.resume_from && దత్తాంశం->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    తిరిగి FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  ఒకవేళ((httpcode != 401) && (httpcode != 407))
    తిరిగి TRUE;

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
  ఒకవేళ((httpcode == 401) && !దత్తాంశం->state.creds)
    తిరిగి TRUE;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ((httpcode == 407) && !దత్తాంశం->conn->http_proxy.creds)
    తిరిగి TRUE;
#అయితే_ముగింపు

  తిరిగి (bool)దత్తాంశం->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode ఫలితం = CURLE_OK;
  గుర్తులేని దీర్ఘ authmask = ~0UL;

  ఒకవేళ(!Curl_creds_has_oauth_bearer(దత్తాంశం->state.creds))
    authmask &= (గుర్తులేని దీర్ఘ)~CURLAUTH_BEARER;

  ఒకవేళ(100 <= దత్తాంశం->req.httpcode && దత్తాంశం->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    తిరిగి CURLE_OK;

  ఒకవేళ(దత్తాంశం->state.authproblem)
    తిరిగి దత్తాంశం->సమితి.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  ఒకవేళ(దత్తాంశం->state.creds &&
     ((దత్తాంశం->req.httpcode == 401) ||
      (దత్తాంశం->req.authneg && దత్తాంశం->req.httpcode < 300))) {
    pickhost = pickoneauth(&దత్తాంశం->state.authhost, authmask, దత్తాంశం->state.creds);
    ఒకవేళ(!pickhost)
      దత్తాంశం->state.authproblem = TRUE;
    లేకపోతే
      దత్తాంశం->info.httpauthpicked = దత్తాంశం->state.authhost.picked;
    ఒకవేళ(దత్తాంశం->state.authhost.picked == CURLAUTH_NTLM &&
       (దత్తాంశం->req.httpversion_పంపిన > 11)) {
      infof(దత్తాంశం, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      దత్తాంశం->state.http_neg.wanted = CURL_HTTP_V1x;
      దత్తాంశం->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ(conn->http_proxy.creds &&
     ((దత్తాంశం->req.httpcode == 407) ||
      (దత్తాంశం->req.authneg && దత్తాంశం->req.httpcode < 300))) {
    pickproxy = pickoneauth(&దత్తాంశం->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    ఒకవేళ(!pickproxy)
      దత్తాంశం->state.authproblem = TRUE;
    లేకపోతే
      దత్తాంశం->info.proxyauthpicked = దత్తాంశం->state.authproxy.picked;
  }
#అయితే_ముగింపు

  ఒకవేళ(pickhost || pickproxy) {
    ఫలితం = http_perhapsrewind(దత్తాంశం, conn);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(దత్తాంశం->req.newurl);
    /* clone URL */
    దత్తాంశం->req.newurl = Curl_bufref_dup(&దత్తాంశం->state.url);
    ఒకవేళ(!దత్తాంశం->req.newurl)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  లేకపోతే ఒకవేళ((దత్తాంశం->req.httpcode < 300) &&
          !దత్తాంశం->state.authhost.పూర్తి &&
          దత్తాంశం->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (దత్తాంశం->state.httpreq != HTTPREQ_GET) &&
          (దత్తాంశం->state.httpreq != HTTPREQ_తల)) {
    /* clone URL */
    దత్తాంశం->req.newurl = Curl_bufref_dup(&దత్తాంశం->state.url);
    ఒకవేళ(!దత్తాంశం->req.newurl)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    దత్తాంశం->state.authhost.పూర్తి = TRUE;
  }
  ఒకవేళ(http_should_fail(దత్తాంశం, దత్తాంశం->req.httpcode)) {
    failf(దత్తాంశం, "The requested URL returned error: %d",
          దత్తాంశం->req.httpcode);
    ఫలితం = CURLE_HTTP_RETURNED_ERROR;
  }

  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
స్థిర CURLcode output_auth_శీర్షికలు(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం connectdata *conn,
                                    నిర్మాణం auth *authstatus,
                                    స్థిరరాశి అక్షరం *request,
                                    స్థిరరాశి అక్షరం *మార్గం,
                                    bool proxy)
{
  స్థిరరాశి అక్షరం *auth = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)conn;

#నిర్వచితమైతే CURL_DISABLE_DIGEST_AUTH
  (శూన్య)request;
  (శూన్య)మార్గం;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_AWS
  ఒకవేళ((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    ఫలితం = Curl_output_aws_sigv4(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  లేకపోతే
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTPSIG
  ఒకవేళ((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    ఒకవేళ(Curl_auth_allowed_గమ్యం_host(దత్తాంశం)) {
      auth = "HTTPSIG";
      ఫలితం = Curl_output_httpsig(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    లేకపోతే
      authstatus->పూర్తి = TRUE;
  }
  లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SPNEGO
  ఒకవేళ(authstatus->picked == CURLAUTH_NEGOTIATE) {
    ఒకవేళ(
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(దత్తాంశం, conn,
                                        STRCONST("Proxy-authorization"))) ||
#అయితే_ముగింపు
      (!proxy && !Curl_checkheaders(దత్తాంశం, STRCONST("Authorization")) &&
       Curl_auth_allowed_గమ్యం_host(దత్తాంశం))) {
      auth = "Negotiate";
      ఫలితం = Curl_output_negotiate(దత్తాంశం, conn, proxy);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    లేకపోతే
      authstatus->పూర్తి = TRUE;
  }
  లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే USE_NTLM
  ఒకవేళ(authstatus->picked == CURLAUTH_NTLM) {
    ఒకవేళ(proxy || Curl_auth_allowed_గమ్యం_host(దత్తాంశం)) {
      auth = "NTLM";
      ఫలితం = Curl_output_ntlm(దత్తాంశం, proxy);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    లేకపోతే
      authstatus->పూర్తి = TRUE;
  }
  లేకపోతే
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
  ఒకవేళ(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    ఫలితం = Curl_output_digest(దత్తాంశం,
                                proxy,
                                (స్థిరరాశి గుర్తులేని అక్షరం *)request,
                                (స్థిరరాశి గుర్తులేని అక్షరం *)మార్గం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  లేకపోతే
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_BASIC_AUTH
  ఒకవేళ(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    ఒకవేళ(
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
       (proxy && conn->http_proxy.creds &&
        Curl_creds_has_user_or_pass(conn->http_proxy.creds) &&
        !Curl_checkProxyheaders(దత్తాంశం, conn,
                                STRCONST("Proxy-authorization"))) ||
#అయితే_ముగింపు
       (!proxy && దత్తాంశం->state.creds &&
        Curl_creds_has_user_or_pass(దత్తాంశం->state.creds) &&
        !Curl_checkheaders(దత్తాంశం, STRCONST("Authorization")))) {
      auth = "Basic";
      ఫలితం = http_output_basic(దత్తాంశం, conn, proxy);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->పూర్తి = TRUE;
  }
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_BEARER_AUTH
  ఒకవేళ(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    ఒకవేళ(!proxy && Curl_creds_has_oauth_bearer(దత్తాంశం->state.creds) &&
       !Curl_checkheaders(దత్తాంశం, STRCONST("Authorization"))) {
      auth = "Bearer";
      ఫలితం = http_output_bearer(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->పూర్తి = TRUE;
  }
#అయితే_ముగింపు

  ఒకవేళ(auth) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    ఒకవేళ(proxy)
      దత్తాంశం->info.proxyauthpicked = authstatus->picked;
    లేకపోతే
      దత్తాంశం->info.httpauthpicked = authstatus->picked;
    infof(దత్తాంశం, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (దత్తాంశం->state.creds ?
           దత్తాంశం->state.creds->user : ""));
#లేకపోతే
    (శూన్య)proxy;
    infof(దత్తాంశం, "Server auth using %s with user '%s'",
          auth, దత్తాంశం->state.creds ?
          దత్తాంశం->state.creds->user : "");
#అయితే_ముగింపు
    authstatus->multipass = !authstatus->పూర్తి;
  }
  లేకపోతే {
    authstatus->multipass = FALSE;
    ఒకవేళ(proxy)
      దత్తాంశం->info.proxyauthpicked = 0;
    లేకపోతే
      దత్తాంశం->info.httpauthpicked = 0;
  }

  తిరిగి ఫలితం;
}

CURLcode Curl_http_output_auth(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn,
                               స్థిరరాశి అక్షరం *request,
                               Curl_HttpReq httpreq,
                               స్థిరరాశి అక్షరం *మార్గం,
                               స్థిరరాశి అక్షరం *query,
                               bool is_connect)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం auth *authhost;
  నిర్మాణం auth *authproxy;
  స్థిరరాశి అక్షరం *మార్గం_and_query = మార్గం;
  అక్షరం *tmp_str = ఖాళీ;

  DEBUGASSERT(దత్తాంశం);
  authhost = &దత్తాంశం->state.authhost;
  authproxy = &దత్తాంశం->state.authproxy;

  ఒకవేళ(
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    (!conn->http_proxy.peer || !conn->http_proxy.creds) &&
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SPNEGO
    !(authhost->want & CURLAUTH_NEGOTIATE) &&
    !(authproxy->want & CURLAUTH_NEGOTIATE) &&
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTPSIG
    !(authhost->want & CURLAUTH_HTTPSIG) &&
#అయితే_ముగింపు
    !దత్తాంశం->state.creds) {
    /* no authentication with no user or password */
    authhost->పూర్తి = TRUE;
    authproxy->పూర్తి = TRUE;
    ఫలితం = CURLE_OK;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(query) {
    tmp_str = curl_maprintf("%s?%s", మార్గం, query);
    ఒకవేళ(!tmp_str) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
    మార్గం_and_query = tmp_str;
  }

  ఒకవేళ(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  ఒకవేళ(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  ఒకవేళ(conn->బిట్లు.origin_is_proxy || is_connect) {
    ఫలితం = output_auth_శీర్షికలు(దత్తాంశం, conn, authproxy, request,
                                 మార్గం_and_query, TRUE);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  లేకపోతే
#లేకపోతే
  (శూన్య)is_connect;
#అయితే_ముగింపు /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->పూర్తి = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  ఒకవేళ(దత్తాంశం->state.creds || Curl_auth_allowed_గమ్యం_host(దత్తాంశం))
    ఫలితం = output_auth_శీర్షికలు(దత్తాంశం, conn, authhost, request,
                                 మార్గం_and_query, FALSE);
  లేకపోతే
    authhost->పూర్తి = TRUE;

  ఒకవేళ(((authhost->multipass && !authhost->పూర్తి) ||
      (authproxy->multipass && !authproxy->పూర్తి)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_తల)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    దత్తాంశం->req.authneg = TRUE;
  }
  లేకపోతే
    దత్తాంశం->req.authneg = FALSE;

బహిర్గతం:
  curlx_free(tmp_str);
  తిరిగి ఫలితం;
}

#లేకపోతే /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn,
                               స్థిరరాశి అక్షరం *request,
                               Curl_HttpReq httpreq,
                               స్థిరరాశి అక్షరం *మార్గం,
                               స్థిరరాశి అక్షరం *query,
                               bool is_connect)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)conn;
  (శూన్య)request;
  (శూన్య)httpreq;
  (శూన్య)మార్గం;
  (శూన్య)query;
  (శూన్య)is_connect;
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP_AUTH, else */

#ఒకవేళ defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
స్థిర bool authcmp(స్థిరరాశి అక్షరం *auth, స్థిరరాశి అక్షరం *పంక్తి)
{
  /* the auth string must not have an alnum following */
  size_t n = అక్షరశ్రేణి_పొడవు(auth);
  తిరిగి curl_strnequal(auth, పంక్తి, n) && !ISALNUM(పంక్తి[n]);
}
#అయితే_ముగింపు

#నిర్వచితమైతే USE_SPNEGO
స్థిర CURLcode auth_spnego(నిర్మాణం Curl_easy *దత్తాంశం,
                            bool proxy,
                            స్థిరరాశి అక్షరం *auth,
                            నిర్మాణం auth *authp,
                            uint32_t *availp)
{
  ఒకవేళ((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    ఒకవేళ(authp->picked == CURLAUTH_NEGOTIATE) {
      నిర్మాణం connectdata *conn = దత్తాంశం->conn;
      CURLcode ఫలితం = Curl_ఆదానం_negotiate(దత్తాంశం, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      ఒకవేళ(!ఫలితం) {
        curlx_free(దత్తాంశం->req.newurl);
        దత్తాంశం->req.newurl = Curl_bufref_dup(&దత్తాంశం->state.url);
        ఒకవేళ(!దత్తాంశం->req.newurl)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        దత్తాంశం->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      లేకపోతే
        దత్తాంశం->state.authproblem = TRUE;
    }
  }
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు

#నిర్వచితమైతే USE_NTLM
స్థిర CURLcode auth_ntlm(నిర్మాణం Curl_easy *దత్తాంశం,
                          bool proxy,
                          స్థిరరాశి అక్షరం *auth,
                          నిర్మాణం auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  ఒకవేళ((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    ఒకవేళ(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode ఫలితం = Curl_ఆదానం_ntlm(దత్తాంశం, proxy, auth);
      ఒకవేళ(!ఫలితం)
        దత్తాంశం->state.authproblem = FALSE;
      లేకపోతే {
        ఒకవేళ(ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
          తిరిగి ఫలితం;
        infof(దత్తాంశం, "NTLM authentication problem, ignoring.");
        దత్తాంశం->state.authproblem = TRUE;
      }
    }
  }
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
స్థిర CURLcode auth_digest(నిర్మాణం Curl_easy *దత్తాంశం,
                            bool proxy,
                            స్థిరరాశి అక్షరం *auth,
                            నిర్మాణం auth *authp,
                            uint32_t *availp)
{
  ఒకవేళ(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(దత్తాంశం, "Ignoring duplicate digest auth header.");
  }
  లేకపోతే ఒకవేళ(Curl_auth_is_digest_supported()) {
    CURLcode ఫలితం;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    ఫలితం = Curl_ఆదానం_digest(దత్తాంశం, proxy, auth);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
        తిరిగి ఫలితం;
      infof(దత్తాంశం, "Digest authentication problem, ignoring.");
      దత్తాంశం->state.authproblem = TRUE;
    }
  }
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_BASIC_AUTH
స్థిర CURLcode auth_basic(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  ఒకవేళ(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(దత్తాంశం, "Basic authentication problem, ignoring.");
    దత్తాంశం->state.authproblem = TRUE;
  }
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_BEARER_AUTH
స్థిర CURLcode auth_bearer(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  ఒకవేళ(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(దత్తాంశం, "Bearer authentication problem, ignoring.");
    దత్తాంశం->state.authproblem = TRUE;
  }
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_ఆదానం_auth(నిర్మాణం Curl_easy *దత్తాంశం, bool proxy,
                              స్థిరరాశి అక్షరం *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#ఒకవేళ defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  నిర్మాణం auth *authp;
  CURLcode ఫలితం = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(దత్తాంశం);

  ఒకవేళ(proxy) {
    availp = &దత్తాంశం->info.proxyauthavail;
    authp = &దత్తాంశం->state.authproxy;
  }
  లేకపోతే {
    availp = &దత్తాంశం->info.httpauthavail;
    authp = &దత్తాంశం->state.authhost;
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

  అయితే(*auth) {
#నిర్వచితమైతే USE_SPNEGO
    ఒకవేళ(authcmp("Negotiate", auth))
      ఫలితం = auth_spnego(దత్తాంశం, proxy, auth, authp, availp);
#అయితే_ముగింపు
#నిర్వచితమైతే USE_NTLM
    ఒకవేళ(!ఫలితం && authcmp("NTLM", auth))
      ఫలితం = auth_ntlm(దత్తాంశం, proxy, auth, authp, availp);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
    ఒకవేళ(!ఫలితం && authcmp("Digest", auth))
      ఫలితం = auth_digest(దత్తాంశం, proxy, auth, authp, availp);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_BASIC_AUTH
    ఒకవేళ(!ఫలితం && authcmp("Basic", auth))
      ఫలితం = auth_basic(దత్తాంశం, authp, availp);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_BEARER_AUTH
    ఒకవేళ(authcmp("Bearer", auth))
      ఫలితం = auth_bearer(దత్తాంశం, authp, availp);
#అయితే_ముగింపు

    ఒకవేళ(ఫలితం)
      ఆపు;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    ఒకవేళ(auth) /* if we are on a comma, skip it */
      auth++;
    లేకపోతే
      ఆపు;
    curlx_str_passblanks(&auth);
  }
  తిరిగి ఫలితం;
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)proxy;
  (శూన్య)auth;
  /* nothing to do when disabled */
  తిరిగి CURLE_OK;
#అయితే_ముగింపు
}

స్థిర శూన్య http_switch_గమ్యం_get(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం సంకేతం_2)
{
  స్థిరరాశి అక్షరం *req = CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST);

  ఒకవేళ((req || దత్తాంశం->state.httpreq != HTTPREQ_GET) &&
     (దత్తాంశం->సమితి.http_follow_బహుళకం == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((శూన్య)సంకేతం_2);
    infof(దత్తాంశం, "Switch to GET because of %d response", సంకేతం_2);
    దత్తాంశం->state.http_ignorecustom = TRUE;
  }
  లేకపోతే ఒకవేళ(req && (దత్తాంశం->సమితి.http_follow_బహుళకం != CURLFOLLOW_FIRSTONLY))
    infof(దత్తాంశం, "Stick to %s instead of GET", req);

  దత్తాంశం->state.httpreq = HTTPREQ_GET;
  Curl_creader_సమితి_rewind(దత్తాంశం, FALSE);
}

#నిర్వచించు HTTPREQ_IS_POST(దత్తాంశం)                    \
  ((దత్తాంశం)->state.httpreq == HTTPREQ_POST ||      \
   (దత్తాంశం)->state.httpreq == HTTPREQ_POST_FORM || \
   (దత్తాంశం)->state.httpreq == HTTPREQ_POST_MIME)

స్థిర CURLcode సమితి_auto_referer(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLU *u;
  అక్షరం *referer = ఖాళీ;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&దత్తాంశం->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  ఒకవేళ(!u)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  uc = curl_url_సమితి(u, CURLUPART_URL,
                    Curl_bufref_ptr(&దత్తాంశం->state.url), 0);
  ఒకవేళ(!uc)
    uc = curl_url_సమితి(u, CURLUPART_FRAGMENT, ఖాళీ, 0);
  ఒకవేళ(!uc)
    uc = curl_url_సమితి(u, CURLUPART_USER, ఖాళీ, 0);
  ఒకవేళ(!uc)
    uc = curl_url_సమితి(u, CURLUPART_PASSWORD, ఖాళీ, 0);
  ఒకవేళ(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  ఒకవేళ(uc || !referer)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  Curl_bufref_సమితి(&దత్తాంశం->state.referer, referer, 0, curl_free);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode follow_parse_url(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *newurl,
                                 followtype type, అక్షరం **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(దత్తాంశం->state.uh);
  uc = curl_url_సమితి(దత్తాంశం->state.uh, CURLUPART_URL, newurl, (గుర్తులేని పూర్ణాంకం)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (దత్తాంశం->సమితి.మార్గం_as_is ? CURLU_మార్గం_AS_IS : 0)));
  *ucp = uc;
  ఒకవేళ(uc) {
    ఒకవేళ((uc == CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం) || (type != FOLLOW_FAKE)) {
      failf(దత్తాంశం, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      తిరిగి Curl_uc_గమ్యం_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    ఒకవేళ(!*follow_urlp)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  లేకపోతే {
    CURLU *u = curl_url();
    ఒకవేళ(!u)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    uc = curl_url_సమితి(u, CURLUPART_URL,
                      Curl_bufref_ptr(&దత్తాంశం->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    ఒకవేళ(!uc)
      uc = curl_url_get(దత్తాంశం->state.uh, CURLUPART_URL, follow_urlp, 0);
    ఒకవేళ(uc) {
      curl_url_cleanup(u);
      తిరిగి Curl_uc_గమ్యం_curlcode(uc);
    }

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, దత్తాంశం->state.uh);
      curl_url_cleanup(u);
      ఒకవేళ(!same_origin)
        Curl_auth_digest_cleanup(&దత్తాంశం->state.digest);
    }
#లేకపోతే
    curl_url_cleanup(u);
#అయితే_ముగింపు
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode follow_fake(నిర్మాణం Curl_easy *దత్తాంశం, అక్షరం *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  ఒకవేళ(!uc) {
    CURLU *u = curl_url();
    అక్షరం *nocred = ఖాళీ;

    ఒకవేళ(!u) {
      curlx_free(follow_url);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    ఒకవేళ(!curl_url_సమితి(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (దత్తాంశం->సమితి.మార్గం_as_is ? CURLU_మార్గం_AS_IS : 0)) &&
       !curl_url_సమితి(u, CURLUPART_USER, ఖాళీ, 0) &&
       !curl_url_సమితి(u, CURLUPART_PASSWORD, ఖాళీ, 0))
      (శూన్య)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_ఖాళీలు);
    curl_url_cleanup(u);
    ఒకవేళ(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  దత్తాంశం->info.wouldredirect = follow_url;

  ఒకవేళ(reachedmax) {
    failf(దత్తాంశం, "Maximum (%d) redirects followed", దత్తాంశం->సమితి.maxredirs);
    తిరిగి CURLE_TOO_MANY_REDIRECTS;
  }
  తిరిగి CURLE_OK;
}

స్థిర bool follow_switch_get(నిర్మాణం Curl_easy *దత్తాంశం)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  ఎంపిక(దత్తాంశం->info.httpcode) {
  స్థితి 301: /* Moved Permanently */
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
    ఒకవేళ(HTTPREQ_IS_POST(దత్తాంశం) && !దత్తాంశం->సమితి.post301) {
      http_switch_గమ్యం_get(దత్తాంశం, 301);
      తిరిగి TRUE;
    }
    ఆపు;
  స్థితి 302: /*దొరికింది*/
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
    ఒకవేళ(HTTPREQ_IS_POST(దత్తాంశం) && !దత్తాంశం->సమితి.post302) {
      http_switch_గమ్యం_get(దత్తాంశం, 302);
      తిరిగి TRUE;
    }
    ఆపు;
  స్థితి 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    ఒకవేళ(!HTTPREQ_IS_POST(దత్తాంశం) || !దత్తాంశం->సమితి.post303) {
      http_switch_గమ్యం_get(దత్తాంశం, 303);
      తిరిగి TRUE;
    }
    ఆపు;

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
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి FALSE;
}

CURLcode Curl_http_follow(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  అక్షరం *follow_url = ఖాళీ;
  CURLUcode uc;
  CURLcode ఫలితం;
  CURLcode rewind_ఫలితం;
  bool switch_గమ్యం_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  ఒకవేళ(type != FOLLOW_FAKE)
    దత్తాంశం->state.అభ్యర్థనలు++; /* count all real follows */
  ఒకవేళ(type == FOLLOW_REDIR) {
    ఒకవేళ((దత్తాంశం->సమితి.maxredirs != -1) &&
       (దత్తాంశం->state.followlocation >= దత్తాంశం->సమితి.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    లేకపోతే {
      దత్తాంశం->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      ఒకవేళ(దత్తాంశం->సమితి.http_auto_referer) {
        ఫలితం = సమితి_auto_referer(దత్తాంశం);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }
    }
  }

  ఒకవేళ((type != FOLLOW_RETRY) &&
     (దత్తాంశం->req.httpcode != 401) && (దత్తాంశం->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, ఖాళీ, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  ఫలితం = follow_parse_url(దత్తాంశం, newurl, type, &follow_url, &uc);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  DEBUGASSERT(follow_url);

  ఒకవేళ(type == FOLLOW_FAKE)
    తిరిగి follow_fake(దత్తాంశం, follow_url, uc, reachedmax);

  ఒకవేళ(disallowport)
    దత్తాంశం->state.allow_పోర్టు = FALSE;

  Curl_bufref_సమితి(&దత్తాంశం->state.url, follow_url, 0, curl_free);
  rewind_ఫలితం = Curl_req_soft_reset(&దత్తాంశం->req, దత్తాంశం);
  infof(దత్తాంశం, "Issue another request to this URL: '%s'", follow_url);
  ఒకవేళ((దత్తాంశం->సమితి.http_follow_బహుళకం == CURLFOLLOW_FIRSTONLY) &&
     !దత్తాంశం->state.http_ignorecustom &&
     CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST)) {
    దత్తాంశం->state.http_ignorecustom = TRUE;
    infof(దత్తాంశం, "Drop custom request method for next request");
  }

  switch_గమ్యం_get = follow_switch_get(దత్తాంశం);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  ఒకవేళ(rewind_ఫలితం && !switch_గమ్యం_get)
    తిరిగి rewind_ఫలితం;

  Curl_pgrs_సమయం(దత్తాంశం, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(దత్తాంశం);

  తిరిగి CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(స్థిరరాశి అక్షరం *headerline, /* line to check */
                        స్థిరరాశి అక్షరం *శీర్షిక, /* header keyword _with_ colon */
                        స్థిరరాశి size_t hlen, /* len of the keyword in bytes */
                        స్థిరరాశి అక్షరం *content, /* content string to find */
                        స్థిరరాశి size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  స్థిరరాశి అక్షరం *p;
  నిర్మాణం Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(శీర్షిక);
  DEBUGASSERT(content);

  ఒకవేళ(!curl_strnequal(headerline, శీర్షిక, hlen))
    తిరిగి FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  ఒకవేళ(curlx_str_cspn(&p, &val, "\r\n"))
    తిరిగి FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  ఒకవేళ(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    కొరకు(len = curlx_strlen(&val); len >= clen;) {
      నిర్మాణం Curl_str తదుపరి;
      స్థిరరాశి అక్షరం *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      ఒకవేళ(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        తిరిగి TRUE; /* match! */
      /* advance to the next comma */
      ఒకవేళ(curlx_str_until(&p, &తదుపరి, len, ',') ||
         curlx_str_single(&p, ','))
        ఆపు; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      చేయి
        curlx_str_passblanks(&p);
      అయితే(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      ఒకవేళ((size_t)(p - o) > len)
        ఆపు;
      len -= (p - o);
    }
  }
  తిరిగి FALSE; /* no match */
}

నిర్మాణం cr_exp100_ctx {
  నిర్మాణం Curl_creader super;
  నిర్మాణం curltime ప్రారంభం; /* time started waiting */
  గణన expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

స్థిర శూన్య http_exp100_continue(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_exp100_ctx *ctx = reader->ctx;
  ఒకవేళ(ctx->state > EXP100_SEND_దత్తాంశం) {
    ctx->state = EXP100_SEND_దత్తాంశం;
    Curl_expire_clear(దత్తాంశం, EXPIRE_100_TIMEOUT);
  }
}

స్థిర CURLcode cr_exp100_read(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_creader *reader,
                               అక్షరం *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  నిర్మాణం cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  ఎంపిక(ctx->state) {
  స్థితి EXP100_SENDING_REQUEST:
    ఒకవేళ(!Curl_req_sendbuf_ఖాళీలు(దత్తాంశం)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(దత్తాంశం, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      తిరిగి CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(దత్తాంశం, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", దత్తాంశం->సమితి.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->ప్రారంభం = *Curl_pgrs_now(దత్తాంశం);
    Curl_expire_సమితి(దత్తాంశం, EXPIRE_100_TIMEOUT,
                    దత్తాంశం->సమితి.expect_100_timeout, &ctx->ప్రారంభం);
    *nread = 0;
    *eos = FALSE;
    తిరిగి CURLE_OK;
  స్థితి EXP100_FAILED:
    DEBUGF(infof(దత్తాంశం, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    తిరిగి CURLE_READ_ERROR;
  స్థితి EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &ctx->ప్రారంభం);
    ఒకవేళ(ms < దత్తాంశం->సమితి.expect_100_timeout) {
      DEBUGF(infof(దత్తాంశం, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      తిరిగి CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(దత్తాంశం, reader);
    infof(దత్తాంశం, "Done waiting for 100-continue");
    FALLTHROUGH();
  అప్రమేయ:
    DEBUGF(infof(దత్తాంశం, "cr_exp100_read, pass through"));
    తిరిగి Curl_creader_read(దత్తాంశం, reader->తదుపరి, buf, blen, nread, eos);
  }
}

స్థిర శూన్య cr_exp100_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం Curl_creader *reader, పూర్ణాంకం premature)
{
  నిర్మాణం cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_దత్తాంశం;
  Curl_expire_clear(దత్తాంశం, EXPIRE_100_TIMEOUT);
}

స్థిర స్థిరరాశి నిర్మాణం Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_మొత్తం_నిడివి,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_పూర్తి,
  పరిమాణం(నిర్మాణం cr_exp100_ctx)
};

స్థిర CURLcode http_exp100_కూడు_reader(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *reader = ఖాళీ;
  CURLcode ఫలితం;

  ఫలితం = Curl_creader_create(&reader, దత్తాంశం, &cr_exp100, CURL_CR_PROTOCOL);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_creader_కూడు(దత్తాంశం, reader);
  ఒకవేళ(!ఫలితం) {
    నిర్మాణం cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  ఒకవేళ(ఫలితం && reader)
    Curl_creader_free(దత్తాంశం, reader);
  తిరిగి ఫలితం;
}

స్థిర శూన్య http_exp100_got100(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *r = Curl_creader_get_by_type(దత్తాంశం, &cr_exp100);
  ఒకవేళ(r)
    http_exp100_continue(దత్తాంశం, r);
}

స్థిర bool http_exp100_is_నిరీక్షణ(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *r = Curl_creader_get_by_type(దత్తాంశం, &cr_exp100);
  ఒకవేళ(r) {
    నిర్మాణం cr_exp100_ctx *ctx = r->ctx;
    తిరిగి ctx->state == EXP100_AWAITING_CONTINUE;
  }
  తిరిగి FALSE;
}

స్థిర శూన్య http_exp100_send_anyway(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *r = Curl_creader_get_by_type(దత్తాంశం, &cr_exp100);
  ఒకవేళ(r)
    http_exp100_continue(దత్తాంశం, r);
}

స్థిర bool http_exp100_is_selected(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *r = Curl_creader_get_by_type(దత్తాంశం, &cr_exp100);
  తిరిగి !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం easy_pollset *ps)
{
  /* write mode */
  తిరిగి Curl_pollset_కూడు_బహిర్గతం(దత్తాంశం, ps, దత్తాంశం->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(CURL_REQ_WANT_RECV(దత్తాంశం)) {
    ఫలితం = Curl_pollset_కూడు_లోపల(దత్తాంశం, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  ఒకవేళ(!ఫలితం && Curl_req_want_send(దత్తాంశం) && !http_exp100_is_నిరీక్షణ(దత్తాంశం)) {
    ఫలితం = Curl_pollset_కూడు_బహిర్గతం(దత్తాంశం, ps, conn->sock[FIRSTSOCKET]);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode http_write_శీర్షిక(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *hd, size_t hdlen)
{
  CURLcode ఫలితం;
  పూర్ణాంకం writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_లోపల, hd, hdlen);

  writetype = CLIENTWRITE_శీర్షిక |
    ((దత్తాంశం->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  ఫలితం = Curl_client_write(దత్తాంశం, writetype, hd, hdlen);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = Curl_bump_headersize(దత్తాంశం, hdlen, FALSE);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  దత్తాంశం->req.deductheadercount = (100 <= దత్తాంశం->req.httpcode &&
                                 199 >= దత్తాంశం->req.httpcode) ?
    దత్తాంశం->req.headerbytecount : 0;
  తిరిగి ఫలితం;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                        CURLcode స్థితి_2, bool premature)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  దత్తాంశం->state.authhost.multipass = FALSE;
  దత్తాంశం->state.authproxy.multipass = FALSE;

  ఒకవేళ(curlx_dyn_len(&దత్తాంశం->state.headerb)) {
    (శూన్య)http_write_శీర్షిక(దత్తాంశం, curlx_dyn_ptr(&దత్తాంశం->state.headerb),
                            curlx_dyn_len(&దత్తాంశం->state.headerb));
  }
  curlx_dyn_reset(&దత్తాంశం->state.headerb);

  ఒకవేళ(స్థితి_2)
    తిరిగి స్థితి_2;

  ఒకవేళ(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->బిట్లు.retry &&
     !దత్తాంశం->సమితి.connect_only &&
     (దత్తాంశం->req.bytecount +
      దత్తాంశం->req.headerbytecount -
      దత్తాంశం->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(దత్తాంశం, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    తిరిగి CURLE_GOT_NOTHING;
  }

  తిరిగి CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
స్థిర bool http_may_use_1_1(స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  ఒకవేళ(దత్తాంశం->state.http_neg.rcvd_min == 10)
    తిరిగి FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  ఒకవేళ(conn && conn->httpversion_చూసిన == 10)
    తిరిగి FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  ఒకవేళ(దత్తాంశం->state.http_neg.only_10 &&
     (!conn || conn->httpversion_చూసిన <= 10))
    తిరిగి FALSE;
  /* We are not restricted to use 1.0 only. */
  తిరిగి !దత్తాంశం->state.http_neg.only_10;
}

స్థిర గుర్తులేని అక్షరం http_request_సంస్కరణ(నిర్మాణం Curl_easy *దత్తాంశం)
{
  గుర్తులేని అక్షరం v = Curl_conn_http_సంస్కరణ(దత్తాంశం, దత్తాంశం->conn);
  ఒకవేళ(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(దత్తాంశం) ? 11 : 10;
  }
  తిరిగి v;
}

స్థిర స్థిరరాశి అక్షరం *get_http_string(పూర్ణాంకం httpversion)
{
  ఎంపిక(httpversion) {
  స్థితి 30:
    తిరిగి "3";
  స్థితి 20:
    తిరిగి "2";
  స్థితి 11:
    తిరిగి "1.1";
  అప్రమేయ:
    తిరిగి "1.0";
  }
}

CURLcode Curl_కూడు_custom_శీర్షికలు(నిర్మాణం Curl_easy *దత్తాంశం,
                                 bool is_connect, పూర్ణాంకం httpversion,
                                 నిర్మాణం dynbuf *req)
{
  నిర్మాణం curl_slist *h[2];
  నిర్మాణం curl_slist *శీర్షికలు;
  పూర్ణాంకం numlists = 1; /* by default */
  పూర్ణాంకం i;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  గణన Curl_proxy_use proxy;

  ఒకవేళ(is_connect)
    proxy = శీర్షిక_CONNECT;
  లేకపోతే
    proxy = దత్తాంశం->conn->బిట్లు.origin_is_proxy ? శీర్షిక_PROXY : శీర్షిక_SERVER;

  ఎంపిక(proxy) {
  స్థితి శీర్షిక_SERVER:
    h[0] = దత్తాంశం->సమితి.శీర్షికలు;
    ఆపు;
  స్థితి శీర్షిక_PROXY:
    h[0] = దత్తాంశం->సమితి.శీర్షికలు;
    ఒకవేళ(దత్తాంశం->సమితి.sep_శీర్షికలు) {
      h[1] = దత్తాంశం->సమితి.proxyheaders;
      numlists++;
    }
    ఆపు;
  స్థితి శీర్షిక_CONNECT:
    ఒకవేళ(దత్తాంశం->సమితి.sep_శీర్షికలు)
      h[0] = దత్తాంశం->సమితి.proxyheaders;
    లేకపోతే
      h[0] = దత్తాంశం->సమితి.శీర్షికలు;
    ఆపు;
  స్థితి శీర్షిక_CONNECT_UDP:
    ఒకవేళ(దత్తాంశం->సమితి.sep_శీర్షికలు)
      h[0] = దత్తాంశం->సమితి.proxyheaders;
    లేకపోతే
      h[0] = దత్తాంశం->సమితి.శీర్షికలు;
    ఆపు;
  }
#లేకపోతే
  (శూన్య)is_connect;
  h[0] = దత్తాంశం->సమితి.శీర్షికలు;
#అయితే_ముగింపు

  /* loop through one or two lists */
  కొరకు(i = 0; i < numlists; i++) {
    కొరకు(శీర్షికలు = h[i]; శీర్షికలు; శీర్షికలు = శీర్షికలు->తదుపరి) {
      CURLcode ఫలితం = CURLE_OK;
      bool blankheader = FALSE;
      నిర్మాణం Curl_str name;
      స్థిరరాశి అక్షరం *p = శీర్షికలు->దత్తాంశం;
      స్థిరరాశి అక్షరం *origp = p;
      size_t hlen = అక్షరశ్రేణి_పొడవు(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      ఒకవేళ(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      లేకపోతే {
        p = origp;
        ఒకవేళ(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          నిర్మాణం Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          ఒకవేళ(!curlx_strlen(&val))
            /* no content, do not send this */
            కొనసాగు;
        }
        లేకపోతే
          /* no colon */
          కొనసాగు;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      ఒకవేళ(దత్తాంశం->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      లేకపోతే ఒకవేళ(దత్తాంశం->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      లేకపోతే ఒకవేళ(దత్తాంశం->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      లేకపోతే ఒకవేళ(దత్తాంశం->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      లేకపోతే ఒకవేళ(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      లేకపోతే ఒకవేళ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      లేకపోతే ఒకవేళ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_గమ్యం_host(దత్తాంశం))
        ;
      లేకపోతే ఒకవేళ(blankheader) {
        ఫలితం = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        ఒకవేళ(!ఫలితం)
          ఫలితం = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      లేకపోతే
        ఫలితం = curlx_dyn_addf(req, "%s\r\n", origp);

      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
  }

  తిరిగి CURLE_OK;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_PARSEDATE
CURLcode Curl_కూడు_timecondition(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం dynbuf *req)
{
  స్థిరరాశి నిర్మాణం tm *tm;
  నిర్మాణం tm keeptime;
  CURLcode ఫలితం;
  అక్షరం datestr[80];
  స్థిరరాశి అక్షరం *condp;
  size_t len;

  ఒకవేళ(దత్తాంశం->సమితి.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    తిరిగి CURLE_OK;

  ఫలితం = curlx_gmtime(దత్తాంశం->సమితి.timevalue, &keeptime);
  ఒకవేళ(ఫలితం) {
    failf(దత్తాంశం, "Invalid TIMEVALUE");
    తిరిగి ఫలితం;
  }
  tm = &keeptime;

  ఎంపిక(దత్తాంశం->సమితి.timecondition) {
  అప్రమేయ:
    DEBUGF(infof(దత్తాంశం, "invalid time condition"));
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  స్థితి CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    ఆపు;
  స్థితి CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    ఆపు;
  స్థితి CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    ఆపు;
  }

  ఒకవేళ(Curl_checkheaders(దత్తాంశం, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    తిరిగి CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, పరిమాణం(datestr),
                 "%s: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                 condp,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);

  ఫలితం = curlx_dyn_కూడు(req, datestr);
  తిరిగి ఫలితం;
}
#లేకపోతే
/* disabled */
CURLcode Curl_కూడు_timecondition(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం dynbuf *req)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)req;
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు

శూన్య Curl_http_పద్ధతి(నిర్మాణం Curl_easy *దత్తాంశం,
                      స్థిరరాశి అక్షరం **పద్ధతి, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)దత్తాంశం->state.httpreq;
  స్థిరరాశి అక్షరం *request;
#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
  ఒకవేళ(దత్తాంశం->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  లేకపోతే
#అయితే_ముగింపు
  ఒకవేళ((దత్తాంశం->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     దత్తాంశం->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  ఒకవేళ(!దత్తాంశం->state.http_ignorecustom &&
     CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST);
  }
  లేకపోతే {
    ఒకవేళ(దత్తాంశం->req.no_body)
      request = "HEAD";
    లేకపోతే {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_తల));
      ఎంపిక(httpreq) {
      స్థితి HTTPREQ_POST:
      స్థితి HTTPREQ_POST_FORM:
      స్థితి HTTPREQ_POST_MIME:
        request = "POST";
        ఆపు;
      స్థితి HTTPREQ_PUT:
        request = "PUT";
        ఆపు;
      అప్రమేయ: /* this should never happen */
      స్థితి HTTPREQ_GET:
        request = "GET";
        ఆపు;
      స్థితి HTTPREQ_తల:
        request = "HEAD";
        ఆపు;
      }
    }
  }
  *పద్ధతి = request;
  *reqp = httpreq;
}

స్థిర CURLcode http_సమితి_aptr_host(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  స్థిరరాశి అక్షరం *ptr = ఖాళీ;

  curlx_safefree(దత్తాంశం->state.http_host);
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  curlx_safefree(దత్తాంశం->req.cookiehost);
#అయితే_ముగింపు

  ఒకవేళ(Curl_peer_equal(దత్తాంశం->state.initial_origin, దత్తాంశం->state.origin))
    ptr = Curl_checkheaders(దత్తాంశం, STRCONST("Host"));

  ఒకవేళ(ptr) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    అక్షరం *cookiehost;
    CURLcode ఫలితం = copy_custom_విలువ(ptr, &cookiehost);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఒకవేళ(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    లేకపోతే {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      ఒకవేళ(*cookiehost == '[') {
        అక్షరం *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, అక్షరశ్రేణి_పొడవు(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        ఒకవేళ(closingbracket)
          *closingbracket = 0;
      }
      లేకపోతే {
        పూర్ణాంకం startsearch = 0;
        అక్షరం *colon = strchr(cookiehost + startsearch, ':');
        ఒకవేళ(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      దత్తాంశం->req.cookiehost = cookiehost;
    }
#అయితే_ముగింపు

    ఒకవేళ(!curl_strequal("Host:", ptr)) {
      దత్తాంశం->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      ఒకవేళ(!దత్తాంశం->state.http_host)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }
  లేకపోతే {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)దత్తాంశం->state.origin->ipv6;
    నిర్మాణం dynbuf tmp;
    size_t hlen;
    CURLcode ఫలితం;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    ఫలితం = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    ఒకవేళ(!ఫలితం && ipv6)
      ఫలితం = curlx_dyn_addn(&tmp, STRCONST("["));
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_కూడు(&tmp, దత్తాంశం->state.origin->hostname);
    ఒకవేళ(!ఫలితం && ipv6)
      ఫలితం = curlx_dyn_addn(&tmp, STRCONST("]"));
    ఒకవేళ(!ఫలితం &&
       ((దత్తాంశం->state.origin->పోర్టు != దత్తాంశం->state.origin->scheme->defport) ||
       (దత్తాంశం->state.origin->scheme->family != conn->scheme->family))) {
      ఫలితం = curlx_dyn_addf(&tmp, ":%u", దత్తాంశం->state.origin->పోర్టు);
    }

    దత్తాంశం->state.http_host = ఫలితం ? ఖాళీ : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    తిరిగి ఫలితం;
  }
  తిరిగి CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
స్థిర CURLcode http_లక్ష్యం(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం dynbuf *r)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *మార్గం = దత్తాంశం->state.పైకి.మార్గం;
  స్థిరరాశి అక్షరం *query = దత్తాంశం->state.పైకి.query;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
#అయితే_ముగింపు

  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_లక్ష్యం)) {
    మార్గం = CURL_EASY_STR(దత్తాంశం, STRING_లక్ష్యం);
    query = ఖాళీ;
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ(conn->బిట్లు.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    అక్షరం *url;
    CURLU *h = curl_url_dup(దత్తాంశం->state.uh);
    ఒకవేళ(!h)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    ఒకవేళ(!దత్తాంశం->state.origin->ipv6 &&
       (దత్తాంశం->state.origin->user_hostname != దత్తాంశం->state.origin->hostname)) {
      uc = curl_url_సమితి(h, CURLUPART_HOST, దత్తాంశం->state.origin->hostname, 0);
      ఒకవేళ(uc) {
        curl_url_cleanup(h);
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
    }
    uc = curl_url_సమితి(h, CURLUPART_FRAGMENT, ఖాళీ, 0);
    ఒకవేళ(uc) {
      curl_url_cleanup(h);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    ఒకవేళ(దత్తాంశం->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_సమితి(h, CURLUPART_USER, ఖాళీ, 0);
      ఒకవేళ(uc) {
        curl_url_cleanup(h);
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      uc = curl_url_సమితి(h, CURLUPART_PASSWORD, ఖాళీ, 0);
      ఒకవేళ(uc) {
        curl_url_cleanup(h);
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
    }
    లేకపోతే ఒకవేళ(దత్తాంశం->state.creds && (దత్తాంశం->state.creds->ఆధారం != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_సమితి(h, CURLUPART_USER,
                        దత్తాంశం->state.creds->user, CURLU_URLENCODE);
      ఒకవేళ(!uc)
        uc = curl_url_సమితి(h, CURLUPART_PASSWORD,
                          దత్తాంశం->state.creds->passwd, CURLU_URLENCODE);
      ఒకవేళ(uc) {
        curl_url_cleanup(h);
        తిరిగి Curl_uc_గమ్యం_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_పోర్టు);
    ఒకవేళ(uc) {
      curl_url_cleanup(h);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    curl_url_cleanup(h);

    /* target or URL */
    ఫలితం = curlx_dyn_కూడు(r, CURL_EASY_STR(దత్తాంశం, STRING_లక్ష్యం) ?
      CURL_EASY_STR(దత్తాంశం, STRING_లక్ష్యం) : url);
    curlx_free(url);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ఒకవేళ((దత్తాంశం->state.origin->scheme == &Curl_scheme_ftp) &&
       దత్తాంశం->సమితి.proxy_transfer_బహుళకం) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = అక్షరశ్రేణి_పొడవు(మార్గం);
      bool type_present = FALSE;
      ఒకవేళ((len >= 7) && !memcmp(&మార్గం[len - 7], ";type=", 6)) {
        ఎంపిక(Curl_raw_toupper(మార్గం[len - 1])) {
        స్థితి 'A':
        స్థితి 'D':
        స్థితి 'I':
          type_present = TRUE;
          ఆపు;
        }
      }
      ఒకవేళ(!type_present) {
        ఫలితం = curlx_dyn_addf(r, ";type=%c",
                                దత్తాంశం->state.prefer_ascii ? 'a' : 'i');
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }
    }
  }

  లేకపోతే
#అయితే_ముగింపు
  {
    ఫలితం = curlx_dyn_కూడు(r, మార్గం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఒకవేళ(query)
      ఫలితం = curlx_dyn_addf(r, "?%s", query);
  }

  తిరిగి ఫలితం;
}

#ఒకవేళ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
స్థిర CURLcode సమితి_post_reader(నిర్మాణం Curl_easy *దత్తాంశం, Curl_HttpReq httpreq)
{
  CURLcode ఫలితం;

  ఎంపిక(httpreq) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_MIME
  స్థితి HTTPREQ_POST_MIME:
    దత్తాంశం->state.mimepost = దత్తాంశం->సమితి.mimepostp;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FORM_API
  స్థితి HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    ఒకవేళ(!దత్తాంశం->state.formp) {
      దత్తాంశం->state.formp = curlx_calloc(1, పరిమాణం(curl_mimepart));
      ఒకవేళ(!దత్తాంశం->state.formp)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      Curl_mime_cleanpart(దత్తాంశం->state.formp);
      ఫలితం = Curl_getformdata(దత్తాంశం, దత్తాంశం->state.formp, దత్తాంశం->సమితి.httppost,
                                దత్తాంశం->state.fread_func);
      ఒకవేళ(ఫలితం) {
        curlx_safefree(దత్తాంశం->state.formp);
        తిరిగి ఫలితం;
      }
      దత్తాంశం->state.mimepost = దత్తాంశం->state.formp;
    }
    ఆపు;
#అయితే_ముగింపు
  అప్రమేయ:
    దత్తాంశం->state.mimepost = ఖాళీ;
    ఆపు;
  }

  ఎంపిక(httpreq) {
  స్థితి HTTPREQ_POST_FORM:
  స్థితి HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#నిర్వచితం_కాకపోతే CURL_DISABLE_MIME
    ఒకవేళ(దత్తాంశం->state.mimepost) {
      స్థిరరాశి అక్షరం *cthdr = Curl_checkheaders(దత్తాంశం, STRCONST("Content-Type"));

      /* Read and seek body only. */
      దత్తాంశం->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      ఒకవేళ(cthdr)
        కొరకు(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      లేకపోతే ఒకవేళ(దత్తాంశం->state.mimepost->రకం == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_శీర్షికలు(దత్తాంశం->state.mimepost, దత్తాంశం->సమితి.శీర్షికలు, 0);
      ఫలితం = Curl_mime_prepare_శీర్షికలు(దత్తాంశం, దత్తాంశం->state.mimepost, cthdr,
                                         ఖాళీ, MIMESTRATEGY_FORM);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      curl_mime_శీర్షికలు(దత్తాంశం->state.mimepost, ఖాళీ, 0);
      ఫలితం = Curl_creader_సమితి_mime(దత్తాంశం, దత్తాంశం->state.mimepost);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
      ఫలితం = Curl_creader_సమితి_null(దత్తాంశం);
    }
    దత్తాంశం->state.infilesize = Curl_creader_మొత్తం_నిడివి(దత్తాంశం);
    తిరిగి ఫలితం;

  అప్రమేయ:
    తిరిగి Curl_creader_సమితి_null(దత్తాంశం);
  }
  /* never reached */
}
#అయితే_ముగింపు

స్థిర CURLcode సమితి_reader(నిర్మాణం Curl_easy *దత్తాంశం, Curl_HttpReq httpreq)
{
  CURLcode ఫలితం = CURLE_OK;
  curl_off_t postsize = దత్తాంశం->state.infilesize;

  DEBUGASSERT(దత్తాంశం->conn);

  ఒకవేళ(దత్తాంశం->req.authneg) {
    తిరిగి Curl_creader_సమితి_null(దత్తాంశం);
  }

  ఎంపిక(httpreq) {
  స్థితి HTTPREQ_PUT: /* Let's PUT the data to the server! */
    తిరిగి postsize ? Curl_creader_సమితి_fread(దత్తాంశం, postsize) :
      Curl_creader_సమితి_null(దత్తాంశం);

#ఒకవేళ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  స్థితి HTTPREQ_POST_FORM:
  స్థితి HTTPREQ_POST_MIME:
    తిరిగి సమితి_post_reader(దత్తాంశం, httpreq);
#అయితే_ముగింపు

  స్థితి HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    ఒకవేళ(!postsize) {
      ఫలితం = Curl_creader_సమితి_null(దత్తాంశం);
    }
    లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      ఒకవేళ(plen == SIZE_MAX)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      లేకపోతే ఒకవేళ(plen)
        ఫలితం = Curl_creader_సమితి_buf(దత్తాంశం, దత్తాంశం->సమితి.postfields, plen);
      లేకపోతే
        ఫలితం = Curl_creader_సమితి_null(దత్తాంశం);
    }
    లేకపోతే {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      అక్షరం *ptr = Curl_checkheaders(దత్తాంశం, STRCONST("Transfer-Encoding"));
      ఒకవేళ(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      ఫలితం = Curl_creader_సమితి_fread(దత్తాంశం, chunked ? -1 : postsize);
    }
    తిరిగి ఫలితం;

  అప్రమేయ:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    దత్తాంశం->state.infilesize = 0;
    తిరిగి Curl_creader_సమితి_null(దత్తాంశం);
  }
  /* not reached */
}

స్థిర CURLcode http_resume(నిర్మాణం Curl_easy *దత్తాంశం, Curl_HttpReq httpreq)
{
  ఒకవేళ((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     దత్తాంశం->state.resume_from) {
    failf(దత్తాంశం, "HTTP upload cannot be resumed");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode http_req_సమితి_TE(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం dynbuf *req,
                                పూర్ణాంకం httpversion)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *ptr;

  ptr = Curl_checkheaders(దత్తాంశం, STRCONST("Transfer-Encoding"));
  ఒకవేళ(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    దత్తాంశం->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    ఒకవేళ(దత్తాంశం->req.upload_chunky && (httpversion >= 20)) {
      infof(దత్తాంశం, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      దత్తాంశం->req.upload_chunky = FALSE;
    }
  }
  లేకపోతే {
    curl_off_t req_clen = Curl_creader_మొత్తం_నిడివి(దత్తాంశం);

    ఒకవేళ(req_clen < 0) {
      /* indeterminate request content length */
      ఒకవేళ(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        దత్తాంశం->req.upload_chunky = (httpversion < 20);
      }
      లేకపోతే {
        failf(దత్తాంశం, "Chunky upload is not supported by HTTP 1.0");
        తిరిగి CURLE_UPLOAD_FAILED;
      }
    }
    లేకపోతే {
      /* else, no chunky upload */
      దత్తాంశం->req.upload_chunky = FALSE;
    }

    ఒకవేళ(దత్తాంశం->req.upload_chunky)
      ఫలితం = curlx_dyn_కూడు(req, "Transfer-Encoding: chunked\r\n");
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode addexpect(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం dynbuf *r,
                          పూర్ణాంకం httpversion, bool *announced_exp100)
{
  CURLcode ఫలితం;
  అక్షరం *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  ఒకవేళ(దత్తాంశం->req.upgr101 != UPGR101_NONE)
    తిరిగి CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(దత్తాంశం, STRCONST("Expect"));
  ఒకవేళ(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  లేకపోతే ఒకవేళ(!దత్తాంశం->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_నిడివి(దత్తాంశం);
    ఒకవేళ(client_len > EXPECT_100_పరిమితి || client_len < 0) {
      ఫలితం = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      *announced_exp100 = TRUE;
    }
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode http_కూడు_content_hds(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం dynbuf *r,
                                     పూర్ణాంకం httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode ఫలితం = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(దత్తాంశం->conn);
  ఒకవేళ(దత్తాంశం->req.upload_chunky) {
    ఫలితం = Curl_httpchunk_కూడు_reader(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_మొత్తం_నిడివి(దత్తాంశం);
  ఎంపిక(httpreq) {
  స్థితి HTTPREQ_PUT:
  స్థితి HTTPREQ_POST:
#ఒకవేళ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  స్థితి HTTPREQ_POST_FORM:
  స్థితి HTTPREQ_POST_MIME:
#అయితే_ముగింపు
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    ఒకవేళ(req_clen >= 0 && !దత్తాంశం->req.upload_chunky &&
       (దత్తాంశం->req.authneg ||
        !Curl_checkheaders(దత్తాంశం, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      ఫలితం = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

#నిర్వచితం_కాకపోతే CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    ఒకవేళ(దత్తాంశం->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      నిర్మాణం curl_slist *hdr;

      కొరకు(hdr = దత్తాంశం->state.mimepost->curlheaders; hdr; hdr = hdr->తదుపరి) {
        ఫలితం = curlx_dyn_addf(r, "%s\r\n", hdr->దత్తాంశం);
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
      }
    }
#అయితే_ముగింపు
    ఒకవేళ(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(దత్తాంశం, STRCONST("Content-Type"))) {
      ఫలితం = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
    ఫలితం = addexpect(దత్తాంశం, r, httpversion, &announced_exp100);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }

  Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, req_clen);
  ఒకవేళ(announced_exp100)
    ఫలితం = http_exp100_కూడు_reader(దత్తాంశం);

బహిర్గతం:
  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES

స్థిర CURLcode http_cookies(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం dynbuf *r)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *addcookies = ఖాళీ;
  bool linecap = FALSE;
  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_COOKIE) &&
     !Curl_checkheaders(దత్తాంశం, STRCONST("Cookie")) &&
     Curl_auth_allowed_గమ్యం_host(దత్తాంశం))
    addcookies = CURL_EASY_STR(దత్తాంశం, STRING_COOKIE);

  ఒకవేళ(దత్తాంశం->cookies || addcookies) {
    నిర్మాణం Curl_llist list;
    పూర్ణాంకం లెక్క = 0;

    ఒకవేళ(దత్తాంశం->cookies && దత్తాంశం->state.cookie_engine) {
      bool okay;
      స్థిరరాశి అక్షరం *host = దత్తాంశం->req.cookiehost ?
        దత్తాంశం->req.cookiehost : దత్తాంశం->state.origin->hostname;
      Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE, CURL_తాళం_ACCESS_SINGLE);
      ఫలితం = Curl_cookie_getlist(దత్తాంశం, &okay, host, &list);
      ఒకవేళ(!ఫలితం && okay) {
        నిర్మాణం Curl_llist_కణుపు *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        కొరకు(n = Curl_llist_తల(&list); n; n = Curl_కణుపు_తదుపరి(n)) {
          నిర్మాణం Cookie *co = Curl_కణుపు_elem(n);
          ఒకవేళ(co->విలువ) {
            size_t కూడు;
            ఒకవేళ(!లెక్క) {
              ఫలితం = curlx_dyn_addn(r, STRCONST("Cookie: "));
              ఒకవేళ(ఫలితం)
                ఆపు;
            }
            కూడు = అక్షరశ్రేణి_పొడవు(co->name) + అక్షరశ్రేణి_పొడవు(co->విలువ) + 1;
            ఒకవేళ(clen + కూడు >= MAX_COOKIE_శీర్షిక_LEN) {
              infof(దత్తాంశం, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              ఆపు;
            }
            ఫలితం = curlx_dyn_addf(r, "%s%s=%s", లెక్క ? "; " : "",
                                    co->name, co->విలువ);
            ఒకవేళ(ఫలితం)
              ఆపు;
            clen += కూడు + (లెక్క ? 2 : 0);
            లెక్క++;
          }
        }
        Curl_llist_destroy(&list, ఖాళీ);
      }
      Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE);
    }
    ఒకవేళ(addcookies && !ఫలితం && !linecap) {
      ఒకవేళ(!లెక్క)
        ఫలితం = curlx_dyn_addn(r, STRCONST("Cookie: "));
      ఒకవేళ(!ఫలితం) {
        ఫలితం = curlx_dyn_addf(r, "%s%s", లెక్క ? "; " : "", addcookies);
        లెక్క++;
      }
    }
    ఒకవేళ(లెక్క && !ఫలితం)
      ఫలితం = curlx_dyn_addn(r, STRCONST("\r\n"));

    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  తిరిగి ఫలితం;
}
#లేకపోతే
#నిర్వచించు http_cookies(a, b) CURLE_OK
#అయితే_ముగింపు

స్థిర CURLcode http_range(నిర్మాణం Curl_easy *దత్తాంశం,
                           Curl_HttpReq httpreq)
{
  ఒకవేళ(దత్తాంశం->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    ఒకవేళ(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_తల)) &&
       !Curl_checkheaders(దత్తాంశం, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(దత్తాంశం->state.rangeline);
      దత్తాంశం->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 దత్తాంశం->state.range);
      ఒకవేళ(!దత్తాంశం->state.rangeline)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    లేకపోతే ఒకవేళ((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(దత్తాంశం, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_మొత్తం_నిడివి(దత్తాంశం);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(దత్తాంశం->state.rangeline);

      ఒకవేళ(దత్తాంశం->సమితి.సమితి_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        దత్తాంశం->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      లేకపోతే ఒకవేళ(దత్తాంశం->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t మొత్తం_len = దత్తాంశం->req.authneg ?
                               దత్తాంశం->state.infilesize :
                               (దత్తాంశం->state.resume_from + req_clen);
        దత్తాంశం->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        దత్తాంశం->state.range, మొత్తం_len - 1, మొత్తం_len);
      }
      లేకపోతే {
        /* Range was selected and then we pass the incoming range and append
           total size */
        దత్తాంశం->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        దత్తాంశం->state.range, req_clen);
      }
      ఒకవేళ(!దత్తాంశం->state.rangeline)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode http_firstwrite(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;

  ఒకవేళ(దత్తాంశం->req.newurl) {
    ఒకవేళ(conn->బిట్లు.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(దత్తాంశం);
      k->పూర్తి = TRUE;
      తిరిగి CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(దత్తాంశం, "Ignoring the response-body");
  }
  ఒకవేళ(దత్తాంశం->state.resume_from && !k->content_range &&
     (దత్తాంశం->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    ఒకవేళ(k->కొలత == దత్తాంశం->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(దత్తాంశం, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(దత్తాంశం);
      k->పూర్తి = TRUE;
      తిరిగి CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(దత్తాంశం, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    తిరిగి CURLE_RANGE_ERROR;
  }

  ఒకవేళ(దత్తాంశం->సమితి.timecondition && !దత్తాంశం->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(దత్తాంశం, k->timeofdoc)) {
    k->పూర్తి = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    దత్తాంశం->info.httpcode = 304;
    infof(దత్తాంశం, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    తిరిగి CURLE_OK;
  } /* we have a time condition */

  తిరిగి CURLE_OK;
}

స్థిర CURLcode http_check_new_conn(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  స్థిరరాశి అక్షరం *info_సంస్కరణ = ఖాళీ;
  స్థిరరాశి అక్షరం *alpn;
  CURLcode ఫలితం;

  alpn = Curl_conn_get_alpn_negotiated(దత్తాంశం, conn);
  ఒకవేళ(alpn && !strcmp("h3", alpn)) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    ఒకవేళ(!conn->బిట్లు.origin_is_proxy)
#అయితే_ముగింపు
      DEBUGASSERT(Curl_conn_http_సంస్కరణ(దత్తాంశం, conn) == 30);
    info_సంస్కరణ = "HTTP/3";
  }
  లేకపోతే ఒకవేళ(alpn && !strcmp("h2", alpn)) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    ఒకవేళ((Curl_conn_http_సంస్కరణ(దత్తాంశం, conn) != 20) &&
       conn->బిట్లు.origin_is_proxy) {
      ఫలితం = Curl_http2_switch(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    లేకపోతే
#అయితే_ముగింపు
    DEBUGASSERT(Curl_conn_http_సంస్కరణ(దత్తాంశం, conn) == 20);
    info_సంస్కరణ = "HTTP/2";
  }
  లేకపోతే {
    /* Check if user wants to use HTTP/2 with clear TCP */
    ఒకవేళ(Curl_http2_may_switch(దత్తాంశం)) {
      DEBUGF(infof(దత్తాంశం, "HTTP/2 over clean TCP"));
      ఫలితం = Curl_http2_switch(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      info_సంస్కరణ = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_చూసిన = 20;
      Curl_conn_సమితి_multiplex(conn);
    }
    లేకపోతే
      info_సంస్కరణ = "HTTP/1.x";
  }

  ఒకవేళ(info_సంస్కరణ)
    infof(దత్తాంశం, "using %s", info_సంస్కరణ);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode http_కూడు_connection_hd(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం dynbuf *req)
{
  నిర్మాణం curl_slist *తల;
  స్థిరరాశి అక్షరం *sep = "Connection: ";
  CURLcode ఫలితం = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  కొరకు(తల = దత్తాంశం->సమితి.శీర్షికలు; తల; తల = తల->తదుపరి) {
    ఒకవేళ(curl_strnequal(తల->దత్తాంశం, "Connection", 10) &&
       Curl_headersep(తల->దత్తాంశం[10]) &&
       !http_శీర్షిక_is_ఖాళీలు(తల->దత్తాంశం)) {
      అక్షరం *విలువ;
      ఫలితం = copy_custom_విలువ(తల->దత్తాంశం, &విలువ);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      ఫలితం = curlx_dyn_addf(req, "%s%s", sep, విలువ);
      sep = ", ";
      curlx_free(విలువ);
      ఆపు; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  ఒకవేళ(!ఫలితం && దత్తాంశం->state.http_hd_te) {
    ఫలితం = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  ఒకవేళ(!ఫలితం && దత్తాంశం->state.http_hd_upgrade) {
    ఫలితం = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  ఒకవేళ(!ఫలితం && దత్తాంశం->state.http_hd_h2_settings) {
    ఫలితం = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  ఒకవేళ(!ఫలితం && (rlen < curlx_dyn_len(req)))
    ఫలితం = curlx_dyn_addn(req, STRCONST("\r\n"));
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  కొరకు(తల = దత్తాంశం->సమితి.శీర్షికలు; తల; తల = తల->తదుపరి) {
    ఒకవేళ(curl_strnequal(తల->దత్తాంశం, "Connection", 10) &&
       Curl_headersep(తల->దత్తాంశం[10]) &&
       !http_శీర్షిక_is_ఖాళీలు(తల->దత్తాంశం)) {
      ఒకవేళ(skip) {
        skip = FALSE;
        కొనసాగు;
      }
      ఫలితం = curlx_dyn_addf(req, "%s\r\n", తల->దత్తాంశం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
  }

  తిరిగి CURLE_OK;
}

/* Header identifier in order we send them by default */
రకనిర్వచనం గణన {
  H1_HD_REQUEST,
  H1_HD_HOST,
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  H1_HD_PROXY_AUTH,
#అయితే_ముగింపు
  H1_HD_AUTH,
  H1_HD_RANGE,
  H1_HD_USER_AGENT,
  H1_HD_ACCEPT,
  H1_HD_TE,
  H1_HD_ACCEPT_ENCODING,
  H1_HD_REFERER,
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  H1_HD_PROXY_CONNECTION,
#అయితే_ముగింపు
  H1_HD_TRANSFER_ENCODING,
#నిర్వచితం_కాకపోతే CURL_DISABLE_ALTSVC
  H1_HD_ALT_వాడిన,
#అయితే_ముగింపు
  H1_HD_UPGRADE,
  H1_HD_COOKIES,
  H1_HD_CONDITIONALS,
  H1_HD_CUSTOM,
  H1_HD_CONTENT,
  H1_HD_CONNECTION,
  H1_HD_చివరి  /* the last, empty header line */
} http_hd_t;

స్థిర CURLcode http_కూడు_hd(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం dynbuf *req,
                            http_hd_t id,
                            గుర్తులేని అక్షరం httpversion,
                            స్థిరరాశి అక్షరం *పద్ధతి,
                            Curl_HttpReq httpreq)
{
  CURLcode ఫలితం = CURLE_OK;
#ఒకవేళ !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
#అయితే_ముగింపు
  ఎంపిక(id) {
  స్థితి H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    ఫలితం = curlx_dyn_addf(req, "%s ", పద్ధతి);
    ఒకవేళ(!ఫలితం)
      ఫలితం = http_లక్ష్యం(దత్తాంశం, req);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    ఆపు;

  స్థితి H1_HD_HOST:
    ఒకవేళ(దత్తాంశం->state.http_host) {
      ఫలితం = curlx_dyn_కూడు(req, దత్తాంశం->state.http_host);
      ఒకవేళ(!ఫలితం)
        ఫలితం = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    ఆపు;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  స్థితి H1_HD_PROXY_AUTH:
    ఒకవేళ(దత్తాంశం->req.hd_proxy_auth)
      ఫలితం = curlx_dyn_కూడు(req, దత్తాంశం->req.hd_proxy_auth);
    ఆపు;
#అయితే_ముగింపు

  స్థితి H1_HD_AUTH:
    ఒకవేళ(దత్తాంశం->req.hd_auth)
      ఫలితం = curlx_dyn_కూడు(req, దత్తాంశం->req.hd_auth);
    ఆపు;

  స్థితి H1_HD_RANGE:
    ఒకవేళ(దత్తాంశం->state.use_range && దత్తాంశం->state.rangeline)
      ఫలితం = curlx_dyn_కూడు(req, దత్తాంశం->state.rangeline);
    ఆపు;

  స్థితి H1_HD_USER_AGENT: {
    స్థిరరాశి అక్షరం *ua = CURL_EASY_STR(దత్తాంశం, STRING_USERAGENT);
    ఒకవేళ(ua && *ua && !Curl_checkheaders(దత్తాంశం, STRCONST("User-Agent")))
      ఫలితం = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    ఆపు;
  }

  స్థితి H1_HD_ACCEPT:
    ఒకవేళ(!Curl_checkheaders(దత్తాంశం, STRCONST("Accept")))
      ఫలితం = curlx_dyn_కూడు(req, "Accept: */*\r\n");
    ఆపు;

  స్థితి H1_HD_TE:
#నిర్వచితమైతే HAVE_LIBZ
    ఒకవేళ(!Curl_checkheaders(దత్తాంశం, STRCONST("TE")) &&
       దత్తాంశం->సమితి.http_transfer_encoding) {
      దత్తాంశం->state.http_hd_te = TRUE;
      ఫలితం = curlx_dyn_కూడు(req, "TE: gzip\r\n");
    }
#అయితే_ముగింపు
    ఆపు;

  స్థితి H1_HD_ACCEPT_ENCODING: {
    స్థిరరాశి అక్షరం *enc = CURL_EASY_STR(దత్తాంశం, STRING_ENCODING);
    ఒకవేళ(enc && !Curl_checkheaders(దత్తాంశం, STRCONST("Accept-Encoding")))
      ఫలితం = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    ఆపు;
  }

  స్థితి H1_HD_REFERER:
    ఒకవేళ(Curl_bufref_ptr(&దత్తాంశం->state.referer) &&
       !Curl_checkheaders(దత్తాంశం, STRCONST("Referer")))
      ఫలితం = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&దత్తాంశం->state.referer));
    ఆపు;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  స్థితి H1_HD_PROXY_CONNECTION:
    ఒకవేళ(conn->బిట్లు.origin_is_proxy &&
       !Curl_checkheaders(దత్తాంశం, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(దత్తాంశం, దత్తాంశం->conn, STRCONST("Proxy-Connection")))
      ఫలితం = curlx_dyn_కూడు(req, "Proxy-Connection: Keep-Alive\r\n");
    ఆపు;
#అయితే_ముగింపు

  స్థితి H1_HD_TRANSFER_ENCODING:
    ఫలితం = http_req_సమితి_TE(దత్తాంశం, req, httpversion);
    ఆపు;

#నిర్వచితం_కాకపోతే CURL_DISABLE_ALTSVC
  స్థితి H1_HD_ALT_వాడిన:
    ఒకవేళ(conn->బిట్లు.altused && conn->ద్వారా_peer &&
       !Curl_checkheaders(దత్తాంశం, STRCONST("Alt-Used")))
      ఫలితం = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->ద్వారా_peer->hostname, conn->ద్వారా_peer->పోర్టు);
    ఆపు;
#అయితే_ముగింపు

  స్థితి H1_HD_UPGRADE:
    ఒకవేళ(!Curl_conn_is_ssl(దత్తాంశం->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (దత్తాంశం->state.http_neg.wanted & CURL_HTTP_V2x) &&
       దత్తాంశం->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      ఫలితం = Curl_http2_request_upgrade(req, దత్తాంశం);
    }
#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
    ఒకవేళ(!ఫలితం && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      ఫలితం = Curl_ws_request(దత్తాంశం, req);
#అయితే_ముగింపు
    ఆపు;

  స్థితి H1_HD_COOKIES:
    ఫలితం = http_cookies(దత్తాంశం, req);
    ఆపు;

  స్థితి H1_HD_CONDITIONALS:
    ఫలితం = Curl_కూడు_timecondition(దత్తాంశం, req);
    ఆపు;

  స్థితి H1_HD_CUSTOM:
    ఫలితం = Curl_కూడు_custom_శీర్షికలు(దత్తాంశం, FALSE, httpversion, req);
    ఆపు;

  స్థితి H1_HD_CONTENT:
    ఫలితం = http_కూడు_content_hds(దత్తాంశం, req, httpversion, httpreq);
    ఆపు;

  స్థితి H1_HD_CONNECTION: {
    ఫలితం = http_కూడు_connection_hd(దత్తాంశం, req);
    ఆపు;
  }

  స్థితి H1_HD_చివరి:
    ఫలితం = curlx_dyn_addn(req, STRCONST("\r\n"));
    ఆపు;
  }
  తిరిగి ఫలితం;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  Curl_HttpReq httpreq;
  స్థిరరాశి అక్షరం *పద్ధతి;
  నిర్మాణం dynbuf req;
  గుర్తులేని అక్షరం httpversion;
  size_t hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *పూర్తి = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&దత్తాంశం->state.headerb);
  దత్తాంశం->state.maybe_folded = FALSE;

  ఒకవేళ(!దత్తాంశం->conn->బిట్లు.reuse) {
    ఫలితం = http_check_new_conn(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  ఫలితం = Curl_శీర్షికలు_init(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  దత్తాంశం->state.http_hd_te = FALSE;
  దత్తాంశం->state.http_hd_upgrade = FALSE;
  దత్తాంశం->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_పద్ధతి(దత్తాంశం, &పద్ధతి, &httpreq);

  /* select host to send */
  ఫలితం = http_సమితి_aptr_host(దత్తాంశం);
  /* setup the authentication headers, how that method and host are known */
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_http_output_auth(దత్తాంశం, దత్తాంశం->conn, పద్ధతి, httpreq,
                                   దత్తాంశం->state.పైకి.మార్గం,
                                   దత్తాంశం->state.పైకి.query, FALSE);
  /* Setup input reader, resume information and ranges */
  ఒకవేళ(!ఫలితం)
    ఫలితం = సమితి_reader(దత్తాంశం, httpreq);
  ఒకవేళ(!ఫలితం)
    ఫలితం = http_resume(దత్తాంశం, httpreq);
  ఒకవేళ(!ఫలితం)
    ఫలితం = http_range(దత్తాంశం, httpreq);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  httpversion = http_request_సంస్కరణ(దత్తాంశం);
  /* Add request line and all headers to `req` */
  కొరకు(hd_id = 0; hd_id <= H1_HD_చివరి; ++hd_id) {
    ఫలితం = http_కూడు_hd(దత్తాంశం, &req, (http_hd_t)hd_id,
                         httpversion, పద్ధతి, httpreq);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_అమరిక_sendrecv(దత్తాంశం, FIRSTSOCKET, -1);
  ఫలితం = Curl_req_send(దత్తాంశం, &req, httpversion);

  ఒకవేళ((httpversion >= 20) && దత్తాంశం->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    దత్తాంశం->req.upload_chunky = FALSE;

బహిర్గతం:
  ఒకవేళ(ఫలితం == CURLE_TOO_LARGE)
    failf(దత్తాంశం, "HTTP request too large");

  curlx_dyn_free(&req);
  తిరిగి ఫలితం;
}

రకనిర్వచనం గణన {
  స్థితి_UNKNOWN, /* not enough data to tell yet */
  స్థితి_పూర్తి, /* a status line was read */
  స్థితి_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
స్థిర bool checkprefixmax(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *బఫరు, size_t len)
{
  size_t ch = CURLMIN(అక్షరశ్రేణి_పొడవు(ఉపసర్గ), len);
  తిరిగి curl_strnequal(ఉపసర్గ, బఫరు, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
స్థిర statusline checkhttpprefix(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *s, size_t len)
{
  నిర్మాణం curl_slist *తల = దత్తాంశం->సమితి.http200aliases;
  statusline rc = స్థితి_BAD;
  statusline onmatch = len >= 5 ? స్థితి_పూర్తి : స్థితి_UNKNOWN;

  అయితే(తల) {
    ఒకవేళ(checkprefixmax(తల->దత్తాంశం, s, len)) {
      rc = onmatch;
      ఆపు;
    }
    తల = తల->తదుపరి;
  }

  ఒకవేళ((rc != స్థితి_పూర్తి) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  తిరిగి rc;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
స్థిర statusline checkrtspprefix(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *s, size_t len)
{
  statusline స్థితి_2 = స్థితి_BAD;
  statusline onmatch = len >= 5 ? స్థితి_పూర్తి : స్థితి_UNKNOWN;
  (శూన్య)దత్తాంశం;
  ఒకవేళ(checkprefixmax("RTSP/", s, len))
    స్థితి_2 = onmatch;

  తిరిగి స్థితి_2;
}
#అయితే_ముగింపు /* CURL_DISABLE_RTSP */

స్థిర statusline checkprotoprefix(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం connectdata *conn,
                                   స్థిరరాశి అక్షరం *s, size_t len)
{
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  ఒకవేళ(conn->scheme->protocol & CURLPROTO_RTSP)
    తిరిగి checkrtspprefix(దత్తాంశం, s, len);
#లేకపోతే
  (శూన్య)conn;
#అయితే_ముగింపు /* CURL_DISABLE_RTSP */

  తిరిగి checkhttpprefix(దత్తాంశం, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#నిర్వచించు HD_IS(hd, hdlen, n) \
  (((hdlen) >= (పరిమాణం(n) - 1)) && curl_strnequal(n, hd, పరిమాణం(n) - 1))

#నిర్వచించు HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (పరిమాణం(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, పరిమాణం(n) - 1)) ? ((hd) + (పరిమాణం(n) - 1)) : ఖాళీ)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#నిర్వచించు HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((పరిమాణం(n) - 1) + (పరిమాణం(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
స్థిర CURLcode http_శీర్షిక_a(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *hd, size_t hdlen)
{
#నిర్వచితం_కాకపోతే CURL_DISABLE_ALTSVC
  స్థిరరాశి అక్షరం *v;
  v = (దత్తాంశం->asi &&
       (Curl_xfer_is_secure(దత్తాంశం) ||
#నిర్వచితమైతే DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#లేకపోతే
        0
#అయితే_ముగింపు
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : ఖాళీ;
  ఒకవేళ(v) {
    /* the ALPN of the current request */
    నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
    గణన alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    తిరిగి Curl_altsvc_parse(దత్తాంశం, దత్తాంశం->asi, v, దత్తాంశం->state.origin, id);
  }
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)hd;
  (శూన్య)hdlen;
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
స్థిర CURLcode http_శీర్షిక_c(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *hd, size_t hdlen)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  స్థిరరాశి అక్షరం *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !దత్తాంశం->సమితి.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : ఖాళీ;
  ఒకవేళ(v) {
    చేయి {
      curl_off_t contentlength;
      పూర్ణాంకం offt = curlx_str_numblanks(&v, &contentlength);

      ఒకవేళ(offt == STRE_అధికప్రవాహం) {
        /* out of range */
        ఒకవేళ(దత్తాంశం->సమితి.max_filesize) {
          failf(దత్తాంశం, "Maximum file size exceeded");
          తిరిగి CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(దత్తాంశం, "Overflow Content-Length: value");
        తిరిగి CURLE_OK;
      }
      లేకపోతే {
        ఒకవేళ((offt == STRE_OK) &&
           ((k->కొలత == -1) || /* not set to something before */
            (k->కొలత == contentlength))) { /* or the same value */

          k->కొలత = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          ఒకవేళ(!curlx_str_single(&v, ','))
            కొనసాగు;

          ఒకవేళ(!curlx_str_newline(&v)) {
            k->maxdownload = k->కొలత;
            తిరిగి CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(దత్తాంశం, "Invalid Content-Length: value");
        తిరిగి CURLE_WEIRD_SERVER_జవాబు;
      }
    } అయితే(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(దత్తాంశం, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : ఖాళీ;
  ఒకవేళ(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    తిరిగి Curl_build_unencoding_దొంతర(దత్తాంశం, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  ఒకవేళ(v) {
    అక్షరం *contenttype = Curl_copy_శీర్షిక_విలువ(hd);
    ఒకవేళ(!contenttype)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఒకవేళ(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    లేకపోతే {
      curlx_free(దత్తాంశం->info.contenttype);
      దత్తాంశం->info.contenttype = contenttype;
    }
    తిరిగి CURLE_OK;
  }
  ఒకవేళ((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    తిరిగి CURLE_OK;
  }
  ఒకవేళ((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(దత్తాంశం, "HTTP/1.0 connection set to keep alive");
    తిరిగి CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : ఖాళీ;
  ఒకవేళ(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    స్థిరరాశి అక్షరం *ptr = v;

    /* Move forward until first digit or asterisk */
    అయితే(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    ఒకవేళ(ISDIGIT(*ptr)) {
      ఒకవేళ(!curlx_str_సంఖ్య(&ptr, &k->స్థానభ్రంశం, CURL_OFF_T_MAX) &&
         (దత్తాంశం->state.resume_from == k->స్థానభ్రంశం))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    లేకపోతే ఒకవేళ(k->httpcode < 300)
      దత్తాంశం->state.resume_from = 0; /* get everything */
  }
  తిరిగి CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
స్థిర CURLcode http_శీర్షిక_l(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *hd, size_t hdlen)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  స్థిరరాశి అక్షరం *v = (!k->http_bodyless &&
                   (దత్తాంశం->సమితి.timecondition || దత్తాంశం->సమితి.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : ఖాళీ;
  ఒకవేళ(v) {
    ఒకవేళ(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    ఒకవేళ(దత్తాంశం->సమితి.get_filetime)
      దత్తాంశం->info.filetime = k->timeofdoc;
    తిరిగి CURLE_OK;
  }
  ఒకవేళ(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    అక్షరం *location = Curl_copy_శీర్షిక_విలువ(hd);
    ఒకవేళ(!location)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఒకవేళ(!*location ||
       (దత్తాంశం->req.location && !strcmp(దత్తాంశం->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      తిరిగి CURLE_OK;
    }
    లేకపోతే {
      /* has value and is not an exact repeat */
      ఒకవేళ(దత్తాంశం->req.location) {
        failf(దత్తాంశం, "Multiple Location headers");
        curlx_free(location);
        తిరిగి CURLE_WEIRD_SERVER_జవాబు;
      }
      దత్తాంశం->req.location = location;

      ఒకవేళ((k->httpcode >= 300 && k->httpcode < 400) &&
         దత్తాంశం->సమితి.http_follow_బహుళకం) {
        CURLcode ఫలితం;
        DEBUGASSERT(!దత్తాంశం->req.newurl);
        దత్తాంశం->req.newurl = curlx_strdup(దత్తాంశం->req.location); /* clone */
        ఒకవేళ(!దత్తాంశం->req.newurl)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        ఫలితం = http_perhapsrewind(దత్తాంశం, conn);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;

        /* mark the next request as a followed location: */
        దత్తాంశం->state.this_is_a_follow = TRUE;
      }
    }
  }
  తిరిగి CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
స్థిర CURLcode http_శీర్షిక_p(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *hd, size_t hdlen)
{
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  స్థిరరాశి అక్షరం *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  ఒకవేళ(v) {
    నిర్మాణం connectdata *conn = దత్తాంశం->conn;
    ఒకవేళ((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(దత్తాంశం, "HTTP/1.0 proxy connection set to keep alive");
    }
    లేకపోతే ఒకవేళ((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(దత్తాంశం, "HTTP/1.1 proxy connection set close");
    }
    తిరిగి CURLE_OK;
  }
#అయితే_ముగింపు
  ఒకవేళ((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    అక్షరం *auth;
    CURLcode ఫలితం;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    ఒకవేళ(!దత్తాంశం->conn->బిట్లు.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      తిరిగి CURLE_OK;
#అయితే_ముగింపు
    auth = Curl_copy_శీర్షిక_విలువ(hd);
    ఫలితం = auth ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఒకవేళ(!ఫలితం) {
      ఫలితం = Curl_http_ఆదానం_auth(దత్తాంశం, TRUE, auth);
      curlx_free(auth);
    }
    తిరిగి ఫలితం;
  }
#నిర్వచితమైతే USE_SPNEGO
  ఒకవేళ(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    నిర్మాణం connectdata *conn = దత్తాంశం->conn;
    నిర్మాణం negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    నిర్మాణం auth *authp = &దత్తాంశం->state.authhost;
    ఒకవేళ(!negdata)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఒకవేళ(authp->picked == CURLAUTH_NEGOTIATE) {
      అక్షరం *persistentauth = Curl_copy_శీర్షిక_విలువ(hd);
      ఒకవేళ(!persistentauth)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(దత్తాంశం, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
స్థిర CURLcode http_శీర్షిక_r(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *hd, size_t hdlen)
{
  స్థిరరాశి అక్షరం *v = HD_VAL(hd, hdlen, "Retry-After:");
  ఒకవేళ(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    ఒకవేళ(!Curl_getdate_capped(v, &date)) {
      time_t current = సమయం(ఖాళీ);
      ఒకవేళ(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    లేకపోతే
      /* Try it as a decimal number, ignore errors */
      (శూన్య)curlx_str_సంఖ్య(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    ఒకవేళ(retry_after > 21600)
      retry_after = 21600;
    దత్తాంశం->info.retry_after = retry_after;
  }
  తిరిగి CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
స్థిర CURLcode http_శీర్షిక_s(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *hd, size_t hdlen)
{
#ఒకవేళ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  స్థిరరాశి అక్షరం *v;
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)hd;
  (శూన్య)hdlen;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  v = (దత్తాంశం->cookies && దత్తాంశం->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : ఖాళీ;
  ఒకవేళ(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    స్థిరరాశి అక్షరం *host = దత్తాంశం->req.cookiehost ?
      దత్తాంశం->req.cookiehost : దత్తాంశం->state.origin->hostname;
    స్థిరరాశి గుర్తులేని అక్షరం secure_context = Curl_secure_context(దత్తాంశం, host) ?
      COOKIE_SECURE : 0;
    CURLcode ఫలితం;
    Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE, CURL_తాళం_ACCESS_SINGLE);
    ఫలితం = Curl_cookie_కూడు(దత్తాంశం, దత్తాంశం->cookies, v, host,
                             దత్తాంశం->state.పైకి.మార్గం,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE);
    తిరిగి ఫలితం;
  }
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (దత్తాంశం->hsts &&
       (Curl_xfer_is_secure(దత్తాంశం) ||
#నిర్వచితమైతే DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#లేకపోతే
        0
#అయితే_ముగింపు
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : ఖాళీ;
  ఒకవేళ(v) {
    CURLcode ఫలితం = Curl_hsts_parse(
      దత్తాంశం->hsts, దత్తాంశం->state.origin->hostname, v);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
        తిరిగి ఫలితం;
      infof(దత్తాంశం, "Illegal STS header skipped");
    }
#నిర్వచితమైతే DEBUGBUILD
    లేకపోతే
      infof(దత్తాంశం, "Parsed STS header fine (%zu entries)",
            Curl_llist_లెక్క(&దత్తాంశం->hsts->list));
#అయితే_ముగింపు
  }
#అయితే_ముగింపు

  తిరిగి CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
స్థిర CURLcode http_శీర్షిక_t(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *hd, size_t hdlen)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;

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
  స్థిరరాశి అక్షరం *v = (!k->http_bodyless &&
                   (దత్తాంశం->state.httpreq != HTTPREQ_తల) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : ఖాళీ;
  ఒకవేళ(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode ఫలితం = Curl_build_unencoding_దొంతర(దత్తాంశం, v, TRUE);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఒకవేళ(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(దత్తాంశం, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    తిరిగి CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  ఒకవేళ(v) {
    దత్తాంశం->req.resp_trailer = TRUE;
    తిరిగి CURLE_OK;
  }
  తిరిగి CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
స్థిర CURLcode http_శీర్షిక_w(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *hd, size_t hdlen)
{
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    అక్షరం *auth = Curl_copy_శీర్షిక_విలువ(hd);
    ఒకవేళ(!auth)
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    లేకపోతే {
      ఫలితం = Curl_http_ఆదానం_auth(దత్తాంశం, FALSE, auth);
      curlx_free(auth);
    }
  }
  తిరిగి ఫలితం;
}

/*
 * http_header() parses a single response header.
 */
స్థిర CURLcode http_శీర్షిక(నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి అక్షరం *hd, size_t hdlen)
{
  CURLcode ఫలితం = CURLE_OK;

  ఎంపిక(hd[0]) {
  స్థితి 'a':
  స్థితి 'A':
    ఫలితం = http_శీర్షిక_a(దత్తాంశం, hd, hdlen);
    ఆపు;
  స్థితి 'c':
  స్థితి 'C':
    ఫలితం = http_శీర్షిక_c(దత్తాంశం, hd, hdlen);
    ఆపు;
  స్థితి 'l':
  స్థితి 'L':
    ఫలితం = http_శీర్షిక_l(దత్తాంశం, hd, hdlen);
    ఆపు;
  స్థితి 'p':
  స్థితి 'P':
    ఫలితం = http_శీర్షిక_p(దత్తాంశం, hd, hdlen);
    ఆపు;
  స్థితి 'r':
  స్థితి 'R':
    ఫలితం = http_శీర్షిక_r(దత్తాంశం, hd, hdlen);
    ఆపు;
  స్థితి 's':
  స్థితి 'S':
    ఫలితం = http_శీర్షిక_s(దత్తాంశం, hd, hdlen);
    ఆపు;
  స్థితి 't':
  స్థితి 'T':
    ఫలితం = http_శీర్షిక_t(దత్తాంశం, hd, hdlen);
    ఆపు;
  స్థితి 'w':
  స్థితి 'W':
    ఫలితం = http_శీర్షిక_w(దత్తాంశం, hd, hdlen);
    ఆపు;
  }

  ఒకవేళ(!ఫలితం) {
    నిర్మాణం connectdata *conn = దత్తాంశం->conn;
    ఒకవేళ(conn->scheme->protocol & CURLPROTO_RTSP)
      ఫలితం = Curl_rtsp_parseheader(దత్తాంశం, hd);
  }
  తిరిగి ఫలితం;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
స్థిర CURLcode http_statusline(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *conn)
{
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;

  ఎంపిక(k->httpversion) {
  స్థితి 10:
  స్థితి 11:
#నిర్వచితమైతే USE_HTTP2
  స్థితి 20:
#అయితే_ముగింపు
#నిర్వచితమైతే USE_HTTP3
  స్థితి 30:
#అయితే_ముగింపు
    /* no major version switch mid-connection */
    ఒకవేళ(k->httpversion_పంపిన &&
       (k->httpversion / 10 != k->httpversion_పంపిన / 10)) {
      failf(దత్తాంశం, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_పంపిన / 10, k->httpversion / 10);
      తిరిగి CURLE_WEIRD_SERVER_జవాబు;
    }
    ఆపు;
  అప్రమేయ:
    failf(దత్తాంశం, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
  }

  దత్తాంశం->info.httpcode = k->httpcode;
  దత్తాంశం->info.httpversion = k->httpversion;
  conn->httpversion_చూసిన = k->httpversion;

  ఒకవేళ(!దత్తాంశం->state.http_neg.rcvd_min ||
     దత్తాంశం->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    దత్తాంశం->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  ఒకవేళ(దత్తాంశం->state.resume_from && దత్తాంశం->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  ఒకవేళ(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(దత్తాంశం, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  ఎంపిక(k->httpcode) {
  స్థితి 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    ఒకవేళ(దత్తాంశం->సమితి.timecondition)
      దత్తాంశం->info.timecond = TRUE;
    FALLTHROUGH();
  స్థితి 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->కొలత = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
స్థిర CURLcode http_కొలత(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  ఒకవేళ(దత్తాంశం->req.ignore_cl || k->chunk) {
    k->కొలత = k->maxdownload = -1;
  }
  లేకపోతే ఒకవేళ(k->కొలత != -1) {
    ఒకవేళ(దత్తాంశం->సమితి.max_filesize &&
       !k->ignorebody &&
       (k->కొలత > దత్తాంశం->సమితి.max_filesize)) {
      failf(దత్తాంశం, "Maximum file size exceeded");
      తిరిగి CURLE_FILESIZE_EXCEEDED;
    }
    ఒకవేళ(k->ignorebody)
      infof(దత్తాంశం, "setting size while ignoring");
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, k->కొలత);
    k->maxdownload = k->కొలత;
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_verify_శీర్షిక(నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి అక్షరం *hd, size_t hdlen)
{
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  size_t i;
  bool has_colon = FALSE;
  కొరకు(i = 0; i < hdlen; i++) {
    ఎంపిక(hd[i]) {
    స్థితి '\0':
      /* this is bad, bail out */
      failf(దత్తాంశం, "NUL byte in header");
      తిరిగి CURLE_WEIRD_SERVER_జవాబు;
    స్థితి '\r':
      ఒకవేళ(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(దత్తాంశం, "Carriage return found in header");
        తిరిగి CURLE_WEIRD_SERVER_జవాబు;
      }
      ఆపు;
    స్థితి ':':
      has_colon = TRUE;
      ఆపు;
    }
  }
  ఒకవేళ(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    తిరిగి CURLE_OK;
  ఒకవేళ(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  లేకపోతే ఒకవేళ(!has_colon) {
    /* this is bad, bail out */
    failf(దత్తాంశం, "Header without colon");
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_bump_headersize(నిర్మాణం Curl_easy *దత్తాంశం,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  గుర్తులేని పూర్ణాంకం max = MAX_HTTP_RESP_శీర్షిక_కొలత;
  ఒకవేళ(delta < MAX_HTTP_RESP_శీర్షిక_కొలత) {
    దత్తాంశం->info.శీర్షిక_కొలత += (గుర్తులేని పూర్ణాంకం)delta;
    దత్తాంశం->req.allheadercount += (గుర్తులేని పూర్ణాంకం)delta;
    ఒకవేళ(!connect_only)
      దత్తాంశం->req.headerbytecount += (గుర్తులేని పూర్ణాంకం)delta;
    ఒకవేళ(దత్తాంశం->req.allheadercount > max)
      bad = దత్తాంశం->req.allheadercount;
    లేకపోతే ఒకవేళ(దత్తాంశం->info.శీర్షిక_కొలత > (max * 20)) {
      bad = దత్తాంశం->info.శీర్షిక_కొలత;
      max *= 20;
    }
  }
  లేకపోతే
    bad = దత్తాంశం->req.allheadercount + delta;
  ఒకవేళ(bad) {
    failf(దత్తాంశం, "Too large response headers: %zu > %u", bad, max);
    తిరిగి CURLE_RECV_ERROR;
  }
  తిరిగి CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
స్థిర CURLcode http_on_101_upgrade(నిర్మాణం Curl_easy *దత్తాంశం,
                                    స్థిరరాశి అక్షరం *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_మారింది)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;

#ఒకవేళ !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (శూన్య)buf;
  (శూన్య)blen;
  (శూన్య)pconsumed;
#లేకపోతే
  CURLcode ఫలితం;
  పూర్ణాంకం upgr101_requested = k->upgr101;
#అయితే_ముగింపు

  ఒకవేళ(k->httpversion_పంపిన != 11) {
    /* invalid for other HTTP versions */
    failf(దత్తాంశం, "server sent 101 response while not talking HTTP/1.1");
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->బిట్లు.upgrade_లోపల_పురోగతి = FALSE;
  *conn_మారింది = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#నిర్వచితమైతే USE_NGHTTP2
  ఒకవేళ(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(దత్తాంశం, "Received 101, Switching to HTTP/2");
    ఫలితం = Curl_http2_upgrade(దత్తాంశం, conn, FIRSTSOCKET, buf, blen);
    ఒకవేళ(!ఫలితం)
      *pconsumed += blen;
    తిరిగి ఫలితం;
  }
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
  ఒకవేళ(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(దత్తాంశం, "Received 101, Switching to WebSocket");
    ఫలితం = Curl_ws_accept(దత్తాంశం, buf, blen);
    ఒకవేళ(!ఫలితం)
      *pconsumed += blen; /* ws accept handled the data */
    తిరిగి ఫలితం;
  }
#అయితే_ముగింపు
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->శీర్షిక = FALSE;
  తిరిగి CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
స్థిర CURLcode http_on_1xx_response(నిర్మాణం Curl_easy *దత్తాంశం,
                                     స్థిరరాశి అక్షరం *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_మారింది)
{
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->శీర్షిక = TRUE;
  k->headerline = 0; /* restart the header line counter */

  ఎంపిక(k->httpcode) {
  స్థితి 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(దత్తాంశం);
    ఆపు;
  స్థితి 101:
    తిరిగి http_on_101_upgrade(దత్తాంశం, buf, blen, pconsumed, conn_మారింది);
  అప్రమేయ:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    ఆపు;
  }
  తిరిగి CURLE_OK;
}

#ఒకవేళ defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
స్థిర శూన్య http_check_auth_సంవృతం(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#నిర్వచితమైతే USE_NTLM
  ఒకవేళ(conn->బిట్లు.close &&
     (((దత్తాంశం->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((దత్తాంశం->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(దత్తాంశం, "Connection closure while negotiating auth (HTTP 1.0?)");
    దత్తాంశం->state.authproblem = TRUE;
  }
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SPNEGO
  ఒకవేళ(conn->బిట్లు.close &&
    (((దత్తాంశం->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((దత్తాంశం->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(దత్తాంశం, "Connection closure while negotiating auth (HTTP 1.0?)");
    దత్తాంశం->state.authproblem = TRUE;
  }
  ఒకవేళ((conn->http_negotiate_state == CURL_NEGO_AUTH_పూర్తి) &&
     (దత్తాంశం->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  ఒకవేళ((conn->proxy_negotiate_state == CURL_NEGO_AUTH_పూర్తి) &&
     (దత్తాంశం->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#అయితే_ముగింపు
}
#లేకపోతే
#నిర్వచించు http_check_auth_సంవృతం(x, y) /*ఖాళీలు*/
#అయితే_ముగింపు

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
స్థిర CURLcode http_handle_send_error(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!దత్తాంశం->req.authneg && !conn->బిట్లు.close &&
     !Curl_creader_will_rewind(దత్తాంశం)) {
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

    ఎంపిక(దత్తాంశం->state.httpreq) {
    స్థితి HTTPREQ_PUT:
    స్థితి HTTPREQ_POST:
    స్థితి HTTPREQ_POST_FORM:
    స్థితి HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      ఒకవేళ(!Curl_req_పూర్తి_sending(దత్తాంశం)) {
        ఒకవేళ((k->httpcode == 417) && http_exp100_is_selected(దత్తాంశం)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          ఒకవేళ(!k->writebytecount && http_exp100_is_నిరీక్షణ(దత్తాంశం)) {
            infof(దత్తాంశం, "Got HTTP failure 417 while waiting for a 100");
          }
          లేకపోతే {
            infof(దత్తాంశం, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            ఫలితం = http_perhapsrewind(దత్తాంశం, conn);
            ఒకవేళ(ఫలితం)
              తిరిగి ఫలితం;
          }
          దత్తాంశం->state.disableexpect = TRUE;
          Curl_req_abort_sending(దత్తాంశం);
          DEBUGASSERT(!దత్తాంశం->req.newurl);
          దత్తాంశం->req.newurl = Curl_bufref_dup(&దత్తాంశం->state.url);
          ఒకవేళ(!దత్తాంశం->req.newurl)
            తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        }
        లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.http_keep_sending_on_error) {
          infof(దత్తాంశం, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(దత్తాంశం);
        }
        లేకపోతే {
          infof(దత్తాంశం, "HTTP error before end of send, stop sending");
          streamclose(conn);
          ఫలితం = Curl_req_abort_sending(దత్తాంశం);
          ఒకవేళ(ఫలితం)
            తిరిగి ఫలితం;
        }
      }
      ఆపు;

    అప్రమేయ: /* default label present to avoid compiler warnings */
      ఆపు;
    }
  }

  ఒకవేళ(Curl_creader_will_rewind(దత్తాంశం) && !Curl_req_పూర్తి_sending(దత్తాంశం)) {
    /* We rewind before next send, continue sending now */
    infof(దత్తాంశం, "Keep sending data to get tossed away");
    CURL_REQ_సమితి_SEND(దత్తాంశం);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode http_on_response(నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి అక్షరం *చివరి_hd, size_t చివరి_hd_len,
                                 స్థిరరాశి అక్షరం *buf, size_t blen,
                                 size_t *pconsumed)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  bool conn_మారింది = FALSE;

  (శూన్య)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  ఒకవేళ(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    ఒకవేళ(దత్తాంశం->req.httpversion != 20)
      infof(దత్తాంశం, "Lying server, not serving HTTP/2");
  }

  ఒకవేళ(k->httpcode < 200 && చివరి_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    ఫలితం = http_write_శీర్షిక(దత్తాంశం, చివరి_hd, చివరి_hd_len);
    చివరి_hd = ఖాళీ; /* handled it */
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(k->httpcode < 100) {
    failf(దత్తాంశం, "Unsupported response code in HTTP response");
    ఫలితం = CURLE_UNSUPPORTED_PROTOCOL;
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ(k->httpcode < 200) {
    ఫలితం = http_on_1xx_response(దత్తాంశం, buf, blen, pconsumed, &conn_మారింది);
    వెళ్ళు బహిర్గతం;
  }

  /* k->httpcode >= 200, final response */
  k->శీర్షిక = FALSE;
  ఒకవేళ(conn->బిట్లు.upgrade_లోపల_పురోగతి) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->బిట్లు.upgrade_లోపల_పురోగతి = FALSE;
    conn_మారింది = TRUE;
  }

  ఒకవేళ((k->కొలత == -1) && !k->chunk && !conn->బిట్లు.close &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     దత్తాంశం->state.httpreq != HTTPREQ_తల) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(దత్తాంశం, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_సంవృతం(దత్తాంశం, conn);

#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  ఒకవేళ(దత్తాంశం->req.upgr101 == UPGR101_WS) {
    failf(దత్తాంశం, "Refused WebSocket upgrade: %d", k->httpcode);
    ఫలితం = CURLE_HTTP_RETURNED_ERROR;
    వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు

  /* Check if this response means the transfer errored. */
  ఒకవేళ(http_should_fail(దత్తాంశం, దత్తాంశం->req.httpcode)) {
    failf(దత్తాంశం, "The requested URL returned error: %d",
          k->httpcode);
    ఫలితం = CURLE_HTTP_RETURNED_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  ఫలితం = Curl_http_auth_act(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(k->httpcode >= 300) {
    ఫలితం = http_handle_send_error(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  /* final response without error, prepare to receive the body */
  ఫలితం = http_firstwrite(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  ఫలితం = http_కొలత(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  ఒకవేళ((conn->scheme->protocol & CURLPROTO_RTSP) && దత్తాంశం->req.no_body &&
     !దత్తాంశం->సమితి.opt_no_body && (k->కొలత > 0))
    దత్తాంశం->req.no_body = FALSE;
#అయితే_ముగింపు

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  ఒకవేళ(దత్తాంశం->req.no_body)
    k->download_పూర్తి = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  ఒకవేళ((k->maxdownload == 0) && (k->httpversion_పంపిన < 20))
    k->download_పూర్తి = TRUE;

బహిర్గతం:
  ఒకవేళ(చివరి_hd)
    /* if not written yet, write it now */
    ఫలితం = Curl_1st_fatal(ఫలితం,
                            http_write_శీర్షిక(దత్తాంశం, చివరి_hd, చివరి_hd_len));
  ఒకవేళ(conn_మారింది)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(దత్తాంశం->multi);
  తిరిగి ఫలితం;
}

స్థిర CURLcode http_rw_hd(నిర్మాణం Curl_easy *దత్తాంశం,
                           స్థిరరాశి అక్షరం *hd, size_t hdlen,
                           స్థిరరాశి అక్షరం *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  పూర్ణాంకం writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  ఒకవేళ((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    నిర్మాణం dynbuf చివరి_శీర్షిక;
    size_t consumed;

    curlx_dyn_init(&చివరి_శీర్షిక, hdlen + 1);
    ఫలితం = curlx_dyn_addn(&చివరి_శీర్షిక, hd, hdlen);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&దత్తాంశం->state.headerb);
    ఫలితం = http_on_response(దత్తాంశం, curlx_dyn_ptr(&చివరి_శీర్షిక),
                              curlx_dyn_len(&చివరి_శీర్షిక),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&చివరి_శీర్షిక);
    తిరిగి ఫలితం;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_శీర్షిక;
  ఒకవేళ(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    ఒకవేళ(దత్తాంశం->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      స్థిరరాశి అక్షరం *p = hd;

      curlx_str_passblanks(&p);
      ఒకవేళ(!అక్షరశ్రేణి_పోలిక(p, "HTTP/", 5)) {
        p += 5;
        ఎంపిక(*p) {
        స్థితి '1':
          p++;
          ఒకవేళ((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            ఒకవేళ(ISBLANK(p[2])) {
              k->httpversion = (గుర్తులేని అక్షరం)(10 + (p[1] - '0'));
              p += 3;
              ఒకవేళ(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          ఒకవేళ(!fine_statusline) {
            failf(దత్తాంశం, "Unsupported HTTP/1 subversion in response");
            తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
          }
          ఆపు;
        స్థితి '2':
        స్థితి '3':
          ఒకవేళ(!ISBLANK(p[1]))
            ఆపు;
          k->httpversion = (గుర్తులేని అక్షరం)((*p - '0') * 10);
          p += 2;
          ఒకవేళ(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            ఒకవేళ(!ISBLANK(*p))
              ఆపు;
            fine_statusline = TRUE;
          }
          ఆపు;
        అప్రమేయ: /* unsupported */
          failf(దత్తాంశం, "Unsupported HTTP version in response");
          తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      ఒకవేళ(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(దత్తాంశం, hd, hdlen);
        ఒకవేళ(check == స్థితి_పూర్తి) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    లేకపోతే ఒకవేళ(దత్తాంశం->conn->scheme->protocol & CURLPROTO_RTSP) {
      స్థిరరాశి అక్షరం *p = hd;
      నిర్మాణం Curl_str ver;
      curl_off_t స్థితి_2;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      ఒకవేళ(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_సంఖ్య(&p, &స్థితి_2, 999)) {
        curlx_str_trimblanks(&ver);
        ఒకవేళ(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (పూర్ణాంకం)స్థితి_2;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      ఒకవేళ(!fine_statusline)
        తిరిగి CURLE_WEIRD_SERVER_జవాబు;
    }

    ఒకవేళ(fine_statusline) {
      ఫలితం = http_statusline(దత్తాంశం, దత్తాంశం->conn);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      writetype |= CLIENTWRITE_స్థితి;
    }
    లేకపోతే {
      k->శీర్షిక = FALSE;   /* this is not a header line */
      తిరిగి CURLE_WEIRD_SERVER_జవాబు;
    }
  }

  ఫలితం = Curl_verify_శీర్షిక(దత్తాంశం, hd, hdlen);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = http_శీర్షిక(దత్తాంశం, hd, hdlen);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_లోపల, hd, hdlen);

  ఒకవేళ(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  ఫలితం = Curl_client_write(దత్తాంశం, writetype, hd, hdlen);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = Curl_bump_headersize(దత్తాంశం, hdlen, FALSE);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  తిరిగి CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
శూన్య Curl_http_గమ్యం_fold(నిర్మాణం dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  స్థిరరాశి అక్షరం *hd = curlx_dyn_ptr(bf);
  ఒకవేళ(len && (hd[len - 1] == '\n'))
    len--;
  ఒకవేళ(len && (hd[len - 1] == '\r'))
    len--;
  అయితే(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

స్థిర శూన్య unfold_శీర్షిక(నిర్మాణం Curl_easy *దత్తాంశం)
{
  Curl_http_గమ్యం_fold(&దత్తాంశం->state.headerb);
  దత్తాంశం->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
స్థిర CURLcode http_parse_శీర్షికలు(నిర్మాణం Curl_easy *దత్తాంశం,
                                   స్థిరరాశి అక్షరం *buf, size_t blen,
                                   size_t *pconsumed)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  స్థిరరాశి అక్షరం *చివర_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  ఒకవేళ(దత్తాంశం->state.maybe_folded && blen) {
    ఒకవేళ(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_శీర్షిక(దత్తాంశం);
    }
    లేకపోతే {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      ఫలితం = http_rw_hd(దత్తాంశం, curlx_dyn_ptr(&దత్తాంశం->state.headerb),
                          curlx_dyn_len(&దత్తాంశం->state.headerb),
                          ఖాళీ, 0, &ignore_this);
      curlx_dyn_reset(&దత్తాంశం->state.headerb);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    దత్తాంశం->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  అయితే(blen && k->శీర్షిక) {
    size_t consumed;
    size_t hlen;
    స్థిరరాశి అక్షరం *hd;
    size_t unfold_len = 0;

    ఒకవేళ(దత్తాంశం->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      అయితే(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      ఒకవేళ(blen) {
        /* insert a single space */
        ఫలితం = curlx_dyn_addn(&దత్తాంశం->state.headerb, " ", 1);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
        దత్తాంశం->state.leading_unfold = FALSE; /* done now */
      }
    }

    చివర_ptr = memchr(buf, '\n', blen);
    ఒకవేళ(!చివర_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      ఫలితం = curlx_dyn_addn(&దత్తాంశం->state.headerb, buf, blen);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      *pconsumed += blen + unfold_len;

      ఒకవేళ(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(దత్తాంశం, conn,
                           curlx_dyn_ptr(&దత్తాంశం->state.headerb),
                           curlx_dyn_len(&దత్తాంశం->state.headerb));

        ఒకవేళ(st == స్థితి_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->శీర్షిక = FALSE;
          streamclose(conn);
          ఒకవేళ((k->httpversion >= 10) || conn->బిట్లు.reuse) {
            failf(దత్తాంశం, "Invalid status line");
            తిరిగి CURLE_WEIRD_SERVER_జవాబు;
          }
          ఒకవేళ(!దత్తాంశం->state.http_neg.accept_09) {
            failf(దత్తాంశం, "Received HTTP/0.9 when not allowed");
            తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          వెళ్ళు బహిర్గతం;
        }
      }
      వెళ్ళు బహిర్గతం; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (చివర_ptr - buf) + 1;

    ఫలితం = curlx_dyn_addn(&దత్తాంశం->state.headerb, buf, consumed);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&దత్తాంశం->state.headerb);
    hd = curlx_dyn_ptr(&దత్తాంశం->state.headerb);

    ఒకవేళ(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(దత్తాంశం, conn, hd, hlen);
      ఒకవేళ(st == స్థితి_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        ఒకవేళ((k->httpversion >= 10) || conn->బిట్లు.reuse) {
          failf(దత్తాంశం, "Invalid status line");
          తిరిగి CURLE_WEIRD_SERVER_జవాబు;
        }
        ఒకవేళ(!దత్తాంశం->state.http_neg.accept_09) {
          failf(దత్తాంశం, "Received HTTP/0.9 when not allowed");
          తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->శీర్షిక = FALSE;
        leftover_body = TRUE;
        వెళ్ళు బహిర్గతం;
      }
    }
    లేకపోతే {
      ఒకవేళ(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        ఒకవేళ(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_శీర్షిక(దత్తాంశం);
          కొనసాగు;
        }
        లేకపోతే ఒకవేళ(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          దత్తాంశం->state.maybe_folded = TRUE;
          ఆపు;
        }
      }
    }

    ఫలితం = http_rw_hd(దత్తాంశం, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&దత్తాంశం->state.headerb);
    ఒకవేళ(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
బహిర్గతం:
  ఒకవేళ(!k->శీర్షిక && !leftover_body) {
    curlx_dyn_free(&దత్తాంశం->state.headerb);
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి అక్షరం *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode ఫలితం;
  size_t consumed;
  అక్షరం tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  ఫలితం = http_rw_hd(దత్తాంశం, hd, hdlen, &tmp, 0, &consumed);
  ఒకవేళ(!ఫలితం && is_eos) {
    ఫలితం = Curl_client_write(దత్తాంశం, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  తిరిగి ఫలితం;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *buf, size_t blen,
                                  size_t *pconsumed)
{
  ఒకవేళ(!దత్తాంశం->req.శీర్షిక) {
    *pconsumed = 0;
    తిరిగి CURLE_OK;
  }
  లేకపోతే {
    CURLcode ఫలితం;

    ఫలితం = http_parse_శీర్షికలు(దత్తాంశం, buf, blen, pconsumed);
    ఒకవేళ(!ఫలితం && !దత్తాంశం->req.శీర్షిక) {
      ఒకవేళ(!దత్తాంశం->req.no_body && curlx_dyn_len(&దత్తాంశం->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&దత్తాంశం->state.headerb),
                                   curlx_dyn_len(&దత్తాంశం->state.headerb));
      }
      curlx_dyn_free(&దత్తాంశం->state.headerb);
    }
    తిరిగి ఫలితం;
  }
}

CURLcode Curl_http_write_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *buf, size_t blen,
                              bool is_eos)
{
  CURLcode ఫలితం;
  size_t consumed;
  పూర్ణాంకం flags;

  ఫలితం = Curl_http_write_resp_hds(దత్తాంశం, buf, blen, &consumed);
  ఒకవేళ(ఫలితం || దత్తాంశం->req.పూర్తి)
    వెళ్ళు బహిర్గతం;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !దత్తాంశం->req.శీర్షిక);
  ఒకవేళ(!దత్తాంశం->req.శీర్షిక && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    ఒకవేళ(is_eos)
      flags |= CLIENTWRITE_EOS;
    ఫలితం = Curl_client_write(దత్తాంశం, flags, buf, blen);
  }
బహిర్గతం:
  తిరిగి ఫలితం;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_స్థితి(int32_t *pstatus, స్థిరరాశి అక్షరం *s, size_t len)
{
  CURLcode ఫలితం = CURLE_BAD_FUNCTION_వాదం;
  int32_t స్థితి_2 = 0;
  పూర్ణాంకం i;

  ఒకవేళ(len != 3)
    వెళ్ళు బహిర్గతం;

  కొరకు(i = 0; i < 3; ++i) {
    అక్షరం c = s[i];

    ఒకవేళ(c < '0' || c > '9')
      వెళ్ళు బహిర్గతం;

    స్థితి_2 *= 10;
    స్థితి_2 += c - '0';
  }
  ఫలితం = CURLE_OK;
బహిర్గతం:
  *pstatus = ఫలితం ? -1 : స్థితి_2;
  తిరిగి ఫలితం;
}

CURLcode Curl_http_req_make(నిర్మాణం httpreq **preq,
                            స్థిరరాశి అక్షరం *పద్ధతి, size_t m_len,
                            స్థిరరాశి అక్షరం *scheme, size_t s_len,
                            స్థిరరాశి అక్షరం *authority, size_t a_len,
                            స్థిరరాశి అక్షరం *మార్గం, size_t p_len)
{
  నిర్మాణం httpreq *req;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  DEBUGASSERT(పద్ధతి && m_len);

  req = curlx_calloc(1, పరిమాణం(*req) + m_len);
  ఒకవేళ(!req)
    వెళ్ళు బహిర్గతం;
#ఒకవేళ defined(__GNUC__) && __GNUC__ >= 13
#సూచన GCC diagnostic నెట్టు
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#సూచన GCC diagnostic ignored "-Warray-bounds"
#అయితే_ముగింపు
  జ్ఞాపకం_నకలు(req->పద్ధతి, పద్ధతి, m_len);
#ఒకవేళ defined(__GNUC__) && __GNUC__ >= 13
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు
  ఒకవేళ(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    ఒకవేళ(!req->scheme)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    ఒకవేళ(!req->authority)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(మార్గం) {
    req->మార్గం = curlx_memdup0(మార్గం, p_len);
    ఒకవేళ(!req->మార్గం)
      వెళ్ళు బహిర్గతం;
  }
  Curl_dynhds_init(&req->శీర్షికలు, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  ఫలితం = CURLE_OK;

బహిర్గతం:
  ఒకవేళ(ఫలితం && req)
    Curl_http_req_free(req);
  *preq = ఫలితం ? ఖాళీ : req;
  తిరిగి ఫలితం;
}

స్థిర CURLcode req_కేటాయించు_url_authority(నిర్మాణం httpreq *req, CURLU *url)
{
  అక్షరం *host, *పోర్టు;
  నిర్మాణం dynbuf buf;
  CURLUcode uc;
  CURLcode ఫలితం = CURLE_URL_MALFORMAT;

  host = పోర్టు = ఖాళీ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  ఒకవేళ(uc && uc != CURLUE_NO_HOST)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(!host) {
    req->authority = ఖాళీ;
    ఫలితం = CURLE_OK;
    వెళ్ళు బహిర్గతం;
  }

  uc = curl_url_get(url, CURLUPART_పోర్టు, &పోర్టు, CURLU_NO_DEFAULT_పోర్టు);
  ఒకవేళ(uc && uc != CURLUE_NO_పోర్టు)
    వెళ్ళు బహిర్గతం;

  ఫలితం = curlx_dyn_కూడు(&buf, host);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(పోర్టు) {
    ఫలితం = curlx_dyn_addf(&buf, ":%s", పోర్టు);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  req->authority = curlx_dyn_ptr(&buf);
బహిర్గతం:
  curlx_free(host);
  curlx_free(పోర్టు);
  ఒకవేళ(ఫలితం)
    curlx_dyn_free(&buf);
  తిరిగి ఫలితం;
}

స్థిర CURLcode req_కేటాయించు_url_మార్గం(నిర్మాణం httpreq *req, CURLU *url)
{
  అక్షరం *మార్గం, *query;
  నిర్మాణం dynbuf buf;
  CURLUcode uc;
  CURLcode ఫలితం = CURLE_URL_MALFORMAT;

  మార్గం = query = ఖాళీ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_మార్గం, &మార్గం, 0);
  ఒకవేళ(uc)
    వెళ్ళు బహిర్గతం;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  ఒకవేళ(uc && uc != CURLUE_NO_QUERY)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(!query) {
    req->మార్గం = మార్గం;
    మార్గం = ఖాళీ;
  }
  లేకపోతే {
    ఫలితం = curlx_dyn_కూడు(&buf, మార్గం);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addf(&buf, "?%s", query);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    req->మార్గం = curlx_dyn_ptr(&buf);
  }
  ఫలితం = CURLE_OK;

బహిర్గతం:
  curlx_free(మార్గం);
  curlx_free(query);
  ఒకవేళ(ఫలితం)
    curlx_dyn_free(&buf);
  తిరిగి ఫలితం;
}

CURLcode Curl_http_req_make2(నిర్మాణం httpreq **preq,
                             స్థిరరాశి అక్షరం *పద్ధతి, size_t m_len,
                             CURLU *url, స్థిరరాశి అక్షరం *scheme_default)
{
  నిర్మాణం httpreq *req;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  CURLUcode uc;

  DEBUGASSERT(పద్ధతి && m_len);

  req = curlx_calloc(1, పరిమాణం(*req) + m_len);
  ఒకవేళ(!req)
    వెళ్ళు బహిర్గతం;
  జ్ఞాపకం_నకలు(req->పద్ధతి, పద్ధతి, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  ఒకవేళ(uc && uc != CURLUE_NO_SCHEME)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    ఒకవేళ(!req->scheme)
      వెళ్ళు బహిర్గతం;
  }

  ఫలితం = req_కేటాయించు_url_authority(req, url);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఫలితం = req_కేటాయించు_url_మార్గం(req, url);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  Curl_dynhds_init(&req->శీర్షికలు, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  ఫలితం = CURLE_OK;

బహిర్గతం:
  ఒకవేళ(ఫలితం && req)
    Curl_http_req_free(req);
  *preq = ఫలితం ? ఖాళీ : req;
  తిరిగి ఫలితం;
}

శూన్య Curl_http_req_free(నిర్మాణం httpreq *req)
{
  ఒకవేళ(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->మార్గం);
    Curl_dynhds_free(&req->శీర్షికలు);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

నిర్మాణం name_const {
  స్థిరరాశి అక్షరం *name;
  size_t namelen;
};

స్థిర స్థిరరాశి నిర్మాణం name_const H2_NON_క్షేత్రం[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

స్థిర bool h2_permissible_క్షేత్రం(నిర్మాణం dynhds_నమోదు *e)
{
  size_t i;
  కొరకు(i = 0; i < CURL_ARRAYSIZE(H2_NON_క్షేత్రం); ++i) {
    ఒకవేళ(e->namelen == H2_NON_క్షేత్రం[i].namelen &&
       curl_strnequal(H2_NON_క్షేత్రం[i].name, e->name, e->namelen))
      తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

స్థిర bool http_TE_has_token(స్థిరరాశి అక్షరం *fvalue, స్థిరరాశి అక్షరం *token)
{
  అయితే(*fvalue) {
    నిర్మాణం Curl_str name;

    /* skip to first token */
    అయితే(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    ఒకవేళ(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      తిరిగి FALSE;
    ఒకవేళ(curlx_str_casecompare(&name, token))
      తిరిగి TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    అయితే(*fvalue && *fvalue != ',') {
      ఒకవేళ(*fvalue == '"') {
        నిర్మాణం Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        ఒకవేళ(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_శీర్షిక))
          తిరిగి FALSE;
      }
      లేకపోతే
        fvalue++;
    }
  }
  తిరిగి FALSE;
}

CURLcode Curl_http_req_గమ్యం_h2(నిర్మాణం dynhds *h2_శీర్షికలు,
                             నిర్మాణం httpreq *req, నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి అక్షరం *scheme = ఖాళీ, *authority = ఖాళీ;
  నిర్మాణం dynhds_నమోదు *e;
  size_t i;
  CURLcode ఫలితం;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_శీర్షికలు);

  ఒకవేళ(req->scheme) {
    scheme = req->scheme;
  }
  లేకపోతే ఒకవేళ(strcmp("CONNECT", req->పద్ధతి)) {
    scheme = Curl_checkheaders(దత్తాంశం, STRCONST(HTTP_PSEUDO_SCHEME));
    ఒకవేళ(scheme) {
      scheme += పరిమాణం(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(దత్తాంశం, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    లేకపోతే {
      scheme = దత్తాంశం->state.origin->scheme->name;
    }
  }

  ఒకవేళ(req->authority) {
    authority = req->authority;
  }
  లేకపోతే {
    e = Curl_dynhds_get(&req->శీర్షికలు, STRCONST("Host"));
    ఒకవేళ(e)
      authority = e->విలువ;
  }

  Curl_dynhds_reset(h2_శీర్షికలు);
  Curl_dynhds_సమితి_opts(h2_శీర్షికలు, DYNHDS_OPT_LOWERCASE);
  ఫలితం = Curl_dynhds_కూడు(h2_శీర్షికలు, STRCONST(HTTP_PSEUDO_పద్ధతి),
                           req->పద్ధతి, అక్షరశ్రేణి_పొడవు(req->పద్ధతి));
  ఒకవేళ(!ఫలితం && scheme) {
    ఫలితం = Curl_dynhds_కూడు(h2_శీర్షికలు, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, అక్షరశ్రేణి_పొడవు(scheme));
  }
  ఒకవేళ(!ఫలితం && authority) {
    ఫలితం = Curl_dynhds_కూడు(h2_శీర్షికలు, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, అక్షరశ్రేణి_పొడవు(authority));
  }
  ఒకవేళ(!ఫలితం && req->మార్గం) {
    ఫలితం = Curl_dynhds_కూడు(h2_శీర్షికలు, STRCONST(HTTP_PSEUDO_మార్గం),
                             req->మార్గం, అక్షరశ్రేణి_పొడవు(req->మార్గం));
  }
  కొరకు(i = 0; !ఫలితం && i < Curl_dynhds_లెక్క(&req->శీర్షికలు); ++i) {
    e = Curl_dynhds_getn(&req->శీర్షికలు, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    ఒకవేళ(e->namelen == 2 && curl_strequal("TE", e->name)) {
      ఒకవేళ(http_TE_has_token(e->విలువ, "trailers"))
        ఫలితం = Curl_dynhds_కూడు(h2_శీర్షికలు, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    లేకపోతే ఒకవేళ(h2_permissible_క్షేత్రం(e)) {
      ఫలితం = Curl_dynhds_కూడు(h2_శీర్షికలు, e->name, e->namelen,
                               e->విలువ, e->valuelen);
    }
  }

  తిరిగి ఫలితం;
}

CURLcode Curl_http_resp_make(నిర్మాణం http_resp **presp,
                             పూర్ణాంకం స్థితి_2,
                             స్థిరరాశి అక్షరం *description)
{
  నిర్మాణం http_resp *resp;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  resp = curlx_calloc(1, పరిమాణం(*resp));
  ఒకవేళ(!resp)
    వెళ్ళు బహిర్గతం;

  resp->స్థితి_2 = స్థితి_2;
  ఒకవేళ(description) {
    resp->description = curlx_strdup(description);
    ఒకవేళ(!resp->description)
      వెళ్ళు బహిర్గతం;
  }
  Curl_dynhds_init(&resp->శీర్షికలు, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  ఫలితం = CURLE_OK;

బహిర్గతం:
  ఒకవేళ(ఫలితం && resp)
    Curl_http_resp_free(resp);
  *presp = ఫలితం ? ఖాళీ : resp;
  తిరిగి ఫలితం;
}

శూన్య Curl_http_resp_free(నిర్మాణం http_resp *resp)
{
  ఒకవేళ(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->శీర్షికలు);
    Curl_dynhds_free(&resp->trailers);
    ఒకవేళ(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_http = {
  Curl_http_అమరిక_conn,                 /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_పూర్తి,                       /* done */
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

#అయితే_ముగింపు /* CURL_DISABLE_HTTP */
