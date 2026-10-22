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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"

#ifndef CURL_DISABLE_HTTP

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif

#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#endif

#अन्तर्भाव "transfer.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "formdata.h"
#अन्तर्भाव "mime.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "cookie.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "http_digest.h"
#अन्तर्भाव "http_ntlm.h"
#अन्तर्भाव "http_negotiate.h"
#अन्तर्भाव "http_aws_sigv4.h"
#अन्तर्भाव "http_httpsig.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "urlapi-int.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "dynhds.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "headers.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "parsedate.h" /* for the week day and month names */
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "content_encoding.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "http2.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "altsvc.h"
#अन्तर्भाव "hsts.h"
#अन्तर्भाव "rtsp.h"
#अन्तर्भाव "ws.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curlx/strparse.h"

शून्य Curl_http_neg_init(संरचना Curl_easy *data, संरचना http_negotiation *neg)
{
  स_रखो(neg, 0, आकार(*neg));
  neg->accept_09 = data->set.http09_allowed;
  निवड(data->set.httpwant) {
  स्थिती CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    थांबा;
  स्थिती CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    थांबा;
  स्थिती CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    थांबा;
  स्थिती CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    थांबा;
  स्थिती CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    data->state.http_neg.h2_prior_knowledge = TRUE;
    थांबा;
  स्थिती CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    थांबा;
  स्थिती CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    थांबा;
  स्थिती CURL_HTTP_VERSION_NONE:
  पूर्वनिर्धारित:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    थांबा;
  }
}

CURLcode Curl_http_setup_conn(संरचना Curl_easy *data,
                              संरचना connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  जर(data->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode result = Curl_conn_may_http3(data, conn);
    जर(result)
      परत result;
  }
  परत CURLE_OK;
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
अक्षर *Curl_checkProxyheaders(संरचना Curl_easy *data,
                             स्थिरांक संरचना connectdata *conn,
                             स्थिरांक अक्षर *thisheader,
                             स्थिरांक माप_प्रकार thislen)
{
  संरचना curl_slist *head;

  साठी(head = (conn->http_proxy.peer && data->set.sep_headers) ?
        data->set.proxyheaders : data->set.headers;
      head; head = head->next) {
    जर(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      परत head->data;
  }

  परत रिक्त;
}
#endif

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
स्थिर bool header_has_मूल्य(स्थिरांक अक्षर **headerp, संरचना Curl_str *outp)
{
  bool मूल्य = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  जर(मूल्य) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  परत मूल्य;
}

स्थिर bool http_header_is_empty(स्थिरांक अक्षर *header)
{
  संरचना Curl_str out;

  जर(header_has_मूल्य(&header, &out)) {
    परत curlx_strlen(&out) == 0;
  }
  परत TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
स्थिर CURLcode copy_custom_मूल्य(स्थिरांक अक्षर *header, अक्षर **valp)
{
  संरचना Curl_str out = { 0 };

  /* find the end of the header name */
  जर(header_has_मूल्य(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    जर(*valp)
      परत CURLE_OK;
    परत CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = रिक्त;
  परत CURLE_BAD_FUNCTION_ARGUMENT;
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
अक्षर *Curl_copy_header_मूल्य(स्थिरांक अक्षर *header)
{
  संरचना Curl_str out;

  /* find the end of the header name */
  जर(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_SIZE, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_SIZE);
    curlx_str_trimblanks(&out);
    परत curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  परत रिक्त;
}

#ifndef CURL_DISABLE_HTTP_AUTH

#ifndef CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
स्थिर CURLcode http_output_basic(संरचना Curl_easy *data,
                                  संरचना connectdata *conn, bool proxy)
{
  माप_प्रकार size = 0;
  अक्षर *authorization = रिक्त;
  अक्षर **p_hd;
  CURLcode result;
  संरचना Curl_creds *creds = रिक्त;
  अक्षर *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  जर(proxy) {
#ifndef CURL_DISABLE_PROXY
    p_hd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#अन्यथा
    (शून्य)conn;
    परत CURLE_NOT_BUILT_IN;
#endif
  }
  अन्यथा {
    p_hd = &data->req.hd_auth;
    creds = data->state.creds;
  }

  जर(!creds) {
    DEBUGASSERT(0);
    परत CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  जर(!out)
    परत CURLE_OUT_OF_MEMORY;

  result = curlx_base64_encode((uint8_t *)out, म_माप(out),
                               &authorization, &size);
  जर(result)
    जा fail;

  जर(!authorization) {
    result = CURLE_REMOTE_ACCESS_DENIED;
    जा fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  जर(!*p_hd) {
    result = CURLE_OUT_OF_MEMORY;
    जा fail;
  }

fail:
  curlx_free(out);
  परत result;
}

#endif

#ifndef CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
स्थिर CURLcode http_output_bearer(संरचना Curl_easy *data)
{
  अक्षर **userp;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(data->state.creds));
  userp = &data->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(data->state.creds));

  जर(!*userp) {
    result = CURLE_OUT_OF_MEMORY;
    जा fail;
  }

fail:
  परत result;
}
#endif

#endif

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
स्थिर bool pickoneauth(संरचना auth *pick, अचिन्हित दीर्घ mask,
                        संरचना Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  अचिन्हित दीर्घ avail = pick->avail & pick->want & mask;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  जर(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ifndef CURL_DISABLE_BEARER_AUTH
  अन्यथा जर((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  अन्यथा जर((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#endif
  अन्यथा जर(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ifndef CURL_DISABLE_BASIC_AUTH
  अन्यथा जर((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#endif
#ifndef CURL_DISABLE_AWS
  अन्यथा जर(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  अन्यथा जर(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#endif
  अन्यथा {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  परत picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
स्थिर CURLcode http_perhapsrewind(संरचना Curl_easy *data,
                                   संरचना connectdata *conn)
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
  VERBOSE(स्थिरांक अक्षर *ongoing_auth = रिक्त);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  जर(needs_rewind) {
    infof(data, "Need to rewind upload for next request");
    Curl_creader_set_rewind(data, TRUE);
  }

  जर(conn->bits.बंद)
    /* If we already decided to close this connection, we cannot veto. */
    परत CURLE_OK;

  जर(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ifdef USE_NTLM
    जर((data->state.authproxy.picked == CURLAUTH_NTLM) ||
       (data->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      जर((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
#ifdef USE_SPNEGO
    /* There is still data left to send */
    जर((data->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (data->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      जर((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
  }

  जर(abort_upload) {
    जर(upload_remain >= 0)
      infof(data, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    अन्यथा
      infof(data, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    data->req.size = 0; /* do not download any more than 0 bytes */
    data->req.http_bodyless = TRUE;
  }
  परत CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
स्थिर bool http_should_fail(संरचना Curl_easy *data, पूर्णांक httpcode)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  जर(!data->set.http_fail_on_error)
    परत FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  जर(httpcode < 400)
    परत FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  जर(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    परत FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  जर((httpcode != 401) && (httpcode != 407))
    परत TRUE;

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
  जर((httpcode == 401) && !data->state.creds)
    परत TRUE;
#ifndef CURL_DISABLE_PROXY
  जर((httpcode == 407) && !data->conn->http_proxy.creds)
    परत TRUE;
#endif

  परत (bool)data->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(संरचना Curl_easy *data)
{
  संरचना connectdata *conn = data->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode result = CURLE_OK;
  अचिन्हित दीर्घ authmask = ~0UL;

  जर(!Curl_creds_has_oauth_bearer(data->state.creds))
    authmask &= (अचिन्हित दीर्घ)~CURLAUTH_BEARER;

  जर(100 <= data->req.httpcode && data->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    परत CURLE_OK;

  जर(data->state.authproblem)
    परत data->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  जर(data->state.creds &&
     ((data->req.httpcode == 401) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickhost = pickoneauth(&data->state.authhost, authmask, data->state.creds);
    जर(!pickhost)
      data->state.authproblem = TRUE;
    अन्यथा
      data->info.httpauthpicked = data->state.authhost.picked;
    जर(data->state.authhost.picked == CURLAUTH_NTLM &&
       (data->req.httpversion_sent > 11)) {
      infof(data, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ifndef CURL_DISABLE_PROXY
  जर(conn->http_proxy.creds &&
     ((data->req.httpcode == 407) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickproxy = pickoneauth(&data->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    जर(!pickproxy)
      data->state.authproblem = TRUE;
    अन्यथा
      data->info.proxyauthpicked = data->state.authproxy.picked;
  }
#endif

  जर(pickhost || pickproxy) {
    result = http_perhapsrewind(data, conn);
    जर(result)
      परत result;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(data->req.newurl);
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    जर(!data->req.newurl)
      परत CURLE_OUT_OF_MEMORY;
  }
  अन्यथा जर((data->req.httpcode < 300) &&
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
    जर(!data->req.newurl)
      परत CURLE_OUT_OF_MEMORY;
    data->state.authhost.done = TRUE;
  }
  जर(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          data->req.httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
  }

  परत result;
}

#ifndef CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
स्थिर CURLcode output_auth_headers(संरचना Curl_easy *data,
                                    संरचना connectdata *conn,
                                    संरचना auth *authstatus,
                                    स्थिरांक अक्षर *request,
                                    स्थिरांक अक्षर *path,
                                    bool proxy)
{
  स्थिरांक अक्षर *auth = रिक्त;
  CURLcode result = CURLE_OK;
  (शून्य)conn;

#ifdef CURL_DISABLE_DIGEST_AUTH
  (शून्य)request;
  (शून्य)path;
#endif
#ifndef CURL_DISABLE_AWS
  जर((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    result = Curl_output_aws_sigv4(data);
    जर(result)
      परत result;
  }
  अन्यथा
#endif
#ifndef CURL_DISABLE_HTTPSIG
  जर((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    जर(Curl_auth_allowed_to_host(data)) {
      auth = "HTTPSIG";
      result = Curl_output_httpsig(data);
      जर(result)
        परत result;
    }
    अन्यथा
      authstatus->done = TRUE;
  }
  अन्यथा
#endif
#ifdef USE_SPNEGO
  जर(authstatus->picked == CURLAUTH_NEGOTIATE) {
    जर(
#ifndef CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(data, conn,
                                        STRCONST("Proxy-authorization"))) ||
#endif
      (!proxy && !Curl_checkheaders(data, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(data))) {
      auth = "Negotiate";
      result = Curl_output_negotiate(data, conn, proxy);
      जर(result)
        परत result;
    }
    अन्यथा
      authstatus->done = TRUE;
  }
  अन्यथा
#endif
#ifdef USE_NTLM
  जर(authstatus->picked == CURLAUTH_NTLM) {
    जर(proxy || Curl_auth_allowed_to_host(data)) {
      auth = "NTLM";
      result = Curl_output_ntlm(data, proxy);
      जर(result)
        परत result;
    }
    अन्यथा
      authstatus->done = TRUE;
  }
  अन्यथा
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  जर(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    result = Curl_output_digest(data,
                                proxy,
                                (स्थिरांक अचिन्हित अक्षर *)request,
                                (स्थिरांक अचिन्हित अक्षर *)path);
    जर(result)
      परत result;
  }
  अन्यथा
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
  जर(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    जर(
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
      जर(result)
        परत result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
  जर(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    जर(!proxy && Curl_creds_has_oauth_bearer(data->state.creds) &&
       !Curl_checkheaders(data, STRCONST("Authorization"))) {
      auth = "Bearer";
      result = http_output_bearer(data);
      जर(result)
        परत result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif

  जर(auth) {
#ifndef CURL_DISABLE_PROXY
    जर(proxy)
      data->info.proxyauthpicked = authstatus->picked;
    अन्यथा
      data->info.httpauthpicked = authstatus->picked;
    infof(data, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (data->state.creds ?
           data->state.creds->user : ""));
#अन्यथा
    (शून्य)proxy;
    infof(data, "Server auth using %s with user '%s'",
          auth, data->state.creds ?
          data->state.creds->user : "");
#endif
    authstatus->multipass = !authstatus->done;
  }
  अन्यथा {
    authstatus->multipass = FALSE;
    जर(proxy)
      data->info.proxyauthpicked = 0;
    अन्यथा
      data->info.httpauthpicked = 0;
  }

  परत result;
}

CURLcode Curl_http_output_auth(संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               स्थिरांक अक्षर *request,
                               Curl_HttpReq httpreq,
                               स्थिरांक अक्षर *path,
                               स्थिरांक अक्षर *query,
                               bool is_connect)
{
  CURLcode result = CURLE_OK;
  संरचना auth *authhost;
  संरचना auth *authproxy;
  स्थिरांक अक्षर *path_and_query = path;
  अक्षर *tmp_str = रिक्त;

  DEBUGASSERT(data);
  authhost = &data->state.authhost;
  authproxy = &data->state.authproxy;

  जर(
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
    जा out;
  }

  जर(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    जर(!tmp_str) {
      result = CURLE_OUT_OF_MEMORY;
      जा out;
    }
    path_and_query = tmp_str;
  }

  जर(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  जर(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ifndef CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  जर(conn->bits.origin_is_proxy || is_connect) {
    result = output_auth_headers(data, conn, authproxy, request,
                                 path_and_query, TRUE);
    जर(result)
      जा out;
  }
  अन्यथा
#अन्यथा
  (शून्य)is_connect;
#endif /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->done = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  जर(data->state.creds || Curl_auth_allowed_to_host(data))
    result = output_auth_headers(data, conn, authhost, request,
                                 path_and_query, FALSE);
  अन्यथा
    authhost->done = TRUE;

  जर(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_HEAD)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    data->req.authneg = TRUE;
  }
  अन्यथा
    data->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  परत result;
}

#अन्यथा /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               स्थिरांक अक्षर *request,
                               Curl_HttpReq httpreq,
                               स्थिरांक अक्षर *path,
                               स्थिरांक अक्षर *query,
                               bool is_connect)
{
  (शून्य)data;
  (शून्य)conn;
  (शून्य)request;
  (शून्य)httpreq;
  (शून्य)path;
  (शून्य)query;
  (शून्य)is_connect;
  परत CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP_AUTH, else */

#जर defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
स्थिर bool authcmp(स्थिरांक अक्षर *auth, स्थिरांक अक्षर *ओळ)
{
  /* the auth string must not have an alnum following */
  माप_प्रकार n = म_माप(auth);
  परत curl_strnequal(auth, ओळ, n) && !ISALNUM(ओळ[n]);
}
#endif

#ifdef USE_SPNEGO
स्थिर CURLcode auth_spnego(संरचना Curl_easy *data,
                            bool proxy,
                            स्थिरांक अक्षर *auth,
                            संरचना auth *authp,
                            uint32_t *availp)
{
  जर((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    जर(authp->picked == CURLAUTH_NEGOTIATE) {
      संरचना connectdata *conn = data->conn;
      CURLcode result = Curl_आदान_negotiate(data, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      जर(!result) {
        curlx_free(data->req.newurl);
        data->req.newurl = Curl_bufref_dup(&data->state.url);
        जर(!data->req.newurl)
          परत CURLE_OUT_OF_MEMORY;
        data->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      अन्यथा
        data->state.authproblem = TRUE;
    }
  }
  परत CURLE_OK;
}
#endif

#ifdef USE_NTLM
स्थिर CURLcode auth_ntlm(संरचना Curl_easy *data,
                          bool proxy,
                          स्थिरांक अक्षर *auth,
                          संरचना auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  जर((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    जर(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode result = Curl_आदान_ntlm(data, proxy, auth);
      जर(!result)
        data->state.authproblem = FALSE;
      अन्यथा {
        जर(result == CURLE_OUT_OF_MEMORY)
          परत result;
        infof(data, "NTLM authentication problem, ignoring.");
        data->state.authproblem = TRUE;
      }
    }
  }
  परत CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_DIGEST_AUTH
स्थिर CURLcode auth_digest(संरचना Curl_easy *data,
                            bool proxy,
                            स्थिरांक अक्षर *auth,
                            संरचना auth *authp,
                            uint32_t *availp)
{
  जर(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(data, "Ignoring duplicate digest auth header.");
  }
  अन्यथा जर(Curl_auth_is_digest_supported()) {
    CURLcode result;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    result = Curl_आदान_digest(data, proxy, auth);
    जर(result) {
      जर(result == CURLE_OUT_OF_MEMORY)
        परत result;
      infof(data, "Digest authentication problem, ignoring.");
      data->state.authproblem = TRUE;
    }
  }
  परत CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BASIC_AUTH
स्थिर CURLcode auth_basic(संरचना Curl_easy *data,
                           संरचना auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  जर(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Basic authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  परत CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BEARER_AUTH
स्थिर CURLcode auth_bearer(संरचना Curl_easy *data,
                            संरचना auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  जर(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Bearer authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  परत CURLE_OK;
}
#endif

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_आदान_auth(संरचना Curl_easy *data, bool proxy,
                              स्थिरांक अक्षर *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#जर defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  संरचना auth *authp;
  CURLcode result = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(data);

  जर(proxy) {
    availp = &data->info.proxyauthavail;
    authp = &data->state.authproxy;
  }
  अन्यथा {
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

  पर्यंत(*auth) {
#ifdef USE_SPNEGO
    जर(authcmp("Negotiate", auth))
      result = auth_spnego(data, proxy, auth, authp, availp);
#endif
#ifdef USE_NTLM
    जर(!result && authcmp("NTLM", auth))
      result = auth_ntlm(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
    जर(!result && authcmp("Digest", auth))
      result = auth_digest(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
    जर(!result && authcmp("Basic", auth))
      result = auth_basic(data, authp, availp);
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
    जर(authcmp("Bearer", auth))
      result = auth_bearer(data, authp, availp);
#endif

    जर(result)
      थांबा;

    /* there may be multiple methods on one line, so keep reading */
    auth = म_अक्षर(auth, ',');
    जर(auth) /* if we are on a comma, skip it */
      auth++;
    अन्यथा
      थांबा;
    curlx_str_passblanks(&auth);
  }
  परत result;
#अन्यथा
  (शून्य)data;
  (शून्य)proxy;
  (शून्य)auth;
  /* nothing to do when disabled */
  परत CURLE_OK;
#endif
}

स्थिर शून्य http_switch_to_get(संरचना Curl_easy *data, पूर्णांक code)
{
  स्थिरांक अक्षर *req = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  जर((req || data->state.httpreq != HTTPREQ_GET) &&
     (data->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((शून्य)code);
    infof(data, "Switch to GET because of %d response", code);
    data->state.http_ignorecustom = TRUE;
  }
  अन्यथा जर(req && (data->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(data, "Stick to %s instead of GET", req);

  data->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(data, FALSE);
}

#define HTTPREQ_IS_POST(data)                    \
  ((data)->state.httpreq == HTTPREQ_POST ||      \
   (data)->state.httpreq == HTTPREQ_POST_FORM || \
   (data)->state.httpreq == HTTPREQ_POST_MIME)

स्थिर CURLcode set_auto_referer(संरचना Curl_easy *data)
{
  CURLU *u;
  अक्षर *referer = रिक्त;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&data->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  जर(!u)
    परत CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&data->state.url), 0);
  जर(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, रिक्त, 0);
  जर(!uc)
    uc = curl_url_set(u, CURLUPART_USER, रिक्त, 0);
  जर(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, रिक्त, 0);
  जर(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  जर(uc || !referer)
    परत CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&data->state.referer, referer, 0, curl_free);
  परत CURLE_OK;
}

स्थिर CURLcode follow_parse_url(संरचना Curl_easy *data, स्थिरांक अक्षर *newurl,
                                 followtype type, अक्षर **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(data->state.uh);
  uc = curl_url_set(data->state.uh, CURLUPART_URL, newurl, (अचिन्हित पूर्णांक)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  जर(uc) {
    जर((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(data, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      परत Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    जर(!*follow_urlp)
      परत CURLE_OUT_OF_MEMORY;
  }
  अन्यथा {
    CURLU *u = curl_url();
    जर(!u)
      परत CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&data->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    जर(!uc)
      uc = curl_url_get(data->state.uh, CURLUPART_URL, follow_urlp, 0);
    जर(uc) {
      curl_url_cleanup(u);
      परत Curl_uc_to_curlcode(uc);
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, data->state.uh);
      curl_url_cleanup(u);
      जर(!same_origin)
        Curl_auth_digest_cleanup(&data->state.digest);
    }
#अन्यथा
    curl_url_cleanup(u);
#endif
  }
  परत CURLE_OK;
}

स्थिर CURLcode follow_fake(संरचना Curl_easy *data, अक्षर *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  जर(!uc) {
    CURLU *u = curl_url();
    अक्षर *nocred = रिक्त;

    जर(!u) {
      curlx_free(follow_url);
      परत CURLE_OUT_OF_MEMORY;
    }
    जर(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, रिक्त, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, रिक्त, 0))
      (शून्य)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    जर(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  data->info.wouldredirect = follow_url;

  जर(reachedmax) {
    failf(data, "Maximum (%d) redirects followed", data->set.maxredirs);
    परत CURLE_TOO_MANY_REDIRECTS;
  }
  परत CURLE_OK;
}

स्थिर bool follow_switch_get(संरचना Curl_easy *data)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  निवड(data->info.httpcode) {
  स्थिती 301: /* Moved Permanently */
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
    जर(HTTPREQ_IS_POST(data) && !data->set.post301) {
      http_switch_to_get(data, 301);
      परत TRUE;
    }
    थांबा;
  स्थिती 302: /* Found */
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
    जर(HTTPREQ_IS_POST(data) && !data->set.post302) {
      http_switch_to_get(data, 302);
      परत TRUE;
    }
    थांबा;
  स्थिती 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    जर(!HTTPREQ_IS_POST(data) || !data->set.post303) {
      http_switch_to_get(data, 303);
      परत TRUE;
    }
    थांबा;

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
  पूर्वनिर्धारित:
    थांबा;
  }
  परत FALSE;
}

CURLcode Curl_http_follow(संरचना Curl_easy *data, स्थिरांक अक्षर *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  अक्षर *follow_url = रिक्त;
  CURLUcode uc;
  CURLcode result;
  CURLcode rewind_result;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  जर(type != FOLLOW_FAKE)
    data->state.requests++; /* count all real follows */
  जर(type == FOLLOW_REDIR) {
    जर((data->set.maxredirs != -1) &&
       (data->state.followlocation >= data->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    अन्यथा {
      data->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      जर(data->set.http_auto_referer) {
        result = set_auto_referer(data);
        जर(result)
          परत result;
      }
    }
  }

  जर((type != FOLLOW_RETRY) &&
     (data->req.httpcode != 401) && (data->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, रिक्त, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  result = follow_parse_url(data, newurl, type, &follow_url, &uc);
  जर(result)
    परत result;

  DEBUGASSERT(follow_url);

  जर(type == FOLLOW_FAKE)
    परत follow_fake(data, follow_url, uc, reachedmax);

  जर(disallowport)
    data->state.allow_port = FALSE;

  Curl_bufref_set(&data->state.url, follow_url, 0, curl_free);
  rewind_result = Curl_req_soft_reset(&data->req, data);
  infof(data, "Issue another request to this URL: '%s'", follow_url);
  जर((data->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    data->state.http_ignorecustom = TRUE;
    infof(data, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(data);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  जर(rewind_result && !switch_to_get)
    परत rewind_result;

  Curl_pgrsTime(data, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(data);

  परत CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(स्थिरांक अक्षर *headerline, /* line to check */
                        स्थिरांक अक्षर *header, /* header keyword _with_ colon */
                        स्थिरांक माप_प्रकार hlen, /* len of the keyword in bytes */
                        स्थिरांक अक्षर *content, /* content string to find */
                        स्थिरांक माप_प्रकार clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  स्थिरांक अक्षर *p;
  संरचना Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  जर(!curl_strnequal(headerline, header, hlen))
    परत FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  जर(curlx_str_cspn(&p, &val, "\r\n"))
    परत FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  जर(curlx_strlen(&val) >= clen) {
    माप_प्रकार len;
    p = curlx_str(&val);
    साठी(len = curlx_strlen(&val); len >= clen;) {
      संरचना Curl_str next;
      स्थिरांक अक्षर *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      जर(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        परत TRUE; /* match! */
      /* advance to the next comma */
      जर(curlx_str_until(&p, &next, len, ',') ||
         curlx_str_single(&p, ','))
        थांबा; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      करा
        curlx_str_passblanks(&p);
      पर्यंत(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      जर((माप_प्रकार)(p - o) > len)
        थांबा;
      len -= (p - o);
    }
  }
  परत FALSE; /* no match */
}

संरचना cr_exp100_ctx {
  संरचना Curl_creader super;
  संरचना curltime start; /* time started waiting */
  गणना expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

स्थिर शून्य http_exp100_continue(संरचना Curl_easy *data,
                                 संरचना Curl_creader *reader)
{
  संरचना cr_exp100_ctx *ctx = reader->ctx;
  जर(ctx->state > EXP100_SEND_DATA) {
    ctx->state = EXP100_SEND_DATA;
    Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
  }
}

स्थिर CURLcode cr_exp100_read(संरचना Curl_easy *data,
                               संरचना Curl_creader *reader,
                               अक्षर *buf, माप_प्रकार blen,
                               माप_प्रकार *nread, bool *eos)
{
  संरचना cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  निवड(ctx->state) {
  स्थिती EXP100_SENDING_REQUEST:
    जर(!Curl_req_sendbuf_empty(data)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(data, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      परत CURLE_OK;
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
    परत CURLE_OK;
  स्थिती EXP100_FAILED:
    DEBUGF(infof(data, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    परत CURLE_READ_ERROR;
  स्थिती EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->start);
    जर(ms < data->set.expect_100_timeout) {
      DEBUGF(infof(data, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      परत CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(data, reader);
    infof(data, "Done waiting for 100-continue");
    FALLTHROUGH();
  पूर्वनिर्धारित:
    DEBUGF(infof(data, "cr_exp100_read, pass through"));
    परत Curl_creader_read(data, reader->next, buf, blen, nread, eos);
  }
}

स्थिर शून्य cr_exp100_done(संरचना Curl_easy *data,
                           संरचना Curl_creader *reader, पूर्णांक premature)
{
  संरचना cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_DATA;
  Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
}

स्थिर स्थिरांक संरचना Curl_crtype cr_exp100 = {
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
  आकार(संरचना cr_exp100_ctx)
};

स्थिर CURLcode http_exp100_add_reader(संरचना Curl_easy *data)
{
  संरचना Curl_creader *reader = रिक्त;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_exp100, CURL_CR_PROTOCOL);
  जर(!result)
    result = Curl_creader_add(data, reader);
  जर(!result) {
    संरचना cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  जर(result && reader)
    Curl_creader_free(data, reader);
  परत result;
}

स्थिर शून्य http_exp100_got100(संरचना Curl_easy *data)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  जर(r)
    http_exp100_continue(data, r);
}

स्थिर bool http_exp100_is_waiting(संरचना Curl_easy *data)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  जर(r) {
    संरचना cr_exp100_ctx *ctx = r->ctx;
    परत ctx->state == EXP100_AWAITING_CONTINUE;
  }
  परत FALSE;
}

स्थिर शून्य http_exp100_send_anyway(संरचना Curl_easy *data)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  जर(r)
    http_exp100_continue(data, r);
}

स्थिर bool http_exp100_is_selected(संरचना Curl_easy *data)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  परत !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(संरचना Curl_easy *data,
                                 संरचना easy_pollset *ps)
{
  /* write mode */
  परत Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(संरचना Curl_easy *data,
                                   संरचना easy_pollset *ps)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  जर(CURL_REQ_WANT_RECV(data)) {
    result = Curl_pollset_add_in(data, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  जर(!result && Curl_req_want_send(data) && !http_exp100_is_waiting(data)) {
    result = Curl_pollset_add_out(data, ps, conn->sock[FIRSTSOCKET]);
  }
  परत result;
}

स्थिर CURLcode http_write_header(संरचना Curl_easy *data,
                                  स्थिरांक अक्षर *hd, माप_प्रकार hdlen)
{
  CURLcode result;
  पूर्णांक writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((data->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  result = Curl_client_write(data, writetype, hd, hdlen);
  जर(result)
    परत result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  जर(result)
    परत result;

  data->req.deductheadercount = (100 <= data->req.httpcode &&
                                 199 >= data->req.httpcode) ?
    data->req.headerbytecount : 0;
  परत result;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(संरचना Curl_easy *data,
                        CURLcode status, bool premature)
{
  संरचना connectdata *conn = data->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  data->state.authhost.multipass = FALSE;
  data->state.authproxy.multipass = FALSE;

  जर(curlx_dyn_len(&data->state.headerb)) {
    (शून्य)http_write_header(data, curlx_dyn_ptr(&data->state.headerb),
                            curlx_dyn_len(&data->state.headerb));
  }
  curlx_dyn_reset(&data->state.headerb);

  जर(status)
    परत status;

  जर(!premature && /* this check is pointless when DONE is called before the
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
    परत CURLE_GOT_NOTHING;
  }

  परत CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
स्थिर bool http_may_use_1_1(स्थिरांक संरचना Curl_easy *data)
{
  स्थिरांक संरचना connectdata *conn = data->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  जर(data->state.http_neg.rcvd_min == 10)
    परत FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  जर(conn && conn->httpversion_seen == 10)
    परत FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  जर(data->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    परत FALSE;
  /* We are not restricted to use 1.0 only. */
  परत !data->state.http_neg.only_10;
}

स्थिर अचिन्हित अक्षर http_request_version(संरचना Curl_easy *data)
{
  अचिन्हित अक्षर v = Curl_conn_http_version(data, data->conn);
  जर(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(data) ? 11 : 10;
  }
  परत v;
}

स्थिर स्थिरांक अक्षर *get_http_string(पूर्णांक httpversion)
{
  निवड(httpversion) {
  स्थिती 30:
    परत "3";
  स्थिती 20:
    परत "2";
  स्थिती 11:
    परत "1.1";
  पूर्वनिर्धारित:
    परत "1.0";
  }
}

CURLcode Curl_add_custom_headers(संरचना Curl_easy *data,
                                 bool is_connect, पूर्णांक httpversion,
                                 संरचना dynbuf *req)
{
  संरचना curl_slist *h[2];
  संरचना curl_slist *headers;
  पूर्णांक numlists = 1; /* by default */
  पूर्णांक i;

#ifndef CURL_DISABLE_PROXY
  गणना Curl_proxy_use proxy;

  जर(is_connect)
    proxy = HEADER_CONNECT;
  अन्यथा
    proxy = data->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  निवड(proxy) {
  स्थिती HEADER_SERVER:
    h[0] = data->set.headers;
    थांबा;
  स्थिती HEADER_PROXY:
    h[0] = data->set.headers;
    जर(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    थांबा;
  स्थिती HEADER_CONNECT:
    जर(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    अन्यथा
      h[0] = data->set.headers;
    थांबा;
  स्थिती HEADER_CONNECT_UDP:
    जर(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    अन्यथा
      h[0] = data->set.headers;
    थांबा;
  }
#अन्यथा
  (शून्य)is_connect;
  h[0] = data->set.headers;
#endif

  /* loop through one or two lists */
  साठी(i = 0; i < numlists; i++) {
    साठी(headers = h[i]; headers; headers = headers->next) {
      CURLcode result = CURLE_OK;
      bool blankheader = FALSE;
      संरचना Curl_str name;
      स्थिरांक अक्षर *p = headers->data;
      स्थिरांक अक्षर *origp = p;
      माप_प्रकार hlen = म_माप(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      जर(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !स_प्रथम(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      अन्यथा {
        p = origp;
        जर(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          संरचना Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          जर(!curlx_strlen(&val))
            /* no content, do not send this */
            सुरूठेवा;
        }
        अन्यथा
          /* no colon */
          सुरूठेवा;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      जर(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      अन्यथा जर(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा जर(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा जर(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      अन्यथा जर(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      अन्यथा जर((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      अन्यथा जर((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      अन्यथा जर(blankheader) {
        result = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        जर(!result)
          result = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      अन्यथा
        result = curlx_dyn_addf(req, "%s\r\n", origp);

      जर(result)
        परत result;
    }
  }

  परत CURLE_OK;
}

#ifndef CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(संरचना Curl_easy *data,
                                संरचना dynbuf *req)
{
  स्थिरांक संरचना पंचांग *पंचांग;
  संरचना पंचांग keeptime;
  CURLcode result;
  अक्षर datestr[80];
  स्थिरांक अक्षर *condp;
  माप_प्रकार len;

  जर(data->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    परत CURLE_OK;

  result = curlx_gmtime(data->set.timevalue, &keeptime);
  जर(result) {
    failf(data, "Invalid TIMEVALUE");
    परत result;
  }
  पंचांग = &keeptime;

  निवड(data->set.timecondition) {
  पूर्वनिर्धारित:
    DEBUGF(infof(data, "invalid time condition"));
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  स्थिती CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    थांबा;
  स्थिती CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    थांबा;
  स्थिती CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    थांबा;
  }

  जर(Curl_checkheaders(data, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    परत CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, आकार(datestr),
                 "%s: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                 condp,
                 Curl_wkday[पंचांग->tm_wday ? पंचांग->tm_wday - 1 : 6],
                 पंचांग->tm_mday,
                 Curl_month[पंचांग->tm_mon],
                 पंचांग->tm_year + 1900,
                 पंचांग->tm_hour,
                 पंचांग->tm_min,
                 पंचांग->tm_sec);

  result = curlx_dyn_add(req, datestr);
  परत result;
}
#अन्यथा
/* disabled */
CURLcode Curl_add_timecondition(संरचना Curl_easy *data,
                                संरचना dynbuf *req)
{
  (शून्य)data;
  (शून्य)req;
  परत CURLE_OK;
}
#endif

शून्य Curl_http_method(संरचना Curl_easy *data,
                      स्थिरांक अक्षर **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)data->state.httpreq;
  स्थिरांक अक्षर *request;
#ifndef CURL_DISABLE_WEBSOCKETS
  जर(data->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  अन्यथा
#endif
  जर((data->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     data->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  जर(!data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
  }
  अन्यथा {
    जर(data->req.no_body)
      request = "HEAD";
    अन्यथा {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_HEAD));
      निवड(httpreq) {
      स्थिती HTTPREQ_POST:
      स्थिती HTTPREQ_POST_FORM:
      स्थिती HTTPREQ_POST_MIME:
        request = "POST";
        थांबा;
      स्थिती HTTPREQ_PUT:
        request = "PUT";
        थांबा;
      पूर्वनिर्धारित: /* this should never happen */
      स्थिती HTTPREQ_GET:
        request = "GET";
        थांबा;
      स्थिती HTTPREQ_HEAD:
        request = "HEAD";
        थांबा;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

स्थिर CURLcode http_set_aptr_host(संरचना Curl_easy *data)
{
  संरचना connectdata *conn = data->conn;
  स्थिरांक अक्षर *ptr = रिक्त;

  curlx_safefree(data->state.http_host);
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(data->req.cookiehost);
#endif

  जर(Curl_peer_equal(data->state.initial_origin, data->state.origin))
    ptr = Curl_checkheaders(data, STRCONST("Host"));

  जर(ptr) {
#ifndef CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    अक्षर *cookiehost;
    CURLcode result = copy_custom_मूल्य(ptr, &cookiehost);
    जर(result)
      परत result;
    जर(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    अन्यथा {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      जर(*cookiehost == '[') {
        अक्षर *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        स_हटाओ(cookiehost, cookiehost + 1, म_माप(cookiehost) - 1);
        closingbracket = म_अक्षर(cookiehost, ']');
        जर(closingbracket)
          *closingbracket = 0;
      }
      अन्यथा {
        पूर्णांक startsearch = 0;
        अक्षर *colon = म_अक्षर(cookiehost + startsearch, ':');
        जर(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      data->req.cookiehost = cookiehost;
    }
#endif

    जर(!curl_strequal("Host:", ptr)) {
      data->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      जर(!data->state.http_host)
        परत CURLE_OUT_OF_MEMORY;
    }
  }
  अन्यथा {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)data->state.origin->ipv6;
    संरचना dynbuf tmp;
    माप_प्रकार hlen;
    CURLcode result;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    result = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    जर(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("["));
    जर(!result)
      result = curlx_dyn_add(&tmp, data->state.origin->hostname);
    जर(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("]"));
    जर(!result &&
       ((data->state.origin->port != data->state.origin->scheme->defport) ||
       (data->state.origin->scheme->family != conn->scheme->family))) {
      result = curlx_dyn_addf(&tmp, ":%u", data->state.origin->port);
    }

    data->state.http_host = result ? रिक्त : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    परत result;
  }
  परत CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
स्थिर CURLcode http_target(संरचना Curl_easy *data,
                            संरचना dynbuf *r)
{
  CURLcode result = CURLE_OK;
  स्थिरांक अक्षर *path = data->state.up.path;
  स्थिरांक अक्षर *query = data->state.up.query;
#ifndef CURL_DISABLE_PROXY
  संरचना connectdata *conn = data->conn;
#endif

  जर(CURL_EASY_STR(data, STRING_TARGET)) {
    path = CURL_EASY_STR(data, STRING_TARGET);
    query = रिक्त;
  }

#ifndef CURL_DISABLE_PROXY
  जर(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    अक्षर *url;
    CURLU *h = curl_url_dup(data->state.uh);
    जर(!h)
      परत CURLE_OUT_OF_MEMORY;

    जर(!data->state.origin->ipv6 &&
       (data->state.origin->user_hostname != data->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, data->state.origin->hostname, 0);
      जर(uc) {
        curl_url_cleanup(h);
        परत CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, रिक्त, 0);
    जर(uc) {
      curl_url_cleanup(h);
      परत CURLE_OUT_OF_MEMORY;
    }

    जर(data->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, रिक्त, 0);
      जर(uc) {
        curl_url_cleanup(h);
        परत CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, रिक्त, 0);
      जर(uc) {
        curl_url_cleanup(h);
        परत CURLE_OUT_OF_MEMORY;
      }
    }
    अन्यथा जर(data->state.creds && (data->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        data->state.creds->user, CURLU_URLENCODE);
      जर(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          data->state.creds->passwd, CURLU_URLENCODE);
      जर(uc) {
        curl_url_cleanup(h);
        परत Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    जर(uc) {
      curl_url_cleanup(h);
      परत CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    result = curlx_dyn_add(r, CURL_EASY_STR(data, STRING_TARGET) ?
      CURL_EASY_STR(data, STRING_TARGET) : url);
    curlx_free(url);
    जर(result)
      परत result;

    जर((data->state.origin->scheme == &Curl_scheme_ftp) &&
       data->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      माप_प्रकार len = म_माप(path);
      bool type_present = FALSE;
      जर((len >= 7) && !स_भेद(&path[len - 7], ";type=", 6)) {
        निवड(Curl_raw_toupper(path[len - 1])) {
        स्थिती 'A':
        स्थिती 'D':
        स्थिती 'I':
          type_present = TRUE;
          थांबा;
        }
      }
      जर(!type_present) {
        result = curlx_dyn_addf(r, ";type=%c",
                                data->state.prefer_ascii ? 'a' : 'i');
        जर(result)
          परत result;
      }
    }
  }

  अन्यथा
#endif
  {
    result = curlx_dyn_add(r, path);
    जर(result)
      परत result;
    जर(query)
      result = curlx_dyn_addf(r, "?%s", query);
  }

  परत result;
}

#जर !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
स्थिर CURLcode set_post_reader(संरचना Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result;

  निवड(httpreq) {
#ifndef CURL_DISABLE_MIME
  स्थिती HTTPREQ_POST_MIME:
    data->state.mimepost = data->set.mimepostp;
    थांबा;
#endif
#ifndef CURL_DISABLE_FORM_API
  स्थिती HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    जर(!data->state.formp) {
      data->state.formp = curlx_calloc(1, आकार(curl_mimepart));
      जर(!data->state.formp)
        परत CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(data->state.formp);
      result = Curl_getformdata(data, data->state.formp, data->set.httppost,
                                data->state.fread_func);
      जर(result) {
        curlx_safefree(data->state.formp);
        परत result;
      }
      data->state.mimepost = data->state.formp;
    }
    थांबा;
#endif
  पूर्वनिर्धारित:
    data->state.mimepost = रिक्त;
    थांबा;
  }

  निवड(httpreq) {
  स्थिती HTTPREQ_POST_FORM:
  स्थिती HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ifndef CURL_DISABLE_MIME
    जर(data->state.mimepost) {
      स्थिरांक अक्षर *cthdr = Curl_checkheaders(data, STRCONST("Content-Type"));

      /* Read and seek body only. */
      data->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      जर(cthdr)
        साठी(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      अन्यथा जर(data->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(data->state.mimepost, data->set.headers, 0);
      result = Curl_mime_prepare_headers(data, data->state.mimepost, cthdr,
                                         रिक्त, MIMESTRATEGY_FORM);
      जर(result)
        परत result;
      curl_mime_headers(data->state.mimepost, रिक्त, 0);
      result = Curl_creader_set_mime(data, data->state.mimepost);
      जर(result)
        परत result;
    }
    अन्यथा
#endif
    {
      result = Curl_creader_set_null(data);
    }
    data->state.infilesize = Curl_creader_total_length(data);
    परत result;

  पूर्वनिर्धारित:
    परत Curl_creader_set_null(data);
  }
  /* never reached */
}
#endif

स्थिर CURLcode set_reader(संरचना Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t postsize = data->state.infilesize;

  DEBUGASSERT(data->conn);

  जर(data->req.authneg) {
    परत Curl_creader_set_null(data);
  }

  निवड(httpreq) {
  स्थिती HTTPREQ_PUT: /* Let's PUT the data to the server! */
    परत postsize ? Curl_creader_set_fread(data, postsize) :
      Curl_creader_set_null(data);

#जर !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  स्थिती HTTPREQ_POST_FORM:
  स्थिती HTTPREQ_POST_MIME:
    परत set_post_reader(data, httpreq);
#endif

  स्थिती HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    जर(!postsize) {
      result = Curl_creader_set_null(data);
    }
    अन्यथा जर(data->set.postfields) {
      माप_प्रकार plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      जर(plen == SIZE_MAX)
        परत CURLE_OUT_OF_MEMORY;
      अन्यथा जर(plen)
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      अन्यथा
        result = Curl_creader_set_null(data);
    }
    अन्यथा {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      अक्षर *ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
      जर(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      result = Curl_creader_set_fread(data, chunked ? -1 : postsize);
    }
    परत result;

  पूर्वनिर्धारित:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    data->state.infilesize = 0;
    परत Curl_creader_set_null(data);
  }
  /* not reached */
}

स्थिर CURLcode http_resume(संरचना Curl_easy *data, Curl_HttpReq httpreq)
{
  जर((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     data->state.resume_from) {
    failf(data, "HTTP upload cannot be resumed");
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }
  परत CURLE_OK;
}

स्थिर CURLcode http_req_set_TE(संरचना Curl_easy *data,
                                संरचना dynbuf *req,
                                पूर्णांक httpversion)
{
  CURLcode result = CURLE_OK;
  स्थिरांक अक्षर *ptr;

  ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
  जर(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    data->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    जर(data->req.upload_chunky && (httpversion >= 20)) {
      infof(data, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      data->req.upload_chunky = FALSE;
    }
  }
  अन्यथा {
    curl_off_t req_clen = Curl_creader_total_length(data);

    जर(req_clen < 0) {
      /* indeterminate request content length */
      जर(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        data->req.upload_chunky = (httpversion < 20);
      }
      अन्यथा {
        failf(data, "Chunky upload is not supported by HTTP 1.0");
        परत CURLE_UPLOAD_FAILED;
      }
    }
    अन्यथा {
      /* else, no chunky upload */
      data->req.upload_chunky = FALSE;
    }

    जर(data->req.upload_chunky)
      result = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  परत result;
}

स्थिर CURLcode addexpect(संरचना Curl_easy *data, संरचना dynbuf *r,
                          पूर्णांक httpversion, bool *announced_exp100)
{
  CURLcode result;
  अक्षर *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  जर(data->req.upgr101 != UPGR101_NONE)
    परत CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(data, STRCONST("Expect"));
  जर(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  अन्यथा जर(!data->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_length(data);
    जर(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      result = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      जर(result)
        परत result;
      *announced_exp100 = TRUE;
    }
  }
  परत CURLE_OK;
}

स्थिर CURLcode http_add_content_hds(संरचना Curl_easy *data,
                                     संरचना dynbuf *r,
                                     पूर्णांक httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(data->conn);
  जर(data->req.upload_chunky) {
    result = Curl_httpchunk_add_reader(data);
    जर(result)
      परत result;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_total_length(data);
  निवड(httpreq) {
  स्थिती HTTPREQ_PUT:
  स्थिती HTTPREQ_POST:
#जर !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  स्थिती HTTPREQ_POST_FORM:
  स्थिती HTTPREQ_POST_MIME:
#endif
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    जर(req_clen >= 0 && !data->req.upload_chunky &&
       (data->req.authneg ||
        !Curl_checkheaders(data, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      result = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    जर(result)
      जा out;

#ifndef CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    जर(data->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      संरचना curl_slist *hdr;

      साठी(hdr = data->state.mimepost->curlheaders; hdr; hdr = hdr->next) {
        result = curlx_dyn_addf(r, "%s\r\n", hdr->data);
        जर(result)
          जा out;
      }
    }
#endif
    जर(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(data, STRCONST("Content-Type"))) {
      result = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      जर(result)
        जा out;
    }
    result = addexpect(data, r, httpversion, &announced_exp100);
    जर(result)
      जा out;
    थांबा;
  पूर्वनिर्धारित:
    थांबा;
  }

  Curl_pgrsSetUploadSize(data, req_clen);
  जर(announced_exp100)
    result = http_exp100_add_reader(data);

out:
  परत result;
}

#ifndef CURL_DISABLE_COOKIES

स्थिर CURLcode http_cookies(संरचना Curl_easy *data,
                             संरचना dynbuf *r)
{
  CURLcode result = CURLE_OK;
  स्थिरांक अक्षर *addcookies = रिक्त;
  bool linecap = FALSE;
  जर(CURL_EASY_STR(data, STRING_COOKIE) &&
     !Curl_checkheaders(data, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(data))
    addcookies = CURL_EASY_STR(data, STRING_COOKIE);

  जर(data->cookies || addcookies) {
    संरचना Curl_llist list;
    पूर्णांक मोजणी = 0;

    जर(data->cookies && data->state.cookie_engine) {
      bool okay;
      स्थिरांक अक्षर *host = data->req.cookiehost ?
        data->req.cookiehost : data->state.origin->hostname;
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      result = Curl_cookie_getlist(data, &okay, host, &list);
      जर(!result && okay) {
        संरचना Curl_llist_गाठ *n;
        माप_प्रकार clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        साठी(n = Curl_llist_head(&list); n; n = Curl_गाठ_next(n)) {
          संरचना Cookie *co = Curl_गाठ_elem(n);
          जर(co->मूल्य) {
            माप_प्रकार add;
            जर(!मोजणी) {
              result = curlx_dyn_addn(r, STRCONST("Cookie: "));
              जर(result)
                थांबा;
            }
            add = म_माप(co->name) + म_माप(co->मूल्य) + 1;
            जर(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(data, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              थांबा;
            }
            result = curlx_dyn_addf(r, "%s%s=%s", मोजणी ? "; " : "",
                                    co->name, co->मूल्य);
            जर(result)
              थांबा;
            clen += add + (मोजणी ? 2 : 0);
            मोजणी++;
          }
        }
        Curl_llist_destroy(&list, रिक्त);
      }
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
    जर(addcookies && !result && !linecap) {
      जर(!मोजणी)
        result = curlx_dyn_addn(r, STRCONST("Cookie: "));
      जर(!result) {
        result = curlx_dyn_addf(r, "%s%s", मोजणी ? "; " : "", addcookies);
        मोजणी++;
      }
    }
    जर(मोजणी && !result)
      result = curlx_dyn_addn(r, STRCONST("\r\n"));

    जर(result)
      परत result;
  }
  परत result;
}
#अन्यथा
#define http_cookies(a, b) CURLE_OK
#endif

स्थिर CURLcode http_range(संरचना Curl_easy *data,
                           Curl_HttpReq httpreq)
{
  जर(data->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    जर(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_HEAD)) &&
       !Curl_checkheaders(data, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);
      data->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 data->state.range);
      जर(!data->state.rangeline)
        परत CURLE_OUT_OF_MEMORY;
    }
    अन्यथा जर((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(data, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_total_length(data);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);

      जर(data->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      अन्यथा जर(data->state.resume_from) {
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
      अन्यथा {
        /* Range was selected and then we pass the incoming range and append
           total size */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        data->state.range, req_clen);
      }
      जर(!data->state.rangeline)
        परत CURLE_OUT_OF_MEMORY;
    }
  }
  परत CURLE_OK;
}

स्थिर CURLcode http_firstwrite(संरचना Curl_easy *data)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;

  जर(data->req.newurl) {
    जर(conn->bits.बंद) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      परत CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(data, "Ignoring the response-body");
  }
  जर(data->state.resume_from && !k->content_range &&
     (data->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    जर(k->size == data->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(data, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      परत CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(data, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    परत CURLE_RANGE_ERROR;
  }

  जर(data->set.timecondition && !data->state.range &&
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
    परत CURLE_OK;
  } /* we have a time condition */

  परत CURLE_OK;
}

स्थिर CURLcode http_check_new_conn(संरचना Curl_easy *data)
{
  संरचना connectdata *conn = data->conn;
  स्थिरांक अक्षर *info_version = रिक्त;
  स्थिरांक अक्षर *alpn;
  CURLcode result;

  alpn = Curl_conn_get_alpn_negotiated(data, conn);
  जर(alpn && !म_भेद("h3", alpn)) {
#ifndef CURL_DISABLE_PROXY
    जर(!conn->bits.origin_is_proxy)
#endif
      DEBUGASSERT(Curl_conn_http_version(data, conn) == 30);
    info_version = "HTTP/3";
  }
  अन्यथा जर(alpn && !म_भेद("h2", alpn)) {
#ifndef CURL_DISABLE_PROXY
    जर((Curl_conn_http_version(data, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      result = Curl_http2_switch(data);
      जर(result)
        परत result;
    }
    अन्यथा
#endif
    DEBUGASSERT(Curl_conn_http_version(data, conn) == 20);
    info_version = "HTTP/2";
  }
  अन्यथा {
    /* Check if user wants to use HTTP/2 with clear TCP */
    जर(Curl_http2_may_switch(data)) {
      DEBUGF(infof(data, "HTTP/2 over clean TCP"));
      result = Curl_http2_switch(data);
      जर(result)
        परत result;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    अन्यथा
      info_version = "HTTP/1.x";
  }

  जर(info_version)
    infof(data, "using %s", info_version);
  परत CURLE_OK;
}

स्थिर CURLcode http_add_connection_hd(संरचना Curl_easy *data,
                                       संरचना dynbuf *req)
{
  संरचना curl_slist *head;
  स्थिरांक अक्षर *sep = "Connection: ";
  CURLcode result = CURLE_OK;
  माप_प्रकार rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  साठी(head = data->set.headers; head; head = head->next) {
    जर(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      अक्षर *मूल्य;
      result = copy_custom_मूल्य(head->data, &मूल्य);
      जर(result)
        परत result;
      result = curlx_dyn_addf(req, "%s%s", sep, मूल्य);
      sep = ", ";
      curlx_free(मूल्य);
      थांबा; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  जर(!result && data->state.http_hd_te) {
    result = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  जर(!result && data->state.http_hd_upgrade) {
    result = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  जर(!result && data->state.http_hd_h2_settings) {
    result = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  जर(!result && (rlen < curlx_dyn_len(req)))
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
  जर(result)
    परत result;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  साठी(head = data->set.headers; head; head = head->next) {
    जर(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      जर(skip) {
        skip = FALSE;
        सुरूठेवा;
      }
      result = curlx_dyn_addf(req, "%s\r\n", head->data);
      जर(result)
        परत result;
    }
  }

  परत CURLE_OK;
}

/* Header identifier in order we send them by default */
प्रकारव्याख्या गणना {
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

स्थिर CURLcode http_add_hd(संरचना Curl_easy *data,
                            संरचना dynbuf *req,
                            http_hd_t id,
                            अचिन्हित अक्षर httpversion,
                            स्थिरांक अक्षर *method,
                            Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
#जर !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  संरचना connectdata *conn = data->conn;
#endif
  निवड(id) {
  स्थिती H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    result = curlx_dyn_addf(req, "%s ", method);
    जर(!result)
      result = http_target(data, req);
    जर(!result)
      result = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    थांबा;

  स्थिती H1_HD_HOST:
    जर(data->state.http_host) {
      result = curlx_dyn_add(req, data->state.http_host);
      जर(!result)
        result = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    थांबा;

#ifndef CURL_DISABLE_PROXY
  स्थिती H1_HD_PROXY_AUTH:
    जर(data->req.hd_proxy_auth)
      result = curlx_dyn_add(req, data->req.hd_proxy_auth);
    थांबा;
#endif

  स्थिती H1_HD_AUTH:
    जर(data->req.hd_auth)
      result = curlx_dyn_add(req, data->req.hd_auth);
    थांबा;

  स्थिती H1_HD_RANGE:
    जर(data->state.use_range && data->state.rangeline)
      result = curlx_dyn_add(req, data->state.rangeline);
    थांबा;

  स्थिती H1_HD_USER_AGENT: {
    स्थिरांक अक्षर *ua = CURL_EASY_STR(data, STRING_USERAGENT);
    जर(ua && *ua && !Curl_checkheaders(data, STRCONST("User-Agent")))
      result = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    थांबा;
  }

  स्थिती H1_HD_ACCEPT:
    जर(!Curl_checkheaders(data, STRCONST("Accept")))
      result = curlx_dyn_add(req, "Accept: */*\r\n");
    थांबा;

  स्थिती H1_HD_TE:
#ifdef HAVE_LIBZ
    जर(!Curl_checkheaders(data, STRCONST("TE")) &&
       data->set.http_transfer_encoding) {
      data->state.http_hd_te = TRUE;
      result = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#endif
    थांबा;

  स्थिती H1_HD_ACCEPT_ENCODING: {
    स्थिरांक अक्षर *enc = CURL_EASY_STR(data, STRING_ENCODING);
    जर(enc && !Curl_checkheaders(data, STRCONST("Accept-Encoding")))
      result = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    थांबा;
  }

  स्थिती H1_HD_REFERER:
    जर(Curl_bufref_ptr(&data->state.referer) &&
       !Curl_checkheaders(data, STRCONST("Referer")))
      result = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&data->state.referer));
    थांबा;

#ifndef CURL_DISABLE_PROXY
  स्थिती H1_HD_PROXY_CONNECTION:
    जर(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(data, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(data, data->conn, STRCONST("Proxy-Connection")))
      result = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    थांबा;
#endif

  स्थिती H1_HD_TRANSFER_ENCODING:
    result = http_req_set_TE(data, req, httpversion);
    थांबा;

#ifndef CURL_DISABLE_ALTSVC
  स्थिती H1_HD_ALT_USED:
    जर(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(data, STRCONST("Alt-Used")))
      result = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    थांबा;
#endif

  स्थिती H1_HD_UPGRADE:
    जर(!Curl_conn_is_ssl(data->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
       data->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      result = Curl_http2_request_upgrade(req, data);
    }
#ifndef CURL_DISABLE_WEBSOCKETS
    जर(!result && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      result = Curl_ws_request(data, req);
#endif
    थांबा;

  स्थिती H1_HD_COOKIES:
    result = http_cookies(data, req);
    थांबा;

  स्थिती H1_HD_CONDITIONALS:
    result = Curl_add_timecondition(data, req);
    थांबा;

  स्थिती H1_HD_CUSTOM:
    result = Curl_add_custom_headers(data, FALSE, httpversion, req);
    थांबा;

  स्थिती H1_HD_CONTENT:
    result = http_add_content_hds(data, req, httpversion, httpreq);
    थांबा;

  स्थिती H1_HD_CONNECTION: {
    result = http_add_connection_hd(data, req);
    थांबा;
  }

  स्थिती H1_HD_LAST:
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
    थांबा;
  }
  परत result;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  Curl_HttpReq httpreq;
  स्थिरांक अक्षर *method;
  संरचना dynbuf req;
  अचिन्हित अक्षर httpversion;
  माप_प्रकार hd_id;

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

  जर(!data->conn->bits.reuse) {
    result = http_check_new_conn(data);
    जर(result)
      जा out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  result = Curl_headers_init(data);
  जर(result)
    जा out;

  data->state.http_hd_te = FALSE;
  data->state.http_hd_upgrade = FALSE;
  data->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(data, &method, &httpreq);

  /* select host to send */
  result = http_set_aptr_host(data);
  /* setup the authentication headers, how that method and host are known */
  जर(!result)
    result = Curl_http_output_auth(data, data->conn, method, httpreq,
                                   data->state.up.path,
                                   data->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  जर(!result)
    result = set_reader(data, httpreq);
  जर(!result)
    result = http_resume(data, httpreq);
  जर(!result)
    result = http_range(data, httpreq);
  जर(result)
    जा out;

  httpversion = http_request_version(data);
  /* Add request line and all headers to `req` */
  साठी(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    result = http_add_hd(data, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    जर(result)
      जा out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);
  result = Curl_req_send(data, &req, httpversion);

  जर((httpversion >= 20) && data->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    data->req.upload_chunky = FALSE;

out:
  जर(result == CURLE_TOO_LARGE)
    failf(data, "HTTP request too large");

  curlx_dyn_free(&req);
  परत result;
}

प्रकारव्याख्या गणना {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
स्थिर bool checkprefixmax(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *buffer, माप_प्रकार len)
{
  माप_प्रकार ch = CURLMIN(म_माप(prefix), len);
  परत curl_strnequal(prefix, buffer, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
स्थिर statusline checkhttpprefix(संरचना Curl_easy *data,
                                  स्थिरांक अक्षर *s, माप_प्रकार len)
{
  संरचना curl_slist *head = data->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  पर्यंत(head) {
    जर(checkprefixmax(head->data, s, len)) {
      rc = onmatch;
      थांबा;
    }
    head = head->next;
  }

  जर((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  परत rc;
}

#ifndef CURL_DISABLE_RTSP
स्थिर statusline checkrtspprefix(संरचना Curl_easy *data,
                                  स्थिरांक अक्षर *s, माप_प्रकार len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (शून्य)data;
  जर(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  परत status;
}
#endif /* CURL_DISABLE_RTSP */

स्थिर statusline checkprotoprefix(संरचना Curl_easy *data,
                                   संरचना connectdata *conn,
                                   स्थिरांक अक्षर *s, माप_प्रकार len)
{
#ifndef CURL_DISABLE_RTSP
  जर(conn->scheme->protocol & CURLPROTO_RTSP)
    परत checkrtspprefix(data, s, len);
#अन्यथा
  (शून्य)conn;
#endif /* CURL_DISABLE_RTSP */

  परत checkhttpprefix(data, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#define HD_IS(hd, hdlen, n) \
  (((hdlen) >= (आकार(n) - 1)) && curl_strnequal(n, hd, आकार(n) - 1))

#define HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (आकार(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, आकार(n) - 1)) ? ((hd) + (आकार(n) - 1)) : रिक्त)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#define HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((आकार(n) - 1) + (आकार(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
स्थिर CURLcode http_header_a(संरचना Curl_easy *data,
                              स्थिरांक अक्षर *hd, माप_प्रकार hdlen)
{
#ifndef CURL_DISABLE_ALTSVC
  स्थिरांक अक्षर *v;
  v = (data->asi &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        दो_पर्या("CURL_ALTSVC_HTTP")
#अन्यथा
        0
#endif
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : रिक्त;
  जर(v) {
    /* the ALPN of the current request */
    संरचना SingleRequest *k = &data->req;
    गणना alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    परत Curl_altsvc_parse(data, data->asi, v, data->state.origin, id);
  }
#अन्यथा
  (शून्य)data;
  (शून्य)hd;
  (शून्य)hdlen;
#endif
  परत CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
स्थिर CURLcode http_header_c(संरचना Curl_easy *data,
                              स्थिरांक अक्षर *hd, माप_प्रकार hdlen)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;
  स्थिरांक अक्षर *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !data->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : रिक्त;
  जर(v) {
    करा {
      curl_off_t contentlength;
      पूर्णांक offt = curlx_str_numblanks(&v, &contentlength);

      जर(offt == STRE_OVERFLOW) {
        /* out of range */
        जर(data->set.max_filesize) {
          failf(data, "Maximum file size exceeded");
          परत CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(data, "Overflow Content-Length: value");
        परत CURLE_OK;
      }
      अन्यथा {
        जर((offt == STRE_OK) &&
           ((k->size == -1) || /* not set to something before */
            (k->size == contentlength))) { /* or the same value */

          k->size = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          जर(!curlx_str_single(&v, ','))
            सुरूठेवा;

          जर(!curlx_str_newline(&v)) {
            k->maxdownload = k->size;
            परत CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(data, "Invalid Content-Length: value");
        परत CURLE_WEIRD_SERVER_REPLY;
      }
    } पर्यंत(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(data, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : रिक्त;
  जर(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    परत Curl_build_unencoding_चळत(data, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  जर(v) {
    अक्षर *contenttype = Curl_copy_header_मूल्य(hd);
    जर(!contenttype)
      परत CURLE_OUT_OF_MEMORY;
    जर(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    अन्यथा {
      curlx_free(data->info.contenttype);
      data->info.contenttype = contenttype;
    }
    परत CURLE_OK;
  }
  जर((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    परत CURLE_OK;
  }
  जर((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(data, "HTTP/1.0 connection set to keep alive");
    परत CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : रिक्त;
  जर(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    स्थिरांक अक्षर *ptr = v;

    /* Move forward until first digit or asterisk */
    पर्यंत(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    जर(ISDIGIT(*ptr)) {
      जर(!curlx_str_संख्या(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (data->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    अन्यथा जर(k->httpcode < 300)
      data->state.resume_from = 0; /* get everything */
  }
  परत CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
स्थिर CURLcode http_header_l(संरचना Curl_easy *data,
                              स्थिरांक अक्षर *hd, माप_प्रकार hdlen)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;
  स्थिरांक अक्षर *v = (!k->http_bodyless &&
                   (data->set.timecondition || data->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : रिक्त;
  जर(v) {
    जर(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    जर(data->set.get_filetime)
      data->info.filetime = k->timeofdoc;
    परत CURLE_OK;
  }
  जर(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    अक्षर *location = Curl_copy_header_मूल्य(hd);
    जर(!location)
      परत CURLE_OUT_OF_MEMORY;
    जर(!*location ||
       (data->req.location && !म_भेद(data->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      परत CURLE_OK;
    }
    अन्यथा {
      /* has value and is not an exact repeat */
      जर(data->req.location) {
        failf(data, "Multiple Location headers");
        curlx_free(location);
        परत CURLE_WEIRD_SERVER_REPLY;
      }
      data->req.location = location;

      जर((k->httpcode >= 300 && k->httpcode < 400) &&
         data->set.http_follow_mode) {
        CURLcode result;
        DEBUGASSERT(!data->req.newurl);
        data->req.newurl = curlx_strdup(data->req.location); /* clone */
        जर(!data->req.newurl)
          परत CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        result = http_perhapsrewind(data, conn);
        जर(result)
          परत result;

        /* mark the next request as a followed location: */
        data->state.this_is_a_follow = TRUE;
      }
    }
  }
  परत CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
स्थिर CURLcode http_header_p(संरचना Curl_easy *data,
                              स्थिरांक अक्षर *hd, माप_प्रकार hdlen)
{
  संरचना SingleRequest *k = &data->req;

#ifndef CURL_DISABLE_PROXY
  स्थिरांक अक्षर *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  जर(v) {
    संरचना connectdata *conn = data->conn;
    जर((k->httpversion == 10) && conn->http_proxy.peer &&
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
    अन्यथा जर((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(data, "HTTP/1.1 proxy connection set close");
    }
    परत CURLE_OK;
  }
#endif
  जर((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    अक्षर *auth;
    CURLcode result;
#ifndef CURL_DISABLE_PROXY
    जर(!data->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      परत CURLE_OK;
#endif
    auth = Curl_copy_header_मूल्य(hd);
    result = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    जर(!result) {
      result = Curl_http_आदान_auth(data, TRUE, auth);
      curlx_free(auth);
    }
    परत result;
  }
#ifdef USE_SPNEGO
  जर(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    संरचना connectdata *conn = data->conn;
    संरचना negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    संरचना auth *authp = &data->state.authhost;
    जर(!negdata)
      परत CURLE_OUT_OF_MEMORY;
    जर(authp->picked == CURLAUTH_NEGOTIATE) {
      अक्षर *persistentauth = Curl_copy_header_मूल्य(hd);
      जर(!persistentauth)
        परत CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(data, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#endif
  परत CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
स्थिर CURLcode http_header_r(संरचना Curl_easy *data,
                              स्थिरांक अक्षर *hd, माप_प्रकार hdlen)
{
  स्थिरांक अक्षर *v = HD_VAL(hd, hdlen, "Retry-After:");
  जर(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    समय_प्रकार date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    जर(!Curl_getdate_capped(v, &date)) {
      समय_प्रकार current = समय(रिक्त);
      जर(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    अन्यथा
      /* Try it as a decimal number, ignore errors */
      (शून्य)curlx_str_संख्या(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    जर(retry_after > 21600)
      retry_after = 21600;
    data->info.retry_after = retry_after;
  }
  परत CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
स्थिर CURLcode http_header_s(संरचना Curl_easy *data,
                              स्थिरांक अक्षर *hd, माप_प्रकार hdlen)
{
#जर !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  स्थिरांक अक्षर *v;
#अन्यथा
  (शून्य)data;
  (शून्य)hd;
  (शून्य)hdlen;
#endif

#ifndef CURL_DISABLE_COOKIES
  v = (data->cookies && data->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : रिक्त;
  जर(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    स्थिरांक अक्षर *host = data->req.cookiehost ?
      data->req.cookiehost : data->state.origin->hostname;
    स्थिरांक अचिन्हित अक्षर secure_context = Curl_secure_context(data, host) ?
      COOKIE_SECURE : 0;
    CURLcode result;
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    result = Curl_cookie_add(data, data->cookies, v, host,
                             data->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    परत result;
  }
#endif
#ifndef CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (data->hsts &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        दो_पर्या("CURL_HSTS_HTTP")
#अन्यथा
        0
#endif
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : रिक्त;
  जर(v) {
    CURLcode result = Curl_hsts_parse(
      data->hsts, data->state.origin->hostname, v);
    जर(result) {
      जर(result == CURLE_OUT_OF_MEMORY)
        परत result;
      infof(data, "Illegal STS header skipped");
    }
#ifdef DEBUGBUILD
    अन्यथा
      infof(data, "Parsed STS header fine (%zu entries)",
            Curl_llist_मोजणी(&data->hsts->list));
#endif
  }
#endif

  परत CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
स्थिर CURLcode http_header_t(संरचना Curl_easy *data,
                              स्थिरांक अक्षर *hd, माप_प्रकार hdlen)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;

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
  स्थिरांक अक्षर *v = (!k->http_bodyless &&
                   (data->state.httpreq != HTTPREQ_HEAD) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : रिक्त;
  जर(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode result = Curl_build_unencoding_चळत(data, v, TRUE);
    जर(result)
      परत result;
    जर(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(data, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    परत CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  जर(v) {
    data->req.resp_trailer = TRUE;
    परत CURLE_OK;
  }
  परत CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
स्थिर CURLcode http_header_w(संरचना Curl_easy *data,
                              स्थिरांक अक्षर *hd, माप_प्रकार hdlen)
{
  संरचना SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  जर((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    अक्षर *auth = Curl_copy_header_मूल्य(hd);
    जर(!auth)
      result = CURLE_OUT_OF_MEMORY;
    अन्यथा {
      result = Curl_http_आदान_auth(data, FALSE, auth);
      curlx_free(auth);
    }
  }
  परत result;
}

/*
 * http_header() parses a single response header.
 */
स्थिर CURLcode http_header(संरचना Curl_easy *data,
                            स्थिरांक अक्षर *hd, माप_प्रकार hdlen)
{
  CURLcode result = CURLE_OK;

  निवड(hd[0]) {
  स्थिती 'a':
  स्थिती 'A':
    result = http_header_a(data, hd, hdlen);
    थांबा;
  स्थिती 'c':
  स्थिती 'C':
    result = http_header_c(data, hd, hdlen);
    थांबा;
  स्थिती 'l':
  स्थिती 'L':
    result = http_header_l(data, hd, hdlen);
    थांबा;
  स्थिती 'p':
  स्थिती 'P':
    result = http_header_p(data, hd, hdlen);
    थांबा;
  स्थिती 'r':
  स्थिती 'R':
    result = http_header_r(data, hd, hdlen);
    थांबा;
  स्थिती 's':
  स्थिती 'S':
    result = http_header_s(data, hd, hdlen);
    थांबा;
  स्थिती 't':
  स्थिती 'T':
    result = http_header_t(data, hd, hdlen);
    थांबा;
  स्थिती 'w':
  स्थिती 'W':
    result = http_header_w(data, hd, hdlen);
    थांबा;
  }

  जर(!result) {
    संरचना connectdata *conn = data->conn;
    जर(conn->scheme->protocol & CURLPROTO_RTSP)
      result = Curl_rtsp_parseheader(data, hd);
  }
  परत result;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
स्थिर CURLcode http_statusline(संरचना Curl_easy *data,
                                संरचना connectdata *conn)
{
  संरचना SingleRequest *k = &data->req;

  निवड(k->httpversion) {
  स्थिती 10:
  स्थिती 11:
#ifdef USE_HTTP2
  स्थिती 20:
#endif
#ifdef USE_HTTP3
  स्थिती 30:
#endif
    /* no major version switch mid-connection */
    जर(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(data, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      परत CURLE_WEIRD_SERVER_REPLY;
    }
    थांबा;
  पूर्वनिर्धारित:
    failf(data, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    परत CURLE_UNSUPPORTED_PROTOCOL;
  }

  data->info.httpcode = k->httpcode;
  data->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  जर(!data->state.http_neg.rcvd_min ||
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
  जर(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  जर(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(data, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  निवड(k->httpcode) {
  स्थिती 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    जर(data->set.timecondition)
      data->info.timecond = TRUE;
    FALLTHROUGH();
  स्थिती 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->size = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    थांबा;
  पूर्वनिर्धारित:
    थांबा;
  }
  परत CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
स्थिर CURLcode http_size(संरचना Curl_easy *data)
{
  संरचना SingleRequest *k = &data->req;
  जर(data->req.ignore_cl || k->chunk) {
    k->size = k->maxdownload = -1;
  }
  अन्यथा जर(k->size != -1) {
    जर(data->set.max_filesize &&
       !k->ignorebody &&
       (k->size > data->set.max_filesize)) {
      failf(data, "Maximum file size exceeded");
      परत CURLE_FILESIZE_EXCEEDED;
    }
    जर(k->ignorebody)
      infof(data, "setting size while ignoring");
    Curl_pgrsSetDownloadSize(data, k->size);
    k->maxdownload = k->size;
  }
  परत CURLE_OK;
}

CURLcode Curl_verify_header(संरचना Curl_easy *data,
                            स्थिरांक अक्षर *hd, माप_प्रकार hdlen)
{
  संरचना SingleRequest *k = &data->req;
  माप_प्रकार i;
  bool has_colon = FALSE;
  साठी(i = 0; i < hdlen; i++) {
    निवड(hd[i]) {
    स्थिती '\0':
      /* this is bad, bail out */
      failf(data, "NUL byte in header");
      परत CURLE_WEIRD_SERVER_REPLY;
    स्थिती '\r':
      जर(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(data, "Carriage return found in header");
        परत CURLE_WEIRD_SERVER_REPLY;
      }
      थांबा;
    स्थिती ':':
      has_colon = TRUE;
      थांबा;
    }
  }
  जर(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    परत CURLE_OK;
  जर(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  अन्यथा जर(!has_colon) {
    /* this is bad, bail out */
    failf(data, "Header without colon");
    परत CURLE_WEIRD_SERVER_REPLY;
  }
  परत CURLE_OK;
}

CURLcode Curl_bump_headersize(संरचना Curl_easy *data,
                              माप_प्रकार delta,
                              bool connect_only)
{
  माप_प्रकार bad = 0;
  अचिन्हित पूर्णांक max = MAX_HTTP_RESP_HEADER_SIZE;
  जर(delta < MAX_HTTP_RESP_HEADER_SIZE) {
    data->info.header_size += (अचिन्हित पूर्णांक)delta;
    data->req.allheadercount += (अचिन्हित पूर्णांक)delta;
    जर(!connect_only)
      data->req.headerbytecount += (अचिन्हित पूर्णांक)delta;
    जर(data->req.allheadercount > max)
      bad = data->req.allheadercount;
    अन्यथा जर(data->info.header_size > (max * 20)) {
      bad = data->info.header_size;
      max *= 20;
    }
  }
  अन्यथा
    bad = data->req.allheadercount + delta;
  जर(bad) {
    failf(data, "Too large response headers: %zu > %u", bad, max);
    परत CURLE_RECV_ERROR;
  }
  परत CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
स्थिर CURLcode http_on_101_upgrade(संरचना Curl_easy *data,
                                    स्थिरांक अक्षर *buf, माप_प्रकार blen,
                                    माप_प्रकार *pconsumed,
                                    bool *conn_changed)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;

#जर !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (शून्य)buf;
  (शून्य)blen;
  (शून्य)pconsumed;
#अन्यथा
  CURLcode result;
  पूर्णांक upgr101_requested = k->upgr101;
#endif

  जर(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(data, "server sent 101 response while not talking HTTP/1.1");
    परत CURLE_WEIRD_SERVER_REPLY;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->bits.upgrade_in_progress = FALSE;
  *conn_changed = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ifdef USE_NGHTTP2
  जर(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(data, "Received 101, Switching to HTTP/2");
    result = Curl_http2_upgrade(data, conn, FIRSTSOCKET, buf, blen);
    जर(!result)
      *pconsumed += blen;
    परत result;
  }
#endif
#ifndef CURL_DISABLE_WEBSOCKETS
  जर(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(data, "Received 101, Switching to WebSocket");
    result = Curl_ws_accept(data, buf, blen);
    जर(!result)
      *pconsumed += blen; /* ws accept handled the data */
    परत result;
  }
#endif
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  परत CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
स्थिर CURLcode http_on_1xx_response(संरचना Curl_easy *data,
                                     स्थिरांक अक्षर *buf, माप_प्रकार blen,
                                     माप_प्रकार *pconsumed,
                                     bool *conn_changed)
{
  संरचना SingleRequest *k = &data->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  निवड(k->httpcode) {
  स्थिती 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(data);
    थांबा;
  स्थिती 101:
    परत http_on_101_upgrade(data, buf, blen, pconsumed, conn_changed);
  पूर्वनिर्धारित:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    थांबा;
  }
  परत CURLE_OK;
}

#जर defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
स्थिर शून्य http_check_auth_closure(संरचना Curl_easy *data,
                                    संरचना connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ifdef USE_NTLM
  जर(conn->bits.बंद &&
     (((data->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((data->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
#endif
#ifdef USE_SPNEGO
  जर(conn->bits.बंद &&
    (((data->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((data->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
  जर((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  जर((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#endif
}
#अन्यथा
#define http_check_auth_closure(x, y) /* empty */
#endif

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
स्थिर CURLcode http_handle_send_error(संरचना Curl_easy *data)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  जर(!data->req.authneg && !conn->bits.बंद &&
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

    निवड(data->state.httpreq) {
    स्थिती HTTPREQ_PUT:
    स्थिती HTTPREQ_POST:
    स्थिती HTTPREQ_POST_FORM:
    स्थिती HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      जर(!Curl_req_done_sending(data)) {
        जर((k->httpcode == 417) && http_exp100_is_selected(data)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          जर(!k->writebytecount && http_exp100_is_waiting(data)) {
            infof(data, "Got HTTP failure 417 while waiting for a 100");
          }
          अन्यथा {
            infof(data, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            result = http_perhapsrewind(data, conn);
            जर(result)
              परत result;
          }
          data->state.disableexpect = TRUE;
          Curl_req_abort_sending(data);
          DEBUGASSERT(!data->req.newurl);
          data->req.newurl = Curl_bufref_dup(&data->state.url);
          जर(!data->req.newurl)
            परत CURLE_OUT_OF_MEMORY;
        }
        अन्यथा जर(data->set.http_keep_sending_on_error) {
          infof(data, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(data);
        }
        अन्यथा {
          infof(data, "HTTP error before end of send, stop sending");
          streamclose(conn);
          result = Curl_req_abort_sending(data);
          जर(result)
            परत result;
        }
      }
      थांबा;

    पूर्वनिर्धारित: /* default label present to avoid compiler warnings */
      थांबा;
    }
  }

  जर(Curl_creader_will_rewind(data) && !Curl_req_done_sending(data)) {
    /* We rewind before next send, continue sending now */
    infof(data, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(data);
  }
  परत result;
}

स्थिर CURLcode http_on_response(संरचना Curl_easy *data,
                                 स्थिरांक अक्षर *last_hd, माप_प्रकार last_hd_len,
                                 स्थिरांक अक्षर *buf, माप_प्रकार blen,
                                 माप_प्रकार *pconsumed)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  संरचना SingleRequest *k = &data->req;
  bool conn_changed = FALSE;

  (शून्य)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  जर(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    जर(data->req.httpversion != 20)
      infof(data, "Lying server, not serving HTTP/2");
  }

  जर(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    result = http_write_header(data, last_hd, last_hd_len);
    last_hd = रिक्त; /* handled it */
    जर(result)
      जा out;
  }

  जर(k->httpcode < 100) {
    failf(data, "Unsupported response code in HTTP response");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    जा out;
  }
  अन्यथा जर(k->httpcode < 200) {
    result = http_on_1xx_response(data, buf, blen, pconsumed, &conn_changed);
    जा out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  जर(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  जर((k->size == -1) && !k->chunk && !conn->bits.बंद &&
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
  जर(data->req.upgr101 == UPGR101_WS) {
    failf(data, "Refused WebSocket upgrade: %d", k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    जा out;
  }
#endif

  /* Check if this response means the transfer errored. */
  जर(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    जा out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  result = Curl_http_auth_act(data);
  जर(result)
    जा out;

  जर(k->httpcode >= 300) {
    result = http_handle_send_error(data);
    जर(result)
      जा out;
  }

  /* final response without error, prepare to receive the body */
  result = http_firstwrite(data);
  जर(result)
    जा out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  result = http_size(data);
  जर(result)
    जा out;

#ifndef CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  जर((conn->scheme->protocol & CURLPROTO_RTSP) && data->req.no_body &&
     !data->set.opt_no_body && (k->size > 0))
    data->req.no_body = FALSE;
#endif

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  जर(data->req.no_body)
    k->download_done = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  जर((k->maxdownload == 0) && (k->httpversion_sent < 20))
    k->download_done = TRUE;

out:
  जर(last_hd)
    /* if not written yet, write it now */
    result = Curl_1st_fatal(result,
                            http_write_header(data, last_hd, last_hd_len));
  जर(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(data->multi);
  परत result;
}

स्थिर CURLcode http_rw_hd(संरचना Curl_easy *data,
                           स्थिरांक अक्षर *hd, माप_प्रकार hdlen,
                           स्थिरांक अक्षर *buf_remain, माप_प्रकार blen,
                           माप_प्रकार *pconsumed)
{
  CURLcode result = CURLE_OK;
  संरचना SingleRequest *k = &data->req;
  पूर्णांक writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  जर((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    संरचना dynbuf last_header;
    माप_प्रकार consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    result = curlx_dyn_addn(&last_header, hd, hdlen);
    जर(result)
      परत result;

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
    परत result;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_HEADER;
  जर(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    जर(data->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      स्थिरांक अक्षर *p = hd;

      curlx_str_passblanks(&p);
      जर(!म_भेदन(p, "HTTP/", 5)) {
        p += 5;
        निवड(*p) {
        स्थिती '1':
          p++;
          जर((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            जर(ISBLANK(p[2])) {
              k->httpversion = (अचिन्हित अक्षर)(10 + (p[1] - '0'));
              p += 3;
              जर(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          जर(!fine_statusline) {
            failf(data, "Unsupported HTTP/1 subversion in response");
            परत CURLE_UNSUPPORTED_PROTOCOL;
          }
          थांबा;
        स्थिती '2':
        स्थिती '3':
          जर(!ISBLANK(p[1]))
            थांबा;
          k->httpversion = (अचिन्हित अक्षर)((*p - '0') * 10);
          p += 2;
          जर(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            जर(!ISBLANK(*p))
              थांबा;
            fine_statusline = TRUE;
          }
          थांबा;
        पूर्वनिर्धारित: /* unsupported */
          failf(data, "Unsupported HTTP version in response");
          परत CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      जर(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(data, hd, hdlen);
        जर(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    अन्यथा जर(data->conn->scheme->protocol & CURLPROTO_RTSP) {
      स्थिरांक अक्षर *p = hd;
      संरचना Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      जर(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_संख्या(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        जर(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (पूर्णांक)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      जर(!fine_statusline)
        परत CURLE_WEIRD_SERVER_REPLY;
    }

    जर(fine_statusline) {
      result = http_statusline(data, data->conn);
      जर(result)
        परत result;
      writetype |= CLIENTWRITE_STATUS;
    }
    अन्यथा {
      k->header = FALSE;   /* this is not a header line */
      परत CURLE_WEIRD_SERVER_REPLY;
    }
  }

  result = Curl_verify_header(data, hd, hdlen);
  जर(result)
    परत result;

  result = http_header(data, hd, hdlen);
  जर(result)
    परत result;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  जर(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  result = Curl_client_write(data, writetype, hd, hdlen);
  जर(result)
    परत result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  जर(result)
    परत result;

  परत CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
शून्य Curl_http_to_fold(संरचना dynbuf *bf)
{
  माप_प्रकार len = curlx_dyn_len(bf);
  स्थिरांक अक्षर *hd = curlx_dyn_ptr(bf);
  जर(len && (hd[len - 1] == '\n'))
    len--;
  जर(len && (hd[len - 1] == '\r'))
    len--;
  पर्यंत(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

स्थिर शून्य unfold_header(संरचना Curl_easy *data)
{
  Curl_http_to_fold(&data->state.headerb);
  data->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
स्थिर CURLcode http_parse_headers(संरचना Curl_easy *data,
                                   स्थिरांक अक्षर *buf, माप_प्रकार blen,
                                   माप_प्रकार *pconsumed)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  संरचना SingleRequest *k = &data->req;
  स्थिरांक अक्षर *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  जर(data->state.maybe_folded && blen) {
    जर(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(data);
    }
    अन्यथा {
      /* the header data we hold is a complete header, pass it on */
      माप_प्रकार ignore_this;
      result = http_rw_hd(data, curlx_dyn_ptr(&data->state.headerb),
                          curlx_dyn_len(&data->state.headerb),
                          रिक्त, 0, &ignore_this);
      curlx_dyn_reset(&data->state.headerb);
      जर(result)
        परत result;
    }
    data->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  पर्यंत(blen && k->header) {
    माप_प्रकार consumed;
    माप_प्रकार hlen;
    स्थिरांक अक्षर *hd;
    माप_प्रकार unfold_len = 0;

    जर(data->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      पर्यंत(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      जर(blen) {
        /* insert a single space */
        result = curlx_dyn_addn(&data->state.headerb, " ", 1);
        जर(result)
          परत result;
        data->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = स_प्रथम(buf, '\n', blen);
    जर(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      result = curlx_dyn_addn(&data->state.headerb, buf, blen);
      जर(result)
        परत result;
      *pconsumed += blen + unfold_len;

      जर(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(data, conn,
                           curlx_dyn_ptr(&data->state.headerb),
                           curlx_dyn_len(&data->state.headerb));

        जर(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          जर((k->httpversion >= 10) || conn->bits.reuse) {
            failf(data, "Invalid status line");
            परत CURLE_WEIRD_SERVER_REPLY;
          }
          जर(!data->state.http_neg.accept_09) {
            failf(data, "Received HTTP/0.9 when not allowed");
            परत CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          जा out;
        }
      }
      जा out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    result = curlx_dyn_addn(&data->state.headerb, buf, consumed);
    जर(result)
      परत result;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&data->state.headerb);
    hd = curlx_dyn_ptr(&data->state.headerb);

    जर(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(data, conn, hd, hlen);
      जर(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        जर((k->httpversion >= 10) || conn->bits.reuse) {
          failf(data, "Invalid status line");
          परत CURLE_WEIRD_SERVER_REPLY;
        }
        जर(!data->state.http_neg.accept_09) {
          failf(data, "Received HTTP/0.9 when not allowed");
          परत CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        जा out;
      }
    }
    अन्यथा {
      जर(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        जर(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(data);
          सुरूठेवा;
        }
        अन्यथा जर(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          data->state.maybe_folded = TRUE;
          थांबा;
        }
      }
    }

    result = http_rw_hd(data, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&data->state.headerb);
    जर(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    जर(result)
      परत result;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  जर(!k->header && !leftover_body) {
    curlx_dyn_free(&data->state.headerb);
  }
  परत CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(संरचना Curl_easy *data,
                                 स्थिरांक अक्षर *hd, माप_प्रकार hdlen,
                                 bool is_eos)
{
  CURLcode result;
  माप_प्रकार consumed;
  अक्षर tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  result = http_rw_hd(data, hd, hdlen, &tmp, 0, &consumed);
  जर(!result && is_eos) {
    result = Curl_client_write(data, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  परत result;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(संरचना Curl_easy *data,
                                  स्थिरांक अक्षर *buf, माप_प्रकार blen,
                                  माप_प्रकार *pconsumed)
{
  जर(!data->req.header) {
    *pconsumed = 0;
    परत CURLE_OK;
  }
  अन्यथा {
    CURLcode result;

    result = http_parse_headers(data, buf, blen, pconsumed);
    जर(!result && !data->req.header) {
      जर(!data->req.no_body && curlx_dyn_len(&data->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&data->state.headerb),
                                   curlx_dyn_len(&data->state.headerb));
      }
      curlx_dyn_free(&data->state.headerb);
    }
    परत result;
  }
}

CURLcode Curl_http_write_resp(संरचना Curl_easy *data,
                              स्थिरांक अक्षर *buf, माप_प्रकार blen,
                              bool is_eos)
{
  CURLcode result;
  माप_प्रकार consumed;
  पूर्णांक flags;

  result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
  जर(result || data->req.done)
    जा out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !data->req.header);
  जर(!data->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    जर(is_eos)
      flags |= CLIENTWRITE_EOS;
    result = Curl_client_write(data, flags, buf, blen);
  }
out:
  परत result;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, स्थिरांक अक्षर *s, माप_प्रकार len)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  पूर्णांक i;

  जर(len != 3)
    जा out;

  साठी(i = 0; i < 3; ++i) {
    अक्षर c = s[i];

    जर(c < '0' || c > '9')
      जा out;

    status *= 10;
    status += c - '0';
  }
  result = CURLE_OK;
out:
  *pstatus = result ? -1 : status;
  परत result;
}

CURLcode Curl_http_req_make(संरचना httpreq **preq,
                            स्थिरांक अक्षर *method, माप_प्रकार m_len,
                            स्थिरांक अक्षर *scheme, माप_प्रकार s_len,
                            स्थिरांक अक्षर *authority, माप_प्रकार a_len,
                            स्थिरांक अक्षर *path, माप_प्रकार p_len)
{
  संरचना httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, आकार(*req) + m_len);
  जर(!req)
    जा out;
#जर defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ढकला
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  स_नकल(req->method, method, m_len);
#जर defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic काढा
#endif
  जर(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    जर(!req->scheme)
      जा out;
  }
  जर(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    जर(!req->authority)
      जा out;
  }
  जर(path) {
    req->path = curlx_memdup0(path, p_len);
    जर(!req->path)
      जा out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  जर(result && req)
    Curl_http_req_free(req);
  *preq = result ? रिक्त : req;
  परत result;
}

स्थिर CURLcode req_assign_url_authority(संरचना httpreq *req, CURLU *url)
{
  अक्षर *host, *port;
  संरचना dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  host = port = रिक्त;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  जर(uc && uc != CURLUE_NO_HOST)
    जा out;
  जर(!host) {
    req->authority = रिक्त;
    result = CURLE_OK;
    जा out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  जर(uc && uc != CURLUE_NO_PORT)
    जा out;

  result = curlx_dyn_add(&buf, host);
  जर(result)
    जा out;
  जर(port) {
    result = curlx_dyn_addf(&buf, ":%s", port);
    जर(result)
      जा out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  जर(result)
    curlx_dyn_free(&buf);
  परत result;
}

स्थिर CURLcode req_assign_url_path(संरचना httpreq *req, CURLU *url)
{
  अक्षर *path, *query;
  संरचना dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  path = query = रिक्त;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  जर(uc)
    जा out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  जर(uc && uc != CURLUE_NO_QUERY)
    जा out;

  जर(!query) {
    req->path = path;
    path = रिक्त;
  }
  अन्यथा {
    result = curlx_dyn_add(&buf, path);
    जर(!result)
      result = curlx_dyn_addf(&buf, "?%s", query);
    जर(result)
      जा out;
    req->path = curlx_dyn_ptr(&buf);
  }
  result = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  जर(result)
    curlx_dyn_free(&buf);
  परत result;
}

CURLcode Curl_http_req_make2(संरचना httpreq **preq,
                             स्थिरांक अक्षर *method, माप_प्रकार m_len,
                             CURLU *url, स्थिरांक अक्षर *scheme_default)
{
  संरचना httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, आकार(*req) + m_len);
  जर(!req)
    जा out;
  स_नकल(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  जर(uc && uc != CURLUE_NO_SCHEME)
    जा out;
  जर(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    जर(!req->scheme)
      जा out;
  }

  result = req_assign_url_authority(req, url);
  जर(result)
    जा out;
  result = req_assign_url_path(req, url);
  जर(result)
    जा out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  जर(result && req)
    Curl_http_req_free(req);
  *preq = result ? रिक्त : req;
  परत result;
}

शून्य Curl_http_req_free(संरचना httpreq *req)
{
  जर(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->path);
    Curl_dynhds_free(&req->headers);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

संरचना name_const {
  स्थिरांक अक्षर *name;
  माप_प्रकार namelen;
};

स्थिर स्थिरांक संरचना name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

स्थिर bool h2_permissible_field(संरचना dynhds_entry *e)
{
  माप_प्रकार i;
  साठी(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    जर(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      परत FALSE;
  }
  परत TRUE;
}

स्थिर bool http_TE_has_token(स्थिरांक अक्षर *fvalue, स्थिरांक अक्षर *token)
{
  पर्यंत(*fvalue) {
    संरचना Curl_str name;

    /* skip to first token */
    पर्यंत(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    जर(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      परत FALSE;
    जर(curlx_str_casecompare(&name, token))
      परत TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    पर्यंत(*fvalue && *fvalue != ',') {
      जर(*fvalue == '"') {
        संरचना Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        जर(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          परत FALSE;
      }
      अन्यथा
        fvalue++;
    }
  }
  परत FALSE;
}

CURLcode Curl_http_req_to_h2(संरचना dynhds *h2_headers,
                             संरचना httpreq *req, संरचना Curl_easy *data)
{
  स्थिरांक अक्षर *scheme = रिक्त, *authority = रिक्त;
  संरचना dynhds_entry *e;
  माप_प्रकार i;
  CURLcode result;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  जर(req->scheme) {
    scheme = req->scheme;
  }
  अन्यथा जर(म_भेद("CONNECT", req->method)) {
    scheme = Curl_checkheaders(data, STRCONST(HTTP_PSEUDO_SCHEME));
    जर(scheme) {
      scheme += आकार(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(data, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    अन्यथा {
      scheme = data->state.origin->scheme->name;
    }
  }

  जर(req->authority) {
    authority = req->authority;
  }
  अन्यथा {
    e = Curl_dynhds_get(&req->headers, STRCONST("Host"));
    जर(e)
      authority = e->मूल्य;
  }

  Curl_dynhds_reset(h2_headers);
  Curl_dynhds_set_opts(h2_headers, DYNHDS_OPT_LOWERCASE);
  result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, म_माप(req->method));
  जर(!result && scheme) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, म_माप(scheme));
  }
  जर(!result && authority) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, म_माप(authority));
  }
  जर(!result && req->path) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, म_माप(req->path));
  }
  साठी(i = 0; !result && i < Curl_dynhds_मोजणी(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    जर(e->namelen == 2 && curl_strequal("TE", e->name)) {
      जर(http_TE_has_token(e->मूल्य, "trailers"))
        result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    अन्यथा जर(h2_permissible_field(e)) {
      result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->मूल्य, e->valuelen);
    }
  }

  परत result;
}

CURLcode Curl_http_resp_make(संरचना http_resp **presp,
                             पूर्णांक status,
                             स्थिरांक अक्षर *description)
{
  संरचना http_resp *resp;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, आकार(*resp));
  जर(!resp)
    जा out;

  resp->status = status;
  जर(description) {
    resp->description = curlx_strdup(description);
    जर(!resp->description)
      जा out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  जर(result && resp)
    Curl_http_resp_free(resp);
  *presp = result ? रिक्त : resp;
  परत result;
}

शून्य Curl_http_resp_free(संरचना http_resp *resp)
{
  जर(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    जर(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
स्थिरांक संरचना Curl_protocol Curl_protocol_http = {
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
