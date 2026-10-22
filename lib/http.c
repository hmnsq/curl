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
#ضمن "curl_setup.h"
#ضمن "urldata.h"

#ifndef CURL_DISABLE_HTTP

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif

#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ضمن <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#endif

#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "formdata.h"
#ضمن "mime.h"
#ضمن "progress.h"
#ضمن "curlx/base64.h"
#ضمن "cookie.h"
#ضمن "vauth/vauth.h"
#ضمن "vquic/vquic.h"
#ضمن "http_digest.h"
#ضمن "http_ntlm.h"
#ضمن "http_negotiate.h"
#ضمن "http_aws_sigv4.h"
#ضمن "http_httpsig.h"
#ضمن "url.h"
#ضمن "urlapi-int.h"
#ضمن "curl_share.h"
#ضمن "dynhds.h"
#ضمن "http.h"
#ضمن "headers.h"
#ضمن "select.h"
#ضمن "parsedate.h" /* for the week day and month names */
#ضمن "multiif.h"
#ضمن "strcase.h"
#ضمن "content_encoding.h"
#ضمن "http_proxy.h"
#ضمن "http2.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "curlx/strdup.h"
#ضمن "altsvc.h"
#ضمن "hsts.h"
#ضمن "rtsp.h"
#ضمن "ws.h"
#ضمن "bufref.h"
#ضمن "curlx/strparse.h"

فراغ Curl_http_neg_init(بنية Curl_easy *data, بنية http_negotiation *neg)
{
  memset(neg, 0, حجم(*neg));
  neg->accept_09 = data->set.http09_allowed;
  بدل(data->set.httpwant) {
  حالة CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    اكسر;
  حالة CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    اكسر;
  حالة CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    اكسر;
  حالة CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    اكسر;
  حالة CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    data->state.http_neg.h2_prior_knowledge = TRUE;
    اكسر;
  حالة CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    اكسر;
  حالة CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    اكسر;
  حالة CURL_HTTP_VERSION_NONE:
  افتراضي:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    اكسر;
  }
}

CURLcode Curl_http_setup_conn(بنية Curl_easy *data,
                              بنية connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  إذا(data->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode result = Curl_conn_may_http3(data, conn);
    إذا(result)
      أرجع result;
  }
  أرجع CURLE_OK;
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
حرف *Curl_checkProxyheaders(بنية Curl_easy *data,
                             ثابت بنية connectdata *conn,
                             ثابت حرف *thisheader,
                             ثابت size_t thislen)
{
  بنية curl_slist *head;

  لكل(head = (conn->http_proxy.peer && data->set.sep_headers) ?
        data->set.proxyheaders : data->set.headers;
      head; head = head->next) {
    إذا(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      أرجع head->data;
  }

  أرجع عدم;
}
#endif

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
ساكن bool header_has_قدر(ثابت حرف **headerp, بنية Curl_str *outp)
{
  bool قدر = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  إذا(قدر) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  أرجع قدر;
}

ساكن bool http_header_is_empty(ثابت حرف *header)
{
  بنية Curl_str out;

  إذا(header_has_قدر(&header, &out)) {
    أرجع curlx_strlen(&out) == 0;
  }
  أرجع TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
ساكن CURLcode copy_custom_قدر(ثابت حرف *header, حرف **valp)
{
  بنية Curl_str out = { 0 };

  /* find the end of the header name */
  إذا(header_has_قدر(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    إذا(*valp)
      أرجع CURLE_OK;
    أرجع CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = عدم;
  أرجع CURLE_BAD_FUNCTION_ARGUMENT;
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
حرف *Curl_copy_header_قدر(ثابت حرف *header)
{
  بنية Curl_str out;

  /* find the end of the header name */
  إذا(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_SIZE, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_SIZE);
    curlx_str_trimblanks(&out);
    أرجع curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  أرجع عدم;
}

#ifndef CURL_DISABLE_HTTP_AUTH

#ifndef CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
ساكن CURLcode http_output_basic(بنية Curl_easy *data,
                                  بنية connectdata *conn, bool proxy)
{
  size_t size = 0;
  حرف *authorization = عدم;
  حرف **p_hd;
  CURLcode result;
  بنية Curl_creds *creds = عدم;
  حرف *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  إذا(proxy) {
#ifndef CURL_DISABLE_PROXY
    p_hd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#وإلا
    (فراغ)conn;
    أرجع CURLE_NOT_BUILT_IN;
#endif
  }
  وإلا {
    p_hd = &data->req.hd_auth;
    creds = data->state.creds;
  }

  إذا(!creds) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  إذا(!out)
    أرجع CURLE_OUT_OF_MEMORY;

  result = curlx_base64_encode((uint8_t *)out, strlen(out),
                               &authorization, &size);
  إذا(result)
    اذهب fail;

  إذا(!authorization) {
    result = CURLE_REMOTE_ACCESS_DENIED;
    اذهب fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  إذا(!*p_hd) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب fail;
  }

fail:
  curlx_free(out);
  أرجع result;
}

#endif

#ifndef CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
ساكن CURLcode http_output_bearer(بنية Curl_easy *data)
{
  حرف **userp;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(data->state.creds));
  userp = &data->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(data->state.creds));

  إذا(!*userp) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب fail;
  }

fail:
  أرجع result;
}
#endif

#endif

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
ساكن bool pickoneauth(بنية auth *pick, unsigned long mask,
                        بنية Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  unsigned long avail = pick->avail & pick->want & mask;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  إذا(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ifndef CURL_DISABLE_BEARER_AUTH
  وإلا إذا((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  وإلا إذا((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#endif
  وإلا إذا(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ifndef CURL_DISABLE_BASIC_AUTH
  وإلا إذا((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#endif
#ifndef CURL_DISABLE_AWS
  وإلا إذا(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  وإلا إذا(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#endif
  وإلا {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  أرجع picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
ساكن CURLcode http_perhapsrewind(بنية Curl_easy *data,
                                   بنية connectdata *conn)
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
  VERBOSE(ثابت حرف *ongoing_auth = عدم);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  إذا(needs_rewind) {
    infof(data, "Need to rewind upload for next request");
    Curl_creader_set_rewind(data, TRUE);
  }

  إذا(conn->bits.close)
    /* If we already decided to close this connection, we cannot veto. */
    أرجع CURLE_OK;

  إذا(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ifdef USE_NTLM
    إذا((data->state.authproxy.picked == CURLAUTH_NTLM) ||
       (data->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      إذا((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
#ifdef USE_SPNEGO
    /* There is still data left to send */
    إذا((data->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (data->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      إذا((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
  }

  إذا(abort_upload) {
    إذا(upload_remain >= 0)
      infof(data, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    وإلا
      infof(data, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    data->req.size = 0; /* do not download any more than 0 bytes */
    data->req.http_bodyless = TRUE;
  }
  أرجع CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
ساكن bool http_should_fail(بنية Curl_easy *data, صحيح httpcode)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  إذا(!data->set.http_fail_on_error)
    أرجع FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  إذا(httpcode < 400)
    أرجع FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  إذا(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    أرجع FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  إذا((httpcode != 401) && (httpcode != 407))
    أرجع TRUE;

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
  إذا((httpcode == 401) && !data->state.creds)
    أرجع TRUE;
#ifndef CURL_DISABLE_PROXY
  إذا((httpcode == 407) && !data->conn->http_proxy.creds)
    أرجع TRUE;
#endif

  أرجع (bool)data->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(بنية Curl_easy *data)
{
  بنية connectdata *conn = data->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode result = CURLE_OK;
  unsigned long authmask = ~0UL;

  إذا(!Curl_creds_has_oauth_bearer(data->state.creds))
    authmask &= (unsigned long)~CURLAUTH_BEARER;

  إذا(100 <= data->req.httpcode && data->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    أرجع CURLE_OK;

  إذا(data->state.authproblem)
    أرجع data->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  إذا(data->state.creds &&
     ((data->req.httpcode == 401) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickhost = pickoneauth(&data->state.authhost, authmask, data->state.creds);
    إذا(!pickhost)
      data->state.authproblem = TRUE;
    وإلا
      data->info.httpauthpicked = data->state.authhost.picked;
    إذا(data->state.authhost.picked == CURLAUTH_NTLM &&
       (data->req.httpversion_sent > 11)) {
      infof(data, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ifndef CURL_DISABLE_PROXY
  إذا(conn->http_proxy.creds &&
     ((data->req.httpcode == 407) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickproxy = pickoneauth(&data->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    إذا(!pickproxy)
      data->state.authproblem = TRUE;
    وإلا
      data->info.proxyauthpicked = data->state.authproxy.picked;
  }
#endif

  إذا(pickhost || pickproxy) {
    result = http_perhapsrewind(data, conn);
    إذا(result)
      أرجع result;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(data->req.newurl);
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    إذا(!data->req.newurl)
      أرجع CURLE_OUT_OF_MEMORY;
  }
  وإلا إذا((data->req.httpcode < 300) &&
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
    إذا(!data->req.newurl)
      أرجع CURLE_OUT_OF_MEMORY;
    data->state.authhost.done = TRUE;
  }
  إذا(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          data->req.httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
  }

  أرجع result;
}

#ifndef CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
ساكن CURLcode output_auth_headers(بنية Curl_easy *data,
                                    بنية connectdata *conn,
                                    بنية auth *authstatus,
                                    ثابت حرف *request,
                                    ثابت حرف *path,
                                    bool proxy)
{
  ثابت حرف *auth = عدم;
  CURLcode result = CURLE_OK;
  (فراغ)conn;

#ifdef CURL_DISABLE_DIGEST_AUTH
  (فراغ)request;
  (فراغ)path;
#endif
#ifndef CURL_DISABLE_AWS
  إذا((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    result = Curl_output_aws_sigv4(data);
    إذا(result)
      أرجع result;
  }
  وإلا
#endif
#ifndef CURL_DISABLE_HTTPSIG
  إذا((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    إذا(Curl_auth_allowed_to_host(data)) {
      auth = "HTTPSIG";
      result = Curl_output_httpsig(data);
      إذا(result)
        أرجع result;
    }
    وإلا
      authstatus->done = TRUE;
  }
  وإلا
#endif
#ifdef USE_SPNEGO
  إذا(authstatus->picked == CURLAUTH_NEGOTIATE) {
    إذا(
#ifndef CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(data, conn,
                                        STRCONST("Proxy-authorization"))) ||
#endif
      (!proxy && !Curl_checkheaders(data, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(data))) {
      auth = "Negotiate";
      result = Curl_output_negotiate(data, conn, proxy);
      إذا(result)
        أرجع result;
    }
    وإلا
      authstatus->done = TRUE;
  }
  وإلا
#endif
#ifdef USE_NTLM
  إذا(authstatus->picked == CURLAUTH_NTLM) {
    إذا(proxy || Curl_auth_allowed_to_host(data)) {
      auth = "NTLM";
      result = Curl_output_ntlm(data, proxy);
      إذا(result)
        أرجع result;
    }
    وإلا
      authstatus->done = TRUE;
  }
  وإلا
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  إذا(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    result = Curl_output_digest(data,
                                proxy,
                                (ثابت unsigned حرف *)request,
                                (ثابت unsigned حرف *)path);
    إذا(result)
      أرجع result;
  }
  وإلا
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
  إذا(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    إذا(
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
      إذا(result)
        أرجع result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
  إذا(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    إذا(!proxy && Curl_creds_has_oauth_bearer(data->state.creds) &&
       !Curl_checkheaders(data, STRCONST("Authorization"))) {
      auth = "Bearer";
      result = http_output_bearer(data);
      إذا(result)
        أرجع result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif

  إذا(auth) {
#ifndef CURL_DISABLE_PROXY
    إذا(proxy)
      data->info.proxyauthpicked = authstatus->picked;
    وإلا
      data->info.httpauthpicked = authstatus->picked;
    infof(data, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (data->state.creds ?
           data->state.creds->user : ""));
#وإلا
    (فراغ)proxy;
    infof(data, "Server auth using %s with user '%s'",
          auth, data->state.creds ?
          data->state.creds->user : "");
#endif
    authstatus->multipass = !authstatus->done;
  }
  وإلا {
    authstatus->multipass = FALSE;
    إذا(proxy)
      data->info.proxyauthpicked = 0;
    وإلا
      data->info.httpauthpicked = 0;
  }

  أرجع result;
}

CURLcode Curl_http_output_auth(بنية Curl_easy *data,
                               بنية connectdata *conn,
                               ثابت حرف *request,
                               Curl_HttpReq httpreq,
                               ثابت حرف *path,
                               ثابت حرف *query,
                               bool is_connect)
{
  CURLcode result = CURLE_OK;
  بنية auth *authhost;
  بنية auth *authproxy;
  ثابت حرف *path_and_query = path;
  حرف *tmp_str = عدم;

  DEBUGASSERT(data);
  authhost = &data->state.authhost;
  authproxy = &data->state.authproxy;

  إذا(
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
    اذهب out;
  }

  إذا(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    إذا(!tmp_str) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
    path_and_query = tmp_str;
  }

  إذا(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  إذا(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ifndef CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  إذا(conn->bits.origin_is_proxy || is_connect) {
    result = output_auth_headers(data, conn, authproxy, request,
                                 path_and_query, TRUE);
    إذا(result)
      اذهب out;
  }
  وإلا
#وإلا
  (فراغ)is_connect;
#endif /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->done = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  إذا(data->state.creds || Curl_auth_allowed_to_host(data))
    result = output_auth_headers(data, conn, authhost, request,
                                 path_and_query, FALSE);
  وإلا
    authhost->done = TRUE;

  إذا(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_HEAD)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    data->req.authneg = TRUE;
  }
  وإلا
    data->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  أرجع result;
}

#وإلا /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(بنية Curl_easy *data,
                               بنية connectdata *conn,
                               ثابت حرف *request,
                               Curl_HttpReq httpreq,
                               ثابت حرف *path,
                               ثابت حرف *query,
                               bool is_connect)
{
  (فراغ)data;
  (فراغ)conn;
  (فراغ)request;
  (فراغ)httpreq;
  (فراغ)path;
  (فراغ)query;
  (فراغ)is_connect;
  أرجع CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP_AUTH, else */

#إذا defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
ساكن bool authcmp(ثابت حرف *auth, ثابت حرف *سطر)
{
  /* the auth string must not have an alnum following */
  size_t n = strlen(auth);
  أرجع curl_strnequal(auth, سطر, n) && !ISALNUM(سطر[n]);
}
#endif

#ifdef USE_SPNEGO
ساكن CURLcode auth_spnego(بنية Curl_easy *data,
                            bool proxy,
                            ثابت حرف *auth,
                            بنية auth *authp,
                            uint32_t *availp)
{
  إذا((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    إذا(authp->picked == CURLAUTH_NEGOTIATE) {
      بنية connectdata *conn = data->conn;
      CURLcode result = Curl_داخلہ_negotiate(data, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      إذا(!result) {
        curlx_free(data->req.newurl);
        data->req.newurl = Curl_bufref_dup(&data->state.url);
        إذا(!data->req.newurl)
          أرجع CURLE_OUT_OF_MEMORY;
        data->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      وإلا
        data->state.authproblem = TRUE;
    }
  }
  أرجع CURLE_OK;
}
#endif

#ifdef USE_NTLM
ساكن CURLcode auth_ntlm(بنية Curl_easy *data,
                          bool proxy,
                          ثابت حرف *auth,
                          بنية auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  إذا((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    إذا(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode result = Curl_داخلہ_ntlm(data, proxy, auth);
      إذا(!result)
        data->state.authproblem = FALSE;
      وإلا {
        إذا(result == CURLE_OUT_OF_MEMORY)
          أرجع result;
        infof(data, "NTLM authentication problem, ignoring.");
        data->state.authproblem = TRUE;
      }
    }
  }
  أرجع CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_DIGEST_AUTH
ساكن CURLcode auth_digest(بنية Curl_easy *data,
                            bool proxy,
                            ثابت حرف *auth,
                            بنية auth *authp,
                            uint32_t *availp)
{
  إذا(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(data, "Ignoring duplicate digest auth header.");
  }
  وإلا إذا(Curl_auth_is_digest_supported()) {
    CURLcode result;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    result = Curl_داخلہ_digest(data, proxy, auth);
    إذا(result) {
      إذا(result == CURLE_OUT_OF_MEMORY)
        أرجع result;
      infof(data, "Digest authentication problem, ignoring.");
      data->state.authproblem = TRUE;
    }
  }
  أرجع CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BASIC_AUTH
ساكن CURLcode auth_basic(بنية Curl_easy *data,
                           بنية auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  إذا(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Basic authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  أرجع CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BEARER_AUTH
ساكن CURLcode auth_bearer(بنية Curl_easy *data,
                            بنية auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  إذا(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Bearer authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  أرجع CURLE_OK;
}
#endif

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_داخلہ_auth(بنية Curl_easy *data, bool proxy,
                              ثابت حرف *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#إذا defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  بنية auth *authp;
  CURLcode result = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(data);

  إذا(proxy) {
    availp = &data->info.proxyauthavail;
    authp = &data->state.authproxy;
  }
  وإلا {
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

  طالما(*auth) {
#ifdef USE_SPNEGO
    إذا(authcmp("Negotiate", auth))
      result = auth_spnego(data, proxy, auth, authp, availp);
#endif
#ifdef USE_NTLM
    إذا(!result && authcmp("NTLM", auth))
      result = auth_ntlm(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
    إذا(!result && authcmp("Digest", auth))
      result = auth_digest(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
    إذا(!result && authcmp("Basic", auth))
      result = auth_basic(data, authp, availp);
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
    إذا(authcmp("Bearer", auth))
      result = auth_bearer(data, authp, availp);
#endif

    إذا(result)
      اكسر;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    إذا(auth) /* if we are on a comma, skip it */
      auth++;
    وإلا
      اكسر;
    curlx_str_passblanks(&auth);
  }
  أرجع result;
#وإلا
  (فراغ)data;
  (فراغ)proxy;
  (فراغ)auth;
  /* nothing to do when disabled */
  أرجع CURLE_OK;
#endif
}

ساكن فراغ http_switch_to_get(بنية Curl_easy *data, صحيح code)
{
  ثابت حرف *req = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  إذا((req || data->state.httpreq != HTTPREQ_GET) &&
     (data->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((فراغ)code);
    infof(data, "Switch to GET because of %d response", code);
    data->state.http_ignorecustom = TRUE;
  }
  وإلا إذا(req && (data->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(data, "Stick to %s instead of GET", req);

  data->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(data, FALSE);
}

#define HTTPREQ_IS_POST(data)                    \
  ((data)->state.httpreq == HTTPREQ_POST ||      \
   (data)->state.httpreq == HTTPREQ_POST_FORM || \
   (data)->state.httpreq == HTTPREQ_POST_MIME)

ساكن CURLcode set_auto_referer(بنية Curl_easy *data)
{
  CURLU *u;
  حرف *referer = عدم;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&data->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  إذا(!u)
    أرجع CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&data->state.url), 0);
  إذا(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, عدم, 0);
  إذا(!uc)
    uc = curl_url_set(u, CURLUPART_USER, عدم, 0);
  إذا(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, عدم, 0);
  إذا(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  إذا(uc || !referer)
    أرجع CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&data->state.referer, referer, 0, curl_free);
  أرجع CURLE_OK;
}

ساكن CURLcode follow_parse_url(بنية Curl_easy *data, ثابت حرف *newurl,
                                 followtype type, حرف **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(data->state.uh);
  uc = curl_url_set(data->state.uh, CURLUPART_URL, newurl, (unsigned صحيح)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  إذا(uc) {
    إذا((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(data, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      أرجع Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    إذا(!*follow_urlp)
      أرجع CURLE_OUT_OF_MEMORY;
  }
  وإلا {
    CURLU *u = curl_url();
    إذا(!u)
      أرجع CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&data->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    إذا(!uc)
      uc = curl_url_get(data->state.uh, CURLUPART_URL, follow_urlp, 0);
    إذا(uc) {
      curl_url_cleanup(u);
      أرجع Curl_uc_to_curlcode(uc);
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, data->state.uh);
      curl_url_cleanup(u);
      إذا(!same_origin)
        Curl_auth_digest_cleanup(&data->state.digest);
    }
#وإلا
    curl_url_cleanup(u);
#endif
  }
  أرجع CURLE_OK;
}

ساكن CURLcode follow_fake(بنية Curl_easy *data, حرف *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  إذا(!uc) {
    CURLU *u = curl_url();
    حرف *nocred = عدم;

    إذا(!u) {
      curlx_free(follow_url);
      أرجع CURLE_OUT_OF_MEMORY;
    }
    إذا(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, عدم, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, عدم, 0))
      (فراغ)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    إذا(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  data->info.wouldredirect = follow_url;

  إذا(reachedmax) {
    failf(data, "Maximum (%d) redirects followed", data->set.maxredirs);
    أرجع CURLE_TOO_MANY_REDIRECTS;
  }
  أرجع CURLE_OK;
}

ساكن bool follow_switch_get(بنية Curl_easy *data)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  بدل(data->info.httpcode) {
  حالة 301: /* Moved Permanently */
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
    إذا(HTTPREQ_IS_POST(data) && !data->set.post301) {
      http_switch_to_get(data, 301);
      أرجع TRUE;
    }
    اكسر;
  حالة 302: /* Found */
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
    إذا(HTTPREQ_IS_POST(data) && !data->set.post302) {
      http_switch_to_get(data, 302);
      أرجع TRUE;
    }
    اكسر;
  حالة 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    إذا(!HTTPREQ_IS_POST(data) || !data->set.post303) {
      http_switch_to_get(data, 303);
      أرجع TRUE;
    }
    اكسر;

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
  افتراضي:
    اكسر;
  }
  أرجع FALSE;
}

CURLcode Curl_http_follow(بنية Curl_easy *data, ثابت حرف *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  حرف *follow_url = عدم;
  CURLUcode uc;
  CURLcode result;
  CURLcode rewind_result;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  إذا(type != FOLLOW_FAKE)
    data->state.requests++; /* count all real follows */
  إذا(type == FOLLOW_REDIR) {
    إذا((data->set.maxredirs != -1) &&
       (data->state.followlocation >= data->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    وإلا {
      data->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      إذا(data->set.http_auto_referer) {
        result = set_auto_referer(data);
        إذا(result)
          أرجع result;
      }
    }
  }

  إذا((type != FOLLOW_RETRY) &&
     (data->req.httpcode != 401) && (data->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, عدم, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  result = follow_parse_url(data, newurl, type, &follow_url, &uc);
  إذا(result)
    أرجع result;

  DEBUGASSERT(follow_url);

  إذا(type == FOLLOW_FAKE)
    أرجع follow_fake(data, follow_url, uc, reachedmax);

  إذا(disallowport)
    data->state.allow_port = FALSE;

  Curl_bufref_set(&data->state.url, follow_url, 0, curl_free);
  rewind_result = Curl_req_soft_reset(&data->req, data);
  infof(data, "Issue another request to this URL: '%s'", follow_url);
  إذا((data->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    data->state.http_ignorecustom = TRUE;
    infof(data, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(data);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  إذا(rewind_result && !switch_to_get)
    أرجع rewind_result;

  Curl_pgrsTime(data, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(data);

  أرجع CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(ثابت حرف *headerline, /* line to check */
                        ثابت حرف *header, /* header keyword _with_ colon */
                        ثابت size_t hlen, /* len of the keyword in bytes */
                        ثابت حرف *content, /* content string to find */
                        ثابت size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  ثابت حرف *p;
  بنية Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  إذا(!curl_strnequal(headerline, header, hlen))
    أرجع FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  إذا(curlx_str_cspn(&p, &val, "\r\n"))
    أرجع FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  إذا(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    لكل(len = curlx_strlen(&val); len >= clen;) {
      بنية Curl_str next;
      ثابت حرف *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      إذا(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        أرجع TRUE; /* match! */
      /* advance to the next comma */
      إذا(curlx_str_until(&p, &next, len, ',') ||
         curlx_str_single(&p, ','))
        اكسر; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      افعل
        curlx_str_passblanks(&p);
      طالما(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      إذا((size_t)(p - o) > len)
        اكسر;
      len -= (p - o);
    }
  }
  أرجع FALSE; /* no match */
}

بنية cr_exp100_ctx {
  بنية Curl_creader super;
  بنية curltime start; /* time started waiting */
  تعداد expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

ساكن فراغ http_exp100_continue(بنية Curl_easy *data,
                                 بنية Curl_creader *reader)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  إذا(ctx->state > EXP100_SEND_DATA) {
    ctx->state = EXP100_SEND_DATA;
    Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
  }
}

ساكن CURLcode cr_exp100_read(بنية Curl_easy *data,
                               بنية Curl_creader *reader,
                               حرف *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  بدل(ctx->state) {
  حالة EXP100_SENDING_REQUEST:
    إذا(!Curl_req_sendbuf_empty(data)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(data, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      أرجع CURLE_OK;
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
    أرجع CURLE_OK;
  حالة EXP100_FAILED:
    DEBUGF(infof(data, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    أرجع CURLE_READ_ERROR;
  حالة EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->start);
    إذا(ms < data->set.expect_100_timeout) {
      DEBUGF(infof(data, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      أرجع CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(data, reader);
    infof(data, "Done waiting for 100-continue");
    FALLTHROUGH();
  افتراضي:
    DEBUGF(infof(data, "cr_exp100_read, pass through"));
    أرجع Curl_creader_read(data, reader->next, buf, blen, nread, eos);
  }
}

ساكن فراغ cr_exp100_done(بنية Curl_easy *data,
                           بنية Curl_creader *reader, صحيح premature)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_DATA;
  Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
}

ساكن ثابت بنية Curl_crtype cr_exp100 = {
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
  حجم(بنية cr_exp100_ctx)
};

ساكن CURLcode http_exp100_add_reader(بنية Curl_easy *data)
{
  بنية Curl_creader *reader = عدم;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_exp100, CURL_CR_PROTOCOL);
  إذا(!result)
    result = Curl_creader_add(data, reader);
  إذا(!result) {
    بنية cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  إذا(result && reader)
    Curl_creader_free(data, reader);
  أرجع result;
}

ساكن فراغ http_exp100_got100(بنية Curl_easy *data)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  إذا(r)
    http_exp100_continue(data, r);
}

ساكن bool http_exp100_is_waiting(بنية Curl_easy *data)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  إذا(r) {
    بنية cr_exp100_ctx *ctx = r->ctx;
    أرجع ctx->state == EXP100_AWAITING_CONTINUE;
  }
  أرجع FALSE;
}

ساكن فراغ http_exp100_send_anyway(بنية Curl_easy *data)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  إذا(r)
    http_exp100_continue(data, r);
}

ساكن bool http_exp100_is_selected(بنية Curl_easy *data)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  أرجع !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(بنية Curl_easy *data,
                                 بنية easy_pollset *ps)
{
  /* write mode */
  أرجع Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(بنية Curl_easy *data,
                                   بنية easy_pollset *ps)
{
  بنية connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  إذا(CURL_REQ_WANT_RECV(data)) {
    result = Curl_pollset_add_in(data, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  إذا(!result && Curl_req_want_send(data) && !http_exp100_is_waiting(data)) {
    result = Curl_pollset_add_out(data, ps, conn->sock[FIRSTSOCKET]);
  }
  أرجع result;
}

ساكن CURLcode http_write_header(بنية Curl_easy *data,
                                  ثابت حرف *hd, size_t hdlen)
{
  CURLcode result;
  صحيح writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((data->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  result = Curl_client_write(data, writetype, hd, hdlen);
  إذا(result)
    أرجع result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  إذا(result)
    أرجع result;

  data->req.deductheadercount = (100 <= data->req.httpcode &&
                                 199 >= data->req.httpcode) ?
    data->req.headerbytecount : 0;
  أرجع result;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(بنية Curl_easy *data,
                        CURLcode status, bool premature)
{
  بنية connectdata *conn = data->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  data->state.authhost.multipass = FALSE;
  data->state.authproxy.multipass = FALSE;

  إذا(curlx_dyn_len(&data->state.headerb)) {
    (فراغ)http_write_header(data, curlx_dyn_ptr(&data->state.headerb),
                            curlx_dyn_len(&data->state.headerb));
  }
  curlx_dyn_reset(&data->state.headerb);

  إذا(status)
    أرجع status;

  إذا(!premature && /* this check is pointless when DONE is called before the
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
    أرجع CURLE_GOT_NOTHING;
  }

  أرجع CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
ساكن bool http_may_use_1_1(ثابت بنية Curl_easy *data)
{
  ثابت بنية connectdata *conn = data->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  إذا(data->state.http_neg.rcvd_min == 10)
    أرجع FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  إذا(conn && conn->httpversion_seen == 10)
    أرجع FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  إذا(data->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    أرجع FALSE;
  /* We are not restricted to use 1.0 only. */
  أرجع !data->state.http_neg.only_10;
}

ساكن unsigned حرف http_request_version(بنية Curl_easy *data)
{
  unsigned حرف v = Curl_conn_http_version(data, data->conn);
  إذا(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(data) ? 11 : 10;
  }
  أرجع v;
}

ساكن ثابت حرف *get_http_string(صحيح httpversion)
{
  بدل(httpversion) {
  حالة 30:
    أرجع "3";
  حالة 20:
    أرجع "2";
  حالة 11:
    أرجع "1.1";
  افتراضي:
    أرجع "1.0";
  }
}

CURLcode Curl_add_custom_headers(بنية Curl_easy *data,
                                 bool is_connect, صحيح httpversion,
                                 بنية dynbuf *req)
{
  بنية curl_slist *h[2];
  بنية curl_slist *headers;
  صحيح numlists = 1; /* by default */
  صحيح i;

#ifndef CURL_DISABLE_PROXY
  تعداد Curl_proxy_use proxy;

  إذا(is_connect)
    proxy = HEADER_CONNECT;
  وإلا
    proxy = data->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  بدل(proxy) {
  حالة HEADER_SERVER:
    h[0] = data->set.headers;
    اكسر;
  حالة HEADER_PROXY:
    h[0] = data->set.headers;
    إذا(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    اكسر;
  حالة HEADER_CONNECT:
    إذا(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    وإلا
      h[0] = data->set.headers;
    اكسر;
  حالة HEADER_CONNECT_UDP:
    إذا(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    وإلا
      h[0] = data->set.headers;
    اكسر;
  }
#وإلا
  (فراغ)is_connect;
  h[0] = data->set.headers;
#endif

  /* loop through one or two lists */
  لكل(i = 0; i < numlists; i++) {
    لكل(headers = h[i]; headers; headers = headers->next) {
      CURLcode result = CURLE_OK;
      bool blankheader = FALSE;
      بنية Curl_str name;
      ثابت حرف *p = headers->data;
      ثابت حرف *origp = p;
      size_t hlen = strlen(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      إذا(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      وإلا {
        p = origp;
        إذا(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          بنية Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          إذا(!curlx_strlen(&val))
            /* no content, do not send this */
            استمر;
        }
        وإلا
          /* no colon */
          استمر;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      إذا(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      وإلا إذا(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      وإلا إذا(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      وإلا إذا((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      وإلا إذا((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      وإلا إذا(blankheader) {
        result = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        إذا(!result)
          result = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      وإلا
        result = curlx_dyn_addf(req, "%s\r\n", origp);

      إذا(result)
        أرجع result;
    }
  }

  أرجع CURLE_OK;
}

#ifndef CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(بنية Curl_easy *data,
                                بنية dynbuf *req)
{
  ثابت بنية tm *tm;
  بنية tm keeptime;
  CURLcode result;
  حرف datestr[80];
  ثابت حرف *condp;
  size_t len;

  إذا(data->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    أرجع CURLE_OK;

  result = curlx_gmtime(data->set.timevalue, &keeptime);
  إذا(result) {
    failf(data, "Invalid TIMEVALUE");
    أرجع result;
  }
  tm = &keeptime;

  بدل(data->set.timecondition) {
  افتراضي:
    DEBUGF(infof(data, "invalid time condition"));
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  حالة CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    اكسر;
  حالة CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    اكسر;
  حالة CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    اكسر;
  }

  إذا(Curl_checkheaders(data, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    أرجع CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, حجم(datestr),
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
  أرجع result;
}
#وإلا
/* disabled */
CURLcode Curl_add_timecondition(بنية Curl_easy *data,
                                بنية dynbuf *req)
{
  (فراغ)data;
  (فراغ)req;
  أرجع CURLE_OK;
}
#endif

فراغ Curl_http_method(بنية Curl_easy *data,
                      ثابت حرف **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)data->state.httpreq;
  ثابت حرف *request;
#ifndef CURL_DISABLE_WEBSOCKETS
  إذا(data->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  وإلا
#endif
  إذا((data->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     data->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  إذا(!data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
  }
  وإلا {
    إذا(data->req.no_body)
      request = "HEAD";
    وإلا {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_HEAD));
      بدل(httpreq) {
      حالة HTTPREQ_POST:
      حالة HTTPREQ_POST_FORM:
      حالة HTTPREQ_POST_MIME:
        request = "POST";
        اكسر;
      حالة HTTPREQ_PUT:
        request = "PUT";
        اكسر;
      افتراضي: /* this should never happen */
      حالة HTTPREQ_GET:
        request = "GET";
        اكسر;
      حالة HTTPREQ_HEAD:
        request = "HEAD";
        اكسر;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

ساكن CURLcode http_set_aptr_host(بنية Curl_easy *data)
{
  بنية connectdata *conn = data->conn;
  ثابت حرف *ptr = عدم;

  curlx_safefree(data->state.http_host);
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(data->req.cookiehost);
#endif

  إذا(Curl_peer_equal(data->state.initial_origin, data->state.origin))
    ptr = Curl_checkheaders(data, STRCONST("Host"));

  إذا(ptr) {
#ifndef CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    حرف *cookiehost;
    CURLcode result = copy_custom_قدر(ptr, &cookiehost);
    إذا(result)
      أرجع result;
    إذا(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    وإلا {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      إذا(*cookiehost == '[') {
        حرف *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, strlen(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        إذا(closingbracket)
          *closingbracket = 0;
      }
      وإلا {
        صحيح startsearch = 0;
        حرف *colon = strchr(cookiehost + startsearch, ':');
        إذا(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      data->req.cookiehost = cookiehost;
    }
#endif

    إذا(!curl_strequal("Host:", ptr)) {
      data->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      إذا(!data->state.http_host)
        أرجع CURLE_OUT_OF_MEMORY;
    }
  }
  وإلا {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)data->state.origin->ipv6;
    بنية dynbuf tmp;
    size_t hlen;
    CURLcode result;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    result = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    إذا(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("["));
    إذا(!result)
      result = curlx_dyn_add(&tmp, data->state.origin->hostname);
    إذا(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("]"));
    إذا(!result &&
       ((data->state.origin->port != data->state.origin->scheme->defport) ||
       (data->state.origin->scheme->family != conn->scheme->family))) {
      result = curlx_dyn_addf(&tmp, ":%u", data->state.origin->port);
    }

    data->state.http_host = result ? عدم : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    أرجع result;
  }
  أرجع CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
ساكن CURLcode http_target(بنية Curl_easy *data,
                            بنية dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ثابت حرف *path = data->state.up.path;
  ثابت حرف *query = data->state.up.query;
#ifndef CURL_DISABLE_PROXY
  بنية connectdata *conn = data->conn;
#endif

  إذا(CURL_EASY_STR(data, STRING_TARGET)) {
    path = CURL_EASY_STR(data, STRING_TARGET);
    query = عدم;
  }

#ifndef CURL_DISABLE_PROXY
  إذا(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    حرف *url;
    CURLU *h = curl_url_dup(data->state.uh);
    إذا(!h)
      أرجع CURLE_OUT_OF_MEMORY;

    إذا(!data->state.origin->ipv6 &&
       (data->state.origin->user_hostname != data->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, data->state.origin->hostname, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, عدم, 0);
    إذا(uc) {
      curl_url_cleanup(h);
      أرجع CURLE_OUT_OF_MEMORY;
    }

    إذا(data->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, عدم, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, عدم, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_OUT_OF_MEMORY;
      }
    }
    وإلا إذا(data->state.creds && (data->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        data->state.creds->user, CURLU_URLENCODE);
      إذا(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          data->state.creds->passwd, CURLU_URLENCODE);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    إذا(uc) {
      curl_url_cleanup(h);
      أرجع CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    result = curlx_dyn_add(r, CURL_EASY_STR(data, STRING_TARGET) ?
      CURL_EASY_STR(data, STRING_TARGET) : url);
    curlx_free(url);
    إذا(result)
      أرجع result;

    إذا((data->state.origin->scheme == &Curl_scheme_ftp) &&
       data->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = strlen(path);
      bool type_present = FALSE;
      إذا((len >= 7) && !memcmp(&path[len - 7], ";type=", 6)) {
        بدل(Curl_raw_toupper(path[len - 1])) {
        حالة 'A':
        حالة 'D':
        حالة 'I':
          type_present = TRUE;
          اكسر;
        }
      }
      إذا(!type_present) {
        result = curlx_dyn_addf(r, ";type=%c",
                                data->state.prefer_ascii ? 'a' : 'i');
        إذا(result)
          أرجع result;
      }
    }
  }

  وإلا
#endif
  {
    result = curlx_dyn_add(r, path);
    إذا(result)
      أرجع result;
    إذا(query)
      result = curlx_dyn_addf(r, "?%s", query);
  }

  أرجع result;
}

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
ساكن CURLcode set_post_reader(بنية Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result;

  بدل(httpreq) {
#ifndef CURL_DISABLE_MIME
  حالة HTTPREQ_POST_MIME:
    data->state.mimepost = data->set.mimepostp;
    اكسر;
#endif
#ifndef CURL_DISABLE_FORM_API
  حالة HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    إذا(!data->state.formp) {
      data->state.formp = curlx_calloc(1, حجم(curl_mimepart));
      إذا(!data->state.formp)
        أرجع CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(data->state.formp);
      result = Curl_getformdata(data, data->state.formp, data->set.httppost,
                                data->state.fread_func);
      إذا(result) {
        curlx_safefree(data->state.formp);
        أرجع result;
      }
      data->state.mimepost = data->state.formp;
    }
    اكسر;
#endif
  افتراضي:
    data->state.mimepost = عدم;
    اكسر;
  }

  بدل(httpreq) {
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ifndef CURL_DISABLE_MIME
    إذا(data->state.mimepost) {
      ثابت حرف *cthdr = Curl_checkheaders(data, STRCONST("Content-Type"));

      /* Read and seek body only. */
      data->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      إذا(cthdr)
        لكل(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      وإلا إذا(data->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(data->state.mimepost, data->set.headers, 0);
      result = Curl_mime_prepare_headers(data, data->state.mimepost, cthdr,
                                         عدم, MIMESTRATEGY_FORM);
      إذا(result)
        أرجع result;
      curl_mime_headers(data->state.mimepost, عدم, 0);
      result = Curl_creader_set_mime(data, data->state.mimepost);
      إذا(result)
        أرجع result;
    }
    وإلا
#endif
    {
      result = Curl_creader_set_null(data);
    }
    data->state.infilesize = Curl_creader_total_length(data);
    أرجع result;

  افتراضي:
    أرجع Curl_creader_set_null(data);
  }
  /* never reached */
}
#endif

ساكن CURLcode set_reader(بنية Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t postsize = data->state.infilesize;

  DEBUGASSERT(data->conn);

  إذا(data->req.authneg) {
    أرجع Curl_creader_set_null(data);
  }

  بدل(httpreq) {
  حالة HTTPREQ_PUT: /* Let's PUT the data to the server! */
    أرجع postsize ? Curl_creader_set_fread(data, postsize) :
      Curl_creader_set_null(data);

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
    أرجع set_post_reader(data, httpreq);
#endif

  حالة HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    إذا(!postsize) {
      result = Curl_creader_set_null(data);
    }
    وإلا إذا(data->set.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      إذا(plen == SIZE_MAX)
        أرجع CURLE_OUT_OF_MEMORY;
      وإلا إذا(plen)
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      وإلا
        result = Curl_creader_set_null(data);
    }
    وإلا {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      حرف *ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
      إذا(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      result = Curl_creader_set_fread(data, chunked ? -1 : postsize);
    }
    أرجع result;

  افتراضي:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    data->state.infilesize = 0;
    أرجع Curl_creader_set_null(data);
  }
  /* not reached */
}

ساكن CURLcode http_resume(بنية Curl_easy *data, Curl_HttpReq httpreq)
{
  إذا((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     data->state.resume_from) {
    failf(data, "HTTP upload cannot be resumed");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_req_set_TE(بنية Curl_easy *data,
                                بنية dynbuf *req,
                                صحيح httpversion)
{
  CURLcode result = CURLE_OK;
  ثابت حرف *ptr;

  ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
  إذا(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    data->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    إذا(data->req.upload_chunky && (httpversion >= 20)) {
      infof(data, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      data->req.upload_chunky = FALSE;
    }
  }
  وإلا {
    curl_off_t req_clen = Curl_creader_total_length(data);

    إذا(req_clen < 0) {
      /* indeterminate request content length */
      إذا(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        data->req.upload_chunky = (httpversion < 20);
      }
      وإلا {
        failf(data, "Chunky upload is not supported by HTTP 1.0");
        أرجع CURLE_UPLOAD_FAILED;
      }
    }
    وإلا {
      /* else, no chunky upload */
      data->req.upload_chunky = FALSE;
    }

    إذا(data->req.upload_chunky)
      result = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  أرجع result;
}

ساكن CURLcode addexpect(بنية Curl_easy *data, بنية dynbuf *r,
                          صحيح httpversion, bool *announced_exp100)
{
  CURLcode result;
  حرف *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  إذا(data->req.upgr101 != UPGR101_NONE)
    أرجع CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(data, STRCONST("Expect"));
  إذا(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  وإلا إذا(!data->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_length(data);
    إذا(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      result = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      إذا(result)
        أرجع result;
      *announced_exp100 = TRUE;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_add_content_hds(بنية Curl_easy *data,
                                     بنية dynbuf *r,
                                     صحيح httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(data->conn);
  إذا(data->req.upload_chunky) {
    result = Curl_httpchunk_add_reader(data);
    إذا(result)
      أرجع result;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_total_length(data);
  بدل(httpreq) {
  حالة HTTPREQ_PUT:
  حالة HTTPREQ_POST:
#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
#endif
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    إذا(req_clen >= 0 && !data->req.upload_chunky &&
       (data->req.authneg ||
        !Curl_checkheaders(data, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      result = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    إذا(result)
      اذهب out;

#ifndef CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    إذا(data->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      بنية curl_slist *hdr;

      لكل(hdr = data->state.mimepost->curlheaders; hdr; hdr = hdr->next) {
        result = curlx_dyn_addf(r, "%s\r\n", hdr->data);
        إذا(result)
          اذهب out;
      }
    }
#endif
    إذا(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(data, STRCONST("Content-Type"))) {
      result = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      إذا(result)
        اذهب out;
    }
    result = addexpect(data, r, httpversion, &announced_exp100);
    إذا(result)
      اذهب out;
    اكسر;
  افتراضي:
    اكسر;
  }

  Curl_pgrsSetUploadSize(data, req_clen);
  إذا(announced_exp100)
    result = http_exp100_add_reader(data);

out:
  أرجع result;
}

#ifndef CURL_DISABLE_COOKIES

ساكن CURLcode http_cookies(بنية Curl_easy *data,
                             بنية dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ثابت حرف *addcookies = عدم;
  bool linecap = FALSE;
  إذا(CURL_EASY_STR(data, STRING_COOKIE) &&
     !Curl_checkheaders(data, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(data))
    addcookies = CURL_EASY_STR(data, STRING_COOKIE);

  إذا(data->cookies || addcookies) {
    بنية Curl_llist list;
    صحيح گنتی = 0;

    إذا(data->cookies && data->state.cookie_engine) {
      bool okay;
      ثابت حرف *host = data->req.cookiehost ?
        data->req.cookiehost : data->state.origin->hostname;
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      result = Curl_cookie_getlist(data, &okay, host, &list);
      إذا(!result && okay) {
        بنية Curl_llist_گرہ *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        لكل(n = Curl_llist_head(&list); n; n = Curl_گرہ_next(n)) {
          بنية Cookie *co = Curl_گرہ_elem(n);
          إذا(co->قدر) {
            size_t add;
            إذا(!گنتی) {
              result = curlx_dyn_addn(r, STRCONST("Cookie: "));
              إذا(result)
                اكسر;
            }
            add = strlen(co->name) + strlen(co->قدر) + 1;
            إذا(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(data, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              اكسر;
            }
            result = curlx_dyn_addf(r, "%s%s=%s", گنتی ? "; " : "",
                                    co->name, co->قدر);
            إذا(result)
              اكسر;
            clen += add + (گنتی ? 2 : 0);
            گنتی++;
          }
        }
        Curl_llist_destroy(&list, عدم);
      }
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
    إذا(addcookies && !result && !linecap) {
      إذا(!گنتی)
        result = curlx_dyn_addn(r, STRCONST("Cookie: "));
      إذا(!result) {
        result = curlx_dyn_addf(r, "%s%s", گنتی ? "; " : "", addcookies);
        گنتی++;
      }
    }
    إذا(گنتی && !result)
      result = curlx_dyn_addn(r, STRCONST("\r\n"));

    إذا(result)
      أرجع result;
  }
  أرجع result;
}
#وإلا
#define http_cookies(a, b) CURLE_OK
#endif

ساكن CURLcode http_range(بنية Curl_easy *data,
                           Curl_HttpReq httpreq)
{
  إذا(data->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    إذا(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_HEAD)) &&
       !Curl_checkheaders(data, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);
      data->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 data->state.range);
      إذا(!data->state.rangeline)
        أرجع CURLE_OUT_OF_MEMORY;
    }
    وإلا إذا((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(data, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_total_length(data);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);

      إذا(data->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      وإلا إذا(data->state.resume_from) {
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
      وإلا {
        /* Range was selected and then we pass the incoming range and append
           total size */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        data->state.range, req_clen);
      }
      إذا(!data->state.rangeline)
        أرجع CURLE_OUT_OF_MEMORY;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_firstwrite(بنية Curl_easy *data)
{
  بنية connectdata *conn = data->conn;
  بنية SingleRequest *k = &data->req;

  إذا(data->req.newurl) {
    إذا(conn->bits.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      أرجع CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(data, "Ignoring the response-body");
  }
  إذا(data->state.resume_from && !k->content_range &&
     (data->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    إذا(k->size == data->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(data, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      أرجع CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(data, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    أرجع CURLE_RANGE_ERROR;
  }

  إذا(data->set.timecondition && !data->state.range &&
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
    أرجع CURLE_OK;
  } /* we have a time condition */

  أرجع CURLE_OK;
}

ساكن CURLcode http_check_new_conn(بنية Curl_easy *data)
{
  بنية connectdata *conn = data->conn;
  ثابت حرف *info_version = عدم;
  ثابت حرف *alpn;
  CURLcode result;

  alpn = Curl_conn_get_alpn_negotiated(data, conn);
  إذا(alpn && !strcmp("h3", alpn)) {
#ifndef CURL_DISABLE_PROXY
    إذا(!conn->bits.origin_is_proxy)
#endif
      DEBUGASSERT(Curl_conn_http_version(data, conn) == 30);
    info_version = "HTTP/3";
  }
  وإلا إذا(alpn && !strcmp("h2", alpn)) {
#ifndef CURL_DISABLE_PROXY
    إذا((Curl_conn_http_version(data, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      result = Curl_http2_switch(data);
      إذا(result)
        أرجع result;
    }
    وإلا
#endif
    DEBUGASSERT(Curl_conn_http_version(data, conn) == 20);
    info_version = "HTTP/2";
  }
  وإلا {
    /* Check if user wants to use HTTP/2 with clear TCP */
    إذا(Curl_http2_may_switch(data)) {
      DEBUGF(infof(data, "HTTP/2 over clean TCP"));
      result = Curl_http2_switch(data);
      إذا(result)
        أرجع result;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    وإلا
      info_version = "HTTP/1.x";
  }

  إذا(info_version)
    infof(data, "using %s", info_version);
  أرجع CURLE_OK;
}

ساكن CURLcode http_add_connection_hd(بنية Curl_easy *data,
                                       بنية dynbuf *req)
{
  بنية curl_slist *head;
  ثابت حرف *sep = "Connection: ";
  CURLcode result = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  لكل(head = data->set.headers; head; head = head->next) {
    إذا(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      حرف *قدر;
      result = copy_custom_قدر(head->data, &قدر);
      إذا(result)
        أرجع result;
      result = curlx_dyn_addf(req, "%s%s", sep, قدر);
      sep = ", ";
      curlx_free(قدر);
      اكسر; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  إذا(!result && data->state.http_hd_te) {
    result = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  إذا(!result && data->state.http_hd_upgrade) {
    result = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  إذا(!result && data->state.http_hd_h2_settings) {
    result = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  إذا(!result && (rlen < curlx_dyn_len(req)))
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
  إذا(result)
    أرجع result;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  لكل(head = data->set.headers; head; head = head->next) {
    إذا(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      إذا(skip) {
        skip = FALSE;
        استمر;
      }
      result = curlx_dyn_addf(req, "%s\r\n", head->data);
      إذا(result)
        أرجع result;
    }
  }

  أرجع CURLE_OK;
}

/* Header identifier in order we send them by default */
تعريفنوع تعداد {
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

ساكن CURLcode http_add_hd(بنية Curl_easy *data,
                            بنية dynbuf *req,
                            http_hd_t id,
                            unsigned حرف httpversion,
                            ثابت حرف *method,
                            Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
#إذا !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  بنية connectdata *conn = data->conn;
#endif
  بدل(id) {
  حالة H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    result = curlx_dyn_addf(req, "%s ", method);
    إذا(!result)
      result = http_target(data, req);
    إذا(!result)
      result = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    اكسر;

  حالة H1_HD_HOST:
    إذا(data->state.http_host) {
      result = curlx_dyn_add(req, data->state.http_host);
      إذا(!result)
        result = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    اكسر;

#ifndef CURL_DISABLE_PROXY
  حالة H1_HD_PROXY_AUTH:
    إذا(data->req.hd_proxy_auth)
      result = curlx_dyn_add(req, data->req.hd_proxy_auth);
    اكسر;
#endif

  حالة H1_HD_AUTH:
    إذا(data->req.hd_auth)
      result = curlx_dyn_add(req, data->req.hd_auth);
    اكسر;

  حالة H1_HD_RANGE:
    إذا(data->state.use_range && data->state.rangeline)
      result = curlx_dyn_add(req, data->state.rangeline);
    اكسر;

  حالة H1_HD_USER_AGENT: {
    ثابت حرف *ua = CURL_EASY_STR(data, STRING_USERAGENT);
    إذا(ua && *ua && !Curl_checkheaders(data, STRCONST("User-Agent")))
      result = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    اكسر;
  }

  حالة H1_HD_ACCEPT:
    إذا(!Curl_checkheaders(data, STRCONST("Accept")))
      result = curlx_dyn_add(req, "Accept: */*\r\n");
    اكسر;

  حالة H1_HD_TE:
#ifdef HAVE_LIBZ
    إذا(!Curl_checkheaders(data, STRCONST("TE")) &&
       data->set.http_transfer_encoding) {
      data->state.http_hd_te = TRUE;
      result = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#endif
    اكسر;

  حالة H1_HD_ACCEPT_ENCODING: {
    ثابت حرف *enc = CURL_EASY_STR(data, STRING_ENCODING);
    إذا(enc && !Curl_checkheaders(data, STRCONST("Accept-Encoding")))
      result = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    اكسر;
  }

  حالة H1_HD_REFERER:
    إذا(Curl_bufref_ptr(&data->state.referer) &&
       !Curl_checkheaders(data, STRCONST("Referer")))
      result = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&data->state.referer));
    اكسر;

#ifndef CURL_DISABLE_PROXY
  حالة H1_HD_PROXY_CONNECTION:
    إذا(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(data, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(data, data->conn, STRCONST("Proxy-Connection")))
      result = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    اكسر;
#endif

  حالة H1_HD_TRANSFER_ENCODING:
    result = http_req_set_TE(data, req, httpversion);
    اكسر;

#ifndef CURL_DISABLE_ALTSVC
  حالة H1_HD_ALT_USED:
    إذا(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(data, STRCONST("Alt-Used")))
      result = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    اكسر;
#endif

  حالة H1_HD_UPGRADE:
    إذا(!Curl_conn_is_ssl(data->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
       data->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      result = Curl_http2_request_upgrade(req, data);
    }
#ifndef CURL_DISABLE_WEBSOCKETS
    إذا(!result && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      result = Curl_ws_request(data, req);
#endif
    اكسر;

  حالة H1_HD_COOKIES:
    result = http_cookies(data, req);
    اكسر;

  حالة H1_HD_CONDITIONALS:
    result = Curl_add_timecondition(data, req);
    اكسر;

  حالة H1_HD_CUSTOM:
    result = Curl_add_custom_headers(data, FALSE, httpversion, req);
    اكسر;

  حالة H1_HD_CONTENT:
    result = http_add_content_hds(data, req, httpversion, httpreq);
    اكسر;

  حالة H1_HD_CONNECTION: {
    result = http_add_connection_hd(data, req);
    اكسر;
  }

  حالة H1_HD_LAST:
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
    اكسر;
  }
  أرجع result;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(بنية Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  Curl_HttpReq httpreq;
  ثابت حرف *method;
  بنية dynbuf req;
  unsigned حرف httpversion;
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

  إذا(!data->conn->bits.reuse) {
    result = http_check_new_conn(data);
    إذا(result)
      اذهب out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  result = Curl_headers_init(data);
  إذا(result)
    اذهب out;

  data->state.http_hd_te = FALSE;
  data->state.http_hd_upgrade = FALSE;
  data->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(data, &method, &httpreq);

  /* select host to send */
  result = http_set_aptr_host(data);
  /* setup the authentication headers, how that method and host are known */
  إذا(!result)
    result = Curl_http_output_auth(data, data->conn, method, httpreq,
                                   data->state.up.path,
                                   data->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  إذا(!result)
    result = set_reader(data, httpreq);
  إذا(!result)
    result = http_resume(data, httpreq);
  إذا(!result)
    result = http_range(data, httpreq);
  إذا(result)
    اذهب out;

  httpversion = http_request_version(data);
  /* Add request line and all headers to `req` */
  لكل(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    result = http_add_hd(data, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    إذا(result)
      اذهب out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);
  result = Curl_req_send(data, &req, httpversion);

  إذا((httpversion >= 20) && data->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    data->req.upload_chunky = FALSE;

out:
  إذا(result == CURLE_TOO_LARGE)
    failf(data, "HTTP request too large");

  curlx_dyn_free(&req);
  أرجع result;
}

تعريفنوع تعداد {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
ساكن bool checkprefixmax(ثابت حرف *prefix, ثابت حرف *buffer, size_t len)
{
  size_t ch = CURLMIN(strlen(prefix), len);
  أرجع curl_strnequal(prefix, buffer, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
ساكن statusline checkhttpprefix(بنية Curl_easy *data,
                                  ثابت حرف *s, size_t len)
{
  بنية curl_slist *head = data->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  طالما(head) {
    إذا(checkprefixmax(head->data, s, len)) {
      rc = onmatch;
      اكسر;
    }
    head = head->next;
  }

  إذا((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  أرجع rc;
}

#ifndef CURL_DISABLE_RTSP
ساكن statusline checkrtspprefix(بنية Curl_easy *data,
                                  ثابت حرف *s, size_t len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (فراغ)data;
  إذا(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  أرجع status;
}
#endif /* CURL_DISABLE_RTSP */

ساكن statusline checkprotoprefix(بنية Curl_easy *data,
                                   بنية connectdata *conn,
                                   ثابت حرف *s, size_t len)
{
#ifndef CURL_DISABLE_RTSP
  إذا(conn->scheme->protocol & CURLPROTO_RTSP)
    أرجع checkrtspprefix(data, s, len);
#وإلا
  (فراغ)conn;
#endif /* CURL_DISABLE_RTSP */

  أرجع checkhttpprefix(data, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#define HD_IS(hd, hdlen, n) \
  (((hdlen) >= (حجم(n) - 1)) && curl_strnequal(n, hd, حجم(n) - 1))

#define HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (حجم(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, حجم(n) - 1)) ? ((hd) + (حجم(n) - 1)) : عدم)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#define HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((حجم(n) - 1) + (حجم(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
ساكن CURLcode http_header_a(بنية Curl_easy *data,
                              ثابت حرف *hd, size_t hdlen)
{
#ifndef CURL_DISABLE_ALTSVC
  ثابت حرف *v;
  v = (data->asi &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#وإلا
        0
#endif
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : عدم;
  إذا(v) {
    /* the ALPN of the current request */
    بنية SingleRequest *k = &data->req;
    تعداد alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    أرجع Curl_altsvc_parse(data, data->asi, v, data->state.origin, id);
  }
#وإلا
  (فراغ)data;
  (فراغ)hd;
  (فراغ)hdlen;
#endif
  أرجع CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
ساكن CURLcode http_header_c(بنية Curl_easy *data,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = data->conn;
  بنية SingleRequest *k = &data->req;
  ثابت حرف *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !data->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : عدم;
  إذا(v) {
    افعل {
      curl_off_t contentlength;
      صحيح offt = curlx_str_numblanks(&v, &contentlength);

      إذا(offt == STRE_OVERFLOW) {
        /* out of range */
        إذا(data->set.max_filesize) {
          failf(data, "Maximum file size exceeded");
          أرجع CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(data, "Overflow Content-Length: value");
        أرجع CURLE_OK;
      }
      وإلا {
        إذا((offt == STRE_OK) &&
           ((k->size == -1) || /* not set to something before */
            (k->size == contentlength))) { /* or the same value */

          k->size = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          إذا(!curlx_str_single(&v, ','))
            استمر;

          إذا(!curlx_str_newline(&v)) {
            k->maxdownload = k->size;
            أرجع CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(data, "Invalid Content-Length: value");
        أرجع CURLE_WEIRD_SERVER_REPLY;
      }
    } طالما(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(data, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : عدم;
  إذا(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    أرجع Curl_build_unencoding_ڈھیر(data, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  إذا(v) {
    حرف *contenttype = Curl_copy_header_قدر(hd);
    إذا(!contenttype)
      أرجع CURLE_OUT_OF_MEMORY;
    إذا(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    وإلا {
      curlx_free(data->info.contenttype);
      data->info.contenttype = contenttype;
    }
    أرجع CURLE_OK;
  }
  إذا((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    أرجع CURLE_OK;
  }
  إذا((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(data, "HTTP/1.0 connection set to keep alive");
    أرجع CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : عدم;
  إذا(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    ثابت حرف *ptr = v;

    /* Move forward until first digit or asterisk */
    طالما(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    إذا(ISDIGIT(*ptr)) {
      إذا(!curlx_str_نمبر(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (data->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    وإلا إذا(k->httpcode < 300)
      data->state.resume_from = 0; /* get everything */
  }
  أرجع CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
ساكن CURLcode http_header_l(بنية Curl_easy *data,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = data->conn;
  بنية SingleRequest *k = &data->req;
  ثابت حرف *v = (!k->http_bodyless &&
                   (data->set.timecondition || data->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : عدم;
  إذا(v) {
    إذا(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    إذا(data->set.get_filetime)
      data->info.filetime = k->timeofdoc;
    أرجع CURLE_OK;
  }
  إذا(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    حرف *location = Curl_copy_header_قدر(hd);
    إذا(!location)
      أرجع CURLE_OUT_OF_MEMORY;
    إذا(!*location ||
       (data->req.location && !strcmp(data->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      أرجع CURLE_OK;
    }
    وإلا {
      /* has value and is not an exact repeat */
      إذا(data->req.location) {
        failf(data, "Multiple Location headers");
        curlx_free(location);
        أرجع CURLE_WEIRD_SERVER_REPLY;
      }
      data->req.location = location;

      إذا((k->httpcode >= 300 && k->httpcode < 400) &&
         data->set.http_follow_mode) {
        CURLcode result;
        DEBUGASSERT(!data->req.newurl);
        data->req.newurl = curlx_strdup(data->req.location); /* clone */
        إذا(!data->req.newurl)
          أرجع CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        result = http_perhapsrewind(data, conn);
        إذا(result)
          أرجع result;

        /* mark the next request as a followed location: */
        data->state.this_is_a_follow = TRUE;
      }
    }
  }
  أرجع CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
ساكن CURLcode http_header_p(بنية Curl_easy *data,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &data->req;

#ifndef CURL_DISABLE_PROXY
  ثابت حرف *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  إذا(v) {
    بنية connectdata *conn = data->conn;
    إذا((k->httpversion == 10) && conn->http_proxy.peer &&
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
    وإلا إذا((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(data, "HTTP/1.1 proxy connection set close");
    }
    أرجع CURLE_OK;
  }
#endif
  إذا((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    حرف *auth;
    CURLcode result;
#ifndef CURL_DISABLE_PROXY
    إذا(!data->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      أرجع CURLE_OK;
#endif
    auth = Curl_copy_header_قدر(hd);
    result = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    إذا(!result) {
      result = Curl_http_داخلہ_auth(data, TRUE, auth);
      curlx_free(auth);
    }
    أرجع result;
  }
#ifdef USE_SPNEGO
  إذا(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    بنية connectdata *conn = data->conn;
    بنية negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    بنية auth *authp = &data->state.authhost;
    إذا(!negdata)
      أرجع CURLE_OUT_OF_MEMORY;
    إذا(authp->picked == CURLAUTH_NEGOTIATE) {
      حرف *persistentauth = Curl_copy_header_قدر(hd);
      إذا(!persistentauth)
        أرجع CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(data, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#endif
  أرجع CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
ساكن CURLcode http_header_r(بنية Curl_easy *data,
                              ثابت حرف *hd, size_t hdlen)
{
  ثابت حرف *v = HD_VAL(hd, hdlen, "Retry-After:");
  إذا(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    إذا(!Curl_getdate_capped(v, &date)) {
      time_t current = time(عدم);
      إذا(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    وإلا
      /* Try it as a decimal number, ignore errors */
      (فراغ)curlx_str_نمبر(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    إذا(retry_after > 21600)
      retry_after = 21600;
    data->info.retry_after = retry_after;
  }
  أرجع CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
ساكن CURLcode http_header_s(بنية Curl_easy *data,
                              ثابت حرف *hd, size_t hdlen)
{
#إذا !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ثابت حرف *v;
#وإلا
  (فراغ)data;
  (فراغ)hd;
  (فراغ)hdlen;
#endif

#ifndef CURL_DISABLE_COOKIES
  v = (data->cookies && data->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : عدم;
  إذا(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ثابت حرف *host = data->req.cookiehost ?
      data->req.cookiehost : data->state.origin->hostname;
    ثابت unsigned حرف secure_context = Curl_secure_context(data, host) ?
      COOKIE_SECURE : 0;
    CURLcode result;
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    result = Curl_cookie_add(data, data->cookies, v, host,
                             data->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    أرجع result;
  }
#endif
#ifndef CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (data->hsts &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#وإلا
        0
#endif
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : عدم;
  إذا(v) {
    CURLcode result = Curl_hsts_parse(
      data->hsts, data->state.origin->hostname, v);
    إذا(result) {
      إذا(result == CURLE_OUT_OF_MEMORY)
        أرجع result;
      infof(data, "Illegal STS header skipped");
    }
#ifdef DEBUGBUILD
    وإلا
      infof(data, "Parsed STS header fine (%zu entries)",
            Curl_llist_گنتی(&data->hsts->list));
#endif
  }
#endif

  أرجع CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
ساكن CURLcode http_header_t(بنية Curl_easy *data,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = data->conn;
  بنية SingleRequest *k = &data->req;

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
  ثابت حرف *v = (!k->http_bodyless &&
                   (data->state.httpreq != HTTPREQ_HEAD) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : عدم;
  إذا(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode result = Curl_build_unencoding_ڈھیر(data, v, TRUE);
    إذا(result)
      أرجع result;
    إذا(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(data, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    أرجع CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  إذا(v) {
    data->req.resp_trailer = TRUE;
    أرجع CURLE_OK;
  }
  أرجع CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
ساكن CURLcode http_header_w(بنية Curl_easy *data,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  إذا((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    حرف *auth = Curl_copy_header_قدر(hd);
    إذا(!auth)
      result = CURLE_OUT_OF_MEMORY;
    وإلا {
      result = Curl_http_داخلہ_auth(data, FALSE, auth);
      curlx_free(auth);
    }
  }
  أرجع result;
}

/*
 * http_header() parses a single response header.
 */
ساكن CURLcode http_header(بنية Curl_easy *data,
                            ثابت حرف *hd, size_t hdlen)
{
  CURLcode result = CURLE_OK;

  بدل(hd[0]) {
  حالة 'a':
  حالة 'A':
    result = http_header_a(data, hd, hdlen);
    اكسر;
  حالة 'c':
  حالة 'C':
    result = http_header_c(data, hd, hdlen);
    اكسر;
  حالة 'l':
  حالة 'L':
    result = http_header_l(data, hd, hdlen);
    اكسر;
  حالة 'p':
  حالة 'P':
    result = http_header_p(data, hd, hdlen);
    اكسر;
  حالة 'r':
  حالة 'R':
    result = http_header_r(data, hd, hdlen);
    اكسر;
  حالة 's':
  حالة 'S':
    result = http_header_s(data, hd, hdlen);
    اكسر;
  حالة 't':
  حالة 'T':
    result = http_header_t(data, hd, hdlen);
    اكسر;
  حالة 'w':
  حالة 'W':
    result = http_header_w(data, hd, hdlen);
    اكسر;
  }

  إذا(!result) {
    بنية connectdata *conn = data->conn;
    إذا(conn->scheme->protocol & CURLPROTO_RTSP)
      result = Curl_rtsp_parseheader(data, hd);
  }
  أرجع result;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
ساكن CURLcode http_statusline(بنية Curl_easy *data,
                                بنية connectdata *conn)
{
  بنية SingleRequest *k = &data->req;

  بدل(k->httpversion) {
  حالة 10:
  حالة 11:
#ifdef USE_HTTP2
  حالة 20:
#endif
#ifdef USE_HTTP3
  حالة 30:
#endif
    /* no major version switch mid-connection */
    إذا(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(data, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      أرجع CURLE_WEIRD_SERVER_REPLY;
    }
    اكسر;
  افتراضي:
    failf(data, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  data->info.httpcode = k->httpcode;
  data->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  إذا(!data->state.http_neg.rcvd_min ||
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
  إذا(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  إذا(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(data, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  بدل(k->httpcode) {
  حالة 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    إذا(data->set.timecondition)
      data->info.timecond = TRUE;
    FALLTHROUGH();
  حالة 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->size = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
ساكن CURLcode http_size(بنية Curl_easy *data)
{
  بنية SingleRequest *k = &data->req;
  إذا(data->req.ignore_cl || k->chunk) {
    k->size = k->maxdownload = -1;
  }
  وإلا إذا(k->size != -1) {
    إذا(data->set.max_filesize &&
       !k->ignorebody &&
       (k->size > data->set.max_filesize)) {
      failf(data, "Maximum file size exceeded");
      أرجع CURLE_FILESIZE_EXCEEDED;
    }
    إذا(k->ignorebody)
      infof(data, "setting size while ignoring");
    Curl_pgrsSetDownloadSize(data, k->size);
    k->maxdownload = k->size;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_verify_header(بنية Curl_easy *data,
                            ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &data->req;
  size_t i;
  bool has_colon = FALSE;
  لكل(i = 0; i < hdlen; i++) {
    بدل(hd[i]) {
    حالة '\0':
      /* this is bad, bail out */
      failf(data, "NUL byte in header");
      أرجع CURLE_WEIRD_SERVER_REPLY;
    حالة '\r':
      إذا(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(data, "Carriage return found in header");
        أرجع CURLE_WEIRD_SERVER_REPLY;
      }
      اكسر;
    حالة ':':
      has_colon = TRUE;
      اكسر;
    }
  }
  إذا(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    أرجع CURLE_OK;
  إذا(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  وإلا إذا(!has_colon) {
    /* this is bad, bail out */
    failf(data, "Header without colon");
    أرجع CURLE_WEIRD_SERVER_REPLY;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_bump_headersize(بنية Curl_easy *data,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  unsigned صحيح max = MAX_HTTP_RESP_HEADER_SIZE;
  إذا(delta < MAX_HTTP_RESP_HEADER_SIZE) {
    data->info.header_size += (unsigned صحيح)delta;
    data->req.allheadercount += (unsigned صحيح)delta;
    إذا(!connect_only)
      data->req.headerbytecount += (unsigned صحيح)delta;
    إذا(data->req.allheadercount > max)
      bad = data->req.allheadercount;
    وإلا إذا(data->info.header_size > (max * 20)) {
      bad = data->info.header_size;
      max *= 20;
    }
  }
  وإلا
    bad = data->req.allheadercount + delta;
  إذا(bad) {
    failf(data, "Too large response headers: %zu > %u", bad, max);
    أرجع CURLE_RECV_ERROR;
  }
  أرجع CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
ساكن CURLcode http_on_101_upgrade(بنية Curl_easy *data,
                                    ثابت حرف *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_changed)
{
  بنية connectdata *conn = data->conn;
  بنية SingleRequest *k = &data->req;

#إذا !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (فراغ)buf;
  (فراغ)blen;
  (فراغ)pconsumed;
#وإلا
  CURLcode result;
  صحيح upgr101_requested = k->upgr101;
#endif

  إذا(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(data, "server sent 101 response while not talking HTTP/1.1");
    أرجع CURLE_WEIRD_SERVER_REPLY;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->bits.upgrade_in_progress = FALSE;
  *conn_changed = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ifdef USE_NGHTTP2
  إذا(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(data, "Received 101, Switching to HTTP/2");
    result = Curl_http2_upgrade(data, conn, FIRSTSOCKET, buf, blen);
    إذا(!result)
      *pconsumed += blen;
    أرجع result;
  }
#endif
#ifndef CURL_DISABLE_WEBSOCKETS
  إذا(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(data, "Received 101, Switching to WebSocket");
    result = Curl_ws_accept(data, buf, blen);
    إذا(!result)
      *pconsumed += blen; /* ws accept handled the data */
    أرجع result;
  }
#endif
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  أرجع CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
ساكن CURLcode http_on_1xx_response(بنية Curl_easy *data,
                                     ثابت حرف *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_changed)
{
  بنية SingleRequest *k = &data->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  بدل(k->httpcode) {
  حالة 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(data);
    اكسر;
  حالة 101:
    أرجع http_on_101_upgrade(data, buf, blen, pconsumed, conn_changed);
  افتراضي:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    اكسر;
  }
  أرجع CURLE_OK;
}

#إذا defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
ساكن فراغ http_check_auth_closure(بنية Curl_easy *data,
                                    بنية connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ifdef USE_NTLM
  إذا(conn->bits.close &&
     (((data->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((data->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
#endif
#ifdef USE_SPNEGO
  إذا(conn->bits.close &&
    (((data->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((data->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
  إذا((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  إذا((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#endif
}
#وإلا
#define http_check_auth_closure(x, y) /* empty */
#endif

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
ساكن CURLcode http_handle_send_error(بنية Curl_easy *data)
{
  بنية connectdata *conn = data->conn;
  بنية SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  إذا(!data->req.authneg && !conn->bits.close &&
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

    بدل(data->state.httpreq) {
    حالة HTTPREQ_PUT:
    حالة HTTPREQ_POST:
    حالة HTTPREQ_POST_FORM:
    حالة HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      إذا(!Curl_req_done_sending(data)) {
        إذا((k->httpcode == 417) && http_exp100_is_selected(data)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          إذا(!k->writebytecount && http_exp100_is_waiting(data)) {
            infof(data, "Got HTTP failure 417 while waiting for a 100");
          }
          وإلا {
            infof(data, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            result = http_perhapsrewind(data, conn);
            إذا(result)
              أرجع result;
          }
          data->state.disableexpect = TRUE;
          Curl_req_abort_sending(data);
          DEBUGASSERT(!data->req.newurl);
          data->req.newurl = Curl_bufref_dup(&data->state.url);
          إذا(!data->req.newurl)
            أرجع CURLE_OUT_OF_MEMORY;
        }
        وإلا إذا(data->set.http_keep_sending_on_error) {
          infof(data, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(data);
        }
        وإلا {
          infof(data, "HTTP error before end of send, stop sending");
          streamclose(conn);
          result = Curl_req_abort_sending(data);
          إذا(result)
            أرجع result;
        }
      }
      اكسر;

    افتراضي: /* default label present to avoid compiler warnings */
      اكسر;
    }
  }

  إذا(Curl_creader_will_rewind(data) && !Curl_req_done_sending(data)) {
    /* We rewind before next send, continue sending now */
    infof(data, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(data);
  }
  أرجع result;
}

ساكن CURLcode http_on_response(بنية Curl_easy *data,
                                 ثابت حرف *last_hd, size_t last_hd_len,
                                 ثابت حرف *buf, size_t blen,
                                 size_t *pconsumed)
{
  بنية connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  بنية SingleRequest *k = &data->req;
  bool conn_changed = FALSE;

  (فراغ)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  إذا(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    إذا(data->req.httpversion != 20)
      infof(data, "Lying server, not serving HTTP/2");
  }

  إذا(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    result = http_write_header(data, last_hd, last_hd_len);
    last_hd = عدم; /* handled it */
    إذا(result)
      اذهب out;
  }

  إذا(k->httpcode < 100) {
    failf(data, "Unsupported response code in HTTP response");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    اذهب out;
  }
  وإلا إذا(k->httpcode < 200) {
    result = http_on_1xx_response(data, buf, blen, pconsumed, &conn_changed);
    اذهب out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  إذا(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  إذا((k->size == -1) && !k->chunk && !conn->bits.close &&
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
  إذا(data->req.upgr101 == UPGR101_WS) {
    failf(data, "Refused WebSocket upgrade: %d", k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    اذهب out;
  }
#endif

  /* Check if this response means the transfer errored. */
  إذا(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    اذهب out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  result = Curl_http_auth_act(data);
  إذا(result)
    اذهب out;

  إذا(k->httpcode >= 300) {
    result = http_handle_send_error(data);
    إذا(result)
      اذهب out;
  }

  /* final response without error, prepare to receive the body */
  result = http_firstwrite(data);
  إذا(result)
    اذهب out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  result = http_size(data);
  إذا(result)
    اذهب out;

#ifndef CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  إذا((conn->scheme->protocol & CURLPROTO_RTSP) && data->req.no_body &&
     !data->set.opt_no_body && (k->size > 0))
    data->req.no_body = FALSE;
#endif

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  إذا(data->req.no_body)
    k->download_done = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  إذا((k->maxdownload == 0) && (k->httpversion_sent < 20))
    k->download_done = TRUE;

out:
  إذا(last_hd)
    /* if not written yet, write it now */
    result = Curl_1st_fatal(result,
                            http_write_header(data, last_hd, last_hd_len));
  إذا(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(data->multi);
  أرجع result;
}

ساكن CURLcode http_rw_hd(بنية Curl_easy *data,
                           ثابت حرف *hd, size_t hdlen,
                           ثابت حرف *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  بنية SingleRequest *k = &data->req;
  صحيح writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  إذا((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    بنية dynbuf last_header;
    size_t consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    result = curlx_dyn_addn(&last_header, hd, hdlen);
    إذا(result)
      أرجع result;

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
    أرجع result;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_HEADER;
  إذا(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    إذا(data->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ثابت حرف *p = hd;

      curlx_str_passblanks(&p);
      إذا(!strncmp(p, "HTTP/", 5)) {
        p += 5;
        بدل(*p) {
        حالة '1':
          p++;
          إذا((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            إذا(ISBLANK(p[2])) {
              k->httpversion = (unsigned حرف)(10 + (p[1] - '0'));
              p += 3;
              إذا(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          إذا(!fine_statusline) {
            failf(data, "Unsupported HTTP/1 subversion in response");
            أرجع CURLE_UNSUPPORTED_PROTOCOL;
          }
          اكسر;
        حالة '2':
        حالة '3':
          إذا(!ISBLANK(p[1]))
            اكسر;
          k->httpversion = (unsigned حرف)((*p - '0') * 10);
          p += 2;
          إذا(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            إذا(!ISBLANK(*p))
              اكسر;
            fine_statusline = TRUE;
          }
          اكسر;
        افتراضي: /* unsupported */
          failf(data, "Unsupported HTTP version in response");
          أرجع CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      إذا(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(data, hd, hdlen);
        إذا(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    وإلا إذا(data->conn->scheme->protocol & CURLPROTO_RTSP) {
      ثابت حرف *p = hd;
      بنية Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      إذا(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_نمبر(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        إذا(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (صحيح)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      إذا(!fine_statusline)
        أرجع CURLE_WEIRD_SERVER_REPLY;
    }

    إذا(fine_statusline) {
      result = http_statusline(data, data->conn);
      إذا(result)
        أرجع result;
      writetype |= CLIENTWRITE_STATUS;
    }
    وإلا {
      k->header = FALSE;   /* this is not a header line */
      أرجع CURLE_WEIRD_SERVER_REPLY;
    }
  }

  result = Curl_verify_header(data, hd, hdlen);
  إذا(result)
    أرجع result;

  result = http_header(data, hd, hdlen);
  إذا(result)
    أرجع result;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  إذا(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  result = Curl_client_write(data, writetype, hd, hdlen);
  إذا(result)
    أرجع result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  إذا(result)
    أرجع result;

  أرجع CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
فراغ Curl_http_to_fold(بنية dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  ثابت حرف *hd = curlx_dyn_ptr(bf);
  إذا(len && (hd[len - 1] == '\n'))
    len--;
  إذا(len && (hd[len - 1] == '\r'))
    len--;
  طالما(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

ساكن فراغ unfold_header(بنية Curl_easy *data)
{
  Curl_http_to_fold(&data->state.headerb);
  data->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
ساكن CURLcode http_parse_headers(بنية Curl_easy *data,
                                   ثابت حرف *buf, size_t blen,
                                   size_t *pconsumed)
{
  بنية connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  بنية SingleRequest *k = &data->req;
  ثابت حرف *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  إذا(data->state.maybe_folded && blen) {
    إذا(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(data);
    }
    وإلا {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      result = http_rw_hd(data, curlx_dyn_ptr(&data->state.headerb),
                          curlx_dyn_len(&data->state.headerb),
                          عدم, 0, &ignore_this);
      curlx_dyn_reset(&data->state.headerb);
      إذا(result)
        أرجع result;
    }
    data->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  طالما(blen && k->header) {
    size_t consumed;
    size_t hlen;
    ثابت حرف *hd;
    size_t unfold_len = 0;

    إذا(data->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      طالما(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      إذا(blen) {
        /* insert a single space */
        result = curlx_dyn_addn(&data->state.headerb, " ", 1);
        إذا(result)
          أرجع result;
        data->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = memchr(buf, '\n', blen);
    إذا(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      result = curlx_dyn_addn(&data->state.headerb, buf, blen);
      إذا(result)
        أرجع result;
      *pconsumed += blen + unfold_len;

      إذا(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(data, conn,
                           curlx_dyn_ptr(&data->state.headerb),
                           curlx_dyn_len(&data->state.headerb));

        إذا(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          إذا((k->httpversion >= 10) || conn->bits.reuse) {
            failf(data, "Invalid status line");
            أرجع CURLE_WEIRD_SERVER_REPLY;
          }
          إذا(!data->state.http_neg.accept_09) {
            failf(data, "Received HTTP/0.9 when not allowed");
            أرجع CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          اذهب out;
        }
      }
      اذهب out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    result = curlx_dyn_addn(&data->state.headerb, buf, consumed);
    إذا(result)
      أرجع result;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&data->state.headerb);
    hd = curlx_dyn_ptr(&data->state.headerb);

    إذا(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(data, conn, hd, hlen);
      إذا(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        إذا((k->httpversion >= 10) || conn->bits.reuse) {
          failf(data, "Invalid status line");
          أرجع CURLE_WEIRD_SERVER_REPLY;
        }
        إذا(!data->state.http_neg.accept_09) {
          failf(data, "Received HTTP/0.9 when not allowed");
          أرجع CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        اذهب out;
      }
    }
    وإلا {
      إذا(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        إذا(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(data);
          استمر;
        }
        وإلا إذا(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          data->state.maybe_folded = TRUE;
          اكسر;
        }
      }
    }

    result = http_rw_hd(data, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&data->state.headerb);
    إذا(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    إذا(result)
      أرجع result;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  إذا(!k->header && !leftover_body) {
    curlx_dyn_free(&data->state.headerb);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(بنية Curl_easy *data,
                                 ثابت حرف *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode result;
  size_t consumed;
  حرف tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  result = http_rw_hd(data, hd, hdlen, &tmp, 0, &consumed);
  إذا(!result && is_eos) {
    result = Curl_client_write(data, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  أرجع result;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(بنية Curl_easy *data,
                                  ثابت حرف *buf, size_t blen,
                                  size_t *pconsumed)
{
  إذا(!data->req.header) {
    *pconsumed = 0;
    أرجع CURLE_OK;
  }
  وإلا {
    CURLcode result;

    result = http_parse_headers(data, buf, blen, pconsumed);
    إذا(!result && !data->req.header) {
      إذا(!data->req.no_body && curlx_dyn_len(&data->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&data->state.headerb),
                                   curlx_dyn_len(&data->state.headerb));
      }
      curlx_dyn_free(&data->state.headerb);
    }
    أرجع result;
  }
}

CURLcode Curl_http_write_resp(بنية Curl_easy *data,
                              ثابت حرف *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result;
  size_t consumed;
  صحيح flags;

  result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
  إذا(result || data->req.done)
    اذهب out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !data->req.header);
  إذا(!data->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    إذا(is_eos)
      flags |= CLIENTWRITE_EOS;
    result = Curl_client_write(data, flags, buf, blen);
  }
out:
  أرجع result;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, ثابت حرف *s, size_t len)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  صحيح i;

  إذا(len != 3)
    اذهب out;

  لكل(i = 0; i < 3; ++i) {
    حرف c = s[i];

    إذا(c < '0' || c > '9')
      اذهب out;

    status *= 10;
    status += c - '0';
  }
  result = CURLE_OK;
out:
  *pstatus = result ? -1 : status;
  أرجع result;
}

CURLcode Curl_http_req_make(بنية httpreq **preq,
                            ثابت حرف *method, size_t m_len,
                            ثابت حرف *scheme, size_t s_len,
                            ثابت حرف *authority, size_t a_len,
                            ثابت حرف *path, size_t p_len)
{
  بنية httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, حجم(*req) + m_len);
  إذا(!req)
    اذهب out;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ڈالو
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memcpy(req->method, method, m_len);
#إذا defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic نکالو
#endif
  إذا(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    إذا(!req->scheme)
      اذهب out;
  }
  إذا(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    إذا(!req->authority)
      اذهب out;
  }
  إذا(path) {
    req->path = curlx_memdup0(path, p_len);
    إذا(!req->path)
      اذهب out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  إذا(result && req)
    Curl_http_req_free(req);
  *preq = result ? عدم : req;
  أرجع result;
}

ساكن CURLcode req_assign_url_authority(بنية httpreq *req, CURLU *url)
{
  حرف *host, *port;
  بنية dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  host = port = عدم;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  إذا(uc && uc != CURLUE_NO_HOST)
    اذهب out;
  إذا(!host) {
    req->authority = عدم;
    result = CURLE_OK;
    اذهب out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  إذا(uc && uc != CURLUE_NO_PORT)
    اذهب out;

  result = curlx_dyn_add(&buf, host);
  إذا(result)
    اذهب out;
  إذا(port) {
    result = curlx_dyn_addf(&buf, ":%s", port);
    إذا(result)
      اذهب out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  إذا(result)
    curlx_dyn_free(&buf);
  أرجع result;
}

ساكن CURLcode req_assign_url_path(بنية httpreq *req, CURLU *url)
{
  حرف *path, *query;
  بنية dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  path = query = عدم;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  إذا(uc)
    اذهب out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  إذا(uc && uc != CURLUE_NO_QUERY)
    اذهب out;

  إذا(!query) {
    req->path = path;
    path = عدم;
  }
  وإلا {
    result = curlx_dyn_add(&buf, path);
    إذا(!result)
      result = curlx_dyn_addf(&buf, "?%s", query);
    إذا(result)
      اذهب out;
    req->path = curlx_dyn_ptr(&buf);
  }
  result = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  إذا(result)
    curlx_dyn_free(&buf);
  أرجع result;
}

CURLcode Curl_http_req_make2(بنية httpreq **preq,
                             ثابت حرف *method, size_t m_len,
                             CURLU *url, ثابت حرف *scheme_default)
{
  بنية httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, حجم(*req) + m_len);
  إذا(!req)
    اذهب out;
  memcpy(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  إذا(uc && uc != CURLUE_NO_SCHEME)
    اذهب out;
  إذا(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    إذا(!req->scheme)
      اذهب out;
  }

  result = req_assign_url_authority(req, url);
  إذا(result)
    اذهب out;
  result = req_assign_url_path(req, url);
  إذا(result)
    اذهب out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  إذا(result && req)
    Curl_http_req_free(req);
  *preq = result ? عدم : req;
  أرجع result;
}

فراغ Curl_http_req_free(بنية httpreq *req)
{
  إذا(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->path);
    Curl_dynhds_free(&req->headers);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

بنية name_const {
  ثابت حرف *name;
  size_t namelen;
};

ساكن ثابت بنية name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

ساكن bool h2_permissible_field(بنية dynhds_entry *e)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    إذا(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool http_TE_has_token(ثابت حرف *fvalue, ثابت حرف *token)
{
  طالما(*fvalue) {
    بنية Curl_str name;

    /* skip to first token */
    طالما(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    إذا(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      أرجع FALSE;
    إذا(curlx_str_casecompare(&name, token))
      أرجع TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    طالما(*fvalue && *fvalue != ',') {
      إذا(*fvalue == '"') {
        بنية Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        إذا(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          أرجع FALSE;
      }
      وإلا
        fvalue++;
    }
  }
  أرجع FALSE;
}

CURLcode Curl_http_req_to_h2(بنية dynhds *h2_headers,
                             بنية httpreq *req, بنية Curl_easy *data)
{
  ثابت حرف *scheme = عدم, *authority = عدم;
  بنية dynhds_entry *e;
  size_t i;
  CURLcode result;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  إذا(req->scheme) {
    scheme = req->scheme;
  }
  وإلا إذا(strcmp("CONNECT", req->method)) {
    scheme = Curl_checkheaders(data, STRCONST(HTTP_PSEUDO_SCHEME));
    إذا(scheme) {
      scheme += حجم(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(data, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    وإلا {
      scheme = data->state.origin->scheme->name;
    }
  }

  إذا(req->authority) {
    authority = req->authority;
  }
  وإلا {
    e = Curl_dynhds_get(&req->headers, STRCONST("Host"));
    إذا(e)
      authority = e->قدر;
  }

  Curl_dynhds_reset(h2_headers);
  Curl_dynhds_set_opts(h2_headers, DYNHDS_OPT_LOWERCASE);
  result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, strlen(req->method));
  إذا(!result && scheme) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, strlen(scheme));
  }
  إذا(!result && authority) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, strlen(authority));
  }
  إذا(!result && req->path) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, strlen(req->path));
  }
  لكل(i = 0; !result && i < Curl_dynhds_گنتی(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    إذا(e->namelen == 2 && curl_strequal("TE", e->name)) {
      إذا(http_TE_has_token(e->قدر, "trailers"))
        result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    وإلا إذا(h2_permissible_field(e)) {
      result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->قدر, e->valuelen);
    }
  }

  أرجع result;
}

CURLcode Curl_http_resp_make(بنية http_resp **presp,
                             صحيح status,
                             ثابت حرف *description)
{
  بنية http_resp *resp;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, حجم(*resp));
  إذا(!resp)
    اذهب out;

  resp->status = status;
  إذا(description) {
    resp->description = curlx_strdup(description);
    إذا(!resp->description)
      اذهب out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  إذا(result && resp)
    Curl_http_resp_free(resp);
  *presp = result ? عدم : resp;
  أرجع result;
}

فراغ Curl_http_resp_free(بنية http_resp *resp)
{
  إذا(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    إذا(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ثابت بنية Curl_protocol Curl_protocol_http = {
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
