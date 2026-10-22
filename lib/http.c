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
#অন্তর্ভাব "urldata.h"

#ifndef CURL_DISABLE_HTTP

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif

#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাব <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাব <sys/param.h>
#endif

#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "formdata.h"
#অন্তর্ভাব "mime.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "cookie.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "vquic/vquic.h"
#অন্তর্ভাব "http_digest.h"
#অন্তর্ভাব "http_ntlm.h"
#অন্তর্ভাব "http_negotiate.h"
#অন্তর্ভাব "http_aws_sigv4.h"
#অন্তর্ভাব "http_httpsig.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "urlapi-int.h"
#অন্তর্ভাব "curl_share.h"
#অন্তর্ভাব "dynhds.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "headers.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "parsedate.h" /* for the week day and month names */
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "content_encoding.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "http2.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "altsvc.h"
#অন্তর্ভাব "hsts.h"
#অন্তর্ভাব "rtsp.h"
#অন্তর্ভাব "ws.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "curlx/strparse.h"

শূন্য Curl_http_neg_init(কাঠামো Curl_easy *তথ্য, কাঠামো http_negotiation *neg)
{
  memset(neg, 0, আকার(*neg));
  neg->accept_09 = তথ্য->set.http09_allowed;
  নির্বাচন(তথ্য->set.httpwant) {
  ক্ষেত্র CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    বিরতি;
  ক্ষেত্র CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    বিরতি;
  ক্ষেত্র CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    বিরতি;
  ক্ষেত্র CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    বিরতি;
  ক্ষেত্র CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    তথ্য->state.http_neg.h2_prior_knowledge = TRUE;
    বিরতি;
  ক্ষেত্র CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    বিরতি;
  ক্ষেত্র CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    বিরতি;
  ক্ষেত্র CURL_HTTP_VERSION_NONE:
  পূর্বনির্ধারিত:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    বিরতি;
  }
}

CURLcode Curl_http_setup_conn(কাঠামো Curl_easy *তথ্য,
                              কাঠামো connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  যদি(তথ্য->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode ফলাফল = Curl_conn_may_http3(তথ্য, conn);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  ফেরত CURLE_OK;
}

#ifndef CURL_DISABLE_PROXY
/*
 * checkProxyHeaders() checks the linked list of custom proxy headers
 * if proxy headers are not available, then it will lookup into http header
 * link list
 *
 * It takes a connectdata struct as input to see if this is a proxy request or
 * not, as it then might check a different header list. Provide the header
 * prefix without colon!
 */
অক্ষর *Curl_checkProxyheaders(কাঠামো Curl_easy *তথ্য,
                             ধ্রুবক কাঠামো connectdata *conn,
                             ধ্রুবক অক্ষর *thisheader,
                             ধ্রুবক size_t thislen)
{
  কাঠামো curl_slist *শীর্ষ;

  জন্য(শীর্ষ = (conn->http_proxy.peer && তথ্য->set.sep_headers) ?
        তথ্য->set.proxyheaders : তথ্য->set.headers;
      শীর্ষ; শীর্ষ = শীর্ষ->পরবর্তী) {
    যদি(curl_strnequal(শীর্ষ->তথ্য, thisheader, thislen) &&
       Curl_headersep(শীর্ষ->তথ্য[thislen]))
      ফেরত শীর্ষ->তথ্য;
  }

  ফেরত ফাঁকা;
}
#endif

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
স্থির bool header_has_মান(ধ্রুবক অক্ষর **headerp, কাঠামো Curl_str *outp)
{
  bool মান = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  যদি(মান) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  ফেরত মান;
}

স্থির bool http_header_is_empty(ধ্রুবক অক্ষর *header)
{
  কাঠামো Curl_str out;

  যদি(header_has_মান(&header, &out)) {
    ফেরত curlx_strlen(&out) == 0;
  }
  ফেরত TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
স্থির CURLcode copy_custom_মান(ধ্রুবক অক্ষর *header, অক্ষর **valp)
{
  কাঠামো Curl_str out = { 0 };

  /* find the end of the header name */
  যদি(header_has_মান(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    যদি(*valp)
      ফেরত CURLE_OK;
    ফেরত CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = ফাঁকা;
  ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
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
অক্ষর *Curl_copy_header_মান(ধ্রুবক অক্ষর *header)
{
  কাঠামো Curl_str out;

  /* find the end of the header name */
  যদি(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_আকার, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_আকার);
    curlx_str_trimblanks(&out);
    ফেরত curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  ফেরত ফাঁকা;
}

#ifndef CURL_DISABLE_HTTP_AUTH

#ifndef CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
স্থির CURLcode http_output_basic(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *conn, bool proxy)
{
  size_t আকার_2 = 0;
  অক্ষর *authorization = ফাঁকা;
  অক্ষর **p_hd;
  CURLcode ফলাফল;
  কাঠামো Curl_creds *creds = ফাঁকা;
  অক্ষর *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  যদি(proxy) {
#ifndef CURL_DISABLE_PROXY
    p_hd = &তথ্য->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#অন্যথা
    (শূন্য)conn;
    ফেরত CURLE_NOT_BUILT_IN;
#endif
  }
  অন্যথা {
    p_hd = &তথ্য->req.hd_auth;
    creds = তথ্য->state.creds;
  }

  যদি(!creds) {
    DEBUGASSERT(0);
    ফেরত CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  যদি(!out)
    ফেরত CURLE_OUT_OF_MEMORY;

  ফলাফল = curlx_base64_encode((uint8_t *)out, strlen(out),
                               &authorization, &আকার_2);
  যদি(ফলাফল)
    যাও fail;

  যদি(!authorization) {
    ফলাফল = CURLE_REMOTE_ACCESS_DENIED;
    যাও fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  যদি(!*p_hd) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও fail;
  }

fail:
  curlx_free(out);
  ফেরত ফলাফল;
}

#endif

#ifndef CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
স্থির CURLcode http_output_bearer(কাঠামো Curl_easy *তথ্য)
{
  অক্ষর **userp;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(তথ্য->state.creds));
  userp = &তথ্য->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(তথ্য->state.creds));

  যদি(!*userp) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও fail;
  }

fail:
  ফেরত ফলাফল;
}
#endif

#endif

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
স্থির bool pickoneauth(কাঠামো auth *pick, unsigned long mask,
                        কাঠামো Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  unsigned long avail = pick->avail & pick->want & mask;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  যদি(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ifndef CURL_DISABLE_BEARER_AUTH
  অন্যথা যদি((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  অন্যথা যদি((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#endif
  অন্যথা যদি(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ifndef CURL_DISABLE_BASIC_AUTH
  অন্যথা যদি((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#endif
#ifndef CURL_DISABLE_AWS
  অন্যথা যদি(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  অন্যথা যদি(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#endif
  অন্যথা {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  ফেরত picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
স্থির CURLcode http_perhapsrewind(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো connectdata *conn)
{
  curl_off_t bytessent = তথ্য->req.writebytecount;
  curl_off_t expectsend = Curl_creader_যোগ_দৈর্ঘ্য(তথ্য);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(তথ্য);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!তথ্য->req.upload_done && !little_upload_remains);
  VERBOSE(ধ্রুবক অক্ষর *ongoing_auth = ফাঁকা);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  যদি(needs_rewind) {
    infof(তথ্য, "Need to rewind upload for next request");
    Curl_creader_set_rewind(তথ্য, TRUE);
  }

  যদি(conn->bits.close)
    /* If we already decided to close this connection, we cannot veto. */
    ফেরত CURLE_OK;

  যদি(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ifdef USE_NTLM
    যদি((তথ্য->state.authproxy.picked == CURLAUTH_NTLM) ||
       (তথ্য->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      যদি((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
#ifdef USE_SPNEGO
    /* There is still data left to send */
    যদি((তথ্য->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (তথ্য->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      যদি((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
  }

  যদি(abort_upload) {
    যদি(upload_remain >= 0)
      infof(তথ্য, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    অন্যথা
      infof(তথ্য, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    তথ্য->req.আকার_2 = 0; /* do not download any more than 0 bytes */
    তথ্য->req.http_bodyless = TRUE;
  }
  ফেরত CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
স্থির bool http_should_fail(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা httpcode)
{
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  যদি(!তথ্য->set.http_fail_on_error)
    ফেরত FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  যদি(httpcode < 400)
    ফেরত FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  যদি(তথ্য->state.resume_from && তথ্য->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    ফেরত FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  যদি((httpcode != 401) && (httpcode != 407))
    ফেরত TRUE;

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
  যদি((httpcode == 401) && !তথ্য->state.creds)
    ফেরত TRUE;
#ifndef CURL_DISABLE_PROXY
  যদি((httpcode == 407) && !তথ্য->conn->http_proxy.creds)
    ফেরত TRUE;
#endif

  ফেরত (bool)তথ্য->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode ফলাফল = CURLE_OK;
  unsigned long authmask = ~0UL;

  যদি(!Curl_creds_has_oauth_bearer(তথ্য->state.creds))
    authmask &= (unsigned long)~CURLAUTH_BEARER;

  যদি(100 <= তথ্য->req.httpcode && তথ্য->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    ফেরত CURLE_OK;

  যদি(তথ্য->state.authproblem)
    ফেরত তথ্য->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  যদি(তথ্য->state.creds &&
     ((তথ্য->req.httpcode == 401) ||
      (তথ্য->req.authneg && তথ্য->req.httpcode < 300))) {
    pickhost = pickoneauth(&তথ্য->state.authhost, authmask, তথ্য->state.creds);
    যদি(!pickhost)
      তথ্য->state.authproblem = TRUE;
    অন্যথা
      তথ্য->info.httpauthpicked = তথ্য->state.authhost.picked;
    যদি(তথ্য->state.authhost.picked == CURLAUTH_NTLM &&
       (তথ্য->req.httpversion_sent > 11)) {
      infof(তথ্য, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      তথ্য->state.http_neg.wanted = CURL_HTTP_V1x;
      তথ্য->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ifndef CURL_DISABLE_PROXY
  যদি(conn->http_proxy.creds &&
     ((তথ্য->req.httpcode == 407) ||
      (তথ্য->req.authneg && তথ্য->req.httpcode < 300))) {
    pickproxy = pickoneauth(&তথ্য->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    যদি(!pickproxy)
      তথ্য->state.authproblem = TRUE;
    অন্যথা
      তথ্য->info.proxyauthpicked = তথ্য->state.authproxy.picked;
  }
#endif

  যদি(pickhost || pickproxy) {
    ফলাফল = http_perhapsrewind(তথ্য, conn);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(তথ্য->req.newurl);
    /* clone URL */
    তথ্য->req.newurl = Curl_bufref_dup(&তথ্য->state.url);
    যদি(!তথ্য->req.newurl)
      ফেরত CURLE_OUT_OF_MEMORY;
  }
  অন্যথা যদি((তথ্য->req.httpcode < 300) &&
          !তথ্য->state.authhost.done &&
          তথ্য->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (তথ্য->state.httpreq != HTTPREQ_GET) &&
          (তথ্য->state.httpreq != HTTPREQ_শীর্ষ)) {
    /* clone URL */
    তথ্য->req.newurl = Curl_bufref_dup(&তথ্য->state.url);
    যদি(!তথ্য->req.newurl)
      ফেরত CURLE_OUT_OF_MEMORY;
    তথ্য->state.authhost.done = TRUE;
  }
  যদি(http_should_fail(তথ্য, তথ্য->req.httpcode)) {
    failf(তথ্য, "The requested URL returned error: %d",
          তথ্য->req.httpcode);
    ফলাফল = CURLE_HTTP_RETURNED_ERROR;
  }

  ফেরত ফলাফল;
}

#ifndef CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
স্থির CURLcode output_auth_headers(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো connectdata *conn,
                                    কাঠামো auth *authstatus,
                                    ধ্রুবক অক্ষর *request,
                                    ধ্রুবক অক্ষর *path,
                                    bool proxy)
{
  ধ্রুবক অক্ষর *auth = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)conn;

#ifdef CURL_DISABLE_DIGEST_AUTH
  (শূন্য)request;
  (শূন্য)path;
#endif
#ifndef CURL_DISABLE_AWS
  যদি((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    ফলাফল = Curl_output_aws_sigv4(তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  অন্যথা
#endif
#ifndef CURL_DISABLE_HTTPSIG
  যদি((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    যদি(Curl_auth_allowed_to_host(তথ্য)) {
      auth = "HTTPSIG";
      ফলাফল = Curl_output_httpsig(তথ্য);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    অন্যথা
      authstatus->done = TRUE;
  }
  অন্যথা
#endif
#ifdef USE_SPNEGO
  যদি(authstatus->picked == CURLAUTH_NEGOTIATE) {
    যদি(
#ifndef CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(তথ্য, conn,
                                        STRCONST("Proxy-authorization"))) ||
#endif
      (!proxy && !Curl_checkheaders(তথ্য, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(তথ্য))) {
      auth = "Negotiate";
      ফলাফল = Curl_output_negotiate(তথ্য, conn, proxy);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    অন্যথা
      authstatus->done = TRUE;
  }
  অন্যথা
#endif
#ifdef USE_NTLM
  যদি(authstatus->picked == CURLAUTH_NTLM) {
    যদি(proxy || Curl_auth_allowed_to_host(তথ্য)) {
      auth = "NTLM";
      ফলাফল = Curl_output_ntlm(তথ্য, proxy);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    অন্যথা
      authstatus->done = TRUE;
  }
  অন্যথা
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  যদি(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    ফলাফল = Curl_output_digest(তথ্য,
                                proxy,
                                (ধ্রুবক unsigned অক্ষর *)request,
                                (ধ্রুবক unsigned অক্ষর *)path);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  অন্যথা
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
  যদি(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    যদি(
#ifndef CURL_DISABLE_PROXY
       (proxy && conn->http_proxy.creds &&
        Curl_creds_has_user_or_pass(conn->http_proxy.creds) &&
        !Curl_checkProxyheaders(তথ্য, conn,
                                STRCONST("Proxy-authorization"))) ||
#endif
       (!proxy && তথ্য->state.creds &&
        Curl_creds_has_user_or_pass(তথ্য->state.creds) &&
        !Curl_checkheaders(তথ্য, STRCONST("Authorization")))) {
      auth = "Basic";
      ফলাফল = http_output_basic(তথ্য, conn, proxy);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
  যদি(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    যদি(!proxy && Curl_creds_has_oauth_bearer(তথ্য->state.creds) &&
       !Curl_checkheaders(তথ্য, STRCONST("Authorization"))) {
      auth = "Bearer";
      ফলাফল = http_output_bearer(তথ্য);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif

  যদি(auth) {
#ifndef CURL_DISABLE_PROXY
    যদি(proxy)
      তথ্য->info.proxyauthpicked = authstatus->picked;
    অন্যথা
      তথ্য->info.httpauthpicked = authstatus->picked;
    infof(তথ্য, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (তথ্য->state.creds ?
           তথ্য->state.creds->user : ""));
#অন্যথা
    (শূন্য)proxy;
    infof(তথ্য, "Server auth using %s with user '%s'",
          auth, তথ্য->state.creds ?
          তথ্য->state.creds->user : "");
#endif
    authstatus->multipass = !authstatus->done;
  }
  অন্যথা {
    authstatus->multipass = FALSE;
    যদি(proxy)
      তথ্য->info.proxyauthpicked = 0;
    অন্যথা
      তথ্য->info.httpauthpicked = 0;
  }

  ফেরত ফলাফল;
}

CURLcode Curl_http_output_auth(কাঠামো Curl_easy *তথ্য,
                               কাঠামো connectdata *conn,
                               ধ্রুবক অক্ষর *request,
                               Curl_HttpReq httpreq,
                               ধ্রুবক অক্ষর *path,
                               ধ্রুবক অক্ষর *query,
                               bool is_connect)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো auth *authhost;
  কাঠামো auth *authproxy;
  ধ্রুবক অক্ষর *path_and_query = path;
  অক্ষর *tmp_str = ফাঁকা;

  DEBUGASSERT(তথ্য);
  authhost = &তথ্য->state.authhost;
  authproxy = &তথ্য->state.authproxy;

  যদি(
#ifndef CURL_DISABLE_PROXY
    (!conn->http_proxy.peer || !conn->http_proxy.creds) &&
#endif
#ifdef USE_SPNEGO
    !(authhost->want & CURLAUTH_NEGOTIATE) &&
    !(authproxy->want & CURLAUTH_NEGOTIATE) &&
#endif
#ifndef CURL_DISABLE_HTTPSIG
    !(authhost->want & CURLAUTH_HTTPSIG) &&
#endif
    !তথ্য->state.creds) {
    /* no authentication with no user or password */
    authhost->done = TRUE;
    authproxy->done = TRUE;
    ফলাফল = CURLE_OK;
    যাও out;
  }

  যদি(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    যদি(!tmp_str) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
    path_and_query = tmp_str;
  }

  যদি(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  যদি(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ifndef CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  যদি(conn->bits.origin_is_proxy || is_connect) {
    ফলাফল = output_auth_headers(তথ্য, conn, authproxy, request,
                                 path_and_query, TRUE);
    যদি(ফলাফল)
      যাও out;
  }
  অন্যথা
#অন্যথা
  (শূন্য)is_connect;
#endif /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->done = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  যদি(তথ্য->state.creds || Curl_auth_allowed_to_host(তথ্য))
    ফলাফল = output_auth_headers(তথ্য, conn, authhost, request,
                                 path_and_query, FALSE);
  অন্যথা
    authhost->done = TRUE;

  যদি(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_শীর্ষ)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    তথ্য->req.authneg = TRUE;
  }
  অন্যথা
    তথ্য->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  ফেরত ফলাফল;
}

#অন্যথা /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(কাঠামো Curl_easy *তথ্য,
                               কাঠামো connectdata *conn,
                               ধ্রুবক অক্ষর *request,
                               Curl_HttpReq httpreq,
                               ধ্রুবক অক্ষর *path,
                               ধ্রুবক অক্ষর *query,
                               bool is_connect)
{
  (শূন্য)তথ্য;
  (শূন্য)conn;
  (শূন্য)request;
  (শূন্য)httpreq;
  (শূন্য)path;
  (শূন্য)query;
  (শূন্য)is_connect;
  ফেরত CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP_AUTH, else */

#যদি defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
স্থির bool authcmp(ধ্রুবক অক্ষর *auth, ধ্রুবক অক্ষর *সারি)
{
  /* the auth string must not have an alnum following */
  size_t n = strlen(auth);
  ফেরত curl_strnequal(auth, সারি, n) && !ISALNUM(সারি[n]);
}
#endif

#ifdef USE_SPNEGO
স্থির CURLcode auth_spnego(কাঠামো Curl_easy *তথ্য,
                            bool proxy,
                            ধ্রুবক অক্ষর *auth,
                            কাঠামো auth *authp,
                            uint32_t *availp)
{
  যদি((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    যদি(authp->picked == CURLAUTH_NEGOTIATE) {
      কাঠামো connectdata *conn = তথ্য->conn;
      CURLcode ফলাফল = Curl_প্রবেশ_negotiate(তথ্য, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      যদি(!ফলাফল) {
        curlx_free(তথ্য->req.newurl);
        তথ্য->req.newurl = Curl_bufref_dup(&তথ্য->state.url);
        যদি(!তথ্য->req.newurl)
          ফেরত CURLE_OUT_OF_MEMORY;
        তথ্য->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      অন্যথা
        তথ্য->state.authproblem = TRUE;
    }
  }
  ফেরত CURLE_OK;
}
#endif

#ifdef USE_NTLM
স্থির CURLcode auth_ntlm(কাঠামো Curl_easy *তথ্য,
                          bool proxy,
                          ধ্রুবক অক্ষর *auth,
                          কাঠামো auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  যদি((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    যদি(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode ফলাফল = Curl_প্রবেশ_ntlm(তথ্য, proxy, auth);
      যদি(!ফলাফল)
        তথ্য->state.authproblem = FALSE;
      অন্যথা {
        যদি(ফলাফল == CURLE_OUT_OF_MEMORY)
          ফেরত ফলাফল;
        infof(তথ্য, "NTLM authentication problem, ignoring.");
        তথ্য->state.authproblem = TRUE;
      }
    }
  }
  ফেরত CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_DIGEST_AUTH
স্থির CURLcode auth_digest(কাঠামো Curl_easy *তথ্য,
                            bool proxy,
                            ধ্রুবক অক্ষর *auth,
                            কাঠামো auth *authp,
                            uint32_t *availp)
{
  যদি(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(তথ্য, "Ignoring duplicate digest auth header.");
  }
  অন্যথা যদি(Curl_auth_is_digest_supported()) {
    CURLcode ফলাফল;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    ফলাফল = Curl_প্রবেশ_digest(তথ্য, proxy, auth);
    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_OUT_OF_MEMORY)
        ফেরত ফলাফল;
      infof(তথ্য, "Digest authentication problem, ignoring.");
      তথ্য->state.authproblem = TRUE;
    }
  }
  ফেরত CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BASIC_AUTH
স্থির CURLcode auth_basic(কাঠামো Curl_easy *তথ্য,
                           কাঠামো auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  যদি(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(তথ্য, "Basic authentication problem, ignoring.");
    তথ্য->state.authproblem = TRUE;
  }
  ফেরত CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BEARER_AUTH
স্থির CURLcode auth_bearer(কাঠামো Curl_easy *তথ্য,
                            কাঠামো auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  যদি(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(তথ্য, "Bearer authentication problem, ignoring.");
    তথ্য->state.authproblem = TRUE;
  }
  ফেরত CURLE_OK;
}
#endif

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_প্রবেশ_auth(কাঠামো Curl_easy *তথ্য, bool proxy,
                              ধ্রুবক অক্ষর *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#যদি defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  কাঠামো auth *authp;
  CURLcode ফলাফল = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(তথ্য);

  যদি(proxy) {
    availp = &তথ্য->info.proxyauthavail;
    authp = &তথ্য->state.authproxy;
  }
  অন্যথা {
    availp = &তথ্য->info.httpauthavail;
    authp = &তথ্য->state.authhost;
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

  যখন(*auth) {
#ifdef USE_SPNEGO
    যদি(authcmp("Negotiate", auth))
      ফলাফল = auth_spnego(তথ্য, proxy, auth, authp, availp);
#endif
#ifdef USE_NTLM
    যদি(!ফলাফল && authcmp("NTLM", auth))
      ফলাফল = auth_ntlm(তথ্য, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
    যদি(!ফলাফল && authcmp("Digest", auth))
      ফলাফল = auth_digest(তথ্য, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
    যদি(!ফলাফল && authcmp("Basic", auth))
      ফলাফল = auth_basic(তথ্য, authp, availp);
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
    যদি(authcmp("Bearer", auth))
      ফলাফল = auth_bearer(তথ্য, authp, availp);
#endif

    যদি(ফলাফল)
      বিরতি;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    যদি(auth) /* if we are on a comma, skip it */
      auth++;
    অন্যথা
      বিরতি;
    curlx_str_passblanks(&auth);
  }
  ফেরত ফলাফল;
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)proxy;
  (শূন্য)auth;
  /* nothing to do when disabled */
  ফেরত CURLE_OK;
#endif
}

স্থির শূন্য http_switch_to_get(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা code)
{
  ধ্রুবক অক্ষর *req = CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST);

  যদি((req || তথ্য->state.httpreq != HTTPREQ_GET) &&
     (তথ্য->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((শূন্য)code);
    infof(তথ্য, "Switch to GET because of %d response", code);
    তথ্য->state.http_ignorecustom = TRUE;
  }
  অন্যথা যদি(req && (তথ্য->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(তথ্য, "Stick to %s instead of GET", req);

  তথ্য->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(তথ্য, FALSE);
}

#define HTTPREQ_IS_POST(তথ্য)                    \
  ((তথ্য)->state.httpreq == HTTPREQ_POST ||      \
   (তথ্য)->state.httpreq == HTTPREQ_POST_FORM || \
   (তথ্য)->state.httpreq == HTTPREQ_POST_MIME)

স্থির CURLcode set_auto_referer(কাঠামো Curl_easy *তথ্য)
{
  CURLU *u;
  অক্ষর *referer = ফাঁকা;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&তথ্য->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  যদি(!u)
    ফেরত CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&তথ্য->state.url), 0);
  যদি(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, ফাঁকা, 0);
  যদি(!uc)
    uc = curl_url_set(u, CURLUPART_USER, ফাঁকা, 0);
  যদি(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, ফাঁকা, 0);
  যদি(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  যদি(uc || !referer)
    ফেরত CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&তথ্য->state.referer, referer, 0, curl_free);
  ফেরত CURLE_OK;
}

স্থির CURLcode follow_parse_url(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *newurl,
                                 followtype type, অক্ষর **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(তথ্য->state.uh);
  uc = curl_url_set(তথ্য->state.uh, CURLUPART_URL, newurl, (unsigned পূর্ণসংখ্যা)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (তথ্য->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  যদি(uc) {
    যদি((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(তথ্য, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      ফেরত Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    যদি(!*follow_urlp)
      ফেরত CURLE_OUT_OF_MEMORY;
  }
  অন্যথা {
    CURLU *u = curl_url();
    যদি(!u)
      ফেরত CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&তথ্য->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    যদি(!uc)
      uc = curl_url_get(তথ্য->state.uh, CURLUPART_URL, follow_urlp, 0);
    যদি(uc) {
      curl_url_cleanup(u);
      ফেরত Curl_uc_to_curlcode(uc);
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, তথ্য->state.uh);
      curl_url_cleanup(u);
      যদি(!same_origin)
        Curl_auth_digest_cleanup(&তথ্য->state.digest);
    }
#অন্যথা
    curl_url_cleanup(u);
#endif
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode follow_fake(কাঠামো Curl_easy *তথ্য, অক্ষর *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  যদি(!uc) {
    CURLU *u = curl_url();
    অক্ষর *nocred = ফাঁকা;

    যদি(!u) {
      curlx_free(follow_url);
      ফেরত CURLE_OUT_OF_MEMORY;
    }
    যদি(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (তথ্য->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, ফাঁকা, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, ফাঁকা, 0))
      (শূন্য)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    যদি(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  তথ্য->info.wouldredirect = follow_url;

  যদি(reachedmax) {
    failf(তথ্য, "Maximum (%d) redirects followed", তথ্য->set.maxredirs);
    ফেরত CURLE_TOO_MANY_REDIRECTS;
  }
  ফেরত CURLE_OK;
}

স্থির bool follow_switch_get(কাঠামো Curl_easy *তথ্য)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  নির্বাচন(তথ্য->info.httpcode) {
  ক্ষেত্র 301: /* Moved Permanently */
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
    যদি(HTTPREQ_IS_POST(তথ্য) && !তথ্য->set.post301) {
      http_switch_to_get(তথ্য, 301);
      ফেরত TRUE;
    }
    বিরতি;
  ক্ষেত্র 302: /* Found */
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
    যদি(HTTPREQ_IS_POST(তথ্য) && !তথ্য->set.post302) {
      http_switch_to_get(তথ্য, 302);
      ফেরত TRUE;
    }
    বিরতি;
  ক্ষেত্র 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    যদি(!HTTPREQ_IS_POST(তথ্য) || !তথ্য->set.post303) {
      http_switch_to_get(তথ্য, 303);
      ফেরত TRUE;
    }
    বিরতি;

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
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত FALSE;
}

CURLcode Curl_http_follow(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  অক্ষর *follow_url = ফাঁকা;
  CURLUcode uc;
  CURLcode ফলাফল;
  CURLcode rewind_ফলাফল;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  যদি(type != FOLLOW_FAKE)
    তথ্য->state.requests++; /* count all real follows */
  যদি(type == FOLLOW_REDIR) {
    যদি((তথ্য->set.maxredirs != -1) &&
       (তথ্য->state.followlocation >= তথ্য->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    অন্যথা {
      তথ্য->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      যদি(তথ্য->set.http_auto_referer) {
        ফলাফল = set_auto_referer(তথ্য);
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }
    }
  }

  যদি((type != FOLLOW_RETRY) &&
     (তথ্য->req.httpcode != 401) && (তথ্য->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, ফাঁকা, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  ফলাফল = follow_parse_url(তথ্য, newurl, type, &follow_url, &uc);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  DEBUGASSERT(follow_url);

  যদি(type == FOLLOW_FAKE)
    ফেরত follow_fake(তথ্য, follow_url, uc, reachedmax);

  যদি(disallowport)
    তথ্য->state.allow_port = FALSE;

  Curl_bufref_set(&তথ্য->state.url, follow_url, 0, curl_free);
  rewind_ফলাফল = Curl_req_soft_reset(&তথ্য->req, তথ্য);
  infof(তথ্য, "Issue another request to this URL: '%s'", follow_url);
  যদি((তথ্য->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !তথ্য->state.http_ignorecustom &&
     CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST)) {
    তথ্য->state.http_ignorecustom = TRUE;
    infof(তথ্য, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(তথ্য);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  যদি(rewind_ফলাফল && !switch_to_get)
    ফেরত rewind_ফলাফল;

  Curl_pgrsTime(তথ্য, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(তথ্য);

  ফেরত CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(ধ্রুবক অক্ষর *headerline, /* line to check */
                        ধ্রুবক অক্ষর *header, /* header keyword _with_ colon */
                        ধ্রুবক size_t hlen, /* len of the keyword in bytes */
                        ধ্রুবক অক্ষর *content, /* content string to find */
                        ধ্রুবক size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  ধ্রুবক অক্ষর *p;
  কাঠামো Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  যদি(!curl_strnequal(headerline, header, hlen))
    ফেরত FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  যদি(curlx_str_cspn(&p, &val, "\r\n"))
    ফেরত FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  যদি(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    জন্য(len = curlx_strlen(&val); len >= clen;) {
      কাঠামো Curl_str পরবর্তী;
      ধ্রুবক অক্ষর *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      যদি(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        ফেরত TRUE; /* match! */
      /* advance to the next comma */
      যদি(curlx_str_until(&p, &পরবর্তী, len, ',') ||
         curlx_str_single(&p, ','))
        বিরতি; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      করো
        curlx_str_passblanks(&p);
      যখন(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      যদি((size_t)(p - o) > len)
        বিরতি;
      len -= (p - o);
    }
  }
  ফেরত FALSE; /* no match */
}

কাঠামো cr_exp100_ctx {
  কাঠামো Curl_creader super;
  কাঠামো curltime start; /* time started waiting */
  গণনা expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

স্থির শূন্য http_exp100_continue(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো Curl_creader *reader)
{
  কাঠামো cr_exp100_ctx *ctx = reader->ctx;
  যদি(ctx->state > EXP100_SEND_তথ্য) {
    ctx->state = EXP100_SEND_তথ্য;
    Curl_expire_clear(তথ্য, EXPIRE_100_TIMEOUT);
  }
}

স্থির CURLcode cr_exp100_read(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_creader *reader,
                               অক্ষর *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  কাঠামো cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  নির্বাচন(ctx->state) {
  ক্ষেত্র EXP100_SENDING_REQUEST:
    যদি(!Curl_req_sendbuf_empty(তথ্য)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(তথ্য, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      ফেরত CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(তথ্য, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", তথ্য->set.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->start = *Curl_pgrs_now(তথ্য);
    Curl_expire_set(তথ্য, EXPIRE_100_TIMEOUT,
                    তথ্য->set.expect_100_timeout, &ctx->start);
    *nread = 0;
    *eos = FALSE;
    ফেরত CURLE_OK;
  ক্ষেত্র EXP100_FAILED:
    DEBUGF(infof(তথ্য, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    ফেরত CURLE_READ_ERROR;
  ক্ষেত্র EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &ctx->start);
    যদি(ms < তথ্য->set.expect_100_timeout) {
      DEBUGF(infof(তথ্য, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      ফেরত CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(তথ্য, reader);
    infof(তথ্য, "Done waiting for 100-continue");
    FALLTHROUGH();
  পূর্বনির্ধারিত:
    DEBUGF(infof(তথ্য, "cr_exp100_read, pass through"));
    ফেরত Curl_creader_read(তথ্য, reader->পরবর্তী, buf, blen, nread, eos);
  }
}

স্থির শূন্য cr_exp100_done(কাঠামো Curl_easy *তথ্য,
                           কাঠামো Curl_creader *reader, পূর্ণসংখ্যা premature)
{
  কাঠামো cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_তথ্য;
  Curl_expire_clear(তথ্য, EXPIRE_100_TIMEOUT);
}

স্থির ধ্রুবক কাঠামো Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_যোগ_দৈর্ঘ্য,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_done,
  আকার(কাঠামো cr_exp100_ctx)
};

স্থির CURLcode http_exp100_add_reader(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *reader = ফাঁকা;
  CURLcode ফলাফল;

  ফলাফল = Curl_creader_create(&reader, তথ্য, &cr_exp100, CURL_CR_PROTOCOL);
  যদি(!ফলাফল)
    ফলাফল = Curl_creader_add(তথ্য, reader);
  যদি(!ফলাফল) {
    কাঠামো cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  যদি(ফলাফল && reader)
    Curl_creader_free(তথ্য, reader);
  ফেরত ফলাফল;
}

স্থির শূন্য http_exp100_got100(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *r = Curl_creader_get_by_type(তথ্য, &cr_exp100);
  যদি(r)
    http_exp100_continue(তথ্য, r);
}

স্থির bool http_exp100_is_waiting(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *r = Curl_creader_get_by_type(তথ্য, &cr_exp100);
  যদি(r) {
    কাঠামো cr_exp100_ctx *ctx = r->ctx;
    ফেরত ctx->state == EXP100_AWAITING_CONTINUE;
  }
  ফেরত FALSE;
}

স্থির শূন্য http_exp100_send_anyway(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *r = Curl_creader_get_by_type(তথ্য, &cr_exp100);
  যদি(r)
    http_exp100_continue(তথ্য, r);
}

স্থির bool http_exp100_is_selected(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *r = Curl_creader_get_by_type(তথ্য, &cr_exp100);
  ফেরত !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো easy_pollset *ps)
{
  /* write mode */
  ফেরত Curl_pollset_add_out(তথ্য, ps, তথ্য->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো easy_pollset *ps)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল = CURLE_OK;

  যদি(CURL_REQ_WANT_RECV(তথ্য)) {
    ফলাফল = Curl_pollset_add_in(তথ্য, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  যদি(!ফলাফল && Curl_req_want_send(তথ্য) && !http_exp100_is_waiting(তথ্য)) {
    ফলাফল = Curl_pollset_add_out(তথ্য, ps, conn->sock[FIRSTSOCKET]);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode http_write_header(কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক অক্ষর *hd, size_t hdlen)
{
  CURLcode ফলাফল;
  পূর্ণসংখ্যা writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(তথ্য, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((তথ্য->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  ফলাফল = Curl_client_write(তথ্য, writetype, hd, hdlen);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = Curl_bump_headersize(তথ্য, hdlen, FALSE);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  তথ্য->req.deductheadercount = (100 <= তথ্য->req.httpcode &&
                                 199 >= তথ্য->req.httpcode) ?
    তথ্য->req.headerbytecount : 0;
  ফেরত ফলাফল;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(কাঠামো Curl_easy *তথ্য,
                        CURLcode status, bool premature)
{
  কাঠামো connectdata *conn = তথ্য->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  তথ্য->state.authhost.multipass = FALSE;
  তথ্য->state.authproxy.multipass = FALSE;

  যদি(curlx_dyn_len(&তথ্য->state.headerb)) {
    (শূন্য)http_write_header(তথ্য, curlx_dyn_ptr(&তথ্য->state.headerb),
                            curlx_dyn_len(&তথ্য->state.headerb));
  }
  curlx_dyn_reset(&তথ্য->state.headerb);

  যদি(status)
    ফেরত status;

  যদি(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->bits.retry &&
     !তথ্য->set.connect_only &&
     (তথ্য->req.bytecount +
      তথ্য->req.headerbytecount -
      তথ্য->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(তথ্য, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    ফেরত CURLE_GOT_NOTHING;
  }

  ফেরত CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
স্থির bool http_may_use_1_1(ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক কাঠামো connectdata *conn = তথ্য->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  যদি(তথ্য->state.http_neg.rcvd_min == 10)
    ফেরত FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  যদি(conn && conn->httpversion_seen == 10)
    ফেরত FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  যদি(তথ্য->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    ফেরত FALSE;
  /* We are not restricted to use 1.0 only. */
  ফেরত !তথ্য->state.http_neg.only_10;
}

স্থির unsigned অক্ষর http_request_version(কাঠামো Curl_easy *তথ্য)
{
  unsigned অক্ষর v = Curl_conn_http_version(তথ্য, তথ্য->conn);
  যদি(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(তথ্য) ? 11 : 10;
  }
  ফেরত v;
}

স্থির ধ্রুবক অক্ষর *get_http_string(পূর্ণসংখ্যা httpversion)
{
  নির্বাচন(httpversion) {
  ক্ষেত্র 30:
    ফেরত "3";
  ক্ষেত্র 20:
    ফেরত "2";
  ক্ষেত্র 11:
    ফেরত "1.1";
  পূর্বনির্ধারিত:
    ফেরত "1.0";
  }
}

CURLcode Curl_add_custom_headers(কাঠামো Curl_easy *তথ্য,
                                 bool is_connect, পূর্ণসংখ্যা httpversion,
                                 কাঠামো dynbuf *req)
{
  কাঠামো curl_slist *h[2];
  কাঠামো curl_slist *headers;
  পূর্ণসংখ্যা numlists = 1; /* by default */
  পূর্ণসংখ্যা i;

#ifndef CURL_DISABLE_PROXY
  গণনা Curl_proxy_use proxy;

  যদি(is_connect)
    proxy = HEADER_CONNECT;
  অন্যথা
    proxy = তথ্য->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  নির্বাচন(proxy) {
  ক্ষেত্র HEADER_SERVER:
    h[0] = তথ্য->set.headers;
    বিরতি;
  ক্ষেত্র HEADER_PROXY:
    h[0] = তথ্য->set.headers;
    যদি(তথ্য->set.sep_headers) {
      h[1] = তথ্য->set.proxyheaders;
      numlists++;
    }
    বিরতি;
  ক্ষেত্র HEADER_CONNECT:
    যদি(তথ্য->set.sep_headers)
      h[0] = তথ্য->set.proxyheaders;
    অন্যথা
      h[0] = তথ্য->set.headers;
    বিরতি;
  ক্ষেত্র HEADER_CONNECT_UDP:
    যদি(তথ্য->set.sep_headers)
      h[0] = তথ্য->set.proxyheaders;
    অন্যথা
      h[0] = তথ্য->set.headers;
    বিরতি;
  }
#অন্যথা
  (শূন্য)is_connect;
  h[0] = তথ্য->set.headers;
#endif

  /* loop through one or two lists */
  জন্য(i = 0; i < numlists; i++) {
    জন্য(headers = h[i]; headers; headers = headers->পরবর্তী) {
      CURLcode ফলাফল = CURLE_OK;
      bool blankheader = FALSE;
      কাঠামো Curl_str name;
      ধ্রুবক অক্ষর *p = headers->তথ্য;
      ধ্রুবক অক্ষর *origp = p;
      size_t hlen = strlen(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      যদি(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      অন্যথা {
        p = origp;
        যদি(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          কাঠামো Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          যদি(!curlx_strlen(&val))
            /* no content, do not send this */
            চালু;
        }
        অন্যথা
          /* no colon */
          চালু;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      যদি(তথ্য->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      অন্যথা যদি(তথ্য->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      অন্যথা যদি(তথ্য->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      অন্যথা যদি(তথ্য->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      অন্যথা যদি(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      অন্যথা যদি((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      অন্যথা যদি((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(তথ্য))
        ;
      অন্যথা যদি(blankheader) {
        ফলাফল = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        যদি(!ফলাফল)
          ফলাফল = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      অন্যথা
        ফলাফল = curlx_dyn_addf(req, "%s\r\n", origp);

      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
  }

  ফেরত CURLE_OK;
}

#ifndef CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(কাঠামো Curl_easy *তথ্য,
                                কাঠামো dynbuf *req)
{
  ধ্রুবক কাঠামো tm *tm;
  কাঠামো tm keeptime;
  CURLcode ফলাফল;
  অক্ষর datestr[80];
  ধ্রুবক অক্ষর *condp;
  size_t len;

  যদি(তথ্য->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    ফেরত CURLE_OK;

  ফলাফল = curlx_gmtime(তথ্য->set.timevalue, &keeptime);
  যদি(ফলাফল) {
    failf(তথ্য, "Invalid TIMEVALUE");
    ফেরত ফলাফল;
  }
  tm = &keeptime;

  নির্বাচন(তথ্য->set.timecondition) {
  পূর্বনির্ধারিত:
    DEBUGF(infof(তথ্য, "invalid time condition"));
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  ক্ষেত্র CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    বিরতি;
  ক্ষেত্র CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    বিরতি;
  ক্ষেত্র CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    বিরতি;
  }

  যদি(Curl_checkheaders(তথ্য, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    ফেরত CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, আকার(datestr),
                 "%s: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                 condp,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);

  ফলাফল = curlx_dyn_add(req, datestr);
  ফেরত ফলাফল;
}
#অন্যথা
/* disabled */
CURLcode Curl_add_timecondition(কাঠামো Curl_easy *তথ্য,
                                কাঠামো dynbuf *req)
{
  (শূন্য)তথ্য;
  (শূন্য)req;
  ফেরত CURLE_OK;
}
#endif

শূন্য Curl_http_method(কাঠামো Curl_easy *তথ্য,
                      ধ্রুবক অক্ষর **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)তথ্য->state.httpreq;
  ধ্রুবক অক্ষর *request;
#ifndef CURL_DISABLE_WEBSOCKETS
  যদি(তথ্য->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  অন্যথা
#endif
  যদি((তথ্য->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     তথ্য->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  যদি(!তথ্য->state.http_ignorecustom &&
     CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST);
  }
  অন্যথা {
    যদি(তথ্য->req.no_body)
      request = "HEAD";
    অন্যথা {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_শীর্ষ));
      নির্বাচন(httpreq) {
      ক্ষেত্র HTTPREQ_POST:
      ক্ষেত্র HTTPREQ_POST_FORM:
      ক্ষেত্র HTTPREQ_POST_MIME:
        request = "POST";
        বিরতি;
      ক্ষেত্র HTTPREQ_PUT:
        request = "PUT";
        বিরতি;
      পূর্বনির্ধারিত: /* this should never happen */
      ক্ষেত্র HTTPREQ_GET:
        request = "GET";
        বিরতি;
      ক্ষেত্র HTTPREQ_শীর্ষ:
        request = "HEAD";
        বিরতি;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

স্থির CURLcode http_set_aptr_host(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  ধ্রুবক অক্ষর *ptr = ফাঁকা;

  curlx_safefree(তথ্য->state.http_host);
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(তথ্য->req.cookiehost);
#endif

  যদি(Curl_peer_equal(তথ্য->state.initial_origin, তথ্য->state.origin))
    ptr = Curl_checkheaders(তথ্য, STRCONST("Host"));

  যদি(ptr) {
#ifndef CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    অক্ষর *cookiehost;
    CURLcode ফলাফল = copy_custom_মান(ptr, &cookiehost);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    যদি(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    অন্যথা {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      যদি(*cookiehost == '[') {
        অক্ষর *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, strlen(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        যদি(closingbracket)
          *closingbracket = 0;
      }
      অন্যথা {
        পূর্ণসংখ্যা startsearch = 0;
        অক্ষর *colon = strchr(cookiehost + startsearch, ':');
        যদি(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      তথ্য->req.cookiehost = cookiehost;
    }
#endif

    যদি(!curl_strequal("Host:", ptr)) {
      তথ্য->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      যদি(!তথ্য->state.http_host)
        ফেরত CURLE_OUT_OF_MEMORY;
    }
  }
  অন্যথা {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)তথ্য->state.origin->ipv6;
    কাঠামো dynbuf tmp;
    size_t hlen;
    CURLcode ফলাফল;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    ফলাফল = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    যদি(!ফলাফল && ipv6)
      ফলাফল = curlx_dyn_addn(&tmp, STRCONST("["));
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_add(&tmp, তথ্য->state.origin->hostname);
    যদি(!ফলাফল && ipv6)
      ফলাফল = curlx_dyn_addn(&tmp, STRCONST("]"));
    যদি(!ফলাফল &&
       ((তথ্য->state.origin->port != তথ্য->state.origin->scheme->defport) ||
       (তথ্য->state.origin->scheme->family != conn->scheme->family))) {
      ফলাফল = curlx_dyn_addf(&tmp, ":%u", তথ্য->state.origin->port);
    }

    তথ্য->state.http_host = ফলাফল ? ফাঁকা : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    ফেরত ফলাফল;
  }
  ফেরত CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
স্থির CURLcode http_target(কাঠামো Curl_easy *তথ্য,
                            কাঠামো dynbuf *r)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *path = তথ্য->state.up.path;
  ধ্রুবক অক্ষর *query = তথ্য->state.up.query;
#ifndef CURL_DISABLE_PROXY
  কাঠামো connectdata *conn = তথ্য->conn;
#endif

  যদি(CURL_EASY_STR(তথ্য, STRING_TARGET)) {
    path = CURL_EASY_STR(তথ্য, STRING_TARGET);
    query = ফাঁকা;
  }

#ifndef CURL_DISABLE_PROXY
  যদি(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    অক্ষর *url;
    CURLU *h = curl_url_dup(তথ্য->state.uh);
    যদি(!h)
      ফেরত CURLE_OUT_OF_MEMORY;

    যদি(!তথ্য->state.origin->ipv6 &&
       (তথ্য->state.origin->user_hostname != তথ্য->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, তথ্য->state.origin->hostname, 0);
      যদি(uc) {
        curl_url_cleanup(h);
        ফেরত CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, ফাঁকা, 0);
    যদি(uc) {
      curl_url_cleanup(h);
      ফেরত CURLE_OUT_OF_MEMORY;
    }

    যদি(তথ্য->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, ফাঁকা, 0);
      যদি(uc) {
        curl_url_cleanup(h);
        ফেরত CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, ফাঁকা, 0);
      যদি(uc) {
        curl_url_cleanup(h);
        ফেরত CURLE_OUT_OF_MEMORY;
      }
    }
    অন্যথা যদি(তথ্য->state.creds && (তথ্য->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        তথ্য->state.creds->user, CURLU_URLENCODE);
      যদি(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          তথ্য->state.creds->passwd, CURLU_URLENCODE);
      যদি(uc) {
        curl_url_cleanup(h);
        ফেরত Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    যদি(uc) {
      curl_url_cleanup(h);
      ফেরত CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    ফলাফল = curlx_dyn_add(r, CURL_EASY_STR(তথ্য, STRING_TARGET) ?
      CURL_EASY_STR(তথ্য, STRING_TARGET) : url);
    curlx_free(url);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    যদি((তথ্য->state.origin->scheme == &Curl_scheme_ftp) &&
       তথ্য->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = strlen(path);
      bool type_present = FALSE;
      যদি((len >= 7) && !memcmp(&path[len - 7], ";type=", 6)) {
        নির্বাচন(Curl_raw_toupper(path[len - 1])) {
        ক্ষেত্র 'A':
        ক্ষেত্র 'D':
        ক্ষেত্র 'I':
          type_present = TRUE;
          বিরতি;
        }
      }
      যদি(!type_present) {
        ফলাফল = curlx_dyn_addf(r, ";type=%c",
                                তথ্য->state.prefer_ascii ? 'a' : 'i');
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }
    }
  }

  অন্যথা
#endif
  {
    ফলাফল = curlx_dyn_add(r, path);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    যদি(query)
      ফলাফল = curlx_dyn_addf(r, "?%s", query);
  }

  ফেরত ফলাফল;
}

#যদি !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
স্থির CURLcode set_post_reader(কাঠামো Curl_easy *তথ্য, Curl_HttpReq httpreq)
{
  CURLcode ফলাফল;

  নির্বাচন(httpreq) {
#ifndef CURL_DISABLE_MIME
  ক্ষেত্র HTTPREQ_POST_MIME:
    তথ্য->state.mimepost = তথ্য->set.mimepostp;
    বিরতি;
#endif
#ifndef CURL_DISABLE_FORM_API
  ক্ষেত্র HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    যদি(!তথ্য->state.formp) {
      তথ্য->state.formp = curlx_calloc(1, আকার(curl_mimepart));
      যদি(!তথ্য->state.formp)
        ফেরত CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(তথ্য->state.formp);
      ফলাফল = Curl_getformdata(তথ্য, তথ্য->state.formp, তথ্য->set.httppost,
                                তথ্য->state.fread_func);
      যদি(ফলাফল) {
        curlx_safefree(তথ্য->state.formp);
        ফেরত ফলাফল;
      }
      তথ্য->state.mimepost = তথ্য->state.formp;
    }
    বিরতি;
#endif
  পূর্বনির্ধারিত:
    তথ্য->state.mimepost = ফাঁকা;
    বিরতি;
  }

  নির্বাচন(httpreq) {
  ক্ষেত্র HTTPREQ_POST_FORM:
  ক্ষেত্র HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ifndef CURL_DISABLE_MIME
    যদি(তথ্য->state.mimepost) {
      ধ্রুবক অক্ষর *cthdr = Curl_checkheaders(তথ্য, STRCONST("Content-Type"));

      /* Read and seek body only. */
      তথ্য->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      যদি(cthdr)
        জন্য(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      অন্যথা যদি(তথ্য->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(তথ্য->state.mimepost, তথ্য->set.headers, 0);
      ফলাফল = Curl_mime_prepare_headers(তথ্য, তথ্য->state.mimepost, cthdr,
                                         ফাঁকা, MIMESTRATEGY_FORM);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      curl_mime_headers(তথ্য->state.mimepost, ফাঁকা, 0);
      ফলাফল = Curl_creader_set_mime(তথ্য, তথ্য->state.mimepost);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    অন্যথা
#endif
    {
      ফলাফল = Curl_creader_set_null(তথ্য);
    }
    তথ্য->state.infilesize = Curl_creader_যোগ_দৈর্ঘ্য(তথ্য);
    ফেরত ফলাফল;

  পূর্বনির্ধারিত:
    ফেরত Curl_creader_set_null(তথ্য);
  }
  /* never reached */
}
#endif

স্থির CURLcode set_reader(কাঠামো Curl_easy *তথ্য, Curl_HttpReq httpreq)
{
  CURLcode ফলাফল = CURLE_OK;
  curl_off_t postsize = তথ্য->state.infilesize;

  DEBUGASSERT(তথ্য->conn);

  যদি(তথ্য->req.authneg) {
    ফেরত Curl_creader_set_null(তথ্য);
  }

  নির্বাচন(httpreq) {
  ক্ষেত্র HTTPREQ_PUT: /* Let's PUT the data to the server! */
    ফেরত postsize ? Curl_creader_set_fread(তথ্য, postsize) :
      Curl_creader_set_null(তথ্য);

#যদি !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ক্ষেত্র HTTPREQ_POST_FORM:
  ক্ষেত্র HTTPREQ_POST_MIME:
    ফেরত set_post_reader(তথ্য, httpreq);
#endif

  ক্ষেত্র HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    যদি(!postsize) {
      ফলাফল = Curl_creader_set_null(তথ্য);
    }
    অন্যথা যদি(তথ্য->set.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      যদি(plen == SIZE_MAX)
        ফেরত CURLE_OUT_OF_MEMORY;
      অন্যথা যদি(plen)
        ফলাফল = Curl_creader_set_buf(তথ্য, তথ্য->set.postfields, plen);
      অন্যথা
        ফলাফল = Curl_creader_set_null(তথ্য);
    }
    অন্যথা {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      অক্ষর *ptr = Curl_checkheaders(তথ্য, STRCONST("Transfer-Encoding"));
      যদি(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      ফলাফল = Curl_creader_set_fread(তথ্য, chunked ? -1 : postsize);
    }
    ফেরত ফলাফল;

  পূর্বনির্ধারিত:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    তথ্য->state.infilesize = 0;
    ফেরত Curl_creader_set_null(তথ্য);
  }
  /* not reached */
}

স্থির CURLcode http_resume(কাঠামো Curl_easy *তথ্য, Curl_HttpReq httpreq)
{
  যদি((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     তথ্য->state.resume_from) {
    failf(তথ্য, "HTTP upload cannot be resumed");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode http_req_set_TE(কাঠামো Curl_easy *তথ্য,
                                কাঠামো dynbuf *req,
                                পূর্ণসংখ্যা httpversion)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *ptr;

  ptr = Curl_checkheaders(তথ্য, STRCONST("Transfer-Encoding"));
  যদি(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    তথ্য->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    যদি(তথ্য->req.upload_chunky && (httpversion >= 20)) {
      infof(তথ্য, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      তথ্য->req.upload_chunky = FALSE;
    }
  }
  অন্যথা {
    curl_off_t req_clen = Curl_creader_যোগ_দৈর্ঘ্য(তথ্য);

    যদি(req_clen < 0) {
      /* indeterminate request content length */
      যদি(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        তথ্য->req.upload_chunky = (httpversion < 20);
      }
      অন্যথা {
        failf(তথ্য, "Chunky upload is not supported by HTTP 1.0");
        ফেরত CURLE_UPLOAD_FAILED;
      }
    }
    অন্যথা {
      /* else, no chunky upload */
      তথ্য->req.upload_chunky = FALSE;
    }

    যদি(তথ্য->req.upload_chunky)
      ফলাফল = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  ফেরত ফলাফল;
}

স্থির CURLcode addexpect(কাঠামো Curl_easy *তথ্য, কাঠামো dynbuf *r,
                          পূর্ণসংখ্যা httpversion, bool *announced_exp100)
{
  CURLcode ফলাফল;
  অক্ষর *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  যদি(তথ্য->req.upgr101 != UPGR101_NONE)
    ফেরত CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(তথ্য, STRCONST("Expect"));
  যদি(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  অন্যথা যদি(!তথ্য->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_দৈর্ঘ্য(তথ্য);
    যদি(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      ফলাফল = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      যদি(ফলাফল)
        ফেরত ফলাফল;
      *announced_exp100 = TRUE;
    }
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode http_add_content_hds(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো dynbuf *r,
                                     পূর্ণসংখ্যা httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode ফলাফল = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(তথ্য->conn);
  যদি(তথ্য->req.upload_chunky) {
    ফলাফল = Curl_httpchunk_add_reader(তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_যোগ_দৈর্ঘ্য(তথ্য);
  নির্বাচন(httpreq) {
  ক্ষেত্র HTTPREQ_PUT:
  ক্ষেত্র HTTPREQ_POST:
#যদি !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ক্ষেত্র HTTPREQ_POST_FORM:
  ক্ষেত্র HTTPREQ_POST_MIME:
#endif
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    যদি(req_clen >= 0 && !তথ্য->req.upload_chunky &&
       (তথ্য->req.authneg ||
        !Curl_checkheaders(তথ্য, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      ফলাফল = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    যদি(ফলাফল)
      যাও out;

#ifndef CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    যদি(তথ্য->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      কাঠামো curl_slist *hdr;

      জন্য(hdr = তথ্য->state.mimepost->curlheaders; hdr; hdr = hdr->পরবর্তী) {
        ফলাফল = curlx_dyn_addf(r, "%s\r\n", hdr->তথ্য);
        যদি(ফলাফল)
          যাও out;
      }
    }
#endif
    যদি(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(তথ্য, STRCONST("Content-Type"))) {
      ফলাফল = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      যদি(ফলাফল)
        যাও out;
    }
    ফলাফল = addexpect(তথ্য, r, httpversion, &announced_exp100);
    যদি(ফলাফল)
      যাও out;
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }

  Curl_pgrs_Set_Upload_আকার(তথ্য, req_clen);
  যদি(announced_exp100)
    ফলাফল = http_exp100_add_reader(তথ্য);

out:
  ফেরত ফলাফল;
}

#ifndef CURL_DISABLE_COOKIES

স্থির CURLcode http_cookies(কাঠামো Curl_easy *তথ্য,
                             কাঠামো dynbuf *r)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *addcookies = ফাঁকা;
  bool linecap = FALSE;
  যদি(CURL_EASY_STR(তথ্য, STRING_COOKIE) &&
     !Curl_checkheaders(তথ্য, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(তথ্য))
    addcookies = CURL_EASY_STR(তথ্য, STRING_COOKIE);

  যদি(তথ্য->cookies || addcookies) {
    কাঠামো Curl_llist list;
    পূর্ণসংখ্যা গুনতি = 0;

    যদি(তথ্য->cookies && তথ্য->state.cookie_engine) {
      bool okay;
      ধ্রুবক অক্ষর *host = তথ্য->req.cookiehost ?
        তথ্য->req.cookiehost : তথ্য->state.origin->hostname;
      Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      ফলাফল = Curl_cookie_getlist(তথ্য, &okay, host, &list);
      যদি(!ফলাফল && okay) {
        কাঠামো Curl_llist_গ্রন্থি *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        জন্য(n = Curl_llist_শীর্ষ(&list); n; n = Curl_গ্রন্থি_পরবর্তী(n)) {
          কাঠামো Cookie *co = Curl_গ্রন্থি_elem(n);
          যদি(co->মান) {
            size_t add;
            যদি(!গুনতি) {
              ফলাফল = curlx_dyn_addn(r, STRCONST("Cookie: "));
              যদি(ফলাফল)
                বিরতি;
            }
            add = strlen(co->name) + strlen(co->মান) + 1;
            যদি(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(তথ্য, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              বিরতি;
            }
            ফলাফল = curlx_dyn_addf(r, "%s%s=%s", গুনতি ? "; " : "",
                                    co->name, co->মান);
            যদি(ফলাফল)
              বিরতি;
            clen += add + (গুনতি ? 2 : 0);
            গুনতি++;
          }
        }
        Curl_llist_destroy(&list, ফাঁকা);
      }
      Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_COOKIE);
    }
    যদি(addcookies && !ফলাফল && !linecap) {
      যদি(!গুনতি)
        ফলাফল = curlx_dyn_addn(r, STRCONST("Cookie: "));
      যদি(!ফলাফল) {
        ফলাফল = curlx_dyn_addf(r, "%s%s", গুনতি ? "; " : "", addcookies);
        গুনতি++;
      }
    }
    যদি(গুনতি && !ফলাফল)
      ফলাফল = curlx_dyn_addn(r, STRCONST("\r\n"));

    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  ফেরত ফলাফল;
}
#অন্যথা
#define http_cookies(a, b) CURLE_OK
#endif

স্থির CURLcode http_range(কাঠামো Curl_easy *তথ্য,
                           Curl_HttpReq httpreq)
{
  যদি(তথ্য->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    যদি(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_শীর্ষ)) &&
       !Curl_checkheaders(তথ্য, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(তথ্য->state.rangeline);
      তথ্য->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 তথ্য->state.range);
      যদি(!তথ্য->state.rangeline)
        ফেরত CURLE_OUT_OF_MEMORY;
    }
    অন্যথা যদি((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(তথ্য, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_যোগ_দৈর্ঘ্য(তথ্য);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(তথ্য->state.rangeline);

      যদি(তথ্য->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        তথ্য->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      অন্যথা যদি(তথ্য->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t যোগ_len = তথ্য->req.authneg ?
                               তথ্য->state.infilesize :
                               (তথ্য->state.resume_from + req_clen);
        তথ্য->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        তথ্য->state.range, যোগ_len - 1, যোগ_len);
      }
      অন্যথা {
        /* Range was selected and then we pass the incoming range and append
           total size */
        তথ্য->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        তথ্য->state.range, req_clen);
      }
      যদি(!তথ্য->state.rangeline)
        ফেরত CURLE_OUT_OF_MEMORY;
    }
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode http_firstwrite(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো SingleRequest *k = &তথ্য->req;

  যদি(তথ্য->req.newurl) {
    যদি(conn->bits.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(তথ্য);
      k->done = TRUE;
      ফেরত CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(তথ্য, "Ignoring the response-body");
  }
  যদি(তথ্য->state.resume_from && !k->content_range &&
     (তথ্য->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    যদি(k->আকার_2 == তথ্য->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(তথ্য, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(তথ্য);
      k->done = TRUE;
      ফেরত CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(তথ্য, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    ফেরত CURLE_RANGE_ERROR;
  }

  যদি(তথ্য->set.timecondition && !তথ্য->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(তথ্য, k->timeofdoc)) {
    k->done = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    তথ্য->info.httpcode = 304;
    infof(তথ্য, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    ফেরত CURLE_OK;
  } /* we have a time condition */

  ফেরত CURLE_OK;
}

স্থির CURLcode http_check_new_conn(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  ধ্রুবক অক্ষর *info_version = ফাঁকা;
  ধ্রুবক অক্ষর *alpn;
  CURLcode ফলাফল;

  alpn = Curl_conn_get_alpn_negotiated(তথ্য, conn);
  যদি(alpn && !strcmp("h3", alpn)) {
#ifndef CURL_DISABLE_PROXY
    যদি(!conn->bits.origin_is_proxy)
#endif
      DEBUGASSERT(Curl_conn_http_version(তথ্য, conn) == 30);
    info_version = "HTTP/3";
  }
  অন্যথা যদি(alpn && !strcmp("h2", alpn)) {
#ifndef CURL_DISABLE_PROXY
    যদি((Curl_conn_http_version(তথ্য, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      ফলাফল = Curl_http2_switch(তথ্য);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    অন্যথা
#endif
    DEBUGASSERT(Curl_conn_http_version(তথ্য, conn) == 20);
    info_version = "HTTP/2";
  }
  অন্যথা {
    /* Check if user wants to use HTTP/2 with clear TCP */
    যদি(Curl_http2_may_switch(তথ্য)) {
      DEBUGF(infof(তথ্য, "HTTP/2 over clean TCP"));
      ফলাফল = Curl_http2_switch(তথ্য);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    অন্যথা
      info_version = "HTTP/1.x";
  }

  যদি(info_version)
    infof(তথ্য, "using %s", info_version);
  ফেরত CURLE_OK;
}

স্থির CURLcode http_add_connection_hd(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো dynbuf *req)
{
  কাঠামো curl_slist *শীর্ষ;
  ধ্রুবক অক্ষর *sep = "Connection: ";
  CURLcode ফলাফল = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  জন্য(শীর্ষ = তথ্য->set.headers; শীর্ষ; শীর্ষ = শীর্ষ->পরবর্তী) {
    যদি(curl_strnequal(শীর্ষ->তথ্য, "Connection", 10) &&
       Curl_headersep(শীর্ষ->তথ্য[10]) &&
       !http_header_is_empty(শীর্ষ->তথ্য)) {
      অক্ষর *মান;
      ফলাফল = copy_custom_মান(শীর্ষ->তথ্য, &মান);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      ফলাফল = curlx_dyn_addf(req, "%s%s", sep, মান);
      sep = ", ";
      curlx_free(মান);
      বিরতি; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  যদি(!ফলাফল && তথ্য->state.http_hd_te) {
    ফলাফল = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  যদি(!ফলাফল && তথ্য->state.http_hd_upgrade) {
    ফলাফল = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  যদি(!ফলাফল && তথ্য->state.http_hd_h2_settings) {
    ফলাফল = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  যদি(!ফলাফল && (rlen < curlx_dyn_len(req)))
    ফলাফল = curlx_dyn_addn(req, STRCONST("\r\n"));
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  জন্য(শীর্ষ = তথ্য->set.headers; শীর্ষ; শীর্ষ = শীর্ষ->পরবর্তী) {
    যদি(curl_strnequal(শীর্ষ->তথ্য, "Connection", 10) &&
       Curl_headersep(শীর্ষ->তথ্য[10]) &&
       !http_header_is_empty(শীর্ষ->তথ্য)) {
      যদি(skip) {
        skip = FALSE;
        চালু;
      }
      ফলাফল = curlx_dyn_addf(req, "%s\r\n", শীর্ষ->তথ্য);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
  }

  ফেরত CURLE_OK;
}

/* Header identifier in order we send them by default */
প্রকারলেখ গণনা {
  H1_HD_REQUEST,
  H1_HD_HOST,
#ifndef CURL_DISABLE_PROXY
  H1_HD_PROXY_AUTH,
#endif
  H1_HD_AUTH,
  H1_HD_RANGE,
  H1_HD_USER_AGENT,
  H1_HD_ACCEPT,
  H1_HD_TE,
  H1_HD_ACCEPT_ENCODING,
  H1_HD_REFERER,
#ifndef CURL_DISABLE_PROXY
  H1_HD_PROXY_CONNECTION,
#endif
  H1_HD_TRANSFER_ENCODING,
#ifndef CURL_DISABLE_ALTSVC
  H1_HD_ALT_USED,
#endif
  H1_HD_UPGRADE,
  H1_HD_COOKIES,
  H1_HD_CONDITIONALS,
  H1_HD_CUSTOM,
  H1_HD_CONTENT,
  H1_HD_CONNECTION,
  H1_HD_LAST  /* the last, empty header line */
} http_hd_t;

স্থির CURLcode http_add_hd(কাঠামো Curl_easy *তথ্য,
                            কাঠামো dynbuf *req,
                            http_hd_t id,
                            unsigned অক্ষর httpversion,
                            ধ্রুবক অক্ষর *method,
                            Curl_HttpReq httpreq)
{
  CURLcode ফলাফল = CURLE_OK;
#যদি !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  কাঠামো connectdata *conn = তথ্য->conn;
#endif
  নির্বাচন(id) {
  ক্ষেত্র H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    ফলাফল = curlx_dyn_addf(req, "%s ", method);
    যদি(!ফলাফল)
      ফলাফল = http_target(তথ্য, req);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    বিরতি;

  ক্ষেত্র H1_HD_HOST:
    যদি(তথ্য->state.http_host) {
      ফলাফল = curlx_dyn_add(req, তথ্য->state.http_host);
      যদি(!ফলাফল)
        ফলাফল = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    বিরতি;

#ifndef CURL_DISABLE_PROXY
  ক্ষেত্র H1_HD_PROXY_AUTH:
    যদি(তথ্য->req.hd_proxy_auth)
      ফলাফল = curlx_dyn_add(req, তথ্য->req.hd_proxy_auth);
    বিরতি;
#endif

  ক্ষেত্র H1_HD_AUTH:
    যদি(তথ্য->req.hd_auth)
      ফলাফল = curlx_dyn_add(req, তথ্য->req.hd_auth);
    বিরতি;

  ক্ষেত্র H1_HD_RANGE:
    যদি(তথ্য->state.use_range && তথ্য->state.rangeline)
      ফলাফল = curlx_dyn_add(req, তথ্য->state.rangeline);
    বিরতি;

  ক্ষেত্র H1_HD_USER_AGENT: {
    ধ্রুবক অক্ষর *ua = CURL_EASY_STR(তথ্য, STRING_USERAGENT);
    যদি(ua && *ua && !Curl_checkheaders(তথ্য, STRCONST("User-Agent")))
      ফলাফল = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    বিরতি;
  }

  ক্ষেত্র H1_HD_ACCEPT:
    যদি(!Curl_checkheaders(তথ্য, STRCONST("Accept")))
      ফলাফল = curlx_dyn_add(req, "Accept: */*\r\n");
    বিরতি;

  ক্ষেত্র H1_HD_TE:
#ifdef HAVE_LIBZ
    যদি(!Curl_checkheaders(তথ্য, STRCONST("TE")) &&
       তথ্য->set.http_transfer_encoding) {
      তথ্য->state.http_hd_te = TRUE;
      ফলাফল = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#endif
    বিরতি;

  ক্ষেত্র H1_HD_ACCEPT_ENCODING: {
    ধ্রুবক অক্ষর *enc = CURL_EASY_STR(তথ্য, STRING_ENCODING);
    যদি(enc && !Curl_checkheaders(তথ্য, STRCONST("Accept-Encoding")))
      ফলাফল = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    বিরতি;
  }

  ক্ষেত্র H1_HD_REFERER:
    যদি(Curl_bufref_ptr(&তথ্য->state.referer) &&
       !Curl_checkheaders(তথ্য, STRCONST("Referer")))
      ফলাফল = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&তথ্য->state.referer));
    বিরতি;

#ifndef CURL_DISABLE_PROXY
  ক্ষেত্র H1_HD_PROXY_CONNECTION:
    যদি(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(তথ্য, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(তথ্য, তথ্য->conn, STRCONST("Proxy-Connection")))
      ফলাফল = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    বিরতি;
#endif

  ক্ষেত্র H1_HD_TRANSFER_ENCODING:
    ফলাফল = http_req_set_TE(তথ্য, req, httpversion);
    বিরতি;

#ifndef CURL_DISABLE_ALTSVC
  ক্ষেত্র H1_HD_ALT_USED:
    যদি(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(তথ্য, STRCONST("Alt-Used")))
      ফলাফল = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    বিরতি;
#endif

  ক্ষেত্র H1_HD_UPGRADE:
    যদি(!Curl_conn_is_ssl(তথ্য->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (তথ্য->state.http_neg.wanted & CURL_HTTP_V2x) &&
       তথ্য->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      ফলাফল = Curl_http2_request_upgrade(req, তথ্য);
    }
#ifndef CURL_DISABLE_WEBSOCKETS
    যদি(!ফলাফল && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      ফলাফল = Curl_ws_request(তথ্য, req);
#endif
    বিরতি;

  ক্ষেত্র H1_HD_COOKIES:
    ফলাফল = http_cookies(তথ্য, req);
    বিরতি;

  ক্ষেত্র H1_HD_CONDITIONALS:
    ফলাফল = Curl_add_timecondition(তথ্য, req);
    বিরতি;

  ক্ষেত্র H1_HD_CUSTOM:
    ফলাফল = Curl_add_custom_headers(তথ্য, FALSE, httpversion, req);
    বিরতি;

  ক্ষেত্র H1_HD_CONTENT:
    ফলাফল = http_add_content_hds(তথ্য, req, httpversion, httpreq);
    বিরতি;

  ক্ষেত্র H1_HD_CONNECTION: {
    ফলাফল = http_add_connection_hd(তথ্য, req);
    বিরতি;
  }

  ক্ষেত্র H1_HD_LAST:
    ফলাফল = curlx_dyn_addn(req, STRCONST("\r\n"));
    বিরতি;
  }
  ফেরত ফলাফল;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  Curl_HttpReq httpreq;
  ধ্রুবক অক্ষর *method;
  কাঠামো dynbuf req;
  unsigned অক্ষর httpversion;
  size_t hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *done = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&তথ্য->state.headerb);
  তথ্য->state.maybe_folded = FALSE;

  যদি(!তথ্য->conn->bits.reuse) {
    ফলাফল = http_check_new_conn(তথ্য);
    যদি(ফলাফল)
      যাও out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  ফলাফল = Curl_headers_init(তথ্য);
  যদি(ফলাফল)
    যাও out;

  তথ্য->state.http_hd_te = FALSE;
  তথ্য->state.http_hd_upgrade = FALSE;
  তথ্য->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(তথ্য, &method, &httpreq);

  /* select host to send */
  ফলাফল = http_set_aptr_host(তথ্য);
  /* setup the authentication headers, how that method and host are known */
  যদি(!ফলাফল)
    ফলাফল = Curl_http_output_auth(তথ্য, তথ্য->conn, method, httpreq,
                                   তথ্য->state.up.path,
                                   তথ্য->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  যদি(!ফলাফল)
    ফলাফল = set_reader(তথ্য, httpreq);
  যদি(!ফলাফল)
    ফলাফল = http_resume(তথ্য, httpreq);
  যদি(!ফলাফল)
    ফলাফল = http_range(তথ্য, httpreq);
  যদি(ফলাফল)
    যাও out;

  httpversion = http_request_version(তথ্য);
  /* Add request line and all headers to `req` */
  জন্য(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    ফলাফল = http_add_hd(তথ্য, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    যদি(ফলাফল)
      যাও out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(তথ্য, FIRSTSOCKET, -1);
  ফলাফল = Curl_req_send(তথ্য, &req, httpversion);

  যদি((httpversion >= 20) && তথ্য->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    তথ্য->req.upload_chunky = FALSE;

out:
  যদি(ফলাফল == CURLE_TOO_LARGE)
    failf(তথ্য, "HTTP request too large");

  curlx_dyn_free(&req);
  ফেরত ফলাফল;
}

প্রকারলেখ গণনা {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
স্থির bool checkprefixmax(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *বাফার, size_t len)
{
  size_t ch = CURLMIN(strlen(prefix), len);
  ফেরত curl_strnequal(prefix, বাফার, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
স্থির statusline checkhttpprefix(কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক অক্ষর *s, size_t len)
{
  কাঠামো curl_slist *শীর্ষ = তথ্য->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  যখন(শীর্ষ) {
    যদি(checkprefixmax(শীর্ষ->তথ্য, s, len)) {
      rc = onmatch;
      বিরতি;
    }
    শীর্ষ = শীর্ষ->পরবর্তী;
  }

  যদি((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  ফেরত rc;
}

#ifndef CURL_DISABLE_RTSP
স্থির statusline checkrtspprefix(কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক অক্ষর *s, size_t len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (শূন্য)তথ্য;
  যদি(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  ফেরত status;
}
#endif /* CURL_DISABLE_RTSP */

স্থির statusline checkprotoprefix(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো connectdata *conn,
                                   ধ্রুবক অক্ষর *s, size_t len)
{
#ifndef CURL_DISABLE_RTSP
  যদি(conn->scheme->protocol & CURLPROTO_RTSP)
    ফেরত checkrtspprefix(তথ্য, s, len);
#অন্যথা
  (শূন্য)conn;
#endif /* CURL_DISABLE_RTSP */

  ফেরত checkhttpprefix(তথ্য, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#define HD_IS(hd, hdlen, n) \
  (((hdlen) >= (আকার(n) - 1)) && curl_strnequal(n, hd, আকার(n) - 1))

#define HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (আকার(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, আকার(n) - 1)) ? ((hd) + (আকার(n) - 1)) : ফাঁকা)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#define HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((আকার(n) - 1) + (আকার(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
স্থির CURLcode http_header_a(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *hd, size_t hdlen)
{
#ifndef CURL_DISABLE_ALTSVC
  ধ্রুবক অক্ষর *v;
  v = (তথ্য->asi &&
       (Curl_xfer_is_secure(তথ্য) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#অন্যথা
        0
#endif
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : ফাঁকা;
  যদি(v) {
    /* the ALPN of the current request */
    কাঠামো SingleRequest *k = &তথ্য->req;
    গণনা alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    ফেরত Curl_altsvc_parse(তথ্য, তথ্য->asi, v, তথ্য->state.origin, id);
  }
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)hd;
  (শূন্য)hdlen;
#endif
  ফেরত CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
স্থির CURLcode http_header_c(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *hd, size_t hdlen)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো SingleRequest *k = &তথ্য->req;
  ধ্রুবক অক্ষর *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !তথ্য->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : ফাঁকা;
  যদি(v) {
    করো {
      curl_off_t contentlength;
      পূর্ণসংখ্যা offt = curlx_str_numblanks(&v, &contentlength);

      যদি(offt == STRE_OVERFLOW) {
        /* out of range */
        যদি(তথ্য->set.max_filesize) {
          failf(তথ্য, "Maximum file size exceeded");
          ফেরত CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(তথ্য, "Overflow Content-Length: value");
        ফেরত CURLE_OK;
      }
      অন্যথা {
        যদি((offt == STRE_OK) &&
           ((k->আকার_2 == -1) || /* not set to something before */
            (k->আকার_2 == contentlength))) { /* or the same value */

          k->আকার_2 = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          যদি(!curlx_str_single(&v, ','))
            চালু;

          যদি(!curlx_str_newline(&v)) {
            k->maxdownload = k->আকার_2;
            ফেরত CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(তথ্য, "Invalid Content-Length: value");
        ফেরত CURLE_WEIRD_SERVER_REPLY;
      }
    } যখন(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(তথ্য, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : ফাঁকা;
  যদি(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    ফেরত Curl_build_unencoding_স্তূপ(তথ্য, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  যদি(v) {
    অক্ষর *contenttype = Curl_copy_header_মান(hd);
    যদি(!contenttype)
      ফেরত CURLE_OUT_OF_MEMORY;
    যদি(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    অন্যথা {
      curlx_free(তথ্য->info.contenttype);
      তথ্য->info.contenttype = contenttype;
    }
    ফেরত CURLE_OK;
  }
  যদি((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    ফেরত CURLE_OK;
  }
  যদি((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(তথ্য, "HTTP/1.0 connection set to keep alive");
    ফেরত CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : ফাঁকা;
  যদি(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    ধ্রুবক অক্ষর *ptr = v;

    /* Move forward until first digit or asterisk */
    যখন(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    যদি(ISDIGIT(*ptr)) {
      যদি(!curlx_str_সংখ্যা(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (তথ্য->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    অন্যথা যদি(k->httpcode < 300)
      তথ্য->state.resume_from = 0; /* get everything */
  }
  ফেরত CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
স্থির CURLcode http_header_l(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *hd, size_t hdlen)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো SingleRequest *k = &তথ্য->req;
  ধ্রুবক অক্ষর *v = (!k->http_bodyless &&
                   (তথ্য->set.timecondition || তথ্য->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : ফাঁকা;
  যদি(v) {
    যদি(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    যদি(তথ্য->set.get_filetime)
      তথ্য->info.filetime = k->timeofdoc;
    ফেরত CURLE_OK;
  }
  যদি(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    অক্ষর *location = Curl_copy_header_মান(hd);
    যদি(!location)
      ফেরত CURLE_OUT_OF_MEMORY;
    যদি(!*location ||
       (তথ্য->req.location && !strcmp(তথ্য->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      ফেরত CURLE_OK;
    }
    অন্যথা {
      /* has value and is not an exact repeat */
      যদি(তথ্য->req.location) {
        failf(তথ্য, "Multiple Location headers");
        curlx_free(location);
        ফেরত CURLE_WEIRD_SERVER_REPLY;
      }
      তথ্য->req.location = location;

      যদি((k->httpcode >= 300 && k->httpcode < 400) &&
         তথ্য->set.http_follow_mode) {
        CURLcode ফলাফল;
        DEBUGASSERT(!তথ্য->req.newurl);
        তথ্য->req.newurl = curlx_strdup(তথ্য->req.location); /* clone */
        যদি(!তথ্য->req.newurl)
          ফেরত CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        ফলাফল = http_perhapsrewind(তথ্য, conn);
        যদি(ফলাফল)
          ফেরত ফলাফল;

        /* mark the next request as a followed location: */
        তথ্য->state.this_is_a_follow = TRUE;
      }
    }
  }
  ফেরত CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
স্থির CURLcode http_header_p(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *hd, size_t hdlen)
{
  কাঠামো SingleRequest *k = &তথ্য->req;

#ifndef CURL_DISABLE_PROXY
  ধ্রুবক অক্ষর *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  যদি(v) {
    কাঠামো connectdata *conn = তথ্য->conn;
    যদি((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(তথ্য, "HTTP/1.0 proxy connection set to keep alive");
    }
    অন্যথা যদি((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(তথ্য, "HTTP/1.1 proxy connection set close");
    }
    ফেরত CURLE_OK;
  }
#endif
  যদি((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    অক্ষর *auth;
    CURLcode ফলাফল;
#ifndef CURL_DISABLE_PROXY
    যদি(!তথ্য->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      ফেরত CURLE_OK;
#endif
    auth = Curl_copy_header_মান(hd);
    ফলাফল = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    যদি(!ফলাফল) {
      ফলাফল = Curl_http_প্রবেশ_auth(তথ্য, TRUE, auth);
      curlx_free(auth);
    }
    ফেরত ফলাফল;
  }
#ifdef USE_SPNEGO
  যদি(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    কাঠামো connectdata *conn = তথ্য->conn;
    কাঠামো negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    কাঠামো auth *authp = &তথ্য->state.authhost;
    যদি(!negdata)
      ফেরত CURLE_OUT_OF_MEMORY;
    যদি(authp->picked == CURLAUTH_NEGOTIATE) {
      অক্ষর *persistentauth = Curl_copy_header_মান(hd);
      যদি(!persistentauth)
        ফেরত CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(তথ্য, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#endif
  ফেরত CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
স্থির CURLcode http_header_r(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *hd, size_t hdlen)
{
  ধ্রুবক অক্ষর *v = HD_VAL(hd, hdlen, "Retry-After:");
  যদি(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    যদি(!Curl_getdate_capped(v, &date)) {
      time_t current = time(ফাঁকা);
      যদি(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    অন্যথা
      /* Try it as a decimal number, ignore errors */
      (শূন্য)curlx_str_সংখ্যা(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    যদি(retry_after > 21600)
      retry_after = 21600;
    তথ্য->info.retry_after = retry_after;
  }
  ফেরত CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
স্থির CURLcode http_header_s(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *hd, size_t hdlen)
{
#যদি !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ধ্রুবক অক্ষর *v;
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)hd;
  (শূন্য)hdlen;
#endif

#ifndef CURL_DISABLE_COOKIES
  v = (তথ্য->cookies && তথ্য->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : ফাঁকা;
  যদি(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ধ্রুবক অক্ষর *host = তথ্য->req.cookiehost ?
      তথ্য->req.cookiehost : তথ্য->state.origin->hostname;
    ধ্রুবক unsigned অক্ষর secure_context = Curl_secure_context(তথ্য, host) ?
      COOKIE_SECURE : 0;
    CURLcode ফলাফল;
    Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    ফলাফল = Curl_cookie_add(তথ্য, তথ্য->cookies, v, host,
                             তথ্য->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_COOKIE);
    ফেরত ফলাফল;
  }
#endif
#ifndef CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (তথ্য->hsts &&
       (Curl_xfer_is_secure(তথ্য) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#অন্যথা
        0
#endif
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : ফাঁকা;
  যদি(v) {
    CURLcode ফলাফল = Curl_hsts_parse(
      তথ্য->hsts, তথ্য->state.origin->hostname, v);
    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_OUT_OF_MEMORY)
        ফেরত ফলাফল;
      infof(তথ্য, "Illegal STS header skipped");
    }
#ifdef DEBUGBUILD
    অন্যথা
      infof(তথ্য, "Parsed STS header fine (%zu entries)",
            Curl_llist_গুনতি(&তথ্য->hsts->list));
#endif
  }
#endif

  ফেরত CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
স্থির CURLcode http_header_t(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *hd, size_t hdlen)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো SingleRequest *k = &তথ্য->req;

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
  ধ্রুবক অক্ষর *v = (!k->http_bodyless &&
                   (তথ্য->state.httpreq != HTTPREQ_শীর্ষ) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : ফাঁকা;
  যদি(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode ফলাফল = Curl_build_unencoding_স্তূপ(তথ্য, v, TRUE);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    যদি(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(তথ্য, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    ফেরত CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  যদি(v) {
    তথ্য->req.resp_trailer = TRUE;
    ফেরত CURLE_OK;
  }
  ফেরত CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
স্থির CURLcode http_header_w(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *hd, size_t hdlen)
{
  কাঠামো SingleRequest *k = &তথ্য->req;
  CURLcode ফলাফল = CURLE_OK;

  যদি((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    অক্ষর *auth = Curl_copy_header_মান(hd);
    যদি(!auth)
      ফলাফল = CURLE_OUT_OF_MEMORY;
    অন্যথা {
      ফলাফল = Curl_http_প্রবেশ_auth(তথ্য, FALSE, auth);
      curlx_free(auth);
    }
  }
  ফেরত ফলাফল;
}

/*
 * http_header() parses a single response header.
 */
স্থির CURLcode http_header(কাঠামো Curl_easy *তথ্য,
                            ধ্রুবক অক্ষর *hd, size_t hdlen)
{
  CURLcode ফলাফল = CURLE_OK;

  নির্বাচন(hd[0]) {
  ক্ষেত্র 'a':
  ক্ষেত্র 'A':
    ফলাফল = http_header_a(তথ্য, hd, hdlen);
    বিরতি;
  ক্ষেত্র 'c':
  ক্ষেত্র 'C':
    ফলাফল = http_header_c(তথ্য, hd, hdlen);
    বিরতি;
  ক্ষেত্র 'l':
  ক্ষেত্র 'L':
    ফলাফল = http_header_l(তথ্য, hd, hdlen);
    বিরতি;
  ক্ষেত্র 'p':
  ক্ষেত্র 'P':
    ফলাফল = http_header_p(তথ্য, hd, hdlen);
    বিরতি;
  ক্ষেত্র 'r':
  ক্ষেত্র 'R':
    ফলাফল = http_header_r(তথ্য, hd, hdlen);
    বিরতি;
  ক্ষেত্র 's':
  ক্ষেত্র 'S':
    ফলাফল = http_header_s(তথ্য, hd, hdlen);
    বিরতি;
  ক্ষেত্র 't':
  ক্ষেত্র 'T':
    ফলাফল = http_header_t(তথ্য, hd, hdlen);
    বিরতি;
  ক্ষেত্র 'w':
  ক্ষেত্র 'W':
    ফলাফল = http_header_w(তথ্য, hd, hdlen);
    বিরতি;
  }

  যদি(!ফলাফল) {
    কাঠামো connectdata *conn = তথ্য->conn;
    যদি(conn->scheme->protocol & CURLPROTO_RTSP)
      ফলাফল = Curl_rtsp_parseheader(তথ্য, hd);
  }
  ফেরত ফলাফল;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
স্থির CURLcode http_statusline(কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata *conn)
{
  কাঠামো SingleRequest *k = &তথ্য->req;

  নির্বাচন(k->httpversion) {
  ক্ষেত্র 10:
  ক্ষেত্র 11:
#ifdef USE_HTTP2
  ক্ষেত্র 20:
#endif
#ifdef USE_HTTP3
  ক্ষেত্র 30:
#endif
    /* no major version switch mid-connection */
    যদি(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(তথ্য, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      ফেরত CURLE_WEIRD_SERVER_REPLY;
    }
    বিরতি;
  পূর্বনির্ধারিত:
    failf(তথ্য, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    ফেরত CURLE_UNSUPPORTED_PROTOCOL;
  }

  তথ্য->info.httpcode = k->httpcode;
  তথ্য->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  যদি(!তথ্য->state.http_neg.rcvd_min ||
     তথ্য->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    তথ্য->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  যদি(তথ্য->state.resume_from && তথ্য->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  যদি(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(তথ্য, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  নির্বাচন(k->httpcode) {
  ক্ষেত্র 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    যদি(তথ্য->set.timecondition)
      তথ্য->info.timecond = TRUE;
    FALLTHROUGH();
  ক্ষেত্র 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->আকার_2 = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
স্থির CURLcode http_আকার(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো SingleRequest *k = &তথ্য->req;
  যদি(তথ্য->req.ignore_cl || k->chunk) {
    k->আকার_2 = k->maxdownload = -1;
  }
  অন্যথা যদি(k->আকার_2 != -1) {
    যদি(তথ্য->set.max_filesize &&
       !k->ignorebody &&
       (k->আকার_2 > তথ্য->set.max_filesize)) {
      failf(তথ্য, "Maximum file size exceeded");
      ফেরত CURLE_FILESIZE_EXCEEDED;
    }
    যদি(k->ignorebody)
      infof(তথ্য, "setting size while ignoring");
    Curl_pgrs_Set_Download_আকার(তথ্য, k->আকার_2);
    k->maxdownload = k->আকার_2;
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_verify_header(কাঠামো Curl_easy *তথ্য,
                            ধ্রুবক অক্ষর *hd, size_t hdlen)
{
  কাঠামো SingleRequest *k = &তথ্য->req;
  size_t i;
  bool has_colon = FALSE;
  জন্য(i = 0; i < hdlen; i++) {
    নির্বাচন(hd[i]) {
    ক্ষেত্র '\0':
      /* this is bad, bail out */
      failf(তথ্য, "NUL byte in header");
      ফেরত CURLE_WEIRD_SERVER_REPLY;
    ক্ষেত্র '\r':
      যদি(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(তথ্য, "Carriage return found in header");
        ফেরত CURLE_WEIRD_SERVER_REPLY;
      }
      বিরতি;
    ক্ষেত্র ':':
      has_colon = TRUE;
      বিরতি;
    }
  }
  যদি(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    ফেরত CURLE_OK;
  যদি(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  অন্যথা যদি(!has_colon) {
    /* this is bad, bail out */
    failf(তথ্য, "Header without colon");
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_bump_headersize(কাঠামো Curl_easy *তথ্য,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  unsigned পূর্ণসংখ্যা max = MAX_HTTP_RESP_HEADER_আকার;
  যদি(delta < MAX_HTTP_RESP_HEADER_আকার) {
    তথ্য->info.header_আকার += (unsigned পূর্ণসংখ্যা)delta;
    তথ্য->req.allheadercount += (unsigned পূর্ণসংখ্যা)delta;
    যদি(!connect_only)
      তথ্য->req.headerbytecount += (unsigned পূর্ণসংখ্যা)delta;
    যদি(তথ্য->req.allheadercount > max)
      bad = তথ্য->req.allheadercount;
    অন্যথা যদি(তথ্য->info.header_আকার > (max * 20)) {
      bad = তথ্য->info.header_আকার;
      max *= 20;
    }
  }
  অন্যথা
    bad = তথ্য->req.allheadercount + delta;
  যদি(bad) {
    failf(তথ্য, "Too large response headers: %zu > %u", bad, max);
    ফেরত CURLE_RECV_ERROR;
  }
  ফেরত CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
স্থির CURLcode http_on_101_upgrade(কাঠামো Curl_easy *তথ্য,
                                    ধ্রুবক অক্ষর *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_changed)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো SingleRequest *k = &তথ্য->req;

#যদি !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (শূন্য)buf;
  (শূন্য)blen;
  (শূন্য)pconsumed;
#অন্যথা
  CURLcode ফলাফল;
  পূর্ণসংখ্যা upgr101_requested = k->upgr101;
#endif

  যদি(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(তথ্য, "server sent 101 response while not talking HTTP/1.1");
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->bits.upgrade_in_progress = FALSE;
  *conn_changed = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ifdef USE_NGHTTP2
  যদি(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(তথ্য, "Received 101, Switching to HTTP/2");
    ফলাফল = Curl_http2_upgrade(তথ্য, conn, FIRSTSOCKET, buf, blen);
    যদি(!ফলাফল)
      *pconsumed += blen;
    ফেরত ফলাফল;
  }
#endif
#ifndef CURL_DISABLE_WEBSOCKETS
  যদি(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(তথ্য, "Received 101, Switching to WebSocket");
    ফলাফল = Curl_ws_accept(তথ্য, buf, blen);
    যদি(!ফলাফল)
      *pconsumed += blen; /* ws accept handled the data */
    ফেরত ফলাফল;
  }
#endif
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  ফেরত CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
স্থির CURLcode http_on_1xx_response(কাঠামো Curl_easy *তথ্য,
                                     ধ্রুবক অক্ষর *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_changed)
{
  কাঠামো SingleRequest *k = &তথ্য->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  নির্বাচন(k->httpcode) {
  ক্ষেত্র 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(তথ্য);
    বিরতি;
  ক্ষেত্র 101:
    ফেরত http_on_101_upgrade(তথ্য, buf, blen, pconsumed, conn_changed);
  পূর্বনির্ধারিত:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    বিরতি;
  }
  ফেরত CURLE_OK;
}

#যদি defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
স্থির শূন্য http_check_auth_closure(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ifdef USE_NTLM
  যদি(conn->bits.close &&
     (((তথ্য->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((তথ্য->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(তথ্য, "Connection closure while negotiating auth (HTTP 1.0?)");
    তথ্য->state.authproblem = TRUE;
  }
#endif
#ifdef USE_SPNEGO
  যদি(conn->bits.close &&
    (((তথ্য->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((তথ্য->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(তথ্য, "Connection closure while negotiating auth (HTTP 1.0?)");
    তথ্য->state.authproblem = TRUE;
  }
  যদি((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (তথ্য->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  যদি((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (তথ্য->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#endif
}
#অন্যথা
#define http_check_auth_closure(x, y) /* empty */
#endif

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
স্থির CURLcode http_handle_send_error(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো SingleRequest *k = &তথ্য->req;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!তথ্য->req.authneg && !conn->bits.close &&
     !Curl_creader_will_rewind(তথ্য)) {
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

    নির্বাচন(তথ্য->state.httpreq) {
    ক্ষেত্র HTTPREQ_PUT:
    ক্ষেত্র HTTPREQ_POST:
    ক্ষেত্র HTTPREQ_POST_FORM:
    ক্ষেত্র HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      যদি(!Curl_req_done_sending(তথ্য)) {
        যদি((k->httpcode == 417) && http_exp100_is_selected(তথ্য)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          যদি(!k->writebytecount && http_exp100_is_waiting(তথ্য)) {
            infof(তথ্য, "Got HTTP failure 417 while waiting for a 100");
          }
          অন্যথা {
            infof(তথ্য, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            ফলাফল = http_perhapsrewind(তথ্য, conn);
            যদি(ফলাফল)
              ফেরত ফলাফল;
          }
          তথ্য->state.disableexpect = TRUE;
          Curl_req_abort_sending(তথ্য);
          DEBUGASSERT(!তথ্য->req.newurl);
          তথ্য->req.newurl = Curl_bufref_dup(&তথ্য->state.url);
          যদি(!তথ্য->req.newurl)
            ফেরত CURLE_OUT_OF_MEMORY;
        }
        অন্যথা যদি(তথ্য->set.http_keep_sending_on_error) {
          infof(তথ্য, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(তথ্য);
        }
        অন্যথা {
          infof(তথ্য, "HTTP error before end of send, stop sending");
          streamclose(conn);
          ফলাফল = Curl_req_abort_sending(তথ্য);
          যদি(ফলাফল)
            ফেরত ফলাফল;
        }
      }
      বিরতি;

    পূর্বনির্ধারিত: /* default label present to avoid compiler warnings */
      বিরতি;
    }
  }

  যদি(Curl_creader_will_rewind(তথ্য) && !Curl_req_done_sending(তথ্য)) {
    /* We rewind before next send, continue sending now */
    infof(তথ্য, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(তথ্য);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode http_on_response(কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক অক্ষর *last_hd, size_t last_hd_len,
                                 ধ্রুবক অক্ষর *buf, size_t blen,
                                 size_t *pconsumed)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো SingleRequest *k = &তথ্য->req;
  bool conn_changed = FALSE;

  (শূন্য)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  যদি(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    যদি(তথ্য->req.httpversion != 20)
      infof(তথ্য, "Lying server, not serving HTTP/2");
  }

  যদি(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    ফলাফল = http_write_header(তথ্য, last_hd, last_hd_len);
    last_hd = ফাঁকা; /* handled it */
    যদি(ফলাফল)
      যাও out;
  }

  যদি(k->httpcode < 100) {
    failf(তথ্য, "Unsupported response code in HTTP response");
    ফলাফল = CURLE_UNSUPPORTED_PROTOCOL;
    যাও out;
  }
  অন্যথা যদি(k->httpcode < 200) {
    ফলাফল = http_on_1xx_response(তথ্য, buf, blen, pconsumed, &conn_changed);
    যাও out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  যদি(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  যদি((k->আকার_2 == -1) && !k->chunk && !conn->bits.close &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     তথ্য->state.httpreq != HTTPREQ_শীর্ষ) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(তথ্য, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_closure(তথ্য, conn);

#ifndef CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  যদি(তথ্য->req.upgr101 == UPGR101_WS) {
    failf(তথ্য, "Refused WebSocket upgrade: %d", k->httpcode);
    ফলাফল = CURLE_HTTP_RETURNED_ERROR;
    যাও out;
  }
#endif

  /* Check if this response means the transfer errored. */
  যদি(http_should_fail(তথ্য, তথ্য->req.httpcode)) {
    failf(তথ্য, "The requested URL returned error: %d",
          k->httpcode);
    ফলাফল = CURLE_HTTP_RETURNED_ERROR;
    যাও out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  ফলাফল = Curl_http_auth_act(তথ্য);
  যদি(ফলাফল)
    যাও out;

  যদি(k->httpcode >= 300) {
    ফলাফল = http_handle_send_error(তথ্য);
    যদি(ফলাফল)
      যাও out;
  }

  /* final response without error, prepare to receive the body */
  ফলাফল = http_firstwrite(তথ্য);
  যদি(ফলাফল)
    যাও out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  ফলাফল = http_আকার(তথ্য);
  যদি(ফলাফল)
    যাও out;

#ifndef CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  যদি((conn->scheme->protocol & CURLPROTO_RTSP) && তথ্য->req.no_body &&
     !তথ্য->set.opt_no_body && (k->আকার_2 > 0))
    তথ্য->req.no_body = FALSE;
#endif

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  যদি(তথ্য->req.no_body)
    k->download_done = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  যদি((k->maxdownload == 0) && (k->httpversion_sent < 20))
    k->download_done = TRUE;

out:
  যদি(last_hd)
    /* if not written yet, write it now */
    ফলাফল = Curl_1st_fatal(ফলাফল,
                            http_write_header(তথ্য, last_hd, last_hd_len));
  যদি(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(তথ্য->multi);
  ফেরত ফলাফল;
}

স্থির CURLcode http_rw_hd(কাঠামো Curl_easy *তথ্য,
                           ধ্রুবক অক্ষর *hd, size_t hdlen,
                           ধ্রুবক অক্ষর *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো SingleRequest *k = &তথ্য->req;
  পূর্ণসংখ্যা writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  যদি((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    কাঠামো dynbuf last_header;
    size_t consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    ফলাফল = curlx_dyn_addn(&last_header, hd, hdlen);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&তথ্য->state.headerb);
    ফলাফল = http_on_response(তথ্য, curlx_dyn_ptr(&last_header),
                              curlx_dyn_len(&last_header),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&last_header);
    ফেরত ফলাফল;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_HEADER;
  যদি(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    যদি(তথ্য->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ধ্রুবক অক্ষর *p = hd;

      curlx_str_passblanks(&p);
      যদি(!strncmp(p, "HTTP/", 5)) {
        p += 5;
        নির্বাচন(*p) {
        ক্ষেত্র '1':
          p++;
          যদি((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            যদি(ISBLANK(p[2])) {
              k->httpversion = (unsigned অক্ষর)(10 + (p[1] - '0'));
              p += 3;
              যদি(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          যদি(!fine_statusline) {
            failf(তথ্য, "Unsupported HTTP/1 subversion in response");
            ফেরত CURLE_UNSUPPORTED_PROTOCOL;
          }
          বিরতি;
        ক্ষেত্র '2':
        ক্ষেত্র '3':
          যদি(!ISBLANK(p[1]))
            বিরতি;
          k->httpversion = (unsigned অক্ষর)((*p - '0') * 10);
          p += 2;
          যদি(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            যদি(!ISBLANK(*p))
              বিরতি;
            fine_statusline = TRUE;
          }
          বিরতি;
        পূর্বনির্ধারিত: /* unsupported */
          failf(তথ্য, "Unsupported HTTP version in response");
          ফেরত CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      যদি(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(তথ্য, hd, hdlen);
        যদি(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    অন্যথা যদি(তথ্য->conn->scheme->protocol & CURLPROTO_RTSP) {
      ধ্রুবক অক্ষর *p = hd;
      কাঠামো Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      যদি(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_সংখ্যা(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        যদি(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (পূর্ণসংখ্যা)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      যদি(!fine_statusline)
        ফেরত CURLE_WEIRD_SERVER_REPLY;
    }

    যদি(fine_statusline) {
      ফলাফল = http_statusline(তথ্য, তথ্য->conn);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      writetype |= CLIENTWRITE_STATUS;
    }
    অন্যথা {
      k->header = FALSE;   /* this is not a header line */
      ফেরত CURLE_WEIRD_SERVER_REPLY;
    }
  }

  ফলাফল = Curl_verify_header(তথ্য, hd, hdlen);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = http_header(তথ্য, hd, hdlen);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(তথ্য, CURLINFO_HEADER_IN, hd, hdlen);

  যদি(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  ফলাফল = Curl_client_write(তথ্য, writetype, hd, hdlen);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = Curl_bump_headersize(তথ্য, hdlen, FALSE);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফেরত CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
শূন্য Curl_http_to_fold(কাঠামো dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  ধ্রুবক অক্ষর *hd = curlx_dyn_ptr(bf);
  যদি(len && (hd[len - 1] == '\n'))
    len--;
  যদি(len && (hd[len - 1] == '\r'))
    len--;
  যখন(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

স্থির শূন্য unfold_header(কাঠামো Curl_easy *তথ্য)
{
  Curl_http_to_fold(&তথ্য->state.headerb);
  তথ্য->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
স্থির CURLcode http_parse_headers(কাঠামো Curl_easy *তথ্য,
                                   ধ্রুবক অক্ষর *buf, size_t blen,
                                   size_t *pconsumed)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো SingleRequest *k = &তথ্য->req;
  ধ্রুবক অক্ষর *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  যদি(তথ্য->state.maybe_folded && blen) {
    যদি(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(তথ্য);
    }
    অন্যথা {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      ফলাফল = http_rw_hd(তথ্য, curlx_dyn_ptr(&তথ্য->state.headerb),
                          curlx_dyn_len(&তথ্য->state.headerb),
                          ফাঁকা, 0, &ignore_this);
      curlx_dyn_reset(&তথ্য->state.headerb);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    তথ্য->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  যখন(blen && k->header) {
    size_t consumed;
    size_t hlen;
    ধ্রুবক অক্ষর *hd;
    size_t unfold_len = 0;

    যদি(তথ্য->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      যখন(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      যদি(blen) {
        /* insert a single space */
        ফলাফল = curlx_dyn_addn(&তথ্য->state.headerb, " ", 1);
        যদি(ফলাফল)
          ফেরত ফলাফল;
        তথ্য->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = memchr(buf, '\n', blen);
    যদি(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      ফলাফল = curlx_dyn_addn(&তথ্য->state.headerb, buf, blen);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      *pconsumed += blen + unfold_len;

      যদি(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(তথ্য, conn,
                           curlx_dyn_ptr(&তথ্য->state.headerb),
                           curlx_dyn_len(&তথ্য->state.headerb));

        যদি(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          যদি((k->httpversion >= 10) || conn->bits.reuse) {
            failf(তথ্য, "Invalid status line");
            ফেরত CURLE_WEIRD_SERVER_REPLY;
          }
          যদি(!তথ্য->state.http_neg.accept_09) {
            failf(তথ্য, "Received HTTP/0.9 when not allowed");
            ফেরত CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          যাও out;
        }
      }
      যাও out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    ফলাফল = curlx_dyn_addn(&তথ্য->state.headerb, buf, consumed);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&তথ্য->state.headerb);
    hd = curlx_dyn_ptr(&তথ্য->state.headerb);

    যদি(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(তথ্য, conn, hd, hlen);
      যদি(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        যদি((k->httpversion >= 10) || conn->bits.reuse) {
          failf(তথ্য, "Invalid status line");
          ফেরত CURLE_WEIRD_SERVER_REPLY;
        }
        যদি(!তথ্য->state.http_neg.accept_09) {
          failf(তথ্য, "Received HTTP/0.9 when not allowed");
          ফেরত CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        যাও out;
      }
    }
    অন্যথা {
      যদি(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        যদি(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(তথ্য);
          চালু;
        }
        অন্যথা যদি(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          তথ্য->state.maybe_folded = TRUE;
          বিরতি;
        }
      }
    }

    ফলাফল = http_rw_hd(তথ্য, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&তথ্য->state.headerb);
    যদি(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  যদি(!k->header && !leftover_body) {
    curlx_dyn_free(&তথ্য->state.headerb);
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক অক্ষর *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode ফলাফল;
  size_t consumed;
  অক্ষর tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  ফলাফল = http_rw_hd(তথ্য, hd, hdlen, &tmp, 0, &consumed);
  যদি(!ফলাফল && is_eos) {
    ফলাফল = Curl_client_write(তথ্য, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  ফেরত ফলাফল;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক অক্ষর *buf, size_t blen,
                                  size_t *pconsumed)
{
  যদি(!তথ্য->req.header) {
    *pconsumed = 0;
    ফেরত CURLE_OK;
  }
  অন্যথা {
    CURLcode ফলাফল;

    ফলাফল = http_parse_headers(তথ্য, buf, blen, pconsumed);
    যদি(!ফলাফল && !তথ্য->req.header) {
      যদি(!তথ্য->req.no_body && curlx_dyn_len(&তথ্য->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&তথ্য->state.headerb),
                                   curlx_dyn_len(&তথ্য->state.headerb));
      }
      curlx_dyn_free(&তথ্য->state.headerb);
    }
    ফেরত ফলাফল;
  }
}

CURLcode Curl_http_write_resp(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *buf, size_t blen,
                              bool is_eos)
{
  CURLcode ফলাফল;
  size_t consumed;
  পূর্ণসংখ্যা flags;

  ফলাফল = Curl_http_write_resp_hds(তথ্য, buf, blen, &consumed);
  যদি(ফলাফল || তথ্য->req.done)
    যাও out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !তথ্য->req.header);
  যদি(!তথ্য->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    যদি(is_eos)
      flags |= CLIENTWRITE_EOS;
    ফলাফল = Curl_client_write(তথ্য, flags, buf, blen);
  }
out:
  ফেরত ফলাফল;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, ধ্রুবক অক্ষর *s, size_t len)
{
  CURLcode ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  পূর্ণসংখ্যা i;

  যদি(len != 3)
    যাও out;

  জন্য(i = 0; i < 3; ++i) {
    অক্ষর c = s[i];

    যদি(c < '0' || c > '9')
      যাও out;

    status *= 10;
    status += c - '0';
  }
  ফলাফল = CURLE_OK;
out:
  *pstatus = ফলাফল ? -1 : status;
  ফেরত ফলাফল;
}

CURLcode Curl_http_req_make(কাঠামো httpreq **preq,
                            ধ্রুবক অক্ষর *method, size_t m_len,
                            ধ্রুবক অক্ষর *scheme, size_t s_len,
                            ধ্রুবক অক্ষর *authority, size_t a_len,
                            ধ্রুবক অক্ষর *path, size_t p_len)
{
  কাঠামো httpreq *req;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, আকার(*req) + m_len);
  যদি(!req)
    যাও out;
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ঠেলো
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memcpy(req->method, method, m_len);
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic বের_করো
#endif
  যদি(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    যদি(!req->scheme)
      যাও out;
  }
  যদি(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    যদি(!req->authority)
      যাও out;
  }
  যদি(path) {
    req->path = curlx_memdup0(path, p_len);
    যদি(!req->path)
      যাও out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  ফলাফল = CURLE_OK;

out:
  যদি(ফলাফল && req)
    Curl_http_req_free(req);
  *preq = ফলাফল ? ফাঁকা : req;
  ফেরত ফলাফল;
}

স্থির CURLcode req_assign_url_authority(কাঠামো httpreq *req, CURLU *url)
{
  অক্ষর *host, *port;
  কাঠামো dynbuf buf;
  CURLUcode uc;
  CURLcode ফলাফল = CURLE_URL_MALFORMAT;

  host = port = ফাঁকা;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  যদি(uc && uc != CURLUE_NO_HOST)
    যাও out;
  যদি(!host) {
    req->authority = ফাঁকা;
    ফলাফল = CURLE_OK;
    যাও out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  যদি(uc && uc != CURLUE_NO_PORT)
    যাও out;

  ফলাফল = curlx_dyn_add(&buf, host);
  যদি(ফলাফল)
    যাও out;
  যদি(port) {
    ফলাফল = curlx_dyn_addf(&buf, ":%s", port);
    যদি(ফলাফল)
      যাও out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  যদি(ফলাফল)
    curlx_dyn_free(&buf);
  ফেরত ফলাফল;
}

স্থির CURLcode req_assign_url_path(কাঠামো httpreq *req, CURLU *url)
{
  অক্ষর *path, *query;
  কাঠামো dynbuf buf;
  CURLUcode uc;
  CURLcode ফলাফল = CURLE_URL_MALFORMAT;

  path = query = ফাঁকা;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  যদি(uc)
    যাও out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  যদি(uc && uc != CURLUE_NO_QUERY)
    যাও out;

  যদি(!query) {
    req->path = path;
    path = ফাঁকা;
  }
  অন্যথা {
    ফলাফল = curlx_dyn_add(&buf, path);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addf(&buf, "?%s", query);
    যদি(ফলাফল)
      যাও out;
    req->path = curlx_dyn_ptr(&buf);
  }
  ফলাফল = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  যদি(ফলাফল)
    curlx_dyn_free(&buf);
  ফেরত ফলাফল;
}

CURLcode Curl_http_req_make2(কাঠামো httpreq **preq,
                             ধ্রুবক অক্ষর *method, size_t m_len,
                             CURLU *url, ধ্রুবক অক্ষর *scheme_default)
{
  কাঠামো httpreq *req;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, আকার(*req) + m_len);
  যদি(!req)
    যাও out;
  memcpy(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  যদি(uc && uc != CURLUE_NO_SCHEME)
    যাও out;
  যদি(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    যদি(!req->scheme)
      যাও out;
  }

  ফলাফল = req_assign_url_authority(req, url);
  যদি(ফলাফল)
    যাও out;
  ফলাফল = req_assign_url_path(req, url);
  যদি(ফলাফল)
    যাও out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  ফলাফল = CURLE_OK;

out:
  যদি(ফলাফল && req)
    Curl_http_req_free(req);
  *preq = ফলাফল ? ফাঁকা : req;
  ফেরত ফলাফল;
}

শূন্য Curl_http_req_free(কাঠামো httpreq *req)
{
  যদি(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->path);
    Curl_dynhds_free(&req->headers);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

কাঠামো name_const {
  ধ্রুবক অক্ষর *name;
  size_t namelen;
};

স্থির ধ্রুবক কাঠামো name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

স্থির bool h2_permissible_field(কাঠামো dynhds_entry *e)
{
  size_t i;
  জন্য(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    যদি(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      ফেরত FALSE;
  }
  ফেরত TRUE;
}

স্থির bool http_TE_has_token(ধ্রুবক অক্ষর *fvalue, ধ্রুবক অক্ষর *token)
{
  যখন(*fvalue) {
    কাঠামো Curl_str name;

    /* skip to first token */
    যখন(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    যদি(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      ফেরত FALSE;
    যদি(curlx_str_casecompare(&name, token))
      ফেরত TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    যখন(*fvalue && *fvalue != ',') {
      যদি(*fvalue == '"') {
        কাঠামো Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        যদি(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          ফেরত FALSE;
      }
      অন্যথা
        fvalue++;
    }
  }
  ফেরত FALSE;
}

CURLcode Curl_http_req_to_h2(কাঠামো dynhds *h2_headers,
                             কাঠামো httpreq *req, কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক অক্ষর *scheme = ফাঁকা, *authority = ফাঁকা;
  কাঠামো dynhds_entry *e;
  size_t i;
  CURLcode ফলাফল;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  যদি(req->scheme) {
    scheme = req->scheme;
  }
  অন্যথা যদি(strcmp("CONNECT", req->method)) {
    scheme = Curl_checkheaders(তথ্য, STRCONST(HTTP_PSEUDO_SCHEME));
    যদি(scheme) {
      scheme += আকার(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(তথ্য, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    অন্যথা {
      scheme = তথ্য->state.origin->scheme->name;
    }
  }

  যদি(req->authority) {
    authority = req->authority;
  }
  অন্যথা {
    e = Curl_dynhds_get(&req->headers, STRCONST("Host"));
    যদি(e)
      authority = e->মান;
  }

  Curl_dynhds_reset(h2_headers);
  Curl_dynhds_set_opts(h2_headers, DYNHDS_OPT_LOWERCASE);
  ফলাফল = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, strlen(req->method));
  যদি(!ফলাফল && scheme) {
    ফলাফল = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, strlen(scheme));
  }
  যদি(!ফলাফল && authority) {
    ফলাফল = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, strlen(authority));
  }
  যদি(!ফলাফল && req->path) {
    ফলাফল = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, strlen(req->path));
  }
  জন্য(i = 0; !ফলাফল && i < Curl_dynhds_গুনতি(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    যদি(e->namelen == 2 && curl_strequal("TE", e->name)) {
      যদি(http_TE_has_token(e->মান, "trailers"))
        ফলাফল = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    অন্যথা যদি(h2_permissible_field(e)) {
      ফলাফল = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->মান, e->valuelen);
    }
  }

  ফেরত ফলাফল;
}

CURLcode Curl_http_resp_make(কাঠামো http_resp **presp,
                             পূর্ণসংখ্যা status,
                             ধ্রুবক অক্ষর *description)
{
  কাঠামো http_resp *resp;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, আকার(*resp));
  যদি(!resp)
    যাও out;

  resp->status = status;
  যদি(description) {
    resp->description = curlx_strdup(description);
    যদি(!resp->description)
      যাও out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  ফলাফল = CURLE_OK;

out:
  যদি(ফলাফল && resp)
    Curl_http_resp_free(resp);
  *presp = ফলাফল ? ফাঁকা : resp;
  ফেরত ফলাফল;
}

শূন্য Curl_http_resp_free(কাঠামো http_resp *resp)
{
  যদি(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    যদি(resp->পূর্ব)
      Curl_http_resp_free(resp->পূর্ব);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_http = {
  Curl_http_setup_conn,                 /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_done,                       /* done */
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

#endif /* CURL_DISABLE_HTTP */
