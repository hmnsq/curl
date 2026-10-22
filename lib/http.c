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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"

#ifndef CURL_DISABLE_HTTP

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif

#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#അന്തര്ഭാവ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#അന്തര്ഭാവ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#അന്തര്ഭാവ <sys/param.h>
#endif

#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "formdata.h"
#അന്തര്ഭാവ "mime.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "curlx/base64.h"
#അന്തര്ഭാവ "cookie.h"
#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "vquic/vquic.h"
#അന്തര്ഭാവ "http_digest.h"
#അന്തര്ഭാവ "http_ntlm.h"
#അന്തര്ഭാവ "http_negotiate.h"
#അന്തര്ഭാവ "http_aws_sigv4.h"
#അന്തര്ഭാവ "http_httpsig.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "urlapi-int.h"
#അന്തര്ഭാവ "curl_share.h"
#അന്തര്ഭാവ "dynhds.h"
#അന്തര്ഭാവ "http.h"
#അന്തര്ഭാവ "headers.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "parsedate.h" /* for the week day and month names */
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "content_encoding.h"
#അന്തര്ഭാവ "http_proxy.h"
#അന്തര്ഭാവ "http2.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "altsvc.h"
#അന്തര്ഭാവ "hsts.h"
#അന്തര്ഭാവ "rtsp.h"
#അന്തര്ഭാവ "ws.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "curlx/strparse.h"

ശൂന്യം Curl_http_neg_init(ഘടന Curl_easy *data, ഘടന http_negotiation *neg)
{
  memset(neg, 0, വലുപ്പം(*neg));
  neg->accept_09 = data->set.http09_allowed;
  തിരഞ്ഞെടുപ്പ്(data->set.httpwant) {
  അവസ്ഥ CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    നിർത്തുക;
  അവസ്ഥ CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    നിർത്തുക;
  അവസ്ഥ CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    നിർത്തുക;
  അവസ്ഥ CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    നിർത്തുക;
  അവസ്ഥ CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    data->state.http_neg.h2_prior_knowledge = TRUE;
    നിർത്തുക;
  അവസ്ഥ CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    നിർത്തുക;
  അവസ്ഥ CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    നിർത്തുക;
  അവസ്ഥ CURL_HTTP_VERSION_NONE:
  സ്ഥിരസ്ഥിതി:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    നിർത്തുക;
  }
}

CURLcode Curl_http_setup_conn(ഘടന Curl_easy *data,
                              ഘടന connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  എങ്കിൽ(data->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode result = Curl_conn_may_http3(data, conn);
    എങ്കിൽ(result)
      തിരികെ result;
  }
  തിരികെ CURLE_OK;
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
അക്ഷരം *Curl_checkProxyheaders(ഘടന Curl_easy *data,
                             സ്ഥിരം ഘടന connectdata *conn,
                             സ്ഥിരം അക്ഷരം *thisheader,
                             സ്ഥിരം size_t thislen)
{
  ഘടന curl_slist *head;

  വേണ്ടി(head = (conn->http_proxy.peer && data->set.sep_headers) ?
        data->set.proxyheaders : data->set.headers;
      head; head = head->next) {
    എങ്കിൽ(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      തിരികെ head->data;
  }

  തിരികെ ശൂന്യ;
}
#endif

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
സ്ഥിതം bool header_has_മൂല്യം(സ്ഥിരം അക്ഷരം **headerp, ഘടന Curl_str *outp)
{
  bool മൂല്യം = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  എങ്കിൽ(മൂല്യം) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  തിരികെ മൂല്യം;
}

സ്ഥിതം bool http_header_is_empty(സ്ഥിരം അക്ഷരം *header)
{
  ഘടന Curl_str out;

  എങ്കിൽ(header_has_മൂല്യം(&header, &out)) {
    തിരികെ curlx_strlen(&out) == 0;
  }
  തിരികെ TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
സ്ഥിതം CURLcode copy_custom_മൂല്യം(സ്ഥിരം അക്ഷരം *header, അക്ഷരം **valp)
{
  ഘടന Curl_str out = { 0 };

  /* find the end of the header name */
  എങ്കിൽ(header_has_മൂല്യം(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    എങ്കിൽ(*valp)
      തിരികെ CURLE_OK;
    തിരികെ CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = ശൂന്യ;
  തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
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
അക്ഷരം *Curl_copy_header_മൂല്യം(സ്ഥിരം അക്ഷരം *header)
{
  ഘടന Curl_str out;

  /* find the end of the header name */
  എങ്കിൽ(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_SIZE, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_SIZE);
    curlx_str_trimblanks(&out);
    തിരികെ curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  തിരികെ ശൂന്യ;
}

#ifndef CURL_DISABLE_HTTP_AUTH

#ifndef CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
സ്ഥിതം CURLcode http_output_basic(ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn, bool proxy)
{
  size_t size = 0;
  അക്ഷരം *authorization = ശൂന്യ;
  അക്ഷരം **p_hd;
  CURLcode result;
  ഘടന Curl_creds *creds = ശൂന്യ;
  അക്ഷരം *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  എങ്കിൽ(proxy) {
#ifndef CURL_DISABLE_PROXY
    p_hd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#അല്ലെങ്കിൽ
    (ശൂന്യം)conn;
    തിരികെ CURLE_NOT_BUILT_IN;
#endif
  }
  അല്ലെങ്കിൽ {
    p_hd = &data->req.hd_auth;
    creds = data->state.creds;
  }

  എങ്കിൽ(!creds) {
    DEBUGASSERT(0);
    തിരികെ CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  എങ്കിൽ(!out)
    തിരികെ CURLE_OUT_OF_MEMORY;

  result = curlx_base64_encode((uint8_t *)out, strlen(out),
                               &authorization, &size);
  എങ്കിൽ(result)
    പോകുക fail;

  എങ്കിൽ(!authorization) {
    result = CURLE_REMOTE_ACCESS_DENIED;
    പോകുക fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  എങ്കിൽ(!*p_hd) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക fail;
  }

fail:
  curlx_free(out);
  തിരികെ result;
}

#endif

#ifndef CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
സ്ഥിതം CURLcode http_output_bearer(ഘടന Curl_easy *data)
{
  അക്ഷരം **userp;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(data->state.creds));
  userp = &data->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(data->state.creds));

  എങ്കിൽ(!*userp) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക fail;
  }

fail:
  തിരികെ result;
}
#endif

#endif

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
സ്ഥിതം bool pickoneauth(ഘടന auth *pick, unsigned long mask,
                        ഘടന Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  unsigned long avail = pick->avail & pick->want & mask;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  എങ്കിൽ(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ifndef CURL_DISABLE_BEARER_AUTH
  അല്ലെങ്കിൽ എങ്കിൽ((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  അല്ലെങ്കിൽ എങ്കിൽ((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#endif
  അല്ലെങ്കിൽ എങ്കിൽ(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ifndef CURL_DISABLE_BASIC_AUTH
  അല്ലെങ്കിൽ എങ്കിൽ((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#endif
#ifndef CURL_DISABLE_AWS
  അല്ലെങ്കിൽ എങ്കിൽ(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  അല്ലെങ്കിൽ എങ്കിൽ(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#endif
  അല്ലെങ്കിൽ {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  തിരികെ picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
സ്ഥിതം CURLcode http_perhapsrewind(ഘടന Curl_easy *data,
                                   ഘടന connectdata *conn)
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
  VERBOSE(സ്ഥിരം അക്ഷരം *ongoing_auth = ശൂന്യ);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  എങ്കിൽ(needs_rewind) {
    infof(data, "Need to rewind upload for next request");
    Curl_creader_set_rewind(data, TRUE);
  }

  എങ്കിൽ(conn->bits.close)
    /* If we already decided to close this connection, we cannot veto. */
    തിരികെ CURLE_OK;

  എങ്കിൽ(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ifdef USE_NTLM
    എങ്കിൽ((data->state.authproxy.picked == CURLAUTH_NTLM) ||
       (data->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      എങ്കിൽ((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
#ifdef USE_SPNEGO
    /* There is still data left to send */
    എങ്കിൽ((data->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (data->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      എങ്കിൽ((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
  }

  എങ്കിൽ(abort_upload) {
    എങ്കിൽ(upload_remain >= 0)
      infof(data, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    അല്ലെങ്കിൽ
      infof(data, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    data->req.size = 0; /* do not download any more than 0 bytes */
    data->req.http_bodyless = TRUE;
  }
  തിരികെ CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
സ്ഥിതം bool http_should_fail(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ httpcode)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  എങ്കിൽ(!data->set.http_fail_on_error)
    തിരികെ FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  എങ്കിൽ(httpcode < 400)
    തിരികെ FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  എങ്കിൽ(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    തിരികെ FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  എങ്കിൽ((httpcode != 401) && (httpcode != 407))
    തിരികെ TRUE;

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
  എങ്കിൽ((httpcode == 401) && !data->state.creds)
    തിരികെ TRUE;
#ifndef CURL_DISABLE_PROXY
  എങ്കിൽ((httpcode == 407) && !data->conn->http_proxy.creds)
    തിരികെ TRUE;
#endif

  തിരികെ (bool)data->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(ഘടന Curl_easy *data)
{
  ഘടന connectdata *conn = data->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode result = CURLE_OK;
  unsigned long authmask = ~0UL;

  എങ്കിൽ(!Curl_creds_has_oauth_bearer(data->state.creds))
    authmask &= (unsigned long)~CURLAUTH_BEARER;

  എങ്കിൽ(100 <= data->req.httpcode && data->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    തിരികെ CURLE_OK;

  എങ്കിൽ(data->state.authproblem)
    തിരികെ data->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  എങ്കിൽ(data->state.creds &&
     ((data->req.httpcode == 401) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickhost = pickoneauth(&data->state.authhost, authmask, data->state.creds);
    എങ്കിൽ(!pickhost)
      data->state.authproblem = TRUE;
    അല്ലെങ്കിൽ
      data->info.httpauthpicked = data->state.authhost.picked;
    എങ്കിൽ(data->state.authhost.picked == CURLAUTH_NTLM &&
       (data->req.httpversion_sent > 11)) {
      infof(data, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ifndef CURL_DISABLE_PROXY
  എങ്കിൽ(conn->http_proxy.creds &&
     ((data->req.httpcode == 407) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickproxy = pickoneauth(&data->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    എങ്കിൽ(!pickproxy)
      data->state.authproblem = TRUE;
    അല്ലെങ്കിൽ
      data->info.proxyauthpicked = data->state.authproxy.picked;
  }
#endif

  എങ്കിൽ(pickhost || pickproxy) {
    result = http_perhapsrewind(data, conn);
    എങ്കിൽ(result)
      തിരികെ result;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(data->req.newurl);
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    എങ്കിൽ(!data->req.newurl)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }
  അല്ലെങ്കിൽ എങ്കിൽ((data->req.httpcode < 300) &&
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
    എങ്കിൽ(!data->req.newurl)
      തിരികെ CURLE_OUT_OF_MEMORY;
    data->state.authhost.done = TRUE;
  }
  എങ്കിൽ(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          data->req.httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
  }

  തിരികെ result;
}

#ifndef CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
സ്ഥിതം CURLcode output_auth_headers(ഘടന Curl_easy *data,
                                    ഘടന connectdata *conn,
                                    ഘടന auth *authstatus,
                                    സ്ഥിരം അക്ഷരം *request,
                                    സ്ഥിരം അക്ഷരം *path,
                                    bool proxy)
{
  സ്ഥിരം അക്ഷരം *auth = ശൂന്യ;
  CURLcode result = CURLE_OK;
  (ശൂന്യം)conn;

#ifdef CURL_DISABLE_DIGEST_AUTH
  (ശൂന്യം)request;
  (ശൂന്യം)path;
#endif
#ifndef CURL_DISABLE_AWS
  എങ്കിൽ((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    result = Curl_output_aws_sigv4(data);
    എങ്കിൽ(result)
      തിരികെ result;
  }
  അല്ലെങ്കിൽ
#endif
#ifndef CURL_DISABLE_HTTPSIG
  എങ്കിൽ((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    എങ്കിൽ(Curl_auth_allowed_to_host(data)) {
      auth = "HTTPSIG";
      result = Curl_output_httpsig(data);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    അല്ലെങ്കിൽ
      authstatus->done = TRUE;
  }
  അല്ലെങ്കിൽ
#endif
#ifdef USE_SPNEGO
  എങ്കിൽ(authstatus->picked == CURLAUTH_NEGOTIATE) {
    എങ്കിൽ(
#ifndef CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(data, conn,
                                        STRCONST("Proxy-authorization"))) ||
#endif
      (!proxy && !Curl_checkheaders(data, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(data))) {
      auth = "Negotiate";
      result = Curl_output_negotiate(data, conn, proxy);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    അല്ലെങ്കിൽ
      authstatus->done = TRUE;
  }
  അല്ലെങ്കിൽ
#endif
#ifdef USE_NTLM
  എങ്കിൽ(authstatus->picked == CURLAUTH_NTLM) {
    എങ്കിൽ(proxy || Curl_auth_allowed_to_host(data)) {
      auth = "NTLM";
      result = Curl_output_ntlm(data, proxy);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    അല്ലെങ്കിൽ
      authstatus->done = TRUE;
  }
  അല്ലെങ്കിൽ
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  എങ്കിൽ(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    result = Curl_output_digest(data,
                                proxy,
                                (സ്ഥിരം unsigned അക്ഷരം *)request,
                                (സ്ഥിരം unsigned അക്ഷരം *)path);
    എങ്കിൽ(result)
      തിരികെ result;
  }
  അല്ലെങ്കിൽ
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
  എങ്കിൽ(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    എങ്കിൽ(
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
      എങ്കിൽ(result)
        തിരികെ result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
  എങ്കിൽ(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    എങ്കിൽ(!proxy && Curl_creds_has_oauth_bearer(data->state.creds) &&
       !Curl_checkheaders(data, STRCONST("Authorization"))) {
      auth = "Bearer";
      result = http_output_bearer(data);
      എങ്കിൽ(result)
        തിരികെ result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif

  എങ്കിൽ(auth) {
#ifndef CURL_DISABLE_PROXY
    എങ്കിൽ(proxy)
      data->info.proxyauthpicked = authstatus->picked;
    അല്ലെങ്കിൽ
      data->info.httpauthpicked = authstatus->picked;
    infof(data, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (data->state.creds ?
           data->state.creds->user : ""));
#അല്ലെങ്കിൽ
    (ശൂന്യം)proxy;
    infof(data, "Server auth using %s with user '%s'",
          auth, data->state.creds ?
          data->state.creds->user : "");
#endif
    authstatus->multipass = !authstatus->done;
  }
  അല്ലെങ്കിൽ {
    authstatus->multipass = FALSE;
    എങ്കിൽ(proxy)
      data->info.proxyauthpicked = 0;
    അല്ലെങ്കിൽ
      data->info.httpauthpicked = 0;
  }

  തിരികെ result;
}

CURLcode Curl_http_output_auth(ഘടന Curl_easy *data,
                               ഘടന connectdata *conn,
                               സ്ഥിരം അക്ഷരം *request,
                               Curl_HttpReq httpreq,
                               സ്ഥിരം അക്ഷരം *path,
                               സ്ഥിരം അക്ഷരം *query,
                               bool is_connect)
{
  CURLcode result = CURLE_OK;
  ഘടന auth *authhost;
  ഘടന auth *authproxy;
  സ്ഥിരം അക്ഷരം *path_and_query = path;
  അക്ഷരം *tmp_str = ശൂന്യ;

  DEBUGASSERT(data);
  authhost = &data->state.authhost;
  authproxy = &data->state.authproxy;

  എങ്കിൽ(
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
    പോകുക out;
  }

  എങ്കിൽ(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    എങ്കിൽ(!tmp_str) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
    path_and_query = tmp_str;
  }

  എങ്കിൽ(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  എങ്കിൽ(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ifndef CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  എങ്കിൽ(conn->bits.origin_is_proxy || is_connect) {
    result = output_auth_headers(data, conn, authproxy, request,
                                 path_and_query, TRUE);
    എങ്കിൽ(result)
      പോകുക out;
  }
  അല്ലെങ്കിൽ
#അല്ലെങ്കിൽ
  (ശൂന്യം)is_connect;
#endif /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->done = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  എങ്കിൽ(data->state.creds || Curl_auth_allowed_to_host(data))
    result = output_auth_headers(data, conn, authhost, request,
                                 path_and_query, FALSE);
  അല്ലെങ്കിൽ
    authhost->done = TRUE;

  എങ്കിൽ(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_HEAD)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    data->req.authneg = TRUE;
  }
  അല്ലെങ്കിൽ
    data->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  തിരികെ result;
}

#അല്ലെങ്കിൽ /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(ഘടന Curl_easy *data,
                               ഘടന connectdata *conn,
                               സ്ഥിരം അക്ഷരം *request,
                               Curl_HttpReq httpreq,
                               സ്ഥിരം അക്ഷരം *path,
                               സ്ഥിരം അക്ഷരം *query,
                               bool is_connect)
{
  (ശൂന്യം)data;
  (ശൂന്യം)conn;
  (ശൂന്യം)request;
  (ശൂന്യം)httpreq;
  (ശൂന്യം)path;
  (ശൂന്യം)query;
  (ശൂന്യം)is_connect;
  തിരികെ CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP_AUTH, else */

#എങ്കിൽ defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
സ്ഥിതം bool authcmp(സ്ഥിരം അക്ഷരം *auth, സ്ഥിരം അക്ഷരം *വരി)
{
  /* the auth string must not have an alnum following */
  size_t n = strlen(auth);
  തിരികെ curl_strnequal(auth, വരി, n) && !ISALNUM(വരി[n]);
}
#endif

#ifdef USE_SPNEGO
സ്ഥിതം CURLcode auth_spnego(ഘടന Curl_easy *data,
                            bool proxy,
                            സ്ഥിരം അക്ഷരം *auth,
                            ഘടന auth *authp,
                            uint32_t *availp)
{
  എങ്കിൽ((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    എങ്കിൽ(authp->picked == CURLAUTH_NEGOTIATE) {
      ഘടന connectdata *conn = data->conn;
      CURLcode result = Curl_നിവേശം_negotiate(data, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      എങ്കിൽ(!result) {
        curlx_free(data->req.newurl);
        data->req.newurl = Curl_bufref_dup(&data->state.url);
        എങ്കിൽ(!data->req.newurl)
          തിരികെ CURLE_OUT_OF_MEMORY;
        data->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      അല്ലെങ്കിൽ
        data->state.authproblem = TRUE;
    }
  }
  തിരികെ CURLE_OK;
}
#endif

#ifdef USE_NTLM
സ്ഥിതം CURLcode auth_ntlm(ഘടന Curl_easy *data,
                          bool proxy,
                          സ്ഥിരം അക്ഷരം *auth,
                          ഘടന auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  എങ്കിൽ((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    എങ്കിൽ(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode result = Curl_നിവേശം_ntlm(data, proxy, auth);
      എങ്കിൽ(!result)
        data->state.authproblem = FALSE;
      അല്ലെങ്കിൽ {
        എങ്കിൽ(result == CURLE_OUT_OF_MEMORY)
          തിരികെ result;
        infof(data, "NTLM authentication problem, ignoring.");
        data->state.authproblem = TRUE;
      }
    }
  }
  തിരികെ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_DIGEST_AUTH
സ്ഥിതം CURLcode auth_digest(ഘടന Curl_easy *data,
                            bool proxy,
                            സ്ഥിരം അക്ഷരം *auth,
                            ഘടന auth *authp,
                            uint32_t *availp)
{
  എങ്കിൽ(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(data, "Ignoring duplicate digest auth header.");
  }
  അല്ലെങ്കിൽ എങ്കിൽ(Curl_auth_is_digest_supported()) {
    CURLcode result;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    result = Curl_നിവേശം_digest(data, proxy, auth);
    എങ്കിൽ(result) {
      എങ്കിൽ(result == CURLE_OUT_OF_MEMORY)
        തിരികെ result;
      infof(data, "Digest authentication problem, ignoring.");
      data->state.authproblem = TRUE;
    }
  }
  തിരികെ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BASIC_AUTH
സ്ഥിതം CURLcode auth_basic(ഘടന Curl_easy *data,
                           ഘടന auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  എങ്കിൽ(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Basic authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  തിരികെ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BEARER_AUTH
സ്ഥിതം CURLcode auth_bearer(ഘടന Curl_easy *data,
                            ഘടന auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  എങ്കിൽ(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Bearer authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  തിരികെ CURLE_OK;
}
#endif

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_നിവേശം_auth(ഘടന Curl_easy *data, bool proxy,
                              സ്ഥിരം അക്ഷരം *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#എങ്കിൽ defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  ഘടന auth *authp;
  CURLcode result = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(data);

  എങ്കിൽ(proxy) {
    availp = &data->info.proxyauthavail;
    authp = &data->state.authproxy;
  }
  അല്ലെങ്കിൽ {
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

  ആകുമ്പോൾ(*auth) {
#ifdef USE_SPNEGO
    എങ്കിൽ(authcmp("Negotiate", auth))
      result = auth_spnego(data, proxy, auth, authp, availp);
#endif
#ifdef USE_NTLM
    എങ്കിൽ(!result && authcmp("NTLM", auth))
      result = auth_ntlm(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
    എങ്കിൽ(!result && authcmp("Digest", auth))
      result = auth_digest(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
    എങ്കിൽ(!result && authcmp("Basic", auth))
      result = auth_basic(data, authp, availp);
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
    എങ്കിൽ(authcmp("Bearer", auth))
      result = auth_bearer(data, authp, availp);
#endif

    എങ്കിൽ(result)
      നിർത്തുക;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    എങ്കിൽ(auth) /* if we are on a comma, skip it */
      auth++;
    അല്ലെങ്കിൽ
      നിർത്തുക;
    curlx_str_passblanks(&auth);
  }
  തിരികെ result;
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)proxy;
  (ശൂന്യം)auth;
  /* nothing to do when disabled */
  തിരികെ CURLE_OK;
#endif
}

സ്ഥിതം ശൂന്യം http_switch_to_get(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ code)
{
  സ്ഥിരം അക്ഷരം *req = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  എങ്കിൽ((req || data->state.httpreq != HTTPREQ_GET) &&
     (data->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((ശൂന്യം)code);
    infof(data, "Switch to GET because of %d response", code);
    data->state.http_ignorecustom = TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(req && (data->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(data, "Stick to %s instead of GET", req);

  data->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(data, FALSE);
}

#define HTTPREQ_IS_POST(data)                    \
  ((data)->state.httpreq == HTTPREQ_POST ||      \
   (data)->state.httpreq == HTTPREQ_POST_FORM || \
   (data)->state.httpreq == HTTPREQ_POST_MIME)

സ്ഥിതം CURLcode set_auto_referer(ഘടന Curl_easy *data)
{
  CURLU *u;
  അക്ഷരം *referer = ശൂന്യ;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&data->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  എങ്കിൽ(!u)
    തിരികെ CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&data->state.url), 0);
  എങ്കിൽ(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, ശൂന്യ, 0);
  എങ്കിൽ(!uc)
    uc = curl_url_set(u, CURLUPART_USER, ശൂന്യ, 0);
  എങ്കിൽ(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, ശൂന്യ, 0);
  എങ്കിൽ(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  എങ്കിൽ(uc || !referer)
    തിരികെ CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&data->state.referer, referer, 0, curl_free);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode follow_parse_url(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *newurl,
                                 followtype type, അക്ഷരം **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(data->state.uh);
  uc = curl_url_set(data->state.uh, CURLUPART_URL, newurl, (unsigned പൂർണ്ണസംഖ്യ)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  എങ്കിൽ(uc) {
    എങ്കിൽ((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(data, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      തിരികെ Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    എങ്കിൽ(!*follow_urlp)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }
  അല്ലെങ്കിൽ {
    CURLU *u = curl_url();
    എങ്കിൽ(!u)
      തിരികെ CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&data->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    എങ്കിൽ(!uc)
      uc = curl_url_get(data->state.uh, CURLUPART_URL, follow_urlp, 0);
    എങ്കിൽ(uc) {
      curl_url_cleanup(u);
      തിരികെ Curl_uc_to_curlcode(uc);
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, data->state.uh);
      curl_url_cleanup(u);
      എങ്കിൽ(!same_origin)
        Curl_auth_digest_cleanup(&data->state.digest);
    }
#അല്ലെങ്കിൽ
    curl_url_cleanup(u);
#endif
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode follow_fake(ഘടന Curl_easy *data, അക്ഷരം *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  എങ്കിൽ(!uc) {
    CURLU *u = curl_url();
    അക്ഷരം *nocred = ശൂന്യ;

    എങ്കിൽ(!u) {
      curlx_free(follow_url);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }
    എങ്കിൽ(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, ശൂന്യ, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, ശൂന്യ, 0))
      (ശൂന്യം)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    എങ്കിൽ(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  data->info.wouldredirect = follow_url;

  എങ്കിൽ(reachedmax) {
    failf(data, "Maximum (%d) redirects followed", data->set.maxredirs);
    തിരികെ CURLE_TOO_MANY_REDIRECTS;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം bool follow_switch_get(ഘടന Curl_easy *data)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  തിരഞ്ഞെടുപ്പ്(data->info.httpcode) {
  അവസ്ഥ 301: /* Moved Permanently */
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
    എങ്കിൽ(HTTPREQ_IS_POST(data) && !data->set.post301) {
      http_switch_to_get(data, 301);
      തിരികെ TRUE;
    }
    നിർത്തുക;
  അവസ്ഥ 302: /* Found */
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
    എങ്കിൽ(HTTPREQ_IS_POST(data) && !data->set.post302) {
      http_switch_to_get(data, 302);
      തിരികെ TRUE;
    }
    നിർത്തുക;
  അവസ്ഥ 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    എങ്കിൽ(!HTTPREQ_IS_POST(data) || !data->set.post303) {
      http_switch_to_get(data, 303);
      തിരികെ TRUE;
    }
    നിർത്തുക;

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
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ FALSE;
}

CURLcode Curl_http_follow(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  അക്ഷരം *follow_url = ശൂന്യ;
  CURLUcode uc;
  CURLcode result;
  CURLcode rewind_result;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  എങ്കിൽ(type != FOLLOW_FAKE)
    data->state.requests++; /* count all real follows */
  എങ്കിൽ(type == FOLLOW_REDIR) {
    എങ്കിൽ((data->set.maxredirs != -1) &&
       (data->state.followlocation >= data->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    അല്ലെങ്കിൽ {
      data->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      എങ്കിൽ(data->set.http_auto_referer) {
        result = set_auto_referer(data);
        എങ്കിൽ(result)
          തിരികെ result;
      }
    }
  }

  എങ്കിൽ((type != FOLLOW_RETRY) &&
     (data->req.httpcode != 401) && (data->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, ശൂന്യ, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  result = follow_parse_url(data, newurl, type, &follow_url, &uc);
  എങ്കിൽ(result)
    തിരികെ result;

  DEBUGASSERT(follow_url);

  എങ്കിൽ(type == FOLLOW_FAKE)
    തിരികെ follow_fake(data, follow_url, uc, reachedmax);

  എങ്കിൽ(disallowport)
    data->state.allow_port = FALSE;

  Curl_bufref_set(&data->state.url, follow_url, 0, curl_free);
  rewind_result = Curl_req_soft_reset(&data->req, data);
  infof(data, "Issue another request to this URL: '%s'", follow_url);
  എങ്കിൽ((data->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    data->state.http_ignorecustom = TRUE;
    infof(data, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(data);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  എങ്കിൽ(rewind_result && !switch_to_get)
    തിരികെ rewind_result;

  Curl_pgrsTime(data, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(data);

  തിരികെ CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(സ്ഥിരം അക്ഷരം *headerline, /* line to check */
                        സ്ഥിരം അക്ഷരം *header, /* header keyword _with_ colon */
                        സ്ഥിരം size_t hlen, /* len of the keyword in bytes */
                        സ്ഥിരം അക്ഷരം *content, /* content string to find */
                        സ്ഥിരം size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  സ്ഥിരം അക്ഷരം *p;
  ഘടന Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  എങ്കിൽ(!curl_strnequal(headerline, header, hlen))
    തിരികെ FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  എങ്കിൽ(curlx_str_cspn(&p, &val, "\r\n"))
    തിരികെ FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  എങ്കിൽ(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    വേണ്ടി(len = curlx_strlen(&val); len >= clen;) {
      ഘടന Curl_str next;
      സ്ഥിരം അക്ഷരം *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      എങ്കിൽ(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        തിരികെ TRUE; /* match! */
      /* advance to the next comma */
      എങ്കിൽ(curlx_str_until(&p, &next, len, ',') ||
         curlx_str_single(&p, ','))
        നിർത്തുക; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      ചെയ്യുക
        curlx_str_passblanks(&p);
      ആകുമ്പോൾ(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      എങ്കിൽ((size_t)(p - o) > len)
        നിർത്തുക;
      len -= (p - o);
    }
  }
  തിരികെ FALSE; /* no match */
}

ഘടന cr_exp100_ctx {
  ഘടന Curl_creader super;
  ഘടന curltime start; /* time started waiting */
  എണ്ണൽ expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

സ്ഥിതം ശൂന്യം http_exp100_continue(ഘടന Curl_easy *data,
                                 ഘടന Curl_creader *reader)
{
  ഘടന cr_exp100_ctx *ctx = reader->ctx;
  എങ്കിൽ(ctx->state > EXP100_SEND_DATA) {
    ctx->state = EXP100_SEND_DATA;
    Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
  }
}

സ്ഥിതം CURLcode cr_exp100_read(ഘടന Curl_easy *data,
                               ഘടന Curl_creader *reader,
                               അക്ഷരം *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ഘടന cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  തിരഞ്ഞെടുപ്പ്(ctx->state) {
  അവസ്ഥ EXP100_SENDING_REQUEST:
    എങ്കിൽ(!Curl_req_sendbuf_empty(data)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(data, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      തിരികെ CURLE_OK;
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
    തിരികെ CURLE_OK;
  അവസ്ഥ EXP100_FAILED:
    DEBUGF(infof(data, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    തിരികെ CURLE_READ_ERROR;
  അവസ്ഥ EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->start);
    എങ്കിൽ(ms < data->set.expect_100_timeout) {
      DEBUGF(infof(data, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      തിരികെ CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(data, reader);
    infof(data, "Done waiting for 100-continue");
    FALLTHROUGH();
  സ്ഥിരസ്ഥിതി:
    DEBUGF(infof(data, "cr_exp100_read, pass through"));
    തിരികെ Curl_creader_read(data, reader->next, buf, blen, nread, eos);
  }
}

സ്ഥിതം ശൂന്യം cr_exp100_done(ഘടന Curl_easy *data,
                           ഘടന Curl_creader *reader, പൂർണ്ണസംഖ്യ premature)
{
  ഘടന cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_DATA;
  Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_crtype cr_exp100 = {
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
  വലുപ്പം(ഘടന cr_exp100_ctx)
};

സ്ഥിതം CURLcode http_exp100_add_reader(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *reader = ശൂന്യ;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_exp100, CURL_CR_PROTOCOL);
  എങ്കിൽ(!result)
    result = Curl_creader_add(data, reader);
  എങ്കിൽ(!result) {
    ഘടന cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  എങ്കിൽ(result && reader)
    Curl_creader_free(data, reader);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം http_exp100_got100(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  എങ്കിൽ(r)
    http_exp100_continue(data, r);
}

സ്ഥിതം bool http_exp100_is_waiting(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  എങ്കിൽ(r) {
    ഘടന cr_exp100_ctx *ctx = r->ctx;
    തിരികെ ctx->state == EXP100_AWAITING_CONTINUE;
  }
  തിരികെ FALSE;
}

സ്ഥിതം ശൂന്യം http_exp100_send_anyway(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  എങ്കിൽ(r)
    http_exp100_continue(data, r);
}

സ്ഥിതം bool http_exp100_is_selected(ഘടന Curl_easy *data)
{
  ഘടന Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  തിരികെ !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(ഘടന Curl_easy *data,
                                 ഘടന easy_pollset *ps)
{
  /* write mode */
  തിരികെ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(ഘടന Curl_easy *data,
                                   ഘടന easy_pollset *ps)
{
  ഘടന connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(CURL_REQ_WANT_RECV(data)) {
    result = Curl_pollset_add_in(data, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  എങ്കിൽ(!result && Curl_req_want_send(data) && !http_exp100_is_waiting(data)) {
    result = Curl_pollset_add_out(data, ps, conn->sock[FIRSTSOCKET]);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode http_write_header(ഘടന Curl_easy *data,
                                  സ്ഥിരം അക്ഷരം *hd, size_t hdlen)
{
  CURLcode result;
  പൂർണ്ണസംഖ്യ writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((data->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  result = Curl_client_write(data, writetype, hd, hdlen);
  എങ്കിൽ(result)
    തിരികെ result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  എങ്കിൽ(result)
    തിരികെ result;

  data->req.deductheadercount = (100 <= data->req.httpcode &&
                                 199 >= data->req.httpcode) ?
    data->req.headerbytecount : 0;
  തിരികെ result;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(ഘടന Curl_easy *data,
                        CURLcode status, bool premature)
{
  ഘടന connectdata *conn = data->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  data->state.authhost.multipass = FALSE;
  data->state.authproxy.multipass = FALSE;

  എങ്കിൽ(curlx_dyn_len(&data->state.headerb)) {
    (ശൂന്യം)http_write_header(data, curlx_dyn_ptr(&data->state.headerb),
                            curlx_dyn_len(&data->state.headerb));
  }
  curlx_dyn_reset(&data->state.headerb);

  എങ്കിൽ(status)
    തിരികെ status;

  എങ്കിൽ(!premature && /* this check is pointless when DONE is called before the
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
    തിരികെ CURLE_GOT_NOTHING;
  }

  തിരികെ CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
സ്ഥിതം bool http_may_use_1_1(സ്ഥിരം ഘടന Curl_easy *data)
{
  സ്ഥിരം ഘടന connectdata *conn = data->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  എങ്കിൽ(data->state.http_neg.rcvd_min == 10)
    തിരികെ FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  എങ്കിൽ(conn && conn->httpversion_seen == 10)
    തിരികെ FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  എങ്കിൽ(data->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    തിരികെ FALSE;
  /* We are not restricted to use 1.0 only. */
  തിരികെ !data->state.http_neg.only_10;
}

സ്ഥിതം unsigned അക്ഷരം http_request_version(ഘടന Curl_easy *data)
{
  unsigned അക്ഷരം v = Curl_conn_http_version(data, data->conn);
  എങ്കിൽ(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(data) ? 11 : 10;
  }
  തിരികെ v;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *get_http_string(പൂർണ്ണസംഖ്യ httpversion)
{
  തിരഞ്ഞെടുപ്പ്(httpversion) {
  അവസ്ഥ 30:
    തിരികെ "3";
  അവസ്ഥ 20:
    തിരികെ "2";
  അവസ്ഥ 11:
    തിരികെ "1.1";
  സ്ഥിരസ്ഥിതി:
    തിരികെ "1.0";
  }
}

CURLcode Curl_add_custom_headers(ഘടന Curl_easy *data,
                                 bool is_connect, പൂർണ്ണസംഖ്യ httpversion,
                                 ഘടന dynbuf *req)
{
  ഘടന curl_slist *h[2];
  ഘടന curl_slist *headers;
  പൂർണ്ണസംഖ്യ numlists = 1; /* by default */
  പൂർണ്ണസംഖ്യ i;

#ifndef CURL_DISABLE_PROXY
  എണ്ണൽ Curl_proxy_use proxy;

  എങ്കിൽ(is_connect)
    proxy = HEADER_CONNECT;
  അല്ലെങ്കിൽ
    proxy = data->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  തിരഞ്ഞെടുപ്പ്(proxy) {
  അവസ്ഥ HEADER_SERVER:
    h[0] = data->set.headers;
    നിർത്തുക;
  അവസ്ഥ HEADER_PROXY:
    h[0] = data->set.headers;
    എങ്കിൽ(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    നിർത്തുക;
  അവസ്ഥ HEADER_CONNECT:
    എങ്കിൽ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    അല്ലെങ്കിൽ
      h[0] = data->set.headers;
    നിർത്തുക;
  അവസ്ഥ HEADER_CONNECT_UDP:
    എങ്കിൽ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    അല്ലെങ്കിൽ
      h[0] = data->set.headers;
    നിർത്തുക;
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)is_connect;
  h[0] = data->set.headers;
#endif

  /* loop through one or two lists */
  വേണ്ടി(i = 0; i < numlists; i++) {
    വേണ്ടി(headers = h[i]; headers; headers = headers->next) {
      CURLcode result = CURLE_OK;
      bool blankheader = FALSE;
      ഘടന Curl_str name;
      സ്ഥിരം അക്ഷരം *p = headers->data;
      സ്ഥിരം അക്ഷരം *origp = p;
      size_t hlen = strlen(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      എങ്കിൽ(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      അല്ലെങ്കിൽ {
        p = origp;
        എങ്കിൽ(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          ഘടന Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          എങ്കിൽ(!curlx_strlen(&val))
            /* no content, do not send this */
            തുടരുക;
        }
        അല്ലെങ്കിൽ
          /* no colon */
          തുടരുക;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      എങ്കിൽ(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      അല്ലെങ്കിൽ എങ്കിൽ(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      അല്ലെങ്കിൽ എങ്കിൽ(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      അല്ലെങ്കിൽ എങ്കിൽ(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      അല്ലെങ്കിൽ എങ്കിൽ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      അല്ലെങ്കിൽ എങ്കിൽ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      അല്ലെങ്കിൽ എങ്കിൽ(blankheader) {
        result = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        എങ്കിൽ(!result)
          result = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      അല്ലെങ്കിൽ
        result = curlx_dyn_addf(req, "%s\r\n", origp);

      എങ്കിൽ(result)
        തിരികെ result;
    }
  }

  തിരികെ CURLE_OK;
}

#ifndef CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(ഘടന Curl_easy *data,
                                ഘടന dynbuf *req)
{
  സ്ഥിരം ഘടന tm *tm;
  ഘടന tm keeptime;
  CURLcode result;
  അക്ഷരം datestr[80];
  സ്ഥിരം അക്ഷരം *condp;
  size_t len;

  എങ്കിൽ(data->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    തിരികെ CURLE_OK;

  result = curlx_gmtime(data->set.timevalue, &keeptime);
  എങ്കിൽ(result) {
    failf(data, "Invalid TIMEVALUE");
    തിരികെ result;
  }
  tm = &keeptime;

  തിരഞ്ഞെടുപ്പ്(data->set.timecondition) {
  സ്ഥിരസ്ഥിതി:
    DEBUGF(infof(data, "invalid time condition"));
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  അവസ്ഥ CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    നിർത്തുക;
  അവസ്ഥ CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    നിർത്തുക;
  അവസ്ഥ CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    നിർത്തുക;
  }

  എങ്കിൽ(Curl_checkheaders(data, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    തിരികെ CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, വലുപ്പം(datestr),
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
  തിരികെ result;
}
#അല്ലെങ്കിൽ
/* disabled */
CURLcode Curl_add_timecondition(ഘടന Curl_easy *data,
                                ഘടന dynbuf *req)
{
  (ശൂന്യം)data;
  (ശൂന്യം)req;
  തിരികെ CURLE_OK;
}
#endif

ശൂന്യം Curl_http_method(ഘടന Curl_easy *data,
                      സ്ഥിരം അക്ഷരം **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)data->state.httpreq;
  സ്ഥിരം അക്ഷരം *request;
#ifndef CURL_DISABLE_WEBSOCKETS
  എങ്കിൽ(data->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  അല്ലെങ്കിൽ
#endif
  എങ്കിൽ((data->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     data->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  എങ്കിൽ(!data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(data->req.no_body)
      request = "HEAD";
    അല്ലെങ്കിൽ {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_HEAD));
      തിരഞ്ഞെടുപ്പ്(httpreq) {
      അവസ്ഥ HTTPREQ_POST:
      അവസ്ഥ HTTPREQ_POST_FORM:
      അവസ്ഥ HTTPREQ_POST_MIME:
        request = "POST";
        നിർത്തുക;
      അവസ്ഥ HTTPREQ_PUT:
        request = "PUT";
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി: /* this should never happen */
      അവസ്ഥ HTTPREQ_GET:
        request = "GET";
        നിർത്തുക;
      അവസ്ഥ HTTPREQ_HEAD:
        request = "HEAD";
        നിർത്തുക;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

സ്ഥിതം CURLcode http_set_aptr_host(ഘടന Curl_easy *data)
{
  ഘടന connectdata *conn = data->conn;
  സ്ഥിരം അക്ഷരം *ptr = ശൂന്യ;

  curlx_safefree(data->state.http_host);
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(data->req.cookiehost);
#endif

  എങ്കിൽ(Curl_peer_equal(data->state.initial_origin, data->state.origin))
    ptr = Curl_checkheaders(data, STRCONST("Host"));

  എങ്കിൽ(ptr) {
#ifndef CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    അക്ഷരം *cookiehost;
    CURLcode result = copy_custom_മൂല്യം(ptr, &cookiehost);
    എങ്കിൽ(result)
      തിരികെ result;
    എങ്കിൽ(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    അല്ലെങ്കിൽ {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      എങ്കിൽ(*cookiehost == '[') {
        അക്ഷരം *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, strlen(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        എങ്കിൽ(closingbracket)
          *closingbracket = 0;
      }
      അല്ലെങ്കിൽ {
        പൂർണ്ണസംഖ്യ startsearch = 0;
        അക്ഷരം *colon = strchr(cookiehost + startsearch, ':');
        എങ്കിൽ(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      data->req.cookiehost = cookiehost;
    }
#endif

    എങ്കിൽ(!curl_strequal("Host:", ptr)) {
      data->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      എങ്കിൽ(!data->state.http_host)
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
  }
  അല്ലെങ്കിൽ {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)data->state.origin->ipv6;
    ഘടന dynbuf tmp;
    size_t hlen;
    CURLcode result;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    result = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    എങ്കിൽ(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("["));
    എങ്കിൽ(!result)
      result = curlx_dyn_add(&tmp, data->state.origin->hostname);
    എങ്കിൽ(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("]"));
    എങ്കിൽ(!result &&
       ((data->state.origin->port != data->state.origin->scheme->defport) ||
       (data->state.origin->scheme->family != conn->scheme->family))) {
      result = curlx_dyn_addf(&tmp, ":%u", data->state.origin->port);
    }

    data->state.http_host = result ? ശൂന്യ : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    തിരികെ result;
  }
  തിരികെ CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
സ്ഥിതം CURLcode http_target(ഘടന Curl_easy *data,
                            ഘടന dynbuf *r)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *path = data->state.up.path;
  സ്ഥിരം അക്ഷരം *query = data->state.up.query;
#ifndef CURL_DISABLE_PROXY
  ഘടന connectdata *conn = data->conn;
#endif

  എങ്കിൽ(CURL_EASY_STR(data, STRING_TARGET)) {
    path = CURL_EASY_STR(data, STRING_TARGET);
    query = ശൂന്യ;
  }

#ifndef CURL_DISABLE_PROXY
  എങ്കിൽ(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    അക്ഷരം *url;
    CURLU *h = curl_url_dup(data->state.uh);
    എങ്കിൽ(!h)
      തിരികെ CURLE_OUT_OF_MEMORY;

    എങ്കിൽ(!data->state.origin->ipv6 &&
       (data->state.origin->user_hostname != data->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, data->state.origin->hostname, 0);
      എങ്കിൽ(uc) {
        curl_url_cleanup(h);
        തിരികെ CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, ശൂന്യ, 0);
    എങ്കിൽ(uc) {
      curl_url_cleanup(h);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }

    എങ്കിൽ(data->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, ശൂന്യ, 0);
      എങ്കിൽ(uc) {
        curl_url_cleanup(h);
        തിരികെ CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, ശൂന്യ, 0);
      എങ്കിൽ(uc) {
        curl_url_cleanup(h);
        തിരികെ CURLE_OUT_OF_MEMORY;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(data->state.creds && (data->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        data->state.creds->user, CURLU_URLENCODE);
      എങ്കിൽ(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          data->state.creds->passwd, CURLU_URLENCODE);
      എങ്കിൽ(uc) {
        curl_url_cleanup(h);
        തിരികെ Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    എങ്കിൽ(uc) {
      curl_url_cleanup(h);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    result = curlx_dyn_add(r, CURL_EASY_STR(data, STRING_TARGET) ?
      CURL_EASY_STR(data, STRING_TARGET) : url);
    curlx_free(url);
    എങ്കിൽ(result)
      തിരികെ result;

    എങ്കിൽ((data->state.origin->scheme == &Curl_scheme_ftp) &&
       data->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = strlen(path);
      bool type_present = FALSE;
      എങ്കിൽ((len >= 7) && !memcmp(&path[len - 7], ";type=", 6)) {
        തിരഞ്ഞെടുപ്പ്(Curl_raw_toupper(path[len - 1])) {
        അവസ്ഥ 'A':
        അവസ്ഥ 'D':
        അവസ്ഥ 'I':
          type_present = TRUE;
          നിർത്തുക;
        }
      }
      എങ്കിൽ(!type_present) {
        result = curlx_dyn_addf(r, ";type=%c",
                                data->state.prefer_ascii ? 'a' : 'i');
        എങ്കിൽ(result)
          തിരികെ result;
      }
    }
  }

  അല്ലെങ്കിൽ
#endif
  {
    result = curlx_dyn_add(r, path);
    എങ്കിൽ(result)
      തിരികെ result;
    എങ്കിൽ(query)
      result = curlx_dyn_addf(r, "?%s", query);
  }

  തിരികെ result;
}

#എങ്കിൽ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
സ്ഥിതം CURLcode set_post_reader(ഘടന Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result;

  തിരഞ്ഞെടുപ്പ്(httpreq) {
#ifndef CURL_DISABLE_MIME
  അവസ്ഥ HTTPREQ_POST_MIME:
    data->state.mimepost = data->set.mimepostp;
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_FORM_API
  അവസ്ഥ HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    എങ്കിൽ(!data->state.formp) {
      data->state.formp = curlx_calloc(1, വലുപ്പം(curl_mimepart));
      എങ്കിൽ(!data->state.formp)
        തിരികെ CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(data->state.formp);
      result = Curl_getformdata(data, data->state.formp, data->set.httppost,
                                data->state.fread_func);
      എങ്കിൽ(result) {
        curlx_safefree(data->state.formp);
        തിരികെ result;
      }
      data->state.mimepost = data->state.formp;
    }
    നിർത്തുക;
#endif
  സ്ഥിരസ്ഥിതി:
    data->state.mimepost = ശൂന്യ;
    നിർത്തുക;
  }

  തിരഞ്ഞെടുപ്പ്(httpreq) {
  അവസ്ഥ HTTPREQ_POST_FORM:
  അവസ്ഥ HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ifndef CURL_DISABLE_MIME
    എങ്കിൽ(data->state.mimepost) {
      സ്ഥിരം അക്ഷരം *cthdr = Curl_checkheaders(data, STRCONST("Content-Type"));

      /* Read and seek body only. */
      data->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      എങ്കിൽ(cthdr)
        വേണ്ടി(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      അല്ലെങ്കിൽ എങ്കിൽ(data->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(data->state.mimepost, data->set.headers, 0);
      result = Curl_mime_prepare_headers(data, data->state.mimepost, cthdr,
                                         ശൂന്യ, MIMESTRATEGY_FORM);
      എങ്കിൽ(result)
        തിരികെ result;
      curl_mime_headers(data->state.mimepost, ശൂന്യ, 0);
      result = Curl_creader_set_mime(data, data->state.mimepost);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    അല്ലെങ്കിൽ
#endif
    {
      result = Curl_creader_set_null(data);
    }
    data->state.infilesize = Curl_creader_total_length(data);
    തിരികെ result;

  സ്ഥിരസ്ഥിതി:
    തിരികെ Curl_creader_set_null(data);
  }
  /* never reached */
}
#endif

സ്ഥിതം CURLcode set_reader(ഘടന Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t postsize = data->state.infilesize;

  DEBUGASSERT(data->conn);

  എങ്കിൽ(data->req.authneg) {
    തിരികെ Curl_creader_set_null(data);
  }

  തിരഞ്ഞെടുപ്പ്(httpreq) {
  അവസ്ഥ HTTPREQ_PUT: /* Let's PUT the data to the server! */
    തിരികെ postsize ? Curl_creader_set_fread(data, postsize) :
      Curl_creader_set_null(data);

#എങ്കിൽ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  അവസ്ഥ HTTPREQ_POST_FORM:
  അവസ്ഥ HTTPREQ_POST_MIME:
    തിരികെ set_post_reader(data, httpreq);
#endif

  അവസ്ഥ HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    എങ്കിൽ(!postsize) {
      result = Curl_creader_set_null(data);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(data->set.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      എങ്കിൽ(plen == SIZE_MAX)
        തിരികെ CURLE_OUT_OF_MEMORY;
      അല്ലെങ്കിൽ എങ്കിൽ(plen)
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      അല്ലെങ്കിൽ
        result = Curl_creader_set_null(data);
    }
    അല്ലെങ്കിൽ {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      അക്ഷരം *ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
      എങ്കിൽ(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      result = Curl_creader_set_fread(data, chunked ? -1 : postsize);
    }
    തിരികെ result;

  സ്ഥിരസ്ഥിതി:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    data->state.infilesize = 0;
    തിരികെ Curl_creader_set_null(data);
  }
  /* not reached */
}

സ്ഥിതം CURLcode http_resume(ഘടന Curl_easy *data, Curl_HttpReq httpreq)
{
  എങ്കിൽ((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     data->state.resume_from) {
    failf(data, "HTTP upload cannot be resumed");
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode http_req_set_TE(ഘടന Curl_easy *data,
                                ഘടന dynbuf *req,
                                പൂർണ്ണസംഖ്യ httpversion)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *ptr;

  ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
  എങ്കിൽ(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    data->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    എങ്കിൽ(data->req.upload_chunky && (httpversion >= 20)) {
      infof(data, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      data->req.upload_chunky = FALSE;
    }
  }
  അല്ലെങ്കിൽ {
    curl_off_t req_clen = Curl_creader_total_length(data);

    എങ്കിൽ(req_clen < 0) {
      /* indeterminate request content length */
      എങ്കിൽ(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        data->req.upload_chunky = (httpversion < 20);
      }
      അല്ലെങ്കിൽ {
        failf(data, "Chunky upload is not supported by HTTP 1.0");
        തിരികെ CURLE_UPLOAD_FAILED;
      }
    }
    അല്ലെങ്കിൽ {
      /* else, no chunky upload */
      data->req.upload_chunky = FALSE;
    }

    എങ്കിൽ(data->req.upload_chunky)
      result = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode addexpect(ഘടന Curl_easy *data, ഘടന dynbuf *r,
                          പൂർണ്ണസംഖ്യ httpversion, bool *announced_exp100)
{
  CURLcode result;
  അക്ഷരം *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  എങ്കിൽ(data->req.upgr101 != UPGR101_NONE)
    തിരികെ CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(data, STRCONST("Expect"));
  എങ്കിൽ(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!data->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_length(data);
    എങ്കിൽ(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      result = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      എങ്കിൽ(result)
        തിരികെ result;
      *announced_exp100 = TRUE;
    }
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode http_add_content_hds(ഘടന Curl_easy *data,
                                     ഘടന dynbuf *r,
                                     പൂർണ്ണസംഖ്യ httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(data->conn);
  എങ്കിൽ(data->req.upload_chunky) {
    result = Curl_httpchunk_add_reader(data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_total_length(data);
  തിരഞ്ഞെടുപ്പ്(httpreq) {
  അവസ്ഥ HTTPREQ_PUT:
  അവസ്ഥ HTTPREQ_POST:
#എങ്കിൽ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  അവസ്ഥ HTTPREQ_POST_FORM:
  അവസ്ഥ HTTPREQ_POST_MIME:
#endif
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    എങ്കിൽ(req_clen >= 0 && !data->req.upload_chunky &&
       (data->req.authneg ||
        !Curl_checkheaders(data, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      result = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    എങ്കിൽ(result)
      പോകുക out;

#ifndef CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    എങ്കിൽ(data->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      ഘടന curl_slist *hdr;

      വേണ്ടി(hdr = data->state.mimepost->curlheaders; hdr; hdr = hdr->next) {
        result = curlx_dyn_addf(r, "%s\r\n", hdr->data);
        എങ്കിൽ(result)
          പോകുക out;
      }
    }
#endif
    എങ്കിൽ(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(data, STRCONST("Content-Type"))) {
      result = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      എങ്കിൽ(result)
        പോകുക out;
    }
    result = addexpect(data, r, httpversion, &announced_exp100);
    എങ്കിൽ(result)
      പോകുക out;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }

  Curl_pgrsSetUploadSize(data, req_clen);
  എങ്കിൽ(announced_exp100)
    result = http_exp100_add_reader(data);

out:
  തിരികെ result;
}

#ifndef CURL_DISABLE_COOKIES

സ്ഥിതം CURLcode http_cookies(ഘടന Curl_easy *data,
                             ഘടന dynbuf *r)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *addcookies = ശൂന്യ;
  bool linecap = FALSE;
  എങ്കിൽ(CURL_EASY_STR(data, STRING_COOKIE) &&
     !Curl_checkheaders(data, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(data))
    addcookies = CURL_EASY_STR(data, STRING_COOKIE);

  എങ്കിൽ(data->cookies || addcookies) {
    ഘടന Curl_llist list;
    പൂർണ്ണസംഖ്യ എണ്ണം = 0;

    എങ്കിൽ(data->cookies && data->state.cookie_engine) {
      bool okay;
      സ്ഥിരം അക്ഷരം *host = data->req.cookiehost ?
        data->req.cookiehost : data->state.origin->hostname;
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      result = Curl_cookie_getlist(data, &okay, host, &list);
      എങ്കിൽ(!result && okay) {
        ഘടന Curl_llist_കണ്ണി *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        വേണ്ടി(n = Curl_llist_head(&list); n; n = Curl_കണ്ണി_next(n)) {
          ഘടന Cookie *co = Curl_കണ്ണി_elem(n);
          എങ്കിൽ(co->മൂല്യം) {
            size_t add;
            എങ്കിൽ(!എണ്ണം) {
              result = curlx_dyn_addn(r, STRCONST("Cookie: "));
              എങ്കിൽ(result)
                നിർത്തുക;
            }
            add = strlen(co->name) + strlen(co->മൂല്യം) + 1;
            എങ്കിൽ(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(data, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              നിർത്തുക;
            }
            result = curlx_dyn_addf(r, "%s%s=%s", എണ്ണം ? "; " : "",
                                    co->name, co->മൂല്യം);
            എങ്കിൽ(result)
              നിർത്തുക;
            clen += add + (എണ്ണം ? 2 : 0);
            എണ്ണം++;
          }
        }
        Curl_llist_destroy(&list, ശൂന്യ);
      }
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
    എങ്കിൽ(addcookies && !result && !linecap) {
      എങ്കിൽ(!എണ്ണം)
        result = curlx_dyn_addn(r, STRCONST("Cookie: "));
      എങ്കിൽ(!result) {
        result = curlx_dyn_addf(r, "%s%s", എണ്ണം ? "; " : "", addcookies);
        എണ്ണം++;
      }
    }
    എങ്കിൽ(എണ്ണം && !result)
      result = curlx_dyn_addn(r, STRCONST("\r\n"));

    എങ്കിൽ(result)
      തിരികെ result;
  }
  തിരികെ result;
}
#അല്ലെങ്കിൽ
#define http_cookies(a, b) CURLE_OK
#endif

സ്ഥിതം CURLcode http_range(ഘടന Curl_easy *data,
                           Curl_HttpReq httpreq)
{
  എങ്കിൽ(data->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    എങ്കിൽ(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_HEAD)) &&
       !Curl_checkheaders(data, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);
      data->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 data->state.range);
      എങ്കിൽ(!data->state.rangeline)
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(data, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_total_length(data);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);

      എങ്കിൽ(data->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      അല്ലെങ്കിൽ എങ്കിൽ(data->state.resume_from) {
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
      അല്ലെങ്കിൽ {
        /* Range was selected and then we pass the incoming range and append
           total size */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        data->state.range, req_clen);
      }
      എങ്കിൽ(!data->state.rangeline)
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode http_firstwrite(ഘടന Curl_easy *data)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന SingleRequest *k = &data->req;

  എങ്കിൽ(data->req.newurl) {
    എങ്കിൽ(conn->bits.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      തിരികെ CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(data, "Ignoring the response-body");
  }
  എങ്കിൽ(data->state.resume_from && !k->content_range &&
     (data->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    എങ്കിൽ(k->size == data->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(data, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      തിരികെ CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(data, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    തിരികെ CURLE_RANGE_ERROR;
  }

  എങ്കിൽ(data->set.timecondition && !data->state.range &&
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
    തിരികെ CURLE_OK;
  } /* we have a time condition */

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode http_check_new_conn(ഘടന Curl_easy *data)
{
  ഘടന connectdata *conn = data->conn;
  സ്ഥിരം അക്ഷരം *info_version = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *alpn;
  CURLcode result;

  alpn = Curl_conn_get_alpn_negotiated(data, conn);
  എങ്കിൽ(alpn && !strcmp("h3", alpn)) {
#ifndef CURL_DISABLE_PROXY
    എങ്കിൽ(!conn->bits.origin_is_proxy)
#endif
      DEBUGASSERT(Curl_conn_http_version(data, conn) == 30);
    info_version = "HTTP/3";
  }
  അല്ലെങ്കിൽ എങ്കിൽ(alpn && !strcmp("h2", alpn)) {
#ifndef CURL_DISABLE_PROXY
    എങ്കിൽ((Curl_conn_http_version(data, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      result = Curl_http2_switch(data);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    അല്ലെങ്കിൽ
#endif
    DEBUGASSERT(Curl_conn_http_version(data, conn) == 20);
    info_version = "HTTP/2";
  }
  അല്ലെങ്കിൽ {
    /* Check if user wants to use HTTP/2 with clear TCP */
    എങ്കിൽ(Curl_http2_may_switch(data)) {
      DEBUGF(infof(data, "HTTP/2 over clean TCP"));
      result = Curl_http2_switch(data);
      എങ്കിൽ(result)
        തിരികെ result;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    അല്ലെങ്കിൽ
      info_version = "HTTP/1.x";
  }

  എങ്കിൽ(info_version)
    infof(data, "using %s", info_version);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode http_add_connection_hd(ഘടന Curl_easy *data,
                                       ഘടന dynbuf *req)
{
  ഘടന curl_slist *head;
  സ്ഥിരം അക്ഷരം *sep = "Connection: ";
  CURLcode result = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  വേണ്ടി(head = data->set.headers; head; head = head->next) {
    എങ്കിൽ(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      അക്ഷരം *മൂല്യം;
      result = copy_custom_മൂല്യം(head->data, &മൂല്യം);
      എങ്കിൽ(result)
        തിരികെ result;
      result = curlx_dyn_addf(req, "%s%s", sep, മൂല്യം);
      sep = ", ";
      curlx_free(മൂല്യം);
      നിർത്തുക; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  എങ്കിൽ(!result && data->state.http_hd_te) {
    result = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  എങ്കിൽ(!result && data->state.http_hd_upgrade) {
    result = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  എങ്കിൽ(!result && data->state.http_hd_h2_settings) {
    result = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  എങ്കിൽ(!result && (rlen < curlx_dyn_len(req)))
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
  എങ്കിൽ(result)
    തിരികെ result;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  വേണ്ടി(head = data->set.headers; head; head = head->next) {
    എങ്കിൽ(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      എങ്കിൽ(skip) {
        skip = FALSE;
        തുടരുക;
      }
      result = curlx_dyn_addf(req, "%s\r\n", head->data);
      എങ്കിൽ(result)
        തിരികെ result;
    }
  }

  തിരികെ CURLE_OK;
}

/* Header identifier in order we send them by default */
തരനിർവചനം എണ്ണൽ {
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

സ്ഥിതം CURLcode http_add_hd(ഘടന Curl_easy *data,
                            ഘടന dynbuf *req,
                            http_hd_t id,
                            unsigned അക്ഷരം httpversion,
                            സ്ഥിരം അക്ഷരം *method,
                            Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
#എങ്കിൽ !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  ഘടന connectdata *conn = data->conn;
#endif
  തിരഞ്ഞെടുപ്പ്(id) {
  അവസ്ഥ H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    result = curlx_dyn_addf(req, "%s ", method);
    എങ്കിൽ(!result)
      result = http_target(data, req);
    എങ്കിൽ(!result)
      result = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    നിർത്തുക;

  അവസ്ഥ H1_HD_HOST:
    എങ്കിൽ(data->state.http_host) {
      result = curlx_dyn_add(req, data->state.http_host);
      എങ്കിൽ(!result)
        result = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    നിർത്തുക;

#ifndef CURL_DISABLE_PROXY
  അവസ്ഥ H1_HD_PROXY_AUTH:
    എങ്കിൽ(data->req.hd_proxy_auth)
      result = curlx_dyn_add(req, data->req.hd_proxy_auth);
    നിർത്തുക;
#endif

  അവസ്ഥ H1_HD_AUTH:
    എങ്കിൽ(data->req.hd_auth)
      result = curlx_dyn_add(req, data->req.hd_auth);
    നിർത്തുക;

  അവസ്ഥ H1_HD_RANGE:
    എങ്കിൽ(data->state.use_range && data->state.rangeline)
      result = curlx_dyn_add(req, data->state.rangeline);
    നിർത്തുക;

  അവസ്ഥ H1_HD_USER_AGENT: {
    സ്ഥിരം അക്ഷരം *ua = CURL_EASY_STR(data, STRING_USERAGENT);
    എങ്കിൽ(ua && *ua && !Curl_checkheaders(data, STRCONST("User-Agent")))
      result = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    നിർത്തുക;
  }

  അവസ്ഥ H1_HD_ACCEPT:
    എങ്കിൽ(!Curl_checkheaders(data, STRCONST("Accept")))
      result = curlx_dyn_add(req, "Accept: */*\r\n");
    നിർത്തുക;

  അവസ്ഥ H1_HD_TE:
#ifdef HAVE_LIBZ
    എങ്കിൽ(!Curl_checkheaders(data, STRCONST("TE")) &&
       data->set.http_transfer_encoding) {
      data->state.http_hd_te = TRUE;
      result = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#endif
    നിർത്തുക;

  അവസ്ഥ H1_HD_ACCEPT_ENCODING: {
    സ്ഥിരം അക്ഷരം *enc = CURL_EASY_STR(data, STRING_ENCODING);
    എങ്കിൽ(enc && !Curl_checkheaders(data, STRCONST("Accept-Encoding")))
      result = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    നിർത്തുക;
  }

  അവസ്ഥ H1_HD_REFERER:
    എങ്കിൽ(Curl_bufref_ptr(&data->state.referer) &&
       !Curl_checkheaders(data, STRCONST("Referer")))
      result = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&data->state.referer));
    നിർത്തുക;

#ifndef CURL_DISABLE_PROXY
  അവസ്ഥ H1_HD_PROXY_CONNECTION:
    എങ്കിൽ(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(data, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(data, data->conn, STRCONST("Proxy-Connection")))
      result = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    നിർത്തുക;
#endif

  അവസ്ഥ H1_HD_TRANSFER_ENCODING:
    result = http_req_set_TE(data, req, httpversion);
    നിർത്തുക;

#ifndef CURL_DISABLE_ALTSVC
  അവസ്ഥ H1_HD_ALT_USED:
    എങ്കിൽ(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(data, STRCONST("Alt-Used")))
      result = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    നിർത്തുക;
#endif

  അവസ്ഥ H1_HD_UPGRADE:
    എങ്കിൽ(!Curl_conn_is_ssl(data->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
       data->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      result = Curl_http2_request_upgrade(req, data);
    }
#ifndef CURL_DISABLE_WEBSOCKETS
    എങ്കിൽ(!result && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      result = Curl_ws_request(data, req);
#endif
    നിർത്തുക;

  അവസ്ഥ H1_HD_COOKIES:
    result = http_cookies(data, req);
    നിർത്തുക;

  അവസ്ഥ H1_HD_CONDITIONALS:
    result = Curl_add_timecondition(data, req);
    നിർത്തുക;

  അവസ്ഥ H1_HD_CUSTOM:
    result = Curl_add_custom_headers(data, FALSE, httpversion, req);
    നിർത്തുക;

  അവസ്ഥ H1_HD_CONTENT:
    result = http_add_content_hds(data, req, httpversion, httpreq);
    നിർത്തുക;

  അവസ്ഥ H1_HD_CONNECTION: {
    result = http_add_connection_hd(data, req);
    നിർത്തുക;
  }

  അവസ്ഥ H1_HD_LAST:
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
    നിർത്തുക;
  }
  തിരികെ result;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  Curl_HttpReq httpreq;
  സ്ഥിരം അക്ഷരം *method;
  ഘടന dynbuf req;
  unsigned അക്ഷരം httpversion;
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

  എങ്കിൽ(!data->conn->bits.reuse) {
    result = http_check_new_conn(data);
    എങ്കിൽ(result)
      പോകുക out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  result = Curl_headers_init(data);
  എങ്കിൽ(result)
    പോകുക out;

  data->state.http_hd_te = FALSE;
  data->state.http_hd_upgrade = FALSE;
  data->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(data, &method, &httpreq);

  /* select host to send */
  result = http_set_aptr_host(data);
  /* setup the authentication headers, how that method and host are known */
  എങ്കിൽ(!result)
    result = Curl_http_output_auth(data, data->conn, method, httpreq,
                                   data->state.up.path,
                                   data->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  എങ്കിൽ(!result)
    result = set_reader(data, httpreq);
  എങ്കിൽ(!result)
    result = http_resume(data, httpreq);
  എങ്കിൽ(!result)
    result = http_range(data, httpreq);
  എങ്കിൽ(result)
    പോകുക out;

  httpversion = http_request_version(data);
  /* Add request line and all headers to `req` */
  വേണ്ടി(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    result = http_add_hd(data, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    എങ്കിൽ(result)
      പോകുക out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);
  result = Curl_req_send(data, &req, httpversion);

  എങ്കിൽ((httpversion >= 20) && data->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    data->req.upload_chunky = FALSE;

out:
  എങ്കിൽ(result == CURLE_TOO_LARGE)
    failf(data, "HTTP request too large");

  curlx_dyn_free(&req);
  തിരികെ result;
}

തരനിർവചനം എണ്ണൽ {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
സ്ഥിതം bool checkprefixmax(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *buffer, size_t len)
{
  size_t ch = CURLMIN(strlen(prefix), len);
  തിരികെ curl_strnequal(prefix, buffer, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
സ്ഥിതം statusline checkhttpprefix(ഘടന Curl_easy *data,
                                  സ്ഥിരം അക്ഷരം *s, size_t len)
{
  ഘടന curl_slist *head = data->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  ആകുമ്പോൾ(head) {
    എങ്കിൽ(checkprefixmax(head->data, s, len)) {
      rc = onmatch;
      നിർത്തുക;
    }
    head = head->next;
  }

  എങ്കിൽ((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  തിരികെ rc;
}

#ifndef CURL_DISABLE_RTSP
സ്ഥിതം statusline checkrtspprefix(ഘടന Curl_easy *data,
                                  സ്ഥിരം അക്ഷരം *s, size_t len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (ശൂന്യം)data;
  എങ്കിൽ(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  തിരികെ status;
}
#endif /* CURL_DISABLE_RTSP */

സ്ഥിതം statusline checkprotoprefix(ഘടന Curl_easy *data,
                                   ഘടന connectdata *conn,
                                   സ്ഥിരം അക്ഷരം *s, size_t len)
{
#ifndef CURL_DISABLE_RTSP
  എങ്കിൽ(conn->scheme->protocol & CURLPROTO_RTSP)
    തിരികെ checkrtspprefix(data, s, len);
#അല്ലെങ്കിൽ
  (ശൂന്യം)conn;
#endif /* CURL_DISABLE_RTSP */

  തിരികെ checkhttpprefix(data, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#define HD_IS(hd, hdlen, n) \
  (((hdlen) >= (വലുപ്പം(n) - 1)) && curl_strnequal(n, hd, വലുപ്പം(n) - 1))

#define HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (വലുപ്പം(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, വലുപ്പം(n) - 1)) ? ((hd) + (വലുപ്പം(n) - 1)) : ശൂന്യ)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#define HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((വലുപ്പം(n) - 1) + (വലുപ്പം(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
സ്ഥിതം CURLcode http_header_a(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *hd, size_t hdlen)
{
#ifndef CURL_DISABLE_ALTSVC
  സ്ഥിരം അക്ഷരം *v;
  v = (data->asi &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#അല്ലെങ്കിൽ
        0
#endif
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : ശൂന്യ;
  എങ്കിൽ(v) {
    /* the ALPN of the current request */
    ഘടന SingleRequest *k = &data->req;
    എണ്ണൽ alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    തിരികെ Curl_altsvc_parse(data, data->asi, v, data->state.origin, id);
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)hd;
  (ശൂന്യം)hdlen;
#endif
  തിരികെ CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
സ്ഥിതം CURLcode http_header_c(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *hd, size_t hdlen)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന SingleRequest *k = &data->req;
  സ്ഥിരം അക്ഷരം *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !data->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : ശൂന്യ;
  എങ്കിൽ(v) {
    ചെയ്യുക {
      curl_off_t contentlength;
      പൂർണ്ണസംഖ്യ offt = curlx_str_numblanks(&v, &contentlength);

      എങ്കിൽ(offt == STRE_OVERFLOW) {
        /* out of range */
        എങ്കിൽ(data->set.max_filesize) {
          failf(data, "Maximum file size exceeded");
          തിരികെ CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(data, "Overflow Content-Length: value");
        തിരികെ CURLE_OK;
      }
      അല്ലെങ്കിൽ {
        എങ്കിൽ((offt == STRE_OK) &&
           ((k->size == -1) || /* not set to something before */
            (k->size == contentlength))) { /* or the same value */

          k->size = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          എങ്കിൽ(!curlx_str_single(&v, ','))
            തുടരുക;

          എങ്കിൽ(!curlx_str_newline(&v)) {
            k->maxdownload = k->size;
            തിരികെ CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(data, "Invalid Content-Length: value");
        തിരികെ CURLE_WEIRD_SERVER_REPLY;
      }
    } ആകുമ്പോൾ(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(data, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : ശൂന്യ;
  എങ്കിൽ(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    തിരികെ Curl_build_unencoding_അട്ടി(data, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  എങ്കിൽ(v) {
    അക്ഷരം *contenttype = Curl_copy_header_മൂല്യം(hd);
    എങ്കിൽ(!contenttype)
      തിരികെ CURLE_OUT_OF_MEMORY;
    എങ്കിൽ(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    അല്ലെങ്കിൽ {
      curlx_free(data->info.contenttype);
      data->info.contenttype = contenttype;
    }
    തിരികെ CURLE_OK;
  }
  എങ്കിൽ((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    തിരികെ CURLE_OK;
  }
  എങ്കിൽ((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(data, "HTTP/1.0 connection set to keep alive");
    തിരികെ CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : ശൂന്യ;
  എങ്കിൽ(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    സ്ഥിരം അക്ഷരം *ptr = v;

    /* Move forward until first digit or asterisk */
    ആകുമ്പോൾ(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    എങ്കിൽ(ISDIGIT(*ptr)) {
      എങ്കിൽ(!curlx_str_സംഖ്യ(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (data->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(k->httpcode < 300)
      data->state.resume_from = 0; /* get everything */
  }
  തിരികെ CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
സ്ഥിതം CURLcode http_header_l(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *hd, size_t hdlen)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന SingleRequest *k = &data->req;
  സ്ഥിരം അക്ഷരം *v = (!k->http_bodyless &&
                   (data->set.timecondition || data->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : ശൂന്യ;
  എങ്കിൽ(v) {
    എങ്കിൽ(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    എങ്കിൽ(data->set.get_filetime)
      data->info.filetime = k->timeofdoc;
    തിരികെ CURLE_OK;
  }
  എങ്കിൽ(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    അക്ഷരം *location = Curl_copy_header_മൂല്യം(hd);
    എങ്കിൽ(!location)
      തിരികെ CURLE_OUT_OF_MEMORY;
    എങ്കിൽ(!*location ||
       (data->req.location && !strcmp(data->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      തിരികെ CURLE_OK;
    }
    അല്ലെങ്കിൽ {
      /* has value and is not an exact repeat */
      എങ്കിൽ(data->req.location) {
        failf(data, "Multiple Location headers");
        curlx_free(location);
        തിരികെ CURLE_WEIRD_SERVER_REPLY;
      }
      data->req.location = location;

      എങ്കിൽ((k->httpcode >= 300 && k->httpcode < 400) &&
         data->set.http_follow_mode) {
        CURLcode result;
        DEBUGASSERT(!data->req.newurl);
        data->req.newurl = curlx_strdup(data->req.location); /* clone */
        എങ്കിൽ(!data->req.newurl)
          തിരികെ CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        result = http_perhapsrewind(data, conn);
        എങ്കിൽ(result)
          തിരികെ result;

        /* mark the next request as a followed location: */
        data->state.this_is_a_follow = TRUE;
      }
    }
  }
  തിരികെ CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
സ്ഥിതം CURLcode http_header_p(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *hd, size_t hdlen)
{
  ഘടന SingleRequest *k = &data->req;

#ifndef CURL_DISABLE_PROXY
  സ്ഥിരം അക്ഷരം *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  എങ്കിൽ(v) {
    ഘടന connectdata *conn = data->conn;
    എങ്കിൽ((k->httpversion == 10) && conn->http_proxy.peer &&
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
    അല്ലെങ്കിൽ എങ്കിൽ((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(data, "HTTP/1.1 proxy connection set close");
    }
    തിരികെ CURLE_OK;
  }
#endif
  എങ്കിൽ((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    അക്ഷരം *auth;
    CURLcode result;
#ifndef CURL_DISABLE_PROXY
    എങ്കിൽ(!data->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      തിരികെ CURLE_OK;
#endif
    auth = Curl_copy_header_മൂല്യം(hd);
    result = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    എങ്കിൽ(!result) {
      result = Curl_http_നിവേശം_auth(data, TRUE, auth);
      curlx_free(auth);
    }
    തിരികെ result;
  }
#ifdef USE_SPNEGO
  എങ്കിൽ(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    ഘടന connectdata *conn = data->conn;
    ഘടന negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    ഘടന auth *authp = &data->state.authhost;
    എങ്കിൽ(!negdata)
      തിരികെ CURLE_OUT_OF_MEMORY;
    എങ്കിൽ(authp->picked == CURLAUTH_NEGOTIATE) {
      അക്ഷരം *persistentauth = Curl_copy_header_മൂല്യം(hd);
      എങ്കിൽ(!persistentauth)
        തിരികെ CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(data, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#endif
  തിരികെ CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
സ്ഥിതം CURLcode http_header_r(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *hd, size_t hdlen)
{
  സ്ഥിരം അക്ഷരം *v = HD_VAL(hd, hdlen, "Retry-After:");
  എങ്കിൽ(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    എങ്കിൽ(!Curl_getdate_capped(v, &date)) {
      time_t current = time(ശൂന്യ);
      എങ്കിൽ(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    അല്ലെങ്കിൽ
      /* Try it as a decimal number, ignore errors */
      (ശൂന്യം)curlx_str_സംഖ്യ(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    എങ്കിൽ(retry_after > 21600)
      retry_after = 21600;
    data->info.retry_after = retry_after;
  }
  തിരികെ CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
സ്ഥിതം CURLcode http_header_s(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *hd, size_t hdlen)
{
#എങ്കിൽ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  സ്ഥിരം അക്ഷരം *v;
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)hd;
  (ശൂന്യം)hdlen;
#endif

#ifndef CURL_DISABLE_COOKIES
  v = (data->cookies && data->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : ശൂന്യ;
  എങ്കിൽ(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    സ്ഥിരം അക്ഷരം *host = data->req.cookiehost ?
      data->req.cookiehost : data->state.origin->hostname;
    സ്ഥിരം unsigned അക്ഷരം secure_context = Curl_secure_context(data, host) ?
      COOKIE_SECURE : 0;
    CURLcode result;
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    result = Curl_cookie_add(data, data->cookies, v, host,
                             data->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    തിരികെ result;
  }
#endif
#ifndef CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (data->hsts &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#അല്ലെങ്കിൽ
        0
#endif
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : ശൂന്യ;
  എങ്കിൽ(v) {
    CURLcode result = Curl_hsts_parse(
      data->hsts, data->state.origin->hostname, v);
    എങ്കിൽ(result) {
      എങ്കിൽ(result == CURLE_OUT_OF_MEMORY)
        തിരികെ result;
      infof(data, "Illegal STS header skipped");
    }
#ifdef DEBUGBUILD
    അല്ലെങ്കിൽ
      infof(data, "Parsed STS header fine (%zu entries)",
            Curl_llist_എണ്ണം(&data->hsts->list));
#endif
  }
#endif

  തിരികെ CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
സ്ഥിതം CURLcode http_header_t(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *hd, size_t hdlen)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന SingleRequest *k = &data->req;

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
  സ്ഥിരം അക്ഷരം *v = (!k->http_bodyless &&
                   (data->state.httpreq != HTTPREQ_HEAD) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : ശൂന്യ;
  എങ്കിൽ(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode result = Curl_build_unencoding_അട്ടി(data, v, TRUE);
    എങ്കിൽ(result)
      തിരികെ result;
    എങ്കിൽ(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(data, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    തിരികെ CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  എങ്കിൽ(v) {
    data->req.resp_trailer = TRUE;
    തിരികെ CURLE_OK;
  }
  തിരികെ CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
സ്ഥിതം CURLcode http_header_w(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *hd, size_t hdlen)
{
  ഘടന SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  എങ്കിൽ((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    അക്ഷരം *auth = Curl_copy_header_മൂല്യം(hd);
    എങ്കിൽ(!auth)
      result = CURLE_OUT_OF_MEMORY;
    അല്ലെങ്കിൽ {
      result = Curl_http_നിവേശം_auth(data, FALSE, auth);
      curlx_free(auth);
    }
  }
  തിരികെ result;
}

/*
 * http_header() parses a single response header.
 */
സ്ഥിതം CURLcode http_header(ഘടന Curl_easy *data,
                            സ്ഥിരം അക്ഷരം *hd, size_t hdlen)
{
  CURLcode result = CURLE_OK;

  തിരഞ്ഞെടുപ്പ്(hd[0]) {
  അവസ്ഥ 'a':
  അവസ്ഥ 'A':
    result = http_header_a(data, hd, hdlen);
    നിർത്തുക;
  അവസ്ഥ 'c':
  അവസ്ഥ 'C':
    result = http_header_c(data, hd, hdlen);
    നിർത്തുക;
  അവസ്ഥ 'l':
  അവസ്ഥ 'L':
    result = http_header_l(data, hd, hdlen);
    നിർത്തുക;
  അവസ്ഥ 'p':
  അവസ്ഥ 'P':
    result = http_header_p(data, hd, hdlen);
    നിർത്തുക;
  അവസ്ഥ 'r':
  അവസ്ഥ 'R':
    result = http_header_r(data, hd, hdlen);
    നിർത്തുക;
  അവസ്ഥ 's':
  അവസ്ഥ 'S':
    result = http_header_s(data, hd, hdlen);
    നിർത്തുക;
  അവസ്ഥ 't':
  അവസ്ഥ 'T':
    result = http_header_t(data, hd, hdlen);
    നിർത്തുക;
  അവസ്ഥ 'w':
  അവസ്ഥ 'W':
    result = http_header_w(data, hd, hdlen);
    നിർത്തുക;
  }

  എങ്കിൽ(!result) {
    ഘടന connectdata *conn = data->conn;
    എങ്കിൽ(conn->scheme->protocol & CURLPROTO_RTSP)
      result = Curl_rtsp_parseheader(data, hd);
  }
  തിരികെ result;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
സ്ഥിതം CURLcode http_statusline(ഘടന Curl_easy *data,
                                ഘടന connectdata *conn)
{
  ഘടന SingleRequest *k = &data->req;

  തിരഞ്ഞെടുപ്പ്(k->httpversion) {
  അവസ്ഥ 10:
  അവസ്ഥ 11:
#ifdef USE_HTTP2
  അവസ്ഥ 20:
#endif
#ifdef USE_HTTP3
  അവസ്ഥ 30:
#endif
    /* no major version switch mid-connection */
    എങ്കിൽ(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(data, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      തിരികെ CURLE_WEIRD_SERVER_REPLY;
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    failf(data, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
  }

  data->info.httpcode = k->httpcode;
  data->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  എങ്കിൽ(!data->state.http_neg.rcvd_min ||
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
  എങ്കിൽ(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  എങ്കിൽ(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(data, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  തിരഞ്ഞെടുപ്പ്(k->httpcode) {
  അവസ്ഥ 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    എങ്കിൽ(data->set.timecondition)
      data->info.timecond = TRUE;
    FALLTHROUGH();
  അവസ്ഥ 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->size = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
സ്ഥിതം CURLcode http_size(ഘടന Curl_easy *data)
{
  ഘടന SingleRequest *k = &data->req;
  എങ്കിൽ(data->req.ignore_cl || k->chunk) {
    k->size = k->maxdownload = -1;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(k->size != -1) {
    എങ്കിൽ(data->set.max_filesize &&
       !k->ignorebody &&
       (k->size > data->set.max_filesize)) {
      failf(data, "Maximum file size exceeded");
      തിരികെ CURLE_FILESIZE_EXCEEDED;
    }
    എങ്കിൽ(k->ignorebody)
      infof(data, "setting size while ignoring");
    Curl_pgrsSetDownloadSize(data, k->size);
    k->maxdownload = k->size;
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_verify_header(ഘടന Curl_easy *data,
                            സ്ഥിരം അക്ഷരം *hd, size_t hdlen)
{
  ഘടന SingleRequest *k = &data->req;
  size_t i;
  bool has_colon = FALSE;
  വേണ്ടി(i = 0; i < hdlen; i++) {
    തിരഞ്ഞെടുപ്പ്(hd[i]) {
    അവസ്ഥ '\0':
      /* this is bad, bail out */
      failf(data, "NUL byte in header");
      തിരികെ CURLE_WEIRD_SERVER_REPLY;
    അവസ്ഥ '\r':
      എങ്കിൽ(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(data, "Carriage return found in header");
        തിരികെ CURLE_WEIRD_SERVER_REPLY;
      }
      നിർത്തുക;
    അവസ്ഥ ':':
      has_colon = TRUE;
      നിർത്തുക;
    }
  }
  എങ്കിൽ(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    തിരികെ CURLE_OK;
  എങ്കിൽ(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  അല്ലെങ്കിൽ എങ്കിൽ(!has_colon) {
    /* this is bad, bail out */
    failf(data, "Header without colon");
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_bump_headersize(ഘടന Curl_easy *data,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  unsigned പൂർണ്ണസംഖ്യ max = MAX_HTTP_RESP_HEADER_SIZE;
  എങ്കിൽ(delta < MAX_HTTP_RESP_HEADER_SIZE) {
    data->info.header_size += (unsigned പൂർണ്ണസംഖ്യ)delta;
    data->req.allheadercount += (unsigned പൂർണ്ണസംഖ്യ)delta;
    എങ്കിൽ(!connect_only)
      data->req.headerbytecount += (unsigned പൂർണ്ണസംഖ്യ)delta;
    എങ്കിൽ(data->req.allheadercount > max)
      bad = data->req.allheadercount;
    അല്ലെങ്കിൽ എങ്കിൽ(data->info.header_size > (max * 20)) {
      bad = data->info.header_size;
      max *= 20;
    }
  }
  അല്ലെങ്കിൽ
    bad = data->req.allheadercount + delta;
  എങ്കിൽ(bad) {
    failf(data, "Too large response headers: %zu > %u", bad, max);
    തിരികെ CURLE_RECV_ERROR;
  }
  തിരികെ CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
സ്ഥിതം CURLcode http_on_101_upgrade(ഘടന Curl_easy *data,
                                    സ്ഥിരം അക്ഷരം *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_changed)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന SingleRequest *k = &data->req;

#എങ്കിൽ !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (ശൂന്യം)buf;
  (ശൂന്യം)blen;
  (ശൂന്യം)pconsumed;
#അല്ലെങ്കിൽ
  CURLcode result;
  പൂർണ്ണസംഖ്യ upgr101_requested = k->upgr101;
#endif

  എങ്കിൽ(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(data, "server sent 101 response while not talking HTTP/1.1");
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->bits.upgrade_in_progress = FALSE;
  *conn_changed = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ifdef USE_NGHTTP2
  എങ്കിൽ(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(data, "Received 101, Switching to HTTP/2");
    result = Curl_http2_upgrade(data, conn, FIRSTSOCKET, buf, blen);
    എങ്കിൽ(!result)
      *pconsumed += blen;
    തിരികെ result;
  }
#endif
#ifndef CURL_DISABLE_WEBSOCKETS
  എങ്കിൽ(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(data, "Received 101, Switching to WebSocket");
    result = Curl_ws_accept(data, buf, blen);
    എങ്കിൽ(!result)
      *pconsumed += blen; /* ws accept handled the data */
    തിരികെ result;
  }
#endif
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  തിരികെ CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
സ്ഥിതം CURLcode http_on_1xx_response(ഘടന Curl_easy *data,
                                     സ്ഥിരം അക്ഷരം *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_changed)
{
  ഘടന SingleRequest *k = &data->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  തിരഞ്ഞെടുപ്പ്(k->httpcode) {
  അവസ്ഥ 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(data);
    നിർത്തുക;
  അവസ്ഥ 101:
    തിരികെ http_on_101_upgrade(data, buf, blen, pconsumed, conn_changed);
  സ്ഥിരസ്ഥിതി:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

#എങ്കിൽ defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
സ്ഥിതം ശൂന്യം http_check_auth_closure(ഘടന Curl_easy *data,
                                    ഘടന connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ifdef USE_NTLM
  എങ്കിൽ(conn->bits.close &&
     (((data->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((data->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
#endif
#ifdef USE_SPNEGO
  എങ്കിൽ(conn->bits.close &&
    (((data->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((data->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
  എങ്കിൽ((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  എങ്കിൽ((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#endif
}
#അല്ലെങ്കിൽ
#define http_check_auth_closure(x, y) /* empty */
#endif

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
സ്ഥിതം CURLcode http_handle_send_error(ഘടന Curl_easy *data)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!data->req.authneg && !conn->bits.close &&
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

    തിരഞ്ഞെടുപ്പ്(data->state.httpreq) {
    അവസ്ഥ HTTPREQ_PUT:
    അവസ്ഥ HTTPREQ_POST:
    അവസ്ഥ HTTPREQ_POST_FORM:
    അവസ്ഥ HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      എങ്കിൽ(!Curl_req_done_sending(data)) {
        എങ്കിൽ((k->httpcode == 417) && http_exp100_is_selected(data)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          എങ്കിൽ(!k->writebytecount && http_exp100_is_waiting(data)) {
            infof(data, "Got HTTP failure 417 while waiting for a 100");
          }
          അല്ലെങ്കിൽ {
            infof(data, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            result = http_perhapsrewind(data, conn);
            എങ്കിൽ(result)
              തിരികെ result;
          }
          data->state.disableexpect = TRUE;
          Curl_req_abort_sending(data);
          DEBUGASSERT(!data->req.newurl);
          data->req.newurl = Curl_bufref_dup(&data->state.url);
          എങ്കിൽ(!data->req.newurl)
            തിരികെ CURLE_OUT_OF_MEMORY;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(data->set.http_keep_sending_on_error) {
          infof(data, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(data);
        }
        അല്ലെങ്കിൽ {
          infof(data, "HTTP error before end of send, stop sending");
          streamclose(conn);
          result = Curl_req_abort_sending(data);
          എങ്കിൽ(result)
            തിരികെ result;
        }
      }
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി: /* default label present to avoid compiler warnings */
      നിർത്തുക;
    }
  }

  എങ്കിൽ(Curl_creader_will_rewind(data) && !Curl_req_done_sending(data)) {
    /* We rewind before next send, continue sending now */
    infof(data, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(data);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode http_on_response(ഘടന Curl_easy *data,
                                 സ്ഥിരം അക്ഷരം *last_hd, size_t last_hd_len,
                                 സ്ഥിരം അക്ഷരം *buf, size_t blen,
                                 size_t *pconsumed)
{
  ഘടന connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  ഘടന SingleRequest *k = &data->req;
  bool conn_changed = FALSE;

  (ശൂന്യം)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  എങ്കിൽ(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    എങ്കിൽ(data->req.httpversion != 20)
      infof(data, "Lying server, not serving HTTP/2");
  }

  എങ്കിൽ(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    result = http_write_header(data, last_hd, last_hd_len);
    last_hd = ശൂന്യ; /* handled it */
    എങ്കിൽ(result)
      പോകുക out;
  }

  എങ്കിൽ(k->httpcode < 100) {
    failf(data, "Unsupported response code in HTTP response");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(k->httpcode < 200) {
    result = http_on_1xx_response(data, buf, blen, pconsumed, &conn_changed);
    പോകുക out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  എങ്കിൽ(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  എങ്കിൽ((k->size == -1) && !k->chunk && !conn->bits.close &&
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
  എങ്കിൽ(data->req.upgr101 == UPGR101_WS) {
    failf(data, "Refused WebSocket upgrade: %d", k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    പോകുക out;
  }
#endif

  /* Check if this response means the transfer errored. */
  എങ്കിൽ(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    പോകുക out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  result = Curl_http_auth_act(data);
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ(k->httpcode >= 300) {
    result = http_handle_send_error(data);
    എങ്കിൽ(result)
      പോകുക out;
  }

  /* final response without error, prepare to receive the body */
  result = http_firstwrite(data);
  എങ്കിൽ(result)
    പോകുക out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  result = http_size(data);
  എങ്കിൽ(result)
    പോകുക out;

#ifndef CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  എങ്കിൽ((conn->scheme->protocol & CURLPROTO_RTSP) && data->req.no_body &&
     !data->set.opt_no_body && (k->size > 0))
    data->req.no_body = FALSE;
#endif

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  എങ്കിൽ(data->req.no_body)
    k->download_done = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  എങ്കിൽ((k->maxdownload == 0) && (k->httpversion_sent < 20))
    k->download_done = TRUE;

out:
  എങ്കിൽ(last_hd)
    /* if not written yet, write it now */
    result = Curl_1st_fatal(result,
                            http_write_header(data, last_hd, last_hd_len));
  എങ്കിൽ(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(data->multi);
  തിരികെ result;
}

സ്ഥിതം CURLcode http_rw_hd(ഘടന Curl_easy *data,
                           സ്ഥിരം അക്ഷരം *hd, size_t hdlen,
                           സ്ഥിരം അക്ഷരം *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  ഘടന SingleRequest *k = &data->req;
  പൂർണ്ണസംഖ്യ writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  എങ്കിൽ((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    ഘടന dynbuf last_header;
    size_t consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    result = curlx_dyn_addn(&last_header, hd, hdlen);
    എങ്കിൽ(result)
      തിരികെ result;

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
    തിരികെ result;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_HEADER;
  എങ്കിൽ(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    എങ്കിൽ(data->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      സ്ഥിരം അക്ഷരം *p = hd;

      curlx_str_passblanks(&p);
      എങ്കിൽ(!strncmp(p, "HTTP/", 5)) {
        p += 5;
        തിരഞ്ഞെടുപ്പ്(*p) {
        അവസ്ഥ '1':
          p++;
          എങ്കിൽ((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            എങ്കിൽ(ISBLANK(p[2])) {
              k->httpversion = (unsigned അക്ഷരം)(10 + (p[1] - '0'));
              p += 3;
              എങ്കിൽ(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          എങ്കിൽ(!fine_statusline) {
            failf(data, "Unsupported HTTP/1 subversion in response");
            തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
          }
          നിർത്തുക;
        അവസ്ഥ '2':
        അവസ്ഥ '3':
          എങ്കിൽ(!ISBLANK(p[1]))
            നിർത്തുക;
          k->httpversion = (unsigned അക്ഷരം)((*p - '0') * 10);
          p += 2;
          എങ്കിൽ(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            എങ്കിൽ(!ISBLANK(*p))
              നിർത്തുക;
            fine_statusline = TRUE;
          }
          നിർത്തുക;
        സ്ഥിരസ്ഥിതി: /* unsupported */
          failf(data, "Unsupported HTTP version in response");
          തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      എങ്കിൽ(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(data, hd, hdlen);
        എങ്കിൽ(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(data->conn->scheme->protocol & CURLPROTO_RTSP) {
      സ്ഥിരം അക്ഷരം *p = hd;
      ഘടന Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      എങ്കിൽ(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_സംഖ്യ(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        എങ്കിൽ(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (പൂർണ്ണസംഖ്യ)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      എങ്കിൽ(!fine_statusline)
        തിരികെ CURLE_WEIRD_SERVER_REPLY;
    }

    എങ്കിൽ(fine_statusline) {
      result = http_statusline(data, data->conn);
      എങ്കിൽ(result)
        തിരികെ result;
      writetype |= CLIENTWRITE_STATUS;
    }
    അല്ലെങ്കിൽ {
      k->header = FALSE;   /* this is not a header line */
      തിരികെ CURLE_WEIRD_SERVER_REPLY;
    }
  }

  result = Curl_verify_header(data, hd, hdlen);
  എങ്കിൽ(result)
    തിരികെ result;

  result = http_header(data, hd, hdlen);
  എങ്കിൽ(result)
    തിരികെ result;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  എങ്കിൽ(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  result = Curl_client_write(data, writetype, hd, hdlen);
  എങ്കിൽ(result)
    തിരികെ result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  എങ്കിൽ(result)
    തിരികെ result;

  തിരികെ CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
ശൂന്യം Curl_http_to_fold(ഘടന dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  സ്ഥിരം അക്ഷരം *hd = curlx_dyn_ptr(bf);
  എങ്കിൽ(len && (hd[len - 1] == '\n'))
    len--;
  എങ്കിൽ(len && (hd[len - 1] == '\r'))
    len--;
  ആകുമ്പോൾ(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

സ്ഥിതം ശൂന്യം unfold_header(ഘടന Curl_easy *data)
{
  Curl_http_to_fold(&data->state.headerb);
  data->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
സ്ഥിതം CURLcode http_parse_headers(ഘടന Curl_easy *data,
                                   സ്ഥിരം അക്ഷരം *buf, size_t blen,
                                   size_t *pconsumed)
{
  ഘടന connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  ഘടന SingleRequest *k = &data->req;
  സ്ഥിരം അക്ഷരം *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  എങ്കിൽ(data->state.maybe_folded && blen) {
    എങ്കിൽ(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(data);
    }
    അല്ലെങ്കിൽ {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      result = http_rw_hd(data, curlx_dyn_ptr(&data->state.headerb),
                          curlx_dyn_len(&data->state.headerb),
                          ശൂന്യ, 0, &ignore_this);
      curlx_dyn_reset(&data->state.headerb);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    data->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  ആകുമ്പോൾ(blen && k->header) {
    size_t consumed;
    size_t hlen;
    സ്ഥിരം അക്ഷരം *hd;
    size_t unfold_len = 0;

    എങ്കിൽ(data->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      ആകുമ്പോൾ(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      എങ്കിൽ(blen) {
        /* insert a single space */
        result = curlx_dyn_addn(&data->state.headerb, " ", 1);
        എങ്കിൽ(result)
          തിരികെ result;
        data->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = memchr(buf, '\n', blen);
    എങ്കിൽ(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      result = curlx_dyn_addn(&data->state.headerb, buf, blen);
      എങ്കിൽ(result)
        തിരികെ result;
      *pconsumed += blen + unfold_len;

      എങ്കിൽ(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(data, conn,
                           curlx_dyn_ptr(&data->state.headerb),
                           curlx_dyn_len(&data->state.headerb));

        എങ്കിൽ(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          എങ്കിൽ((k->httpversion >= 10) || conn->bits.reuse) {
            failf(data, "Invalid status line");
            തിരികെ CURLE_WEIRD_SERVER_REPLY;
          }
          എങ്കിൽ(!data->state.http_neg.accept_09) {
            failf(data, "Received HTTP/0.9 when not allowed");
            തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          പോകുക out;
        }
      }
      പോകുക out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    result = curlx_dyn_addn(&data->state.headerb, buf, consumed);
    എങ്കിൽ(result)
      തിരികെ result;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&data->state.headerb);
    hd = curlx_dyn_ptr(&data->state.headerb);

    എങ്കിൽ(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(data, conn, hd, hlen);
      എങ്കിൽ(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        എങ്കിൽ((k->httpversion >= 10) || conn->bits.reuse) {
          failf(data, "Invalid status line");
          തിരികെ CURLE_WEIRD_SERVER_REPLY;
        }
        എങ്കിൽ(!data->state.http_neg.accept_09) {
          failf(data, "Received HTTP/0.9 when not allowed");
          തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        പോകുക out;
      }
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        എങ്കിൽ(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(data);
          തുടരുക;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          data->state.maybe_folded = TRUE;
          നിർത്തുക;
        }
      }
    }

    result = http_rw_hd(data, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&data->state.headerb);
    എങ്കിൽ(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    എങ്കിൽ(result)
      തിരികെ result;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  എങ്കിൽ(!k->header && !leftover_body) {
    curlx_dyn_free(&data->state.headerb);
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(ഘടന Curl_easy *data,
                                 സ്ഥിരം അക്ഷരം *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode result;
  size_t consumed;
  അക്ഷരം tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  result = http_rw_hd(data, hd, hdlen, &tmp, 0, &consumed);
  എങ്കിൽ(!result && is_eos) {
    result = Curl_client_write(data, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  തിരികെ result;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(ഘടന Curl_easy *data,
                                  സ്ഥിരം അക്ഷരം *buf, size_t blen,
                                  size_t *pconsumed)
{
  എങ്കിൽ(!data->req.header) {
    *pconsumed = 0;
    തിരികെ CURLE_OK;
  }
  അല്ലെങ്കിൽ {
    CURLcode result;

    result = http_parse_headers(data, buf, blen, pconsumed);
    എങ്കിൽ(!result && !data->req.header) {
      എങ്കിൽ(!data->req.no_body && curlx_dyn_len(&data->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&data->state.headerb),
                                   curlx_dyn_len(&data->state.headerb));
      }
      curlx_dyn_free(&data->state.headerb);
    }
    തിരികെ result;
  }
}

CURLcode Curl_http_write_resp(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result;
  size_t consumed;
  പൂർണ്ണസംഖ്യ flags;

  result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
  എങ്കിൽ(result || data->req.done)
    പോകുക out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !data->req.header);
  എങ്കിൽ(!data->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    എങ്കിൽ(is_eos)
      flags |= CLIENTWRITE_EOS;
    result = Curl_client_write(data, flags, buf, blen);
  }
out:
  തിരികെ result;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, സ്ഥിരം അക്ഷരം *s, size_t len)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  പൂർണ്ണസംഖ്യ i;

  എങ്കിൽ(len != 3)
    പോകുക out;

  വേണ്ടി(i = 0; i < 3; ++i) {
    അക്ഷരം c = s[i];

    എങ്കിൽ(c < '0' || c > '9')
      പോകുക out;

    status *= 10;
    status += c - '0';
  }
  result = CURLE_OK;
out:
  *pstatus = result ? -1 : status;
  തിരികെ result;
}

CURLcode Curl_http_req_make(ഘടന httpreq **preq,
                            സ്ഥിരം അക്ഷരം *method, size_t m_len,
                            സ്ഥിരം അക്ഷരം *scheme, size_t s_len,
                            സ്ഥിരം അക്ഷരം *authority, size_t a_len,
                            സ്ഥിരം അക്ഷരം *path, size_t p_len)
{
  ഘടന httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, വലുപ്പം(*req) + m_len);
  എങ്കിൽ(!req)
    പോകുക out;
#എങ്കിൽ defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic തള്ളുക
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memcpy(req->method, method, m_len);
#എങ്കിൽ defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic എടുക്കുക
#endif
  എങ്കിൽ(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    എങ്കിൽ(!req->scheme)
      പോകുക out;
  }
  എങ്കിൽ(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    എങ്കിൽ(!req->authority)
      പോകുക out;
  }
  എങ്കിൽ(path) {
    req->path = curlx_memdup0(path, p_len);
    എങ്കിൽ(!req->path)
      പോകുക out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  എങ്കിൽ(result && req)
    Curl_http_req_free(req);
  *preq = result ? ശൂന്യ : req;
  തിരികെ result;
}

സ്ഥിതം CURLcode req_assign_url_authority(ഘടന httpreq *req, CURLU *url)
{
  അക്ഷരം *host, *port;
  ഘടന dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  host = port = ശൂന്യ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  എങ്കിൽ(uc && uc != CURLUE_NO_HOST)
    പോകുക out;
  എങ്കിൽ(!host) {
    req->authority = ശൂന്യ;
    result = CURLE_OK;
    പോകുക out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  എങ്കിൽ(uc && uc != CURLUE_NO_PORT)
    പോകുക out;

  result = curlx_dyn_add(&buf, host);
  എങ്കിൽ(result)
    പോകുക out;
  എങ്കിൽ(port) {
    result = curlx_dyn_addf(&buf, ":%s", port);
    എങ്കിൽ(result)
      പോകുക out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  എങ്കിൽ(result)
    curlx_dyn_free(&buf);
  തിരികെ result;
}

സ്ഥിതം CURLcode req_assign_url_path(ഘടന httpreq *req, CURLU *url)
{
  അക്ഷരം *path, *query;
  ഘടന dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  path = query = ശൂന്യ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  എങ്കിൽ(uc)
    പോകുക out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  എങ്കിൽ(uc && uc != CURLUE_NO_QUERY)
    പോകുക out;

  എങ്കിൽ(!query) {
    req->path = path;
    path = ശൂന്യ;
  }
  അല്ലെങ്കിൽ {
    result = curlx_dyn_add(&buf, path);
    എങ്കിൽ(!result)
      result = curlx_dyn_addf(&buf, "?%s", query);
    എങ്കിൽ(result)
      പോകുക out;
    req->path = curlx_dyn_ptr(&buf);
  }
  result = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  എങ്കിൽ(result)
    curlx_dyn_free(&buf);
  തിരികെ result;
}

CURLcode Curl_http_req_make2(ഘടന httpreq **preq,
                             സ്ഥിരം അക്ഷരം *method, size_t m_len,
                             CURLU *url, സ്ഥിരം അക്ഷരം *scheme_default)
{
  ഘടന httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, വലുപ്പം(*req) + m_len);
  എങ്കിൽ(!req)
    പോകുക out;
  memcpy(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  എങ്കിൽ(uc && uc != CURLUE_NO_SCHEME)
    പോകുക out;
  എങ്കിൽ(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    എങ്കിൽ(!req->scheme)
      പോകുക out;
  }

  result = req_assign_url_authority(req, url);
  എങ്കിൽ(result)
    പോകുക out;
  result = req_assign_url_path(req, url);
  എങ്കിൽ(result)
    പോകുക out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  എങ്കിൽ(result && req)
    Curl_http_req_free(req);
  *preq = result ? ശൂന്യ : req;
  തിരികെ result;
}

ശൂന്യം Curl_http_req_free(ഘടന httpreq *req)
{
  എങ്കിൽ(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->path);
    Curl_dynhds_free(&req->headers);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

ഘടന name_const {
  സ്ഥിരം അക്ഷരം *name;
  size_t namelen;
};

സ്ഥിതം സ്ഥിരം ഘടന name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

സ്ഥിതം bool h2_permissible_field(ഘടന dynhds_entry *e)
{
  size_t i;
  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    എങ്കിൽ(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      തിരികെ FALSE;
  }
  തിരികെ TRUE;
}

സ്ഥിതം bool http_TE_has_token(സ്ഥിരം അക്ഷരം *fvalue, സ്ഥിരം അക്ഷരം *token)
{
  ആകുമ്പോൾ(*fvalue) {
    ഘടന Curl_str name;

    /* skip to first token */
    ആകുമ്പോൾ(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    എങ്കിൽ(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      തിരികെ FALSE;
    എങ്കിൽ(curlx_str_casecompare(&name, token))
      തിരികെ TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    ആകുമ്പോൾ(*fvalue && *fvalue != ',') {
      എങ്കിൽ(*fvalue == '"') {
        ഘടന Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        എങ്കിൽ(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          തിരികെ FALSE;
      }
      അല്ലെങ്കിൽ
        fvalue++;
    }
  }
  തിരികെ FALSE;
}

CURLcode Curl_http_req_to_h2(ഘടന dynhds *h2_headers,
                             ഘടന httpreq *req, ഘടന Curl_easy *data)
{
  സ്ഥിരം അക്ഷരം *scheme = ശൂന്യ, *authority = ശൂന്യ;
  ഘടന dynhds_entry *e;
  size_t i;
  CURLcode result;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  എങ്കിൽ(req->scheme) {
    scheme = req->scheme;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(strcmp("CONNECT", req->method)) {
    scheme = Curl_checkheaders(data, STRCONST(HTTP_PSEUDO_SCHEME));
    എങ്കിൽ(scheme) {
      scheme += വലുപ്പം(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(data, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    അല്ലെങ്കിൽ {
      scheme = data->state.origin->scheme->name;
    }
  }

  എങ്കിൽ(req->authority) {
    authority = req->authority;
  }
  അല്ലെങ്കിൽ {
    e = Curl_dynhds_get(&req->headers, STRCONST("Host"));
    എങ്കിൽ(e)
      authority = e->മൂല്യം;
  }

  Curl_dynhds_reset(h2_headers);
  Curl_dynhds_set_opts(h2_headers, DYNHDS_OPT_LOWERCASE);
  result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, strlen(req->method));
  എങ്കിൽ(!result && scheme) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, strlen(scheme));
  }
  എങ്കിൽ(!result && authority) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, strlen(authority));
  }
  എങ്കിൽ(!result && req->path) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, strlen(req->path));
  }
  വേണ്ടി(i = 0; !result && i < Curl_dynhds_എണ്ണം(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    എങ്കിൽ(e->namelen == 2 && curl_strequal("TE", e->name)) {
      എങ്കിൽ(http_TE_has_token(e->മൂല്യം, "trailers"))
        result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    അല്ലെങ്കിൽ എങ്കിൽ(h2_permissible_field(e)) {
      result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->മൂല്യം, e->valuelen);
    }
  }

  തിരികെ result;
}

CURLcode Curl_http_resp_make(ഘടന http_resp **presp,
                             പൂർണ്ണസംഖ്യ status,
                             സ്ഥിരം അക്ഷരം *description)
{
  ഘടന http_resp *resp;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, വലുപ്പം(*resp));
  എങ്കിൽ(!resp)
    പോകുക out;

  resp->status = status;
  എങ്കിൽ(description) {
    resp->description = curlx_strdup(description);
    എങ്കിൽ(!resp->description)
      പോകുക out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  എങ്കിൽ(result && resp)
    Curl_http_resp_free(resp);
  *presp = result ? ശൂന്യ : resp;
  തിരികെ result;
}

ശൂന്യം Curl_http_resp_free(ഘടന http_resp *resp)
{
  എങ്കിൽ(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    എങ്കിൽ(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_http = {
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
