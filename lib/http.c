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
#شامل "curl_setup.h"
#شامل "urldata.h"

#ifndef CURL_DISABLE_HTTP

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif

#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#شامل <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#endif

#شامل "transfer.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "formdata.h"
#شامل "mime.h"
#شامل "progress.h"
#شامل "curlx/base64.h"
#شامل "cookie.h"
#شامل "vauth/vauth.h"
#شامل "vquic/vquic.h"
#شامل "http_digest.h"
#شامل "http_ntlm.h"
#شامل "http_negotiate.h"
#شامل "http_aws_sigv4.h"
#شامل "http_httpsig.h"
#شامل "url.h"
#شامل "urlapi-int.h"
#شامل "curl_share.h"
#شامل "dynhds.h"
#شامل "http.h"
#شامل "headers.h"
#شامل "select.h"
#شامل "parsedate.h" /* for the week day and month names */
#شامل "multiif.h"
#شامل "strcase.h"
#شامل "content_encoding.h"
#شامل "http_proxy.h"
#شامل "http2.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "curlx/strdup.h"
#شامل "altsvc.h"
#شامل "hsts.h"
#شامل "rtsp.h"
#شامل "ws.h"
#شامل "bufref.h"
#شامل "curlx/strparse.h"

تهی Curl_http_neg_init(ساختار Curl_easy *data, ساختار http_negotiation *neg)
{
  memset(neg, 0, اندازه(*neg));
  neg->accept_09 = data->set.http09_allowed;
  انتخاب(data->set.httpwant) {
  حالت CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    بشکن;
  حالت CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    بشکن;
  حالت CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    بشکن;
  حالت CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    بشکن;
  حالت CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    data->state.http_neg.h2_prior_knowledge = TRUE;
    بشکن;
  حالت CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    بشکن;
  حالت CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    بشکن;
  حالت CURL_HTTP_VERSION_NONE:
  پیشفرض:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    بشکن;
  }
}

CURLcode Curl_http_setup_conn(ساختار Curl_easy *data,
                              ساختار connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  اگر(data->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode result = Curl_conn_may_http3(data, conn);
    اگر(result)
      بازگشت result;
  }
  بازگشت CURLE_OK;
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
نویسه *Curl_checkProxyheaders(ساختار Curl_easy *data,
                             ثابت ساختار connectdata *conn,
                             ثابت نویسه *thisheader,
                             ثابت size_t thislen)
{
  ساختار curl_slist *head;

  برای(head = (conn->http_proxy.peer && data->set.sep_headers) ?
        data->set.proxyheaders : data->set.headers;
      head; head = head->next) {
    اگر(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      بازگشت head->data;
  }

  بازگشت پوچ;
}
#endif

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
ایستا bool header_has_مقدار(ثابت نویسه **headerp, ساختار Curl_str *outp)
{
  bool مقدار = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  اگر(مقدار) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  بازگشت مقدار;
}

ایستا bool http_header_is_empty(ثابت نویسه *header)
{
  ساختار Curl_str out;

  اگر(header_has_مقدار(&header, &out)) {
    بازگشت curlx_strlen(&out) == 0;
  }
  بازگشت TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
ایستا CURLcode copy_custom_مقدار(ثابت نویسه *header, نویسه **valp)
{
  ساختار Curl_str out = { 0 };

  /* find the end of the header name */
  اگر(header_has_مقدار(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    اگر(*valp)
      بازگشت CURLE_OK;
    بازگشت CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = پوچ;
  بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
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
نویسه *Curl_copy_header_مقدار(ثابت نویسه *header)
{
  ساختار Curl_str out;

  /* find the end of the header name */
  اگر(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_SIZE, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_SIZE);
    curlx_str_trimblanks(&out);
    بازگشت curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  بازگشت پوچ;
}

#ifndef CURL_DISABLE_HTTP_AUTH

#ifndef CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
ایستا CURLcode http_output_basic(ساختار Curl_easy *data,
                                  ساختار connectdata *conn, bool proxy)
{
  size_t size = 0;
  نویسه *authorization = پوچ;
  نویسه **p_hd;
  CURLcode result;
  ساختار Curl_creds *creds = پوچ;
  نویسه *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  اگر(proxy) {
#ifndef CURL_DISABLE_PROXY
    p_hd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#وگرنه
    (تهی)conn;
    بازگشت CURLE_NOT_BUILT_IN;
#endif
  }
  وگرنه {
    p_hd = &data->req.hd_auth;
    creds = data->state.creds;
  }

  اگر(!creds) {
    DEBUGASSERT(0);
    بازگشت CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  اگر(!out)
    بازگشت CURLE_OUT_OF_MEMORY;

  result = curlx_base64_encode((uint8_t *)out, strlen(out),
                               &authorization, &size);
  اگر(result)
    برو fail;

  اگر(!authorization) {
    result = CURLE_REMOTE_ACCESS_DENIED;
    برو fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  اگر(!*p_hd) {
    result = CURLE_OUT_OF_MEMORY;
    برو fail;
  }

fail:
  curlx_free(out);
  بازگشت result;
}

#endif

#ifndef CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
ایستا CURLcode http_output_bearer(ساختار Curl_easy *data)
{
  نویسه **userp;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(data->state.creds));
  userp = &data->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(data->state.creds));

  اگر(!*userp) {
    result = CURLE_OUT_OF_MEMORY;
    برو fail;
  }

fail:
  بازگشت result;
}
#endif

#endif

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
ایستا bool pickoneauth(ساختار auth *pick, unsigned long mask,
                        ساختار Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  unsigned long avail = pick->avail & pick->want & mask;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  اگر(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ifndef CURL_DISABLE_BEARER_AUTH
  وگرنه اگر((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  وگرنه اگر((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#endif
  وگرنه اگر(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ifndef CURL_DISABLE_BASIC_AUTH
  وگرنه اگر((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#endif
#ifndef CURL_DISABLE_AWS
  وگرنه اگر(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  وگرنه اگر(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#endif
  وگرنه {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  بازگشت picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
ایستا CURLcode http_perhapsrewind(ساختار Curl_easy *data,
                                   ساختار connectdata *conn)
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
  VERBOSE(ثابت نویسه *ongoing_auth = پوچ);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  اگر(needs_rewind) {
    infof(data, "Need to rewind upload for next request");
    Curl_creader_set_rewind(data, TRUE);
  }

  اگر(conn->bits.close)
    /* If we already decided to close this connection, we cannot veto. */
    بازگشت CURLE_OK;

  اگر(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ifdef USE_NTLM
    اگر((data->state.authproxy.picked == CURLAUTH_NTLM) ||
       (data->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      اگر((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
#ifdef USE_SPNEGO
    /* There is still data left to send */
    اگر((data->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (data->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      اگر((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
  }

  اگر(abort_upload) {
    اگر(upload_remain >= 0)
      infof(data, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    وگرنه
      infof(data, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    data->req.size = 0; /* do not download any more than 0 bytes */
    data->req.http_bodyless = TRUE;
  }
  بازگشت CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
ایستا bool http_should_fail(ساختار Curl_easy *data, صحیح httpcode)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  اگر(!data->set.http_fail_on_error)
    بازگشت FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  اگر(httpcode < 400)
    بازگشت FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  اگر(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    بازگشت FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  اگر((httpcode != 401) && (httpcode != 407))
    بازگشت TRUE;

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
  اگر((httpcode == 401) && !data->state.creds)
    بازگشت TRUE;
#ifndef CURL_DISABLE_PROXY
  اگر((httpcode == 407) && !data->conn->http_proxy.creds)
    بازگشت TRUE;
#endif

  بازگشت (bool)data->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(ساختار Curl_easy *data)
{
  ساختار connectdata *conn = data->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode result = CURLE_OK;
  unsigned long authmask = ~0UL;

  اگر(!Curl_creds_has_oauth_bearer(data->state.creds))
    authmask &= (unsigned long)~CURLAUTH_BEARER;

  اگر(100 <= data->req.httpcode && data->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    بازگشت CURLE_OK;

  اگر(data->state.authproblem)
    بازگشت data->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  اگر(data->state.creds &&
     ((data->req.httpcode == 401) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickhost = pickoneauth(&data->state.authhost, authmask, data->state.creds);
    اگر(!pickhost)
      data->state.authproblem = TRUE;
    وگرنه
      data->info.httpauthpicked = data->state.authhost.picked;
    اگر(data->state.authhost.picked == CURLAUTH_NTLM &&
       (data->req.httpversion_sent > 11)) {
      infof(data, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ifndef CURL_DISABLE_PROXY
  اگر(conn->http_proxy.creds &&
     ((data->req.httpcode == 407) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickproxy = pickoneauth(&data->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    اگر(!pickproxy)
      data->state.authproblem = TRUE;
    وگرنه
      data->info.proxyauthpicked = data->state.authproxy.picked;
  }
#endif

  اگر(pickhost || pickproxy) {
    result = http_perhapsrewind(data, conn);
    اگر(result)
      بازگشت result;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(data->req.newurl);
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    اگر(!data->req.newurl)
      بازگشت CURLE_OUT_OF_MEMORY;
  }
  وگرنه اگر((data->req.httpcode < 300) &&
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
    اگر(!data->req.newurl)
      بازگشت CURLE_OUT_OF_MEMORY;
    data->state.authhost.done = TRUE;
  }
  اگر(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          data->req.httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
  }

  بازگشت result;
}

#ifndef CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
ایستا CURLcode output_auth_headers(ساختار Curl_easy *data,
                                    ساختار connectdata *conn,
                                    ساختار auth *authstatus,
                                    ثابت نویسه *request,
                                    ثابت نویسه *path,
                                    bool proxy)
{
  ثابت نویسه *auth = پوچ;
  CURLcode result = CURLE_OK;
  (تهی)conn;

#ifdef CURL_DISABLE_DIGEST_AUTH
  (تهی)request;
  (تهی)path;
#endif
#ifndef CURL_DISABLE_AWS
  اگر((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    result = Curl_output_aws_sigv4(data);
    اگر(result)
      بازگشت result;
  }
  وگرنه
#endif
#ifndef CURL_DISABLE_HTTPSIG
  اگر((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    اگر(Curl_auth_allowed_to_host(data)) {
      auth = "HTTPSIG";
      result = Curl_output_httpsig(data);
      اگر(result)
        بازگشت result;
    }
    وگرنه
      authstatus->done = TRUE;
  }
  وگرنه
#endif
#ifdef USE_SPNEGO
  اگر(authstatus->picked == CURLAUTH_NEGOTIATE) {
    اگر(
#ifndef CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(data, conn,
                                        STRCONST("Proxy-authorization"))) ||
#endif
      (!proxy && !Curl_checkheaders(data, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(data))) {
      auth = "Negotiate";
      result = Curl_output_negotiate(data, conn, proxy);
      اگر(result)
        بازگشت result;
    }
    وگرنه
      authstatus->done = TRUE;
  }
  وگرنه
#endif
#ifdef USE_NTLM
  اگر(authstatus->picked == CURLAUTH_NTLM) {
    اگر(proxy || Curl_auth_allowed_to_host(data)) {
      auth = "NTLM";
      result = Curl_output_ntlm(data, proxy);
      اگر(result)
        بازگشت result;
    }
    وگرنه
      authstatus->done = TRUE;
  }
  وگرنه
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  اگر(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    result = Curl_output_digest(data,
                                proxy,
                                (ثابت unsigned نویسه *)request,
                                (ثابت unsigned نویسه *)path);
    اگر(result)
      بازگشت result;
  }
  وگرنه
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
  اگر(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    اگر(
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
      اگر(result)
        بازگشت result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
  اگر(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    اگر(!proxy && Curl_creds_has_oauth_bearer(data->state.creds) &&
       !Curl_checkheaders(data, STRCONST("Authorization"))) {
      auth = "Bearer";
      result = http_output_bearer(data);
      اگر(result)
        بازگشت result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif

  اگر(auth) {
#ifndef CURL_DISABLE_PROXY
    اگر(proxy)
      data->info.proxyauthpicked = authstatus->picked;
    وگرنه
      data->info.httpauthpicked = authstatus->picked;
    infof(data, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (data->state.creds ?
           data->state.creds->user : ""));
#وگرنه
    (تهی)proxy;
    infof(data, "Server auth using %s with user '%s'",
          auth, data->state.creds ?
          data->state.creds->user : "");
#endif
    authstatus->multipass = !authstatus->done;
  }
  وگرنه {
    authstatus->multipass = FALSE;
    اگر(proxy)
      data->info.proxyauthpicked = 0;
    وگرنه
      data->info.httpauthpicked = 0;
  }

  بازگشت result;
}

CURLcode Curl_http_output_auth(ساختار Curl_easy *data,
                               ساختار connectdata *conn,
                               ثابت نویسه *request,
                               Curl_HttpReq httpreq,
                               ثابت نویسه *path,
                               ثابت نویسه *query,
                               bool is_connect)
{
  CURLcode result = CURLE_OK;
  ساختار auth *authhost;
  ساختار auth *authproxy;
  ثابت نویسه *path_and_query = path;
  نویسه *tmp_str = پوچ;

  DEBUGASSERT(data);
  authhost = &data->state.authhost;
  authproxy = &data->state.authproxy;

  اگر(
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
    برو out;
  }

  اگر(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    اگر(!tmp_str) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
    path_and_query = tmp_str;
  }

  اگر(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  اگر(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ifndef CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  اگر(conn->bits.origin_is_proxy || is_connect) {
    result = output_auth_headers(data, conn, authproxy, request,
                                 path_and_query, TRUE);
    اگر(result)
      برو out;
  }
  وگرنه
#وگرنه
  (تهی)is_connect;
#endif /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->done = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  اگر(data->state.creds || Curl_auth_allowed_to_host(data))
    result = output_auth_headers(data, conn, authhost, request,
                                 path_and_query, FALSE);
  وگرنه
    authhost->done = TRUE;

  اگر(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_HEAD)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    data->req.authneg = TRUE;
  }
  وگرنه
    data->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  بازگشت result;
}

#وگرنه /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(ساختار Curl_easy *data,
                               ساختار connectdata *conn,
                               ثابت نویسه *request,
                               Curl_HttpReq httpreq,
                               ثابت نویسه *path,
                               ثابت نویسه *query,
                               bool is_connect)
{
  (تهی)data;
  (تهی)conn;
  (تهی)request;
  (تهی)httpreq;
  (تهی)path;
  (تهی)query;
  (تهی)is_connect;
  بازگشت CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP_AUTH, else */

#اگر defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
ایستا bool authcmp(ثابت نویسه *auth, ثابت نویسه *سطر)
{
  /* the auth string must not have an alnum following */
  size_t n = strlen(auth);
  بازگشت curl_strnequal(auth, سطر, n) && !ISALNUM(سطر[n]);
}
#endif

#ifdef USE_SPNEGO
ایستا CURLcode auth_spnego(ساختار Curl_easy *data,
                            bool proxy,
                            ثابت نویسه *auth,
                            ساختار auth *authp,
                            uint32_t *availp)
{
  اگر((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    اگر(authp->picked == CURLAUTH_NEGOTIATE) {
      ساختار connectdata *conn = data->conn;
      CURLcode result = Curl_درون_داد_negotiate(data, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      اگر(!result) {
        curlx_free(data->req.newurl);
        data->req.newurl = Curl_bufref_dup(&data->state.url);
        اگر(!data->req.newurl)
          بازگشت CURLE_OUT_OF_MEMORY;
        data->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      وگرنه
        data->state.authproblem = TRUE;
    }
  }
  بازگشت CURLE_OK;
}
#endif

#ifdef USE_NTLM
ایستا CURLcode auth_ntlm(ساختار Curl_easy *data,
                          bool proxy,
                          ثابت نویسه *auth,
                          ساختار auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  اگر((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    اگر(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode result = Curl_درون_داد_ntlm(data, proxy, auth);
      اگر(!result)
        data->state.authproblem = FALSE;
      وگرنه {
        اگر(result == CURLE_OUT_OF_MEMORY)
          بازگشت result;
        infof(data, "NTLM authentication problem, ignoring.");
        data->state.authproblem = TRUE;
      }
    }
  }
  بازگشت CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_DIGEST_AUTH
ایستا CURLcode auth_digest(ساختار Curl_easy *data,
                            bool proxy,
                            ثابت نویسه *auth,
                            ساختار auth *authp,
                            uint32_t *availp)
{
  اگر(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(data, "Ignoring duplicate digest auth header.");
  }
  وگرنه اگر(Curl_auth_is_digest_supported()) {
    CURLcode result;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    result = Curl_درون_داد_digest(data, proxy, auth);
    اگر(result) {
      اگر(result == CURLE_OUT_OF_MEMORY)
        بازگشت result;
      infof(data, "Digest authentication problem, ignoring.");
      data->state.authproblem = TRUE;
    }
  }
  بازگشت CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BASIC_AUTH
ایستا CURLcode auth_basic(ساختار Curl_easy *data,
                           ساختار auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  اگر(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Basic authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  بازگشت CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BEARER_AUTH
ایستا CURLcode auth_bearer(ساختار Curl_easy *data,
                            ساختار auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  اگر(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Bearer authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  بازگشت CURLE_OK;
}
#endif

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_درون_داد_auth(ساختار Curl_easy *data, bool proxy,
                              ثابت نویسه *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#اگر defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  ساختار auth *authp;
  CURLcode result = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(data);

  اگر(proxy) {
    availp = &data->info.proxyauthavail;
    authp = &data->state.authproxy;
  }
  وگرنه {
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

  وقتی(*auth) {
#ifdef USE_SPNEGO
    اگر(authcmp("Negotiate", auth))
      result = auth_spnego(data, proxy, auth, authp, availp);
#endif
#ifdef USE_NTLM
    اگر(!result && authcmp("NTLM", auth))
      result = auth_ntlm(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
    اگر(!result && authcmp("Digest", auth))
      result = auth_digest(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
    اگر(!result && authcmp("Basic", auth))
      result = auth_basic(data, authp, availp);
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
    اگر(authcmp("Bearer", auth))
      result = auth_bearer(data, authp, availp);
#endif

    اگر(result)
      بشکن;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    اگر(auth) /* if we are on a comma, skip it */
      auth++;
    وگرنه
      بشکن;
    curlx_str_passblanks(&auth);
  }
  بازگشت result;
#وگرنه
  (تهی)data;
  (تهی)proxy;
  (تهی)auth;
  /* nothing to do when disabled */
  بازگشت CURLE_OK;
#endif
}

ایستا تهی http_switch_to_get(ساختار Curl_easy *data, صحیح code)
{
  ثابت نویسه *req = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  اگر((req || data->state.httpreq != HTTPREQ_GET) &&
     (data->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((تهی)code);
    infof(data, "Switch to GET because of %d response", code);
    data->state.http_ignorecustom = TRUE;
  }
  وگرنه اگر(req && (data->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(data, "Stick to %s instead of GET", req);

  data->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(data, FALSE);
}

#define HTTPREQ_IS_POST(data)                    \
  ((data)->state.httpreq == HTTPREQ_POST ||      \
   (data)->state.httpreq == HTTPREQ_POST_FORM || \
   (data)->state.httpreq == HTTPREQ_POST_MIME)

ایستا CURLcode set_auto_referer(ساختار Curl_easy *data)
{
  CURLU *u;
  نویسه *referer = پوچ;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&data->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  اگر(!u)
    بازگشت CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&data->state.url), 0);
  اگر(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, پوچ, 0);
  اگر(!uc)
    uc = curl_url_set(u, CURLUPART_USER, پوچ, 0);
  اگر(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, پوچ, 0);
  اگر(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  اگر(uc || !referer)
    بازگشت CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&data->state.referer, referer, 0, curl_free);
  بازگشت CURLE_OK;
}

ایستا CURLcode follow_parse_url(ساختار Curl_easy *data, ثابت نویسه *newurl,
                                 followtype type, نویسه **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(data->state.uh);
  uc = curl_url_set(data->state.uh, CURLUPART_URL, newurl, (unsigned صحیح)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  اگر(uc) {
    اگر((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(data, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      بازگشت Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    اگر(!*follow_urlp)
      بازگشت CURLE_OUT_OF_MEMORY;
  }
  وگرنه {
    CURLU *u = curl_url();
    اگر(!u)
      بازگشت CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&data->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    اگر(!uc)
      uc = curl_url_get(data->state.uh, CURLUPART_URL, follow_urlp, 0);
    اگر(uc) {
      curl_url_cleanup(u);
      بازگشت Curl_uc_to_curlcode(uc);
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, data->state.uh);
      curl_url_cleanup(u);
      اگر(!same_origin)
        Curl_auth_digest_cleanup(&data->state.digest);
    }
#وگرنه
    curl_url_cleanup(u);
#endif
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode follow_fake(ساختار Curl_easy *data, نویسه *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  اگر(!uc) {
    CURLU *u = curl_url();
    نویسه *nocred = پوچ;

    اگر(!u) {
      curlx_free(follow_url);
      بازگشت CURLE_OUT_OF_MEMORY;
    }
    اگر(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, پوچ, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, پوچ, 0))
      (تهی)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    اگر(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  data->info.wouldredirect = follow_url;

  اگر(reachedmax) {
    failf(data, "Maximum (%d) redirects followed", data->set.maxredirs);
    بازگشت CURLE_TOO_MANY_REDIRECTS;
  }
  بازگشت CURLE_OK;
}

ایستا bool follow_switch_get(ساختار Curl_easy *data)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  انتخاب(data->info.httpcode) {
  حالت 301: /* Moved Permanently */
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
    اگر(HTTPREQ_IS_POST(data) && !data->set.post301) {
      http_switch_to_get(data, 301);
      بازگشت TRUE;
    }
    بشکن;
  حالت 302: /* Found */
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
    اگر(HTTPREQ_IS_POST(data) && !data->set.post302) {
      http_switch_to_get(data, 302);
      بازگشت TRUE;
    }
    بشکن;
  حالت 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    اگر(!HTTPREQ_IS_POST(data) || !data->set.post303) {
      http_switch_to_get(data, 303);
      بازگشت TRUE;
    }
    بشکن;

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
  پیشفرض:
    بشکن;
  }
  بازگشت FALSE;
}

CURLcode Curl_http_follow(ساختار Curl_easy *data, ثابت نویسه *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  نویسه *follow_url = پوچ;
  CURLUcode uc;
  CURLcode result;
  CURLcode rewind_result;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  اگر(type != FOLLOW_FAKE)
    data->state.requests++; /* count all real follows */
  اگر(type == FOLLOW_REDIR) {
    اگر((data->set.maxredirs != -1) &&
       (data->state.followlocation >= data->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    وگرنه {
      data->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      اگر(data->set.http_auto_referer) {
        result = set_auto_referer(data);
        اگر(result)
          بازگشت result;
      }
    }
  }

  اگر((type != FOLLOW_RETRY) &&
     (data->req.httpcode != 401) && (data->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, پوچ, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  result = follow_parse_url(data, newurl, type, &follow_url, &uc);
  اگر(result)
    بازگشت result;

  DEBUGASSERT(follow_url);

  اگر(type == FOLLOW_FAKE)
    بازگشت follow_fake(data, follow_url, uc, reachedmax);

  اگر(disallowport)
    data->state.allow_port = FALSE;

  Curl_bufref_set(&data->state.url, follow_url, 0, curl_free);
  rewind_result = Curl_req_soft_reset(&data->req, data);
  infof(data, "Issue another request to this URL: '%s'", follow_url);
  اگر((data->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    data->state.http_ignorecustom = TRUE;
    infof(data, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(data);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  اگر(rewind_result && !switch_to_get)
    بازگشت rewind_result;

  Curl_pgrsTime(data, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(data);

  بازگشت CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(ثابت نویسه *headerline, /* line to check */
                        ثابت نویسه *header, /* header keyword _with_ colon */
                        ثابت size_t hlen, /* len of the keyword in bytes */
                        ثابت نویسه *content, /* content string to find */
                        ثابت size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  ثابت نویسه *p;
  ساختار Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  اگر(!curl_strnequal(headerline, header, hlen))
    بازگشت FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  اگر(curlx_str_cspn(&p, &val, "\r\n"))
    بازگشت FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  اگر(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    برای(len = curlx_strlen(&val); len >= clen;) {
      ساختار Curl_str next;
      ثابت نویسه *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      اگر(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        بازگشت TRUE; /* match! */
      /* advance to the next comma */
      اگر(curlx_str_until(&p, &next, len, ',') ||
         curlx_str_single(&p, ','))
        بشکن; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      انجامبده
        curlx_str_passblanks(&p);
      وقتی(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      اگر((size_t)(p - o) > len)
        بشکن;
      len -= (p - o);
    }
  }
  بازگشت FALSE; /* no match */
}

ساختار cr_exp100_ctx {
  ساختار Curl_creader super;
  ساختار curltime start; /* time started waiting */
  شمارش expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

ایستا تهی http_exp100_continue(ساختار Curl_easy *data,
                                 ساختار Curl_creader *reader)
{
  ساختار cr_exp100_ctx *ctx = reader->ctx;
  اگر(ctx->state > EXP100_SEND_DATA) {
    ctx->state = EXP100_SEND_DATA;
    Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
  }
}

ایستا CURLcode cr_exp100_read(ساختار Curl_easy *data,
                               ساختار Curl_creader *reader,
                               نویسه *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ساختار cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  انتخاب(ctx->state) {
  حالت EXP100_SENDING_REQUEST:
    اگر(!Curl_req_sendbuf_empty(data)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(data, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      بازگشت CURLE_OK;
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
    بازگشت CURLE_OK;
  حالت EXP100_FAILED:
    DEBUGF(infof(data, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    بازگشت CURLE_READ_ERROR;
  حالت EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->start);
    اگر(ms < data->set.expect_100_timeout) {
      DEBUGF(infof(data, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      بازگشت CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(data, reader);
    infof(data, "Done waiting for 100-continue");
    FALLTHROUGH();
  پیشفرض:
    DEBUGF(infof(data, "cr_exp100_read, pass through"));
    بازگشت Curl_creader_read(data, reader->next, buf, blen, nread, eos);
  }
}

ایستا تهی cr_exp100_done(ساختار Curl_easy *data,
                           ساختار Curl_creader *reader, صحیح premature)
{
  ساختار cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_DATA;
  Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
}

ایستا ثابت ساختار Curl_crtype cr_exp100 = {
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
  اندازه(ساختار cr_exp100_ctx)
};

ایستا CURLcode http_exp100_add_reader(ساختار Curl_easy *data)
{
  ساختار Curl_creader *reader = پوچ;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_exp100, CURL_CR_PROTOCOL);
  اگر(!result)
    result = Curl_creader_add(data, reader);
  اگر(!result) {
    ساختار cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  اگر(result && reader)
    Curl_creader_free(data, reader);
  بازگشت result;
}

ایستا تهی http_exp100_got100(ساختار Curl_easy *data)
{
  ساختار Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  اگر(r)
    http_exp100_continue(data, r);
}

ایستا bool http_exp100_is_waiting(ساختار Curl_easy *data)
{
  ساختار Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  اگر(r) {
    ساختار cr_exp100_ctx *ctx = r->ctx;
    بازگشت ctx->state == EXP100_AWAITING_CONTINUE;
  }
  بازگشت FALSE;
}

ایستا تهی http_exp100_send_anyway(ساختار Curl_easy *data)
{
  ساختار Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  اگر(r)
    http_exp100_continue(data, r);
}

ایستا bool http_exp100_is_selected(ساختار Curl_easy *data)
{
  ساختار Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  بازگشت !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(ساختار Curl_easy *data,
                                 ساختار easy_pollset *ps)
{
  /* write mode */
  بازگشت Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(ساختار Curl_easy *data,
                                   ساختار easy_pollset *ps)
{
  ساختار connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  اگر(CURL_REQ_WANT_RECV(data)) {
    result = Curl_pollset_add_in(data, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  اگر(!result && Curl_req_want_send(data) && !http_exp100_is_waiting(data)) {
    result = Curl_pollset_add_out(data, ps, conn->sock[FIRSTSOCKET]);
  }
  بازگشت result;
}

ایستا CURLcode http_write_header(ساختار Curl_easy *data,
                                  ثابت نویسه *hd, size_t hdlen)
{
  CURLcode result;
  صحیح writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((data->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  result = Curl_client_write(data, writetype, hd, hdlen);
  اگر(result)
    بازگشت result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  اگر(result)
    بازگشت result;

  data->req.deductheadercount = (100 <= data->req.httpcode &&
                                 199 >= data->req.httpcode) ?
    data->req.headerbytecount : 0;
  بازگشت result;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(ساختار Curl_easy *data,
                        CURLcode status, bool premature)
{
  ساختار connectdata *conn = data->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  data->state.authhost.multipass = FALSE;
  data->state.authproxy.multipass = FALSE;

  اگر(curlx_dyn_len(&data->state.headerb)) {
    (تهی)http_write_header(data, curlx_dyn_ptr(&data->state.headerb),
                            curlx_dyn_len(&data->state.headerb));
  }
  curlx_dyn_reset(&data->state.headerb);

  اگر(status)
    بازگشت status;

  اگر(!premature && /* this check is pointless when DONE is called before the
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
    بازگشت CURLE_GOT_NOTHING;
  }

  بازگشت CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
ایستا bool http_may_use_1_1(ثابت ساختار Curl_easy *data)
{
  ثابت ساختار connectdata *conn = data->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  اگر(data->state.http_neg.rcvd_min == 10)
    بازگشت FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  اگر(conn && conn->httpversion_seen == 10)
    بازگشت FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  اگر(data->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    بازگشت FALSE;
  /* We are not restricted to use 1.0 only. */
  بازگشت !data->state.http_neg.only_10;
}

ایستا unsigned نویسه http_request_version(ساختار Curl_easy *data)
{
  unsigned نویسه v = Curl_conn_http_version(data, data->conn);
  اگر(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(data) ? 11 : 10;
  }
  بازگشت v;
}

ایستا ثابت نویسه *get_http_string(صحیح httpversion)
{
  انتخاب(httpversion) {
  حالت 30:
    بازگشت "3";
  حالت 20:
    بازگشت "2";
  حالت 11:
    بازگشت "1.1";
  پیشفرض:
    بازگشت "1.0";
  }
}

CURLcode Curl_add_custom_headers(ساختار Curl_easy *data,
                                 bool is_connect, صحیح httpversion,
                                 ساختار dynbuf *req)
{
  ساختار curl_slist *h[2];
  ساختار curl_slist *headers;
  صحیح numlists = 1; /* by default */
  صحیح i;

#ifndef CURL_DISABLE_PROXY
  شمارش Curl_proxy_use proxy;

  اگر(is_connect)
    proxy = HEADER_CONNECT;
  وگرنه
    proxy = data->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  انتخاب(proxy) {
  حالت HEADER_SERVER:
    h[0] = data->set.headers;
    بشکن;
  حالت HEADER_PROXY:
    h[0] = data->set.headers;
    اگر(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    بشکن;
  حالت HEADER_CONNECT:
    اگر(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    وگرنه
      h[0] = data->set.headers;
    بشکن;
  حالت HEADER_CONNECT_UDP:
    اگر(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    وگرنه
      h[0] = data->set.headers;
    بشکن;
  }
#وگرنه
  (تهی)is_connect;
  h[0] = data->set.headers;
#endif

  /* loop through one or two lists */
  برای(i = 0; i < numlists; i++) {
    برای(headers = h[i]; headers; headers = headers->next) {
      CURLcode result = CURLE_OK;
      bool blankheader = FALSE;
      ساختار Curl_str name;
      ثابت نویسه *p = headers->data;
      ثابت نویسه *origp = p;
      size_t hlen = strlen(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      اگر(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      وگرنه {
        p = origp;
        اگر(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          ساختار Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          اگر(!curlx_strlen(&val))
            /* no content, do not send this */
            ادامه;
        }
        وگرنه
          /* no colon */
          ادامه;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      اگر(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      وگرنه اگر(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وگرنه اگر(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وگرنه اگر(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      وگرنه اگر(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      وگرنه اگر((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      وگرنه اگر((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      وگرنه اگر(blankheader) {
        result = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        اگر(!result)
          result = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      وگرنه
        result = curlx_dyn_addf(req, "%s\r\n", origp);

      اگر(result)
        بازگشت result;
    }
  }

  بازگشت CURLE_OK;
}

#ifndef CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(ساختار Curl_easy *data,
                                ساختار dynbuf *req)
{
  ثابت ساختار tm *tm;
  ساختار tm keeptime;
  CURLcode result;
  نویسه datestr[80];
  ثابت نویسه *condp;
  size_t len;

  اگر(data->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    بازگشت CURLE_OK;

  result = curlx_gmtime(data->set.timevalue, &keeptime);
  اگر(result) {
    failf(data, "Invalid TIMEVALUE");
    بازگشت result;
  }
  tm = &keeptime;

  انتخاب(data->set.timecondition) {
  پیشفرض:
    DEBUGF(infof(data, "invalid time condition"));
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  حالت CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    بشکن;
  حالت CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    بشکن;
  حالت CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    بشکن;
  }

  اگر(Curl_checkheaders(data, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    بازگشت CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, اندازه(datestr),
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
  بازگشت result;
}
#وگرنه
/* disabled */
CURLcode Curl_add_timecondition(ساختار Curl_easy *data,
                                ساختار dynbuf *req)
{
  (تهی)data;
  (تهی)req;
  بازگشت CURLE_OK;
}
#endif

تهی Curl_http_method(ساختار Curl_easy *data,
                      ثابت نویسه **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)data->state.httpreq;
  ثابت نویسه *request;
#ifndef CURL_DISABLE_WEBSOCKETS
  اگر(data->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  وگرنه
#endif
  اگر((data->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     data->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  اگر(!data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
  }
  وگرنه {
    اگر(data->req.no_body)
      request = "HEAD";
    وگرنه {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_HEAD));
      انتخاب(httpreq) {
      حالت HTTPREQ_POST:
      حالت HTTPREQ_POST_FORM:
      حالت HTTPREQ_POST_MIME:
        request = "POST";
        بشکن;
      حالت HTTPREQ_PUT:
        request = "PUT";
        بشکن;
      پیشفرض: /* this should never happen */
      حالت HTTPREQ_GET:
        request = "GET";
        بشکن;
      حالت HTTPREQ_HEAD:
        request = "HEAD";
        بشکن;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

ایستا CURLcode http_set_aptr_host(ساختار Curl_easy *data)
{
  ساختار connectdata *conn = data->conn;
  ثابت نویسه *ptr = پوچ;

  curlx_safefree(data->state.http_host);
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(data->req.cookiehost);
#endif

  اگر(Curl_peer_equal(data->state.initial_origin, data->state.origin))
    ptr = Curl_checkheaders(data, STRCONST("Host"));

  اگر(ptr) {
#ifndef CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    نویسه *cookiehost;
    CURLcode result = copy_custom_مقدار(ptr, &cookiehost);
    اگر(result)
      بازگشت result;
    اگر(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    وگرنه {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      اگر(*cookiehost == '[') {
        نویسه *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, strlen(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        اگر(closingbracket)
          *closingbracket = 0;
      }
      وگرنه {
        صحیح startsearch = 0;
        نویسه *colon = strchr(cookiehost + startsearch, ':');
        اگر(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      data->req.cookiehost = cookiehost;
    }
#endif

    اگر(!curl_strequal("Host:", ptr)) {
      data->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      اگر(!data->state.http_host)
        بازگشت CURLE_OUT_OF_MEMORY;
    }
  }
  وگرنه {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)data->state.origin->ipv6;
    ساختار dynbuf tmp;
    size_t hlen;
    CURLcode result;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    result = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    اگر(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("["));
    اگر(!result)
      result = curlx_dyn_add(&tmp, data->state.origin->hostname);
    اگر(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("]"));
    اگر(!result &&
       ((data->state.origin->port != data->state.origin->scheme->defport) ||
       (data->state.origin->scheme->family != conn->scheme->family))) {
      result = curlx_dyn_addf(&tmp, ":%u", data->state.origin->port);
    }

    data->state.http_host = result ? پوچ : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    بازگشت result;
  }
  بازگشت CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
ایستا CURLcode http_target(ساختار Curl_easy *data,
                            ساختار dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ثابت نویسه *path = data->state.up.path;
  ثابت نویسه *query = data->state.up.query;
#ifndef CURL_DISABLE_PROXY
  ساختار connectdata *conn = data->conn;
#endif

  اگر(CURL_EASY_STR(data, STRING_TARGET)) {
    path = CURL_EASY_STR(data, STRING_TARGET);
    query = پوچ;
  }

#ifndef CURL_DISABLE_PROXY
  اگر(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    نویسه *url;
    CURLU *h = curl_url_dup(data->state.uh);
    اگر(!h)
      بازگشت CURLE_OUT_OF_MEMORY;

    اگر(!data->state.origin->ipv6 &&
       (data->state.origin->user_hostname != data->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, data->state.origin->hostname, 0);
      اگر(uc) {
        curl_url_cleanup(h);
        بازگشت CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, پوچ, 0);
    اگر(uc) {
      curl_url_cleanup(h);
      بازگشت CURLE_OUT_OF_MEMORY;
    }

    اگر(data->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, پوچ, 0);
      اگر(uc) {
        curl_url_cleanup(h);
        بازگشت CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, پوچ, 0);
      اگر(uc) {
        curl_url_cleanup(h);
        بازگشت CURLE_OUT_OF_MEMORY;
      }
    }
    وگرنه اگر(data->state.creds && (data->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        data->state.creds->user, CURLU_URLENCODE);
      اگر(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          data->state.creds->passwd, CURLU_URLENCODE);
      اگر(uc) {
        curl_url_cleanup(h);
        بازگشت Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    اگر(uc) {
      curl_url_cleanup(h);
      بازگشت CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    result = curlx_dyn_add(r, CURL_EASY_STR(data, STRING_TARGET) ?
      CURL_EASY_STR(data, STRING_TARGET) : url);
    curlx_free(url);
    اگر(result)
      بازگشت result;

    اگر((data->state.origin->scheme == &Curl_scheme_ftp) &&
       data->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = strlen(path);
      bool type_present = FALSE;
      اگر((len >= 7) && !memcmp(&path[len - 7], ";type=", 6)) {
        انتخاب(Curl_raw_toupper(path[len - 1])) {
        حالت 'A':
        حالت 'D':
        حالت 'I':
          type_present = TRUE;
          بشکن;
        }
      }
      اگر(!type_present) {
        result = curlx_dyn_addf(r, ";type=%c",
                                data->state.prefer_ascii ? 'a' : 'i');
        اگر(result)
          بازگشت result;
      }
    }
  }

  وگرنه
#endif
  {
    result = curlx_dyn_add(r, path);
    اگر(result)
      بازگشت result;
    اگر(query)
      result = curlx_dyn_addf(r, "?%s", query);
  }

  بازگشت result;
}

#اگر !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
ایستا CURLcode set_post_reader(ساختار Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result;

  انتخاب(httpreq) {
#ifndef CURL_DISABLE_MIME
  حالت HTTPREQ_POST_MIME:
    data->state.mimepost = data->set.mimepostp;
    بشکن;
#endif
#ifndef CURL_DISABLE_FORM_API
  حالت HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    اگر(!data->state.formp) {
      data->state.formp = curlx_calloc(1, اندازه(curl_mimepart));
      اگر(!data->state.formp)
        بازگشت CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(data->state.formp);
      result = Curl_getformdata(data, data->state.formp, data->set.httppost,
                                data->state.fread_func);
      اگر(result) {
        curlx_safefree(data->state.formp);
        بازگشت result;
      }
      data->state.mimepost = data->state.formp;
    }
    بشکن;
#endif
  پیشفرض:
    data->state.mimepost = پوچ;
    بشکن;
  }

  انتخاب(httpreq) {
  حالت HTTPREQ_POST_FORM:
  حالت HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ifndef CURL_DISABLE_MIME
    اگر(data->state.mimepost) {
      ثابت نویسه *cthdr = Curl_checkheaders(data, STRCONST("Content-Type"));

      /* Read and seek body only. */
      data->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      اگر(cthdr)
        برای(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      وگرنه اگر(data->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(data->state.mimepost, data->set.headers, 0);
      result = Curl_mime_prepare_headers(data, data->state.mimepost, cthdr,
                                         پوچ, MIMESTRATEGY_FORM);
      اگر(result)
        بازگشت result;
      curl_mime_headers(data->state.mimepost, پوچ, 0);
      result = Curl_creader_set_mime(data, data->state.mimepost);
      اگر(result)
        بازگشت result;
    }
    وگرنه
#endif
    {
      result = Curl_creader_set_null(data);
    }
    data->state.infilesize = Curl_creader_total_length(data);
    بازگشت result;

  پیشفرض:
    بازگشت Curl_creader_set_null(data);
  }
  /* never reached */
}
#endif

ایستا CURLcode set_reader(ساختار Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t postsize = data->state.infilesize;

  DEBUGASSERT(data->conn);

  اگر(data->req.authneg) {
    بازگشت Curl_creader_set_null(data);
  }

  انتخاب(httpreq) {
  حالت HTTPREQ_PUT: /* Let's PUT the data to the server! */
    بازگشت postsize ? Curl_creader_set_fread(data, postsize) :
      Curl_creader_set_null(data);

#اگر !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالت HTTPREQ_POST_FORM:
  حالت HTTPREQ_POST_MIME:
    بازگشت set_post_reader(data, httpreq);
#endif

  حالت HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    اگر(!postsize) {
      result = Curl_creader_set_null(data);
    }
    وگرنه اگر(data->set.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      اگر(plen == SIZE_MAX)
        بازگشت CURLE_OUT_OF_MEMORY;
      وگرنه اگر(plen)
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      وگرنه
        result = Curl_creader_set_null(data);
    }
    وگرنه {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      نویسه *ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
      اگر(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      result = Curl_creader_set_fread(data, chunked ? -1 : postsize);
    }
    بازگشت result;

  پیشفرض:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    data->state.infilesize = 0;
    بازگشت Curl_creader_set_null(data);
  }
  /* not reached */
}

ایستا CURLcode http_resume(ساختار Curl_easy *data, Curl_HttpReq httpreq)
{
  اگر((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     data->state.resume_from) {
    failf(data, "HTTP upload cannot be resumed");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode http_req_set_TE(ساختار Curl_easy *data,
                                ساختار dynbuf *req,
                                صحیح httpversion)
{
  CURLcode result = CURLE_OK;
  ثابت نویسه *ptr;

  ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
  اگر(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    data->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    اگر(data->req.upload_chunky && (httpversion >= 20)) {
      infof(data, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      data->req.upload_chunky = FALSE;
    }
  }
  وگرنه {
    curl_off_t req_clen = Curl_creader_total_length(data);

    اگر(req_clen < 0) {
      /* indeterminate request content length */
      اگر(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        data->req.upload_chunky = (httpversion < 20);
      }
      وگرنه {
        failf(data, "Chunky upload is not supported by HTTP 1.0");
        بازگشت CURLE_UPLOAD_FAILED;
      }
    }
    وگرنه {
      /* else, no chunky upload */
      data->req.upload_chunky = FALSE;
    }

    اگر(data->req.upload_chunky)
      result = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  بازگشت result;
}

ایستا CURLcode addexpect(ساختار Curl_easy *data, ساختار dynbuf *r,
                          صحیح httpversion, bool *announced_exp100)
{
  CURLcode result;
  نویسه *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  اگر(data->req.upgr101 != UPGR101_NONE)
    بازگشت CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(data, STRCONST("Expect"));
  اگر(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  وگرنه اگر(!data->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_length(data);
    اگر(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      result = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      اگر(result)
        بازگشت result;
      *announced_exp100 = TRUE;
    }
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode http_add_content_hds(ساختار Curl_easy *data,
                                     ساختار dynbuf *r,
                                     صحیح httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(data->conn);
  اگر(data->req.upload_chunky) {
    result = Curl_httpchunk_add_reader(data);
    اگر(result)
      بازگشت result;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_total_length(data);
  انتخاب(httpreq) {
  حالت HTTPREQ_PUT:
  حالت HTTPREQ_POST:
#اگر !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالت HTTPREQ_POST_FORM:
  حالت HTTPREQ_POST_MIME:
#endif
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    اگر(req_clen >= 0 && !data->req.upload_chunky &&
       (data->req.authneg ||
        !Curl_checkheaders(data, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      result = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    اگر(result)
      برو out;

#ifndef CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    اگر(data->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      ساختار curl_slist *hdr;

      برای(hdr = data->state.mimepost->curlheaders; hdr; hdr = hdr->next) {
        result = curlx_dyn_addf(r, "%s\r\n", hdr->data);
        اگر(result)
          برو out;
      }
    }
#endif
    اگر(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(data, STRCONST("Content-Type"))) {
      result = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      اگر(result)
        برو out;
    }
    result = addexpect(data, r, httpversion, &announced_exp100);
    اگر(result)
      برو out;
    بشکن;
  پیشفرض:
    بشکن;
  }

  Curl_pgrsSetUploadSize(data, req_clen);
  اگر(announced_exp100)
    result = http_exp100_add_reader(data);

out:
  بازگشت result;
}

#ifndef CURL_DISABLE_COOKIES

ایستا CURLcode http_cookies(ساختار Curl_easy *data,
                             ساختار dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ثابت نویسه *addcookies = پوچ;
  bool linecap = FALSE;
  اگر(CURL_EASY_STR(data, STRING_COOKIE) &&
     !Curl_checkheaders(data, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(data))
    addcookies = CURL_EASY_STR(data, STRING_COOKIE);

  اگر(data->cookies || addcookies) {
    ساختار Curl_llist list;
    صحیح شمار = 0;

    اگر(data->cookies && data->state.cookie_engine) {
      bool okay;
      ثابت نویسه *host = data->req.cookiehost ?
        data->req.cookiehost : data->state.origin->hostname;
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      result = Curl_cookie_getlist(data, &okay, host, &list);
      اگر(!result && okay) {
        ساختار Curl_llist_گره *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        برای(n = Curl_llist_head(&list); n; n = Curl_گره_next(n)) {
          ساختار Cookie *co = Curl_گره_elem(n);
          اگر(co->مقدار) {
            size_t add;
            اگر(!شمار) {
              result = curlx_dyn_addn(r, STRCONST("Cookie: "));
              اگر(result)
                بشکن;
            }
            add = strlen(co->name) + strlen(co->مقدار) + 1;
            اگر(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(data, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              بشکن;
            }
            result = curlx_dyn_addf(r, "%s%s=%s", شمار ? "; " : "",
                                    co->name, co->مقدار);
            اگر(result)
              بشکن;
            clen += add + (شمار ? 2 : 0);
            شمار++;
          }
        }
        Curl_llist_destroy(&list, پوچ);
      }
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
    اگر(addcookies && !result && !linecap) {
      اگر(!شمار)
        result = curlx_dyn_addn(r, STRCONST("Cookie: "));
      اگر(!result) {
        result = curlx_dyn_addf(r, "%s%s", شمار ? "; " : "", addcookies);
        شمار++;
      }
    }
    اگر(شمار && !result)
      result = curlx_dyn_addn(r, STRCONST("\r\n"));

    اگر(result)
      بازگشت result;
  }
  بازگشت result;
}
#وگرنه
#define http_cookies(a, b) CURLE_OK
#endif

ایستا CURLcode http_range(ساختار Curl_easy *data,
                           Curl_HttpReq httpreq)
{
  اگر(data->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    اگر(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_HEAD)) &&
       !Curl_checkheaders(data, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);
      data->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 data->state.range);
      اگر(!data->state.rangeline)
        بازگشت CURLE_OUT_OF_MEMORY;
    }
    وگرنه اگر((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(data, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_total_length(data);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);

      اگر(data->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      وگرنه اگر(data->state.resume_from) {
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
      وگرنه {
        /* Range was selected and then we pass the incoming range and append
           total size */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        data->state.range, req_clen);
      }
      اگر(!data->state.rangeline)
        بازگشت CURLE_OUT_OF_MEMORY;
    }
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode http_firstwrite(ساختار Curl_easy *data)
{
  ساختار connectdata *conn = data->conn;
  ساختار SingleRequest *k = &data->req;

  اگر(data->req.newurl) {
    اگر(conn->bits.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      بازگشت CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(data, "Ignoring the response-body");
  }
  اگر(data->state.resume_from && !k->content_range &&
     (data->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    اگر(k->size == data->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(data, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      بازگشت CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(data, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    بازگشت CURLE_RANGE_ERROR;
  }

  اگر(data->set.timecondition && !data->state.range &&
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
    بازگشت CURLE_OK;
  } /* we have a time condition */

  بازگشت CURLE_OK;
}

ایستا CURLcode http_check_new_conn(ساختار Curl_easy *data)
{
  ساختار connectdata *conn = data->conn;
  ثابت نویسه *info_version = پوچ;
  ثابت نویسه *alpn;
  CURLcode result;

  alpn = Curl_conn_get_alpn_negotiated(data, conn);
  اگر(alpn && !strcmp("h3", alpn)) {
#ifndef CURL_DISABLE_PROXY
    اگر(!conn->bits.origin_is_proxy)
#endif
      DEBUGASSERT(Curl_conn_http_version(data, conn) == 30);
    info_version = "HTTP/3";
  }
  وگرنه اگر(alpn && !strcmp("h2", alpn)) {
#ifndef CURL_DISABLE_PROXY
    اگر((Curl_conn_http_version(data, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      result = Curl_http2_switch(data);
      اگر(result)
        بازگشت result;
    }
    وگرنه
#endif
    DEBUGASSERT(Curl_conn_http_version(data, conn) == 20);
    info_version = "HTTP/2";
  }
  وگرنه {
    /* Check if user wants to use HTTP/2 with clear TCP */
    اگر(Curl_http2_may_switch(data)) {
      DEBUGF(infof(data, "HTTP/2 over clean TCP"));
      result = Curl_http2_switch(data);
      اگر(result)
        بازگشت result;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    وگرنه
      info_version = "HTTP/1.x";
  }

  اگر(info_version)
    infof(data, "using %s", info_version);
  بازگشت CURLE_OK;
}

ایستا CURLcode http_add_connection_hd(ساختار Curl_easy *data,
                                       ساختار dynbuf *req)
{
  ساختار curl_slist *head;
  ثابت نویسه *sep = "Connection: ";
  CURLcode result = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  برای(head = data->set.headers; head; head = head->next) {
    اگر(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      نویسه *مقدار;
      result = copy_custom_مقدار(head->data, &مقدار);
      اگر(result)
        بازگشت result;
      result = curlx_dyn_addf(req, "%s%s", sep, مقدار);
      sep = ", ";
      curlx_free(مقدار);
      بشکن; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  اگر(!result && data->state.http_hd_te) {
    result = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  اگر(!result && data->state.http_hd_upgrade) {
    result = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  اگر(!result && data->state.http_hd_h2_settings) {
    result = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  اگر(!result && (rlen < curlx_dyn_len(req)))
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
  اگر(result)
    بازگشت result;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  برای(head = data->set.headers; head; head = head->next) {
    اگر(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      اگر(skip) {
        skip = FALSE;
        ادامه;
      }
      result = curlx_dyn_addf(req, "%s\r\n", head->data);
      اگر(result)
        بازگشت result;
    }
  }

  بازگشت CURLE_OK;
}

/* Header identifier in order we send them by default */
تعریفنوع شمارش {
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

ایستا CURLcode http_add_hd(ساختار Curl_easy *data,
                            ساختار dynbuf *req,
                            http_hd_t id,
                            unsigned نویسه httpversion,
                            ثابت نویسه *method,
                            Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
#اگر !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  ساختار connectdata *conn = data->conn;
#endif
  انتخاب(id) {
  حالت H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    result = curlx_dyn_addf(req, "%s ", method);
    اگر(!result)
      result = http_target(data, req);
    اگر(!result)
      result = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    بشکن;

  حالت H1_HD_HOST:
    اگر(data->state.http_host) {
      result = curlx_dyn_add(req, data->state.http_host);
      اگر(!result)
        result = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    بشکن;

#ifndef CURL_DISABLE_PROXY
  حالت H1_HD_PROXY_AUTH:
    اگر(data->req.hd_proxy_auth)
      result = curlx_dyn_add(req, data->req.hd_proxy_auth);
    بشکن;
#endif

  حالت H1_HD_AUTH:
    اگر(data->req.hd_auth)
      result = curlx_dyn_add(req, data->req.hd_auth);
    بشکن;

  حالت H1_HD_RANGE:
    اگر(data->state.use_range && data->state.rangeline)
      result = curlx_dyn_add(req, data->state.rangeline);
    بشکن;

  حالت H1_HD_USER_AGENT: {
    ثابت نویسه *ua = CURL_EASY_STR(data, STRING_USERAGENT);
    اگر(ua && *ua && !Curl_checkheaders(data, STRCONST("User-Agent")))
      result = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    بشکن;
  }

  حالت H1_HD_ACCEPT:
    اگر(!Curl_checkheaders(data, STRCONST("Accept")))
      result = curlx_dyn_add(req, "Accept: */*\r\n");
    بشکن;

  حالت H1_HD_TE:
#ifdef HAVE_LIBZ
    اگر(!Curl_checkheaders(data, STRCONST("TE")) &&
       data->set.http_transfer_encoding) {
      data->state.http_hd_te = TRUE;
      result = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#endif
    بشکن;

  حالت H1_HD_ACCEPT_ENCODING: {
    ثابت نویسه *enc = CURL_EASY_STR(data, STRING_ENCODING);
    اگر(enc && !Curl_checkheaders(data, STRCONST("Accept-Encoding")))
      result = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    بشکن;
  }

  حالت H1_HD_REFERER:
    اگر(Curl_bufref_ptr(&data->state.referer) &&
       !Curl_checkheaders(data, STRCONST("Referer")))
      result = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&data->state.referer));
    بشکن;

#ifndef CURL_DISABLE_PROXY
  حالت H1_HD_PROXY_CONNECTION:
    اگر(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(data, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(data, data->conn, STRCONST("Proxy-Connection")))
      result = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    بشکن;
#endif

  حالت H1_HD_TRANSFER_ENCODING:
    result = http_req_set_TE(data, req, httpversion);
    بشکن;

#ifndef CURL_DISABLE_ALTSVC
  حالت H1_HD_ALT_USED:
    اگر(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(data, STRCONST("Alt-Used")))
      result = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    بشکن;
#endif

  حالت H1_HD_UPGRADE:
    اگر(!Curl_conn_is_ssl(data->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
       data->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      result = Curl_http2_request_upgrade(req, data);
    }
#ifndef CURL_DISABLE_WEBSOCKETS
    اگر(!result && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      result = Curl_ws_request(data, req);
#endif
    بشکن;

  حالت H1_HD_COOKIES:
    result = http_cookies(data, req);
    بشکن;

  حالت H1_HD_CONDITIONALS:
    result = Curl_add_timecondition(data, req);
    بشکن;

  حالت H1_HD_CUSTOM:
    result = Curl_add_custom_headers(data, FALSE, httpversion, req);
    بشکن;

  حالت H1_HD_CONTENT:
    result = http_add_content_hds(data, req, httpversion, httpreq);
    بشکن;

  حالت H1_HD_CONNECTION: {
    result = http_add_connection_hd(data, req);
    بشکن;
  }

  حالت H1_HD_LAST:
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
    بشکن;
  }
  بازگشت result;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  Curl_HttpReq httpreq;
  ثابت نویسه *method;
  ساختار dynbuf req;
  unsigned نویسه httpversion;
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

  اگر(!data->conn->bits.reuse) {
    result = http_check_new_conn(data);
    اگر(result)
      برو out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  result = Curl_headers_init(data);
  اگر(result)
    برو out;

  data->state.http_hd_te = FALSE;
  data->state.http_hd_upgrade = FALSE;
  data->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(data, &method, &httpreq);

  /* select host to send */
  result = http_set_aptr_host(data);
  /* setup the authentication headers, how that method and host are known */
  اگر(!result)
    result = Curl_http_output_auth(data, data->conn, method, httpreq,
                                   data->state.up.path,
                                   data->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  اگر(!result)
    result = set_reader(data, httpreq);
  اگر(!result)
    result = http_resume(data, httpreq);
  اگر(!result)
    result = http_range(data, httpreq);
  اگر(result)
    برو out;

  httpversion = http_request_version(data);
  /* Add request line and all headers to `req` */
  برای(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    result = http_add_hd(data, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    اگر(result)
      برو out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);
  result = Curl_req_send(data, &req, httpversion);

  اگر((httpversion >= 20) && data->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    data->req.upload_chunky = FALSE;

out:
  اگر(result == CURLE_TOO_LARGE)
    failf(data, "HTTP request too large");

  curlx_dyn_free(&req);
  بازگشت result;
}

تعریفنوع شمارش {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
ایستا bool checkprefixmax(ثابت نویسه *prefix, ثابت نویسه *buffer, size_t len)
{
  size_t ch = CURLMIN(strlen(prefix), len);
  بازگشت curl_strnequal(prefix, buffer, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
ایستا statusline checkhttpprefix(ساختار Curl_easy *data,
                                  ثابت نویسه *s, size_t len)
{
  ساختار curl_slist *head = data->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  وقتی(head) {
    اگر(checkprefixmax(head->data, s, len)) {
      rc = onmatch;
      بشکن;
    }
    head = head->next;
  }

  اگر((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  بازگشت rc;
}

#ifndef CURL_DISABLE_RTSP
ایستا statusline checkrtspprefix(ساختار Curl_easy *data,
                                  ثابت نویسه *s, size_t len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (تهی)data;
  اگر(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  بازگشت status;
}
#endif /* CURL_DISABLE_RTSP */

ایستا statusline checkprotoprefix(ساختار Curl_easy *data,
                                   ساختار connectdata *conn,
                                   ثابت نویسه *s, size_t len)
{
#ifndef CURL_DISABLE_RTSP
  اگر(conn->scheme->protocol & CURLPROTO_RTSP)
    بازگشت checkrtspprefix(data, s, len);
#وگرنه
  (تهی)conn;
#endif /* CURL_DISABLE_RTSP */

  بازگشت checkhttpprefix(data, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#define HD_IS(hd, hdlen, n) \
  (((hdlen) >= (اندازه(n) - 1)) && curl_strnequal(n, hd, اندازه(n) - 1))

#define HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (اندازه(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, اندازه(n) - 1)) ? ((hd) + (اندازه(n) - 1)) : پوچ)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#define HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((اندازه(n) - 1) + (اندازه(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
ایستا CURLcode http_header_a(ساختار Curl_easy *data,
                              ثابت نویسه *hd, size_t hdlen)
{
#ifndef CURL_DISABLE_ALTSVC
  ثابت نویسه *v;
  v = (data->asi &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#وگرنه
        0
#endif
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : پوچ;
  اگر(v) {
    /* the ALPN of the current request */
    ساختار SingleRequest *k = &data->req;
    شمارش alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    بازگشت Curl_altsvc_parse(data, data->asi, v, data->state.origin, id);
  }
#وگرنه
  (تهی)data;
  (تهی)hd;
  (تهی)hdlen;
#endif
  بازگشت CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
ایستا CURLcode http_header_c(ساختار Curl_easy *data,
                              ثابت نویسه *hd, size_t hdlen)
{
  ساختار connectdata *conn = data->conn;
  ساختار SingleRequest *k = &data->req;
  ثابت نویسه *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !data->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : پوچ;
  اگر(v) {
    انجامبده {
      curl_off_t contentlength;
      صحیح offt = curlx_str_numblanks(&v, &contentlength);

      اگر(offt == STRE_OVERFLOW) {
        /* out of range */
        اگر(data->set.max_filesize) {
          failf(data, "Maximum file size exceeded");
          بازگشت CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(data, "Overflow Content-Length: value");
        بازگشت CURLE_OK;
      }
      وگرنه {
        اگر((offt == STRE_OK) &&
           ((k->size == -1) || /* not set to something before */
            (k->size == contentlength))) { /* or the same value */

          k->size = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          اگر(!curlx_str_single(&v, ','))
            ادامه;

          اگر(!curlx_str_newline(&v)) {
            k->maxdownload = k->size;
            بازگشت CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(data, "Invalid Content-Length: value");
        بازگشت CURLE_WEIRD_SERVER_REPLY;
      }
    } وقتی(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(data, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : پوچ;
  اگر(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    بازگشت Curl_build_unencoding_پشته(data, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  اگر(v) {
    نویسه *contenttype = Curl_copy_header_مقدار(hd);
    اگر(!contenttype)
      بازگشت CURLE_OUT_OF_MEMORY;
    اگر(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    وگرنه {
      curlx_free(data->info.contenttype);
      data->info.contenttype = contenttype;
    }
    بازگشت CURLE_OK;
  }
  اگر((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    بازگشت CURLE_OK;
  }
  اگر((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(data, "HTTP/1.0 connection set to keep alive");
    بازگشت CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : پوچ;
  اگر(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    ثابت نویسه *ptr = v;

    /* Move forward until first digit or asterisk */
    وقتی(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    اگر(ISDIGIT(*ptr)) {
      اگر(!curlx_str_عدد(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (data->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    وگرنه اگر(k->httpcode < 300)
      data->state.resume_from = 0; /* get everything */
  }
  بازگشت CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
ایستا CURLcode http_header_l(ساختار Curl_easy *data,
                              ثابت نویسه *hd, size_t hdlen)
{
  ساختار connectdata *conn = data->conn;
  ساختار SingleRequest *k = &data->req;
  ثابت نویسه *v = (!k->http_bodyless &&
                   (data->set.timecondition || data->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : پوچ;
  اگر(v) {
    اگر(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    اگر(data->set.get_filetime)
      data->info.filetime = k->timeofdoc;
    بازگشت CURLE_OK;
  }
  اگر(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    نویسه *location = Curl_copy_header_مقدار(hd);
    اگر(!location)
      بازگشت CURLE_OUT_OF_MEMORY;
    اگر(!*location ||
       (data->req.location && !strcmp(data->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      بازگشت CURLE_OK;
    }
    وگرنه {
      /* has value and is not an exact repeat */
      اگر(data->req.location) {
        failf(data, "Multiple Location headers");
        curlx_free(location);
        بازگشت CURLE_WEIRD_SERVER_REPLY;
      }
      data->req.location = location;

      اگر((k->httpcode >= 300 && k->httpcode < 400) &&
         data->set.http_follow_mode) {
        CURLcode result;
        DEBUGASSERT(!data->req.newurl);
        data->req.newurl = curlx_strdup(data->req.location); /* clone */
        اگر(!data->req.newurl)
          بازگشت CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        result = http_perhapsrewind(data, conn);
        اگر(result)
          بازگشت result;

        /* mark the next request as a followed location: */
        data->state.this_is_a_follow = TRUE;
      }
    }
  }
  بازگشت CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
ایستا CURLcode http_header_p(ساختار Curl_easy *data,
                              ثابت نویسه *hd, size_t hdlen)
{
  ساختار SingleRequest *k = &data->req;

#ifndef CURL_DISABLE_PROXY
  ثابت نویسه *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  اگر(v) {
    ساختار connectdata *conn = data->conn;
    اگر((k->httpversion == 10) && conn->http_proxy.peer &&
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
    وگرنه اگر((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(data, "HTTP/1.1 proxy connection set close");
    }
    بازگشت CURLE_OK;
  }
#endif
  اگر((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    نویسه *auth;
    CURLcode result;
#ifndef CURL_DISABLE_PROXY
    اگر(!data->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      بازگشت CURLE_OK;
#endif
    auth = Curl_copy_header_مقدار(hd);
    result = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    اگر(!result) {
      result = Curl_http_درون_داد_auth(data, TRUE, auth);
      curlx_free(auth);
    }
    بازگشت result;
  }
#ifdef USE_SPNEGO
  اگر(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    ساختار connectdata *conn = data->conn;
    ساختار negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    ساختار auth *authp = &data->state.authhost;
    اگر(!negdata)
      بازگشت CURLE_OUT_OF_MEMORY;
    اگر(authp->picked == CURLAUTH_NEGOTIATE) {
      نویسه *persistentauth = Curl_copy_header_مقدار(hd);
      اگر(!persistentauth)
        بازگشت CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(data, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#endif
  بازگشت CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
ایستا CURLcode http_header_r(ساختار Curl_easy *data,
                              ثابت نویسه *hd, size_t hdlen)
{
  ثابت نویسه *v = HD_VAL(hd, hdlen, "Retry-After:");
  اگر(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    اگر(!Curl_getdate_capped(v, &date)) {
      time_t current = time(پوچ);
      اگر(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    وگرنه
      /* Try it as a decimal number, ignore errors */
      (تهی)curlx_str_عدد(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    اگر(retry_after > 21600)
      retry_after = 21600;
    data->info.retry_after = retry_after;
  }
  بازگشت CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
ایستا CURLcode http_header_s(ساختار Curl_easy *data,
                              ثابت نویسه *hd, size_t hdlen)
{
#اگر !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ثابت نویسه *v;
#وگرنه
  (تهی)data;
  (تهی)hd;
  (تهی)hdlen;
#endif

#ifndef CURL_DISABLE_COOKIES
  v = (data->cookies && data->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : پوچ;
  اگر(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ثابت نویسه *host = data->req.cookiehost ?
      data->req.cookiehost : data->state.origin->hostname;
    ثابت unsigned نویسه secure_context = Curl_secure_context(data, host) ?
      COOKIE_SECURE : 0;
    CURLcode result;
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    result = Curl_cookie_add(data, data->cookies, v, host,
                             data->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    بازگشت result;
  }
#endif
#ifndef CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (data->hsts &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#وگرنه
        0
#endif
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : پوچ;
  اگر(v) {
    CURLcode result = Curl_hsts_parse(
      data->hsts, data->state.origin->hostname, v);
    اگر(result) {
      اگر(result == CURLE_OUT_OF_MEMORY)
        بازگشت result;
      infof(data, "Illegal STS header skipped");
    }
#ifdef DEBUGBUILD
    وگرنه
      infof(data, "Parsed STS header fine (%zu entries)",
            Curl_llist_شمار(&data->hsts->list));
#endif
  }
#endif

  بازگشت CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
ایستا CURLcode http_header_t(ساختار Curl_easy *data,
                              ثابت نویسه *hd, size_t hdlen)
{
  ساختار connectdata *conn = data->conn;
  ساختار SingleRequest *k = &data->req;

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
  ثابت نویسه *v = (!k->http_bodyless &&
                   (data->state.httpreq != HTTPREQ_HEAD) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : پوچ;
  اگر(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode result = Curl_build_unencoding_پشته(data, v, TRUE);
    اگر(result)
      بازگشت result;
    اگر(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(data, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    بازگشت CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  اگر(v) {
    data->req.resp_trailer = TRUE;
    بازگشت CURLE_OK;
  }
  بازگشت CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
ایستا CURLcode http_header_w(ساختار Curl_easy *data,
                              ثابت نویسه *hd, size_t hdlen)
{
  ساختار SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  اگر((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    نویسه *auth = Curl_copy_header_مقدار(hd);
    اگر(!auth)
      result = CURLE_OUT_OF_MEMORY;
    وگرنه {
      result = Curl_http_درون_داد_auth(data, FALSE, auth);
      curlx_free(auth);
    }
  }
  بازگشت result;
}

/*
 * http_header() parses a single response header.
 */
ایستا CURLcode http_header(ساختار Curl_easy *data,
                            ثابت نویسه *hd, size_t hdlen)
{
  CURLcode result = CURLE_OK;

  انتخاب(hd[0]) {
  حالت 'a':
  حالت 'A':
    result = http_header_a(data, hd, hdlen);
    بشکن;
  حالت 'c':
  حالت 'C':
    result = http_header_c(data, hd, hdlen);
    بشکن;
  حالت 'l':
  حالت 'L':
    result = http_header_l(data, hd, hdlen);
    بشکن;
  حالت 'p':
  حالت 'P':
    result = http_header_p(data, hd, hdlen);
    بشکن;
  حالت 'r':
  حالت 'R':
    result = http_header_r(data, hd, hdlen);
    بشکن;
  حالت 's':
  حالت 'S':
    result = http_header_s(data, hd, hdlen);
    بشکن;
  حالت 't':
  حالت 'T':
    result = http_header_t(data, hd, hdlen);
    بشکن;
  حالت 'w':
  حالت 'W':
    result = http_header_w(data, hd, hdlen);
    بشکن;
  }

  اگر(!result) {
    ساختار connectdata *conn = data->conn;
    اگر(conn->scheme->protocol & CURLPROTO_RTSP)
      result = Curl_rtsp_parseheader(data, hd);
  }
  بازگشت result;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
ایستا CURLcode http_statusline(ساختار Curl_easy *data,
                                ساختار connectdata *conn)
{
  ساختار SingleRequest *k = &data->req;

  انتخاب(k->httpversion) {
  حالت 10:
  حالت 11:
#ifdef USE_HTTP2
  حالت 20:
#endif
#ifdef USE_HTTP3
  حالت 30:
#endif
    /* no major version switch mid-connection */
    اگر(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(data, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      بازگشت CURLE_WEIRD_SERVER_REPLY;
    }
    بشکن;
  پیشفرض:
    failf(data, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    بازگشت CURLE_UNSUPPORTED_PROTOCOL;
  }

  data->info.httpcode = k->httpcode;
  data->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  اگر(!data->state.http_neg.rcvd_min ||
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
  اگر(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  اگر(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(data, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  انتخاب(k->httpcode) {
  حالت 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    اگر(data->set.timecondition)
      data->info.timecond = TRUE;
    FALLTHROUGH();
  حالت 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->size = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
ایستا CURLcode http_size(ساختار Curl_easy *data)
{
  ساختار SingleRequest *k = &data->req;
  اگر(data->req.ignore_cl || k->chunk) {
    k->size = k->maxdownload = -1;
  }
  وگرنه اگر(k->size != -1) {
    اگر(data->set.max_filesize &&
       !k->ignorebody &&
       (k->size > data->set.max_filesize)) {
      failf(data, "Maximum file size exceeded");
      بازگشت CURLE_FILESIZE_EXCEEDED;
    }
    اگر(k->ignorebody)
      infof(data, "setting size while ignoring");
    Curl_pgrsSetDownloadSize(data, k->size);
    k->maxdownload = k->size;
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_verify_header(ساختار Curl_easy *data,
                            ثابت نویسه *hd, size_t hdlen)
{
  ساختار SingleRequest *k = &data->req;
  size_t i;
  bool has_colon = FALSE;
  برای(i = 0; i < hdlen; i++) {
    انتخاب(hd[i]) {
    حالت '\0':
      /* this is bad, bail out */
      failf(data, "NUL byte in header");
      بازگشت CURLE_WEIRD_SERVER_REPLY;
    حالت '\r':
      اگر(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(data, "Carriage return found in header");
        بازگشت CURLE_WEIRD_SERVER_REPLY;
      }
      بشکن;
    حالت ':':
      has_colon = TRUE;
      بشکن;
    }
  }
  اگر(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    بازگشت CURLE_OK;
  اگر(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  وگرنه اگر(!has_colon) {
    /* this is bad, bail out */
    failf(data, "Header without colon");
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_bump_headersize(ساختار Curl_easy *data,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  unsigned صحیح max = MAX_HTTP_RESP_HEADER_SIZE;
  اگر(delta < MAX_HTTP_RESP_HEADER_SIZE) {
    data->info.header_size += (unsigned صحیح)delta;
    data->req.allheadercount += (unsigned صحیح)delta;
    اگر(!connect_only)
      data->req.headerbytecount += (unsigned صحیح)delta;
    اگر(data->req.allheadercount > max)
      bad = data->req.allheadercount;
    وگرنه اگر(data->info.header_size > (max * 20)) {
      bad = data->info.header_size;
      max *= 20;
    }
  }
  وگرنه
    bad = data->req.allheadercount + delta;
  اگر(bad) {
    failf(data, "Too large response headers: %zu > %u", bad, max);
    بازگشت CURLE_RECV_ERROR;
  }
  بازگشت CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
ایستا CURLcode http_on_101_upgrade(ساختار Curl_easy *data,
                                    ثابت نویسه *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_changed)
{
  ساختار connectdata *conn = data->conn;
  ساختار SingleRequest *k = &data->req;

#اگر !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (تهی)buf;
  (تهی)blen;
  (تهی)pconsumed;
#وگرنه
  CURLcode result;
  صحیح upgr101_requested = k->upgr101;
#endif

  اگر(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(data, "server sent 101 response while not talking HTTP/1.1");
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->bits.upgrade_in_progress = FALSE;
  *conn_changed = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ifdef USE_NGHTTP2
  اگر(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(data, "Received 101, Switching to HTTP/2");
    result = Curl_http2_upgrade(data, conn, FIRSTSOCKET, buf, blen);
    اگر(!result)
      *pconsumed += blen;
    بازگشت result;
  }
#endif
#ifndef CURL_DISABLE_WEBSOCKETS
  اگر(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(data, "Received 101, Switching to WebSocket");
    result = Curl_ws_accept(data, buf, blen);
    اگر(!result)
      *pconsumed += blen; /* ws accept handled the data */
    بازگشت result;
  }
#endif
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  بازگشت CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
ایستا CURLcode http_on_1xx_response(ساختار Curl_easy *data,
                                     ثابت نویسه *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_changed)
{
  ساختار SingleRequest *k = &data->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  انتخاب(k->httpcode) {
  حالت 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(data);
    بشکن;
  حالت 101:
    بازگشت http_on_101_upgrade(data, buf, blen, pconsumed, conn_changed);
  پیشفرض:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    بشکن;
  }
  بازگشت CURLE_OK;
}

#اگر defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
ایستا تهی http_check_auth_closure(ساختار Curl_easy *data,
                                    ساختار connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ifdef USE_NTLM
  اگر(conn->bits.close &&
     (((data->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((data->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
#endif
#ifdef USE_SPNEGO
  اگر(conn->bits.close &&
    (((data->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((data->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
  اگر((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  اگر((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#endif
}
#وگرنه
#define http_check_auth_closure(x, y) /* empty */
#endif

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
ایستا CURLcode http_handle_send_error(ساختار Curl_easy *data)
{
  ساختار connectdata *conn = data->conn;
  ساختار SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  اگر(!data->req.authneg && !conn->bits.close &&
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

    انتخاب(data->state.httpreq) {
    حالت HTTPREQ_PUT:
    حالت HTTPREQ_POST:
    حالت HTTPREQ_POST_FORM:
    حالت HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      اگر(!Curl_req_done_sending(data)) {
        اگر((k->httpcode == 417) && http_exp100_is_selected(data)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          اگر(!k->writebytecount && http_exp100_is_waiting(data)) {
            infof(data, "Got HTTP failure 417 while waiting for a 100");
          }
          وگرنه {
            infof(data, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            result = http_perhapsrewind(data, conn);
            اگر(result)
              بازگشت result;
          }
          data->state.disableexpect = TRUE;
          Curl_req_abort_sending(data);
          DEBUGASSERT(!data->req.newurl);
          data->req.newurl = Curl_bufref_dup(&data->state.url);
          اگر(!data->req.newurl)
            بازگشت CURLE_OUT_OF_MEMORY;
        }
        وگرنه اگر(data->set.http_keep_sending_on_error) {
          infof(data, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(data);
        }
        وگرنه {
          infof(data, "HTTP error before end of send, stop sending");
          streamclose(conn);
          result = Curl_req_abort_sending(data);
          اگر(result)
            بازگشت result;
        }
      }
      بشکن;

    پیشفرض: /* default label present to avoid compiler warnings */
      بشکن;
    }
  }

  اگر(Curl_creader_will_rewind(data) && !Curl_req_done_sending(data)) {
    /* We rewind before next send, continue sending now */
    infof(data, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(data);
  }
  بازگشت result;
}

ایستا CURLcode http_on_response(ساختار Curl_easy *data,
                                 ثابت نویسه *last_hd, size_t last_hd_len,
                                 ثابت نویسه *buf, size_t blen,
                                 size_t *pconsumed)
{
  ساختار connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  ساختار SingleRequest *k = &data->req;
  bool conn_changed = FALSE;

  (تهی)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  اگر(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    اگر(data->req.httpversion != 20)
      infof(data, "Lying server, not serving HTTP/2");
  }

  اگر(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    result = http_write_header(data, last_hd, last_hd_len);
    last_hd = پوچ; /* handled it */
    اگر(result)
      برو out;
  }

  اگر(k->httpcode < 100) {
    failf(data, "Unsupported response code in HTTP response");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    برو out;
  }
  وگرنه اگر(k->httpcode < 200) {
    result = http_on_1xx_response(data, buf, blen, pconsumed, &conn_changed);
    برو out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  اگر(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  اگر((k->size == -1) && !k->chunk && !conn->bits.close &&
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
  اگر(data->req.upgr101 == UPGR101_WS) {
    failf(data, "Refused WebSocket upgrade: %d", k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    برو out;
  }
#endif

  /* Check if this response means the transfer errored. */
  اگر(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    برو out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  result = Curl_http_auth_act(data);
  اگر(result)
    برو out;

  اگر(k->httpcode >= 300) {
    result = http_handle_send_error(data);
    اگر(result)
      برو out;
  }

  /* final response without error, prepare to receive the body */
  result = http_firstwrite(data);
  اگر(result)
    برو out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  result = http_size(data);
  اگر(result)
    برو out;

#ifndef CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  اگر((conn->scheme->protocol & CURLPROTO_RTSP) && data->req.no_body &&
     !data->set.opt_no_body && (k->size > 0))
    data->req.no_body = FALSE;
#endif

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  اگر(data->req.no_body)
    k->download_done = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  اگر((k->maxdownload == 0) && (k->httpversion_sent < 20))
    k->download_done = TRUE;

out:
  اگر(last_hd)
    /* if not written yet, write it now */
    result = Curl_1st_fatal(result,
                            http_write_header(data, last_hd, last_hd_len));
  اگر(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(data->multi);
  بازگشت result;
}

ایستا CURLcode http_rw_hd(ساختار Curl_easy *data,
                           ثابت نویسه *hd, size_t hdlen,
                           ثابت نویسه *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  ساختار SingleRequest *k = &data->req;
  صحیح writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  اگر((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    ساختار dynbuf last_header;
    size_t consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    result = curlx_dyn_addn(&last_header, hd, hdlen);
    اگر(result)
      بازگشت result;

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
    بازگشت result;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_HEADER;
  اگر(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    اگر(data->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ثابت نویسه *p = hd;

      curlx_str_passblanks(&p);
      اگر(!strncmp(p, "HTTP/", 5)) {
        p += 5;
        انتخاب(*p) {
        حالت '1':
          p++;
          اگر((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            اگر(ISBLANK(p[2])) {
              k->httpversion = (unsigned نویسه)(10 + (p[1] - '0'));
              p += 3;
              اگر(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          اگر(!fine_statusline) {
            failf(data, "Unsupported HTTP/1 subversion in response");
            بازگشت CURLE_UNSUPPORTED_PROTOCOL;
          }
          بشکن;
        حالت '2':
        حالت '3':
          اگر(!ISBLANK(p[1]))
            بشکن;
          k->httpversion = (unsigned نویسه)((*p - '0') * 10);
          p += 2;
          اگر(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            اگر(!ISBLANK(*p))
              بشکن;
            fine_statusline = TRUE;
          }
          بشکن;
        پیشفرض: /* unsupported */
          failf(data, "Unsupported HTTP version in response");
          بازگشت CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      اگر(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(data, hd, hdlen);
        اگر(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    وگرنه اگر(data->conn->scheme->protocol & CURLPROTO_RTSP) {
      ثابت نویسه *p = hd;
      ساختار Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      اگر(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_عدد(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        اگر(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (صحیح)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      اگر(!fine_statusline)
        بازگشت CURLE_WEIRD_SERVER_REPLY;
    }

    اگر(fine_statusline) {
      result = http_statusline(data, data->conn);
      اگر(result)
        بازگشت result;
      writetype |= CLIENTWRITE_STATUS;
    }
    وگرنه {
      k->header = FALSE;   /* this is not a header line */
      بازگشت CURLE_WEIRD_SERVER_REPLY;
    }
  }

  result = Curl_verify_header(data, hd, hdlen);
  اگر(result)
    بازگشت result;

  result = http_header(data, hd, hdlen);
  اگر(result)
    بازگشت result;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  اگر(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  result = Curl_client_write(data, writetype, hd, hdlen);
  اگر(result)
    بازگشت result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  اگر(result)
    بازگشت result;

  بازگشت CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
تهی Curl_http_to_fold(ساختار dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  ثابت نویسه *hd = curlx_dyn_ptr(bf);
  اگر(len && (hd[len - 1] == '\n'))
    len--;
  اگر(len && (hd[len - 1] == '\r'))
    len--;
  وقتی(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

ایستا تهی unfold_header(ساختار Curl_easy *data)
{
  Curl_http_to_fold(&data->state.headerb);
  data->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
ایستا CURLcode http_parse_headers(ساختار Curl_easy *data,
                                   ثابت نویسه *buf, size_t blen,
                                   size_t *pconsumed)
{
  ساختار connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  ساختار SingleRequest *k = &data->req;
  ثابت نویسه *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  اگر(data->state.maybe_folded && blen) {
    اگر(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(data);
    }
    وگرنه {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      result = http_rw_hd(data, curlx_dyn_ptr(&data->state.headerb),
                          curlx_dyn_len(&data->state.headerb),
                          پوچ, 0, &ignore_this);
      curlx_dyn_reset(&data->state.headerb);
      اگر(result)
        بازگشت result;
    }
    data->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  وقتی(blen && k->header) {
    size_t consumed;
    size_t hlen;
    ثابت نویسه *hd;
    size_t unfold_len = 0;

    اگر(data->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      وقتی(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      اگر(blen) {
        /* insert a single space */
        result = curlx_dyn_addn(&data->state.headerb, " ", 1);
        اگر(result)
          بازگشت result;
        data->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = memchr(buf, '\n', blen);
    اگر(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      result = curlx_dyn_addn(&data->state.headerb, buf, blen);
      اگر(result)
        بازگشت result;
      *pconsumed += blen + unfold_len;

      اگر(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(data, conn,
                           curlx_dyn_ptr(&data->state.headerb),
                           curlx_dyn_len(&data->state.headerb));

        اگر(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          اگر((k->httpversion >= 10) || conn->bits.reuse) {
            failf(data, "Invalid status line");
            بازگشت CURLE_WEIRD_SERVER_REPLY;
          }
          اگر(!data->state.http_neg.accept_09) {
            failf(data, "Received HTTP/0.9 when not allowed");
            بازگشت CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          برو out;
        }
      }
      برو out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    result = curlx_dyn_addn(&data->state.headerb, buf, consumed);
    اگر(result)
      بازگشت result;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&data->state.headerb);
    hd = curlx_dyn_ptr(&data->state.headerb);

    اگر(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(data, conn, hd, hlen);
      اگر(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        اگر((k->httpversion >= 10) || conn->bits.reuse) {
          failf(data, "Invalid status line");
          بازگشت CURLE_WEIRD_SERVER_REPLY;
        }
        اگر(!data->state.http_neg.accept_09) {
          failf(data, "Received HTTP/0.9 when not allowed");
          بازگشت CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        برو out;
      }
    }
    وگرنه {
      اگر(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        اگر(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(data);
          ادامه;
        }
        وگرنه اگر(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          data->state.maybe_folded = TRUE;
          بشکن;
        }
      }
    }

    result = http_rw_hd(data, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&data->state.headerb);
    اگر(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    اگر(result)
      بازگشت result;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  اگر(!k->header && !leftover_body) {
    curlx_dyn_free(&data->state.headerb);
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(ساختار Curl_easy *data,
                                 ثابت نویسه *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode result;
  size_t consumed;
  نویسه tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  result = http_rw_hd(data, hd, hdlen, &tmp, 0, &consumed);
  اگر(!result && is_eos) {
    result = Curl_client_write(data, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  بازگشت result;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(ساختار Curl_easy *data,
                                  ثابت نویسه *buf, size_t blen,
                                  size_t *pconsumed)
{
  اگر(!data->req.header) {
    *pconsumed = 0;
    بازگشت CURLE_OK;
  }
  وگرنه {
    CURLcode result;

    result = http_parse_headers(data, buf, blen, pconsumed);
    اگر(!result && !data->req.header) {
      اگر(!data->req.no_body && curlx_dyn_len(&data->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&data->state.headerb),
                                   curlx_dyn_len(&data->state.headerb));
      }
      curlx_dyn_free(&data->state.headerb);
    }
    بازگشت result;
  }
}

CURLcode Curl_http_write_resp(ساختار Curl_easy *data,
                              ثابت نویسه *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result;
  size_t consumed;
  صحیح flags;

  result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
  اگر(result || data->req.done)
    برو out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !data->req.header);
  اگر(!data->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    اگر(is_eos)
      flags |= CLIENTWRITE_EOS;
    result = Curl_client_write(data, flags, buf, blen);
  }
out:
  بازگشت result;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, ثابت نویسه *s, size_t len)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  صحیح i;

  اگر(len != 3)
    برو out;

  برای(i = 0; i < 3; ++i) {
    نویسه c = s[i];

    اگر(c < '0' || c > '9')
      برو out;

    status *= 10;
    status += c - '0';
  }
  result = CURLE_OK;
out:
  *pstatus = result ? -1 : status;
  بازگشت result;
}

CURLcode Curl_http_req_make(ساختار httpreq **preq,
                            ثابت نویسه *method, size_t m_len,
                            ثابت نویسه *scheme, size_t s_len,
                            ثابت نویسه *authority, size_t a_len,
                            ثابت نویسه *path, size_t p_len)
{
  ساختار httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, اندازه(*req) + m_len);
  اگر(!req)
    برو out;
#اگر defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic افزودن
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memcpy(req->method, method, m_len);
#اگر defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic برداشت
#endif
  اگر(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    اگر(!req->scheme)
      برو out;
  }
  اگر(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    اگر(!req->authority)
      برو out;
  }
  اگر(path) {
    req->path = curlx_memdup0(path, p_len);
    اگر(!req->path)
      برو out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  اگر(result && req)
    Curl_http_req_free(req);
  *preq = result ? پوچ : req;
  بازگشت result;
}

ایستا CURLcode req_assign_url_authority(ساختار httpreq *req, CURLU *url)
{
  نویسه *host, *port;
  ساختار dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  host = port = پوچ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  اگر(uc && uc != CURLUE_NO_HOST)
    برو out;
  اگر(!host) {
    req->authority = پوچ;
    result = CURLE_OK;
    برو out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  اگر(uc && uc != CURLUE_NO_PORT)
    برو out;

  result = curlx_dyn_add(&buf, host);
  اگر(result)
    برو out;
  اگر(port) {
    result = curlx_dyn_addf(&buf, ":%s", port);
    اگر(result)
      برو out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  اگر(result)
    curlx_dyn_free(&buf);
  بازگشت result;
}

ایستا CURLcode req_assign_url_path(ساختار httpreq *req, CURLU *url)
{
  نویسه *path, *query;
  ساختار dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  path = query = پوچ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  اگر(uc)
    برو out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  اگر(uc && uc != CURLUE_NO_QUERY)
    برو out;

  اگر(!query) {
    req->path = path;
    path = پوچ;
  }
  وگرنه {
    result = curlx_dyn_add(&buf, path);
    اگر(!result)
      result = curlx_dyn_addf(&buf, "?%s", query);
    اگر(result)
      برو out;
    req->path = curlx_dyn_ptr(&buf);
  }
  result = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  اگر(result)
    curlx_dyn_free(&buf);
  بازگشت result;
}

CURLcode Curl_http_req_make2(ساختار httpreq **preq,
                             ثابت نویسه *method, size_t m_len,
                             CURLU *url, ثابت نویسه *scheme_default)
{
  ساختار httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, اندازه(*req) + m_len);
  اگر(!req)
    برو out;
  memcpy(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  اگر(uc && uc != CURLUE_NO_SCHEME)
    برو out;
  اگر(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    اگر(!req->scheme)
      برو out;
  }

  result = req_assign_url_authority(req, url);
  اگر(result)
    برو out;
  result = req_assign_url_path(req, url);
  اگر(result)
    برو out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  اگر(result && req)
    Curl_http_req_free(req);
  *preq = result ? پوچ : req;
  بازگشت result;
}

تهی Curl_http_req_free(ساختار httpreq *req)
{
  اگر(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->path);
    Curl_dynhds_free(&req->headers);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

ساختار name_const {
  ثابت نویسه *name;
  size_t namelen;
};

ایستا ثابت ساختار name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

ایستا bool h2_permissible_field(ساختار dynhds_entry *e)
{
  size_t i;
  برای(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    اگر(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      بازگشت FALSE;
  }
  بازگشت TRUE;
}

ایستا bool http_TE_has_token(ثابت نویسه *fvalue, ثابت نویسه *token)
{
  وقتی(*fvalue) {
    ساختار Curl_str name;

    /* skip to first token */
    وقتی(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    اگر(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      بازگشت FALSE;
    اگر(curlx_str_casecompare(&name, token))
      بازگشت TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    وقتی(*fvalue && *fvalue != ',') {
      اگر(*fvalue == '"') {
        ساختار Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        اگر(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          بازگشت FALSE;
      }
      وگرنه
        fvalue++;
    }
  }
  بازگشت FALSE;
}

CURLcode Curl_http_req_to_h2(ساختار dynhds *h2_headers,
                             ساختار httpreq *req, ساختار Curl_easy *data)
{
  ثابت نویسه *scheme = پوچ, *authority = پوچ;
  ساختار dynhds_entry *e;
  size_t i;
  CURLcode result;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  اگر(req->scheme) {
    scheme = req->scheme;
  }
  وگرنه اگر(strcmp("CONNECT", req->method)) {
    scheme = Curl_checkheaders(data, STRCONST(HTTP_PSEUDO_SCHEME));
    اگر(scheme) {
      scheme += اندازه(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(data, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    وگرنه {
      scheme = data->state.origin->scheme->name;
    }
  }

  اگر(req->authority) {
    authority = req->authority;
  }
  وگرنه {
    e = Curl_dynhds_get(&req->headers, STRCONST("Host"));
    اگر(e)
      authority = e->مقدار;
  }

  Curl_dynhds_reset(h2_headers);
  Curl_dynhds_set_opts(h2_headers, DYNHDS_OPT_LOWERCASE);
  result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, strlen(req->method));
  اگر(!result && scheme) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, strlen(scheme));
  }
  اگر(!result && authority) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, strlen(authority));
  }
  اگر(!result && req->path) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, strlen(req->path));
  }
  برای(i = 0; !result && i < Curl_dynhds_شمار(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    اگر(e->namelen == 2 && curl_strequal("TE", e->name)) {
      اگر(http_TE_has_token(e->مقدار, "trailers"))
        result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    وگرنه اگر(h2_permissible_field(e)) {
      result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->مقدار, e->valuelen);
    }
  }

  بازگشت result;
}

CURLcode Curl_http_resp_make(ساختار http_resp **presp,
                             صحیح status,
                             ثابت نویسه *description)
{
  ساختار http_resp *resp;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, اندازه(*resp));
  اگر(!resp)
    برو out;

  resp->status = status;
  اگر(description) {
    resp->description = curlx_strdup(description);
    اگر(!resp->description)
      برو out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  اگر(result && resp)
    Curl_http_resp_free(resp);
  *presp = result ? پوچ : resp;
  بازگشت result;
}

تهی Curl_http_resp_free(ساختار http_resp *resp)
{
  اگر(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    اگر(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ثابت ساختار Curl_protocol Curl_protocol_http = {
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
