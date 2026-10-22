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

শূন্য Curl_http_neg_init(সংরচনা Curl_easy *data, সংরচনা http_negotiation *neg)
{
  memset(neg, 0, পরিমাণ(*neg));
  neg->accept_09 = data->set.http09_allowed;
  বিকল্প(data->set.httpwant) {
  পক্ষ CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    ভঙ্গ;
  পক্ষ CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    ভঙ্গ;
  পক্ষ CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    ভঙ্গ;
  পক্ষ CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    ভঙ্গ;
  পক্ষ CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    data->state.http_neg.h2_prior_knowledge = TRUE;
    ভঙ্গ;
  পক্ষ CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    ভঙ্গ;
  পক্ষ CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    ভঙ্গ;
  পক্ষ CURL_HTTP_VERSION_NONE:
  স্বভাব:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    ভঙ্গ;
  }
}

CURLcode Curl_http_setup_conn(সংরচনা Curl_easy *data,
                              সংরচনা connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  যদি(data->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode result = Curl_conn_may_http3(data, conn);
    যদি(result)
      প্রত্যাগম result;
  }
  প্রত্যাগম CURLE_OK;
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
বর্ণ *Curl_checkProxyheaders(সংরচনা Curl_easy *data,
                             ধ্রুব সংরচনা connectdata *conn,
                             ধ্রুব বর্ণ *thisheader,
                             ধ্রুব size_t thislen)
{
  সংরচনা curl_slist *head;

  প্রতি(head = (conn->http_proxy.peer && data->set.sep_headers) ?
        data->set.proxyheaders : data->set.headers;
      head; head = head->next) {
    যদি(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      প্রত্যাগম head->data;
  }

  প্রত্যাগম রিক্ত;
}
#endif

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
স্থির bool header_has_মান(ধ্রুব বর্ণ **headerp, সংরচনা Curl_str *outp)
{
  bool মান = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  যদি(মান) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  প্রত্যাগম মান;
}

স্থির bool http_header_is_empty(ধ্রুব বর্ণ *header)
{
  সংরচনা Curl_str out;

  যদি(header_has_মান(&header, &out)) {
    প্রত্যাগম curlx_strlen(&out) == 0;
  }
  প্রত্যাগম TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
স্থির CURLcode copy_custom_মান(ধ্রুব বর্ণ *header, বর্ণ **valp)
{
  সংরচনা Curl_str out = { 0 };

  /* find the end of the header name */
  যদি(header_has_মান(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    যদি(*valp)
      প্রত্যাগম CURLE_OK;
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = রিক্ত;
  প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
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
বর্ণ *Curl_copy_header_মান(ধ্রুব বর্ণ *header)
{
  সংরচনা Curl_str out;

  /* find the end of the header name */
  যদি(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_SIZE, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_SIZE);
    curlx_str_trimblanks(&out);
    প্রত্যাগম curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  প্রত্যাগম রিক্ত;
}

#ifndef CURL_DISABLE_HTTP_AUTH

#ifndef CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
স্থির CURLcode http_output_basic(সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn, bool proxy)
{
  size_t size = 0;
  বর্ণ *authorization = রিক্ত;
  বর্ণ **p_hd;
  CURLcode result;
  সংরচনা Curl_creds *creds = রিক্ত;
  বর্ণ *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  যদি(proxy) {
#ifndef CURL_DISABLE_PROXY
    p_hd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#অন্যথা
    (শূন্য)conn;
    প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
  }
  অন্যথা {
    p_hd = &data->req.hd_auth;
    creds = data->state.creds;
  }

  যদি(!creds) {
    DEBUGASSERT(0);
    প্রত্যাগম CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  যদি(!out)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  result = curlx_base64_encode((uint8_t *)out, strlen(out),
                               &authorization, &size);
  যদি(result)
    গচ্ছ fail;

  যদি(!authorization) {
    result = CURLE_REMOTE_ACCESS_DENIED;
    গচ্ছ fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  যদি(!*p_hd) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ fail;
  }

fail:
  curlx_free(out);
  প্রত্যাগম result;
}

#endif

#ifndef CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
স্থির CURLcode http_output_bearer(সংরচনা Curl_easy *data)
{
  বর্ণ **userp;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(data->state.creds));
  userp = &data->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(data->state.creds));

  যদি(!*userp) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ fail;
  }

fail:
  প্রত্যাগম result;
}
#endif

#endif

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
স্থির bool pickoneauth(সংরচনা auth *pick, unsigned long mask,
                        সংরচনা Curl_creds *creds)
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

  প্রত্যাগম picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
স্থির CURLcode http_perhapsrewind(সংরচনা Curl_easy *data,
                                   সংরচনা connectdata *conn)
{
  curl_off_t bytessent = data->req.writebytecount;
  curl_off_t expectsend = Curl_creader_total_length(data);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(data);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!data->req.upload_done && !little_upload_remains);
  VERBOSE(ধ্রুব বর্ণ *ongoing_auth = রিক্ত);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  যদি(needs_rewind) {
    infof(data, "Need to rewind upload for next request");
    Curl_creader_set_rewind(data, TRUE);
  }

  যদি(conn->bits.close)
    /* If we already decided to close this connection, we cannot veto. */
    প্রত্যাগম CURLE_OK;

  যদি(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ifdef USE_NTLM
    যদি((data->state.authproxy.picked == CURLAUTH_NTLM) ||
       (data->state.authhost.picked == CURLAUTH_NTLM)) {
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
    যদি((data->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (data->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
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
      infof(data, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    অন্যথা
      infof(data, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    data->req.size = 0; /* do not download any more than 0 bytes */
    data->req.http_bodyless = TRUE;
  }
  প্রত্যাগম CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
স্থির bool http_should_fail(সংরচনা Curl_easy *data, পূর্ণাঙ্ক httpcode)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  যদি(!data->set.http_fail_on_error)
    প্রত্যাগম FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  যদি(httpcode < 400)
    প্রত্যাগম FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  যদি(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    প্রত্যাগম FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  যদি((httpcode != 401) && (httpcode != 407))
    প্রত্যাগম TRUE;

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
  যদি((httpcode == 401) && !data->state.creds)
    প্রত্যাগম TRUE;
#ifndef CURL_DISABLE_PROXY
  যদি((httpcode == 407) && !data->conn->http_proxy.creds)
    প্রত্যাগম TRUE;
#endif

  প্রত্যাগম (bool)data->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(সংরচনা Curl_easy *data)
{
  সংরচনা connectdata *conn = data->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode result = CURLE_OK;
  unsigned long authmask = ~0UL;

  যদি(!Curl_creds_has_oauth_bearer(data->state.creds))
    authmask &= (unsigned long)~CURLAUTH_BEARER;

  যদি(100 <= data->req.httpcode && data->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    প্রত্যাগম CURLE_OK;

  যদি(data->state.authproblem)
    প্রত্যাগম data->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  যদি(data->state.creds &&
     ((data->req.httpcode == 401) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickhost = pickoneauth(&data->state.authhost, authmask, data->state.creds);
    যদি(!pickhost)
      data->state.authproblem = TRUE;
    অন্যথা
      data->info.httpauthpicked = data->state.authhost.picked;
    যদি(data->state.authhost.picked == CURLAUTH_NTLM &&
       (data->req.httpversion_sent > 11)) {
      infof(data, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ifndef CURL_DISABLE_PROXY
  যদি(conn->http_proxy.creds &&
     ((data->req.httpcode == 407) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickproxy = pickoneauth(&data->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    যদি(!pickproxy)
      data->state.authproblem = TRUE;
    অন্যথা
      data->info.proxyauthpicked = data->state.authproxy.picked;
  }
#endif

  যদি(pickhost || pickproxy) {
    result = http_perhapsrewind(data, conn);
    যদি(result)
      প্রত্যাগম result;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(data->req.newurl);
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    যদি(!data->req.newurl)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }
  অন্যথা যদি((data->req.httpcode < 300) &&
          !data->state.authhost.done &&
          data->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (data->state.httpreq != HTTPREQ_GET) &&
          (data->state.httpreq != HTTPREQ_HEAD)) {
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    যদি(!data->req.newurl)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    data->state.authhost.done = TRUE;
  }
  যদি(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          data->req.httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
  }

  প্রত্যাগম result;
}

#ifndef CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
স্থির CURLcode output_auth_headers(সংরচনা Curl_easy *data,
                                    সংরচনা connectdata *conn,
                                    সংরচনা auth *authstatus,
                                    ধ্রুব বর্ণ *request,
                                    ধ্রুব বর্ণ *path,
                                    bool proxy)
{
  ধ্রুব বর্ণ *auth = রিক্ত;
  CURLcode result = CURLE_OK;
  (শূন্য)conn;

#ifdef CURL_DISABLE_DIGEST_AUTH
  (শূন্য)request;
  (শূন্য)path;
#endif
#ifndef CURL_DISABLE_AWS
  যদি((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    result = Curl_output_aws_sigv4(data);
    যদি(result)
      প্রত্যাগম result;
  }
  অন্যথা
#endif
#ifndef CURL_DISABLE_HTTPSIG
  যদি((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    যদি(Curl_auth_allowed_to_host(data)) {
      auth = "HTTPSIG";
      result = Curl_output_httpsig(data);
      যদি(result)
        প্রত্যাগম result;
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
      (proxy && !Curl_checkProxyheaders(data, conn,
                                        STRCONST("Proxy-authorization"))) ||
#endif
      (!proxy && !Curl_checkheaders(data, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(data))) {
      auth = "Negotiate";
      result = Curl_output_negotiate(data, conn, proxy);
      যদি(result)
        প্রত্যাগম result;
    }
    অন্যথা
      authstatus->done = TRUE;
  }
  অন্যথা
#endif
#ifdef USE_NTLM
  যদি(authstatus->picked == CURLAUTH_NTLM) {
    যদি(proxy || Curl_auth_allowed_to_host(data)) {
      auth = "NTLM";
      result = Curl_output_ntlm(data, proxy);
      যদি(result)
        প্রত্যাগম result;
    }
    অন্যথা
      authstatus->done = TRUE;
  }
  অন্যথা
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  যদি(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    result = Curl_output_digest(data,
                                proxy,
                                (ধ্রুব unsigned বর্ণ *)request,
                                (ধ্রুব unsigned বর্ণ *)path);
    যদি(result)
      প্রত্যাগম result;
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
        !Curl_checkProxyheaders(data, conn,
                                STRCONST("Proxy-authorization"))) ||
#endif
       (!proxy && data->state.creds &&
        Curl_creds_has_user_or_pass(data->state.creds) &&
        !Curl_checkheaders(data, STRCONST("Authorization")))) {
      auth = "Basic";
      result = http_output_basic(data, conn, proxy);
      যদি(result)
        প্রত্যাগম result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
  যদি(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    যদি(!proxy && Curl_creds_has_oauth_bearer(data->state.creds) &&
       !Curl_checkheaders(data, STRCONST("Authorization"))) {
      auth = "Bearer";
      result = http_output_bearer(data);
      যদি(result)
        প্রত্যাগম result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif

  যদি(auth) {
#ifndef CURL_DISABLE_PROXY
    যদি(proxy)
      data->info.proxyauthpicked = authstatus->picked;
    অন্যথা
      data->info.httpauthpicked = authstatus->picked;
    infof(data, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (data->state.creds ?
           data->state.creds->user : ""));
#অন্যথা
    (শূন্য)proxy;
    infof(data, "Server auth using %s with user '%s'",
          auth, data->state.creds ?
          data->state.creds->user : "");
#endif
    authstatus->multipass = !authstatus->done;
  }
  অন্যথা {
    authstatus->multipass = FALSE;
    যদি(proxy)
      data->info.proxyauthpicked = 0;
    অন্যথা
      data->info.httpauthpicked = 0;
  }

  প্রত্যাগম result;
}

CURLcode Curl_http_output_auth(সংরচনা Curl_easy *data,
                               সংরচনা connectdata *conn,
                               ধ্রুব বর্ণ *request,
                               Curl_HttpReq httpreq,
                               ধ্রুব বর্ণ *path,
                               ধ্রুব বর্ণ *query,
                               bool is_connect)
{
  CURLcode result = CURLE_OK;
  সংরচনা auth *authhost;
  সংরচনা auth *authproxy;
  ধ্রুব বর্ণ *path_and_query = path;
  বর্ণ *tmp_str = রিক্ত;

  DEBUGASSERT(data);
  authhost = &data->state.authhost;
  authproxy = &data->state.authproxy;

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
    !data->state.creds) {
    /* no authentication with no user or password */
    authhost->done = TRUE;
    authproxy->done = TRUE;
    result = CURLE_OK;
    গচ্ছ out;
  }

  যদি(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    যদি(!tmp_str) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
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
    result = output_auth_headers(data, conn, authproxy, request,
                                 path_and_query, TRUE);
    যদি(result)
      গচ্ছ out;
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
  যদি(data->state.creds || Curl_auth_allowed_to_host(data))
    result = output_auth_headers(data, conn, authhost, request,
                                 path_and_query, FALSE);
  অন্যথা
    authhost->done = TRUE;

  যদি(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_HEAD)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    data->req.authneg = TRUE;
  }
  অন্যথা
    data->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  প্রত্যাগম result;
}

#অন্যথা /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(সংরচনা Curl_easy *data,
                               সংরচনা connectdata *conn,
                               ধ্রুব বর্ণ *request,
                               Curl_HttpReq httpreq,
                               ধ্রুব বর্ণ *path,
                               ধ্রুব বর্ণ *query,
                               bool is_connect)
{
  (শূন্য)data;
  (শূন্য)conn;
  (শূন্য)request;
  (শূন্য)httpreq;
  (শূন্য)path;
  (শূন্য)query;
  (শূন্য)is_connect;
  প্রত্যাগম CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP_AUTH, else */

#যদি defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
স্থির bool authcmp(ধ্রুব বর্ণ *auth, ধ্রুব বর্ণ *সারি)
{
  /* the auth string must not have an alnum following */
  size_t n = strlen(auth);
  প্রত্যাগম curl_strnequal(auth, সারি, n) && !ISALNUM(সারি[n]);
}
#endif

#ifdef USE_SPNEGO
স্থির CURLcode auth_spnego(সংরচনা Curl_easy *data,
                            bool proxy,
                            ধ্রুব বর্ণ *auth,
                            সংরচনা auth *authp,
                            uint32_t *availp)
{
  যদি((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    যদি(authp->picked == CURLAUTH_NEGOTIATE) {
      সংরচনা connectdata *conn = data->conn;
      CURLcode result = Curl_প্রবেশ_negotiate(data, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      যদি(!result) {
        curlx_free(data->req.newurl);
        data->req.newurl = Curl_bufref_dup(&data->state.url);
        যদি(!data->req.newurl)
          প্রত্যাগম CURLE_OUT_OF_MEMORY;
        data->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      অন্যথা
        data->state.authproblem = TRUE;
    }
  }
  প্রত্যাগম CURLE_OK;
}
#endif

#ifdef USE_NTLM
স্থির CURLcode auth_ntlm(সংরচনা Curl_easy *data,
                          bool proxy,
                          ধ্রুব বর্ণ *auth,
                          সংরচনা auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  যদি((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    যদি(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode result = Curl_প্রবেশ_ntlm(data, proxy, auth);
      যদি(!result)
        data->state.authproblem = FALSE;
      অন্যথা {
        যদি(result == CURLE_OUT_OF_MEMORY)
          প্রত্যাগম result;
        infof(data, "NTLM authentication problem, ignoring.");
        data->state.authproblem = TRUE;
      }
    }
  }
  প্রত্যাগম CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_DIGEST_AUTH
স্থির CURLcode auth_digest(সংরচনা Curl_easy *data,
                            bool proxy,
                            ধ্রুব বর্ণ *auth,
                            সংরচনা auth *authp,
                            uint32_t *availp)
{
  যদি(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(data, "Ignoring duplicate digest auth header.");
  }
  অন্যথা যদি(Curl_auth_is_digest_supported()) {
    CURLcode result;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    result = Curl_প্রবেশ_digest(data, proxy, auth);
    যদি(result) {
      যদি(result == CURLE_OUT_OF_MEMORY)
        প্রত্যাগম result;
      infof(data, "Digest authentication problem, ignoring.");
      data->state.authproblem = TRUE;
    }
  }
  প্রত্যাগম CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BASIC_AUTH
স্থির CURLcode auth_basic(সংরচনা Curl_easy *data,
                           সংরচনা auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  যদি(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Basic authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  প্রত্যাগম CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BEARER_AUTH
স্থির CURLcode auth_bearer(সংরচনা Curl_easy *data,
                            সংরচনা auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  যদি(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Bearer authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  প্রত্যাগম CURLE_OK;
}
#endif

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_প্রবেশ_auth(সংরচনা Curl_easy *data, bool proxy,
                              ধ্রুব বর্ণ *auth) /* the first non-space */
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
  সংরচনা auth *authp;
  CURLcode result = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(data);

  যদি(proxy) {
    availp = &data->info.proxyauthavail;
    authp = &data->state.authproxy;
  }
  অন্যথা {
    availp = &data->info.httpauthavail;
    authp = &data->state.authhost;
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

  যাবত্(*auth) {
#ifdef USE_SPNEGO
    যদি(authcmp("Negotiate", auth))
      result = auth_spnego(data, proxy, auth, authp, availp);
#endif
#ifdef USE_NTLM
    যদি(!result && authcmp("NTLM", auth))
      result = auth_ntlm(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
    যদি(!result && authcmp("Digest", auth))
      result = auth_digest(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
    যদি(!result && authcmp("Basic", auth))
      result = auth_basic(data, authp, availp);
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
    যদি(authcmp("Bearer", auth))
      result = auth_bearer(data, authp, availp);
#endif

    যদি(result)
      ভঙ্গ;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    যদি(auth) /* if we are on a comma, skip it */
      auth++;
    অন্যথা
      ভঙ্গ;
    curlx_str_passblanks(&auth);
  }
  প্রত্যাগম result;
#অন্যথা
  (শূন্য)data;
  (শূন্য)proxy;
  (শূন্য)auth;
  /* nothing to do when disabled */
  প্রত্যাগম CURLE_OK;
#endif
}

স্থির শূন্য http_switch_to_get(সংরচনা Curl_easy *data, পূর্ণাঙ্ক code)
{
  ধ্রুব বর্ণ *req = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  যদি((req || data->state.httpreq != HTTPREQ_GET) &&
     (data->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((শূন্য)code);
    infof(data, "Switch to GET because of %d response", code);
    data->state.http_ignorecustom = TRUE;
  }
  অন্যথা যদি(req && (data->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(data, "Stick to %s instead of GET", req);

  data->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(data, FALSE);
}

#define HTTPREQ_IS_POST(data)                    \
  ((data)->state.httpreq == HTTPREQ_POST ||      \
   (data)->state.httpreq == HTTPREQ_POST_FORM || \
   (data)->state.httpreq == HTTPREQ_POST_MIME)

স্থির CURLcode set_auto_referer(সংরচনা Curl_easy *data)
{
  CURLU *u;
  বর্ণ *referer = রিক্ত;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&data->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  যদি(!u)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&data->state.url), 0);
  যদি(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, রিক্ত, 0);
  যদি(!uc)
    uc = curl_url_set(u, CURLUPART_USER, রিক্ত, 0);
  যদি(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, রিক্ত, 0);
  যদি(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  যদি(uc || !referer)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&data->state.referer, referer, 0, curl_free);
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode follow_parse_url(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *newurl,
                                 followtype type, বর্ণ **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(data->state.uh);
  uc = curl_url_set(data->state.uh, CURLUPART_URL, newurl, (unsigned পূর্ণাঙ্ক)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  যদি(uc) {
    যদি((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(data, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      প্রত্যাগম Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    যদি(!*follow_urlp)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }
  অন্যথা {
    CURLU *u = curl_url();
    যদি(!u)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&data->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    যদি(!uc)
      uc = curl_url_get(data->state.uh, CURLUPART_URL, follow_urlp, 0);
    যদি(uc) {
      curl_url_cleanup(u);
      প্রত্যাগম Curl_uc_to_curlcode(uc);
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, data->state.uh);
      curl_url_cleanup(u);
      যদি(!same_origin)
        Curl_auth_digest_cleanup(&data->state.digest);
    }
#অন্যথা
    curl_url_cleanup(u);
#endif
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode follow_fake(সংরচনা Curl_easy *data, বর্ণ *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  যদি(!uc) {
    CURLU *u = curl_url();
    বর্ণ *nocred = রিক্ত;

    যদি(!u) {
      curlx_free(follow_url);
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
    যদি(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, রিক্ত, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, রিক্ত, 0))
      (শূন্য)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    যদি(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  data->info.wouldredirect = follow_url;

  যদি(reachedmax) {
    failf(data, "Maximum (%d) redirects followed", data->set.maxredirs);
    প্রত্যাগম CURLE_TOO_MANY_REDIRECTS;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির bool follow_switch_get(সংরচনা Curl_easy *data)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  বিকল্প(data->info.httpcode) {
  পক্ষ 301: /* Moved Permanently */
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
    যদি(HTTPREQ_IS_POST(data) && !data->set.post301) {
      http_switch_to_get(data, 301);
      প্রত্যাগম TRUE;
    }
    ভঙ্গ;
  পক্ষ 302: /* Found */
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
    যদি(HTTPREQ_IS_POST(data) && !data->set.post302) {
      http_switch_to_get(data, 302);
      প্রত্যাগম TRUE;
    }
    ভঙ্গ;
  পক্ষ 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    যদি(!HTTPREQ_IS_POST(data) || !data->set.post303) {
      http_switch_to_get(data, 303);
      প্রত্যাগম TRUE;
    }
    ভঙ্গ;

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
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম FALSE;
}

CURLcode Curl_http_follow(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  বর্ণ *follow_url = রিক্ত;
  CURLUcode uc;
  CURLcode result;
  CURLcode rewind_result;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  যদি(type != FOLLOW_FAKE)
    data->state.requests++; /* count all real follows */
  যদি(type == FOLLOW_REDIR) {
    যদি((data->set.maxredirs != -1) &&
       (data->state.followlocation >= data->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    অন্যথা {
      data->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      যদি(data->set.http_auto_referer) {
        result = set_auto_referer(data);
        যদি(result)
          প্রত্যাগম result;
      }
    }
  }

  যদি((type != FOLLOW_RETRY) &&
     (data->req.httpcode != 401) && (data->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, রিক্ত, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  result = follow_parse_url(data, newurl, type, &follow_url, &uc);
  যদি(result)
    প্রত্যাগম result;

  DEBUGASSERT(follow_url);

  যদি(type == FOLLOW_FAKE)
    প্রত্যাগম follow_fake(data, follow_url, uc, reachedmax);

  যদি(disallowport)
    data->state.allow_port = FALSE;

  Curl_bufref_set(&data->state.url, follow_url, 0, curl_free);
  rewind_result = Curl_req_soft_reset(&data->req, data);
  infof(data, "Issue another request to this URL: '%s'", follow_url);
  যদি((data->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    data->state.http_ignorecustom = TRUE;
    infof(data, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(data);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  যদি(rewind_result && !switch_to_get)
    প্রত্যাগম rewind_result;

  Curl_pgrsTime(data, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(data);

  প্রত্যাগম CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(ধ্রুব বর্ণ *headerline, /* line to check */
                        ধ্রুব বর্ণ *header, /* header keyword _with_ colon */
                        ধ্রুব size_t hlen, /* len of the keyword in bytes */
                        ধ্রুব বর্ণ *content, /* content string to find */
                        ধ্রুব size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  ধ্রুব বর্ণ *p;
  সংরচনা Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  যদি(!curl_strnequal(headerline, header, hlen))
    প্রত্যাগম FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  যদি(curlx_str_cspn(&p, &val, "\r\n"))
    প্রত্যাগম FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  যদি(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    প্রতি(len = curlx_strlen(&val); len >= clen;) {
      সংরচনা Curl_str next;
      ধ্রুব বর্ণ *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      যদি(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        প্রত্যাগম TRUE; /* match! */
      /* advance to the next comma */
      যদি(curlx_str_until(&p, &next, len, ',') ||
         curlx_str_single(&p, ','))
        ভঙ্গ; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      কুরু
        curlx_str_passblanks(&p);
      যাবত্(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      যদি((size_t)(p - o) > len)
        ভঙ্গ;
      len -= (p - o);
    }
  }
  প্রত্যাগম FALSE; /* no match */
}

সংরচনা cr_exp100_ctx {
  সংরচনা Curl_creader super;
  সংরচনা curltime start; /* time started waiting */
  গণনা expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

স্থির শূন্য http_exp100_continue(সংরচনা Curl_easy *data,
                                 সংরচনা Curl_creader *reader)
{
  সংরচনা cr_exp100_ctx *ctx = reader->ctx;
  যদি(ctx->state > EXP100_SEND_DATA) {
    ctx->state = EXP100_SEND_DATA;
    Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
  }
}

স্থির CURLcode cr_exp100_read(সংরচনা Curl_easy *data,
                               সংরচনা Curl_creader *reader,
                               বর্ণ *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  সংরচনা cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  বিকল্প(ctx->state) {
  পক্ষ EXP100_SENDING_REQUEST:
    যদি(!Curl_req_sendbuf_empty(data)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(data, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      প্রত্যাগম CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(data, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", data->set.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->start = *Curl_pgrs_now(data);
    Curl_expire_set(data, EXPIRE_100_TIMEOUT,
                    data->set.expect_100_timeout, &ctx->start);
    *nread = 0;
    *eos = FALSE;
    প্রত্যাগম CURLE_OK;
  পক্ষ EXP100_FAILED:
    DEBUGF(infof(data, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    প্রত্যাগম CURLE_READ_ERROR;
  পক্ষ EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->start);
    যদি(ms < data->set.expect_100_timeout) {
      DEBUGF(infof(data, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      প্রত্যাগম CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(data, reader);
    infof(data, "Done waiting for 100-continue");
    FALLTHROUGH();
  স্বভাব:
    DEBUGF(infof(data, "cr_exp100_read, pass through"));
    প্রত্যাগম Curl_creader_read(data, reader->next, buf, blen, nread, eos);
  }
}

স্থির শূন্য cr_exp100_done(সংরচনা Curl_easy *data,
                           সংরচনা Curl_creader *reader, পূর্ণাঙ্ক premature)
{
  সংরচনা cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_DATA;
  Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
}

স্থির ধ্রুব সংরচনা Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_total_length,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_done,
  পরিমাণ(সংরচনা cr_exp100_ctx)
};

স্থির CURLcode http_exp100_add_reader(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *reader = রিক্ত;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_exp100, CURL_CR_PROTOCOL);
  যদি(!result)
    result = Curl_creader_add(data, reader);
  যদি(!result) {
    সংরচনা cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  যদি(result && reader)
    Curl_creader_free(data, reader);
  প্রত্যাগম result;
}

স্থির শূন্য http_exp100_got100(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  যদি(r)
    http_exp100_continue(data, r);
}

স্থির bool http_exp100_is_waiting(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  যদি(r) {
    সংরচনা cr_exp100_ctx *ctx = r->ctx;
    প্রত্যাগম ctx->state == EXP100_AWAITING_CONTINUE;
  }
  প্রত্যাগম FALSE;
}

স্থির শূন্য http_exp100_send_anyway(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  যদি(r)
    http_exp100_continue(data, r);
}

স্থির bool http_exp100_is_selected(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  প্রত্যাগম !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(সংরচনা Curl_easy *data,
                                 সংরচনা easy_pollset *ps)
{
  /* write mode */
  প্রত্যাগম Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(সংরচনা Curl_easy *data,
                                   সংরচনা easy_pollset *ps)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  যদি(CURL_REQ_WANT_RECV(data)) {
    result = Curl_pollset_add_in(data, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  যদি(!result && Curl_req_want_send(data) && !http_exp100_is_waiting(data)) {
    result = Curl_pollset_add_out(data, ps, conn->sock[FIRSTSOCKET]);
  }
  প্রত্যাগম result;
}

স্থির CURLcode http_write_header(সংরচনা Curl_easy *data,
                                  ধ্রুব বর্ণ *hd, size_t hdlen)
{
  CURLcode result;
  পূর্ণাঙ্ক writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((data->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  result = Curl_client_write(data, writetype, hd, hdlen);
  যদি(result)
    প্রত্যাগম result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  যদি(result)
    প্রত্যাগম result;

  data->req.deductheadercount = (100 <= data->req.httpcode &&
                                 199 >= data->req.httpcode) ?
    data->req.headerbytecount : 0;
  প্রত্যাগম result;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(সংরচনা Curl_easy *data,
                        CURLcode status, bool premature)
{
  সংরচনা connectdata *conn = data->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  data->state.authhost.multipass = FALSE;
  data->state.authproxy.multipass = FALSE;

  যদি(curlx_dyn_len(&data->state.headerb)) {
    (শূন্য)http_write_header(data, curlx_dyn_ptr(&data->state.headerb),
                            curlx_dyn_len(&data->state.headerb));
  }
  curlx_dyn_reset(&data->state.headerb);

  যদি(status)
    প্রত্যাগম status;

  যদি(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->bits.retry &&
     !data->set.connect_only &&
     (data->req.bytecount +
      data->req.headerbytecount -
      data->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(data, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    প্রত্যাগম CURLE_GOT_NOTHING;
  }

  প্রত্যাগম CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
স্থির bool http_may_use_1_1(ধ্রুব সংরচনা Curl_easy *data)
{
  ধ্রুব সংরচনা connectdata *conn = data->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  যদি(data->state.http_neg.rcvd_min == 10)
    প্রত্যাগম FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  যদি(conn && conn->httpversion_seen == 10)
    প্রত্যাগম FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  যদি(data->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    প্রত্যাগম FALSE;
  /* We are not restricted to use 1.0 only. */
  প্রত্যাগম !data->state.http_neg.only_10;
}

স্থির unsigned বর্ণ http_request_version(সংরচনা Curl_easy *data)
{
  unsigned বর্ণ v = Curl_conn_http_version(data, data->conn);
  যদি(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(data) ? 11 : 10;
  }
  প্রত্যাগম v;
}

স্থির ধ্রুব বর্ণ *get_http_string(পূর্ণাঙ্ক httpversion)
{
  বিকল্প(httpversion) {
  পক্ষ 30:
    প্রত্যাগম "3";
  পক্ষ 20:
    প্রত্যাগম "2";
  পক্ষ 11:
    প্রত্যাগম "1.1";
  স্বভাব:
    প্রত্যাগম "1.0";
  }
}

CURLcode Curl_add_custom_headers(সংরচনা Curl_easy *data,
                                 bool is_connect, পূর্ণাঙ্ক httpversion,
                                 সংরচনা dynbuf *req)
{
  সংরচনা curl_slist *h[2];
  সংরচনা curl_slist *headers;
  পূর্ণাঙ্ক numlists = 1; /* by default */
  পূর্ণাঙ্ক i;

#ifndef CURL_DISABLE_PROXY
  গণনা Curl_proxy_use proxy;

  যদি(is_connect)
    proxy = HEADER_CONNECT;
  অন্যথা
    proxy = data->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  বিকল্প(proxy) {
  পক্ষ HEADER_SERVER:
    h[0] = data->set.headers;
    ভঙ্গ;
  পক্ষ HEADER_PROXY:
    h[0] = data->set.headers;
    যদি(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    ভঙ্গ;
  পক্ষ HEADER_CONNECT:
    যদি(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    অন্যথা
      h[0] = data->set.headers;
    ভঙ্গ;
  পক্ষ HEADER_CONNECT_UDP:
    যদি(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    অন্যথা
      h[0] = data->set.headers;
    ভঙ্গ;
  }
#অন্যথা
  (শূন্য)is_connect;
  h[0] = data->set.headers;
#endif

  /* loop through one or two lists */
  প্রতি(i = 0; i < numlists; i++) {
    প্রতি(headers = h[i]; headers; headers = headers->next) {
      CURLcode result = CURLE_OK;
      bool blankheader = FALSE;
      সংরচনা Curl_str name;
      ধ্রুব বর্ণ *p = headers->data;
      ধ্রুব বর্ণ *origp = p;
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
          সংরচনা Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          যদি(!curlx_strlen(&val))
            /* no content, do not send this */
            অনুবর্ত;
        }
        অন্যথা
          /* no colon */
          অনুবর্ত;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      যদি(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      অন্যথা যদি(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      অন্যথা যদি(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      অন্যথা যদি(data->req.authneg &&
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
              !Curl_auth_allowed_to_host(data))
        ;
      অন্যথা যদি(blankheader) {
        result = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        যদি(!result)
          result = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      অন্যথা
        result = curlx_dyn_addf(req, "%s\r\n", origp);

      যদি(result)
        প্রত্যাগম result;
    }
  }

  প্রত্যাগম CURLE_OK;
}

#ifndef CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(সংরচনা Curl_easy *data,
                                সংরচনা dynbuf *req)
{
  ধ্রুব সংরচনা tm *tm;
  সংরচনা tm keeptime;
  CURLcode result;
  বর্ণ datestr[80];
  ধ্রুব বর্ণ *condp;
  size_t len;

  যদি(data->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    প্রত্যাগম CURLE_OK;

  result = curlx_gmtime(data->set.timevalue, &keeptime);
  যদি(result) {
    failf(data, "Invalid TIMEVALUE");
    প্রত্যাগম result;
  }
  tm = &keeptime;

  বিকল্প(data->set.timecondition) {
  স্বভাব:
    DEBUGF(infof(data, "invalid time condition"));
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  পক্ষ CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    ভঙ্গ;
  পক্ষ CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    ভঙ্গ;
  পক্ষ CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    ভঙ্গ;
  }

  যদি(Curl_checkheaders(data, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    প্রত্যাগম CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, পরিমাণ(datestr),
                 "%s: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                 condp,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);

  result = curlx_dyn_add(req, datestr);
  প্রত্যাগম result;
}
#অন্যথা
/* disabled */
CURLcode Curl_add_timecondition(সংরচনা Curl_easy *data,
                                সংরচনা dynbuf *req)
{
  (শূন্য)data;
  (শূন্য)req;
  প্রত্যাগম CURLE_OK;
}
#endif

শূন্য Curl_http_method(সংরচনা Curl_easy *data,
                      ধ্রুব বর্ণ **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)data->state.httpreq;
  ধ্রুব বর্ণ *request;
#ifndef CURL_DISABLE_WEBSOCKETS
  যদি(data->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  অন্যথা
#endif
  যদি((data->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     data->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  যদি(!data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
  }
  অন্যথা {
    যদি(data->req.no_body)
      request = "HEAD";
    অন্যথা {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_HEAD));
      বিকল্প(httpreq) {
      পক্ষ HTTPREQ_POST:
      পক্ষ HTTPREQ_POST_FORM:
      পক্ষ HTTPREQ_POST_MIME:
        request = "POST";
        ভঙ্গ;
      পক্ষ HTTPREQ_PUT:
        request = "PUT";
        ভঙ্গ;
      স্বভাব: /* this should never happen */
      পক্ষ HTTPREQ_GET:
        request = "GET";
        ভঙ্গ;
      পক্ষ HTTPREQ_HEAD:
        request = "HEAD";
        ভঙ্গ;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

স্থির CURLcode http_set_aptr_host(সংরচনা Curl_easy *data)
{
  সংরচনা connectdata *conn = data->conn;
  ধ্রুব বর্ণ *ptr = রিক্ত;

  curlx_safefree(data->state.http_host);
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(data->req.cookiehost);
#endif

  যদি(Curl_peer_equal(data->state.initial_origin, data->state.origin))
    ptr = Curl_checkheaders(data, STRCONST("Host"));

  যদি(ptr) {
#ifndef CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    বর্ণ *cookiehost;
    CURLcode result = copy_custom_মান(ptr, &cookiehost);
    যদি(result)
      প্রত্যাগম result;
    যদি(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    অন্যথা {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      যদি(*cookiehost == '[') {
        বর্ণ *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, strlen(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        যদি(closingbracket)
          *closingbracket = 0;
      }
      অন্যথা {
        পূর্ণাঙ্ক startsearch = 0;
        বর্ণ *colon = strchr(cookiehost + startsearch, ':');
        যদি(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      data->req.cookiehost = cookiehost;
    }
#endif

    যদি(!curl_strequal("Host:", ptr)) {
      data->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      যদি(!data->state.http_host)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
  }
  অন্যথা {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)data->state.origin->ipv6;
    সংরচনা dynbuf tmp;
    size_t hlen;
    CURLcode result;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    result = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    যদি(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("["));
    যদি(!result)
      result = curlx_dyn_add(&tmp, data->state.origin->hostname);
    যদি(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("]"));
    যদি(!result &&
       ((data->state.origin->port != data->state.origin->scheme->defport) ||
       (data->state.origin->scheme->family != conn->scheme->family))) {
      result = curlx_dyn_addf(&tmp, ":%u", data->state.origin->port);
    }

    data->state.http_host = result ? রিক্ত : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    প্রত্যাগম result;
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
স্থির CURLcode http_target(সংরচনা Curl_easy *data,
                            সংরচনা dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ধ্রুব বর্ণ *path = data->state.up.path;
  ধ্রুব বর্ণ *query = data->state.up.query;
#ifndef CURL_DISABLE_PROXY
  সংরচনা connectdata *conn = data->conn;
#endif

  যদি(CURL_EASY_STR(data, STRING_TARGET)) {
    path = CURL_EASY_STR(data, STRING_TARGET);
    query = রিক্ত;
  }

#ifndef CURL_DISABLE_PROXY
  যদি(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    বর্ণ *url;
    CURLU *h = curl_url_dup(data->state.uh);
    যদি(!h)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;

    যদি(!data->state.origin->ipv6 &&
       (data->state.origin->user_hostname != data->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, data->state.origin->hostname, 0);
      যদি(uc) {
        curl_url_cleanup(h);
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, রিক্ত, 0);
    যদি(uc) {
      curl_url_cleanup(h);
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }

    যদি(data->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, রিক্ত, 0);
      যদি(uc) {
        curl_url_cleanup(h);
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, রিক্ত, 0);
      যদি(uc) {
        curl_url_cleanup(h);
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      }
    }
    অন্যথা যদি(data->state.creds && (data->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        data->state.creds->user, CURLU_URLENCODE);
      যদি(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          data->state.creds->passwd, CURLU_URLENCODE);
      যদি(uc) {
        curl_url_cleanup(h);
        প্রত্যাগম Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    যদি(uc) {
      curl_url_cleanup(h);
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    result = curlx_dyn_add(r, CURL_EASY_STR(data, STRING_TARGET) ?
      CURL_EASY_STR(data, STRING_TARGET) : url);
    curlx_free(url);
    যদি(result)
      প্রত্যাগম result;

    যদি((data->state.origin->scheme == &Curl_scheme_ftp) &&
       data->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = strlen(path);
      bool type_present = FALSE;
      যদি((len >= 7) && !memcmp(&path[len - 7], ";type=", 6)) {
        বিকল্প(Curl_raw_toupper(path[len - 1])) {
        পক্ষ 'A':
        পক্ষ 'D':
        পক্ষ 'I':
          type_present = TRUE;
          ভঙ্গ;
        }
      }
      যদি(!type_present) {
        result = curlx_dyn_addf(r, ";type=%c",
                                data->state.prefer_ascii ? 'a' : 'i');
        যদি(result)
          প্রত্যাগম result;
      }
    }
  }

  অন্যথা
#endif
  {
    result = curlx_dyn_add(r, path);
    যদি(result)
      প্রত্যাগম result;
    যদি(query)
      result = curlx_dyn_addf(r, "?%s", query);
  }

  প্রত্যাগম result;
}

#যদি !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
স্থির CURLcode set_post_reader(সংরচনা Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result;

  বিকল্প(httpreq) {
#ifndef CURL_DISABLE_MIME
  পক্ষ HTTPREQ_POST_MIME:
    data->state.mimepost = data->set.mimepostp;
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_FORM_API
  পক্ষ HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    যদি(!data->state.formp) {
      data->state.formp = curlx_calloc(1, পরিমাণ(curl_mimepart));
      যদি(!data->state.formp)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(data->state.formp);
      result = Curl_getformdata(data, data->state.formp, data->set.httppost,
                                data->state.fread_func);
      যদি(result) {
        curlx_safefree(data->state.formp);
        প্রত্যাগম result;
      }
      data->state.mimepost = data->state.formp;
    }
    ভঙ্গ;
#endif
  স্বভাব:
    data->state.mimepost = রিক্ত;
    ভঙ্গ;
  }

  বিকল্প(httpreq) {
  পক্ষ HTTPREQ_POST_FORM:
  পক্ষ HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ifndef CURL_DISABLE_MIME
    যদি(data->state.mimepost) {
      ধ্রুব বর্ণ *cthdr = Curl_checkheaders(data, STRCONST("Content-Type"));

      /* Read and seek body only. */
      data->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      যদি(cthdr)
        প্রতি(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      অন্যথা যদি(data->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(data->state.mimepost, data->set.headers, 0);
      result = Curl_mime_prepare_headers(data, data->state.mimepost, cthdr,
                                         রিক্ত, MIMESTRATEGY_FORM);
      যদি(result)
        প্রত্যাগম result;
      curl_mime_headers(data->state.mimepost, রিক্ত, 0);
      result = Curl_creader_set_mime(data, data->state.mimepost);
      যদি(result)
        প্রত্যাগম result;
    }
    অন্যথা
#endif
    {
      result = Curl_creader_set_null(data);
    }
    data->state.infilesize = Curl_creader_total_length(data);
    প্রত্যাগম result;

  স্বভাব:
    প্রত্যাগম Curl_creader_set_null(data);
  }
  /* never reached */
}
#endif

স্থির CURLcode set_reader(সংরচনা Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t postsize = data->state.infilesize;

  DEBUGASSERT(data->conn);

  যদি(data->req.authneg) {
    প্রত্যাগম Curl_creader_set_null(data);
  }

  বিকল্প(httpreq) {
  পক্ষ HTTPREQ_PUT: /* Let's PUT the data to the server! */
    প্রত্যাগম postsize ? Curl_creader_set_fread(data, postsize) :
      Curl_creader_set_null(data);

#যদি !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  পক্ষ HTTPREQ_POST_FORM:
  পক্ষ HTTPREQ_POST_MIME:
    প্রত্যাগম set_post_reader(data, httpreq);
#endif

  পক্ষ HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    যদি(!postsize) {
      result = Curl_creader_set_null(data);
    }
    অন্যথা যদি(data->set.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      যদি(plen == SIZE_MAX)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      অন্যথা যদি(plen)
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      অন্যথা
        result = Curl_creader_set_null(data);
    }
    অন্যথা {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      বর্ণ *ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
      যদি(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      result = Curl_creader_set_fread(data, chunked ? -1 : postsize);
    }
    প্রত্যাগম result;

  স্বভাব:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    data->state.infilesize = 0;
    প্রত্যাগম Curl_creader_set_null(data);
  }
  /* not reached */
}

স্থির CURLcode http_resume(সংরচনা Curl_easy *data, Curl_HttpReq httpreq)
{
  যদি((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     data->state.resume_from) {
    failf(data, "HTTP upload cannot be resumed");
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode http_req_set_TE(সংরচনা Curl_easy *data,
                                সংরচনা dynbuf *req,
                                পূর্ণাঙ্ক httpversion)
{
  CURLcode result = CURLE_OK;
  ধ্রুব বর্ণ *ptr;

  ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
  যদি(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    data->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    যদি(data->req.upload_chunky && (httpversion >= 20)) {
      infof(data, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      data->req.upload_chunky = FALSE;
    }
  }
  অন্যথা {
    curl_off_t req_clen = Curl_creader_total_length(data);

    যদি(req_clen < 0) {
      /* indeterminate request content length */
      যদি(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        data->req.upload_chunky = (httpversion < 20);
      }
      অন্যথা {
        failf(data, "Chunky upload is not supported by HTTP 1.0");
        প্রত্যাগম CURLE_UPLOAD_FAILED;
      }
    }
    অন্যথা {
      /* else, no chunky upload */
      data->req.upload_chunky = FALSE;
    }

    যদি(data->req.upload_chunky)
      result = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  প্রত্যাগম result;
}

স্থির CURLcode addexpect(সংরচনা Curl_easy *data, সংরচনা dynbuf *r,
                          পূর্ণাঙ্ক httpversion, bool *announced_exp100)
{
  CURLcode result;
  বর্ণ *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  যদি(data->req.upgr101 != UPGR101_NONE)
    প্রত্যাগম CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(data, STRCONST("Expect"));
  যদি(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  অন্যথা যদি(!data->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_length(data);
    যদি(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      result = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      যদি(result)
        প্রত্যাগম result;
      *announced_exp100 = TRUE;
    }
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode http_add_content_hds(সংরচনা Curl_easy *data,
                                     সংরচনা dynbuf *r,
                                     পূর্ণাঙ্ক httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(data->conn);
  যদি(data->req.upload_chunky) {
    result = Curl_httpchunk_add_reader(data);
    যদি(result)
      প্রত্যাগম result;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_total_length(data);
  বিকল্প(httpreq) {
  পক্ষ HTTPREQ_PUT:
  পক্ষ HTTPREQ_POST:
#যদি !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  পক্ষ HTTPREQ_POST_FORM:
  পক্ষ HTTPREQ_POST_MIME:
#endif
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    যদি(req_clen >= 0 && !data->req.upload_chunky &&
       (data->req.authneg ||
        !Curl_checkheaders(data, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      result = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    যদি(result)
      গচ্ছ out;

#ifndef CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    যদি(data->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      সংরচনা curl_slist *hdr;

      প্রতি(hdr = data->state.mimepost->curlheaders; hdr; hdr = hdr->next) {
        result = curlx_dyn_addf(r, "%s\r\n", hdr->data);
        যদি(result)
          গচ্ছ out;
      }
    }
#endif
    যদি(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(data, STRCONST("Content-Type"))) {
      result = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      যদি(result)
        গচ্ছ out;
    }
    result = addexpect(data, r, httpversion, &announced_exp100);
    যদি(result)
      গচ্ছ out;
    ভঙ্গ;
  স্বভাব:
    ভঙ্গ;
  }

  Curl_pgrsSetUploadSize(data, req_clen);
  যদি(announced_exp100)
    result = http_exp100_add_reader(data);

out:
  প্রত্যাগম result;
}

#ifndef CURL_DISABLE_COOKIES

স্থির CURLcode http_cookies(সংরচনা Curl_easy *data,
                             সংরচনা dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ধ্রুব বর্ণ *addcookies = রিক্ত;
  bool linecap = FALSE;
  যদি(CURL_EASY_STR(data, STRING_COOKIE) &&
     !Curl_checkheaders(data, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(data))
    addcookies = CURL_EASY_STR(data, STRING_COOKIE);

  যদি(data->cookies || addcookies) {
    সংরচনা Curl_llist list;
    পূর্ণাঙ্ক গুনতি = 0;

    যদি(data->cookies && data->state.cookie_engine) {
      bool okay;
      ধ্রুব বর্ণ *host = data->req.cookiehost ?
        data->req.cookiehost : data->state.origin->hostname;
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      result = Curl_cookie_getlist(data, &okay, host, &list);
      যদি(!result && okay) {
        সংরচনা Curl_llist_গ্রন্থি *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        প্রতি(n = Curl_llist_head(&list); n; n = Curl_গ্রন্থি_next(n)) {
          সংরচনা Cookie *co = Curl_গ্রন্থি_elem(n);
          যদি(co->মান) {
            size_t add;
            যদি(!গুনতি) {
              result = curlx_dyn_addn(r, STRCONST("Cookie: "));
              যদি(result)
                ভঙ্গ;
            }
            add = strlen(co->name) + strlen(co->মান) + 1;
            যদি(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(data, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              ভঙ্গ;
            }
            result = curlx_dyn_addf(r, "%s%s=%s", গুনতি ? "; " : "",
                                    co->name, co->মান);
            যদি(result)
              ভঙ্গ;
            clen += add + (গুনতি ? 2 : 0);
            গুনতি++;
          }
        }
        Curl_llist_destroy(&list, রিক্ত);
      }
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
    যদি(addcookies && !result && !linecap) {
      যদি(!গুনতি)
        result = curlx_dyn_addn(r, STRCONST("Cookie: "));
      যদি(!result) {
        result = curlx_dyn_addf(r, "%s%s", গুনতি ? "; " : "", addcookies);
        গুনতি++;
      }
    }
    যদি(গুনতি && !result)
      result = curlx_dyn_addn(r, STRCONST("\r\n"));

    যদি(result)
      প্রত্যাগম result;
  }
  প্রত্যাগম result;
}
#অন্যথা
#define http_cookies(a, b) CURLE_OK
#endif

স্থির CURLcode http_range(সংরচনা Curl_easy *data,
                           Curl_HttpReq httpreq)
{
  যদি(data->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    যদি(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_HEAD)) &&
       !Curl_checkheaders(data, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);
      data->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 data->state.range);
      যদি(!data->state.rangeline)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
    অন্যথা যদি((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(data, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_total_length(data);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);

      যদি(data->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      অন্যথা যদি(data->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t total_len = data->req.authneg ?
                               data->state.infilesize :
                               (data->state.resume_from + req_clen);
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        data->state.range, total_len - 1, total_len);
      }
      অন্যথা {
        /* Range was selected and then we pass the incoming range and append
           total size */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        data->state.range, req_clen);
      }
      যদি(!data->state.rangeline)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode http_firstwrite(সংরচনা Curl_easy *data)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা SingleRequest *k = &data->req;

  যদি(data->req.newurl) {
    যদি(conn->bits.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      প্রত্যাগম CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(data, "Ignoring the response-body");
  }
  যদি(data->state.resume_from && !k->content_range &&
     (data->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    যদি(k->size == data->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(data, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      প্রত্যাগম CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(data, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    প্রত্যাগম CURLE_RANGE_ERROR;
  }

  যদি(data->set.timecondition && !data->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(data, k->timeofdoc)) {
    k->done = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    data->info.httpcode = 304;
    infof(data, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    প্রত্যাগম CURLE_OK;
  } /* we have a time condition */

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode http_check_new_conn(সংরচনা Curl_easy *data)
{
  সংরচনা connectdata *conn = data->conn;
  ধ্রুব বর্ণ *info_version = রিক্ত;
  ধ্রুব বর্ণ *alpn;
  CURLcode result;

  alpn = Curl_conn_get_alpn_negotiated(data, conn);
  যদি(alpn && !strcmp("h3", alpn)) {
#ifndef CURL_DISABLE_PROXY
    যদি(!conn->bits.origin_is_proxy)
#endif
      DEBUGASSERT(Curl_conn_http_version(data, conn) == 30);
    info_version = "HTTP/3";
  }
  অন্যথা যদি(alpn && !strcmp("h2", alpn)) {
#ifndef CURL_DISABLE_PROXY
    যদি((Curl_conn_http_version(data, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      result = Curl_http2_switch(data);
      যদি(result)
        প্রত্যাগম result;
    }
    অন্যথা
#endif
    DEBUGASSERT(Curl_conn_http_version(data, conn) == 20);
    info_version = "HTTP/2";
  }
  অন্যথা {
    /* Check if user wants to use HTTP/2 with clear TCP */
    যদি(Curl_http2_may_switch(data)) {
      DEBUGF(infof(data, "HTTP/2 over clean TCP"));
      result = Curl_http2_switch(data);
      যদি(result)
        প্রত্যাগম result;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    অন্যথা
      info_version = "HTTP/1.x";
  }

  যদি(info_version)
    infof(data, "using %s", info_version);
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode http_add_connection_hd(সংরচনা Curl_easy *data,
                                       সংরচনা dynbuf *req)
{
  সংরচনা curl_slist *head;
  ধ্রুব বর্ণ *sep = "Connection: ";
  CURLcode result = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  প্রতি(head = data->set.headers; head; head = head->next) {
    যদি(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      বর্ণ *মান;
      result = copy_custom_মান(head->data, &মান);
      যদি(result)
        প্রত্যাগম result;
      result = curlx_dyn_addf(req, "%s%s", sep, মান);
      sep = ", ";
      curlx_free(মান);
      ভঙ্গ; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  যদি(!result && data->state.http_hd_te) {
    result = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  যদি(!result && data->state.http_hd_upgrade) {
    result = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  যদি(!result && data->state.http_hd_h2_settings) {
    result = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  যদি(!result && (rlen < curlx_dyn_len(req)))
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
  যদি(result)
    প্রত্যাগম result;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  প্রতি(head = data->set.headers; head; head = head->next) {
    যদি(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      যদি(skip) {
        skip = FALSE;
        অনুবর্ত;
      }
      result = curlx_dyn_addf(req, "%s\r\n", head->data);
      যদি(result)
        প্রত্যাগম result;
    }
  }

  প্রত্যাগম CURLE_OK;
}

/* Header identifier in order we send them by default */
প্রকারনির্দেশ গণনা {
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

স্থির CURLcode http_add_hd(সংরচনা Curl_easy *data,
                            সংরচনা dynbuf *req,
                            http_hd_t id,
                            unsigned বর্ণ httpversion,
                            ধ্রুব বর্ণ *method,
                            Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
#যদি !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  সংরচনা connectdata *conn = data->conn;
#endif
  বিকল্প(id) {
  পক্ষ H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    result = curlx_dyn_addf(req, "%s ", method);
    যদি(!result)
      result = http_target(data, req);
    যদি(!result)
      result = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    ভঙ্গ;

  পক্ষ H1_HD_HOST:
    যদি(data->state.http_host) {
      result = curlx_dyn_add(req, data->state.http_host);
      যদি(!result)
        result = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    ভঙ্গ;

#ifndef CURL_DISABLE_PROXY
  পক্ষ H1_HD_PROXY_AUTH:
    যদি(data->req.hd_proxy_auth)
      result = curlx_dyn_add(req, data->req.hd_proxy_auth);
    ভঙ্গ;
#endif

  পক্ষ H1_HD_AUTH:
    যদি(data->req.hd_auth)
      result = curlx_dyn_add(req, data->req.hd_auth);
    ভঙ্গ;

  পক্ষ H1_HD_RANGE:
    যদি(data->state.use_range && data->state.rangeline)
      result = curlx_dyn_add(req, data->state.rangeline);
    ভঙ্গ;

  পক্ষ H1_HD_USER_AGENT: {
    ধ্রুব বর্ণ *ua = CURL_EASY_STR(data, STRING_USERAGENT);
    যদি(ua && *ua && !Curl_checkheaders(data, STRCONST("User-Agent")))
      result = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    ভঙ্গ;
  }

  পক্ষ H1_HD_ACCEPT:
    যদি(!Curl_checkheaders(data, STRCONST("Accept")))
      result = curlx_dyn_add(req, "Accept: */*\r\n");
    ভঙ্গ;

  পক্ষ H1_HD_TE:
#ifdef HAVE_LIBZ
    যদি(!Curl_checkheaders(data, STRCONST("TE")) &&
       data->set.http_transfer_encoding) {
      data->state.http_hd_te = TRUE;
      result = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#endif
    ভঙ্গ;

  পক্ষ H1_HD_ACCEPT_ENCODING: {
    ধ্রুব বর্ণ *enc = CURL_EASY_STR(data, STRING_ENCODING);
    যদি(enc && !Curl_checkheaders(data, STRCONST("Accept-Encoding")))
      result = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    ভঙ্গ;
  }

  পক্ষ H1_HD_REFERER:
    যদি(Curl_bufref_ptr(&data->state.referer) &&
       !Curl_checkheaders(data, STRCONST("Referer")))
      result = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&data->state.referer));
    ভঙ্গ;

#ifndef CURL_DISABLE_PROXY
  পক্ষ H1_HD_PROXY_CONNECTION:
    যদি(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(data, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(data, data->conn, STRCONST("Proxy-Connection")))
      result = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    ভঙ্গ;
#endif

  পক্ষ H1_HD_TRANSFER_ENCODING:
    result = http_req_set_TE(data, req, httpversion);
    ভঙ্গ;

#ifndef CURL_DISABLE_ALTSVC
  পক্ষ H1_HD_ALT_USED:
    যদি(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(data, STRCONST("Alt-Used")))
      result = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    ভঙ্গ;
#endif

  পক্ষ H1_HD_UPGRADE:
    যদি(!Curl_conn_is_ssl(data->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
       data->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      result = Curl_http2_request_upgrade(req, data);
    }
#ifndef CURL_DISABLE_WEBSOCKETS
    যদি(!result && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      result = Curl_ws_request(data, req);
#endif
    ভঙ্গ;

  পক্ষ H1_HD_COOKIES:
    result = http_cookies(data, req);
    ভঙ্গ;

  পক্ষ H1_HD_CONDITIONALS:
    result = Curl_add_timecondition(data, req);
    ভঙ্গ;

  পক্ষ H1_HD_CUSTOM:
    result = Curl_add_custom_headers(data, FALSE, httpversion, req);
    ভঙ্গ;

  পক্ষ H1_HD_CONTENT:
    result = http_add_content_hds(data, req, httpversion, httpreq);
    ভঙ্গ;

  পক্ষ H1_HD_CONNECTION: {
    result = http_add_connection_hd(data, req);
    ভঙ্গ;
  }

  পক্ষ H1_HD_LAST:
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
    ভঙ্গ;
  }
  প্রত্যাগম result;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  Curl_HttpReq httpreq;
  ধ্রুব বর্ণ *method;
  সংরচনা dynbuf req;
  unsigned বর্ণ httpversion;
  size_t hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *done = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&data->state.headerb);
  data->state.maybe_folded = FALSE;

  যদি(!data->conn->bits.reuse) {
    result = http_check_new_conn(data);
    যদি(result)
      গচ্ছ out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  result = Curl_headers_init(data);
  যদি(result)
    গচ্ছ out;

  data->state.http_hd_te = FALSE;
  data->state.http_hd_upgrade = FALSE;
  data->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(data, &method, &httpreq);

  /* select host to send */
  result = http_set_aptr_host(data);
  /* setup the authentication headers, how that method and host are known */
  যদি(!result)
    result = Curl_http_output_auth(data, data->conn, method, httpreq,
                                   data->state.up.path,
                                   data->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  যদি(!result)
    result = set_reader(data, httpreq);
  যদি(!result)
    result = http_resume(data, httpreq);
  যদি(!result)
    result = http_range(data, httpreq);
  যদি(result)
    গচ্ছ out;

  httpversion = http_request_version(data);
  /* Add request line and all headers to `req` */
  প্রতি(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    result = http_add_hd(data, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    যদি(result)
      গচ্ছ out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);
  result = Curl_req_send(data, &req, httpversion);

  যদি((httpversion >= 20) && data->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    data->req.upload_chunky = FALSE;

out:
  যদি(result == CURLE_TOO_LARGE)
    failf(data, "HTTP request too large");

  curlx_dyn_free(&req);
  প্রত্যাগম result;
}

প্রকারনির্দেশ গণনা {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
স্থির bool checkprefixmax(ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ *buffer, size_t len)
{
  size_t ch = CURLMIN(strlen(prefix), len);
  প্রত্যাগম curl_strnequal(prefix, buffer, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
স্থির statusline checkhttpprefix(সংরচনা Curl_easy *data,
                                  ধ্রুব বর্ণ *s, size_t len)
{
  সংরচনা curl_slist *head = data->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  যাবত্(head) {
    যদি(checkprefixmax(head->data, s, len)) {
      rc = onmatch;
      ভঙ্গ;
    }
    head = head->next;
  }

  যদি((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  প্রত্যাগম rc;
}

#ifndef CURL_DISABLE_RTSP
স্থির statusline checkrtspprefix(সংরচনা Curl_easy *data,
                                  ধ্রুব বর্ণ *s, size_t len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (শূন্য)data;
  যদি(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  প্রত্যাগম status;
}
#endif /* CURL_DISABLE_RTSP */

স্থির statusline checkprotoprefix(সংরচনা Curl_easy *data,
                                   সংরচনা connectdata *conn,
                                   ধ্রুব বর্ণ *s, size_t len)
{
#ifndef CURL_DISABLE_RTSP
  যদি(conn->scheme->protocol & CURLPROTO_RTSP)
    প্রত্যাগম checkrtspprefix(data, s, len);
#অন্যথা
  (শূন্য)conn;
#endif /* CURL_DISABLE_RTSP */

  প্রত্যাগম checkhttpprefix(data, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#define HD_IS(hd, hdlen, n) \
  (((hdlen) >= (পরিমাণ(n) - 1)) && curl_strnequal(n, hd, পরিমাণ(n) - 1))

#define HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (পরিমাণ(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, পরিমাণ(n) - 1)) ? ((hd) + (পরিমাণ(n) - 1)) : রিক্ত)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#define HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((পরিমাণ(n) - 1) + (পরিমাণ(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
স্থির CURLcode http_header_a(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *hd, size_t hdlen)
{
#ifndef CURL_DISABLE_ALTSVC
  ধ্রুব বর্ণ *v;
  v = (data->asi &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#অন্যথা
        0
#endif
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : রিক্ত;
  যদি(v) {
    /* the ALPN of the current request */
    সংরচনা SingleRequest *k = &data->req;
    গণনা alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    প্রত্যাগম Curl_altsvc_parse(data, data->asi, v, data->state.origin, id);
  }
#অন্যথা
  (শূন্য)data;
  (শূন্য)hd;
  (শূন্য)hdlen;
#endif
  প্রত্যাগম CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
স্থির CURLcode http_header_c(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *hd, size_t hdlen)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা SingleRequest *k = &data->req;
  ধ্রুব বর্ণ *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !data->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : রিক্ত;
  যদি(v) {
    কুরু {
      curl_off_t contentlength;
      পূর্ণাঙ্ক offt = curlx_str_numblanks(&v, &contentlength);

      যদি(offt == STRE_OVERFLOW) {
        /* out of range */
        যদি(data->set.max_filesize) {
          failf(data, "Maximum file size exceeded");
          প্রত্যাগম CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(data, "Overflow Content-Length: value");
        প্রত্যাগম CURLE_OK;
      }
      অন্যথা {
        যদি((offt == STRE_OK) &&
           ((k->size == -1) || /* not set to something before */
            (k->size == contentlength))) { /* or the same value */

          k->size = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          যদি(!curlx_str_single(&v, ','))
            অনুবর্ত;

          যদি(!curlx_str_newline(&v)) {
            k->maxdownload = k->size;
            প্রত্যাগম CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(data, "Invalid Content-Length: value");
        প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
      }
    } যাবত্(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(data, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : রিক্ত;
  যদি(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    প্রত্যাগম Curl_build_unencoding_স্তূপ(data, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  যদি(v) {
    বর্ণ *contenttype = Curl_copy_header_মান(hd);
    যদি(!contenttype)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    যদি(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    অন্যথা {
      curlx_free(data->info.contenttype);
      data->info.contenttype = contenttype;
    }
    প্রত্যাগম CURLE_OK;
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
    প্রত্যাগম CURLE_OK;
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
    infof(data, "HTTP/1.0 connection set to keep alive");
    প্রত্যাগম CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : রিক্ত;
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

    ধ্রুব বর্ণ *ptr = v;

    /* Move forward until first digit or asterisk */
    যাবত্(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    যদি(ISDIGIT(*ptr)) {
      যদি(!curlx_str_সংখ্যা(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (data->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    অন্যথা যদি(k->httpcode < 300)
      data->state.resume_from = 0; /* get everything */
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
স্থির CURLcode http_header_l(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *hd, size_t hdlen)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা SingleRequest *k = &data->req;
  ধ্রুব বর্ণ *v = (!k->http_bodyless &&
                   (data->set.timecondition || data->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : রিক্ত;
  যদি(v) {
    যদি(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    যদি(data->set.get_filetime)
      data->info.filetime = k->timeofdoc;
    প্রত্যাগম CURLE_OK;
  }
  যদি(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    বর্ণ *location = Curl_copy_header_মান(hd);
    যদি(!location)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    যদি(!*location ||
       (data->req.location && !strcmp(data->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      প্রত্যাগম CURLE_OK;
    }
    অন্যথা {
      /* has value and is not an exact repeat */
      যদি(data->req.location) {
        failf(data, "Multiple Location headers");
        curlx_free(location);
        প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
      }
      data->req.location = location;

      যদি((k->httpcode >= 300 && k->httpcode < 400) &&
         data->set.http_follow_mode) {
        CURLcode result;
        DEBUGASSERT(!data->req.newurl);
        data->req.newurl = curlx_strdup(data->req.location); /* clone */
        যদি(!data->req.newurl)
          প্রত্যাগম CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        result = http_perhapsrewind(data, conn);
        যদি(result)
          প্রত্যাগম result;

        /* mark the next request as a followed location: */
        data->state.this_is_a_follow = TRUE;
      }
    }
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
স্থির CURLcode http_header_p(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *hd, size_t hdlen)
{
  সংরচনা SingleRequest *k = &data->req;

#ifndef CURL_DISABLE_PROXY
  ধ্রুব বর্ণ *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  যদি(v) {
    সংরচনা connectdata *conn = data->conn;
    যদি((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(data, "HTTP/1.0 proxy connection set to keep alive");
    }
    অন্যথা যদি((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(data, "HTTP/1.1 proxy connection set close");
    }
    প্রত্যাগম CURLE_OK;
  }
#endif
  যদি((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    বর্ণ *auth;
    CURLcode result;
#ifndef CURL_DISABLE_PROXY
    যদি(!data->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      প্রত্যাগম CURLE_OK;
#endif
    auth = Curl_copy_header_মান(hd);
    result = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    যদি(!result) {
      result = Curl_http_প্রবেশ_auth(data, TRUE, auth);
      curlx_free(auth);
    }
    প্রত্যাগম result;
  }
#ifdef USE_SPNEGO
  যদি(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    সংরচনা connectdata *conn = data->conn;
    সংরচনা negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    সংরচনা auth *authp = &data->state.authhost;
    যদি(!negdata)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    যদি(authp->picked == CURLAUTH_NEGOTIATE) {
      বর্ণ *persistentauth = Curl_copy_header_মান(hd);
      যদি(!persistentauth)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(data, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#endif
  প্রত্যাগম CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
স্থির CURLcode http_header_r(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *hd, size_t hdlen)
{
  ধ্রুব বর্ণ *v = HD_VAL(hd, hdlen, "Retry-After:");
  যদি(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    যদি(!Curl_getdate_capped(v, &date)) {
      time_t current = time(রিক্ত);
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
    data->info.retry_after = retry_after;
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
স্থির CURLcode http_header_s(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *hd, size_t hdlen)
{
#যদি !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ধ্রুব বর্ণ *v;
#অন্যথা
  (শূন্য)data;
  (শূন্য)hd;
  (শূন্য)hdlen;
#endif

#ifndef CURL_DISABLE_COOKIES
  v = (data->cookies && data->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : রিক্ত;
  যদি(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ধ্রুব বর্ণ *host = data->req.cookiehost ?
      data->req.cookiehost : data->state.origin->hostname;
    ধ্রুব unsigned বর্ণ secure_context = Curl_secure_context(data, host) ?
      COOKIE_SECURE : 0;
    CURLcode result;
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    result = Curl_cookie_add(data, data->cookies, v, host,
                             data->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    প্রত্যাগম result;
  }
#endif
#ifndef CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (data->hsts &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#অন্যথা
        0
#endif
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : রিক্ত;
  যদি(v) {
    CURLcode result = Curl_hsts_parse(
      data->hsts, data->state.origin->hostname, v);
    যদি(result) {
      যদি(result == CURLE_OUT_OF_MEMORY)
        প্রত্যাগম result;
      infof(data, "Illegal STS header skipped");
    }
#ifdef DEBUGBUILD
    অন্যথা
      infof(data, "Parsed STS header fine (%zu entries)",
            Curl_llist_গুনতি(&data->hsts->list));
#endif
  }
#endif

  প্রত্যাগম CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
স্থির CURLcode http_header_t(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *hd, size_t hdlen)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা SingleRequest *k = &data->req;

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
  ধ্রুব বর্ণ *v = (!k->http_bodyless &&
                   (data->state.httpreq != HTTPREQ_HEAD) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : রিক্ত;
  যদি(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode result = Curl_build_unencoding_স্তূপ(data, v, TRUE);
    যদি(result)
      প্রত্যাগম result;
    যদি(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(data, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    প্রত্যাগম CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  যদি(v) {
    data->req.resp_trailer = TRUE;
    প্রত্যাগম CURLE_OK;
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
স্থির CURLcode http_header_w(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *hd, size_t hdlen)
{
  সংরচনা SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  যদি((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    বর্ণ *auth = Curl_copy_header_মান(hd);
    যদি(!auth)
      result = CURLE_OUT_OF_MEMORY;
    অন্যথা {
      result = Curl_http_প্রবেশ_auth(data, FALSE, auth);
      curlx_free(auth);
    }
  }
  প্রত্যাগম result;
}

/*
 * http_header() parses a single response header.
 */
স্থির CURLcode http_header(সংরচনা Curl_easy *data,
                            ধ্রুব বর্ণ *hd, size_t hdlen)
{
  CURLcode result = CURLE_OK;

  বিকল্প(hd[0]) {
  পক্ষ 'a':
  পক্ষ 'A':
    result = http_header_a(data, hd, hdlen);
    ভঙ্গ;
  পক্ষ 'c':
  পক্ষ 'C':
    result = http_header_c(data, hd, hdlen);
    ভঙ্গ;
  পক্ষ 'l':
  পক্ষ 'L':
    result = http_header_l(data, hd, hdlen);
    ভঙ্গ;
  পক্ষ 'p':
  পক্ষ 'P':
    result = http_header_p(data, hd, hdlen);
    ভঙ্গ;
  পক্ষ 'r':
  পক্ষ 'R':
    result = http_header_r(data, hd, hdlen);
    ভঙ্গ;
  পক্ষ 's':
  পক্ষ 'S':
    result = http_header_s(data, hd, hdlen);
    ভঙ্গ;
  পক্ষ 't':
  পক্ষ 'T':
    result = http_header_t(data, hd, hdlen);
    ভঙ্গ;
  পক্ষ 'w':
  পক্ষ 'W':
    result = http_header_w(data, hd, hdlen);
    ভঙ্গ;
  }

  যদি(!result) {
    সংরচনা connectdata *conn = data->conn;
    যদি(conn->scheme->protocol & CURLPROTO_RTSP)
      result = Curl_rtsp_parseheader(data, hd);
  }
  প্রত্যাগম result;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
স্থির CURLcode http_statusline(সংরচনা Curl_easy *data,
                                সংরচনা connectdata *conn)
{
  সংরচনা SingleRequest *k = &data->req;

  বিকল্প(k->httpversion) {
  পক্ষ 10:
  পক্ষ 11:
#ifdef USE_HTTP2
  পক্ষ 20:
#endif
#ifdef USE_HTTP3
  পক্ষ 30:
#endif
    /* no major version switch mid-connection */
    যদি(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(data, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
    }
    ভঙ্গ;
  স্বভাব:
    failf(data, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    প্রত্যাগম CURLE_UNSUPPORTED_PROTOCOL;
  }

  data->info.httpcode = k->httpcode;
  data->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  যদি(!data->state.http_neg.rcvd_min ||
     data->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    data->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  যদি(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  যদি(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(data, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  বিকল্প(k->httpcode) {
  পক্ষ 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    যদি(data->set.timecondition)
      data->info.timecond = TRUE;
    FALLTHROUGH();
  পক্ষ 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->size = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    ভঙ্গ;
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
স্থির CURLcode http_size(সংরচনা Curl_easy *data)
{
  সংরচনা SingleRequest *k = &data->req;
  যদি(data->req.ignore_cl || k->chunk) {
    k->size = k->maxdownload = -1;
  }
  অন্যথা যদি(k->size != -1) {
    যদি(data->set.max_filesize &&
       !k->ignorebody &&
       (k->size > data->set.max_filesize)) {
      failf(data, "Maximum file size exceeded");
      প্রত্যাগম CURLE_FILESIZE_EXCEEDED;
    }
    যদি(k->ignorebody)
      infof(data, "setting size while ignoring");
    Curl_pgrsSetDownloadSize(data, k->size);
    k->maxdownload = k->size;
  }
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_verify_header(সংরচনা Curl_easy *data,
                            ধ্রুব বর্ণ *hd, size_t hdlen)
{
  সংরচনা SingleRequest *k = &data->req;
  size_t i;
  bool has_colon = FALSE;
  প্রতি(i = 0; i < hdlen; i++) {
    বিকল্প(hd[i]) {
    পক্ষ '\0':
      /* this is bad, bail out */
      failf(data, "NUL byte in header");
      প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
    পক্ষ '\r':
      যদি(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(data, "Carriage return found in header");
        প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
      }
      ভঙ্গ;
    পক্ষ ':':
      has_colon = TRUE;
      ভঙ্গ;
    }
  }
  যদি(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    প্রত্যাগম CURLE_OK;
  যদি(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  অন্যথা যদি(!has_colon) {
    /* this is bad, bail out */
    failf(data, "Header without colon");
    প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
  }
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_bump_headersize(সংরচনা Curl_easy *data,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  unsigned পূর্ণাঙ্ক max = MAX_HTTP_RESP_HEADER_SIZE;
  যদি(delta < MAX_HTTP_RESP_HEADER_SIZE) {
    data->info.header_size += (unsigned পূর্ণাঙ্ক)delta;
    data->req.allheadercount += (unsigned পূর্ণাঙ্ক)delta;
    যদি(!connect_only)
      data->req.headerbytecount += (unsigned পূর্ণাঙ্ক)delta;
    যদি(data->req.allheadercount > max)
      bad = data->req.allheadercount;
    অন্যথা যদি(data->info.header_size > (max * 20)) {
      bad = data->info.header_size;
      max *= 20;
    }
  }
  অন্যথা
    bad = data->req.allheadercount + delta;
  যদি(bad) {
    failf(data, "Too large response headers: %zu > %u", bad, max);
    প্রত্যাগম CURLE_RECV_ERROR;
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
স্থির CURLcode http_on_101_upgrade(সংরচনা Curl_easy *data,
                                    ধ্রুব বর্ণ *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_changed)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা SingleRequest *k = &data->req;

#যদি !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (শূন্য)buf;
  (শূন্য)blen;
  (শূন্য)pconsumed;
#অন্যথা
  CURLcode result;
  পূর্ণাঙ্ক upgr101_requested = k->upgr101;
#endif

  যদি(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(data, "server sent 101 response while not talking HTTP/1.1");
    প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
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
    infof(data, "Received 101, Switching to HTTP/2");
    result = Curl_http2_upgrade(data, conn, FIRSTSOCKET, buf, blen);
    যদি(!result)
      *pconsumed += blen;
    প্রত্যাগম result;
  }
#endif
#ifndef CURL_DISABLE_WEBSOCKETS
  যদি(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(data, "Received 101, Switching to WebSocket");
    result = Curl_ws_accept(data, buf, blen);
    যদি(!result)
      *pconsumed += blen; /* ws accept handled the data */
    প্রত্যাগম result;
  }
#endif
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  প্রত্যাগম CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
স্থির CURLcode http_on_1xx_response(সংরচনা Curl_easy *data,
                                     ধ্রুব বর্ণ *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_changed)
{
  সংরচনা SingleRequest *k = &data->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  বিকল্প(k->httpcode) {
  পক্ষ 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(data);
    ভঙ্গ;
  পক্ষ 101:
    প্রত্যাগম http_on_101_upgrade(data, buf, blen, pconsumed, conn_changed);
  স্বভাব:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    ভঙ্গ;
  }
  প্রত্যাগম CURLE_OK;
}

#যদি defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
স্থির শূন্য http_check_auth_closure(সংরচনা Curl_easy *data,
                                    সংরচনা connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ifdef USE_NTLM
  যদি(conn->bits.close &&
     (((data->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((data->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
#endif
#ifdef USE_SPNEGO
  যদি(conn->bits.close &&
    (((data->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((data->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
  যদি((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  যদি((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 407)) {
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
স্থির CURLcode http_handle_send_error(সংরচনা Curl_easy *data)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  যদি(!data->req.authneg && !conn->bits.close &&
     !Curl_creader_will_rewind(data)) {
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

    বিকল্প(data->state.httpreq) {
    পক্ষ HTTPREQ_PUT:
    পক্ষ HTTPREQ_POST:
    পক্ষ HTTPREQ_POST_FORM:
    পক্ষ HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      যদি(!Curl_req_done_sending(data)) {
        যদি((k->httpcode == 417) && http_exp100_is_selected(data)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          যদি(!k->writebytecount && http_exp100_is_waiting(data)) {
            infof(data, "Got HTTP failure 417 while waiting for a 100");
          }
          অন্যথা {
            infof(data, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            result = http_perhapsrewind(data, conn);
            যদি(result)
              প্রত্যাগম result;
          }
          data->state.disableexpect = TRUE;
          Curl_req_abort_sending(data);
          DEBUGASSERT(!data->req.newurl);
          data->req.newurl = Curl_bufref_dup(&data->state.url);
          যদি(!data->req.newurl)
            প্রত্যাগম CURLE_OUT_OF_MEMORY;
        }
        অন্যথা যদি(data->set.http_keep_sending_on_error) {
          infof(data, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(data);
        }
        অন্যথা {
          infof(data, "HTTP error before end of send, stop sending");
          streamclose(conn);
          result = Curl_req_abort_sending(data);
          যদি(result)
            প্রত্যাগম result;
        }
      }
      ভঙ্গ;

    স্বভাব: /* default label present to avoid compiler warnings */
      ভঙ্গ;
    }
  }

  যদি(Curl_creader_will_rewind(data) && !Curl_req_done_sending(data)) {
    /* We rewind before next send, continue sending now */
    infof(data, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(data);
  }
  প্রত্যাগম result;
}

স্থির CURLcode http_on_response(সংরচনা Curl_easy *data,
                                 ধ্রুব বর্ণ *last_hd, size_t last_hd_len,
                                 ধ্রুব বর্ণ *buf, size_t blen,
                                 size_t *pconsumed)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  সংরচনা SingleRequest *k = &data->req;
  bool conn_changed = FALSE;

  (শূন্য)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  যদি(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    যদি(data->req.httpversion != 20)
      infof(data, "Lying server, not serving HTTP/2");
  }

  যদি(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    result = http_write_header(data, last_hd, last_hd_len);
    last_hd = রিক্ত; /* handled it */
    যদি(result)
      গচ্ছ out;
  }

  যদি(k->httpcode < 100) {
    failf(data, "Unsupported response code in HTTP response");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    গচ্ছ out;
  }
  অন্যথা যদি(k->httpcode < 200) {
    result = http_on_1xx_response(data, buf, blen, pconsumed, &conn_changed);
    গচ্ছ out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  যদি(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  যদি((k->size == -1) && !k->chunk && !conn->bits.close &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     data->state.httpreq != HTTPREQ_HEAD) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(data, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_closure(data, conn);

#ifndef CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  যদি(data->req.upgr101 == UPGR101_WS) {
    failf(data, "Refused WebSocket upgrade: %d", k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    গচ্ছ out;
  }
#endif

  /* Check if this response means the transfer errored. */
  যদি(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    গচ্ছ out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  result = Curl_http_auth_act(data);
  যদি(result)
    গচ্ছ out;

  যদি(k->httpcode >= 300) {
    result = http_handle_send_error(data);
    যদি(result)
      গচ্ছ out;
  }

  /* final response without error, prepare to receive the body */
  result = http_firstwrite(data);
  যদি(result)
    গচ্ছ out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  result = http_size(data);
  যদি(result)
    গচ্ছ out;

#ifndef CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  যদি((conn->scheme->protocol & CURLPROTO_RTSP) && data->req.no_body &&
     !data->set.opt_no_body && (k->size > 0))
    data->req.no_body = FALSE;
#endif

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  যদি(data->req.no_body)
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
    result = Curl_1st_fatal(result,
                            http_write_header(data, last_hd, last_hd_len));
  যদি(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(data->multi);
  প্রত্যাগম result;
}

স্থির CURLcode http_rw_hd(সংরচনা Curl_easy *data,
                           ধ্রুব বর্ণ *hd, size_t hdlen,
                           ধ্রুব বর্ণ *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  সংরচনা SingleRequest *k = &data->req;
  পূর্ণাঙ্ক writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  যদি((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    সংরচনা dynbuf last_header;
    size_t consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    result = curlx_dyn_addn(&last_header, hd, hdlen);
    যদি(result)
      প্রত্যাগম result;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&data->state.headerb);
    result = http_on_response(data, curlx_dyn_ptr(&last_header),
                              curlx_dyn_len(&last_header),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&last_header);
    প্রত্যাগম result;
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
    যদি(data->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ধ্রুব বর্ণ *p = hd;

      curlx_str_passblanks(&p);
      যদি(!strncmp(p, "HTTP/", 5)) {
        p += 5;
        বিকল্প(*p) {
        পক্ষ '1':
          p++;
          যদি((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            যদি(ISBLANK(p[2])) {
              k->httpversion = (unsigned বর্ণ)(10 + (p[1] - '0'));
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
            failf(data, "Unsupported HTTP/1 subversion in response");
            প্রত্যাগম CURLE_UNSUPPORTED_PROTOCOL;
          }
          ভঙ্গ;
        পক্ষ '2':
        পক্ষ '3':
          যদি(!ISBLANK(p[1]))
            ভঙ্গ;
          k->httpversion = (unsigned বর্ণ)((*p - '0') * 10);
          p += 2;
          যদি(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            যদি(!ISBLANK(*p))
              ভঙ্গ;
            fine_statusline = TRUE;
          }
          ভঙ্গ;
        স্বভাব: /* unsupported */
          failf(data, "Unsupported HTTP version in response");
          প্রত্যাগম CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      যদি(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(data, hd, hdlen);
        যদি(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    অন্যথা যদি(data->conn->scheme->protocol & CURLPROTO_RTSP) {
      ধ্রুব বর্ণ *p = hd;
      সংরচনা Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      যদি(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_সংখ্যা(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        যদি(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (পূর্ণাঙ্ক)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      যদি(!fine_statusline)
        প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
    }

    যদি(fine_statusline) {
      result = http_statusline(data, data->conn);
      যদি(result)
        প্রত্যাগম result;
      writetype |= CLIENTWRITE_STATUS;
    }
    অন্যথা {
      k->header = FALSE;   /* this is not a header line */
      প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
    }
  }

  result = Curl_verify_header(data, hd, hdlen);
  যদি(result)
    প্রত্যাগম result;

  result = http_header(data, hd, hdlen);
  যদি(result)
    প্রত্যাগম result;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  যদি(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  result = Curl_client_write(data, writetype, hd, hdlen);
  যদি(result)
    প্রত্যাগম result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  যদি(result)
    প্রত্যাগম result;

  প্রত্যাগম CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
শূন্য Curl_http_to_fold(সংরচনা dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  ধ্রুব বর্ণ *hd = curlx_dyn_ptr(bf);
  যদি(len && (hd[len - 1] == '\n'))
    len--;
  যদি(len && (hd[len - 1] == '\r'))
    len--;
  যাবত্(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

স্থির শূন্য unfold_header(সংরচনা Curl_easy *data)
{
  Curl_http_to_fold(&data->state.headerb);
  data->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
স্থির CURLcode http_parse_headers(সংরচনা Curl_easy *data,
                                   ধ্রুব বর্ণ *buf, size_t blen,
                                   size_t *pconsumed)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  সংরচনা SingleRequest *k = &data->req;
  ধ্রুব বর্ণ *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  যদি(data->state.maybe_folded && blen) {
    যদি(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(data);
    }
    অন্যথা {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      result = http_rw_hd(data, curlx_dyn_ptr(&data->state.headerb),
                          curlx_dyn_len(&data->state.headerb),
                          রিক্ত, 0, &ignore_this);
      curlx_dyn_reset(&data->state.headerb);
      যদি(result)
        প্রত্যাগম result;
    }
    data->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  যাবত্(blen && k->header) {
    size_t consumed;
    size_t hlen;
    ধ্রুব বর্ণ *hd;
    size_t unfold_len = 0;

    যদি(data->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      যাবত্(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      যদি(blen) {
        /* insert a single space */
        result = curlx_dyn_addn(&data->state.headerb, " ", 1);
        যদি(result)
          প্রত্যাগম result;
        data->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = memchr(buf, '\n', blen);
    যদি(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      result = curlx_dyn_addn(&data->state.headerb, buf, blen);
      যদি(result)
        প্রত্যাগম result;
      *pconsumed += blen + unfold_len;

      যদি(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(data, conn,
                           curlx_dyn_ptr(&data->state.headerb),
                           curlx_dyn_len(&data->state.headerb));

        যদি(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          যদি((k->httpversion >= 10) || conn->bits.reuse) {
            failf(data, "Invalid status line");
            প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
          }
          যদি(!data->state.http_neg.accept_09) {
            failf(data, "Received HTTP/0.9 when not allowed");
            প্রত্যাগম CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          গচ্ছ out;
        }
      }
      গচ্ছ out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    result = curlx_dyn_addn(&data->state.headerb, buf, consumed);
    যদি(result)
      প্রত্যাগম result;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&data->state.headerb);
    hd = curlx_dyn_ptr(&data->state.headerb);

    যদি(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(data, conn, hd, hlen);
      যদি(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        যদি((k->httpversion >= 10) || conn->bits.reuse) {
          failf(data, "Invalid status line");
          প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
        }
        যদি(!data->state.http_neg.accept_09) {
          failf(data, "Received HTTP/0.9 when not allowed");
          প্রত্যাগম CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        গচ্ছ out;
      }
    }
    অন্যথা {
      যদি(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        যদি(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(data);
          অনুবর্ত;
        }
        অন্যথা যদি(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          data->state.maybe_folded = TRUE;
          ভঙ্গ;
        }
      }
    }

    result = http_rw_hd(data, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&data->state.headerb);
    যদি(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    যদি(result)
      প্রত্যাগম result;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  যদি(!k->header && !leftover_body) {
    curlx_dyn_free(&data->state.headerb);
  }
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(সংরচনা Curl_easy *data,
                                 ধ্রুব বর্ণ *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode result;
  size_t consumed;
  বর্ণ tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  result = http_rw_hd(data, hd, hdlen, &tmp, 0, &consumed);
  যদি(!result && is_eos) {
    result = Curl_client_write(data, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  প্রত্যাগম result;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(সংরচনা Curl_easy *data,
                                  ধ্রুব বর্ণ *buf, size_t blen,
                                  size_t *pconsumed)
{
  যদি(!data->req.header) {
    *pconsumed = 0;
    প্রত্যাগম CURLE_OK;
  }
  অন্যথা {
    CURLcode result;

    result = http_parse_headers(data, buf, blen, pconsumed);
    যদি(!result && !data->req.header) {
      যদি(!data->req.no_body && curlx_dyn_len(&data->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&data->state.headerb),
                                   curlx_dyn_len(&data->state.headerb));
      }
      curlx_dyn_free(&data->state.headerb);
    }
    প্রত্যাগম result;
  }
}

CURLcode Curl_http_write_resp(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result;
  size_t consumed;
  পূর্ণাঙ্ক flags;

  result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
  যদি(result || data->req.done)
    গচ্ছ out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !data->req.header);
  যদি(!data->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    যদি(is_eos)
      flags |= CLIENTWRITE_EOS;
    result = Curl_client_write(data, flags, buf, blen);
  }
out:
  প্রত্যাগম result;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, ধ্রুব বর্ণ *s, size_t len)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  পূর্ণাঙ্ক i;

  যদি(len != 3)
    গচ্ছ out;

  প্রতি(i = 0; i < 3; ++i) {
    বর্ণ c = s[i];

    যদি(c < '0' || c > '9')
      গচ্ছ out;

    status *= 10;
    status += c - '0';
  }
  result = CURLE_OK;
out:
  *pstatus = result ? -1 : status;
  প্রত্যাগম result;
}

CURLcode Curl_http_req_make(সংরচনা httpreq **preq,
                            ধ্রুব বর্ণ *method, size_t m_len,
                            ধ্রুব বর্ণ *scheme, size_t s_len,
                            ধ্রুব বর্ণ *authority, size_t a_len,
                            ধ্রুব বর্ণ *path, size_t p_len)
{
  সংরচনা httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, পরিমাণ(*req) + m_len);
  যদি(!req)
    গচ্ছ out;
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
      গচ্ছ out;
  }
  যদি(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    যদি(!req->authority)
      গচ্ছ out;
  }
  যদি(path) {
    req->path = curlx_memdup0(path, p_len);
    যদি(!req->path)
      গচ্ছ out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  যদি(result && req)
    Curl_http_req_free(req);
  *preq = result ? রিক্ত : req;
  প্রত্যাগম result;
}

স্থির CURLcode req_assign_url_authority(সংরচনা httpreq *req, CURLU *url)
{
  বর্ণ *host, *port;
  সংরচনা dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  host = port = রিক্ত;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  যদি(uc && uc != CURLUE_NO_HOST)
    গচ্ছ out;
  যদি(!host) {
    req->authority = রিক্ত;
    result = CURLE_OK;
    গচ্ছ out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  যদি(uc && uc != CURLUE_NO_PORT)
    গচ্ছ out;

  result = curlx_dyn_add(&buf, host);
  যদি(result)
    গচ্ছ out;
  যদি(port) {
    result = curlx_dyn_addf(&buf, ":%s", port);
    যদি(result)
      গচ্ছ out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  যদি(result)
    curlx_dyn_free(&buf);
  প্রত্যাগম result;
}

স্থির CURLcode req_assign_url_path(সংরচনা httpreq *req, CURLU *url)
{
  বর্ণ *path, *query;
  সংরচনা dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  path = query = রিক্ত;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  যদি(uc)
    গচ্ছ out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  যদি(uc && uc != CURLUE_NO_QUERY)
    গচ্ছ out;

  যদি(!query) {
    req->path = path;
    path = রিক্ত;
  }
  অন্যথা {
    result = curlx_dyn_add(&buf, path);
    যদি(!result)
      result = curlx_dyn_addf(&buf, "?%s", query);
    যদি(result)
      গচ্ছ out;
    req->path = curlx_dyn_ptr(&buf);
  }
  result = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  যদি(result)
    curlx_dyn_free(&buf);
  প্রত্যাগম result;
}

CURLcode Curl_http_req_make2(সংরচনা httpreq **preq,
                             ধ্রুব বর্ণ *method, size_t m_len,
                             CURLU *url, ধ্রুব বর্ণ *scheme_default)
{
  সংরচনা httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, পরিমাণ(*req) + m_len);
  যদি(!req)
    গচ্ছ out;
  memcpy(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  যদি(uc && uc != CURLUE_NO_SCHEME)
    গচ্ছ out;
  যদি(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    যদি(!req->scheme)
      গচ্ছ out;
  }

  result = req_assign_url_authority(req, url);
  যদি(result)
    গচ্ছ out;
  result = req_assign_url_path(req, url);
  যদি(result)
    গচ্ছ out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  যদি(result && req)
    Curl_http_req_free(req);
  *preq = result ? রিক্ত : req;
  প্রত্যাগম result;
}

শূন্য Curl_http_req_free(সংরচনা httpreq *req)
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

সংরচনা name_const {
  ধ্রুব বর্ণ *name;
  size_t namelen;
};

স্থির ধ্রুব সংরচনা name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

স্থির bool h2_permissible_field(সংরচনা dynhds_entry *e)
{
  size_t i;
  প্রতি(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    যদি(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      প্রত্যাগম FALSE;
  }
  প্রত্যাগম TRUE;
}

স্থির bool http_TE_has_token(ধ্রুব বর্ণ *fvalue, ধ্রুব বর্ণ *token)
{
  যাবত্(*fvalue) {
    সংরচনা Curl_str name;

    /* skip to first token */
    যাবত্(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    যদি(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      প্রত্যাগম FALSE;
    যদি(curlx_str_casecompare(&name, token))
      প্রত্যাগম TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    যাবত্(*fvalue && *fvalue != ',') {
      যদি(*fvalue == '"') {
        সংরচনা Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        যদি(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          প্রত্যাগম FALSE;
      }
      অন্যথা
        fvalue++;
    }
  }
  প্রত্যাগম FALSE;
}

CURLcode Curl_http_req_to_h2(সংরচনা dynhds *h2_headers,
                             সংরচনা httpreq *req, সংরচনা Curl_easy *data)
{
  ধ্রুব বর্ণ *scheme = রিক্ত, *authority = রিক্ত;
  সংরচনা dynhds_entry *e;
  size_t i;
  CURLcode result;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  যদি(req->scheme) {
    scheme = req->scheme;
  }
  অন্যথা যদি(strcmp("CONNECT", req->method)) {
    scheme = Curl_checkheaders(data, STRCONST(HTTP_PSEUDO_SCHEME));
    যদি(scheme) {
      scheme += পরিমাণ(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(data, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    অন্যথা {
      scheme = data->state.origin->scheme->name;
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
  result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, strlen(req->method));
  যদি(!result && scheme) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, strlen(scheme));
  }
  যদি(!result && authority) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, strlen(authority));
  }
  যদি(!result && req->path) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, strlen(req->path));
  }
  প্রতি(i = 0; !result && i < Curl_dynhds_গুনতি(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    যদি(e->namelen == 2 && curl_strequal("TE", e->name)) {
      যদি(http_TE_has_token(e->মান, "trailers"))
        result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    অন্যথা যদি(h2_permissible_field(e)) {
      result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->মান, e->valuelen);
    }
  }

  প্রত্যাগম result;
}

CURLcode Curl_http_resp_make(সংরচনা http_resp **presp,
                             পূর্ণাঙ্ক status,
                             ধ্রুব বর্ণ *description)
{
  সংরচনা http_resp *resp;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, পরিমাণ(*resp));
  যদি(!resp)
    গচ্ছ out;

  resp->status = status;
  যদি(description) {
    resp->description = curlx_strdup(description);
    যদি(!resp->description)
      গচ্ছ out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  যদি(result && resp)
    Curl_http_resp_free(resp);
  *presp = result ? রিক্ত : resp;
  প্রত্যাগম result;
}

শূন্য Curl_http_resp_free(সংরচনা http_resp *resp)
{
  যদি(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    যদি(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ধ্রুব সংরচনা Curl_protocol Curl_protocol_http = {
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
